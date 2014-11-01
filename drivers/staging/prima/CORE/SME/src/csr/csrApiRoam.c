/*
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
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
  
  
    \file csrApiRoam.c
  
    Implementation for the Common Roaming interfaces.
  
    Copyright (C) 2008 Qualcomm, Incorporated
  
 
   ========================================================================== */
/*                                                                           
                                           

                                                                       
                                                                

                                                      
                                                                                            
                                                                    
                                                               
                                                                           */
#include "aniGlobal.h" //                  
#include "wlan_qct_wda.h"
#include "halMsgApi.h" //                        
#include "limUtils.h"
#include "palApi.h"
#include "csrInsideApi.h"
#include "smsDebug.h"
#include "logDump.h"
#include "smeQosInternal.h"
#include "wlan_qct_tl.h"
#include "smeInside.h"
#include "vos_diag_core_event.h"
#include "vos_diag_core_log.h"
#include "csrApi.h"
#include "pmc.h"
#include "vos_nvitem.h"
#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
#include "csrNeighborRoam.h"
#endif /*                               */
#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
#include "csrCcx.h"
#endif /*                                              */
#define CSR_NUM_IBSS_START_CHANNELS_50      4
#define CSR_NUM_IBSS_START_CHANNELS_24      3
#define CSR_DEF_IBSS_START_CHANNEL_50       36
#define CSR_DEF_IBSS_START_CHANNEL_24       1
#define CSR_IBSS_JOIN_TIMEOUT_PERIOD        ( 1 *  PAL_TIMER_TO_SEC_UNIT )  //         
#define CSR_WAIT_FOR_KEY_TIMEOUT_PERIOD         ( 5 * PAL_TIMER_TO_SEC_UNIT )  //                               
#define CSR_WAIT_FOR_WPS_KEY_TIMEOUT_PERIOD         ( 120 * PAL_TIMER_TO_SEC_UNIT )  //                     
/*                                                                           
                                      
                                                                           */
#define CSR_VCC_UL_MAC_LOSS_THRESHOLD 9
/*                                                                           
                           
                                                                           */
#define CSR_VCC_RSSI_THRESHOLD 80
#define CSR_MIN_GLOBAL_STAT_QUERY_PERIOD   500 //  
#define CSR_MIN_GLOBAL_STAT_QUERY_PERIOD_IN_BMPS 2000 //  
#define CSR_MIN_TL_STAT_QUERY_PERIOD       500 //  
#define CSR_DIAG_LOG_STAT_PERIOD           3000 //  
//                       
//                                                                                         
//                                            
//                                            
#define SME_DETECT_AC_WEIGHT_DIFF(loAC, hiAC)   (v_BOOL_t)(((hiAC) > (loAC)) ? (((hiAC)-(loAC)) > 4) : 0)
//                                                    
#define CSR_DONT_SEND_DISASSOC_OVER_THE_AIR 1
#define RSSI_HACK_BMPS (-40)
#define MAX_CB_VALUE_IN_INI (2)

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
static tANI_BOOLEAN bRoamScanOffloadStarted = VOS_FALSE;
#endif

/*                                                                           
                          
                                                                          */
static tCsrRoamSession csrRoamRoamSession[CSR_ROAM_SESSION_MAX];

/*                                                                           
                   
                                                                          */
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
int diagAuthTypeFromCSRType(eCsrAuthType authType)
{
    int n = AUTH_OPEN;
    switch(authType)
    {
    case eCSR_AUTH_TYPE_SHARED_KEY:
        n = AUTH_SHARED;
        break;
    case eCSR_AUTH_TYPE_WPA:
        n = AUTH_WPA_EAP;
        break;
    case eCSR_AUTH_TYPE_WPA_PSK:
        n = AUTH_WPA_PSK;
        break;
    case eCSR_AUTH_TYPE_RSN:
        n = AUTH_WPA2_EAP;
        break;
    case eCSR_AUTH_TYPE_RSN_PSK:
#ifdef WLAN_FEATURE_11W
    case eCSR_AUTH_TYPE_RSN_PSK_SHA256:
#endif
        n = AUTH_WPA2_PSK;
        break;
#ifdef FEATURE_WLAN_WAPI
    case eCSR_AUTH_TYPE_WAPI_WAI_CERTIFICATE:
        n = AUTH_WAPI_CERT;
        break;
    case eCSR_AUTH_TYPE_WAPI_WAI_PSK:
        n = AUTH_WAPI_PSK;
        break;
#endif /*                   */
    default:
        break;
    }
    return (n);
}
int diagEncTypeFromCSRType(eCsrEncryptionType encType)
{
    int n = ENC_MODE_OPEN;
    switch(encType)
    {
    case eCSR_ENCRYPT_TYPE_WEP40_STATICKEY:
    case eCSR_ENCRYPT_TYPE_WEP40:
        n = ENC_MODE_WEP40;
        break;
    case eCSR_ENCRYPT_TYPE_WEP104_STATICKEY:
    case eCSR_ENCRYPT_TYPE_WEP104:
        n = ENC_MODE_WEP104;
        break;
    case eCSR_ENCRYPT_TYPE_TKIP:
        n = ENC_MODE_TKIP;
        break;
    case eCSR_ENCRYPT_TYPE_AES:
        n = ENC_MODE_AES;
        break;
#ifdef FEATURE_WLAN_WAPI
    case eCSR_ENCRYPT_TYPE_WPI:
        n = ENC_MODE_SMS4;
        break;
#endif /*                   */
    default:
        break;
    }
    return (n);
}
#endif //                                    
static const tANI_U8 csrStartIbssChannels50[ CSR_NUM_IBSS_START_CHANNELS_50 ] = { 36, 40,  44,  48}; 
static const tANI_U8 csrStartIbssChannels24[ CSR_NUM_IBSS_START_CHANNELS_24 ] = { 1, 6, 11 };
static void initConfigParam(tpAniSirGlobal pMac);
static tANI_BOOLEAN csrRoamProcessResults( tpAniSirGlobal pMac, tSmeCmd *pCommand,
                                       eCsrRoamCompleteResult Result, void *Context );
static eHalStatus csrRoamStartIbss( tpAniSirGlobal pMac, tANI_U32 sessionId, 
                                    tCsrRoamProfile *pProfile, 
                                    tANI_BOOLEAN *pfSameIbss );
static void csrRoamUpdateConnectedProfileFromNewBss( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirSmeNewBssInfo *pNewBss );
static void csrRoamPrepareBssParams(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, 
                                     tSirBssDescription *pBssDesc, tBssConfigParam *pBssConfig, tDot11fBeaconIEs *pIes);
static ePhyChanBondState csrGetCBModeFromIes(tpAniSirGlobal pMac, tANI_U8 primaryChn, tDot11fBeaconIEs *pIes);
eHalStatus csrInitGetChannels(tpAniSirGlobal pMac);
static void csrRoamingStateConfigCnfProcessor( tpAniSirGlobal pMac, tANI_U32 result );
eHalStatus csrRoamOpen(tpAniSirGlobal pMac);
eHalStatus csrRoamClose(tpAniSirGlobal pMac);
void csrRoamMICErrorTimerHandler(void *pv);
void csrRoamTKIPCounterMeasureTimerHandler(void *pv);
tANI_BOOLEAN csrRoamIsSameProfileKeys(tpAniSirGlobal pMac, tCsrRoamConnectedProfile *pConnProfile, tCsrRoamProfile *pProfile2);
 
static eHalStatus csrRoamStartRoamingTimer(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 interval);
static eHalStatus csrRoamStopRoamingTimer(tpAniSirGlobal pMac, tANI_U32 sessionId);
static void csrRoamRoamingTimerHandler(void *pv);
eHalStatus csrRoamStartIbssJoinTimer(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 interval);
eHalStatus csrRoamStopIbssJoinTimer(tpAniSirGlobal pMac, tANI_U32 sessionId);
static void csrRoamIbssJoinTimerHandler(void *pv);
eHalStatus csrRoamStartWaitForKeyTimer(tpAniSirGlobal pMac, tANI_U32 interval);
eHalStatus csrRoamStopWaitForKeyTimer(tpAniSirGlobal pMac);
static void csrRoamWaitForKeyTimeOutHandler(void *pv);
 
static eHalStatus CsrInit11dInfo(tpAniSirGlobal pMac, tCsr11dinfo *ps11dinfo);
static eHalStatus csrInitChannelPowerList( tpAniSirGlobal pMac, tCsr11dinfo *ps11dinfo);
static eHalStatus csrRoamFreeConnectedInfo( tpAniSirGlobal pMac, tCsrRoamConnectedInfo *pConnectedInfo );
eHalStatus csrSendMBSetContextReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId, 
           tSirMacAddr peerMacAddr, tANI_U8 numKeys, tAniEdType edType, 
           tANI_BOOLEAN fUnicast, tAniKeyDirection aniKeyDirection,
           tANI_U8 keyId, tANI_U8 keyLength, tANI_U8 *pKey, tANI_U8 paeRole, 
           tANI_U8 *pKeyRsc );
static eHalStatus csrRoamIssueReassociate( tpAniSirGlobal pMac, tANI_U32 sessionId, 
                                    tSirBssDescription *pSirBssDesc, tDot11fBeaconIEs *pIes, 
                                    tCsrRoamProfile *pProfile );
void csrRoamStatisticsTimerHandler(void *pv);
void csrRoamStatsGlobalClassDTimerHandler(void *pv);
static void csrRoamLinkUp(tpAniSirGlobal pMac, tCsrBssid bssid);
VOS_STATUS csrRoamVccTriggerRssiIndCallback(tHalHandle hHal, 
                                            v_U8_t  rssiNotification, 
                                            void * context);
static void csrRoamLinkDown(tpAniSirGlobal pMac, tANI_U32 sessionId);
void csrRoamVccTrigger(tpAniSirGlobal pMac);
eHalStatus csrSendMBStatsReqMsg( tpAniSirGlobal pMac, tANI_U32 statsMask, tANI_U8 staId);
/*
                                                                    
*/
static void csrRoamRemoveStatListEntry(tpAniSirGlobal pMac, tListElem *pEntry);
static eCsrCfgDot11Mode csrRoamGetPhyModeBandForBss( tpAniSirGlobal pMac, tCsrRoamProfile *pProfile,tANI_U8 operationChn, eCsrBand *pBand );
static eHalStatus csrRoamGetQosInfoFromBss(tpAniSirGlobal pMac, tSirBssDescription *pBssDesc);
tCsrStatsClientReqInfo * csrRoamInsertEntryIntoList( tpAniSirGlobal pMac,
                                                     tDblLinkList *pStaList,
                                                     tCsrStatsClientReqInfo *pStaEntry);
void csrRoamStatsClientTimerHandler(void *pv);
tCsrPeStatsReqInfo *  csrRoamCheckPeStatsReqList(tpAniSirGlobal pMac, tANI_U32  statsMask, 
                                                 tANI_U32 periodicity, tANI_BOOLEAN *pFound, tANI_U8 staId);
void csrRoamReportStatistics(tpAniSirGlobal pMac, tANI_U32 statsMask, 
                             tCsrStatsCallback callback, tANI_U8 staId, void *pContext);
void csrRoamSaveStatsFromTl(tpAniSirGlobal pMac, WLANTL_TRANSFER_STA_TYPE *pTlStats);
void csrRoamTlStatsTimerHandler(void *pv);
void csrRoamPeStatsTimerHandler(void *pv);
tListElem * csrRoamCheckClientReqList(tpAniSirGlobal pMac, tANI_U32 statsMask);
void csrRoamRemoveEntryFromPeStatsReqList(tpAniSirGlobal pMac, tCsrPeStatsReqInfo *pPeStaEntry);
tListElem * csrRoamFindInPeStatsReqList(tpAniSirGlobal pMac, tANI_U32  statsMask);
eHalStatus csrRoamDeregStatisticsReq(tpAniSirGlobal pMac);
static tANI_U32 csrFindIbssSession( tpAniSirGlobal pMac );
static eHalStatus csrRoamStartWds( tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, tSirBssDescription *pBssDesc );
static void csrInitSession( tpAniSirGlobal pMac, tANI_U32 sessionId );
static eHalStatus csrRoamIssueSetKeyCommand( tpAniSirGlobal pMac, tANI_U32 sessionId, 
                                             tCsrRoamSetKey *pSetKey, tANI_U32 roamId );
//                                                                                  
static eHalStatus csrRoamGetQosInfoFromBss(tpAniSirGlobal pMac, tSirBssDescription *pBssDesc);
void csrRoamReissueRoamCommand(tpAniSirGlobal pMac);
#ifdef FEATURE_WLAN_BTAMP_UT_RF
void csrRoamJoinRetryTimerHandler(void *pv);
#endif
extern void SysProcessMmhMsg(tpAniSirGlobal pMac, tSirMsgQ* pMsg);
extern void btampEstablishLogLinkHdlr(void* pMsg);
static void csrSerDesUnpackDiassocRsp(tANI_U8 *pBuf, tSirSmeDisassocRsp *pRsp);
void csrReinitPreauthCmd(tpAniSirGlobal pMac, tSmeCmd *pCommand);

//                           
static void csrRoamInitGlobals(tpAniSirGlobal pMac)
{
    if(pMac)
    {
        vos_mem_zero(&csrRoamRoamSession, sizeof(csrRoamRoamSession));
        pMac->roam.roamSession = csrRoamRoamSession;
    }
    return;
}

static void csrRoamDeInitGlobals(tpAniSirGlobal pMac)
{
    if(pMac)
    {
        pMac->roam.roamSession = NULL;
    }
    return;
}
eHalStatus csrOpen(tpAniSirGlobal pMac)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
#ifndef CONFIG_ENABLE_LINUX_REG
    static uNvTables nvTables;
    VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;
    v_REGDOMAIN_t regId;
#endif
    tANI_U32 i;
    
    do
    {
        /*                             */
        csrRoamInitGlobals(pMac);
        for( i = 0; i < CSR_ROAM_SESSION_MAX; i++ )
           csrRoamStateChange( pMac, eCSR_ROAMING_STATE_STOP, i);

        initConfigParam(pMac);
        if(!HAL_STATUS_SUCCESS((status = csrScanOpen(pMac))))
            break;
        if(!HAL_STATUS_SUCCESS((status = csrRoamOpen(pMac))))
            break;
        pMac->roam.nextRoamId = 1;  //             
        if(!HAL_STATUS_SUCCESS(csrLLOpen(pMac->hHdd, &pMac->roam.statsClientReqList)))
           break;
        if(!HAL_STATUS_SUCCESS(csrLLOpen(pMac->hHdd, &pMac->roam.peStatsReqList)))
           break;
        if(!HAL_STATUS_SUCCESS(csrLLOpen(pMac->hHdd, &pMac->roam.roamCmdPendingList)))
           break;

#ifndef CONFIG_ENABLE_LINUX_REG
        vosStatus = vos_nv_readDefaultCountryTable( &nvTables );
        if ( VOS_IS_STATUS_SUCCESS(vosStatus) )
        {
            palCopyMemory( pMac->hHdd, pMac->scan.countryCodeDefault, 
                    nvTables.defaultCountryTable.countryCode, WNI_CFG_COUNTRY_CODE_LEN );
            status = eHAL_STATUS_SUCCESS;
        }
        else
        {
            smsLog( pMac, LOGE, FL("  fail to get NV_FIELD_IMAGE") );
            //                 
            pMac->scan.countryCodeDefault[0] = 'U';
            pMac->scan.countryCodeDefault[1] = 'S';
            pMac->scan.countryCodeDefault[2] = 'I';
            //                             
        }
        smsLog( pMac, LOG1, FL(" country Code from nvRam %.2s"), pMac->scan.countryCodeDefault );

        if (!('0' == pMac->scan.countryCodeDefault[0] &&
            '0' == pMac->scan.countryCodeDefault[1]))
        {
            csrGetRegulatoryDomainForCountry(pMac, pMac->scan.countryCodeDefault,
                                             &regId, COUNTRY_NV);
        }
        else
        {
            regId = REGDOMAIN_WORLD;
        }
        WDA_SetRegDomain(pMac, regId, eSIR_TRUE);
        pMac->scan.domainIdDefault = regId;
        pMac->scan.domainIdCurrent = pMac->scan.domainIdDefault;
        status = palCopyMemory(pMac->hHdd, pMac->scan.countryCodeCurrent, 
                         pMac->scan.countryCodeDefault, WNI_CFG_COUNTRY_CODE_LEN);
        status = csrInitGetChannels( pMac );
#endif
    }while(0);
    
    return (status);
}

/*                                                                           
                       
                                                                       
                      
                                                                             */
eHalStatus csrInitChannels(tpAniSirGlobal pMac)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    static uNvTables nvTables;
    VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;
    v_REGDOMAIN_t regId;

    vosStatus = vos_nv_readDefaultCountryTable( &nvTables );
    if ( VOS_IS_STATUS_SUCCESS(vosStatus) )
    {
        vos_mem_copy(pMac->scan.countryCodeDefault,
                     nvTables.defaultCountryTable.countryCode,
                     WNI_CFG_COUNTRY_CODE_LEN);
    }
    else
    {
        smsLog( pMac, LOGE, FL("  fail to get NV_FIELD_IMAGE") );
        //                 
        pMac->scan.countryCodeDefault[0] = 'U';
        pMac->scan.countryCodeDefault[1] = 'S';
        pMac->scan.countryCodeDefault[2] = 'I';
    }
    smsLog( pMac, LOG1, FL(" country Code from nvRam %.2s"), pMac->scan.countryCodeDefault );

    if (!('0' == pMac->scan.countryCodeDefault[0] &&
        '0' == pMac->scan.countryCodeDefault[1]))
    {
        csrGetRegulatoryDomainForCountry(pMac, pMac->scan.countryCodeDefault,
                                         &regId, COUNTRY_NV);
    }
    else
    {
        regId = REGDOMAIN_WORLD;
    }

    WDA_SetRegDomain(pMac, regId, eSIR_TRUE);
    pMac->scan.domainIdDefault = regId;
    pMac->scan.domainIdCurrent = pMac->scan.domainIdDefault;
    vos_mem_copy(pMac->scan.countryCodeCurrent, pMac->scan.countryCodeDefault,
                 WNI_CFG_COUNTRY_CODE_LEN);
    status = csrInitGetChannels( pMac );

    return status;
}

eHalStatus csrSetRegInfo(tHalHandle hHal,  tANI_U8 *apCntryCode)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
    v_REGDOMAIN_t regId;
    v_U8_t        cntryCodeLength;
    if(NULL == apCntryCode)
    {
       smsLog( pMac, LOGW, FL(" Invalid country Code Pointer") );
       return eHAL_STATUS_FAILURE;
    }
    smsLog( pMac, LOGW, FL(" country Code %.2s"), apCntryCode );
    /*                                                
                                              
                                                           */
    cntryCodeLength = WNI_CFG_COUNTRY_CODE_LEN;
/*
                                          

                                                   
     
                                                                
                                  
     
*/
    status = csrGetRegulatoryDomainForCountry(pMac, apCntryCode, &regId,
                                              COUNTRY_USER);
    if (status != eHAL_STATUS_SUCCESS)
    {
        smsLog( pMac, LOGE, FL("  fail to get regId for country Code %.2s"), apCntryCode );
        return status;
    }
    status = WDA_SetRegDomain(hHal, regId, eSIR_TRUE);
    if (status != eHAL_STATUS_SUCCESS)
    {
        smsLog( pMac, LOGE, FL("  fail to get regId for country Code %.2s"), apCntryCode );
        return status;
    }
    pMac->scan.domainIdDefault = regId;
    pMac->scan.domainIdCurrent = pMac->scan.domainIdDefault;
    /*                */
    palFillMemory( pMac->hHdd,
                   pMac->scan.countryCodeDefault,
                   WNI_CFG_COUNTRY_CODE_LEN,
                   0 );
    /*                                */
    palCopyMemory( pMac->hHdd, pMac->scan.countryCodeDefault, 
                apCntryCode, cntryCodeLength );
    /*                                                             */
    if((WNI_CFG_COUNTRY_CODE_LEN - 1) == cntryCodeLength)
    {
       palFillMemory( pMac->hHdd,
                      pMac->scan.countryCodeDefault + 2,
                      1,
                      0x20 );
    }
    status = palCopyMemory(pMac->hHdd, pMac->scan.countryCodeCurrent, 
                               pMac->scan.countryCodeDefault, WNI_CFG_COUNTRY_CODE_LEN);
    status = csrInitGetChannels( pMac );
    return status;
}
eHalStatus csrSetChannels(tHalHandle hHal,  tCsrConfigParam *pParam  )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
    tANI_U8   index = 0;
    palCopyMemory( pMac->hHdd, pParam->Csr11dinfo.countryCode, 
                   pMac->scan.countryCodeCurrent, WNI_CFG_COUNTRY_CODE_LEN );
    for ( index = 0; index < pMac->scan.base20MHzChannels.numChannels ; index++)
    {
        pParam->Csr11dinfo.Channels.channelList[index] = pMac->scan.base20MHzChannels.channelList[ index ];
        pParam->Csr11dinfo.ChnPower[index].firstChannel = pMac->scan.base20MHzChannels.channelList[ index ];
        pParam->Csr11dinfo.ChnPower[index].numChannels = 1;
        pParam->Csr11dinfo.ChnPower[index].maxtxPower = pMac->scan.defaultPowerTable[index].pwr;
    }
    pParam->Csr11dinfo.Channels.numChannels = pMac->scan.base20MHzChannels.numChannels;
    
    return status;
}
eHalStatus csrClose(tpAniSirGlobal pMac)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;

    csrRoamClose(pMac);
    csrScanClose(pMac);
    csrLLClose(&pMac->roam.statsClientReqList);
    csrLLClose(&pMac->roam.peStatsReqList);
    csrLLClose(&pMac->roam.roamCmdPendingList);
    /*                */
    csrRoamDeInitGlobals(pMac);
    return (status);
} 

eHalStatus csrUpdateChannelList(tpAniSirGlobal pMac)
{
    tSirUpdateChanList *pChanList;
    tCsrScanStruct *pScan = &pMac->scan;
    tANI_U8 numChan = pScan->base20MHzChannels.numChannels;
    tANI_U32 bufLen = sizeof(tSirUpdateChanList) +
        (sizeof(tSirUpdateChanParam) * (numChan - 1));
    vos_msg_t msg;
    tANI_U8 i;

    pChanList = (tSirUpdateChanList *) vos_mem_malloc(bufLen);
    if (!pChanList)
    {
        VOS_TRACE(VOS_MODULE_ID_VOSS, VOS_TRACE_LEVEL_ERROR,
                "Failed to allocate memory for tSirUpdateChanList");
        return eHAL_STATUS_FAILED_ALLOC;
    }

    msg.type = WDA_UPDATE_CHAN_LIST_REQ;
    msg.reserved = 0;
    msg.bodyptr = pChanList;
    pChanList->numChan = numChan;
    for (i = 0; i < pChanList->numChan; i++)
    {
        pChanList->chanParam[i].chanId = pScan->defaultPowerTable[i].chanId;
        pChanList->chanParam[i].pwr = cfgGetRegulatoryMaxTransmitPower(pMac,
                pScan->defaultPowerTable[i].chanId);
        if (vos_nv_getChannelEnabledState(pChanList->chanParam[i].chanId) ==
            NV_CHANNEL_DFS)
            pChanList->chanParam[i].dfsSet = VOS_TRUE;
        else
            pChanList->chanParam[i].dfsSet = VOS_FALSE;
    }

    if(VOS_STATUS_SUCCESS != vos_mq_post_message(VOS_MODULE_ID_WDA, &msg))
    {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL,
                "%s: Failed to post msg to WDA", __func__);
        vos_mem_free(pChanList);
        return eHAL_STATUS_FAILURE;
    }

    return eHAL_STATUS_SUCCESS;
}

eHalStatus csrStart(tpAniSirGlobal pMac)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_U32 i;
 
    do
    {
       //                           
        pMac->roam.gVosContext = vos_get_global_context(VOS_MODULE_ID_SME, pMac);
        for( i = 0; i < CSR_ROAM_SESSION_MAX; i++ )
           csrRoamStateChange( pMac, eCSR_ROAMING_STATE_IDLE, i );

        status = csrRoamStart(pMac);
        if(!HAL_STATUS_SUCCESS(status)) break;
        pMac->scan.f11dInfoApplied = eANI_BOOLEAN_FALSE;
        status = pmcRegisterPowerSaveCheck(pMac, csrCheckPSReady, pMac);
        if(!HAL_STATUS_SUCCESS(status)) break;
        pMac->roam.sPendingCommands = 0;
        csrScanEnable(pMac);
#if   defined WLAN_FEATURE_NEIGHBOR_ROAMING
        status = csrNeighborRoamInit(pMac);
#endif /*                               */
        pMac->roam.tlStatsReqInfo.numClient = 0;
        pMac->roam.tlStatsReqInfo.periodicity = 0;
        pMac->roam.tlStatsReqInfo.timerRunning = FALSE;
        //                                     
        pMac->roam.vccLinkQuality = eCSR_ROAM_LINK_QUAL_MIN_IND;
        if(!HAL_STATUS_SUCCESS(status)) 
        {
           smsLog(pMac, LOGW, " csrStart: Couldn't Init HO control blk ");
           break;
        }
#ifdef QCA_WIFI_2_0
        if (pMac->fScanOffload)
        {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
                    "Scan offload is enabled, update default chan list");
            status = csrUpdateChannelList(pMac);
        }
#else
        status = csrUpdateChannelList(pMac);
#endif

    }while(0);
#if defined(ANI_LOGDUMP)
    csrDumpInit(pMac);
#endif //                        
    return (status);
}

eHalStatus csrStop(tpAniSirGlobal pMac, tHalStopType stopType)
{
    tANI_U32 sessionId;
    tANI_U32 i;

    for(sessionId = 0; sessionId < CSR_ROAM_SESSION_MAX; sessionId++)
    {
        csrRoamCloseSession(pMac, sessionId, TRUE, NULL, NULL);
    }
    csrScanDisable(pMac);
    pMac->scan.fCancelIdleScan = eANI_BOOLEAN_FALSE;
    pMac->scan.fRestartIdleScan = eANI_BOOLEAN_FALSE;
    csrLLPurge( &pMac->roam.roamCmdPendingList, eANI_BOOLEAN_TRUE );
    
#if   defined WLAN_FEATURE_NEIGHBOR_ROAMING
    csrNeighborRoamClose(pMac);
#endif
    csrScanFlushResult(pMac); //                      
    //                                                        
    //                                                             
    (void) pmcDeregisterPowerSaveCheck(pMac, csrCheckPSReady);
    //                                   
    pMac->scan.domainIdCurrent = pMac->scan.domainIdDefault;
    csrResetCountryInformation(pMac, eANI_BOOLEAN_TRUE, eANI_BOOLEAN_FALSE );

    for( i = 0; i < CSR_ROAM_SESSION_MAX; i++ )
    {
       csrRoamStateChange( pMac, eCSR_ROAMING_STATE_STOP, i );
       pMac->roam.curSubState[i] = eCSR_ROAM_SUBSTATE_NONE;
    }

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
    /*                                            
                                                   
                                               
                                                     
     */
    if (HAL_STOP_TYPE_SYS_RESET == stopType)
    {
       bRoamScanOffloadStarted = VOS_FALSE;
    }
#endif

    return (eHAL_STATUS_SUCCESS);
}

eHalStatus csrReady(tpAniSirGlobal pMac)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    csrScanGetSupportedChannels( pMac );
    //                                                     
    //                                       
    csrInitBGScanChannelList(pMac);
    /*                          */
    csrScanStartResultAgingTimer(pMac);
    /*                                                                     
                                               */  
    if(0 != pMac->scan.scanResultCfgAgingTime )
    {
       csrScanStartResultCfgAgingTimer(pMac);
    }
    //                                        
    WLANTL_GetACWeights(pMac->roam.gVosContext, pMac->roam.ucACWeights);
    status = csrInitChannelList( pMac );
    if ( ! HAL_STATUS_SUCCESS( status ) )
    {
       smsLog( pMac, LOGE, "csrInitChannelList failed during csrReady with status=%d",
               status );
    }
    return (status);
}
void csrSetDefaultDot11Mode( tpAniSirGlobal pMac )
{
    v_U32_t wniDot11mode = 0;
    wniDot11mode = csrTranslateToWNICfgDot11Mode(pMac,pMac->roam.configParam.uCfgDot11Mode);
    ccmCfgSetInt(pMac, WNI_CFG_DOT11_MODE, wniDot11mode, NULL, eANI_BOOLEAN_FALSE);
}
void csrSetGlobalCfgs( tpAniSirGlobal pMac )
{

    ccmCfgSetInt(pMac, WNI_CFG_FRAGMENTATION_THRESHOLD, csrGetFragThresh(pMac), NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetInt(pMac, WNI_CFG_RTS_THRESHOLD, csrGetRTSThresh(pMac), NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetInt(pMac, WNI_CFG_11D_ENABLED,
                        ((pMac->roam.configParam.Is11hSupportEnabled) ? pMac->roam.configParam.Is11dSupportEnabled : pMac->roam.configParam.Is11dSupportEnabled), 
                        NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetInt(pMac, WNI_CFG_11H_ENABLED, pMac->roam.configParam.Is11hSupportEnabled, NULL, eANI_BOOLEAN_FALSE);
    /*                                                                                                                                 
                                                                                                          
     */
    ccmCfgSetInt(pMac, WNI_CFG_CHANNEL_BONDING_MODE, !!(pMac->roam.configParam.channelBondingMode5GHz), NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, pMac->roam.configParam.HeartbeatThresh24, NULL, eANI_BOOLEAN_FALSE);
    
    //                                                                    
    //                                                                      
    csrSetDefaultDot11Mode( pMac );    
}

eHalStatus csrRoamOpen(tpAniSirGlobal pMac)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_U32 i;
    tCsrRoamSession *pSession;
    do
    {
        for( i = 0; i < CSR_ROAM_SESSION_MAX; i++ )
        {
            pSession = CSR_GET_SESSION( pMac, i );
            pSession->roamingTimerInfo.pMac = pMac;
            pSession->roamingTimerInfo.sessionId = CSR_SESSION_ID_INVALID;
        }
        pMac->roam.WaitForKeyTimerInfo.pMac = pMac;
        pMac->roam.WaitForKeyTimerInfo.sessionId = CSR_SESSION_ID_INVALID;
        status = vos_timer_init(&pMac->roam.hTimerWaitForKey, VOS_TIMER_TYPE_SW,
                                csrRoamWaitForKeyTimeOutHandler,
                                &pMac->roam.WaitForKeyTimerInfo);
      if (!HAL_STATUS_SUCCESS(status))
      {
        smsLog(pMac, LOGE, FL("cannot allocate memory for WaitForKey time out timer"));
        break;
      }
      status = vos_timer_init(&pMac->roam.tlStatsReqInfo.hTlStatsTimer,
                              VOS_TIMER_TYPE_SW, csrRoamTlStatsTimerHandler, pMac);
      if (!HAL_STATUS_SUCCESS(status))
      {
         smsLog(pMac, LOGE, FL("cannot allocate memory for summary Statistics timer"));
         return eHAL_STATUS_FAILURE;
      }
    }while (0);
    return (status);
}

eHalStatus csrRoamClose(tpAniSirGlobal pMac)
{
    tANI_U32 sessionId;
    for(sessionId = 0; sessionId < CSR_ROAM_SESSION_MAX; sessionId++)
    {
        csrRoamCloseSession(pMac, sessionId, TRUE, NULL, NULL);
    }
    vos_timer_stop(&pMac->roam.hTimerWaitForKey);
    vos_timer_destroy(&pMac->roam.hTimerWaitForKey);
    vos_timer_stop(&pMac->roam.tlStatsReqInfo.hTlStatsTimer);
    vos_timer_destroy(&pMac->roam.tlStatsReqInfo.hTlStatsTimer);
    return (eHAL_STATUS_SUCCESS);
}

eHalStatus csrRoamStart(tpAniSirGlobal pMac)
{
    (void)pMac;
    return (eHAL_STATUS_SUCCESS);
}

void csrRoamStop(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
   csrRoamStopRoamingTimer(pMac, sessionId);
   /*                                                                                        */
   csrRoamDeregStatisticsReq(pMac);
}
eHalStatus csrRoamGetConnectState(tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrConnectState *pState)
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
    if ( CSR_IS_SESSION_VALID(pMac, sessionId) && (NULL != pState) )
    {
        status = eHAL_STATUS_SUCCESS;
        *pState = pMac->roam.roamSession[sessionId].connectState;
    }    
    return (status);
}

eHalStatus csrRoamCopyConnectProfile(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamConnectedProfile *pProfile)
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    tANI_U32 size = 0;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    if(pProfile)
    {
        if(pSession->pConnectBssDesc)
        {
            do
            {
                size = pSession->pConnectBssDesc->length + sizeof(pSession->pConnectBssDesc->length);
                if(size)
                {
                    status = palAllocateMemory(pMac->hHdd, (void **)&pProfile->pBssDesc, size);
                    if(HAL_STATUS_SUCCESS(status))
                    {
                        palCopyMemory(pMac->hHdd, pProfile->pBssDesc, pSession->pConnectBssDesc, size);
                    }
                    else
                        break;
                }
                else
                {
                    pProfile->pBssDesc = NULL;
                }
                pProfile->AuthType = pSession->connectedProfile.AuthType;
                pProfile->EncryptionType = pSession->connectedProfile.EncryptionType;
                pProfile->mcEncryptionType = pSession->connectedProfile.mcEncryptionType;
                pProfile->BSSType = pSession->connectedProfile.BSSType;
                pProfile->operationChannel = pSession->connectedProfile.operationChannel;
                pProfile->CBMode = pSession->connectedProfile.CBMode;
                palCopyMemory(pMac->hHdd, &pProfile->bssid, &pSession->connectedProfile.bssid, sizeof(tCsrBssid));
                palCopyMemory(pMac->hHdd, &pProfile->SSID, &pSession->connectedProfile.SSID, sizeof(tSirMacSSid));
#ifdef WLAN_FEATURE_VOWIFI_11R
                if (pSession->connectedProfile.MDID.mdiePresent)
                {
                    pProfile->MDID.mdiePresent = 1;
                    pProfile->MDID.mobilityDomain = pSession->connectedProfile.MDID.mobilityDomain;
                }
                else
                {
                    pProfile->MDID.mdiePresent = 0;
                    pProfile->MDID.mobilityDomain = 0;
                }
#endif
#ifdef FEATURE_WLAN_CCX
                pProfile->isCCXAssoc = pSession->connectedProfile.isCCXAssoc;
                if (csrIsAuthTypeCCX(pSession->connectedProfile.AuthType))
                {
                    palCopyMemory( pMac->hHdd, pProfile->ccxCckmInfo.krk, 
                        pSession->connectedProfile.ccxCckmInfo.krk, CSR_KRK_KEY_LEN );
                    pProfile->ccxCckmInfo.reassoc_req_num=
                        pSession->connectedProfile.ccxCckmInfo.reassoc_req_num;
                    pProfile->ccxCckmInfo.krk_plumbed = 
                        pSession->connectedProfile.ccxCckmInfo.krk_plumbed;
                }
#endif
            }while(0);
        }
    }
    
    return (status);
}

eHalStatus csrRoamGetConnectProfile(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamConnectedProfile *pProfile)
{
    eHalStatus status = eHAL_STATUS_FAILURE;

    if((csrIsConnStateConnected(pMac, sessionId)) ||
       (csrIsConnStateIbss(pMac, sessionId)))
    {
        if(pProfile)
        {
            status = csrRoamCopyConnectProfile(pMac, sessionId, pProfile);
        }
    }
    return (status);
}

eHalStatus csrRoamFreeConnectProfile(tpAniSirGlobal pMac, tCsrRoamConnectedProfile *pProfile)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    
    if(pProfile->pBssDesc)
    {
        palFreeMemory(pMac->hHdd, pProfile->pBssDesc);
    }
    if(pProfile->pAddIEAssoc)
    {
        palFreeMemory(pMac->hHdd, pProfile->pAddIEAssoc);
    }
    palZeroMemory(pMac->hHdd, pProfile, sizeof(tCsrRoamConnectedProfile));
    pProfile->AuthType = eCSR_AUTH_TYPE_UNKNOWN;
    return (status);
}

static eHalStatus csrRoamFreeConnectedInfo( tpAniSirGlobal pMac, tCsrRoamConnectedInfo *pConnectedInfo )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    if( pConnectedInfo->pbFrames )
    {
        palFreeMemory( pMac->hHdd, pConnectedInfo->pbFrames );
        pConnectedInfo->pbFrames = NULL;
    }
    pConnectedInfo->nBeaconLength = 0;
    pConnectedInfo->nAssocReqLength = 0;
    pConnectedInfo->nAssocRspLength = 0;
    pConnectedInfo->staId = 0;
#ifdef WLAN_FEATURE_VOWIFI_11R
    pConnectedInfo->nRICRspLength = 0;
#endif    
#ifdef FEATURE_WLAN_CCX
    pConnectedInfo->nTspecIeLength = 0;
#endif    
    return ( status );
}

    
                
                
void csrReleaseCommandPreauth(tpAniSirGlobal pMac, tSmeCmd *pCommand)
{
    csrReinitPreauthCmd(pMac, pCommand);
    csrReleaseCommand( pMac, pCommand );
}
                
void csrReleaseCommandRoam(tpAniSirGlobal pMac, tSmeCmd *pCommand)
{
    csrReinitRoamCmd(pMac, pCommand);
    csrReleaseCommand( pMac, pCommand );
}

void csrReleaseCommandScan(tpAniSirGlobal pMac, tSmeCmd *pCommand)
{
    csrReinitScanCmd(pMac, pCommand);
    csrReleaseCommand( pMac, pCommand );
}

void csrReleaseCommandWmStatusChange(tpAniSirGlobal pMac, tSmeCmd *pCommand)
{
    csrReinitWmStatusChangeCmd(pMac, pCommand);
    csrReleaseCommand( pMac, pCommand );
}

void csrReinitSetKeyCmd(tpAniSirGlobal pMac, tSmeCmd *pCommand)
{
    palZeroMemory(pMac->hHdd, &pCommand->u.setKeyCmd, sizeof(tSetKeyCmd));
}

void csrReinitRemoveKeyCmd(tpAniSirGlobal pMac, tSmeCmd *pCommand)
{
    palZeroMemory(pMac->hHdd, &pCommand->u.removeKeyCmd, sizeof(tRemoveKeyCmd));
}

void csrReleaseCommandSetKey(tpAniSirGlobal pMac, tSmeCmd *pCommand)
{
    csrReinitSetKeyCmd(pMac, pCommand);
    csrReleaseCommand( pMac, pCommand );
}
void csrReleaseCommandRemoveKey(tpAniSirGlobal pMac, tSmeCmd *pCommand)
{
    csrReinitRemoveKeyCmd(pMac, pCommand);
    csrReleaseCommand( pMac, pCommand );
}
void csrAbortCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand, tANI_BOOLEAN fStopping )
{

    if( eSmeCsrCommandMask & pCommand->command )
    {
        switch (pCommand->command)
        {
        case eSmeCommandScan:
            //                                                                 
            smsLog( pMac, LOGW, "%s: Drop scan reason %d callback 0x%X",
                __func__, pCommand->u.scanCmd.reason, (unsigned int)pCommand->u.scanCmd.callback);
            if (NULL != pCommand->u.scanCmd.callback)
            {
                smsLog( pMac, LOGW, "%s callback scan requester", __func__);
                csrScanCallCallback(pMac, pCommand, eCSR_SCAN_ABORT);
            }
            csrReleaseCommandScan( pMac, pCommand );
            break;
        case eSmeCommandRoam:
            csrReleaseCommandRoam( pMac, pCommand );
            break;

        case eSmeCommandWmStatusChange:
            csrReleaseCommandWmStatusChange( pMac, pCommand );
            break;

        case eSmeCommandSetKey:
            csrReleaseCommandSetKey( pMac, pCommand );
            break;

        case eSmeCommandRemoveKey:
            csrReleaseCommandRemoveKey( pMac, pCommand );
            break;

    default:
            smsLog( pMac, LOGW, " CSR abort standard command %d", pCommand->command );
            csrReleaseCommand( pMac, pCommand );
            break;
        }
    }
}

void csrRoamSubstateChange( tpAniSirGlobal pMac, eCsrRoamSubState NewSubstate, tANI_U32 sessionId)
{
    smsLog( pMac, LOG1, "   CSR RoamSubstate: [ %d <== %d ]", NewSubstate, pMac->roam.curSubState[sessionId]);

    if(pMac->roam.curSubState[sessionId] == NewSubstate)
    {
       return;
    }
    pMac->roam.curSubState[sessionId] = NewSubstate;
}

eCsrRoamState csrRoamStateChange( tpAniSirGlobal pMac, eCsrRoamState NewRoamState, tANI_U8 sessionId)
{
    eCsrRoamState PreviousState;
          
    smsLog( pMac, LOG1, "CSR RoamState: [ %d <== %d ]", NewRoamState, pMac->roam.curState[sessionId]);

    PreviousState = pMac->roam.curState[sessionId];
    
    if ( NewRoamState != pMac->roam.curState[sessionId] ) 
    {
        //                                                                               
        if ( CSR_IS_ROAM_JOINING(pMac, sessionId) ) 
        {
            csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_NONE, sessionId );
        }

        pMac->roam.curState[sessionId] = NewRoamState;
    }
    return( PreviousState );
}

void csrAssignRssiForCategory(tpAniSirGlobal pMac, tANI_S8 bestApRssi, tANI_U8 catOffset)
{
    int i;
    if(catOffset)
    {
        pMac->roam.configParam.bCatRssiOffset = catOffset;
        for(i = 0; i < CSR_NUM_RSSI_CAT; i++)
        {
            pMac->roam.configParam.RSSICat[CSR_NUM_RSSI_CAT - i - 1] = (int)bestApRssi - pMac->roam.configParam.nSelect5GHzMargin - (int)(i * catOffset);
        }
    }
}

static void initConfigParam(tpAniSirGlobal pMac)
{
    int i;
    pMac->roam.configParam.agingCount = CSR_AGING_COUNT;
    pMac->roam.configParam.channelBondingMode24GHz = WNI_CFG_CHANNEL_BONDING_MODE_DISABLE;
    pMac->roam.configParam.channelBondingMode5GHz = WNI_CFG_CHANNEL_BONDING_MODE_ENABLE;

    pMac->roam.configParam.phyMode = eCSR_DOT11_MODE_TAURUS;
    pMac->roam.configParam.eBand = eCSR_BAND_ALL;
    pMac->roam.configParam.uCfgDot11Mode = eCSR_CFG_DOT11_MODE_TAURUS;
    pMac->roam.configParam.FragmentationThreshold = eCSR_DOT11_FRAG_THRESH_DEFAULT;
    pMac->roam.configParam.HeartbeatThresh24 = 40;
    pMac->roam.configParam.HeartbeatThresh50 = 40;
    pMac->roam.configParam.Is11dSupportEnabled = eANI_BOOLEAN_FALSE;
    pMac->roam.configParam.Is11dSupportEnabledOriginal = eANI_BOOLEAN_FALSE;
    pMac->roam.configParam.Is11eSupportEnabled = eANI_BOOLEAN_TRUE;
    pMac->roam.configParam.Is11hSupportEnabled = eANI_BOOLEAN_TRUE;
    pMac->roam.configParam.RTSThreshold = 2346;
    pMac->roam.configParam.shortSlotTime = eANI_BOOLEAN_TRUE;
    pMac->roam.configParam.WMMSupportMode = eCsrRoamWmmAuto;
    pMac->roam.configParam.ProprietaryRatesEnabled = eANI_BOOLEAN_TRUE;
    pMac->roam.configParam.TxRate = eCSR_TX_RATE_AUTO;
    pMac->roam.configParam.impsSleepTime = CSR_IDLE_SCAN_NO_PS_INTERVAL;
    pMac->roam.configParam.scanAgeTimeNCNPS = CSR_SCAN_AGING_TIME_NOT_CONNECT_NO_PS;  
    pMac->roam.configParam.scanAgeTimeNCPS = CSR_SCAN_AGING_TIME_NOT_CONNECT_W_PS;   
    pMac->roam.configParam.scanAgeTimeCNPS = CSR_SCAN_AGING_TIME_CONNECT_NO_PS;   
    pMac->roam.configParam.scanAgeTimeCPS = CSR_SCAN_AGING_TIME_CONNECT_W_PS;   
    for(i = 0; i < CSR_NUM_RSSI_CAT; i++)
    {
        pMac->roam.configParam.BssPreferValue[i] = i;
    }
    csrAssignRssiForCategory(pMac, CSR_BEST_RSSI_VALUE, CSR_DEFAULT_RSSI_DB_GAP);
    pMac->roam.configParam.nRoamingTime = CSR_DEFAULT_ROAMING_TIME;
    pMac->roam.configParam.fEnforce11dChannels = eANI_BOOLEAN_FALSE;
    pMac->roam.configParam.fSupplicantCountryCodeHasPriority = eANI_BOOLEAN_FALSE;
    pMac->roam.configParam.fEnforceCountryCodeMatch = eANI_BOOLEAN_FALSE;
    pMac->roam.configParam.fEnforceDefaultDomain = eANI_BOOLEAN_FALSE;
    pMac->roam.configParam.nActiveMaxChnTime = CSR_ACTIVE_MAX_CHANNEL_TIME;
    pMac->roam.configParam.nActiveMinChnTime = CSR_ACTIVE_MIN_CHANNEL_TIME;
    pMac->roam.configParam.nPassiveMaxChnTime = CSR_PASSIVE_MAX_CHANNEL_TIME;
    pMac->roam.configParam.nPassiveMinChnTime = CSR_PASSIVE_MIN_CHANNEL_TIME;
    pMac->roam.configParam.nActiveMaxChnTimeBtc = CSR_ACTIVE_MAX_CHANNEL_TIME_BTC;
    pMac->roam.configParam.nActiveMinChnTimeBtc = CSR_ACTIVE_MIN_CHANNEL_TIME_BTC;
    pMac->roam.configParam.disableAggWithBtc = eANI_BOOLEAN_TRUE;
#ifdef WLAN_AP_STA_CONCURRENCY
    pMac->roam.configParam.nActiveMaxChnTimeConc = CSR_ACTIVE_MAX_CHANNEL_TIME_CONC;
    pMac->roam.configParam.nActiveMinChnTimeConc = CSR_ACTIVE_MIN_CHANNEL_TIME_CONC;
    pMac->roam.configParam.nPassiveMaxChnTimeConc = CSR_PASSIVE_MAX_CHANNEL_TIME_CONC;
    pMac->roam.configParam.nPassiveMinChnTimeConc = CSR_PASSIVE_MIN_CHANNEL_TIME_CONC;
    pMac->roam.configParam.nRestTimeConc = CSR_REST_TIME_CONC;
    pMac->roam.configParam.nNumStaChanCombinedConc = CSR_NUM_STA_CHAN_COMBINED_CONC;
    pMac->roam.configParam.nNumP2PChanCombinedConc = CSR_NUM_P2P_CHAN_COMBINED_CONC;
#endif
    pMac->roam.configParam.IsIdleScanEnabled = TRUE; //                               
    pMac->roam.configParam.nTxPowerCap = CSR_MAX_TX_POWER;
    pMac->roam.configParam.statsReqPeriodicity = CSR_MIN_GLOBAL_STAT_QUERY_PERIOD;
    pMac->roam.configParam.statsReqPeriodicityInPS = CSR_MIN_GLOBAL_STAT_QUERY_PERIOD_IN_BMPS;
#ifdef WLAN_FEATURE_VOWIFI_11R
    pMac->roam.configParam.csr11rConfig.IsFTResourceReqSupported = 0;
#endif
#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
    pMac->roam.configParam.neighborRoamConfig.nMaxNeighborRetries = 3;
    pMac->roam.configParam.neighborRoamConfig.nNeighborLookupRssiThreshold = 120;
    pMac->roam.configParam.neighborRoamConfig.nNeighborReassocRssiThreshold = 125;
    pMac->roam.configParam.neighborRoamConfig.nNeighborScanMinChanTime = 20;
    pMac->roam.configParam.neighborRoamConfig.nNeighborScanMaxChanTime = 40;
    pMac->roam.configParam.neighborRoamConfig.nNeighborScanTimerPeriod = 200;
    pMac->roam.configParam.neighborRoamConfig.neighborScanChanList.numChannels = 3;
    pMac->roam.configParam.neighborRoamConfig.neighborScanChanList.channelList[0] = 1;
    pMac->roam.configParam.neighborRoamConfig.neighborScanChanList.channelList[1] = 6;
    pMac->roam.configParam.neighborRoamConfig.neighborScanChanList.channelList[2] = 11;
    pMac->roam.configParam.neighborRoamConfig.nNeighborResultsRefreshPeriod = 20000; //          
    pMac->roam.configParam.neighborRoamConfig.nEmptyScanRefreshPeriod = 0;
#endif
#ifdef WLAN_FEATURE_11AC
     pMac->roam.configParam.nVhtChannelWidth = WNI_CFG_VHT_CHANNEL_WIDTH_80MHZ + 1;
#endif

    pMac->roam.configParam.addTSWhenACMIsOff = 0;
    pMac->roam.configParam.fScanTwice = eANI_BOOLEAN_FALSE;

    //                                                      
    //                          
    pMac->roam.configParam.doBMPSWorkaround = 0;

}
eCsrBand csrGetCurrentBand(tHalHandle hHal)
{
    tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
    return pMac->roam.configParam.bandCapability;
}


#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
/*
                                          
*/
eHalStatus csrFlushRoamScanRoamChannelList(tpAniSirGlobal pMac)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tpCsrNeighborRoamControlInfo    pNeighborRoamInfo = &pMac->roam.neighborRoamInfo;

    /*                                        */
    if (NULL != pNeighborRoamInfo->roamChannelInfo.currentChannelListInfo.ChannelList)
    {
        vos_mem_free(pNeighborRoamInfo->roamChannelInfo.currentChannelListInfo.ChannelList);
        pNeighborRoamInfo->roamChannelInfo.currentChannelListInfo.ChannelList = NULL;
        pNeighborRoamInfo->roamChannelInfo.currentChannelListInfo.numOfChannels = 0;
    }
    return status;
}
#endif  /*                                             */


#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
/*
                                          
*/
eHalStatus csrFlushCfgBgScanRoamChannelList(tpAniSirGlobal pMac)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tpCsrNeighborRoamControlInfo    pNeighborRoamInfo = &pMac->roam.neighborRoamInfo;

    /*                                        */
    if (NULL != pNeighborRoamInfo->cfgParams.channelInfo.ChannelList)
    {
        vos_mem_free(pNeighborRoamInfo->cfgParams.channelInfo.ChannelList);
        pNeighborRoamInfo->cfgParams.channelInfo.ChannelList = NULL;
        pNeighborRoamInfo->cfgParams.channelInfo.numOfChannels = 0;
    }
    return status;
}



/*
                                                                  
                                
*/
eHalStatus csrCreateBgScanRoamChannelList(tpAniSirGlobal pMac,
                                          const tANI_U8 *pChannelList,
                                          const tANI_U8 numChannels)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tpCsrNeighborRoamControlInfo    pNeighborRoamInfo = &pMac->roam.neighborRoamInfo;

    pNeighborRoamInfo->cfgParams.channelInfo.numOfChannels = numChannels;

    pNeighborRoamInfo->cfgParams.channelInfo.ChannelList =
            vos_mem_malloc(pNeighborRoamInfo->cfgParams.channelInfo.numOfChannels);

    if (NULL == pNeighborRoamInfo->cfgParams.channelInfo.ChannelList)
    {
        smsLog(pMac, LOGE, FL("Memory Allocation for CFG Channel List failed"));
        pNeighborRoamInfo->cfgParams.channelInfo.numOfChannels = 0;
        return eHAL_STATUS_RESOURCES;
    }

    /*                                  */
    palCopyMemory(pMac->hHdd, pNeighborRoamInfo->cfgParams.channelInfo.ChannelList,
                        pChannelList,
                        pNeighborRoamInfo->cfgParams.channelInfo.numOfChannels);
    return status;
}

/*                                                                     
                                      
                                                                          
                                                                                     
                                                                                 
*/
eHalStatus csrUpdateBgScanConfigIniChannelList(tpAniSirGlobal pMac,
                                               eCsrBand eBand)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tpCsrNeighborRoamControlInfo    pNeighborRoamInfo = &pMac->roam.neighborRoamInfo;
    tANI_U8 outNumChannels = 0;
    tANI_U8 inNumChannels = 0;
    tANI_U8 *inPtr = NULL;
    tANI_U8 i = 0;
    tANI_U8 ChannelList[WNI_CFG_VALID_CHANNEL_LIST_LEN] = {0};

    if (NULL == pNeighborRoamInfo->cfgParams.channelInfo.ChannelList)

    {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
            "No update required for channel list "
            "either cfg.ini channel list is not set up or "
            "auto band (Band %d)", eBand);
        return status;
    }

    inNumChannels = pNeighborRoamInfo->cfgParams.channelInfo.numOfChannels;
    inPtr = pNeighborRoamInfo->cfgParams.channelInfo.ChannelList;
    if (eCSR_BAND_24 == eBand)
    {
        for (i = 0; i < inNumChannels; i++)
        {
            if (CSR_IS_CHANNEL_24GHZ(inPtr[i]) && csrRoamIsChannelValid(pMac, inPtr[i]))
            {
                ChannelList[outNumChannels++] = inPtr[i];
            }
        }
        csrFlushCfgBgScanRoamChannelList(pMac);
        csrCreateBgScanRoamChannelList(pMac, ChannelList, outNumChannels);
    }
    else if (eCSR_BAND_5G == eBand)
    {
        for (i = 0; i < inNumChannels; i++)
        {
            /*                        */
            if (CSR_IS_CHANNEL_5GHZ(inPtr[i]) &&
               csrRoamIsChannelValid(pMac, inPtr[i]) &&
               !CSR_IS_CHANNEL_DFS(inPtr[i]))
            {
               ChannelList[outNumChannels++] = inPtr[i];
            }
        }
        csrFlushCfgBgScanRoamChannelList(pMac);
        csrCreateBgScanRoamChannelList(pMac, ChannelList, outNumChannels);
    }
    else if (eCSR_BAND_ALL == eBand)
    {
        for (i = 0; i < inNumChannels; i++)
        {
            if (csrRoamIsChannelValid(pMac, inPtr[i]) &&
               !CSR_IS_CHANNEL_DFS(inPtr[i]))
            {
               ChannelList[outNumChannels++] = inPtr[i];
            }
        }
        csrFlushCfgBgScanRoamChannelList(pMac);
        csrCreateBgScanRoamChannelList(pMac, ChannelList, outNumChannels);
    }
    else
    {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN,
            "Invalid band, No operation carried out (Band %d)", eBand);
        status = eHAL_STATUS_INVALID_PARAMETER;
    }

    return status;
}
#endif

#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
/*                                                                     
                                                                        
                                                                              
                  
                                                                                     
                                                                                 
*/
eHalStatus csrCreateRoamScanChannelList(tpAniSirGlobal pMac,
                                        tANI_U8 *pChannelList,
                                        tANI_U8 numChannels,
                                        const eCsrBand eBand)
{
    eHalStatus                   status = eHAL_STATUS_SUCCESS;
    tpCsrNeighborRoamControlInfo pNeighborRoamInfo = &pMac->roam.neighborRoamInfo;
    tANI_U8                      outNumChannels = 0;
    tANI_U8                      inNumChannels = numChannels;
    tANI_U8                      *inPtr = pChannelList;
    tANI_U8                      i = 0;
    tANI_U8                      ChannelList[WNI_CFG_VALID_CHANNEL_LIST_LEN] = {0};
    tANI_U8                      tmpChannelList[WNI_CFG_VALID_CHANNEL_LIST_LEN] = {0};
    tANI_U8                      mergedOutputNumOfChannels = 0;
    tpCsrChannelInfo             currChannelListInfo = &pNeighborRoamInfo->roamChannelInfo.currentChannelListInfo;

    /*                                                                                  
                                                                                             
                                                                                              */
    if (pMac->scan.occupiedChannels.numChannels)
    {
       csrNeighborRoamMergeChannelLists(pMac,
                  &pMac->scan.occupiedChannels.channelList[0],
                  pMac->scan.occupiedChannels.numChannels,
                  inPtr,
                  inNumChannels,
                  &mergedOutputNumOfChannels);
       inNumChannels =  mergedOutputNumOfChannels;
    }

    if (eCSR_BAND_24 == eBand)
    {
        for (i = 0; i < inNumChannels; i++)
        {
            if (CSR_IS_CHANNEL_24GHZ(inPtr[i]) && csrRoamIsChannelValid(pMac, inPtr[i]))
            {
                ChannelList[outNumChannels++] = inPtr[i];
            }
        }
    }
    else if (eCSR_BAND_5G == eBand)
    {
        for (i = 0; i < inNumChannels; i++)
        {
            /*                        */
            if (CSR_IS_CHANNEL_5GHZ(inPtr[i]) &&
               csrRoamIsChannelValid(pMac, inPtr[i]) &&
               !CSR_IS_CHANNEL_DFS(inPtr[i]))
            {
               ChannelList[outNumChannels++] = inPtr[i];
            }
        }
    }
    else if (eCSR_BAND_ALL == eBand)
    {
        for (i = 0; i < inNumChannels; i++)
        {
            if (csrRoamIsChannelValid(pMac, inPtr[i]) &&
               !CSR_IS_CHANNEL_DFS(inPtr[i]))
            {
               ChannelList[outNumChannels++] = inPtr[i];
            }
        }
    }
    else
    {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN,
            "Invalid band, No operation carried out (Band %d)", eBand);
        return eHAL_STATUS_INVALID_PARAMETER;
    }

    /*                                                        
                             
                                                                      
                                                                             
                                                                               */

    if ((eCSR_BAND_ALL == eBand) && CSR_IS_ROAM_INTRA_BAND_ENABLED(pMac))
    {
        csrNeighborRoamChannelsFilterByCurrentBand(
                             pMac,
                             ChannelList,
                             outNumChannels,
                             tmpChannelList,
                             &outNumChannels);
        palCopyMemory(pMac->hHdd, ChannelList,
                      tmpChannelList, outNumChannels);
    }

    /*                                      */
    if(outNumChannels)
    {
        /*                              */
        if (NULL != currChannelListInfo->ChannelList)
        {
            vos_mem_free(currChannelListInfo->ChannelList);
            currChannelListInfo->ChannelList = NULL;
            currChannelListInfo->numOfChannels = 0;
        }

        currChannelListInfo->ChannelList = vos_mem_malloc(outNumChannels * sizeof(tANI_U8));
        if (NULL == currChannelListInfo->ChannelList)
        {
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL,
                "Failed to allocate memory for roam scan channel list");
            currChannelListInfo->numOfChannels = 0;
            return VOS_STATUS_E_RESOURCES;
        }
        palCopyMemory(pMac->hHdd, currChannelListInfo->ChannelList,
                      ChannelList, outNumChannels);
    }
    return status;
}
#endif /*                                             */

eHalStatus csrSetBand(tHalHandle hHal, eCsrBand eBand)
{
    tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
    eHalStatus status = eHAL_STATUS_SUCCESS;
    if (CSR_IS_PHY_MODE_A_ONLY(pMac) &&
            (eBand == eCSR_BAND_24))
    {
        /*                                               
                                                 */
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
            "failed to set band cfg80211 = %u, band = %u",
            pMac->roam.configParam.uCfgDot11Mode, eBand);
        return eHAL_STATUS_INVALID_PARAMETER;
    }
    if ((CSR_IS_PHY_MODE_B_ONLY(pMac) ||
                CSR_IS_PHY_MODE_G_ONLY(pMac)) &&
            (eBand == eCSR_BAND_5G))
    {
        /*                                                   
                                               */
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
            "failed to set band dot11mode = %u, band = %u",
            pMac->roam.configParam.uCfgDot11Mode, eBand);
        return eHAL_STATUS_INVALID_PARAMETER;
    }
    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
            "Band changed to %u (0 - ALL, 1 - 2.4 GHZ, 2 - 5GHZ)", eBand);
    pMac->roam.configParam.eBand = eBand;
    pMac->roam.configParam.bandCapability = eBand;
    csrScanGetSupportedChannels( pMac );
#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
    if (!csrRoamIsRoamOffloadScanEnabled(pMac))
        csrUpdateBgScanConfigIniChannelList( pMac, eBand );
#endif
    status = csrInitGetChannels( pMac );
    if (eHAL_STATUS_SUCCESS == status)
        csrInitChannelList( hHal );
    return status;
}


/*                                                                                         
                                                                                   
                                                                                          
                                     
                                                                        
                                                                        
                                                     
                                                                                                      
                     
                    
                     
                                                                                                   
                                  
                                           
                                       
                                       
 */
ePhyChanBondState csrConvertCBIniValueToPhyCBState(v_U32_t cbIniValue)
{

   ePhyChanBondState phyCbState;
   switch (cbIniValue) {
      //               
      case 0:
        phyCbState = PHY_SINGLE_CHANNEL_CENTERED;
        break;
      //              
      case 1:
        phyCbState = PHY_DOUBLE_CHANNEL_HIGH_PRIMARY;
        break;
      //               
      case 2:
        phyCbState = PHY_DOUBLE_CHANNEL_LOW_PRIMARY;
        break;
#ifdef WLAN_FEATURE_11AC
      case 3:
        phyCbState = PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_CENTERED; 
        break;
      case 4:
        phyCbState = PHY_QUADRUPLE_CHANNEL_20MHZ_CENTERED_40MHZ_CENTERED;
        break;
      case 5:
        phyCbState = PHY_QUADRUPLE_CHANNEL_20MHZ_CENTERED_40MHZ_CENTERED;
        break; 
      case 6:
        phyCbState = PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_LOW;
        break;
      case 7:
        phyCbState = PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_LOW;
        break; 
      case 8:
        phyCbState = PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_HIGH;
        break;
      case 9:
        phyCbState = PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_HIGH;
        break; 
#endif 
      default:
        //                                                       
        phyCbState = PHY_SINGLE_CHANNEL_CENTERED;
        break;
   }
   return phyCbState;
}

v_U32_t csrConvertPhyCBStateToIniValue(ePhyChanBondState phyCbState)
{

   v_U32_t cbIniValue;
   switch (phyCbState) {
      //               
      case PHY_SINGLE_CHANNEL_CENTERED:
        cbIniValue = 0;
        break;
      //              
      case PHY_DOUBLE_CHANNEL_HIGH_PRIMARY:
        cbIniValue = 1;
        break;
      //               
      case PHY_DOUBLE_CHANNEL_LOW_PRIMARY:
        cbIniValue = 2;
        break;
#ifdef WLAN_FEATURE_11AC
      case PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_CENTERED:
        cbIniValue = 3;
        break;
      case PHY_QUADRUPLE_CHANNEL_20MHZ_CENTERED_40MHZ_CENTERED:
        cbIniValue = 4;
        break;
      case PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_CENTERED:
        cbIniValue = 5;
        break;
      case PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_LOW:
        cbIniValue = 6;
        break;
      case PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_LOW:
        cbIniValue = 7;
        break;
      case PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_HIGH:
        cbIniValue = 8;
        break;
      case PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_HIGH:
        cbIniValue = 9;
        break;
#endif
      default:
        //                          
        cbIniValue = 10;
        break;
   }
   return cbIniValue;
}

eHalStatus csrChangeDefaultConfigParam(tpAniSirGlobal pMac, tCsrConfigParam *pParam)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;

    if(pParam)
    {
        pMac->roam.configParam.WMMSupportMode = pParam->WMMSupportMode;
        pMac->roam.configParam.Is11eSupportEnabled = pParam->Is11eSupportEnabled;
        pMac->roam.configParam.FragmentationThreshold = pParam->FragmentationThreshold;
        pMac->roam.configParam.Is11dSupportEnabled = pParam->Is11dSupportEnabled;
        pMac->roam.configParam.Is11dSupportEnabledOriginal = pParam->Is11dSupportEnabled;
        pMac->roam.configParam.Is11hSupportEnabled = pParam->Is11hSupportEnabled;

        pMac->roam.configParam.fenableMCCMode = pParam->fEnableMCCMode;
        pMac->roam.configParam.fAllowMCCGODiffBI = pParam->fAllowMCCGODiffBI;
        
        /*                                                   
                                                                                                                                                 
                                                                                                           
         */
        if (pParam->channelBondingMode24GHz > MAX_CB_VALUE_IN_INI)
        {
            smsLog( pMac, LOGW, "Invalid CB value from ini in 2.4GHz band %d, CB DISABLED", pParam->channelBondingMode24GHz);
        }
        pMac->roam.configParam.channelBondingMode24GHz = csrConvertCBIniValueToPhyCBState(pParam->channelBondingMode24GHz);
        if (pParam->channelBondingMode5GHz > MAX_CB_VALUE_IN_INI)
        {
            smsLog( pMac, LOGW, "Invalid CB value from ini in 5GHz band %d, CB DISABLED", pParam->channelBondingMode5GHz);
        }
        pMac->roam.configParam.channelBondingMode5GHz = csrConvertCBIniValueToPhyCBState(pParam->channelBondingMode5GHz);
        pMac->roam.configParam.RTSThreshold = pParam->RTSThreshold;
        pMac->roam.configParam.phyMode = pParam->phyMode;
        pMac->roam.configParam.shortSlotTime = pParam->shortSlotTime;
        pMac->roam.configParam.HeartbeatThresh24 = pParam->HeartbeatThresh24;
        pMac->roam.configParam.HeartbeatThresh50 = pParam->HeartbeatThresh50;
        pMac->roam.configParam.ProprietaryRatesEnabled = pParam->ProprietaryRatesEnabled;
        pMac->roam.configParam.TxRate = pParam->TxRate;
        pMac->roam.configParam.AdHocChannel24 = pParam->AdHocChannel24;
        pMac->roam.configParam.AdHocChannel5G = pParam->AdHocChannel5G;
        pMac->roam.configParam.bandCapability = pParam->bandCapability;
        pMac->roam.configParam.cbChoice = pParam->cbChoice;
        pMac->roam.configParam.bgScanInterval = pParam->bgScanInterval;
        pMac->roam.configParam.disableAggWithBtc = pParam->disableAggWithBtc;
        //                                                    
        //                                 
        if (pParam->nActiveMaxChnTime)
        {
            pMac->roam.configParam.nActiveMaxChnTime = pParam->nActiveMaxChnTime;
        }
        if (pParam->nActiveMinChnTime)
        {
            pMac->roam.configParam.nActiveMinChnTime = pParam->nActiveMinChnTime;
        }
        if (pParam->nPassiveMaxChnTime)
        {
            pMac->roam.configParam.nPassiveMaxChnTime = pParam->nPassiveMaxChnTime;
        }
        if (pParam->nPassiveMinChnTime)
        {
            pMac->roam.configParam.nPassiveMinChnTime = pParam->nPassiveMinChnTime;
        }
        if (pParam->nActiveMaxChnTimeBtc)
        {
            pMac->roam.configParam.nActiveMaxChnTimeBtc = pParam->nActiveMaxChnTimeBtc;
        }
        if (pParam->nActiveMinChnTimeBtc)
        {
            pMac->roam.configParam.nActiveMinChnTimeBtc = pParam->nActiveMinChnTimeBtc;
        }
#ifdef WLAN_AP_STA_CONCURRENCY
        if (pParam->nActiveMaxChnTimeConc)
        {
            pMac->roam.configParam.nActiveMaxChnTimeConc = pParam->nActiveMaxChnTimeConc;
        }
        if (pParam->nActiveMinChnTimeConc)
        {
            pMac->roam.configParam.nActiveMinChnTimeConc = pParam->nActiveMinChnTimeConc;
        }
        if (pParam->nPassiveMaxChnTimeConc)
        {
            pMac->roam.configParam.nPassiveMaxChnTimeConc = pParam->nPassiveMaxChnTimeConc;
        }
        if (pParam->nPassiveMinChnTimeConc)
        {
            pMac->roam.configParam.nPassiveMinChnTimeConc = pParam->nPassiveMinChnTimeConc;
        }
        if (pParam->nRestTimeConc)
        {
            pMac->roam.configParam.nRestTimeConc = pParam->nRestTimeConc;
        }
        if (pParam->nNumStaChanCombinedConc)
        {
            pMac->roam.configParam.nNumStaChanCombinedConc = pParam->nNumStaChanCombinedConc;
        }
        if (pParam->nNumP2PChanCombinedConc)
        {
            pMac->roam.configParam.nNumP2PChanCombinedConc = pParam->nNumP2PChanCombinedConc;
        }
#endif
        //                                                                
        if (pParam->impsSleepTime)
        {
            //                                          
            tANI_U32 impsSleepTime = pParam->impsSleepTime * PAL_TIMER_TO_SEC_UNIT;
            if(CSR_IDLE_SCAN_NO_PS_INTERVAL_MIN <= impsSleepTime)
            {
                pMac->roam.configParam.impsSleepTime = impsSleepTime;
            }
        else
        {
            pMac->roam.configParam.impsSleepTime = CSR_IDLE_SCAN_NO_PS_INTERVAL;
        }
        }
        else
        {
            pMac->roam.configParam.impsSleepTime = 0;
        }
        pMac->roam.configParam.eBand = pParam->eBand;
        pMac->roam.configParam.uCfgDot11Mode = csrGetCfgDot11ModeFromCsrPhyMode(NULL, pMac->roam.configParam.phyMode, 
                                                    pMac->roam.configParam.ProprietaryRatesEnabled);
        //                                                                    
        //                                 
        if (pParam->nScanResultAgeCount)
        {
            pMac->roam.configParam.agingCount = pParam->nScanResultAgeCount;
        }
        if(pParam->scanAgeTimeNCNPS)
        {
            pMac->roam.configParam.scanAgeTimeNCNPS = pParam->scanAgeTimeNCNPS;  
        }
        if(pParam->scanAgeTimeNCPS)
        {
            pMac->roam.configParam.scanAgeTimeNCPS = pParam->scanAgeTimeNCPS;   
        }
        if(pParam->scanAgeTimeCNPS)
        {
            pMac->roam.configParam.scanAgeTimeCNPS = pParam->scanAgeTimeCNPS;   
        }
        if(pParam->scanAgeTimeCPS)
        {
            pMac->roam.configParam.scanAgeTimeCPS = pParam->scanAgeTimeCPS;   
        }
        
        csrAssignRssiForCategory(pMac, CSR_BEST_RSSI_VALUE, pParam->bCatRssiOffset);
        pMac->roam.configParam.nRoamingTime = pParam->nRoamingTime;
        pMac->roam.configParam.fEnforce11dChannels = pParam->fEnforce11dChannels;
        pMac->roam.configParam.fSupplicantCountryCodeHasPriority = pParam->fSupplicantCountryCodeHasPriority;
        pMac->roam.configParam.fEnforceCountryCodeMatch = pParam->fEnforceCountryCodeMatch;
        pMac->roam.configParam.fEnforceDefaultDomain = pParam->fEnforceDefaultDomain;
        pMac->roam.configParam.vccRssiThreshold = pParam->vccRssiThreshold;
        pMac->roam.configParam.vccUlMacLossThreshold = pParam->vccUlMacLossThreshold;
        pMac->roam.configParam.IsIdleScanEnabled = pParam->IsIdleScanEnabled;
        pMac->roam.configParam.statsReqPeriodicity = pParam->statsReqPeriodicity;
        pMac->roam.configParam.statsReqPeriodicityInPS = pParam->statsReqPeriodicityInPS;
        //                                         
        pMac->roam.configParam.nTxPowerCap = pParam->nTxPowerCap;
        if( csrIs11dSupported( pMac ) )
        {
            status = CsrInit11dInfo(pMac, &pParam->Csr11dinfo);
        }
        else
        {
            pMac->scan.curScanType = eSIR_ACTIVE_SCAN;
        }

        /*                                                              
                                                                        */
        if( csrIs11hSupported( pMac ) && !csrIs11dSupported( pMac ) )
        {
            csrInitChannelPowerList(pMac, &pParam->Csr11dinfo);
        }


#ifdef WLAN_FEATURE_VOWIFI_11R
        palCopyMemory( pMac->hHdd, &pMac->roam.configParam.csr11rConfig, &pParam->csr11rConfig, sizeof(tCsr11rConfigParams) );
        smsLog( pMac, LOG1, "IsFTResourceReqSupp = %d", pMac->roam.configParam.csr11rConfig.IsFTResourceReqSupported);
#endif
#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
        pMac->roam.configParam.isFastTransitionEnabled = pParam->isFastTransitionEnabled;
        pMac->roam.configParam.RoamRssiDiff = pParam->RoamRssiDiff;
        pMac->roam.configParam.nImmediateRoamRssiDiff = pParam->nImmediateRoamRssiDiff;
        smsLog( pMac, LOG1, "nImmediateRoamRssiDiff = %d",
                pMac->roam.configParam.nImmediateRoamRssiDiff );
        pMac->roam.configParam.nRoamPrefer5GHz = pParam->nRoamPrefer5GHz;
        pMac->roam.configParam.nRoamIntraBand = pParam->nRoamIntraBand;
        pMac->roam.configParam.isWESModeEnabled = pParam->isWESModeEnabled;
        pMac->roam.configParam.nProbes = pParam->nProbes;
        pMac->roam.configParam.nRoamScanHomeAwayTime = pParam->nRoamScanHomeAwayTime;
#endif
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
        pMac->roam.configParam.isRoamOffloadScanEnabled = pParam->isRoamOffloadScanEnabled;
        pMac->roam.configParam.bFastRoamInConIniFeatureEnabled = pParam->bFastRoamInConIniFeatureEnabled;
#endif
#ifdef FEATURE_WLAN_LFR
        pMac->roam.configParam.isFastRoamIniFeatureEnabled = pParam->isFastRoamIniFeatureEnabled;
        pMac->roam.configParam.MAWCEnabled = pParam->MAWCEnabled;
#endif

#ifdef FEATURE_WLAN_CCX
        pMac->roam.configParam.isCcxIniFeatureEnabled = pParam->isCcxIniFeatureEnabled;
#endif
#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
        palCopyMemory( pMac->hHdd, &pMac->roam.configParam.neighborRoamConfig, &pParam->neighborRoamConfig, sizeof(tCsrNeighborRoamConfigParams) );
        smsLog( pMac, LOG1, "nNeighborScanTimerPerioid = %d", pMac->roam.configParam.neighborRoamConfig.nNeighborScanTimerPeriod);
        smsLog( pMac, LOG1, "nNeighborReassocRssiThreshold = %d", pMac->roam.configParam.neighborRoamConfig.nNeighborReassocRssiThreshold);
        smsLog( pMac, LOG1, "nNeighborLookupRssiThreshold = %d", pMac->roam.configParam.neighborRoamConfig.nNeighborLookupRssiThreshold);
        smsLog( pMac, LOG1, "nNeighborScanMinChanTime = %d", pMac->roam.configParam.neighborRoamConfig.nNeighborScanMinChanTime);
        smsLog( pMac, LOG1, "nNeighborScanMaxChanTime = %d", pMac->roam.configParam.neighborRoamConfig.nNeighborScanMaxChanTime);
        smsLog( pMac, LOG1, "nMaxNeighborRetries = %d", pMac->roam.configParam.neighborRoamConfig.nMaxNeighborRetries);
        smsLog( pMac, LOG1, "nNeighborResultsRefreshPeriod = %d", pMac->roam.configParam.neighborRoamConfig.nNeighborResultsRefreshPeriod);
        smsLog( pMac, LOG1, "nEmptyScanRefreshPeriod = %d", pMac->roam.configParam.neighborRoamConfig.nEmptyScanRefreshPeriod);
        {
           int i;
           smsLog( pMac, LOG1, FL("Num of Channels in CFG Channel List: %d"), pMac->roam.configParam.neighborRoamConfig.neighborScanChanList.numChannels);
           for( i=0; i< pMac->roam.configParam.neighborRoamConfig.neighborScanChanList.numChannels; i++)
           {
              smsLog( pMac, LOG1, "%d ", pMac->roam.configParam.neighborRoamConfig.neighborScanChanList.channelList[i] );
           }
           smsLog( pMac, LOG1, "");
        }
#endif
        pMac->roam.configParam.addTSWhenACMIsOff = pParam->addTSWhenACMIsOff;
        pMac->scan.fValidateList = pParam->fValidateList;
        pMac->scan.fEnableBypass11d = pParam->fEnableBypass11d;
        pMac->scan.fEnableDFSChnlScan = pParam->fEnableDFSChnlScan;
        pMac->scan.scanResultCfgAgingTime = pParam->scanCfgAgingTime;
        pMac->roam.configParam.fScanTwice = pParam->fScanTwice;
        pMac->scan.fFirstScanOnly2GChnl = pParam->fFirstScanOnly2GChnl;
        /*                                                                                                        
                                                                                                      
                                                                                                                            
                                                                                                                          
                         
         */
        //                                                      
        //                          
        pMac->roam.configParam.doBMPSWorkaround = 0;

#ifdef WLAN_FEATURE_11AC
        pMac->roam.configParam.nVhtChannelWidth = pParam->nVhtChannelWidth;
        pMac->roam.configParam.txBFEnable= pParam->enableTxBF;
        pMac->roam.configParam.txBFCsnValue = pParam->txBFCsnValue;
        pMac->roam.configParam.enableVhtFor24GHz = pParam->enableVhtFor24GHz;
#endif
        pMac->roam.configParam.txLdpcEnable = pParam->enableTxLdpc;

        pMac->roam.configParam.enableOxygenNwk = pParam->enableOxygenNwk;

        pMac->roam.configParam.isAmsduSupportInAMPDU = pParam->isAmsduSupportInAMPDU;
        pMac->roam.configParam.nSelect5GHzMargin = pParam->nSelect5GHzMargin;
    }
    
    return status;
}

eHalStatus csrGetConfigParam(tpAniSirGlobal pMac, tCsrConfigParam *pParam)
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
    if(pParam)
    {
        pParam->WMMSupportMode = pMac->roam.configParam.WMMSupportMode;
        pParam->Is11eSupportEnabled = pMac->roam.configParam.Is11eSupportEnabled;
        pParam->FragmentationThreshold = pMac->roam.configParam.FragmentationThreshold;
        pParam->Is11dSupportEnabled = pMac->roam.configParam.Is11dSupportEnabled;
        pParam->Is11dSupportEnabledOriginal = pMac->roam.configParam.Is11dSupportEnabledOriginal;
        pParam->Is11hSupportEnabled = pMac->roam.configParam.Is11hSupportEnabled;
        pParam->channelBondingMode24GHz = csrConvertPhyCBStateToIniValue(pMac->roam.configParam.channelBondingMode24GHz);
        pParam->channelBondingMode5GHz = csrConvertPhyCBStateToIniValue(pMac->roam.configParam.channelBondingMode5GHz);
        pParam->RTSThreshold = pMac->roam.configParam.RTSThreshold;
        pParam->phyMode = pMac->roam.configParam.phyMode;
        pParam->shortSlotTime = pMac->roam.configParam.shortSlotTime;
        pParam->HeartbeatThresh24 = pMac->roam.configParam.HeartbeatThresh24;
        pParam->HeartbeatThresh50 = pMac->roam.configParam.HeartbeatThresh50;
        pParam->ProprietaryRatesEnabled = pMac->roam.configParam.ProprietaryRatesEnabled;
        pParam->TxRate = pMac->roam.configParam.TxRate;
        pParam->AdHocChannel24 = pMac->roam.configParam.AdHocChannel24;
        pParam->AdHocChannel5G = pMac->roam.configParam.AdHocChannel5G;
        pParam->bandCapability = pMac->roam.configParam.bandCapability;
        pParam->cbChoice = pMac->roam.configParam.cbChoice;
        pParam->bgScanInterval = pMac->roam.configParam.bgScanInterval;
        pParam->nActiveMaxChnTime = pMac->roam.configParam.nActiveMaxChnTime;
        pParam->nActiveMinChnTime = pMac->roam.configParam.nActiveMinChnTime;
        pParam->nPassiveMaxChnTime = pMac->roam.configParam.nPassiveMaxChnTime;
        pParam->nPassiveMinChnTime = pMac->roam.configParam.nPassiveMinChnTime;
        pParam->nActiveMaxChnTimeBtc = pMac->roam.configParam.nActiveMaxChnTimeBtc;
        pParam->nActiveMinChnTimeBtc = pMac->roam.configParam.nActiveMinChnTimeBtc;
        pParam->disableAggWithBtc = pMac->roam.configParam.disableAggWithBtc;
#ifdef WLAN_AP_STA_CONCURRENCY
        pParam->nActiveMaxChnTimeConc = pMac->roam.configParam.nActiveMaxChnTimeConc;
        pParam->nActiveMinChnTimeConc = pMac->roam.configParam.nActiveMinChnTimeConc;
        pParam->nPassiveMaxChnTimeConc = pMac->roam.configParam.nPassiveMaxChnTimeConc;
        pParam->nPassiveMinChnTimeConc = pMac->roam.configParam.nPassiveMinChnTimeConc;
        pParam->nRestTimeConc = pMac->roam.configParam.nRestTimeConc;
        pParam->nNumStaChanCombinedConc = pMac->roam.configParam.nNumStaChanCombinedConc;
        pParam->nNumP2PChanCombinedConc = pMac->roam.configParam.nNumP2PChanCombinedConc;
#endif
        //                                          
        pParam->impsSleepTime = pMac->roam.configParam.impsSleepTime / PAL_TIMER_TO_SEC_UNIT;
        pParam->eBand = pMac->roam.configParam.eBand;
        pParam->nScanResultAgeCount = pMac->roam.configParam.agingCount;
        pParam->scanAgeTimeNCNPS = pMac->roam.configParam.scanAgeTimeNCNPS;  
        pParam->scanAgeTimeNCPS = pMac->roam.configParam.scanAgeTimeNCPS;   
        pParam->scanAgeTimeCNPS = pMac->roam.configParam.scanAgeTimeCNPS;   
        pParam->scanAgeTimeCPS = pMac->roam.configParam.scanAgeTimeCPS;   
        pParam->bCatRssiOffset = pMac->roam.configParam.bCatRssiOffset;
        pParam->nRoamingTime = pMac->roam.configParam.nRoamingTime;
        pParam->fEnforce11dChannels = pMac->roam.configParam.fEnforce11dChannels;
        pParam->fSupplicantCountryCodeHasPriority = pMac->roam.configParam.fSupplicantCountryCodeHasPriority;
        pParam->fEnforceCountryCodeMatch = pMac->roam.configParam.fEnforceCountryCodeMatch;
        pParam->fEnforceDefaultDomain = pMac->roam.configParam.fEnforceDefaultDomain;        
        pParam->vccRssiThreshold = pMac->roam.configParam.vccRssiThreshold;
        pParam->vccUlMacLossThreshold = pMac->roam.configParam.vccUlMacLossThreshold;
        pParam->IsIdleScanEnabled = pMac->roam.configParam.IsIdleScanEnabled;
        pParam->nTxPowerCap = pMac->roam.configParam.nTxPowerCap;
        pParam->statsReqPeriodicity = pMac->roam.configParam.statsReqPeriodicity;
        pParam->statsReqPeriodicityInPS = pMac->roam.configParam.statsReqPeriodicityInPS;
        pParam->addTSWhenACMIsOff = pMac->roam.configParam.addTSWhenACMIsOff;
        pParam->fValidateList = pMac->roam.configParam.fValidateList;
        pParam->fEnableBypass11d = pMac->scan.fEnableBypass11d;
        pParam->fEnableDFSChnlScan = pMac->scan.fEnableDFSChnlScan;
        pParam->fScanTwice = pMac->roam.configParam.fScanTwice;
        pParam->fFirstScanOnly2GChnl = pMac->scan.fFirstScanOnly2GChnl;
        pParam->fEnableMCCMode = pMac->roam.configParam.fenableMCCMode;
        pParam->fAllowMCCGODiffBI = pMac->roam.configParam.fAllowMCCGODiffBI;
        pParam->scanCfgAgingTime = pMac->scan.scanResultCfgAgingTime;

#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
        palCopyMemory( pMac->hHdd, &pParam->neighborRoamConfig, &pMac->roam.configParam.neighborRoamConfig, sizeof(tCsrNeighborRoamConfigParams) );
#endif
#ifdef WLAN_FEATURE_11AC
        pParam->nVhtChannelWidth = pMac->roam.configParam.nVhtChannelWidth;
        pParam->enableTxBF = pMac->roam.configParam.txBFEnable;
        pParam->txBFCsnValue = pMac->roam.configParam.txBFCsnValue;
#endif
#ifdef WLAN_FEATURE_VOWIFI_11R
        palCopyMemory( pMac->hHdd, &pMac->roam.configParam.csr11rConfig, &pParam->csr11rConfig, sizeof(tCsr11rConfigParams) );
#endif
#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_LFR)
        pParam->isFastTransitionEnabled = pMac->roam.configParam.isFastTransitionEnabled;
        pParam->RoamRssiDiff = pMac->roam.configParam.RoamRssiDiff;
        pParam->nImmediateRoamRssiDiff = pMac->roam.configParam.nImmediateRoamRssiDiff;
        pParam->nRoamPrefer5GHz = pMac->roam.configParam.nRoamPrefer5GHz;
        pParam->nRoamIntraBand = pMac->roam.configParam.nRoamIntraBand;
        pParam->isWESModeEnabled = pMac->roam.configParam.isWESModeEnabled;
        pParam->nProbes = pMac->roam.configParam.nProbes;
        pParam->nRoamScanHomeAwayTime = pMac->roam.configParam.nRoamScanHomeAwayTime;
#endif
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
        pParam->isRoamOffloadScanEnabled = pMac->roam.configParam.isRoamOffloadScanEnabled;
        pParam->bFastRoamInConIniFeatureEnabled = pMac->roam.configParam.bFastRoamInConIniFeatureEnabled;
#endif
#ifdef FEATURE_WLAN_LFR
        pParam->isFastRoamIniFeatureEnabled = pMac->roam.configParam.isFastRoamIniFeatureEnabled;
#endif

#ifdef FEATURE_WLAN_CCX
        pParam->isCcxIniFeatureEnabled = pMac->roam.configParam.isCcxIniFeatureEnabled;
#endif
#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
        palCopyMemory( pMac->hHdd, &pParam->neighborRoamConfig, &pMac->roam.configParam.neighborRoamConfig, sizeof(tCsrNeighborRoamConfigParams) );
        {
           int i;
           smsLog( pMac, LOG1, FL("Num of Channels in CFG Channel List: %d"), pMac->roam.configParam.neighborRoamConfig.neighborScanChanList.numChannels);
           for( i=0; i< pMac->roam.configParam.neighborRoamConfig.neighborScanChanList.numChannels; i++)
           {
              smsLog( pMac, LOG1, "%d ", pMac->roam.configParam.neighborRoamConfig.neighborScanChanList.channelList[i] );
           }
           smsLog( pMac, LOG1, "");
        }
#endif

        pParam->enableTxLdpc = pMac->roam.configParam.txLdpcEnable;
        pParam->enableOxygenNwk = pMac->roam.configParam.enableOxygenNwk;

        pParam->isAmsduSupportInAMPDU = pMac->roam.configParam.isAmsduSupportInAMPDU;
        pParam->nSelect5GHzMargin = pMac->roam.configParam.nSelect5GHzMargin;

        csrSetChannels(pMac, pParam);

        status = eHAL_STATUS_SUCCESS;
    }
    return (status);
}

eHalStatus csrSetPhyMode(tHalHandle hHal, tANI_U32 phyMode, eCsrBand eBand, tANI_BOOLEAN *pfRestartNeeded)
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
    tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
    tANI_BOOLEAN fRestartNeeded = eANI_BOOLEAN_FALSE;
    eCsrPhyMode newPhyMode = eCSR_DOT11_MODE_AUTO;
    do
    {
        if(eCSR_BAND_24 == eBand)
        {
            if(CSR_IS_RADIO_A_ONLY(pMac)) break;
            if((eCSR_DOT11_MODE_11a & phyMode) || (eCSR_DOT11_MODE_11a_ONLY & phyMode)) break;
        }
        if(eCSR_BAND_5G == eBand)
        {
            if(CSR_IS_RADIO_BG_ONLY(pMac)) break;
            if((eCSR_DOT11_MODE_11b & phyMode) || (eCSR_DOT11_MODE_11b_ONLY & phyMode) ||
                (eCSR_DOT11_MODE_11g & phyMode) || (eCSR_DOT11_MODE_11g_ONLY & phyMode) 
                ) 
            {
                break;
            }
        }
        if((0 == phyMode) || (eCSR_DOT11_MODE_TAURUS & phyMode))
        {
            newPhyMode = eCSR_DOT11_MODE_TAURUS;
        }
        else if(eCSR_DOT11_MODE_AUTO & phyMode)
        {
            newPhyMode = eCSR_DOT11_MODE_AUTO;
        }
        else
        {
            //                                               
            if(eCSR_DOT11_MODE_11n_ONLY & phyMode)
            {
                if(eCSR_DOT11_MODE_11n_ONLY != phyMode) break;
                newPhyMode = eCSR_DOT11_MODE_11n_ONLY;
            }
            else if(eCSR_DOT11_MODE_11a_ONLY & phyMode)
            {
                if(eCSR_DOT11_MODE_11a_ONLY != phyMode) break;
                if(eCSR_BAND_24 == eBand) break;
                newPhyMode = eCSR_DOT11_MODE_11a_ONLY;
                eBand = eCSR_BAND_5G;
            }
            else if(eCSR_DOT11_MODE_11g_ONLY & phyMode)
            {
                if(eCSR_DOT11_MODE_11g_ONLY != phyMode) break;
                if(eCSR_BAND_5G == eBand) break;
                newPhyMode = eCSR_DOT11_MODE_11g_ONLY;
                eBand = eCSR_BAND_24;
            }
            else if(eCSR_DOT11_MODE_11b_ONLY & phyMode)
            {
                if(eCSR_DOT11_MODE_11b_ONLY != phyMode) break;
                if(eCSR_BAND_5G == eBand) break;
                newPhyMode = eCSR_DOT11_MODE_11b_ONLY;
                eBand = eCSR_BAND_24;
            }
            else if(eCSR_DOT11_MODE_11n & phyMode)
            {
                newPhyMode = eCSR_DOT11_MODE_11n;
            }
            else if(eCSR_DOT11_MODE_abg & phyMode)
            {
                newPhyMode = eCSR_DOT11_MODE_abg;
            }
            else if(eCSR_DOT11_MODE_11a & phyMode)
            {
                if((eCSR_DOT11_MODE_11g & phyMode) || (eCSR_DOT11_MODE_11b & phyMode))
                {
                    if(eCSR_BAND_ALL == eBand)
                    {
                        newPhyMode = eCSR_DOT11_MODE_abg;
                    }
                    else
                    {
                        //           
                        break;
                    }
                }
                else
                {
                    newPhyMode = eCSR_DOT11_MODE_11a;
                    eBand = eCSR_BAND_5G;
                }
            }
            else if(eCSR_DOT11_MODE_11g & phyMode)
            {
                newPhyMode = eCSR_DOT11_MODE_11g;
                eBand = eCSR_BAND_24;
            }
            else if(eCSR_DOT11_MODE_11b & phyMode)
            {
                newPhyMode = eCSR_DOT11_MODE_11b;
                eBand = eCSR_BAND_24;
            }
            else
            {
                //                     
                smsLog( pMac, LOGE, FL(" cannot recognize the phy mode 0x%08X"), phyMode );
                newPhyMode = eCSR_DOT11_MODE_AUTO;
            }
        }
        //               
        status = eHAL_STATUS_SUCCESS;
        //                                                 
        if(eBand != pMac->roam.configParam.eBand)
        {
            fRestartNeeded = eANI_BOOLEAN_TRUE;
            break;
        }
        if(newPhyMode != pMac->roam.configParam.phyMode)
        {
            fRestartNeeded = eANI_BOOLEAN_TRUE;
            break;
        }
    }while(0);
    if(HAL_STATUS_SUCCESS(status))
    {
        pMac->roam.configParam.eBand = eBand;
        pMac->roam.configParam.phyMode = newPhyMode;
        if(pfRestartNeeded)
        {
            *pfRestartNeeded = fRestartNeeded;
        }
    }
    return (status);
}
    
void csrPruneChannelListForMode( tpAniSirGlobal pMac, tCsrChannel *pChannelList )
{
    tANI_U8 Index;
    tANI_U8 cChannels;
    //                                                            
    if ( !CSR_IS_OPEARTING_DUAL_BAND( pMac ) )
    {
        //                                                                  
        //                             
        if ( CSR_IS_24_BAND_ONLY( pMac ) )
        {
            for( Index = 0, cChannels = 0; Index < pChannelList->numChannels;
                 Index++ )
            {
                if ( CSR_IS_CHANNEL_24GHZ(pChannelList->channelList[ Index ]) )
                {
                    pChannelList->channelList[ cChannels ] = pChannelList->channelList[ Index ];
                    cChannels++;
                }
            }
            //                                                                                     
            //                                                                                       
            //                                                                                           
            //                    
            //
            //                                                                         
            //                                                                                      
           
            if ( pChannelList->numChannels > cChannels )
            {
                palZeroMemory( pMac->hHdd, &pChannelList->channelList[ cChannels ],
                               sizeof( pChannelList->channelList[ 0 ] ) * ( pChannelList->numChannels - cChannels ) );
                
            }
            
            pChannelList->numChannels = cChannels;
        }
        else if ( CSR_IS_5G_BAND_ONLY( pMac ) )
        {
            for ( Index = 0, cChannels = 0; Index < pChannelList->numChannels; Index++ )
            {
                if ( CSR_IS_CHANNEL_5GHZ(pChannelList->channelList[ Index ]) )
                {
                    pChannelList->channelList[ cChannels ] = pChannelList->channelList[ Index ];
                    cChannels++;
                }
            }
            //                                                                                     
            //                                                                                       
            //                                                                                           
            //                    
            //
            //                                                                         
            //                                                                                      
            if ( pChannelList->numChannels > cChannels )
            {
                palZeroMemory( pMac->hHdd, &pChannelList->channelList[ cChannels ],
                               sizeof( pChannelList->channelList[ 0 ] ) * ( pChannelList->numChannels - cChannels ) );
            }            
                               
            pChannelList->numChannels = cChannels;
        }
    }
}
#define INFRA_AP_DEFAULT_CHANNEL 6
eHalStatus csrIsValidChannel(tpAniSirGlobal pMac, tANI_U8 chnNum)
{
    tANI_U8 index= 0;
    eHalStatus status = eHAL_STATUS_FAILURE;
    for (index=0; index < pMac->scan.base20MHzChannels.numChannels ;index++)
    {
        if(pMac->scan.base20MHzChannels.channelList[ index ] == chnNum){
            status = eHAL_STATUS_SUCCESS;
            break;
        }
    }
    return status;
}

eHalStatus csrInitGetChannels(tpAniSirGlobal pMac)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_U8 num20MHzChannelsFound = 0;
    VOS_STATUS vosStatus;
    tANI_U8 Index = 0;
    tANI_U8 num40MHzChannelsFound = 0;
    
    
    //                                                              
    //                                                                                                       
    //                                                                   
    vosStatus = vos_nv_getChannelListWithPower( pMac->scan.defaultPowerTable, &num20MHzChannelsFound, 
                        pMac->scan.defaultPowerTable40MHz, &num40MHzChannelsFound);
    if ( (VOS_STATUS_SUCCESS != vosStatus) || (num20MHzChannelsFound == 0) )
    {
        smsLog( pMac, LOGE, FL("failed to get channels "));
        status = eHAL_STATUS_FAILURE;
    }
    else
    {
        if ( num20MHzChannelsFound > WNI_CFG_VALID_CHANNEL_LIST_LEN )
        {
            num20MHzChannelsFound = WNI_CFG_VALID_CHANNEL_LIST_LEN;
        }
        pMac->scan.numChannelsDefault = num20MHzChannelsFound;
        //                                         
        //                                                
        for ( Index = 0; Index < num20MHzChannelsFound; Index++)
        {
            pMac->scan.base20MHzChannels.channelList[ Index ] = pMac->scan.defaultPowerTable[ Index ].chanId;
        }
        pMac->scan.base20MHzChannels.numChannels = num20MHzChannelsFound;
        if(num40MHzChannelsFound > WNI_CFG_VALID_CHANNEL_LIST_LEN)
        {
            num40MHzChannelsFound = WNI_CFG_VALID_CHANNEL_LIST_LEN;
        }
        for ( Index = 0; Index < num40MHzChannelsFound; Index++)
        {
            pMac->scan.base40MHzChannels.channelList[ Index ] = pMac->scan.defaultPowerTable40MHz[ Index ].chanId;
        }
        pMac->scan.base40MHzChannels.numChannels = num40MHzChannelsFound;
    }
    return (status);  
}

eHalStatus csrInitChannelList( tHalHandle hHal )
{
    tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
    eHalStatus status = eHAL_STATUS_SUCCESS;
    csrPruneChannelListForMode(pMac, &pMac->scan.baseChannels);
    csrPruneChannelListForMode(pMac, &pMac->scan.base20MHzChannels);
    csrSaveChannelPowerForBand(pMac, eANI_BOOLEAN_FALSE);
    csrSaveChannelPowerForBand(pMac, eANI_BOOLEAN_TRUE);
    //                                                                     
    csrApplyChannelPowerCountryInfo( pMac, &pMac->scan.base20MHzChannels, pMac->scan.countryCodeCurrent, eANI_BOOLEAN_TRUE );
 
    return (status);
}
eHalStatus csrChangeConfigParams(tpAniSirGlobal pMac, 
                                 tCsrUpdateConfigParam *pUpdateConfigParam)
{
   eHalStatus status = eHAL_STATUS_FAILURE;
   tCsr11dinfo *ps11dinfo = NULL;
   ps11dinfo = &pUpdateConfigParam->Csr11dinfo;
   status = CsrInit11dInfo(pMac, ps11dinfo);
   return status;
}

static eHalStatus CsrInit11dInfo(tpAniSirGlobal pMac, tCsr11dinfo *ps11dinfo)
{
  eHalStatus status = eHAL_STATUS_FAILURE;
  tANI_U8  index;
  tANI_U32 count=0;
  tSirMacChanInfo *pChanInfo;
  tSirMacChanInfo *pChanInfoStart;
  tANI_BOOLEAN applyConfig = TRUE;

  if(!ps11dinfo)
  {
     return (status);
  }
  if ( ps11dinfo->Channels.numChannels && ( WNI_CFG_VALID_CHANNEL_LIST_LEN >= ps11dinfo->Channels.numChannels ) ) 
  {
    pMac->scan.base20MHzChannels.numChannels = ps11dinfo->Channels.numChannels;
    status = palCopyMemory(pMac->hHdd, pMac->scan.base20MHzChannels.channelList, 
                           ps11dinfo->Channels.channelList, ps11dinfo->Channels.numChannels);
    if(!HAL_STATUS_SUCCESS(status)) return (status);
  }
  else
  {
     //         
     return (eHAL_STATUS_SUCCESS);
  }
  //                  
  status = palCopyMemory(pMac->hHdd, pMac->scan.countryCodeDefault, 
                         ps11dinfo->countryCode, WNI_CFG_COUNTRY_CODE_LEN);
  if(!HAL_STATUS_SUCCESS(status)) return (status);
  //                                                                             
  //                                                                  
  if(0 == pMac->scan.countryCodeCurrent[0])
  {
     status = palCopyMemory(pMac->hHdd, pMac->scan.countryCodeCurrent, 
                         ps11dinfo->countryCode, WNI_CFG_COUNTRY_CODE_LEN);
     if(!HAL_STATUS_SUCCESS(status)) return (status);
  }
  //                                       
  if(HAL_STATUS_SUCCESS(palAllocateMemory(pMac->hHdd, (void **)&pChanInfo, sizeof(tSirMacChanInfo) * WNI_CFG_VALID_CHANNEL_LIST_LEN)))
  {
      palZeroMemory(pMac->hHdd, pChanInfo, sizeof(tSirMacChanInfo) * WNI_CFG_VALID_CHANNEL_LIST_LEN);
      pChanInfoStart = pChanInfo;
      for(index = 0; index < ps11dinfo->Channels.numChannels; index++)
      {
        pChanInfo->firstChanNum = ps11dinfo->ChnPower[index].firstChannel;
        pChanInfo->numChannels  = ps11dinfo->ChnPower[index].numChannels;
        pChanInfo->maxTxPower   = CSR_ROAM_MIN( ps11dinfo->ChnPower[index].maxtxPower, pMac->roam.configParam.nTxPowerCap );
        pChanInfo++;
        count++;
      }
      if(count)
      {
          csrSaveToChannelPower2G_5G( pMac, count * sizeof(tSirMacChanInfo), pChanInfoStart );
      }
      palFreeMemory(pMac->hHdd, pChanInfoStart);
  }
  //                                                                                   
  if( HAL_STATUS_SUCCESS(status) )
  {
      for( index = 0; index < CSR_ROAM_SESSION_MAX; index++ )
      {
          if((CSR_IS_SESSION_VALID(pMac, index)) && CSR_IS_ROAM_STOP(pMac, index))
          {
              applyConfig = FALSE;
          }
    }

    if(TRUE == applyConfig)
    {
        //                                                                     
        csrApplyChannelPowerCountryInfo( pMac, &pMac->scan.base20MHzChannels, pMac->scan.countryCodeCurrent, eANI_BOOLEAN_TRUE );
    }

  }
  return (status);
}
/*                                                                       */
eHalStatus csrInitChannelPowerList( tpAniSirGlobal pMac, tCsr11dinfo *ps11dinfo)
{
  tANI_U8  index;
  tANI_U32 count=0;
  tSirMacChanInfo *pChanInfo;
  tSirMacChanInfo *pChanInfoStart;

  if(!ps11dinfo || !pMac)
  {
     return eHAL_STATUS_FAILURE;
  }

  if(HAL_STATUS_SUCCESS(palAllocateMemory(pMac->hHdd, (void **)&pChanInfo, sizeof(tSirMacChanInfo) * WNI_CFG_VALID_CHANNEL_LIST_LEN)))
  {
      palZeroMemory(pMac->hHdd, pChanInfo, sizeof(tSirMacChanInfo) * WNI_CFG_VALID_CHANNEL_LIST_LEN);
      pChanInfoStart = pChanInfo;

      for(index = 0; index < ps11dinfo->Channels.numChannels; index++)
      {
        pChanInfo->firstChanNum = ps11dinfo->ChnPower[index].firstChannel;
        pChanInfo->numChannels  = ps11dinfo->ChnPower[index].numChannels;
        pChanInfo->maxTxPower   = CSR_ROAM_MIN( ps11dinfo->ChnPower[index].maxtxPower, pMac->roam.configParam.nTxPowerCap );
        pChanInfo++;
        count++;
      }
      if(count)
      {
          csrSaveToChannelPower2G_5G( pMac, count * sizeof(tSirMacChanInfo), pChanInfoStart );
      }
      palFreeMemory(pMac->hHdd, pChanInfoStart);
  }

  return eHAL_STATUS_SUCCESS;
}

//                    
//                                                                                               
void csrRoamRemoveDuplicateCommand(tpAniSirGlobal pMac, tANI_U32 sessionId, tSmeCmd *pCommand, eCsrRoamReason eRoamReason)
{
    tListElem *pEntry, *pNextEntry;
    tSmeCmd *pDupCommand;
    tDblLinkList localList;

    vos_mem_zero(&localList, sizeof(tDblLinkList));
    if(!HAL_STATUS_SUCCESS(csrLLOpen(pMac->hHdd, &localList)))
    {
        smsLog(pMac, LOGE, FL(" failed to open list"));
        return;
    }
    csrLLLock( &pMac->sme.smeCmdPendingList );
    pEntry = csrLLPeekHead( &pMac->sme.smeCmdPendingList, LL_ACCESS_NOLOCK );
    while( pEntry )
    {
        pNextEntry = csrLLNext( &pMac->sme.smeCmdPendingList, pEntry, LL_ACCESS_NOLOCK );
        pDupCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
        //                                 
        //                                                     
        //                                            
        //                                          
        //                                                                      
        if ( 
            (pCommand && ( pCommand->sessionId == pDupCommand->sessionId ) &&
                ((pCommand->command == pDupCommand->command) &&
                /*                                                       
                                                                                 
                                                   
                 */
                (vos_mem_compare(pDupCommand->u.roamCmd.peerMac, pCommand->u.roamCmd.peerMac, sizeof(v_MACADDR_t))) &&
                 (pCommand->u.roamCmd.roamReason == pDupCommand->u.roamCmd.roamReason ||
                    eCsrForcedDisassoc == pCommand->u.roamCmd.roamReason ||
                    eCsrHddIssued == pCommand->u.roamCmd.roamReason))) 
                ||
            //                          
            ( (sessionId == pDupCommand->sessionId) &&
              (eSmeCommandRoam == pDupCommand->command) &&
                 ((eCsrForcedDisassoc == eRoamReason) ||
                    (eCsrHddIssued == eRoamReason))
               )
           )
        {
            smsLog(pMac, LOGW, FL("   roamReason = %d"), pDupCommand->u.roamCmd.roamReason);
            //                                                         
            if(csrLLRemoveEntry( &pMac->sme.smeCmdPendingList, pEntry, LL_ACCESS_NOLOCK ))
            {
                csrLLInsertTail(&localList, pEntry, LL_ACCESS_NOLOCK);
            }
        }
        pEntry = pNextEntry;
    }
    csrLLUnlock( &pMac->sme.smeCmdPendingList );

    while( (pEntry = csrLLRemoveHead(&localList, LL_ACCESS_NOLOCK)) )
    {
        pDupCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
        //                                         
        csrRoamCallCallback(pMac, pDupCommand->sessionId, NULL, pDupCommand->u.roamCmd.roamId,
                                            eCSR_ROAM_CANCELLED, eCSR_ROAM_RESULT_NONE);
        csrReleaseCommandRoam(pMac, pDupCommand);
    }
    csrLLClose(&localList);
}
eHalStatus csrRoamCallCallback(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamInfo *pRoamInfo, 
                               tANI_U32 roamId, eRoamCmdStatus u1, eCsrRoamResult u2)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
    WLAN_VOS_DIAG_EVENT_DEF(connectionStatus, vos_event_wlan_status_payload_type);
#endif
    tCsrRoamSession *pSession;
    if( CSR_IS_SESSION_VALID( pMac, sessionId) )
    {
        pSession = CSR_GET_SESSION( pMac, sessionId );
    }
    else
    {
       smsLog(pMac, LOGE, "Session ID:%d is not valid", sessionId);
       VOS_ASSERT(0);
       return eHAL_STATUS_FAILURE;
    }

    if(eCSR_ROAM_ASSOCIATION_COMPLETION == u1 && pRoamInfo)
    {
        smsLog(pMac, LOGW, " Assoc complete result = %d statusCode = %d reasonCode = %d", u2, pRoamInfo->statusCode, pRoamInfo->reasonCode);
    }
    if ((u1 == eCSR_ROAM_FT_REASSOC_FAILED) && (pSession->bRefAssocStartCnt)) {
        /*
                                                              
                                                          
                                                              
                                                             
                                                           
                                                                   
                                                               
                                                
         */
        pSession->bRefAssocStartCnt--;
    }

    if ( (pSession == NULL) ||
        (eANI_BOOLEAN_FALSE == pSession->sessionActive) )
    {
        smsLog(pMac, LOG1, "Session ID is not valid");
        return eHAL_STATUS_FAILURE;
    }

    if(NULL != pSession->callback)
    {
        if( pRoamInfo )
        {
            pRoamInfo->sessionId = (tANI_U8)sessionId;
        }

        /*                                                                                     
                                                                                        
                                                                                          
                 
                                                                                                       
                                                         */
        //                                                    
        status = pSession->callback(pSession->pContext, pRoamInfo, roamId, u1, u2);
        //                                                    
    }
    //                                                     
    //                                                                
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR    
    palZeroMemory(pMac->hHdd, &connectionStatus, sizeof(vos_event_wlan_status_payload_type));

    if((eCSR_ROAM_ASSOCIATION_COMPLETION == u1) && (eCSR_ROAM_RESULT_ASSOCIATED == u2) && pRoamInfo)
    {
       connectionStatus.eventId = eCSR_WLAN_STATUS_CONNECT;
       connectionStatus.bssType = pRoamInfo->u.pConnectedProfile->BSSType;
       if(NULL != pRoamInfo->pBssDesc)
       {
          connectionStatus.rssi = pRoamInfo->pBssDesc->rssi * (-1);
          connectionStatus.channel = pRoamInfo->pBssDesc->channelId;
       }
       connectionStatus.qosCapability = pRoamInfo->u.pConnectedProfile->qosConnection;
       connectionStatus.authType = (v_U8_t)diagAuthTypeFromCSRType(pRoamInfo->u.pConnectedProfile->AuthType);
       connectionStatus.encryptionType = (v_U8_t)diagEncTypeFromCSRType(pRoamInfo->u.pConnectedProfile->EncryptionType);
       palCopyMemory(pMac->hHdd, connectionStatus.ssid, pRoamInfo->u.pConnectedProfile->SSID.ssId, 6);
       connectionStatus.reason = eCSR_REASON_UNSPECIFIED;
       WLAN_VOS_DIAG_EVENT_REPORT(&connectionStatus, EVENT_WLAN_STATUS);
    }
    if((eCSR_ROAM_MIC_ERROR_IND == u1) || (eCSR_ROAM_RESULT_MIC_FAILURE == u2))
    {
       connectionStatus.eventId = eCSR_WLAN_STATUS_DISCONNECT;
       connectionStatus.reason = eCSR_REASON_MIC_ERROR;
       WLAN_VOS_DIAG_EVENT_REPORT(&connectionStatus, EVENT_WLAN_STATUS);
    }
    if(eCSR_ROAM_RESULT_FORCED == u2)
    {
       connectionStatus.eventId = eCSR_WLAN_STATUS_DISCONNECT;
       connectionStatus.reason = eCSR_REASON_USER_REQUESTED;
       WLAN_VOS_DIAG_EVENT_REPORT(&connectionStatus, EVENT_WLAN_STATUS);
    }
    if(eCSR_ROAM_RESULT_DISASSOC_IND == u2)
    {
       connectionStatus.eventId = eCSR_WLAN_STATUS_DISCONNECT;
       connectionStatus.reason = eCSR_REASON_DISASSOC;
       WLAN_VOS_DIAG_EVENT_REPORT(&connectionStatus, EVENT_WLAN_STATUS);
    }
    if(eCSR_ROAM_RESULT_DEAUTH_IND == u2)
    {
       connectionStatus.eventId = eCSR_WLAN_STATUS_DISCONNECT;
       connectionStatus.reason = eCSR_REASON_DEAUTH;
       WLAN_VOS_DIAG_EVENT_REPORT(&connectionStatus, EVENT_WLAN_STATUS);
    }
#endif //                             
    
    return (status);
}
//                                                        
tANI_BOOLEAN csrRoamIsHandoffInProgress(tpAniSirGlobal pMac)
{
#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
    return csrNeighborRoamIsHandoffInProgress(pMac);                    
#else
    return eANI_BOOLEAN_FALSE;
#endif
}
eHalStatus csrRoamIssueDisassociate( tpAniSirGlobal pMac, tANI_U32 sessionId,
                                     eCsrRoamSubState NewSubstate, tANI_BOOLEAN fMICFailure )
{   
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tCsrBssid bssId = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    tANI_U16 reasonCode;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    //                                       
    if ( csrIsConnStateConnectedInfra( pMac, sessionId ) )
    {
        smsLog(pMac, LOG1, FL(" restore AC weights (%d-%d-%d-%d)"), pMac->roam.ucACWeights[0], pMac->roam.ucACWeights[1],
            pMac->roam.ucACWeights[2], pMac->roam.ucACWeights[3]);
        WLANTL_SetACWeights(pMac->roam.gVosContext, pMac->roam.ucACWeights);
    }
    
    if ( fMICFailure )
    {
        reasonCode = eSIR_MAC_MIC_FAILURE_REASON;
    }
    else if (NewSubstate == eCSR_ROAM_SUBSTATE_DISASSOC_HANDOFF)
    {
        reasonCode = eSIR_MAC_DISASSOC_DUE_TO_FTHANDOFF_REASON;
    } 
    else 
    {
        reasonCode = eSIR_MAC_UNSPEC_FAILURE_REASON;
    }    
#ifdef WLAN_FEATURE_VOWIFI_11R
    if ( (csrRoamIsHandoffInProgress(pMac)) && 
         (NewSubstate != eCSR_ROAM_SUBSTATE_DISASSOC_HANDOFF))
    {
        tpCsrNeighborRoamControlInfo pNeighborRoamInfo = &pMac->roam.neighborRoamInfo;
        palCopyMemory(pMac->hHdd, &bssId, pNeighborRoamInfo->csrNeighborRoamProfile.BSSIDs.bssid, sizeof(tSirMacAddr));
    } 
    else 
#endif
    if(pSession->pConnectBssDesc)
    {
        palCopyMemory(pMac->hHdd, &bssId, pSession->pConnectBssDesc->bssId, sizeof(tCsrBssid));
    }
    
    
    smsLog( pMac, LOG2, "CSR Attempting to Disassociate Bssid="MAC_ADDRESS_STR
            " subState = %d reason=%d",
            MAC_ADDR_ARRAY(bssId), NewSubstate, reasonCode);

    csrRoamSubstateChange( pMac, NewSubstate, sessionId);

    status = csrSendMBDisassocReqMsg( pMac, sessionId, bssId, reasonCode );    
    
    if(HAL_STATUS_SUCCESS(status)) 
    {
        csrRoamLinkDown(pMac, sessionId);
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
        //                                                                                             
        if(eCSR_ROAM_SUBSTATE_DISASSOC_HANDOFF != NewSubstate)
        {
            //                                         
            sme_QosCsrEventInd(pMac, (v_U8_t)sessionId, SME_QOS_CSR_DISCONNECT_REQ, NULL);
        }
#endif
    }
    else
    {
        smsLog(pMac, LOGW, FL("csrSendMBDisassocReqMsg failed with status %d"), status);
    }

    return (status);
}

/*                                                                            
                                      
                                                                           
                                                
                                                             
                                                                 
                      
                                                                             */
eHalStatus csrRoamIssueDisassociateStaCmd( tpAniSirGlobal pMac, 
                                           tANI_U32 sessionId, 
                                           tANI_U8 *pPeerMacAddr,
                                           tANI_U32 reason)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSmeCmd *pCommand;

    do
    {
        pCommand = csrGetCommandBuffer( pMac );
        if ( !pCommand ) 
        {
            smsLog( pMac, LOGE, FL(" fail to get command buffer") );
            status = eHAL_STATUS_RESOURCES;
            break;
        }
        pCommand->command = eSmeCommandRoam;
        pCommand->sessionId = (tANI_U8)sessionId;
        pCommand->u.roamCmd.roamReason = eCsrForcedDisassocSta;
        vos_mem_copy(pCommand->u.roamCmd.peerMac, pPeerMacAddr, 6);
        pCommand->u.roamCmd.reason = (tSirMacReasonCodes)reason;
        status = csrQueueSmeCommand(pMac, pCommand, eANI_BOOLEAN_FALSE);
        if( !HAL_STATUS_SUCCESS( status ) )
        {
            smsLog( pMac, LOGE, FL(" fail to send message status = %d"), status );
            csrReleaseCommandRoam( pMac, pCommand );
        }
    }while(0);

    return status;
}


/*                                                                            
                             
                                                                     
                                                
                                                             
                                                                 
                      
                                                                             */
eHalStatus csrRoamIssueDeauthStaCmd( tpAniSirGlobal pMac, 
                                     tANI_U32 sessionId, 
                                     tANI_U8 *pPeerMacAddr,
                                     tANI_U32 reason)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSmeCmd *pCommand;

    do
    {
        pCommand = csrGetCommandBuffer( pMac );
        if ( !pCommand ) 
        {
            smsLog( pMac, LOGE, FL(" fail to get command buffer") );
            status = eHAL_STATUS_RESOURCES;
            break;
        }
        pCommand->command = eSmeCommandRoam;
        pCommand->sessionId = (tANI_U8)sessionId;
        pCommand->u.roamCmd.roamReason = eCsrForcedDeauthSta;
        vos_mem_copy(pCommand->u.roamCmd.peerMac, pPeerMacAddr, 6);
        pCommand->u.roamCmd.reason = (tSirMacReasonCodes)reason;
        status = csrQueueSmeCommand(pMac, pCommand, eANI_BOOLEAN_FALSE);
        if( !HAL_STATUS_SUCCESS( status ) )
        {
            smsLog( pMac, LOGE, FL(" fail to send message status = %d"), status );
            csrReleaseCommandRoam( pMac, pCommand );
        }
    }while(0);

    return status;
}
eHalStatus
csrRoamIssueTkipCounterMeasures( tpAniSirGlobal pMac, tANI_U32 sessionId,
                                    tANI_BOOLEAN bEnable )
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    tCsrBssid bssId = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    if (!pSession)
    {
        smsLog( pMac, LOGE, "csrRoamIssueTkipCounterMeasures:CSR Session not found");
        return (status);
    }
    if (pSession->pConnectBssDesc)
    {
        palCopyMemory(pMac->hHdd, &bssId, pSession->pConnectBssDesc->bssId, sizeof(tCsrBssid));
    }
    else
    {
        smsLog( pMac, LOGE, "csrRoamIssueTkipCounterMeasures:Connected BSS Description in CSR Session not found");
        return (status);
    }
    smsLog( pMac, LOG2, "CSR issuing tkip counter measures for Bssid = "MAC_ADDRESS_STR", Enable = %d",
                  MAC_ADDR_ARRAY(bssId), bEnable);
    status = csrSendMBTkipCounterMeasuresReqMsg( pMac, sessionId, bEnable, bssId );
    return (status);
}
eHalStatus
csrRoamGetAssociatedStas( tpAniSirGlobal pMac, tANI_U32 sessionId,
                            VOS_MODULE_ID modId,  void *pUsrContext,
                            void *pfnSapEventCallback, v_U8_t *pAssocStasBuf )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tCsrBssid bssId = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    if (!pSession)
    {
        smsLog( pMac, LOGE, "csrRoamGetAssociatedStas:CSR Session not found");
        return (status);
    }
    if(pSession->pConnectBssDesc)
    {
        palCopyMemory( pMac->hHdd, &bssId, pSession->pConnectBssDesc->bssId, sizeof(tCsrBssid) );
    }
    else
    {
        smsLog( pMac, LOGE, "csrRoamGetAssociatedStas:Connected BSS Description in CSR Session not found");
        return (status);
    }
    smsLog( pMac, LOG2, "CSR getting associated stations for Bssid = "MAC_ADDRESS_STR,
                  MAC_ADDR_ARRAY(bssId));
    status = csrSendMBGetAssociatedStasReqMsg( pMac, sessionId, modId, bssId, pUsrContext, pfnSapEventCallback, pAssocStasBuf );
    return (status);
}
eHalStatus
csrRoamGetWpsSessionOverlap( tpAniSirGlobal pMac, tANI_U32 sessionId,
                             void *pUsrContext, void *pfnSapEventCallback, v_MACADDR_t pRemoveMac )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tCsrBssid bssId = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    
    if (!pSession)
    {
        smsLog( pMac, LOGE, "csrRoamGetWpsSessionOverlap:CSR Session not found");
        return (status);
    }
    if(pSession->pConnectBssDesc)
    {
        palCopyMemory( pMac->hHdd, &bssId, pSession->pConnectBssDesc->bssId, sizeof(tCsrBssid) );
    }
    else
    {
        smsLog( pMac, LOGE, "csrRoamGetWpsSessionOverlap:Connected BSS Description in CSR Session not found");
        return (status);
    }
    smsLog( pMac, LOG2, "CSR getting WPS Session Overlap for Bssid = "MAC_ADDRESS_STR,
                  MAC_ADDR_ARRAY(bssId));
     
    status = csrSendMBGetWPSPBCSessions( pMac, sessionId, bssId, pUsrContext, pfnSapEventCallback, pRemoveMac);            
            
    return (status);
}
eHalStatus csrRoamIssueDeauth( tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamSubState NewSubstate )
{   
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tCsrBssid bssId = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if (!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    if(pSession->pConnectBssDesc)
    {
        palCopyMemory(pMac->hHdd, &bssId, pSession->pConnectBssDesc->bssId, sizeof(tCsrBssid));
    }
    smsLog( pMac, LOG2, "CSR Attempting to Deauth Bssid= "MAC_ADDRESS_STR,
                  MAC_ADDR_ARRAY(bssId));
    csrRoamSubstateChange( pMac, NewSubstate, sessionId);
    
    status = csrSendMBDeauthReqMsg( pMac, sessionId, bssId, eSIR_MAC_DEAUTH_LEAVING_BSS_REASON );
    if(!HAL_STATUS_SUCCESS(status))
    {
        smsLog(pMac, LOGW, FL("csrSendMBDeauthReqMsg failed with status %d"), status);
    }

    return (status);
}

eHalStatus csrRoamSaveConnectedBssDesc( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirBssDescription *pBssDesc )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    tANI_U32 size;

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    //                                                                                    
    //                                                                                  
    if(NULL == pBssDesc)
    {
        csrFreeConnectBssDesc(pMac, sessionId);
    }
    else 
    {
        size = pBssDesc->length + sizeof( pBssDesc->length );
        if(NULL != pSession->pConnectBssDesc)
        {
            if(((pSession->pConnectBssDesc->length) + sizeof(pSession->pConnectBssDesc->length)) < size)
            {
                //                                                                           
                csrFreeConnectBssDesc(pMac, sessionId);
            }
        }
        if(NULL == pSession->pConnectBssDesc)
        {
            status = palAllocateMemory( pMac->hHdd, (void **)&pSession->pConnectBssDesc, size); 
        }
        if ( HAL_STATUS_SUCCESS(status) && pSession->pConnectBssDesc ) 
        {
            palCopyMemory( pMac->hHdd, pSession->pConnectBssDesc, pBssDesc, size );    
        }
    }
    
    return (status);
}

eHalStatus csrRoamPrepareBssConfig(tpAniSirGlobal pMac, tCsrRoamProfile *pProfile, 
                                    tSirBssDescription *pBssDesc, tBssConfigParam *pBssConfig,
                                    tDot11fBeaconIEs *pIes)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    eCsrCfgDot11Mode cfgDot11Mode;
    VOS_ASSERT( pIes != NULL );
    if (pIes == NULL)
        return eHAL_STATUS_FAILURE;

    do
    {
        palCopyMemory(pMac->hHdd, &pBssConfig->BssCap, &pBssDesc->capabilityInfo, sizeof(tSirMacCapabilityInfo));
        //       
        pBssConfig->qosType = csrGetQoSFromBssDesc(pMac, pBssDesc, pIes);
        //        
        if(pIes->SSID.present)
        {
            palCopyMemory(pMac->hHdd, &pBssConfig->SSID.ssId, pIes->SSID.ssid, pIes->SSID.num_ssid);
            pBssConfig->SSID.length = pIes->SSID.num_ssid;
        }
        else
            pBssConfig->SSID.length = 0;
        if(csrIsNULLSSID(pBssConfig->SSID.ssId, pBssConfig->SSID.length))
        {
            smsLog(pMac, LOGW, "  BSS desc SSID is a wildcard");
            //                                                     
            if(pProfile->SSIDs.numOfSSIDs == 0)
            {
                smsLog(pMac, LOGW, "  Both BSS desc and profile doesn't have SSID");
                status = eHAL_STATUS_FAILURE;
                break;
            }
        }
        if(CSR_IS_CHANNEL_5GHZ(pBssDesc->channelId))
        {
            pBssConfig->eBand = eCSR_BAND_5G;
        }
        else
        {
            pBssConfig->eBand = eCSR_BAND_24;
        }
        //       
        if(csrIsPhyModeMatch( pMac, pProfile->phyMode, pBssDesc, pProfile, &cfgDot11Mode, pIes ))
        {
            pBssConfig->uCfgDot11Mode = cfgDot11Mode;
        }
        else 
        {
            smsLog(pMac, LOGW, "   Can not find match phy mode");
            //        
            if(eCSR_BAND_24 == pBssConfig->eBand)
            {
                pBssConfig->uCfgDot11Mode = eCSR_CFG_DOT11_MODE_11G;
            }
            else
            {
                pBssConfig->uCfgDot11Mode = eCSR_CFG_DOT11_MODE_11A;
            }
        }
        //   
        if ((pBssConfig->uCfgDot11Mode != eCSR_CFG_DOT11_MODE_11N) &&
                (pMac->roam.configParam.WMMSupportMode == eCsrRoamWmmNoQos))
        {
            //                                                             
            //                   
            pBssConfig->qosType = eCSR_MEDIUM_ACCESS_DCF;
        }

        if (((pBssConfig->uCfgDot11Mode == eCSR_CFG_DOT11_MODE_11N)  || 
                         (pBssConfig->uCfgDot11Mode == eCSR_CFG_DOT11_MODE_11AC)) &&
                         ((pBssConfig->qosType != eCSR_MEDIUM_ACCESS_WMM_eDCF_DSCP) ||
                          (pBssConfig->qosType != eCSR_MEDIUM_ACCESS_11e_HCF) ||
                          (pBssConfig->qosType != eCSR_MEDIUM_ACCESS_11e_eDCF) ))
        {
            //                                                     
            //                                              
            pBssConfig->qosType = eCSR_MEDIUM_ACCESS_WMM_eDCF_DSCP;
        }

        //         
        switch( pProfile->negotiatedAuthType ) 
        {
            default:
            case eCSR_AUTH_TYPE_WPA:
            case eCSR_AUTH_TYPE_WPA_PSK:
            case eCSR_AUTH_TYPE_WPA_NONE:
            case eCSR_AUTH_TYPE_OPEN_SYSTEM:
                pBssConfig->authType = eSIR_OPEN_SYSTEM;
                break;
            case eCSR_AUTH_TYPE_SHARED_KEY:
                pBssConfig->authType = eSIR_SHARED_KEY;
                break;
            case eCSR_AUTH_TYPE_AUTOSWITCH:
                pBssConfig->authType = eSIR_AUTO_SWITCH;
                break;
        }
        //               
        if( eCSR_CFG_DOT11_MODE_11B != cfgDot11Mode )
        {
            pBssConfig->uShortSlotTime = pMac->roam.configParam.shortSlotTime;
        }
        else
        {
            pBssConfig->uShortSlotTime = 0;
        }
        if(pBssConfig->BssCap.ibss)
        {
            //                            
            pBssConfig->f11hSupport = eANI_BOOLEAN_FALSE; 
        }
        else
        {
            pBssConfig->f11hSupport = pMac->roam.configParam.Is11hSupportEnabled;
        }
        //                
        pBssConfig->uPowerLimit = csrGet11hPowerConstraint(pMac, &pIes->PowerConstraints);
        //         
        if ( CSR_IS_11A_BSS( pBssDesc ) )
        {
             pBssConfig->uHeartBeatThresh = pMac->roam.configParam.HeartbeatThresh50;        
        }
        else
        {
             pBssConfig->uHeartBeatThresh = pMac->roam.configParam.HeartbeatThresh24;
        }
        //            
        //                                                                                 
        //                                                      
        if ( pBssDesc->beaconInterval )
        {
            //                                       
            pBssConfig->uJoinTimeOut = CSR_ROAM_MAX(10 * pBssDesc->beaconInterval, CSR_JOIN_FAILURE_TIMEOUT_MIN);
        }
        else 
        {
            pBssConfig->uJoinTimeOut = CSR_JOIN_FAILURE_TIMEOUT_DEFAULT;
        }
        //           
        pBssConfig->cbMode = csrGetCBModeFromIes(pMac, pBssDesc->channelId, pIes);
    }while(0);
    return (status);
}

static eHalStatus csrRoamPrepareBssConfigFromProfile(tpAniSirGlobal pMac, tCsrRoamProfile *pProfile, 
                                                     tBssConfigParam *pBssConfig, tSirBssDescription *pBssDesc)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_U8 operationChannel = 0; 
    tANI_U8 qAPisEnabled = FALSE;
    //    
    pBssConfig->SSID.length = 0;
    if(pProfile->SSIDs.numOfSSIDs)
    {
        //                      
        palCopyMemory(pMac->hHdd, &pBssConfig->SSID, &pProfile->SSIDs.SSIDList[0].SSID, sizeof(tSirMacSSid));
    }
    else
    {
        //                 
        return eHAL_STATUS_FAILURE;
    }
    //                           
    if( csrIsBssTypeIBSS(pProfile->BSSType) )
    {
        pBssConfig->BssCap.ibss = 1;
    }
    else
    {
        pBssConfig->BssCap.ess = 1;
    }
    if( eCSR_ENCRYPT_TYPE_NONE != pProfile->EncryptionType.encryptionType[0] )
    {
        pBssConfig->BssCap.privacy = 1;
    }
    pBssConfig->eBand = pMac->roam.configParam.eBand;
    //       
    if(pProfile->ChannelInfo.ChannelList)
    {
       operationChannel = pProfile->ChannelInfo.ChannelList[0];
    }
    pBssConfig->uCfgDot11Mode = csrRoamGetPhyModeBandForBss(pMac, pProfile, operationChannel, 
                                        &pBssConfig->eBand);
    //   
    //                                           
    if ( pBssConfig->BssCap.ess == 1 ) 
    {
        /*                          */
        if(CSR_IS_INFRA_AP(pProfile) && (eCsrRoamWmmNoQos != pMac->roam.configParam.WMMSupportMode )){
             qAPisEnabled = TRUE;
        }
        else
        if (csrRoamGetQosInfoFromBss(pMac, pBssDesc) == eHAL_STATUS_SUCCESS) {
             qAPisEnabled = TRUE;
        } else {
             qAPisEnabled = FALSE;
        }
    } else {
             qAPisEnabled = TRUE;
    }
    if (( eCsrRoamWmmNoQos != pMac->roam.configParam.WMMSupportMode && qAPisEnabled) ||
          (( eCSR_CFG_DOT11_MODE_11N == pBssConfig->uCfgDot11Mode && qAPisEnabled) ||
             ( eCSR_CFG_DOT11_MODE_TAURUS == pBssConfig->uCfgDot11Mode ) ) //                 
      )
    {
        pBssConfig->qosType = eCSR_MEDIUM_ACCESS_WMM_eDCF_DSCP;
    } else {
        pBssConfig->qosType = eCSR_MEDIUM_ACCESS_DCF;
    }
    
    //         
    switch( pProfile->AuthType.authType[0] ) //                             
    {
        default:
        case eCSR_AUTH_TYPE_WPA:
        case eCSR_AUTH_TYPE_WPA_PSK:
        case eCSR_AUTH_TYPE_WPA_NONE:
        case eCSR_AUTH_TYPE_OPEN_SYSTEM:
            pBssConfig->authType = eSIR_OPEN_SYSTEM;
            break;
        case eCSR_AUTH_TYPE_SHARED_KEY:
            pBssConfig->authType = eSIR_SHARED_KEY;
            break;
        case eCSR_AUTH_TYPE_AUTOSWITCH:
            pBssConfig->authType = eSIR_AUTO_SWITCH;
            break;
    }
    //               
    if( WNI_CFG_PHY_MODE_11B != pBssConfig->uCfgDot11Mode )
    {
        pBssConfig->uShortSlotTime = pMac->roam.configParam.shortSlotTime;
    }
    else
    {
        pBssConfig->uShortSlotTime = 0;
    }
    //                                              
    pBssConfig->f11hSupport = eANI_BOOLEAN_FALSE;
    pBssConfig->uPowerLimit = 0;
    //         
    if ( eCSR_BAND_5G == pBssConfig->eBand )
    {
        pBssConfig->uHeartBeatThresh = pMac->roam.configParam.HeartbeatThresh50;        
    }
    else
    {
        pBssConfig->uHeartBeatThresh = pMac->roam.configParam.HeartbeatThresh24;
    }
    //            
    pBssConfig->uJoinTimeOut = CSR_JOIN_FAILURE_TIMEOUT_DEFAULT;

    return (status);
}
static eHalStatus csrRoamGetQosInfoFromBss(tpAniSirGlobal pMac, tSirBssDescription *pBssDesc)
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    tDot11fBeaconIEs *pIes = NULL;
   
  do
   {
      if(!HAL_STATUS_SUCCESS(csrGetParsedBssDescriptionIEs(pMac, pBssDesc, &pIes)))
      {
         //       
         VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, 
                   "csrRoamGetQosInfoFromBss() failed");
         break;
      }
      //                                         
      if( CSR_IS_QOS_BSS(pIes) )
      {
         status = eHAL_STATUS_SUCCESS;
      }
   } while (0);

   if (NULL != pIes)
   {
       vos_mem_free(pIes);
   }

   return status;
}

void csrSetCfgPrivacy( tpAniSirGlobal pMac, tCsrRoamProfile *pProfile, tANI_BOOLEAN fPrivacy )
{
    //                                                                                                  
    //                                                                                                      
    //                                      
    tANI_U32 PrivacyEnabled = 0;
    tANI_U32 RsnEnabled = 0;
    tANI_U32 WepDefaultKeyId = 0;
    tANI_U32 WepKeyLength = WNI_CFG_WEP_KEY_LENGTH_5;   /*                 */
    tANI_U32 Key0Length = 0;
    tANI_U32 Key1Length = 0;
    tANI_U32 Key2Length = 0;
    tANI_U32 Key3Length = 0;
    
    //                             
    tANI_U8 Key0[ WNI_CFG_WEP_DEFAULT_KEY_1_LEN ];
    tANI_U8 Key1[ WNI_CFG_WEP_DEFAULT_KEY_2_LEN ];
    tANI_U8 Key2[ WNI_CFG_WEP_DEFAULT_KEY_3_LEN ];
    tANI_U8 Key3[ WNI_CFG_WEP_DEFAULT_KEY_4_LEN ];
    
    switch ( pProfile->negotiatedUCEncryptionType )
    {
        case eCSR_ENCRYPT_TYPE_NONE:
        
            //                                             
            PrivacyEnabled = 0;           
            RsnEnabled = 0;
            
            //                                                                    
            
            //                                                   
            Key0Length = 0;
            Key1Length = 0;
            Key2Length = 0;
            Key3Length = 0;
            
            break;
            
        case eCSR_ENCRYPT_TYPE_WEP40_STATICKEY:
        case eCSR_ENCRYPT_TYPE_WEP40:
            
            //                                             
            PrivacyEnabled = 1;           
            RsnEnabled = 0;
                        
            //                            
            WepDefaultKeyId = pProfile->Keys.defaultIndex;
            //                                   
            WepKeyLength = WNI_CFG_WEP_KEY_LENGTH_5;            
            
            //                                                                                             
            if ( pProfile->Keys.KeyLength[0] ) 
            {
                palCopyMemory( pMac->hHdd, Key0, pProfile->Keys.KeyMaterial[0], WNI_CFG_WEP_KEY_LENGTH_5 );
                Key0Length = WNI_CFG_WEP_KEY_LENGTH_5;
            }
            else
            {
                Key0Length = 0;
            }
            
            if ( pProfile->Keys.KeyLength[1] ) 
            {
                palCopyMemory( pMac->hHdd, Key1, pProfile->Keys.KeyMaterial[1], WNI_CFG_WEP_KEY_LENGTH_5 );
                Key1Length = WNI_CFG_WEP_KEY_LENGTH_5;
            }
            else
            {
                Key1Length = 0;
            }
            
            if ( pProfile->Keys.KeyLength[2] ) 
            {
                palCopyMemory( pMac->hHdd, Key2, pProfile->Keys.KeyMaterial[2], WNI_CFG_WEP_KEY_LENGTH_5 );
                Key2Length = WNI_CFG_WEP_KEY_LENGTH_5;                
            }
            else
            {
                Key2Length = 0;
            }
            
            if ( pProfile->Keys.KeyLength[3] ) 
            {
                palCopyMemory( pMac->hHdd, Key3, pProfile->Keys.KeyMaterial[3], WNI_CFG_WEP_KEY_LENGTH_5 );
                Key3Length = WNI_CFG_WEP_KEY_LENGTH_5;                
            }
            else
            {
                Key3Length = 0;
            }      
            break;
        
        case eCSR_ENCRYPT_TYPE_WEP104_STATICKEY:
        case eCSR_ENCRYPT_TYPE_WEP104:
            
            //                                             
            PrivacyEnabled = 1;           
            RsnEnabled = 0;
            
            //                            
            WepDefaultKeyId = pProfile->Keys.defaultIndex;
           
            //                                     
            WepKeyLength = WNI_CFG_WEP_KEY_LENGTH_13;
            
            //                                                                                             
            if ( pProfile->Keys.KeyLength[0] ) 
            {
                palCopyMemory( pMac->hHdd, Key0, pProfile->Keys.KeyMaterial[ 0 ], WNI_CFG_WEP_KEY_LENGTH_13 );
                Key0Length = WNI_CFG_WEP_KEY_LENGTH_13;
            }
            else
            {
                Key0Length = 0;
            }
            
            if ( pProfile->Keys.KeyLength[1] ) 
            {
                palCopyMemory( pMac->hHdd, Key1, pProfile->Keys.KeyMaterial[ 1 ], WNI_CFG_WEP_KEY_LENGTH_13 );
                Key1Length = WNI_CFG_WEP_KEY_LENGTH_13;
            }
            else
            {
                Key1Length = 0;
            }
            
            if ( pProfile->Keys.KeyLength[2] ) 
            {
                palCopyMemory( pMac->hHdd, Key2, pProfile->Keys.KeyMaterial[ 2 ], WNI_CFG_WEP_KEY_LENGTH_13 );
                Key2Length = WNI_CFG_WEP_KEY_LENGTH_13;
            }
            else
            {
                Key2Length = 0;
            }
            
            if ( pProfile->Keys.KeyLength[3] ) 
            {
                palCopyMemory( pMac->hHdd, Key3, pProfile->Keys.KeyMaterial[ 3 ], WNI_CFG_WEP_KEY_LENGTH_13 );
                Key3Length = WNI_CFG_WEP_KEY_LENGTH_13;
            }
            else
            {
                Key3Length = 0;
            }
           
            break;
        
        case eCSR_ENCRYPT_TYPE_TKIP:
        case eCSR_ENCRYPT_TYPE_AES:
#ifdef FEATURE_WLAN_WAPI
        case eCSR_ENCRYPT_TYPE_WPI:
#endif /*                   */
            //                                                                                                  
            //                                                                                                                     
            PrivacyEnabled = (0 != fPrivacy);
                         
            //                                            
            RsnEnabled = 1;
            
            //                                                                    
            
            //                                                          
            Key0Length = 0;
            Key1Length = 0;
            Key2Length = 0;
            Key3Length = 0;        
          
            break;     
        default:
            PrivacyEnabled = 0;
            RsnEnabled = 0;
            break;            
    }           
    
    ccmCfgSetInt(pMac, WNI_CFG_PRIVACY_ENABLED, PrivacyEnabled, NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetInt(pMac, WNI_CFG_RSN_ENABLED, RsnEnabled, NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetStr(pMac, WNI_CFG_WEP_DEFAULT_KEY_1, Key0, Key0Length, NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetStr(pMac, WNI_CFG_WEP_DEFAULT_KEY_2, Key1, Key1Length, NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetStr(pMac, WNI_CFG_WEP_DEFAULT_KEY_3, Key2, Key2Length, NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetStr(pMac, WNI_CFG_WEP_DEFAULT_KEY_4, Key3, Key3Length, NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetInt(pMac, WNI_CFG_WEP_KEY_LENGTH, WepKeyLength, NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetInt(pMac, WNI_CFG_WEP_DEFAULT_KEYID, WepDefaultKeyId, NULL, eANI_BOOLEAN_FALSE);
}

static void csrSetCfgSsid( tpAniSirGlobal pMac, tSirMacSSid *pSSID )
{
    tANI_U32 len = 0;
    if(pSSID->length <= WNI_CFG_SSID_LEN)
    {
        len = pSSID->length;
    }
    ccmCfgSetStr(pMac, WNI_CFG_SSID, (tANI_U8 *)pSSID->ssId, len, NULL, eANI_BOOLEAN_FALSE);
}

eHalStatus csrSetQosToCfg( tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrMediaAccessType qosType )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_U32 QoSEnabled;
    tANI_U32 WmeEnabled;
    //                                                                              
    switch( qosType )
    {
        case eCSR_MEDIUM_ACCESS_WMM_eDCF_802dot1p:
            QoSEnabled = FALSE;
            WmeEnabled = TRUE;
            break;
        case eCSR_MEDIUM_ACCESS_WMM_eDCF_DSCP:
            QoSEnabled = FALSE;
            WmeEnabled = TRUE;
            break;
        case eCSR_MEDIUM_ACCESS_WMM_eDCF_NoClassify:
            QoSEnabled = FALSE;
            WmeEnabled = TRUE;
            break;
        case eCSR_MEDIUM_ACCESS_11e_eDCF:
            QoSEnabled = TRUE;
            WmeEnabled = FALSE;
            break;
        case eCSR_MEDIUM_ACCESS_11e_HCF:
            QoSEnabled = TRUE;
            WmeEnabled = FALSE;
            break;
        default:
        case eCSR_MEDIUM_ACCESS_DCF:
            QoSEnabled = FALSE;
            WmeEnabled = FALSE;
            break;
    }
    //                                  
    pMac->roam.roamSession[sessionId].fWMMConnection = (tANI_BOOLEAN)WmeEnabled;
    pMac->roam.roamSession[sessionId].fQOSConnection = (tANI_BOOLEAN)QoSEnabled;
    return (status);
}
static eHalStatus csrGetRateSet( tpAniSirGlobal pMac,  tCsrRoamProfile *pProfile, eCsrPhyMode phyMode, tSirBssDescription *pBssDesc,
                           tDot11fBeaconIEs *pIes, tSirMacRateSet *pOpRateSet, tSirMacRateSet *pExRateSet)
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    int i;
    eCsrCfgDot11Mode cfgDot11Mode;
    tANI_U8 *pDstRate;
    palZeroMemory(pMac->hHdd, pOpRateSet, sizeof(tSirMacRateSet));
    palZeroMemory(pMac->hHdd, pExRateSet, sizeof(tSirMacRateSet));
    VOS_ASSERT( pIes != NULL );
    
    if( NULL != pIes )
    {
        csrIsPhyModeMatch( pMac, phyMode, pBssDesc, pProfile, &cfgDot11Mode, pIes );
        //                                                                       
        //                                                                      
        //                                                                      
        //                                                                     
        //                                                               
        //                                            
        //                                                                   
        //                                                                  
        //                                        
        pDstRate = pOpRateSet->rate;
        if(pIes->SuppRates.present)
        {
            for ( i = 0; i < pIes->SuppRates.num_rates; i++ ) 
            {
                if ( csrRatesIsDot11RateSupported( pMac, pIes->SuppRates.rates[ i ] ) ) 
                {
                    *pDstRate++ = pIes->SuppRates.rates[ i ];
                    pOpRateSet->numRates++;
                }
            }
        }
        if ( eCSR_CFG_DOT11_MODE_11G == cfgDot11Mode || 
             eCSR_CFG_DOT11_MODE_11N == cfgDot11Mode ||
             eCSR_CFG_DOT11_MODE_TAURUS == cfgDot11Mode ||
             eCSR_CFG_DOT11_MODE_ABG == cfgDot11Mode )
        {
            //                                                                 
            //                                                                
            //     
            pDstRate = pExRateSet->rate;
            if(pIes->ExtSuppRates.present)
            {
                for ( i = 0; i < pIes->ExtSuppRates.num_rates; i++ ) 
                {
                    if ( csrRatesIsDot11RateSupported( pMac, pIes->ExtSuppRates.rates[ i ] ) ) 
                    {
                        *pDstRate++ = pIes->ExtSuppRates.rates[ i ];
                        pExRateSet->numRates++;
                    }
                }
            }
        }
    }//               
    else
    {
        smsLog(pMac, LOGE, FL("failed to parse BssDesc"));
    }
    if (pOpRateSet->numRates > 0 || pExRateSet->numRates > 0) status = eHAL_STATUS_SUCCESS;
    return status;
}
    
static void csrSetCfgRateSet( tpAniSirGlobal pMac, eCsrPhyMode phyMode, tCsrRoamProfile *pProfile,
                              tSirBssDescription *pBssDesc, tDot11fBeaconIEs *pIes)
{
    int i;
    tANI_U8 *pDstRate;
    eCsrCfgDot11Mode cfgDot11Mode;
    tANI_U8 OperationalRates[ CSR_DOT11_SUPPORTED_RATES_MAX ];    //                                              
    tANI_U32 OperationalRatesLength = 0;
    tANI_U8 ExtendedOperationalRates[ CSR_DOT11_EXTENDED_SUPPORTED_RATES_MAX ];    //                                              
    tANI_U32 ExtendedOperationalRatesLength = 0;
    tANI_U8 ProprietaryOperationalRates[ 4 ];    //                                                          
    tANI_U32 ProprietaryOperationalRatesLength = 0;
    tANI_U32 PropRatesEnable = 0;
    tANI_U8 MCSRateIdxSet[ SIZE_OF_SUPPORTED_MCS_SET ];
    tANI_U32 MCSRateLength = 0;
    VOS_ASSERT( pIes != NULL );
    if( NULL != pIes )
    {
        csrIsPhyModeMatch( pMac, phyMode, pBssDesc, pProfile, &cfgDot11Mode, pIes );
        //                                                                       
        //                                                                      
        //                                                                      
        //                                                                     
        //                                                               
        //                                            
        //                                                                   
        //                                                                  
        //                                        
        pDstRate = OperationalRates;
        if(pIes->SuppRates.present)
        {
            for ( i = 0; i < pIes->SuppRates.num_rates; i++ ) 
            {
                if ( csrRatesIsDot11RateSupported( pMac, pIes->SuppRates.rates[ i ] ) &&
                     ( OperationalRatesLength < CSR_DOT11_SUPPORTED_RATES_MAX ))
                {
                    *pDstRate++ = pIes->SuppRates.rates[ i ];
                    OperationalRatesLength++;
                }
            }
        }
        if ( eCSR_CFG_DOT11_MODE_11G == cfgDot11Mode || 
             eCSR_CFG_DOT11_MODE_11N == cfgDot11Mode ||
             eCSR_CFG_DOT11_MODE_TAURUS == cfgDot11Mode ||
             eCSR_CFG_DOT11_MODE_ABG == cfgDot11Mode )
        {
            //                                                                 
            //                                                                
            //     
            pDstRate = ExtendedOperationalRates;
            if(pIes->ExtSuppRates.present)
            {
                for ( i = 0; i < pIes->ExtSuppRates.num_rates; i++ ) 
                {
                    if ( csrRatesIsDot11RateSupported( pMac, pIes->ExtSuppRates.rates[ i ] ) &&
                     ( ExtendedOperationalRatesLength < CSR_DOT11_EXTENDED_SUPPORTED_RATES_MAX ))
                    {
                        *pDstRate++ = pIes->ExtSuppRates.rates[ i ];
                        ExtendedOperationalRatesLength++;
                    }
                }
            }
        }
        //                                                                   
        //                       
        if( pIes->Airgo.present && pMac->roam.configParam.ProprietaryRatesEnabled )
        {
            PropRatesEnable = 1;
        }
        else
        {
            PropRatesEnable = 0;
        }
        //                                                                     
        //                                                                    
        //               
        if ( PropRatesEnable && pIes->Airgo.PropSuppRates.present &&
             ( pIes->Airgo.PropSuppRates.num_rates > 0 )) 
        {
            ProprietaryOperationalRatesLength = pIes->Airgo.PropSuppRates.num_rates;
            if ( ProprietaryOperationalRatesLength > sizeof(ProprietaryOperationalRates) )
            {
               ProprietaryOperationalRatesLength = sizeof (ProprietaryOperationalRates);
            }
            palCopyMemory( pMac->hHdd, ProprietaryOperationalRates, pIes->Airgo.PropSuppRates.rates, ProprietaryOperationalRatesLength );
        }
        else {
            //                        
            ProprietaryOperationalRatesLength = 0;
        }
        /*              */
        pDstRate = MCSRateIdxSet;
        if ( pIes->HTCaps.present )
        {
           for ( i = 0; i < VALID_MAX_MCS_INDEX; i++ )
           {
              if ( (unsigned int)pIes->HTCaps.supportedMCSSet[0] & (1 << i) )
              {
                 MCSRateLength++;
                 *pDstRate++ = i;
              }
           }
        }
        //                                              
        ccmCfgSetStr(pMac, WNI_CFG_OPERATIONAL_RATE_SET, OperationalRates, 
                        OperationalRatesLength, NULL, eANI_BOOLEAN_FALSE);
        ccmCfgSetStr(pMac, WNI_CFG_EXTENDED_OPERATIONAL_RATE_SET, ExtendedOperationalRates, 
                            ExtendedOperationalRatesLength, NULL, eANI_BOOLEAN_FALSE);
        ccmCfgSetStr(pMac, WNI_CFG_PROPRIETARY_OPERATIONAL_RATE_SET, 
                        ProprietaryOperationalRates, 
                        ProprietaryOperationalRatesLength, NULL, eANI_BOOLEAN_FALSE);
        ccmCfgSetInt(pMac, WNI_CFG_PROPRIETARY_ANI_FEATURES_ENABLED, PropRatesEnable, NULL, eANI_BOOLEAN_FALSE);
        ccmCfgSetStr(pMac, WNI_CFG_CURRENT_MCS_SET, MCSRateIdxSet, 
                        MCSRateLength, NULL, eANI_BOOLEAN_FALSE);        
    }//               
    else
    {
        smsLog(pMac, LOGE, FL("failed to parse BssDesc"));
    }
}

static void csrSetCfgRateSetFromProfile( tpAniSirGlobal pMac,
                                         tCsrRoamProfile *pProfile  )
{
    tSirMacRateSetIE DefaultSupportedRates11a = {  SIR_MAC_RATESET_EID, 
                                                   { 8, 
                                                     { SIR_MAC_RATE_6, 
                                                   SIR_MAC_RATE_9, 
                                                   SIR_MAC_RATE_12, 
                                                   SIR_MAC_RATE_18,
                                                   SIR_MAC_RATE_24,
                                                   SIR_MAC_RATE_36,
                                                   SIR_MAC_RATE_48,
                                                       SIR_MAC_RATE_54  } } };
    tSirMacRateSetIE DefaultSupportedRates11b = {  SIR_MAC_RATESET_EID, 
                                                   { 4, 
                                                     { SIR_MAC_RATE_1, 
                                                   SIR_MAC_RATE_2, 
                                                   SIR_MAC_RATE_5_5, 
                                                       SIR_MAC_RATE_11  } } };
                                                              
                                                              
    tSirMacPropRateSet DefaultSupportedPropRates = { 3, 
                                                     { SIR_MAC_RATE_72,
                                                     SIR_MAC_RATE_96,
                                                       SIR_MAC_RATE_108 } };
    eCsrCfgDot11Mode cfgDot11Mode;
    eCsrBand eBand;
    tANI_U8 OperationalRates[ CSR_DOT11_SUPPORTED_RATES_MAX ];    //                                              
    tANI_U32 OperationalRatesLength = 0;
    tANI_U8 ExtendedOperationalRates[ CSR_DOT11_EXTENDED_SUPPORTED_RATES_MAX ];    //                                              
    tANI_U32 ExtendedOperationalRatesLength = 0;
    tANI_U8 ProprietaryOperationalRates[ 4 ];    //                                                          
    tANI_U32 ProprietaryOperationalRatesLength = 0;
    tANI_U32 PropRatesEnable = 0;
    tANI_U8 operationChannel = 0; 
    if(pProfile->ChannelInfo.ChannelList)
    {
       operationChannel = pProfile->ChannelInfo.ChannelList[0];
    }
    cfgDot11Mode = csrRoamGetPhyModeBandForBss( pMac, pProfile, operationChannel, &eBand );
    //                                                                                    
    //                                                                             
    //                                                                       
    //                                                                          
    //                                  
    if ( eCSR_BAND_5G == eBand ) 
    {       
        //                                                          
        OperationalRatesLength = DefaultSupportedRates11a.supportedRateSet.numRates *
                                            sizeof(*DefaultSupportedRates11a.supportedRateSet.rate);
        palCopyMemory( pMac->hHdd, OperationalRates,         
                        DefaultSupportedRates11a.supportedRateSet.rate, 
                        OperationalRatesLength );
                         
        //                                  
        ExtendedOperationalRatesLength = 0;
        //                                               
        if ( pMac->roam.configParam.ProprietaryRatesEnabled ) 
        {
            ProprietaryOperationalRatesLength = DefaultSupportedPropRates.numPropRates * 
                                                            sizeof(*DefaultSupportedPropRates.propRate);         
            palCopyMemory( pMac->hHdd, ProprietaryOperationalRates,
                            DefaultSupportedPropRates.propRate, 
                            ProprietaryOperationalRatesLength );
        }    
        else 
        {       
            //                     
            ProprietaryOperationalRatesLength = 0;         
        }    
    }    
    else if ( eCSR_CFG_DOT11_MODE_11B == cfgDot11Mode ) 
    {       
        //                                                  
        OperationalRatesLength = DefaultSupportedRates11b.supportedRateSet.numRates *
                                              sizeof(*DefaultSupportedRates11b.supportedRateSet.rate);
        palCopyMemory( pMac->hHdd, OperationalRates, 
                        DefaultSupportedRates11b.supportedRateSet.rate, 
                        OperationalRatesLength );
        //                                  
        ExtendedOperationalRatesLength = 0;
        //                     
        ProprietaryOperationalRatesLength = 0;
    }    
    else 
    {       
        //    
        
        //                                                  
        OperationalRatesLength = DefaultSupportedRates11b.supportedRateSet.numRates * 
                                            sizeof(*DefaultSupportedRates11b.supportedRateSet.rate);
        palCopyMemory( pMac->hHdd, OperationalRates, 
                        DefaultSupportedRates11b.supportedRateSet.rate, 
                        OperationalRatesLength );
        
        //                                       
        ExtendedOperationalRatesLength = DefaultSupportedRates11a.supportedRateSet.numRates * 
                                                    sizeof(*DefaultSupportedRates11a.supportedRateSet.rate);
        palCopyMemory( pMac->hHdd, ExtendedOperationalRates,         
                        DefaultSupportedRates11a.supportedRateSet.rate, 
                        ExtendedOperationalRatesLength );
        
        //                                               
        if ( pMac->roam.configParam.ProprietaryRatesEnabled ) 
        {
            ProprietaryOperationalRatesLength = DefaultSupportedPropRates.numPropRates *
                                                            sizeof(*DefaultSupportedPropRates.propRate);         
            palCopyMemory( pMac->hHdd, ProprietaryOperationalRates, 
                            DefaultSupportedPropRates.propRate, 
                            ProprietaryOperationalRatesLength );
        }  
        else 
        {       
           //                     
            ProprietaryOperationalRatesLength = 0;         
        }    
    }  
    //                                                                                                    
    if ( ProprietaryOperationalRatesLength && pMac->roam.configParam.ProprietaryRatesEnabled ) 
    {
        PropRatesEnable = 1;                
    }
    else 
    {
        PropRatesEnable = 0;    
    }
        
    //                                              
    ccmCfgSetStr(pMac, WNI_CFG_OPERATIONAL_RATE_SET, OperationalRates, 
                    OperationalRatesLength, NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetStr(pMac, WNI_CFG_EXTENDED_OPERATIONAL_RATE_SET, ExtendedOperationalRates, 
                        ExtendedOperationalRatesLength, NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetStr(pMac, WNI_CFG_PROPRIETARY_OPERATIONAL_RATE_SET, 
                    ProprietaryOperationalRates, 
                    ProprietaryOperationalRatesLength, NULL, eANI_BOOLEAN_FALSE);
    ccmCfgSetInt(pMac, WNI_CFG_PROPRIETARY_ANI_FEATURES_ENABLED, PropRatesEnable, NULL, eANI_BOOLEAN_FALSE);
}
void csrRoamCcmCfgSetCallback(tHalHandle hHal, tANI_S32 result)
{
    tpAniSirGlobal pMac = PMAC_STRUCT( hHal );

    tListElem *pEntry = csrLLPeekHead(&pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK);
    tANI_U32 sessionId;
    tSmeCmd *pCommand = NULL;

    if(NULL == pEntry)
    {
        smsLog(pMac, LOGW, "   CFG_CNF with active list empty");
        return;
    }
    pCommand = GET_BASE_ADDR(pEntry, tSmeCmd, Link);
    sessionId = pCommand->sessionId;

    if(CSR_IS_ROAM_JOINING(pMac, sessionId) && CSR_IS_ROAM_SUBSTATE_CONFIG(pMac, sessionId))
    {
        csrRoamingStateConfigCnfProcessor(pMac, (tANI_U32)result);
    }
}

//                                                                             
tANI_U32 csrRoamGetPhyModeFromDot11Mode(eCsrCfgDot11Mode dot11Mode, eCsrBand band)
{
    if(eCSR_CFG_DOT11_MODE_11B == dot11Mode)
    {
        return (WNI_CFG_PHY_MODE_11B);
    }
    else
    {
        if(eCSR_BAND_24 == band)
            return (WNI_CFG_PHY_MODE_11G);
    }
    return (WNI_CFG_PHY_MODE_11A);
}
        
        
#ifdef WLAN_FEATURE_11AC
ePhyChanBondState csrGetHTCBStateFromVHTCBState(ePhyChanBondState aniCBMode)
{
    switch ( aniCBMode )
    {
        case PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_LOW:
        case PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_CENTERED:
        case PHY_QUADRUPLE_CHANNEL_20MHZ_HIGH_40MHZ_HIGH:
            return PHY_DOUBLE_CHANNEL_HIGH_PRIMARY;
        case PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_LOW:
        case PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_CENTERED:
        case PHY_QUADRUPLE_CHANNEL_20MHZ_LOW_40MHZ_HIGH:
            return PHY_DOUBLE_CHANNEL_LOW_PRIMARY;
        case PHY_QUADRUPLE_CHANNEL_20MHZ_CENTERED_40MHZ_CENTERED:
        default :
            return PHY_SINGLE_CHANNEL_CENTERED;
    }
}
#endif

//                
eHalStatus csrRoamSetBssConfigCfg(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile,
                          tSirBssDescription *pBssDesc, tBssConfigParam *pBssConfig,
                          tDot11fBeaconIEs *pIes, tANI_BOOLEAN resetCountry)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_U32   cfgCb = WNI_CFG_CHANNEL_BONDING_MODE_DISABLE;
    tANI_U8    channel = 0;
    //                                                                   
    //                                                                 
    if (pBssDesc)
    {
        if (csrLearnCountryInformation(pMac, pBssDesc, pIes, eANI_BOOLEAN_TRUE))
        {
            //                                                       
            pMac->scan.fAmbiguous11dInfoFound = eANI_BOOLEAN_FALSE;
            if (VOS_TRUE == resetCountry)
            {
                csrApplyCountryInformation(pMac, FALSE);
            }
            else
            {
                csrApplyCountryInformation(pMac, TRUE);
            }
        }
        if ((csrIs11dSupported (pMac)) && pIes)
        {
            if (!pIes->Country.present)
            {
                csrResetCountryInformation(pMac, eANI_BOOLEAN_FALSE, eANI_BOOLEAN_FALSE );
            }
            else
            {
                //                                                                 
                //                                               
                palCopyMemory(pMac->hHdd, pMac->scan.currentCountryBssid,
                              pBssDesc->bssId, sizeof(tSirMacAddr));
            }
        }
    }
    //   
    csrSetQosToCfg( pMac, sessionId, pBssConfig->qosType );
    //    
    csrSetCfgSsid(pMac, &pBssConfig->SSID );
    //                  
    //                                                                                                      
    //             
    //                                                                                           

    //                                                                                                                                 
        
    //         
    ccmCfgSetInt(pMac, WNI_CFG_AUTHENTICATION_TYPE, pBssConfig->authType, NULL, eANI_BOOLEAN_FALSE);
    //               
    csrSetCfgPrivacy(pMac, pProfile, (tANI_BOOLEAN)pBssConfig->BssCap.privacy );
    //               
    ccmCfgSetInt(pMac, WNI_CFG_11G_SHORT_SLOT_TIME_ENABLED, pBssConfig->uShortSlotTime, NULL, eANI_BOOLEAN_FALSE);
    //   
    ccmCfgSetInt(pMac, WNI_CFG_11D_ENABLED,
                        ((pBssConfig->f11hSupport) ? pBssConfig->f11hSupport : pProfile->ieee80211d),
                        NULL, eANI_BOOLEAN_FALSE);
    /*     
                                                                                                                  
    */
    ccmCfgSetInt(pMac, WNI_CFG_LOCAL_POWER_CONSTRAINT, pBssConfig->uPowerLimit, NULL, eANI_BOOLEAN_FALSE);
    //  

    if(CSR_IS_INFRA_AP(pProfile) || CSR_IS_WDS_AP(pProfile) || CSR_IS_IBSS(pProfile))
    {
        channel = pProfile->operationChannel;
    }
    else
    {
        if(pBssDesc)
        {
            channel = pBssDesc->channelId;
        }
    }
    if(0 != channel)
    {
        if(CSR_IS_CHANNEL_24GHZ(channel))
        {//                                                        
            cfgCb = WNI_CFG_CHANNEL_BONDING_MODE_DISABLE;
        }
        else
        {
           cfgCb = pBssConfig->cbMode;
        }
    }
#ifdef WLAN_FEATURE_11AC
    //                                                                       
    //                                               
    //                                                                          
    if(cfgCb > PHY_DOUBLE_CHANNEL_HIGH_PRIMARY)
    {
        if(!WDA_getFwWlanFeatCaps(DOT11AC)) {
            cfgCb = csrGetHTCBStateFromVHTCBState(cfgCb);
        }
        else 
        {
            ccmCfgSetInt(pMac, WNI_CFG_VHT_CHANNEL_WIDTH,  pMac->roam.configParam.nVhtChannelWidth, NULL, eANI_BOOLEAN_FALSE);
        }
    }
    else
#endif
    ccmCfgSetInt(pMac, WNI_CFG_CHANNEL_BONDING_MODE, cfgCb, NULL, eANI_BOOLEAN_FALSE);
    //    
    //          
    if(pBssDesc)
    {
        csrSetCfgRateSet(pMac, (eCsrPhyMode)pProfile->phyMode, pProfile, pBssDesc, pIes);
    }
    else
    {
        csrSetCfgRateSetFromProfile(pMac, pProfile);
    }
    //                                                                   
    //             
    csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_CONFIG, sessionId );

    ccmCfgSetInt(pMac, WNI_CFG_JOIN_FAILURE_TIMEOUT, pBssConfig->uJoinTimeOut, (tCcmCfgSetCallback)csrRoamCcmCfgSetCallback, eANI_BOOLEAN_FALSE);
    return (status);
}

eHalStatus csrRoamStopNetwork( tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, 
                               tSirBssDescription *pBssDesc, tDot11fBeaconIEs *pIes)
{
    eHalStatus status;
    tBssConfigParam *pBssConfig;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    
    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    status = palAllocateMemory(pMac->hHdd, (void **)&pBssConfig, sizeof(tBssConfigParam)); 
    if(HAL_STATUS_SUCCESS(status))
    {
        palZeroMemory(pMac->hHdd, pBssConfig, sizeof(tBssConfigParam));
        status = csrRoamPrepareBssConfig(pMac, pProfile, pBssDesc, pBssConfig, pIes);
        if(HAL_STATUS_SUCCESS(status))
        {
            pSession->bssParams.uCfgDot11Mode = pBssConfig->uCfgDot11Mode;
            /*                                                */
            pSession->bssParams.cbMode= pBssConfig->cbMode;
            //                                                  
            if( csrIsBssTypeIBSS(pProfile->BSSType) || CSR_IS_WDS( pProfile )
              || CSR_IS_INFRA_AP(pProfile)
            )
            {
                csrRoamPrepareBssParams(pMac, sessionId, pProfile, pBssDesc, pBssConfig, pIes);
            }
            //                                            
            //                                        
            if ( csrIsConnStateIbss( pMac, sessionId ) ) 
            {
                status = csrRoamIssueStopBss( pMac, sessionId, eCSR_ROAM_SUBSTATE_DISCONNECT_CONTINUE_ROAMING );
            }
            else 
            {
                //                                               
                if ( csrIsConnStateInfra( pMac, sessionId ) ) 
                {
                    //                                                                 
                    //                                                                  
                    //                                      
                    if ( pBssDesc && ( ( csrIsIbssBssDesc( pBssDesc ) ) ||
                          !csrIsSsidEqual( pMac, pSession->pConnectBssDesc, pBssDesc, pIes ) ) )
                    {
                        //                                                                
                        status = csrRoamIssueDisassociate( pMac, sessionId,
                                      eCSR_ROAM_SUBSTATE_DISCONNECT_CONTINUE_ROAMING, FALSE );
                    }
                    else
                    {
                        //                                                                                     
                        //                                                                            
                        if ( pBssDesc )
                        {
                            //                             
                            status = csrRoamSetBssConfigCfg(pMac, sessionId, pProfile,
                                                                 pBssDesc, pBssConfig,
                                                             pIes, eANI_BOOLEAN_FALSE);
                        }
                    }
                }
                else
                {
                    //                                                                                     
                    //                 
                    if ( pBssDesc || CSR_IS_WDS_AP( pProfile )
                     || CSR_IS_INFRA_AP(pProfile)
                    )
                    {
                        tANI_BOOLEAN  is11rRoamingFlag = eANI_BOOLEAN_FALSE;
                        is11rRoamingFlag = csrRoamIs11rAssoc(pMac);
                        //                             
                        status = csrRoamSetBssConfigCfg(pMac, sessionId, pProfile,
                                                              pBssDesc, pBssConfig,
                                                              pIes, is11rRoamingFlag);
                    }
                }
            }
        }//                               
        palFreeMemory(pMac->hHdd, pBssConfig);
    }//               
    return (status);
}

eCsrJoinState csrRoamJoin( tpAniSirGlobal pMac, tANI_U32 sessionId, 
                           tCsrScanResultInfo *pScanResult, tCsrRoamProfile *pProfile )
{
    eCsrJoinState eRoamState = eCsrContinueRoaming;
    eHalStatus status;
    tSirBssDescription *pBssDesc = &pScanResult->BssDescriptor;
    tDot11fBeaconIEs *pIesLocal = (tDot11fBeaconIEs *)( pScanResult->pvIes ); //                
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return (eCsrStopRoaming);
    }
    
    if( CSR_IS_WDS_STA( pProfile ) )
    {
        status = csrRoamStartWds( pMac, sessionId, pProfile, pBssDesc );
        if( !HAL_STATUS_SUCCESS( status ) )
        {
            eRoamState = eCsrStopRoaming;
        }
    }
    else
    {
        if( !pIesLocal && (!HAL_STATUS_SUCCESS(csrGetParsedBssDescriptionIEs(pMac, pBssDesc, &pIesLocal))) )
        {
            smsLog(pMac, LOGE, FL(" fail to parse IEs"));
            return (eCsrStopRoaming);
        }
        if ( csrIsInfraBssDesc( pBssDesc ) ) 
    {
        //                                                                                                           
        //                                                                                                           
        //                                                                                  
                
        if ( csrIsConnStateConnectedInfra(pMac, sessionId) && 
             csrIsBssIdEqual( pMac, pBssDesc, pSession->pConnectBssDesc ) &&
                 csrIsSsidEqual( pMac, pSession->pConnectBssDesc, pBssDesc, pIesLocal )
           )               
        {   
            //                                                                                               
            //                                                                                            
            //                                                                                                
            //                   
            if(csrIsSameProfile(pMac, &pSession->connectedProfile, pProfile))
            {
                smsLog(pMac, LOGW, FL("  detect same profile authType = %d encryType = %d"), pProfile->AuthType, pProfile->EncryptionType);
                if(csrRoamIsSameProfileKeys(pMac, &pSession->connectedProfile, pProfile))
                {
                    eRoamState = eCsrReassocToSelfNoCapChange;
                }
                else
                {
                    tBssConfigParam bssConfig;
                    //               
                    palZeroMemory(pMac->hHdd, &bssConfig, sizeof(bssConfig));
                    status = csrRoamPrepareBssConfig(pMac, pProfile, pBssDesc, &bssConfig, pIesLocal);
                    if(HAL_STATUS_SUCCESS(status))
                    {
                        pSession->bssParams.uCfgDot11Mode = bssConfig.uCfgDot11Mode;
                        pSession->bssParams.cbMode = bssConfig.cbMode;
                        //                                                          
                        status = csrRoamSetBssConfigCfg(pMac, sessionId, pProfile,
                                                        pBssDesc, &bssConfig,
                                                        pIesLocal, eANI_BOOLEAN_FALSE);
                        if(!HAL_STATUS_SUCCESS(status))
                        {
                            eRoamState = eCsrStopRoaming;
                        }
                    }
                    else
                    {
                        eRoamState = eCsrStopRoaming;
                    }
                }//            
            }
            else
            {
                if(!HAL_STATUS_SUCCESS(csrRoamIssueDisassociate( pMac, sessionId,
                                                        eCSR_ROAM_SUBSTATE_DISASSOC_REQ, FALSE )))
                {
                    smsLog(pMac, LOGW, FL("  fail to issue disassociate"));
                    eRoamState = eCsrStopRoaming;
                }
            }
        }
        else
        {
            //                                                                                                    
            //                                                                                               
            //                  
            //
            //
            //                                                                       
                if(!HAL_STATUS_SUCCESS(csrRoamStopNetwork(pMac, sessionId, pProfile, pBssDesc, pIesLocal)))
            {
                eRoamState = eCsrStopRoaming;
            }
        }
        }//     
    else
    {
            if(!HAL_STATUS_SUCCESS(csrRoamStopNetwork(pMac, sessionId, pProfile, pBssDesc, pIesLocal)))
        {
            eRoamState = eCsrStopRoaming;
        }
    }
        if( pIesLocal && !pScanResult->pvIes )
        {
            palFreeMemory(pMac->hHdd, pIesLocal);
        }
    }
    return( eRoamState );
}

eHalStatus csrRoamShouldRoam(tpAniSirGlobal pMac, tANI_U32 sessionId, 
                             tSirBssDescription *pBssDesc, tANI_U32 roamId)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tCsrRoamInfo roamInfo;
    palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
    roamInfo.pBssDesc = pBssDesc;
    status = csrRoamCallCallback(pMac, sessionId, &roamInfo, roamId, eCSR_ROAM_SHOULD_ROAM, eCSR_ROAM_RESULT_NONE);
    return (status);
}
//                                                                  
static void csrRoamAssignDefaultParam( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
    //                                               
    //         
    switch( pCommand->u.roamCmd.roamProfile.AuthType.authType[0] ) //                             
    {
        default:
        case eCSR_AUTH_TYPE_WPA:
        case eCSR_AUTH_TYPE_WPA_PSK:
        case eCSR_AUTH_TYPE_WPA_NONE:
        case eCSR_AUTH_TYPE_OPEN_SYSTEM:
             pCommand->u.roamCmd.roamProfile.negotiatedAuthType = eCSR_AUTH_TYPE_OPEN_SYSTEM;
             break;

        case eCSR_AUTH_TYPE_SHARED_KEY:
             pCommand->u.roamCmd.roamProfile.negotiatedAuthType = eCSR_AUTH_TYPE_SHARED_KEY;
             break;

        case eCSR_AUTH_TYPE_AUTOSWITCH:
             pCommand->u.roamCmd.roamProfile.negotiatedAuthType = eCSR_AUTH_TYPE_AUTOSWITCH;
             break;
    }
    pCommand->u.roamCmd.roamProfile.negotiatedUCEncryptionType = 
    pCommand->u.roamCmd.roamProfile.EncryptionType.encryptionType[0]; 
    //                                                                       
    pCommand->u.roamCmd.roamProfile.negotiatedMCEncryptionType = 
    pCommand->u.roamCmd.roamProfile.EncryptionType.encryptionType[0];
}


static void csrSetAbortRoamingCommand(tpAniSirGlobal pMac, tSmeCmd *pCommand)
{
   switch(pCommand->u.roamCmd.roamReason)
   {
   case eCsrLostLink1:
      pCommand->u.roamCmd.roamReason = eCsrLostLink1Abort;
      break;
   case eCsrLostLink2:
      pCommand->u.roamCmd.roamReason = eCsrLostLink2Abort;
      break;
   case eCsrLostLink3:
      pCommand->u.roamCmd.roamReason = eCsrLostLink3Abort;
      break;
   default:
      smsLog(pMac, LOGE, FL(" aborting roaming reason %d not recognized"),
         pCommand->u.roamCmd.roamReason);
      break;
   }
}

static eCsrJoinState csrRoamJoinNextBss( tpAniSirGlobal pMac, tSmeCmd *pCommand, tANI_BOOLEAN fUseSameBss )
{
    eHalStatus status;
    tCsrScanResult *pScanResult = NULL;
    eCsrJoinState eRoamState = eCsrStopRoaming;
    tScanResultList *pBSSList = (tScanResultList *)pCommand->u.roamCmd.hBSSList;
    tANI_BOOLEAN fDone = eANI_BOOLEAN_FALSE;
    tCsrRoamInfo roamInfo, *pRoamInfo = NULL;
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
    v_U8_t acm_mask = 0;
#endif 
    tANI_U32 sessionId = pCommand->sessionId;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    tCsrRoamProfile *pProfile = &pCommand->u.roamCmd.roamProfile;
    tANI_U8  concurrentChannel = 0;
    
    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return (eCsrStopRoaming);
    }
    
    do  
    {
        //                                                                    
        //                                           
        
        palZeroMemory(pMac->hHdd, &roamInfo, sizeof(roamInfo));
        memcpy (&roamInfo.bssid, &pSession->joinFailStatusCode.bssId, sizeof(tSirMacAddr));
        if(NULL != pBSSList)
        {
            //                                                               
            //                                                  
            if((eANI_BOOLEAN_FALSE == fUseSameBss) || (pCommand->u.roamCmd.pRoamBssEntry == NULL))
            {
                if(pCommand->u.roamCmd.pRoamBssEntry == NULL)
                {
                    //                 
                    pCommand->u.roamCmd.pLastRoamBss = NULL;
                    pCommand->u.roamCmd.pRoamBssEntry = csrLLPeekHead(&pBSSList->List, LL_ACCESS_LOCK);
                }
                else
                {
                    pCommand->u.roamCmd.pRoamBssEntry = csrLLNext(&pBSSList->List, pCommand->u.roamCmd.pRoamBssEntry, LL_ACCESS_LOCK);
                    if(NULL == pCommand->u.roamCmd.pRoamBssEntry)
                    {
                        //                      
                        //                                            
                        break;
                    }
                    else
                    {
                        //                                                          
                        //                                                           
                        roamInfo.pBssDesc = pCommand->u.roamCmd.pLastRoamBss;     //                      
                        roamInfo.statusCode = pSession->joinFailStatusCode.statusCode;
                        roamInfo.reasonCode = pSession->joinFailStatusCode.reasonCode;
                        pRoamInfo = &roamInfo;
                    }
                }
                while(pCommand->u.roamCmd.pRoamBssEntry)
                {
                    pScanResult = GET_BASE_ADDR(pCommand->u.roamCmd.pRoamBssEntry, tCsrScanResult, Link);
                    /*                                                                    */
                    /*                                                 */
                    if (vos_concurrent_sessions_running() && 
                        !csrIsValidMcConcurrentSession(pMac, sessionId, &pScanResult->Result.BssDescriptor))
                    {
                        concurrentChannel = 
                            csrGetConcurrentOperationChannel(pMac);
                        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH, "%s: "
                                " csr Concurrent Channel = %d", __func__, concurrentChannel);
                        if ((concurrentChannel) && 
                                (concurrentChannel == 
                                 pScanResult->Result.BssDescriptor.channelId))
                        {
                            //                                       
                            //                                        
                            //                        
                            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
                                    FL("Concurrent channel match =%d"),
                                    concurrentChannel);
                            concurrentChannel = 0; 
                        }
                    }

                    if (!concurrentChannel)
                    {
                        
                        if(HAL_STATUS_SUCCESS(csrRoamShouldRoam(pMac,
                            sessionId, &pScanResult->Result.BssDescriptor,
                            pCommand->u.roamCmd.roamId)))
                        {
                            //               
                            break;
                        }
                     }
                     else
                     {
                         eRoamState = eCsrStopRoamingDueToConcurrency;
                     }
                    pCommand->u.roamCmd.pRoamBssEntry = csrLLNext(&pBSSList->List, pCommand->u.roamCmd.pRoamBssEntry, LL_ACCESS_LOCK);
                    if(NULL == pCommand->u.roamCmd.pRoamBssEntry)
                    {
                        //                      
                        fDone = eANI_BOOLEAN_TRUE;
                        break;
                    }
                }
                if(fDone)
                {
                    break;
                }
            }
        }
        //                                                     
        //                                                                
        //                                            
        if( CSR_IS_INFRASTRUCTURE( pProfile ) )
        {
            if(pRoamInfo)
            {
                if(pSession->bRefAssocStartCnt)
                {
                    pSession->bRefAssocStartCnt--;
                    //                                                                           
                    csrRoamCallCallback(pMac, sessionId, pRoamInfo, pCommand->u.roamCmd.roamId,
                                        eCSR_ROAM_ASSOCIATION_COMPLETION,
                                        eCSR_ROAM_RESULT_NOT_ASSOCIATED);
                }
            }
            /*                                                                */
            if ( !CSR_IS_ROAMING(pSession) && CSR_IS_ROAMING_COMMAND(pCommand) )
            {
                //                                                        
                smsLog(pMac, LOGW, FL("  Roam command (reason %d) aborted due to roaming completed"),
                        pCommand->u.roamCmd.roamReason);
                eRoamState = eCsrStopRoaming;
                csrSetAbortRoamingCommand(pMac, pCommand);
                break;
            }
            palZeroMemory(pMac->hHdd, &roamInfo, sizeof(roamInfo));
            if(pScanResult)
            {
                tDot11fBeaconIEs *pIesLocal = (tDot11fBeaconIEs *)pScanResult->Result.pvIes;
                if( !pIesLocal && (!HAL_STATUS_SUCCESS(csrGetParsedBssDescriptionIEs(pMac, &pScanResult->Result.BssDescriptor, &pIesLocal))) )
                {
                    smsLog(pMac, LOGE, FL(" cannot parse IEs"));
                    fDone = eANI_BOOLEAN_TRUE;
                    eRoamState = eCsrStopRoaming;
                    break;
                }
                roamInfo.pBssDesc = &pScanResult->Result.BssDescriptor;
                pCommand->u.roamCmd.pLastRoamBss = roamInfo.pBssDesc;
                //                                                             
                if( pCommand->u.roamCmd.roamProfile.uapsd_mask &&
                    CSR_IS_QOS_BSS(pIesLocal) &&
                    CSR_IS_UAPSD_BSS(pIesLocal) )
                {
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
                    acm_mask = sme_QosGetACMMask(pMac, &pScanResult->Result.BssDescriptor, 
                         pIesLocal);
#endif /*                            */
                }
                else
                {
                    pCommand->u.roamCmd.roamProfile.uapsd_mask = 0;
                }
                if( pIesLocal && !pScanResult->Result.pvIes)
                {
                    palFreeMemory(pMac->hHdd, pIesLocal);
                }
            }
            else
            {
                pCommand->u.roamCmd.roamProfile.uapsd_mask = 0;
            }
            roamInfo.pProfile = pProfile;
            pSession->bRefAssocStartCnt++;
            csrRoamCallCallback( pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, 
                                 eCSR_ROAM_ASSOCIATION_START, eCSR_ROAM_RESULT_NONE );
        }
        if ( NULL == pCommand->u.roamCmd.pRoamBssEntry ) 
        {
            //                                                                 
            if ( CSR_IS_START_IBSS(pProfile) ) 
            {
                tANI_BOOLEAN fSameIbss = eANI_BOOLEAN_FALSE;
                //                              
                csrRoamAssignDefaultParam( pMac, pCommand );
                status = csrRoamStartIbss( pMac, sessionId, pProfile, &fSameIbss );
                if(HAL_STATUS_SUCCESS(status))
                {
                    if ( fSameIbss ) 
                    {
                        eRoamState = eCsrStartIbssSameIbss;
                    }
                    else
                    {
                        eRoamState = eCsrContinueRoaming;
                    }
                }
                else
                {
                    //                            
                    eRoamState = eCsrStopRoaming;
                }
                break;
            }
            else if ( (CSR_IS_WDS_AP(pProfile))
             || (CSR_IS_INFRA_AP(pProfile))
            )
            {
                //                             
                csrRoamAssignDefaultParam( pMac, pCommand );
                /*                                             */
                status = csrRoamStartWds( pMac, sessionId, pProfile, NULL );
                if(HAL_STATUS_SUCCESS(status))
                {
                    eRoamState = eCsrContinueRoaming;
                }
                else 
                {
                    //                            
                    eRoamState = eCsrStopRoaming;
                }
            }
            else 
            {
                //                 
                smsLog(pMac, LOGW, FL("cannot continue without BSS list"));
                eRoamState = eCsrStopRoaming;
                break;
            }
        } 
        else //            
        {
            //                                                                    
            pScanResult = GET_BASE_ADDR(pCommand->u.roamCmd.pRoamBssEntry, tCsrScanResult, Link);
            pCommand->u.roamCmd.roamProfile.negotiatedUCEncryptionType = pScanResult->ucEncryptionType; //                                      
            pCommand->u.roamCmd.roamProfile.negotiatedMCEncryptionType = pScanResult->mcEncryptionType;
            pCommand->u.roamCmd.roamProfile.negotiatedAuthType = pScanResult->authType;
            if ( CSR_IS_START_IBSS(&pCommand->u.roamCmd.roamProfile) )
            {
                if(csrIsSameProfile(pMac, &pSession->connectedProfile, pProfile))
                {
                    eRoamState = eCsrStartIbssSameIbss;
                    break;
                } 
            }
            if( pCommand->u.roamCmd.fReassocToSelfNoCapChange )
            {
                //                                              
                pCommand->u.roamCmd.fReassocToSelfNoCapChange = eANI_BOOLEAN_FALSE;
                eRoamState = eCsrReassocToSelfNoCapChange;
                break;
            }
            //                            
            eRoamState = csrRoamJoin( pMac, sessionId, &pScanResult->Result, pProfile );
            break;
        }
        
    } while( 0 );
    if( (eCsrStopRoaming == eRoamState) && (CSR_IS_INFRASTRUCTURE( pProfile )) )
    {
        //                                                                          
        if(pSession->bRefAssocStartCnt > 0)
        {
            pSession->bRefAssocStartCnt--;
            //                                                                           
            pRoamInfo = &roamInfo;
            csrRoamCallCallback(pMac, sessionId, pRoamInfo, pCommand->u.roamCmd.roamId, 
                                        eCSR_ROAM_ASSOCIATION_COMPLETION, 
                                        eCSR_ROAM_RESULT_NOT_ASSOCIATED);
        }
    }

    return( eRoamState );
}

static eHalStatus csrRoam( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    eCsrJoinState RoamState;
    tANI_U32 sessionId = pCommand->sessionId;
    
    //                                      
    {
        //                        
        RoamState = csrRoamJoinNextBss( pMac, pCommand, eANI_BOOLEAN_FALSE );

        //                     
        if (( eCsrStopRoaming == RoamState ) || ( eCsrStopRoamingDueToConcurrency == RoamState))
        {
            tANI_BOOLEAN fComplete = eANI_BOOLEAN_FALSE;
            //                                           
            if ( csrIsConnStateInfra(pMac, sessionId) ) 
            {
                //                                          
                status = csrRoamIssueDisassociate( pMac, sessionId, eCSR_ROAM_SUBSTATE_DISASSOC_NOTHING_TO_JOIN, FALSE );
                if(!HAL_STATUS_SUCCESS(status))
                {
                    smsLog(pMac, LOGW, FL("  failed to issue disassociate, status = %d"), status);
                    //                                                      
                    fComplete = eANI_BOOLEAN_TRUE;
                }
            }
            else if( csrIsConnStateIbss(pMac, sessionId) )
            {
                status = csrRoamIssueStopBss( pMac, sessionId, eCSR_ROAM_SUBSTATE_STOP_BSS_REQ );
                if(!HAL_STATUS_SUCCESS(status))
                {
                    smsLog(pMac, LOGW, FL("  failed to issue stop bss, status = %d"), status);
                    //                                                      
                    fComplete = eANI_BOOLEAN_TRUE;
                }
            }
            else if (csrIsConnStateConnectedInfraAp(pMac, sessionId))
            {
                status = csrRoamIssueStopBss( pMac, sessionId, eCSR_ROAM_SUBSTATE_STOP_BSS_REQ );
                if(!HAL_STATUS_SUCCESS(status))
                {
                    smsLog(pMac, LOGW, FL("  failed to issue stop bss, status = %d"), status);
                    //                                                      
                    fComplete = eANI_BOOLEAN_TRUE;
                }
            }
            else
            {        
                fComplete = eANI_BOOLEAN_TRUE;
            }
            if(fComplete)
            {
               //                                                      
               if(eCsrStopRoamingDueToConcurrency == RoamState)
               {
                   csrRoamComplete( pMac, eCsrJoinFailureDueToConcurrency, NULL );
               }
               else
               {
                   csrRoamComplete( pMac, eCsrNothingToJoin, NULL );
               }
            }    
       }
       else if ( eCsrReassocToSelfNoCapChange == RoamState )
       {
           csrRoamComplete( pMac, eCsrSilentlyStopRoamingSaveState, NULL );
       }
       else if ( eCsrStartIbssSameIbss == RoamState )
       {
           csrRoamComplete( pMac, eCsrSilentlyStopRoaming, NULL );        
       }
    }//                 
    
    return status;
}
eHalStatus csrProcessFTReassocRoamCommand ( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
    tANI_U32 sessionId;
    tCsrRoamSession *pSession;
    tCsrScanResult *pScanResult = NULL;
    tSirBssDescription *pBssDesc = NULL;
    eHalStatus status = eHAL_STATUS_SUCCESS;
    sessionId = pCommand->sessionId;
    pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    if(CSR_IS_ROAMING(pSession) && pSession->fCancelRoaming)
    {
        //                                                     
        smsLog(pMac, LOG1, FL("  Roam command cancelled"));
        csrRoamComplete(pMac, eCsrNothingToJoin, NULL); 
        return eHAL_STATUS_FAILURE;
    }
    if (pCommand->u.roamCmd.pRoamBssEntry)
    {
        pScanResult = GET_BASE_ADDR(pCommand->u.roamCmd.pRoamBssEntry, tCsrScanResult, Link);
        pBssDesc = &pScanResult->Result.BssDescriptor;
    }
    else
    {
        //                                                     
        smsLog(pMac, LOG1, FL("  Roam command cancelled"));
        csrRoamComplete(pMac, eCsrNothingToJoin, NULL); 
        return eHAL_STATUS_FAILURE;
    }
    status = csrRoamIssueReassociate(pMac, sessionId, pBssDesc, 
        (tDot11fBeaconIEs *)( pScanResult->Result.pvIes ), &pCommand->u.roamCmd.roamProfile);
    return status;
}

eHalStatus csrRoamProcessCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tCsrRoamInfo roamInfo;
    tANI_U32 sessionId = pCommand->sessionId;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    switch ( pCommand->u.roamCmd.roamReason )
    {
    case eCsrForcedDisassoc:
        status = csrRoamProcessDisassocDeauth( pMac, pCommand, TRUE, FALSE );
        csrFreeRoamProfile(pMac, sessionId);
        break;
     case eCsrSmeIssuedDisassocForHandoff:
        //                                                                                                     
#if 0 //                           
        status = csrRoamProcessDisassociate( pMac, pCommand, FALSE );
#else
        status = csrRoamProcessDisassocDeauth( pMac, pCommand, TRUE, FALSE );
#endif

        break;
    case eCsrForcedDisassocMICFailure:
        status = csrRoamProcessDisassocDeauth( pMac, pCommand, TRUE, TRUE );
        csrFreeRoamProfile(pMac, sessionId);
        break;
    case eCsrForcedDeauth:
        status = csrRoamProcessDisassocDeauth( pMac, pCommand, FALSE, FALSE );
        csrFreeRoamProfile(pMac, sessionId);
        break;
    case eCsrHddIssuedReassocToSameAP:
    case eCsrSmeIssuedReassocToSameAP:
    {
        tDot11fBeaconIEs *pIes = NULL;

        if( pSession->pConnectBssDesc )
        {
            status = csrGetParsedBssDescriptionIEs(pMac, pSession->pConnectBssDesc, &pIes);
            if(!HAL_STATUS_SUCCESS(status) )
            {
                smsLog(pMac, LOGE, FL("  fail to parse IEs"));
            }
            else
            {
                roamInfo.reasonCode = eCsrRoamReasonStaCapabilityChanged;
                csrRoamCallCallback(pMac, pSession->sessionId, &roamInfo, 0, eCSR_ROAM_ROAMING_START, eCSR_ROAM_RESULT_NONE);
                pSession->roamingReason = eCsrReassocRoaming;
                roamInfo.pBssDesc = pSession->pConnectBssDesc;
                roamInfo.pProfile = &pCommand->u.roamCmd.roamProfile;
                pSession->bRefAssocStartCnt++;
                csrRoamCallCallback( pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, 
                                     eCSR_ROAM_ASSOCIATION_START, eCSR_ROAM_RESULT_NONE );
   
                smsLog(pMac, LOG1, FL("  calling csrRoamIssueReassociate"));
                status = csrRoamIssueReassociate( pMac, sessionId, pSession->pConnectBssDesc, pIes,
                                                  &pCommand->u.roamCmd.roamProfile );
                if(!HAL_STATUS_SUCCESS(status))
                {
                    smsLog(pMac, LOGE, FL("csrRoamIssueReassociate failed with status %d"), status);
                    csrReleaseCommandRoam( pMac, pCommand );
                }

                palFreeMemory(pMac->hHdd, pIes);
                pIes = NULL;
            }
        }
        break;
    }
    case eCsrCapsChange:
        smsLog(pMac, LOGE, FL("received eCsrCapsChange "));
        csrRoamStateChange( pMac, eCSR_ROAMING_STATE_JOINING, sessionId );
        status = csrRoamIssueDisassociate( pMac, sessionId, eCSR_ROAM_SUBSTATE_DISCONNECT_CONTINUE_ROAMING, FALSE); 
        break;
    case eCsrSmeIssuedFTReassoc:
        smsLog(pMac, LOG1, FL("received FT Reassoc Req "));
        status = csrProcessFTReassocRoamCommand(pMac, pCommand);
        break;

    case eCsrStopBss:
       csrRoamStateChange( pMac, eCSR_ROAMING_STATE_JOINING, sessionId);
       status = csrRoamIssueStopBss( pMac, sessionId, eCSR_ROAM_SUBSTATE_STOP_BSS_REQ );
       break;

    case eCsrForcedDisassocSta:
       csrRoamStateChange( pMac, eCSR_ROAMING_STATE_JOINING, sessionId);
       csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_DISASSOC_REQ, sessionId);
       status = csrSendMBDisassocReqMsg( pMac, sessionId, pCommand->u.roamCmd.peerMac, 
                     pCommand->u.roamCmd.reason);
       break;

    case eCsrForcedDeauthSta:
       csrRoamStateChange( pMac, eCSR_ROAMING_STATE_JOINING, sessionId);
       csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_DEAUTH_REQ, sessionId);
       status = csrSendMBDeauthReqMsg( pMac, sessionId, pCommand->u.roamCmd.peerMac, 
                     pCommand->u.roamCmd.reason);
       break;

    case eCsrPerformPreauth:
        smsLog(pMac, LOG1, FL("Attempting FT PreAuth Req"));
        status = csrRoamIssueFTPreauthReq(pMac, sessionId, 
                pCommand->u.roamCmd.pLastRoamBss);
        break;

    default:
        csrRoamStateChange( pMac, eCSR_ROAMING_STATE_JOINING, sessionId );

        if( pCommand->u.roamCmd.fUpdateCurRoamProfile )
        {
            //                             
            csrFreeRoamProfile(pMac, sessionId);
            if(HAL_STATUS_SUCCESS(palAllocateMemory(pMac->hHdd, (void **)&pSession->pCurRoamProfile, sizeof(tCsrRoamProfile))))
            {
                palZeroMemory(pMac->hHdd, pSession->pCurRoamProfile, sizeof(tCsrRoamProfile));
                csrRoamCopyProfile(pMac, pSession->pCurRoamProfile, &pCommand->u.roamCmd.roamProfile);
            }
        }
 
        //                                                              
        //                                                         
 
        //                                                            
        status = csrRoam( pMac, pCommand );
        if(!HAL_STATUS_SUCCESS(status))
        {
            smsLog(pMac, LOGW, FL("csrRoam() failed with status = 0x%08X"), status);
        }
        break;
    }
    return (status);
}

void csrReinitPreauthCmd(tpAniSirGlobal pMac, tSmeCmd *pCommand) 
{
    pCommand->u.roamCmd.pLastRoamBss = NULL;
    pCommand->u.roamCmd.pRoamBssEntry = NULL;
    //                                                                  
    palZeroMemory(pMac->hHdd, &pCommand->u.roamCmd, sizeof(tRoamCmd));
}

void csrReinitRoamCmd(tpAniSirGlobal pMac, tSmeCmd *pCommand) 
{
    if(pCommand->u.roamCmd.fReleaseBssList)
    {
        csrScanResultPurge(pMac, pCommand->u.roamCmd.hBSSList);
        pCommand->u.roamCmd.fReleaseBssList = eANI_BOOLEAN_FALSE;
        pCommand->u.roamCmd.hBSSList = CSR_INVALID_SCANRESULT_HANDLE;
    }
    if(pCommand->u.roamCmd.fReleaseProfile)
    {
        csrReleaseProfile(pMac, &pCommand->u.roamCmd.roamProfile);
        pCommand->u.roamCmd.fReleaseProfile = eANI_BOOLEAN_FALSE;
    }
    pCommand->u.roamCmd.pRoamBssEntry = NULL;
    //                                                                  
    palZeroMemory(pMac->hHdd, &pCommand->u.roamCmd, sizeof(tRoamCmd));
}

void csrReinitWmStatusChangeCmd(tpAniSirGlobal pMac, tSmeCmd *pCommand)
{
    palZeroMemory(pMac->hHdd, &pCommand->u.wmStatusChangeCmd, sizeof(tWmStatusChangeCmd));
}
void csrRoamComplete( tpAniSirGlobal pMac, eCsrRoamCompleteResult Result, void *Context )
{
    tListElem *pEntry;
    tSmeCmd *pCommand;
    tANI_BOOLEAN fReleaseCommand = eANI_BOOLEAN_TRUE;
    smsLog( pMac, LOG2, "Roam Completion ..." );
    pEntry = csrLLPeekHead( &pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK );
    if ( pEntry )
    {
        pCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
        //                                                                    
        //                                
        if ( eSmeCommandRoam == pCommand->command )
        {
            //                                                                                              
            //                                                                                         
            fReleaseCommand = csrRoamProcessResults( pMac, pCommand, Result, Context );
            if( fReleaseCommand )
            {
                if( csrLLRemoveEntry( &pMac->sme.smeCmdActiveList, pEntry, LL_ACCESS_LOCK ) )
                {
                    csrReleaseCommandRoam( pMac, pCommand );
                }
                else
                {
                    smsLog( pMac, LOGE, " **********csrRoamComplete fail to release command reason %d",
                           pCommand->u.roamCmd.roamReason );
                }
            }
            else
            {
                smsLog( pMac, LOGE, " **********csrRoamComplete fail to release command reason %d",
                       pCommand->u.roamCmd.roamReason );
            }
        }
        else
        {
            smsLog( pMac, LOGW, "CSR: Roam Completion called but ROAM command is not ACTIVE ..." );
        }
    }
    else
    {
        smsLog( pMac, LOGW, "CSR: Roam Completion called but NO commands are ACTIVE ..." );
    }
    if( fReleaseCommand )
    {
        smeProcessPendingQueue( pMac );
    }
}

void csrResetPMKIDCandidateList( tpAniSirGlobal pMac, tANI_U32 sessionId )
{
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return;
    }
    palZeroMemory( pMac->hHdd, &(pSession->PmkidCandidateInfo[0]), sizeof(tPmkidCandidateInfo) * CSR_MAX_PMKID_ALLOWED );
    pSession->NumPmkidCandidate = 0;
}
#ifdef FEATURE_WLAN_WAPI
void csrResetBKIDCandidateList( tpAniSirGlobal pMac, tANI_U32 sessionId )
{
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return;
    }
    palZeroMemory( pMac->hHdd, &(pSession->BkidCandidateInfo[0]), sizeof(tBkidCandidateInfo) * CSR_MAX_BKID_ALLOWED );
    pSession->NumBkidCandidate = 0;
}
#endif /*                   */
extern tANI_U8 csrWpaOui[][ CSR_WPA_OUI_SIZE ];

static eHalStatus csrRoamSaveSecurityRspIE(tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrAuthType authType, 
                                         tSirBssDescription *pSirBssDesc,
                                         tDot11fBeaconIEs *pIes)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    tDot11fBeaconIEs *pIesLocal = pIes;

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    if((eCSR_AUTH_TYPE_WPA == authType) ||
        (eCSR_AUTH_TYPE_WPA_PSK == authType) ||
        (eCSR_AUTH_TYPE_RSN == authType) ||
        (eCSR_AUTH_TYPE_RSN_PSK == authType)
#if defined WLAN_FEATURE_VOWIFI_11R
      ||
       (eCSR_AUTH_TYPE_FT_RSN == authType) ||
       (eCSR_AUTH_TYPE_FT_RSN_PSK == authType)
#endif /*                   */
#ifdef FEATURE_WLAN_WAPI 
      ||
       (eCSR_AUTH_TYPE_WAPI_WAI_PSK == authType) ||
       (eCSR_AUTH_TYPE_WAPI_WAI_CERTIFICATE == authType)
#endif /*                   */
#ifdef WLAN_FEATURE_11W
      ||
       (eCSR_AUTH_TYPE_RSN_PSK_SHA256 == authType)
#endif /*                   */
        )
    {
        if( !pIesLocal && (!HAL_STATUS_SUCCESS(csrGetParsedBssDescriptionIEs(pMac, pSirBssDesc, &pIesLocal))) )
        {
            smsLog(pMac, LOGE, FL(" cannot parse IEs"));
        }
        if( pIesLocal )
        {
            tANI_U32 nIeLen;
            tANI_U8 *pIeBuf;
            if((eCSR_AUTH_TYPE_RSN == authType) ||
#if defined WLAN_FEATURE_VOWIFI_11R
                (eCSR_AUTH_TYPE_FT_RSN == authType) ||
                (eCSR_AUTH_TYPE_FT_RSN_PSK == authType) ||
#endif /*                         */
#if defined WLAN_FEATURE_11W
                (eCSR_AUTH_TYPE_RSN_PSK_SHA256 == authType) ||
#endif
                (eCSR_AUTH_TYPE_RSN_PSK == authType))
            {
                if(pIesLocal->RSN.present)
                {
                    //                           
                    nIeLen = 8 //                                                    
                        + pIesLocal->RSN.pwise_cipher_suite_count * 4    //                   
                        + 2 //               
                        + pIesLocal->RSN.akm_suite_count * 4 //          
                        + 2; //        
                    if( pIesLocal->RSN.pmkid_count )
                    {
                        nIeLen += 2 + pIesLocal->RSN.pmkid_count * 4;  //     
                    }
                    //                                          
                    if(HAL_STATUS_SUCCESS((status = palAllocateMemory(pMac->hHdd, (void **)&pSession->pWpaRsnRspIE, nIeLen + 2))))
                    {
                        palZeroMemory(pMac->hHdd, pSession->pWpaRsnRspIE, nIeLen + 2);
                        pSession->pWpaRsnRspIE[0] = DOT11F_EID_RSN;
                        pSession->pWpaRsnRspIE[1] = (tANI_U8)nIeLen;
                        //                    
                        pIeBuf = pSession->pWpaRsnRspIE + 2;
                        palCopyMemory(pMac->hHdd, pIeBuf, &pIesLocal->RSN.version, sizeof(pIesLocal->RSN.version));
                        pIeBuf += sizeof(pIesLocal->RSN.version);
                        palCopyMemory(pMac->hHdd, pIeBuf, &pIesLocal->RSN.gp_cipher_suite, sizeof(pIesLocal->RSN.gp_cipher_suite));
                        pIeBuf += sizeof(pIesLocal->RSN.gp_cipher_suite);
                        palCopyMemory(pMac->hHdd, pIeBuf, &pIesLocal->RSN.pwise_cipher_suite_count, sizeof(pIesLocal->RSN.pwise_cipher_suite_count));
                        pIeBuf += sizeof(pIesLocal->RSN.pwise_cipher_suite_count );
                        if( pIesLocal->RSN.pwise_cipher_suite_count )
                        {
                            //                        
                            palCopyMemory(pMac->hHdd, pIeBuf, pIesLocal->RSN.pwise_cipher_suites, 
                                            pIesLocal->RSN.pwise_cipher_suite_count * 4);
                            pIeBuf += pIesLocal->RSN.pwise_cipher_suite_count * 4;
                        }
                        palCopyMemory(pMac->hHdd, pIeBuf, &pIesLocal->RSN.akm_suite_count, 2);
                        pIeBuf += 2;
                        if( pIesLocal->RSN.akm_suite_count )
                        {
                            //               
                            palCopyMemory(pMac->hHdd, pIeBuf, pIesLocal->RSN.akm_suites, 
                                            pIesLocal->RSN.akm_suite_count * 4);
                            pIeBuf += pIesLocal->RSN.akm_suite_count * 4;
                        }
                        //             
                        palCopyMemory(pMac->hHdd, pIeBuf, pIesLocal->RSN.akm_suites + 
                                        pIesLocal->RSN.akm_suite_count * 4, 
                                        2 + pIesLocal->RSN.pmkid_count * 4);
                        pSession->nWpaRsnRspIeLength = nIeLen + 2; 
                    }
                }
            }
            else if((eCSR_AUTH_TYPE_WPA == authType) ||
                (eCSR_AUTH_TYPE_WPA_PSK == authType))
            {
                if(pIesLocal->WPA.present)
                {
                    //                           
                    nIeLen = 12 //                                                       
                        + pIesLocal->WPA.unicast_cipher_count * 4    //               
                        + 2 //                
                        + pIesLocal->WPA.auth_suite_count * 4; //           
                    //                                                            
                    //                                                           
                    //                                                           
                    //                                 
                    //                                          
                    if(HAL_STATUS_SUCCESS((status = palAllocateMemory(pMac->hHdd, (void **)&pSession->pWpaRsnRspIE, nIeLen + 2))))
                    {
                        pSession->pWpaRsnRspIE[0] = DOT11F_EID_WPA;
                        pSession->pWpaRsnRspIE[1] = (tANI_U8)nIeLen;
                        pIeBuf = pSession->pWpaRsnRspIE + 2;
                        //            
                        palCopyMemory(pMac->hHdd, pIeBuf, &csrWpaOui[1], 4);
                        pIeBuf += 4;
                        palCopyMemory(pMac->hHdd, pIeBuf, &pIesLocal->WPA.version, 8 + 
                                        pIesLocal->WPA.unicast_cipher_count * 4);
                        pIeBuf += 8 + pIesLocal->WPA.unicast_cipher_count * 4;
                        palCopyMemory(pMac->hHdd, pIeBuf, &pIesLocal->WPA.auth_suite_count, 2 + 
                                        pIesLocal->WPA.auth_suite_count * 4);
                        pIeBuf += pIesLocal->WPA.auth_suite_count * 4;
                        pSession->nWpaRsnRspIeLength = nIeLen + 2; 
                    }
                }
            }
#ifdef FEATURE_WLAN_WAPI
          else if((eCSR_AUTH_TYPE_WAPI_WAI_PSK == authType) ||
                  (eCSR_AUTH_TYPE_WAPI_WAI_CERTIFICATE == authType))
          {
                if(pIesLocal->WAPI.present)
                {
                   //                           
                   nIeLen = 4 //                          
                      + pIesLocal->WAPI.akm_suite_count * 4 //           
                      + 2 //                        
                      + pIesLocal->WAPI.unicast_cipher_suite_count * 4    //                   
                      + 6; //                                    
                      if( pIesLocal->WAPI.bkid_count )
                      {
                           nIeLen += 2 + pIesLocal->WAPI.bkid_count * 4;  //    
        }
                      
                   //                                          
                   if(HAL_STATUS_SUCCESS((status = palAllocateMemory(pMac->hHdd, (void **)&pSession->pWapiRspIE, nIeLen + 2))))
                   {
                      pSession->pWapiRspIE[0] = DOT11F_EID_WAPI;
                      pSession->pWapiRspIE[1] = (tANI_U8)nIeLen;
                      pIeBuf = pSession->pWapiRspIE + 2;
                      //                         
                      palCopyMemory(pMac->hHdd, pIeBuf, &pIesLocal->WAPI.version, 2);
                      pIeBuf += 4;
                      if( pIesLocal->WAPI.akm_suite_count )
                      {
                         //               
                         palCopyMemory(pMac->hHdd, pIeBuf, pIesLocal->WAPI.akm_suites, 
                                        pIesLocal->WAPI.akm_suite_count * 4);
                         pIeBuf += pIesLocal->WAPI.akm_suite_count * 4;
    }
                      palCopyMemory(pMac->hHdd, pIeBuf, &pIesLocal->WAPI.unicast_cipher_suite_count, 2);
                      pIeBuf += 2;
                      if( pIesLocal->WAPI.unicast_cipher_suite_count )
                      {
                         //                        
                         palCopyMemory(pMac->hHdd, pIeBuf, pIesLocal->WAPI.unicast_cipher_suites, 
                                        pIesLocal->WAPI.unicast_cipher_suite_count * 4);
                         pIeBuf += pIesLocal->WAPI.unicast_cipher_suite_count * 4;
                      }
                      //               
                      palCopyMemory(pMac->hHdd, pIeBuf, pIesLocal->WAPI.multicast_cipher_suite, 4);
                      pIeBuf += 4;
                      //                  
                      palCopyMemory(pMac->hHdd, pIeBuf, pIesLocal->WAPI.multicast_cipher_suite + 4, 2);
                      pIeBuf += 2;
                      //          
                      palCopyMemory(pMac->hHdd, pIeBuf, &pIesLocal->WAPI.bkid_count, 2);
                      pIeBuf += 2;
                      if( pIesLocal->WAPI.bkid_count )
                      {
                         //               
                         palCopyMemory(pMac->hHdd, pIeBuf, pIesLocal->WAPI.bkid, pIesLocal->WAPI.bkid_count * 4);
                         pIeBuf += pIesLocal->WAPI.bkid_count * 4;
                      }
                      pSession->nWapiRspIeLength = nIeLen + 2;
                   }
                }
          }
#endif /*                   */
            if( !pIes )
            {
                //                 
                palFreeMemory(pMac->hHdd, pIesLocal);
        }
    }
    }
    return (status);
}

static void csrCheckAndUpdateACWeight( tpAniSirGlobal pMac, tDot11fBeaconIEs *pIEs )
{
    v_U8_t bACWeights[WLANTL_MAX_AC];
    v_U8_t paramBk, paramBe, paramVi, paramVo;
    v_BOOL_t fWeightChange = VOS_FALSE;
    //                                                                   
    //                                                                         
    //                                                                             
    //                                                                           
    if( pIEs->WMMParams.present )
    {
        //                            
        bACWeights[WLANTL_AC_BK] = pMac->roam.ucACWeights[WLANTL_AC_BK];
        bACWeights[WLANTL_AC_BE] = pMac->roam.ucACWeights[WLANTL_AC_BE];
        bACWeights[WLANTL_AC_VI] = pMac->roam.ucACWeights[WLANTL_AC_VI];
        bACWeights[WLANTL_AC_VO] = pMac->roam.ucACWeights[WLANTL_AC_VO];
        paramBk = pIEs->WMMParams.acbk_aifsn + pIEs->WMMParams.acbk_acwmin;
        paramBe = pIEs->WMMParams.acbe_aifsn + pIEs->WMMParams.acbe_acwmin;
        paramVi = pIEs->WMMParams.acvi_aifsn + pIEs->WMMParams.acvi_acwmin;
        paramVo = pIEs->WMMParams.acvo_aifsn + pIEs->WMMParams.acvo_acwmin;
        if( SME_DETECT_AC_WEIGHT_DIFF(paramBk, paramBe) )
        {
            bACWeights[WLANTL_AC_BE] = bACWeights[WLANTL_AC_BK];
            fWeightChange = VOS_TRUE;
        }
        if( SME_DETECT_AC_WEIGHT_DIFF(paramBk, paramVi) )
        {
            bACWeights[WLANTL_AC_VI] = bACWeights[WLANTL_AC_BK];
            fWeightChange = VOS_TRUE;
        }
        else if( SME_DETECT_AC_WEIGHT_DIFF(paramBe, paramVi) )
        {
            bACWeights[WLANTL_AC_VI] = bACWeights[WLANTL_AC_BE];
            fWeightChange = VOS_TRUE;
        }
        if( SME_DETECT_AC_WEIGHT_DIFF(paramBk, paramVo) )
        {
            bACWeights[WLANTL_AC_VO] = bACWeights[WLANTL_AC_BK];
            fWeightChange = VOS_TRUE;
        }
        else if( SME_DETECT_AC_WEIGHT_DIFF(paramBe, paramVo) )
        {
            bACWeights[WLANTL_AC_VO] = bACWeights[WLANTL_AC_BE];
            fWeightChange = VOS_TRUE;
        }
        else if( SME_DETECT_AC_WEIGHT_DIFF(paramVi, paramVo) )
        {
            bACWeights[WLANTL_AC_VO] = bACWeights[WLANTL_AC_VI];
            fWeightChange = VOS_TRUE;
        }
        if(fWeightChange)
        {
            smsLog(pMac, LOGE, FL(" change AC weights (%d-%d-%d-%d)"), bACWeights[0], bACWeights[1],
                bACWeights[2], bACWeights[3]);
            WLANTL_SetACWeights(pMac->roam.gVosContext, bACWeights);
        }
    }
}
#ifdef WLAN_FEATURE_VOWIFI_11R
//                                                             
tANI_BOOLEAN csrRoamIs11rAssoc(tpAniSirGlobal pMac)
{
#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
    return csrNeighborRoamIs11rAssoc(pMac);
#else
    return eANI_BOOLEAN_FALSE;
#endif
}
#endif
#ifdef FEATURE_WLAN_CCX
//                                                             
tANI_BOOLEAN csrRoamIsCCXAssoc(tpAniSirGlobal pMac)
{
#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
    return csrNeighborRoamIsCCXAssoc(pMac);
#else
    return eANI_BOOLEAN_FALSE;
#endif
}
#endif
#ifdef FEATURE_WLAN_LFR
//                                                                   
tANI_BOOLEAN csrRoamIsFastRoamEnabled(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    tCsrRoamSession *pSession = NULL;

    if (CSR_IS_SESSION_VALID( pMac, sessionId ) )
    {
        pSession = CSR_GET_SESSION( pMac, sessionId );
        if (NULL != pSession->pCurRoamProfile)
        {
            if (pSession->pCurRoamProfile->csrPersona != VOS_STA_MODE)
            {
                return eANI_BOOLEAN_FALSE;
            }
        }
    }

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
    if (eANI_BOOLEAN_TRUE == CSR_IS_FASTROAM_IN_CONCURRENCY_INI_FEATURE_ENABLED(pMac))
    {
        return (pMac->roam.configParam.isFastRoamIniFeatureEnabled);
    }
    else
#endif
    {
        return (pMac->roam.configParam.isFastRoamIniFeatureEnabled &&
            (!csrIsConcurrentSessionRunning(pMac)));
    }
}

#ifdef FEATURE_WLAN_CCX
/*                                                                            

                                 

                                                                                       

                                                  

                                                                                   

                                                                           */
tANI_BOOLEAN csrNeighborRoamIsCCXAssoc(tpAniSirGlobal pMac)
{
    return pMac->roam.neighborRoamInfo.isCCXAssoc;
}
#endif /*                  */

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
//                                                                
tANI_BOOLEAN csrRoamIsRoamOffloadScanEnabled(tpAniSirGlobal pMac)
{
    return (pMac->roam.configParam.isRoamOffloadScanEnabled);
}
#endif
#endif

#if defined(FEATURE_WLAN_CCX)
tANI_BOOLEAN csrRoamIsCcxIniFeatureEnabled(tpAniSirGlobal pMac)
{
    return pMac->roam.configParam.isCcxIniFeatureEnabled;
}
#endif /*                */

//                                                      
static tANI_BOOLEAN csrRoamProcessResults( tpAniSirGlobal pMac, tSmeCmd *pCommand,
                                       eCsrRoamCompleteResult Result, void *Context )
{
    tANI_BOOLEAN fReleaseCommand = eANI_BOOLEAN_TRUE;
    tSirBssDescription *pSirBssDesc = NULL;   
    tSirMacAddr BroadcastMac = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    tCsrScanResult *pScanResult = NULL;
    tCsrRoamInfo roamInfo;
    sme_QosAssocInfo assocInfo;
    sme_QosCsrEventIndType ind_qos;//                                
    tANI_U8 acm_mask = 0; //                                                
    tDot11fBeaconIEs *pIes = NULL;
    tANI_U32 sessionId = pCommand->sessionId;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    tCsrRoamProfile *pProfile = &pCommand->u.roamCmd.roamProfile;
    eRoamCmdStatus roamStatus;
    eCsrRoamResult roamResult;
    eHalStatus status;
    tANI_U32 key_timeout_interval = 0;
    tSirSmeStartBssRsp  *pSmeStartBssRsp = NULL;

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eANI_BOOLEAN_FALSE;
    }

    smsLog( pMac, LOG1, FL("Processing ROAM results..."));
    switch( Result )
    {
        case eCsrJoinSuccess:
            //                     
            //   
            //                                                                  
            //   
        case eCsrReassocSuccess:
            if(eCsrReassocSuccess == Result)
            {
                ind_qos = SME_QOS_CSR_REASSOC_COMPLETE;
            }
            else
            {
                ind_qos = SME_QOS_CSR_ASSOC_COMPLETE;
            }
            //                                                                         
            //                   
            smsLog(pMac, LOGW, FL("receives association indication"));
            palZeroMemory(pMac->hHdd, &roamInfo, sizeof(roamInfo));
            //                           
            if(pSession->pWpaRsnRspIE)
            {
                pSession->nWpaRsnRspIeLength = 0;
                palFreeMemory(pMac->hHdd, pSession->pWpaRsnRspIE);
                pSession->pWpaRsnRspIE = NULL;
            }
#ifdef FEATURE_WLAN_WAPI
            if(pSession->pWapiRspIE)
            {
                pSession->nWapiRspIeLength = 0;
                palFreeMemory(pMac->hHdd, pSession->pWapiRspIE);
                pSession->pWapiRspIE = NULL;
            }
#endif /*                   */
#ifdef FEATURE_WLAN_BTAMP_UT_RF
            //                                         
            pSession->maxRetryCount = 0;
            csrRoamStopJoinRetryTimer(pMac, sessionId);
#endif
            /*                                                                    
                                                   
            */
            //                                                      

            /*                                                            
                                                 
                                                            
             */
            pMac->pmc.remainInPowerActiveTillDHCP = FALSE;
            if( CSR_IS_INFRASTRUCTURE( pProfile ) )
            {
                pSession->connectState = eCSR_ASSOC_STATE_TYPE_INFRA_ASSOCIATED;
            }
            else
            {
                pSession->connectState = eCSR_ASSOC_STATE_TYPE_WDS_CONNECTED;
            }
            //                                                              
            //                                                  
            if( (eCsrHddIssuedReassocToSameAP == pCommand->u.roamCmd.roamReason) ||
                (eCsrSmeIssuedReassocToSameAP == pCommand->u.roamCmd.roamReason) )
            {
                pSirBssDesc = pSession->pConnectBssDesc;
                if(pSirBssDesc)
                {
                    palCopyMemory(pMac->hHdd, &roamInfo.bssid, &pSirBssDesc->bssId, sizeof(tCsrBssid));
                } 
            }
            else
            {
     
                if(pCommand->u.roamCmd.pRoamBssEntry)
                {
                    pScanResult = GET_BASE_ADDR(pCommand->u.roamCmd.pRoamBssEntry, tCsrScanResult, Link);
                    if(pScanResult != NULL)
                    {
                        pSirBssDesc = &pScanResult->Result.BssDescriptor;
                        //                
                        pIes = (tDot11fBeaconIEs *)( pScanResult->Result.pvIes );
                        palCopyMemory(pMac->hHdd, &roamInfo.bssid, &pSirBssDesc->bssId, sizeof(tCsrBssid));
                    }
                }
            }
            if( pSirBssDesc )
            {
                roamInfo.staId = HAL_STA_INVALID_IDX;
                csrRoamSaveConnectedInfomation(pMac, sessionId, pProfile, pSirBssDesc, pIes);
                    //               
                csrRoamSaveSecurityRspIE(pMac, sessionId, pProfile->negotiatedAuthType, pSirBssDesc, pIes);
#ifdef FEATURE_WLAN_CCX
                roamInfo.isCCXAssoc = pSession->connectedProfile.isCCXAssoc;
#endif
                
                //                                                                                            
                //                 
                //                                                                             
                //                                                                        
                csrRoamStateChange( pMac, eCSR_ROAMING_STATE_JOINED, sessionId );
                //                                                                                               
                //                                                                                           
                //                                                                   
                //
                //                                                                                                   
                //                                                                             
                //
                //                                                                                
                //
            
                if( CSR_IS_ENC_TYPE_STATIC( pProfile->negotiatedUCEncryptionType ) &&
                                        !pProfile->bWPSAssociation)
                {
                    //                                                                          
                    if( !HAL_STATUS_SUCCESS( csrRoamIssueSetContextReq( pMac, sessionId,
                                                pProfile->negotiatedUCEncryptionType, 
                                                pSirBssDesc, &(pSirBssDesc->bssId),
                                                FALSE, TRUE, eSIR_TX_RX, 0, 0, NULL, 0 ) ) ) //                                              
                    {
                        smsLog( pMac, LOGE, FL("  Set context for unicast fail") );
                        csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_NONE, sessionId );
                    }
                    //                                                                            
                    csrRoamIssueSetContextReq( pMac, sessionId, pProfile->negotiatedMCEncryptionType,
                                               pSirBssDesc, &BroadcastMac,
                                               FALSE, FALSE, eSIR_TX_RX, 0, 0, NULL, 0 ); //                                              
                }
                else
                {
                    //                                        
                    roamInfo.fAuthRequired = eANI_BOOLEAN_TRUE;
                    //                                                               
                    csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_WAIT_FOR_KEY, sessionId );

                    /*                                                         
                                                                 
                     */
                     pMac->pmc.remainInPowerActiveTillDHCP = TRUE;

                    if(pProfile->bWPSAssociation)
                    {
                        key_timeout_interval = CSR_WAIT_FOR_WPS_KEY_TIMEOUT_PERIOD;
                    }
                    else
                    {
                        key_timeout_interval = CSR_WAIT_FOR_KEY_TIMEOUT_PERIOD;
                    }
                    
                    //                                 
                    pMac->roam.WaitForKeyTimerInfo.sessionId = (tANI_U8)sessionId;
                    //                                                                            
                    if(!HAL_STATUS_SUCCESS( csrRoamStartWaitForKeyTimer( pMac, key_timeout_interval ) ) )
                    {
                        //                                       
                        smsLog( pMac, LOGE, FL("   Failed to start pre-auth timer") );
                        csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_NONE, sessionId);
                    }
                }
                
                assocInfo.pBssDesc = pSirBssDesc; //             
                assocInfo.pProfile = pProfile;
                if(Context)
                {
                    tSirSmeJoinRsp *pJoinRsp = (tSirSmeJoinRsp *)Context;
                    tANI_U32 len;
                    csrRoamFreeConnectedInfo( pMac, &pSession->connectedInfo );
                    len = pJoinRsp->assocReqLength + pJoinRsp->assocRspLength + pJoinRsp->beaconLength;
#ifdef WLAN_FEATURE_VOWIFI_11R
                    len += pJoinRsp->parsedRicRspLen;
#endif /*                         */                    
#ifdef FEATURE_WLAN_CCX
                    len += pJoinRsp->tspecIeLen;
#endif
                    if(len)
                    {
                        if(HAL_STATUS_SUCCESS(palAllocateMemory(pMac->hHdd, 
                                        (void **)&pSession->connectedInfo.pbFrames, len)))
                        {
                            if(HAL_STATUS_SUCCESS( palCopyMemory(pMac->hHdd, 
                                            pSession->connectedInfo.pbFrames, pJoinRsp->frames, len) ))
                            {
                                pSession->connectedInfo.nAssocReqLength = pJoinRsp->assocReqLength;
                                pSession->connectedInfo.nAssocRspLength = pJoinRsp->assocRspLength;
                                pSession->connectedInfo.nBeaconLength = pJoinRsp->beaconLength;
#ifdef WLAN_FEATURE_VOWIFI_11R
                                pSession->connectedInfo.nRICRspLength = pJoinRsp->parsedRicRspLen;
#endif /*                         */                                
#ifdef FEATURE_WLAN_CCX
                                pSession->connectedInfo.nTspecIeLength = pJoinRsp->tspecIeLen;
#endif
                                roamInfo.nAssocReqLength = pJoinRsp->assocReqLength;
                                roamInfo.nAssocRspLength = pJoinRsp->assocRspLength;
                                roamInfo.nBeaconLength = pJoinRsp->beaconLength;
                                roamInfo.pbFrames = pSession->connectedInfo.pbFrames;
                            }
                            else
                            {
                                smsLog(pMac, LOGE, "%s: Copying of memory failed for %d bytes !!!",
                                        __func__, len);
                                palFreeMemory( pMac->hHdd, pSession->connectedInfo.pbFrames );
                                pSession->connectedInfo.pbFrames = NULL;
                            }
                        }
                    }
                    if(pCommand->u.roamCmd.fReassoc)
                    {
                        roamInfo.fReassocReq = roamInfo.fReassocRsp = eANI_BOOLEAN_TRUE;
                    }
                    pSession->connectedInfo.staId = ( tANI_U8 )pJoinRsp->staId;
                    roamInfo.staId = ( tANI_U8 )pJoinRsp->staId;
                    roamInfo.ucastSig = ( tANI_U8 )pJoinRsp->ucastSig;
                    roamInfo.bcastSig = ( tANI_U8 )pJoinRsp->bcastSig;
                }
                else
                {
                   if(pCommand->u.roamCmd.fReassoc)
                   {
                       roamInfo.fReassocReq = roamInfo.fReassocRsp = eANI_BOOLEAN_TRUE;
                       roamInfo.nAssocReqLength = pSession->connectedInfo.nAssocReqLength;
                       roamInfo.nAssocRspLength = pSession->connectedInfo.nAssocRspLength;
                       roamInfo.nBeaconLength = pSession->connectedInfo.nBeaconLength;
                       roamInfo.pbFrames = pSession->connectedInfo.pbFrames;
                   }
                }
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
                //                                                         
                //                    
                sme_QosCsrEventInd(pMac, (v_U8_t)sessionId, ind_qos, &assocInfo);
#endif
                roamInfo.pBssDesc = pSirBssDesc;
                roamInfo.statusCode = pSession->joinFailStatusCode.statusCode;
                roamInfo.reasonCode = pSession->joinFailStatusCode.reasonCode;
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
                acm_mask = sme_QosGetACMMask(pMac, pSirBssDesc, NULL);
#endif /*                            */
                pSession->connectedProfile.acm_mask = acm_mask;
                //                                                                               
                //                                       
                if( pSession->connectedProfile.modifyProfileFields.uapsd_mask )
                {
                    smsLog(pMac, LOGE, " uapsd_mask (0x%X) set, request UAPSD now",
                        pSession->connectedProfile.modifyProfileFields.uapsd_mask);
                    pmcStartUapsd( pMac, NULL, NULL );
                }
                pSession->connectedProfile.dot11Mode = pSession->bssParams.uCfgDot11Mode;
                roamInfo.u.pConnectedProfile = &pSession->connectedProfile;
                if( pSession->bRefAssocStartCnt > 0 )
                {
                    pSession->bRefAssocStartCnt--;
                    //                                                      
                    //                          
                    if(!IS_FEATURE_SUPPORTED_BY_FW(SLM_SESSIONIZATION) && ( csrIsConcurrentSessionRunning( pMac )))
                    {
                       pMac->roam.configParam.doBMPSWorkaround = 1;
                    }
                    csrRoamCallCallback(pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, eCSR_ROAM_ASSOCIATION_COMPLETION, eCSR_ROAM_RESULT_ASSOCIATED);
                }
                
                csrRoamCompletion(pMac, sessionId, NULL, pCommand, eCSR_ROAM_RESULT_NONE, eANI_BOOLEAN_TRUE);
                //                               
                csrResetPMKIDCandidateList( pMac, sessionId );
                //                                                         
                //                                                                            
                //                                                                              
                //                                              
                //                              
                if( csrIsConnStateConnectedInfra(pMac, sessionId ) && pIes )
                {
                    csrCheckAndUpdateACWeight(pMac, pIes);
                }
#ifdef FEATURE_WLAN_WAPI
                //                              
                csrResetBKIDCandidateList( pMac, sessionId );
#endif /*                   */
            }
            else
            {
                smsLog(pMac, LOGW, "  Roam command doesn't have a BSS desc");
            }
            csrScanCancelIdleScan(pMac);
            //                                                     
            //                                                                                    
            if( !CSR_IS_WAIT_FOR_KEY(pMac, sessionId) )
            {
                csrRoamLinkUp(pMac, pSession->connectedProfile.bssid);
            }
            //                                                                                
            //                                                                        
            if (pmcShouldBmpsTimerRun(pMac))
            {
                if (pmcStartTrafficTimer(pMac, BMPS_TRAFFIC_TIMER_ALLOW_SECURITY_DHCP)
                    != eHAL_STATUS_SUCCESS)
                {
                    smsLog(pMac, LOGP, FL("Cannot start BMPS Retry timer"));
                }
                smsLog(pMac, LOG2, FL("BMPS Retry Timer already running or started"));
            }
            break;

        case eCsrStartBssSuccess:
            //                                                                       
            //                                                                 
            //                                                                         
            //                                                                    
            //                                                                           
            //                                                                        
            smsLog(pMac, LOG2, FL("receives start BSS ok indication"));
            status = eHAL_STATUS_FAILURE;
            pSmeStartBssRsp = (tSirSmeStartBssRsp *)Context;
            palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
            if( CSR_IS_IBSS( pProfile ) )
            {
                pSession->connectState = eCSR_ASSOC_STATE_TYPE_IBSS_DISCONNECTED;
            }
            else if (CSR_IS_INFRA_AP(pProfile))
            {
                pSession->connectState = eCSR_ASSOC_STATE_TYPE_INFRA_DISCONNECTED;
            }
            else
            {
                pSession->connectState = eCSR_ASSOC_STATE_TYPE_WDS_DISCONNECTED;
            }
            if( !CSR_IS_WDS_STA( pProfile ) )
            {
                csrRoamStateChange( pMac, eCSR_ROAMING_STATE_JOINED, sessionId );
                pSirBssDesc = &pSmeStartBssRsp->bssDescription;
                if( !HAL_STATUS_SUCCESS(csrGetParsedBssDescriptionIEs( pMac, pSirBssDesc, &pIes )) )
                {
                    smsLog(pMac, LOG2, FL("cannot parse IBSS IEs"));
                    roamInfo.pBssDesc = pSirBssDesc;
                    //                                                                                  
                    csrRoamCallCallback( pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, 
                            eCSR_ROAM_IBSS_IND, eCSR_ROAM_RESULT_IBSS_START_FAILED );
                    break;
                }
                if (!CSR_IS_INFRA_AP(pProfile))
                {
                    pScanResult = csrScanAppendBssDescription( pMac, pSirBssDesc, pIes, FALSE );
                }
                csrRoamSaveConnectedBssDesc(pMac, sessionId, pSirBssDesc);
                csrRoamFreeConnectProfile(pMac, &pSession->connectedProfile);
                csrRoamFreeConnectedInfo( pMac, &pSession->connectedInfo );
                if(pSirBssDesc)
                {
                    csrRoamSaveConnectedInfomation(pMac, sessionId, pProfile, pSirBssDesc, pIes);
                    palCopyMemory(pMac->hHdd, &roamInfo.bssid, &pSirBssDesc->bssId, sizeof(tCsrBssid));
                }
                //                                   
                palFreeMemory(pMac->hHdd, pIes);
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
                {
                    vos_log_ibss_pkt_type *pIbssLog;
                    tANI_U32 bi;
    
                    WLAN_VOS_DIAG_LOG_ALLOC(pIbssLog, vos_log_ibss_pkt_type, LOG_WLAN_IBSS_C);
                    if(pIbssLog)
                    { 
                        if(CSR_INVALID_SCANRESULT_HANDLE == pCommand->u.roamCmd.hBSSList)
                        {
                            //                                                          
                            pIbssLog->eventId = WLAN_IBSS_EVENT_START_IBSS_RSP;
                        }
                        else
                        {
                            pIbssLog->eventId = WLAN_IBSS_EVENT_JOIN_IBSS_RSP;
                        }
                        if(pSirBssDesc)
                        {
                            palCopyMemory(pMac->hHdd, pIbssLog->bssid, pSirBssDesc->bssId, 6);
                            pIbssLog->operatingChannel = pSirBssDesc->channelId;
                        }
                        if(HAL_STATUS_SUCCESS(ccmCfgGetInt(pMac, WNI_CFG_BEACON_INTERVAL, &bi)))
                        {
                            //                                       
                            pIbssLog->beaconInterval = (v_U8_t)bi;
                        }
                        WLAN_VOS_DIAG_LOG_REPORT(pIbssLog);
                    }
                }
#endif //                                    
                //                                                                                            
                //                               
                if( CSR_IS_ENC_TYPE_STATIC( pProfile->negotiatedUCEncryptionType ) && !CSR_IS_INFRA_AP( pSession->pCurRoamProfile ))
                {
                    //                                                                                          
                    csrRoamIssueSetContextReq( pMac, sessionId, 
                                        pProfile->negotiatedMCEncryptionType, 
                                        pSirBssDesc, &BroadcastMac,
                                        FALSE, FALSE, eSIR_TX_RX, 0, 0, NULL, 0 ); //                                              
                }
            }
            else
            {
                //                                            
                //                      
                if(pCommand->u.roamCmd.pRoamBssEntry)
                {
                    if((pScanResult = GET_BASE_ADDR(pCommand->u.roamCmd.pRoamBssEntry, tCsrScanResult, Link)))
                    {
                        pSirBssDesc = &pScanResult->Result.BssDescriptor;
                        pIes = (tDot11fBeaconIEs *)( pScanResult->Result.pvIes );
                        //                                              
                        csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_JOIN_REQ, sessionId);
                        status = csrSendJoinReqMsg( pMac, sessionId, pSirBssDesc, pProfile, pIes, eWNI_SME_JOIN_REQ );
                    }
                }
                else
                {
                    smsLog( pMac, LOGE, " StartBSS for WDS station with no BssDesc" );
                    VOS_ASSERT( 0 );
                }
            }
            //                                                                                        
            //                                                                                        
            //                                                
            if( !CSR_IS_JOIN_TO_IBSS( pProfile ) )
            {
                roamStatus = eCSR_ROAM_IBSS_IND;
                roamResult = eCSR_ROAM_RESULT_IBSS_STARTED;
                if( CSR_IS_WDS( pProfile ) )
                {
                    roamStatus = eCSR_ROAM_WDS_IND;
                    roamResult = eCSR_ROAM_RESULT_WDS_STARTED;
                }
                if( CSR_IS_INFRA_AP( pProfile ) )
                {
                    roamStatus = eCSR_ROAM_INFRA_IND;
                    roamResult = eCSR_ROAM_RESULT_INFRA_STARTED;
                }
                 
                //                                                                                        
                //                                                                                        
                //                                                
                palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
                roamInfo.statusCode = pSession->joinFailStatusCode.statusCode;
                roamInfo.reasonCode = pSession->joinFailStatusCode.reasonCode;
                if(CSR_INVALID_SCANRESULT_HANDLE == pCommand->u.roamCmd.hBSSList)
                {
                    //                                                          
                    roamInfo.pBssDesc = pSirBssDesc;
                }
                roamInfo.staId = (tANI_U8)pSmeStartBssRsp->staId;
                vos_mem_copy (roamInfo.bssid, pSirBssDesc->bssId, sizeof(tCsrBssid));
                 //                                                      
                 //                          
                if(!IS_FEATURE_SUPPORTED_BY_FW(SLM_SESSIONIZATION) &&
                   ( csrIsConcurrentSessionRunning( pMac )))
                {
                   pMac->roam.configParam.doBMPSWorkaround = 1;
                }

                csrRoamCallCallback( pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, roamStatus, roamResult );
            }
    
            csrScanCancelIdleScan(pMac);
            //                                                           
            if( CSR_IS_IBSS( pProfile) && CSR_INVALID_SCANRESULT_HANDLE != pCommand->u.roamCmd.hBSSList)
            {
              //                         
                csrRoamStartIbssJoinTimer(pMac, sessionId, CSR_IBSS_JOIN_TIMEOUT_PERIOD); //        
                pSession->ibss_join_pending = TRUE;
            }
            if( HAL_STATUS_SUCCESS( status ) )
            {
                //                                         
                fReleaseCommand = eANI_BOOLEAN_FALSE;
            }
            else if( CSR_IS_WDS_STA( pProfile ) )
            {
                //                                                      
                csrRoamIssueDisassociateCmd( pMac, sessionId, eCSR_DISCONNECT_REASON_UNSPECIFIED );
                csrRoamCallCallback( pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, 
                                        eCSR_ROAM_WDS_IND, eCSR_ROAM_RESULT_WDS_STOPPED );
            }
            break;
        case eCsrStartBssFailure:
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
            {
                vos_log_ibss_pkt_type *pIbssLog;
                WLAN_VOS_DIAG_LOG_ALLOC(pIbssLog, vos_log_ibss_pkt_type, LOG_WLAN_IBSS_C);
                if(pIbssLog)
                {
                    pIbssLog->status = WLAN_IBSS_STATUS_FAILURE;
                    WLAN_VOS_DIAG_LOG_REPORT(pIbssLog);
                }
            }
#endif //                                    
            roamStatus = eCSR_ROAM_IBSS_IND;
            roamResult = eCSR_ROAM_RESULT_IBSS_STARTED;
            if( CSR_IS_WDS( pProfile ) )
            {
                roamStatus = eCSR_ROAM_WDS_IND;
                roamResult = eCSR_ROAM_RESULT_WDS_STARTED;
            }
            if( CSR_IS_INFRA_AP( pProfile ) )
            {
                roamStatus = eCSR_ROAM_INFRA_IND;
                roamResult = eCSR_ROAM_RESULT_INFRA_START_FAILED;
            }
            if(Context)
            {
                pSirBssDesc = (tSirBssDescription *)Context;
            }
            else
            {
                pSirBssDesc = NULL;
            }
            palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
            roamInfo.pBssDesc = pSirBssDesc;
            //                                                                                  
            csrRoamCallCallback( pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, roamStatus, roamResult );
            csrSetDefaultDot11Mode( pMac );
            break;
        case eCsrSilentlyStopRoaming:
            //                                                  
            //                
            //                                    
            smsLog(pMac, LOGW, FL("receives silently roaming indication"));
            csrRoamStateChange( pMac, eCSR_ROAMING_STATE_JOINED, sessionId );
            csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_NONE, sessionId );
            palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
            roamInfo.pBssDesc = pSession->pConnectBssDesc;
            if( roamInfo.pBssDesc )
            {
                palCopyMemory(pMac->hHdd, &roamInfo.bssid, &roamInfo.pBssDesc->bssId, sizeof(tCsrBssid));
            }
            //                                                                                   
            //                                                 
            csrRoamCallCallback( pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, 
                                        eCSR_ROAM_IBSS_IND, eCSR_ROAM_RESULT_NONE );
            break;
        case eCsrSilentlyStopRoamingSaveState:
            //                                                    
            //                
            smsLog(pMac, LOGW, FL("receives silently stop roaming indication"));
            palZeroMemory(pMac->hHdd, &roamInfo, sizeof(roamInfo));
            
            //                                       
            pMac->roam.curState[sessionId] = eCSR_ROAMING_STATE_JOINED;
            //                                                                          
            roamInfo.pBssDesc = pSession->pConnectBssDesc;
            if( roamInfo.pBssDesc )
            {
                palCopyMemory(pMac->hHdd, &roamInfo.bssid, &roamInfo.pBssDesc->bssId, sizeof(tCsrBssid));
            }
            roamInfo.statusCode = pSession->joinFailStatusCode.statusCode;
            roamInfo.reasonCode = pSession->joinFailStatusCode.reasonCode;
            roamInfo.nBeaconLength = pSession->connectedInfo.nBeaconLength;
            roamInfo.nAssocReqLength = pSession->connectedInfo.nAssocReqLength;
            roamInfo.nAssocRspLength = pSession->connectedInfo.nAssocRspLength;
            roamInfo.pbFrames = pSession->connectedInfo.pbFrames;
            roamInfo.staId = pSession->connectedInfo.staId;
            roamInfo.u.pConnectedProfile = &pSession->connectedProfile;
            VOS_ASSERT( roamInfo.staId != 0 );
            pSession->bRefAssocStartCnt--;
            csrRoamCallCallback(pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, 
                                        eCSR_ROAM_ASSOCIATION_COMPLETION, eCSR_ROAM_RESULT_ASSOCIATED);
            csrRoamCompletion(pMac, sessionId, NULL, pCommand, eCSR_ROAM_RESULT_ASSOCIATED, eANI_BOOLEAN_TRUE);
            break;
        case eCsrReassocFailure:
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
            sme_QosCsrEventInd(pMac, (tANI_U8)sessionId, SME_QOS_CSR_REASSOC_FAILURE, NULL);
#endif
        case eCsrJoinWdsFailure:
            smsLog(pMac, LOGW, FL("failed to join WDS"));
            csrFreeConnectBssDesc(pMac, sessionId);
            csrRoamFreeConnectProfile(pMac, &pSession->connectedProfile);
            csrRoamFreeConnectedInfo( pMac, &pSession->connectedInfo );
            palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
            roamInfo.pBssDesc = pCommand->u.roamCmd.pLastRoamBss;
            roamInfo.statusCode = pSession->joinFailStatusCode.statusCode;
            roamInfo.reasonCode = pSession->joinFailStatusCode.reasonCode;
            csrRoamCallCallback(pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, 
                                    eCSR_ROAM_WDS_IND, 
                                    eCSR_ROAM_RESULT_WDS_NOT_ASSOCIATED);
            //                      
            break;
        case eCsrJoinFailure:
        case eCsrNothingToJoin:
        case eCsrJoinFailureDueToConcurrency:
        default:
        {
            smsLog(pMac, LOGW, FL("receives no association indication"));
            smsLog(pMac, LOG1, FL("Assoc ref count %d"),
                   pSession->bRefAssocStartCnt);
            if( CSR_IS_INFRASTRUCTURE( &pSession->connectedProfile ) || 
                CSR_IS_ROAM_SUBSTATE_STOP_BSS_REQ( pMac, sessionId ) )
            {
                //                                                                         
                csrFreeConnectBssDesc(pMac, sessionId);
                csrRoamFreeConnectProfile(pMac, &pSession->connectedProfile);
                csrRoamFreeConnectedInfo( pMac, &pSession->connectedInfo );
                csrSetDefaultDot11Mode( pMac );
            }

            switch( pCommand->u.roamCmd.roamReason )
            {
                //                                                                   
                //                                                                    
                //                               
                case eCsrSmeIssuedFTReassoc:
                case eCsrSmeIssuedAssocToSimilarAP:
                case eCsrHddIssued:
                case eCsrSmeIssuedDisassocForHandoff:
                    csrRoamStateChange( pMac, eCSR_ROAMING_STATE_IDLE, sessionId );
                    palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
                    roamInfo.pBssDesc = pCommand->u.roamCmd.pLastRoamBss;
                    roamInfo.statusCode = pSession->joinFailStatusCode.statusCode;
                    roamInfo.reasonCode = pSession->joinFailStatusCode.reasonCode;
                    palCopyMemory(pMac->hHdd, &roamInfo.bssid, &pSession->joinFailStatusCode.bssId, sizeof(tCsrBssid));

                    /*                                  */
#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
                    /*                                                                                                     */
                    if (csrRoamIsHandoffInProgress(pMac))
                    {
                        /*                                                                        */
                        csrNeighborRoamIndicateConnect(pMac, (tANI_U8)sessionId, VOS_STATUS_E_FAILURE);
                    }
#endif
                        if(pSession->bRefAssocStartCnt > 0)
                        {
                            pSession->bRefAssocStartCnt--;
                            if(eCsrJoinFailureDueToConcurrency == Result)
                            {
                                csrRoamCallCallback(pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, 
                                                eCSR_ROAM_ASSOCIATION_COMPLETION, 
                                                eCSR_ROAM_RESULT_ASSOC_FAIL_CON_CHANNEL);
                            }
                            else
                            {
                                csrRoamCallCallback(pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, 
                                                eCSR_ROAM_ASSOCIATION_COMPLETION, 
                                                eCSR_ROAM_RESULT_FAILURE);
                            }
                        }
                        else
                        {
                            /*                                          
                                                                            
                                                                           
                                                                 
                             */
                            csrRoamCallCallback(pMac, sessionId, &roamInfo,
                                    pCommand->u.scanCmd.roamId,
                                    eCSR_ROAM_ASSOCIATION_FAILURE,
                                    eCSR_ROAM_RESULT_FAILURE);
                        }
                    smsLog(pMac, LOG1, FL("  roam(reason %d) failed"), pCommand->u.roamCmd.roamReason);
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
                    sme_QosUpdateHandOff((tANI_U8)sessionId, VOS_FALSE);
                    sme_QosCsrEventInd(pMac, (tANI_U8)sessionId, SME_QOS_CSR_DISCONNECT_IND, NULL);
#endif
                    csrRoamCompletion(pMac, sessionId, NULL, pCommand, eCSR_ROAM_RESULT_FAILURE, eANI_BOOLEAN_FALSE);
                    csrScanStartIdleScan(pMac);
#ifdef FEATURE_WLAN_BTAMP_UT_RF
                    //                                                                      
                    //                                                                
                    if( CSR_IS_WDS_STA(pProfile) )
                    {
                        csrRoamStartJoinRetryTimer(pMac, sessionId, CSR_JOIN_RETRY_TIMEOUT_PERIOD);
                    }
#endif
                    break;
                case eCsrHddIssuedReassocToSameAP:
                case eCsrSmeIssuedReassocToSameAP:
                    csrRoamStateChange( pMac, eCSR_ROAMING_STATE_IDLE, sessionId);

                    csrRoamCallCallback(pMac, sessionId, NULL, pCommand->u.roamCmd.roamId, eCSR_ROAM_DISASSOCIATED, eCSR_ROAM_RESULT_FORCED);
#ifndef WLAN_MDM_CODE_REDUCTION_OPT                                        
                    sme_QosCsrEventInd(pMac, (tANI_U8)sessionId, SME_QOS_CSR_DISCONNECT_IND, NULL);
#endif
                    csrRoamCompletion(pMac, sessionId, NULL, pCommand, eCSR_ROAM_RESULT_FAILURE, eANI_BOOLEAN_FALSE);
                    csrScanStartIdleScan(pMac);
                    break;
                case eCsrForcedDisassoc:
                case eCsrForcedDeauth:
                case eCsrSmeIssuedIbssJoinFailure:
                    csrRoamStateChange( pMac, eCSR_ROAMING_STATE_IDLE, sessionId);

                    if(eCsrSmeIssuedIbssJoinFailure == pCommand->u.roamCmd.roamReason)
                    {
                        //                                 
                        csrRoamCallCallback(pMac, sessionId, NULL, 0, eCSR_ROAM_IBSS_IND, eCSR_ROAM_RESULT_IBSS_JOIN_FAILED);
                    }
                    else
                    {
                        csrRoamCallCallback(pMac, sessionId, NULL, 
                                            pCommand->u.roamCmd.roamId, 
                                            eCSR_ROAM_DISASSOCIATED, eCSR_ROAM_RESULT_FORCED);
                    }
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
                    sme_QosCsrEventInd(pMac, (tANI_U8)sessionId, SME_QOS_CSR_DISCONNECT_IND, NULL);
#endif
                    csrRoamLinkDown(pMac, sessionId);
                    /*
                                                                        
                                    
                     */
                    if (pMac->roam.deauthRspStatus == eSIR_SME_DEAUTH_STATUS)
                    {
                        smsLog(pMac, LOGW, FL("FW still in connected state "));
                        break;
                    }
                    csrScanStartIdleScan(pMac);
                    break;
                case eCsrForcedIbssLeave:
                     csrRoamCallCallback(pMac, sessionId, NULL, 
                                        pCommand->u.roamCmd.roamId, 
                                        eCSR_ROAM_IBSS_LEAVE,
                                        eCSR_ROAM_RESULT_IBSS_STOP);
                    break;
                case eCsrForcedDisassocMICFailure:
                    csrRoamStateChange( pMac, eCSR_ROAMING_STATE_IDLE, sessionId );

                    csrRoamCallCallback(pMac, sessionId, NULL, pCommand->u.roamCmd.roamId, eCSR_ROAM_DISASSOCIATED, eCSR_ROAM_RESULT_MIC_FAILURE);
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
                    sme_QosCsrEventInd(pMac, (tANI_U8)sessionId, SME_QOS_CSR_DISCONNECT_REQ, NULL);
#endif
                    csrScanStartIdleScan(pMac);
                    break;
                case eCsrStopBss:
                    csrRoamCallCallback(pMac, sessionId, NULL, 
                                        pCommand->u.roamCmd.roamId, 
                                        eCSR_ROAM_INFRA_IND, 
                                        eCSR_ROAM_RESULT_INFRA_STOPPED);
                    break;
                case eCsrForcedDisassocSta:
                case eCsrForcedDeauthSta:
                   csrRoamStateChange( pMac, eCSR_ROAMING_STATE_JOINED, sessionId);
                   if( CSR_IS_SESSION_VALID(pMac, sessionId) )
                   {                    
                       pSession = CSR_GET_SESSION(pMac, sessionId);
                       if (!pSession)
                           break;

                       if ( CSR_IS_INFRA_AP(&pSession->connectedProfile) )
                       {
                           roamInfo.u.pConnectedProfile = &pSession->connectedProfile;
                           palCopyMemory(pMac->hHdd, roamInfo.peerMac, 
                              pCommand->u.roamCmd.peerMac, sizeof(tSirMacAddr));
                           roamInfo.reasonCode = eCSR_ROAM_RESULT_FORCED;
                           roamInfo.statusCode = eSIR_SME_SUCCESS;
                           status = csrRoamCallCallback(pMac, sessionId, 
                                       &roamInfo, pCommand->u.roamCmd.roamId, 
                                       eCSR_ROAM_LOSTLINK, eCSR_ROAM_RESULT_FORCED);
                       }
                   }
                   break;
                case eCsrLostLink1:
                    //                                                          
                    csrScanRequestLostLink2(pMac, sessionId);
                    break;
                case eCsrLostLink2:
                    //                                                          
                    csrScanRequestLostLink3(pMac, sessionId);
                    break;
                case eCsrLostLink3:
                default:
                    csrRoamStateChange( pMac, eCSR_ROAMING_STATE_IDLE, sessionId );

                    //                                                   
                    csrScanHandleFailedLostlink3(pMac, sessionId);
                    break;
            }
            break;
        }
    }
    return ( fReleaseCommand );
}

eHalStatus csrRoamRegisterCallback(tpAniSirGlobal pMac, csrRoamCompleteCallback callback, void *pContext)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    return (status);
}

eHalStatus csrRoamCopyProfile(tpAniSirGlobal pMac, tCsrRoamProfile *pDstProfile, tCsrRoamProfile *pSrcProfile)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_U32 size = 0;
    
    do
    {
        palZeroMemory(pMac->hHdd, pDstProfile, sizeof(tCsrRoamProfile));
        if(pSrcProfile->BSSIDs.numOfBSSIDs)
        {
            size = sizeof(tCsrBssid) * pSrcProfile->BSSIDs.numOfBSSIDs;
            status = palAllocateMemory(pMac->hHdd, (void **)&pDstProfile->BSSIDs.bssid, size);
            if(!HAL_STATUS_SUCCESS(status))
            {
                break;
            }
            pDstProfile->BSSIDs.numOfBSSIDs = pSrcProfile->BSSIDs.numOfBSSIDs;
            palCopyMemory(pMac->hHdd, pDstProfile->BSSIDs.bssid, pSrcProfile->BSSIDs.bssid, size);
        }
        if(pSrcProfile->SSIDs.numOfSSIDs)
        {
            size = sizeof(tCsrSSIDInfo) * pSrcProfile->SSIDs.numOfSSIDs;
            status = palAllocateMemory(pMac->hHdd, (void **)&pDstProfile->SSIDs.SSIDList, size);
            if(!HAL_STATUS_SUCCESS(status))
            {
                break;
            }
            pDstProfile->SSIDs.numOfSSIDs = pSrcProfile->SSIDs.numOfSSIDs;
            palCopyMemory(pMac->hHdd, pDstProfile->SSIDs.SSIDList, pSrcProfile->SSIDs.SSIDList, size);
        }
        if(pSrcProfile->nWPAReqIELength)
        {
            status = palAllocateMemory(pMac->hHdd, (void **)&pDstProfile->pWPAReqIE, pSrcProfile->nWPAReqIELength);
            if(!HAL_STATUS_SUCCESS(status))
            {
                break;
            }
            pDstProfile->nWPAReqIELength = pSrcProfile->nWPAReqIELength;
            palCopyMemory(pMac->hHdd, pDstProfile->pWPAReqIE, pSrcProfile->pWPAReqIE, pSrcProfile->nWPAReqIELength);
        }
        if(pSrcProfile->nRSNReqIELength)
        {
            status = palAllocateMemory(pMac->hHdd, (void **)&pDstProfile->pRSNReqIE, pSrcProfile->nRSNReqIELength);
            if(!HAL_STATUS_SUCCESS(status))
            {
                break;
            }
            pDstProfile->nRSNReqIELength = pSrcProfile->nRSNReqIELength;
            palCopyMemory(pMac->hHdd, pDstProfile->pRSNReqIE, pSrcProfile->pRSNReqIE, pSrcProfile->nRSNReqIELength);
        }
#ifdef FEATURE_WLAN_WAPI
        if(pSrcProfile->nWAPIReqIELength)
        {
            status = palAllocateMemory(pMac->hHdd, (void **)&pDstProfile->pWAPIReqIE, pSrcProfile->nWAPIReqIELength);
            if(!HAL_STATUS_SUCCESS(status))
            {
                break;
            }
            pDstProfile->nWAPIReqIELength = pSrcProfile->nWAPIReqIELength;
            palCopyMemory(pMac->hHdd, pDstProfile->pWAPIReqIE, pSrcProfile->pWAPIReqIE, pSrcProfile->nWAPIReqIELength);
        }
#endif /*                   */
        if(pSrcProfile->nAddIEScanLength)
        {
            status = palAllocateMemory(pMac->hHdd,
                     (void **)&pDstProfile->pAddIEScan, pSrcProfile->nAddIEScanLength);
            if(!HAL_STATUS_SUCCESS(status))
            {
                break;
            }
            pDstProfile->nAddIEScanLength = pSrcProfile->nAddIEScanLength;
            palCopyMemory(pMac->hHdd, pDstProfile->pAddIEScan, pSrcProfile->pAddIEScan,
                pSrcProfile->nAddIEScanLength);
        }
        if(pSrcProfile->nAddIEAssocLength)
        {
            status = palAllocateMemory(pMac->hHdd,
                     (void **)&pDstProfile->pAddIEAssoc, pSrcProfile->nAddIEAssocLength);
            if(!HAL_STATUS_SUCCESS(status))
            {
                break;
            }
            pDstProfile->nAddIEAssocLength = pSrcProfile->nAddIEAssocLength;
            palCopyMemory(pMac->hHdd, pDstProfile->pAddIEAssoc, pSrcProfile->pAddIEAssoc,
                pSrcProfile->nAddIEAssocLength);
        }
        if(pSrcProfile->ChannelInfo.ChannelList)
        {
            status = palAllocateMemory(pMac->hHdd, (void **)&pDstProfile->ChannelInfo.ChannelList, pSrcProfile->ChannelInfo.numOfChannels);
            if(!HAL_STATUS_SUCCESS(status))
            {
                break;
            }
            pDstProfile->ChannelInfo.numOfChannels = pSrcProfile->ChannelInfo.numOfChannels;
            palCopyMemory(pMac->hHdd, pDstProfile->ChannelInfo.ChannelList, pSrcProfile->ChannelInfo.ChannelList, pSrcProfile->ChannelInfo.numOfChannels);
        }
        pDstProfile->AuthType = pSrcProfile->AuthType;
        pDstProfile->EncryptionType = pSrcProfile->EncryptionType;
        pDstProfile->mcEncryptionType = pSrcProfile->mcEncryptionType;
        pDstProfile->negotiatedUCEncryptionType = pSrcProfile->negotiatedUCEncryptionType;
        pDstProfile->negotiatedMCEncryptionType = pSrcProfile->negotiatedMCEncryptionType;
        pDstProfile->negotiatedAuthType = pSrcProfile->negotiatedAuthType;
#ifdef WLAN_FEATURE_11W
        pDstProfile->MFPEnabled = pSrcProfile->MFPEnabled;
        pDstProfile->MFPRequired = pSrcProfile->MFPRequired;
        pDstProfile->MFPCapable = pSrcProfile->MFPCapable;
#endif
        pDstProfile->BSSType = pSrcProfile->BSSType;
        pDstProfile->phyMode = pSrcProfile->phyMode;
        pDstProfile->csrPersona = pSrcProfile->csrPersona;
        
#ifdef FEATURE_WLAN_WAPI
        if(csrIsProfileWapi(pSrcProfile))
        {
             if(pDstProfile->phyMode & eCSR_DOT11_MODE_11n)
             {
                pDstProfile->phyMode &= ~eCSR_DOT11_MODE_11n;
             }
        }
#endif /*                   */
        pDstProfile->CBMode = pSrcProfile->CBMode;
        /*                 */
        pDstProfile->bWPSAssociation = pSrcProfile->bWPSAssociation;
        pDstProfile->uapsd_mask = pSrcProfile->uapsd_mask;
        pDstProfile->beaconInterval = pSrcProfile->beaconInterval;
        pDstProfile->privacy           = pSrcProfile->privacy;
        pDstProfile->fwdWPSPBCProbeReq = pSrcProfile->fwdWPSPBCProbeReq;
        pDstProfile->csr80211AuthType  = pSrcProfile->csr80211AuthType;
        pDstProfile->dtimPeriod        = pSrcProfile->dtimPeriod;
        pDstProfile->ApUapsdEnable     = pSrcProfile->ApUapsdEnable;   
        pDstProfile->SSIDs.SSIDList[0].ssidHidden = pSrcProfile->SSIDs.SSIDList[0].ssidHidden;
        pDstProfile->protEnabled       = pSrcProfile->protEnabled;  
        pDstProfile->obssProtEnabled   = pSrcProfile->obssProtEnabled;  
        pDstProfile->cfg_protection    = pSrcProfile->cfg_protection;
        pDstProfile->wps_state         = pSrcProfile->wps_state;
        pDstProfile->ieee80211d        = pSrcProfile->ieee80211d;
        palCopyMemory(pMac->hHdd, &pDstProfile->Keys, &pSrcProfile->Keys, sizeof(pDstProfile->Keys));
#ifdef WLAN_FEATURE_VOWIFI_11R
        if (pSrcProfile->MDID.mdiePresent)
        {
            pDstProfile->MDID.mdiePresent = 1;
            pDstProfile->MDID.mobilityDomain = pSrcProfile->MDID.mobilityDomain;
        }
#endif
    }while(0);
    
    if(!HAL_STATUS_SUCCESS(status))
    {
        csrReleaseProfile(pMac, pDstProfile);
        pDstProfile = NULL;
    }
    
    return (status);
}
eHalStatus csrRoamCopyConnectedProfile(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pDstProfile )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tCsrRoamConnectedProfile *pSrcProfile = &pMac->roam.roamSession[sessionId].connectedProfile; 
    do
    {
        palZeroMemory(pMac->hHdd, pDstProfile, sizeof(tCsrRoamProfile));
        if(pSrcProfile->bssid)
        {
            status = palAllocateMemory(pMac->hHdd, (void **)&pDstProfile->BSSIDs.bssid, sizeof(tCsrBssid));
            if(!HAL_STATUS_SUCCESS(status))
            {
                smsLog( pMac, LOGE,
                    FL("failed to allocate memory for BSSID"
                    "%02x:%02x:%02x:%02x:%02x:%02x"),
                    pSrcProfile->bssid[0], pSrcProfile->bssid[1], pSrcProfile->bssid[2],
                    pSrcProfile->bssid[3], pSrcProfile->bssid[4], pSrcProfile->bssid[5]);
                break;
            }
            pDstProfile->BSSIDs.numOfBSSIDs = 1;
            palCopyMemory(pMac->hHdd, pDstProfile->BSSIDs.bssid, pSrcProfile->bssid, sizeof(tCsrBssid));
        }
        if(pSrcProfile->SSID.ssId)
        {
            status = palAllocateMemory(pMac->hHdd, (void **)&pDstProfile->SSIDs.SSIDList, sizeof(tCsrSSIDInfo));
            if(!HAL_STATUS_SUCCESS(status))
            {
                smsLog( pMac, LOGE,
                 FL("failed to allocate memory for SSIDList"
                    "%02x:%02x:%02x:%02x:%02x:%02x"),
                    pSrcProfile->bssid[0], pSrcProfile->bssid[1], pSrcProfile->bssid[2],
                    pSrcProfile->bssid[3], pSrcProfile->bssid[4], pSrcProfile->bssid[5]);
                break;
            }
            pDstProfile->SSIDs.numOfSSIDs = 1;
            pDstProfile->SSIDs.SSIDList[0].handoffPermitted = pSrcProfile->handoffPermitted;
            pDstProfile->SSIDs.SSIDList[0].ssidHidden = pSrcProfile->ssidHidden;
            palCopyMemory(pMac->hHdd, &pDstProfile->SSIDs.SSIDList[0].SSID, &pSrcProfile->SSID, sizeof(tSirMacSSid));
        }
        if(pSrcProfile->nAddIEAssocLength)
        {
            status = palAllocateMemory(pMac->hHdd, (void **)&pDstProfile->pAddIEAssoc, pSrcProfile->nAddIEAssocLength);
            if(!HAL_STATUS_SUCCESS(status))
            {
                smsLog( pMac, LOGE, FL(" failed to allocate memory for additional IEs ") );
                break;
            }
            pDstProfile->nAddIEAssocLength = pSrcProfile->nAddIEAssocLength;
            palCopyMemory(pMac->hHdd, pDstProfile->pAddIEAssoc, pSrcProfile->pAddIEAssoc,
                pSrcProfile->nAddIEAssocLength);
        }
        status = palAllocateMemory(pMac->hHdd, (void **)&pDstProfile->ChannelInfo.ChannelList, 1);
        if(!HAL_STATUS_SUCCESS(status))
        {
           break;
        }
        pDstProfile->ChannelInfo.numOfChannels = 1;
        pDstProfile->ChannelInfo.ChannelList[0] = pSrcProfile->operationChannel;
        pDstProfile->AuthType.numEntries = 1;
        pDstProfile->AuthType.authType[0] = pSrcProfile->AuthType;
        pDstProfile->negotiatedAuthType = pSrcProfile->AuthType;
        pDstProfile->EncryptionType.numEntries = 1;
        pDstProfile->EncryptionType.encryptionType[0] = pSrcProfile->EncryptionType;
        pDstProfile->negotiatedUCEncryptionType = pSrcProfile->EncryptionType;
        pDstProfile->mcEncryptionType.numEntries = 1;
        pDstProfile->mcEncryptionType.encryptionType[0] = pSrcProfile->mcEncryptionType;
        pDstProfile->negotiatedMCEncryptionType = pSrcProfile->mcEncryptionType;
        pDstProfile->BSSType = pSrcProfile->BSSType;
        pDstProfile->CBMode = pSrcProfile->CBMode;
        palCopyMemory(pMac->hHdd, &pDstProfile->Keys, &pSrcProfile->Keys, sizeof(pDstProfile->Keys));
#ifdef WLAN_FEATURE_VOWIFI_11R
        if (pSrcProfile->MDID.mdiePresent)
        {
            pDstProfile->MDID.mdiePresent = 1;
            pDstProfile->MDID.mobilityDomain = pSrcProfile->MDID.mobilityDomain;
        }
#endif
    
    }while(0);
    
    if(!HAL_STATUS_SUCCESS(status))
    {
        csrReleaseProfile(pMac, pDstProfile);
        pDstProfile = NULL;
    }
    
    return (status);
}

eHalStatus csrRoamIssueConnect(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, 
                                tScanResultHandle hBSSList, 
                                eCsrRoamReason reason, tANI_U32 roamId, tANI_BOOLEAN fImediate,
                                tANI_BOOLEAN fClearScan)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSmeCmd *pCommand;
    
    pCommand = csrGetCommandBuffer(pMac);
    if(NULL == pCommand)
    {
        smsLog( pMac, LOGE, FL(" fail to get command buffer") );
        status = eHAL_STATUS_RESOURCES;
    }
    else
    {
        if( fClearScan )
        {
            csrScanCancelIdleScan(pMac);
            csrScanAbortMacScanNotForConnect(pMac);
        }
        pCommand->u.roamCmd.fReleaseProfile = eANI_BOOLEAN_FALSE;
        if(NULL == pProfile)
        {
            //               
            //                                                                                   
            //                                          
            pCommand->u.roamCmd.roamProfile.AuthType.numEntries = 1; 
            pCommand->u.roamCmd.roamProfile.AuthType.authType[0] = eCSR_AUTH_TYPE_OPEN_SYSTEM;
            pCommand->u.roamCmd.roamProfile.EncryptionType.numEntries = 1;
            pCommand->u.roamCmd.roamProfile.EncryptionType.encryptionType[0] = eCSR_ENCRYPT_TYPE_NONE;
            pCommand->u.roamCmd.roamProfile.csrPersona = VOS_STA_MODE; 
        }
        else
        {
            //                          
            status = csrRoamCopyProfile(pMac, &pCommand->u.roamCmd.roamProfile, pProfile);
            if(HAL_STATUS_SUCCESS(status))
            {
                pCommand->u.roamCmd.fReleaseProfile = eANI_BOOLEAN_TRUE;
            }
        }
        pCommand->command = eSmeCommandRoam;
        pCommand->sessionId = (tANI_U8)sessionId;
        pCommand->u.roamCmd.hBSSList = hBSSList;
        pCommand->u.roamCmd.roamId = roamId;
        pCommand->u.roamCmd.roamReason = reason;
        //                                                    
        pCommand->u.roamCmd.fReleaseBssList = eANI_BOOLEAN_TRUE;
        pCommand->u.roamCmd.fUpdateCurRoamProfile = eANI_BOOLEAN_TRUE;
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
                  FL("CSR PERSONA=%d"),
                  pCommand->u.roamCmd.roamProfile.csrPersona);
        status = csrQueueSmeCommand(pMac, pCommand, fImediate);
        if( !HAL_STATUS_SUCCESS( status ) )
        {
            smsLog( pMac, LOGE, FL(" fail to send message status = %d"), status );
            csrReleaseCommandRoam( pMac, pCommand );
        }
    }
    
    return (status);
}
eHalStatus csrRoamIssueReassoc(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile,
                               tCsrRoamModifyProfileFields *pMmodProfileFields,
                               eCsrRoamReason reason, tANI_U32 roamId, tANI_BOOLEAN fImediate)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSmeCmd *pCommand;
    
    pCommand = csrGetCommandBuffer(pMac);
    if(NULL == pCommand)
    {
        smsLog( pMac, LOGE, FL(" fail to get command buffer") );
        status = eHAL_STATUS_RESOURCES;
    }
    else
    {
        csrScanCancelIdleScan(pMac);
        csrScanAbortMacScanNotForConnect(pMac);
        if(pProfile)
        {
           //                                                     
           pCommand->u.roamCmd.fReleaseProfile = eANI_BOOLEAN_FALSE;
           //                          
           status = csrRoamCopyProfile(pMac, &pCommand->u.roamCmd.roamProfile, pProfile);
           pCommand->u.roamCmd.fUpdateCurRoamProfile = eANI_BOOLEAN_TRUE;
        }
        else
        {
            status = csrRoamCopyConnectedProfile(pMac, sessionId, &pCommand->u.roamCmd.roamProfile);
            //                                            
            pCommand->u.roamCmd.roamProfile.uapsd_mask = pMmodProfileFields->uapsd_mask;
        }
        if(HAL_STATUS_SUCCESS(status))
        {
           pCommand->u.roamCmd.fReleaseProfile = eANI_BOOLEAN_TRUE;
        }
        pCommand->command = eSmeCommandRoam;
        pCommand->sessionId = (tANI_U8)sessionId;
        pCommand->u.roamCmd.roamId = roamId;
        pCommand->u.roamCmd.roamReason = reason;
        //                                                    
        //                                                             
        pCommand->u.roamCmd.hBSSList = CSR_INVALID_SCANRESULT_HANDLE; 
        pCommand->u.roamCmd.fReleaseBssList = eANI_BOOLEAN_FALSE;
        pCommand->u.roamCmd.fReassoc = eANI_BOOLEAN_TRUE;
        status = csrQueueSmeCommand(pMac, pCommand, fImediate);
        if( !HAL_STATUS_SUCCESS( status ) )
    {
            smsLog( pMac, LOGE, FL(" fail to send message status = %d"), status );
            csrRoamCompletion(pMac, sessionId, NULL, pCommand, eCSR_ROAM_RESULT_FAILURE, eANI_BOOLEAN_FALSE);
            csrReleaseCommandRoam( pMac, pCommand );
    }
    }
    return (status);
}

eHalStatus csrRoamEnqueuePreauth(tpAniSirGlobal pMac, tANI_U32 sessionId, tpSirBssDescription pBssDescription,
                                eCsrRoamReason reason, tANI_BOOLEAN fImmediate)
//                                                                                               
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSmeCmd *pCommand;
    
    pCommand = csrGetCommandBuffer(pMac);
    if(NULL == pCommand)
    {
        smsLog( pMac, LOGE, FL(" fail to get command buffer") );
        status = eHAL_STATUS_RESOURCES;
    }
    else
    {
        if(pBssDescription)
        {
            //                                      
            pCommand->command = eSmeCommandRoam;
            pCommand->sessionId = (tANI_U8)sessionId;
            pCommand->u.roamCmd.roamReason = reason;
            //                               
            //                                                        
            pCommand->u.roamCmd.pLastRoamBss = pBssDescription;
            status = csrQueueSmeCommand(pMac, pCommand, fImmediate);
            if( !HAL_STATUS_SUCCESS( status ) )
            {
                smsLog( pMac, LOGE, FL(" fail to enqueue preauth command, status = %d"), status );
                csrReleaseCommandPreauth( pMac, pCommand );
            }
        }
        else
        {
           //              
           status = eHAL_STATUS_RESOURCES;
        }
    }
    return (status);
}

eHalStatus csrRoamDequeuePreauth(tpAniSirGlobal pMac)
{
    tListElem *pEntry;
    tSmeCmd *pCommand;
    pEntry = csrLLPeekHead( &pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK );
    if ( pEntry )
    {
        pCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
        if ( (eSmeCommandRoam == pCommand->command) && 
                (eCsrPerformPreauth == pCommand->u.roamCmd.roamReason))
        {             
            smsLog( pMac, LOG1, FL("DQ-Command = %d, Reason = %d"),
                    pCommand->command, pCommand->u.roamCmd.roamReason);
            if (csrLLRemoveEntry( &pMac->sme.smeCmdActiveList, pEntry, LL_ACCESS_LOCK )) {
                csrReleaseCommandPreauth( pMac, pCommand );
            }
        } else  {
            smsLog( pMac, LOGE, FL("Command = %d, Reason = %d "),
                    pCommand->command, pCommand->u.roamCmd.roamReason);
        }
    }
    else {
        smsLog( pMac, LOGE, FL("pEntry NULL for eWNI_SME_FT_PRE_AUTH_RSP"));
    }
    smeProcessPendingQueue( pMac );
    return eHAL_STATUS_SUCCESS;
}

eHalStatus csrRoamConnectWithBSSList(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, 
                                     tScanResultHandle hBssListIn, tANI_U32 *pRoamId)
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    tScanResultHandle hBSSList;
    tANI_U32 roamId = 0;
    status = csrScanCopyResultList(pMac, hBssListIn, &hBSSList);
    if(HAL_STATUS_SUCCESS(status))
    {
        roamId = GET_NEXT_ROAM_ID(&pMac->roam);
        if(pRoamId)
        {
            *pRoamId = roamId;
        }
        status = csrRoamIssueConnect(pMac, sessionId, pProfile, hBSSList, eCsrHddIssued, 
                                        roamId, eANI_BOOLEAN_FALSE, eANI_BOOLEAN_FALSE);
        if(!HAL_STATUS_SUCCESS(status))
        {
            smsLog(pMac, LOGE, FL("failed to start a join process"));
            csrScanResultPurge(pMac, hBSSList);
        }
    }
    return (status);
}

eHalStatus csrRoamConnect(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, 
                          tScanResultHandle hBssListIn, tANI_U32 *pRoamId)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tScanResultHandle hBSSList;
    tCsrScanResultFilter *pScanFilter;
    tANI_U32 roamId = 0;
    tANI_BOOLEAN fCallCallback = eANI_BOOLEAN_FALSE;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    if (NULL == pProfile)
    {
        smsLog(pMac, LOGP, FL("No profile specified"));
        return eHAL_STATUS_FAILURE;
    }
    smsLog(pMac, LOG1, FL("called  BSSType = %d authtype = %d  encryType = %d"),
                pProfile->BSSType, pProfile->AuthType.authType[0], pProfile->EncryptionType.encryptionType[0]);
    if( CSR_IS_WDS( pProfile ) && 
        !HAL_STATUS_SUCCESS( status = csrIsBTAMPAllowed( pMac, pProfile->operationChannel ) ) )
    {
        smsLog(pMac, LOGE, FL("Request for BT AMP connection failed, channel requested is different than infra = %d"),
               pProfile->operationChannel);
        return status;
    }
    csrRoamCancelRoaming(pMac, sessionId);
    csrScanRemoveFreshScanCommand(pMac, sessionId);
    csrScanCancelIdleScan(pMac);
    //                                                                    
    csrScanAbortMacScan(pMac);
    if (!vos_concurrent_sessions_running() && (VOS_STA_SAP_MODE == pProfile->csrPersona))//                                                
    {
        csrScanDisable(pMac);
    }
    csrRoamRemoveDuplicateCommand(pMac, sessionId, NULL, eCsrHddIssued);
    //                          
    if(csrIsConnStateConnected(pMac, sessionId))
    {
        if(pProfile->SSIDs.numOfSSIDs && !csrIsSsidInList(pMac, &pSession->connectedProfile.SSID, &pProfile->SSIDs))
        {
            csrRoamIssueDisassociateCmd(pMac, sessionId, eCSR_DISCONNECT_REASON_UNSPECIFIED);
        }
    }
#ifdef FEATURE_WLAN_BTAMP_UT_RF
    pSession->maxRetryCount = CSR_JOIN_MAX_RETRY_COUNT; 
#endif
    if(CSR_INVALID_SCANRESULT_HANDLE != hBssListIn)
    {
        smsLog(pMac, LOG1, FL("is called with BSSList"));
        status = csrRoamConnectWithBSSList(pMac, sessionId, pProfile, hBssListIn, pRoamId);
        if(pRoamId)
        {
            roamId = *pRoamId;
        }
        if(!HAL_STATUS_SUCCESS(status))
        {
            fCallCallback = eANI_BOOLEAN_TRUE;
        }
    }
    else
    {
        status = palAllocateMemory(pMac->hHdd, (void **)&pScanFilter, sizeof(tCsrScanResultFilter));
        if(HAL_STATUS_SUCCESS(status))
        {
            palZeroMemory(pMac->hHdd, pScanFilter, sizeof(tCsrScanResultFilter));
            //                         
            if(NULL == pProfile)
            {
                //             
                pScanFilter->EncryptionType.numEntries = 1;
                pScanFilter->EncryptionType.encryptionType[0] = eCSR_ENCRYPT_TYPE_NONE;
            }//                       
            else 
            {
                //                                         
                status = csrRoamPrepareFilterFromProfile(pMac, pProfile, pScanFilter);
            }//                 
            roamId = GET_NEXT_ROAM_ID(&pMac->roam);
            if(pRoamId)
            {
                *pRoamId = roamId;
            }
            
            if(HAL_STATUS_SUCCESS(status))
            {
                /*                 */
                if(NULL != pProfile)
                {
                    pScanFilter->bWPSAssociation = pProfile->bWPSAssociation;
                }
                else
                {
                    pScanFilter->bWPSAssociation = 0;
                }
                do
                {
                    if( (pProfile && CSR_IS_WDS_AP( pProfile ) )
                     || (pProfile && CSR_IS_INFRA_AP ( pProfile ))
                    )
                    {
                        //                              
                        status = csrRoamIssueConnect(pMac, sessionId, pProfile, NULL, eCsrHddIssued, 
                                                    roamId, eANI_BOOLEAN_FALSE, eANI_BOOLEAN_FALSE);
                        if(!HAL_STATUS_SUCCESS(status))
                        {
                            smsLog(pMac, LOGE, FL("   CSR failed to issue start BSS command with status = 0x%08X"), status);
                            fCallCallback = eANI_BOOLEAN_TRUE;
                        }
                        else
                        {
                            smsLog(pMac, LOG1, FL("Connect request to proceed for AMP/SoftAP mode"));
                        }
                        break;
                    }
                    status = csrScanGetResult(pMac, pScanFilter, &hBSSList);
                    smsLog(pMac, LOG1, "************ csrScanGetResult Status ********* %d", status);
                    if(HAL_STATUS_SUCCESS(status))
                    {
                        status = csrRoamIssueConnect(pMac, sessionId, pProfile, hBSSList, eCsrHddIssued, 
                                                    roamId, eANI_BOOLEAN_FALSE, eANI_BOOLEAN_FALSE);
                        if(!HAL_STATUS_SUCCESS(status))
                        {
                            smsLog(pMac, LOGE, FL("   CSR failed to issue connect command with status = 0x%08X"), status);
                            csrScanResultPurge(pMac, hBSSList);
                            fCallCallback = eANI_BOOLEAN_TRUE;
                        }
                    }//                
                    else if(NULL != pProfile)
                    {
                        //                                  
                        if(CSR_IS_START_IBSS(pProfile))
                        {
                            status = csrRoamIssueConnect(pMac, sessionId, pProfile, NULL, eCsrHddIssued, 
                                                        roamId, eANI_BOOLEAN_FALSE, eANI_BOOLEAN_FALSE);
                            if(!HAL_STATUS_SUCCESS(status))
                            {
                                smsLog(pMac, LOGE, "   CSR failed to issue startIBSS command with status = 0x%08X", status);
                                fCallCallback = eANI_BOOLEAN_TRUE;
                            }
                        }
                        else
                        {
                            //                  
                            status = csrScanForSSID(pMac, sessionId, pProfile, roamId, TRUE);
                            if(!HAL_STATUS_SUCCESS(status))
                            {
                                smsLog(pMac, LOGE, FL("   CSR failed to issue SSID scan command with status = 0x%08X"), status);
                                fCallCallback = eANI_BOOLEAN_TRUE;
                            }
                            else
                            {
                                smsLog(pMac, LOG1, FL("SSID scan requested for Infra connect req"));
                            }
                        }
                    }
                    else
                    {
                        fCallCallback = eANI_BOOLEAN_TRUE;
                    }
                } while (0);
                if(NULL != pProfile)
                {
                    //                                                   
                    csrFreeScanFilter(pMac, pScanFilter);
                }
            }//                                
            
            palFreeMemory(pMac->hHdd, pScanFilter);
        }//                                
    }//                    
    //                                                    
    if( fCallCallback )
    {
        csrRoamCallCallback(pMac, sessionId, NULL, roamId, eCSR_ROAM_FAILED, eCSR_ROAM_RESULT_FAILURE);
    }
   
    return (status);
}                         
eHalStatus csrRoamReassoc(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile,
                          tCsrRoamModifyProfileFields modProfileFields,
                          tANI_U32 *pRoamId)
{
   eHalStatus status = eHAL_STATUS_SUCCESS;
   tANI_BOOLEAN fCallCallback = eANI_BOOLEAN_TRUE;
   tANI_U32 roamId = 0;
   tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
   if (NULL == pProfile)
   {
      smsLog(pMac, LOGP, FL("No profile specified"));
      return eHAL_STATUS_FAILURE;
   }
   smsLog(pMac, LOG1, FL("called  BSSType = %d authtype = %d  encryType = %d"), pProfile->BSSType, pProfile->AuthType.authType[0], pProfile->EncryptionType.encryptionType[0]);
   csrRoamCancelRoaming(pMac, sessionId);
   csrScanRemoveFreshScanCommand(pMac, sessionId);
   csrScanCancelIdleScan(pMac);
   csrScanAbortMacScanNotForConnect(pMac);
   csrRoamRemoveDuplicateCommand(pMac, sessionId, NULL, eCsrHddIssuedReassocToSameAP);
   if(csrIsConnStateConnected(pMac, sessionId))
   {
      if(pProfile)
      {
         if(pProfile->SSIDs.numOfSSIDs && 
            csrIsSsidInList(pMac, &pSession->connectedProfile.SSID, &pProfile->SSIDs))
         {
            fCallCallback = eANI_BOOLEAN_FALSE;
         }
         else
         {
            smsLog(pMac, LOG1, FL("Not connected to the same SSID asked in the profile"));
         }
      }
      else if(!palEqualMemory(pMac->hHdd, &modProfileFields, 
                              &pSession->connectedProfile.modifyProfileFields, 
                              sizeof(tCsrRoamModifyProfileFields)))
      {
         fCallCallback = eANI_BOOLEAN_FALSE;
      }
      else
      {
         smsLog(pMac, LOG1, FL("Either the profile is NULL or none of the fields "
                               "in tCsrRoamModifyProfileFields got modified"));
      }
   }
   else
   {
      smsLog(pMac, LOG1, FL("Not connected! No need to reassoc"));
   }
   if(!fCallCallback)
   {
      roamId = GET_NEXT_ROAM_ID(&pMac->roam);
      if(pRoamId)
      {
         *pRoamId = roamId;
      }

      status = csrRoamIssueReassoc(pMac, sessionId, pProfile, &modProfileFields, 
                                   eCsrHddIssuedReassocToSameAP, roamId, eANI_BOOLEAN_FALSE);
   }
   else
   {
      status = csrRoamCallCallback(pMac, sessionId, NULL, roamId, 
                                   eCSR_ROAM_FAILED, eCSR_ROAM_RESULT_FAILURE);
   }
   return status;
}
eHalStatus csrRoamJoinLastProfile(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    tScanResultHandle hBSSList = NULL;
    tCsrScanResultFilter *pScanFilter = NULL;
    tANI_U32 roamId;
    tCsrRoamProfile *pProfile = NULL;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    do
    {
        if(pSession->pCurRoamProfile)
        {
            csrScanCancelIdleScan(pMac);
            csrScanAbortMacScanNotForConnect(pMac);
            //                                                                                            
            status = palAllocateMemory(pMac->hHdd, (void **)&pProfile, sizeof(tCsrRoamProfile));
            if(!HAL_STATUS_SUCCESS(status))
                break;
            palZeroMemory(pMac->hHdd, pProfile, sizeof(tCsrRoamProfile));
            status = csrRoamCopyProfile(pMac, pProfile, pSession->pCurRoamProfile);
            if(!HAL_STATUS_SUCCESS(status))
                break;
            status = palAllocateMemory(pMac->hHdd, (void **)&pScanFilter, sizeof(tCsrScanResultFilter));
            if(!HAL_STATUS_SUCCESS(status))
            {
                break;
            }
            palZeroMemory(pMac->hHdd, pScanFilter, sizeof(tCsrScanResultFilter));
            status = csrRoamPrepareFilterFromProfile(pMac, pProfile, pScanFilter);
            if(!HAL_STATUS_SUCCESS(status))
            {
                break;
            }
            roamId = GET_NEXT_ROAM_ID(&pMac->roam);
            status = csrScanGetResult(pMac, pScanFilter, &hBSSList);
            if(HAL_STATUS_SUCCESS(status))
            {
                //                                                                        
                csrMoveBssToHeadFromBSSID(pMac, &pSession->connectedProfile.bssid, hBSSList);
                status = csrRoamIssueConnect(pMac, sessionId, pProfile, hBSSList, eCsrHddIssued, 
                                                roamId, eANI_BOOLEAN_FALSE, eANI_BOOLEAN_FALSE);
                if(!HAL_STATUS_SUCCESS(status))
                {
                    csrScanResultPurge(pMac, hBSSList);
                    break;
                }
            }
            else
            {
                //                         
                //                                                      
                status = csrScanForSSID(pMac, sessionId, pProfile, roamId, TRUE);
                if(!HAL_STATUS_SUCCESS(status))
                {
                    break;
                }
            }
        }//                 
        else
        {
            smsLog(pMac, LOGW, FL("cannot find a roaming profile"));
            break;
        }
    }while(0);
    if(pScanFilter)
    {
        csrFreeScanFilter(pMac, pScanFilter);
        palFreeMemory(pMac->hHdd, pScanFilter);
    }
    if(NULL != pProfile)
    {
        csrReleaseProfile(pMac, pProfile);
        palFreeMemory(pMac->hHdd, pProfile);
    }
    return (status);
}
eHalStatus csrRoamReconnect(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    if(csrIsConnStateConnected(pMac, sessionId))
    {
        status = csrRoamIssueDisassociateCmd(pMac, sessionId, eCSR_DISCONNECT_REASON_UNSPECIFIED);
        if(HAL_STATUS_SUCCESS(status))
        {
            status = csrRoamJoinLastProfile(pMac, sessionId);
        }
    }
    return (status);
}

eHalStatus csrRoamConnectToLastProfile(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    smsLog(pMac, LOGW, FL("is called"));
    csrRoamCancelRoaming(pMac, sessionId);
    csrRoamRemoveDuplicateCommand(pMac, sessionId, NULL, eCsrHddIssued);
    if(csrIsConnStateDisconnected(pMac, sessionId))
    {
        status = csrRoamJoinLastProfile(pMac, sessionId);
    }
    return (status);
}

eHalStatus csrRoamProcessDisassocDeauth( tpAniSirGlobal pMac, tSmeCmd *pCommand, tANI_BOOLEAN fDisassoc, tANI_BOOLEAN fMICFailure )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_BOOLEAN fComplete = eANI_BOOLEAN_FALSE;
    eCsrRoamSubState NewSubstate;
    tANI_U32 sessionId = pCommand->sessionId;
    
    //                             
    csrRoamStateChange( pMac, eCSR_ROAMING_STATE_JOINING, sessionId );

    if ( csrIsConnStateIbss( pMac, sessionId ) )
    {
        //                                            
        status = csrRoamIssueStopBss( pMac, sessionId, eCSR_ROAM_SUBSTATE_STOP_BSS_REQ );
        fComplete = (!HAL_STATUS_SUCCESS(status));
    }
    else if ( csrIsConnStateInfra( pMac, sessionId ) )
    {
        smsLog(pMac, LOG1, FL(" restore AC weights (%d-%d-%d-%d)"), pMac->roam.ucACWeights[0], pMac->roam.ucACWeights[1],
            pMac->roam.ucACWeights[2], pMac->roam.ucACWeights[3]);
        //                                       
        WLANTL_SetACWeights(pMac->roam.gVosContext, pMac->roam.ucACWeights);
        //                                                                              
        NewSubstate = eCSR_ROAM_SUBSTATE_DISASSOC_FORCED;
        if(eCsrSmeIssuedDisassocForHandoff == pCommand->u.roamCmd.roamReason)
        {
            NewSubstate = eCSR_ROAM_SUBSTATE_DISASSOC_HANDOFF;
        }
        if( fDisassoc )
        {
            status = csrRoamIssueDisassociate( pMac, sessionId, NewSubstate, fMICFailure );
        }
        else
        {
            status = csrRoamIssueDeauth( pMac, sessionId, eCSR_ROAM_SUBSTATE_DEAUTH_REQ );
        }
        fComplete = (!HAL_STATUS_SUCCESS(status));
    }
    else if ( csrIsConnStateWds( pMac, sessionId ) )
    {
        if( CSR_IS_WDS_AP( &pMac->roam.roamSession[sessionId].connectedProfile ) )
        {
            status = csrRoamIssueStopBss( pMac, sessionId, eCSR_ROAM_SUBSTATE_STOP_BSS_REQ );
            fComplete = (!HAL_STATUS_SUCCESS(status));
        }
        //                          
        else  if( csrIsConnStateConnectedWds( pMac, sessionId ) ) //                          
        {
 
            pCommand->u.roamCmd.fStopWds = eANI_BOOLEAN_TRUE;
            if( fDisassoc )
            {
                status = csrRoamIssueDisassociate( pMac, sessionId, 
                                eCSR_ROAM_SUBSTATE_DISCONNECT_CONTINUE_ROAMING, fMICFailure );
                fComplete = (!HAL_STATUS_SUCCESS(status));
            }
        }
    } else {
        //                                                           
        //                          
           fComplete = eANI_BOOLEAN_TRUE;
           status = eHAL_STATUS_FAILURE;
    }
    if(fComplete)
    {
        csrRoamComplete( pMac, eCsrNothingToJoin, NULL );
    }

    if(HAL_STATUS_SUCCESS(status))
    {
        if ( csrIsConnStateInfra( pMac, sessionId ) )
        {
            //                                 
            pMac->roam.roamSession[sessionId].connectState = eCSR_ASSOC_STATE_TYPE_NOT_CONNECTED;
        }
    }
    else
    {
        smsLog(pMac, LOGW, FL(" failed with status %d"), status);
    }
    return (status);
}

/*                                            */
/*
                                                                                 
 
                      
                                             
                                                           
                                                                                     
                      
 
*/

eHalStatus csrRoamIssueDisassociateCmd( tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamDisconnectReason reason )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSmeCmd *pCommand;
        tANI_BOOLEAN fHighPriority = eANI_BOOLEAN_FALSE;
    do
    {
        smsLog( pMac, LOG1, FL("  reason = %d"), reason );
        pCommand = csrGetCommandBuffer( pMac );
        if ( !pCommand ) 
        {
            smsLog( pMac, LOGE, FL(" fail to get command buffer") );
            status = eHAL_STATUS_RESOURCES;
            break;
        }
        //                                              
        if( CSR_IS_WAIT_FOR_KEY( pMac, sessionId ) )
        {
            csrRoamStopWaitForKeyTimer( pMac );
            csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_NONE, sessionId);
        }
        pCommand->command = eSmeCommandRoam;
        pCommand->sessionId = (tANI_U8)sessionId;
        switch ( reason )
        {
        case eCSR_DISCONNECT_REASON_MIC_ERROR:
            pCommand->u.roamCmd.roamReason = eCsrForcedDisassocMICFailure;
            break;
        case eCSR_DISCONNECT_REASON_DEAUTH:
            pCommand->u.roamCmd.roamReason = eCsrForcedDeauth;
            break;
        case eCSR_DISCONNECT_REASON_HANDOFF:
            fHighPriority = eANI_BOOLEAN_TRUE;
            pCommand->u.roamCmd.roamReason = eCsrSmeIssuedDisassocForHandoff;
            break;
        case eCSR_DISCONNECT_REASON_UNSPECIFIED:
        case eCSR_DISCONNECT_REASON_DISASSOC:
            pCommand->u.roamCmd.roamReason = eCsrForcedDisassoc;
            break;
        case eCSR_DISCONNECT_REASON_IBSS_JOIN_FAILURE:
            pCommand->u.roamCmd.roamReason = eCsrSmeIssuedIbssJoinFailure;
            break;
        case eCSR_DISCONNECT_REASON_IBSS_LEAVE:
            pCommand->u.roamCmd.roamReason = eCsrForcedIbssLeave;
            break;
        default:
            break;
        }
        status = csrQueueSmeCommand(pMac, pCommand, fHighPriority);
        if( !HAL_STATUS_SUCCESS( status ) )
        {
            smsLog( pMac, LOGE, FL(" fail to send message status = %d"), status );
            csrReleaseCommandRoam( pMac, pCommand );
        }
    } while( 0 );
    return( status );
}

eHalStatus csrRoamIssueStopBssCmd( tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_BOOLEAN fHighPriority )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSmeCmd *pCommand;
    pCommand = csrGetCommandBuffer( pMac );
    if ( NULL != pCommand ) 
    {
        //                                              
        if( CSR_IS_WAIT_FOR_KEY( pMac, sessionId) )
        {
            csrRoamStopWaitForKeyTimer( pMac );
            csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_NONE, sessionId);
        }
        pCommand->command = eSmeCommandRoam;
        pCommand->sessionId = (tANI_U8)sessionId;
        pCommand->u.roamCmd.roamReason = eCsrStopBss;
        status = csrQueueSmeCommand(pMac, pCommand, fHighPriority);
        if( !HAL_STATUS_SUCCESS( status ) )
        {
            smsLog( pMac, LOGE, FL(" fail to send message status = %d"), status );
            csrReleaseCommandRoam( pMac, pCommand );
        }
    }
    else
    {
        smsLog( pMac, LOGE, FL(" fail to get command buffer") );
        status = eHAL_STATUS_RESOURCES;
    }
    return ( status );
}

eHalStatus csrRoamDisconnectInternal(tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamDisconnectReason reason)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
#ifdef FEATURE_WLAN_BTAMP_UT_RF
    //             
    pSession->maxRetryCount = 0;
    csrRoamStopJoinRetryTimer(pMac, sessionId);
#endif
    //                               
    //                                    
    if(csrIsConnStateConnected(pMac, sessionId) || csrIsBssTypeIBSS(pSession->connectedProfile.BSSType) 
                || csrIsBssTypeWDS(pSession->connectedProfile.BSSType) 
                || csrIsRoamCommandWaitingForSession(pMac, sessionId) )
                
    {
        smsLog(pMac, LOG2, FL("called"));
        status = csrRoamIssueDisassociateCmd(pMac, sessionId, reason);
    }
    else
    {
        smsLog( pMac, LOGE, FL("Roam command is not present"));
    }
    return (status);
}

eHalStatus csrRoamDisconnect(tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamDisconnectReason reason)
{
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    csrRoamCancelRoaming(pMac, sessionId);
    pSession->ibss_join_pending = FALSE;
    csrRoamStopIbssJoinTimer(pMac, sessionId);
    csrRoamRemoveDuplicateCommand(pMac, sessionId, NULL, eCsrForcedDisassoc);
    
    return (csrRoamDisconnectInternal(pMac, sessionId, reason));
}

eHalStatus csrRoamSaveConnectedInfomation(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, 
                                          tSirBssDescription *pSirBssDesc, tDot11fBeaconIEs *pIes)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tDot11fBeaconIEs *pIesTemp = pIes;
    tANI_U8 index;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    tCsrRoamConnectedProfile *pConnectProfile = &pSession->connectedProfile;

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }

    if(pConnectProfile->pAddIEAssoc)
    {
        palFreeMemory(pMac->hHdd, pConnectProfile->pAddIEAssoc);
        pConnectProfile->pAddIEAssoc = NULL;
    }
    palZeroMemory(pMac->hHdd, &pSession->connectedProfile, sizeof(tCsrRoamConnectedProfile));
    pConnectProfile->AuthType = pProfile->negotiatedAuthType;
        pConnectProfile->AuthInfo = pProfile->AuthType;
    pConnectProfile->CBMode = pProfile->CBMode;  //                         
    pConnectProfile->EncryptionType = pProfile->negotiatedUCEncryptionType;
        pConnectProfile->EncryptionInfo = pProfile->EncryptionType;
    pConnectProfile->mcEncryptionType = pProfile->negotiatedMCEncryptionType;
        pConnectProfile->mcEncryptionInfo = pProfile->mcEncryptionType;
    pConnectProfile->BSSType = pProfile->BSSType;
    pConnectProfile->modifyProfileFields.uapsd_mask = pProfile->uapsd_mask;
    pConnectProfile->operationChannel = pSirBssDesc->channelId;
    pConnectProfile->beaconInterval = pSirBssDesc->beaconInterval;

    if (!pConnectProfile->beaconInterval)
    {
        smsLog(pMac, LOGW, FL("ERROR: Beacon interval is ZERO"));
    }

    palCopyMemory(pMac->hHdd, &pConnectProfile->Keys, &pProfile->Keys, sizeof(tCsrKeys));

    /*                                                                                     */
    if(pProfile->nAddIEAssocLength)
    {
        status = palAllocateMemory(pMac->hHdd,
                 (void **)&pConnectProfile->pAddIEAssoc, pProfile->nAddIEAssocLength);
        if(!HAL_STATUS_SUCCESS(status))
        {
            smsLog(pMac, LOGE, FL("Failed to allocate memory for additional IEs")) ;
            return eHAL_STATUS_FAILURE;
        }
        pConnectProfile->nAddIEAssocLength = pProfile->nAddIEAssocLength;
        palCopyMemory(pMac->hHdd, pConnectProfile->pAddIEAssoc, pProfile->pAddIEAssoc,
                pProfile->nAddIEAssocLength);
    }
    
    //          
    csrGetBssIdBssDesc(pMac, pSirBssDesc, &pConnectProfile->bssid);
#ifdef WLAN_FEATURE_VOWIFI_11R
    if (pSirBssDesc->mdiePresent)
    {
        pConnectProfile->MDID.mdiePresent = 1;
        pConnectProfile->MDID.mobilityDomain = (pSirBssDesc->mdie[1] << 8) | (pSirBssDesc->mdie[0]);
    }
#endif
    if( NULL == pIesTemp )
    {
        status = csrGetParsedBssDescriptionIEs(pMac, pSirBssDesc, &pIesTemp);
    }
#ifdef FEATURE_WLAN_CCX
    if ((csrIsProfileCCX(pProfile) || 
         ((pIesTemp->CCXVersion.present) 
          && ((pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_OPEN_SYSTEM) 
              || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_WPA) 
              || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_WPA_PSK) 
              || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_RSN) 
#ifdef WLAN_FEATURE_11W
              || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_RSN_PSK_SHA256)
#endif
              || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_RSN_PSK))))
        && (pMac->roam.configParam.isCcxIniFeatureEnabled))
    {
        pConnectProfile->isCCXAssoc = 1;
    }
#endif
    //         
    if(HAL_STATUS_SUCCESS(status))
    {
        if(pIesTemp->SSID.present)
        {
            pConnectProfile->SSID.length = pIesTemp->SSID.num_ssid;
            palCopyMemory(pMac->hHdd, pConnectProfile->SSID.ssId, 
                            pIesTemp->SSID.ssid, pIesTemp->SSID.num_ssid);
        }
        
        //                 
        status = csrRoamSaveConnectedBssDesc(pMac, sessionId, pSirBssDesc);
           
           if( CSR_IS_QOS_BSS(pIesTemp) || pIesTemp->HTCaps.present)
           {
              //                                                                                        
              pConnectProfile->qap = TRUE;
           }
           else
           {
              pConnectProfile->qap = FALSE;
           }
        if ( NULL == pIes )
        {
            //                                   
            palFreeMemory(pMac->hHdd, pIesTemp);
        }
    }
    //                   
    pConnectProfile->qosConnection = pMac->roam.roamSession[sessionId].fWMMConnection;
    
    if(!HAL_STATUS_SUCCESS(status))
    {
        csrFreeConnectBssDesc(pMac, sessionId);
    }
    for(index = 0; index < pProfile->SSIDs.numOfSSIDs; index++)
    {
       if((pProfile->SSIDs.SSIDList[index].SSID.length == pConnectProfile->SSID.length) &&
          palEqualMemory(pMac->hHdd, pProfile->SSIDs.SSIDList[index].SSID.ssId, 
                         pConnectProfile->SSID.ssId, pConnectProfile->SSID.length))
       {
          pConnectProfile->handoffPermitted = pProfile->SSIDs.SSIDList[index].handoffPermitted;
          break;
       }
       pConnectProfile->handoffPermitted = FALSE;
    }
    
    return (status);
}

static void csrRoamJoinRspProcessor( tpAniSirGlobal pMac, tSirSmeJoinRsp *pSmeJoinRsp )
{
   tListElem *pEntry = NULL;
   tSmeCmd *pCommand = NULL;
   //                                                  
   pEntry = csrLLPeekHead(&pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK);
   if(pEntry)
   {
       pCommand = GET_BASE_ADDR(pEntry, tSmeCmd, Link);
   }
   if ( eSIR_SME_SUCCESS == pSmeJoinRsp->statusCode ) 
   {
            if(pCommand && eCsrSmeIssuedAssocToSimilarAP == pCommand->u.roamCmd.roamReason)
            {
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
               sme_QosCsrEventInd(pMac, pSmeJoinRsp->sessionId, SME_QOS_CSR_HANDOFF_COMPLETE, NULL);
#endif
            }
            csrRoamComplete( pMac, eCsrJoinSuccess, (void *)pSmeJoinRsp );
   }
   else
   {
        tANI_U32 roamId = 0;
        tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, pSmeJoinRsp->sessionId );
        if(!pSession)
        {
            smsLog(pMac, LOGE, FL("  session %d not found "), pSmeJoinRsp->sessionId);
            return;
        }
        
        
        //                                                  
        //                                               
        if(pCommand)
        {
            roamId = pCommand->u.roamCmd.roamId;
        }
        pSession->joinFailStatusCode.statusCode = pSmeJoinRsp->statusCode;
        pSession->joinFailStatusCode.reasonCode = pSmeJoinRsp->protStatusCode;
        smsLog( pMac, LOGW, "SmeJoinReq failed with statusCode= 0x%08lX [%d]", pSmeJoinRsp->statusCode, pSmeJoinRsp->statusCode );
#if   defined WLAN_FEATURE_NEIGHBOR_ROAMING
        /*                                                                                                     */
        if (csrRoamIsHandoffInProgress(pMac))
        {
            csrRoamCallCallback(pMac, pSmeJoinRsp->sessionId, NULL, roamId, eCSR_ROAM_DISASSOCIATED, eCSR_ROAM_RESULT_FORCED);
            /*                                                                        */
            csrNeighborRoamIndicateConnect(pMac, pSmeJoinRsp->sessionId, VOS_STATUS_E_FAILURE);
        }
#endif
        if (pCommand)
        {
            if(CSR_IS_WDS_STA( &pCommand->u.roamCmd.roamProfile ))
            {
              pCommand->u.roamCmd.fStopWds = eANI_BOOLEAN_TRUE;
              pSession->connectedProfile.BSSType = eCSR_BSS_TYPE_WDS_STA;
              csrRoamReissueRoamCommand(pMac);
            }
            else if( CSR_IS_WDS( &pCommand->u.roamCmd.roamProfile ) )
            {
                csrRoamComplete( pMac, eCsrNothingToJoin, NULL );
            }
            else
            {
                csrRoam(pMac, pCommand);
            }    
        }    
        else
        {
           csrRoamComplete( pMac, eCsrNothingToJoin, NULL );
        }
    } /*                                                      */
}

eHalStatus csrRoamIssueJoin( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirBssDescription *pSirBssDesc, 
                             tDot11fBeaconIEs *pIes,
                             tCsrRoamProfile *pProfile, tANI_U32 roamId )
{
    eHalStatus status;
    smsLog( pMac, LOG1, "Attempting to Join Bssid= "MAC_ADDRESS_STR,
                  MAC_ADDR_ARRAY(pSirBssDesc->bssId));
    
    //                                              
    csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_JOIN_REQ, sessionId);
    //                            
    status = csrSendJoinReqMsg( pMac, sessionId, pSirBssDesc, pProfile, pIes, eWNI_SME_JOIN_REQ );
    return (status);
}

static eHalStatus csrRoamIssueReassociate( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirBssDescription *pSirBssDesc, 
                              tDot11fBeaconIEs *pIes, tCsrRoamProfile *pProfile)
{
    csrRoamStateChange( pMac, eCSR_ROAMING_STATE_JOINING, sessionId);
    //                                              
    csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_REASSOC_REQ, sessionId );

     VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
               FL(" calling csrSendJoinReqMsg (eWNI_SME_REASSOC_REQ)"));
    
    //                            
    return csrSendJoinReqMsg( pMac, sessionId, pSirBssDesc, pProfile, pIes, eWNI_SME_REASSOC_REQ);
}

void csrRoamReissueRoamCommand(tpAniSirGlobal pMac)
{
    tListElem *pEntry;
    tSmeCmd *pCommand;
    tCsrRoamInfo roamInfo;
    tANI_U32 sessionId;
    tCsrRoamSession *pSession;
            
    pEntry = csrLLPeekHead(&pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK);
    if(pEntry)
    {
        pCommand = GET_BASE_ADDR(pEntry, tSmeCmd, Link);
        if ( eSmeCommandRoam == pCommand->command )
        {
            sessionId = pCommand->sessionId;
            pSession = CSR_GET_SESSION( pMac, sessionId );

            if(!pSession)
            {
                smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                return;
            }
            /*                                                                    
                                                                                  
                                                                                    
                                              
            */
            if(pSession->bRefAssocStartCnt > 0)
            {
                pSession->bRefAssocStartCnt--;
            }
            if( pCommand->u.roamCmd.fStopWds )
            {
                palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
                roamInfo.pBssDesc = pCommand->u.roamCmd.pLastRoamBss;
                roamInfo.statusCode = pSession->joinFailStatusCode.statusCode;
                roamInfo.reasonCode = pSession->joinFailStatusCode.reasonCode;
                if (CSR_IS_WDS(&pSession->connectedProfile)){
                pSession->connectState = eCSR_ASSOC_STATE_TYPE_WDS_DISCONNECTED;
                csrRoamCallCallback(pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId, 
                                        eCSR_ROAM_WDS_IND, 
                                        eCSR_ROAM_RESULT_WDS_DISASSOCIATED);
                                }else if (CSR_IS_INFRA_AP(&pSession->connectedProfile)){
                                        pSession->connectState = eCSR_ASSOC_STATE_TYPE_INFRA_DISCONNECTED;
                                        csrRoamCallCallback(pMac, sessionId, &roamInfo, pCommand->u.roamCmd.roamId,
                                                                                eCSR_ROAM_INFRA_IND,
                                                                                eCSR_ROAM_RESULT_INFRA_DISASSOCIATED);
                                }  
 

                if( !HAL_STATUS_SUCCESS( csrRoamIssueStopBss( pMac, sessionId, eCSR_ROAM_SUBSTATE_STOP_BSS_REQ ) ) )
                {
                    smsLog(pMac, LOGE, " Failed to reissue stop_bss command for WDS after disassociated");
                    csrRoamComplete( pMac, eCsrNothingToJoin, NULL );
                }
            }
            else if(eCsrStopRoaming == csrRoamJoinNextBss(pMac, pCommand, eANI_BOOLEAN_TRUE))
            {
                smsLog(pMac, LOGW, " Failed to reissue join command after disassociated");
                csrRoamComplete( pMac, eCsrNothingToJoin, NULL );
            }
        }
        else
        {
            smsLog(pMac, LOGW, "  Command is not roaming after disassociated");
        }
    }
    else 
    {
        smsLog(pMac, LOGE, "   Disassoc rsp cannot continue because no command is available");
    }
}

tANI_BOOLEAN csrIsRoamCommandWaitingForSession(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    tANI_BOOLEAN fRet = eANI_BOOLEAN_FALSE;
    tListElem *pEntry;
    tSmeCmd *pCommand = NULL;
    //                                                   
    csrLLLock( &pMac->sme.smeCmdActiveList );
    pEntry = csrLLPeekHead(&pMac->sme.smeCmdActiveList, LL_ACCESS_NOLOCK);
    if(pEntry)
    {
        pCommand = GET_BASE_ADDR(pEntry, tSmeCmd, Link);
        if( ( eSmeCommandRoam == pCommand->command ) && ( sessionId == pCommand->sessionId ) )
        {
            fRet = eANI_BOOLEAN_TRUE;
        }
    }
    if(eANI_BOOLEAN_FALSE == fRet)
    {
        csrLLLock(&pMac->sme.smeCmdPendingList);
        pEntry = csrLLPeekHead(&pMac->sme.smeCmdPendingList, LL_ACCESS_NOLOCK);
        while(pEntry)
        {
            pCommand = GET_BASE_ADDR(pEntry, tSmeCmd, Link);
            if( ( eSmeCommandRoam == pCommand->command ) && ( sessionId == pCommand->sessionId ) )
            {
                fRet = eANI_BOOLEAN_TRUE;
                break;
            }
            pEntry = csrLLNext(&pMac->sme.smeCmdPendingList, pEntry, LL_ACCESS_NOLOCK);
        }
        csrLLUnlock(&pMac->sme.smeCmdPendingList);
    }
    if (eANI_BOOLEAN_FALSE == fRet)
    {
        csrLLLock(&pMac->roam.roamCmdPendingList);
        pEntry = csrLLPeekHead(&pMac->roam.roamCmdPendingList, LL_ACCESS_NOLOCK);
        while (pEntry)
        {
            pCommand = GET_BASE_ADDR(pEntry, tSmeCmd, Link);
            if (( eSmeCommandRoam == pCommand->command ) && ( sessionId == pCommand->sessionId ) )
            {
                fRet = eANI_BOOLEAN_TRUE;
                break;
            }
            pEntry = csrLLNext(&pMac->roam.roamCmdPendingList, pEntry, LL_ACCESS_NOLOCK);
        }
        csrLLUnlock(&pMac->roam.roamCmdPendingList);
    }
    csrLLUnlock( &pMac->sme.smeCmdActiveList );
    return (fRet);
}

tANI_BOOLEAN csrIsRoamCommandWaiting(tpAniSirGlobal pMac)
{
    tANI_BOOLEAN fRet = eANI_BOOLEAN_FALSE;
    tANI_U32 i;
    for( i = 0; i < CSR_ROAM_SESSION_MAX; i++ )
    {
        if( CSR_IS_SESSION_VALID( pMac, i ) && ( fRet = csrIsRoamCommandWaitingForSession( pMac, i ) ) )
        {
            break;
        }
    }
    return ( fRet );
}

tANI_BOOLEAN csrIsCommandWaiting(tpAniSirGlobal pMac)
{
    tANI_BOOLEAN fRet = eANI_BOOLEAN_FALSE;
    //                                                   
    csrLLLock( &pMac->sme.smeCmdActiveList );
    fRet = csrLLIsListEmpty(&pMac->sme.smeCmdActiveList, LL_ACCESS_NOLOCK);
    if(eANI_BOOLEAN_FALSE == fRet)
    {
        fRet = csrLLIsListEmpty(&pMac->sme.smeCmdPendingList, LL_ACCESS_LOCK);
    }
    csrLLUnlock( &pMac->sme.smeCmdActiveList );
    return (fRet);
}

tANI_BOOLEAN csrIsScanForRoamCommandActive( tpAniSirGlobal pMac )
{
    tANI_BOOLEAN fRet = eANI_BOOLEAN_FALSE;
    tListElem *pEntry;
    tCsrCmd *pCommand;
    //                                                   
    csrLLLock( &pMac->sme.smeCmdActiveList );
    pEntry = csrLLPeekHead(&pMac->sme.smeCmdActiveList, LL_ACCESS_NOLOCK);
    if( pEntry )
    {
        pCommand = GET_BASE_ADDR(pEntry, tCsrCmd, Link);
        if( ( eCsrRoamCommandScan == pCommand->command ) && 
            ( ( eCsrScanForSsid == pCommand->u.scanCmd.reason ) || 
              ( eCsrScanForCapsChange == pCommand->u.scanCmd.reason ) ||
              ( eCsrScanP2PFindPeer == pCommand->u.scanCmd.reason ) ) )
        {
            fRet = eANI_BOOLEAN_TRUE;
        }
    }
    csrLLUnlock( &pMac->sme.smeCmdActiveList );
    return (fRet);
}
eHalStatus csrRoamIssueReassociateCmd( tpAniSirGlobal pMac, tANI_U32 sessionId )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSmeCmd *pCommand = NULL;
    tANI_BOOLEAN fHighPriority = eANI_BOOLEAN_TRUE;
    tANI_BOOLEAN fRemoveCmd = FALSE;
    tListElem *pEntry; 
    //                                                                        
    pEntry = csrLLPeekHead( &pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK );
    if ( pEntry )
    {
        pCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
        if ( !pCommand ) 
        {
            smsLog( pMac, LOGE, FL(" fail to get command buffer") );
            return eHAL_STATUS_RESOURCES;
        }
        if ( eSmeCommandRoam == pCommand->command )
        {
            if (pCommand->u.roamCmd.roamReason == eCsrSmeIssuedAssocToSimilarAP)
            {
                fRemoveCmd = csrLLRemoveEntry( &pMac->sme.smeCmdActiveList, pEntry, LL_ACCESS_LOCK );
            }
            else 
            {
                smsLog( pMac, LOGE, FL(" Unexpected active roam command present ") );
            }
            if (fRemoveCmd == FALSE)
            {
                //                                                       
                //               
                pCommand = NULL;
            }
        }
    }
    if(NULL == pCommand)
    {
        smsLog( pMac, LOGE, FL(" fail to get command buffer as expected based on previous connect roam command") );
        return eHAL_STATUS_RESOURCES;
    }
    do 
    {
        //                                              
        if( CSR_IS_WAIT_FOR_KEY( pMac, sessionId ) )
        {
            csrRoamStopWaitForKeyTimer( pMac );
            csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_NONE, sessionId );
        }
        pCommand->command = eSmeCommandRoam;
        pCommand->sessionId = (tANI_U8)sessionId;
        pCommand->u.roamCmd.roamReason = eCsrSmeIssuedFTReassoc;
        status = csrQueueSmeCommand(pMac, pCommand, fHighPriority);
        if( !HAL_STATUS_SUCCESS( status ) )
        {
            smsLog( pMac, LOGE, FL(" fail to send message status = %d"), status );
            csrReleaseCommandRoam( pMac, pCommand );
        }
    } while( 0 );

    return( status );
}
static void csrRoamingStateConfigCnfProcessor( tpAniSirGlobal pMac, tANI_U32 result )
{
    tListElem *pEntry = csrLLPeekHead(&pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK);
    tCsrScanResult *pScanResult = NULL;
    tSirBssDescription *pBssDesc = NULL;
    tSmeCmd *pCommand = NULL;
    tANI_U32 sessionId;
    tCsrRoamSession *pSession;
    if(NULL == pEntry)
    {
        smsLog(pMac, LOGW, "   CFG_CNF with active list empty");
        return;
    }
    pCommand = GET_BASE_ADDR(pEntry, tSmeCmd, Link);
    sessionId = pCommand->sessionId;
    pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return;
    }
    
    if(CSR_IS_ROAMING(pSession) && pSession->fCancelRoaming)
    {
        //                                                     
        smsLog(pMac, LOGW, FL("  Roam command cancelled"));
        csrRoamComplete(pMac, eCsrNothingToJoin, NULL); 
    }
    /*                                                                */
    else if ( !CSR_IS_ROAMING(pSession) && CSR_IS_ROAMING_COMMAND(pCommand) )
    {
        //                                                        
        smsLog(pMac, LOGW, FL("  Roam command (reason %d) aborted due to roaming completed\n"),
           pCommand->u.roamCmd.roamReason);
        csrSetAbortRoamingCommand( pMac, pCommand );
        csrRoamComplete(pMac, eCsrNothingToJoin, NULL);
    }
    else
    {
        if ( CCM_IS_RESULT_SUCCESS(result) )
        {
            smsLog(pMac, LOG2, "Cfg sequence complete");
            //                                                                           
            //                      
            if(pCommand->u.roamCmd.pRoamBssEntry)
            {
                pScanResult = GET_BASE_ADDR(pCommand->u.roamCmd.pRoamBssEntry, tCsrScanResult, Link);
                pBssDesc = &pScanResult->Result.BssDescriptor;
            }
            if ( csrIsBssTypeIBSS( pCommand->u.roamCmd.roamProfile.BSSType ) ||
                 CSR_IS_WDS( &pCommand->u.roamCmd.roamProfile ) 
                  || CSR_IS_INFRA_AP(&pCommand->u.roamCmd.roamProfile) 
            )
            {
                if(!HAL_STATUS_SUCCESS(csrRoamIssueStartBss( pMac, sessionId,
                                        &pSession->bssParams, &pCommand->u.roamCmd.roamProfile, 
                                        pBssDesc, pCommand->u.roamCmd.roamId )))
                {
                    smsLog(pMac, LOGW, " CSR start BSS failed");
                    //                               
                    csrRoamComplete(pMac, eCsrStartBssFailure, NULL);
                }
            }
            else
            {
                if (!pCommand->u.roamCmd.pRoamBssEntry)
                {
                    smsLog(pMac, LOGW, " pRoamBssEntry is NULL");
                    //                               
                    csrRoamComplete(pMac, eCsrJoinFailure, NULL);
                    return;
                } 
                //                                              
                VOS_ASSERT(pScanResult != NULL); 
                if ( csrIsInfraBssDesc( pBssDesc ) )
                {
                    tDot11fBeaconIEs *pIesLocal = (tDot11fBeaconIEs *)pScanResult->Result.pvIes;
                    if(pIesLocal || (HAL_STATUS_SUCCESS(csrGetParsedBssDescriptionIEs(pMac, pBssDesc, &pIesLocal))) )
                    {
                    //                                                    
                    if( csrIsConnStateConnectedInfra( pMac, sessionId ) )
                    {
                        //                                             
                        if (  csrIsSsidEqual( pMac, pSession->pConnectBssDesc, pBssDesc, 
                                                    pIesLocal ) )
                        //                                                
                        {
                            //                      
                            pCommand->u.roamCmd.fReassoc = eANI_BOOLEAN_TRUE;
                                csrRoamIssueReassociate( pMac, sessionId, pBssDesc, pIesLocal,
                                                        &pCommand->u.roamCmd.roamProfile );
                        }
                        else
                        {
                                                     
                            //                                                                                        
                            //                          
                            if(!HAL_STATUS_SUCCESS(csrRoamIssueJoin( pMac, sessionId, pBssDesc, 
                                                                                                            pIesLocal, 
                                                    &pCommand->u.roamCmd.roamProfile, pCommand->u.roamCmd.roamId )))
                            {
                                //                  
                                csrRoam( pMac, pCommand );
                            }
                        }
                    }
                    else
                    {
                        eHalStatus  status = eHAL_STATUS_SUCCESS;
                         
                        /*                                                                               
                                                                                                                */
                        /*                                                                                               
                                            */
#ifdef WLAN_FEATURE_VOWIFI_11R
                        if (csrRoamIsHandoffInProgress(pMac) && 
                                                csrRoamIs11rAssoc(pMac))
                        {
                            status = csrRoamIssueReassociate(pMac, sessionId, pBssDesc, 
                                    (tDot11fBeaconIEs *)( pScanResult->Result.pvIes ), &pCommand->u.roamCmd.roamProfile);
                        }
                        else
#endif
#ifdef FEATURE_WLAN_CCX
                        if (csrRoamIsHandoffInProgress(pMac) && 
                                                csrRoamIsCCXAssoc(pMac))
                        {
                            //                                   
                            status = csrRoamIssueReassociateCmd(pMac, sessionId);
                        }
                        else
#endif
#ifdef FEATURE_WLAN_LFR
                        if (csrRoamIsHandoffInProgress(pMac) && 
                                                csrRoamIsFastRoamEnabled(pMac, sessionId))
                        {
                            //                                   
                            status = csrRoamIssueReassociateCmd(pMac, sessionId);
                        }
                        else
#endif
                        //                                                             
                        //              
                        {
                            status = csrRoamIssueJoin( pMac, sessionId, pBssDesc, 
                                                (tDot11fBeaconIEs *)( pScanResult->Result.pvIes ),
                                                &pCommand->u.roamCmd.roamProfile, pCommand->u.roamCmd.roamId );
                        }
                        if(!HAL_STATUS_SUCCESS(status))
                        {
                            //                  
                            csrRoam( pMac, pCommand );
                        }
                    }
                        if( !pScanResult->Result.pvIes )
                        {
                            //                 
                            palFreeMemory(pMac->hHdd, pIesLocal);
                        }
                    }
                }//                                    
                else
                {
                    smsLog(pMac, LOGW, FL("  found BSSType mismatching the one in BSS description"));
                }
            }//    
        }//                                
        else
        {
            //                                                                          
            //                                 
            if( pCommand->u.roamCmd.pRoamBssEntry && CSR_IS_INFRASTRUCTURE( &pCommand->u.roamCmd.roamProfile ) )
            {
            csrRoam(pMac, pCommand);
            }
            else
            {
                //                               
                if ( csrIsBssTypeIBSS( pCommand->u.roamCmd.roamProfile.BSSType ) )
                {
                    csrRoamComplete(pMac, eCsrStartBssFailure, NULL);
                }
                else
                {
                    csrRoamComplete( pMac, eCsrNothingToJoin, NULL );
                }
            }
        }
    }//                    
}

static void csrRoamRoamingStateAuthRspProcessor( tpAniSirGlobal pMac, tSirSmeAuthRsp *pSmeAuthRsp )
{
    //                                          
    smsLog(pMac, LOGW, FL("is no-op"));
    if ( eSIR_SME_SUCCESS == pSmeAuthRsp->statusCode ) 
    {
        smsLog( pMac, LOGW, "CSR SmeAuthReq Successful" );
        //                                                                                
        //                    
        /*                                                             
                                                                    */
    }
    else {
        smsLog( pMac, LOGW, "CSR SmeAuthReq failed with statusCode= 0x%08lX [%d]", pSmeAuthRsp->statusCode, pSmeAuthRsp->statusCode );
        /*                                         
                                                                                                        
                               */
    }
}

static void csrRoamRoamingStateReassocRspProcessor( tpAniSirGlobal pMac, tpSirSmeJoinRsp pSmeJoinRsp )
{
    eCsrRoamCompleteResult result;
    tpCsrNeighborRoamControlInfo    pNeighborRoamInfo = &pMac->roam.neighborRoamInfo;
    tCsrRoamInfo roamInfo;
    tANI_U32 roamId = 0;
    
    if ( eSIR_SME_SUCCESS == pSmeJoinRsp->statusCode ) 
    {
        smsLog( pMac, LOGW, "CSR SmeReassocReq Successful" );
        result = eCsrReassocSuccess;
        /*                                       */
#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
        /*                                                                                 
                                                                                                */
        if (csrRoamIsHandoffInProgress(pMac))
        {
            /*                                                         */
            sme_QosCsrEventInd(pMac, pSmeJoinRsp->sessionId, SME_QOS_CSR_HANDOFF_COMPLETE, NULL);
            csrRoamComplete( pMac, result, pSmeJoinRsp);
        }
        else
#endif
        {
            csrRoamComplete( pMac, result, NULL );
        }
    }
    /*                                                                     
                                                              */
    else
    {
        smsLog( pMac, LOGW, "CSR SmeReassocReq failed with statusCode= 0x%08lX [%d]", pSmeJoinRsp->statusCode, pSmeJoinRsp->statusCode );
        result = eCsrReassocFailure;
#ifdef WLAN_FEATURE_VOWIFI_11R
        if ((eSIR_SME_FT_REASSOC_TIMEOUT_FAILURE == pSmeJoinRsp->statusCode) ||
                        (eSIR_SME_FT_REASSOC_FAILURE == pSmeJoinRsp->statusCode))
        {
                //                                       
                if (pNeighborRoamInfo)
                {
                        vos_mem_zero(&roamInfo, sizeof(tCsrRoamInfo));
                        csrRoamCallCallback(pMac, pNeighborRoamInfo->csrSessionId,
                                        &roamInfo, roamId, eCSR_ROAM_FT_REASSOC_FAILED, eSIR_SME_SUCCESS);
                        /*
                                                                      
                                                                
                                                                       
                                                                       
                                                                          
                                                                         
                                                                        
                                      
                         */
                        csrRoamComplete( pMac, eCsrJoinFailure, NULL );
                        return;
                }
        }
#endif
        //                                                                                                         
        //                                                                                                      
        //                                                                                
        //                                                                 
        if(!HAL_STATUS_SUCCESS(csrRoamIssueDisassociate( pMac, pSmeJoinRsp->sessionId,
                        eCSR_ROAM_SUBSTATE_DISASSOC_REASSOC_FAILURE, FALSE )))
        {
            csrRoamComplete( pMac, eCsrJoinFailure, NULL );
        }
    }
}

static void csrRoamRoamingStateStopBssRspProcessor(tpAniSirGlobal pMac, tSirSmeRsp *pSmeRsp)
{
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
    {
        vos_log_ibss_pkt_type *pIbssLog;
        WLAN_VOS_DIAG_LOG_ALLOC(pIbssLog, vos_log_ibss_pkt_type, LOG_WLAN_IBSS_C);
        if(pIbssLog)
        {
            pIbssLog->eventId = WLAN_IBSS_EVENT_STOP_RSP;
            if(eSIR_SME_SUCCESS != pSmeRsp->statusCode)
            {
                pIbssLog->status = WLAN_IBSS_STATUS_FAILURE;
            }
            WLAN_VOS_DIAG_LOG_REPORT(pIbssLog);
        }
    }
#endif //                             
    pMac->roam.roamSession[pSmeRsp->sessionId].connectState = eCSR_ASSOC_STATE_TYPE_NOT_CONNECTED;
    if(CSR_IS_ROAM_SUBSTATE_STOP_BSS_REQ( pMac, pSmeRsp->sessionId))
    {
        csrRoamComplete( pMac, eCsrNothingToJoin, NULL );
    }
    else if(CSR_IS_ROAM_SUBSTATE_DISCONNECT_CONTINUE( pMac, pSmeRsp->sessionId))
    {
        csrRoamReissueRoamCommand(pMac);
    }
}

void csrRoamRoamingStateDisassocRspProcessor( tpAniSirGlobal pMac, tSirSmeDisassocRsp *pSmeRsp )
{
    tSirResultCodes statusCode;
#if defined WLAN_FEATURE_NEIGHBOR_ROAMING
    tScanResultHandle hBSSList;
    tANI_BOOLEAN fCallCallback, fRemoveCmd;
    eHalStatus status;
    tCsrRoamInfo roamInfo;
    tCsrScanResultFilter *pScanFilter = NULL;
    tANI_U32 roamId = 0;
    tCsrRoamProfile *pCurRoamProfile = NULL;
    tListElem *pEntry = NULL;
    tSmeCmd *pCommand = NULL;
#endif
    tANI_U32 sessionId;
    tCsrRoamSession *pSession;

    tSirSmeDisassocRsp SmeDisassocRsp;

    csrSerDesUnpackDiassocRsp((tANI_U8 *)pSmeRsp, &SmeDisassocRsp);
    sessionId = SmeDisassocRsp.sessionId;
    statusCode = SmeDisassocRsp.statusCode;

    smsLog( pMac, LOG2, "csrRoamRoamingStateDisassocRspProcessor sessionId %d", sessionId);

    if ( csrIsConnStateInfra( pMac, sessionId ) )
    {
        pMac->roam.roamSession[sessionId].connectState = eCSR_ASSOC_STATE_TYPE_NOT_CONNECTED;
    }
    pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return;
    }
    
    if ( CSR_IS_ROAM_SUBSTATE_DISASSOC_NO_JOIN( pMac, sessionId ) )
    {
        csrRoamComplete( pMac, eCsrNothingToJoin, NULL );
    }
    else if ( CSR_IS_ROAM_SUBSTATE_DISASSOC_FORCED( pMac, sessionId ) ||
              CSR_IS_ROAM_SUBSTATE_DISASSOC_REQ( pMac, sessionId ) )
    {
        if ( eSIR_SME_SUCCESS == statusCode )
        {
            smsLog( pMac, LOG2, "CSR SmeDisassocReq force disassociated Successfully" );
            //                                                                                    
        } 
        csrRoamComplete( pMac, eCsrNothingToJoin, NULL );
    }
    else if ( CSR_IS_ROAM_SUBSTATE_DISASSOC_HO( pMac, sessionId ) )
    {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO_HIGH,
                      "CSR SmeDisassocReq due to HO on session %d", sessionId );
#if   defined (WLAN_FEATURE_NEIGHBOR_ROAMING)
      /*
                                                                
                                                        
        */
       status = palAllocateMemory(pMac->hHdd, (void **)&pScanFilter, sizeof(tCsrScanResultFilter));
       if(HAL_STATUS_SUCCESS(status))
       {
           palZeroMemory(pMac->hHdd, pScanFilter, sizeof(tCsrScanResultFilter));
           status = csrRoamPrepareFilterFromProfile(pMac, 
                    &pMac->roam.neighborRoamInfo.csrNeighborRoamProfile, pScanFilter);
           if(!HAL_STATUS_SUCCESS(status))
           {
               smsLog(pMac, LOGE, "%s: failed to prepare scan filter with status %d",
                       __func__, status);
               goto POST_ROAM_FAILURE;
           }
           else
           {
               status = csrScanGetResult(pMac, pScanFilter, &hBSSList);
               if (!HAL_STATUS_SUCCESS(status))
               {
                   smsLog( pMac, LOGE,"%s: csrScanGetResult failed with status %d",
                           __func__, status);
                   goto POST_ROAM_FAILURE;
               }
           }
       }
       else
       {
           smsLog( pMac, LOGE,"%s: alloc for pScanFilter failed with status %d",
                   __func__, status);
           goto POST_ROAM_FAILURE;
       }

       /*
                                                                          
                                                   
        */
        pEntry = csrLLPeekHead( &pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK );
        if ( pEntry )
        {
            pCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
            /*                                                                    
                                              
             */
            if ( eSmeCommandRoam == pCommand->command )
            {

                /*
                                                                                           
                                                                                           
                                                          
                 */
                fRemoveCmd = csrLLRemoveEntry( &pMac->sme.smeCmdActiveList, pEntry, LL_ACCESS_LOCK );
                if(pCommand->u.roamCmd.fReleaseProfile)
                {
                    csrReleaseProfile(pMac, &pCommand->u.roamCmd.roamProfile);
                    pCommand->u.roamCmd.fReleaseProfile = eANI_BOOLEAN_FALSE;
                }
                if( fRemoveCmd )
                    csrReleaseCommandRoam( pMac, pCommand );
                else
                {
                    smsLog( pMac, LOGE, "%s: fail to remove cmd reason %d",
                            __func__, pCommand->u.roamCmd.roamReason );
                }
            }
            else
            {
                smsLog( pMac, LOGE, "%s: roam command not active", __func__ );
            }
        }
        else
        {
            smsLog( pMac, LOGE, "%s: NO commands are active", __func__ );
        }

        /*                                                    */
        roamInfo.reasonCode = eCsrRoamReasonBetterAP;

        palCopyMemory(pMac->hHdd, roamInfo.bssid, 
            pMac->roam.neighborRoamInfo.csrNeighborRoamProfile.BSSIDs.bssid, 
            sizeof(tSirMacAddr));

        csrRoamCallCallback(pMac,sessionId, &roamInfo, 0, 
            eCSR_ROAM_ROAMING_START, eCSR_ROAM_RESULT_NONE);

        /*                                                                          */
        if(HAL_STATUS_SUCCESS(palAllocateMemory(pMac->hHdd, 
           (void **)&pCurRoamProfile, sizeof(tCsrRoamProfile))))
        {
            palZeroMemory(pMac->hHdd, pCurRoamProfile, sizeof(tCsrRoamProfile));
            csrRoamCopyProfile(pMac, pCurRoamProfile, pSession->pCurRoamProfile);
            //                                                
            status = csrRoamIssueConnect(pMac, sessionId, pCurRoamProfile, 
                    hBSSList, eCsrSmeIssuedAssocToSimilarAP, 
                    roamId, eANI_BOOLEAN_TRUE, eANI_BOOLEAN_FALSE);

            if(!HAL_STATUS_SUCCESS(status))
            {
                smsLog( pMac, LOGE,"%s: csrRoamIssueConnect failed with status %d",
                        __func__, status);
                fCallCallback = eANI_BOOLEAN_TRUE;
            }
        
            /*                                                         */
            sme_QosCsrEventInd(pMac, sessionId, SME_QOS_CSR_HANDOFF_ASSOC_REQ, NULL);
            csrReleaseProfile(pMac, pCurRoamProfile);
            palFreeMemory(pMac->hHdd, pCurRoamProfile);
            csrFreeScanFilter(pMac, pScanFilter);
            palFreeMemory( pMac->hHdd, pScanFilter );
            return;
        }

POST_ROAM_FAILURE:
        if (pScanFilter)
        {
            csrFreeScanFilter(pMac, pScanFilter);
            palFreeMemory( pMac->hHdd, pScanFilter );
        }
        if (pCurRoamProfile)
            palFreeMemory(pMac->hHdd, pCurRoamProfile);

        /*                                                 */
        vos_mem_zero(&roamInfo, sizeof(tCsrRoamInfo));
        csrRoamCallCallback(pMac, sessionId,
                &roamInfo, 0, eCSR_ROAM_FT_REASSOC_FAILED, eSIR_SME_SUCCESS);

        /* 
                                                                                        
                                                                                    
                                                                                  
                                                                                    
                 
         */
        if (!HAL_STATUS_SUCCESS(csrRoamIssueDisassociate(pMac, sessionId, 
            eCSR_ROAM_SUBSTATE_DISASSOC_REASSOC_FAILURE, FALSE)))
        {
            smsLog( pMac, LOGE,"%s: csrRoamIssueDisassociate failed with status %d",
                    __func__, status);
            csrRoamComplete( pMac, eCsrJoinFailure, NULL );
        }
#endif

    } //                                                    
    else if ( CSR_IS_ROAM_SUBSTATE_REASSOC_FAIL( pMac, sessionId ) )
    {
        //                                                             
        csrRoamComplete( pMac, eCsrJoinFailure, NULL );
    }
    else
    {
        if ( eSIR_SME_SUCCESS == statusCode )
        {
            //                                                 
            //
            //                                                  
            //                                                                                   
            //                                                                                      
            //                        
            //                                                                                     
            //                                                                                
            
            smsLog( pMac, LOG2, "CSR SmeDisassocReq disassociated Successfully" );
        }
        else
        {
            smsLog( pMac, LOGE, "SmeDisassocReq failed with statusCode= 0x%08lX", statusCode );
        }
        //                                                      
        csrRoamReissueRoamCommand(pMac);
    }
}

static void csrRoamRoamingStateDeauthRspProcessor( tpAniSirGlobal pMac, tSirSmeDeauthRsp *pSmeRsp )
{
    tSirResultCodes statusCode;
    //                                            
    smsLog(pMac, LOGW, FL("is no-op"));
    statusCode = csrGetDeAuthRspStatusCode( pSmeRsp );
    pMac->roam.deauthRspStatus = statusCode;
    if ( CSR_IS_ROAM_SUBSTATE_DEAUTH_REQ( pMac, pSmeRsp->sessionId) )
    {
        csrRoamComplete( pMac, eCsrNothingToJoin, NULL );
    }
    else
    {
        if ( eSIR_SME_SUCCESS == statusCode ) 
        {
            //                                          
            //
            smsLog( pMac, LOG2, "CSR SmeDeauthReq disassociated Successfully" );
        }
        else
        {
            smsLog( pMac, LOGW, "SmeDeauthReq failed with statusCode= 0x%08lX", statusCode );
        }
        //                                                      
        csrRoamReissueRoamCommand(pMac);
    }
}

static void csrRoamRoamingStateStartBssRspProcessor( tpAniSirGlobal pMac, tSirSmeStartBssRsp *pSmeStartBssRsp )
{
    eCsrRoamCompleteResult result;
    
    if ( eSIR_SME_SUCCESS == pSmeStartBssRsp->statusCode ) 
    {
        smsLog( pMac, LOGW, "SmeStartBssReq Successful" );
        result = eCsrStartBssSuccess;
    }
    else 
    {
        smsLog( pMac, LOGW, "SmeStartBssReq failed with statusCode= 0x%08lX", pSmeStartBssRsp->statusCode );
        //                                     
        result = eCsrStartBssFailure;
    }
    csrRoamComplete( pMac, result, pSmeStartBssRsp);
}

/*
                                                                                         
                                                                                        
                                                                                       
                                                                                         
                   
*/
void csrRoamingStateMsgProcessor( tpAniSirGlobal pMac, void *pMsgBuf )
{
    tSirSmeRsp *pSmeRsp;
    tSmeIbssPeerInd *pIbssPeerInd;
    tCsrRoamInfo roamInfo;
        //                                                     
        tANI_U32 sessionId = 0;
    pSmeRsp = (tSirSmeRsp *)pMsgBuf;
    smsLog( pMac, LOG2, "Message %d[0x%04X] received in substate %d",
                pSmeRsp->messageType, pSmeRsp->messageType,
                pMac->roam.curSubState[pSmeRsp->sessionId] );
    pSmeRsp->messageType = (pSmeRsp->messageType);
    pSmeRsp->length = (pSmeRsp->length);
    pSmeRsp->statusCode = (pSmeRsp->statusCode);
    switch (pSmeRsp->messageType) 
    {
        
        case eWNI_SME_JOIN_RSP:      //                                                       
            if (CSR_IS_ROAM_SUBSTATE_JOIN_REQ(pMac, pSmeRsp->sessionId))
            {
                //                  
                csrRoamJoinRspProcessor( pMac, (tSirSmeJoinRsp *)pSmeRsp );
            }
            break;
                
        case eWNI_SME_AUTH_RSP:       //                                        
            if (CSR_IS_ROAM_SUBSTATE_AUTH_REQ( pMac, pSmeRsp->sessionId) ) 
            {
                //                  
                csrRoamRoamingStateAuthRspProcessor( pMac, (tSirSmeAuthRsp *)pSmeRsp );
            }
            break;
                
        case eWNI_SME_REASSOC_RSP:     //                                         
            if (CSR_IS_ROAM_SUBSTATE_REASSOC_REQ( pMac, pSmeRsp->sessionId) ) 
            {
                csrRoamRoamingStateReassocRspProcessor( pMac, (tpSirSmeJoinRsp )pSmeRsp );
            }
            break;
                   
        case eWNI_SME_STOP_BSS_RSP:    //                                    
            {
                csrRoamRoamingStateStopBssRspProcessor(pMac, pSmeRsp);
            }
            break;
                
        case eWNI_SME_DISASSOC_RSP:    //                                        
            if ( CSR_IS_ROAM_SUBSTATE_DISASSOC_REQ( pMac, pSmeRsp->sessionId )      ||
                 CSR_IS_ROAM_SUBSTATE_DISASSOC_NO_JOIN( pMac, pSmeRsp->sessionId )  ||
                 CSR_IS_ROAM_SUBSTATE_REASSOC_FAIL( pMac, pSmeRsp->sessionId )      ||
                 CSR_IS_ROAM_SUBSTATE_DISASSOC_FORCED( pMac, pSmeRsp->sessionId )   ||
                 CSR_IS_ROAM_SUBSTATE_DISCONNECT_CONTINUE( pMac, pSmeRsp->sessionId ) ||
//  
                 CSR_IS_ROAM_SUBSTATE_DISASSOC_HO( pMac, pSmeRsp->sessionId )         )
            {
                smsLog(pMac, LOG1, FL("eWNI_SME_DISASSOC_RSP subState = %d"), pMac->roam.curSubState[pSmeRsp->sessionId]);
                csrRoamRoamingStateDisassocRspProcessor( pMac, (tSirSmeDisassocRsp *)pSmeRsp );
            }
            break;
                   
        case eWNI_SME_DEAUTH_RSP:    //                                            
            if ( CSR_IS_ROAM_SUBSTATE_DEAUTH_REQ( pMac, pSmeRsp->sessionId ) ) 
            {
                csrRoamRoamingStateDeauthRspProcessor( pMac, (tSirSmeDeauthRsp *)pSmeRsp );
            }
            break;
                   
        case eWNI_SME_START_BSS_RSP:      //                                     
            if (CSR_IS_ROAM_SUBSTATE_START_BSS_REQ( pMac, pSmeRsp->sessionId ) ) 
            {
                csrRoamRoamingStateStartBssRspProcessor( pMac, (tSirSmeStartBssRsp *)pSmeRsp );
            } 
            break;
                   
        case WNI_CFG_SET_CNF:    //                                                                        
            if ( CSR_IS_ROAM_SUBSTATE_CONFIG( pMac, pSmeRsp->sessionId ) ) 
            {
                csrRoamingStateConfigCnfProcessor( pMac, ((tCsrCfgSetRsp *)pSmeRsp)->respStatus );
            }
            break;
        //                                                                                                
        case eWNI_SME_IBSS_PEER_DEPARTED_IND:
            pIbssPeerInd = (tSmeIbssPeerInd*)pSmeRsp;
            smsLog(pMac, LOGE, "CSR: Peer departed notification from LIM in joining state");
            palZeroMemory( pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo) );
                        roamInfo.staId = (tANI_U8)pIbssPeerInd->staId;
            roamInfo.ucastSig = (tANI_U8)pIbssPeerInd->ucastSig;
            roamInfo.bcastSig = (tANI_U8)pIbssPeerInd->bcastSig;
            palCopyMemory(pMac->hHdd, &roamInfo.peerMac, pIbssPeerInd->peerAddr, sizeof(tCsrBssid));
            csrRoamCallCallback(pMac, sessionId, &roamInfo, 0, 
                                eCSR_ROAM_CONNECT_STATUS_UPDATE, 
                                eCSR_ROAM_RESULT_IBSS_PEER_DEPARTED);
            break;
        default:
            smsLog( pMac, LOG1, "Unexpected message type = %d[0x%X] received in substate %d",
                      pSmeRsp->messageType, pSmeRsp->messageType,
                      pMac->roam.curSubState[pSmeRsp->sessionId] );

            //                                                  
                        if(!csrIsConnStateDisconnected(pMac, sessionId))
                        {
                                csrRoamCheckForLinkStatusChange( pMac, pSmeRsp );
                        }
            break;          
    }
}

void csrRoamJoinedStateMsgProcessor( tpAniSirGlobal pMac, void *pMsgBuf )
{
    tSirSmeRsp *pSirMsg = (tSirSmeRsp *)pMsgBuf;
    switch (pSirMsg->messageType) 
    {
       case eWNI_SME_GET_STATISTICS_RSP:
          smsLog( pMac, LOG2, FL("Stats rsp from PE"));
          csrRoamStatsRspProcessor( pMac, pSirMsg );
          break;
        case eWNI_SME_UPPER_LAYER_ASSOC_CNF:
        {
            tCsrRoamSession  *pSession;
            tSirSmeAssocIndToUpperLayerCnf *pUpperLayerAssocCnf;
            tCsrRoamInfo roamInfo;
            tCsrRoamInfo *pRoamInfo = NULL;
            tANI_U32 sessionId;
            eHalStatus status;
            smsLog( pMac, LOG1, FL("ASSOCIATION confirmation can be given to upper layer "));
            palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
            pRoamInfo = &roamInfo;
            pUpperLayerAssocCnf = (tSirSmeAssocIndToUpperLayerCnf *)pMsgBuf;
            status = csrRoamGetSessionIdFromBSSID( pMac, (tCsrBssid *)pUpperLayerAssocCnf->bssId, &sessionId );
            pSession = CSR_GET_SESSION(pMac, sessionId);

            if(!pSession)
            {
                smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                return;
            }
            
            pRoamInfo->statusCode = eSIR_SME_SUCCESS; //                                
            pRoamInfo->u.pConnectedProfile = &pSession->connectedProfile;
            pRoamInfo->staId = (tANI_U8)pUpperLayerAssocCnf->aid;
            pRoamInfo->rsnIELen = (tANI_U8)pUpperLayerAssocCnf->rsnIE.length;
            pRoamInfo->prsnIE = pUpperLayerAssocCnf->rsnIE.rsnIEdata;
            pRoamInfo->addIELen = (tANI_U8)pUpperLayerAssocCnf->addIE.length;
            pRoamInfo->paddIE = pUpperLayerAssocCnf->addIE.addIEdata;           
            palCopyMemory(pMac->hHdd, pRoamInfo->peerMac, pUpperLayerAssocCnf->peerMacAddr, sizeof(tSirMacAddr));
            palCopyMemory(pMac->hHdd, &pRoamInfo->bssid, pUpperLayerAssocCnf->bssId, sizeof(tCsrBssid));
            pRoamInfo->wmmEnabledSta = pUpperLayerAssocCnf->wmmEnabledSta;
            if(CSR_IS_INFRA_AP(pRoamInfo->u.pConnectedProfile) )
            {
                pMac->roam.roamSession[sessionId].connectState = eCSR_ASSOC_STATE_TYPE_INFRA_CONNECTED;
                pRoamInfo->fReassocReq = pUpperLayerAssocCnf->reassocReq;
                status = csrRoamCallCallback(pMac, sessionId, pRoamInfo, 0, eCSR_ROAM_INFRA_IND, eCSR_ROAM_RESULT_INFRA_ASSOCIATION_CNF);
            }
            if(CSR_IS_WDS_AP( pRoamInfo->u.pConnectedProfile))
            {
                vos_sleep( 100 );
                pMac->roam.roamSession[sessionId].connectState = eCSR_ASSOC_STATE_TYPE_WDS_CONNECTED;//   
                status = csrRoamCallCallback(pMac, sessionId, pRoamInfo, 0, eCSR_ROAM_WDS_IND, eCSR_ROAM_RESULT_WDS_ASSOCIATION_IND);//   
            }

        }
        break;
       default:
          csrRoamCheckForLinkStatusChange( pMac, pSirMsg );
          break;
    }
}

eHalStatus csrRoamIssueSetContextReq( tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrEncryptionType EncryptType, 
                                     tSirBssDescription *pBssDescription,
                                tSirMacAddr *bssId, tANI_BOOLEAN addKey,
                                 tANI_BOOLEAN fUnicast, tAniKeyDirection aniKeyDirection, 
                                 tANI_U8 keyId, tANI_U16 keyLength, 
                                 tANI_U8 *pKey, tANI_U8 paeRole )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tAniEdType edType;
    
    if(eCSR_ENCRYPT_TYPE_UNKNOWN == EncryptType)
    {
        EncryptType = eCSR_ENCRYPT_TYPE_NONE; //   
    }
    
    edType = csrTranslateEncryptTypeToEdType( EncryptType );
    
    //                                                                                                              
    //                                                                                                            
    if ( CSR_IS_ENC_TYPE_STATIC( EncryptType ) ||
           addKey )     
    {
        tCsrRoamSetKey setKey;
        setKey.encType = EncryptType;
        setKey.keyDirection = aniKeyDirection;    //                   
        palCopyMemory( pMac->hHdd, &setKey.peerMac, bssId, sizeof(tCsrBssid) );   
        setKey.paeRole = paeRole;      //                
        setKey.keyId = keyId;  //          
        setKey.keyLength = keyLength;  
        if( keyLength )
        {
            palCopyMemory( pMac->hHdd, setKey.Key, pKey, keyLength );
        }
        status = csrRoamIssueSetKeyCommand( pMac, sessionId, &setKey, 0 );
    }
    return (status);
}

static eHalStatus csrRoamIssueSetKeyCommand( tpAniSirGlobal pMac, tANI_U32 sessionId, 
                                             tCsrRoamSetKey *pSetKey, tANI_U32 roamId )
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
    tSmeCmd *pCommand = NULL;
#ifdef FEATURE_WLAN_CCX
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
#endif /*                  */
 
    do
    {
        pCommand = csrGetCommandBuffer(pMac);
        if(NULL == pCommand)
        {
            smsLog( pMac, LOGE, FL(" fail to get command buffer") );
            status = eHAL_STATUS_RESOURCES;
            break;
        }
        pCommand->command = eSmeCommandSetKey;
        pCommand->sessionId = (tANI_U8)sessionId;
        //                                                
        //                                                        
        if ( ( eCSR_ENCRYPT_TYPE_WEP40 == pSetKey->encType ) || 
             ( eCSR_ENCRYPT_TYPE_WEP40_STATICKEY == pSetKey->encType ) ) 
        {
            //                                
            if( pSetKey->keyLength )
            {
                if ( pSetKey->keyLength < CSR_WEP40_KEY_LEN ) 
                {
                    smsLog( pMac, LOGW, "Invalid WEP40 keylength [= %d] in SetContext call", pSetKey->keyLength );
                    break;        
                }
                
                pCommand->u.setKeyCmd.keyLength = CSR_WEP40_KEY_LEN;
                palCopyMemory( pMac->hHdd, pCommand->u.setKeyCmd.Key, pSetKey->Key, CSR_WEP40_KEY_LEN );
            }
        }
        else if ( ( eCSR_ENCRYPT_TYPE_WEP104 == pSetKey->encType ) || 
             ( eCSR_ENCRYPT_TYPE_WEP104_STATICKEY == pSetKey->encType ) ) 
        {
            //                                
            if( pSetKey->keyLength )
            {
                if ( pSetKey->keyLength < CSR_WEP104_KEY_LEN ) 
                {
                    smsLog( pMac, LOGW, "Invalid WEP104 keylength [= %d] in SetContext call", pSetKey->keyLength );
                    break;        
                }
                
                pCommand->u.setKeyCmd.keyLength = CSR_WEP104_KEY_LEN;
                palCopyMemory( pMac->hHdd, pCommand->u.setKeyCmd.Key, pSetKey->Key, CSR_WEP104_KEY_LEN );
            }
        }
        else if ( eCSR_ENCRYPT_TYPE_TKIP == pSetKey->encType ) 
        {
            if ( pSetKey->keyLength < CSR_TKIP_KEY_LEN )
            {
                smsLog( pMac, LOGW, "Invalid TKIP keylength [= %d] in SetContext call", pSetKey->keyLength );
                break;
            }
            pCommand->u.setKeyCmd.keyLength = CSR_TKIP_KEY_LEN;
            palCopyMemory( pMac->hHdd, pCommand->u.setKeyCmd.Key, pSetKey->Key, CSR_TKIP_KEY_LEN );
        }
        else if ( eCSR_ENCRYPT_TYPE_AES == pSetKey->encType ) 
        {
            if ( pSetKey->keyLength < CSR_AES_KEY_LEN )
            {
                smsLog( pMac, LOGW, "Invalid AES/CCMP keylength [= %d] in SetContext call", pSetKey->keyLength );
                break;
            }
            pCommand->u.setKeyCmd.keyLength = CSR_AES_KEY_LEN;
            palCopyMemory( pMac->hHdd, pCommand->u.setKeyCmd.Key, pSetKey->Key, CSR_AES_KEY_LEN );
        }
#ifdef FEATURE_WLAN_WAPI
        else if ( eCSR_ENCRYPT_TYPE_WPI == pSetKey->encType ) 
        {
            if ( pSetKey->keyLength < CSR_WAPI_KEY_LEN )
            {
                smsLog( pMac, LOGW, "Invalid WAPI keylength [= %d] in SetContext call", pSetKey->keyLength );
                break;
            }
            pCommand->u.setKeyCmd.keyLength = CSR_WAPI_KEY_LEN;
            palCopyMemory( pMac->hHdd, pCommand->u.setKeyCmd.Key, pSetKey->Key, CSR_WAPI_KEY_LEN );
        }
#endif /*                   */
#ifdef FEATURE_WLAN_CCX
        else if ( eCSR_ENCRYPT_TYPE_KRK == pSetKey->encType ) 
        {
            if ( pSetKey->keyLength < CSR_KRK_KEY_LEN )
            {
                smsLog( pMac, LOGW, "Invalid KRK keylength [= %d] in SetContext call", pSetKey->keyLength );
                break;
            }
            palCopyMemory( pMac->hHdd, pSession->ccxCckmInfo.krk, pSetKey->Key, CSR_KRK_KEY_LEN );
            pSession->ccxCckmInfo.reassoc_req_num=1;
            pSession->ccxCckmInfo.krk_plumbed = eANI_BOOLEAN_TRUE;
            status = eHAL_STATUS_SUCCESS;
            break;
        }
#endif /*                  */

#ifdef WLAN_FEATURE_11W
        //                 
        else if (eCSR_ENCRYPT_TYPE_AES_CMAC == pSetKey->encType)
        {
            if (pSetKey->keyLength < CSR_AES_KEY_LEN)
            {
                smsLog(pMac, LOGW, "Invalid AES/CCMP keylength [= %d] in SetContext call", pSetKey->keyLength);
                break;
            }
            pCommand->u.setKeyCmd.keyLength = CSR_AES_KEY_LEN;
            palCopyMemory(pMac->hHdd, pCommand->u.setKeyCmd.Key, pSetKey->Key, CSR_AES_KEY_LEN);
        }
#endif
        status = eHAL_STATUS_SUCCESS;
        pCommand->u.setKeyCmd.roamId = roamId;
        pCommand->u.setKeyCmd.encType = pSetKey->encType;
        pCommand->u.setKeyCmd.keyDirection = pSetKey->keyDirection;    //                   
        palCopyMemory( pMac->hHdd, &pCommand->u.setKeyCmd.peerMac, &pSetKey->peerMac, sizeof(tCsrBssid) );   
        pCommand->u.setKeyCmd.paeRole = pSetKey->paeRole;      //                
        pCommand->u.setKeyCmd.keyId = pSetKey->keyId;
        palCopyMemory( pMac->hHdd, pCommand->u.setKeyCmd.keyRsc, pSetKey->keyRsc, CSR_MAX_RSC_LEN );
        //                                                                                                            
         
        status = csrQueueSmeCommand(pMac, pCommand, eANI_BOOLEAN_TRUE);
        if( !HAL_STATUS_SUCCESS( status ) )
        {
            smsLog( pMac, LOGE, FL(" fail to send message status = %d"), status );
        }
    } while (0);
    //                                                          
    //                                                 
    if ( ( NULL != pCommand ) &&
         ( (!HAL_STATUS_SUCCESS( status ) )
#ifdef FEATURE_WLAN_CCX
            || ( eCSR_ENCRYPT_TYPE_KRK == pSetKey->encType ) 
#endif /*                  */
           ) )
    {
        csrReleaseCommandSetKey( pMac, pCommand );
    }
    return( status );
}

eHalStatus csrRoamIssueRemoveKeyCommand( tpAniSirGlobal pMac, tANI_U32 sessionId,
                                         tCsrRoamRemoveKey *pRemoveKey, tANI_U32 roamId )
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
    tSmeCmd *pCommand = NULL;
    tANI_BOOLEAN fImediate = eANI_BOOLEAN_TRUE;
    do
    {
        if( !csrIsSetKeyAllowed(pMac, sessionId) ) 
        {
            smsLog( pMac, LOGW, FL(" wrong state not allowed to set key") );
            status = eHAL_STATUS_CSR_WRONG_STATE;
            break;
        }
        pCommand = csrGetCommandBuffer(pMac);
        if(NULL == pCommand)
        {
            smsLog( pMac, LOGE, FL(" fail to get command buffer") );
            status = eHAL_STATUS_RESOURCES;
            break;
        }
        pCommand->command = eSmeCommandRemoveKey;
        pCommand->sessionId = (tANI_U8)sessionId;
        pCommand->u.removeKeyCmd.roamId = roamId;
        pCommand->u.removeKeyCmd.encType = pRemoveKey->encType;
        palCopyMemory( pMac->hHdd, &pCommand->u.removeKeyCmd.peerMac, &pRemoveKey->peerMac, sizeof(tSirMacAddr) );
        pCommand->u.removeKeyCmd.keyId = pRemoveKey->keyId;
        if( CSR_IS_WAIT_FOR_KEY( pMac, sessionId ) )
        {
            //                                                                           
            fImediate = eANI_BOOLEAN_FALSE;
        }
        smsLog( pMac, LOGE, FL("keyType=%d, keyId=%d, PeerMac="MAC_ADDRESS_STR),
            pRemoveKey->encType, pRemoveKey->keyId,
            MAC_ADDR_ARRAY(pCommand->u.removeKeyCmd.peerMac));
        status = csrQueueSmeCommand(pMac, pCommand, fImediate);
        if( !HAL_STATUS_SUCCESS( status ) )
        {
            smsLog( pMac, LOGE, FL(" fail to send message status = %d"), status );
            break;
        }
    } while (0);
    if( !HAL_STATUS_SUCCESS( status ) && ( NULL != pCommand ) )
    {
        csrReleaseCommandRemoveKey( pMac, pCommand );
    }
    return (status );
}

eHalStatus csrRoamProcessSetKeyCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
    eHalStatus status;
    tANI_U8 numKeys = ( pCommand->u.setKeyCmd.keyLength ) ? 1 : 0;
    tAniEdType edType = csrTranslateEncryptTypeToEdType( pCommand->u.setKeyCmd.encType );
    tANI_BOOLEAN fUnicast = ( pCommand->u.setKeyCmd.peerMac[0] == 0xFF ) ? eANI_BOOLEAN_FALSE : eANI_BOOLEAN_TRUE;
    tANI_U32 sessionId = pCommand->sessionId;
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    WLAN_VOS_DIAG_EVENT_DEF(setKeyEvent, vos_event_wlan_security_payload_type);

    if(NULL == pSession){
       smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
       return eHAL_STATUS_FAILURE;
    }

    if(eSIR_ED_NONE != edType)
    {
        palZeroMemory(pMac->hHdd, &setKeyEvent, sizeof(vos_event_wlan_security_payload_type));
        if( *(( tANI_U8 *)&pCommand->u.setKeyCmd.peerMac) & 0x01 )
        {
            setKeyEvent.eventId = WLAN_SECURITY_EVENT_SET_GTK_REQ;
            setKeyEvent.encryptionModeMulticast = (v_U8_t)diagEncTypeFromCSRType(pCommand->u.setKeyCmd.encType);
            setKeyEvent.encryptionModeUnicast = (v_U8_t)diagEncTypeFromCSRType(pSession->connectedProfile.EncryptionType);
        }
        else
        {
            setKeyEvent.eventId = WLAN_SECURITY_EVENT_SET_PTK_REQ;
            setKeyEvent.encryptionModeUnicast = (v_U8_t)diagEncTypeFromCSRType(pCommand->u.setKeyCmd.encType);
            setKeyEvent.encryptionModeMulticast = (v_U8_t)diagEncTypeFromCSRType(pSession->connectedProfile.mcEncryptionType);
        }
        palCopyMemory( pMac->hHdd, setKeyEvent.bssid, pSession->connectedProfile.bssid, 6 );
        if(CSR_IS_ENC_TYPE_STATIC(pCommand->u.setKeyCmd.encType))
        {
            tANI_U32 defKeyId;
            //                            
            if(HAL_STATUS_SUCCESS(ccmCfgGetInt(pMac, WNI_CFG_WEP_DEFAULT_KEYID, &defKeyId)))
            {
                setKeyEvent.keyId = (v_U8_t)defKeyId;
            }
        }
        else
        {
            setKeyEvent.keyId = pCommand->u.setKeyCmd.keyId;
        }
        setKeyEvent.authMode = (v_U8_t)diagAuthTypeFromCSRType(pSession->connectedProfile.AuthType);
        WLAN_VOS_DIAG_EVENT_REPORT(&setKeyEvent, EVENT_WLAN_SECURITY);
    }
#endif //                             
    if( csrIsSetKeyAllowed(pMac, sessionId) )
    {
        status = csrSendMBSetContextReqMsg( pMac, sessionId, 
                    ( tANI_U8 *)&pCommand->u.setKeyCmd.peerMac, 
                                            numKeys, edType, fUnicast, pCommand->u.setKeyCmd.keyDirection, 
                                            pCommand->u.setKeyCmd.keyId, pCommand->u.setKeyCmd.keyLength, 
                    pCommand->u.setKeyCmd.Key, pCommand->u.setKeyCmd.paeRole, 
                    pCommand->u.setKeyCmd.keyRsc);
    }
    else
    {
        smsLog( pMac, LOGW, FL(" cannot process not connected") );
        //                                                            
        status = eHAL_STATUS_CSR_WRONG_STATE;
    }
    if( !HAL_STATUS_SUCCESS(status) )
    {
        smsLog( pMac, LOGE, FL("  error status %d"), status );
        csrRoamCallCallback( pMac, sessionId, NULL, pCommand->u.setKeyCmd.roamId, eCSR_ROAM_SET_KEY_COMPLETE, eCSR_ROAM_RESULT_FAILURE);
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
        if(eSIR_ED_NONE != edType)
        {
            if( *(( tANI_U8 *)&pCommand->u.setKeyCmd.peerMac) & 0x01 )
            {
                setKeyEvent.eventId = WLAN_SECURITY_EVENT_SET_GTK_RSP;
            }
            else
            {
                setKeyEvent.eventId = WLAN_SECURITY_EVENT_SET_PTK_RSP;
            }
            setKeyEvent.status = WLAN_SECURITY_STATUS_FAILURE;
            WLAN_VOS_DIAG_EVENT_REPORT(&setKeyEvent, EVENT_WLAN_SECURITY);
        }
#endif //                             
    }
    return ( status );
}

eHalStatus csrRoamProcessRemoveKeyCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
    eHalStatus status;
    tpSirSmeRemoveKeyReq pMsg = NULL;
    tANI_U16 wMsgLen = sizeof(tSirSmeRemoveKeyReq);
    tANI_U8 *p;
    tANI_U32 sessionId = pCommand->sessionId;
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    WLAN_VOS_DIAG_EVENT_DEF(removeKeyEvent, vos_event_wlan_security_payload_type);
    if(NULL == pSession){
       smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
       return eHAL_STATUS_FAILURE;
    }
    palZeroMemory(pMac->hHdd, &removeKeyEvent, sizeof(vos_event_wlan_security_payload_type));
    removeKeyEvent.eventId = WLAN_SECURITY_EVENT_REMOVE_KEY_REQ;
    removeKeyEvent.encryptionModeMulticast = (v_U8_t)diagEncTypeFromCSRType(pSession->connectedProfile.mcEncryptionType);
    removeKeyEvent.encryptionModeUnicast = (v_U8_t)diagEncTypeFromCSRType(pSession->connectedProfile.EncryptionType);
    palCopyMemory( pMac->hHdd, removeKeyEvent.bssid, pSession->connectedProfile.bssid, 6 );
    removeKeyEvent.keyId = pCommand->u.removeKeyCmd.keyId;
    removeKeyEvent.authMode = (v_U8_t)diagAuthTypeFromCSRType(pSession->connectedProfile.AuthType);
    WLAN_VOS_DIAG_EVENT_REPORT(&removeKeyEvent, EVENT_WLAN_SECURITY);
#endif //                             
    if( csrIsSetKeyAllowed(pMac, sessionId) )
    {
        status = palAllocateMemory( pMac->hHdd, (void **)&pMsg, wMsgLen );
    }
    else
    {
        smsLog( pMac, LOGW, FL(" wrong state not allowed to set key") );
        //                                                     
        status = eHAL_STATUS_CSR_WRONG_STATE;
    }
    if( HAL_STATUS_SUCCESS( status ) )
    {
        palZeroMemory(pMac->hHdd, pMsg, wMsgLen);
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_REMOVEKEY_REQ);
                pMsg->length = pal_cpu_to_be16(wMsgLen);
        pMsg->sessionId = (tANI_U8)sessionId;
        pMsg->transactionId = 0;
        p = (tANI_U8 *)pMsg + sizeof(pMsg->messageType) + sizeof(pMsg->length) +
            sizeof(pMsg->sessionId) + sizeof(pMsg->transactionId);
        //                               
        palCopyMemory( pMac->hHdd, p, &pMac->roam.roamSession[sessionId].connectedProfile.bssid, sizeof(tSirMacAddr) );
        p += sizeof(tSirMacAddr);
        //            
        palCopyMemory( pMac->hHdd, p, pCommand->u.removeKeyCmd.peerMac, sizeof(tSirMacAddr) );
        p += sizeof(tSirMacAddr);
        //       
        *p = (tANI_U8)csrTranslateEncryptTypeToEdType( pCommand->u.removeKeyCmd.encType );
        p++;
        //        
        if( ( eCSR_ENCRYPT_TYPE_WEP40_STATICKEY == pCommand->u.removeKeyCmd.encType ) || 
            ( eCSR_ENCRYPT_TYPE_WEP104_STATICKEY == pCommand->u.removeKeyCmd.encType ) )
        {
            *p = (tANI_U8)eSIR_WEP_STATIC;
        }
        else
        {
            *p = (tANI_U8)eSIR_WEP_DYNAMIC;
        }
        p++;
        //     
        *p = pCommand->u.removeKeyCmd.keyId;
        p++;
        *p = (pCommand->u.removeKeyCmd.peerMac[0] == 0xFF ) ? 0 : 1;
        status = palSendMBMessage(pMac->hHdd, pMsg);
    }
    if( !HAL_STATUS_SUCCESS( status ) )
    {
        smsLog( pMac, LOGE, FL(" error status "), status );
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
        removeKeyEvent.eventId = WLAN_SECURITY_EVENT_REMOVE_KEY_RSP;
        removeKeyEvent.status = WLAN_SECURITY_STATUS_FAILURE;
        WLAN_VOS_DIAG_EVENT_REPORT(&removeKeyEvent, EVENT_WLAN_SECURITY);
#endif //                             
        csrRoamCallCallback( pMac, sessionId, NULL, pCommand->u.removeKeyCmd.roamId, eCSR_ROAM_REMOVE_KEY_COMPLETE, eCSR_ROAM_RESULT_FAILURE);
    }
    return ( status );
}

eHalStatus csrRoamSetKey( tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamSetKey *pSetKey, tANI_U32 roamId )
{
    eHalStatus status;
 
    if( !csrIsSetKeyAllowed(pMac, sessionId) )
    {
        status = eHAL_STATUS_CSR_WRONG_STATE;
    }
    else
    {
        status = csrRoamIssueSetKeyCommand( pMac, sessionId, pSetKey, roamId );
    }
    return ( status );
}

/*
                                                                   
                                                                 
                                               
*/
eHalStatus csrRoamPrepareFilterFromProfile(tpAniSirGlobal pMac, tCsrRoamProfile *pProfile, 
                                           tCsrScanResultFilter *pScanFilter)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_U32 size = 0;
    tANI_U8  index = 0;
    
    do
    {
        if(pProfile->BSSIDs.numOfBSSIDs)
        {
            size = sizeof(tCsrBssid) * pProfile->BSSIDs.numOfBSSIDs;
            status = palAllocateMemory(pMac->hHdd, (void **)&pScanFilter->BSSIDs.bssid, size);
            if(!HAL_STATUS_SUCCESS(status))
            {
                break;
            }
            pScanFilter->BSSIDs.numOfBSSIDs = pProfile->BSSIDs.numOfBSSIDs;
            palCopyMemory(pMac->hHdd, pScanFilter->BSSIDs.bssid, pProfile->BSSIDs.bssid, size);
        }
        if(pProfile->SSIDs.numOfSSIDs)
        {
            if( !CSR_IS_WDS_STA( pProfile ) )
            {
                pScanFilter->SSIDs.numOfSSIDs = pProfile->SSIDs.numOfSSIDs;
            }
            else
            {
                //               
                //                                                                                 
                pScanFilter->SSIDs.numOfSSIDs = 1;
            }
            size = sizeof(tCsrSSIDInfo) * pProfile->SSIDs.numOfSSIDs;
            status = palAllocateMemory(pMac->hHdd, (void **)&pScanFilter->SSIDs.SSIDList, size);
            if(!HAL_STATUS_SUCCESS(status))
            {
                break;
            }
            palCopyMemory(pMac->hHdd, pScanFilter->SSIDs.SSIDList, pProfile->SSIDs.SSIDList, size);
        }
        if(!pProfile->ChannelInfo.ChannelList || (pProfile->ChannelInfo.ChannelList[0] == 0) )
        {
            pScanFilter->ChannelInfo.numOfChannels = 0;
            pScanFilter->ChannelInfo.ChannelList = NULL;
        }
        else if(pProfile->ChannelInfo.numOfChannels)
        {
           status = palAllocateMemory(pMac->hHdd, (void **)&pScanFilter->ChannelInfo.ChannelList, sizeof(*pScanFilter->ChannelInfo.ChannelList) * pProfile->ChannelInfo.numOfChannels);
           pScanFilter->ChannelInfo.numOfChannels = 0;
            if(HAL_STATUS_SUCCESS(status))
            {
              for(index = 0; index < pProfile->ChannelInfo.numOfChannels; index++)
              {
                 if(csrRoamIsChannelValid(pMac, pProfile->ChannelInfo.ChannelList[index]))
                 {
                    pScanFilter->ChannelInfo.ChannelList[pScanFilter->ChannelInfo.numOfChannels] 
                       = pProfile->ChannelInfo.ChannelList[index];
                    pScanFilter->ChannelInfo.numOfChannels++;
                 }
                 else 
                 {
                         smsLog(pMac, LOG1, FL("process a channel (%d) that is invalid"), pProfile->ChannelInfo.ChannelList[index]);
                 }
            }
            }
            else
            {
                break;
            }
        }
        else 
        {
            smsLog(pMac, LOGW, FL("Channel list empty"));
            status = eHAL_STATUS_FAILURE;
            break;
        }
        pScanFilter->uapsd_mask = pProfile->uapsd_mask;
        pScanFilter->authType = pProfile->AuthType;
        pScanFilter->EncryptionType = pProfile->EncryptionType;
        pScanFilter->mcEncryptionType = pProfile->mcEncryptionType;
        pScanFilter->BSSType = pProfile->BSSType;
        pScanFilter->phyMode = pProfile->phyMode;
#ifdef FEATURE_WLAN_WAPI
        //                                                                       
        //                  
        if(csrIsProfileWapi(pProfile))
        {
             if(pScanFilter->phyMode & eCSR_DOT11_MODE_11n)
             {
                pScanFilter->phyMode &= ~eCSR_DOT11_MODE_11n;
             }
             if(pScanFilter->phyMode & eCSR_DOT11_MODE_AUTO)
             {
                pScanFilter->phyMode &= ~eCSR_DOT11_MODE_AUTO;
             }
             if(!pScanFilter->phyMode)
             {
                 pScanFilter->phyMode = eCSR_DOT11_MODE_11g;
             }
        }
#endif /*                   */
        /*                 */
        pScanFilter->bWPSAssociation = pProfile->bWPSAssociation;
        if( pProfile->countryCode[0] )
        {
            //                                                                            
            palCopyMemory( pMac->hHdd, pScanFilter->countryCode, pProfile->countryCode, 
                        WNI_CFG_COUNTRY_CODE_LEN );
        }
#ifdef WLAN_FEATURE_VOWIFI_11R
        if (pProfile->MDID.mdiePresent)
        {
            pScanFilter->MDID.mdiePresent = 1;
            pScanFilter->MDID.mobilityDomain = pProfile->MDID.mobilityDomain;
        }
#endif
    
    }while(0);
    
    if(!HAL_STATUS_SUCCESS(status))
    {
        csrFreeScanFilter(pMac, pScanFilter);
    }
    
    return(status);
}

tANI_BOOLEAN csrRoamIssueWmStatusChange( tpAniSirGlobal pMac, tANI_U32 sessionId,
                                         eCsrRoamWmStatusChangeTypes Type, tSirSmeRsp *pSmeRsp )
{
    tANI_BOOLEAN fCommandQueued = eANI_BOOLEAN_FALSE;
    tSmeCmd *pCommand;
    do
    {
        //                           
        if ( ( eCsrDisassociated != Type ) && ( eCsrDeauthenticated != Type ) ) break;
        pCommand = csrGetCommandBuffer( pMac );
        if ( !pCommand )
        {
            smsLog( pMac, LOGE, FL(" fail to get command buffer") );
            break;
        }
        //                                                 
        if( CSR_IS_WAIT_FOR_KEY( pMac, sessionId ) )
        {
            csrRoamStopWaitForKeyTimer( pMac );
            csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_NONE, sessionId);
        }
        pCommand->command = eSmeCommandWmStatusChange;
        pCommand->sessionId = (tANI_U8)sessionId;
        pCommand->u.wmStatusChangeCmd.Type = Type;
        if ( eCsrDisassociated ==  Type )
        {
            palCopyMemory( pMac->hHdd, &pCommand->u.wmStatusChangeCmd.u.DisassocIndMsg, pSmeRsp, 
                                sizeof( pCommand->u.wmStatusChangeCmd.u.DisassocIndMsg ) );
        }
        else
        {
            palCopyMemory( pMac->hHdd, &pCommand->u.wmStatusChangeCmd.u.DeauthIndMsg, pSmeRsp, 
                            sizeof( pCommand->u.wmStatusChangeCmd.u.DeauthIndMsg ) );
        }
        if( HAL_STATUS_SUCCESS( csrQueueSmeCommand(pMac, pCommand, eANI_BOOLEAN_TRUE) ) )
        {
            fCommandQueued = eANI_BOOLEAN_TRUE;
        }
        else
        {
            smsLog( pMac, LOGE, FL(" fail to send message ") );
            csrReleaseCommandWmStatusChange( pMac, pCommand );
        }

        /*                                                                               */
        csrSetDefaultDot11Mode( pMac );
    } while( 0 );
    return( fCommandQueued );
}

static void csrUpdateRssi(tpAniSirGlobal pMac, void* pMsg)
{
    v_S7_t  rssi = 0;
    tAniGetRssiReq *pGetRssiReq = (tAniGetRssiReq*)pMsg;
    if(pGetRssiReq)
    {
        if(NULL != pGetRssiReq->pVosContext)
        {
            WLANTL_GetRssi(pGetRssiReq->pVosContext, pGetRssiReq->staId, &rssi);
        }
        else
        {
            smsLog( pMac, LOGE, FL("pGetRssiReq->pVosContext is NULL"));
            return;
        }
            
        if(NULL != pGetRssiReq->rssiCallback)
        {
            ((tCsrRssiCallback)(pGetRssiReq->rssiCallback))(rssi, pGetRssiReq->staId, pGetRssiReq->pDevContext);
        }
        else
        {
            smsLog( pMac, LOGE, FL("pGetRssiReq->rssiCallback is NULL"));
            return;
        }
    }
    else
    {
        smsLog( pMac, LOGE, FL("pGetRssiReq is NULL"));
    }
    return;
}

static void csrUpdateSnr(tpAniSirGlobal pMac, void* pMsg)
{
    tANI_S8  snr = 0;
    tAniGetSnrReq *pGetSnrReq = (tAniGetSnrReq*)pMsg;

    if (pGetSnrReq)
    {
        if (VOS_STATUS_SUCCESS !=
            WDA_GetSnr(pGetSnrReq->staId, &snr))
        {
            smsLog(pMac, LOGE, FL("Error in WLANTL_GetSnr"));
            return;
        }

        if (pGetSnrReq->snrCallback)
        {
            ((tCsrSnrCallback)(pGetSnrReq->snrCallback))(snr, pGetSnrReq->staId,
                                                       pGetSnrReq->pDevContext);
        }
        else
        {
            smsLog(pMac, LOGE, FL("pGetSnrReq->snrCallback is NULL"));
            return;
        }
    }
    else
    {
        smsLog(pMac, LOGE, FL("pGetSnrReq is NULL"));
    }
    return;
}
#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)
void csrRoamRssiRspProcessor(tpAniSirGlobal pMac, void* pMsg)
{
    tAniGetRoamRssiRsp* pRoamRssiRsp = (tAniGetRoamRssiRsp*)pMsg;

    if (NULL != pRoamRssiRsp)
    {
        /*                                                                    
                                                         */
        tpAniGetRssiReq reqBkp = (tAniGetRssiReq*)pRoamRssiRsp->rssiReq;
        v_S7_t rssi = pRoamRssiRsp->rssi;

        if ((NULL != reqBkp) && (NULL != reqBkp->rssiCallback))
        {
            ((tCsrRssiCallback)(reqBkp->rssiCallback))(rssi, pRoamRssiRsp->staId, reqBkp->pDevContext);
            reqBkp->rssiCallback = NULL;
            vos_mem_free(reqBkp);
            pRoamRssiRsp->rssiReq = NULL;
        }
        else
        {
            smsLog( pMac, LOGE, FL("reqBkp->rssiCallback is NULL"));
            if (NULL != reqBkp)
            {
                vos_mem_free(reqBkp);
                pRoamRssiRsp->rssiReq = NULL;
            }
        }
    }
    else
    {
        smsLog( pMac, LOGE, FL("pRoamRssiRsp is NULL"));
    }
    return;
}
#endif


#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
void csrTsmStatsRspProcessor(tpAniSirGlobal pMac, void* pMsg)
{
    tAniGetTsmStatsRsp* pTsmStatsRsp = (tAniGetTsmStatsRsp*)pMsg;

    if (NULL != pTsmStatsRsp)
    {
        /*                                                                    
                                                                */
        tpAniGetTsmStatsReq reqBkp = (tAniGetTsmStatsReq*)pTsmStatsRsp->tsmStatsReq;

        if (NULL != reqBkp)
        {
            if (NULL != reqBkp->tsmStatsCallback)
            {
                ((tCsrTsmStatsCallback)(reqBkp->tsmStatsCallback))(pTsmStatsRsp->tsmMetrics,
                                 pTsmStatsRsp->staId, reqBkp->pDevContext);
                reqBkp->tsmStatsCallback = NULL;
            }
            vos_mem_free(reqBkp);
            pTsmStatsRsp->tsmStatsReq = NULL;
        }
        else
        {
            smsLog( pMac, LOGE, FL("reqBkp is NULL"));
            if (NULL != reqBkp)
            {
                vos_mem_free(reqBkp);
                pTsmStatsRsp->tsmStatsReq = NULL;
            }
        }
    }
    else
    {
        smsLog( pMac, LOGE, FL("pTsmStatsRsp is NULL"));
    }
    return;
}

void csrSendCcxAdjacentApRepInd(tpAniSirGlobal pMac, tCsrRoamSession *pSession)
{
    tANI_U32 roamTS2 = 0;
    tCsrRoamInfo roamInfo;
    tpPESession pSessionEntry = NULL;
    tANI_U8 sessionId = CSR_SESSION_ID_INVALID;

    if (NULL == pSession)
    {
        smsLog(pMac, LOGE, FL("pSession is NULL"));
        return;
    }

    roamTS2 = vos_timer_get_system_time();
    roamInfo.tsmRoamDelay = roamTS2 - pSession->roamTS1;
    smsLog(pMac, LOG1, "Bssid("MAC_ADDRESS_STR") Roaming Delay(%u ms)",
           MAC_ADDR_ARRAY(pSession->connectedProfile.bssid),
           roamInfo.tsmRoamDelay);

    pSessionEntry = peFindSessionByBssid(pMac, pSession->connectedProfile.bssid, &sessionId);
    if (NULL == pSessionEntry)
    {
        smsLog(pMac, LOGE, FL("session %d not found"), sessionId);
        return;
    }
    pSessionEntry->ccxContext.tsm.tsmMetrics.RoamingDly = roamInfo.tsmRoamDelay;
    csrRoamCallCallback(pMac, pSession->sessionId, &roamInfo,
                        0, eCSR_ROAM_CCX_ADJ_AP_REPORT_IND, 0);
}
#endif /*                                             */

static void csrRoamRssiIndHdlr(tpAniSirGlobal pMac, void* pMsg)
{
    WLANTL_TlIndicationReq *pTlRssiInd = (WLANTL_TlIndicationReq*)pMsg;
    if(pTlRssiInd)
    {
        if(NULL != pTlRssiInd->tlCallback)
        {
            ((WLANTL_RSSICrossThresholdCBType)(pTlRssiInd->tlCallback))
            (pTlRssiInd->pAdapter, pTlRssiInd->rssiNotification, pTlRssiInd->pUserCtxt, pTlRssiInd->avgRssi);
        }
        else
        {
            smsLog( pMac, LOGE, FL("pTlRssiInd->tlCallback is NULL"));
        }
    }
    else
    {
        smsLog( pMac, LOGE, FL("pTlRssiInd is NULL"));
    }
    return;
}

eHalStatus csrSendResetApCapsChanged(tpAniSirGlobal pMac, tSirMacAddr *bssId)
{
    tpSirResetAPCapsChange pMsg;
    tANI_U16 len;
    eHalStatus status   = eHAL_STATUS_SUCCESS;

    /*                                    */
    len = sizeof(tSirResetAPCapsChange);
    status = palAllocateMemory( pMac->hHdd, (void **)&pMsg, len );
    if (HAL_STATUS_SUCCESS(status))
    {
        palZeroMemory(pMac->hHdd, pMsg, sizeof(tSirResetAPCapsChange) );
        pMsg->messageType     = eWNI_SME_RESET_AP_CAPS_CHANGED;
        pMsg->length          = len;
        palCopyMemory( pMac->hHdd, pMsg->bssId, bssId, sizeof(tSirMacAddr) );
        smsLog( pMac, LOG1, FL("CSR reset caps change for Bssid= "MAC_ADDRESS_STR),
                MAC_ADDR_ARRAY(pMsg->bssId));
        status = palSendMBMessage(pMac->hHdd, pMsg);
    }
    else
    {
        smsLog( pMac, LOGE, FL("Memory allocation failed\n"));
    }
    return status;
}

void csrRoamCheckForLinkStatusChange( tpAniSirGlobal pMac, tSirSmeRsp *pSirMsg )
{
    tSirSmeAssocInd *pAssocInd;
    tSirSmeDisassocInd *pDisassocInd;
    tSirSmeDeauthInd *pDeauthInd;
    tSirSmeWmStatusChangeNtf *pStatusChangeMsg;
    tSirSmeNewBssInfo *pNewBss;
    tSmeIbssPeerInd *pIbssPeerInd;
    tSirMacAddr Broadcastaddr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    tSirSmeApNewCaps *pApNewCaps;
    eCsrRoamResult result = eCSR_ROAM_RESULT_NONE;
    eRoamCmdStatus roamStatus = eCSR_ROAM_FAILED;
    tCsrRoamInfo *pRoamInfo = NULL;
    tCsrRoamInfo roamInfo;
    eHalStatus status;
    tANI_U32 sessionId = CSR_SESSION_ID_INVALID;
    tCsrRoamSession *pSession = NULL;
    tpSirSmeSwitchChannelInd pSwitchChnInd;
    tSmeMaxAssocInd *pSmeMaxAssocInd;
    tSmeCmd pCommand;
    pSirMsg->messageType = (pSirMsg->messageType);
    pSirMsg->length = (pSirMsg->length);
    pSirMsg->statusCode = (pSirMsg->statusCode);
    palZeroMemory(pMac->hHdd, &roamInfo, sizeof(roamInfo));
    switch( pSirMsg->messageType ) 
    {
        case eWNI_SME_ASSOC_IND:
            {
                tCsrRoamSession  *pSession;
                smsLog( pMac, LOG1, FL("ASSOCIATION Indication from SME"));
                pAssocInd = (tSirSmeAssocInd *)pSirMsg;
                status = csrRoamGetSessionIdFromBSSID( pMac, (tCsrBssid *)pAssocInd->bssId, &sessionId );
                if( HAL_STATUS_SUCCESS( status ) )
                {
                    pSession = CSR_GET_SESSION(pMac, sessionId);

                    if(!pSession)
                    {
                        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                        return;
                    }

                pRoamInfo = &roamInfo;

                //                                                  
                pRoamInfo->assocReqLength = pAssocInd->assocReqLength;
                pRoamInfo->assocReqPtr = pAssocInd->assocReqPtr;

                pRoamInfo->beaconPtr = pAssocInd->beaconPtr;
                pRoamInfo->beaconLength = pAssocInd->beaconLength;                
                pRoamInfo->statusCode = eSIR_SME_SUCCESS; //                                
                pRoamInfo->u.pConnectedProfile = &pSession->connectedProfile;

                    pRoamInfo->staId = (tANI_U8)pAssocInd->staId;
                    pRoamInfo->rsnIELen = (tANI_U8)pAssocInd->rsnIE.length;
                    pRoamInfo->prsnIE = pAssocInd->rsnIE.rsnIEdata;
                
                pRoamInfo->addIELen = (tANI_U8)pAssocInd->addIE.length;
                pRoamInfo->paddIE =  pAssocInd->addIE.addIEdata;
                    palCopyMemory(pMac->hHdd, pRoamInfo->peerMac, pAssocInd->peerMacAddr, sizeof(tSirMacAddr));
                    palCopyMemory(pMac->hHdd, &pRoamInfo->bssid, pAssocInd->bssId, sizeof(tCsrBssid));
                    pRoamInfo->wmmEnabledSta = pAssocInd->wmmEnabledSta;
                    if(CSR_IS_WDS_AP( pRoamInfo->u.pConnectedProfile))
                        status = csrRoamCallCallback(pMac, sessionId, pRoamInfo, 0, eCSR_ROAM_WDS_IND, eCSR_ROAM_RESULT_WDS_ASSOCIATION_IND);//   
                    if(CSR_IS_INFRA_AP(pRoamInfo->u.pConnectedProfile))
                    {
                        if( CSR_IS_ENC_TYPE_STATIC( pSession->pCurRoamProfile->negotiatedUCEncryptionType ))
                        {
                            csrRoamIssueSetContextReq( pMac, sessionId, pSession->pCurRoamProfile->negotiatedUCEncryptionType, 
                                    pSession->pConnectBssDesc,
                                    &(pRoamInfo->peerMac),
                                    FALSE, TRUE, eSIR_TX_RX, 0, 0, NULL, 0 ); //                                              
                            pRoamInfo->fAuthRequired = FALSE;
                        }
                        else
                        {
                            pRoamInfo->fAuthRequired = TRUE;
                        }
                        status = csrRoamCallCallback(pMac, sessionId, pRoamInfo, 0, eCSR_ROAM_INFRA_IND, eCSR_ROAM_RESULT_INFRA_ASSOCIATION_IND);
                        if (!HAL_STATUS_SUCCESS(status))
                            pRoamInfo->statusCode = eSIR_SME_ASSOC_REFUSED;//                              
                    }
                    /*                                           */
                    status = csrSendAssocCnfMsg( pMac, pAssocInd, status );//   
                    
                    /*                                                                  
                                                       */
                    if(CSR_IS_WDS_AP( pRoamInfo->u.pConnectedProfile))
                {
                    status = csrSendAssocIndToUpperLayerCnfMsg(pMac, pAssocInd, status, sessionId);
                }
                else if(CSR_IS_INFRA_AP(pRoamInfo->u.pConnectedProfile) && (pRoamInfo->statusCode != eSIR_SME_ASSOC_REFUSED))
                {
                    pRoamInfo->fReassocReq = pAssocInd->reassocReq;
                    //                                                                                                                         
                    status = csrSendAssocIndToUpperLayerCnfMsg(pMac, pAssocInd, status, sessionId);
                }
                }
            }
            break;
        case eWNI_SME_DISASSOC_IND:
            //                                                             
            //                                                              
            //                                                    
            pDisassocInd = (tSirSmeDisassocInd *)pSirMsg;
            status = csrRoamGetSessionIdFromBSSID( pMac, (tCsrBssid *)pDisassocInd->bssId, &sessionId );
            if( HAL_STATUS_SUCCESS( status ) )
            {
                smsLog( pMac, LOGE, FL("DISASSOCIATION Indication from MAC for session %d "), sessionId);
                smsLog( pMac, LOGE, FL("DISASSOCIATION from peer =" MAC_ADDRESS_STR " "
                                       " reason = %d status = %d "),
                                       MAC_ADDR_ARRAY(pDisassocInd->peerMacAddr),
                                       pDisassocInd->reasonCode, pDisassocInd->statusCode);
                //                                                           
                //                                                         
                //                                                   
                //                                                  
#ifdef WLAN_FEATURE_VOWIFI_11R
                if (csrRoamIs11rAssoc(pMac) && (csrNeighborRoamStatePreauthDone(pMac)))
                {
                    csrNeighborRoamTranistionPreauthDoneToDisconnected(pMac);
                }
#endif
#ifdef FEATURE_WLAN_CCX
                if (csrRoamIsCCXAssoc(pMac) && (csrNeighborRoamStatePreauthDone(pMac)))
                {
                    csrNeighborRoamTranistionPreauthDoneToDisconnected(pMac);
                }
#endif
#ifdef FEATURE_WLAN_LFR
                if (csrRoamIsFastRoamEnabled(pMac, sessionId) && (csrNeighborRoamStatePreauthDone(pMac)))
                {
                    csrNeighborRoamTranistionPreauthDoneToDisconnected(pMac);
                }
#endif
                pSession = CSR_GET_SESSION( pMac, sessionId );

                if(!pSession)
                {
                    smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                    return;
                }

                if ( csrIsConnStateInfra( pMac, sessionId ) )
                {
                    pSession->connectState = eCSR_ASSOC_STATE_TYPE_NOT_CONNECTED;
                }
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
                sme_QosCsrEventInd(pMac, (v_U8_t)sessionId, SME_QOS_CSR_DISCONNECT_IND, NULL);
#endif
                csrRoamLinkDown(pMac, sessionId);
                csrRoamIssueWmStatusChange( pMac, sessionId, eCsrDisassociated, pSirMsg );
                if(CSR_IS_INFRA_AP(&pSession->connectedProfile))
                {

                    pRoamInfo = &roamInfo;

                    pRoamInfo->statusCode = pDisassocInd->statusCode; 
                    pRoamInfo->u.pConnectedProfile = &pSession->connectedProfile;

                    pRoamInfo->staId = (tANI_U8)pDisassocInd->staId;

                    palCopyMemory(pMac->hHdd, pRoamInfo->peerMac, pDisassocInd->peerMacAddr, sizeof(tSirMacAddr));
                    palCopyMemory(pMac->hHdd, &pRoamInfo->bssid, pDisassocInd->bssId, sizeof(tCsrBssid));

                    status = csrRoamCallCallback(pMac, sessionId, pRoamInfo, 0, eCSR_ROAM_INFRA_IND, eCSR_ROAM_RESULT_DISASSOC_IND);

                    /*
                                                                                      
                                                   
                    */
                    pCommand.command = eSmeCommandRoam;
                    pCommand.sessionId = (tANI_U8)sessionId;
                    pCommand.u.roamCmd.roamReason = eCsrForcedDeauthSta;
                    vos_mem_copy(pCommand.u.roamCmd.peerMac, pDisassocInd->peerMacAddr, sizeof(tSirMacAddr));
                    csrRoamRemoveDuplicateCommand(pMac, sessionId, &pCommand, eCsrForcedDeauthSta);
                }
            }
            else
            {
                smsLog(pMac, LOGE, FL(" Session Id not found for BSSID " MAC_ADDRESS_STR),
                                        MAC_ADDR_ARRAY(pDisassocInd->bssId));
            }
            break;
        case eWNI_SME_DEAUTH_IND:
            smsLog( pMac, LOG1, FL("DEAUTHENTICATION Indication from MAC"));
            pDeauthInd = (tpSirSmeDeauthInd)pSirMsg;
            status = csrRoamGetSessionIdFromBSSID( pMac, (tCsrBssid *)pDeauthInd->bssId, &sessionId );
            if( HAL_STATUS_SUCCESS( status ) )
            {
                //                                                           
                //                                                         
                //                                                   
                //                                                  
#ifdef WLAN_FEATURE_VOWIFI_11R
                if (csrRoamIs11rAssoc(pMac) && (csrNeighborRoamStatePreauthDone(pMac)))
                {
                    csrNeighborRoamTranistionPreauthDoneToDisconnected(pMac);
                }
#endif
#ifdef FEATURE_WLAN_CCX
                if (csrRoamIsCCXAssoc(pMac) && (csrNeighborRoamStatePreauthDone(pMac)))
                {
                    csrNeighborRoamTranistionPreauthDoneToDisconnected(pMac);
                }
#endif
#ifdef FEATURE_WLAN_LFR
                if (csrRoamIsFastRoamEnabled(pMac, sessionId) && (csrNeighborRoamStatePreauthDone(pMac)))
                {
                    csrNeighborRoamTranistionPreauthDoneToDisconnected(pMac);
                }
#endif
                pSession = CSR_GET_SESSION( pMac, sessionId );

                if(!pSession)
                {
                    smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                    return;
                }

                if ( csrIsConnStateInfra( pMac, sessionId ) )
                {
                    pSession->connectState = eCSR_ASSOC_STATE_TYPE_NOT_CONNECTED;
                }
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
                sme_QosCsrEventInd(pMac, (v_U8_t)sessionId, SME_QOS_CSR_DISCONNECT_IND, NULL);
#endif
                csrRoamLinkDown(pMac, sessionId);
                csrRoamIssueWmStatusChange( pMac, sessionId, eCsrDeauthenticated, pSirMsg );
                if(CSR_IS_INFRA_AP(&pSession->connectedProfile))
                {

                    pRoamInfo = &roamInfo;

                    pRoamInfo->statusCode = pDeauthInd->statusCode;
                    pRoamInfo->u.pConnectedProfile = &pSession->connectedProfile;

                    pRoamInfo->staId = (tANI_U8)pDeauthInd->staId;

                    palCopyMemory(pMac->hHdd, pRoamInfo->peerMac, pDeauthInd->peerMacAddr, sizeof(tSirMacAddr));
                    palCopyMemory(pMac->hHdd, &pRoamInfo->bssid, pDeauthInd->bssId, sizeof(tCsrBssid));

                    status = csrRoamCallCallback(pMac, sessionId, pRoamInfo, 0, eCSR_ROAM_INFRA_IND, eCSR_ROAM_RESULT_DEAUTH_IND);
                }
            }
            break;
        
        case eWNI_SME_SWITCH_CHL_REQ:        //                                                          
            smsLog( pMac, LOGW, FL("eWNI_SME_SWITCH_CHL_REQ from SME"));
            pSwitchChnInd = (tpSirSmeSwitchChannelInd)pSirMsg;
            //                               
            //                                           
            status = csrRoamGetSessionIdFromBSSID( pMac, (tCsrBssid *)pSwitchChnInd->bssId, &sessionId );
            if( HAL_STATUS_SUCCESS( status ) )
            {
                pSession = CSR_GET_SESSION( pMac, sessionId );
                if(!pSession)
                {
                    smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                    return;
                }
                pSession->connectedProfile.operationChannel = (tANI_U8)pSwitchChnInd->newChannelId;
                if(pSession->pConnectBssDesc)
                {
                    pSession->pConnectBssDesc->channelId = (tANI_U8)pSwitchChnInd->newChannelId;
                }
            }
            break;
                
        case eWNI_SME_DEAUTH_RSP:
            smsLog( pMac, LOGW, FL("eWNI_SME_DEAUTH_RSP from SME"));
            {
                tSirSmeDeauthRsp* pDeauthRsp = (tSirSmeDeauthRsp *)pSirMsg;
                sessionId = pDeauthRsp->sessionId;
                if( CSR_IS_SESSION_VALID(pMac, sessionId) )
                {                    
                    pSession = CSR_GET_SESSION(pMac, sessionId);
                    if(!pSession)
                    {
                        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                        return;
                    }
                    if ( CSR_IS_INFRA_AP(&pSession->connectedProfile) )
                    {
                        pRoamInfo = &roamInfo;
                        pRoamInfo->u.pConnectedProfile = &pSession->connectedProfile;
                        palCopyMemory(pMac->hHdd, pRoamInfo->peerMac, pDeauthRsp->peerMacAddr, sizeof(tSirMacAddr));
                        pRoamInfo->reasonCode = eCSR_ROAM_RESULT_FORCED;
                        pRoamInfo->statusCode = pDeauthRsp->statusCode;
                        status = csrRoamCallCallback(pMac, sessionId, pRoamInfo, 0, eCSR_ROAM_LOSTLINK, eCSR_ROAM_RESULT_FORCED);
                    }
                }
            }
            break;
            
        case eWNI_SME_DISASSOC_RSP:
            /*                                                                                    */
            smsLog( pMac, LOGW, FL("eWNI_SME_DISASSOC_RSP from SME "));
            {
                tSirSmeDisassocRsp *pDisassocRsp = (tSirSmeDisassocRsp *)pSirMsg;
                sessionId = pDisassocRsp->sessionId;
                if( CSR_IS_SESSION_VALID(pMac, sessionId) )
                {                    
                    pSession = CSR_GET_SESSION(pMac, sessionId);
                    if(!pSession)
                    {
                        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                        return;
                    }
                    if ( CSR_IS_INFRA_AP(&pSession->connectedProfile) )
                    {
                        pRoamInfo = &roamInfo;
                        pRoamInfo->u.pConnectedProfile = &pSession->connectedProfile;
                        palCopyMemory(pMac->hHdd, pRoamInfo->peerMac, pDisassocRsp->peerMacAddr, sizeof(tSirMacAddr));
                        pRoamInfo->reasonCode = eCSR_ROAM_RESULT_FORCED;
                        pRoamInfo->statusCode = pDisassocRsp->statusCode;
                        status = csrRoamCallCallback(pMac, sessionId, pRoamInfo, 0, eCSR_ROAM_LOSTLINK, eCSR_ROAM_RESULT_FORCED);
                    }
                }
            }
            break;
        case eWNI_SME_MIC_FAILURE_IND:
            {
                tpSirSmeMicFailureInd pMicInd = (tpSirSmeMicFailureInd)pSirMsg;
                tCsrRoamInfo roamInfo, *pRoamInfo = NULL;
                eCsrRoamResult result = eCSR_ROAM_RESULT_MIC_ERROR_UNICAST;

                status = csrRoamGetSessionIdFromBSSID( pMac, (tCsrBssid *)pMicInd->bssId, &sessionId );
                if( HAL_STATUS_SUCCESS( status ) )
                {
                    palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
                    roamInfo.u.pMICFailureInfo = &pMicInd->info;
                    pRoamInfo = &roamInfo;
                    if(pMicInd->info.multicast)
                    {
                        result = eCSR_ROAM_RESULT_MIC_ERROR_GROUP;
                    }
                    else
                    {
                        result = eCSR_ROAM_RESULT_MIC_ERROR_UNICAST;
                    }
                    csrRoamCallCallback(pMac, sessionId, pRoamInfo, 0, eCSR_ROAM_MIC_ERROR_IND, result);
                }

#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
                {
                    WLAN_VOS_DIAG_EVENT_DEF(secEvent, vos_event_wlan_security_payload_type);
                    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
                    if(!pSession)
                    {
                        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                        return;
                    }

                    palZeroMemory(pMac->hHdd, &secEvent, sizeof(vos_event_wlan_security_payload_type));
                    secEvent.eventId = WLAN_SECURITY_EVENT_MIC_ERROR;
                    secEvent.encryptionModeMulticast = 
                        (v_U8_t)diagEncTypeFromCSRType(pSession->connectedProfile.mcEncryptionType);
                    secEvent.encryptionModeUnicast = 
                        (v_U8_t)diagEncTypeFromCSRType(pSession->connectedProfile.EncryptionType);
                    secEvent.authMode = 
                        (v_U8_t)diagAuthTypeFromCSRType(pSession->connectedProfile.AuthType);
                    palCopyMemory( pMac->hHdd, secEvent.bssid, pSession->connectedProfile.bssid, 6 );
                    WLAN_VOS_DIAG_EVENT_REPORT(&secEvent, EVENT_WLAN_SECURITY);
                }
#endif//                             

            }
            break;
        case eWNI_SME_WPS_PBC_PROBE_REQ_IND:
            {
                tpSirSmeProbeReqInd pProbeReqInd = (tpSirSmeProbeReqInd)pSirMsg;
                tCsrRoamInfo roamInfo;
                smsLog( pMac, LOG1, FL("WPS PBC Probe request Indication from SME"));
           
                status = csrRoamGetSessionIdFromBSSID( pMac, (tCsrBssid *)pProbeReqInd->bssId, &sessionId );
                if( HAL_STATUS_SUCCESS( status ) )
                {
                    palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
                    roamInfo.u.pWPSPBCProbeReq = &pProbeReqInd->WPSPBCProbeReq;
                    csrRoamCallCallback(pMac, sessionId, &roamInfo, 0, eCSR_ROAM_WPS_PBC_PROBE_REQ_IND, 
                        eCSR_ROAM_RESULT_WPS_PBC_PROBE_REQ_IND);
                }
            }
            break;        
            
        case eWNI_SME_WM_STATUS_CHANGE_NTF:
            pStatusChangeMsg = (tSirSmeWmStatusChangeNtf *)pSirMsg;
            switch( pStatusChangeMsg->statusChangeCode ) 
            {
                case eSIR_SME_IBSS_ACTIVE:
                    sessionId = csrFindIbssSession( pMac );
                    if( CSR_SESSION_ID_INVALID != sessionId )
                    {
                        pSession = CSR_GET_SESSION( pMac, sessionId );
                        if(!pSession)
                        {
                            smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                            return;
                        }
                        pSession->connectState = eCSR_ASSOC_STATE_TYPE_IBSS_CONNECTED;
                        if(pSession->pConnectBssDesc)
                        {
                            palCopyMemory(pMac->hHdd, &roamInfo.bssid, pSession->pConnectBssDesc->bssId, sizeof(tCsrBssid));
                            roamInfo.u.pConnectedProfile = &pSession->connectedProfile;
                            pRoamInfo = &roamInfo;
                        }
                        else
                        {
                            smsLog(pMac, LOGE, "  CSR eSIR_SME_IBSS_NEW_PEER connected BSS is empty");
                        }
                        result = eCSR_ROAM_RESULT_IBSS_CONNECT;
                        roamStatus = eCSR_ROAM_CONNECT_STATUS_UPDATE;
                    }
                    break;
                case eSIR_SME_IBSS_INACTIVE:
                    sessionId = csrFindIbssSession( pMac );
                    if( CSR_SESSION_ID_INVALID != sessionId )
                    {
                        pSession = CSR_GET_SESSION( pMac, sessionId );
                        if(!pSession)
                        {
                            smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                            return;
                        }
                        pSession->connectState = eCSR_ASSOC_STATE_TYPE_IBSS_DISCONNECTED;
                        result = eCSR_ROAM_RESULT_IBSS_INACTIVE;
                        roamStatus = eCSR_ROAM_CONNECT_STATUS_UPDATE;
                    }
                    break;
                case eSIR_SME_JOINED_NEW_BSS:    //                 
                    sessionId = csrFindIbssSession( pMac );
                    if( CSR_SESSION_ID_INVALID != sessionId )
                    {
                        pSession = CSR_GET_SESSION( pMac, sessionId );
                        if(!pSession)
                        {
                            smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                            return;
                        }
                        //                                        
                        pNewBss = &pStatusChangeMsg->statusChangeInfo.newBssInfo;
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
                        {
                            vos_log_ibss_pkt_type *pIbssLog;
                            tANI_U32 bi;
                            WLAN_VOS_DIAG_LOG_ALLOC(pIbssLog, vos_log_ibss_pkt_type, LOG_WLAN_IBSS_C);
                            if(pIbssLog)
                            {
                                pIbssLog->eventId = WLAN_IBSS_EVENT_COALESCING;
                                if(pNewBss)
                                {
                                    palCopyMemory(pMac->hHdd, pIbssLog->bssid, pNewBss->bssId, 6);
                                    if(pNewBss->ssId.length)
                                    {
                                        palCopyMemory(pMac->hHdd, pIbssLog->ssid, pNewBss->ssId.ssId, pNewBss->ssId.length);
                                    }
                                    pIbssLog->operatingChannel = pNewBss->channelNumber;
                                }
                                if(HAL_STATUS_SUCCESS(ccmCfgGetInt(pMac, WNI_CFG_BEACON_INTERVAL, &bi)))
                                {
                                    //                                       
                                    pIbssLog->beaconInterval = (v_U8_t)bi;
                                }
                                WLAN_VOS_DIAG_LOG_REPORT(pIbssLog);
                            }
                        }
#endif //                             
                        csrRoamUpdateConnectedProfileFromNewBss( pMac, sessionId, pNewBss );

                        if ((eCSR_ENCRYPT_TYPE_NONE ==
                                    pSession->connectedProfile.EncryptionType ))
                        {
                            csrRoamIssueSetContextReq( pMac, sessionId,
                                     pSession->connectedProfile.EncryptionType,
                                     pSession->pConnectBssDesc,
                                     &Broadcastaddr,
                                     FALSE, FALSE, eSIR_TX_RX, 0, 0, NULL, 0 );
                        }
                        result = eCSR_ROAM_RESULT_IBSS_COALESCED;
                        roamStatus = eCSR_ROAM_IBSS_IND;
                        palCopyMemory(pMac->hHdd, &roamInfo.bssid, &pNewBss->bssId, sizeof(tCsrBssid));
                        pRoamInfo = &roamInfo;
                        //                                           
                        if(pSession->pConnectBssDesc)
                        {
                            palCopyMemory(pMac->hHdd, pSession->pConnectBssDesc->bssId, &pNewBss->bssId, sizeof(tCsrBssid));
                        }
                        //                                               
                        //                         
                        if(pSession->ibss_join_pending)
                        {
                           pSession->ibss_join_pending = FALSE;
                           csrRoamStopIbssJoinTimer(pMac, sessionId);
                           result = eCSR_ROAM_RESULT_IBSS_JOIN_SUCCESS;
                        }
                    }
                    smsLog(pMac, LOGW, "CSR:  eSIR_SME_JOINED_NEW_BSS received from PE");
                    break;
                //                                                                          
                case eSIR_SME_AP_CAPS_CHANGED:
                    pApNewCaps = &pStatusChangeMsg->statusChangeInfo.apNewCaps;
                    smsLog(pMac, LOGW, "CSR handling eSIR_SME_AP_CAPS_CHANGED");
                    status = csrRoamGetSessionIdFromBSSID( pMac, (tCsrBssid *)pApNewCaps->bssId, &sessionId );
                    if( HAL_STATUS_SUCCESS( status ) )
                    {
                        if ((eCSR_ROAMING_STATE_JOINED == pMac->roam.curState[sessionId]) &&
                                ((eCSR_ROAM_SUBSTATE_JOINED_REALTIME_TRAFFIC == pMac->roam.curSubState[sessionId]) ||
                                 (eCSR_ROAM_SUBSTATE_NONE == pMac->roam.curSubState[sessionId]) ||
                                 (eCSR_ROAM_SUBSTATE_JOINED_NON_REALTIME_TRAFFIC == pMac->roam.curSubState[sessionId]) ||
                                 (eCSR_ROAM_SUBSTATE_JOINED_NO_TRAFFIC == pMac->roam.curSubState[sessionId]))
                           )
                        {
                            smsLog(pMac, LOGW, "Calling csrRoamDisconnectInternal");
                            csrRoamDisconnectInternal(pMac, sessionId, eCSR_DISCONNECT_REASON_UNSPECIFIED);
                        }
                        else
                        {
                            smsLog(pMac, LOGW,
                                    "Skipping csrScanForCapabilityChange as "
                                    "CSR is in state %d and sub-state %d",
                                    pMac->roam.curState[sessionId],
                                    pMac->roam.curSubState[sessionId]);
                            /*                                                                       
                                                                                 
                                                                                                          
                                                                                       */
                            csrSendResetApCapsChanged(pMac, &pApNewCaps->bssId);
                        }
                    }
                    break;

                default:
                    roamStatus = eCSR_ROAM_FAILED;
                    result = eCSR_ROAM_RESULT_NONE;
                    break;
            }  //                               
            if(eCSR_ROAM_RESULT_NONE != result)
            {
                csrRoamCallCallback(pMac, sessionId, pRoamInfo, 0, roamStatus, result);
            }
            break;
        case eWNI_SME_IBSS_NEW_PEER_IND:
            pIbssPeerInd = (tSmeIbssPeerInd *)pSirMsg;
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
            {
                vos_log_ibss_pkt_type *pIbssLog;
                WLAN_VOS_DIAG_LOG_ALLOC(pIbssLog, vos_log_ibss_pkt_type, LOG_WLAN_IBSS_C);
                if(pIbssLog)
                {
                    pIbssLog->eventId = WLAN_IBSS_EVENT_PEER_JOIN;
                    palCopyMemory(pMac->hHdd, pIbssLog->peerMacAddr, &pIbssPeerInd->peerAddr, 6);
                    WLAN_VOS_DIAG_LOG_REPORT(pIbssLog);
                }
            }
#endif //                             
            sessionId = csrFindIbssSession( pMac );
            if( CSR_SESSION_ID_INVALID != sessionId )
            {
                pSession = CSR_GET_SESSION( pMac, sessionId );

                if(!pSession)
                {
                    smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                    return;
                }
            //                                                                                              
                if(pSession->pConnectBssDesc)
                {
                    palCopyMemory(pMac->hHdd, &roamInfo.peerMac, pIbssPeerInd->peerAddr, sizeof(tCsrBssid));
                    palCopyMemory(pMac->hHdd, &roamInfo.bssid, pSession->pConnectBssDesc->bssId, sizeof(tCsrBssid));
                    if(pIbssPeerInd->mesgLen > sizeof(tSmeIbssPeerInd))
                    {
                        status = palAllocateMemory(pMac->hHdd, (void **)&roamInfo.pbFrames, 
                                                (pIbssPeerInd->mesgLen - sizeof(tSmeIbssPeerInd)));
                        if(HAL_STATUS_SUCCESS(status))
                        {
                            roamInfo.nBeaconLength = (pIbssPeerInd->mesgLen - sizeof(tSmeIbssPeerInd));
                            palCopyMemory(pMac->hHdd, roamInfo.pbFrames, ((tANI_U8 *)pIbssPeerInd) + sizeof(tSmeIbssPeerInd),
                                                roamInfo.nBeaconLength);
                        }
                        roamInfo.staId = (tANI_U8)pIbssPeerInd->staId;
                        roamInfo.ucastSig = (tANI_U8)pIbssPeerInd->ucastSig;
                        roamInfo.bcastSig = (tANI_U8)pIbssPeerInd->bcastSig;
                        status = palAllocateMemory(pMac->hHdd, (void **)&roamInfo.pBssDesc, 
                                                pSession->pConnectBssDesc->length);
                        if(HAL_STATUS_SUCCESS(status))
                        {
                            palCopyMemory(pMac->hHdd, roamInfo.pBssDesc, pSession->pConnectBssDesc, 
                                                pSession->pConnectBssDesc->length);
                        }
                        if(HAL_STATUS_SUCCESS(status))
                        {
                            pRoamInfo = &roamInfo;
                        }
                        else
                        {
                            if(roamInfo.pbFrames)
                            {
                                palFreeMemory(pMac->hHdd, roamInfo.pbFrames);
                            }
                            if(roamInfo.pBssDesc)
                            {
                                palFreeMemory(pMac->hHdd, roamInfo.pBssDesc);
                            }
                        }
                    }
                    else
                    {
                        pRoamInfo = &roamInfo;
                    }
                    if ((eCSR_ENCRYPT_TYPE_NONE ==
                              pSession->connectedProfile.EncryptionType ))
                    {
                       csrRoamIssueSetContextReq( pMac, sessionId,
                                     pSession->connectedProfile.EncryptionType,
                                     pSession->pConnectBssDesc,
                                     &(pIbssPeerInd->peerAddr),
                                     FALSE, TRUE, eSIR_TX_RX, 0, 0, NULL, 0 ); //                                              
                    }
                }
                else
                {
                    smsLog(pMac, LOGW, "  CSR eSIR_SME_IBSS_NEW_PEER connected BSS is empty");
                }
                //                                     
                if (pRoamInfo)
                {
                    pRoamInfo->u.pConnectedProfile = &pSession->connectedProfile;
                }
                csrRoamCallCallback(pMac, sessionId, pRoamInfo, 0, 
                            eCSR_ROAM_CONNECT_STATUS_UPDATE, eCSR_ROAM_RESULT_IBSS_NEW_PEER);
                if(pRoamInfo)
                {
                    if(roamInfo.pbFrames)
                    {
                        palFreeMemory(pMac->hHdd, roamInfo.pbFrames);
                    }
                    if(roamInfo.pBssDesc)
                    {
                        palFreeMemory(pMac->hHdd, roamInfo.pBssDesc);
                    }
                }
            }
            break;
        case eWNI_SME_IBSS_PEER_DEPARTED_IND:
            pIbssPeerInd = (tSmeIbssPeerInd*)pSirMsg;
            sessionId = csrFindIbssSession( pMac );
            if( CSR_SESSION_ID_INVALID != sessionId )
            {
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
                {
                    vos_log_ibss_pkt_type *pIbssLog;
 
                    WLAN_VOS_DIAG_LOG_ALLOC(pIbssLog, vos_log_ibss_pkt_type, LOG_WLAN_IBSS_C);
                    if(pIbssLog)
                    {
                        pIbssLog->eventId = WLAN_IBSS_EVENT_PEER_LEAVE;
                        if(pIbssPeerInd)
                        {
                            palCopyMemory(pMac->hHdd, pIbssLog->peerMacAddr, &pIbssPeerInd->peerAddr, 6);
                        }
                        WLAN_VOS_DIAG_LOG_REPORT(pIbssLog);
                    }
                }
#endif //                             
                smsLog(pMac, LOGW, "CSR: Peer departed notification from LIM");
                if(pIbssPeerInd)
                {
                   roamInfo.staId = (tANI_U8)pIbssPeerInd->staId;
                   roamInfo.ucastSig = (tANI_U8)pIbssPeerInd->ucastSig;
                   roamInfo.bcastSig = (tANI_U8)pIbssPeerInd->bcastSig;
                   palCopyMemory(pMac->hHdd, &roamInfo.peerMac, pIbssPeerInd->peerAddr, sizeof(tCsrBssid));
                }else
                   smsLog(pMac, LOGE, "CSR: departed peer info is NULL");

                csrRoamCallCallback(pMac, sessionId, &roamInfo, 0, 
                        eCSR_ROAM_CONNECT_STATUS_UPDATE, eCSR_ROAM_RESULT_IBSS_PEER_DEPARTED);
            }
            break;
        case eWNI_SME_SETCONTEXT_RSP:
            {
                tSirSmeSetContextRsp *pRsp = (tSirSmeSetContextRsp *)pSirMsg;
                tListElem *pEntry;
                tSmeCmd *pCommand;
                
                pEntry = csrLLPeekHead( &pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK );
                if ( pEntry )
                {
                    pCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
                    if ( eSmeCommandSetKey == pCommand->command )
                    {                
                        sessionId = pCommand->sessionId;        
                        pSession = CSR_GET_SESSION( pMac, sessionId );

                        if(!pSession)
                        {
                            smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                            return;
                        }
       
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
                        if(eCSR_ENCRYPT_TYPE_NONE != pSession->connectedProfile.EncryptionType)
                        {
                            WLAN_VOS_DIAG_EVENT_DEF(setKeyEvent, vos_event_wlan_security_payload_type);
                            palZeroMemory(pMac->hHdd, &setKeyEvent, sizeof(vos_event_wlan_security_payload_type));
                            if( pRsp->peerMacAddr[0] & 0x01 )
                            {
                                setKeyEvent.eventId = WLAN_SECURITY_EVENT_SET_GTK_RSP;
                            }
                            else
                            {
                                setKeyEvent.eventId = WLAN_SECURITY_EVENT_SET_PTK_RSP;
                            }
                            setKeyEvent.encryptionModeMulticast = 
                                (v_U8_t)diagEncTypeFromCSRType(pSession->connectedProfile.mcEncryptionType);
                            setKeyEvent.encryptionModeUnicast = 
                                (v_U8_t)diagEncTypeFromCSRType(pSession->connectedProfile.EncryptionType);
                            palCopyMemory( pMac->hHdd, setKeyEvent.bssid, pSession->connectedProfile.bssid, 6 );
                            setKeyEvent.authMode = 
                                (v_U8_t)diagAuthTypeFromCSRType(pSession->connectedProfile.AuthType);
                            if( eSIR_SME_SUCCESS != pRsp->statusCode )
                            {
                                setKeyEvent.status = WLAN_SECURITY_STATUS_FAILURE;
                            }
                            WLAN_VOS_DIAG_EVENT_REPORT(&setKeyEvent, EVENT_WLAN_SECURITY);
                        }
#endif //                             
                        if( CSR_IS_WAIT_FOR_KEY( pMac, sessionId) )
                        {
                            csrRoamStopWaitForKeyTimer( pMac );

                            //                                                        
                            csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_NONE, sessionId);
                            //                                                                           
                            //                            
                            if( csrIsConnStateConnectedInfra(pMac, sessionId) ) 
                            {
                                csrRoamLinkUp(pMac, pSession->connectedProfile.bssid);
                            }
                        }
                        if( eSIR_SME_SUCCESS == pRsp->statusCode )
                        {
                            palCopyMemory( pMac, &roamInfo.peerMac, &pRsp->peerMacAddr, sizeof(tCsrBssid) );
                                //                                                                      
                                //                                                                     
                                if( palEqualMemory( pMac->hHdd, &Broadcastaddr, pRsp->peerMacAddr, 
                                           sizeof(tSirMacAddr) ) )
                                {
#ifdef WLAN_ACTIVEMODE_OFFLOAD_FEATURE
                                    if(IS_ACTIVEMODE_OFFLOAD_FEATURE_ENABLE)
                                    {
                                       tpSirSetActiveModeSetBncFilterReq pMsg;
                                       palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof(tSirSetActiveModeSetBncFilterReq));
                                       pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_SET_BCN_FILTER_REQ);
                                       pMsg->length = pal_cpu_to_be16(sizeof( tANI_U8));
                                       pMsg->seesionId = sessionId;
                                       status = palSendMBMessage(pMac->hHdd, pMsg ); 
                                    }
#endif
                                       result = eCSR_ROAM_RESULT_AUTHENTICATED;
                                }
                                else
                                {
                                    result = eCSR_ROAM_RESULT_NONE;
                                }
                            pRoamInfo = &roamInfo;
                        }
                        else
                        {
                            result = eCSR_ROAM_RESULT_FAILURE;
                            smsLog(pMac, LOGE, "CSR: Roam Completion setkey "
                                   "command failed(%d) PeerMac "MAC_ADDRESS_STR,
                                   pRsp->statusCode, MAC_ADDR_ARRAY(pRsp->peerMacAddr));
                        }
                        csrRoamCallCallback(pMac, sessionId, &roamInfo, pCommand->u.setKeyCmd.roamId, 
                                            eCSR_ROAM_SET_KEY_COMPLETE, result);
                        //                                                                
                        //                                                       
                        sme_QosCsrEventInd(pMac, (v_U8_t)sessionId, SME_QOS_CSR_SET_KEY_SUCCESS_IND, NULL);
#ifdef FEATURE_WLAN_CCX
                        //                                 
                        if (result == eCSR_ROAM_RESULT_AUTHENTICATED &&
                            pSession->isPrevApInfoValid && 
                            pSession->connectedProfile.isCCXAssoc)
                        {
#ifdef FEATURE_WLAN_CCX_UPLOAD
                            csrSendCcxAdjacentApRepInd(pMac, pSession);
#else
                            csrCcxSendAdjacentApRepMsg(pMac, pSession);
#endif
                            pSession->isPrevApInfoValid = FALSE;
                        }
#endif
                        if( csrLLRemoveEntry( &pMac->sme.smeCmdActiveList, pEntry, LL_ACCESS_LOCK ) )
                        {
                            csrReleaseCommandSetKey( pMac, pCommand );
                        }
                    }
                    else
                    {
                        smsLog( pMac, LOGE, "CSR: Roam Completion called but setkey command is not ACTIVE ..." );
                    }
                }
                else
                {
                    smsLog( pMac, LOGE, "CSR: SetKey Completion called but NO commands are ACTIVE ..." );
                }
                smeProcessPendingQueue( pMac );
            }
            break;
        case eWNI_SME_REMOVEKEY_RSP:
            {
                tSirSmeRemoveKeyRsp *pRsp = (tSirSmeRemoveKeyRsp *)pSirMsg;
                tListElem *pEntry;
                tSmeCmd *pCommand;
                
                pEntry = csrLLPeekHead( &pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK );
                if ( pEntry )
                {
                    pCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
                    if ( eSmeCommandRemoveKey == pCommand->command )
                    {                
                        sessionId = pCommand->sessionId;
                        pSession = CSR_GET_SESSION( pMac, sessionId );

                        if(!pSession)
                        {
                            smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
                            return;
                        }
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
                        {
                            WLAN_VOS_DIAG_EVENT_DEF(removeKeyEvent, vos_event_wlan_security_payload_type);
                            palZeroMemory(pMac->hHdd, &removeKeyEvent, sizeof(vos_event_wlan_security_payload_type));
                            removeKeyEvent.eventId = WLAN_SECURITY_EVENT_REMOVE_KEY_RSP;
                            removeKeyEvent.encryptionModeMulticast = 
                                (v_U8_t)diagEncTypeFromCSRType(pSession->connectedProfile.mcEncryptionType);
                            removeKeyEvent.encryptionModeUnicast = 
                                (v_U8_t)diagEncTypeFromCSRType(pSession->connectedProfile.EncryptionType);
                            palCopyMemory( pMac->hHdd, removeKeyEvent.bssid, pSession->connectedProfile.bssid, 6 );
                            removeKeyEvent.authMode = 
                                (v_U8_t)diagAuthTypeFromCSRType(pSession->connectedProfile.AuthType);
                            if( eSIR_SME_SUCCESS != pRsp->statusCode )
                            {
                                removeKeyEvent.status = WLAN_SECURITY_STATUS_FAILURE;
                            }
                            WLAN_VOS_DIAG_EVENT_REPORT(&removeKeyEvent, EVENT_WLAN_SECURITY);
                        }
#endif //                             
                        if( eSIR_SME_SUCCESS == pRsp->statusCode )
                        {
                            palCopyMemory( pMac, &roamInfo.peerMac, &pRsp->peerMacAddr, sizeof(tCsrBssid) );
                            result = eCSR_ROAM_RESULT_NONE;
                            pRoamInfo = &roamInfo;
                        }
                        else
                        {
                            result = eCSR_ROAM_RESULT_FAILURE;
                        }
                        csrRoamCallCallback(pMac, sessionId, &roamInfo, pCommand->u.setKeyCmd.roamId, 
                                            eCSR_ROAM_REMOVE_KEY_COMPLETE, result);
                        if( csrLLRemoveEntry( &pMac->sme.smeCmdActiveList, pEntry, LL_ACCESS_LOCK ) )
                        {
                            csrReleaseCommandRemoveKey( pMac, pCommand );
                        }
                    }
                    else
                    {
                        smsLog( pMac, LOGW, "CSR: Roam Completion called but setkey command is not ACTIVE ..." );
                    }
                }
                else
                {
                    smsLog( pMac, LOGW, "CSR: SetKey Completion called but NO commands are ACTIVE ..." );
                }
                smeProcessPendingQueue( pMac );
            }
            break;
        case eWNI_SME_GET_STATISTICS_RSP:
            smsLog( pMac, LOG2, FL("Stats rsp from PE"));
            csrRoamStatsRspProcessor( pMac, pSirMsg );
            break;
#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)
        case eWNI_SME_GET_ROAM_RSSI_RSP:
            smsLog( pMac, LOG2, FL("Stats rsp from PE"));
            csrRoamRssiRspProcessor( pMac, pSirMsg );
            break;
#endif
#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
        case eWNI_SME_GET_TSM_STATS_RSP:
            smsLog( pMac, LOG2, FL("TSM Stats rsp from PE"));
            csrTsmStatsRspProcessor( pMac, pSirMsg );
            break;
#endif /*                                             */
        case eWNI_SME_GET_RSSI_REQ:
            smsLog( pMac, LOG2, FL("GetRssiReq from self"));
            csrUpdateRssi( pMac, pSirMsg );
            break;

        case eWNI_SME_GET_SNR_REQ:
            smsLog( pMac, LOG2, FL("GetSnrReq from self"));
            csrUpdateSnr(pMac, pSirMsg);
            break;

#ifdef WLAN_FEATURE_VOWIFI_11R
        case eWNI_SME_FT_PRE_AUTH_RSP:
            csrRoamFTPreAuthRspProcessor( pMac, (tpSirFTPreAuthRsp)pSirMsg );
            break;
#endif
        case eWNI_SME_MAX_ASSOC_EXCEEDED:
            pSmeMaxAssocInd = (tSmeMaxAssocInd*)pSirMsg;
            smsLog( pMac, LOG1, FL("send indication that max assoc have been reached and the new peer cannot be accepted"));
            sessionId = pSmeMaxAssocInd->sessionId;
            roamInfo.sessionId = sessionId;
            palCopyMemory(pMac->hHdd, &roamInfo.peerMac, pSmeMaxAssocInd->peerMac, sizeof(tCsrBssid));
            csrRoamCallCallback(pMac, sessionId, &roamInfo, 0, 
                    eCSR_ROAM_INFRA_IND, eCSR_ROAM_RESULT_MAX_ASSOC_EXCEEDED);
            break;
            
        case eWNI_SME_BTAMP_LOG_LINK_IND:
            smsLog( pMac, LOG1, FL("Establish logical link req from HCI serialized through MC thread"));
            btampEstablishLogLinkHdlr( pSirMsg );
            break;
        case eWNI_SME_RSSI_IND:
            smsLog( pMac, LOG1, FL("RSSI indication from TL serialized through MC thread"));
            csrRoamRssiIndHdlr( pMac, pSirMsg );
        break;
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
        case eWNI_SME_CANDIDATE_FOUND_IND:
            smsLog( pMac, LOG2, FL("Candidate found indication from PE"));
            csrNeighborRoamCandidateFoundIndHdlr( pMac, pSirMsg );
        break;
        case eWNI_SME_HANDOFF_REQ:
            smsLog( pMac, LOG2, FL("Handoff Req from self"));
            csrNeighborRoamHandoffReqHdlr( pMac, pSirMsg );
            break;
#endif

        default:
            break;
    }  //                           
}

void csrCallRoamingCompletionCallback(tpAniSirGlobal pMac, tCsrRoamSession *pSession, 
                                      tCsrRoamInfo *pRoamInfo, tANI_U32 roamId, eCsrRoamResult roamResult)
{
   if(pSession)
   {
      if(pSession->bRefAssocStartCnt)
      {
         pSession->bRefAssocStartCnt--;
         VOS_ASSERT( pSession->bRefAssocStartCnt == 0);
         //                                                                            
         csrRoamCallCallback(pMac, pSession->sessionId, NULL, roamId, 
                                               eCSR_ROAM_ASSOCIATION_COMPLETION, 
                                               eCSR_ROAM_RESULT_FAILURE);
      }
      csrRoamCallCallback(pMac, pSession->sessionId, pRoamInfo, roamId, eCSR_ROAM_ROAMING_COMPLETION, roamResult);
   }
   else
   {
      smsLog(pMac, LOGW, FL("  pSession is NULL"));
   }
}


eHalStatus csrRoamStartRoaming(tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamingReason roamingReason)
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    if(CSR_IS_LOSTLINK_ROAMING(roamingReason) && 
        (eANI_BOOLEAN_FALSE == pMac->roam.roamSession[sessionId].fCancelRoaming))
    {
        status = csrScanRequestLostLink1( pMac, sessionId );
    }
    return(status);
}

//                                                                   
tANI_BOOLEAN csrRoamCompleteRoaming(tpAniSirGlobal pMac, tANI_U32 sessionId, 
                                    tANI_BOOLEAN fForce, eCsrRoamResult roamResult)
{
    tANI_BOOLEAN fCompleted = eANI_BOOLEAN_TRUE;
    tANI_TIMESTAMP roamTime = (tANI_TIMESTAMP)(pMac->roam.configParam.nRoamingTime * PAL_TICKS_PER_SECOND);
    tANI_TIMESTAMP curTime = (tANI_TIMESTAMP)palGetTickCount(pMac->hHdd);
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eANI_BOOLEAN_FALSE;
    }
    //                        
    if(pSession->fCancelRoaming || fForce || 
       ((curTime - pSession->roamingStartTime) > roamTime) ||
       eCsrReassocRoaming == pSession->roamingReason ||
       eCsrDynamicRoaming == pSession->roamingReason)
    {
        smsLog(pMac, LOGW, FL("  indicates roaming completion"));
        if(pSession->fCancelRoaming && CSR_IS_LOSTLINK_ROAMING(pSession->roamingReason))
        {
            //                                                     
            //                                                                       
            //                                                                             
            //                        
            if(eSIR_BEACON_MISSED == pSession->roamingStatusCode)
            {
                roamResult = eCSR_ROAM_RESULT_LOSTLINK;
            }
            else if(eCsrLostlinkRoamingDisassoc == pSession->roamingReason)
            {
                roamResult = eCSR_ROAM_RESULT_DISASSOC_IND;
            }
            else if(eCsrLostlinkRoamingDeauth == pSession->roamingReason)
            {
                roamResult = eCSR_ROAM_RESULT_DEAUTH_IND;
            }
            else
            {
                roamResult = eCSR_ROAM_RESULT_LOSTLINK;
            }
        }
        csrCallRoamingCompletionCallback(pMac, pSession, NULL, 0, roamResult);
        pSession->roamingReason = eCsrNotRoaming;
    }
    else
    {
        pSession->roamResult = roamResult;
        if(!HAL_STATUS_SUCCESS(csrRoamStartRoamingTimer(pMac, sessionId, PAL_TIMER_TO_SEC_UNIT)))
        {
            csrCallRoamingCompletionCallback(pMac, pSession, NULL, 0, roamResult);
            pSession->roamingReason = eCsrNotRoaming;
        }
        else
        {
            fCompleted = eANI_BOOLEAN_FALSE;
        }
    }
    return(fCompleted);
}

void csrRoamCancelRoaming(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return;
    }
    
    if(CSR_IS_ROAMING(pSession))
    {
        smsLog(pMac, LOGW, "   Cancelling roaming");
        pSession->fCancelRoaming = eANI_BOOLEAN_TRUE;
        if(CSR_IS_ROAM_JOINING(pMac, sessionId) && CSR_IS_ROAM_SUBSTATE_CONFIG(pMac, sessionId))
        {
            //                                                                   
        }
        else
        {
            eCsrRoamResult roamResult = CSR_IS_LOSTLINK_ROAMING(pSession->roamingReason) ? 
                                                    eCSR_ROAM_RESULT_LOSTLINK : eCSR_ROAM_RESULT_NONE;
            //                              
            csrRoamCompleteRoaming(pMac, sessionId, eANI_BOOLEAN_TRUE, roamResult);
            //                                                                                    
            csrScanAbortMacScan(pMac);
            csrRoamStopRoamingTimer(pMac, sessionId);
        }
    }
}

void csrRoamRoamingTimerHandler(void *pv)
{
    tCsrTimerInfo *pInfo = (tCsrTimerInfo *)pv;
    tpAniSirGlobal pMac = pInfo->pMac;
    tANI_U32 sessionId = pInfo->sessionId;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return;
    }
    
    if(eANI_BOOLEAN_FALSE == pSession->fCancelRoaming) 
    {
        if(!HAL_STATUS_SUCCESS(csrRoamStartRoaming(pMac, sessionId, pSession->roamingReason)))
        {
            csrCallRoamingCompletionCallback(pMac, pSession, NULL, 0, pSession->roamResult);
            pSession->roamingReason = eCsrNotRoaming;
        }
    }
}

eHalStatus csrRoamStartRoamingTimer(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 interval)
{
    eHalStatus status;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found"), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    smsLog(pMac, LOG1, " csrScanStartRoamingTimer");
    pSession->roamingTimerInfo.sessionId = (tANI_U8)sessionId;
    status = vos_timer_start(&pSession->hTimerRoaming, interval/PAL_TIMER_TO_MS_UNIT);
    
    return (status);
}

eHalStatus csrRoamStopRoamingTimer(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    return (vos_timer_stop(&pMac->roam.roamSession[sessionId].hTimerRoaming));
}

void csrRoamWaitForKeyTimeOutHandler(void *pv)
{
    tCsrTimerInfo *pInfo = (tCsrTimerInfo *)pv;
    tpAniSirGlobal pMac = pInfo->pMac;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, pInfo->sessionId );
    eHalStatus status = eHAL_STATUS_FAILURE;

    smsLog(pMac, LOGW, "WaitForKey timer expired in state=%d sub-state=%d",
            pMac->roam.neighborRoamInfo.neighborRoamState,
            pMac->roam.curSubState[pInfo->sessionId]);

    if( CSR_IS_WAIT_FOR_KEY( pMac, pInfo->sessionId ) )
    {
#ifdef FEATURE_WLAN_LFR
        if (csrNeighborRoamIsHandoffInProgress(pMac))
        {
            /* 
                                                                  
                                           
             */
            smsLog(pMac, LOG2, "Enabling HB timer after WaitKey expiry"
                    " (nHBCount=%d)",
                    pMac->roam.configParam.HeartbeatThresh24);
            ccmCfgSetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD,
                    pMac->roam.configParam.HeartbeatThresh24,
                    NULL, eANI_BOOLEAN_FALSE);
        }
#endif
        smsLog(pMac, LOGW, " SME pre-auth state timeout. ");

        //                                                  
        if (CSR_ROAM_SESSION_MAX > pInfo->sessionId)
        {
            csrRoamSubstateChange(pMac, eCSR_ROAM_SUBSTATE_NONE,
                                  pInfo->sessionId);
        }

        if (pSession)
        {
            if( csrIsConnStateConnectedInfra(pMac, pInfo->sessionId) ) 
            {
                csrRoamLinkUp(pMac, pSession->connectedProfile.bssid);
                smeProcessPendingQueue(pMac);
                if( (pSession->connectedProfile.AuthType ==
                                           eCSR_AUTH_TYPE_SHARED_KEY) &&
                    ( (pSession->connectedProfile.EncryptionType ==
                                           eCSR_ENCRYPT_TYPE_WEP40) ||
                      (pSession->connectedProfile.EncryptionType ==
                                           eCSR_ENCRYPT_TYPE_WEP104) ))
                {
                    status = sme_AcquireGlobalLock( &pMac->sme );
                    if ( HAL_STATUS_SUCCESS( status ) )
                    {
                        csrRoamDisconnect( pMac, pInfo->sessionId,
                                      eCSR_DISCONNECT_REASON_UNSPECIFIED );
                        sme_ReleaseGlobalLock( &pMac->sme );
                    }
                }
            }
            else
            {
                smsLog(pMac, LOGW, "%s: could not post link up",
                        __func__);
            }
        }
        else
        {
            smsLog(pMac, LOGW, "%s: session not found", __func__);
        }
    }
    
}

eHalStatus csrRoamStartWaitForKeyTimer(tpAniSirGlobal pMac, tANI_U32 interval)
{
    eHalStatus status;
#ifdef FEATURE_WLAN_LFR
    if (csrNeighborRoamIsHandoffInProgress(pMac))
    {
        /*                                                      */
        smsLog(pMac, LOG2, "%s: disabling HB timer in state=%d sub-state=%d",
                __func__,
                pMac->roam.neighborRoamInfo.neighborRoamState,
                pMac->roam.curSubState[pMac->roam.WaitForKeyTimerInfo.sessionId]);
        ccmCfgSetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD, 0, NULL, eANI_BOOLEAN_FALSE);
    }
#endif
    smsLog(pMac, LOG1, " csrScanStartWaitForKeyTimer");
    status = vos_timer_start(&pMac->roam.hTimerWaitForKey, interval/PAL_TIMER_TO_MS_UNIT);
    
    return (status);
}

eHalStatus csrRoamStopWaitForKeyTimer(tpAniSirGlobal pMac)
{
    smsLog(pMac, LOG2, "WaitForKey timer stopped in state=%d sub-state=%d",
            pMac->roam.neighborRoamInfo.neighborRoamState,
            pMac->roam.curSubState[pMac->roam.WaitForKeyTimerInfo.sessionId]);
#ifdef FEATURE_WLAN_LFR
    if (csrNeighborRoamIsHandoffInProgress(pMac))
    {
        /* 
                                                              
                                                          
         */
        smsLog(pMac, LOG2, "Enabling HB timer after WaitKey stop"
                " (nHBCount=%d)",
                pMac->roam.configParam.HeartbeatThresh24);
        ccmCfgSetInt(pMac, WNI_CFG_HEART_BEAT_THRESHOLD,
            pMac->roam.configParam.HeartbeatThresh24,
            NULL, eANI_BOOLEAN_FALSE);
    }
#endif
    return (vos_timer_stop(&pMac->roam.hTimerWaitForKey));
}

void csrRoamIbssJoinTimerHandler(void *pv)
{
    tCsrTimerInfo *pInfo = (tCsrTimerInfo *)pv;
    tpAniSirGlobal pMac = pInfo->pMac;
    eCsrRoamDisconnectReason reason = eCSR_DISCONNECT_REASON_IBSS_JOIN_FAILURE;
    tANI_U32 sessionId = pInfo->sessionId;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return;
    }
    
    pSession->ibss_join_pending = FALSE;
    //                                                                                 
    //                                                                                                    
    //                                
    csrRoamDisconnectInternal(pMac, sessionId, reason);
}
eHalStatus csrRoamStartIbssJoinTimer(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 interval)
{
    eHalStatus status;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    smsLog(pMac, LOG1, " csrRoamStartIbssJoinTimer");
    pSession->ibssJoinTimerInfo.sessionId = (tANI_U8)sessionId;
    status = vos_timer_start(&pSession->hTimerIbssJoining, interval/PAL_TIMER_TO_MS_UNIT);
    
    return (status);
}
eHalStatus csrRoamStopIbssJoinTimer(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    return (vos_timer_stop(&pMac->roam.roamSession[sessionId].hTimerIbssJoining));
}
void csrRoamCompletion(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamInfo *pRoamInfo, tSmeCmd *pCommand, 
                        eCsrRoamResult roamResult, tANI_BOOLEAN fSuccess)
{
    eRoamCmdStatus roamStatus = csrGetRoamCompleteStatus(pMac, sessionId);
    tANI_U32 roamId = 0;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    /*                                             */
    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return;
    }
    
    if(pCommand)
    {
        roamId = pCommand->u.roamCmd.roamId;
        VOS_ASSERT( sessionId == pCommand->sessionId );
    }
    if(eCSR_ROAM_ROAMING_COMPLETION == roamStatus)
    {
        //                                    
        csrRoamCompleteRoaming(pMac, sessionId, fSuccess, roamResult);
    }
    else
    {
        VOS_ASSERT(pSession->bRefAssocStartCnt == 0);
        smsLog(pMac, LOGW, FL("  indicates association completion. roamResult = %d"), roamResult);
        csrRoamCallCallback(pMac, sessionId, pRoamInfo, roamId, roamStatus, roamResult);
    }
}

eHalStatus csrRoamLostLink( tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 type, tSirSmeRsp *pSirMsg)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirSmeDeauthInd *pDeauthIndMsg = NULL;
    tSirSmeDisassocInd *pDisassocIndMsg = NULL;
    eCsrRoamResult result = eCSR_ROAM_RESULT_LOSTLINK;
    tCsrRoamInfo *pRoamInfo = NULL;
    tCsrRoamInfo roamInfo;
    tANI_BOOLEAN fToRoam;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    /*                                             */
    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    //                                                                              
    fToRoam = CSR_IS_INFRASTRUCTURE(&pSession->connectedProfile);
    pSession->fCancelRoaming = eANI_BOOLEAN_FALSE;
    if ( eWNI_SME_DISASSOC_IND == type )
    {
        result = eCSR_ROAM_RESULT_DISASSOC_IND;
        pDisassocIndMsg = (tSirSmeDisassocInd *)pSirMsg;
        pSession->roamingStatusCode = pDisassocIndMsg->statusCode;
        pSession->joinFailStatusCode.reasonCode = pDisassocIndMsg->reasonCode;
    }
    else if ( eWNI_SME_DEAUTH_IND == type )
    {
        result = eCSR_ROAM_RESULT_DEAUTH_IND;
        pDeauthIndMsg = (tSirSmeDeauthInd *)pSirMsg;
        pSession->roamingStatusCode = pDeauthIndMsg->statusCode;
        /*                                 */
        pSession->joinFailStatusCode.reasonCode =
                (pDeauthIndMsg->reasonCode == eSIR_BEACON_MISSED) ?
                0 : pDeauthIndMsg->reasonCode;
       /*                                      
                                                
                                                   */
    }
    else
    {
        smsLog(pMac, LOGW, FL("gets an unknown type (%d)"), type);
        result = eCSR_ROAM_RESULT_NONE;
        pSession->joinFailStatusCode.reasonCode = 1;
    }
    
    //                                    
    if(!CSR_IS_INFRA_AP(&pSession->connectedProfile))
    {
        csrRoamCallCallback(pMac, sessionId, NULL, 0, eCSR_ROAM_LOSTLINK_DETECTED, result);
    }
    
    if ( eWNI_SME_DISASSOC_IND == type )
    {
        status = csrSendMBDisassocCnfMsg(pMac, pDisassocIndMsg);
    }
    else if ( eWNI_SME_DEAUTH_IND == type )
    {
        status = csrSendMBDeauthCnfMsg(pMac, pDeauthIndMsg);
    }
    if(!HAL_STATUS_SUCCESS(status))
    {
        //                                                          
        fToRoam = eANI_BOOLEAN_FALSE;
    }

    //                                 
    palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
    roamInfo.statusCode = (tSirResultCodes)pSession->roamingStatusCode;
    roamInfo.reasonCode = pSession->joinFailStatusCode.reasonCode;
    if( eWNI_SME_DISASSOC_IND == type)
    {
        //          
        palCopyMemory(pMac->hHdd, roamInfo.peerMac, pDisassocIndMsg->peerMacAddr, sizeof(tSirMacAddr));
        roamInfo.staId = (tANI_U8)pDisassocIndMsg->staId;
    }
    else if( eWNI_SME_DEAUTH_IND == type )
    {
        //          
        palCopyMemory(pMac->hHdd, roamInfo.peerMac, pDeauthIndMsg->peerMacAddr, sizeof(tSirMacAddr));
        roamInfo.staId = (tANI_U8)pDeauthIndMsg->staId;
    }
    smsLog(pMac, LOGW, FL("roamInfo.staId (%d)"), roamInfo.staId);

    /*                                                                              
                                                                                   
                                                        */
    if(fToRoam)
    {
        //                                                    
        csrRoamRemoveConnectedBssFromScanCache(pMac, &pSession->connectedProfile);
        //                                      
        if( pMac->roam.configParam.nRoamingTime )
        {
            if(HAL_STATUS_SUCCESS(status = csrRoamStartRoaming(pMac, sessionId,
                        ( eWNI_SME_DEAUTH_IND == type ) ? 
                        eCsrLostlinkRoamingDeauth : eCsrLostlinkRoamingDisassoc)))
            {
                palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
                //                                               
                if(csrIsBssTypeIBSS(pSession->connectedProfile.BSSType))
                {
                    roamInfo.u.pConnectedProfile = &pSession->connectedProfile;
                    roamInfo.statusCode = (tSirResultCodes)pSession->roamingStatusCode;
                    roamInfo.reasonCode = pSession->joinFailStatusCode.reasonCode;
                }
                else
                {
                   roamInfo.reasonCode = eCsrRoamReasonSmeIssuedForLostLink;
                }
                    pRoamInfo = &roamInfo;
                pSession->roamingReason = ( eWNI_SME_DEAUTH_IND == type ) ? 
                        eCsrLostlinkRoamingDeauth : eCsrLostlinkRoamingDisassoc;
                pSession->roamingStartTime = (tANI_TIMESTAMP)palGetTickCount(pMac->hHdd);
                csrRoamCallCallback(pMac, sessionId, pRoamInfo, 0, eCSR_ROAM_ROAMING_START, eCSR_ROAM_RESULT_LOSTLINK);
            }
            else
            {
                smsLog(pMac, LOGW, " %s Fail to start roaming, status = %d", __func__, status);
                fToRoam = eANI_BOOLEAN_FALSE;
            }
        }
        else
        {
            //                                          
            fToRoam = eANI_BOOLEAN_FALSE;
        }
    }
    if(!fToRoam)
    {
        //                            
        if(!CSR_IS_INFRA_AP(&pSession->connectedProfile))
        {
            /*                                                                     
                                                                                 
                                                   
             */
            csrRoamCallCallback(pMac, sessionId, &roamInfo, 0, eCSR_ROAM_LOSTLINK, result);
        }

       /*                                               
                                                                                    */
        if(CSR_IS_INFRASTRUCTURE(&pSession->connectedProfile))
        {
            csrScanStartIdleScan(pMac);
        }
    }
    
    return (status);
}

eHalStatus csrRoamLostLinkAfterhandoffFailure( tpAniSirGlobal pMac,tANI_U32 sessionId)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tListElem *pEntry = NULL;
    tSmeCmd *pCommand = NULL;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    pSession->fCancelRoaming =  eANI_BOOLEAN_FALSE;
    //                                                    
    csrRoamRemoveConnectedBssFromScanCache(pMac, &pSession->connectedProfile);
    if(pMac->roam.configParam.nRoamingTime)
    {
       if(HAL_STATUS_SUCCESS(status = csrRoamStartRoaming(pMac,sessionId, pSession->roamingReason)))
       {
          //                                                                        
          pEntry = csrLLPeekHead(&pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK);
          if(pEntry)
          {
              pCommand = GET_BASE_ADDR(pEntry, tSmeCmd, Link);
          }
          if(pCommand)
          {
             if (( eSmeCommandRoam == pCommand->command ) &&
                 ( eCsrSmeIssuedAssocToSimilarAP == pCommand->u.roamCmd.roamReason))
             {
                 if( csrLLRemoveEntry( &pMac->sme.smeCmdActiveList, pEntry, LL_ACCESS_LOCK ) )
                 {
                    csrReleaseCommandRoam( pMac, pCommand );
                 }
             }
          }
          smsLog( pMac, LOGW, "Lost link roaming started ...");
       }
    }
    else
    {
       //                                          
       status = eHAL_STATUS_FAILURE;
    }
    
    return (status);
}
void csrRoamWmStatusChangeComplete( tpAniSirGlobal pMac )
{
    tListElem *pEntry;
    tSmeCmd *pCommand;
    pEntry = csrLLPeekHead( &pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK );
    if ( pEntry )
    {
        pCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
        if ( eSmeCommandWmStatusChange == pCommand->command )
        {
            //                                                                      
            //                  
            if( csrLLRemoveEntry( &pMac->sme.smeCmdActiveList, pEntry, LL_ACCESS_LOCK ) )
            {
                csrReleaseCommandWmStatusChange( pMac, pCommand );            
            }
            else
            {
                smsLog( pMac, LOGE, " ******csrRoamWmStatusChangeComplete fail to release command");
            }
            
        }
        else
        {
            smsLog( pMac, LOGW, "CSR: WmStatusChange Completion called but LOST LINK command is not ACTIVE ..." );
        }
    }
    else
    {
        smsLog( pMac, LOGW, "CSR: WmStatusChange Completion called but NO commands are ACTIVE ..." );
    }
    smeProcessPendingQueue( pMac );
}

void csrRoamProcessWmStatusChangeCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    tSirSmeRsp *pSirSmeMsg;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, pCommand->sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), pCommand->sessionId);
        return;
    }
    
    switch ( pCommand->u.wmStatusChangeCmd.Type )
    {
        case eCsrDisassociated:
            pSirSmeMsg = (tSirSmeRsp *)&pCommand->u.wmStatusChangeCmd.u.DisassocIndMsg;
            status = csrRoamLostLink(pMac, pCommand->sessionId, eWNI_SME_DISASSOC_IND, pSirSmeMsg);
            break;
        case eCsrDeauthenticated:
            pSirSmeMsg = (tSirSmeRsp *)&pCommand->u.wmStatusChangeCmd.u.DeauthIndMsg;
            status = csrRoamLostLink(pMac, pCommand->sessionId, eWNI_SME_DEAUTH_IND, pSirSmeMsg);
            break;
        default:
            smsLog(pMac, LOGW, FL("gets an unknown command %d"), pCommand->u.wmStatusChangeCmd.Type);
            break;
    }
    //                                                                                  
    if( CSR_IS_CONN_WDS(&pSession->connectedProfile) )
    {
        if( !HAL_STATUS_SUCCESS(csrRoamIssueStopBssCmd( pMac, pCommand->sessionId, eANI_BOOLEAN_TRUE )) )
        {
            //                
            smsLog(pMac, LOGE, FL("  failed to issue stopBSS command"));
        }
    }
    //                                                                          
    //                                                
    csrRoamWmStatusChangeComplete( pMac );
}

//                                                
//                                                                                                         
//                                                                     
static eCsrCfgDot11Mode csrRoamGetPhyModeBandForBss( tpAniSirGlobal pMac, tCsrRoamProfile *pProfile, 
                                                     tANI_U8 operationChn, eCsrBand *pBand )
{
    eCsrPhyMode phyModeIn = (eCsrPhyMode)pProfile->phyMode;
    eCsrCfgDot11Mode cfgDot11Mode = csrGetCfgDot11ModeFromCsrPhyMode(pProfile, phyModeIn, 
                                            pMac->roam.configParam.ProprietaryRatesEnabled);
    eCsrBand eBand;
    
    //                                                                                                
    if( ((!CSR_IS_INFRA_AP(pProfile )&& !CSR_IS_WDS(pProfile )) && 
         ((eCSR_CFG_DOT11_MODE_AUTO == pMac->roam.configParam.uCfgDot11Mode) ||
         (eCSR_CFG_DOT11_MODE_ABG == pMac->roam.configParam.uCfgDot11Mode))) ||
        (eCSR_CFG_DOT11_MODE_AUTO == cfgDot11Mode) || (eCSR_CFG_DOT11_MODE_ABG == cfgDot11Mode) )
    {
        switch( pMac->roam.configParam.uCfgDot11Mode )
        {
            case eCSR_CFG_DOT11_MODE_11A:
                cfgDot11Mode = eCSR_CFG_DOT11_MODE_11A;
                eBand = eCSR_BAND_5G;
                break;
            case eCSR_CFG_DOT11_MODE_11B:
                cfgDot11Mode = eCSR_CFG_DOT11_MODE_11B;
                eBand = eCSR_BAND_24;
                break;
            case eCSR_CFG_DOT11_MODE_11G:
                cfgDot11Mode = eCSR_CFG_DOT11_MODE_11G;
                eBand = eCSR_BAND_24;
                break;            
            case eCSR_CFG_DOT11_MODE_11N:
                cfgDot11Mode = eCSR_CFG_DOT11_MODE_11N;
                eBand = CSR_IS_CHANNEL_24GHZ(operationChn) ? eCSR_BAND_24 : eCSR_BAND_5G;
                break;
#ifdef WLAN_FEATURE_11AC
            case eCSR_CFG_DOT11_MODE_11AC:
                if (IS_FEATURE_SUPPORTED_BY_FW(DOT11AC))
                {
                    cfgDot11Mode = eCSR_CFG_DOT11_MODE_11AC;
                    eBand = eCSR_BAND_5G;
                }
                else
                {
                    cfgDot11Mode = eCSR_CFG_DOT11_MODE_11N;
                    eBand = CSR_IS_CHANNEL_24GHZ(operationChn) ? eCSR_BAND_24 : eCSR_BAND_5G;
                }
                break;
            case eCSR_CFG_DOT11_MODE_11AC_ONLY:
                if (IS_FEATURE_SUPPORTED_BY_FW(DOT11AC))
                {
                    cfgDot11Mode = eCSR_CFG_DOT11_MODE_11AC_ONLY;
                    eBand = eCSR_BAND_5G;
                }
                else
                {
                    eBand = CSR_IS_CHANNEL_24GHZ(operationChn) ? eCSR_BAND_24 : eCSR_BAND_5G;
                    cfgDot11Mode = eCSR_CFG_DOT11_MODE_11N;
                }
                break;
#endif
            case eCSR_CFG_DOT11_MODE_AUTO:
                eBand = pMac->roam.configParam.eBand;
                if (eCSR_BAND_24 == eBand)
                {
                    //                                                      
                    //                                                     
                    //                                                     
                    //                                                        
                    //                                                      
                    //                                                     
                    //                                                        
                    //                                                         
                    //                                                           
                    //           
                    //
                    //                                                          
                    //                                                         
                    //                                                          
                    cfgDot11Mode = eCSR_CFG_DOT11_MODE_11B;
                }
                else
                {
#ifdef WLAN_FEATURE_11AC
                    if (IS_FEATURE_SUPPORTED_BY_FW(DOT11AC))
                    {
                        cfgDot11Mode = eCSR_CFG_DOT11_MODE_11AC;
                        eBand = eCSR_BAND_5G;
                    }
                    else
                    {
                        cfgDot11Mode = eCSR_CFG_DOT11_MODE_11N;
                        eBand = CSR_IS_CHANNEL_24GHZ(operationChn) ? eCSR_BAND_24 : eCSR_BAND_5G;
                    }
#else
                    cfgDot11Mode = eCSR_CFG_DOT11_MODE_11N;
                    eBand = CSR_IS_CHANNEL_24GHZ(operationChn) ? eCSR_BAND_24 : eCSR_BAND_5G;
#endif
                }
                break;
            default:
                //                                      
                //                                                      
                if ( eCSR_OPERATING_CHANNEL_AUTO == operationChn )
                {
                    eBand = pMac->roam.configParam.eBand;
                    if(eCSR_BAND_24 == eBand)
                    {
                        //                                                                        
                        cfgDot11Mode = eCSR_CFG_DOT11_MODE_11B;
                    }
                    else
                    {
                        //           
                        eBand = eCSR_BAND_5G;
                        cfgDot11Mode = eCSR_CFG_DOT11_MODE_11A;
                    }
                }
                else if ( CSR_IS_CHANNEL_24GHZ(operationChn) )
                {
                    //                                                      
                    //                                                     
                    //                                                     
                    //                                                        
                    //                                                      
                    //                                                     
                    //                                                        
                    //                                                         
                    //                                                           
                    //           
                    //
                    //                                                          
                    //                                                         
                    //                                                          
                    cfgDot11Mode = eCSR_CFG_DOT11_MODE_11B;
                    eBand = eCSR_BAND_24;
                }
                else 
                {   
                    //                                               
                    cfgDot11Mode = eCSR_CFG_DOT11_MODE_11A;
                    eBand = eCSR_BAND_5G;
                }
                break;
        }//      
    }//                                             
    else
    {
            //                                     
            if ( eCSR_OPERATING_CHANNEL_AUTO == operationChn )
            {
                //                       
                eBand = pMac->roam.configParam.eBand;
                if(eCSR_BAND_ALL == eBand)
                {
                    //           
                    eBand = eCSR_BAND_5G;
                }
            }
            else if ( CSR_IS_CHANNEL_24GHZ(operationChn) )
            {
                eBand = eCSR_BAND_24;
            }
            else 
            {   
                eBand = eCSR_BAND_5G;
            }
    }
    if(pBand)
    {
        *pBand = eBand;
    }
    
   if (operationChn == 14){ 
     smsLog(pMac, LOGE, FL("  Switching to Dot11B mode "));
     cfgDot11Mode = eCSR_CFG_DOT11_MODE_11B;
   }

    /*                                                                                                               */
    if( (!CSR_IS_11n_ALLOWED(pProfile->EncryptionType.encryptionType[0] ) || ((pProfile->privacy == 1) && (pProfile->EncryptionType.encryptionType[0] == eCSR_ENCRYPT_TYPE_NONE))  ) &&
        ((eCSR_CFG_DOT11_MODE_11N == cfgDot11Mode) ||
#ifdef WLAN_FEATURE_11AC
        (eCSR_CFG_DOT11_MODE_11AC == cfgDot11Mode) ||
#endif
        (eCSR_CFG_DOT11_MODE_TAURUS == cfgDot11Mode)) )
    {
        //                     
        if ( CSR_IS_CHANNEL_24GHZ(operationChn) )
        {
            cfgDot11Mode = eCSR_CFG_DOT11_MODE_11G;
        }
        else
        {
            cfgDot11Mode = eCSR_CFG_DOT11_MODE_11A;
        }
    }
    return( cfgDot11Mode );
}

eHalStatus csrRoamIssueStopBss( tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamSubState NewSubstate )
{
    eHalStatus status;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
    {
        vos_log_ibss_pkt_type *pIbssLog;
        WLAN_VOS_DIAG_LOG_ALLOC(pIbssLog, vos_log_ibss_pkt_type, LOG_WLAN_IBSS_C);
        if(pIbssLog)
        {
            pIbssLog->eventId = WLAN_IBSS_EVENT_STOP_REQ;
            WLAN_VOS_DIAG_LOG_REPORT(pIbssLog);
        }
    }
#endif //                             
    pSession->ibss_join_pending = FALSE;
    csrRoamStopIbssJoinTimer(pMac, sessionId );
    //                                                  
    csrRoamSubstateChange( pMac, NewSubstate, sessionId );

    //                                                    
    status = csrSendMBStopBssReqMsg( pMac, sessionId );
    if(!HAL_STATUS_SUCCESS(status))
    {
        smsLog(pMac, LOGW, FL("csrSendMBStopBssReqMsg failed with status %d"), status);
    }
    return (status);
}

//                                                              
eHalStatus csrGetCfgValidChannels(tpAniSirGlobal pMac, tANI_U8 *pChannels, tANI_U32 *pNumChan)
{
   
    return (ccmCfgGetStr(pMac, WNI_CFG_VALID_CHANNEL_LIST,
                  (tANI_U8 *)pChannels,
                  pNumChan));
}

tPowerdBm csrGetCfgMaxTxPower (tpAniSirGlobal pMac, tANI_U8 channel)
{
    tANI_U32    cfgLength = 0;
    tANI_U16    cfgId = 0;
    tPowerdBm   maxTxPwr = 0;
    tANI_U8     *pCountryInfo = NULL;
    eHalStatus  status;
    tANI_U8     count = 0;
    tANI_U8     firstChannel;
    tANI_U8     maxChannels;

    if (CSR_IS_CHANNEL_5GHZ(channel))
    {
        cfgId = WNI_CFG_MAX_TX_POWER_5;
        cfgLength = WNI_CFG_MAX_TX_POWER_5_LEN;
    }
    else if (CSR_IS_CHANNEL_24GHZ(channel))
    {
        cfgId = WNI_CFG_MAX_TX_POWER_2_4;
        cfgLength = WNI_CFG_MAX_TX_POWER_2_4_LEN;
    }
    else
        return  maxTxPwr;

    status = palAllocateMemory(pMac->hHdd, (void **)&pCountryInfo, cfgLength);
    if (status != eHAL_STATUS_SUCCESS)
    {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                 FL("%s: palAllocateMemory() failed, status = %d"),
              __FUNCTION__, status);
        goto error;
    }
    status = ccmCfgGetStr(pMac, cfgId, (tANI_U8 *)pCountryInfo, &cfgLength);
    if (status != eHAL_STATUS_SUCCESS)
    {
        goto error;
    }
    /*                                     */
    while (count <= (cfgLength - (sizeof(tSirMacChanInfo))))
    {
        firstChannel = pCountryInfo[count++];
        maxChannels = pCountryInfo[count++];
        maxTxPwr = pCountryInfo[count++];

        if ((channel >= firstChannel) &&
            (channel < (firstChannel + maxChannels)))
        {
            break;
        }
    }

error:
    if (NULL != pCountryInfo)
        palFreeMemory(pMac->hHdd, pCountryInfo);

    return maxTxPwr;
}


tANI_BOOLEAN csrRoamIsChannelValid( tpAniSirGlobal pMac, tANI_U8 channel )
{
    tANI_BOOLEAN fValid = FALSE;
    tANI_U32 idxValidChannels;
    tANI_U32 len = sizeof(pMac->roam.validChannelList);
    
    if (HAL_STATUS_SUCCESS(csrGetCfgValidChannels(pMac, pMac->roam.validChannelList, &len)))
    {
        for ( idxValidChannels = 0; ( idxValidChannels < len ); idxValidChannels++ )
        {
            if ( channel == pMac->roam.validChannelList[ idxValidChannels ] )
            {
                fValid = TRUE;
                break;
            }
        }
    }    
    pMac->roam.numValidChannels = len;   
    return fValid;
}

tANI_BOOLEAN csrRoamIsValid40MhzChannel(tpAniSirGlobal pMac, tANI_U8 channel)
{
    tANI_BOOLEAN fValid = eANI_BOOLEAN_FALSE;
    tANI_U8 i;
    for(i = 0; i < pMac->scan.base40MHzChannels.numChannels; i++)
    {
        if(channel == pMac->scan.base40MHzChannels.channelList[i])
        {
            fValid = eANI_BOOLEAN_TRUE;
            break;
        }
    }
    return (fValid);
}

//                                                                  
 static ePhyChanBondState csrGetCBModeFromIes(tpAniSirGlobal pMac, tANI_U8 primaryChn, tDot11fBeaconIEs *pIes)
{
    ePhyChanBondState eRet = PHY_SINGLE_CHANNEL_CENTERED;
    tANI_U8 centerChn;
    tANI_U32 ChannelBondingMode;
    if(CSR_IS_CHANNEL_24GHZ(primaryChn))
    {
        ChannelBondingMode = pMac->roam.configParam.channelBondingMode24GHz;
    }
    else
    {
        ChannelBondingMode = pMac->roam.configParam.channelBondingMode5GHz;
    }
    //                                    
    if(WNI_CFG_CHANNEL_BONDING_MODE_DISABLE != ChannelBondingMode)
    {
        if(pIes->HTCaps.present && (eHT_CHANNEL_WIDTH_40MHZ == pIes->HTCaps.supportedChannelWidthSet))
        {
            //                          
            if (NULL != &(pIes->RSN.pwise_cipher_suites[0][0]) &&
               !memcmp( &(pIes->RSN.pwise_cipher_suites[0][0]),
                                        "\x00\x0f\xac\x02" ,4))
            {
                smsLog(pMac, LOGW, " No channel bonding in TKIP mode ");
                eRet = PHY_SINGLE_CHANNEL_CENTERED;
            }

            else if(pIes->HTInfo.present)
            {
                /*                                                                           
                                                                                   
                 */
                smsLog(pMac, LOG1, "scws %u rtws %u sco %u",
                    pIes->HTCaps.supportedChannelWidthSet,
                    pIes->HTInfo.recommendedTxWidthSet,
                    pIes->HTInfo.secondaryChannelOffset);

                if (pIes->HTInfo.recommendedTxWidthSet == eHT_CHANNEL_WIDTH_40MHZ)
                    eRet = (ePhyChanBondState)pIes->HTInfo.secondaryChannelOffset;
                else
                    eRet = PHY_SINGLE_CHANNEL_CENTERED;
                switch (eRet) {
                    case PHY_DOUBLE_CHANNEL_LOW_PRIMARY:
                        centerChn = primaryChn + CSR_CB_CENTER_CHANNEL_OFFSET;
                        break;
                    case PHY_DOUBLE_CHANNEL_HIGH_PRIMARY:
                        centerChn = primaryChn - CSR_CB_CENTER_CHANNEL_OFFSET;
                        break;
                    case PHY_SINGLE_CHANNEL_CENTERED:
                    default:
                        centerChn = primaryChn;
                        break;
                }
                if((PHY_SINGLE_CHANNEL_CENTERED != eRet) && !csrRoamIsValid40MhzChannel(pMac, centerChn))
                {
                    smsLog(pMac, LOGE, "  Invalid center channel (%d), disable 40MHz mode", centerChn);
                    //                                   
                }
            }
        }
    }
    return eRet;
}
tANI_BOOLEAN csrIsEncryptionInList( tpAniSirGlobal pMac, tCsrEncryptionList *pCipherList, eCsrEncryptionType encryptionType )
{
    tANI_BOOLEAN fFound = FALSE;
    tANI_U32 idx;
    for( idx = 0; idx < pCipherList->numEntries; idx++ )
    {
        if( pCipherList->encryptionType[idx] == encryptionType )
        {
            fFound = TRUE;
            break;
        }
    }
    return fFound;
}
tANI_BOOLEAN csrIsAuthInList( tpAniSirGlobal pMac, tCsrAuthList *pAuthList, eCsrAuthType authType )
{
    tANI_BOOLEAN fFound = FALSE;
    tANI_U32 idx;
    for( idx = 0; idx < pAuthList->numEntries; idx++ )
    {
        if( pAuthList->authType[idx] == authType )
        {
            fFound = TRUE;
            break;
        }
    }
    return fFound;
}
tANI_BOOLEAN csrIsSameProfile(tpAniSirGlobal pMac, tCsrRoamConnectedProfile *pProfile1, tCsrRoamProfile *pProfile2)
{
    tANI_BOOLEAN fCheck = eANI_BOOLEAN_FALSE;
    tCsrScanResultFilter *pScanFilter = NULL;
    eHalStatus status = eHAL_STATUS_SUCCESS;
    
    if(pProfile1 && pProfile2)
    {
        status = palAllocateMemory(pMac->hHdd, (void **)&pScanFilter, sizeof(tCsrScanResultFilter));
        if(HAL_STATUS_SUCCESS(status))
        {
            palZeroMemory(pMac->hHdd, pScanFilter, sizeof(tCsrScanResultFilter));
            status = csrRoamPrepareFilterFromProfile(pMac, pProfile2, pScanFilter);
            if(HAL_STATUS_SUCCESS(status))
            {
                fCheck = eANI_BOOLEAN_FALSE;
                do
                {
                    tANI_U32 i;
                    for(i = 0; i < pScanFilter->SSIDs.numOfSSIDs; i++)
                    {
                        fCheck = csrIsSsidMatch( pMac, pScanFilter->SSIDs.SSIDList[i].SSID.ssId, 
                                                pScanFilter->SSIDs.SSIDList[i].SSID.length,
                                                pProfile1->SSID.ssId, pProfile1->SSID.length, eANI_BOOLEAN_FALSE );
                        if ( fCheck ) break;
                    }
                    if(!fCheck)
                    {
                        break;
                    }
                    if( !csrIsAuthInList( pMac, &pProfile2->AuthType, pProfile1->AuthType)
                        || pProfile2->BSSType != pProfile1->BSSType
                        || !csrIsEncryptionInList( pMac, &pProfile2->EncryptionType, pProfile1->EncryptionType )
                        )
                    {
                        fCheck = eANI_BOOLEAN_FALSE;
                        break;
                    }
#ifdef WLAN_FEATURE_VOWIFI_11R
                    if (pProfile1->MDID.mdiePresent || pProfile2->MDID.mdiePresent)
                    {
                        if (pProfile1->MDID.mobilityDomain != pProfile2->MDID.mobilityDomain)
                        {
                            fCheck = eANI_BOOLEAN_FALSE;
                            break;
                        }
                    }
#endif
                    //           
                    fCheck = eANI_BOOLEAN_TRUE;
                }while(0);
                csrFreeScanFilter(pMac, pScanFilter);
            }
            palFreeMemory(pMac->hHdd, pScanFilter);
        }
    }
    
    return (fCheck);
}

tANI_BOOLEAN csrRoamIsSameProfileKeys(tpAniSirGlobal pMac, tCsrRoamConnectedProfile *pConnProfile, tCsrRoamProfile *pProfile2)
{
    tANI_BOOLEAN fCheck = eANI_BOOLEAN_FALSE;
    int i;
    do
    {
        //                         
        if(!csrIsEncryptionInList(pMac, &pProfile2->EncryptionType, eCSR_ENCRYPT_TYPE_WEP40_STATICKEY) &&
            !csrIsEncryptionInList(pMac, &pProfile2->EncryptionType, eCSR_ENCRYPT_TYPE_WEP104_STATICKEY))
        {
            fCheck = eANI_BOOLEAN_TRUE;
            break;
        }
        if(!csrIsEncryptionInList(pMac, &pProfile2->EncryptionType, pConnProfile->EncryptionType)) break;
        if(pConnProfile->Keys.defaultIndex != pProfile2->Keys.defaultIndex) break;
        for(i = 0; i < CSR_MAX_NUM_KEY; i++)
        {
            if(pConnProfile->Keys.KeyLength[i] != pProfile2->Keys.KeyLength[i]) break;
            if(!palEqualMemory(pMac->hHdd, &pConnProfile->Keys.KeyMaterial[i], 
                            &pProfile2->Keys.KeyMaterial[i], pProfile2->Keys.KeyLength[i]))
            {
                break;
            }
        }
        if( i == CSR_MAX_NUM_KEY)
        {
            fCheck = eANI_BOOLEAN_TRUE;
        }
    }while(0);
    return (fCheck);
}

//    

tANI_U8 csrRoamGetIbssStartChannelNumber50( tpAniSirGlobal pMac )
{
    tANI_U8 channel = 0;     
    tANI_U32 idx;
    tANI_U32 idxValidChannels;
    tANI_BOOLEAN fFound = FALSE;
    tANI_U32 len = sizeof(pMac->roam.validChannelList);
    
    if(eCSR_OPERATING_CHANNEL_ANY != pMac->roam.configParam.AdHocChannel5G)
    {
        channel = pMac->roam.configParam.AdHocChannel5G;
        if(!csrRoamIsChannelValid(pMac, channel))
        {
            channel = 0;
        }
    }
    if (0 == channel && HAL_STATUS_SUCCESS(csrGetCfgValidChannels(pMac, (tANI_U8 *)pMac->roam.validChannelList, &len)))
    {
        for ( idx = 0; ( idx < CSR_NUM_IBSS_START_CHANNELS_50 ) && !fFound; idx++ ) 
        {
            for ( idxValidChannels = 0; ( idxValidChannels < len ) && !fFound; idxValidChannels++ )
            {
                if ( csrStartIbssChannels50[ idx ] == pMac->roam.validChannelList[ idxValidChannels ] )
                {
                    fFound = TRUE;
                    channel = csrStartIbssChannels50[ idx ];
                }
            }
        }
        //                                                                                                             
        if (!fFound)    
        {
            for ( idxValidChannels = 0; idxValidChannels < len ; idxValidChannels++ )
            {
                if ( CSR_IS_CHANNEL_5GHZ(pMac->roam.validChannelList[ idxValidChannels ]) )   //                              
                {
                    channel = csrStartIbssChannels50[ idxValidChannels ];
                    break;
                }
            }
        }
    }//  
    
    return( channel );    
}

tANI_U8 csrRoamGetIbssStartChannelNumber24( tpAniSirGlobal pMac )
{
    tANI_U8 channel = 1;
    tANI_U32 idx;
    tANI_U32 idxValidChannels;
    tANI_BOOLEAN fFound = FALSE;
    tANI_U32 len = sizeof(pMac->roam.validChannelList);
    
    if(eCSR_OPERATING_CHANNEL_ANY != pMac->roam.configParam.AdHocChannel24)
    {
        channel = pMac->roam.configParam.AdHocChannel24;
        if(!csrRoamIsChannelValid(pMac, channel))
        {
            channel = 0;
        }
    }
    
    if (0 == channel && HAL_STATUS_SUCCESS(csrGetCfgValidChannels(pMac, (tANI_U8 *)pMac->roam.validChannelList, &len)))
    {
        for ( idx = 0; ( idx < CSR_NUM_IBSS_START_CHANNELS_24 ) && !fFound; idx++ ) 
        {
            for ( idxValidChannels = 0; ( idxValidChannels < len ) && !fFound; idxValidChannels++ )
            {
                if ( csrStartIbssChannels24[ idx ] == pMac->roam.validChannelList[ idxValidChannels ] )
                {
                    fFound = TRUE;
                    channel = csrStartIbssChannels24[ idx ];
                }
            }
        }
    }
    
    return( channel );    
}

static void csrRoamGetBssStartParms( tpAniSirGlobal pMac, tCsrRoamProfile *pProfile, 
                                      tCsrRoamStartBssParams *pParam )
{
    eCsrCfgDot11Mode cfgDot11Mode;
    eCsrBand eBand;
    tANI_U8 channel = 0;
    tSirNwType nwType;
    tANI_U8 operationChannel = 0; 
    
    if(pProfile->ChannelInfo.numOfChannels && pProfile->ChannelInfo.ChannelList)
    {
       operationChannel = pProfile->ChannelInfo.ChannelList[0];
    }
    
    cfgDot11Mode = csrRoamGetPhyModeBandForBss( pMac, pProfile, operationChannel, &eBand );
    
    if( ( (pProfile->csrPersona == VOS_P2P_CLIENT_MODE) ||
          (pProfile->csrPersona == VOS_P2P_GO_MODE) )
     && ( cfgDot11Mode == eCSR_CFG_DOT11_MODE_11B)
      )
    {
        /*                          */
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_FATAL, 
              FL("For P2PClient/P2P-GO (persona %d) cfgDot11Mode is 11B"),
              pProfile->csrPersona);
        VOS_ASSERT(0);
    }
    switch( cfgDot11Mode )
    {
        case eCSR_CFG_DOT11_MODE_11G:
            nwType = eSIR_11G_NW_TYPE;
            break;
        case eCSR_CFG_DOT11_MODE_11B:
            nwType = eSIR_11B_NW_TYPE;
            break;   
        case eCSR_CFG_DOT11_MODE_11A:
            nwType = eSIR_11A_NW_TYPE;
            break;
        default:
        case eCSR_CFG_DOT11_MODE_11N:
        case eCSR_CFG_DOT11_MODE_TAURUS:
            //                                                                              
            if(eCSR_BAND_24 == eBand)
            {
                nwType = eSIR_11G_NW_TYPE;
            }
            else
            {
                nwType = eSIR_11A_NW_TYPE;
            }
            break;
    }   

    pParam->extendedRateSet.numRates = 0;

    switch ( nwType )
    {
        default:
            smsLog(pMac, LOGE, FL("sees an unknown pSirNwType (%d)"), nwType);
        case eSIR_11A_NW_TYPE:
        
            pParam->operationalRateSet.numRates = 8;
        
            pParam->operationalRateSet.rate[0] = SIR_MAC_RATE_6 | CSR_DOT11_BASIC_RATE_MASK;
            pParam->operationalRateSet.rate[1] = SIR_MAC_RATE_9;
            pParam->operationalRateSet.rate[2] = SIR_MAC_RATE_12 | CSR_DOT11_BASIC_RATE_MASK;
            pParam->operationalRateSet.rate[3] = SIR_MAC_RATE_18;
            pParam->operationalRateSet.rate[4] = SIR_MAC_RATE_24 | CSR_DOT11_BASIC_RATE_MASK;
            pParam->operationalRateSet.rate[5] = SIR_MAC_RATE_36;
            pParam->operationalRateSet.rate[6] = SIR_MAC_RATE_48;
            pParam->operationalRateSet.rate[7] = SIR_MAC_RATE_54;
            
            if ( eCSR_OPERATING_CHANNEL_ANY == operationChannel ) 
            {
                channel = csrRoamGetIbssStartChannelNumber50( pMac );
                if( 0 == channel &&
                    CSR_IS_PHY_MODE_DUAL_BAND(pProfile->phyMode) && 
                    CSR_IS_PHY_MODE_DUAL_BAND(pMac->roam.configParam.phyMode) 
                    )
                {
                    //                                                                    
                    //                                                                                  
                    nwType = eSIR_11B_NW_TYPE;
                    channel = csrRoamGetIbssStartChannelNumber24( pMac );
                   pParam->operationalRateSet.numRates = 4;
                   pParam->operationalRateSet.rate[0] = SIR_MAC_RATE_1 | CSR_DOT11_BASIC_RATE_MASK;
                   pParam->operationalRateSet.rate[1] = SIR_MAC_RATE_2 | CSR_DOT11_BASIC_RATE_MASK;
                   pParam->operationalRateSet.rate[2] = SIR_MAC_RATE_5_5 | CSR_DOT11_BASIC_RATE_MASK;
                   pParam->operationalRateSet.rate[3] = SIR_MAC_RATE_11 | CSR_DOT11_BASIC_RATE_MASK;
                }
            }
            else 
            {
                channel = operationChannel;
            }
            break;
            
        case eSIR_11B_NW_TYPE:
            pParam->operationalRateSet.numRates = 4;
            pParam->operationalRateSet.rate[0] = SIR_MAC_RATE_1 | CSR_DOT11_BASIC_RATE_MASK;
            pParam->operationalRateSet.rate[1] = SIR_MAC_RATE_2 | CSR_DOT11_BASIC_RATE_MASK;
            pParam->operationalRateSet.rate[2] = SIR_MAC_RATE_5_5 | CSR_DOT11_BASIC_RATE_MASK;
            pParam->operationalRateSet.rate[3] = SIR_MAC_RATE_11 | CSR_DOT11_BASIC_RATE_MASK;
            if ( eCSR_OPERATING_CHANNEL_ANY == operationChannel ) 
            {
                channel = csrRoamGetIbssStartChannelNumber24( pMac );
            }
            else 
            {
                channel = operationChannel;
            }
            
            break;     
        case eSIR_11G_NW_TYPE:
            /*                                              */ 
            if( (pProfile->csrPersona == VOS_P2P_CLIENT_MODE) ||
                (pProfile->csrPersona == VOS_P2P_GO_MODE)
              )
            {
                pParam->operationalRateSet.numRates = 8;
            
                pParam->operationalRateSet.rate[0] = SIR_MAC_RATE_6 | CSR_DOT11_BASIC_RATE_MASK;
                pParam->operationalRateSet.rate[1] = SIR_MAC_RATE_9;
                pParam->operationalRateSet.rate[2] = SIR_MAC_RATE_12 | CSR_DOT11_BASIC_RATE_MASK;
                pParam->operationalRateSet.rate[3] = SIR_MAC_RATE_18;
                pParam->operationalRateSet.rate[4] = SIR_MAC_RATE_24 | CSR_DOT11_BASIC_RATE_MASK;
                pParam->operationalRateSet.rate[5] = SIR_MAC_RATE_36;
                pParam->operationalRateSet.rate[6] = SIR_MAC_RATE_48;
                pParam->operationalRateSet.rate[7] = SIR_MAC_RATE_54;
            }
            else
            {
            pParam->operationalRateSet.numRates = 4;
            pParam->operationalRateSet.rate[0] = SIR_MAC_RATE_1 | CSR_DOT11_BASIC_RATE_MASK;
            pParam->operationalRateSet.rate[1] = SIR_MAC_RATE_2 | CSR_DOT11_BASIC_RATE_MASK;
            pParam->operationalRateSet.rate[2] = SIR_MAC_RATE_5_5 | CSR_DOT11_BASIC_RATE_MASK;
            pParam->operationalRateSet.rate[3] = SIR_MAC_RATE_11 | CSR_DOT11_BASIC_RATE_MASK;
               
            pParam->extendedRateSet.numRates = 8;
                        pParam->extendedRateSet.rate[0] = SIR_MAC_RATE_6;
            pParam->extendedRateSet.rate[1] = SIR_MAC_RATE_9;
            pParam->extendedRateSet.rate[2] = SIR_MAC_RATE_12;
            pParam->extendedRateSet.rate[3] = SIR_MAC_RATE_18;
            pParam->extendedRateSet.rate[4] = SIR_MAC_RATE_24;
            pParam->extendedRateSet.rate[5] = SIR_MAC_RATE_36;
            pParam->extendedRateSet.rate[6] = SIR_MAC_RATE_48;
            pParam->extendedRateSet.rate[7] = SIR_MAC_RATE_54;
            }
            
            if ( eCSR_OPERATING_CHANNEL_ANY == operationChannel ) 
            {
                channel = csrRoamGetIbssStartChannelNumber24( pMac );
            }
            else 
            {
                channel = operationChannel;
            }
            
            break;            
    }
    pParam->operationChn = channel;
    pParam->sirNwType = nwType;
}

static void csrRoamGetBssStartParmsFromBssDesc( tpAniSirGlobal pMac, tSirBssDescription *pBssDesc, 
                                                 tDot11fBeaconIEs *pIes, tCsrRoamStartBssParams *pParam )
{
    
    if( pParam )
    {
        pParam->sirNwType = pBssDesc->nwType;
        pParam->cbMode = PHY_SINGLE_CHANNEL_CENTERED;
        pParam->operationChn = pBssDesc->channelId;
        palCopyMemory( pMac->hHdd, &pParam->bssid, pBssDesc->bssId, sizeof(tCsrBssid) );
    
        if( pIes )
        {
            if(pIes->SuppRates.present)
            {
                pParam->operationalRateSet.numRates = pIes->SuppRates.num_rates;
                if(pIes->SuppRates.num_rates > SIR_MAC_RATESET_EID_MAX)
                {
                    smsLog(pMac, LOGE, FL("num_rates :%d is more than SIR_MAC_RATESET_EID_MAX, resetting to SIR_MAC_RATESET_EID_MAX"),
                      pIes->SuppRates.num_rates);
                    pIes->SuppRates.num_rates = SIR_MAC_RATESET_EID_MAX;
                }
                palCopyMemory(pMac->hHdd, pParam->operationalRateSet.rate, pIes->SuppRates.rates, 
                    sizeof(*pIes->SuppRates.rates) * pIes->SuppRates.num_rates);
            }
            if( pIes->SSID.present )
            {
                pParam->ssId.length = pIes->SSID.num_ssid;
                palCopyMemory(pMac->hHdd, pParam->ssId.ssId, pIes->SSID.ssid, pParam->ssId.length);
            }
            pParam->cbMode = csrGetCBModeFromIes(pMac, pParam->operationChn, pIes);
        }
        else
        {
            pParam->ssId.length = 0;
           pParam->operationalRateSet.numRates = 0;
        }
    }
}

static void csrRoamDetermineMaxRateForAdHoc( tpAniSirGlobal pMac, tSirMacRateSet *pSirRateSet )
{
    tANI_U8 MaxRate = 0;
    tANI_U32 i;
    tANI_U8 *pRate;    
   
    pRate = pSirRateSet->rate;
    for ( i = 0; i < pSirRateSet->numRates; i++ )
    {
        MaxRate = CSR_MAX( MaxRate, ( pRate[ i ] & (~CSR_DOT11_BASIC_RATE_MASK) ) );
    }
    
    //                                                        
    
    //                                  
    
    return;
}

eHalStatus csrRoamIssueStartBss( tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamStartBssParams *pParam, 
                                 tCsrRoamProfile *pProfile, tSirBssDescription *pBssDesc, tANI_U32 roamId )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    eCsrBand eBand;
    //                                                   
    csrRoamSubstateChange( pMac, eCSR_ROAM_SUBSTATE_START_BSS_REQ, sessionId );
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
    //                                                
    if( CSR_IS_IBSS( pProfile ) )
    {
        vos_log_ibss_pkt_type *pIbssLog;
        WLAN_VOS_DIAG_LOG_ALLOC(pIbssLog, vos_log_ibss_pkt_type, LOG_WLAN_IBSS_C);
        if(pIbssLog)
        {
            if(pBssDesc)
            {
                pIbssLog->eventId = WLAN_IBSS_EVENT_JOIN_IBSS_REQ;
                palCopyMemory(pMac->hHdd, pIbssLog->bssid, pBssDesc->bssId, 6);
            }
            else
            {
                pIbssLog->eventId = WLAN_IBSS_EVENT_START_IBSS_REQ;
            }
            palCopyMemory(pMac->hHdd, pIbssLog->ssid, pParam->ssId.ssId,
                pParam->ssId.length);
            if(pProfile->ChannelInfo.numOfChannels == 0)
            {
                pIbssLog->channelSetting = AUTO_PICK;
            }
            else
            {
                pIbssLog->channelSetting = SPECIFIED;
            }
            pIbssLog->operatingChannel = pParam->operationChn;
            WLAN_VOS_DIAG_LOG_REPORT(pIbssLog);
        }
    }
#endif //                             
    //                                       
    pParam->nRSNIELength = (tANI_U16)pProfile->nRSNReqIELength;
    pParam->pRSNIE = pProfile->pRSNReqIE;

    pParam->privacy           = pProfile->privacy;
    pParam->fwdWPSPBCProbeReq = pProfile->fwdWPSPBCProbeReq;   
    pParam->authType          = pProfile->csr80211AuthType;
    pParam->beaconInterval    = pProfile->beaconInterval;
    pParam->dtimPeriod        = pProfile->dtimPeriod;
    pParam->ApUapsdEnable     = pProfile->ApUapsdEnable;
    pParam->ssidHidden        = pProfile->SSIDs.SSIDList[0].ssidHidden;
    if (CSR_IS_INFRA_AP(pProfile)&& (pParam->operationChn != 0))
    {
        if (csrIsValidChannel(pMac, pParam->operationChn) != eHAL_STATUS_SUCCESS)
        {
            pParam->operationChn = INFRA_AP_DEFAULT_CHANNEL;                   
        }  
    }
    pParam->protEnabled     = pProfile->protEnabled;
    pParam->obssProtEnabled = pProfile->obssProtEnabled;
    pParam->ht_protection   = pProfile->cfg_protection;
    pParam->wps_state       = pProfile->wps_state;

    pParam->uCfgDot11Mode = csrRoamGetPhyModeBandForBss(pMac, pProfile, pParam->operationChn /*                           */, 
                                        &eBand);
    pParam->bssPersona = pProfile->csrPersona;
    //                                                              
    status = csrSendMBStartBssReqMsg( pMac, sessionId, pProfile->BSSType, pParam, pBssDesc );
    return (status);
}

static void csrRoamPrepareBssParams(tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, 
                                     tSirBssDescription *pBssDesc, tBssConfigParam *pBssConfig, tDot11fBeaconIEs *pIes)
{
    tANI_U8 Channel;
    ePhyChanBondState cbMode = PHY_SINGLE_CHANNEL_CENTERED;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return;
    }
    
    if( pBssDesc )
    {
        csrRoamGetBssStartParmsFromBssDesc( pMac, pBssDesc, pIes, &pSession->bssParams );
        //                                                                                   
        //                                           
        //                                              
        if( CSR_IS_WDS_STA( pProfile ) )
        {
            palCopyMemory(pMac->hHdd, &pSession->bssParams.bssid, &pSession->selfMacAddr, sizeof(tCsrBssid));
        }
    }
    else
    {
        csrRoamGetBssStartParms(pMac, pProfile, &pSession->bssParams);
        //                  
        if(pProfile->SSIDs.numOfSSIDs)
        {
            palCopyMemory(pMac->hHdd, &pSession->bssParams.ssId, pProfile->SSIDs.SSIDList, sizeof(tSirMacSSid));
        }
        //                                              
        if( CSR_IS_WDS_STA( pProfile ) )
        {
            palCopyMemory(pMac->hHdd, &pSession->bssParams.bssid, &pSession->selfMacAddr, sizeof(tCsrBssid));
        }
        //                   
        else if( pProfile->BSSIDs.numOfBSSIDs )
        {
            palCopyMemory(pMac->hHdd, &pSession->bssParams.bssid, pProfile->BSSIDs.bssid, sizeof(tCsrBssid));
        }
        else
        {
            palZeroMemory( pMac->hHdd, &pSession->bssParams.bssid, sizeof(tCsrBssid) );
        }
    }
    Channel = pSession->bssParams.operationChn;
    //                                                     
    //                                                        
    //                                         
    pProfile->operationChannel = Channel;
    
    if(Channel == 0)
    {
        smsLog(pMac, LOGW, "   CSR cannot find a channel to start IBSS");
    }
    else
    {
  
        csrRoamDetermineMaxRateForAdHoc( pMac, &pSession->bssParams.operationalRateSet );
        if (CSR_IS_INFRA_AP(pProfile) || CSR_IS_START_IBSS( pProfile ) )
        {
            if(CSR_IS_CHANNEL_24GHZ(Channel) )
            {
                cbMode = pMac->roam.configParam.channelBondingMode24GHz;
            }
            else
            {
                cbMode = pMac->roam.configParam.channelBondingMode5GHz;
            }
            smsLog(pMac, LOG1, "## cbMode %d", cbMode);
            pBssConfig->cbMode = cbMode;
            pSession->bssParams.cbMode = cbMode;
        }
    }
}

static eHalStatus csrRoamStartIbss( tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, 
                                    tANI_BOOLEAN *pfSameIbss )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_BOOLEAN fSameIbss = FALSE;
     
    if ( csrIsConnStateIbss( pMac, sessionId ) ) 
    { 
        //                                                                      
        //                                                                      
        if ( csrIsSameProfile( pMac, &pMac->roam.roamSession[sessionId].connectedProfile, pProfile ) ) 
        {
            fSameIbss = TRUE;
        }
        else
        {
            status = csrRoamIssueStopBss( pMac, sessionId, eCSR_ROAM_SUBSTATE_DISCONNECT_CONTINUE_ROAMING );
        }       
    }
    else if ( csrIsConnStateConnectedInfra( pMac, sessionId ) ) 
    {
        //                                                          
        status = csrRoamIssueDisassociate( pMac, sessionId, eCSR_ROAM_SUBSTATE_DISCONNECT_CONTINUE_ROAMING, FALSE );
    }
    else 
    {
        tBssConfigParam *pBssConfig;
        
        status = palAllocateMemory(pMac->hHdd, (void **)&pBssConfig, sizeof(tBssConfigParam)); 
        if(HAL_STATUS_SUCCESS(status))
        {
            palZeroMemory(pMac->hHdd, pBssConfig, sizeof(tBssConfigParam));
            //                                                                        
            //                                                   
            status = csrRoamPrepareBssConfigFromProfile(pMac, pProfile, pBssConfig, NULL);
            if(HAL_STATUS_SUCCESS(status))
            {
                //            
                pMac->roam.roamSession[sessionId].bssParams.uCfgDot11Mode = pBssConfig->uCfgDot11Mode;
                //                                          
                csrRoamPrepareBssParams(pMac, sessionId, pProfile, NULL, pBssConfig, NULL);
                status = csrRoamSetBssConfigCfg(pMac, sessionId, pProfile,
                                                NULL, pBssConfig,
                                                NULL, eANI_BOOLEAN_FALSE);
            }
            palFreeMemory(pMac->hHdd, pBssConfig);
        }//               
    }
    
    if(pfSameIbss)
    {
        *pfSameIbss = fSameIbss;
    }
    return( status );
}

static void csrRoamUpdateConnectedProfileFromNewBss( tpAniSirGlobal pMac, tANI_U32 sessionId, 
                                                     tSirSmeNewBssInfo *pNewBss )
{
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return;
    }
    
    if( pNewBss )
    {
        //                           
        pSession->connectedProfile.operationChannel = pNewBss->channelNumber;
        //                                                                              
        palCopyMemory( pMac->hHdd, &pSession->connectedProfile.bssid, 
                      &(pNewBss->bssId), sizeof( tCsrBssid ) );    
    }
    return;
}

#ifdef FEATURE_WLAN_WAPI
eHalStatus csrRoamSetBKIDCache( tpAniSirGlobal pMac, tANI_U32 sessionId, tBkidCacheInfo *pBKIDCache,
                                 tANI_U32 numItems )
{
   eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
   tCsrRoamSession *pSession;
   if(!CSR_IS_SESSION_VALID( pMac, sessionId ))
   {
       smsLog(pMac, LOGE, FL("  Invalid session ID"));
       return status;
   }
   smsLog(pMac, LOGW, "csrRoamSetBKIDCache called, numItems = %d", numItems);
   pSession = CSR_GET_SESSION( pMac, sessionId );
   if(numItems <= CSR_MAX_BKID_ALLOWED)
   {
       status = eHAL_STATUS_SUCCESS;
       //                                    
       pSession->NumBkidCache = (tANI_U16)numItems;
       if(numItems && pBKIDCache)
       {
           status = palCopyMemory( pMac->hHdd, pSession->BkidCacheInfo, pBKIDCache,
                           sizeof(tBkidCacheInfo) * numItems );
       }
   }
   return (status);
}
eHalStatus csrRoamGetBKIDCache(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pNum,
                                tBkidCacheInfo *pBkidCache)
{
   eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
   tCsrRoamSession *pSession;
   if(!CSR_IS_SESSION_VALID( pMac, sessionId ))
   {
       smsLog(pMac, LOGE, FL("  Invalid session ID"));
       return status;
   }
   pSession = CSR_GET_SESSION( pMac, sessionId );
   if(pNum && pBkidCache)
   {
       if(pSession->NumBkidCache == 0)
       {
           *pNum = 0;
           status = eHAL_STATUS_SUCCESS;
       }
       else if(*pNum >= pSession->NumBkidCache)
       {
           if(pSession->NumBkidCache > CSR_MAX_PMKID_ALLOWED)
           {
               smsLog(pMac, LOGE, FL("NumPmkidCache :%d is more than CSR_MAX_PMKID_ALLOWED, resetting to CSR_MAX_PMKID_ALLOWED"),
                 pSession->NumBkidCache);
               pSession->NumBkidCache = CSR_MAX_PMKID_ALLOWED;
           }
           palCopyMemory( pMac->hHdd, pBkidCache, pSession->BkidCacheInfo,
                           sizeof(tBkidCacheInfo) * pSession->NumBkidCache );
           *pNum = pSession->NumBkidCache;
           status = eHAL_STATUS_SUCCESS;
       }
   }
   return (status);
}
tANI_U32 csrRoamGetNumBKIDCache(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
   return (pMac->roam.roamSession[sessionId].NumBkidCache);
}
#endif /*                   */
eHalStatus csrRoamSetPMKIDCache( tpAniSirGlobal pMac, tANI_U32 sessionId,
                                 tPmkidCacheInfo *pPMKIDCache, tANI_U32 numItems )
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    
    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    smsLog(pMac, LOGW, "csrRoamSetPMKIDCache called, numItems = %d", numItems);
    if(numItems <= CSR_MAX_PMKID_ALLOWED)
    {
#ifdef FEATURE_WLAN_DIAG_SUPPORT_CSR
        {
            WLAN_VOS_DIAG_EVENT_DEF(secEvent, vos_event_wlan_security_payload_type);
            palZeroMemory(pMac->hHdd, &secEvent, sizeof(vos_event_wlan_security_payload_type));
            secEvent.eventId = WLAN_SECURITY_EVENT_PMKID_UPDATE;
            secEvent.encryptionModeMulticast = 
                (v_U8_t)diagEncTypeFromCSRType(pSession->connectedProfile.mcEncryptionType);
            secEvent.encryptionModeUnicast = 
                (v_U8_t)diagEncTypeFromCSRType(pSession->connectedProfile.EncryptionType);
            palCopyMemory( pMac->hHdd, secEvent.bssid, pSession->connectedProfile.bssid, 6 );
            secEvent.authMode = 
                (v_U8_t)diagAuthTypeFromCSRType(pSession->connectedProfile.AuthType);
            WLAN_VOS_DIAG_EVENT_REPORT(&secEvent, EVENT_WLAN_SECURITY);
        }
#endif//                             
        status = eHAL_STATUS_SUCCESS;
        //                                    
        pSession->NumPmkidCache = (tANI_U16)numItems;
        if(numItems && pPMKIDCache)
        {
            status = palCopyMemory( pMac->hHdd, pSession->PmkidCacheInfo, pPMKIDCache,
                            sizeof(tPmkidCacheInfo) * numItems );
        }
    }
    return (status);
}

eHalStatus csrRoamDelPMKIDfromCache( tpAniSirGlobal pMac, tANI_U32 sessionId,
                                 tANI_U8 *pBSSId )
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    tANI_BOOLEAN fMatchFound = FALSE;
    tANI_U32 Index;
    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    do
    {
        for( Index=0; Index < pSession->NumPmkidCache; Index++ )
        {
            smsLog(pMac, LOGW, "Delete PMKID for "
                   MAC_ADDRESS_STR, MAC_ADDR_ARRAY(pBSSId));
            if( palEqualMemory( pMac->hHdd, pBSSId, pSession->PmkidCacheInfo[Index].BSSID, sizeof(tCsrBssid) ) )
            {
                fMatchFound = TRUE;
                break;
            }
        }
        if( !fMatchFound ) break;
        palZeroMemory( pMac->hHdd, pSession->PmkidCacheInfo[Index].BSSID, sizeof(tCsrBssid));
        status = eHAL_STATUS_SUCCESS;
    }
    while( 0 );
    smsLog(pMac, LOGW, "csrDelPMKID called return match = %d Status = %d",
        fMatchFound, status);
    return status;
}
tANI_U32 csrRoamGetNumPMKIDCache(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    return (pMac->roam.roamSession[sessionId].NumPmkidCache);
}

eHalStatus csrRoamGetPMKIDCache(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pNum, tPmkidCacheInfo *pPmkidCache)
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    if(pNum && pPmkidCache)
    {
        if(pSession->NumPmkidCache == 0)
        {
            *pNum = 0;
            status = eHAL_STATUS_SUCCESS;
        }
        else if(*pNum >= pSession->NumPmkidCache)
        {
            if(pSession->NumPmkidCache > CSR_MAX_PMKID_ALLOWED)
            {
                smsLog(pMac, LOGE, FL("NumPmkidCache :%d is more than CSR_MAX_PMKID_ALLOWED, resetting to CSR_MAX_PMKID_ALLOWED"),
                  pSession->NumPmkidCache);
                pSession->NumPmkidCache = CSR_MAX_PMKID_ALLOWED;
            }
            palCopyMemory( pMac->hHdd, pPmkidCache, pSession->PmkidCacheInfo,
                            sizeof(tPmkidCacheInfo) * pSession->NumPmkidCache );
            *pNum = pSession->NumPmkidCache;
            status = eHAL_STATUS_SUCCESS;
        }
    }
    return (status);
}

eHalStatus csrRoamGetWpaRsnReqIE(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pLen, tANI_U8 *pBuf)
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
    tANI_U32 len;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    
    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    if(pLen)
    {
        len = *pLen;
        *pLen = pSession->nWpaRsnReqIeLength;
        if(pBuf)
        {
            if(len >= pSession->nWpaRsnReqIeLength)
            {
                status = palCopyMemory(pMac->hHdd, pBuf, pSession->pWpaRsnReqIE, pSession->nWpaRsnReqIeLength);
            }
        }
    }
    return (status);
}

eHalStatus csrRoamGetWpaRsnRspIE(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pLen, tANI_U8 *pBuf)
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
    tANI_U32 len;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    if(pLen)
    {
        len = *pLen;
        *pLen = pSession->nWpaRsnRspIeLength;
        if(pBuf)
        {
            if(len >= pSession->nWpaRsnRspIeLength)
            {
                status = palCopyMemory(pMac->hHdd, pBuf, pSession->pWpaRsnRspIE, pSession->nWpaRsnRspIeLength);
            }
        }
    }
    return (status);
}
#ifdef FEATURE_WLAN_WAPI
eHalStatus csrRoamGetWapiReqIE(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pLen, tANI_U8 *pBuf)
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
    tANI_U32 len;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    if(pLen)
    {
        len = *pLen;
        *pLen = pSession->nWapiReqIeLength;
        if(pBuf)
        {
            if(len >= pSession->nWapiReqIeLength)
            {
                status = palCopyMemory(pMac->hHdd, pBuf, pSession->pWapiReqIE, pSession->nWapiReqIeLength);
            }
        }
    }
    return (status);
}
eHalStatus csrRoamGetWapiRspIE(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pLen, tANI_U8 *pBuf)
{
    eHalStatus status = eHAL_STATUS_INVALID_PARAMETER;
    tANI_U32 len;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    if(pLen)
    {
        len = *pLen;
        *pLen = pSession->nWapiRspIeLength;
        if(pBuf)
        {
            if(len >= pSession->nWapiRspIeLength)
            {
                status = palCopyMemory(pMac->hHdd, pBuf, pSession->pWapiRspIE, pSession->nWapiRspIeLength);
            }
        }
    }
    return (status);
}
#endif /*                   */
eRoamCmdStatus csrGetRoamCompleteStatus(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    eRoamCmdStatus retStatus = eCSR_ROAM_CONNECT_COMPLETION;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return (retStatus);
    }
    
    if(CSR_IS_ROAMING(pSession))
    {
        retStatus = eCSR_ROAM_ROAMING_COMPLETION;
        pSession->fRoaming = eANI_BOOLEAN_FALSE;
    }
    return (retStatus);
}

//                                                                 
eHalStatus csrRoamRemoveConnectedBssFromScanCache(tpAniSirGlobal pMac,
                                                  tCsrRoamConnectedProfile *pConnProfile)
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    tCsrScanResultFilter *pScanFilter = NULL;
    tListElem *pEntry;
    tCsrScanResult *pResult;
        tDot11fBeaconIEs *pIes;
    tANI_BOOLEAN fMatch;
    if(!(csrIsMacAddressZero(pMac, &pConnProfile->bssid) ||
            csrIsMacAddressBroadcast(pMac, &pConnProfile->bssid)))
    {
        do
        {
            //                                                                                
            status = palAllocateMemory(pMac->hHdd, (void **)&pScanFilter, sizeof(tCsrScanResultFilter));
            if(!HAL_STATUS_SUCCESS(status)) break;
            palZeroMemory(pMac->hHdd, pScanFilter, sizeof(tCsrScanResultFilter));
            status = palAllocateMemory(pMac->hHdd, (void **)&pScanFilter->BSSIDs.bssid, sizeof(tCsrBssid));
            if(!HAL_STATUS_SUCCESS(status)) break;
            palCopyMemory(pMac->hHdd, pScanFilter->BSSIDs.bssid, &pConnProfile->bssid, sizeof(tCsrBssid));
            pScanFilter->BSSIDs.numOfBSSIDs = 1;
            if(!csrIsNULLSSID(pConnProfile->SSID.ssId, pConnProfile->SSID.length))
            {
                status = palAllocateMemory(pMac->hHdd, (void **)&pScanFilter->SSIDs.SSIDList, sizeof(tCsrSSIDInfo));
                if(!HAL_STATUS_SUCCESS(status)) break;
                palCopyMemory(pMac->hHdd, &pScanFilter->SSIDs.SSIDList[0].SSID, &pConnProfile->SSID, sizeof(tSirMacSSid));
            }
            pScanFilter->authType.numEntries = 1;
            pScanFilter->authType.authType[0] = pConnProfile->AuthType;
            pScanFilter->BSSType = pConnProfile->BSSType;
            pScanFilter->EncryptionType.numEntries = 1;
            pScanFilter->EncryptionType.encryptionType[0] = pConnProfile->EncryptionType;
            pScanFilter->mcEncryptionType.numEntries = 1;
            pScanFilter->mcEncryptionType.encryptionType[0] = pConnProfile->mcEncryptionType;
            //                                                     
            pScanFilter->ChannelInfo.numOfChannels = 0;
            //                                
            pScanFilter->uapsd_mask = 0;
            pScanFilter->bWPSAssociation = eANI_BOOLEAN_FALSE;
            pScanFilter->countryCode[0] = 0;
            pScanFilter->phyMode = eCSR_DOT11_MODE_TAURUS;
            csrLLLock(&pMac->scan.scanResultList);
            pEntry = csrLLPeekHead( &pMac->scan.scanResultList, LL_ACCESS_NOLOCK );
            while( pEntry ) 
            {
                pResult = GET_BASE_ADDR( pEntry, tCsrScanResult, Link );
                                pIes = (tDot11fBeaconIEs *)( pResult->Result.pvIes );
                fMatch = csrMatchBSS(pMac, &pResult->Result.BssDescriptor, 
                               pScanFilter, NULL, NULL, NULL, &pIes);
                //                                                  
                if( !pResult->Result.pvIes )
                {
                    //                                                         
                    palFreeMemory(pMac->hHdd, pIes);
                }
                if(fMatch)
                {
                    //                
                    if( csrLLRemoveEntry(&pMac->scan.scanResultList, pEntry, LL_ACCESS_NOLOCK) )
                    {
                        //               
                        csrFreeScanResultEntry( pMac, pResult );
                    }
                    break;
                }
                pEntry = csrLLNext(&pMac->scan.scanResultList, pEntry, LL_ACCESS_NOLOCK);
            }//     
            csrLLUnlock(&pMac->scan.scanResultList);
        }while(0);
        if(pScanFilter)
        {
            csrFreeScanFilter(pMac, pScanFilter);
            palFreeMemory(pMac->hHdd, pScanFilter);
        }
    }
    return (status);
}

//      
eHalStatus csrIsBTAMPAllowed( tpAniSirGlobal pMac, tANI_U32 chnId )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_U32 sessionId;
    for( sessionId = 0; sessionId < CSR_ROAM_SESSION_MAX; sessionId++ )
    {
        if( CSR_IS_SESSION_VALID( pMac, sessionId ) )
        {
            if( csrIsConnStateIbss( pMac, sessionId ) || csrIsBTAMP( pMac, sessionId ) )
            {
                //                                             
                smsLog( pMac, LOGW, " BTAMP is not allowed due to IBSS/BT-AMP exist in session %d", sessionId );
                status = eHAL_STATUS_CSR_WRONG_STATE;
                break;
            }
            if( csrIsConnStateInfra( pMac, sessionId ) )
            {
                if( chnId && 
                    ( (tANI_U8)chnId != pMac->roam.roamSession[sessionId].connectedProfile.operationChannel ) )
                {
                    smsLog( pMac, LOGW, " BTAMP is not allowed due to channel (%d) diff than infr channel (%d)",
                        chnId, pMac->roam.roamSession[sessionId].connectedProfile.operationChannel );
                    status = eHAL_STATUS_CSR_WRONG_STATE;
                    break;
                }
            }
        }
    }
    return ( status );
}

static eHalStatus csrRoamStartWds( tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile, tSirBssDescription *pBssDesc )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    tBssConfigParam bssConfig;

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    if ( csrIsConnStateIbss( pMac, sessionId ) ) 
    { 
        status = csrRoamIssueStopBss( pMac, sessionId, eCSR_ROAM_SUBSTATE_DISCONNECT_CONTINUE_ROAMING );
    }
    else if ( csrIsConnStateConnectedInfra( pMac, sessionId ) ) 
    {
        //                                                          
        status = csrRoamIssueDisassociate( pMac, sessionId, eCSR_ROAM_SUBSTATE_DISCONNECT_CONTINUE_ROAMING, FALSE );
    }
    else
    {
        //                                                                                     
        //                                           
        //                                                                                            
        //                                            
        VOS_ASSERT( !csrIsConnStateWds( pMac, sessionId ) );
        palZeroMemory(pMac->hHdd, &bssConfig, sizeof(tBssConfigParam));
        /*                                     */
        palCopyMemory( pMac->hHdd, &pSession->bssParams.bssid, pProfile->BSSIDs.bssid[0], sizeof(tCsrBssid) );
        //                                                              
        //                                                            
        status = csrRoamPrepareBssConfigFromProfile(pMac, pProfile, &bssConfig, pBssDesc);
        if(HAL_STATUS_SUCCESS(status))
        {
            //                         
            csrFreeRoamProfile( pMac, sessionId );
            if (HAL_STATUS_SUCCESS(palAllocateMemory(pMac->hHdd,
                                   (void **)&pSession->pCurRoamProfile,
                                   sizeof(tCsrRoamProfile))))
            {
                palZeroMemory(pMac->hHdd, pSession->pCurRoamProfile, sizeof(tCsrRoamProfile));
                csrRoamCopyProfile(pMac, pSession->pCurRoamProfile, pProfile);
            }
            //                                         
            csrRoamPrepareBssParams(pMac, sessionId, pProfile, NULL, &bssConfig, NULL);
            status = csrRoamSetBssConfigCfg(pMac, sessionId, pProfile,
                                            NULL, &bssConfig,
                                            NULL, eANI_BOOLEAN_FALSE);
        }
    }

    return( status );
}

//                          

//                                                                                                      
//                                                                   
static void csrPrepareJoinReassocReqBuffer( tpAniSirGlobal pMac,
                                            tSirBssDescription *pBssDescription,
                                            tANI_U8 *pBuf, tANI_U8 uapsdMask)
{
    tCsrChannelSet channelGroup;
    tSirMacCapabilityInfo *pAP_capabilityInfo;
    tAniBool fTmp;
    tANI_BOOLEAN found = FALSE;
    tANI_U32 size = 0;
    tANI_S8 pwrLimit = 0;
    tANI_U16 i;
    //                                                                                  
    *pBuf++ = (tANI_U8)FALSE;  //                                      
    *pBuf++ = (tANI_U8)FALSE;  //                                          
    *pBuf++ = (tANI_U8)FALSE;  //                                        
    //        
    //                                                       
    pAP_capabilityInfo = (tSirMacCapabilityInfo *)&pBssDescription->capabilityInfo;
    //                                                                                                          
    if ( csrIs11hSupported( pMac ) && pAP_capabilityInfo->spectrumMgt && eSIR_11A_NW_TYPE == pBssDescription->nwType )
    {
        fTmp = (tAniBool)pal_cpu_to_be32(1);
    }
    else
        fTmp = (tAniBool)0;
   
    //                                                    
    palCopyMemory( pMac->hHdd, pBuf, (tANI_U8 *)&fTmp, sizeof(tAniBool) );
    pBuf += sizeof(tAniBool);
    *pBuf++ = MIN_STA_PWR_CAP_DBM; //                                         
    found = csrSearchChannelListForTxPower(pMac, pBssDescription, &channelGroup);
    //                                                  
    //                                               
    //                                                       
    //                                                     
    pwrLimit = csrGetCfgMaxTxPower (pMac, pBssDescription->channelId);
    if (0 != pwrLimit)
    {
        *pBuf++ = pwrLimit;
    }
    else
    {
        *pBuf++ = MAX_STA_PWR_CAP_DBM;
    }
    size = sizeof(pMac->roam.validChannelList);
    if(HAL_STATUS_SUCCESS(csrGetCfgValidChannels(pMac, (tANI_U8 *)pMac->roam.validChannelList, &size)))
    { 
        *pBuf++ = (tANI_U8)size;        //                            
        for ( i = 0; i < size; i++) 
        {
            *pBuf++ = pMac->roam.validChannelList[ i ];   //                             
             
        }
    }
    else
    {
        smsLog(pMac, LOGE, FL("can not find any valid channel"));
        *pBuf++ = 0;  //                    
    }                                                                                                                     
    //                                     
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
    if( btcIsReadyForUapsd(pMac) )
#endif /*                            */
    {
       *pBuf++ = uapsdMask;
    }
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
    else
    {
        smsLog(pMac, LOGE, FL(" BTC doesn't allow UAPSD for uapsd_mask(0x%X)"), uapsdMask);
        *pBuf++ = 0;
    }
#endif /*                            */
  
    //                                                      
    palCopyMemory( pMac->hHdd, pBuf, pBssDescription, 
                    pBssDescription->length + sizeof( pBssDescription->length ) );
    pBuf += pBssDescription->length + sizeof( pBssDescription->length );   //                       
}

/* 
                                                               
                                                               
                                                                                                                                           
                                                                                                                                        
                                                                                                                                                
                                                                                                                        
  */
eHalStatus csrSendJoinReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirBssDescription *pBssDescription, 
                              tCsrRoamProfile *pProfile, tDot11fBeaconIEs *pIes, tANI_U16 messageType )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirSmeJoinReq *pMsg;
    tANI_U8 *pBuf;
    v_U8_t acm_mask = 0, uapsd_mask;
    tANI_U16 msgLen, wTmp, ieLen;
    tSirMacRateSet OpRateSet;
    tSirMacRateSet ExRateSet;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    tANI_U32 dwTmp;
    tANI_U8 wpaRsnIE[DOT11F_IE_RSN_MAX_LEN];    //                              
    tANI_U32 ucDot11Mode = 0;

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    /*                       */
    if (NULL == pBssDescription)
    {
        smsLog(pMac, LOGE, FL(" pBssDescription is NULL"));
        return eHAL_STATUS_FAILURE;
    }

    do {
        pSession->joinFailStatusCode.statusCode = eSIR_SME_SUCCESS;
        pSession->joinFailStatusCode.reasonCode = 0;
        memcpy (&pSession->joinFailStatusCode.bssId, &pBssDescription->bssId, sizeof(tSirMacAddr));
        //                                                                                     
        //                                                                                       
        //                                                                                  
        //                                   
        //
        //                                                                               
        //                                                                                   
        //                                                          
        msgLen = sizeof( tSirSmeJoinReq ) - sizeof( *pBssDescription ) + 
            pBssDescription->length + sizeof( pBssDescription->length ) +
            sizeof( tCsrWpaIe ) + sizeof( tCsrWpaAuthIe ) + sizeof( tANI_U16 ); //                                                 
        status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, msgLen);
        if ( !HAL_STATUS_SUCCESS(status) ) break;
        palZeroMemory(pMac->hHdd, pMsg, msgLen);
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)messageType);
        pMsg->length = pal_cpu_to_be16(msgLen);
        pBuf = &pMsg->sessionId;
        //          
        *pBuf = (tANI_U8)sessionId;
        pBuf++;
        //              
        *pBuf = 0;
        *( pBuf + 1 ) = 0;
        pBuf += sizeof(tANI_U16);
        //     
        if( pIes->SSID.present && pIes->SSID.num_ssid )
        {
            //         
            *pBuf = pIes->SSID.num_ssid;
            pBuf++;
            palCopyMemory( pMac->hHdd, pBuf, pIes->SSID.ssid, pIes->SSID.num_ssid );
            pBuf += pIes->SSID.num_ssid;
        }
        else
        {
            *pBuf = 0;
            pBuf++;
        }
        //            
        palCopyMemory( pMac->hHdd, (tSirMacAddr *)pBuf, &pSession->selfMacAddr, sizeof(tSirMacAddr) );
        pBuf += sizeof(tSirMacAddr);
        //        
        dwTmp = pal_cpu_to_be32( csrTranslateBsstypeToMacType( pProfile->BSSType ) );
        if (dwTmp == eSIR_BTAMP_STA_MODE) dwTmp = eSIR_BTAMP_AP_MODE; //                           
        palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tSirBssType) );
        pBuf += sizeof(tSirBssType);
        //          
        ucDot11Mode = csrTranslateToWNICfgDot11Mode( pMac, pSession->bssParams.uCfgDot11Mode );
        if (pBssDescription->channelId <= 14 &&
            FALSE == pMac->roam.configParam.enableVhtFor24GHz &&
            WNI_CFG_DOT11_MODE_11AC == ucDot11Mode)
        {
            //                                             
            ucDot11Mode = WNI_CFG_DOT11_MODE_11N;
        }
        *pBuf = (tANI_U8)ucDot11Mode;
        pBuf++;
        //       
        *pBuf = (tANI_U8)pProfile->csrPersona;
        pBuf++;
        //      
        *pBuf = (tANI_U8)pSession->bssParams.cbMode;
        pBuf++;

        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO,
                  FL("CSR PERSONA=%d CSR CbMode %d"), pProfile->csrPersona, pSession->bssParams.cbMode);

        //                  
        *pBuf = pProfile->uapsd_mask;
        pBuf++;



        status = csrGetRateSet(pMac, pProfile, (eCsrPhyMode)pProfile->phyMode, pBssDescription, pIes, &OpRateSet, &ExRateSet);
        if (HAL_STATUS_SUCCESS(status) )
        {
            //                   
            if (OpRateSet.numRates) {
                *pBuf++ = OpRateSet.numRates;
                palCopyMemory(pMac->hHdd, pBuf, OpRateSet.rate, OpRateSet.numRates);
                pBuf += OpRateSet.numRates;
            } else *pBuf++ = 0;
            //                
            if (ExRateSet.numRates) {
                *pBuf++ = ExRateSet.numRates;
                palCopyMemory(pMac->hHdd, pBuf, ExRateSet.rate, ExRateSet.numRates);
                pBuf += ExRateSet.numRates;
            } else *pBuf++ = 0;
        }
        else
        {
            *pBuf++ = 0;
            *pBuf++ = 0;
        }
        //      
        if ( csrIsProfileWpa( pProfile ) )
        {
            //                                        
            ieLen = csrRetrieveWpaIe( pMac, pProfile, pBssDescription, pIes,
                    (tCsrWpaIe *)( wpaRsnIE ) );
        }
        else if( csrIsProfileRSN( pProfile ) )
        {
            //                                        
            ieLen = csrRetrieveRsnIe( pMac, sessionId, pProfile, pBssDescription, pIes,
                    (tCsrRSNIe *)( wpaRsnIE ) );
        }
#ifdef FEATURE_WLAN_WAPI
        else if( csrIsProfileWapi( pProfile ) )
        {
            //                                         
            ieLen = csrRetrieveWapiIe( pMac, sessionId, pProfile, pBssDescription, pIes,
                    (tCsrWapiIe *)( wpaRsnIE ) );
        }
#endif /*                   */
        else
        {
            ieLen = 0;
        }
        //                              
        if( ieLen )
        {
            if(ieLen > DOT11F_IE_RSN_MAX_LEN)
            {
                smsLog(pMac, LOGE, FL(" WPA RSN IE length :%d is more than DOT11F_IE_RSN_MAX_LEN, resetting to %d"), ieLen, DOT11F_IE_RSN_MAX_LEN);
                ieLen = DOT11F_IE_RSN_MAX_LEN;
            }
#ifdef FEATURE_WLAN_WAPI
            if( csrIsProfileWapi( pProfile ) )
            {
                //                                             
                if(ieLen > pSession->nWapiReqIeLength)
                {
                    if(pSession->pWapiReqIE && pSession->nWapiReqIeLength)
                    {
                        palFreeMemory(pMac->hHdd, pSession->pWapiReqIE);
                    }
                    status = palAllocateMemory(pMac->hHdd, (void **)&pSession->pWapiReqIE, ieLen);
                    if(!HAL_STATUS_SUCCESS(status)) break;
                }
                pSession->nWapiReqIeLength = ieLen;
                palCopyMemory(pMac->hHdd, pSession->pWapiReqIE, wpaRsnIE, ieLen);
                wTmp = pal_cpu_to_be16( ieLen );
                palCopyMemory( pMac->hHdd, pBuf, &wTmp, sizeof(tANI_U16) );
                pBuf += sizeof(tANI_U16);
                palCopyMemory( pMac->hHdd, pBuf, wpaRsnIE, ieLen );
                pBuf += ieLen;
            }
            else//                            
#endif /*                   */
            {
                //                                             
                if(ieLen > pSession->nWpaRsnReqIeLength)
                {
                    if(pSession->pWpaRsnReqIE && pSession->nWpaRsnReqIeLength)
                    {
                        palFreeMemory(pMac->hHdd, pSession->pWpaRsnReqIE);
                    }
                    status = palAllocateMemory(pMac->hHdd, (void **)&pSession->pWpaRsnReqIE, ieLen);
                    if(!HAL_STATUS_SUCCESS(status)) break;
                }
                pSession->nWpaRsnReqIeLength = ieLen;
                palCopyMemory(pMac->hHdd, pSession->pWpaRsnReqIE, wpaRsnIE, ieLen);
                wTmp = pal_cpu_to_be16( ieLen );
                palCopyMemory( pMac->hHdd, pBuf, &wTmp, sizeof(tANI_U16) );
                pBuf += sizeof(tANI_U16);
                palCopyMemory( pMac->hHdd, pBuf, wpaRsnIE, ieLen );
                pBuf += ieLen;
            }
        }
        else
        {
            //                      
            pSession->nWpaRsnReqIeLength = 0;
            if(pSession->pWpaRsnReqIE)
            {
                palFreeMemory(pMac->hHdd, pSession->pWpaRsnReqIE);
                pSession->pWpaRsnReqIE = NULL;
            }
#ifdef FEATURE_WLAN_WAPI
            pSession->nWapiReqIeLength = 0;
            if(pSession->pWapiReqIE)
            {
                palFreeMemory(pMac->hHdd, pSession->pWapiReqIE);
                pSession->pWapiReqIE = NULL;
            }
#endif /*                   */
            //                   
            *pBuf = 0;
            *(pBuf + 1) = 0;
            pBuf += 2;
        }
#ifdef FEATURE_WLAN_CCX
        if( eWNI_SME_JOIN_REQ == messageType )
        {
            //                                            
            //                   
            *pBuf = 0;
            *(pBuf + 1) = 0;
            pBuf += 2;
        }
        else if(eWNI_SME_REASSOC_REQ == messageType )
        {
            //       
            if( csrIsProfileCCX( pProfile ) )
            {
                //                                         
#ifdef FEATURE_WLAN_CCX_UPLOAD
                ieLen = pSession->suppCckmIeInfo.cckmIeLen;
                palCopyMemory(pMac->hHdd, (void *) (wpaRsnIE),
                     pSession->suppCckmIeInfo.cckmIe, ieLen);
#else
                ieLen = csrConstructCcxCckmIe( pMac,
                                          pSession,
                                          pProfile,
                                          pBssDescription,
                                          pSession->pWpaRsnReqIE,
                                          pSession->nWpaRsnReqIeLength,
                                          (void *)( wpaRsnIE ) );
#endif /*                         */
            }
            else
            {
                ieLen = 0;
            }
            //                                                                    
            if( ieLen )
            {
                //                                                     
                wTmp = pal_cpu_to_be16( ieLen );
                palCopyMemory( pMac->hHdd, pBuf, &wTmp, sizeof(tANI_U16) );
                pBuf += sizeof(tANI_U16);
                palCopyMemory( pMac->hHdd, pBuf, wpaRsnIE, ieLen );
                pBuf += ieLen;
            }
            else
            {
                //                            
                //                   
                *pBuf = 0;
                *(pBuf + 1) = 0;
                pBuf += 2;
            }
        }
#endif /*                  */
        //          
        if(pProfile->nAddIEScanLength && pProfile->pAddIEScan)
        {
            ieLen = pProfile->nAddIEScanLength;
            if(ieLen > pSession->nAddIEScanLength)
            {
                if(pSession->pAddIEScan && pSession->nAddIEScanLength)
                {
                    palFreeMemory(pMac->hHdd, pSession->pAddIEScan);
                }
                status = palAllocateMemory(pMac->hHdd,
                                 (void **)&pSession->pAddIEScan, ieLen);
                if(!HAL_STATUS_SUCCESS(status)) break;
            }
            pSession->nAddIEScanLength = ieLen;
            palCopyMemory(pMac->hHdd, pSession->pAddIEScan, 
                                      pProfile->pAddIEScan, ieLen);
            wTmp = pal_cpu_to_be16( ieLen );
            palCopyMemory( pMac->hHdd, pBuf, &wTmp, sizeof(tANI_U16) );
            pBuf += sizeof(tANI_U16);
            palCopyMemory( pMac->hHdd, pBuf, pProfile->pAddIEScan, ieLen );
            pBuf += ieLen;
        }
        else
        {
            pSession->nAddIEScanLength = 0;
            if(pSession->pAddIEScan)
            {
                palFreeMemory(pMac->hHdd, pSession->pAddIEScan);
                pSession->pAddIEScan = NULL;
            }
            *pBuf = 0;
            *(pBuf + 1) = 0;
            pBuf += 2;
        }
        //           
        if(pProfile->nAddIEAssocLength && pProfile->pAddIEAssoc)
        {
            ieLen = pProfile->nAddIEAssocLength;
            if(ieLen > pSession->nAddIEAssocLength)
            {
                if(pSession->pAddIEAssoc && pSession->nAddIEAssocLength)
        {
                    palFreeMemory(pMac->hHdd, pSession->pAddIEAssoc);
        }
                status = palAllocateMemory(pMac->hHdd,
                                 (void **)&pSession->pAddIEAssoc, ieLen);
                if(!HAL_STATUS_SUCCESS(status)) break;
        }
            pSession->nAddIEAssocLength = ieLen;
            palCopyMemory(pMac->hHdd, pSession->pAddIEAssoc,
                                      pProfile->pAddIEAssoc, ieLen);
            wTmp = pal_cpu_to_be16( ieLen );
            palCopyMemory( pMac->hHdd, pBuf, &wTmp, sizeof(tANI_U16) );
            pBuf += sizeof(tANI_U16);
            palCopyMemory( pMac->hHdd, pBuf, pProfile->pAddIEAssoc, ieLen );
            pBuf += ieLen;
        }
        else
        {
            pSession->nAddIEAssocLength = 0;
            if(pSession->pAddIEAssoc)
            {
                palFreeMemory(pMac->hHdd, pSession->pAddIEAssoc);
                pSession->pAddIEAssoc = NULL;
            }
            *pBuf = 0;
            *(pBuf + 1) = 0;
            pBuf += 2;
        }

        if(eWNI_SME_REASSOC_REQ == messageType )
        {
            //                                        
            uapsd_mask = (v_U8_t)pProfile->uapsd_mask;
            if( uapsd_mask && ( NULL != pBssDescription ) )
            {
                if( CSR_IS_QOS_BSS(pIes) && CSR_IS_UAPSD_BSS(pIes) )
                {
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
                    acm_mask = sme_QosGetACMMask(pMac, pBssDescription, pIes);
#endif /*                            */
                }
                else
                {
                    uapsd_mask = 0;
                }
            }
        }

        dwTmp = pal_cpu_to_be32( csrTranslateEncryptTypeToEdType( pProfile->negotiatedUCEncryptionType) );
        palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tANI_U32) );
        pBuf += sizeof(tANI_U32);

        dwTmp = pal_cpu_to_be32( csrTranslateEncryptTypeToEdType( pProfile->negotiatedMCEncryptionType) );
        palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tANI_U32) );
        pBuf += sizeof(tANI_U32);
#ifdef WLAN_FEATURE_11W
        //              
        if (pProfile->MFPEnabled)
        {
            dwTmp = pal_cpu_to_be32(eSIR_ED_AES_128_CMAC);
        }
        else
        {
            dwTmp = pal_cpu_to_be32(eSIR_ED_NONE);
        }
        palCopyMemory(pMac->hHdd, pBuf, &dwTmp, sizeof(tANI_U32));
        pBuf += sizeof(tANI_U32);
#endif
#ifdef WLAN_FEATURE_VOWIFI_11R
        pProfile->MDID.mdiePresent = pBssDescription->mdiePresent;
        if (csrIsProfile11r( pProfile )
#ifdef FEATURE_WLAN_CCX
           && !((pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_OPEN_SYSTEM) &&
                (pIes->CCXVersion.present) && (pMac->roam.configParam.isCcxIniFeatureEnabled))
#endif
        )
        {
            //                 
            dwTmp = pal_cpu_to_be32(TRUE);
            palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tAniBool) );
            pBuf += sizeof(tAniBool);
        }
        else
        {
            //                 
            dwTmp = pal_cpu_to_be32(FALSE);
            palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tAniBool) );
            pBuf += sizeof(tAniBool);
        }
#endif
#ifdef FEATURE_WLAN_CCX

        //                       
        if (TRUE == pMac->roam.configParam.isCcxIniFeatureEnabled)
        {
            dwTmp = pal_cpu_to_be32(TRUE);
            palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tAniBool) );
            pBuf += sizeof(tAniBool);
        }
        else
        {
            dwTmp = pal_cpu_to_be32(FALSE);
            palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tAniBool) );
            pBuf += sizeof(tAniBool);
        }

        /*                                                         
                                                                   
                                                                   
                                                                    
               
         */
        if ((csrIsProfileCCX(pProfile) ||
                  ((pIes->CCXVersion.present)
                   && ((pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_OPEN_SYSTEM)
                       || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_WPA)
                       || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_WPA_PSK)
                       || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_RSN)
#ifdef WLAN_FEATURE_11W
                       || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_RSN_PSK_SHA256)
#endif
                       || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_RSN_PSK))))
                 && (pMac->roam.configParam.isCcxIniFeatureEnabled))
        {
            //                 
            dwTmp = pal_cpu_to_be32(TRUE);
            palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tAniBool) );
            pBuf += sizeof(tAniBool);
        }
        else
        {
            //                
            dwTmp = pal_cpu_to_be32(FALSE);
            palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tAniBool) );
            pBuf += sizeof(tAniBool);
        }

        if (eWNI_SME_JOIN_REQ == messageType)
        {
            tCCXTspecInfo ccxTspec;
            //                                                              
            //                                
            palZeroMemory(pMac->hHdd, &ccxTspec, sizeof(tCCXTspecInfo));
            palCopyMemory( pMac->hHdd, pBuf, &ccxTspec, sizeof(tCCXTspecInfo));
            pBuf += sizeof(tCCXTspecInfo);
        }
        else if (eWNI_SME_REASSOC_REQ == messageType)
        {
        if ((csrIsProfileCCX(pProfile) ||
             ((pIes->CCXVersion.present)
              && ((pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_OPEN_SYSTEM)
                  || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_WPA)
                  || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_WPA_PSK)
                  || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_RSN)
#ifdef WLAN_FEATURE_11W
                  || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_RSN_PSK_SHA256)
#endif
                  || (pProfile->negotiatedAuthType == eCSR_AUTH_TYPE_RSN_PSK))))
            && (pMac->roam.configParam.isCcxIniFeatureEnabled))
        {
           tCCXTspecInfo ccxTspec;
           //                      
           palZeroMemory(pMac->hHdd, &ccxTspec, sizeof(tCCXTspecInfo));
           ccxTspec.numTspecs = sme_QosCCxRetrieveTspecInfo(pMac, sessionId, (tTspecInfo *) &ccxTspec.tspec[0]);
           *pBuf = ccxTspec.numTspecs;
           pBuf += sizeof(tANI_U8);
           //                                           
           if (ccxTspec.numTspecs) {
               palCopyMemory(pMac->hHdd, pBuf, (void*)&ccxTspec.tspec[0], (ccxTspec.numTspecs*sizeof(tTspecInfo)));
           }
           pBuf += sizeof(ccxTspec.tspec);
        }
        else
        {
                tCCXTspecInfo ccxTspec;
                //                                                              
                //                                
                palZeroMemory(pMac->hHdd, &ccxTspec, sizeof(tCCXTspecInfo));
                palCopyMemory( pMac->hHdd, pBuf, &ccxTspec, sizeof(tCCXTspecInfo));
                pBuf += sizeof(tCCXTspecInfo);
            }
        }
#endif //                 
#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)
        //                                
        if (pMac->roam.configParam.isFastTransitionEnabled
#ifdef FEATURE_WLAN_LFR
         || csrRoamIsFastRoamEnabled(pMac, sessionId)
#endif
         )
        {
            dwTmp = pal_cpu_to_be32(TRUE);
            palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tAniBool) );
            pBuf += sizeof(tAniBool);
        }
        else
        {
            dwTmp = pal_cpu_to_be32(FALSE);
            palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tAniBool) );
            pBuf += sizeof(tAniBool);
        }
#endif
#ifdef FEATURE_WLAN_LFR
        if(csrRoamIsFastRoamEnabled(pMac, sessionId))
        {
            //                           
            dwTmp = pal_cpu_to_be32(TRUE);
            palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tAniBool) );
            pBuf += sizeof(tAniBool);
        }
        else
        {
            dwTmp = pal_cpu_to_be32(FALSE);
            palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tAniBool) );
            pBuf += sizeof(tAniBool);
        }
#endif

        //                        
        *pBuf = (tANI_U8)pMac->roam.configParam.txLdpcEnable;
        pBuf++;

        if ((csrIs11hSupported (pMac)) && (CSR_IS_CHANNEL_5GHZ(pBssDescription->channelId)) &&
            (pIes->Country.present) && (!pMac->roam.configParam.fSupplicantCountryCodeHasPriority))
        {
            csrSaveToChannelPower2G_5G( pMac, pIes->Country.num_triplets * sizeof(tSirMacChanInfo),
                                        (tSirMacChanInfo *)(&pIes->Country.triplets[0]) );
            csrApplyPower2Current(pMac);
        }

#ifdef WLAN_FEATURE_11AC
        //                      
        *pBuf = (tANI_U8)pMac->roam.configParam.txBFEnable;
        pBuf++;

        //             
        *pBuf = (tANI_U8)pMac->roam.configParam.txBFCsnValue;
        pBuf++;
#endif
        *pBuf = (tANI_U8)pMac->roam.configParam.isAmsduSupportInAMPDU;
        pBuf++;

        //    
        if(pMac->roam.roamSession[sessionId].fWMMConnection)
        {
           //            
            dwTmp = pal_cpu_to_be32(TRUE);
            vos_mem_copy(pBuf, &dwTmp, sizeof(tAniBool));
            pBuf += sizeof(tAniBool);
        }
        else
        {
            dwTmp = pal_cpu_to_be32(FALSE);
            vos_mem_copy(pBuf, &dwTmp, sizeof(tAniBool));
            pBuf += sizeof(tAniBool);
        }

        //    
        if(pMac->roam.roamSession[sessionId].fQOSConnection)
        {
            //            
            dwTmp = pal_cpu_to_be32(TRUE);
            vos_mem_copy(pBuf, &dwTmp, sizeof(tAniBool));
            pBuf += sizeof(tAniBool);
        }
        else
        {
            dwTmp = pal_cpu_to_be32(FALSE);
            vos_mem_copy(pBuf, &dwTmp, sizeof(tAniBool));
            pBuf += sizeof(tAniBool);
        }
        //       
        csrPrepareJoinReassocReqBuffer( pMac, pBssDescription, pBuf,
                (tANI_U8)pProfile->uapsd_mask);

        status = palSendMBMessage(pMac->hHdd, pMsg );
        if(!HAL_STATUS_SUCCESS(status))
        {
            break;
        }
        else
        {
#ifndef WLAN_MDM_CODE_REDUCTION_OPT
            if (eWNI_SME_JOIN_REQ == messageType)
            {
                //                                               
                sme_QosCsrEventInd(pMac, (v_U8_t)sessionId, SME_QOS_CSR_JOIN_REQ, NULL);
            }
            else if (eWNI_SME_REASSOC_REQ == messageType)
            {
                //                                                  
                sme_QosCsrEventInd(pMac, (v_U8_t)sessionId, SME_QOS_CSR_REASSOC_REQ, NULL);
            }
#endif
        }
    } while( 0 );
    return( status );
}

//
eHalStatus csrSendMBDisassocReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirMacAddr bssId, tANI_U16 reasonCode )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirSmeDisassocReq *pMsg;
    tANI_U8 *pBuf;
    tANI_U16 wTmp;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    if (!CSR_IS_SESSION_VALID( pMac, sessionId ))
        return eHAL_STATUS_FAILURE;
    do {
        status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof( tSirSmeDisassocReq ));
        if ( !HAL_STATUS_SUCCESS(status) ) break;
        palZeroMemory(pMac->hHdd, pMsg, sizeof( tSirSmeDisassocReq ));
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_DISASSOC_REQ);
        pMsg->length = pal_cpu_to_be16((tANI_U16)sizeof( tSirSmeDisassocReq ));
        pBuf = &pMsg->sessionId;
        //          
        *pBuf++ = (tANI_U8)sessionId;
        //              
        *pBuf = 0;
        *( pBuf + 1 ) = 0;
        pBuf += sizeof(tANI_U16);
     
        if ( (pSession->pCurRoamProfile != NULL) &&
             ((CSR_IS_INFRA_AP(pSession->pCurRoamProfile)) ||
              (CSR_IS_WDS_AP(pSession->pCurRoamProfile))) )
        {
            //                                                        
            status = palCopyMemory( pMac->hHdd, (tSirMacAddr *)pBuf, pSession->selfMacAddr, sizeof( tSirMacAddr ) );
            pBuf = pBuf + sizeof ( tSirMacAddr );
            //                                                           
            status = palCopyMemory( pMac->hHdd, (tSirMacAddr *)pBuf, bssId, sizeof( tSirMacAddr ) ); //                                        
            pBuf = pBuf + sizeof ( tSirMacAddr );
        }
        else
        {
            //                                                           
            status = palCopyMemory( pMac->hHdd, (tSirMacAddr *)pBuf, bssId, sizeof( tSirMacAddr ) );
            pBuf = pBuf + sizeof ( tSirMacAddr );
            status = palCopyMemory( pMac->hHdd, (tSirMacAddr *)pBuf, bssId, sizeof( pMsg->bssId ) );
            pBuf = pBuf + sizeof ( tSirMacAddr );
        }
        if(!HAL_STATUS_SUCCESS(status))
        {
            palFreeMemory(pMac->hHdd, pMsg);
            break;
        }
        //           
        wTmp = pal_cpu_to_be16(reasonCode);
        status = palCopyMemory( pMac->hHdd, pBuf, &wTmp, sizeof(tANI_U16) );
        if(!HAL_STATUS_SUCCESS(status))
        {
            palFreeMemory(pMac->hHdd, pMsg);
            break;
        }
        pBuf += sizeof(tANI_U16);
        /*                                                                    
                                                                                */
        if ( CSR_IS_ROAM_SUBSTATE_DISASSOC_HO(pMac, sessionId)
#ifdef WLAN_FEATURE_VOWIFI_11R
                && csrRoamIs11rAssoc(pMac)
#endif
           )            
        {
            *pBuf = CSR_DONT_SEND_DISASSOC_OVER_THE_AIR;  /*                                                         */
        }
        pBuf += sizeof(tANI_U8);
        status = palSendMBMessage( pMac->hHdd, pMsg );
    } while( 0 );
    return( status );
}
eHalStatus csrSendMBTkipCounterMeasuresReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_BOOLEAN bEnable, tSirMacAddr bssId )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirSmeTkipCntrMeasReq *pMsg;
    tANI_U8 *pBuf;
    do
    {
        status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof( tSirSmeTkipCntrMeasReq ));
        if ( !HAL_STATUS_SUCCESS(status) ) break;
        palZeroMemory(pMac->hHdd, pMsg, sizeof( tSirSmeTkipCntrMeasReq ));
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_TKIP_CNTR_MEAS_REQ);
        pMsg->length = pal_cpu_to_be16((tANI_U16)sizeof( tSirSmeTkipCntrMeasReq ));
        pBuf = &pMsg->sessionId;
        //          
        *pBuf++ = (tANI_U8)sessionId;
        //              
        *pBuf = 0;
        *( pBuf + 1 ) = 0;
        pBuf += sizeof(tANI_U16);
        //      
        status = palCopyMemory( pMac->hHdd, pMsg->bssId, bssId, sizeof( tSirMacAddr ) );
        pBuf = pBuf + sizeof ( tSirMacAddr );
        //        
        *pBuf = (tANI_BOOLEAN)bEnable;
        if(!HAL_STATUS_SUCCESS(status))
        {
            palFreeMemory(pMac->hHdd, pMsg);
            break;
        }
        status = palSendMBMessage( pMac->hHdd, pMsg );
    } while( 0 );
    return( status );
}
eHalStatus
csrSendMBGetAssociatedStasReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId,
                                    VOS_MODULE_ID modId, tSirMacAddr bssId,
                                    void *pUsrContext, void *pfnSapEventCallback,
                                    tANI_U8 *pAssocStasBuf )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirSmeGetAssocSTAsReq *pMsg;
    tANI_U8 *pBuf = NULL, *wTmpBuf = NULL;
    tANI_U32 dwTmp;
    do
    {
        status = palAllocateMemory( pMac->hHdd, (void **)&pMsg, sizeof( tSirSmeGetAssocSTAsReq ) );
        if (!HAL_STATUS_SUCCESS(status)) break;
        palZeroMemory( pMac->hHdd, pMsg, sizeof( tSirSmeGetAssocSTAsReq ) );
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_GET_ASSOC_STAS_REQ);
        pBuf = (tANI_U8 *)&pMsg->bssId;
        wTmpBuf = pBuf;
        //      
        palCopyMemory( pMac->hHdd, (tSirMacAddr *)pBuf, bssId, sizeof(tSirMacAddr) );
        pBuf += sizeof(tSirMacAddr);
        //       
        dwTmp = pal_cpu_to_be16((tANI_U16)modId);
        palCopyMemory( pMac->hHdd, pBuf, (tANI_U8 *)&dwTmp, sizeof(tANI_U16));
        pBuf += sizeof(tANI_U16);
        //            
        dwTmp = pal_cpu_to_be32((tANI_U32)pUsrContext);
        palCopyMemory( pMac->hHdd, pBuf, (tANI_U8 *)&dwTmp, sizeof(tANI_U32));
        pBuf += sizeof(tANI_U32);
        //                    
        dwTmp = pal_cpu_to_be32((tANI_U32)pfnSapEventCallback);
        palCopyMemory( pMac->hHdd, pBuf, (tANI_U8 *)&dwTmp, sizeof(tANI_U32));
        pBuf += sizeof(tANI_U32);
        //              
        dwTmp = pal_cpu_to_be32((tANI_U32)pAssocStasBuf);
        palCopyMemory( pMac->hHdd, pBuf, (tANI_U8 *)&dwTmp, sizeof(tANI_U32));
        pBuf += sizeof(tANI_U32);
        pMsg->length = pal_cpu_to_be16((tANI_U16)(sizeof(tANI_U32 ) + (pBuf - wTmpBuf)));//                
        status = palSendMBMessage( pMac->hHdd, pMsg );
    } while( 0 );
    return( status );
        }
eHalStatus
csrSendMBGetWPSPBCSessions( tpAniSirGlobal pMac, tANI_U32 sessionId,
                            tSirMacAddr bssId, void *pUsrContext, void *pfnSapEventCallback,v_MACADDR_t pRemoveMac)
        {
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirSmeGetWPSPBCSessionsReq *pMsg;
    tANI_U8 *pBuf = NULL, *wTmpBuf = NULL;
    tANI_U32 dwTmp;
    do
        {
        status = palAllocateMemory( pMac->hHdd, (void **)&pMsg, sizeof(tSirSmeGetWPSPBCSessionsReq) );
        if (!HAL_STATUS_SUCCESS(status)) break;
        palZeroMemory( pMac->hHdd, pMsg, sizeof( tSirSmeGetWPSPBCSessionsReq ) );
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_GET_WPSPBC_SESSION_REQ);
        pBuf = (tANI_U8 *)&pMsg->pUsrContext;
        VOS_ASSERT(pBuf);

        wTmpBuf = pBuf;
        //            
        dwTmp = pal_cpu_to_be32((tANI_U32)pUsrContext);
        palCopyMemory( pMac->hHdd, pBuf, (tANI_U8 *)&dwTmp, sizeof(tANI_U32));
        pBuf += sizeof(tANI_U32);
        //                  
        dwTmp = pal_cpu_to_be32((tANI_U32)pfnSapEventCallback);
        palCopyMemory( pMac->hHdd, pBuf, (tANI_U8 *)&dwTmp, sizeof(tANI_U32));
        pBuf += sizeof(tANI_U32);
        //      
        palCopyMemory( pMac->hHdd, (tSirMacAddr *)pBuf, bssId, sizeof(tSirMacAddr) );
        pBuf += sizeof(tSirMacAddr);
        //                                  
        palCopyMemory( pMac->hHdd, (tSirMacAddr *)pBuf, pRemoveMac.bytes, sizeof(v_MACADDR_t));
        pBuf += sizeof(v_MACADDR_t);
        pMsg->length = pal_cpu_to_be16((tANI_U16)(sizeof(tANI_U32 ) + (pBuf - wTmpBuf)));//                
        status = palSendMBMessage( pMac->hHdd, pMsg );
    } while( 0 );
    return( status );
}

eHalStatus
csrSendChngMCCBeaconInterval(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    tpSirChangeBIParams pMsg;
    tANI_U16 len = 0;
    eHalStatus status   = eHAL_STATUS_SUCCESS;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }

    //                                                                              
    if(!pMac->roam.roamSession[sessionId].bssParams.updatebeaconInterval )
        return eHAL_STATUS_SUCCESS;

    pMac->roam.roamSession[sessionId].bssParams.updatebeaconInterval =  eANI_BOOLEAN_FALSE;

     /*                                    */
     len = sizeof(tSirChangeBIParams); 
     status = palAllocateMemory( pMac->hHdd, (void **)&pMsg, len );
     if(HAL_STATUS_SUCCESS(status))
     {
         palZeroMemory(pMac->hHdd, pMsg, sizeof(tSirChangeBIParams) );
         pMsg->messageType     = eWNI_SME_CHNG_MCC_BEACON_INTERVAL;
         pMsg->length          = len;

        //      
        palCopyMemory( pMac->hHdd, (tSirMacAddr *)pMsg->bssId, &pSession->selfMacAddr, sizeof(tSirMacAddr) );
        smsLog( pMac, LOG1, FL("CSR Attempting to change BI for Bssid= "MAC_ADDRESS_STR),
               MAC_ADDR_ARRAY(pMsg->bssId));
        pMsg->sessionId       = sessionId;
        smsLog(pMac, LOG1, FL("  session %d BeaconInterval %d"), sessionId, pMac->roam.roamSession[sessionId].bssParams.beaconInterval);
        pMsg->beaconInterval = pMac->roam.roamSession[sessionId].bssParams.beaconInterval; 
        status = palSendMBMessage(pMac->hHdd, pMsg);
    }
     return status;
}

eHalStatus csrSendMBDeauthReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirMacAddr bssId, tANI_U16 reasonCode )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirSmeDeauthReq *pMsg;
    tANI_U8 *pBuf;
    tANI_U16 wTmp;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    if (!CSR_IS_SESSION_VALID( pMac, sessionId ))
        return eHAL_STATUS_FAILURE;
    do {
        status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof( tSirSmeDeauthReq ));
        if ( !HAL_STATUS_SUCCESS(status) ) break;
        palZeroMemory(pMac->hHdd, pMsg, sizeof( tSirSmeDeauthReq ));
                pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_DEAUTH_REQ);
                pMsg->length = pal_cpu_to_be16((tANI_U16)sizeof( tSirSmeDeauthReq ));
        //         
        pBuf = &pMsg->sessionId;
        *pBuf++ = (tANI_U8)sessionId;
        
        //            
        *pBuf = 0;
        *(pBuf + 1 ) = 0;
        pBuf += sizeof(tANI_U16);
        if ((pSession->pCurRoamProfile != NULL)  && (
             (CSR_IS_INFRA_AP(pSession->pCurRoamProfile)) || 
             (CSR_IS_WDS_AP(pSession->pCurRoamProfile)))){ 
            //                                                
            status = palCopyMemory( pMac->hHdd, (tSirMacAddr *)pBuf, pSession->selfMacAddr, sizeof( pMsg->peerMacAddr ) );
            pBuf =  pBuf + sizeof(tSirMacAddr);
        }
        else
        {
            //                                                
            status = palCopyMemory( pMac->hHdd, (tSirMacAddr *)pBuf, bssId, sizeof( pMsg->peerMacAddr ) );
            pBuf =  pBuf + sizeof(tSirMacAddr);
        }
        if(!HAL_STATUS_SUCCESS(status))
        {
            palFreeMemory(pMac->hHdd, pMsg);
            break;
        }     
                //                                                           
        status = palCopyMemory( pMac->hHdd, (tSirMacAddr *) pBuf, bssId, sizeof( pMsg->peerMacAddr ) );
        pBuf =  pBuf + sizeof(tSirMacAddr);
        if(!HAL_STATUS_SUCCESS(status))
        {
            palFreeMemory(pMac->hHdd, pMsg);
            break;
        }     
        wTmp = pal_cpu_to_be16(reasonCode);
        status = palCopyMemory( pMac->hHdd, pBuf, &wTmp,sizeof( tANI_U16 ) );
        if(!HAL_STATUS_SUCCESS(status))
        {
            palFreeMemory(pMac->hHdd, pMsg);
            break;
        }
        status = palSendMBMessage( pMac->hHdd, pMsg );
    } while( 0 );
    return( status );
}

eHalStatus csrSendMBDisassocCnfMsg( tpAniSirGlobal pMac, tpSirSmeDisassocInd pDisassocInd )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirSmeDisassocCnf *pMsg;
    do {
        status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof( tSirSmeDisassocCnf ));
        if ( !HAL_STATUS_SUCCESS(status) ) break;
        palZeroMemory(pMac->hHdd, pMsg, sizeof( tSirSmeDisassocCnf ));
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_DISASSOC_CNF);
        pMsg->statusCode = (tSirResultCodes)pal_cpu_to_be32(eSIR_SME_SUCCESS);
        pMsg->length = pal_cpu_to_be16((tANI_U16)sizeof( tSirSmeDisassocCnf ));
        status = palCopyMemory(pMac->hHdd, pMsg->peerMacAddr, pDisassocInd->peerMacAddr, sizeof(pMsg->peerMacAddr)); 
        if(!HAL_STATUS_SUCCESS(status))
        {
            palFreeMemory(pMac->hHdd, pMsg);
            break;
        }
//                      
        status = palCopyMemory(pMac->hHdd, pMsg->bssId, pDisassocInd->bssId, sizeof(pMsg->peerMacAddr)); 
        if(!HAL_STATUS_SUCCESS(status))
        {
            palFreeMemory(pMac->hHdd, pMsg);
            break;
        }
//                   
        status = palSendMBMessage( pMac->hHdd, pMsg );
    } while( 0 );
    return( status );
}

eHalStatus csrSendMBDeauthCnfMsg( tpAniSirGlobal pMac, tpSirSmeDeauthInd pDeauthInd )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirSmeDeauthCnf *pMsg;
    do {
        status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof( tSirSmeDeauthCnf ));
        if ( !HAL_STATUS_SUCCESS(status) ) break;
        palZeroMemory(pMac->hHdd, pMsg, sizeof( tSirSmeDeauthCnf ));
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_DEAUTH_CNF);
        pMsg->statusCode = (tSirResultCodes)pal_cpu_to_be32(eSIR_SME_SUCCESS);
        pMsg->length = pal_cpu_to_be16((tANI_U16)sizeof( tSirSmeDeauthCnf ));
        status = palCopyMemory(pMac->hHdd, pMsg->bssId, pDeauthInd->bssId, sizeof(pMsg->bssId)); 
        if(!HAL_STATUS_SUCCESS(status))
        {
            palFreeMemory(pMac->hHdd, pMsg);
            break;
        }
        status = palCopyMemory(pMac->hHdd, pMsg->peerMacAddr, pDeauthInd->peerMacAddr, sizeof(pMsg->peerMacAddr)); 
        if(!HAL_STATUS_SUCCESS(status))
        {
            palFreeMemory(pMac->hHdd, pMsg);
            break;
        }
        status = palSendMBMessage( pMac->hHdd, pMsg );
    } while( 0 );
    return( status );
}
eHalStatus csrSendAssocCnfMsg( tpAniSirGlobal pMac, tpSirSmeAssocInd pAssocInd, eHalStatus Halstatus )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirSmeAssocCnf *pMsg;
    tANI_U8 *pBuf;
    tSirResultCodes statusCode;
    tANI_U16 wTmp;
    do {
        status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof( tSirSmeAssocCnf ));
        if ( !HAL_STATUS_SUCCESS(status) ) break;
        palZeroMemory(pMac->hHdd, pMsg, sizeof( tSirSmeAssocCnf ));
                pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_ASSOC_CNF);
                pMsg->length = pal_cpu_to_be16((tANI_U16)sizeof( tSirSmeAssocCnf ));
        pBuf = (tANI_U8 *)&pMsg->statusCode;
        if(HAL_STATUS_SUCCESS(Halstatus))
            statusCode = (tSirResultCodes)pal_cpu_to_be32(eSIR_SME_SUCCESS);
        else
            statusCode = (tSirResultCodes)pal_cpu_to_be32(eSIR_SME_ASSOC_REFUSED);
        palCopyMemory( pMac->hHdd, pBuf, &statusCode, sizeof(tSirResultCodes) );
        pBuf += sizeof(tSirResultCodes);
        //      
        status = palCopyMemory(pMac->hHdd, (tSirMacAddr *)pBuf, pAssocInd->bssId, sizeof(tSirMacAddr));
        pBuf += sizeof (tSirMacAddr);
        //            
        status = palCopyMemory(pMac->hHdd, (tSirMacAddr *)pBuf, pAssocInd->peerMacAddr, sizeof(tSirMacAddr)); 
        pBuf += sizeof (tSirMacAddr);
        //    
        wTmp = pal_cpu_to_be16(pAssocInd->aid);
        palCopyMemory( pMac->hHdd, pBuf, &wTmp, sizeof(tANI_U16) );
        pBuf += sizeof (tANI_U16);
        //               
        status = palCopyMemory(pMac->hHdd, (tSirMacAddr *)pBuf, pAssocInd->bssId, sizeof(tSirMacAddr));
        pBuf += sizeof (tSirMacAddr);
        //                   
        *pBuf = 11;
        status = palSendMBMessage( pMac->hHdd, pMsg );
        if(!HAL_STATUS_SUCCESS(status))
        {
            //                                 
            break;
        }
    } while( 0 );
    return( status );
}
eHalStatus csrSendAssocIndToUpperLayerCnfMsg(   tpAniSirGlobal pMac, 
                                                tpSirSmeAssocInd pAssocInd, 
                                                eHalStatus Halstatus, 
                                                tANI_U8 sessionId)
{
    tSirMsgQ            msgQ;
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirSmeAssocIndToUpperLayerCnf *pMsg;
    tANI_U8 *pBuf;
    tSirResultCodes statusCode;
    tANI_U16 wTmp;
    do {
        status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof( tSirSmeAssocIndToUpperLayerCnf ));
        if ( !HAL_STATUS_SUCCESS(status) ) break;
        palZeroMemory(pMac->hHdd, pMsg, sizeof( tSirSmeAssocIndToUpperLayerCnf ));

        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_UPPER_LAYER_ASSOC_CNF);
        pMsg->length = pal_cpu_to_be16((tANI_U16)sizeof( tSirSmeAssocIndToUpperLayerCnf ));

        pMsg->sessionId = sessionId;

        pBuf = (tANI_U8 *)&pMsg->statusCode;
        if(HAL_STATUS_SUCCESS(Halstatus))
            statusCode = (tSirResultCodes)pal_cpu_to_be32(eSIR_SME_SUCCESS);
        else
            statusCode = (tSirResultCodes)pal_cpu_to_be32(eSIR_SME_ASSOC_REFUSED);
        palCopyMemory( pMac->hHdd, pBuf, &statusCode, sizeof(tSirResultCodes) );
        pBuf += sizeof(tSirResultCodes);
        //      
        status = palCopyMemory(pMac->hHdd, (tSirMacAddr *)pBuf, pAssocInd->bssId, sizeof(tSirMacAddr));
        pBuf += sizeof (tSirMacAddr);
        //            
        status = palCopyMemory(pMac->hHdd, (tSirMacAddr *)pBuf, pAssocInd->peerMacAddr, sizeof(tSirMacAddr)); 
        pBuf += sizeof (tSirMacAddr);
        //      
        wTmp = pal_cpu_to_be16(pAssocInd->staId);
        palCopyMemory( pMac->hHdd, pBuf, &wTmp, sizeof(tANI_U16) );
        pBuf += sizeof (tANI_U16);
        //               
        status = palCopyMemory(pMac->hHdd, (tSirMacAddr *)pBuf, pAssocInd->bssId, sizeof(tSirMacAddr));
        pBuf += sizeof (tSirMacAddr);
        //                   
        *pBuf = 11;
        pBuf += sizeof (tANI_U8);
        //                                                                              
        //   
        *pBuf = pAssocInd->wmmEnabledSta;
        pBuf += sizeof (tANI_U8);
        //      
        status = palCopyMemory(pMac->hHdd, (tSirRSNie *)pBuf, &pAssocInd->rsnIE, sizeof(tSirRSNie));
        pBuf += sizeof (tSirRSNie);
        //             
        status = palCopyMemory(pMac->hHdd, (void *)pBuf, &pAssocInd->addIE, sizeof(tSirAddie));
        pBuf += sizeof (tSirAddie);
        //          
        *pBuf = pAssocInd->reassocReq;
        pBuf += sizeof (tANI_U8);
        msgQ.type = eWNI_SME_UPPER_LAYER_ASSOC_CNF;
        msgQ.bodyptr = pMsg;
        msgQ.bodyval = 0;
        SysProcessMmhMsg(pMac, &msgQ);
    } while( 0 );
    return( status );
}

eHalStatus csrSendMBSetContextReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId,
            tSirMacAddr peerMacAddr, tANI_U8 numKeys, tAniEdType edType, 
            tANI_BOOLEAN fUnicast, tAniKeyDirection aniKeyDirection,
            tANI_U8 keyId, tANI_U8 keyLength, tANI_U8 *pKey, tANI_U8 paeRole, 
            tANI_U8 *pKeyRsc )
{
    tSirSmeSetContextReq *pMsg;
    tANI_U16 msgLen;
    eHalStatus status = eHAL_STATUS_FAILURE;
    tAniEdType tmpEdType;
    tAniKeyDirection tmpDirection;
    tANI_U8 *pBuf = NULL;
    tANI_U8 *p = NULL;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    do {
        if( ( 1 != numKeys ) && ( 0 != numKeys ) ) break;
        //                                                                                                
        //                                                                                 
        msgLen  = sizeof( tANI_U16) + sizeof( tANI_U16 ) + sizeof( tSirMacAddr ) +
                  sizeof( tSirMacAddr ) + 1 + sizeof(tANI_U16) +
                  sizeof( pMsg->keyMaterial.length ) + sizeof( pMsg->keyMaterial.edType ) + sizeof( pMsg->keyMaterial.numKeys ) +
                  ( sizeof( pMsg->keyMaterial.key ) );
                     
        status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, msgLen);
        if ( !HAL_STATUS_SUCCESS(status) ) break;
        palZeroMemory(pMac->hHdd, pMsg, msgLen);
                pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_SETCONTEXT_REQ);
                pMsg->length = pal_cpu_to_be16(msgLen);
        //         
        pBuf = &pMsg->sessionId;
        *pBuf = (tANI_U8)sessionId;
        pBuf++;
        //              
        *pBuf = 0;
        *(pBuf + 1) = 0;
        pBuf += sizeof(tANI_U16);
        //            
        palCopyMemory( pMac->hHdd, pBuf, 
             (tANI_U8 *)peerMacAddr, sizeof(tSirMacAddr) );

        pBuf += sizeof(tSirMacAddr);

        //      
        palCopyMemory( pMac->hHdd, pBuf, 
            (tANI_U8 *)&pSession->connectedProfile.bssid, sizeof(tSirMacAddr) );

        pBuf += sizeof(tSirMacAddr);

        p = pBuf;
                //                                                                                                         
                //                                            
                //
                //                                                                                                     
                //                                                           
                p = pal_set_U16( p, pal_cpu_to_be16((tANI_U16)( sizeof( pMsg->keyMaterial.numKeys ) + ( numKeys * sizeof( pMsg->keyMaterial.key ) ) )) );
                //                             
        tmpEdType = (tAniEdType)pal_cpu_to_be32(edType);
        palCopyMemory( pMac->hHdd, p, (tANI_U8 *)&tmpEdType, sizeof(tAniEdType) );
        p += sizeof( pMsg->keyMaterial.edType );
        //                                        
        *p = numKeys;
        p += sizeof( pMsg->keyMaterial.numKeys );   
        //                            
        *p = keyId;
        p += sizeof( pMsg->keyMaterial.key[ 0 ].keyId );
        //                                          
        *p = (tANI_U8)fUnicast;
        p += sizeof( pMsg->keyMaterial.key[ 0 ].unicast );
                //                                             
        tmpDirection = (tAniKeyDirection)pal_cpu_to_be32(aniKeyDirection);
        palCopyMemory( pMac->hHdd, p, (tANI_U8 *)&tmpDirection, sizeof(tAniKeyDirection) );
        p += sizeof(tAniKeyDirection);
        //                        
        palCopyMemory( pMac->hHdd, p, pKeyRsc, CSR_MAX_RSC_LEN );
        p += sizeof( pMsg->keyMaterial.key[ 0 ].keyRsc );
                //                     
                *p = paeRole;   //                
                p++;
                //                                    
                p = pal_set_U16( p, pal_cpu_to_be16(keyLength) );
        if ( keyLength && pKey ) 
        {   
            palCopyMemory( pMac->hHdd, p, pKey, keyLength ); 
            if(keyLength == 16)
            {
                smsLog(pMac, LOG1, "  SME Set keyIdx (%d) encType(%d) key = %02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X",
                keyId, edType, pKey[0], pKey[1], pKey[2], pKey[3], pKey[4],
                pKey[5], pKey[6], pKey[7], pKey[8],
                pKey[9], pKey[10], pKey[11], pKey[12], pKey[13], pKey[14], pKey[15]);
            }
        }
        status = palSendMBMessage(pMac->hHdd, pMsg);
    } while( 0 );
    return( status );
}

eHalStatus csrSendMBStartBssReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId, eCsrRoamBssType bssType, 
                                    tCsrRoamStartBssParams *pParam, tSirBssDescription *pBssDesc )
{
    eHalStatus status;
    tSirSmeStartBssReq *pMsg;
    tANI_U8 *pBuf = NULL;
    tANI_U8  *wTmpBuf  = NULL;
    tANI_U16 msgLen, wTmp;
    tANI_U32 dwTmp;
    tSirNwType nwType;
    ePhyChanBondState cbMode;
    tANI_U32 authType;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    do {
        pSession->joinFailStatusCode.statusCode = eSIR_SME_SUCCESS;
        pSession->joinFailStatusCode.reasonCode = 0;
        msgLen = sizeof(tSirSmeStartBssReq);
        status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, msgLen);
        if ( !HAL_STATUS_SUCCESS(status) ) break;
        palZeroMemory(pMac->hHdd, pMsg, msgLen);
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_START_BSS_REQ);
        pBuf = &pMsg->sessionId;
        wTmpBuf = pBuf;
        //         
        *pBuf = (tANI_U8)sessionId;
        pBuf++;
        //              
        *pBuf = 0;
        *(pBuf + 1) = 0;
        pBuf += sizeof(tANI_U16);
        //       
        palCopyMemory( pMac->hHdd, pBuf, pParam->bssid, sizeof(tSirMacAddr) );
        pBuf += sizeof(tSirMacAddr);
        //            
        palCopyMemory( pMac->hHdd, pBuf, pSession->selfMacAddr, sizeof(tSirMacAddr) );
        pBuf += sizeof(tSirMacAddr);
        //               
        if( pBssDesc && pBssDesc->beaconInterval )
        {
            wTmp = pal_cpu_to_be16( pBssDesc->beaconInterval );
        }
        else if(pParam->beaconInterval)
        {
            wTmp = pal_cpu_to_be16( pParam->beaconInterval );
        }
        else
        {
            wTmp = pal_cpu_to_be16( WNI_CFG_BEACON_INTERVAL_STADEF );
        }
        if(csrIsconcurrentsessionValid (pMac, sessionId,
                                   pParam->bssPersona)
                                   == eHAL_STATUS_SUCCESS )
        {    
           csrValidateMCCBeaconInterval(pMac, pParam->operationChn, &wTmp, sessionId,
                                      pParam->bssPersona);
           //                           
           pParam->beaconInterval = wTmp;
        }
        else
        {
            smsLog( pMac,LOGE, FL("****Start BSS failed persona already exists***"));
            status = eHAL_STATUS_FAILURE;
            palFreeMemory( pMac->hHdd, pMsg );
            return status;
        }

        palCopyMemory( pMac->hHdd, pBuf, &wTmp, sizeof( tANI_U16 ) ); 
        pBuf += sizeof(tANI_U16);
        //          
        *pBuf = (tANI_U8)csrTranslateToWNICfgDot11Mode( pMac, pParam->uCfgDot11Mode );
        pBuf += 1;
        //        
        dwTmp = pal_cpu_to_be32( csrTranslateBsstypeToMacType( bssType ) );
        palCopyMemory( pMac->hHdd, pBuf, &dwTmp, sizeof(tSirBssType) );
        pBuf += sizeof(tSirBssType);
        //     
        if( pParam->ssId.length )
        {
            //         
            *pBuf = pParam->ssId.length;
            pBuf++;
            palCopyMemory( pMac->hHdd, pBuf, pParam->ssId.ssId, pParam->ssId.length );
            pBuf += pParam->ssId.length;
        }
        else
        {
            *pBuf = 0;
            pBuf++;        
        }
        //                   
        *pBuf = pParam->operationChn;
        pBuf++;
        //                                        
        cbMode = (ePhyChanBondState)pal_cpu_to_be32(pParam->cbMode);
        palCopyMemory( pMac->hHdd, pBuf, (tANI_U8 *)&cbMode, sizeof(ePhyChanBondState) );
        pBuf += sizeof(ePhyChanBondState);

        //            
        *pBuf = pParam->privacy;
        pBuf++;
 
        //          
        *pBuf = pParam->ApUapsdEnable;
        pBuf++;
        //               
        *pBuf = pParam->ssidHidden;
        pBuf++;
        *pBuf = (tANI_U8)pParam->fwdWPSPBCProbeReq;
        pBuf++;
        
        //                            
        *pBuf = (tANI_U8)pParam->protEnabled;
        pBuf++;
        //                                                            
        *pBuf = (tANI_U8)pParam->obssProtEnabled;
        pBuf++;
        //                             
        wTmp = pal_cpu_to_be16( pParam->ht_protection );
        palCopyMemory( pMac->hHdd, pBuf, &wTmp, sizeof( tANI_U16 ) ); 
        pBuf += sizeof(tANI_U16);
        //              
        authType = pal_cpu_to_be32(pParam->authType);
        palCopyMemory( pMac->hHdd, pBuf, (tANI_U8 *)&authType, sizeof(tANI_U32));
        pBuf += sizeof(tANI_U32);
        //         
        dwTmp = pal_cpu_to_be32(pParam->dtimPeriod);
        palCopyMemory( pMac->hHdd, pBuf, (tANI_U8 *)&dwTmp, sizeof(tANI_U32));
        pBuf += sizeof(tANI_U32);
        //              
        *pBuf = pParam->wps_state;
        pBuf++;
        //       
        *pBuf = (tANI_U8)pParam->bssPersona;
        pBuf++;
        
        //                       
        *pBuf = (tANI_U8)(tANI_U8)pMac->roam.configParam.txLdpcEnable;
        pBuf++;

        //                             
        *pBuf = (tANI_U8)pMac->roam.configParam.enableOxygenNwk;
        pBuf++;

        //           
        if( pParam->nRSNIELength > sizeof(pMsg->rsnIE.rsnIEdata) )
        {
            status = eHAL_STATUS_INVALID_PARAMETER;
            palFreeMemory( pMac->hHdd, pMsg );
            break;
        }
        wTmp = pal_cpu_to_be16( pParam->nRSNIELength );
        palCopyMemory( pMac->hHdd, pBuf, &wTmp, sizeof(tANI_U16) );
        pBuf += sizeof(tANI_U16);
        if( wTmp )
        {
            wTmp = pParam->nRSNIELength;
            palCopyMemory( pMac->hHdd, pBuf, pParam->pRSNIE, wTmp );
            pBuf += wTmp;
        }
        nwType = (tSirNwType)pal_cpu_to_be32(pParam->sirNwType);
        palCopyMemory( pMac->hHdd, pBuf, (tANI_U8 *)&nwType, sizeof(tSirNwType) );
        pBuf += sizeof(tSirNwType);
        *pBuf = pParam->operationalRateSet.numRates; //                        
        pBuf++;
        palCopyMemory( pMac->hHdd, pBuf, pParam->operationalRateSet.rate, pParam->operationalRateSet.numRates );
        pBuf += pParam->operationalRateSet.numRates ;
        *pBuf++ = pParam->extendedRateSet.numRates;
        if(0 != pParam->extendedRateSet.numRates)
        {
            palCopyMemory( pMac->hHdd, pBuf, pParam->extendedRateSet.rate, pParam->extendedRateSet.numRates );
            pBuf += pParam->extendedRateSet.numRates;
        }
        msgLen = (tANI_U16)(sizeof(tANI_U32 ) + (pBuf - wTmpBuf)); //                
        pMsg->length = pal_cpu_to_be16(msgLen);
        
        status = palSendMBMessage(pMac->hHdd, pMsg);
    } while( 0 );
  return( status );
}

eHalStatus csrSendMBStopBssReqMsg( tpAniSirGlobal pMac, tANI_U32 sessionId )
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    tSirSmeStopBssReq *pMsg;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    tANI_U8 *pBuf;
    tANI_U16 msgLen;

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    
    do {
        status = palAllocateMemory(pMac, (void **)&pMsg, sizeof(tSirSmeStopBssReq));
        if ( !HAL_STATUS_SUCCESS(status) ) break;
        palZeroMemory(pMac->hHdd, pMsg, sizeof( tSirSmeStopBssReq ));
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_STOP_BSS_REQ);
        pBuf = &pMsg->sessionId;
        //         
        *pBuf = (tANI_U8)sessionId;
        pBuf++;
        //              
        *pBuf = 0;
        pBuf += sizeof(tANI_U16);
        //           
        *pBuf  = 0;
        pBuf += sizeof(tSirResultCodes);
        //       
        //                                                                                    
        if( CSR_IS_CONN_WDS_STA(&pSession->connectedProfile) )
        {
            palCopyMemory( pMac->hHdd, pBuf,(tANI_U8 *)&pSession->selfMacAddr,  sizeof(tSirMacAddr) );                
        }
        else
        {
            palCopyMemory( pMac->hHdd, pBuf,(tANI_U8 *)&pSession->connectedProfile.bssid,  sizeof(tSirMacAddr) );              
        }
       pBuf += sizeof(tSirMacAddr);
       msgLen = sizeof(tANI_U16) + sizeof(tANI_U16) + 1 + sizeof(tANI_U16) + sizeof(tSirResultCodes) + sizeof(tSirMacAddr);
       pMsg->length =  pal_cpu_to_be16(msgLen);
       status =  palSendMBMessage( pMac->hHdd, pMsg );
#if 0            
        status = palAllocateMemory(pMac, (void **)&pMsg, sizeof(tSirSmeStopBssReq));
        if ( !HAL_STATUS_SUCCESS(status) ) break;
        palZeroMemory(pMac->hHdd, pMsg, sizeof( tSirSmeStopBssReq ));
                pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_STOP_BSS_REQ);
                pMsg->reasonCode = 0;
        //      
        //                                                                                    
        if( CSR_IS_CONN_WDS_STA(&pSession->connectedProfile) )
        {
            pbBssid = (tANI_U8 *)&pSession->selfMacAddr;
        }
        else
        {
            pbBssid = (tANI_U8 *)&pSession->connectedProfile.bssid;
        }
        palCopyMemory( pMac->hHdd, &pMsg->bssId, pbBssid, sizeof(tSirMacAddr) );
        pMsg->transactionId = 0;
        pMsg->sessionId = (tANI_U8)sessionId;
                pMsg->length = pal_cpu_to_be16((tANI_U16)sizeof( tSirSmeStopBssReq ));
                status = palSendMBMessage( pMac->hHdd, pMsg );
#endif                
        } while( 0 );
    return( status );
}

eHalStatus csrReassoc(tpAniSirGlobal pMac, tANI_U32 sessionId, 
                      tCsrRoamModifyProfileFields *pModProfileFields,
                      tANI_U32 *pRoamId, v_BOOL_t fForce)
{
   eHalStatus status = eHAL_STATUS_FAILURE;
   tANI_U32 roamId = 0;
   tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
   if((csrIsConnStateConnected(pMac, sessionId)) &&
      (fForce || (!palEqualMemory(pMac->hHdd, &pModProfileFields, 
                       &pSession->connectedProfile.modifyProfileFields, 
                       sizeof(tCsrRoamModifyProfileFields)))) )
   {
      roamId = GET_NEXT_ROAM_ID(&pMac->roam);
      if(pRoamId)
      {
         *pRoamId = roamId;
      }

      status = csrRoamIssueReassoc(pMac, sessionId, NULL, pModProfileFields, 
                                   eCsrSmeIssuedReassocToSameAP, roamId, 
                                   eANI_BOOLEAN_FALSE);
   }
   return status;
}
static eHalStatus csrRoamSessionOpened(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tCsrRoamInfo roamInfo;
    palZeroMemory(pMac->hHdd, &roamInfo, sizeof(tCsrRoamInfo));
    status = csrRoamCallCallback(pMac, sessionId, &roamInfo, 0,
                            eCSR_ROAM_SESSION_OPENED, eCSR_ROAM_RESULT_NONE);
    return (status);
}
eHalStatus csrProcessAddStaSessionRsp( tpAniSirGlobal pMac, tANI_U8 *pMsg)
{
   eHalStatus                         status = eHAL_STATUS_SUCCESS;
   tListElem                          *pEntry = NULL;
   tSmeCmd                            *pCommand = NULL;
   tSirSmeAddStaSelfRsp               *pRsp;
   do
   {
      if(pMsg == NULL)
      {
         smsLog(pMac, LOGE, "in %s msg ptr is NULL", __func__);
         status = eHAL_STATUS_FAILURE;
         break;
      }
      pEntry = csrLLPeekHead( &pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK );
      if(pEntry)
      {
         pCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
         if(eSmeCommandAddStaSession == pCommand->command)
         {
            pRsp = (tSirSmeAddStaSelfRsp*)pMsg;
            smsLog( pMac, LOG1, "Add Sta rsp status = %d", pRsp->status );
            //                                                                                                     
            csrRoamSessionOpened(pMac, pCommand->sessionId);
            //                                          
            if(csrLLRemoveEntry(&pMac->sme.smeCmdActiveList, pEntry, LL_ACCESS_LOCK))
            {
               //                                                      
               csrReleaseCommand(pMac, pCommand);
            }
            smeProcessPendingQueue( pMac );
         }
         else
         {
            smsLog(pMac, LOGE, "in %s eWNI_SME_ADD_STA_SELF_RSP Received but NO Add sta session command are ACTIVE ...",
                  __func__);
            status = eHAL_STATUS_FAILURE;
            break;
         }
      }
      else
      {
         smsLog(pMac, LOGE, "in %s eWNI_SME_ADD_STA_SELF_RSP Received but NO commands are ACTIVE ...",
               __func__);
         status = eHAL_STATUS_FAILURE;
         break;
      }
   } while(0);
   return status;
}
eHalStatus csrSendMBAddSelfStaReqMsg(tpAniSirGlobal pMac,
                                     tAddStaForSessionCmd *pAddStaReq)
{
   tSirSmeAddStaSelfReq *pMsg;
   tANI_U16 msgLen;
   eHalStatus status = eHAL_STATUS_FAILURE;
   do {
      msgLen  = sizeof(tSirSmeAddStaSelfReq);
      status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, msgLen);
      if ( !HAL_STATUS_SUCCESS(status) ) break;
      palZeroMemory(pMac->hHdd, pMsg, msgLen);
      pMsg->mesgType = pal_cpu_to_be16((tANI_U16)eWNI_SME_ADD_STA_SELF_REQ);
      pMsg->mesgLen = pal_cpu_to_be16(msgLen);
      //                     
      palCopyMemory(pMac->hHdd, (tANI_U8 *)pMsg->selfMacAddr,
                      (tANI_U8 *)&pAddStaReq->selfMacAddr, sizeof(tSirMacAddr));

      pMsg->currDeviceMode = pAddStaReq->currDeviceMode;

      smsLog( pMac, LOG1, FL("selfMac="MAC_ADDRESS_STR),
              MAC_ADDR_ARRAY(pMsg->selfMacAddr));
      status = palSendMBMessage(pMac->hHdd, pMsg);
   } while( 0 );
   return( status );
}
eHalStatus csrIssueAddStaForSessionReq(tpAniSirGlobal pMac,
                                       tANI_U32 sessionId,
                                       tSirMacAddr sessionMacAddr)
{
   eHalStatus status = eHAL_STATUS_SUCCESS;
   tSmeCmd *pCommand;
   pCommand = csrGetCommandBuffer(pMac);
   if(NULL == pCommand)
   {
      status = eHAL_STATUS_RESOURCES;
   }
   else
   {
      pCommand->command = eSmeCommandAddStaSession;
      pCommand->sessionId = (tANI_U8)sessionId;
      palCopyMemory( pMac->hHdd, pCommand->u.addStaSessionCmd.selfMacAddr, sessionMacAddr, sizeof( tSirMacAddr ) );
      pCommand->u.addStaSessionCmd.currDeviceMode = pMac->sme.currDeviceMode;
      status = csrQueueSmeCommand(pMac, pCommand, TRUE);
      if( !HAL_STATUS_SUCCESS( status ) )
      {
         //                 
         smsLog( pMac, LOGE, FL(" fail to send message status = %d"), status );
      }
   }
   return (status);
}
eHalStatus csrProcessAddStaSessionCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
   return csrSendMBAddSelfStaReqMsg(pMac, &pCommand->u.addStaSessionCmd);
}
eHalStatus csrRoamOpenSession(tpAniSirGlobal pMac,
                              csrRoamCompleteCallback callback,
                              void *pContext, tANI_U8 *pSelfMacAddr,
                              tANI_U8 *pbSessionId)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_U32 i;
    tCsrRoamSession *pSession;
    *pbSessionId = CSR_SESSION_ID_INVALID;
    for( i = 0; i < CSR_ROAM_SESSION_MAX; i++ )
    {
        if( !CSR_IS_SESSION_VALID( pMac, i ) )
        {
            pSession = CSR_GET_SESSION( pMac, i );
            status = eHAL_STATUS_SUCCESS;
            pSession->sessionActive = eANI_BOOLEAN_TRUE;
            pSession->sessionId = (tANI_U8)i;
                pSession->callback = callback;
            pSession->pContext = pContext;
            palCopyMemory( pMac->hHdd, &pSession->selfMacAddr, pSelfMacAddr, sizeof(tCsrBssid) );
            *pbSessionId = (tANI_U8)i;
            status = vos_timer_init(&pSession->hTimerRoaming, VOS_TIMER_TYPE_SW,
                                    csrRoamRoamingTimerHandler,
                                    &pSession->roamingTimerInfo);
            if (!HAL_STATUS_SUCCESS(status))
            {
                smsLog(pMac, LOGE, FL("cannot allocate memory for Roaming timer"));
                break;
            }
#ifdef FEATURE_WLAN_BTAMP_UT_RF
            status = vos_timer_init(&pSession->hTimerJoinRetry, VOS_TIMER_TYPE_SW,
                                    csrRoamJoinRetryTimerHandler,
                                    &pSession->joinRetryTimerInfo);
            if (!HAL_STATUS_SUCCESS(status))
            {
                smsLog(pMac, LOGE, FL("cannot allocate memory for joinretry timer"));
                break;
            }
#endif
            pSession->ibssJoinTimerInfo.pMac = pMac;
            pSession->ibssJoinTimerInfo.sessionId = CSR_SESSION_ID_INVALID;
            status = vos_timer_init(&pSession->hTimerIbssJoining, VOS_TIMER_TYPE_SW,
                                    csrRoamIbssJoinTimerHandler,
                                    &pSession->ibssJoinTimerInfo);
            if (!HAL_STATUS_SUCCESS(status))
            {
                smsLog(pMac, LOGE, FL("cannot allocate memory for IbssJoining timer"));
                break;
            }
            status = csrIssueAddStaForSessionReq (pMac, i, pSelfMacAddr);
            break;
        }
    }
    if( CSR_ROAM_SESSION_MAX == i )
    {
        //                       
        status = eHAL_STATUS_RESOURCES;
    }
    return ( status );
}
eHalStatus csrProcessDelStaSessionRsp( tpAniSirGlobal pMac, tANI_U8 *pMsg)
{
   eHalStatus                         status = eHAL_STATUS_SUCCESS;
   tListElem                          *pEntry = NULL;
   tSmeCmd                            *pCommand = NULL;
   tSirSmeDelStaSelfRsp               *pRsp;
   do
   {
      if(pMsg == NULL)
      {
         smsLog(pMac, LOGE, "in %s msg ptr is NULL", __func__);
         status = eHAL_STATUS_FAILURE;
         break;
      }
      pEntry = csrLLPeekHead( &pMac->sme.smeCmdActiveList, LL_ACCESS_LOCK );
      if(pEntry)
      {
         pCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
         if(eSmeCommandDelStaSession == pCommand->command)
         {
            tANI_U8 sessionId = pCommand->sessionId;
            pRsp = (tSirSmeDelStaSelfRsp*)pMsg;
            smsLog( pMac, LOG1, "Del Sta rsp status = %d", pRsp->status );
            //                     
            csrCleanupSession(pMac, sessionId);
            if(pCommand->u.delStaSessionCmd.callback)
            {
                 
                status = sme_ReleaseGlobalLock( &pMac->sme );
                if ( HAL_STATUS_SUCCESS( status ) )
                {
                    pCommand->u.delStaSessionCmd.callback(
                                      pCommand->u.delStaSessionCmd.pContext);
                    status = sme_AcquireGlobalLock( &pMac->sme );
                    if (! HAL_STATUS_SUCCESS( status ) )
                    {
                        smsLog(pMac, LOGP, "%s: Failed to Acquire Lock", __func__);
                        return status;
                    }
                }
                else {
                    smsLog(pMac, LOGE, "%s: Failed to Release Lock", __func__);
                }
            } 
   
            //                                          
            if(csrLLRemoveEntry(&pMac->sme.smeCmdActiveList, pEntry, LL_ACCESS_LOCK))
            {
               //                                                      
               csrReleaseCommand(pMac, pCommand);
            }
            smeProcessPendingQueue( pMac );
         }
         else
         {
            smsLog(pMac, LOGE, "in %s eWNI_SME_DEL_STA_SELF_RSP Received but NO Del sta session command are ACTIVE ...",
                  __func__);
            status = eHAL_STATUS_FAILURE;
            break;
         }
      }
      else
      {
         smsLog(pMac, LOGE, "in %s eWNI_SME_DEL_STA_SELF_RSP Received but NO commands are ACTIVE ...",
               __func__);
         status = eHAL_STATUS_FAILURE;
         break;
      }
   } while(0);
   return status;
}
eHalStatus csrSendMBDelSelfStaReqMsg( tpAniSirGlobal pMac, tSirMacAddr macAddr )
{
   tSirSmeDelStaSelfReq *pMsg;
   tANI_U16 msgLen;
   eHalStatus status = eHAL_STATUS_FAILURE;
   do {
      msgLen  = sizeof( tANI_U16 ) + sizeof( tANI_U16 ) + sizeof( tSirMacAddr ) /* 
                              */;
      status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, msgLen);
      if ( !HAL_STATUS_SUCCESS(status) ) break;
   
      palZeroMemory(pMac->hHdd, pMsg, msgLen);
      pMsg->mesgType = pal_cpu_to_be16((tANI_U16)eWNI_SME_DEL_STA_SELF_REQ);
      pMsg->mesgLen = pal_cpu_to_be16(msgLen);
      //                     
      palCopyMemory( pMac->hHdd, (tANI_U8 *)pMsg->selfMacAddr, (tANI_U8 *)macAddr, sizeof(tSirMacAddr) );
      status = palSendMBMessage(pMac->hHdd, pMsg);
   } while( 0 );
   return( status );
}
eHalStatus csrIssueDelStaForSessionReq(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                       tSirMacAddr sessionMacAddr,
                                       csrRoamSessionCloseCallback callback,
                                       void *pContext)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
   tSmeCmd *pCommand;
   pCommand = csrGetCommandBuffer(pMac);
   if(NULL == pCommand)
   {
      status = eHAL_STATUS_RESOURCES;
   }
   else
   {
      pCommand->command = eSmeCommandDelStaSession;
      pCommand->sessionId = (tANI_U8)sessionId;
      pCommand->u.delStaSessionCmd.callback = callback;
      pCommand->u.delStaSessionCmd.pContext = pContext;
      palCopyMemory( pMac->hHdd, pCommand->u.delStaSessionCmd.selfMacAddr, sessionMacAddr, sizeof( tSirMacAddr ) );
      status = csrQueueSmeCommand(pMac, pCommand, TRUE);
      if( !HAL_STATUS_SUCCESS( status ) )
      {
         //                 
         smsLog( pMac, LOGE, FL(" fail to send message status = %d"), status );
      }
   }
   return (status);
}
eHalStatus csrProcessDelStaSessionCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
   return csrSendMBDelSelfStaReqMsg( pMac, 
         pCommand->u.delStaSessionCmd.selfMacAddr );
}
static void purgeCsrSessionCmdList(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    tDblLinkList *pList = &pMac->roam.roamCmdPendingList;
    tListElem *pEntry, *pNext;
    tSmeCmd *pCommand;
    tDblLinkList localList;

    vos_mem_zero(&localList, sizeof(tDblLinkList));
    if(!HAL_STATUS_SUCCESS(csrLLOpen(pMac->hHdd, &localList)))
    {
        smsLog(pMac, LOGE, FL(" failed to open list"));
        return;
    }
    csrLLLock(pList);
    pEntry = csrLLPeekHead(pList, LL_ACCESS_NOLOCK);
    while(pEntry != NULL)
    {
        pNext = csrLLNext(pList, pEntry, LL_ACCESS_NOLOCK);
        pCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
        if(pCommand->sessionId == sessionId)
        {
            if(csrLLRemoveEntry(pList, pEntry, LL_ACCESS_NOLOCK))
            {
                csrLLInsertTail(&localList, pEntry, LL_ACCESS_NOLOCK);
            }
        }
        pEntry = pNext;
    }
    csrLLUnlock(pList);

    while( (pEntry = csrLLRemoveHead(&localList, LL_ACCESS_NOLOCK)) )
    {
        pCommand = GET_BASE_ADDR( pEntry, tSmeCmd, Link );
        csrAbortCommand(pMac, pCommand, eANI_BOOLEAN_TRUE);
    }
    csrLLClose(&localList);
}

void csrCleanupSession(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    if( CSR_IS_SESSION_VALID( pMac, sessionId ) )
    {
        tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
        csrRoamStop(pMac, sessionId);
        csrFreeConnectBssDesc(pMac, sessionId);
        csrRoamFreeConnectProfile( pMac, &pSession->connectedProfile );
        csrRoamFreeConnectedInfo ( pMac, &pSession->connectedInfo);
        vos_timer_destroy(&pSession->hTimerRoaming);
#ifdef FEATURE_WLAN_BTAMP_UT_RF
        vos_timer_destroy(&pSession->hTimerJoinRetry);
#endif
        vos_timer_destroy(&pSession->hTimerIbssJoining);
        purgeSmeSessionCmdList(pMac, sessionId);
        purgeCsrSessionCmdList(pMac, sessionId);
        csrInitSession(pMac, sessionId);
    }
}

eHalStatus csrRoamCloseSession( tpAniSirGlobal pMac, tANI_U32 sessionId,
                                tANI_BOOLEAN fSync, 
                                csrRoamSessionCloseCallback callback,
                                void *pContext )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    if( CSR_IS_SESSION_VALID( pMac, sessionId ) )
    {
        tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
        if(fSync)
        {
            csrCleanupSession(pMac, sessionId);
        }
        else
        { 
            purgeSmeSessionCmdList(pMac, sessionId);
            purgeCsrSessionCmdList(pMac, sessionId);
            status = csrIssueDelStaForSessionReq( pMac, sessionId,
                                        pSession->selfMacAddr, callback, pContext);
        }
    }
    else
    {
        status = eHAL_STATUS_INVALID_PARAMETER;
    }
    return ( status );
}

static void csrInitSession( tpAniSirGlobal pMac, tANI_U32 sessionId )
{
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return;
    }
    
    pSession->sessionActive = eANI_BOOLEAN_FALSE;
    pSession->sessionId = CSR_SESSION_ID_INVALID;
    pSession->callback = NULL;
    pSession->pContext = NULL;
    pSession->ibss_join_pending = FALSE;
    pSession->connectState = eCSR_ASSOC_STATE_TYPE_NOT_CONNECTED;
    //                                                                   
    csrFreeRoamProfile( pMac, sessionId );
    csrRoamFreeConnectProfile(pMac, &pSession->connectedProfile);
    csrRoamFreeConnectedInfo( pMac, &pSession->connectedInfo );
    csrFreeConnectBssDesc(pMac, sessionId);
    csrScanEnable(pMac);
    palZeroMemory( pMac->hHdd, &pSession->selfMacAddr, sizeof(tCsrBssid) );
    if(pSession->pWpaRsnReqIE)
    {
        palFreeMemory(pMac->hHdd, pSession->pWpaRsnReqIE);
        pSession->pWpaRsnReqIE = NULL;
    }
    pSession->nWpaRsnReqIeLength = 0;
    if(pSession->pWpaRsnRspIE)
    {
        palFreeMemory(pMac->hHdd, pSession->pWpaRsnRspIE);
        pSession->pWpaRsnRspIE = NULL;
    }
    pSession->nWpaRsnRspIeLength = 0;
#ifdef FEATURE_WLAN_WAPI
    if(pSession->pWapiReqIE)
    {
        palFreeMemory(pMac->hHdd, pSession->pWapiReqIE);
        pSession->pWapiReqIE = NULL;
    }
    pSession->nWapiReqIeLength = 0;
    if(pSession->pWapiRspIE)
    {
        palFreeMemory(pMac->hHdd, pSession->pWapiRspIE);
        pSession->pWapiRspIE = NULL;
    }
    pSession->nWapiRspIeLength = 0;
#endif /*                   */
    if(pSession->pAddIEScan)
    {
        palFreeMemory(pMac->hHdd, pSession->pAddIEScan);
        pSession->pAddIEScan = NULL;
    }
    pSession->nAddIEScanLength = 0;
    if(pSession->pAddIEAssoc)
    {
        palFreeMemory(pMac->hHdd, pSession->pAddIEAssoc);
        pSession->pAddIEAssoc = NULL;
}
    pSession->nAddIEAssocLength = 0;
}

eHalStatus csrRoamGetSessionIdFromBSSID( tpAniSirGlobal pMac, tCsrBssid *bssid, tANI_U32 *pSessionId )
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    tANI_U32 i;
    for( i = 0; i < CSR_ROAM_SESSION_MAX; i++ )
    {
        if( CSR_IS_SESSION_VALID( pMac, i ) )
        {
            if( csrIsMacAddressEqual( pMac, bssid, &pMac->roam.roamSession[i].connectedProfile.bssid ) )
            {
                //        
                status = eHAL_STATUS_SUCCESS;
                *pSessionId = i;
                break;
            }
        }
    }
    return( status );
}

//                                                                                             
//                                            
static tANI_U32 csrFindIbssSession( tpAniSirGlobal pMac )
{
    tANI_U32 i, nRet = CSR_SESSION_ID_INVALID;
    tCsrRoamSession *pSession;
    for( i = 0; i < CSR_ROAM_SESSION_MAX; i++ )
    {
        if( CSR_IS_SESSION_VALID( pMac, i ) )
        {
            pSession = CSR_GET_SESSION( pMac, i );
            if( pSession->pCurRoamProfile && ( csrIsBssTypeIBSS( pSession->connectedProfile.BSSType ) ) )
            {
                //        
                nRet = i;
                break;
            }
        }
    }
    return (nRet);
}
static void csrRoamLinkUp(tpAniSirGlobal pMac, tCsrBssid bssid)
{
   /*                                                                    
                                                                           */
   
   smsLog(pMac, LOGW, " csrRoamLinkUp: WLAN link UP with AP= "MAC_ADDRESS_STR,
          MAC_ADDR_ARRAY(bssid));
   /*                                                                     */
   pMac->roam.configParam.vccRssiThreshold =
      ( 0 == pMac->roam.configParam.vccRssiThreshold ) ? 
      CSR_VCC_RSSI_THRESHOLD : pMac->roam.configParam.vccRssiThreshold;
   pMac->roam.vccLinkQuality = eCSR_ROAM_LINK_QUAL_POOR_IND;
    /*                                                                     */
   pMac->roam.configParam.vccUlMacLossThreshold =
      ( 0 == pMac->roam.configParam.vccUlMacLossThreshold ) ? 
      CSR_VCC_UL_MAC_LOSS_THRESHOLD : pMac->roam.configParam.vccUlMacLossThreshold;
#if   defined WLAN_FEATURE_NEIGHBOR_ROAMING
    {
        tANI_U32 sessionId = 0;
        /*                                                                   */
        csrRoamGetSessionIdFromBSSID(pMac, (tCsrBssid *)bssid, &sessionId);
        csrNeighborRoamIndicateConnect(pMac, sessionId, VOS_STATUS_SUCCESS);
    }
#endif
}

static void csrRoamLinkDown(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
   tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );

    if(!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return;
    }
    
   //                                                  
   if( eCSR_BSS_TYPE_INFRASTRUCTURE != pSession->connectedProfile.BSSType )
   {
      return;
   }
   /*                                                                                        */
   csrRoamDeregStatisticsReq(pMac);
   pMac->roam.vccLinkQuality = eCSR_ROAM_LINK_QUAL_POOR_IND;
#if   defined WLAN_FEATURE_NEIGHBOR_ROAMING
   /*                                                                      */
   csrNeighborRoamIndicateDisconnect(pMac, sessionId);
#endif

    //                                                      
    //                          
    if(!IS_FEATURE_SUPPORTED_BY_FW(SLM_SESSIONIZATION) && 
        csrIsInfraApStarted( pMac ) &&  
        pMac->roam.configParam.doBMPSWorkaround)
   {
       pMac->roam.configParam.doBMPSWorkaround = 0;
   }
}

void csrRoamTlStatsTimerHandler(void *pv)
{
   tpAniSirGlobal pMac = PMAC_STRUCT( pv );
   eHalStatus status;
   pMac->roam.tlStatsReqInfo.timerRunning = FALSE;

   smsLog(pMac, LOG1, FL(" TL stat timer is no-op. It needs to support multiple stations"));

#if 0
   //                     
   //                
   if(WLANTL_GetStatistics(pMac->roam.gVosContext, &tlStats, pMac->roam.connectedInfo.staId))
   {
      smsLog(pMac, LOGE, FL("csrRoamTlStatsTimerHandler:couldn't get the stats from TL"));
   }
   else
   {
      //           
      csrRoamSaveStatsFromTl(pMac, tlStats);
   }
#endif
   if(!pMac->roam.tlStatsReqInfo.timerRunning)
   {
      if(pMac->roam.tlStatsReqInfo.periodicity)
      {
         //           
         status = vos_timer_start(&pMac->roam.tlStatsReqInfo.hTlStatsTimer,
                                pMac->roam.tlStatsReqInfo.periodicity);
         if (!HAL_STATUS_SUCCESS(status))
         {
            smsLog(pMac, LOGE, FL("csrRoamTlStatsTimerHandler:cannot start TlStatsTimer timer"));
            return;
         }
         pMac->roam.tlStatsReqInfo.timerRunning = TRUE;
      }
   }
}
void csrRoamPeStatsTimerHandler(void *pv)
{
   tCsrPeStatsReqInfo *pPeStatsReqListEntry = (tCsrPeStatsReqInfo *)pv;
   eHalStatus status;
   tpAniSirGlobal pMac = pPeStatsReqListEntry->pMac;
   VOS_STATUS vosStatus;
   tPmcPowerState powerState;
   pPeStatsReqListEntry->timerRunning = FALSE;
   if( pPeStatsReqListEntry->timerStopFailed == TRUE )
   {
      //                                                                 
      //                                                                  

      /*                   */
      vosStatus = vos_timer_destroy( &pPeStatsReqListEntry->hPeStatsTimer );
      if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) )
      {
         smsLog(pMac, LOGE, FL("csrRoamPeStatsTimerHandler:failed to destroy hPeStatsTimer timer"));
      }

      //               
      vos_mem_free(pPeStatsReqListEntry);
      pPeStatsReqListEntry = NULL;
   }
   else
   {
      if(!pPeStatsReqListEntry->rspPending)
      {
         status = csrSendMBStatsReqMsg(pMac, pPeStatsReqListEntry->statsMask & ~(1 << eCsrGlobalClassDStats), 
                                       pPeStatsReqListEntry->staId);
         if(!HAL_STATUS_SUCCESS(status))
         {
            smsLog(pMac, LOGE, FL("csrRoamPeStatsTimerHandler:failed to send down stats req to PE"));
         }
         else
         {
            pPeStatsReqListEntry->rspPending = TRUE;
         }
      }

      //               
      if(pPeStatsReqListEntry->periodicity && 
         (VOS_TIMER_STATE_STOPPED == vos_timer_getCurrentState(&pPeStatsReqListEntry->hPeStatsTimer)))
      {
         pmcQueryPowerState(pMac, &powerState, NULL, NULL);
         if(ePMC_FULL_POWER == powerState)
         {
            if(pPeStatsReqListEntry->periodicity < pMac->roam.configParam.statsReqPeriodicity)
            {
               pPeStatsReqListEntry->periodicity = pMac->roam.configParam.statsReqPeriodicity;
            }
         }
         else
         {
            if(pPeStatsReqListEntry->periodicity < pMac->roam.configParam.statsReqPeriodicityInPS)
            {
               pPeStatsReqListEntry->periodicity = pMac->roam.configParam.statsReqPeriodicityInPS;
            }
         }
         //           
         vosStatus = vos_timer_start( &pPeStatsReqListEntry->hPeStatsTimer, pPeStatsReqListEntry->periodicity );
         if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) ) 
         {
            smsLog(pMac, LOGE, FL("csrRoamPeStatsTimerHandler:cannot start hPeStatsTimer timer"));
            return;
         }
         pPeStatsReqListEntry->timerRunning = TRUE;

      }

   }
}
void csrRoamStatsClientTimerHandler(void *pv)
{
   tCsrStatsClientReqInfo *pStaEntry = (tCsrStatsClientReqInfo *)pv;
   if(VOS_TIMER_STATE_STOPPED == vos_timer_getCurrentState(&pStaEntry->timer))
   {
#if 0
       //                                
       //               
       vosStatus = vos_timer_start( &pStaEntry->timer, pStaEntry->periodicity );
   
       if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) ) 
       {
          smsLog(pStaEntry->pMac, LOGE, FL("csrGetStatistics:cannot start StatsClient timer"));
       }
#endif       
   }
#if 0
   //                        
   csrRoamReportStatistics(pStaEntry->pMac, pStaEntry->statsMask, pStaEntry->callback, 
                           pStaEntry->staId, pStaEntry->pContext);
#endif
}



eHalStatus csrSendMBStatsReqMsg( tpAniSirGlobal pMac, tANI_U32 statsMask, tANI_U8 staId)
{
   tAniGetPEStatsReq *pMsg;
   eHalStatus status = eHAL_STATUS_SUCCESS;
   status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof(tAniGetPEStatsReq));
   if ( !HAL_STATUS_SUCCESS(status) ) 
   {
      smsLog(pMac, LOG1, " csrSendMBStatsReqMsg: failed to allocate mem for stats req ");
      return status;
   }
   //                                       
   pMsg->msgType = pal_cpu_to_be16((tANI_U16)eWNI_SME_GET_STATISTICS_REQ);
   pMsg->msgLen = (tANI_U16)sizeof(tAniGetPEStatsReq);
   pMsg->staId = staId;
   pMsg->statsMask = statsMask;
   status = palSendMBMessage(pMac->hHdd, pMsg );    
   if(!HAL_STATUS_SUCCESS(status))
   {
      smsLog(pMac, LOG1, " csrSendMBStatsReqMsg: failed to send down the stats req ");
   }
   return status;
}
void csrRoamStatsRspProcessor(tpAniSirGlobal pMac, tSirSmeRsp *pSirMsg)
{
   tAniGetPEStatsRsp *pSmeStatsRsp;
   eHalStatus status = eHAL_STATUS_FAILURE;
   tListElem *pEntry = NULL;
   tCsrStatsClientReqInfo *pTempStaEntry = NULL;
   tCsrPeStatsReqInfo *pPeStaEntry = NULL;
   tANI_U32  tempMask = 0;
   tANI_U8 counter = 0;
   tANI_U8 *pStats = NULL;
   tANI_U32   length = 0;
   v_PVOID_t  pvosGCtx;
   v_S7_t     rssi = 0, snr = 0;
   tANI_U32   *pRssi = NULL, *pSnr = NULL;
   tANI_U32   linkCapacity;
   pSmeStatsRsp = (tAniGetPEStatsRsp *)pSirMsg;
   if(pSmeStatsRsp->rc)
   {
      smsLog( pMac, LOGW, FL("csrRoamStatsRspProcessor:stats rsp from PE shows failure"));
      goto post_update;
   }
   tempMask = pSmeStatsRsp->statsMask;
   pStats = ((tANI_U8 *)&pSmeStatsRsp->statsMask) + sizeof(pSmeStatsRsp->statsMask);
   /*                                                                           
                                                                                       
                              
    */
   length = pSmeStatsRsp->msgLen - sizeof(tAniGetPEStatsRsp);
   //                                                            
   while(tempMask)
   {
      if(tempMask & 1)
      {
         switch(counter)
         {
         case eCsrSummaryStats:
            smsLog( pMac, LOG2, FL("csrRoamStatsRspProcessor:summary stats"));
            status = palCopyMemory(pMac->hHdd, (tANI_U8 *)&pMac->roam.summaryStatsInfo, 
                                   pStats, sizeof(tCsrSummaryStatsInfo));
            if(!HAL_STATUS_SUCCESS(status))
            {
               smsLog( pMac, LOGW, FL("csrRoamStatsRspProcessor:failed to copy summary stats"));
            }
            pStats += sizeof(tCsrSummaryStatsInfo);
            length -= sizeof(tCsrSummaryStatsInfo);
            break;
         case eCsrGlobalClassAStats:
            smsLog( pMac, LOG2, FL("csrRoamStatsRspProcessor:ClassA stats"));
            status = palCopyMemory(pMac->hHdd, (tANI_U8 *)&pMac->roam.classAStatsInfo, 
                                   pStats, sizeof(tCsrGlobalClassAStatsInfo));
            if(!HAL_STATUS_SUCCESS(status))
            {
               smsLog( pMac, LOGW, FL("csrRoamStatsRspProcessor:failed to copy ClassA stats"));
            }
            pStats += sizeof(tCsrGlobalClassAStatsInfo);
            length -= sizeof(tCsrGlobalClassAStatsInfo);
            break;
         case eCsrGlobalClassBStats:
            smsLog( pMac, LOG2, FL("csrRoamStatsRspProcessor:ClassB stats"));
            status = palCopyMemory(pMac->hHdd, (tANI_U8 *)&pMac->roam.classBStatsInfo, 
                                   pStats, sizeof(tCsrGlobalClassBStatsInfo));
            if(!HAL_STATUS_SUCCESS(status))
            {
               smsLog( pMac, LOGW, FL("csrRoamStatsRspProcessor:failed to copy ClassB stats"));
            }
            pStats += sizeof(tCsrGlobalClassBStatsInfo);
            length -= sizeof(tCsrGlobalClassBStatsInfo);
            break;
         case eCsrGlobalClassCStats:
            smsLog( pMac, LOG2, FL("csrRoamStatsRspProcessor:ClassC stats"));
            status = palCopyMemory(pMac->hHdd, (tANI_U8 *)&pMac->roam.classCStatsInfo, 
                                   pStats, sizeof(tCsrGlobalClassCStatsInfo));
            if(!HAL_STATUS_SUCCESS(status))
            {
               smsLog( pMac, LOGW, FL("csrRoamStatsRspProcessor:failed to copy ClassC stats"));
            }
            pStats += sizeof(tCsrGlobalClassCStatsInfo);
            length -= sizeof(tCsrGlobalClassCStatsInfo);
            break;
         case eCsrPerStaStats:
            smsLog( pMac, LOG2, FL("csrRoamStatsRspProcessor:PerSta stats"));
            if( CSR_MAX_STA > pSmeStatsRsp->staId )
            {
               status = palCopyMemory(pMac->hHdd, (tANI_U8 *)&pMac->roam.perStaStatsInfo[pSmeStatsRsp->staId], 
                                      pStats, sizeof(tCsrPerStaStatsInfo));
            }
            else
            {
               status = eHAL_STATUS_FAILURE;
               smsLog( pMac, LOGE, FL("csrRoamStatsRspProcessor:out bound staId:%d"), pSmeStatsRsp->staId);
               VOS_ASSERT( 0 );
            }
            if(!HAL_STATUS_SUCCESS(status))
            {
               smsLog( pMac, LOGW, FL("csrRoamStatsRspProcessor:failed to copy PerSta stats"));
            }
            pStats += sizeof(tCsrPerStaStatsInfo);
            length -= sizeof(tCsrPerStaStatsInfo);
            break;
         default:
            smsLog( pMac, LOGW, FL("csrRoamStatsRspProcessor:unknown stats type"));
            break;
         }
      }
      tempMask >>=1;
      counter++;
   }
   pvosGCtx = vos_get_global_context(VOS_MODULE_ID_SME, pMac);
   if (length != 0)
   {
       pRssi = (tANI_U32*)pStats;
       rssi = (v_S7_t)*pRssi;
       pStats += sizeof(tANI_U32);
       length -= sizeof(tANI_U32);
   }
   else
   {
       /*                                                       */
       rssi = RSSI_HACK_BMPS;
   }

   WDA_UpdateRssiBmps(pvosGCtx, pSmeStatsRsp->staId, rssi);

   if (length != 0)
   {
       linkCapacity = *(tANI_U32*)pStats;
       pStats += sizeof(tANI_U32);
       length -= sizeof(tANI_U32);
   }
   else
   {
       linkCapacity = 0;
   }

   WDA_UpdateLinkCapacity(pvosGCtx, pSmeStatsRsp->staId, linkCapacity);

   if (length != 0)
   {
       pSnr = (tANI_U32*)pStats;
       snr = (v_S7_t)*pSnr;
   }
   else
   {
       snr = SNR_HACK_BMPS;
   }

   WDA_UpdateSnrBmps(pvosGCtx, pSmeStatsRsp->staId, snr);
post_update:   
   //                                          
   pEntry = csrRoamFindInPeStatsReqList(pMac, pSmeStatsRsp->statsMask);
   if(pEntry)
      {
      pPeStaEntry = GET_BASE_ADDR( pEntry, tCsrPeStatsReqInfo, link );
      pPeStaEntry->rspPending = FALSE;
   
   }
   //                         
   pEntry = csrRoamCheckClientReqList(pMac, pSmeStatsRsp->statsMask);
   if(pEntry)
   {
      pTempStaEntry = GET_BASE_ADDR( pEntry, tCsrStatsClientReqInfo, link );
      if(pTempStaEntry->timerExpired)
      {
         //                        
         csrRoamReportStatistics(pMac, pTempStaEntry->statsMask, pTempStaEntry->callback, 
                                 pTempStaEntry->staId, pTempStaEntry->pContext);
         //                                
         csrRoamRemoveStatListEntry(pMac, pEntry);
         pTempStaEntry = NULL;
      }
   }
}
tListElem * csrRoamFindInPeStatsReqList(tpAniSirGlobal pMac, tANI_U32  statsMask)
{
   tListElem *pEntry = NULL;
   tCsrPeStatsReqInfo *pTempStaEntry = NULL;
   pEntry = csrLLPeekHead( &pMac->roam.peStatsReqList, LL_ACCESS_LOCK );
   if(!pEntry)
   {
      //          
      smsLog(pMac, LOG2, "csrRoamFindInPeStatsReqList: List empty, no request to PE");
      return NULL;
   }
   while( pEntry )
   {
      pTempStaEntry = GET_BASE_ADDR( pEntry, tCsrPeStatsReqInfo, link );
      if(pTempStaEntry->statsMask == statsMask)
      {
         smsLog(pMac, LOG3, "csrRoamFindInPeStatsReqList: match found");
         break;
      }
      pEntry = csrLLNext( &pMac->roam.peStatsReqList, pEntry, LL_ACCESS_NOLOCK );
   }
   return pEntry;
}

tListElem * csrRoamChecknUpdateClientReqList(tpAniSirGlobal pMac, tCsrStatsClientReqInfo *pStaEntry,
                                             tANI_BOOLEAN update)
{
   tListElem *pEntry;
   tCsrStatsClientReqInfo *pTempStaEntry;
   pEntry = csrLLPeekHead( &pMac->roam.statsClientReqList, LL_ACCESS_LOCK );
   if(!pEntry)
   {
      //          
      smsLog(pMac, LOG2, "csrRoamChecknUpdateClientReqList: List empty, no request from "
             "upper layer client(s)");
      return NULL;
   }
   while( pEntry )
   {
      pTempStaEntry = GET_BASE_ADDR( pEntry, tCsrStatsClientReqInfo, link );
      if((pTempStaEntry->requesterId == pStaEntry->requesterId) && 
         (pTempStaEntry->statsMask == pStaEntry->statsMask))
      {
         smsLog(pMac, LOG3, "csrRoamChecknUpdateClientReqList: match found");
         if(update)
         {
            pTempStaEntry->periodicity = pStaEntry->periodicity;
            pTempStaEntry->callback = pStaEntry->callback;
            pTempStaEntry->pContext = pStaEntry->pContext;
         }
         break;
      }
      pEntry = csrLLNext( &pMac->roam.statsClientReqList, pEntry, LL_ACCESS_NOLOCK );
   }
   return pEntry;
}
tListElem * csrRoamCheckClientReqList(tpAniSirGlobal pMac, tANI_U32 statsMask)
{
   tListElem *pEntry;
   tCsrStatsClientReqInfo *pTempStaEntry;
   pEntry = csrLLPeekHead( &pMac->roam.statsClientReqList, LL_ACCESS_LOCK );
   if(!pEntry)
   {
      //          
      smsLog(pMac, LOG2, "csrRoamCheckClientReqList: List empty, no request from "
             "upper layer client(s)");
      return NULL;
   }
   while( pEntry )
   {
      pTempStaEntry = GET_BASE_ADDR( pEntry, tCsrStatsClientReqInfo, link );
      if((pTempStaEntry->statsMask & ~(1 << eCsrGlobalClassDStats))  == statsMask)
      {
         smsLog(pMac, LOG3, "csrRoamCheckClientReqList: match found");
         break;
      }
      pEntry = csrLLNext( &pMac->roam.statsClientReqList, pEntry, LL_ACCESS_NOLOCK );
   }
   return pEntry;
}
eHalStatus csrRoamRegisterLinkQualityIndCallback(tpAniSirGlobal pMac,
                                                 csrRoamLinkQualityIndCallback   callback,  
                                                 void                           *pContext)
{
   pMac->roam.linkQualityIndInfo.callback = callback;
   pMac->roam.linkQualityIndInfo.context = pContext;
   if( NULL == callback )
   {
     smsLog(pMac, LOGW, "csrRoamRegisterLinkQualityIndCallback: indication callback being deregistered");
   }
   else
   {
     smsLog(pMac, LOGW, "csrRoamRegisterLinkQualityIndCallback: indication callback being registered");
     /*                                                                         */
   }
   return eHAL_STATUS_SUCCESS;
}
void csrRoamVccTrigger(tpAniSirGlobal pMac)
{
   eCsrRoamLinkQualityInd newVccLinkQuality;
   tANI_U32 ul_mac_loss = 0;
   tANI_U32 ul_mac_loss_trigger_threshold;
 /*                                                                         */
   /*                                                                         
                                                                          
                                                                      
                                                                            */
   ul_mac_loss_trigger_threshold = 
      pMac->roam.configParam.vccUlMacLossThreshold;
   VOS_ASSERT( ul_mac_loss_trigger_threshold != 0 );
   smsLog(pMac, LOGW, "csrRoamVccTrigger: UL_MAC_LOSS_THRESHOLD is %d",
          ul_mac_loss_trigger_threshold );
   if(ul_mac_loss_trigger_threshold < ul_mac_loss)
   {
      smsLog(pMac, LOGW, "csrRoamVccTrigger: link quality is POOR ");
      newVccLinkQuality = eCSR_ROAM_LINK_QUAL_POOR_IND;
   }
   else
   {
      smsLog(pMac, LOGW, "csrRoamVccTrigger: link quality is GOOD");
      newVccLinkQuality = eCSR_ROAM_LINK_QUAL_GOOD_IND;
   }
   smsLog(pMac, LOGW, "csrRoamVccTrigger: link qual : *** UL_MAC_LOSS %d *** ",
          ul_mac_loss);
   if(newVccLinkQuality != pMac->roam.vccLinkQuality)
   {
      smsLog(pMac, LOGW, "csrRoamVccTrigger: link quality changed: trigger necessary");
      if(NULL != pMac->roam.linkQualityIndInfo.callback) 
      {
         smsLog(pMac, LOGW, "csrRoamVccTrigger: link quality indication %d",
                newVccLinkQuality );
         
         /*                                                                     */
         pMac->roam.linkQualityIndInfo.callback( newVccLinkQuality, 
                                                 pMac->roam.linkQualityIndInfo.context );
         //                     
      }
   }
   pMac->roam.vccLinkQuality = newVccLinkQuality;

}
VOS_STATUS csrRoamVccTriggerRssiIndCallback(tHalHandle hHal, 
                                            v_U8_t  rssiNotification, 
                                            void * context)
{
   tpAniSirGlobal pMac = PMAC_STRUCT( context );
   eCsrRoamLinkQualityInd newVccLinkQuality;
        //                                
        tANI_U32 sessionId = 0;
   VOS_STATUS status = VOS_STATUS_SUCCESS;
   /*                                                                         
                                                                          
                                                                      
                                                                            */
   smsLog(pMac, LOGW, "csrRoamVccTriggerRssiIndCallback: RSSI trigger threshold is %d",
          pMac->roam.configParam.vccRssiThreshold);
   if(!csrIsConnStateConnectedInfra(pMac, sessionId))
   {
      smsLog(pMac, LOGW, "csrRoamVccTriggerRssiIndCallback: ignoring the indication as we are not connected");
      return VOS_STATUS_SUCCESS;
   }
   if(WLANTL_HO_THRESHOLD_DOWN == rssiNotification)
   {
      smsLog(pMac, LOGW, "csrRoamVccTriggerRssiIndCallback: link quality is POOR");
      newVccLinkQuality = eCSR_ROAM_LINK_QUAL_POOR_IND;
   }
   else if(WLANTL_HO_THRESHOLD_UP == rssiNotification)
   {
      smsLog(pMac, LOGW, "csrRoamVccTriggerRssiIndCallback: link quality is GOOD ");
      newVccLinkQuality = eCSR_ROAM_LINK_QUAL_GOOD_IND;
   }
   else
   {
      smsLog(pMac, LOGW, "csrRoamVccTriggerRssiIndCallback: unknown rssi notification %d", rssiNotification);
      //                                               
      newVccLinkQuality = pMac->roam.vccLinkQuality;    
      VOS_ASSERT(0);
   }

   if(newVccLinkQuality != pMac->roam.vccLinkQuality)
   {
      smsLog(pMac, LOGW, "csrRoamVccTriggerRssiIndCallback: link quality changed: trigger necessary");
      if(NULL != pMac->roam.linkQualityIndInfo.callback) 
      {
         smsLog(pMac, LOGW, "csrRoamVccTriggerRssiIndCallback: link quality indication %d",
                newVccLinkQuality);
        /*                                                                     */
        pMac->roam.linkQualityIndInfo.callback( newVccLinkQuality, 
                                                pMac->roam.linkQualityIndInfo.context );
         //                     
      }
   }
   pMac->roam.vccLinkQuality = newVccLinkQuality;
   return status;
}
tCsrStatsClientReqInfo * csrRoamInsertEntryIntoList( tpAniSirGlobal pMac,
                                                     tDblLinkList *pStaList,
                                                     tCsrStatsClientReqInfo *pStaEntry)
{
   tCsrStatsClientReqInfo *pNewStaEntry = NULL;
   eHalStatus  status;
   //                                                                            
   //                   
   if(NULL == csrRoamChecknUpdateClientReqList(pMac, pStaEntry, TRUE))
   {
   
      status = palAllocateMemory(pMac->hHdd, (void **)&pNewStaEntry, sizeof(tCsrStatsClientReqInfo));
      if (!HAL_STATUS_SUCCESS(status))
      {
         smsLog(pMac, LOGW, "csrRoamInsertEntryIntoList: couldn't allocate memory for the "
                "entry");
         return NULL;
      }
   
      pNewStaEntry->callback = pStaEntry->callback;
      pNewStaEntry->pContext = pStaEntry->pContext;
      pNewStaEntry->periodicity = pStaEntry->periodicity;
      pNewStaEntry->requesterId = pStaEntry->requesterId;
      pNewStaEntry->statsMask = pStaEntry->statsMask;
      pNewStaEntry->pPeStaEntry = pStaEntry->pPeStaEntry;
      pNewStaEntry->pMac = pStaEntry->pMac;
      pNewStaEntry->staId = pStaEntry->staId;
      pNewStaEntry->timerExpired = pStaEntry->timerExpired;
      
      csrLLInsertTail( pStaList, &pNewStaEntry->link, LL_ACCESS_LOCK  );
   }
   return pNewStaEntry;
}

tCsrPeStatsReqInfo * csrRoamInsertEntryIntoPeStatsReqList( tpAniSirGlobal pMac,
                                                           tDblLinkList *pStaList,
                                                           tCsrPeStatsReqInfo *pStaEntry)
{
   tCsrPeStatsReqInfo *pNewStaEntry = NULL;
   eHalStatus  status;
   status = palAllocateMemory(pMac->hHdd, (void **)&pNewStaEntry, sizeof(tCsrPeStatsReqInfo));
   if (!HAL_STATUS_SUCCESS(status))
   {
      smsLog(pMac, LOGW, "csrRoamInsertEntryIntoPeStatsReqList: couldn't allocate memory for the "
                  "entry");
      return NULL;
   }
   
   pNewStaEntry->hPeStatsTimer = pStaEntry->hPeStatsTimer;
   pNewStaEntry->numClient = pStaEntry->numClient;
   pNewStaEntry->periodicity = pStaEntry->periodicity;
   pNewStaEntry->statsMask = pStaEntry->statsMask;
   pNewStaEntry->pMac = pStaEntry->pMac;
   pNewStaEntry->staId = pStaEntry->staId;
   pNewStaEntry->timerRunning = pStaEntry->timerRunning;
   pNewStaEntry->rspPending = pStaEntry->rspPending;
   
   csrLLInsertTail( pStaList, &pNewStaEntry->link, LL_ACCESS_LOCK  );
   return pNewStaEntry;
}
eHalStatus csrGetRssi(tpAniSirGlobal pMac, 
                            tCsrRssiCallback callback, 
                            tANI_U8 staId, tCsrBssid bssId, void *pContext, void* pVosContext)
{  
   eHalStatus status = eHAL_STATUS_SUCCESS;
   vos_msg_t  msg;
   tANI_U32 sessionId;

   tAniGetRssiReq *pMsg;
   smsLog(pMac, LOG2, FL("called"));
   status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof(tAniGetRssiReq));
   if ( !HAL_STATUS_SUCCESS(status) ) 
   {
      smsLog(pMac, LOGE, " csrGetRssi: failed to allocate mem for req ");
      return status;
   }
   csrRoamGetSessionIdFromBSSID(pMac, (tCsrBssid *)bssId, &sessionId);

   pMsg->msgType = pal_cpu_to_be16((tANI_U16)eWNI_SME_GET_RSSI_REQ);
   pMsg->msgLen = (tANI_U16)sizeof(tAniGetRssiReq);
   pMsg->sessionId = sessionId;
   pMsg->staId = staId;
   pMsg->rssiCallback = callback;
   pMsg->pDevContext = pContext;
   pMsg->pVosContext = pVosContext;
   msg.type = eWNI_SME_GET_RSSI_REQ;
   msg.bodyptr = pMsg;
   msg.reserved = 0;
   if(VOS_STATUS_SUCCESS != vos_mq_post_message(VOS_MQ_ID_SME, &msg))
   {
       smsLog(pMac, LOGE, " csrGetRssi failed to post msg to self ");
       palFreeMemory(pMac->hHdd, (void *)pMsg);
       status = eHAL_STATUS_FAILURE;
   }
   smsLog(pMac, LOG2, FL("returned"));
   return status;
}

eHalStatus csrGetSnr(tpAniSirGlobal pMac,
                     tCsrSnrCallback callback,
                     tANI_U8 staId, tCsrBssid bssId,
                     void *pContext)
{
   eHalStatus status = eHAL_STATUS_SUCCESS;
   vos_msg_t  msg;
   tANI_U32 sessionId;

   tAniGetSnrReq *pMsg;

   smsLog(pMac, LOG2, FL("called"));

   pMsg =(tAniGetSnrReq *)vos_mem_malloc(sizeof(tAniGetSnrReq));
   if (NULL == pMsg )
   {
      smsLog(pMac, LOGE, "%s: failed to allocate mem for req",__func__);
      return status;
   }

   csrRoamGetSessionIdFromBSSID(pMac, (tCsrBssid *)bssId, &sessionId);

   pMsg->msgType = pal_cpu_to_be16((tANI_U16)eWNI_SME_GET_SNR_REQ);
   pMsg->msgLen = (tANI_U16)sizeof(tAniGetSnrReq);
   pMsg->sessionId = sessionId;
   pMsg->staId = staId;
   pMsg->snrCallback = callback;
   pMsg->pDevContext = pContext;
   msg.type = eWNI_SME_GET_SNR_REQ;
   msg.bodyptr = pMsg;
   msg.reserved = 0;

   if (VOS_STATUS_SUCCESS != vos_mq_post_message(VOS_MQ_ID_SME, &msg))
   {
       smsLog(pMac, LOGE, "%s failed to post msg to self", __func__);
       vos_mem_free((v_VOID_t *)pMsg);
       status = eHAL_STATUS_FAILURE;
   }

   smsLog(pMac, LOG2, FL("returned"));
   return status;
}

#if defined WLAN_FEATURE_VOWIFI_11R || defined FEATURE_WLAN_CCX || defined(FEATURE_WLAN_LFR)
eHalStatus csrGetRoamRssi(tpAniSirGlobal pMac,
                            tCsrRssiCallback callback,
                            tANI_U8 staId, tCsrBssid bssId, void *pContext, void* pVosContext)
{
   eHalStatus status = eHAL_STATUS_SUCCESS;
   tAniGetRssiReq *pMsg;

   status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof(tAniGetRssiReq));
   if ( !HAL_STATUS_SUCCESS(status) )
   {
      smsLog(pMac, LOGE, " csrGetRoamRssi: failed to allocate mem for req");
      return status;
   }
   //                                       
   pMsg->msgType = pal_cpu_to_be16((tANI_U16)eWNI_SME_GET_ROAM_RSSI_REQ);
   pMsg->msgLen = (tANI_U16)sizeof(tAniGetRssiReq);
   pMsg->staId = staId;
   pMsg->rssiCallback = callback;
   pMsg->pDevContext = pContext;
   pMsg->pVosContext = pVosContext;
   status = palSendMBMessage(pMac->hHdd, pMsg );
   if(!HAL_STATUS_SUCCESS(status))
   {
      smsLog(pMac, LOG1, " csrGetRoamRssi: failed to send down the rssi req");
      //                                 
      status = eHAL_STATUS_FAILURE;
   }
   return status;
}
#endif



#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
eHalStatus csrGetTsmStats(tpAniSirGlobal pMac,
                          tCsrTsmStatsCallback callback,
                          tANI_U8 staId,
                          tCsrBssid bssId,
                          void *pContext,
                          void* pVosContext,
                          tANI_U8 tid)
{
   eHalStatus          status = eHAL_STATUS_SUCCESS;
   tAniGetTsmStatsReq *pMsg = NULL;

   status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof(tAniGetTsmStatsReq));
   if ( !HAL_STATUS_SUCCESS(status) )
   {
      smsLog(pMac, LOGE, "csrGetTsmStats: failed to allocate mem for req");
      return status;
   }
   //                                       
   pMsg->msgType = pal_cpu_to_be16((tANI_U16)eWNI_SME_GET_TSM_STATS_REQ);
   pMsg->msgLen = (tANI_U16)sizeof(tAniGetTsmStatsReq);
   pMsg->staId = staId;
   pMsg->tid = tid;
   palCopyMemory(pMac->hHdd, pMsg->bssId, bssId, sizeof(tSirMacAddr));
   pMsg->tsmStatsCallback = callback;
   pMsg->pDevContext = pContext;
   pMsg->pVosContext = pVosContext;
   status = palSendMBMessage(pMac->hHdd, pMsg );
   if(!HAL_STATUS_SUCCESS(status))
   {
      smsLog(pMac, LOG1, " csrGetTsmStats: failed to send down the rssi req");
      //                                 
      status = eHAL_STATUS_FAILURE;
   }
   return status;
}
#endif  /*                                             */


/*                                                                            
                        
                                                                             

                                                   
                                            
                                 
                                                                             */
tANI_U16 csrGetTLSTAState(tpAniSirGlobal pMac, tANI_U8 staId)
{
   WLANTL_STAStateType tlSTAState;
   tlSTAState = WLANTL_STA_INIT;

   //                        
   if ( !VOS_IS_STATUS_SUCCESS(WLANTL_GetSTAState(pMac->roam.gVosContext, staId, &tlSTAState)) )
   {
      smsLog(pMac, LOGE, FL("csrGetTLSTAState:couldn't get the STA state from TL"));
   }

   return tlSTAState;
}

eHalStatus csrGetStatistics(tpAniSirGlobal pMac, eCsrStatsRequesterType requesterId, 
                            tANI_U32 statsMask, 
                            tCsrStatsCallback callback, 
                            tANI_U32 periodicity, tANI_BOOLEAN cache, 
                            tANI_U8 staId, void *pContext)
{  
   tCsrStatsClientReqInfo staEntry;
   tCsrStatsClientReqInfo *pStaEntry = NULL;
   tCsrPeStatsReqInfo *pPeStaEntry = NULL; 
   tListElem *pEntry = NULL;
   tANI_BOOLEAN found = FALSE;
   eHalStatus status = eHAL_STATUS_SUCCESS;
   tANI_BOOLEAN insertInClientList = FALSE;
   VOS_STATUS vosStatus;
   WLANTL_TRANSFER_STA_TYPE *pTlStats;

   if( csrIsAllSessionDisconnected(pMac) )
   {
      //                                                                                                    
      return eHAL_STATUS_FAILURE;
   }
   if((!statsMask) && (!callback))
   {
      //   
      smsLog(pMac, LOGW, "csrGetStatistics: statsMask & callback empty in the request");
      return eHAL_STATUS_FAILURE;
   }
   //                                         
   staEntry.requesterId = requesterId;
   staEntry.statsMask = statsMask;
   //                                              
   if((statsMask) && (!callback))
   {
      pEntry = csrRoamChecknUpdateClientReqList(pMac, &staEntry, FALSE);
      if(!pEntry)
      {
         //   
         smsLog(pMac, LOGW, "csrGetStatistics: callback is empty in the request & couldn't "
                "find any existing request in statsClientReqList");
         return eHAL_STATUS_FAILURE;
      }
      else
      {
         //                 
         pStaEntry = GET_BASE_ADDR( pEntry, tCsrStatsClientReqInfo, link );
         if(NULL != pStaEntry->pPeStaEntry)
         {
            pStaEntry->pPeStaEntry->numClient--;
            //                                                            
            if(!pStaEntry->pPeStaEntry->numClient)
            {
               csrRoamRemoveEntryFromPeStatsReqList(pMac, pStaEntry->pPeStaEntry);
            }
         }

         //                                                
         pMac->roam.tlStatsReqInfo.numClient--;
         if(!pMac->roam.tlStatsReqInfo.numClient)
         {
            if(pMac->roam.tlStatsReqInfo.timerRunning)
            {
               status = vos_timer_stop(&pMac->roam.tlStatsReqInfo.hTlStatsTimer);
               if (!HAL_STATUS_SUCCESS(status))
               {
                  smsLog(pMac, LOGE, FL("csrGetStatistics:cannot stop TlStatsTimer timer"));
                  return eHAL_STATUS_FAILURE;
               }
            }
            pMac->roam.tlStatsReqInfo.periodicity = 0;
            pMac->roam.tlStatsReqInfo.timerRunning = FALSE;
         }
         vos_timer_stop( &pStaEntry->timer );
         //                               
         vosStatus = vos_timer_destroy( &pStaEntry->timer );
         if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) )
         {
            smsLog(pMac, LOGE, FL("csrGetStatistics:failed to destroy Client req timer"));
         }
         csrRoamRemoveStatListEntry(pMac, pEntry);
         pStaEntry = NULL;
         return eHAL_STATUS_SUCCESS;
      }
   }
   
   if(cache && !periodicity)
   {
      //                       
      csrRoamReportStatistics(pMac, statsMask, callback, staId, pContext);
   }
   else
   {
      //                                      
      staEntry.callback = callback;
      staEntry.pContext = pContext;
      staEntry.periodicity = periodicity;
      staEntry.pPeStaEntry = NULL;
      staEntry.staId = staId;
      staEntry.pMac = pMac;
      staEntry.timerExpired = FALSE;
   
   
      //                                                              
      if(periodicity)
      {
      
         //                            
         if(statsMask & ~(1 << eCsrGlobalClassDStats))
         {
         
            //                                                    
            pPeStaEntry = csrRoamCheckPeStatsReqList(pMac, statsMask & ~(1 << eCsrGlobalClassDStats), 
                                               periodicity, &found, staId);
            if(!pPeStaEntry)
            {
               //                                           
               return eHAL_STATUS_FAILURE;
            }
            else
            {
               staEntry.pPeStaEntry = pPeStaEntry;
            }
               
         }
         //                                                                                       
         if(statsMask & (1 << eCsrGlobalClassDStats))
         {
            if(cache && pMac->roam.tlStatsReqInfo.numClient)
            {
               smsLog(pMac, LOGE, FL("csrGetStatistics:Looking for cached stats from TL"));
            }
            else
            {
            
               //                  
               if(pMac->roam.tlStatsReqInfo.periodicity)
               {
                  pMac->roam.tlStatsReqInfo.periodicity = 
                     CSR_ROAM_MIN(periodicity, pMac->roam.tlStatsReqInfo.periodicity);
               }
               else
               {
                  pMac->roam.tlStatsReqInfo.periodicity = periodicity;
               }
               if(pMac->roam.tlStatsReqInfo.periodicity < CSR_MIN_TL_STAT_QUERY_PERIOD)
               {
                  pMac->roam.tlStatsReqInfo.periodicity = CSR_MIN_TL_STAT_QUERY_PERIOD;
               }
               
               if(!pMac->roam.tlStatsReqInfo.timerRunning)
               {
                  pTlStats = (WLANTL_TRANSFER_STA_TYPE *)vos_mem_malloc(sizeof(WLANTL_TRANSFER_STA_TYPE));
                  if(NULL != pTlStats)
                  {
                     //                        
                     if(WLANTL_GetStatistics(pMac->roam.gVosContext, pTlStats, staId))
                     {
                        smsLog(pMac, LOGE, FL("csrGetStatistics:couldn't get the stats from TL"));
                     }
                     else
                     {
                        //           
                        csrRoamSaveStatsFromTl(pMac, pTlStats);
                     }
                     vos_mem_free(pTlStats);
                     pTlStats = NULL;
                  }
                  else
                  {
                     smsLog(pMac, LOGE, FL("cannot allocate memory for TL stat"));
                  }

                  if(pMac->roam.tlStatsReqInfo.periodicity)
                  {
                     //           
                     status = vos_timer_start(&pMac->roam.tlStatsReqInfo.hTlStatsTimer,
                                            pMac->roam.tlStatsReqInfo.periodicity);
                     if (!HAL_STATUS_SUCCESS(status))
                     {
                        smsLog(pMac, LOGE, FL("csrGetStatistics:cannot start TlStatsTimer timer"));
                        return eHAL_STATUS_FAILURE;
                     }
                     pMac->roam.tlStatsReqInfo.timerRunning = TRUE;
                  }
               }
            }
            pMac->roam.tlStatsReqInfo.numClient++;
         }
   
         insertInClientList = TRUE;
      }
      //                                                              
      else if(!cache && !periodicity)
      {
         if(statsMask & ~(1 << eCsrGlobalClassDStats))
         {
            //               
            status = csrSendMBStatsReqMsg(pMac, statsMask & ~(1 << eCsrGlobalClassDStats), staId);
            if(!HAL_STATUS_SUCCESS(status))
            {
               smsLog(pMac, LOGE, FL("csrGetStatistics:failed to send down stats req to PE"));
            }
            //                                                                       
            //          
            staEntry.timerExpired = TRUE;
            insertInClientList = TRUE;
         }
         if(statsMask & (1 << eCsrGlobalClassDStats))
         {
            pTlStats = (WLANTL_TRANSFER_STA_TYPE *)vos_mem_malloc(sizeof(WLANTL_TRANSFER_STA_TYPE));
            if(NULL != pTlStats)
            {
               //                        
               if(!VOS_IS_STATUS_SUCCESS(WLANTL_GetStatistics(pMac->roam.gVosContext, pTlStats, staId)))
               {
                  smsLog(pMac, LOGE, FL("csrGetStatistics:couldn't get the stats from TL"));
               }
               else
               {
                  //           
                  csrRoamSaveStatsFromTl(pMac, pTlStats);
               }
               vos_mem_free(pTlStats);
               pTlStats = NULL;
            }
            else
            {
               smsLog(pMac, LOGE, FL("cannot allocate memory for TL stat"));
            }

         }
         //                                  
         if(!insertInClientList)
         {
            //                
            csrRoamReportStatistics(pMac, statsMask, callback, staId, pContext);
         }
      }
      if(insertInClientList)
      {
         pStaEntry = csrRoamInsertEntryIntoList(pMac, &pMac->roam.statsClientReqList, &staEntry); 
         if(!pStaEntry)
         {
            //   
            smsLog(pMac, LOGW, "csrGetStatistics: Failed to insert req in statsClientReqList");
            return eHAL_STATUS_FAILURE;
         }
         pStaEntry->periodicity = periodicity;
         //                            
         if(periodicity)
         {
            vosStatus = vos_timer_init( &pStaEntry->timer, VOS_TIMER_TYPE_SW, 
                                        csrRoamStatsClientTimerHandler, pStaEntry );
            if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) )
            {
               smsLog(pMac, LOGE, FL("csrGetStatistics:cannot init StatsClient timer"));
               return eHAL_STATUS_FAILURE;
            }
            vosStatus = vos_timer_start( &pStaEntry->timer, periodicity );
            if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) ) 
            {
               smsLog(pMac, LOGE, FL("csrGetStatistics:cannot start StatsClient timer"));
               return eHAL_STATUS_FAILURE;
            }
         }
      }
   }
   return eHAL_STATUS_SUCCESS;
}

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD

static tSirRetStatus
csrRoamScanOffloadPopulateMacHeader(tpAniSirGlobal pMac,
                                    tANI_U8* pBD,
                                    tANI_U8 type,
                                    tANI_U8 subType,
                                    tSirMacAddr peerAddr,
                                    tSirMacAddr selfMacAddr)
{
        tSirRetStatus   statusCode = eSIR_SUCCESS;
        tpSirMacMgmtHdr pMacHdr;

        /*                               */
        pMacHdr = (tpSirMacMgmtHdr) (pBD);

        /*            */
        pMacHdr->fc.protVer = SIR_MAC_PROTOCOL_VERSION;
        pMacHdr->fc.type    = type;
        pMacHdr->fc.subType = subType;

        /*                   */
        palCopyMemory( pMac->hHdd,
                        (tANI_U8 *) pMacHdr->da,
                        (tANI_U8 *) peerAddr,
                        sizeof( tSirMacAddr ));

        sirCopyMacAddr(pMacHdr->sa,selfMacAddr);

        /*                   */
        palCopyMemory( pMac->hHdd,
                        (tANI_U8 *) pMacHdr->bssId,
                        (tANI_U8 *) peerAddr,
                        sizeof( tSirMacAddr ));
        return statusCode;
} /*                                           */

static tSirRetStatus
csrRoamScanOffloadPrepareProbeReqTemplate(tpAniSirGlobal pMac,
                                          tANI_U8 nChannelNum,
                                          tANI_U32 dot11mode,
                                          tSirMacAddr selfMacAddr,
                                          tANI_U8 *pFrame,
                                          tANI_U16 *pusLen)
{
        tDot11fProbeRequest pr;
        tANI_U32            nStatus, nBytes, nPayload;
        tSirRetStatus       nSirStatus;
        /*        */
        tSirMacAddr         bssId = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        /*                                                                */


        palZeroMemory( pMac->hHdd, ( tANI_U8* )&pr, sizeof( pr ) );

        PopulateDot11fSuppRates( pMac, nChannelNum, &pr.SuppRates,NULL);

        if ( WNI_CFG_DOT11_MODE_11B != dot11mode )
        {
                PopulateDot11fExtSuppRates1( pMac, nChannelNum, &pr.ExtSuppRates );
        }


        if (IS_DOT11_MODE_HT(dot11mode))
        {
                PopulateDot11fHTCaps( pMac, NULL, &pr.HTCaps );
        }


        nStatus = dot11fGetPackedProbeRequestSize( pMac, &pr, &nPayload );
        if ( DOT11F_FAILED( nStatus ) )
        {
                VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                                "Failed to calculate the packed size f"
                                "or a Probe Request (0x%08x).\n", nStatus );


                nPayload = sizeof( tDot11fProbeRequest );
        }
        else if ( DOT11F_WARNED( nStatus ) )
        {
                VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                                "There were warnings while calculating"
                                "the packed size for a Probe Request ("
                                "0x%08x).\n", nStatus );
        }

        nBytes = nPayload + sizeof( tSirMacMgmtHdr );

        /*                       */
        palZeroMemory( pMac->hHdd, pFrame, nBytes );


        nSirStatus = csrRoamScanOffloadPopulateMacHeader( pMac, pFrame, SIR_MAC_MGMT_FRAME,
                        SIR_MAC_MGMT_PROBE_REQ, bssId,selfMacAddr);

        if ( eSIR_SUCCESS != nSirStatus )
        {
                VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                                "Failed to populate the buffer descriptor for a Probe Request (%d).\n",
                                nSirStatus );
                return nSirStatus;
        }


        nStatus = dot11fPackProbeRequest( pMac, &pr, pFrame +
                        sizeof( tSirMacMgmtHdr ),
                        nPayload, &nPayload );
        if ( DOT11F_FAILED( nStatus ) )
        {
                VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                                "Failed to pack a Probe Request (0x%08x).\n", nStatus );
                return eSIR_FAILURE;
        }
        else if ( DOT11F_WARNED( nStatus ) )
        {
                VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                                "There were warnings while packing a Probe Request (0x%08x).\n" );
        }

        *pusLen = nPayload + sizeof(tSirMacMgmtHdr);
        return eSIR_SUCCESS;
}

eHalStatus csrRoamOffloadScan(tpAniSirGlobal pMac, tANI_U8 command, tANI_U8 reason)
{
   vos_msg_t msg;
   tSirRoamOffloadScanReq *pRequestBuf;
   tpCsrNeighborRoamControlInfo    pNeighborRoamInfo = &pMac->roam.neighborRoamInfo;
   tCsrRoamSession *pSession;
   tANI_U8 i,j,num_channels = 0, ucDot11Mode;
   tANI_U8 *ChannelList = NULL;
   tANI_U32 sessionId;
   eHalStatus status = eHAL_STATUS_SUCCESS;
   tpCsrChannelInfo    currChannelListInfo;
   tANI_U32 host_channels = 0;
   eCsrBand eBand;
   tANI_U8 ChannelCacheStr[128] = {0};
   tSirBssDescription *pBssDesc = NULL;
   tDot11fBeaconIEs *pIes = NULL;
   tANI_U8 minRate = 0, dataRate;
   tANI_U8 operationChannel;

   currChannelListInfo = &pNeighborRoamInfo->roamChannelInfo.currentChannelListInfo;

   if (0 == csrRoamIsRoamOffloadScanEnabled(pMac))
   {
      smsLog( pMac, LOGE,"isRoamOffloadScanEnabled not set");
      return eHAL_STATUS_FAILURE;
   }

   if ((VOS_TRUE == bRoamScanOffloadStarted) && (ROAM_SCAN_OFFLOAD_START == command))
   {
        smsLog( pMac, LOGE,"Roam Scan Offload is already started");
        return eHAL_STATUS_FAILURE;
   }
   status = csrRoamGetSessionIdFromBSSID(pMac,
                            (tCsrBssid *)pNeighborRoamInfo->currAPbssid,
                                        &sessionId);
   /*                                                                        
                                                                              
                                                                                    
                                                                                             
                                                                                           
                                                */
   if ((pMac->roam.neighborRoamInfo.neighborRoamState == eCSR_NEIGHBOR_ROAM_STATE_INIT) &&
       (command != ROAM_SCAN_OFFLOAD_STOP))
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, "Scan Command not sent to FW with state = %d and cmd = %d\n",
              pMac->roam.neighborRoamInfo.neighborRoamState, command);
      return eHAL_STATUS_FAILURE;
   }

   if ( !HAL_STATUS_SUCCESS( status ) )
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: Not able to find the sessionId for Roam Offload scan request", __func__);
      return eHAL_STATUS_FAILURE;
   }
   pSession = CSR_GET_SESSION( pMac, sessionId );
   pBssDesc = pSession->pConnectBssDesc;
   if (pBssDesc == NULL)
   {
      VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: pBssDesc not found for current session", __func__);
      return eHAL_STATUS_FAILURE;
   }
   pRequestBuf = vos_mem_malloc(sizeof(tSirRoamOffloadScanReq));
   if (NULL == pRequestBuf)
   {
           VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: Not able to allocate memory for Roam Offload scan request", __func__);
           return eHAL_STATUS_FAILED_ALLOC;
   }

    vos_mem_zero(pRequestBuf,sizeof(tSirRoamOffloadScanReq));
    /*                                                                                           
                                                         */
    if(command == ROAM_SCAN_OFFLOAD_STOP)
    {
       pRequestBuf->RoamScanOffloadEnabled = 0;
       /*                                       
                                                        
                                                          */
       goto send_roam_scan_offload_cmd;
    }
    else
       pRequestBuf->RoamScanOffloadEnabled = pMac->roam.configParam.isRoamOffloadScanEnabled;
    vos_mem_copy(pRequestBuf->ConnectedNetwork.currAPbssid, pNeighborRoamInfo->currAPbssid,
                 sizeof(tCsrBssid));
    pRequestBuf->ConnectedNetwork.ssId.length =
            pMac->roam.roamSession[sessionId].connectedProfile.SSID.length;
    vos_mem_copy(pRequestBuf->ConnectedNetwork.ssId.ssId,
                 pMac->roam.roamSession[sessionId].connectedProfile.SSID.ssId,
                 pRequestBuf->ConnectedNetwork.ssId.length);
    pRequestBuf->ConnectedNetwork.authentication =
            pMac->roam.roamSession[sessionId].connectedProfile.AuthType;
    pRequestBuf->ConnectedNetwork.encryption =
            pMac->roam.roamSession[sessionId].connectedProfile.EncryptionType;
    pRequestBuf->ConnectedNetwork.mcencryption =
            pMac->roam.roamSession[sessionId].connectedProfile.mcEncryptionType;
    if (pNeighborRoamInfo->cfgParams.neighborLookupThreshold)
    {
       pRequestBuf->LookupThreshold =
            (v_S7_t)pNeighborRoamInfo->cfgParams.neighborLookupThreshold * (-1);
       pRequestBuf->RxSensitivityThreshold = LFR_SENSITIVITY_THR_DEFAULT;
    }
    else
    {
       VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, "Calculate Adaptive Threshold");
       operationChannel = pSession->connectedProfile.operationChannel;

       if (!HAL_STATUS_SUCCESS(csrGetParsedBssDescriptionIEs(pMac, pBssDesc, &pIes)))
       {
          VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
          "%s: csrGetParsedBssDescriptionIEs failed", __func__);
          vos_mem_free(pRequestBuf);
          return eHAL_STATUS_FAILURE;
       }
       if (pIes->SuppRates.present)
       {
          VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, "Number \t  Rate");
          /*                                              */
          for (i = 0; i < pIes->SuppRates.num_rates; i++)
          {
              /*                                     */
              if (csrRatesIsDot11RateSupported(pMac, pIes->SuppRates.rates[i])
                  && (pIes->SuppRates.rates[i] & 0x80))
              {
                  /*                             */
                  dataRate = (pIes->SuppRates.rates[i] & 0x7F)/2;
                  VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, "%d \t\t %d", i, dataRate);
                  if (minRate == 0)
                    minRate = dataRate;
                  else
                    minRate = (minRate < dataRate) ? minRate:dataRate;
              }
          }

          if (pIes->ExtSuppRates.present)
          {
             for (i = 0; i < pIes->ExtSuppRates.num_rates; i++)
             {
                 /*                                     */
                 if (csrRatesIsDot11RateSupported(pMac, pIes->ExtSuppRates.rates[i])
                      && (pIes->ExtSuppRates.rates[i] & 0x80))
                 {
                    /*                             */
                    dataRate = (pIes->ExtSuppRates.rates[i] & 0x7F)/2;
                    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, "%d \t\t %d", i, dataRate);
                    if (minRate == 0)
                      minRate = dataRate;
                    else
                      minRate = (minRate < dataRate) ? minRate:dataRate;
                 }
             }
          }
           VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_DEBUG, "MinRate = %d", minRate);
       }
       else
       {
          VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
          "%s: Supp Rates not present in pIes", __func__);
          vos_mem_free(pRequestBuf);
          return eHAL_STATUS_FAILURE;
       }
       if (NULL != pIes)
       {
          vos_mem_free(pIes);
          pIes = NULL;
       }
       switch (minRate)
       {
          case 1:
            pRequestBuf->RxSensitivityThreshold = LFR_SENSITIVITY_THR_1MBPS;
            pRequestBuf->LookupThreshold = LFR_LOOKUP_THR_1MBPS;
            break;
          case 2:
            pRequestBuf->RxSensitivityThreshold = LFR_SENSITIVITY_THR_2MBPS;
            pRequestBuf->LookupThreshold = LFR_LOOKUP_THR_2MBPS;
            break;
          case 5:
            pRequestBuf->RxSensitivityThreshold = LFR_SENSITIVITY_THR_5_5MBPS;
            pRequestBuf->LookupThreshold = LFR_LOOKUP_THR_5_5MBPS;
            break;
          case 6:
            if (CSR_IS_CHANNEL_24GHZ(operationChannel))
            {
               pRequestBuf->RxSensitivityThreshold = LFR_SENSITIVITY_THR_6MBPS_2G;
               pRequestBuf->LookupThreshold = LFR_LOOKUP_THR_6MBPS_2G;
            }
            else
            {
               pRequestBuf->RxSensitivityThreshold = LFR_SENSITIVITY_THR_6MBPS_5G;
               pRequestBuf->LookupThreshold = LFR_LOOKUP_THR_6MBPS_5G;
            }
            break;
          case 11:
            pRequestBuf->RxSensitivityThreshold = LFR_SENSITIVITY_THR_11MBPS;
            pRequestBuf->LookupThreshold = LFR_LOOKUP_THR_11MBPS;
            break;
          case 12:
            if (CSR_IS_CHANNEL_24GHZ(operationChannel))
            {
               pRequestBuf->RxSensitivityThreshold = LFR_SENSITIVITY_THR_12MBPS_2G;
               pRequestBuf->LookupThreshold = LFR_LOOKUP_THR_12MBPS_2G;
            }
            else
            {
               pRequestBuf->RxSensitivityThreshold = LFR_SENSITIVITY_THR_12MBPS_5G;
               pRequestBuf->LookupThreshold = LFR_LOOKUP_THR_12MBPS_5G;
            }
            break;
          case 24:
            if (CSR_IS_CHANNEL_24GHZ(operationChannel))
            {
               pRequestBuf->RxSensitivityThreshold = LFR_SENSITIVITY_THR_24MBPS_2G;
               pRequestBuf->LookupThreshold = LFR_LOOKUP_THR_24MBPS_2G;
            }
            else
            {
               pRequestBuf->RxSensitivityThreshold = LFR_SENSITIVITY_THR_24MBPS_5G;
               pRequestBuf->LookupThreshold = LFR_LOOKUP_THR_24MBPS_5G;
            }
            break;
          default:
            pRequestBuf->LookupThreshold = LFR_LOOKUP_THR_DEFAULT;
            pRequestBuf->RxSensitivityThreshold = LFR_SENSITIVITY_THR_DEFAULT;
            break;
       }
    }
    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_DEBUG,
                "Chnl=%d,MinRate=%d,RxSenThr=%d,LookupThr=%d",
                operationChannel, minRate,
                pRequestBuf->RxSensitivityThreshold,
                pRequestBuf->LookupThreshold);
    pRequestBuf->RoamRssiDiff =
            pMac->roam.configParam.RoamRssiDiff;
    pRequestBuf->Command = command;
    pRequestBuf->StartScanReason = reason;
    pRequestBuf->NeighborScanTimerPeriod =
            pNeighborRoamInfo->cfgParams.neighborScanPeriod;
    pRequestBuf->NeighborRoamScanRefreshPeriod =
            pNeighborRoamInfo->cfgParams.neighborResultsRefreshPeriod;
    pRequestBuf->NeighborScanChannelMinTime =
            pNeighborRoamInfo->cfgParams.minChannelScanTime;
    pRequestBuf->NeighborScanChannelMaxTime =
            pNeighborRoamInfo->cfgParams.maxChannelScanTime;
    pRequestBuf->EmptyRefreshScanPeriod =
            pNeighborRoamInfo->cfgParams.emptyScanRefreshPeriod;
    /*              */
    pRequestBuf->MAWCEnabled =
            pMac->roam.configParam.MAWCEnabled;
#ifdef FEATURE_WLAN_CCX
    pRequestBuf->IsCCXEnabled = pMac->roam.configParam.isCcxIniFeatureEnabled;
#endif
    if (
#ifdef FEATURE_WLAN_CCX
       ((pNeighborRoamInfo->isCCXAssoc) &&
        (pNeighborRoamInfo->roamChannelInfo.IAPPNeighborListReceived ==
         eANI_BOOLEAN_FALSE)) ||
        (pNeighborRoamInfo->isCCXAssoc == eANI_BOOLEAN_FALSE) ||
#endif //    
        currChannelListInfo->numOfChannels == 0)
    {

      /*                                                                              
                                         */
       if (pNeighborRoamInfo->cfgParams.channelInfo.numOfChannels)
       {
          ChannelList = pNeighborRoamInfo->cfgParams.channelInfo.ChannelList;
          /*                                                                
                                    */
          eBand = pMac->roam.configParam.bandCapability;
          if ((eCSR_BAND_24 != eBand) && (eCSR_BAND_5G != eBand) && (eCSR_BAND_ALL != eBand))
          {
             VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
                       "Invalid band, No operation carried out (Band %d)", eBand);
             vos_mem_free(pRequestBuf);
             return eHAL_STATUS_FAILURE;
          }
          for (i=0; i<pNeighborRoamInfo->cfgParams.channelInfo.numOfChannels; i++)
          {
            if(((eCSR_BAND_24 == eBand) && CSR_IS_CHANNEL_24GHZ(*ChannelList)) ||
              ((eCSR_BAND_5G == eBand) && CSR_IS_CHANNEL_5GHZ(*ChannelList)) ||
              (eCSR_BAND_ALL == eBand))
            {
              if(!CSR_IS_CHANNEL_DFS(*ChannelList) &&
                 csrRoamIsChannelValid(pMac, *ChannelList) &&
                 *ChannelList && (num_channels < SIR_ROAM_MAX_CHANNELS))
              {
                 pRequestBuf->ConnectedNetwork.ChannelCache[num_channels++] = *ChannelList;
              }
            }
            ChannelList++;
          }
          pRequestBuf->ConnectedNetwork.ChannelCount = num_channels;
          pRequestBuf->ChannelCacheType = CHANNEL_LIST_STATIC;
       }
       else
       {
          ChannelList = pMac->scan.occupiedChannels.channelList;
          for(i=0; i<pMac->scan.occupiedChannels.numChannels; i++)
          {
             if(!CSR_IS_CHANNEL_DFS(*ChannelList) &&
                *ChannelList && (num_channels < SIR_ROAM_MAX_CHANNELS))
             {
                pRequestBuf->ConnectedNetwork.ChannelCache[num_channels++] = *ChannelList;
             }
             ChannelList++;
          }
          pRequestBuf->ConnectedNetwork.ChannelCount = num_channels;
          /*                                                                        
                                                                                             
                                                                  */
          if (reason == REASON_FLUSH_CHANNEL_LIST)
              pRequestBuf->ChannelCacheType = CHANNEL_LIST_DYNAMIC_FLUSH;
          else {
                 if (csrNeighborRoamIsNewConnectedProfile(pMac))
                     pRequestBuf->ChannelCacheType = CHANNEL_LIST_DYNAMIC_INIT;
                 else
                     pRequestBuf->ChannelCacheType = CHANNEL_LIST_DYNAMIC_UPDATE;
          }
       }
    }
#ifdef FEATURE_WLAN_CCX
    else
    {
      /*                                                          
                                                                       
                                                              */
       if (currChannelListInfo->numOfChannels != 0)
       {
          ChannelList = currChannelListInfo->ChannelList;
          for (i=0;i<currChannelListInfo->numOfChannels;i++)
          {
           if(!CSR_IS_CHANNEL_DFS(*ChannelList) && *ChannelList)
           {
            pRequestBuf->ConnectedNetwork.ChannelCache[num_channels++] = *ChannelList;
           }
           ChannelList++;
          }
          pRequestBuf->ConnectedNetwork.ChannelCount = num_channels;
          pRequestBuf->ChannelCacheType = CHANNEL_LIST_DYNAMIC_UPDATE;
       }
     }
#endif
    for (i = 0, j = 0;j < (sizeof(ChannelCacheStr)/sizeof(ChannelCacheStr[0])) && i < pRequestBuf->ConnectedNetwork.ChannelCount; i++)
    {
            j += snprintf(ChannelCacheStr + j, sizeof(ChannelCacheStr) - j," %d",
                          pRequestBuf->ConnectedNetwork.ChannelCache[i]);
    }
    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_DEBUG,
              "ChnlCacheType:%d, No of Chnls:%d,Channels: %s",
              pRequestBuf->ChannelCacheType,
              pRequestBuf->ConnectedNetwork.ChannelCount,
              ChannelCacheStr);
    num_channels = 0;
    ChannelList = NULL;

    /*                                 */
    host_channels = sizeof(pMac->roam.validChannelList);
    if (HAL_STATUS_SUCCESS(csrGetCfgValidChannels(pMac, pMac->roam.validChannelList, &host_channels)))
    {
        ChannelList = pMac->roam.validChannelList;
        pMac->roam.numValidChannels = host_channels;
    }
    else
    {
        VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR,
              "%s:Failed to get the valid channel list", __func__);
        vos_mem_free(pRequestBuf);
        return eHAL_STATUS_FAILURE;
    }
    for(i=0; i<pMac->roam.numValidChannels; i++)
    {
        if(!CSR_IS_CHANNEL_DFS(*ChannelList) && *ChannelList)
        {
            pRequestBuf->ValidChannelList[num_channels++] = *ChannelList;
        }
        ChannelList++;
    }
    pRequestBuf->ValidChannelCount = num_channels;

    pRequestBuf->MDID.mdiePresent =
            pMac->roam.roamSession[sessionId].connectedProfile.MDID.mdiePresent;
    pRequestBuf->MDID.mobilityDomain =
            pMac->roam.roamSession[sessionId].connectedProfile.MDID.mobilityDomain;
    pRequestBuf->nProbes = pMac->roam.configParam.nProbes;

    pRequestBuf->HomeAwayTime = pMac->roam.configParam.nRoamScanHomeAwayTime;
    /*                                                                      
                                                                          
                                                              */
    if (pRequestBuf->HomeAwayTime < (pRequestBuf->NeighborScanChannelMaxTime + (2 * CSR_ROAM_SCAN_CHANNEL_SWITCH_TIME)))
    {
        VOS_TRACE( VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_WARN,
                  "%s: Invalid config, Home away time(%d) is less than (twice RF switching time + channel max time)(%d)"
                  " Hence enforcing home away time to disable (0)",
                  __func__, pRequestBuf->HomeAwayTime,
                  (pRequestBuf->NeighborScanChannelMaxTime + (2 * CSR_ROAM_SCAN_CHANNEL_SWITCH_TIME)));
        pRequestBuf->HomeAwayTime = 0;
    }
    VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_DEBUG,"HomeAwayTime:%d",pRequestBuf->HomeAwayTime);

   /*                                                             */
    ucDot11Mode = (tANI_U8) csrTranslateToWNICfgDot11Mode(pMac,
                                                           csrFindBestPhyMode( pMac, pMac->roam.configParam.phyMode ));
   csrRoamScanOffloadPrepareProbeReqTemplate(pMac,SIR_ROAM_SCAN_24G_DEFAULT_CH, ucDot11Mode, pSession->selfMacAddr,
                                             pRequestBuf->p24GProbeTemplate, &pRequestBuf->us24GProbeTemplateLen);

   csrRoamScanOffloadPrepareProbeReqTemplate(pMac,SIR_ROAM_SCAN_5G_DEFAULT_CH, ucDot11Mode, pSession->selfMacAddr,
                                             pRequestBuf->p5GProbeTemplate, &pRequestBuf->us5GProbeTemplateLen);
send_roam_scan_offload_cmd:
   msg.type     = WDA_ROAM_SCAN_OFFLOAD_REQ;
   msg.reserved = 0;
   msg.bodyptr  = pRequestBuf;
   if (!VOS_IS_STATUS_SUCCESS(vos_mq_post_message(VOS_MODULE_ID_WDA, &msg)))
   {
       VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_ERROR, "%s: Not able to post WDA_ROAM_SCAN_OFFLOAD_REQ message to WDA", __func__);
       vos_mem_free(pRequestBuf);
       return eHAL_STATUS_FAILURE;
   }
   else
   {
        if (ROAM_SCAN_OFFLOAD_START == command)
            bRoamScanOffloadStarted = VOS_TRUE;
        else if (ROAM_SCAN_OFFLOAD_STOP == command)
            bRoamScanOffloadStarted = VOS_FALSE;
    }

   VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_DEBUG, "Roam Scan Offload Command %d, Reason %d", command, reason);
   return status;
}

eHalStatus csrRoamOffloadScanRspHdlr(tpAniSirGlobal pMac, tANI_U8 reason)
{
    switch(reason)
    {
        case 0:
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_DEBUG, "Rsp for Roam Scan Offload with failure status");
            break;
        case REASON_OS_REQUESTED_ROAMING_NOW:
            csrNeighborRoamProceedWithHandoffReq(pMac);
            break;
        default:
            VOS_TRACE(VOS_MODULE_ID_SME, VOS_TRACE_LEVEL_INFO, "Rsp for Roam Scan Offload with reason %d", reason);
    }
    return eHAL_STATUS_SUCCESS;
}
#endif

tCsrPeStatsReqInfo * csrRoamCheckPeStatsReqList(tpAniSirGlobal pMac, tANI_U32  statsMask, 
                                                tANI_U32 periodicity, tANI_BOOLEAN *pFound, tANI_U8 staId)
{
   tANI_BOOLEAN found = FALSE;
   eHalStatus status = eHAL_STATUS_SUCCESS;
   tCsrPeStatsReqInfo staEntry;
   tCsrPeStatsReqInfo *pTempStaEntry = NULL;
   tListElem *pStaEntry = NULL;
   VOS_STATUS vosStatus;
   tPmcPowerState powerState;
   *pFound = FALSE;
      
   pStaEntry = csrRoamFindInPeStatsReqList(pMac, statsMask);
   if(pStaEntry)
   {
      pTempStaEntry = GET_BASE_ADDR( pStaEntry, tCsrPeStatsReqInfo, link );
      if(pTempStaEntry->periodicity)
      {
         pTempStaEntry->periodicity = 
            CSR_ROAM_MIN(periodicity, pTempStaEntry->periodicity);
      }
      else
      {
         pTempStaEntry->periodicity = periodicity;
      }
      pTempStaEntry->numClient++;
         found = TRUE;
   }
   else
   {
      palZeroMemory(pMac->hHdd, &staEntry, sizeof(tCsrPeStatsReqInfo));
      staEntry.numClient = 1;
      staEntry.periodicity = periodicity;
      staEntry.pMac = pMac;
      staEntry.rspPending = FALSE;
      staEntry.staId = staId;
      staEntry.statsMask = statsMask;
      staEntry.timerRunning = FALSE;
      pTempStaEntry = csrRoamInsertEntryIntoPeStatsReqList(pMac, &pMac->roam.peStatsReqList, &staEntry); 
      if(!pTempStaEntry)
      {
         //   
         smsLog(pMac, LOGW, "csrRoamCheckPeStatsReqList: Failed to insert req in peStatsReqList");
         return NULL;
      }
   }
   pmcQueryPowerState(pMac, &powerState, NULL, NULL);
   if(ePMC_FULL_POWER == powerState)
   {
      if(pTempStaEntry->periodicity < pMac->roam.configParam.statsReqPeriodicity)
      {
         pTempStaEntry->periodicity = pMac->roam.configParam.statsReqPeriodicity;
      }
   }
   else
   {
      if(pTempStaEntry->periodicity < pMac->roam.configParam.statsReqPeriodicityInPS)
      {
         pTempStaEntry->periodicity = pMac->roam.configParam.statsReqPeriodicityInPS;
      }
   }
   if(!pTempStaEntry->timerRunning)
   {
      //                                                                                   
      if(!pTempStaEntry->rspPending && 
         !pTempStaEntry->periodicity)
      {
         status = csrSendMBStatsReqMsg(pMac, statsMask & ~(1 << eCsrGlobalClassDStats), staId);
         if(!HAL_STATUS_SUCCESS(status))
         {
            smsLog(pMac, LOGE, FL("csrRoamCheckPeStatsReqList:failed to send down stats req to PE"));
         }
         else
         {
            pTempStaEntry->rspPending = TRUE;
         }
      }
      if(pTempStaEntry->periodicity)
      {
         if(!found)
         {
            
            vosStatus = vos_timer_init( &pTempStaEntry->hPeStatsTimer, VOS_TIMER_TYPE_SW, 
                                        csrRoamPeStatsTimerHandler, pTempStaEntry );
            if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) )
            {
               smsLog(pMac, LOGE, FL("csrRoamCheckPeStatsReqList:cannot init hPeStatsTimer timer"));
               return NULL;
            }
         }
         //           
         smsLog(pMac, LOG1, "csrRoamCheckPeStatsReqList:peStatsTimer period %d", pTempStaEntry->periodicity);
         vosStatus = vos_timer_start( &pTempStaEntry->hPeStatsTimer, pTempStaEntry->periodicity );
         if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) ) 
         {
            smsLog(pMac, LOGE, FL("csrRoamCheckPeStatsReqList:cannot start hPeStatsTimer timer"));
            return NULL;
         }
         pTempStaEntry->timerRunning = TRUE;
      }
   }
   *pFound = found;
   return pTempStaEntry;
}

/*
                                                                    
*/
static void csrRoamRemoveStatListEntry(tpAniSirGlobal pMac, tListElem *pEntry)
{
    if(pEntry)
    {
        if(csrLLRemoveEntry(&pMac->roam.statsClientReqList, pEntry, LL_ACCESS_LOCK))
        {
            palFreeMemory(pMac->hHdd, GET_BASE_ADDR( pEntry, tCsrStatsClientReqInfo, link ));
            }
        }
    }

void csrRoamRemoveEntryFromPeStatsReqList(tpAniSirGlobal pMac, tCsrPeStatsReqInfo *pPeStaEntry)
{
   tListElem *pEntry;
   tCsrPeStatsReqInfo *pTempStaEntry;
   VOS_STATUS vosStatus;
   pEntry = csrLLPeekHead( &pMac->roam.peStatsReqList, LL_ACCESS_LOCK );
   if(!pEntry)
   {
      //          
      smsLog(pMac, LOGW, "csrRoamRemoveEntryFromPeStatsReqList: List empty, no stats req for PE");
      return;
   }
   while( pEntry )
   {
      pTempStaEntry = GET_BASE_ADDR( pEntry, tCsrPeStatsReqInfo, link );
      if( pTempStaEntry && pTempStaEntry->statsMask == pPeStaEntry->statsMask)
      {
         smsLog(pMac, LOGW, "csrRoamRemoveEntryFromPeStatsReqList: match found");
         if(pTempStaEntry->timerRunning)
         {
            vosStatus = vos_timer_stop( &pTempStaEntry->hPeStatsTimer );
            /*                                                        
                                                                        
                                                  
             */
            if ( vosStatus == VOS_STATUS_SUCCESS )
            {
               /*                                   */
               pTempStaEntry->timerRunning = FALSE;

               /*                   */
               vosStatus = vos_timer_destroy( &pTempStaEntry->hPeStatsTimer );
               if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) )
               {
                  smsLog(pMac, LOGE, FL("csrRoamRemoveEntryFromPeStatsReqList:failed to destroy hPeStatsTimer timer"));
               }
            }
            else
            {
               //                                                            
               //                                              
               pTempStaEntry->timerStopFailed = TRUE;
            }
         } 

         if(csrLLRemoveEntry(&pMac->roam.peStatsReqList, pEntry, LL_ACCESS_LOCK))
         {
            //                                                             
            if(!pTempStaEntry->timerStopFailed)
            {
               palFreeMemory(pMac->hHdd, pTempStaEntry);
               pTempStaEntry = NULL;
            }
            break;
         }

         pEntry = csrLLNext( &pMac->roam.peStatsReqList, pEntry, LL_ACCESS_NOLOCK );
      }
   }
   return;
}


void csrRoamSaveStatsFromTl(tpAniSirGlobal pMac, WLANTL_TRANSFER_STA_TYPE *pTlStats)
{

   pMac->roam.classDStatsInfo.num_rx_bytes_crc_ok = pTlStats->rxBcntCRCok;
   pMac->roam.classDStatsInfo.rx_bc_byte_cnt = pTlStats->rxBCBcnt;
   pMac->roam.classDStatsInfo.rx_bc_frm_cnt = pTlStats->rxBCFcnt;
   pMac->roam.classDStatsInfo.rx_byte_cnt = pTlStats->rxBcnt;
   pMac->roam.classDStatsInfo.rx_mc_byte_cnt = pTlStats->rxMCBcnt;
   pMac->roam.classDStatsInfo.rx_mc_frm_cnt = pTlStats->rxMCFcnt;
   pMac->roam.classDStatsInfo.rx_rate = pTlStats->rxRate;
   //              
   pMac->roam.classDStatsInfo.rx_uc_byte_cnt[0] = pTlStats->rxUCBcnt;
   pMac->roam.classDStatsInfo.rx_uc_frm_cnt = pTlStats->rxUCFcnt;
   pMac->roam.classDStatsInfo.tx_bc_byte_cnt = pTlStats->txBCBcnt;
   pMac->roam.classDStatsInfo.tx_bc_frm_cnt = pTlStats->txBCFcnt;
   pMac->roam.classDStatsInfo.tx_mc_byte_cnt = pTlStats->txMCBcnt;
   pMac->roam.classDStatsInfo.tx_mc_frm_cnt = pTlStats->txMCFcnt;
   //              
   pMac->roam.classDStatsInfo.tx_uc_byte_cnt[0] = pTlStats->txUCBcnt;
   pMac->roam.classDStatsInfo.tx_uc_frm_cnt = pTlStats->txUCFcnt;

}

void csrRoamReportStatistics(tpAniSirGlobal pMac, tANI_U32 statsMask, 
                             tCsrStatsCallback callback, tANI_U8 staId, void *pContext)
{
   tANI_U8 stats[500];
   tANI_U8 *pStats = NULL;
   tANI_U32 tempMask = 0;
   tANI_U8 counter = 0;
   eHalStatus status = eHAL_STATUS_FAILURE;
   if(!callback)
   {
      smsLog(pMac, LOGE, FL("csrRoamReportStatistics:cannot report callback NULL"));
      return;
   }
   if(!statsMask)
   {
      smsLog(pMac, LOGE, FL("csrRoamReportStatistics:cannot report statsMask is 0"));
      return;
   }
   pStats = stats;
   tempMask = statsMask;
   while(tempMask)
   {
      if(tempMask & 1)
      {
         //                                                            
         switch(counter)
         {
         case eCsrSummaryStats:
            smsLog( pMac, LOG2, FL("csrRoamReportStatistics:summary stats"));
            status = palCopyMemory(pMac->hHdd, pStats, (tANI_U8 *)&pMac->roam.summaryStatsInfo, 
                                   sizeof(tCsrSummaryStatsInfo));
            if(!HAL_STATUS_SUCCESS(status))
            {
               smsLog( pMac, LOG1, FL("csrRoamReportStatistics:failed to copy summary stats"));
            }
            pStats += sizeof(tCsrSummaryStatsInfo);
            break;
         case eCsrGlobalClassAStats:
            smsLog( pMac, LOG2, FL("csrRoamReportStatistics:ClassA stats"));
            status = palCopyMemory(pMac->hHdd, pStats, (tANI_U8 *)&pMac->roam.classAStatsInfo, 
                                   sizeof(tCsrGlobalClassAStatsInfo));
            if(!HAL_STATUS_SUCCESS(status))
            {
               smsLog( pMac, LOG1, FL("csrRoamReportStatistics:failed to copy ClassA stats"));
            }
            pStats += sizeof(tCsrGlobalClassAStatsInfo);
            break;
         case eCsrGlobalClassBStats:
            smsLog( pMac, LOG2, FL("csrRoamReportStatistics:ClassB stats"));
            status = palCopyMemory(pMac->hHdd, pStats, (tANI_U8 *)&pMac->roam.classBStatsInfo, 
                                   sizeof(tCsrGlobalClassBStatsInfo));
            if(!HAL_STATUS_SUCCESS(status))
            {
               smsLog( pMac, LOG1, FL("csrRoamReportStatistics:failed to copy ClassB stats"));
            }
            pStats += sizeof(tCsrGlobalClassBStatsInfo);
            break;
         case eCsrGlobalClassCStats:
            smsLog( pMac, LOG2, FL("csrRoamReportStatistics:ClassC stats"));
            status = palCopyMemory(pMac->hHdd, pStats, (tANI_U8 *)&pMac->roam.classCStatsInfo, 
                                   sizeof(tCsrGlobalClassCStatsInfo));
            if(!HAL_STATUS_SUCCESS(status))
            {
               smsLog( pMac, LOG1, FL("csrRoamReportStatistics:failed to copy ClassC stats"));
            }
            pStats += sizeof(tCsrGlobalClassCStatsInfo);
            break;
         case eCsrGlobalClassDStats:
            smsLog( pMac, LOG2, FL("csrRoamReportStatistics:ClassD stats"));
            status = palCopyMemory(pMac->hHdd, pStats, (tANI_U8 *)&pMac->roam.classDStatsInfo, 
                                   sizeof(tCsrGlobalClassDStatsInfo));
            if(!HAL_STATUS_SUCCESS(status))
            {
               smsLog( pMac, LOG1, FL("csrRoamReportStatistics:failed to copy ClassD stats"));
            }
            pStats += sizeof(tCsrGlobalClassDStatsInfo);
            break;
         case eCsrPerStaStats:
            smsLog( pMac, LOG2, FL("csrRoamReportStatistics:PerSta stats"));
            status = palCopyMemory(pMac->hHdd, pStats, (tANI_U8 *)&pMac->roam.perStaStatsInfo[staId], 
                                   sizeof(tCsrPerStaStatsInfo));
            if(!HAL_STATUS_SUCCESS(status))
            {
               smsLog( pMac, LOG1, FL("csrRoamReportStatistics:failed to copy PerSta stats"));
            }
            pStats += sizeof(tCsrPerStaStatsInfo);
            break;
         default:
            smsLog( pMac, LOG1, FL("csrRoamReportStatistics:unknown stats type"));
            break;
         }
      }
      tempMask >>=1;
      counter++;
   }
   callback(stats, pContext );
}

eHalStatus csrRoamDeregStatisticsReq(tpAniSirGlobal pMac)
{
   tListElem *pEntry = NULL;
   tListElem *pPrevEntry = NULL;
   tCsrStatsClientReqInfo *pTempStaEntry = NULL;
   eHalStatus status = eHAL_STATUS_SUCCESS;
   VOS_STATUS vosStatus;
   pEntry = csrLLPeekHead( &pMac->roam.statsClientReqList, LL_ACCESS_LOCK );
   if(!pEntry)
   {
      //          
      smsLog(pMac, LOGW, "csrRoamDeregStatisticsReq: List empty, no request from "
             "upper layer client(s)");
      return status;
   }
   while( pEntry )
   {
      if(pPrevEntry)
      {
         pTempStaEntry = GET_BASE_ADDR( pPrevEntry, tCsrStatsClientReqInfo, link );
         //                        
         csrRoamReportStatistics(pMac, pTempStaEntry->statsMask, pTempStaEntry->callback, 
                                 pTempStaEntry->staId, pTempStaEntry->pContext);
         csrRoamRemoveStatListEntry(pMac, pPrevEntry);
      }
      pTempStaEntry = GET_BASE_ADDR( pEntry, tCsrStatsClientReqInfo, link );
      if (pTempStaEntry->pPeStaEntry)  //                       
      {
         pTempStaEntry->pPeStaEntry->numClient--;
         //                                                            
         if(!pTempStaEntry->pPeStaEntry->numClient)
         {
            csrRoamRemoveEntryFromPeStatsReqList(pMac, pTempStaEntry->pPeStaEntry);
         }
      }
      //                                                
      pMac->roam.tlStatsReqInfo.numClient--;
      if(!pMac->roam.tlStatsReqInfo.numClient)
      {
         if(pMac->roam.tlStatsReqInfo.timerRunning)
         {
            status = vos_timer_stop(&pMac->roam.tlStatsReqInfo.hTlStatsTimer);
            if (!HAL_STATUS_SUCCESS(status))
            {
               smsLog(pMac, LOGE, FL("csrRoamDeregStatisticsReq:cannot stop TlStatsTimer timer"));
               //                
            }
         }
         pMac->roam.tlStatsReqInfo.periodicity = 0;
         pMac->roam.tlStatsReqInfo.timerRunning = FALSE;
      }
      if (pTempStaEntry->periodicity)
      {
          //                                            
          //                                                              
          //                                                       
          
          vos_timer_stop( &pTempStaEntry->timer );
          //                               
          vosStatus = vos_timer_destroy( &pTempStaEntry->timer );
          if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) )
          {
              smsLog(pMac, LOGE, FL("csrRoamDeregStatisticsReq:failed to destroy Client req timer"));
          }
      }
      
      
      pPrevEntry = pEntry;
      pEntry = csrLLNext( &pMac->roam.statsClientReqList, pEntry, LL_ACCESS_NOLOCK );
   }
   //            
   if(pPrevEntry)
   {
      pTempStaEntry = GET_BASE_ADDR( pPrevEntry, tCsrStatsClientReqInfo, link );
      //                        
      csrRoamReportStatistics(pMac, pTempStaEntry->statsMask, pTempStaEntry->callback, 
                                 pTempStaEntry->staId, pTempStaEntry->pContext);
      csrRoamRemoveStatListEntry(pMac, pPrevEntry);
   }
   return status;
   
}

eHalStatus csrIsFullPowerNeeded( tpAniSirGlobal pMac, tSmeCmd *pCommand, 
                                   tRequestFullPowerReason *pReason,
                                   tANI_BOOLEAN *pfNeedPower )
{
    tANI_BOOLEAN fNeedFullPower = eANI_BOOLEAN_FALSE;
    tRequestFullPowerReason reason = eSME_REASON_OTHER;
    tPmcState pmcState;
    eHalStatus status = eHAL_STATUS_SUCCESS;
        //                                
        tANI_U32 sessionId = 0;
    if( pfNeedPower )
    {
        *pfNeedPower = eANI_BOOLEAN_FALSE;
    }
        //                           
        if( !(eSmeCsrCommandMask & pCommand->command) )
        {
                return eHAL_STATUS_SUCCESS;
        }
    //                     
    pmcState = pmcGetPmcState( pMac );
    switch( pmcState )
    {
    case REQUEST_IMPS:
    case IMPS:
        if( eSmeCommandScan == pCommand->command )
        {
            switch( pCommand->u.scanCmd.reason )
            {
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
            case eCsrScanGetLfrResult:
#endif
            case eCsrScanGetResult:
            case eCsrScanBGScanAbort:
            case eCsrScanBGScanEnable:
            case eCsrScanGetScanChnInfo:
                //                                        
                fNeedFullPower = eANI_BOOLEAN_FALSE;
                break;
            default:
                //                                        
                fNeedFullPower = eANI_BOOLEAN_TRUE;
                break;
            } //      
        }
        else
        {
            //                                        
            fNeedFullPower = eANI_BOOLEAN_TRUE;
        }
        break;
    case REQUEST_BMPS:
    case BMPS:
    case REQUEST_START_UAPSD:
    case UAPSD:
    //                          
    case REQUEST_ENTER_WOWL:
    case WOWL:
        if( eSmeCommandRoam == pCommand->command )
        {
            tScanResultList *pBSSList = (tScanResultList *)pCommand->u.roamCmd.hBSSList;
            tCsrScanResult *pScanResult;
            tListElem *pEntry;
            switch ( pCommand->u.roamCmd.roamReason )
            {
            case eCsrForcedDisassoc:
            case eCsrForcedDisassocMICFailure:
                reason = eSME_LINK_DISCONNECTED_BY_HDD;
                fNeedFullPower = eANI_BOOLEAN_TRUE;
                break;
                case eCsrSmeIssuedDisassocForHandoff:
            case eCsrForcedDeauth:
            case eCsrHddIssuedReassocToSameAP:
            case eCsrSmeIssuedReassocToSameAP:
                fNeedFullPower = eANI_BOOLEAN_TRUE;
                break;
            case eCsrCapsChange:
                fNeedFullPower = eANI_BOOLEAN_TRUE;
                break;
            default:
                //                                                                             
                //                                                                        
                if ( csrIsConnStateConnectedInfra(pMac, sessionId) && pBSSList )
                {
                    //                                
                    pEntry = csrLLPeekHead(&pBSSList->List, LL_ACCESS_LOCK);
                    if( pEntry )
                    {
                        pScanResult = GET_BASE_ADDR(pEntry, tCsrScanResult, Link);
#if 0
                                                //                                             
                        if( csrIsBssIdEqual( pMac, &pScanResult->Result.BssDescriptor, pMac->roam.pConnectBssDesc ) &&
                            csrIsSsidEqual( pMac, pMac->roam.pConnectBssDesc, 
                                            &pScanResult->Result.BssDescriptor, (tDot11fBeaconIEs *)( pScanResult->Result.pvIes ) ) )
                        {
                            //                                                                                               
                            //                                                                                            
                            //                                                                                                
                            //                   
                            if(csrIsSameProfile(pMac, &pMac->roam.connectedProfile, pProfile))
                            {
                                if(csrRoamIsSameProfileKeys(pMac, &pMac->roam.connectedProfile, pProfile))
                                {
                                    //                                                                   
                                    //                      
                                    //                                                        
                                    //            
                                    pCommand->u.roamCmd.fReassocToSelfNoCapChange = eANI_BOOLEAN_TRUE;
                                    break;
                                }
                            }
                        }
#endif
                    }
                }
                //                                    
                fNeedFullPower = eANI_BOOLEAN_TRUE;
                break;
            }
        }
        else if( eSmeCommandWmStatusChange == pCommand->command )
        {
            //                       
            fNeedFullPower = eANI_BOOLEAN_TRUE;
            reason = eSME_LINK_DISCONNECTED_BY_OTHER;
        }
#ifdef FEATURE_WLAN_TDLS
        else if( eSmeCommandTdlsAddPeer == pCommand->command )
        {
            //                                                  
            fNeedFullPower = eANI_BOOLEAN_TRUE;
            reason = eSME_FULL_PWR_NEEDED_BY_TDLS_PEER_SETUP;
        }
#endif
        break;
    case REQUEST_STOP_UAPSD:
    case REQUEST_EXIT_WOWL:
        if( eSmeCommandRoam == pCommand->command )
        {
            fNeedFullPower = eANI_BOOLEAN_TRUE;
            switch ( pCommand->u.roamCmd.roamReason )
            {
                case eCsrForcedDisassoc:
                case eCsrForcedDisassocMICFailure:
                    reason = eSME_LINK_DISCONNECTED_BY_HDD;
                    break;
                default:
                    break;
            }
                }
        break;
    case STOPPED:
    case REQUEST_STANDBY:
    case STANDBY:
    case LOW_POWER:
        //                                  
        smsLog( pMac, LOGE, FL( "  cannot process because PMC is in stopped/standby state %d" ), pmcState );
        status = eHAL_STATUS_FAILURE;
        break;
    case FULL_POWER:
    case REQUEST_FULL_POWER:
    default:
        //                                                              
        break;
    } //      
    if( pReason )
    {
        *pReason = reason;
    }
    if( pfNeedPower )
    {
        *pfNeedPower = fNeedFullPower;
    }
    return ( status );
}

static eHalStatus csrRequestFullPower( tpAniSirGlobal pMac, tSmeCmd *pCommand )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tANI_BOOLEAN fNeedFullPower = eANI_BOOLEAN_FALSE;
    tRequestFullPowerReason reason = eSME_REASON_OTHER;
    status = csrIsFullPowerNeeded( pMac, pCommand, &reason, &fNeedFullPower );
    if( fNeedFullPower && HAL_STATUS_SUCCESS( status ) )
    {
        status = pmcRequestFullPower(pMac, csrFullPowerCallback, pMac, reason);
    }
    return ( status );
}

tSmeCmd *csrGetCommandBuffer( tpAniSirGlobal pMac )
{
    tSmeCmd *pCmd = smeGetCommandBuffer( pMac );
    if( pCmd )
    {
        pMac->roam.sPendingCommands++;
    }
    return ( pCmd );
}

void csrReleaseCommand(tpAniSirGlobal pMac, tSmeCmd *pCommand)
{
   if (pMac->roam.sPendingCommands > 0)
   {
       //                                                         
       //                                           
       pMac->roam.sPendingCommands--;
       smeReleaseCommand( pMac, pCommand );
   }
   else
   {
       smsLog(pMac, LOGE, FL( "no pending commands"));
       VOS_ASSERT(0);
   }
}

//                                               
eHalStatus csrQueueSmeCommand( tpAniSirGlobal pMac, tSmeCmd *pCommand, tANI_BOOLEAN fHighPriority )
{
    eHalStatus status;
    if( (eSmeCommandScan == pCommand->command) && pMac->scan.fDropScanCmd )
    {
        smsLog(pMac, LOGW, FL(" drop scan (scan reason %d) command"),
           pCommand->u.scanCmd.reason);
        return eHAL_STATUS_CSR_WRONG_STATE;
    }

    if ((pMac->fScanOffload) && (pCommand->command == eSmeCommandScan))
    {
        csrLLInsertTail(&pMac->sme.smeScanCmdPendingList,
                        &pCommand->Link, LL_ACCESS_LOCK);
        //                             
        smeProcessPendingQueue(pMac);
        status = eHAL_STATUS_SUCCESS;
        goto end;
    }

    //                                                                              
    //                                              
    status = csrRequestFullPower( pMac, pCommand );
    if( HAL_STATUS_SUCCESS( status ) )
    {
        tANI_BOOLEAN fNoCmdPending;
        //                                               
        fNoCmdPending = csrLLIsListEmpty( &pMac->roam.roamCmdPendingList, eANI_BOOLEAN_FALSE );
        if( fNoCmdPending )
        {
            smePushCommand( pMac, pCommand, fHighPriority );
        }
        else
        {
            //                                                                                   
            //                                             
            if( !fHighPriority )
            {
                csrLLInsertTail( &pMac->roam.roamCmdPendingList, &pCommand->Link, eANI_BOOLEAN_FALSE );
            }
            else {
                csrLLInsertHead( &pMac->roam.roamCmdPendingList, &pCommand->Link, eANI_BOOLEAN_FALSE );
            }
        }
    }
    else if( eHAL_STATUS_PMC_PENDING == status )
    {
        //                                             
        if( !fHighPriority )
        {
            csrLLInsertTail( &pMac->roam.roamCmdPendingList, &pCommand->Link, eANI_BOOLEAN_FALSE );
        }
        else {
            csrLLInsertHead( &pMac->roam.roamCmdPendingList, &pCommand->Link, eANI_BOOLEAN_FALSE );
        }
        //                                    
        status = eHAL_STATUS_SUCCESS;
    }
    else
    {
        //                                                                                 
        //                    
        smsLog( pMac, LOGE, FL( "  cannot queue command %d" ), pCommand->command );
    }
end:
    return ( status );
}
eHalStatus csrRoamUpdateAPWPSIE( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirAPWPSIEs* pAPWPSIES )
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirUpdateAPWPSIEsReq *pMsg;
    tANI_U8 *pBuf = NULL, *wTmpBuf = NULL;
    
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    if (NULL == pSession)
    {
        smsLog( pMac, LOGE, FL( "  Session does not exist for session id %d" ), sessionId);
        return eHAL_STATUS_FAILURE;
    }

    do
    {
        status = palAllocateMemory( pMac->hHdd, (void **)&pMsg, sizeof(tSirUpdateAPWPSIEsReq) );
        if (!HAL_STATUS_SUCCESS(status)) break;
        palZeroMemory( pMac->hHdd, pMsg, sizeof(tSirUpdateAPWPSIEsReq) );
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_UPDATE_APWPSIE_REQ);

        pBuf = (tANI_U8 *)&pMsg->transactionId;
        VOS_ASSERT(pBuf);

        wTmpBuf = pBuf;
        //              
        *pBuf = 0;
        *( pBuf + 1 ) = 0;
        pBuf += sizeof(tANI_U16);
        //      
        palCopyMemory( pMac->hHdd, (tSirMacAddr *)pBuf, &pSession->selfMacAddr, sizeof(tSirMacAddr) );
        pBuf += sizeof(tSirMacAddr);
        //         
        *pBuf++ = (tANI_U8)sessionId;
        //         
        palCopyMemory( pMac->hHdd, (tSirAPWPSIEs *)pBuf, pAPWPSIES, sizeof(tSirAPWPSIEs));
        pBuf += sizeof(tSirAPWPSIEs);
        pMsg->length = pal_cpu_to_be16((tANI_U16)(sizeof(tANI_U32) + (pBuf - wTmpBuf))); //                
        status = palSendMBMessage(pMac->hHdd, pMsg);
    } while( 0 );
    return ( status );
}
eHalStatus csrRoamUpdateWPARSNIEs( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirRSNie * pAPSirRSNie)
{
    eHalStatus status = eHAL_STATUS_SUCCESS;
    tSirUpdateAPWPARSNIEsReq *pMsg;
    tANI_U8 *pBuf = NULL, *wTmpBuf = NULL;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    if (NULL == pSession)
    {
        smsLog( pMac, LOGE, FL( "  Session does not exist for session id %d" ), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    do
    {
        status = palAllocateMemory( pMac->hHdd, (void **)&pMsg, sizeof(tSirUpdateAPWPARSNIEsReq) );
        if (!HAL_STATUS_SUCCESS(status)) break;
        palZeroMemory( pMac->hHdd, pMsg, sizeof( tSirUpdateAPWPARSNIEsReq ) );
        pMsg->messageType = pal_cpu_to_be16((tANI_U16)eWNI_SME_SET_APWPARSNIEs_REQ);
        pBuf = (tANI_U8 *)&pMsg->transactionId;
        wTmpBuf = pBuf;
        //              
        *pBuf = 0;
        *( pBuf + 1 ) = 0;
        pBuf += sizeof(tANI_U16);
        VOS_ASSERT(pBuf);

        //      
        palCopyMemory( pMac->hHdd, (tSirMacAddr *)pBuf, &pSession->selfMacAddr, sizeof(tSirMacAddr) );
        pBuf += sizeof(tSirMacAddr);
        //          
        *pBuf++ = (tANI_U8)sessionId;
    
        //            
        palCopyMemory( pMac->hHdd, (tSirRSNie *)pBuf, pAPSirRSNie, sizeof(tSirRSNie));
        pBuf += sizeof(tSirRSNie);
        pMsg->length = pal_cpu_to_be16((tANI_U16)(sizeof(tANI_U32 ) + (pBuf - wTmpBuf))); //                
    status = palSendMBMessage(pMac->hHdd, pMsg);
    } while( 0 );
    return ( status );
}

#ifdef WLAN_FEATURE_VOWIFI_11R
//                                                                                                                   
eHalStatus csrRoamIssueFTPreauthReq(tHalHandle hHal, tANI_U32 sessionId, tpSirBssDescription pBssDescription)
{
    tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
    tpSirFTPreAuthReq pftPreAuthReq;
    tANI_U16 auth_req_len = 0;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    auth_req_len = sizeof(tSirFTPreAuthReq);
    pftPreAuthReq = (tpSirFTPreAuthReq)vos_mem_malloc(auth_req_len);
    if (pftPreAuthReq == NULL)
    {
        smsLog(pMac, LOGE, FL("Memory allocation for FT Preauth request failed"));
        return eHAL_STATUS_RESOURCES;
    }
    //                                                                               
    pMac->ft.ftSmeContext.smeSessionId = sessionId;
    vos_mem_zero(pftPreAuthReq, auth_req_len);

    pftPreAuthReq->pbssDescription = (tpSirBssDescription)vos_mem_malloc(
            sizeof(pBssDescription->length) + pBssDescription->length);

    pftPreAuthReq->messageType = pal_cpu_to_be16(eWNI_SME_FT_PRE_AUTH_REQ);

    pftPreAuthReq->preAuthchannelNum = pBssDescription->channelId;

    palCopyMemory(pMac->hHdd, (void *)&pftPreAuthReq->currbssId, (void *)pSession->connectedProfile.bssid, sizeof(tSirMacAddr));
    palCopyMemory(pMac->hHdd, (void *)&pftPreAuthReq->preAuthbssId, (void *)pBssDescription->bssId, sizeof(tSirMacAddr));  

#ifdef WLAN_FEATURE_VOWIFI_11R
    if (csrRoamIs11rAssoc(pMac) && 
          (pMac->roam.roamSession[sessionId].connectedProfile.AuthType != eCSR_AUTH_TYPE_OPEN_SYSTEM))
    {
        pftPreAuthReq->ft_ies_length = (tANI_U16)pMac->ft.ftSmeContext.auth_ft_ies_length;
        palCopyMemory(pMac->hHdd, pftPreAuthReq->ft_ies, pMac->ft.ftSmeContext.auth_ft_ies, 
                                pMac->ft.ftSmeContext.auth_ft_ies_length);
    }
    else
#endif
    {
        pftPreAuthReq->ft_ies_length = 0; 
    }
    vos_mem_copy(pftPreAuthReq->pbssDescription, pBssDescription,
                 sizeof(pBssDescription->length) + pBssDescription->length);
    pftPreAuthReq->length = pal_cpu_to_be16(auth_req_len); 
    return palSendMBMessage(pMac->hHdd, pftPreAuthReq);
}
/*                                                                          
                                                                      
                  
   
                                                                  
                                                                   
                                                                          */
void csrRoamFTPreAuthRspProcessor( tHalHandle hHal, tpSirFTPreAuthRsp pFTPreAuthRsp )
{
    tpAniSirGlobal pMac = PMAC_STRUCT( hHal );
    eHalStatus  status = eHAL_STATUS_SUCCESS;
#if defined(FEATURE_WLAN_LFR) || defined(FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_CCX_UPLOAD)
    tCsrRoamInfo roamInfo;
#endif

#if defined WLAN_FEATURE_VOWIFI_11R_DEBUG
    smsLog( pMac, LOGE, FL("Preauth response status code 0x%x"), pFTPreAuthRsp->status);
#endif
#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
    status = csrNeighborRoamPreauthRspHandler(pMac, pFTPreAuthRsp->status);
    if (status != eHAL_STATUS_SUCCESS) {
        /*
                                                       
         */
        smsLog(pMac, LOGW, FL("Preauth was not processed: %d"), status);
        return;
    }
#endif
    /*                                                                                      */
    if (VOS_STATUS_SUCCESS != (VOS_STATUS)pFTPreAuthRsp->status)
        return;
    //                  
    pMac->ft.ftSmeContext.FTState = eFT_AUTH_COMPLETE;
    //                                                                                                     
    pMac->ft.ftSmeContext.psavedFTPreAuthRsp = pFTPreAuthRsp;
    /*                                                       */
    if (csrRoamIs11rAssoc(pMac))
    {
        sme_QosCsrEventInd(pMac, pMac->ft.ftSmeContext.smeSessionId, SME_QOS_CSR_PREAUTH_SUCCESS_IND, NULL);
    }
    /*                                                                                      
                                                                     */
    status = vos_timer_start(&pMac->ft.ftSmeContext.preAuthReassocIntvlTimer,
                                                            60);
    if (eHAL_STATUS_SUCCESS != status)
    {
        smsLog(pMac, LOGE, FL("Preauth reassoc interval timer start failed to start with status %d"), status);
        return;
    }
    //                           
    palCopyMemory(pMac->hHdd, (void *)&pMac->ft.ftSmeContext.preAuthbssId, (void *)pFTPreAuthRsp->preAuthbssId, sizeof(tCsrBssid));
    if (csrRoamIs11rAssoc(pMac))
       csrRoamCallCallback(pMac, pFTPreAuthRsp->smeSessionId, NULL, 0, 
                        eCSR_ROAM_FT_RESPONSE, eCSR_ROAM_RESULT_NONE);

#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
    if (csrRoamIsCCXAssoc(pMac))
    {
        /*          */
        csrRoamReadTSF(pMac, (tANI_U8 *)roamInfo.timestamp);

        //                                          
        palCopyMemory(pMac->hHdd, (void *)&roamInfo.bssid, (void *)pFTPreAuthRsp->preAuthbssId, sizeof(tCsrBssid));
        csrRoamCallCallback(pMac, pFTPreAuthRsp->smeSessionId, &roamInfo, 0, eCSR_ROAM_CCKM_PREAUTH_NOTIFY, 0);
    }
#endif /*                                             */
#ifdef FEATURE_WLAN_LFR
    //                                                           
    //                                                  
    if (csrRoamIsFastRoamEnabled(pMac, CSR_SESSION_ID_INVALID))
    {
        //                                           
        palCopyMemory(pMac->hHdd, (void *)&roamInfo.bssid, (void *)pFTPreAuthRsp->preAuthbssId, sizeof(tCsrBssid));
        csrRoamCallCallback(pMac, pFTPreAuthRsp->smeSessionId, &roamInfo, 0, eCSR_ROAM_PMK_NOTIFY, 0);
    }

#endif

    //                       
    pMac->ft.ftSmeContext.psavedFTPreAuthRsp = NULL;
}
#endif
#ifdef FEATURE_WLAN_BTAMP_UT_RF
void csrRoamJoinRetryTimerHandler(void *pv)
{
    tCsrTimerInfo *pInfo = (tCsrTimerInfo *)pv;
    tpAniSirGlobal pMac = pInfo->pMac;
    tANI_U32 sessionId = pInfo->sessionId;
    tCsrRoamSession *pSession;
    
    if( CSR_IS_SESSION_VALID(pMac, sessionId) )
    {
        smsLog( pMac, LOGE, FL( "  retrying the last roam profile on session %d" ), sessionId );
        pSession = CSR_GET_SESSION( pMac, sessionId );
        if(pSession->pCurRoamProfile && csrIsConnStateDisconnected(pMac, sessionId))
        {
            if( !HAL_STATUS_SUCCESS(csrRoamJoinLastProfile(pMac, sessionId)) )
            {
               smsLog( pMac, LOGE, FL( "  fail to retry the last roam profile" ) );
            }
        }
    }
}
eHalStatus csrRoamStartJoinRetryTimer(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 interval)
{
    eHalStatus status = eHAL_STATUS_FAILURE;
    tCsrRoamSession *pSession = CSR_GET_SESSION( pMac, sessionId );
    
    if(pSession->pCurRoamProfile && pSession->maxRetryCount)
    {
        smsLog(pMac, LOGE, FL(" call sessionId %d retry count %d left"), sessionId, pSession->maxRetryCount);
        pSession->maxRetryCount--;
        pSession->joinRetryTimerInfo.pMac = pMac;
        pSession->joinRetryTimerInfo.sessionId = (tANI_U8)sessionId;
        status = vos_timer_start(&pSession->hTimerJoinRetry, interval/PAL_TIMER_TO_MS_UNIT);
        if (!HAL_STATUS_SUCCESS(status))
        {
            smsLog(pMac, LOGE, FL(" fail to start timer status %s"), status);
        }
    }
    else
    {
        smsLog(pMac, LOGE, FL(" not to start timer due to no profile or reach mac ret (%d)"),
               pSession->maxRetryCount);
    }
    
    return (status);
}
eHalStatus csrRoamStopJoinRetryTimer(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
    smsLog(pMac, LOGE, " csrRoamStopJoinRetryTimer");
    if( CSR_IS_SESSION_VALID(pMac, sessionId) )
    {
        return (vos_timer_stop(&pMac->roam.roamSession[sessionId].hTimerJoinRetry));
    }
    
    return eHAL_STATUS_SUCCESS;
}
#endif


/*
                                             
                                  
                           
                             
                        
                                        
                                                    
                           
                                                                                      
*/
static void csrSerDesUnpackDiassocRsp(tANI_U8 *pBuf, tSirSmeDisassocRsp *pRsp)
{
   if(pBuf && pRsp)
   {
      pBuf += 4; //                    
      pRsp->sessionId  = *pBuf++;
      pal_get_U16( pBuf, (tANI_U16 *)&pRsp->transactionId );
      pBuf += 2;
      pal_get_U32( pBuf, (tANI_U32 *)&pRsp->statusCode );
      pBuf += 4;
      vos_mem_copy(pRsp->peerMacAddr, pBuf, 6);
   }
}

eHalStatus csrGetDefaultCountryCodeFrmNv(tpAniSirGlobal pMac, tANI_U8 *pCountry)
{
   static uNvTables nvTables;
   eHalStatus status = eHAL_STATUS_SUCCESS;
   VOS_STATUS vosStatus = vos_nv_readDefaultCountryTable( &nvTables );

   /*                                          */
   if ( VOS_IS_STATUS_SUCCESS(vosStatus) )
   {
      palCopyMemory( pMac->hHdd, pCountry,
            nvTables.defaultCountryTable.countryCode,
            WNI_CFG_COUNTRY_CODE_LEN );
      return status;
   }
   else
   {
      palCopyMemory( pMac->hHdd, pCountry,
            "XXX",
            WNI_CFG_COUNTRY_CODE_LEN );
      status = eHAL_STATUS_FAILURE;
      return status;
   }
}

eHalStatus csrGetCurrentCountryCode(tpAniSirGlobal pMac, tANI_U8 *pCountry)
{
   palCopyMemory( pMac->hHdd, pCountry,
         pMac->scan.countryCode11d,
         WNI_CFG_COUNTRY_CODE_LEN );
   return eHAL_STATUS_SUCCESS;
}

eHalStatus csrSetTxPower(tpAniSirGlobal pMac, v_U8_t sessionId, v_U8_t mW)
{
   tSirSetTxPowerReq *pMsg = NULL;
   eHalStatus status = eHAL_STATUS_SUCCESS;
   tCsrRoamSession *pSession = CSR_GET_SESSION(pMac, sessionId);

   if (!pSession)
   {
       smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
       return eHAL_STATUS_FAILURE;
   }

   status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof(tSirSetTxPowerReq));
   if (HAL_STATUS_SUCCESS(status))
   {
       palZeroMemory(pMac->hHdd, (void *)pMsg, sizeof(tSirSetTxPowerReq));
       pMsg->messageType     = eWNI_SME_SET_TX_POWER_REQ;
       pMsg->length          = sizeof(tSirSetTxPowerReq);
       pMsg->mwPower         = mW;
       palCopyMemory( pMac->hHdd,
             (tSirMacAddr *)pMsg->bssId,
             &pSession->selfMacAddr,
             sizeof(tSirMacAddr) );
       status = palSendMBMessage(pMac->hHdd, pMsg);
       if (!HAL_STATUS_SUCCESS(status))
       {
           smsLog(pMac, LOGE, FL(" csr set TX Power Post MSG Fail %d "), status);
           //                                 
       }
   }
   return status;
}

/*                                                       */
tANI_BOOLEAN csrRoamIsStaMode(tpAniSirGlobal pMac, tANI_U32 sessionId)
{
  tCsrRoamSession *pSession = NULL;
  pSession = CSR_GET_SESSION ( pMac, sessionId );
  if(!pSession)
  {
    smsLog(pMac, LOGE, FL(" %s: session %d not found "), __func__, sessionId);
    return eANI_BOOLEAN_FALSE;
  }
  if ( !CSR_IS_SESSION_VALID ( pMac, sessionId ) )
  {
    smsLog(pMac, LOGE, FL(" %s: Inactive session"), __func__);
    return eANI_BOOLEAN_FALSE;
  }
  if ( eCSR_BSS_TYPE_INFRASTRUCTURE != pSession->connectedProfile.BSSType )
  {
     return eANI_BOOLEAN_FALSE;
  }
  /*                                                             
                                                                      
                                                                      
                                                                    
                                                                  
                        */

  if ( NULL != pSession->pCurRoamProfile )
  {
    if ( pSession->pCurRoamProfile->csrPersona == VOS_STA_MODE )
    {
      return eANI_BOOLEAN_TRUE;
    } else {
            smsLog(pMac, LOGE, FL(" %s: pCurRoamProfile is NULL\n"), __func__);
            return eANI_BOOLEAN_FALSE;
           }
    }

  return eANI_BOOLEAN_FALSE;
}

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
eHalStatus csrHandoffRequest(tpAniSirGlobal pMac,
                             tCsrHandoffRequest *pHandoffInfo)
{
   eHalStatus status = eHAL_STATUS_SUCCESS;
   vos_msg_t  msg;

   tAniHandoffReq *pMsg;
   status = palAllocateMemory(pMac->hHdd, (void **)&pMsg, sizeof(tAniHandoffReq));
   if ( !HAL_STATUS_SUCCESS(status) )
   {
      smsLog(pMac, LOGE, " csrHandoffRequest: failed to allocate mem for req ");
      return status;
   }
   pMsg->msgType = pal_cpu_to_be16((tANI_U16)eWNI_SME_HANDOFF_REQ);
   pMsg->msgLen = (tANI_U16)sizeof(tAniHandoffReq);
   pMsg->sessionId = pMac->roam.neighborRoamInfo.csrSessionId;
   pMsg->channel = pHandoffInfo->channel;
   palCopyMemory(pMac->hHdd, pMsg->bssid,
                       pHandoffInfo->bssid,
                       6);
   msg.type = eWNI_SME_HANDOFF_REQ;
   msg.bodyptr = pMsg;
   msg.reserved = 0;
   if(VOS_STATUS_SUCCESS != vos_mq_post_message(VOS_MQ_ID_SME, &msg))
   {
       smsLog(pMac, LOGE, " csrHandoffRequest failed to post msg to self ");
       palFreeMemory(pMac->hHdd, (void *)pMsg);
       status = eHAL_STATUS_FAILURE;
   }
   return status;
}
#endif /*                                */

#if defined(FEATURE_WLAN_CCX) && defined(FEATURE_WLAN_CCX_UPLOAD)
/*                                                                            
                    
                                                                                       
                                                                          
                                        
                                          
                                             
                                             
                                
                                                                             */
VOS_STATUS csrSetCCKMIe(tpAniSirGlobal pMac, const tANI_U8 sessionId,
                            const tANI_U8 *pCckmIe,
                            const tANI_U8 ccKmIeLen)
{
    eHalStatus       status = eHAL_STATUS_SUCCESS;
    tCsrRoamSession *pSession = CSR_GET_SESSION(pMac, sessionId);

    if (!pSession)
    {
        smsLog(pMac, LOGE, FL("  session %d not found "), sessionId);
        return eHAL_STATUS_FAILURE;
    }
    palCopyMemory(pMac->hHdd, pSession->suppCckmIeInfo.cckmIe, pCckmIe, ccKmIeLen);
    pSession->suppCckmIeInfo.cckmIeLen = ccKmIeLen;
    return status;
}

/*                                                                            
                      
                                                                                           
                                                                                    
                                        
                                             
                                
                                                                             */
VOS_STATUS csrRoamReadTSF(tpAniSirGlobal pMac, tANI_U8 *pTimestamp)
{
    eHalStatus              status     = eHAL_STATUS_SUCCESS;
    tCsrNeighborRoamBSSInfo handoffNode;
    tANI_U32                timer_diff = 0;
    tANI_U32                timeStamp[2];
    tpSirBssDescription     pBssDescription = NULL;

    csrNeighborRoamGetHandoffAPInfo(pMac, &handoffNode);
    pBssDescription = handoffNode.pBssDescription;

    //                                   
    timer_diff = vos_timer_get_system_time() - pBssDescription->scanSysTimeMsec;
    //                                
    timer_diff = (tANI_U32)(timer_diff * SYSTEM_TIME_MSEC_TO_USEC);

    timeStamp[0] = pBssDescription->timeStamp[0];
    timeStamp[1] = pBssDescription->timeStamp[1];

    UpdateCCKMTSF(&(timeStamp[0]), &(timeStamp[1]), &timer_diff);

    palCopyMemory(pMac->hHdd, pTimestamp, (void *) &timeStamp[0],
                    sizeof (tANI_U32) * 2);
    return status;
}

#endif /*                                            */


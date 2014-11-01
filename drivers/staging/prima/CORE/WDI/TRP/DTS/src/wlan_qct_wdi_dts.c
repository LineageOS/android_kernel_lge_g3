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
 *     
 *       \file  wlan_qct_wdi_dts.c
 *          
 *       \brief  Data Transport Service API 
 *                               
 * WLAN Device Abstraction layer External API for Dataservice
 * DESCRIPTION
 *  This file contains the external API implemntation exposed by the 
 *   wlan device abstarction layer module.
 *
 *   Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved.
 *   Qualcomm Confidential and Proprietary
 */


#include "wlan_qct_wdi.h"
#include "wlan_qct_dxe.h"
#include "wlan_qct_wdi_ds.h"
#include "wlan_qct_wdi_ds_i.h"
#include "wlan_qct_wdi_dts.h"
#include "wlan_qct_wdi_dp.h"
#include "wlan_qct_wdi_sta.h"

static WDTS_TransportDriverTrype gTransportDriver = {
  WLANDXE_Open, 
  WLANDXE_Start, 
  WLANDXE_ClientRegistration, 
  WLANDXE_TxFrame,
  WLANDXE_CompleteTX,
  WLANDXE_SetPowerState,
  WLANDXE_ChannelDebug,
  WLANDXE_Stop,
  WLANDXE_Close,
  WLANDXE_GetFreeTxDataResNumber
};

static WDTS_SetPowerStateCbInfoType gSetPowerStateCbInfo;

typedef struct 
{
   uint32 phyRate;   //                              
   uint32 tputRate;  //                              
   uint32 tputBpms;  //                                                                          
   uint32 tputBpus;  //                                                   
}WDTS_RateInfo;

#define WDTS_MAX_RATE_NUM               137
#define WDTS_MAX_11B_RATE_NUM           8
#define MB_PER_SEC_TO_BYTES_PER_MSEC    13

WDTS_RateInfo g11bRateInfo[WDTS_MAX_11B_RATE_NUM]  = {
    //         
    {  10,  9,  117, 8}, //       
    {  20,  17, 221, 5}, //       
    {  55,  41, 533, 2}, //       
    { 110,  68, 884, 1}, //       

    //                  
    {  10,  10,  130, 8}, //       
    {  20,  18,  234, 5}, //       
    {  55,  44,  572, 2}, //       
    { 110,  77, 1001, 1}, //       
};

WDTS_RateInfo gRateInfo[WDTS_MAX_RATE_NUM]  = {
    //         
    {  10,  9,  117, 0}, //       
    {  20,  17, 221, 0}, //       
    {  55,  41, 533, 0}, //       
    { 110,  68, 884, 0}, //       

    //                  
    {  10,  10,  130, 0}, //       
    {  20,  18,  234, 0}, //       
    {  55,  44,  572, 0}, //       
    { 110,  77, 1001, 0}, //       

    //    
    {  60,  50,  650, 1}, //       
    {  90,  70,  910, 1}, //       
    { 120, 100, 1300, 1}, //        
    { 180, 150, 1950, 2}, //        
    { 240, 190, 2470, 2}, //        
    { 360, 280, 3640, 4}, //        
    { 480, 350, 4550, 5}, //        
    { 540, 380, 4940, 6}, //        

    //        
    {  65,  54,  702, 1}, //        
    { 130, 108, 1404, 1}, //        
    { 195, 161, 2093, 2}, //        
    { 260, 217, 2821, 3}, //        
    { 390, 326, 4238, 4}, //        
    { 520, 435, 5655, 6}, //        
    { 585, 492, 6396, 6}, //        
    { 650, 548, 7124, 7}, //        

    //            
    {  72,  59,  767, 1}, //        
    { 144, 118, 1534, 2}, //        
    { 217, 180, 2340, 2}, //        
    { 289, 243, 3159, 3}, //        
    { 434, 363, 4719, 5}, //        
    { 578, 486, 6318, 6}, //        
    { 650, 548, 7124, 7}, //        
    { 722, 606, 7878, 8}, //        

    //           
    {  65,  54,  702, 1}, //        
    { 130, 108, 1404, 1}, //        
    { 195, 161, 2093, 2}, //        
    { 260, 217, 2821, 3}, //        
    { 390, 326, 4238, 4}, //        
    { 520, 435, 5655, 6}, //        
    { 585, 492, 6396, 6}, //        
    { 650, 548, 7124, 7}, //        

    //                      
    { 135,   110,  1430,  1}, //        
    { 270,   223,  2899,  3}, //        
    { 405,   337,  4381,  4}, //        
    { 540,   454,  5902,  6}, //        
    { 810,   679,  8827,  9}, //        
    { 1080,  909, 11817, 12}, //        
    { 1215, 1022, 13286, 13}, //        
    { 1350, 1137, 14781, 15}, //        

    //                         
    { 150,   121,  1573,  2}, //        
    { 300,   249,  3237,  3}, //        
    { 450,   378,  4914,  5}, //        
    { 600,   503,  6539,  7}, //        
    { 900,   758,  9854,  10}, //        
    { 1200, 1010, 13130, 13}, //        
    { 1350, 1137, 14781, 15}, //        
    { 1500, 1262, 16406, 16}, //        

    //                         
    { 135,   110,   1430,  1}, //        
    { 270,   223,   2899,  3}, //        
    { 405,   337,   4381,  4}, //        
    { 540,   454,   5902,  6}, //        
    { 810,   679,   8827,  9}, //        
    { 1080,  909,  11817, 12}, //        
    { 1215, 1022,  13286, 13}, //        
    { 1350, 1137,  14781, 15}, //        

    //      
    { 1350,  675,  8775,  9}, //           
    { 1350,  675,  8775,  9}, //           
    {   65,   45,   585,  1}, //        
    {  130,   91,  1183,  1}, //        
    {  195,  136,  1768,  2}, //        
    {  260,  182,  2366,  2}, //        
    {  390,  273,  3549,  4}, //        
    {  520,  364,  4732,  5}, //        
    {  585,  409,  5317,  5}, //        
    {  650,  455,  5915,  6}, //        
    {  780,  546,  7098,  7}, //        
    { 1350,  675,  8775,  9}, //           
    { 1350,  675,  8775,  9}, //           
    { 1350,  675,  8775,  9}, //           
    { 1350,  675,  8775,  9}, //        
    { 1350,  675,  8775,  9}, //        
    { 1350,  675,  8775,  9}, //        
    { 1350,  675,  8775,  9}, //        
    { 1350,  675,  8775,  9}, //        
    { 1350,  675,  8775,  9}, //        
    {  655,  458,  5954,  6}, //        
    {  722,  505,  6565,  7}, //        
    {  866,  606,  7878,  8}, //        
    { 1350,  675,  8775,  9}, //           
    { 1350,  675,  8775,  9}, //           
    { 1350,  675,  8775,  9}, //           
    {  135,   94,  1222,  1}, //        
    {  270,  189,  2457,  2}, //        
    {  405,  283,  3679,  4}, //        
    {  540,  378,  4914,  5}, //        
    {  810,  567,  7371,  7}, //        
    { 1080,  756,  9828, 10}, //        
    { 1215,  850, 11050, 11}, //        
    { 1350,  675,  8775,  9}, //        
    { 1350,  675,  8775,  9}, //        
    { 1620,  810, 10530, 11}, //        
    { 1800,  900, 11700, 12}, //         
    { 1350,  675,  8775,  9}, //            
    { 1350,  675,  8775,  9}, //         
    { 1350,  675,  8775,  9}, //         
    { 1350,  675,  8775,  9}, //         
    { 1350,  675,  8775,  9}, //         
    { 1350,  675,  8775,  9}, //         
    { 1200,  840, 10920, 11}, //         
    { 1350,  675,  8775,  9}, //         
    { 1500,  750,  9750, 10}, //         
    { 1350,  675,  8775,  9}, //         
    { 1800,  900, 11700, 12}, //         
    { 2000, 1000, 13000, 13}, //         
    { 1350,  675,  8775,  9}, //         
    {  292,  204,  2652,  3}, //         
    {  585,  409,  5317,  5}, //         
    {  877,  613,  7969,  8}, //         
    { 1170,  819, 10647, 11}, //         
    { 1755,  877, 11401, 11}, //         
    { 2340, 1170, 15210, 15}, //         
    { 2632, 1316, 17108, 17}, //         
    { 2925, 1462, 19006, 19}, //         
    { 1350,  675,  8775,  9}, //         
    { 3510, 1755, 22815, 23}, //         
    { 3900, 1950, 25350, 25}, //         
    { 1350,  675,  8775,  9}, //            
    { 1350,  675,  8775,  9}, //         
    { 1350,  675,  8775,  9}, //         
    { 1350,  675,  8775,  9}, //         
    { 1350,  675,  8775,  9}, //         
    { 1350,  675,  8775,  9}, //         
    { 1350,  675,  8775,  9}, //         
    { 2925, 1462, 19006, 19}, //         
    { 3250, 1625, 21125, 21}, //         
    { 1350,  675,  8775,  9}, //         
    { 3900, 1950, 25350, 25}, //         
    { 4333, 2166, 28158, 28}  //         
 };

/*          */
typedef struct
{
  wpt_uint32 txBytesPushed;
  wpt_uint32 txPacketsPushed; //                                 
}WDI_DTS_TX_TrafficStatsType;

/*          */
typedef struct
{
  wpt_uint32 rxBytesRcvd;
  wpt_uint32 rxPacketsRcvd;  //                                 
}WDI_DTS_RX_TrafficStatsType;

typedef struct {
   wpt_uint8 running;
   WDI_DTS_RX_TrafficStatsType rxStats[HAL_NUM_STA][WDTS_MAX_RATE_NUM];
   WDI_DTS_TX_TrafficStatsType txStats[HAL_NUM_STA];
   WDI_TrafficStatsType        netTxRxStats[HAL_NUM_STA];
}WDI_DTS_TrafficStatsType;

static WDI_DTS_TrafficStatsType gDsTrafficStats;

#define DTS_RATE_TPUT(x) gRateInfo[x].tputBpus
#define DTS_11BRATE_TPUT_MULTIPLIER(x) g11bRateInfo[x].tputBpus

/*                                                     */
#define DTS_RX_DELAY_FRAMESIZE_THRESHOLD  500

/*                                                               
                                                                                
                                                                               
                                                                                 
                                                                                                               
 */
void WDTS_FillRateInfo(wpt_uint8 macEff, wpt_int16 startRateIndex, wpt_int16 endRateIndex)
{
    int i;

    DTI_TRACE( DTI_TRACE_LEVEL_ERROR, "Change only 11ac rates\n");

    for (i=startRateIndex; i<=endRateIndex; i++)
    {
        //                                            
        gRateInfo[i].tputRate = ((gRateInfo[i].phyRate * macEff)/100);
        //                                                                                        
        gRateInfo[i].tputBpms = gRateInfo[i].tputRate * MB_PER_SEC_TO_BYTES_PER_MSEC;
        //                                                                            
        gRateInfo[i].tputBpus = ((gRateInfo[i].tputBpms + 500) / 1000);
        if (gRateInfo[i].tputBpus == 0)
            gRateInfo[i].tputBpus = 1;

        DTI_TRACE( DTI_TRACE_LEVEL_ERROR, "%4u, %4u, %5u, %2u",
                            gRateInfo[i].phyRate,
                            gRateInfo[i].tputRate,
                            gRateInfo[i].tputBpms,
                            gRateInfo[i].tputBpus );
    }
}

/*                     
                                                             
               
                                         
                                              
                       
 */
void WDTS_GetTrafficStats(WDI_TrafficStatsType** pStats, wpt_uint32 *len)
{
   if(gDsTrafficStats.running)
   {
      uint8 staIdx, rate;
      WDI_TrafficStatsType *pNetTxRxStats = gDsTrafficStats.netTxRxStats;
      wpalMemoryZero(pNetTxRxStats, sizeof(gDsTrafficStats.netTxRxStats));

      for(staIdx = 0; staIdx < HAL_NUM_STA; staIdx++, pNetTxRxStats++)
      {
          pNetTxRxStats->txBytesPushed += gDsTrafficStats.txStats[staIdx].txBytesPushed;
          pNetTxRxStats->txPacketsPushed+= gDsTrafficStats.txStats[staIdx].txPacketsPushed;
          for(rate = 0; rate < WDTS_MAX_11B_RATE_NUM; rate++)
          {
             pNetTxRxStats->rxBytesRcvd +=
               gDsTrafficStats.rxStats[staIdx][rate].rxBytesRcvd;
             pNetTxRxStats->rxPacketsRcvd +=
               gDsTrafficStats.rxStats[staIdx][rate].rxPacketsRcvd;
             pNetTxRxStats->rxTimeTotal +=
               gDsTrafficStats.rxStats[staIdx][rate].rxBytesRcvd*DTS_11BRATE_TPUT_MULTIPLIER(rate);
          }
          for(rate = WDTS_MAX_11B_RATE_NUM; rate < WDTS_MAX_RATE_NUM; rate++)
          {
             pNetTxRxStats->rxBytesRcvd += 
               gDsTrafficStats.rxStats[staIdx][rate].rxBytesRcvd;
             pNetTxRxStats->rxPacketsRcvd += 
               gDsTrafficStats.rxStats[staIdx][rate].rxPacketsRcvd;
             pNetTxRxStats->rxTimeTotal += 
               gDsTrafficStats.rxStats[staIdx][rate].rxBytesRcvd/DTS_RATE_TPUT(rate);
          }

          pNetTxRxStats->rxTimeTotal = pNetTxRxStats->rxTimeTotal/1000;

      }
      *pStats = gDsTrafficStats.netTxRxStats;
      *len = sizeof(gDsTrafficStats.netTxRxStats);
   }
   else
   {
      *pStats = NULL;
      *len = 0;
   }
}

/*                            
                                                                         
                    
                       
 */
void WDTS_DeactivateTrafficStats(void)
{
   gDsTrafficStats.running = eWLAN_PAL_FALSE;
}

/*                          
                                                                       
                    
                       
 */
void WDTS_ActivateTrafficStats(void)
{
   gDsTrafficStats.running = eWLAN_PAL_TRUE;
}

/*                       
                                                          
                    
                       
 */
void WDTS_ClearTrafficStats(void)
{
   wpalMemoryZero(gDsTrafficStats.rxStats, sizeof(gDsTrafficStats.rxStats));
   wpalMemoryZero(gDsTrafficStats.txStats, sizeof(gDsTrafficStats.txStats));
}

/*                                  
                                                                       
                                 
              
                                                            
                                
                                                 
                                                        
  
 */
wpt_status WDTS_TxPacketComplete(void *pContext, wpt_packet *pFrame, wpt_status status)
{
  WDI_DS_ClientDataType *pClientData = (WDI_DS_ClientDataType*)(pContext);
  WDI_DS_TxMetaInfoType     *pTxMetadata;
  void *pvBDHeader, *physBDHeader;
  wpt_uint8 staIndex;

  //                 
  if(NULL == pContext || NULL == pFrame){
    return eWLAN_PAL_STATUS_E_FAILURE;
  }


  //                                 
  pTxMetadata = WDI_DS_ExtractTxMetaData(pFrame);
  pTxMetadata->txCompleteStatus = status;

  //                         
  WDI_GetBDPointers(pFrame, &pvBDHeader,  &physBDHeader);
  switch(pTxMetadata->frmType) 
  {
    case WDI_MAC_DATA_FRAME:
    /*                                                           
                                             
    */
#ifdef FEATURE_WLAN_TDLS
    /*                                                                           
                                                                                                  
                                                                                        
                                         
    */
    if((!pTxMetadata->isEapol) &&
       ((pTxMetadata->txFlags & WDI_USE_BD_RATE2_FOR_MANAGEMENT_FRAME) != WDI_USE_BD_RATE2_FOR_MANAGEMENT_FRAME))
#else
    if(!pTxMetadata->isEapol)
#endif
    {
      /*                                                     */
      WDI_SwapTxBd(pvBDHeader);
      staIndex = (wpt_uint8)WDI_TX_BD_GET_STA_ID(pvBDHeader);
      WDI_DS_MemPoolFree(&(pClientData->dataMemPool), pvBDHeader, physBDHeader);
      WDI_DS_MemPoolDecreaseReserveCount(&(pClientData->dataMemPool), staIndex);
      break;
    }
    //                                                        
    case WDI_MAC_MGMT_FRAME:
      WDI_DS_MemPoolFree(&(pClientData->mgmtMemPool), pvBDHeader, physBDHeader);
      break;
  }
  WDI_SetBDPointers(pFrame, 0, 0);

  //                            
  pClientData->txCompleteCB(pClientData->pCallbackContext, pFrame);  
  return eWLAN_PAL_STATUS_SUCCESS;

}


/*                                                                               
                                               
     
                                                                              
 
                                                                          
                         
                          
                                                   
                                       
                                                             

                     
                                                                                */
v_U64_t
WDTS_GetReplayCounterFromRxBD
(
   v_U8_t *pucRxBDHeader
)
{
  v_U64_t ullcurrentReplayCounter = 0;
  /*                                                                      */
/*                                            
                                 
                  
                  
                  
                  
                  

                  
                   */
  
#ifdef ANI_BIG_BYTE_ENDIAN
    /*                                              */
    ullcurrentReplayCounter = WDI_RX_BD_GET_PMICMD_20TO23(pucRxBDHeader); 
    ullcurrentReplayCounter <<= 16;
    ullcurrentReplayCounter |= (( WDI_RX_BD_GET_PMICMD_24TO25(pucRxBDHeader) & 0xFFFF0000) >> 16);
    return ullcurrentReplayCounter;
#else
    /*                                              */
    ullcurrentReplayCounter = (WDI_RX_BD_GET_PMICMD_24TO25(pucRxBDHeader) & 0x0000FFFF); 
    ullcurrentReplayCounter <<= 32; 
    ullcurrentReplayCounter |= WDI_RX_BD_GET_PMICMD_20TO23(pucRxBDHeader); 
    return ullcurrentReplayCounter;
#endif
}


/*                         
                                                                       
                        
              
                                                            
                                
                                                 
                                                        
  
 */
wpt_status WDTS_RxPacket (void *pContext, wpt_packet *pFrame, WDTS_ChannelType channel)
{
  WDI_DS_ClientDataType *pClientData = 
    (WDI_DS_ClientDataType*)(pContext);
  wpt_boolean       bASF, bFSF, bLSF, bAEF;
  wpt_uint8                   ucMPDUHOffset, ucMPDUHLen, ucTid;
  wpt_uint8                   *pBDHeader;
  wpt_uint16                  usMPDUDOffset, usMPDULen;
  WDI_DS_RxMetaInfoType     *pRxMetadata;
  wpt_uint8                  isFcBd = 0;

  tpSirMacFrameCtl  pMacFrameCtl;
  //                 
  if(NULL == pContext || NULL == pFrame){
    return eWLAN_PAL_STATUS_E_FAILURE;
  }

  /*                                                                        
                                        
                                                                            */
  pBDHeader = (wpt_uint8*)wpalPacketGetRawBuf(pFrame);
  if(NULL == pBDHeader)
  {
    DTI_TRACE( DTI_TRACE_LEVEL_ERROR,
       "WLAN TL:BD header received NULL - dropping packet");
    wpalPacketFree(pFrame);
    return eWLAN_PAL_STATUS_E_FAILURE;
  }
  WDI_SwapRxBd(pBDHeader);

  ucMPDUHOffset = (wpt_uint8)WDI_RX_BD_GET_MPDU_H_OFFSET(pBDHeader);
  usMPDUDOffset = (wpt_uint16)WDI_RX_BD_GET_MPDU_D_OFFSET(pBDHeader);
  usMPDULen     = (wpt_uint16)WDI_RX_BD_GET_MPDU_LEN(pBDHeader);
  ucMPDUHLen    = (wpt_uint8)WDI_RX_BD_GET_MPDU_H_LEN(pBDHeader);
  ucTid         = (wpt_uint8)WDI_RX_BD_GET_TID(pBDHeader);

  /*                                                        
                                                     
                                  
                                                                 
                                                               
                                              
                                               */
  if (usMPDULen < DTS_RX_DELAY_FRAMESIZE_THRESHOLD)
  {
    wpalBusyWait(1);
  }

  /*                                                                        
                             
                                                                            */
  bASF = WDI_RX_BD_GET_ASF(pBDHeader);
  bAEF = WDI_RX_BD_GET_AEF(pBDHeader);
  bFSF = WDI_RX_BD_GET_ESF(pBDHeader);
  bLSF = WDI_RX_BD_GET_LSF(pBDHeader);
  isFcBd = WDI_RX_FC_BD_GET_FC(pBDHeader);

  DTI_TRACE( DTI_TRACE_LEVEL_INFO,
      "WLAN TL:BD header processing data: HO %d DO %d Len %d HLen %d"
      " Tid %d BD %d",
      ucMPDUHOffset, usMPDUDOffset, usMPDULen, ucMPDUHLen, ucTid,
      WDI_RX_BD_HEADER_SIZE);

  if(!isFcBd)
  {
      if(usMPDUDOffset <= ucMPDUHOffset || usMPDULen < ucMPDUHLen) {
        DTI_TRACE( DTI_TRACE_LEVEL_ERROR,
            "WLAN TL:BD header corrupted - dropping packet");
        /*                  */ 
        wpalPacketFree(pFrame);
        return eWLAN_PAL_STATUS_SUCCESS;
      }

      if((ucMPDUHOffset < WDI_RX_BD_HEADER_SIZE) &&  (!(bASF && !bFSF))){
        /*                                                                */
        /*                  */ 
        wpalPacketFree(pFrame);
        return eWLAN_PAL_STATUS_SUCCESS;
      }

      /*                                     
                                                
                                                                  */
      if((ucMPDUHOffset < WDI_RX_BD_HEADER_SIZE) && (bASF && !bFSF)){
        ucMPDUHOffset = usMPDUDOffset;
      }

      if(VPKT_SIZE_BUFFER < (usMPDULen+ucMPDUHOffset)){
        DTI_TRACE( DTI_TRACE_LEVEL_FATAL,
                   "Invalid Frame size, might memory corrupted");

        /*                                                   
                                                            
         */
        WPAL_BUG(0);

        wpalPacketFree(pFrame);
        return eWLAN_PAL_STATUS_SUCCESS;
      }
      if(eWLAN_PAL_STATUS_SUCCESS != wpalPacketSetRxLength(pFrame, usMPDULen+ucMPDUHOffset))
      {
          DTI_TRACE( DTI_TRACE_LEVEL_ERROR, "Invalid Frame Length, Frame dropped..");
          wpalPacketFree(pFrame);
          return eWLAN_PAL_STATUS_SUCCESS;
      }
      if(eWLAN_PAL_STATUS_SUCCESS != wpalPacketRawTrimHead(pFrame, ucMPDUHOffset))
      {
          DTI_TRACE( DTI_TRACE_LEVEL_ERROR, "Failed to trim Raw Packet Head, Frame dropped..");
          wpalPacketFree(pFrame);
          return eWLAN_PAL_STATUS_SUCCESS;
      }
     

      pRxMetadata = WDI_DS_ExtractRxMetaData(pFrame);

      pRxMetadata->fc = isFcBd;
      pRxMetadata->staId = WDI_RX_BD_GET_STA_ID(pBDHeader);
      pRxMetadata->addr3Idx = WDI_RX_BD_GET_ADDR3_IDX(pBDHeader);
      pRxMetadata->rxChannel = WDI_RX_BD_GET_RX_CHANNEL(pBDHeader);
      pRxMetadata->rfBand = WDI_RX_BD_GET_RFBAND(pBDHeader);
      pRxMetadata->rtsf = WDI_RX_BD_GET_RTSF(pBDHeader);
      pRxMetadata->bsf = WDI_RX_BD_GET_BSF(pBDHeader);
      pRxMetadata->scan = WDI_RX_BD_GET_SCAN(pBDHeader);
      pRxMetadata->dpuSig = WDI_RX_BD_GET_DPU_SIG(pBDHeader);
      pRxMetadata->ft = WDI_RX_BD_GET_FT(pBDHeader);
      pRxMetadata->ne = WDI_RX_BD_GET_NE(pBDHeader);
      pRxMetadata->llcr = WDI_RX_BD_GET_LLCR(pBDHeader);
      pRxMetadata->bcast = WDI_RX_BD_GET_UB(pBDHeader);
      pRxMetadata->tid = ucTid;
      pRxMetadata->dpuFeedback = WDI_RX_BD_GET_DPU_FEEDBACK(pBDHeader);
      pRxMetadata->rateIndex = WDI_RX_BD_GET_RATEINDEX(pBDHeader);
      pRxMetadata->rxpFlags = WDI_RX_BD_GET_RXPFLAGS(pBDHeader);
      pRxMetadata->mclkRxTimestamp = WDI_RX_BD_GET_TIMESTAMP(pBDHeader);
#ifdef WLAN_FEATURE_11W
      pRxMetadata->rmf = WDI_RX_BD_GET_RMF(pBDHeader);
#endif
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
      pRxMetadata->offloadScanLearn = WDI_RX_BD_GET_OFFLOADSCANLEARN(pBDHeader);
      pRxMetadata->roamCandidateInd = WDI_RX_BD_GET_ROAMCANDIDATEIND(pBDHeader);
#endif

      /*                                                                  
                                                                                   */
      if ( 0 == WDI_RX_BD_GET_FT(pBDHeader) ) {
        if ( bASF ) {
          pRxMetadata->subtype = WDI_MAC_DATA_QOS_DATA;
          pRxMetadata->type    = WDI_MAC_DATA_FRAME;
        } else {
          pMacFrameCtl = (tpSirMacFrameCtl)(((wpt_uint8*)pBDHeader) + ucMPDUHOffset);
          pRxMetadata->subtype = pMacFrameCtl->subType;
          pRxMetadata->type    = pMacFrameCtl->type;
        }
      } else {
        pMacFrameCtl = (tpSirMacFrameCtl)(((wpt_uint8*)pBDHeader) + WDI_RX_BD_HEADER_SIZE);
        pRxMetadata->subtype = pMacFrameCtl->subType;
        pRxMetadata->type    = pMacFrameCtl->type;
      }

      pRxMetadata->mpduHeaderPtr = pBDHeader + ucMPDUHOffset;
      pRxMetadata->mpduDataPtr = pBDHeader + usMPDUDOffset;
      pRxMetadata->mpduLength = usMPDULen;
      pRxMetadata->mpduHeaderLength = ucMPDUHLen;

      /*                                                                        
                                 
                                                                                */
      pRxMetadata->ampdu_reorderOpcode  = (wpt_uint8)WDI_RX_BD_GET_BA_OPCODE(pBDHeader);
      pRxMetadata->ampdu_reorderSlotIdx = (wpt_uint8)WDI_RX_BD_GET_BA_SI(pBDHeader);
      pRxMetadata->ampdu_reorderFwdIdx  = (wpt_uint8)WDI_RX_BD_GET_BA_FI(pBDHeader);
      pRxMetadata->currentPktSeqNo       = (wpt_uint16)WDI_RX_BD_GET_BA_CSN(pBDHeader);


      /*                                                                        
                                 
                                                                                */
      pRxMetadata->amsdu_asf  =  bASF;
      pRxMetadata->amsdu_aef  =  bAEF;
      pRxMetadata->amsdu_esf  =  bFSF;
      pRxMetadata->amsdu_lsf  =  bLSF;
      pRxMetadata->amsdu_size =  WDI_RX_BD_GET_AMSDU_SIZE(pBDHeader);

      pRxMetadata->rssi0 = WDI_RX_BD_GET_RSSI0(pBDHeader);
      pRxMetadata->rssi1 = WDI_RX_BD_GET_RSSI1(pBDHeader);


        /*          
                                  
                                      
                                         
                                  

                                                     
                                                                      
                      
      */

      pRxMetadata->replayCount = WDTS_GetReplayCounterFromRxBD(pBDHeader);
      pRxMetadata->snr = WDI_RX_BD_GET_SNR(pBDHeader); 

      /* 
                                                          
       */ 
      WPAL_PACKET_SET_BD_POINTER(pFrame, pBDHeader);
      WPAL_PACKET_SET_BD_LENGTH(pFrame, sizeof(WDI_RxBdType));

      //                            
      pClientData->receiveFrameCB(pClientData->pCallbackContext, pFrame);  
  }
  else
  {
      wpalPacketSetRxLength(pFrame, usMPDULen+ucMPDUHOffset);
      wpalPacketRawTrimHead(pFrame, ucMPDUHOffset);

      pRxMetadata = WDI_DS_ExtractRxMetaData(pFrame);
      //                    
      pRxMetadata->fc = isFcBd;
      pRxMetadata->mclkRxTimestamp = WDI_RX_BD_GET_TIMESTAMP(pBDHeader);
      pRxMetadata->fcStaTxDisabledBitmap = WDI_RX_FC_BD_GET_STA_TX_DISABLED_BITMAP(pBDHeader);
      pRxMetadata->fcSTAValidMask = WDI_RX_FC_BD_GET_STA_VALID_MASK(pBDHeader);
      //                            
      pClientData->receiveFrameCB(pClientData->pCallbackContext, pFrame);  
  }

  //                
  if(gDsTrafficStats.running && pRxMetadata->staId < HAL_NUM_STA)
  {
     if(pRxMetadata->rateIndex < WDTS_MAX_RATE_NUM)
     {
        if(pRxMetadata->type == WDI_MAC_DATA_FRAME)
        {
           gDsTrafficStats.rxStats[pRxMetadata->staId][pRxMetadata->rateIndex].rxBytesRcvd +=
              pRxMetadata->mpduLength;
           gDsTrafficStats.rxStats[pRxMetadata->staId][pRxMetadata->rateIndex].rxPacketsRcvd++;
        }
     }
  }
  return eWLAN_PAL_STATUS_SUCCESS;
}



/*                                      
                                                                       
                                  
              
                                                            
                                                        
                                                 
                                                        
 */
wpt_status WDTS_OOResourceNotification(void *pContext, WDTS_ChannelType channel, wpt_boolean on)
{
  WDI_DS_ClientDataType *pClientData =
    (WDI_DS_ClientDataType *) pContext;
  static wpt_uint8 ac_mask = 0x1f;

  //                 
  if(NULL == pContext){
    return eWLAN_PAL_STATUS_E_FAILURE;
  }
  
  if(on){
    ac_mask |=  channel == WDTS_CHANNEL_TX_LOW_PRI?  0x0f : 0x10;
  } else {
    ac_mask &=  channel == WDTS_CHANNEL_TX_LOW_PRI?  0x10 : 0x0f;
  }


  //                    
  pClientData->txResourceCB(pClientData->pCallbackContext, ac_mask); 
  return eWLAN_PAL_STATUS_SUCCESS;

}

/*                     
                                                         
              
                                                                   
                      
  
                                                 
                                                        
  
 */
wpt_status WDTS_openTransport( void *pContext)
{
  void *pDTDriverContext; 
  WDI_DS_ClientDataType *pClientData;
  WDI_Status sWdiStatus = WDI_STATUS_SUCCESS;

  pClientData = (WDI_DS_ClientDataType*) wpalMemoryAllocate(sizeof(WDI_DS_ClientDataType));
  if (!pClientData){
    return eWLAN_PAL_STATUS_E_NOMEM;
  }

  pClientData->suspend = 0;
  WDI_DS_AssignDatapathContext(pContext, (void*)pClientData);

  pDTDriverContext = gTransportDriver.open(); 
  if( NULL == pDTDriverContext )
  {
     DTI_TRACE( DTI_TRACE_LEVEL_ERROR, " %s fail from transport open", __func__);
     return eWLAN_PAL_STATUS_E_FAILURE;
  }
  WDT_AssignTransportDriverContext(pContext, pDTDriverContext);
  gTransportDriver.register_client(pDTDriverContext, WDTS_RxPacket, WDTS_TxPacketComplete, 
    WDTS_OOResourceNotification, (void*)pClientData);

  /*                                         */
  sWdiStatus = WDI_DS_MemPoolCreate(&pClientData->mgmtMemPool, WDI_DS_MAX_CHUNK_SIZE, 
                                                     WDI_DS_HI_PRI_RES_NUM);
  if (WDI_STATUS_SUCCESS != sWdiStatus){
    return eWLAN_PAL_STATUS_E_NOMEM;
  }

  /*                                         */
  sWdiStatus = WDI_DS_MemPoolCreate(&pClientData->dataMemPool, WDI_DS_MAX_CHUNK_SIZE, 
                                                      WDI_DS_LO_PRI_RES_NUM);
  if (WDI_STATUS_SUCCESS != sWdiStatus){
    return eWLAN_PAL_STATUS_E_NOMEM;
  }

  wpalMemoryZero(&gDsTrafficStats, sizeof(gDsTrafficStats));

  return eWLAN_PAL_STATUS_SUCCESS;

}



/*                      
                                                      
              
                                                                  
                     
  
                                                 
                                                        
  
 */
wpt_status WDTS_startTransport( void *pContext)
{
  void *pDTDriverContext = WDT_GetTransportDriverContext(pContext);
  gTransportDriver.start(pDTDriverContext); 
  return eWLAN_PAL_STATUS_SUCCESS;

}


/*                         
                                                                                                      
              
                                                                                    
                                
                                                 
                                                        
  
 */
wpt_status WDTS_TxPacket(void *pContext, wpt_packet *pFrame)
{
  void *pDTDriverContext = WDT_GetTransportDriverContext(pContext);
  WDI_DS_TxMetaInfoType     *pTxMetadata;
  WDTS_ChannelType channel = WDTS_CHANNEL_TX_LOW_PRI;
  wpt_status status = eWLAN_PAL_STATUS_SUCCESS;

  //                                 
  pTxMetadata = WDI_DS_ExtractTxMetaData(pFrame);

  //                
  if(gDsTrafficStats.running && pTxMetadata->staIdx < HAL_NUM_STA)
  {
     if(pTxMetadata->frmType & WDI_MAC_DATA_FRAME)
     {
        gDsTrafficStats.txStats[pTxMetadata->staIdx].txBytesPushed +=
           pTxMetadata->fPktlen;
        gDsTrafficStats.txStats[pTxMetadata->staIdx].txPacketsPushed += 1;
      }
  }

  //                                 
  channel =  (pTxMetadata->frmType & WDI_MAC_DATA_FRAME)? 
    /*                                              
                                                                              
                                                
                                                                               
     */
#ifdef FEATURE_WLAN_TDLS
     /*                                                                           
                                                                                                   
                                                                                         
                                          
      */
      (((pTxMetadata->isEapol) || (pTxMetadata->txFlags & WDI_USE_BD_RATE2_FOR_MANAGEMENT_FRAME))? WDTS_CHANNEL_TX_HIGH_PRI : WDTS_CHANNEL_TX_LOW_PRI) : WDTS_CHANNEL_TX_HIGH_PRI;
#else
      ((pTxMetadata->isEapol) ? WDTS_CHANNEL_TX_HIGH_PRI : WDTS_CHANNEL_TX_LOW_PRI) : WDTS_CHANNEL_TX_HIGH_PRI;
#endif
  //                                   
  status =  gTransportDriver.xmit(pDTDriverContext, pFrame, channel);
  return status;
}

/*                           
                                                                                              
              
                                                                                    
                                               
                                                 
                                                        
  
 */
wpt_status WDTS_CompleteTx(void *pContext, wpt_uint32 ucTxResReq)
{
  void *pDTDriverContext = WDT_GetTransportDriverContext(pContext);
  
  //                                         
  return gTransportDriver.txComplete(pDTDriverContext, ucTxResReq);
}

/*                                   
                                                                             
                                   
              
                                      
                      
  
 */
void  WDTS_SetPowerStateCb(wpt_status   status, unsigned int dxePhyAddr)
{
   //           
   if(NULL != gSetPowerStateCbInfo.cback) 
   {
      gSetPowerStateCbInfo.cback(status, dxePhyAddr, gSetPowerStateCbInfo.pUserData);
   }
}


/*                               
                                                                                    
              
                                                                                    
                                             
                                                              
                                                        
  
 */
wpt_status WDTS_SetPowerState(void *pContext, WDTS_PowerStateType  powerState,
                              WDTS_SetPowerStateCbType cback)
{
   void *pDTDriverContext = WDT_GetTransportDriverContext(pContext);
   wpt_status status = eWLAN_PAL_STATUS_SUCCESS;

   if( cback )
   {
      //                       
      gSetPowerStateCbInfo.pUserData = pContext;
      gSetPowerStateCbInfo.cback = cback;
      status = gTransportDriver.setPowerState(pDTDriverContext, powerState,
                                            WDTS_SetPowerStateCb);
   }
   else
   {
      status = gTransportDriver.setPowerState(pDTDriverContext, powerState,
                                               NULL);
   }

   return status;
}

/*                            
                                            
                                                   
                                                           
              
                                                 
                                                   
                                                                         
                                                             
                     
  
 */
void WDTS_ChannelDebug(wpt_boolean displaySnapshot, wpt_uint8 debugFlags)
{
   gTransportDriver.channelDebug(displaySnapshot, debugFlags);
   return;
}

/*                    
                                      
              
                                                                                    
                                                 
                                                        
  
 */
wpt_status WDTS_Stop(void *pContext)
{
  void *pDTDriverContext = WDT_GetTransportDriverContext(pContext);
  wpt_status status = eWLAN_PAL_STATUS_SUCCESS;

  status =  gTransportDriver.stop(pDTDriverContext);

  wpalMemoryZero(&gDsTrafficStats, sizeof(gDsTrafficStats));

  return status;
}

/*                    
                                      
              
                                                                                    
                                                 
                                                        
  
 */
wpt_status WDTS_Close(void *pContext)
{
  void *pDTDriverContext = WDT_GetTransportDriverContext(pContext);
  WDI_DS_ClientDataType *pClientData = WDI_DS_GetDatapathContext(pContext);
  wpt_status status = eWLAN_PAL_STATUS_SUCCESS;

  /*                                        */
  WDI_DS_MemPoolDestroy(&pClientData->mgmtMemPool);
  
  /*                                        */
  WDI_DS_MemPoolDestroy(&pClientData->dataMemPool);
  
  status =  gTransportDriver.close(pDTDriverContext);

  wpalMemoryFree(pClientData);

  return status;
}

/*                                            
              
                                                                                     
                                                               
  
 */
wpt_uint32 WDTS_GetFreeTxDataResNumber(void *pContext)
{
  return 
     gTransportDriver.getFreeTxDataResNumber(WDT_GetTransportDriverContext(pContext));
}

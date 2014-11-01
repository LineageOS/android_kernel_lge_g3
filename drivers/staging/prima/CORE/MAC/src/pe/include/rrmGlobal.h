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

#if !defined( __RRMGLOBAL_H )
#define __RRMGLOBAL_H

/**=========================================================================

  \file  rrmGlobal.h

  \brief Definitions for SME APIs

   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.

   Qualcomm Confidential and Proprietary.

  ========================================================================*/

typedef enum eRrmRetStatus
{
    eRRM_SUCCESS,
    eRRM_INCAPABLE,
    eRRM_REFUSED,
    eRRM_FAILURE
} tRrmRetStatus;

typedef struct sSirChannelInfo
{
   tANI_U8 regulatoryClass;
   tANI_U8 channelNum;
} tSirChannelInfo, * tpSirChannelInfo;

typedef struct sSirBeaconReportReqInd
{
   tANI_U16     messageType; //                               
   tANI_U16     length;
   tSirMacAddr  bssId;
   tANI_U16     measurementDuration[SIR_CCX_MAX_MEAS_IE_REQS];   //  
   tANI_U16     randomizationInterval; //  
   tSirChannelInfo channelInfo;
   tSirMacAddr      macaddrBssid;   //           
   tANI_U8      fMeasurementtype[SIR_CCX_MAX_MEAS_IE_REQS];  //                                   
   tAniSSID     ssId;              //               
   tANI_U16      uDialogToken;
   tSirChannelList channelList; //                       
} tSirBeaconReportReqInd, * tpSirBeaconReportReqInd;


typedef struct sSirBeaconReportXmitInd
{
   tANI_U16    messageType; //                                     
   tANI_U16    length;
   tSirMacAddr bssId;
   tANI_U16     uDialogToken;
   tANI_U8     fMeasureDone;
   tANI_U16    duration;
   tANI_U8     regClass;
   tANI_U8     numBssDesc;
   tpSirBssDescription pBssDescription[SIR_BCN_REPORT_MAX_BSS_DESC];
} tSirBeaconReportXmitInd, * tpSirBeaconReportXmitInd;

typedef struct sSirNeighborReportReqInd
{
   tANI_U16     messageType; //                                 
   tANI_U16     length;
   tSirMacAddr  bssId;  //                
   tANI_U16     noSSID; //                                        
                        //                                                  
   tSirMacSSid  ucSSID;  
} tSirNeighborReportReqInd, * tpSirNeighborReportReqInd;
                                   

typedef struct sSirNeighborBssDescription
{
   tANI_U16        length;
   tSirMacAddr     bssId;
   tANI_U8         regClass;
   tANI_U8         channel;
   tANI_U8         phyType;
   union sSirNeighborBssidInfo {
         struct _rrmInfo {
                tANI_U32      fApPreauthReachable:2;  //                            
                tANI_U32      fSameSecurityMode:1;
                tANI_U32      fSameAuthenticator:1;
                tANI_U32      fCapSpectrumMeasurement:1; //                            
                tANI_U32      fCapQos:1; 
                tANI_U32      fCapApsd:1; 
                tANI_U32      fCapRadioMeasurement:1; 
                tANI_U32      fCapDelayedBlockAck:1; 
                tANI_U32      fCapImmediateBlockAck:1;
                tANI_U32      fMobilityDomain:1;
                tANI_U32      reserved:21; 
         } rrmInfo;
         struct _ccxInfo {
                tANI_U32      channelBand:8;
                tANI_U32      minRecvSigPower:8;
                tANI_U32      apTxPower:8;
                tANI_U32      roamHysteresis:8;
                tANI_U32      adaptScanThres:8;

                tANI_U32      transitionTime:8;
                tANI_U32      tsfOffset:16;

                tANI_U32      beaconInterval:16;
                tANI_U32      reserved: 16;
         } ccxInfo;
   } bssidInfo;
 
   //                                     
}tSirNeighborBssDescription, *tpSirNeighborBssDescripton;

typedef struct sSirNeighborReportInd
{
   tANI_U16     messageType; //                             
   tANI_U16     length;
   tANI_U16     numNeighborReports;
   tSirMacAddr  bssId;  //                
   //                              
   tSirNeighborBssDescription sNeighborBssDescription[1];
} tSirNeighborReportInd, * tpSirNeighborReportInd;

typedef struct sRRMBeaconReportRequestedIes
{
   tANI_U8 num;
   tANI_U8 *pElementIds;
}tRRMBeaconReportRequestedIes, *tpRRMBeaconReportRequestedIes;

//                         
//                                                                
#define BEACON_REPORTING_DETAIL_NO_FF_IE 0
#define BEACON_REPORTING_DETAIL_ALL_FF_REQ_IE 1
#define BEACON_REPORTING_DETAIL_ALL_FF_IE 2


typedef struct sRRMReq
{
   tANI_U8 dialog_token; //                
   tANI_U8 token; //                          
   tANI_U8 type;
   union {
      struct {
         tANI_U8 reportingDetail;
         tRRMBeaconReportRequestedIes reqIes;
      }Beacon;
   }request;
}tRRMReq, *tpRRMReq;

typedef struct sRRMCaps
{
    tANI_U8  LinkMeasurement: 1;
    tANI_U8      NeighborRpt: 1;
    tANI_U8         parallel: 1;
    tANI_U8         repeated: 1;
    tANI_U8    BeaconPassive: 1;
    tANI_U8     BeaconActive: 1;
    tANI_U8      BeaconTable: 1;
    tANI_U8    BeaconRepCond: 1;
    tANI_U8 FrameMeasurement: 1;
    tANI_U8      ChannelLoad: 1;
    tANI_U8   NoiseHistogram: 1;
    tANI_U8       statistics: 1;
    tANI_U8   LCIMeasurement: 1;
    tANI_U8       LCIAzimuth: 1;
    tANI_U8    TCMCapability: 1;
    tANI_U8     triggeredTCM: 1;
    tANI_U8     APChanReport: 1;
    tANI_U8    RRMMIBEnabled: 1;
    tANI_U8 MeasurementPilotEnabled: 1;
    tANI_U8 NeighborTSFOffset: 1;
    tANI_U8  RCPIMeasurement: 1;
    tANI_U8  RSNIMeasurement: 1;
    tANI_U8 BssAvgAccessDelay: 1;
    tANI_U8 BSSAvailAdmission: 1;
    tANI_U8 AntennaInformation: 1;

    tANI_U8 operatingChanMax;
    tANI_U8 nonOperatingChanMax;
    tANI_U8 MeasurementPilot;
}tRRMCaps, *tpRRMCaps;

typedef struct sRrmPEContext
{
   tANI_U8  rrmEnable;
   //                             
   tANI_U32   startTSF[2]; //                                                                                                 
                           //                                                                                        
   tRRMCaps   rrmEnabledCaps;
   tPowerdBm  txMgmtPower;
   tANI_U8  DialogToken; //                                                    
   tpRRMReq pCurrentReq;
}tRrmPEContext, *tpRrmPEContext;

//                                                   
#define RCPI_LOW_RSSI_VALUE   (-110)
#define RCPI_MAX_VALUE        (220)
#define CALCULATE_RCPI(rssi)  (((rssi) + 110) * 2)


#endif //                         

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

#if !defined( __WLAN_QCT_WTI_DS_H )
#define __WLAN_QCT_WTI_DS_H

/**=========================================================================
 *     
 *       \file  wlan_qct_wdi_ds.h
 *          
 *       \brief define Dataservice API 
 *                               
 * WLAN Device Abstraction layer External API for Dataservice
 * DESCRIPTION
 *  This file contains the external API exposed by the 
 *   wlan device abstarction layer module.
 *
 *   Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved.
 *   Qualcomm Confidential and Proprietary
 */


#include "wlan_qct_pal_type.h"
#include "wlan_qct_pal_status.h"
#include "wlan_qct_pal_packet.h"
#include "wlan_qct_wdi.h"
   

typedef struct 
{
   wpt_uint32 txFlags;
   wpt_uint8 ac;
   wpt_uint8 isEapol:1; //                       
   wpt_uint8 isWai:1;   //                       
   wpt_uint8 fdisableFrmXlt:1;   //                                    
   wpt_uint8 qosEnabled:1; //                   
   wpt_uint8 fenableWDS:1; //                 
   wpt_uint8 reserved1:3;
   wpt_uint8 typeSubtype;
   wpt_uint8 fUP;
   wpt_uint8 fSTAMACAddress[6];
   wpt_uint8 addr2MACAddress[6];
   wpt_uint8 frmType;
   wpt_uint8 fStaType;
   wpt_uint8 fProtMgmtFrame;
   wpt_uint16 fPktlen;
   wpt_status txCompleteStatus;
   wpt_uint8  staIdx;
} WDI_DS_TxMetaInfoType;


typedef enum
{
  WDI_DS_OPCODE_INVALID         = 0,
  WDI_DS_OPCODE_QCUR_FWDBUF     = 1,
  WDI_DS_OPCODE_FWDBUF_FWDCUR   = 2,
  WDI_DS_OPCODE_QCUR            = 3,
  WDI_DS_OPCODE_FWDBUF_QUEUECUR = 4,
  WDI_DS_OPCODE_FWDBUF_DROPCUR  = 5,
  WDI_DS_OPCODE_FWDALL_DROPCUR  = 6,
  WDI_DS_OPCODE_FWDALL_QCUR     = 7,
  WDI_DS_OPCODE_TEARDOWN        = 8,
  WDI_DS_OPCODE_DROPCUR         = 9,
  WDI_DS_OPCODE_MAX
}WDI_DS_BAOpCodeEnumType;

typedef struct 
{
   wpt_uint8 staId;
   wpt_uint8 addr3Idx;
   wpt_uint8 rxChannel;
   wpt_uint8 type:2;
   wpt_uint8 subtype:4;
   wpt_uint8 rfBand:2;

   wpt_uint16 rtsf:1;  //                                                                     
   wpt_uint16 bsf:1;   //                                    
   wpt_uint16 unknownUcastPkt:1;   //                                           
   wpt_uint16 scan:1;   //                                      
   wpt_uint16 dpuSig:3;   //             
   wpt_uint16 ft:1;   //                                         
   wpt_uint16 ne:1;   //                                                                
   wpt_uint16 llcr:1;   //                                 
   wpt_uint16 bcast:1;   //                                                 
   wpt_uint16 tid:4;
   wpt_uint16 reserved1:1;
   wpt_uint8 dpuFeedback;
   wpt_int8 snr;

   wpt_uint32 currentPktSeqNo:12;  /*                        */
   wpt_uint32 ampdu_reorderOpcode:4;
   wpt_uint32 ampdu_reorderSlotIdx:6;
   wpt_uint32 ampdu_reorderFwdIdx:6;
   wpt_uint32 reserved3:4;

   wpt_uint16 amsdu_size;
   wpt_uint32 amsdu_asf:1;
   wpt_uint32 amsdu_esf:1;
   wpt_uint32 amsdu_lsf:1;
   wpt_uint32 amsdu_aef:1;
   wpt_uint32 reserved2:4;

   wpt_uint8 *mpduHeaderPtr;
   wpt_uint8 *mpduDataPtr;
   wpt_uint32  mpduLength;
   wpt_uint32  mpduHeaderLength;

   wpt_uint32  rateIndex;
   wpt_uint32  rxpFlags;
   wpt_uint32  mclkRxTimestamp;

   //                   
   wpt_uint8  fc;
   wpt_uint32 fcSTATxQStatus:16;
   wpt_uint32 fcSTAThreshIndMask:16;
   wpt_uint32 fcSTAPwrSaveStateMask:16;
   wpt_uint32 fcSTAValidMask:16;

   wpt_uint16 fcStaTxDisabledBitmap;
   wpt_uint8 fcSTATxQLen[12]; //                   
   wpt_uint8 fcSTACurTxRate[12]; //                              

   wpt_uint64 replayCount;

   wpt_uint32 rssi0;
   wpt_uint32 rssi1;

#ifdef WLAN_FEATURE_11W
   wpt_uint32 rmf:1;
#endif
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
   wpt_uint32 offloadScanLearn;
   wpt_uint32 roamCandidateInd;
#endif
} WDI_DS_RxMetaInfoType;

typedef struct sPktMetaInfo
{
   union
   {
      WDI_DS_TxMetaInfoType txMetaInfo;
      WDI_DS_RxMetaInfoType rxMetaInfo;
   } u;
} WDI_DS_MetaInfoType;

WPT_STATIC WPT_INLINE WDI_DS_RxMetaInfoType* WDI_DS_ExtractRxMetaData (wpt_packet *pFrame)
{
  WDI_DS_RxMetaInfoType * pRxMetadata =
    &((WDI_DS_MetaInfoType *)WPAL_PACKET_GET_METAINFO_POINTER(pFrame))->u.rxMetaInfo;
  return pRxMetadata;
}


WPT_STATIC WPT_INLINE WDI_DS_TxMetaInfoType* WDI_DS_ExtractTxMetaData (wpt_packet *pFrame)
{
        WDI_DS_TxMetaInfoType * pTxMetadata =
                &((WDI_DS_MetaInfoType *)WPAL_PACKET_GET_METAINFO_POINTER(pFrame))->u.txMetaInfo;
        return pTxMetadata;
}


typedef void (*WDI_DS_TxCompleteCallback)(void *pContext, wpt_packet *pFrame);
typedef void (*WDI_DS_RxPacketCallback) (void *pContext, wpt_packet *pFrame);
typedef void (*WDI_DS_TxFlowControlCallback)(void *pContext, wpt_uint8 ac_mask);



/*                            
              
                                                                   
                      
                                                                            
                                        
                                                                           
                                    
                                                                        
                               
  
                                                 
                                                        
  
 */
WDI_Status WDI_DS_Register( void *pContext, 
  WDI_DS_TxCompleteCallback pfnTxCompleteCallback,
  WDI_DS_RxPacketCallback pfnRxPacketCallback, 
  WDI_DS_TxFlowControlCallback pfnTxFlowControlCallback,
  void *pCallbackContext);



/*                        
              
                                                                                     
                                 
                                                             
                                                 
                                                        
  
 */


WDI_Status WDI_DS_TxPacket(void *pContext,
  wpt_packet *pFrame,
  wpt_boolean more);
  
  
/*                                 
              
                                                                                     
                                                
                                                 
                                                        
  
 */


WDI_Status WDI_DS_TxComplete(void *pContext, wpt_uint32 ucTxResReq);

/*                                
              
                                                                                     
                                                 
                                                        
  
 */


WDI_Status WDI_DS_TxSuspend(void *pContext);


/*                               
              
                                                                                     
                                                 
                                                        
  
 */


WDI_Status WDI_DS_TxResume(void *pContext);

/*                                  
              
                                                                                     
                                     
                 
                                                 
  
 */
wpt_uint32 WDI_DS_GetReservedResCountPerSTA(void *pContext,
                                                        WDI_ResPoolType wdiResPool,
                                                        wpt_uint8 staId);

/*                         
              
                                                                                     
                 
                                
  
 */
WDI_Status WDI_DS_AddSTAMemPool(void *pContext, wpt_uint8 staIndex);

/*                            
              
                                                                                     
                 
                                
  
 */
WDI_Status WDI_DS_DelSTAMemPool(void *pContext, wpt_uint8 staIndex);

/*                                              
              
                                                                                     
                     
                                              
                                  
  
 */
WDI_Status WDI_DS_SetStaIdxPerBssIdx(void *pContext, wpt_uint8 bssIdx, wpt_uint8 staIdx);

/*                                              
              
                                                                                     
                     
                                              
                                  
  
 */
WDI_Status WDI_DS_GetStaIdxFromBssIdx(void *pContext, wpt_uint8 bssIdx, wpt_uint8 *staIdx);

/*                                                
              
                                                                                     
                     
                                              
                                  
  
 */
WDI_Status WDI_DS_ClearStaIdxPerBssIdx(void *pContext, wpt_uint8 bssIdx, wpt_uint8 staIdx);

/*                               
                                                             
               
                                         
                                              
                       
 */
void WDI_DS_GetTrafficStats(WDI_TrafficStatsType** pStats, wpt_uint32 *len);

/*                                      
                                                                         
                    
                       
 */
void WDI_DS_DeactivateTrafficStats(void);

/*                                    
                                                                       
                    
                       
 */
void WDI_DS_ActivateTrafficStats(void);

/*                                 
                                                          
                    
                       
 */
void WDI_DS_ClearTrafficStats(void);

#endif

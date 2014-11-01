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

#ifndef WLAN_QCT_HAL_H
#define WLAN_QCT_HAL_H
#include "vos_status.h"
#include "halTypes.h"
#ifndef PALTYPES_H__


//                      
#define tANI_U8        v_U8_t
 
//                       
#define tANI_U16    v_U16_t    

//                       
#define tANI_U32    v_U32_t
 
//                    
#define    tANI_S8        v_S7_t
 
//                     
#define tANI_S16    v_S15_t
 
//                     
#define tANI_S32    v_S31_t

#define eHalStatus    VOS_STATUS

#endif
#define QWLAN_HAL_DXE0_MASTERID  5

typedef struct sHalBdGeneric {
    /*      */
    //             
    tANI_U32 dpuRF : 8;
    tANI_U32 dpuSignature:3;     /*                                  */
    tANI_U32 staSignature:3;
    tANI_U32 reserved : 14;
    tANI_U32 dpuNE : 1;
    tANI_U32 dpuNC : 1;
    tANI_U32 bdt : 2;                        /*         */
    //           

    /*      */
    //             
    tANI_U32 reserved1:32;                      
    //           


    /*      */
    //             
    tANI_U32 headPduIdx : 16;                /*                */
    tANI_U32 tailPduIdx : 16;                /*                */
    //           

    /*      */
    //             
    tANI_U32 mpduHeaderLength : 8;           /*                    */
    tANI_U32 mpduHeaderOffset : 8;           /*                          */
    tANI_U32 mpduDataOffset : 9;             /*                        */
    tANI_U32 pduCount : 7;                   /*           */
    //           

    /*      */
    //             
    tANI_U32 mpduLength : 16;                /*             */
    tANI_U32 reserved3:4;            /*                          */
    tANI_U32 tid : 4;                        /*                         */
    tANI_U32 rateIndex : 8;
    //           

    /*      */
    //             
    tANI_U32 dpuDescIdx : 8;
    tANI_U32 addr1Index : 8;  //                                
    tANI_U32 addr2Index : 8;  //                                
    tANI_U32 addr3Index : 8;  //                                
    //           
//                                                                       
} tHalBdGeneric, *tpHalBdGeneric;


/*
                 
 */

typedef struct sHalPdu {
    tANI_U8 payload[124];
    tANI_U32 nextPduIdx;                     /*             */
//                                                            
} tHalPdu, *tpHalPdu;

/*                                               */
typedef struct sUapsdInfo {
    tANI_U8  staidx;        //          
    tANI_U8  ac;            //                
    tANI_U8  up;            //              
    tANI_U32 srvInterval;   //                 
    tANI_U32 susInterval;   //                 
    tANI_U32 delayInterval; //               
} tUapsdInfo, tpUapsdInfo;

#define HAL_TXBD_BDRATE_DEFAULT 0
#define HAL_TXBD_BDRATE_FIRST   1
#define HAL_TXBD_BDRATE_SECOND  2
#define HAL_TXBD_BDRATE_THIRD   3

#define HAL_FRAME_TYPE_MASK     0x30
#define HAL_FRAME_TYPE_OFFSET   0x4
#define HAL_FRAME_SUBTYPE_MASK  0x0F

#define HAL_TXBD_BD_SSN_FILL_HOST             0
#define HAL_TXBD_BD_SSN_FILL_DPU_NON_QOS    1
#define HAL_TXBD_BD_SSN_FILL_DPU_QOS        2

#define HAL_ACKPOLICY_ACK_REQUIRED        0
#define HAL_ACKPOLICY_ACK_NOTREQUIRED    1

#define HAL_BDRATE_BCDATA_FRAME            1
#define HAL_BDRATE_BCMGMT_FRAME            2
#define HAL_BDRATE_CTRL_FRAME            3
    
/*                              */
#define HAL_DEFAULT_UNICAST_ENABLED     1
#define HAL_RMF_DISABLED                 0
#define HAL_RMF_ENABLED                 1
#define HAL_NO_ENCRYPTION_DISABLED        0
#define HAL_NO_ENCRYPTION_ENABLED         1
    
#define WLANHAL_RX_BD_ADDR3_SELF_IDX      0

//                                                                             
#define WLANHAL_TX_BD_HEADER_SIZE 40
#define WLANHAL_RX_BD_HEADER_SIZE 76


#define WLANHAL_RX_BD_HEADER_OFFSET       0

#define WLANHAL_RX_BD_GET_MPDU_H_OFFSET( _pvBDHeader )   (((tpHalRxBd)_pvBDHeader)->mpduHeaderOffset)

#define WLANHAL_RX_BD_GET_MPDU_D_OFFSET( _pvBDHeader )   (((tpHalRxBd)_pvBDHeader)->mpduDataOffset)

#define WLANHAL_RX_BD_GET_MPDU_LEN( _pvBDHeader )        (((tpHalRxBd)_pvBDHeader)->mpduLength)

#define WLANHAL_RX_BD_GET_MPDU_H_LEN( _pvBDHeader )      (((tpHalRxBd)_pvBDHeader)->mpduHeaderLength)

#define WLANHAL_RX_BD_GET_FT( _pvBDHeader )        (((tpHalRxBd)_pvBDHeader)->ft)

#define WLANHAL_RX_BD_GET_LLC( _pvBDHeader )        (((tpHalRxBd)_pvBDHeader)->llc)

#define WLANHAL_RX_BD_GET_TID( _pvBDHeader )        (((tpHalRxBd)_pvBDHeader)->tid)

#define WLANHAL_RX_BD_GET_ASF( _pvBDHeader )        (((tpHalRxBd)_pvBDHeader)->asf)

#define WLANHAL_RX_BD_GET_AEF( _pvBDHeader )           (((tpHalRxBd)_pvBDHeader)->aef)

#define WLANHAL_RX_BD_GET_LSF( _pvBDHeader )           (((tpHalRxBd)_pvBDHeader)->lsf)

#define WLANHAL_RX_BD_GET_ESF( _pvBDHeader )           (((tpHalRxBd)_pvBDHeader)->esf)

#define WLANHAL_RX_BD_GET_STA_ID( _pvBDHeader )     (((tpHalRxBd)_pvBDHeader)->addr2Index)
#define WLANHAL_RX_BD_GET_ADDR3_IDX( _pvBDHeader )     (((tpHalRxBd)_pvBDHeader)->addr3Index)
#define WLANHAL_RX_BD_GET_ADDR1_IDX( _pvBDHeader )     (((tpHalRxBd)_pvBDHeader)->addr1Index)

#define WLANHAL_TX_BD_GET_TID( _pvBDHeader )           (((tpHalTxBd)_pvBDHeader)->tid)
#define WLANHAL_TX_BD_GET_STA_ID( _pvBDHeader )    (((tpHalTxBd)_pvBDHeader)->staIndex)

#define WLANHAL_RX_BD_GET_DPU_SIG( _pvBDHeader )   (((tpHalRxBd)_pvBDHeader)->dpuSignature)

#define WLANHAL_FC_RX_BD_REPORT_CONTENT_SIZE        (2 * HAL_NUM_STA * sizeof(tANI_U8))   //                                                             
#define WLANHAL_FC_TX_BD_HEADER_SIZE                sizeof(tHalFcTxBd)
#define WLANHAL_RX_BD_GET_FC( _pvBDHeader )                      (((tpHalFcRxBd)_pvBDHeader)->fc)
#define WLANHAL_RX_BD_GET_RX_TIME_STAMP( _pvBDHeader )           (((tpHalFcRxBd)_pvBDHeader)->mclkRxTimestamp)
#define WLANHAL_RX_BD_GET_STA_VALID_MASK( _pvBDHeader )          (((tpHalFcRxBd)_pvBDHeader)->fcSTAValidMask)
#define WLANHAL_RX_BD_GET_STA_PS_STATE( _pvBDHeader )            (((tpHalFcRxBd)_pvBDHeader)->fcSTAPwrSaveStateMask)
#define WLANHAL_RX_BD_GET_STA_TH_IND( _pvBDHeader )              (((tpHalFcRxBd)_pvBDHeader)->fcSTAThreshIndMask)
#define WLANHAL_RX_BD_GET_STA_TXQ_STATUS( _pvBDHeader )          (((tpHalFcRxBd)_pvBDHeader)->fcSTATxQStatus)
#define WLANHAL_RX_BD_GET_STA_TXQ_LEN( _pvBDHeader, staIdx )     (((tpHalFcRxBd)_pvBDHeader)->fcSTATxQLen[staIdx])
#define WLANHAL_RX_BD_GET_STA_CUR_TX_RATE( _pvBDHeader, staIdx ) (((tpHalFcRxBd)_pvBDHeader)->fcSTACurTxRate[staIdx])

#define WLANHAL_TX_BD_GET_RMF(_pvBDHeader)          (((tpHalRxBd)_pvBDHeader)->rmf)

#define WLANHAL_TX_BD_GET_UB(_pvBDHeader)           (((tpHalRxBd)_pvBDHeader)->ub)

#define WLANHAL_RX_BD_GET_RMF(_pvBDHeader)          (((tpHalRxBd)_pvBDHeader)->rmf)

#define WLANHAL_RX_BD_GET_UB(_pvBDHeader)           (((tpHalRxBd)_pvBDHeader)->ub)

#define WLANHAL_RX_BD_GET_RATEINDEX(_pvBDHeader)    (((tpHalRxBd)_pvBDHeader)->rateIndex)

#define WLANHAL_RX_BD_GET_TIMESTAMP(_pvBDHeader)    (((tpHalRxBd)_pvBDHeader)->mclkRxTimestamp)

#define tHalFcRxBd       halFcRxBd_type             
#define tpHalFcRxBd      phalFcRxBd_type
#define tHalFcTxBd       halFcTxBd_type
#define tpHalFcTxBd      pHalFcTxBd_type              
#define tHalFcTxParams   tFcTxParams_type
#define tHalFcRxParams   tFcRxParams_type               
#define tpHalFcTxParams  pFcTxParams_type               
#define tpHalFcRxParams  pFcRxParams_type             

/*                                                              */
#define WLANHAL_RX_BD_GET_RSSI0( _pvBDHeader )  \
    (((((tpHalRxBd)_pvBDHeader)->phyStats0) >> 24) & 0xff)
#define WLANHAL_RX_BD_GET_RSSI1( _pvBDHeader )  \
    (((((tpHalRxBd)_pvBDHeader)->phyStats0) >> 16) & 0xff)
#define WLANHAL_RX_BD_GET_RSSI2( _pvBDHeader )  \
    (((((tpHalRxBd)_pvBDHeader)->phyStats0) >> 0) & 0xff)
#define WLANHAL_RX_BD_GET_RSSI3( _pvBDHeader )  \
    ((((tpHalRxBd)_pvBDHeader)->phyStats0) & 0xff)

//                                 
#define WLANHAL_GET_RSSI_AVERAGE( _pvBDHeader ) \
    (((WLANHAL_RX_BD_GET_RSSI0(_pvBDHeader)) + \
    (WLANHAL_RX_BD_GET_RSSI1(_pvBDHeader)) + \
    (WLANHAL_RX_BD_GET_RSSI2(_pvBDHeader)) + \
    (WLANHAL_RX_BD_GET_RSSI3(_pvBDHeader))) / 4)

//                                 
#define WLANHAL_RX_BD_GET_SNR( _pvBDHeader )    \
    (((((tpHalRxBd)_pvBDHeader)->phyStats1) >> 24) & 0xff)
/*                                                                 */
#define WLANHAL_RX_BD_GET_DPU_SIG( _pvBDHeader )   (((tpHalRxBd)_pvBDHeader)->dpuSignature)


#define WLANHAL_TX_BD_SET_MPDU_DATA_OFFSET( _bd, _off )        (((tpHalTxBd)_bd)->mpduDataOffset = _off)
 
#define WLANHAL_TX_BD_SET_MPDU_HEADER_OFFSET( _bd, _off )    (((tpHalTxBd)_bd)->mpduHeaderOffset = _off)

#define WLANHAL_TX_BD_SET_MPDU_HEADER_LEN( _bd, _len )        (((tpHalTxBd)_bd)->mpduHeaderLength = _len)

#define WLANHAL_TX_BD_SET_MPDU_LEN( _bd, _len )                (((tpHalTxBd)_bd)->mpduLength = _len)

#define WLANHAL_RX_BD_GET_BA_OPCODE(_pvBDHeader)        (((tpHalRxBd)_pvBDHeader)->reorderOpcode)

#define WLANHAL_RX_BD_GET_BA_FI(_pvBDHeader)            (((tpHalRxBd)_pvBDHeader)->reorderFwdIdx)

#define WLANHAL_RX_BD_GET_BA_SI(_pvBDHeader)            (((tpHalRxBd)_pvBDHeader)->reorderSlotIdx)

#define WLANHAL_RX_BD_GET_BA_CSN(_pvBDHeader)           (((tpHalRxBd)_pvBDHeader)->currentPktSeqNo)

#define WLANHAL_RX_BD_GET_BA_ESN(_pvBDHeader)           (((tpHalRxBd)_pvBDHeader)->expectedPktSeqNo)

#define WLANHAL_RX_BD_GET_RXP_FLAGS(_pvBDHeader)            (((tpHalRxBd)_pvBDHeader)->rxpFlags)

#define WLANHAL_RX_BD_GET_TYPE_SUBTYPE(_pvBDHeader)            (((tpHalRxBd)_pvBDHeader)->frameTypeSubtype)
#define WLANHAL_RX_BD_SET_TYPE_SUBTYPE( _bd, _typeSubtype )        (((tpHalRxBd)_bd)->frameTypeSubtype = _typeSubtype)


#define WLANHAL_RX_BD_ASF_SET                1 /*                                                */

#define WLANHAL_RX_BD_FSF_SET               1

#define WLANHAL_RX_BD_LSF_SET               1

#define WLANHAL_RX_BD_AEF_SET               1

 
#define WLANHAL_RX_BD_LLC_PRESENT            0 /*                                          */

#define WLANHAL_RX_BD_FT_DONE                  1 /*                                               */

/*                                                                                 
                                        */
#define DPU_FEEDBACK_WPI_UNPROTECTED 0x20   
#define WLANHAL_RX_IS_UNPROTECTED_WPI_FRAME(_pvBDHeader)  \
        (DPU_FEEDBACK_WPI_UNPROTECTED == ((WDI_RxBdType *)_pvBDHeader)->dpuFeedback)

/*                                                                          

                                              

              
                                                                    

               
                                               
    
             

      
                                                   
                                                  
   
              
                                                                               
                                       

               
  
                                                                            */

tANI_U8 WLANHAL_RxBD_GetFrameTypeSubType(v_PVOID_t _pvBDHeader, tANI_U16 usFrmCtrl);


#define HAL_TXCOMP_REQUESTED_MASK           0x1  //                                 
#define HAL_USE_SELF_STA_REQUESTED_MASK     0x2  //                                               
#define HAL_TX_NO_ENCRYPTION_MASK           0x4  //                                               
#if defined(LIBRA_WAPI_SUPPORT)
#define HAL_WAPI_STA_MASK            0x8  //                                             
#endif

#define HAL_TRIGGER_ENABLED_AC_MASK         0x10 //                                                     
#define HAL_USE_NO_ACK_REQUESTED_MASK       0x20

#define HAL_USE_BD_RATE2_FOR_MANAGEMENT_FRAME 0x40 //                                                            
#define HAL_USE_PEER_STA_REQUESTED_MASK   0x80 //                                                      

#ifdef FEATURE_WLAN_TDLS
#define HAL_TDLS_PEER_STA_MASK              0x80 //                                
#endif
/*                                                                          

                              

              
                                                                         

               
                                                          
    
             

      
                                                                           
                                                                    
                                                                       
                                           
                                                                           
                                                          
                                                           
                                                   
                                                                     
                                            
                                                         
    
                                                                                                    
                                                                                                                 
                                                                                                                  
                              
                                                                                                         
      
                
                                 

   
              
                                                              

               
  
                                                                            */
VOS_STATUS WLANHAL_FillTxBd(void *pAdapter, tANI_U8 typeSubtype, void *pDestMacAddr, void *pAddr2,
        tANI_U8* ptid, tANI_U8 disableFrmXtl, void *pTxBd, tANI_U8 txFlag, tANI_U32 timeStamp);

VOS_STATUS WLANHAL_FillFcTxBd(void *pVosGCtx, void *pFcParams, void *pFcTxBd);
/*                                     */
void WLANHAL_SwapFcRxBd(tANI_U8 *pBd);

/*                  */
void WLANHAL_Swap32Bytes(tANI_U8* pData, tANI_U32 size);

/*                   */
void WLANHAL_SwapRxBd(tANI_U8 *pBd);
void WLANHAL_RxAmsduBdFix(void *pVosGCtx,v_PVOID_t _pvBDHeader);

#ifdef WLAN_PERF
tANI_U32 WLANHAL_TxBdFastFwd(void *pAdapter, tANI_U8 *pDestMac, tANI_U8 tid, tANI_U8 unicastDst,  void *pTxBd, tANI_U16);
#endif

VOS_STATUS WLANHAL_EnableUapsdAcParams(void* pVosGCtx, tANI_U8 staIdx, tUapsdInfo *pUapsdInfo);
VOS_STATUS WLANHAL_DisableUapsdAcParams(void* pVosGCtx, tANI_U8 staIdx, tANI_U8 ac);

VOS_STATUS WLANHAL_EnableIdleBdPduInterrupt(void* pVosGCtx, tANI_U8 idleBdPduThreshold);

#ifdef FEATURE_ON_CHIP_REORDERING
tANI_U8 WLANHAL_IsOnChipReorderingEnabledForTID(void* pVosGCtx, tANI_U8 staIdx, tANI_U8 tid);
#endif

#ifdef WLAN_SOFTAP_VSTA_FEATURE
v_BOOL_t WLANHAL_IsHwFrameTxTranslationCapable(v_PVOID_t pVosGCtx, tANI_U8 staIdx);
#endif

#define tHalRxBd    halRxBd_type
#define tpHalRxBd    phalRxBd_type

#define tHalTxBd    halTxBd_type
#define tpHalTxBd    pHalTxBd_type

#ifdef BA_PARAM_STRUCTURE
#else
#define BA_PARAM_STRUCTURE
//
//           
//                                                 
//                                                   
//

//
//                                                  
//                             
//
typedef struct sBAParams
{

  //                                                     
  //                          
  tANI_U16 baSessionID;

  //                                            
  tANI_U8 baTID;

  //                                                                                                                
  tANI_U8 baBufferSize;

  tANI_U16 SSN;
  tANI_U8 winSize;
  tANI_U8 STAID;

} tBAParams, *tpBAParams;

//
//          
//                                      
//
typedef struct sAddBARsp
{
  //             
  tANI_U16 mesgType;

  //               
  tANI_U16 mesgLen;

  //             
  tANI_U16 baSessionID;

  tANI_U16 replyWinSize;
}tAddBARsp, *tpAddBARsp;

//
//          
//                                  
//                      
//                                                   
//
typedef struct sAddBAInd
{

  //             
  tANI_U16 mesgType;

  //               
  tANI_U16 mesgLen;

  tBAParams baSession;

} tAddBAInd, *tpAddBAInd;

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
typedef struct sDelBAInd
{
  tANI_U8 staIdx;

  tANI_U8 baTID;
  //             
  tANI_U16 mesgType;

  //               
  tANI_U16 mesgLen;

} tDelBAInd, *tpDelBAInd;
#endif

/*                                               
  
                          
  
                                                 
 */
//
//           
//                                        
//
typedef struct sFlushACReq
{
    //             
    tANI_U16 mesgType;

    //               
    tANI_U16 mesgLen;

    //                                   
    tANI_U8  ucSTAId;

    //                                                   
    tANI_U8   ucTid;

} tFlushACReq, *tpFlushACReq;

//
//
//           
//                                        
//
typedef struct sFlushACRsp
{
    //             
    tANI_U16 mesgType;

    //               
    tANI_U16 mesgLen;

    //                                   
    tANI_U8  ucSTAId;

    //                                                   
    tANI_U8   ucTid;

    //                               
    tANI_U8 status;
} tFlushACRsp, *tpFlushACRsp;

#endif


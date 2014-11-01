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

/*===========================================================================

                       W L A N _ Q C T _ W D I _ D P. C

  OVERVIEW:

  This software unit holds the implementation of the WLAN Device Abstraction     
  Layer Internal Utility routines to be used by the Data Path.

  The functions externalized by this module are to be only by the WDI data
  path.
 
  The module leveraged as much as functionality as was possible from the HAL
  in Libra/Volans.
 
  DEPENDENCIES:

  Are listed for each API below.


  Copyright (c) 2010 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                             


                                      
                                                                             
                                    

                                                                           */

#include "wlan_qct_pal_api.h"
#include "wlan_qct_pal_type.h"
#include "wlan_qct_wdi.h"
#include "wlan_qct_wdi_i.h"
#include "wlan_qct_wdi_sta.h"
#include "wlan_qct_wdi_dp.h"
#include "wlan_qct_wdi_bd.h"
#include "wlan_qct_pal_trace.h"

#include "wlan_qct_dev_defs.h"
#define MAC_ADDR_ARRAY(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MAC_ADDRESS_STR "%02x:%02x:%02x:%02x:%02x:%02x"

extern uint8 WDA_IsWcnssWlanCompiledVersionGreaterThanOrEqual(uint8 major, uint8 minor, uint8 version, uint8 revision);
extern uint8 WDA_IsWcnssWlanReportedVersionGreaterThanOrEqual(uint8 major, uint8 minor, uint8 version, uint8 revision);


/*                                                                            
                                         
                                                                            */

/*                                                                           
                       
                                                                           */
static wpt_uint8 btqmQosTid2QidMapping[] = 
{ 
    BTQM_QID0, 
    BTQM_QID1, 
    BTQM_QID2, 
    BTQM_QID3, 
    BTQM_QID4, 
    BTQM_QID5, 
    BTQM_QID6, 
    BTQM_QID7 
};
/*                                                                           
                                           
                                                                            */

/* 
                                                                 
                                   
  
                                                            
  
                         
*/
WDI_Status 
WDI_DP_UtilsInit
(
  WDI_ControlBlockType*  pWDICtx
)
{
  WDI_RxBdType*  pAmsduRxBdFixMask; 

    //                                   
  pWDICtx->ucDpuRF = BMUWQ_BTQM_TX_MGMT; 

#ifdef WLAN_PERF
  pWDICtx->uBdSigSerialNum = 0;
#endif

  pAmsduRxBdFixMask = &pWDICtx->wdiRxAmsduBdFixMask;

  wpalMemoryFill(pAmsduRxBdFixMask,sizeof(WDI_RxBdType), 0xff);

  pAmsduRxBdFixMask->penultimatePduIdx = 0;
  pAmsduRxBdFixMask->headPduIdx        = 0;
  pAmsduRxBdFixMask->tailPduIdx        = 0;
  pAmsduRxBdFixMask->mpduHeaderLength  = 0;
  pAmsduRxBdFixMask->mpduHeaderOffset  = 0;
  pAmsduRxBdFixMask->mpduDataOffset    = 0;
  pAmsduRxBdFixMask->pduCount          = 0;
  pAmsduRxBdFixMask->mpduLength        = 0;
  pAmsduRxBdFixMask->asf               = 0;
  pAmsduRxBdFixMask->esf               = 0;
  pAmsduRxBdFixMask->lsf               = 0;
  pAmsduRxBdFixMask->processOrder      = 0;
  pAmsduRxBdFixMask->sybFrameIdx       = 0;
  pAmsduRxBdFixMask->totalMsduSize     = 0;
  pAmsduRxBdFixMask->aduFeedback       = 0;

  return WDI_STATUS_SUCCESS;
}/*                */


/* 
                                                            
                                   
  
                                                            
  
                         
*/
WDI_Status
WDI_DP_UtilsExit
( 
    WDI_ControlBlockType*  pWDICtx
)
{
   return WDI_STATUS_SUCCESS;
}/*                */

/* 
                                                    
  
                                      
                                        
  
               
*/
WPT_STATIC WPT_INLINE void 
WDI_SwapBytes
(
    wpt_uint8 *pBd, 
    wpt_uint32 nbSwap
)
{
  wpt_uint32 *pU32;
  wpt_uint32 nU32;
  wpt_uint32 wc;

  nU32 = (((nbSwap) + 3)>>2);

  pU32 = (wpt_uint32 *)pBd;
  for ( wc = 0; wc < nU32; wc++ )
  {
    pU32[ wc ] = WPAL_BE32_TO_CPU( pU32[ wc ] );
  }
}/*             */

/* 
                                                                 
           
 
                    
                        
  
     
                                    
*/
WDI_Status 
WDI_BmuGetQidForQOSTid
(
    wpt_uint8   ucTid, 
    wpt_uint8*  pQid
)
{
    if (ucTid > BTQM_QUEUE_TX_TID_7 )
        return WDI_STATUS_E_FAILURE;
        
    *pQid = btqmQosTid2QidMapping[ucTid];
    return WDI_STATUS_SUCCESS;
}/*                      */

#ifdef WLAN_PERF

/* 
                                                            
  
                                                           
 
                                           
    
                                      

                                         
  
                        
*/
static wpt_uint32 
WDI_ComputeTxBdSignature
(  
    WDI_ControlBlockType*  pWDICtx,
    wpt_uint8*             pDestMac, 
    wpt_uint8              ucTid, 
    wpt_uint8              ucUnicastDst
)
{
    wpt_uint16 *pMacU16 = (wpt_uint16 *) pDestMac;

    return ((pMacU16[0] ^ pMacU16[1] ^ pMacU16[2])<< WDI_TXBD_SIG_MACADDR_HASH_OFFSET |
        pWDICtx->uBdSigSerialNum << WDI_TXBD_SIG_SERIAL_OFFSET | 
        ucTid << WDI_TXBD_SIG_TID_OFFSET |
        ucUnicastDst << WDI_TXBD_SIG_UCAST_DATA_OFFSET);
}/*                        */


/* 
                                                           
                  
  
                                      
                                   
                                     
                                         
                                               
                            
  
                                                        
*/
wpt_uint32 
WDI_TxBdFastFwd
(
    WDI_ControlBlockType*  pWDICtx,
    wpt_uint8*             pDestMac, 
    wpt_uint8              ucTid, 
    wpt_uint8              ucUnicastDst, 
    void*                  pTxBd, 
    wpt_uint16             usMpduLength 
 )
{
    WDI_TxBdType*     pBd      = (WDI_TxBdType*) pTxBd;
    wpt_uint32        uRetval  = 0;
#ifdef WPT_LITTLE_BYTE_ENDIAN
    wpt_uint16        usSwapped; 
    wpt_uint16*       pU16     = (wpt_uint16 *) pTxBd;
#endif

    /*                                                                        */

    if( pBd->txBdSignature ==  
        WDI_ComputeTxBdSignature(pWDICtx, pDestMac, ucTid, ucUnicastDst))
    {

#ifdef WPT_LITTLE_BYTE_ENDIAN
       /*                                                                             */
       usSwapped = wpt_cpu_to_be16(usMpduLength); 
       pU16[8]   = usSwapped;
#else
        /*                                                            */
       //                                               
       /*                                                       */
       pBd->mpduLength = usMpduLength;
#endif
       uRetval = 1;
    }
    return uRetval ;
}/*               */

#endif /*         */

/*                                                                           
                                          
                                                                            */

/* 
                                                           

                                                

                                                                     
                                
 
                                                           
                    
 
                                                   
 
                                                        
                                         
 
                                                               
           
 
                                                               
                         
 
                                                                
                     
 
                                                              
                      
 
         

      
                                                     
 
                                                       

                                           
    
                                           

                                                     
                                                               
                                                         
                                                 

                                   

                                                 

                                                                
                                                         

                                                                           
   
          
                                                              
 
*/

WDI_Status
WDI_FillTxBd
(
    WDI_ControlBlockType*  pWDICtx, 
    wpt_uint8              ucTypeSubtype, 
    void*                  pDestMacAddr,
    void*                  pAddr2,
    wpt_uint8*             pTid, 
    wpt_uint8              ucDisableFrmXtl, 
    void*                  pTxBd, 
    wpt_uint8              ucTxFlag, 
    wpt_uint8              ucProtMgmtFrame,
    wpt_uint32             uTimeStamp,
    wpt_uint8              isEapol,
    wpt_uint8*             staIndex
)
{
    wpt_uint8              ucTid        = *pTid; 
    WDI_TxBdType*          pBd          = (WDI_TxBdType*) pTxBd;
    WDI_Status             wdiStatus    = WDI_STATUS_SUCCESS;
    wpt_uint8              ucUnicastDst = 0;
    wpt_uint8              ucType       = 0;
    wpt_uint8              ucSubType    = 0;
    wpt_uint8              ucIsRMF      = 0;
    WDI_BSSSessionType*    pBSSSes;
    wpt_uint8              ucSTAType   = 0;
#ifdef WLAN_PERF
    wpt_uint32      uTxBdSignature = pBd->txBdSignature;
#endif
    tANI_U8                 useStaRateForBcastFrames = 0;
    /*                                                                       */

    /*                                                                        
                                               
                                                                            */
    ucType = (ucTypeSubtype & WDI_FRAME_TYPE_MASK) >> WDI_FRAME_TYPE_OFFSET;
    ucSubType = (ucTypeSubtype & WDI_FRAME_SUBTYPE_MASK);

    WPAL_TRACE( eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_WARN, 
               "Type: %d/%d, MAC S: %08x. MAC D: %08x., Tid=%d, frmXlat=%d, pTxBD=%08x ucTxFlag 0x%X\n", 
                ucType, ucSubType, 
                *((wpt_uint32 *) pAddr2), 
               *((wpt_uint32 *) pDestMacAddr), 
                ucTid, 
               !ucDisableFrmXtl, pTxBd, ucTxFlag );


    //                                                                                                                      
    //                                                                                                                                      
    //                                                                                    
    {    
        if (WDA_IsWcnssWlanCompiledVersionGreaterThanOrEqual(0, 0, 2, 0) &&
            WDA_IsWcnssWlanReportedVersionGreaterThanOrEqual(0, 0, 2, 0))
            useStaRateForBcastFrames = 1;
    }


    /*                                                                       
                               
                                        
                                                                       
                                       
                                                             
                                         
                                                                                 
                                                             
                                                                              
                             
                                                                               
                              
                                                                              
                                  
                                                                            */
    pBd->bdt   = HWBD_TYPE_GENERIC; 

    //                                                                                       
    //                                                 
    if(ucTxFlag & WDI_TRIGGER_ENABLED_AC_MASK) {
        pBd->dpuRF = pWDICtx->ucDpuRF; 
    } else 
    {
        pBd->dpuRF = BMUWQ_BTQM_TX_MGMT; 
    }


    pBd->tid           = ucTid; 
    //                                                                     
    //                 
    pBd->reserved4 = 0;
    pBd->fwTxComplete0 = 0;

    /*                                                       */
    pBd->txComplete1   = (ucTxFlag & WDI_TXCOMP_REQUESTED_MASK) ? 1 : 0; 

    pBd->ap    = WDI_ACKPOLICY_ACK_REQUIRED; 
    pBd->dpuNE = WDI_NO_ENCRYPTION_DISABLED;  

    ucUnicastDst = !(((wpt_uint8 *)pDestMacAddr)[0] & 0x01);    
    *((wpt_uint32 *)pBd + WDI_DPU_FEEDBACK_OFFSET) = 0;

    if(!ucUnicastDst)
    {
      pBd->ap    = WDI_ACKPOLICY_ACK_NOTREQUIRED; 
    }

    if (ucType == WDI_MAC_DATA_FRAME)
    {

        /*                                                                 
                                                                              
                                                             
                                                                            
                                                              
 
                                                        
                                                                         
                                                                         
                                            
                                                                          
                          
         */

        if ((ucSubType & WDI_MAC_DATA_QOS_DATA)&&
            (ucSubType != WDI_MAC_DATA_QOS_NULL))
        {
            pBd->bd_ssn = WDI_TXBD_BD_SSN_FILL_DPU_QOS;
        }
        else
        {
            pBd->bd_ssn = WDI_TXBD_BD_SSN_FILL_DPU_NON_QOS;
        }

        /*                        
                                                                   
                                                           
                                                                      
                                                       
                                                                    
                   
                                                       
 
                                                                            
                                                                              
                                                                              
                                                                
         */

        //                                                                         
        //                                    
        //                                              


        if (useStaRateForBcastFrames)
        {
            pBd->bdRate = WDI_TXBD_BDRATE_DEFAULT;
        }
        else
        {
            pBd->bdRate = (ucUnicastDst)? WDI_TXBD_BDRATE_DEFAULT : WDI_BDRATE_BCDATA_FRAME;
        }
#ifdef FEATURE_WLAN_TDLS
        if ( ucTxFlag & WDI_USE_BD_RATE2_FOR_MANAGEMENT_FRAME)
        {
           pBd->bdRate = WDI_BDRATE_CTRL_FRAME;
        }
#endif
        pBd->rmf    = WDI_RMF_DISABLED;     

        /*                                                                   */
        if( WDI_MAC_DATA_NULL == (ucSubType & ~WDI_MAC_DATA_QOS_DATA))
        {
            ucDisableFrmXtl = 1;
            if (ucTxFlag & WDI_TXCOMP_REQUESTED_MASK) 
            {
                /*                                   */
                pBd->dpuRF = BMUWQ_FW_TRANSMIT; 
            }
            else
            {
#ifdef LIBRA_WAPI_SUPPORT
                if (ucTxFlag & WDI_WAPI_STA_MASK)
                {
                    pBd->dpuRF = BMUWQ_WAPI_DPU_TX;
                    /*                                            */
                    pBd->dpuNE = WDI_NO_ENCRYPTION_ENABLED;
                }
#endif
            }
         }
#if defined(WLAN_PERF) || defined(FEATURE_WLAN_WAPI) || defined(LIBRA_WAPI_SUPPORT)
        //                        
        else
        {
#if defined(FEATURE_WLAN_WAPI)
            //                                                                           
            if( (ucTxFlag & WDI_TX_NO_ENCRYPTION_MASK) )
            {
                pBd->dpuNE = WDI_NO_ENCRYPTION_ENABLED;
            }
#endif //                          
#ifdef LIBRA_WAPI_SUPPORT
            if (ucTxFlag & WDI_WAPI_STA_MASK)
            {
                pBd->dpuRF = BMUWQ_WAPI_DPU_TX;
            }
#endif //                  
#if defined(WLAN_PERF)
    uTxBdSignature = WDI_ComputeTxBdSignature(pWDICtx, pDestMacAddr, ucTid, ucUnicastDst);
#endif //                  
        }
#endif        
    }
    else if (ucType == WDI_MAC_MGMT_FRAME)
    {

        /*                                                                    
                                             
                                                                         
                                       
                                                                                  
                                                                                       
                                                                            
                                                                                    
                                                                  
                                                                             */
         /*                                       */
         /*                                       */
         if( ucSubType ==  WDI_MAC_MGMT_PROBE_REQ )
         {
             pBd->bdRate = WDI_BDRATE_BCMGMT_FRAME;
         }
         else
         {
             if (useStaRateForBcastFrames)
             {
                 pBd->bdRate = (ucUnicastDst)? WDI_BDRATE_BCMGMT_FRAME : WDI_TXBD_BDRATE_DEFAULT;
             }
             else
             {
                 pBd->bdRate = WDI_BDRATE_BCMGMT_FRAME;
             }
         }
         if ( ucTxFlag & WDI_USE_BD_RATE2_FOR_MANAGEMENT_FRAME)
         {
           pBd->bdRate = WDI_BDRATE_CTRL_FRAME;
         }

         pBd->bd_ssn = WDI_TXBD_BD_SSN_FILL_DPU_NON_QOS;
         if((ucSubType == WDI_MAC_MGMT_ACTION) || (ucSubType == WDI_MAC_MGMT_DEAUTH) || 
            (ucSubType == WDI_MAC_MGMT_DISASSOC))
            ucIsRMF = 1;
         ucDisableFrmXtl = 1;
    } 
    else 
    {   //               
        /*                                                                     
                 */
        WDI_ASSERT(0);
    }

    pBd->ub = !ucUnicastDst;

    /*                                                          
                                              
                                                                               
                 
                             
                                                                               
               
                      
     */

    pBd->ft = pWDICtx->bFrameTransEnabled & !ucDisableFrmXtl;

    if( !pBd->ft)
    {
        /*                                                            
                         
                    
                      
                                  
                           
                                                  
         */
    
        WDI_StaStruct*  pSta = (WDI_StaStruct*) pWDICtx->staTable;
        wpt_uint8       ucStaId;

        /*                          */
        pBd->ft = 0;
#ifdef WLAN_PERF
        /*                                 */
        uTxBdSignature = WDI_TXBD_SIG_MGMT_MAGIC; 
#endif
        if((ucTxFlag & WDI_USE_SELF_STA_REQUESTED_MASK) &&
            !(ucIsRMF && ucProtMgmtFrame))
        {
#ifdef HAL_SELF_STA_PER_BSS
            //                                                                           
           wdiStatus = WDI_STATableFindStaidByAddr( pWDICtx, 
                                              *(wpt_macAddr*)pAddr2, &ucStaId );
           if (WDI_STATUS_SUCCESS != wdiStatus) 
           {
                WPAL_TRACE(eWLAN_MODULE_DAL_DATA, eWLAN_PAL_TRACE_LEVEL_ERROR, "WDI_STATableFindStaidByAddr failed");
                WPAL_TRACE(eWLAN_MODULE_DAL_DATA, eWLAN_PAL_TRACE_LEVEL_ERROR, "STA ID = %d " MAC_ADDRESS_STR,
                                        ucStaId, MAC_ADDR_ARRAY(*(wpt_macAddr*)pAddr2));
                return WDI_STATUS_E_NOT_ALLOWED;
           }
#else
            ucStaId = pWDICtx->ucSelfStaId;
#endif
        }
        else
        {
            /*
                                                                                                            
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              
                                                                                                              */
            //                                               
           if( ucUnicastDst ) 
           {
             wdiStatus = WDI_STATableFindStaidByAddr( pWDICtx, 
                 *(wpt_macAddr*)pDestMacAddr, &ucStaId ); 
             //                                             
             //                                                                    
             //                                                                      
             //                                          
             //                                                                   
             //                              
             //                                                                 
             //                         


             if (WDI_STATUS_SUCCESS != wdiStatus) 
             {
               if(ucType == WDI_MAC_MGMT_FRAME)
               {
                 //                                                   
                 //                   
                 wdiStatus = WDI_STATableFindStaidByAddr( pWDICtx, 
                     *(wpt_macAddr*)pAddr2, &ucStaId ); 
               }
               else
               {
                 if( !ucDisableFrmXtl )
                 {
                   //                                                
                   //                                                       
                   //                                             
                   wdiStatus = WDI_STATableFindStaidByAddr( pWDICtx, 
                       *(wpt_macAddr*)pAddr2, &ucStaId ); 
                   if (WDI_STATUS_SUCCESS == wdiStatus)
                   {
                     //                     
                     WDI_StaStruct* pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
                     wpt_uint8                 bssIdx  = 0;

                     pBSSSes = NULL;
                     //                               
                     wdiStatus = WDI_STATUS_E_NOT_ALLOWED;

                     //                             
                     if (pSTATable[ucStaId].bssIdx != WDI_BSS_INVALID_IDX) 
                     {
                       //                                                  
                       //                                              
                       bssIdx = WDI_FindAssocSessionByBSSIdx( pWDICtx,
                           pSTATable[ucStaId].bssIdx,
                           &pBSSSes);

                       if ( NULL != pBSSSes )
                       {
                         //                            
                         wdiStatus = WDI_STATableFindStaidByAddr( pWDICtx, 
                             pBSSSes->macBSSID, &ucStaId ); 
                       }
                     }
                   }
                 }
               }
               //                                                      
               //                         
               if(WDI_STATUS_SUCCESS != wdiStatus )
               {
                 return WDI_STATUS_E_NOT_ALLOWED;
               }
             }
           } 
            else
            {
              //                                             
              wpt_uint8 bssSessIdx;

              //                                                                   
              wdiStatus = WDI_STATableFindStaidByAddr( pWDICtx, 
                                              *(wpt_macAddr*)pAddr2, &ucStaId ); 
              if (WDI_STATUS_SUCCESS != wdiStatus)
              {
                return WDI_STATUS_E_NOT_ALLOWED;
              }

              //                                       
              bssSessIdx = pSta[ucStaId].bssIdx;

              //                                             
              (void) WDI_FindAssocSessionByBSSIdx( pWDICtx, bssSessIdx, 
                                                   &pBSSSes ); 
              if (NULL == pBSSSes)
              {
                //                      
                return WDI_STATUS_E_FAILURE;
              }
              ucStaId = pBSSSes->bcastStaIdx;
           }
         }    

        pBd->staIndex = ucStaId;
        
        *staIndex = ucStaId;

        pSta += ucStaId;  //                                                

        if(ucType == WDI_MAC_MGMT_FRAME)
        {
            if (ucUnicastDst) 
            {
                /*                                            */
                if (ucTxFlag & WDI_USE_NO_ACK_REQUESTED_MASK) 
                {
                    pBd->queueId = BTQM_QUEUE_SELF_STA_BCAST_MGMT;
                }
                else
                {
                    /*                                      */ 
                    pBd->queueId = BTQM_QUEUE_SELF_STA_UCAST_MGMT;
                }
            } 
            else 
            {
                /*                                            */ 
                pBd->queueId = BTQM_QUEUE_SELF_STA_BCAST_MGMT;
            }

            if(ucIsRMF && pSta->rmfEnabled)
            {
                pBd->dpuNE = !ucProtMgmtFrame;
                pBd->rmf = 1;
                if(!ucUnicastDst)
                    pBd->dpuDescIdx = pSta->bcastMgmtDpuIndex; /*      */
                else
                    pBd->dpuDescIdx = pSta->dpuIndex; /*     */
            }
            else
            {
                pBd->dpuNE = WDI_NO_ENCRYPTION_ENABLED;  
                pBd->rmf = 0;
                pBd->dpuDescIdx = pSta->dpuIndex; /*                                  */
            }
        }
        else
        {
            /*             */
            /*                                                                 */
            wpt_uint8 queueId = 0;

   
            WDI_STATableGetStaType(pWDICtx, ucStaId, &ucSTAType);
            if(!ucUnicastDst)
                pBd->queueId = BTQM_QID0;
#ifndef HAL_SELF_STA_PER_BSS
            else if( ucUnicastDst && (ucStaId == pWDICtx->ucSelfStaId))
                pBd->queueId = BTQM_QUEUE_SELF_STA_UCAST_DATA;
#else
            else if( ucUnicastDst && (ucSTAType == WDI_STA_ENTRY_SELF))
                pBd->queueId = BTQM_QUEUE_SELF_STA_UCAST_DATA;
#endif
            else if (pSta->qosEnabled) 
            {
                WDI_BmuGetQidForQOSTid( ucTid, &queueId); 
                pBd->queueId = (wpt_uint32) queueId;
            }
            else
                pBd->queueId = BTQM_QUEUE_TX_nQOS;

            if(ucUnicastDst)
            {
                pBd->dpuDescIdx = pSta->dpuIndex; /*                        */
            }
            else
            {
                pBd->dpuDescIdx = pSta->bcastDpuIndex; /*                       */
            }
        }

        pBd->dpuSignature = pSta->dpuSig;

        /*                             
                                                                                 
                                                                                        
                   
                                                                                       
                                         
                
                                                                                                                                       
                                        
          */
#ifdef WLAN_SOFTAP_VSTA_FEATURE
       //                                                                      
       //                                        
       //                                                                 
       if (IS_VSTA_IDX(ucStaId) ||
                 (
#ifdef FEATURE_WLAN_TDLS
                  (ucSTAType == WDI_STA_ENTRY_TDLS_PEER ) &&
#endif
                  (ucTxFlag & WDI_TRIGGER_ENABLED_AC_MASK)) || isEapol)
       {
           pBd->dpuRF = BMUWQ_FW_DPU_TX;
       }
#endif

    }

    /*                                                                        
                                                                   
                                                                        
                          
                                           
                                                                             */
    WDI_SwapTxBd((wpt_uint8 *)pBd); 

#ifdef WLAN_PERF
    /*                                                                       
                  */
    pBd->txBdSignature = uTxBdSignature ;
#endif        

    return wdiStatus;
}/*            */


/* 
                                                               
                                                           
  
                                                             
                                                       
  
                                                                            
                                              
*/

wpt_uint8 
WDI_RxBD_GetFrameTypeSubType
(
    void* _pvBDHeader, 
    wpt_uint16 usFrmCtrl
)
{
    WDI_RxBdType*    pRxBd = (WDI_RxBdType*) _pvBDHeader;
    wpt_uint8        typeSubType;
    WDI_MacFrameCtl  wdiFrmCtl; 
    
    if (pRxBd->ft != WDI_RX_BD_FT_DONE)
    {
        if (pRxBd->asf)
        {
            typeSubType = (WDI_MAC_DATA_FRAME << WDI_FRAME_TYPE_OFFSET) |
                                                       WDI_MAC_DATA_QOS_DATA;
        } else {
           wpalMemoryCopy(&wdiFrmCtl, &usFrmCtrl, sizeof(wdiFrmCtl)); 
           typeSubType = (wdiFrmCtl.type << WDI_FRAME_TYPE_OFFSET) |
                                                       wdiFrmCtl.subType;
        }
    }
    else
    {
        wpalMemoryCopy(&wdiFrmCtl, &usFrmCtrl, sizeof(wdiFrmCtl)); 
        typeSubType = (wdiFrmCtl.type << WDI_FRAME_TYPE_OFFSET) |
                        wdiFrmCtl.subType;
    }
    
    return typeSubType;
}/*                            */

/* 
                                     

  
                                        
  
             
*/
void 
WDI_SwapRxBd(wpt_uint8 *pBd)
{
#ifndef WDI_BIG_BYTE_ENDIAN
    WDI_SwapBytes(pBd , WDI_RX_BD_HEADER_SIZE);
#endif
}/*            */


/* 
                                      
  
                                         
  
               
*/
void 
WDI_SwapTxBd(wpt_uint8 *pBd)
{
#ifndef WDI_BIG_BYTE_ENDIAN
    WDI_SwapBytes(pBd , WDI_TX_BD_HEADER_SIZE);
#endif
}/*            */

/*                                                */
/* 
                                                      


                                           
                                              

             
*/
void 
WDI_RxAmsduBdFix
(
    WDI_ControlBlockType*  pWDICtx,
    void*                 _pvBDHeader
)
{
    WDI_RxBdType*          pRxBd   = (WDI_RxBdType*) _pvBDHeader;
    wpt_uint32 *pModBd, *pMaskBd, *pFirstBd, i;
    /*                                                                       */

    if (pRxBd->asf)
    {
        if (pRxBd->sybFrameIdx == 0)
        {
            //                          
            pWDICtx->wdiRxAmsduFirstBdCache = *pRxBd;
        }
        else
        {
            pModBd   = (wpt_uint32*)pRxBd;
            pMaskBd  = (wpt_uint32*)&pWDICtx->wdiRxAmsduBdFixMask;
            pFirstBd = (wpt_uint32*)&pWDICtx->wdiRxAmsduFirstBdCache;

            for (i = 0; i < sizeof(WDI_RxBdType)/sizeof(wpt_uint32 *); i++)
            {
                //                                                            
                //                                                         
                pModBd[i] = (pModBd[i] & ~pMaskBd[i]) |
                            (pFirstBd[i] & pMaskBd[i]);
            }
        }
    }
    return;
}/*                */


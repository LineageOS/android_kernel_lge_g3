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

/*===========================================================================


                       W L A N _ Q C T _ T L . C

  OVERVIEW:

  This software unit holds the implementation of the WLAN Transport Layer.

  The functions externalized by this module are to be called ONLY by other
  WLAN modules that properly register with the Transport Layer initially.

  DEPENDENCIES:

  Are listed for each API below.


  Copyright (c) 2008 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                             


                                      
                                                                             
                                                                              
                                                                              
                               
                                                                            
                                                                           
                                                                              
                                                                            
                                  
                                                                           
                                                                        
                                     
                                                               
                                                                             
                                              
                                                                                     
                                                                                 
                                            
                                                                              
                                              
                                                 
                                                                                    
                                              
                                                                  
                                                                          
                                            
                                                                             
                                       
                                                                               
                                      
                                                        
                                      
                                                          
                                                    
                                               
                                                                 
                                                                               
                                                                              
                                     
                                         
                                                 
                                                                        
                                                      
                                          
                                                     
                                                                
                                                               
                                                   
                                                   
                                                                             
                                                  
                                       
                                                 
                                             
                                                        
                                                                             
                                                                           
                                                                                  
                                                             
                                                  
                                                         
                                                        
                                    

                                                                           */

/*                                                                            
                
                                                                            */
#include "wlan_qct_tl.h" 
#include "wlan_qct_wda.h" 
#include "wlan_qct_tli.h" 
#include "wlan_qct_tli_ba.h" 
#include "wlan_qct_tl_hosupport.h"
#include "tlDebug.h"
#ifdef FEATURE_WLAN_WAPI
/*                                */
#include "wlan_qct_wdi_bd.h"
#endif
/*                                */
#define TL_DEBUG
/*                                        */
//                   
//                               
//                  
#ifdef TL_DEBUG_FC
#include <wlan_qct_pal_status.h>
#include <wlan_qct_pal_device.h> //                 
#endif

/*                                                                            
                                         
                                                                            */
/*                */
static v_U8_t WLANTL_LLC_HEADER[] =  {0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00 };

#ifdef FEATURE_WLAN_CCX
/*                         */
static v_U8_t WLANTL_AIRONET_SNAP_HEADER[] =  {0xAA, 0xAA, 0x03, 0x00, 0x40, 0x96, 0x00, 0x00 };
#endif //                

/*                           */
const v_U8_t WLANTL_BT_AMP_OUI[] =  {0x00, 0x19, 0x58 };

#define WLANTL_MAX_SNR_DATA_SAMPLES 20

#ifdef VOLANS_PERF
#define WLANTL_BD_PDU_INTERRUPT_ENABLE_THRESHOLD  120
#define WLANTL_BD_PDU_INTERRUPT_GET_THRESHOLD  120

/*                                         */
int bdPduInterruptEnableThreshold = WLANTL_BD_PDU_INTERRUPT_ENABLE_THRESHOLD;
int bdPduInterruptGetThreshold = WLANTL_BD_PDU_INTERRUPT_GET_THRESHOLD;
#endif /*             */

/*                                    
                                      
                                     */
#define WLANTL_IS_DATA_FRAME(_type_sub)                               \
                     ( WLANTL_DATA_FRAME_TYPE == ( (_type_sub) & 0x30 ))

#define WLANTL_IS_QOS_DATA_FRAME(_type_sub)                                      \
                     (( WLANTL_DATA_FRAME_TYPE == ( (_type_sub) & 0x30 )) &&     \
                      ( WLANTL_80211_DATA_QOS_SUBTYPE == ( (_type_sub) & 0xF ))) 

#define WLANTL_IS_MGMT_FRAME(_type_sub)                                     \
                     ( WLANTL_MGMT_FRAME_TYPE == ( (_type_sub) & 0x30 ))

#define WLANTL_IS_CTRL_FRAME(_type_sub)                                     \
                     ( WLANTL_CTRL_FRAME_TYPE == ( (_type_sub) & 0x30 ))

#ifdef FEATURE_WLAN_TDLS
#define WLANTL_IS_TDLS_FRAME(_eth_type)                                     \
                     ( WLANTL_LLC_TDLS_TYPE == ( _eth_type))
#endif

/*                                                                     */
#define WLANTL_MAX_ALLOWED_LEN    (1514 + 100)

#define WLANTL_MASK_AC  0x03

//                        
//                                                                                      
#define WLANTL_LWM_EGRESS_INGRESS_THRESHOLD (0.75)

//                                                   
#define WLANTL_LWM_INGRESS_SAMPLE_THRESHOLD (64)

//                                             
#define WLANTL_STA_BMU_THRESHOLD_MAX (256)

#define WLANTL_AC_MASK (0x7)
#define WLANTL_STAID_OFFSET (0x6)

/*                         */
#define SWAP_ENDIAN_UINT32(a)          ((a) = ((a) >> 0x18 ) |(((a) & 0xFF0000) >> 0x08) | \
                                            (((a) & 0xFF00) << 0x08)  | (((a) & 0xFF) << 0x18))



/*                                                                            
                    
                                                                            */
#define TL_LITTLE_BIT_ENDIAN

typedef struct
{

#ifndef TL_LITTLE_BIT_ENDIAN

   v_U8_t subType :4;
   v_U8_t type :2;
   v_U8_t protVer :2;

   v_U8_t order :1;
   v_U8_t wep :1;
   v_U8_t moreData :1;
   v_U8_t powerMgmt :1;
   v_U8_t retry :1;
   v_U8_t moreFrag :1;
   v_U8_t fromDS :1;
   v_U8_t toDS :1;

#else

   v_U8_t protVer :2;
   v_U8_t type :2;
   v_U8_t subType :4;

   v_U8_t toDS :1;
   v_U8_t fromDS :1;
   v_U8_t moreFrag :1;
   v_U8_t retry :1;
   v_U8_t powerMgmt :1;
   v_U8_t moreData :1;
   v_U8_t wep :1;
   v_U8_t order :1;

#endif

} WLANTL_MACFCType;

/*               */
typedef struct
{
 /*                     */
 WLANTL_MACFCType  wFrmCtrl;

 /*             */
 v_U16_t  usDurationId;

 /*                  */
 v_U8_t   vA1[VOS_MAC_ADDR_SIZE];

 /*                 */
 v_U8_t   vA2[VOS_MAC_ADDR_SIZE];

 /*                 */
 v_U8_t   vA3[VOS_MAC_ADDR_SIZE];

 /*                        */
 v_U16_t  usSeqCtrl;

 //                                            
#define WLAN80211_MANDATORY_HEADER_SIZE \
    (sizeof(WLANTL_MACFCType) + sizeof(v_U16_t) + \
    (3 * (sizeof(v_U8_t) * VOS_MAC_ADDR_SIZE))  + \
    sizeof(v_U16_t))

 /*                     */
 v_U8_t   optvA4[VOS_MAC_ADDR_SIZE];

 /*                            */
 v_U16_t  usQosCtrl;
}WLANTL_80211HeaderType;

/*              */
typedef struct
{
 /*                           */
 v_U8_t   vDA[VOS_MAC_ADDR_SIZE];

 /*                      */
 v_U8_t   vSA[VOS_MAC_ADDR_SIZE];

 /*              */
 v_U16_t  usLenType;
}WLANTL_8023HeaderType;

/*                                                                            
                          
                                                                            */
#define WLAN_TL_INVALID_U_SIG 255
#define WLAN_TL_INVALID_B_SIG 255
#define ENTER() VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO, "Enter:%s", __func__)

#define WLAN_TL_AC_ARRAY_2_MASK( _pSTA, _ucACMask, i ) \
  do\
  {\
    _ucACMask = 0; \
    for ( i = 0; i < WLANTL_MAX_AC; i++ ) \
    { \
      if ( 0 != (_pSTA)->aucACMask[i] ) \
      { \
        _ucACMask |= ( 1 << i ); \
      } \
    } \
  } while (0);

/*                                                                            
                              
                                                                            */

/*                                                                            
                                               
                                                                            */

static VOS_STATUS 
WLANTL_GetEtherType
(
  v_U8_t               * aucBDHeader,
  vos_pkt_t            * vosDataBuff,
  v_U8_t                 ucMPDUHLen,
  v_U16_t              * usEtherType
);

#ifdef FEATURE_WLAN_TDLS_INTERNAL
/*                                                                                     
                                                                                                          
                                                                      
                                                                  
                                                                                               
                                         
                                                                          
                                                                                                       
*/
static VOS_STATUS 
WLANTL_GetEtherType_2
(
  v_U8_t               * aucBDHeader,
  vos_pkt_t            * vosDataBuff,
  v_U8_t                 ucMPDUHLen,
  v_U16_t              * usEtherType
);
#endif
#ifdef FEATURE_WLAN_WAPI
/*                                                                           
                                                                             
                                                                          
                                                                             */
v_U8_t gUcIsWai;
#endif

/*                                                                            
                                    
                                                                           */

/*                                                                            
                                          
                                                                            */
/*                                                                          

                                         

             
                                                                       
                                                            

              

            

    
                                                           

              

              

                                                                            */
void WLANTL_FreeClientMemory
(WLANTL_STAClientType* pClientSTA[WLAN_MAX_STA_COUNT])
{
    v_U32_t i = 0;
    for(i =0; i < WLAN_MAX_STA_COUNT; i++)
    {
        if( NULL != pClientSTA[i] )
        {
           vos_mem_free(pClientSTA[i]);
        }
        pClientSTA[i] = NULL;
    }
    return;
}

/*                                                                          

                         

             
                                                                      
                                                                      
                           

              

            

      
                                                                       
                                                                   
                                    

              
                                                            

                                                                             
                              
                                              

              

                                                                            */
VOS_STATUS
WLANTL_Open
(
  v_PVOID_t               pvosGCtx,
  WLANTL_ConfigInfoType*  pTLConfig
)
{
  WLANTL_CbType*  pTLCb = NULL; 
  v_U8_t          ucIndex; 
  tHalHandle      smeContext;
  v_U32_t i = 0;
#if defined WLAN_FEATURE_NEIGHBOR_ROAMING
  VOS_STATUS      status = VOS_STATUS_SUCCESS;
#endif
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  ENTER();
  vos_alloc_context( pvosGCtx, VOS_MODULE_ID_TL, 
                    (void*)&pTLCb, sizeof(WLANTL_CbType));

  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if (( NULL == pTLCb ) || ( NULL == pTLConfig ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
               "WLAN TL: Invalid input pointer on WLANTL_Open TL %p Config %p", pTLCb, pTLConfig ));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                    */
  vos_trace_setLevel(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR);

  smeContext = vos_get_context(VOS_MODULE_ID_SME, pvosGCtx);
  if ( NULL == smeContext )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                      "%s: Invalid smeContext", __func__));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                          */
  vos_mem_zero((v_VOID_t *)pTLCb, sizeof(WLANTL_CbType));

  /*                                                                        
                                                    
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:WLANTL_Open"));

  for ( i =0; i<WLAN_MAX_STA_COUNT; i++ )
  {
      if ( i < WLAN_NON32_STA_COUNT )
      {
          pTLCb->atlSTAClients[i] = vos_mem_malloc(sizeof(WLANTL_STAClientType));
          /*                                                                           */
         if ( NULL == pTLCb->atlSTAClients[i] )
         {
             TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR, "WLAN TL: StaClient allocation failed"));
             WLANTL_FreeClientMemory(pTLCb->atlSTAClients);
             vos_free_context(pvosGCtx, VOS_MODULE_ID_TL, pTLCb);
             return VOS_STATUS_E_FAULT;
         }
         vos_mem_zero((v_VOID_t *) pTLCb->atlSTAClients[i], sizeof(WLANTL_STAClientType));
      }
      else
      {
          pTLCb->atlSTAClients[i] = NULL;
      }
  }

  pTLCb->reorderBufferPool = vos_mem_malloc(sizeof(WLANTL_REORDER_BUFFER_T) * WLANTL_MAX_BA_SESSION);
  if (NULL == pTLCb->reorderBufferPool)
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR, "WLAN TL: Reorder buffer allocation failed\n"));
    WLANTL_FreeClientMemory(pTLCb->atlSTAClients);
    vos_free_context(pvosGCtx, VOS_MODULE_ID_TL, pTLCb);
    return VOS_STATUS_E_FAULT;
  }

  vos_mem_zero((v_VOID_t *)pTLCb->reorderBufferPool, sizeof(WLANTL_REORDER_BUFFER_T) * WLANTL_MAX_BA_SESSION);

  WLANTL_CleanCB(pTLCb, 0 /*            */);

  for ( ucIndex = 0; ucIndex < WLANTL_MAX_AC ; ucIndex++)
  {
    pTLCb->tlConfigInfo.ucAcWeights[ucIndex] = pTLConfig->ucAcWeights[ucIndex];
  }

  for ( ucIndex = 0; ucIndex < WLANTL_MAX_AC ; ucIndex++)
  {
    pTLCb->tlConfigInfo.ucReorderAgingTime[ucIndex] = pTLConfig->ucReorderAgingTime[ucIndex];
  }

  //                                                     
  pTLCb->uCurServedAC = WLANTL_AC_BK;
  pTLCb->ucCurLeftWeight = 1;
  pTLCb->ucCurrentSTA = WLAN_MAX_STA_COUNT-1;

#if 0
  //                       
  vos_mem_zero(&pTLCb->tlFCInfo, sizeof(tFcTxParams_type));
  //                                                                             
  pTLCb->tlFCInfo.fcConfig = 0x1;
#endif

  pTLCb->vosTxFCBuf = NULL;
  pTLCb->tlConfigInfo.uMinFramesProcThres =
                pTLConfig->uMinFramesProcThres;

#ifdef FEATURE_WLAN_TDLS
  pTLCb->ucTdlsPeerCount = 0;
#endif

  pTLCb->tlConfigInfo.uDelayedTriggerFrmInt =
                pTLConfig->uDelayedTriggerFrmInt;

  /*                                                                        
                               
                                                                           */
  vos_pkt_get_packet(&pTLCb->vosDummyBuf, VOS_PKT_TYPE_RX_RAW, 1, 1,
                     1/*    */,NULL, NULL);

  WLANTL_InitBAReorderBuffer(pvosGCtx);
#if defined WLAN_FEATURE_NEIGHBOR_ROAMING
   /*                                 
                                                */
  status = WLANTL_HSInit(pvosGCtx);
  if(!VOS_IS_STATUS_SUCCESS(status))
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "Handoff support module init fail"));
    WLANTL_FreeClientMemory(pTLCb->atlSTAClients);
    vos_mem_free(pTLCb->reorderBufferPool);
    vos_free_context(pvosGCtx, VOS_MODULE_ID_TL, pTLCb);
    return status;
  }
#endif

  pTLCb->isBMPS = VOS_FALSE;
  pmcRegisterDeviceStateUpdateInd( smeContext,
                                   WLANTL_PowerStateChangedCB, pvosGCtx );

  return VOS_STATUS_SUCCESS;
}/*             */

/*                                                                          

                          

             
                                                                          
                                                          

              

            

      
                                                                       
                                                                   

              
                                                            

                                                                             
                              
                                              

                                                                         
                 

              

                                                                            */
VOS_STATUS
WLANTL_Start
(
  v_PVOID_t  pvosGCtx
)
{
  WLANTL_CbType*      pTLCb      = NULL;
  v_U32_t             uResCount = WDA_TLI_MIN_RES_DATA;
  VOS_STATUS          vosStatus;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  ENTER();
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_Start"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                               
                                     
                                                                          */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:WLAN TL:WLANTL_Start"));

  vosStatus = WDA_DS_Register( pvosGCtx, 
                          WLANTL_TxComp, 
                          WLANTL_RxFrames,
                          WLANTL_GetFrames, 
                          WLANTL_ResourceCB,
                          WDA_TLI_MIN_RES_DATA, 
                          pvosGCtx, 
                          &uResCount ); 

  if ( VOS_STATUS_SUCCESS != vosStatus )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR, 
               "WLAN TL:TL failed to register with BAL/WDA, Err: %d",
               vosStatus));
    return vosStatus;
  }

  /*                     */
  vos_atomic_set_U8( &pTLCb->ucTxSuspended, 0);

  pTLCb->uResCount = uResCount;
  return VOS_STATUS_SUCCESS;
}/*              */

/*                                                                          

                         

             
                                                                            
                                                                            
                  

              

            

      
                                                                       
                                                                   

              
                                                            

                                                                             
                              
                                              

              

                                                                            */
VOS_STATUS
WLANTL_Stop
(
  v_PVOID_t  pvosGCtx
)
{
  WLANTL_CbType*  pTLCb = NULL;
  v_U8_t      ucIndex;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  ENTER();
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ChangeSTAState"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                  
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:WLANTL_Stop"));

  /*                      */
  vos_atomic_set_U8( &pTLCb->ucTxSuspended, 1);

  if ( NULL != pTLCb->tlMgmtFrmClient.vosPendingDataBuff )
  {
    vos_pkt_return_packet(pTLCb->tlMgmtFrmClient.vosPendingDataBuff);
    pTLCb->tlMgmtFrmClient.vosPendingDataBuff = NULL;
  }

  if ( NULL != pTLCb->tlBAPClient.vosPendingDataBuff )
  {
    vos_pkt_return_packet(pTLCb->tlBAPClient.vosPendingDataBuff);
    pTLCb->tlBAPClient.vosPendingDataBuff = NULL;
  }

#if defined WLAN_FEATURE_NEIGHBOR_ROAMING
  if(VOS_STATUS_SUCCESS != WLANTL_HSStop(pvosGCtx))
  {
    TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
               "Handoff Support module stop fail"));
  }
#endif

  /*                                                                         
                         
                                                                            */
  for ( ucIndex = 0; ucIndex < WLAN_MAX_STA_COUNT; ucIndex++)
  {
    if ( NULL != pTLCb->atlSTAClients[ucIndex] )
    {
        WLANTL_CleanSTA(pTLCb->atlSTAClients[ucIndex], 1 /*                */);
    }
  }


  return VOS_STATUS_SUCCESS;
}/*             */

/*                                                                          

                          

             
                                                                         
                               

              

            

      
                                                                       
                                                                   

              
                                                            

                                                                             
                              
                                              

              

                                                                            */
VOS_STATUS
WLANTL_Close
(
  v_PVOID_t  pvosGCtx
)
{
  WLANTL_CbType*  pTLCb = NULL;
  tHalHandle smeContext;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  ENTER();
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ChangeSTAState"));
    return VOS_STATUS_E_FAULT;
  }
  /*                                                                        
                       
                                                                           */
  smeContext = vos_get_context(VOS_MODULE_ID_SME, pvosGCtx);
  if ( NULL == smeContext )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                      "%s: Invalid smeContext", __func__));
    //                                                    
  }
  else
  {
    pmcDeregisterDeviceStateUpdateInd( smeContext, WLANTL_PowerStateChangedCB );
  }

#if defined WLAN_FEATURE_NEIGHBOR_ROAMING
  if(VOS_STATUS_SUCCESS != WLANTL_HSDeInit(pvosGCtx))
  {
    TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
               "Handoff Support module DeInit fail"));
  }
#endif

  /*                                                                        
                             
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL: WLANTL_Close"));
  WLANTL_CleanCB(pTLCb, 1 /*                               */);

  WLANTL_FreeClientMemory(pTLCb->atlSTAClients);

  vos_mem_free(pTLCb->reorderBufferPool);

  /*                                                                        
                                    
                                                                           */
  vos_free_context(pvosGCtx, VOS_MODULE_ID_TL, pTLCb);
  return VOS_STATUS_SUCCESS;
}/*              */

/*                                                                            
                        
                                                                            */
/*                                                                          

                                                    

             
                                                                               

              

            
     
                          
                                                                         
     

              

        

                                                                            */
void
WLANTL_ConfigureSwFrameTXXlationForAll
(
  v_PVOID_t pvosGCtx, 
  v_BOOL_t enableFrameXlation
)
{
  v_U8_t ucIndex;
  /*                                                                        
                            
                                                                           */
  WLANTL_CbType* pTLCb = VOS_GET_TL_CB(pvosGCtx);
  WLANTL_STAClientType* pClientSTA = NULL;
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
           "WLAN TL:Invalid TL pointer from pvosGCtx on "
           "WLANTL_ConfigureSwFrameTXXlationForAll"));
    return;
  }

  TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
     "WLANTL_ConfigureSwFrameTXXlationForAll: Configure SW frameXlation %d", 
      enableFrameXlation));

  for ( ucIndex = 0; ucIndex < WLAN_MAX_TID; ucIndex++) 
  {
    pClientSTA = pTLCb->atlSTAClients[ucIndex];
    if ( NULL != pClientSTA && 0 != pClientSTA->ucExists )
    {
#ifdef WLAN_SOFTAP_VSTA_FEATURE
      //                                                                
      //                                                              
      //                                                           
      if (!WDA_IsHwFrameTxTranslationCapable(pvosGCtx, ucIndex))
      {
        pClientSTA->wSTADesc.ucSwFrameTXXlation = 1;
      }
      else
#endif
        pClientSTA->wSTADesc.ucSwFrameTXXlation = enableFrameXlation;
    }
  }
}

/*                                                                           

                                    

             

                                                                          
                                                                                 
                                                                

                                                                                          
                                                                                  
                                
    
                                                                                 
    
              

                                                          

   
            

                         

              

                                                            
                                                      

              
                                                                            
                                                                      
                                                                           
                                                                                  

                                                                         
            

                                                                            */

VOS_STATUS 
WLANTL_StartForwarding
(
  v_U8_t ucSTAId,
  v_U8_t ucUcastSig, 
  v_U8_t ucBcastSig 
)
{
  vos_msg_t      sMessage;
  v_U32_t        uData;             
 /*                                                                      */

  /*                                      */
  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
             "Serializing TL Start Forwarding Cached for control STA %d", 
              ucSTAId );

  vos_mem_zero( &sMessage, sizeof(vos_msg_t) );

  uData = ucSTAId | (ucUcastSig << 8 ) | (ucBcastSig << 16); 
  sMessage.bodyptr = (v_PVOID_t)uData;
  sMessage.type    = WLANTL_TX_FWD_CACHED;

  return vos_tx_mq_serialize(VOS_MQ_ID_TL, &sMessage);

} /*                          */

/*                                                                           

                                

             

                                                                          
                                                   

                                                                           
                             

   
              

                                                          

   
            

                         

              

       
   
              
                                                                                 
                                                                                

                                                                             
                                          
   
                                                                            */
void WLANTL_AssocFailed(v_U8_t staId)
{
  //                                                            
  //                                                                  
  //                                               
  //                                                    
  //                                                                                   
  if(!VOS_IS_STATUS_SUCCESS(WLANTL_StartForwarding(staId,0,0)))
  {
    VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
       " %s fails to start forwarding", __func__);
  }
}
  
  /*                                                                           

                             

             
                                                                                   
                                                              
                                                                               
                                                                               

              

                                                           
  
            

                                            
                                          
  
               

                                        
   
              
   
                                                                            */

VOS_STATUS WLANTL_Finish_ULA( void (*callbackRoutine) (void *callbackContext),
                              void *callbackContext)
{
   return WDA_DS_FinishULA( callbackRoutine, callbackContext);
}


/*                                                                           

                                      

             

                                                                          
                                                                      
                                                                          
                    

              

                                                          

                
                                                                       
                                                                      
                                                        

            

                                                                       
                                                                   
                                                                           
                                                                          
                                    
                                                                           
                                                                       
                                 

              

                                                            

                                                    
                                                                            
                                                                       
                                                       
                                              

              

                                                                            */
VOS_STATUS
WLANTL_RegisterSTAClient
(
  v_PVOID_t                 pvosGCtx,
  WLANTL_STARxCBType        pfnSTARx,
  WLANTL_TxCompCBType       pfnSTATxComp,
  WLANTL_STAFetchPktCBType  pfnSTAFetchPkt,
  WLAN_STADescType*         pwSTADescType,
  v_S7_t                    rssi
)
{
  WLANTL_CbType*  pTLCb = NULL;
  WLANTL_STAClientType* pClientSTA = NULL;
  v_U8_t    ucTid = 0;/*                                                                   */
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  ENTER();
  if (( NULL == pwSTADescType ) || ( NULL == pfnSTARx ) ||
      ( NULL == pfnSTAFetchPkt ))
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_RegisterSTAClient"));
    return VOS_STATUS_E_INVAL;
  }

  if ( WLANTL_STA_ID_INVALID( pwSTADescType->ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WLAN TL:Invalid station id requested on WLANTL_RegisterSTAClient"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
      "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_RegisterSTAClient"));
    return VOS_STATUS_E_FAULT;
  }

  //                                                   
  if ( NULL == pTLCb->atlSTAClients[pwSTADescType->ucSTAId] ){
      pTLCb->atlSTAClients[pwSTADescType->ucSTAId] = vos_mem_malloc(sizeof(WLANTL_STAClientType));
      if ( NULL == pTLCb->atlSTAClients[pwSTADescType->ucSTAId] ){
          TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL: STA Client memory allocation failed in WLANTL_RegisterSTAClient"));
          return VOS_STATUS_E_FAILURE;
      }
      TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
          "WLAN TL: STA Client memory allocation in WLANTL_RegisterSTAClient"));
      vos_mem_zero((v_VOID_t *) pTLCb->atlSTAClients[pwSTADescType->ucSTAId],sizeof(WLANTL_STAClientType));
  }

  //                                                                 
  pClientSTA = pTLCb->atlSTAClients[pwSTADescType->ucSTAId];
  if ( 0 != pClientSTA->ucExists )
  {
    pClientSTA->ucExists++;
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
      "WLAN TL:Station was already registered on WLANTL_RegisterSTAClient"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                            
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Registering STA Client ID: %d", pwSTADescType->ucSTAId ));

  pClientSTA->pfnSTARx       = pfnSTARx;
  pClientSTA->pfnSTAFetchPkt = pfnSTAFetchPkt;

  /*                                                                  
                           */
  if ( NULL != pfnSTATxComp )
  {
    pClientSTA->pfnSTATxComp   = pfnSTATxComp;
  }

  pClientSTA->tlState  = WLANTL_STA_INIT;
  pClientSTA->tlPri    = WLANTL_STA_PRI_NORMAL;
  pClientSTA->wSTADesc.ucSTAId  = pwSTADescType->ucSTAId;
  pClientSTA->ptkInstalled = 0;

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Registering STA Client ID: %d with UC %d and BC %d", 
             pwSTADescType->ucSTAId, 
              pwSTADescType->ucUcastSig, pwSTADescType->ucBcastSig));

  pClientSTA->wSTADesc.wSTAType = pwSTADescType->wSTAType;

  pClientSTA->wSTADesc.ucQosEnabled = pwSTADescType->ucQosEnabled;

  pClientSTA->wSTADesc.ucAddRmvLLC = pwSTADescType->ucAddRmvLLC;

  pClientSTA->wSTADesc.ucProtectedFrame = pwSTADescType->ucProtectedFrame;

#ifdef FEATURE_WLAN_CCX
  pClientSTA->wSTADesc.ucIsCcxSta = pwSTADescType->ucIsCcxSta;

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Registering STA Client ID: %d QoS %d Add LLC %d ProtFrame %d CcxSta %d", 
             pwSTADescType->ucSTAId, 
             pwSTADescType->ucQosEnabled,
             pwSTADescType->ucAddRmvLLC,
             pwSTADescType->ucProtectedFrame,
             pwSTADescType->ucIsCcxSta));
#else

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Registering STA Client ID: %d QoS %d Add LLC %d ProtFrame %d", 
             pwSTADescType->ucSTAId, 
             pwSTADescType->ucQosEnabled,
             pwSTADescType->ucAddRmvLLC,
             pwSTADescType->ucProtectedFrame));

#endif //                
#ifdef WLAN_SOFTAP_VSTA_FEATURE
  //                                                                
  //                                                              
  //                                                           

  if (!WDA_IsHwFrameTxTranslationCapable(pvosGCtx, pwSTADescType->ucSTAId)
      || ( WLAN_STA_BT_AMP == pwSTADescType->wSTAType))
  {
      pwSTADescType->ucSwFrameTXXlation = 1;
  }
#endif

  pClientSTA->wSTADesc.ucSwFrameTXXlation = pwSTADescType->ucSwFrameTXXlation;
  pClientSTA->wSTADesc.ucSwFrameRXXlation = pwSTADescType->ucSwFrameRXXlation;

#ifdef FEATURE_WLAN_WAPI
  pClientSTA->wSTADesc.ucIsWapiSta = pwSTADescType->ucIsWapiSta;
#endif /*                   */

  vos_copy_macaddr( &pClientSTA->wSTADesc.vSTAMACAddress, &pwSTADescType->vSTAMACAddress);

  vos_copy_macaddr( &pClientSTA->wSTADesc.vBSSIDforIBSS, &pwSTADescType->vBSSIDforIBSS);

  vos_copy_macaddr( &pClientSTA->wSTADesc.vSelfMACAddress, &pwSTADescType->vSelfMACAddress);

  /*                                                */
  pClientSTA->ucIsReplayCheckValid = pwSTADescType->ucIsReplayCheckValid;
  pClientSTA->ulTotalReplayPacketsDetected =  0;
/*                                            */
  for(ucTid = 0; ucTid < WLANTL_MAX_TID ; ucTid++)
  {
    pClientSTA->ullReplayCounter[ucTid] =  0;
  }

  /*                                                                    
                                                                      
                                                                         
                                                                   
                                         
                                                                        */
  pClientSTA->ucCurrentAC     = WLANTL_AC_VO;
  pClientSTA->ucCurrentWeight = 0;
  pClientSTA->ucServicedAC    = WLANTL_AC_BK;
  pClientSTA->ucEapolPktPending = 0;

  vos_mem_zero( pClientSTA->aucACMask, sizeof(pClientSTA->aucACMask));

  vos_mem_zero( &pClientSTA->wUAPSDInfo, sizeof(pClientSTA->wUAPSDInfo));

  /*                                                                    
                                            
                                                                        */
  vos_mem_zero( pClientSTA->atlBAReorderInfo,
     sizeof(pClientSTA->atlBAReorderInfo[0])*
     WLAN_MAX_TID);

  vos_mem_zero( pClientSTA->aucMPDUHeader,
                WLANTL_MPDU_HEADER_LEN);

  pClientSTA->ucMPDUHeaderLen   = 0;
  pClientSTA->vosAMSDUChain     = NULL;
  pClientSTA->vosAMSDUChainRoot = NULL;


  /*             
                                                    
                                                     
                                           */
  for (ucTid = 0; ucTid < WLAN_MAX_TID ; ucTid++)
  {
    if (!VOS_IS_STATUS_SUCCESS(
        vos_lock_init(&pClientSTA->atlBAReorderInfo[ucTid].reorderLock)))
    {
       TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "Lock Init Fail"));
       return VOS_STATUS_E_FAILURE;
    }
  }
  /*                                                                    
              
                                                                        */
  vos_mem_zero( pClientSTA->auRxCount,
      sizeof(pClientSTA->auRxCount[0])*
      WLAN_MAX_TID);

  vos_mem_zero( pClientSTA->auTxCount,
      sizeof(pClientSTA->auRxCount[0])*
      WLAN_MAX_TID);
  /*                                                                      
                                                                            
                                                             */
  pClientSTA->rssiAvg = rssi;
#ifdef FEATURE_WLAN_TDLS
  if(WLAN_STA_TDLS == pClientSTA->wSTADesc.wSTAType)
  {
    /*                                            */
    pClientSTA->rssiAlpha = WLANTL_HO_TDLS_ALPHA;
  }
  else
  {
    pClientSTA->rssiAlpha = WLANTL_HO_DEFAULT_ALPHA;
  }
#else
    pClientSTA->rssiAlpha = WLANTL_HO_DEFAULT_ALPHA;
#endif /*                   */

  /*                                             */
  vos_atomic_set_U8(
        &pClientSTA->ucTxSuspended, 0);

  /*                                                  */
  pTLCb->ucRegisteredStaId = pwSTADescType->ucSTAId;

  /*                                          */
  if ( WLAN_STA_BT_AMP == pwSTADescType->wSTAType )
  {
    pTLCb->tlBAPClient.ucBAPSTAId = pwSTADescType->ucSTAId;
  }

  /*                                                                        
                    
                                                                           */
  memset(&pClientSTA->trafficStatistics,
         0, sizeof(WLANTL_TRANSFER_STA_TYPE));


  /*                                                                        
                                                   
                                                                           */
  pClientSTA->tlState = pwSTADescType->ucInitState;
  pClientSTA->ucRxBlocked = 1;
  /*                                                                       
                                                                  
                                                                          */
  pClientSTA->ucExists++;

  //                        
  pClientSTA->ucLwmModeEnabled = FALSE;
  pClientSTA->ucLwmEventReported = FALSE;
  pClientSTA->bmuMemConsumed = 0;
  pClientSTA->uIngress_length = 0;
  pClientSTA->uBuffThresholdMax = WLANTL_STA_BMU_THRESHOLD_MAX;

  pClientSTA->uLwmThreshold = WLANTL_STA_BMU_THRESHOLD_MAX / 3;

  //                                                        
  if ( WLAN_STA_SOFTAP != pwSTADescType->wSTAType )
  { 
    /*                                                                        
                                                               
                                                                             */
    if(!VOS_IS_STATUS_SUCCESS(WLANTL_StartForwarding( pwSTADescType->ucSTAId, 
                              pwSTADescType->ucUcastSig, 
                              pwSTADescType->ucBcastSig)))
    {
      VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         " %s fails to start forwarding", __func__);
    }
#ifdef FEATURE_WLAN_TDLS
    if( WLAN_STA_TDLS == pwSTADescType->wSTAType )
        pTLCb->ucTdlsPeerCount++;
#endif
  }
  return VOS_STATUS_SUCCESS;
}/*                          */

/*                                                                           

                                   

             

                                                                        
                       

              

                                                                        
           

            

                                                                       
                                                                   
                                                        

              

                                                            

                                                                            
                                                                       
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_ClearSTAClient
(
  v_PVOID_t         pvosGCtx,
  v_U8_t            ucSTAId
)
{
  WLANTL_CbType*  pTLCb = NULL; 
  v_U8_t  ucIndex;
  /*                                                                       */

  ENTER();
  /*                                                                        
                
                                                                           */
  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
           "WLAN TL:Invalid station id requested on WLANTL_ClearSTAClient"));
    return VOS_STATUS_E_FAULT;
  }
  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
       "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ClearSTAClient"));
    return VOS_STATUS_E_FAULT;
  }

  if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pTLCb->atlSTAClients[ucSTAId]->ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL:Station was not previously registered on WLANTL_ClearSTAClient"));
    /*                                  */
    WLANTL_StartForwarding(ucSTAId,0,0);
    return VOS_STATUS_E_EXISTS;
  }

  /*                                 */
  for (ucIndex = 0; ucIndex < WLAN_MAX_TID ; ucIndex++)
  {
     WLANTL_BaSessionDel(pvosGCtx, ucSTAId, ucIndex);
     vos_lock_destroy(&pTLCb->atlSTAClients[ucSTAId]->atlBAReorderInfo[ucIndex].reorderLock);
  }

#ifdef FEATURE_WLAN_TDLS
  /*                                                  */  
  if(WLAN_STA_TDLS == pTLCb->atlSTAClients[ucSTAId]->wSTADesc.wSTAType
      && pTLCb->ucTdlsPeerCount)
      pTLCb->ucTdlsPeerCount--;
#endif

  /*                                                                        
                 
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Clearing STA Client ID: %d", ucSTAId ));
  WLANTL_CleanSTA(pTLCb->atlSTAClients[ucSTAId], 1 /*             */);

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Clearing STA Reset History RSSI and Region number"));
  pTLCb->hoSupport.currentHOState.historyRSSI = 0;
  pTLCb->hoSupport.currentHOState.regionNumber = 0;

  return VOS_STATUS_SUCCESS;
}/*                       */

/*                                                                           

                                   

             

                                                                   
                                           

              

                                                                      
           

                                                                        
                                                                          
                                                                        

                                                                            
                  
                                                                            
                

            

                                                                       
                                                                   
                                                                       
                                                                       


              

                                                            

                                                     
                                                                             
                                                                        
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_ChangeSTAState
(
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId,
  WLANTL_STAStateType   tlSTAState
)
{
  WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( tlSTAState >= WLANTL_STA_MAX_STATE )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid parameter sent on WLANTL_ChangeSTAState"));
    return VOS_STATUS_E_INVAL;
  }

  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
             "WLAN TL:Invalid station id requested on WLANTL_ChangeSTAState"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ChangeSTAState"));
    return VOS_STATUS_E_FAULT;
  }

  if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pTLCb->atlSTAClients[ucSTAId]->ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
     "WLAN TL:Station was not previously registered on WLANTL_ChangeSTAState"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                    
                                                          
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Changing state for STA Client ID: %d from %d to %d",
             ucSTAId, pTLCb->atlSTAClients[ucSTAId]->tlState, tlSTAState));

  pTLCb->atlSTAClients[ucSTAId]->tlState = tlSTAState;

  return VOS_STATUS_SUCCESS;
}/*                       */

/*                                                                           

                                    

             

                                                                         

              

                                                                      
           

            

                                                                       
                                                                   
                                                                    
                             

              

                                                            

                                                                             
                                                                        
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_STAPtkInstalled
(
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId
)
{
  WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                                                                           */

  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
             "WLAN TL:Invalid station id requested on WLANTL_ChangeSTAState"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         FL("WLAN TL:Invalid TL pointer from pvosGCtx")));
    return VOS_STATUS_E_FAULT;
  }

  if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          FL("WLAN TL:Client Memory was not allocated")));
      return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pTLCb->atlSTAClients[ucSTAId]->ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
     FL("WLAN TL:Station was not previously registered")));
    return VOS_STATUS_E_EXISTS;
  }

  pTLCb->atlSTAClients[ucSTAId]->ptkInstalled = 1;

  return VOS_STATUS_SUCCESS;
}/*                        */

/*                                                                           

                                

             

                                                   

              

                                                                          


            

      
                                                                       
                                                                   
                                             

       
                                                                            


              

                                                            

                                                     
                                                                             
                                                                        
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_GetSTAState
(
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId,
  WLANTL_STAStateType   *ptlSTAState
)
{
  WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == ptlSTAState )
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid parameter sent on WLANTL_GetSTAState"));
    return VOS_STATUS_E_INVAL;
  }

  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
             "WLAN TL:Invalid station id requested on WLANTL_GetSTAState"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_GetSTAState"));
    return VOS_STATUS_E_FAULT;
  }

  if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
  {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Client Memory was not allocated on %s", __func__));
        return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pTLCb->atlSTAClients[ucSTAId]->ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
     "WLAN TL:Station was not previously registered on WLANTL_GetSTAState"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                 
                                                                           */
  *ptlSTAState = pTLCb->atlSTAClients[ucSTAId]->tlState;

  return VOS_STATUS_SUCCESS;
}/*                    */

/*                                                                          
                                         

             
                                                                

              
                                                                             

            

      
                                                                    
                                                                            
      
                                                        
      
                                       

              
                                                              

                                                       
                                                                               
                                                                          
                                                     
                                                

                
                                                                            */


VOS_STATUS
WLANTL_UpdateSTABssIdforIBSS
(
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId,
  v_U8_t               *pBssid
)
{
  WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
             "WLAN TL:Invalid station id requested %s", __func__));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WLAN TL:Invalid TL pointer from pvosGCtx %s", __func__));
    return VOS_STATUS_E_FAULT;
  }

  if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
  {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Client Memory was not allocated on %s", __func__));
        return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pTLCb->atlSTAClients[ucSTAId]->ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
     "WLAN TL:Station was not previously registered %s", __func__));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                         
                                                                           */
  vos_mem_copy( &pTLCb->atlSTAClients[ucSTAId]->wSTADesc.vBSSIDforIBSS,
                                     pBssid, sizeof(v_MACADDR_t));

  return VOS_STATUS_SUCCESS;
}

/*                                                                           

                                  

             

                                                                       
           

              

                                                                 
                             

                                                                    
                                                                   
                                                                    
                                                                
                                           

            

                                                                    
                                                                
                                                                    

              

                                                            

                                                     
                                                                          
                                                                           
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_STAPktPending
(
  v_PVOID_t            pvosGCtx,
  v_U8_t               ucSTAId,
  WLANTL_ACEnumType    ucAc
)
{
  WLANTL_CbType*  pTLCb = NULL;
  WLANTL_STAClientType* pClientSTA = NULL;
  /*                                                                       */

  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
      "WLAN TL:Packet pending indication for STA: %d AC: %d", ucSTAId, ucAc);

  /*                                                                        
                
                                                                           */
  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid station id requested on WLANTL_STAPktPending"));
    return VOS_STATUS_E_FAULT;
  }
  /*                                                                        
                                                
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_STAPktPending"));
    return VOS_STATUS_E_FAULT;
  }

  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pClientSTA )
  {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Client Memory was not allocated on %s", __func__));
        return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pClientSTA->ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL:Station was not previously registered on WLANTL_STAPktPending"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                     
                                                                        
                                                                     
                                                                      
                  
                                                                        */
  pTLCb->ucRegisteredStaId = ucSTAId;

  if( WLANTL_STA_CONNECTED == pClientSTA->tlState )
  { /*                                                                     
                                                      
                                                                  
                                                   
                                                                              */
    pClientSTA->ucEapolPktPending = 1;

    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
      "WLAN TL:Packet pending indication for STA: %d AC: %d State: %d", 
               ucSTAId, ucAc, pClientSTA->tlState);
  }

  /*                                                                       
                                                                       
                            
                                                                      
                                           
                                                                         */

      pClientSTA->aucACMask[ucAc] = 1;

      vos_atomic_set_U8( &pClientSTA->ucPktPending, 1);

      /*                                                                        
                                                                          
                  
                                                                                */
       if (( pTLCb->uResCount >=  WDA_TLI_MIN_RES_DATA ) &&
          ( 0 == pTLCb->ucTxSuspended ))
      {
        TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
              "Issuing Xmit start request to BAL"));
           WDA_DS_StartXmit(pvosGCtx);
      }
      else
      {
        /*                                                                     
                                                                         
                                                       
                                                                               */
        VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
              "WLAN TL:Request to send but condition not met. Res: %d,Suspend: %d",
              pTLCb->uResCount, pTLCb->ucTxSuspended );
      }
  return VOS_STATUS_SUCCESS;
}/*                      */

/*                                                                          

                                   

             

                                                                      
                                                                         
                

              

                                                                            
           

            

                                                                       
                                                                   
                                                                      

              

                                                            

                                                     
                                                                          
                                                                           
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_SetSTAPriority
(
  v_PVOID_t            pvosGCtx,
  v_U8_t               ucSTAId,
  WLANTL_STAPriorityType   tlSTAPri
)
{
  WLANTL_CbType*  pTLCb = NULL;
  WLANTL_STAClientType* pClientSTA = NULL;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Invalid station id requested on WLANTL_SetSTAPriority"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
        "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_SetSTAPriority"));
    return VOS_STATUS_E_FAULT;
  }

  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pClientSTA )
  {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Client Memory was not allocated on %s", __func__));
        return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pClientSTA->ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL:Station was not previously registered on WLANTL_SetSTAPriority"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                                                              
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Changing state for STA Pri ID: %d from %d to %d",
             ucSTAId, pClientSTA->tlPri, tlSTAPri));
  pClientSTA->tlPri = tlSTAPri;

  return VOS_STATUS_SUCCESS;
}/*                       */


/*                                                                            
                        
                                                                            */

/*                                                                          

                                      

             
                                                                           

              
                                                              

            

      
                                                                       
                                                                            
                                                                          
                                  
                         
                                                                              
                               


              

                                                            

                                                     
                                                                          
                                                                           
                                                          
                                              

              

                                                                            */
VOS_STATUS
WLANTL_RegisterBAPClient
(
  v_PVOID_t              pvosGCtx,
  WLANTL_BAPRxCBType     pfnTlBAPRxFrm,
  WLANTL_FlushOpCompCBType  pfnFlushOpCompleteCb
)
{
  WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == pfnTlBAPRxFrm )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid parameter sent on WLANTL_RegisterBAPClient"));
    return VOS_STATUS_E_INVAL;
  }

  if ( NULL == pfnFlushOpCompleteCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
      "Invalid Flush Complete Cb parameter sent on WLANTL_RegisterBAPClient"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_RegisterBAPClient"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                    
                                                                           */
  if ( 0 != pTLCb->tlBAPClient.ucExists )
  {
    pTLCb->tlBAPClient.ucExists++;
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:BAP client was already registered"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                            
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Registering BAP Client" ));

  pTLCb->tlBAPClient.ucExists++;

  if ( NULL != pfnTlBAPRxFrm ) 
  {
    pTLCb->tlBAPClient.pfnTlBAPRx             = pfnTlBAPRxFrm;
  }

  pTLCb->tlBAPClient.pfnFlushOpCompleteCb   = pfnFlushOpCompleteCb;

  pTLCb->tlBAPClient.vosPendingDataBuff     = NULL;

  return VOS_STATUS_SUCCESS;
}/*                          */


/*                                                                          

                             

             
                                                              

              
                                                                      

                                                                           
                                                                       
                                                                
                                  

                                                                             
                                                                

            

      
                                                                       
                                                                            
                                                                         
                               
                                                     
                                                                        
                                                            

              
                                                            

                                                                        
                                   
                                                          
                                                                           
                                              

                                                                      
                                                        

              

                                                                            */
VOS_STATUS
WLANTL_TxBAPFrm
(
  v_PVOID_t               pvosGCtx,
  vos_pkt_t*              vosDataBuff,
  WLANTL_MetaInfoType*    pMetaInfo,
  WLANTL_TxCompCBType     pfnTlBAPTxComp
)
{
  WLANTL_CbType*  pTLCb      = NULL;
  VOS_STATUS      vosStatus  = VOS_STATUS_SUCCESS;
  v_MACADDR_t     vDestMacAddr;
  v_U16_t         usPktLen;
  v_U8_t          ucStaId = 0;
  v_U8_t          extraHeadSpace = 0;
  v_U8_t          ucWDSEnabled = 0;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_TxBAPFrm"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                    
                                                                           */
  if (( 0 == pTLCb->tlBAPClient.ucExists ) ||
      ( WLANTL_STA_ID_INVALID(pTLCb->tlBAPClient.ucBAPSTAId) ))
  {
    pTLCb->tlBAPClient.ucExists++;
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:BAP client not register on WLANTL_TxBAPFrm"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                                     
                                                                          */
  if ( NULL != pTLCb->tlBAPClient.vosPendingDataBuff )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
        "WLAN TL:BT-AMP Frame already pending tx in TL on WLANTL_TxBAPFrm"));
    return VOS_STATUS_E_BUSY;
  }

  /*                                                                        
                                                                          
          
                                                                           
                                                                           */

  /*                                                                        
                                   
                                                                           */
  ucStaId = pTLCb->tlBAPClient.ucBAPSTAId;
  if ( NULL == pTLCb->atlSTAClients[ucStaId] )
  {
       TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
           "WLAN TL:Client Memory was not allocated on %s", __func__));
       return VOS_STATUS_E_FAILURE;
  }
  if (( 0 == pMetaInfo->ucDisableFrmXtl ) && 
      ( 0 != pTLCb->atlSTAClients[ucStaId]->wSTADesc.ucSwFrameTXXlation ))
  {
    vosStatus =  WLANTL_Translate8023To80211Header( vosDataBuff, &vosStatus,
                                                    pTLCb, &ucStaId,
                                                    pMetaInfo, &ucWDSEnabled,
                                                    &extraHeadSpace);

    if ( VOS_STATUS_SUCCESS != vosStatus )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL:Error when translating header WLANTL_TxBAPFrm"));

      return vosStatus;
    }

    pMetaInfo->ucDisableFrmXtl = 1;
  }

  /*                                                                         
                              
                                                                            */

  /*                                                           */
  pMetaInfo->ucType |= (WLANTL_80211_DATA_TYPE << 4);
  pMetaInfo->ucType |= (WLANTL_80211_DATA_QOS_SUBTYPE);

  vosStatus = WDA_DS_BuildTxPacketInfo( pvosGCtx, vosDataBuff , 
                    &vDestMacAddr, pMetaInfo->ucDisableFrmXtl, 
                    &usPktLen, pTLCb->atlSTAClients[ucStaId]->wSTADesc.ucQosEnabled,
                    ucWDSEnabled, extraHeadSpace, pMetaInfo->ucType,
                            &pTLCb->atlSTAClients[ucStaId]->wSTADesc.vSelfMACAddress,
                    pMetaInfo->ucTID, 0 /*        */, pMetaInfo->usTimeStamp,
                    pMetaInfo->ucIsEapol || pMetaInfo->ucIsWai, pMetaInfo->ucUP );

  if ( VOS_STATUS_SUCCESS != vosStatus )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Failed while building TX header %d", vosStatus));
    return vosStatus;
  }

  if ( NULL != pfnTlBAPTxComp )
  {
    vos_pkt_set_user_data_ptr( vosDataBuff, VOS_PKT_USER_DATA_ID_TL,
                               (v_PVOID_t)pfnTlBAPTxComp);
  }
  else
  {
    vos_pkt_set_user_data_ptr( vosDataBuff, VOS_PKT_USER_DATA_ID_TL,
                               (v_PVOID_t)WLANTL_TxCompDefaultCb);

  }

  vos_atomic_set_U32( (v_U32_t*)&pTLCb->tlBAPClient.vosPendingDataBuff,
                      (v_U32_t)vosDataBuff);

  /*                                                                        
                                                                     
              
                                                                           */
  if (( pTLCb->uResCount >=  WDA_TLI_MIN_RES_BAP ) &&
      ( 0 == pTLCb->ucTxSuspended ))
  {
    WDA_DS_StartXmit(pvosGCtx);
  }
  else
  {
    /*                                                                     
                                                                     
                                                   
                                                                          */
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
          "WLAN TL:Request to send from BAP but condition not met.Res: %d,"
                 "Suspend: %d", pTLCb->uResCount, pTLCb->ucTxSuspended ));
  }

  return VOS_STATUS_SUCCESS;
}/*                 */


/*                                                                            
                        
                                                                            */

/*                                                                          

                            

             
                                                                        
                                                                        
                 

              

                                                                   
                                                                      
                                            

            

      
                                                                       
                                                                            
                                                              

       
                                                 


              
                                                            

                                                     
                                                                          
                                                                           
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_GetRssi
(
  v_PVOID_t        pvosGCtx,
  v_U8_t           ucSTAId,
  v_S7_t*          pRssi
)
{
  WLANTL_CbType*  pTLCb = NULL;
  WLANTL_STAClientType* pClientSTA = NULL;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == pRssi )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_GetRssi"));
    return VOS_STATUS_E_INVAL;
  }

  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid station id requested on WLANTL_GetRssi"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_GetRssi"));
    return VOS_STATUS_E_FAULT;
  }

  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pClientSTA )
  {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Client Memory was not allocated on %s", __func__));
        return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pClientSTA->ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WLAN TL:Station was not previously registered on WLANTL_GetRssi"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                                                    
                                                                           */
  if(pTLCb->isBMPS)
  {
    *pRssi = pClientSTA->rssiAvgBmps;
    /*                                                                           
                                                                                 
                                                                            
                    */
    if(0 == *pRssi)
    {
      *pRssi = pClientSTA->rssiAvg;
    }
  }
  else
  {
    *pRssi = pClientSTA->rssiAvg;
  }

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                    "WLAN TL:WLANTL_GetRssi for STA: %d RSSI: %d%s",
                    ucSTAId, *pRssi,
                    pTLCb->isBMPS ? " in BMPS" : ""));

  return VOS_STATUS_SUCCESS;
}/*                */

/*                                                                          

                           

             
                                                                       
                                                                        
                 

              

                                                                   
                                                                      
                                            

            

      
                                                                       
                                                                            
                                                              

       
                                              


              
                                                            

                                                     
                                                                          
                                                                           
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_GetSnr
(
  tANI_U8           ucSTAId,
  tANI_S8*          pSnr
)
{
  WLANTL_CbType*  pTLCb = NULL;
  WLANTL_STAClientType* pClientSTA = NULL;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if (NULL == pSnr)
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on %s", __func__));
    return VOS_STATUS_E_INVAL;
  }

  if (WLANTL_STA_ID_INVALID(ucSTAId))
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid station id requested on %s", __func__));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                
                                                                           */
  pTLCb = VOS_GET_TL_CB(vos_get_global_context(VOS_MODULE_ID_TL, NULL));
  if (NULL == pTLCb)
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid TL pointer from pvosGCtx on %s", __func__));
    return VOS_STATUS_E_FAULT;
  }

  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if (NULL == pClientSTA)
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
    return VOS_STATUS_E_FAILURE;
  }

  if (0 == pClientSTA->ucExists)
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WLAN TL:Station was not previously registered on %s", __func__));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                                                    
                                                                           */
  if (pTLCb->isBMPS)
  {
    *pSnr = pClientSTA->snrAvgBmps;
  }
  else
  {
    /*                                                                          
                                                                     
                                                                         
                                                                          
                                                   
      
                                                                         
                                                                       
                                                                          
                                                                    
                                    
     */
    if (pClientSTA->snrIdx > (WLANTL_MAX_SNR_DATA_SAMPLES/2)
        || !(pClientSTA->prevSnrAvg))
    {
       *pSnr = pClientSTA->snrSum / pClientSTA->snrIdx;
    }
    else if (pClientSTA->prevSnrAvg)
    {
       *pSnr = pClientSTA->prevSnrAvg;
    }
    else
    {
       *pSnr = SNR_HACK_BMPS;
    }
  }

  VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
            "WLAN TL:WLANTL_GetSnr for STA: %d SNR: %d%s",
            ucSTAId, *pSnr,
            pTLCb->isBMPS ? " in BMPS" : "");

  return VOS_STATUS_SUCCESS;
}/*               */
/*                                                                          

                                   

             
                                                                       
                                                                        
                 

              

                                                                   
                                                                      
                                            

            

      
                                                                       
                                                                            
                                                              

       
                                                       


              
                                                            

                                                     
                                                                          
                                                                           
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_GetLinkQuality
(
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId,
  v_U32_t*              puLinkQuality
)
{
  WLANTL_CbType*  pTLCb = NULL;
  WLANTL_STAClientType* pClientSTA = NULL;

  /*                                                                        
                
                                                                           */
  if ( NULL == puLinkQuality )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "Invalid parameter sent on WLANTL_GetLinkQuality"));
    return VOS_STATUS_E_INVAL;
  }

  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "Invalid station id requested on WLANTL_GetLinkQuality"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "Invalid TL pointer from pvosGCtx on WLANTL_GetLinkQuality"));
    return VOS_STATUS_E_FAULT;
  }
  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pClientSTA )
  {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Client Memory was not allocated on %s", __func__));
        return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pClientSTA->ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "Station was not previously registered on WLANTL_GetLinkQuality"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                                                    
                                                                           */
  *puLinkQuality = pClientSTA->uLinkQualityAvg;

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLANTL_GetLinkQuality for STA: %d LinkQuality: %d", ucSTAId, *puLinkQuality));

  return VOS_STATUS_SUCCESS;
}/*                       */

/*                                                                          

                                

             
                                                                              
                                                                            
              

              

            

      
                                                                       
                                                                            
                                                              
                                                   

       
                                                                             
                           

              
                                              

              
                                                                            */
VOS_STATUS
WLANTL_FlushStaTID
(
  v_PVOID_t             pvosGCtx,
  v_U8_t                ucSTAId,
  v_U8_t                ucTid
)
{
  WLANTL_CbType*  pTLCb = NULL;
  tpFlushACReq FlushACReqPtr = NULL;
  vos_msg_t vosMessage;


  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "Invalid station id requested on WLANTL_FlushStaTID"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "Invalid TL pointer from pvosGCtx on WLANTL_FlushStaTID"));
    return VOS_STATUS_E_FAULT;
  }

  if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
  {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Client Memory was not allocated on %s", __func__));
        return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pTLCb->atlSTAClients[ucSTAId]->ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "Station was not previously registered on WLANTL_FlushStaTID"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                                                                                  
                                                                          */
  FlushACReqPtr = vos_mem_malloc(sizeof(tFlushACReq));

  if ( NULL == FlushACReqPtr )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
        "WLAN TL: fatal failure, cannot allocate Flush Req structure"));
    VOS_ASSERT(0);
    return VOS_STATUS_E_NOMEM;
  }

  //                                       
  FlushACReqPtr->mesgType    = SIR_TL_HAL_FLUSH_AC_REQ;
  FlushACReqPtr->mesgLen     = sizeof(tFlushACReq);
  FlushACReqPtr->mesgLen     = sizeof(tFlushACReq);
  FlushACReqPtr->ucSTAId = ucSTAId;
  FlushACReqPtr->ucTid = ucTid;

  vosMessage.type            = WDA_TL_FLUSH_AC_REQ;
  vosMessage.bodyptr = (void *)FlushACReqPtr;

  vos_mq_post_message(VOS_MQ_ID_WDA, &vosMessage);
  return VOS_STATUS_SUCCESS;
}


/*                                                                            
                       
                                                                            */

/*                                                                          

                                          

             
                                                                        

              
                                                         

            

      
                                                                      
                                                                            
                                                                     
                                         

              
                                                            

                                                     
                                                                        
                                   
                                                                 
                                              

              

                                                                            */
VOS_STATUS
WLANTL_RegisterMgmtFrmClient
(
  v_PVOID_t               pvosGCtx,
  WLANTL_MgmtFrmRxCBType  pfnTlMgmtFrmRx
)
{
  WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == pfnTlMgmtFrmRx )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
           "WLAN TL:Invalid parameter sent on WLANTL_RegisterMgmtFrmClient"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ChangeSTAState"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                    
                                                                           */
  if ( 0 != pTLCb->tlMgmtFrmClient.ucExists )
  {
    pTLCb->tlMgmtFrmClient.ucExists++;
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Management frame client was already registered"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                            
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
            "WLAN TL:Registering Management Frame Client" ));

  pTLCb->tlMgmtFrmClient.ucExists++;

  if ( NULL != pfnTlMgmtFrmRx )
  {
    pTLCb->tlMgmtFrmClient.pfnTlMgmtFrmRx = pfnTlMgmtFrmRx;
  }

  pTLCb->tlMgmtFrmClient.vosPendingDataBuff = NULL;

  return VOS_STATUS_SUCCESS;
}/*                              */

/*                                                                          

                                            

             
                                                                          

              
                                                         

            

      
                                                                      
                                                                            
              
                                                            

                                                                        
                                   
                                                               
                                              

              

                                                                            */
VOS_STATUS
WLANTL_DeRegisterMgmtFrmClient
(
  v_PVOID_t               pvosGCtx
)
{
  WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ChangeSTAState"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                    
                                                                           */
  if ( 0 == pTLCb->tlMgmtFrmClient.ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Management frame client was never registered"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
                              
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
            "WLAN TL:Deregistering Management Frame Client" ));

  pTLCb->tlMgmtFrmClient.pfnTlMgmtFrmRx = WLANTL_MgmtFrmRxDefaultCb;
  if ( NULL != pTLCb->tlMgmtFrmClient.vosPendingDataBuff)
  {
    TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
              "WLAN TL:Management cache buffer not empty on deregistering"
               " - dropping packet" ));
    vos_pkt_return_packet(pTLCb->tlMgmtFrmClient.vosPendingDataBuff);

    pTLCb->tlMgmtFrmClient.vosPendingDataBuff = NULL; 
  }

  pTLCb->tlMgmtFrmClient.ucExists = 0;

  return VOS_STATUS_SUCCESS;
}/*                              */

/*                                                                          

                              

             
                                                             
                                                                            
                                                                       
                                      
                                                                

              
                                                         

                                                                            
                                                                

                                                                         
                                                                   

            

      
                                                                      
                                                                   
                                                                      
                                           
                                                                          
                                                          
                                                           
                                                   
                                                                     
                                                                         
                                                                          
                                                                    
                                                        
                                                                    
                                                                       
                                                                         
                                                                       
                             


              
                                                            

                                                     
                                                                        
                                   
                                                                 
                                                                         
                                              

                                                                      
                                                        

              

                                                                            */
VOS_STATUS
WLANTL_TxMgmtFrm
(
  v_PVOID_t            pvosGCtx,
  vos_pkt_t*           vosFrmBuf,
  v_U16_t              usFrmLen,
  v_U8_t               wFrmType,
  v_U8_t               ucTid,
  WLANTL_TxCompCBType  pfnCompTxFunc,
  v_PVOID_t            pvBDHeader,
  v_U8_t               ucAckResponse
)
{
  WLANTL_CbType*  pTLCb = NULL;
  v_MACADDR_t     vDestMacAddr;
  VOS_STATUS      vosStatus = VOS_STATUS_SUCCESS;
  v_U16_t         usPktLen;
  v_U32_t         usTimeStamp = 0;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == vosFrmBuf )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_TxMgmtFrm"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGP(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
              "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_TxMgmtFrm"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                                 
                                                                           */
  if ( 0 == pTLCb->tlMgmtFrmClient.ucExists )
  {
    pTLCb->tlMgmtFrmClient.ucExists++;
    TLLOGP(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
          "WLAN TL:Management Frame client not register on WLANTL_TxMgmtFrm"));
    return VOS_STATUS_E_EXISTS;
  }

   /*                                                                        
                                    
                                                                           */
  //                                                        
  if ( NULL != pTLCb->tlMgmtFrmClient.vosPendingDataBuff )
  {

    TLLOGP(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
        "WLAN TL:Management Frame already pending tx in TL: failing old one"));


    /*                                                   */
    //                                                                         
    //                                   

    //                                                                
    //                                                  

    /*                                                                      */
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
    return VOS_STATUS_E_RESOURCES;
  }


  /*                                                                        
                                                  
                                                                           */
  if ( NULL == pvBDHeader )
  {
     v_MACADDR_t*     pvAddr2MacAddr;
     v_U8_t   uQosHdr = VOS_FALSE;

     /*                                                              */
     vosStatus = vos_pkt_peek_data( vosFrmBuf, 
                                    WLANTL_MAC_ADDR_ALIGN(1) + VOS_MAC_ADDR_SIZE,
                                    (v_PVOID_t)&pvAddr2MacAddr, VOS_MAC_ADDR_SIZE);

     if ( VOS_STATUS_SUCCESS != vosStatus )
     {
       TLLOGP(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
                "WLAN TL:Failed while attempting to get addr2 %d", vosStatus));
       return vosStatus;
     }

    /*                                                               
                                                        
     */
    if (WLANTL_IS_QOS_DATA_FRAME(wFrmType))                                      \
    {
        uQosHdr = VOS_TRUE;
    }
    /*                                                                      
                                 
                                                                           */
    vosStatus = WDA_DS_BuildTxPacketInfo( pvosGCtx, vosFrmBuf , &vDestMacAddr, 
                   1 /*                     */, &usPktLen, uQosHdr /*                   */, 
                   0 /*         */, 0, wFrmType, pvAddr2MacAddr, ucTid, 
                   ucAckResponse, usTimeStamp, 0, 0 );


    if ( !VOS_IS_STATUS_SUCCESS(vosStatus) )
    {
      TLLOGP(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
                "WLAN TL:Failed while attempting to build TX header %d", vosStatus));
      return vosStatus;
    }
   }/*                          */

  /*                                                                        
                                                                          
          
                                                                           
                                                                           */
  if ( NULL != pfnCompTxFunc )
  {
    vos_pkt_set_user_data_ptr( vosFrmBuf, VOS_PKT_USER_DATA_ID_TL,
                               (v_PVOID_t)pfnCompTxFunc);
  }
  else
  {
    vos_pkt_set_user_data_ptr( vosFrmBuf, VOS_PKT_USER_DATA_ID_TL,
                               (v_PVOID_t)WLANTL_TxCompDefaultCb);

  }

  vos_atomic_set_U32( (v_U32_t*)&pTLCb->tlMgmtFrmClient.vosPendingDataBuff,
                      (v_U32_t)vosFrmBuf);

  /*                                                                        
                                                                     
              
                                                                           */
  if ( pTLCb->uResCount >=  WDA_TLI_MIN_RES_MF )
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
              "WLAN TL:Issuing Xmit start request to BAL for MGMT"));
    vosStatus = WDA_DS_StartXmit(pvosGCtx);
    if(VOS_STATUS_SUCCESS != vosStatus)
    {
       TLLOGP(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
              "WLAN TL:WDA_DS_StartXmit fails. vosStatus %d", vosStatus));
       vos_atomic_set_U32( (v_U32_t*)&pTLCb->tlMgmtFrmClient.vosPendingDataBuff,0); 
    }
    return vosStatus;
    
  }
  else
  {
    /*                                                                     
                                                                     
                                                   
                                                                          */
    TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
       "WLAN TL:Request to send for Mgmt Frm but condition not met. Res: %d",
               pTLCb->uResCount));
  }

  return VOS_STATUS_SUCCESS;
}/*                  */

/*                                                                            
                        
                                                                            */

/*                                                                          

                                      

             
                                                   
                       

              

            

      
                                                                       
                                                                   


              
                                                            

                                                                        
                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_ResetNotification
(
  v_PVOID_t   pvosGCtx
)
{
  WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                           
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
      "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ResetNotification"));
    return VOS_STATUS_E_FAULT;
  }

  WLANTL_CleanCB(pTLCb, 1 /*                                    */);
  return VOS_STATUS_SUCCESS;
}/*                          */

/*                                                                          

                                  

             
                                                                   
                   

              
                                                                       
              

                                                                           
                                                                     
                                                                        
                                                                

            

      
                                                                       
                                                                   
                                                                            
                                                                         
                                                                          
                                        


              
                                                            

                                                                           
                                                                            
                                                    
                                               

              

                                                                            */

VOS_STATUS
WLANTL_SuspendDataTx
(
  v_PVOID_t              pvosGCtx,
  v_U8_t*                pucSTAId,
  WLANTL_SuspendCBType   pfnSuspendTx
)
{
  WLANTL_CbType*  pTLCb = NULL;
  vos_msg_t       vosMsg;

  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_SuspendDataTx"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                                      
                                                                           */
  if (NULL == pucSTAId)
  {
    /*                                  */
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
              "WLAN TL:General suspend requested"));
    vos_atomic_set_U8( &pTLCb->ucTxSuspended, 1);
    vosMsg.reserved = WLAN_MAX_STA_COUNT;
  }
  else
  {
    if ( WLANTL_STA_ID_INVALID( *pucSTAId ) )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
             "WLAN TL:Invalid station id %d requested on WLANTL_SuspendDataTx", *pucSTAId));
      return VOS_STATUS_E_FAULT;
    }

    if ( NULL == pTLCb->atlSTAClients[*pucSTAId] )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Invalid pTLCb->atlSTAClients pointer for STA Id :%d on "
            "WLANTL_SuspendDataTx", *pucSTAId));
      return VOS_STATUS_E_FAULT;
    }

    if ( 0 == pTLCb->atlSTAClients[*pucSTAId]->ucExists )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
             "WLAN TL:Station %d was not previously registered on WLANTL_SuspendDataTx", *pucSTAId));
      return VOS_STATUS_E_EXISTS;
    }

    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
         "WLAN TL:Suspend request for station: %d", *pucSTAId));
    vos_atomic_set_U8( &pTLCb->atlSTAClients[*pucSTAId]->ucTxSuspended, 1);
    vosMsg.reserved = *pucSTAId;
  }

  /*                                                                        
                                       
                                                                           */
  vosMsg.type     = WLANTL_TX_SIG_SUSPEND;
  vosMsg.bodyptr     = (v_PVOID_t)pfnSuspendTx;

  if(!VOS_IS_STATUS_SUCCESS(vos_tx_mq_serialize( VOS_MQ_ID_TL, &vosMsg)))
  {
    VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
       " %s fails to post message", __func__);
  }

  return VOS_STATUS_SUCCESS;
}/*                      */

/*                                                                          

                                 

             
                                                              

                                                                         
                                    

              

            

      
                                                                       
                                                                   
                                                                          
                                                 

              
                                                            

                                                                           
                                                                            
                                                    
                                               

              

                                                                            */

VOS_STATUS
WLANTL_ResumeDataTx
(
  v_PVOID_t      pvosGCtx,
  v_U8_t*        pucSTAId
)
{
  WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
           "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ResumeDataTx"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                   
                                                                           */
  if ( NULL == pucSTAId )
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
              "WLAN TL:General resume requested"));
    vos_atomic_set_U8( &pTLCb->ucTxSuspended, 0);
  }
  else
  {
    if ( WLANTL_STA_ID_INVALID( *pucSTAId ))
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Invalid station id %d requested on WLANTL_ResumeDataTx", *pucSTAId));
      return VOS_STATUS_E_FAULT;
    }

    if ( NULL == pTLCb->atlSTAClients[*pucSTAId] )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Invalid pTLCb->atlSTAClients pointer for STA Id :%d on "
            "WLANTL_ResumeDataTx", *pucSTAId));
      return VOS_STATUS_E_FAULT;
    }

    if ( 0 == pTLCb->atlSTAClients[*pucSTAId]->ucExists )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
             "WLAN TL:Station %d was not previously registered on WLANTL_ResumeDataTx", *pucSTAId));
      return VOS_STATUS_E_EXISTS;
    }

    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
         "WLAN TL:Resume request for station: %d", *pucSTAId));
    vos_atomic_set_U8( &pTLCb->atlSTAClients[*pucSTAId]->ucTxSuspended, 0);
  }

  /*                                                                        
                         
                                                                           */
  if (( pTLCb->uResCount >=  WDA_TLI_MIN_RES_MF ) &&
      ( 0 == pTLCb->ucTxSuspended ))
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:Resuming transmission"));
    return WDA_DS_StartXmit(pvosGCtx);
  }

  return VOS_STATUS_SUCCESS;
}/*                     */

/*                                                                          
                              

             
                                                                      

              
                                                   

            

     
                                                                      
                                                                  
                                                   

              
                                                            

                                                  
                                                                         
                                    
                                               


              

                                                                            */
VOS_STATUS
WLANTL_SuspendCB
(
  v_PVOID_t             pvosGCtx,
  WLANTL_SuspendCBType  pfnSuspendCB,
  v_U16_t               usReserved
)
{
  WLANTL_CbType*  pTLCb   = NULL;
  v_U8_t          ucSTAId = (v_U8_t)usReserved;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == pfnSuspendCB )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
               "WLAN TL: No Call back processing requested WLANTL_SuspendCB"));
    return VOS_STATUS_SUCCESS;
  }

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_SuspendCB"));
    return VOS_STATUS_E_FAULT;
  }

  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    pfnSuspendCB(pvosGCtx, NULL, VOS_STATUS_SUCCESS);
  }
  else
  {
    pfnSuspendCB(pvosGCtx, &ucSTAId, VOS_STATUS_SUCCESS);
  }

  return VOS_STATUS_SUCCESS;
}/*                */


/*                                                                            
                                
                                                                            */

/*                                                                          

                                  

             
                                                                 
                

              

            

      
                                                                       
                                                                   
                                             
                                           

       
                                                                        

              
                                                            

                                                      
                                                                           
                                                                            
                                                    
                                               

              

                                                                            */
VOS_STATUS
WLANTL_GetTxPktCount
(
  v_PVOID_t      pvosGCtx,
  v_U8_t         ucSTAId,
  v_U8_t         ucTid,
  v_U32_t*       puTxPktCount
)
{
  WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == puTxPktCount )
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid parameter sent on WLANTL_GetTxPktCount"));
    return VOS_STATUS_E_INVAL;
  }

  if ( WLANTL_STA_ID_INVALID( ucSTAId ) || WLANTL_TID_INVALID( ucTid) )
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL:Invalid station id %d/tid %d requested on WLANTL_GetTxPktCount",
            ucSTAId, ucTid));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                                        
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_GetTxPktCount"));
    return VOS_STATUS_E_FAULT;
  }

  if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
  {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Client Memory was not allocated on %s", __func__));
        return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pTLCb->atlSTAClients[ucSTAId]->ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL:Station was not previously registered on WLANTL_GetTxPktCount %d",
     ucSTAId));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
               
                                                                           */
  //                                                      
    //                                                                   
      //                         

  *puTxPktCount = pTLCb->atlSTAClients[ucSTAId]->auTxCount[ucTid];

  return VOS_STATUS_SUCCESS;
}/*                      */

/*                                                                          

                                  

             
                                                              
                

              

            

      
                                                                       
                                                                   
                                             
                                           

      
                                                                        

              
                                                            

                                                      
                                                                           
                                                                            
                                                    
                                               

              

                                                                            */
VOS_STATUS
WLANTL_GetRxPktCount
(
  v_PVOID_t      pvosGCtx,
  v_U8_t         ucSTAId,
  v_U8_t         ucTid,
  v_U32_t*       puRxPktCount
)
{
  WLANTL_CbType*  pTLCb = NULL;
  WLANTL_STAClientType* pClientSTA = NULL;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == puRxPktCount )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_GetRxPktCount"));
    return VOS_STATUS_E_INVAL;
  }

  if ( WLANTL_STA_ID_INVALID( ucSTAId ) || WLANTL_TID_INVALID( ucTid) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL:Invalid station id %d/tid %d requested on WLANTL_GetRxPktCount",
             ucSTAId, ucTid));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                                          
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_GetRxPktCount"));
    return VOS_STATUS_E_FAULT;
  }
  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pClientSTA )
  {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Client Memory was not allocated on %s", __func__));
        return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pClientSTA->ucExists )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL:Station was not previously registered on WLANTL_GetRxPktCount"));
    return VOS_STATUS_E_EXISTS;
  }

  /*                                                                        
               
                                                                           */
  TLLOG3(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_MED,
            "WLAN TL:Requested rx packet count for STA: %d, TID: %d",
             ucSTAId, ucTid));

  *puRxPktCount = pClientSTA->auRxCount[ucTid];

  return VOS_STATUS_SUCCESS;
}/*                      */

VOS_STATUS
WLANTL_TxFCFrame
(
  v_PVOID_t       pvosGCtx
);
/*                                                                            
                                                
                                                                            */

/*                                                                          

                              

             

                                                                      
                                                                         
                                                                        

              

                                                                          
                  

            

      
                                                                       
                                                                            
                                                            
                                                                           
                                      

       
                                                                         
                                                                        
                                                        

              

                                                            

                                                                              
                                       

              

      
    
                                                                               
                  

                                                                            */
v_U32_t
WLANTL_GetFrames
(
  v_PVOID_t       pvosGCtx,
  vos_pkt_t     **ppFrameDataBuff,
  v_U32_t         uSize,
  v_U8_t          uFlowMask,
  v_BOOL_t*       pbUrgent
)
{
   vos_pkt_t**         pvosDataBuff = (vos_pkt_t**)ppFrameDataBuff;
   WLANTL_CbType*      pTLCb = NULL;
   WLANTL_STAClientType* pClientSTA = NULL;
   v_U32_t             uRemaining = uSize;
   vos_pkt_t*          vosRoot;
   vos_pkt_t*          vosTempBuf;
   WLANTL_STAFuncType  pfnSTAFsm;
   v_U16_t             usPktLen;
   v_U32_t             uResLen;
   v_U8_t              ucSTAId;
   v_U8_t              ucAC;
   vos_pkt_t*          vosDataBuff;
   v_U32_t             uTotalPktLen;
   v_U32_t             i=0;
   v_U32_t             j=0;
   v_U32_t             ucResult = 0;
   VOS_STATUS          vosStatus;
   WLANTL_STAEventType   wSTAEvent;
   tBssSystemRole       systemRole;
   tpAniSirGlobal pMac;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if (( NULL == pTLCb ) || ( NULL == pvosDataBuff ))
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ChangeSTAState"));
    return ucResult;
  }

  pMac = vos_get_context(VOS_MODULE_ID_PE, pvosGCtx);
  if ( NULL == pMac )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                      "%s: Invalid pMac", __func__));
    return ucResult;
  }

  vosDataBuff = pTLCb->vosDummyBuf; /*                                   
                                                        */

  pTLCb->uResCount = uSize;

  /*                                                                       
                                                          
                                                                          */
  vosRoot = vosDataBuff;
 
  /*                                                                       
                                                           
                                                                          */
  pTLCb->bUrgent      = FALSE;

  while (( pTLCb->tlConfigInfo.uMinFramesProcThres < pTLCb->uResCount ) &&
         ( 0 < uRemaining ))
  {
    systemRole = wdaGetGlobalSystemRole(pMac);
#ifdef WLAN_SOFTAP_FLOWCTRL_EN
/*                                                                             */
    if (eSYSTEM_AP_ROLE == systemRole)
    {
       if (pTLCb->done_once == 0 && NULL == pTLCb->vosTxFCBuf)
       {
          WLANTL_TxFCFrame (pvosGCtx);
          pTLCb->done_once ++;
       }
    } 
    if ( NULL != pTLCb->vosTxFCBuf )
    {
       //                                               
       WDA_TLI_PROCESS_FRAME_LEN( pTLCb->vosTxFCBuf, usPktLen, uResLen, uTotalPktLen);
    
       if ( ( pTLCb->uResCount > uResLen ) &&
            ( uRemaining > uTotalPktLen ) &&
            ( uFlowMask & ( 1 << WDA_TXFLOW_FC ) ) )
       {
          TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                   "WLAN TL:Chaining FC frame first on GetFrame"));

          vos_pkt_chain_packet( vosDataBuff, pTLCb->vosTxFCBuf, 1 /*    */ );

          vos_atomic_set_U32( (v_U32_t*)&pTLCb->vosTxFCBuf, (v_U32_t)NULL);

          /*                           */
          pTLCb->bUrgent      = TRUE;

          /*                              */
          uRemaining        -= (usPktLen + WDA_DXE_HEADER_SIZE);

          /*                      */
          pTLCb->uResCount -= uResLen;
       }
       else
       {
          TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                "WLAN TL:send fc out of source %s", __func__));
          ucResult = ( pTLCb->uResCount > uResLen )?VOS_TRUE:VOS_FALSE;
          break; /*                                     */
       }
   }
   else 
#endif //                       

    if (( NULL != pTLCb->tlMgmtFrmClient.vosPendingDataBuff ) &&
        ( uFlowMask & ( 1 << WDA_TXFLOW_MGMT ) ) )
    {
      WDA_TLI_PROCESS_FRAME_LEN( pTLCb->tlMgmtFrmClient.vosPendingDataBuff,
                          usPktLen, uResLen, uTotalPktLen);

      if (usPktLen > WLANTL_MAX_ALLOWED_LEN)
      {
          usPktLen = WLANTL_MAX_ALLOWED_LEN;
          VOS_ASSERT(0);
      }

      if ( ( pTLCb->uResCount > uResLen ) &&
           ( uRemaining > uTotalPktLen ) &&
           ( uFlowMask & ( 1 << WDA_TXFLOW_MGMT ) ) )
      {
        TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                   "WLAN TL:Chaining management frame on GetFrame"));

        vos_pkt_chain_packet( vosDataBuff,
                              pTLCb->tlMgmtFrmClient.vosPendingDataBuff,
                              1 /*    */ );

        vos_atomic_set_U32( (v_U32_t*)&pTLCb->tlMgmtFrmClient.
                                  vosPendingDataBuff, (v_U32_t)NULL);

        /*                                   */
        pTLCb->bUrgent      = TRUE;

        /*                              */
        uRemaining        -= (usPktLen + WDA_DXE_HEADER_SIZE);

        /*                      */
        pTLCb->uResCount -= uResLen;
      }
      else
      {
        ucResult = ( pTLCb->uResCount > uResLen )?VOS_TRUE:VOS_FALSE;
        break; /*                                     */
      }
    }
    else if (( pTLCb->tlBAPClient.vosPendingDataBuff ) &&
             ( WDA_TLI_MIN_RES_BAP <= pTLCb->uResCount ) &&
             ( 0 == pTLCb->ucTxSuspended )          )
    {
      WDA_TLI_PROCESS_FRAME_LEN( pTLCb->tlBAPClient.vosPendingDataBuff,
                          usPktLen, uResLen, uTotalPktLen);

      if (usPktLen > WLANTL_MAX_ALLOWED_LEN)
      {
          usPktLen = WLANTL_MAX_ALLOWED_LEN;
          VOS_ASSERT(0);
      }

      if ( ( pTLCb->uResCount > (uResLen + WDA_TLI_MIN_RES_MF ) ) &&
           ( uRemaining > uTotalPktLen ))
      {
        TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                   "WLAN TL:Chaining BT-AMP frame on GetFrame"));

        vos_pkt_chain_packet( vosDataBuff,
                              pTLCb->tlBAPClient.vosPendingDataBuff,
                              1 /*    */ );

        /*                            */
        pTLCb->bUrgent      = TRUE;

        vos_atomic_set_U32( (v_U32_t*)&pTLCb->tlBAPClient.vosPendingDataBuff,
                            (v_U32_t)NULL);

        /*                              */
        uRemaining        -=  (usPktLen + WDA_DXE_HEADER_SIZE);

        /*                      */
        pTLCb->uResCount  -= uResLen;
      }
      else
      {
        ucResult = uResLen + WDA_TLI_MIN_RES_MF;
        break; /*                                     */
      }
    }
    /*                                                               */
    /*                                                                            
                                                                        
                                                   
                                                                                
                                                                                           
                                 
     */
    else if (( WDA_TLI_MIN_RES_MF <= pTLCb->uResCount )&&
             ( 0 == pTLCb->ucTxSuspended ) &&
             ( uFlowMask & ( 1 << WDA_TXFLOW_MGMT ) )
            )
    {
        vosTempBuf = NULL;
        /*                                                                     
                                                              
                                                                              */
        for ( i = 0; i < WLAN_MAX_STA_COUNT; i++)
        {
           if ((NULL != pTLCb->atlSTAClients[i]) &&
               (pTLCb->atlSTAClients[i]->ucExists) &&
               (0 == pTLCb->atlSTAClients[i]->ucTxSuspended) &&
               (WLANTL_STA_CONNECTED == pTLCb->atlSTAClients[i]->tlState) &&
               (pTLCb->atlSTAClients[i]->ucPktPending)
               )
               break;
        }

        if (i >= WLAN_MAX_STA_COUNT)
        {
           /*                                  */
           break;
        }
        /*                                     */
        ucSTAId = i;

        for (j = WLANTL_MAX_AC ; j > 0; j--)
        {
          if (0 != pTLCb->atlSTAClients[ucSTAId]->aucACMask[j-1])
          {
            pTLCb->atlSTAClients[ucSTAId]->ucCurrentAC = j-1;
            pTLCb->uCurServedAC = j-1;
          }
        }

        pClientSTA = pTLCb->atlSTAClients[ucSTAId];

        wSTAEvent = WLANTL_TX_EVENT;

        pfnSTAFsm = tlSTAFsm[pClientSTA->tlState].
                        pfnSTATbl[wSTAEvent];

        if ( NULL != pfnSTAFsm )
        {
          pClientSTA->ucNoMoreData = 0;
          vosStatus  = pfnSTAFsm( pvosGCtx, ucSTAId, &vosTempBuf, VOS_FALSE);

          if (( VOS_STATUS_SUCCESS != vosStatus ) &&
              ( NULL != vosTempBuf ))
          {
               pClientSTA->pfnSTATxComp( pvosGCtx, vosTempBuf, vosStatus );
               vosTempBuf = NULL;
               break;
          }/*               */
        }

        if (NULL != vosTempBuf)
        {
            WDA_TLI_PROCESS_FRAME_LEN( vosTempBuf, usPktLen, uResLen, uTotalPktLen);

            if (usPktLen > WLANTL_MAX_ALLOWED_LEN)
            {
                usPktLen = WLANTL_MAX_ALLOWED_LEN;
                VOS_ASSERT(0);
            }

            TLLOG4(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_LOW,
                      "WLAN TL:Resources needed by frame: %d", uResLen));

            if ( ( pTLCb->uResCount >= (uResLen + WDA_TLI_MIN_RES_MF ) ) &&
               ( uRemaining > uTotalPktLen )
               )
            {
              TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                        "WLAN TL:Chaining data frame on GetFrame"));

              vos_pkt_chain_packet( vosDataBuff, vosTempBuf, 1 /*    */ );

              /*                             */
              pTLCb->bUrgent      = TRUE;

              vosTempBuf =  NULL;

              /*                              */
              uRemaining  -= (usPktLen + WDA_DXE_HEADER_SIZE);

               /*                      */
              pTLCb->uResCount  -= uResLen;

              //                  
              pClientSTA->uIngress_length += uResLen;
              pClientSTA->uBuffThresholdMax = (pClientSTA->uBuffThresholdMax >= uResLen) ?
                (pClientSTA->uBuffThresholdMax - uResLen) : 0;
              pClientSTA->ucEapolPktPending = 0;
              VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                "WLAN TL:GetFrames STA: %d EAPOLPktPending %d",
                         ucSTAId, pClientSTA->ucEapolPktPending);
            }
         }
         else
         {  //                                
            TLLOG2(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                   "WLAN TL:GetFrames STA: %d, no EAPOL frame, continue.",
                   ucSTAId));
            continue;
         }
    }

    else if (( WDA_TLI_MIN_RES_DATA <= pTLCb->uResCount ) &&
             ( 0 == pTLCb->ucTxSuspended ) &&
             (( uFlowMask & ( 1 << WDA_TXFLOW_AC_BK ) ) ||
              ( uFlowMask & ( 1 << WDA_TXFLOW_AC_BE ) ) ||
              ( uFlowMask & ( 1 << WDA_TXFLOW_AC_VI ) ) ||
              ( uFlowMask & ( 1 << WDA_TXFLOW_AC_VO ) )) )
    {
      /*                                                                     
                                                                          
                        
                                                                            */
      vosTempBuf = NULL;

      if ( NULL != pTLCb->vosTempBuf ) 
      {
        vosTempBuf          = pTLCb->vosTempBuf;
        pTLCb->vosTempBuf   = NULL;
        ucSTAId             = pTLCb->ucCachedSTAId; 
        ucAC                = pTLCb->ucCachedAC;

        if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
        {
            TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL:Client Memory was not allocated on %s", __func__));
            continue;
        }

        pTLCb->atlSTAClients[ucSTAId]->ucNoMoreData = 0;
        pClientSTA = pTLCb->atlSTAClients[ucSTAId];

        TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                   "WLAN TL:Chaining cached data frame on GetFrame"));
      }
      else
      {
        WLAN_TLGetNextTxIds( pvosGCtx, &ucSTAId);
        if (ucSTAId >= WLAN_MAX_STA_COUNT)
        {
         /*                                                    
                                                               
                                                          
                                                    */

            break;
        }
        /*                                                           
                               */
        pClientSTA = pTLCb->atlSTAClients[ucSTAId];
        if ( NULL == pClientSTA )
        {
            TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL:Client Memory was not allocated on %s", __func__));
            continue;
        }

        ucAC = pClientSTA->ucCurrentAC;

        pClientSTA->ucNoMoreData = 1;
        TLLOG4(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_LOW,
                   "WLAN TL: %s get one data frame, station ID %d ", __func__, ucSTAId));
        /*                                                                   
                                                              
                                                                            */
        if ( ( ! WLANTL_STA_ID_INVALID( ucSTAId ) ) &&
           ( 0 == pClientSTA->ucTxSuspended ) &&
           ( 0 == pClientSTA->fcStaTxDisabled) )
        {
          TLLOG4(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_LOW,
                   "WLAN TL: %s sta id valid and not suspended ",__func__));
          wSTAEvent = WLANTL_TX_EVENT;

          pfnSTAFsm = tlSTAFsm[pClientSTA->tlState].
                        pfnSTATbl[wSTAEvent];

          if ( NULL != pfnSTAFsm )
          {
            pClientSTA->ucNoMoreData = 0;
            vosStatus  = pfnSTAFsm( pvosGCtx, ucSTAId, &vosTempBuf, VOS_FALSE);

            if (( VOS_STATUS_SUCCESS != vosStatus ) &&
                ( NULL != vosTempBuf ))
            {
                 pClientSTA->pfnSTATxComp( pvosGCtx,
                                                             vosTempBuf,
                                                             vosStatus );
                 vosTempBuf = NULL;
            }/*               */
          }/*                   */
        }/*                             */
        else
        {
           if ( ! WLANTL_STA_ID_INVALID( ucSTAId ) ) 
           {
                TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "WLAN TL:Not fetching frame because suspended for sta ID %d", 
                   ucSTAId));
           }
        }
      }/*      */

      if ( NULL != vosTempBuf )
      {
        WDA_TLI_PROCESS_FRAME_LEN( vosTempBuf, usPktLen, uResLen, uTotalPktLen);

        if (usPktLen > WLANTL_MAX_ALLOWED_LEN)
        {
            usPktLen = WLANTL_MAX_ALLOWED_LEN;
            VOS_ASSERT(0);
        }

        TLLOG4(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_LOW,
                  "WLAN TL:Resources needed by frame: %d", uResLen));

        if ( ( pTLCb->uResCount >= (uResLen + WDA_TLI_MIN_RES_BAP ) ) &&
             ( uRemaining > uTotalPktLen ) &&
             ( uFlowMask & ( 1 << ucAC ) ) )
        {
          TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                    "WLAN TL:Chaining data frame on GetFrame"));

          vos_pkt_chain_packet( vosDataBuff, vosTempBuf, 1 /*    */ );
          vosTempBuf =  NULL;

          /*                              */
          uRemaining        -= (usPktLen + WDA_DXE_HEADER_SIZE);

           /*                      */
          pTLCb->uResCount  -= uResLen;

          //                  
          pClientSTA->uIngress_length += uResLen;
          pClientSTA->uBuffThresholdMax = (pClientSTA->uBuffThresholdMax >= uResLen) ?
            (pClientSTA->uBuffThresholdMax - uResLen) : 0;

        }
        else
        {
          /*                                                    */
          pTLCb->vosTempBuf = vosTempBuf;
          pTLCb->ucCachedSTAId = ucSTAId;
          pTLCb->ucCachedAC    = ucAC;
          ucResult = uResLen + WDA_TLI_MIN_RES_BAP;
          TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "min %d res required by TL.", ucResult ));
          break; /*                                     */
        }
      }
      else
      {
           for ( i = 0; i < WLAN_MAX_STA_COUNT; i++)
           {
              if (NULL != pTLCb->atlSTAClients[i] && (pTLCb->atlSTAClients[i]->ucExists) &&
                  (pTLCb->atlSTAClients[i]->ucPktPending))
              {
                  /*                               */
                  break;
              }
           }
           if (i >= WLAN_MAX_STA_COUNT)
           {
              /*                                  */
              break;
           }
           else
           {
              /*                   */
              continue;
           }
        } 
      }
    else
    {
      TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Returning from GetFrame: resources = %d suspended = %d",
                 pTLCb->uResCount, pTLCb->ucTxSuspended));
      /*                                                               
                                                                            */
      if(( 0 == pTLCb->ucTxSuspended ) && 
         (( uFlowMask & ( 1 << WDA_TXFLOW_AC_BK ) ) || 
          ( uFlowMask & ( 1 << WDA_TXFLOW_AC_BE ) ) ||
          ( uFlowMask & ( 1 << WDA_TXFLOW_AC_VI ) ) || 
          ( uFlowMask & ( 1 << WDA_TXFLOW_AC_VO ) )))
      {
         TLLOG1(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
             "WLAN TL:Returning from GetFrame: resources = %d",
                 pTLCb->uResCount));
         ucResult = WDA_TLI_MIN_RES_DATA;
      }
       break; /*                         */
    }

    pTLCb->usPendingTxCompleteCount++;
    /*                                */
    vos_pkt_walk_packet_chain( vosDataBuff, &vosDataBuff, 0/*     */ );
  }

  /*                                                                      
                                   
                                                                         */
  vos_pkt_walk_packet_chain( vosRoot, &vosDataBuff, 1/*    */ );

  *pvosDataBuff = vosDataBuff;
  if (pbUrgent)
  {
      *pbUrgent     = pTLCb->bUrgent;
  }
  else
  {
      VOS_ASSERT( pbUrgent );
  }
  return ucResult;
}/*                  */


/*                                                                          

                           

             
                                                                          
                                        

              
                                                             

            

      
                                                                       
                                                                            
                                                                          
                                                                           
                                                                   
                                                                           
                                                                    

              
                                                            

                                                      
                                                                         
                                    
                                                    
                                               

              

                                                                            */
VOS_STATUS
WLANTL_TxComp
(
  v_PVOID_t       pvosGCtx,
  vos_pkt_t      *pFrameDataBuff,
  VOS_STATUS      wTxStatus
)
{
  vos_pkt_t*           vosDataBuff = (vos_pkt_t*)pFrameDataBuff;
  WLANTL_CbType*       pTLCb     = NULL;
  WLANTL_TxCompCBType  pfnTxComp = NULL;
  VOS_STATUS           vosStatus = VOS_STATUS_SUCCESS;
  vos_pkt_t*           vosTempTx = NULL;

  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == vosDataBuff )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Extraneous NULL data pointer on WLANTL_TxComp"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                           
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_TxComp"));
    return VOS_STATUS_E_FAULT;
  }

  while ((0 < pTLCb->usPendingTxCompleteCount) &&
         ( VOS_STATUS_SUCCESS == vosStatus ) &&
         ( NULL !=  vosDataBuff))
  {
    vos_pkt_get_user_data_ptr(  vosDataBuff, VOS_PKT_USER_DATA_ID_TL,
                               (v_PVOID_t)&pfnTxComp);

    /*                                                                      */
    if ( NULL == pfnTxComp )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL:NULL pointer to Tx Complete on WLANTL_TxComp"));
      VOS_ASSERT(0);
      return VOS_STATUS_E_FAULT;
    }

    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:Calling Tx complete for pkt %p in function %p",
               vosDataBuff, pfnTxComp));

    vosTempTx = vosDataBuff;
    vosStatus = vos_pkt_walk_packet_chain( vosDataBuff,
                                           &vosDataBuff, 1/*    */);

    pfnTxComp( pvosGCtx, vosTempTx, wTxStatus );

    pTLCb->usPendingTxCompleteCount--;
  }

 
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
            "WLAN TL: current TL values are: resources = %d "
            "pTLCb->usPendingTxCompleteCount = %d",
              pTLCb->uResCount, pTLCb->usPendingTxCompleteCount));

  return VOS_STATUS_SUCCESS;
}/*               */

/*                                                                          

                                  

             
                                                                           
                                       

              
                                                             
                                                                          
                                                                           
                                       
   
            

      
    
                                            
                                      
                                           
                                                                
                                                                 

              
                                                            

                                                                             
                                                  
                                               

              

                                                                            */
static VOS_STATUS
WLANTL_CacheSTAFrame
(
  WLANTL_CbType*    pTLCb,
  v_U8_t            ucSTAId,
  vos_pkt_t*        vosTempBuff,
  v_U32_t           uDPUSig,
  v_U8_t            bBcast,
  v_U8_t            ucFrmType
)
{
  v_U8_t    ucUcastSig;
  v_U8_t    ucBcastSig;
  v_BOOL_t  bOldSTAPkt;
  WLANTL_STAClientType* pClientSTA = NULL;
  /*                                                                      */

  /*                                                                           
                  
                                                                            */ 
  if (( NULL == pTLCb ) || ( NULL == vosTempBuff ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL: Invalid input pointer on WLANTL_CacheSTAFrame TL %p"
               " Packet %p", pTLCb, vosTempBuff ));
    return VOS_STATUS_E_FAULT;
  }

  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WLAN TL:Invalid station id requested on WLANTL_CacheSTAFrame"));
    return VOS_STATUS_E_FAULT;
  }
  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pClientSTA )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Attempting to cache pkt for STA %d, BD DPU Sig: %d with sig UC: %d, BC: %d", 
             ucSTAId, uDPUSig,
             pClientSTA->wSTADesc.ucUcastSig,
             pClientSTA->wSTADesc.ucBcastSig));

  if(WLANTL_IS_CTRL_FRAME(ucFrmType))
  {
      TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLAN TL: No need to cache CTRL frame. Dropping"));
      vos_pkt_return_packet(vosTempBuff); 
      return VOS_STATUS_SUCCESS;
  }

  /*                                                                         
                                                                      
                                                                        
    
                                                                          
                    
                                                                         
                         
                                                                           */
  if ( bBcast )
  {
    ucBcastSig = (v_U8_t)uDPUSig;
    bOldSTAPkt = (( WLAN_TL_INVALID_B_SIG != 
                  pClientSTA->wSTADesc.ucBcastSig ) &&
      ( ucBcastSig == pClientSTA->wSTADesc.ucBcastSig ));
  }
  else
  {
    ucUcastSig = (v_U8_t)uDPUSig;
    bOldSTAPkt = (( WLAN_TL_INVALID_U_SIG != 
                    pClientSTA->wSTADesc.ucUcastSig ) &&
        ( ucUcastSig == pClientSTA->wSTADesc.ucUcastSig ));
  }

  /*                                                                        
                                                                     
                                                      
                                                                           
                       
                                                                           */
  if ( bOldSTAPkt || bBcast )
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:Data packet matches old sig for sig DPU: %d UC: %d, "
               "BC: %d - dropping", 
               uDPUSig, 
               pClientSTA->wSTADesc.ucUcastSig,
               pClientSTA->wSTADesc.ucBcastSig));
    vos_pkt_return_packet(vosTempBuff); 
  }
  else
  {
    if ( NULL == pClientSTA->vosBegCachedFrame )
    {
      /*                                            */
      pClientSTA->vosBegCachedFrame = vosTempBuff;
    }
    else
    {
      /*                                                                 */
      vos_pkt_chain_packet(pClientSTA->vosEndCachedFrame,
                           vosTempBuff, VOS_TRUE);
    }
    pClientSTA->vosEndCachedFrame = vosTempBuff;
  }/*               */

  return VOS_STATUS_SUCCESS; 
}/*                    */

/*                                                                          

                                      

             
                                                                   

              
                                                             
    
            

      

                                                                       
                            

              
                                                            

                                               

              

      
                                                                           
                                                                   
                                                                          
                                                                      
    
                                                                            */
static VOS_STATUS
WLANTL_FlushCachedFrames
(
  vos_pkt_t*      vosDataBuff
)
{
  /*                                                                      
                                                                    
                                                                        */
  if ( NULL != vosDataBuff )
  {
    vos_pkt_return_packet(vosDataBuff);
  }

  return VOS_STATUS_SUCCESS;  
}/*                        */

/*                                                                          

                                     

             
                                                                                    
                                                                                    
                     
     

              
                                                             
   
            

      
    
                                            
                                      

              
                                                            

                                                                         
                                    
                                               

              
                                                                           
                                                                   
                                                                          
                                                                      

                                                                            */
static VOS_STATUS
WLANTL_ForwardSTAFrames
(
  void*             pvosGCtx,
  v_U8_t            ucSTAId,
  v_U8_t            ucUcastSig,
  v_U8_t            ucBcastSig
)
{
  WLANTL_CbType*  pTLCb = NULL;
  WLANTL_STAClientType* pClientSTA = NULL;
  /*                                                                        */

  /*                                                                           
                  
                                                                            */ 
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
        "WLAN TL: Invalid input pointer on WLANTL_ForwardSTAFrames TL %p",
         pTLCb ));
    return VOS_STATUS_E_FAULT;
  }

  if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
         "WLAN TL:Invalid station id requested on WLANTL_ForwardSTAFrames"));
    return VOS_STATUS_E_FAULT;
  }

  //                                                       

  /*                                                                        
                                                               
                                            
                                                                           */ 
  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pClientSTA )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  if ( 0 == pClientSTA->ucExists )
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
       "WLAN TL:Station has been deleted for STA %d - flushing cache", ucSTAId));
    WLANTL_FlushCachedFrames(pClientSTA->vosBegCachedFrame);
    goto done; 
  }

  /*                                                                        
                                                                            
                                                             

                                                                               
                                                                               
                   
                                                                          */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
       "WLAN TL:Preparing to fwd packets for STA %d", ucSTAId));

  /*                                                                       
                                 
                                                                          */
  pClientSTA->wSTADesc.ucUcastSig  = ucUcastSig;
  pClientSTA->wSTADesc.ucBcastSig  = ucBcastSig;

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
       "WLAN TL:Fwd-ing packets for STA %d UC %d BC %d",
       ucSTAId, ucUcastSig, ucBcastSig));

  /*                                                                           
                                                        
                                                                            */ 
  if ( NULL != pClientSTA->vosBegCachedFrame )
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL: Fwd-ing Cached packets for station %d", ucSTAId ));

    WLANTL_RxCachedFrames( pTLCb, 
                           ucSTAId, 
                           pClientSTA->vosBegCachedFrame);
  }
  else
  {
     TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL: NO cached packets for station %d", ucSTAId ));
  }

done:
  /*                                                                           
                           
                                                                            */
  pClientSTA->vosBegCachedFrame = NULL;
  pClientSTA->vosEndCachedFrame = NULL;

    /*                                                                       
                                                                  
                                                                          */
  pClientSTA->ucRxBlocked = 0;

  //                                                         
  return VOS_STATUS_SUCCESS; 

}/*                       */


#if defined(FEATURE_WLAN_CCX) || defined(FEATURE_WLAN_CCX_UPLOAD)
/*                                                                          

                                

             
                                                                    

              

            

      
    
                                                    
                                           

          
                                                                       
                                       
              
                                                            

                                  
                                    

              

                                                                            */
v_BOOL_t
WLANTL_IsIAPPFrame
(
  v_PVOID_t         pvBDHeader,
  vos_pkt_t*        vosTempBuff
)
{
  v_U16_t             usMPDUDOffset;
  v_U8_t              ucOffset;
  v_U8_t              ucSnapHdr[WLANTL_LLC_SNAP_SIZE];
  v_SIZE_t            usSnapHdrSize = WLANTL_LLC_SNAP_SIZE;
  VOS_STATUS          vosStatus;

  /*                                                                       */

  /*                                                                        
                                  
                                                                           */
  if ( VOS_FALSE == WDA_IS_RX_LLC_PRESENT(pvBDHeader) )
  {
      TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                  "WLAN TL:LLC header removed, cannot determine BT-AMP type -"
                  "dropping pkt"));
      /*             */
      vos_pkt_return_packet(vosTempBuff);
      return VOS_TRUE;
  }
  usMPDUDOffset = (v_U8_t)WDA_GET_RX_MPDU_DATA_OFFSET(pvBDHeader);
  ucOffset      = (v_U8_t)usMPDUDOffset + WLANTL_LLC_SNAP_OFFSET;

  vosStatus = vos_pkt_extract_data( vosTempBuff, ucOffset,
                                (v_PVOID_t)ucSnapHdr, &usSnapHdrSize);

  if (( VOS_STATUS_SUCCESS != vosStatus)) 
  {
    TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                "Unable to extract Snap Hdr of data  packet -"
                "dropping pkt"));
    return VOS_FALSE;
  }

 /*                                                                        
                                                             
                                                                           */
  //                                           
  //                 
  if (( WLANTL_LLC_SNAP_SIZE != usSnapHdrSize ) ||
     ( 0 == vos_mem_compare(ucSnapHdr, (v_PVOID_t)WLANTL_AIRONET_SNAP_HEADER,
                            WLANTL_LLC_SNAP_SIZE ) ))
  {
    return VOS_FALSE;
  }

  return VOS_TRUE;

}
#endif //                

/*                                                                          

                                    

             
                                                                   

              
                                                             
                                                                             
                       

            

      
    
                                                    
                                           
                                            
                                      

          
                                                                       
                                       
              
                                                            

                                                      
                                                                         
                                    
                                               

              

                                                                            */
v_BOOL_t
WLANTL_ProcessBAPFrame
(
  v_PVOID_t         pvBDHeader,
  vos_pkt_t*        vosTempBuff,
  WLANTL_CbType*    pTLCb,
  v_U8_t*           pFirstDataPktArrived,
  v_U8_t            ucSTAId
)
{
  v_U16_t             usMPDUDOffset;
  v_U8_t              ucOffset;
  v_U8_t              ucOUI[WLANTL_LLC_OUI_SIZE];
  v_SIZE_t            usOUISize = WLANTL_LLC_OUI_SIZE;
  VOS_STATUS          vosStatus;
  v_U16_t             usType;
  v_SIZE_t            usTypeLen = sizeof(usType);
  v_U8_t              ucMPDUHOffset;
  v_U8_t              ucMPDUHLen = 0;
  v_U16_t             usActualHLen = 0;

  /*                                                                       */

  /*                                                                        
                                                                       
                                                                           */
  if ( VOS_FALSE == WDA_IS_RX_LLC_PRESENT(pvBDHeader) )
  {
    TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
          "WLAN TL:LLC header removed, cannot determine BT-AMP type -"
              "dropping pkt"));
    /*             */
    vos_pkt_return_packet(vosTempBuff);
    return VOS_TRUE; 
  }

  usMPDUDOffset = (v_U8_t)WDA_GET_RX_MPDU_DATA_OFFSET(pvBDHeader);
  ucMPDUHOffset = (v_U8_t)WDA_GET_RX_MPDU_HEADER_OFFSET(pvBDHeader);
  ucMPDUHLen    = (v_U8_t)WDA_GET_RX_MPDU_HEADER_LEN(pvBDHeader);
  ucOffset      = (v_U8_t)usMPDUDOffset + WLANTL_LLC_OUI_OFFSET;

  vosStatus = vos_pkt_extract_data( vosTempBuff, ucOffset,
                                (v_PVOID_t)ucOUI, &usOUISize);

#if 0
  //                                           
  //                 
  if (( WLANTL_LLC_OUI_SIZE != usOUISize ) ||
     ( 0 == vos_mem_compare(ucOUI, (v_PVOID_t)WLANTL_BT_AMP_OUI,
                            WLANTL_LLC_OUI_SIZE ) ))
  {
    TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                "LLC header points to diff OUI in BT-AMP station -"
                "dropping pkt"));
    /*             */
    vos_pkt_return_packet(vosTempBuff);
    return VOS_TRUE;
  }
#endif
  /*                                                                        
                                                                 
                                                                           */
  vosStatus = vos_pkt_extract_data( vosTempBuff,
                                 ucOffset + WLANTL_LLC_OUI_SIZE,
                                (v_PVOID_t)&usType, &usTypeLen);

  if (( VOS_STATUS_SUCCESS != vosStatus) ||
      ( sizeof(usType) != usTypeLen ))
  {
    TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                "Unable to extract type on incoming BAP packet -"
                "dropping pkt"));
    /*             */
    vos_pkt_return_packet(vosTempBuff);
    return VOS_TRUE;
  }

  /*                                                                        
                                                                            
                                                                           */
  usType = vos_be16_to_cpu(usType);

  if (WLANTL_BAP_IS_NON_DATA_PKT_TYPE(usType))
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
      "WLAN TL:Non-data packet received over BT-AMP link: %d, => BAP",
               usType));

    /*                                                */
    if ( VOS_STATUS_SUCCESS != vos_pkt_flatten_rx_pkt(&vosTempBuff))
    {
      TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                 "WLAN TL:Cannot flatten BT-AMP packet - dropping"));
      /*             */
      vos_pkt_return_packet(vosTempBuff);
      return VOS_TRUE;
    }

    /*                          */
    if ( VOS_STATUS_SUCCESS != WDA_DS_TrimRxPacketInfo( vosTempBuff ) )
    {
      TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
        "WLAN TL:BD header corrupted - dropping packet"));
      /*             */
      vos_pkt_return_packet(vosTempBuff);
      return VOS_TRUE;
    }

    if ( 0 == WDA_GET_RX_FT_DONE(pvBDHeader) )
    {
      TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
          "Non-data packet received over BT-AMP link: Sending it for "
          "frame Translation"));

      if (usMPDUDOffset > ucMPDUHOffset)
      {
        usActualHLen = usMPDUDOffset - ucMPDUHOffset;
      }

      /*                                          */
      WLANTL_Translate80211To8023Header( vosTempBuff, &vosStatus, usActualHLen,
                                         ucMPDUHLen, pTLCb,ucSTAId, VOS_FALSE);
      
    }
    if (pTLCb->tlBAPClient.pfnTlBAPRx)
        pTLCb->tlBAPClient.pfnTlBAPRx( vos_get_global_context(VOS_MODULE_ID_TL,pTLCb),
                                       vosTempBuff,
                                       (WLANTL_BAPFrameEnumType)usType );
    else
    {
        VOS_ASSERT(0);
    }

    return VOS_TRUE;
  }
  else
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
      "WLAN TL: BAP DATA packet received over BT-AMP link: %d, => BAP",
               usType));
   /*           */
 #if 0
    /*                                                                    
                                                            
                                                                        
                                                                        */
    if ( *pFirstDataPktArrived == 0)
    {
      pTLCb->atlSTAClients[ucSTAId].rssiAvg =
         WLANHAL_GET_RSSI_AVERAGE( pvBDHeader );
      pTLCb->atlSTAClients[ucSTAId].uLinkQualityAvg = 
        WLANHAL_RX_BD_GET_SNR( pvBDHeader );

      //                                           
      *pFirstDataPktArrived = 1;
    }
    else
    {
      pTLCb->atlSTAClients[ucSTAId].rssiAvg =
          (WLANHAL_GET_RSSI_AVERAGE( pvBDHeader ) + 
           pTLCb->atlSTAClients[ucSTAId].rssiAvg)/2;
      pTLCb->atlSTAClients[ucSTAId].uLinkQualityAvg =
          (WLANHAL_RX_BD_GET_SNR( pvBDHeader ) +  
           pTLCb->atlSTAClients[ucSTAId].uLinkQualityAvg)/2;
    }/*                       */
 #endif
  }/*                  */

  return VOS_FALSE; 
}/*                      */


/*                                                                          

                                   

             
                                                                                 
                                                 

              
                                                             
                                           

            

      
                                                         
                                                    
                                            
                                                 

          
                                                                       
                                       
              
                                                            

                                                 
                                                                         
                                    
                                               

              
                                                                                    
                                                               

                                                                            */
v_BOOL_t
WLANTL_ProcessFCFrame
(
  v_PVOID_t         pvosGCtx,
  vos_pkt_t*        pvosDataBuff,
  v_PVOID_t         pvBDHeader
)
{
#if 1 //                                                                                               
      //                                                      
  v_U8_t            ucSTAId;
  v_U16_t           ucStaValidBitmap;
  v_U16_t           ucStaTxDisabledBitmap;
  WLANTL_CbType*    pTLCb = NULL;
  #ifdef TL_DEBUG_FC
  v_U32_t           rxTimeStamp;
  v_U32_t           curTick;
  #endif
  /*                                                                        
                           
                                                                          */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_SuspendDataTx"));
    return VOS_STATUS_E_FAULT;
  }
  ucStaValidBitmap = WDA_GET_RX_FC_VALID_STA_MASK(pvBDHeader);
  ucStaTxDisabledBitmap = WDA_GET_RX_FC_STA_TX_DISABLED_BITMAP(pvBDHeader);
#ifdef TL_DEBUG_FC
  rxTimeStamp = WDA_GET_RX_TIMESTAMP(pvBDHeader);
  /*                                                                                          */
  wpalReadRegister(0x03081400+0x1D4, &curTick);

  TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "%ld (%ld-%ld): Disabled %x Valid %x\n", curTick > rxTimeStamp ? curTick - rxTimeStamp : rxTimeStamp - (0xFFFFFFFF - curTick),
    curTick, rxTimeStamp,  ucStaTxDisabledBitmap, ucStaValidBitmap));
#endif
  for(ucSTAId = 0; ucStaValidBitmap != 0; ucStaValidBitmap >>=1, ucStaTxDisabledBitmap >>= 1, ucSTAId ++)
  {
    if ( (0 == (ucStaValidBitmap & 0x1)) || (pTLCb->atlSTAClients[ucSTAId] && (0 == pTLCb->atlSTAClients[ucSTAId]->ucExists)) )
        continue;

    if (ucStaTxDisabledBitmap & 0x1)
    {
      WLANTL_SuspendDataTx(pvosGCtx, &ucSTAId, NULL);
    }
    else
    {
      WLANTL_ResumeDataTx(pvosGCtx, &ucSTAId);
    }
  }

#else
  VOS_STATUS          vosStatus;
  tpHalFcRxBd         pvFcRxBd = NULL;
  v_U8_t              ucBitCheck = 0x1;
  v_U8_t              ucStaValid = 0;
  v_U8_t              ucSTAId = 0;

      VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
                 "Received FC Response");
  if ( (NULL == pTLCb) || (NULL == pvosDataBuff))
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
        "WLAN TL:Invalid pointer in %s \n", __func__));
    return VOS_STATUS_E_FAULT;
  }
  vosStatus = vos_pkt_peek_data( pvosDataBuff, 0, (v_PVOID_t)&pvFcRxBd,
                                   sizeof(tHalFcRxBd));

  if ( (VOS_STATUS_SUCCESS != vosStatus) || (NULL == pvFcRxBd) )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "WLAN TL:wrong FC Rx packet"));
      return VOS_STATUS_E_INVAL;
  }
  
  //                                         
  WLANHAL_SwapFcRxBd(&pvFcRxBd->fcSTATxQLen[0]);

  //                                                 
  for( ucStaValid = (v_U8_t)pvFcRxBd->fcSTAValidMask; ucStaValid; ucStaValid >>= 1, ucBitCheck <<= 1, ucSTAId ++)
  {
    if ( (0 == (ucStaValid & 0x1)) || (0 == pTLCb->atlSTAClients[ucSTAId].ucExists) )
    {
      continue;
    }

    if ( pvFcRxBd->fcSTAThreshIndMask & ucBitCheck )
    {
      //                                                      
      if( pTLCb->atlSTAClients[ucSTAId].ucLwmModeEnabled )
      {
        //                                                             
        pTLCb->atlSTAClients[ucSTAId].ucLwmEventReported = TRUE;
      }
      else
      {
        TLLOG4(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_LOW,
                 "WLAN TL: FW report LWM event but the station %d is not in LWM mode \n", ucSTAId));
      }
    }

    //                                                                            
    if (WLANTL_LWM_INGRESS_SAMPLE_THRESHOLD <= pTLCb->atlSTAClients[ucSTAId].uIngress_length)
    {
      //                                                                                 
      v_U32_t uEgress_length = pTLCb->atlSTAClients[ucSTAId].uIngress_length + 
        pTLCb->atlSTAClients[ucSTAId].bmuMemConsumed - pvFcRxBd->fcSTATxQLen[ucSTAId];

      //                                                                               
      //                                             
      if ( (pTLCb->atlSTAClients[ucSTAId].uIngress_length > uEgress_length) &&
           ((pTLCb->atlSTAClients[ucSTAId].uIngress_length - uEgress_length ) >= 
            (pTLCb->atlSTAClients[ucSTAId].uIngress_length >> 2))
         )
      {   
         TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                    "WLAN TL:Enable LWM mode for station %d\n", ucSTAId));
         pTLCb->atlSTAClients[ucSTAId].ucLwmModeEnabled = TRUE;
      }
      else
      {
        if( pTLCb->atlSTAClients[ucSTAId].ucLwmModeEnabled )
        {
          TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                    "WLAN TL:Disable LWM mode for station %d\n", ucSTAId));
          pTLCb->atlSTAClients[ucSTAId].ucLwmModeEnabled = FALSE;
        }

      }

      //                                                    
      pTLCb->atlSTAClients[ucSTAId].bmuMemConsumed = pvFcRxBd->fcSTATxQLen[ucSTAId];
      pTLCb->atlSTAClients[ucSTAId].uIngress_length = 0;
    } //                                                                                      

    if( pTLCb->atlSTAClients[ucSTAId].ucLwmModeEnabled )
    {
      //                                                   
      pTLCb->atlSTAClients[ucSTAId].uBuffThresholdMax =  WLANTL_STA_BMU_THRESHOLD_MAX -
        pvFcRxBd->fcSTATxQLen[ucSTAId];
    }

  }
#endif

  return VOS_STATUS_SUCCESS;
}


/*                                                                          

                             

             
                                                                         
                                                                           
                                                                         
                                          

              
                                                             
                                                                         
                                                             

            

      
                                                                       
                                                                            

                                                                          
                                                                         
                                       

              
                                                            

                                                      
                                                                         
                                    
                                               

              

                                                                            */
VOS_STATUS
WLANTL_RxFrames
(
  v_PVOID_t      pvosGCtx,
  vos_pkt_t     *pFrameDataBuff
)
{
  vos_pkt_t*          vosDataBuff = (vos_pkt_t*)pFrameDataBuff;
  WLANTL_CbType*      pTLCb = NULL;
  WLANTL_STAClientType* pClientSTA = NULL;
  WLANTL_STAFuncType  pfnSTAFsm;
  vos_pkt_t*          vosTempBuff;
  v_U8_t              ucSTAId;
  VOS_STATUS          vosStatus;
  v_U8_t              ucFrmType;
  v_PVOID_t           pvBDHeader = NULL;
  WLANTL_STAEventType wSTAEvent  = WLANTL_RX_EVENT;
  v_U8_t              ucTid      = 0;
  v_BOOL_t            broadcast  = VOS_FALSE;
  v_BOOL_t            selfBcastLoopback = VOS_FALSE;
  static v_U8_t       first_data_pkt_arrived;
  v_U32_t             uDPUSig; 
  v_U16_t             usPktLen;
#ifdef FEATURE_WLAN_TDLS_INTERNAL 
  v_U8_t              ucMPDUHLen = 0 ;
  v_U16_t             usEtherType = 0;
#endif
  v_BOOL_t            bForwardIAPPwithLLC = VOS_FALSE;

  /*                                                                       */

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:TL Receive Frames called"));

  /*                                                                        
                
                                                                            */
  if ( NULL == vosDataBuff )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_RxFrames"));
    return VOS_STATUS_E_INVAL;
  }

 /*                                                                        
                                                      
                                                                          */
   vosDataBuff->timestamp = vos_timer_get_system_ticks();

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
        "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ChangeSTAState"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                     
                                                               
                                                                        */
  vosTempBuff = vosDataBuff;

  while ( NULL != vosTempBuff )
  {
    broadcast = VOS_FALSE;
    selfBcastLoopback = VOS_FALSE; 

    vos_pkt_walk_packet_chain( vosDataBuff, &vosDataBuff, 1/*    */ );

    /*                                                                     
                                       
                                                                    
                                                                          */
    vosStatus = WDA_DS_PeekRxPacketInfo( vosTempBuff, (v_PVOID_t)&pvBDHeader, 1/*       */ );

    if ( NULL == pvBDHeader )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "WLAN TL:Cannot extract BD header"));
      /*             */
      vos_pkt_return_packet(vosTempBuff);
      vosTempBuff = vosDataBuff;
      continue;
    }

    /*                                                                     
                                        
                                                                         */
    if(WDA_IS_RX_FC(pvBDHeader))
    {
      TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLAN TL:receive one FC frame"));

      WLANTL_ProcessFCFrame(pvosGCtx, vosTempBuff, pvBDHeader);
      /*             */
      vos_pkt_return_packet(vosTempBuff);
      vosTempBuff = vosDataBuff;
      continue;
    }

    /*                 
                                                                
                                */
    if(WDA_GET_RX_ASF(pvBDHeader))
    {
      WDA_DS_RxAmsduBdFix(pvosGCtx, pvBDHeader);
    }

    /*                                                                     
                                                                
                                   
                                                                         */

    vosStatus = WDA_DS_GetFrameTypeSubType( pvosGCtx, vosTempBuff,
                         pvBDHeader, &ucFrmType );
    if ( VOS_STATUS_SUCCESS != vosStatus )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                   "WLAN TL:Cannot extract Frame Control Field"));
      /*             */
      vos_pkt_return_packet(vosTempBuff);
      vosTempBuff = vosDataBuff;
      continue;
    }

#ifdef FEATURE_WLAN_TDLS_INTERNAL
    if ( WLANTL_IS_DATA_FRAME(ucFrmType))
    {
       ucMPDUHLen    = (v_U8_t)WDA_GET_RX_MPDU_HEADER_LEN(pvBDHeader);
       WLANTL_GetEtherType_2(pvBDHeader, vosTempBuff, ucMPDUHLen, &usEtherType) ;
    }
#endif
    vos_pkt_get_packet_length(vosTempBuff, &usPktLen);

    /*                                                                     
                                        
                                                                         */

    if ( WLANTL_IS_MGMT_FRAME(ucFrmType) 
#ifdef FEATURE_WLAN_TDLS_INTERNAL
        || (WLANTL_IS_TDLS_FRAME(usEtherType)) 
#endif
       )
    {
      TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLAN TL:Sending packet to management client"));
      if ( VOS_STATUS_SUCCESS != vos_pkt_flatten_rx_pkt(&vosTempBuff))
      {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                   "WLAN TL:Cannot flatten packet - dropping"));
        /*             */
        vos_pkt_return_packet(vosTempBuff);
        vosTempBuff = vosDataBuff;
        continue;
      }
      ucSTAId = (v_U8_t)WDA_GET_RX_STAID( pvBDHeader );
      /*                      */
      if(!WLANTL_STA_ID_INVALID(ucSTAId))
      {
#if defined WLAN_FEATURE_NEIGHBOR_ROAMING
        /*                      */
        vosStatus = WLANTL_HSHandleRXFrame(pvosGCtx,
                                           WLANTL_MGMT_FRAME_TYPE,
                                           pvBDHeader,
                                           ucSTAId,
                                           VOS_FALSE,
                                           NULL);
#else
        vosStatus = WLANTL_ReadRSSI(pvosGCtx, pvBDHeader, ucSTAId);
#endif
        if (!VOS_IS_STATUS_SUCCESS(vosStatus))
        {
          TLLOGW(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                           "Handle RX Management Frame fail within Handoff "
                           "support module"));
          /*                           
                                              
                                                  
                                         
                        
           */
        }
        vosStatus = WLANTL_ReadSNR(pvosGCtx, pvBDHeader, ucSTAId);

        if (!VOS_IS_STATUS_SUCCESS(vosStatus))
        {
          TLLOGW(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                            FL("Failed to Read SNR")));
        }
      }

      pTLCb->tlMgmtFrmClient.pfnTlMgmtFrmRx( pvosGCtx, vosTempBuff); 
    }
    else /*            */
    {
      ucSTAId = (v_U8_t)WDA_GET_RX_STAID( pvBDHeader );
      ucTid   = (v_U8_t)WDA_GET_RX_TID( pvBDHeader );

      TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLAN TL:Data packet received for STA %d", ucSTAId));

      /*                                                                  
                                                                      
                                                                    
                                                                         */
      if ( WDA_IS_RX_BCAST(pvBDHeader))
      {
        TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
        "WLAN TL:TL rx Bcast frame - sending to last registered station"));
        broadcast = VOS_TRUE;
        
        /*                                                                   
                                                                             
                                                                              
                                                                            */ 
        if( WLANHAL_RX_BD_ADDR3_SELF_IDX == 
            (v_U8_t)WDA_GET_RX_ADDR3_IDX( pvBDHeader )) 
        {
          selfBcastLoopback = VOS_TRUE; 
        }
      }/*        */

      if ( WLANTL_STA_ID_INVALID(ucSTAId) )
      {
        TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                   "WLAN TL:STA ID invalid - dropping pkt"));
        /*             */
        vos_pkt_return_packet(vosTempBuff);
        vosTempBuff = vosDataBuff;
        continue;
      }

      /*                                                                      
                                                                            
                                            
                                                                           
                                                  
                                                                         
                                       
                                                                              
                                                   
                                                                             */
      pClientSTA = pTLCb->atlSTAClients[ucSTAId];
      if (NULL == pClientSTA)
      {
          TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                   "WLAN TL:STA not allocated memory. Dropping packet"));
          vos_pkt_return_packet(vosTempBuff);
          vosTempBuff = vosDataBuff;
          continue;
      }

#ifdef FEATURE_WLAN_TDLS
      if (( pClientSTA->ucExists ) &&
           (WLAN_STA_TDLS == pClientSTA->wSTADesc.wSTAType) &&
           (pClientSTA->ucTxSuspended))
          vos_atomic_set_U8( &pClientSTA->ucTxSuspended, 0 );
      else if ( !broadcast && (pClientSTA->ucExists == 0 ) )
      {
          tpSirMacMgmtHdr pMacHeader = WDA_GET_RX_MAC_HEADER( pvBDHeader );

          /*                                                           */
          if ( (pMacHeader->fc.fromDS == 0) &&
               (pMacHeader->fc.toDS == 0) )
          {
              v_U8_t ucAddr3STAId;

              ucAddr3STAId = WDA_GET_RX_ADDR3_IDX(pvBDHeader);

              if ( WLANTL_STA_ID_INVALID(ucAddr3STAId) )
              {
                TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                           "WLAN TL:STA ID %d invalid - dropping pkt", ucAddr3STAId));
                /*             */
                vos_pkt_return_packet(vosTempBuff);
                vosTempBuff = vosDataBuff;
                continue;
              }

              if (!(pTLCb->atlSTAClients[ucAddr3STAId] && pTLCb->atlSTAClients[ucAddr3STAId]->ucExists &&
                  (WLAN_STA_INFRA == pTLCb->atlSTAClients[ucAddr3STAId]->wSTADesc.wSTAType) &&
                  (WLANTL_STA_AUTHENTICATED == pTLCb->atlSTAClients[ucAddr3STAId]->tlState)))
              {
                  TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                           "%s: staId %d addr3Id %d tlState %d. Unkown Receiver/Transmitter Dropping packet\n", __func__,
                           ucSTAId, ucAddr3STAId, pTLCb->atlSTAClients[ucAddr3STAId]->tlState));
                  vos_pkt_return_packet(vosTempBuff);
                  vosTempBuff = vosDataBuff;
                  continue;
              }
              else
              {
                  TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                           "%s: staId %d doesn't exist, but mapped to AP staId %d \n", __func__,
                           ucSTAId, ucAddr3STAId));
                  ucSTAId = ucAddr3STAId;
                  pClientSTA = pTLCb->atlSTAClients[ucAddr3STAId];
              }
          }
      }
#endif

      if ((( 0 == pClientSTA->ucExists ) ||
          ( (0 != pClientSTA->ucRxBlocked)
            //                                                           
            && (WLAN_STA_SOFTAP != pClientSTA->wSTADesc.wSTAType)
          ) ||
          ( WLANTL_STA_DISCONNECTED == pClientSTA->tlState)) &&
            /*                                                                                                   
                                                                                           
                                                                         */
          ((WDA_IS_RX_BCAST(pvBDHeader) == 0)))
      {
        uDPUSig = WDA_GET_RX_DPUSIG( pvBDHeader );
          //                                                             
        TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                 "%s: staId %d exist %d tlState %d cache rx frame \n", __func__, ucSTAId,
                 pClientSTA->ucExists, pClientSTA->tlState));
        WLANTL_CacheSTAFrame( pTLCb, ucSTAId, vosTempBuff, uDPUSig, broadcast, ucFrmType);
        vosTempBuff = vosDataBuff;
        continue;
      }

#ifdef FEATURE_WLAN_CCX_UPLOAD
      if ((pClientSTA->wSTADesc.ucIsCcxSta)|| broadcast)
      {
        /*                                                                    
                                                    
                                             
                                                      
                                                                            */
        if ( WLANTL_IsIAPPFrame(pvBDHeader,
                                vosTempBuff))
        {
            bForwardIAPPwithLLC = VOS_TRUE;
        }
      }
#endif

#if defined(FEATURE_WLAN_CCX) && !defined(FEATURE_WLAN_CCX_UPLOAD)
      if ((pClientSTA->wSTADesc.ucIsCcxSta)|| broadcast)
      {
        /*                                                                    
                                                     
                                              
                                                      
                                                                            */
        if ( WLANTL_IsIAPPFrame(pvBDHeader,
                                vosTempBuff))
        {
            if ( VOS_STATUS_SUCCESS != vos_pkt_flatten_rx_pkt(&vosTempBuff))
            {
               TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                        "WLAN TL:Cannot flatten packet - dropping"));
               /*             */
               vos_pkt_return_packet(vosTempBuff);
            } else {

               TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
                        "WLAN TL: Received CCX IAPP Frame"));

               pTLCb->tlMgmtFrmClient.pfnTlMgmtFrmRx( pvosGCtx, vosTempBuff); 
            }
            vosTempBuff = vosDataBuff;
            continue;
        }
      }
#endif  /*                                                                */

      if ( WLAN_STA_BT_AMP == pClientSTA->wSTADesc.wSTAType )
      {
        /*                                                                    
                                                                          
                                                      
                                                                            */
        if ( WLANTL_ProcessBAPFrame( pvBDHeader,
                                     vosTempBuff,
                                     pTLCb,
                                    &first_data_pkt_arrived,
                                     ucSTAId))
        {
          vosTempBuff = vosDataBuff;
          continue;
        }
      }/*                 */
      else if(selfBcastLoopback == VOS_TRUE)
      { 
        /*             */ 
        vos_pkt_return_packet(vosTempBuff); 
        vosTempBuff = vosDataBuff; 
        continue; 
      } 
      
      /*                                                                     
                                                   
                                                                           */
      wSTAEvent = WLANTL_RX_EVENT;

      pfnSTAFsm = tlSTAFsm[pClientSTA->tlState].
                      pfnSTATbl[wSTAEvent];

      if ( NULL != pfnSTAFsm )
      {
#if defined WLAN_FEATURE_NEIGHBOR_ROAMING
        /*                      */
        vosStatus = WLANTL_HSHandleRXFrame(pvosGCtx,
                                           WLANTL_DATA_FRAME_TYPE,
                                           pvBDHeader,
                                           ucSTAId,
                                           broadcast,
                                           vosTempBuff);
        broadcast = VOS_FALSE;
#else
        vosStatus = WLANTL_ReadRSSI(pvosGCtx, pvBDHeader, ucSTAId);
#endif /*                         */
        if (!VOS_IS_STATUS_SUCCESS(vosStatus))
        {
          TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
            "Handle RX Data Frame fail within Handoff support module"));
          /*                            
                                              
                                                
                                       
                      
           */
        }
        vosStatus = WLANTL_ReadSNR(pvosGCtx, pvBDHeader, ucSTAId);

        if (!VOS_IS_STATUS_SUCCESS(vosStatus))
        {
          TLLOGW(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                           FL("Failed to Read SNR")));
        }

        pfnSTAFsm( pvosGCtx, ucSTAId, &vosTempBuff, bForwardIAPPwithLLC);
      }
      else
        {
          TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
            "WLAN TL:NULL state function, STA:%d, State: %d- dropping packet",
                   ucSTAId, pClientSTA->tlState));
          /*             */
          vos_pkt_return_packet(vosTempBuff);
          vosTempBuff = vosDataBuff;
          continue;
        }
    }/*                */

    vosTempBuff = vosDataBuff;
  }/*           */

  return VOS_STATUS_SUCCESS;
}/*                 */


/*                                                                          

                                   

             
                                                                            
             

              
                                                             
                                                                         
                                                             

            

      
                                  
   
                                                                

                                                                       
                                                                          
                                              

              
                                                            

                                                      
                                                                         
                                    
                                               

              

                                                                            */
VOS_STATUS
WLANTL_RxCachedFrames
(
  WLANTL_CbType*  pTLCb,
  v_U8_t          ucSTAId,
  vos_pkt_t*      vosDataBuff
)
{
  WLANTL_STAClientType* pClientSTA = NULL;
  WLANTL_STAFuncType  pfnSTAFsm;
  vos_pkt_t*          vosTempBuff;
  VOS_STATUS          vosStatus;
  v_PVOID_t           pvBDHeader = NULL;
  WLANTL_STAEventType wSTAEvent  = WLANTL_RX_EVENT;
  v_U8_t              ucTid      = 0;
  v_BOOL_t            broadcast  = VOS_FALSE;
  v_BOOL_t            bSigMatch  = VOS_FALSE; 
  v_BOOL_t            selfBcastLoopback = VOS_FALSE;
  static v_U8_t       first_data_pkt_arrived;
  v_U32_t             uDPUSig; 
  v_U8_t              ucUcastSig; 
  v_U8_t              ucBcastSig; 
  /*                                                                       */

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:TL Receive Cached Frames called"));

  /*                                                                        
                
                                                                            */
  if ( NULL == vosDataBuff )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_RxFrames"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                     
                                                               
                                                                        */
  vosTempBuff = vosDataBuff;

  while ( NULL != vosTempBuff )
  {
    broadcast = VOS_FALSE;
    selfBcastLoopback = VOS_FALSE; 

    vos_pkt_walk_packet_chain( vosDataBuff, &vosDataBuff, 1/*    */ );

          TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:Sending new cached packet to station %d", ucSTAId));
    /*                                                                     
                                       
                                                                    
                                                                          */
    vosStatus = WDA_DS_PeekRxPacketInfo( vosTempBuff, (v_PVOID_t)&pvBDHeader, 0 );

    if ( NULL == pvBDHeader )
          {
            TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "WLAN TL:Cannot extract BD header"));
          /*             */
          vos_pkt_return_packet(vosTempBuff);
          vosTempBuff = vosDataBuff;
          continue;
        }

    uDPUSig = WDA_GET_RX_DPUSIG( pvBDHeader );

    /*                 
                                                                
                                */
    if(WDA_GET_RX_ASF(pvBDHeader))
    {
      WDA_DS_RxAmsduBdFix(vos_get_global_context(VOS_MODULE_ID_TL,pTLCb), 
                           pvBDHeader);
    }

    ucTid   = (v_U8_t)WDA_GET_RX_TID( pvBDHeader );

    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:Data packet cached for STA %d", ucSTAId);

    /*                                                                  
                                                                    
                                                                  
                                                                       */
    if ( WDA_IS_RX_BCAST(pvBDHeader))
    {
          TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLAN TL:TL rx Bcast frame "));
      broadcast = VOS_TRUE;

      /*                                                                     
                                                                            
       */ 
      if( WLANHAL_RX_BD_ADDR3_SELF_IDX == 
          (v_U8_t)WDA_GET_RX_ADDR3_IDX( pvBDHeader )) 
      {
        selfBcastLoopback = VOS_TRUE; 
      }
    }/*        */

     /*                                                                         
                                                                         
                          
                                                                             */
    pClientSTA = pTLCb->atlSTAClients[ucSTAId];


    if ( NULL == pClientSTA )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
    }

    if ( broadcast )
    {
      ucBcastSig = (v_U8_t)uDPUSig;
      bSigMatch = (( WLAN_TL_INVALID_B_SIG != 
                    pClientSTA->wSTADesc.ucBcastSig ) &&
        ( ucBcastSig == pClientSTA->wSTADesc.ucBcastSig ));
    }
    else
    {
      ucUcastSig = (v_U8_t)uDPUSig;
      bSigMatch = (( WLAN_TL_INVALID_U_SIG != 
                      pClientSTA->wSTADesc.ucUcastSig ) &&
          ( ucUcastSig == pClientSTA->wSTADesc.ucUcastSig ));
    }

    /*                                                                         
                                            
                                                                             */
    if ( !bSigMatch )
    {
            TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_MED,
        "WLAN TL: Cached packet does not match DPU Sig of the new STA - drop "
        " DPU Sig %d  UC %d BC %d B %d",
        uDPUSig,
        pClientSTA->wSTADesc.ucUcastSig,
        pClientSTA->wSTADesc.ucBcastSig,
        broadcast));

      /*             */ 
      vos_pkt_return_packet(vosTempBuff); 
      vosTempBuff = vosDataBuff; 
      continue; 

    }/*                     */

    /*                                                                        
                            
                                                                         
                              
                                                                            */
    if ( WLAN_STA_BT_AMP == pClientSTA->wSTADesc.wSTAType )
    {
      /*                                                                    
                                                                        
                                                    
                                                                          */
      if ( WLANTL_ProcessBAPFrame( pvBDHeader,
                                   vosTempBuff,
                                   pTLCb,
                                  &first_data_pkt_arrived,
                                   ucSTAId))
      {
          vosTempBuff = vosDataBuff;
          continue;
        }
      }/*                 */
      else if(selfBcastLoopback == VOS_TRUE)
      { 
        /*             */ 
        vos_pkt_return_packet(vosTempBuff); 
        vosTempBuff = vosDataBuff; 
        continue; 
      } 
      
      /*                                                                     
                                                   
                                                                           */
      wSTAEvent = WLANTL_RX_EVENT;

      pfnSTAFsm = tlSTAFsm[pClientSTA->tlState].
                      pfnSTATbl[wSTAEvent];

      if ( NULL != pfnSTAFsm )
      {
#if defined WLAN_FEATURE_NEIGHBOR_ROAMING
        /*                      */
        vosStatus = WLANTL_HSHandleRXFrame(vos_get_global_context(
                                         VOS_MODULE_ID_TL,pTLCb),
                                           WLANTL_DATA_FRAME_TYPE,
                                           pvBDHeader,
                                           ucSTAId,
                                           broadcast,
                                           vosTempBuff);
        broadcast = VOS_FALSE;
#else
        vosStatus = WLANTL_ReadRSSI(vos_get_global_context(VOS_MODULE_ID_TL,pTLCb), pvBDHeader, ucSTAId);
#endif /*                         */
        if(!VOS_IS_STATUS_SUCCESS(vosStatus))
        {
          TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "Handle RX Data Frame fail within Handoff support module"));
          /*                            
                                              
                                             
                                    
                   
           */
        }
        pfnSTAFsm( vos_get_global_context(VOS_MODULE_ID_TL,pTLCb), ucSTAId, 
                 &vosTempBuff, VOS_FALSE);
      }
      else
      {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:NULL state function, STA:%d, State: %d- dropping packet",
                   ucSTAId, pClientSTA->tlState));
        /*             */
        vos_pkt_return_packet(vosTempBuff);
        vosTempBuff = vosDataBuff;
        continue;
      }

    vosTempBuff = vosDataBuff;
  }/*           */

  return VOS_STATUS_SUCCESS;
}/*                       */

/*                                                                          
                               

             
                                                                    

              
                                                                          

            

      
                                                                       
                                                                            

              
                                                            

                                                                         
                                    
                                               

              

                                                                            */
VOS_STATUS
WLANTL_ResourceCB
(
  v_PVOID_t       pvosGCtx,
  v_U32_t         uCount
)
{
   WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
        "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ChangeSTAState"));
    return VOS_STATUS_E_FAULT;
  }

  pTLCb->uResCount = uCount;


  /*                                                                       
                                             
                                                                          */
  if (( pTLCb->uResCount >=  WDA_TLI_MIN_RES_MF ) &&
      ( 0 == pTLCb->ucTxSuspended ))
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
         "WLAN TL:Issuing Xmit start request to BAL for avail res ASYNC"));
    return WDA_DS_StartXmit(pvosGCtx);
  }

  return VOS_STATUS_SUCCESS;
}/*                   */


/*                                                                          
                                   

             
                                                                            
                               

              
                                                                          

            

      
                                                                       
                                                                            

              
                                                            

                                             
                                                                  

              

                                                                            */
v_BOOL_t
WLANTL_IsTxXmitPending
(
  v_PVOID_t       pvosGCtx
)
{

   WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
        "WLAN TL:Invalid TL pointer from pvosGCtx in WLANTL_IsTxXmitPending "));
    return FALSE;
  }

  return pTLCb->isTxTranmitMsgPending;

}/*                       */

/*                                                                          
                                    

             
                                                                             
                               

              
                                                                          

            

      
                                                                       
                                                                            

                        

              

                                                                            */

v_VOID_t
WLANTL_SetTxXmitPending
(
  v_PVOID_t       pvosGCtx
)
{

   WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
        "WLAN TL:Invalid TL pointer from pvosGCtx in WLANTL_SetTxXmitPending"));
    return;
  }

  pTLCb->isTxTranmitMsgPending = 1;
  return;

}/*                         */

/*                                                                          
                                      

             
                                                                                
                               

              
                                                                          

            

      
                                                                       
                                                                            

                       

              

                                                                            */

v_VOID_t
WLANTL_ClearTxXmitPending
(
  v_PVOID_t       pvosGCtx
)
{

   WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
        "WLAN TL:Invalid TL pointer from pvosGCtx in WLANTL_ClearTxXmitPending "));
    return;
  }

  pTLCb->isTxTranmitMsgPending = 0;
  return;
}/*                          */

/*                                                                          
                                        

             
                                                                          
                                                                             
                                                        

              
                                                       

            

      
                                                                       
                                                                            

                        

              

                                                                            */

v_VOID_t
WLANTL_TxThreadDebugHandler
(
 v_PVOID_t *pVosContext
)
{
   WLANTL_CbType* pTLCb = NULL;
   WLANTL_STAClientType* pClientSTA = NULL;
   int i = 0;
   tWDA_CbContext *pWDA = NULL;

   TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
        "WLAN TL: %s Enter ", __func__));

   pTLCb = VOS_GET_TL_CB(pVosContext);
   pWDA = (tWDA_CbContext *)vos_get_global_context(VOS_MODULE_ID_WDA, pVosContext);

   if ( NULL == pVosContext || NULL == pTLCb )
   {
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                        "Global VoS Context or TL Context are NULL"));
        return;
   }

   if (NULL != pWDA)
   {
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WDA uTxFlowMask: %d", pWDA->uTxFlowMask));
   }
   TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "************************TL DUMP INFORMATION**************"));

   TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "uDelayedTriggerFrmInt:%d\tuMinFramesProcThres:%d",
          pTLCb->tlConfigInfo.uDelayedTriggerFrmInt,
          pTLCb->tlConfigInfo.uMinFramesProcThres));

   TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "Management Frame Client exists: %d",
          pTLCb->tlMgmtFrmClient.ucExists));
   TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "usPendingTxCompleteCount: %d\tucTxSuspended: %d",
          pTLCb->usPendingTxCompleteCount,
          pTLCb->ucTxSuspended));

   TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "uResCount: %d", pTLCb->uResCount));

   TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "ucRegisteredStaId: %d\tucCurrentSTA: %d",
          pTLCb->ucRegisteredStaId, pTLCb->ucCurrentSTA));

   TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "UrgentFrameProcessing: %s\tuFramesProcThres: %d",
          (pTLCb->bUrgent?"True":"False"), pTLCb->uFramesProcThres));

   TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "isTxTranmitMsgPending: %d\t isBMPS: %s",
          pTLCb->isTxTranmitMsgPending, pTLCb->isBMPS?"True":"False"));

#ifdef FEATURE_WLAN_TDLS
   TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "TDLS Peer Count: %d", pTLCb->ucTdlsPeerCount));
#endif

   TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "++++++++++++++++++++Registerd Client Information++++++++++"));

   for ( i =0; i<WLAN_MAX_STA_COUNT; i++ )
   {
        pClientSTA = pTLCb->atlSTAClients[i];
        if( NULL == pClientSTA || 0 == pClientSTA->ucExists)
        {
                continue;
        }

        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "######################STA Index: %d ############################",i));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR, "WLAN_STADescType:"));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "STAId: %d\t STA MAC Address: %pM", pClientSTA->wSTADesc.ucSTAId,
              pClientSTA->wSTADesc.vSTAMACAddress.bytes));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "STA Type: %d\tProtectedFrame: %d",
              pClientSTA->wSTADesc.wSTAType, pClientSTA->wSTADesc.ucProtectedFrame));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "QoS: %d\tRxFrameTrans: %d\tTxFrameTrans: %d",
               pClientSTA->wSTADesc.ucQosEnabled, pClientSTA->wSTADesc.ucSwFrameRXXlation,
               pClientSTA->wSTADesc.ucSwFrameTXXlation));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "ucUcastSig: %d\tucBcastSig: %d", pClientSTA->wSTADesc.ucUcastSig,
              pClientSTA->wSTADesc.ucBcastSig));

        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "ClientIndex: %d\t Exists: %d", i, pClientSTA->ucExists));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "TL State: %d\t TL Priority: %d", pClientSTA->tlState,
              pClientSTA->tlPri));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "ucTxSuspended: %d\tucPktPending: %d", pClientSTA->ucTxSuspended,
              pClientSTA->ucPktPending));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "ucEAPOLPktPending: %d\tucNoMoreData: %d",
              pClientSTA->ucEapolPktPending, pClientSTA->ucNoMoreData));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "ucRxBlocked: %d\t fcStaTxDisabled: %d", pClientSTA->ucRxBlocked,
               pClientSTA->fcStaTxDisabled));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "ucCurrentAC: %d\tucServicedAC: %d", pClientSTA->ucCurrentAC,
               pClientSTA->ucServicedAC));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "TID: %d\tautTxCount[0]: %d\tauRxCount[0]: %d",0, pClientSTA->auTxCount[0],
              pClientSTA->auRxCount[0]));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "aucAcMask[0]: %d\taucAcMask[1]: %d\taucAcMask[2]: %d\taucAcMask[3]: %d\t",
              pClientSTA->aucACMask[0], pClientSTA->aucACMask[1],
              pClientSTA->aucACMask[2], pClientSTA->aucACMask[3]));
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "ucCurrentWeight: %d", pClientSTA->ucCurrentWeight));

        if( WLAN_STA_SOFTAP == pClientSTA->wSTADesc.wSTAType)
        {
            TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "TrafficStatistics for SOFTAP Station:"));
            TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "RUF=%d\tRMF=%d\tRBF=%d", pClientSTA->trafficStatistics.rxUCFcnt,
                                            pClientSTA->trafficStatistics.rxMCFcnt,
                                            pClientSTA->trafficStatistics.rxBCFcnt));
            TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "RUB=%d\tRMB=%d\tRBB=%d", pClientSTA->trafficStatistics.rxUCBcnt,
                                            pClientSTA->trafficStatistics.rxMCBcnt,
                                            pClientSTA->trafficStatistics.rxBCBcnt));
            TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "TUF=%d\tTMF=%d\tTBF=%d", pClientSTA->trafficStatistics.txUCFcnt,
                                            pClientSTA->trafficStatistics.txMCFcnt,
                                            pClientSTA->trafficStatistics.txBCFcnt));
            TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "TUB=%d\tTMB=%d\tTBB=%d", pClientSTA->trafficStatistics.txUCBcnt,
                                            pClientSTA->trafficStatistics.txMCBcnt,
                                            pClientSTA->trafficStatistics.txBCBcnt));
        }
    }
   return;
}

/*                                                                          
                                  

             
                                                          

              
                                                       

            

      
                                                                        

                        

              

                                                                            */

v_VOID_t
WLANTL_TLDebugMessage
(
 v_BOOL_t   displaySnapshot
)
{
   vos_msg_t vosMsg;
   VOS_STATUS status;

   if(displaySnapshot)
   {
        vosMsg.reserved = 0;
        vosMsg.bodyptr  = NULL;
        vosMsg.type     = WLANTL_TX_SNAPSHOT;

        status = vos_tx_mq_serialize( VOS_MODULE_ID_TL, &vosMsg);
        if(status != VOS_STATUS_SUCCESS)
        {
            TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR, "TX Msg Posting Failed with status: %d",status));
            return;
        }
   }
   return;
}

/*                                                                            
                                           
                                                                            */

/*                                                                          
                              

             
                                                                    

              
                                                                          

            

     
                                                                      
                                                                  
                                                            
                                              

              
                                                            

                                                                         
                                    
                                               

                                                                     
                                        

              

                                                                            */
VOS_STATUS
WLANTL_STATxConn
(
  v_PVOID_t     pvosGCtx,
  v_U8_t        ucSTAId,
  vos_pkt_t**   pvosDataBuff,
  v_BOOL_t      bForwardIAPPwithLLC
)
{
   v_U16_t              usPktLen;
   VOS_STATUS           vosStatus;
   v_MACADDR_t          vDestMacAddr;
   vos_pkt_t*           vosDataBuff = NULL;
   WLANTL_CbType*       pTLCb       = NULL;
   WLANTL_STAClientType* pClientSTA = NULL;
   WLANTL_MetaInfoType  tlMetaInfo;
   v_U8_t               ucTypeSubtype = 0;
   v_U8_t               ucTid;
   v_U8_t               extraHeadSpace = 0;
   v_U8_t               ucWDSEnabled = 0;
   v_U8_t               ucAC, ucACMask, i; 
   v_U8_t               txFlag = HAL_TX_NO_ENCRYPTION_MASK;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
   VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
             "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_STATxConn");
   *pvosDataBuff = NULL;
    return VOS_STATUS_E_FAULT;
  }
  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pClientSTA )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  /*                                                                   
                                                              
                                                             
                                                            
                                                                 
                                                                  
                                                                   
                                                                     
                                                              
                                   
                                                                   
                                           
                                                                     */


  //                                  
  //                                                                 

  /*                                                                        
                         
                                                                           */
#ifdef FEATURE_WLAN_TDLS
  if ((WLAN_STA_SOFTAP != pClientSTA->wSTADesc.wSTAType) &&
      (!vos_concurrent_sessions_running()) &&
      !pTLCb->ucTdlsPeerCount)
  {
#else
  if ((WLAN_STA_SOFTAP != pClientSTA->wSTADesc.wSTAType) &&
      (!vos_concurrent_sessions_running()))
  {
#endif
      ucAC = pClientSTA->ucCurrentAC;

  /*                                                                   
                                                              
                                                             
                                                            
                                                                 
                                                                  
                                                                   
                                                                     
                                                              
                                   
                                                                   
                                           
                                                                     */
      pClientSTA->aucACMask[ucAC] = 0;
  }
  else
  {
      //           
      ucAC = pTLCb->uCurServedAC;
      pClientSTA->aucACMask[ucAC] = 0;
  }

    /*                                                                   
                                                                      
                                                                               
                                               */
  //                                                                        
  //                                                    
  vos_atomic_set_U8( &pClientSTA->ucPktPending, 0);
  WLAN_TL_AC_ARRAY_2_MASK( pClientSTA, ucACMask, i);
    /*                                                                   
                                                                           
                                                                                     
                                                     */
  if ( 0 == ucACMask )
  {
      pClientSTA->ucNoMoreData = 1;
  }
  else
  {
      vos_atomic_set_U8( &pClientSTA->ucPktPending, 1);
  }


  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
            "WLAN TL: WLANTL_STATxConn fetching packet from HDD for AC: %d AC Mask: %d Pkt Pending: %d", 
             ucAC, ucACMask, pClientSTA->ucPktPending);

  /*                                                                        
                            
                                                                           */
//   
#if 0 
  if (WLAN_STA_SOFTAP != pTLCb->atlSTAClients[ucSTAId].wSTADesc.wSTAType && 
     (!vos_concurrent_sessions_running()))
  {
    //              
    //                                                                   
    vosStatus = pTLCb->atlSTAClients[ucSTAId].pfnSTAFetchPkt( pvosGCtx,
                                  &ucSTAId,
                                  pTLCb->atlSTAClients[ucSTAId].ucCurrentAC,
                                  &vosDataBuff, &tlMetaInfo );
  }
  else
  {
    //           
    WLANTL_ACEnumType ucAC = pTLCb->uCurServedAC;
    vosStatus = pTLCb->atlSTAClients[ucSTAId].pfnSTAFetchPkt( pvosGCtx, 
                               &ucSTAId,
                               ucAC,
                                                &vosDataBuff, &tlMetaInfo );
  }
#endif

  vosStatus = pClientSTA->pfnSTAFetchPkt( pvosGCtx,
                               &ucSTAId,
                               ucAC,
                               &vosDataBuff, &tlMetaInfo );

  if (( VOS_STATUS_SUCCESS != vosStatus ) || ( NULL == vosDataBuff ))
  {
    TLLOG1(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
               "WLAN TL:No more data at HDD status %d", vosStatus));
    *pvosDataBuff = NULL;

    /*                                                                    
                                                                
                                         
                                                                       
                            
                                                                        */
    pClientSTA->ucCurrentAC     = WLANTL_AC_VO;
    pClientSTA->ucCurrentWeight = 0;

    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
              "WLAN TL: WLANTL_STATxConn no more packets in HDD for AC: %d AC Mask: %d", 
               ucAC, ucACMask);

    return vosStatus;
  }

  /*                                                                  
                              */
  vos_atomic_set_U8( &pClientSTA->ucPktPending, 1);
  pClientSTA->ucNoMoreData = 0;
  pClientSTA->aucACMask[ucAC] = 1;

#ifdef WLAN_PERF 
  vos_pkt_set_user_data_ptr( vosDataBuff, VOS_PKT_USER_DATA_ID_BAL, 
                             (v_PVOID_t)0);

#endif /*         */


#ifdef FEATURE_WLAN_WAPI
   /*                                                                        
                                                                          
                                                                           */
   if ( 0 == tlMetaInfo.ucIsEapol && 0 == tlMetaInfo.ucIsWai )
   {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
                "WLAN TL:Only EAPOL or WAI packets allowed before authentication"));

     /*                    */
     pClientSTA->pfnSTATxComp( pvosGCtx, vosDataBuff,
                                                VOS_STATUS_E_BADMSG);
     vosDataBuff = NULL;
     *pvosDataBuff = NULL;
     return VOS_STATUS_SUCCESS;
  }
#else
   if ( 0 == tlMetaInfo.ucIsEapol )
  {
     TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                "WLAN TL:Received non EAPOL packet before authentication"));

    /*                    */
    pClientSTA->pfnSTATxComp( pvosGCtx, vosDataBuff,
                                                VOS_STATUS_E_BADMSG);
    vosDataBuff = NULL;
    *pvosDataBuff = NULL;
    return VOS_STATUS_SUCCESS;
  }
#endif /*                   */

  /*                                                                         
            
                                                                           */
  ucTid     = tlMetaInfo.ucTID;

  /*                      */
  if ( WLANTL_TID_INVALID(ucTid)) 
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
            "WLAN TL:Invalid TID sent in meta info %d - defaulting to 0 (BE)", 
             ucTid));
     ucTid = 0; 
  }

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Attaching BD header to pkt on WLANTL_STATxConn"));

#ifdef FEATURE_WLAN_WAPI
  /*                                                                        
                                                                          
                         
                                                                           */
  if ( ( 1 == tlMetaInfo.ucIsWai ) ||
       ( 0 == tlMetaInfo.ucDisableFrmXtl ) )
#else
  /*                                                                        
                                                                    
                                                                           */
  if ( ( 0 == tlMetaInfo.ucDisableFrmXtl ) &&
      ( 0 != pClientSTA->wSTADesc.ucSwFrameTXXlation) )
#endif //                        
  {
    vosStatus =  WLANTL_Translate8023To80211Header( vosDataBuff, &vosStatus,
                                                    pTLCb, &ucSTAId,
                                                    &tlMetaInfo, &ucWDSEnabled,
                                                    &extraHeadSpace);
    if ( VOS_STATUS_SUCCESS != vosStatus )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL:Error when translating header WLANTL_STATxConn"));

      return vosStatus;
    }

    tlMetaInfo.ucDisableFrmXtl = 1;
  }

  /*                                                                         
                              
                                                                            */
  ucTypeSubtype |= (WLANTL_80211_DATA_TYPE << 4);

  if ( pClientSTA->wSTADesc.ucQosEnabled )
  {
    ucTypeSubtype |= (WLANTL_80211_DATA_QOS_SUBTYPE);
  }

#ifdef FEATURE_WLAN_WAPI
  /*                                                                           
                                                                           
                                                                              
                                                                       
                                                                      
                                                                             
                                                          
   */
  if (tlMetaInfo.ucIsEapol && pClientSTA->ptkInstalled)
  {
    txFlag = 0;
  }
#else
  if (pClientSTA->ptkInstalled)
  {
    txFlag = 0;
  }
#endif

  vosStatus = (VOS_STATUS)WDA_DS_BuildTxPacketInfo( pvosGCtx, vosDataBuff , &vDestMacAddr,
                          tlMetaInfo.ucDisableFrmXtl, &usPktLen,
                          pClientSTA->wSTADesc.ucQosEnabled, ucWDSEnabled,
                          extraHeadSpace,
                          ucTypeSubtype, &pClientSTA->wSTADesc.vSelfMACAddress,
                          ucTid, txFlag,
                          tlMetaInfo.usTimeStamp, tlMetaInfo.ucIsEapol || tlMetaInfo.ucIsWai, tlMetaInfo.ucUP );

  if ( VOS_STATUS_SUCCESS != vosStatus )
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL:Failed while attempting to fill BD %d", vosStatus));
    *pvosDataBuff = NULL;
    return vosStatus;
  }

  /*                                                                       
                                                      
                                              
                                                                           */
  pClientSTA->auTxCount[ucTid]++;

  vos_pkt_set_user_data_ptr( vosDataBuff, VOS_PKT_USER_DATA_ID_TL,
               (v_PVOID_t)pClientSTA->pfnSTATxComp );

  /*                                                                        
                                          
                                                                          */
  *pvosDataBuff = vosDataBuff;
  /*                                 */
  pTLCb->bUrgent      = TRUE;

  /*               */
  if (!(tlMetaInfo.ucBcast || tlMetaInfo.ucMcast))
  {
    /*                     */
    pClientSTA->trafficStatistics.txUCFcnt++;
    pClientSTA->trafficStatistics.txUCBcnt += usPktLen;
  }

  return VOS_STATUS_SUCCESS;
}/*                  */


/*                                                                          
                              

             
                                                      

              
                                                                          

            

     
                                                                      
                                                                  
                                                            
                                              

              
                                                            

                                                                         
                                    
                                               

                                                                     
                                        

              

                                                                            */
VOS_STATUS
WLANTL_STATxAuth
(
  v_PVOID_t     pvosGCtx,
  v_U8_t        ucSTAId,
  vos_pkt_t**   pvosDataBuff,
  v_BOOL_t      bForwardIAPPwithLLC
)
{
   v_U16_t               usPktLen;
   VOS_STATUS            vosStatus;
   v_MACADDR_t           vDestMacAddr;
   vos_pkt_t*            vosDataBuff = NULL;
   WLANTL_CbType*        pTLCb       = NULL;
   WLANTL_MetaInfoType   tlMetaInfo;
   v_U8_t                ucTypeSubtype = 0;
   WLANTL_ACEnumType     ucAC;
   WLANTL_ACEnumType     ucNextAC;
   v_U8_t                ucTid;
   v_U8_t                ucSwFrmXtl = 0;
   v_U8_t                extraHeadSpace = 0;
   WLANTL_STAClientType *pStaClient = NULL;
   v_U8_t                ucWDSEnabled = 0;
   v_U8_t                ucTxFlag   = 0; 
   v_U8_t                ucACMask, i; 
  /*                                                                       */

  /*                                                                        
                
                           
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if (( NULL == pTLCb ) || ( NULL == pvosDataBuff ))
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Invalid input params on WLANTL_STATxAuth TL %x DB %p",
             pTLCb, pvosDataBuff));
    if (NULL != pvosDataBuff)
    {
        *pvosDataBuff = NULL;
    }
    if(NULL != pTLCb)
    {
        if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
        {
            TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL:Client Memory was not allocated on %s", __func__));
            return VOS_STATUS_E_FAILURE;
        }
        pTLCb->atlSTAClients[ucSTAId]->ucNoMoreData = 1;
    }
    return VOS_STATUS_E_FAULT;
  }
  pStaClient = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pStaClient )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  vos_mem_zero(&tlMetaInfo, sizeof(tlMetaInfo));
  /*                                                                        
                         
                                                                           */
#ifdef FEATURE_WLAN_TDLS
  if ((WLAN_STA_SOFTAP != pStaClient->wSTADesc.wSTAType) &&
      (!vos_concurrent_sessions_running()) &&
      !pTLCb->ucTdlsPeerCount)
  {
#else
  if ((WLAN_STA_SOFTAP != pStaClient->wSTADesc.wSTAType) &&
      (!vos_concurrent_sessions_running()))
  {
#endif
  ucAC = pStaClient->ucCurrentAC;

  /*                                                                   
                                                              
                                                             
                                                            
                                                                 
                                                                  
                                                                   
                                                                     
                                                              
                                   
                                                                   
                                           
                                                                     */
  pStaClient->aucACMask[pStaClient->ucCurrentAC] = 0; 

  //                                                           
  //                                                 
  }
  else
  {
    //           
    ucAC = pTLCb->uCurServedAC;
    pStaClient->aucACMask[ucAC] = 0; 

    //                                                 
  }

  WLAN_TL_AC_ARRAY_2_MASK( pStaClient, ucACMask, i); 
    /*                                                                    
                                                                      
                                                                                
                                               */
  if ( 0 == ucACMask )
  {
    vos_atomic_set_U8( &pStaClient->ucPktPending, 0);
    pStaClient->ucNoMoreData = 1;
  }

  vosStatus = pStaClient->pfnSTAFetchPkt( pvosGCtx, 
                               &ucSTAId,
                               ucAC,
                               &vosDataBuff, &tlMetaInfo );


  if (( VOS_STATUS_SUCCESS != vosStatus ) || ( NULL == vosDataBuff ))
  {

    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
               "WLAN TL:Failed while attempting to fetch pkt from HDD %d",
                   vosStatus);
    *pvosDataBuff = NULL;
    /*                                                                    
                                                                  
                                           
                                                                         
                              
                                                                        */
    pStaClient->ucCurrentAC     = WLANTL_AC_VO;
    pStaClient->ucCurrentWeight = 0;

    return vosStatus;
  }

  WLANTL_StatHandleTXFrame(pvosGCtx, ucSTAId, vosDataBuff, NULL, &tlMetaInfo);

  /*                                                                  
                              */
  vos_atomic_set_U8( &pStaClient->ucPktPending, 1);
  pStaClient->ucNoMoreData = 0;

  if (WLAN_STA_SOFTAP != pStaClient->wSTADesc.wSTAType)
  {
  //                                                             
  //                                                                   
  }

#ifdef WLAN_PERF 
   vos_pkt_set_user_data_ptr( vosDataBuff, VOS_PKT_USER_DATA_ID_BAL, 
                       (v_PVOID_t)0);
#endif /*         */

   /*                                                                         
             
                                                                            */
   ucTid     = tlMetaInfo.ucTID;

  /*                      */
  if ( WLANTL_TID_INVALID(ucTid)) 
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
            "WLAN TL:Invalid TID sent in meta info %d - defaulting to 0 (BE)", 
             ucTid));
     ucTid = 0; 
  }

  /*                                  */
  pStaClient->ucServicedAC = ucAC; 

  if ( ucAC == pStaClient->ucCurrentAC ) 
  {
    pStaClient->aucACMask[pStaClient->ucCurrentAC] = 1;
    pStaClient->ucCurrentWeight--;
  }
  else
  {
    pStaClient->ucCurrentAC     = ucAC;
    pStaClient->ucCurrentWeight = pTLCb->tlConfigInfo.ucAcWeights[ucAC] - 1;

    pStaClient->aucACMask[pStaClient->ucCurrentAC] = 1;

  }

  if (WLAN_STA_SOFTAP != pStaClient->wSTADesc.wSTAType)
  {
  if ( 0 == pStaClient->ucCurrentWeight ) 
  {
    WLANTL_ACEnumType tempAC = ucAC;
    /*                                                                       
                                       
                                                                           */
    while ( 0 != ucACMask ) 
    {
      ucNextAC = (WLANTL_ACEnumType)(( tempAC - 1 ) & WLANTL_MASK_AC); 
      if ( 0 != pStaClient->aucACMask[ucNextAC] )
      {
         pStaClient->ucCurrentAC     = ucNextAC;
         pStaClient->ucCurrentWeight = pTLCb->tlConfigInfo.ucAcWeights[ucNextAC];

         TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                    "WLAN TL: Changing serviced AC to: %d with Weight: %d",
                    pStaClient->ucCurrentAC , 
                    pStaClient->ucCurrentWeight));
         break;
      }
      tempAC = ucNextAC;
    }
  }
  }

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Attaching BD header to pkt on WLANTL_STATxAuth"));

  /*                                                                        
                                   
                                                                           */
  if ( 0 == tlMetaInfo.ucDisableFrmXtl )
  {
     /*                         */
     //                                                        
     //                                    
     //                                                                    
     //                                                                  
#ifdef FEATURE_WLAN_WAPI
     //                                     
#endif
     //                                    

     if ( ( 0 == pStaClient->wSTADesc.ucSwFrameTXXlation ) &&
          ( 0 == tlMetaInfo.ucBcast ) &&
          ( 0 == tlMetaInfo.ucMcast )
#ifdef FEATURE_WLAN_WAPI
          && ( tlMetaInfo.ucIsWai != 1 )
#endif
        )
     {
#ifdef WLAN_PERF 
        v_U32_t uFastFwdOK = 0;

        /*                                                                */
        WDA_TLI_FastHwFwdDataFrame( pvosGCtx, vosDataBuff , &vosStatus, 
                                   &uFastFwdOK, &tlMetaInfo, &pStaClient->wSTADesc);

        if( VOS_STATUS_SUCCESS == vosStatus )
        {
            if(uFastFwdOK)
            {
                /*                                    */
                vos_pkt_set_user_data_ptr( vosDataBuff, VOS_PKT_USER_DATA_ID_TL, 
                               (v_PVOID_t)pStaClient->pfnSTATxComp );

                *pvosDataBuff = vosDataBuff;

                /*                                                            
                                        */
                pStaClient->auTxCount[tlMetaInfo.ucTID]++;
                return vosStatus;
             }
             /*                                                           */
        }
        else
        {

            TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                       "WLAN TL:Failed while attempting to fastFwd BD %d", vosStatus));
            *pvosDataBuff = NULL;
            return vosStatus;
        }
#endif /*         */
     }
     else
     {
        /*                      */

#ifdef FEATURE_WLAN_WAPI
       gUcIsWai = tlMetaInfo.ucIsWai,
#endif

       vosStatus =  WLANTL_Translate8023To80211Header( vosDataBuff, &vosStatus,
                                                    pTLCb, &ucSTAId,
                                                    &tlMetaInfo, &ucWDSEnabled,
                                                    &extraHeadSpace);
       if ( VOS_STATUS_SUCCESS != vosStatus )
       {
          TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                    "WLAN TL:Error when translating header WLANTL_STATxAuth"));
          return vosStatus;
       }

       TLLOG4(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_LOW,
                    "WLAN TL software translation success \n"));
       ucSwFrmXtl = 1;
       tlMetaInfo.ucDisableFrmXtl = 1;
    }
  }
#ifdef FEATURE_WLAN_TDLS
    /*                                                                  
                                                   */
    pStaClient = pTLCb->atlSTAClients[ucSTAId];

    if ( NULL == pStaClient )
    {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
        "pStaClient is NULL %s", __func__));
        return VOS_STATUS_E_FAILURE;
    }
#endif
  /*                                                                         
                              
                                                                            */
  ucTypeSubtype |= (WLANTL_80211_DATA_TYPE << 4);

  if ( pStaClient->wSTADesc.ucQosEnabled ) 
  {
    ucTypeSubtype |= (WLANTL_80211_DATA_QOS_SUBTYPE);
  }

  ucTxFlag  = (0 != pStaClient->wUAPSDInfo[ucAC].ucSet)?
              HAL_TRIGGER_ENABLED_AC_MASK:0;

#ifdef FEATURE_WLAN_WAPI
  if ( pStaClient->wSTADesc.ucIsWapiSta == 1 )
  {
#ifdef LIBRA_WAPI_SUPPORT
    ucTxFlag = ucTxFlag | HAL_WAPI_STA_MASK;
#endif //                  
    if ( tlMetaInfo.ucIsWai == 1 ) 
    {
      ucTxFlag = ucTxFlag | HAL_TX_NO_ENCRYPTION_MASK;
    }
  }
#endif /*                   */
#ifdef FEATURE_WLAN_TDLS
  if ( pStaClient->wSTADesc.wSTAType == WLAN_STA_TDLS )
  {
    ucTxFlag = ucTxFlag | HAL_TDLS_PEER_STA_MASK;
  }
#endif /*                   */

  vosStatus = (VOS_STATUS)WDA_DS_BuildTxPacketInfo( pvosGCtx, 
                     vosDataBuff , &vDestMacAddr,
                     tlMetaInfo.ucDisableFrmXtl, &usPktLen,
                     pStaClient->wSTADesc.ucQosEnabled, ucWDSEnabled, 
                     extraHeadSpace,
                     ucTypeSubtype, &pStaClient->wSTADesc.vSelfMACAddress,
                     ucTid, ucTxFlag, tlMetaInfo.usTimeStamp, 
                     tlMetaInfo.ucIsEapol, tlMetaInfo.ucUP );

  if(!VOS_IS_STATUS_SUCCESS(vosStatus))
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "Fill TX BD Error status %d", vosStatus));

    return vosStatus;
  }

  /*               */
  if (!(tlMetaInfo.ucBcast || tlMetaInfo.ucMcast))
  {
    /*                     */
    pStaClient->trafficStatistics.txUCFcnt++;
    pStaClient->trafficStatistics.txUCBcnt += usPktLen;
  }

#ifndef FEATURE_WLAN_TDLS
  /*                                                                       
                                                      
                                                                           */
  pStaClient->auTxCount[ucTid]++;
#else
  pTLCb->atlSTAClients[ucSTAId]->auTxCount[ucTid]++;
#endif

  /*                                                                      
                                                                           
                                                                           
                                
                                                                          
                                                                              
                                                                        */
#if 0
  if (( ucTid != tlMetaInfo.ucTID ) &&
      ( 0 != pStaClient->wSTADesc.ucQosEnabled ) && 
      ( 0 != ucSwFrmXtl ))
  {
    /*                                                                     
                                                                   
                                                                         */
    ucQCOffset = WLANHAL_TX_BD_HEADER_SIZE + WLANTL_802_11_HEADER_LEN;

    //                                      
    vos_pkt_peek_data( vosDataBuff, ucQCOffset,(v_PVOID_t)&pucQosCtrl,
                       sizeof(*pucQosCtrl));
    *pucQosCtrl = ucTid; //                   
  }
#endif

  if ( VOS_STATUS_SUCCESS != vosStatus )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Failed while attempting to fill BD %d", vosStatus));
    *pvosDataBuff = NULL;
    return vosStatus;
  }

  vos_pkt_set_user_data_ptr( vosDataBuff, VOS_PKT_USER_DATA_ID_TL,
                   (v_PVOID_t)pStaClient->pfnSTATxComp );

  *pvosDataBuff = vosDataBuff;

  /*                                                              */
  if ( pStaClient->ucServicedAC > WLANTL_AC_BE ) 
  {
    pTLCb->bUrgent= TRUE;
  }

  return VOS_STATUS_SUCCESS;
}/*                  */

/*                                                                          
                              

             
                                                    

              
                                                                          

            

     
                                                                      
                                                                  
                                                            
                                              

              
                                                            

                                               

              

                                                                            */
VOS_STATUS
WLANTL_STATxDisc
(
  v_PVOID_t     pvosGCtx,
  v_U8_t        ucSTAId,
  vos_pkt_t**   pvosDataBuff,
  v_BOOL_t      bForwardIAPPwithLLC
)
{
   WLANTL_CbType*        pTLCb       = NULL;
   WLANTL_STAClientType* pClientSTA = NULL;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_STATxAuth"));
    *pvosDataBuff = NULL;
    return VOS_STATUS_E_FAULT;
  }

  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pClientSTA )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  /*                                                                        
         
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
    "WLAN TL:Packet should not be transmitted in state disconnected ignoring"
            " request"));

  *pvosDataBuff = NULL;
   pClientSTA->ucNoMoreData = 1;
   
   //                                                  
   vos_atomic_set_U8( &pClientSTA->ucPktPending, 0);

  return VOS_STATUS_SUCCESS;
}/*                  */

/*                                                                          
                              

             
                                           

              
                                                                          

            

     
                                                                      
                                                                  
                                                            
                                                 

              
                                                            

                                                  
                                                                         
                                    
                                               

              

                                                                            */
VOS_STATUS
WLANTL_STARxConn
(
  v_PVOID_t     pvosGCtx,
  v_U8_t        ucSTAId,
  vos_pkt_t**   pvosDataBuff,
  v_BOOL_t      bForwardIAPPwithLLC
)
{
   WLANTL_CbType*           pTLCb = NULL;
   WLANTL_STAClientType*    pClientSTA = NULL;
   v_U16_t                  usEtherType = 0;
   v_U16_t                  usPktLen;
   v_U8_t                   ucMPDUHOffset;
   v_U16_t                  usMPDUDOffset;
   v_U16_t                  usMPDULen;
   v_U8_t                   ucMPDUHLen;
   v_U16_t                  usActualHLen = 0;
   VOS_STATUS               vosStatus  = VOS_STATUS_SUCCESS;
   vos_pkt_t*               vosDataBuff;
   v_PVOID_t                aucBDHeader;
   v_U8_t                   ucTid;
   WLANTL_RxMetaInfoType    wRxMetaInfo;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if (( NULL == pvosDataBuff ) || ( NULL == ( vosDataBuff = *pvosDataBuff )))
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_STARxConn"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
        "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ChangeSTAState"));
    return VOS_STATUS_E_FAULT;
  }
  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pClientSTA )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  /*                                                                        
                                        
                                                                           */
  vosStatus = WDA_DS_PeekRxPacketInfo( vosDataBuff, (v_PVOID_t)&aucBDHeader, 0/*       */ );

  if ( NULL == aucBDHeader )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Cannot extract BD header"));
    VOS_ASSERT( 0 );
    return VOS_STATUS_E_FAULT;
  }


  ucMPDUHOffset = (v_U8_t)WDA_GET_RX_MPDU_HEADER_OFFSET(aucBDHeader);
  usMPDUDOffset = (v_U16_t)WDA_GET_RX_MPDU_DATA_OFFSET(aucBDHeader);
  usMPDULen     = (v_U16_t)WDA_GET_RX_MPDU_LEN(aucBDHeader);
  ucMPDUHLen    = (v_U8_t)WDA_GET_RX_MPDU_HEADER_LEN(aucBDHeader);
  ucTid         = (v_U8_t)WDA_GET_RX_TID(aucBDHeader);

  vos_pkt_get_packet_length( vosDataBuff, &usPktLen);

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:BD header processing data: HO %d DO %d Len %d HLen %d",
             ucMPDUHOffset, usMPDUDOffset, usMPDULen, ucMPDUHLen));

    /*                                             */
  if ( VOS_STATUS_SUCCESS != WDA_DS_TrimRxPacketInfo( vosDataBuff ) )
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:BD header corrupted - dropping packet"));
    /*             */
    vos_pkt_return_packet(vosDataBuff);
    return VOS_STATUS_SUCCESS;
  }

  vosStatus = WLANTL_GetEtherType(aucBDHeader,vosDataBuff,ucMPDUHLen,&usEtherType);
  
  if( VOS_IS_STATUS_SUCCESS(vosStatus) )
  {
#ifdef FEATURE_WLAN_WAPI
    /*                                                                          */
    /*                                                                               */
    /*                                                         */
    if ( WLANTL_LLC_8021X_TYPE  != usEtherType && WLANTL_LLC_WAI_TYPE  != usEtherType )
    {
      VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                 "WLAN TL:RX Frame not EAPOL or WAI EtherType %d - dropping", usEtherType );
      /*             */
      vos_pkt_return_packet(vosDataBuff);
    }
#else
    if ( WLANTL_LLC_8021X_TYPE  != usEtherType )
    {
      VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "WLAN TL:RX Frame not EAPOL EtherType %d - dropping", usEtherType);
      /*             */
      vos_pkt_return_packet(vosDataBuff);
    }
#endif /*                   */
    else /*                                       */  
    {

      VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
                 "WLAN TL:RX Frame  EAPOL EtherType %d - processing", usEtherType);

      if (( 0 == WDA_GET_RX_FT_DONE(aucBDHeader) ) &&
         ( 0 != pClientSTA->wSTADesc.ucSwFrameRXXlation))
      {
      if (usMPDUDOffset > ucMPDUHOffset)
      {
         usActualHLen = usMPDUDOffset - ucMPDUHOffset;
      }

      vosStatus = WLANTL_Translate80211To8023Header( vosDataBuff, &vosStatus, usActualHLen, 
                      ucMPDUHLen, pTLCb, ucSTAId, bForwardIAPPwithLLC);

        if ( VOS_STATUS_SUCCESS != vosStatus ) 
        {
          TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Failed to translate from 802.11 to 802.3 - dropping"));
          /*             */
          vos_pkt_return_packet(vosDataBuff);
          return vosStatus;
        }
      }
      /*                                                                   
                               
                                                                         */
      if ( !WLANTL_TID_INVALID( ucTid) ) 
      {
        pClientSTA->auRxCount[ucTid]++;
      }
      else
      {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid tid  %d (Station ID %d) on %s",
               ucTid, ucSTAId, __func__));
      }

      TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:Sending EAPoL frame to station %d AC %d", ucSTAId, ucTid));

      /*                                                                   
                                   
                                                                         */
      wRxMetaInfo.ucUP = ucTid;

      TLLOG4(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_LOW,
               "WLAN TL %s:Sending data chain to station \n", __func__));
      if ( WLAN_STA_SOFTAP == pClientSTA->wSTADesc.wSTAType )
      {
        wRxMetaInfo.ucDesSTAId = WLAN_RX_SAP_SELF_STA_ID;
        pClientSTA->pfnSTARx( pvosGCtx, vosDataBuff, ucSTAId,
                                            &wRxMetaInfo );
      }
      else
      pClientSTA->pfnSTARx( pvosGCtx, vosDataBuff, ucSTAId,
                                            &wRxMetaInfo );
    }/*                        */
  }/*                  */

  return VOS_STATUS_SUCCESS;
}/*                  */

/*                                                                          
                                

             
                                                                         

              

            

     
                                                                      
                                                                  
                                                            
                                              

              
                                                            

                                                  
                                                                         
                                    
                                               

              

                                                                            */

VOS_STATUS
WLANTL_FwdPktToHDD
(
  v_PVOID_t       pvosGCtx,
  vos_pkt_t*     pvosDataBuff,
  v_U8_t          ucSTAId
)
{
   v_MACADDR_t              DestMacAddress;
   v_MACADDR_t              *pDestMacAddress = &DestMacAddress;
   v_SIZE_t                 usMacAddSize = VOS_MAC_ADDR_SIZE;
   WLANTL_CbType*           pTLCb = NULL;
   WLANTL_STAClientType*    pClientSTA = NULL;
   vos_pkt_t*               vosDataBuff ;
   VOS_STATUS               vosStatus = VOS_STATUS_SUCCESS;
   v_U32_t                  STAMetaInfo;
   vos_pkt_t*               vosNextDataBuff ;
   v_U8_t                   ucDesSTAId;
   WLANTL_RxMetaInfoType    wRxMetaInfo;

  /*                                                                        
                
                                                                           */
  if (( NULL == pvosDataBuff ) || ( NULL == ( vosDataBuff = pvosDataBuff )))
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid parameter sent on WLANTL_FwdPktToHdd"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_FwdPktToHdd"));
    return VOS_STATUS_E_FAULT;
  }

  if(WLANTL_STA_ID_INVALID(ucSTAId))
  {
     TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,"ucSTAId %d is not valid",
                 ucSTAId));
     return VOS_STATUS_E_INVAL;
  }

  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pClientSTA )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

   /*                                                                            
                                                                                       
                                                                                       
                                                                                */

   vosDataBuff = pvosDataBuff;
   while (vosDataBuff != NULL)
   {
      vos_pkt_walk_packet_chain( vosDataBuff, &vosNextDataBuff, 1/*    */ );
      vos_pkt_get_user_data_ptr( vosDataBuff, VOS_PKT_USER_DATA_ID_TL,
                                 (v_PVOID_t *)&STAMetaInfo );
      wRxMetaInfo.ucUP = (v_U8_t)(STAMetaInfo & WLANTL_AC_MASK);
      ucDesSTAId = (v_U8_t)((STAMetaInfo) >> WLANTL_STAID_OFFSET); 
       
      vosStatus = vos_pkt_extract_data( vosDataBuff, 0, (v_VOID_t *)pDestMacAddress, &usMacAddSize);
      if ( VOS_STATUS_SUCCESS != vosStatus )
      {
         TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL: recv corrupted data packet\n"));
         vos_pkt_return_packet(vosDataBuff);
         return vosStatus;
      }

      TLLOG4(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_LOW,
                        "station mac "MAC_ADDRESS_STR,
                        MAC_ADDR_ARRAY(pDestMacAddress->bytes)));

      if (vos_is_macaddr_broadcast( pDestMacAddress ) || vos_is_macaddr_group(pDestMacAddress))
      {
          //                             
          ucDesSTAId = WLAN_RX_BCMC_STA_ID;
          TLLOG4(VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO_LOW,
                    "%s: BC/MC packet, id %d\n", __func__, WLAN_RX_BCMC_STA_ID));
      }
      else
      {
         if (vos_is_macaddr_equal(pDestMacAddress, &pClientSTA->wSTADesc.vSelfMACAddress))
         {
            //                         
            ucDesSTAId = WLAN_RX_SAP_SELF_STA_ID;
            TLLOG4(VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO_LOW,
                     "%s: packet to AP itself, id %d\n", __func__, WLAN_RX_SAP_SELF_STA_ID));
         }
         else if (( WLAN_MAX_STA_COUNT <= ucDesSTAId ) || (NULL != pTLCb->atlSTAClients[ucDesSTAId] && pTLCb->atlSTAClients[ucDesSTAId]->ucExists == 0))
         {
            //                                      
            TLLOG4(VOS_TRACE( VOS_MODULE_ID_HDD_SOFTAP, VOS_TRACE_LEVEL_INFO_LOW,
                 "%s: get an station index larger than WLAN_MAX_STA_COUNT %d\n", __func__, ucDesSTAId));
            ucDesSTAId = WLAN_RX_SAP_SELF_STA_ID;
         }

         
         //                                   
      }

      wRxMetaInfo.ucUP = (v_U8_t)(STAMetaInfo & WLANTL_AC_MASK);
      wRxMetaInfo.ucDesSTAId = ucDesSTAId;
     
      vosStatus = pClientSTA->pfnSTARx( pvosGCtx, vosDataBuff, ucDesSTAId,
                                            &wRxMetaInfo );
      if ( VOS_STATUS_SUCCESS != vosStatus )
      {
          TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL: failed to send pkt to HDD \n"));
          vos_pkt_return_packet(vosDataBuff);

          return vosStatus;
      }
      vosDataBuff = vosNextDataBuff;
   }
   return VOS_STATUS_SUCCESS;
}

/*                                                                          
                              

             
                                                     

              
                                                                          

            

     
                                                                      
                                                                  
                                                            
                                              

              
                                                            

                                                  
                                                                         
                                    
                                               

              

                                                                            */
VOS_STATUS
WLANTL_STARxAuth
(
  v_PVOID_t     pvosGCtx,
  v_U8_t        ucSTAId,
  vos_pkt_t**   pvosDataBuff,
  v_BOOL_t      bForwardIAPPwithLLC
)
{
   WLANTL_CbType*           pTLCb = NULL;
   WLANTL_STAClientType*    pClientSTA = NULL;
   v_U8_t                   ucAsf; /*                 */
   v_U16_t                  usMPDUDOffset;
   v_U8_t                   ucMPDUHOffset;
   v_U16_t                  usMPDULen;
   v_U8_t                   ucMPDUHLen;
   v_U16_t                  usActualHLen = 0;   
   v_U8_t                   ucTid;
#ifdef FEATURE_WLAN_WAPI
   v_U16_t                  usEtherType = 0;
#endif
   v_U16_t                  usPktLen;
   vos_pkt_t*               vosDataBuff ;
   v_PVOID_t                aucBDHeader;
   VOS_STATUS               vosStatus;
   WLANTL_RxMetaInfoType    wRxMetaInfo;
   static v_U8_t            ucPMPDUHLen;
   v_U32_t*                  STAMetaInfoPtr;
   v_U8_t                   ucEsf=0; /*                              */
   v_U64_t                  ullcurrentReplayCounter=0; /*                      */
   v_U64_t                  ullpreviousReplayCounter=0; /*                       */
   v_U16_t                  ucUnicastBroadcastType=0; /*                                             */
   struct _BARFrmStruct     *pBarFrame = NULL;

  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if (( NULL == pvosDataBuff ) || ( NULL == ( vosDataBuff = *pvosDataBuff )))
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid parameter sent on WLANTL_STARxAuth"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_STARxAuth"));
    return VOS_STATUS_E_FAULT;
  }
  pClientSTA = pTLCb->atlSTAClients[ucSTAId];

  if ( NULL == pClientSTA )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  /*                                                                        
                                        
                                                                           */
  WDA_DS_PeekRxPacketInfo( vosDataBuff, (v_PVOID_t)&aucBDHeader, 0 );

  ucMPDUHOffset = (v_U8_t)WDA_GET_RX_MPDU_HEADER_OFFSET(aucBDHeader);
  usMPDUDOffset = (v_U16_t)WDA_GET_RX_MPDU_DATA_OFFSET(aucBDHeader);
  usMPDULen     = (v_U16_t)WDA_GET_RX_MPDU_LEN(aucBDHeader);
  ucMPDUHLen    = (v_U8_t)WDA_GET_RX_MPDU_HEADER_LEN(aucBDHeader);
  ucTid         = (v_U8_t)WDA_GET_RX_TID(aucBDHeader);

  /*                         
                                                                   
                                                               */
  if( (WDA_GET_RX_TYPE(aucBDHeader) == SIR_MAC_CTRL_FRAME) &&
      (WDA_GET_RX_SUBTYPE(aucBDHeader) == SIR_MAC_CTRL_BAR))
  {
      pBarFrame = (struct _BARFrmStruct *)(WDA_GET_RX_MAC_HEADER(aucBDHeader));
      ucTid = pBarFrame->barControl.numTID;
  }

  /*                                                         */
  ucUnicastBroadcastType  = (v_U16_t)WDA_IS_RX_BCAST(aucBDHeader);
  if(0 != ucMPDUHLen)
  {
    ucPMPDUHLen = ucMPDUHLen;
  }

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:BD header processing data: HO %d DO %d Len %d HLen %d"
             " Tid %d BD %d",
             ucMPDUHOffset, usMPDUDOffset, usMPDULen, ucMPDUHLen, ucTid,
             WLANHAL_RX_BD_HEADER_SIZE));

  vos_pkt_get_packet_length( vosDataBuff, &usPktLen);

  if ( VOS_STATUS_SUCCESS != WDA_DS_TrimRxPacketInfo( vosDataBuff ) )
  {
    if((WDA_GET_RX_ASF(aucBDHeader) && !WDA_GET_RX_ESF(aucBDHeader)))
  {
    /*                              
                                       */
    if(( usMPDUDOffset >  ucMPDUHOffset ) &&
       ( usMPDULen     >= ucMPDUHLen ) && ( usPktLen >= usMPDULen ) &&
       ( !WLANTL_TID_INVALID(ucTid) ))
    {
        ucMPDUHOffset = usMPDUDOffset - WLANTL_MPDU_HEADER_LEN; 
    }
    else
    {
      TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                "WLAN TL:BD header corrupted - dropping packet"));
      /*             */
      vos_pkt_return_packet(vosDataBuff);
      return VOS_STATUS_SUCCESS;
    }
  }
  else
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
              "WLAN TL:BD header corrupted - dropping packet"));
    /*             */
    vos_pkt_return_packet(vosDataBuff);
    return VOS_STATUS_SUCCESS;
  }
  }

#ifdef FEATURE_WLAN_WAPI
  if ( pClientSTA->wSTADesc.ucIsWapiSta )
  {
    vosStatus = WLANTL_GetEtherType(aucBDHeader, vosDataBuff, ucMPDUHLen, &usEtherType);
    if( VOS_IS_STATUS_SUCCESS(vosStatus) )
    {
      if ( WLANTL_LLC_WAI_TYPE  == usEtherType )
      {        
        if ( !( WLANHAL_RX_IS_UNPROTECTED_WPI_FRAME(aucBDHeader)) )
        {
          TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                     "WLAN TL:WAI frame was received encrypted - dropping"));
          /*             */
          /*                                           */
          //                                   
          //                                     
        }
      }
      else
      {
        if (  WLANHAL_RX_IS_UNPROTECTED_WPI_FRAME(aucBDHeader) ) 
        {
          TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                     "WLAN TL:Non-WAI frame was received unencrypted - dropping"));
          /*             */
          vos_pkt_return_packet(vosDataBuff); 
          return vosStatus; //                 
        }
      }
    }
    else //                                                    
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "WLAN TL:Could not extract EtherType"));
      //                       
      return vosStatus; //                 
    }
  }
#endif /*                   */

  /*                                                                      
                             
                                                                      
                                           
                                
                                                                           */
  if ( !WLANTL_TID_INVALID( ucTid) ) 
  {
    pClientSTA->auRxCount[ucTid]++;
  }
  else
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
           "WLAN TL:Invalid tid  %d (Station ID %d) on %s",
           ucTid, ucSTAId, __func__));
  }

  /*                                                                        
                                                
                                                                           */
  ucAsf = (v_U8_t)WDA_GET_RX_ASF(aucBDHeader);

  if ( 0 != ucAsf )
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:Packet is AMSDU sub frame - sending for completion"));
    vosStatus = WLANTL_AMSDUProcess( pvosGCtx, &vosDataBuff, aucBDHeader, ucSTAId,
                         ucMPDUHLen, usMPDULen );
    if(NULL == vosDataBuff)
    {
       //                       
       return VOS_STATUS_SUCCESS;
    }
  }
  /*                           
                                              */
    /*                                                                   
                                    
                                    
                                                                          */
  if (( 0 == WDA_GET_RX_FT_DONE(aucBDHeader) ) &&
      ( 0 != pClientSTA->wSTADesc.ucSwFrameRXXlation) &&
      ( WLANTL_IS_DATA_FRAME(WDA_GET_RX_TYPE_SUBTYPE(aucBDHeader)) ))
  {
    if(0 == ucMPDUHLen)
    {
      ucMPDUHLen = ucPMPDUHLen;
    }
    if (usMPDUDOffset > ucMPDUHOffset)
    {
      usActualHLen = usMPDUDOffset - ucMPDUHOffset;
    }
    vosStatus = WLANTL_Translate80211To8023Header( vosDataBuff, &vosStatus, usActualHLen, 
                        ucMPDUHLen, pTLCb, ucSTAId, bForwardIAPPwithLLC);

      if ( VOS_STATUS_SUCCESS != vosStatus )
      {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Failed to translate from 802.11 to 802.3 - dropping"));
        /*             */
        vos_pkt_return_packet(vosDataBuff);
        return vosStatus;
      }
    }
    /*                                                                     
                                                                            
                                                                              */
    if( WLAN_STA_SOFTAP == pClientSTA->wSTADesc.wSTAType)
    {
       STAMetaInfoPtr = (v_U32_t *)(ucTid | (WDA_GET_RX_ADDR3_IDX(aucBDHeader) << WLANTL_STAID_OFFSET));
       vos_pkt_set_user_data_ptr( vosDataBuff, VOS_PKT_USER_DATA_ID_TL,
                                 (v_PVOID_t)STAMetaInfoPtr);
    }

  /*                                                                        
                                                   
                                                                           */
  if ( 0 != pClientSTA->atlBAReorderInfo[ucTid].ucExists )
  {
    WLANTL_MSDUReorder( pTLCb, &vosDataBuff, aucBDHeader, ucSTAId, ucTid );
  }

if(0 == ucUnicastBroadcastType
#ifdef FEATURE_ON_CHIP_REORDERING
   && (WLANHAL_IsOnChipReorderingEnabledForTID(pvosGCtx, ucSTAId, ucTid) != TRUE)
#endif
)
{
  /*                                                                 */
  if(VOS_TRUE == pClientSTA->ucIsReplayCheckValid)
  {
      /*                                        */

      /*                                    */
      if ( 0 != ucAsf )
      {
          /*                                                          
                                                                    */
          /*                                                
                                               */
          ucEsf =  WDA_GET_RX_ESF( aucBDHeader );
          if( 0 != ucEsf )
          {
              v_BOOL_t status;
              /*                                              */
              ullcurrentReplayCounter = WDA_DS_GetReplayCounter(aucBDHeader);
 
              VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
               "WLAN TL: AMSDU currentReplayCounter [0x%llX]\n",ullcurrentReplayCounter);
              
              /*                                                               */
              ullpreviousReplayCounter = pClientSTA->ullReplayCounter[ucTid];

              VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
               "WLAN TL: AMSDU previousReplayCounter [0x%llX]\n",ullpreviousReplayCounter);

              /*                                      
                                                     
                                                     */
              status =  WLANTL_IsReplayPacket( ullcurrentReplayCounter, ullpreviousReplayCounter);
              if(VOS_FALSE == status)
              {
                   /*                                                             */    
                   pClientSTA->ullReplayCounter[ucTid] = ullcurrentReplayCounter;
              }
              else
              {
                  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "WLAN TL: AMSDU Drop the replay packet with PN : [0x%llX]\n",ullcurrentReplayCounter);

                  pClientSTA->ulTotalReplayPacketsDetected++;
                  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "WLAN TL: AMSDU total dropped replay packets on STA ID  %X is [0x%lX]\n",
                  ucSTAId,  pClientSTA->ulTotalReplayPacketsDetected);

                  /*                 */
                  vos_pkt_return_packet(vosDataBuff);
                  return VOS_STATUS_SUCCESS;
              }
          }
      }
      else
      {
           v_BOOL_t status;

           /*                                              */
           ullcurrentReplayCounter = WDA_DS_GetReplayCounter(aucBDHeader);

           VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
             "WLAN TL: Non-AMSDU currentReplayCounter [0x%llX]\n",ullcurrentReplayCounter);

           /*                                                               */
           ullpreviousReplayCounter = pClientSTA->ullReplayCounter[ucTid];

           VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
               "WLAN TL: Non-AMSDU previousReplayCounter [0x%llX]\n",ullpreviousReplayCounter);

           /*                                  
                                              
                                                       */ 
           status =  WLANTL_IsReplayPacket( ullcurrentReplayCounter, ullpreviousReplayCounter);
           if(VOS_FALSE == status)
           {
                /*                                                             */    
                pClientSTA->ullReplayCounter[ucTid] = ullcurrentReplayCounter;
           }
           else
           {
              VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL: Non-AMSDU Drop the replay packet with PN : [0x%llX]\n",ullcurrentReplayCounter);

               pClientSTA->ulTotalReplayPacketsDetected++;
               VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL: Non-AMSDU total dropped replay packets on STA ID %X is [0x%lX]\n",
                ucSTAId, pClientSTA->ulTotalReplayPacketsDetected);

               /*                                */
               vos_pkt_return_packet(vosDataBuff);
               return VOS_STATUS_SUCCESS;
           }
      }
  }
}
/*                                                              
                                                               */

  if ( NULL != vosDataBuff )
  {
    if( WLAN_STA_SOFTAP == pClientSTA->wSTADesc.wSTAType)
    {
      WLANTL_FwdPktToHDD( pvosGCtx, vosDataBuff, ucSTAId );
    }
    else
    {
      wRxMetaInfo.ucUP = ucTid;
      wRxMetaInfo.rssiAvg = pClientSTA->rssiAvg;
#ifdef FEATURE_WLAN_TDLS
      if (WLAN_STA_TDLS == pClientSTA->wSTADesc.wSTAType)
      {
          wRxMetaInfo.isStaTdls = TRUE;
      }
      else
      {
          wRxMetaInfo.isStaTdls = FALSE;
      }
#endif
      pClientSTA->pfnSTARx( pvosGCtx, vosDataBuff, ucSTAId,
                                            &wRxMetaInfo );
    }
  }/*             */

  return VOS_STATUS_SUCCESS;
}/*                  */


/*                                                                          
                              

             
                                                   

              
                                                                          

            

     
                                                                      
                                                                  
                                                            
                                              

              
                                                            

                                               

              

                                                                            */
VOS_STATUS
WLANTL_STARxDisc
(
  v_PVOID_t     pvosGCtx,
  v_U8_t        ucSTAId,
  vos_pkt_t**   pvosDataBuff,
  v_BOOL_t      bForwardIAPPwithLLC
)
{
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if (( NULL == pvosDataBuff ) || ( NULL ==  *pvosDataBuff ))
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid parameter sent on WLANTL_STARxDisc"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                       
                                                                           */
  TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
             "WLAN TL:Packet should not be received in state disconnected"
             " - dropping"));
  vos_pkt_return_packet(*pvosDataBuff);
  *pvosDataBuff = NULL;

  return VOS_STATUS_SUCCESS;
}/*                  */

/*                                                                          
                                                   
                                                                            */

/*                                                                          
                                 

             
                                                                   
                     

              
                                                                  

            

      
                                                                       
                                                                   
                                                   


              
                                                            

                                                  
                                                                         
                                    
                                               

              

                                                                            */
VOS_STATUS
WLANTL_McProcessMsg
(
  v_PVOID_t        pvosGCtx,
  vos_msg_t*       message
)
{
   WLANTL_CbType*  pTLCb = NULL;
   tAddBAInd*      ptAddBaInd = NULL;
   tDelBAInd*      ptDelBaInd = NULL;
   tAddBARsp*      ptAddBaRsp = NULL;
   vos_msg_t       vosMessage;
   VOS_STATUS      vosStatus;
   tpFlushACRsp FlushACRspPtr;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == message )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_ProcessMainMessage"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
     "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_ProcessMainMessage"));
    return VOS_STATUS_E_FAULT;
  }

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
        "WLAN TL:Received message: %d through main flow", message->type));

  switch( message->type )
  {
  case WDA_TL_FLUSH_AC_RSP:
    //                                          
    FlushACRspPtr = (tpFlushACRsp)(message->bodyptr);
    //                                              
    if ( NULL == pTLCb->tlBAPClient.pfnFlushOpCompleteCb )
    {
      VOS_ASSERT(0);
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
       "WLAN TL:Invalid TL pointer pfnFlushOpCompleteCb"));
      return VOS_STATUS_E_FAULT;
    }

    TLLOG2(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
            "Received message:  Flush complete received by TL"));

    //                                                                   
    //                                                       
    //                                              
    pTLCb->tlBAPClient.pfnFlushOpCompleteCb( pvosGCtx,
            FlushACRspPtr->ucSTAId,
            FlushACRspPtr->ucTid, FlushACRspPtr->status );

    //                                          
    TLLOG2(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
        "Flush complete received by TL: Freeing %p", FlushACRspPtr));
    vos_mem_free((v_VOID_t *)FlushACRspPtr);
    break;

  case WDA_HDD_ADDBA_REQ:
   ptAddBaInd = (tAddBAInd*)(message->bodyptr);
    vosStatus = WLANTL_BaSessionAdd( pvosGCtx,
                                 ptAddBaInd->baSession.baSessionID,
                                     ptAddBaInd->baSession.STAID,
                                     ptAddBaInd->baSession.baTID,
                                 (v_U32_t)ptAddBaInd->baSession.baBufferSize,
                                 ptAddBaInd->baSession.winSize,
                                 ptAddBaInd->baSession.SSN);
    ptAddBaRsp = vos_mem_malloc(sizeof(*ptAddBaRsp));

    if ( NULL == ptAddBaRsp )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL: fatal failure, cannot allocate BA Rsp structure"));
      VOS_ASSERT(0);
      return VOS_STATUS_E_NOMEM;
    }

    if ( VOS_STATUS_SUCCESS == vosStatus )
    {
      TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                "WLAN TL: Sending success indication to HAL for ADD BA"));
      /*            */
      ptAddBaRsp->mesgType    = WDA_HDD_ADDBA_RSP;
      vosMessage.type         = WDA_HDD_ADDBA_RSP;
    }
    else
    {
      TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLAN TL: Sending failure indication to HAL for ADD BA"));

      /*            */
      ptAddBaRsp->mesgType    = WDA_BA_FAIL_IND;
      vosMessage.type         = WDA_BA_FAIL_IND;
    }

    ptAddBaRsp->mesgLen     = sizeof(tAddBARsp);
    ptAddBaRsp->baSessionID = ptAddBaInd->baSession.baSessionID;
      /*                                                                       */
      ptAddBaRsp->replyWinSize = WLANTL_MAX_WINSIZE;
    vosMessage.bodyptr = ptAddBaRsp;

    vos_mq_post_message( VOS_MQ_ID_WDA, &vosMessage );
    WLANTL_McFreeMsg (pvosGCtx, message);
  break;
  case WDA_DELETEBA_IND:
    ptDelBaInd = (tDelBAInd*)(message->bodyptr);
    vosStatus  = WLANTL_BaSessionDel(pvosGCtx,
                                 ptDelBaInd->staIdx,
                                 ptDelBaInd->baTID);

    if ( VOS_STATUS_SUCCESS != vosStatus )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL: Failed to del BA session STA:%d TID:%d Status :%d",
               ptDelBaInd->staIdx,
               ptDelBaInd->baTID,
               vosStatus));
    }
    WLANTL_McFreeMsg (pvosGCtx, message);
    break;
  default:
    /*                     */
    break;
  }

  return VOS_STATUS_SUCCESS;
}/*                           */

/*                                                                          
                              

             
                                                                           
                                                                          
                                                                           
               

              
                                                                  

            

      
                                                                       
                                                                   
                                                   


              
                                                            

                                               

              

                                                                            */
VOS_STATUS
WLANTL_McFreeMsg
(
  v_PVOID_t        pvosGCtx,
  vos_msg_t*       message
)
{
  WLANTL_CbType*  pTLCb = NULL;
  /*                                                                       */

   /*                                                                        
                
                                                                           */
  if ( NULL == message )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_McFreeMsg"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
     "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_McFreeMsg"));
    return VOS_STATUS_E_FAULT;
  }

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
        "WLAN TL:Received message: %d through main free", message->type));

  switch( message->type )
  {
  case WDA_HDD_ADDBA_REQ:
  case WDA_DELETEBA_IND:
    /*                     */
    vos_mem_free(message->bodyptr);
    message->bodyptr = NULL;
    break;
  default:
    /*                     */
    break;
  }

  return VOS_STATUS_SUCCESS;
}/*                */

/*                                                                          
                                 

             
                                                                   
                   

              
                                                                  

            

      
                                                                       
                                                                   
                                                   


              
                                                            

                                                  
                                                                         
                                    
                                               

                                                                           
                                  
              

                                                                            */
VOS_STATUS
WLANTL_TxProcessMsg
(
  v_PVOID_t        pvosGCtx,
  vos_msg_t*       message
)
{
   VOS_STATUS      vosStatus = VOS_STATUS_SUCCESS;
   v_U32_t         uData;
   v_U8_t          ucSTAId; 
   v_U8_t          ucUcastSig;
   v_U8_t          ucBcastSig;
   void (*callbackRoutine) (void *callbackContext);
   void *callbackContext;
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == message )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_ProcessTxMessage"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                   
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Received message: %d through tx flow", message->type));

  switch( message->type )
  {
  case WLANTL_TX_SIG_SUSPEND:
    vosStatus = WLANTL_SuspendCB( pvosGCtx,
                                 (WLANTL_SuspendCBType)message->bodyptr,
                                 message->reserved);
    break;
  case WLANTL_TX_RES_NEEDED:
    vosStatus = WLANTL_GetTxResourcesCB( pvosGCtx );
     break;
  
  case WLANTL_TX_FWD_CACHED:
    /*                                                                     
                                                                 
                                                             
                                   
                                                                         */
    uData       = (v_U32_t)message->bodyptr; 
    ucSTAId     = ( uData & 0x000000FF); 
    ucUcastSig  = ( uData & 0x0000FF00)>>8; 
    ucBcastSig  = (v_U8_t)(( uData & 0x00FF0000)>>16); 
    vosStatus   = WLANTL_ForwardSTAFrames( pvosGCtx, ucSTAId, 
                                           ucUcastSig, ucBcastSig);
    break;
  case WDA_DS_TX_START_XMIT:
      WLANTL_ClearTxXmitPending(pvosGCtx);
      vosStatus = WDA_DS_TxFrames( pvosGCtx );
      break;

  case WDA_DS_FINISH_ULA:
    callbackContext = (void *)message->bodyval;
    callbackRoutine = message->bodyptr;
    callbackRoutine(callbackContext);
    break;

  case WLANTL_TX_SNAPSHOT:
    /*                                             
                  */
    WLANTL_TxThreadDebugHandler(pvosGCtx);
    WDA_TransportChannelDebug(NULL, VOS_TRUE, VOS_FALSE);
    break;

  default:
    /*                     */
    break;
  }

  return vosStatus;
}/*                     */

/*                                                                          
                              

             
                                                                           
                                                                          
                                                                           
               

              
                                                                  

            

      
                                                                       
                                                                   
                                                   


              
                                                            

                                               

              

                                                                            */
VOS_STATUS
WLANTL_TxFreeMsg
(
  v_PVOID_t        pvosGCtx,
  vos_msg_t*       message
)
{
  /*                                                                       */

  /*                        */
  return VOS_STATUS_SUCCESS;
}/*                */

/*                                                                          

                              

             
                                                      
                                                 

              
                                                             
                                           

            

      
                                                                       
                                                                   

              
                                                            

                                                  
                                             
                                             

              
                                                                                                
                

                                                                            */
VOS_STATUS
WLANTL_TxFCFrame
(
  v_PVOID_t       pvosGCtx
)
{
#if 0
  WLANTL_CbType*      pTLCb = NULL;
  VOS_STATUS          vosStatus;
  tpHalFcTxBd         pvFcTxBd = NULL;
  vos_pkt_t *         pPacket = NULL;
  v_U8_t              ucSTAId = 0;
  v_U8_t              ucBitCheck = 1;

  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO,
               "WLAN TL: Send FC frame %s", __func__);

  /*                                                                        
                
                                                                            */
  if ( NULL == pvosGCtx )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter %s", __func__));
    return VOS_STATUS_E_INVAL;
  }
  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);

  if (NULL == pTLCb)
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
        "WLAN TL:Invalid pointer in %s \n", __func__));
    return VOS_STATUS_E_INVAL;
  }
  
  //                   
  vosStatus = vos_pkt_get_packet( &pPacket, VOS_PKT_TYPE_TX_802_11_MGMT, sizeof(tHalFcTxBd), 1, 
                                    VOS_FALSE, NULL, NULL );

  if ( VOS_STATUS_SUCCESS != vosStatus )
  {
    return VOS_STATUS_E_INVAL;
  }

  vosStatus = vos_pkt_reserve_head( pPacket, (void *)&pvFcTxBd, sizeof(tHalFcTxBd));

  if( VOS_STATUS_SUCCESS != vosStatus )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "%s: failed to reserve FC TX BD %d\n",__func__, sizeof(tHalFcTxBd)));
    vos_pkt_return_packet( pPacket );
    return VOS_STATUS_E_FAULT;
  }

  //                                                       
  pTLCb->tlFCInfo.fcSTAThreshEnabledMask = 0;
  pTLCb->tlFCInfo.fcSTATxMoreDataMask = 0;
  for( ucSTAId = 0, ucBitCheck = 1 ; ucSTAId < WLAN_MAX_STA_COUNT; ucBitCheck <<= 1, ucSTAId ++)
  {
    if (0 == pTLCb->atlSTAClients[ucSTAId].ucExists)
    {
      continue;
    }

    if (pTLCb->atlSTAClients[ucSTAId].ucPktPending)
    {
      pTLCb->tlFCInfo.fcSTATxMoreDataMask |= ucBitCheck;
    }

    if ( (pTLCb->atlSTAClients[ucSTAId].ucLwmModeEnabled) &&
         (pTLCb->atlSTAClients[ucSTAId].bmuMemConsumed > pTLCb->atlSTAClients[ucSTAId].uLwmThreshold))
    {
      pTLCb->tlFCInfo.fcSTAThreshEnabledMask |= ucBitCheck;

      pTLCb->tlFCInfo.fcSTAThresh[ucSTAId] = (tANI_U8)pTLCb->atlSTAClients[ucSTAId].uLwmThreshold;

      pTLCb->atlSTAClients[ucSTAId].ucLwmEventReported = FALSE;
    }

  }
  
  //                          
  pTLCb->tlFCInfo.fcConfig |= 0x4;                               

  //                  
  vosStatus = WLANHAL_FillFcTxBd(pvosGCtx, &pTLCb->tlFCInfo, (void *)pvFcTxBd);

  if( VOS_STATUS_SUCCESS != vosStatus )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "%s: Fill FC TX BD unsuccessful\n", __func__));
    vos_pkt_return_packet( pPacket );
    return VOS_STATUS_E_FAULT;
  }

  if (NULL != pTLCb->vosTxFCBuf)
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "%s: Previous FC TX BD not sent\n", __func__));
    vos_pkt_return_packet(pTLCb->vosTxFCBuf);
  }

  pTLCb->vosTxFCBuf = pPacket;

  vos_pkt_set_user_data_ptr( pPacket, VOS_PKT_USER_DATA_ID_TL,
                               (v_PVOID_t)WLANTL_TxCompDefaultCb);
  vosStatus = WDA_DS_StartXmit(pvosGCtx);

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL: send FC frame leave %s", __func__));
#endif
  return VOS_STATUS_SUCCESS;
}


/*                                                                          
                                     

             
                                                                            
                                    

              
                                                                  

            

      
                                                                       
                                                                   


              
                                                            

                                                                         
                                    
                                               

                                                                           
                                  
              

                                                                            */
VOS_STATUS
WLANTL_GetTxResourcesCB
(
  v_PVOID_t        pvosGCtx
)
{
  WLANTL_CbType*  pTLCb      = NULL;
  v_U32_t         uResCount  = WDA_TLI_MIN_RES_DATA;
  VOS_STATUS      vosStatus  = VOS_STATUS_SUCCESS;
  v_U8_t          ucMgmt     = 0;
  v_U8_t          ucBAP      = 0;
  v_U8_t          ucData     = 0;
#ifdef WLAN_SOFTAP_FLOWCTRL_EN
  tBssSystemRole systemRole;
  tpAniSirGlobal pMac;
#endif
  /*                                                                       */
  /*                                                                        
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid TL pointer from pvosGCtx on"
               " WLANTL_ProcessTxMessage"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                                                        
                             
                                                                           */
  vosStatus = WDA_DS_GetTxResources( pvosGCtx, &uResCount );

  if ( (VOS_STATUS_SUCCESS != vosStatus) && (VOS_STATUS_E_RESOURCES != vosStatus))
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:TL failed to get resources from BAL, Err: %d",
               vosStatus));
    return vosStatus;
  }

  /*                                                                                
                                                                                      
                                                       */
  if (VOS_STATUS_E_RESOURCES == vosStatus)
  {
#ifdef VOLANS_PERF
     WLANHAL_EnableIdleBdPduInterrupt(pvosGCtx, (tANI_U8)bdPduInterruptGetThreshold);
     VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL: Enabling Idle BD/PDU interrupt, Current resources = %d", uResCount);
#else
    return VOS_STATUS_E_FAILURE;
#endif
  }

  pTLCb->uResCount = uResCount;
  

#ifdef WLAN_SOFTAP_FLOWCTRL_EN
  /*                                                                            */ 
  pTLCb->sendFCFrame ++;
  pMac = vos_get_context(VOS_MODULE_ID_WDA, pvosGCtx);
  systemRole = wdaGetGlobalSystemRole(pMac);
  if (eSYSTEM_AP_ROLE == systemRole)
  {
     if (pTLCb->sendFCFrame % 16 == 0)
     {
         TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                "Transmit FC"));
         WLANTL_TxFCFrame (pvosGCtx);
     } 
  }
#endif //                        

  ucData = ( pTLCb->uResCount >=  WDA_TLI_MIN_RES_DATA );
  ucBAP  = ( pTLCb->uResCount >=  WDA_TLI_MIN_RES_BAP ) &&
           ( NULL != pTLCb->tlBAPClient.vosPendingDataBuff );
  ucMgmt = ( pTLCb->uResCount >=  WDA_TLI_MIN_RES_MF ) &&
           ( NULL != pTLCb->tlMgmtFrmClient.vosPendingDataBuff );

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL: Eval Resume tx Res: %d DATA: %d BAP: %d MGMT: %d",
             pTLCb->uResCount, ucData, ucBAP, ucMgmt));

  if (( 0 == pTLCb->ucTxSuspended ) &&
      (( 0 != ucData ) || ( 0 != ucMgmt ) || ( 0 != ucBAP ) ) )
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "Issuing Xmit start request to BAL for avail res SYNC"));
    vosStatus =WDA_DS_StartXmit(pvosGCtx);
  }
  return vosStatus;
}/*                       */

/*                                                                          
                       
                                                                            */

/*                                                                          
                                               

             
                                                                           

              


            

     
                                      
          
                                                                          
                                     

         
                                                                            

      
                                             

              

                                              

                                                                             
                                     

              

                                                                            */
VOS_STATUS
WLANTL_Translate8023To80211Header
(
  vos_pkt_t*      vosDataBuff,
  VOS_STATUS*     pvosStatus,
  WLANTL_CbType*  pTLCb,
  v_U8_t          *pucStaId,
  WLANTL_MetaInfoType *tlMetaInfo,
  v_U8_t          *ucWDSEnabled,
  v_U8_t          *extraHeadSpace
)
{
  WLANTL_8023HeaderType  w8023Header;
  WLANTL_80211HeaderType *pw80211Header; //                                          
  VOS_STATUS             vosStatus;
  v_U8_t                 MandatoryucHeaderSize = WLAN80211_MANDATORY_HEADER_SIZE;
  v_U8_t                 ucHeaderSize = 0;
  v_VOID_t               *ppvBDHeader = NULL;
  WLANTL_STAClientType*  pClientSTA = NULL;
  v_U8_t                 ucQoSOffset = WLAN80211_MANDATORY_HEADER_SIZE;
  v_U8_t                 ucStaId;
#ifdef FEATURE_WLAN_CCX_UPLOAD
  v_BOOL_t               bIAPPTxwithLLC = VOS_FALSE;
  v_SIZE_t               wIAPPSnapSize = WLANTL_LLC_HEADER_LEN;
  v_U8_t                 wIAPPSnap[WLANTL_LLC_HEADER_LEN] = {0};
#endif
  *ucWDSEnabled = 0; //                 
  vosStatus = vos_pkt_pop_head( vosDataBuff, &w8023Header,
                                sizeof(w8023Header));

  if ( VOS_STATUS_SUCCESS != vosStatus )
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
     "WLAN TL: Packet pop header fails on WLANTL_Translate8023To80211Header"));
     return vosStatus;
  }

  if( NULL == pucStaId )
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
     "WLAN TL: Invalid pointer for StaId"));
     return VOS_STATUS_E_INVAL;
  }
  ucStaId = *pucStaId;
  pClientSTA = pTLCb->atlSTAClients[ucStaId];

  if ( NULL == pClientSTA )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

#ifdef FEATURE_WLAN_TDLS

  if ( WLAN_STA_INFRA == pTLCb->atlSTAClients[ucStaId]->wSTADesc.wSTAType
      && pTLCb->ucTdlsPeerCount )
  {
    v_U8_t ucIndex = 0;
    for ( ucIndex = 0; ucIndex < WLAN_MAX_STA_COUNT ; ucIndex++)
    {
      if ( ucIndex != ucStaId && pTLCb->atlSTAClients[ucIndex] && pTLCb->atlSTAClients[ucIndex]->ucExists &&
          (pTLCb->atlSTAClients[ucIndex]->tlState == WLANTL_STA_AUTHENTICATED) &&
          (!pTLCb->atlSTAClients[ucIndex]->ucTxSuspended) &&
          vos_mem_compare( (void*)pTLCb->atlSTAClients[ucIndex]->wSTADesc.vSTAMACAddress.bytes,
            (void*)w8023Header.vDA, 6) )
      {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_LOW,
              "WLAN TL: Got a TDLS station. Using that index"));
        ucStaId = ucIndex;
        *pucStaId = ucStaId;
        pClientSTA = pTLCb->atlSTAClients[ucStaId];
        if ( NULL == pClientSTA )
        {
            TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL:Client Memory was not allocated on %s", __func__));
            return VOS_STATUS_E_FAILURE;
        }
        break;
      }
    }
  }
#endif

#ifdef FEATURE_WLAN_CCX_UPLOAD
if ((0 == w8023Header.usLenType) && (pClientSTA->wSTADesc.ucIsCcxSta))
{
    vos_pkt_extract_data(vosDataBuff,0,&wIAPPSnap[0],&wIAPPSnapSize);
    if (vos_mem_compare(wIAPPSnap,WLANTL_AIRONET_SNAP_HEADER,WLANTL_LLC_HEADER_LEN))
    {
        /*                                                              
                                                                                        */
         bIAPPTxwithLLC = VOS_TRUE;
    }
    else
    {
        bIAPPTxwithLLC = VOS_FALSE;
    }
}
#endif /*                         */

  if ((0 != pClientSTA->wSTADesc.ucAddRmvLLC)
#ifdef FEATURE_WLAN_CCX_UPLOAD
      && (!bIAPPTxwithLLC)
#endif /*                         */
     )
  {
    /*                 */
    vosStatus = vos_pkt_push_head(vosDataBuff,
                    &w8023Header.usLenType, sizeof(w8023Header.usLenType));

    if ( VOS_STATUS_SUCCESS != vosStatus )
    {
       TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "WLAN TL: Packet push ether type fails on"
                  " WLANTL_Translate8023To80211Header"));
       return vosStatus;
    }

#ifdef BTAMP_TEST
    //                                                                  
    //                                                           
    {
    static v_U8_t WLANTL_BT_AMP_LLC_HEADER[] =  {0xAA, 0xAA, 0x03, 0x00, 0x19, 0x58 };
    vosStatus = vos_pkt_push_head(vosDataBuff, WLANTL_BT_AMP_LLC_HEADER,
                       sizeof(WLANTL_BT_AMP_LLC_HEADER));

    if ( VOS_STATUS_SUCCESS != vosStatus )
    {
       TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "WLAN TL: Packet push LLC header fails on"
                  " WLANTL_Translate8023To80211Header"));
       return vosStatus;
    }
    }
#else
    vosStatus = vos_pkt_push_head(vosDataBuff, WLANTL_LLC_HEADER,
                       sizeof(WLANTL_LLC_HEADER));

    if ( VOS_STATUS_SUCCESS != vosStatus )
    {
       TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "WLAN TL: Packet push LLC header fails on"
                  " WLANTL_Translate8023To80211Header"));
       return vosStatus;
    }
#endif
  }/*                     */
  else
  {
#ifdef FEATURE_WLAN_CCX_UPLOAD
      bIAPPTxwithLLC = VOS_FALSE; /*                                                        */
#endif /*                         */
       TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLAN TL: STA Client registered to not remove LLC"
                  " WLANTL_Translate8023To80211Header"));
  }

#ifdef BTAMP_TEST
  pClientSTA->wSTADesc.wSTAType = WLAN_STA_BT_AMP;
#endif

  //                                                     
  //                                   
  ucHeaderSize = MandatoryucHeaderSize;
  if (pClientSTA->wSTADesc.ucQosEnabled)
  {  
    ucHeaderSize += sizeof(pw80211Header->usQosCtrl);
  }
  if (pClientSTA->wSTADesc.wSTAType == WLAN_STA_BT_AMP)
  {  
    ucHeaderSize += sizeof(pw80211Header->optvA4);
    ucQoSOffset += sizeof(pw80211Header->optvA4);
  }

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
      " WLANTL_Translate8023To80211Header : Header size = %d ", ucHeaderSize));

  vos_pkt_reserve_head( vosDataBuff, &ppvBDHeader, ucHeaderSize );
  if ( NULL == ppvBDHeader )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL:VOSS packet corrupted "));
    *pvosStatus = VOS_STATUS_E_INVAL;
    return *pvosStatus;
  }


  //                                                
  /*         */
  pw80211Header = (WLANTL_80211HeaderType *)(ppvBDHeader);
  //                               
  vos_mem_set( pw80211Header, ucHeaderSize, 0 );
  vos_mem_copy( pw80211Header->vA2, w8023Header.vSA,  VOS_MAC_ADDR_SIZE);


#ifdef FEATURE_WLAN_WAPI
  if (( WLANTL_STA_AUTHENTICATED == pClientSTA->tlState ||
        pClientSTA->ptkInstalled ) && gUcIsWai != 1)
#else
  if ( WLANTL_STA_AUTHENTICATED == pClientSTA->tlState ||
       pClientSTA->ptkInstalled )
#endif
  {
    pw80211Header->wFrmCtrl.wep =
                 pClientSTA->wSTADesc.ucProtectedFrame;
  }

  pw80211Header->usDurationId = 0;
  pw80211Header->usSeqCtrl    = 0;

  pw80211Header->wFrmCtrl.type     = WLANTL_80211_DATA_TYPE;



  if(pClientSTA->wSTADesc.ucQosEnabled)
  {
      pw80211Header->wFrmCtrl.subType  = WLANTL_80211_DATA_QOS_SUBTYPE;

      *((v_U16_t *)((v_U8_t *)ppvBDHeader + ucQoSOffset)) = tlMetaInfo->ucUP;

  }
  else
  {
      pw80211Header->wFrmCtrl.subType  = 0;

  //                                                                        
  //                                                                          
  //                                      
  }


  switch( pClientSTA->wSTADesc.wSTAType )
  {
     case WLAN_STA_IBSS:
        pw80211Header->wFrmCtrl.toDS          = 0;
        pw80211Header->wFrmCtrl.fromDS        = 0;

        /*
                                                                
                                                                           
                                                            
         */
        if (0 == tlMetaInfo->ucBcast && 1 == tlMetaInfo->ucMcast)
        {
           vos_copy_macaddr( (v_MACADDR_t*)&pw80211Header->vA1,
                             (v_MACADDR_t*)&w8023Header.vDA);
        }
        else
        {
           vos_copy_macaddr( (v_MACADDR_t*)&pw80211Header->vA1,
                 &pClientSTA->wSTADesc.vSTAMACAddress);
        }
        vos_mem_copy( pw80211Header->vA3,
              &pClientSTA->wSTADesc.vBSSIDforIBSS ,
              VOS_MAC_ADDR_SIZE);
        break;

     case WLAN_STA_BT_AMP:
        *ucWDSEnabled = 1; //        
        pw80211Header->wFrmCtrl.toDS          = 1;
        pw80211Header->wFrmCtrl.fromDS        = 1;
        vos_copy_macaddr( (v_MACADDR_t*)&pw80211Header->vA1,
              &pClientSTA->wSTADesc.vSTAMACAddress);
        vos_mem_copy( pw80211Header->vA2,
              w8023Header.vSA, VOS_MAC_ADDR_SIZE);
        vos_copy_macaddr( (v_MACADDR_t*)&pw80211Header->vA3,
              &pClientSTA->wSTADesc.vSTAMACAddress);
        /*                             */
        vos_mem_copy( pw80211Header->optvA4,
              w8023Header.vSA, VOS_MAC_ADDR_SIZE);
        TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "BTAMP CASE NOW ---------staid=%d\n",
                 ucStaId));
        break;

     case WLAN_STA_SOFTAP:
        *ucWDSEnabled = 0; //         
        pw80211Header->wFrmCtrl.toDS          = 0;
        pw80211Header->wFrmCtrl.fromDS        = 1;
        /*                 */
        vos_mem_copy( pw80211Header->vA1, w8023Header.vDA , VOS_MAC_ADDR_SIZE);   
        vos_copy_macaddr( (v_MACADDR_t*)&pw80211Header->vA2,
              &pClientSTA->wSTADesc.vSelfMACAddress);
        vos_mem_copy( pw80211Header->vA3,
              w8023Header.vSA, VOS_MAC_ADDR_SIZE);
        TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "sw 802 to 80211 softap case  ---------staid=%d\n",
                 ucStaId));
        break;
#ifdef FEATURE_WLAN_TDLS
     case WLAN_STA_TDLS:
        pw80211Header->wFrmCtrl.toDS          = 0;
        pw80211Header->wFrmCtrl.fromDS        = 0;
        /*      */
        vos_copy_macaddr( (v_MACADDR_t*)&pw80211Header->vA1,
              &pClientSTA->wSTADesc.vSTAMACAddress);
        vos_mem_copy( pw80211Header->vA3,
              &pClientSTA->wSTADesc.vBSSIDforIBSS ,
              VOS_MAC_ADDR_SIZE);
        VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
              ("TL:TDLS CASE NOW ---------staid=%d\n"), ucStaId);
        break;
#endif
     case WLAN_STA_INFRA:
     default:
        pw80211Header->wFrmCtrl.toDS          = 1;
        pw80211Header->wFrmCtrl.fromDS        = 0;
        vos_copy_macaddr( (v_MACADDR_t*)&pw80211Header->vA1,
              &pClientSTA->wSTADesc.vSTAMACAddress);
        vos_mem_copy( pw80211Header->vA3, w8023Header.vDA , VOS_MAC_ADDR_SIZE);
        TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "REGULAR INFRA LINK CASE---------staid=%d\n",
                 ucStaId));
        break;
  }
  //                                                
  /*         */
  pw80211Header = (WLANTL_80211HeaderType *)(ppvBDHeader);
  return VOS_STATUS_SUCCESS;
}/*                                 */


/*                                                                             
                                                  
                                                                             */
#if 0 //                

#define WLANTL_DEBUG_FRAME_BYTE_PER_LINE    16
#define WLANTL_DEBUG_FRAME_BYTE_PER_BYTE    4

static v_VOID_t WLANTL_DebugFrame
(
   v_PVOID_t   dataPointer,
   v_U32_t     dataSize
)
{
   v_U8_t   lineBuffer[WLANTL_DEBUG_FRAME_BYTE_PER_LINE];
   v_U32_t  numLines;
   v_U32_t  numBytes;
   v_U32_t  idx;
   v_U8_t   *linePointer;

   numLines = dataSize / WLANTL_DEBUG_FRAME_BYTE_PER_LINE;
   numBytes = dataSize % WLANTL_DEBUG_FRAME_BYTE_PER_LINE;
   linePointer = (v_U8_t *)dataPointer;

   TLLOGE(VOS_TRACE(VOS_MODULE_ID_SAL, VOS_TRACE_LEVEL_ERROR, "WLAN TL:Frame Debug Frame Size %d, Pointer 0x%p", dataSize, dataPointer));
   for(idx = 0; idx < numLines; idx++)
   {
      memset(lineBuffer, 0, WLANTL_DEBUG_FRAME_BYTE_PER_LINE);
      memcpy(lineBuffer, linePointer, WLANTL_DEBUG_FRAME_BYTE_PER_LINE);
      TLLOGE(VOS_TRACE(VOS_MODULE_ID_SAL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL:0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x 0x%2x",
                 lineBuffer[0], lineBuffer[1], lineBuffer[2], lineBuffer[3], lineBuffer[4], lineBuffer[5], lineBuffer[6], lineBuffer[7],
                 lineBuffer[8], lineBuffer[9], lineBuffer[10], lineBuffer[11], lineBuffer[12], lineBuffer[13], lineBuffer[14], lineBuffer[15]));
      linePointer += WLANTL_DEBUG_FRAME_BYTE_PER_LINE;
   }

   if(0 == numBytes)
      return;

   memset(lineBuffer, 0, WLANTL_DEBUG_FRAME_BYTE_PER_LINE);
   memcpy(lineBuffer, linePointer, numBytes);
   for(idx = 0; idx < WLANTL_DEBUG_FRAME_BYTE_PER_LINE / WLANTL_DEBUG_FRAME_BYTE_PER_BYTE; idx++)
   {
      TLLOGE(VOS_TRACE(VOS_MODULE_ID_SAL, VOS_TRACE_LEVEL_ERROR, "WLAN TL:0x%2x 0x%2x 0x%2x 0x%2x",
                lineBuffer[idx * WLANTL_DEBUG_FRAME_BYTE_PER_BYTE], lineBuffer[1 + idx * WLANTL_DEBUG_FRAME_BYTE_PER_BYTE],
                lineBuffer[2 + idx * WLANTL_DEBUG_FRAME_BYTE_PER_BYTE], lineBuffer[3 + idx * WLANTL_DEBUG_FRAME_BYTE_PER_BYTE]));
      if(((idx + 1) * WLANTL_DEBUG_FRAME_BYTE_PER_BYTE) >= numBytes)
         break;
   }

   return;
}
#endif

/*                                                                             
                   
                                                                             */

/*                                                                          
                                               

             
                                                                           

              


            

     
                                      
                                
                                                  
                                                                              

         
                                                                            

      
                                             

               

                                                            
                                              

              

                                                                            */
VOS_STATUS
WLANTL_Translate80211To8023Header
(
  vos_pkt_t*      vosDataBuff,
  VOS_STATUS*     pvosStatus,
  v_U16_t         usActualHLen,
  v_U8_t          ucHeaderLen,
  WLANTL_CbType*  pTLCb,
  v_U8_t          ucSTAId,
  v_BOOL_t        bForwardIAPPwithLLC
)
{
  WLANTL_8023HeaderType  w8023Header;
  WLANTL_80211HeaderType w80211Header;
  v_U8_t                 aucLLCHeader[WLANTL_LLC_HEADER_LEN];
  VOS_STATUS             vosStatus;
  v_U16_t                usDataStartOffset = 0;
  /*                                                                       */

  if ( sizeof(w80211Header) < ucHeaderLen )
  {
     TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
       "Warning !: Check the header size for the Rx frame structure=%d received=%dn",
       sizeof(w80211Header), ucHeaderLen));
     ucHeaderLen = sizeof(w80211Header);
  }

  //                                                                    
  //                                                                                   
  vosStatus = vos_pkt_pop_head( vosDataBuff, &w80211Header, ucHeaderLen);

  if ( VOS_STATUS_SUCCESS != vosStatus )
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL: Failed to pop 80211 header from packet %d",
                vosStatus));

     return vosStatus;
  }

  switch ( w80211Header.wFrmCtrl.fromDS )
  {
  case 0:
    if ( w80211Header.wFrmCtrl.toDS )
    {
      //              
      vos_mem_copy( w8023Header.vDA, w80211Header.vA3, VOS_MAC_ADDR_SIZE);
      vos_mem_copy( w8023Header.vSA, w80211Header.vA2, VOS_MAC_ADDR_SIZE);
      TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                  "WLAN TL SoftAP: 802 3 DA %08x SA %08x \n",
                  w8023Header.vDA, w8023Header.vSA));
    }
    else 
    {
      /*      */
      vos_mem_copy( w8023Header.vDA, w80211Header.vA1, VOS_MAC_ADDR_SIZE);
      vos_mem_copy( w8023Header.vSA, w80211Header.vA2, VOS_MAC_ADDR_SIZE);
    }
    break;
  case 1:
    if ( w80211Header.wFrmCtrl.toDS )
    {
      /*             */
      vos_mem_copy( w8023Header.vDA, w80211Header.vA1, VOS_MAC_ADDR_SIZE);
      vos_mem_copy( w8023Header.vSA, w80211Header.vA2, VOS_MAC_ADDR_SIZE);
    }
    else
    { /*       */
      vos_mem_copy( w8023Header.vDA, w80211Header.vA1, VOS_MAC_ADDR_SIZE);
      vos_mem_copy( w8023Header.vSA, w80211Header.vA3, VOS_MAC_ADDR_SIZE);
    }
    break;
  }

  if( usActualHLen > ucHeaderLen )
  {
     usDataStartOffset = usActualHLen - ucHeaderLen;
  }

  if ( 0 < usDataStartOffset )
  {
    vosStatus = vos_pkt_trim_head( vosDataBuff, usDataStartOffset );

    if ( VOS_STATUS_SUCCESS != vosStatus )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "WLAN TL: Failed to trim header from packet %d",
                  vosStatus));
      return vosStatus;
    }
  }

  if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  if ( 0 != pTLCb->atlSTAClients[ucSTAId]->wSTADesc.ucAddRmvLLC
#ifdef FEATURE_WLAN_CCX_UPLOAD
    && (!bForwardIAPPwithLLC)
#endif /*                          */
     )
  {
    //                       
    vosStatus = vos_pkt_pop_head( vosDataBuff, aucLLCHeader,
                                  WLANTL_LLC_HEADER_LEN);

    if ( VOS_STATUS_SUCCESS != vosStatus )
    {
       TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "WLAN TL: Failed to pop LLC header from packet %d",
                  vosStatus));

       return vosStatus;
    }

    //                  
    vos_mem_copy(&w8023Header.usLenType,
      &aucLLCHeader[WLANTL_LLC_HEADER_LEN - sizeof(w8023Header.usLenType)],
      sizeof(w8023Header.usLenType) );
  }
  else
  {
    vosStatus = vos_pkt_get_packet_length(vosDataBuff,
                                        &w8023Header.usLenType);

    if ( VOS_STATUS_SUCCESS != vosStatus )
    {
       TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "WLAN TL: Failed to get packet length %d",
                  vosStatus));

       return vosStatus;
    }

    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
        "WLAN TL: BTAMP len (ethertype) fld = %d",
        w8023Header.usLenType));
    w8023Header.usLenType = vos_cpu_to_be16(w8023Header.usLenType);
  }

  vos_pkt_push_head(vosDataBuff, &w8023Header, sizeof(w8023Header));

#ifdef BTAMP_TEST
  {
  //                           
  v_U8_t *temp_w8023Header = NULL;
  vosStatus = vos_pkt_peek_data( vosDataBuff, 0,
    &temp_w8023Header, sizeof(w8023Header) );
  }
#endif
#if 0 /*        */
  vos_pkt_get_packet_length(vosDataBuff, &usLen);
  vos_pkt_pop_head( vosDataBuff, aucData, usLen);

  WLANTL_DebugFrame(aucData, usLen);

  vos_pkt_push_head(vosDataBuff, aucData, usLen);

#endif

  *pvosStatus = VOS_STATUS_SUCCESS;

  return VOS_STATUS_SUCCESS;
}/*                                 */

/*                                                                          
                                        

             
                                                                           
               

              
                                                                          

            

     
                                                    
                                                            
                                            

         
                                                                         

              
                                                            

                                                  
                                                                 
                                 

              
         
                                                                            */
VOS_STATUS
WLANTL_FindFrameTypeBcMcUc
(
  WLANTL_CbType *pTLCb,
  v_U8_t        ucSTAId,
  vos_pkt_t     *vosDataBuff,
  v_U8_t        *pucBcMcUc
)
{
   VOS_STATUS    vosStatus = VOS_STATUS_SUCCESS;
   v_PVOID_t     aucBDHeader;
   v_PVOID_t     pvPeekData;
   /*                                                                       */

   /*                                                                        
                 
                                                                            */
   if ((NULL == pTLCb) || 
       (NULL == vosDataBuff) || 
       (NULL == pucBcMcUc))
   {
      TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
             "WLAN TL:Invalid parameter in WLANTL_FindFrameTypeBcMcUc"));
      return VOS_STATUS_E_INVAL;
   }

   if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
   {
       TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
           "WLAN TL:Client Memory was not allocated on %s", __func__));
       return VOS_STATUS_E_FAILURE;
   }

   /*                                                                        
                                         
                                                                            */
   vosStatus = WDA_DS_PeekRxPacketInfo(vosDataBuff, (v_PVOID_t)&aucBDHeader, 0/*       */ );

   if (NULL == aucBDHeader)
   {
      TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
             "WLAN TL:WLANTL_FindFrameTypeBcMcUc - Cannot extract BD header"));
      VOS_ASSERT(0);
      return VOS_STATUS_E_BADMSG;
   }

   if ((0 == WDA_GET_RX_FT_DONE(aucBDHeader)) &&
       (0 != pTLCb->atlSTAClients[ucSTAId]->wSTADesc.ucSwFrameRXXlation))
   {
      /*                                            */
      TLLOG2(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:WLANTL_FindFrameTypeBcMcUc - 802.11 frame, peeking Addr1"));
      vosStatus = vos_pkt_peek_data(vosDataBuff, WLANTL_MAC_ADDR_ALIGN(1), 
                                    (v_PVOID_t)&pvPeekData, VOS_MAC_ADDR_SIZE);
   }
   else
   {
      /*                                                     */
      TLLOG2(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:WLANTL_FindFrameTypeBcMcUc - 802.3 frame, peeking DA"));
      vosStatus = vos_pkt_peek_data(vosDataBuff, WLANTL_MAC_ADDR_ALIGN(0),
                                    (v_PVOID_t)&pvPeekData, VOS_MAC_ADDR_SIZE);
   }

   if (VOS_STATUS_SUCCESS != vosStatus) 
   {
      TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
             "WLAN TL:WLANTL_FindFrameTypeBcMcUc - Failed to peek MAC address"));
      return vosStatus;
   }

   if (((tANI_U8 *)pvPeekData)[0] == 0xff)
   {
      *pucBcMcUc = WLANTL_FRAME_TYPE_BCAST;
   }
   else
   {
      if ((((tANI_U8 *)pvPeekData)[0] & 0x01) == 0x01)
         *pucBcMcUc = WLANTL_FRAME_TYPE_MCAST;
      else
         *pucBcMcUc = WLANTL_FRAME_TYPE_UCAST;
   }

   TLLOG2(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
          "WLAN TL:WLANTL_FindFrameTypeBcMcUc - Addr1Byte1 is: %x", 
          ((tANI_U8 *)pvPeekData)[0]));

  return VOS_STATUS_SUCCESS;
}

#if 0
#ifdef WLAN_PERF 
/*                                                                          
                                       

              
                                                                            
                                                                              
                                                                            
                                                                           
                                                                             
                                                         

               
     
             

     
                               
                                      
                                           
                                         
    
      
                                   
                                                                          
   
              
                 

               
  
                                                                            */
static void
WLANTL_FastHwFwdDataFrame
( 
  v_PVOID_t     pvosGCtx,
  vos_pkt_t*    vosDataBuff,
  VOS_STATUS*   pvosStatus,
  v_U32_t*       puFastFwdOK,
  WLANTL_MetaInfoType*  pMetaInfo,
  WLAN_STADescType*  pStaInfo
 
)
{
    v_PVOID_t   pvPeekData;
    v_U8_t      ucDxEBDWLANHeaderLen = WLANTL_BD_HEADER_LEN(0) + sizeof(WLANBAL_sDXEHeaderType); 
    v_U8_t      ucIsUnicast;
    WLANBAL_sDXEHeaderType  *pDxEHeader;
    v_PVOID_t   pvBDHeader;
    v_PVOID_t   pucBuffPtr;
    v_U16_t      usPktLen;

   /*                                                                       
                         
                                                                           */

    vos_pkt_get_packet_length( vosDataBuff, &usPktLen);

   /*                                                                       
                       
                                                                           */
    *pvosStatus = vos_pkt_peek_data( vosDataBuff, 
                                 WLANTL_MAC_ADDR_ALIGN(0), 
                                 (v_PVOID_t)&pvPeekData, 
                                 VOS_MAC_ADDR_SIZE );

    if ( VOS_STATUS_SUCCESS != *pvosStatus ) 
    {
       TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                  "WLAN TL:Failed while attempting to extract MAC Addr %d", 
                  *pvosStatus));
       *pvosStatus = VOS_STATUS_E_INVAL;
       return;
    }

   /*                                                                       
                                                         
                                                                           */

    vos_pkt_reserve_head( vosDataBuff, &pucBuffPtr, 
                        ucDxEBDWLANHeaderLen );
    if ( NULL == pucBuffPtr )
    {
        TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                    "WLAN TL:No enough space in VOSS packet %p for DxE/BD/WLAN header", vosDataBuff));
       *pvosStatus = VOS_STATUS_E_INVAL;
        return;
    }
    pDxEHeader = (WLANBAL_sDXEHeaderType  *)pucBuffPtr;
    pvBDHeader = (v_PVOID_t) &pDxEHeader[1];

    /*                                 
                                                                           
                                                                       
                                                                        
                                          
     */
     if(pStaInfo->wSTAType == WLAN_STA_IBSS)
        ucIsUnicast = !(((tANI_U8 *)pvPeekData)[0] & 0x01);
     else
        ucIsUnicast = 1;
 
     *puFastFwdOK = (v_U32_t) WLANHAL_TxBdFastFwd(pvosGCtx, pvPeekData, pMetaInfo->ucTID, ucIsUnicast, pvBDHeader, usPktLen );
    
      /*                                                                       */
      if(! *puFastFwdOK){
          vos_pkt_trim_head(vosDataBuff, ucDxEBDWLANHeaderLen);
      }else{
        /*                                                                                       
         */
        v_U32_t uPacketSize = WLANTL_BD_HEADER_LEN(0) + usPktLen;
        vos_pkt_set_user_data_ptr( vosDataBuff, VOS_PKT_USER_DATA_ID_BAL, 
                       (v_PVOID_t)uPacketSize);
        pDxEHeader->size  = SWAP_ENDIAN_UINT32(uPacketSize);
      }
     *pvosStatus = VOS_STATUS_SUCCESS;
      return;
}
#endif /*         */
#endif

#if 0
/*                                                                          
                                     

             
                                                                  

              
                                                   

            

     
                                     
                                           

      
                                                   
                                                                 
                                                       
                                 

              
              

              

                                                                            */
void
WLANTL_PrepareBDHeader
(
  vos_pkt_t*      vosDataBuff,
  v_PVOID_t*      ppvBDHeader,
  v_MACADDR_t*    pvDestMacAdddr,
  v_U8_t          ucDisableFrmXtl,
  VOS_STATUS*     pvosStatus,
  v_U16_t*        pusPktLen,
  v_U8_t          ucQosEnabled,
  v_U8_t          ucWDSEnabled,
  v_U8_t          extraHeadSpace
)
{
  v_U8_t      ucHeaderOffset;
  v_U8_t      ucHeaderLen;
  v_U8_t      ucBDHeaderLen = WLANTL_BD_HEADER_LEN(ucDisableFrmXtl);

  /*                                                                       */
  /*                                                                         
                                
                                                   
                                                                            */
  vos_pkt_get_packet_length( vosDataBuff, pusPktLen);

  if ( WLANTL_MAC_HEADER_LEN(ucDisableFrmXtl) > *pusPktLen )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL: Length of the packet smaller than expected network"
               " header %d", *pusPktLen ));

    *pvosStatus = VOS_STATUS_E_INVAL;
    return;
  }

  vos_pkt_reserve_head( vosDataBuff, ppvBDHeader,
                        ucBDHeaderLen );
  if ( NULL == *ppvBDHeader )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL:VOSS packet corrupted on Attach BD header"));
    *pvosStatus = VOS_STATUS_E_INVAL;
    return;
  }

  /*                                                                       
                       
                                                                          */
  {
   v_SIZE_t usMacAddrSize = VOS_MAC_ADDR_SIZE;
   *pvosStatus = vos_pkt_extract_data( vosDataBuff,
                                     ucBDHeaderLen +
                                     WLANTL_MAC_ADDR_ALIGN(ucDisableFrmXtl),
                                     (v_PVOID_t)pvDestMacAdddr,
                                     &usMacAddrSize );
  }
  if ( VOS_STATUS_SUCCESS != *pvosStatus )
  {
     TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "WLAN TL:Failed while attempting to extract MAC Addr %d",
                *pvosStatus));
  }
  else
  {
    /*                                                                     
                              
                                  
                                    
                              
                                                              
                                                                        */
    ucHeaderOffset = ucBDHeaderLen;
    ucHeaderLen    = WLANTL_MAC_HEADER_LEN(ucDisableFrmXtl);

    if ( 0 != ucDisableFrmXtl )
    {
      if ( 0 != ucQosEnabled )
      {
        ucHeaderLen += WLANTL_802_11_HEADER_QOS_CTL;
      }

      //                                                  
      if ( ucWDSEnabled != 0 )
      {
        //                                     
        ucHeaderLen    += WLANTL_802_11_HEADER_ADDR4_LEN;
      }
      if ( extraHeadSpace != 0 )
      {
        //                                                                   
        *pusPktLen = *pusPktLen - extraHeadSpace;
      }
    }

    WLANHAL_TX_BD_SET_MPDU_HEADER_LEN( *ppvBDHeader, ucHeaderLen);
    WLANHAL_TX_BD_SET_MPDU_HEADER_OFFSET( *ppvBDHeader, ucHeaderOffset);
    WLANHAL_TX_BD_SET_MPDU_DATA_OFFSET( *ppvBDHeader,
                                          ucHeaderOffset + ucHeaderLen + extraHeadSpace);
    WLANHAL_TX_BD_SET_MPDU_LEN( *ppvBDHeader, *pusPktLen);

    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                "WLAN TL: VALUES ARE HLen=%x Hoff=%x doff=%x len=%x ex=%d",
                ucHeaderLen, ucHeaderOffset, 
                (ucHeaderOffset + ucHeaderLen + extraHeadSpace), 
                *pusPktLen, extraHeadSpace));
  }/*                    */

}/*                        */
#endif

//                                                    
/*                                                                          
                                 

             
                                                                                  

                                                         

                                                                                         
                                                                                       
                                                                                        
           
      
                                                                                          
                                                                                                
                                                                                                
                                         

              

            

     
                                                                    
                                                                

      
                          

              
                                                            

                                            

              
   
                                                                                             
                                                                               
                                                                               

                                                                            */
VOS_STATUS
WLAN_TLAPGetNextTxIds
(
  v_PVOID_t    pvosGCtx,
  v_U8_t*      pucSTAId
)
{
  WLANTL_CbType*  pTLCb;
  v_U8_t          ucACFilter = 1;
  v_U8_t          ucNextSTA ; 
  v_BOOL_t        isServed = TRUE;  //                                      
  v_U8_t          ucACLoopNum = WLANTL_AC_VO + 1; //                    
  v_U8_t          uFlowMask; //                     
  uint8           ucACMask; 
  uint8           i = 0; 
  /*                                                                        
                            
                                                                          */
  //        

  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
      "WLAN TL:Invalid TL pointer from pvosGCtx on WLAN_TLAPGetNextTxIds"));
    return VOS_STATUS_E_FAULT;
  }

  if ( VOS_STATUS_SUCCESS != WDA_DS_GetTxFlowMask( pvosGCtx, &uFlowMask ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
      "WLAN TL:Failed to retrieve Flow control mask from WDA"));
    return VOS_STATUS_E_FAULT;
  }

  ucNextSTA = pTLCb->ucCurrentSTA;

  ++ucNextSTA;

  if ( WLAN_MAX_STA_COUNT <= ucNextSTA )
  {
    //                  
    ucNextSTA = 0;
    pTLCb->ucCurLeftWeight--;
    isServed = FALSE;
    if ( 0 == pTLCb->ucCurLeftWeight )
    {
      //                        
      if ( WLANTL_AC_BK == (WLANTL_ACEnumType)pTLCb->uCurServedAC )
      {
        //                                                   
        pTLCb->uCurServedAC = WLANTL_AC_VO;
      }
      else 
      {
        pTLCb->uCurServedAC --;
      }

      pTLCb->ucCurLeftWeight =  pTLCb->tlConfigInfo.ucAcWeights[pTLCb->uCurServedAC];
 
    } //                              
  } //                                   

  //                                                                                  
  //                      
  if ((1 == pTLCb->ucCurLeftWeight) && (ucNextSTA != 0))
  {
    ucACLoopNum ++; //               
  }

  /*                                                                                    
                              */
  for (; ucACLoopNum > 0;  ucACLoopNum--)
  {

    ucACFilter = 1 << pTLCb->uCurServedAC;

    //                                               
    for (; (pTLCb->ucCurLeftWeight > 0) && (uFlowMask & ucACFilter); pTLCb->ucCurLeftWeight-- )
    {

      for ( ; ucNextSTA < WLAN_MAX_STA_COUNT; ucNextSTA ++ )
      {
        if(NULL == pTLCb->atlSTAClients[ucNextSTA])
        {
            continue;
        }
        WLAN_TL_AC_ARRAY_2_MASK (pTLCb->atlSTAClients[ucNextSTA], ucACMask, i);

        if ( (0 == pTLCb->atlSTAClients[ucNextSTA]->ucExists) ||
             ((0 == pTLCb->atlSTAClients[ucNextSTA]->ucPktPending) && !(ucACMask)) ||
             (0 == (ucACMask & ucACFilter)) )

        {
          //                                                           
          continue;
        }

        if (WLANTL_STA_AUTHENTICATED != pTLCb->atlSTAClients[ucNextSTA]->tlState)
        {
          TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "%s Sta %d not in auth state so skipping it.",
                 __func__, ucNextSTA));
          continue;
        }

        //                                                                    
        //                                                                                   
        //                                                                                        
        //                                   
        if ( (TRUE == pTLCb->atlSTAClients[ucNextSTA]->ucLwmModeEnabled) &&
             ((FALSE == pTLCb->atlSTAClients[ucNextSTA]->ucLwmEventReported) ||
                 (0 < pTLCb->atlSTAClients[ucNextSTA]->uBuffThresholdMax))
           )
        {
          continue;
        }


        //                                           
        *pucSTAId = ucNextSTA;
        pTLCb->ucCurrentSTA = ucNextSTA;
        pTLCb->atlSTAClients[*pucSTAId]->ucCurrentAC = pTLCb->uCurServedAC;
  
        TLLOG4(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_LOW,
                   " TL serve one station AC: %d  W: %d StaId: %d",
                   pTLCb->uCurServedAC, pTLCb->ucCurLeftWeight, pTLCb->ucCurrentSTA ));
      
        return VOS_STATUS_SUCCESS;
      } //        

      ucNextSTA = 0;
      if ( FALSE == isServed )
      {
        //                                                                    
        break;
      }
      //                                                   
      isServed = FALSE;

    } //           

    if (WLANTL_AC_BK == pTLCb->uCurServedAC)
    {
      pTLCb->uCurServedAC = WLANTL_AC_VO;
    }
    else
    {
      pTLCb->uCurServedAC--;
    }
    pTLCb->ucCurLeftWeight =  pTLCb->tlConfigInfo.ucAcWeights[pTLCb->uCurServedAC];

  }//        

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                   " TL can't find one station to serve \n" ));

  pTLCb->uCurServedAC = WLANTL_AC_BK;
  pTLCb->ucCurLeftWeight = 1;
  //                                         
  pTLCb->ucCurrentSTA = WLAN_MAX_STA_COUNT; 

  *pucSTAId = pTLCb->ucCurrentSTA;
  return VOS_STATUS_E_FAULT;
}


/*                                                                          
                                 

             
                                                 

              

            

     
                                                                    
                                                                

      
                          


              
                                                            

                                               

              

                                                                            */
VOS_STATUS
WLAN_TLGetNextTxIds
(
  v_PVOID_t    pvosGCtx,
  v_U8_t*      pucSTAId
)
{
  WLANTL_CbType*  pTLCb;
  v_U8_t          ucNextAC;
  v_U8_t          ucNextSTA; 
  v_U8_t          ucCount; 
  v_U8_t          uFlowMask; //                     
  v_U8_t          ucACMask = 0;
  v_U8_t          i = 0; 

  tBssSystemRole systemRole; //                     
  tpAniSirGlobal pMac;

  pMac = vos_get_context(VOS_MODULE_ID_PE, pvosGCtx);
  if ( NULL == pMac )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                      "%s: Invalid pMac", __func__));
    return VOS_STATUS_E_FAULT;
  }

  systemRole = wdaGetGlobalSystemRole(pMac);

  /*                                                                        
                            
                                                                          */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
      "WLAN TL:Invalid TL pointer from pvosGCtx on WLAN_TLGetNextTxIds"));
    return VOS_STATUS_E_FAULT;
  }

#ifdef FEATURE_WLAN_TDLS
  if ((eSYSTEM_AP_ROLE == systemRole) || (vos_concurrent_sessions_running()) || pTLCb->ucTdlsPeerCount)
#else
  if ((eSYSTEM_AP_ROLE == systemRole) || (vos_concurrent_sessions_running()))
#endif
  {
    return WLAN_TLAPGetNextTxIds(pvosGCtx,pucSTAId);
  }


  if ( VOS_STATUS_SUCCESS != WDA_DS_GetTxFlowMask( pvosGCtx, &uFlowMask ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
      "WLAN TL:Failed to retrieve Flow control mask from WDA"));
    return VOS_STATUS_E_FAULT;
  }

  /*                                     */
  /*                                                                       
                                                                    
                                                           
                                                                           */
  ucNextSTA = pTLCb->ucCurrentSTA;
  
  pTLCb->ucCurrentSTA = WLAN_MAX_STA_COUNT; 
  for ( ucCount = 0; 
        ucCount < WLAN_MAX_STA_COUNT;
        ucCount++ )
  {
    ucNextSTA = ( (ucNextSTA+1) >= WLAN_MAX_STA_COUNT )?0:(ucNextSTA+1);
    if(NULL == pTLCb->atlSTAClients[ucNextSTA])
    {
        continue;
    }
    if (( pTLCb->atlSTAClients[ucNextSTA]->ucExists ) &&
        ( pTLCb->atlSTAClients[ucNextSTA]->ucPktPending ))
    {
      if (WLANTL_STA_AUTHENTICATED == pTLCb->atlSTAClients[ucNextSTA]->tlState)
      {
        TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "STA ID: %d on WLAN_TLGetNextTxIds", *pucSTAId));
        pTLCb->ucCurrentSTA = ucNextSTA;
        break;
      }
      else
      {
        TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "%s Sta %d is not in auth state, skipping this sta.",
               __func__, ucNextSTA));
      }
    }
  }

  *pucSTAId = pTLCb->ucCurrentSTA;

  if ( WLANTL_STA_ID_INVALID( *pucSTAId ) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
      "WLAN TL:No station registered with TL at this point"));

    return VOS_STATUS_E_FAULT;

  }

  /*                                                */
  WLAN_TL_AC_ARRAY_2_MASK( pTLCb->atlSTAClients[*pucSTAId], ucACMask, i);
  
  if ( 0 == ucACMask )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
      "WLAN TL: Mask 0 "
      "STA ID: %d on WLAN_TLGetNextTxIds", *pucSTAId));

     /*                                      */
     *pucSTAId = WLAN_MAX_STA_COUNT;

     return VOS_STATUS_E_FAULT;
  }

  /*                                                                       
                                                                      
                                                                      
                                                            
                                                                         */
  if ( 0 < pTLCb->atlSTAClients[*pucSTAId]->ucCurrentWeight  )
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                  "WLAN TL: Maintaining serviced AC to: %d for Weight: %d",
                  pTLCb->atlSTAClients[*pucSTAId]->ucCurrentAC ,
                  pTLCb->atlSTAClients[*pucSTAId]->ucCurrentWeight));
    return VOS_STATUS_SUCCESS;
  }

  /*                                                                       
                                                 
                                                                         */
  ucNextAC = pTLCb->atlSTAClients[*pucSTAId]->ucCurrentAC;
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "Next AC: %d", ucNextAC));

  while ( 0 != ucACMask )
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             " AC Mask: %d Next: %d Res : %d",
               ucACMask, ( 1 << ucNextAC ), ( ucACMask & ( 1 << ucNextAC ))));

    if ( 0 != ( ucACMask & ( 1 << ucNextAC ) & uFlowMask ))
    {
       pTLCb->atlSTAClients[*pucSTAId]->ucCurrentAC     =
                                   (WLANTL_ACEnumType)ucNextAC;
       pTLCb->atlSTAClients[*pucSTAId]->ucCurrentWeight =
                       pTLCb->tlConfigInfo.ucAcWeights[ucNextAC];

        TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                  "WLAN TL: Switching serviced AC to: %d with Weight: %d",
                  pTLCb->atlSTAClients[*pucSTAId]->ucCurrentAC ,
                  pTLCb->atlSTAClients[*pucSTAId]->ucCurrentWeight));
       break;
    }

    ucNextAC = ( ucNextAC - 1 ) & WLANTL_MASK_AC;

    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "Next AC %d", ucNextAC));

  }

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             " C AC: %d C W: %d",
             pTLCb->atlSTAClients[*pucSTAId]->ucCurrentAC,
             pTLCb->atlSTAClients[*pucSTAId]->ucCurrentWeight));

  return VOS_STATUS_SUCCESS;
}/*                     */



/*                                                                          
                                                            
                                                                            */

/*                                                                          

                                       

             
                                                                             
                                                                       

              

            
             

              

                                        

                                                                            */
VOS_STATUS
WLANTL_MgmtFrmRxDefaultCb
(
  v_PVOID_t  pvosGCtx,
  v_PVOID_t  vosBuff
)
{
  if ( NULL != vosBuff )
  {
    TLLOGP(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
             "WLAN TL:Fatal failure: No registered Mgmt Frm client on pkt RX"));
    /*             */
    vos_pkt_return_packet((vos_pkt_t *)vosBuff);
  }

      TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
                 "WLAN TL: No registered Mgmt Frm client on pkt RX. Load/Unload in progress, Ignore"));

  return VOS_STATUS_E_FAILURE;
}/*                         */

/*                                                                          

                                  

             
                                                                        
                                                                  

              

            
             

              

                                        

                                                                            */
VOS_STATUS
WLANTL_BAPRxDefaultCb
(
  v_PVOID_t  pvosGCtx,
  vos_pkt_t*       vosDataBuff,
  WLANTL_BAPFrameEnumType frameType
)
{
  TLLOGP(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
             "WLAN TL:Fatal failure: No registered BAP client on BAP pkt RX"));
#ifndef BTAMP_TEST
  VOS_ASSERT(0);
#endif
  return VOS_STATUS_E_FAILURE;
}/*                         */

/*                                                                          

                                   

             
                                                                        
                                                                      
                                                        

              

            
             

              

                                         

                                                                            */
VOS_STATUS
WLANTL_STARxDefaultCb
(
  v_PVOID_t               pvosGCtx,
  vos_pkt_t*              vosDataBuff,
  v_U8_t                  ucSTAId,
  WLANTL_RxMetaInfoType*  pRxMetaInfo
)
{
  TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
       "WLAN TL: No registered STA client rx cb for STAID: %d dropping pkt",
               ucSTAId));
  vos_pkt_return_packet(vosDataBuff);
  return VOS_STATUS_SUCCESS;
}/*                         */


/*                                                                          

                                         

             
                                                                       
                                                                         
                                                        

              

            
             

              

                                         

                                                                            */
VOS_STATUS
WLANTL_STAFetchPktDefaultCb
(
  v_PVOID_t              pvosGCtx,
  v_U8_t*                pucSTAId,
  WLANTL_ACEnumType      ucAC,
  vos_pkt_t**            vosDataBuff,
  WLANTL_MetaInfoType*   tlMetaInfo
)
{
  TLLOGP(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
             "WLAN TL:Fatal failure: No registered STA client on data pkt RX"));
  VOS_ASSERT(0);
  return VOS_STATUS_E_FAILURE;
}/*                         */

/*                                                                          

                                    

             
                                                                     
                         

            

      
                                                                  
                                                                         
                               
                                                                       
                                              


              
                                                                    
                                                         

                                                                       

                                                                            */
VOS_STATUS
WLANTL_TxCompDefaultCb
(
 v_PVOID_t      pvosGCtx,
 vos_pkt_t*     vosDataBuff,
 VOS_STATUS     wTxSTAtus
)
{
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
         "WLAN TL:TXComp not registered, releasing pkt to prevent mem leak"));
  return vos_pkt_return_packet(vosDataBuff);
}/*                      */


/*                                                                          
                       
                                                                            */

/*                                                                          

                            

             
                           

              

            

      
                                              
                                                                              

              
                                                            

                                                   
                                                

              

                                                                            */
VOS_STATUS
WLANTL_CleanCB
(
  WLANTL_CbType*  pTLCb,
  v_U8_t      ucEmpty
)
{
  v_U8_t ucIndex;
  /*                                                                       */

  /*                                                                         
                
                                                                            */
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_CleanCB"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                    */
  pTLCb->usPendingTxCompleteCount = 0;

  /*                     */
   vos_atomic_set_U8( &pTLCb->ucTxSuspended, 1);

  /*               */
  pTLCb->uResCount = 0;


  /*                                                                         
                   
                                                                            */
  for ( ucIndex = 0; ucIndex < WLAN_MAX_STA_COUNT ; ucIndex++)
  {
    if(NULL != pTLCb->atlSTAClients[ucIndex])
    {
        WLANTL_CleanSTA( pTLCb->atlSTAClients[ucIndex], ucEmpty);
    }
  }

  /*                                                                         
                           
                                                                            */
  pTLCb->tlMgmtFrmClient.ucExists = 0;

  if ( ( 0 != ucEmpty) &&
       ( NULL != pTLCb->tlMgmtFrmClient.vosPendingDataBuff ))
  {
    vos_pkt_return_packet(pTLCb->tlMgmtFrmClient.vosPendingDataBuff);
  }

  pTLCb->tlMgmtFrmClient.vosPendingDataBuff  = NULL;

  /*                                                                    */
  pTLCb->tlMgmtFrmClient.pfnTlMgmtFrmRx = WLANTL_MgmtFrmRxDefaultCb;

  /*                                                                         
                 
                                                                            */
  pTLCb->tlBAPClient.ucExists = 0;

  if (( 0 != ucEmpty) &&
      ( NULL != pTLCb->tlBAPClient.vosPendingDataBuff ))
  {
    vos_pkt_return_packet(pTLCb->tlBAPClient.vosPendingDataBuff);
  }
  
  if (( 0 != ucEmpty) &&
      ( NULL != pTLCb->vosDummyBuf ))
  {
    vos_pkt_return_packet(pTLCb->vosDummyBuf);
  }

  pTLCb->tlBAPClient.vosPendingDataBuff  = NULL;

  pTLCb->vosDummyBuf = NULL;
  pTLCb->vosTempBuf  = NULL;
  pTLCb->ucCachedSTAId = WLAN_MAX_STA_COUNT;

  /*                                                                    */
  pTLCb->tlBAPClient.pfnTlBAPRx = WLANTL_BAPRxDefaultCb;

  pTLCb->ucRegisteredStaId = WLAN_MAX_STA_COUNT;

  return VOS_STATUS_SUCCESS;

}/*               */

/*                                                                          

                             

             
                                   

              

            

      
                                                                       
                                                                   
                                                                      

              
                                                            

                                                  
                                               

              

                                                                            */
VOS_STATUS
WLANTL_CleanSTA
(
  WLANTL_STAClientType*  ptlSTAClient,
  v_U8_t             ucEmpty
)
{
  v_U8_t  ucIndex;
  /*                                                                       */

  /*                                                                         
                
                                                                            */
  if ( NULL == ptlSTAClient )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_CleanSTA"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                         
                                                                           */
  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL: Clearing STA Client ID: %d, Empty flag: %d",
             ptlSTAClient->wSTADesc.ucSTAId, ucEmpty ));

  ptlSTAClient->pfnSTARx          = WLANTL_STARxDefaultCb;
  ptlSTAClient->pfnSTATxComp      = WLANTL_TxCompDefaultCb;
  ptlSTAClient->pfnSTAFetchPkt    = WLANTL_STAFetchPktDefaultCb;

  ptlSTAClient->tlState           = WLANTL_STA_INIT;
  ptlSTAClient->tlPri             = WLANTL_STA_PRI_NORMAL;

  vos_zero_macaddr( &ptlSTAClient->wSTADesc.vSTAMACAddress );
  vos_zero_macaddr( &ptlSTAClient->wSTADesc.vBSSIDforIBSS );
  vos_zero_macaddr( &ptlSTAClient->wSTADesc.vSelfMACAddress );

  ptlSTAClient->wSTADesc.ucSTAId  = 0;
  ptlSTAClient->wSTADesc.wSTAType = WLAN_STA_MAX;

  ptlSTAClient->wSTADesc.ucQosEnabled     = 0;
  ptlSTAClient->wSTADesc.ucAddRmvLLC      = 0;
  ptlSTAClient->wSTADesc.ucSwFrameTXXlation = 0;
  ptlSTAClient->wSTADesc.ucSwFrameRXXlation = 0;
  ptlSTAClient->wSTADesc.ucProtectedFrame = 0;

  /*                                                                         
                                 
                                                                            */
  if ( ( 0 != ucEmpty ) &&
       ( NULL != ptlSTAClient->vosAMSDUChainRoot ))
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
               "WLAN TL:Non NULL vosAMSDUChainRoot (=%p) on WLANTL_CleanSTA,"
               "suspecting a memory corruption"));

  }

  ptlSTAClient->vosAMSDUChain     = NULL;
  ptlSTAClient->vosAMSDUChainRoot = NULL;

  vos_mem_zero( (v_PVOID_t)ptlSTAClient->aucMPDUHeader,
                 WLANTL_MPDU_HEADER_LEN);
  ptlSTAClient->ucMPDUHeaderLen    = 0;

  /*                                                                         
                                      
                                                                            */
  for ( ucIndex = 0; ucIndex < WLAN_MAX_TID ; ucIndex++)
  {
    if(0 == ptlSTAClient->atlBAReorderInfo[ucIndex].ucExists)
    {
      continue;
    }
    if(NULL != ptlSTAClient->atlBAReorderInfo[ucIndex].reorderBuffer)
    {
      ptlSTAClient->atlBAReorderInfo[ucIndex].reorderBuffer->isAvailable = VOS_TRUE;
      memset(&ptlSTAClient->atlBAReorderInfo[ucIndex].reorderBuffer->arrayBuffer[0], 0, WLANTL_MAX_WINSIZE * sizeof(v_PVOID_t));
    }
    vos_timer_destroy(&ptlSTAClient->atlBAReorderInfo[ucIndex].agingTimer);
    memset(&ptlSTAClient->atlBAReorderInfo[ucIndex], 0, sizeof(WLANTL_BAReorderType));
  }

  /*                                                                         
                                
                                                                             */
   ptlSTAClient->ucCurrentAC     = WLANTL_AC_VO;
   ptlSTAClient->ucCurrentWeight = 0;
   ptlSTAClient->ucServicedAC    = WLANTL_AC_BK;

   vos_mem_zero( ptlSTAClient->aucACMask, sizeof(ptlSTAClient->aucACMask));
   vos_mem_zero( &ptlSTAClient->wUAPSDInfo, sizeof(ptlSTAClient->wUAPSDInfo));


  /*                                                                    
              
                                                                        */
   vos_mem_zero( ptlSTAClient->auRxCount,
                 sizeof(ptlSTAClient->auRxCount[0])* WLAN_MAX_TID);
   vos_mem_zero( ptlSTAClient->auTxCount,
                 sizeof(ptlSTAClient->auTxCount[0])* WLAN_MAX_TID);
   ptlSTAClient->rssiAvg = 0;

   /*                                             */
   vos_atomic_set_U8( &ptlSTAClient->ucTxSuspended, 0);
   vos_atomic_set_U8( &ptlSTAClient->ucNoMoreData, 1);

  if ( 0 == ucEmpty )
  {
    ptlSTAClient->wSTADesc.ucUcastSig       = WLAN_TL_INVALID_U_SIG;
    ptlSTAClient->wSTADesc.ucBcastSig       = WLAN_TL_INVALID_B_SIG;
  }

  ptlSTAClient->ucExists       = 0;

  /*                                                                    
                    
                                                                        */
  memset(&ptlSTAClient->trafficStatistics,
         0,
         sizeof(WLANTL_TRANSFER_STA_TYPE));

  /*                                                    */
  return VOS_STATUS_SUCCESS;
}/*                 */


/*                                                                          
                                     

             
                                                                        
                                                                         
                                                          
                                    

              
                                                                  

            

      
                                                                       
                                                                   
                              
                                                        
                                                 
                                                                 
                                                                      
                                                                    
                                                                              
                                      

              
                                                            

                                               

              

                                                                            */
VOS_STATUS
WLANTL_EnableUAPSDForAC
(
  v_PVOID_t          pvosGCtx,
  v_U8_t             ucSTAId,
  WLANTL_ACEnumType  ucAC,
  v_U8_t             ucTid,
  v_U8_t             ucUP,
  v_U32_t            uServiceInt,
  v_U32_t            uSuspendInt,
  WLANTL_TSDirType   wTSDir
)
{

  WLANTL_CbType*      pTLCb      = NULL;
  VOS_STATUS          vosStatus   = VOS_STATUS_SUCCESS;
  tUapsdInfo          halUAPSDInfo; 
 /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if (( NULL == pTLCb ) || WLANTL_STA_ID_INVALID( ucSTAId )
      ||   WLANTL_AC_INVALID(ucAC))
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid input params on WLANTL_EnableUAPSDForAC"
               " TL: %p  STA: %d  AC: %d",
               pTLCb, ucSTAId, ucAC));
    return VOS_STATUS_E_FAULT;
  }

  if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  /*                                                                    */
  pTLCb->atlSTAClients[ucSTAId]->wUAPSDInfo[ucAC].ucSet = 1;

#ifdef FEATURE_WLAN_TDLS
  if(pTLCb->atlSTAClients[ucSTAId]->wSTADesc.wSTAType != WLAN_STA_TDLS)
#endif
  {
    if( 0 == uServiceInt )
    {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Input params on WLANTL_EnableUAPSDForAC"
               " SI: %d", uServiceInt ));
    }

    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:Enabling U-APSD in FW for STA: %d AC: %d SI: %d SPI: %d "
               "DI: %d",
               ucSTAId, ucAC, uServiceInt, uSuspendInt,
               pTLCb->tlConfigInfo.uDelayedTriggerFrmInt));

    /*                     */
    halUAPSDInfo.staidx         = ucSTAId;
    halUAPSDInfo.ac             = ucAC;
    halUAPSDInfo.up             = ucUP;
    halUAPSDInfo.srvInterval    = uServiceInt;
    halUAPSDInfo.susInterval    = uSuspendInt;
    halUAPSDInfo.delayInterval  = pTLCb->tlConfigInfo.uDelayedTriggerFrmInt;

    /*          */
    vosStatus = WDA_EnableUapsdAcParams(pvosGCtx, ucSTAId, &halUAPSDInfo);
  }
  return vosStatus;

}/*                       */


/*                                                                          
                                      

             
                                                                          
                                                                        
                                                          
                                    

              
                                                                  

            

      
                                                                       
                                                                   
                              
                                                      


              
                                                            

                                               

              

                                                                            */
VOS_STATUS
WLANTL_DisableUAPSDForAC
(
  v_PVOID_t          pvosGCtx,
  v_U8_t             ucSTAId,
  WLANTL_ACEnumType  ucAC
)
{
  WLANTL_CbType* pTLCb;
  /*                                                                       */

  /*                                                                        
                
                            
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if (( NULL == pTLCb ) || WLANTL_STA_ID_INVALID( ucSTAId )
      ||   WLANTL_AC_INVALID(ucAC) )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid input params on WLANTL_DisableUAPSDForAC"
               " TL: %p  STA: %d  AC: %d", pTLCb, ucSTAId, ucAC ));
    return VOS_STATUS_E_FAULT;
  }

  if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  /*                                                         */
  pTLCb->atlSTAClients[ucSTAId]->wUAPSDInfo[ucAC].ucSet = 1;

  TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
             "WLAN TL:Disabling U-APSD in FW for STA: %d AC: %d ",
             ucSTAId, ucAC));

  /*          */
  WDA_DisableUapsdAcParams(pvosGCtx, ucSTAId, ucAC);

  return VOS_STATUS_SUCCESS;
}/*                          */

#if defined WLAN_FEATURE_NEIGHBOR_ROAMING
/*                                                                          
                                         

                                                                      
                         
                                                                            
                                        

                   
    
                                          
                                                  
                                                                   
                                                    
                                                            
                                        

                         

                   
  
                                                                            */
VOS_STATUS WLANTL_RegRSSIIndicationCB
(
   v_PVOID_t                       pAdapter,
   v_S7_t                          rssiValue,
   v_U8_t                          triggerEvent,
   WLANTL_RSSICrossThresholdCBType crossCBFunction,
   VOS_MODULE_ID                   moduleID,
   v_PVOID_t                       usrCtxt
)
{
   VOS_STATUS                     status = VOS_STATUS_SUCCESS;

   status = WLANTL_HSRegRSSIIndicationCB(pAdapter,
                                         rssiValue,
                                         triggerEvent,
                                         crossCBFunction,
                                         moduleID,
                                         usrCtxt);

   return status;
}

/*                                                                          
                                           

                                                  

                   
    
                                          
                                                  
                                                                   
                                                    
   
                         

                   
  
                                                                            */
VOS_STATUS WLANTL_DeregRSSIIndicationCB
(
   v_PVOID_t                       pAdapter,
   v_S7_t                          rssiValue,
   v_U8_t                          triggerEvent,
   WLANTL_RSSICrossThresholdCBType crossCBFunction,
   VOS_MODULE_ID                   moduleID
)
{
   VOS_STATUS                     status = VOS_STATUS_SUCCESS;

   status = WLANTL_HSDeregRSSIIndicationCB(pAdapter,
                                           rssiValue,
                                           triggerEvent,
                                           crossCBFunction,
                                           moduleID);
   return status;
}

/*                                                                          
                              

                                                          
                                                                         
                                                                   
                                              
                                                        
                           

                   
    
                                          
                                    
   
                         

                   
  
                                                                            */
VOS_STATUS WLANTL_SetAlpha
(
   v_PVOID_t pAdapter,
   v_U8_t    valueAlpha
)
{
   VOS_STATUS                     status = VOS_STATUS_SUCCESS;

   status = WLANTL_HSSetAlpha(pAdapter, valueAlpha);
   return status;
}

/*                                                                          

           

               
    
              

               

                                                                            */
VOS_STATUS WLANTL_BMPSRSSIRegionChangedNotification
(
   v_PVOID_t             pAdapter,
   tpSirRSSINotification pRSSINotification
)
{
   VOS_STATUS                     status = VOS_STATUS_SUCCESS;

   status = WLANTL_HSBMPSRSSIRegionChangedNotification(pAdapter, pRSSINotification);
   return status;
}

/*                                                                          
                                         

                                                                  
                                                       
                                                                             
                      
                                                                       
                                           

                   
    
                                          
                                                             
                                                            
                                                                      
                                              
                                        
   
                         

                   
  
                                                                            */
VOS_STATUS WLANTL_RegGetTrafficStatus
(
   v_PVOID_t                          pAdapter,
   v_U32_t                            idleThreshold,
   v_U32_t                            measurePeriod,
   WLANTL_TrafficStatusChangedCBType  trfficStatusCB,
   v_PVOID_t                          usrCtxt
)
{
   VOS_STATUS                     status = VOS_STATUS_SUCCESS;

   status = WLANTL_HSRegGetTrafficStatus(pAdapter,
                                idleThreshold,
                                measurePeriod,
                                trfficStatusCB,
                                usrCtxt);
   return status;
}
#endif
/*                                                                          
                                    

                                                              

                    
    
                                           
                                                                
                                                          
   
                          

                    
  
                                                                            */
VOS_STATUS WLANTL_GetStatistics
(
   v_PVOID_t                  pAdapter,
   WLANTL_TRANSFER_STA_TYPE  *statBuffer,
   v_U8_t                     STAid
)
{
  WLANTL_CbType            *pTLCb  = VOS_GET_TL_CB(pAdapter);
  WLANTL_STAClientType*     pClientSTA = NULL;
  VOS_STATUS                status = VOS_STATUS_SUCCESS;
  WLANTL_TRANSFER_STA_TYPE *statistics = NULL;

  /*                                                                        
                
                             
                                                                           */
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Invalid TL pointer on WLANTL_GetStatistics"));
    return VOS_STATUS_E_FAULT;
  }

  pClientSTA = pTLCb->atlSTAClients[STAid];

  if ( NULL == pClientSTA )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  if(0 == pClientSTA->ucExists)
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL: %d STA ID does not exist", STAid));
    return VOS_STATUS_E_INVAL;
  }

  if(NULL == statBuffer)
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL:Invalid TL statistics buffer pointer on WLANTL_GetStatistics"));
    return VOS_STATUS_E_INVAL;
  }

  statistics = &pClientSTA->trafficStatistics;
  memcpy(statBuffer, statistics, sizeof(WLANTL_TRANSFER_STA_TYPE));

  return status;
}

/*                                                                          
                                      

                                                                    
                                           

                    
    
                                           
                                                                
   
                          

                    
  
                                                                            */
VOS_STATUS WLANTL_ResetStatistics
(
   v_PVOID_t                  pAdapter,
   v_U8_t                     STAid
)
{
  WLANTL_CbType            *pTLCb  = VOS_GET_TL_CB(pAdapter);
  WLANTL_STAClientType*     pClientSTA = NULL;
  VOS_STATUS                status = VOS_STATUS_SUCCESS;
  WLANTL_TRANSFER_STA_TYPE *statistics = NULL;

  /*                                                                        
                
                             
                                                                           */
  if (NULL == pTLCb) 
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Invalid TL pointer on WLANTL_GetStatistics"));
    return VOS_STATUS_E_FAULT;
  }

  pClientSTA = pTLCb->atlSTAClients[STAid];

  if ( NULL == pClientSTA )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  if(0 == pClientSTA->ucExists)
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL: %d STA ID does not exist", STAid));
    return VOS_STATUS_E_INVAL;
  }

  statistics = &pClientSTA->trafficStatistics;
  vos_mem_zero((v_VOID_t *)statistics, sizeof(WLANTL_TRANSFER_STA_TYPE));

  return status;
}

/*                                                                          
                                       

                                                                  
                                      

                    

                                           
                                                                
                                        
                                             
   
                          

                    
  
                                                                            */
VOS_STATUS WLANTL_GetSpecStatistic
(
   v_PVOID_t                    pAdapter,
   WLANTL_TRANSFER_STATIC_TYPE  statType,
   v_U32_t                     *buffer,
   v_U8_t                       STAid
)
{
  WLANTL_CbType            *pTLCb  = VOS_GET_TL_CB(pAdapter);
  WLANTL_STAClientType*     pClientSTA = NULL;
  VOS_STATUS                status = VOS_STATUS_SUCCESS;
  WLANTL_TRANSFER_STA_TYPE *statistics = NULL;

  /*                                                                        
                
                             
                                                                           */
  if (NULL == pTLCb) 
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Invalid TL pointer on WLANTL_GetStatistics"));
    return VOS_STATUS_E_FAULT;
  }
  pClientSTA = pTLCb->atlSTAClients[STAid];

  if ( NULL == pClientSTA )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  if(0 == pClientSTA->ucExists)
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL: %d STA ID does not exist", STAid));
    return VOS_STATUS_E_INVAL;
  }

  if(NULL == buffer)
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL:Invalid TL statistic buffer pointer on WLANTL_GetStatistics"));
    return VOS_STATUS_E_INVAL;
  }

  statistics = &pClientSTA->trafficStatistics;
  switch(statType)
  {
    case WLANTL_STATIC_TX_UC_FCNT:
      *buffer = statistics->txUCFcnt;
      break;

    case WLANTL_STATIC_TX_MC_FCNT:
      *buffer = statistics->txMCFcnt;
      break;

    case WLANTL_STATIC_TX_BC_FCNT:
      *buffer = statistics->txBCFcnt;
      break;

    case WLANTL_STATIC_TX_UC_BCNT:
      *buffer = statistics->txUCBcnt;
      break;

    case WLANTL_STATIC_TX_MC_BCNT:
      *buffer = statistics->txMCBcnt;
      break;

    case WLANTL_STATIC_TX_BC_BCNT:
      *buffer = statistics->txBCBcnt;
      break;

    case WLANTL_STATIC_RX_UC_FCNT:
      *buffer = statistics->rxUCFcnt;
      break;

    case WLANTL_STATIC_RX_MC_FCNT:
      *buffer = statistics->rxMCFcnt;
      break;

    case WLANTL_STATIC_RX_BC_FCNT:
      *buffer = statistics->rxBCFcnt;
      break;

    case WLANTL_STATIC_RX_UC_BCNT:
      *buffer = statistics->rxUCBcnt;
      break;

    case WLANTL_STATIC_RX_MC_BCNT:
      *buffer = statistics->rxMCBcnt;
      break;

    case WLANTL_STATIC_RX_BC_BCNT:
      *buffer = statistics->rxBCBcnt;
      break;

    case WLANTL_STATIC_RX_BCNT:
      *buffer = statistics->rxBcnt;
      break;

    case WLANTL_STATIC_RX_BCNT_CRC_OK:
      *buffer = statistics->rxBcntCRCok;
      break;

    case WLANTL_STATIC_RX_RATE:
      *buffer = statistics->rxRate;
      break;

    default:
      *buffer = 0;
      status = VOS_STATUS_E_INVAL;
      break;
  }


  return status;
}

/*                                                                          
                                         

                                                                    
                                     
                                    

                    
    
                                           
                                                                
                                        

                          

                    
  
                                                                            */
VOS_STATUS WLANTL_ResetSpecStatistic
(
   v_PVOID_t                    pAdapter,
   WLANTL_TRANSFER_STATIC_TYPE  statType,
   v_U8_t                       STAid
)
{
  WLANTL_CbType            *pTLCb  = VOS_GET_TL_CB(pAdapter);
  WLANTL_STAClientType*     pClientSTA = NULL;
  VOS_STATUS                status = VOS_STATUS_SUCCESS;
  WLANTL_TRANSFER_STA_TYPE *statistics = NULL;

  /*                                                                        
                
                             
                                                                           */
  if (NULL == pTLCb) 
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "WLAN TL:Invalid TL pointer on WLANTL_GetStatistics"));
    return VOS_STATUS_E_FAULT;
  }

  pClientSTA = pTLCb->atlSTAClients[STAid];

  if ( NULL == pClientSTA )
  {
      TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
          "WLAN TL:Client Memory was not allocated on %s", __func__));
      return VOS_STATUS_E_FAILURE;
  }

  if(0 == pClientSTA->ucExists)
  {
    TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
    "WLAN TL: %d STA ID does not exist", STAid));
    return VOS_STATUS_E_INVAL;
  }

  statistics = &pClientSTA->trafficStatistics;
  switch(statType)
  {
    case WLANTL_STATIC_TX_UC_FCNT:
      statistics->txUCFcnt = 0;
      break;

    case WLANTL_STATIC_TX_MC_FCNT:
      statistics->txMCFcnt = 0;
      break;

    case WLANTL_STATIC_TX_BC_FCNT:
      statistics->txBCFcnt = 0;
      break;

    case WLANTL_STATIC_TX_UC_BCNT:
      statistics->txUCBcnt = 0;
      break;

    case WLANTL_STATIC_TX_MC_BCNT:
      statistics->txMCBcnt = 0;
      break;

    case WLANTL_STATIC_TX_BC_BCNT:
      statistics->txBCBcnt = 0;
      break;

    case WLANTL_STATIC_RX_UC_FCNT:
      statistics->rxUCFcnt = 0;
      break;

    case WLANTL_STATIC_RX_MC_FCNT:
      statistics->rxMCFcnt = 0;
      break;

    case WLANTL_STATIC_RX_BC_FCNT:
      statistics->rxBCFcnt = 0;
      break;

    case WLANTL_STATIC_RX_UC_BCNT:
      statistics->rxUCBcnt = 0;
      break;

    case WLANTL_STATIC_RX_MC_BCNT:
      statistics->rxMCBcnt = 0;
      break;

    case WLANTL_STATIC_RX_BC_BCNT:
      statistics->rxBCBcnt = 0;
      break;

    case WLANTL_STATIC_RX_BCNT:
      statistics->rxBcnt = 0;
      break;

    case WLANTL_STATIC_RX_BCNT_CRC_OK:
      statistics->rxBcntCRCok = 0;
      break;

    case WLANTL_STATIC_RX_RATE:
      statistics->rxRate = 0;
      break;

    default:
      status = VOS_STATUS_E_INVAL;
      break;
  }

  return status;
}


/*                                                                          

           

                                               
    
                                                    

               

                                                                            */
VOS_STATUS WLANTL_ReadRSSI
(
   v_PVOID_t        pAdapter,
   v_PVOID_t        pBDHeader,
   v_U8_t           STAid
)
{
   WLANTL_CbType   *tlCtxt = VOS_GET_TL_CB(pAdapter);
   v_S7_t           currentRSSI, currentRSSI0, currentRSSI1;


   if(NULL == tlCtxt)
   {
      TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR, "%s Invalid TL handle", __func__));
      return VOS_STATUS_E_INVAL;
   }

   if ( NULL == tlCtxt->atlSTAClients[STAid] )
   {
       TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
           "WLAN TL:Client Memory was not allocated on %s", __func__));
       return VOS_STATUS_E_FAILURE;
   }

   currentRSSI0 = WLANTL_GETRSSI0(pBDHeader);
   currentRSSI1 = WLANTL_GETRSSI1(pBDHeader);
   currentRSSI  = (currentRSSI0 > currentRSSI1) ? currentRSSI0 : currentRSSI1;

   tlCtxt->atlSTAClients[STAid]->rssiAvg = currentRSSI;

   return VOS_STATUS_SUCCESS;
}

/*                                                                          

           

                                              

                                                   

               

                                                                            */
VOS_STATUS WLANTL_ReadSNR
(
   v_PVOID_t        pAdapter,
   v_PVOID_t        pBDHeader,
   v_U8_t           STAid
)
{
   WLANTL_CbType   *tlCtxt = VOS_GET_TL_CB(pAdapter);
   v_S7_t           currentSNR;


   if (NULL == tlCtxt)
   {
      TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                       "%s Invalid TL handle", __func__));
      return VOS_STATUS_E_INVAL;
   }

   if (NULL == tlCtxt->atlSTAClients[STAid])
   {
       TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
           "WLAN TL:Client Memory was not allocated on %s", __func__));
       return VOS_STATUS_E_FAILURE;
   }

   currentSNR = WLANTL_GETSNR(pBDHeader);

   TLLOG2(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
          "%s: snrsum: %d snridx: %d prevsnravg: %d",
           __func__,
           tlCtxt->atlSTAClients[STAid]->snrSum,
           tlCtxt->atlSTAClients[STAid]->snrIdx,
           tlCtxt->atlSTAClients[STAid]->prevSnrAvg));

   /*                                                          
                                                   
                                                 
                                                    
                                                                   
                                                             
                             
     
                          
    */
   if (tlCtxt->atlSTAClients[STAid]->snrIdx >= WLANTL_MAX_SNR_DATA_SAMPLES)
   {
       tlCtxt->atlSTAClients[STAid]->prevSnrAvg =
               tlCtxt->atlSTAClients[STAid]->snrSum /
               tlCtxt->atlSTAClients[STAid]->snrIdx;
       tlCtxt->atlSTAClients[STAid]->snrSum = 0;
       tlCtxt->atlSTAClients[STAid]->snrIdx = 0;
   }
   tlCtxt->atlSTAClients[STAid]->snrSum += currentSNR;
   tlCtxt->atlSTAClients[STAid]->snrIdx += 1;

   return VOS_STATUS_SUCCESS;
}

/*
             
                                                     
   
                                                                        
                                                                             

    
                                                                  

                         
*/
VOS_STATUS  
WLANTL_GetACWeights 
( 
  v_PVOID_t             pvosGCtx,
  v_U8_t*               pACWeights
)
{
   WLANTL_CbType*  pTLCb = NULL;
   v_U8_t          ucIndex; 
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == pACWeights )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_GetACWeights"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                                                
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_GetACWeights"));
    return VOS_STATUS_E_FAULT;
  }
  for ( ucIndex = 0; ucIndex < WLANTL_MAX_AC ; ucIndex++)
  {
    pACWeights[ucIndex] = pTLCb->tlConfigInfo.ucAcWeights[ucIndex];
  }

  return VOS_STATUS_SUCCESS;
}



/*
             
                                                 
   
                                                                        
                                                                             
                                                                      


                         
*/
VOS_STATUS  
WLANTL_SetACWeights 
( 
  v_PVOID_t             pvosGCtx,
  v_U8_t*               pACWeights
)
{
   WLANTL_CbType*  pTLCb = NULL;
   v_U8_t          ucIndex; 
  /*                                                                       */

  /*                                                                        
                
                                                                           */
  if ( NULL == pACWeights )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Invalid parameter sent on WLANTL_GetACWeights"));
    return VOS_STATUS_E_INVAL;
  }

  /*                                                                        
                                                
                                                                           */
  pTLCb = VOS_GET_TL_CB(pvosGCtx);
  if ( NULL == pTLCb )
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
              "WLAN TL:Invalid TL pointer from pvosGCtx on WLANTL_GetACWeights"));
    return VOS_STATUS_E_FAULT;
  }
  for ( ucIndex = 0; ucIndex < WLANTL_MAX_AC ; ucIndex++)
  {
    pTLCb->tlConfigInfo.ucAcWeights[ucIndex] = pACWeights[ucIndex];
  }

  return VOS_STATUS_SUCCESS;
}


/*                                                                          

           

               
    
              

               

                                                                            */
void WLANTL_PowerStateChangedCB
(
   v_PVOID_t pAdapter,
   tPmcState newState
)
{
   WLANTL_CbType                *tlCtxt = VOS_GET_TL_CB(pAdapter);

   if (NULL == tlCtxt)
   {
     VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "%s: Invalid TL Control Block", __func__ );
     return;
   }

   VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO, "Power state changed, new state is %d", newState );
   switch(newState)
   {
      case FULL_POWER:
         tlCtxt->isBMPS = VOS_FALSE;
         break;

      case BMPS:
#if defined WLAN_FEATURE_NEIGHBOR_ROAMING
         WLANTL_SetFWRSSIThresholds(pAdapter);
#endif

         tlCtxt->isBMPS = VOS_TRUE;
         break;

      case IMPS:
      case LOW_POWER:
      case REQUEST_BMPS:
      case REQUEST_FULL_POWER:
      case REQUEST_IMPS:
      case STOPPED:
      case REQUEST_START_UAPSD:
      case REQUEST_STOP_UAPSD:
      case UAPSD:
      case REQUEST_STANDBY:
      case STANDBY:
      case REQUEST_ENTER_WOWL:
      case REQUEST_EXIT_WOWL:
      case WOWL:
         TLLOGW(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN, "Not handle this events %d", newState ));
         break;

      default:
         TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR, "Not a valid event %d", newState ));
         break;
   }

   return;
}
/*                                                                          
                                   

                                                          

                    
    
                                          
                                            
                                                   
                                                       

                                                                               
                                                                           
                                                           

                    
  
                                                                            */
static VOS_STATUS WLANTL_GetEtherType
(
   v_U8_t               * aucBDHeader,
   vos_pkt_t            * vosDataBuff,
   v_U8_t                 ucMPDUHLen,
   v_U16_t              * pUsEtherType
)
{
  v_U8_t                   ucOffset;
  v_U16_t                  usEtherType = *pUsEtherType;
  v_SIZE_t                 usLLCSize = sizeof(usEtherType);
  VOS_STATUS               vosStatus  = VOS_STATUS_SUCCESS;
  
  /*                                                                        
                                                                    
                                                                           */
  if ( VOS_FALSE == WDA_IS_RX_LLC_PRESENT( aucBDHeader ) )
  {
    ucOffset = WLANTL_802_3_HEADER_LEN - sizeof(usEtherType); 
  }
  else
  {
    ucOffset = ucMPDUHLen + WLANTL_LLC_PROTO_TYPE_OFFSET;
  }

  /*                                                                        
                     
                                                                          */
  vosStatus = vos_pkt_extract_data( vosDataBuff, ucOffset, 
                                    (v_PVOID_t)&usEtherType, &usLLCSize); 

  if (( VOS_STATUS_SUCCESS != vosStatus ) || 
      ( sizeof(usEtherType) != usLLCSize ))
      
  {
    TLLOGE(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
               "WLAN TL:Error extracting Ether type from data packet"));
    /*             */
    vos_pkt_return_packet(vosDataBuff);
    vosStatus = VOS_STATUS_E_FAILURE;
  }
  else
  {
    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:Ether type retrieved before endianess conv: %d", 
               usEtherType));

    usEtherType = vos_be16_to_cpu(usEtherType);
    *pUsEtherType = usEtherType;

    TLLOG2(VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:Ether type retrieved: %d", usEtherType));
  }
  
  return vosStatus;
}

/*                                                                          
                                          

                                                                         

                    
    
                                                                
                                                                                 
                                                              

                                                                                 

                    

                                                                            */
VOS_STATUS WLANTL_GetSoftAPStatistics(v_PVOID_t pAdapter, WLANTL_TRANSFER_STA_TYPE *statsSum, v_BOOL_t bReset)
{
    v_U8_t i = 0;
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    WLANTL_CbType *pTLCb  = VOS_GET_TL_CB(pAdapter);
    WLANTL_TRANSFER_STA_TYPE statBufferTemp;
    vos_mem_zero((v_VOID_t *)&statBufferTemp, sizeof(WLANTL_TRANSFER_STA_TYPE));
    vos_mem_zero((v_VOID_t *)statsSum, sizeof(WLANTL_TRANSFER_STA_TYPE));


    if ( NULL == pTLCb )
    {
       return VOS_STATUS_E_FAULT;
    } 

    //                                                          
    for (i = 0; i < WLAN_MAX_STA_COUNT; i++)
    {
        if ( NULL == pTLCb->atlSTAClients[i])
        {
            continue;
        }
        if (pTLCb->atlSTAClients[i]->wSTADesc.wSTAType == WLAN_STA_SOFTAP)
        {
           vosStatus = WLANTL_GetStatistics(pAdapter, &statBufferTemp, i);//                                                            

           if (!VOS_IS_STATUS_SUCCESS(vosStatus))
                return VOS_STATUS_E_FAULT;

            //                    
           statsSum->txUCFcnt += statBufferTemp.txUCFcnt;
           statsSum->txMCFcnt += statBufferTemp.txMCFcnt;
           statsSum->txBCFcnt += statBufferTemp.txBCFcnt;
           statsSum->txUCBcnt += statBufferTemp.txUCBcnt;
           statsSum->txMCBcnt += statBufferTemp.txMCBcnt;
           statsSum->txBCBcnt += statBufferTemp.txBCBcnt;
           statsSum->rxUCFcnt += statBufferTemp.rxUCFcnt;
           statsSum->rxMCFcnt += statBufferTemp.rxMCFcnt;
           statsSum->rxBCFcnt += statBufferTemp.rxBCFcnt;
           statsSum->rxUCBcnt += statBufferTemp.rxUCBcnt;
           statsSum->rxMCBcnt += statBufferTemp.rxMCBcnt;
           statsSum->rxBCBcnt += statBufferTemp.rxBCBcnt;

           if (bReset)
           {
              vosStatus = WLANTL_ResetStatistics(pAdapter, i);
              if (!VOS_IS_STATUS_SUCCESS(vosStatus))
                return VOS_STATUS_E_FAULT;               
          }
        }
    }

    return vosStatus;
}
#ifdef FEATURE_WLAN_TDLS_INTERNAL
/*                                                                          
                                     

                                                          

                    
    
                                          
                                            
                                                   
                                                       

                                                                               
                                                                           
                                                           

                    
  
                                                                            */
static VOS_STATUS WLANTL_GetEtherType_2
(
   v_U8_t               * aucBDHeader,
   vos_pkt_t            * vosDataBuff,
   v_U8_t                 ucMPDUHLen,
   v_U16_t              * pUsEtherType
)
{
  v_U8_t                   ucOffset;
  v_U16_t                  usEtherType = *pUsEtherType;
  v_SIZE_t                 usLLCSize = sizeof(usEtherType);
  VOS_STATUS               vosStatus  = VOS_STATUS_SUCCESS;
  //                                     
  v_U8_t                   ucMPDUHOffset ; 
  /*                                                                        
                                                                    
                                                                           */
  //                                                                      
  //                                                           
  ucMPDUHOffset = (v_U8_t)WDA_GET_RX_MPDU_HEADER_OFFSET(aucBDHeader);

  if ( VOS_TRUE == WDA_IS_RX_LLC_PRESENT(aucBDHeader) )
  {
    ucOffset = ucMPDUHOffset + WLANTL_802_3_HEADER_LEN - sizeof(usEtherType); 
  }
  else
  {
    ucOffset = WLANHAL_RX_BD_HEADER_SIZE + ucMPDUHLen 
                                            + WLANTL_LLC_PROTO_TYPE_OFFSET;
  }

  /*                                                                        
                     
                                                                          */
  vosStatus = vos_pkt_extract_data( vosDataBuff, ucOffset, 
                                    (v_PVOID_t)&usEtherType, &usLLCSize); 

  /*                           */
  if(vos_be16_to_cpu(usEtherType) == 0x890d)
  {
     VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR, 
                      ("TDLS frame llc %x \n"), vos_be16_to_cpu(usEtherType)) ;
  }
      
    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:Ether type retrieved before endianess conv: %d", 
               usEtherType);

    usEtherType = vos_be16_to_cpu(usEtherType);
    *pUsEtherType = usEtherType;

    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
               "WLAN TL:Ether type retrieved: %d", usEtherType);
  
  return vosStatus;
}
#endif /*                   */

/*                                                                               
                                     
     
                                                                  
 
                                                                          
                         
                          
                                                                                
                                                                                
                                       
                                                     
                                                         

                     
                                                                                */
v_BOOL_t
WLANTL_IsReplayPacket
(
  v_U64_t    ullcurrentReplayCounter,
  v_U64_t    ullpreviousReplayCounter
)
{
   /*                                                                       
                                     */
    if(ullpreviousReplayCounter < ullcurrentReplayCounter)
    {
        /*                         */
        return VOS_FALSE;
    }
    else
    {

        /*                                                                  
                                                                 */
        VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
        "WLAN TL: Replay packet found with replay counter :[0x%llX]",ullcurrentReplayCounter);
           
        return VOS_TRUE;
    }
}

#if 0
/*                                                                               
                                               
     
                                                                              
 
                                                                          
                         
                          
                                                   
                                       
                                                             

                     
                                                                                */
v_U64_t
WLANTL_GetReplayCounterFromRxBD
(
   v_U8_t *pucRxBDHeader
)
{
/*                                            
                                 
                  
                  
                  
                  
                  

                  
                   */

#ifdef ANI_BIG_BYTE_ENDIAN
    v_U64_t ullcurrentReplayCounter = 0;
    /*                                              */
    ullcurrentReplayCounter = WLANHAL_RX_BD_GET_PMICMD_20TO23(pucRxBDHeader); 
    ullcurrentReplayCounter <<= 16;
    ullcurrentReplayCounter |= (( WLANHAL_RX_BD_GET_PMICMD_24TO25(pucRxBDHeader) & 0xFFFF0000) >> 16);
    return ullcurrentReplayCounter;
#else
    v_U64_t ullcurrentReplayCounter = 0;
    /*                                              */
    ullcurrentReplayCounter = (WLANHAL_RX_BD_GET_PMICMD_24TO25(pucRxBDHeader) & 0x0000FFFF); 
    ullcurrentReplayCounter <<= 32; 
    ullcurrentReplayCounter |= WLANHAL_RX_BD_GET_PMICMD_20TO23(pucRxBDHeader); 
    return ullcurrentReplayCounter;
#endif
}
#endif

/*                                                                               
                                    
     
                                                                          
 
                    
                          
                        
                                       
                    

                     
                                                                                */

void WLANTL_PostResNeeded(v_PVOID_t pvosGCtx)
{
  vos_msg_t            vosMsg;

  vosMsg.reserved = 0;
  vosMsg.bodyptr  = NULL;
  vosMsg.type     = WLANTL_TX_RES_NEEDED;
  VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_INFO_HIGH,
        "WLAN TL: BD/PDU available interrupt received, Posting message to TL");
  if(!VOS_IS_STATUS_SUCCESS(vos_tx_mq_serialize( VOS_MQ_ID_TL, &vosMsg)))
  {
    VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
       " %s fails to post message", __func__);
  }
}

/*                                                                               
                                      

                                                                   

                     

            

                                                            
                                                    
                                                           

                     

                     
                                                                                */

void WLANTL_UpdateRssiBmps(v_PVOID_t pvosGCtx, v_U8_t staId, v_S7_t rssi)
{
  WLANTL_CbType* pTLCb = VOS_GET_TL_CB(pvosGCtx);

  if (NULL != pTLCb && NULL != pTLCb->atlSTAClients[staId])
  {
    pTLCb->atlSTAClients[staId]->rssiAvgBmps = rssi;
  }
}

/*                                                                               
                                     

                                                                  

                     

            

                                                            
                                                    
                                                        

                     

                     
                                                                                */

void WLANTL_UpdateSnrBmps(v_PVOID_t pvosGCtx, v_U8_t staId, v_S7_t snr)
{
  WLANTL_CbType* pTLCb = VOS_GET_TL_CB(pvosGCtx);

  if (NULL != pTLCb && NULL != pTLCb->atlSTAClients[staId])
  {
    pTLCb->atlSTAClients[staId]->snrAvgBmps = snr;
  }
}

/*                                                                               
                                          

                                                                    

                     

            

                                                            
                                                    
                                                       

                     

                     
                                                                                */

void WLANTL_UpdateLinkCapacity(v_PVOID_t pvosGCtx, v_U8_t staId, v_U32_t linkCapacity)
{
    WLANTL_CbType* pTLCb = VOS_GET_TL_CB(pvosGCtx);

    if (NULL != pTLCb && NULL != pTLCb->atlSTAClients[staId])
    {
        pTLCb->atlSTAClients[staId]->linkCapacity = linkCapacity;
    }
}


/*                                                                           

                                       

             

                                              

              

                                                                          


            

      
                                                                       
                                                                   
                                             

       
                                                               
                                     


              

                                                            

                                                     
                                                                             
                                                                        
                                                   
                                              

              

                                                                            */
VOS_STATUS
WLANTL_GetSTALinkCapacity
(
    v_PVOID_t             pvosGCtx,
    v_U8_t                ucSTAId,
    v_U32_t               *plinkCapacity
)
{
    WLANTL_CbType*  pTLCb = NULL;
    /*                                                                       */

    /*                                                                        
                  
                                                                             */
    if ( NULL == plinkCapacity )
    {
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                         FL("WLAN TL:Invalid parameter")));
        return VOS_STATUS_E_INVAL;
    }

    if ( WLANTL_STA_ID_INVALID( ucSTAId ) )
    {
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                         FL("WLAN TL:Invalid station id")));
        return VOS_STATUS_E_FAULT;
    }

    /*                                                                        
                                                  
                                                                             */
    pTLCb = VOS_GET_TL_CB(pvosGCtx);
    if ( NULL == pTLCb )
    {
         TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                          FL("WLAN TL:Invalid TL pointer from pvosGCtx")));
         return VOS_STATUS_E_FAULT;
    }

    if ( NULL == pTLCb->atlSTAClients[ucSTAId] )
    {
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                         FL("WLAN TL:Client Memory was not allocated")));
        return VOS_STATUS_E_FAILURE;
    }

    if ( 0 == pTLCb->atlSTAClients[ucSTAId]->ucExists )
    {
        TLLOGE(VOS_TRACE(VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_WARN,
                         FL("WLAN TL:Station was not previously registered")));
        return VOS_STATUS_E_EXISTS;
    }

    /*                                                                        
                   
                                                                             */
    *plinkCapacity = pTLCb->atlSTAClients[ucSTAId]->linkCapacity;

    return VOS_STATUS_SUCCESS;
}/*                           */

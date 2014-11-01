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

                      b a p A p i D a t a . C
                                               
  OVERVIEW:
  
  This software unit holds the implementation of the WLAN BAP modules
  "platform independent" Data path functions.
  
  The functions externalized by this module are to be called ONLY by other 
  WLAN modules (HDD) that properly register with the BAP Layer initially.

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
//                        
#include "vos_trace.h"
//                           
#include "wlan_qct_tl.h"

#include "wlan_qct_hal.h"

/*                            */ 
#include "bapApi.h" 
#include "bapInternal.h" 
#include "bapApiTimer.h"

//                 
/*                                                                            
                                         
                                                                            */
/*                       */

#undef BAP_LITTLE_BIT_ENDIAN
#define BAP_LITTLE_BIT_ENDIAN

/*                      */

/*                         */
#define WLANBAP_LLC_HEADER_LEN   8 
#if 0
/*                                                  */
#define WLANBAP_LLC_OUI_OFFSET                 3

/*                                                     */
#define WLANBAP_LLC_OUI_SIZE                   3

/*                                                            */
#define WLANBAP_LLC_PROTO_TYPE_OFFSET  (WLANBAP_LLC_OUI_OFFSET +  WLANBAP_LLC_OUI_SIZE)

/*                                                          */
#define WLANBAP_LLC_PROTO_TYPE_SIZE            2
#endif

/*                           */
/*                          */
#define WLANBAP_BT_AMP_TYPE_DATA       0x0001

/*                                     */
#define WLANBAP_BT_AMP_TYPE_AR         0x0002

/*                                    */
#define WLANBAP_BT_AMP_TYPE_SEC        0x0003

/*                        */
#define  WLANBAP_802_3_HEADER_LEN             14

/*                                     */
#define  WLANBAP_802_3_HEADER_DA_OFFSET        0

//                              
const v_U8_t WLANBAP_BT_AMP_OUI[] =  {0x00, 0x19, 0x58 };

/*                */
static v_U8_t WLANBAP_LLC_HEADER[] =  {0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00 };

/*                       */

//                                                    
#define WLANBAP_HCI_ACL_HEADER_LEN    4

//                      
//                     
#define DUMPLOG_ON
#ifdef DUMPLOG_ON
#define DUMPLOG(n, name1, name2, aStr, size) do {                       \
        int i;                                                          \
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,"%d. %s: %s = \n", n, name1, name2); \
        for (i = 0; i < size; i++)                                      \
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,"%2.2x%s", ((unsigned char *)aStr)[i], i % 16 == 15 ? "\n" : " "); \
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,"\n");      \
    } while (0)
#else
#define DUMPLOG(n, name1, name2, aStr, size)
#endif

#if 0
//                      
#define DBGLOG printf
#define DUMPLOG
#if defined DUMPLOG
#define DUMPLOG(n, name1, name2, aStr, size) do {                       \
        int i;                                                          \
        DBGLOG("%d. %s: %s = \n", n, name1, name2);                     \
        for (i = 0; i < size; i++)                                      \
            DBGLOG("%2.2x%s", ((unsigned char *)aStr)[i], i % 16 == 15 ? "\n" : " "); \
        DBGLOG("\n");                                                   \
    } while (0)
#else
#define DUMPLOG(n, name1, name2, aStr, size)
#endif
#endif

/*                                                                            
                    
                                                                            */
//                                           
#if 0
/*              */
typedef struct 
{
 /*                           */
 v_U8_t   vDA[VOS_MAC_ADDR_SIZE];

 /*                      */
 v_U8_t   vSA[VOS_MAC_ADDR_SIZE];

 /*              */
 v_U16_t  usLenType;  /*                                                   */
                      /*                                                
                                                         */
}WLANBAP_8023HeaderType;
#endif

/* 
                                    
  
                                           
                                            
                                            
                                            
                                            
                                            
                                            
                                            
                                            
                                            
  
        
                            
                                          
                                          
                                 
                                  
                                                         
  
 */

typedef struct 
{

#ifndef BAP_LITTLE_BIT_ENDIAN

   v_U8_t phyLinkHandle; /*                                              */

   v_U8_t BCFlag :2;
   v_U8_t PBFlag :2;
   v_U8_t logLinkHandle :4;

   v_U16_t dataLength;  /*                                                                                */

#else

   v_U8_t phyLinkHandle;

   v_U8_t logLinkHandle :4;
   v_U8_t PBFlag :2;
   v_U8_t BCFlag :2;

   v_U16_t dataLength;  /*                                                  */

#endif

} WLANBAP_HCIACLHeaderType;



/*                                                                            
                          
                                                                            */

/*                                                                            
                              
                                                                            */

/*                                                                            
                                               
                                                                            */

/*                                                                            
                                    
                                                                           */

/*                                                                            
                                          
                                                                            */

#define WLANBAP_DEBUG_FRAME_BYTE_PER_LINE    16
#define WLANBAP_DEBUG_FRAME_BYTE_PER_BYTE    4

/*                                                                           

                                    

              

                                                                      
                                                                    


             

                                                                      
                                                                      

                                                       
                                                                    
                                                 
                                                                        
   
              

                                                              

                                                      
                                             
                                               

               
  
                                                                            */
VOS_STATUS
WLANBAP_XlateTxDataPkt
( 
    ptBtampHandle     btampHandle,  /*                                           
                                                              */ 
    v_U8_t            phy_link_handle,  /*                                                  */
    WLANTL_ACEnumType    *pucAC,        /*                    */
    WLANTL_MetaInfoType  *tlMetaInfo, /*                                                                  */
    vos_pkt_t        *vosDataBuff
)
{
    ptBtampContext           pBtampCtx = (ptBtampContext) btampHandle; 
    tpBtampLogLinkCtx        pLogLinkContext;
    WLANBAP_8023HeaderType   w8023Header;
    WLANBAP_HCIACLHeaderType hciACLHeader;
    v_U8_t                   aucLLCHeader[WLANBAP_LLC_HEADER_LEN];
    VOS_STATUS               vosStatus;
    v_U8_t                   ucSTAId;  /*                                     */
    v_PVOID_t                pHddHdl; /*                                 */
    v_U16_t                  headerLength;  /*                       */
    v_U16_t                  protoType = WLANBAP_BT_AMP_TYPE_DATA;  /*                        */
    v_U32_t                  value = 0;
    /*                                                                       */

 
    /*                                                                        
                           
                                                                              */
    if ( NULL == pBtampCtx) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "Invalid BAP handle value in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    //                                                     
    //                                             
    vosStatus = vos_pkt_pop_head( vosDataBuff, &hciACLHeader, WLANBAP_HCI_ACL_HEADER_LEN);

    if ( VOS_STATUS_SUCCESS != vosStatus ) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                  "WLAN BAP: Failed to pop HCI ACL header from packet %d",
                  vosStatus);

        return vosStatus;
    }

    //                                        
    //                                        

    if ( phy_link_handle != hciACLHeader.phyLinkHandle ) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "WLAN BAP: phy_link_handle mismatch in %s phy_link_handle=%d hciACLHeader.phyLinkHandle=%d",
                __func__, phy_link_handle, hciACLHeader.phyLinkHandle);
        return VOS_STATUS_E_INVAL;
    }


    /*                                                                */ 

    vosStatus = WLANBAP_GetStaIdFromLinkCtx ( 
            btampHandle,  /*                       */ 
            phy_link_handle,  /*                          */
            &ucSTAId,  /*                                     */
            &pHddHdl); /*                              */
    if ( VOS_STATUS_SUCCESS != vosStatus ) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
                     "Unable to retrieve STA Id from BAP context and phy_link_handle in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    //                                        
    //                                        
    if (!BTAMP_VALID_LOG_LINK( hciACLHeader.logLinkHandle))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
                "WLAN BAP: Invalid logical link handle (%d) in %s. Corrected.", 
                hciACLHeader.logLinkHandle,
                __func__);

        //                                                      
        hciACLHeader.logLinkHandle = 1;
        //                          
    }

    /*                                                              */ 
    /*                                                                         */ 
    pLogLinkContext = &(pBtampCtx->btampLogLinkCtx[ hciACLHeader.logLinkHandle ]);

    //                                        
    //                                        
    //                                                          
    if ( pLogLinkContext->present != VOS_TRUE)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
                "WLAN BAP: Invalid logical link entry in %s",
                __func__);

        return VOS_STATUS_E_INVAL;
    }

    //                           

    //                                                     
    *pucAC = pLogLinkContext->btampAC;
    //                                                                   
    tlMetaInfo->ucTID = pLogLinkContext->ucTID;
    tlMetaInfo->ucUP = pLogLinkContext->ucUP;
    tlMetaInfo->ucIsEapol = VOS_FALSE;
    tlMetaInfo->ucDisableFrmXtl = VOS_FALSE;
    tlMetaInfo->ucBcast = VOS_FALSE; /*                      */ /*                                 */
    tlMetaInfo->ucMcast = VOS_FALSE;
    tlMetaInfo->ucType = 0x00;  /*                        */
//                                                                                                         

    //                      

    vos_mem_copy( w8023Header.vDA, pBtampCtx->peer_mac_addr, VOS_MAC_ADDR_SIZE);
    vos_mem_copy( w8023Header.vSA, pBtampCtx->self_mac_addr, VOS_MAC_ADDR_SIZE);
    
    /*                                                          */
    headerLength = vos_le16_to_cpu(hciACLHeader.dataLength);  
    headerLength += WLANBAP_LLC_HEADER_LEN;  
    /*                                            */
    w8023Header.usLenType = vos_cpu_to_be16(headerLength);

    /*                                   */
    protoType = vos_cpu_to_be16( protoType);

    /*                         */
    vos_mem_copy(aucLLCHeader, 
            WLANBAP_LLC_HEADER,  
            sizeof(WLANBAP_LLC_HEADER));
    vos_mem_copy(&aucLLCHeader[WLANBAP_LLC_OUI_OFFSET], 
            WLANBAP_BT_AMP_OUI,  
            WLANBAP_LLC_OUI_SIZE);
    vos_mem_copy(&aucLLCHeader[WLANBAP_LLC_PROTO_TYPE_OFFSET], 
            &protoType,  //                        
            WLANBAP_LLC_PROTO_TYPE_SIZE);
 
    /*                        */
    vos_pkt_push_head(vosDataBuff, 
            aucLLCHeader, 
            WLANBAP_LLC_HEADER_LEN);  

    /*                          */
    vos_pkt_push_head(vosDataBuff, &w8023Header, sizeof(w8023Header));


    /*                                                                       
                  */
    value = (v_U32_t)hciACLHeader.logLinkHandle;
    vos_pkt_set_user_data_ptr( vosDataBuff, VOS_PKT_USER_DATA_ID_BAP,
                               (v_VOID_t *)value);

    return VOS_STATUS_SUCCESS;
}/*                      */

/*                                                                           

                                        

              

                                                                            
                                                                         
                          


             

                                                                      
 
                                           
 
                                                       
   
              

                                                              

                                                      
                                             
                                               

               
  
                                                                            */
VOS_STATUS
WLANBAP_GetAcFromTxDataPkt
( 
  ptBtampHandle     btampHandle,  /*                                           
                                                            */
  void              *pHciData,     /*                               */
  WLANTL_ACEnumType *pucAC        /*                    */
)
{
    ptBtampContext           pBtampCtx; 
    tpBtampLogLinkCtx        pLogLinkContext;
    WLANBAP_HCIACLHeaderType hciACLHeader;
    /*                                                                        
                           
                                                                              */
    if (( NULL == btampHandle) || (NULL == pHciData) || (NULL == pucAC))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "Invalid params in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }
    pBtampCtx = (ptBtampContext) btampHandle;

    vos_mem_copy( &hciACLHeader, pHciData, WLANBAP_HCI_ACL_HEADER_LEN);
    //                                        
    if (!BTAMP_VALID_LOG_LINK( hciACLHeader.logLinkHandle))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
                "WLAN BAP: Invalid logical link handle (%d) in %s", 
                hciACLHeader.logLinkHandle,
                __func__);

        return VOS_STATUS_E_INVAL;
    }

    /*                                                              */ 
    /*                                                                         */ 
    pLogLinkContext = &(pBtampCtx->btampLogLinkCtx[ hciACLHeader.logLinkHandle ]);

    //                                        
    //                                                          
    if ( pLogLinkContext->present != VOS_TRUE)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
                "WLAN BAP: Invalid logical link entry in %s",
                __func__);

        return VOS_STATUS_E_INVAL;
    }

    //              

    //                                                     
    *pucAC = pLogLinkContext->btampAC;

    return VOS_STATUS_SUCCESS;
}

/*                                                                           

                                    

              

                                                                    
                                                             
                                                                        


             

                                                                      
                                                       
                                                                  
                                                 
   
              

                                                              

                                                      
                                             
                                               

               
  
                                                                            */
VOS_STATUS
WLANBAP_XlateRxDataPkt
( 
  ptBtampHandle     btampHandle, 
  v_U8_t            phy_link_handle,  /*                                                  */
  WLANTL_ACEnumType  *pucAC,        /*                                                  */
  vos_pkt_t        *vosDataBuff
)
{
    WLANBAP_8023HeaderType  w8023Header;
    WLANBAP_HCIACLHeaderType hciACLHeader;
    v_U8_t                   aucLLCHeader[WLANBAP_LLC_HEADER_LEN];
    ptBtampContext           pBtampCtx = (ptBtampContext) btampHandle; 
    VOS_STATUS               vosStatus;
    //                                                                       
    v_U16_t                  hciDataLength;  /*                           */
    v_U16_t                  protoType = WLANBAP_BT_AMP_TYPE_DATA;  /*                        */
    /*                                                                       */

    /*                                                                        
                           
                                                                              */
    if ( NULL == pBtampCtx) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "Invalid BAP handle value in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    //                                                     
    //                                              
    vos_mem_set( &w8023Header, sizeof(w8023Header), 0 );
    vosStatus = vos_pkt_pop_head( vosDataBuff, &w8023Header, sizeof(w8023Header));

    if ( VOS_STATUS_SUCCESS != vosStatus ) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                  "WLAN BAP: Failed to pop 802.3 header from packet %d",
                  vosStatus);

        return vosStatus;
    }

    //                                
    //                                             
    vos_mem_set( aucLLCHeader, WLANBAP_LLC_HEADER_LEN, 0 );
    vosStatus = vos_pkt_pop_head( vosDataBuff, aucLLCHeader, WLANBAP_LLC_HEADER_LEN);

    if ( VOS_STATUS_SUCCESS != vosStatus ) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                  "WLAN BAP: Failed to pop LLC/SNAP header from packet %d",
                  vosStatus);

        return vosStatus;
    }

#ifdef BAP_DEBUG
    //                                        
    //                                                         
    if ( !(vos_mem_compare( aucLLCHeader, 
             WLANBAP_LLC_HEADER,  
             sizeof(WLANBAP_LLC_HEADER)  
             - WLANBAP_LLC_OUI_SIZE)  /*                                       */ 
         && vos_mem_compare( &aucLLCHeader[WLANBAP_LLC_OUI_OFFSET], 
             (v_VOID_t*)WLANBAP_BT_AMP_OUI,  
             WLANBAP_LLC_OUI_SIZE)))  /*                 */ 
    {

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "Invalid LLC header for BT-AMP packet in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }
#endif //         

    /*                                   */
    protoType = vos_cpu_to_be16( protoType);
    //                                                                 
    //                                            
    //                                                                 
    //                 
    if ( !(vos_mem_compare( &aucLLCHeader[WLANBAP_LLC_PROTO_TYPE_OFFSET], 
            &protoType,  //                        
            WLANBAP_LLC_PROTO_TYPE_SIZE))) 
    {

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "Invalid (non-data) frame type in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

#ifdef BAP_DEBUG
    //                                        
    /*                                                                        
                                                                  
                                                   
                                                                              */
    if ( !(vos_mem_compare( w8023Header.vDA, pBtampCtx->self_mac_addr, VOS_MAC_ADDR_SIZE)
    && vos_mem_compare( w8023Header.vSA, pBtampCtx->peer_mac_addr, VOS_MAC_ADDR_SIZE))) 
    {

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "MAC address mismatch in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }
#endif //         

    /*                                                                   
                                                                                
                                                          
     */ 


    //                                 
    hciACLHeader.phyLinkHandle = phy_link_handle;

    //                                    
    //                                                                               
    hciACLHeader.logLinkHandle = 0;
    hciACLHeader.PBFlag = WLANBAP_HCI_PKT_AMP;
    hciACLHeader.BCFlag = 0;

    /*                                      */
    hciDataLength =  vos_be16_to_cpu(w8023Header.usLenType);
    /*                                                  */
    hciDataLength -= WLANBAP_LLC_HEADER_LEN;
    /*                                             */
    hciACLHeader.dataLength = vos_cpu_to_le16(hciDataLength);  

    /*                     
                                                                  
     */
    *pucAC = 0;        

    /*                        */
    vos_pkt_push_head(vosDataBuff, &hciACLHeader, WLANBAP_HCI_ACL_HEADER_LEN);

    return VOS_STATUS_SUCCESS;
} /*                        */

/*                                                                            

                                    

                
                                                  
    
                                                                     
                                                  
                                                                      
                                                


             

      
                                                                
                                                                    
                                     

          
                                                                       
                                                                      
                                                                    
                                                       
                                                                          
                                                                           
                                                  

       
                                                                        
                                                      


  
               
                                                            

                                                                            */
VOS_STATUS 
WLANBAP_STAFetchPktCB 
( 
  v_PVOID_t             pvosGCtx,
  v_U8_t*               pucSTAId,
  v_U8_t                ucAC,
  vos_pkt_t**           vosDataBuff,
  WLANTL_MetaInfoType*  tlMetaInfo
)
{
    VOS_STATUS    vosStatus; 
    ptBtampHandle bapHdl;  /*                                     */ 
    ptBtampContext bapContext; /*                                       */ 
    v_PVOID_t     pHddHdl; /*                                 */

    /*                                                 */ 

    vosStatus = WLANBAP_GetCtxFromStaId ( 
            *pucSTAId,  /*                                     */
            &bapHdl,  /*                                            */ 
            &bapContext,  /*                                             */ 
            &pHddHdl); /*                                   */
    if ( VOS_STATUS_SUCCESS != vosStatus ) 
    {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
                   "Unable to retrieve BSL or BAP context from STA Id in WLANBAP_STAFetchPktCB");
      return VOS_STATUS_E_FAULT;
    }

    /*                                                 */ 
    vosStatus = (*bapContext->pfnBtampFetchPktCB)( 
            pHddHdl, 
            (WLANTL_ACEnumType)   ucAC, /*                     */ 
            vosDataBuff, 
            tlMetaInfo);    
    if ( VOS_STATUS_SUCCESS != vosStatus ) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
                "Callback registered by BSL failed to fetch pkt in WLANNBAP_STAFetchPktCB");
        return VOS_STATUS_E_FAULT;
    }

    return vosStatus;
} /*                       */ 

/*                                                                            

                             

                
                                             
    
                                                                      
                         

             

      
                                                                   
                                                                      
                                
                                                                    
                                              
                              
                                                        
   
               
                                                            

                                                                            */
VOS_STATUS 
WLANBAP_STARxCB
( 
  v_PVOID_t          pvosGCtx,
  vos_pkt_t*         vosDataBuff,
  v_U8_t             ucSTAId,
  WLANTL_RxMetaInfoType* pRxMetaInfo
)
{
    VOS_STATUS    vosStatus; 
    ptBtampHandle bapHdl;  /*                                     */ 
    ptBtampContext bapContext; /*                                       */ 
    v_PVOID_t     pHddHdl; /*                                 */
    ptBtampHandle            btampHandle;
    WLANBAP_8023HeaderType   w8023Header;
    v_U8_t                   aucLLCHeader[WLANBAP_LLC_HEADER_LEN];
    v_U16_t                  protoType ;
    v_SIZE_t                 llcHeaderLen = WLANBAP_LLC_HEADER_LEN ;
    
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
                   "In WLANBAP_STARxCB");

    /*                                                 */ 

    vosStatus = WLANBAP_GetCtxFromStaId ( 
            ucSTAId,  /*                                     */
            &bapHdl,  /*                                            */ 
            &bapContext,  /*                                             */ 
            &pHddHdl); /*                                   */
    if ( VOS_STATUS_SUCCESS != vosStatus ) 
    {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
                   "Unable to retrieve BSL or BAP context from STA Id in WLANBAP_STARxCB");
      /*             */
      vos_pkt_return_packet(vosDataBuff);
      return VOS_STATUS_E_FAULT;
    }


    vosStatus = vos_pkt_extract_data( vosDataBuff, sizeof(w8023Header), (v_VOID_t *)aucLLCHeader,
                                   &llcHeaderLen);

    if ( NULL == aucLLCHeader/*          */ )
    {
        VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                 "WLANBAP_STARxCB:Cannot extract LLC header");
        /*             */
        vos_pkt_return_packet(vosDataBuff);
        return VOS_STATUS_E_FAULT;
    }
    
    vos_mem_copy(&protoType,&aucLLCHeader[WLANBAP_LLC_PROTO_TYPE_OFFSET],WLANBAP_LLC_PROTO_TYPE_SIZE);
    protoType = vos_be16_to_cpu(protoType);
    
    VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
            "%s: received : %d, => BAP",__func__,
                 protoType);
    
    if(WLANBAP_BT_AMP_TYPE_DATA == protoType)
    {
        if (bapContext->bapLinkSupervisionTimerInterval)
        {
            /*                              */
            //                                                          
            //                                                               
            bapContext->dataPktPending = VOS_TRUE;//                                                           
            /*                                                 */ 
            vosStatus = (*bapContext->pfnBtamp_STARxCB)( 
                pHddHdl,
                vosDataBuff,
                pRxMetaInfo);
        }
        else
        {
            VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_FATAL,
                     "WLANBAP_STARxCB:bapLinkSupervisionTimerInterval is 0");
            /*             */
            vos_pkt_return_packet(vosDataBuff);
        }
    }
    else
    {
          VOS_TRACE( VOS_MODULE_ID_TL, VOS_TRACE_LEVEL_ERROR,
                "%s: link Supervision packet received over TL: %d, => BAP",
                     __func__,protoType);
          btampHandle = (ptBtampHandle)bapContext; 
          vosStatus = WLANBAP_RxProcLsPkt(
                        btampHandle,
                        bapContext->phy_link_handle,
                        protoType,
                        vosDataBuff
                        );
    }  

    return vosStatus;
} /*                 */


/*                                                                            

                              

                
                                                 
    
                                                                     
                       

             

      
                                                                   
                                                                          
                                
                                                                        
                                               

  
               
                                                            

                                                                            */
VOS_STATUS 
WLANBAP_TxCompCB
( 
  v_PVOID_t      pvosGCtx,
  vos_pkt_t*     vosDataBuff,
  VOS_STATUS     wTxSTAtus 
)
{
    VOS_STATUS    vosStatus; 
    ptBtampHandle bapHdl;  /*                                     */ 
    ptBtampContext bapContext; /*                                       */ 
    v_PVOID_t     pHddHdl; /*                                 */
    v_PVOID_t      pvlogLinkHandle = NULL;
    v_U32_t       value;

    WLANBAP_HCIACLHeaderType hciACLHeader;

    /*                                   */ 

    /*                                                           */
    /*                                                    */
    /*                                                                     */
    /*                                                            */
    //                                      
    //                                                           
    //                                                                   
    //                                                                        
    //                                                          
    /*                                */ 
    //                   
    bapHdl = (v_PVOID_t)gpBtampCtx;
    /*                                                                   */ 
    bapContext = ((ptBtampContext) bapHdl);  

    /*                                                                        
                         
                                                                            */
    if ( NULL == vosDataBuff) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "Invalid vosDataBuff value in %s", __func__);
        return VOS_STATUS_E_FAULT;
    }

    if ( NULL == bapContext) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "Invalid bapContext value in %s", __func__);
        vos_pkt_return_packet( vosDataBuff ); 
        return VOS_STATUS_E_FAULT;
    }

    pHddHdl = bapContext->pHddHdl;
    vosStatus = VOS_STATUS_SUCCESS;
    if ( VOS_STATUS_SUCCESS != vosStatus ) 
    {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
                   "Unable to retrieve BSL or BAP context from STA Id in WLANBAP_TxCompCB");
      vos_pkt_return_packet( vosDataBuff ); 
      return VOS_STATUS_E_FAULT;
    }

    /*                                                  */
    vos_pkt_get_user_data_ptr( vosDataBuff, VOS_PKT_USER_DATA_ID_BAP,
                               &pvlogLinkHandle);

    value = (v_U32_t)pvlogLinkHandle;
    hciACLHeader.logLinkHandle = value;

#ifdef BAP_DEBUG
    /*                                  */
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
              "WLAN BAP Context Monitor: bapContext value = %p in %s:%d. vosDataBuff=%p", bapContext, __func__, __LINE__, vosDataBuff );
#endif //         

    //                                        
//                              
    if (BTAMP_VALID_LOG_LINK( hciACLHeader.logLinkHandle))
    {
       vos_atomic_increment_U32(
           &bapContext->btampLogLinkCtx[hciACLHeader.logLinkHandle].uTxPktCompleted);
//                                                            
//                                
//                                                            
    } else 
    {
       VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "In %s:%d: Invalid logical link handle: %d", __func__, __LINE__, hciACLHeader.logLinkHandle);
    }

    /*                                                 */ 
    vosStatus = (*bapContext->pfnBtampTxCompCB)( 
            pHddHdl,
            vosDataBuff,
            wTxSTAtus);

    return vosStatus;
} /*                  */

/*                                                                          

                                       

              
                                                                    
                                                                    
                                                                   
                                                                   
                                                                    
    
                                                                      
                                                                      
                                                                        
    
               
    
             

      
                                                                      
   
              
                                                              

                                                                               
                                
                                               

               
  
                                                                            */
VOS_STATUS 
WLANBAP_RegisterDataPlane
( 
  ptBtampHandle btampHandle,  /*               */ 
  WLANBAP_STAFetchPktCBType pfnBtampFetchPktCB, 
  WLANBAP_STARxCBType pfnBtamp_STARxCB,
  WLANBAP_TxCompCBType pfnBtampTxCompCB,
  //                                                                                     
  v_PVOID_t      pHddHdl   /*                      */
)
{
    ptBtampContext pBtampCtx = (ptBtampContext) btampHandle; 

  
    /*                                                                        
                         
                                                                             */
    if ( NULL == pBtampCtx) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "Invalid BAP handle value in WLANBAP_RegisterDataPlane");
        return VOS_STATUS_E_FAULT;
    }

    //                                                                      
    pBtampCtx->pfnBtampFetchPktCB = pfnBtampFetchPktCB; 
    pBtampCtx->pfnBtamp_STARxCB = pfnBtamp_STARxCB;
    pBtampCtx->pfnBtampTxCompCB = pfnBtampTxCompCB;

    //                               
    pBtampCtx->pHddHdl = pHddHdl;
    /*                                    */ 
    pBtampCtx->ucDataTrafficMode = WLANBAP_FLOW_CONTROL_MODE_BLOCK_BASED;

    return VOS_STATUS_SUCCESS;
} /*                           */


/*                                                                           

                                   

              

                                                                        
                                                                       
                                                    

               

                                                                    

             

                                                                      
                                                                          
                                                                    
                                                                            
                                                                            
                                        
                                                                     
                                                          
   
              

                                                              

                                                      
                                             
                                               

               
  
                                                                            */
VOS_STATUS
WLANBAP_STAPktPending 
( 
  ptBtampHandle  btampHandle,  /*                                                           */ 
  v_U8_t         phy_link_handle,  /*                                                  */
  WLANTL_ACEnumType ucAc   /*                                                     */
)
{
    VOS_STATUS     vosStatus; 
    ptBtampContext pBtampCtx = (ptBtampContext) btampHandle; 
    v_PVOID_t      pvosGCtx;
    v_U8_t         ucSTAId;  /*                                     */
    v_PVOID_t      pHddHdl; /*                                 */

  
#ifdef BAP_DEBUG
    /*                                      */
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
              "WLAN BAP Context Monitor: pBtampCtx value = %p in %s:%d", pBtampCtx, __func__, __LINE__ );
#endif //         

    /*                                                                        
                         
                                                                             */
    if ( NULL == pBtampCtx) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "Invalid BAP handle value in WLANBAP_STAPktPending"); 
        return VOS_STATUS_E_FAULT;
    }

    //                          
    pvosGCtx = pBtampCtx->pvosGCtx;
 
    /*                                                                */ 

    vosStatus = WLANBAP_GetStaIdFromLinkCtx ( 
            btampHandle,  /*                       */ 
            phy_link_handle,  /*                          */
            &ucSTAId,  /*                                     */
            &pHddHdl); /*                              */
    if ( VOS_STATUS_SUCCESS != vosStatus ) 
    {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
                   "Unable to retrieve STA Id from BAP context and phy_link_handle in WLANBAP_STAPktPending");
      return VOS_STATUS_E_FAULT;
    }


    //                                        
    vosStatus = WLANTL_STAPktPending( 
            pvosGCtx,
            ucSTAId,
            ucAc);
    if ( VOS_STATUS_SUCCESS != vosStatus ) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                        "Tx: Packet rejected by TL in WLANBAP_STAPktPending");
        return vosStatus;
    }            
    pBtampCtx->dataPktPending = VOS_TRUE;//                                                           
    return VOS_STATUS_SUCCESS;
} /*                       */ 

/*                                                                            

                                             

              
                                       
                                   

               
        

             

      
                                                                      
                                                  
                                                                                

   
              
                                                              

                                                           
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPRegisterBAPCallbacks 
( 
  ptBtampHandle           btampHandle, /*                                  */
                            /*                                    */
                            /*                                         */
                            /*                                   */
  tpWLAN_BAPEventCB       pBapHCIEventCB, /*                                                     */ 
  v_PVOID_t               pAppHdl  //                    
)
{
    ptBtampContext pBtampCtx = (ptBtampContext) btampHandle; 

  
    /*                                                                        
                         
                                                                             */
    if ( NULL == pBtampCtx) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "Invalid BAP handle value in WLAN_BAPRegisterBAPCallbacks");
        return VOS_STATUS_E_FAULT;
    }

    //                         
    pBtampCtx->pBapHCIEventCB = pBapHCIEventCB; 

    //                               
    pBtampCtx->pAppHdl = pAppHdl;

    return VOS_STATUS_SUCCESS;
} /*                              */



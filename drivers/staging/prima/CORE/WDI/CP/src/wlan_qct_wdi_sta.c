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

                       W L A N _ Q C T _ W D I _ S T A . C

  OVERVIEW:

  This software unit holds the implementation of the WLAN Device Abstraction     
  Layer Station Table Management Entity.

  The functions externalized by this module are internal APIs for DAL Core
  and can only be called by it. 

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
#include "wlan_qct_wdi.h" 
#include "wlan_qct_wdi_i.h" 
#include "wlan_qct_wdi_sta.h" 
#include "wlan_qct_pal_api.h"
#include "wlan_qct_pal_trace.h"


/*                                                                            
                      
                                                                            */
/* 
                                                       
                                      

 
                                                          
  
     
                                    
*/
WDI_Status WDI_STATableInit
(
   WDI_ControlBlockType*  pWDICtx
)
{
    wpt_uint8  ucMaxStations;
    /*                                                                       */    

    ucMaxStations     = (wpt_uint8) pWDICtx->ucMaxStations;
    
    /*                                                                      
                                        
                                                                            */
    pWDICtx->staTable = wpalMemoryAllocate(ucMaxStations * sizeof(WDI_StaStruct));

    if (NULL == pWDICtx->staTable)
    {
            
        WDI_STATableClose(pWDICtx);

        WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                  "Error allocating memory on WDI_STATableInit"); 
        return WDI_STATUS_E_FAILURE;
    }
    
    wpalMemoryZero( pWDICtx->staTable, ucMaxStations * sizeof( WDI_StaStruct ));

#ifndef HAL_SELF_STA_PER_BSS
    //                                              
    pWDICtx->ucSelfStaId = WDI_STA_INVALID_IDX;
#endif

    return WDI_STATUS_SUCCESS;
}/*                */

/* 
                                                                
                

 
                                                          
  
     
                                    
*/
WDI_Status 
WDI_STATableStart
(
    WDI_ControlBlockType*  pWDICtx
)
{
    wpt_uint8 ucMaxStations;
    /*                                                                  */
    
    ucMaxStations     = (wpt_uint8) pWDICtx->ucMaxStations;
 
    return WDI_STATUS_SUCCESS;
}/*                 */

/* 
                                               

 
                                                          
  
     
                                    
*/
WDI_Status 
WDI_STATableStop
(
    WDI_ControlBlockType*  pWDICtx
)
{
    wpt_uint8 ucMaxStations;
    /*                                                                     */

#ifndef HAL_SELF_STA_PER_BSS
    /*                         */
    pWDICtx->ucSelfStaId = WDI_STA_INVALID_IDX;
#endif

    ucMaxStations     = pWDICtx->ucMaxStations;
    
    wpalMemoryZero( (void *) pWDICtx->staTable,
            ucMaxStations * sizeof( WDI_StaStruct ));

    return WDI_STATUS_SUCCESS;
}/*                */

/* 
                                                                
              

 
                                                          
  
     
                                    
*/
WDI_Status 
WDI_STATableClose
(
  WDI_ControlBlockType*  pWDICtx
)
{
    WDI_Status status = WDI_STATUS_SUCCESS;
        
    //            
    if (pWDICtx->staTable != NULL)
        wpalMemoryFree( pWDICtx->staTable);

    pWDICtx->staTable = NULL;
    return status;
}/*                 */


/* 
                                                    

 
                                                      
                                          
  
     
                                    
*/
WDI_Status
WDI_STATableAddSta
(
    WDI_ControlBlockType*  pWDICtx,
    WDI_AddStaParams*      pwdiParam
)
{
    wpt_uint8        ucSTAIdx  = 0;
    WDI_StaStruct*   pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
    /*                                                                     */

    /*                                                                       
                  
                                                                           
                      
                                                                           */
    if (( pwdiParam->ucSTAIdx  == WDI_STA_INVALID_IDX) ||
        ( pwdiParam->ucSTAIdx >= pWDICtx->ucMaxStations ))
    {
      WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                "Station id sent by HAL is invalid - not OK"); 
      WDI_ASSERT(0); 
      return WDI_STATUS_E_FAILURE; 
    }
    
    ucSTAIdx =  pwdiParam->ucSTAIdx;

    /*                                                                       
           */
    pSTATable[ucSTAIdx].valid = 1;     
    
    
    //                                            
    WDI_STATableSetStaType(pWDICtx, ucSTAIdx, pwdiParam->ucStaType);
    WDI_STATableSetStaQosEnabled(pWDICtx, ucSTAIdx, 
          (wpt_uint8)(pwdiParam->ucWmmEnabled | pwdiParam->ucHTCapable) );

#ifdef WLAN_PERF
    pWDICtx->uBdSigSerialNum ++;
#endif
    
    wpalMemoryCopy(pSTATable[ucSTAIdx].macBSSID, 
                   pwdiParam->macBSSID, WDI_MAC_ADDR_LEN);

    /*                                                                        
                                  
                                                                            */
    pSTATable[ucSTAIdx].dpuIndex              = pwdiParam->dpuIndex; 
    pSTATable[ucSTAIdx].dpuSig                = pwdiParam->dpuSig; 

    pSTATable[ucSTAIdx].bcastDpuIndex         = pwdiParam->bcastDpuIndex; 
    pSTATable[ucSTAIdx].bcastDpuSignature     = pwdiParam->bcastDpuSignature; 

    pSTATable[ucSTAIdx].bcastMgmtDpuIndex     = pwdiParam->bcastMgmtDpuIndex; 
    pSTATable[ucSTAIdx].bcastMgmtDpuSignature = pwdiParam->bcastMgmtDpuSignature; 

    /*                          */
    pSTATable[ucSTAIdx].rmfEnabled            = pwdiParam->ucRmfEnabled;

    pSTATable[ucSTAIdx].bssIdx                = pwdiParam->ucBSSIdx;

    /*                                                   */
    if(WDI_STATUS_SUCCESS != WDI_STATableSetStaAddr( pWDICtx, 
                                                     ucSTAIdx, 
                                                     pwdiParam->staMacAddr))
    {
       WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "Failed to update station entry - internal failure");
       WDI_ASSERT(0);
       return WDI_STATUS_E_FAILURE; 
    }

    /*                                                     */
    if(WDI_STATUS_SUCCESS != WDI_STATableSetBSSID( pWDICtx, 
                                                     ucSTAIdx, 
                                                     pwdiParam->macBSSID))
    {
       WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "Failed to update station entry - internal failure");
       WDI_ASSERT(0);
       return WDI_STATUS_E_FAILURE; 
    }

    return WDI_STATUS_SUCCESS;
}/*          */

/* 
                                                         

 
                                                          
                                               
  
     
                                    
*/
WDI_Status
WDI_STATableDelSta
(
    WDI_ControlBlockType*  pWDICtx,
    wpt_uint8              ucSTAIdx
)
{
    WDI_StaStruct*   pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
    /*                                                                     */

    /*                                                                       
                  
                                                                           
                      
                                                                           */
    if(( ucSTAIdx  == WDI_STA_INVALID_IDX )||
        ( ucSTAIdx >= pWDICtx->ucMaxStations ))
    {
       WPAL_TRACE(eWLAN_MODULE_DAL_CTRL, eWLAN_PAL_TRACE_LEVEL_ERROR,
                 "STA Id invalid on Del STA - internal failure");
       WDI_ASSERT(0);
       return WDI_STATUS_E_FAILURE; 
    }
    
    wpalMemoryZero(&pSTATable[ucSTAIdx], sizeof(pSTATable[ucSTAIdx])); 
    pSTATable->valid = 0; 
    return WDI_STATUS_SUCCESS;
}/*                  */

/* 
                                                                       

 
                                                          
                                    
  
     
                                    
*/
WDI_Status
WDI_STATableBSSDelSta
(
    WDI_ControlBlockType*  pWDICtx,
    wpt_uint8              ucBSSIdx
)
{
    WDI_StaStruct*   pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
    wpt_uint8        ucSTAIdx;
    /*                                                                     */

    for (ucSTAIdx = 0; (ucSTAIdx < pWDICtx->ucMaxStations); ucSTAIdx++)
    {
        if( (pSTATable[ucSTAIdx].ucStaType == WDI_STA_ENTRY_PEER) && 
                                 (pSTATable[ucSTAIdx].bssIdx == ucBSSIdx))
        {
            WDI_STATableDelSta(pWDICtx, ucSTAIdx);
        }
    }

    return WDI_STATUS_SUCCESS;
}/*                     */


/* 
                                                                
                         

 
                                                          
                                       
                                               
  
     
                                    
*/
WDI_Status 
WDI_STATableGetStaBSSIDAddr
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_macAddr*           pmacBSSID
)
{
  WDI_StaStruct* pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
  /*                                                                      */

  if ((ucSTAIdx < pWDICtx->ucMaxStations) && (pSTATable[ucSTAIdx].valid))
  {
     wpalMemoryCopy(*pmacBSSID, pSTATable[ucSTAIdx].macBSSID, WDI_MAC_ADDR_LEN);
     return WDI_STATUS_SUCCESS;
  }
  else
     return WDI_STATUS_E_FAILURE;
}/*                            */


/* 
                                                              
                 

 
                                                          
                                       
                                         
  
     
                                    
*/
WDI_Status 
WDI_STATableGetStaQosEnabled
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_uint8*             qosEnabled
)
{
  WDI_StaStruct* pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
  /*                                                                      */

  if ((ucSTAIdx < pWDICtx->ucMaxStations) && (pSTATable[ucSTAIdx].valid) && qosEnabled)
  {
     *qosEnabled = pSTATable[ucSTAIdx].qosEnabled;
     return WDI_STATUS_SUCCESS;
  }
  else
     return WDI_STATUS_E_FAILURE;
}/*                            */

/* 
                                                           

 
                                                     
                                  
                                
  
     
                                    
*/
WDI_Status 
WDI_STATableSetStaQosEnabled
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_uint8              qosEnabled
)
{
    WDI_StaStruct* pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
    if ((ucSTAIdx < pWDICtx->ucMaxStations) && (pSTATable[ucSTAIdx].valid))
    {
        pSTATable[ucSTAIdx].qosEnabled = qosEnabled;
        return WDI_STATUS_SUCCESS;
    }
    else
        return WDI_STATUS_E_FAILURE;
}/*                            */

/* 
                                                     

 
                                                    
                                 
                               
  
     
                                    
*/
WDI_Status 
WDI_STATableGetStaType
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_uint8*             pStaType
)
{
    WDI_StaStruct* pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
    if ((ucSTAIdx < pWDICtx->ucMaxStations) && (pSTATable[ucSTAIdx].valid))
    {
        *pStaType = pSTATable[ucSTAIdx].ucStaType;
        return WDI_STATUS_SUCCESS;
    }
    else
        return WDI_STATUS_E_FAILURE;
}/*                      */

/* 
                                                      

 
                                                    
                                 
                            
  
     
                                    
*/
WDI_Status 
WDI_STATableSetStaType
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_uint8              staType
)
{
    WDI_StaStruct* pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
    if ((ucSTAIdx < pWDICtx->ucMaxStations) && (pSTATable[ucSTAIdx].valid))
    {
        pSTATable[ucSTAIdx].ucStaType = staType;
        return WDI_STATUS_SUCCESS;
    }
    else
        return WDI_STATUS_E_FAILURE;
}/*                      */


/* 
                                                    

 
                          
                           
  
     
                                    
*/
WPT_STATIC WPT_INLINE wpt_uint8
WDI_CompareMacAddr
(
  wpt_uint8 addr1[], 
  wpt_uint8 addr2[]
)
{
#if defined( _X86_ )
    wpt_uint32 align = (0x3 & ((wpt_uint32) addr1 | (wpt_uint32) addr2 ));

    if( align ==0){
        return ((*((wpt_uint16 *) &(addr1[4])) == *((wpt_uint16 *) &(addr2[4])))&&
                (*((wpt_uint32 *) addr1) == *((wpt_uint32 *) addr2)));
    }else if(align == 2){
        return ((*((wpt_uint16 *) &addr1[4]) == *((wpt_uint16 *) &addr2[4])) &&
            (*((wpt_uint16 *) &addr1[2]) == *((wpt_uint16 *) &addr2[2])) &&
            (*((wpt_uint16 *) &addr1[0]) == *((wpt_uint16 *) &addr2[0])));
    }else{
        return ( (addr1[5]==addr2[5])&&
            (addr1[4]==addr2[4])&&
            (addr1[3]==addr2[3])&&
            (addr1[2]==addr2[2])&&
            (addr1[1]==addr2[1])&&
            (addr1[0]==addr2[0]));
    }
#else
         return ( (addr1[0]==addr2[0])&&
            (addr1[1]==addr2[1])&&
            (addr1[2]==addr2[2])&&
            (addr1[3]==addr2[3])&&
            (addr1[4]==addr2[4])&&
            (addr1[5]==addr2[5]));
#endif
}/*                  */


/* 
                                                                         
                                                                   
 
                                      
                                  
                                     
  
     
                                    
*/
WDI_Status
WDI_STATableFindStaidByAddr
(
    WDI_ControlBlockType*  pWDICtx, 
    wpt_macAddr            staAddr, 
    wpt_uint8*             pucStaId
)
{
    WDI_Status wdiStatus = WDI_STATUS_E_FAILURE;
    wpt_uint8 i;
    WDI_StaStruct* pSTATable = (WDI_StaStruct*) pWDICtx->staTable;

    for (i=0; i < pWDICtx->ucMaxStations; i++, pSTATable++)
    {
        if ( (pSTATable->valid == 1) && (WDI_CompareMacAddr(pSTATable->staAddr, staAddr)) )
        {
            *pucStaId = i;
            wdiStatus = WDI_STATUS_SUCCESS;
            break;
        }
    }
    return wdiStatus;
}/*                           */

/* 
                                                    
 
                                      
                                 
                                          
  
     
                                    
*/
WDI_Status 
WDI_STATableGetStaAddr
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_uint8**            pStaAddr
)
{
    WDI_StaStruct* pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
    if ((ucSTAIdx < pWDICtx->ucMaxStations) && (pSTATable[ucSTAIdx].valid))
    {
        *pStaAddr = pSTATable[ucSTAIdx].staAddr;
        return WDI_STATUS_SUCCESS;
    }
    else
        return WDI_STATUS_E_FAILURE;
}/*                      */

/* 
                                                    
 
                                      
                                  
                                          
  
     
                                    
*/
WDI_Status 
WDI_STATableSetStaAddr
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_macAddr            staAddr
)
{
    WDI_StaStruct* pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
    if ((ucSTAIdx < pWDICtx->ucMaxStations) && (pSTATable[ucSTAIdx].valid))
    {
        wpalMemoryCopy (pSTATable[ucSTAIdx].staAddr, staAddr, 6);
        return WDI_STATUS_SUCCESS;
    }
    else
        return WDI_STATUS_E_FAILURE;
}/*                      */

/* 
                                                              
 
                                      
                                  
                                          
  
     
                                    
*/
WDI_Status 
WDI_STATableSetBSSID
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_macAddr            macBSSID
)
{
    WDI_StaStruct* pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
    if ((ucSTAIdx < pWDICtx->ucMaxStations) && (pSTATable[ucSTAIdx].valid))
    {
        wpalMemoryCopy (pSTATable[ucSTAIdx].macBSSID, macBSSID, 6);
        return WDI_STATUS_SUCCESS;
    }
    else
        return WDI_STATUS_E_FAILURE;
}/*                    */

/* 
                                                                   
 
                                      
                                  
                              
  
     
                                    
*/
WDI_Status 
WDI_STATableSetBSSIdx
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_uint8              ucBSSIdx
)
{
    WDI_StaStruct* pSTATable = (WDI_StaStruct*) pWDICtx->staTable;
    if ((ucSTAIdx < pWDICtx->ucMaxStations) && (pSTATable[ucSTAIdx].valid))
    {
        pSTATable[ucSTAIdx].bssIdx = ucBSSIdx;
        return WDI_STATUS_SUCCESS;
    }
    else
        return WDI_STATUS_E_FAILURE;
}/*                     */


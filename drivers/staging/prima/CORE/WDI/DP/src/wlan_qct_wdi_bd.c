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
 *       \file  wlan_qct_dti_bd.c
 *
 *       \brief Datapath utilities file.
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
#include "wlan_qct_wdi_ds.h"
#include "wlan_qct_wdi_ds_i.h"
#include "wlan_qct_wdi_dts.h"
#include "wlan_qct_wdi_dp.h"
#include "wlan_qct_pal_type.h"
#include "wlan_qct_pal_status.h"
#include "wlan_qct_pal_api.h"
#include "wlan_qct_pal_packet.h"



/*                                                                          
  
                                   

            
                                                        

           

   
                                 


            
          

            

                                                                            */
void
WDI_DS_PrepareBDHeader (wpt_packet* palPacket,
                        wpt_uint8 ucDisableHWFrmXtl, wpt_uint8 alignment)
{
  void*          pvBDHeader;
  wpt_uint8      ucHeaderOffset;
  wpt_uint8      ucHeaderLen;
  wpt_uint8      ucQosEnabled;
  wpt_uint8      ucWDSEnabled;
  wpt_uint32     ucMpduLen;
  wpt_uint32     ucPktLen;
  WDI_DS_TxMetaInfoType     *pTxMetadata;


  /*                                            */
  pvBDHeader = WPAL_PACKET_GET_BD_POINTER(palPacket);
  pTxMetadata = WDI_DS_ExtractTxMetaData(palPacket);
  ucQosEnabled = pTxMetadata->qosEnabled;
  ucWDSEnabled = pTxMetadata->fenableWDS;

  WPAL_PACKET_SET_BD_LENGTH(palPacket, WDI_TX_BD_HEADER_SIZE);

  /*                                                                     
                          
                            
                              
                        
                                                        
                                                                        */

  if ( ucDisableHWFrmXtl ) {
    ucHeaderOffset = WDI_TX_BD_HEADER_SIZE;
    ucHeaderLen = WDI_802_11_HEADER_LEN;
    if ( 0 != ucQosEnabled ) {
      ucHeaderLen += WDI_802_11_HEADER_QOS_CTL;
    }
    if ( 0 != ucWDSEnabled) {
      ucHeaderLen    += WDI_802_11_HEADER_ADDR4_LEN;
    }
  } else {
    ucHeaderOffset = WDI_TX_BD_HEADER_SIZE+WDI_802_11_MAX_HEADER_LEN;
    ucHeaderLen = WDI_802_3_HEADER_LEN;
  }

  WDI_TX_BD_SET_MPDU_HEADER_LEN( pvBDHeader, ucHeaderLen);
  WDI_TX_BD_SET_MPDU_HEADER_OFFSET( pvBDHeader, ucHeaderOffset);
  WDI_TX_BD_SET_MPDU_DATA_OFFSET( pvBDHeader,
      ucHeaderOffset + ucHeaderLen + alignment);

  //                                                              
  ucPktLen  = wpalPacketGetLength( palPacket ); //                        
  /*                                                            
                                                                  */
  ucMpduLen = ucPktLen - WPAL_PACKET_GET_BD_LENGTH( palPacket );
  WDI_TX_BD_SET_MPDU_LEN( pvBDHeader, ucMpduLen );

  DTI_TRACE(  DTI_TRACE_LEVEL_INFO,
      "WLAN DTI: VALUES ARE HLen=%x Hoff=%x doff=%x len=%x ex=%d",
      ucHeaderLen, ucHeaderOffset,
      (ucHeaderOffset + ucHeaderLen + alignment),
      pTxMetadata->fPktlen, alignment);

}/*                        */

/*                                                                          
  
                               

            
                                             
           

   
                                             



                                                                            */

/*
                                             
 */
WDI_Status WDI_DS_MemPoolCreate(WDI_DS_BdMemPoolType *memPool, wpt_uint8 chunkSize,
                                                                  wpt_uint8 numChunks)
{
  wpt_uint8 staLoop;

  //                                                                                                        
  memPool->numChunks = 0;
  memPool->chunkSize = chunkSize + 16 - (chunkSize%8);
  memPool->pVirtBaseAddress = wpalDmaMemoryAllocate((numChunks * memPool->chunkSize),
          &(memPool->pPhysBaseAddress));

  if( memPool->pVirtBaseAddress == 0)
    return WDI_STATUS_E_FAILURE;

  memPool->AllocationBitmap = (wpt_uint32*)wpalMemoryAllocate( (numChunks/32 + 1) * sizeof(wpt_uint32));
  if( NULL == memPool->AllocationBitmap)
     return WDI_STATUS_E_FAILURE;
  wpalMemoryZero(memPool->AllocationBitmap, (numChunks/32+1)*sizeof(wpt_uint32));

  //                                 
  for(staLoop = 0; staLoop < WDI_DS_MAX_STA_ID; staLoop++)
  {
    memPool->numChunkSTA[staLoop].STAIndex = 0xFF;
    memPool->numChunkSTA[staLoop].numChunkReservedBySTA = 0;
    memPool->numChunkSTA[staLoop].validIdx = 0;
  }

  return WDI_STATUS_SUCCESS;
}

/*
                          
 */
void WDI_DS_MemPoolDestroy(WDI_DS_BdMemPoolType *memPool)
{
  //                          
  wpalDmaMemoryFree(memPool->pVirtBaseAddress);
  wpalMemoryFree(memPool->AllocationBitmap);
  wpalMemoryZero(memPool, sizeof(*memPool));
}
/*
                        
 */
WPT_STATIC WPT_INLINE int find_leading_zero_and_setbit(wpt_uint32 *bitmap, wpt_uint32 maxNumPool)
{
  wpt_uint32 i,j, word;
  int ret_val = -1;

  for(i=0; i < (maxNumPool/32 + 1); i++){
    j = 0;
    word = bitmap[i];
    for(j=0; j< 32; j++){
      if((word & 1) == 0) {
        bitmap[i] |= (1 << j);
        return((i<<5) + j);
      }
      word >>= 1;
    }
  }
  return ret_val;
}

void *WDI_DS_MemPoolAlloc(WDI_DS_BdMemPoolType *memPool, void **pPhysAddress,
                               WDI_ResPoolType wdiResPool)
{
  wpt_uint32 index;
  void *pVirtAddress;
  wpt_uint32 maxNumPool;
  switch(wdiResPool)
  {
    case WDI_MGMT_POOL_ID:
      maxNumPool = WDI_DS_HI_PRI_RES_NUM;
      break;
    case WDI_DATA_POOL_ID:
       maxNumPool = WDI_DS_LO_PRI_RES_NUM;
      break;
    default:
      return NULL;
  }

  if(maxNumPool == memPool->numChunks)
  {
     return NULL;
  }
  //                                           

  if((index = find_leading_zero_and_setbit(memPool->AllocationBitmap, maxNumPool)) == -EPERM)
  {
     //                
     DTI_TRACE(  DTI_TRACE_LEVEL_INFO, "WDI_DS_MemPoolAlloc: index:%d(NULL), numChunks:%d",
                  index, memPool->numChunks );
     return NULL;
  }
  memPool->numChunks++;
  //                                                                           
  pVirtAddress  = (wpt_uint8 *)memPool->pVirtBaseAddress + (memPool->chunkSize * index) + 8;
  *pPhysAddress = (wpt_uint8 *)memPool->pPhysBaseAddress + (memPool->chunkSize * index) + 8;

  DTI_TRACE(  DTI_TRACE_LEVEL_INFO, "WDI_DS_MemPoolAlloc: index:%d, numChunks:%d", index, memPool->numChunks );

  return pVirtAddress;

}

/*
                    
 */
void  WDI_DS_MemPoolFree(WDI_DS_BdMemPoolType *memPool, void *pVirtAddress, void *pPhysAddress)
{
  wpt_uint32 index =
    ((wpt_uint8 *)pVirtAddress - (wpt_uint8 *)memPool->pVirtBaseAddress - 8)/memPool->chunkSize;
  wpt_uint32 word = memPool->AllocationBitmap[index/32];
  word &= ~(1<<(index%32));
  memPool->AllocationBitmap[index/32] = word;
  memPool->numChunks--;

  //                                                                                    
}


/* 
                                                              
                        

                                                        

     
                                    
*/
wpt_uint32 WDI_DS_GetAvailableResCount(WDI_DS_BdMemPoolType *pMemPool)
{
  return pMemPool->numChunks;
}

/* 
                            
                                

                                                        
                                 

     
                                    
*/
WDI_Status WDI_DS_MemPoolAddSTA(WDI_DS_BdMemPoolType *memPool, wpt_uint8 staIndex)
{
  if(memPool->numChunkSTA[staIndex].STAIndex != 0xFF)
  {
    /*                                     */
    return WDI_STATUS_SUCCESS;
  }

  memPool->numChunkSTA[staIndex].STAIndex = staIndex;
  memPool->numChunkSTA[staIndex].numChunkReservedBySTA = 0;
  memPool->numChunkSTA[staIndex].validIdx = 1;
  return WDI_STATUS_SUCCESS;
}

/* 
                            
                               

                                                        
                                 

     
                                    
*/
WDI_Status WDI_DS_MemPoolDelSTA(WDI_DS_BdMemPoolType *memPool, wpt_uint8 staIndex)
{
  if(memPool->numChunkSTA[staIndex].STAIndex == 0xFF)
  {
    /*                                      */
      return WDI_STATUS_E_FAILURE;
  }

  memPool->numChunkSTA[staIndex].STAIndex = 0xFF;
  memPool->numChunkSTA[staIndex].numChunkReservedBySTA = 0;
  memPool->numChunkSTA[staIndex].validIdx = 0;
  return WDI_STATUS_SUCCESS;
}

/* 
                                                                     
                        

                                                        
                                 
     
                                    
*/
wpt_uint32 WDI_DS_MemPoolGetRsvdResCountPerSTA(WDI_DS_BdMemPoolType *pMemPool, wpt_uint8  staId)
{
  return pMemPool->numChunkSTA[staId].numChunkReservedBySTA;
}

/* 
                                                           

                                                        
                                 
     
                                    
*/
void WDI_DS_MemPoolIncreaseReserveCount(WDI_DS_BdMemPoolType *memPool, wpt_uint8  staId)
{

  if((memPool->numChunkSTA[staId].validIdx) && (staId < WDI_DS_MAX_STA_ID))
  {
    memPool->numChunkSTA[staId].numChunkReservedBySTA++;
  }
  return;
}

/* 
                                                           

                                                        
                                 
     
                                    
*/
void WDI_DS_MemPoolDecreaseReserveCount(WDI_DS_BdMemPoolType *memPool, wpt_uint8  staId)
{
  if(0 == memPool->numChunkSTA[staId].numChunkReservedBySTA)
  {
    DTI_TRACE( DTI_TRACE_LEVEL_ERROR,
               "SAT %d reserved resource count cannot be smaller than 0", staId );
    return;
  }

  if((memPool->numChunkSTA[staId].validIdx) && (staId < WDI_DS_MAX_STA_ID))
  {
    memPool->numChunkSTA[staId].numChunkReservedBySTA--;
  }
  return;
}

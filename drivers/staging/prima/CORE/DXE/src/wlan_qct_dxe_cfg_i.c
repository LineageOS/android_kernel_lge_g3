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
  
  @file  wlan_qct_dxe_cfg_i.c
  
  @brief 
               
   This file contains the external API exposed by the wlan data transfer abstraction layer module.
   Copyright (c) 2011 QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                                   


                                          
                                                                                  
                                       

                                                                           */

/*                                                                           

                                                  

                                                                           */

/*                                                                            
                
                                                                            */
#include "wlan_qct_dxe_i.h"

/*                                                                            
                                         
                                                                            */
typedef struct
{
   WDTS_ChannelType           wlanChannel;
   WLANDXE_DMAChannelType     DMAChannel;
   WLANDXE_ChannelConfigType *channelConfig;
} WLANDXE_ChannelMappingType;

wpt_uint32 channelBaseAddressList[WLANDXE_DMA_CHANNEL_MAX] =
{
   WLANDXE_DMA_CHAN0_BASE_ADDRESS,
   WLANDXE_DMA_CHAN1_BASE_ADDRESS,
   WLANDXE_DMA_CHAN2_BASE_ADDRESS,
   WLANDXE_DMA_CHAN3_BASE_ADDRESS,
   WLANDXE_DMA_CHAN4_BASE_ADDRESS,
   WLANDXE_DMA_CHAN5_BASE_ADDRESS,
   WLANDXE_DMA_CHAN6_BASE_ADDRESS
};

wpt_uint32 channelInterruptMask[WLANDXE_DMA_CHANNEL_MAX] =
{
   WLANDXE_INT_MASK_CHAN_0,
   WLANDXE_INT_MASK_CHAN_1,
   WLANDXE_INT_MASK_CHAN_2,
   WLANDXE_INT_MASK_CHAN_3,
   WLANDXE_INT_MASK_CHAN_4,
   WLANDXE_INT_MASK_CHAN_5,
   WLANDXE_INT_MASK_CHAN_6
};

WLANDXE_ChannelConfigType chanTXLowPriConfig =
{
   /*                         */
   WLANDXE_CHANNEL_HANDLE_CIRCULA,

   /*                                         */
   WLANDXE_LO_PRI_RES_NUM ,

   /*                   */
   0,

   /*                    */
   23,

   /*                          */
   0,

   /*               */
   WLANDXE_DESC_CTRL_XTYPE_H2B,

   /*                                                           */
   4,

   /*                                     */
   eWLAN_PAL_TRUE,

   /*                            */
   0,

   /*                              */
   5,

   /*                                               */
   eWLAN_PAL_TRUE,

   /*                      */
   eWLAN_PAL_TRUE
};

WLANDXE_ChannelConfigType chanTXHighPriConfig =
{
   /*                         */
   WLANDXE_CHANNEL_HANDLE_CIRCULA,

   /*                                         */
   WLANDXE_HI_PRI_RES_NUM ,

   /*                   */
   0,

   /*                    */
   23,

   /*                          */
   0,

   /*               */
   WLANDXE_DESC_CTRL_XTYPE_H2B,

   /*                                                            */
   6,

   /*                                     */
   eWLAN_PAL_TRUE,

   /*                            */
   0,

   /*                              */
   7,

   /*                                              */
   eWLAN_PAL_TRUE,

   /*                      */
   eWLAN_PAL_TRUE
};

WLANDXE_ChannelConfigType chanRXLowPriConfig =
{
   /*                         */
   WLANDXE_CHANNEL_HANDLE_CIRCULA,

   /*                                         */
   256,

   /*                                      */
   1,

   /*                                 */
   /*                        */
   11,

   /*                          */
   0,

   /*               */
   WLANDXE_DESC_CTRL_XTYPE_B2H,

   /*                                                            */
   5,

   /*                                     */
   eWLAN_PAL_TRUE,

   /*                            */
   0,

   /*                              */
   6,

   /*                                              */
   eWLAN_PAL_TRUE,

   /*                      */
   eWLAN_PAL_TRUE
};

WLANDXE_ChannelConfigType chanRXHighPriConfig =
{
   /*                         */
   WLANDXE_CHANNEL_HANDLE_CIRCULA,

   /*                                         */
   256,

   /*                                      */
   1,

   /*                    */
   4,

   /*                          */
   0,

   /*               */
   WLANDXE_DESC_CTRL_XTYPE_B2H,

   /*                                                            */
   6,

   /*                                     */
   eWLAN_PAL_TRUE,

   /*                            */
   0,

   /*                              */
   8,

   /*                                              */
   eWLAN_PAL_TRUE,

   /*                      */
   eWLAN_PAL_TRUE
};

#ifdef WLANDXE_TEST_CHANNEL_ENABLE
WLANDXE_ChannelConfigType chanH2HTestConfig =
{
   /*                         */
   WLANDXE_CHANNEL_HANDLE_CIRCULA,

   /*                                         */
   5,

   /*                                      */
   0,

   /*                                 */
   /*                        */
   5,

   /*                          */
   0,

   /*               */
   WLANDXE_DESC_CTRL_XTYPE_H2H,

   /*                                                            */
   5,

   /*                                     */
   eWLAN_PAL_FALSE,

   /*                            */
   0,

   /*                              */
   0,

   /*                                              */
   eWLAN_PAL_TRUE,

   /*                      */
   eWLAN_PAL_TRUE
};
#endif /*                             */

WLANDXE_ChannelMappingType channelList[WDTS_CHANNEL_MAX] =
{
   {WDTS_CHANNEL_TX_LOW_PRI,  WLANDXE_DMA_CHANNEL_0, &chanTXLowPriConfig},
   {WDTS_CHANNEL_TX_HIGH_PRI, WLANDXE_DMA_CHANNEL_4, &chanTXHighPriConfig},
   {WDTS_CHANNEL_RX_LOW_PRI,  WLANDXE_DMA_CHANNEL_1, &chanRXLowPriConfig},
#ifndef WLANDXE_TEST_CHANNEL_ENABLE
   {WDTS_CHANNEL_RX_HIGH_PRI, WLANDXE_DMA_CHANNEL_3, &chanRXHighPriConfig},
#else
   {WDTS_CHANNEL_H2H_TEST_TX,    WLANDXE_DMA_CHANNEL_2, &chanH2HTestConfig},
   {WDTS_CHANNEL_H2H_TEST_RX,    WLANDXE_DMA_CHANNEL_2, &chanH2HTestConfig}
#endif /*                             */
};

WLANDXE_TxCompIntConfigType txCompInt = 
{
   /*                                     */
   WLANDXE_TX_COMP_INT_PER_K_FRAMES,

   /*                                                             */
   WLANDXE_TX_LOW_RES_THRESHOLD,

   /*                                                              */
   WLANDXE_TX_LOW_RES_THRESHOLD,

   /*                                              */
   5,

   /*                              */
   /*                      */ 5,

   /*                     */
   10
};

/*                                                                          
                   
                            

                 

               
                                          
                                                                 

           
                

                                                                           */
wpt_status dxeCommonDefaultConfig
(
   WLANDXE_CtrlBlkType     *dxeCtrlBlk
)
{
   wpt_status                  status = eWLAN_PAL_STATUS_SUCCESS;

   dxeCtrlBlk->rxReadyCB     = NULL;
   dxeCtrlBlk->txCompCB      = NULL;
   dxeCtrlBlk->lowResourceCB = NULL;

   wpalMemoryCopy(&dxeCtrlBlk->txCompInt,
                  &txCompInt,
                  sizeof(WLANDXE_TxCompIntConfigType));

   return status;
}

/*                                                                          
                   
                             

                 
                                                                 
                                                        

               
                                          
                                                                 
                                           
                                                             

           
                

                                                                           */
wpt_status dxeChannelDefaultConfig
(
   WLANDXE_CtrlBlkType     *dxeCtrlBlk,
   WLANDXE_ChannelCBType   *channelEntry
)
{
   wpt_status                  status = eWLAN_PAL_STATUS_SUCCESS;
   wpt_uint32                  baseAddress;
   wpt_uint32                  dxeControlRead  = 0;
   wpt_uint32                  dxeControlWrite = 0;
   wpt_uint32                  dxeControlWriteValid = 0;
   wpt_uint32                  dxeControlWriteEop = 0;
   wpt_uint32                  dxeControlWriteEopInt = 0;
   wpt_uint32                  idx;
   wpt_uint32                  rxResourceCount = 0;
   WLANDXE_ChannelMappingType *mappedChannel = NULL;

   /*              */
   if((NULL == dxeCtrlBlk) || (NULL == channelEntry))
   {
      HDXE_MSG(eWLAN_MODULE_DAL_DATA, eWLAN_PAL_TRACE_LEVEL_ERROR,
               "dxeLinkDescAndCtrlBlk Channel Entry is not valid");
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   for(idx = 0; idx < WDTS_CHANNEL_MAX; idx++)
   {
      if(channelEntry->channelType == channelList[idx].wlanChannel)
      {
         mappedChannel = &channelList[idx];
         break;
      }
   }

   if((NULL == mappedChannel) || (WDTS_CHANNEL_MAX == idx))
   {
      HDXE_MSG(eWLAN_MODULE_DAL_DATA, eWLAN_PAL_TRACE_LEVEL_ERROR,
              "%s Failed to map channel", __func__);
      return eWLAN_PAL_STATUS_E_INVAL;
   }

   wpalMemoryCopy(&channelEntry->channelConfig,
                  mappedChannel->channelConfig,
                  sizeof(WLANDXE_ChannelConfigType));

   baseAddress = channelBaseAddressList[mappedChannel->DMAChannel];
   channelEntry->channelRegister.chDXEBaseAddr        = baseAddress;
   channelEntry->channelRegister.chDXEStatusRegAddr   = baseAddress + WLANDXE_DMA_CH_STATUS_REG;
   channelEntry->channelRegister.chDXEDesclRegAddr    = baseAddress + WLANDXE_DMA_CH_DESCL_REG;
   channelEntry->channelRegister.chDXEDeschRegAddr    = baseAddress + WLANDXE_DMA_CH_DESCH_REG;
   channelEntry->channelRegister.chDXELstDesclRegAddr = baseAddress + WLANDXE_DMA_CH_LST_DESCL_REG;
   channelEntry->channelRegister.chDXECtrlRegAddr     = baseAddress + WLANDXE_DMA_CH_CTRL_REG;
   channelEntry->channelRegister.chDXESzRegAddr       = baseAddress + WLANDXE_DMA_CH_SZ_REG;
   channelEntry->channelRegister.chDXEDadrlRegAddr    = baseAddress + WLANDXE_DMA_CH_DADRL_REG;
   channelEntry->channelRegister.chDXEDadrhRegAddr    = baseAddress + WLANDXE_DMA_CH_DADRH_REG;
   channelEntry->channelRegister.chDXESadrlRegAddr    = baseAddress + WLANDXE_DMA_CH_SADRL_REG;
   channelEntry->channelRegister.chDXESadrhRegAddr    = baseAddress + WLANDXE_DMA_CH_SADRH_REG;

   /*              
                                                       
                                                                       
                */
   channelEntry->extraConfig.chan_mask = 0;
   /*                                            */
   channelEntry->extraConfig.chan_mask |= WLANDXE_CH_CTRL_EDVEN_MASK;
   /*                                     */
   channelEntry->extraConfig.chan_mask |= WLANDXE_CH_CTRL_EDEN_MASK;
   /*                                   */
   channelEntry->extraConfig.chan_mask |= WLANDXE_CH_CTRL_INE_ERR_MASK;
   /*                            */
   channelEntry->extraConfig.chan_mask |= WLANDXE_CH_CTRL_INE_DONE_MASK;
   /*                                */
   channelEntry->extraConfig.chan_mask |= WLANDXE_CH_CTRL_INE_ED_MASK;
   /*                                                                */
   channelEntry->extraConfig.chan_mask |= 
                mappedChannel->DMAChannel << WLANDXE_CH_CTRL_CTR_SEL_OFFSET;
   /*               */
   channelEntry->extraConfig.chan_mask |= mappedChannel->channelConfig->xfrType;
   /*                                                      */
   if(!channelEntry->channelConfig.useShortDescFmt)
   {
      channelEntry->extraConfig.chan_mask |= WLANDXE_DESC_CTRL_DFMT;
   }
   /*                                                               */
   if((WDTS_CHANNEL_TX_LOW_PRI  == channelEntry->channelType) ||
      (WDTS_CHANNEL_TX_HIGH_PRI == channelEntry->channelType))
   {
      channelEntry->extraConfig.chan_mask |= WLANDXE_CH_CTRL_DIQ_MASK;
   }
   /*                                                               */
   else if((WDTS_CHANNEL_RX_LOW_PRI  == channelEntry->channelType) ||
           (WDTS_CHANNEL_RX_HIGH_PRI == channelEntry->channelType))
   {
      channelEntry->extraConfig.chan_mask |= WLANDXE_CH_CTRL_SIQ_MASK;
   }
   else
   {
      /*                                                
                    */
   }
   /*                     */
   channelEntry->extraConfig.chan_mask |= WLANDXE_CH_CTRL_SWAP_MASK;
   /*                                 */
   channelEntry->extraConfig.chan_mask |= WLANDXE_CH_CTRL_ENDIAN_MASK;
   /*                      */
   channelEntry->extraConfig.chan_mask |= 
                 channelEntry->channelConfig.bmuThdSel << WLANDXE_CH_CTRL_BTHLD_SEL_OFFSET;
   /*                              */
   channelEntry->extraConfig.chan_mask |= WLANDXE_CH_CTRL_EOP_MASK;
   /*                  */
   channelEntry->extraConfig.chan_mask |= channelEntry->channelConfig.chPriority << WLANDXE_CH_CTRL_PRIO_OFFSET;
   /*         */
   channelEntry->extraConfig.chan_mask |= WLANDXE_DESC_CTRL_PDU_REL;
   /*                                      */
   channelEntry->extraConfig.chan_mask_read_disable = channelEntry->extraConfig.chan_mask;
   /*                                     */
   channelEntry->extraConfig.chan_mask |= WLANDXE_CH_CTRL_EN_MASK;
   /*                   */

   /*             
                                                                       
                                                               */
   dxeControlRead = 0;
   /*                                               */
   dxeControlRead |= WLANDXE_DESC_CTRL_SIQ;
   /*                                  */
   dxeControlRead |= WLANDXE_DESC_CTRL_XTYPE_B2H;
   /*                                      */
   dxeControlRead |= WLANDXE_DESC_CTRL_EOP;
   /*                                                             */
   if(!channelEntry->channelConfig.bdPresent)
   {
      dxeControlRead |= WLANDXE_DESC_CTRL_BDH;
   }
   /*                  */
   dxeControlRead |= channelEntry->channelConfig.chPriority << WLANDXE_CH_CTRL_PRIO_OFFSET;
   /*                                                       */
   dxeControlRead |= channelEntry->channelConfig.bmuThdSel << WLANDXE_CH_CTRL_BTHLD_SEL_OFFSET;
   /*                                           */
   dxeControlRead |= WLANDXE_DESC_CTRL_PDU_REL;
   /*                                                      */
   if(!channelEntry->channelConfig.useShortDescFmt)
   {
      dxeControlRead |= WLANDXE_DESC_CTRL_DFMT;
   }
   /*                              */
   dxeControlRead |= WLANDXE_DESC_CTRL_INT;
   /*                                                   */
   dxeControlRead |= WLANDXE_DESC_CTRL_VALID;
   /*                     */
   dxeControlRead |= WLANDXE_DESC_CTRL_BDT_SWAP;
   /*                    */
   if((WDTS_CHANNEL_TX_LOW_PRI  == channelEntry->channelType) ||
      (WDTS_CHANNEL_TX_HIGH_PRI == channelEntry->channelType))
   {
      dxeControlRead |= WLANDXE_DESC_CTRL_ENDIANNESS;
   }

   /*                */
   channelEntry->extraConfig.cw_ctrl_read = WLANDXE_U32_SWAP_ENDIAN(dxeControlRead);
   /*                   */

   /*              
                                                        
                                                                    */
   channelEntry->extraConfig.cw_ctrl_write = 0;
   /*                                */
   dxeControlWrite |= mappedChannel->channelConfig->xfrType;
   /*                                       */
   if(!channelEntry->channelConfig.bdPresent)
   {
      dxeControlWrite |= WLANDXE_DESC_CTRL_BDH;
   }
   /*                  */
   dxeControlWrite |= channelEntry->channelConfig.chPriority << WLANDXE_CH_CTRL_PRIO_OFFSET;
   /*                                                      */
   if(!channelEntry->channelConfig.useShortDescFmt)
   {
      dxeControlWrite |= WLANDXE_DESC_CTRL_DFMT;
   }
   /*                                                       */
   dxeControlWrite |= channelEntry->channelConfig.bmuThdSel << WLANDXE_CH_CTRL_BTHLD_SEL_OFFSET;
   /*                   */
   dxeControlWrite |= WLANDXE_DESC_CTRL_DIQ;
   /*                     */
   dxeControlWrite |= WLANDXE_DESC_CTRL_BDT_SWAP;
   /*                    */
   dxeControlWrite |= WLANDXE_DESC_CTRL_ENDIANNESS;
   /*                  */
   dxeControlWrite |= WLANDXE_DESC_CTRL_INT;

   dxeControlWriteValid  = dxeControlWrite | WLANDXE_DESC_CTRL_VALID;
   dxeControlWriteEop    = dxeControlWriteValid | WLANDXE_DESC_CTRL_EOP;
   dxeControlWriteEopInt = dxeControlWriteEop | WLANDXE_DESC_CTRL_INT;

   /*                                              */
   channelEntry->extraConfig.cw_ctrl_write = WLANDXE_U32_SWAP_ENDIAN(dxeControlWrite);
   /*                    */

   /*                              
                                                 
                                                   */
   channelEntry->extraConfig.cw_ctrl_write_valid =
                  WLANDXE_U32_SWAP_ENDIAN(dxeControlWriteValid);

   /*                          
                    */
   channelEntry->extraConfig.cw_ctrl_write_eop =
                  WLANDXE_U32_SWAP_ENDIAN(dxeControlWriteEop);

   /*                                  
                                                                       */
   channelEntry->extraConfig.cw_ctrl_write_eop_int =
                  WLANDXE_U32_SWAP_ENDIAN(dxeControlWriteEopInt);


   /*               */
   channelEntry->extraConfig.chk_size_mask = 
            mappedChannel->channelConfig->chk_size << 10;

   channelEntry->extraConfig.refWQ_swapped = 
                WLANDXE_U32_SWAP_ENDIAN(channelEntry->channelConfig.refWQ);

   /*                                     */
   channelEntry->extraConfig.intMask = channelInterruptMask[mappedChannel->DMAChannel];


   wpalGetNumRxRawPacket(&rxResourceCount);
   if((WDTS_CHANNEL_TX_LOW_PRI == channelEntry->channelType) ||
      (0 == rxResourceCount))
   {
      channelEntry->numDesc         = mappedChannel->channelConfig->nDescs;
   }
   else
   {
      channelEntry->numDesc         = rxResourceCount / 4;
   }
   channelEntry->assignedDMAChannel = mappedChannel->DMAChannel;
   channelEntry->numFreeDesc             = 0;
   channelEntry->numRsvdDesc             = 0;
   channelEntry->numFragmentCurrentChain = 0;
   channelEntry->numTotalFrame           = 0;
   channelEntry->hitLowResource          = eWLAN_PAL_FALSE;

   return status;
}

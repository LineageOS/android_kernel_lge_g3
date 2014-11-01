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

#if !defined( __SAP_CH_SELECT_H )
#define __SAP_CH_SELECT_H

/*
* Copyright (c) 2012-2013 Qualcomm Atheros, Inc.
* All Rights Reserved.
* Qualcomm Atheros Confidential and Proprietary.
*/
/*===========================================================================

                      s a p C h S e l e c t . h
                                               
  OVERVIEW:
  
  This software unit holds the implementation of the WLAN SAP modules
  functions for channel selection.

  DEPENDENCIES:   
  

  Are listed for each API below. 
  
  
  Copyright (c) 2010 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                



                                             
                                                                                  
                                          

                                                                           */


/*                                                                          
               
                                                                        */
#include "aniGlobal.h"
/*                                                                          
                  
                                                                          */

#define SPECT_24GHZ_CH_COUNT    (11) //                      
#define SAPDFS_NORMALISE_1000      (1000/9) //                                     
/*             
                                     
                                     
*/
#define SOFTAP_MIN_RSSI         (-100)
#define SOFTAP_MAX_RSSI         (0)
#define SOFTAP_MIN_COUNT        (0)
#define SOFTAP_MAX_COUNT        (60)
#define SOFTAP_RSSI_WEIGHT      (20)
#define SOFTAP_COUNT_WEIGHT     (20)

#define SAP_DEFAULT_CHANNEL     (6)
#define SAP_DEFAULT_5GHZ_CHANNEL      (40)
#define SAP_CHANNEL_NOT_SELECTED (0)

#define SOFTAP_HT20_CHANNELWIDTH 0
#define SAP_SUBBAND1_RSSI_EFFECT_PRIMARY  (-20) //                                                          
#define SAP_SUBBAND2_RSSI_EFFECT_PRIMARY  (-30) //                                                     
#define SAP_SUBBAND3_RSSI_EFFECT_PRIMARY  (-40) //                                                     

#define SAP_24GHZ_FIRST_OVERLAP_CHAN_RSSI_EFFECT_PRIMARY      (-10) //                                                                        
#define SAP_24GHZ_SEC_OVERLAP_CHAN_RSSI_EFFECT_PRIMARY        (-20) //                                                                         
#define SAP_24GHZ_THIRD_OVERLAP_CHAN_RSSI_EFFECT_PRIMARY      (-30) //                                                                        
#define SAP_24GHZ_FOURTH_OVERLAP_CHAN_RSSI_EFFECT_PRIMARY     (-40) //                                                                         

typedef enum
{
    CHANNEL_1 = 1,
    CHANNEL_2,
    CHANNEL_3,
    CHANNEL_4,
    CHANNEL_5,
    CHANNEL_6,
    CHANNEL_7,
    CHANNEL_8,
    CHANNEL_9,
    CHANNEL_10,
    CHANNEL_11
} tSapChannel;

/* 
                                                                 
                                                     
*/

typedef struct sSapChannelListInfo{
    v_U8_t numChannel;
    v_U8_t *channelList;
} tSapChannelListInfo;

typedef struct {
    v_U16_t chNum;      //               
    v_U16_t channelWidth;      //              
    v_U16_t bssCount;   //                                         
    v_S31_t rssiAgr;    //                                                                     
    v_U32_t weight;     //                          
    v_BOOL_t valid;     //                                                       
} tSapSpectChInfo;//                

/* 
                                                           
                                                              
*/

typedef struct {
    tSapSpectChInfo *pSpectCh;//                                                                              
    v_U8_t numSpectChans;      //                                      
} tSapChSelSpectInfo;//                

/* 
                                                      
 */
typedef struct sSapChSelParams {
    void *pSpectInfoParams;//                                                
    v_U16_t numChannels;
} tSapChSelParams;

#endif //                              


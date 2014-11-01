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
* Copyright (c) 2012-2013 Qualcomm Atheros, Inc.
* All Rights Reserved.
* Qualcomm Atheros Confidential and Proprietary.
*/

/**
 *
 *  @file:         wlan_qct_dev_defs.h
 *
 *  @brief:        This file contains the hardware related definitions.
 *
 *  Copyright (C)  2008, Qualcomm Technologies, Inc. All rights reserved.
 */

#ifndef __WLAN_QCT_DEV_DEFS_H
#define __WLAN_QCT_DEV_DEFS_H


/*                                                                     
                               
                                                                       
 */

#ifdef WCN_PRONTO

#ifdef WLAN_SOFTAP_VSTA_FEATURE
//                       
#define HAL_NUM_ASSOC_STA           32 //                                                           
#define HAL_NUM_STA                 41
#define HAL_NUM_HW_STA              16

#define HAL_NUM_GPSTA               4
#define HAL_NUM_UMA_DESC_ENTRIES    HAL_NUM_HW_STA //               

#define HAL_NUM_BSSID               2
#define HAL_NUM_STA_WITHOUT_VSTA    12
#define HAL_NUM_STA_INCLUDING_VSTA  32

#define HAL_NUM_VSTA                (HAL_NUM_STA - HAL_NUM_HW_STA)
#define QWLANFW_MAX_NUM_VSTA        (HAL_NUM_VSTA)
#define QWLANFW_VSTA_INVALID_IDX    (HAL_NUM_STA+1)
#define QWLAN_VSTA_MIN_IDX          (HAL_NUM_HW_STA)
#define QWLANFW_NUM_GPSTA           (HAL_NUM_GPSTA)

//           
#define HAL_NUM_STA_WITHOUT_VSTA_PRONTO_V1 9
#define HAL_NUM_STA_WITHOUT_VSTA_PRONTO_V2 (HAL_NUM_STA_WITHOUT_VSTA)

#define IS_VSTA_VALID_IDX(__x) \
                          ((__x) != QWLANFW_VSTA_INVALID_IDX)

#define IS_VSTA_IDX(__x) \
                   (((__x) >= QWLAN_VSTA_MIN_IDX) && ((__x) < HAL_NUM_STA))

#define GET_VSTA_INDEX_FOR_STA_INDEX(__idx)    ((__idx) - QWLAN_VSTA_MIN_IDX)

//                                  
#define IS_GPSTA_IDX(__x) \
    (((__x) >= (HAL_NUM_HW_STA-HAL_NUM_GPSTA)) && \
     ((__x) < HAL_NUM_HW_STA))

//                                        
#define IS_HWSTA_IDX(__x) \
    ((__x) < (HAL_NUM_HW_STA-HAL_NUM_GPSTA))

#define HAL_NUM_STA_INCLUDING_VSTA  32

#elif WCN_PRONTO_V1

/*                                                                 
                                                                   
                                                                    
                      
  
                                                 
                                                                      
                  
  
                                                                          
                                                                      
                                                                  
 */
#define HAL_NUM_HW_STA              9
#define HAL_NUM_STA                 (HAL_NUM_HW_STA)
#define HAL_NUM_BSSID               2
#define HAL_NUM_UMA_DESC_ENTRIES    9
#define HAL_NUM_ASSOC_STA           7


#else /*               */

#define HAL_NUM_HW_STA              14
#define HAL_NUM_STA                 (HAL_NUM_HW_STA)
#define HAL_NUM_BSSID               4
#define HAL_NUM_UMA_DESC_ENTRIES    14
#define HAL_NUM_ASSOC_STA           12


#endif /*                                           */
#else  /*            */

/*
                                        
  
                                                                 
                            
                         
                                
  
                                                      
                            
                         
                                                               
                                             
  
                                                                                      
                                 
                                                                             
                            
 */
#ifdef WLAN_SOFTAP_VSTA_FEATURE
#define HAL_NUM_ASSOC_STA           32
#define HAL_NUM_STA                 41
#define HAL_NUM_HW_STA              16
#define HAL_NUM_GPSTA               4
#define HAL_NUM_VSTA                (HAL_NUM_STA - HAL_NUM_HW_STA)

#define QWLANFW_MAX_NUM_VSTA        HAL_NUM_VSTA
#define QWLANFW_VSTA_INVALID_IDX    (HAL_NUM_STA+1)
#define QWLAN_VSTA_MIN_IDX          HAL_NUM_HW_STA
#define QWLANFW_NUM_GPSTA           HAL_NUM_GPSTA


#define IS_VSTA_VALID_IDX(__x) \
                          ((__x) != QWLANFW_VSTA_INVALID_IDX)

#define IS_VSTA_IDX(__x) \
                   (((__x) >= QWLAN_VSTA_MIN_IDX) && ((__x) < HAL_NUM_STA))

#define GET_VSTA_INDEX_FOR_STA_INDEX(__idx)    ((__idx) - QWLAN_VSTA_MIN_IDX)

//                                  
#define IS_GPSTA_IDX(__x) \
    (((__x) >= (HAL_NUM_HW_STA-HAL_NUM_GPSTA)) && \
     ((__x) < HAL_NUM_HW_STA))

//                                        
#define IS_HWSTA_IDX(__x) \
    ((__x) < (HAL_NUM_HW_STA-HAL_NUM_GPSTA))

#define HAL_NUM_STA_INCLUDING_VSTA  32
#define HAL_NUM_STA_WITHOUT_VSTA    12

#else
#define HAL_NUM_STA                 12
#define HAL_NUM_ASSOC_STA           10
#define HAL_NUM_HW_STA              12
#endif

#define HAL_NUM_BSSID               2
#define HAL_NUM_UMA_DESC_ENTRIES    HAL_NUM_HW_STA

#endif /*            */

#ifdef FEATURE_WLAN_TDLS
#define CXM_TDLS_MAX_NUM_STA            32
#endif

#define HAL_INVALID_BSSIDX          HAL_NUM_BSSID

#define MAX_NUM_OF_BACKOFFS         8
#define HAL_MAX_ASSOC_ID            HAL_NUM_STA

#define WLANHAL_TX_BD_HEADER_SIZE   40  //                     
#define WLANHAL_RX_BD_HEADER_SIZE   76  

/*
                              
                                       
                                                
                                 
                                     
                                  
                                 
                                  
 */

#define HAL_ENC_POLICY_NULL        0
#define HAL_ENC_POLICY_WEP40       1
#define HAL_ENC_POLICY_WEP104      2
#define HAL_ENC_POLICY_TKIP        3
#define HAL_ENC_POLICY_AES_CCM     4

/*                                                                       */
/*     */
/*                                                                       */

/*
                                                                            
  
 */

typedef enum sBmuWqId {

    /*                          */

    /*     */
    BMUWQ_BMU_IDLE_BD = 0,
    BMUWQ_BMU_IDLE_PDU = 1,

    /*     */
    BMUWQ_RXP_UNKNWON_ADDR = 2,  /*                            */

    /*        */
    BMUWQ_DPU_RX = 3,

    /*        */
    BMUWQ_DPU_TX = 6,

    /*          */
    BMUWQ_FW_TRANSMIT = 12,  /*               */
    BMUWQ_FW_RECV = 7,       /*               */

    BMUWQ_FW_RPE_RECV = 16,   /*                   */
    FW_SCO_WQ = BMUWQ_FW_RPE_RECV,

    /*           */
    BMUWQ_DPU_ERROR_WQ = 8,

    /*        */
    BMUWQ_DXE_RX = 11,

    BMUWQ_DXE_RX_HI = 4,

    /*         */
    BMUWQ_ADU_UMA_TX = 23,
    BMUWQ_ADU_UMA_RX = 24,

    /*          */
    BMUWQ_BTQM = 25,

    /*                                                                */
    BMUWQ_SINK = 255,

#ifdef WCN_PRONTO
    BMUWQ_BMU_CMEM_IDLE_BD = 27,
    /*                              */
    BMUWQ_NUM = 28,
    
    //                                                                         
    BMUWQ_NOT_SUPPORTED_MASK = 0x0,
#else
    /*                             */
    BMUWQ_NUM = 27,

    //                                                 
    BMUWQ_NOT_SUPPORTED_MASK = 0x7e0000,
#endif //          


    /*         */
    BMUWQ_BTQM_TX_MGMT = BMUWQ_BTQM,
    BMUWQ_BTQM_TX_DATA = BMUWQ_BTQM,
    BMUWQ_BMU_WQ2 = BMUWQ_RXP_UNKNWON_ADDR,
    BMUWQ_FW_DPU_TX = 5,

    //                                                                         
    BMUWQ_FW_RECV_EXCEPTION = 14, //                                           

    //                                                                                                     
    //                           
    BMUWQ_HOST_RX_UNKNOWN_ADDR2_FRAMES = 15, //                                         

    /*                                  */

    /*                  */
    BMUWQ_ADU_UMA_TX_ERROR_WQ = 13, /*                   */
    BMUWQ_ADU_UMA_RX_ERROR_WQ = 10, /*                   */

    /*               */
    BMUWQ_DPU_ERROR_WQ2 = 9, /*                   */

    /*        */
    //                                                                                              
    //                                                           
    //                                                                                       
    BMUWQ_FW_DXECH2_1 = 16,  /*                                                          */

    /*                       */
    BMUWQ_RXP_DEFAULT_PUSH_WQ = 17,
/*                                       
                        
                        
                        
                        
                        
                       
*/
} tBmuWqId;

typedef enum
{
    BTQM_QID0 = 0,
    BTQM_QID1,
    BTQM_QID2,
    BTQM_QID3,
    BTQM_QID4,
    BTQM_QID5,
    BTQM_QID6,
    BTQM_QID7,
    BTQM_QID8,
    BTQM_QID9,
    BTQM_QID10,

    BTQM_QUEUE_TX_TID_0 = BTQM_QID0,
    BTQM_QUEUE_TX_TID_1,
    BTQM_QUEUE_TX_TID_2,
    BTQM_QUEUE_TX_TID_3,
    BTQM_QUEUE_TX_TID_4,
    BTQM_QUEUE_TX_TID_5,
    BTQM_QUEUE_TX_TID_6,
    BTQM_QUEUE_TX_TID_7,


    /*                 
       */
    BTQM_QUEUE_TX_nQOS = BTQM_QID8,
    BTQM_QUEUE_SELF_STA_BCAST_MGMT = BTQM_QID10,    
    BTQM_QUEUE_SELF_STA_UCAST_MGMT = BTQM_QID9,
    BTQM_QUEUE_SELF_STA_UCAST_DATA = BTQM_QID9,
    BTQM_QUEUE_NULL_FRAME          = BTQM_QID9,      
    BTQM_QUEUE_SELF_STA_PROBE_RSP =  BTQM_QID9,
    BTQM_QUEUE_TX_AC_BE = BTQM_QUEUE_TX_TID_0,
    BTQM_QUEUE_TX_AC_BK = BTQM_QUEUE_TX_TID_2,
    BTQM_QUEUE_TX_AC_VI = BTQM_QUEUE_TX_TID_4,
    BTQM_QUEUE_TX_AC_VO = BTQM_QUEUE_TX_TID_6
}tBtqmQId;

#define STACFG_MAX_TC   8

/*                                                                       */
/*         */
/*                                                                       */
#define HWBD_TYPE_GENERIC                  0   /*                   */
#define HWBD_TYPE_FRAG                     1   /*                        */

/*                                                                       */
/*                                                                       */
/*                                                                       */
#ifdef WLAN_HAL_PRIMA
   #define WLAN_SOC_PRIMA_MAX_TX_POWER 22
   #define WLAN_SOC_PRIMA_MIN_TX_POWER 6
#else
   /*                         */
   #define WLAN_SOC_PRIMA_MAX_TX_POWER 22
   #define WLAN_SOC_PRIMA_MIN_TX_POWER 6
#endif //                

#endif /*                       */

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


/** ------------------------------------------------------------------------- *
    ------------------------------------------------------------------------- *


    \file halTypes.h

    \brief This header captures types that must be shared in common with individual
            module headers before inclusion into halCommonApi.h.

    $Id$

    Copyright (C) 2006 Airgo Networks, Incorporated


   ========================================================================== */

#ifndef HALTYPES_H
#define HALTYPES_H
#ifndef WINXP_APPS_BUILD    //                                                         

#endif /*                  */

#include "palTypes.h"
#include "wlan_nv.h"

#define OFFSET_OF(structType,fldName)   (&((structType*)0)->fldName)

/*                                                                             

                       

                                                                             
                                   

                                                                               */
typedef void *tHalHandle;

//                                              
#define HAL_INVALID_HAL_HANDLE ( NULL )


/*                                                                             

                    

                                                                       
                            

                                                                               */
typedef enum
{
    eHAL_STATUS_SUCCESS,

    //                                                                          
    //                               
    eHAL_STATUS_FAILURE,
    eHAL_STATUS_FAILED_ALLOC,
    eHAL_STATUS_RESOURCES,

    //                                                                   
    eHAL_STATUS_NOT_OPEN,

    //                                                 
    eHAL_STATUS_CARD_NOT_PRESENT,

    //                   
    eHAL_STATUS_INTERRUPT_ENABLED,
    eHAL_STATUS_INTERRUPT_DISABLED,
    eHAL_STATUS_NO_INTERRUPTS,
    eHAL_STATUS_INTERRUPT_PRESENT,
    eHAL_STATUS_ALL_INTERRUPTS_PROCESSED,
    eHAL_STATUS_INTERRUPT_NOT_PROCESSED,        //                                       

    //                                                   
    eHAL_STATUS_INVALID_PARAMETER,

    //                                    
    eHAL_STATUS_NOT_INITIALIZED,

    //                                   
    eHAL_STATUS_INVALID_STAIDX,
    eHAL_STATUS_INVALID_BSSIDX,
    eHAL_STATUS_STA_TABLE_FULL,             //                                          
    eHAL_STATUS_BSSID_TABLE_FULL,
    eHAL_STATUS_DUPLICATE_BSSID,
    eHAL_STATUS_DUPLICATE_STA,
    eHAL_STATUS_BSSID_INVALID,
    eHAL_STATUS_STA_INVALID,
    eHAL_STATUS_INVALID_KEYID,
    eHAL_STATUS_INVALID_SIGNATURE,

    //   
    eHAL_STATUS_DXE_FAILED_NO_DESCS,
    eHAL_STATUS_DXE_CHANNEL_NOT_CONFIG,         //                       
    eHAL_STATUS_DXE_CHANNEL_MISUSE,             //                                                  
    eHAL_STATUS_DXE_VIRTUAL_MEM_ALLOC_ERROR,    //
    eHAL_STATUS_DXE_SHARED_MEM_ALLOC_ERROR,     //
    eHAL_STATUS_DXE_INVALID_CHANNEL,
    eHAL_STATUS_DXE_INVALID_CALLBACK,
    eHAL_STATUS_DXE_INCONSISTENT_DESC_COUNT,
    eHAL_STATUS_DXE_XFR_QUEUE_ERROR,
    eHAL_STATUS_DXE_INVALID_BUFFER,
    eHAL_STATUS_DXE_INCOMPLETE_PACKET,
    eHAL_STATUS_DXE_INVALID_PARAMETER,
    eHAL_STATUS_DXE_CH_ALREADY_CONFIGURED,
    eHAL_STATUS_DXE_USB_INVALID_EP,
    eHAL_STATUS_DXE_GEN_ERROR,


    //                                                 
    eHAL_STATUS_E_NULL_VALUE,
    eHAL_STATUS_E_FILE_NOT_FOUND,
    eHAL_STATUS_E_FILE_INVALID_CONTENT,
    eHAL_STATUS_E_MALLOC_FAILED,
    eHAL_STATUS_E_FILE_READ_FAILED,
    eHAL_STATUS_E_IMAGE_INVALID,
    eHAL_STATUS_E_IMAGE_UNSUPPORTED,

    //                                                           
    //                       
    eHAL_STATUS_DEVICE_MEMORY_MISALIGNED,          //                                                  
    eHAL_STATUS_DEVICE_MEMORY_LENGTH_ERROR,        //                                           

    //                                                    
    eHAL_STATUS_NET_CONGESTION,

    //                                                                               
    //                                                                            
    //                                                                            
    //                       
    eHAL_STATUS_RX_PACKET_DROPPED,
    eHAL_STATUS_RX_PACKET_DROPPED_MIN = eHAL_STATUS_RX_PACKET_DROPPED,
    eHAL_STATUS_RX_PACKET_DROPPED_NULL_DATA,
    eHAL_STATUS_RX_PACKET_DROPPED_WDS_FRAME,
    eHAL_STATUS_RX_PACKET_DROPPED_FILTERED,
    eHAL_STATUS_RX_PACKET_DROPPED_GROUP_FROM_SELF,
    eHAL_STATUS_RX_PACKET_DROPPED_MAX = eHAL_STATUS_RX_PACKET_DROPPED_GROUP_FROM_SELF,

    //                                                                                                        
    eHAL_STATUS_PMU_NOT_POWERED_UP,

    //                                
    eHAL_STATUS_BA_ENQUEUED,        //                                   
    eHAL_STATUS_BA_INVALID,

    //                              
    eHAL_STATUS_BA_RX_BUFFERS_FULL,
    eHAL_STATUS_BA_RX_MAX_SESSIONS_REACHED,
    eHAL_STATUS_BA_RX_INVALID_SESSION_ID,

    //                                                        
    eHAL_STATUS_BA_RX_DROP_FRAME,
    eHAL_STATUS_BA_RX_INDICATE_FRAME,
    eHAL_STATUS_BA_RX_ENQUEUE_FRAME,

    //                  
    eHAL_STATUS_PMC_PENDING,
    eHAL_STATUS_PMC_DISABLED,
    eHAL_STATUS_PMC_NOT_NOW,
    eHAL_STATUS_PMC_AC_POWER,
    eHAL_STATUS_PMC_SYS_ERROR,
    eHAL_STATUS_PMC_CANNOT_ENTER_IMPS,
    eHAL_STATUS_PMC_ALREADY_IN_IMPS,

    eHAL_STATUS_HEARTBEAT_TMOUT,
    eHAL_STATUS_NTH_BEACON_DELIVERY,

    //   
    eHAL_STATUS_CSR_WRONG_STATE,

    //    
    eHAL_STATUS_DPU_DESCRIPTOR_TABLE_FULL,
    eHAL_STATUS_DPU_MICKEY_TABLE_FULL,

    //                
    eHAL_STATUS_FW_MSG_FAILURE,                //                                  
    eHAL_STATUS_FW_MSG_TIMEDOUT,
    eHAL_STATUS_FW_MSG_INVALID,
    eHAL_STATUS_FW_SEND_MSG_FAILED,
    eHAL_STATUS_FW_PS_BUSY,

    eHAL_STATUS_TIMER_START_FAILED,
    eHAL_STATUS_TIMER_STOP_FAILED,

    eHAL_STATUS_TL_SUSPEND_TIMEOUT,

    eHAL_STATUS_UMA_DESCRIPTOR_TABLE_FULL,

    eHAL_STATUS_SET_CHAN_ALREADY_ON_REQUESTED_CHAN,

#ifdef WLAN_FEATURE_VOWIFI_11R
    eHAL_STATUS_FT_PREAUTH_KEY_SUCCESS,
    eHAL_STATUS_FT_PREAUTH_KEY_FAILED,
#endif

    //                                                                
    eHAL_STATUS_MAX

} eHalStatus;

typedef enum
{
    HAL_STOP_TYPE_SYS_RESET,
    HAL_STOP_TYPE_SYS_DEEP_SLEEP,
    HAL_STOP_TYPE_RF_KILL,
}tHalStopType;

//                                                  
#define HAL_STATUS_SUCCESS( variable ) ( eHAL_STATUS_SUCCESS == ( variable ) )

//                          
typedef enum sHalBitVal  //                   
{
    eHAL_CLEAR,
    eHAL_SET
}tHalBitVal;

//                                                              
//          
enum {
   eHI_PRI,
   ePROT,
   eDBG
};

//                                     
typedef enum eBssSystemRole
{
    eSYSTEM_UNKNOWN_ROLE,
    eSYSTEM_AP_ROLE,
    eSYSTEM_STA_IN_IBSS_ROLE,
    eSYSTEM_STA_ROLE,
    eSYSTEM_BTAMP_STA_ROLE,
    eSYSTEM_BTAMP_AP_ROLE,

    eSYSTEM_LAST_ROLE,
    eSYSTEM_MULTI_BSS_ROLE = eSYSTEM_LAST_ROLE
} tBssSystemRole;


//                                        
//                                       
//                                        
typedef enum sHalCBsidebandType
{
    eHAL_SIDEBAND_CENTER=0,
    eHAL_SIDEBAND_LOWER,
    eHAL_SIDEBAND_UPPER,
    eHAL_SIDEBAND_COPY
}tHalCBsidebandType;


//            
typedef enum {
    eHAL_IDLE,
    eHAL_INIT,
    eHAL_CFG, //                       
    eHAL_STARTED, //                               
    eHAL_SYS_READY, //                                
    eHAL_NORMAL, //                                                                   
} tHAL_STATE;




//                                               
typedef enum
{
    //                                           
    eHAL_SYS_MODE_NORMAL = 0,
    eHAL_SYS_MODE_LEARN,
    eHAL_SYS_MODE_SCAN,
    eHAL_SYS_MODE_PROMISC,
    eHAL_SYS_MODE_SUSPEND_LINK,
    eHAL_SYS_MODE_ROAM_SCAN,
    eHAL_SYS_MODE_ROAM_SUSPEND_LINK,
} eHalSysMode;




//                                                
//                            
typedef enum {

    HAL_TXRX_FRM_RAW,
    HAL_TXRX_FRM_ETH2,
    HAL_TXRX_FRM_802_3,
    HAL_TXRX_FRM_802_11_MGMT,
    HAL_TXRX_FRM_802_11_CTRL,
    HAL_TXRX_FRM_802_11_DATA,
    HAL_TXRX_FRM_IGNORED,   //                          
    HAL_TXRX_FRM_MAX

} eFrameType;


typedef enum
{
    ANI_TXDIR_IBSS = 0,
    ANI_TXDIR_TODS,
    ANI_TXDIR_FROMDS,
    ANI_TXDIR_WDS

} eFrameTxDir;

typedef enum
{
    eRF_BAND_UNKNOWN = 0,
    eRF_BAND_2_4_GHZ = 1,
    eRF_BAND_5_GHZ = 2
} eRfBandMode;


#ifndef __offsetof
#define __offsetof(type, field) ((tANI_U32)(&((type *)0)->field))
#endif

#ifndef offsetof
#define offsetof(type, field) __offsetof(type, field)
#endif

#define HAL_MAX_TXPOWER_INVALID       127

#define MIN_STA_PWR_CAP_DBM         13
#define MAX_STA_PWR_CAP_DBM         19 

/*                                                                          
                */
/*                                                                     
                                 
                                                                       
 */

/*
                                         
  
                                                                  
                            
                         
                               
  
                                                        
                            
                         
                                                               
                                           
 */

#define HAL_INVALID_BSSIDX          (HAL_NUM_BSSID + 1)
#define HAL_NUM_UMA_DESC_ENTRIES    8
#define MAX_NUM_OF_BACKOFFS         8

#define IS_VALID_BSSIDX(__x) \
                        ((__x) < HAL_NUM_BSSID)

#ifdef WLAN_SOFTAP_VSTA_FEATURE
#define HAL_NUM_ASSOC_STA           32
#define HAL_NUM_STA                 41
#define HAL_NUM_HW_STA              16
#define HAL_NUM_GPSTA               4
#define HAL_NUM_VSTA                (HAL_NUM_STA - HAL_NUM_HW_STA)

#define QWLANFW_MAX_NUM_VSTA        HAL_NUM_VSTA
#define QWLANFW_VSTA_INVALID_IDX    (HAL_NUM_STA + 1)
#define QWLAN_VSTA_MIN_IDX          HAL_NUM_HW_STA
#define QWLANFW_NUM_GPSTA           HAL_NUM_GPSTA


#define IS_VSTA_VALID_IDX(__x) \
                          ((__x) != QWLANFW_VSTA_INVALID_IDX)

#define IS_VSTA_IDX(__x) \
                   (((__x) >= QWLAN_VSTA_MIN_IDX) && ((__x) < HAL_NUM_STA))

                 
//                                  
#define IS_GPSTA_IDX(__x) \
    (((__x) >= (HAL_NUM_HW_STA-HAL_NUM_GPSTA)) && \
     ((__x) < HAL_NUM_HW_STA))

//                                        
#define IS_HWSTA_IDX(__x) \
    ((__x) < (HAL_NUM_HW_STA-HAL_NUM_GPSTA))

#else
/*                                                                  
                                                                         
                                                         */
#define HAL_NUM_STA                 12
#define HAL_NUM_ASSOC_STA           10
#define HAL_NUM_HW_STA              12
#endif

/*
                              
                                       
                                                
                                 
                                     
                                  
                                 
                                  
 */

#define HAL_ENC_POLICY_NULL        0
#define HAL_ENC_POLICY_WEP40       1
#define HAL_ENC_POLICY_WEP104      2
#define HAL_ENC_POLICY_TKIP        3
#define HAL_ENC_POLICY_AES_CCM     4

#define STACFG_MAX_TC   8


#endif


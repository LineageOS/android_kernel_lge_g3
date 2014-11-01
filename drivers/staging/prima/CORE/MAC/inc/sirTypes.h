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

/*
                                                 
  
                            
                        
            
                                                                  
                                                                       
 */

#ifndef __SIR_TYPES_H
#define __SIR_TYPES_H

#include "halTypes.h"

/*                                                 
                                                   
                                                   
                                                   
                                                  */

//                    
typedef enum eSirRetStatus
{
    eSIR_SUCCESS,
    eSIR_FAILURE,

    //               
    eSIR_SYS_ERROR_BASE=0x100,
    eSIR_SYS_TX_THREAD_CREATE_FAILED,
    eSIR_SYS_TX_THREAD_RESUME_FAILED,
    eSIR_SYS_TX_MSG_Q_CREATE_FAILED,
    eSIR_SYS_TX_Q_SEND_FAILED,
    eSIR_SYS_TX_Q_RECV_FAILED,
    eSIR_SYS_TX_TIMER_ACTIVATE_FAILED,
    eSIR_SYS_TX_TIMER_CHANGE_FAILED,
    eSIR_SYS_TX_TIMER_CREATE_FAILED,
    eSIR_MEM_ALLOC_FAILED,
    eSIR_PCI_ERROR,

    //                    
    eSIR_HAL_ERROR_BASE=0x1000,
    eSIR_HAL_STAID_INVALID,                   //  
    eSIR_HAL_TCDESC_INVALID,                  //  
    eSIR_HAL_TX_WQ_NOT_VALID,                 //  
    eSIR_HAL_PREV_BMU_CMD_INCOMPLETE,         //  
    eSIR_HAL_EEPROM_CRC_FAILED,               //  
    eSIR_HAL_PCI_REVID_INVALID,               //  
    eSIR_HAL_STA_TC_ID_INVALID,               //  
    eSIR_HAL_TXWQ_EMPTY,                      //  
    eSIR_HAL_ROUT_TBL_TYPE_STYPE_INVALID,     //  
    eSIR_HAL_TFP_ENABLE_FAILED,               //  
    eSIR_HAL_TFP_ABORT_CMD_FAILED,            //  
    eSIR_HAL_TFP_TEMPL_BCNLEN_INVALID,        //  
    eSIR_HAL_TFP_TEMPL_SCHLEN_INVALID,        //  
    eSIR_HAL_TFP_TEMPL_CFENDLEN_INVALID,      //  
    eSIR_HAL_TFP_TEMPL_RRLEN_INVALID,         //  
    eSIR_HAL_TFP_TEMPL_PSPOLLLEN_INVALID,     //   
    eSIR_HAL_TFP_TEMPL_CTSLEN_INVALID,        //   
    eSIR_HAL_TFP_TEMPL_CFPOLLLEN_INVALID,     //   
    eSIR_HAL_TFP_TEMPL_BACKLEN_INVALID,       //   
    eSIR_HAL_INPUT_INVALID,                   //   
    eSIR_HAL_GET_PDU_FAILED,                  //   
    eSIR_HAL_ADD_STA_ACK_POLICY_INVALID,      //   
    eSIR_HAL_STA_EXISTS,                      //   
    eSIR_HAL_STA_DOES_NOT_EXIST,              //   
    eSIR_HAL_MASTER_WQ_ID_INVALID,            //   
    eSIR_HAL_WQ_NOT_EMPTY,                    //   
    eSIR_HAL_WQ_EMPTY,                        //   
    eSIR_HAL_PDUCNT_AND_NEXTPTR_MISMATCH,     //   
    eSIR_HAL_ERR_NUM_BYTES_TO_BE_SET_TOO_BIG, //   
    eSIR_HAL_GET_PKT_LENGTH_INVALID,          //   
    eSIR_HAL_AS_CNT_INVALID,                  //   
    eSIR_HAL_RFP_AGE_CMD_SEQFAIL,             //   
    eSIR_HAL_RFP_AGE_CMD_AGE_CMD_TCFAIL,      //   
    eSIR_HAL_RFP_AGE_CMD_PASS,                //   
    eSIR_HAL_RFP_AGE_CMD_TIMEDOUT,            //   
    eSIR_HAL_RHP_HASH_CMD_TIMEOUT,            //   
    eSIR_HAL_RHP_ROUTING_TBL_SET_FAILED,      //   
    eSIR_HAL_RHP_ROUTING_TBL_GET_FAILED,      //   

    eSIR_HAL_CAL_STATUS_CHK_FAILED,

    eSIR_HAL_SYS_ARM_DBG_MODE_SET_FAILED,
    eSIR_HAL_TFP_BCN_SENT,
    eSIR_HAL_TFP_BCN_NOT_SENT,
    eSIR_HAL_TFP_BKOF_ID_INVALID,
    eSIR_HAL_TFP_CFB_ENABLE_INPUT_INVALID,
    eSIR_HAL_TFP_EDCF_TXOP_INVALID,
    eSIR_HAL_TFP_TEMPL_LEN_INVALID,
    eSIR_HAL_KEY_ID_INVALID,
    eSIR_HAL_KEY_LEN_INVALID,
    eSIR_HAL_CHID_INVALID,
    eSIR_HAL_HIF_BURST_READ_FAILED,
    eSIR_HAL_HIF_BURST_WRITE_FAILED,
    eSIR_HAL_HIF_BURST_LEN_REQ_INVALID,
    eSIR_HAL_HIF_TX_NO_FRAG_DESC,

    eSIR_HAL_INVALID_PRODUCT_ID,          //   

    eSIR_HAL_INVALID_CAPABILITY, //   
    eSIR_HAL_CB_NOT_ENABLED, //   
    eSIR_HAL_MAC_RATE_INVALID, //   
    eSIR_HAL_RHP_HANG, //   
    eSIR_HAL_UNSUPPORTED, //   
    eSIR_HAL_TSPEC_INVALID, //   

    //                 
    eSIR_NIM_ERROR_BASE=0x2000,
    eSIR_NIM_ERR_INVALID_EVENT,


    //                 
    eSIR_NIM_MMH_ERROR_BASE=0x2100,
    eSIR_NIM_MMH_ERR_INV_EVENT,
    eSIR_NIM_MMH_ERR_MSG_LEN,
    eSIR_NIM_MMH_ERR_IN_Q_TYPE,

    //                 
    eSIR_NIM_MNT_ERROR_BASE=0x2140,

    //           
    eSIR_NIM_WDT_ERROR_BASE=0x2180,

    //                 
    eSIR_LIM_ERROR_BASE=0x2200,
    eSIR_LIM_IGNORE_BEACON,
    eSIR_LIM_INVALID_STA,
    eSIR_LIM_MAX_STA_REACHED_ERROR,

    //                 
    eSIR_SCH_ERROR_BASE=0x2300,

    //                 
    eSIR_PMM_ERROR_BASE=0x2400,
    eSIR_PMM_INVALID_MODE,
    eSIR_PMM_INVALID_STATE,
    eSIR_PMM_INVALID_ROLE,
    eSIR_PMM_STA_NOT_ASSOCIATED,
    eSIR_PMM_HEART_BEAT_TMOUT,
    eSIR_PMM_NTH_BEACON_DELIVERY,

    //                 
    eSIR_ARQ_ERROR_BASE=0x2500,

    //                 
    eSIR_CFG_ERROR_BASE=2600,
    eSIR_CFG_INVALID_ID,
    eSIR_CFG_INVALID_LEN,

    //                    
    eSIR_PRS_ERROR_BASE=0x2700,
    eSIR_IGNORE_IE,

    //                                          
    eSIR_ERROR_LAST

} tSirRetStatus;

# endif //              

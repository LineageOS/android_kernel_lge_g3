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
* Copyright (c) 2012 Qualcomm Atheros, Inc.
* All Rights Reserved.
* Qualcomm Atheros Confidential and Proprietary.
*/

/*===========================================================================
 *
 *  @file:     wlan_status_code.h
 *
 *  @brief:    Common header file containing all the status codes
 *             All status codes have been consolidated into one enum
 *
 *  @author:   Kumar Anand
 *             Copyright (C) 2010, Qualcomm Technologies, Inc.
 *             All rights reserved.
 *
 *=========================================================================*/

#ifndef __WLAN_STATUS_CODE_H__
#define __WLAN_STATUS_CODE_H__

/*                                                                         
               
                                                                         */

/*                                                                            
                                         
                                                                            */

/*                                                                    */
#define WLAN_STATUS_MAX_ENUM_SIZE    0x7FFFFFFF

/*                                                                            
                     
                                                                            */

typedef enum
{
   /*                       */
   PAL_STATUS_SUCCESS = 0,

   /*                       */
   eHAL_STATUS_SUCCESS = 0,

   /*                                                             
                                                               */
   PAL_STATUS_INVAL,

   /*                                                          
                                          */
   PAL_STATUS_ALREADY,

   /*                                              */
   PAL_STATUS_EMPTY,

   /*                                         */
   PAL_STATUS_FAILURE,

   /*                     */
   eHAL_STATUS_FAILURE,

   /*              */
   eHAL_STATUS_INVALID_PARAMETER,

   /*                      */
   eHAL_STATUS_INVALID_STAIDX,

   /*                          */
   eHAL_STATUS_DPU_DESCRIPTOR_TABLE_FULL,

   /*               */
   eHAL_STATUS_NO_INTERRUPTS,

   /*                   */
   eHAL_STATUS_INTERRUPT_PRESENT,

   /*                      */
   eHAL_STATUS_STA_TABLE_FULL,

   /*                         */
   eHAL_STATUS_DUPLICATE_STA,

   /*                  */
   eHAL_STATUS_BSSID_INVALID,

   /*                */
   eHAL_STATUS_STA_INVALID,

   /*                       */
   eHAL_STATUS_DUPLICATE_BSSID,

   /*                    */
   eHAL_STATUS_INVALID_BSSIDX,

   /*                     */
   eHAL_STATUS_BSSID_TABLE_FULL,

   /*                      */
   eHAL_STATUS_INVALID_SIGNATURE,

   /*                */
   eHAL_STATUS_INVALID_KEYID,

   /*                              */
   eHAL_STATUS_SET_CHAN_ALREADY_ON_REQUESTED_CHAN,

   /*                              */
   eHAL_STATUS_UMA_DESCRIPTOR_TABLE_FULL,

   /*                       */
   eHAL_STATUS_DPU_MICKEY_TABLE_FULL,

   /*                               */
   eHAL_STATUS_BA_RX_BUFFERS_FULL,
   eHAL_STATUS_BA_RX_MAX_SESSIONS_REACHED,
   eHAL_STATUS_BA_RX_INVALID_SESSION_ID,

   eHAL_STATUS_TIMER_START_FAILED,
   eHAL_STATUS_TIMER_STOP_FAILED,
   eHAL_STATUS_FAILED_ALLOC,

   /*                    */
   eHAL_STATUS_NOTIFY_BSS_FAIL,

   /*                                                     */
   eHAL_STATUS_DEL_STA_SELF_IGNORED_REF_COUNT_NOT_ZERO,

   /*                                           */
   eHAL_STATUS_ADD_STA_SELF_IGNORED_REF_COUNT_NOT_ZERO,

   /*                                     */
   eHAL_STATUS_FW_SEND_MSG_FAILED,
   
   /*                                     */
   eHAL_STATUS_BSS_DISCONN_BEACON_MISS,
   /*                                */
   eHAL_STATUS_BSS_DISCONN_DEAUTH,
   /*                                  */
   eHAL_STATUS_BSS_DISCONN_DISASSOC,
   
   /*                               */
   eHAL_STATUS_PHY_DATA_ABORT,

   /*                   */
   eHAL_STATUS_PHY_INVALID_NV_FIELD,

   /*                       */
   eHAL_STATUS_WLAN_BOOT_TEST_FAILURE,

   /*                  */
   eHAL_STATUS_MAX_VALUE = WLAN_STATUS_MAX_ENUM_SIZE

} palStatus, eHalStatus;

/*               */
#define PAL_IS_STATUS_SUCCESS(status) (PAL_STATUS_SUCCESS  == (status))
#define HAL_STATUS_SUCCESS( status )  (eHAL_STATUS_SUCCESS == (status))

#endif //                      

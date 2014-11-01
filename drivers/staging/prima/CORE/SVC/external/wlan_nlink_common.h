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
  \file wlan_nlink_common.h
  
  Exports and types for the Netlink Service interface. This header file contains
  message types and definitions that is shared between the user space service
  (e.g. BTC service) and WLAN kernel module.

  Copyright (c) 2009 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

===========================================================================*/

#ifndef WLAN_NLINK_COMMON_H__
#define WLAN_NLINK_COMMON_H__

#include <linux/netlink.h>

/*                                                                           
                     
                                                                           */

/*                                                                           
                                         
                                                                           */
#define WLAN_NL_MAX_PAYLOAD   256     /*                                 */
#define WLAN_NLINK_PROTO_FAMILY  NETLINK_USERSOCK
#define WLAN_NLINK_MCAST_GRP_ID  0x01 

/*                                                                           
                    
                                                                           */

/* 
                                                                                 
                                                                    
                                                              
                                                                             
                                                                     
                                                                            
                                                        
  
                                                       
                                                       
                                                       
 */

//               
#define WLAN_BTC_QUERY_STATE_REQ    0x01  //              
#define WLAN_BTC_BT_EVENT_IND       0x02  //              
#define WLAN_BTC_QUERY_STATE_RSP    0x03  //              
#define WLAN_MODULE_UP_IND          0x04  //              
#define WLAN_MODULE_DOWN_IND        0x05  //              
#define WLAN_STA_ASSOC_DONE_IND     0x06  //              
#define WLAN_STA_DISASSOC_DONE_IND  0x07  //              

//                                                                          
//                                                                  
#define WLAN_AMP_ASSOC_DONE_IND     0x10

//                                                                             
//                                    
#define WLAN_BTC_SOFTAP_BSS_START      0x11


//                                                                  
typedef struct
{
   unsigned char channel;  //                                   
} tWlanAssocData;

#define ANI_NL_MSG_BASE     0x10    /*                     */

typedef enum eAniNlModuleTypes {
   ANI_NL_MSG_PUMAC = ANI_NL_MSG_BASE + 0x01,//               
   ANI_NL_MSG_PTT   = ANI_NL_MSG_BASE + 0x07,//           
   WLAN_NL_MSG_BTC,
   ANI_NL_MSG_MAX  
} tAniNlModTypes, tWlanNlModTypes;

#define WLAN_NL_MSG_BASE ANI_NL_MSG_BASE
#define WLAN_NL_MSG_MAX  ANI_NL_MSG_MAX

//                                             
typedef struct sAniHdr {
   unsigned short type;
   unsigned short length;
} tAniHdr, tAniMsgHdr;

#endif //                     

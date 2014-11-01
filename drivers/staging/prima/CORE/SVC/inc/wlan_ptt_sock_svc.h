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
#ifndef PTT_SOCK_SVC_H
#define PTT_SOCK_SVC_H
#include <wlan_nlink_srv.h>
#include <halTypes.h>
#include <vos_status.h>
#include <wlan_hdd_includes.h>
#include <vos_trace.h>
#include <wlan_nlink_common.h>
/*
                         
                                                                             
                                                                            
                                                                              
                                                                           
                                                                              
                                                                          
       
                                                            
                                                            
                                                            
                                                            
 */
//                           
#define PTT_MSG_READ_REGISTER       0x3040
#define PTT_MSG_WRITE_REGISTER      0x3041
#define PTT_MSG_READ_MEMORY         0x3044
#define PTT_MSG_WRITE_MEMORY        0x3045
#define PTT_MSG_LOG_DUMP_DBG        0x32A1
#define PTT_MSG_FTM_CMDS_TYPE           0x4040
#define ANI_DRIVER_MSG_START         0x0001
#define ANI_MSG_APP_REG_REQ         (ANI_DRIVER_MSG_START + 0)
#define ANI_MSG_APP_REG_RSP         (ANI_DRIVER_MSG_START + 1)
#define ANI_MAX_RADIOS      3
#define ANI_NL_MSG_OK       0
#define ANI_NL_MSG_ERROR    -1
#define ANI_NL_MSG_OVERHEAD (NLMSG_SPACE(tAniHdr + 4))
/*
                                                    
                                          
                        
                        
                        
                        
                        
                        
*/
/*
                                                 
                                                      
                        
                        
                        
                        
                        
                                      
                                  
*/
int ptt_sock_activate_svc(void *pAdapter);
int ptt_sock_send_msg_to_app(tAniHdr *wmsg, int radio, int src_mod, int pid);

/*
                                                                              
                                                                             
                                      
 */
typedef struct sAniNlMsg {
    struct  nlmsghdr nlh;             //               
    int radio;                        //                         
    tAniHdr wmsg;                     //                     
} tAniNlHdr;
typedef struct sAniAppRegReq {
    tAniNlModTypes type;              //          
    int pid;                          //           
} tAniNlAppRegReq;
typedef struct sAniNlAppRegRsp {
    struct nlmsghdr nlh;              //                   
    int radio;                        //           
    tAniHdr wniHdr;                   //                       
    tAniNlAppRegReq regReq;           //                         
    int ret;                          //            
} tAniNlAppRegRsp;
#endif

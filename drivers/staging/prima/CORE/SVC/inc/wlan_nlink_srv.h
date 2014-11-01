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

#ifndef WLAN_NLINK_SRV_H
#define WLAN_NLINK_SRV_H 

#include <linux/skbuff.h>
#include <net/netlink.h>
#include <wlan_nlink_common.h>

#define NLINK_MAX_CALLBACKS (WLAN_NL_MSG_MAX - WLAN_NL_MSG_BASE)

typedef int (* nl_srv_msg_callback)(struct sk_buff * skb);

int nl_srv_init(void);
#ifdef WLAN_KD_READY_NOTIFIER
void nl_srv_exit(int dst_pid);
#else
void nl_srv_exit(void);
#endif /*                        */
int nl_srv_register(tWlanNlModTypes msg_type, nl_srv_msg_callback msg_handler);
int nl_srv_unregister(tWlanNlModTypes msg_type, nl_srv_msg_callback msg_handler);
int nl_srv_ucast(struct sk_buff * skb, int dst_pid);
int nl_srv_bcast(struct sk_buff * skb);
#ifdef WLAN_KD_READY_NOTIFIER
void nl_srv_nl_ready_indication(void);
void nl_srv_nl_close_indication(int pid);
#endif /*                        */
#endif

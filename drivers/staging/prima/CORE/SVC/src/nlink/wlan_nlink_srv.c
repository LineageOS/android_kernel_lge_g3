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

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#include <net/sock.h>
#include <wlan_nlink_srv.h>
#include <vos_trace.h>

//                
static DEFINE_MUTEX(nl_srv_sem);
static struct sock *nl_srv_sock;
static nl_srv_msg_callback nl_srv_msg_handler[NLINK_MAX_CALLBACKS];

#ifdef WLAN_KD_READY_NOTIFIER
const char driverLoaded[]   = "KNLREADY";
const char driverUnLoaded[] = "KNLCLOSE";
#endif /*                        */

//                   
static void nl_srv_rcv (struct sk_buff *sk);
static void nl_srv_rcv_skb (struct sk_buff *skb);
static void nl_srv_rcv_msg (struct sk_buff *skb, struct nlmsghdr *nlh);

/*
                                  
                                        
 */
int nl_srv_init(void)
{
   int retcode = 0;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
   struct netlink_kernel_cfg cfg = {
      .groups = WLAN_NLINK_MCAST_GRP_ID,
      .input = nl_srv_rcv
   };
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,6,0))
   nl_srv_sock = netlink_kernel_create(&init_net, WLAN_NLINK_PROTO_FAMILY,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0))
                                       THIS_MODULE,
#endif
                                       &cfg);
#else
   nl_srv_sock = netlink_kernel_create(&init_net, WLAN_NLINK_PROTO_FAMILY,
      WLAN_NLINK_MCAST_GRP_ID, nl_srv_rcv, NULL, THIS_MODULE);
#endif

   if (nl_srv_sock != NULL) {
      memset(nl_srv_msg_handler, 0, sizeof(nl_srv_msg_handler));
   } else {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
         "NLINK: netlink_kernel_create failed");
      retcode = -ECONNREFUSED;
   }
   return retcode;
}

/*
                              
                                          
 */
#ifdef WLAN_KD_READY_NOTIFIER
void nl_srv_exit(int dst_pid)
#else
void nl_srv_exit(void)
#endif /*                        */
{
#ifdef WLAN_KD_READY_NOTIFIER
   if (0 != dst_pid)
   {
      nl_srv_nl_close_indication(dst_pid);
   }
#endif /*                        */
   netlink_kernel_release(nl_srv_sock);
}

/*
                                                     
                                                     
                                              
 */
int nl_srv_register(tWlanNlModTypes msg_type, nl_srv_msg_callback msg_handler)
{
   int retcode = 0;

   if ((msg_type >= WLAN_NL_MSG_BASE) && (msg_type < WLAN_NL_MSG_MAX) &&
        msg_handler != NULL)
   {
      nl_srv_msg_handler[msg_type - WLAN_NL_MSG_BASE] = msg_handler;
   }
   else {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
         "NLINK: nl_srv_register failed for msg_type %d", msg_type);
      retcode = -EINVAL;
   }

   return retcode;
}
/*
                                                         
 */
int nl_srv_unregister(tWlanNlModTypes msg_type, nl_srv_msg_callback msg_handler)
{
   int retcode = 0;

   if ((msg_type >= WLAN_NL_MSG_BASE) && (msg_type < WLAN_NL_MSG_MAX) &&
       (nl_srv_msg_handler[msg_type - WLAN_NL_MSG_BASE] == msg_handler))
   {
      nl_srv_msg_handler[msg_type - WLAN_NL_MSG_BASE] = NULL;
   }
   else
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
         "NLINK: nl_srv_unregister failed for msg_type %d", msg_type);
      retcode = -EINVAL;
   }

   return retcode;
}

/*
                                                             
                 
 */
int nl_srv_ucast(struct sk_buff *skb, int dst_pid)
{
   int err;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0))
   NETLINK_CB(skb).pid = 0; //            
#else
   NETLINK_CB(skb).portid = 0; //            
#endif
   NETLINK_CB(skb).dst_group = 0; //             

   err = netlink_unicast(nl_srv_sock, skb, dst_pid, MSG_DONTWAIT);

   if (err < 0)
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
      "NLINK: netlink_unicast to pid[%d] failed, ret[0x%X]", dst_pid, err);

   return err;
}

/*
                                                            
                          
 */
int nl_srv_bcast(struct sk_buff *skb)
{
   int err;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,7,0))
   NETLINK_CB(skb).pid = 0; //            
#else
   NETLINK_CB(skb).portid = 0; //            
#endif
   NETLINK_CB(skb).dst_group = WLAN_NLINK_MCAST_GRP_ID; //                 

   err = netlink_broadcast(nl_srv_sock, skb, 0, WLAN_NLINK_MCAST_GRP_ID, GFP_KERNEL);

   if (err < 0)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
         "NLINK: netlink_broadcast failed err = %d", err);
   }
   return err;
}

/*
                                             
                                                         
                                                       
                    
 */
static void nl_srv_rcv (struct sk_buff *sk)
{
   mutex_lock(&nl_srv_sem);
   nl_srv_rcv_skb(sk);
   mutex_unlock(&nl_srv_sem);
}

/*
                                                                    
                                                            
 */
static void nl_srv_rcv_skb (struct sk_buff *skb)
{
   struct nlmsghdr * nlh;

   while (skb->len >= NLMSG_SPACE(0)) {
      u32 rlen;

      nlh = (struct nlmsghdr *)skb->data;

      if (nlh->nlmsg_len < sizeof(*nlh) || skb->len < nlh->nlmsg_len) {
         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN, "NLINK: Invalid "
            "Netlink message: skb[0x%X], len[%d], nlhdr[0x%X], nlmsg_len[%d]",
            (u32)skb, skb->len, (u32)nlh, nlh->nlmsg_len);
         return;
      }

      rlen = NLMSG_ALIGN(nlh->nlmsg_len);
      if (rlen > skb->len)
         rlen = skb->len;
      nl_srv_rcv_msg(skb, nlh);
      skb_pull(skb, rlen);
   }
}

/*
                             
                                                                      
 */
static void nl_srv_rcv_msg (struct sk_buff *skb, struct nlmsghdr *nlh)
{
   int type;

   /*                                         */
   if (!(nlh->nlmsg_flags & NLM_F_REQUEST)) {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
         "NLINK: Received Invalid NL Req type [%x]", nlh->nlmsg_flags);
      return;
   }

   type = nlh->nlmsg_type;

   /*                 */
   if (type < WLAN_NL_MSG_BASE || type >= WLAN_NL_MSG_MAX) {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
         "NLINK: Received Invalid NL Msg type [%x]", type);
      return;
   }

   /*
                                                        
                                         
   */
   if (nlh->nlmsg_len < NLMSG_LENGTH(sizeof(tAniMsgHdr))) {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
         "NLINK: Received NL Msg with invalid len[%x]", nlh->nlmsg_len);
      return;
   }

   VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO,
      "NLINK: Received NL msg type [%d]", type);

   //                                     
   type -= WLAN_NL_MSG_BASE;

   //                    
   if (nl_srv_msg_handler[type] != NULL) {
      (nl_srv_msg_handler[type])(skb);
   } else {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_WARN,
         "NLINK: No handler for Netlink Msg [0x%X]", type);
   }
}

#ifdef WLAN_KD_READY_NOTIFIER
/*
                                                                 
                                                                      
 */
void nl_srv_nl_ready_indication
(
   void
)
{
   struct sk_buff *skb = NULL;
   struct nlmsghdr *nlh;
   int    err;

   skb = alloc_skb(NLMSG_SPACE(sizeof(driverLoaded)), GFP_KERNEL);
   if (NULL == skb)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                 "NLINK: skb alloc fail %s", __func__);
      return;
   }

   nlh = (struct nlmsghdr *)skb->data;
   nlh->nlmsg_pid = 0;  /*             */
   nlh->nlmsg_flags = 0;
   nlh->nlmsg_seq = 0;
   nlh->nlmsg_len = sizeof(driverLoaded);
   memcpy(((char *)nlh) + sizeof(struct nlmsghdr),
          driverLoaded,
          sizeof(driverLoaded));
   skb_put(skb, NLMSG_SPACE(sizeof(driverLoaded)));

   /*                         */
   NETLINK_CB(skb).dst_group = WLAN_NLINK_MCAST_GRP_ID;

   /*                                                */
   err = netlink_broadcast(nl_srv_sock, skb, 0, 1, GFP_KERNEL);
   if (err)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_LOW,
                "NLINK: Ready Indication Send Fail %s, err %d",
                __func__, err);
   }
   return;
}

/*
                                                                 
                                                                      
 */
void nl_srv_nl_close_indication
(
   int pid
)
{
   struct sk_buff *skb = NULL;
   struct nlmsghdr *nlh;
   int err;

   skb = alloc_skb(sizeof(driverUnLoaded),GFP_KERNEL);
   if (NULL == skb)
   {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                 "NLINK: skb alloc fail %s", __func__);
      return;
   }

   nlh = (struct nlmsghdr *)skb->data;
   nlh->nlmsg_pid = 0;  /*             */
   nlh->nlmsg_flags = 0;
   nlh->nlmsg_seq = 0;
   nlh->nlmsg_len = sizeof(driverUnLoaded);
   memcpy(((char *)nlh) + sizeof(struct nlmsghdr),
          driverUnLoaded,
          sizeof(driverUnLoaded));
   skb_put(skb, NLMSG_SPACE(sizeof(driverUnLoaded)));

   /*                         */
   NETLINK_CB(skb).dst_group = 0;
   err = netlink_unicast(nl_srv_sock, skb, pid, MSG_DONTWAIT);
   if (err)
   {
      VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_LOW,
                "NLINK: Close Indication Send Fail %s", __func__);
   }

   return;
}
#endif /*                        */


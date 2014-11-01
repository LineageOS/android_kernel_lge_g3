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
#include <wlan_nlink_srv.h>
#include <wlan_btc_svc.h>
#include <halTypes.h>
#include <vos_status.h>
#include <btcApi.h>
#include <wlan_hdd_includes.h>
#include <vos_trace.h>
//                 
static struct hdd_context_s *pHddCtx;

static int gWiFiChannel;  /*                                           */
static int gAmpChannel;   /*                                          */
static int gBtcDriverMode = WLAN_HDD_INFRA_STATION;  /*                    */


//                   
static int btc_msg_callback (struct sk_buff * skb);
/*
                                             
                                            
 */
void send_btc_nlink_msg (int type, int dest_pid)
{
   struct sk_buff *skb;
   struct nlmsghdr *nlh;
   tAniMsgHdr *aniHdr;
   tWlanAssocData *assocData;
   skb = alloc_skb(NLMSG_SPACE(WLAN_NL_MAX_PAYLOAD), GFP_KERNEL);
   if(skb == NULL) {
      VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
         "BTC: alloc_skb failed\n");
      return;
   }   
   nlh = (struct nlmsghdr *)skb->data;
   nlh->nlmsg_pid = 0;  /*             */
   nlh->nlmsg_flags = 0;
   nlh->nlmsg_seq = 0;
   nlh->nlmsg_type = WLAN_NL_MSG_BTC;
   aniHdr = NLMSG_DATA(nlh);
   aniHdr->type = type;

  /*                                                             */
  if(type == WLAN_BTC_SOFTAP_BSS_START)
  {
     /*                                                         */
     gBtcDriverMode = WLAN_HDD_SOFTAP;
  }
  if(type == WLAN_STA_ASSOC_DONE_IND)
  {
     /*                                                         */
     gBtcDriverMode = WLAN_HDD_INFRA_STATION;
  }

   switch( type )
   {
      case WLAN_STA_DISASSOC_DONE_IND:
         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_LOW,
                    "WiFi unassociated; gAmpChannel %d gWiFiChannel %d", gAmpChannel, gWiFiChannel);

         /*                                                       
                                                                       
                                                                                           
                                                              */
         if((gAmpChannel != 0) || (gWiFiChannel == 0))
         {
           VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_LOW,
                      "No msg for AFH will be sent");
            gWiFiChannel = 0;
            kfree_skb(skb);
            return;
         }
         gWiFiChannel = 0;

         /*                                */

      case WLAN_MODULE_UP_IND:
      case WLAN_MODULE_DOWN_IND:
         aniHdr->length = 0; 
         nlh->nlmsg_len = NLMSG_LENGTH((sizeof(tAniMsgHdr)));
         skb_put(skb, NLMSG_SPACE(sizeof(tAniMsgHdr)));
         break;
      case WLAN_BTC_SOFTAP_BSS_START:
      case WLAN_BTC_QUERY_STATE_RSP:
      case WLAN_STA_ASSOC_DONE_IND:
         aniHdr->length = sizeof(tWlanAssocData);
         nlh->nlmsg_len = NLMSG_LENGTH((sizeof(tAniMsgHdr) + sizeof(tWlanAssocData)));
         assocData = ( tWlanAssocData *)((char*)aniHdr + sizeof(tAniMsgHdr));
         
         assocData->channel = hdd_get_operating_channel( pHddCtx, gBtcDriverMode );

         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_LOW,
                    "New WiFi channel %d gAmpChannel %d gWiFiChannel %d",
                    assocData->channel, gAmpChannel, gWiFiChannel);

         /*                                  */
         if(type == WLAN_STA_ASSOC_DONE_IND)
         {
           /*                                                       
                                                                      
                                                                                             
                                                                                               */
           if((gAmpChannel != 0) || (assocData->channel == gWiFiChannel))
           {
             VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_LOW,
                        "No msg for AFH will be sent");
             gWiFiChannel = assocData->channel;
             kfree_skb(skb);
             return;
           }
         }
         if(type == WLAN_BTC_SOFTAP_BSS_START)
         {
             /*                                                            */
             aniHdr->type = WLAN_STA_ASSOC_DONE_IND;
         }
         gWiFiChannel = assocData->channel;
         skb_put(skb, NLMSG_SPACE((sizeof(tAniMsgHdr)+ sizeof(tWlanAssocData))));
         break;

      case WLAN_AMP_ASSOC_DONE_IND:

         /*                                                                                       
                                                                */

         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_LOW,
                    "New AMP channel %d gAmpChannel %d gWiFiChannel %d", dest_pid, gAmpChannel, gWiFiChannel);
         /*                                                        
                                                                   
                                                                                           
                                                                                  */
         if((gWiFiChannel != 0) || (dest_pid == gAmpChannel))
         {
           VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_LOW,
                      "No msg for AFH will be sent");
            gAmpChannel = dest_pid;
            kfree_skb(skb);
            return;
         }

         gAmpChannel = dest_pid;

         /*                                                         */
         if(dest_pid != 0)
         {
           aniHdr->type = WLAN_STA_ASSOC_DONE_IND;
           aniHdr->length = sizeof(tWlanAssocData);
           nlh->nlmsg_len = NLMSG_LENGTH((sizeof(tAniMsgHdr) + sizeof(tWlanAssocData)));
           assocData = ( tWlanAssocData *)((char*)aniHdr + sizeof(tAniMsgHdr));
           assocData->channel = dest_pid;
           skb_put(skb, NLMSG_SPACE((sizeof(tAniMsgHdr)+ sizeof(tWlanAssocData))));
         }
         else
         {
           aniHdr->type = WLAN_STA_DISASSOC_DONE_IND;
           aniHdr->length = 0;
           nlh->nlmsg_len = NLMSG_LENGTH((sizeof(tAniMsgHdr)));
           skb_put(skb, NLMSG_SPACE(sizeof(tAniMsgHdr)));
         }
         dest_pid = 0;
         break;

      default:
         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR, 
            "BTC: Attempt to send unknown nlink message %d\n", type);
         kfree_skb(skb);
         return;
   }
   if(dest_pid == 0)
      (void)nl_srv_bcast(skb);
   else
      (void)nl_srv_ucast(skb, dest_pid);
}
/*
                                                                
                                                                
 */
int btc_activate_service(void *pAdapter)
{
   pHddCtx = (struct hdd_context_s*)pAdapter;  

   //                                                             
   nl_srv_register(WLAN_NL_MSG_BTC, btc_msg_callback);
   return 0;
}
/*
                                                               
                                                          
 */
int btc_msg_callback (struct sk_buff * skb)
{
   struct nlmsghdr *nlh;
   tAniMsgHdr *msg_hdr;
   tSmeBtEvent *btEvent = NULL;
   nlh = (struct nlmsghdr *)skb->data;
   msg_hdr = NLMSG_DATA(nlh);
   
   /*                                */
   switch(msg_hdr->type)
   {
      case WLAN_BTC_QUERY_STATE_REQ:
         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO, 
            "BTC: Received probe from BTC Service\n");
         send_btc_nlink_msg(WLAN_BTC_QUERY_STATE_RSP, nlh->nlmsg_pid);
         break;
      case WLAN_BTC_BT_EVENT_IND:
         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO, 
            "BTC: Received Bluetooth event indication\n");
         if(msg_hdr->length != sizeof(tSmeBtEvent)) {
            VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
               "BTC: Size mismatch in BT event data\n");
            break;
         }
         btEvent = (tSmeBtEvent*)((char*)msg_hdr + sizeof(tAniMsgHdr));
         (void)sme_BtcSignalBtEvent(pHddCtx->hHal, btEvent);
         break;
      default:
         VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
            "BTC: Received Invalid Msg type [%d]\n", msg_hdr->type);
         break;
   }
   return 0;
}

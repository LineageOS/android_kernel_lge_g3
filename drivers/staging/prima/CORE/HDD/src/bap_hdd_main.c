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

/**========================================================================

  \file  bap_hdd_main.c

  \brief 802.11 BT-AMP PAL Host Device Driver implementation

   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.

   Qualcomm Confidential and Proprietary.

  ========================================================================*/

/*                                                                          

                                            


                                                                       
                                                                


                                                                                                                                                                                                            


                                     
                                                                             
                                       

                                                                            */

/*                                                                          
               
                                                                          */
#ifdef WLAN_BTAMP_FEATURE
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/spinlock.h>
//                        
#include <linux/uaccess.h>
#include <linux/io.h>
//                            
#include <wlan_hdd_includes.h>
#include <wlan_hdd_dp_utils.h>
/*                                                                          */
#include <bap_hdd_main.h>
#include <vos_api.h>
#include <bapApi.h>
#include <btampHCI.h>
/*                                                                          */
#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/hci_core.h>

#include <wlan_hdd_misc.h>
/*                                                                            
                                         
                                                                            */

//                                                                 
//                                                                    
//                            
#define BSL_MAX_PHY_LINKS           ( BSL_MAX_CLIENTS * BSL_MAX_PHY_LINK_PER_CLIENT )
#define BSL_MAX_ALLOWED_PHY_LINKS   255

//                                                   
#define BSL_MAX_RX_PKT_DESCRIPTOR   100
#define BSL_MAX_TX_PKT_DESCRIPTOR   100

//                                                                                  
#define BSL_MAX_SIZE_TX_ACL_QUEUE   50
#define BSL_MAX_SIZE_RX_ACL_QUEUE   50
#define BSL_MAX_SIZE_RX_EVT_QUEUE   50

#if 0
What are the maximum sizes of a command packet, an event packet and an ACL
data packet?

[JimZ]: Sizes:
1. Cmd Maximum size is slightly greater than 672 btyes.  But I am pretty sure
right now that I will never have more than 240 bytes to send down at a time.  And
that is good. Because some rather unpleasant things happen at the HCI interface
if I exceed that.  ( Think 8-bit CPUs.  And the limitations of an 8-bit length
                    field. )

2. Event -  Ditto.

3. Data 1492 bytes
#endif

//     
//                    

#define USE_FINAL_FRAMESC
//                        
//     
//                    

#ifndef USE_FINAL_FRAMESC        //                 
//                              
#define AMP_ASSOC_TLV_TYPE_SIZE 2
#define AMP_ASSOC_TLV_LEN_SIZE 2
#define AMP_ASSOC_TLV_TYPE_AND_LEN_SIZE  (AMP_ASSOC_TLV_TYPE_SIZE + AMP_ASSOC_TLV_LEN_SIZE)

//                              
#define FLOWSPEC_TYPE_SIZE 2
#define FLOWSPEC_LEN_SIZE 2
#define FLOWSPEC_TYPE_AND_LEN_SIZE  (FLOWSPEC_TYPE_SIZE + FLOWSPEC_LEN_SIZE)

//                        
#define CMD_TLV_TYPE_SIZE 2
#define CMD_TLV_LEN_SIZE 2
#define CMD_TLV_TYPE_AND_LEN_SIZE  (CMD_TLV_TYPE_SIZE + CMD_TLV_LEN_SIZE)

//                          
#define EVENT_TLV_TYPE_SIZE 2
#define EVENT_TLV_LEN_SIZE 2
#define EVENT_TLV_TYPE_AND_LEN_SIZE  (EVENT_TLV_TYPE_SIZE + EVENT_TLV_LEN_SIZE)

//                                 
#define DATA_HEADER_SIZE 4

#else                            //                 

//                              
#define AMP_ASSOC_TLV_TYPE_SIZE 1
#define AMP_ASSOC_TLV_LEN_SIZE 2
#define AMP_ASSOC_TLV_TYPE_AND_LEN_SIZE  (AMP_ASSOC_TLV_TYPE_SIZE + AMP_ASSOC_TLV_LEN_SIZE)

//                              
#define FLOWSPEC_TYPE_SIZE 1
#define FLOWSPEC_LEN_SIZE 1
#define FLOWSPEC_TYPE_AND_LEN_SIZE  (FLOWSPEC_TYPE_SIZE + FLOWSPEC_LEN_SIZE)

//                        
#define CMD_TLV_TYPE_SIZE 2
#define CMD_TLV_LEN_SIZE 1
#define CMD_TLV_TYPE_AND_LEN_SIZE  (CMD_TLV_TYPE_SIZE + CMD_TLV_LEN_SIZE)

//                          
#define EVENT_TLV_TYPE_SIZE 1
#define EVENT_TLV_LEN_SIZE 1
#define EVENT_TLV_TYPE_AND_LEN_SIZE  (EVENT_TLV_TYPE_SIZE + EVENT_TLV_LEN_SIZE)

//                                 
#define DATA_HEADER_SIZE 4

#endif                           //                  
//     

#define BSL_MAX_EVENT_SIZE 700

#define BSL_DEV_HANDLE 0x1234

//                      
#define DBGLOG printf
//               
#if defined DUMPLOG
#define DUMPLOG(n, name1, name2, aStr, size) do {                       \
        int i;                                                          \
        DBGLOG("%d. %s: %s = \n", n, name1, name2);                     \
        for (i = 0; i < size; i++)                                      \
            DBGLOG("%2.2x%s", ((unsigned char *)aStr)[i], i % 16 == 15 ? "\n" : " "); \
        DBGLOG("\n");                                                   \
    } while (0)
#else
#define DUMPLOG(n, name1, name2, aStr, size)
#endif

//                                                                 
//                         
#define UNALIGNED
#define INFINITE 0

#define BT_AMP_HCI_CTX_MAGIC 0x48434949    //       

/*                                                                            
                    
                                                                            */

//                        
typedef int            BOOL;
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef void * HANDLE;
typedef char  TCHAR;
typedef void *LPVOID;
typedef const void *LPCVOID;

typedef struct
{
    BOOL                         used;          //                         
    vos_event_t                  ReadableEvt;   //                                  
    ptBtampHandle                bapHdl;        //                  
    vos_list_t                   PhyLinks;      //                                                
//                       
    struct hci_dev               *hdev;        //                               

    /*                                                  */
    spinlock_t                   lock;         /*                            */

    struct                       sk_buff_head txq; /*                               */

    /*                                                */
    unsigned long                rx_state;
    unsigned long                rx_count;
    struct sk_buff               *rx_skb;

    struct net_device            *p_dev; //                               

} BslClientCtxType;

typedef struct
{
    BslClientCtxType* pctx;
    /*                                                                      */
    struct sk_buff               *tx_skb;

    struct work_struct           hciInterfaceProcessing;
    v_U32_t                      magic;

} BslHciWorkStructure;

typedef struct
{
    TCHAR* ValueName;     //                  
    DWORD  Type;          //              
    DWORD  DwordValue;    //            
    TCHAR* StringValue;   //             

} BslRegEntry;

typedef struct
{
    BOOL              used;                //                         
    hdd_list_t        ACLTxQueue[WLANTL_MAX_AC];  //                  
    BslClientCtxType* pClientCtx;          //                                        
    //                 
    v_U8_t            PhyLinkHdl;          //                                
    void*             pPhyLinkDescNode;    //                                           
    //                               

} BslPhyLinkCtxType;

typedef struct
{
    vos_list_node_t    node;  //                      
    BslPhyLinkCtxType* pPhy;  //                              

} BslPhyLinksNodeType;

typedef struct
{
    vos_list_node_t node;     //                      
    vos_pkt_t*      pVosPkt;  //                                                            

} BslRxListNodeType;

//                                  
typedef struct
{
    hdd_list_node_t     node;         //                      
    struct sk_buff *    skb;          //                    

} BslTxListNodeType;

typedef struct
{
    BslPhyLinkCtxType* ptr;   //                                                        

} BslPhyLinkMapEntryType;

/*                                                                            
                          
                                                                            */
BslClientCtxType* gpBslctx;

/*                                                                            
                              
                                                                            */
//                                                           
static void *gpCtx;

//                                                             
static BslPhyLinkMapEntryType BslPhyLinkMap[BSL_MAX_ALLOWED_PHY_LINKS];

//                                                  
static BOOL bBslInited = FALSE;

static BslClientCtxType BslClientCtx[BSL_MAX_CLIENTS];
//                                

static BslPhyLinkCtxType BslPhyLinkCtx[BSL_MAX_PHY_LINKS];
//                             

//                                  
static vos_list_t BslPhyLinksDescPool;
static BslPhyLinksNodeType BslPhyLinksDesc[BSL_MAX_PHY_LINKS];

//                            

/*                                                                           
                         
                                                                           */
static void bslWriteFinish(struct work_struct *work);

/*                                                                           
                                                  
                                                                           */
static int BSL_Open (struct hci_dev *hdev);
static int BSL_Close (struct hci_dev *hdev);
static int BSL_Flush(struct hci_dev *hdev);
static int BSL_IOControl(struct hci_dev *hdev, unsigned int cmd, unsigned long arg);
static int BSL_Write(struct sk_buff *skb);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
static void BSL_Destruct(struct hci_dev *hdev);
#endif


/*                                                                            
                                               
                                                                            */
static v_BOOL_t WLANBAP_AmpConnectionAllowed(void)
{
    v_CONTEXT_t pVosContext = vos_get_global_context( VOS_MODULE_ID_HDD, NULL );
    hdd_context_t *pHddCtx;
    v_BOOL_t retVal = VOS_FALSE;

    if (NULL != pVosContext)
    {
       pHddCtx = vos_get_context( VOS_MODULE_ID_HDD, pVosContext);
       if (NULL != pHddCtx)
       {
           return pHddCtx->isAmpAllowed;
       }
       else
       {
           return retVal;
       }
    }
    return retVal;
}

/* 
                                                                       
                  

                                                         
                                                                    
                                                                      
                       

                                                                       
                                                                       
                                                   

                                                                          
                                                                       
             
                                                                         
                                                                      
                       
                                                                     
             
                                                              

         
                                                          
*/
static VOS_STATUS WLANBAP_STAFetchPktCB
(
    v_PVOID_t             pHddHdl,
    WLANTL_ACEnumType     ucAC,
    vos_pkt_t**           vosDataBuff,
    WLANTL_MetaInfoType*  tlMetaInfo
)
{
    BslPhyLinkCtxType* pPhyCtx;
    VOS_STATUS VosStatus;
    v_U8_t AcIdxStart;
    v_U8_t AcIdx;
    hdd_list_node_t *pLink;
    BslTxListNodeType *pNode;
    struct sk_buff *    skb;
    BslClientCtxType* pctx;
    WLANTL_ACEnumType Ac;
    vos_pkt_t* pVosPkt;
    WLANTL_MetaInfoType TlMetaInfo;
    pctx = &BslClientCtx[0];

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_LOW, "WLANBAP_STAFetchPktCB\n" );

    //                
    if( pHddHdl == NULL || vosDataBuff == NULL ||
            tlMetaInfo == NULL || ucAC >= WLANTL_MAX_AC || ucAC < 0 )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_STAFetchPktCB bad input\n" );
        return VOS_STATUS_E_FAILURE;
    }

    //                                                               
    *vosDataBuff = NULL;

    pPhyCtx = (BslPhyLinkCtxType *)pHddHdl;
    AcIdx = AcIdxStart = ucAC;

    spin_lock_bh(&pPhyCtx->ACLTxQueue[AcIdx].lock);
    VosStatus = hdd_list_remove_front( &pPhyCtx->ACLTxQueue[AcIdx], &pLink );
    spin_unlock_bh(&pPhyCtx->ACLTxQueue[AcIdx].lock);

    if ( VOS_STATUS_E_EMPTY == VosStatus )
    {
        do
        {
            AcIdx = (AcIdx + 1) % WLANTL_MAX_AC;

            spin_lock_bh(&pPhyCtx->ACLTxQueue[AcIdx].lock);
            VosStatus = hdd_list_remove_front( &pPhyCtx->ACLTxQueue[AcIdx], &pLink );
            spin_unlock_bh(&pPhyCtx->ACLTxQueue[AcIdx].lock);

        }
        while ( VosStatus == VOS_STATUS_E_EMPTY && AcIdx != AcIdxStart );

        if ( VosStatus == VOS_STATUS_E_EMPTY )
        {
            //                                                                  
            return(VOS_STATUS_E_EMPTY);
        }
        else if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_ASSERT( 0 );
        }
    }

    if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
    {
        VOS_ASSERT( 0 );
    }

    pNode = (BslTxListNodeType *)pLink;
    skb   = pNode->skb;

   //                                       
   //                                          
    //                          
    VosStatus = vos_pkt_wrap_data_packet( &pVosPkt,
                                          VOS_PKT_TYPE_TX_802_3_DATA,
                                          skb,
                                          NULL,
                                          NULL);

    if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_STAFetchPktCB vos_pkt_wrap_data_packet "
             "failed status =%d\n", VosStatus );
        kfree_skb(skb);  
        return VosStatus;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO, "%s: pVosPkt(vos_pkt_t *)=%p\n", __func__,
               pVosPkt );

    VosStatus = WLANBAP_XlateTxDataPkt( pctx->bapHdl, pPhyCtx->PhyLinkHdl,
                                        &Ac, &TlMetaInfo, pVosPkt);

    if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_STAFetchPktCB WLANBAP_XlateTxDataPkt "
             "failed status =%d\n", VosStatus );

        //                  
        VosStatus = vos_pkt_return_packet( pVosPkt );
        kfree_skb(skb);  
        VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

        return VosStatus;
    }
    //                    
    *vosDataBuff = pVosPkt;

    //                                              
    *tlMetaInfo = TlMetaInfo;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: *vosDataBuff(vos_pkt_t *)=%p\n", __func__, *vosDataBuff );

    return(VOS_STATUS_SUCCESS);
} //                        

/* 
                                                                             

                                                         
                                                              
                                                               

                                                                           
                                                                                
                                                                             
                           
                                                                  

         
                                                          
*/
static VOS_STATUS WLANBAP_STARxCB
(
    v_PVOID_t              pHddHdl,
    vos_pkt_t*             vosDataBuff,
    WLANTL_RxMetaInfoType* pRxMetaInfo
)
{
    BslPhyLinkCtxType* pctx;
    BslClientCtxType* ppctx;
    VOS_STATUS VosStatus = VOS_STATUS_SUCCESS;
    WLANTL_ACEnumType Ac; //                          
    struct sk_buff *skb = NULL;
    vos_pkt_t* pVosPacket;
    vos_pkt_t* pNextVosPacket;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_LOW, "WLANBAP_STARxCB\n" );

    //                
    if ( pHddHdl == NULL || vosDataBuff == NULL || pRxMetaInfo == NULL )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_STARxCB bad input\n" );
        if(NULL != vosDataBuff)
        {
            VosStatus = vos_pkt_return_packet( vosDataBuff );
        }
        return VOS_STATUS_E_FAILURE;
    }

    pctx = (BslPhyLinkCtxType *)pHddHdl;
    ppctx = pctx->pClientCtx;

    if( NULL == ppctx )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_STARxCB ClientCtx is NULL\n" );
        VosStatus = vos_pkt_return_packet( vosDataBuff );
        return VOS_STATUS_E_FAILURE;
    }

    //                                       
   pVosPacket = vosDataBuff;
   do
   {
       //                                                
       //                                                                   
       VosStatus = vos_pkt_walk_packet_chain( pVosPacket, &pNextVosPacket, VOS_FALSE);
       
       //                                                  
       if (!((VosStatus == VOS_STATUS_SUCCESS) || (VosStatus == VOS_STATUS_E_EMPTY)))
       {
           VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_FATAL,"%s: Failure walking packet chain", __func__);
           return VOS_STATUS_E_FAILURE;
       }
       
       //                   
       VosStatus = WLANBAP_XlateRxDataPkt( ppctx->bapHdl, pctx->PhyLinkHdl,
                                              &Ac, pVosPacket );

       if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
       {
           VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_FATAL, "WLANBAP_STARxCB WLANBAP_XlateRxDataPkt "
           "failed status = %d\n", VosStatus );

           VosStatus = VOS_STATUS_E_FAILURE;

           break;
       }

       //                             
       //                                                     
       VosStatus = vos_pkt_get_os_packet( pVosPacket, (v_VOID_t **)&skb, VOS_TRUE );
       if(!VOS_IS_STATUS_SUCCESS( VosStatus ))
       {
           VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "%s: Failure extracting skb from vos pkt. "
             "VosStatus = %d\n", __func__, VosStatus );

           VosStatus = VOS_STATUS_E_FAILURE;

           break;
       }

       //                                                                               
       skb->dev = (struct net_device *) gpBslctx->hdev;
       bt_cb(skb)->pkt_type = HCI_ACLDATA_PKT;
       //                                              
       //                                      
 
       //                               
       gpBslctx->rx_skb = skb;

       //                                                   
       hci_recv_frame(gpBslctx->rx_skb);

       //                                         
       pVosPacket = pNextVosPacket;
       
   } while (pVosPacket);


    //                                                           
    VosStatus = vos_pkt_return_packet( vosDataBuff );
    VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));


    return(VOS_STATUS_SUCCESS);
} //                  

/* 
                                                                                  

                                                         
                                                               
                                                          

                                                                         
                                                                               
                                                   

         
                                                          
*/
extern v_VOID_t WLANBAP_TxPacketMonitorHandler ( v_PVOID_t ); //                  

static VOS_STATUS WLANBAP_TxCompCB
(
    v_PVOID_t      pHddHdl,
    vos_pkt_t*     vosDataBuff,
    VOS_STATUS     wTxSTAtus
)
{
    VOS_STATUS VosStatus = VOS_STATUS_SUCCESS;
    //                           
    void* pOsPkt = NULL;
    BslPhyLinkCtxType* pctx;
    BslClientCtxType* ppctx;
    static int num_packets;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO, "WLANBAP_TxCompCB. vosDataBuff(vos_pkt_t *)=%p\n", vosDataBuff );

    //                                                                      
    //                                                                     
    //                                                                
    //           

    //                
    if ( vosDataBuff == NULL )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_TxCompCB bad input\n" );
        return VOS_STATUS_E_FAILURE;
    }

    //                        
    VosStatus = vos_pkt_get_os_packet( vosDataBuff, &pOsPkt, VOS_TRUE );
    if(!VOS_IS_STATUS_SUCCESS( VosStatus ))
    {
        //                                                              
        VOS_TRACE( VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,"%s: Failure extracting skb from vos pkt", __func__);
        vos_pkt_return_packet( vosDataBuff );
        return VOS_STATUS_E_FAILURE;
    }

    kfree_skb((struct sk_buff *)pOsPkt);

    //                                
    VosStatus = vos_pkt_return_packet( vosDataBuff );

    if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
    {
        VOS_ASSERT(0);
    }

    //                                                                              
    pctx = (BslPhyLinkCtxType *)pHddHdl;
    ppctx = pctx->pClientCtx;
    num_packets = (num_packets + 1) % 4;
    if (num_packets == 0 )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO, "%s: Sending up number of completed packets.  num_packets = %d.\n", __func__, num_packets );
        WLANBAP_TxPacketMonitorHandler ( (v_PVOID_t) ppctx->bapHdl ); //                  
    }

    return(VOS_STATUS_SUCCESS);
} //                   

/* 
                                                  

                                                                              

         
            

*/
static VOS_STATUS BslFlushTxQueues
(
    BslPhyLinkCtxType* pPhyCtx
)
{
    VOS_STATUS VosStatus = VOS_STATUS_SUCCESS;
    v_SINT_t i = -1;
    hdd_list_node_t* pLink;
    BslTxListNodeType *pNode;


    if(TRUE == pPhyCtx->used)
    {
        while (++i != WLANTL_MAX_AC)
        {
            //                                   
            spin_lock_bh(&pPhyCtx->ACLTxQueue[i].lock);
            while (true)
            {
                VosStatus = hdd_list_remove_front(&pPhyCtx->ACLTxQueue[i], &pLink );
                if(VOS_STATUS_E_EMPTY != VosStatus)
                {
                    pNode = (BslTxListNodeType *)pLink;
                    kfree_skb(pNode->skb);
                    continue;
                }
                break;
            }
            spin_unlock_bh(&pPhyCtx->ACLTxQueue[i].lock);
        }
    }
    return(VOS_STATUS_SUCCESS);
} //                 


/* 
                                                                               

                                                         

         
      

*/
static void BslReleasePhyCtx
(
    BslPhyLinkCtxType* pPhyCtx
)
{
    v_U32_t OldMapVal;
    VOS_STATUS VosStatus = VOS_STATUS_SUCCESS;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslReleasePhyCtx\n" );

    pPhyCtx->used = FALSE;


    if (BslPhyLinkMap[pPhyCtx->PhyLinkHdl].ptr == NULL) return;


    //                                                                                
    OldMapVal = vos_atomic_set_U32( (v_U32_t *) (BslPhyLinkMap[pPhyCtx->PhyLinkHdl].ptr),
                                    (v_U32_t) 0 );

    //                        
    VosStatus =  BslFlushTxQueues(pPhyCtx);

    //                         
    //                                                           

    //                                                               
    //                                                   
    if( pPhyCtx->pPhyLinkDescNode )
    {
        VosStatus = vos_list_remove_node( &pPhyCtx->pClientCtx->PhyLinks,
                                          &((BslPhyLinksNodeType*)pPhyCtx->pPhyLinkDescNode)->node);
        VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ) );
        //                                          
        VosStatus = vos_list_insert_front(&BslPhyLinksDescPool,&((BslPhyLinksNodeType*)pPhyCtx->pPhyLinkDescNode)->node);
        VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ) );

        pPhyCtx->pPhyLinkDescNode = NULL;
    }
    pPhyCtx->pClientCtx = NULL;//                                 

    pPhyCtx->used = FALSE;

} //                   

/* 
                                                                                 

                                  
                                
                                
                           
                        
                 
                    

                                                

                                                                          
                                                                 
                                                                           
                                         
                                                                      
                  

         
                                                          

                                                      
                              
*/
static VOS_STATUS WLANBAP_EventCB
(
    v_PVOID_t      pHddHdl,   /*                                       
                                                                              
                                                                              
                                                                               
                                                                           */
    tpBtampHCI_Event pBapHCIEvent, /*                                           
                                                                        */
    v_BOOL_t AssocSpecificEvent /*                                                 */
)
{
    BslClientCtxType* pctx;
    VOS_STATUS VosStatus = VOS_STATUS_SUCCESS;
    vos_pkt_t* pVosPkt;
    v_U32_t PackStatus;
    static v_U8_t Buff[BSL_MAX_EVENT_SIZE]; //                
    v_U32_t Written = 0; //                      
    v_U32_t OldMapVal;
    struct sk_buff *skb = NULL;

    //                
    if ( pBapHCIEvent == NULL )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_EventCB bad input\n" );
        return VOS_STATUS_E_FAILURE;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB event=%d "
       "assoc_specific=%d\n", pBapHCIEvent->bapHCIEventCode, AssocSpecificEvent );

    if ( pHddHdl == NULL )
    {
        /*                                                                  
                                                        
                                                                          
                                                                                                
                                                                                               
        */
        if (( AssocSpecificEvent ) && 
            (BTAMP_TLV_HCI_PHYSICAL_LINK_COMPLETE_EVENT != pBapHCIEvent->bapHCIEventCode) &&
            (BTAMP_TLV_HCI_DISCONNECT_PHYSICAL_LINK_COMPLETE_EVENT != pBapHCIEvent->bapHCIEventCode))
        {
            pctx = gpBslctx;
        }
        else
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_FATAL, "WLANBAP_EventCB bad input\n" );
            return VOS_STATUS_E_FAILURE;
        }
    }


    if(NULL != pHddHdl)
    {
        if ( AssocSpecificEvent )
        {
            //                                           
            pctx = ((BslPhyLinkCtxType *)pHddHdl)->pClientCtx;
        }
        else
        {
            pctx = (BslClientCtxType *)pHddHdl;
        }
    }

    if(NULL == pctx)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "pctx is NULL in %s", __func__);

        return VOS_STATUS_E_FAULT;

    }

    VosStatus = vos_pkt_get_packet( &pVosPkt, VOS_PKT_TYPE_RX_RAW,
                                    BSL_MAX_EVENT_SIZE, 1, 0, NULL, NULL);

    if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_EventCB vos_pkt_get_packet "
          "failed status=%d\n", VosStatus );
        return(VosStatus);
    }

    switch ( pBapHCIEvent->bapHCIEventCode )
    {
        /*            */
    case BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT:
    {
        /*
                                    
                                    
                                                  
                                             
                                                             
                                                              
                                                            
                                                             
                                                               
                                                                
                                                    
                                                 
                                                  
                                                     
                                                      
                                                        
                                                         
                                                   
                                                             
                                                            
                                               
                                                   
                                                          
                                                           
                                                
                                        
                                                         
                                                   
                                                     
                                                 
                                                  
                                                

         */

        //     
        PackStatus = btampPackTlvHCI_Command_Complete_Event( pctx,
                     &pBapHCIEvent->u.btampCommandCompleteEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampPackTlvHCI_Command_Complete_Event failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_COMMAND_STATUS_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Command_Status_Event( pctx,
                     &pBapHCIEvent->u.btampCommandStatusEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampPackTlvHCI_Command_Status_Event failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_NUM_OF_COMPLETED_PKTS_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Num_Completed_Pkts_Event( pctx,
                     &pBapHCIEvent->u.btampNumOfCompletedPktsEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampPackTlvHCI_Num_Completed_Pkts_Event failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_NUM_OF_COMPLETED_DATA_BLOCKS_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Num_Completed_Data_Blocks_Event( pctx,
                     &pBapHCIEvent->u.btampNumOfCompletedDataBlocksEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampPackTlvHCI_Num_Completed_Data_Blocks_Event failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_HARDWARE_ERROR_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Hardware_Error_Event( pctx,
                     &pBapHCIEvent->u.btampHardwareErrorEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_FLUSH_OCCURRED_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Flush_Occurred_Event( pctx,
                     &pBapHCIEvent->u.btampFlushOccurredEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampPackTlvHCI_Flush_Occurred_Event failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_ENHANCED_FLUSH_COMPLETE_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Enhanced_Flush_Complete_Event( pctx,
                     &pBapHCIEvent->u.btampEnhancedFlushCompleteEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampPackTlvHCI_Enhanced_Flush_Complete_Event failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_LOOPBACK_COMMAND_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Loopback_Command_Event( pctx,
                     &pBapHCIEvent->u.btampLoopbackCommandEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_DATA_BUFFER_OVERFLOW_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Data_Buffer_Overflow_Event( pctx,
                     &pBapHCIEvent->u.btampDataBufferOverflowEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_QOS_VIOLATION_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Qos_Violation_Event( pctx,
                     &pBapHCIEvent->u.btampQosViolationEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    /*                 */
    case BTAMP_TLV_HCI_GENERIC_AMP_LINK_KEY_NOTIFICATION_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Generic_AMP_Link_Key_Notification_Event( pctx,
                     &pBapHCIEvent->u.btampGenericAMPLinkKeyNotificationEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_PHYSICAL_LINK_COMPLETE_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Physical_Link_Complete_Event( pctx,
                     &pBapHCIEvent->u.btampPhysicalLinkCompleteEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        //                                                                   
        if ( pBapHCIEvent->u.btampPhysicalLinkCompleteEvent.status ==
                WLANBAP_STATUS_SUCCESS )
        {
            //                            
            VosStatus = WLANBAP_RegisterDataPlane( pctx->bapHdl,
                                                   WLANBAP_STAFetchPktCB,
                                                   WLANBAP_STARxCB,
                                                   WLANBAP_TxCompCB,
                                                   (BslPhyLinkCtxType *)pHddHdl );

            if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
            {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_EventCB WLANBAP_RegisterDataPlane "
                  "failed status = %d\n", VosStatus );
                //                                                        
            }
            else
            {
                //                                                                          
                OldMapVal = vos_atomic_set_U32( (v_U32_t *)BslPhyLinkMap+pBapHCIEvent->u.btampPhysicalLinkCompleteEvent.phy_link_handle,
                                                (v_U32_t) pHddHdl );

//                                                                            
            }
        }
        else if ( pBapHCIEvent->u.btampPhysicalLinkCompleteEvent.status ==
                  WLANBAP_ERROR_HOST_REJ_RESOURCES )
        {
            BslReleasePhyCtx( (BslPhyLinkCtxType *)pHddHdl );
        }
        else if ( pBapHCIEvent->u.btampPhysicalLinkCompleteEvent.status ==
                  WLANBAP_ERROR_HOST_TIMEOUT )
        {
            //                                                                                     
            //                                                                          
            OldMapVal = vos_atomic_set_U32( (v_U32_t *)BslPhyLinkMap+pBapHCIEvent->u.btampPhysicalLinkCompleteEvent.phy_link_handle,
                                            (v_U32_t) pHddHdl );

//                                                                            

            BslReleasePhyCtx( (BslPhyLinkCtxType *)pHddHdl );
        }
        else if ( pBapHCIEvent->u.btampPhysicalLinkCompleteEvent.status ==
                  WLANBAP_ERROR_MAX_NUM_CNCTS )
        {
            //                                                                                             
            //                                                                          
            OldMapVal = vos_atomic_set_U32( (v_U32_t *)BslPhyLinkMap+pBapHCIEvent->u.btampPhysicalLinkCompleteEvent.phy_link_handle,
                                            (v_U32_t) pHddHdl );
//                                                                            

            BslReleasePhyCtx( (BslPhyLinkCtxType *)pHddHdl );
        }
        else if ( pBapHCIEvent->u.btampPhysicalLinkCompleteEvent.status ==
                  WLANBAP_ERROR_HOST_TIMEOUT )
        {
            BslReleasePhyCtx( (BslPhyLinkCtxType *)pHddHdl );
        }
        else if ( pBapHCIEvent->u.btampPhysicalLinkCompleteEvent.status ==
                  0x16 /*                                  */ )
        {
            BslReleasePhyCtx( (BslPhyLinkCtxType *)pHddHdl );
        }
        else if ( pBapHCIEvent->u.btampPhysicalLinkCompleteEvent.status ==
                  0x8 /*                            */ )
        {
            BslReleasePhyCtx( (BslPhyLinkCtxType *)pHddHdl );
        }
        else if ( pBapHCIEvent->u.btampPhysicalLinkCompleteEvent.status ==
                  WLANBAP_ERROR_NO_CNCT )
        {
            BslReleasePhyCtx( (BslPhyLinkCtxType *)pHddHdl );
        }
        else
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_EventCB unexpected HCI Phy Link Comp Evt "
               "status =%d\n", pBapHCIEvent->u.btampPhysicalLinkCompleteEvent.status );
        }

        break;
    }
    case BTAMP_TLV_HCI_CHANNEL_SELECTED_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Channel_Selected_Event( pctx,
                     &pBapHCIEvent->u.btampChannelSelectedEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_DISCONNECT_PHYSICAL_LINK_COMPLETE_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Disconnect_Physical_Link_Complete_Event( pctx,
                     &pBapHCIEvent->u.btampDisconnectPhysicalLinkCompleteEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        //                                                                        
        //                                                                            
        //                                                    
        if ( pBapHCIEvent->u.btampDisconnectPhysicalLinkCompleteEvent.status == WLANBAP_STATUS_SUCCESS &&
                pBapHCIEvent->u.btampDisconnectPhysicalLinkCompleteEvent.reason == WLANBAP_ERROR_TERM_BY_LOCAL_HOST )
        {
            BslReleasePhyCtx( (BslPhyLinkCtxType *)pHddHdl );
        }
        else
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_EventCB unexpected HCI Dis Phy Link Comp Evt "
               "status =%d reason =%d\n", pBapHCIEvent->u.btampDisconnectPhysicalLinkCompleteEvent.status,
                       pBapHCIEvent->u.btampDisconnectPhysicalLinkCompleteEvent.reason );
        }

        break;
    }
    case BTAMP_TLV_HCI_PHYSICAL_LINK_LOSS_WARNING_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Physical_Link_Loss_Warning_Event( pctx,
                     &pBapHCIEvent->u.btampPhysicalLinkLossWarningEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_PHYSICAL_LINK_RECOVERY_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Physical_Link_Recovery_Event( pctx,
                     &pBapHCIEvent->u.btampPhysicalLinkRecoveryEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_LOGICAL_LINK_COMPLETE_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Logical_Link_Complete_Event( pctx,
                     &pBapHCIEvent->u.btampLogicalLinkCompleteEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_DISCONNECT_LOGICAL_LINK_COMPLETE_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Disconnect_Logical_Link_Complete_Event( pctx,
                     &pBapHCIEvent->u.btampDisconnectLogicalLinkCompleteEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_FLOW_SPEC_MODIFY_COMPLETE_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Flow_Spec_Modify_Complete_Event( pctx,
                     &pBapHCIEvent->u.btampFlowSpecModifyCompleteEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    case BTAMP_TLV_HCI_SHORT_RANGE_MODE_CHANGE_COMPLETE_EVENT:
    {
        //     
        PackStatus = btampPackTlvHCI_Short_Range_Mode_Change_Complete_Event( pctx,
                     &pBapHCIEvent->u.btampShortRangeModeChangeCompleteEvent, Buff, BSL_MAX_EVENT_SIZE, &Written );

        if ( !BTAMP_SUCCEEDED( PackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "WLANBAP_EventCB: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", PackStatus);
            //                 
            VosStatus = vos_pkt_return_packet( pVosPkt );

            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

        break;
    }
    default:
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_EventCB unexpected event\n" );

        VosStatus = vos_pkt_return_packet( pVosPkt );

        VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

        return(VOS_STATUS_E_FAILURE);
        break;
    }
    }

    VOS_ASSERT(Written <= BSL_MAX_EVENT_SIZE);

    //                               
    VosStatus = vos_pkt_push_head( pVosPkt, Buff, Written );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_EventCB vos_pkt_push_head "
          "status =%d\n", VosStatus );

            //                  
            VosStatus = vos_pkt_return_packet( pVosPkt );
            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

            return(VOS_STATUS_E_FAILURE);
        }

    //                             
    //                                                     
    VosStatus = vos_pkt_get_os_packet( pVosPkt, (v_VOID_t **)&skb, VOS_TRUE );
    if(!VOS_IS_STATUS_SUCCESS( VosStatus ))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "%s: Failure extracting skb from vos pkt. "
          "VosStatus = %d\n", __func__, VosStatus );

        //                  
        VosStatus = vos_pkt_return_packet( pVosPkt );
        VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

        return(VOS_STATUS_E_FAILURE);
    }

    //                                                           
    VosStatus = vos_pkt_return_packet( pVosPkt );
    VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ));

    //                                                                               
    skb->dev = (struct net_device *) gpBslctx->hdev;
    bt_cb(skb)->pkt_type = HCI_EVENT_PKT;
    //                                              
    //                                      

    //                               
    gpBslctx->rx_skb = skb;

    //                                                   
    hci_recv_frame(gpBslctx->rx_skb);

    return(VOS_STATUS_SUCCESS);
} //                  

static VOS_STATUS  
WLANBAP_PhyLinkFailure
( 
    BslClientCtxType* pctx,
    v_U8_t       phy_link_handle
)
{
    VOS_STATUS  vosStatus;
    tBtampHCI_Event bapHCIEvent;

    /*                                                      */ 
    bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_PHYSICAL_LINK_COMPLETE_EVENT;
    bapHCIEvent.u.btampPhysicalLinkCompleteEvent.present = 1;
    bapHCIEvent.u.btampPhysicalLinkCompleteEvent.status = WLANBAP_ERROR_UNSPECIFIED_ERROR;
    bapHCIEvent.u.btampPhysicalLinkCompleteEvent.phy_link_handle 
        = phy_link_handle;
    bapHCIEvent.u.btampPhysicalLinkCompleteEvent.ch_number 
        = 0;
    //                                                                       
    BslPhyLinkCtx[0].pClientCtx = pctx;
    vosStatus = WLANBAP_EventCB( &BslPhyLinkCtx[0], &bapHCIEvent, TRUE );

    return vosStatus;
}

/* 
                                                                                    
                   

                                                 

         
                                 

*/
static BOOL BslFindAndInitClientCtx
(
    BslClientCtxType** pctx_
)
{
    VOS_STATUS VosStatus = VOS_STATUS_SUCCESS;
    BslClientCtxType* pctx;
    v_U8_t i;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslFindAndInitClientCtx\n" );

    if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
    {
        VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,"%s:BslClientLock already inited",__func__);
        //           
    }

    for ( i=0; i < BSL_MAX_CLIENTS; i++ )
    {
        if ( !BslClientCtx[i].used )
        {
            VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,"%s:BslClientCtx[%d] selected",__func__, i);
            BslClientCtx[i].used = TRUE;
            break;
        }
    }

    if ( i == BSL_MAX_CLIENTS )
    {
        //                                 
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "BslFindAndInitClientCtx no more "
          "clients can be supported MAX=%d\n", BSL_MAX_CLIENTS );
        return FALSE;
    }

    //                        
    pctx = gpBslctx;

    //                      
    VosStatus = WLANBAP_GetNewHndl(&pctx->bapHdl);

    if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
    {
        pctx->used = FALSE;

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s:WLAN_GetNewHndl Failed",__func__);

        return(FALSE);
    }

    //                                                                     
    //                                                                  
    //                                                                    
    //                                                                
    VosStatus = WLAN_BAPRegisterBAPCallbacks( pctx->bapHdl, WLANBAP_EventCB, pctx );

    if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
    {
        pctx->used = FALSE;

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s:WLAN_BAPRegsiterBAPCallaback Failed",__func__);

        return(FALSE);
    }

    //                                                                    
    VosStatus = vos_list_init( &pctx->PhyLinks );
    VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ) );

    *pctx_ = pctx;

    return(TRUE);
} //                         

/* 
                                                                               
         

                                              

         
      

*/
//     
static void BslReleaseClientCtx
(
    BslClientCtxType* pctx
)
{
    VOS_STATUS VosStatus = VOS_STATUS_SUCCESS;
    vos_list_node_t* pLink;
    BslPhyLinksNodeType *pPhyNode;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_LOW, "BslReleaseClientCtx\n" );

    //                                                                                             
    //                                          

    //                                                                                          
    //                                                                                         
    //                                                                        
    VosStatus = WLANBAP_ReleaseHndl( pctx->bapHdl );
    VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ) );


    //                                                                    
    while ( VOS_IS_STATUS_SUCCESS( VosStatus = vos_list_remove_front( &pctx->PhyLinks, &pLink ) ) )
    {
        pPhyNode = (BslPhyLinksNodeType *)pLink;

        //                                                                    
        //                                                         
        pPhyNode->pPhy->pPhyLinkDescNode = NULL;

        BslReleasePhyCtx( pPhyNode->pPhy );
    }

    VOS_ASSERT( VosStatus == VOS_STATUS_E_EMPTY );

    //                           
    VosStatus = vos_list_destroy( &pctx->PhyLinks );
    VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ) );

    pctx->used = FALSE;

} //                      
//      

/* 
                                                            


         
                                 

*/
static BOOL BslInitPhyCtx (void)
{
    v_U16_t i;
    //                 

    for ( i=0; i<BSL_MAX_PHY_LINKS; i++ )
    {
        BslPhyLinkCtx[i].used = FALSE;
    }

    return (TRUE);
} //                


/* 
                                                                                
                                                                         

                                       
                                                                         
                                                            

         
                                 

*/
static BOOL BslFindAndInitPhyCtx
(
    BslClientCtxType*   pctx,
    v_U8_t              PhyLinkHdl,
    BslPhyLinkCtxType** ppPhyCtx
)
{
    VOS_STATUS VosStatus = VOS_STATUS_SUCCESS;
    v_U16_t i;
    v_U16_t j;
    vos_list_node_t* pLink;
    BslPhyLinksNodeType *pNode;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_LOW, "BslFindAndInitPhyCtx\n" );

    for ( i=0; i<BSL_MAX_PHY_LINKS; i++ )
    {
        if ( !BslPhyLinkCtx[i].used )
        {
            BslPhyLinkCtx[i].used = TRUE;
            break;
        }
    }

    if ( i==BSL_MAX_PHY_LINKS )
    {
        return(FALSE);
    }
    else
    {

        //                      

        *ppPhyCtx = BslPhyLinkCtx + i;

        //                                                                              
        BslPhyLinkCtx[i].pClientCtx = pctx;

        //             
        (*ppPhyCtx)->used = TRUE;

        //                          
        BslPhyLinkCtx[i].PhyLinkHdl = PhyLinkHdl;

        //                   
        for ( j=0; j<WLANTL_MAX_AC; j++ )
        {
            hdd_list_init( &BslPhyLinkCtx[i].ACLTxQueue[j], HDD_TX_QUEUE_MAX_LEN );
            //                                                             
            //                                                            
            VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ) );
        }

        //                                                                 
        //                              

        //               
        VosStatus = vos_list_remove_front( &BslPhyLinksDescPool, &pLink );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            //                                                        
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_LOW, "BslFindAndInitPhyCtx failed to "
             "get node from BslPhyLinksDescPool vstatus=%d\n", VosStatus );
            BslReleasePhyCtx( *ppPhyCtx );
            return FALSE;
        }

        //                                
        pNode = (BslPhyLinksNodeType *) pLink;
        pNode->node = *pLink;
        pNode->pPhy = *ppPhyCtx;


        //                                         
        VosStatus = vos_list_insert_back( &pctx->PhyLinks, pLink );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_ASSERT(0);
        }

        //                                                           
        //                                                            
        (*ppPhyCtx)->pPhyLinkDescNode = pNode;

        return(TRUE);
    }
} //                       

/* 
                                                                               
                                                                             

                                              
                                                                    
                                                     

         
                                 

*/
static BOOL BslProcessHCICommand
(
    BslClientCtxType* pctx,
    LPCVOID pBuffer_,
    DWORD Count_
)
{
    LPVOID pBuffer = (LPVOID) pBuffer_; //                                   
    v_U16_t Count = (v_U16_t) Count_;  //                                  
    v_U32_t UnpackStatus;
    VOS_STATUS VosStatus = VOS_STATUS_SUCCESS;
    BOOL Status;
    BslPhyLinkCtxType* pPhyCtx;
    tBtampHCI_Event HCIEvt;
    v_U16_t x = 1;
    int i = 0;

    //                                                                          
    //              
    v_U16_t cmdOpcode = *(UNALIGNED v_U16_t *)pBuffer;
    v_U8_t *pBuf = (v_U8_t *)pBuffer;
    v_U8_t *pTmp = (v_U8_t *)pBuf;

    //                                                                              
    if(*(v_U8_t *)&x == 0)
    {
        //   
        cmdOpcode = ( cmdOpcode & 0xFF ) << 8 | ( cmdOpcode & 0xFF00 ) >> 8;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_LOW, "BslProcessHCICommand: cmdOpcode = %hx\n", cmdOpcode );

    for(i=0; i<4; i++)
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: *pBuf before advancepTmp[%x] = %x\n", i,pTmp[i] );

    pBuf+=CMD_TLV_TYPE_AND_LEN_SIZE;


    switch ( cmdOpcode )
    {
        /*                                */
    case BTAMP_TLV_HCI_CREATE_PHYSICAL_LINK_CMD:
    {
        tBtampTLVHCI_Create_Physical_Link_Cmd CreatePhysicalLinkCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Create_Physical_Link_Cmd( NULL,
                       pBuf, Count, &CreatePhysicalLinkCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Create_Physical_Link_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        if(VOS_FALSE == WLANBAP_AmpConnectionAllowed())
        {
            VosStatus = WLANBAP_PhyLinkFailure(pctx, CreatePhysicalLinkCmd.phy_link_handle);
            if ( VOS_STATUS_SUCCESS != VosStatus )
            {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "BslProcessHCICommand: WLANBAP_PhyLinkFailure failed");
                //                 
                return(FALSE);
            }
            break;
        }

        //                                   
        Status = BslFindAndInitPhyCtx( pctx, CreatePhysicalLinkCmd.phy_link_handle,
                                       &pPhyCtx );

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "CreatePhysicalLinkCmd.phy_link_handle=%d\n",CreatePhysicalLinkCmd.phy_link_handle);

        if ( !Status )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: BslFindAndInitPhyCtx failed");
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPPhysicalLinkCreate( pctx->bapHdl,
                                                &CreatePhysicalLinkCmd, pPhyCtx, &HCIEvt );


        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPPhysicalLinkCreate failed status %d", VosStatus);
            //                 
            BslReleasePhyCtx( pPhyCtx );
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pPhyCtx, &HCIEvt, TRUE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            BslReleasePhyCtx( pPhyCtx );
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_ACCEPT_PHYSICAL_LINK_CMD:
    {
        tBtampTLVHCI_Accept_Physical_Link_Cmd AcceptPhysicalLinkCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Accept_Physical_Link_Cmd( NULL,
                       pBuf, Count, &AcceptPhysicalLinkCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Accept_Physical_Link_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        if(VOS_FALSE == WLANBAP_AmpConnectionAllowed())
        {
            VosStatus = WLANBAP_PhyLinkFailure(pctx, AcceptPhysicalLinkCmd.phy_link_handle);
            if ( VOS_STATUS_SUCCESS != VosStatus )
            {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "BslProcessHCICommand: WLANBAP_PhyLinkFailure failed");
                //                 
                return(FALSE);
            }
            break;
        }

        //                                   
        Status = BslFindAndInitPhyCtx( pctx, AcceptPhysicalLinkCmd.phy_link_handle,
                                       &pPhyCtx );

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "AcceptPhysicalLinkCmd.phy_link_handle=%d\n",AcceptPhysicalLinkCmd.phy_link_handle);

        if ( !Status )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: BslFindAndInitPhyCtx failed");
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPPhysicalLinkAccept( pctx->bapHdl,
                                                &AcceptPhysicalLinkCmd, pPhyCtx, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPPhysicalLinkAccept failed status %d", VosStatus);
            //                 
            BslReleasePhyCtx( pPhyCtx );
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pPhyCtx, &HCIEvt, TRUE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            BslReleasePhyCtx( pPhyCtx );
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_DISCONNECT_PHYSICAL_LINK_CMD:
    {
        tBtampTLVHCI_Disconnect_Physical_Link_Cmd DisconnectPhysicalLinkCmd;
        Count = Count - 3;//                                            
        pTmp = pBuf;
        for(i=0; i<4; i++)
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: *pBuf in Disconnect phy link pTmp[%x] = %x\n", i,pTmp[i] );
        //       
        UnpackStatus = btampUnpackTlvHCI_Disconnect_Physical_Link_Cmd( NULL,
                       pBuf, Count, &DisconnectPhysicalLinkCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Disconnect_Physical_Link_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPPhysicalLinkDisconnect( pctx->bapHdl,
                    &DisconnectPhysicalLinkCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPPhysicalLinkDisconnect failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_CREATE_LOGICAL_LINK_CMD:
    {
        tBtampTLVHCI_Create_Logical_Link_Cmd CreateLogicalLinkCmd;
        Count -= 3; //                                          
        //       
        UnpackStatus = btampUnpackTlvHCI_Create_Logical_Link_Cmd( NULL,
                       pBuf, Count, &CreateLogicalLinkCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Create_Logical_Link_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPLogicalLinkCreate( pctx->bapHdl,
                                               &CreateLogicalLinkCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPLogicalLinkCreate failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_ACCEPT_LOGICAL_LINK_CMD:
    {
        tBtampTLVHCI_Accept_Logical_Link_Cmd AcceptLogicalLinkCmd;
        Count = Count - 3;//                               
        //       
        UnpackStatus = btampUnpackTlvHCI_Accept_Logical_Link_Cmd( NULL,
                       pBuf, Count, &AcceptLogicalLinkCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Accept_Logical_Link_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPLogicalLinkAccept( pctx->bapHdl,
                                               &AcceptLogicalLinkCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPLogicalLinkAccept failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_DISCONNECT_LOGICAL_LINK_CMD:
    {
        tBtampTLVHCI_Disconnect_Logical_Link_Cmd DisconnectLogicalLinkCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Disconnect_Logical_Link_Cmd( NULL,
                       pBuf, Count, &DisconnectLogicalLinkCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Disconnect_Logical_Link_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPLogicalLinkDisconnect( pctx->bapHdl,
                    &DisconnectLogicalLinkCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPLogicalLinkDisconnect failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_LOGICAL_LINK_CANCEL_CMD:
    {
        tBtampTLVHCI_Logical_Link_Cancel_Cmd LogicalLinkCancelCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Logical_Link_Cancel_Cmd( NULL,
                       pBuf, Count, &LogicalLinkCancelCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Logical_Link_Cancel_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPLogicalLinkCancel( pctx->bapHdl,
                                               &LogicalLinkCancelCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPLogicalLinkCancel failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_FLOW_SPEC_MODIFY_CMD:
    {
        tBtampTLVHCI_Flow_Spec_Modify_Cmd FlowSpecModifyCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Flow_Spec_Modify_Cmd( NULL,
                       pBuf, Count, &FlowSpecModifyCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Flow_Spec_Modify_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPFlowSpecModify( pctx->bapHdl,
                                            &FlowSpecModifyCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPFlowSpecModify failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    /*
                                           
    */
    case BTAMP_TLV_HCI_RESET_CMD:
    {
        VosStatus = WLAN_BAPReset( pctx->bapHdl );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReset failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_SET_EVENT_MASK_CMD:
    {
        tBtampTLVHCI_Set_Event_Mask_Cmd SetEventMaskCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Set_Event_Mask_Cmd( NULL,
                       pBuf, Count, &SetEventMaskCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Set_Event_Mask_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPSetEventMask( pctx->bapHdl,
                                          &SetEventMaskCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPSetEventMask failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_FLUSH_CMD:
    {
        tBtampTLVHCI_Flush_Cmd FlushCmd;

        //       
        UnpackStatus = btampUnpackTlvHCI_Flush_Cmd( NULL,
                       pBuf, Count, &FlushCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Flush_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        /*                    */
//                
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "%s:HCI Flush command  - will flush Tx Queue", __func__);
//                  
        //                               
        pPhyCtx = &BslPhyLinkCtx[0];
        VosStatus = BslFlushTxQueues ( pPhyCtx);

        /*                         */
        VosStatus = WLAN_BAPFlush( pctx->bapHdl, &FlushCmd );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "BslProcessHCICommand: WLAN_BAPFlush failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_ENHANCED_FLUSH_CMD:
    {
        tBtampTLVHCI_Enhanced_Flush_Cmd FlushCmd;

        //       
        UnpackStatus = btampUnpackTlvHCI_Enhanced_Flush_Cmd( NULL,
                                                             pBuf, Count, &FlushCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "BslProcessHCICommand: btampUnpackTlvHCI_Enhanced_Flush_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        /*                    */
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s:HCI Flush command  - will flush Tx Queue for pkt type %d", __func__, FlushCmd.packet_type);
        //                           
        if(WLANTL_AC_BE == FlushCmd.packet_type)
        {
            pPhyCtx = &BslPhyLinkCtx[0];
            VosStatus = BslFlushTxQueues ( pPhyCtx);
        }

        /*                         */
        VosStatus = WLAN_EnhancedBAPFlush( pctx->bapHdl, &FlushCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "BslProcessHCICommand: WLAN_EnahncedBAPFlush failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_READ_CONNECTION_ACCEPT_TIMEOUT_CMD:
    {
        VosStatus = WLAN_BAPReadConnectionAcceptTimeout( pctx->bapHdl, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadConnectionAcceptTimeout failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_WRITE_CONNECTION_ACCEPT_TIMEOUT_CMD:
    {
        tBtampTLVHCI_Write_Connection_Accept_Timeout_Cmd WriteConnectionAcceptTimeoutCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Write_Connection_Accept_Timeout_Cmd( NULL,
                       pBuf, Count, &WriteConnectionAcceptTimeoutCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Write_Connection_Accept_Timeout_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPWriteConnectionAcceptTimeout( pctx->bapHdl,
                    &WriteConnectionAcceptTimeoutCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPWriteConnectionAcceptTimeout failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_READ_LINK_SUPERVISION_TIMEOUT_CMD:
    {
        tBtampTLVHCI_Read_Link_Supervision_Timeout_Cmd ReadLinkSupervisionTimeoutCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Read_Link_Supervision_Timeout_Cmd( NULL,
                       pBuf, Count, &ReadLinkSupervisionTimeoutCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Read_Link_Supervision_Timeout_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPReadLinkSupervisionTimeout( pctx->bapHdl,
                    &ReadLinkSupervisionTimeoutCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadLinkSupervisionTimeout failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_WRITE_LINK_SUPERVISION_TIMEOUT_CMD:
    {
        tBtampTLVHCI_Write_Link_Supervision_Timeout_Cmd WriteLinkSupervisionTimeoutCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Write_Link_Supervision_Timeout_Cmd( NULL,
                       pBuf, Count, &WriteLinkSupervisionTimeoutCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Write_Link_Supervision_Timeout_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPWriteLinkSupervisionTimeout( pctx->bapHdl,
                    &WriteLinkSupervisionTimeoutCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPWriteLinkSupervisionTimeout failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    /*                                            */
    case BTAMP_TLV_HCI_READ_LOGICAL_LINK_ACCEPT_TIMEOUT_CMD:
    {
        VosStatus = WLAN_BAPReadLogicalLinkAcceptTimeout( pctx->bapHdl, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadLogicalLinkAcceptTimeout failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_WRITE_LOGICAL_LINK_ACCEPT_TIMEOUT_CMD:
    {
        tBtampTLVHCI_Write_Logical_Link_Accept_Timeout_Cmd WriteLogicalLinkAcceptTimeoutCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Write_Logical_Link_Accept_Timeout_Cmd( NULL,
                       pBuf, Count, &WriteLogicalLinkAcceptTimeoutCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Write_Logical_Link_Accept_Timeout_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPWriteLogicalLinkAcceptTimeout( pctx->bapHdl,
                    &WriteLogicalLinkAcceptTimeoutCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPWriteLogicalLinkAcceptTimeout failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_SET_EVENT_MASK_PAGE_2_CMD:
    {
        tBtampTLVHCI_Set_Event_Mask_Page_2_Cmd SetEventMaskPage2Cmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Set_Event_Mask_Page_2_Cmd( NULL,
                       pBuf, Count, &SetEventMaskPage2Cmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Set_Event_Mask_Page_2_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPSetEventMaskPage2( pctx->bapHdl,
                                               &SetEventMaskPage2Cmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPSetEventMaskPage2 failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_READ_LOCATION_DATA_CMD:
    {
        VosStatus = WLAN_BAPReadLocationData( pctx->bapHdl, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadLocationData failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_WRITE_LOCATION_DATA_CMD:
    {
        tBtampTLVHCI_Write_Location_Data_Cmd WriteLocationDataCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Write_Location_Data_Cmd( NULL,
                       pBuf, Count, &WriteLocationDataCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Write_Location_Data_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPWriteLocationData( pctx->bapHdl,
                                               &WriteLocationDataCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPWriteLocationData failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_READ_FLOW_CONTROL_MODE_CMD:
    {
        VosStatus = WLAN_BAPReadFlowControlMode( pctx->bapHdl, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadFlowControlMode failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_WRITE_FLOW_CONTROL_MODE_CMD:
    {
        tBtampTLVHCI_Write_Flow_Control_Mode_Cmd WriteFlowControlModeCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Write_Flow_Control_Mode_Cmd( NULL,
                       pBuf, Count, &WriteFlowControlModeCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Write_Flow_Control_Mode_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPWriteFlowControlMode( pctx->bapHdl,
                    &WriteFlowControlModeCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPWriteFlowControlMode failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_READ_BEST_EFFORT_FLUSH_TIMEOUT_CMD:
    {
        tBtampTLVHCI_Read_Best_Effort_Flush_Timeout_Cmd ReadBestEffortFlushTimeoutCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Read_Best_Effort_Flush_Timeout_Cmd( NULL,
                       pBuf, Count, &ReadBestEffortFlushTimeoutCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Read_Best_Effort_Flush_Timeout_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPReadBestEffortFlushTimeout( pctx->bapHdl,
                    &ReadBestEffortFlushTimeoutCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadBestEffortFlushTimeout failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_WRITE_BEST_EFFORT_FLUSH_TIMEOUT_CMD:
    {
        tBtampTLVHCI_Write_Best_Effort_Flush_Timeout_Cmd WriteBestEffortFlushTimeoutCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Write_Best_Effort_Flush_Timeout_Cmd( NULL,
                       pBuf, Count, &WriteBestEffortFlushTimeoutCmd);

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Write_Best_Effort_Flush_Timeout_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPWriteBestEffortFlushTimeout( pctx->bapHdl,
                    &WriteBestEffortFlushTimeoutCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPWriteBestEffortFlushTimeout failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    /*                                                                 */
    case BTAMP_TLV_HCI_SET_SHORT_RANGE_MODE_CMD:
    {
        tBtampTLVHCI_Set_Short_Range_Mode_Cmd SetShortRangeModeCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Set_Short_Range_Mode_Cmd( NULL,
                       pBuf, Count, &SetShortRangeModeCmd);

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Set_Short_Range_Mode_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPSetShortRangeMode( pctx->bapHdl,
                                               &SetShortRangeModeCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPSetShortRangeMode failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    /*                                                   */
    /*
                               
    */
    case BTAMP_TLV_HCI_READ_LOCAL_VERSION_INFO_CMD:
    {
        VosStatus = WLAN_BAPReadLocalVersionInfo( pctx->bapHdl, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadLocalVersionInfo failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_READ_LOCAL_SUPPORTED_CMDS_CMD:
    {
        VosStatus = WLAN_BAPReadLocalSupportedCmds( pctx->bapHdl, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadLocalSupportedCmds failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_READ_BUFFER_SIZE_CMD:
    {
        VosStatus = WLAN_BAPReadBufferSize( pctx->bapHdl, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadBufferSize failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    /*                             */
    case BTAMP_TLV_HCI_READ_DATA_BLOCK_SIZE_CMD:
    {
        VosStatus = WLAN_BAPReadDataBlockSize( pctx->bapHdl, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadDataBlockSize failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    /*
                       
    */
    case BTAMP_TLV_HCI_READ_FAILED_CONTACT_COUNTER_CMD:
    {
        tBtampTLVHCI_Read_Failed_Contact_Counter_Cmd ReadFailedContactCounterCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Read_Failed_Contact_Counter_Cmd( NULL,
                       pBuf, Count, &ReadFailedContactCounterCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Read_Failed_Contact_Counter_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPReadFailedContactCounter( pctx->bapHdl,
                    &ReadFailedContactCounterCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadFailedContactCounter failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_RESET_FAILED_CONTACT_COUNTER_CMD:
    {
        tBtampTLVHCI_Reset_Failed_Contact_Counter_Cmd ResetFailedContactCounterCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Reset_Failed_Contact_Counter_Cmd( NULL,
                       pBuf, Count, &ResetFailedContactCounterCmd);

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Reset_Failed_Contact_Counter_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPResetFailedContactCounter( pctx->bapHdl,
                    &ResetFailedContactCounterCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPResetFailedContactCounter failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_READ_LINK_QUALITY_CMD:
    {
        tBtampTLVHCI_Read_Link_Quality_Cmd ReadLinkQualityCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Read_Link_Quality_Cmd( NULL,
                       pBuf, Count, &ReadLinkQualityCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Read_Link_Quality_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPReadLinkQuality( pctx->bapHdl,
                                             &ReadLinkQualityCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadLinkQuality failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_READ_RSSI_CMD:
    {
        tBtampTLVHCI_Read_RSSI_Cmd ReadRssiCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Read_RSSI_Cmd( NULL,
                       pBuf, Count, &ReadRssiCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Read_RSSI_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPReadRSSI( pctx->bapHdl,
                                      &ReadRssiCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadRSSI failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_READ_LOCAL_AMP_INFORMATION_CMD:
    {
        tBtampTLVHCI_Read_Local_AMP_Information_Cmd ReadLocalAmpInformationCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Read_Local_AMP_Information_Cmd( NULL,
                       pBuf, Count, &ReadLocalAmpInformationCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Read_Local_AMP_Information_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPReadLocalAMPInfo( pctx->bapHdl,
                                              &ReadLocalAmpInformationCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadLocalAMPInfo failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_READ_LOCAL_AMP_ASSOC_CMD:
    {
        tBtampTLVHCI_Read_Local_AMP_Assoc_Cmd ReadLocalAmpAssocCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Read_Local_AMP_Assoc_Cmd( NULL,
                       pBuf, Count, &ReadLocalAmpAssocCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Read_Local_AMP_Assoc_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPReadLocalAMPAssoc( pctx->bapHdl,
                                               &ReadLocalAmpAssocCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadLocalAMPAssoc failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_WRITE_REMOTE_AMP_ASSOC_CMD:
    {
        tBtampTLVHCI_Write_Remote_AMP_ASSOC_Cmd WriteRemoteAmpAssocCmd;
        //       

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: HCI_Write_Remote_AMP_ASSOC_Cmd Count = %d", Count);
        DUMPLOG(1, __func__, "HCI_Write_Remote_AMP_ASSOC cmd",
                pBuf,
                Count);

        UnpackStatus = btampUnpackTlvHCI_Write_Remote_AMP_ASSOC_Cmd( NULL,
                       pBuf, Count, &WriteRemoteAmpAssocCmd );

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WriteRemoteAmpAssocCmd.amp_assoc_remaining_length = %d",
                   WriteRemoteAmpAssocCmd.amp_assoc_remaining_length
                 );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Write_Remote_AMP_ASSOC_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

//                     
#ifdef BAP_UNIT_TEST
        {
            unsigned char test_amp_assoc_fragment[] =
            {
                0x01, 0x00, 0x06, 0x00, 0x00, 0xde, 0xad, 0xbe,
                0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
                0x0c, 0x00, 0x55, 0x53, 0x20, 0xc9, 0x0c, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x03, 0x00, 0x06, 0x00, 0x55, 0x53,
                0x20, 0xc9, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x04, 0x00, 0x04, 0x00, 0x03, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x06, 0x00,
                0x00, 0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00
            };
            WriteRemoteAmpAssocCmd.present = 1;
            WriteRemoteAmpAssocCmd.phy_link_handle = 1;
            WriteRemoteAmpAssocCmd.length_so_far = 0;
            WriteRemoteAmpAssocCmd.amp_assoc_remaining_length = 74;
            /*                                                               
                       
             */
            vos_mem_copy(
                WriteRemoteAmpAssocCmd.amp_assoc_fragment,
                test_amp_assoc_fragment,
                sizeof( test_amp_assoc_fragment));

        }
#endif

        VosStatus = WLAN_BAPWriteRemoteAMPAssoc( pctx->bapHdl,
                    &WriteRemoteAmpAssocCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPWriteRemoteAMPAssoc failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    /*
                    
    */
    case BTAMP_TLV_HCI_READ_LOOPBACK_MODE_CMD:
    {
        tBtampTLVHCI_Read_Loopback_Mode_Cmd ReadLoopbackModeCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Read_Loopback_Mode_Cmd( NULL,
                       pBuf, Count, &ReadLoopbackModeCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Read_Loopback_Mode_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPReadLoopbackMode( pctx->bapHdl,
                                              &ReadLoopbackModeCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPReadLoopbackMode failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_WRITE_LOOPBACK_MODE_CMD:
    {
        tBtampTLVHCI_Write_Loopback_Mode_Cmd WriteLoopbackModeCmd;
        //       
        UnpackStatus = btampUnpackTlvHCI_Write_Loopback_Mode_Cmd( NULL,
                       pBuf, Count, &WriteLoopbackModeCmd );

        if ( !BTAMP_SUCCEEDED( UnpackStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: btampUnpackTlvHCI_Write_Loopback_Mode_Cmd failed status %d", UnpackStatus);
            //                 
            return(FALSE);
        }

        VosStatus = WLAN_BAPWriteLoopbackMode( pctx->bapHdl,
                                               &WriteLoopbackModeCmd, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPWriteLoopbackMode failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_VENDOR_SPECIFIC_CMD_0:
    {
        VosStatus = WLAN_BAPVendorSpecificCmd0( pctx->bapHdl, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPVendorSpecificCmd0 failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    case BTAMP_TLV_HCI_VENDOR_SPECIFIC_CMD_1:
    {
        VosStatus = WLAN_BAPVendorSpecificCmd1( pctx->bapHdl, &HCIEvt );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLAN_BAPVendorSpecificCmd1 failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }

        break;
    }
    default:
    {
        /*                                                                              */

        /*                                              */
        HCIEvt.bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_STATUS_EVENT;
        HCIEvt.u.btampCommandStatusEvent.present = 1;
        HCIEvt.u.btampCommandStatusEvent.status = WLANBAP_ERROR_UNKNOWN_HCI_CMND;
        HCIEvt.u.btampCommandStatusEvent.num_hci_command_packets = 1;
        HCIEvt.u.btampCommandStatusEvent.command_opcode
        = cmdOpcode;

        //                                                         
        //                                                         
        //                                                             
        //                                                              
        //                                                  
        VosStatus = WLANBAP_EventCB( pctx, &HCIEvt, FALSE );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BslProcessHCICommand: WLANBAP_EventCB failed status %d", VosStatus);
            //                 
            return(FALSE);
        }


        break;
    }
    }

    return(TRUE);
} //                       


/* 
                                                                                     

                                              
                                                                         
                                                         

         
                                 

*/
#define BTAMP_USE_VOS_WRAPPER
//                            
#ifdef BTAMP_USE_VOS_WRAPPER
static BOOL BslProcessACLDataTx
(
    BslClientCtxType* pctx,
    struct sk_buff *skb,
    v_SIZE_t* pCount
)
#else
static BOOL BslProcessACLDataTx
(
    BslClientCtxType* pctx,
    LPCVOID pBuffer_,
    v_SIZE_t* pCount
)
#endif
{
#ifndef BTAMP_USE_VOS_WRAPPER
    LPVOID pBuffer = (LPVOID) pBuffer_; //                           
#endif
    BOOL findPhyStatus;
    BslPhyLinkCtxType* pPhyCtx;
    VOS_STATUS VosStatus = VOS_STATUS_SUCCESS;
    WLANTL_ACEnumType Ac;
    hdd_list_node_t* pLink;
    BslTxListNodeType *pNode;
    v_SIZE_t ListSize;
    //                                       
#ifndef BTAMP_USE_VOS_WRAPPER
    struct sk_buff *skb;
#endif
#if 0
    static int num_packets;
#endif

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_LOW, "BslProcessACLDataTx\n" );

    //                                                                         
    //                                  
    //                                                                        
    //                                                                    
    //                               
    pPhyCtx = &BslPhyLinkCtx[0];
    findPhyStatus = VOS_TRUE;

    if ( findPhyStatus )
    {
        //                                                       
        pNode = (BslTxListNodeType *) &skb->cb;

        //                                         
        pNode->skb = skb;

        //                            
        pLink = (hdd_list_node_t *) pNode;
        VosStatus = WLANBAP_GetAcFromTxDataPkt(pctx->bapHdl, skb->data, &Ac);
        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "BslProcessACLDataTx WLANBAP_GetAcFromTxDataPkt "
                 "failed status =%d\n", VosStatus );

            Ac = WLANTL_AC_BE;
        }

        //                                         
        //                                                                                              
        spin_lock_bh(&pPhyCtx->ACLTxQueue[Ac].lock);
        VosStatus = hdd_list_insert_back( &pPhyCtx->ACLTxQueue[Ac], pLink );
        spin_unlock_bh(&pPhyCtx->ACLTxQueue[Ac].lock);

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_ASSERT(0);
        }

        //                                                      
        hdd_list_size( &pPhyCtx->ACLTxQueue[Ac], &ListSize );

        if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
        {
            VOS_ASSERT(0);
        }

        if ( ListSize == 1 )
        {
            //                                                 
            VosStatus = WLANBAP_STAPktPending( pctx->bapHdl, pPhyCtx->PhyLinkHdl, Ac );

            if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
            {
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "BslProcessACLDataTx WLANBAP_STAPktPending "
                "failed status =%d\n", VosStatus );
                VOS_ASSERT(0);
            }
        }

        return(TRUE);
    }
    else
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "BslProcessACLDataTx attempting to send "
          "data for a non-existant assocation\n" );

        return(FALSE);
    }


} //                      


static inline void *hci_get_drvdata(struct hci_dev *hdev)
{
    return hdev->driver_data;
}

static inline void hci_set_drvdata(struct hci_dev *hdev, void *data)
{
    hdev->driver_data = data;
}

/*                                                                           
                         
                                                                           */

/*                                                                            

                                                         

                                                                      
                                  
                                                                                 

              

                                              

                                                                            */
//                         
int BSL_Init ( v_PVOID_t  pvosGCtx )
{
    BslClientCtxType* pctx = NULL;
    ptBtampHandle bapHdl = NULL;        //                  
    //                                 
    int err = 0;
    struct hci_dev *hdev = NULL;
    //                                                                
    hdd_adapter_t *pAdapter = NULL;  //                                        
    hdd_context_t *pHddCtx = NULL;
    hdd_config_t *pConfig = NULL;
    hdd_adapter_list_node_t *pAdapterNode = NULL;
    VOS_STATUS status;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BSL_Init");

    /*                                                                        
                                                     
                                                                             */
    //                                                                                            
    pctx = &BslClientCtx[0];

    bapHdl = vos_get_context( VOS_MODULE_ID_BAP, pvosGCtx);
    if ( NULL == bapHdl )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Invalid BAP pointer from pvosGCtx on BSL_Init");
        return VOS_STATUS_E_FAULT;
    }
    //                                               
    gpCtx = pvosGCtx;

    /*                                                                           */
    pctx->bapHdl = bapHdl;

    /*                                                                        */
    gpBslctx = pctx;

    /*                                            */
    BslInitPhyCtx();

    /*                                                    */
    //                                       
    pctx->rx_count = 0;
    pctx->rx_skb = NULL;

    /*                                                             */
    //                     
    //                    
    //                                                                        

    pctx->hdev = NULL;
    //                    
    pHddCtx = (hdd_context_t *)vos_get_context( VOS_MODULE_ID_HDD, pvosGCtx );
    if(NULL != pHddCtx)
    {
        pConfig = pHddCtx->cfg_ini;
    }
    if(NULL == pConfig)
    {
        VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
                  "Didn't register as HCI device");
        return 0;
    }
    else if(0 == pConfig->enableBtAmp)
    {
        VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
                  "Didn't register as HCI device, user option(gEnableBtAmp) is set to 0");
        return 0;
    }

    if (VOS_STA_SAP_MODE == hdd_get_conparam())
    {
        status = hdd_get_front_adapter ( pHddCtx, &pAdapterNode );
        if ( NULL != pAdapterNode && VOS_STATUS_SUCCESS == status )
        {
            if ( WLAN_HDD_SOFTAP == pAdapterNode->pAdapter->device_mode)
            {
                pAdapter = hdd_get_adapter(pHddCtx, WLAN_HDD_SOFTAP);
            }
            else if (WLAN_HDD_P2P_GO == pAdapterNode->pAdapter->device_mode)
            {
                pAdapter = hdd_get_adapter(pHddCtx, WLAN_HDD_P2P_GO);
            }
        }
     }
    else
        pAdapter = hdd_get_adapter(pHddCtx, WLAN_HDD_INFRA_STATION);


    if ( NULL == pAdapter )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Invalid HDD Adapter pointer from pvosGCtx on BSL_Init");
        return VOS_STATUS_E_FAULT;
    }

    /*                                                                       */
    pctx->p_dev = pAdapter->dev;

    /*                       */
    hdev = hci_alloc_dev();
    if (!hdev)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Can't allocate HCI device in BSL_Init");
        return VOS_STATUS_E_FAULT;
    }

    /*                                                            */
    pctx->hdev = hdev;

#if defined HCI_80211 || defined HCI_AMP
#define BUILD_FOR_BLUETOOTH_NEXT_2_6
#else
#undef BUILD_FOR_BLUETOOTH_NEXT_2_6
#endif

#ifdef BUILD_FOR_BLUETOOTH_NEXT_2_6
    /*                                            */
    hdev->bus = HCI_VIRTUAL;
    /*                                   */
#ifdef HCI_80211
    hdev->dev_type = HCI_80211;
#else
    hdev->dev_type = HCI_AMP;
#endif
#ifdef FEATURE_WLAN_BTAMP_UT
    /*                                                                           */
    set_bit(HCI_QUIRK_RAW_DEVICE, &hdev->quirks);
#endif
#else //                            
    /*                                            */
    hdev->type = HCI_VIRTUAL;
    /*                                   */
    //                           
    //                           
    /*                                                                          */
    set_bit(HCI_QUIRK_RAW_DEVICE, &hdev->quirks);
#endif //                            
    /*                                                            */

    hci_set_drvdata(hdev, pctx);
    /*                                                                         */
    SET_HCIDEV_DEV(hdev, &pctx->p_dev->dev);

    hdev->open     = BSL_Open;
    hdev->close    = BSL_Close;
    hdev->flush    = BSL_Flush;
    hdev->send     = BSL_Write;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
    hdev->destruct = BSL_Destruct;
    hdev->owner = THIS_MODULE;
#endif
    hdev->ioctl    = BSL_IOControl;


    /*                                                        */
    msleep(1000);

    /*                     */
    err = hci_register_dev(hdev);
    if (err < 0)
    {
        VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                  "Unable to register HCI device, err=%d\n", err);
        pctx->hdev = NULL;
        hci_free_dev(hdev);
        return -ENODEV;
    }

    pHddCtx->isAmpAllowed = VOS_TRUE;
    return 0;
} //           

/*                                                                            

                                                              

                                                                                    

              

                                              

                                                                            */

int BSL_Deinit( v_PVOID_t  pvosGCtx )
{
    //            
    struct hci_dev *hdev;
    BslClientCtxType* pctx = NULL;

    //                                                     
    pctx = gpBslctx;

    if ( NULL == pctx )
    {
        //                                                    
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Invalid BSL pointer from pvosGCtx on BSL_Init");
        return VOS_STATUS_E_FAULT;
    }

    /*                                                             */
    hdev = pctx->hdev;

    if (!hdev)
        return 0;

    /*                                                                      */
    /*                                                                           */
    /*                                                                               */
    hci_unregister_dev(hdev);
    /*                                                              */
    BSL_Close(hdev);
    hci_free_dev(hdev);
    pctx->hdev = NULL;

    return 0;
} //             


/* 
                                                                            
                                                                           
                                                  

                                                               
                                                                
                                    

         
                                                                    

                                                   
*/
//                                                             
static int BSL_Open( struct hci_dev *hdev )
{
    VOS_STATUS VosStatus = VOS_STATUS_SUCCESS;
    BslClientCtxType* pctx = (BslClientCtxType *)(hci_get_drvdata(hdev));
    v_U16_t i;
    BOOL rval;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BSL_Open");

    /*                                            */
    if (bBslInited)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "BSL_Open: Already Opened.");
        return -EPERM; /*                         */
    }

    if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BSLClientLock already inited");
        //                              
        return 0;
    }

    VosStatus = vos_list_init( &BslPhyLinksDescPool );

    if ( !VOS_IS_STATUS_SUCCESS( VosStatus ) )
    {
        //                             
        return 0;
    }

    //                                                                        
    for ( i=0; i<BSL_MAX_PHY_LINKS; i++ )
    {
        VosStatus = vos_list_insert_front( &BslPhyLinksDescPool, &BslPhyLinksDesc[i].node );
        VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ) );
    }

    //                                                                      
    bBslInited = TRUE;

    rval = BslFindAndInitClientCtx( &pctx );

    if(rval != TRUE)
    {
        //                                                                       
        //       
        //                             
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BSLFindAndInitClientContext failed");
        return 0;
    }


    /*                                                */
    return 0;
} //           

/* 
                                                                       
                                                                    

                                                               
                                                                
                                    

         
                                                  
*/
//                                                               
static int BSL_Close ( struct hci_dev *hdev )
{
    VOS_STATUS VosStatus = VOS_STATUS_SUCCESS;
    BslClientCtxType* pctx;
    vos_list_node_t* pLink;
    v_U16_t i;
    v_CONTEXT_t pVosContext = vos_get_global_context( VOS_MODULE_ID_HDD, NULL );
    hdd_context_t *pHddCtx;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BSL_Close");
    if (NULL != pVosContext)
    {
       pHddCtx = vos_get_context( VOS_MODULE_ID_HDD, pVosContext);
       if (NULL != pHddCtx)
       {
          pHddCtx->isAmpAllowed = VOS_FALSE;
       }
    }

    //                                                                 
    //                                                                    
    //                                                                    
    //                                                                          
    //                 
    if ( hdev == NULL )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BSL_Close: NULL hdev specified");
        return FALSE;
    }

    pctx = (BslClientCtxType *)(hci_get_drvdata(hdev));

    if ( pctx == NULL || !bBslInited)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_LOW, "BSL_Close: %s is not open", hdev->name);
        return TRUE;
    }

    //                                                          
    BslReleaseClientCtx( pctx );
    for ( i=0; i<BslPhyLinksDescPool.count; i++ )
    {
        VosStatus = vos_list_remove_front( &BslPhyLinksDescPool, &pLink );
        //                                                                         
        //                                                        
    }
    VosStatus = vos_list_destroy( &BslPhyLinksDescPool );

    VOS_ASSERT(VOS_IS_STATUS_SUCCESS( VosStatus ) );


    bBslInited = FALSE;

//                           
    return(0);
} //           

/* 
                                                                     

                                                               
                                                                     
                                                                              
                                               

         
                                                  
*/
//                                                                                  
static int BSL_IOControl(struct hci_dev *hdev, unsigned int cmd, unsigned long arg)
{
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BSL_IOControl - not supported");
    return(TRUE);
} //                

/* 
                                                                              

                                                               

         
                                                  
*/
static int BSL_Flush(struct hci_dev *hdev)
{
    VOS_STATUS VosStatus;
    BslPhyLinkCtxType* pPhyCtx;

    //                                                                                                 
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "%s - will flush ALL Tx Queues", __func__);

    /*                    */
    //                               
    pPhyCtx = &BslPhyLinkCtx[0];

    VosStatus = BslFlushTxQueues ( pPhyCtx);

    //             
    return(0);
} //            

/* 
                                                               

                                                               

         
                                                  
*/
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
static void BSL_Destruct(struct hci_dev *hdev)
{
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BSL_Destruct - not supported");
    return; //       
} //               
#endif


/* 
                                                               
                                                                            
                                                

                                                                            
                                                                              

         
                                                
                                            
*/
//                                                                        
//                                                    
static int BSL_Write(struct sk_buff *skb)
{
    struct hci_dev *hdev;
    BslClientCtxType* pctx;
    v_SIZE_t written = 0;
    BOOL status;
    //                     
    BslHciWorkStructure *pHciContext;

    //                                                                         

    //                    
    if ( skb == NULL )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: skb is bad i/p", __func__);
        //                                 
        return 0;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: Count (skb->len)=%d", __func__, skb->len);

    //                    
    if ( 0 == skb->len )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: skb is empty", __func__);
        //                                 
        return 0;
    }

    hdev = (struct hci_dev *)(skb->dev);

    //                                       
    if ( hdev == NULL )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: Frame for Unknown HCI device (hdev=NULL)", __func__);
        //                               
        return 0;
    }

    pctx = (BslClientCtxType *)hci_get_drvdata(hdev);

    //                    
    if ( pctx == NULL )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: pctx is bad i/p", __func__);
        //                                 
        return 0;
        /*                                   */
        //                               
    }

    //                                    
    switch (bt_cb(skb)->pkt_type)
    {
    case HCI_ACLDATA_PKT:
        //                                
        VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                  "%s: HCI ACL data tx, skb=%p",
                  __func__, skb);
        //         
        hdev->stat.acl_tx++;
        //                             
        written = skb->len;
#ifdef BTAMP_USE_VOS_WRAPPER
        status = BslProcessACLDataTx( pctx, skb, &written );
#else
        status = BslProcessACLDataTx( pctx, skb->data, &written );
        //                
        kfree_skb(skb);
#endif //                     
        break;
    case HCI_COMMAND_PKT:
        //                             
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: HCI command", __func__);
        hdev->stat.cmd_tx++;

        //                                                                                
        pHciContext = kmalloc(sizeof(*pHciContext), GFP_ATOMIC);
        if (NULL == pHciContext)
        {
            //                                                       
            VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                      "%s: Unable to allocate context", __func__);
            kfree_skb(skb);
            return 0;
        }

        //                                                       
        //                                 
        pHciContext->tx_skb = skb;
        //                                                                           
        pHciContext->pctx = pctx;
        pHciContext->magic = BT_AMP_HCI_CTX_MAGIC;
        INIT_WORK(&pHciContext->hciInterfaceProcessing,
                  bslWriteFinish);

        VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
                  "%s: Scheduling work for skb %p, BT-AMP Client context %p, work %p",
                  __func__, skb, pctx, pHciContext);

        status = schedule_work(&pHciContext->hciInterfaceProcessing);

        //             
        if ( 0 == status )
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "%s: hciInterfaceProcessing work already queued. This should never happen.", __func__);
        }


        //                            
        //                                             
        written = skb->len;
        break;
    case HCI_SCODATA_PKT:
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: unknown type", __func__);
        hdev->stat.sco_tx++;
        //                                                
        status = FALSE;
        //                
        kfree_skb(skb);
        break;
    default:
        //                                                
        status = FALSE;
        //                
        kfree_skb(skb);
        break;
    };


    //                                                                                            
    //                                          

//                           
    //                   
    return(written);
} //            

/* 
                                                                       
                         

                                                   

                        

*/
static void bslWriteFinish(struct work_struct *work)
{
    //                        
    //                                                                 
    BslHciWorkStructure *pHciContext =
        container_of(work, BslHciWorkStructure, hciInterfaceProcessing);
    BslClientCtxType* pctx = pHciContext->pctx;
    VOS_STATUS status;
    struct sk_buff *skb;
    struct hci_dev *hdev;
    //                     
    v_SIZE_t written = 0;

    VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_INFO_LOW,
              "%s: Entered, context %p",
              __func__, pctx);

    //                    
    if ( pctx == NULL )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: pctx is bad i/p", __func__);
        return; //                           
    }

    //                   
    skb = pHciContext->tx_skb;
    kfree( pHciContext);

    //                    
    if ( skb == NULL )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: skb is bad i/p", __func__);
        return; //                           
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: Count (skb->len)=%d", __func__, skb->len);

    hdev = (struct hci_dev *)(skb->dev);

    //                                       
    if ( hdev == NULL )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: Frame for Unknown HCI device (hdev=NULL)", __func__);
        return; //                         
    }


    //                    
    if ( pctx != (BslClientCtxType *)hci_get_drvdata(hdev));
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: pctx and hdev not consistent - bad i/p", __func__);
        return; //                           
        /*                                   */
        //                               
    }

    //                                    
    switch (bt_cb(skb)->pkt_type)
    {
    case HCI_COMMAND_PKT:
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: HCI command", __func__);
        hdev->stat.cmd_tx++;
        //            
        status = BslProcessHCICommand( pctx, skb->data, skb->len-CMD_TLV_TYPE_AND_LEN_SIZE);
        //                            
        //                                             
        written = skb->len;
        break;
    case HCI_SCODATA_PKT:
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: unknown type", __func__);
        hdev->stat.sco_tx++;
        //                                                
        status = FALSE;
        break;
    default:
        //                                                
        status = FALSE;
        break;
    };

    VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
              "%s: Freeing skb %p",
              __func__, skb);

    consume_skb(skb);

//                                                                  
//                   
    return;
} //                

VOS_STATUS WLANBAP_SetConfig
(
    WLANBAP_ConfigType *pConfig
)
{
    BslClientCtxType* pctx;
    VOS_STATUS status;
    //                
    if ( pConfig == NULL )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_SetConfig bad input\n" );
        return VOS_STATUS_E_FAILURE;
    }
    pctx = gpBslctx;
    if ( NULL == pctx )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Invalid BSL pointer from pctx on WLANBAP_SetConfig");
        return VOS_STATUS_E_FAULT;
    }

    //                      
    status = WLANBAP_GetNewHndl(&pctx->bapHdl);
    if ( !VOS_IS_STATUS_SUCCESS( status ) )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_SetConfig can't get BAP handle\n" );
        return VOS_STATUS_E_FAILURE;
    }


    status = WLAN_BAPSetConfig(pctx->bapHdl, pConfig);
    if ( !VOS_IS_STATUS_SUCCESS( status ) )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "WLANBAP_SetConfig can't set BAP config\n" );
        return VOS_STATUS_E_FAILURE;
    }

    return(VOS_STATUS_SUCCESS);
}

VOS_STATUS WLANBAP_RegisterWithHCI(hdd_adapter_t *pAdapter)
{
    struct hci_dev *hdev = NULL;
    BslClientCtxType* pctx = NULL;
    int err = 0;
    v_CONTEXT_t pVosContext = vos_get_global_context( VOS_MODULE_ID_HDD, NULL );
    hdd_context_t *pHddCtx;

    pctx = gpBslctx;

    if ( NULL == pctx )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Invalid BSL pointer from pctx on WLANBAP_RegisterWithHCI");
        return VOS_STATUS_E_FAULT;
    }
    if ( NULL == pAdapter )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Invalid HDD Adapter pointer from pvosGCtx on WLANBAP_RegisterWithHCI");
        return VOS_STATUS_E_FAULT;
    }

    if(NULL != pctx->hdev)
    {
        VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_WARN,
                  "Already registered as HCI device\n");
        return VOS_STATUS_SUCCESS;
    }



    /*                                                                       */
    pctx->p_dev = pAdapter->dev;

    /*                       */
    hdev = hci_alloc_dev();
    if (!hdev)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Can't allocate HCI device in WLANBAP_RegisterWithHCI");
        return VOS_STATUS_E_FAULT;
    }

    /*                                                            */
    pctx->hdev = hdev;

#if defined HCI_80211 || defined HCI_AMP
#define BUILD_FOR_BLUETOOTH_NEXT_2_6
#else
#undef BUILD_FOR_BLUETOOTH_NEXT_2_6
#endif

#ifdef BUILD_FOR_BLUETOOTH_NEXT_2_6
    /*                                            */
    hdev->bus = HCI_VIRTUAL;
    /*                                   */
#ifdef HCI_80211
    hdev->dev_type = HCI_80211;
#else
    hdev->dev_type = HCI_AMP;
#endif
#ifdef FEATURE_WLAN_BTAMP_UT
    /*                                                                           */
    set_bit(HCI_QUIRK_RAW_DEVICE, &hdev->quirks);
#endif
#else //                            
    /*                                            */
    hdev->type = HCI_VIRTUAL;
    /*                                   */
    //                           
    //                           
    /*                                                                          */
    set_bit(HCI_QUIRK_RAW_DEVICE, &hdev->quirks);
#endif //                            
    /*                                                            */
    hci_set_drvdata(hdev, pctx);
    /*                                                                         */
    SET_HCIDEV_DEV(hdev, &pctx->p_dev->dev);

    hdev->open     = BSL_Open;
    hdev->close    = BSL_Close;
    hdev->flush    = BSL_Flush;
    hdev->send     = BSL_Write;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))
    hdev->owner = THIS_MODULE;
    hdev->destruct = BSL_Destruct;
#endif
    hdev->ioctl    = BSL_IOControl;


    /*                                                        */
    msleep(1000);

    /*                     */
    err = hci_register_dev(hdev);
    if (err < 0)
    {
        VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                  "Unable to register HCI device, err=%d\n", err);
        pctx->hdev = NULL;
        hci_free_dev(hdev);
        return VOS_STATUS_E_FAULT;
    }
    if (NULL != pVosContext)
    {
       pHddCtx = vos_get_context( VOS_MODULE_ID_HDD, pVosContext);
       if (NULL != pHddCtx)
       {
          pHddCtx->isAmpAllowed = VOS_TRUE;
       }
    }

    return VOS_STATUS_SUCCESS;
}

VOS_STATUS WLANBAP_DeregisterFromHCI(void)
{
    struct hci_dev *hdev;
    BslClientCtxType* pctx = NULL;

    pctx = gpBslctx;

    if ( NULL == pctx )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Invalid BSL pointer from pvosGCtx on WLANBAP_DeregisterFromHCI");
        return VOS_STATUS_E_FAULT;
    }

    /*                                                             */
    hdev = pctx->hdev;

    if (!hdev)
        return VOS_STATUS_E_FAULT;

    /*                                                                           */
    /*                                                                               */
    hci_unregister_dev(hdev);

    /*                                                              */
    BSL_Close(hdev);
    hci_free_dev(hdev);
    pctx->hdev = NULL;

    return VOS_STATUS_SUCCESS;
}

VOS_STATUS WLANBAP_StopAmp(void)
{
    BslClientCtxType* pctx;
    VOS_STATUS status = VOS_STATUS_SUCCESS;

    pctx = gpBslctx;

    if(NULL == pctx)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Invalid BSL pointer from pvosGCtx on WLANBAP_StopAmp");
        status = VOS_STATUS_E_FAULT;
    }
    else
    {
        //                                   
        if(VOS_TRUE == WLAN_BAPSessionOn(pctx->bapHdl))
        {
            status = WLAN_BAPDisconnect(pctx->bapHdl);
        }
    }
    return status;
}

v_BOOL_t WLANBAP_AmpSessionOn(void)
{
    BslClientCtxType* pctx;

    pctx = gpBslctx;
    if(NULL == pctx)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                   "Invalid BSL pointer from pvosGCtx on WLANBAP_AmpSessionOn");
        return VOS_FALSE;
    }
    else
    {
        return( WLAN_BAPSessionOn(pctx->bapHdl));
    }
}


#endif //                   

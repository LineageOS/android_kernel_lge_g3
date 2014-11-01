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
/*============================================================================
  FILE:         vos_diag.c

  OVERVIEW:     This source file contains definitions for vOS diag APIs

  DEPENDENCIES: 
 
                Copyright (c) 2007 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
============================================================================*/

#include "vos_types.h"
#include "i_vos_diag_core_log.h"
#include "i_vos_diag_core_event.h"
#include "wlan_hdd_main.h"
#include "wlan_nlink_common.h"
#include "vos_sched.h"
#include "wlan_ptt_sock_svc.h"


#define PTT_MSG_DIAG_CMDS_TYPE   0x5050

#define DIAG_TYPE_LOGS   1 
#define DIAG_TYPE_EVENTS 2

#define DIAG_SWAP16(A) ((((tANI_U16)(A) & 0xff00) >> 8) | (((tANI_U16)(A) & 0x00ff) << 8))



typedef struct event_report_s
{
    v_U32_t diag_type;
    v_U16_t event_id;
    v_U16_t length;
} event_report_t;


/*                                                                            
  
                              

                                                                

                                                 
                                
                
  
                                                                            */

void vos_log_set_code (v_VOID_t *ptr, v_U16_t code)
{
    if (ptr)
    {
        /*                                                               */
        ((log_hdr_type *) ptr)->code = code;
    }

} 

/*                                                                            
  
                                

                                                               

                                                 
                                    
              
                
  
                                                                            */

void vos_log_set_length (v_VOID_t *ptr, v_U16_t length)
{
    if(ptr)
    {
        /*                                                               */
        ((log_hdr_type *) ptr)->len = (v_U16_t) length;
    }
} 

/*                                                                            
  
                            

                                                                                                   

                                                 
              
                
  
                                                                            */

void vos_log_submit(v_VOID_t *plog_hdr_ptr)
{

    log_hdr_type *pHdr = (log_hdr_type*) plog_hdr_ptr;

    tAniHdr *wmsg = NULL;
    v_U8_t *pBuf;
    struct hdd_context_s *pHddCtx;
    v_CONTEXT_t pVosContext= NULL;
    v_U16_t data_len;
    v_U16_t total_len;


     /*                       */
    pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);

     /*                    */
    pHddCtx = ((VosContextType*)(pVosContext))->pHDDContext;

#ifdef WLAN_KD_READY_NOTIFIER
    /*                                            */
    if ((pHddCtx->kd_nl_init) && (!pHddCtx->ptt_pid))
    {
        nl_srv_nl_ready_indication();
    }
#endif /*                        */

   /*                                                                               */
    if(pHddCtx->ptt_pid)
    {
        data_len = pHdr->len;
    
        total_len = sizeof(tAniHdr)+sizeof(v_U32_t)+data_len;
    
        pBuf =  (v_U8_t*)vos_mem_malloc(total_len);
    
        if(!pBuf)
        {
            VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR, "vos_mem_malloc failed\n");
            return;
        }
        
        vos_mem_zero((v_VOID_t*)pBuf,total_len);
    
        wmsg = (tAniHdr*)pBuf;
        wmsg->type = PTT_MSG_DIAG_CMDS_TYPE;
        wmsg->length = total_len;
        wmsg->length = DIAG_SWAP16(wmsg->length);
        pBuf += sizeof(tAniHdr);
    
    
            /*                          */
        *(v_U32_t*)pBuf = DIAG_TYPE_LOGS;
        pBuf += sizeof(v_U32_t);
    
    
        memcpy(pBuf, pHdr,data_len);
    
        if(pHddCtx->ptt_pid)
        {
            if( ptt_sock_send_msg_to_app(wmsg, 0, ANI_NL_MSG_PUMAC, pHddCtx->ptt_pid) < 0) {
        
                VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                          ("Ptt Socket error sending message to the app!!\n"));
                vos_mem_free((v_VOID_t *)wmsg);
                return;
            }
       
        }
        vos_mem_free((v_VOID_t*)wmsg);
    }
    return;
}

/*                                                                            
  
                                      

                                                                                                     

                                                 
              
                
  
                                                                            */

void vos_event_report_payload(v_U16_t event_Id, v_U16_t length, v_VOID_t *pPayload)
{ 


    tAniHdr *wmsg = NULL;
    v_U8_t *pBuf;
    struct hdd_context_s *pHddCtx;
    v_CONTEXT_t pVosContext= NULL;
    event_report_t *pEvent_report;
    v_U16_t total_len;

     /*                       */
    pVosContext = vos_get_global_context(VOS_MODULE_ID_SYS, NULL);

     /*                    */
    pHddCtx = ((VosContextType*)(pVosContext))->pHDDContext;

#ifdef WLAN_KD_READY_NOTIFIER
    /*                                            */
    if ((pHddCtx->kd_nl_init) && (!pHddCtx->ptt_pid))
    {
        nl_srv_nl_ready_indication();
    }
#endif /*                        */
    
    /*                                                                               */
    if(pHddCtx->ptt_pid)
    {
        total_len = sizeof(tAniHdr)+sizeof(event_report_t)+length;
        
        pBuf =  (v_U8_t*)vos_mem_malloc(total_len);
    
        if(!pBuf)
        {
            VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR, "vos_mem_malloc failed\n");
            return;
        }
        wmsg = (tAniHdr*)pBuf;
        wmsg->type = PTT_MSG_DIAG_CMDS_TYPE;
        wmsg->length = total_len;
        wmsg->length = DIAG_SWAP16(wmsg->length);
        pBuf += sizeof(tAniHdr);
    
        pEvent_report = (event_report_t*)pBuf;
        pEvent_report->diag_type = DIAG_TYPE_EVENTS;
        pEvent_report->event_id = event_Id;
        pEvent_report->length = length;
    
        pBuf += sizeof(event_report_t); 
    
        memcpy(pBuf, pPayload,length);
      
        if( ptt_sock_send_msg_to_app(wmsg, 0, ANI_NL_MSG_PUMAC, pHddCtx->ptt_pid) < 0) {
            VOS_TRACE(VOS_MODULE_ID_HDD, VOS_TRACE_LEVEL_ERROR,
                       ("Ptt Socket error sending message to the app!!\n"));
            vos_mem_free((v_VOID_t*)wmsg);
            return;
        }
    
        vos_mem_free((v_VOID_t*)wmsg);
    }
  
    return;
    
}

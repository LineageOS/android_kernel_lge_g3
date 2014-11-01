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


#if !defined( __SME_QOSAPI_H )
#define __SME_QOSAPI_H


/**=========================================================================
  
  \file  sme_QosApi.h
  
  \brief prototype for SME QoS APIs
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */
#include "vos_lock.h"
#include "vos_trace.h"
#include "vos_memory.h"
#include "vos_types.h"
#include "aniGlobal.h"
#include "sirApi.h"

/*                                                                          
                           
                                                                          */
#define SME_QOS_UAPSD_VO      0x01
#define SME_QOS_UAPSD_VI      0x02
#define SME_QOS_UAPSD_BE      0x08
#define SME_QOS_UAPSD_BK      0x04

/*                                                                           
                   
                                                                          */

/*                                                                           
                                                                            
                                                                           */
typedef enum
{
   //                                                                         
   //                                                        
   SME_QOS_STATUS_SETUP_SUCCESS_IND = 0, 
   //                                                                  
   SME_QOS_STATUS_SETUP_SUCCESS_APSD_SET_ALREADY,
   //                                                                          
   SME_QOS_STATUS_SETUP_FAILURE_RSP, 
   //    
   SME_QOS_STATUS_SETUP_INVALID_PARAMS_RSP, 
   //                                  
   SME_QOS_STATUS_SETUP_NOT_QOS_AP_RSP, 
   //                                                                 
   SME_QOS_STATUS_SETUP_REQ_PENDING_RSP, 
   //                                                                    
   //                
   //                                                                    
   SME_QOS_STATUS_SETUP_MODIFIED_IND,
   //                                 
   SME_QOS_STATUS_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP, 
   //                                                                           
   //                                                                       
   //                                                                 
   SME_QOS_STATUS_SETUP_SUCCESS_IND_APSD_PENDING, 
   //                                                                           
   //                                                                       
   //                                                         
   SME_QOS_STATUS_SETUP_SUCCESS_IND_APSD_SET_FAILED, 

   //                                                                        
   //                                             
   //                                                          
   SME_QOS_STATUS_RELEASE_SUCCESS_RSP = 100,
   //                                                                          
   SME_QOS_STATUS_RELEASE_FAILURE_RSP,
   //                               
   SME_QOS_STATUS_RELEASE_QOS_LOST_IND,
   //                                                                           
   //                     
   SME_QOS_STATUS_RELEASE_REQ_PENDING_RSP, 
   //    
   SME_QOS_STATUS_RELEASE_INVALID_PARAMS_RSP, 

   //                                                                          
   //                        
   SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_IND = 200,
   //                                                                  
   SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_APSD_SET_ALREADY,
   //                                                                          
   SME_QOS_STATUS_MODIFY_SETUP_FAILURE_RSP,
   //                                                                 
   SME_QOS_STATUS_MODIFY_SETUP_PENDING_RSP,
   //                                 
   SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_NO_ACM_NO_APSD_RSP, 
   //    
   SME_QOS_STATUS_MODIFY_SETUP_INVALID_PARAMS_RSP, 
   //                                                                           
   //                                                                       
   //                                                                 
   SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_IND_APSD_PENDING, 
   //                                                                           
   //                                                                       
   //                                                         
   SME_QOS_STATUS_MODIFY_SETUP_SUCCESS_IND_APSD_SET_FAILED,
   //                                    
   SME_QOS_STATUS_HANDING_OFF = 300,
   //                                                            
   SME_QOS_STATUS_OUT_OF_APSD_POWER_MODE_IND = 400,
   //                                                            
   SME_QOS_STATUS_INTO_APSD_POWER_MODE_IND,
   
}sme_QosStatusType;

/*                                                                           
                                                      
   
                                                                   
                                                                           */
typedef enum
{
   SME_QOS_WMM_UP_BE      = 0,
   SME_QOS_WMM_UP_BK      = 1,
   SME_QOS_WMM_UP_RESV    = 2,    /*                                       */
   SME_QOS_WMM_UP_EE      = 3,
   SME_QOS_WMM_UP_CL      = 4,
   SME_QOS_WMM_UP_VI      = 5,
   SME_QOS_WMM_UP_VO      = 6,
   SME_QOS_WMM_UP_NC      = 7,

   SME_QOS_WMM_UP_MAX

}sme_QosWmmUpType;

/*                                                                           
                                              
   
                                  
                                                                           */

typedef enum
{
   SME_QOS_WMM_TS_DIR_UPLINK   = 0,
   SME_QOS_WMM_TS_DIR_DOWNLINK = 1,
   SME_QOS_WMM_TS_DIR_RESV     = 2,   /*                                   */
   SME_QOS_WMM_TS_DIR_BOTH     = 3,

}sme_QosWmmDirType;

/*                                                                           
                                                 
   
                                
                                                                           */

typedef enum
{
   SME_QOS_WMM_TS_ACK_POLICY_NORMAL_ACK   = 0,
   SME_QOS_WMM_TS_ACK_POLICY_RESV1 = 1,
   SME_QOS_WMM_TS_ACK_POLICY_RESV2     = 2,   /*                                   */
   SME_QOS_WMM_TS_ACK_POLICY_HT_IMMEDIATE_BLOCK_ACK     = 3,

}sme_QosWmmAckPolicyType;

/*                                                                           
                                 
   
                              
                                                                           */
typedef struct
{
   v_U8_t              burst_size_defn;
   sme_QosWmmAckPolicyType    ack_policy;
   sme_QosWmmUpType    up;        /*                                  */
   v_U8_t              psb;       /*                                  */
   sme_QosWmmDirType   direction; /*                                  */
   v_U8_t              tid;       /*                                  */
} sme_QosWmmTsInfoType;

/*                                                                           
                                             
                                                                           */
typedef struct
{ 
  sme_QosWmmTsInfoType            ts_info;
  v_U16_t                         nominal_msdu_size;
  v_U16_t                         maximum_msdu_size;
  v_U32_t                         min_service_interval;
  v_U32_t                         max_service_interval;
  v_U32_t                         inactivity_interval;
  v_U32_t                         suspension_interval;
  v_U32_t                         svc_start_time;
  v_U32_t                         min_data_rate;
  v_U32_t                         mean_data_rate;
  v_U32_t                         peak_data_rate;
  v_U32_t                         max_burst_size;
  v_U32_t                         delay_bound;
  v_U32_t                         min_phy_rate;
  v_U16_t                         surplus_bw_allowance;
  v_U16_t                         medium_time;
} sme_QosWmmTspecInfo;


/*                                                                           
                                      
                                                                          */

/*                                                                          
                                                                             
                                                                               
                                                                               
                                                  
  
                                               
                                                                                 
                                                                                
                                                                                 
                                                                               
                                                                          
                                   
  
                                                           
  

     
  
                                                                            */
typedef eHalStatus (*sme_QosCallback)(tHalHandle hHal, void * HDDcontext, 
                                      sme_QosWmmTspecInfo * pCurrentQoSInfo, 
                                      sme_QosStatusType status,
                                      v_U32_t QosFlowID);   

/*                                                                          
                                                                               
                                                                           
                                                                              
                                                                            
                                                                            
                 
  
                                               
                                                                                     
                                                                                           
                                                                             
                                                                               
                                                                  
                                                                       
                                                                             
                                                                                
                                     
                                                                              
                                                                 
                                                                               
                                                          
                                                                              
                                                                          
                                                                              
                           
                                                                            
                               
                  
                                                                              
  
                                                      
     
  
                                                                            */
sme_QosStatusType sme_QosSetupReq(tHalHandle hHal, tANI_U32 sessionId,
                                  sme_QosWmmTspecInfo * pQoSInfo,
                                  sme_QosCallback QoSCallback, void * HDDcontext,
                                  sme_QosWmmUpType UPType, v_U32_t * pQosFlowID);

/*                                                                          
                                                                            
                                                                           
                                                                                
                                                                             
                                                                              
                                                                            
                                                
  
                                               
                                                                               
                                                                  
                                                                             
                           
                                                                             
                                            
                  
                                                                        
                                                      
  
                                                
     
  
                                                                            */
sme_QosStatusType sme_QosModifyReq(tHalHandle hHal, 
                                   sme_QosWmmTspecInfo * pQoSInfo,
                                   v_U32_t QosFlowID);

/*                                                                          
                                                                             
                                                                           
                                                                            
                                                                            
  
                                               
                                                                                 
                                                                            
                               
  
                                                                     
                                                        
  
     
  
                                                                            */
sme_QosStatusType sme_QosReleaseReq(tHalHandle hHal, v_U32_t QosFlowID);

/*                                                                          
                                                                             
                                                                                       
  
                                               
                                                                               
                                                 
                                                           
  
                                                                                    
                                                                       
  
     
  
                                                                            */
v_BOOL_t sme_QosIsTSInfoAckPolicyValid(tpAniSirGlobal pMac,
    sme_QosWmmTspecInfo * pQoSInfo,
    v_U8_t sessionId);


/*                                                                          
                                                                        
                                    
                               
                                                                    

     

                                                                         */
void sme_QosUpdateHandOff(v_U8_t sessionId,
     v_BOOL_t updateHandOff);

#endif //                              

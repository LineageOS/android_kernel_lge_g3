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
#ifndef _WLAN_HDD_WMM_H
#define _WLAN_HDD_WMM_H
/*============================================================================
  @file wlan_hdd_wmm.h

  This module (wlan_hdd_wmm.h interface + wlan_hdd_wmm.c implementation)
  houses all the logic for WMM in HDD.

  On the control path, it has the logic to setup QoS, modify QoS and delete
  QoS (QoS here refers to a TSPEC). The setup QoS comes in two flavors: an
  explicit application invoked and an internal HDD invoked.  The implicit QoS
  is for applications that do NOT call the custom QCT WLAN OIDs for QoS but
  which DO mark their traffic for priortization. It also has logic to start,
  update and stop the U-APSD trigger frame generation. It also has logic to
  read WMM related config parameters from the registry.

  On the data path, it has the logic to figure out the WMM AC of an egress
  packet and when to signal TL to serve a particular AC queue. It also has the
  logic to retrieve a packet based on WMM priority in response to a fetch from
  TL.

  The remaining functions are utility functions for information hiding.


               Copyright (c) 2008-9 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Confidential and Proprietary
============================================================================*/
/*          */

/*                                                                            
                
                                                                            */
#include <linux/workqueue.h>
#include <linux/list.h>
#include <wlan_hdd_main.h>
#include <wlan_hdd_wext.h>
#include <wlan_qct_tl.h>
#include <sme_QosApi.h>

/*                                                                            
                                         
                                                                            */

//                        

#define HDD_WMM_CTX_MAGIC 0x574d4d58    //       

#define HDD_WMM_HANDLE_IMPLICIT 0xFFFFFFFF

#define HDD_WLAN_INVALID_STA_ID 0xFF

/*                                                                            
                    
                                                                            */
/*                                                                               
*/
typedef enum
{
   HDD_LINUX_AC_VO = 0,
   HDD_LINUX_AC_VI = 1,
   HDD_LINUX_AC_BE = 2,
   HDD_LINUX_AC_BK = 3

} hdd_wmm_linuxac_t;
 
/*                                          
*/
typedef enum
{
   HDD_WMM_CLASSIFICATION_DSCP = 0,
   HDD_WMM_CLASSIFICATION_802_1Q = 1

} hdd_wmm_classification_t;

/*                    
*/
typedef enum
{
   HDD_WMM_NON_UAPSD = 0,
   HDD_WMM_UAPSD = 1

} hdd_wmm_uapsd_state_t;


typedef enum
{
   //                                                                        
   //                                                                    
   HDD_WMM_USER_MODE_AUTO = 0,
   //                                                                        
   HDD_WMM_USER_MODE_QBSS_ONLY = 1,
   //                                                                       
   //                                                                        
   //                                                
   HDD_WMM_USER_MODE_NO_QOS = 2,

} hdd_wmm_user_mode_t;

//                
//                                                                
#define HDD_AC_VO 0x1
#define HDD_AC_VI 0x2
#define HDD_AC_BK 0x4
#define HDD_AC_BE 0x8

/*                                       
*/
typedef struct
{
   struct list_head             node;
   v_U32_t                      handle;
   v_U32_t                      qosFlowId;
   hdd_adapter_t*               pAdapter;
   WLANTL_ACEnumType            acType;
   hdd_wlan_wmm_status_e        lastStatus;
   struct work_struct           wmmAcSetupImplicitQos;
   v_U32_t                      magic;
} hdd_wmm_qos_context_t;

/*                                               
*/
typedef struct
{
   //                                       
   v_BOOL_t                     wmmAcAccessRequired;

   //                                                          
   v_BOOL_t                     wmmAcAccessNeeded;

   //                                                    
   v_BOOL_t                     wmmAcAccessPending;

   //                                             
   v_BOOL_t                     wmmAcAccessFailed;

   //                                                
   v_BOOL_t                     wmmAcAccessGranted;

   //                                                              
   //                                                               
   //                               
   v_BOOL_t                     wmmAcAccessAllowed;

   //                             
   v_BOOL_t                     wmmAcTspecValid;

   //                                  
   v_BOOL_t                     wmmAcUapsdInfoValid;

   //                                               
   sme_QosWmmTspecInfo          wmmAcTspecInfo;

   //                          
   v_BOOL_t                     wmmAcIsUapsdEnabled;
   v_U32_t                      wmmAcUapsdServiceInterval;
   v_U32_t                      wmmAcUapsdSuspensionInterval;
   sme_QosWmmDirType            wmmAcUapsdDirection;

#ifdef FEATURE_WLAN_CCX
   //                                     
   v_U32_t                      wmmInactivityTime;
   v_U32_t                      wmmPrevTrafficCnt;
   vos_timer_t                  wmmInactivityTimer;
#endif

} hdd_wmm_ac_status_t;

/*                                
*/
typedef struct
{
   struct list_head             wmmContextList;
   struct mutex                 wmmLock;
   hdd_wmm_ac_status_t          wmmAcStatus[WLANTL_MAX_AC];
   v_BOOL_t                     wmmQap;
   v_BOOL_t                     wmmQosConnection;
} hdd_wmm_status_t;

extern const v_U8_t hdd_QdiscAcToTlAC[];
extern const v_U8_t hddWmmUpToAcMap[]; 
extern const v_U8_t hddLinuxUpToAcMap[];

/*                                                                             
                                                                             
                                                                             
                      

                                               

                                                   
                                           

                                                                             */
VOS_STATUS hdd_wmm_init ( hdd_context_t* pHddCtx );

/*                                                                             
                                                                                     
                                                                             
                      

                                                    

                                                   
                                           

                                                                             */
VOS_STATUS hdd_wmm_adapter_init( hdd_adapter_t *pAdapter );

/*                                                                             
                                                                                    
                                                                     

                                                    

                                                   
                                           

                                                                             */
VOS_STATUS hdd_wmm_adapter_close ( hdd_adapter_t* pAdapter );

/*                                                                             
                                                                           
                              

                                                         
                                                        

                                               
                                                                             */
v_U16_t hdd_wmm_select_queue(struct net_device * dev, struct sk_buff *skb);

/*                                                                             
                                                                             
                                              


                                                         
                                              

                                     
                                                                             */

v_U16_t hdd_hostapd_select_queue(struct net_device * dev, struct sk_buff *skb);



/*                                                                             
                                                                           
                                              

                                                    
                                                        
                                                             

                        
                                                                             */
v_VOID_t hdd_wmm_classify_pkt ( hdd_adapter_t* pAdapter,
                                struct sk_buff *skb,
                                WLANTL_ACEnumType* pAcType,
                                sme_QosWmmUpType* pUserPri);


/*                                                                             
                                                                          
                         

                                                    
                                                  
                                                                          
                                                 

                                                   
                                           
                                                                             */
VOS_STATUS hdd_wmm_acquire_access( hdd_adapter_t* pAdapter,
                                   WLANTL_ACEnumType acType,
                                   v_BOOL_t * pGranted );

/*                                                                             
                                                                      
                                              

                                                    
                                                     
                                     

                                                   
                                           
                                                                             */
VOS_STATUS hdd_wmm_assoc( hdd_adapter_t* pAdapter,
                          tCsrRoamInfo *pRoamInfo,
                          eCsrRoamBssType eBssType );

/*                                                                             
                                                                        
                                                  

                                                    
                                                     
                                     

                                                   
                                           
                                                                             */
VOS_STATUS hdd_wmm_connect( hdd_adapter_t* pAdapter,
                            tCsrRoamInfo *pRoamInfo,
                            eCsrRoamBssType eBssType );

/*                                                                             
                                                                     
                                                                     

                                                     
                                                                          

                                                   
                                           
                                                                             */
VOS_STATUS hdd_wmm_get_uapsd_mask( hdd_adapter_t* pAdapter,
                                   tANI_U8 *pUapsdMask );

/*                                                                             
                                                                      
                                  

                                                     

                                              
                                                   
                                                                             */
v_BOOL_t hdd_wmm_is_active( hdd_adapter_t* pAdapter );

/*                                                                             
                                                                        
                           

                                                     
                                                           
                                                      

                                           
                                                                             */
hdd_wlan_wmm_status_e hdd_wmm_addts( hdd_adapter_t* pAdapter,
                                     v_U32_t handle,
                                     sme_QosWmmTspecInfo* pTspec );

/*                                                                             
                                                                           
                           

                                                     
                                                           

                                           
                                                                             */
hdd_wlan_wmm_status_e hdd_wmm_delts( hdd_adapter_t* pAdapter,
                                     v_U32_t handle );

/*                                                                             
                                                                               
                                       

                                                     
                                                           

                                           
                                                                             */
hdd_wlan_wmm_status_e hdd_wmm_checkts( hdd_adapter_t* pAdapter,
                                       v_U32_t handle );
/*                                                                             
                                                                           
            
                                                     

                                                    
                                            
                                                                             */
VOS_STATUS hdd_wmm_adapter_clear( hdd_adapter_t *pAdapter );

#endif /*                         */

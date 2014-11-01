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


#if !defined( __SMEQOSINTERNAL_H )
#define __SMEQOSINTERNAL_H


/**=========================================================================
  
  \file  smeQosInternal.h
  
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
#include "sme_QosApi.h"
#include "smeInternal.h"

/*                                                                           
                   
                                                                          */
#define SME_QOS_AP_SUPPORTS_APSD         0x80

/*                                                                           
                                                     
                                                                
                                                                           */
typedef enum
{
   SME_QOS_EDCA_AC_BE = 0,  /*                                         */
   SME_QOS_EDCA_AC_BK = 1,  /*                                         */
   SME_QOS_EDCA_AC_VI = 2,  /*                                         */
   SME_QOS_EDCA_AC_VO = 3,  /*                                         */
  
   SME_QOS_EDCA_AC_MAX
} sme_QosEdcaAcType;


/*                                                                           
                                                                                
         
                                                                           */
typedef enum
{
   SME_QOS_CSR_JOIN_REQ = 0,
   SME_QOS_CSR_ASSOC_COMPLETE,
   SME_QOS_CSR_REASSOC_REQ,
   SME_QOS_CSR_REASSOC_COMPLETE,
   SME_QOS_CSR_REASSOC_FAILURE,
   SME_QOS_CSR_DISCONNECT_REQ,
   SME_QOS_CSR_DISCONNECT_IND,
   SME_QOS_CSR_HANDOFF_ASSOC_REQ,
   SME_QOS_CSR_HANDOFF_COMPLETE,
   SME_QOS_CSR_HANDOFF_FAILURE,
#ifdef WLAN_FEATURE_VOWIFI_11R
   SME_QOS_CSR_PREAUTH_SUCCESS_IND,
   SME_QOS_CSR_SET_KEY_SUCCESS_IND,
#endif
}sme_QosCsrEventIndType;

#ifdef FEATURE_WLAN_DIAG_SUPPORT
typedef enum
{
   SME_QOS_DIAG_ADDTS_REQ = 0,
   SME_QOS_DIAG_ADDTS_RSP,
   SME_QOS_DIAG_DELTS

}sme_QosDiagQosEventSubtype;

typedef enum
{
   SME_QOS_DIAG_ADDTS_ADMISSION_ACCEPTED = 0,
   SME_QOS_DIAG_ADDTS_INVALID_PARAMS,
   SME_QOS_DIAG_ADDTS_RESERVED,
   SME_QOS_DIAG_ADDTS_REFUSED,
   SME_QOS_DIAG_USER_REQUESTED,
   SME_QOS_DIAG_DELTS_IND_FROM_AP,

}sme_QosDiagQosEventReasonCode;

#endif //                         
/*                                                                           
                                                                             
                   
                                                                           */
typedef struct
{ 
   tSirBssDescription            *pBssDesc;
   tCsrRoamProfile               *pProfile;
} sme_QosAssocInfo;

/*                                                                           
                                                                
                                                                          */

/*                                                                           
                                                                               
                   

                                                                
    
                           
                                                                            */
eHalStatus sme_QosOpen(tpAniSirGlobal pMac);

/*                                                                           
                                                                              
                                                                           
                        

                                                                
    
                           
                                                                            */
eHalStatus sme_QosClose(tpAniSirGlobal pMac);

/*                                                                          
                                                                          
                               
  
                                                              
                                                                               
                                                                         
  
                                                       
  
     
  
                                                                            */
eHalStatus sme_QosSetParams(tpAniSirGlobal pMac, sme_QosWmmTspecInfo * pQoSInfo);

/*                                                                          
                                                                              
                                              
  
                                                              
                                                                       
                                                                              
                                       
                                                                            
  
                     
  
     
  
                                                                            */
eHalStatus sme_QosMsgProcessor( tpAniSirGlobal pMac,  v_U16_t msg_type, 
                                void *pMsgBuf);

/*                                                                           
                                              
                                                                          */

/*                                                                          
                                                                                 
                                                                          
  
                                                              
                                                                             
                                                            
  
                                                        
  
     
  
                                                                            */
eHalStatus sme_QosValidateParams(tpAniSirGlobal pMac, 
                                 tSirBssDescription *pBssDesc);

/*                                                                          
                                                                                
                                                                            

                                                              
                                                                 
                                                       
  
                    
  
     
  
                                                                            */
eHalStatus sme_QosCsrEventInd(tpAniSirGlobal pMac,
                              v_U8_t sessionId,
                              sme_QosCsrEventIndType ind, 
                              void *pEvent_info);

/*                                                                          
                                                                              
                                         

                                                              
                                                                         
                                                                

  
                                                                 
  
     
  
                                                                            */
v_U8_t sme_QosGetACMMask(tpAniSirGlobal pMac, tSirBssDescription *pSirBssDesc, tDot11fBeaconIEs *pIes);

/*
                           
                                                                                       
*/
sme_QosStatusType sme_QosTriggerUapsdChange( tpAniSirGlobal pMac );

#ifdef FEATURE_WLAN_CCX
v_U8_t sme_QosCCxRetrieveTspecInfo(tpAniSirGlobal pMac, v_U8_t sessionId, tTspecInfo *pTspecInfo);

#endif

#endif //                                  

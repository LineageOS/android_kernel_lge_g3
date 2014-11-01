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

#if defined WLAN_FEATURE_VOWIFI_11R
/**=========================================================================
  
   Macros and Function prototypes FT and 802.11R purposes 

   Copyright 2010 (c) Qualcomm Technologies, Inc.  All Rights Reserved.
   Qualcomm Technologies Confidential and Proprietary.

  ========================================================================*/

#ifndef __LIMFTDEFS_H__
#define __LIMFTDEFS_H__


#include <palTypes.h>
#include "halMsgApi.h"

/*                                                                           
                                        
                                                                          */
#define SIR_MDIE_SIZE               3 //                                   
#define MAX_TIDS                    8
#define MAX_FTIE_SIZE             256 //                                                      


/*                                                                           
                   
                                                                          */
/*                                                                          
                          
                                                                          */
typedef struct sSirFTPreAuthReq
{
   tANI_U16    messageType;      //                         
   tANI_U16    length;
   tANI_BOOLEAN bPreAuthRspProcessed; /*                                                
                                                                                  */
   tANI_U8     preAuthchannelNum;
   tSirMacAddr currbssId;        //                                               
   tSirMacAddr preAuthbssId;     //                    
   tANI_U16    ft_ies_length;
   tANI_U8     ft_ies[MAX_FTIE_SIZE];
   tpSirBssDescription  pbssDescription;
} tSirFTPreAuthReq, *tpSirFTPreAuthReq;

/*                                                                         
                          
                                                                          */
typedef struct sSirFTPreAuthRsp
{
   tANI_U16         messageType;      //                         
   tANI_U16         length;
   tANI_U8          smeSessionId;
   tSirMacAddr      preAuthbssId;     //                    
   tSirRetStatus    status;
   tANI_U16         ft_ies_length;
   tANI_U8          ft_ies[MAX_FTIE_SIZE];
   tANI_U16         ric_ies_length;
   tANI_U8          ric_ies[MAX_FTIE_SIZE];
} tSirFTPreAuthRsp, *tpSirFTPreAuthRsp;

/*                                                                          
                              
                                                                          */
typedef struct sSirFTUpdateKeyInfo
{
   tANI_U16             messageType;
   tANI_U16             length;
   tSirMacAddr          bssId;
   tSirKeyMaterial      keyMaterial;
} tSirFTUpdateKeyInfo, *tpSirFTUpdateKeyInfo;

/*                                                                          
                              
                                                                          */
typedef struct sSirFTPreAuthKeyInfo
{
    tANI_U8 extSetStaKeyParamValid; //                         
    tSetStaKeyParams extSetStaKeyParam;  //                               
} tSirFTPreAuthKeyInfo, *tpSirFTPreAuthKeyInfo;

/*                                                                         
                       
                                                                          */
typedef struct sFTPEContext
{
    tpSirFTPreAuthReq pFTPreAuthReq;                      //                      
    void              *psavedsessionEntry;
    tSirRetStatus     ftPreAuthStatus;
    tANI_U16          saved_auth_rsp_length;
    tANI_U8           saved_auth_rsp[MAX_FTIE_SIZE];
    tSirFTPreAuthKeyInfo    *pPreAuthKeyInfo;
    //                                      
    void              *pftSessionEntry;                   //                                   
    void              *pAddBssReq;                        //                  
    void              *pAddStaReq;                        //                  

} tftPEContext, *tpftPEContext; 


#endif /*                 */ 

#endif /*                         */

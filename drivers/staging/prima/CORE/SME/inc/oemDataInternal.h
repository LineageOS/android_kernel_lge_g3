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

#ifdef FEATURE_OEM_DATA_SUPPORT

/** ------------------------------------------------------------------------- * 
    ------------------------------------------------------------------------- *  

  
    \file oemDataInternal.h
  
    Exports and types for the Common OEM DATA REQ/RSP Module interfaces.
  
    Copyright (C) 2010 Qualcomm Inc.
  
   ========================================================================== */


#ifndef __OEM_DATA_INTERNAL_H__
#define __OEM_DATA_INTERNAL_H__

#include "palTimer.h"
#include "csrSupport.h"
#include "vos_nvitem.h"
#include "wlan_qct_tl.h"

#include "oemDataApi.h"

typedef struct tagOemDataStruct
{
    tANI_U32                         nextOemReqId; //               
    tANI_BOOLEAN                     oemDataReqActive; //                                                       
                                                        //                        
    oemData_OemDataReqCompleteCallback   callback; //                                                    
    void*                            pContext; //                              
    tANI_U32                         oemDataReqID; //                   
    tOemDataRsp*                     pOemDataRsp; //        
    tOemDataReqConfig                oemDataReqConfig; //                        
    tANI_U8                          sessionId; //                                       
} tOemDataStruct;

typedef struct tagOemDataCmd
{
    tANI_U32                            oemDataReqID;
    oemData_OemDataReqCompleteCallback      callback;
    void*                               pContext;
    tOemDataReq                         oemDataReq;
} tOemDataCmd;

#endif //                       

#endif //                        

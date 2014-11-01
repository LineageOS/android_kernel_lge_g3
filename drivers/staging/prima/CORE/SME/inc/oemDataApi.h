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

  
    \file oemDataApi.h
  
    Exports and types for the Common OEM DATA REQ/RSP Module interfaces.
  
    Copyright (C) 2010 Qualcomm Inc.
  
 
   ========================================================================== */

#ifndef __OEM_DATA_API_H__
#define __OEM_DATA_API_H__
#include "sirApi.h"
#include "sirMacProtDef.h"
#include "csrLinkList.h"

#ifndef OEM_DATA_REQ_SIZE
#define OEM_DATA_REQ_SIZE 134
#endif

#ifndef OEM_DATA_RSP_SIZE
#define OEM_DATA_RSP_SIZE 1968
#endif

/*                                                                                                            
                                    
                                                                                                            */

/*                                           */
typedef struct tagOemDataReq
{
    tANI_U8   sessionId;
    tANI_U8   oemDataReq[OEM_DATA_REQ_SIZE];
} tOemDataReq, tOemDataReqConfig;

/*                                                                                                            
                                     
                                                                                                            */
typedef struct tagOemDataRsp
{
    tANI_U8   oemDataRsp[OEM_DATA_RSP_SIZE];
} tOemDataRsp;

/*                                                                                                           */

typedef enum
{
    eOEM_DATA_REQ_SUCCESS=1,
    eOEM_DATA_REQ_FAILURE,
    eOEM_DATA_REQ_INVALID_MODE,
} eOemDataReqStatus;

/*                                                                            
                              
                                                                                             
                           
                                                                                 */

eHalStatus oemData_OemDataReqOpen(tHalHandle hHal);

/*                                                                            
                               
                                                                     
                           
                                                                                 */

eHalStatus oemData_OemDataReqClose(tHalHandle hHal);

/*                                                                                  */
typedef eHalStatus (*oemData_OemDataReqCompleteCallback)(
                                           tHalHandle, 
                                           void* p2, 
                                           tANI_U32 oemDataReqID, 
                                           eOemDataReqStatus status);

/*                                                                            
                          
                                  
                                               
                                                                          
                                                                    
                                                          
                           
                                                                                 */
eHalStatus oemData_OemDataReq(tHalHandle, tANI_U8, tOemDataReqConfig *, tANI_U32 *pOemDataReqID, 
                            oemData_OemDataReqCompleteCallback callback, void *pContext);

/*                                                                            
                            
                                                                             
                                                  
                           
                                                                                 */
eHalStatus sme_HandleOemDataRsp(tHalHandle hHal, tANI_U8*);

/*                                                                            
                                   
                                                                            
                               
                           
                                                                                 */
eHalStatus oemData_IsOemDataReqAllowed(tHalHandle hHal);

#endif //                 

#endif //                        

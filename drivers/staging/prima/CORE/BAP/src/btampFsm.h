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
 * Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 */

/*                                                            */
/*                                                                 */


#ifndef __BTAMPFSM_H__
#define __BTAMPFSM_H__

#include "bapRsn8021xFsm.h"

/*                   */
typedef enum
{
    AUTHENTICATING,
    DISCONNECTED,
    CONNECTING,
    DISCONNECTING,
    SCANNING,
    CONNECTED,
    S1,
    KEYING,
    VALIDATED,
    STARTING,
} BTAMPFSM_STATES_T;


#if 0
/*                                           */
typedef struct
{
    BTAMPFSM_ENTRY_FLAG_T disconnectedEntry;
    BTAMPFSM_STATEVAR_T stateVar;
    BTAMPFSM_INST_ID_T inst_id;
} BTAMPFSM_INSTANCEDATA_T;
#endif // 

/*                                                  */
#define BTAMPFSM_INSTANCEDATA_INIT {  1,  DISCONNECTED/*                */,  0 /*             */};

/*                                       */
void btampfsmChangeToState(BTAMPFSM_INSTANCEDATA_T *instance, BTAMPFSM_STATES_T state);



/*                                        */
//   
VOS_STATUS
btampFsm
(
    //                                    
    ptBtampContext btampContext, /*                    */
//                                                            
    ptWLAN_BAPEvent bapEvent, /*                     */
    v_U8_t *status    /*                               */
);

VOS_STATUS
bapSetKey( v_PVOID_t pvosGCtx, tCsrRoamSetKey *pSetKeyInfo );

int bapSuppDisconnect(tBtampContext *ctx);
int bapAuthDisconnect(tBtampContext *ctx);
VOS_STATUS btampEstablishLogLink(ptBtampContext btampContext);
#endif

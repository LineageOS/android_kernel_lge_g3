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

#if !defined( __BAPRSN_TXRX_H )
#define __BAPRSN_TXRX_H

/**=============================================================================
  
  bapRsnTxRx.h
  
  \brief
  
  Description...
    
  
               Copyright 2008 (c) Qualcomm, Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary.
  
  ==============================================================================*/

#include "vos_types.h"
#include "vos_status.h"
#include "vos_packet.h"
#include "bapRsnAsfPacket.h"


typedef int (*pnfTxCompleteHandler)( v_PVOID_t pvosGCtx, vos_pkt_t *pPacket, VOS_STATUS retStatus );
typedef int (*pnfRxFrameHandler)( v_PVOID_t pvosGCtx, vos_pkt_t *pPacket );

/*
                               
                                  

                                                                              
*/
VOS_STATUS bapRsnSendEapolFrame( v_PVOID_t pvosGCtx, tAniPacket *pAniPkt );


/*
                                      
                                                           

                                                                        
                                                               
*/
VOS_STATUS bapRsnRegisterTxRxCallbacks( pnfTxCompleteHandler pfnTxCom, pnfRxFrameHandler pnfRxFrame );

//                                                     
void bapRsnClearTxRxCallbacks(void);

/*
                                   
                                                                      

                                                     
*/
VOS_STATUS bapRsnRegisterRxCallback( v_PVOID_t pvosGCtx );

VOS_STATUS bapRsnRxCallback(v_PVOID_t pv, vos_pkt_t *pPacket);

#endif //               





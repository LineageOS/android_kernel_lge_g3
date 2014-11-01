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
  
                                                        
                                                                           
  
                            
                        
            
                                                          
                                                                       
  
 */

#ifndef __DPH_HASH_TABLE_H__
#define __DPH_HASH_TABLE_H__

#include "aniGlobal.h"
//                                            
static inline tANI_U8
dphCompareMacAddr(tANI_U8 addr1[], tANI_U8 addr2[])
{
    return((addr1[0] == addr2[0]) &&
       (addr1[1] == addr2[1]) &&
       (addr1[2] == addr2[2]) &&
       (addr1[3] == addr2[3]) &&
       (addr1[4] == addr2[4]) &&
       (addr1[5] == addr2[5]));
}

//                  
typedef struct
{

  //                       
  tpDphHashNode *pHashTable;

  //                 
  tDphHashNode  *pDphNodeArray;
  tANI_U16 size;
} dphHashTableClass;

//                       
extern dphHashTableClass dphHashTable;

//                    
extern void dphPrintMacAddr(struct sAniSirGlobal *pMac, tANI_U8 addr[], tANI_U32);

tpDphHashNode dphLookupHashEntry(tpAniSirGlobal pMac, tANI_U8 staAddr[], tANI_U16 *pStaId, dphHashTableClass* pDphHashTable);
tpDphHashNode dphLookupAssocId(tpAniSirGlobal pMac,  tANI_U16 staIdx, tANI_U16* assocId, dphHashTableClass* pDphHashTable);


//                                
extern tpDphHashNode dphGetHashEntry(tpAniSirGlobal pMac, tANI_U16 staId, dphHashTableClass* pDphHashTable);

//                                
extern tpDphHashNode dphAddHashEntry(tpAniSirGlobal pMac, tSirMacAddr staAddr, tANI_U16 staId, dphHashTableClass* pDphHashTable);

//                                     
extern tSirRetStatus dphDeleteHashEntry(tpAniSirGlobal pMac, tSirMacAddr staAddr, tANI_U16 staId, dphHashTableClass* pDphHashTable);

void dphHashTableClassInit(tpAniSirGlobal pMac, dphHashTableClass* pDphHashTable);
//                      
extern tpDphHashNode dphInitStaState(tpAniSirGlobal pMac, tSirMacAddr staAddr,
        tANI_U16 staId, tANI_U8 validStaIdx, dphHashTableClass* pDphHashTable);


#endif

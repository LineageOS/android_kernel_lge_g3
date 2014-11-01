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
/*                        */
#include "cfgApi.h"         //           
#include "limApi.h"         //           
#include "sirTypes.h"
#include "sysDebug.h"
#include "sysEntryFunc.h"
#include "macInitApi.h"
#if defined(ANI_LOGDUMP)
#include "logDump.h"
#endif //                        

#ifdef TRACE_RECORD
#include "macTrace.h"
#endif

extern tSirRetStatus halDoCfgInit(tpAniSirGlobal pMac);
extern tSirRetStatus halProcessStartEvent(tpAniSirGlobal pMac);




tSirRetStatus macReset(tpAniSirGlobal pMac, tANI_U32 rc);

tSirRetStatus macPreStart(tHalHandle hHal)
{
   tSirRetStatus status = eSIR_SUCCESS;
   tANI_BOOLEAN memAllocFailed = eANI_BOOLEAN_FALSE;
   tpAniSirGlobal pMac = (tpAniSirGlobal) hHal;
   tANI_U8 i;

   for(i=0; i<MAX_DUMP_TABLE_ENTRY; i++)
   {
      pMac->dumpTableEntry[i] = vos_mem_malloc(sizeof(tDumpModuleEntry));
      if ( NULL == pMac->dumpTableEntry[i] )
      {
         memAllocFailed = eANI_BOOLEAN_TRUE;
         break;
      }
      else
      {
         vos_mem_set(pMac->dumpTableEntry[i], sizeof(tSirMbMsg), 0);
      }
   }
   if( memAllocFailed )
   {
      while(i>0)
      {
         i--;
         vos_mem_free(pMac->dumpTableEntry[i]);
      }
      sysLog(pMac, LOGE, FL("pMac->dumpTableEntry is NULL\n"));
      status = eSIR_FAILURE;
   }

#if defined(ANI_LOGDUMP)
   //                                                   
   logDumpInit(pMac);
#endif //                        

   return status;
}

tSirRetStatus macStart(tHalHandle hHal, void* pHalMacStartParams)
{
   tSirRetStatus status = eSIR_SUCCESS;
   tpAniSirGlobal pMac = (tpAniSirGlobal) hHal;

   if (NULL == pMac)
   {
      VOS_ASSERT(0);
      status = eSIR_FAILURE;
      return status;
   }

   pMac->gDriverType = ((tHalMacStartParameters*)pHalMacStartParams)->driverType;

   sysLog(pMac, LOG2, FL("called\n"));

   do
   {

#if defined(TRACE_RECORD)
      //              
      macTraceInit(pMac);
#endif

      pMac->pResetMsg = vos_mem_malloc(sizeof(tSirMbMsg));
      if ( NULL == pMac->pResetMsg )
      {
         sysLog(pMac, LOGE, FL("pMac->pResetMsg is NULL\n"));
         status = eSIR_FAILURE;
         break;
      }
      else
      {
         vos_mem_set(pMac->pResetMsg, sizeof(tSirMbMsg), 0);
      }

      if (pMac->gDriverType != eDRIVER_TYPE_MFG)
      {
         status = peStart(pMac);
      }

   } while(0);
   pMac->sys.abort = false;

   return status;
}


/*                                                               
           
                                                                                                      
                                                                           
                        
                   
                     
                                                               */

tSirRetStatus macStop(tHalHandle hHal, tHalStopType stopType)
{
    tANI_U8 i;
    tpAniSirGlobal pMac = (tpAniSirGlobal) hHal;
    peStop(pMac);
    cfgCleanup( pMac );
    //                                                    
    //                                                   
    if(false == pMac->sys.abort)
    {
        vos_mem_free(pMac->pResetMsg);
        pMac->pResetMsg = NULL;
    }
    /*                         */
    for(i=0; i<MAX_DUMP_TABLE_ENTRY; i++)
    {
        vos_mem_free(pMac->dumpTableEntry[i]);
    }

    return eSIR_SUCCESS;
}

/*                                                               
           
                                                                                             
                                                              
                              
                        
                                           
                     
                                                               */

tSirRetStatus macOpen(tHalHandle *pHalHandle, tHddHandle hHdd, tMacOpenParameters *pMacOpenParms)
{
    tpAniSirGlobal pMac = NULL;

    if(pHalHandle == NULL)
        return eSIR_FAILURE;

    /*
                                                                                         
                                  
                                                                                
                                      
     */

    /*               */
    pMac = vos_mem_malloc(sizeof(tAniSirGlobal));
    if ( NULL == pMac )
        return eSIR_FAILURE;

    /*                               */
    vos_mem_set(pMac, sizeof(tAniSirGlobal), 0);

    /*                                       */
    //                                              

    /*
                                             
                                                                          
                 
     */
    pMac->hHdd      = hHdd;
    pMac->pAdapter  = hHdd; //                        
    *pHalHandle     = (tHalHandle)pMac;

    {
        /*                                             */
        if( eSIR_SUCCESS != logInit(pMac))
           return eSIR_FAILURE;

        /*                                                */
        if( eSIR_SUCCESS != cfgInit(pMac) )
            return eSIR_FAILURE;

        sysInitGlobals(pMac);
    }


    return peOpen(pMac, pMacOpenParms);
}

/*                                                               
            
                                                                          
                                                                
                            
            
                                                               */

tSirRetStatus macClose(tHalHandle hHal)
{

    tpAniSirGlobal pMac = (tpAniSirGlobal) hHal;

    peClose(pMac);

    /*                                                 */
    cfgDeInit(pMac);

    logDeinit(pMac);

    //                                                   
    vos_mem_free( pMac );

    return eSIR_SUCCESS;
}

/*                                                               
            
                                                                                                   
                            
                    
                         
                                                               */

tSirRetStatus macReset(tpAniSirGlobal pMac, tANI_U32 rc)
{
    tSirRetStatus status = eSIR_SUCCESS;
    sysLog(pMac, LOGE, FL("*************No-op. Need to call WDA reset function \n"));
    return status;
}

//                                                                       
/* 
                 
  
            
                                                                
  
         
  
               
  
  
        
  
                                                 
                                    
                                        
 */

void
macSysResetReq(tpAniSirGlobal pMac, tANI_U32 rc)
{
    sysLog(pMac, LOGE, FL("Reason Code = 0x%X\n"),rc);

    switch (rc)
    {
        case eSIR_STOP_BSS:
        case eSIR_SME_BSS_RESTART:
        case eSIR_RADIO_HW_SWITCH_STATUS_IS_OFF:
        case eSIR_CFB_FLAG_STUCK_EXCEPTION:
                //      
                //                   
                break;

        case eSIR_EOF_SOF_EXCEPTION:
        case eSIR_BMU_EXCEPTION:
        case eSIR_CP_EXCEPTION:
        case eSIR_LOW_PDU_EXCEPTION:
        case eSIR_USER_TRIG_RESET:
        case eSIR_AHB_HANG_EXCEPTION:
        default:
             macReset(pMac, rc);
            break;

    }
}

//                                                              
/* 
                        
  
            
                                                                      
  
         
  
               
  
  
        
  
                                                 
                                        
 */

void
macSysResetReqFromHDD(void *pMac, tANI_U32 rc)
{
    macSysResetReq( (tpAniSirGlobal)pMac, rc );
}


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

/*===========================================================================

               W L A N   S Y S T E M   M O D U L E


DESCRIPTION
  This file contains the system module that implements the 'exectution model'
  in the Gen6 host software.


  Copyright (c) 2008 QUALCOMM Incorporated. All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                                   


                                        
                                                                             
                                                                            
                                               
                                                                            
                                                                
                                       

                                                                           */


#include <wlan_qct_sys.h>
#include <vos_api.h>

#include <sirTypes.h>   //                         
#include <sirParams.h>  //                     
#include <sirApi.h>     //                                 
#include <wniApi.h>     //                                 
#include "aniGlobal.h"
#include "wlan_qct_wda.h"
#include "sme_Api.h"
#include "macInitApi.h"

VOS_STATUS WLANFTM_McProcessMsg (v_VOID_t *message);



//                                                                        
//                                                                       
//                      
#define FTM_SYS_MSG_COOKIE      0xFACE

#define SYS_MSG_COOKIE ( 0xFACE )

//                                                 
#ifndef FIELD_OFFSET
#define FIELD_OFFSET(x,y) offsetof(x,y)
#endif

VOS_STATUS sys_SendSmeStartReq( v_CONTEXT_t pVosContext );

//                                      
typedef struct
{
   sysResponseCback mcStartCB;
   v_VOID_t *       mcStartUserData;

} sysContextData;


static vos_event_t gStopEvt;

VOS_STATUS sysBuildMessageHeader( SYS_MSG_ID sysMsgId, vos_msg_t *pMsg )
{
   pMsg->type     = sysMsgId;
   pMsg->reserved = SYS_MSG_COOKIE;

   return( VOS_STATUS_SUCCESS );
}


VOS_STATUS sysOpen( v_CONTEXT_t pVosContext )
{
   return( VOS_STATUS_SUCCESS );
}



v_VOID_t sysStopCompleteCb
(
  v_VOID_t *pUserData
)
{
  vos_event_t* pStopEvt = (vos_event_t *) pUserData;
  VOS_STATUS vosStatus;
/*                                                                         */

  vosStatus = vos_event_set( pStopEvt );
  VOS_ASSERT( VOS_IS_STATUS_SUCCESS ( vosStatus ) );

} /*                               */

VOS_STATUS sysStop( v_CONTEXT_t pVosContext )
{
   VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;
   vos_msg_t sysMsg;
   v_U8_t evtIndex;

   /*                           */
   vosStatus = vos_event_init( &gStopEvt );

   if(! VOS_IS_STATUS_SUCCESS( vosStatus ))
   {
      return vosStatus;
   }

   /*                                                        */
   sysBuildMessageHeader( SYS_MSG_ID_MC_STOP, &sysMsg );

   //                                                                     
   //                                      
   //          
   sysMsg.bodyptr = (void *)sysStopCompleteCb;
   sysMsg.bodyval = (v_U32_t) &gStopEvt;

   //                   
   vosStatus = vos_mq_post_message( VOS_MQ_ID_SYS, &sysMsg );
   if ( !VOS_IS_STATUS_SUCCESS(vosStatus) )
   {
      vosStatus = VOS_STATUS_E_BADMSG;
   }

   vosStatus = vos_wait_events( &gStopEvt, 1, 0, &evtIndex );
   VOS_ASSERT( VOS_IS_STATUS_SUCCESS ( vosStatus ) );

   vosStatus = vos_event_destroy( &gStopEvt );
   VOS_ASSERT( VOS_IS_STATUS_SUCCESS ( vosStatus ) );

   return( vosStatus );
}


VOS_STATUS sysClose( v_CONTEXT_t pVosContext )
{
   return( VOS_STATUS_SUCCESS );
}






#if defined(__ANI_COMPILER_PRAGMA_PACK_STACK)
#pragma pack( push )
#pragma pack( 1 )
#elif defined(__ANI_COMPILER_PRAGMA_PACK)
#pragma pack( 1 )
#endif

typedef struct sPolFileVersion
{
  unsigned char  MajorVersion;
  unsigned char  MinorVersion;
  unsigned char  Suffix;
  unsigned char  Build;

} tPolFileVersion;


typedef struct sPolFileHeader
{
  tPolFileVersion FileVersion;
  tPolFileVersion HWCapabilities;
  unsigned long   FileLength;
  unsigned long   NumDirectoryEntries;

} tPolFileHeader;


typedef enum ePolFileDirTypes
{
  ePOL_DIR_TYPE_BOOTLOADER = 0,
  ePOL_DIR_TYPE_STA_FIRMWARE,
  ePOL_DIR_TYPE_AP_FIRMWARE,
  ePOL_DIR_TYPE_DIAG_FIRMWARE,
  ePOL_DIR_TYPE_STA_CONFIG,
  ePOL_DIR_TYPE_AP_CONFIG

} tPolFileDirTypes;


typedef struct sPolFileDirEntry
{
  unsigned long DirEntryType;
  unsigned long DirEntryFileOffset;
  unsigned long DirEntryLength;

} tPolFileDirEntry;

#if defined(__ANI_COMPILER_PRAGMA_PACK_STACK)
#pragma pack( pop )
#endif


static unsigned short polFileChkSum( unsigned short *FileData, unsigned long NumWords )
{
  unsigned long Sum;

  for ( Sum = 0; NumWords > 0; NumWords-- )
  {
    Sum += *FileData++;
  }

  Sum  = (Sum >> 16) + (Sum & 0xffff); //          
  Sum += (Sum >> 16);                  //                          

  return( (unsigned short)( ~Sum ) );
}

v_BOOL_t sys_validateStaConfig( void *pImage, unsigned long cbFile,
   void **ppStaConfig, v_SIZE_t *pcbStaConfig )
{
   v_BOOL_t fFound = VOS_FALSE;
   tPolFileHeader   *pFileHeader = NULL;
   tPolFileDirEntry *pDirEntry = NULL;
   v_U32_t idx;

   do
   {
      //                                                 
      if ( polFileChkSum( ( v_U16_t *)pImage, cbFile / sizeof( v_U16_t ) ) )
      {
         VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                    "Failed to validate the checksum for CFG binary"  );
         break;
      }

      pFileHeader = (tPolFileHeader *)pImage;

      *ppStaConfig = NULL;
      *pcbStaConfig = 0;

      pDirEntry = ( tPolFileDirEntry* ) ( pFileHeader + 1 );

      for ( idx = 0; idx < pFileHeader->NumDirectoryEntries; ++idx )
      {
         if ( ePOL_DIR_TYPE_STA_CONFIG == pDirEntry[ idx ].DirEntryType )
         {
            *ppStaConfig = pDirEntry[ idx ].DirEntryFileOffset + ( v_U8_t * )pFileHeader;

            *pcbStaConfig = pDirEntry[ idx ].DirEntryLength;

            break;
         }

      } //                                        

      if ( NULL != *ppStaConfig  )
      {
         VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO_LOW,
                    "Found the Station CFG in the CFG binary!!" );

         fFound = VOS_TRUE;
      }
      else
      {
         VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                    "Failed to find Station CFG in the CFG binary" );
      }

   } while( 0 );

   VOS_ASSERT( VOS_TRUE == fFound );

   return( fFound );
}










VOS_STATUS sysMcProcessMsg( v_CONTEXT_t pVosContext, vos_msg_t *pMsg )
{
   VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;
   v_VOID_t *hHal;

   if (NULL == pMsg)
   {
      VOS_ASSERT(0);
      VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
            "%s: NULL pointer to vos_msg_t", __func__);
      return VOS_STATUS_E_INVAL;
   }

   //                                                                  
   //                                                                 
   //                                                                
   //                                                                   
   //                                                
   if ( SYS_MSG_COOKIE == pMsg->reserved )
   {
      //                                   
      switch( pMsg->type )
      {
         case SYS_MSG_ID_MC_START:
         {
            /*                                                       
                                        */
            VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                       " Received SYS_MSG_ID_MC_START message msgType= %d [0x%08lx]",
                       pMsg->type, pMsg->type );
            VOS_ASSERT(0);
            break;
         }

         case SYS_MSG_ID_MC_STOP:
         {
            VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_INFO,
                       "Processing SYS MC STOP" );

            //                       
            hHal = vos_get_context( VOS_MODULE_ID_PE, pVosContext );
            if (NULL == hHal)
            {
               VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                          "%s: Invalid hHal", __func__ );
            }
            else
            {
               vosStatus = sme_Stop( hHal, HAL_STOP_TYPE_SYS_DEEP_SLEEP);
               VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );

               vosStatus = macStop( hHal, HAL_STOP_TYPE_SYS_DEEP_SLEEP );
               VOS_ASSERT( VOS_IS_STATUS_SUCCESS( vosStatus ) );

               ((sysResponseCback)pMsg->bodyptr)((v_VOID_t *)pMsg->bodyval);

               vosStatus = VOS_STATUS_SUCCESS;
            }
            break;
         }

         //                                               
         //                                 
         case SYS_MSG_ID_MC_THR_PROBE:
         {
            VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                       " Received SYS_MSG_ID_MC_THR_PROBE message msgType = %d [0x%08lx]",
                       pMsg->type, pMsg->type);
            break;
         }

         case SYS_MSG_ID_MC_TIMER:
         {
            vos_timer_callback_t timerCB;
            //                       
            //                                                  
            //                                                      
            //                 
            timerCB = (vos_timer_callback_t)pMsg->bodyptr;

            //                                          
            timerCB( (v_VOID_t *)pMsg->bodyval );

            break;
         }
         case SYS_MSG_ID_FTM_RSP:
         {
             WLANFTM_McProcessMsg((v_VOID_t *)pMsg->bodyptr);
             break;
         }

         default:
         {
            VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                       "Unknown message type in sysMcProcessMsg() msgType= %d [0x%08lx]",
                       pMsg->type, pMsg->type );
            break;
        }

      }   //                           

   }   //                  
   else
   {
      //                              
      switch( pMsg->type )
      {

         default:
         {
            VOS_ASSERT( 0 );

            VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                       "Received SYS message cookie with unidentified "
                       "MC message type= %d [0x%08lX]", pMsg->type, pMsg->type );

            vosStatus = VOS_STATUS_E_BADMSG;
            if (pMsg->bodyptr) 
               vos_mem_free(pMsg->bodyptr);
            break;
         }
      }   //                         
   }   //         

   return( vosStatus );
}




VOS_STATUS sysTxProcessMsg( v_CONTEXT_t pVosContext, vos_msg_t *pMsg )
{
   VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;

   if (NULL == pMsg)
   {
      VOS_ASSERT(0);
      VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
            "%s: NULL pointer to vos_msg_t", __func__);
      return VOS_STATUS_E_INVAL;
   }

   //                                                                  
   //                                                                 
   //                                                                
   //                                                                   
   //                                                
   if ( SYS_MSG_COOKIE == pMsg->reserved )
   {
      //                                   
      switch( pMsg->type )
      {
         //                                               
         //                                 
         case SYS_MSG_ID_TX_THR_PROBE:
         {
           /*                                                       
                                        */
            VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                       " Received SYS_MSG_ID_TX_THR_PROBE message msgType= %d [0x%08lx]",
                       pMsg->type, pMsg->type );
            VOS_ASSERT(0);

            break;
         }

         case SYS_MSG_ID_TX_TIMER:
         {
            vos_timer_callback_t timerCB;

            //                       
            //                                                  
            //                                                      
            //                 
            timerCB = (vos_timer_callback_t)pMsg->bodyptr;

            //                                          
            timerCB( (v_VOID_t *)pMsg->bodyval );

            break;
         }

         default:
         {
            VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                       "Unknown message type in sysTxProcessMsg() msgType= %d [0x%08lx]",
                       pMsg->type, pMsg->type );
            break;
        }

      }   //                           
   }   //                  
   else
   {
      VOS_ASSERT( 0 );

      VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                 "Received SYS message cookie with unidentified TX message "
                 " type= %d [0x%08lX]", pMsg->type, pMsg->type );

      vosStatus = VOS_STATUS_E_BADMSG;
   }   //         

   return( vosStatus );
}


VOS_STATUS sysRxProcessMsg( v_CONTEXT_t pVosContext, vos_msg_t *pMsg )
{
   VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;

   if (NULL == pMsg)
   {
      VOS_ASSERT(0);
      VOS_TRACE(VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
            "%s: NULL pointer to vos_msg_t", __func__);
      return VOS_STATUS_E_INVAL;
   }

   //                                                                  
   //                                                                 
   //                                                                
   //                                                                   
   //                                                
   if ( SYS_MSG_COOKIE == pMsg->reserved )
   {
      //                                   
      switch( pMsg->type )
      {
         case SYS_MSG_ID_RX_TIMER:
         {
            vos_timer_callback_t timerCB;

            //                       
            //                                                  
            //                                                      
            //                 
            timerCB = (vos_timer_callback_t)pMsg->bodyptr;

            //                                          
            timerCB( (v_VOID_t *)pMsg->bodyval );

            break;
         }

         default:
         {
            VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                       "Unknown message type in sysRxProcessMsg() msgType= %d [0x%08lx]",
                       pMsg->type, pMsg->type );
            break;
        }

      }   //                           
   }   //                  
   else
   {
      VOS_ASSERT( 0 );

      VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                 "Received SYS message cookie with unidentified RX message "
                 " type= %d [0x%08lX]", pMsg->type, pMsg->type );

      vosStatus = VOS_STATUS_E_BADMSG;
   }   //         

   return( vosStatus );
}


v_VOID_t sysMcFreeMsg( v_CONTEXT_t pVContext, vos_msg_t* pMsg )
{
   return;
}


v_VOID_t sysTxFreeMsg( v_CONTEXT_t pVContext, vos_msg_t* pMsg )
{
   return;
}


void
SysProcessMmhMsg
(
  tpAniSirGlobal pMac,
  tSirMsgQ* pMsg
)
{
  VOS_MQ_ID   targetMQ = VOS_MQ_ID_SYS;
/*                                                                         */
  /*
                                         
                                              
                                      
  */


  if (NULL == pMsg)
  {
      VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                 "NULL Message Pointer");
      VOS_ASSERT(0);
      return;
  }


  switch (pMsg->type)
  {
    /*
                                           
    */
    case WNI_CFG_DNLD_REQ:
    case WNI_CFG_DNLD_CNF:
    case WDA_APP_SETUP_NTF:
    case WDA_NIC_OPER_NTF:
    case WDA_RESET_REQ:
    case eWNI_SME_START_RSP:
    {
      /*                                        */
      targetMQ = VOS_MQ_ID_SYS;

      VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                 "Handling for the Message ID %d is removed in SYS\r\n",
                 pMsg->type);

      VOS_ASSERT(0);
      break;
    }


    /*
                                           
    */
    case WNI_CFG_DNLD_RSP:
    case WDA_INIT_START_REQ:
    {
      /*                                        */
      targetMQ = VOS_MQ_ID_WDA;

      VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                 "Handling for the Message ID %d is removed as there is no HAL \r\n",
                 pMsg->type);

      VOS_ASSERT(0);
      break;
    }

    case eWNI_SME_START_REQ:
    case WNI_CFG_GET_REQ:
    case WNI_CFG_SET_REQ:
    case WNI_CFG_SET_REQ_NO_RSP:
    case eWNI_SME_SYS_READY_IND:
    {
       /*                                       */
      targetMQ = VOS_MQ_ID_PE;
      break;
    }


    case WNI_CFG_GET_RSP:
    case WNI_CFG_SET_CNF:
/*                              
                               
                                
                                  
                                
                                 
                                
                                
                                 
                                
                                  
                                 
                                 
                                
                                  */ //                                       
    {
       /*                                        */
      targetMQ = VOS_MQ_ID_SME;
      break;
    }

    default:
    {

      if ( ( pMsg->type >= eWNI_SME_MSG_TYPES_BEGIN )  &&  ( pMsg->type <= eWNI_SME_MSG_TYPES_END ) )
      {
         targetMQ = VOS_MQ_ID_SME;
         break;
      }

      VOS_TRACE( VOS_MODULE_ID_SYS, VOS_TRACE_LEVEL_ERROR,
                 "Message of ID %d is not yet handled by SYS\r\n",
                 pMsg->type);

      VOS_ASSERT(0);
    }

  }


  /*
                                                                
  */
  if(VOS_STATUS_SUCCESS != vos_mq_post_message(targetMQ, (vos_msg_t*)pMsg))
  {
    //                                                                           
    /*                         */
    if(pMsg->bodyptr)
    {
      vos_mem_free( pMsg->bodyptr);
    }
  }

} /*                    */

/*                                                                          
                              

             
                                                                            
                                                                          

              
                                                                 

            

      
                                                                        
                                                                   
                                                   


              
                                                            

                       

              
          
                                                                            */

void wlan_sys_ftm(void *pMsgPtr)
{
    vos_msg_t  vosMessage;



    vosMessage.reserved = FTM_SYS_MSG_COOKIE;
    vosMessage.type     = SYS_MSG_ID_FTM_RSP;
    vosMessage.bodyptr  = pMsgPtr;

    vos_mq_post_message(VOS_MQ_ID_SYS, &vosMessage);

    return;
}



void wlan_sys_probe(void)
{
    vos_msg_t  vosMessage;

    vosMessage.reserved = FTM_SYS_MSG_COOKIE;
    vosMessage.type     = SYS_MSG_ID_MC_THR_PROBE;
    vosMessage.bodyptr  = NULL;

    vos_mq_post_message(VOS_MQ_ID_SYS, &vosMessage);

    return;
}


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

                      b a p A p i L i n k C n t l . C
                                               
  OVERVIEW:
  
  This software unit holds the implementation of the WLAN BAP modules
  Link Control functions.
  
  The functions externalized by this module are to be called ONLY by other 
  WLAN modules (HDD) that properly register with the BAP Layer initially.

  DEPENDENCIES: 

  Are listed for each API below. 
  
  
  Copyright (c) 2008 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                                                                                                                                              


                                      
                                                                             
                                    

                                                                           */

/*                                                                            
                
                                                                            */
//                        
#include "vos_trace.h"
//                                    
#include "csrApi.h"

/*                            */ 
#include "bapApi.h" 
#include "bapInternal.h" 
#include "btampFsm.h"

//                 
/*                                                                            
                                         
                                                                            */


/*                                                                            
                    
                                                                            */

/*                                                                            
                          
                                                                            */

/*                                                                            
                              
                                                                            */

/*                                                                            
                                               
                                                                            */

/*                                                                            
                                    
                                                                           */

/*                                                                            
                                          
                                                                            */

/*                                                                            

                                    

              
                                                  

               
        

             

      
                                                                
                                                                                        
                                                              
                                                                                   
                                                                 
                               
   
              
                                                                  

                                 
  
               
  
                                                                            */
#if 0
eCSR_ROAM_RESULT_WDS_STARTED 
#define eWLAN_BAP_MAC_START_BSS_SUCCESS /*                                                           */

eCSR_ROAM_RESULT_FAILURE 
eCSR_ROAM_RESULT_NOT_ASSOCIATED
#define eWLAN_BAP_MAC_START_FAILS /*                                                                                          */

eCSR_ROAM_RESULT_WDS_ASSOCIATED
#define eWLAN_BAP_MAC_CONNECT_COMPLETED /*                                                              */


eCSR_ROAM_RESULT_FAILURE 
eCSR_ROAM_RESULT_NOT_ASSOCIATED 
#define eWLAN_BAP_MAC_CONNECT_FAILED /*                                                                                          */


eCSR_ROAM_RESULT_WDS_ASSOCIATION_IND
#define eWLAN_BAP_MAC_CONNECT_INDICATION /*                                                                   */


eCSR_ROAM_RESULT_KEY_SET 
#define eWLAN_BAP_MAC_KEY_SET_SUCCESS /*                                                       */


eCSR_ROAM_RESULT_WDS_DISASSOC_IND
#define eWLAN_BAP_MAC_INDICATES_MEDIA_DISCONNECTION /*                                                                */


eCSR_ROAM_RESULT_WDS_STOPPED
#define eWLAN_BAP_MAC_READY_FOR_CONNECTIONS /*                                                           */

#endif // 


eHalStatus
WLANBAP_RoamCallback
(
  void *pContext, 
  tCsrRoamInfo *pCsrRoamInfo,
  tANI_U32 roamId, 
  eRoamCmdStatus roamStatus, 
  eCsrRoamResult roamResult
)
{
    eHalStatus  halStatus = eHAL_STATUS_SUCCESS; 
    /*                    */    
    ptBtampContext btampContext = (ptBtampContext) pContext; 
    tWLAN_BAPEvent bapEvent; /*                     */
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS; 
    v_U8_t status;    /*                               */
    /*                                                                       */

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, before switch on roamStatus = %d", __func__, roamStatus);

    switch (roamStatus) {
        //                                                                
        //                                                                
        //                                                     
        //                              
        case eCSR_ROAM_ASSOCIATION_START: 
            /*                                                           */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_ROAMING_START", roamResult);   
            //                                                                              
#if 0
            if ((pCsrRoamInfo) && (pCsrRoamInfo->pConnectedProfile) && (pCsrRoamInfo->pConnectedProfile->pBssDesc))
            {
                memcpy(bssid.ether_addr_octet, pCsrRoamInfo->pConnectedProfile->pBssDesc->bssId,
                       sizeof(tSirMacAddr)); 
                apple80211Interface->willRoam(&bssid);  //                                 
                VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: willRoam returns\n", __func__);
            }
#endif // 
            /*                             */ 
            bapEvent.event = eWLAN_BAP_MAC_START_BSS_SUCCESS; 
            bapEvent.params = pCsrRoamInfo;
            bapEvent.u1 = roamStatus;
            bapEvent.u2 = roamResult;

            /*              */ 
            vosStatus = btampFsm(btampContext, &bapEvent, &status);

            break;

        case eCSR_ROAM_SET_KEY_COMPLETE:
            /*                                                         */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamStatus = %s (%d)", __func__, "eCSR_ROAM_SET_KEY_COMPLETE", roamStatus);   

            /*                             */ 
            bapEvent.event = eWLAN_BAP_MAC_KEY_SET_SUCCESS; 
            bapEvent.params = pCsrRoamInfo;
            bapEvent.u1 = roamStatus;
            bapEvent.u2 = roamResult;

            /*              */ 
            vosStatus = btampFsm(btampContext, &bapEvent, &status);
  
            break;

        case eCSR_ROAM_DISASSOCIATED: 
            /*                                                      */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamStatus = %s (%d)", __func__, "eCSR_ROAM_DISASSOCIATED", roamStatus);   
        case eCSR_ROAM_LOSTLINK:
            /*                                                 */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamStatus = %s (%d)", __func__, "eCSR_ROAM_LOSTLINK", roamStatus);   

            if (roamResult != eCSR_ROAM_RESULT_NONE) {
                /*                             */ 
                bapEvent.event = eWLAN_BAP_MAC_READY_FOR_CONNECTIONS; 
                bapEvent.params = pCsrRoamInfo;
                bapEvent.u1 = roamStatus;
                bapEvent.u2 = roamResult; 
                
                /*              */ 
                vosStatus = btampFsm(btampContext, &bapEvent, &status);
            }
  
            break;

        default:
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, unsupported CSR roamStatus = %d", __func__, roamStatus);

            break;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, before switch on roamResult = %d", __func__, roamResult);

    switch (roamResult) {
        //                                                             
        case eCSR_ROAM_RESULT_IBSS_STARTED:  
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_IBSS_STARTED", roamResult);   
        case eCSR_ROAM_RESULT_WDS_STARTED: 
            /*                                                           */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_WDS_STARTED", roamResult);   

            /*                             */ 
            bapEvent.event = eWLAN_BAP_MAC_START_BSS_SUCCESS; 
            bapEvent.params = pCsrRoamInfo;
            bapEvent.u1 = roamStatus;
            bapEvent.u2 = roamResult;

            /*              */ 
            vosStatus = btampFsm(btampContext, &bapEvent, &status);
  
            break;

        //                                                             
        //                                                                
        case eCSR_ROAM_RESULT_FAILURE: 
        //                                     
        //                                        
            /*                                                                                          */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_FAILURE", roamResult);   
#ifdef FEATURE_WLAN_BTAMP_UT_RF
            break;
#endif
        case eCSR_ROAM_RESULT_WDS_START_FAILED:
            /*                                                                */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_WDS_START_FAILED", roamResult);   

            /*                             */ 
            /*                                                          
                                             
             */ 
            bapEvent.event = eWLAN_BAP_MAC_START_FAILS; 
            bapEvent.params = pCsrRoamInfo;
            bapEvent.u1 = roamStatus;
            bapEvent.u2 = roamResult;

            /*              */ 
            vosStatus = btampFsm(btampContext, &bapEvent, &status);

            break;

        //                                                                               
        case eCSR_ROAM_RESULT_IBSS_CONNECT:
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_IBSS_CONNECT", roamResult);   
        case eCSR_ROAM_RESULT_ASSOCIATED:
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_ASSOCIATED", roamResult);   
        case eCSR_ROAM_RESULT_WDS_ASSOCIATED:
            /*                                                              */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_WDS_ASSOCIATED", roamResult);   

            /*                             */ 
            bapEvent.event = eWLAN_BAP_MAC_CONNECT_COMPLETED;
            bapEvent.params = pCsrRoamInfo;
            bapEvent.u1 = roamStatus;
            bapEvent.u2 = roamResult;

            /*              */ 
            vosStatus = btampFsm(btampContext, &bapEvent, &status);
  
            break;

        //                                                             
        //                                                                
        //                               
        case eCSR_ROAM_RESULT_IBSS_START_FAILED:
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_IBSS_START_FAILED", roamResult);   
        case eCSR_ROAM_RESULT_NOT_ASSOCIATED:
            /*                                                                                          */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_NOT_ASSOCIATED", roamResult);   
#ifdef FEATURE_WLAN_BTAMP_UT_RF
            break;
#endif
        case eCSR_ROAM_RESULT_WDS_NOT_ASSOCIATED:
            /*                                                                  */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_WDS_NOT_ASSOCIATED", roamResult);   

            /*                             */ 
            bapEvent.event = eWLAN_BAP_MAC_CONNECT_FAILED; 
            bapEvent.params = pCsrRoamInfo;
            bapEvent.u1 = roamStatus;
            bapEvent.u2 = roamResult;

            /*              */ 
            vosStatus = btampFsm(btampContext, &bapEvent, &status);
  
            break;

        //                                                     
        //                                                         
        //                                   
            //                                                                                                                                                      

        //                                                             
        //                          
        case eCSR_ROAM_RESULT_WDS_ASSOCIATION_IND:
            /*                                                                   */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_WDS_ASSOCIATION_IND", roamResult);   

            /*                             */ 
            bapEvent.event = eWLAN_BAP_MAC_CONNECT_INDICATION;
            bapEvent.params = pCsrRoamInfo;
            bapEvent.u1 = roamStatus;
            bapEvent.u2 = roamResult;

            /*              */ 
            vosStatus = btampFsm(btampContext, &bapEvent, &status);
  
            /*                                                              */ 
            if ( status != WLANBAP_STATUS_SUCCESS) 
                halStatus = eHAL_STATUS_FAILURE;
  
            break;

        //                                              
#if 0
        case eCSR_ROAM_RESULT_KEY_SET: 
            /*                                                       */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_KEY_SET", roamResult);   

            /*                             */ 
            bapEvent.event = eWLAN_BAP_MAC_KEY_SET_SUCCESS; 
            bapEvent.params = pCsrRoamInfo;
            bapEvent.u1 = roamStatus;
            bapEvent.u2 = roamResult;

            /*              */ 
            vosStatus = btampFsm(btampContext, &bapEvent, &status);
  
            break;
#endif // 

        case eCSR_ROAM_RESULT_DISASSOC_IND:
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_DISASSOC_IND", roamResult);   
        case eCSR_ROAM_RESULT_WDS_DISASSOCIATED:
            /*                                                                 */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_WDS_DISASSOCIATED", roamResult);   

            /*                             */ 
            bapEvent.event =  eWLAN_BAP_MAC_INDICATES_MEDIA_DISCONNECTION; 
            bapEvent.params = pCsrRoamInfo;
            bapEvent.u1 = roamStatus;
            bapEvent.u2 = roamResult;

            /*              */ 
            vosStatus = btampFsm(btampContext, &bapEvent, &status);
  
            /*                             */ 
            bapEvent.event =  eWLAN_BAP_MAC_READY_FOR_CONNECTIONS;
            bapEvent.params = pCsrRoamInfo;
            bapEvent.u1 = roamStatus;
            bapEvent.u2 = roamResult;

            /*              */ 
            vosStatus = btampFsm(btampContext, &bapEvent, &status);

            break;

        //                                                             
        case eCSR_ROAM_RESULT_IBSS_INACTIVE:
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_IBSS_INACTIVE", roamResult);   
        case eCSR_ROAM_RESULT_WDS_STOPPED:
            /*                                                           */
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, CSR roamResult = %s (%d)", __func__, "eCSR_ROAM_RESULT_WDS_STOPPED", roamResult);   

            /*                             */ 
            bapEvent.event = eWLAN_BAP_MAC_READY_FOR_CONNECTIONS; 
            bapEvent.params = pCsrRoamInfo;
            bapEvent.u1 = roamStatus;
            bapEvent.u2 = roamResult;

            /*              */ 
            vosStatus = btampFsm(btampContext, &bapEvent, &status);
  
            break;

        default:
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, unsupported CSR roamResult = %d", __func__, roamResult);

            break;
    }

#if 0
    switch (roamResult) {
        case eCSR_ROAM_RESULT_IBSS_CONNECT:
            //                              

            //                 
            btampContext->mAssociatedStatus = WLANBAP_STATUS_SUCCESS;
            btampContext->mAssociated = VOS_TRUE;
            //                                               
            if (pCsrRoamInfo) 
                memcpy(btampContext->assocBssid, pCsrRoamInfo->peerMacOrBssidForIBSS, 6);

            //                                                    
            //                                                     
            //                                                  
            //                                               
            //                                                    
            //                                                              
            //                                                           
            //                                                             
            //                                                            
            //                                                              
            //                                

            btampContext->systemRole = eSYSTEM_STA_IN_IBSS_ROLE;

            if (mLinkStatus == 0)
            {
                //                        
                DBGLOG("%s: marking link as up in %s\n", __func__, "eCSR_ROAM_RESULT_IBSS_CONNECT");
                mLinkStatus = 1;
                ((IO80211Interface*) mNetworkIF)->setLinkState(kIO80211NetworkLinkUp);
                outputQueue->setCapacity(TRANSMIT_QUEUE_SIZE);
                outputQueue->start();
                //                           
                ((IO80211Interface*) mNetworkIF)->postMessage(APPLE80211_M_ASSOC_DONE);
            }
            else
            {
                DBGLOG("%s: link is already up in %s\n", __func__, "eCSR_ROAM_RESULT_IBSS_CONNECT");
            }
            break;

        case eCSR_ROAM_RESULT_IBSS_INACTIVE:
            //                                                        
            if (mLinkStatus != 0)
            {
                DBGLOG("%s: marking link as down in %s\n", __func__, "eCSR_ROAM_RESULT_IBSS_INACTIVE");
                mLinkStatus = (tANI_U8) 0;
                //                     
                ((IO80211Interface*) mNetworkIF)->setLinkState(kIO80211NetworkLinkDown);
                outputQueue->stop();
                outputQueue->setCapacity(0);

                //                 
                btampContext->mAssociated = false;
            }
            else
            {
                DBGLOG("%s: link already down in %s\n", __func__, "eCSR_ROAM_RESULT_IBSS_INACTIVE");
            }

            break;

        case eCSR_ROAM_RESULT_ASSOCIATED:
            btampContext->mAssociatedStatus = APPLE80211_STATUS_SUCCESS;
            btampContext->mAssociated = true;

            if ((pCsrRoamInfo) && (pCsrRoamInfo->pBssDesc)) {
                ccpCsrToAppleScanResult(mPMacObject, pCsrRoamInfo->pBssDesc, &scanResult); 
                
                /*                                  */
                ccpCsrToAssocApiedata( mPMacObject, pCsrRoamInfo->pBssDesc, &(btampContext->apiedata));
                
                if (BssidChanged((tCsrBssid*) btampContext->assocBssid, (ether_addr*) scanResult.asr_bssid)) { 
                    memcpy(btampContext->assocBssid, scanResult.asr_bssid, 6);
                    ((IO80211Interface*) mNetworkIF)->postMessage(APPLE80211_M_BSSID_CHANGED );
                }
            }

            ((IO80211Interface*) mNetworkIF)->postMessage(APPLE80211_M_ASSOC_DONE);

            if (mLinkStatus == 0)
            {
                mLinkStatus = (tANI_U8) 1;
                ((IO80211Interface*) mNetworkIF)->setLinkState(kIO80211NetworkLinkUp);
                DBGLOG("%s: marking link as up in %s\n", __func__, "eCSR_ROAM_RESULT_ASSOCIATED");
                outputQueue->setCapacity(TRANSMIT_QUEUE_SIZE);
                outputQueue->start();
            }
            else
            {
                DBGLOG("%s: link is already up in %s\n", __func__, "eCSR_ROAM_RESULT_ASSOCIATED");
            }
            break;
        case eCSR_ROAM_RESULT_NOT_ASSOCIATED:
            btampContext->mAssociatedStatus = APPLE80211_STATUS_UNAVAILABLE;
            btampContext->mAssociated = false;

            if (mLinkStatus != 0)
            {
                DBGLOG("%s: marking link as down in %s\n", __func__, "eCSR_ROAM_RESULT_NOT_ASSOCIATED");
                mLinkStatus = (tANI_U8) 0;
                ((IO80211Interface*) mNetworkIF)->setLinkState(kIO80211NetworkLinkDown);
            }
            else
            {
                DBGLOG("%s: link already down in %s\n", __func__, "eCSR_ROAM_RESULT_NOT_ASSOCIATED");
            }
            break;
           
        case eCSR_ROAM_RESULT_FAILURE:
            btampContext->mAssociatedStatus = APPLE80211_STATUS_UNSPECIFIED_FAILURE;
            btampContext->mAssociated = false;

            if (mLinkStatus != 0)
            {
                DBGLOG("%s: marking link as down in %s\n", __func__, "eCSR_ROAM_RESULT_FAILURE");
                mLinkStatus = (tANI_U8) 0;
                ((IO80211Interface*) mNetworkIF)->setLinkState(kIO80211NetworkLinkDown);
            }
            else
            {
                DBGLOG("%s: link already down in %s\n", __func__, "eCSR_ROAM_RESULT_FAILURE");
            }
            break;
        
        case eCSR_ROAM_RESULT_DISASSOC_IND:
            {
                btampContext->mAssociated = false;

                if (mLinkStatus != 0)
                {
                    DBGLOG("%s: marking link as down in %s\n", __func__, "eCSR_ROAM_RESULT_DISASSOC_IND");
                    mLinkStatus = (tANI_U8) 0;
                    ((IO80211Interface*) mNetworkIF)->setLinkState(kIO80211NetworkLinkDown);
                }
                else
                {
                    DBGLOG("%s: link already down in %s\n", __func__, "eCSR_ROAM_RESULT_DISASSOC_IND");
                }

                //                                                                                      
                {
                    //                                       
                    //                                                                                                             
                    //                                                                                                        
                    //                                                                                                  
                    //                                                                                                  
                    //                                                                                              
                    //                                     
                    //                                                                                               
                    //                                                             
                    //                                                                                                         
                    //                                                                                                         
                    btampContext->reasonCode = (tANI_U16) eSIR_MAC_UNSPEC_FAILURE_REASON; //                                       
                    btampContext->deAuthReasonCode = 0; //                                         
                    //                                                                                                 
                    btampContext->disassocReasonCode = btampContext->reasonCode; //                    
                    //                                               
                    //                                                                          
                } 
            }
            break;

        case eCSR_ROAM_RESULT_DEAUTH_IND:
            {
                btampContext->mAssociated = false;

                if (mLinkStatus != 0)
                {
                    DBGLOG("%s: marking link as down in %s\n", __func__, "eCSR_ROAM_RESULT_DEAUTH_IND");
                    mLinkStatus = (tANI_U8) 0;
                    ((IO80211Interface*) mNetworkIF)->setLinkState(kIO80211NetworkLinkDown);
                }
                else
                {
                    DBGLOG("%s: link already down in %s\n", __func__, "eCSR_ROAM_RESULT_DEAUTH_IND");
                }

                //                                                                                      
                {
                    //                                       
                    //                                                                                                           
                    //                                                                                                        
                    //                                                                                                  
                    //                                                                                                  
                    //                                                                                              
                    //                                     
                    //                                                                                               
                    //                                                           
                    //                                                                                                       
                    //                                                                                                       
                    btampContext->reasonCode = (tANI_U16) eSIR_MAC_UNSPEC_FAILURE_REASON; //                                       
                    btampContext->disassocReasonCode = 0; //                                           
                    //                                                                                               
                    btampContext->deAuthReasonCode = btampContext->reasonCode; //                    
                    //                                              
                    //                                                                                                       
                } 
            }
            break;

        case eCSR_ROAM_RESULT_MIC_ERROR_UNICAST: 

            //                                                        
            { 
                if (btampContext->mTKIPCounterMeasures)
                {
                    ((IO80211Interface*) mNetworkIF)->postMessage(APPLE80211_M_MIC_ERROR_UCAST); 
                    DBGLOG("%s: TKIP Countermeasures in effect in %s\n", __func__, "eCSR_ROAM_RESULT_MIC_ERROR_UNICAST"); 
                } 
                else 
                { 
                    DBGLOG("%s: TKIP Countermeasures disabled in %s\n", __func__, "eCSR_ROAM_RESULT_MIC_ERROR_UNICAST"); 
                }
            }
            break;

        case eCSR_ROAM_RESULT_MIC_ERROR_GROUP: 

            //                                                        
            { 
                if (btampContext->mTKIPCounterMeasures)
                { 
                    ((IO80211Interface*) mNetworkIF)->postMessage(APPLE80211_M_MIC_ERROR_MCAST); 
                    DBGLOG("%s: TKIP Countermeasures in effect in %s\n", __func__, "eCSR_ROAM_RESULT_MIC_ERROR_GROUP"); 
                } 
                else 
                { 
                    DBGLOG("%s: TKIP Countermeasures disabled in %s\n", __func__, "eCSR_ROAM_RESULT_MIC_ERROR_GROUP"); 
                }
            }
            break;

        default:
            break;
    }
    switch (roamStatus) {
        case eCSR_ROAM_ROAMING_START: 
            DBGLOG("%s: In %s\n", __func__, "eCSR_ROAM_ROAMING_START");
            //                                                                              
            //                              
            if ((pCsrRoamInfo) && (pCsrRoamInfo->pConnectedProfile) && (pCsrRoamInfo->pConnectedProfile->pBssDesc))
            {
                memcpy(bssid.ether_addr_octet, pCsrRoamInfo->pConnectedProfile->pBssDesc->bssId,
                       sizeof(tSirMacAddr)); 
                apple80211Interface->willRoam(&bssid);  //                                 
                DBGLOG("%s: willRoam returns\n", __func__);
            }
            break;

        case eCSR_ROAM_SHOULD_ROAM:
            if ((pCsrRoamInfo) && (pCsrRoamInfo->pBssDesc)) {
                //                                                                    
                halStatus = ccpCsrToAppleScanResult( 
                        mPMacObject, 
                        pCsrRoamInfo->pBssDesc, 
                        &scanResult); 
                if ( halStatus != 0 ) 
                    return eHAL_STATUS_FAILURE;
                roamAccepted = apple80211Interface->shouldRoam(&scanResult);  //                         
                if (roamAccepted == true) { 
                    //                                           
                    DBGLOG("%s: shouldRoam returns \"acceptable\"\n", __func__);
//     
                    //                                                        
                    //                                                       
                    //                                                               
                    //                                                                
                    memcpy(bssid.ether_addr_octet, pCsrRoamInfo->pBssDesc->bssId, sizeof(tSirMacAddr)); 
                    apple80211Interface->willRoam(&bssid);  //                                 
                    DBGLOG("%s: willRoam (called out of order) returns\n", __func__);
                    DBGLOG("    with BSSID = " MAC_ADDR_STRING(bssid.ether_addr_octet));
//      
                    return eHAL_STATUS_SUCCESS;
                } else { 
                    //                                              
                    DBGLOG("%s: shouldRoam returns \"NOT acceptable\"\n", __func__);
                    return eHAL_STATUS_FAILURE;
                }
            }
            break;

        case eCSR_ROAM_DISASSOCIATED:
            //                                                                                      
            {
                btampContext->mAssociated = false;

                if (mLinkStatus != 0)
                {
                    DBGLOG("%s: marking link as down in %s\n", __func__, "eCSR_ROAM_DISASSOCIATED");
                    mLinkStatus = (tANI_U8) 0;
                    ((IO80211Interface*) mNetworkIF)->setLinkState(kIO80211NetworkLinkDown);
                }
                else
                {
                    DBGLOG("%s: link already down in %s\n", __func__, "eCSR_ROAM_DISASSOCIATED");
                }
            }
            break;

        case eCSR_ROAM_LOSTLINK:
            btampContext->mAssociatedStatus = APPLE80211_STATUS_UNSPECIFIED_FAILURE;
            btampContext->mAssociated = false;

            if (mLinkStatus != 0)
            {
                DBGLOG("%s: marking link as down in %s\n", __func__, "eCSR_ROAM_LOSTLINK");
                mLinkStatus = (tANI_U8) 0;
                ((IO80211Interface*) mNetworkIF)->setLinkState(kIO80211NetworkLinkDown);
            }
            else
            {
                DBGLOG("%s: link already down in %s\n", __func__, "eCSR_ROAM_LOSTLINK");
            }
            break;

        case eCSR_ROAM_ASSOCIATION_START:
            DBGLOG("%s: In %s\n", __func__, "eCSR_ROAM_ASSOCIATION_START");
#if 0
            //                                                                         
            //                                                                   
            //                                                                          
            //                                                                                       
            if ((pCsrRoamInfo) && (pCsrRoamInfo->pBssDesc) {
                memcpy(bssid.ether_addr_octet, pCsrRoamInfo->pBssDesc->bssId, 6); 
                apple80211Interface->willRoam(&bssid);  //                                 
                DBGLOG("%s: willRoam returns\n", __func__);
                DBGLOG("    with BSSID = " MAC_ADDR_STRING(bssid.ether_addr_octet));
            }
#endif // 
            break;

        case eCSR_ROAM_ASSOCIATION_COMPLETION:
            DBGLOG("%s: In %s\n", __func__, "eCSR_ROAM_ASSOCIATION_COMPLETION");
            break;

        case eCSR_ROAM_MIC_ERROR_IND:   //                                                               
        case eCSR_ROAM_CANCELLED:
        case eCSR_ROAM_ROAMING_COMPLETION:
        case eCSR_ROAM_SCAN_FOUND_NEW_BSS:
        default:
            break;
    }
#endif // 

    return halStatus;
}

/*                                                                            
                                            
                                                                            */

/*                                */

/*                                                                            
                                                                         
                                                                              
                                                                              
                                                                            */

/*                                                                            

                                          

              
                                                          

               
        

             

      
                                                                              
                                                                          
                                                                      
                               
                                                                                
                                                                               

          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                  
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPPhysicalLinkCreate 
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Create_Physical_Link_Cmd   *pBapHCIPhysLinkCreate,
  v_PVOID_t      pHddHdl,   /*                                    */
                            /*                                            */
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    tWLAN_BAPEvent bapEvent; /*                     */
    VOS_STATUS  vosStatus;
    /*                                                         */ 
    //                                                 
    ptBtampContext btampContext = (ptBtampContext) btampHandle; /*                    */ 
    v_U8_t status;    /*                               */
    BTAMPFSM_INSTANCEDATA_T *instanceVar = &(btampContext->bapPhysLinkMachine);

    /*                 */ 
    if ((pBapHCIPhysLinkCreate == NULL) || (NULL == btampContext))
    {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "%s: btampHandle value: %p, pBapHCIPhysLinkCreate is %p",
                 __func__,  btampHandle, pBapHCIPhysLinkCreate); 
      return VOS_STATUS_E_FAULT;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);

    if(DISCONNECTED != instanceVar->stateVar)
    {
       /*                                                 */
        status = WLANBAP_ERROR_MAX_NUM_CNCTS;

    }
    else
    {
        /*                             */ 
        bapEvent.event = eWLAN_BAP_HCI_PHYSICAL_LINK_CREATE;
        bapEvent.params = pBapHCIPhysLinkCreate;
        //                                            

        /*                                       */ 
        /*                                                               */
        /*                                                         */ 
        /*                                        */
        vosStatus = WLANBAP_CreateNewPhyLinkCtx ( 
                btampHandle, 
                pBapHCIPhysLinkCreate->phy_link_handle, /*                                         */
                pHddHdl,   /*                                    */
                &btampContext, /*                                                   */ 
                BT_INITIATOR); /*              */ 

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampContext value: %p", __func__,  btampContext);

        /*              */ 
        vosStatus = btampFsm(btampContext, &bapEvent, &status);
    }
  
        /*                                              */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_STATUS_EVENT;
    pBapHCIEvent->u.btampCommandStatusEvent.present = 1;
    pBapHCIEvent->u.btampCommandStatusEvent.status = status;
    pBapHCIEvent->u.btampCommandStatusEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandStatusEvent.command_opcode 
        = BTAMP_TLV_HCI_CREATE_PHYSICAL_LINK_CMD;

    /*     */ 

    return VOS_STATUS_SUCCESS;
} /*                            */ 

/*                                                                            

                                          

              
                                                          

               
        

             

      
                                                                              
                                                                                
                                                                               
   
          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                  
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPPhysicalLinkAccept 
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Accept_Physical_Link_Cmd   *pBapHCIPhysLinkAccept,
  v_PVOID_t      pHddHdl,   /*                                    */
                            /*                                            */
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    tWLAN_BAPEvent bapEvent; /*                     */
    VOS_STATUS  vosStatus;
    /*                                                         */ 
    //                                                 
    ptBtampContext btampContext = (ptBtampContext) btampHandle; /*                    */ 
    v_U8_t status;    /*                               */
    BTAMPFSM_INSTANCEDATA_T *instanceVar;

    /*                 */ 
    if ((pBapHCIPhysLinkAccept == NULL) || (NULL == btampContext))
    {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "%s: btampHandle value: %p, pBapHCIPhysLinkAccept is %p",
                 __func__,  btampHandle, pBapHCIPhysLinkAccept); 
      return VOS_STATUS_E_FAULT;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);

    instanceVar = &(btampContext->bapPhysLinkMachine);
    if(DISCONNECTED != instanceVar->stateVar)
    {
       /*                                                 */
        status = WLANBAP_ERROR_MAX_NUM_CNCTS;

    }
    else
    {
        /*                             */ 
        bapEvent.event = eWLAN_BAP_HCI_PHYSICAL_LINK_ACCEPT;
        bapEvent.params = pBapHCIPhysLinkAccept;
        //                                            

        /*                                       */ 
        /*                                                               */
        /*                                                         */ 
        /*                                        */
        vosStatus = WLANBAP_CreateNewPhyLinkCtx ( 
                btampHandle, 
                pBapHCIPhysLinkAccept->phy_link_handle, /*                                         */
                pHddHdl,   /*                                    */
                &btampContext, /*                                                   */ 
                BT_RESPONDER); /*              */ 

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampContext value: %p", __func__,  btampContext);

        /*              */ 
        vosStatus = btampFsm(btampContext, &bapEvent, &status);
  
    }
    /*                                              */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_STATUS_EVENT;
    pBapHCIEvent->u.btampCommandStatusEvent.present = 1;
    pBapHCIEvent->u.btampCommandStatusEvent.status = status;
    pBapHCIEvent->u.btampCommandStatusEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandStatusEvent.command_opcode 
        = BTAMP_TLV_HCI_ACCEPT_PHYSICAL_LINK_CMD;

    /*     */ 

    return VOS_STATUS_SUCCESS;
} /*                            */ 

/*                                                                            

                                              

              
                                                              

               
        

             

      
                                                                              
                                                                                        
   
          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                      
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPPhysicalLinkDisconnect 
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Disconnect_Physical_Link_Cmd   *pBapHCIPhysLinkDisconnect,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    tWLAN_BAPEvent bapEvent; /*                     */
    VOS_STATUS  vosStatus;
    /*                                                         */ 
    //                                                 
    ptBtampContext btampContext = (ptBtampContext) btampHandle; /*                    */ 
    v_U8_t status;    /*                               */

    /*                 */ 
    if (pBapHCIPhysLinkDisconnect == NULL) {
      return VOS_STATUS_E_FAULT;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                                   */
    if (pBapHCIPhysLinkDisconnect->phy_link_handle != btampContext->phy_link_handle) 
    {
        /*                                              */ 
        pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_STATUS_EVENT;
        pBapHCIEvent->u.btampCommandStatusEvent.present = 1;
        pBapHCIEvent->u.btampCommandStatusEvent.status = WLANBAP_ERROR_NO_CNCT;
        pBapHCIEvent->u.btampCommandStatusEvent.num_hci_command_packets = 1;
        pBapHCIEvent->u.btampCommandStatusEvent.command_opcode
            = BTAMP_TLV_HCI_DISCONNECT_PHYSICAL_LINK_CMD;
        return VOS_STATUS_SUCCESS;
    }

    /*                             */ 
    bapEvent.event = eWLAN_BAP_HCI_PHYSICAL_LINK_DISCONNECT;
    bapEvent.params = pBapHCIPhysLinkDisconnect;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampContext value: %p", __func__,  btampContext);

    /*              */ 
    vosStatus = btampFsm(btampContext, &bapEvent, &status);
  
        /*                             */ 
    bapEvent.event =  eWLAN_BAP_MAC_READY_FOR_CONNECTIONS;
    bapEvent.params = pBapHCIPhysLinkDisconnect;

        /*              */ 
    vosStatus = btampFsm(btampContext, &bapEvent, &status);


    /*                                              */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_STATUS_EVENT;
    pBapHCIEvent->u.btampCommandStatusEvent.present = 1;
    pBapHCIEvent->u.btampCommandStatusEvent.status = status;
    pBapHCIEvent->u.btampCommandStatusEvent.num_hci_command_packets = 1;
    pBapHCIEvent->u.btampCommandStatusEvent.command_opcode
        = BTAMP_TLV_HCI_DISCONNECT_PHYSICAL_LINK_CMD;

    /*     */ 

    return VOS_STATUS_SUCCESS;
} /*                                */

/*                                                                            

                                         

              
                                                         

               
        

             

      
                                                                              
                                                                              
   
          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                 
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPLogicalLinkCreate
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Create_Logical_Link_Cmd   *pBapHCILogLinkCreate,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    tBtampHCI_Event bapHCIEvent; /*                                  */
    VOS_STATUS  vosStatus;
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    v_U16_t log_link_index = 0;
    BTAMPFSM_INSTANCEDATA_T *instanceVar = &(btampContext->bapPhysLinkMachine);
    VOS_STATUS  retval;
    v_U16_t index_for_logLinkCtx = 0;
    /*                                                                       */


    /*                 */ 
    if (btampHandle == NULL) {
      return VOS_STATUS_E_FAULT;
    }

    /*                 */ 
    if (pBapHCILogLinkCreate == NULL) {
      return VOS_STATUS_E_FAULT;
    }


    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                                                          
                                                              
                       */
    /*                                                         */ 
    bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_STATUS_EVENT;
    bapHCIEvent.u.btampCommandStatusEvent.present = 1;
    bapHCIEvent.u.btampCommandStatusEvent.num_hci_command_packets = 1;
    bapHCIEvent.u.btampCommandStatusEvent.command_opcode 
        = BTAMP_TLV_HCI_CREATE_LOGICAL_LINK_CMD;

    retval = VOS_STATUS_E_FAILURE;
    if(DISCONNECTED == instanceVar->stateVar)
    {
       /*                                              */
        pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status =
            WLANBAP_ERROR_CMND_DISALLOWED;
        bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_NO_CNCT;

    }
    else if (CONNECTED != instanceVar->stateVar)
    {
        /*                                              */
        pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status =
            WLANBAP_ERROR_CMND_DISALLOWED;
        bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_CMND_DISALLOWED;
    }
    else if (pBapHCILogLinkCreate->phy_link_handle != btampContext->phy_link_handle)
    {
       /*                              */
        pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status =
            WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
        bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
    }
    else
    {
        btampContext->btamp_logical_link_state = WLAN_BAPLogLinkInProgress;

        if( TRUE == btampContext->btamp_logical_link_cancel_pending )
        {
            pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status =
                WLANBAP_ERROR_NO_CNCT;
            bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_NO_CNCT;
            btampContext->btamp_logical_link_state = WLAN_BAPLogLinkClosed;
            btampContext->btamp_logical_link_cancel_pending = FALSE;
        }
        else
        {
            /*                                                                    
                                                                                 
                                                     */
            if(FALSE == btampContext->btamp_async_logical_link_create)
            {
                 /*                                                    */
                 vosStatus = WLANBAP_CreateNewLogLinkCtx( 
                   btampContext, /*                              */ 
                   pBapHCILogLinkCreate->phy_link_handle, /*                                         */
                   pBapHCILogLinkCreate->tx_flow_spec, /*                                      */
                   pBapHCILogLinkCreate->rx_flow_spec, /*                                      */
                   &log_link_index /*                                     */
                   );
                 if (VOS_STATUS_SUCCESS != vosStatus)
                 {
                     /*                          */
                     pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status =
                        WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
                     bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
                     btampContext->btamp_logical_link_state = WLAN_BAPLogLinkClosed;
                 }
                 else
                 {
                      retval = VOS_STATUS_SUCCESS;
                      bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_STATUS_SUCCESS;

                      pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status = WLANBAP_STATUS_SUCCESS;
                      btampContext->btamp_logical_link_state = WLAN_BAPLogLinkOpen;
                 }
            }
            else
            {
                btampContext->btamp_logical_link_req_info.phyLinkHandle = 
                    pBapHCILogLinkCreate->phy_link_handle;
                vos_mem_copy(btampContext->btamp_logical_link_req_info.txFlowSpec,
                             pBapHCILogLinkCreate->tx_flow_spec, 18);
                vos_mem_copy(btampContext->btamp_logical_link_req_info.rxFlowSpec,
                             pBapHCILogLinkCreate->rx_flow_spec, 18);
                btampContext->btamp_async_logical_link_create = FALSE;
                vosStatus = btampEstablishLogLink(btampContext);
                if(VOS_STATUS_SUCCESS == vosStatus)
                {
                    retval = VOS_STATUS_E_BUSY;//                                                     
                }
                else
                {
                    pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status =
                        WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
                    bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
                    btampContext->btamp_logical_link_state = WLAN_BAPLogLinkClosed;
                }
    
            }
        }
    }

    vosStatus = (*btampContext->pBapHCIEventCB) 
                (  
                 btampContext->pHddHdl,   /*                                               */
                 &bapHCIEvent, /*                                  */
                 VOS_TRUE /*                                       */ 
                );

    index_for_logLinkCtx = log_link_index >> 8;
    /*                                                     */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_LOGICAL_LINK_COMPLETE_EVENT;
    pBapHCIEvent->u.btampLogicalLinkCompleteEvent.present = 1;

    /*                                     */
    pBapHCIEvent->u.btampLogicalLinkCompleteEvent.log_link_handle 
        = log_link_index;
    pBapHCIEvent->u.btampLogicalLinkCompleteEvent.phy_link_handle 
        = pBapHCILogLinkCreate->phy_link_handle;
    pBapHCIEvent->u.btampLogicalLinkCompleteEvent.flow_spec_id
        = btampContext->btampLogLinkCtx[index_for_logLinkCtx].btampFlowSpec.flow_spec_id;

    /*     */ 

    return retval;
} /*                           */

/*                                                                            

                                         

              
                                                         

               
        

             

      
                                                                              
                                                                              
   
          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                 
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPLogicalLinkAccept
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Accept_Logical_Link_Cmd   *pBapHCILogLinkAccept,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    tBtampHCI_Event bapHCIEvent; /*                                  */
    VOS_STATUS  vosStatus;
    ptBtampContext btampContext = (ptBtampContext) btampHandle;
    v_U16_t log_link_index = 0;
    BTAMPFSM_INSTANCEDATA_T *instanceVar = &(btampContext->bapPhysLinkMachine);
    VOS_STATUS  retval;
    v_U16_t index_for_logLinkCtx;
    /*                                                                       */


    /*                 */ 
    if (btampHandle == NULL) {
      return VOS_STATUS_E_FAULT;
    }

    /*                 */ 
    if (pBapHCILogLinkAccept == NULL) {
      return VOS_STATUS_E_FAULT;
    }


    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampHandle value: %p", __func__,  btampHandle);

    /*                                                          
                                                              
                       */
    /*                                                         */ 
    bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_STATUS_EVENT;
    bapHCIEvent.u.btampCommandStatusEvent.present = 1;
    bapHCIEvent.u.btampCommandStatusEvent.num_hci_command_packets = 1;
    bapHCIEvent.u.btampCommandStatusEvent.command_opcode 
        = BTAMP_TLV_HCI_ACCEPT_LOGICAL_LINK_CMD;

    retval = VOS_STATUS_E_FAILURE;
    if(DISCONNECTED == instanceVar->stateVar)
    {
       /*                                              */
        pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status =
            WLANBAP_ERROR_CMND_DISALLOWED;
        bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_NO_CNCT;

    }
    else if (CONNECTED != instanceVar->stateVar)
    {
        /*                                              */
        pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status =
            WLANBAP_ERROR_CMND_DISALLOWED;
        bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_CMND_DISALLOWED;
    }
    else if (pBapHCILogLinkAccept->phy_link_handle != btampContext->phy_link_handle)
    {
       /*                              */
        pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status =
            WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
        bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
    }
    else
    {
        btampContext->btamp_logical_link_state = WLAN_BAPLogLinkInProgress;
        if( TRUE == btampContext->btamp_logical_link_cancel_pending )
        {
            pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status =
                WLANBAP_ERROR_NO_CNCT;
            bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_NO_CNCT;
            btampContext->btamp_logical_link_state = WLAN_BAPLogLinkClosed;
            btampContext->btamp_logical_link_cancel_pending = FALSE;
        }
        else
        {
            /*                                                                    
                                                                                 
                                                     */
            if(FALSE == btampContext->btamp_async_logical_link_create)
            {
                /*                                                    */
                vosStatus = WLANBAP_CreateNewLogLinkCtx( 
                        btampContext, /*                              */ 
                        pBapHCILogLinkAccept->phy_link_handle, /*                                         */
                        pBapHCILogLinkAccept->tx_flow_spec, /*                                      */
                        pBapHCILogLinkAccept->rx_flow_spec, /*                                      */
                        &log_link_index /*                                     */
                        );
                if (VOS_STATUS_SUCCESS != vosStatus)
                {
                    /*                          */
                    pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status =
                        WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
                    bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
                    btampContext->btamp_logical_link_state = WLAN_BAPLogLinkClosed;
                }
                else
                {
                    retval = VOS_STATUS_SUCCESS;
                    bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_STATUS_SUCCESS;

                    pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status = WLANBAP_STATUS_SUCCESS;
                    btampContext->btamp_logical_link_state = WLAN_BAPLogLinkOpen;
                }
            }
            else
            {
                btampContext->btamp_logical_link_req_info.phyLinkHandle = 
                    pBapHCILogLinkAccept->phy_link_handle;
                vos_mem_copy(btampContext->btamp_logical_link_req_info.txFlowSpec,
                             pBapHCILogLinkAccept->tx_flow_spec, 18);
                vos_mem_copy(btampContext->btamp_logical_link_req_info.rxFlowSpec,
                             pBapHCILogLinkAccept->rx_flow_spec, 18);
                btampContext->btamp_async_logical_link_create = FALSE;
                vosStatus = btampEstablishLogLink(btampContext);
                if(VOS_STATUS_SUCCESS == vosStatus)
                {
                    retval = VOS_STATUS_E_BUSY;//                                                     
                }
                else
                {
                    pBapHCIEvent->u.btampLogicalLinkCompleteEvent.status =
                        WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
                    bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
                    btampContext->btamp_logical_link_state = WLAN_BAPLogLinkClosed;
                }
    
            }
        }
    }
    vosStatus = (*btampContext->pBapHCIEventCB) 
            (  
             btampContext->pHddHdl,   /*                                               */
             &bapHCIEvent, /*                                  */
             VOS_TRUE /*                                       */ 
             );

    index_for_logLinkCtx = log_link_index >> 8;

    /*                                                     */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_LOGICAL_LINK_COMPLETE_EVENT;
    pBapHCIEvent->u.btampLogicalLinkCompleteEvent.present = 1;
    /*                                     */
    pBapHCIEvent->u.btampLogicalLinkCompleteEvent.log_link_handle 
        = log_link_index;
    pBapHCIEvent->u.btampLogicalLinkCompleteEvent.phy_link_handle 
        = pBapHCILogLinkAccept->phy_link_handle;
    pBapHCIEvent->u.btampLogicalLinkCompleteEvent.flow_spec_id
        = btampContext->btampLogLinkCtx[index_for_logLinkCtx].btampFlowSpec.flow_spec_id;

    /*     */ 

    return retval;
} /*                           */

/*                                                                            

                                             

              
                                                             

               
        

             

      
                                                                              
                                                                                      
   
          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                     
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPLogicalLinkDisconnect
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Disconnect_Logical_Link_Cmd   *pBapHCILogLinkDisconnect,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
    tBtampHCI_Event     bapHCIEvent; /*                                  */
    ptBtampContext      btampContext = (ptBtampContext) btampHandle;
    tpBtampLogLinkCtx   pLogLinkContext;
    VOS_STATUS          retval = VOS_STATUS_SUCCESS;
    v_U8_t              log_link_index;

   /*                                                                      */

   /*                                                                        
                
                                                                            */
    if (( NULL == pBapHCILogLinkDisconnect ) ||
        ( NULL == btampContext))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
                   "Critical error: Invalid input parameter on %s", 
                   __func__); 
        return VOS_STATUS_E_FAULT; 
    }

    /*                                       */
    log_link_index = ((pBapHCILogLinkDisconnect->log_link_handle) >> 8);

    if( log_link_index >= WLANBAP_MAX_LOG_LINKS )
    {
       VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
                  "Critical error: Invalid input parameter on %s", 
                  __func__); 
        /*                                                                  
                                                                */
        pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_DISCONNECT_LOGICAL_LINK_COMPLETE_EVENT;
        pBapHCIEvent->u.btampDisconnectLogicalLinkCompleteEvent.present = 1;
        pBapHCIEvent->u.btampDisconnectLogicalLinkCompleteEvent.status =
            WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
        return VOS_STATUS_E_INVAL; 

    }

#ifdef BAP_DEBUG
  /*                                      */
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
            "WLAN BAP Context Monitor: btampContext value = %p in %s:%d", btampContext, __func__, __LINE__ );
#endif //         

    bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_STATUS_EVENT;
    bapHCIEvent.u.btampCommandStatusEvent.present = 1;
    bapHCIEvent.u.btampCommandStatusEvent.num_hci_command_packets = 1;
    bapHCIEvent.u.btampCommandStatusEvent.command_opcode 
        = BTAMP_TLV_HCI_DISCONNECT_LOGICAL_LINK_CMD;

   /*                                                                        
                                                                      
                                                                              
                                                                                                           
                                                                            */
    pLogLinkContext = 
    &(btampContext->btampLogLinkCtx[log_link_index]);
    
    //                                        
    if ((VOS_FALSE == pLogLinkContext->present) ||
        (pBapHCILogLinkDisconnect->log_link_handle != pLogLinkContext->log_link_handle))
    {
        /*                                                               
                                                   */
        pBapHCIEvent->u.btampDisconnectLogicalLinkCompleteEvent.status =
            WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
        bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_NO_CNCT;
        retval = VOS_STATUS_E_FAILURE;
#ifdef BAP_DEBUG
        /*                */
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                "%s:%d Invalid Logical Link handle(should be) = %d(%d)", __func__, __LINE__,  
                pBapHCILogLinkDisconnect->log_link_handle, pLogLinkContext->log_link_handle);
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                " Logical Link index = %d", log_link_index);
#endif //         
    }
    else
    {
        /*                                             */ 
        bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_STATUS_SUCCESS;
        pBapHCIEvent->u.btampDisconnectLogicalLinkCompleteEvent.status 
            = WLANBAP_STATUS_SUCCESS;
    

        pLogLinkContext->present         = VOS_FALSE; 
        pLogLinkContext->uTxPktCompleted = 0;
        pLogLinkContext->log_link_handle = 0;
        /*                                        */
        btampContext->total_log_link_index--;
        btampContext->btamp_logical_link_state = WLAN_BAPLogLinkClosed;
    }
    
    /*                                 */
    (*btampContext->pBapHCIEventCB) 
        (  
            btampContext->pHddHdl,   /*                                               */
            &bapHCIEvent, /*                                  */
            VOS_TRUE /*                                       */ 
        );

    /*                                                     */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_DISCONNECT_LOGICAL_LINK_COMPLETE_EVENT;
    pBapHCIEvent->u.btampDisconnectLogicalLinkCompleteEvent.present = 1;
    /*                                     */
    pBapHCIEvent->u.btampDisconnectLogicalLinkCompleteEvent.log_link_handle 
        = pBapHCILogLinkDisconnect->log_link_handle;
    pBapHCIEvent->u.btampDisconnectLogicalLinkCompleteEvent.reason 
        = WLANBAP_ERROR_TERM_BY_LOCAL_HOST;

    return retval;
} /*                               */

/*                                                                            

                                         

              
                                                         

               
        

             

      
                                                                              
                                                                              
   
          
                                                                      
                                                                       
                                                                 
                                                                     
                                                                       
                                              
   
              
                                                              

                                                                 
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPLogicalLinkCancel
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Logical_Link_Cancel_Cmd   *pBapHCILogLinkCancel,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
   ptBtampContext btampContext;
   BTAMPFSM_INSTANCEDATA_T *instanceVar;
    /*                 */ 
    if ((btampHandle == NULL) || (pBapHCILogLinkCancel == NULL) || 
        (pBapHCIEvent == NULL))
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
            "%s: Null Parameters Not allowed", __func__); 
        return VOS_STATUS_E_FAULT;
    }

    btampContext = (ptBtampContext) btampHandle;
    instanceVar = &(btampContext->bapPhysLinkMachine);

    /*                                                         */ 
    pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_COMPLETE_EVENT;
    pBapHCIEvent->u.btampCommandCompleteEvent.present = 1;
    pBapHCIEvent->u.btampCommandCompleteEvent.command_opcode =
        BTAMP_TLV_HCI_LOGICAL_LINK_CANCEL_CMD;
    pBapHCIEvent->u.btampCommandCompleteEvent.num_hci_command_packets = 1;

    if (pBapHCILogLinkCancel->phy_link_handle != btampContext->phy_link_handle) 
    {
        pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Logical_Link_Cancel.status =
            WLANBAP_ERROR_NO_CNCT;
    }
    else
    {
        /*                                                                        
                                                        
                                                                   
                                                        */
        if(WLAN_BAPLogLinkClosed == btampContext->btamp_logical_link_state )
        {
           /*                                              */
           pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Logical_Link_Cancel.status =
               WLANBAP_ERROR_NO_CNCT;
        }
        else if(WLAN_BAPLogLinkOpen == btampContext->btamp_logical_link_state )
        {
           /*                                               */
           pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Logical_Link_Cancel.status =
               WLANBAP_ERROR_MAX_NUM_ACL_CNCTS;       
        }
        else if(WLAN_BAPLogLinkInProgress == btampContext->btamp_logical_link_state )
        {
           /*                                                                         
                             */
            btampContext->btamp_logical_link_cancel_pending = TRUE;
            pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Logical_Link_Cancel.status =
                WLANBAP_STATUS_SUCCESS;       
        }
        else
        {
            pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Logical_Link_Cancel.status =
                WLANBAP_ERROR_NO_CNCT;
        }
    }
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Logical_Link_Cancel.phy_link_handle =
        pBapHCILogLinkCancel->phy_link_handle;
    /*                                                                           
                      */
    pBapHCIEvent->u.btampCommandCompleteEvent.cc_event.Logical_Link_Cancel.tx_flow_spec_id = 
        pBapHCILogLinkCancel->tx_flow_spec_id;

    return VOS_STATUS_SUCCESS;
} /*                           */

/*                                                                            

                                      

              
                                                         
                                                                          
                   

               
        

             

      
                                                                              
                                                                            
   
          
                                                                    
                                                                       
                                                               
   
              
                                                              

                                                                  
                                
  
               
  
                                                                            */
VOS_STATUS  
WLAN_BAPFlowSpecModify
( 
  ptBtampHandle btampHandle,
  tBtampTLVHCI_Flow_Spec_Modify_Cmd   *pBapHCIFlowSpecModify,
  tpBtampHCI_Event pBapHCIEvent /*                                  */
                                /*                                              */
)
{
   v_U16_t                  index_for_logLinkHandle = 0;
   ptBtampContext           btampContext;
   tpBtampLogLinkCtx        pLogLinkContext;
   v_U32_t                  retval;
   VOS_STATUS               vosStatus = VOS_STATUS_SUCCESS;
   tBtampHCI_Event          bapHCIEvent; /*                                  */
   /*                                                                       */
   /*                 */ 
   if ((btampHandle == NULL) || (pBapHCIFlowSpecModify == NULL) || 
       (pBapHCIEvent == NULL))
   {
       VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
           "%s: Null Parameters Not allowed", __func__); 
       return VOS_STATUS_E_FAULT;
   }

   btampContext = (ptBtampContext) btampHandle;

   index_for_logLinkHandle = pBapHCIFlowSpecModify->log_link_handle >> 8; /*                                     */
   VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
              " %s:index_for_logLinkHandle=%d", __func__,index_for_logLinkHandle);

   bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_COMMAND_STATUS_EVENT;
   bapHCIEvent.u.btampCommandStatusEvent.present = 1;
   bapHCIEvent.u.btampCommandStatusEvent.num_hci_command_packets = 1;
   bapHCIEvent.u.btampCommandStatusEvent.command_opcode 
       = BTAMP_TLV_HCI_FLOW_SPEC_MODIFY_CMD;

   /*                                                                        
                                                                     
                                                                           */
   //                                                                      

   /*                                                                        
                                            
                                                                           */
   pLogLinkContext = &(btampContext->btampLogLinkCtx[index_for_logLinkHandle]);

   /*                                                 */
   retval = btampUnpackTlvFlow_Spec((void *)btampContext, pBapHCIFlowSpecModify->tx_flow_spec,
                                    WLAN_BAP_PAL_FLOW_SPEC_TLV_LEN,
                                    &pLogLinkContext->btampFlowSpec);
   if (retval != BTAMP_PARSE_SUCCESS)
   {
     /*                                          */
     vosStatus = VOS_STATUS_E_FAILURE;
     pBapHCIEvent->u.btampFlowSpecModifyCompleteEvent.status =
         WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
     bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
   }
   else
   {
      bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_STATUS_SUCCESS;
      pBapHCIEvent->u.btampFlowSpecModifyCompleteEvent.status 
          = WLANBAP_STATUS_SUCCESS;

   }
           /*                                 */
   vosStatus =   
      (*btampContext->pBapHCIEventCB) 
          (  
           btampContext->pHddHdl,   /*                                               */
           &bapHCIEvent, /*                                  */
           VOS_TRUE /*                                       */ 
          );

   /*                                                         */ 
   pBapHCIEvent->bapHCIEventCode = BTAMP_TLV_HCI_FLOW_SPEC_MODIFY_COMPLETE_EVENT;
   pBapHCIEvent->u.btampFlowSpecModifyCompleteEvent.present = 1;
   pBapHCIEvent->u.btampFlowSpecModifyCompleteEvent.log_link_handle = 
      pBapHCIFlowSpecModify->log_link_handle;

   return vosStatus;
} /*                        */


void WLAN_BAPEstablishLogicalLink(ptBtampContext btampContext)
{
    tBtampHCI_Event bapHCIEvent; /*                                  */
    v_U16_t         log_link_index = 0;
    v_U16_t         index_for_logLinkCtx = 0;
    VOS_STATUS      vosStatus = VOS_STATUS_SUCCESS;

    if (btampContext == NULL) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
            "%s: Null Parameters Not allowed", __func__); 
        return;
    }

    if( TRUE == btampContext->btamp_logical_link_cancel_pending )
    {
        bapHCIEvent.u.btampCommandStatusEvent.status = WLANBAP_ERROR_NO_CNCT;
        btampContext->btamp_logical_link_state = WLAN_BAPLogLinkClosed;
        btampContext->btamp_logical_link_cancel_pending = FALSE;
    }
    else
    {
        /*                                                    */
        vosStatus = WLANBAP_CreateNewLogLinkCtx( 
            btampContext, /*                              */ 
            btampContext->btamp_logical_link_req_info.phyLinkHandle, /*                                         */
            btampContext->btamp_logical_link_req_info.txFlowSpec, /*                                      */
            btampContext->btamp_logical_link_req_info.rxFlowSpec, /*                                      */
            &log_link_index /*                                     */
            );
        if (VOS_STATUS_SUCCESS != vosStatus)
        {
            /*                          */
            bapHCIEvent.u.btampLogicalLinkCompleteEvent.status = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
            btampContext->btamp_logical_link_state = WLAN_BAPLogLinkClosed;
        }
        else
        {
            bapHCIEvent.u.btampLogicalLinkCompleteEvent.status = WLANBAP_STATUS_SUCCESS;
            btampContext->btamp_logical_link_state = WLAN_BAPLogLinkOpen;
        }
    }
    
    index_for_logLinkCtx = log_link_index >> 8;
    /*                                                     */ 
    bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_LOGICAL_LINK_COMPLETE_EVENT;
    bapHCIEvent.u.btampLogicalLinkCompleteEvent.present = 1;

    /*                                     */
    bapHCIEvent.u.btampLogicalLinkCompleteEvent.log_link_handle 
        = log_link_index;
    bapHCIEvent.u.btampLogicalLinkCompleteEvent.phy_link_handle 
        = btampContext->btamp_logical_link_req_info.phyLinkHandle;
    bapHCIEvent.u.btampLogicalLinkCompleteEvent.flow_spec_id
        = btampContext->btampLogLinkCtx[index_for_logLinkCtx].btampFlowSpec.flow_spec_id;

    vosStatus = (*btampContext->pBapHCIEventCB) 
    (  
     btampContext->pHddHdl,   /*                                               */
     &bapHCIEvent, /*                                  */
     VOS_TRUE /*                                       */ 
     );
    return;
}




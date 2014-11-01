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

                      b t a m p F s m . C

  OVERVIEW:

  This software unit holds the implementation of the Finite State Machine that
  controls the operation of each individual AMP Physical link.
  (Currently, this is limited to ONE link.)

  The btampFsm() routine provided by this module is called by the rest of
  the BT-AMP PAL module whenever a control plane operation occurs that requires a
  major state transition.

  DEPENDENCIES:

  Are listed for each API below.


  Copyright (c) 2008 QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*                                                                           

                                           


                                                                       
                                                                


                                                                                                                                                                        


                                      
                                                                             
                                    

                                                                           */

/*                                                                */
/*                                                                 */

/*                                                                            
                
                                                                            */


#include "fsmDefs.h"
//                     
#include "bapInternal.h"
#include "btampFsm_ext.h"

//                                        
#include "bapApiTimer.h"

//                                  
#include "bapRsn8021xFsm.h"

#include "bapRsn8021xAuthFsm.h"
//                                
#include "sme_Api.h"

//                                
#include "pmcApi.h"

//                                                                        
#include "bapApiExt.h"

#include "wlan_nlink_common.h"
#include "wlan_btc_svc.h"

//                                  
//                                                                                 
typedef struct sAniSirGlobal *tpAniSirGlobal;
#include "dot11f.h"

#if 0
/*
                          
                                              
                                                 
                              
 */
#define eWLAN_BAP_MAC_START_BSS_SUCCESS /*                                                           */
#define eWLAN_BAP_MAC_START_FAILS /*                                                                                          */
#define eWLAN_BAP_MAC_SCAN_COMPLETE /*                         */
#define eWLAN_BAP_CHANNEL_NOT_SELECTED /*                                                                                  */
#define eWLAN_BAP_MAC_CONNECT_COMPLETED /*                                                              */
#define eWLAN_BAP_MAC_CONNECT_FAILED /*                                                                                          */
#define eWLAN_BAP_MAC_CONNECT_INDICATION /*                                                                   */
#define eWLAN_BAP_RSN_SUCCESS /*                                     */
#define eWLAN_BAP_RSN_FAILURE /*                                     */
#define eWLAN_BAP_MAC_KEY_SET_SUCCESS /*                                                       */
#define eWLAN_BAP_MAC_INDICATES_MEDIA_DISCONNECTION /*                                                                */
#define eWLAN_BAP_MAC_READY_FOR_CONNECTIONS /*                                                           */
#define eWLAN_BAP_CHANNEL_SELECTION_FAILED /*     */

#endif /*   */

/*                                                                             
            */
#define WLAN_BAP_MIN_24G_CH  1
#define WLAN_BAP_MAX_24G_CH  14


/*                                                                  */
VOS_STATUS
signalHCIDiscLogLinkCompEvent
( 
  ptBtampContext btampContext, /*                    */    
  v_U8_t status,    /*                   */
  v_U16_t log_link_handle,  /*                                   */
  v_U8_t reason    /*                        */
);


/*                                                        */
int
bapSuppDisconnect(tBtampContext *ctx)
{
    //                                         
    //                                                                             
    return ANI_OK;
}

int
bapAuthDisconnect(tBtampContext *ctx)
{
    //                                         
    //                                                                             
    return ANI_OK;
}

VOS_STATUS
bapSetKey( v_PVOID_t pvosGCtx, tCsrRoamSetKey *pSetKeyInfo )
{
    tWLAN_BAPEvent bapEvent; /*                     */
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    ptBtampContext btampContext; /*                        */ 
    v_U8_t status;    /*                               */
    eHalStatus  halStatus;
    v_U32_t roamId = 0xFF;
    tHalHandle     hHal = NULL;
    v_U8_t groupMac[ANI_MAC_ADDR_SIZE] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  
 
    /*                 */ 
    if ((pvosGCtx == NULL) || (pSetKeyInfo == NULL))
    {
      return VOS_STATUS_E_FAULT;
    }

    btampContext = VOS_GET_BAP_CB(pvosGCtx); 
    /*                 */ 
    if ( btampContext == NULL)
    {
      return VOS_STATUS_E_FAULT;
    }
    hHal = VOS_GET_HAL_CB(btampContext->pvosGCtx);
    if (NULL == hHal) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "hHal is NULL in %s", __func__);

        return VOS_STATUS_E_FAULT;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: btampContext value: %p", __func__,  btampContext);

    /*                             */ 
    bapEvent.event = eWLAN_BAP_RSN_SUCCESS;
    bapEvent.params = NULL;

    /*                                                       */ 
    /*                                                    */ 
    vosStatus = btampFsm(btampContext, &bapEvent, &status);

    /*                      */ 
    halStatus = sme_RoamSetKey( 
            hHal, 
            btampContext->sessionId, 
            pSetKeyInfo, 
            &roamId );
    if ( halStatus != eHAL_STATUS_SUCCESS )
    {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
              "[%4d] sme_RoamSetKey returned ERROR status= %d", __LINE__, halStatus );
        return VOS_STATUS_E_FAULT;
    }
                         
    /*                   */ 
    vos_mem_copy( pSetKeyInfo->peerMac, groupMac, sizeof( tAniMacAddr ) );
    halStatus = sme_RoamSetKey( 
            hHal, 
            btampContext->sessionId, 
            pSetKeyInfo, 
            &roamId );
    if ( halStatus != eHAL_STATUS_SUCCESS )
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
                "[%4d] sme_RoamSetKey returned ERROR status= %d", __LINE__, halStatus );
      return VOS_STATUS_E_FAULT;
    }
  
    return vosStatus;
}

/*
                          
                                              
                                                 
                              
 */
#define DUMPLOG_ON
#if defined DUMPLOG_ON
#define DUMPLOG(n, name1, name2, aStr, size) do {                       \
        int i;                                                          \
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%d. %s: %s = \n", n, name1, name2); \
        for (i = 0; i < size; i++)                                      \
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%2.2x%s", ((unsigned char *)aStr)[i], i % 16 == 15 ? "\n" : " "); \
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "\n"); \
    } while (0)
#else
#define DUMPLOG(n, name1, name2, aStr, size)
#endif

/*
                               
 */

VOS_STATUS
gotoS1
( 
    ptBtampContext btampContext, /*                    */    
    ptWLAN_BAPEvent bapEvent, /*                     */
    tWLAN_BAPRole BAPDeviceRole,
    v_U8_t *status    /*                               */
) 
{
  tBtampTLVHCI_Create_Physical_Link_Cmd *pBapHCIPhysLinkCreate 
      = (tBtampTLVHCI_Create_Physical_Link_Cmd *) bapEvent->params;
  tBtampTLVHCI_Accept_Physical_Link_Cmd *pBapHCIPhysLinkAccept 
      = (tBtampTLVHCI_Accept_Physical_Link_Cmd *) bapEvent->params;
  VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
  v_U32_t     conAcceptTOInterval;
  /*                                                                       */

  /*               */
  btampContext->BAPDeviceRole = BAPDeviceRole;

    switch(BAPDeviceRole)
    {
    case BT_INITIATOR:
      /*                                     */
      btampContext->phy_link_handle = pBapHCIPhysLinkCreate->phy_link_handle;
      /*                                                */
      btampContext->key_type = pBapHCIPhysLinkCreate->key_type;
      btampContext->key_length = pBapHCIPhysLinkCreate->key_length;
      vos_mem_copy( 
              btampContext->key_material, 
              pBapHCIPhysLinkCreate->key_material, 
              32);  /*                        */
      break;
    case BT_RESPONDER:
      /*                                     */
      btampContext->phy_link_handle = pBapHCIPhysLinkAccept->phy_link_handle;
      /*                                                */
      btampContext->key_type = pBapHCIPhysLinkAccept->key_type; 
      btampContext->key_length = pBapHCIPhysLinkAccept->key_length; 
      vos_mem_copy( 
              btampContext->key_material, 
              pBapHCIPhysLinkAccept->key_material, 
              32);  /*                        */
      break;
    default:
      *status = WLANBAP_ERROR_HOST_REJ_RESOURCES;     /*                               */
      return VOS_STATUS_E_RESOURCES;
  }

  conAcceptTOInterval = (btampContext->bapConnectionAcceptTimerInterval * 5)/ 8;
  /*                                   */
  vosStatus = WLANBAP_StartConnectionAcceptTimer ( 
          btampContext, 
          conAcceptTOInterval);

  *status = WLANBAP_STATUS_SUCCESS;     /*                               */
  
  return VOS_STATUS_SUCCESS;
} //      

VOS_STATUS
gotoScanning
( 
    ptBtampContext btampContext, /*                    */    
    tWLAN_BAPRole BAPDeviceRole,
    v_U8_t *status    /*                               */
)
{
  /*                         */
  //                 
  *status = WLANBAP_STATUS_SUCCESS;     /*                               */

  return VOS_STATUS_SUCCESS;
}


#if 0
/*                                                                          
 
                                

                                                   

                                 
                                                                      
                                                                                           
                                              
                                                   
                       
                                                                            */
#endif
eCsrRoamBssType 
convertRoleToBssType
(
    tWLAN_BAPRole bapRole  /*             */
) 
{
    switch (bapRole)
    {
        case BT_RESPONDER: 
            //                            
            return eCSR_BSS_TYPE_WDS_STA;            
            //                                                
            //                                                                                         
        case BT_INITIATOR: 
            //                                                      
            return eCSR_BSS_TYPE_WDS_AP;      
            //                                                                               
        default:
            return eCSR_BSS_TYPE_INFRASTRUCTURE;
    }
} //                     


char hexValue[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                   '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
                  };

#define BAP_MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX_BYTES 8
//                                                        
//                                                                 
//                                  
v_U8_t *
bapBin2Hex(const v_U8_t *bytes, v_U32_t len, char delimiter)
{
  static v_U8_t buf[MAX_BYTES*(2+1)];
  v_U32_t i;
  v_U8_t *ptr;

  len = BAP_MIN(len, MAX_BYTES);
    for (i = 0, ptr = buf; i < len; i++)
    {
    *ptr++ = hexValue[ (bytes[i] >> 4) & 0x0f];
    *ptr++ = hexValue[ bytes[i] & 0x0f];
    *ptr++ = delimiter;
    //                                            
    //         
  }

  //                                                           
  if (len > 0)
      ptr--;
  *ptr = '\0';

  return buf;
}//           

char bapSsidPrefixValue[] = {'A', 'M', 'P', '-'};

v_U8_t * 
convertBSSIDToSSID
(
    v_U8_t *bssid  /*             */
) 
{
    static v_U8_t ssId[32]; 

    vos_mem_copy( 
            ssId, 
            bapSsidPrefixValue, 
            4); 

    vos_mem_copy( 
            &ssId[4], 
            bapBin2Hex(bssid, 6, '-'),
            17); 

    return ssId;
} //                   

VOS_STATUS
convertToCsrProfile
(
    ptBtampContext btampContext, /*                    */    
    eCsrRoamBssType bssType,
    tCsrRoamProfile *pProfile   /*                              */
) 
{
    static v_U8_t btampRSNIE[] = {0x30, 0x14, 0x01, 0x00, 0x00, 0x0f, 0xac, 0x04, 0x01, 0x00, 
                                  0x00, 0x0f, 0xac, 0x04, 0x01, 0x00, 0x00, 0x0f, 0xac, 0x02, 0x00, 0x00
                                 };
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    v_S7_t sessionid = -1;
    tHalHandle     hHal = NULL;
    v_U32_t triplet;
    v_U8_t regulatoryClass;
    v_U8_t firstChannel;
    v_U8_t numChannels;
    /*                                                                       */
    if (NULL == btampContext) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "btampContext is NULL in %s", __func__);

        return VOS_STATUS_E_FAULT;
    }

    hHal = VOS_GET_HAL_CB(btampContext->pvosGCtx);
    if (NULL == hHal) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "hHal is NULL in %s", __func__);

        return VOS_STATUS_E_FAULT;
    }

    //                                                                                                             
    //                                               

    //                
    //                                                                        
    pProfile->BSSType = bssType; 
    //                                                 

    //            

    if ( bssType == eCSR_BSS_TYPE_WDS_STA) 
    {
        pProfile->SSIDs.numOfSSIDs = 2;

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: bssType = %s, SSID specified = %s\n", __func__, "eCSR_BSS_TYPE_WDS_STA", convertBSSIDToSSID(btampContext->btamp_Remote_AMP_Assoc.HC_mac_addr));
    
        vos_mem_zero(pProfile->SSIDs.SSIDList[0].SSID.ssId, 
                sizeof(pProfile->SSIDs.SSIDList[0].SSID.ssId));
        vos_mem_copy(pProfile->SSIDs.SSIDList[0].SSID.ssId,
                convertBSSIDToSSID(btampContext->btamp_Remote_AMP_Assoc.HC_mac_addr),
                21);  //                                 
        pProfile->SSIDs.SSIDList[0].SSID.length = 21;

        vos_mem_zero(pProfile->SSIDs.SSIDList[1].SSID.ssId, 
                sizeof(pProfile->SSIDs.SSIDList[1].SSID.ssId));
        vos_mem_copy(pProfile->SSIDs.SSIDList[1].SSID.ssId,
                convertBSSIDToSSID(btampContext->self_mac_addr),
                21);  //                                 
        pProfile->SSIDs.SSIDList[1].SSID.length = 21;
 
        //                              
        pProfile->BSSIDs.numOfBSSIDs = 1;
        vos_mem_copy(pProfile->BSSIDs.bssid, 
                btampContext->btamp_Remote_AMP_Assoc.HC_mac_addr, 
                sizeof( tCsrBssid ) ); 
 
    }
    else if ( bssType == eCSR_BSS_TYPE_WDS_AP)
    {
        pProfile->SSIDs.numOfSSIDs = 1;

        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: bssType = %s, SSID specified = %s\n", __func__, "eCSR_BSS_TYPE_WDS_AP", convertBSSIDToSSID(btampContext->self_mac_addr));
    
        vos_mem_zero(pProfile->SSIDs.SSIDList[0].SSID.ssId, 
                sizeof(pProfile->SSIDs.SSIDList[0].SSID.ssId));
        vos_mem_copy(pProfile->SSIDs.SSIDList[0].SSID.ssId,
                convertBSSIDToSSID(btampContext->self_mac_addr),
                21);  //                                 
        pProfile->SSIDs.SSIDList[0].SSID.length = 21;
 
#if 0
        //                                          
        pProfile->BSSIDs.numOfBSSIDs = 0;
#endif // 
 
        //                                     
        pProfile->BSSIDs.numOfBSSIDs = 1;
        vos_mem_copy(pProfile->BSSIDs.bssid, 
                btampContext->self_mac_addr,
                sizeof( tCsrBssid ) ); 
 
    }
    else
    //                                                               
    {
        pProfile->SSIDs.numOfSSIDs = 1;
    
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s: bssType = %s, SSID specified = %s\n", __func__, "eCSR_BSS_TYPE_WDS_STA", convertBSSIDToSSID(btampContext->btamp_Remote_AMP_Assoc.HC_mac_addr));

        vos_mem_zero(pProfile->SSIDs.SSIDList[0].SSID.ssId, 
                sizeof(pProfile->SSIDs.SSIDList[0].SSID.ssId));
        vos_mem_copy(pProfile->SSIDs.SSIDList[0].SSID.ssId,
                convertBSSIDToSSID(btampContext->btamp_Remote_AMP_Assoc.HC_mac_addr),
                21);  //                                 
        pProfile->SSIDs.SSIDList[0].SSID.length = 21;
 
        //                              
        pProfile->BSSIDs.numOfBSSIDs = 1;
        vos_mem_copy(pProfile->BSSIDs.bssid, 
                btampContext->btamp_Remote_AMP_Assoc.HC_mac_addr, 
                sizeof( tCsrBssid ) ); 
 
    }
 
    //                        
    //                                                      
    //                                                   
    //                                                          
    pProfile->AuthType.numEntries = 1;
    //                                                            
    pProfile->AuthType.authType[0] = eCSR_AUTH_TYPE_RSN_PSK;

    //                              
    //                                                             
    //                                                              
    pProfile->EncryptionType.numEntries = 1;
    //                                                                    
    pProfile->EncryptionType.encryptionType[0] = eCSR_ENCRYPT_TYPE_AES;

    pProfile->mcEncryptionType.numEntries = 1;
    //                                                                      
    pProfile->mcEncryptionType.encryptionType[0] = eCSR_ENCRYPT_TYPE_AES;

    //              
    //                           
    pProfile->pRSNReqIE = &btampRSNIE[0];
    pProfile->nRSNReqIELength = 0x16; //    
    //                           
 
    /*                                                          */
    pProfile->pWPAReqIE = NULL;
    pProfile->nWPAReqIELength = 0;

    //                               

    /*                                                              */
    pProfile->operationChannel = 0;
    regulatoryClass = 0;
    for (triplet = 0; triplet < btampContext->btamp_Remote_AMP_Assoc.HC_pref_num_triplets; triplet++)
        {
        firstChannel = 0;
        numChannels = 0;

        /*                                     */
        if (btampContext->btamp_Remote_AMP_Assoc.HC_pref_triplets[triplet][0] == 201)
            {
            /*                                              */
            switch (btampContext->btamp_Remote_AMP_Assoc.HC_pref_triplets[triplet][1])
            {
                case 254:
                {
                    /*                                                                     
                                                                  */
                    if ((btampContext->btamp_Remote_AMP_Assoc.HC_pref_country[0] == 'X') &&
                        (btampContext->btamp_Remote_AMP_Assoc.HC_pref_country[1] == 'X'))
                    {
                        regulatoryClass = 254;
                        firstChannel = 1;
                        numChannels = 11;
                    }
                    break;
            }
                case 12:
            {
                    /*                                                              */
                    if ((btampContext->btamp_Remote_AMP_Assoc.HC_pref_country[0] == 'U') &&
                        (btampContext->btamp_Remote_AMP_Assoc.HC_pref_country[1] == 'S'))
                    {
                        regulatoryClass = 12;
                        firstChannel = 1;
                        numChannels = 11;
            }
                    break;
                }
                case 4:
            {
                    /*                                                                 */
                    if ((btampContext->btamp_Remote_AMP_Assoc.HC_pref_country[0] == 'G') &&
                        (btampContext->btamp_Remote_AMP_Assoc.HC_pref_country[1] == 'B'))
                    {
                        regulatoryClass = 4;
                        firstChannel = 1;
                        numChannels = 13;
            }
                    break;
                }
                case 30:
            {
                    /*                                                                 */
                    if ((btampContext->btamp_Remote_AMP_Assoc.HC_pref_country[0] == 'J') &&
                        (btampContext->btamp_Remote_AMP_Assoc.HC_pref_country[1] == 'P'))
                    {
                        regulatoryClass = 30;
                        firstChannel = 1;
                        numChannels = 13;
            }
                    break;
        }
                default:
        {
                    break;
                }
            }
            /*                                                                                       
                                                                                                      
                                                                    */
            if (((triplet+1) < btampContext->btamp_Remote_AMP_Assoc.HC_pref_num_triplets) &&
                (btampContext->btamp_Remote_AMP_Assoc.HC_pref_triplets[triplet+1][0] != 201))
            {
                continue;
        }
    }
    else
    {
            /*                                                                  */
            if (regulatoryClass)
            {
                firstChannel = btampContext->btamp_Remote_AMP_Assoc.HC_pref_triplets[triplet][0];
                numChannels = btampContext->btamp_Remote_AMP_Assoc.HC_pref_triplets[triplet][1];
            }
        }

        if (firstChannel && numChannels)
        {
            if (!btampContext->btamp_AMP_Assoc.HC_pref_num_triplets)
            {
                pProfile->operationChannel = firstChannel;
                break;
            }
            else if (((btampContext->btamp_AMP_Assoc.HC_pref_triplets[1][0] + btampContext->btamp_AMP_Assoc.HC_pref_triplets[1][1]) <= firstChannel) ||
               ((firstChannel + numChannels ) <= btampContext->btamp_AMP_Assoc.HC_pref_triplets[1][0]))
            {
                continue;
            }
            else if ((btampContext->btamp_AMP_Assoc.HC_pref_triplets[1][0] + btampContext->btamp_AMP_Assoc.HC_pref_triplets[1][1]) > firstChannel)
            {
                pProfile->operationChannel = firstChannel;
                break;
            }
            else if ((firstChannel + numChannels) > btampContext->btamp_AMP_Assoc.HC_pref_triplets[1][0])
            {
                pProfile->operationChannel =  btampContext->btamp_AMP_Assoc.HC_pref_triplets[1][0];
                break;
            }
        }
    }

    if (!pProfile->operationChannel)
    {
        return VOS_STATUS_E_INVAL;
    }

    /*                         */
    sessionid = sme_GetInfraSessionId(hHal);
    /*                                                                      
                                                      */
    if(-1 != sessionid)
    {
        pProfile->operationChannel = 
            sme_GetInfraOperationChannel(hHal, 
                                         sessionid);
    }
     
    if(sme_IsChannelValid(hHal, pProfile->operationChannel))
    {         
        btampContext->channel = pProfile->operationChannel;
    }
    else
    {
        //                                                
        return VOS_STATUS_E_INVAL;
    }
     
    if ( BT_INITIATOR == btampContext->BAPDeviceRole ) 
    {
      pProfile->ChannelInfo.numOfChannels = 1;
      pProfile->ChannelInfo.ChannelList   = &pProfile->operationChannel;
    }
    else 
    {
        pProfile->ChannelInfo.numOfChannels = 1;
        pProfile->ChannelInfo.ChannelList   = &pProfile->operationChannel;
    }


    //                 
    pProfile->CBMode = eCSR_CB_OFF;

    //                                  
    //                                                                   
    pProfile->phyMode = eCSR_DOT11_MODE_11n; //                                                                         

    //                           
    sme_CfgSetInt(hHal, WNI_CFG_DOT11_MODE, WNI_CFG_DOT11_MODE_11N, NULL, eANI_BOOLEAN_FALSE);

    pProfile->bWPSAssociation = eANI_BOOLEAN_FALSE;

    //                                
    pProfile->uapsd_mask = 0;

    //                    
    return vosStatus;
} //                   

VOS_STATUS
gotoStarting
(
    ptBtampContext btampContext, /*                    */    
    ptWLAN_BAPEvent bapEvent, /*                     */
    eCsrRoamBssType bssType,
    v_U8_t *status    /*                               */
) 
{
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    eHalStatus  halStatus;
    v_U32_t     parseStatus;
    /*            */    
    tHalHandle hHal;
    tBtampTLVHCI_Write_Remote_AMP_ASSOC_Cmd *pBapHCIWriteRemoteAMPAssoc 
        = (tBtampTLVHCI_Write_Remote_AMP_ASSOC_Cmd *) bapEvent->params;
    tBtampAMP_ASSOC btamp_ASSOC; 

    /*                                                                       */
    if (NULL == btampContext) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "btampContext is NULL in %s", __func__);

        return VOS_STATUS_E_FAULT;
    }

    hHal = VOS_GET_HAL_CB(btampContext->pvosGCtx);
    if (NULL == hHal) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "hHal is NULL in %s", __func__);

        return VOS_STATUS_E_FAULT;
    }

    //                                                                         
    //                                                      

    if (eCSR_BSS_TYPE_WDS_STA == bssType)
    {
        /*                                                       */
        vos_sleep( 200 );
        /*                                                       */
        WLANBAP_NeedBTCoexPriority ( btampContext, 1);
    }


    //                      
    halStatus = pmcRequestFullPower(
            hHal, 
            WLANBAP_pmcFullPwrReqCB, 
            btampContext,
            eSME_REASON_OTHER);
            //                                                     
            //                                                    
            //                             
    //                                                              
    //                                                               
    //                                  

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, 
            "In %s, amp_assoc_remaining_length = %d", __func__, 
            pBapHCIWriteRemoteAMPAssoc->amp_assoc_remaining_length); 
#if 0
    DUMPLOG(1, __func__, "amp_assoc_fragment",  
            pBapHCIWriteRemoteAMPAssoc->amp_assoc_fragment, 
            64);
#endif // 

    //                                           
    parseStatus = btampUnpackAMP_ASSOC(
            hHal, 
            pBapHCIWriteRemoteAMPAssoc->amp_assoc_fragment, 
            pBapHCIWriteRemoteAMPAssoc->amp_assoc_remaining_length, 
            &btamp_ASSOC);

    /*                                                                      */
    if ((BTAMP_PARSE_SUCCESS != parseStatus ) && (BTAMP_UNKNOWN_TLVS != parseStatus))  
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, parseStatus = %d", __func__, parseStatus);
        *status = WLANBAP_ERROR_INVALID_HCI_CMND_PARAM;
        return VOS_STATUS_E_BADMSG;
    }

    //                                                                     
    //                               
    if (btamp_ASSOC.AMP_Assoc_MAC_Addr.present == 1)
    {
        /*                           */ 
        vos_mem_copy( 
                btampContext->btamp_Remote_AMP_Assoc.HC_mac_addr, 
                btamp_ASSOC.AMP_Assoc_MAC_Addr.mac_addr,   
                sizeof(btampContext->btamp_Remote_AMP_Assoc.HC_mac_addr)); 
        /*                                       */ 
        vos_mem_copy( 
                btampContext->peer_mac_addr, 
                btamp_ASSOC.AMP_Assoc_MAC_Addr.mac_addr,   
                sizeof(btampContext->peer_mac_addr)); 
     }

    if (btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.present == 1)
    {
        /*                                      */ 
        vos_mem_copy( 
                btampContext->btamp_Remote_AMP_Assoc.HC_pref_country, 
                btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.country,   
                sizeof(btampContext->btamp_Remote_AMP_Assoc.HC_pref_country)); 
        /*                                      */ 
        btampContext->btamp_Remote_AMP_Assoc.HC_pref_num_triplets = 
            btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.num_triplets;   
        if(WLANBAP_MAX_NUM_TRIPLETS < 
           btampContext->btamp_Remote_AMP_Assoc.HC_pref_num_triplets)
        {
            btampContext->btamp_Remote_AMP_Assoc.HC_pref_num_triplets = 
                WLANBAP_MAX_NUM_TRIPLETS;
        }
        vos_mem_copy( 
                btampContext->btamp_Remote_AMP_Assoc.HC_pref_triplets, 
                btamp_ASSOC.AMP_Assoc_Preferred_Channel_List.triplets,   
                sizeof(btampContext->btamp_Remote_AMP_Assoc.HC_pref_triplets[0]) *
                btampContext->btamp_Remote_AMP_Assoc.HC_pref_num_triplets   
                ); 
    }

    if (btamp_ASSOC.AMP_Assoc_Connected_Channel.present == 1)
    {
        /*                                 */ 
        vos_mem_copy( 
                btampContext->btamp_Remote_AMP_Assoc.HC_cnct_country, 
                btamp_ASSOC.AMP_Assoc_Connected_Channel.country,   
                sizeof(btampContext->btamp_Remote_AMP_Assoc.HC_cnct_country)); 
        /*                                 */ 
        btampContext->btamp_Remote_AMP_Assoc.HC_cnct_num_triplets = 
            btamp_ASSOC.AMP_Assoc_Connected_Channel.num_triplets;
        if(WLANBAP_MAX_NUM_TRIPLETS < 
           btampContext->btamp_Remote_AMP_Assoc.HC_cnct_num_triplets)
        {
            btampContext->btamp_Remote_AMP_Assoc.HC_cnct_num_triplets = 
                WLANBAP_MAX_NUM_TRIPLETS;
        }
        vos_mem_copy( 
                btampContext->btamp_Remote_AMP_Assoc.HC_cnct_triplets, 
                btamp_ASSOC.AMP_Assoc_Connected_Channel.triplets,   
                sizeof(btampContext->btamp_Remote_AMP_Assoc.HC_cnct_triplets[0]) *
                btampContext->btamp_Remote_AMP_Assoc.HC_cnct_num_triplets
                ); 
    }

    if (btamp_ASSOC.AMP_Assoc_PAL_Capabilities.present == 1)
    {
        /*                                */ 
        btampContext->btamp_Remote_AMP_Assoc.HC_pal_capabilities 
            = btamp_ASSOC.AMP_Assoc_PAL_Capabilities.pal_capabilities;
    }

    if (btamp_ASSOC.AMP_Assoc_PAL_Version.present == 1)
    {
        /*                           */ 
        btampContext->btamp_Remote_AMP_Assoc.HC_pal_version 
            = btamp_ASSOC.AMP_Assoc_PAL_Version.pal_version;

        btampContext->btamp_Remote_AMP_Assoc.HC_pal_CompanyID 
            = btamp_ASSOC.AMP_Assoc_PAL_Version.pal_CompanyID;

        btampContext->btamp_Remote_AMP_Assoc.HC_pal_subversion 
            = btamp_ASSOC.AMP_Assoc_PAL_Version.pal_subversion;
    }

    //                              
    /*                          */
    //                           
    //                                                                                     
    btampContext->gNeedPhysLinkCompEvent = VOS_TRUE;
    //                     
    btampContext->gDiscRequested = VOS_FALSE;
    //                                    
    btampContext->gPhysLinkStatus = WLANBAP_STATUS_SUCCESS;
    //                                 
    btampContext->gDiscReason = WLANBAP_STATUS_SUCCESS;
    /*                                           */
    //                                            
    /*               */

    if (btampContext->isBapSessionOpen == FALSE)
    {

        halStatus = sme_OpenSession(hHal, 
                                    WLANBAP_RoamCallback, 
                                    btampContext,
                                    //                      
                                    //                                          
                                    btampContext->self_mac_addr,  
                                    &btampContext->sessionId);
        if(eHAL_STATUS_SUCCESS == halStatus)
        {
            btampContext->isBapSessionOpen = TRUE;
        }
        else
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                         "sme_OpenSession failed in %s", __func__);
            *status = WLANBAP_ERROR_NO_CNCT;
            return VOS_STATUS_E_FAILURE;
        }
    }
    /*                                                                                             */
    //                                                               
    //                     
    //                                                
    //      
    vosStatus = convertToCsrProfile ( 
            btampContext, /*                    */    
            bssType,
            &btampContext->csrRoamProfile);   /*                              */
    if(VOS_STATUS_E_INVAL == vosStatus)
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "Incorrect channel to create AMP link %s", __func__);
        *status = WLANBAP_ERROR_NO_SUITABLE_CHANNEL;
        return VOS_STATUS_E_INVAL;
    }
#if 0
    halStatus = sme_RoamConnect(VOS_GET_HAL_CB(btampContext->pvosGCtx), 
            &btampContext->csrRoamProfile, 
            NULL,   /*                               */ 
            &btampContext->csrRoamId);
#endif // 
//     
    halStatus = sme_RoamConnect(hHal, 
            btampContext->sessionId, 
            &btampContext->csrRoamProfile, 
            &btampContext->csrRoamId);
//          

    //                                  
    return vosStatus;
} //            
          
VOS_STATUS
gotoConnecting(
    ptBtampContext btampContext /*                    */
)
{
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;

    /*                                                               */
    /*                                                       */
    WLANBAP_NeedBTCoexPriority ( btampContext, 1);

    return vosStatus;
} //              
 
VOS_STATUS
gotoAuthenticating(
    ptBtampContext btampContext /*                    */
)
{
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;

    /*                                                       */
    WLANBAP_NeedBTCoexPriority ( btampContext, 1);

    return vosStatus;
} //                  

#if 0
VOID initRsnSupplicant()
{
/*                                                  */
}
#endif /*   */
VOS_STATUS
initRsnSupplicant
(
    ptBtampContext btampContext, /*                    */    
    tWLAN_BAPRole BAPDeviceRole
)
{
    VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;

    /*                                                  */
    /*              */
    if (!(suppRsnFsmCreate(btampContext)))
    {
        /*                  */
        /*                    */
    }
    else
    {
        /*                 */
        vosStatus = VOS_STATUS_E_FAILURE;
    }
    /*                                                  */
    return vosStatus;
}

#if 0
VOID initRsnAuthenticator()
{
/*                                                                 */
/*                                                                                                                                    */
}
#endif /*   */
VOS_STATUS
initRsnAuthenticator
(
    ptBtampContext btampContext, /*                    */    
    tWLAN_BAPRole BAPDeviceRole
)
{
    VOS_STATUS vosStatus = VOS_STATUS_SUCCESS;
    /*              */
    if (!(authRsnFsmCreate(btampContext)))
    {
        /*                  */
    }
    else
    {
        /*                 */
        vosStatus = VOS_STATUS_E_FAILURE;
    }
    return vosStatus;
/*                                                                 */
/*                                                                                                                                    */
}

/*                                     */
VOS_STATUS
regStaWithTl
(
    ptBtampContext btampContext, /*                    */    
    tWLAN_BAPRole BAPDeviceRole,
    tCsrRoamInfo *pCsrRoamInfo
)
{
    VOS_STATUS vosStatus;
    WLAN_STADescType staDesc;
    tANI_S8          rssi = 0;

    vos_mem_zero(&staDesc, sizeof(WLAN_STADescType));
    /*                                         */
    btampContext->ucSTAId = staDesc.ucSTAId = pCsrRoamInfo->staId;

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BAP register TL ucSTAId=%d\n", 
               staDesc.ucSTAId );

    /*                              */
    vos_mem_copy( 
            staDesc.vSTAMACAddress.bytes, 
            btampContext->peer_mac_addr, 
            sizeof(btampContext->peer_mac_addr)); 

    /*                              */
    vos_mem_copy( 
            staDesc.vSelfMACAddress.bytes, 
            btampContext->self_mac_addr, 
            sizeof(btampContext->peer_mac_addr)); 

    /*                  */
    staDesc.wSTAType = WLAN_STA_BT_AMP;

    //                                                       
    if( pCsrRoamInfo->u.pConnectedProfile)
    { 
        btampContext->bapQosCfg.bWmmIsEnabled = //   
            pCsrRoamInfo->u.pConnectedProfile->qosConnection;
    }
    else
    {
        btampContext->bapQosCfg.bWmmIsEnabled = 0; 
    }

    //                                
    if( btampContext->bapQosCfg.bWmmIsEnabled )
    {
       staDesc.ucQosEnabled = 1;
    }
    else
    {
       staDesc.ucQosEnabled = 0;
    }

    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "BAP register TL QoS_enabled=%d\n", 
               staDesc.ucQosEnabled );

    //                                           
    //                           
    //                       
    /*                                                                               */
    staDesc.ucSwFrameTXXlation = 1;
    staDesc.ucSwFrameRXXlation = 1; 
    staDesc.ucAddRmvLLC = 0;

    if ( btampContext->ucSecEnabled )
    {
       staDesc.ucProtectedFrame = 1;
    }
    else
    {
       staDesc.ucProtectedFrame = 0;
    }

    staDesc.ucUcastSig = pCsrRoamInfo->ucastSig; 
    staDesc.ucBcastSig = pCsrRoamInfo->bcastSig;
    staDesc.ucInitState = ( btampContext->ucSecEnabled)?
        WLANTL_STA_CONNECTED:WLANTL_STA_AUTHENTICATED;
    staDesc.ucIsReplayCheckValid = VOS_FALSE;
    if(NULL != pCsrRoamInfo->pBssDesc)
    {
        rssi = pCsrRoamInfo->pBssDesc->rssi;
    }
    /*                          */
    vosStatus = WLANTL_RegisterSTAClient 
        ( 
         btampContext->pvosGCtx,
         WLANBAP_STARxCB,  
         WLANBAP_TxCompCB,  
         (WLANTL_STAFetchPktCBType)WLANBAP_STAFetchPktCB,
         &staDesc ,
         rssi);   
    if ( !VOS_IS_STATUS_SUCCESS( vosStatus ) )
    {
       VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
                  "%s: WLANTL_RegisterSTAClient() failed to register.  Status= %d [0x%08lX]",
                  __func__, vosStatus, vosStatus );
    }                                            
     
    if ( !  btampContext->ucSecEnabled )
    {
       VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_MED,
                  "open/shared auth StaId= %d.  Changing TL state to AUTHENTICATED at Join time", btampContext->ucSTAId);
    
       //                                                                      
       //                                
       vosStatus = WLANTL_ChangeSTAState( btampContext->pvosGCtx, staDesc.ucSTAId, 
                                            WLANTL_STA_AUTHENTICATED );
    }                                            
    else
    {

       VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_MED,
                  "ULA auth StaId= %d.  Changing TL state to CONNECTED at Join time", btampContext->ucSTAId );
      
       vosStatus = WLANTL_ChangeSTAState( btampContext->pvosGCtx, staDesc.ucSTAId, 
                                          WLANTL_STA_CONNECTED );
    }                                            

    return VOS_STATUS_SUCCESS;
} /*              */

#if 0
/*                                                                          

                          

                                                               

                                                                            */
#endif

VOS_STATUS
determineChan
(
    ptBtampContext btampContext, /*                    */    
    tWLAN_BAPRole BAPDeviceRole,
    v_U32_t *channel,  /*                 */
    v_U8_t *status    /*                               */
)
{
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    v_U32_t     activeFlag;  /*                     */
    /*                                                                       */

    switch(BAPDeviceRole)
    {
    case BT_INITIATOR:
      /*                                                        */
      /*                                                                             */
      /*                        */
      vosStatus = WLANBAP_GetCurrentChannel (btampContext, channel, &activeFlag);
    break;
    case BT_RESPONDER:
      /*                                                                                                                        */
      /*                             */
      /*                        */
      vosStatus = WLANBAP_GetCurrentChannel (btampContext, channel, &activeFlag);
    break;
    default:
      *status = WLANBAP_ERROR_HOST_REJ_RESOURCES;     /*                               */
      return VOS_STATUS_E_RESOURCES;
  }
  *status = WLANBAP_STATUS_SUCCESS;     /*                               */

  return vosStatus;
} //              

VOS_STATUS
gotoDisconnected 
(
    ptBtampContext btampContext /*                    */    
)
{
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    /*                                                                       */

    //                                            
    //                                          
    //                                                                      
    //                                                       
    //                                   

    /*                                                                 */
    WLANBAP_NeedBTCoexPriority ( btampContext, 0);

    //                                  
    return vosStatus;
} //                  

VOS_STATUS
gotoDisconnecting 
(
    ptBtampContext   btampContext, /*                    */    
    v_U8_t   needPhysLinkCompEvent,
    v_U8_t   physLinkStatus,   /*                             */
//                                                                        
    v_U8_t   discRequested,
    v_U8_t   discReason        /*                             */
)
{

    //                       
    btampContext->gNeedPhysLinkCompEvent = needPhysLinkCompEvent;
    //                 
    btampContext->gPhysLinkStatus = physLinkStatus;   /*                             */
    //               
    btampContext->gDiscRequested = discRequested;
    //             
    btampContext->gDiscReason = discReason;       /*                             */

    //                                            
    //                                               

    /*                                                                   */
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_LOW,
               "Calling send_btc_nlink_msg() with AMP channel = 0");
    send_btc_nlink_msg(WLAN_AMP_ASSOC_DONE_IND, 0);

    return VOS_STATUS_SUCCESS;
} //                  

VOS_STATUS
gotoConnected
(
    ptBtampContext   btampContext /*                    */    
)
{
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    ptBtampHandle     btampHandle = ( ptBtampHandle)btampContext;
//     
    /*                                  */
    vosStatus = WLANBAP_StopConnectionAcceptTimer (btampContext);
//      
    //                            
    //                                                              

    /*                                                                   */
    WLANBAP_NeedBTCoexPriority ( btampContext, 2);

    //                                                              
    //                                
    if ( btampContext->ucSecEnabled )
    { 
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_MED, 
                "open/shared auth StaId= %d.  Changing TL state to AUTHENTICATED at Join time", btampContext->ucSTAId);
    
        vosStatus = WLANTL_ChangeSTAState( 
                btampContext->pvosGCtx, 
                btampContext->ucSTAId, 
                WLANTL_STA_AUTHENTICATED );
    }

    btampContext->dataPktPending = VOS_FALSE;
    vosStatus = WLANBAP_InitLinkSupervision( btampHandle);
 
    /*                                                                 */
    VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_LOW,
               "Calling send_btc_nlink_msg() with AMP channel %d", btampContext->channel);
    send_btc_nlink_msg(WLAN_AMP_ASSOC_DONE_IND, btampContext->channel);

    return vosStatus;
} //              


/*                                 */
VOS_STATUS
signalHCIPhysLinkCompEvent
( 
  ptBtampContext btampContext, /*                    */    
  v_U8_t status    /*                   */
)
{
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    tBtampHCI_Event bapHCIEvent; /*                                  */
    /*                                                                       */

    /*                                                      */ 
    bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_PHYSICAL_LINK_COMPLETE_EVENT;
    bapHCIEvent.u.btampPhysicalLinkCompleteEvent.present = 1;
    bapHCIEvent.u.btampPhysicalLinkCompleteEvent.status = status;
    bapHCIEvent.u.btampPhysicalLinkCompleteEvent.phy_link_handle 
        = btampContext->phy_link_handle;
    bapHCIEvent.u.btampPhysicalLinkCompleteEvent.ch_number 
        = btampContext->channel;

    if(WLANBAP_STATUS_SUCCESS == status)
    {
        /*                                      */
        WLANBAP_StartTxPacketMonitorTimer(btampContext);
    }
    else
    {   //                   
        btampContext->phy_link_handle = 0;
    }

    vosStatus = (*btampContext->pBapHCIEventCB) 
        (  
         btampContext->pHddHdl,   /*                                             */
         &bapHCIEvent, /*                                  */
         VOS_TRUE /*                                       */ 
        );
  
  return vosStatus;
} /*                            */

/*                                                     */
VOS_STATUS
signalHCIPhysLinkDiscEvent
( 
  ptBtampContext btampContext, /*                    */    
  v_U8_t status,    /*                   */
  v_U8_t reason    /*                        */
)
{
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    tBtampHCI_Event bapHCIEvent; /*                                  */
    v_U8_t          i;
    tpBtampLogLinkCtx  pLogLinkContext = NULL;
    /*                                                                       */

#ifdef BAP_DEBUG
  /*                                      */
  VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH,
            "WLAN BAP Context Monitor: btampContext value = %p in %s:%d", btampContext, __func__, __LINE__ );
#endif //         

    /*                                                            */
    for (i = 0 ; i < WLANBAP_MAX_LOG_LINKS; i++)
    {
        pLogLinkContext = &(btampContext->btampLogLinkCtx[i]);

        if (pLogLinkContext->present == VOS_TRUE) 
        { 
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, 
                    "WLAN BAP: Deleting logical link entry %d in %s", i,
                    __func__); 

            /*                                            */
            pLogLinkContext->present = VOS_FALSE; 

            //                                                                                 
            signalHCIDiscLogLinkCompEvent 
                ( btampContext, 
                  WLANBAP_STATUS_SUCCESS, 
                  i,   //              
                  //                                                          
                  WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
        }
    }

    /*                                                            */
    btampContext->current_log_link_index = 0; 
    btampContext->total_log_link_index = 0; 

    /*                                                                 */ 
    bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_DISCONNECT_PHYSICAL_LINK_COMPLETE_EVENT;
    bapHCIEvent.u.btampDisconnectPhysicalLinkCompleteEvent.present = 1;
    bapHCIEvent.u.btampDisconnectPhysicalLinkCompleteEvent.status = status;
    bapHCIEvent.u.btampDisconnectPhysicalLinkCompleteEvent.reason = reason;//                    
    bapHCIEvent.u.btampDisconnectPhysicalLinkCompleteEvent.phy_link_handle 
        = btampContext->phy_link_handle;

    /*                                     */
    WLANBAP_StopTxPacketMonitorTimer(btampContext);

    /*                                                                    
           
                                                    */
    btampContext->phy_link_handle = 0; 
    vosStatus = (*btampContext->pBapHCIEventCB) 
        (  
         btampContext->pHddHdl,   /*                                             */
         &bapHCIEvent, /*                                  */
         VOS_TRUE /*                                       */ 
        );
  
  return vosStatus;
} /*                            */

/*                                                */
VOS_STATUS
signalHCIChanSelEvent
( 
  ptBtampContext btampContext /*                    */    
)
{
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    tBtampHCI_Event bapHCIEvent; /*                                  */
    /*                                                                       */

    /*                                                                 */ 
    bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_CHANNEL_SELECTED_EVENT;
    bapHCIEvent.u.btampChannelSelectedEvent.present = 1;
    bapHCIEvent.u.btampChannelSelectedEvent.phy_link_handle 
        = btampContext->phy_link_handle;
 
    vosStatus = (*btampContext->pBapHCIEventCB) 
        (  
         btampContext->pHddHdl,   /*                                             */
         &bapHCIEvent, /*                                  */
         VOS_TRUE /*                                       */ 
        );

  return vosStatus;
} /*                       */


/*                                                                  */
VOS_STATUS
signalHCIDiscLogLinkCompEvent
( 
  ptBtampContext btampContext, /*                    */    
  v_U8_t status,    /*                   */
  v_U16_t log_link_handle,  /*                                   */
  v_U8_t reason    /*                        */
)
{
    VOS_STATUS  vosStatus = VOS_STATUS_SUCCESS;
    tBtampHCI_Event bapHCIEvent; /*                                  */
    /*                                                                       */

    /*                                                                */ 
    bapHCIEvent.bapHCIEventCode = BTAMP_TLV_HCI_DISCONNECT_LOGICAL_LINK_COMPLETE_EVENT;
    bapHCIEvent.u.btampDisconnectLogicalLinkCompleteEvent.present = 1;
    bapHCIEvent.u.btampDisconnectLogicalLinkCompleteEvent.status = status;
    bapHCIEvent.u.btampDisconnectLogicalLinkCompleteEvent.reason = reason;
    bapHCIEvent.u.btampDisconnectLogicalLinkCompleteEvent.log_link_handle 
    = (log_link_handle  << 8) + btampContext->phy_link_handle;

    vosStatus = (*btampContext->pBapHCIEventCB) 
        (  
         btampContext->pHddHdl,   /*                                             */
         &bapHCIEvent, /*                                  */
         VOS_TRUE /*                                       */ 
        );
  
  return vosStatus;
} /*                               */

 
//                                              
#define WLANBAP_RSN_OUI_SIZE 4
tANI_U8 pRSNOui00[ WLANBAP_RSN_OUI_SIZE ] = { 0x00, 0x0F, 0xAC, 0x00 }; //             
tANI_U8 pRSNOui01[ WLANBAP_RSN_OUI_SIZE ] = { 0x00, 0x0F, 0xAC, 0x01 }; //              
tANI_U8 pRSNOui02[ WLANBAP_RSN_OUI_SIZE ] = { 0x00, 0x0F, 0xAC, 0x02 }; //                
tANI_U8 pRSNOui03[ WLANBAP_RSN_OUI_SIZE ] = { 0x00, 0x0F, 0xAC, 0x03 }; //         
tANI_U8 pRSNOui04[ WLANBAP_RSN_OUI_SIZE ] = { 0x00, 0x0F, 0xAC, 0x04 }; //         
tANI_U8 pRSNOui05[ WLANBAP_RSN_OUI_SIZE ] = { 0x00, 0x0F, 0xAC, 0x05 }; //        

#define GET_IE_LEN_IN_BSS(lenInBss) ( lenInBss + sizeof(lenInBss) - ((int) OFFSET_OF( tSirBssDescription, ieFields))) 
/*                                                      */
v_U32_t 
validAssocInd
( 
    ptBtampContext btampContext, /*                    */    
    tCsrRoamInfo *pRoamInfo
)
{
    /*            */    
    tHalHandle hHal = VOS_GET_HAL_CB(btampContext->pvosGCtx);
    v_U32_t ieLen; 
  
    /*                             */ 
    return VOS_TRUE;

    /*                                    */
    /*                                                       
                                                     
                          
     */
    if ( !vos_mem_compare( btampContext->peer_mac_addr, 
                pRoamInfo->peerMac, 
                           sizeof(btampContext->peer_mac_addr) ))
    {
        /*                  */ 
        return VOS_FALSE;
    }

    /*                                       */ 
    /*                        */ 
    return VOS_TRUE;

    /*                                       */
    if( pRoamInfo->prsnIE == NULL )
    {
        //                              
        //                         
         /*                  */ 
        return VOS_FALSE;
    }

   //                                                            
   //                                              
   ieLen = pRoamInfo->rsnIELen;
 
    /*                                           */
    //                              
    if( ieLen == 0 )
    {
        //                              
        //                         
         /*                  */ 
        return VOS_FALSE;
    }

    {
        //                        
    tDot11fBeaconIEs dot11BeaconIEs; 
    tDot11fIESSID *pDot11SSID;
    tDot11fIERSN  *pDot11RSN;
 
    //                                                                        
    //                                                 
 
    //                                                                
    //                                                                          
    //                                  
    dot11fUnpackBeaconIEs((tpAniSirGlobal) hHal, 
            (tANI_U8 *) pRoamInfo->prsnIE,
            ieLen, 
            &dot11BeaconIEs);

    //                                                             

    pDot11SSID = &dot11BeaconIEs.SSID; 

    //                                                 
    btampContext->assocSsidLen = 0;

        if (pDot11SSID->present )
        {

        //                                                             

        btampContext->assocSsidLen = pDot11SSID->num_ssid;  
        vos_mem_copy(btampContext->assocSsid, 
                pDot11SSID->ssid, 
                btampContext->assocSsidLen );
        }
        else
        return VOS_FALSE;

    //                                                      
    if ( !vos_mem_compare( btampContext->ownSsid, 
                pDot11SSID->ssid, 
                               btampContext->ownSsidLen ))
        {
        /*                  */ 
        return VOS_FALSE;
    }

    pDot11RSN = &dot11BeaconIEs.RSN; 

    //                                                   
    //                                

        if (pDot11RSN->present )
        {

        //                                                           

        //                                              
        if (!vos_mem_compare(pRSNOui02, //         
                pDot11RSN->akm_suites[0], 
                WLANBAP_RSN_OUI_SIZE)) 
            return VOS_FALSE;

        //                                                      
        if (!vos_mem_compare(pRSNOui04, //         
                pDot11RSN->pwise_cipher_suites[0], 
                WLANBAP_RSN_OUI_SIZE)) 
            return VOS_FALSE;
        }
        else
        return VOS_FALSE;


    } //                     

    /*                        */ 
    return VOS_TRUE;
} /*               */

/*                          */
void 
btampfsmChangeToState
(
    BTAMPFSM_INSTANCEDATA_T *instance, 
    BTAMPFSM_STATES_T state
)
{
    instance->stateVar = state;
    //                                        
  
}

/*                                      */
//    
VOS_STATUS
btampFsm
(
    //                                    
    ptBtampContext btampContext, /*                    */    
//                                                            
    ptWLAN_BAPEvent bapEvent, /*                     */
    v_U8_t *status    /*                               */
)
{
    /*                                               
                                   
     */    
    BTAMPFSM_INSTANCEDATA_T *instanceVar;
    v_U32_t msg = bapEvent->event;  /*                                   */
    v_U32_t channel;  /*                 */
    v_U32_t activeFlag;  /*                     */
    VOS_STATUS      vosStatus = VOS_STATUS_SUCCESS;
    ptBtampHandle btampHandle = ( ptBtampHandle)btampContext;
    v_U8_t                   ucSTAId;  /*                                     */
    v_PVOID_t                pHddHdl; /*                                 */
    tHalHandle     hHal = NULL;
    /*                                                                       */
    /*                 */ 
    if (btampHandle == NULL) 
    {
      return VOS_STATUS_E_FAULT;
    }
    instanceVar = &(btampContext->bapPhysLinkMachine);

    hHal = VOS_GET_HAL_CB(btampContext->pvosGCtx);
    if (NULL == hHal) 
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,
                     "hHal is NULL in %s", __func__);

        return VOS_STATUS_E_FAULT;
    }


 
#define CHANNEL_NOT_SELECTED (WLANBAP_GetCurrentChannel (btampContext, &channel, &activeFlag) != VOS_STATUS_SUCCESS)

  /*                                                                       */
  *status = WLANBAP_STATUS_SUCCESS;

    switch(instanceVar->stateVar)
    {

      case DISCONNECTED:
        if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_PHYSICAL_LINK_CREATE))
        {
          /*                                                           */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "DISCONNECTED", "S1");
 
#if 0
         /*                                                                   */
         /*                                     */ 
          if(btampContext->isBapSessionOpen == TRUE)//                                    
          {
          sme_CloseSession(VOS_GET_HAL_CB(btampContext->pvosGCtx),
                                         btampContext->sessionId);
          /*               */
          btampContext->isBapSessionOpen = FALSE; 
          }   
#endif

          /*                     */
          vosStatus = gotoS1( btampContext, bapEvent, BT_INITIATOR, status); 
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, cmd status is %d", __func__, *status);
           /*                       */
          btampfsmChangeToState(instanceVar,S1);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_PHYSICAL_LINK_ACCEPT))
        {
          /*                                                           */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "DISCONNECTED", "S1");
          
#if 0
          if(btampContext->isBapSessionOpen == TRUE)
          {
          sme_CloseSession(VOS_GET_HAL_CB(btampContext->pvosGCtx),
                                         btampContext->sessionId);
          /*               */
          btampContext->isBapSessionOpen = FALSE; 
          }
          /*                           */
#endif

          /*                     */
          vosStatus = gotoS1(btampContext, bapEvent, BT_RESPONDER, status);
          /*                       */
          btampfsmChangeToState(instanceVar,S1);
        }
        else
        {
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, in state %s, invalid event msg %d", __func__, "DISCONNECTED", msg);
          /*                          */
        }
      break;

      case S1:
        if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_WRITE_REMOTE_AMP_ASSOC
           ) && (btampContext->BAPDeviceRole == BT_INITIATOR && !(CHANNEL_NOT_SELECTED)))
        {
          /*                                                       */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "S1", "STARTING");

          /*                           */
          vosStatus = determineChan(btampContext, BT_INITIATOR, &channel, status);
          /*                       */
          btampfsmChangeToState(instanceVar,STARTING);
          //                                                                     
          vosStatus = gotoStarting( btampContext, bapEvent, eCSR_BSS_TYPE_WDS_AP, status); 
          if (VOS_STATUS_SUCCESS != vosStatus)
          {
              btampfsmChangeToState(instanceVar, S1);
          }
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_TIMER_CONNECT_ACCEPT_TIMEOUT))
        {
          /*                                                           */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "S1", "DISCONNECTED");

          /*                           */
          /*                                      */    
          gotoDisconnected( btampContext); 
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTED);
          /*                      */
          signalHCIPhysLinkCompEvent( btampContext, WLANBAP_ERROR_HOST_TIMEOUT);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_PHYSICAL_LINK_DISCONNECT))
        {
          /*                                                           */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "S1", "DISCONNECTED");

          /*                           */
          gotoDisconnected(btampContext);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTED);
          /*                                               */
          signalHCIPhysLinkDiscEvent 
              ( btampContext, 
                WLANBAP_STATUS_SUCCESS,
                WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
          /*                                               */
          signalHCIPhysLinkCompEvent( btampContext, WLANBAP_ERROR_NO_CNCT );
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_WRITE_REMOTE_AMP_ASSOC
                ) && (btampContext->BAPDeviceRole == BT_RESPONDER))
        {
          /*                                                       */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "S1", "STARTING");

          /*                           */
          //                            
          vosStatus = determineChan(btampContext, BT_RESPONDER, &channel, status);
          btampfsmChangeToState(instanceVar,STARTING);//                      
          //                                                                     
          /*                       */
         //                                             
          vosStatus = gotoStarting( btampContext, bapEvent, eCSR_BSS_TYPE_WDS_STA, status); 
          if (VOS_STATUS_SUCCESS != vosStatus)
          {
              btampfsmChangeToState(instanceVar, S1);
          }
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_WRITE_REMOTE_AMP_ASSOC
                ) && (btampContext->BAPDeviceRole == BT_INITIATOR && CHANNEL_NOT_SELECTED))
        {
          /*                                                       */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "S1", "SCANNING");

          /*                           */
          gotoScanning(btampContext, BT_RESPONDER, status);
          /*                       */
          btampfsmChangeToState(instanceVar,SCANNING);
        }
        else
        {
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, in state %s, invalid event msg %d", __func__, "S1", msg);
          /*                          */
        }
      break;

      case STARTING:
        if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_START_BSS_SUCCESS
           ) && (btampContext->BAPDeviceRole == BT_INITIATOR))
        {
          /*                                                               */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "STARTING", "CONNECTING");

          btampfsmChangeToState(instanceVar,CONNECTING);//              

          /*                         */
  /*                             */
          btampContext->channel = ( 0 == btampContext->channel )?1:btampContext->channel;

          /*                           */
          signalHCIChanSelEvent(btampContext);
        
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_PHYSICAL_LINK_DISCONNECT))
        {
          /*                                                                  */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "STARTING", "DISCONNECTING");

          /*                           */
          //                    
          sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  //                                           
                  //                                                          
                  eCSR_DISCONNECT_REASON_UNSPECIFIED);

          gotoDisconnecting(
                  btampContext,
                  VOS_TRUE, 
                  WLANBAP_ERROR_NO_CNCT, 
                  //                                       
                  VOS_FALSE, 
                  WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);
          //                                                             
          //                     
          signalHCIPhysLinkDiscEvent 
              ( btampContext, 
                WLANBAP_STATUS_SUCCESS,
                WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_CHANNEL_SELECTION_FAILED))
        {
          /*                                                                 */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "STARTING", "DISCONNECTED");

          gotoDisconnected(btampContext);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTED);
          /*                           */
          signalHCIPhysLinkCompEvent( btampContext, WLANBAP_ERROR_HOST_REJ_RESOURCES );
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_START_BSS_SUCCESS
                ) && (btampContext->BAPDeviceRole == BT_RESPONDER))
        {
          /*                                                               */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "STARTING", "CONNECTING");

          /*                          */
          /*                             */
          btampContext->channel = ( 0 == btampContext->channel )?1:btampContext->channel;

          /*                       */
          btampfsmChangeToState(instanceVar,CONNECTING);
          /*                           */
            gotoConnecting(btampContext);
          
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_TIMER_CONNECT_ACCEPT_TIMEOUT))
        {
          /*                                                                  */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "STARTING", "DISCONNECTING");

          /*                           */
          //                    
          sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  eCSR_DISCONNECT_REASON_UNSPECIFIED);
          gotoDisconnecting(
                  btampContext, 
                  VOS_TRUE,
                  WLANBAP_ERROR_HOST_TIMEOUT, 
                  VOS_FALSE,
                  0);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_START_FAILS))
        {
          /*                                                                 */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "STARTING", "DISCONNECTED");

          /*                           */
          gotoDisconnected(btampContext);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTED);
          signalHCIPhysLinkCompEvent( btampContext, WLANBAP_ERROR_MAX_NUM_CNCTS );
        }
        else
        {
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, in state %s, invalid event msg %d", __func__, "STARTING", msg);
          /*                          */
        }
      break;

      case CONNECTING:
        if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_CONNECT_COMPLETED
           ) && (btampContext->BAPDeviceRole == BT_RESPONDER))
        {
          /*                                                                     */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "CONNECTING", "AUTHENTICATING");
          //                                                                                                                            

            gotoAuthenticating(btampContext);
          /*                           */
          initRsnSupplicant(btampContext, BT_RESPONDER);
#if 1
          /*                       */
          btampfsmChangeToState(instanceVar,AUTHENTICATING);
#else
          /*                           */
          signalHCIPhysLinkCompEvent(btampContext, WLANBAP_STATUS_SUCCESS);
          gotoConnected(btampContext);
          /*                       */
          btampfsmChangeToState(instanceVar,CONNECTED);
#endif
          /*                          */
          regStaWithTl ( 
                  btampContext, /*                    */ 
                  BT_RESPONDER, 
                  (tCsrRoamInfo *)bapEvent->params);

        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_PHYSICAL_LINK_DISCONNECT))
        {
          /*                                                                    */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "CONNECTING", "DISCONNECTING");

          /*                           */
          //                    
          sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  eCSR_DISCONNECT_REASON_UNSPECIFIED);
          gotoDisconnecting(
                  btampContext,
                  VOS_TRUE, 
                  WLANBAP_ERROR_NO_CNCT, 
                  //                                       
                  VOS_FALSE, 
                  WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);
          //                                                             
          //                     
          signalHCIPhysLinkDiscEvent 
              ( btampContext, 
                WLANBAP_STATUS_SUCCESS,
                WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_CONNECT_INDICATION
        //                                                               
                ) && !validAssocInd(btampContext, (tCsrRoamInfo *)bapEvent->params))
        {
          /*                                                                    */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "CONNECTING", "DISCONNECTING");
          /*                           */
          //                          
          //                                                          
          sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  eCSR_DISCONNECT_REASON_DEAUTH);
                  //                                    
          gotoDisconnecting(
                  btampContext,
                  VOS_TRUE, 
                  WLANBAP_ERROR_AUTHENT_FAILURE, 
                  VOS_FALSE, 
                  0);

          /*                                                          */
          *status = WLANBAP_ERROR_AUTHENT_FAILURE;

          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_CONNECT_INDICATION
        //                                                        
                ) && validAssocInd(btampContext, (tCsrRoamInfo *)bapEvent->params))
        {
          /*                                                                */
          //                                                                                                                            
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "CONNECTING", "AUTHENTICATING");
          //                                                                                                                            

          /*                           */
          //                                                               
          //                                                   
          //                

          //                    
          /*                                                          */
          *status = WLANBAP_STATUS_SUCCESS;

          /*                                             
                                                      
                                                        
                         
           */

          /*                          */
          regStaWithTl ( 
                  btampContext, /*                    */ 
                  BT_INITIATOR, 
                  (tCsrRoamInfo *)bapEvent->params );
          
            gotoAuthenticating(btampContext);
          /*                           */
          initRsnAuthenticator(btampContext, BT_INITIATOR);

#if 1
          /*                       */
          //                                             
          btampfsmChangeToState(instanceVar,AUTHENTICATING);
#else
          /*                           */
          signalHCIPhysLinkCompEvent(btampContext, WLANBAP_STATUS_SUCCESS);
          gotoConnected(btampContext);
          /*                       */
          btampfsmChangeToState(instanceVar,CONNECTED);
#endif
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_CONNECT_FAILED))
        {
          /*                                                                    */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "CONNECTING", "DISCONNECTING");

          /*                           */
            sme_RoamDisconnect(hHal,
                               btampContext->sessionId,
                               eCSR_DISCONNECT_REASON_UNSPECIFIED);
          /*                                                                        
                                                                                            
           */
          //                                     
          gotoDisconnecting(
                  btampContext,
                  VOS_TRUE, 
                  WLANBAP_ERROR_TERM_BY_LOCAL_HOST, //                 
                  VOS_FALSE, 
                  0);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_TIMER_CONNECT_ACCEPT_TIMEOUT))
        {
          /*                                                                    */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "CONNECTING", "DISCONNECTING");

          /*                           */
          //                    
          sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  eCSR_DISCONNECT_REASON_UNSPECIFIED);
          gotoDisconnecting(
                  btampContext,
                  VOS_TRUE, 
                  WLANBAP_ERROR_HOST_TIMEOUT,
                  VOS_FALSE, 
                  0);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);
        }
        else
        {
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, in state %s, invalid event msg %d", __func__, "CONNECTING", msg);
          /*                          */
        }
      break;

      case AUTHENTICATING:
        if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_RSN_SUCCESS
           ) && (btampContext->BAPDeviceRole == BT_RESPONDER))
        {
          /*                                                                 */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "AUTHENTICATING", "KEYING");

          /*                           */
          //                      
#if 0
          sme_RoamSetKey(
                  VOS_GET_HAL_CB(btampContext->pvosGCtx), 
                  btampContext->sessionId, 
                  tSirMacAddr peerBssId, 
                  eCsrEncryptionType encryptType,
                  tANI_U16 keyLength, 
                  tANI_U8 *pKey,
                  VOS_TRUE,   //      
                  tANI_U8 paeRole);
#endif // 
          /*                       */
          btampfsmChangeToState(instanceVar,KEYING);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_RSN_SUCCESS
                ) && (btampContext->BAPDeviceRole == BT_INITIATOR))
        {
          /*                                                                 */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "AUTHENTICATING", "KEYING");

          /*                           */
          //                      
#if 0
          sme_RoamSetKey(
                  VOS_GET_HAL_CB(btampContext->pvosGCtx), 
                  btampContext->sessionId, 
                  tSirMacAddr peerBssId, 
                  eCsrEncryptionType encryptType,
                  tANI_U16 keyLength, 
                  tANI_U8 *pKey,
                  VOS_TRUE,   //      
                  tANI_U8 paeRole);
#endif // 
          /*                       */
          btampfsmChangeToState(instanceVar,KEYING);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_TIMER_CONNECT_ACCEPT_TIMEOUT))
        {
          /*                                                                        */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s ConnectAcceptTimeout", __func__, "AUTHENTICATING", "DISCONNECTING");

          gotoDisconnecting(
                  btampContext,
                  VOS_TRUE, 
                  WLANBAP_ERROR_HOST_TIMEOUT,
                  VOS_FALSE, 
                  0);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);
          /*                           */
         sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  eCSR_DISCONNECT_REASON_UNSPECIFIED);
         
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_PHYSICAL_LINK_DISCONNECT))
        {
          /*                                                                        */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s Physicallink Disconnect", __func__, "AUTHENTICATING", "DISCONNECTING");

          /*                           */
          //                    
          sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  eCSR_DISCONNECT_REASON_UNSPECIFIED);
          gotoDisconnecting(
                  btampContext,
                  VOS_TRUE, 
                  WLANBAP_ERROR_NO_CNCT,
                  //                                       
                  VOS_FALSE, 
                  WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);
          //                                                             
          //                     
          signalHCIPhysLinkDiscEvent 
              ( btampContext, 
                WLANBAP_STATUS_SUCCESS,
                WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_RSN_FAILURE))
        {
          /*                                                                        */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s RSN Failure", __func__, "AUTHENTICATING", "DISCONNECTING");

          /*                           */
          //                          
          sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  eCSR_DISCONNECT_REASON_DEAUTH);
                  //                                    
          gotoDisconnecting(
                  btampContext,
                  VOS_TRUE, 
                  WLANBAP_ERROR_AUTHENT_FAILURE, 
                  VOS_FALSE, 
                  0);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);
        }
        else
        {
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, in state %s, invalid event msg %d", __func__, "AUTHENTICATING", msg);
          /*                          */
        }
      break;

      case CONNECTED:
        if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_PHYSICAL_LINK_DISCONNECT))
        {
          /*                                                                   */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "CONNECTED", "DISCONNECTING");

            gotoDisconnecting(
                  btampContext,
                  VOS_FALSE, 
                  0, 
                  VOS_TRUE, 
                  WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);

          WLANBAP_DeInitLinkSupervision(( ptBtampHandle)btampContext);
          /*                           */
          //                    
          sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  eCSR_DISCONNECT_REASON_UNSPECIFIED);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_INDICATES_MEDIA_DISCONNECTION))
        {

          /*                                                                   */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "CONNECTED", "DISCONNECTING");
          WLANBAP_DeInitLinkSupervision(( ptBtampHandle)btampContext);

          gotoDisconnecting(
                  btampContext,
                  VOS_FALSE, 
                  0, 
                  VOS_TRUE, 
                  WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
            /*                           */
            sme_RoamDisconnect(hHal,
                               btampContext->sessionId,
                               eCSR_DISCONNECT_REASON_UNSPECIFIED);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);
        }
        else
        {
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, in state %s, invalid event msg %d", __func__, "CONNECTED", msg);
          /*                          */
        }
      break;

/*                                                                                   
                                                                              
                            
             
                                                                                
                      
 */
      case DISCONNECTING:
         VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, Entered DISCONNECTING:", __func__);//               
        if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_READY_FOR_CONNECTIONS
           ) && (btampContext->gDiscRequested == VOS_TRUE))
        {
          /*                                                                      */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "DISCONNECTING", "DISCONNECTED");

    //                     
    btampContext->gDiscRequested = VOS_FALSE;
           
    if(btampContext->BAPDeviceRole == BT_INITIATOR) 
          {
                if(!VOS_IS_STATUS_SUCCESS(vos_lock_acquire(&btampContext->bapLock)))
                {
                   VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,"btampFsm, Get LOCK Fail");
                }
              authRsnFsmFree(btampContext);
                if(!VOS_IS_STATUS_SUCCESS(vos_lock_release(&btampContext->bapLock)))
                {
                   VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,"btampFsm, Release LOCK Fail");
                }
          }
          else if(btampContext->BAPDeviceRole == BT_RESPONDER)
          {
              suppRsnFsmFree(btampContext);
          }

          /*                                                                */ 
          vosStatus = WLANBAP_GetStaIdFromLinkCtx ( 
                    btampHandle,  /*                       */ 
                    btampContext->phy_link_handle,  /*                          */
                    &ucSTAId,  /*                                     */
                    &pHddHdl); /*                              */
          if ( VOS_STATUS_SUCCESS != vosStatus ) 
          {
              VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
                          "Unable to retrieve STA Id from BAP context and phy_link_handle in %s", __func__);
              return VOS_STATUS_E_FAULT;
          }
          WLANTL_ClearSTAClient(btampContext->pvosGCtx, ucSTAId);

    //                                     

      //                                                                                                                                       
          /*                       */
        //                                                  

          signalHCIPhysLinkDiscEvent 
              ( btampContext, 
                WLANBAP_STATUS_SUCCESS,
                btampContext->gDiscReason);
          /*                                                        
                                                                  */
          /*                           */
          gotoDisconnected(btampContext);

          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "%s:In DISCONNECTING-changing outer state var to DISCONNECTED", __func__);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTED);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_READY_FOR_CONNECTIONS
                ) && (btampContext->gNeedPhysLinkCompEvent == VOS_TRUE))
        {
          /*                                                                      */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s gNeedPhysLinkComp TRUE", __func__, "DISCONNECTING", "DISCONNECTED");
          if(btampContext->BAPDeviceRole == BT_INITIATOR) 
          {
              if(!VOS_IS_STATUS_SUCCESS(vos_lock_acquire(&btampContext->bapLock)))
              {
                  VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,"btampFsm, Get LOCK Fail");
              }
              authRsnFsmFree(btampContext);
              if(!VOS_IS_STATUS_SUCCESS(vos_lock_release(&btampContext->bapLock)))
              {
                  VOS_TRACE(VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR,"btampFsm, Release LOCK Fail");
              }

          }
          else if(btampContext->BAPDeviceRole == BT_RESPONDER)
          {
              suppRsnFsmFree(btampContext);
          }
          /*                                                                */ 
          vosStatus = WLANBAP_GetStaIdFromLinkCtx ( 
                    btampHandle,  /*                       */ 
                    btampContext->phy_link_handle,  /*                          */
                    &ucSTAId,  /*                                     */
                    &pHddHdl); /*                              */
          if ( VOS_STATUS_SUCCESS != vosStatus ) 
          {
              VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO,
                          "Unable to retrieve STA Id from BAP context and phy_link_handle in %s", __func__);
              return VOS_STATUS_E_FAULT;
          }
          WLANTL_ClearSTAClient(btampContext->pvosGCtx, ucSTAId);


          /*                           */
         //                                                                                                  
          signalHCIPhysLinkCompEvent(btampContext, btampContext->gPhysLinkStatus);
          gotoDisconnected(btampContext);
          /*                                                        
                                                                  */
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTED);
         //                                                                         
        }
        else
        {
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, in state %s, invalid event msg %d", __func__, "DISCONNECTING", msg);
          /*                          */
        }
      break;

      case KEYING:
        if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_TIMER_CONNECT_ACCEPT_TIMEOUT))
        {
          /*                                                                */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "KEYING", "DISCONNECTING");

          /*                           */
          //                    
          sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  eCSR_DISCONNECT_REASON_UNSPECIFIED);
          gotoDisconnecting(
                  btampContext,
                  VOS_TRUE, 
                  WLANBAP_ERROR_HOST_TIMEOUT,
                  VOS_FALSE, 
                  0);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_PHYSICAL_LINK_DISCONNECT))
        {
          /*                                                                */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "KEYING", "DISCONNECTING");

          /*                           */
          //                    
          sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  eCSR_DISCONNECT_REASON_UNSPECIFIED);

          gotoDisconnecting(
                  btampContext,
                  VOS_TRUE, 
                  WLANBAP_ERROR_NO_CNCT,
                  //                                       
                  VOS_FALSE, 
                  WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);

          //                                                             
          //                     
          signalHCIPhysLinkDiscEvent 
              ( btampContext, 
                WLANBAP_STATUS_SUCCESS,
                WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_KEY_SET_SUCCESS))
        {
          /*                                                            */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "KEYING", "CONNECTED");

          /*                           */
          gotoConnected(btampContext);
          /*                       */
          btampfsmChangeToState(instanceVar,CONNECTED);
          signalHCIPhysLinkCompEvent(btampContext, WLANBAP_STATUS_SUCCESS);
        }
        else
        {
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, in state %s, invalid event msg %d", __func__, "KEYING", msg);
          /*                          */
        }
      break;

      case SCANNING:
        if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_SCAN_COMPLETE))
        {
          /*                                                             */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "SCANNING", "STARTING");

          /*                           */
          vosStatus = determineChan(btampContext, BT_INITIATOR, &channel, status);
          //                                                                     
          /*                       */
          btampfsmChangeToState(instanceVar,STARTING);
          vosStatus = gotoStarting( btampContext, bapEvent, eCSR_BSS_TYPE_WDS_AP, status); 
          if (VOS_STATUS_SUCCESS != vosStatus)
          {
              btampfsmChangeToState(instanceVar, SCANNING);
          }
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_TIMER_CONNECT_ACCEPT_TIMEOUT))
        {
          /*                                                                 */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "SCANNING", "DISCONNECTED");

          /*                           */
          gotoDisconnected(btampContext);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTED);

          signalHCIPhysLinkCompEvent( btampContext, WLANBAP_ERROR_HOST_TIMEOUT);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_PHYSICAL_LINK_DISCONNECT))
        {
          /*                                                                 */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "SCANNING", "DISCONNECTED");

          /*                           */
          gotoDisconnected(btampContext);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTED);
          signalHCIPhysLinkDiscEvent 
              ( btampContext, 
                WLANBAP_STATUS_SUCCESS,
                WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
          signalHCIPhysLinkCompEvent( btampContext, WLANBAP_ERROR_NO_CNCT);
        }
        else
        {
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, in state %s, invalid event msg %d", __func__, "SCANNING", msg);
          /*                          */
        }
      break;

      case VALIDATED:
        if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_MAC_CONNECT_COMPLETED
           ) && (btampContext->BAPDeviceRole == BT_INITIATOR))
        {
          /*                                                                    */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "VALIDATED", "AUTHENTICATING");

            gotoAuthenticating(btampContext);
          /*                           */
          initRsnAuthenticator(btampContext, BT_INITIATOR);
          /*                       */
          btampfsmChangeToState(instanceVar,AUTHENTICATING);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_TIMER_CONNECT_ACCEPT_TIMEOUT))
        {
          /*                                                                   */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "VALIDATED", "DISCONNECTING");

          /*                           */
          //                    
          sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  eCSR_DISCONNECT_REASON_UNSPECIFIED);
          gotoDisconnecting(
                  btampContext,
                  VOS_TRUE, 
                  WLANBAP_ERROR_HOST_TIMEOUT,
                  VOS_FALSE, 
                  0);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);
        }
        else if((msg==(BTAMPFSM_EVENT_T)eWLAN_BAP_HCI_PHYSICAL_LINK_DISCONNECT))
        {
          /*                                                                   */
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, from state %s => %s", __func__, "VALIDATED", "DISCONNECTING");

          /*                           */
          //                    
          sme_RoamDisconnect(hHal, 
                  //                 
                  btampContext->sessionId, 
                  eCSR_DISCONNECT_REASON_UNSPECIFIED);

          gotoDisconnecting(
                  btampContext,
                  VOS_TRUE, 
                  WLANBAP_ERROR_NO_CNCT,
                  //                                       
                  VOS_FALSE, 
                  WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
          /*                       */
          btampfsmChangeToState(instanceVar,DISCONNECTING);

          //                                                             
          //                     
          signalHCIPhysLinkDiscEvent 
              ( btampContext, 
                WLANBAP_STATUS_SUCCESS,
                WLANBAP_ERROR_TERM_BY_LOCAL_HOST);
        }
        else
        {
          VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, in state %s, invalid event msg %d", __func__, "VALIDATED", msg);
          /*                          */
        }
      break;

      default:
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_INFO_HIGH, "In %s, invalid state %d", __func__, instanceVar->stateVar);
        /*                        */
      break;
  }

  return vosStatus;
}

VOS_STATUS btampEstablishLogLink(ptBtampContext btampContext)
{  
   VOS_STATUS      vosStatus = VOS_STATUS_SUCCESS;
   vos_msg_t       msg;

   tAniBtAmpLogLinkReq *pMsg;

   pMsg = vos_mem_malloc(sizeof(tAniBtAmpLogLinkReq));
   if ( NULL == pMsg ) 
   {
      VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "In %s, failed to allocate mem for req", __func__);
      return VOS_STATUS_E_NOMEM;
   }

   pMsg->msgType = pal_cpu_to_be16((tANI_U16)eWNI_SME_BTAMP_LOG_LINK_IND);
   pMsg->msgLen = (tANI_U16)sizeof(tAniBtAmpLogLinkReq);
   pMsg->sessionId = btampContext->sessionId;
   pMsg->btampHandle = btampContext;

   msg.type = eWNI_SME_BTAMP_LOG_LINK_IND;
   msg.bodyptr = pMsg;
   msg.reserved = 0;

   if(VOS_STATUS_SUCCESS != vos_mq_post_message(VOS_MQ_ID_SME, &msg))
   {
       VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "In %s, failed to post msg to self", __func__);
       vos_mem_free(pMsg);
       vosStatus = VOS_STATUS_E_FAILURE;
   }
   return vosStatus;
}

void btampEstablishLogLinkHdlr(void* pMsg)
{
    tAniBtAmpLogLinkReq *pBtAmpLogLinkReq = (tAniBtAmpLogLinkReq*)pMsg;
    ptBtampContext btampContext;

    if(pBtAmpLogLinkReq)
    {
        btampContext = (ptBtampContext)pBtAmpLogLinkReq->btampHandle;
        if(NULL != btampContext)
        {
            vos_sleep( 200 );
            WLAN_BAPEstablishLogicalLink(btampContext);
        }
        else
        {
            VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "In %s, btampContext is NULL", __func__);                  
            return;
        }
            
    }
    else
    {
        VOS_TRACE( VOS_MODULE_ID_BAP, VOS_TRACE_LEVEL_ERROR, "In %s, pBtAmpLogLinkReq is NULL", __func__);    
    }
    return;
}


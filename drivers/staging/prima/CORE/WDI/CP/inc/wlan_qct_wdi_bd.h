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
#ifndef WLAN_QCT_WDI_BD_H
#define WLAN_QCT_WDI_BD_H

/*===========================================================================

         W L A N   D E V I C E   A B S T R A C T I O N   L A Y E R 
              I N T E R N A L     A P I       F O R    T H E
                B D   H E A D E R   D E F I N I T I O N 
                
                   
DESCRIPTION
  This file contains the internal BD definition exposed by the DAL Control       
  Path Core module to be used by the DAL Data Path Core. 
  
      
  Copyright (c) 2010 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Confidential and Proprietary
===========================================================================*/


/*                                                                           

                                           


                                                                       
                                                                


                                   


                                   
                                                                             
                                   

                                                                           */

#include "wlan_qct_pal_type.h"


/*                                                                                  
                           
                                                                          */
/*                                                                                    
                                         
                                                                           */
typedef struct 
{
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                        
                                                                                
                                               */
        wpt_uint32 dpuRF:8;
    
        /*                                                                    */
        wpt_uint32 dpuSignature:3;
    
        /*                                                    
                                                                                  
                                                                                
                                        */
        wpt_uint32 stAuF:1;
    
        /*                                 */
        wpt_uint32 A2HF:1;
    
        /*                                                      */
        wpt_uint32 bsf:1;
    
        /*                                                                         
                          */
        wpt_uint32 rtsf:1;

#ifdef WCN_PRONTO_CSU
        /*                                                                            */
        wpt_uint32 csuNoValHd:1;

        /*                                                                                                  */
        wpt_uint32 csuVerifiedTLChksum:1;

        /*                                                                   */
        wpt_uint32 csuVerifiedIPChksum:1;

        /*                                                                     */
        wpt_uint32 csuChksumValid:1;

        /*                                                                */
        wpt_uint32 csuTLChksumError:1;

        /*                                                                    */
        wpt_uint32 csuIPChksumError:1;
#else /*          */
        /*                                                                                     */
        wpt_uint32 rxChannel:4;
        wpt_uint32 scanLearn:1;
        wpt_uint32 reserved0:1;
#endif /*          */

        /*             
                                                                 
                                                                */
        wpt_uint32 llcr:1;
        
        wpt_uint32 umaByPass:1;
    
        /*                                                                         
                                                                               
                                                                                    
                                            
                            
                                   */
        wpt_uint32 rmf:1;
    
        /*  
                                                                      
                                                              
                          
                                      
                                                 */
        wpt_uint32 ub:1;
    
        /*                                                                         
                                                       
                                          
                                        
                                                            */
        wpt_uint32 rxKeyId:3;
        
        /*                             
                                
                    
                                                                             
                                                */
        wpt_uint32 dpuNE:1;
    
        /*  
                                                                             
                                                     
                                                                                
                                                                           
                                                                             
                         
                                         
                                      
                       
                                                                                    
                                                                              
                     */
        wpt_uint32 ft:1;
    
        /*                        
                
                                           
                                    
                                        */
        wpt_uint32 bdt:2;
        
#else
        wpt_uint32 bdt:2;
        wpt_uint32 ft:1;
        wpt_uint32 dpuNE:1;
        wpt_uint32 rxKeyId:3;
        wpt_uint32 ub:1;
        wpt_uint32 rmf:1;
        wpt_uint32 umaByPass:1;
        wpt_uint32 llcr:1;

#ifdef WCN_PRONTO_CSU
        wpt_uint32 csuIPChksumError:1;
        wpt_uint32 csuTLChksumError:1;
        wpt_uint32 csuChksumValid:1;
        wpt_uint32 csuVerifiedIPChksum:1;
        wpt_uint32 csuVerifiedTLChksum:1;
        wpt_uint32 csuNoValHd:1;
#else /*          */
        wpt_uint32 reserved0:1;
        wpt_uint32 scanLearn:1;
        wpt_uint32 rxChannel:4;
#endif /*          */

        wpt_uint32 rtsf:1;
        wpt_uint32 bsf:1;
        wpt_uint32 A2HF:1;
        wpt_uint32 stAuF:1;
        wpt_uint32 dpuSignature:3;
        wpt_uint32 dpuRF:8;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                                                                      
                                                                             
                                              */
        wpt_uint32 penultimatePduIdx:16;
    
#ifdef WCN_PRONTO 
        wpt_uint32 aduFeedback:7;
        wpt_uint32 dpuMagicPacket: 1; 
#else
        wpt_uint32 aduFeedback:8;
#endif //          
    
        /*               */
        wpt_uint32 dpuFeedback:8;
        
#else
        wpt_uint32 dpuFeedback:8;
#ifdef WCN_PRONTO 
        wpt_uint32 dpuMagicPacket: 1; 
        wpt_uint32 aduFeedback:7;
#else
        wpt_uint32 aduFeedback:8;
#endif //          
        wpt_uint32 penultimatePduIdx:16;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                                                                      
                                                                                 
                         */
        wpt_uint32 headPduIdx:16;
    
        /*                                                                      
                                                                                */
        wpt_uint32 tailPduIdx:16;
        
#else
        wpt_uint32 tailPduIdx:16;
        wpt_uint32 headPduIdx:16;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                                                      
                                                                                   
                                 */
        wpt_uint32 mpduHeaderLength:8;
    
        /*                                            
                                                                               
                      */
        wpt_uint32 mpduHeaderOffset:8;
    
        /*                                          
                                                                               
                                                                                
                   
                                                                           */
        wpt_uint32 mpduDataOffset:9;
    
        /*                                       
                                                              */
        wpt_uint32 pduCount:7;
#else
    
        wpt_uint32 pduCount:7;
        wpt_uint32 mpduDataOffset:9;
        wpt_uint32 mpduHeaderOffset:8;
        wpt_uint32 mpduHeaderLength:8;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN

        /*                                                            
                                                                            */
        wpt_uint32 mpduLength:16;

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
        wpt_uint32 offloadScanLearn:1;
        wpt_uint32 roamCandidateInd:1;
#else
        wpt_uint32 reserved22:2;
#endif

#ifdef WCN_PRONTO
        wpt_uint32 reserved3: 1;
        wpt_uint32 rxDXEPriorityRouting:1;
#else
        wpt_uint32 reserved3:2;
#endif //          


        /*                    
                                                                             
                                   */
        wpt_uint32 tid:4;

        wpt_uint32 reserved4:8;
#else
        wpt_uint32 reserved4:8;
        wpt_uint32 tid:4;
#ifdef WCN_PRONTO
        wpt_uint32 rxDXEPriorityRouting:1;
        wpt_uint32 reserved3: 1;
#else
        wpt_uint32 reserved3:2;
#endif //          
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
        wpt_uint32 roamCandidateInd:1;
        wpt_uint32 offloadScanLearn:1;
#else
        wpt_uint32 reserved22:2;
#endif

        wpt_uint32 mpduLength:16;
#endif

        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                        
                                                                                 
                                                                             
                                                                                 
                                                                                 
                                                                                 
                                            */
        wpt_uint32 dpuDescIdx:8;
    
        /*                                                                        
                                                                   */
        wpt_uint32 addr1Index:8;
    
        /*                                                                        
                                                                   */
        wpt_uint32 addr2Index:8;
    
        /*                                                                        
                                                                   */
        wpt_uint32 addr3Index:8;
#else
        wpt_uint32 addr3Index:8;
        wpt_uint32 addr2Index:8;
        wpt_uint32 addr1Index:8;
        wpt_uint32 dpuDescIdx:8;
#endif
    
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                                           */
        wpt_uint32 rateIndex:9;
    
        /*                                                                       */
        wpt_uint32 rxpFlags:23; 
    
#else
    
        wpt_uint32 rxpFlags:23;                     /*          */
        wpt_uint32 rateIndex:9;
    
#endif
        /*          */
        /*                                                                      
                                               */
        wpt_uint32 phyStats0;                      /*                  */
        wpt_uint32 phyStats1;                      /*                  */
    
        /*      */
        /*                                                                  
                                           */
        wpt_uint32 mclkRxTimestamp;                /*                                */
    
        /*           */
        /*                                                             */
        wpt_uint32 pmiCmd4to23[5];               /*                       */
    
        /*      */
#ifdef WCN_PRONTO
#ifdef WPT_BIG_BYTE_ENDIAN
        /*                                                             */
        wpt_uint32 pmiCmd24to25:16;

        /*                                                             */
        wpt_uint32 csuChecksum:16;
#else
        wpt_uint32 csuChecksum:16;
        wpt_uint32 pmiCmd24to25:16;
#endif
#else /*          */
        /*                                                             */
        wpt_uint32 pmiCmd24to25;
#endif /*          */

        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                                                                        
                                                                               
                               */
        wpt_uint32 reorderOpcode:4;
    
        wpt_uint32 reserved6:12;
    
        /*                                                                       
                                                                                   
                */
        wpt_uint32 reorderFwdIdx:6;
    
        /*                                                                     
                                                                            
                                  */
        wpt_uint32 reorderSlotIdx:6;
        
#ifdef WCN_PRONTO
        wpt_uint32 reserved7: 2;
        wpt_uint32 outOfOrderForward: 1;
        wpt_uint32 reorderEnable: 1;
#else
        wpt_uint32 reserved7:4;
#endif //          

#else

#ifdef WCN_PRONTO
        wpt_uint32 reorderEnable: 1;
        wpt_uint32 outOfOrderForward: 1;
        wpt_uint32 reserved7: 2;
#else
        wpt_uint32 reserved7:4;
#endif //          
        wpt_uint32 reorderSlotIdx:6;
        wpt_uint32 reorderFwdIdx:6;
        wpt_uint32 reserved6:12;
        wpt_uint32 reorderOpcode:4;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
        /*                                        
                                                               */
        wpt_uint32 frameTypeSubtype:6;
        wpt_uint32 rfBand:2;
    
        /*                                                         */
        wpt_uint32 currentPktSeqNo:12;
    
        /*                                                                       
                  */
        wpt_uint32 expectedPktSeqNo:12;
#else
        wpt_uint32 expectedPktSeqNo:12;
        wpt_uint32 currentPktSeqNo:12;
        wpt_uint32 rfBand:2;
        wpt_uint32 frameTypeSubtype:6;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                                    */
        wpt_uint32 asf:1;
    
        /*                                                        */
        wpt_uint32 esf:1;
    
        /*                                                       */
        wpt_uint32 lsf:1;
    
        /*                                                */
        wpt_uint32 aef:1;
        
        wpt_uint32 reserved9:4;
    
        /*                                                           
                                                                                
                                                */
        wpt_uint32 processOrder:4;
    
        /*                                                                    
                                                                              
                                                                                
                                                                                  
                                                                                  
                                                                                  
                                                                           
                   */
        wpt_uint32 sybFrameIdx:4;
    
        /*                                             */
        wpt_uint32 totalMsduSize:16;
#else
        wpt_uint32 totalMsduSize:16;
        wpt_uint32 sybFrameIdx:4;
        wpt_uint32 processOrder:4;
        wpt_uint32 reserved9:4;
        wpt_uint32 aef:1;
        wpt_uint32 lsf:1;
        wpt_uint32 esf:1;
        wpt_uint32 asf:1;
#endif

} WDI_RxBdType;

/*                                                                                    
                                                                
                                                                           */
typedef struct 
{
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                        
                                                                                
                                               */
        wpt_uint32 dpuRF:8;
    
        /*                                                                    */
        wpt_uint32 dpuSignature:3;
    
        /*                                                    
                                                                                  
                                                                                
                                        */
        wpt_uint32 stAuF:1;
    
        /*                                 */
        wpt_uint32 A2HF:1;
    
        /*                                                      */
        wpt_uint32 bsf:1;
    
        /*                                                                         
                          */
        wpt_uint32 rtsf:1;
    
        /*                                                                                     */
        wpt_uint32 rxChannel:4;
        wpt_uint32 scanLearn:1;

        wpt_uint32 reserved0:1;
    
        /*             
                                                                 
                                                                */
        wpt_uint32 llcr:1;
        
        wpt_uint32 umaByPass:1;
    
        /*                                                                         
                                                                               
                                                                                    
                                            
                            
                                   */
        wpt_uint32 rmf:1;
    
        /*  
                                                                      
                                                              
                          
                                      
                                                 */
        wpt_uint32 ub:1;
    
        /*                                                                         
                                                       
                                          
                                        
                                                            */
        wpt_uint32 rxKeyId:3;
        
        /*                             
                                
                    
                                                                             
                                                */
        wpt_uint32 dpuNE:1;
    
        /*  
                                                                             
                                                     
                                                                                
                                                                           
                                                                             
                         
                                         
                                      
                       
                                                                                    
                                                                              
                     */
        wpt_uint32 ft:1;
    
        /*                        
                
                                           
                                    
                                        */
        wpt_uint32 bdt:2;
        
#else
        wpt_uint32 bdt:2;
        wpt_uint32 ft:1;
        wpt_uint32 dpuNE:1;
        wpt_uint32 rxKeyId:3;
        wpt_uint32 ub:1;
        wpt_uint32 rmf:1;
        wpt_uint32 reserved1:1;
        wpt_uint32 llc:1;
        wpt_uint32 reserved0:1;
        wpt_uint32 scanLearn:1;
        wpt_uint32 rxChannel:4;
        wpt_uint32 rtsf:1;
        wpt_uint32 bsf:1;
        wpt_uint32 A2HF:1;
        wpt_uint32 stAuF:1;
        wpt_uint32 dpuSignature:3;
        wpt_uint32 dpuRF:8;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                                                                      
                                                                             
                                              */
        wpt_uint32 penultimatePduIdx:16;
    
        wpt_uint32 aduFeedback:8;
    
        /*               */
        wpt_uint32 dpuFeedback:8;
        
#else
        wpt_uint32 dpuFeedback:8;
        wpt_uint32 aduFeedback:8;
        wpt_uint32 penultimatePduIdx:16;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                                                                      
                                                                                 
                         */
        wpt_uint32 headPduIdx:16;
    
        /*                                                                      
                                                                                */
        wpt_uint32 tailPduIdx:16;
        
#else
        wpt_uint32 tailPduIdx:16;
        wpt_uint32 headPduIdx:16;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                                                      
                                                                                   
                                 */
        wpt_uint32 mpduHeaderLength:8;
    
        /*                                            
                                                                               
                      */
        wpt_uint32 mpduHeaderOffset:8;
    
        /*                                          
                                                                               
                                                                                
                   
                                                                           */
        wpt_uint32 mpduDataOffset:9;
    
        /*                                       
                                                              */
        wpt_uint32 pduCount:7;
#else
    
        wpt_uint32 pduCount:7;
        wpt_uint32 mpduDataOffset:9;
        wpt_uint32 mpduHeaderOffset:8;
        wpt_uint32 mpduHeaderLength:8;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                                                             
                                                                            */
        wpt_uint32 mpduLength:16;
    
        wpt_uint32 reserved3:4;
    
        /*                    
                                                                             
                                   */
        wpt_uint32 tid:4;
        
        wpt_uint32 reserved4:7;
        wpt_uint32 fc:1;        //                                        
#else
        wpt_uint32 fc:1;        //                                        
        wpt_uint32 reserved4:7;
        wpt_uint32 tid:4;
        wpt_uint32 reserved3:4;
        wpt_uint32 mpduLength:16;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                        
                                                                                 
                                                                             
                                                                                 
                                                                                 
                                                                                 
                                            */
        wpt_uint32 dpuDescIdx:8;
    
        /*                                                                        
                                                                   */
        wpt_uint32 addr1Index:8;
    
        /*                                                                        
                                                                   */
        wpt_uint32 addr2Index:8;
    
        /*                                                                        
                                                                   */
        wpt_uint32 addr3Index:8;
#else
        wpt_uint32 addr3Index:8;
        wpt_uint32 addr2Index:8;
        wpt_uint32 addr1Index:8;
        wpt_uint32 dpuDescIdx:8;
#endif
    
#ifdef WPT_BIG_BYTE_ENDIAN
    
        /*                                           */
        wpt_uint32 rateIndex:9;
    
        /*                                                                       */
        wpt_uint32 rxpFlags:23; 
    
#else
    
        wpt_uint32 rxpFlags:23;                     /*          */
        wpt_uint32 rateIndex:9;
    
#endif
        /*          */
        /*                                                                      
                                               */
        wpt_uint32 phyStats0;                      /*                  */
        wpt_uint32 phyStats1;                      /*                  */
    
        /*      */
        /*                                                                  
                                           */
        wpt_uint32 mclkRxTimestamp;                /*                                */
    
       /*            */     
#ifdef WPT_BIG_BYTE_ENDIAN  
        /*                                                                                                     */
        wpt_uint32 fcSTAValidMask:16;
        /*                                                                                                         */
        wpt_uint32 fcSTAPwrSaveStateMask:16;
        /*                                                                                                          
                                                                                                                    
                                                                              */        
        wpt_uint32 fcSTAThreshIndMask:16;
        /*                                                                                          */
        wpt_uint32 fcSTATxQStatus:16;
#else
        wpt_uint32 fcSTATxQStatus:16;
        wpt_uint32 fcSTAThreshIndMask:16;
        wpt_uint32 fcSTAPwrSaveStateMask:16;
        wpt_uint32 fcSTAValidMask:16;
#endif
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN 
        wpt_uint32 fcStaTxDisabledBitmap:16;
        wpt_uint32 reserved5:16;
#else   
        wpt_uint32 reserved5:16;
        wpt_uint32 fcStaTxDisabledBitmap:16;
#endif
        
        //                        
        /*             */
        wpt_uint32  fcSTATxQLen[12];            //                   
        wpt_uint32  fcSTACurTxRate[12];         //                               

} WDI_FcRxBdType; //               

/*                                                                                    
                                         
                                                                           */
typedef struct 
{
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
        /*                                                                     
                                                                           */
        wpt_uint32 dpuRF:8;
    
        /*                                                                     */
        wpt_uint32 dpuSignature:3;

#ifdef WCN_PRONTO        
        /*            */
        wpt_uint32 reserved0:2;

         /*                                                                        
                                 */
        wpt_uint32 terminateAMPDU:1;

       /*                                                                        
                                                                                 
                                                                                 
                                                                                   
                   */
        wpt_uint32 umaBssidIdx:2;

        /*                                                               
                                    */
        wpt_uint32 umaBDEnable:1;

        /*                        
                    
                                                                                                  
        */
        wpt_uint32 csuSWMode:1;

        /*                                     
                                                    
                  
        */
        wpt_uint32 csuTXEnable:1;

        /*                                                
                                                      
                                                     
        */
        wpt_uint32 csuEnableTLCksum:1;

        /*                                         
                                                       
                                                       
        */
        wpt_uint32 csuEnableIPCksum:1;

        /*                                                                                       
                                                      
                                                          
         */
        wpt_uint32 csuTLCksumGenerated:1;

        /*                                 
                                                                                     
                             
        */
        wpt_uint32 csuNoValidHeader:1;
#else /*          */
        wpt_uint32 reserved0:12;
#endif /*          */

        /*                                                        
                                                                              
                                                                                
               
                            
                                 
                                                                          
                                                   */
        wpt_uint32 rmf:1;
    
        /*                                                                
                                                              
                          
                                      
                                                 */
        wpt_uint32 ub:1;
    
        wpt_uint32 reserved1:1;
    
        /*                                                                 
                                                                       
                        
                               */
        wpt_uint32 txComplete1:1;
        wpt_uint32 fwTxComplete0:1;
        
        /*                        
                                
                    
                                                                             
                                                */
        wpt_uint32 dpuNE:1;
    
        
        /*                                                                       
                                                                             
                                                     
                                         
                                       */
        wpt_uint32 ft:1;
    
        /*          
                                           
                                    
                                        */
        wpt_uint32 bdt:2;
#else
        wpt_uint32 bdt:2;
        wpt_uint32 ft:1;
        wpt_uint32 dpuNE:1;
        wpt_uint32 fwTxComplete0:1; 
        wpt_uint32 txComplete1:1;
        wpt_uint32 reserved1:1;
        wpt_uint32 ub:1;
        wpt_uint32 rmf:1;
#ifdef WCN_PRONTO        
        wpt_uint32 csuNoValidHeader:1;
        wpt_uint32 csuTLCksumGenerated:1;
        wpt_uint32 csuEnableIPCksum:1;
        wpt_uint32 csuEnableTLCksum:1;
        wpt_uint32 csuTXEnable:1;
        wpt_uint32 csuSWMode:1;
        wpt_uint32 umaBDEnable:1;
        wpt_uint32 umaBssidIdx:2;
        wpt_uint32 terminateAMPDU:1;
        wpt_uint32 reserved0:2;
#else /*          */
        wpt_uint32 reserved0:12;
#endif /*          */
        wpt_uint32 dpuSignature:3;
        wpt_uint32 dpuRF:8;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved2:16; /*                                       */
        wpt_uint32 aduFeedback:8;
    
        /*                         */
        wpt_uint32 dpuFeedback:8;
    
#else
        wpt_uint32 dpuFeedback:8;
        wpt_uint32 aduFeedback:8;
        wpt_uint32 reserved2:16;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
        /*                                                                       
                                                                             
                                                                                
                                                 */
        wpt_uint32 headPduIdx:16;
    
        /*                                                                        
                                                                            
                                                                               
                                */
        wpt_uint32 tailPduIdx:16;
#else
        wpt_uint32 tailPduIdx:16;
        wpt_uint32 headPduIdx:16;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
        /*                                                                   
                                                                           
                                                                                   
                                 */
        wpt_uint32 mpduHeaderLength:8;
    
        /*                                                                   
                                                                                    
                                                                           */
        wpt_uint32 mpduHeaderOffset:8;
    
        /*                                                                   
                                                                                   
                                                                          
                                                                               
                                                                           */
        wpt_uint32 mpduDataOffset:9;
    
        /*                                                                       
                                                                               
                                                   */
        wpt_uint32 pduCount:7;
#else
        wpt_uint32 pduCount:7;
        wpt_uint32 mpduDataOffset:9;
        wpt_uint32 mpduHeaderOffset:8;
        wpt_uint32 mpduHeaderLength:8;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
        /*                                                                   
                                                                                    
                                                                              
                       */
        wpt_uint32 mpduLength:16;
    
        wpt_uint32 reserved3:2;
        /*                                  
                                                          
                                                                                
                                                                                  
               
                                                         
                     */
        wpt_uint32 bd_ssn:2;
    
        /*                    
                                                                                  
                              */
        wpt_uint32 tid:4;
        
        wpt_uint32 reserved4:8;
    
#else
        wpt_uint32 reserved4:8;
        wpt_uint32 tid:4;
        wpt_uint32 bd_ssn:2;
        wpt_uint32 reserved3:2;
        wpt_uint32 mpduLength:16;
#endif
    
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
        /*                        
                                                                        
                                                                              
                                                                                 
                                                                                
                                                                            
                                                                               
                                                                   */
        wpt_uint32 dpuDescIdx:8;
    
        /*                                                                  
                                                    */
        wpt_uint32 staIndex:8;
    
        /*                                                                         
                  
                 
                          */
        wpt_uint32 ap:2;
    
        /*                                        
                                                         
                             
                             
                                      */
        wpt_uint32 bdRate:2;
    
        /*  
                                                                        
                                 */
        wpt_uint32 queueId:5;
    
        wpt_uint32 reserved5:7;
#else
        wpt_uint32 reserved5:7;
        wpt_uint32 queueId:5;
        wpt_uint32 bdRate:2;
        wpt_uint32 ap:2;
        wpt_uint32 staIndex:8;
        wpt_uint32 dpuDescIdx:8;
#endif

        wpt_uint32 txBdSignature;

        /*      */
        wpt_uint32 reserved6;
        /*      */
        /*                                                         */
        wpt_uint32 dxeH2BStartTimestamp;
    
        /*      */
        /*                                                          */
        wpt_uint32 dxeH2BEndTimestamp;

#ifdef WCN_PRONTO
#ifdef WPT_BIG_BYTE_ENDIAN
        /*                                                                  
                                     */
        wpt_uint32 csuTcpUdpStartOffset:10;

        /*                                                                   
                          */
        wpt_uint32 csuPseudoHeaderCksum:16;

        wpt_uint32 reserved7:6;
#else
        wpt_uint32 reserved7:6;
        wpt_uint32 csuPseudoHeaderCksum:16;
        wpt_uint32 csuTcpUdpStartOffset:10;
#endif
#endif /*          */

} WDI_TxBdType;

/*                                                                                    
                                                            
                                                                           */
typedef struct 
{
        /*      */
#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved1:30;
        wpt_uint32 bdt:2;
#else
        wpt_uint32 bdt:2;
        wpt_uint32 reserved1:30;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved2:24;
        wpt_uint32 dpuFeedBack:8;
#else
        wpt_uint32 dpuFeedBack:8;
        wpt_uint32 reserved2:24;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved3:16;
        wpt_uint32 frag0BdIdx:16;
#else
        wpt_uint32 frag0BdIdx:16;
        wpt_uint32 reserved3:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved4:16;
        wpt_uint32 frag1BdIdx:16;
#else
        wpt_uint32 frag1BdIdx:16;
        wpt_uint32 reserved4:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 frag2BdIdx:16;
        wpt_uint32 reserved5:16;
#else
        wpt_uint32 frag2BdIdx:16;
        wpt_uint32 reserved5:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved6:16;
        wpt_uint32 frag3BdIdx:16;
#else
        wpt_uint32 frag3BdIdx:16;
        wpt_uint32 reserved6:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved7:16;
        wpt_uint32 frag4BdIdx:16;
#else
        wpt_uint32 frag4BdIdx:16;
        wpt_uint32 reserved7:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved8:16;
        wpt_uint32 frag5BdIdx:16;
#else
        wpt_uint32 frag5BdIdx:16;
        wpt_uint32 reserved8:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved9:16;
        wpt_uint32 frag6BdIdx:16;
#else
        wpt_uint32 frag6BdIdx:16;
        wpt_uint32 reserved9:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved10:16;
        wpt_uint32 frag7BdIdx:16;
#else
        wpt_uint32 frag7BdIdx:16;
        wpt_uint32 reserved10:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved11:16;
        wpt_uint32 frag8BdIdx:16;
#else
        wpt_uint32 frag8BdIdx:16;
        wpt_uint32 reserved11:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved12:16;
        wpt_uint32 frag9BdIdx:16;
#else
        wpt_uint32 frag9BdIdx:16;
        wpt_uint32 reserved12:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved13:16;
        wpt_uint32 frag10BdIdx:16;
#else
        wpt_uint32 frag10BdIdx:16;
        wpt_uint32 reserved13:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved14:16;
        wpt_uint32 frag11BdIdx:16;
#else
        wpt_uint32 frag11BdIdx:16;
        wpt_uint32 reserved14:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved15:16;
        wpt_uint32 frag12BdIdx:16;
#else
        wpt_uint32 frag12BdIdx:16;
        wpt_uint32 reserved15:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 reserved16:16;
        wpt_uint32 frag13BdIdx:16;
#else
        wpt_uint32 frag13BdIdx:16;
        wpt_uint32 reserved16:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 frag14BdIdx:16;
        wpt_uint32 reserved17:16;
#else
        wpt_uint32 frag14BdIdx:16;
        wpt_uint32 reserved17:16;
#endif

#ifdef WPT_BIG_BYTE_ENDIAN
        wpt_uint32 frag15BdIdx:16;
        wpt_uint32 reserved18:16;
#else
        wpt_uint32 frag15BdIdx:16;
        wpt_uint32 reserved18:16;
#endif

} WDI_RxDeFragBdType;

#endif /*                 */

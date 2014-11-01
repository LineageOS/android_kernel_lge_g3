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

#ifndef WLAN_QCT_DAL_STA_H
#define WLAN_QCT_DAL_STA_H

/*===========================================================================

         W L A N   D E V I C E   A B S T R A C T I O N   L A Y E R 
              I N T E R N A L     A P I       F O R    T H E
                       S T A T I O N    M G M T
                
                   
DESCRIPTION
  This file contains the internal API exposed by the STA Management entity to
  be used by the DAL Control Path Core . 
  
      
  Copyright (c) 2010 QUALCOMM Incorporated. All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/


/*                                                                           

                                           


                                                                       
                                                                


                                   


                                   
                                                                             
                                   

                                                                           */

#include "wlan_qct_pal_type.h"
#include "wlan_qct_pal_api.h"

/*                                                                            
                                        
                                                                           */
/*                      */
#define WDI_STA_INVALID_IDX 0xFF

/*                                                                            
                  
                                                                           */
typedef struct 
{
  wpt_uint8    ucSTAIdx; 
  wpt_uint8    ucWmmEnabled;
  wpt_uint8    ucHTCapable; 

  /*                    */
  wpt_macAddr staMacAddr;

  /*                      */
  wpt_macAddr  macBSSID;

  /*                                                                              
                                                           
                                                          
                                         */
  wpt_uint8   ucStaType;       


  /*               */
  wpt_uint8   dpuIndex;                      //                
  wpt_uint8   dpuSig;                        //              
  wpt_uint8   bcastDpuIndex;
  wpt_uint8   bcastDpuSignature;
  wpt_uint8   bcastMgmtDpuIndex;
  wpt_uint8   bcastMgmtDpuSignature;


  /*                    */
  wpt_uint8   ucRmfEnabled;

  /*                                  */
  wpt_uint8   ucBSSIdx;

}WDI_AddStaParams; 

/*                                                                            
               
                                                                           */
typedef struct
{
  wpt_macAddr staAddr;                //         
     
  wpt_uint8 valid:1;                           //                   
  wpt_uint8 rmfEnabled:1;
  wpt_uint8 htEnabled:1;

  /*                                                */
  wpt_uint8 qosEnabled:1;         

  wpt_uint8 bssIdx;                         //          
  wpt_uint8 staId;

  wpt_macAddr macBSSID;
  //                                                                              
  //                                                       
  //                                                      
  //                                    
  wpt_uint8   ucStaType;       


  /*               */
  wpt_uint8 dpuIndex;                      //                
  wpt_uint8 dpuSig;                        //              
  wpt_uint8 bcastDpuIndex;
  wpt_uint8 bcastDpuSignature;
  wpt_uint8 bcastMgmtDpuIndex;
  wpt_uint8 bcastMgmtDpuSignature;
     
} WDI_StaStruct;

/* 
                                                       
                                      

 
                                                          
  
     
                                    
*/
WDI_Status WDI_STATableInit
(
   WDI_ControlBlockType*  pWDICtx
);

/* 
                                                                
                

 
                                                          
  
     
                                    
*/
WDI_Status 
WDI_STATableStart
(
    WDI_ControlBlockType*  pWDICtx
);

/* 
                                               

 
                                                          
  
     
                                    
*/
WDI_Status 
WDI_STATableStop
(
    WDI_ControlBlockType*  pWDICtx
);

/* 
                                                                
              

 
                                                          
  
     
                                    
*/
WDI_Status 
WDI_STATableClose
(
  WDI_ControlBlockType*  pWDICtx
);


/* 
                                                    

 
                                                      
                                          
  
     
                                    
*/
WDI_Status
WDI_STATableAddSta
(
    WDI_ControlBlockType*  pWDICtx,
    WDI_AddStaParams*      pwdiParam
);

/* 
                                                         

 
                                                          
                                               
  
     
                                    
*/
WDI_Status
WDI_STATableDelSta
(
    WDI_ControlBlockType*  pWDICtx,
    wpt_uint8              ucSTAIdx
);

/* 
                                                                       

 
                                                          
                                  
  
     
                                    
*/
WDI_Status
WDI_STATableBSSDelSta
(
    WDI_ControlBlockType*  pWDICtx,
    wpt_uint8              ucBssIdx
);

/* 
                                                                
                         

 
                                                          
                                       
                                               
  
     
                                    
*/
WDI_Status 
WDI_STATableGetStaBSSIDAddr
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_macAddr*           pmacBSSID
);
/* 
                                                              
                 

 
                                                          
                                       
                                         
  
     
                                    
*/
WDI_Status 
WDI_STATableGetStaQosEnabled
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_uint8*             qosEnabled
);

/* 
                                                           

 
                                                     
                                  
                                
  
     
                                    
*/
WDI_Status 
WDI_STATableSetStaQosEnabled
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_uint8              qosEnabled
);

/* 
                                                     

 
                                                    
                                 
                               
  
     
                                    
*/
WDI_Status 
WDI_STATableGetStaType
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_uint8*             pStaType
);

/* 
                                                      

 
                                                    
                                 
                            
  
     
                                    
*/
WDI_Status 
WDI_STATableSetStaType
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_uint8              staType
);


/* 
                                                                         
                                                                   
 
                                      
                                  
                                     
  
     
                                    
*/
WDI_Status
WDI_STATableFindStaidByAddr
(
    WDI_ControlBlockType*  pWDICtx, 
    wpt_macAddr            staAddr, 
    wpt_uint8*             pucStaId
);

/* 
                                                    
 
                                      
                                 
                                          
  
     
                                    
*/
WDI_Status 
WDI_STATableGetStaAddr
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_uint8**            pStaAddr
);

/* 
                                                    
 
                                      
                                  
                                          
  
     
                                    
*/
WDI_Status 
WDI_STATableSetStaAddr
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_macAddr            staAddr
);

/* 
                                                              
 
                                      
                                  
                                          
  
     
                                    
*/
WDI_Status 
WDI_STATableSetBSSID
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_macAddr            macBSSID
);

/* 
                                                                   
 
                                      
                                  
                             
  
     
                                    
*/
WDI_Status 
WDI_STATableSetBSSIdx
(
    WDI_ControlBlockType*  pWDICtx,  
    wpt_uint8              ucSTAIdx, 
    wpt_uint8              ucBSSIdx
);

#endif /*                  */


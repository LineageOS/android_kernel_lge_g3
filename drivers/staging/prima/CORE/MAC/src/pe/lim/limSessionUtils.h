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


#if!defined( __LIM_SESSION_UTILS_H )
#define __LIM_SESSION_UTILS_H

/*
* Copyright (c) 2012-2013 Qualcomm Atheros, Inc.
* All Rights Reserved.
* Qualcomm Atheros Confidential and Proprietary.
*/

/**=========================================================================
  
  \file  limSessionUtils.h
  
  \brief prototype for lim Session Utility related APIs

  \author Sunit Bhatia
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/


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
tANI_U8 peGetVhtCapable(tpAniSirGlobal pMac);


/*                                                                          
                                                           

                                                                                                            
                                                

                                                  
                                                                               

     
                                                                            */
tANI_U8 peValidateBtJoinRequest(tpAniSirGlobal pMac);

/*                                                                           */


/*                                                                          
                                                                                 

                                                                                                                        
                  
                                                                                                                            
                                      
    
                                                                   
                                                                                                               
  
     
                                                                            */


tpPESession peGetValidPowerSaveSession(tpAniSirGlobal pMac);

/*                                                                           */


/*                                                                          
                                                                          

                                                                                          
      
                                                                   
                                                                                                      
  
     
                                                                            */

tANI_U8 peIsAnySessionActive(tpAniSirGlobal pMac);

/*                                                                           */


/*                                                                          
                                                                          

                                                      

                                                                   

     
                                                                            */

void pePrintActiveSession(tpAniSirGlobal pMac);

/*                                                                           */



/*                                                                          
                                                                 
                                                    

                                                                                
                                                  
    
                                                                   
                                                                             
  
                                                                          
  
     
                                                                            */
tANI_U8
isLimSessionOffChannel(tpAniSirGlobal pMac, tANI_U8 sessionId);
/*                                                                           */

/*                                                                          
                                                                                          
                                                                                             
                                                              
                                                                                                    
                                                                                                      
                                                                          */
void
peGetActiveSessionChannel(tpAniSirGlobal pMac, tANI_U8* resumeChannel, ePhyChanBondState* resumePhyCbState);

/*                                                                          
                                                                               
                                                  

                                                                   
  
                                                                 
                                                                         
  
     
                                                                            */
tANI_U8
limIsChanSwitchRunning (tpAniSirGlobal pMac);

/*                                                                          
                                                                               
                                                  

                                                                   
  
                                                             
                                                                     
  
     
                                                                            */
tANI_U8
limIsInQuietDuration (tpAniSirGlobal pMac);

/*                                                                          
                                                                           
                                                  

                                                                   
  
                                                             
                                                                     
  
     
                                                                            */
tANI_U8
limIsQuietBegin (tpAniSirGlobal pMac);
/*                                                                          
                                                  

                                                                   
  
                                                   
                                                      
  
     
                                                                            */
tANI_U8
limIsInMCC (tpAniSirGlobal pMac);
/*                                                                          
                                                                        
                                                  

                                                                   
                                                                           
  
     
                                                                            */
tANI_U8
peGetCurrentSTAsCount(tpAniSirGlobal pMac);

#ifdef FEATURE_WLAN_LFR
/*                                                                          
                                                                         

                                                                   
                                            
                                               

     
                                                                            */
tANI_U8
limIsFastRoamEnabled(tpAniSirGlobal pMac, tANI_U8 sessionId);
#endif


#endif //                                     


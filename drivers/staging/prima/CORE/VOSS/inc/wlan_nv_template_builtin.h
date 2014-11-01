/*
 * Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of The Linux Foundation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 *WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 *ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 *BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 *IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#if !defined _WLAN_NV_TEMPLATE_BUILTIN_H
#define _WLAN_NV_TEMPLATE_BUILTIN_H


/*                                                                           
                                            

                                                                       
                                                                

                                   

                                         
                                                                        
                                              
                                                                           */


/*
                                           
                                            
                                           
                          
                                                                        
                

                     
                      
                                                            
                                                 
                                                              
                      
                                                    
                                                  
                                                  
                                                       
                                                         

                      
                                                                             
                                                                         
                     
                                                                               
                                                                          
                                          
                                                                             
                                         
                                                                                
                                                                  
                                                        
                                                         
                                                                               
                                        
                                                                               
                         
                                                                         
                                
                                                                           
                                                                          
                                                                               
                                                                                
                                                     
                                                                           
                   
                                                                      
                                                                     
                                  
                          
                                                                              
                                                          
                                                                              
                                                      
                                                  
                                                                        
                                              
                                                                            
                                                                       
                      
                                                                          
                                                        
                                                                             
                                             

                                                                              
                                                                               
                                           
                                                                       
                                                                              
                                                                                
                                                                             
                                                
                                                                            
                                                                          
                                                                           
                    
             
                                                                
                                                               
                                                                           
*/


#include "wlan_nv_types.h"

int allocNvBinTable(int tblIdx, int numEntries);
int allocNvBinEnum(int enumIdx, int numEntries);

typedef enum _nvFieldSizeEnums {
   INDEX_ENUM_ALL = 0,
   INDEX_ENUM_NUM_PHY_MAX_TX_CHAINS,
   INDEX_ENUM_NUM_REG_DOMAINS,
   INDEX_ENUM_NUM_RF_SUBBANDS,
   INDEX_ENUM_NUM_RF_CHANNELS,
   INDEX_ENUM_NUM_2_4GHZ_CHANNELS,
   INDEX_ENUM_NUM_802_11_MODES,
   INDEX_ENUM_NUM_HAL_PHY_RATES,
   INDEX_ENUM_BUILTIN_LAST,
   INDEX_ENUM_BUILTIN_MAX,
} _NV_FIELD_SIZE_ENUMS;

#define INDEX_ENUM_MAX  MAX(INDEX_ENUM_PREDEFINED_MAX, INDEX_ENUM_BUILTIN_MAX)

int getEnumNoOfFields(int enumIdx);
extern _NV_TEMPLATE_ENUM NvEnumsBuiltIn[/*              */][ENUM_ENTRIES_MAX];

typedef enum _nvTableIDs {
   TABLE_sHalNv,
   TABLE_sNvFields,
   TABLE_sRegulatoryChannel,
   TABLE_sRssiChannelOffsets,
   TABLE_sCalData,
   TABLE_sTxBbFilterMode,
   TABLE_sOfdmCmdPwrOffset,
   TABLE_sDefaultCountry,
   TABLE_sFwConfig,
   TABLE_tTpcPowerTable,
   TABLE_tRateGroupPwr,
   TABLE_tRateGroupPwrVR,
   TABLE_sRegulatoryDomains,
   TABLE_sHwCalValues,
   TABLE_sNvTables,
   TABLE_BUILTIN_LAST,
   TABLE_BUILTIN_MAX,
} _NV_TABLE_ID;

#define TABLES_MAX    MAX(TABLE_PREDEFINED_MAX, TABLE_BUILTIN_MAX)

int getTableNoOfFields(int tblIdx);
extern _NV_TEMPLATE_TABLE NvTablesBuiltIn[/*          */][TABLE_ENTRIES_MAX];

#endif //                                        

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

#if !defined( __VOS_NVITEM_H )
#define __VOS_NVITEM_H

/**=========================================================================

  \file  vos_nvitem.h

  \brief virtual Operating System Services (vOSS): Non-Volatile storage API

   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.

   Qualcomm Confidential and Proprietary.

  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */
#include "vos_types.h"
#include "vos_status.h"
#include "wlan_nv.h"
#include "wlan_nv2.h"

/*                                                                          
                                        
                                                                          */
//                                                   
//                                                    
//                                            
//                                            

/*
                                                                  
                                                              
  
                                                                        
                                                                        
                                                                       
                             
  
                                                                    
  
                                                                    
  
                                                                     
  
                       
  
                                                                              
                                                          
  
                                           
  
                                 
 */
#define VNV_ITEM_TABLE \
ADD_VNV_ITEM( VNV_REGULARTORY_DOMAIN_TABLE, REGDOMAIN_COUNT, 144, \
      NV_WLAN_REGULATORY_DOMAIN_FCC_I ) \
ADD_VNV_ITEM( VNV_FIELD_IMAGE, 1, 52, NV_WLAN_FIELD_IMAGE_I ) \
ADD_VNV_ITEM( VNV_RATE_TO_POWER_TABLE, 2, 66, NV_WLAN_RATE_TO_POWER_LIST_I )\
ADD_VNV_ITEM( VNV_DEFAULT_LOCATION, 1, 4, NV_WLAN_DEFAULT_LOCATION_INFO_I ) \
ADD_VNV_ITEM( VNV_TPC_POWER_TABLE, 14, 128, NV_WLAN_TPC_POWER_TABLE_I ) \
ADD_VNV_ITEM( VNV_TPC_PDADC_OFFSETS, 14, 2, NV_WLAN_TPC_PDADC_OFFSETS_I ) \
ADD_VNV_ITEM( VNV_MAXIMUM_TX_POWER, 1, 1, NV_WLAN_MAX_TX_POWER_I ) \
ADD_VNV_ITEM( VNV_RX_SENSITIVITY, 1, MAC_RATE_COUNT, NV_WLAN_RX_SENSITIVITY_I)\
ADD_VNV_ITEM( VNV_NETWORK_TYPE, 1, 1, NV_WLAN_NETWORK_TYPE_I ) \
ADD_VNV_ITEM( VNV_CAL_MEMORY, 1, 3460, NV_WLAN_CAL_MEMORY_I ) \
ADD_VNV_ITEM( VNV_FW_CONFIG, 1, 32, NV_WLAN_FW_CONFIG_I ) \
ADD_VNV_ITEM( VNV_RSSI_CHANNEL_OFFSETS, 2, 56, NV_WLAN_RSSI_CHANNEL_OFFSETS_I ) \
ADD_VNV_ITEM( VNV_HW_CAL_VALUES, 1, 48, NV_WLAN_HW_CAL_VALUES_I ) \
ADD_VNV_ITEM( VNV_ANTENNA_PATH_LOSS, 14, 2, NV_WLAN_ANTENNA_PATH_LOSS_I ) \
ADD_VNV_ITEM( VNV_PACKET_TYPE_POWER_LIMITS, 42, 2, NV_WLAN_PACKET_TYPE_POWER_LIMITS_I ) \
ADD_VNV_ITEM( VNV_OFDM_CMD_PWR_OFFSET, 1, 2, NV_WLAN_OFDM_CMD_PWR_OFFSET_I ) \
ADD_VNV_ITEM( VNV_TX_BB_FILTER_MODE, 1, 4, NV_TX_BB_FILTER_MODE_I ) \
ADD_VNV_ITEM( VNV_FREQUENCY_FOR_1_3V_SUPPLY, 1, 4, NV_FREQUENCY_FOR_1_3V_SUPPLY_I ) \
ADD_VNV_ITEM( VNV_TABLE_VIRTUAL_RATE, 1, 4, VNV_TABLE_VIRTUAL_RATE_I ) \


#define VOS_COUNTRY_CODE_LEN  2
#define VOS_MAC_ADDRESS_LEN   6
#define VOS_NV_FREQUENCY_FOR_1_3V_SUPPLY_3P2MH 0   //       
#define VOS_NV_FREQUENCY_FOR_1_3V_SUPPLY_1P6MH 1   //       


/* 
                                                                              
                                                                                 
                                                         
 */
#define CLPC_PROVISION_DATA L"WLAN_CLPC.PROVISION"

/*                                                                          
                   
                                                                          */
//                                   
typedef enum
{
   //    
   MAC_RATE_11B_1_MBPS,
   MAC_RATE_11B_2_MBPS,
   MAC_RATE_11B_5_5_MBPS,
   MAC_RATE_11B_11_MBPS,

   //    
   MAC_RATE_11G_6_MBPS,
   MAC_RATE_11G_9_MBPS,
   MAC_RATE_11G_12_MBPS,
   MAC_RATE_11G_18_MBPS,
   MAC_RATE_11G_24_MBPS,
   MAC_RATE_11G_36_MBPS,
   MAC_RATE_11G_48_MBPS,
   MAC_RATE_11G_54_MBPS,

   //    
   MAC_RATE_11N_MCS_0,
   MAC_RATE_11N_MCS_1,
   MAC_RATE_11N_MCS_2,
   MAC_RATE_11N_MCS_3,
   MAC_RATE_11N_MCS_4,
   MAC_RATE_11N_MCS_5,
   MAC_RATE_11N_MCS_6,
   MAC_RATE_11N_MCS_7,

   MAC_RATE_COUNT

} v_MAC_RATE_t;

//                                    
typedef enum
{
   REGDOMAIN_FCC,
   REGDOMAIN_ETSI,
   REGDOMAIN_JAPAN,
   REGDOMAIN_WORLD,
   REGDOMAIN_N_AMER_EXC_FCC,
   REGDOMAIN_APAC,
   REGDOMAIN_KOREA,
   REGDOMAIN_HI_5GHZ,
   REGDOMAIN_NO_5GHZ,
   //                               
   REGDOMAIN_COUNT
}
v_REGDOMAIN_t;

typedef enum
{
   COUNTRY_NV,
   COUNTRY_IE,
   COUNTRY_USER,
   COUNTRY_CELL_BASE,
   //                    
   COUNTRY_QUERY,
   COUNTRY_MAX = COUNTRY_QUERY
}
v_CountryInfoSource_t;

//                  
typedef enum
{
   E_NV_V2,
   E_NV_V3,
   E_NV_INVALID
} eNvVersionType;

//                                   
typedef enum
{
#define ADD_VNV_ITEM(_name, _cnt, _size, _label) _name,
   VNV_ITEM_TABLE
#undef ADD_VNV_ITEM
   VNV_TYPE_COUNT
}
VNV_TYPE;

//                  
typedef v_U8_t v_COUNTRYCODE_t[VOS_COUNTRY_CODE_LEN];

//                 
typedef v_U8_t v_MAC_ADDRESS_t[VOS_MAC_ADDRESS_LEN];

/*                                                                         
                                        
                                                                          */
/*                                                                         

                                                 

                                                                       
                                                          

                                                                 
                                                
     

                                                                           */
VOS_STATUS vos_nv_init(void);

/*                                                                         

                                                                            
                                  

                                                                              
                                                                       
                          

                                                   

                                   

                                             

                                                                               
                                                    
                                                          
                                                                          

     

                                                                           */
VOS_STATUS vos_nv_getRegDomainFromCountryCode( v_REGDOMAIN_t *pRegDomain,
      const v_COUNTRYCODE_t countryCode, v_CountryInfoSource_t source);

/*                                                                         

                                                                     
               

                                                                       
                                                    

                                                                      
                                                                        
                                                                          
                                                

                                                                 
                                                                             

                                                                     
                                                                           
                                                                   
                                         
     

                                                                           */
VOS_STATUS vos_nv_getSupportedCountryCode( v_BYTE_t *pBuffer, v_SIZE_t *pBufferSize,
      v_SIZE_t paddingSize );

/*                                                                         

                                                               

                                                                        
                                              
                                                                           
                                                                

                                   

                                                                   

                                                           
                                                               
                                              
     

                                                                           */
VOS_STATUS vos_nv_setValidity( VNV_TYPE type, v_BOOL_t itemIsValid );

/*                                                                         

                                                               

                                                                    
                                              
                                                                           
                                                                

                                   

                                                                         
                               

                                                                  
                                                               
                                              
     

                                                                           */
VOS_STATUS vos_nv_getValidity( VNV_TYPE type, v_BOOL_t *pItemIsValid );

/*                                                                         

                                                           

                                                                           
                                                                      
                                                         

                                                                       
                                                          
                                                            

                             

                                       

                                        

                                         

                                                           
                                                               
                                                          
                                                      
                                              
     

                                                                           */
VOS_STATUS vos_nv_read( VNV_TYPE type, v_VOID_t *outputBuffer,
      v_VOID_t *defaultBuffer, v_SIZE_t bufferSize );

/*                                                                         

                                                                          
        

                                                                        
                                                                            
                                                                         

                                                                       
                                                          
                                                            

                             

                               

                                       

                                        

                                         

                                                           
                                                               
                                                          
                                                      
                                              
     

                                                                           */
VOS_STATUS vos_nv_readAtIndex( VNV_TYPE type, v_UINT_t index,
      v_VOID_t *outputBuffer, v_VOID_t *defaultBuffer, v_SIZE_t bufferSize );

/*                                                                         

                                                                 

                                                                            
                                                            

                            

                                   

                                            

                                                           
                                                               
                                                           
                                                      
                                                
     

                                                                           */
VOS_STATUS vos_nv_write( VNV_TYPE type, v_VOID_t *inputBuffer,
       v_SIZE_t inputBufferSize );

/*                                                                         

                                                                          
              

                                                                         
                                                                          
                                                                            
                                

                            

                               

                                   

                                            

                                                           
                                                               
                                                           
                                                      
                                                
     

                                                                           */
VOS_STATUS vos_nv_writeAtIndex( VNV_TYPE type, v_UINT_t index,
      v_VOID_t *inputBuffer, v_SIZE_t inputBufferSize );

/*                                                                         

                                                                      

                                                                     

                            

                                             

     

                                                                           */
VOS_INLINE_FN v_SIZE_t vos_nv_getElementCount( VNV_TYPE type )
{
   switch (type)
   {
#define ADD_VNV_ITEM(_name, _cnt, _size, _label) case (_name): return (_cnt);
      VNV_ITEM_TABLE
#undef ADD_VNV_ITEM
      default:
         return 0;
   }
}

/*                                                                         

                                                              

                                                              

                            

                                            

     

                                                                           */
VOS_INLINE_FN v_SIZE_t vos_nv_getElementSize( VNV_TYPE type )
{
   switch (type)
   {
#define ADD_VNV_ITEM(_name, _cnt, _size, _label) case (_name): return (_size);
      VNV_ITEM_TABLE
#undef ADD_VNV_ITEM
      default:
         return 0;
   }
}

/*                                                                         

                                                        

                                                        

                            

                                                               

     

                                                                           */
VOS_INLINE_FN v_SIZE_t vos_nv_getItemSize( VNV_TYPE type )
{
   return vos_nv_getElementCount(type) * vos_nv_getElementSize(type);
}

//                                                                    
//                                              

/*                                                                         

                                                                  

                                          

                                         

     

                                                                           */
VOS_STATUS vos_nv_readTxAntennaCount( v_U8_t *pTxAntennaCount );

/*                                                                         

                                                                  

                                          

                                         

     

                                                                           */
VOS_STATUS vos_nv_readRxAntennaCount( v_U8_t *pRxAntennaCount );

/*                                                                         

                                                         

                                  

                                         

     

                                                                           */
VOS_STATUS vos_nv_readMacAddress( v_MAC_ADDRESS_t pMacAddress );

/*                                                                         

                                                                         

                                  
                                                                     

                                         

     

                                                                           */
VOS_STATUS vos_nv_readMultiMacAddress( v_U8_t* pMacAddr, v_U8_t  macCount);

/*                                                                         
                                                                            
                                   
     
                                                                           */
v_REGDOMAIN_t vos_nv_getDefaultRegDomain( void );

/*                                                                         
                                                                       
                            
                                                 
                                                      
                                                 
                                                      
                                         
                                      
     
                                                                           */
VOS_STATUS vos_nv_getSupportedChannels( v_U8_t *p20MhzChannels, int *pNum20MhzChannels,
                                        v_U8_t *p40MhzChannels, int *pNum40MhzChannels);

/*                                                                         
                                                                            
                                                                          
                                         
     
                                                                           */
VOS_STATUS vos_nv_readDefaultCountryTable( uNvTables *tableData );

/*                                                                         
                                                                          
                                                           
                                                 
                                                           
                                                 
                                                           
                                         
                                      
     
                                                                           */
VOS_STATUS vos_nv_getChannelListWithPower(tChannelListWithPower *pChannels20MHz /*                        */,
                                          tANI_U8 *pNum20MHzChannelsFound,
                                          tChannelListWithPower *pChannels40MHz /*                         */,
                                          tANI_U8 *pNum40MHzChannelsFound
                                          );

/*                                                                         

                                                 

                                                                                
                                                                                     

                                                                 
                                                
     

                                                                           */
VOS_STATUS vos_nv_open(void);

/*                                                                         

                                                    

                                                                                     
                               

                                                                 
                                                
     

                                                                           */

VOS_STATUS vos_nv_close(void);

/*                                                                         
                             
                                                
                                
                                         
     
                                                                           */
VOS_STATUS vos_nv_getNVBuffer(v_VOID_t **pNvBuffer, v_SIZE_t *pSize);

/*                                                                         
                                    
                                                
                                
                                         
     
                                                                           */
VOS_STATUS vos_nv_getNVEncodedBuffer(v_VOID_t **pNvBuffer, v_SIZE_t *pSize);


/*                                                                         
                                 
                                                
                                
                                         
     
                                                                           */
VOS_STATUS vos_nv_getNVDictionary(v_VOID_t **pNvBuffer, v_SIZE_t *pSize);

/*                                                                         
                                                          

                                                
                                   
     
                                                                           */
VOS_STATUS vos_nv_isEmbeddedNV(void);

/*                                                                         
                                                        

                                                                         
                                                   
     
                                                                           */
VOS_STATUS vos_nv_setNVEncodedBuffer(v_U8_t *pNvBuffer, v_SIZE_t size);

/*                                                                         
                                                            

                                                                      
                                                       
     
                                                                           */
VOS_STATUS vos_nv_get_dictionary_data(void);

/*                                                                         
                              
                                                         
                                           
                                                 
                                         
     
                                                                           */
VOS_STATUS vos_nv_setRegDomain(void * clientCtxt, v_REGDOMAIN_t regId,
                                                  v_BOOL_t sendRegHint);

/*                                                                         
                                        
                                                                
                                                         
                      
                       
                  
     
                                                                           */

eNVChannelEnabledType vos_nv_getChannelEnabledState
(
   v_U32_t    rfChannel
);

VOS_STATUS vos_init_wiphy_from_nv_bin(void);

/*                                                                         
                              
             
                                       
                      
                      
                           
     
                                                                           */
eNvVersionType vos_nv_getNvVersion
(
   void
);


/*                                                                         
                           
                                                           
                           
     
                                                                           */
v_U16_t vos_chan_to_freq(v_U8_t chanNum);
#endif //               

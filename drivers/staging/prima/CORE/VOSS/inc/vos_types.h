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
#if !defined( __VOS_TYPES_H )
#define __VOS_TYPES_H

/**=========================================================================
  \file  vos_Types.h

  \brief virtual Operating System Servies (vOS)

   Basic type definitions

   Copyright 2008 (c) Qualcomm Technologies, Inc.
   All Rights Reserved.
   Qualcomm Technologies Confidential and Proprietary.
  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */
#include "i_vos_types.h"
#include <string.h>

/*                                                                           
                                        
                                                                          */
//                                    
#define VOS_MAX( _x, _y ) ( ( (_x) > (_y) ) ? (_x) : (_y) )  

//                                   
#define VOS_MIN( _x, _y ) ( ( (_x) < (_y) ) ? (_x) : (_y)  )  

//                                                             
#define VOS_CEIL_DIV( _a, _b ) (( 0 != (_a) % (_b) ) ? ( (_a) / (_b) + 1 ) : ( (_a) / (_b) ))

//                                                           
#define VOS_FLOOR_DIV( _a, _b ) ( ( (_a) - ( (_a) % (_b) ) ) / (_b) )

#define VOS_SWAP_U16(_x) \
   ( ( ( (_x) << 8 ) & 0xFF00 ) | ( ( (_x) >> 8 ) & 0x00FF ) )

#define VOS_SWAP_U32(_x) \
  (( ( ( (_x) << 24 ) & 0xFF000000 ) | ( ( (_x) >> 24 ) & 0x000000FF ) ) | \
   ( ( ( (_x) << 8 ) & 0x00FF0000 ) | ( ( (_x) >> 8 ) & 0x0000FF00 ) ))

//                                                        
#ifdef ANI_LITTLE_BYTE_ENDIAN

#define vos_cpu_to_be32(_x) VOS_SWAP_U32(_x)
#define vos_be32_to_cpu(_x) VOS_SWAP_U32(_x)
#define vos_cpu_to_be16(_x) VOS_SWAP_U16(_x)
#define vos_be16_to_cpu(_x) VOS_SWAP_U16(_x)
#define vos_cpu_to_le32(_x) (_x)
#define vos_le32_to_cpu(_x) (_x)
#define vos_cpu_to_le16(_x) (_x)
#define vos_le16_to_cpu(_x) (_x)

#endif

#ifdef ANI_BIG_BYTE_ENDIAN

#define vos_cpu_to_be32(_x) (_x)
#define vos_be32_to_cpu(_x) (_x)
#define vos_cpu_to_be16(_x) (_x)
#define vos_be16_to_cpu(_x) (_x)
#define vos_cpu_to_le32(_x) VOS_SWAP_U32(_x)
#define vos_le32_to_cpu(_x) VOS_SWAP_U32(_x)
#define vos_cpu_to_le16(_x) VOS_SWAP_U16(_x)
#define vos_le16_to_cpu(_x) VOS_SWAP_U16(_x)

#endif


/*                                                                          
                   
                                                                          */

//                                                                      
//                         
typedef enum
{
   VOS_MODULE_ID_BAP        = 0,
   VOS_MODULE_ID_TL         = 1,
   VOS_MODULE_ID_WDI        = 2,
   //                                         
   VOS_MODULE_ID_HDD        = 5,
   VOS_MODULE_ID_SME        = 6,
   VOS_MODULE_ID_PE         = 7,
   VOS_MODULE_ID_WDA        = 8,
   VOS_MODULE_ID_SYS        = 9,
   VOS_MODULE_ID_VOSS       = 10,
   VOS_MODULE_ID_SAP        = 11,
   VOS_MODULE_ID_HDD_SOFTAP = 12,
   VOS_MODULE_ID_PMC        = 13,

   //                                                             
   //                                                            
   //                                                               
   VOS_MODULE_ID_MAX

} VOS_MODULE_ID;


//                                                                          
//                         
typedef enum
{ /*                                                             */
    VOS_STA_MODE=0, 
    VOS_STA_SAP_MODE=1, //                                                                     
                        //                                       
    VOS_P2P_CLIENT_MODE,
    VOS_P2P_GO_MODE,
    VOS_MONITOR_MODE,
    VOS_FTM_MODE = 5,
    VOS_IBSS_MODE,
    VOS_P2P_DEVICE,
    VOS_MAX_NO_OF_MODE
} tVOS_CON_MODE;

//                                             
//                
//               
//                       
//                   
typedef enum
{
    VOS_STA=1, 
    VOS_SAP=2,
    VOS_STA_SAP=3, //                                                     
    VOS_P2P_CLIENT=4,
    VOS_P2P_GO=8,
    VOS_MAX_CONCURRENCY_PERSONA=4
} tVOS_CONCURRENCY_MODE;
 
#if !defined( NULL )
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

enum
{
   VOS_FALSE = 0, 
   VOS_TRUE  = ( !VOS_FALSE )
};

//                       
typedef v_VOID_t *v_PVOID_t;

//                 
typedef v_UINT_t v_SIZE_t;

//             
typedef v_ULONG_t v_TIME_t;

//                            
typedef v_VOID_t *v_CONTEXT_t;


//                                                              
//                             
//                                             
#define VOS_MAC_ADDR_SIZE ( 6 )

typedef struct
{
   //                                          
   v_BYTE_t bytes[ VOS_MAC_ADDR_SIZE ];
    
} v_MACADDR_t;


//                                                            
//                                                
//                                                              
#define VOS_MAC_ADDR_BROADCAST_INITIALIZER { { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } }

//                                                            
//                         
//                                                         
#define VOS_MAC_ADDR_ZERO_INITIALIZER { { 0, 0, 0, 0, 0, 0 } }



/*                                                                            
  
                                                             

                                                                       
                 
  
                                                              
                                                                    
  
                                            
                                                    
  
     
    
                                                                            */
VOS_INLINE_FN v_BOOL_t vos_is_macaddr_equal( v_MACADDR_t *pMacAddr1, 
                                             v_MACADDR_t *pMacAddr2 )
{
   return ( 0 == memcmp( pMacAddr1, pMacAddr2, VOS_MAC_ADDR_SIZE ) );
}



/*                                                                            
  
                                                                      

                                                                          
            
  
                                                        
  
                                             
                                                      
    
     
  
                                                                            */
VOS_INLINE_FN v_BOOL_t vos_is_macaddr_zero( v_MACADDR_t *pMacAddr )
{
   v_MACADDR_t zeroMacAddr = VOS_MAC_ADDR_ZERO_INITIALIZER;
   
   return( vos_is_macaddr_equal( pMacAddr, &zeroMacAddr ) );
}


/*                                                                            
  
                                                   

                                                 
  
                                                       
  
                  
    
     
  
                                                                            */
VOS_INLINE_FN v_VOID_t vos_zero_macaddr( v_MACADDR_t *pMacAddr )
{
   memset( pMacAddr, 0, VOS_MAC_ADDR_SIZE );
}


/*                                                                            
  
                                                                              

                                                                           
                                                                            
                                                                      
                      
  
                                                            
  
                                                         
                                                                 
  
     
    
                                                                            */
VOS_INLINE_FN v_BOOL_t vos_is_macaddr_group( v_MACADDR_t *pMacAddr )
{
    return( pMacAddr->bytes[ 0 ] & 0x01 );
}


/*                                                                            
  
                                                                                    

                                                                           
                           
  
                                                           
  
                                                             
                                                                     
  
     
    
                                                                            */
VOS_INLINE_FN v_BOOL_t vos_is_macaddr_broadcast( v_MACADDR_t *pMacAddr )
{
   v_MACADDR_t broadcastMacAddr = VOS_MAC_ADDR_BROADCAST_INITIALIZER;
   
   return( vos_is_macaddr_equal( pMacAddr, &broadcastMacAddr ) );
}

/*                                                                            
  
                                                                                    

                                                                           
                           
  
                                                           
  
                                                             
                                                                     
  
     
    
                                                                            */
VOS_INLINE_FN v_BOOL_t vos_is_macaddr_multicast( v_MACADDR_t *pMacAddr )
{
   return( vos_is_macaddr_group( pMacAddr ) &&
          !vos_is_macaddr_broadcast( pMacAddr ) );
}



/*                                                                            
  
                                                                                  

                                                                           
                          
  
                                                           
  
                                                            
                                                                    
  
     
    
                                                                            */
VOS_INLINE_FN v_BOOL_t vos_is_macaddr_directed( v_MACADDR_t *pMacAddr )
{
    return( !vos_is_macaddr_group( pMacAddr ) );
}

/*                                                                            
  
                                                    

                                                                      
  
                                                                           
                                                                        
  
                  
    
     
    
                                                                            */
VOS_INLINE_FN v_VOID_t vos_copy_macaddr( v_MACADDR_t *pDst, v_MACADDR_t *pSrc )
{
    *pDst = *pSrc;
}    


/*                                                                            
  
                                                                                
                                       
                                                                               
                                     
  
                                                                      
  
                  
    
     
    
                                                                            */
VOS_INLINE_FN v_VOID_t vos_set_macaddr_broadcast( v_MACADDR_t *pMacAddr )
{
   memset( pMacAddr, 0xff, VOS_MAC_ADDR_SIZE );
}

/*                                                                            
  
                                                              
  
                                                 
  
                                                          
  
                                                                          
                               
    
                                                            
  
                                                                            */                                                 
v_U32_t vos_atomic_set_U32( v_U32_t *pTarget, v_U32_t value );


//                                                                   
//                                                     
VOS_INLINE_FN v_U8_t vos_atomic_set_U8( v_U8_t *pVariable, v_U8_t value )
{
  if (pVariable == NULL)
  {
    return 0;
  }
  *pVariable = value;
  return value;
}

/*                                                                            
  
                                                                          
  
                                                       
  
                                                                    
                           
    
                                                      
  
                                                                            */                                                 
v_U32_t vos_atomic_increment_U32( v_U32_t *pTarget );


/*                                                                            
  
                                                                          
  
                                                       
  
                                                                    
                           
    
                                                      
  
                                                                            */                                                 
v_U32_t vos_atomic_decrement_U32( v_U32_t *pTarget );

/*                                                                            
  
                                                                                  
                  
  
                                                       
                                                             
  
                                                                    
                           
    
                                                      
  
                                                                            */                                                 
v_U32_t vos_atomic_increment_U32_by_value( v_U32_t *pTarget, v_U32_t value  );

/*                                                                            
  
                                                                                  
                  
  
                                                       
                                                                     
  
                                                                    
                           
    
                                                      
  
                                                                            */                                                 
v_U32_t vos_atomic_decrement_U32_by_value( v_U32_t *pTarget, v_U32_t value  );


v_U32_t vos_get_skip_ssid_check(void); 

v_U32_t vos_get_skip_11e_check(void); 



#endif //                           

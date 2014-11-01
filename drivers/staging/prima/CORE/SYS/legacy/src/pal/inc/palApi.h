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

/** ------------------------------------------------------------------------- *
    ------------------------------------------------------------------------- *


    \file palApi.h

    \brief Exports and types for the Platform Abstraction Layer interfaces.

    $Id$

    Copyright (C) 2006 Airgo Networks, Incorporated
    This file contains all the interfaces for thge Platform Abstration Layer
    functions.  It is intended to be included in all modules that are using
    the PAL interfaces.

   ========================================================================== */
#ifndef PALAPI_H__
#define PALAPI_H__

#include "halTypes.h"

/* 
                                              

                               

                                            

                                                                             
                                                                             
                               
                                                                          
                                                                           
                                                                    

         
                                     
                                                                                               
                                                                              
                                                             
                                  
                                                   
                                                                                
                                                                                             
                                                        


                                            

                                                      

 */


/*                                                                             

                       

                                                                 

                                    

                                                                                  
                      

                                                                                    

                                                                              
                                                                                 
              

                                                                                 */
eHalStatus palReadRegister( tHddHandle hHdd, tANI_U32 regAddress, tANI_U32 *pRegValue );


/*                                                                             

                        

                                                                  

                                    

                                                                                  
                      

                                                                           

                                                                              
                                                                                 
              

                                                                                 */
eHalStatus palWriteRegister( tHddHandle hHdd, tANI_U32 regAddress, tANI_U32 regValue );

/*                                                                             

                             

                                                                        

                                    

                                                                                     
                      

                                                           

                                                                               
                                                                                 
              

                                                                                 */

eHalStatus palAsyncWriteRegister( tHddHandle hHdd, tANI_U32 regAddress, tANI_U32 regValue );


/*                                                                             

                           

                                                                     

                                    

                                                                         
                                                                           

                                                                                
                                               

                                                     

                                                                            
                                                                                 
              

                                                                                 */
eHalStatus palReadDeviceMemory( tHddHandle hHdd, tANI_U32 memOffset, tANI_U8 *pBuffer, tANI_U32 numBytes );

/*                                                                             

                            

                                                                    

                                    

                                                                                 
                                

                                                                              
                        

                                                        

                                                                            
                                                                                 
              

                                                                                 */
eHalStatus palWriteDeviceMemory( tHddHandle hHdd, tANI_U32 memOffset, tANI_U8 *pBuffer, tANI_U32 numBytes );


/*                                                                             

                         

                                                       

                                                                       
                                                                   
                                                    

                                    

                                                                        
                                                                   

                                                      

                                                                              
                                                                             
                                                                                 
                                                     

                                                                                 */
#ifndef FEATURE_WLAN_PAL_MEM_DISABLE

#ifdef MEMORY_DEBUG
#define palAllocateMemory(hHdd, ppMemory, numBytes) palAllocateMemory_debug(hHdd, ppMemory, numBytes, __FILE__, __LINE__)
eHalStatus palAllocateMemory_debug( tHddHandle hHdd, void **ppMemory, tANI_U32 numBytes, char* fileName, tANI_U32 lineNum );
#else
eHalStatus palAllocateMemory( tHddHandle hHdd, void **ppMemory, tANI_U32 numBytes );
#endif


/*                                                                             

                     

                                                                          
                              

                                                                       
                                                                   
                                                    

                                    

                                                           

                                                                              
                                                                             
                                                                                 
                                                     

                                                                                 */
eHalStatus palFreeMemory( tHddHandle hHdd, void *pMemory );



/*                                                                             

                     

                                                                              

                                    

                                                           

                                                       

                                                                      

                                                                              
                                                                             
                                                                                 
                                                     

                                                                                 */
eHalStatus palFillMemory( tHddHandle hHdd, void *pMemory, tANI_U32 numBytes, tANI_BYTE fillValue );

/*                                                                             

                     

                                                                               

                                    

                                                                  

                                                                     

                                                          

                                                  

                                                                                 */
eHalStatus palCopyMemory( tHddHandle hHdd, void *pDst, const void *pSrc, tANI_U32 numBytes );

/*                                                                             

                     

                                                                              

                                    

                                                           

                                                       

                                                                      

                                                                              
                                                                             
                                                                                 
                                                     

                                                                                 */
ANI_INLINE_FUNCTION
eHalStatus palZeroMemory( tHddHandle hHdd, void *pMemory, tANI_U32 numBytes )
{
    return( palFillMemory( hHdd, pMemory, numBytes, 0 ) );
}


/*                                                                             

                      

                                                                          
                                  

                                    

                                                                   

                                                                      

                                                     

                                                                           
                                     

                                                                                 */
tANI_BOOLEAN palEqualMemory( tHddHandle hHdd, void *pMemory1, void *pMemory2, tANI_U32 numBytes );
#endif
/*                                                                             

                           

                                                                     
               

                                    

                                                                  

                                                       

                                                                         

                                                                              
             

                                                                               
                          

                                                                             
                    

                                                                        
                                                                    

                                                                                 */
eHalStatus palFillDeviceMemory( tHddHandle hHdd, tANI_U32 memOffset, tANI_U32 numBytes, tANI_BYTE fillValue );


/*                                                                             

                           

                                                                                

                                    

                                                                  

                                                       

                                                                 

                                                                              
             

                                                                               
                          

                                                                             
                    

                                                                        
                                                                    

                                                                                 */
ANI_INLINE_FUNCTION
eHalStatus palZeroDeviceMemory( tHddHandle hHdd, tANI_U32 memOffset, tANI_U32 numBytes )
{
    return( palFillDeviceMemory( hHdd, memOffset, numBytes, 0 ) );
}

/*                                                                                  

                                                      

                                    

                               

               

                 

                      

                        
                                                                                  */
eHalStatus palPktAlloc(tHddHandle hHdd, eFrameType frmType, tANI_U16 size, void **data, void **ppPacket) ;


//                                             
void palPktFree( tHddHandle hHdd, eFrameType frmType, void* buf, void *pPacket);



//                  
//                                                                  
eHalStatus palSpinLockAlloc( tHddHandle hHdd, tPalSpinLockHandle *pHandle );
//                                                  
eHalStatus palSpinLockFree( tHddHandle hHdd, tPalSpinLockHandle hSpinLock );
//                                                  
eHalStatus palSpinLockTake( tHddHandle hHdd, tPalSpinLockHandle hSpinLock );
//                                                  
eHalStatus palSpinLockGive( tHddHandle hHdd, tPalSpinLockHandle hSpinLock );
//                      


//                                    
//                                                                                         
//                                                       
//                                                                    
eHalStatus palSendMBMessage(tHddHandle hHdd, void *pBuf);

extern void palGetUnicastStats(tHddHandle hHdd, tANI_U32 *tx, tANI_U32 *rx);


/*                                                                                  
                                                                                       
                    

                                    

                       
                                                                                  */
tANI_U32 palGetTickCount(tHddHandle hHdd);

/*                                                                             

                        

                                                                                              
                                                                     

                                    

                                                                         
                                                                           

                                                                                
                                               

                                                     

                                                                            
                                                                                 
              

                                                                                 */
eHalStatus palReadRegMemory( tHddHandle hHdd, tANI_U32 memOffset, tANI_U8 *pBuffer, tANI_U32 numBytes );

/*                                                                             

                              

                                                                                             
                                                                                              
                                      

                                    

                                                                                 
                                

                                                                              
                        

                                                        

                                                                            
                                                                                 
              

                                                                                 */
eHalStatus palAsyncWriteRegMemory( tHddHandle hHdd, tANI_U32 memOffset, tANI_U8 *pBuffer, tANI_U32 numBytes );

/*                                                                             

                         
                                                                                             
                                                                                               
                                                                                              
                                                 

                                    

                                                                                 
                                

                                                                              
                        

                                                        

                                                                            
                                                                                 
              

                                                                                 */
eHalStatus palWriteRegMemory( tHddHandle hHdd, tANI_U32 memOffset, tANI_U8 *pBuffer, tANI_U32 numBytes );


/*                                                                             

                     

                                                                                                       
                                                                                                          

                                    

                                                   

                                                       

                                                                                 

                                                                                 

                                                           

                                                         

                                                                            
                                                                                 
              

                                                                                 */
eHalStatus palWaitRegVal( tHddHandle hHdd, tANI_U32 reg, tANI_U32 mask,
                             tANI_U32 waitRegVal, tANI_U32 perIterWaitInNanoSec,
                             tANI_U32 numIter, tANI_U32 *pReadRegVal );

/*                                                                             

                             

                                                                                                        
                                                 

                                    

                                                        

                                                                

                                                                                     

                                                                            
                                                                                 
              

                                                                                 */
eHalStatus palReadModifyWriteReg( tHddHandle hHdd, tANI_U32 reg, tANI_U32 andMask, tANI_U32 orMask );

//                       
eHalStatus palSemaphoreAlloc( tHddHandle hHdd, tPalSemaphoreHandle *pHandle, tANI_S32 count );
eHalStatus palSemaphoreFree( tHddHandle hHdd, tPalSemaphoreHandle hSemaphore );
eHalStatus palSemaphoreTake( tHddHandle hHdd, tPalSemaphoreHandle hSemaphore );
eHalStatus palSemaphoreGive( tHddHandle hHdd, tPalSemaphoreHandle hSemaphore );
eHalStatus palMutexAlloc( tHddHandle hHdd, tPalSemaphoreHandle *pHandle) ;
eHalStatus palMutexAllocLocked( tHddHandle hHdd, tPalSemaphoreHandle *pHandle) ;

//               
eAniBoolean pal_in_interrupt(void) ;
void pal_local_bh_disable(void) ;
void pal_local_bh_enable(void) ;

//             
tANI_U32 pal_be32_to_cpu(tANI_U32 x) ;
tANI_U32 pal_cpu_to_be32(tANI_U32 x) ;
tANI_U16 pal_be16_to_cpu(tANI_U16 x) ;
tANI_U16 pal_cpu_to_be16(tANI_U16 x) ;


#if defined( ANI_LITTLE_BYTE_ENDIAN )

//                                                                   
ANI_INLINE_FUNCTION unsigned long i_htonl( unsigned long ul )
{
  return( ( ( ul & 0x000000ff ) << 24 ) |
          ( ( ul & 0x0000ff00 ) <<  8 ) |
          ( ( ul & 0x00ff0000 ) >>  8 ) |
          ( ( ul & 0xff000000 ) >> 24 )   );
}

ANI_INLINE_FUNCTION unsigned short i_htons( unsigned short us )
{
  return( ( ( us >> 8 ) & 0x00ff ) + ( ( us << 8 ) & 0xff00 ) );
}

ANI_INLINE_FUNCTION unsigned short i_ntohs( unsigned short us )
{
  return( i_htons( us ) );
}

ANI_INLINE_FUNCTION unsigned long i_ntohl( unsigned long ul )
{
  return( i_htonl( ul ) );
}

#endif //                                     


/*                                                                             

                   

                                                                                 

                                                                                

                                                 

                                                        

                                                                               
                               

                                                                                 */
ANI_INLINE_FUNCTION tANI_U8 * pal_set_U32(tANI_U8 *ptr, tANI_U32 value)
{
#if defined( ANI_BIG_BYTE_ENDIAN )
     *(ptr) = ( tANI_U8 )( value >> 24 );
     *(ptr + 1) = ( tANI_U8 )( value >> 16 );
     *(ptr + 2) = ( tANI_U8 )( value >> 8 );
     *(ptr + 3) = ( tANI_U8 )( value );
#else
    *(ptr + 3) = ( tANI_U8 )( value >> 24 );
    *(ptr + 2) = ( tANI_U8 )( value >> 16 );
    *(ptr + 1) = ( tANI_U8 )( value >> 8 );
    *(ptr) = ( tANI_U8 )( value );
#endif

    return (ptr + 4);
}


/*                                                                             

                   

                                                                                 

                                                                                

                                                 

                                                        

                                                                               
                               

                                                                                 */
ANI_INLINE_FUNCTION tANI_U8 * pal_set_U16(tANI_U8 *ptr, tANI_U16 value)
{
#if defined( ANI_BIG_BYTE_ENDIAN )
     *(ptr) = ( tANI_U8 )( value >> 8 );
     *(ptr + 1) = ( tANI_U8 )( value );
#else
    *(ptr + 1) = ( tANI_U8 )( value >> 8 );
    *(ptr) = ( tANI_U8 )( value );
#endif

    return (ptr + 2);
}


/*                                                                             

                   

                                                                                       

                                                                                

                                                 

                                                                                             
                     

                                                                               
                               

                                                                                 */
ANI_INLINE_FUNCTION tANI_U8 * pal_get_U16(tANI_U8 *ptr, tANI_U16 *pValue)
{
#if defined( ANI_BIG_BYTE_ENDIAN )
    *pValue = (((tANI_U16) (*ptr << 8)) |
            ((tANI_U16) (*(ptr+1))));
#else
    *pValue = (((tANI_U16) (*(ptr+1) << 8)) |
            ((tANI_U16) (*ptr)));
#endif

    return (ptr + 2);
}


/*                                                                             

                   

                                                                                       

                                                                                

                                                 

                                                                                             
                     

                                                                               
                               

                                                                                 */
ANI_INLINE_FUNCTION tANI_U8 * pal_get_U32(tANI_U8 *ptr, tANI_U32 *pValue)
{
#if defined( ANI_BIG_BYTE_ENDIAN )
    *pValue = ( (tANI_U32)(*(ptr) << 24) |
             (tANI_U32)(*(ptr+1) << 16) |
             (tANI_U32)(*(ptr+2) << 8) |
             (tANI_U32)(*(ptr+3)) );
#else
    *pValue = ( (tANI_U32)(*(ptr+3) << 24) |
             (tANI_U32)(*(ptr+2) << 16) |
             (tANI_U32)(*(ptr+1) << 8) |
             (tANI_U32)(*(ptr)) );
#endif

    return (ptr + 4);
}


#endif

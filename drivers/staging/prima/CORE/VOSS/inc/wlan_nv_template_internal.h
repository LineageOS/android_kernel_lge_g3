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
#if !defined _WLAN_NV_TEMPLATE_INTERNAL_H
#define  _WLAN_NV_TEMPLATE_INTERNAL_H

/*
                     
                      
                                                            
                                                 
                                                              
                      
                                                    
                                                  
                                                  
                                                       

                      
                                                                             
                                    
                                                          
                                                                               
                                     
                                                                               
                                                                             
                                         
                                                                       
                                                                           
                                                        
                                                         
                                                                               
                                        
                                                                               
                         
                                                                         
                                
                                                                           
                                                                          
                                                                               
                                                                           
                                                          
                                                                           
                   
                                                                      
                                                                
                                       
                          
                                                                              
                                                          
                                                                              
                                                      
                                                  
                                                                        
                                              
                                                                                
                                                     
                                
                                                                          
                                                      
                                                                             
                                             

                                                                              
                                                                               
                                           
                                                                       
                                                                              
                                                                                
                                                                             
                                                
                                                                            
                                                                      
                                                                    
                              
             
                                                                
                                                               
                                                                           
*/

#include "wlan_nv_types.h"

/*
                       
                                   
                                   
                                              
                                                   
                            
                         
                                
  
 */
/*                                               */
#define STREAM_HEADER_TYPE_MASK   0x80
#define STREAM_HEADER_TYPE_LSB    7
#define IsStreamTemplate(b)       (((b) & (STREAM_HEADER_TYPE_MASK)) ? 1 : 0)

/*                                                      */
#define STREAM_HEADER_TEMPLATE_TYPE_MASK   0x40
#define STREAM_HEADER_TEMPLATE_TYPE_LSB    6
#define IsTemplateStreamTable(b)   (((b) & (STREAM_HEADER_TEMPLATE_TYPE_MASK)) ? 1 : 0)

/*
                          
  
                                  
                           
                                
                                                  
                                             
                                     
                                                           
                     
 */
/*                          
               
                          
            
                                                                               
                                                     
                                                                            
                     
                                                                              
               
 */

#define FIELD_ID_TYPE_MASK                  0x80
#define FIELD_ID_TYPE_LSB                   7
#define IsFieldTypeBasicData(b)             (((b) & (FIELD_ID_TYPE_MASK)) ? 1 : 0)

/*                                    */
#define FIELD_ID_TABLE_OR_ENUM_IDX_MASK     0x7f
#define FIELD_ID_TABLE_OR_ENUM_IDX_LSB      0
#define _TABLE_IDX(b)                       (((b) & ~FIELD_ID_TYPE_MASK) | ((b) & FIELD_ID_TABLE_OR_ENUM_IDX_MASK))
#define IsIdxTableOfAllTables(b)            (((b) & FIELD_ID_TABLE_OR_ENUM_IDX_MASK) ? 0 : 1)
#define IsIdxEnumOfAllEnums(b)              (((b) & FIELD_ID_TABLE_OR_ENUM_IDX_MASK) ? 0 : 1)

/*                                        
            
             
            
             
             
             
 */

#define FIELD_ID_BASIC_DATA_TYPES_MASK      0x7F
#define FIELD_ID_BASIC_DATA_TYPES_LSB       0

typedef enum {
   _FIELD_ID_DATA_TYPE_U8 = 0,
   _FIELD_ID_DATA_TYPE_U32,
   _FIELD_ID_DATA_TYPE_S8,
   _FIELD_ID_DATA_TYPE_S32,
   _FIELD_ID_DATA_TYPE_U16,
   _FIELD_ID_DATA_TYPE_S16,
   _FIELD_ID_DATA_TYPE_LAST,
} _FIELD_ID_BASIC_DATA_TYPE;

#define TheBasicDataType(b)                 (((b) & (FIELD_ID_BASIC_DATA_TYPES_MASK)) >> FIELD_ID_BASIC_DATA_TYPES_LSB)
#define _ID_U8                              ((FIELD_ID_TYPE_MASK) | (_FIELD_ID_DATA_TYPE_U8))
#define _ID_U32                             ((FIELD_ID_TYPE_MASK) | (_FIELD_ID_DATA_TYPE_U32))
#define _ID_S8                              ((FIELD_ID_TYPE_MASK) | (_FIELD_ID_DATA_TYPE_S8))
#define _ID_S32                             ((FIELD_ID_TYPE_MASK) | (_FIELD_ID_DATA_TYPE_S32))
#define _ID_U16                             ((FIELD_ID_TYPE_MASK) | (_FIELD_ID_DATA_TYPE_U16))
#define _ID_S16                             ((FIELD_ID_TYPE_MASK) | (_FIELD_ID_DATA_TYPE_S16))

/*
                      
 */
typedef enum {
   SINGULAR = 0,
   ARRAY_1,
   ARRAY_2,
   ARRAY_3,
   STORAGE_TYPE_LAST,
} _FIELD_ID_STORAGE_TYPE;

#define _STORAGE_TYPE(b)   ((b) & 0x3)
#define _STORAGE_SIZE1(byteLow, byteHigh)   (((((byteHigh) >> 2) & 0x3) << 7) | (((byteLow) >> FIELD_SIZE_VALUE_LSB) & FIELD_SIZE_VALUE_MASK))
#define _STORAGE_SIZE2(byteLow, byteHigh)   (((((byteHigh) >> 4) & 0x3) << 7) | (((byteLow) >> FIELD_SIZE_VALUE_LSB) & FIELD_SIZE_VALUE_MASK))
#define _STORAGE_SIZE3(byteLow, byteHigh)   (((((byteHigh) >> 6) & 0x3) << 7) | (((byteLow) >> FIELD_SIZE_VALUE_LSB) & FIELD_SIZE_VALUE_MASK))

#define _ADD_SIZE1(b)  ((((b) >> 7) & 0x3) << 2)
#define _ADD_SIZE2(b)  ((((b) >> 7) & 0x3) << 4)
#define _ADD_SIZE3(b)  ((((b) >> 7) & 0x3) << 6)

/*
                               
                    
                          
                              
                        
                                               
  
                                                                             
                                            
  
                                 
 */
#define FIELD_SIZE_TYPE_MASK              0x80
#define FIELD_SIZE_TYPE_LSB               7
#define FIELD_SIZE_TYPE_BIT(t)            (((t)<< (FIELD_SIZE_TYPE_LSB)) & (FIELD_SIZE_TYPE_MASK))
#define IsFieldSizeInt(b)                 (((b) & (FIELD_SIZE_TYPE_MASK)) ? 1 : 0)

typedef enum {
   FIELD_SIZE_IDX_ENUM = 0,
   FIELD_SIZE_IDX_INT = 1,
} FIELD_SIZE_TYPE;

#define FIELD_SIZE_VALUE_MASK             0x7f
#define FIELD_SIZE_VALUE_LSB              0
#define FIELD_SIZE_VALUE_BITS(val)        (((val) << (FIELD_SIZE_VALUE_LSB)) & (FIELD_SIZE_VALUE_MASK))

/*
                                           
 */
#define _TABLE_NAME_LEN  2
#define _TABLE_FIELD_FULL_NAME_LEN  47

typedef struct _nvTemplateTableStructInternal {
   tANI_U8   fieldName[_TABLE_NAME_LEN + 1];
   tANI_U8   fieldId;
   tANI_U8   fieldStorageType;
   tANI_U8   fieldStorageSize1;
   tANI_U8   fieldStorageSize2;
   tANI_U8   fieldStorageSize3;
   tANI_U32  offset; //                 
   tANI_U8   fieldFullName[_TABLE_FIELD_FULL_NAME_LEN +1];
} _NV_TEMPLATE_TABLE;

#define _OFFSET_NOT_SET        0xFFFFFFFF
#define TABLE_PREDEFINED_MAX   50
#define TABLE_ENTRIES_MAX      50
#define _LIST_OF_TABLES_IDX     0
#define _TABLE_FIELDS_POS       2
#define _ENUM_START_POS         2
#define _TABLE_FIELD_MIN_LEN    4
#define _ENUM_MIN_LEN           3

#define _ENUM_NAME_LEN _TABLE_NAME_LEN
#define _ENUM_FULL_NAME_LEN    47
typedef struct _nvTemplateEnumStruct {
   tANI_U8   enumName[3];  //              
   tANI_U8   enumValue;
   tANI_U8   enumValuePeer;
   tANI_U8   enumFullName[_ENUM_FULL_NAME_LEN +1];
} _NV_TEMPLATE_ENUM;
#define INDEX_ENUM_PREDEFINED_MAX    20
#define ENUM_ENTRIES_MAX             200

typedef enum {
   _MIS_MATCH = 0,
   _MATCH,
} _NV_TEMPLATE_PROCESS_RC;

#define _NV_BIN_STREAM_HEADER_BYTE          0
#define _NV_BIN_STREAM_TABLE_ID_BYTE        1
#define _NV_BIN_STREAM_ENUM_ID_BYTE         1
#define _NV_BIN_DATA_STREAM_TABLEID_BYTE    1
#define _NV_BIN_ENUM_TEMPLATE_ENTRY_SIZE    3
#define _NV_LIST_OF_TABLE_ID                0

/*
               
 */
#define _STREAM_HEADER_POS            0
#define _ENUM_STREAM_HEADER_POS       _STREAM_HEADER_POS
#define _TABLE_STREAM_HEADER_POS      _STREAM_HEADER_POS
#define _TEMPLATE_INDEX_HEADER_POS    1
#define _ENUM_INDEX_HEADER_POS        _TEMPLATE_INDEX_HEADER_POS
#define _TABLE_INDEX_HEADER_POS       _TEMPLATE_INDEX_HEADER_POS

/*
                     
 */
typedef struct _enumMetaData {
   _NV_TEMPLATE_PROCESS_RC match;
} _ENUM_META_DATA;

#define MAX(a, b)     (((a) > (b)) ? (a) : (b))
#define _NV_STREAM_LEN_MAX           35000

/*
                                                           
 */
typedef enum {
   _OK = 0,
   _RESET_STREAM_FAILED,
   _WRITE_STREAM_FAILED,
   _STREAM_NOT_FIT_BUF,
   _SW_BIN_MISMATCH,
   _INSUFFICIENT_FOR_FIELD_PARSER_ERROR,
   _TABLE_NON_EXIST_IN_TABLE_OF_ALL_TABLES,
   _ENUM_NOT_FOUND_IN_BUILT_IN,
} _ErrorCode;

/*
                           
 */
//                        
#define RESET_STREAM(b)   resetStream(b)
#define NEXT_STREAM(b,c)    nextStream(b,c)

#endif /*                                          */

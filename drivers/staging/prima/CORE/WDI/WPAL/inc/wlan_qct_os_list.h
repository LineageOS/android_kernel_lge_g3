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

#if !defined( __WLAN_QCT_OS_LIST_H )
#define __WLAN_QCT_OS_LIST_H

/**=========================================================================
  
  \file  wlan_qct_pal_list.h
  
  \brief define linked list PAL exports. wpt = (Wlan Pal Type) wpal = (Wlan PAL)
               
   Definitions for platform dependent. It is with VOSS support.
  
   Copyright 2010 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

#include "wlan_qct_pal_type.h"
#include "wlan_qct_pal_status.h"
//                                                                                  
#include "vos_list.h"

typedef vos_list_t wpt_list;
typedef vos_list_node_t wpt_list_node;

#define WPAL_LIST_STATUS_BASIC_CHECK(status) ( VOS_IS_STATUS_SUCCESS(status) ? \
            eWLAN_PAL_STATUS_SUCCESS : eWLAN_PAL_STATUS_E_FAILURE )

#define WPAL_LIST_IS_VOS_STATUS_BUSY(status) (VOS_STATUS_E_BUSY == (status))
#define WPAL_LIST_STATUS_BUSY_CHECK(status) ( VOS_IS_STATUS_SUCCESS(status) ? \
            eWLAN_PAL_STATUS_SUCCESS : (WPAL_LIST_IS_VOS_STATUS_BUSY(status) ? \
            eWLAN_PAL_STATUS_E_BUSY : eWLAN_PAL_STATUS_E_FAILURE) )

/*                                                                            
  
                                                               
    
                                                                        
                                                                   
                                                                          
       

                                                                   
                                           
  
                                                                  
            
 
                                                                
  
                                                                           
                              
  
                                                                              
                                               

                                                                              
                  

                                                                               
                                                                      
                                                   

                                                                       
          
      
  
                                                                            */
#define wpal_list_init(pList) \
         WPAL_LIST_STATUS_BASIC_CHECK( vos_list_init( (vos_list_t *)(pList) ) )


/*                                                                          
  
                                                      

                                                                    
                                                                             
                                                    
   
                                                                       
                                                                              
                                                                         
                                                                      
                                                                         
                      
  
                                                             
  
                                                                     
  
                                                                               
                                                                     
                                                                     

                                                                                 
                                  
          
                                                                       
     
  
                                                                              */
#define wpal_list_destroy(pList) \
    WPAL_LIST_STATUS_BUSY_CHECK( vos_list_destroy( (vos_list_t *)(pList) ) )

/*                                                                            
  
                                                                          

                                                                     
                                          
  
                                                                       
  
                                                                       
  
                                                                              
                                

                                               
    
     
  
                                                                            */
//                                                                           
#define wpal_list_insert_front(pList, pNode) \
    WPAL_LIST_STATUS_BASIC_CHECK( vos_list_insert_front( (vos_list_t *)(pList), (vos_list_node_t *)(pNode) ) )


/*                                                                            
  
                                                                        

                                                                   
                                          
  
                                                                       
  
                                                                       
  
                                                                               
                               

                                               
    
     
  
                                                                            */
//                                                                          
#define wpal_list_insert_back(pList, pNode) \
    WPAL_LIST_STATUS_BASIC_CHECK( vos_list_insert_back( (vos_list_t *)(pList), (vos_list_node_t *)(pNode) ) )


/*                                                                            
  
                                                                          

                                                                     
                                         
  
                                                                      
  
                                                                      
                
  
                                                                             
                                

                                                                                 
                                  

                                                                               
                  
         
                                                                                 
                          
    
      
  
                                                                            */
//                                                                             
#define wpal_list_remove_front(pList, ppNode) \
  ((wpt_status)vos_list_remove_front( (vos_list_t *)(pList), (vos_list_node_t **)(ppNode) ))


/*                                                                            
  
                                                                        

                                                                   
                                          
  
                                                                      
  
                                                                      
                
  
                                                                             
                               

                                               
    
      
  
                                                                            */  
//                                                                            
#define wpal_list_remove_back(pList, ppNode) \
    WPAL_LIST_STATUS_BASIC_CHECK( vos_list_remove_back( (vos_list_t *)(pList), (vos_list_node_t **)(ppNode) ) )


/*                                                                            
  
                                                                

                                                                  
                         
  
                                                                      
  
                                                                   
                                       
  
                                                                           
                                            

                                              
    
     
  
                                                                            */  
//                                                                
#define wpal_list_size(pList, pSize) \
    WPAL_LIST_STATUS_BASIC_CHECK( vos_list_size( (vos_list_t *)(pList), (v_SIZE_t *)(pSize) ) )

/*                                                                            
  
                                                                             

                                                                          
                                                                          
                        
  
                                                                   
  
                                                                      
                        
  
                                                                            
                                

                                     
    
      
  
                                                                            */  
//                                                                           
#define wpal_list_peek_front(pList, ppNode) \
    WPAL_LIST_STATUS_BASIC_CHECK( vos_list_peek_front( (vos_list_t *)(pList), (vos_list_node_t **)(ppNode) ) )

/*                                                                            
  
                                                                           

                                                                         
                                                                         
                        
  
                                                                   
  
                                                                      
                       
  
                                                                           
                                

                                               
    
      
  
                                                                            */  
//                                                                           
#define wpal_list_peek_back(pList, ppNode) \
    WPAL_LIST_STATUS_BASIC_CHECK( vos_list_peek_back( (vos_list_t *)(pList), (vos_list_node_t **)(ppNode) ) )

/*                                                                            
  
                                                                           

                                                                               
                                                                           
                                 
  
                                                                   
  
                                                           
  
                                                                        
                         
  
                                                                               
                                                    

                                               
    
      
  
                                                                            */  
//                                                                       
//                                                        
#define wpal_list_peek_next(pList, pNode, ppNode) \
    WPAL_LIST_STATUS_BASIC_CHECK( vos_list_peek_next( (vos_list_t *)(pList), (vos_list_node_t *)(pNode), (vos_list_node_t **)(ppNode) ) )

/*                                                                            
  
                                                                            

                                                                            
                                                                           
                                 
  
                                                                   
  
                                                           
  
                                                                  
                         
  
                                                                            
                                                    

                                               
    
      
  
                                                                            */                                 
//                                                                       
//                                                        
#define wpal_list_peek_prev(pList, pNode, ppNode) \
    WPAL_LIST_STATUS_BASIC_CHECK( vos_list_peek_prev( (vos_list_t *)(pList), (vos_list_node_t *)(pNode), (vos_list_node_t **)(ppNode) ) )

/*                                                                            
  
                                                                        
            

                                                                       
                                                                  
  
                                                                       
  
                                                                               
  
                                                                              
              
  
                                                                              
                                

                                             
    
     
  
                                                                            */
//                                                     
//                                                                  
//                                                           
#define wpal_list_insert_before(pList, pNodeToInsert, pNode) \
    WPAL_LIST_STATUS_BASIC_CHECK( vos_list_insert_before( (vos_list_t *)(pList), \
                  (vos_list_node_t *)(pNodeToInsert), (vos_list_node_t *)(pNode) ) )

/*                                                                            
  
                                                                             

                                                                      
                                                            
  
                                                                       
  
                                                                               
  
                                                                              
        
  
                                                                              
                                

                                              
    
     
  
                                                                            */                                   
//                                                    
//                                                                 
//                                                                   
#define wpal_list_insert_after(pList, pNodeToInsert, pNode) \
    (WPAL_LIST_STATUS_BASIC_CHECK( vos_list_insert_after((vos_list_t *)(pList), \
                                                         (vos_list_node_t *)(pNodeToInsert), (vos_list_node_t *)(pNode) ))


/*                                                                            
  
                                                                            

                                                                        
                                        
  
                                                                      
  
                                                                  
  
                                                                             
                   

                                               
    
     
  
                                                                            */  
//                                                                                  
#define wpal_list_remove_node(pList, pNodeToRemove) \
    WPAL_LIST_STATUS_BASIC_CHECK( vos_list_remove_node( (vos_list_t *)(pList), \
            (vos_list_node_t *)(pNodeToRemove) ) )


#endif //                     

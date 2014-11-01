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

#if !defined( __VOS_LIST_H )
#define __VOS_LIST_H

/**=========================================================================
  
  \file  vos_list.h
  
  \brief virtual Operating System Services (vOSS) List APIs
               
   Definitions for vOSS Linked Lists API
   
   Lists are implemented as a doubly linked list. An item in a list can 
   be of any type as long as the datatype contains a field of type 
   vos_link_t.

   In general, a list is a doubly linked list of items with a pointer 
   to the front of the list and a pointer to the end of the list.  The
   list items contain a forward and back link.
            
     List                            Nodes
   =============          ===========================
   +-------+
   | Front |------------->+---------+     +---------+
   +-------+              | Next    |---->| Next    |---->NULL
   | Back  |-+            +---------+     +---------+
   +-------+ |   NULL<----| Prev    |<----| Prev    |
             |            +---------+     +---------+
             |            |User Data|     |User Data|
             |            +---------+     +---------+
             |                                 ^
             |                                 |
             +---------------------------------+

   This linked list API is implemented with appropriate locking
   mechanisms to assure operations on the list are thread safe.
  
   Copyright 2008 (c) Qualcomm, Incorporated.  All Rights Reserved.
   
   Qualcomm Confidential and Proprietary.
  
  ========================================================================*/

/*          */

/*                                                                          
               
                                                                          */
#include <vos_types.h>
#include <vos_status.h>
#include <i_vos_list.h>

/*                                                                           
                                        
                                                                          */

/*                                                                           
                   
                                                                          */

/*                                                                          
                                        
                                                                          */
  
/*                                                                            
  
                                                         
    
                                                                       
                                                                   
                                                                          
       

                                                                  
                                           
  
                                                                  
            
 
                                                                
  
                                                                     
                              
  
                                                                        
                                               

                                                                        
                  

                                                                         
                                                                      
                                                   

                                                                 
          
                        
  
                                                                            */
VOS_STATUS vos_list_init( vos_list_t *pList );


/*                                                                          
  
                                                 

                                                                   
                                                                            
                                                    
   
                                                                      
                                                                              
                                                                          
                                                                      
                                                                         
                      
  
                                                             
  
                                                               
  
                                                                         
                                                                     
                                                                     

                                                                           
                                  
          
                                                                 
     
  
                                                                              */
VOS_STATUS vos_list_destroy( vos_list_t *pList );

/*                                                                          
  
                                           

                                                                       
                                                                         
                                      
  
                                                                 
                                                                         
                   
  
                                                                            
                                                                         
                                                        

                                                          
  
                                                            
  
                                                                 

                       
  
                                                                              */
VOS_STATUS vos_list_lock( vos_list_t *pList );

/*                                                                          
  
                                               

                                                                          
                                                                   
                                                                
  
                                                            
  
                                                              
  
                                                                 

                     
  
                                                                              */
VOS_STATUS vos_list_unlock( vos_list_t *pList );


/*                                                                            
  
                                                                         

                                                                    
                                          
  
                                                                       
  
                                                                       
  
                                                                        
                                

                                                                           
                                  
          
                                                                          
                          
    
     
  
                                                                            */
VOS_STATUS vos_list_insert_front( vos_list_t *pList, vos_list_node_t *pNode );


/*                                                                            
  
                                                                       

                                                                  
                                          
  
                                                                       
  
                                                                       
  
                                                                        
                               

                                                                           
                                  
          
                                                                          
                          
    
     
  
                                                                            */
VOS_STATUS vos_list_insert_back( vos_list_t *pList, vos_list_node_t *pNode );

/*                                                                            
  
                                                                               
                                                                            
                                                                

                                                                       
                                          
  
                                                                       
  
                                                                       

                                                                   
                                       
 
                                                                        
                               

                                                                           
                                  
          
                                                                          
                          
    
     
  
                                                                            */
VOS_STATUS vos_list_insert_back_size( vos_list_t *pList, vos_list_node_t *pNode, v_SIZE_t *pSize );


/*                                                                            
  
                                                                         

                                                                    
                                          
  
                                                                      
  
                                                                      
                
  
                                                                       
                                

                                                                           
                                  

                                                                         
                  
         
                                                                           
                          
    
                            
  
                                                                            */
VOS_STATUS vos_list_remove_front( vos_list_t *pList, vos_list_node_t **ppNode );


/*                                                                            
  
                                                                       

                                                                  
                                          
  
                                                                      
  
                                                                      
                
  
                                                                       
                               

                                                                           
                                  

                                                                         
                  
          
                                                                           
                          
    
                            
  
                                                                            */  
VOS_STATUS vos_list_remove_back( vos_list_t *pList, vos_list_node_t **ppNode );


/*                                                                            
  
                                                               

                                                                 
                         
  
                                                                      
  
                                                                   
                                       
  
                                                                     
                                            

                                                                           
                                  
          
                                                                     
    
     
  
                                                                            */  
VOS_STATUS vos_list_size( vos_list_t *pList, v_SIZE_t *pSize );

/*                                                                            
  
                                                                            

                                                                         
                                                                          
                        
  
                                                                   
  
                                                                      
                        
  
                                                                      
                                

                                                                           
                                  

                                                                         
                  
          
                                                                         
    
                            
  
                                                                            */  
VOS_STATUS vos_list_peek_front( vos_list_t *pList, vos_list_node_t **ppNode );

/*                                                                            
  
                                                                          

                                                                        
                                                                         
                        
  
                                                                   
  
                                                                      
                       
  
                                                                     
                                

                                                                           
                                  

                                                                         
                  
          
                                                                         
    
                                                                           
                             
  
                                                                            */  
VOS_STATUS vos_list_peek_back( vos_list_t *pList, vos_list_node_t **ppNode );

/*                                                                            
  
                                                                          

                                                                              
                                                                           
                                 
  
                                                                   
  
                                                           
  
                                                                        
                         
  
                                                                         
                                                    

                                                                           
                                  

                                                                         
                  
          
                                                                             
    
                            
  
                                                                            */  
VOS_STATUS vos_list_peek_next( vos_list_t *pList, vos_list_node_t *pNode, 
                               vos_list_node_t **ppNode );

/*                                                                            
  
                                                                           

                                                                           
                                                                           
                                 
  
                                                                   
  
                                                           
  
                                                                  
                         
  
                                                                      
                                                    

                                                                           
                                  
          
                                                                         
                  
          
                                                                             
    
                            
  
                                                                            */                                 
VOS_STATUS vos_list_peek_prev( vos_list_t *pList, vos_list_node_t *pNode, 
                               vos_list_node_t **ppNode );

/*                                                                            
  
                                                                       
            

                                                                      
                                                                  
  
                                                                       
  
                                                                               
  
                                                                              
              
  
                                                                        
                                

                                                                           
                                  
          
                                                                  
                            
    
     
  
                                                                            */
VOS_STATUS vos_list_insert_before( vos_list_t *pList, vos_list_node_t *pNodeToInsert, 
                                   vos_list_node_t *pNode );

/*                                                                            
  
                                                                            

                                                                     
                                                            
  
                                                                       
  
                                                                               
  
                                                                              
        
  
                                                                        
                                

                                                                           
                                  
          
                                                                  
                            
    
     
  
                                                                            */                                   
VOS_STATUS vos_list_insert_after( vos_list_t *pList, vos_list_node_t *pNodeToInsert, 
                                  vos_list_node_t *pNode );         


/*                                                                            
  
                                                                           

                                                                       
                                        
  
                                                                      
  
                                                                  
  
                                                                       
                   

                                                                           
                                  
          
                                                                         
                  
          
          
                                                                             
    
     
  
                                                                            */  
VOS_STATUS vos_list_remove_node( vos_list_t *pList, vos_list_node_t *pNodeToRemove );



#endif //             

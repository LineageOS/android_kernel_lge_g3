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

/*
  
                                                        
                                                                      
                 
                              
                          
            
                                                       
                                                                       
 */

#ifndef __SYSDEF_H
#define __SYSDEF_H

//                                 
//                                                                   

//                                           
#define SYS_TICK_DUR_NS     10000000    //     
#define SYS_TICK_TO_MICRO_SECOND   10000

//                                            
#define SYS_TICK_DUR_MS     (SYS_TICK_DUR_NS/1000000)

//                         
#define SYS_CLOCKS_PER_MS   120000 //        

//                
#define SYS_ADD_BA_RSP_DUR   1000

//                                      
#define SYS_TICK_DUR_CLK    (SYS_TICK_DUR_MS * SYS_CLOCKS_PER_MS)

//                                   
#define SYS_TICKS_PER_SECOND (1000/SYS_TICK_DUR_MS)

//                              
#define SYS_MS_TO_TICKS(x)   ((x) / SYS_TICK_DUR_MS)

//                                   
#define SYS_SEC_TO_TICKS(x)  ((x) * SYS_TICKS_PER_SECOND)

//                                   
#define SYS_MIN_TO_TICKS(x)  (((x) * 60) * SYS_TICKS_PER_SECOND)

//                                         
#define SYS_MNT_INTERVAL     60

//                  
#define SYS_MS_TO_TU(x)      ((x * 1000) >> 10)

//          
#define SYS_TU_TO_MS(x)      ((x << 10) / 1000)

//                                             

//                           

//          
# define SYS_HAL_Q_SIZE     200        //                        

//                    
# define SYS_MMH_HI_PRI_Q_SIZE      200        //                        

//                  
# define SYS_MMH_PROT_Q_SIZE     400          //                        

//               
# define SYS_MMH_DEBUG_Q_SIZE    800          //                         

//            
# define SYS_MNT_Q_SIZE          200          //                        

//                   
# define SYS_LIM_Q_SIZE          400          //                        

//                         
# define SYS_SCH_Q_SIZE          800          //                        

//                   
# define SYS_PMM_Q_SIZE          800          //                         

//                  
# define SYS_TX_Q_SIZE           2048    //                         

//                  
# define SYS_RX_Q_SIZE           2048   //                         

//                    
# define SYS_NIM_PTT_Q_SIZE   200             //                        

//                       
//               

# define SYS_DPH_SEM_INITIAL_CNT 0

//                    

# define SYS_BBT_SEM_INITIAL_CNT 0

//                    
//     

# define SYS_HAL_THREAD_ENTRY_FUNCTION halEntry
# define SYS_HAL_STACK_SIZE            8192
# define SYS_HAL_THREAD_PRIORITY       2

//     

# define SYS_DPH_THREAD_ENTRY_FUNCTION dphEntry
# define SYS_DPH_STACK_SIZE            8192
# define SYS_DPH_THREAD_PRIORITY       15

//     

# define SYS_BBT_THREAD_ENTRY_FUNCTION bbtEntry
# define SYS_BBT_STACK_SIZE            8192
# define SYS_BBT_THREAD_PRIORITY       16

//     

# define SYS_SCH_STACK_SIZE            8192
# define SYS_SCH_THREAD_PRIORITY       17

//     

# define SYS_PMM_STACK_SIZE            8192
# define SYS_PMM_THREAD_PRIORITY       17

//     

# define SYS_LIM_THREAD_ENTRY_FUNCTION limEntry
# define SYS_LIM_STACK_SIZE            8192
# define SYS_LIM_THREAD_PRIORITY       18

//       

# define SYS_MNT_THREAD_ENTRY_FUNCTION mntEntry
# define SYS_MNT_STACK_SIZE            8192
# define SYS_MNT_THREAD_PRIORITY       25

//     

# define SYS_MMH_THREAD_ENTRY_FUNCTION mmhEntry
# define SYS_MMH_STACK_SIZE            8192
# define SYS_MMH_THREAD_PRIORITY        10

//                 

# define SYS_NIM_PTT_THREAD_STACK_SIZE 8192
# define SYS_NIM_PTT_THREAD_PRIORITY   28

#endif //           

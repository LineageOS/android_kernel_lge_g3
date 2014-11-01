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

#ifndef __WLAN_HDD_DEV_PWR_H
#define __WLAN_HDD_DEV_PWR_H

#include <wlan_hdd_includes.h>
#include <wlan_hdd_power.h>
#include <vos_sched.h>
#include <vos_api.h>

/*                                                                            

                                
                                                                          

                               
                          

                              
                                                     
                                                  

                                                                            */
VOS_STATUS hddRegisterPmOps(hdd_context_t *pHddCtx);

/*                                                                            

                                   
                                                                              

                               
                          

                              
                                                        
                                                     

                                                                            */
VOS_STATUS hddDeregisterPmOps(hdd_context_t *pHddCtx);

/*                                                                            

                                 
                                                

                              
                                               

           

                                                                            */
void hddDevTmLevelChangedHandler(struct device *dev, int changedTmLevel);

/*                                                                            

                           
                                                                          

                               
                          

                              
                                                     
                                                  

                                                                            */
VOS_STATUS hddDevTmRegisterNotifyCallback(hdd_context_t *pHddCtx);

/*                                                                            

                              
                                                                             

                               
                          

                              
                                                        
                                                     

                                                                            */
VOS_STATUS hddDevTmUnregisterNotifyCallback(hdd_context_t *pHddCtx);

#endif

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

#ifndef _ANI_ERRORS_H_
#define _ANI_ERRORS_H_

/* 
                                                                     
            
 */
#define ANI_OK      0
#define ANI_ERROR   ANI_E_FAILED

/* 
                                                                     
                             
 */

#define ANI_E_FAILED        -1 //                                
#define ANI_E_MALLOC_FAILED -2 //                       
#define ANI_E_ILLEGAL_ARG   -3 //                              
#define ANI_E_NULL_VALUE    -4 //                                   
#define ANI_E_RAND_FAILED   -5 //            
#define ANI_E_SHORT_PACKET  -6 //                                     
#define ANI_E_ELEM_NOT_FND  -7 //                   
#define ANI_E_INVALID_LEN   -8 //                             
#define ANI_E_INVALID_DT    -9 //                    
#define ANI_E_TIMEOUT       -10 //                  
#define ANI_E_DBM_INVALID_ID    -11 //                              
#define ANI_E_DBM_KEY_NOT_FOUND -12 //                             
#define ANI_E_FILE_EMPTY    -13 //            
#define ANI_E_INVALID_MT    -14 //                          
#define ANI_E_NOT_IMPLEMENTED   -15 //                         
#define ANI_E_INVALID_PT    -16 //                         
#define ANI_E_INVALID_PV    -17 //                        
#define ANI_E_IPCOPEN    -18 //                 
#define ANI_E_IPCCONNECT    -19 //                    
#define ANI_E_IPCSEND    -20 //                 
#define ANI_E_FILE_NOT_FOUND    -21 //                
#define ANI_E_FILE_INVALID_CONTENT    -22 //                      
#define ANI_E_FILE_READ_FAILED    -23 //                  

//                                                                        

/*
                                                      
                                                                   
                                   
 */
#define ANI_E_RANGE_START_AAG                   -1025

//                         
#define ANI_E_RANGE_START_AAG_CLI               -1029
#define ANI_E_CLI_ARG_MISSING                   -1029 //                          
#define ANI_E_CLI_ARG_INVALID                   -1030 //                          
#define ANI_E_CLI_PAM_RADIUS_DB_UPDATE_FAILURE  -1031 //                                
#define ANI_E_CLI_AUTH_SERVER_NOT_FOUND         -1032 //                      
#define ANI_E_CLI_SYS_INTERNAL_ERROR_PACKET_NULL -1033 //               
#define ANI_E_CLI_INVALID_DEVICE                -1034 //               
#define ANI_E_CLI_AUTH_SERVER_INVALID_IPADDR    -1035 //                
#define ANI_E_CLI_AUTH_ZONE_NOT_EMPTY           -1036 //               
#define ANI_E_CLI_AUTH_ZONE_AUTH_SERVERS_NOT_COPIED -1037 //                
#define ANI_E_CLI_AUTH_ZONE_NAME_MISSING        -1038 //                  
#define ANI_E_CLI_AUTH_ZONE_EMPTY               -1039 //           
#define ANI_E_CLI_AUTH_ZONE_NOT_FOUND           -1040 //               
#define ANI_E_CLI_AUTH_SERVER_ARG_NOT_FOUND     -1041 //                            
#define ANI_E_CLI_IFNAME_ARG_NOT_FOUND          -1042 //                   
#define ANI_E_CLI_KEYINDEX_ARG_NOT_FOUND        -1043 //                     
#define ANI_E_CLI_KEYLENGTH_ARG_NOT_FOUND       -1044 //                      
#define ANI_E_CLI_KEYLENGTH_ARG_INVALID         -1045 //                      
#define ANI_E_CLI_SET_WEP_KEY_FAILED            -1046 //                   
#define ANI_E_CLI_SYS_INTERNAL_ERROR            -1047 //               
#define ANI_E_CLI_OLD_PASSWORD_MISSING          -1048 //                     
#define ANI_E_CLI_NEW_PASSWORD_MISSING          -1049 //             
                                                      //        
#define ANI_E_CLI_WEP_KEY_LEN_ERROR             -1051 //                 
#define ANI_E_CLI_WEP_KEY_HEX_ERROR             -1052 //                 
#define ANI_E_CLI_WPA_MODES_CFG_ERROR           -1053 //                 
#define ANI_E_CLI_WEP_AND_OPEN_CFG_ERROR        -1054 //                   
#define ANI_E_CLI_OPEN_AND_WEP_CFG_ERROR        -1055 //                   
#define ANI_E_CLI_LEGACY_WEP_AND_OPEN_CFG_ERROR -1056 //                   
#define ANI_E_CLI_OPEN_AND_LEGACY_WEP_CFG_ERROR -1057 //                   
#define ANI_E_CLI_WPA_MODES_NOT_AVAILABLE       -1058 //                     
#define ANI_E_CLI_INVALID_LISENCE_KEY           -1059 // invalid license
#define ANI_E_CLI_EXISTING_LISENCE_KEY          -1060 // duplicated license
#define ANI_E_CLI_WEP_INVALID_LENGTH_CHANGE     -1061 //                      
#define ANI_E_CLI_CNF_PASSWORD_MISMATCH         -1062 //                          
#define ANI_E_CLI_INVALID_DISABLE               -1063 //                             
#define ANI_E_CLI_PACKNUM_RANGE_ERROR           -1064 //                             
#define ANI_E_CLI_IP_ADDR_INVALID               -1065 //                   
#define ANI_E_CLI_AUTH_ZONE_NAME_INVALID        -1066 //                  
#define ANI_E_CLI_OLD_PASSWORD_INVALID          -1067 //                     
#define ANI_E_CLI_NO_EXT_RAD_ON_NONSECP         -1068 //                              
                                                      //                               
#define ANI_E_CLI_NO_EXT_AUTH_ZONE_ALLOWED      -1069 //                            
                                                      //                     
#define ANI_E_CLI_PORTAL_ZONE_AUTO_CONFIGURED   -1070 //                               
#define ANI_E_CLI_DEL_REQ_ON_REF_AUTH_SERVER    -1071 //                          
                                                      //                  
#define ANI_E_CLI_DEL_REQ_ON_REF_AUTH_ZONE      -1072 //                        
                                                      //                     
#define ANI_E_CLI_INVALID_INTERIM_UPDT_VALUE    -1073 //                                           
#define ANI_E_RANGE_END_AAG_CLI                 -1199
//                       

//                              
#define ANI_E_MIC_FAILED                        -1200 //                   
#define ANI_E_REPLAY_CHECK_FAILED               -1201 //                    
#define ANI_E_RADIUS_PROFILE_MISSING            -1202 //             
                                                      //          
#define ANI_E_AUTH_FAILED                       -1203 //                      
#define ANI_E_RADIUS_PRIV_LEVEL_MISSING         -1204 //                           
#define ANI_E_RADIUS_PRIV_LEVEL_INCORRECT       -1205 //                             

//                                                                       
#define ANI_E_INVALID_COOKIE                    -1300 //               

#define ANI_E_RANGE_END_AAG                     -2048

/*
                                                      
                              
 */
#define ANI_E_RANGE_START_RAD       -2049
#define ANI_E_RAD_FAILED            -2049 //                         
#define ANI_E_RAD_ATTR_TOO_LONG     -2050 //                    
#define ANI_E_RAD_UNSOLICITED_RESP  -2051 //                      
#define ANI_E_RAD_BAD_RESP_AUTH     -2052 //                            
#define ANI_E_RAD_BAD_MESSG_AUTH    -2053 //                            
#define ANI_E_RAD_ATTR_NOT_FOUND    -2054 //                          
#define ANI_E_RAD_TIMEOUT           -2055 //                                      
#define ANI_E_RAD_REJECT            -2056 //                                   
#define ANI_E_RANGE_END_RAD         -2148

/*
                                                                   
 */
#define ANI_E_RANGE_START_SSM -2149
#define ANI_E_SSM_CERT_UNPARSEABLE          (ANI_E_RANGE_START_SSM - 1)
#define ANI_E_SSM_CERT_EXPIRED              (ANI_E_RANGE_START_SSM - 2)
#define ANI_E_SSM_CERT_THUMBPRINT_MISMATCH  (ANI_E_RANGE_START_SSM - 3)
#define ANI_E_SSM_CERT_NEW_ID               (ANI_E_RANGE_START_SSM - 4)
#define ANI_E_RANGE_END_SSM   -2999

/*
                                                      
                                                          
 */
#define ANI_E_RANGE_START_NETSIM     -3000
#define ANI_E_RANGE_END_NETSIM       -3500

/*
                                                      
                                      
 */
#define ANI_E_RANGE_START_DISC     -3501
#define ANI_E_RANGE_END_DISC       -4000

/*
                                                      
               
 */
#define ANI_E_RANGE_START_EZC     -4001

//                                                                      
#define ANI_E_RANGE_END_EZC       -4500

/*
                                                      
                                 
 */
#define ANI_E_RANGE_START_SWD     -4501

//                                                                   
#define ANI_E_RANGE_END_SWD       -4600

/*
                                                      
                                         
 */
#define ANI_E_RANGE_START_DDS     -4601

//                                                                   
#define ANI_E_RANGE_END_DDS       -4700

/*
                                                  
                    
 */
#define ANI_E_RANGE_START_HTTPS -4701
#define ANI_E_HTTPS_UNREACHABLE      (ANI_E_RANGE_START_HTTPS - 0)
#define ANI_E_HTTPS_UNTRUSTED_CERT   (ANI_E_RANGE_START_HTTPS - 1)
#define ANI_E_HTTPS_RECVD_ALERT      (ANI_E_RANGE_START_HTTPS - 2)
#define ANI_E_HTTPS_FAILED           (ANI_E_RANGE_START_HTTPS - 3)
#define ANI_E_RANGE_END_HTTPS   -4800

/*
                                                  
                         
 */
#define ANI_E_RANGE_START_ENROLLMENT -4801
#define ANI_E_ENROLL_TP_AVAILABLE     (ANI_E_RANGE_START_ENROLLMENT - 0)
#define ANI_E_ENROLL_ALREADY_TRUSTED  (ANI_E_RANGE_START_ENROLLMENT - 1)
#define ANI_E_ENROLL_NOT_FOUND        (ANI_E_RANGE_START_ENROLLMENT - 2)
#define ANI_E_ENROLL_PWD_FAILED       (ANI_E_RANGE_START_ENROLLMENT - 3)
#define ANI_E_ENROLL_FAILED           (ANI_E_RANGE_START_ENROLLMENT - 4)
#define ANI_E_ENROLL_NOT_PRISTINE     (ANI_E_RANGE_START_ENROLLMENT - 5)
#define ANI_E_RANGE_END_ENROLLMENT   -4900


/*
                                                       
 */
#define ANI_E_RANGE_START_NSM -4901
#define ANI_E_NSM_IPADDR_ASSIGNED     (ANI_E_RANGE_START_NSM - 0)
#define ANI_E_RANGE_END_NSM   -5000

/*
                                                            
                      
 */
#define ANI_E_RANGE_START_IMAGE -5001
#define ANI_E_IMAGE_INVALID         (ANI_E_RANGE_START_IMAGE - 0)
#define ANI_E_IMAGE_UNSUPPORTED     (ANI_E_RANGE_START_IMAGE - 1)
#define ANI_E_RANGE_END_IMAGE   -5100

/*
                                                     
 */
#define ANI_E_RANGE_START_CM -5101
#define ANI_E_MESG_UNAVAILABLE      (ANI_E_RANGE_START_IMAGE - 0)
#define ANI_E_RANGE_END_CM   -5200


#define ANI_IS_STATUS_SUCCESS( retVal )  ( ( retVal >= 0 ) )

#endif //              

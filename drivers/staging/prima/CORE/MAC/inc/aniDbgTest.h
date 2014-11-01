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

#ifndef _ANIDBGTEST_H_
#define _ANIDBGTEST_H_

#include "sirTypes.h"

#define MAX_PARMS_SIZE              256
#define MAX_RESPONSE_SIZE           512
#define MAX_PCI_CFG_WRDS            19
#define MAX_NUM_TST_STAS_PER_AP     3

#define ANI_PCI_CFG_MIN_GNT 4
#define ANI_PCI_CFG_MAX_LAT 32
//                                                 
#define EEP_PRODUCT_ID_MPCI_3_2    1
#define EEP_PRODUCT_ID_MPCI_2_2    2
#define EEP_PRODUCT_ID_CARDBUS     3
#define EEP_PRODUCT_ID_HEAP_W_INT  4
#define EEP_PRODUCT_ID_HEAP_W_EXT  5
#define EEP_PRODUCT_ID_MRAP        6

#define ANI_CARDBUS_EXPECTED_CIS \
                  (((0x3416 + sizeof(tPolSystemParameters)) << 3)|1)


//                                        
typedef enum sANI_TEST_ID
{
    //                                                              
    //                                                                
    ANI_TESTID_NONE = 0,

    //                                                  
    //                                   

    ANI_BUS_ACCESS_TEST,
    ANI_MEMORY_TEST,
    ANI_INTERRUPT_TEST,
    ANI_REG_READ_WRITE_TEST,
    ANI_BMU_INIT_PDU_TEST,
    ANI_HASH_TBL_ADD_DEL_GET_TEST_ID,
    ANI_HASH_TBL_MULT_ENTRY_TEST_ID,

    //                
    ANI_SEND_CONTINUOUS_TEST,
    ANI_PKT_ROUTING_TEST,

    ANI_ADD_STA_TEST_ID,
    ANI_DELETE_STA_TEST_ID,

    //          
    ANI_BURST_READ_WRITE_TEST_ID,
    ANI_MOVE_ACK_TEST_ID,
    ANI_RESET_WQ_TEST_ID,
    ANI_HIF_LPBK_TEST_ID,

    //         

    //          
    ANI_BCN_GEN_TEST_ID,

    //          

    ANI_HASH_TBL_DELETE_TEST_ID,
    ANI_HASH_TBL_GET_TEST_ID,

    //          
    ANI_AGING_TEST_ID,
    ANI_COUNTERS_TEST_ID,

    //          
    ANI_SET_PROMOSCOUS_MODE_TEST_ID,
    ANI_SET_SCAN_MODE_TEST_ID,

    //          
    ANI_SET_CYG_REG_READ_WRITE_TEST_ID,

    //          
    ANI_SET_BB_CAL_TEST_ID,

    //         
    ANI_SET_LO_CHAN_TEST_ID,

    //             

    //         
    ANI_CAL_TEST_ID,

    //                   
    ANI_HIF_BURST_DMA_TEST_ID,

    //                                  
    ANI_TESTS_MAX
} tANI_TEST_ID;

//                                                
typedef enum sANI_HDD_TEST_ID {
    ANI_HDD_TESTS_START = 0x10000,
    ANI_PCI_CFG_TEST = ANI_HDD_TESTS_START,
    ANI_EEPROM_TEST,

    //                                      
    ANI_HDD_TESTS_MAX

} tANI_HDD_TEST_ID;

//                                                          
typedef enum sANI_TEST_TRIG_TYPE
{
    ANI_TRIG_TYPE_NONE = 0,

    ANI_TRIG_TYPE_CFG,
    ANI_TRIG_TYPE_START,
    ANI_TRIG_TYPE_GET_RESULTS,
    ANI_TRIG_TYPE_STOP,

    ANI_TRIG_TYPE_MAX
} tANI_TEST_TRIG_TYPE;

//             
typedef enum sANI_DBG_TEST_STATE
{
    ANI_TEST_STATE_IDLE = 0,
    ANI_TEST_STATE_IN_PROGRESS,
    ANI_TEST_STATE_DONE

} tANI_DBG_TEST_STATE;

/* 
                                                                            
                       
 */
typedef enum sANI_DBG_CFG_TYPES
{
    ANI_DBG_CFG_PHY_MODE_RATE,
    ANI_DBG_CFG_ROUTING_FLAGS,
    ANI_DBG_CFG_STA_ID,
    ANI_DBG_CFG_IS_INFRASTRUCTURE_MODE,
    ANI_DBG_CFG_USE_REAL_PHY,
    ANI_DBG_CFG_PHY_DROPS,
    ANI_DBG_CFG_ADD_STA,
    ANI_DBG_CFG_DEL_STA,
    ANI_DBG_CFG_ENABLE_STA_TX,
    ANI_DBG_CFG_DISABLE_STA_TX,
    ANI_DBG_CFG_READ_REGISTER,
    ANI_DBG_CFG_WRITE_REGISTER,
    ANI_DBG_CFG_GET_ARQ_WINDOW_SIZE,
    ANI_DBG_CFG_SET_ARQ_WINDOW_SIZE,
    ANI_DBG_CFG_SET_CHANNEL,
    ANI_DBG_CFG_SET_MAC_ADDRESS,
    ANI_DBG_CFG_SET_MEM,
    ANI_DBG_CFG_GET_MEM,
    ANI_DBG_CFG_CTRL_TXWQ,
    ANI_DBG_CFG_GET_ACTIVITY_SET,
    ANI_DBG_CFG_SET_ACK_POLICY,
    ANI_DBG_CFG_AGING_CMD,
    ANI_DBG_CFG_SET_KEY,
    ANI_DBG_CFG_SET_PER_STA_KEY,
    ANI_DBG_CFG_TFP_ABORT,
    ANI_DBG_CFG_GET_ACT_CHAINS,
    ANI_DBG_CFG_IS_CHAIN_ACTIVE,
    ANI_DBG_CFG_BB_FILTER_CAL,
    ANI_DBG_CFG_DCO_CAL,
    ANI_DBG_CFG_IQ_CAL,
    ANI_DBG_CFG_TX_LO_LEAKAGE_CAL,
    ANI_DBG_CFG_SEND_PKTS,
    ANI_DBG_CFG_STOP_PKTS,
    ANI_DBG_CFG_ENABLE_DISABLE_BEACON_GEN,
    ANI_DBG_CFG_DCO_GET,
    ANI_DBG_CFG_DCO_SET,
    ANI_DBG_CFG_SET_PWR_TEMPL,
    ANI_DBG_CFG_GET_PWR_TEMPL,
    ANI_DBG_CFG_INIT,
    ANI_DBG_CFG_SET_EEPROM_FLD,
    ANI_DBG_CFG_GET_EEPROM_FLD,
    ANI_DBG_CFG_SET_NUM_TRANSMITTERS,
    ANI_DBG_CFG_SET_RX_CHAINS,
    ANI_DBG_CFG_HCF_TEST,
    ANI_DBG_CFG_POLARIS_REV_ID,
    ANI_DBG_CFG_UPDATE_DATA_FROM_EEPROM,
    ANI_DBG_CFG_GET_TEMP,
    ANI_DBG_CFG_SET_STA_ADDRLST,
    ANI_DBG_CFG_GET_PWR_GAIN,
    ANI_DBG_CFG_SW_CLOSED_LOOP_TPC,

    ANI_DBG_CFG_TYPE_MAX
} tANI_DBG_CFG_TYPES;

/* 
                                                                     
                        
 */

//                                
# define ANI_DBG_CFG_PHY_MODE_RATE_LEN 2

//                                                                     
# define ANI_DBG_CFG_ROUTING_FLAGS_LEN 5

# define ANI_DBG_CFG_STA_ID_LEN        2

//                                                                       
//
//                                                                        
//
//                                                                       
//
//                                                         

//                                                         
//                                               
typedef struct sAniDbgCfgGetPolarisVersionResponse
{
    //                  
    unsigned int rc;  //            
    unsigned long version;
} tAniDbgCfgGetPolarisVersionResponse, *tpAniDbgCfgGetPolarisVersionResponse;

//                                                              
//                                   

typedef struct sAniDbgCfgPhyModeRateParams
{
    unsigned int phyMode;
    unsigned int phyRate;
} tAniDbgCfgPhyModeRateParams, *tpAniDbgCfgPhyModeRateParams;

//                                                         
//                                   
typedef struct sAniDbgCfgRoutingFlagsParams
{
    unsigned int type;
    unsigned int subType;
    unsigned int routingFlags;
} tAniDbgCfgRoutingFlagsParams, *tpAniDbgCfgRoutingFlagsParams;

//                                                         
//                            
typedef struct sAniDbgCfgStaIdParams
{
    unsigned int staId;
} tAniDbgCfgStaIdParams, *tpAniDbgCfgStaIdParams;

//                                                         
//                                          
typedef struct sAniDbgCfgIsInfrastructureParams
{
    unsigned int isInfrastructureMode;
} tAniDbgCfgIsInfrastructureParams, *tpAniDbgCfgIsInfrastructureParams;

//                                                         
//                              
typedef struct sAniDbgCfgRealPhyParams
{
    unsigned int useRealPhy;
} tAniDbgCfgRealPhyParams, *tpAniDbgCfgRealPhyParams;

//                                                         
//                           
typedef struct sAniDbgCfgPhyDropParams
{
    unsigned int usePhyDrops;
    unsigned int rate;
    unsigned int burstSize;
    unsigned int mode;
} tAniDbgCfgPhyDropParams, *tpAniDbgCfgPhyDropParams;

//                                                         
//                         
typedef struct sAniDbgCfgAddStaParams
{
    unsigned int staId;
    unsigned char macAddr[6];
    unsigned int phyMode;
    unsigned int rate;
    unsigned int skipSP;
    unsigned int ackPolicy;
} tAniDbgCfgAddStaParams, *tpAniDbgCfgAddStaParams;

//                                                         
//                            
typedef struct sAniDbgCfgDelStaParams
{
    unsigned int staId;
    unsigned char macAddr[6];
} tAniDbgCfgDelStaParams, *tpAniDbgCfgDelStaParams;

//                                                         
//                                                              
#define ANI_TIT_MIN_REG_ADDR 0x02000000


//                                                         
//                                                              
#define ANI_TIT_MAX_REG_ADDR 0x0203ffff

//                                                         
//                                              
typedef struct sAniDbgCfgReadRegParams
{
    unsigned long regAddr;
} tAniDbgCfgReadRegParams, *tpAniDbgCfgReadRegParams;

//                                                         
//                                               
typedef struct sAniDbgCfgWriteRegParams
{
    unsigned long regAddr;
    unsigned long regVal;
} tAniDbgCfgWriteRegParams, *tpAniDbgCfgWriteRegParams;




//                                                         
//                          
typedef struct
{
    unsigned long toneID;   //                                                                     
}tAniDbgCfgSetToneId, *tpAniDbgCfgSetToneId;


//                                                         
//                                               
typedef struct sAniDbgCfgRegReadResponse
{
    //                  
    unsigned int rc;  //            
    unsigned long regVal;
} tAniDbgCfgReadRegResponse, *tpAniDbgCfgReadRegResponse;

//                                                         
//                                     
typedef struct sAniDbgCfgGetArqWindowResponse
{
    unsigned int rc;  //            
    unsigned long val;
} tAniDbgCfgGetArqWindowResponse, *tpAniDbgCfgGetArqWindowResponse;

//                                                         
//                                               
typedef struct sAniDbgCfgSetArqWindowParams
{
    unsigned long windowSize;
} tAniDbgCfgSetArqWindowRegParams, *tpAniDbgCfgSetArqWindowParams;

//                                                         
//                                               
typedef struct sAniDbgCfgSetChanParams
{
    unsigned long chId;
} tAniDbgCfgSetChanParams, *tpAniDbgCfgSetChanParams;


//                                                         
//                                               
typedef struct sAniDbgCfgSetMacAddrParams
{
    unsigned char macAddr[6];
    unsigned int  flag;       //                                  
} tAniDbgCfgSetMacAddrParams, *tpAniDbgCfgSetMacAddrParams;

//                                                         
//                                    
typedef struct sAniDbgCfgSetMemoryParams
{
    unsigned int fUseBurstDma;
    unsigned int numOfWords;
    unsigned int ahbAddr;
    unsigned int writeData; //                                    
} tAniDbgCfgSetMemoryParams, *tpAniDbgCfgSetMemoryParams;

//                                                         
//                                    
typedef struct sAniDbgCfgGetMemoryParams
{
    unsigned int fUseBurstDma;
    unsigned int numOfWords;
    unsigned int ahbAddr;
} tAniDbgCfgGetMemoryParams, *tpAniDbgCfgGetMemoryParams;

//                                                         
//                                              
typedef struct sAniDbgCfgGetMemoryResponse
{
    unsigned int rc;  //                              
    unsigned int readData[1];
} tAniDbgCfgGetMemoryResponse, *tpAniDbgCfgGetMemoryResponse;


//                                                         
//                                                  
typedef struct sAniDbgCfgCtrlTxWqParams
{
    unsigned int staId;
    unsigned int wqId;
    unsigned int action; //                        

} tAniDbgCfgCtrlTxWqParams, *tpAniDbgCfgCtrlTxWqParams;

//                                                         
//                                            
typedef struct sAniDbgCfgGetAsParams
{
    unsigned int id;     //               

} tAniDbgCfgGetAsParams, *tpAniDbgCfgGetAsParams;

//                                            
typedef struct sAniDbgCfgGetAsResponse
{
    unsigned int rc;  //            
    unsigned int nEntries;
    unsigned int entries[64];

} tAniDbgCfgGetAsResponse, *tpAniDbgCfgGetAsResponse;

//                                                         
//                                
typedef struct sAniDbgCfgSetAckPolicyParams
{
    unsigned int id;     //               
    unsigned int policy;
    unsigned int staId;
    unsigned int tcId;

} tAniDbgCfgSetAckPolicyParams, *tpAniDbgCfgSetAckPolicyParams;

//                                                         
//                                  
typedef struct sAniDbgCfgAgingCmdParams
{
    unsigned int staId;
    unsigned int tcId;

} tAniDbgCfgAgingCmdParams, *tpAniDbgCfgAgingCmdParams;

//                                                         
//                                  
typedef struct sAniDbgCfgTfpAbortParams
{
    unsigned int staId;

} tAniDbgCfgTfpAbortParams, *tpAniDbgCfgTfpAbortParams;

//                                                         
//                                              
typedef struct sAniDbgCfgEnableBeaconParams
{
    unsigned int fEnableBeacons;

} tAniDbgCfgEnableBeaconParams, *tpAniDbgCfgEnableBeaconParams;

//                                                         
//                                            
typedef struct sAniDbgCfgSetKeyParams
{
    unsigned int type;  //                 
    unsigned int keyId; //                       
    unsigned int keyLen;
    unsigned char key[16];

} tAniDbgCfgSetKeyParams, *tpAniDbgCfgSetKeyParams;

//                                                         
//                                 
typedef struct sAniDbgCfgSetPerStaKeyParams
{
    unsigned int staId;
    unsigned int id0;
    unsigned int id1;
    unsigned int keyValid; //                         
    unsigned int useDefaultKey;
    unsigned int defaultKeyId;
    unsigned int edPolicy;
    unsigned char key[16];
    unsigned char keylen;

} tAniDbgCfgSetPerStaKeyParams, *tpAniDbgCfgSetPerStaKeyParams;

//                                                         
//                                
typedef struct sAniDbgCalTestParams
{
    unsigned int id;   //                         
    unsigned int chId; //                                                   
}tAniDbgCalTestParams, *tpAniDbgCalTestParams;

typedef struct sAniDbgCalTestResponse
{
    unsigned int rc;
}tAniDbgCalTestResponse, *tpAniDbgCalTestResponse;

//                                                          
//                               
typedef struct sAniDbgCfgGetDcoParams
{
    unsigned int chain;
    unsigned int address;
} tAniDbgCfgGetDcoParams, *tpAniDbgCfgGetDcoParams;

typedef struct sAniDbgCfgGetDcoResponse
{
    unsigned int rc;
    unsigned int val;
} tAniDbgCfgGetDcoResponse, *tpAniDbgCfgGetDcoResponse;

//                                                         
//                               
typedef struct sAniDbgCfgSetDcoParams
{
    unsigned int chain;
    unsigned int address;
    unsigned int val;
} tAniDbgCfgSetDcoParams, *tpAniDbgCfgSetDcoParams;

//                                                         
//                                       
typedef struct sAniDbgCfgSetPwrTemplParams
{
    unsigned int staId;
    unsigned int mode;
    unsigned int rate;
} tAniDbgCfgSetPwrTemplParams, *tpAniDbgCfgSetPwrTemplParams;


//                                           
typedef struct sAniDbgCfgGetPwrTemplParams
{
    unsigned int staId;
} tAniDbgCfgGetPwrTemplParams, *tpAniDbgCfgGetPwrTemplParams;

//                                                         
//                                                      
typedef struct sAniDbgCfgInitParams
{
    unsigned int mode; //             
    unsigned int rate; //             
    unsigned int fIsInfMode; //                              
    unsigned int staId;
    unsigned char ownMacAddr[6]; //                               
    unsigned int fFwd2Host; //                              
    unsigned int fUseRealPhy; //                          
    unsigned int fEnablePhyDrops; //                          
    unsigned int dropModeRate; //                                    
    unsigned int dropModeSize; //                                    
    unsigned int dropModeMode; //                                    
} tAniDbgCfgInitParams, *tpAniDbgCfgInitParams;


//                                           
typedef struct sAniDbgCfgGetPwrTemplResponse
{
    unsigned int rc;
    unsigned int val;
} tAniDbgCfgGetPwrTemplResponse, *tpAniDbgCfgGetPwrTemplResponse;


//                                                         
//                                               

typedef union sAniDbgCfgEepByteSetParams
{
    unsigned char mask;
    unsigned char value;

} tAniDbgCfgEepByteSetParams, *tpAniDbgCfgEepByteSetParams;


//                                            
typedef struct sAniDbgCfgEepSetParams
{
    unsigned int offset;
    unsigned int size;
    unsigned int fIsMaskPresent;
    unsigned char setParams[1];
} tAniDbgCfgEepSetParams, *tpAniDbgCfgEepSetParams;

//                                                              
//                                

//                                                             
//                                                            

//                                            
typedef struct sAniDbgCfgEepGetParams
{
    unsigned int offset;
    unsigned int size;
} tAniDbgCfgEepGetParams, *tpAniDbgCfgEepGetParams;


//                                             
typedef struct sAniDbgCfgEepGetResponse
{
    unsigned int rc;
    unsigned char value[1];
} tAniDbgCfgEepGetResponse, *tpAniDbgCfgEepGetResponse;

//                                                         
//                                                    

//                                                      
typedef struct sAniDbgCfgSetNumTransmitters
{
    unsigned int numTransmitters;

} tAniDbgCfgSetNumTransmitters, *tpAniDbgCfgSetNumTransmitters;

//                                                         
//                                              

//                                                
typedef struct sAniDbgCfgSetRxChains
{
    unsigned int numChains;
    unsigned char chainIndices[3];
} tAniDbgCfgSetRxChains, *tpAniDbgCfgSetRxChains;

//                                                         
//                                                   

//                                                     
typedef struct sAniDbgCfgSwClosedLoopTpc
{
    unsigned int action;
} tAniDbgCfgSwClosedLoopTpc, *tpAniDbgCfgSwClosedLoopTpc;

//                                                         
//                                                           
//                                                             
typedef struct sAniDbgCfgSetStaAddrLst
{
    unsigned long numStas;
    unsigned char macAddrLst[MAX_NUM_TST_STAS_PER_AP][6];
} tAniDbgCfgSetStaAddrLst, *tpAniDbgCfgSetStaAddrLst;

//                                                         
//                                                              
//                      
typedef struct sAniDbgCfgGetPwrGainResponse
{
    unsigned int rc;  //            
    unsigned long pwrCode;
    unsigned long gain0;
    unsigned long gain1;
} tAniDbgCfgGetPwrGainResponse, *tpAniDbgCfgGetPwrGainResponse;

//                                                         
//                                                                                   
typedef struct sAniDbgCfgGetPwrGainParams
{
    unsigned long staID;
} tAniDbgCfgGetPwrGainParams, *tpAniDbgCfgGetPwrGainParams;


//                                                         
//                                      

//                                                          
//                                               
typedef struct sAniDbgCfgGetTempRsp
{
    unsigned int rc;     //                              
    unsigned int temp0;  //                    
    unsigned int temp1;  //                    
} tAniDbgCfgGetTempRsp, *tpAniDbgCfgGetTempRsp;


//                                                         
//                                               
typedef struct sAniDbgCfgResponse
{
    unsigned int rc;  //                              
} tAniDbgCfgResponse, *tpAniDbgCfgResponse;

//                                                                     
/*
                                                                    
                                                                      
                                 

                                            

                                          
    
                     
                             
                              

                                            
    
                       
                                                                
                 
                                                                   
                                

                                                 */
//                                                                      

#define ANI_CFG_OPER_GET    0x0
#define ANI_CFG_OPER_SET    0x1


typedef struct sAniDbgCfg
{
    unsigned char oper;
    unsigned char data[124]; //                                            
} tAniDbgCfg;

//                                                                     

//                                     
//                              
typedef struct sAniDbgInitPduTestResponse
{
    //                  
    unsigned int rc;  //                                                  
                      //            

    unsigned int expected;
    unsigned int i_current;
    unsigned int totalPduCount;
} tAniDbgInitPduTestResponse, *tpAniDbgInitPduTestResponse;


//                                                         
//                                                     
//                              

typedef struct sAniDbgRegReadWriteTestResponse
{
    //                  
    unsigned int rc;  //                                       
    unsigned int registerAddress;
    unsigned int expected;
    unsigned int i_current;
} tAniDbgRegReadWriteTestResponse, *tpAniDbgRegReadWriteTestResponse;

//                                                         
//                                       
//                              

typedef struct sAniDbgIntrTestResponse
{
    //                  
    unsigned int rc;       //                                       
    unsigned int status;   //                          
    unsigned int mask;     //                       
    unsigned int bmu;      //                                

} tAniDbgIntrTestResponse, *tpAniDbgIntrTestResponse;

//                                                         
//                              

typedef struct sAniDbgMemoryTestParams
{
    unsigned int startAddress;
    unsigned int endAddress;
    unsigned int pattern;
    unsigned int testLevel;
} tAniDbgMemoryTestParams, *tpAniDbgMemoryTestParams;

//                                                                      

//                                          

typedef struct sAniDbgMemoryTestResponse
{
    //                  
    unsigned int rc;  //                                                  
    //            
    unsigned int expected;
    unsigned int i_current;
    unsigned int currentAddress;
    unsigned int testAddress;
} tAniDbgMemoryTestResponse, *tpAniDbgMemoryTestResponse;

//                                                         
//                                                   

typedef struct sAniDbgHIFBurstRdWrtTestParams
{
    unsigned int AHBAddr;
    unsigned int size;
    unsigned char data[1];

} tAniDbgHIFBurstRdWrtTestParams, *tpAniDbgHIFBurstRdWrtTestParams;

//                                                                      

//                                                     

typedef struct sAniDbgHIFBurstRdWrtTestResponse
{
    //                  
    unsigned int rc;  //                                                  
    //            
    unsigned char readData[1];

} tAniDbgHIFBurstRdWrtTestResponse, *tpAniDbgHIFBurstRdWrtTestResponse;


//                                                         

//                                                                     

typedef struct sAniDbgSendContinuousTestParams
{
    unsigned int  routingFlags;

    //                                                             
    unsigned int  numFramesToSend;

    //                                                 
    unsigned int  fUseTMRing;

    //                                     
    unsigned int fLoopBkPkts;
} tAniDbgSendContinuousTestParams, *tpAniDbgSendContinuousTestParams;

//                                            

typedef struct sAniDbgSendContinuousTestResponse
{
    unsigned int rc;  //                                           

    unsigned int numPktsSent;
    unsigned int numPktsFailed;
    //                                                                  
    unsigned int lastFailedPayloadSize;
} tAniDbgSendContinuousTestResponse, *tpAniDbgSendContinuousTestResponse;

//                                                         
//                                             
typedef struct sAniDbgPktRoutingTestParams
{
    //                           
    unsigned int  routingFlags;

    //                           
    unsigned int hsBit;

    //                                      
    unsigned int  ackPolicy;

    //                   
    unsigned int  rtsFlag;

    //                                                             
    unsigned int  numPktsToSend;

    //                                     
    unsigned int  frameType;

    //                                               
    unsigned int  frmSubType;

    //                                                 
    unsigned int  fUseTMRing;

    //                         
    unsigned int  payloadSize;

    //                                                           
    unsigned int  fragSize;

    //                                                                        
    //                                                            
    unsigned int  fragment;

    //                                           
    unsigned int staId;

    //                                          
    unsigned int tcId;

    //                                                         
    unsigned int staRange;

    //                                                       
    unsigned int tcRange;

    //                  
    unsigned int burst;

    //                       
    unsigned int crc;

    //                                            
    unsigned int fLoopBkPkts;

} tAniDbgPktRoutingTestParams, *tpAniDbgPktRoutingTestParams;

//                                                            
//                         

typedef struct sAniDbgSendPktResponse
{
    //                   
    unsigned int pktGenCount;

    unsigned int fragCnt;

    unsigned int byteCnt;

    unsigned int lowPduCnt;

    unsigned int qFullCnt;

    //                                                          
    unsigned int lastPayloadSize;

    //                                                     
    unsigned int lastFragSize;

    //              
    unsigned int cumImmAck;

    //      
    unsigned int cumNoAck;

    //    
    unsigned int cumRTS;

    //       
    unsigned int cumNoRTS;

    //                    
    unsigned int cumTC[8];

    //                           
    unsigned int cumFrag[16];

    //                           
    unsigned int cumSTA[4];
    unsigned int cumSTAFrags[4];

} tAniDbgSendPktResponse, *tpAniDbgSendPktResponse;

//                                                
typedef struct sAniDbgPktRoutingTestResponse
{
    //                  
    unsigned int rc;  //                                                  
    //            

    //                                                       
    //                         
    tAniDbgSendPktResponse sendPktsRsp;

} tAniDbgPktRoutingTestResponse, *tpAniDbgPktRoutingTestResponse;

//                                                         

typedef enum sPciCfgTestStatus
{
    ePCI_CFG_TEST_SUCCESS,
    ePCI_CFG_TEST_READ_FAILURE,
    ePCI_CFG_TEST_VEN_DEV_ID_MISMATCH,
    ePCI_CFG_TEST_MIN_GNT_MISMATCH,
    ePCI_CFG_TEST_MAX_LAT_MISMATCH,
    ePCI_CFG_TEST_CIS_PTR_MISMATCH,
    ePCI_CFG_TEST_CIS_CONTENTS_MISMATCH
} tPciCfgTestStatus;

//                                            
typedef struct sAniDbgPciCfgTestResponse
{
    //                  
    unsigned int rc;  //                                                  
    //            

    //                                                                      
    unsigned int pciConfig[MAX_PCI_CFG_WRDS];

} tAniDbgPciCfgTestResponse, *tpAniDbgPciCfgTestResponse;

//                                                         

//                                                   
typedef enum sEepromTestStatus
{
    eEEPROM_TEST_SUCCESS,
    eEEPROM_TEST_FILE_OPEN_FAILURE,
    eEEPROM_TEST_FILE_MAP_FAILURE,
    eEEPROM_TEST_INVALID_FILE_SIZE,
    eEEPROM_TEST_MEMORY_ALLOC_FAILURE,
    eEEPROM_TEST_CRC_MISMATCH_FAILURE
} tEepromTestStatus;


//                                                  

typedef struct sAniDbgEepromTestParams
{
    //                  
    char  eepromFilename[256];

} tAniDbgEepromTestParams, *tpAniDbgEepromTestParams;

//                                            

typedef struct sAniDbgEepromTestResponse
{
    unsigned int rc;  //                                           

} tAniDbgEepromTestResponse, *tpAniDbgEepromTestResponse;

//                                                                      

//                                    

typedef struct sAniDbgRhpHashTblMultipleEntryTestParams
{
    unsigned int n; //                  

} tAniDbgRhpHashTblMultipleEntryTestParams,
    *tpAniDbgRhpHashTblMultipleEntryTestParams;

typedef struct sAniDbgRhpHashTblTestParams
{
    unsigned int  staId;
    unsigned char macAddr[6];
    unsigned int  flags;
    unsigned int  hashFlagRsvd;
    unsigned int  rsvdField;

} tAniDbgRhpHashTblTestParams, *tpAniDbgRhpHashTblTestParams;


typedef struct sAniDbgRhpHashTblTestResponse
{
    unsigned int rc;  //              
} tAniDbgRhpHashTblTestResponse, *tpAniDbgRhpHashTblTestResponse;


//                                                            

//                                                                 
//                                

typedef struct sANI_DBG_TEST_INFO {

    tANI_TEST_ID testId;

    //                                                          
    //                                                      
    //                                           
    //                                                 
    tANI_TEST_TRIG_TYPE testTriggerType;

    //           
    tANI_DBG_TEST_STATE testState;

    //                                                         
    //                                                        
    //           
    unsigned long sizeOfParms;
    unsigned char parms[MAX_PARMS_SIZE];

    //                                                       
    //                                                       
    //                                                     
    //        
    unsigned long sizeOfResponse;
    unsigned char response[MAX_RESPONSE_SIZE];

} tANI_DBG_TEST_INFO;

//                                                                
//                                                                  
//                                                      
#ifdef __cplusplus
extern "C" void* dvtGetFuncPtr(void *, int, int );
#else
extern void* dvtGetFuncPtr(void *, int, int );
#endif


//                                                                       
typedef void t_DbgTestRoutine(void *);

//                                             
extern void dbgTestCompleted(void *mpAdapterPtr);


//                                              

#define ANI_DBG_GRP_INFO_TYPE_MISC      0x00000001
#define ANI_DBG_GRP_INFO_TYPE_BMU       0x00000002
#define ANI_DBG_GRP_INFO_TYPE_TFP       0x00000004
#define ANI_DBG_GRP_INFO_TYPE_RHP       0x00000008
#define ANI_DBG_GRP_INFO_TYPE_RFP       0x00000010
#define ANI_DBG_GRP_INFO_TYPE_STA       0x00000020
#define ANI_DBG_GRP_INFO_TYPE_FPHY      0x00000040
#define ANI_DBG_GRP_INFO_TYPE_FPHY_FIFO 0x00000080
#define ANI_DBG_GRP_INFO_TYPE_RPHY      0x00000100
#define ANI_DBG_GRP_INFO_TYPE_HCF       0x00000200
#define ANI_DBG_GRP_INFO_TYPE_SP        0x00000400
#define ANI_DBG_GRP_INFO_TYPE_CP        0x00000800


#define ANI_DBG_GRP_INFO_TYPE_ALL       (ANI_DBG_GRP_INFO_TYPE_MISC |   \
                                     ANI_DBG_GRP_INFO_TYPE_BMU |        \
                                     ANI_DBG_GRP_INFO_TYPE_TFP |        \
                                     ANI_DBG_GRP_INFO_TYPE_RHP |        \
                                     ANI_DBG_GRP_INFO_TYPE_RFP |        \
                                     ANI_DBG_GRP_INFO_TYPE_STA |        \
                                     ANI_DBG_GRP_INFO_TYPE_FPHY|        \
                                     ANI_DBG_GRP_INFO_TYPE_FPHY_FIFO |  \
                                     ANI_DBG_GRP_INFO_TYPE_RPHY |       \
                                     ANI_DBG_GRP_INFO_TYPE_HCF |        \
                                     ANI_DBG_GRP_INFO_TYPE_SP)



typedef struct sANI_DBG_MISC_INFO {

    unsigned long sysMode;
    unsigned long sysIntrMask;
    unsigned long intrMask;
    unsigned long phyIntrMask;
    unsigned long intrStatus[32];
    unsigned long phyIntrStatus[16];
    unsigned long eofSofExceptionResets;  
    unsigned long bmuExceptionResets;      
    unsigned long lowPduExceptionResets;  
    unsigned long userTriggeredResets;
    unsigned long logPExceptionResets;

} tANI_DBG_MISC_INFO;


typedef struct sANI_DBG_BMU_INFO {

    unsigned long control;
    unsigned long fp_hptr;
    unsigned long tptr;
    unsigned long pdu;
    unsigned long exception;
    unsigned long exceptionMaster;
    unsigned long dropCount;
    unsigned long workQueue[10][4];

} tANI_DBG_BMU_INFO;


typedef struct sANI_DBG_TFP_INFO {

    unsigned long control;
    unsigned long modeEnable;
    unsigned long templEnable;
    unsigned long retryQid;
    unsigned long tsfHi;
    unsigned long tsfLo;
    unsigned long beacon;
    unsigned long probeDelay;
    unsigned long tbttHi;
    unsigned long tbttLo;
    unsigned long nav;
    unsigned long listenInterval;
    unsigned long delayTx;
    unsigned long dtimPeriod;
    unsigned long rtsCount;
    unsigned long rtsFailure;

} tANI_DBG_TFP_INFO;


typedef struct sANI_DBG_RHP_INFO {

    unsigned long sof;
    unsigned long sof_chunk;
    unsigned long fragCount;
    unsigned long dropCount;
    unsigned long fcsCount;
    unsigned long bssIdMismatch;
    unsigned long destMismatch;
    unsigned long lengthError;
    unsigned long pduError;
    unsigned long abortCount;
    unsigned long reqRate;
    unsigned long delayAB;
    unsigned long macAddrHi;
    unsigned long macAddrLo;
    unsigned long bssIdHi;
    unsigned long bssIdLo;
    unsigned long relayCount;
    unsigned long hash_MissCount;
    unsigned long hash_srcHi;
    unsigned long hash_srcLo;
    unsigned long hash_type;
    unsigned long hash_subType;
    unsigned long dbg_hangStatus;
    unsigned long dbg_fragIgnoreCount;
    unsigned long pduCount;

} tANI_DBG_RHP_INFO;


typedef struct sANI_DBG_RFP_INFO {

    unsigned long packets;
    unsigned long multicastPackets;
    unsigned long dupPackets;
    unsigned long byteCount;
    unsigned long dropCount;
    unsigned long byte64;
    unsigned long byte128;
    unsigned long byte256;
    unsigned long byte512;
    unsigned long byte1024;
    unsigned long byte1519;
    unsigned long byte2048;
    unsigned long byte4096;

} tANI_DBG_RFP_INFO;

typedef struct sANI_DBG_SP_INFO {

    unsigned long wep_dky0_w0;
    unsigned long wep_dky0_w1;
    unsigned long wep_default_rc0;

    unsigned long wep_dky1_w0;
    unsigned long wep_dky1_w1;
    unsigned long wep_default_rc1;

    unsigned long wep_dky2_w0;
    unsigned long wep_dky2_w1;
    unsigned long wep_default_rc2;

    unsigned long wep_dky3_w0;
    unsigned long wep_dky3_w1;
    unsigned long wep_default_rc3;

} tANI_DBG_SP_INFO;

typedef struct sANI_DBG_CP_INFO {

    unsigned long cp_control;
    unsigned long Compression_Expansion_Cnt;
    unsigned long Compression_NUM_pkts;

    unsigned long Decompression_NUM_pkts;
    unsigned long Compression_50p_NUM_pkts;
    unsigned long CP_Error_status;

    unsigned long Cp_maximum_pkt_len;
 
} tANI_DBG_CP_INFO;


typedef struct sANI_DBG_STA_TX_WQ_INFO {

    unsigned long txWqAddr;
    unsigned long txWqDump[4];

    unsigned long tptr;
    unsigned long hptr;
    unsigned long aptr;
    unsigned long a_tpkts;
    unsigned long h_tpkts;
    unsigned long frag;
    unsigned long bytes;
    unsigned long ack;
    unsigned long valid;

} tANI_DBG_STA_TX_WQ_INFO;



typedef struct sANI_DBG_TC_DESC {

    unsigned long valid;
    unsigned long rxAckType;
    unsigned long newPkt;
    unsigned long rxSeqNum;
    unsigned long rxPktTimeStamp;
    unsigned long SV;
    unsigned long ackTimeout;
    unsigned long numOfFragsSucessful;
    unsigned long rxBDPtr;
    unsigned long txReplayCountHi;
    unsigned long txReplayCountLo;
    unsigned long rxReplayCountHi;
    unsigned long rxReplayCountLo;

} tANI_DBG_TC_DESC;


typedef struct sANI_DBG_PWR_TEMPL {

    unsigned long retryPhyMode;
    unsigned long retryCb;
    unsigned long retryEsf;
    unsigned long sb;
    unsigned long rate;
    unsigned long esf;
    unsigned long tifs;
    unsigned long edcf;
    unsigned long cb;
    unsigned long mode;
    unsigned long pwrLvl;
    unsigned long nTransmitters;
    unsigned long retry1rate;
    unsigned long retry2rate;

    unsigned long pwrTemplate; //            

} tANI_DBG_PWR_TEMPL;


typedef struct sANI_DBG_STA {

    unsigned long staDescAddr;
    unsigned long staDump[256];

    tANI_DBG_TC_DESC tcDesc[9]; //                         

    unsigned long cbits_hcf;
    unsigned long cbits_ps;
    unsigned long cbits_ps1;
    unsigned long cbits_tx_en;

    unsigned long descStat_aes_sent;
    unsigned long descStat_aes_recv;
    unsigned long descStat_replays;
    unsigned long descStat_formaterr;
    unsigned long descStat_aes_decypterr_default;
    unsigned long descStat_aes_decypterr_ucast;

    unsigned long tfpStat_failed;
    unsigned long tfpStat_retry;
    unsigned long tfpStat_multiretry;
    unsigned long tfpStat_ackto;
    unsigned long tfpStat_frags;
    unsigned long tfpStat_rtsBrqs;
    unsigned long tfpStat_pkts;
    unsigned long tfpStat_ctsBackTimeouts;


    unsigned long phyStatHi;
    unsigned long phyStatLo;

    unsigned long ackToNonPrimRates;
    unsigned long nFragSuccNonPrimRates;

} tANI_DBG_STA;


typedef struct sANI_DBG_STA_INFO {

    unsigned long staId;    //      

    tANI_DBG_STA sta;

    tANI_DBG_STA_TX_WQ_INFO txwq[8];

    tANI_DBG_PWR_TEMPL pwrTempl;

} tANI_DBG_STA_INFO;


typedef struct sANI_DBG_FPHY_INFO {

    unsigned long fphy_symPer;
    unsigned long cca_delayOffset;
    unsigned long cca_startDelay;
    unsigned long timeStamp_Hi;
    unsigned long timeStamp_Lo;
    unsigned long dropRate;
    unsigned long burstSize;
    unsigned long reg;
    unsigned long stat_bytes01;
    unsigned long stat_bytes23;
    unsigned long stat_bytes45;
    unsigned long pkts_tx;
    unsigned long pkts_rx;
    unsigned long pkts_drops;
    unsigned long rxin_sof;
    unsigned long rxin_eof;
    unsigned long rxout_sof;
    unsigned long rxout_eof;
    unsigned long txin_sof;
    unsigned long txin_eof;
    unsigned long txout_sof;
    unsigned long txout_eof;

} tANI_DBG_FPHY_INFO;


#define MAX_FIFO_ENTRIES_PER_REQUEST 200

typedef struct sAniDvtPhyfEntry
{
    unsigned char bytes[50];
    unsigned char len;
    unsigned char mode;
    unsigned char phyLen;
    unsigned char macLen;
    unsigned char dropByte;
    unsigned char reserved;
    unsigned long sof;
    unsigned long eof;
} tAniDvtPhyfEntry;


#define ANI_DBG_FIFO_CMD_GET    0x1
#define ANI_DBG_FIFO_CMD_CLEAR  0x2


typedef struct sANI_DBG_FPHY_FIFO {

    unsigned long command; //                                    
    unsigned long entries; //                
    unsigned long offset; //                    

    unsigned long totalBytes;
    unsigned long approxEntries;

    unsigned long validEntries;
    tAniDvtPhyfEntry Fifo[MAX_FIFO_ENTRIES_PER_REQUEST];

} tANI_DBG_FPHY_FIFO;


typedef struct sANI_DBG_RPHY_MPI_INFO {

    unsigned long tfp_phy_sof;
    unsigned long tfp_phy_eof;
    unsigned long phy_tfp_req;
    unsigned long txa_mpi_data_req;
    unsigned long txb_mpi_data_req;
    unsigned long mpi_txa_data_val;
    unsigned long mpi_txb_data_val;
    unsigned long mpi_txa_pktend;
    unsigned long mpi_txb_pktend;
    unsigned long mpi_txctl_pktend;
    unsigned long mpi_txctl_ctlbytes_val;

} tANI_DBG_RPHY_MPI_INFO;


typedef struct sANI_DBG_RPHY_PMI_INFO {

    unsigned long rxa_mpi_pktstart;
    unsigned long rxb_mpi_pktstart;
    unsigned long rxa_mpi_pktend;
    unsigned long rxb_mpi_pktend;
    unsigned long rxa_mpi_data_val;
    unsigned long rxb_mpi_data_val;
    unsigned long rhp_phy_shutoff;
    unsigned long rhp_phy_sof_c;
    unsigned long rhp_phy_sof_p;
    unsigned long rhp_phy_eof_c;
    unsigned long rhp_phy_eof_p;
    unsigned long phy_rhp_data_val;
    unsigned long pmi_int;

} tANI_DBG_RPHY_PMI_INFO;

typedef struct sANI_DBG_RPHY_PHYINT_INFO {

    unsigned long status;
    unsigned long fast_mask;
    unsigned long slow_mask;
    unsigned long host_mask;

} tANI_DBG_RPHY_PHYINT_INFO;


typedef struct sANI_DBG_RPHY_INFO {

    tANI_DBG_RPHY_MPI_INFO mpi;
    tANI_DBG_RPHY_PMI_INFO pmi;
    tANI_DBG_RPHY_PHYINT_INFO phyint;

} tANI_DBG_RPHY_INFO;


typedef struct sANI_DBG_SCH_INFO
{
    unsigned long curSch; //   
    unsigned long numSch; //   
    unsigned long numInt; //   
    unsigned long numEndInt; //   
    unsigned long numCFB; //   
    unsigned long firstCFB; //   

    unsigned long fixedSch; //   
    unsigned long gDvtPoll; //   
    unsigned long maxTimeout; //   
    unsigned long minTxop; //   
    unsigned long maxTxop; //   
    unsigned long maxTcid; //   
    unsigned long maxSta; //   
    unsigned long minSta; //   
    unsigned long maxInst; //   

    unsigned long firstSch; //   
    unsigned long cfbStart; //   
    unsigned long cfbEnd; //   
    unsigned long cumCFB; //   
    unsigned long cumCP; //   

    unsigned long haltSch; //   
    unsigned long numTim; //   
} tANI_DBG_SCH_INFO;


typedef struct sANI_DBG_HCF_INFO {

    unsigned long       bSetInfo;     //                            
    tANI_DBG_SCH_INFO   schInfo;

} tANI_DBG_HCF_INFO;

typedef struct sANI_DBG_AP_SWITCH_INFO {


    //                                          
    unsigned char   SSID_BSSID_BUF[262]; //                                                    


} tANI_DBG_AP_SWITCH_INFO;


typedef struct sANI_DBG_INFO {

    unsigned long dbgInfoMask; //                                  

    tANI_DBG_MISC_INFO miscInfo;
    tANI_DBG_BMU_INFO bmuInfo;
    tANI_DBG_TFP_INFO tfpInfo;
    tANI_DBG_RHP_INFO rhpInfo;
    tANI_DBG_RFP_INFO rfpInfo;
    tANI_DBG_STA_INFO staInfo;
    tANI_DBG_FPHY_INFO fphyInfo;
    tANI_DBG_FPHY_FIFO fphyFifo;
    tANI_DBG_RPHY_INFO rphyInfo;
    tANI_DBG_HCF_INFO hcfInfo;
    tANI_DBG_SP_INFO spInfo;
    tANI_DBG_CP_INFO cpInfo;

} tANI_DBG_INFO;

extern tSirRetStatus dvtGetDumpInfo(void *pMac, tANI_DBG_INFO *pDbgInfo );

extern tSirRetStatus dvtGetConfigInfo(void *pMac, tANI_DBG_TEST_INFO *pTestInfo );

#ifdef __cplusplus
extern "C" void dvtSetStopTestFlag( unsigned char stopTest );
#else
extern void dvtSetStopTestFlag( unsigned char stopTest );
#endif


#endif //               

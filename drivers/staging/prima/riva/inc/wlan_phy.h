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

#ifndef WLAN_PHY_H
#define WLAN_PHY_H
/*============================================================================ 
@file wlan_phy.h 

Contains definitions of all PHY related structures that aree needed by FTM/PTT

Copyright (c) 2007 Qualcomm Technologies, Inc. All Rights Reserved. 
Qualcomm Technologies Proprietary and Confidential

============================================================================*/
#include <wlan_nv.h>
 
/*                                                                                   
                                                             */
typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8 currentCalibration;
} sCalibrationInfo;

typedef PACKED_PRE struct PACKED_POST {
    tANI_S16 I;   //                        
    tANI_S16 Q;   //                        
}tIQSamples;

typedef tIQSamples tIQAdc;
typedef tIQSamples tIQDac;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8 maxGainIndex;
    tANI_U8 topGainDb;
    tANI_U8 bottomGainDb;
    tANI_U8 unused[1];
}tAsicAgc;

#define TXFIR_MEM                           QWLAN_TXFIR_TXCAL_MEM0_MREG
#define TXFIR_MEM_GAIN_MULT                 (16)    //              
#define TXFIR_MEM_PER_CHAIN                 (16 * TXFIR_MEM_GAIN_MULT)    //                                     

typedef tIQAdc tTxCarrierError;

#define ONE_MICROSECOND             (160)
#define DEFAULT_INTERFRAME_SPACE    (ONE_MICROSECOND * 10)  //               

typedef enum {
   PHYDBG_TX_IDLE = 0,
   PHYDBG_TX_START = 1,
   PHYDBG_TX_WARMUP = 2,
   PHYDBG_TX_INFD = 3,
   PHYDBG_TX_CMD = 4,
   PHYDBG_TX_SVC = 5,
   PHYDBG_TX_PYLDF = 6,
   PHYDBG_TX_PYLDR = 7,
   PHYDBG_TX_CRC = 8,
   PHYDBG_TX_FLUSH = 9,
   PHYDBG_TX_TXDONEWAIT = 10,
   PHYDBG_TX_TIFWAIT = 11
} ePhyDbgTxStatus;

typedef enum {
   PHYDBG_PREAMBLE_OFDM,
   PHYDBG_PREAMBLE_GREENFIELD,
   PHYDBG_PREAMBLE_MIXED,
   PHYDBG_PREAMBLE_SHORTB,
   PHYDBG_PREAMBLE_LONGB,

   PHYDBG_LDPC_PREAMBLE_OFDM = 0x10,
   PHYDBG_LDPC_PREAMBLE_GREENFIELD = 0x11,
   PHYDBG_LDPC_PREAMBLE_MIXED = 0x12
} ePhyDbgPreamble;


//        
#ifdef VERIFY_HALPHY_SIMV_MODEL
#define GRAB_RAM_DBLOCK_SIZE  (256) //                                 
#else
#define GRAB_RAM_DBLOCK_SIZE         (1024)  //                                 
#endif

#define MAX_REQUESTED_GRAB_RAM_SAMPLES 256    //                                
#define GRAB_RAM_BUFFER_DEPTH       (4*1024)   //                                     
#define LAST_GRAB_RAM_SAMPLE_INDEX (GRAB_RAM_BUFFER_DEPTH - 1)


typedef PACKED_PRE struct PACKED_POST {
   tIQAdc rx0;
} tGrabRamSample;


enum {
   GRABRAM_RAWADC = 0,
   GRABRAM_POSTIQ
};
typedef tANI_U32 eGrabRamSampleType;

typedef tANI_S8  tANI_S6;
typedef tANI_S16 tANI_S9;
typedef tANI_S16 tANI_S10;
typedef tANI_S16 tANI_S12;
typedef tANI_U16 tANI_U10;



//                                                                             
typedef tANI_S16 t2Decimal;
#define CONVERT_TO_2DECIMAL_PLACES(x)   (x * 100)
#define CONVERT_FROM_2DECIMAL_PLACES(x) (x / 100)

#ifndef PTT_FLOAT
#define PTT_FLOAT tANI_U32  //                                  
//                                                                             
#endif

typedef enum
{
    PHY_RX_CHAIN_0 = 0,

    PHY_MAX_RX_CHAINS = 1,
    PHY_ALL_RX_CHAINS,
    PHY_NO_RX_CHAINS
}ePhyRxChains;

typedef enum
{
    PHY_I_RAIL = 0,
    PHY_Q_RAIL = 1,
    PHY_NUM_IQ_RAILS
}ePhyIQ;

//                                  

enum
{
    TX_GAIN_STEP_0,
    TX_GAIN_STEP_1,
    TX_GAIN_STEP_2,
    TX_GAIN_STEP_3,
    TX_GAIN_STEP_4,
    TX_GAIN_STEP_5,
    TX_GAIN_STEP_6,
    TX_GAIN_STEP_7,
    TX_GAIN_STEP_8,
    TX_GAIN_STEP_9,
    TX_GAIN_STEP_10,
    TX_GAIN_STEP_11,
    TX_GAIN_STEP_12,
    TX_GAIN_STEP_13,
    TX_GAIN_STEP_14,
    TX_GAIN_STEP_15,
    TX_GAIN_STEP_16,
    TX_GAIN_STEP_17,
    TX_GAIN_STEP_18,
    TX_GAIN_STEP_19,
    TX_GAIN_STEP_20,
    TX_GAIN_STEP_21,
    TX_GAIN_STEP_22,
    TX_GAIN_STEP_23,
    TX_GAIN_STEP_24,
    TX_GAIN_STEP_25,
    TX_GAIN_STEP_26,
    TX_GAIN_STEP_27,
    TX_GAIN_STEP_28,
    TX_GAIN_STEP_29,
    TX_GAIN_STEP_30,
    TX_GAIN_STEP_31,

    RX_GAIN_STEP_0   = 0,
    RX_GAIN_STEP_1,
    RX_GAIN_STEP_2,
    RX_GAIN_STEP_3,
    RX_GAIN_STEP_4,
    RX_GAIN_STEP_5,
    RX_GAIN_STEP_6,
    RX_GAIN_STEP_7,
    RX_GAIN_STEP_8,
    RX_GAIN_STEP_9,
    RX_GAIN_STEP_10,
    RX_GAIN_STEP_11,
    RX_GAIN_STEP_12,
    RX_GAIN_STEP_13,
    RX_GAIN_STEP_14,
    RX_GAIN_STEP_15,

    NUM_TX_GAIN_STEPS = 32,
    MAX_TX_GAIN_STEP = TX_GAIN_STEP_31,

    NUM_RX_GAIN_STEPS = 16,
    MAX_RX_GAIN_STEP = RX_GAIN_STEP_15,
};
typedef tANI_U32 eGainSteps;


//                  
#define DPD_RESPONSE_SIZE 128
typedef PACKED_PRE struct PACKED_POST {
    tANI_U8  dpdCalFailCnt;                     //                                         
    tANI_U8  dpdCalSuccessCnt;                  //                                         
    tANI_U8  dpdColdBootRepeatCalStatus;
    tANI_U8  dpdLastIteration;
    tANI_S16 dpd_threshold[DPD_RESPONSE_SIZE];
    tANI_S16 dpd_aoffset[DPD_RESPONSE_SIZE];
    tANI_S16 dpd_again[DPD_RESPONSE_SIZE];
    tANI_S16 dpd_poffset[DPD_RESPONSE_SIZE];
    tANI_S16 dpd_pgain[DPD_RESPONSE_SIZE];
    tANI_S32 dpd_sample[20];
    tANI_U8  dpd_try;
    tANI_U8  band;
}sDPDcorrectionCalValues;

typedef PACKED_PRE struct PACKED_POST {
    sDPDcorrectionCalValues dpd[PHY_MAX_TX_CHAINS];
}sTxChainsDPDCalValues;


//                                         
typedef PACKED_PRE struct PACKED_POST {
    tANI_S9 coeff_i[5];
    tANI_S9 coeff_q[5];
}sIQCalValues;

//                    
typedef PACKED_PRE struct PACKED_POST {
    tANI_S9 iq_ampimb_coeff;
    tANI_S16 txloleakage_i;     //                                 
    tANI_S16 txloleakage_q;     //                                 
}sTXIQCalValues;

//                    
typedef PACKED_PRE struct PACKED_POST {
    tANI_S9 iqphaseimb_coeff_i[5];
    tANI_S9 iqphaseimb_coeff_q[5];
}sTXIQPhaseImbCalValues;

typedef PACKED_PRE struct PACKED_POST {
    sIQCalValues iq[PHY_MAX_RX_CHAINS];
}sRxChainsIQCalValues;

//                     
typedef PACKED_PRE struct PACKED_POST {
    sTXIQCalValues iq[PHY_MAX_TX_CHAINS];
    sTXIQPhaseImbCalValues iqImb[PHY_MAX_TX_CHAINS];
}sTxChainsIQCalValues;

typedef PACKED_PRE struct PACKED_POST {
    tANI_S9 co_i[3];
    tANI_S9 co_q[3];
}sHKIQCalValues;

typedef PACKED_PRE struct PACKED_POST {
    sHKIQCalValues co[PHY_MAX_TX_CHAINS];
}sTxChainsHKIQCalValues;

typedef PACKED_PRE struct PACKED_POST {
    tANI_U8 lna_code;    //                                         
    tANI_U8 gm_code;     //                                 
}sLnaBandCalValues;

typedef PACKED_PRE struct PACKED_POST {
    sLnaBandCalValues lnaCode[PHY_MAX_RX_CHAINS];
}sTxChainsLnaBandCalValues;

typedef tANI_U16 t_mW;       //          
typedef tANI_U8 tPwrTemplateIndex;   //                                                      

typedef PACKED_PRE struct PACKED_POST {
    tANI_U8 txPowerAdc[PHY_MAX_TX_CHAINS];
}sTxChainsPowerAdcReadings;

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8 agcGain;
}tRxGain;

typedef PACKED_PRE struct PACKED_POST {
    tANI_U8 rx[PHY_MAX_RX_CHAINS];
}sRxChainsData;

typedef sRxChainsData sRxChainsRssi;
typedef sRxChainsData sRxChainsAgcDisable;

typedef PACKED_PRE struct PACKED_POST {
    tANI_BOOLEAN rx[PHY_MAX_RX_CHAINS];
}sRxChainsBoolean;

typedef sRxChainsBoolean sRxChainsAgcEnable;

#define NUM_AGC_GAINS   64
typedef tRxGain sAgcGainLut[NUM_AGC_GAINS];


typedef PACKED_PRE struct PACKED_POST
{
    tANI_S6 iLo;
    tANI_S6 qLo;
}sTxFirLoCorrect;

typedef tIQAdc sTxLoCorrectBB[PHY_MAX_TX_CHAINS][NUM_TX_GAIN_STEPS];

typedef PACKED_PRE struct PACKED_POST
{
    tANI_U32 txIqLoCache[PHY_MAX_TX_CHAINS][NUM_TX_GAIN_STEPS][4];
    tANI_U32 spatialRotation;
}tAsicTxFir;

//               
//                                                                         
//                                                                                          
//                                                                   
#define MAX_TPC_CHANNELS        (NUM_RF_CHANNELS)
#define START_TPC_CHANNEL       (2412)
#define END_TPC_CHANNEL         (2484)

#define MAX_PWR_LUT_DBM (24)
#define MIN_PWR_LUT_DBM (8)


/*                                                                                                     
                                                                                                                 
                                                                                    
                                                                                                                                      
                                                                                                          
                                                                                                           
                                                                                                                                  
                                                                                                          
                                                                                                                  

*/

#define MIN_PWR_LUT_DBM_2DEC_PLACES  (MIN_PWR_LUT_DBM * 100)
#define MAX_PWR_LUT_DBM_2DEC_PLACES  ((MAX_PWR_LUT_DBM * 100) - (1 + (100 * (MAX_PWR_LUT_DBM - MIN_PWR_LUT_DBM))/TPC_MEM_POWER_LUT_DEPTH))

//                                                                                                                             
//                                                           
#define CONVERT_DBM_GINDEX(dbm) (((dbm - MIN_PWR_LUT_DBM) * 32) / (MAX_PWR_LUT_DBM - MIN_PWR_LUT_DBM))

typedef tANI_U8 tTxGainCombo;        //                                                  

typedef PACKED_PRE struct PACKED_POST
{
    tPowerDetect min;
    tPowerDetect max;
}tPwrTemplateRange;



/*
                                                                                                              
                                                                                                                    
                                                                                             
                                                                                                                                             
                                                                                                 
*/

 typedef union
 {
     tTxGainCombo txGain;               //                                              
     tANI_U8 hi8_adjustedPwrDet;        //                                                                           
 }uExtraLutBits;


typedef PACKED_PRE struct PACKED_POST
{
    t2Decimal min;  //                                         
    t2Decimal max;  //                                         
}tPowerdBmRange;        //                                                                     


typedef tANI_U16 tRfADCVal;
typedef tRfADCVal tTempADCVal;

typedef PACKED_PRE struct PACKED_POST
{
    tRfADCVal pdadc_offset;
    tANI_U8 reserved[2];
}tTpcParams;


//                                                                                   
#define CB_RATE_POWER_OFFSET            0
#define CB_RATE_POWER_OFFSET_LAST_INDEX 60  //                                                        

/*                                     */


 typedef PACKED_PRE struct PACKED_POST
 {
     tANI_U8  temperatureAdc;                //                                                     
     tANI_U8  txGain;                        //                                                    
     tANI_U8  pwrDetAdc;                     //                              
     tANI_U8  reserved;
     uAbsPwrPrecision absPowerMeasured;      //                                                          
 }tTpcCalPoint;
 
 
 typedef PACKED_PRE struct PACKED_POST
 {
     tANI_U16 numTpcCalPoints;
     tANI_U16 reserved;
     tTpcCalPoint chain[MAX_TPC_CAL_POINTS];
 }tTpcChainData;
 
 
 typedef PACKED_PRE struct PACKED_POST
 {
     tANI_U16 freq;                                          //                
     tANI_U16 reserved;
     tTpcChainData empirical[PHY_MAX_TX_CHAINS];  //                     
 }tTpcFreqData;
 
 typedef PACKED_PRE struct PACKED_POST
 {
     tANI_U8 numChannels;
     tANI_U8 reserved[3];
     tTpcFreqData calValues[MAX_TPC_CHANNELS];
 }sTpcFreqCalTable;
 
 
typedef PACKED_PRE struct PACKED_POST {
    tPowerDetect lut;                   //                                     
    tANI_U8 reserved[3];
    
    uAbsPwrPrecision abs;               //                                    
}tTxPowerLutOutput;

typedef PACKED_PRE struct PACKED_POST {
    tANI_U8      gain;                  //                                                                            
    tPowerDetect adc;                   //                                                  
    tANI_U16 rawAdc;                   //                            
    
    tTxPowerLutOutput indexMinMatch;    //                                                                          
    tTxPowerLutOutput indexMaxMatch;    //                                                                          
    tTxPowerLutOutput output;           //                                                    
}tTxChainPower;

extern const tRfChannelProps rfChannels[NUM_RF_CHANNELS];

typedef enum
{
    RF_CAL_TONE_28NEG,
    RF_CAL_TONE_24NEG,
    RF_CAL_TONE_20NEG,
    RF_CAL_TONE_16NEG,
    RF_CAL_TONE_12NEG,
    RF_CAL_TONE_8NEG,
    RF_CAL_TONE_4NEG,
    RF_CAL_TONE_4POS,
    RF_CAL_TONE_8POS,
    RF_CAL_TONE_12POS,
    RF_CAL_TONE_16POS,
    RF_CAL_TONE_20POS,
    RF_CAL_TONE_24POS,
    RF_CAL_TONE_28POS,

    NUM_RF_TONES,

    MIN_RF_TONE = RF_CAL_TONE_28NEG,
    MAX_RF_TONE = RF_CAL_TONE_28POS
}eRfTones;

typedef tANI_U8 tDcoCorrect;
typedef tANI_S8 tIm2Correct;

typedef PACKED_PRE struct PACKED_POST {
    tDcoCorrect IDcoCorrect;
    tDcoCorrect QDcoCorrect;
    tANI_U8     dcRange;
}tRxDcoCorrect;

typedef PACKED_PRE struct PACKED_POST {
    tRxDcoCorrect dco[PHY_MAX_RX_CHAINS];
}tRxChainsDcoCorrections;

typedef PACKED_PRE struct PACKED_POST {
    tIm2Correct ICorrect;
    tIm2Correct QCorrect;
}tRxIm2Correct;

typedef PACKED_PRE struct PACKED_POST {
    tRxIm2Correct dco[PHY_MAX_RX_CHAINS];
}tRxChainsIm2Corrections;

typedef PACKED_PRE struct PACKED_POST {
    tDcoCorrect IDcoCorrect;
    tDcoCorrect QDcoCorrect;
}tTxLoCorrect;

typedef PACKED_PRE struct PACKED_POST {
    tTxLoCorrect txLo[PHY_MAX_TX_CHAINS];
}sTxChainsLoCorrections;


//                                                      

#define NUM_RF_RX_GAIN_STEPS    (128)
#define MAX_RF_RX_GAIN_STEP     (NUM_RF_RX_GAIN_STEPS - 1)

#define NUM_RF_TX_GAIN_STEPS    (16)
#define MAX_RF_TX_GAIN_STEP     (NUM_RF_TX_GAIN_STEPS - 1)

#define RF_AGC_GAIN_LUT_DEPTH   (128)
#define NUM_RF_DCO_VALUES       (128) //                                                                                                            
#define MAX_RF_DCO_VALUE        (NUM_RF_DCO_VALUES - 1)


typedef PACKED_PRE struct PACKED_POST
{
    tANI_U16 gainReg1;   //                                                      
}tRfRxGain;


typedef PACKED_PRE struct PACKED_POST
{
    tANI_U16 bbf_gain_cnt;
    tANI_U16 bbf_lin_adj;
    tANI_U16 lo_mix_da_gain_cntl;
    tANI_U16 pa_gain_cntl;
    tANI_U16 da_pa_bias_1_cnt;
    tANI_U16 da_pa_bias_2_cntl;
}tRfTxGain;
 
typedef PACKED_PRE struct PACKED_POST
{
    //                                  
    tANI_U8 rxIf;
    tANI_U8 txIf;
    tANI_U8 txRf;
    tANI_U8 reserved;
}sRfSpecificFilterSettings;

typedef sRfSpecificFilterSettings sRfChannelFilterSettings[NUM_RF_CHANNELS];


typedef PACKED_PRE struct PACKED_POST
{
    tANI_U8 hdet_ctl_ext_atten;
    tANI_U8 hdet_dcoc_code;
    tANI_U8 hdet_dcoc_ib_rcal_en;
    tANI_U8 hdet_dcoc_ib_scal_en;
}sRfNvCalValues;  //               



typedef enum
{
    SYNTH_UNLOCKED,
    SYNTH_LOCK
}eRfSynthLock;

typedef enum
{
    TEMP_SENSOR_PA,
    TEMP_SENSOR_RX
}eRfTempSensor;

typedef enum
{
    TEMPERATURE_BIN_0,          //          
    TEMPERATURE_BIN_1,          //         
    TEMPERATURE_BIN_2,          //          
    TEMPERATURE_BIN_3,          //           
    NUM_TEMPERATURE_BINS
}eTemperatureBins;

typedef PACKED_PRE struct PACKED_POST {
    tANI_U16 hdetDcocCode;
    tANI_U16 hdetDcoOffset;
}sRfHdetCalValues;

#define TPC_TXPWR_ENABLE_MASK                   QWLAN_TPC_TXPWR_ENABLE_EN_MASK


#define TPC_MEM_TX0_PWR_LUT_OFFSET              QWLAN_TPC_POWERDET0_RAM_MREG
#define TPC_MEM_TX1_PWR_LUT_OFFSET              QWLAN_TPC_POWERDET1_RAM_MREG
#define TPC_MEM_TX2_PWR_LUT_OFFSET              QWLAN_TPC_POWERDET2_RAM_MREG
#define TPC_MEM_TX3_PWR_LUT_OFFSET              QWLAN_TPC_POWERDET3_RAM_MREG
#define TPC_MEM_TX0_GAIN_LUT_OFFSET             QWLAN_TPC_GAIN_LUT0_MREG
#define TPC_MEM_TX1_GAIN_LUT_OFFSET             QWLAN_TPC_GAIN_LUT1_MREG
#define TPC_MEM_TX2_GAIN_LUT_OFFSET             QWLAN_TPC_GAIN_LUT2_MREG
#define TPC_MEM_TX3_GAIN_LUT_OFFSET             QWLAN_TPC_GAIN_LUT3_MREG

//                                        
#define TPC_POWERDET_MASK                      QWLAN_TPC_POWERDET0_RAM_POWER_MASK
#define TPC_GAIN_RF_MASK                       QWLAN_TPC_GAIN_LUT0_RF_GAIN_MASK
#define TPC_GAIN_RF_OFFSET                     QWLAN_TPC_GAIN_LUT0_RF_GAIN_OFFSET
#define TPC_GAIN_DIG_MASK                      QWLAN_TPC_GAIN_LUT0_DIG_GAIN_MASK


#define TPC_MEM_GAIN_LUT_DEPTH                  32


#define TPC_ADC_CTRL_REG                        QWLAN_TPC_ADC_CTRL_GET_ADC_REG
#define TPC_ADC_GET_MASK                        QWLAN_TPC_ADC_CTRL_GET_ADC_GET_ADC_MASK

#define TPC_ADC_FAILED_MASK                     QWLAN_TPC_ADC_STATUS_FAILED_MASK
#define TPC_ADC_BUSY_P_MASK                     QWLAN_TPC_ADC_STATUS_BUSY_P_MASK
#define TPC_ADC_BUSY_T_MASK                     QWLAN_TPC_ADC_STATUS_BUSY_T_MASK


#define MSK_1   0x1
#define MSK_2   0x3
#define MSK_3   0x7
#define MSK_4   0xF
#define MSK_5   0x1F
#define MSK_6   0x3F
#define MSK_7   0x7F
#define MSK_8   0xFF
#define MSK_9   0x1FF
#define MSK_10  0x3FF
#define MSK_11  0x7FF
#define MSK_12  0xFFF
#define MSK_13  0x1FFF
#define MSK_14  0x3FFF
#define MSK_15  0x7FFF
#define MSK_16  0xFFFF
#define MSK_17  0x1FFFF
#define MSK_18  0x3FFFF
#define MSK_19  0x7FFFF
#define MSK_20  0xFFFFF
#define MSK_21  0x1FFFFF
#define MSK_22  0x3FFFFF
#define MSK_23  0x7FFFFF
#define MSK_24  0xFFFFFF
#define MSK_25  0x1FFFFFF
#define MSK_26  0x3FFFFFF
#define MSK_27  0x7FFFFFF
#define MSK_28  0xFFFFFFF
#define MSK_29  0x1FFFFFFF
#define MSK_30  0x3FFFFFFF
#define MSK_31  0x7FFFFFFF
#define MSK_32  0xFFFFFFFF


#define COARSE_GAIN_MASK    MSK_4
#define COARSE_GAIN_OFFSET  4
#define FINE_GAIN_MASK      MSK_4   //                                                                                   
#define FINE_GAIN_OFFSET    0

typedef enum
{
    TPC_COARSE_TXPWR_0,
    TPC_COARSE_TXPWR_1,
    TPC_COARSE_TXPWR_2,
    TPC_COARSE_TXPWR_3,
    TPC_COARSE_TXPWR_4,
    TPC_COARSE_TXPWR_5,
    TPC_COARSE_TXPWR_6,
    TPC_COARSE_TXPWR_7,
    TPC_COARSE_TXPWR_8,
    TPC_COARSE_TXPWR_9,
    TPC_COARSE_TXPWR_10,
    TPC_COARSE_TXPWR_11,
    TPC_COARSE_TXPWR_12,
    TPC_COARSE_TXPWR_13,
    TPC_COARSE_TXPWR_14,
    TPC_COARSE_TXPWR_15,
    TPC_COARSE_TXPWR_16,
    TPC_COARSE_TXPWR_17,
    TPC_COARSE_TXPWR_18,
    TPC_COARSE_TXPWR_19,
    TPC_COARSE_TXPWR_20,
    TPC_COARSE_TXPWR_21,
    TPC_COARSE_TXPWR_22,
    TPC_COARSE_TXPWR_23,
    TPC_COARSE_TXPWR_24,
    TPC_COARSE_TXPWR_25,
    TPC_COARSE_TXPWR_26,
    TPC_COARSE_TXPWR_27,
    TPC_COARSE_TXPWR_28,
    TPC_COARSE_TXPWR_29,
    TPC_COARSE_TXPWR_30,
    TPC_COARSE_TXPWR_31,
    NUM_TPC_COARSE_STEPS = TPC_COARSE_TXPWR_31 - TPC_COARSE_TXPWR_0 + 1,
    MIN_TPC_COARSE_TXPWR = TPC_COARSE_TXPWR_0,
    MAX_TPC_COARSE_TXPWR = TPC_COARSE_TXPWR_31
}eTxCoarseGain;                                 //                                          

typedef enum
{
    TPC_FINE_TXPWR_0,
    TPC_FINE_TXPWR_1,
    TPC_FINE_TXPWR_2,
    TPC_FINE_TXPWR_3,
    TPC_FINE_TXPWR_4,
    TPC_FINE_TXPWR_5,
    TPC_FINE_TXPWR_6,
    TPC_FINE_TXPWR_7,
    TPC_FINE_TXPWR_8,
    TPC_FINE_TXPWR_9,
    TPC_FINE_TXPWR_10,
    TPC_FINE_TXPWR_11,
    TPC_FINE_TXPWR_12,
    TPC_FINE_TXPWR_13,
    TPC_FINE_TXPWR_14,
    TPC_FINE_TXPWR_15,
    MIN_TPC_FINE_TXPWR = TPC_FINE_TXPWR_0,
    MAX_TPC_FINE_TXPWR = TPC_FINE_TXPWR_15
}eTxFineGain;                                  //                                             

typedef PACKED_PRE struct PACKED_POST {
    eTxCoarseGain coarsePwr;
    eTxFineGain finePwr;
}tTxGain;

//                                                        
#define HAL_PHY_PERIODIC_CAL_ITER_LIMIT       10

typedef enum
{
    //                                                                                   
    //                
    PHY_CHAIN_SEL_R0_T0_ON,

    PHY_CHAIN_SEL_BT_R0_T0_ON,      //                                      


    //          
    PHY_CHAIN_SEL_R0_ON,
    PHY_CHAIN_SEL_T0_ON,
    PHY_CHAIN_SEL_NO_RX_TX,

    MAX_PHY_CHAIN_SEL,
    INVALID_PHY_CHAIN_SEL,
    PHY_MAX_CHAIN_SELECT = 0x7FFFFFFF  /*                        */
}ePhyChainSelect;

typedef enum
{
#ifdef CHANNEL_BONDED_CAPABLE

    PHY_CCA_40MHZ_SOURCE = 0,
#endif

    PHY_CCA_20MHZ_SOURCE = 1
}ePhyCCASource;

typedef enum
{
    PHY_CCA_FORCED_ON = 0,
    PHY_CCA_ED = 1,
    PHY_CCA_CD = 2,
    PHY_CCA_CD_AND_CS = 3,
    PHY_CCA_ED_AND_CD = 4,
    PHY_CCA_ED_OR_CD = 5,
    PHY_CCA_ED_AND_CD_AND_CS = 6,
    PHY_CCA_ED_OR_CD_AND_CS = 7,
    PHY_CCA_SEC_ED40_AND_NOR_PKTDET40_PKTDET20 = 8,
    PHY_CCA_SEC_BUSY = 9
}ePhyCCAMode;

typedef enum
{
    PHY_RX_DISABLE_NONE             = 0,
    PHY_RX_DISABLE_11AG             = 0x00000001,          
    PHY_RX_DISABLE_11B              = 0x00000002,          
    PHY_RX_DISABLE_11N40            = 0x00000004,          
    PHY_RX_DISABLE_11AC80           = 0x00000008,          

    PHY_RX_DISABLE_11ABG            = (PHY_RX_DISABLE_11AG | PHY_RX_DISABLE_11B),
    PHY_RX_DISABLE_ALL_TYPES        = (PHY_RX_DISABLE_11B | PHY_RX_DISABLE_11AG | 
                                       PHY_RX_DISABLE_11N40 | PHY_RX_DISABLE_11AC80),
}ePhyRxDisabledPktTypes;


//                                  
typedef enum
{
    PHY_NW_DENSITY_LOW = 0,
    PHY_NW_DENSITY_MED,
    PHY_NW_DENSITY_HIGH,
    PHY_NW_DENSITY_ADAPTIVE
} ePhyNwDensity;


typedef enum
{
    ALL_CALS,           //                   
    RX_DCO_CAL_ONLY,
    RX_IM2_CAL_ONLY,
    RX_DCO_IM2_CAL,
    TX_LO_CAL_ONLY,
    RX_IQ_CAL_ONLY,
    TX_IQ_CAL_ONLY,
    HKDAC_TX_IQ_CAL_ONLY,
    NO_CALS = 0xFF
}eCalSelection;


//                                             
typedef enum
{
    FORCE_CLOSED_LOOP_GAIN     = 0,  //                                                                                               
    FORCE_POWER_TEMPLATE_INDEX = 1,  //                                    
    FIXED_POWER_DBM            = 2,  //                                                                 
    REGULATORY_POWER_LIMITS    = 3,  //                                                                          
    RATE_POWER_NON_LIMITED     = 4,   //                                                                                
    POWER_INDX_SRC_MAX_VAL     = 0x7FFFFFFF, //                                
}ePowerTempIndexSource;

#define BIT_0   0x00000001
#define BIT_1   0x00000002
#define BIT_2   0x00000004
#define BIT_3   0x00000008
#define BIT_4   0x00000010
#define BIT_5   0x00000020
#define BIT_6   0x00000040
#define BIT_7   0x00000080
#define BIT_8   0x00000100
#define BIT_9   0x00000200
#define BIT_10  0x00000400  
#define BIT_11  0x00000800
#define BIT_12  0x00001000
#define BIT_13  0x00002000
#define BIT_14  0x00004000
#define BIT_15  0x00008000
#define BIT_16  0x00010000
#define BIT_17  0x00020000
#define BIT_18  0x00040000
#define BIT_19  0x00080000
#define BIT_20  0x00100000
#define BIT_21  0x00200000
#define BIT_22  0x00400000
#define BIT_23  0x00800000
#define BIT_24  0x01000000
#define BIT_25  0x02000000
#define BIT_26  0x04000000
#define BIT_27  0x08000000
#define BIT_28  0x10000000
#define BIT_29  0x20000000
#define BIT_30  0x40000000
#define BIT_31  0x80000000

#define WFM_CLK_80  BIT_3
#define WFM_START   BIT_0
#define WFM_STOP    BIT_1

#define WFM_MEM_I_DATA_MASK         (0x7FF)
#define WFM_MEM_Q_DATA_OFFSET       (0xB)
#define WFM_MEM_Q_DATA_MASK         (0x3FF800)

typedef enum
{
    WAVE_SINGLE_SHOT = 0,
    WAVE_CONTINUOUS = BIT_2
}eWaveMode;

typedef enum
{
    RATE_240 = 0,
    RATE_160 = 1,
    RATE_120 = 2,
    RATE_80 = 3,
    RATE_40 = 4,
    RATE_20 = 5,
}eWaveRate;

#define MAX_TONE_AMPLITUDE  (2^11)  //             

#define MAX_TEST_WAVEFORM_SAMPLES    500

#define NUM_RX_IMB_CAL_TONES    4

#define CAL_WFM_TX_TONE_8_START_IDX         0
#define CAL_WFM_TX_TONE_8_STOP_IDX          255
#define CAL_WFM_TX_TONE_MINUS_8_START_IDX   256
#define CAL_WFM_TX_TONE_MINUS_8_STOP_IDX    511
#define CAL_WFM_RX_TONE_START_IDX           512
#define CAL_WFM_RX_TONE_STOP_IDX            767

#define B_RATE_CAL_ADJUSTMENT               -150
#define GN_RATE_BANDEDGE_ADJUSTMENT         -100

#define TPC_INDEX_WIFI_DIRECT   0
#define TPC_INDEX_LOW_POWER     1
#define MIN_TPC_GAIN_INDEX                      0   //                            
#define TPC_GAIN_LUT_PWR_SLOPE                  2
#define MAX_TPC_GAIN_LUT_DBM    (22)
#define MIN_TPC_GAIN_LUT_DBM    (6)

#define MAX_TPC_GAIN_LUT_DBM_2DEC_PLACES    (MAX_TPC_GAIN_LUT_DBM * 100)
#define MIN_TPC_GAIN_LUT_DBM_2DEC_PLACES    (MIN_TPC_GAIN_LUT_DBM * 100)

typedef enum
{
    RF_BANDWIDTH_20MHZ = 20,
    RF_BANDWIDTH_40MHZ = 40,
    RF_BANDWIDTH_80MHZ = 80,
    RF_MIN_BANDWIDTH = RF_BANDWIDTH_20MHZ,
    RF_MAX_BANDWIDTH = RF_BANDWIDTH_80MHZ,
    RF_BANDWIDTH_INVALID = 0x7FFFFFFF
}eRfBandwidth;

#endif /*            */

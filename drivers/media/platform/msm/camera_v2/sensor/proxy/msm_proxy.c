/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
/*
	Last updated : 2014/05/06, by sungmin.woo@lge.com, seonyoung.kim@lge.com
	change description : fix wrap around problem, 03/18
				       cover non-proxy sensor case, abnormal camera close by kill qcamera-daemon 04/12
				       non-calibration module error, increased max-convergence time 05/06
*/

#define pr_fmt(fmt) "%s:%d " fmt, __func__, __LINE__

#include <linux/module.h>
#include "msm_proxy.h"
#include "msm_cci.h"
#include "msm_proxy_i2c.h"

//******************************** babybear registers cut 1.1
#define IDENTIFICATION__MODEL_ID					0x000
#define IDENTIFICATION__REVISION_ID				0x002
#define REVISION_NOT_CALIBRATED					0x02
#define REVISION_CALIBRATED						0x03
#define FIRMWARE__BOOTUP							0x119
#define RESULT__RANGE_STATUS						0x04D
#define GPIO_HV_PAD01__CONFIG						0x132
#define SYSRANGE__MAX_CONVERGENCE_TIME			0x01C
#define SYSRANGE__RANGE_CHECK_ENABLES			0x02D
#define SYSRANGE__MAX_CONVERGENCE_TIME			0x01C
#define SYSRANGE__EARLY_CONVERGENCE_ESTIMATE	0x022
#define SYSTEM__FRESH_OUT_OF_RESET				0x016
#define SYSRANGE__PART_TO_PART_RANGE_OFFSET	0x024
#define SYSRANGE__CROSSTALK_COMPENSATION_RATE	0x01E
#define SYSRANGE__CROSSTALK_VALID_HEIGHT		0x021
#define SYSRANGE__RANGE_IGNORE_VALID_HEIGHT		0x025
#define SYSRANGE__RANGE_IGNORE_THRESHOLD		0x026
#define SYSRANGE__MAX_AMBIENT_LEVEL_MULT		0x02C
#define SYSALS__INTERMEASUREMENT_PERIOD			0x03E
#define SYSRANGE__INTERMEASUREMENT_PERIOD		0x01B
#define SYSRANGE__START							0x018
#define RESULT__RANGE_VAL							0x062
#define RESULT__RANGE_STRAY						0x063
#define RESULT__RANGE_RAW							0x064
#define RESULT__RANGE_RETURN_SIGNAL_COUNT		0x06C
#define RESULT__RANGE_REFERENCE_SIGNAL_COUNT	0x070
#define RESULT__RANGE_RETURN_AMB_COUNT			0x074
#define RESULT__RANGE_REFERENCE_AMB_COUNT		0x078
#define RESULT__RANGE_RETURN_CONV_TIME			0x07C
#define RESULT__RANGE_REFERENCE_CONV_TIME		0x080
#define SYSTEM__INTERRUPT_CLEAR					0x015
#define RESULT__INTERRUPT_STATUS_GPIO			0x04F
#define SYSTEM__MODE_GPIO1						0x011
#define SYSTEM__INTERRUPT_CONFIG_GPIO			0x014
#define RANGE__RANGE_SCALER						0x096
#define SYSRANGE__PART_TO_PART_RANGE_OFFSET		0x024
//******************************** babybear registers cut 1.1
/// HIGH LIGHT OPTIMIZATION DATA
#define LOW_LIGHT_RETURN_RATE	 1800
#define HIGH_LIGHT_RETURN_RATE	5000

#define LOW_LIGHT_XTALK_RATIO	100
#define HIGH_LIGHT_XTALK_RATIO	 35

#define LOW_LIGHT_IGNORETHRES_RATIO		100
#define HIGH_LIGHT_IGNORETHRES_RATIO 	 28

#define DEFAULT_CROSSTALK     4 // 12 for ST Glass; 2 for LG Glass
#define DEFAULT_IGNORETHRES   0 // 32 fior ST Glass; 0 for LG Glass

// Filter defines
#define FILTERNBOFSAMPLES		10
#define FILTERSTDDEVSAMPLES		6
#define MINFILTERSTDDEVSAMPLES	3
#define MINFILTERVALIDSTDDEVSAMPLES	4
#define FILTERINVALIDDISTANCE	65535

#define it_eep_reg 0x800
#define fj_eep_reg 0x8B0
//Wrap around filter
#define COMPLEX_FILTER

uint32_t MeasurementIndex = 0;
// Lite Filter global variables
uint32_t Default_ZeroVal = 0;
uint32_t Default_VAVGVal = 0;
uint32_t NoDelay_ZeroVal = 0;
uint32_t NoDelay_VAVGVal = 0;
uint32_t Previous_VAVGDiff = 0;
// Complex Filter global variables
uint16_t LastTrueRange[FILTERNBOFSAMPLES];
uint32_t LastReturnRates[FILTERNBOFSAMPLES];
uint32_t PreviousRangeStdDev = 0;
uint32_t PreviousStdDevLimit = 0;
uint32_t PreviousReturnRateStdDev = 0;
uint16_t StdFilteredReads = 0;
uint32_t m_chipid = 0;
uint16_t LastMeasurements[8] = {0,0,0,0,0,0,0,0};
uint16_t AverageOnXSamples = 4;
uint16_t CurrentIndex = 0;

void BabyBear_ParameterOptimization(u32 ambientRate);
u32 BabyBear_damper(u32 inData, u32 ambientRate, u32 LowLightRatio, u32 HighLightRatio);

#ifdef COMPLEX_FILTER
void VL6180_InitComplexFilter(void);
uint16_t VL6180_ComplexFilter(uint16_t m_trueRange_mm, uint16_t m_rawRange_mm, uint32_t m_rtnSignalRate, uint32_t m_rtnAmbientRate, uint16_t errorCode);
uint32_t VL6180_StdDevDamper(uint32_t AmbientRate, uint32_t SignalRate, uint32_t StdDevLimitLowLight, uint32_t StdDevLimitLowLightSNR, uint32_t StdDevLimitHighLight, uint32_t StdDevLimitHighLightSNR);
#else
void VL6180_InitLiteFilter(void);
uint16_t VL6180_LiteFilter(uint16_t m_trueRange_mm, uint16_t m_rawRange_mm, uint32_t m_rtnSignalRate, uint32_t m_rtnAmbientRate, uint16_t errorCode);
#endif

static struct msm_proxy_ctrl_t msm_proxy_t;

static struct msm_camera_i2c_fn_t msm_sensor_cci_func_tbl = {
	.i2c_read = msm_camera_cci_i2c_read,
	.i2c_read_seq = msm_camera_cci_i2c_read_seq,
	.i2c_write = msm_camera_cci_i2c_write,
	.i2c_write_seq = msm_camera_cci_i2c_write_seq,	//sungmin.woo added
	.i2c_write_table = msm_camera_cci_i2c_write_table,
	.i2c_write_seq_table = msm_camera_cci_i2c_write_seq_table,
	.i2c_write_table_w_microdelay =
		msm_camera_cci_i2c_write_table_w_microdelay,
	.i2c_util = msm_sensor_cci_i2c_util,
};

static struct msm_camera_i2c_fn_t msm_sensor_qup_func_tbl = {
	.i2c_read = msm_camera_qup_i2c_read,
	.i2c_read_seq = msm_camera_qup_i2c_read_seq,
	.i2c_write = msm_camera_qup_i2c_write,
	.i2c_write_seq = msm_camera_qup_i2c_write_seq,	//sungmin.woo added
	.i2c_write_table = msm_camera_qup_i2c_write_table,
	.i2c_write_seq_table = msm_camera_qup_i2c_write_seq_table,
	.i2c_write_table_w_microdelay =
		msm_camera_qup_i2c_write_table_w_microdelay,
};

static const struct v4l2_subdev_internal_ops msm_proxy_internal_ops = {
//	.open = msm_proxy_open,
//	.close = msm_proxy_close,
};

int32_t proxy_i2c_read(uint32_t addr, uint16_t *data, enum msm_camera_i2c_data_type data_type)
{
	int32_t ret = 0;

	ret = msm_proxy_t.i2c_client.i2c_func_tbl->i2c_read(&msm_proxy_t.i2c_client, addr, data, data_type);

	if(ret < 0){
		msm_proxy_t.i2c_fail_cnt++;
		pr_err("i2c_fail_cnt = %d\n", msm_proxy_t.i2c_fail_cnt);
	}
	return ret;
}
int32_t proxy_i2c_write(uint32_t addr, uint16_t data, enum msm_camera_i2c_data_type data_type)
{
	int32_t ret = 0;

	ret = msm_proxy_t.i2c_client.i2c_func_tbl->i2c_write(&msm_proxy_t.i2c_client, addr, data, data_type);

	if(ret < 0){
		msm_proxy_t.i2c_fail_cnt++;
		pr_err("i2c_fail_cnt = %d\n", msm_proxy_t.i2c_fail_cnt);
	}
	return ret;
}

int32_t proxy_i2c_write_seq(uint32_t addr, uint8_t *data, uint16_t num_byte)
{
	int32_t ret = 0;

	ret= msm_proxy_t.i2c_client.i2c_func_tbl->i2c_write_seq(&msm_proxy_t.i2c_client, addr, data, num_byte);

	if(ret < 0){
		msm_proxy_t.i2c_fail_cnt++;
		pr_err("i2c_fail_cnt = %d\n", msm_proxy_t.i2c_fail_cnt);
	}
	return ret;
}

int32_t proxy_i2c_read_seq(uint32_t addr, uint8_t *data, uint16_t num_byte)
{
	int32_t ret = 0;

	ret= msm_proxy_t.i2c_client.i2c_func_tbl->i2c_read_seq(&msm_proxy_t.i2c_client, addr, &data[0], num_byte);

	if(ret < 0){
		msm_proxy_t.i2c_fail_cnt++;
		pr_err("i2c_fail_cnt = %d\n", msm_proxy_t.i2c_fail_cnt);
	}
	return ret;
}

int32_t proxy_i2c_e2p_write(uint16_t addr, uint16_t data, enum msm_camera_i2c_data_type data_type)
{
           int32_t ret = 0;
           struct msm_camera_i2c_client *proxy_i2c_client = NULL;
           proxy_i2c_client = &msm_proxy_t.i2c_client;

           proxy_i2c_client->cci_client->sid = 0xA0 >> 1;
           ret = proxy_i2c_client->i2c_func_tbl->i2c_write(proxy_i2c_client, addr, data, data_type);
           proxy_i2c_client->cci_client->sid = msm_proxy_t.sid_proxy;
           return ret;
}

int32_t proxy_i2c_e2p_read(uint16_t addr, uint16_t *data, enum msm_camera_i2c_data_type data_type)
{
	int32_t ret = 0;
	struct msm_camera_i2c_client *proxy_i2c_client = NULL;
	proxy_i2c_client = &msm_proxy_t.i2c_client;

	proxy_i2c_client->cci_client->sid = 0xA0 >> 1;
	ret = proxy_i2c_client->i2c_func_tbl->i2c_read(proxy_i2c_client, addr, data, data_type);
	proxy_i2c_client->cci_client->sid = msm_proxy_t.sid_proxy;

	return ret;
}

int16_t OffsetCalibration(void)
{
           int TimeOut = 0;
           int i;
           int MeasuredDistance = 0;
           int RealDistance = 200;

            int8_t MeasuredOffset = 0;

            uint16_t chipidRangeStart = 0;
           uint16_t statusCode = 0;
           uint16_t distance = 0;

		pr_err("OffsetCalibration start!\n");

            //Set offset to zero
           proxy_i2c_write( SYSRANGE__PART_TO_PART_RANGE_OFFSET,0, 1);

            // Disable CrossTalkCompensation
           proxy_i2c_write( SYSRANGE__CROSSTALK_COMPENSATION_RATE, 0, 1);
           proxy_i2c_write( SYSRANGE__CROSSTALK_COMPENSATION_RATE+1, 0, 1);

            for(i=0;i<10;i++){
                     // Run ten measurements in a row
                     proxy_i2c_write( SYSRANGE__START, 1, 1);
                     TimeOut = 0;
                     do{
                                proxy_i2c_read( SYSRANGE__START, &chipidRangeStart, 1);
                                proxy_i2c_read( RESULT__RANGE_STATUS, &statusCode, 1);

                                 TimeOut +=1;
                                if(TimeOut>2000)
                                          return -1;

                      }while(!(((statusCode&0x01)==0x01)&&(chipidRangeStart==0x00)));

                      // Read distance
                     proxy_i2c_read( RESULT__RANGE_VAL, &distance, 1);
                      distance *= 3;

                      MeasuredDistance = MeasuredDistance + distance;
           }

            MeasuredDistance = MeasuredDistance/10;
           MeasuredOffset = (RealDistance - MeasuredDistance)/3;

	pr_err("OffsetCalibration end!\n");

            return MeasuredOffset;
}

uint16_t proxy_get_from_sensor(void)
{
	uint16_t dist = 0;
	uint16_t chipidInter = 0;
	int i = 0;
	int UseAveraging = 0; // 1= do rolling averaring; 0 = no rolling averaging
	int NbOfValidData = 0;
	int MinValidData = 4;
	uint16_t DistAcc = 0;
	uint16_t NewDistance = 0;
	uint16_t chipidcount = 0;
	uint32_t m_rawRange_mm=0;
	uint32_t m_rtnConvTime=0;
	uint32_t m_rtnSignalRate=0;
	uint32_t m_rtnAmbientRate=0;
	uint32_t m_rtnSignalCount = 0;
	uint32_t m_refSignalCount = 0;
	uint32_t m_rtnAmbientCount =0;
	uint32_t m_refAmbientCount =0;
	uint32_t m_refConvTime =0;
	uint32_t m_refSignalRate =0;
	uint32_t m_refAmbientRate =0;
	uint32_t cRtnSignalCountMax = 0x7FFFFFFF;
	uint32_t  cDllPeriods = 6;
	uint32_t rtnSignalCountUInt = 0;
	uint32_t  calcConvTime = 0;
	uint16_t chipidRangeStart = 0;
	uint16_t statusCode = 0;
	uint16_t errorCode = 0;

	//pr_err("get proxy start\n");

	proxy_i2c_read( SYSRANGE__START, &chipidRangeStart, 1);
	//Read Error Code
	proxy_i2c_read( RESULT__RANGE_STATUS, &statusCode, 1);
	errorCode = statusCode>>4;

	proxy_i2c_read( RESULT__INTERRUPT_STATUS_GPIO, &chipidInter, 1);
	proxy_i2c_read( RESULT__RANGE_VAL, &dist, 1);
	dist *= 3;

	proxy_i2c_read( RESULT__RANGE_RAW, &chipidcount,1);

	m_rawRange_mm = (uint32_t)chipidcount;

	#if 0
	for(i =0; i < 4;  i++){
		proxy_i2c_read( RESULT__RANGE_RETURN_SIGNAL_COUNT+i, &chipidcount, 1);
		rtnSignalCountUInt |=(uint32_t) ((chipidcount)<< (8*( 3-i)));
	}
	#else
	ProxyRead32bit(RESULT__RANGE_RETURN_SIGNAL_COUNT, &rtnSignalCountUInt);
	#endif

	if(rtnSignalCountUInt > cRtnSignalCountMax)
	{
		rtnSignalCountUInt = 0;
	}

	m_rtnSignalCount  = rtnSignalCountUInt;

	#if 0
	for(i =0; i < 4;  i++){
		proxy_i2c_read( RESULT__RANGE_REFERENCE_SIGNAL_COUNT+i, &chipidcount, 1);
		m_refSignalCount  |=(uint32_t)( (chipidcount)<< (8*( 3-i)));
	  }

	for(i =0; i < 4;  i++){
		proxy_i2c_read( RESULT__RANGE_RETURN_AMB_COUNT+i, &chipidcount, 1);
		m_rtnAmbientCount |=(uint32_t)((chipidcount)<< (8*(3-i)));
	  }

	for(i =0; i < 4;  i++){
		proxy_i2c_read( RESULT__RANGE_REFERENCE_AMB_COUNT+i, &chipidcount, 1);
		m_refAmbientCount |=(uint32_t)((chipidcount)<< (8*( 3-i)));
	  }

	for(i =0; i < 4;  i++){
		proxy_i2c_read( RESULT__RANGE_RETURN_CONV_TIME+i, &chipidcount, 1);
		m_rtnConvTime |=(uint32_t)((chipidcount)<< (8*( 3-i)));
	  }

	for(i =0; i < 4;  i++){
		proxy_i2c_read( RESULT__RANGE_REFERENCE_CONV_TIME+i, &chipidcount, 1);
		m_refConvTime |= (uint32_t)((chipidcount)<< (8*( 3-i)));
	  }
	#else
	ProxyRead32bit(RESULT__RANGE_REFERENCE_SIGNAL_COUNT, &m_refSignalCount);
	ProxyRead32bit(RESULT__RANGE_RETURN_AMB_COUNT, &m_rtnAmbientCount);
	ProxyRead32bit(RESULT__RANGE_REFERENCE_AMB_COUNT, &m_refAmbientCount);
	ProxyRead32bit(RESULT__RANGE_RETURN_CONV_TIME, &m_rtnConvTime);
	ProxyRead32bit(RESULT__RANGE_REFERENCE_CONV_TIME, &m_refConvTime);
	#endif

	calcConvTime = m_refConvTime;
	if (m_rtnConvTime > m_refConvTime)
	{
		calcConvTime = m_rtnConvTime;
	}
	if(calcConvTime==0)
		calcConvTime=63000;

	m_rtnSignalRate  = (m_rtnSignalCount*1000)/calcConvTime;
	m_refSignalRate  = (m_refSignalCount*1000)/calcConvTime;
	m_rtnAmbientRate = (m_rtnAmbientCount * cDllPeriods*1000)/calcConvTime;
	m_refAmbientRate = (m_rtnAmbientCount * cDllPeriods*1000)/calcConvTime;

	BabyBear_ParameterOptimization((uint32_t) m_rtnAmbientRate);

	if(((statusCode&0x01)==0x01)&&(chipidRangeStart==0x00)){
		// Do the rolling averaging
		if(UseAveraging==1){
			for(i=0;i<7;i++){
				LastMeasurements[i]=LastMeasurements[i+1];
			}
			if(m_rawRange_mm!=255){
				// Valid Value
				LastMeasurements[7] = dist;
			} else{
				// Not a valid measure
				LastMeasurements[7] = 65535;
			}
			if(CurrentIndex<8){
				MinValidData = (CurrentIndex+1)/2;
				CurrentIndex++;
			}else{
				MinValidData = 4;
			}

			NbOfValidData = 0;
			DistAcc = 0;
			NewDistance = 255*3; // Max distance, equivalent as when no target
			for(i=7;i>=0; i--){
				if(LastMeasurements[i] !=65535){
					// This measurement is valid
					NbOfValidData = NbOfValidData+1;
					DistAcc = DistAcc + LastMeasurements[i];
					if(NbOfValidData>=MinValidData){
						NewDistance = DistAcc/NbOfValidData;
						break;
					}
				}
			}
			// Copy the new distance
			dist = NewDistance;
		}

	#ifdef COMPLEX_FILTER
			dist = VL6180_ComplexFilter(dist, m_rawRange_mm*3, m_rtnSignalRate, m_rtnAmbientRate, errorCode);
	#else
			dist = VL6180_LiteFilter(dist, m_rawRange_mm*3, m_rtnSignalRate, m_rtnAmbientRate, errorCode);
	#endif

		// Start new measurement
		proxy_i2c_write( SYSRANGE__START, 0x01, 1);
		m_chipid = dist;

	}

	else
	{
	   // Return immediately with previous value
	   dist = m_chipid;

	}
	//need to check rc value here //
	//CDBG("proxy = %d\n",  dist);
	//pr_err("get proxy end\n");
	msm_proxy_t.proxy_stat.proxy_val = dist;
	msm_proxy_t.proxy_stat.proxy_conv = calcConvTime;
	msm_proxy_t.proxy_stat.proxy_sig = m_rtnSignalRate;
	msm_proxy_t.proxy_stat.proxy_amb = m_rtnAmbientRate;
	msm_proxy_t.proxy_stat.proxy_raw = m_rawRange_mm*3;

	return dist;

}

static void get_proxy(struct work_struct *work)
{
	struct msm_proxy_ctrl_t *proxy_struct = container_of(work, struct msm_proxy_ctrl_t, proxy_work);
	uint16_t * proxy = &proxy_struct->last_proxy;
	int16_t offset;
	int16_t cal_count = 0;
	int16_t fin_val;
	uint16_t module_id = 0;
	uint8_t shift_module_id = 0;
	
	while(1)
	{
		//pr_err("pause_workqueue = %d\n", proxy_struct->pause_workqueue);

		if(!proxy_struct->pause_workqueue)
		{
			if(proxy_struct->proxy_cal)
			{
				#if 0
				proxy_struct->proxy_stat.cal_done = 0;  //cal done
				offset = OffsetCalibration();
				pr_err("write offset = %d to eeprom\n", offset);
				if ((offset <= 10) && (offset >= (-20))) {
					proxy_i2c_e2p_read(0x800, &cal_count, 2);
					calCountShift = (cal_count & 0xff) >> 8;                  //seperate cal count value

					fin_val = (offset & 0xff) | (calCountShift << 8);
					proxy_i2c_e2p_write(0x800, fin_val, 2);                    //write whole offset value and cal count value
					pr_err("KSY read cal count1 = %d to eeprom\n", calCountShift);
					if(calCountShift >= 200)
						calCountShift = 0;
						//proxy_i2c_e2p_write(0x801, 0, 1);
					else {
						calCountShift++;
						pr_err("KSY writed\n");
						calCountShift2 = calCountShift << 8;
						proxy_i2c_e2p_read(0x800, &offset, 2);
						fin_val = calCountShift2 | (offset & 0xff);
						proxy_i2c_e2p_write(0x800, fin_val, 2);
					}

					pr_err("KSY read cal count2 = %d to eeprom\n", calCountShift);
					proxy_struct->proxy_stat.cal_count = calCountShift;
					proxy_struct->proxy_cal = 0;
					proxy_struct->proxy_stat.cal_done = 1;  //cal done
					msm_proxy_t.proxy_cal = 0;
				}
				else{   // Calibration failed by spec out
					proxy_struct->proxy_stat.cal_done = 2;  //cal fail
					msm_proxy_t.proxy_cal = 0;
				}
				#else
				proxy_struct->proxy_stat.cal_done = 0;  //cal done
				offset = OffsetCalibration();
				pr_err("write offset = %x to eeprom\n", offset);
				
				proxy_i2c_e2p_read(0x700, &module_id, 2);	
				shift_module_id = module_id >> 8;

				
				if ((offset < 11) && (offset > (-21))) {
					
					if((shift_module_id == 0x01) || (shift_module_id == 0x02))	// It module
					{
						proxy_i2c_e2p_read(it_eep_reg, &fin_val, 2);
						cal_count=fin_val>>8;

						cal_count++;
						fin_val= (cal_count<<8) | (0x00FF & offset);
						proxy_i2c_e2p_write(it_eep_reg, fin_val, 2);

						pr_err("KSY read inot cal count = %x to eeprom\n", fin_val);
					}

					else if(shift_module_id == 0x03)
					{	
						proxy_i2c_e2p_read(fj_eep_reg, &fin_val, 2);
						cal_count=fin_val>>8;

						cal_count++;
						fin_val= (cal_count<<8) | (0x00FF & offset);
						proxy_i2c_e2p_write(fj_eep_reg, fin_val, 2);

						pr_err("KSY read fj cal count = %x to eeprom\n", fin_val);
					}
						
					proxy_struct->proxy_stat.cal_count = cal_count;
					proxy_struct->proxy_cal = 0;
					proxy_struct->proxy_stat.cal_done = 1;  //cal done
					msm_proxy_t.proxy_cal = 0;
				}
				else{   // Calibration failed by spec out
					proxy_struct->proxy_stat.cal_done = 2;  //cal fail
					msm_proxy_t.proxy_cal = 0;
				}

				#endif
			}
			*proxy = proxy_get_from_sensor();
		}
		if(proxy_struct->i2c_fail_cnt >= proxy_struct->max_i2c_fail_thres)
		{
			pr_err("proxy workqueue force end due to i2c fail!\n");
			break;
		}
		msleep(53);
		if(proxy_struct->exit_workqueue)
			break;
	}
	pr_err("end workqueue!\n");
}
int16_t stop_proxy(void)
{
	pr_err("stop_proxy!\n");
	if(msm_proxy_t.exit_workqueue == 0)
	{
		if(msm_proxy_t.wq_init_success)
		{
			msm_proxy_t.exit_workqueue = 1;
			destroy_workqueue(msm_proxy_t.work_thread);
			msm_proxy_t.work_thread = NULL;
			pr_err("destroy_workqueue!\n");
		}
	}
	return 0;
}
int16_t pause_proxy(void)
{
	pr_err("pause_proxy!\n");
	msm_proxy_t.pause_workqueue = 1;
	pr_err("pause_workqueue = %d\n", msm_proxy_t.pause_workqueue);
	return 0;
}
int16_t restart_proxy(void)
{
	pr_err("restart_proxy!\n");
	msm_proxy_t.pause_workqueue = 0;
	pr_err("pause_workqueue = %d\n", msm_proxy_t.pause_workqueue);
	return 0;
}
uint16_t msm_proxy_thread_start(void)
{
	pr_err("msm_proxy_thread_start\n");

	if(msm_proxy_t.exit_workqueue)
	{
		msm_proxy_t.exit_workqueue = 0;
		msm_proxy_t.work_thread = create_singlethread_workqueue("my_work_thread");
		if(!msm_proxy_t.work_thread){
			pr_err("creating work_thread fail!\n");
			return 1;
		}

		msm_proxy_t.wq_init_success = 1;

		INIT_WORK(&msm_proxy_t.proxy_work, get_proxy);
		pr_err("INIT_WORK done!\n");

		queue_work(msm_proxy_t.work_thread, &msm_proxy_t.proxy_work);
		pr_err("queue_work done!\n");
	}
	return 0;
}
uint16_t msm_proxy_thread_end(void)
{
	uint16_t ret = 0;
	pr_err("msm_proxy_thread_end\n");
	ret = stop_proxy();
	return ret;
}
uint16_t msm_proxy_thread_pause(void)
{
	uint16_t ret = 0;
	pr_err("msm_proxy_thread_pause\n");
	ret = pause_proxy();
	return ret;
}
uint16_t msm_proxy_thread_restart(void)
{
	uint16_t ret = 0;
	pr_err("msm_proxy_thread_restart\n");
	msm_proxy_t.i2c_fail_cnt = 0;
	ret = restart_proxy();
	return ret;
}
uint16_t msm_proxy_cal(void)
{
	uint16_t ret = 0;
	pr_err("msm_proxy_cal\n");
	msm_proxy_t.proxy_cal = 1;
	return ret;
}
static int32_t msm_proxy_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int rc = 0;
	struct msm_proxy_ctrl_t *act_ctrl_t = NULL;
	pr_err("Enter\n");

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		pr_err("i2c_check_functionality failed\n");
		goto probe_failure;
	}

	act_ctrl_t = (struct msm_proxy_ctrl_t *)(id->driver_data);
	CDBG("client = %x\n", (unsigned int) client);
	act_ctrl_t->i2c_client.client = client;

	act_ctrl_t->act_device_type = MSM_CAMERA_I2C_DEVICE;
	act_ctrl_t->i2c_client.i2c_func_tbl = &msm_sensor_qup_func_tbl;


	snprintf(act_ctrl_t->msm_sd.sd.name, sizeof(act_ctrl_t->msm_sd.sd.name),
		"%s", act_ctrl_t->i2c_driver->driver.name);

	v4l2_i2c_subdev_init(&act_ctrl_t->msm_sd.sd,
		act_ctrl_t->i2c_client.client,
		act_ctrl_t->act_v4l2_subdev_ops);
	v4l2_set_subdevdata(&act_ctrl_t->msm_sd.sd, act_ctrl_t);
	//act_ctrl_t->msm_sd.sd.internal_ops = &msm_proxy_internal_ops;
	act_ctrl_t->msm_sd.sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	media_entity_init(&act_ctrl_t->msm_sd.sd.entity, 0, NULL, 0);
	act_ctrl_t->msm_sd.sd.entity.type = MEDIA_ENT_T_V4L2_SUBDEV;
	//act_ctrl_t->msm_sd.sd.entity.group_id = MSM_CAMERA_SUBDEV_PROXY;
	msm_sd_register(&act_ctrl_t->msm_sd);
	CDBG("succeeded\n");
	pr_err("Exit\n");

	probe_failure:
		return rc;
	}


static int32_t msm_proxy_platform_probe(struct platform_device *pdev)
{

	int32_t rc = 0;

	struct msm_camera_cci_client *cci_client = NULL;
	CDBG("Enter\n");

	if (!pdev->dev.of_node) {
		CDBG("of_node NULL : %d\n", EINVAL);
		return -EINVAL;
	}

	rc = of_property_read_u32((&pdev->dev)->of_node, "cell-index",
		&pdev->id);
	CDBG("cell-index %d, rc %d\n", pdev->id, rc);
	if (rc < 0) {
		pr_err("failed rc %d\n", rc);
		return rc;
	}

	rc = of_property_read_u32((&pdev->dev)->of_node, "qcom,cci-master",
		&msm_proxy_t.cci_master);
	CDBG("qcom,cci-master %d, rc %d\n", msm_proxy_t.cci_master, rc);
	if (rc < 0) {
		pr_err("failed rc %d\n", rc);
		return rc;
	}


	msm_proxy_t.pdev = pdev;

	msm_proxy_t.act_device_type = MSM_CAMERA_PLATFORM_DEVICE;
	msm_proxy_t.i2c_client.i2c_func_tbl = &msm_sensor_cci_func_tbl;
	msm_proxy_t.i2c_client.cci_client = kzalloc(sizeof(
		struct msm_camera_cci_client), GFP_KERNEL);
	if (!msm_proxy_t.i2c_client.cci_client) {
		pr_err("failed no memory\n");
		return -ENOMEM;
	}

	msm_proxy_t.i2c_client.cci_client->sid = 0x29;    //Slave address
	msm_proxy_t.i2c_client.cci_client->retries = 3;
	msm_proxy_t.i2c_client.cci_client->id_map = 0;

	msm_proxy_t.i2c_client.cci_client->cci_i2c_master = MASTER_0;

	msm_proxy_t.i2c_client.addr_type = MSM_CAMERA_I2C_WORD_ADDR;

	cci_client = msm_proxy_t.i2c_client.cci_client;
	cci_client->cci_subdev = msm_cci_get_subdev();
	v4l2_subdev_init(&msm_proxy_t.msm_sd.sd,
		msm_proxy_t.act_v4l2_subdev_ops);
	v4l2_set_subdevdata(&msm_proxy_t.msm_sd.sd, &msm_proxy_t);
	//msm_proxy_t.msm_sd.sd.internal_ops = &msm_proxy_internal_ops;
	msm_proxy_t.msm_sd.sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	snprintf(msm_proxy_t.msm_sd.sd.name,
		ARRAY_SIZE(msm_proxy_t.msm_sd.sd.name), "msm_proxy");
	media_entity_init(&msm_proxy_t.msm_sd.sd.entity, 0, NULL, 0);
	msm_proxy_t.msm_sd.sd.entity.type = MEDIA_ENT_T_V4L2_SUBDEV;
	//msm_proxy_t.msm_sd.sd.entity.group_id = MSM_CAMERA_SUBDEV_PROXY;
	msm_sd_register(&msm_proxy_t.msm_sd);

	msm_proxy_t.sid_proxy = msm_proxy_t.i2c_client.cci_client->sid;
	msm_proxy_t.proxy_func_tbl = NULL;
	msm_proxy_t.exit_workqueue = 1;
	msm_proxy_t.pause_workqueue = 0;
	msm_proxy_t.max_i2c_fail_thres = 5;
	msm_proxy_t.i2c_fail_cnt = 0;
	msm_proxy_t.proxy_cal = 0;
	msm_proxy_t.proxy_stat.cal_done = 0;

	CDBG("Exit\n");

	return rc;
}


static const struct i2c_device_id msm_proxy_i2c_id[] = {
	{"msm_proxy", (kernel_ulong_t)&msm_proxy_t},
	{ }
};

static struct i2c_driver msm_proxy_i2c_driver = {
	.id_table = msm_proxy_i2c_id,
	.probe  = msm_proxy_i2c_probe,
	.remove = __exit_p(msm_proxy_i2c_remove),
	.driver = {
		.name = "msm_proxy",
	},
};

static const struct of_device_id msm_proxy_dt_match[] = {
	{.compatible = "qcom,proxy"},
	{}
};

MODULE_DEVICE_TABLE(of, msm_proxy_dt_match);

static struct platform_driver msm_proxy_platform_driver = {
	.driver = {
		.name = "qcom,proxy",
		.owner = THIS_MODULE,
		.of_match_table = msm_proxy_dt_match,
	},
};
static int __init msm_proxy_init_module(void)
{
	int32_t rc = 0;
	CDBG("Enter\n");
	rc = platform_driver_probe(msm_proxy_t.pdriver,
		msm_proxy_platform_probe);

	CDBG("Enter %d\n", rc);
	if (!rc)
		return rc;
	CDBG("%s:%d rc %d\n", __func__, __LINE__, rc);
	return i2c_add_driver(msm_proxy_t.i2c_driver);
}

int msm_init_proxy(void)
{
	int rc = 0;
	int i;
	uint8_t byteArray[4];
	int8_t offsetByte;
	int16_t fin_val;
	uint8_t cal_count;
	//s8 check_offset;
	//int8_t rangeTemp = 0;
	uint16_t modelID = 0;
	uint16_t revID = 0;
	uint16_t chipidRange = 0;
	//uint16_t chipidOffset = 0;
	uint16_t chipidRangeMax = 0;
	//uint16_t chipidscalar = 0;
	uint16_t chipidgpio = 0;
	uint32_t shift, dataMask;
	//int32_t  readI2C =0x00000000;
	uint16_t readI2C =0x0;
	uint32_t ninepointseven=0;
	//seonyung
	uint16_t CrosstalkHeight;
	uint16_t IgnoreThreshold;
	uint16_t IgnoreThresholdHeight;
	//
	uint16_t proxy_status = 0;
	uint16_t proxy_FATAL = 0;
	
	uint16_t dataByte;
	uint16_t ambpart2partCalib1 = 0;
	uint16_t ambpart2partCalib2 = 0;
	uint16_t module_id = 0;
	uint8_t shift_module_id = 0;

	pr_err("msm_init_proxy ENTER!\n");

	proxy_i2c_read( RESULT__RANGE_STATUS, &proxy_status, 1);
	proxy_i2c_read( 0x290, &proxy_FATAL, 1);

	if((proxy_status & 0x01) && ((proxy_status>>4) == 0) && (proxy_FATAL == 0))
	{
		pr_err("init proxy alive!\n");
	}
	else
	{
		pr_err("init proxy fail!, no proxy sensor found!\n");
		return -1;
	}

	proxy_i2c_read( IDENTIFICATION__MODEL_ID, &modelID, 1);
	proxy_i2c_read( IDENTIFICATION__REVISION_ID, &revID, 1);
	//revID = revID >> 4;
	pr_err("Model ID : 0x%X, REVISION ID : 0x%X\n", modelID, revID);   //if revID == 2;(not calibrated), revID == 3 (calibrated)
	if(revID != REVISION_CALIBRATED)
	{
		pr_err("not calibrated!\n");
		//return -1;
	}

	//waitForStandby
	for(i=0; i<100; i++){
	proxy_i2c_read( FIRMWARE__BOOTUP, &modelID, 1);
		if( (modelID & 0x01) == 1 ){
			i=100;
		}
	}

	//range device ready
	for(i=0; i<100; i++){
		proxy_i2c_read( RESULT__RANGE_STATUS, &modelID, 1);
		if( (modelID & 0x01) == 1){
			i = 100;
			}
	}

	//performRegisterTuningCut1_1
	proxy_i2c_write( GPIO_HV_PAD01__CONFIG, 0x30, 1);
	proxy_i2c_write( 0x0207, 0x01, 1);
	proxy_i2c_write( 0x0208, 0x01, 1);
	proxy_i2c_write( 0x0133, 0x01, 1);
	proxy_i2c_write( 0x0096, 0x00, 1);
	proxy_i2c_write( 0x0097, 0xFD, 1);
	proxy_i2c_write( 0x00e3, 0x00, 1);
	proxy_i2c_write( 0x00e4, 0x04, 1);
	proxy_i2c_write( 0x00e5, 0x02, 1);
	proxy_i2c_write( 0x00e6, 0x01, 1);
	proxy_i2c_write( 0x00e7, 0x03, 1);
	proxy_i2c_write( 0x00f5, 0x02, 1);
	proxy_i2c_write( 0x00D9, 0x05, 1);
	
	// AMB P2P calibration
	proxy_i2c_read(SYSTEM__FRESH_OUT_OF_RESET, &dataByte, 1);
	if(dataByte==0x01)
	{
		proxy_i2c_read( 0x26, &dataByte, 1);
		ambpart2partCalib1 = dataByte<<8;
		proxy_i2c_read( 0x27, &dataByte, 1);
		ambpart2partCalib1 = ambpart2partCalib1 + dataByte;
		proxy_i2c_read( 0x28, &dataByte, 1);
		ambpart2partCalib2 = dataByte<<8;
		proxy_i2c_read( 0x29, &dataByte, 1);
		ambpart2partCalib2 = ambpart2partCalib2 + dataByte;
		if(ambpart2partCalib1!=0)
		{
			// p2p calibrated
			proxy_i2c_write( 0xDA, (ambpart2partCalib1>>8)&0xFF, 1);
			proxy_i2c_write( 0xDB, ambpart2partCalib1&0xFF, 1);
			proxy_i2c_write( 0xDC, (ambpart2partCalib2>>8)&0xFF, 1);
			proxy_i2c_write( 0xDD, ambpart2partCalib2&0xFF, 1);
		}
		else
		{
			// No p2p Calibration, use default settings
			proxy_i2c_write( 0xDB, 0xCE, 1);
			proxy_i2c_write( 0xDC, 0x03, 1);
			proxy_i2c_write( 0xDD, 0xF8, 1);
		}
	}
	
	proxy_i2c_write( 0x009f, 0x00, 1);
	proxy_i2c_write( 0x00a3, 0x3c, 1);
	proxy_i2c_write( 0x00b7, 0x00, 1);
	proxy_i2c_write( 0x00bb, 0x3c, 1);
	proxy_i2c_write( 0x00b2, 0x09, 1);
	proxy_i2c_write( 0x00ca, 0x09, 1);
	proxy_i2c_write( 0x0198, 0x01, 1);
	proxy_i2c_write( 0x01b0, 0x17, 1);
	proxy_i2c_write( 0x01ad, 0x00, 1);
	proxy_i2c_write( 0x00FF, 0x05, 1);
	proxy_i2c_write( 0x0100, 0x05, 1);
	proxy_i2c_write( 0x0199, 0x05, 1);
	proxy_i2c_write( 0x0109, 0x07, 1);
	proxy_i2c_write( 0x010a, 0x30, 1);
	proxy_i2c_write( 0x003f, 0x46, 1);
	proxy_i2c_write( 0x01a6, 0x1b, 1);
	proxy_i2c_write( 0x01ac, 0x3e, 1);
	proxy_i2c_write( 0x01a7, 0x1f, 1);
	proxy_i2c_write( 0x0103, 0x01, 1);
	proxy_i2c_write( 0x0030, 0x00, 1);
	proxy_i2c_write( 0x001b, 0x0A, 1);
	proxy_i2c_write( 0x003e, 0x0A, 1);
	proxy_i2c_write( 0x0131, 0x04, 1);
	proxy_i2c_write( 0x0011, 0x10, 1);
	proxy_i2c_write( 0x0014, 0x24, 1);
	proxy_i2c_write( 0x0031, 0xFF, 1);
	proxy_i2c_write( 0x00d2, 0x01, 1);
	proxy_i2c_write( 0x00f2, 0x01, 1);

	// RangeSetMaxConvergenceTime
	proxy_i2c_write( SYSRANGE__MAX_CONVERGENCE_TIME, 0x3F, 1);
	proxy_i2c_read( SYSRANGE__RANGE_CHECK_ENABLES, &chipidRangeMax, 1);
	chipidRangeMax = chipidRangeMax & 0xFE; // off ECE
	chipidRangeMax = chipidRangeMax | 0x02; // on ignore thr
	proxy_i2c_write( SYSRANGE__RANGE_CHECK_ENABLES, chipidRangeMax, 1);


	proxy_i2c_write( SYSRANGE__MAX_AMBIENT_LEVEL_MULT, 0xFF, 1);//SNR
	//proxy_i2c_write( 0x0B8+3, 0x28, 1);


	// ClearSystemFreshOutofReset
	proxy_i2c_write( SYSTEM__FRESH_OUT_OF_RESET, 0x0, 1);

	#if 0
	//readRangeOffset
	proxy_i2c_read( SYSRANGE__PART_TO_PART_RANGE_OFFSET, &chipidscalar, 1);
	rangeTemp = (int8_t)chipidscalar;
	if(chipidscalar > 0x7F) {
		rangeTemp -= 0xFF;
		}
	#endif
	//Multiread
	#if 0
	for(i =0; i < 2;  i++){
		proxy_i2c_read( RANGE__RANGE_SCALER+i, &chipidOffset, 1);
		readI2C |= ((chipidOffset)<< (8*( 1-i)));
	  }
	#else
	ProxyRead16bit(RANGE__RANGE_SCALER, &readI2C);
	#endif

	//Range_Set_scalar
	for(i = 0; i < sizeof(u16); i++)
	{
		shift = (sizeof(u16) - i - 1)* 0x08;
		dataMask = (0xFF << shift);
		byteArray[i] = (u8)(((u16)((u16)85 & 0x01ff) & dataMask) >> shift);
		proxy_i2c_write( RANGE__RANGE_SCALER + i, byteArray[i], 1);
	}
	//readRangeOffset
	#if 0
	proxy_i2c_read( SYSRANGE__PART_TO_PART_RANGE_OFFSET, &chipidRangeMax, 1);
	rangeTemp = (int8_t)chipidRangeMax;
	if(chipidRangeMax > 0x7F) {
		rangeTemp -= 0xFF;
		}
	rangeTemp /= 3;

	rangeTemp = rangeTemp +1; //roundg
	//Range_Set_Offset
	offsetByte = *((u8*)(&rangeTemp)); // round
	proxy_i2c_write( SYSRANGE__PART_TO_PART_RANGE_OFFSET,(u8)offsetByte, 1);
	#else
	proxy_i2c_e2p_read(0x700, &module_id, 2);	
	shift_module_id = module_id >> 8;
	pr_err("KSY module ID : %d\n", shift_module_id);
		
	if((shift_module_id == 0x01) || (shift_module_id == 0x02))  // It module
	{
		proxy_i2c_e2p_read(it_eep_reg, &fin_val, 2);
		offsetByte = 0x00FF & fin_val;
		cal_count = (0xFF00 & fin_val) >> 8;
		if((offsetByte <= -21) || (offsetByte >= 11) || (cal_count >= 100)) {
			proxy_i2c_e2p_write(it_eep_reg, 0, 2);
			cal_count = 0;		
			offsetByte = 0;
		}
		//	offsetByte -= 255;
		msm_proxy_t.proxy_stat.cal_count = cal_count;
		pr_err("inot read offset = %d from eeprom\n", offsetByte);
		proxy_i2c_write( SYSRANGE__PART_TO_PART_RANGE_OFFSET, offsetByte, 1);

	}
	else if(shift_module_id == 0x03)           //fj module
 	{	
		proxy_i2c_e2p_read(fj_eep_reg, &fin_val, 2);
		offsetByte = 0x00FF & fin_val;
		cal_count = (0xFF00 & fin_val) >> 8;
		if((offsetByte <= -21) || (offsetByte >= 11) || (cal_count >= 100)) {
			proxy_i2c_e2p_write(fj_eep_reg, 0, 2);
			cal_count = 0;		
			offsetByte = 0;
		}
		//	offsetByte -= 255;
		msm_proxy_t.proxy_stat.cal_count = cal_count;
		pr_err("fj read offset = %d from eeprom\n", offsetByte);
		proxy_i2c_write( SYSRANGE__PART_TO_PART_RANGE_OFFSET, offsetByte, 1);
	
	}

	
	#endif

	// Babybear_SetStraylight
	ninepointseven=25;
	proxy_i2c_write( SYSRANGE__CROSSTALK_COMPENSATION_RATE,(ninepointseven>>8)&0xFF, 1);
	proxy_i2c_write( SYSRANGE__CROSSTALK_COMPENSATION_RATE+1,ninepointseven&0xFF, 1);

	CrosstalkHeight = 40;
	proxy_i2c_write( SYSRANGE__CROSSTALK_VALID_HEIGHT,CrosstalkHeight&0xFF, 1);


	// Will ignore all low distances (<100mm) with a low return rate
	IgnoreThreshold = 64; // 64 = 0.5Mcps
	IgnoreThresholdHeight = 33; // 33 * scaler3 = 99mm
	proxy_i2c_write( SYSRANGE__RANGE_IGNORE_THRESHOLD, (IgnoreThreshold>>8)&0xFF, 1);
	proxy_i2c_write( SYSRANGE__RANGE_IGNORE_THRESHOLD+1,IgnoreThreshold&0xFF, 1);
	proxy_i2c_write( SYSRANGE__RANGE_IGNORE_VALID_HEIGHT,IgnoreThresholdHeight&0xFF, 1);

	// Init of Averaging samples : in case of adding glass
	for(i=0; i<8;i++){
		LastMeasurements[i]=65535; // 65535 means no valid data
	}
	CurrentIndex = 0;

	// SetSystemInterruptConfigGPIORanging
	proxy_i2c_read( SYSTEM__INTERRUPT_CONFIG_GPIO, &chipidgpio, 1);
	proxy_i2c_write( SYSTEM__INTERRUPT_CONFIG_GPIO, (chipidgpio | 0x04), 1);


	//RangeSetSystemMode
	chipidRange = 0x01;
	proxy_i2c_write( SYSRANGE__START, chipidRange, 1);


	#ifdef COMPLEX_FILTER
		VL6180_InitComplexFilter();
	#else
		VL6180_InitLiteFilter();
	#endif

	return rc;
}
uint16_t msm_get_proxy(struct msm_sensor_proxy_info_t* proxy_info)
{
	uint16_t proxy;
	proxy = msm_proxy_t.last_proxy;
	//proxy_info = msm_proxy_t.proxy_stat;

	memcpy(proxy_info, &msm_proxy_t.proxy_stat, sizeof(msm_proxy_t.proxy_stat));

	//proxy_info->proxy_val = msm_proxy_t.proxy_stat.proxy_val;
	//proxy_info->proxy_conv= msm_proxy_t.proxy_stat.proxy_conv;
	//proxy_info->proxy_sig = msm_proxy_t.proxy_stat.proxy_sig;
	//proxy_info->proxy_amb = msm_proxy_t.proxy_stat.proxy_amb;
	//proxy_info->proxy_raw = msm_proxy_t.proxy_stat.proxy_raw;
	//proxy_info->cal_done = 	msm_proxy_t.proxy_stat.cal_done;

	//pr_err("proxy = %d\n", proxy);

	return proxy;
}
 void BabyBear_ParameterOptimization(u32 ambientRate)
{
	uint32_t newCrossTalk;
	uint32_t newIgnoreThreshold;
	//CDBG("KSY  BabyBear_ParameterOptimization = %d\n",  ambientRate);
	// Compute new values
	newCrossTalk = BabyBear_damper(DEFAULT_CROSSTALK, ambientRate, LOW_LIGHT_XTALK_RATIO, HIGH_LIGHT_XTALK_RATIO);
	newIgnoreThreshold = BabyBear_damper(DEFAULT_IGNORETHRES, ambientRate, LOW_LIGHT_IGNORETHRES_RATIO, HIGH_LIGHT_IGNORETHRES_RATIO);

	//CDBG("KSY  BabyBear_ParameterOptimization newCrossTalk = %d\n",  newCrossTalk);
	//CDBG("KSY  BabyBear_ParameterOptimization newIgnoreThreshold= %d\n",  newIgnoreThreshold);

	// Program new values
	proxy_i2c_write( SYSRANGE__CROSSTALK_COMPENSATION_RATE, (newCrossTalk>>8)&0xFF, 1);
	proxy_i2c_write( SYSRANGE__CROSSTALK_COMPENSATION_RATE+1,newCrossTalk&0xFF, 1);
#if 0
	proxy_i2c_write( SYSRANGE__RANGE_IGNORE_THRESHOLD, (newIgnoreThreshold>>8)&0xFF, 1);
	proxy_i2c_write( SYSRANGE__RANGE_IGNORE_THRESHOLD+1,newIgnoreThreshold&0xFF, 1);
#endif
}



 u32 BabyBear_damper(u32 inData, u32 ambientRate, u32 LowLightRatio, u32 HighLightRatio)
{
	int Weight;

	uint32_t newVal;

	if(ambientRate<=LOW_LIGHT_RETURN_RATE)
	{
		Weight = LowLightRatio;
	}
	else
	{
		if(ambientRate>=HIGH_LIGHT_RETURN_RATE)
		{
			Weight = HighLightRatio;
		}
		else
		{
			// Interpolation
			Weight = (int)LowLightRatio + ( ((int)ambientRate - LOW_LIGHT_RETURN_RATE) * ((int)HighLightRatio - (int)LowLightRatio) / (HIGH_LIGHT_RETURN_RATE - LOW_LIGHT_RETURN_RATE) );
		}
	}

	newVal = (inData * Weight)/100;

	return newVal;
}


void VL6180_InitLiteFilter(void)
{
    MeasurementIndex = 0;

    Default_ZeroVal = 0;
    Default_VAVGVal = 0;
    NoDelay_ZeroVal = 0;
    NoDelay_VAVGVal = 0;
    Previous_VAVGDiff = 0;
}

uint16_t VL6180_LiteFilter(uint16_t m_trueRange_mm, uint16_t m_rawRange_mm, uint32_t m_rtnSignalRate, uint32_t m_rtnAmbientRate, uint16_t errorCode)
{
    uint16_t m_newTrueRange_mm;
    uint16_t MaxOrInvalidDistance;

    uint16_t registerValue;
 //   uint16_t dataByte;
    uint32_t register32BitsValue1;
    uint32_t register32BitsValue2;
    uint16_t bypassFilter = 0;

    uint32_t VAVGDiff;
    uint32_t IdealVAVGDiff;
    uint32_t MinVAVGDiff;
    uint32_t MaxVAVGDiff;

    // Filter parameters
    uint16_t WrapAroundLowRawRangeLimit = 20;
    uint32_t WrapAroundLowReturnRateLimit = 800;
    uint16_t WrapAroundLowRawRangeLimit2 = 55;
    uint32_t WrapAroundLowReturnRateLimit2 = 300;

    uint32_t WrapAroundLowReturnRateFilterLimit = 600;
    uint16_t WrapAroundHighRawRangeFilterLimit = 350;
    uint32_t WrapAroundHighReturnRateFilterLimit = 900;

    uint32_t WrapAroundMaximumAmbientRateFilterLimit = 7500;

    uint32_t MAX_VAVGDiff = 1800;
    // End Filter parameters

    uint8_t WrapAroundDetected = 0;

    // Determines max distance
    MaxOrInvalidDistance = (uint16_t)(255 * 3);

    // Check if distance is Valid or not
    switch (errorCode)
    {
        case 0x0C:
            m_trueRange_mm = MaxOrInvalidDistance;
            break;
        case 0x0D:
            m_trueRange_mm = MaxOrInvalidDistance;
            break;
        default:
            if (m_rawRange_mm >= MaxOrInvalidDistance)
                m_trueRange_mm = MaxOrInvalidDistance;
            break;
    }

    if ((m_rawRange_mm < WrapAroundLowRawRangeLimit) && (m_rtnSignalRate < WrapAroundLowReturnRateLimit))
    {
        m_trueRange_mm = MaxOrInvalidDistance;
    }

    if ((m_rawRange_mm < WrapAroundLowRawRangeLimit2) && (m_rtnSignalRate < WrapAroundLowReturnRateLimit2))
    {
        m_newTrueRange_mm = MaxOrInvalidDistance;
    }

    bypassFilter = 0;

    if (m_rtnAmbientRate > WrapAroundMaximumAmbientRateFilterLimit)
    {
        // Too high ambient rate
        bypassFilter = 1;
    }

    if (!(((m_rawRange_mm < WrapAroundHighRawRangeFilterLimit) && (m_rtnSignalRate < WrapAroundLowReturnRateFilterLimit)) ||
        ((m_rawRange_mm >= WrapAroundHighRawRangeFilterLimit) && (m_rtnSignalRate < WrapAroundHighReturnRateFilterLimit))
        ))
        bypassFilter = 1;

    proxy_i2c_read( 0x01AC, &registerValue, 1);
    if (bypassFilter == 1)
    {
        // Do not go through the filter
        if (registerValue != 0x3E)
			proxy_i2c_write( 0x01AC, 0x3E, 1);
    }
    else
    {
        // Go through the filter
        #if 0
		proxy_i2c_read( 0x010C, &dataByte, 1);
        register32BitsValue1 = (uint32_t)dataByte << 24;
		proxy_i2c_read( 0x010D, &dataByte, 1);
        register32BitsValue1 |= (uint32_t)dataByte << 16;
		proxy_i2c_read( 0x010E, &dataByte, 1);
        register32BitsValue1 |= (uint32_t)dataByte << 8;
		proxy_i2c_read( 0x010F, &dataByte, 1);
        register32BitsValue1 |= dataByte;
	#else
	ProxyRead32bit(0x010C, &register32BitsValue1);
	#endif

	  #if 0
		proxy_i2c_read( 0x0110, &dataByte, 1);
        register32BitsValue2 = (uint32_t)dataByte << 24;
		proxy_i2c_read( 0x0111, &dataByte, 1);
        register32BitsValue2 |= (uint32_t)dataByte << 16;
		proxy_i2c_read( 0x0112, &dataByte, 1);
        register32BitsValue2 |= (uint32_t)dataByte << 8;
		proxy_i2c_read( 0x0113, &dataByte, 1);
        register32BitsValue2 |= dataByte;
	#else
	ProxyRead32bit(0x0110, &register32BitsValue2);
	#endif
        if (registerValue == 0x3E)
        {
            Default_ZeroVal = register32BitsValue1;
            Default_VAVGVal = register32BitsValue2;
			proxy_i2c_write( 0x01AC, 0x3F, 1);
        }
        else
        {
            NoDelay_ZeroVal = register32BitsValue1;
            NoDelay_VAVGVal = register32BitsValue2;
			proxy_i2c_write( 0x01AC, 0x3E, 1);
        }

        // Computes current VAVGDiff
        if (Default_VAVGVal > NoDelay_VAVGVal)
            VAVGDiff = Default_VAVGVal - NoDelay_VAVGVal;
        else
            VAVGDiff = 0;
        Previous_VAVGDiff = VAVGDiff;

        // Check the VAVGDiff
        IdealVAVGDiff = Default_ZeroVal - NoDelay_ZeroVal;
        if (IdealVAVGDiff > MAX_VAVGDiff)
            MinVAVGDiff = IdealVAVGDiff - MAX_VAVGDiff;
        else
            MinVAVGDiff = 0;
        MaxVAVGDiff = IdealVAVGDiff + MAX_VAVGDiff;
        if (VAVGDiff < MinVAVGDiff || VAVGDiff > MaxVAVGDiff)
            WrapAroundDetected = 1;
    }
    if (WrapAroundDetected == 1)
    {
        m_newTrueRange_mm = MaxOrInvalidDistance;
    }
    else
    {
        m_newTrueRange_mm = m_trueRange_mm;
    }
    MeasurementIndex = MeasurementIndex + 1;

    return m_newTrueRange_mm;
}

void VL6180_InitComplexFilter(void)
{
    int i;

    MeasurementIndex = 0;

    Default_ZeroVal = 0;
    Default_VAVGVal = 0;
    NoDelay_ZeroVal = 0;
    NoDelay_VAVGVal = 0;
    Previous_VAVGDiff = 0;

    StdFilteredReads = 0;
    PreviousRangeStdDev = 0;
    PreviousReturnRateStdDev = 0;

    for (i = 0; i < FILTERNBOFSAMPLES; i++)
    {
        LastTrueRange[i] = FILTERINVALIDDISTANCE;
        LastReturnRates[i] = 0;
    }
}

uint32_t VL6180_StdDevDamper(uint32_t AmbientRate, uint32_t SignalRate, uint32_t StdDevLimitLowLight, uint32_t StdDevLimitLowLightSNR, uint32_t StdDevLimitHighLight, uint32_t StdDevLimitHighLightSNR)
{
    uint32_t newStdDev;
    uint16_t SNR;

    if (AmbientRate > 0)
        SNR = (uint16_t)((100 * SignalRate) / AmbientRate);
    else
        SNR = 9999;

    if (SNR >= StdDevLimitLowLightSNR)
    {
        newStdDev = StdDevLimitLowLight;
    }
    else
    {
        if (SNR <= StdDevLimitHighLightSNR)
            newStdDev = StdDevLimitHighLight;
        else
        {
            newStdDev = (uint32_t)(StdDevLimitHighLight + (SNR - StdDevLimitHighLightSNR) * (int)(StdDevLimitLowLight - StdDevLimitHighLight) / (StdDevLimitLowLightSNR - StdDevLimitHighLightSNR));
        }
    }

    return newStdDev;
}
uint16_t VL6180_ComplexFilter(uint16_t m_trueRange_mm, uint16_t m_rawRange_mm, uint32_t m_rtnSignalRate, uint32_t m_rtnAmbientRate, uint16_t errorCode)
{
    uint16_t m_newTrueRange_mm = 0;

    uint16_t i;
    uint16_t bypassFilter = 0;

    uint16_t registerValue;
   // uint16_t dataByte;
    uint32_t register32BitsValue1;
    uint32_t register32BitsValue2;

    uint16_t ValidDistance = 0;
    uint16_t MaxOrInvalidDistance = 0;

    uint16_t WrapAroundFlag = 0;
    uint16_t NoWrapAroundFlag = 0;
    uint16_t NoWrapAroundHighConfidenceFlag = 0;

    uint16_t FlushFilter = 0;
    uint32_t RateChange = 0;

    uint16_t StdDevSamples = 0;
    uint32_t StdDevDistanceSum = 0;
    uint32_t StdDevDistanceMean = 0;
    uint32_t StdDevDistance = 0;
    uint32_t StdDevRateSum = 0;
    uint32_t StdDevRateMean = 0;
    uint32_t StdDevRate = 0;
    uint32_t StdDevLimitWithTargetMove = 0;

    uint32_t VAVGDiff;
    uint32_t IdealVAVGDiff;
    uint32_t MinVAVGDiff;
    uint32_t MaxVAVGDiff;

    // Filter Parameters
    uint16_t WrapAroundLowRawRangeLimit = 20;
    uint32_t WrapAroundLowReturnRateLimit = 800;
    uint16_t WrapAroundLowRawRangeLimit2 = 55;
    uint32_t WrapAroundLowReturnRateLimit2 = 300;

    uint32_t WrapAroundLowReturnRateFilterLimit = 600;
    uint16_t WrapAroundHighRawRangeFilterLimit = 350;
    uint32_t WrapAroundHighReturnRateFilterLimit = 900;

    uint32_t WrapAroundMaximumAmbientRateFilterLimit = 7500;

    // Temporal filter data and flush values
    uint32_t MinReturnRateFilterFlush = 75;
    uint32_t MaxReturnRateChangeFilterFlush = 50;

    // STDDEV values and damper values
    uint32_t StdDevLimit = 300;
    uint32_t StdDevLimitLowLight = 300;
    uint32_t StdDevLimitLowLightSNR = 30; // 0.3
    uint32_t StdDevLimitHighLight = 2500;
    uint32_t StdDevLimitHighLightSNR = 5; //0.05

    uint32_t StdDevHighConfidenceSNRLimit = 8;

    uint32_t StdDevMovingTargetStdDevLimit = 90000;
    uint32_t StdDevMovingTargetReturnRateLimit = 3500;
    uint32_t StdDevMovingTargetStdDevForReturnRateLimit = 5000;

    uint32_t MAX_VAVGDiff = 1800;

    // WrapAroundDetection variables
    uint16_t WrapAroundNoDelayCheckPeriod = 2;

    // Reads Filtering values
    uint16_t StdFilteredReadsIncrement = 2;
    uint16_t StdMaxFilteredReads = 4;

    // End Filter Parameters

    MaxOrInvalidDistance = (uint16_t)(255 * 3);

    // Check if distance is Valid or not
    switch (errorCode)
    {
        case 0x0C:
            m_trueRange_mm = MaxOrInvalidDistance;
            ValidDistance = 0;
            break;
        case 0x0D:
            m_trueRange_mm = MaxOrInvalidDistance;
            ValidDistance = 1;
            break;
        default:
            if (m_rawRange_mm >= MaxOrInvalidDistance)
            {
                ValidDistance = 0;
            }
            else
            {
                ValidDistance = 1;
            }
            break;
    }
    m_newTrueRange_mm = m_trueRange_mm;

    // Checks on low range data
    if ((m_rawRange_mm < WrapAroundLowRawRangeLimit) && (m_rtnSignalRate < WrapAroundLowReturnRateLimit))
    {
        //Not Valid distance
        m_newTrueRange_mm = MaxOrInvalidDistance;
        bypassFilter = 1;
    }
    if ((m_rawRange_mm < WrapAroundLowRawRangeLimit2) && (m_rtnSignalRate < WrapAroundLowReturnRateLimit2))
    {
        //Not Valid distance
        m_newTrueRange_mm = MaxOrInvalidDistance;
        bypassFilter = 1;
    }

    // Checks on Ambient rate level
    if (m_rtnAmbientRate > WrapAroundMaximumAmbientRateFilterLimit)
    {
        // Too high ambient rate
        FlushFilter = 1;
        bypassFilter = 1;
    }
    // Checks on Filter flush
    if (m_rtnSignalRate < MinReturnRateFilterFlush)
    {
        // Completely lost target, so flush the filter
        FlushFilter = 1;
        bypassFilter = 1;
    }
    if (LastReturnRates[0] != 0)
    {
        if (m_rtnSignalRate > LastReturnRates[0])
            RateChange = (100 * (m_rtnSignalRate - LastReturnRates[0])) / LastReturnRates[0];
        else
            RateChange = (100 * (LastReturnRates[0] - m_rtnSignalRate)) / LastReturnRates[0];
    }
    else
        RateChange = 0;
    if (RateChange > MaxReturnRateChangeFilterFlush)
    {
        FlushFilter = 1;
    }

    if (FlushFilter == 1)
    {
        MeasurementIndex = 0;
        for (i = 0; i < FILTERNBOFSAMPLES; i++)
        {
            LastTrueRange[i] = FILTERINVALIDDISTANCE;
            LastReturnRates[i] = 0;
        }
    }
    else
    {
        for (i = (uint16_t)(FILTERNBOFSAMPLES - 1); i > 0; i--)
        {
            LastTrueRange[i] = LastTrueRange[i - 1];
            LastReturnRates[i] = LastReturnRates[i - 1];
        }
    }
    if (ValidDistance == 1)
        LastTrueRange[0] = m_trueRange_mm;
    else
        LastTrueRange[0] = FILTERINVALIDDISTANCE;
    LastReturnRates[0] = m_rtnSignalRate;

    // Check if we need to go through the filter or not
    if (!(((m_rawRange_mm < WrapAroundHighRawRangeFilterLimit) && (m_rtnSignalRate < WrapAroundLowReturnRateFilterLimit)) ||
        ((m_rawRange_mm >= WrapAroundHighRawRangeFilterLimit) && (m_rtnSignalRate < WrapAroundHighReturnRateFilterLimit))
        ))
        bypassFilter = 1;

    // Check which kind of measurement has been made
	proxy_i2c_read( 0x01AC, &registerValue, 1);

    // Read data for filtering

	#if 0
	proxy_i2c_read( 0x010C, &dataByte, 1);
	register32BitsValue1 = (uint32_t)dataByte << 24;
	proxy_i2c_read( 0x010D, &dataByte, 1);
	register32BitsValue1 |= (uint32_t)dataByte << 16;
	proxy_i2c_read( 0x010E, &dataByte, 1);
	register32BitsValue1 |= (uint32_t)dataByte << 8;
	proxy_i2c_read( 0x010F, &dataByte, 1);
	register32BitsValue1 |= dataByte;
	#else
	ProxyRead32bit(0x010C, &register32BitsValue1);
	#endif

	#if 0
	proxy_i2c_read( 0x0110, &dataByte, 1);
	register32BitsValue2 = (uint32_t)dataByte << 24;
	proxy_i2c_read( 0x0111, &dataByte, 1);
	register32BitsValue2 |= (uint32_t)dataByte << 16;
	proxy_i2c_read( 0x0112, &dataByte, 1);
	register32BitsValue2 |= (uint32_t)dataByte << 8;
	proxy_i2c_read( 0x0113, &dataByte, 1);
	register32BitsValue2 |= dataByte;
	#else
	ProxyRead32bit(0x0110, &register32BitsValue2);
	#endif

    if (registerValue == 0x3E)
    {
        Default_ZeroVal = register32BitsValue1;
        Default_VAVGVal = register32BitsValue2;
    }
    else
    {
        NoDelay_ZeroVal = register32BitsValue1;
        NoDelay_VAVGVal = register32BitsValue2;
    }

    if (bypassFilter == 1)
    {
        // Do not go through the filter
        if (registerValue != 0x3E)
			proxy_i2c_write( 0x01AC, 0x3E, 1);
        // Set both Defaut and NoDelay To same value
        Default_ZeroVal = register32BitsValue1;
        Default_VAVGVal = register32BitsValue2;
        NoDelay_ZeroVal = register32BitsValue1;
        NoDelay_VAVGVal = register32BitsValue2;
        MeasurementIndex = 0;

        // Return immediately
        return m_newTrueRange_mm;
    }

    if (MeasurementIndex % WrapAroundNoDelayCheckPeriod == 0)
	proxy_i2c_write( 0x01AC, 0x3F, 1);
    else

	proxy_i2c_write( 0x01AC, 0x3E, 1);

    MeasurementIndex = (uint16_t)(MeasurementIndex + 1);

    // Computes current VAVGDiff
    if (Default_VAVGVal > NoDelay_VAVGVal)
        VAVGDiff = Default_VAVGVal - NoDelay_VAVGVal;
    else
        VAVGDiff = 0;
    Previous_VAVGDiff = VAVGDiff;

    // Check the VAVGDiff
    if(Default_ZeroVal>NoDelay_ZeroVal)
        IdealVAVGDiff = Default_ZeroVal - NoDelay_ZeroVal;
    else
        IdealVAVGDiff = NoDelay_ZeroVal - Default_ZeroVal;
    if (IdealVAVGDiff > MAX_VAVGDiff)
        MinVAVGDiff = IdealVAVGDiff - MAX_VAVGDiff;
    else
        MinVAVGDiff = 0;
    MaxVAVGDiff = IdealVAVGDiff + MAX_VAVGDiff;
    if (VAVGDiff < MinVAVGDiff || VAVGDiff > MaxVAVGDiff)
    {
        WrapAroundFlag = 1;
    }
    else
    {
        // Go through filtering check

        // StdDevLimit Damper on SNR
        StdDevLimit = VL6180_StdDevDamper(m_rtnAmbientRate, m_rtnSignalRate, StdDevLimitLowLight, StdDevLimitLowLightSNR, StdDevLimitHighLight, StdDevLimitHighLightSNR);

        // Standard deviations computations
        StdDevSamples = 0;
        StdDevDistanceSum = 0;
        StdDevDistanceMean = 0;
        StdDevDistance = 0;
        StdDevRateSum = 0;
        StdDevRateMean = 0;
        StdDevRate = 0;
        for (i = 0; (i < FILTERNBOFSAMPLES) && (StdDevSamples < FILTERSTDDEVSAMPLES); i++)
        {
            if (LastTrueRange[i] != FILTERINVALIDDISTANCE)
            {
                StdDevSamples = (uint16_t)(StdDevSamples + 1);
                StdDevDistanceSum = (uint32_t)(StdDevDistanceSum + LastTrueRange[i]);
                StdDevRateSum = (uint32_t)(StdDevRateSum + LastReturnRates[i]);
            }
        }
        if (StdDevSamples > 0)
        {
            StdDevDistanceMean = (uint32_t)(StdDevDistanceSum / StdDevSamples);
            StdDevRateMean = (uint32_t)(StdDevRateSum / StdDevSamples);
        }
        StdDevSamples = 0;
        StdDevDistanceSum = 0;
        StdDevRateSum = 0;
        for (i = 0; (i < FILTERNBOFSAMPLES) && (StdDevSamples < FILTERSTDDEVSAMPLES); i++)
        {
            if (LastTrueRange[i] != FILTERINVALIDDISTANCE)
            {
                StdDevSamples = (uint16_t)(StdDevSamples + 1);
                StdDevDistanceSum = (uint32_t)(StdDevDistanceSum + (int)(LastTrueRange[i] - StdDevDistanceMean) * (int)(LastTrueRange[i] - StdDevDistanceMean));
                StdDevRateSum = (uint32_t)(StdDevRateSum + (int)(LastReturnRates[i] - StdDevRateMean) * (int)(LastReturnRates[i] - StdDevRateMean));
            }
        }
        if (StdDevSamples >= MINFILTERSTDDEVSAMPLES)
        {
            StdDevDistance = (uint16_t)(StdDevDistanceSum / StdDevSamples);
            StdDevRate = (uint16_t)(StdDevRateSum / StdDevSamples);
        }
        else
        {
			StdDevDistance = 0;
			StdDevRate = 0;
        }

        // Check Return rate standard deviation
        if (StdDevRate < StdDevMovingTargetStdDevLimit)
        {
            if (StdDevSamples < MINFILTERVALIDSTDDEVSAMPLES)
            {
				if(MeasurementIndex<FILTERSTDDEVSAMPLES)
					// Not enough samples to check on standard deviations
					m_newTrueRange_mm = 800;
				else
					m_newTrueRange_mm = MaxOrInvalidDistance;
            }
            else
            {
                // Check distance standard deviation
                if (StdDevRate < StdDevMovingTargetReturnRateLimit)
                    StdDevLimitWithTargetMove = StdDevLimit + (((StdDevMovingTargetStdDevForReturnRateLimit - StdDevLimit) * StdDevRate) / StdDevMovingTargetReturnRateLimit);
                else
                    StdDevLimitWithTargetMove = StdDevMovingTargetStdDevForReturnRateLimit;

                if ((StdDevDistance * StdDevHighConfidenceSNRLimit) < StdDevLimitWithTargetMove)
                {
                    NoWrapAroundHighConfidenceFlag = 1;
                }
                else
                {
                    if (StdDevDistance < StdDevLimitWithTargetMove)
                    {
                        if (StdDevSamples >= MINFILTERVALIDSTDDEVSAMPLES)
                        {
                            NoWrapAroundFlag = 1;
                        }
                        else
                        {
                            m_newTrueRange_mm = MaxOrInvalidDistance;
                        }
                    }
                    else
                    {
                        WrapAroundFlag = 1;
                    }
                }
            }
        }
        else
        {
            WrapAroundFlag = 1;
        }
    }

    if (m_newTrueRange_mm == MaxOrInvalidDistance)
    {
        if (StdFilteredReads > 0)
            StdFilteredReads = (uint16_t)(StdFilteredReads - 1);
    }
    else
    {
        if (WrapAroundFlag == 1)
        {
            m_newTrueRange_mm = MaxOrInvalidDistance;
            StdFilteredReads = (uint16_t)(StdFilteredReads + StdFilteredReadsIncrement);
            if (StdFilteredReads > StdMaxFilteredReads)
                StdFilteredReads = StdMaxFilteredReads;
        }
        else
        {
            if (NoWrapAroundFlag == 1)
            {
                if (StdFilteredReads > 0)
                {
                    m_newTrueRange_mm = MaxOrInvalidDistance;
                    if (StdFilteredReads > StdFilteredReadsIncrement)
                        StdFilteredReads = (uint16_t)(StdFilteredReads - StdFilteredReadsIncrement);
                    else
                        StdFilteredReads = 0;
                }
            }
            else
            {
                if (NoWrapAroundHighConfidenceFlag == 1)
                {
                    StdFilteredReads = 0;
                }
            }
        }

    }
    PreviousRangeStdDev = StdDevDistance;
    PreviousReturnRateStdDev = StdDevRate;
    PreviousStdDevLimit = StdDevLimitWithTargetMove;

    return m_newTrueRange_mm;
}


static long msm_proxy_subdev_ioctl(struct v4l2_subdev *sd,
			unsigned int cmd, void *arg)
{
	struct msm_proxy_ctrl_t *a_ctrl = v4l2_get_subdevdata(sd);
	void __user *argp = (void __user *)arg;
	CDBG("Enter\n");
	CDBG("%s:%d a_ctrl %p argp %p\n", __func__, __LINE__, a_ctrl, argp);
	return -ENOIOCTLCMD;
}

static int32_t msm_proxy_power(struct v4l2_subdev *sd, int on)
{
	int rc = 0;
	pr_err("Enter, on %d\n", on);
	rc = stop_proxy();
	msm_proxy_t.exit_workqueue = 1;
	pr_err("Exit, exit_workqueue %d\n", msm_proxy_t.exit_workqueue);
	return rc;
}

static struct v4l2_subdev_core_ops msm_proxy_subdev_core_ops = {
	.ioctl = msm_proxy_subdev_ioctl,
	.s_power = msm_proxy_power,
};

static struct v4l2_subdev_ops msm_proxy_subdev_ops = {
	.core = &msm_proxy_subdev_core_ops,
};

static struct msm_proxy_ctrl_t msm_proxy_t = {
	.i2c_driver = &msm_proxy_i2c_driver,
	.pdriver = &msm_proxy_platform_driver,
	.act_v4l2_subdev_ops = &msm_proxy_subdev_ops,
	//.proxy_mutex = &msm_proxy_mutex,
};

module_init(msm_proxy_init_module);
MODULE_DESCRIPTION("MSM PROXY");
MODULE_LICENSE("GPL v2");

/*
 *  include/linux/power/rt9428_battery.h
 *  Include header file to Richtek RT9428 fuelgauge driver
 *
 *  Copyright (C) 2014 Richtek Technology Corp.
 *  cy_huang <cy_huang@richtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef __LINUX_POWER_RT9428_BATTERY_H
#define __LINUX_POWER_RT9428_BATTERY_H

#define RT9428_DEVICE_NAME "rt9428"
#define RT9428_DRV_VER	   "1.0.3_G"
#define CONFIG_LGE_PM_RT9428_POLLING
#define CONFIG_LGE_PM_RT9428_SHUTDOWN_SOC_USED
enum {
	RT9428_REG_VBATH = 0x02,
	RT9428_REG_RANGE1_START = RT9428_REG_VBATH,
	RT9428_REG_VBATL,
	RT9428_REG_SOCH,
	RT9428_REG_SOCL,
	RT9428_REG_CTRLH,
	RT9428_REG_CTRLL,
	RT9428_REG_DEVID0,
	RT9428_REG_DEVID1,
	RT9428_REG_STATUS,
	RT9428_REG_CRATE,
	RT9428_REG_CFG0,
	RT9428_REG_CFG1,
	RT9428_REG_OCVH,
	RT9428_REG_OCVL,
	RT9428_REG_RANGE1_STOP = RT9428_REG_OCVL,
	RT9428_REG_MFAH = 0xFE,
	RT9428_REG_RANGE2_START = RT9428_REG_MFAH,
	RT9428_REG_MFAL,
	RT9428_REG_RANGE2_STOP = RT9428_REG_MFAL,
	RT9428_REG_MAX,
};

#define RT9428_SMOOTH_POLL	20
#define RT9428_NORMAL_POLL	30
#define RT9428_SOCALRT_MASK	0x20
#define RT9428_SOCL_SHFT	0
#define RT9428_SOCL_MASK	0x1F
#define RT9428_SOCL_MAX		32
#define RT9428_SOCL_MIN		1

struct rt9428_platform_data {
	unsigned int full_design;
	/* unsigned short vgcomp0; */
	unsigned int vgpara1_hcap;
	unsigned int vgpara2_hcap;
	unsigned int vgpara3_hcap;
	unsigned int vgpara4_hcap;
	unsigned int vgpara5_hcap;
	unsigned int vgpara1_lcap;
	unsigned int vgpara2_lcap;
	unsigned int vgpara3_lcap;
	unsigned int vgpara4_lcap;
	unsigned int vgpara5_lcap;
	int r1_gain_tempcold_hcap;
	int r1_gain_temphot_hcap;
	int r1_gain_tempcold2_hcap;
	int r2_gain_tempcold_hcap;
	int r2_gain_temphot_hcap;
	int r2_gain_tempcold2_hcap;
	int r3_gain_tempcold_hcap;
	int r3_gain_temphot_hcap;
	int r3_gain_tempcold2_hcap;
	int r4_gain_tempcold_hcap;
	int r4_gain_temphot_hcap;
	int r4_gain_tempcold2_hcap;
	int r1_gain_tempcold_lcap;
	int r1_gain_temphot_lcap;
	int r1_gain_tempcold2_lcap;
	int r2_gain_tempcold_lcap;
	int r2_gain_temphot_lcap;
	int r2_gain_tempcold2_lcap;
	int r3_gain_tempcold_lcap;
	int r3_gain_temphot_lcap;
	int r3_gain_tempcold2_lcap;
	int r4_gain_tempcold_lcap;
	int r4_gain_temphot_lcap;
	int r4_gain_tempcold2_lcap;
	unsigned int temp_base;
	unsigned int high_temp_base;
	unsigned int low_temp_base;
	unsigned int low_temp2_base;
	int alert_threshold;
	int soc_comp;
	int alert_gpio;
#ifdef CONFIG_LGE_PM_RT9428_CUT_OFF_UNDER_3P2_USED
	int low_cut_off_gain;
#endif
};

#ifdef CONFIG_RT9428_FGDBG
#define RT_DBG(format, args...) \
	pr_info("%s:%s() line-%d: " format, RT9428_DEVICE_NAME, __func__, \
		__LINE__, ##args)
#else
#define RT_DBG(format, args...)
#endif /* CONFIG_RT9428_FGDBG */

#endif /* #ifndef __LINUX_POWER_RT9428_BATTERY_H */

#define RT9428_BATTERY_LGC 1
#define RT9428_BATTERY_TOCAD 2
#define RT9428_BATTERY_BYD 3
#define RT9428_BATTERY_BYD_TOCAD 4
#define RT9428_BATTERY_TECHNOPHILE 5
#define RT9428_BATTERY_DEFAULT 10

#define RT9428_DEFAULT_SOC 70
#define RT9428_DEFAULT_VCELL 4000
#define BMS1_BMS_DATA_REG_0 0x40B0

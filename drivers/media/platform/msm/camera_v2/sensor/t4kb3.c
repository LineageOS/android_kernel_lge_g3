/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include "msm_sensor.h"
#include <mach/board_lge.h>		//to use lge_get_board_revno()

#define T4KB3_SENSOR_NAME "t4kb3"
DEFINE_MSM_MUTEX(t4kb3_mut);

// LGE_CHNAGE_S sungsik.kim 2013/04/07 {
#define CONFIG_T4KB3_DEBUG
// LGE_CHNAGE_E sungsik.kim 2013/04/07 }

#undef CDBG
#ifdef CONFIG_T4KB3_DEBUG
#define CDBG(fmt, args...) pr_err(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#endif

static struct msm_sensor_ctrl_t t4kb3_s_ctrl;

static struct msm_sensor_power_setting t4kb3_power_setting[] =
	{
		{  /* Set GPIO_RESET to low to disable power on reset*/
			.seq_type = SENSOR_GPIO,
			.seq_val = SENSOR_GPIO_RESET,
			.config_val = GPIO_OUT_LOW,
			.delay = 1,
		},
#if defined(CONFIG_LG_OIS)
		{	// OIS_RESET
			.seq_type = SENSOR_GPIO,
			.seq_val = SENSOR_GPIO_OIS_RESET,
			.config_val = GPIO_OUT_LOW,
			.delay = 1,
		},
#endif
		{										//VIO, GPIO 96
			.seq_type = SENSOR_GPIO,
			.seq_val = SENSOR_GPIO_VIO,
			.config_val = GPIO_OUT_HIGH,
			.delay = 1,
		},
		{										//VDIG
			.seq_type = SENSOR_VREG,
			.seq_val = CAM_VDIG,
			.config_val = 0,
			.delay = 1,
		},
		{										//VANA, GPIO 16
			.seq_type = SENSOR_GPIO,
			.seq_val = SENSOR_GPIO_VANA,
			.config_val = GPIO_OUT_HIGH,
			.delay = 2,
		},
#if defined(CONFIG_LG_PROXY)
		{										//LDAF_EN, PMIC_GPIO 1
			.seq_type = SENSOR_GPIO,
			.seq_val = SENSOR_GPIO_LDAF_EN,
			.config_val = GPIO_OUT_HIGH,
			.delay = 3,
		},
#endif
		{										//VCM, GPIO 145
			.seq_type = SENSOR_GPIO,
			.seq_val = SENSOR_GPIO_VAF,
			.config_val = GPIO_OUT_HIGH,
			.delay = 3,
		},
#if defined(CONFIG_LG_OIS)
		{										//OIS_LDO_EN, GPIO 30
			.seq_type = SENSOR_GPIO,
			.seq_val = SENSOR_GPIO_OIS_LDO_EN,
			.config_val = GPIO_OUT_HIGH,
			.delay = 1,
		},
#endif
		{
			.seq_type = SENSOR_CLK,
			.seq_val = SENSOR_CAM_MCLK,
			.config_val = 0,
			.delay = 1,
		},
		{
			.seq_type = SENSOR_GPIO,
			.seq_val = SENSOR_GPIO_RESET,
			.config_val = GPIO_OUT_HIGH,
			.delay = 1,
		},
#if defined(CONFIG_LG_OIS)
		{
			.seq_type = SENSOR_GPIO,
			.seq_val = SENSOR_GPIO_OIS_RESET,
			.config_val = GPIO_OUT_HIGH,
			.delay = 1,
		},
#endif
		{
			.seq_type = SENSOR_I2C_MUX,
			.seq_val = 0,
			.config_val = 0,
			.delay = 1,
		},
	};

static struct v4l2_subdev_info t4kb3_subdev_info[] = {
	{
		.code = V4L2_MBUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt = 1,
		.order = 0,
	},
};

static const struct i2c_device_id t4kb3_i2c_id[] = {
	{T4KB3_SENSOR_NAME, (kernel_ulong_t)&t4kb3_s_ctrl},
	{ }
};

static int32_t msm_t4kb3_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	return msm_sensor_i2c_probe(client, id, &t4kb3_s_ctrl);
}

static struct i2c_driver t4kb3_i2c_driver = {
	.id_table = t4kb3_i2c_id,
	.probe  = msm_t4kb3_i2c_probe,
	.driver = {
		.name = T4KB3_SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client t4kb3_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static const struct of_device_id t4kb3_dt_match[] = {
	{.compatible = "qcom,t4kb3", .data = &t4kb3_s_ctrl},
	{}
};

MODULE_DEVICE_TABLE(of, t4kb3_dt_match);

static struct platform_driver t4kb3_platform_driver = {
	.driver = {
		.name = "qcom,t4kb3",
		.owner = THIS_MODULE,
		.of_match_table = t4kb3_dt_match,
	},
};

static int32_t t4kb3_platform_probe(struct platform_device *pdev)
{
	int32_t rc = 0;
	const struct of_device_id *match;
	CDBG("%s E\n", __func__);
	match = of_match_device(t4kb3_dt_match, &pdev->dev);
/* LGE_CHANGE_S : WBT, 2013-5-31, jonghwan.ko@lge.com */
	if(!match)
	{
	      pr_err(" %s failed ",__func__);
	      return -ENODEV;
       }
/* LGE_CHANGE_E : WBT, 2013-5-31, jonghwan.ko@lge.com */
	rc = msm_sensor_platform_probe(pdev, match->data);
	CDBG("%s: X, rc = %d\n", __func__, rc);
	return rc;
}

static int __init t4kb3_init_module(void)
{
	int32_t rc = 0;
	CDBG("%s E\n", __func__);

	rc = platform_driver_probe(&t4kb3_platform_driver,
		t4kb3_platform_probe);
	if (!rc) {
		CDBG("%s: X, rc = %d\n", __func__, rc);
		return rc;
	}
	return i2c_add_driver(&t4kb3_i2c_driver);
}

static void __exit t4kb3_exit_module(void)
{
	if (t4kb3_s_ctrl.pdev) {
		msm_sensor_free_sensor_data(&t4kb3_s_ctrl);
		platform_driver_unregister(&t4kb3_platform_driver);
	} else
		i2c_del_driver(&t4kb3_i2c_driver);
	return;
}

static struct msm_sensor_ctrl_t t4kb3_s_ctrl = {
	.sensor_i2c_client = &t4kb3_sensor_i2c_client,
	.power_setting_array.power_setting = t4kb3_power_setting,
	.power_setting_array.size = ARRAY_SIZE(t4kb3_power_setting),
	.msm_sensor_mutex = &t4kb3_mut,
	.sensor_v4l2_subdev_info = t4kb3_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(t4kb3_subdev_info),
};

module_init(t4kb3_init_module);
module_exit(t4kb3_exit_module);
MODULE_DESCRIPTION("t4kb3");
MODULE_LICENSE("GPL v2");

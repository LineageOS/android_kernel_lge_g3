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
#define OV2740_SENSOR_NAME "ov2740"
DEFINE_MSM_MUTEX(ov2740_mut);

#define CONFIG_OV2740_DEBUG
#undef CDBG
#ifdef CONFIG_OV2740_DEBUG
#define CDBG(fmt, args...) pr_err(fmt, ##args)
#else
#define CDBG(fmt, args...) do { } while (0)
#endif
static struct msm_sensor_ctrl_t ov2740_s_ctrl;

static struct msm_sensor_power_setting ov2740_power_setting[] = {
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_RESET,
		.config_val = GPIO_OUT_LOW,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_CLK,
		.seq_val = SENSOR_CAM_MCLK,
		.config_val = 0,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_VREG,
		.seq_val = CAM_VANA,
		.config_val = 0,
		.delay = 0,
	},
	{
		.seq_type = SENSOR_VREG,
		.seq_val = CAM_VIO,
		.config_val = 0,
		.delay = 1,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_VDIG,
		.config_val = GPIO_OUT_HIGH,
		.delay = 10,
	},
	{
		.seq_type = SENSOR_GPIO,
		.seq_val = SENSOR_GPIO_RESET,
		.config_val = GPIO_OUT_HIGH,
		.delay = 5,
	},
	{
		.seq_type = SENSOR_I2C_MUX,
		.seq_val = 0,
		.config_val = 0,
		.delay = 1,
	},
};

static struct v4l2_subdev_info ov2740_subdev_info[] = {
	{
		.code = V4L2_MBUS_FMT_SBGGR10_1X10,
		.colorspace = V4L2_COLORSPACE_JPEG,
		.fmt = 1,
		.order = 0,
	},
};

static const struct i2c_device_id ov2740_i2c_id[] = {
	{OV2740_SENSOR_NAME, (kernel_ulong_t)&ov2740_s_ctrl},
	{ }
};

static int32_t msm_ov2740_i2c_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	return msm_sensor_i2c_probe(client, id, &ov2740_s_ctrl);
}

static struct i2c_driver ov2740_i2c_driver = {
	.id_table = ov2740_i2c_id,
	.probe  = msm_ov2740_i2c_probe,
	.driver = {
		.name = OV2740_SENSOR_NAME,
	},
};

static struct msm_camera_i2c_client ov2740_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

static const struct of_device_id ov2740_dt_match[] = {
	{.compatible = "qcom,ov2740", .data = &ov2740_s_ctrl},
	{}
};

MODULE_DEVICE_TABLE(of, ov2740_dt_match);

static struct platform_driver ov2740_platform_driver = {
	.driver = {
		.name = "qcom,ov2740",
		.owner = THIS_MODULE,
		.of_match_table = ov2740_dt_match,
	},
};

static int32_t ov2740_platform_probe(struct platform_device *pdev)
{
	int32_t rc = 0;
	const struct of_device_id *match;

	CDBG("%s E\n", __func__);
	match = of_match_device(ov2740_dt_match, &pdev->dev);
	if(!match)
	{
		  pr_err(" %s failed ",__func__);
		  return -ENODEV;
	 }
	rc = msm_sensor_platform_probe(pdev, match->data);

	CDBG("%s: X, rc = %d\n", __func__, rc);
	return rc;
}

static int __init ov2740_init_module(void)
{
	int32_t rc = 0;
	pr_info("%s:%d\n", __func__, __LINE__);

	rc = platform_driver_probe(&ov2740_platform_driver,
		ov2740_platform_probe);
	if (!rc)
		return rc;
	pr_info("%s:%d rc %d\n", __func__, __LINE__, rc);
	return i2c_add_driver(&ov2740_i2c_driver);
}

static void __exit ov2740_exit_module(void)
{
	pr_info("%s:%d\n", __func__, __LINE__);
	if (ov2740_s_ctrl.pdev) {
		msm_sensor_free_sensor_data(&ov2740_s_ctrl);
		platform_driver_unregister(&ov2740_platform_driver);
	} else
		i2c_del_driver(&ov2740_i2c_driver);
	return;
}

static struct msm_sensor_ctrl_t ov2740_s_ctrl = {
	.sensor_i2c_client = &ov2740_sensor_i2c_client,
	.power_setting_array.power_setting = ov2740_power_setting,
	.power_setting_array.size = ARRAY_SIZE(ov2740_power_setting),
	.msm_sensor_mutex = &ov2740_mut,
	.sensor_v4l2_subdev_info = ov2740_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(ov2740_subdev_info),
};

module_init(ov2740_init_module);
module_exit(ov2740_exit_module);
MODULE_DESCRIPTION("ov2740");
MODULE_LICENSE("GPL v2");


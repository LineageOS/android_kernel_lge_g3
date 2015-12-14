/*
 * Copyright (C) 2009 Google, Inc.
 * Copyright (C) 2009 HTC Corporation.
 * Copyright (C) 2011 Broadcom Corporation.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*
 *  P6 Department Lab, 5 Team, Mobile Communication Company, LG ELECTRONICS INC., SEOUL, KOREA
 *  This software is used for bluetooth HW enable/disable control,
 *
 *  2013.01 For G2 Bluetooth based on BCM4335
 *
 */

#include <linux/delay.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/rfkill.h>
#include <asm/gpio.h>
#include <asm/mach-types.h>
#include <linux/regulator/consumer.h>
#include <linux/regulator/machine.h>

#define GPIO_BT_RESET_N     41
#define BTRFKILLDBG(fmt, arg...)

static struct rfkill *bt_rfk;
static const char bt_name[] = "brcm_Bluetooth_rfkill";

static int bluetooth_set_power(void *data, bool blocked)
{
	BTRFKILLDBG("bluetooth_set_power set blocked=%d", blocked);
	if (!blocked) {
		gpio_direction_output(GPIO_BT_RESET_N, 0);
		msleep(30);
		gpio_direction_output(GPIO_BT_RESET_N, 1);
		BTRFKILLDBG("Bluetooth RESET HIGH!!");
	} else {
		gpio_direction_output(GPIO_BT_RESET_N, 0);
		BTRFKILLDBG("Bluetooth RESET LOW!!");
	}
	return 0;
}

static struct rfkill_ops bluetooth_rfkill_ops = {
	.set_block = bluetooth_set_power,
};

static struct platform_device bcm_ldisc_device = {
	.name = "bcm_ldisc",
	.id = -1,
	.dev = {
	},
};

static int bluetooth_rfkill_probe(struct platform_device *pdev)
{
	int rc = 0;
	bool default_state = true;  /* off */

	BTRFKILLDBG("bluetooth_rfkill_probe");
	rc = gpio_request(GPIO_BT_RESET_N, "bt_reset");

	if (rc) {
		BTRFKILLDBG("GPIO req error no=%d", rc);
		gpio_free(GPIO_BT_RESET_N);
		rc = gpio_request(GPIO_BT_RESET_N, "bt_reset");
		if (rc) {
			BTRFKILLDBG("GPIO req error no=%d", rc);
			goto err_gpio_reset;
		}
	}
	gpio_direction_output(GPIO_BT_RESET_N, 0);

	bluetooth_set_power(NULL, default_state);

	bt_rfk = rfkill_alloc(bt_name, &pdev->dev, RFKILL_TYPE_BLUETOOTH,
			&bluetooth_rfkill_ops, NULL);
	if (!bt_rfk) {
		BTRFKILLDBG("rfkill alloc failed.");
		rc = -ENOMEM;
		goto err_rfkill_alloc;
	}

	rfkill_set_states(bt_rfk, default_state, false);

	/* userspace cannot take exclusive control */

	rc = rfkill_register(bt_rfk);
	if (rc)
		goto err_rfkill_reg;

	platform_device_register(&bcm_ldisc_device);

	return 0;

err_rfkill_reg:
	rfkill_destroy(bt_rfk);
err_rfkill_alloc:
err_gpio_reset:
	gpio_free(GPIO_BT_RESET_N);
	BTRFKILLDBG("bluetooth_rfkill_probe error!");
	return rc;
}

static int bluetooth_rfkill_remove(struct platform_device *dev)
{
	rfkill_unregister(bt_rfk);
	rfkill_destroy(bt_rfk);
	gpio_free(GPIO_BT_RESET_N);

	return 0;
}
struct bluetooth_rfkill_platform_data {
	int gpio_reset;
};

static struct bluetooth_rfkill_platform_data bluetooth_rfkill_platform;

static struct platform_device bluetooth_rfkill_device = {
	.name = "bluetooth_rfkill",
	.id   = -1,
	.dev = {
		.platform_data = &bluetooth_rfkill_platform,
	},
};

static struct platform_driver bluetooth_rfkill_driver = {
	.probe = bluetooth_rfkill_probe,
	.remove = bluetooth_rfkill_remove,
	.driver = {
		.name = "bluetooth_rfkill",
		.owner = THIS_MODULE,
	},
};

static int __init bluetooth_rfkill_init(void)
{
	int ret;
	BTRFKILLDBG("bluetooth_rfkill_init.");
	ret = platform_driver_register(&bluetooth_rfkill_driver);
	if (ret) {
		BTRFKILLDBG("Fail to register rfkill platform driver");
	}
	BTRFKILLDBG("bluetooth_rfkill_init done");

	return platform_device_register(&bluetooth_rfkill_device);;
}

static void __exit bluetooth_rfkill_exit(void)
{
	platform_device_unregister(&bluetooth_rfkill_device);
	platform_driver_unregister(&bluetooth_rfkill_driver);
}

device_initcall(bluetooth_rfkill_init);
module_exit(bluetooth_rfkill_exit);

MODULE_DESCRIPTION("bluetooth rfkill");
MODULE_AUTHOR("Nick Pelly <npelly@google.com>");
MODULE_LICENSE("GPL");

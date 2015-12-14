/*
 * drivers/arch/arm/mach-msm/lge/lge_misc_ftm_item.c
 *
 * Copyright (C) 2014 LGE, Inc
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/of_fdt.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <asm/uaccess.h>
#include <mach/board_lge.h>
#include <linux/syscalls.h>

#define MODULE_NAME "misc_checker"
#define MISC_PATH "/dev/block/bootdevice/by-name/misc"
#define FTM_ITEM_NAME_MAX_LEN   32
#define LGFTM_ITEM_MAX          3585
#define LGFTM_REBOOT_REASON_SIZE	4
#define LGFTM_REBOOT_REASON_ID	 128
#define FTM_BLOCK_SIZE		2048

static int set_ftm_item(const char *in, struct kernel_param *kp)
{
	int fd;
	char buf[LGFTM_REBOOT_REASON_SIZE];
	int offset = 0;

	mm_segment_t oldfs;

	oldfs = get_fs();
	set_fs(KERNEL_DS);

	fd = sys_open(MISC_PATH, O_WRONLY, 0);

	if (fd < 0) {
		pr_err("[%s %d] sys_open error(%d)\n", __func__, __LINE__, fd);
	}
	else {
		memset(buf, 0, LGFTM_REBOOT_REASON_SIZE);
		offset = FTM_BLOCK_SIZE * LGFTM_REBOOT_REASON_ID ;
		memcpy(buf, in, LGFTM_REBOOT_REASON_SIZE);

		sys_lseek(fd, offset, 0);

	if (sys_write(fd, buf, LGFTM_REBOOT_REASON_SIZE) != LGFTM_REBOOT_REASON_SIZE)
		pr_err("[%s, line %d] sys_write error\n", __func__, __LINE__);
	else
		pr_info("[%s,line %d] write reboot reason value [0x%02x%02x%02x%02x]\n", __func__, __LINE__,buf[0],buf[1],buf[2],buf[3]);

	sys_close(fd);
	}
	return 0;
}

int set_reboot_reason_ftm_item(int reason)
{
	char buf[LGFTM_REBOOT_REASON_SIZE];

	buf[0] = (reason >> 24) & 0xFF;
	buf[1] = (reason >> 16) & 0xFF;
	buf[2] = (reason >>  8) & 0xFF;
	buf[3] = (reason      ) & 0xFF;

	set_ftm_item(buf,0);

	pr_err("%s 0x%08x\n",__func__,reason);

	return 0;
}

EXPORT_SYMBOL(set_reboot_reason_ftm_item);

static int misc_ftm_item_probe(struct platform_device *pdev)
{
	return 0;
}

static int misc_ftm_item_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver misc_ftm_item_driver = {
	.probe = misc_ftm_item_probe,
	.remove = misc_ftm_item_remove,
	.driver = {
		.name = MODULE_NAME,
		.owner = THIS_MODULE,
	},
};

static int __init misc_ftm_item_init(void)
{
	return platform_driver_register(&misc_ftm_item_driver);
}

static void __exit misc_ftm_item_exit(void)
{
	platform_driver_unregister(&misc_ftm_item_driver);
}

module_init(misc_ftm_item_init);
module_exit(misc_ftm_item_exit);

MODULE_DESCRIPTION("LGE Misc FTM Item");
MODULE_AUTHOR("<mina.park@lge.com>");
MODULE_LICENSE("GPL");

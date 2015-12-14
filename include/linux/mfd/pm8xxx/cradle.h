/* include/linux/mfd/pm8xxx/cradle.h
 *
 * Copyright (c) 2011-2012, LG Electronics Inc, All rights reserved.
 * Author: Fred Cho <fred.cho@lge.com>
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

#ifndef __PM8XXX_CRADLE_H__
#define __PM8XXX_CRADLE_H__

#define HALL_IC_DEV_NAME "bu52031nvx"
#define CARKIT_DEV_NAME  "bu52031nvx-carkit"

/* SMART COVER Support */
#define SMARTCOVER_NO_DEV			0
#define SMARTCOVER_POUCH_CLOSED		1
#define SMARTCOVER_POUCH_OPENED		0
#define SMARTCOVER_CAMERA_CLOSED	1
#define SMARTCOVER_CAMERA_OPENED	2
#define SMARTCOVER_CAMERA_VIEW		256

/* Carkit support -only VZW*/
#ifdef CONFIG_BU52031NVX_CARKIT
#define	CARKIT_NO_DEV				0	// only VZW
#define	CARKIT_DESKDOCK				1	// only VZW
#define CARKIT_DOCKED				2	// only VZW
#endif

struct pm8xxx_cradle_platform_data {
	int hallic_pouch_detect_pin;
	unsigned int hallic_pouch_irq;
	int hallic_camera_detect_pin;
	unsigned int hallic_camera_irq;
	unsigned long irq_flags;
};

struct pm8xxx_carkit_platform_data {
	int hallic_carkit_detect_pin;
	unsigned int hallic_carkit_irq;
	unsigned long irq_flags;
};

void cradle_set_deskdock(int state);
int cradle_get_deskdock(void);

#ifdef CONFIG_BU52031NVX_CARKIT
void carkit_set_deskdock(int state);
int carkit_get_deskdock(void);
#endif
int get_smartcover_status(void);

#endif /* __PM8XXX_CRADLE_H__ */

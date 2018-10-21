/*
 *  Copyright (C) 2012-2013 Samsung Electronics Co., Ltd.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _EXFAT_DATA_H
#define _EXFAT_DATA_H
#include "exfat_config.h"
#include "exfat_global.h"
#ifdef __cplusplus
extern "C" {
#endif
#define MAX_DEVICE              2
#if defined (CONFIG_LGE_FS_INCREASE_MAX_DRIVE)
#define MAX_DRIVE               4
#else
#define MAX_DRIVE               2
#endif
#define MAX_OPEN                20
#define MAX_DENTRY              512
#define FAT_CACHE_SIZE          128
#define FAT_CACHE_HASH_SIZE     64
#define BUF_CACHE_SIZE          256
#define BUF_CACHE_HASH_SIZE     64
#define DEFAULT_CODEPAGE        437
#define DEFAULT_IOCHARSET       "utf8"

#define DEFAULT_READAHEAD_SIZE		(4096 * 1024)

#ifdef __cplusplus
}
#endif
#endif

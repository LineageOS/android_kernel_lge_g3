# Makefile for V4L2 FM driver

fm_drv-objs := fmdrv_main.o fmdrv_v4l2.o fmdrv_rx.o
obj-$(CONFIG_V4L2_FM_DRIVER) := fm_drv.o

KBUILD_CFLAGS += -w
EXTRA_CFLAGS := -I$(PWD)/../include/

#BT_S : [CONBT-2455] LGC_BT_COMMON_IMP_KO_MODULE_LOAD_FAIL_FIX ("Unknown symbol _GLOBAL_OFFSET_TABLE_" Error fix)
CFLAGS_MODULE = -fno-pic
#BT_E : [CONBT-2455] LGC_BT_COMMON_IMP_KO_MODULE_LOAD_FAIL_FIX ("Unknown symbol _GLOBAL_OFFSET_TABLE_" Error fix)

ccflags-y := -DVERSION="\"$(BUILD_MODULE_VERSION)\""
ccflags-y += -DDEF_V4L2_FM_AUDIO_PATH=FM_AUDIO_I2S

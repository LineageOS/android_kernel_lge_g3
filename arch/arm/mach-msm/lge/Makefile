# LGE Board Selection
obj-$(CONFIG_MACH_MSM8974_G3) += 8974ac-g3/

obj-$(CONFIG_MACH_LGE) += devices_lge.o
obj-$(CONFIG_MACH_LGE) += lge_charging_scenario.o

obj-$(CONFIG_WIFI_CONTROL_FUNC) += wifi_bcm_lge.o

obj-$(CONFIG_LGE_BOOTLOADER_LOG) += lge_bootloader_log.o
obj-$(CONFIG_LGE_BOOTLOADER_TIME_CHECKER) += lge_bootloader_time_checker.o
obj-$(CONFIG_LGE_QFPROM_INTERFACE) += lge_qfprom_access.o

CFLAGS_devices_lge.o += -Idrivers/staging/android

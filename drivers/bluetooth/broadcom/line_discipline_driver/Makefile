# Makefile for Line discipline driver

brcm_hci_ldisc-objs := brcm_hci.o brcm_sh_ldisc.o brcm_bluesleep.o
obj-$(CONFIG_LINE_DISCIPLINE_DRIVER) := brcm_hci_ldisc.o

KBUILD_CFLAGS += -w
EXTRA_CFLAGS := -I$(PWD)/../include/

#BT_S : [CONBT-2455] LGC_BT_COMMON_IMP_KO_MODULE_LOAD_FAIL_FIX ("Unknown symbol _GLOBAL_OFFSET_TABLE_" Error fix)
CFLAGS_MODULE = -fno-pic
#BT_E : [CONBT-2455] LGC_BT_COMMON_IMP_KO_MODULE_LOAD_FAIL_FIX ("Unknown symbol _GLOBAL_OFFSET_TABLE_" Error fix)

ccflags-y := -DVERSION="\"$(BUILD_MODULE_VERSION)\""
ccflags-y += -DLPM_BLUESLEEP

#
# Copyright 2014 Freescale Semiconductor, Inc.
#
# SPDX-License-Identifier:      GPL-2.0+
#

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
else
obj-$(CONFIG_T600) += t600.o
obj-$(CONFIG_T600) += eth_t600.o
obj-$(CONFIG_T600) += acc_cpld.o
obj-$(CONFIG_CDEC_CPLD) += cdec_cpld.o 
obj-$(CONFIG_PCI)      += pci.o
endif

obj-y   += ddr.o
obj-y   += law.o
obj-y   += tlb.o

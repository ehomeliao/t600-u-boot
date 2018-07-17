/*
 * Copyright 2007-2014 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <pci.h>
#include <asm/fsl_pci.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <asm/fsl_serdes.h>

void pci_init_board(void)
{
	unsigned int dir = 0x00800000, val;
	ccsr_gpio_t *gpio4 = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR + 0x3000);

	/* Since CDEC PCIE reset is triggered by reset sequence, need to reset/release manually */
	/* Set GPIO4_8 as 0 to reset CDEC */
	dir |= (in_be32(&gpio4->gpdir) & 0xff7fffff);
	val = (in_be32(&gpio4->gpdat) & 0xff7fffff);
	out_be32(&gpio4->gpdat, val);
	out_be32(&gpio4->gpdir, dir);
	udelay(100000);

	/* Set GPIO4_8 as 1 to release CDEC */
	val |= 0x00800000;
	out_be32(&gpio4->gpdat, val);
	udelay(100000);

	fsl_pcie_init_board(0);
}

void pci_of_setup(void *blob, bd_t *bd)
{
	FT_FSL_PCI_SETUP;
}

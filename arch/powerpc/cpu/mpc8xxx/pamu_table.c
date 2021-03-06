/*
 * Copyright 2012 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/fsl_pamu.h>

DECLARE_GLOBAL_DATA_PTR;

void construct_pamu_addr_table(struct pamu_addr_tbl *tbl, int *num_entries)
{
	int i  = 0;

	tbl->start_addr[i] =
			(uint64_t)virt_to_phys((void *)CONFIG_SYS_SDRAM_BASE);
	tbl->size[i] = (phys_size_t)(min(gd->ram_size, CONFIG_MAX_MEM_MAPPED));
	tbl->end_addr[i] = tbl->start_addr[i] +  tbl->size[i] - 1;

	i++;
#ifdef CONFIG_SYS_FLASH_BASE_PHYS
	tbl->start_addr[i] =
		(uint64_t)virt_to_phys((void *)CONFIG_SYS_FLASH_BASE_PHYS);
	tbl->size[i] = 256 * 1024 * 1024; /* 256MB flash */
	tbl->end_addr[i] = tbl->start_addr[i] +  tbl->size[i] - 1;

	i++;
#endif
#ifdef DEBUG
	int j;
	printf("address\t\t\tsize\n");
	for (j = 0; j < i ; j++)
		printf("%llx \t\t\t%llx\n",  tbl->start_addr[j],  tbl->size[j]);
#endif

	*num_entries = i;
}

int sec_config_pamu_table(uint32_t liodn_ns, uint32_t liodn_s)
{
	struct pamu_addr_tbl tbl;
	int num_entries = 0;
	int ret = 0;

	construct_pamu_addr_table(&tbl, &num_entries);

	ret = config_pamu(&tbl, num_entries, liodn_ns);
	if (ret)
		return ret;

	ret = config_pamu(&tbl, num_entries, liodn_s);
	if (ret)
		return ret;

	return ret;
}

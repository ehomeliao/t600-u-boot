/*
 * Copyright 2009-2013 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <i2c.h>
#include <netdev.h>
#include <linux/compiler.h>
#include <asm/mmu.h>
#include <asm/processor.h>
#include <asm/immap_85xx.h>
#include <asm/fsl_law.h>
#include <asm/fsl_serdes.h>
#include <asm/fsl_portals.h>
#include <asm/fsl_liodn.h>
#include <fm_eth.h>
#include <fs.h>
#include <usb.h>
#include <acc_cpld.h>
#include "t600.h"
#ifdef CONFIG_CDEC_CPLD
#include <cdec_cpld.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
	struct cpu_type *cpu = gd->arch.cpu;
	static const char *freq[3] = {"100.00MHZ", "125.00MHz", "156.25MHZ"};

	printf("Board: T600\n");
	puts("SERDES Reference Clocks:\n");
	printf("SD1_CLK1=%s, SD1_CLK2=%s\n", freq[2], freq[0]);
	printf("SD2_CLK1=%s, SD2_CLK2=%s\n", freq[0], freq[0]);

	return 0;
}

#ifdef CONFIG_CDEC_CPLD
static int  bord_fpga_config_reset_skip_jadge(void)
{

	if (is_from_warm_boot() && is_mbcnt_loaded()) {
		return 1;
	} else {
		return 0;
	}

#if 0
	pci_dev_t dev;
	dev = pci_find_device (MBCNT_VENDER_ID, MBCNT_DEVICE_ID, 0);
	if (dev == -1) {
		return 0;
	}
	printf(" MBCNT BAR0=0x%x\n", (uint32_t)pci_map_bar(dev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM));
	return 1;
#endif
}

static int board_abortfpgaconf(int configdelay)
{
	int abort = 0;

	printf("Hit any key to skip FPGA Config: %2d ", configdelay);

	/*
	 * Check if key already pressed
	 * Don't check if configdelay < 0
	 */
	if (configdelay >= 0) {
		if (tstc()) {			/* we got a key press	*/
			(void) getc();		/* consume input	*/
			puts ("\b\b\b 0");
			abort = 1;			/* don't auto boot	*/
		}
	}

	while ((configdelay > 0) && (!abort)) {
		int i;

		--configdelay;
		/* delay 100 * 10ms */
		for (i=0; !abort && i<100; ++i) {
			if (tstc()) {			/* we got a key press	*/
				abort  = 1;			/* don't auto boot	*/
				configdelay = 0;	/* no more delay	*/
				(void) getc();		/* consume input	*/
				break;
			}
			udelay(10000);
		}

		printf("\b\b\b%2d ", configdelay);
	}

	putc('\n');


	return abort;
}

static int bord_fpga_config_user_skip_jadge(void)
{
	unsigned int rtn=1;
	int configdelay=3;
	
	if (configdelay >= 0 && !board_abortfpgaconf(configdelay)) {
		rtn=0;
		
	} else { /* config delay */
		puts("SKIP FPGA Config(USER OPERATION)... \n");
#if 0
		setenv("fpgmode", "0");
		CPLD_NVS_WRITE(fpga_conf_skip_state,0); 
		CPLD_NVS_WRITE(fpga_conf_error,1); 
		CPLD_NVS_WRITE(fpga_conf_side_state,0); 
		init_fpga_conf_fail_cnt();
#endif
	}
	
	return (rtn);
	
}
static int bord_fpga_config_sub_v2(const char *filename, unsigned int fpga_config_index, unsigned int fpga_config_mode, int fstype, unsigned int fpga_n)
{
	unsigned int fpga_config_size=0;
	unsigned int temp=0;
	unsigned long addr = FPGA_CONFIG_RAM_START_ADR;
	char szBuffer[256];
	char* dirname = szBuffer;
#if 0
	loff_t bytes = 0;
	loff_t pos = 0;
	loff_t len_read;
#else
	int bytes = 0;
	int pos = 0;
	int len_read;
#endif
	int ret;
	unsigned int bank = 1;
	unsigned int fpga_usb = 0;
	int len;
	char *mbcntdir = getenv("mbcntdir");
#if 0
	unsigned int act_bank = 0;

	/* Kernel Start Bank infomation                 */
	/* bank  = 0 SATA side0 kernel(Defunt)          */
	/*         1 SATA side1 kernel                  */
	/*         2 In boot flash'kernel(Debug)        */
	/*         3 Network (Debug)                    */
	/*         4 USB (Degug)                        */
	bank = getenv_ulong("bank", 16, 0x0);
	
	switch(bank) {
	case 0:								/* SATA 0面起動 */
		/* Open SATA 0:1 device */
		if (fs_set_blk_dev("sata","0:1", fstype)){
			puts("Error Access SATA 0:1 \n");
			return RET_ERROR;
		}
		break;
	case 1:								/* SATA 1面起動 */
		/* Open SATA 0:2 device */
		if (fs_set_blk_dev("sata","0:2", fstype)){
			puts("Error Access SATA 0:2 \n");
			return RET_ERROR;
		}
		break;
	default:
	/*case 2:*/								/* BootFlash 起動 */
	/*case 3:*/								/* Network 起動   */
	/*case 4:*/								/* USB    起動    */
		/* FPGA Config Partition bank env get */
		/* act_bank = 0 use partition 0 (defult) */
		/*            1 use partition 1 (defult) */
		if ( -1 == getenv_yesno("act_bank")) {
			fpconf_debug("# Nothing act_bank env \n");
			setenv("act_bank", "0");
		}
		
		act_bank = getenv_ulong("act_bank", 16, 0x0);
		
		
		if (1 == act_bank) {
			/* Open SATA 0:2 device */
			if (fs_set_blk_dev("sata","0:2", fstype)){
				puts("Error Access SATA 0:2 \n");
				return RET_ERROR;
			}
		} else {
			/* Open SATA 0:1 device */
			if (fs_set_blk_dev("sata","0:1", fstype)){
				puts("Error Access SATA 0:1 \n");
				return RET_ERROR;
			}
		}
		
		break;
	}
	
	
	if (1 == fpga_n) {
		/* FPGA Config File dirctory env get                */
		strcpy (dirname, getenv("fpgacnfdir"));
	}
	
	if (2 == fpga_n) {
		/* FPGA Config File dirctory env get                */
		strcpy (dirname, getenv("fpgacnfdir2"));
	}
#endif

	bank = getenv_ulong("bank", 16, 0x1);
	fpga_usb = getenv_ulong("fpga_usb", 16, 0);
	if (fpga_usb == 1) {
		usb_stop();
		printf("(Re)start USB...\n");
		if (usb_init() >= 0) {
#ifdef CONFIG_USB_STORAGE
			/* try to recognize storage devices immediately */
			usb_stor_scan(1);
#endif
			/* USB:/T600 FAT */
			if (fs_set_blk_dev("usb","0:1", 1)){
				puts("Error Access usb 0:1 (FAT)\n");
				return RET_ERROR;
			}
			if (mbcntdir != NULL) {
				len = snprintf(dirname, 256, "/%s/%s", mbcntdir, filename);
			} else {
				strcpy(dirname, filename);
			}
		} else {
			puts("Error to start USB\n");
		}
	} else if (bank == 1) {
		/* sda1 with EXT2 */
		if (fs_set_blk_dev("sata","1:5", 2)){
			puts("Error Access SATA 1:5 \n");
			return RET_ERROR;
		}
		if (mbcntdir != NULL) {
			len = snprintf(dirname, 256, "/%s/%s", mbcntdir, filename);
		} else {
			strcpy(dirname, filename);
		}
	} else if (bank == 2) {
		/* sda2 with EXT2 */
		if (fs_set_blk_dev("sata","1:6", 2)){
			puts("Error Access SATA 1:6 \n");
			return RET_ERROR;
		}
		if (mbcntdir != NULL) {
			len = snprintf(dirname, 256, "/%s/%s", mbcntdir, filename);
		} else {
			strcpy(dirname, filename);
		}
	} else if (bank == 3) {
		usb_stop();
		printf("(Re)start USB...\n");
		if (usb_init() >= 0) {
#ifdef CONFIG_USB_STORAGE
			/* try to recognize storage devices immediately */
			usb_stor_scan(1);
#endif
			/* USB:/T600 FAT */
			if (fs_set_blk_dev("usb","0:1", 1)){
				puts("Error Access usb 0:1 (FAT)\n");
				return RET_ERROR;
			}
			strcpy(dirname, "T600/");
			strcat(dirname, filename);
		} else {
			puts("Error to start USB\n");
		}
	} else {
		/* Diag:\T600 with FAT */
		if (fs_set_blk_dev("sata","1", 1)){
			puts("Error Access SATA 1 (FAT) \n");
			return RET_ERROR;
		}
		strcpy(dirname, "T600/");
		strcat(dirname, filename);
	}

	/* FPGA Config File(FP01.bin) Read From SATA 0 to RAM 20000000 */
	len_read = fs_read(dirname, addr, pos, bytes);

	if (len_read <= 0) {
#if 0
		if((0==bank) ||(1==bank)) {
			printf("Error Read File(%s) From SATA 0:%01d \n",dirname,bank+1);
		} else {
			printf("Error Read File(%s) From SATA 0:%01d \n",dirname,act_bank+1);
		}
#else
		printf("Error Read MBCNT File(%s)n",dirname);
#endif
		return RET_RERY;
	}

	/* Chack size */
	temp = (len_read%2);
	fpga_config_size = (temp + len_read);

	/* WATCHDOG RESET */	
//	CPLD_WATCHDOG_RESET();	

	ret=fpga_config_main(fpga_config_index, fpga_config_mode, fpga_config_size);

//	fpconf_debug("# FPGA CPLD Configuration Infomation is %x\n",ret);

	/* memset((void*)FPGA_CONFIG_RAM_START_ADR, 0, fpga_config_size); */

	return ret;
}

static int bord_fpga_config_sata(unsigned int fpga_n,unsigned int config_side)
{
	/* FPGA Config From SATA */
//	unsigned int fpga_config_cwkad=0;
	
	unsigned int fpga_config_mode=0;
	unsigned int rtn=0;
	int fstype;
	
	char fnameBuffer[10];
	char* fname = fnameBuffer;
	
	
	
//	fstype = 2;	/* 2 is FS_TYPE_EXT */
	fstype = 1; /* 1 is FS_TYPE_FAT */
	
	/* FPGA Config CPLD WA env get                    */
	/* fpcfcpld_wkad1 = 0 Not use work around(defult) */
	/*                  1 use work aroud              */
//	fpga_config_cwkad = getenv_ulong("fpcfcpld_wkad1", 16, 0x0);
	strcpy (fname, "fp01.bin");

#if 0	
	if (1 == fpga_n) {
		/* FPGA Config Type Fucntion env get                */
		/* fpcnfmd =  0 Xixlinx/Altera uncompress mode FPGA */
		/*            1 Altera compress mode FPGA           */
		fpga_config_mode = getenv_ulong("fpcnfmd", 16, 0x0);
		
		if(0 == config_side) {
			strcpy (fname, "fp01.bin");
		} else {
			strcpy (fname, "fp11.bin");
		}
		
		fpconf_debug("# The file name is %s\n",fname);
	}

	if (2 == fpga_n) {
		/* FPGA Config Type Fucntion env get                */
		/* fpcnfmd2 = 0 Xixlinx/Altera uncompress mode FPGA */
		/*            1 Altera compress mode FPGA           */
		fpga_config_mode = getenv_ulong("fpcnfmd2", 16, 0x0);
		
		if(0 == config_side) {
			strcpy (fname, "fp02.bin");
		} else {
			strcpy (fname, "fp12.bin");
		}
		
		fpconf_debug("# The file name is %s\n",fname);
	}
#endif
	
	rtn = bord_fpga_config_sub_v2(fname, fpga_n, fpga_config_mode, fstype, fpga_n);

#if 0	
	if (fpga_config_cwkad) {
		rtn = bord_fpga_config_sub_v2(fname, fpga_n, fpga_config_mode, fstype, fpga_n); /* workaround */
	}
#endif
	
	switch(rtn) {
	case RET_ERROR:
		printf("Error FPGA#%01d Configuration...Side%01d \n",fpga_n,config_side);
		SEVERITY_LED = SEVERITY_RED_ON;
		SYS_LED = SYS_RED_ON;
#if 0
		CPLD_WRITE(severity_led,0x00000001);	
		setenv("fpgmode", "0");
		CPLD_NVS_WRITE(fpga_conf_skip_state,0); 
		CPLD_NVS_WRITE(fpga_conf_error,1); 
		CPLD_NVS_WRITE(fpga_conf_side_state,0); 
		init_fpga_conf_fail_cnt();
#endif
		break;
	case RET_SUCCESS:
		printf("FPGA#%01d Configured...Side%01d \n",fpga_n,config_side);
		/* Record to CPLD as MBCNT is loaded */
		mbcnt_loaded(1);
#if 0
		if(0 == config_side) {
			setenv("fpgmode", "1");
			CPLD_NVS_WRITE(fpga_conf_side_state,1); 
		} else {
			setenv("fpgmode", "2");
			CPLD_NVS_WRITE(fpga_conf_side_state,2); 
		}
		CPLD_NVS_WRITE(fpga_conf_skip_state,0); 
		CPLD_NVS_WRITE(fpga_conf_error,0);
#endif
		break;
	case RET_RERY:

//		if (1 == config_side) {
			printf("Error FPGA#%01d Configuration...Side%01d \n",fpga_n,config_side);
			SEVERITY_LED = SEVERITY_RED_ON;
			SYS_LED = SYS_RED_ON;
#if 0
			CPLD_WRITE(severity_led,0x00000001);	
			setenv("fpgmode", "0");
			CPLD_NVS_WRITE(fpga_conf_skip_state,0); 
			CPLD_NVS_WRITE(fpga_conf_error,1); 
			CPLD_NVS_WRITE(fpga_conf_side_state,0); 
			init_fpga_conf_fail_cnt();
#endif		
			rtn = RET_ERROR;
//		}
		break;
	default:
		break;
	}
	
	
	return (rtn);
	
	
}

void bord_fpga_config(void)
{
	unsigned int config_side = 0;

	if (getenv_ulong("fpgaskip", 16, 0x0) == 1) {
		printf("SKIP FPGA Config(SKIP ENV FLAG)...\n");
		return;
	}
	
	/* Check FPGA Config SKIP ,When User Operationt */
	if (1 == bord_fpga_config_user_skip_jadge()) {
		return ;
	}
	
	/* Check FPGA Config SKIP ,When CPU Reset */
	if(1 == bord_fpga_config_reset_skip_jadge()) {
		printf("SKIP FPGA Config since it is loaded\n");
		return ;
	}

	if (RET_RERY == bord_fpga_config_sata(1,config_side)) {
		puts("Retry FPGA Configuration... \n");
		bord_fpga_config_sata(1,++config_side);
	}

	puts("\n");
	puts("\n");
}

void board_fpga_preinit(void)
{
	cdec_cpld_preinit();
}

void board_fpga_init(void)
{
	cdec_cpld_init();
} 
#endif

int board_early_init_r(void)
{
	const unsigned int flashbase = CONFIG_SYS_FLASH_BASE;
	const u8 flash_esel = find_tlb_idx((void *)flashbase, 1);
	/*
	 * Remap Boot flash + PROMJET region to caching-inhibited
	 * so that flash can be erased properly.
	 */

	/* Flush d-cache and invalidate i-cache of any FLASH data */
	flush_dcache();
	invalidate_icache();

	/* invalidate existing TLB entry for flash + promjet */
	disable_tlb(flash_esel);

	set_tlb(1, flashbase, CONFIG_SYS_FLASH_BASE_PHYS,
		MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
		0, flash_esel, BOOKE_PAGESZ_256M, 1);

	set_liodns();
#ifdef CONFIG_SYS_DPAA_QBMAN
	setup_portals();
#endif

	return 0;
}

unsigned long get_board_sys_clk(void)
{
	return CONFIG_SYS_CLK_FREQ;
}

unsigned long get_board_ddr_clk(void)
{
	return CONFIG_DDR_CLK_FREQ;
}

int misc_init_r(void)
{
	return 0;
}

void ft_board_setup(void *blob, bd_t *bd)
{
	phys_addr_t base;
	phys_size_t size;

	ft_cpu_setup(blob, bd);

	base = getenv_bootm_low();
	size = getenv_bootm_size();

	fdt_fixup_memory(blob, (u64)base, (u64)size);

#ifdef CONFIG_PCI
	pci_of_setup(blob, bd);
#endif

	fdt_fixup_liodn(blob);
	fdt_fixup_dr_usb(blob, bd);

#ifdef CONFIG_SYS_DPAA_FMAN
	fdt_fixup_fman_ethernet(blob);
	fdt_fixup_board_enet(blob);
#endif
}

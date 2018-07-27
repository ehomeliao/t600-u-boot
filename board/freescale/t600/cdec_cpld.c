/************************************************************************/ 
/** 
 * @file cdec_cpld.c
 * @brief This is a program group that specializes 
 * @brief in P2041 FJ individual board. 
 * @author FJT)Konno
 * $Date:: 2016-10-28 18:54:31 +0900#$$
 * 
 * SPDX-License-Identifier: GPL-2.0+
 * This program is free software: you can redistribute it and/or modify
 * it un under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * 
 * COPYRIGHT(C) FUJITSU LIMITED 2015
 */
/************************************************************************/ 

#include <common.h>
#include <asm/io.h>
#include <version.h>
#include "cdec_cpld.h"


//#define CDEC_CPLD_DEBUG

#ifdef CDEC_CPLD_DEBUG
#define    debug_printf(x) printf x
#else
#define    debug_printf(x)
#endif


/***********************************************************************/ 
/** 
 * 
 * @brief  fpga_config_main
 * @brief 
 * @brief 
 * 
 * @param unsigned int  
 * @param unsigned int  
 * @param unsigned int  
 * 
 * @brief RETUN Type int
 * @retval 
 * @retval 
 * 
 * @attention --
 * 
 * --------------------------------------------------------------------- 
 * 
 * SPDX-License-Identifier: GPL-2.0+
 * This program is free software: you can redistribute it and/or modify
 * it un under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * 
 * COPYRIGHT(C) FUJITSU LIMITED 2015
 */
/***********************************************************************/ 
int fpga_config_main(unsigned int fpga, unsigned int mode,unsigned int size)
{
	unsigned int read_reg=0;
	unsigned short *conf_start_adr=0;
	unsigned int conf_data=0;
	unsigned int count = size/2;
	unsigned int i;
	/* debug_cpld = 1; */
	
	/* FPGA#1 is configed */
	if(1 == fpga) {
		
		/* Config Mode setting */
		FPGA_CNF_SET(fpga) = mode;
		debug_printf(("FPGA_CNF_SET(fpga) = mode;\n"));
		debug_printf((" FPGA_CNF_SET(%p) = %08lx \n",&FPGA_CNF_SET(fpga),FPGA_CNF_SET(fpga))); //debug
		debug_printf((" FPGA_CNF_MON(%p) = %08lx \n",&FPGA_CNF_MON(fpga),FPGA_CNF_MON(fpga))); //debug

		
		/* Config startUp setting */
		FPGA_CNF_START(fpga) = 0x00000001;
		debug_printf(("FPGA_CNF_START(fpga) = 0x00000001;\n"));
		debug_printf((" FPGA_CNF_START(%p) = %08lx \n",&FPGA_CNF_START(fpga),FPGA_CNF_START(fpga))); //debug
		debug_printf((" FPGA_CNF_MON(%p) = %08lx \n",&FPGA_CNF_MON(fpga),FPGA_CNF_MON(fpga))); //debug

		udelay(WAIT_30MS);	/* wait 30msec */
		
		/* Configuration Access Chack */
		read_reg = FPGA_CNF_ST(fpga);
		debug_printf(("read_reg = FPGA_CNF_ST(fpga);\n"));
		debug_printf((" FPGA_CNF_ST(%p) = %08x \n",&FPGA_CNF_ST(fpga),read_reg)); //debug
		debug_printf((" FPGA_CNF_MON(%p) = %08lx \n",&FPGA_CNF_MON(fpga),FPGA_CNF_MON(fpga))); //debug
		
		/* Chack CNF_START_ERR */
		if(read_reg & 0x1) {
			/* Configuration Start Error. */
			/* FULT */
			SEVERITY_LED = SEVERITY_RED_ON;
			SYS_LED = SYS_RED_ON;
#if 0
			CPLD_WRITE(severity_led,0x00000001);
			cpld_debug("W %08x <== %08x\n",(unsigned int)((offsetof(cpld_data_t, severity_led) + CPLD_BASE)),0x00000001);
#endif
			
			return RET_FAULT;
			
		} else {
			/* No error */
			conf_start_adr = (unsigned short *)FPGA_CONFIG_RAM_START_ADR;
			
			conf_data = (unsigned int)*conf_start_adr;
			
			for(i=0;i <count; i++){
				/* FPGA Config */
				FPGA_CNF_DATA(fpga) = conf_data;
#if 0
				if(i<100){
					cpld_debug("W %08x <== %08x From 0x%08x \n",(unsigned int)(GET_CPLD_AD(fpga1_cnf_data)),(unsigned int)conf_data,(unsigned int)conf_start_adr);
				}
#endif
				conf_start_adr++;
				conf_data = (unsigned int)*conf_start_adr;
			}
//			cpld_debug("LAST ADDRES 0x%08x LAST DATA 0x%08x \n",(unsigned int)conf_start_adr,(unsigned int)conf_data);
			
			/* Configuration end Process */
			FPGA_CNF_END(fpga) = 0x00000001;
			debug_printf(("FPGA_CNF_END(fpga) = 0x00000001;\n"));
			debug_printf((" FPGA_CNF_END(%p) = %08lx \n",&FPGA_CNF_END(fpga),FPGA_CNF_END(fpga))); //debug
			debug_printf((" FPGA_CNF_MON(%p) = %08lx \n",&FPGA_CNF_MON(fpga),FPGA_CNF_MON(fpga))); //debug
			
			udelay(WAIT_30MS);	/* wait 30msec */
			
			/* FPGA Config Status Read */
			read_reg = FPGA_CNF_ST(fpga);
			debug_printf(("read_reg = FPGA_CNF_ST(fpga);\n"));
			debug_printf((" FPGA_CNF_ST(%p) = %08x \n",&FPGA_CNF_ST(fpga),read_reg)); //debug
			debug_printf((" FPGA_CNF_MON(%p) = %08lx \n",&FPGA_CNF_MON(fpga),FPGA_CNF_MON(fpga))); //debug
			
			if(read_reg & 0x4) {
				/* Configuration Initialization Error */
				
				/* FULT */
				SEVERITY_LED = SEVERITY_RED_ON;
				SYS_LED = SYS_RED_ON;
#if 0
				CPLD_WRITE(severity_led,0x00000001);
				cpld_debug("W %08x <== %08x\n",(unsigned int)((offsetof(cpld_data_t, severity_led) + CPLD_BASE)),0x00000001);
#endif
				
				return RET_FAULT;
			}
			
			if(read_reg & 0xA) {
				/* Configuration Done Error. */
				/* Configuration Data Error.. */
				
				return RET_RERY;
			}
		}
		
	}
	
	return RET_SUCCESS;
}

int cdec_cpld_preinit(void)
{
	/* Turn SYS LED */
	PIU_ALM_FORCE = 0x300;
	PIU_ALM_MASK = 0x300;
	SYS_LED = SYS_YEL_ON;

	/* Turn off ALM LED */
	SEVERITY_LED = SEVERITY_LED_OFF;

	return 0;

}

int cdec_cpld_init(void)
{
	unsigned int read_reg=0;

	/* PIU Power Enable */
	PIU_PWR_CTRL = 0x3;

	/* Wait 1000ms */
	msleep(1000);

	/* PIU Reset Release */
	PIU_RESET = 0x0;

	/* MBCNT Reset Release */
	MBCNT_RESET = 0x0;

	/* Clock Down Alarm Mask */
	CLK_DWN_EQP_MSK = 0x20;

	/* WDT Alarm Mask */
	WDT_ALM_STATUS = 0x0;

	/* TX_DIS Alarm Mask */
	TX_DIS = 0x100;

	/* Clear Clock Down Alarm */
	read_reg = CLK_DWN_EQP;

	/* Clear WDT Alarm */
	read_reg = WDT_ALM_MSK;

	/* Wait 1000ms, PIU1 of some device need delay for PCIE scaning later */
	/* TODO: Need more test if we oould reduce the delay time */
	msleep(1000);
	return RET_SUCCESS;
}

/************************************************************************/ 
/** 
 * @file cmd_fpga_conf.c
 * @brief This is a program group that specializes
 * @brief in P2041 FJ individual board. 
 * @author FJT)Konno
 * $Date:: 2016-02-8 13:45:03 +0900#$$
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
#include <command.h>
#include <fs.h>
//#include <version.h>
#include <linux/compiler.h>
#ifdef CONFIG_SYS_COREBOOT
#include <asm/arch/sysinfo.h>
#endif
#include <cdec_cpld.h>
#include <acc_cpld.h>
//#define MSAI_DEBUG

#ifdef MSAI_DEBUG
#define    debug_printf(x) printf x
#else
#define    debug_printf(x)
#endif


//Max length of FPGA configuration file name
#define FILE_ADDRESS_MAX 64


/***********************************************************************/ 
/** 
 * 
 * @brief  fpga_conf_Main
 * @brief FPGA configuration main program
 * 
 * @param int   location 0:Memory 1:SATA-Drive 
 * @param char* File address for CDEC
 * @param int   FPGA index number 1:FPGA#1 2:FPGA#2
 * @param int   Compress   0:Uncompressed 1:Compressed
 * @param char* File address for MDEC
 * 
 * @brief RETUN Type int
 * @retval RET_SUCCESS  Command finished successfully.
 * @retval RET_WARNING  Command finished successfully but with a warning.
 * @retval RET_ERROR    Command failed.
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
int fpga_conf_Main(int v_location, char * v_file_address, int v_fpga_index, unsigned int v_conf_mode)
{
    unsigned int fpga_config_size=0;
    unsigned int fpga_config_flash_start_adr=0;
    unsigned int temp=0;
    int rtn=RET_SUCCESS;
    int fstype; 
    unsigned long addr;
    loff_t bytes;
    loff_t pos; 
    loff_t len_read;

    printf("Start FPGA Configuration...\n");

    if(v_location){             // v_locastion = 1 means Memory location
        fpga_config_flash_start_adr = simple_strtoul(v_file_address,NULL,0);

        if(v_fpga_index == 1){
            fpga_config_size = getenv_ulong("fpcnfsz", 16, 0x00000000);
        }else{
            fpga_config_size = getenv_ulong("fpcnfsz2", 16, 0x00000000);
        } 
        /* Chack size */
        temp = (fpga_config_size%2);
        fpga_config_size = (temp + fpga_config_size);

        debug_printf(("fpga_config_size=%d(0x%x)\n",fpga_config_size,fpga_config_size)); //debug
        debug_printf(("FPGA_CONFIG_RAM_START_ADR=%x\n",FPGA_CONFIG_RAM_START_ADR)); //debug

        /* Copy FPGA-Config Data From Memory to FPGA Configuration work area */
        memcpy((char *)FPGA_CONFIG_RAM_START_ADR, (char *)fpga_config_flash_start_adr, fpga_config_size);

    }else{
        fstype = 2;	/* 2 is FS_TYPE_EXT */
        addr = FPGA_CONFIG_RAM_START_ADR;
        pos = 0;
        bytes = 0;

        printf("before fs_set_blk_dev\n"); // debug

        if (fs_set_blk_dev("sata","1:1", fstype)){
            printf("Error Access SATA 0 \n");
            return RET_ERROR;
        }

        /* Load FPGA Configuration File from SATA0 to RAM */
        len_read = fs_read(v_file_address, addr, pos, bytes);

        debug_printf(("fs_read len=%d\n",len_read)); // debug

        if (len_read <= 0) {
            printf("Error Read File From SATA 0 \n");
            return RET_ERROR;
        }

        /* Chack size */ 
        temp = (len_read%2);
        fpga_config_size = (temp + len_read);
        
        debug_printf(("fpga_config_size=%d(0x%x)\n",fpga_config_size,fpga_config_size)); //debug
    }

	 rtn = fpga_config_main(v_fpga_index, v_conf_mode, fpga_config_size);
					
    if(0 != rtn){
        printf("Error FPGA Configuration... \n");
    } else {
        mbcnt_loaded(1);
        printf("End FPGA Configuration... \n");
    }

    /* clear whole structure to make sure all reserved fields are zero */
    memset((void*)FPGA_CONFIG_RAM_START_ADR, 0, fpga_config_size);


    return rtn;
}

/***********************************************************************/ 
/** 
 * 
 * @brief  do_cmd_fpga_conf
 * @brief FPGA configuration comman
 * 
 * @param cmd_tbl_t*  CommandTbl pointor
 * @param int  repeat flag 0:off 1:on
 * @param int  a number of comand parametor
 * @param char*  a valu of comand parametor
 * 
 * @brief RETUN Type int
 * @retval 0 Normal end
 * @retval -1 Display cmd_usage
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
static int do_cmd_fpga_conf(cmd_tbl_t *cmdtp, int cmdflag, int argc, char * const argv[])
{
    int ret = RET_SUCCESS;

    int v_location;
    char v_file_address[FILE_ADDRESS_MAX+1];
    int v_fpga_index;
    unsigned int v_conf_mode;

    char s_location[16];
    //char s_file_address[33];
    char s_fpga_index[16];
    char s_conf_mode[16];

    if(!strcmp(argv[1],"-f")){
        v_location = 0;                 // SATA-Drive 
        strcpy(s_location,"SATA-Drive");
    }else if(!strcmp(argv[1],"-a")){
        v_location = 1;                 // Memory 
        strcpy(s_location,"Memory");
    }else{
        goto usage;
    }
    strcpy(v_file_address, argv[2]);

    if(!strcmp(argv[3],"1")){           // FPGA#1 
        v_fpga_index = 1;
        strcpy(s_fpga_index,"#1");
    }else if(!strcmp(argv[3],"2")){     // FPGA#2 
        v_fpga_index = 2;
        strcpy(s_fpga_index,"#2");
    }else{
        goto usage;
    }

    if(!strcmp(argv[4],"u")){           // Uncompressed 
        v_conf_mode = 0;
        strcpy(s_conf_mode,"Uncompressed");
    }else if(!strcmp(argv[4],"c")){     // Compressed 
        v_conf_mode = 1;
        strcpy(s_conf_mode,"Compressed");
    }else{
        goto usage;
    }


    // Display acknowledged setting 
    printf("Configuration Data Location : %s\n",s_location);
    if(!v_location){
        printf("File Name : %s\n",v_file_address);
    }else{
        printf("Start-Address :%s\n",v_file_address);
    }
    printf("FPGA Index  : %s\n",s_fpga_index);
    printf("Data Format : %s\n",s_conf_mode);

    // Call main function
    ret = fpga_conf_Main(v_location, v_file_address, v_fpga_index, v_conf_mode);

    return ret;

usage:
    return cmd_usage(cmdtp);
	
}

U_BOOT_CMD(
    fpga_conf,	7,		0,	do_cmd_fpga_conf,
    "FPGA configuration command",
    "-f <file_name> <fpga_index> <conf_mode>\n"
    "fpga_conf -a <mem_addr> <fpga_index> <conf_mode>\n"
    "  file_name  --  Configuration data file name in SATA drive\n"
    "  mem_addr   --  Start adress of configuration data in Memory\n"
    "  fpga_index --  1-2: Target FPGA index\n"
    "  conf_mode  --  u:Uncompressed, c:Compressed\n"
); 

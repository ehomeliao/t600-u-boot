/************************************************************************/ 
/** 
 * @file cmd_inventory.c
 * @brief This is a program group that specializes
 * @brief in P2041 FJ individual board. 
 * @author FJT)Konno
 * $Date:: 2016-11-1 18:27:26 +0900#$$
 * 
 * SPDX-License-Identifier: GPL-2.0+
 * This program is free software: you can redistribute it and/or modify
 * it un under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * 
 * COPYRIGHT(C) FUJITSU LIMITED 2015
 */
/************************************************************************/ 
/*
 * P.I. Read/Write Utilities
 */

#include <common.h>
#define fflush(x)
#include <command.h>
#include <errno.h>
#include <i2c.h>
#include <acc_cpld.h>
#include "cmd_inventory.h"



#ifdef DEBUG_CMD_INVENTORY
unsigned int inv_dbg;
#endif

/*----------------------------------------------------------------------*/
/* for inventory command												*/
/*----------------------------------------------------------------------*/
//----------------------------------------------------------------------------
//  VARIABLE DEFINITION
//----------------------------------------------------------------------------
typedef volatile unsigned char  *  REG_8;  /*< 8bitレジスタアクセス型 */

#define IFT_PRINTF(xx...)	printf(xx);

#define MAX_BUF_SIZE 		512
#define ITEM_NUM_MAX 		46
#define ITEM_NUM_MAX_TEMP 	30
static unsigned char buf[MAX_BUF_SIZE] = {0};

unsigned short	card_kind = 0;

/***********************************************************************/ 
/** 
 * 
 * @brief  calcCheckSum
 * @brief Calucuration CheckSum
 * 
 * @param unsigned char*  Data Buffer pointor
 * @param int  Number of data
 * 
 * @brief RETUN Type unsigned short
 * @retval Valu of CheckSum
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
static unsigned short calcCheckSum(unsigned char *buf, int num)
{
	int i;
	unsigned short checksum = 0;
	
	DBG_PRT_INV(DBG_LEVEL_2,"%s:<0>num=0x%08x\n", __FUNCTION__, num);
	
	for(i=0;i<num;i++){
		checksum += buf[i];
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<0>checksum=0x%08x buf[%03x]=0x%08x \n", __FUNCTION__, checksum,i,buf[i]);
	}
	
	return((~checksum) + 1);
}

#define STORE_BUFFER(t, s) {															\
	t->code			=		s->code;													\
	t->version_flag =		s->version_flag;											\
	memcpy(t->issue,		s->issue,			pi_param_size[3]);						\
	memcpy(t->acronym,		s->acronym,			pi_param_size[4]);						\
	memcpy(t->fc_num,		s->fc_num,			pi_param_size[5]);						\
	memcpy(t->clei,			s->clei, 			pi_param_size[6]);						\
	memcpy(t->date,			s->date, 			pi_param_size[7]);						\
	memcpy(t->td_label_loc,	s->td_label_loc,	pi_param_size[8]);						\
	memcpy(t->serial_num,	s->serial_num,		pi_param_size[9]);						\
	t->rsv1 		=		s->rsv1;													\
	t->rsv2 		=		s->rsv2;													\
	t->rsv3 		=		s->rsv3;													\
	memcpy(t->mac_max,		s->mac_max,			pi_param_size[10]);						\
	memcpy(t->mac_min,		s->mac_min,			pi_param_size[11]);						\
	t->fwdl_type	=		s->fwdl_type;												\
	memcpy(t->rsv4,			s->rsv4,			31);									\
	t->rsv5 		=		s->rsv5;													\
	memcpy(t->rsv6,			s->rsv6,			3);										\
	memcpy(t->rsv7,			s->rsv7,			3);										\
	t->rsv8 		=		s->rsv8;													\
	memcpy(t->pcb_revision,	s->pcb_revision,	pi_param_size[13]);						\
	memcpy(t->temp_settings,s->temp_settings,	sizeof(pi_temp_t)*TEMP_NUM);			\
	memcpy(t->min_wave_length,	s->min_wave_length,	pi_param_size[15]);					\
	memcpy(t->max_wave_length,	s->max_wave_length,	pi_param_size[16]);					\
	memcpy(t->frequency_spacing,s->frequency_spacing,	pi_param_size[17]);				\
	t->rsv9 		=		s->rsv9;													\
	t->pressure_sensor	=	s->pressure_sensor;											\
	t->min_fan_step		=	s->min_fan_step;											\
	t->max_fan_step		=	s->max_fan_step;											\
	t->min_tmp_threshold = 	s->min_tmp_threshold;										\
	t->amb_tmp_pitch 	= 	s->amb_tmp_pitch;											\
	t->fan_cnt_intervall = 	s->fan_cnt_intervall;										\
	memcpy(t->min_pre_threshold,s->min_pre_threshold,pi_param_size[24]);				\
	memcpy(t->int_fan_step,	s->int_fan_step,	pi_param_size[25]);						\
	t->amb_tmp_hysteresis = s->amb_tmp_hysteresis;										\
	t->amb_prs_hysteresis = s->amb_prs_hysteresis;										\
	t->brk_tmp_overprotect = s->brk_tmp_overprotect;									\
	t->slf_tmp_alrtprotect = s->slf_tmp_alrtprotect;									\
	t->slf_tmp_almprotect = s->slf_tmp_almprotect;										\
	t->col_fail_protect = s->col_fail_protect;											\
	t->col_fail_clerprotect = s->col_fail_clerprotect;									\
	t->col_fail_masktime = s->col_fail_masktime;										\
	memcpy(t->rsv10,		s->rsv10,			13);									\
	t->warm_up		= s->warm_up;														\
	memcpy(t->rsv11,		s->rsv11,			2);										\
	t->pd1_ini_val	= s->pd1_ini_val;													\
	t->pd2_ini_val	= s->pd2_ini_val;													\
	t->max_abs_th	=	s->max_abs_th;													\
	t->voa_adjustment_value		=	s->voa_adjustment_value;							\
	t->voa_max_setting_value	=	s->voa_max_setting_value;							\
	t->voa_min_setting_value	=	s->voa_min_setting_value;							\
	t->loop_target_level		=	s->loop_target_level;								\
	t->loop_target_th_plus		=	s->loop_target_th_plus;								\
	t->loop_target_th_minus		=	s->loop_target_th_minus;							\
	memcpy(t->max_pow_drawn,	s->max_pow_drawn,	pi_param_size[44]);					\
	memcpy(t->low_power_level,	s->low_power_level,	pi_param_size[45]);					\
	memcpy(t->rsv12,		s->rsv12,			18);									\
	t->checksum	=	calcCheckSum((unsigned char*)&t->code, 0xFE);						\
}


/***********************************************************************/ 
/** 
 * 
 * @brief  inbyte_nowait
 * @brief charactor get
 * 
 * @param void  
 * 
 * @brief RETUN Type char
 * @retval getc()
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
char inbyte_nowait(void)
{
	return getc();
}

/***********************************************************************/ 
/** 
 * 
 * @brief  MainCmdCom_yesnoinput
 * @brief yes/no input process
 * @brief 
 * 
 * @param void  
 * 
 * @brief RETUN Type int
 * @retval 0  OK
 * @retval -1 NG
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
int MainCmdCom_yesnoinput(void)
{
	char	inputKey;
	while(1)
		{
			inputKey = inbyte_nowait();
			if((inputKey == 'y')||(inputKey == 'Y'))
				{
					IFT_PRINTF("%c\n",inputKey);
					return 0;
				}
			else if((inputKey == 'n')||(inputKey == 'N'))
				{
					IFT_PRINTF("%c\n",inputKey);
					return -1;
				}
			else if(inputKey ==  0x1b)
				{
					IFT_PRINTF("%c\n",inputKey);
					return -1;
				}
			else
				{
					continue;
				}
		}
}

/***********************************************************************/ 
/** 
 * 
 * @brief  MainCmdCom_datainput_inventory
 * @brief Data input process
 * 
 * @param char*  input data
 * @param int    data size
 * 
 * @brief RETUN Type void
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
void MainCmdCom_datainput_inventory(char *inputdata, int wrtdt_sz)
{
	unsigned char	size;
	char	inputKey;
	char	inputKeyBuf[LINEMSG_SZ];
	
	memset( inputKeyBuf, 0, sizeof(inputKeyBuf) );


	DBG_PRT_INV(DBG_LEVEL_2,"%s:<0> START\n", __FUNCTION__);
	
	for( size=0; ; )
	{
		/* キー入力文字取得(codeにキーが入る) */
		inputKey = inbyte_nowait();

		if( inputKey & 0x80 ) 
		{	/* Break or Extend key code */
			continue;
		}

		if( ((inputKey >= '0') && (inputKey <= '9')) ||
			((inputKey >= 'A') && (inputKey <= 'Z')) ||
			((inputKey >= 'a') && (inputKey <= 'z')) ||
			((inputKey == '#') || (inputKey == '.') || (inputKey == '-') || (inputKey == ' ')) )
		{
			if( size > ((wrtdt_sz*2)-1) ) 
			{/* 入力データサイズオーバー */
				continue;
			}

			inputKeyBuf[size] = inputKey;
			//if(inputKey)
			size++;

			// キーボードバッファ出力
			IFT_PRINTF("%c",inputKey );
			fflush(stdout);
		
		}
		else if( inputKey == 0x0d )	// return key(CR)
		{
			if( size == 0 ) {// 入力なしのエンターは受付けない
				continue;
			}

			IFT_PRINTF("\n");

			inputKeyBuf[size] = '\0';
			strcpy( inputdata, inputKeyBuf );
			DBG_PRT_INV(DBG_LEVEL_4,"inputKeyBuf : %s   inputdata %s\n" ,inputKeyBuf,inputdata );
			
			break ;
		}
		else if( inputKey == 0x08 )	// back space
		{
			if( size <= 0 ) {
				continue;
			}

			/* カーソル位置を空白にする */
			inputKeyBuf[size-1] = ' ';
		
			backSpace();	// カーソルを1つ戻す
							// 空白にする
							// 空白後、カーソルは1つ進むので、戻す

		    fflush(stdout);
		
			size--;					//実際の文字列の長さ
		}

	}

	DBG_PRT_INV(DBG_LEVEL_2,"%s:<0> END\n", __FUNCTION__);
	return;
}

#if 0
static void hexdump(unsigned char *buf, int len)
{
	int i;

	for (i = 0; i < len; i++) {
		if ((i % 16) == 0)
			printf("%s%08x: ", i ? "\n" : "",
							(unsigned int)&buf[i]);
		printf("%02x ", buf[i]);
	}
	printf("\n");
}
#endif

int DevE2pDataRead(unsigned int addr, uint8_t *buffer, int len)
{
	int ret = -1;
	uint8_t channel;

	if (i2c_set_bus_num(1))
		goto FAILED;

	/* open I2C channel */
	channel = 0x10;
	if (i2c_write(0x72, 0, 1, &channel, 1) != 0)
		goto FAILED;

	if (i2c_read(0x52, addr, 2, buffer, len) == 0)
		ret = 0;

	/* close I2C channel */
	channel = 0x0;
	i2c_write(0x72, 0, 1, &channel, 1);

FAILED:
	return ret;
}

int DevE2pDataWrite(unsigned int addr, uint8_t *buffer, int len)
{
	int ret = -1, count = len;
	uint8_t channel, *ptr = buffer;
	unsigned int pos = addr;

	//hexdump(buffer, len);
	/* Disable primary EEPROM protection */
	if (acc_eeprom_wp(1, 0))
		goto FAILED;

	if (i2c_set_bus_num(1))
		goto FAILED;

	/* open I2C channel */
	channel = 0x10;
	if (i2c_write(0x72, 0, 1, &channel, 1) != 0)
		goto FAILED;


	while (count-- > 0) {
		if (i2c_write(0x52, pos++, 2, ptr++, 1) != 0)
			goto FAILED;

#if !defined(CONFIG_SYS_I2C_FRAM)
		udelay(11000);
#endif
	}

	if (count < 0)
		ret = 0;

	/* close I2C channel */
	channel = 0x0;
	i2c_write(0x72, 0, 1, &channel, 1);

FAILED:
	return ret;
}


/***********************************************************************/ 
/** 
 * 
 * @brief  CheckPIParam
 * @brief The check of the format of Data for which P.I is specified is done,
 * @brief and data is stored in Buffer. 
 * 
 * @param unsigned int  Check mode
 *                                 bit1  simple_strtol()でHex変換しNG無しをチェック
 *                                 bit2  
 *                                 bit3  Dataのサイズチェック(HEX Code用)
 *                                 bit4  Dataのサイズチェック(ASCII Code用)
 *                                 bit5  
 *                                 bit6  先頭スペース付きASCII Codeのチェックと格納
 *                                 bit7  "-"付き ASCII Codeフォーマットのチェックと格納
 *                                 bit8  
 *                                 bit9  "."付き ASCII Codeフォーマットのチェックと格納
 *                                 bit10 subの値に従いチェック 
 *                                 bit11 
 *                                 bit12 "#####.##"のフォーマットチェック
 *                                 bit13 "###.##"のフォーマットチェック
 *                                 bit14 
 * @param unsigned int  Check mode sub
 *                                 sub=1 2D label Location Codeチェック
 *                                       小山製造の場合は[0][1]、Richardson製造の場合は[0][3]
 *                                 sub=2 Pressure Sensor Existenceチェック
 *                                        00：搭載なし 01：搭載あり
 * @param char*  Input Key Buffer pointor
 * @param int  Input Key number
 * @param unsigned int*  conver value of input key
 * @param char*  not setting if NULL
 *               Setting Buffer pointor(ex common->issue)
 * 
 * @brief RETUN Type int
 * @retval 0 Normal end
 * @retval 1 Command Failed
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
int CheckPIParam(unsigned int mode, unsigned int sub, char* in_buf, int number, unsigned int* r_v, char* r_s)
{
	
	unsigned char	bit_shift=0;
	char   *errptr;
	int		len;
	int		tmp;
	unsigned int	tmp2;
	char	*p;
	char	input[20];

	DBG_PRT_INV(DBG_LEVEL_2,"%s:<0>mode=0x%08x sub=0x%08x number=%d\n", __FUNCTION__, mode, sub, number);

	if (input == NULL) {
		printf("%s: internal error.\n", __FUNCTION__);
		return -1;
	}
	memcpy(input, in_buf, sizeof(input)-1);

	switch (number) {
	case 1:
	case 2:
	case 10:
	case 11:
	case 12:
	case 14:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
	case 38:
	case 39:
	case 40:
	case 41:
	case 42:
	case 43:
		if (r_v == NULL) {
			printf("%s: internal error.(r_v) %2d mode=0x%08x\n", __FUNCTION__, number, mode);
			return -1;
		}
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 13:
	case 15:
	case 16:
	case 17:
	case 44:
	case 45:
		if (r_s == NULL) {
			printf("%s: internal error.(r_s) %2d mode=0x%08x\n", __FUNCTION__, number, mode);
			return -1;
		}
		break;
	default:
		break;
	}

	/* (1) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<1>check mode=(%2d)\n", __FUNCTION__, bit_shift);
		*r_v = (unsigned int)simple_strtol(input, &errptr, 16);
		if ((*errptr == '\0') || (*errptr == 0xa)) {
		}
		else {
			printf("Invalid syntax. (Input Hex code Error:factor=%d)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
	}
	/* (2) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<2>check mode=(%2d)\n", __FUNCTION__, bit_shift);
		p = strtok(input,".");
		p = strtok(NULL ,".");
		if (p != NULL) {
			*(p-1) = 0x00;
		}
		simple_strtol(input, &errptr, 10);
		if ((*errptr == '\0') || (*errptr == 0xa)) {
		}
		else {
			printf("Invalid syntax. (caractor convert eroor:factor=%d.1)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		if (p != NULL) {
			simple_strtol(p, &errptr, 10);
			if ((*errptr == '\0') || (*errptr == 0xa)) {
			}
			else {
				printf("Invalid syntax. (caractor convert eroor:factor=%d.2)\n", bit_shift);
				goto CHK_PARAM_NG;
			}
		}
		memcpy(input, in_buf, sizeof(input)-1);
		// データのコピーは(4)のチェック後に実施するのでここでは未実施
	}
	/* (3) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<3> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		switch(pi_param_size[number]) {
		case 1:
			*r_v &= 0xff;
			break;
		case 2:
			*r_v &= 0xffff;
			break;
		case 4:
			*r_v &= 0xffffffff;
			break;
		default:
			break;
		}
	}
	/* (4) */
	/* 入力文字数のチェック */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<4> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		if (strlen(input) != pi_param_size[number]) {
			printf("Invalid syntax. (Caractor length error:factor=%d)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		/* All Burankを入力された場合の設定判定 */
		if ((number == 6) || (number == 9) || (number == 15) || (number == 16) || (number == 17)) {
			strncpy(r_s, input, pi_param_size[number]);
		}
		else {
			/* その他の項目のデータのコピーは他のチェック後に実施するのでここでは未実施 */
		}
	}
	/* (5) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<5> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		// MainCmdFuncInventory()で実施
	}
	/* (6) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<6> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		p = strtok(input," ");
		if (strlen(p) != 2) {
			printf("Invalid syntax. (need burank :factor=%d)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		r_s[0] = 0x20;	// space
		strncpy(&r_s[1], p, pi_param_size[number]-1);
	}
	/* (7) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<7> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		p = strtok(input,"-");
		p = strtok(NULL,"-");
		if (p == NULL) {
			printf("Invalid syntax. (need - :factor=%d.1)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		len = strlen(p);
		if (len > 4) {
			printf("Invalid syntax. (-@@@@ length error :factor=%d.2)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		strncpy(r_s, in_buf, pi_param_size[number]);
		if (len < 4) r_s[8] = 0x20;	// space
		if (len < 3) r_s[7] = 0x20;	// space
		if (len < 2) r_s[6] = 0x20;	// space
		if (len < 1) r_s[5] = 0x20;	// space
	}
	/* (8) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<8> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		if ((input[0] != 'F') || (input[1] != 'C')) {
			printf("Invalid syntax. (need FC caractor:factor=%d.1)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		strncpy(r_s, input, pi_param_size[number]);
		len = strlen(&input[2]);
		if (len < 8) r_s[9] = 0x20;	// space
		if (len < 7) r_s[8] = 0x20;	// space
		if (len < 6) r_s[7] = 0x20;	// space
		if (len < 5) r_s[6] = 0x20;	// space
		if (len < 4) r_s[5] = 0x20;	// space
		if (len < 3) r_s[4] = 0x20;	// space
		if (len < 2) r_s[3] = 0x20;	// space
		if (len < 1) r_s[2] = 0x20;	// space
	}
	/* (9) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<9> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		p = strtok(input,".");
		p = strtok(NULL,".");
		if (p == NULL) {
			printf("Invalid syntax. (need YY :factor=%d.1)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		len = strlen(p);
		if (len != 2) {
			printf("Invalid syntax. (need MM :factor=%d.2)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		strncpy(r_s, in_buf, pi_param_size[number]);
	}
	/* (10) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<10> check mode=(%2d) sub=%2d\n", __FUNCTION__, bit_shift,sub);
		switch (sub) {
		case 1:
			DBG_PRT_INV(DBG_LEVEL_4,"%s:<10> input=%s\n", __FUNCTION__, input);
			DBG_PRT_INV(DBG_LEVEL_4,"%s:<10> pi_chek_val_1[0][0]=%c \n", __FUNCTION__, pi_chek_val_1[0][0]);
			DBG_PRT_INV(DBG_LEVEL_4,"%s:<10> pi_chek_val_1[0][1]=%c \n", __FUNCTION__, pi_chek_val_1[0][1]);
			DBG_PRT_INV(DBG_LEVEL_4,"%s:<10> pi_chek_val_1[1][0]=%c \n", __FUNCTION__, pi_chek_val_1[1][0]);
			DBG_PRT_INV(DBG_LEVEL_4,"%s:<10> pi_chek_val_1[1][1]=%c \n", __FUNCTION__, pi_chek_val_1[1][1]);
			
			if (((input[0] == pi_chek_val_1[0][0]) && (input[1] == pi_chek_val_1[0][1])) ||
				((input[0] == pi_chek_val_1[1][0]) && (input[1] == pi_chek_val_1[1][1]))){
				strncpy(r_s, input, pi_param_size[number]);
			}
			else {
				printf("Invalid syntax. (Only use 01 or 03 :factor=%d.%d)\n", bit_shift, sub);
				goto CHK_PARAM_NG;
			}
			break;
		default:
			break;
		}
	}
	/* (11) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<11> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		p = strtok(input," ");
		len = strlen(p);
		switch (len) {
		case 3:	// 末尾スペース無(文字列だけ)
			break;
		case 4:	// 末尾スペース有
			if (p[3] != 0x20) {
				printf("Invalid syntax. (need space :factor=%d.1)\n", bit_shift);
				goto CHK_PARAM_NG;
			}
			break;
		default:
			printf("Invalid syntax. (caractor lenght error:factor=%d.2)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		r_s[0] = 0x20;	// space
		strncpy(&r_s[1], p, pi_param_size[number]-2);
		r_s[4] = 0x20;	// space
	}
	/* (12) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<12> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		p = strtok(input,".");
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<12> p=%s \n", __FUNCTION__, p);
		if (p == NULL) {
			printf("Invalid syntax. (Not #####.## format:factor=%d.1)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		len = strlen(p);
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<12> len=%02x \n", __FUNCTION__,len);
		if (len != 5) {
			printf("Invalid syntax. (Not #####.## format:factor=%d.2)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		p = strtok(NULL,".");
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<12> p=%s \n", __FUNCTION__, p);
		if (p == NULL) {
			printf("Invalid syntax. (Not #####.## format:factor=%d.3)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		len = strlen(p);
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<12> len=%02x \n", __FUNCTION__,len);
		if (len != 2) {
			printf("Invalid syntax. (Not #####.## format:factor=%d.4)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		strncpy(r_s, in_buf, pi_param_size[number]);
		
	}
	/* (13) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<13> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		p = strtok(input,".");
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<13> p=%s \n", __FUNCTION__, p);
		if (p == NULL) {
			printf("Invalid syntax. (Not ###.## format:factor=%d.1)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		len = strlen(p);
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<13> len=%02x \n", __FUNCTION__,len);
		if (len != 3) {
			printf("Invalid syntax. (Not ###.## format:factor=%d.2)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		p = strtok(NULL,".");
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<13> p=%s \n", __FUNCTION__, p);
		if (p == NULL) {
			printf("Invalid syntax. (Not ###.## format:factor=%d.3)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		len = strlen(p);
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<13> len=%02x \n", __FUNCTION__,len);
		if (len != 2) {
			printf("Invalid syntax. (Not ###.## format:factor=%d.4)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		strncpy(r_s, in_buf, pi_param_size[number]);
	}
	/* (14) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<14> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		p = strtok(input,".");
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<14> p=%s \n", __FUNCTION__, p);
		if (p == NULL) {
			printf("Invalid syntax. (Not ###.# format:factor=%d.1)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		len = strlen(p);
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<14> len=%02x \n", __FUNCTION__,len);
		if (len != 3) {
			printf("Invalid syntax. (Not ###.# format:factor=%d.2)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		p = strtok(NULL,".");
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<14> p=%s \n", __FUNCTION__, p);
		if (p == NULL) {
			printf("Invalid syntax. (Not ###.# format:factor=%d.3)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		len = strlen(p);
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<14> len=%02x \n", __FUNCTION__,len);
		if (len != 1) {
			printf("Invalid syntax. (Not ###.# format:factor=%d.4)\n", bit_shift);
			goto CHK_PARAM_NG;
		}
		strncpy(r_s, in_buf, pi_param_size[number]);
	}
	/* (15) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<14> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		tmp = (unsigned int)simple_strtol(input, &errptr, 16);
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<14> tmp=(%2x)\n", __FUNCTION__, tmp);
		
		if((0x00 == tmp) || (0x01 == tmp)){
		} else {
			printf("Invalid syntax. (Only use 0x00 or 0x01 :factor=%d.%d)\n", bit_shift, sub);
			goto CHK_PARAM_NG;
		}
	}
	/* (16) */
	if (mode & (1<<bit_shift++)) {
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<15> check mode=(%2d)\n", __FUNCTION__, bit_shift);
		tmp2 = (unsigned int)simple_strtol(input, &errptr, 16);
		DBG_PRT_INV(DBG_LEVEL_4,"%s:<15> tmp=(%2x)\n", __FUNCTION__, tmp2);
		
		if(0xFF<tmp2){
			printf("Invalid syntax. (Only use 0x01--0xFF :factor=%d.%d)\n", bit_shift, sub);
			goto CHK_PARAM_NG;
		}
	}

	return 0;
	
 CHK_PARAM_NG:
	return -1;
}


/***********************************************************************/ 
/** 
 * 
 * @brief  inv_num_message_temp
 * @brief Dispaly P.I Command menu about temp Message
 * 
 * @param unsigned int  
 * 
 * @brief RETUN Type int
 * @retval 0 Normal end
 * @retval 1 Command Failed
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
int inv_num_message_temp(unsigned int number)
{
	unsigned int rtn = 0;
	
	if ((number < 1) || (ITEM_NUM_MAX_TEMP < number)) {
		printf("unknown number\n");
		
		return 1;
	}


	printf("Input hexadecimal number(ex 0x12) ->");
	
	return rtn;
}


/***********************************************************************/ 
/** 
 * 
 * @brief  inv_num_message
 * @brief Dispaly P.I Command menu Message
 * 
 * @param unsigned int  Number of menu
 * 
 * @brief RETUN Type int
 * @retval 0 Normal end
 * @retval 1 Command Failed
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
int inv_num_message(unsigned int number)
{
	unsigned int rtn = 0;
	
	if ((number < 1) || (ITEM_NUM_MAX < number)) {
		printf("unknown number\n");
		
		return 1;
	}

	DBG_PRT_INV(DBG_LEVEL_2,"%s:<0> START\n", __FUNCTION__);

	switch(number){
		case 1:					/* (1)  unit code                      */ 
#if defined(INVENTORY_UCODE_CHK)
			printf("Input hexadecimal number(ex 0x1234) ->");
#else
			printf("The unit code cannot be input.(0x%04x fixation)\n", card_kind);
			rtn=1;
#endif
			break;
		case 2:					/* (2)  version flag                   */
		case 18:				/* (18) Pressure Sensor Existence      */
		case 19:				/* (19) Min. FAN Step                  */
		case 20:				/* (20) Max. FAN Step                  */
		case 21:				/* (21) Min.Temperature Threshold      */
		case 22:				/* (22) Ambient Temperature pitch      */
		case 23:				/* (23) FAN Control Interval           */
		case 26:				/* (26) Ambient Temperature Hysteresis */
		case 27:				/* (27) Ambient Pressure Hysteresis    */
		case 28:				/* (28) Break Temp Over Protection     */
		case 29:				/* (29) Shelf Temp Alert Protection    */
		case 30:				/* (30) Shelf Temp ALM Protection      */
		case 31:				/* (31) Cooling Fail Protection        */
		case 32:				/* (32) Cooling Fail Clear Protection  */
		case 33:				/* (33) Cooling Fail Mask time         */
			printf("Input hexadecimal number(ex 0x12) ->");
			break;
		case 3:					/* (3)  issue number                   */
		case 4:					/* (4)  Abbreviation Name              */
		case 5:					/* (5)  FC number                      */
		case 6:					/* (6)  CLEI code                      */
		case 7:					/* (7)  Product Year and Month         */
		case 8:					/* (8)  2D label location              */
		case 9:					/* (9)  serial number                  */
		case 13:				/* (13) PCB Revision                   */
			printf("Input characters ->");
			break;
		case 15:				/* (15) Min. Wavelength                */
		case 16:				/* (16) Min. Wavelength                */
			printf("Input characters(ex 10000.00) ->");
			break;
		case 17:				/* (17) Frequency Spacing              */
			printf("Input characters(ex 100.00) ->");
			break;
		case 10:				/* (10) MAC Address Block Max          */
		case 11:				/* (11) MAC Address Bl                 */
			printf("Input MAC address\n");
			printf("ex. 00-17-42-01-23-45 ->");
			break;
		case 12:				/* (12) FWDL type. FPGA Download Issue */
		case 34:				/* (34) WarmUp Timer                   */
		case 35:				/* (35) PD1 Initial Value              */
		case 36:				/* (36) PD2 Initial Value              */
		case 37:				/* (37) Max ABS Threshold              */
		case 38:				/* (38) VOA Adjustment Fixed Val       */
		case 39:				/* (39) VOA Max Setting Value          */
		case 40:				/* (40) VOA Min Setting Value          */
		case 41:				/* (41) Loop Target Level              */
		case 42:				/* (42) Loop Target Threshold+         */
		case 43:				/* (43) Loop Target Threshold-         */
			printf("Input hexadecimal number(ex 0x1234) ->");
			break;
		case 24:				/* (24) Min. Pressure Threshold        */
			printf("Input Min. Pressure Threshold \n");
			printf("ex. 03-C8-03-9D-03-38-02-F6-00-00 ->");
			break;
		case 25:				/* (25) Initial FAN Step               */
			printf("Input Min. Pressure Threshold \n");
			printf("ex. 03-04-05-06-07-00 ->");
			break;
		case 14:				/* (14) Parts Temp. Threshold          */
			/* 画面切り替え */
			printf("Parts Temp. Threshold Start! \n");
			break;
		case 44:				/* (44) MAX Power Drawn                */
		case 45:				/* (45) Low Power Level                */
			printf("Input characters(ex 100.0) ->");
			break;
		case 46:				/* (46) Checksum                       */
			printf("The Checksum cannot be input\n");
			rtn=1;
			break;
		default:
			break;
	}
	
	DBG_PRT_INV(DBG_LEVEL_2,"%s:<0> END\n", __FUNCTION__);
	return rtn;
}



#define TMP_SIZE	16

/***********************************************************************/ 
/** 
 * 
 * @brief  MainCmdFuncInventory
 * @brief  main program of P.I Write/Read 
 * 
 * @param char*  a valu of comand parametor
 * @param int  a number of comand parametor
 * 
 * @brief RETUN Type int MainCmdFuncInventory
 * @retval 0 Normal end
 * @retval 1 Command Failed
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
int MainCmdFuncInventory (char* paramString[], int paramNum)
{
	int	i,j,k,ret=0;
	uint8_t *sp;
	int	number;
	int	number2;
	int	my_side=0;
	char   *errptr;
	unsigned int	value;
	struct inventory_t *common, *sav_common, *common_p[MAX_SIDE_NUM];
	char   input[20];
	unsigned char   *mac_str[6];
	unsigned char   *fan_str1[10];
	unsigned char   *fan_str2[6];

	int item_num = 0;
	int item_num2 = 0;
	char *card_name_ptr = NULL;
	int rtn = 0;

	int	init_flag = 0;
	int	cksum_flag = 0;
	int	side_deg_flag = 0;

	int def_flag = 0;										/* 0=EEPROM内容表示、1=Defult内容表示 */
	struct inventory_info_t	*def_data;
	unsigned char	r_side=0;								/* 0=1面、1=2面 */
	char			tmp[TMP_SIZE];
	
	DBG_PRT_INV(DBG_LEVEL_3,"%s:paramNum=%d \n", __FUNCTION__,paramNum);
	/* パラメタ個数チェック */
	if((paramNum < 1) || (4 < paramNum))
	{
		printf("Invalid parameter.\n");
		return 1;
	}

	DBG_PRT_INV(DBG_LEVEL_3,"%s:paramString[0]=%s \n", __FUNCTION__,paramString[0]);
	DBG_PRT_INV(DBG_LEVEL_3,"%s:paramString[1]=%s \n", __FUNCTION__,paramString[1]);
	DBG_PRT_INV(DBG_LEVEL_3,"%s:paramString[2]=%s \n", __FUNCTION__,paramString[2]);
	DBG_PRT_INV(DBG_LEVEL_3,"%s:paramString[3]=%s \n", __FUNCTION__,paramString[3]);
	
	if ((paramNum == 2) && (strcmp(paramString[0],"-f") == 0)) {
		/* inventory -f <bladname> */
		init_flag = 1;										/* 初期化モード */
		card_name_ptr = paramString[1];
	}
	else if ((paramNum == 2) && (strcmp(paramString[0],"-c") == 0)) {
		/* inventory -c <bladname> */
		cksum_flag = 1;										/* CheckSum表示モード */
		card_name_ptr = paramString[1];
	}
	else if ((paramNum == 3) && (strcmp(paramString[1],"-s") == 0)) {
		/* inventory <bladname> -s n */
		side_deg_flag = 1;									/* 面指定モード */
		card_name_ptr = paramString[0];
		my_side =simple_strtoul(paramString[2], NULL, 16);
		
		if(1<my_side){
			printf("Invalid parameter. (option=%s)\n", paramString[0]);
			return 1;
		}
	}
	else if ((paramNum == 4) && (strcmp(paramString[0],"-c") == 0) && (strcmp(paramString[2],"-s") == 0)) {
		/* inventory -c <bladname> -s n */
		cksum_flag = 1;										/* CheckSum表示モード */
		side_deg_flag = 1;									/* 面指定モード */
		card_name_ptr = paramString[1];
		my_side =simple_strtoul(paramString[3], NULL, 16);
		
		if(1<my_side){
			printf("Invalid parameter. (option=%s)\n", paramString[0]);
			return 1;
		}
	}
	else if (paramNum == 1) {
		/* inventory <bladname> */
		card_name_ptr = paramString[0];
	}
	else {
		printf("Invalid parameter. (option=%s)\n", paramString[0]);
		return 1;
	}

	/* カード名称判断 */
	if(strcmp(card_name_ptr, CARD_NAME_CPSIE) == 0) {
		card_kind = CODE_CPSIE;
		def_data  = &pi_default_cpsie;
	}
	else if (strcmp(card_name_ptr, CARD_NAME_CPULP) == 0) {
		card_kind = CODE_CPULP;
		def_data  = &pi_default_cpulp;
	}
	else if (strcmp(card_name_ptr, CARD_NAME_CPUTP) == 0) {
		card_kind = CODE_CPUTP;
		def_data  = &pi_default_cputp;
	}
	else if (strcmp(card_name_ptr, CARD_NAME_CPUSP) == 0) {
		card_kind = CODE_CPUSP;
		def_data  = &pi_default_cpusp;
	}
	else if (strcmp(card_name_ptr, CARD_NAME_FPSIE) == 0) {
		card_kind = CODE_FPSIE;
		def_data  = &pi_default_fpsie;
	}
	else if (strcmp(card_name_ptr, CARD_NAME_FPULP) == 0) {
		card_kind = CODE_FPULP;
		def_data  = &pi_default_fpulp;
	}
	else if (strcmp(card_name_ptr, CARD_NAME_FPUTP) == 0) {
		card_kind = CODE_FPUTP;
		def_data  = &pi_default_fputp;
	}
	else if (strcmp(card_name_ptr, CARD_NAME_FPUSP) == 0) {
		card_kind = CODE_FPUSP;
		def_data  = &pi_default_fpusp;
	}
	else {
		printf("device must be %s, %s, %s, %s, %s, %s, %s, %s\n",
			   CARD_NAME_CPSIE, CARD_NAME_CPULP, CARD_NAME_CPUTP, CARD_NAME_CPUSP,
			   CARD_NAME_FPSIE, CARD_NAME_FPULP, CARD_NAME_FPUTP, CARD_NAME_FPUSP);
		return 1;
	}

/*	printf("inventory START!\n"); */

	
	/* 各面へのポインタを格納 */
	/* buf[MAX_BUF_SIZE]はGlobal Buffer    */
	/* common_p[0] は 1面用Buffer ポインタ */
	/* common_p[1] は 2面用Buffer ポインタ */
	for (i=0; i<MAX_SIDE_NUM; i++) {
		common_p[i] = (struct inventory_t*)&buf[i*SIZE_SIDE];
		
		DBG_PRT_INV(DBG_LEVEL_3,"%s: %d common_p=%p\n", __FUNCTION__, i, common_p[i]);
	}
	/* spはEEPROM Read/Write時に使用するポインタ */
	sp	 = (uint8_t*)buf;
	
	DBG_PRT_INV(DBG_LEVEL_3,"%s: sp=%p buf=%p \n", __FUNCTION__, sp, buf);
	
	/* Initialize inventory EEPROM with 0xFF */
	if (init_flag == 1) {
		/* 初期化モード ON */
		printf("Initialize inventory EEPROM with 0xFF\n");
		
		/* Global Bufferに0xFFを1面と2面に詰める */
		for(j=0;j<MAX_BUF_SIZE;j++){
			buf[j] = 0xff;
		}
		
		/* 64Kbit全領域初期化 */
		for(i=0;i<0x10;i++){
			if ((ret = DevE2pDataWrite(i*MAX_BUF_SIZE, sp, MAX_BUF_SIZE)) != 0) {
				printf("EEPROM Write NG\n");
				goto CMD_EXIT;
			}
			printf("*"); fflush(stdout);
		}
		printf("\n");
		ret = 0;											/* 正常終了 */
		goto CMD_EXIT;
	}

	/*============================================================================================*/
	/* Common Part                                                                                */
	/*============================================================================================*/

	/* EEPROM値読み出し */
	if ((ret = DevE2pDataRead(0, sp, MAX_BUF_SIZE)) != 0){
		printf("EEPROM Read NG\n");
		goto CMD_EXIT;
	}

	/* 初期状態かチェック */
	for (i=0; i<MAX_SIDE_NUM; i++) {
		/* common_p[i]は spポインタ,bufポインタと同じなのでこれを使用 */
		common = common_p[i];
		/* 下記条件を満たす場合は初期化状態とみなす */
		if ((common->code == 0xffff) &&
			(((unsigned char)common->issue[0] == 0xffU) && ((unsigned char)common->issue[1] == 0xffU) && ((unsigned char)common->issue[2] == 0xffU)) &&
			(common->checksum == 0xffff)) {
			DBG_PRT_INV(DBG_LEVEL_3,"%s: EEPROM initialized.\n", __FUNCTION__);
			def_flag = 1;									/* Defult Data表示Flag On */
			break;
		}
	}
	
	if (def_flag == 0) {
		/* チェックサム値チェック */
		for (i=0; i<MAX_SIDE_NUM; i++) {
			common = common_p[i];
			if (calcCheckSum((unsigned char*)&common->code, 0xFE) == common->checksum) {
				break;
			}
			printf("EEPROM %d side checksum NG.\n", i+1);
		}
		if (i == MAX_SIDE_NUM) {
			def_flag = 1;	/* 全面チェックサム異常 */
		}
		else {
			r_side = i;		/* チェックサムOKの面番号 */
		}
	}
	
	/* デフォルトデータ表示モードかを判断 */
	if (def_flag) {
		PRT_MSG_DEFAULT_VALUE;
		for (i=0; i<MAX_SIDE_NUM; i++) {
			
			/* Bufferに デフォルトデータを入れなおす */
			STORE_BUFFER(common_p[i], def_data);
		}
	}
	else {
		/* 既存データの場合は既にReadしたBuffer内容をそのまま使用 */
		PRT_MSG_HARDWARE_VALUE(r_side);
	}
	
	/* common_p[i]は spポインタ,bufポインタと同じなのでこれを使用 */
	
	/* 面指定があるかを判断 */
	if (side_deg_flag) {
		common = common_p[my_side];
		printf("Side %d Display\n", my_side);
		DBG_PRT_INV(DBG_LEVEL_3,"%s: Side ON my_side=%d common=%p \n", __FUNCTION__, my_side, common);
	} else {
		/* チェックサムOKの面番号で表示 */
		common = common_p[r_side];
		DBG_PRT_INV(DBG_LEVEL_3,"%s: Side OF r_side=%d common=%p \n", __FUNCTION__, r_side, common);
	}
	
	for (;;) {
		errptr = (char *)NULL;
		printf("\n\n");

		item_num = 1;
		printf("%2d: unit code                          ->0x%04X\n", item_num++, common->code);
		printf("%2d: version flag                       ->0x%02X\n", item_num++, common->version_flag);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->issue,			pi_param_size[item_num]);
		printf("%2d: issue number                       ->\"%s\"\n", item_num++, tmp);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->acronym,		pi_param_size[item_num]);
		printf("%2d: acronym                            ->\"%s\"\n", item_num++, tmp);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->fc_num,		pi_param_size[item_num]);
		printf("%2d: FC number                          ->\"%s\"\n", item_num++, tmp);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->clei,			pi_param_size[item_num]);
		printf("%2d: CLEI code                          ->\"%s\"\n", item_num++, tmp);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->date,			pi_param_size[item_num]);
		printf("%2d: production date                    ->\"%s\"\n", item_num++, tmp);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->td_label_loc,	pi_param_size[item_num]);
		printf("%2d: 2D label location                  ->\"%s\"\n", item_num++, tmp);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->serial_num,	pi_param_size[item_num]);
		printf("%2d: serial number                      ->\"%s\"\n", item_num++, tmp);
		printf("%2d: MAC Addr Block Min                 ->%02X-%02X-%02X-%02X-%02X-%02X\n",
							item_num++, common->mac_max[0], common->mac_max[1], common->mac_max[2],
										common->mac_max[3], common->mac_max[4], common->mac_max[5]);
		printf("%2d: MAC Addr Block Max                 ->%02X-%02X-%02X-%02X-%02X-%02X\n",
							item_num++, common->mac_min[0], common->mac_min[1], common->mac_min[2],
										common->mac_min[3], common->mac_min[4], common->mac_min[5]);
		printf("%2d: FPGA Download Issue                ->0x%04X\n", item_num++, common->fwdl_type);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->pcb_revision,	pi_param_size[item_num]);
		printf("%2d: PCB Revision                       ->\"%s\"\n", item_num++, tmp);
		printf("%2d: Parts Temp. Threshold\n",           item_num++);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->min_wave_length,	pi_param_size[item_num]);
		printf("%2d: Min Wavelength                     ->\"%s\"\n", item_num++, tmp);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->max_wave_length,	pi_param_size[item_num]);
		printf("%2d: Max Wavelength                     ->\"%s\"\n", item_num++, tmp);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->frequency_spacing,	pi_param_size[item_num]);
		printf("%2d: Frequency Spacing                  ->\"%s\"\n", item_num++, tmp);
		printf("%2d: Pressure Sensor                    ->0x%02X\n", item_num++, common->pressure_sensor);
		printf("%2d: Min FAN Step                       ->0x%02X\n", item_num++, common->min_fan_step);
		printf("%2d: Max FAN Step                       ->0x%02X\n", item_num++, common->max_fan_step);
		printf("%2d: FAN#Min.Temperature Threshold      ->0x%02X\n", item_num++, common->min_tmp_threshold);
		printf("%2d: FAN#Ambient Temperature pitch      ->0x%02X\n", item_num++, common->amb_tmp_pitch);
		printf("%2d: FAN#Control Interval               ->0x%02X\n", item_num++, common->fan_cnt_intervall);
		printf("%2d: FAN#Min. Pressure Threshold        ->%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X\n", item_num++,
		common->min_pre_threshold[0], common->min_pre_threshold[1], common->min_pre_threshold[2],common->min_pre_threshold[3],
		common->min_pre_threshold[4], common->min_pre_threshold[5], common->min_pre_threshold[6],common->min_pre_threshold[7],
		common->min_pre_threshold[8], common->min_pre_threshold[9]);
		printf("%2d: FAN#Initial FAN Step               ->%02X-%02X-%02X-%02X-%02X-%02X\n", item_num++,
		common->int_fan_step[0],common->int_fan_step[1],common->int_fan_step[2],common->int_fan_step[3],common->int_fan_step[4],common->int_fan_step[5]);
		printf("%2d: FAN#Ambient Temperature Hysteresis ->0x%02X\n", item_num++, common->amb_tmp_hysteresis);
		printf("%2d: FAN#Ambient Pressure Hysteresis    ->0x%02X\n", item_num++, common->amb_prs_hysteresis);
		printf("%2d: FAN#Break Temp Over Protection     ->0x%02X\n", item_num++, common->brk_tmp_overprotect);
		printf("%2d: FAN#Shelf Temp Alert Protection    ->0x%02X\n", item_num++, common->slf_tmp_alrtprotect);
		printf("%2d: FAN#Shelf Temp ALM Protection      ->0x%02X\n", item_num++, common->slf_tmp_almprotect);
		printf("%2d: FAN#Cooling Fail Protection        ->0x%02X\n", item_num++, common->col_fail_protect);
		printf("%2d: FAN#Cooling Fail Clear Protection  ->0x%02X\n", item_num++, common->col_fail_clerprotect);
		printf("%2d: FAN#Cooling Fail Mask time         ->0x%02X\n", item_num++, common->col_fail_masktime);
		printf("%2d: warm up timer                      ->0x%04X\n", item_num++, common->warm_up);
		printf("%2d: PD1 Initial Value                  ->0x%04X\n", item_num++, common->pd1_ini_val);
		printf("%2d: PD2 Initial Value                  ->0x%04X\n", item_num++, common->pd2_ini_val);
		printf("%2d: Max ABS Threshold                  ->0x%04X\n", item_num++, common->max_abs_th);
		printf("%2d: VOA Adjustment Val                 ->0x%04X\n", item_num++, common->voa_adjustment_value);
		printf("%2d: VOA Max Setting Value              ->0x%04X\n", item_num++, common->voa_max_setting_value);
		printf("%2d: VOA Min Setting Value              ->0x%04X\n", item_num++, common->voa_min_setting_value);
		printf("%2d: Loop Target Level                  ->0x%04X\n", item_num++, common->loop_target_level);
		printf("%2d: Loop Target Threshold+             ->0x%04X\n", item_num++, common->loop_target_th_plus);
		printf("%2d: Loop Target Threshold-             ->0x%04X\n", item_num++, common->loop_target_th_minus);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->max_pow_drawn,		pi_param_size[item_num]);
		printf("%2d: MAX Power Drawn                    ->\"%s\"\n", item_num++, tmp);
		memset(tmp, 0, TMP_SIZE);	strncpy(tmp, common->low_power_level,	pi_param_size[item_num]);
		printf("%2d: Low Power Level                    ->\"%s\"\n", item_num++, tmp);
		if(1 == cksum_flag) {
			printf("%2d: Checksum                           ->0x%04X\n", item_num++, common->checksum);
		}
		
		/* sleep */
#if 0
		i_eeprom_msleep(10);
#endif

		if (def_flag) {	/* デフォルトデータ */
			PRT_MSG_DEFAULT_VALUE;
		}
		else {			/* 既存データ */
			PRT_MSG_HARDWARE_VALUE(r_side);
		}
		printf(" Q: exit this part\n");

		printf("\nSelect number ->");

		/* キー入力(項目番号) */
		MainCmdCom_datainput_inventory(input, sizeof(input)-1);

		/* 終了か？ */
		if (((input[0] == 'Q') || (input[0] == 'q')) &&
			((input[1] == 0xa) || (input[1] == 0x0))) {
			break;
		}

		/* 指定項目番号のチェック */
		number = (int)simple_strtol(input, &errptr, 10);
		
		if(1 == inv_num_message(number)){
			continue;
		}
		
		if(14 != number){
			/* キー入力（項目値）*/
			MainCmdCom_datainput_inventory(input, sizeof(input)-1);
		}else{
			/* Parts Temp. Threshold 用メニュー表示 */
			
			for (;;) {
				errptr = (char *)NULL;
				printf("\n\n");

				DBG_PRT_INV(DBG_LEVEL_1,"%s:<0> Temp Root\n", __FUNCTION__);

				item_num2 = 1;
				for (i=0; i<TEMP_NUM; i++) {
					printf("%2d: Break Temp #%-2d                      ->0x%02X\n",item_num2++, i+1, common->temp_settings[i].break_temp);
					printf("%2d: Max Temp   #%-2d                      ->0x%02X\n",item_num2++, i+1, common->temp_settings[i].max_temp);
					printf("%2d: Min Temp   #%-2d                      ->0x%02X\n",item_num2++, i+1, common->temp_settings[i].min_temp);
				}
				
				if (def_flag) {	/* デフォルトデータ */
					PRT_MSG_DEFAULT_VALUE;
				}
				else {			/* 既存データ */
					PRT_MSG_HARDWARE_VALUE(r_side);
				}
				printf(" Q: exit this part\n");

				printf("\nSelect number ->");

				/* キー入力(項目番号) */
				MainCmdCom_datainput_inventory(input, sizeof(input)-1);

				/* 終了か？ */
				if (((input[0] == 'Q') || (input[0] == 'q')) &&
					((input[1] == 0xa) || (input[1] == 0x0))) {
					break;
				}

				/* 指定項目番号のチェック */
				number2 = (int)simple_strtol(input, &errptr, 10);
				
				if(1 == inv_num_message_temp(number2)){
					continue;
				}
				
				/* キー入力（項目値）*/
				MainCmdCom_datainput_inventory(input, sizeof(input)-1);
				
				switch (number2) {
				case 1:
				case 4:
				case 7:
				case 10:
				case 13:
				case 16:
				case 19:
				case 22:
				case 25:
				case 28:
					ret = CheckPIParam(0x8005, 0, input, number, &value, NULL);
					if (ret == 0) {
						k = (number2+2)/3;
						k = k-1;
						DBG_PRT_INV(DBG_LEVEL_1,"%s:<1> Temp Root number2 %02d j %02d\n", __FUNCTION__,number2,j);
						common->temp_settings[k].break_temp = value;
					}
					break;
				case 2:
				case 5:
				case 8:
				case 11:
				case 14:
				case 17:
				case 20:
				case 23:
				case 26:
				case 29:
					ret = CheckPIParam(0x8005, 0, input, number, &value, NULL);
					if (ret == 0) {
						k = (number2+1)/3;
						k = k-1;
						DBG_PRT_INV(DBG_LEVEL_1,"%s:<1> Temp Root number2 %02d j %02d\n", __FUNCTION__,number2,j);
						common->temp_settings[k].max_temp = value;
					}
					break;
				case 3:
				case 6:
				case 9:
				case 12:
				case 15:
				case 18:
				case 21:
				case 24:
				case 27:
				case 30:
					ret = CheckPIParam(0x8005, 0, input, number, &value, NULL);
					if (ret == 0) {
						k = number2/3;
						k = k-1;
						DBG_PRT_INV(DBG_LEVEL_1,"%s:<1> Temp Root number2 %02d j %02d\n", __FUNCTION__,number2,j);
						common->temp_settings[k].min_temp = value;
					}
					break;
				
				default:
					break;
				}
				DBG_PRT_INV(DBG_LEVEL_1,"%s:<1> Temp Root\n", __FUNCTION__);
			} /* for */
			
			DBG_PRT_INV(DBG_LEVEL_1,"%s:<3> Temp Root\n", __FUNCTION__);
			
			
			
		}
		
		switch (number) {
#if defined(INVENTORY_UCODE_CHK)
		case 1:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->code = (unsigned short)value;
			break;
#endif
		case 2:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->version_flag = value;
			break;
		case 3:
			ret = CheckPIParam(0x0028, 0, input, number, NULL, common->issue);
			break;
		case 4:
			ret = CheckPIParam(0x0048, 0, input, number, NULL, common->acronym);
			break;
		case 5:
			ret = CheckPIParam(0x0088, 0, input, number, NULL, common->fc_num);
			break;
		case 6:
			ret = CheckPIParam(0x0008, 0, input, number, NULL, common->clei);
			break;
		case 7:
			ret = CheckPIParam(0x0108, 0, input, number, NULL, common->date);
			break;
		case 8:
			ret = CheckPIParam(0x0208, 1, input, number, NULL, common->td_label_loc);
			break;
		case 9:
			ret = CheckPIParam(0x0008, 0, input, number, NULL, common->serial_num);
			break;
		case 10:
			mac_str[0] = (unsigned char *)strtok(input,"-");
			mac_str[1] = (unsigned char *)strtok(NULL,"-");
			mac_str[2] = (unsigned char *)strtok(NULL,"-");
			mac_str[3] = (unsigned char *)strtok(NULL,"-");
			mac_str[4] = (unsigned char *)strtok(NULL,"-");
			mac_str[5] = (unsigned char *)strtok(NULL,"-");

			/* syntax check */
			if((mac_str[0] == NULL) || (mac_str[1] == NULL) ||
			   (mac_str[2] == NULL) || (mac_str[3] == NULL) ||
			   (mac_str[4] == NULL) || (mac_str[5] == NULL)){
				printf("Invalid syntax. (factor=5)\n");
				break;
			}

			common->mac_max[0] = simple_strtoul((const char *)mac_str[0],NULL,16) & 0xFF;
			common->mac_max[1] = simple_strtoul((const char *)mac_str[1],NULL,16) & 0xFF;
			common->mac_max[2] = simple_strtoul((const char *)mac_str[2],NULL,16) & 0xFF;
			common->mac_max[3] = simple_strtoul((const char *)mac_str[3],NULL,16) & 0xFF;
			common->mac_max[4] = simple_strtoul((const char *)mac_str[4],NULL,16) & 0xFF;
			common->mac_max[5] = simple_strtoul((const char *)mac_str[5],NULL,16) & 0xFF;
			break;

		case 11:
			mac_str[0] = (unsigned char *)strtok(input,"-");
			mac_str[1] = (unsigned char *)strtok(NULL,"-");
			mac_str[2] = (unsigned char *)strtok(NULL,"-");
			mac_str[3] = (unsigned char *)strtok(NULL,"-");
			mac_str[4] = (unsigned char *)strtok(NULL,"-");
			mac_str[5] = (unsigned char *)strtok(NULL,"-");

			/* syntax check */
			if((mac_str[0] == NULL) || (mac_str[1] == NULL) ||
			   (mac_str[2] == NULL) || (mac_str[3] == NULL) ||
			   (mac_str[4] == NULL) || (mac_str[5] == NULL)){
				printf("Invalid syntax. (factor=5)\n");
				break;
			}

			common->mac_min[0] = simple_strtoul((const char *)mac_str[0],NULL,16) & 0xFF;
			common->mac_min[1] = simple_strtoul((const char *)mac_str[1],NULL,16) & 0xFF;
			common->mac_min[2] = simple_strtoul((const char *)mac_str[2],NULL,16) & 0xFF;
			common->mac_min[3] = simple_strtoul((const char *)mac_str[3],NULL,16) & 0xFF;
			common->mac_min[4] = simple_strtoul((const char *)mac_str[4],NULL,16) & 0xFF;
			common->mac_min[5] = simple_strtoul((const char *)mac_str[5],NULL,16) & 0xFF;
			break;
		case 12:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->fwdl_type = value;
			break;
		case 13:
			ret = CheckPIParam(0x0408, 0, input, number, NULL, common->pcb_revision);
			break;
		case 14:
			
			break;
		case 15:
			if (strcmp(input, "        ") == 0) {
				ret = CheckPIParam(0x0008, 0, input, number, NULL, common->min_wave_length);
			} else {
				ret = CheckPIParam(0x080A, 0, input, number, NULL, common->min_wave_length);
			}
			break;
		case 16:
			if (strcmp(input, "        ") == 0) {
				ret = CheckPIParam(0x0008, 0, input, number, NULL, common->max_wave_length);
			} else {
				ret = CheckPIParam(0x080A, 0, input, number, NULL, common->max_wave_length);
			}
			break;
		case 17:
			if (strcmp(input, "      ") == 0) {
				ret = CheckPIParam(0x0008, 0, input, number, NULL, common->frequency_spacing);
			}else{
				ret = CheckPIParam(0x100A, 0, input, number, NULL, common->frequency_spacing);
			}
			
			break;
		case 18:
			ret = CheckPIParam(0x4005, 2, input, number, &value, NULL);
			if (ret == 0)	common->pressure_sensor = value;
			break;
		case 19:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->min_fan_step = value;
			break;
		case 20:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->max_fan_step = value;
			break;
		case 21:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->min_tmp_threshold = value;
			break;
		case 22:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->amb_tmp_pitch = value;
			break;
		case 23:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->fan_cnt_intervall = value;
			break;
		case 24:
			fan_str1[0] = (unsigned char *)strtok(input,"-");
			fan_str1[1] = (unsigned char *)strtok(NULL,"-");
			fan_str1[2] = (unsigned char *)strtok(NULL,"-");
			fan_str1[3] = (unsigned char *)strtok(NULL,"-");
			fan_str1[4] = (unsigned char *)strtok(NULL,"-");
			fan_str1[5] = (unsigned char *)strtok(NULL,"-");
			fan_str1[6] = (unsigned char *)strtok(NULL,"-");
			fan_str1[7] = (unsigned char *)strtok(NULL,"-");
			fan_str1[8] = (unsigned char *)strtok(NULL,"-");
			fan_str1[9] = (unsigned char *)strtok(NULL,"-");
			
			/* syntax check */
			if((fan_str1[0] == NULL) || (fan_str1[1] == NULL) ||
			   (fan_str1[2] == NULL) || (fan_str1[3] == NULL) ||
			   (fan_str1[4] == NULL) || (fan_str1[5] == NULL) ||
			   (fan_str1[6] == NULL) || (fan_str1[7] == NULL) ||
			   (fan_str1[8] == NULL) || (fan_str1[9] == NULL) ){
				printf("Invalid syntax. (factor=5)\n");
				break;
			}
			
			common->min_pre_threshold[0] = simple_strtoul((const char *)fan_str1[0],NULL,16) & 0xFF;
			common->min_pre_threshold[1] = simple_strtoul((const char *)fan_str1[1],NULL,16) & 0xFF;
			common->min_pre_threshold[2] = simple_strtoul((const char *)fan_str1[2],NULL,16) & 0xFF;
			common->min_pre_threshold[3] = simple_strtoul((const char *)fan_str1[3],NULL,16) & 0xFF;
			common->min_pre_threshold[4] = simple_strtoul((const char *)fan_str1[4],NULL,16) & 0xFF;
			common->min_pre_threshold[5] = simple_strtoul((const char *)fan_str1[5],NULL,16) & 0xFF;
			common->min_pre_threshold[6] = simple_strtoul((const char *)fan_str1[6],NULL,16) & 0xFF;
			common->min_pre_threshold[7] = simple_strtoul((const char *)fan_str1[7],NULL,16) & 0xFF;
			common->min_pre_threshold[8] = simple_strtoul((const char *)fan_str1[8],NULL,16) & 0xFF;
			common->min_pre_threshold[9] = simple_strtoul((const char *)fan_str1[9],NULL,16) & 0xFF;
			break;
		case 25:
			fan_str2[0] = (unsigned char *)strtok(input,"-");
			fan_str2[1] = (unsigned char *)strtok(NULL,"-");
			fan_str2[2] = (unsigned char *)strtok(NULL,"-");
			fan_str2[3] = (unsigned char *)strtok(NULL,"-");
			fan_str2[4] = (unsigned char *)strtok(NULL,"-");
			fan_str2[5] = (unsigned char *)strtok(NULL,"-");
			
			/* syntax check */
			if((fan_str2[0] == NULL) || (fan_str2[1] == NULL) ||
			   (fan_str2[2] == NULL) || (fan_str2[3] == NULL) ||
			   (fan_str2[4] == NULL) || (fan_str2[5] == NULL) ){
				printf("Invalid syntax. (factor=5)\n");
				break;
			}
			
			common->int_fan_step[0] = simple_strtoul((const char *)fan_str2[0],NULL,16) & 0xFF;
			common->int_fan_step[1] = simple_strtoul((const char *)fan_str2[1],NULL,16) & 0xFF;
			common->int_fan_step[2] = simple_strtoul((const char *)fan_str2[2],NULL,16) & 0xFF;
			common->int_fan_step[3] = simple_strtoul((const char *)fan_str2[3],NULL,16) & 0xFF;
			common->int_fan_step[4] = simple_strtoul((const char *)fan_str2[4],NULL,16) & 0xFF;
			common->int_fan_step[5] = simple_strtoul((const char *)fan_str2[5],NULL,16) & 0xFF;
			break;
		case 26:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->amb_tmp_hysteresis = value;
			break;
		case 27:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->amb_prs_hysteresis = value;
			break;
		case 28:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->brk_tmp_overprotect = value;
			break;
		case 29:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->slf_tmp_alrtprotect = value;
			break;
		case 30:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->slf_tmp_almprotect = value;
			break;
		case 31:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->col_fail_protect = value;
			break;
		case 32:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->col_fail_clerprotect = value;
			break;
		case 33:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->col_fail_masktime = value;
			break;
		case 34:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->warm_up = value;
			break;
		case 35:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->pd1_ini_val = value;
			break;
		case 36:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->pd2_ini_val = value;
			break;
		case 37:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->max_abs_th = value;
			break;
		case 38:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->voa_adjustment_value = value;
			break;
		case 39:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->voa_max_setting_value = value;
			break;
		case 40:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->voa_min_setting_value = value;
			break;
		case 41:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->loop_target_level = value;
			break;
		case 42:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->loop_target_th_plus = value;
			break;
		case 43:
			ret = CheckPIParam(0x0005, 0, input, number, &value, NULL);
			if (ret == 0)	common->loop_target_th_minus = value;
			break;
		case 44:
			ret = CheckPIParam(0x200A, 0, input, number, NULL, common->max_pow_drawn);
			break;
		case 45:
			ret = CheckPIParam(0x200A, 0, input, number, NULL, common->low_power_level);
			break;
		default:
			break;
		}

		/* チェックサム表示モードの場合、チェックサムの再計算を行う */
	/*	if(1 == cksum_flag) { */
			common->checksum = calcCheckSum((unsigned char*)&common->code, 0xFE);
	/*	} */
		
	} /* for */

	/*--------------------------------------------*/
	/* store buffer                               */
	/*--------------------------------------------*/
	/* 面指定があるかを判断 */
	if (side_deg_flag) {
		
		if(0 == my_side) {
			sav_common = common_p[1];
			DBG_PRT_INV(DBG_LEVEL_3,"%s: Side ON my_side=%d sav_common=%p common_p[1]=%p\n", __FUNCTION__, my_side, sav_common,common_p[1]);
		} else {
			sav_common = common_p[0];
			DBG_PRT_INV(DBG_LEVEL_3,"%s: Side ON my_side=%d sav_common=%p common_p[0]=%p\n", __FUNCTION__, my_side, sav_common,common_p[0]);
		}
	} else {
		/* チェックサムOKの面番号で表示 */
		if(0 == r_side) {
			sav_common = common_p[1];
			DBG_PRT_INV(DBG_LEVEL_3,"%s: Side OF  r_side=%d sav_common=%p common_p[1]=%p\n", __FUNCTION__, r_side, sav_common,common_p[1]);
		} else {
			sav_common = common_p[0];
			DBG_PRT_INV(DBG_LEVEL_3,"%s: Side OF  r_side=%d sav_common=%p common_p[0]=%p\n", __FUNCTION__, r_side, sav_common,common_p[0]);
		}
	}


	
	/* 逆面Bufferにセーブする */
	STORE_BUFFER(sav_common, common);
	
	/******************************/
	/* buf -> EEPROM              */
	/******************************/
	printf("\nDo you write this values for inventory into device?(y/n)->");

	/* キー入力（終了判定）*/
	rtn = MainCmdCom_yesnoinput();	
	if (rtn == 0) {
		/* Yes */
		if ((ret = DevE2pDataWrite(0, sp, MAX_BUF_SIZE)) != 0) {
			printf("EEPROM Write NG\n");
			goto CMD_EXIT;
		}
	} else {
		/* No */
		printf("discard values and exit\n");
	}

 CMD_EXIT:

	return ret;
}

/***********************************************************************/ 
/** 
 * 
 * @brief  do_inventory
 * @brief  main program of inventory cmd
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
int do_inventory(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int rc = 0;
#ifdef DEBUG_CMD_INVENTORY
	inv_dbg = getenv_ulong("invdbg", 16, 0x0);
#endif
	rc = MainCmdFuncInventory((char**)&argv[1], (argc-1));
	
	if(0 != rc){
		return cmd_usage(cmdtp);
	} else {
		return rc;
	}
}

/***************************************************/
U_BOOT_CMD(
	inventory,	5,	0,	do_inventory,
	"P.I. utility command",
	"[-f | -c] <bladname>\n"
	"-f            - EEPROM initialization\n"
	"-c            - Display of checksum field\n"
	"<bladname>    - CPULP : for ULP CPUSUB P.I.\n"
	"                CPUTP : for UTP CPUSUB P.I.\n"
	"                CPUSP : for USP CPUSUB P.I.\n"
	"                CPSIE : for SIE CPUSUB P.I.\n"
#if 0
	"                FPSIE : for SIE FPGA P.I.\n"
	"                FPULP : for ULP FPGA P.I.\n"
	"                FPUTP : for UTP FPGA P.I.\n"
	"                FPUSP : for USP FPGA P.I.\n"
#endif

);

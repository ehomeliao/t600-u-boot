/************************************************************************/ 
/** 
 * @file cmd_inventory.h
 * @brief This is a HeaderFile that specializes
 * @brief in P2041 FJ individual board. 
 * @author FJT)Konno
 * $Date:: 2016-03-31 18:26:40 +0900#$$
 * 
 * SPDX-License-Identifier: GPL-2.0+
 * This program is free software: you can redistribute it and/or modify
 * it un under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * 
 * COPYRIGHT(C) FUJITSU LIMITED 2015
 */
/************************************************************************/ 

#ifndef __CMD_INVENTORY_H__
#define __CMD_INVENTORY_H__

/*----------------------------------------------------------------------*/
/* for inventory command												*/
/*----------------------------------------------------------------------*/
/* #define DEBUG_CMD_INVENTORY */

#define DBG_LEVEL_1 1 /* Main関数内のシーケンス Level  */
#define DBG_LEVEL_2 2 /* Sub 関数内のシーケンス Level  */
#define DBG_LEVEL_3 3 /* Main関数内の詳細Data出力Level */
#define DBG_LEVEL_4 4 /* Sub 関数内の詳細Data出力Level */

/* #define DEBUG_CMD_INVENTORY */

#ifdef DEBUG_CMD_INVENTORY
extern unsigned int inv_dbg;

#define inventory_dbg_cond(level,inv_dbg,fmt, args...)	\
	do {												\
		if((0 != inv_dbg)&&(level<inv_dbg))				\
			printf(pr_fmt(fmt), ##args);				\
	} while (0)

#define DBG_PRT(xx...)	printf(xx);
#define DBG_PRT_INV(level,fmt, args...)			\
		inventory_dbg_cond(level,inv_dbg, fmt, ##args)
#else
#define DBG_PRT(xx...)
#define DBG_PRT_INV(level,fmt, args...)
#endif

#define INVENTORY_UCODE_CHK


#define MAX_SIDE_NUM		2
#define SIZE_SIDE			0x100

#define CARD_NAME_CPSIE		"CPSIE"
#define CARD_NAME_CPULP		"CPULP"
#define CARD_NAME_CPUTP		"CPUTP"
#define CARD_NAME_CPUSP		"CPUSP"
#define CARD_NAME_FPSIE		"FPSIE"
#define CARD_NAME_FPULP		"FPULP"
#define CARD_NAME_FPUTP		"FPUTP"
#define CARD_NAME_FPUSP		"FPUSP"

#define CODE_CPSIE			0x0001	// T.B.D.
#define CODE_CPULP			0x0002	// T.B.D.
#define CODE_CPUTP			0x0003	// T.B.D.
#define CODE_CPUSP			0x1111
#define CODE_FPSIE			0x0005	// T.B.D.
#define CODE_FPULP			0x0006	// T.B.D.
#define CODE_FPUTP			0x0007	// T.B.D.
#define CODE_FPUSP			0x0008	// T.B.D.

#define TEMP_NUM			10
typedef struct {
	unsigned char	break_temp;	
	unsigned char	max_temp;
	unsigned char	min_temp;
} pi_temp_t;

#define FAN_TEMP_TH			5		// 環境温度閾値の数
#define FAN_HIGH_DEGREE		4		// 高度種別の数

struct inventory_t {
	unsigned short	code;						/* 0000-0001 (1)  unit code							*/
	unsigned char	version_flag;				/* 0002      (2)  version flag						*/
	char			issue[3];					/* 0003-0005 (3)  issue number						*/
	char			acronym[9];					/* 0006-000E (4)  Abbreviation Name					*/
	char			fc_num[10];					/* 000F-0018 (5)  FC number							*/
	char			clei[10];					/* 0019-0022 (6)  CLEI code							*/
	char			date[5];					/* 0023-0027 (7)  Product Year and Month			*/
	char			td_label_loc[2];			/* 0028-0029 (8)  2D label location					*/
	char			serial_num[5];				/* 002A-002E (9)  serial number						*/
	unsigned char	rsv1;						/* 002F												*/
	unsigned char	rsv2;						/* 0030												*/
	unsigned char	rsv3;						/* 0031												*/
	unsigned char	mac_max[6];					/* 0032-0037 (10) MAC Address Block Max				*/
	unsigned char	mac_min[6];					/* 0038-003D (11) MAC Address Block Minimum			*/
	unsigned short	fwdl_type;					/* 003E-003F (12) FWDL type. FPGA Download Issue	*/
	unsigned char	rsv4[31];					/* 0040-005E										*/
	unsigned char	rsv5;						/* 005F												*/
	char			pcb_revision[5];			/* 0060-0064 (13) PCB Revision						*/
	unsigned char	rsv6[3];					/* 0065-0067										*/
	unsigned char	rsv7[3];					/* 0068-006A										*/
	unsigned char	rsv8;						/* 006B												*/
	pi_temp_t		temp_settings[TEMP_NUM];	/* 006C-0089 (14) 006C-0089 Parts Temp. Threshold	*/
	char			min_wave_length[8];			/* 008A-0091 (15) 008A-0091 Min. Wavelength			*/
	char			max_wave_length[8];			/* 0092-0099 (16) 0092-0099 Min. Wavelength			*/
	char			frequency_spacing[6];		/* 009A-009F (17) 009A-009F Frequency Spacing		*/
	unsigned char	rsv9;						/* 00A0												*/
	unsigned char	pressure_sensor;			/* 00A1      (18) Pressure Sensor Existence			*/
	unsigned char	min_fan_step;				/* 00A2      (19) Min. FAN Step						*/
	unsigned char	max_fan_step;				/* 00A3      (20) Max. FAN Step						*/
	unsigned char	min_tmp_threshold;			/* 00A4      (21) Min.Temperature Threshold			*/
	unsigned char	amb_tmp_pitch;				/* 00A5      (22) Ambient Temperature pitch			*/
	unsigned char	fan_cnt_intervall;			/* 00A6      (23) FAN Control Interval				*/
	unsigned char	min_pre_threshold[10];		/* 00A7-00B0 (24) Min. Pressure Threshold			*/
	unsigned char	int_fan_step[6];			/* 00B1-00B6 (25) Initial FAN Step					*/
	unsigned char	amb_tmp_hysteresis;			/* 00B7      (26) Ambient Temperature Hysteresis	*/
	unsigned char	amb_prs_hysteresis;			/* 00B8      (27) Ambient Pressure Hysteresis		*/
	unsigned char	brk_tmp_overprotect;		/* 00B9      (28) Break Temp Over Protection		*/
	unsigned char	slf_tmp_alrtprotect;		/* 00BA      (29) Shelf Temp Alert Protection		*/
	unsigned char	slf_tmp_almprotect;			/* 00BB      (30) Shelf Temp ALM Protection			*/
	unsigned char	col_fail_protect;			/* 00BC      (31) Cooling Fail Protection			*/
	unsigned char	col_fail_clerprotect;		/* 00BD      (32) Cooling Fail Clear Protection		*/
	unsigned char	col_fail_masktime;			/* 00BE      (33) Cooling Fail Mask time			*/
	unsigned char	rsv10[13];					/* 00BF-00CB										*/
	unsigned short	warm_up;					/* 00CC-00CD (34) WarmUp Timer						*/
	unsigned char	rsv11[2];					/* 00CE-00CF										*/
	unsigned short	pd1_ini_val;				/* 00D0-00D1 (35) PD1 Initial Value					*/
	unsigned short	pd2_ini_val;				/* 00D2-00D3 (36) PD2 Initial Value					*/
	unsigned short	max_abs_th;					/* 00D4-00D5 (37) Max ABS Threshold					*/
	unsigned short	voa_adjustment_value;		/* 00D6-00D7 (38) VOA Adjustment Fixed Val			*/
	unsigned short	voa_max_setting_value;		/* 00D8-00D9 (39) VOA Max Setting Value				*/
	unsigned short	voa_min_setting_value;		/* 00DA-00DB (40) VOA Min Setting Value				*/
	unsigned short	loop_target_level;			/* 00DC-00DD (41) Loop Target Level					*/
	unsigned short	loop_target_th_plus;		/* 00DE-00DF (42) Loop Target Threshold+			*/
	unsigned short	loop_target_th_minus;		/* 00E0-00E1 (43) Loop Target Threshold-			*/
	char			max_pow_drawn[5];			/* 00E2-00E6 (44) MAX Power Drawn					*/
	char			low_power_level[5];			/* 00E7-00EB (45) Low Power Level					*/
	unsigned char	rsv12[18];					/* 00EC-00FD										*/
	unsigned short	checksum;					/* 00FE-00FF (46) Checksum							*/
};


struct inventory_info_t {
	unsigned short	code;						/* 0000-0001 (1)  unit code							*/
	unsigned char	version_flag;				/* 0002      (2)  version flag						*/
	char			*issue;						/* 0003-0005 (3)  issue number						*/
	char			*acronym;					/* 0006-000E (4)  Abbreviation Name					*/
	char			*fc_num;					/* 000F-0018 (5)  FC number							*/
	char			*clei;						/* 0019-0022 (6)  CLEI code							*/
	char			*date;						/* 0023-0027 (7)  Product Year and Month			*/
	char			*td_label_loc;				/* 0028-0029 (8)  2D label location					*/
	char			*serial_num;				/* 002A-002E (9)  serial number						*/
	unsigned char	rsv1;						/* 002F												*/
	unsigned char	rsv2;						/* 0030												*/
	unsigned char	rsv3;						/* 0031												*/
	unsigned char	mac_max[6];					/* 0032-0037 (10) MAC Address Block Max				*/
	unsigned char	mac_min[6];					/* 0038-003D (11) MAC Address Block Minimum			*/
	unsigned short	fwdl_type;					/* 003E-003F (12) FWDL type. FPGA Download Issue	*/
	unsigned char	rsv4[31];					/* 0040-005E										*/
	unsigned char	rsv5;						/* 005F												*/
	char			*pcb_revision;				/* 0060-0064 (13) PCB Revision						*/
	unsigned char	rsv6[3];					/* 0065-0067										*/
	unsigned char	rsv7[3];					/* 0068-006A										*/
	unsigned char	rsv8;						/* 006B												*/
	pi_temp_t		temp_settings[10];			/* 006C-0089 (14) 006C-0089 Parts Temp. Threshold	*/
	char			*min_wave_length;			/* 008A-0091 (15) 008A-0091 Min. Wavelength			*/
	char			*max_wave_length;			/* 0092-0099 (16) 0092-0099 Min. Wavelength			*/
	char			*frequency_spacing;			/* 009A-009F (17) 009A-009F Frequency Spacing		*/
	unsigned char	rsv9;						/* 00A0												*/
	unsigned char	pressure_sensor;			/* 00A1      (18) Pressure Sensor Existence			*/
	unsigned char	min_fan_step;				/* 00A2      (19) Min. FAN Step						*/
	unsigned char	max_fan_step;				/* 00A3      (20) Max. FAN Step						*/
	unsigned char	min_tmp_threshold;			/* 00A4      (21) Min.Temperature Threshold			*/
	unsigned char	amb_tmp_pitch;				/* 00A5      (22) Ambient Temperature pitch			*/
	unsigned char	fan_cnt_intervall;			/* 00A6      (23) FAN Control Interval				*/
	unsigned char	min_pre_threshold[10];		/* 00A7-00B0 (24) Min. Pressure Threshold			*/
	unsigned char	int_fan_step[6];			/* 00B1-00B6 (25) Initial FAN Step					*/
	unsigned char	amb_tmp_hysteresis;			/* 00B7      (26) Ambient Temperature Hysteresis	*/
	unsigned char	amb_prs_hysteresis;			/* 00B8      (27) Ambient Pressure Hysteresis		*/
	unsigned char	brk_tmp_overprotect;		/* 00B9      (28) Break Temp Over Protection		*/
	unsigned char	slf_tmp_alrtprotect;		/* 00BA      (29) Shelf Temp Alert Protection		*/
	unsigned char	slf_tmp_almprotect;			/* 00BB      (30) Shelf Temp ALM Protection			*/
	unsigned char	col_fail_protect;			/* 00BC      (31) Cooling Fail Protection			*/
	unsigned char	col_fail_clerprotect;		/* 00BD      (32) Cooling Fail Clear Protection		*/
	unsigned char	col_fail_masktime;			/* 00BE      (33) Cooling Fail Mask time			*/
	unsigned char	rsv10[13];					/* 00BF-00CB										*/
	unsigned short	warm_up;					/* 00CC-00CD (34) WarmUp Timer						*/
	unsigned char	rsv11[2];					/* 00CE-00CF										*/
	unsigned short	pd1_ini_val;				/* 00D0-00D1 (35) PD1 Initial Value					*/
	unsigned short	pd2_ini_val;				/* 00D2-00D3 (36) PD2 Initial Value					*/
	unsigned short	max_abs_th;					/* 00D4-00D5 (37) Max ABS Threshold					*/
	unsigned short	voa_adjustment_value;		/* 00D6-00D7 (38) VOA Adjustment Fixed Val			*/
	unsigned short	voa_max_setting_value;		/* 00D8-00D9 (39) VOA Max Setting Value				*/
	unsigned short	voa_min_setting_value;		/* 00DA-00DB (40) VOA Min Setting Value				*/
	unsigned short	loop_target_level;			/* 00DC-00DD (41) Loop Target Level					*/
	unsigned short	loop_target_th_plus;		/* 00DE-00DF (42) Loop Target Threshold+			*/
	unsigned short	loop_target_th_minus;		/* 00E0-00E1 (43) Loop Target Threshold-			*/
	char			*max_pow_drawn;				/* 00E2-00E6 (44) MAX Power Drawn					*/
	char			*low_power_level;			/* 00E7-00EB (45) Low Power Level					*/
	unsigned char	rsv12[18];					/* 00EC-00FD										*/
	unsigned short	checksum;					/* 00FE-00FF (46) Checksum							*/
};

/* P.I. Data (Default) */
struct inventory_info_t pi_default_cpsie =
{
	CODE_CPSIE,			/* (1) */
	0x01,				/* (2) */
	" 01",				/* (3) */
	"BDS1-BEA1",		/* (4) */
	"FC9543BEA1",		/* (5) */
	"WOTRC6AJAA",		/* (6) */
	"YY.MM",			/* (7) */
	"01",				/* (8) */
	"#####",			/* (9) */
	0x00, 0x00, 0x00,	/* rsv1-3 */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (10) */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (11) */
	0x0001,				/* (12) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/* rsv4-5 */
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 0x00,
	" ##A ",			/* (13) */
	{0x00,0x00,0x00}, {0x00,0x00,0x00}, 0x00,			/* rsv6-8 */
	{{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},	/* (14) */
	 {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00}},
	"        ",		/* (15) */
	"        ",		/* (16) */
	"      ",		/* (17) */
	0x00,			/* rsv9 */
	0x01,			/* (18) */
	0x00,			/* (19) */
	0x00,			/* (20) */
	0x0F,			/* (21) */
	0x05,			/* (22) */
	0x14,			/* (23) */
	{0x03,0xC8,0x03,0x9D,0x03,0x38,0x02,0xF6,0x00,0x00},	/* (24) */
	{0x03,0x04,0x05,0x06,0x07,0x00},						/* (25) */
	0x02,			/* (26) */
	0x14,			/* (27) */
	0x14,			/* (28) */
	0x14,			/* (29) */
	0x14,			/* (30) */
	0x1E,			/* (31) */
	0x3F,			/* (32) */
	0x3C,			/* (33) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	/* rsv_10 */
	0x0000,			// (34)
	{0x00,0x00},	// rsv11
	0x0000,			// (35)
	0x0000,			// (36)
	0x0000,			// (37)
	0x0000,			// (38)
	0x0000,			// (39)
	0x0000,			// (40)
	0x0000,			// (41)
	0x0000,			// (42)
	0x0000,			// (43)
	"000.0",		// (44)
	"000.0",		// (45)
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// rsv12
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	0x0000			// (46)
};

struct inventory_info_t pi_default_cpulp =
{
	CODE_CPULP,		// (1)
	0x01,			// (2)
	" 01",			// (3)
	"BDS1-BEA1",	// (4)
	"FC9543BEA1",	// (5)
	"WOTRC6AJAA",	// (6)
	"YY.MM",		// (7)
	"01",			// (8)
	"#####",		// (9)
	0x00, 0x00, 0x00,	/* rsv1-3 */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (10) */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (11) */
	0x0001,				/* (12) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/* rsv4-5 */
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 0x00,
	" ##A ",			/* (13) */
	{0x00,0x00,0x00}, {0x00,0x00,0x00}, 0x00,			/* rsv6-8 */
	{{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},	/* (14) */
	 {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00}},
	"        ",		/* (15) */
	"        ",		/* (16) */
	"      ",		/* (17) */
	0x00,			/* rsv9 */
	0x01,			/* (18) */
	0x00,			/* (19) */
	0x00,			/* (20) */
	0x0F,			/* (21) */
	0x05,			/* (22) */
	0x14,			/* (23) */
	{0x03,0xC8,0x03,0x9D,0x03,0x38,0x02,0xF6,0x00,0x00},	/* (24) */
	{0x03,0x04,0x05,0x06,0x07,0x00},						/* (25) */
	0x02,			/* (26) */
	0x14,			/* (27) */
	0x14,			/* (28) */
	0x14,			/* (29) */
	0x14,			/* (30) */
	0x1E,			/* (31) */
	0x3F,			/* (32) */
	0x3C,			/* (33) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	/* rsv_10 */
	0x0000,			// (34)
	{0x00,0x00},	// rsv11
	0x0000,			// (35)
	0x0000,			// (36)
	0x0000,			// (37)
	0x0000,			// (38)
	0x0000,			// (39)
	0x0000,			// (40)
	0x0000,			// (41)
	0x0000,			// (42)
	0x0000,			// (43)
	"000.0",		// (44)
	"000.0",		// (45)
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// rsv12
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	0x0000			// (46)
};

struct inventory_info_t pi_default_cputp =
{
	CODE_CPUTP,		// (1)
	0x01,			// (2)
	" 01",			// (3)
	"BDS1-BEA1",	// (4)
	"FC9543BEA1",	// (5)
	"WOTRC6AJAA",	// (6)
	"YY.MM",		// (7)
	"01",			// (8)
	"#####",		// (9)
	0x00, 0x00, 0x00,	/* rsv1-3 */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (10) */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (11) */
	0x0001,				/* (12) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/* rsv4-5 */
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 0x00,
	" ##A ",			/* (13) */
	{0x00,0x00,0x00}, {0x00,0x00,0x00}, 0x00,			/* rsv6-8 */
	{{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},	/* (14) */
	 {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00}},
	"        ",		/* (15) */
	"        ",		/* (16) */
	"      ",		/* (17) */
	0x00,			/* rsv9 */
	0x01,			/* (18) */
	0x00,			/* (19) */
	0x00,			/* (20) */
	0x0F,			/* (21) */
	0x05,			/* (22) */
	0x14,			/* (23) */
	{0x03,0xC8,0x03,0x9D,0x03,0x38,0x02,0xF6,0x00,0x00},	/* (24) */
	{0x03,0x04,0x05,0x06,0x07,0x00},						/* (25) */
	0x02,			/* (26) */
	0x14,			/* (27) */
	0x14,			/* (28) */
	0x14,			/* (29) */
	0x14,			/* (30) */
	0x1E,			/* (31) */
	0x3F,			/* (32) */
	0x3C,			/* (33) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	/* rsv_10 */
	0x0000,			// (34)
	{0x00,0x00},	// rsv11
	0x0000,			// (35)
	0x0000,			// (36)
	0x0000,			// (37)
	0x0000,			// (38)
	0x0000,			// (39)
	0x0000,			// (40)
	0x0000,			// (41)
	0x0000,			// (42)
	0x0000,			// (43)
	"000.0",		// (44)
	"000.0",		// (45)
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// rsv12
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	0x0000			// (46)
};

struct inventory_info_t pi_default_cpusp =
{
	CODE_CPUSP,		// (1)
	0x01,			// (2)
	" 01",			// (3)
	"BDS1-BEA1",	// (4)
	"FC9543BEA1",	// (5)
	"WOTRC6AJAA",	// (6)
	"YY.MM",		// (7)
	"01",			// (8)
	"#####",		// (9)
	0x00, 0x00, 0x00,	/* rsv1-3 */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (10) */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (11) */
	0x0001,				/* (12) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/* rsv4-5 */
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 0x00,
	" ##A ",			/* (13) */
	{0x00,0x00,0x00}, {0x00,0x00,0x00}, 0x00,			/* rsv6-8 */
	{{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},	/* (14) */
	 {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00}},
	"        ",		/* (15) */
	"        ",		/* (16) */
	"      ",		/* (17) */
	0x00,			/* rsv9 */
	0x01,			/* (18) */
	0x00,			/* (19) */
	0x00,			/* (20) */
	0x0F,			/* (21) */
	0x05,			/* (22) */
	0x14,			/* (23) */
	{0x03,0xC8,0x03,0x9D,0x03,0x38,0x02,0xF6,0x00,0x00},	/* (24) */
	{0x03,0x04,0x05,0x06,0x07,0x00},						/* (25) */
	0x02,			/* (26) */
	0x14,			/* (27) */
	0x14,			/* (28) */
	0x14,			/* (29) */
	0x14,			/* (30) */
	0x1E,			/* (31) */
	0x3F,			/* (32) */
	0x3C,			/* (33) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	/* rsv_10 */
	0x0000,			// (34)
	{0x00,0x00},	// rsv11
	0x0000,			// (35)
	0x0000,			// (36)
	0x0000,			// (37)
	0x0000,			// (38)
	0x0000,			// (39)
	0x0000,			// (40)
	0x0000,			// (41)
	0x0000,			// (42)
	0x0000,			// (43)
	"000.0",		// (44)
	"000.0",		// (45)
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// rsv12
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	0x0000			// (46)
};

struct inventory_info_t pi_default_fpsie =
{
	CODE_FPSIE,		// (1)
	0x01,			// (2)
	" 01",			// (3)
	"BDS1-BEA1",	// (4)
	"FC9543BEA1",	// (5)
	"WOTRC6AJAA",	// (6)
	"YY.MM",		// (7)
	"01",			// (8)
	"#####",		// (9)
	0x00, 0x00, 0x00,	/* rsv1-3 */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (10) */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (11) */
	0x0001,				/* (12) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/* rsv4-5 */
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 0x00,
	" ##A ",			/* (13) */
	{0x00,0x00,0x00}, {0x00,0x00,0x00}, 0x00,			/* rsv6-8 */
	{{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},	/* (14) */
	 {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00}},
	"        ",		/* (15) */
	"        ",		/* (16) */
	"      ",		/* (17) */
	0x00,			/* rsv9 */
	0x01,			/* (18) */
	0x00,			/* (19) */
	0x00,			/* (20) */
	0x0F,			/* (21) */
	0x05,			/* (22) */
	0x14,			/* (23) */
	{0x03,0xC8,0x03,0x9D,0x03,0x38,0x02,0xF6,0x00,0x00},	/* (24) */
	{0x03,0x04,0x05,0x06,0x07,0x00},						/* (25) */
	0x02,			/* (26) */
	0x14,			/* (27) */
	0x14,			/* (28) */
	0x14,			/* (29) */
	0x14,			/* (30) */
	0x1E,			/* (31) */
	0x3F,			/* (32) */
	0x3C,			/* (33) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	/* rsv_10 */
	0x0000,			// (34)
	{0x00,0x00},	// rsv11
	0x0000,			// (35)
	0x0000,			// (36)
	0x0000,			// (37)
	0x0000,			// (38)
	0x0000,			// (39)
	0x0000,			// (40)
	0x0000,			// (41)
	0x0000,			// (42)
	0x0000,			// (43)
	"000.0",		// (44)
	"000.0",		// (45)
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// rsv12
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	0x0000			// (46)
};

struct inventory_info_t pi_default_fpulp =
{
	CODE_FPULP,		// (1)
	0x01,			// (2)
	" 01",			// (3)
	"BDS1-BEA1",	// (4)
	"FC9543BEA1",	// (5)
	"WOTRC6AJAA",	// (6)
	"YY.MM",		// (7)
	"01",			// (8)
	"#####",		// (9)
	0x00, 0x00, 0x00,	/* rsv1-3 */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (10) */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (11) */
	0x0001,				/* (12) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/* rsv4-5 */
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 0x00,
	" ##A ",			/* (13) */
	{0x00,0x00,0x00}, {0x00,0x00,0x00}, 0x00,			/* rsv6-8 */
	{{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},	/* (14) */
	 {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00}},
	"        ",		/* (15) */
	"        ",		/* (16) */
	"      ",		/* (17) */
	0x00,			/* rsv9 */
	0x01,			/* (18) */
	0x00,			/* (19) */
	0x00,			/* (20) */
	0x0F,			/* (21) */
	0x05,			/* (22) */
	0x14,			/* (23) */
	{0x03,0xC8,0x03,0x9D,0x03,0x38,0x02,0xF6,0x00,0x00},	/* (24) */
	{0x03,0x04,0x05,0x06,0x07,0x00},						/* (25) */
	0x02,			/* (26) */
	0x14,			/* (27) */
	0x14,			/* (28) */
	0x14,			/* (29) */
	0x14,			/* (30) */
	0x1E,			/* (31) */
	0x3F,			/* (32) */
	0x3C,			/* (33) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	/* rsv_10 */
	0x0000,			// (34)
	{0x00,0x00},	// rsv11
	0x0000,			// (35)
	0x0000,			// (36)
	0x0000,			// (37)
	0x0000,			// (38)
	0x0000,			// (39)
	0x0000,			// (40)
	0x0000,			// (41)
	0x0000,			// (42)
	0x0000,			// (43)
	"000.0",		// (44)
	"000.0",		// (45)
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// rsv12
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	0x0000			// (46)
};

struct inventory_info_t pi_default_fputp =
{
	CODE_FPUTP,		// (1)
	0x01,			// (2)
	" 01",			// (3)
	"BDS1-BEA1",	// (4)
	"FC9543BEA1",	// (5)
	"WOTRC6AJAA",	// (6)
	"YY.MM",		// (7)
	"01",			// (8)
	"#####",		// (9)
	0x00, 0x00, 0x00,	/* rsv1-3 */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (10) */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (11) */
	0x0001,				/* (12) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/* rsv4-5 */
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 0x00,
	" ##A ",			/* (13) */
	{0x00,0x00,0x00}, {0x00,0x00,0x00}, 0x00,			/* rsv6-8 */
	{{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},	/* (14) */
	 {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00}},
	"        ",		/* (15) */
	"        ",		/* (16) */
	"      ",		/* (17) */
	0x00,			/* rsv9 */
	0x01,			/* (18) */
	0x00,			/* (19) */
	0x00,			/* (20) */
	0x0F,			/* (21) */
	0x05,			/* (22) */
	0x14,			/* (23) */
	{0x03,0xC8,0x03,0x9D,0x03,0x38,0x02,0xF6,0x00,0x00},	/* (24) */
	{0x03,0x04,0x05,0x06,0x07,0x00},						/* (25) */
	0x02,			/* (26) */
	0x14,			/* (27) */
	0x14,			/* (28) */
	0x14,			/* (29) */
	0x14,			/* (30) */
	0x1E,			/* (31) */
	0x3F,			/* (32) */
	0x3C,			/* (33) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	/* rsv_10 */
	0x0000,			// (34)
	{0x00,0x00},	// rsv11
	0x0000,			// (35)
	0x0000,			// (36)
	0x0000,			// (37)
	0x0000,			// (38)
	0x0000,			// (39)
	0x0000,			// (40)
	0x0000,			// (41)
	0x0000,			// (42)
	0x0000,			// (43)
	"000.0",		// (44)
	"000.0",		// (45)
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// rsv12
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	0x0000			// (46)
};

struct inventory_info_t pi_default_fpusp =
{
	CODE_FPUSP,		// (1)
	0x01,			// (2)
	" 01",			// (3)
	"BDS1-BEA1",	// (4)
	"FC9543BEA1",	// (5)
	"WOTRC6AJAA",	// (6)
	"YY.MM",		// (7)
	"01",			// (8)
	"#####",		// (9)
	0x00, 0x00, 0x00,	/* rsv1-3 */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (10) */
	{0x00,0x00,0x00,0x00,0x00,0x00},	/* (11) */
	0x0001,				/* (12) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	/* rsv4-5 */
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, 0x00,
	" ##A ",			/* (13) */
	{0x00,0x00,0x00}, {0x00,0x00,0x00}, 0x00,			/* rsv6-8 */
	{{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},	/* (14) */
	 {0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00}},
	"        ",		/* (15) */
	"        ",		/* (16) */
	"      ",		/* (17) */
	0x00,			/* rsv9 */
	0x01,			/* (18) */
	0x00,			/* (19) */
	0x00,			/* (20) */
	0x0F,			/* (21) */
	0x05,			/* (22) */
	0x14,			/* (23) */
	{0x03,0xC8,0x03,0x9D,0x03,0x38,0x02,0xF6,0x00,0x00},	/* (24) */
	{0x03,0x04,0x05,0x06,0x07,0x00},						/* (25) */
	0x02,			/* (26) */
	0x14,			/* (27) */
	0x14,			/* (28) */
	0x14,			/* (29) */
	0x14,			/* (30) */
	0x1E,			/* (31) */
	0x3F,			/* (32) */
	0x3C,			/* (33) */
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	/* rsv_10 */
	0x0000,			// (34)
	{0x00,0x00},	// rsv11
	0x0000,			// (35)
	0x0000,			// (36)
	0x0000,			// (37)
	0x0000,			// (38)
	0x0000,			// (39)
	0x0000,			// (40)
	0x0000,			// (41)
	0x0000,			// (42)
	0x0000,			// (43)
	"000.0",		// (44)
	"000.0",		// (45)
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	// rsv12
	 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	0x0000			// (46)
};

unsigned char pi_param_size[] = {
	0,
	2,	/*                                                   (1)  unit code							*/
	1,	/*                                                   (2)  version flag						*/
	3,	/*                                                   (3)  issue number						*/
	9,	/*                                                   (4)  Abbreviation Name					*/
	10,	/*                                                   (5)  FC number							*/
	10,	/*                                                   (6)  CLEI code							*/
	5,	/*                                                   (7)  Product Year and Month			*/
	2,	/*                                                   (8)  2D label location					*/
	5,	/*                                                   (9)  serial number						*/
	6,	/*                                                   (10) MAC Address Block Max				*/
	6,	/*                                                   (11) MAC Address Block Minimum			*/
	2,	/*                                                   (12) FWDL type. FPGA Download Issue	*/
	5,	/*                                                   (13) PCB Revision						*/
	1,	/*                                                   (14) Parts Temp. Threshold				*/
	8,	/*                                                   (15) Min. Wavelength					*/
	8,	/*                                                   (16) Min. Wavelength					*/
	6,	/*                                                   (17) Frequency Spacing					*/
	1,	/*                                                   (18) Pressure Sensor Existence			*/
	1,	/*                                                   (19) Min. FAN Step						*/
	1,	/*                                                   (20) Max. FAN Step						*/
	1,	/*                                                   (21) Min.Temperature Threshold			*/
	1,	/*                                                   (22) Ambient Temperature pitch			*/
	1,	/*                                                   (23) FAN Control Interval				*/
	10,	/*                                                   (24) Min. Pressure Threshold			*/
	6,	/*                                                   (25) Initial FAN Step					*/
	1,	/*                                                   (26) Ambient Temperature Hysteresis	*/
	1,	/*                                                   (27) Ambient Pressure Hysteresis		*/
	1,	/*                                                   (28) Break Temp Over Protection		*/
	1,	/*                                                   (29) Shelf Temp Alert Protection		*/
	1,	/*                                                   (30) Shelf Temp ALM Protection			*/
	1,	/*                                                   (31) Cooling Fail Protection			*/
	1,	/*                                                   (32) Cooling Fail Clear Protection		*/
	1,	/*                                                   (33) Cooling Fail Mask time			*/
	2,	/*                                                   (34) WarmUp Timer						*/
	2,	/*                                                   (35) PD1 Initial Value					*/
	2,	/*                                                   (36) PD2 Initial Value					*/
	2,	/*                                                   (37) Max ABS Threshold					*/
	2,	/*                                                   (38) VOA Adjustment Fixed Val			*/
	2,	/*                                                   (39) VOA Max Setting Value				*/
	2,	/*                                                   (40) VOA Min Setting Value				*/
	2,	/*                                                   (41) Loop Target Level					*/
	2,	/*                                                   (42) Loop Target Threshold+			*/
	2,	/*                                                   (43) Loop Target Threshold-			*/
	5,	/*                                                   (44) MAX Power Drawn					*/
	5,	/*                                                   (45) Low Power Level					*/
	2	/*                                                   (46) Checksum							*/
};


// P.Iパラメータチェック用規定値
char pi_chek_val_1[2][2] = {
	{'0', '1'},
	{'0', '3'}
};

/* messages */
#define CMD_MAXCOL_SZ			256
#define LINEMSG_SZ				CMD_MAXCOL_SZ
#define CMD_MAXROW_SZ			20		//20 row
#define KEY_LEFT				'<'
#define KEY_UP					'+'		//^[[A
#define KEY_DOWN				'*'		//^[[B
#define KEY_RIGHT				'>'
#define KEY_BACKSPACE			'\b'
#define KEY_SPACE				' '
#define KEY_ENTER				'\r'
#define KEY_ESC					0x1b	//^[
#define backSpace()				printf("%c",KEY_BACKSPACE);	\
								printf("%c",KEY_SPACE);		\
								printf("%c",KEY_BACKSPACE)

#define PRT_MSG_DEFAULT_VALUE		printf("P.I EEPROM is All 0xF. So display default value\n")
#define PRT_MSG_HARDWARE_VALUE(a)	printf("P.I EEPROM has value(%d SIDE). So display hardware value\n", a+1)

#endif /* __CMD_INVENTORY_H__ */ 

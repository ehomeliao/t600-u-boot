#define FPGA_CONFIG_RAM_START_ADR	0x1000000
#define WAIT_30MS							30000
#define RET_SUCCESS						0
#define RET_RERY							-1
#define RET_FAULT							-2
#define RET_ERROR							-3

#define CDEC_CPLD_BASE               0xc0000000
#define CDEC_CPLD_FPGA_CNF_BASE      (CDEC_CPLD_BASE + 0xD00000)

#define FPGA_CNF_SET(i)			(*(volatile unsigned long int *)(CDEC_CPLD_FPGA_CNF_BASE + 0x1000*(i-1) + 0x00))
#define FPGA_CNF_START(i)		(*(volatile unsigned long int *)(CDEC_CPLD_FPGA_CNF_BASE + 0x1000*(i-1) + 0x10))
#define FPGA_CNF_END(i)			(*(volatile unsigned long int *)(CDEC_CPLD_FPGA_CNF_BASE + 0x1000*(i-1) + 0x14))
#define FPGA_CNF_ST(i)			(*(volatile unsigned long int *)(CDEC_CPLD_FPGA_CNF_BASE + 0x1000*(i-1) + 0x18))
#define FPGA_CNF_MON(i)			(*(volatile unsigned long int *)(CDEC_CPLD_FPGA_CNF_BASE + 0x1000*(i-1) + 0x1C))
#define FPGA_CNF_DATA(i)		(*(volatile unsigned long int *)(CDEC_CPLD_FPGA_CNF_BASE + 0x1000*(i-1) + 0x30))
#define CLK_DWN_EQP				(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0x000060))
#define CLK_DWN_EQP_MSK			(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0x00006c))
#define WDT_ALM_MSK				(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0x000090))
#define WDT_ALM_STATUS			(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0x00009c))
#define SEVERITY_LED				(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0x000304))
#define SEVERITY_LED_OFF		0x00000000
#define SEVERITY_RED_ON			0x00000001
#define SEVERITY_RED_BLINNK_L	0x00000002
#define SEVERITY_RED_BLINNK_H	0x00000004
#define SEVERITY_YEL_ON			0x00000100
#define SEVERITY_YEL_BLINNK_L	0x00000200
#define SEVERITY_YEL_BLINNK_H	0x00000400
#define SYS_LED					(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0x00030c))
#define SYS_RED_OFF				0x00000000
#define SYS_RED_ON				0x00000001
#define SYS_RED_BLINK_L			0x00000002
#define SYS_RED_BLINK_H			0x00000004
#define SYS_GREEN_ON				0x00000010
#define SYS_GREEN_BLINK_L		0x00000020
#define SYS_GREEN_BLINK_H		0x00000040
#define SYS_YEL_ON				0x00000100
#define SYS_YEL_BLINK_L			0x00000200
#define SYS_YEL_BLINK_H			0x00000400
#define MBCNT_RESET				(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0xa00034))
#define PIU_RESET					(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0xa00038))
#define PIU_PWR_CTRL				(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0xa00400))
#define PIU_ALM_FORCE			(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0xa0051c))
#define PIU_ALM_MASK				(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0xa0053c))
#define PIU_PWR_CTRL				(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0xa00400))
#define TX_DIS						(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0x1900004))

#define MBCNT_VENDER_ID			0x10cf
#define MBCNT_DEVICE_ID			0x0021

#define msleep(a)    udelay(a * 1000)

int fpga_config_main(unsigned int fpga, unsigned int mode,unsigned int size);
int cdec_cpld_init(void);

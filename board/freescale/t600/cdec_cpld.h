
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
#define MBCNT_RESET				(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0xa00034))
#define PIU_RESET					(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0xa00038))
#define PIU_PWR_CTRL				(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0xa00400))
#define TX_DIS						(*(volatile unsigned long int *)(CDEC_CPLD_BASE + 0x1900004))

#define msleep(a)    udelay(a * 1000)

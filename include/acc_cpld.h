#ifndef __ACC_CPLD_H__
#define __ACC_CPLD_H__

#define ACC_CPLD_BUS		0
#define ACC_CPLD_ADDR	0x60

#define REG_CPU_BOARD_VERSION	0x1
#define CPU_BOARD_REV_MASK	0x7

#define REG_CPLD_VERSION	0x2

#define REG_RESET_CONTROL3	0x7
#define FJ_CDEC_PCIE_RESET		( 1 << 5)

#define REG_SYS_STATUS	0x11
#define EEPROM1_WP				( 1 << 0)
#define EEPROM2_WP				( 1 << 1)
#define PCA9559_WP				( 1 << 2)
#define NOR_FLASH_DISABLE_WP	( 1 << 3)


#define REG_MGMT_STATUS	0x15
#define BCM5389_BOOT_SPI		( 1 << 0)
#define BCM5389_SPI_CS			( 1 << 1)

#define REG_MGMT_RESET	0x17
#define SFP_IXEXP_RESET			( 1 << 0)
#define BCM5389_RESET			( 1 << 1)

#define REG_SYS_BOOTUP_STATUS2	0x25
#define WARM_BOOT_RECORD		( 1 << 1)

#define REG_SYS_STATUS2	0x27
#define MBCNT_LOADED				( 1 << 0)

int get_acc_cpu_board_version(uint8_t *version);
int get_acc_cpld_version(uint8_t *version);
int fj_cdec_pcie_reset(int reset);
int bcm5389_reset(int reset);
int setup_bcm5389(void);
int acc_eeprom_wp(int primary, int enable);
int nor_flash_wp(int enable);
int is_from_warm_boot(void);
int mbcnt_loaded(int load);
int is_mbcnt_loaded(void);

#endif /* __ACC_CPLD_H__ */

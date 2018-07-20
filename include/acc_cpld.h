#ifndef __ACC_CPLD_H__
#define __ACC_CPLD_H__

#define ACC_CPLD_BUS		0
#define ACC_CPLD_ADDR	0x60

#define REG_SYS_STATUS	0x11
#define EEPROM1_WP				( 1 << 0)
#define EEPROM2_WP				( 1 << 1)
#define PCA9559_WP				( 1 << 2)
#define NOR_FLASH_DISABLE_WP	( 1 << 3)


#define REG_MGMT_STATUS	0x15
#define BCM5389_BOOT_SPI		( 1 << 0)
#define BCM5389_SPI_CS			( 1 << 1)

int setup_bcm5389(void);
int acc_eeprom_wp(int primary, int enable);
int nor_flash_wp(int enable);

#endif /* __ACC_CPLD_H__ */

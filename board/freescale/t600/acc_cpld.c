#include <common.h>
#include <command.h>
#include <i2c.h>
#include <acc_cpld.h>

static int acc_cpld_write(uint8_t offset, uint8_t value)
{
	int ret = -1;
	if (i2c_set_bus_num(ACC_CPLD_BUS) == 0) {
		if (i2c_write(ACC_CPLD_ADDR, offset, 1, &value, 1) == 0) {
			ret = 0;
		}
	}

	return ret;
}

static int acc_cpld_read(uint8_t offset, uint8_t *value)
{
	int ret = -1;
	if (i2c_set_bus_num(ACC_CPLD_BUS) == 0) {
		if (i2c_read(ACC_CPLD_ADDR, offset, 1, value, 1) == 0) {
			ret = 0;
		}
	}

	return ret;
}


static int acc_cold_write_mask(uint8_t offset, uint8_t mask, uint8_t value)
{
	int ret = -1;
	uint8_t update;

	if (acc_cpld_read(offset, &update) == 0) {
		update &= ~mask;
		update |= (mask & value);
		if (acc_cpld_write(offset, update) == 0) {
			ret = 0;
		}
	}

	return ret;
}

int setup_bcm5389(void)
{
	return acc_cold_write_mask(REG_MGMT_STATUS, BCM5389_BOOT_SPI | BCM5389_SPI_CS, BCM5389_BOOT_SPI | BCM5389_SPI_CS);
}

int acc_eeprom_wp(int primary, int enable)
{
	uint8_t mask, value;

	if (primary) {
		mask = EEPROM1_WP;
	} else {
		mask = EEPROM2_WP;
	}

	if (enable) {
		value = mask;
	} else {
		value = 0;
	}

	return acc_cold_write_mask(REG_SYS_STATUS, EEPROM1_WP, value);
}

int nor_flash_wp(int enable)
{
	uint8_t value;

	if (enable) {
		value = 0;
	} else {
		value = NOR_FLASH_DISABLE_WP;
	}

	return acc_cold_write_mask(REG_SYS_STATUS, NOR_FLASH_DISABLE_WP, value);
}

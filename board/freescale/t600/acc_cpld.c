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

int get_acc_cpu_board_version(uint8_t *version)
{
	int ret = -1;

	if (acc_cpld_read(REG_CPU_BOARD_VERSION, version) == 0) {
		*version = (*version & CPU_BOARD_REV_MASK);
		ret = 0;
	}

	return ret;

}

int get_acc_cpld_version(uint8_t *version)
{
	int ret = -1;

	if (acc_cpld_read(REG_CPLD_VERSION, version) == 0) {
		*version = (*version & 0x0f);
		ret = 0;
	}

	return ret;

}

int fj_cdec_pcie_reset(int reset)
{
	int ret = -1;

	if (reset == 0) {
		ret = acc_cold_write_mask(REG_RESET_CONTROL3, FJ_CDEC_PCIE_RESET, FJ_CDEC_PCIE_RESET);
	} else {
		ret = acc_cold_write_mask(REG_RESET_CONTROL3, FJ_CDEC_PCIE_RESET, 0);
	}

	return ret;
}

int bcm5389_reset(int reset)
{
	int ret = -1;

	if (reset == 0) {
		ret = acc_cold_write_mask(REG_MGMT_RESET, BCM5389_RESET, BCM5389_RESET);
	} else {
		ret = acc_cold_write_mask(REG_MGMT_RESET, BCM5389_RESET, 0);
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

int is_from_warm_boot(void)
{
	uint8_t status;
	int ret = 0;
	
	if (acc_cpld_read(REG_SYS_BOOTUP_STATUS2, &status) == 0) {
		if (status & WARM_BOOT_RECORD) {
			ret = 1;
		}
	}

	return ret;
}


int mbcnt_loaded(int load)
{
	uint8_t value;

	if (load) {
		value = MBCNT_LOADED;
	} else {
		value = 0;
	}

	return acc_cold_write_mask(REG_SYS_STATUS2, MBCNT_LOADED, value);
}


int is_mbcnt_loaded(void) {
	uint8_t loaded;
	int ret = 0;
	
	if (acc_cpld_read(REG_SYS_STATUS2, &loaded) == 0) {
		if (loaded & MBCNT_LOADED) {
			ret = 1;
		}
	}

	return ret;
}



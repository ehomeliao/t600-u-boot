#include <common.h>
#include <command.h>
#include <acc_cpld.h>

static int do_nor_protect(cmd_tbl_t *cmdtp, int flag, int argc,
			 char * const argv[])
{
	if (strcmp (argv[1], "enable") == 0) {
		return nor_flash_wp(1);
	} else if (strcmp (argv[1], "disable") == 0) {
		return nor_flash_wp(0);
	} else {
		return CMD_RET_USAGE;
	}
}


U_BOOT_CMD(
	norprotect,	3,	1,	do_nor_protect,
	"enable/disable NOR protection by setting CPLD",
	"{ enable | disable }\n"
);

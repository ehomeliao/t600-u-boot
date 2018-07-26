#include <common.h>
#include <command.h>
#include <pci.h>
#include <acc_cpld.h>

static int do_rom_version(cmd_tbl_t *cmdtp, int flag, int argc,
			 char * const argv[])
{
	uint8_t cpld_version;
	uint32_t fpga_version;
	ulong addr;
	pci_dev_t dev;
	const void *buf;

	if (get_acc_cpld_version(&cpld_version) == 0) {
		printf("Accton CPLD: 0x%x\n", cpld_version);
	} else {
		printf("Accton CPLD: N/A\n");
	}

   dev = pci_find_device (0x10cf, 0x0000, 0);
	if (dev != -1) {
		addr = (uint32_t)pci_map_bar(dev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM);
		buf = map_sysmem(addr, 4);
		fpga_version = *(volatile uint32_t *)buf;
		unmap_sysmem(buf);
		printf("CDEC       : 0x%8x\n", fpga_version);
	} else {
		printf("CDEC       : N/A\n");
	}

   dev = pci_find_device (0x10cf, 0x0002, 0);
	if (dev != -1) {
		addr = (uint32_t)pci_map_bar(dev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM);
		buf = map_sysmem(addr, 4);
		fpga_version = *(volatile uint32_t *)buf;
		unmap_sysmem(buf);
		if (PCI_BUS(dev) == 5) {
			printf("MDEC1      : 0x%8x\n", fpga_version);
			/* keep search MDEC2 */
   		dev = pci_find_device (0x10cf, 0x0002, 1);
			if (dev != -1) {
				addr = (uint32_t)pci_map_bar(dev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM);
				buf = map_sysmem(addr, 4);
				fpga_version = *(volatile uint32_t *)buf;
				unmap_sysmem(buf);
				printf("MDEC2      : 0x%8x\n", fpga_version);
			} else {
				printf("MDEC2      : N/A\n");
			}
		} else {
			printf("MDEC1      : N/A\n");
			printf("MDEC2      : 0x%8x\n", fpga_version);
		}
	} else {
		printf("MDEC1      : N/A\n");
		printf("MDEC2      : N/A\n");
	}


   dev = pci_find_device (0x10cf, 0x0021, 0);
	if (dev != -1) {
		addr = (uint32_t)pci_map_bar(dev, PCI_BASE_ADDRESS_0, PCI_REGION_MEM);
		buf = map_sysmem(addr, 4);
		fpga_version = *(volatile uint32_t *)buf;
		unmap_sysmem(buf);
		printf("MBCNT      : 0x%8x\n", fpga_version);
	} else {
		printf("MBCNT      : N/A\n");
	}

	return 0;
}


U_BOOT_CMD(
	romver,	1,	1,	do_rom_version,
	"Show version of FPGA",
	"N/A\n"
);

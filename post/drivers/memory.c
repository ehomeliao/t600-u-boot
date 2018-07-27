/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/* FUJITSU LIMITED
 * $Date:: 2016-09-12 00:00:00 +0900#$
*/

#include <common.h>

/* Memory test
 *
 * General observations:
 * o The recommended test sequence is to test the data lines: if they are
 *   broken, nothing else will work properly.  Then test the address
 *   lines.  Finally, test the cells in the memory now that the test
 *   program knows that the address and data lines work properly.
 *   This sequence also helps isolate and identify what is faulty.
 *
 * o For the address line test, it is a good idea to use the base
 *   address of the lowest memory location, which causes a '1' bit to
 *   walk through a field of zeros on the address lines and the highest
 *   memory location, which causes a '0' bit to walk through a field of
 *   '1's on the address line.
 *
 * o Floating buses can fool memory tests if the test routine writes
 *   a value and then reads it back immediately.  The problem is, the
 *   write will charge the residual capacitance on the data bus so the
 *   bus retains its state briefely.  When the test program reads the
 *   value back immediately, the capacitance of the bus can allow it
 *   to read back what was written, even though the memory circuitry
 *   is broken.  To avoid this, the test program should write a test
 *   pattern to the target location, write a different pattern elsewhere
 *   to charge the residual capacitance in a differnt manner, then read
 *   the target location back.
 *
 * o Always read the target location EXACTLY ONCE and save it in a local
 *   variable.  The problem with reading the target location more than
 *   once is that the second and subsequent reads may work properly,
 *   resulting in a failed test that tells the poor technician that
 *   "Memory error at 00000000, wrote aaaaaaaa, read aaaaaaaa" which
 *   doesn't help him one bit and causes puzzled phone calls.  Been there,
 *   done that.
 *
 * Data line test:
 * ---------------
 * This tests data lines for shorts and opens by forcing adjacent data
 * to opposite states. Because the data lines could be routed in an
 * arbitrary manner the must ensure test patterns ensure that every case
 * is tested. By using the following series of binary patterns every
 * combination of adjacent bits is test regardless of routing.
 *
 *     ...101010101010101010101010
 *     ...110011001100110011001100
 *     ...111100001111000011110000
 *     ...111111110000000011111111
 *
 * Carrying this out, gives us six hex patterns as follows:
 *
 *     0xaaaaaaaaaaaaaaaa
 *     0xcccccccccccccccc
 *     0xf0f0f0f0f0f0f0f0
 *     0xff00ff00ff00ff00
 *     0xffff0000ffff0000
 *     0xffffffff00000000
 *
 * To test for short and opens to other signals on our boards, we
 * simply test with the 1's complemnt of the paterns as well, resulting
 * in twelve patterns total.
 *
 * After writing a test pattern. a special pattern 0x0123456789ABCDEF is
 * written to a different address in case the data lines are floating.
 * Thus, if a byte lane fails, you will see part of the special
 * pattern in that byte lane when the test runs.  For example, if the
 * xx__xxxxxxxxxxxx byte line fails, you will see aa23aaaaaaaaaaaa
 * (for the 'a' test pattern).
 *
 * Address line test:
 * ------------------
 *  This function performs a test to verify that all the address lines
 *  hooked up to the RAM work properly.  If there is an address line
 *  fault, it usually shows up as two different locations in the address
 *  map (related by the faulty address line) mapping to one physical
 *  memory storage location.  The artifact that shows up is writing to
 *  the first location "changes" the second location.
 *
 * To test all address lines, we start with the given base address and
 * xor the address with a '1' bit to flip one address line.  For each
 * test, we shift the '1' bit left to test the next address line.
 *
 * In the actual code, we start with address sizeof(ulong) since our
 * test pattern we use is a ulong and thus, if we tried to test lower
 * order address bits, it wouldn't work because our pattern would
 * overwrite itself.
 *
 * Example for a 4 bit address space with the base at 0000:
 *   0000 <- base
 *   0001 <- test 1
 *   0010 <- test 2
 *   0100 <- test 3
 *   1000 <- test 4
 * Example for a 4 bit address space with the base at 0010:
 *   0010 <- base
 *   0011 <- test 1
 *   0000 <- (below the base address, skipped)
 *   0110 <- test 2
 *   1010 <- test 3
 *
 * The test locations are successively tested to make sure that they are
 * not "mirrored" onto the base address due to a faulty address line.
 * Note that the base and each test location are related by one address
 * line flipped.  Note that the base address need not be all zeros.
 *
 * Memory tests 1-4:
 * -----------------
 * These tests verify RAM using sequential writes and reads
 * to/from RAM. There are several test cases that use different patterns to
 * verify RAM. Each test case fills a region of RAM with one pattern and
 * then reads the region back and compares its contents with the pattern.
 * The following patterns are used:
 *
 *  1a) zero pattern (0x00000000)
 *  1b) negative pattern (0xffffffff)
 *  1c) checkerboard pattern (0x55555555)
 *  1d) checkerboard pattern (0xaaaaaaaa)
 *  2)  bit-flip pattern ((1 << (offset % 32))
 *  3)  address pattern (offset)
 *  4)  address pattern (~offset)
 *
 * Being run in normal mode, the test verifies only small 4Kb
 * regions of RAM around each 1Mb boundary. For example, for 64Mb
 * RAM the following areas are verified: 0x00000000-0x00000800,
 * 0x000ff800-0x00100800, 0x001ff800-0x00200800, ..., 0x03fff800-
 * 0x04000000. If the test is run in slow-test mode, it verifies
 * the whole RAM.
 */

#include <post.h>
#include <watchdog.h>
#if defined(CONFIG_T600)
#include <fsl_ddr_sdram.h>
#endif

#if CONFIG_POST & (CONFIG_SYS_POST_MEMORY | CONFIG_SYS_POST_MEM_REGIONS)

DECLARE_GLOBAL_DATA_PTR;

/*
 * Define INJECT_*_ERRORS for testing error detection in the presence of
 * _good_ hardware.
 */
#undef  INJECT_DATA_ERRORS
#undef  INJECT_ADDRESS_ERRORS

#ifdef INJECT_DATA_ERRORS
#warning "Injecting data line errors for testing purposes"
#endif

#ifdef INJECT_ADDRESS_ERRORS
#warning "Injecting address line errors for testing purposes"
#endif

#if defined(CONFIG_T600)
#define TEST_PTN_ALL0 0
#define TEST_PTN_RAN1 1
#define TEST_PTN_RAN2 2
#define TEST_PTN_ALLF 3
#define MEMCLR_OFF    0
#define MEMCLR_ON     1
#define TST_START     0
#define TST_NOW       1
#define TST_END       2

typedef struct mtp_table_t {
	unsigned long mtp1;
	unsigned long mtp2;
	unsigned long mtp3;
	unsigned long mtp4;
	unsigned long mtp5;
	unsigned long mtp6;
	unsigned long mtp7;
	unsigned long mtp8;
	unsigned long mtp9;
	unsigned long mtp10;
} mtp_table_t;


/*****************************************************************/ 
/** 
 * 
 * @brief memory_hwtest
 * @brief This function tests the memory of DDR by using P2041 CCSR.
 * 
 * @param ptn     test patn                  0:All 0/1:Randam1/2:Randam2/3:ALL F
 * @param tst_end test messag control flag   0:Test Start 1:Test Now 2:Test End
 * @param memclr  memory claer control flag  0:Not memoru clear 1:Do memory clear
 * @retval 0 Normal end
 * @retval 1 Fail end
 * 
 * @attention --
 * 
 * ------------------------------------------------------------ 
 * SPDX-License-Identifier:	GPL-2.0+
 * This program is free software: you can redistribute it and/or modify
 * it un under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * COPYRIGHT(C) FUJITSU LIMITED 2015
 */
/*****************************************************************/ 
static int memory_hwtest(unsigned int ptn,unsigned tst_end,unsigned int memclr)
{
	struct ccsr_ddr __iomem *io_ddr = (void *)CONFIG_SYS_FSL_DDR_ADDR;
	unsigned long ret = 0;
	unsigned long reg = 0;
	unsigned long reg1 = 0;
	unsigned long err_detect = 0;
	unsigned long err_sbe = 0;
	unsigned long dump_addr;
#ifdef DEBUG_T600_MEMTEST
	unsigned long dump_addr1;
#endif
	unsigned long i,j;
	unsigned long idx = ptn;
	mtp_table_t mtp_data[4] = {
		{	/* ALL 0 PTN */
			0x00000000,0x00000000,0x00000000,0x00000000,
			0x00000000,0x00000000,0x00000000,0x00000000,
			0x00000000,0x00000000
		},
		{	/* ramdam1 PTN */	
			0xa5a5a5a5,0x5a5a5a5a,0x00000000,0xffffffff,
			0x01234567,0x89abcdef,0xfedcba98,0x76543210,
			0xffffffff,0x00000000
		},
		{	/* ramdam2 PTN */	
			0x5a5a5a5a,0xa5a5a5a5,0xffffffff,0x00000000,
			0xfedcba98,0x76543210,0x01234567,0x89abcdef,
			0x00000000,0xffffffff
		},
		{	/* ALL F PTN */	
			0xffffffff,0xffffffff,0xffffffff,0xffffffff,
			0xffffffff,0xffffffff,0xffffffff,0xffffffff,
			0xffffffff,0xffffffff
		}
	};
	
#ifdef DEBUG_T600_MEMTEST
	printf("#Set D_INIT before MEM_EN\n");
	printf("#Set MEM_EN\n");
	dump_addr1 = (unsigned long)(unsigned long)&(io_ddr->sdram_cfg);
	printf("-------------------------------------------------\n");
	for (j = 0; j < 0x10; j += 16) {
		printf(" %08x", (unsigned int)dump_addr1);
		for (i = 0; i < 4; i++) {
			printf(" %08x", (unsigned int)in_be32((void *)dump_addr1));
			dump_addr1 += 4;
		}
		printf("\n");
	}
#endif

#if defined(DEBUG_T600_MEMTESTI)
	printf("DDR TEST: Hardware Test Function...\n");
#else
	if(TST_START==tst_end) printf("DDR TEST: Hardware Test Function...");
#endif

	/* Set MEM_HALT=1 and delay 1ms */
	reg1 = in_be32(&io_ddr->sdram_cfg);
	reg1 = (reg1 | 0x00000002);
	out_be32(&io_ddr->sdram_cfg, reg1);
	udelay(1000);
	asm volatile("sync;isync");
	
#ifdef DEBUG_T600_MEMTEST
	printf("#Set MEM_HALT=1 and delay 1ms \n");
#endif
	
	/* Set MTP1-10 test pattern registers */
	out_be32(&io_ddr->err_disable, 0x0);
	out_be32(&io_ddr->mtp1, mtp_data[idx].mtp1);
	out_be32(&io_ddr->mtp2, mtp_data[idx].mtp2);
	out_be32(&io_ddr->mtp3, mtp_data[idx].mtp3);
	out_be32(&io_ddr->mtp4, mtp_data[idx].mtp4);
	out_be32(&io_ddr->mtp5, mtp_data[idx].mtp5);
	out_be32(&io_ddr->mtp6, mtp_data[idx].mtp6);
	out_be32(&io_ddr->mtp7, mtp_data[idx].mtp7);
	out_be32(&io_ddr->mtp8, mtp_data[idx].mtp8);
	out_be32(&io_ddr->mtp9, mtp_data[idx].mtp9);
	out_be32(&io_ddr->mtp10, mtp_data[idx].mtp10);
	udelay(200);
	asm volatile("sync;isync");
	
#ifdef DEBUG_T600_MEMTEST
	printf("#Set MTP1-10 test pattern registers \n");
#endif

	/* Set MT_EN =1 : Start toriger */
	out_be32(&io_ddr->mtcr, 0x80000000);
	udelay(200);
	asm volatile("sync;isync");
	
#ifdef DEBUG_T600_MEMTEST
	printf("#Set MT_EN =1\n");
#endif

#ifdef DEBUG_T600_MEMTEST
	printf("sdram_cfg 0x%08x \n",(unsigned int)in_be32(&io_ddr->sdram_cfg));
	dump_addr1 = (unsigned long)(unsigned long)&(io_ddr->mtcr);
	printf("-------------------------------------------------\n");
	for (j = 0; j < 0x100; j += 16) {
		printf(" %08x", (unsigned int)dump_addr1);
		for (i = 0; i < 4; i++) {
			printf(" %08x", (unsigned int)in_be32((void *)dump_addr1));
			dump_addr1 += 4;
		}
		printf("\n");
	}
#endif

	/* Set MEM_HALT=0 and delay 1ms */
	reg1 = in_be32(&io_ddr->sdram_cfg);
	reg1 = (reg1 & 0xfffffffd);
	out_be32(&io_ddr->sdram_cfg, reg1);
	udelay(1000);
	asm volatile("sync;isync");
	
#ifdef DEBUG_T600_MEMTEST
	printf("#Set MEM_HALT=0 and delay 1ms \n");
#endif	
	
	/* Poll MT_EN to clear */
#ifdef DEBUG_T600_MEMTEST
	printf("#Start Poll MT_EN to clear \n");
#endif
	do {
		reg = in_be32(&io_ddr->mtcr);
	}while((reg >> 31) != 0);
#ifdef DEBUG_T600_MEMTEST
	printf("#End Poll MT_EN to clear \n");
#endif
	
	
#ifdef DEBUG_T600_MEMTEST
	printf("&io_ddr->mtcr[0x%08x]:0x%08x \n",(unsigned int)&io_ddr->mtcr,(unsigned int)reg);
	
	if(0 == ptn) {
		dump_addr1 = (unsigned long)0x10000000;
	} else if(1 == ptn) {
		dump_addr1 = (unsigned long)0x20000000;
	} else if(2 == ptn) {
		dump_addr1 = (unsigned long)0x30000000;
	} else if(3 == ptn) {
		dump_addr1 = (unsigned long)0x40000000;
	} else if(4 == ptn) {
		dump_addr1 = (unsigned long)0x50000000;
	} else {
		dump_addr1 = (unsigned long)0x10000000;
	}
	
	printf("-------------------------------------------------\n");
	for (j = 0; j < 0x30; j += 16) {
		printf(" %08x", (unsigned int)dump_addr1);
		for (i = 0; i < 4; i++) {
			printf(" %08x", (unsigned int)in_be32((void *)dump_addr1));
			dump_addr1 += 4;
		}
		printf("\n");
	}
#endif
	
	/* Error result Chack */
	err_detect = in_be32(&io_ddr->err_detect);
	err_sbe = in_be32(&io_ddr->err_sbe);
	if ((err_detect) || (err_sbe)) {
		printf("NG\n");
		ret = 1;
		dump_addr = (unsigned long)&(io_ddr->data_err_inject_hi);
		printf(" Address  +00      +04      +08      +0C     \n");
		printf("-------------------------------------------------\n");
		for (j = 0; j < 0x100; j += 16) {
			printf(" %08x", (unsigned int)dump_addr);
			for (i = 0; i < 4; i++) {
				printf(" %08x", (unsigned int)in_be32((void *)dump_addr));
				dump_addr += 4;
			}
			printf("\n");
		}
	} else if(reg & 0x1) {
		printf("COMPRER FULT\n");
		ret = 0;
		
		dump_addr = (unsigned long)&(io_ddr->data_err_inject_hi);
		printf(" Address  +00      +04      +08      +0C     \n");
		printf("-------------------------------------------------\n");
		for (j = 0; j < 0x100; j += 16) {
			printf(" %08x", (unsigned int)dump_addr);
			for (i = 0; i < 4; i++) {
				printf(" %08x", (unsigned int)in_be32((void *)dump_addr));
				dump_addr += 4;
			}
			printf("\n");
		}
		
		if (1 == memclr) {
			printf("DDR ALL CLEAR...\n");
			/* Set MEM_HALT=1 and delay 1ms */
			reg1 = in_be32(&io_ddr->sdram_cfg);
			reg1 = (reg1 | 0x00000002);
			out_be32(&io_ddr->sdram_cfg, reg1);
			udelay(1000);
			asm volatile("sync;isync");
			
			/* Set MTP1-10 test pattern registers */
			out_be32(&io_ddr->mtp1, 0x0);
			out_be32(&io_ddr->mtp2, 0x0);
			out_be32(&io_ddr->mtp3, 0x0);
			out_be32(&io_ddr->mtp4, 0x0);
			out_be32(&io_ddr->mtp5, 0x0);
			out_be32(&io_ddr->mtp6, 0x0);
			out_be32(&io_ddr->mtp7, 0x0);
			out_be32(&io_ddr->mtp8, 0x0);
			out_be32(&io_ddr->mtp9, 0x0);
			out_be32(&io_ddr->mtp10, 0x0);
			udelay(200);
			asm volatile("sync;isync");
			
			
			/* Set MT_EN =1 : Start toriger */
			out_be32(&io_ddr->mtcr, 0x81000000);
			udelay(200);
			asm volatile("sync;isync");
			
			/* Set MEM_HALT=0 and delay 1ms */
			reg1 = in_be32(&io_ddr->sdram_cfg);
			reg1 = (reg1 & 0xfffffffd);
			out_be32(&io_ddr->sdram_cfg, reg1);
			udelay(1000);
			asm volatile("sync;isync");
			
			do {
				reg = in_be32(&io_ddr->mtcr);
			}while((reg >> 31) != 0);
		
#if 0
			/* Error Clear */
			out_be32(&io_ddr->data_err_inject_hi, 0x0);
			out_be32(&io_ddr->data_err_inject_lo, 0x0);
			out_be32(&io_ddr->ecc_err_inject, 0x0);
			out_be32(&io_ddr->capture_data_hi, 0x0);
			out_be32(&io_ddr->capture_data_lo, 0x0);
			out_be32(&io_ddr->capture_ecc, 0x0);
			out_be32(&io_ddr->err_detect, 0x0);
			out_be32(&io_ddr->err_disable, 0x0);
			out_be32(&io_ddr->capture_attributes, 0x0);
			out_be32(&io_ddr->capture_address, 0x0);
			out_be32(&io_ddr->capture_ext_address, 0x0);
			out_be32(&io_ddr->err_sbe, 0x0);
			udelay(200);
			asm volatile("sync;isync");
#endif

			printf("OK\n");
		}
		
	} else {
		if(TST_END==tst_end) printf("OK\n");
		ret = 0;
		
		if (1 == memclr) {
			printf("DDR ALL CLEAR...");
			/* Set MEM_HALT=1 and delay 1ms */
			reg1 = in_be32(&io_ddr->sdram_cfg);
			reg1 = (reg1 | 0x00000002);
			out_be32(&io_ddr->sdram_cfg, reg1);
			udelay(1000);
			asm volatile("sync;isync");
			
			/* Set MTP1-10 test pattern registers */
			out_be32(&io_ddr->mtp1, 0x0);
			out_be32(&io_ddr->mtp2, 0x0);
			out_be32(&io_ddr->mtp3, 0x0);
			out_be32(&io_ddr->mtp4, 0x0);
			out_be32(&io_ddr->mtp5, 0x0);
			out_be32(&io_ddr->mtp6, 0x0);
			out_be32(&io_ddr->mtp7, 0x0);
			out_be32(&io_ddr->mtp8, 0x0);
			out_be32(&io_ddr->mtp9, 0x0);
			out_be32(&io_ddr->mtp10, 0x0);
			udelay(200);
			asm volatile("sync;isync");
			
			/* Set MT_EN =1 : Start toriger */
			out_be32(&io_ddr->mtcr, 0x81000000);
			udelay(200);
			asm volatile("sync;isync");
			
			/* Set MEM_HALT=0 and delay 1ms */
			reg1 = in_be32(&io_ddr->sdram_cfg);
			reg1 = (reg1 & 0xfffffffd);
			out_be32(&io_ddr->sdram_cfg, reg1);
			udelay(1000);
			asm volatile("sync;isync");

			do {
				reg = in_be32(&io_ddr->mtcr);
			}while((reg >> 31) != 0);
			
			printf("OK\n");
		}
		
		/* Error Clear */
		out_be32(&io_ddr->data_err_inject_hi, 0x0);
		out_be32(&io_ddr->data_err_inject_lo, 0x0);
		out_be32(&io_ddr->ecc_err_inject, 0x0);
		out_be32(&io_ddr->capture_data_hi, 0x0);
		out_be32(&io_ddr->capture_data_lo, 0x0);
		out_be32(&io_ddr->capture_ecc, 0x0);
		out_be32(&io_ddr->err_detect, 0x0);
		out_be32(&io_ddr->err_disable, 0x0);
		out_be32(&io_ddr->capture_attributes, 0x0);
		out_be32(&io_ddr->capture_address, 0x0);
		out_be32(&io_ddr->capture_ext_address, 0x0);
		out_be32(&io_ddr->err_sbe, 0x0);
		udelay(200);
		asm volatile("sync;isync");
		
	}

	return ret;
}


#endif

#ifndef CONFIG_T600
/*
 * This function performs a double word move from the data at
 * the source pointer to the location at the destination pointer.
 * This is helpful for testing memory on processors which have a 64 bit
 * wide data bus.
 *
 * On those PowerPC with FPU, use assembly and a floating point move:
 * this does a 64 bit move.
 *
 * For other processors, let the compiler generate the best code it can.
 */
static void move64(const unsigned long long *src, unsigned long long *dest)
{
#if defined(CONFIG_MPC8260) || defined(CONFIG_MPC824X)
	asm ("lfd  0, 0(3)\n\t" /* fpr0	  =  *scr	*/
	 "stfd 0, 0(4)"		/* *dest  =  fpr0	*/
	 : : : "fr0" );		/* Clobbers fr0		*/
    return;
#else
	*dest = *src;
#endif
}

/*
 * This is 64 bit wide test patterns.  Note that they reside in ROM
 * (which presumably works) and the tests write them to RAM which may
 * not work.
 *
 * The "otherpattern" is written to drive the data bus to values other
 * than the test pattern.  This is for detecting floating bus lines.
 *
 */
const static unsigned long long pattern[] = {
	0xaaaaaaaaaaaaaaaaULL,
	0xccccccccccccccccULL,
	0xf0f0f0f0f0f0f0f0ULL,
	0xff00ff00ff00ff00ULL,
	0xffff0000ffff0000ULL,
	0xffffffff00000000ULL,
	0x00000000ffffffffULL,
	0x0000ffff0000ffffULL,
	0x00ff00ff00ff00ffULL,
	0x0f0f0f0f0f0f0f0fULL,
	0x3333333333333333ULL,
	0x5555555555555555ULL
};
const unsigned long long otherpattern = 0x0123456789abcdefULL;


static int memory_post_dataline(unsigned long long * pmem)
{
	unsigned long long temp64 = 0;
	int num_patterns = ARRAY_SIZE(pattern);
	int i;
	unsigned int hi, lo, pathi, patlo;
	int ret = 0;

	for ( i = 0; i < num_patterns; i++) {
		move64(&(pattern[i]), pmem++);
		/*
		 * Put a different pattern on the data lines: otherwise they
		 * may float long enough to read back what we wrote.
		 */
		move64(&otherpattern, pmem--);
		move64(pmem, &temp64);

#ifdef INJECT_DATA_ERRORS
		temp64 ^= 0x00008000;
#endif

		if (temp64 != pattern[i]){
			pathi = (pattern[i]>>32) & 0xffffffff;
			patlo = pattern[i] & 0xffffffff;

			hi = (temp64>>32) & 0xffffffff;
			lo = temp64 & 0xffffffff;

			post_log("Memory (date line) error at %08x, "
				  "wrote %08x%08x, read %08x%08x !\n",
					  pmem, pathi, patlo, hi, lo);
			ret = -1;
		}
	}
	return ret;
}

static int memory_post_addrline(ulong *testaddr, ulong *base, ulong size)
{
	ulong *target;
	ulong *end;
	ulong readback;
	ulong xor;
	int   ret = 0;

	end = (ulong *)((ulong)base + size);	/* pointer arith! */
	xor = 0;
	for(xor = sizeof(ulong); xor > 0; xor <<= 1) {
		target = (ulong *)((ulong)testaddr ^ xor);
		if((target >= base) && (target < end)) {
			*testaddr = ~*target;
			readback  = *target;

#ifdef INJECT_ADDRESS_ERRORS
			if(xor == 0x00008000) {
				readback = *testaddr;
			}
#endif
			if(readback == *testaddr) {
				post_log("Memory (address line) error at %08x<->%08x, "
					"XOR value %08x !\n",
					testaddr, target, xor);
				ret = -1;
			}
		}
	}
	return ret;
}

static int memory_post_test1(unsigned long start,
			      unsigned long size,
			      unsigned long val)
{
	unsigned long i;
	ulong *mem = (ulong *) start;
	ulong readback;
	int ret = 0;

	for (i = 0; i < size / sizeof (ulong); i++) {
		mem[i] = val;
		if (i % 1024 == 0)
			WATCHDOG_RESET();
	}

	for (i = 0; i < size / sizeof (ulong) && !ret; i++) {
		readback = mem[i];
		if (readback != val) {
			post_log("Memory error at %08x, "
				  "wrote %08x, read %08x !\n",
					  mem + i, val, readback);

			ret = -1;
			break;
		}
		if (i % 1024 == 0)
			WATCHDOG_RESET();
	}

	return ret;
}

static int memory_post_test2(unsigned long start, unsigned long size)
{
	unsigned long i;
	ulong *mem = (ulong *) start;
	ulong readback;
	int ret = 0;

	for (i = 0; i < size / sizeof (ulong); i++) {
		mem[i] = 1 << (i % 32);
		if (i % 1024 == 0)
			WATCHDOG_RESET();
	}

	for (i = 0; i < size / sizeof (ulong) && !ret; i++) {
		readback = mem[i];
		if (readback != (1 << (i % 32))) {
			post_log("Memory error at %08x, "
				  "wrote %08x, read %08x !\n",
					  mem + i, 1 << (i % 32), readback);

			ret = -1;
			break;
		}
		if (i % 1024 == 0)
			WATCHDOG_RESET();
	}

	return ret;
}

static int memory_post_test3(unsigned long start, unsigned long size)
{
	unsigned long i;
	ulong *mem = (ulong *) start;
	ulong readback;
	int ret = 0;

	for (i = 0; i < size / sizeof (ulong); i++) {
		mem[i] = i;
		if (i % 1024 == 0)
			WATCHDOG_RESET();
	}

	for (i = 0; i < size / sizeof (ulong) && !ret; i++) {
		readback = mem[i];
		if (readback != i) {
			post_log("Memory error at %08x, "
				  "wrote %08x, read %08x !\n",
					  mem + i, i, readback);

			ret = -1;
			break;
		}
		if (i % 1024 == 0)
			WATCHDOG_RESET();
	}

	return ret;
}

static int memory_post_test4(unsigned long start, unsigned long size)
{
	unsigned long i;
	ulong *mem = (ulong *) start;
	ulong readback;
	int ret = 0;

	for (i = 0; i < size / sizeof (ulong); i++) {
		mem[i] = ~i;
		if (i % 1024 == 0)
			WATCHDOG_RESET();
	}

	for (i = 0; i < size / sizeof (ulong) && !ret; i++) {
		readback = mem[i];
		if (readback != ~i) {
			post_log("Memory error at %08x, "
				  "wrote %08x, read %08x !\n",
					  mem + i, ~i, readback);

			ret = -1;
			break;
		}
		if (i % 1024 == 0)
			WATCHDOG_RESET();
	}

	return ret;
}

static int memory_post_test_lines(unsigned long start, unsigned long size)
{
	int ret = 0;

	ret = memory_post_dataline((unsigned long long *)start);
	WATCHDOG_RESET();
	if (!ret)
		ret = memory_post_addrline((ulong *)start, (ulong *)start,
				size);
	WATCHDOG_RESET();
	if (!ret)
		ret = memory_post_addrline((ulong *)(start+size-8),
				(ulong *)start, size);
	WATCHDOG_RESET();

	return ret;
}

static int memory_post_test_patterns(unsigned long start, unsigned long size)
{
	int ret = 0;

	ret = memory_post_test1(start, size, 0x00000000);
	WATCHDOG_RESET();
	if (!ret)
		ret = memory_post_test1(start, size, 0xffffffff);
	WATCHDOG_RESET();
	if (!ret)
		ret = memory_post_test1(start, size, 0x55555555);
	WATCHDOG_RESET();
	if (!ret)
		ret = memory_post_test1(start, size, 0xaaaaaaaa);
	WATCHDOG_RESET();
	if (!ret)
		ret = memory_post_test2(start, size);
	WATCHDOG_RESET();
	if (!ret)
		ret = memory_post_test3(start, size);
	WATCHDOG_RESET();
	if (!ret)
		ret = memory_post_test4(start, size);
	WATCHDOG_RESET();

	return ret;
}

static int memory_post_test_regions(unsigned long start, unsigned long size)
{
	unsigned long i;
	int ret = 0;

	for (i = 0; i < (size >> 20) && (!ret); i++) {
		if (!ret)
			ret = memory_post_test_patterns(start + (i << 20),
				0x800);
		if (!ret)
			ret = memory_post_test_patterns(start + (i << 20) +
				0xff800, 0x800);
	}

	return ret;
}

static int memory_post_tests(unsigned long start, unsigned long size)
{
	int ret = 0;

	ret = memory_post_test_lines(start, size);
	if (!ret)
		ret = memory_post_test_patterns(start, size);

	return ret;
}
#endif

/*
 * !! this is only valid, if you have contiguous memory banks !!
 */
__attribute__((weak))
int arch_memory_test_prepare(u32 *vstart, u32 *size, phys_addr_t *phys_offset)
{
	bd_t *bd = gd->bd;

	*vstart = CONFIG_SYS_SDRAM_BASE;
	*size = (gd->ram_size >= 256 << 20 ?
			256 << 20 : gd->ram_size) - (1 << 20);

	/* Limit area to be tested with the board info struct */
	if ((*vstart) + (*size) > (ulong)bd)
		*size = (ulong)bd - *vstart;

	return 0;
}

__attribute__((weak))
int arch_memory_test_advance(u32 *vstart, u32 *size, phys_addr_t *phys_offset)
{
	return 1;
}

__attribute__((weak))
int arch_memory_test_cleanup(u32 *vstart, u32 *size, phys_addr_t *phys_offset)
{
	return 0;
}

__attribute__((weak))
void arch_memory_failure_handle(void)
{
	return;
}

#ifndef CONFIG_T600
int memory_regions_post_test(int flags)
{
	int ret = 0;
	phys_addr_t phys_offset = 0;
	u32 memsize, vstart;

	arch_memory_test_prepare(&vstart, &memsize, &phys_offset);

	ret = memory_post_test_lines(vstart, memsize);
	if (!ret)
		ret = memory_post_test_regions(vstart, memsize);

	return ret;
}
#endif

int memory_post_test(int flags)
{
	int ret = 0;
	phys_addr_t phys_offset = 0;
	u32 memsize, vstart;

	arch_memory_test_prepare(&vstart, &memsize, &phys_offset);
	
#if defined(CONFIG_T600)
	ret = memory_hwtest(TEST_PTN_RAN1,TST_START,MEMCLR_OFF);
	if (0 == ret) {
		ret = memory_hwtest(TEST_PTN_RAN2,TST_NOW,MEMCLR_OFF);
		if (0 == ret) {
			ret = memory_hwtest(TEST_PTN_ALLF,TST_END,MEMCLR_ON);
		}
	}
	
#else
	do {
		if (flags & POST_SLOWTEST) {
			ret = memory_post_tests(vstart, memsize);
		} else {			/* POST_NORMAL */
			ret = memory_post_test_regions(vstart, memsize);
		}
	} while (!ret &&
		!arch_memory_test_advance(&vstart, &memsize, &phys_offset));
#endif

	arch_memory_test_cleanup(&vstart, &memsize, &phys_offset);
	if (ret)
		arch_memory_failure_handle();

	return ret;
}

#endif /* CONFIG_POST&(CONFIG_SYS_POST_MEMORY|CONFIG_SYS_POST_MEM_REGIONS) */

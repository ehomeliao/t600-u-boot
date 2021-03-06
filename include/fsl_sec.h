/*
 * Common internal memory map for some Freescale SoCs
 *
 * Copyright 2014 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __FSL_SEC_H
#define __FSL_SEC_H

#include <common.h>
#include <asm/io.h>

#ifdef CONFIG_SYS_FSL_SEC_LE
#define sec_in32(a)       in_le32(a)
#define sec_out32(a, v)   out_le32(a, v)
#define sec_in16(a)       in_le16(a)
#define sec_clrbits32     clrbits_le32
#define sec_setbits32     setbits_le32
#define snvs_in32(a)       in_le32(a)
#define snvs_out32(a, v)   out_le32(a, v)
#define snvs_in16(a)       in_le16(a)
#define snvs_clrbits32     clrbits_le32
#define snvs_setbits32     setbits_le32
#elif defined(CONFIG_SYS_FSL_SEC_BE)
#define sec_in32(a)       in_be32(a)
#define sec_out32(a, v)   out_be32(a, v)
#define sec_in16(a)       in_be16(a)
#define sec_clrbits32     clrbits_be32
#define sec_setbits32     setbits_be32
#define snvs_in32(a)       in_be32(a)
#define snvs_out32(a, v)   out_be32(a, v)
#define snvs_in16(a)       in_be16(a)
#define snvs_clrbits32     clrbits_be32
#define snvs_setbits32     setbits_be32
#else
#error Neither CONFIG_SYS_FSL_SEC_LE nor CONFIG_SYS_FSL_SEC_BE is defined
#endif

/* Security Engine Block (MS = Most Sig., LS = Least Sig.) */
#if CONFIG_SYS_FSL_SEC_COMPAT >= 4
/* RNG4 TRNG test registers */
struct rng4tst {
#define RTMCTL_PRGM 0x00010000	/* 1 -> program mode, 0 -> run mode */
	u32 rtmctl;		/* misc. control register */
	u32 rtscmisc;		/* statistical check misc. register */
	u32 rtpkrrng;		/* poker range register */
#define RTSDCTL_ENT_DLY_MIN	1200
#define RTSDCTL_ENT_DLY_MAX	12800
	union {
		u32 rtpkrmax;	/* PRGM=1: poker max. limit register */
		u32 rtpkrsq;	/* PRGM=0: poker square calc. result register */
	};
#define RTSDCTL_ENT_DLY_SHIFT 16
#define RTSDCTL_ENT_DLY_MASK (0xffff << RTSDCTL_ENT_DLY_SHIFT)
	u32 rtsdctl;		/* seed control register */
	union {
		u32 rtsblim;	/* PRGM=1: sparse bit limit register */
		u32 rttotsam;	/* PRGM=0: total samples register */
	};
	u32 rtfreqmin;		/* frequency count min. limit register */
	union {
		u32 rtfreqmax;	/* PRGM=1: freq. count max. limit register */
		u32 rtfreqcnt;	/* PRGM=0: freq. count register */
	};
	u32 rsvd1[40];
#define RNG_STATE0_HANDLE_INSTANTIATED	0x00000001
	u32 rdsta;		/*RNG DRNG Status Register*/
	u32 rsvd2[15];
};

typedef struct ccsr_sec {
	u32	res0;
	u32	mcfgr;		/* Master CFG Register */
	u8	res1[0x4];
	u32	scfgr;
	struct {
		u32	ms;	/* Job Ring LIODN Register, MS */
		u32	ls;	/* Job Ring LIODN Register, LS */
	} jrliodnr[4];
	u8	res2[0x2c];
	u32	jrstartr;	/* Job Ring Start Register */
	struct {
		u32	ms;	/* RTIC LIODN Register, MS */
		u32	ls;	/* RTIC LIODN Register, LS */
	} rticliodnr[4];
	u8	res3[0x1c];
	u32	decorr;		/* DECO Request Register */
	struct {
		u32	ms;	/* DECO LIODN Register, MS */
		u32	ls;	/* DECO LIODN Register, LS */
	} decoliodnr[8];
	u8	res4[0x40];
	u32	dar;		/* DECO Avail Register */
	u32	drr;		/* DECO Reset Register */
	u8	res5[0x4d8];
	struct rng4tst rng;	/* RNG Registers */
	u8	res6[0x8a0];
	u32	crnr_ms;	/* CHA Revision Number Register, MS */
	u32	crnr_ls;	/* CHA Revision Number Register, LS */
	u32	ctpr_ms;	/* Compile Time Parameters Register, MS */
	u32	ctpr_ls;	/* Compile Time Parameters Register, LS */
	u8	res7[0x10];
	u32	far_ms;		/* Fault Address Register, MS */
	u32	far_ls;		/* Fault Address Register, LS */
	u32	falr;		/* Fault Address LIODN Register */
	u32	fadr;		/* Fault Address Detail Register */
	u8	res8[0x4];
	u32	csta;		/* CAAM Status Register */
	u8	res9[0x8];
	u32	rvid;		/* Run Time Integrity Checking Version ID Reg.*/
	u32	ccbvid;		/* CHA Cluster Block Version ID Register */
	u32	chavid_ms;	/* CHA Version ID Register, MS */
	u32	chavid_ls;	/* CHA Version ID Register, LS */
	u32	chanum_ms;	/* CHA Number Register, MS */
	u32	chanum_ls;	/* CHA Number Register, LS */
	u32	secvid_ms;	/* SEC Version ID Register, MS */
	u32	secvid_ls;	/* SEC Version ID Register, LS */
	u8	res10[0x6020];
	u32	qilcr_ms;	/* Queue Interface LIODN CFG Register, MS */
	u32	qilcr_ls;	/* Queue Interface LIODN CFG Register, LS */
	u8	res11[0x8fd8];
} ccsr_sec_t;

#define SEC_CTPR_MS_AXI_LIODN		0x08000000
#define SEC_CTPR_MS_QI			0x02000000
#define SEC_CTPR_MS_VIRT_EN_INCL	0x00000001
#define SEC_CTPR_MS_VIRT_EN_POR		0x00000002
#define SEC_RVID_MA			0x0f000000
#define SEC_CHANUM_MS_JRNUM_MASK	0xf0000000
#define SEC_CHANUM_MS_JRNUM_SHIFT	28
#define SEC_CHANUM_MS_DECONUM_MASK	0x0f000000
#define SEC_CHANUM_MS_DECONUM_SHIFT	24
#define SEC_SECVID_MS_IPID_MASK	0xffff0000
#define SEC_SECVID_MS_IPID_SHIFT	16
#define SEC_SECVID_MS_MAJ_REV_MASK	0x0000ff00
#define SEC_SECVID_MS_MAJ_REV_SHIFT	8
#define SEC_CCBVID_ERA_MASK		0xff000000
#define SEC_CCBVID_ERA_SHIFT		24
#define SEC_SCFGR_RDBENABLE		0x00000400
#define SEC_SCFGR_VIRT_EN		0x00008000
#define SEC_CHAVID_LS_RNG_SHIFT		16
#define SEC_CHAVID_RNG_LS_MASK		0x000f0000

#define CONFIG_JRSTARTR_JR0		0x00000001
#endif


struct jr_regs {
#ifdef CONFIG_SYS_FSL_SEC_LE
	u32 irba_l;
	u32 irba_h;
#else
	u32 irba_h;
	u32 irba_l;
#endif
	u32 rsvd1;
	u32 irs;
	u32 rsvd2;
	u32 irsa;
	u32 rsvd3;
	u32 irja;
#ifdef CONFIG_SYS_FSL_SEC_LE
	u32 orba_l;
	u32 orba_h;
#else
	u32 orba_h;
	u32 orba_l;
#endif
	u32 rsvd4;
	u32 ors;
	u32 rsvd5;
	u32 orjr;
	u32 rsvd6;
	u32 orsf;
	u32 rsvd7;
	u32 jrsta;
	u32 rsvd8;
	u32 jrint;
	u32 jrcfg0;
	u32 jrcfg1;
	u32 rsvd9;
	u32 irri;
	u32 rsvd10;
	u32 orwi;
	u32 rsvd11;
	u32 jrcr;
};


struct ccsr_snvs_regs {
	u8 reserved0[0x04];
	u32 hp_com;	/* 0x04 SNVS_HP Command Register */
	u8 reserved2[0x0c];
	u32 hp_stat;	/* 0x08 SNVS_HP Status Register */
};

#define HPCOMR_SW_SV 0x100		/* Security Violation bit */
#define HPCOMR_SW_FSV 0x200		/* Fatal Security Violation bit */
#define HPCOMR_SSM_ST 0x1		/* SSM_ST field in SNVS command reg */
#define HPSR_SSM_ST_CHECK	0x900	/* SNVS is in check state */
#define HPSR_SSM_ST_NON_SECURE	0xb00	/* SNVS is in non secure state */
#define HPSR_SSM_ST_TRUST	0xd00	/* SNVS is in trusted state */
#define HPSR_SSM_ST_SOFT_FAIL	0x300	/* SNVS is in soft fail state */
#define HPSR_SSM_ST_MASK	0xf00	/* Mask for SSM_ST field */

/*
 * SNVS read. This specifies the possible reads
 * from the SNVS
 */
enum {
	SNVS_SSM_ST,
	SNVS_SW_FSV,
	SNVS_SW_SV,
};

int change_sec_mon_state(uint32_t initial_state, uint32_t final_state);

#endif /* __FSL_SEC_H */

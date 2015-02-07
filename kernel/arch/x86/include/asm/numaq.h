/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/*
 * Written by: Patricia Gaughen, IBM Corporation
 *
 * Copyright (C) 2002, IBM Corp.
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 * NON INFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Send feedback to <gone@us.ibm.com>
 */

#ifndef _ASM_X86_NUMAQ_H
#define _ASM_X86_NUMAQ_H

#ifdef CONFIG_X86_NUMAQ

extern int found_numaq;
extern int get_memcfg_numaq(void);

/*
 * SYS_CFG_DATA_PRIV_ADDR, struct eachquadmem, and struct sys_cfg_data are the
 */
#define SYS_CFG_DATA_PRIV_ADDR		0x0009d000 /* place for scd in private
						      quad space */

/*
 * Communication area for each processor on lynxer-processor tests.
 *
 * NOTE: If you change the size of this eachproc structure you need
 *       to change the definition for EACH_QUAD_SIZE.
 */
struct eachquadmem {
	unsigned int	priv_mem_start;		/* Starting address of this */
						/* quad's private memory. */
						/* This is always 0. */
						/* In MB. */
	unsigned int	priv_mem_size;		/* Size of this quad's */
						/* private memory. */
						/* In MB. */
	unsigned int	low_shrd_mem_strp_start;/* Starting address of this */
						/* quad's low shared block */
						/* (untranslated). */
						/* In MB. */
	unsigned int	low_shrd_mem_start;	/* Starting address of this */
						/* quad's low shared memory */
						/* (untranslated). */
						/* In MB. */
	unsigned int	low_shrd_mem_size;	/* Size of this quad's low */
						/* shared memory. */
						/* In MB. */
	unsigned int	lmmio_copb_start;	/* Starting address of this */
						/* quad's local memory */
						/* mapped I/O in the */
						/* compatibility OPB. */
						/* In MB. */
	unsigned int	lmmio_copb_size;	/* Size of this quad's local */
						/* memory mapped I/O in the */
						/* compatibility OPB. */
						/* In MB. */
	unsigned int	lmmio_nopb_start;	/* Starting address of this */
						/* quad's local memory */
						/* mapped I/O in the */
						/* non-compatibility OPB. */
						/* In MB. */
	unsigned int	lmmio_nopb_size;	/* Size of this quad's local */
						/* memory mapped I/O in the */
						/* non-compatibility OPB. */
						/* In MB. */
	unsigned int	io_apic_0_start;	/* Starting address of I/O */
						/* APIC 0. */
	unsigned int	io_apic_0_sz;		/* Size I/O APIC 0. */
	unsigned int	io_apic_1_start;	/* Starting address of I/O */
						/* APIC 1. */
	unsigned int	io_apic_1_sz;		/* Size I/O APIC 1. */
	unsigned int	hi_shrd_mem_start;	/* Starting address of this */
						/* quad's high shared memory.*/
						/* In MB. */
	unsigned int	hi_shrd_mem_size;	/* Size of this quad's high */
						/* shared memory. */
						/* In MB. */
	unsigned int	mps_table_addr;		/* Address of this quad's */
						/* MPS tables from BIOS, */
						/* in system space.*/
	unsigned int	lcl_MDC_pio_addr;	/* Port-I/O address for */
						/* local access of MDC. */
	unsigned int	rmt_MDC_mmpio_addr;	/* MM-Port-I/O address for */
						/* remote access of MDC. */
	unsigned int	mm_port_io_start;	/* Starting address of this */
						/* quad's memory mapped Port */
						/* I/O space. */
	unsigned int	mm_port_io_size;	/* Size of this quad's memory*/
						/* mapped Port I/O space. */
	unsigned int	mm_rmt_io_apic_start;	/* Starting address of this */
						/* quad's memory mapped */
						/* remote I/O APIC space. */
	unsigned int	mm_rmt_io_apic_size;	/* Size of this quad's memory*/
						/* mapped remote I/O APIC */
						/* space. */
	unsigned int	mm_isa_start;		/* Starting address of this */
						/* quad's memory mapped ISA */
						/* space (contains MDC */
						/* memory space). */
	unsigned int	mm_isa_size;		/* Size of this quad's memory*/
						/* mapped ISA space (contains*/
						/* MDC memory space). */
	unsigned int	rmt_qmi_addr;		/* Remote addr to access QMI.*/
	unsigned int	lcl_qmi_addr;		/* Local addr to access QMI. */
};

/*
 * Note: This structure must be NOT be changed unless the multiproc and
 * OS are changed to reflect the new structure.
 */
struct sys_cfg_data {
	unsigned int	quad_id;
	unsigned int	bsp_proc_id; /* Boot Strap Processor in this quad. */
	unsigned int	scd_version; /* Version number of this table. */
	unsigned int	first_quad_id;
	unsigned int	quads_present31_0; /* 1 bit for each quad */
	unsigned int	quads_present63_32; /* 1 bit for each quad */
	unsigned int	config_flags;
	unsigned int	boot_flags;
	unsigned int	csr_start_addr; /* Absolute value (not in MB) */
	unsigned int	csr_size; /* Absolute value (not in MB) */
	unsigned int	lcl_apic_start_addr; /* Absolute value (not in MB) */
	unsigned int	lcl_apic_size; /* Absolute value (not in MB) */
	unsigned int	low_shrd_mem_base; /* 0 or 512MB or 1GB */
	unsigned int	low_shrd_mem_quad_offset; /* 0,128M,256M,512M,1G */
					/* may not be totally populated */
	unsigned int	split_mem_enbl; /* 0 for no low shared memory */
	unsigned int	mmio_sz; /* Size of total system memory mapped I/O */
				 /* (in MB). */
	unsigned int	quad_spin_lock; /* Spare location used for quad */
					/* bringup. */
	unsigned int	nonzero55; /* For checksumming. */
	unsigned int	nonzeroaa; /* For checksumming. */
	unsigned int	scd_magic_number;
	unsigned int	system_type;
	unsigned int	checksum;
	/*
	 *	memory configuration area for each quad
	 */
	struct		eachquadmem eq[MAX_NUMNODES];	/* indexed by quad id */
};

void numaq_tsc_disable(void);

#else
static inline int get_memcfg_numaq(void)
{
	return 0;
}
#endif /* CONFIG_X86_NUMAQ */
#endif /* _ASM_X86_NUMAQ_H */


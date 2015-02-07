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
 *               Blackfin CPLB exception handling.
 *               Copyright 2004-2007 Analog Devices Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <linux/module.h>
#include <linux/mm.h>

#include <asm/blackfin.h>
#include <asm/cacheflush.h>
#include <asm/cplbinit.h>
#include <asm/mmu_context.h>

/*
 * WARNING
 *
 * This file is compiled with certain -ffixed-reg options.  We have to
 * make sure not to call any functions here that could clobber these
 * registers.
 */

int page_mask_nelts;
int page_mask_order;
unsigned long *current_rwx_mask[NR_CPUS];

int nr_dcplb_miss[NR_CPUS], nr_icplb_miss[NR_CPUS];
int nr_icplb_supv_miss[NR_CPUS], nr_dcplb_prot[NR_CPUS];
int nr_cplb_flush[NR_CPUS];

static inline void disable_dcplb(void)
{
	unsigned long ctrl;
	SSYNC();
	ctrl = bfin_read_DMEM_CONTROL();
	ctrl &= ~ENDCPLB;
	bfin_write_DMEM_CONTROL(ctrl);
	SSYNC();
}

static inline void enable_dcplb(void)
{
	unsigned long ctrl;
	SSYNC();
	ctrl = bfin_read_DMEM_CONTROL();
	ctrl |= ENDCPLB;
	bfin_write_DMEM_CONTROL(ctrl);
	SSYNC();
}

static inline void disable_icplb(void)
{
	unsigned long ctrl;
	SSYNC();
	ctrl = bfin_read_IMEM_CONTROL();
	ctrl &= ~ENICPLB;
	bfin_write_IMEM_CONTROL(ctrl);
	SSYNC();
}

static inline void enable_icplb(void)
{
	unsigned long ctrl;
	SSYNC();
	ctrl = bfin_read_IMEM_CONTROL();
	ctrl |= ENICPLB;
	bfin_write_IMEM_CONTROL(ctrl);
	SSYNC();
}

/*
 * Given the contents of the status register, return the index of the
 * CPLB that caused the fault.
 */
static inline int faulting_cplb_index(int status)
{
	int signbits = __builtin_bfin_norm_fr1x32(status & 0xFFFF);
	return 30 - signbits;
}

/*
 * Given the contents of the status register and the DCPLB_DATA contents,
 * return true if a write access should be permitted.
 */
static inline int write_permitted(int status, unsigned long data)
{
	if (status & FAULT_USERSUPV)
		return !!(data & CPLB_SUPV_WR);
	else
		return !!(data & CPLB_USER_WR);
}

/* Counters to implement round-robin replacement.  */
static int icplb_rr_index[NR_CPUS], dcplb_rr_index[NR_CPUS];

/*
 * Find an ICPLB entry to be evicted and return its index.
 */
static int evict_one_icplb(unsigned int cpu)
{
	int i;
	for (i = first_switched_icplb; i < MAX_CPLBS; i++)
		if ((icplb_tbl[cpu][i].data & CPLB_VALID) == 0)
			return i;
	i = first_switched_icplb + icplb_rr_index[cpu];
	if (i >= MAX_CPLBS) {
		i -= MAX_CPLBS - first_switched_icplb;
		icplb_rr_index[cpu] -= MAX_CPLBS - first_switched_icplb;
	}
	icplb_rr_index[cpu]++;
	return i;
}

static int evict_one_dcplb(unsigned int cpu)
{
	int i;
	for (i = first_switched_dcplb; i < MAX_CPLBS; i++)
		if ((dcplb_tbl[cpu][i].data & CPLB_VALID) == 0)
			return i;
	i = first_switched_dcplb + dcplb_rr_index[cpu];
	if (i >= MAX_CPLBS) {
		i -= MAX_CPLBS - first_switched_dcplb;
		dcplb_rr_index[cpu] -= MAX_CPLBS - first_switched_dcplb;
	}
	dcplb_rr_index[cpu]++;
	return i;
}

static noinline int dcplb_miss(unsigned int cpu)
{
	unsigned long addr = bfin_read_DCPLB_FAULT_ADDR();
	int status = bfin_read_DCPLB_STATUS();
	unsigned long *mask;
	int idx;
	unsigned long d_data;

	nr_dcplb_miss[cpu]++;

	d_data = CPLB_SUPV_WR | CPLB_VALID | CPLB_DIRTY | PAGE_SIZE_4KB;
#ifdef CONFIG_BFIN_DCACHE
	if (bfin_addr_dcachable(addr)) {
		d_data |= CPLB_L1_CHBL | ANOMALY_05000158_WORKAROUND;
#ifdef CONFIG_BFIN_WT
		d_data |= CPLB_L1_AOW | CPLB_WT;
#endif
	}
#endif
	if (addr >= physical_mem_end) {
		if (addr >= ASYNC_BANK0_BASE && addr < ASYNC_BANK3_BASE + ASYNC_BANK3_SIZE
		    && (status & FAULT_USERSUPV)) {
			addr &= ~0x3fffff;
			d_data &= ~PAGE_SIZE_4KB;
			d_data |= PAGE_SIZE_4MB;
		} else if (addr >= BOOT_ROM_START && addr < BOOT_ROM_START + BOOT_ROM_LENGTH
		    && (status & (FAULT_RW | FAULT_USERSUPV)) == FAULT_USERSUPV) {
			addr &= ~(1 * 1024 * 1024 - 1);
			d_data &= ~PAGE_SIZE_4KB;
			d_data |= PAGE_SIZE_1MB;
		} else
			return CPLB_PROT_VIOL;
	} else if (addr >= _ramend) {
	    d_data |= CPLB_USER_RD | CPLB_USER_WR;
	} else {
		mask = current_rwx_mask[cpu];
		if (mask) {
			int page = addr >> PAGE_SHIFT;
			int idx = page >> 5;
			int bit = 1 << (page & 31);

			if (mask[idx] & bit)
				d_data |= CPLB_USER_RD;

			mask += page_mask_nelts;
			if (mask[idx] & bit)
				d_data |= CPLB_USER_WR;
		}
	}
	idx = evict_one_dcplb(cpu);

	addr &= PAGE_MASK;
	dcplb_tbl[cpu][idx].addr = addr;
	dcplb_tbl[cpu][idx].data = d_data;

	disable_dcplb();
	bfin_write32(DCPLB_DATA0 + idx * 4, d_data);
	bfin_write32(DCPLB_ADDR0 + idx * 4, addr);
	enable_dcplb();

	return 0;
}

static noinline int icplb_miss(unsigned int cpu)
{
	unsigned long addr = bfin_read_ICPLB_FAULT_ADDR();
	int status = bfin_read_ICPLB_STATUS();
	int idx;
	unsigned long i_data;

	nr_icplb_miss[cpu]++;

	/* If inside the uncached DMA region, fault.  */
	if (addr >= _ramend - DMA_UNCACHED_REGION && addr < _ramend)
		return CPLB_PROT_VIOL;

	if (status & FAULT_USERSUPV)
		nr_icplb_supv_miss[cpu]++;

	/*
	 * First, try to find a CPLB that matches this address.  If we
	 * find one, then the fact that we're in the miss handler means
	 * that the instruction crosses a page boundary.
	 */
	for (idx = first_switched_icplb; idx < MAX_CPLBS; idx++) {
		if (icplb_tbl[cpu][idx].data & CPLB_VALID) {
			unsigned long this_addr = icplb_tbl[cpu][idx].addr;
			if (this_addr <= addr && this_addr + PAGE_SIZE > addr) {
				addr += PAGE_SIZE;
				break;
			}
		}
	}

	i_data = CPLB_VALID | CPLB_PORTPRIO | PAGE_SIZE_4KB;

#ifdef CONFIG_BFIN_ICACHE
	/*
	 * Normal RAM, and possibly the reserved memory area, are
	 * cacheable.
	 */
	if (addr < _ramend ||
	    (addr < physical_mem_end && reserved_mem_icache_on))
		i_data |= CPLB_L1_CHBL | ANOMALY_05000158_WORKAROUND;
#endif

	if (addr >= physical_mem_end) {
		if (addr >= BOOT_ROM_START && addr < BOOT_ROM_START + BOOT_ROM_LENGTH
		    && (status & FAULT_USERSUPV)) {
			addr &= ~(1 * 1024 * 1024 - 1);
			i_data &= ~PAGE_SIZE_4KB;
			i_data |= PAGE_SIZE_1MB;
		} else
		    return CPLB_PROT_VIOL;
	} else if (addr >= _ramend) {
		i_data |= CPLB_USER_RD;
	} else {
		/*
		 * Two cases to distinguish - a supervisor access must
		 * necessarily be for a module page; we grant it
		 * unconditionally (could do better here in the future).
		 * Otherwise, check the x bitmap of the current process.
		 */
		if (!(status & FAULT_USERSUPV)) {
			unsigned long *mask = current_rwx_mask[cpu];

			if (mask) {
				int page = addr >> PAGE_SHIFT;
				int idx = page >> 5;
				int bit = 1 << (page & 31);

				mask += 2 * page_mask_nelts;
				if (mask[idx] & bit)
					i_data |= CPLB_USER_RD;
			}
		}
	}
	idx = evict_one_icplb(cpu);
	addr &= PAGE_MASK;
	icplb_tbl[cpu][idx].addr = addr;
	icplb_tbl[cpu][idx].data = i_data;

	disable_icplb();
	bfin_write32(ICPLB_DATA0 + idx * 4, i_data);
	bfin_write32(ICPLB_ADDR0 + idx * 4, addr);
	enable_icplb();

	return 0;
}

static noinline int dcplb_protection_fault(unsigned int cpu)
{
	int status = bfin_read_DCPLB_STATUS();

	nr_dcplb_prot[cpu]++;

	if (status & FAULT_RW) {
		int idx = faulting_cplb_index(status);
		unsigned long data = dcplb_tbl[cpu][idx].data;
		if (!(data & CPLB_WT) && !(data & CPLB_DIRTY) &&
		    write_permitted(status, data)) {
			data |= CPLB_DIRTY;
			dcplb_tbl[cpu][idx].data = data;
			bfin_write32(DCPLB_DATA0 + idx * 4, data);
			return 0;
		}
	}
	return CPLB_PROT_VIOL;
}

int cplb_hdr(int seqstat, struct pt_regs *regs)
{
	int cause = seqstat & 0x3f;
	unsigned int cpu = smp_processor_id();
	switch (cause) {
	case 0x23:
		return dcplb_protection_fault(cpu);
	case 0x2C:
		return icplb_miss(cpu);
	case 0x26:
		return dcplb_miss(cpu);
	default:
		return 1;
	}
}

void flush_switched_cplbs(unsigned int cpu)
{
	int i;
	unsigned long flags;

	nr_cplb_flush[cpu]++;

	local_irq_save_hw(flags);
	disable_icplb();
	for (i = first_switched_icplb; i < MAX_CPLBS; i++) {
		icplb_tbl[cpu][i].data = 0;
		bfin_write32(ICPLB_DATA0 + i * 4, 0);
	}
	enable_icplb();

	disable_dcplb();
	for (i = first_switched_dcplb; i < MAX_CPLBS; i++) {
		dcplb_tbl[cpu][i].data = 0;
		bfin_write32(DCPLB_DATA0 + i * 4, 0);
	}
	enable_dcplb();
	local_irq_restore_hw(flags);

}

void set_mask_dcplbs(unsigned long *masks, unsigned int cpu)
{
	int i;
	unsigned long addr = (unsigned long)masks;
	unsigned long d_data;
	unsigned long flags;

	if (!masks) {
		current_rwx_mask[cpu] = masks;
		return;
	}

	local_irq_save_hw(flags);
	current_rwx_mask[cpu] = masks;

	d_data = CPLB_SUPV_WR | CPLB_VALID | CPLB_DIRTY | PAGE_SIZE_4KB;
#ifdef CONFIG_BFIN_DCACHE
	d_data |= CPLB_L1_CHBL;
#ifdef CONFIG_BFIN_WT
	d_data |= CPLB_L1_AOW | CPLB_WT;
#endif
#endif

	disable_dcplb();
	for (i = first_mask_dcplb; i < first_switched_dcplb; i++) {
		dcplb_tbl[cpu][i].addr = addr;
		dcplb_tbl[cpu][i].data = d_data;
		bfin_write32(DCPLB_DATA0 + i * 4, d_data);
		bfin_write32(DCPLB_ADDR0 + i * 4, addr);
		addr += PAGE_SIZE;
	}
	enable_dcplb();
	local_irq_restore_hw(flags);
}

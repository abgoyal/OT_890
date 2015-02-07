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
 *  arch/s390/kernel/early.c
 *
 *    Copyright IBM Corp. 2007
 *    Author(s): Hongjie Yang <hongjie@us.ibm.com>,
 *		 Heiko Carstens <heiko.carstens@de.ibm.com>
 */

#include <linux/init.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/ctype.h>
#include <linux/lockdep.h>
#include <linux/module.h>
#include <linux/pfn.h>
#include <linux/uaccess.h>
#include <asm/ebcdic.h>
#include <asm/ipl.h>
#include <asm/lowcore.h>
#include <asm/processor.h>
#include <asm/sections.h>
#include <asm/setup.h>
#include <asm/cpcmd.h>
#include <asm/sclp.h>
#include "entry.h"

/*
 * Create a Kernel NSS if the SAVESYS= parameter is defined
 */
#define DEFSYS_CMD_SIZE		128
#define SAVESYS_CMD_SIZE	32

char kernel_nss_name[NSS_NAME_SIZE + 1];

static void __init setup_boot_command_line(void);


#ifdef CONFIG_SHARED_KERNEL
int __init savesys_ipl_nss(char *cmd, const int cmdlen);

asm(
	"	.section .init.text,\"ax\",@progbits\n"
	"	.align	4\n"
	"	.type	savesys_ipl_nss, @function\n"
	"savesys_ipl_nss:\n"
#ifdef CONFIG_64BIT
	"	stmg	6,15,48(15)\n"
	"	lgr	14,3\n"
	"	sam31\n"
	"	diag	2,14,0x8\n"
	"	sam64\n"
	"	lgr	2,14\n"
	"	lmg	6,15,48(15)\n"
#else
	"	stm	6,15,24(15)\n"
	"	lr	14,3\n"
	"	diag	2,14,0x8\n"
	"	lr	2,14\n"
	"	lm	6,15,24(15)\n"
#endif
	"	br	14\n"
	"	.size	savesys_ipl_nss, .-savesys_ipl_nss\n");

static noinline __init void create_kernel_nss(void)
{
	unsigned int i, stext_pfn, eshared_pfn, end_pfn, min_size;
#ifdef CONFIG_BLK_DEV_INITRD
	unsigned int sinitrd_pfn, einitrd_pfn;
#endif
	int response;
	size_t len;
	char *savesys_ptr;
	char upper_command_line[COMMAND_LINE_SIZE];
	char defsys_cmd[DEFSYS_CMD_SIZE];
	char savesys_cmd[SAVESYS_CMD_SIZE];

	/* Do nothing if we are not running under VM */
	if (!MACHINE_IS_VM)
		return;

	/* Convert COMMAND_LINE to upper case */
	for (i = 0; i < strlen(boot_command_line); i++)
		upper_command_line[i] = toupper(boot_command_line[i]);

	savesys_ptr = strstr(upper_command_line, "SAVESYS=");

	if (!savesys_ptr)
		return;

	savesys_ptr += 8;    /* Point to the beginning of the NSS name */
	for (i = 0; i < NSS_NAME_SIZE; i++) {
		if (savesys_ptr[i] == ' ' || savesys_ptr[i] == '\0')
			break;
		kernel_nss_name[i] = savesys_ptr[i];
	}

	stext_pfn = PFN_DOWN(__pa(&_stext));
	eshared_pfn = PFN_DOWN(__pa(&_eshared));
	end_pfn = PFN_UP(__pa(&_end));
	min_size = end_pfn << 2;

	sprintf(defsys_cmd, "DEFSYS %s 00000-%.5X EW %.5X-%.5X SR %.5X-%.5X",
		kernel_nss_name, stext_pfn - 1, stext_pfn, eshared_pfn - 1,
		eshared_pfn, end_pfn);

#ifdef CONFIG_BLK_DEV_INITRD
	if (INITRD_START && INITRD_SIZE) {
		sinitrd_pfn = PFN_DOWN(__pa(INITRD_START));
		einitrd_pfn = PFN_UP(__pa(INITRD_START + INITRD_SIZE));
		min_size = einitrd_pfn << 2;
		sprintf(defsys_cmd, "%s EW %.5X-%.5X", defsys_cmd,
		sinitrd_pfn, einitrd_pfn);
	}
#endif

	sprintf(defsys_cmd, "%s EW MINSIZE=%.7iK PARMREGS=0-13",
		defsys_cmd, min_size);
	sprintf(savesys_cmd, "SAVESYS %s \n IPL %s",
		kernel_nss_name, kernel_nss_name);

	__cpcmd(defsys_cmd, NULL, 0, &response);

	if (response != 0) {
		kernel_nss_name[0] = '\0';
		return;
	}

	len = strlen(savesys_cmd);
	ASCEBC(savesys_cmd, len);
	response = savesys_ipl_nss(savesys_cmd, len);

	/* On success: response is equal to the command size,
	 *	       max SAVESYS_CMD_SIZE
	 * On error: response contains the numeric portion of cp error message.
	 *	     for SAVESYS it will be >= 263
	 */
	if (response > SAVESYS_CMD_SIZE) {
		kernel_nss_name[0] = '\0';
		return;
	}

	/* re-setup boot command line with new ipl vm parms */
	ipl_update_parameters();
	setup_boot_command_line();

	ipl_flags = IPL_NSS_VALID;
}

#else /* CONFIG_SHARED_KERNEL */

static inline void create_kernel_nss(void) { }

#endif /* CONFIG_SHARED_KERNEL */

/*
 * Clear bss memory
 */
static noinline __init void clear_bss_section(void)
{
	memset(__bss_start, 0, __bss_stop - __bss_start);
}

/*
 * Initialize storage key for kernel pages
 */
static noinline __init void init_kernel_storage_key(void)
{
	unsigned long end_pfn, init_pfn;

	end_pfn = PFN_UP(__pa(&_end));

	for (init_pfn = 0 ; init_pfn < end_pfn; init_pfn++)
		page_set_storage_key(init_pfn << PAGE_SHIFT, PAGE_DEFAULT_KEY);
}

static noinline __init void detect_machine_type(void)
{
	struct cpuinfo_S390 *cpuinfo = &S390_lowcore.cpu_data;

	get_cpu_id(&S390_lowcore.cpu_data.cpu_id);

	/* Running under z/VM ? */
	if (cpuinfo->cpu_id.version == 0xff)
		machine_flags |= MACHINE_FLAG_VM;

	/* Running under KVM ? */
	if (cpuinfo->cpu_id.version == 0xfe)
		machine_flags |= MACHINE_FLAG_KVM;
}

static __init void early_pgm_check_handler(void)
{
	unsigned long addr;
	const struct exception_table_entry *fixup;

	addr = S390_lowcore.program_old_psw.addr;
	fixup = search_exception_tables(addr & PSW_ADDR_INSN);
	if (!fixup)
		disabled_wait(0);
	S390_lowcore.program_old_psw.addr = fixup->fixup | PSW_ADDR_AMODE;
}

static noinline __init void setup_lowcore_early(void)
{
	psw_t psw;

	psw.mask = PSW_BASE_BITS | PSW_DEFAULT_KEY;
	psw.addr = PSW_ADDR_AMODE | (unsigned long) s390_base_ext_handler;
	S390_lowcore.external_new_psw = psw;
	psw.addr = PSW_ADDR_AMODE | (unsigned long) s390_base_pgm_handler;
	S390_lowcore.program_new_psw = psw;
	s390_base_pgm_handler_fn = early_pgm_check_handler;
}

static noinline __init void setup_hpage(void)
{
#ifndef CONFIG_DEBUG_PAGEALLOC
	unsigned int facilities;

	facilities = stfl();
	if (!(facilities & (1UL << 23)) || !(facilities & (1UL << 29)))
		return;
	machine_flags |= MACHINE_FLAG_HPAGE;
	__ctl_set_bit(0, 23);
#endif
}

static __init void detect_mvpg(void)
{
#ifndef CONFIG_64BIT
	int rc;

	asm volatile(
		"	la	0,0\n"
		"	mvpg	%2,%2\n"
		"0:	la	%0,0\n"
		"1:\n"
		EX_TABLE(0b,1b)
		: "=d" (rc) : "0" (-EOPNOTSUPP), "a" (0) : "memory", "cc", "0");
	if (!rc)
		machine_flags |= MACHINE_FLAG_MVPG;
#endif
}

static __init void detect_ieee(void)
{
#ifndef CONFIG_64BIT
	int rc, tmp;

	asm volatile(
		"	efpc	%1,0\n"
		"0:	la	%0,0\n"
		"1:\n"
		EX_TABLE(0b,1b)
		: "=d" (rc), "=d" (tmp): "0" (-EOPNOTSUPP) : "cc");
	if (!rc)
		machine_flags |= MACHINE_FLAG_IEEE;
#endif
}

static __init void detect_csp(void)
{
#ifndef CONFIG_64BIT
	int rc;

	asm volatile(
		"	la	0,0\n"
		"	la	1,0\n"
		"	la	2,4\n"
		"	csp	0,2\n"
		"0:	la	%0,0\n"
		"1:\n"
		EX_TABLE(0b,1b)
		: "=d" (rc) : "0" (-EOPNOTSUPP) : "cc", "0", "1", "2");
	if (!rc)
		machine_flags |= MACHINE_FLAG_CSP;
#endif
}

static __init void detect_diag9c(void)
{
	unsigned int cpu_address;
	int rc;

	cpu_address = stap();
	asm volatile(
		"	diag	%2,0,0x9c\n"
		"0:	la	%0,0\n"
		"1:\n"
		EX_TABLE(0b,1b)
		: "=d" (rc) : "0" (-EOPNOTSUPP), "d" (cpu_address) : "cc");
	if (!rc)
		machine_flags |= MACHINE_FLAG_DIAG9C;
}

static __init void detect_diag44(void)
{
#ifdef CONFIG_64BIT
	int rc;

	asm volatile(
		"	diag	0,0,0x44\n"
		"0:	la	%0,0\n"
		"1:\n"
		EX_TABLE(0b,1b)
		: "=d" (rc) : "0" (-EOPNOTSUPP) : "cc");
	if (!rc)
		machine_flags |= MACHINE_FLAG_DIAG44;
#endif
}

static __init void detect_machine_facilities(void)
{
#ifdef CONFIG_64BIT
	unsigned int facilities;

	facilities = stfl();
	if (facilities & (1 << 28))
		machine_flags |= MACHINE_FLAG_IDTE;
	if (facilities & (1 << 23))
		machine_flags |= MACHINE_FLAG_PFMF;
	if (facilities & (1 << 4))
		machine_flags |= MACHINE_FLAG_MVCOS;
#endif
}

static __init void rescue_initrd(void)
{
#ifdef CONFIG_BLK_DEV_INITRD
	/*
	 * Move the initrd right behind the bss section in case it starts
	 * within the bss section. So we don't overwrite it when the bss
	 * section gets cleared.
	 */
	if (!INITRD_START || !INITRD_SIZE)
		return;
	if (INITRD_START >= (unsigned long) __bss_stop)
		return;
	memmove(__bss_stop, (void *) INITRD_START, INITRD_SIZE);
	INITRD_START = (unsigned long) __bss_stop;
#endif
}

/* Set up boot command line */
static void __init setup_boot_command_line(void)
{
	char *parm = NULL;

	/* copy arch command line */
	strlcpy(boot_command_line, COMMAND_LINE, ARCH_COMMAND_LINE_SIZE);
	boot_command_line[ARCH_COMMAND_LINE_SIZE - 1] = 0;

	/* append IPL PARM data to the boot command line */
	if (MACHINE_IS_VM) {
		parm = boot_command_line + strlen(boot_command_line);
		*parm++ = ' ';
		get_ipl_vmparm(parm);
		if (parm[0] == '=')
			memmove(boot_command_line, parm + 1, strlen(parm));
	}
}


/*
 * Save ipl parameters, clear bss memory, initialize storage keys
 * and create a kernel NSS at startup if the SAVESYS= parm is defined
 */
void __init startup_init(void)
{
	ipl_save_parameters();
	rescue_initrd();
	clear_bss_section();
	init_kernel_storage_key();
	lockdep_init();
	lockdep_off();
	sort_main_extable();
	setup_lowcore_early();
	detect_machine_type();
	ipl_update_parameters();
	setup_boot_command_line();
	create_kernel_nss();
	detect_mvpg();
	detect_ieee();
	detect_csp();
	detect_diag9c();
	detect_diag44();
	detect_machine_facilities();
	setup_hpage();
	sclp_facilities_detect();
	detect_memory_layout(memory_chunk);
	lockdep_on();
}

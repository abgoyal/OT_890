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
 * arch/blackfin/kernel/bfin_ksyms.c - exports for random symbols
 *
 * Copyright 2004-2008 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#include <linux/module.h>
#include <linux/uaccess.h>

#include <asm/cacheflush.h>

/* Allow people to have their own Blackfin exception handler in a module */
EXPORT_SYMBOL(bfin_return_from_exception);

/* All the Blackfin cache functions: mach-common/cache.S */
EXPORT_SYMBOL(blackfin_dcache_invalidate_range);
EXPORT_SYMBOL(blackfin_icache_dcache_flush_range);
EXPORT_SYMBOL(blackfin_icache_flush_range);
EXPORT_SYMBOL(blackfin_dcache_flush_range);
EXPORT_SYMBOL(blackfin_dflush_page);

/* The following are special because they're not called
 * explicitly (the C compiler generates them).  Fortunately,
 * their interface isn't gonna change any time soon now, so
 * it's OK to leave it out of version control.
 */
EXPORT_SYMBOL(memcpy);
EXPORT_SYMBOL(memset);
EXPORT_SYMBOL(memcmp);
EXPORT_SYMBOL(memmove);
EXPORT_SYMBOL(memchr);

/*
 * libgcc functions - functions that are used internally by the
 * compiler...  (prototypes are not correct though, but that
 * doesn't really matter since they're not versioned).
 */
extern void __ashldi3(void);
extern void __ashrdi3(void);
extern void __smulsi3_highpart(void);
extern void __umulsi3_highpart(void);
extern void __divsi3(void);
extern void __lshrdi3(void);
extern void __modsi3(void);
extern void __muldi3(void);
extern void __udivsi3(void);
extern void __umodsi3(void);
EXPORT_SYMBOL(__ashldi3);
EXPORT_SYMBOL(__ashrdi3);
EXPORT_SYMBOL(__umulsi3_highpart);
EXPORT_SYMBOL(__smulsi3_highpart);
EXPORT_SYMBOL(__divsi3);
EXPORT_SYMBOL(__lshrdi3);
EXPORT_SYMBOL(__modsi3);
EXPORT_SYMBOL(__muldi3);
EXPORT_SYMBOL(__udivsi3);
EXPORT_SYMBOL(__umodsi3);

/* Input/output symbols: lib/{in,out}s.S */
EXPORT_SYMBOL(outsb);
EXPORT_SYMBOL(insb);
EXPORT_SYMBOL(outsw);
EXPORT_SYMBOL(outsw_8);
EXPORT_SYMBOL(insw);
EXPORT_SYMBOL(insw_8);
EXPORT_SYMBOL(outsl);
EXPORT_SYMBOL(insl);
EXPORT_SYMBOL(insl_16);

#ifdef CONFIG_SMP
EXPORT_SYMBOL(__raw_atomic_update_asm);
EXPORT_SYMBOL(__raw_atomic_clear_asm);
EXPORT_SYMBOL(__raw_atomic_set_asm);
EXPORT_SYMBOL(__raw_atomic_xor_asm);
EXPORT_SYMBOL(__raw_atomic_test_asm);
EXPORT_SYMBOL(__raw_xchg_1_asm);
EXPORT_SYMBOL(__raw_xchg_2_asm);
EXPORT_SYMBOL(__raw_xchg_4_asm);
EXPORT_SYMBOL(__raw_cmpxchg_1_asm);
EXPORT_SYMBOL(__raw_cmpxchg_2_asm);
EXPORT_SYMBOL(__raw_cmpxchg_4_asm);
EXPORT_SYMBOL(__raw_spin_is_locked_asm);
EXPORT_SYMBOL(__raw_spin_lock_asm);
EXPORT_SYMBOL(__raw_spin_trylock_asm);
EXPORT_SYMBOL(__raw_spin_unlock_asm);
EXPORT_SYMBOL(__raw_read_lock_asm);
EXPORT_SYMBOL(__raw_read_trylock_asm);
EXPORT_SYMBOL(__raw_read_unlock_asm);
EXPORT_SYMBOL(__raw_write_lock_asm);
EXPORT_SYMBOL(__raw_write_trylock_asm);
EXPORT_SYMBOL(__raw_write_unlock_asm);
EXPORT_SYMBOL(__raw_bit_set_asm);
EXPORT_SYMBOL(__raw_bit_clear_asm);
EXPORT_SYMBOL(__raw_bit_toggle_asm);
EXPORT_SYMBOL(__raw_bit_test_asm);
EXPORT_SYMBOL(__raw_bit_test_set_asm);
EXPORT_SYMBOL(__raw_bit_test_clear_asm);
EXPORT_SYMBOL(__raw_bit_test_toggle_asm);
EXPORT_SYMBOL(__raw_uncached_fetch_asm);
#ifdef __ARCH_SYNC_CORE_DCACHE
EXPORT_SYMBOL(__raw_smp_mark_barrier_asm);
EXPORT_SYMBOL(__raw_smp_check_barrier_asm);
#endif
#endif

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

#ifndef _ASM_X86_IO_64_H
#define _ASM_X86_IO_64_H


/*
 * This file contains the definitions for the x86 IO instructions
 * inb/inw/inl/outb/outw/outl and the "string versions" of the same
 * (insb/insw/insl/outsb/outsw/outsl). You can also use "pausing"
 * versions of the single-IO instructions (inb_p/inw_p/..).
 *
 * This file is not meant to be obfuscating: it's just complicated
 * to (a) handle it all in a way that makes gcc able to optimize it
 * as well as possible and (b) trying to avoid writing the same thing
 * over and over again with slight variations and possibly making a
 * mistake somewhere.
 */

/*
 * Thanks to James van Artsdalen for a better timing-fix than
 * the two short jumps: using outb's to a nonexistent port seems
 * to guarantee better timings even on fast machines.
 *
 * On the other hand, I'd like to be sure of a non-existent port:
 * I feel a bit unsafe about using 0x80 (should be safe, though)
 *
 *		Linus
 */

 /*
  *  Bit simplified and optimized by Jan Hubicka
  *  Support of BIGMEM added by Gerhard Wichert, Siemens AG, July 1999.
  *
  *  isa_memset_io, isa_memcpy_fromio, isa_memcpy_toio added,
  *  isa_read[wl] and isa_write[wl] fixed
  *  - Arnaldo Carvalho de Melo <acme@conectiva.com.br>
  */

extern void native_io_delay(void);

extern int io_delay_type;
extern void io_delay_init(void);

#if defined(CONFIG_PARAVIRT)
#include <asm/paravirt.h>
#else

static inline void slow_down_io(void)
{
	native_io_delay();
#ifdef REALLY_SLOW_IO
	native_io_delay();
	native_io_delay();
	native_io_delay();
#endif
}
#endif

/*
 * Talk about misusing macros..
 */
#define __OUT1(s, x)							\
static inline void out##s(unsigned x value, unsigned short port) {

#define __OUT2(s, s1, s2)				\
asm volatile ("out" #s " %" s1 "0,%" s2 "1"

#ifndef REALLY_SLOW_IO
#define REALLY_SLOW_IO
#define UNSET_REALLY_SLOW_IO
#endif

#define __OUT(s, s1, x)							\
	__OUT1(s, x) __OUT2(s, s1, "w") : : "a" (value), "Nd" (port));	\
	}								\
	__OUT1(s##_p, x) __OUT2(s, s1, "w") : : "a" (value), "Nd" (port)); \
	slow_down_io();							\
}

#define __IN1(s)							\
static inline RETURN_TYPE in##s(unsigned short port)			\
{									\
	RETURN_TYPE _v;

#define __IN2(s, s1, s2)						\
	asm volatile ("in" #s " %" s2 "1,%" s1 "0"

#define __IN(s, s1, i...)						\
	__IN1(s) __IN2(s, s1, "w") : "=a" (_v) : "Nd" (port), ##i);	\
	return _v;							\
	}								\
	__IN1(s##_p) __IN2(s, s1, "w") : "=a" (_v) : "Nd" (port), ##i);	\
	slow_down_io(); \
	return _v; }

#ifdef UNSET_REALLY_SLOW_IO
#undef REALLY_SLOW_IO
#endif

#define __INS(s)							\
static inline void ins##s(unsigned short port, void *addr,		\
			  unsigned long count)				\
{									\
	asm volatile ("rep ; ins" #s					\
		      : "=D" (addr), "=c" (count)			\
		      : "d" (port), "0" (addr), "1" (count));		\
}

#define __OUTS(s)							\
static inline void outs##s(unsigned short port, const void *addr,	\
			   unsigned long count)				\
{									\
	asm volatile ("rep ; outs" #s					\
		      : "=S" (addr), "=c" (count)			\
		      : "d" (port), "0" (addr), "1" (count));		\
}

#define RETURN_TYPE unsigned char
__IN(b, "")
#undef RETURN_TYPE
#define RETURN_TYPE unsigned short
__IN(w, "")
#undef RETURN_TYPE
#define RETURN_TYPE unsigned int
__IN(l, "")
#undef RETURN_TYPE

__OUT(b, "b", char)
__OUT(w, "w", short)
__OUT(l, , int)

__INS(b)
__INS(w)
__INS(l)

__OUTS(b)
__OUTS(w)
__OUTS(l)

#define IO_SPACE_LIMIT 0xffff

#if defined(__KERNEL__) && defined(__x86_64__)

#include <linux/vmalloc.h>

#ifndef __i386__
/*
 * Change virtual addresses to physical addresses and vv.
 * These are pretty trivial
 */
static inline unsigned long virt_to_phys(volatile void *address)
{
	return __pa(address);
}

static inline void *phys_to_virt(unsigned long address)
{
	return __va(address);
}
#endif

/*
 * Change "struct page" to physical address.
 */
#define page_to_phys(page)    ((dma_addr_t)page_to_pfn(page) << PAGE_SHIFT)

#include <asm-generic/iomap.h>

/*
 * This one maps high address device memory and turns off caching for that area.
 * it's useful if some control registers are in such an area and write combining
 * or read caching is not desirable:
 */
extern void __iomem *ioremap_nocache(resource_size_t offset, unsigned long size);
extern void __iomem *ioremap_cache(resource_size_t offset, unsigned long size);
extern void __iomem *ioremap_prot(resource_size_t offset, unsigned long size,
				unsigned long prot_val);

/*
 * The default ioremap() behavior is non-cached:
 */
static inline void __iomem *ioremap(resource_size_t offset, unsigned long size)
{
	return ioremap_nocache(offset, size);
}

extern void iounmap(volatile void __iomem *addr);

extern void __iomem *fix_ioremap(unsigned idx, unsigned long phys);

/*
 * ISA I/O bus memory addresses are 1:1 with the physical address.
 */
#define isa_virt_to_bus virt_to_phys
#define isa_page_to_bus page_to_phys
#define isa_bus_to_virt phys_to_virt

/*
 * However PCI ones are not necessarily 1:1 and therefore these interfaces
 * are forbidden in portable PCI drivers.
 *
 * Allow them on x86 for legacy drivers, though.
 */
#define virt_to_bus virt_to_phys
#define bus_to_virt phys_to_virt

void __memcpy_fromio(void *, unsigned long, unsigned);
void __memcpy_toio(unsigned long, const void *, unsigned);

static inline void memcpy_fromio(void *to, const volatile void __iomem *from,
				 unsigned len)
{
	__memcpy_fromio(to, (unsigned long)from, len);
}

static inline void memcpy_toio(volatile void __iomem *to, const void *from,
			       unsigned len)
{
	__memcpy_toio((unsigned long)to, from, len);
}

void memset_io(volatile void __iomem *a, int b, size_t c);

/*
 * ISA space is 'always mapped' on a typical x86 system, no need to
 * explicitly ioremap() it. The fact that the ISA IO space is mapped
 * to PAGE_OFFSET is pure coincidence - it does not mean ISA values
 * are physical addresses. The following constant pointer can be
 * used as the IO-area pointer (it can be iounmapped as well, so the
 * analogy with PCI is quite large):
 */
#define __ISA_IO_base ((char __iomem *)(PAGE_OFFSET))

#define flush_write_buffers()

/*
 * Convert a virtual cached pointer to an uncached pointer
 */
#define xlate_dev_kmem_ptr(p)	p

#endif /* __KERNEL__ */

#endif /* _ASM_X86_IO_64_H */

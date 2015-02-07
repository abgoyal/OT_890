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

#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/irq.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>

void __iomem *of_ioremap(struct resource *res, unsigned long offset, unsigned long size, char *name)
{
	unsigned long ret = res->start + offset;
	struct resource *r;

	if (res->flags & IORESOURCE_MEM)
		r = request_mem_region(ret, size, name);
	else
		r = request_region(ret, size, name);
	if (!r)
		ret = 0;

	return (void __iomem *) ret;
}
EXPORT_SYMBOL(of_ioremap);

void of_iounmap(struct resource *res, void __iomem *base, unsigned long size)
{
	if (res->flags & IORESOURCE_MEM)
		release_mem_region((unsigned long) base, size);
	else
		release_region((unsigned long) base, size);
}
EXPORT_SYMBOL(of_iounmap);

static int node_match(struct device *dev, void *data)
{
	struct of_device *op = to_of_device(dev);
	struct device_node *dp = data;

	return (op->node == dp);
}

struct of_device *of_find_device_by_node(struct device_node *dp)
{
	struct device *dev = bus_find_device(&of_platform_bus_type, NULL,
					     dp, node_match);

	if (dev)
		return to_of_device(dev);

	return NULL;
}
EXPORT_SYMBOL(of_find_device_by_node);

unsigned int irq_of_parse_and_map(struct device_node *node, int index)
{
	struct of_device *op = of_find_device_by_node(node);

	if (!op || index >= op->num_irqs)
		return 0;

	return op->irqs[index];
}
EXPORT_SYMBOL(irq_of_parse_and_map);

/* Take the archdata values for IOMMU, STC, and HOSTDATA found in
 * BUS and propagate to all child of_device objects.
 */
void of_propagate_archdata(struct of_device *bus)
{
	struct dev_archdata *bus_sd = &bus->dev.archdata;
	struct device_node *bus_dp = bus->node;
	struct device_node *dp;

	for (dp = bus_dp->child; dp; dp = dp->sibling) {
		struct of_device *op = of_find_device_by_node(dp);

		op->dev.archdata.iommu = bus_sd->iommu;
		op->dev.archdata.stc = bus_sd->stc;
		op->dev.archdata.host_controller = bus_sd->host_controller;
		op->dev.archdata.numa_node = bus_sd->numa_node;

		if (dp->child)
			of_propagate_archdata(op);
	}
}

struct bus_type of_platform_bus_type;
EXPORT_SYMBOL(of_platform_bus_type);

static inline u64 of_read_addr(const u32 *cell, int size)
{
	u64 r = 0;
	while (size--)
		r = (r << 32) | *(cell++);
	return r;
}

static void __init get_cells(struct device_node *dp,
			     int *addrc, int *sizec)
{
	if (addrc)
		*addrc = of_n_addr_cells(dp);
	if (sizec)
		*sizec = of_n_size_cells(dp);
}

/* Max address size we deal with */
#define OF_MAX_ADDR_CELLS	4

struct of_bus {
	const char	*name;
	const char	*addr_prop_name;
	int		(*match)(struct device_node *parent);
	void		(*count_cells)(struct device_node *child,
				       int *addrc, int *sizec);
	int		(*map)(u32 *addr, const u32 *range,
			       int na, int ns, int pna);
	unsigned long	(*get_flags)(const u32 *addr, unsigned long);
};

/*
 * Default translator (generic bus)
 */

static void of_bus_default_count_cells(struct device_node *dev,
				       int *addrc, int *sizec)
{
	get_cells(dev, addrc, sizec);
}

/* Make sure the least significant 64-bits are in-range.  Even
 * for 3 or 4 cell values it is a good enough approximation.
 */
static int of_out_of_range(const u32 *addr, const u32 *base,
			   const u32 *size, int na, int ns)
{
	u64 a = of_read_addr(addr, na);
	u64 b = of_read_addr(base, na);

	if (a < b)
		return 1;

	b += of_read_addr(size, ns);
	if (a >= b)
		return 1;

	return 0;
}

static int of_bus_default_map(u32 *addr, const u32 *range,
			      int na, int ns, int pna)
{
	u32 result[OF_MAX_ADDR_CELLS];
	int i;

	if (ns > 2) {
		printk("of_device: Cannot handle size cells (%d) > 2.", ns);
		return -EINVAL;
	}

	if (of_out_of_range(addr, range, range + na + pna, na, ns))
		return -EINVAL;

	/* Start with the parent range base.  */
	memcpy(result, range + na, pna * 4);

	/* Add in the child address offset.  */
	for (i = 0; i < na; i++)
		result[pna - 1 - i] +=
			(addr[na - 1 - i] -
			 range[na - 1 - i]);

	memcpy(addr, result, pna * 4);

	return 0;
}

static unsigned long of_bus_default_get_flags(const u32 *addr, unsigned long flags)
{
	if (flags)
		return flags;
	return IORESOURCE_MEM;
}

/*
 * PCI bus specific translator
 */

static int of_bus_pci_match(struct device_node *np)
{
	if (!strcmp(np->name, "pci")) {
		const char *model = of_get_property(np, "model", NULL);

		if (model && !strcmp(model, "SUNW,simba"))
			return 0;

		/* Do not do PCI specific frobbing if the
		 * PCI bridge lacks a ranges property.  We
		 * want to pass it through up to the next
		 * parent as-is, not with the PCI translate
		 * method which chops off the top address cell.
		 */
		if (!of_find_property(np, "ranges", NULL))
			return 0;

		return 1;
	}

	return 0;
}

static int of_bus_simba_match(struct device_node *np)
{
	const char *model = of_get_property(np, "model", NULL);

	if (model && !strcmp(model, "SUNW,simba"))
		return 1;

	/* Treat PCI busses lacking ranges property just like
	 * simba.
	 */
	if (!strcmp(np->name, "pci")) {
		if (!of_find_property(np, "ranges", NULL))
			return 1;
	}

	return 0;
}

static int of_bus_simba_map(u32 *addr, const u32 *range,
			    int na, int ns, int pna)
{
	return 0;
}

static void of_bus_pci_count_cells(struct device_node *np,
				   int *addrc, int *sizec)
{
	if (addrc)
		*addrc = 3;
	if (sizec)
		*sizec = 2;
}

static int of_bus_pci_map(u32 *addr, const u32 *range,
			  int na, int ns, int pna)
{
	u32 result[OF_MAX_ADDR_CELLS];
	int i;

	/* Check address type match */
	if ((addr[0] ^ range[0]) & 0x03000000)
		return -EINVAL;

	if (of_out_of_range(addr + 1, range + 1, range + na + pna,
			    na - 1, ns))
		return -EINVAL;

	/* Start with the parent range base.  */
	memcpy(result, range + na, pna * 4);

	/* Add in the child address offset, skipping high cell.  */
	for (i = 0; i < na - 1; i++)
		result[pna - 1 - i] +=
			(addr[na - 1 - i] -
			 range[na - 1 - i]);

	memcpy(addr, result, pna * 4);

	return 0;
}

static unsigned long of_bus_pci_get_flags(const u32 *addr, unsigned long flags)
{
	u32 w = addr[0];

	/* For PCI, we override whatever child busses may have used.  */
	flags = 0;
	switch((w >> 24) & 0x03) {
	case 0x01:
		flags |= IORESOURCE_IO;
		break;

	case 0x02: /* 32 bits */
	case 0x03: /* 64 bits */
		flags |= IORESOURCE_MEM;
		break;
	}
	if (w & 0x40000000)
		flags |= IORESOURCE_PREFETCH;
	return flags;
}

/*
 * SBUS bus specific translator
 */

static int of_bus_sbus_match(struct device_node *np)
{
	return !strcmp(np->name, "sbus") ||
		!strcmp(np->name, "sbi");
}

static void of_bus_sbus_count_cells(struct device_node *child,
				   int *addrc, int *sizec)
{
	if (addrc)
		*addrc = 2;
	if (sizec)
		*sizec = 1;
}

/*
 * FHC/Central bus specific translator.
 *
 * This is just needed to hard-code the address and size cell
 * counts.  'fhc' and 'central' nodes lack the #address-cells and
 * #size-cells properties, and if you walk to the root on such
 * Enterprise boxes all you'll get is a #size-cells of 2 which is
 * not what we want to use.
 */
static int of_bus_fhc_match(struct device_node *np)
{
	return !strcmp(np->name, "fhc") ||
		!strcmp(np->name, "central");
}

#define of_bus_fhc_count_cells of_bus_sbus_count_cells

/*
 * Array of bus specific translators
 */

static struct of_bus of_busses[] = {
	/* PCI */
	{
		.name = "pci",
		.addr_prop_name = "assigned-addresses",
		.match = of_bus_pci_match,
		.count_cells = of_bus_pci_count_cells,
		.map = of_bus_pci_map,
		.get_flags = of_bus_pci_get_flags,
	},
	/* SIMBA */
	{
		.name = "simba",
		.addr_prop_name = "assigned-addresses",
		.match = of_bus_simba_match,
		.count_cells = of_bus_pci_count_cells,
		.map = of_bus_simba_map,
		.get_flags = of_bus_pci_get_flags,
	},
	/* SBUS */
	{
		.name = "sbus",
		.addr_prop_name = "reg",
		.match = of_bus_sbus_match,
		.count_cells = of_bus_sbus_count_cells,
		.map = of_bus_default_map,
		.get_flags = of_bus_default_get_flags,
	},
	/* FHC */
	{
		.name = "fhc",
		.addr_prop_name = "reg",
		.match = of_bus_fhc_match,
		.count_cells = of_bus_fhc_count_cells,
		.map = of_bus_default_map,
		.get_flags = of_bus_default_get_flags,
	},
	/* Default */
	{
		.name = "default",
		.addr_prop_name = "reg",
		.match = NULL,
		.count_cells = of_bus_default_count_cells,
		.map = of_bus_default_map,
		.get_flags = of_bus_default_get_flags,
	},
};

static struct of_bus *of_match_bus(struct device_node *np)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(of_busses); i ++)
		if (!of_busses[i].match || of_busses[i].match(np))
			return &of_busses[i];
	BUG();
	return NULL;
}

static int __init build_one_resource(struct device_node *parent,
				     struct of_bus *bus,
				     struct of_bus *pbus,
				     u32 *addr,
				     int na, int ns, int pna)
{
	const u32 *ranges;
	int rone, rlen;

	ranges = of_get_property(parent, "ranges", &rlen);
	if (ranges == NULL || rlen == 0) {
		u32 result[OF_MAX_ADDR_CELLS];
		int i;

		memset(result, 0, pna * 4);
		for (i = 0; i < na; i++)
			result[pna - 1 - i] =
				addr[na - 1 - i];

		memcpy(addr, result, pna * 4);
		return 0;
	}

	/* Now walk through the ranges */
	rlen /= 4;
	rone = na + pna + ns;
	for (; rlen >= rone; rlen -= rone, ranges += rone) {
		if (!bus->map(addr, ranges, na, ns, pna))
			return 0;
	}

	/* When we miss an I/O space match on PCI, just pass it up
	 * to the next PCI bridge and/or controller.
	 */
	if (!strcmp(bus->name, "pci") &&
	    (addr[0] & 0x03000000) == 0x01000000)
		return 0;

	return 1;
}

static int __init use_1to1_mapping(struct device_node *pp)
{
	/* If we have a ranges property in the parent, use it.  */
	if (of_find_property(pp, "ranges", NULL) != NULL)
		return 0;

	/* If the parent is the dma node of an ISA bus, pass
	 * the translation up to the root.
	 *
	 * Some SBUS devices use intermediate nodes to express
	 * hierarchy within the device itself.  These aren't
	 * real bus nodes, and don't have a 'ranges' property.
	 * But, we should still pass the translation work up
	 * to the SBUS itself.
	 */
	if (!strcmp(pp->name, "dma") ||
	    !strcmp(pp->name, "espdma") ||
	    !strcmp(pp->name, "ledma") ||
	    !strcmp(pp->name, "lebuffer"))
		return 0;

	/* Similarly for all PCI bridges, if we get this far
	 * it lacks a ranges property, and this will include
	 * cases like Simba.
	 */
	if (!strcmp(pp->name, "pci"))
		return 0;

	return 1;
}

static int of_resource_verbose;

static void __init build_device_resources(struct of_device *op,
					  struct device *parent)
{
	struct of_device *p_op;
	struct of_bus *bus;
	int na, ns;
	int index, num_reg;
	const void *preg;

	if (!parent)
		return;

	p_op = to_of_device(parent);
	bus = of_match_bus(p_op->node);
	bus->count_cells(op->node, &na, &ns);

	preg = of_get_property(op->node, bus->addr_prop_name, &num_reg);
	if (!preg || num_reg == 0)
		return;

	/* Convert to num-cells.  */
	num_reg /= 4;

	/* Convert to num-entries.  */
	num_reg /= na + ns;

	/* Prevent overrunning the op->resources[] array.  */
	if (num_reg > PROMREG_MAX) {
		printk(KERN_WARNING "%s: Too many regs (%d), "
		       "limiting to %d.\n",
		       op->node->full_name, num_reg, PROMREG_MAX);
		num_reg = PROMREG_MAX;
	}

	for (index = 0; index < num_reg; index++) {
		struct resource *r = &op->resource[index];
		u32 addr[OF_MAX_ADDR_CELLS];
		const u32 *reg = (preg + (index * ((na + ns) * 4)));
		struct device_node *dp = op->node;
		struct device_node *pp = p_op->node;
		struct of_bus *pbus, *dbus;
		u64 size, result = OF_BAD_ADDR;
		unsigned long flags;
		int dna, dns;
		int pna, pns;

		size = of_read_addr(reg + na, ns);
		memcpy(addr, reg, na * 4);

		flags = bus->get_flags(addr, 0);

		if (use_1to1_mapping(pp)) {
			result = of_read_addr(addr, na);
			goto build_res;
		}

		dna = na;
		dns = ns;
		dbus = bus;

		while (1) {
			dp = pp;
			pp = dp->parent;
			if (!pp) {
				result = of_read_addr(addr, dna);
				break;
			}

			pbus = of_match_bus(pp);
			pbus->count_cells(dp, &pna, &pns);

			if (build_one_resource(dp, dbus, pbus, addr,
					       dna, dns, pna))
				break;

			flags = pbus->get_flags(addr, flags);

			dna = pna;
			dns = pns;
			dbus = pbus;
		}

	build_res:
		memset(r, 0, sizeof(*r));

		if (of_resource_verbose)
			printk("%s reg[%d] -> %llx\n",
			       op->node->full_name, index,
			       result);

		if (result != OF_BAD_ADDR) {
			if (tlb_type == hypervisor)
				result &= 0x0fffffffffffffffUL;

			r->start = result;
			r->end = result + size - 1;
			r->flags = flags;
		}
		r->name = op->node->name;
	}
}

static struct device_node * __init
apply_interrupt_map(struct device_node *dp, struct device_node *pp,
		    const u32 *imap, int imlen, const u32 *imask,
		    unsigned int *irq_p)
{
	struct device_node *cp;
	unsigned int irq = *irq_p;
	struct of_bus *bus;
	phandle handle;
	const u32 *reg;
	int na, num_reg, i;

	bus = of_match_bus(pp);
	bus->count_cells(dp, &na, NULL);

	reg = of_get_property(dp, "reg", &num_reg);
	if (!reg || !num_reg)
		return NULL;

	imlen /= ((na + 3) * 4);
	handle = 0;
	for (i = 0; i < imlen; i++) {
		int j;

		for (j = 0; j < na; j++) {
			if ((reg[j] & imask[j]) != imap[j])
				goto next;
		}
		if (imap[na] == irq) {
			handle = imap[na + 1];
			irq = imap[na + 2];
			break;
		}

	next:
		imap += (na + 3);
	}
	if (i == imlen) {
		/* Psycho and Sabre PCI controllers can have 'interrupt-map'
		 * properties that do not include the on-board device
		 * interrupts.  Instead, the device's 'interrupts' property
		 * is already a fully specified INO value.
		 *
		 * Handle this by deciding that, if we didn't get a
		 * match in the parent's 'interrupt-map', and the
		 * parent is an IRQ translater, then use the parent as
		 * our IRQ controller.
		 */
		if (pp->irq_trans)
			return pp;

		return NULL;
	}

	*irq_p = irq;
	cp = of_find_node_by_phandle(handle);

	return cp;
}

static unsigned int __init pci_irq_swizzle(struct device_node *dp,
					   struct device_node *pp,
					   unsigned int irq)
{
	const struct linux_prom_pci_registers *regs;
	unsigned int bus, devfn, slot, ret;

	if (irq < 1 || irq > 4)
		return irq;

	regs = of_get_property(dp, "reg", NULL);
	if (!regs)
		return irq;

	bus = (regs->phys_hi >> 16) & 0xff;
	devfn = (regs->phys_hi >> 8) & 0xff;
	slot = (devfn >> 3) & 0x1f;

	if (pp->irq_trans) {
		/* Derived from Table 8-3, U2P User's Manual.  This branch
		 * is handling a PCI controller that lacks a proper set of
		 * interrupt-map and interrupt-map-mask properties.  The
		 * Ultra-E450 is one example.
		 *
		 * The bit layout is BSSLL, where:
		 * B: 0 on bus A, 1 on bus B
		 * D: 2-bit slot number, derived from PCI device number as
		 *    (dev - 1) for bus A, or (dev - 2) for bus B
		 * L: 2-bit line number
		 */
		if (bus & 0x80) {
			/* PBM-A */
			bus  = 0x00;
			slot = (slot - 1) << 2;
		} else {
			/* PBM-B */
			bus  = 0x10;
			slot = (slot - 2) << 2;
		}
		irq -= 1;

		ret = (bus | slot | irq);
	} else {
		/* Going through a PCI-PCI bridge that lacks a set of
		 * interrupt-map and interrupt-map-mask properties.
		 */
		ret = ((irq - 1 + (slot & 3)) & 3) + 1;
	}

	return ret;
}

static int of_irq_verbose;

static unsigned int __init build_one_device_irq(struct of_device *op,
						struct device *parent,
						unsigned int irq)
{
	struct device_node *dp = op->node;
	struct device_node *pp, *ip;
	unsigned int orig_irq = irq;
	int nid;

	if (irq == 0xffffffff)
		return irq;

	if (dp->irq_trans) {
		irq = dp->irq_trans->irq_build(dp, irq,
					       dp->irq_trans->data);

		if (of_irq_verbose)
			printk("%s: direct translate %x --> %x\n",
			       dp->full_name, orig_irq, irq);

		goto out;
	}

	/* Something more complicated.  Walk up to the root, applying
	 * interrupt-map or bus specific translations, until we hit
	 * an IRQ translator.
	 *
	 * If we hit a bus type or situation we cannot handle, we
	 * stop and assume that the original IRQ number was in a
	 * format which has special meaning to it's immediate parent.
	 */
	pp = dp->parent;
	ip = NULL;
	while (pp) {
		const void *imap, *imsk;
		int imlen;

		imap = of_get_property(pp, "interrupt-map", &imlen);
		imsk = of_get_property(pp, "interrupt-map-mask", NULL);
		if (imap && imsk) {
			struct device_node *iret;
			int this_orig_irq = irq;

			iret = apply_interrupt_map(dp, pp,
						   imap, imlen, imsk,
						   &irq);

			if (of_irq_verbose)
				printk("%s: Apply [%s:%x] imap --> [%s:%x]\n",
				       op->node->full_name,
				       pp->full_name, this_orig_irq,
				       (iret ? iret->full_name : "NULL"), irq);

			if (!iret)
				break;

			if (iret->irq_trans) {
				ip = iret;
				break;
			}
		} else {
			if (!strcmp(pp->name, "pci")) {
				unsigned int this_orig_irq = irq;

				irq = pci_irq_swizzle(dp, pp, irq);
				if (of_irq_verbose)
					printk("%s: PCI swizzle [%s] "
					       "%x --> %x\n",
					       op->node->full_name,
					       pp->full_name, this_orig_irq,
					       irq);

			}

			if (pp->irq_trans) {
				ip = pp;
				break;
			}
		}
		dp = pp;
		pp = pp->parent;
	}
	if (!ip)
		return orig_irq;

	irq = ip->irq_trans->irq_build(op->node, irq,
				       ip->irq_trans->data);
	if (of_irq_verbose)
		printk("%s: Apply IRQ trans [%s] %x --> %x\n",
		       op->node->full_name, ip->full_name, orig_irq, irq);

out:
	nid = of_node_to_nid(dp);
	if (nid != -1) {
		cpumask_t numa_mask = *cpumask_of_node(nid);

		irq_set_affinity(irq, &numa_mask);
	}

	return irq;
}

static struct of_device * __init scan_one_device(struct device_node *dp,
						 struct device *parent)
{
	struct of_device *op = kzalloc(sizeof(*op), GFP_KERNEL);
	const unsigned int *irq;
	struct dev_archdata *sd;
	int len, i;

	if (!op)
		return NULL;

	sd = &op->dev.archdata;
	sd->prom_node = dp;
	sd->op = op;

	op->node = dp;

	op->clock_freq = of_getintprop_default(dp, "clock-frequency",
					       (25*1000*1000));
	op->portid = of_getintprop_default(dp, "upa-portid", -1);
	if (op->portid == -1)
		op->portid = of_getintprop_default(dp, "portid", -1);

	irq = of_get_property(dp, "interrupts", &len);
	if (irq) {
		op->num_irqs = len / 4;

		/* Prevent overrunning the op->irqs[] array.  */
		if (op->num_irqs > PROMINTR_MAX) {
			printk(KERN_WARNING "%s: Too many irqs (%d), "
			       "limiting to %d.\n",
			       dp->full_name, op->num_irqs, PROMINTR_MAX);
			op->num_irqs = PROMINTR_MAX;
		}
		memcpy(op->irqs, irq, op->num_irqs * 4);
	} else {
		op->num_irqs = 0;
	}

	build_device_resources(op, parent);
	for (i = 0; i < op->num_irqs; i++)
		op->irqs[i] = build_one_device_irq(op, parent, op->irqs[i]);

	op->dev.parent = parent;
	op->dev.bus = &of_platform_bus_type;
	if (!parent)
		dev_set_name(&op->dev, "root");
	else
		dev_set_name(&op->dev, "%08x", dp->node);

	if (of_device_register(op)) {
		printk("%s: Could not register of device.\n",
		       dp->full_name);
		kfree(op);
		op = NULL;
	}

	return op;
}

static void __init scan_tree(struct device_node *dp, struct device *parent)
{
	while (dp) {
		struct of_device *op = scan_one_device(dp, parent);

		if (op)
			scan_tree(dp->child, &op->dev);

		dp = dp->sibling;
	}
}

static void __init scan_of_devices(void)
{
	struct device_node *root = of_find_node_by_path("/");
	struct of_device *parent;

	parent = scan_one_device(root, NULL);
	if (!parent)
		return;

	scan_tree(root->child, &parent->dev);
}

static int __init of_bus_driver_init(void)
{
	int err;

	err = of_bus_type_init(&of_platform_bus_type, "of");
	if (!err)
		scan_of_devices();

	return err;
}

postcore_initcall(of_bus_driver_init);

static int __init of_debug(char *str)
{
	int val = 0;

	get_option(&str, &val);
	if (val & 1)
		of_resource_verbose = 1;
	if (val & 2)
		of_irq_verbose = 1;
	return 1;
}

__setup("of_debug=", of_debug);

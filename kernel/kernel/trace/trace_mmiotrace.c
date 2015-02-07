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
 * Memory mapped I/O tracing
 *
 * Copyright (C) 2008 Pekka Paalanen <pq@iki.fi>
 */

#define DEBUG 1

#include <linux/kernel.h>
#include <linux/mmiotrace.h>
#include <linux/pci.h>
#include <asm/atomic.h>

#include "trace.h"

struct header_iter {
	struct pci_dev *dev;
};

static struct trace_array *mmio_trace_array;
static bool overrun_detected;
static unsigned long prev_overruns;
static atomic_t dropped_count;

static void mmio_reset_data(struct trace_array *tr)
{
	overrun_detected = false;
	prev_overruns = 0;

	tracing_reset_online_cpus(tr);
}

static int mmio_trace_init(struct trace_array *tr)
{
	pr_debug("in %s\n", __func__);
	mmio_trace_array = tr;

	mmio_reset_data(tr);
	enable_mmiotrace();
	return 0;
}

static void mmio_trace_reset(struct trace_array *tr)
{
	pr_debug("in %s\n", __func__);

	disable_mmiotrace();
	mmio_reset_data(tr);
	mmio_trace_array = NULL;
}

static void mmio_trace_start(struct trace_array *tr)
{
	pr_debug("in %s\n", __func__);
	mmio_reset_data(tr);
}

static int mmio_print_pcidev(struct trace_seq *s, const struct pci_dev *dev)
{
	int ret = 0;
	int i;
	resource_size_t start, end;
	const struct pci_driver *drv = pci_dev_driver(dev);

	/* XXX: incomplete checks for trace_seq_printf() return value */
	ret += trace_seq_printf(s, "PCIDEV %02x%02x %04x%04x %x",
				dev->bus->number, dev->devfn,
				dev->vendor, dev->device, dev->irq);
	/*
	 * XXX: is pci_resource_to_user() appropriate, since we are
	 * supposed to interpret the __ioremap() phys_addr argument based on
	 * these printed values?
	 */
	for (i = 0; i < 7; i++) {
		pci_resource_to_user(dev, i, &dev->resource[i], &start, &end);
		ret += trace_seq_printf(s, " %llx",
			(unsigned long long)(start |
			(dev->resource[i].flags & PCI_REGION_FLAG_MASK)));
	}
	for (i = 0; i < 7; i++) {
		pci_resource_to_user(dev, i, &dev->resource[i], &start, &end);
		ret += trace_seq_printf(s, " %llx",
			dev->resource[i].start < dev->resource[i].end ?
			(unsigned long long)(end - start) + 1 : 0);
	}
	if (drv)
		ret += trace_seq_printf(s, " %s\n", drv->name);
	else
		ret += trace_seq_printf(s, " \n");
	return ret;
}

static void destroy_header_iter(struct header_iter *hiter)
{
	if (!hiter)
		return;
	pci_dev_put(hiter->dev);
	kfree(hiter);
}

static void mmio_pipe_open(struct trace_iterator *iter)
{
	struct header_iter *hiter;
	struct trace_seq *s = &iter->seq;

	trace_seq_printf(s, "VERSION 20070824\n");

	hiter = kzalloc(sizeof(*hiter), GFP_KERNEL);
	if (!hiter)
		return;

	hiter->dev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, NULL);
	iter->private = hiter;
}

/* XXX: This is not called when the pipe is closed! */
static void mmio_close(struct trace_iterator *iter)
{
	struct header_iter *hiter = iter->private;
	destroy_header_iter(hiter);
	iter->private = NULL;
}

static unsigned long count_overruns(struct trace_iterator *iter)
{
	unsigned long cnt = atomic_xchg(&dropped_count, 0);
	unsigned long over = ring_buffer_overruns(iter->tr->buffer);

	if (over > prev_overruns)
		cnt += over - prev_overruns;
	prev_overruns = over;
	return cnt;
}

static ssize_t mmio_read(struct trace_iterator *iter, struct file *filp,
				char __user *ubuf, size_t cnt, loff_t *ppos)
{
	ssize_t ret;
	struct header_iter *hiter = iter->private;
	struct trace_seq *s = &iter->seq;
	unsigned long n;

	n = count_overruns(iter);
	if (n) {
		/* XXX: This is later than where events were lost. */
		trace_seq_printf(s, "MARK 0.000000 Lost %lu events.\n", n);
		if (!overrun_detected)
			pr_warning("mmiotrace has lost events.\n");
		overrun_detected = true;
		goto print_out;
	}

	if (!hiter)
		return 0;

	mmio_print_pcidev(s, hiter->dev);
	hiter->dev = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, hiter->dev);

	if (!hiter->dev) {
		destroy_header_iter(hiter);
		iter->private = NULL;
	}

print_out:
	ret = trace_seq_to_user(s, ubuf, cnt);
	return (ret == -EBUSY) ? 0 : ret;
}

static enum print_line_t mmio_print_rw(struct trace_iterator *iter)
{
	struct trace_entry *entry = iter->ent;
	struct trace_mmiotrace_rw *field;
	struct mmiotrace_rw *rw;
	struct trace_seq *s	= &iter->seq;
	unsigned long long t	= ns2usecs(iter->ts);
	unsigned long usec_rem	= do_div(t, 1000000ULL);
	unsigned secs		= (unsigned long)t;
	int ret = 1;

	trace_assign_type(field, entry);
	rw = &field->rw;

	switch (rw->opcode) {
	case MMIO_READ:
		ret = trace_seq_printf(s,
			"R %d %lu.%06lu %d 0x%llx 0x%lx 0x%lx %d\n",
			rw->width, secs, usec_rem, rw->map_id,
			(unsigned long long)rw->phys,
			rw->value, rw->pc, 0);
		break;
	case MMIO_WRITE:
		ret = trace_seq_printf(s,
			"W %d %lu.%06lu %d 0x%llx 0x%lx 0x%lx %d\n",
			rw->width, secs, usec_rem, rw->map_id,
			(unsigned long long)rw->phys,
			rw->value, rw->pc, 0);
		break;
	case MMIO_UNKNOWN_OP:
		ret = trace_seq_printf(s,
			"UNKNOWN %lu.%06lu %d 0x%llx %02x,%02x,%02x 0x%lx %d\n",
			secs, usec_rem, rw->map_id,
			(unsigned long long)rw->phys,
			(rw->value >> 16) & 0xff, (rw->value >> 8) & 0xff,
			(rw->value >> 0) & 0xff, rw->pc, 0);
		break;
	default:
		ret = trace_seq_printf(s, "rw what?\n");
		break;
	}
	if (ret)
		return TRACE_TYPE_HANDLED;
	return TRACE_TYPE_PARTIAL_LINE;
}

static enum print_line_t mmio_print_map(struct trace_iterator *iter)
{
	struct trace_entry *entry = iter->ent;
	struct trace_mmiotrace_map *field;
	struct mmiotrace_map *m;
	struct trace_seq *s	= &iter->seq;
	unsigned long long t	= ns2usecs(iter->ts);
	unsigned long usec_rem	= do_div(t, 1000000ULL);
	unsigned secs		= (unsigned long)t;
	int ret;

	trace_assign_type(field, entry);
	m = &field->map;

	switch (m->opcode) {
	case MMIO_PROBE:
		ret = trace_seq_printf(s,
			"MAP %lu.%06lu %d 0x%llx 0x%lx 0x%lx 0x%lx %d\n",
			secs, usec_rem, m->map_id,
			(unsigned long long)m->phys, m->virt, m->len,
			0UL, 0);
		break;
	case MMIO_UNPROBE:
		ret = trace_seq_printf(s,
			"UNMAP %lu.%06lu %d 0x%lx %d\n",
			secs, usec_rem, m->map_id, 0UL, 0);
		break;
	default:
		ret = trace_seq_printf(s, "map what?\n");
		break;
	}
	if (ret)
		return TRACE_TYPE_HANDLED;
	return TRACE_TYPE_PARTIAL_LINE;
}

static enum print_line_t mmio_print_mark(struct trace_iterator *iter)
{
	struct trace_entry *entry = iter->ent;
	struct print_entry *print = (struct print_entry *)entry;
	const char *msg		= print->buf;
	struct trace_seq *s	= &iter->seq;
	unsigned long long t	= ns2usecs(iter->ts);
	unsigned long usec_rem	= do_div(t, 1000000ULL);
	unsigned secs		= (unsigned long)t;
	int ret;

	/* The trailing newline must be in the message. */
	ret = trace_seq_printf(s, "MARK %lu.%06lu %s", secs, usec_rem, msg);
	if (!ret)
		return TRACE_TYPE_PARTIAL_LINE;

	if (entry->flags & TRACE_FLAG_CONT)
		trace_seq_print_cont(s, iter);

	return TRACE_TYPE_HANDLED;
}

static enum print_line_t mmio_print_line(struct trace_iterator *iter)
{
	switch (iter->ent->type) {
	case TRACE_MMIO_RW:
		return mmio_print_rw(iter);
	case TRACE_MMIO_MAP:
		return mmio_print_map(iter);
	case TRACE_PRINT:
		return mmio_print_mark(iter);
	default:
		return TRACE_TYPE_HANDLED; /* ignore unknown entries */
	}
}

static struct tracer mmio_tracer __read_mostly =
{
	.name		= "mmiotrace",
	.init		= mmio_trace_init,
	.reset		= mmio_trace_reset,
	.start		= mmio_trace_start,
	.pipe_open	= mmio_pipe_open,
	.close		= mmio_close,
	.read		= mmio_read,
	.print_line	= mmio_print_line,
};

__init static int init_mmio_trace(void)
{
	return register_tracer(&mmio_tracer);
}
device_initcall(init_mmio_trace);

static void __trace_mmiotrace_rw(struct trace_array *tr,
				struct trace_array_cpu *data,
				struct mmiotrace_rw *rw)
{
	struct ring_buffer_event *event;
	struct trace_mmiotrace_rw *entry;
	unsigned long irq_flags;

	event	= ring_buffer_lock_reserve(tr->buffer, sizeof(*entry),
					   &irq_flags);
	if (!event) {
		atomic_inc(&dropped_count);
		return;
	}
	entry	= ring_buffer_event_data(event);
	tracing_generic_entry_update(&entry->ent, 0, preempt_count());
	entry->ent.type			= TRACE_MMIO_RW;
	entry->rw			= *rw;
	ring_buffer_unlock_commit(tr->buffer, event, irq_flags);

	trace_wake_up();
}

void mmio_trace_rw(struct mmiotrace_rw *rw)
{
	struct trace_array *tr = mmio_trace_array;
	struct trace_array_cpu *data = tr->data[smp_processor_id()];
	__trace_mmiotrace_rw(tr, data, rw);
}

static void __trace_mmiotrace_map(struct trace_array *tr,
				struct trace_array_cpu *data,
				struct mmiotrace_map *map)
{
	struct ring_buffer_event *event;
	struct trace_mmiotrace_map *entry;
	unsigned long irq_flags;

	event	= ring_buffer_lock_reserve(tr->buffer, sizeof(*entry),
					   &irq_flags);
	if (!event) {
		atomic_inc(&dropped_count);
		return;
	}
	entry	= ring_buffer_event_data(event);
	tracing_generic_entry_update(&entry->ent, 0, preempt_count());
	entry->ent.type			= TRACE_MMIO_MAP;
	entry->map			= *map;
	ring_buffer_unlock_commit(tr->buffer, event, irq_flags);

	trace_wake_up();
}

void mmio_trace_mapping(struct mmiotrace_map *map)
{
	struct trace_array *tr = mmio_trace_array;
	struct trace_array_cpu *data;

	preempt_disable();
	data = tr->data[smp_processor_id()];
	__trace_mmiotrace_map(tr, data, map);
	preempt_enable();
}

int mmio_trace_printk(const char *fmt, va_list args)
{
	return trace_vprintk(0, -1, fmt, args);
}

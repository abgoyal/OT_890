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
    include/linux/comedidev.h
    header file for kernel-only structures, variables, and constants

    COMEDI - Linux Control and Measurement Device Interface
    Copyright (C) 1997-2000 David A. Schleef <ds@schleef.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef _COMEDIDEV_H
#define _COMEDIDEV_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include "interrupt.h"
#include <linux/dma-mapping.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#include "comedi.h"

#define DPRINTK(format, args...)	do {		\
	if (comedi_debug)				\
		printk(KERN_DEBUG "comedi: " format , ## args);	\
} while (0)

#define COMEDI_VERSION(a, b, c) (((a) << 16) + ((b) << 8) + (c))
#define COMEDI_VERSION_CODE COMEDI_VERSION(COMEDI_MAJORVERSION, COMEDI_MINORVERSION, COMEDI_MICROVERSION)
#define COMEDI_RELEASE VERSION

#define COMEDI_INITCLEANUP_NOMODULE(x)					\
	static int __init x ## _init_module(void)			\
		{return comedi_driver_register(&(x));}			\
	static void __exit x ## _cleanup_module(void)			\
		{comedi_driver_unregister(&(x));} 			\
	module_init(x ## _init_module);					\
	module_exit(x ## _cleanup_module);					\

#define COMEDI_MODULE_MACROS						\
	MODULE_AUTHOR("Comedi http://www.comedi.org");		\
	MODULE_DESCRIPTION("Comedi low-level driver");			\
	MODULE_LICENSE("GPL");						\

#define COMEDI_INITCLEANUP(x)						\
	COMEDI_MODULE_MACROS		\
	COMEDI_INITCLEANUP_NOMODULE(x)

#define COMEDI_PCI_INITCLEANUP_NOMODULE(comedi_driver, pci_id_table) \
	static int __devinit comedi_driver ## _pci_probe(struct pci_dev *dev, \
		const struct pci_device_id *ent) \
	{ \
		return comedi_pci_auto_config(dev, comedi_driver.driver_name); \
	} \
	static void __devexit comedi_driver ## _pci_remove(struct pci_dev *dev) \
	{ \
		comedi_pci_auto_unconfig(dev); \
	} \
	static struct pci_driver comedi_driver ## _pci_driver = \
	{ \
		.id_table = pci_id_table, \
		.probe = &comedi_driver ## _pci_probe, \
		.remove = __devexit_p(&comedi_driver ## _pci_remove) \
	}; \
	static int __init comedi_driver ## _init_module(void) \
	{ \
		int retval; \
		retval = comedi_driver_register(&comedi_driver); \
		if (retval < 0) \
			return retval; \
		comedi_driver ## _pci_driver.name = (char *)comedi_driver.driver_name; \
		return pci_register_driver(&comedi_driver ## _pci_driver); \
	} \
	static void __exit comedi_driver ## _cleanup_module(void) \
	{ \
		pci_unregister_driver(&comedi_driver ## _pci_driver); \
		comedi_driver_unregister(&comedi_driver); \
	} \
	module_init(comedi_driver ## _init_module); \
	module_exit(comedi_driver ## _cleanup_module);

#define COMEDI_PCI_INITCLEANUP(comedi_driver, pci_id_table) \
	COMEDI_MODULE_MACROS \
	COMEDI_PCI_INITCLEANUP_NOMODULE(comedi_driver, pci_id_table)

#define PCI_VENDOR_ID_INOVA		0x104c
#define PCI_VENDOR_ID_NATINST		0x1093
#define PCI_VENDOR_ID_DATX		0x1116
#define PCI_VENDOR_ID_COMPUTERBOARDS	0x1307
#define PCI_VENDOR_ID_ADVANTECH		0x13fe
#define PCI_VENDOR_ID_RTD		0x1435
#define PCI_VENDOR_ID_AMPLICON		0x14dc
#define PCI_VENDOR_ID_ADLINK		0x144a
#define PCI_VENDOR_ID_ICP		0x104c
#define PCI_VENDOR_ID_CONTEC		0x1221
#define PCI_VENDOR_ID_MEILHAUS		0x1402

#define COMEDI_NUM_MINORS 0x100
#define COMEDI_NUM_LEGACY_MINORS 0x10
#define COMEDI_NUM_BOARD_MINORS 0x30
#define COMEDI_FIRST_SUBDEVICE_MINOR COMEDI_NUM_BOARD_MINORS

typedef struct comedi_device_struct comedi_device;
typedef struct comedi_subdevice_struct comedi_subdevice;
typedef struct comedi_async_struct comedi_async;
typedef struct comedi_driver_struct comedi_driver;
typedef struct comedi_lrange_struct comedi_lrange;

typedef struct device device_create_result_type;

#define COMEDI_DEVICE_CREATE(cs, parent, devt, drvdata, device, fmt...) \
	device_create(cs, ((parent) ? (parent) : (device)), devt, drvdata, fmt)

struct comedi_subdevice_struct {
	comedi_device *device;
	int type;
	int n_chan;
	volatile int subdev_flags;
	int len_chanlist;	/* maximum length of channel/gain list */

	void *private;

	comedi_async *async;

	void *lock;
	void *busy;
	unsigned runflags;
	spinlock_t spin_lock;

	int io_bits;

	lsampl_t maxdata;	/* if maxdata==0, use list */
	const lsampl_t *maxdata_list;	/* list is channel specific */

	unsigned int flags;
	const unsigned int *flaglist;

	unsigned int settling_time_0;

	const comedi_lrange *range_table;
	const comedi_lrange *const *range_table_list;

	unsigned int *chanlist;	/* driver-owned chanlist (not used) */

	int (*insn_read) (comedi_device *, comedi_subdevice *, comedi_insn *,
		lsampl_t *);
	int (*insn_write) (comedi_device *, comedi_subdevice *, comedi_insn *,
		lsampl_t *);
	int (*insn_bits) (comedi_device *, comedi_subdevice *, comedi_insn *,
		lsampl_t *);
	int (*insn_config) (comedi_device *, comedi_subdevice *, comedi_insn *,
		lsampl_t *);

	int (*do_cmd) (comedi_device *, comedi_subdevice *);
	int (*do_cmdtest) (comedi_device *, comedi_subdevice *, comedi_cmd *);
	int (*poll) (comedi_device *, comedi_subdevice *);
	int (*cancel) (comedi_device *, comedi_subdevice *);
	/* int (*do_lock)(comedi_device *,comedi_subdevice *); */
	/* int (*do_unlock)(comedi_device *,comedi_subdevice *); */

	/* called when the buffer changes */
	int (*buf_change) (comedi_device *dev, comedi_subdevice *s,
		unsigned long new_size);

	void (*munge) (comedi_device *dev, comedi_subdevice *s, void *data,
		unsigned int num_bytes, unsigned int start_chan_index);
	enum dma_data_direction async_dma_dir;

	unsigned int state;

	device_create_result_type *class_dev;
	int minor;
};

struct comedi_buf_page {
	void *virt_addr;
	dma_addr_t dma_addr;
};

struct comedi_async_struct {
	comedi_subdevice *subdevice;

	void *prealloc_buf;	/* pre-allocated buffer */
	unsigned int prealloc_bufsz;	/* buffer size, in bytes */
	struct comedi_buf_page *buf_page_list;	/* virtual and dma address of each page */
	unsigned n_buf_pages;	/* num elements in buf_page_list */

	unsigned int max_bufsize;	/* maximum buffer size, bytes */
	unsigned int mmap_count;	/* current number of mmaps of prealloc_buf */

	unsigned int buf_write_count;	/* byte count for writer (write completed) */
	unsigned int buf_write_alloc_count;	/* byte count for writer (allocated for writing) */
	unsigned int buf_read_count;	/* byte count for reader (read completed) */
	unsigned int buf_read_alloc_count;	/* byte count for reader (allocated for reading) */

	unsigned int buf_write_ptr;	/* buffer marker for writer */
	unsigned int buf_read_ptr;	/* buffer marker for reader */

	unsigned int cur_chan;	/* useless channel marker for interrupt */
	/* number of bytes that have been received for current scan */
	unsigned int scan_progress;
	/* keeps track of where we are in chanlist as for munging */
	unsigned int munge_chan;
	/* number of bytes that have been munged */
	unsigned int munge_count;
	/* buffer marker for munging */
	unsigned int munge_ptr;

	unsigned int events;	/* events that have occurred */

	comedi_cmd cmd;

	wait_queue_head_t wait_head;

	/* callback stuff */
	unsigned int cb_mask;
	int (*cb_func) (unsigned int flags, void *);
	void *cb_arg;

	int (*inttrig) (comedi_device *dev, comedi_subdevice *s,
			unsigned int x);
};

struct comedi_driver_struct {
	struct comedi_driver_struct *next;

	const char *driver_name;
	struct module *module;
	int (*attach) (comedi_device *, comedi_devconfig *);
	int (*detach) (comedi_device *);

	/* number of elements in board_name and board_id arrays */
	unsigned int num_names;
	const char *const *board_name;
	/* offset in bytes from one board name pointer to the next */
	int offset;
};

struct comedi_device_struct {
	int use_count;
	comedi_driver *driver;
	void *private;

	device_create_result_type *class_dev;
	int minor;
	/* hw_dev is passed to dma_alloc_coherent when allocating async buffers
	 * for subdevices that have async_dma_dir set to something other than
	 * DMA_NONE */
	struct device *hw_dev;

	const char *board_name;
	const void *board_ptr;
	int attached;
	int rt;
	spinlock_t spinlock;
	struct mutex mutex;
	int in_request_module;

	int n_subdevices;
	comedi_subdevice *subdevices;

	/* dumb */
	unsigned long iobase;
	unsigned int irq;

	comedi_subdevice *read_subdev;
	comedi_subdevice *write_subdev;

	struct fasync_struct *async_queue;

	void (*open) (comedi_device *dev);
	void (*close) (comedi_device *dev);
};

struct comedi_device_file_info {
	comedi_device *device;
	comedi_subdevice *read_subdevice;
	comedi_subdevice *write_subdevice;
};

#ifdef CONFIG_COMEDI_DEBUG
extern int comedi_debug;
#else
static const int comedi_debug;
#endif

/*
 * function prototypes
 */

void comedi_event(comedi_device *dev, comedi_subdevice *s);
void comedi_error(const comedi_device *dev, const char *s);

/* we can expand the number of bits used to encode devices/subdevices into
 the minor number soon, after more distros support > 8 bit minor numbers
 (like after Debian Etch gets released) */
enum comedi_minor_bits {
	COMEDI_DEVICE_MINOR_MASK = 0xf,
	COMEDI_SUBDEVICE_MINOR_MASK = 0xf0
};
static const unsigned COMEDI_SUBDEVICE_MINOR_SHIFT = 4;
static const unsigned COMEDI_SUBDEVICE_MINOR_OFFSET = 1;

struct comedi_device_file_info *comedi_get_device_file_info(unsigned minor);

static inline comedi_subdevice *comedi_get_read_subdevice(
				const struct comedi_device_file_info *info)
{
	if (info->read_subdevice)
		return info->read_subdevice;
	if (info->device == NULL)
		return NULL;
	return info->device->read_subdev;
}

static inline comedi_subdevice *comedi_get_write_subdevice(
				const struct comedi_device_file_info *info)
{
	if (info->write_subdevice)
		return info->write_subdevice;
	if (info->device == NULL)
		return NULL;
	return info->device->write_subdev;
}

void comedi_device_detach(comedi_device *dev);
int comedi_device_attach(comedi_device *dev, comedi_devconfig *it);
int comedi_driver_register(comedi_driver *);
int comedi_driver_unregister(comedi_driver *);

void init_polling(void);
void cleanup_polling(void);
void start_polling(comedi_device *);
void stop_polling(comedi_device *);

int comedi_buf_alloc(comedi_device *dev, comedi_subdevice *s, unsigned long
	new_size);

#ifdef CONFIG_PROC_FS
void comedi_proc_init(void);
void comedi_proc_cleanup(void);
#else
static inline void comedi_proc_init(void)
{
}
static inline void comedi_proc_cleanup(void)
{
}
#endif

/* subdevice runflags */
enum subdevice_runflags {
	SRF_USER = 0x00000001,
	SRF_RT = 0x00000002,
	/* indicates an COMEDI_CB_ERROR event has occurred since the last
	 * command was started */
	SRF_ERROR = 0x00000004,
	SRF_RUNNING = 0x08000000
};

/*
   various internal comedi functions
 */

int do_rangeinfo_ioctl(comedi_device *dev, comedi_rangeinfo *arg);
int check_chanlist(comedi_subdevice *s, int n, unsigned int *chanlist);
void comedi_set_subdevice_runflags(comedi_subdevice *s, unsigned mask,
	unsigned bits);
unsigned comedi_get_subdevice_runflags(comedi_subdevice *s);
int insn_inval(comedi_device *dev, comedi_subdevice *s,
	comedi_insn *insn, lsampl_t *data);

/* range stuff */

#define RANGE(a, b)		{(a)*1e6, (b)*1e6, 0}
#define RANGE_ext(a, b)		{(a)*1e6, (b)*1e6, RF_EXTERNAL}
#define RANGE_mA(a, b)		{(a)*1e6, (b)*1e6, UNIT_mA}
#define RANGE_unitless(a, b)	{(a)*1e6, (b)*1e6, 0}	/* XXX */
#define BIP_RANGE(a)		{-(a)*1e6, (a)*1e6, 0}
#define UNI_RANGE(a)		{0, (a)*1e6, 0}

extern const comedi_lrange range_bipolar10;
extern const comedi_lrange range_bipolar5;
extern const comedi_lrange range_bipolar2_5;
extern const comedi_lrange range_unipolar10;
extern const comedi_lrange range_unipolar5;
extern const comedi_lrange range_unknown;

#define range_digital		range_unipolar5

#if __GNUC__ >= 3
#define GCC_ZERO_LENGTH_ARRAY
#else
#define GCC_ZERO_LENGTH_ARRAY 0
#endif

struct comedi_lrange_struct {
	int length;
	comedi_krange range[GCC_ZERO_LENGTH_ARRAY];
};

/* some silly little inline functions */

static inline int alloc_subdevices(comedi_device *dev,
				   unsigned int num_subdevices)
{
	unsigned i;

	dev->n_subdevices = num_subdevices;
	dev->subdevices =
		kcalloc(num_subdevices, sizeof(comedi_subdevice), GFP_KERNEL);
	if (!dev->subdevices)
		return -ENOMEM;
	for (i = 0; i < num_subdevices; ++i) {
		dev->subdevices[i].device = dev;
		dev->subdevices[i].async_dma_dir = DMA_NONE;
		spin_lock_init(&dev->subdevices[i].spin_lock);
		dev->subdevices[i].minor = -1;
	}
	return 0;
}

static inline int alloc_private(comedi_device *dev, int size)
{
	dev->private = kzalloc(size, GFP_KERNEL);
	if (!dev->private)
		return -ENOMEM;
	return 0;
}

static inline unsigned int bytes_per_sample(const comedi_subdevice *subd)
{
	if (subd->subdev_flags & SDF_LSAMPL)
		return sizeof(lsampl_t);
	else
		return sizeof(sampl_t);
}

/* must be used in attach to set dev->hw_dev if you wish to dma directly
into comedi's buffer */
static inline void comedi_set_hw_dev(comedi_device *dev, struct device *hw_dev)
{
	if (dev->hw_dev)
		put_device(dev->hw_dev);

	dev->hw_dev = hw_dev;
	if (dev->hw_dev) {
		dev->hw_dev = get_device(dev->hw_dev);
		BUG_ON(dev->hw_dev == NULL);
	}
}

int comedi_buf_put(comedi_async *async, sampl_t x);
int comedi_buf_get(comedi_async *async, sampl_t *x);

unsigned int comedi_buf_write_n_available(comedi_async *async);
unsigned int comedi_buf_write_alloc(comedi_async *async, unsigned int nbytes);
unsigned int comedi_buf_write_alloc_strict(comedi_async *async,
	unsigned int nbytes);
unsigned comedi_buf_write_free(comedi_async *async, unsigned int nbytes);
unsigned comedi_buf_read_alloc(comedi_async *async, unsigned nbytes);
unsigned comedi_buf_read_free(comedi_async *async, unsigned int nbytes);
unsigned int comedi_buf_read_n_available(comedi_async *async);
void comedi_buf_memcpy_to(comedi_async *async, unsigned int offset,
	const void *source, unsigned int num_bytes);
void comedi_buf_memcpy_from(comedi_async *async, unsigned int offset,
	void *destination, unsigned int num_bytes);
static inline unsigned comedi_buf_write_n_allocated(comedi_async *async)
{
	return async->buf_write_alloc_count - async->buf_write_count;
}
static inline unsigned comedi_buf_read_n_allocated(comedi_async *async)
{
	return async->buf_read_alloc_count - async->buf_read_count;
}

void comedi_reset_async_buf(comedi_async *async);

static inline void *comedi_aux_data(int options[], int n)
{
	unsigned long address;
	unsigned long addressLow;
	int bit_shift;
	if (sizeof(int) >= sizeof(void *))
		address = options[COMEDI_DEVCONF_AUX_DATA_LO];
	else {
		address = options[COMEDI_DEVCONF_AUX_DATA_HI];
		bit_shift = sizeof(int) * 8;
		address <<= bit_shift;
		addressLow = options[COMEDI_DEVCONF_AUX_DATA_LO];
		addressLow &= (1UL << bit_shift) - 1;
		address |= addressLow;
	}
	if (n >= 1)
		address += options[COMEDI_DEVCONF_AUX_DATA0_LENGTH];
	if (n >= 2)
		address += options[COMEDI_DEVCONF_AUX_DATA1_LENGTH];
	if (n >= 3)
		address += options[COMEDI_DEVCONF_AUX_DATA2_LENGTH];
	BUG_ON(n > 3);
	return (void *)address;
}

int comedi_alloc_board_minor(struct device *hardware_device);
void comedi_free_board_minor(unsigned minor);
int comedi_alloc_subdevice_minor(comedi_device *dev, comedi_subdevice *s);
void comedi_free_subdevice_minor(comedi_subdevice *s);
int comedi_pci_auto_config(struct pci_dev *pcidev, const char *board_name);
void comedi_pci_auto_unconfig(struct pci_dev *pcidev);

#include "comedi_rt.h"

#endif /* _COMEDIDEV_H */

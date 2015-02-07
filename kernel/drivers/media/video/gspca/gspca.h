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

#ifndef GSPCAV2_H
#define GSPCAV2_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
#include <linux/videodev2.h>
#include <media/v4l2-common.h>
#include <linux/mutex.h>

/* compilation option */
#define GSPCA_DEBUG 1

#ifdef GSPCA_DEBUG
/* GSPCA our debug messages */
extern int gspca_debug;
#define PDEBUG(level, fmt, args...) \
	do {\
		if (gspca_debug & (level)) \
			printk(KERN_INFO MODULE_NAME ": " fmt "\n", ## args); \
	} while (0)
#define D_ERR  0x01
#define D_PROBE 0x02
#define D_CONF 0x04
#define D_STREAM 0x08
#define D_FRAM 0x10
#define D_PACK 0x20
#define D_USBI 0x40
#define D_USBO 0x80
#define D_V4L2 0x0100
#else
#define PDEBUG(level, fmt, args...)
#endif
#undef err
#define err(fmt, args...) \
	do {\
		printk(KERN_ERR MODULE_NAME ": " fmt "\n", ## args); \
	} while (0)
#undef info
#define info(fmt, args...) \
	do {\
		printk(KERN_INFO MODULE_NAME ": " fmt "\n", ## args); \
	} while (0)
#undef warn
#define warn(fmt, args...) \
	do {\
		printk(KERN_WARNING MODULE_NAME ": " fmt "\n", ## args); \
	} while (0)

#define GSPCA_MAX_FRAMES 16	/* maximum number of video frame buffers */
/* image transfers */
#define MAX_NURBS 4		/* max number of URBs */
#define ISO_MAX_PKT 32		/* max number of packets in an ISOC transfer */
#define ISO_MAX_SIZE 0x8000	/* max size of one URB buffer (32 Kb) */

/* device information - set at probe time */
struct cam {
	int bulk_size;		/* buffer size when image transfer by bulk */
	const struct v4l2_pix_format *cam_mode;	/* size nmodes */
	char nmodes;
	__u8 bulk_nurbs;	/* number of URBs in bulk mode
				 * - cannot be > MAX_NURBS
				 * - when 0 and bulk_size != 0 means
				 *   1 URB and submit done by subdriver */
	__u8 epaddr;
};

struct gspca_dev;
struct gspca_frame;

/* subdriver operations */
typedef int (*cam_op) (struct gspca_dev *);
typedef void (*cam_v_op) (struct gspca_dev *);
typedef int (*cam_cf_op) (struct gspca_dev *, const struct usb_device_id *);
typedef int (*cam_jpg_op) (struct gspca_dev *,
				struct v4l2_jpegcompression *);
typedef int (*cam_streamparm_op) (struct gspca_dev *,
				  struct v4l2_streamparm *);
typedef int (*cam_qmnu_op) (struct gspca_dev *,
			struct v4l2_querymenu *);
typedef void (*cam_pkt_op) (struct gspca_dev *gspca_dev,
				struct gspca_frame *frame,
				__u8 *data,
				int len);

struct ctrl {
	struct v4l2_queryctrl qctrl;
	int (*set)(struct gspca_dev *, __s32);
	int (*get)(struct gspca_dev *, __s32 *);
};

/* subdriver description */
struct sd_desc {
/* information */
	const char *name;	/* sub-driver name */
/* controls */
	const struct ctrl *ctrls;
	int nctrls;
/* mandatory operations */
	cam_cf_op config;	/* called on probe */
	cam_op init;		/* called on probe and resume */
	cam_op start;		/* called on stream on */
	cam_pkt_op pkt_scan;
/* optional operations */
	cam_v_op stopN;		/* called on stream off - main alt */
	cam_v_op stop0;		/* called on stream off & disconnect - alt 0 */
	cam_v_op dq_callback;	/* called when a frame has been dequeued */
	cam_jpg_op get_jcomp;
	cam_jpg_op set_jcomp;
	cam_qmnu_op querymenu;
	cam_streamparm_op get_streamparm;
	cam_streamparm_op set_streamparm;
};

/* packet types when moving from iso buf to frame buf */
enum gspca_packet_type {
	DISCARD_PACKET,
	FIRST_PACKET,
	INTER_PACKET,
	LAST_PACKET
};

struct gspca_frame {
	__u8 *data;			/* frame buffer */
	__u8 *data_end;			/* end of frame while filling */
	int vma_use_count;
	struct v4l2_buffer v4l2_buf;
};

struct gspca_dev {
	struct video_device vdev;	/* !! must be the first item */
	struct module *module;		/* subdriver handling the device */
	struct usb_device *dev;
	struct file *capt_file;		/* file doing video capture */

	struct cam cam;				/* device information */
	const struct sd_desc *sd_desc;		/* subdriver description */
	unsigned ctrl_dis;		/* disabled controls (bit map) */

#define USB_BUF_SZ 64
	__u8 *usb_buf;				/* buffer for USB exchanges */
	struct urb *urb[MAX_NURBS];

	__u8 *frbuf;				/* buffer for nframes */
	struct gspca_frame frame[GSPCA_MAX_FRAMES];
	__u32 frsz;				/* frame size */
	char nframes;				/* number of frames */
	char fr_i;				/* frame being filled */
	char fr_q;				/* next frame to queue */
	char fr_o;				/* next frame to dequeue */
	signed char fr_queue[GSPCA_MAX_FRAMES];	/* frame queue */
	__u8 last_packet_type;
	__s8 empty_packet;		/* if (-1) don't check empty packets */
	__u8 streaming;

	__u8 curr_mode;			/* current camera mode */
	__u32 pixfmt;			/* current mode parameters */
	__u16 width;
	__u16 height;
	__u32 sequence;			/* frame sequence number */

	wait_queue_head_t wq;		/* wait queue */
	struct mutex usb_lock;		/* usb exchange protection */
	struct mutex read_lock;		/* read protection */
	struct mutex queue_lock;	/* ISOC queue protection */
#ifdef CONFIG_PM
	char frozen;			/* suspend - resume */
#endif
	char users;			/* number of opens */
	char present;			/* device connected */
	char nbufread;			/* number of buffers for read() */
	char nurbs;			/* number of allocated URBs */
	char memory;			/* memory type (V4L2_MEMORY_xxx) */
	__u8 iface;			/* USB interface number */
	__u8 alt;			/* USB alternate setting */
	__u8 nbalt;			/* number of USB alternate settings */
};

int gspca_dev_probe(struct usb_interface *intf,
		const struct usb_device_id *id,
		const struct sd_desc *sd_desc,
		int dev_size,
		struct module *module);
void gspca_disconnect(struct usb_interface *intf);
struct gspca_frame *gspca_frame_add(struct gspca_dev *gspca_dev,
				    enum gspca_packet_type packet_type,
				    struct gspca_frame *frame,
				    const __u8 *data,
				    int len);
struct gspca_frame *gspca_get_i_frame(struct gspca_dev *gspca_dev);
#ifdef CONFIG_PM
int gspca_suspend(struct usb_interface *intf, pm_message_t message);
int gspca_resume(struct usb_interface *intf);
#endif
int gspca_auto_gain_n_exposure(struct gspca_dev *gspca_dev, int avg_lum,
	int desired_avg_lum, int deadzone, int gain_knee, int exposure_knee);
#endif /* GSPCAV2_H */

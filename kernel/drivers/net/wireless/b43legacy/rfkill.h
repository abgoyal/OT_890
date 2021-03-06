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

#ifndef B43legacy_RFKILL_H_
#define B43legacy_RFKILL_H_

struct b43legacy_wldev;

#ifdef CONFIG_B43LEGACY_RFKILL

#include <linux/rfkill.h>
#include <linux/workqueue.h>
#include <linux/input-polldev.h>



struct b43legacy_rfkill {
	/* The RFKILL subsystem data structure */
	struct rfkill *rfkill;
	/* The poll device for the RFKILL input button */
	struct input_polled_dev *poll_dev;
	/* Did initialization succeed? Used for freeing. */
	bool registered;
	/* The unique name of this rfkill switch */
	char name[sizeof("b43legacy-phy4294967295")];
};

/* The init function returns void, because we are not interested
 * in failing the b43 init process when rfkill init failed. */
void b43legacy_rfkill_init(struct b43legacy_wldev *dev);
void b43legacy_rfkill_exit(struct b43legacy_wldev *dev);

char *b43legacy_rfkill_led_name(struct b43legacy_wldev *dev);


#else /* CONFIG_B43LEGACY_RFKILL */
/* No RFKILL support. */

struct b43legacy_rfkill {
	/* empty */
};

static inline void b43legacy_rfkill_alloc(struct b43legacy_wldev *dev)
{
}
static inline void b43legacy_rfkill_free(struct b43legacy_wldev *dev)
{
}
static inline void b43legacy_rfkill_init(struct b43legacy_wldev *dev)
{
}
static inline void b43legacy_rfkill_exit(struct b43legacy_wldev *dev)
{
}
static inline char *b43legacy_rfkill_led_name(struct b43legacy_wldev *dev)
{
	return NULL;
}

#endif /* CONFIG_B43LEGACY_RFKILL */

#endif /* B43legacy_RFKILL_H_ */

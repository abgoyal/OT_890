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

/**
 * @file meslock.c
 *
 * @brief Implements the subdevice lock class.
 * @note Copyright (C) 2006 Meilhaus Electronic GmbH (support@meilhaus.de)
 * @author Guenter Gebhardt
 */

/*
 * Copyright (C) 2006 Meilhaus Electronic GmbH (support@meilhaus.de)
 *
 * This file is free software; you can redistribute it and/or modify
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/spinlock.h>

#include "medefines.h"
#include "meerror.h"

#include "medebug.h"
#include "meslock.h"

int me_slock_enter(struct me_slock *slock, struct file *filep)
{
	PDEBUG_LOCKS("executed.\n");

	spin_lock(&slock->spin_lock);

	if ((slock->filep) != NULL && (slock->filep != filep)) {
		PERROR("Subdevice is locked by another process.\n");
		spin_unlock(&slock->spin_lock);
		return ME_ERRNO_LOCKED;
	}

	slock->count++;

	spin_unlock(&slock->spin_lock);

	return ME_ERRNO_SUCCESS;
}

int me_slock_exit(struct me_slock *slock, struct file *filep)
{
	PDEBUG_LOCKS("executed.\n");

	spin_lock(&slock->spin_lock);
	slock->count--;
	spin_unlock(&slock->spin_lock);

	return ME_ERRNO_SUCCESS;
}

int me_slock_lock(struct me_slock *slock, struct file *filep, int lock)
{
	PDEBUG_LOCKS("executed.\n");

	switch (lock) {

	case ME_LOCK_RELEASE:
		spin_lock(&slock->spin_lock);

		if (slock->filep == filep)
			slock->filep = NULL;

		spin_unlock(&slock->spin_lock);

		break;

	case ME_LOCK_SET:
		spin_lock(&slock->spin_lock);

		if (slock->count) {
			spin_unlock(&slock->spin_lock);
			PERROR("Subdevice is used by another process.\n");
			return ME_ERRNO_USED;
		} else if (slock->filep == NULL)
			slock->filep = filep;
		else if (slock->filep != filep) {
			spin_unlock(&slock->spin_lock);
			PERROR("Subdevice is locked by another process.\n");
			return ME_ERRNO_LOCKED;
		}

		spin_unlock(&slock->spin_lock);

		break;

	case ME_LOCK_CHECK:
		spin_lock(&slock->spin_lock);

		if (slock->count) {
			spin_unlock(&slock->spin_lock);
			return ME_ERRNO_USED;
		} else if ((slock->filep != NULL) && (slock->filep != filep)) {
			spin_unlock(&slock->spin_lock);
			return ME_ERRNO_LOCKED;
		}

		spin_unlock(&slock->spin_lock);

		break;

	default:
		break;
	}

	return ME_ERRNO_SUCCESS;
}

void me_slock_deinit(struct me_slock *slock)
{
	PDEBUG_LOCKS("executed.\n");
}

int me_slock_init(me_slock_t * slock)
{
	PDEBUG_LOCKS("executed.\n");

	slock->filep = NULL;
	slock->count = 0;
	spin_lock_init(&slock->spin_lock);

	return 0;
}

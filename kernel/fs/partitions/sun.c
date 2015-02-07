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
 *  fs/partitions/sun.c
 *
 *  Code extracted from drivers/block/genhd.c
 *
 *  Copyright (C) 1991-1998  Linus Torvalds
 *  Re-organised Feb 1998 Russell King
 */

#include "check.h"
#include "sun.h"

int sun_partition(struct parsed_partitions *state, struct block_device *bdev)
{
	int i;
	__be16 csum;
	int slot = 1;
	__be16 *ush;
	Sector sect;
	struct sun_disklabel {
		unsigned char info[128];   /* Informative text string */
		struct sun_vtoc {
		    __be32 version;     /* Layout version */
		    char   volume[8];   /* Volume name */
		    __be16 nparts;      /* Number of partitions */
		    struct sun_info {           /* Partition hdrs, sec 2 */
			__be16 id;
			__be16 flags;
		    } infos[8];
		    __be16 padding;     /* Alignment padding */
		    __be32 bootinfo[3];  /* Info needed by mboot */
		    __be32 sanity;       /* To verify vtoc sanity */
		    __be32 reserved[10]; /* Free space */
		    __be32 timestamp[8]; /* Partition timestamp */
		} vtoc;
		__be32 write_reinstruct; /* sectors to skip, writes */
		__be32 read_reinstruct;  /* sectors to skip, reads */
		unsigned char spare[148]; /* Padding */
		__be16 rspeed;     /* Disk rotational speed */
		__be16 pcylcount;  /* Physical cylinder count */
		__be16 sparecyl;   /* extra sects per cylinder */
		__be16 obs1;       /* gap1 */
		__be16 obs2;       /* gap2 */
		__be16 ilfact;     /* Interleave factor */
		__be16 ncyl;       /* Data cylinder count */
		__be16 nacyl;      /* Alt. cylinder count */
		__be16 ntrks;      /* Tracks per cylinder */
		__be16 nsect;      /* Sectors per track */
		__be16 obs3;       /* bhead - Label head offset */
		__be16 obs4;       /* ppart - Physical Partition */
		struct sun_partition {
			__be32 start_cylinder;
			__be32 num_sectors;
		} partitions[8];
		__be16 magic;      /* Magic number */
		__be16 csum;       /* Label xor'd checksum */
	} * label;
	struct sun_partition *p;
	unsigned long spc;
	char b[BDEVNAME_SIZE];
	int use_vtoc;
	int nparts;

	label = (struct sun_disklabel *)read_dev_sector(bdev, 0, &sect);
	if (!label)
		return -1;

	p = label->partitions;
	if (be16_to_cpu(label->magic) != SUN_LABEL_MAGIC) {
/*		printk(KERN_INFO "Dev %s Sun disklabel: bad magic %04x\n",
		       bdevname(bdev, b), be16_to_cpu(label->magic)); */
		put_dev_sector(sect);
		return 0;
	}
	/* Look at the checksum */
	ush = ((__be16 *) (label+1)) - 1;
	for (csum = 0; ush >= ((__be16 *) label);)
		csum ^= *ush--;
	if (csum) {
		printk("Dev %s Sun disklabel: Csum bad, label corrupted\n",
		       bdevname(bdev, b));
		put_dev_sector(sect);
		return 0;
	}

	/* Check to see if we can use the VTOC table */
	use_vtoc = ((be32_to_cpu(label->vtoc.sanity) == SUN_VTOC_SANITY) &&
		    (be32_to_cpu(label->vtoc.version) == 1) &&
		    (be16_to_cpu(label->vtoc.nparts) <= 8));

	/* Use 8 partition entries if not specified in validated VTOC */
	nparts = (use_vtoc) ? be16_to_cpu(label->vtoc.nparts) : 8;

	/*
	 * So that old Linux-Sun partitions continue to work,
	 * alow the VTOC to be used under the additional condition ...
	 */
	use_vtoc = use_vtoc || !(label->vtoc.sanity ||
				 label->vtoc.version || label->vtoc.nparts);
	spc = be16_to_cpu(label->ntrks) * be16_to_cpu(label->nsect);
	for (i = 0; i < nparts; i++, p++) {
		unsigned long st_sector;
		unsigned int num_sectors;

		st_sector = be32_to_cpu(p->start_cylinder) * spc;
		num_sectors = be32_to_cpu(p->num_sectors);
		if (num_sectors) {
			put_partition(state, slot, st_sector, num_sectors);
			state->parts[slot].flags = 0;
			if (use_vtoc) {
				if (be16_to_cpu(label->vtoc.infos[i].id) == LINUX_RAID_PARTITION)
					state->parts[slot].flags |= ADDPART_FLAG_RAID;
				else if (be16_to_cpu(label->vtoc.infos[i].id) == SUN_WHOLE_DISK)
					state->parts[slot].flags |= ADDPART_FLAG_WHOLEDISK;
			}
		}
		slot++;
	}
	printk("\n");
	put_dev_sector(sect);
	return 1;
}

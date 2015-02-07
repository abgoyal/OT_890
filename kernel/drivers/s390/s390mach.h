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
 *  drivers/s390/s390mach.h
 *   S/390 data definitions for machine check processing
 *
 *  S390 version
 *    Copyright (C) 2000 IBM Deutschland Entwicklung GmbH, IBM Corporation
 *    Author(s): Ingo Adlung (adlung@de.ibm.com)
 */

#ifndef __s390mach_h
#define __s390mach_h

#include <asm/types.h>

struct mci {
	__u32   sd              :  1; /* 00 system damage */
	__u32   pd              :  1; /* 01 instruction-processing damage */
	__u32   sr              :  1; /* 02 system recovery */
	__u32   to_be_defined_1 :  1; /* 03 */
	__u32   cd              :  1; /* 04 timing-facility damage */
	__u32   ed              :  1; /* 05 external damage */
	__u32   to_be_defined_2 :  1; /* 06 */
	__u32   dg              :  1; /* 07 degradation */
	__u32   w               :  1; /* 08 warning pending */
	__u32   cp              :  1; /* 09 channel-report pending */
	__u32   sp              :  1; /* 10 service-processor damage */
	__u32   ck              :  1; /* 11 channel-subsystem damage */
	__u32   to_be_defined_3 :  2; /* 12-13 */
	__u32   b               :  1; /* 14 backed up */
	__u32   to_be_defined_4 :  1; /* 15 */
	__u32   se              :  1; /* 16 storage error uncorrected */
	__u32   sc              :  1; /* 17 storage error corrected */
	__u32   ke              :  1; /* 18 storage-key error uncorrected */
	__u32   ds              :  1; /* 19 storage degradation */
	__u32   wp              :  1; /* 20 psw mwp validity */
	__u32   ms              :  1; /* 21 psw mask and key validity */
	__u32   pm              :  1; /* 22 psw program mask and cc validity */
	__u32   ia              :  1; /* 23 psw instruction address validity */
	__u32   fa              :  1; /* 24 failing storage address validity */
	__u32   to_be_defined_5 :  1; /* 25 */
	__u32   ec              :  1; /* 26 external damage code validity */
	__u32   fp              :  1; /* 27 floating point register validity */
	__u32   gr              :  1; /* 28 general register validity */
	__u32   cr              :  1; /* 29 control register validity */
	__u32   to_be_defined_6 :  1; /* 30 */
	__u32   st              :  1; /* 31 storage logical validity */
	__u32   ie              :  1; /* 32 indirect storage error */
	__u32   ar              :  1; /* 33 access register validity */
	__u32   da              :  1; /* 34 delayed access exception */
	__u32   to_be_defined_7 :  7; /* 35-41 */
	__u32   pr              :  1; /* 42 tod programmable register validity */
	__u32   fc              :  1; /* 43 fp control register validity */
	__u32   ap              :  1; /* 44 ancillary report */
	__u32   to_be_defined_8 :  1; /* 45 */
	__u32   ct              :  1; /* 46 cpu timer validity */
	__u32   cc              :  1; /* 47 clock comparator validity */
	__u32	to_be_defined_9 : 16; /* 47-63 */
};

/*
 * Channel Report Word
 */
struct crw {
	__u32 res1    :  1;   /* reserved zero */
	__u32 slct    :  1;   /* solicited */
	__u32 oflw    :  1;   /* overflow */
	__u32 chn     :  1;   /* chained */
	__u32 rsc     :  4;   /* reporting source code */
	__u32 anc     :  1;   /* ancillary report */
	__u32 res2    :  1;   /* reserved zero */
	__u32 erc     :  6;   /* error-recovery code */
	__u32 rsid    : 16;   /* reporting-source ID */
} __attribute__ ((packed));

typedef void (*crw_handler_t)(struct crw *, struct crw *, int);

extern int s390_register_crw_handler(int rsc, crw_handler_t handler);
extern void s390_unregister_crw_handler(int rsc);

#define NR_RSCS 16

#define CRW_RSC_MONITOR  0x2  /* monitoring facility */
#define CRW_RSC_SCH      0x3  /* subchannel */
#define CRW_RSC_CPATH    0x4  /* channel path */
#define CRW_RSC_CONFIG   0x9  /* configuration-alert facility */
#define CRW_RSC_CSS      0xB  /* channel subsystem */

#define CRW_ERC_EVENT    0x00 /* event information pending */
#define CRW_ERC_AVAIL    0x01 /* available */
#define CRW_ERC_INIT     0x02 /* initialized */
#define CRW_ERC_TERROR   0x03 /* temporary error */
#define CRW_ERC_IPARM    0x04 /* installed parm initialized */
#define CRW_ERC_TERM     0x05 /* terminal */
#define CRW_ERC_PERRN    0x06 /* perm. error, fac. not init */
#define CRW_ERC_PERRI    0x07 /* perm. error, facility init */
#define CRW_ERC_PMOD     0x08 /* installed parameters modified */

static inline int stcrw(struct crw *pcrw )
{
	int ccode;

	__asm__ __volatile__(
		"stcrw 0(%2)\n\t"
		"ipm %0\n\t"
		"srl %0,28\n\t"
		: "=d" (ccode), "=m" (*pcrw)
		: "a" (pcrw)
		: "cc" );
	return ccode;
}

#define ED_ETR_SYNC	12	/* External damage ETR sync check */
#define ED_ETR_SWITCH	13	/* External damage ETR switch to local */

#define ED_STP_SYNC	7	/* External damage STP sync check */
#define ED_STP_ISLAND	6	/* External damage STP island check */

struct pt_regs;

void s390_handle_mcck(void);
void s390_do_machine_check(struct pt_regs *regs);
#endif /* __s390mach */

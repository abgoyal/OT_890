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
 * ecma_167.h
 *
 * This file is based on ECMA-167 3rd edition (June 1997)
 * http://www.ecma.ch
 *
 * Copyright (c) 2001-2002  Ben Fennema <bfennema@falcon.csc.calpoly.edu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU Public License ("GPL").
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <linux/types.h>

#ifndef _ECMA_167_H
#define _ECMA_167_H 1

/* Character set specification (ECMA 167r3 1/7.2.1) */
typedef struct {
	uint8_t		charSetType;
	uint8_t		charSetInfo[63];
} __attribute__ ((packed)) charspec;

/* Character Set Type (ECMA 167r3 1/7.2.1.1) */
#define CHARSPEC_TYPE_CS0		0x00	/* (1/7.2.2) */
#define CHARSPEC_TYPE_CS1		0x01	/* (1/7.2.3) */
#define CHARSPEC_TYPE_CS2		0x02	/* (1/7.2.4) */
#define CHARSPEC_TYPE_CS3		0x03	/* (1/7.2.5) */
#define CHARSPEC_TYPE_CS4		0x04	/* (1/7.2.6) */
#define CHARSPEC_TYPE_CS5		0x05	/* (1/7.2.7) */
#define CHARSPEC_TYPE_CS6		0x06	/* (1/7.2.8) */
#define CHARSPEC_TYPE_CS7		0x07	/* (1/7.2.9) */
#define CHARSPEC_TYPE_CS8		0x08	/* (1/7.2.10) */

typedef uint8_t		dstring;

/* Timestamp (ECMA 167r3 1/7.3) */
typedef struct {
	__le16		typeAndTimezone;
	__le16		year;
	uint8_t		month;
	uint8_t		day;
	uint8_t		hour;
	uint8_t		minute;
	uint8_t		second;
	uint8_t		centiseconds;
	uint8_t		hundredsOfMicroseconds;
	uint8_t		microseconds;
} __attribute__ ((packed)) timestamp;

/* Type and Time Zone (ECMA 167r3 1/7.3.1) */
#define TIMESTAMP_TYPE_MASK		0xF000
#define TIMESTAMP_TYPE_CUT		0x0000
#define TIMESTAMP_TYPE_LOCAL		0x1000
#define TIMESTAMP_TYPE_AGREEMENT	0x2000
#define TIMESTAMP_TIMEZONE_MASK		0x0FFF

/* Entity identifier (ECMA 167r3 1/7.4) */
typedef struct {
	uint8_t		flags;
	uint8_t		ident[23];
	uint8_t		identSuffix[8];
} __attribute__ ((packed)) regid;

/* Flags (ECMA 167r3 1/7.4.1) */
#define ENTITYID_FLAGS_DIRTY		0x00
#define ENTITYID_FLAGS_PROTECTED	0x01

/* Volume Structure Descriptor (ECMA 167r3 2/9.1) */
#define VSD_STD_ID_LEN			5
struct volStructDesc {
	uint8_t		structType;
	uint8_t		stdIdent[VSD_STD_ID_LEN];
	uint8_t		structVersion;
	uint8_t		structData[2041];
} __attribute__ ((packed));

/* Standard Identifier (EMCA 167r2 2/9.1.2) */
#define VSD_STD_ID_NSR02		"NSR02"	/* (3/9.1) */

/* Standard Identifier (ECMA 167r3 2/9.1.2) */
#define VSD_STD_ID_BEA01		"BEA01"	/* (2/9.2) */
#define VSD_STD_ID_BOOT2		"BOOT2"	/* (2/9.4) */
#define VSD_STD_ID_CD001		"CD001"	/* (ECMA-119) */
#define VSD_STD_ID_CDW02		"CDW02"	/* (ECMA-168) */
#define VSD_STD_ID_NSR03		"NSR03"	/* (3/9.1) */
#define VSD_STD_ID_TEA01		"TEA01"	/* (2/9.3) */

/* Beginning Extended Area Descriptor (ECMA 167r3 2/9.2) */
struct beginningExtendedAreaDesc {
	uint8_t		structType;
	uint8_t		stdIdent[VSD_STD_ID_LEN];
	uint8_t		structVersion;
	uint8_t		structData[2041];
} __attribute__ ((packed));

/* Terminating Extended Area Descriptor (ECMA 167r3 2/9.3) */
struct terminatingExtendedAreaDesc {
	uint8_t		structType;
	uint8_t		stdIdent[VSD_STD_ID_LEN];
	uint8_t		structVersion;
	uint8_t		structData[2041];
} __attribute__ ((packed));

/* Boot Descriptor (ECMA 167r3 2/9.4) */
struct bootDesc {
	uint8_t		structType;
	uint8_t		stdIdent[VSD_STD_ID_LEN];
	uint8_t		structVersion;
	uint8_t		reserved1;
	regid		archType;
	regid		bootIdent;
	__le32		bootExtLocation;
	__le32		bootExtLength;
	__le64		loadAddress;
	__le64		startAddress;
	timestamp	descCreationDateAndTime;
	__le16		flags;
	uint8_t		reserved2[32];
	uint8_t		bootUse[1906];
} __attribute__ ((packed));

/* Flags (ECMA 167r3 2/9.4.12) */
#define BOOT_FLAGS_ERASE		0x01

/* Extent Descriptor (ECMA 167r3 3/7.1) */
typedef struct {
	__le32		extLength;
	__le32		extLocation;
} __attribute__ ((packed)) extent_ad;

typedef struct {
	uint32_t	extLength;
	uint32_t	extLocation;
} kernel_extent_ad;

/* Descriptor Tag (ECMA 167r3 3/7.2) */
typedef struct {
	__le16		tagIdent;
	__le16		descVersion;
	uint8_t		tagChecksum;
	uint8_t		reserved;
	__le16		tagSerialNum;
	__le16		descCRC;
	__le16		descCRCLength;
	__le32		tagLocation;
} __attribute__ ((packed)) tag;

/* Tag Identifier (ECMA 167r3 3/7.2.1) */
#define TAG_IDENT_PVD			0x0001
#define TAG_IDENT_AVDP			0x0002
#define TAG_IDENT_VDP			0x0003
#define TAG_IDENT_IUVD			0x0004
#define TAG_IDENT_PD			0x0005
#define TAG_IDENT_LVD			0x0006
#define TAG_IDENT_USD			0x0007
#define TAG_IDENT_TD			0x0008
#define TAG_IDENT_LVID			0x0009

/* NSR Descriptor (ECMA 167r3 3/9.1) */
struct NSRDesc {
	uint8_t		structType;
	uint8_t		stdIdent[VSD_STD_ID_LEN];
	uint8_t		structVersion;
	uint8_t		reserved;
	uint8_t		structData[2040];
} __attribute__ ((packed));

/* Primary Volume Descriptor (ECMA 167r3 3/10.1) */
struct primaryVolDesc {
	tag		descTag;
	__le32		volDescSeqNum;
	__le32		primaryVolDescNum;
	dstring		volIdent[32];
	__le16		volSeqNum;
	__le16		maxVolSeqNum;
	__le16		interchangeLvl;
	__le16		maxInterchangeLvl;
	__le32		charSetList;
	__le32		maxCharSetList;
	dstring		volSetIdent[128];
	charspec	descCharSet;
	charspec	explanatoryCharSet;
	extent_ad	volAbstract;
	extent_ad	volCopyright;
	regid		appIdent;
	timestamp	recordingDateAndTime;
	regid		impIdent;
	uint8_t		impUse[64];
	__le32		predecessorVolDescSeqLocation;
	__le16		flags;
	uint8_t		reserved[22];
} __attribute__ ((packed));

/* Flags (ECMA 167r3 3/10.1.21) */
#define PVD_FLAGS_VSID_COMMON		0x0001

/* Anchor Volume Descriptor Pointer (ECMA 167r3 3/10.2) */
struct anchorVolDescPtr {
	tag		descTag;
	extent_ad	mainVolDescSeqExt;
	extent_ad	reserveVolDescSeqExt;
	uint8_t	 	reserved[480];
} __attribute__ ((packed));

/* Volume Descriptor Pointer (ECMA 167r3 3/10.3) */
struct volDescPtr {
	tag		descTag;
	__le32		volDescSeqNum;
	extent_ad	nextVolDescSeqExt;
	uint8_t		reserved[484];
} __attribute__ ((packed));

/* Implementation Use Volume Descriptor (ECMA 167r3 3/10.4) */
struct impUseVolDesc {
	tag		descTag;
	__le32		volDescSeqNum;
	regid		impIdent;
	uint8_t		impUse[460];
} __attribute__ ((packed));

/* Partition Descriptor (ECMA 167r3 3/10.5) */
struct partitionDesc {
	tag descTag;
	__le32 volDescSeqNum;
	__le16 partitionFlags;
	__le16 partitionNumber;
	regid partitionContents;
	uint8_t partitionContentsUse[128];
	__le32 accessType;
	__le32 partitionStartingLocation;
	__le32 partitionLength;
	regid impIdent;
	uint8_t impUse[128];
	uint8_t reserved[156];
} __attribute__ ((packed));

/* Partition Flags (ECMA 167r3 3/10.5.3) */
#define PD_PARTITION_FLAGS_ALLOC	0x0001

/* Partition Contents (ECMA 167r2 3/10.5.3) */
#define PD_PARTITION_CONTENTS_NSR02	"+NSR02"

/* Partition Contents (ECMA 167r3 3/10.5.5) */
#define PD_PARTITION_CONTENTS_FDC01	"+FDC01"
#define PD_PARTITION_CONTENTS_CD001	"+CD001"
#define PD_PARTITION_CONTENTS_CDW02	"+CDW02"
#define PD_PARTITION_CONTENTS_NSR03	"+NSR03"

/* Access Type (ECMA 167r3 3/10.5.7) */
#define PD_ACCESS_TYPE_NONE		0x00000000
#define PD_ACCESS_TYPE_READ_ONLY	0x00000001
#define PD_ACCESS_TYPE_WRITE_ONCE	0x00000002
#define PD_ACCESS_TYPE_REWRITABLE	0x00000003
#define PD_ACCESS_TYPE_OVERWRITABLE	0x00000004

/* Logical Volume Descriptor (ECMA 167r3 3/10.6) */
struct logicalVolDesc {
	tag		descTag;
	__le32		volDescSeqNum;
	charspec	descCharSet;
	dstring		logicalVolIdent[128];
	__le32		logicalBlockSize;
	regid		domainIdent;
	uint8_t		logicalVolContentsUse[16];
	__le32		mapTableLength;
	__le32		numPartitionMaps;
	regid		impIdent;
	uint8_t		impUse[128];
	extent_ad	integritySeqExt;
	uint8_t		partitionMaps[0];
} __attribute__ ((packed));

/* Generic Partition Map (ECMA 167r3 3/10.7.1) */
struct genericPartitionMap {
	uint8_t		partitionMapType;
	uint8_t		partitionMapLength;
	uint8_t		partitionMapping[0];
} __attribute__ ((packed));

/* Partition Map Type (ECMA 167r3 3/10.7.1.1) */
#define GP_PARTITION_MAP_TYPE_UNDEF	0x00
#define GP_PARTIITON_MAP_TYPE_1		0x01
#define GP_PARTITION_MAP_TYPE_2		0x02

/* Type 1 Partition Map (ECMA 167r3 3/10.7.2) */
struct genericPartitionMap1 {
	uint8_t		partitionMapType;
	uint8_t		partitionMapLength;
	__le16		volSeqNum;
	__le16		partitionNum;
} __attribute__ ((packed));

/* Type 2 Partition Map (ECMA 167r3 3/10.7.3) */
struct genericPartitionMap2 {
	uint8_t		partitionMapType;
	uint8_t		partitionMapLength;
	uint8_t		partitionIdent[62];
} __attribute__ ((packed));

/* Unallocated Space Descriptor (ECMA 167r3 3/10.8) */
struct unallocSpaceDesc {
	tag		descTag;
	__le32		volDescSeqNum;
	__le32		numAllocDescs;
	extent_ad	allocDescs[0];
} __attribute__ ((packed));

/* Terminating Descriptor (ECMA 167r3 3/10.9) */
struct terminatingDesc {
	tag		descTag;
	uint8_t		reserved[496];
} __attribute__ ((packed));

/* Logical Volume Integrity Descriptor (ECMA 167r3 3/10.10) */
struct logicalVolIntegrityDesc {
	tag		descTag;
	timestamp	recordingDateAndTime;
	__le32		integrityType;
	extent_ad	nextIntegrityExt;
	uint8_t		logicalVolContentsUse[32];
	__le32		numOfPartitions;
	__le32		lengthOfImpUse;
	__le32		freeSpaceTable[0];
	__le32		sizeTable[0];
	uint8_t		impUse[0];
} __attribute__ ((packed));

/* Integrity Type (ECMA 167r3 3/10.10.3) */
#define LVID_INTEGRITY_TYPE_OPEN	0x00000000
#define LVID_INTEGRITY_TYPE_CLOSE	0x00000001

/* Recorded Address (ECMA 167r3 4/7.1) */
typedef struct {
	__le32		logicalBlockNum;
	__le16	 	partitionReferenceNum;
} __attribute__ ((packed)) lb_addr;

/* ... and its in-core analog */
typedef struct {
	uint32_t		logicalBlockNum;
	uint16_t	 	partitionReferenceNum;
} kernel_lb_addr;

/* Short Allocation Descriptor (ECMA 167r3 4/14.14.1) */
typedef struct {
        __le32		extLength;
        __le32		extPosition;
} __attribute__ ((packed)) short_ad;

/* Long Allocation Descriptor (ECMA 167r3 4/14.14.2) */
typedef struct {
	__le32		extLength;
	lb_addr		extLocation;
	uint8_t		impUse[6];
} __attribute__ ((packed)) long_ad;

typedef struct {
	uint32_t	extLength;
	kernel_lb_addr	extLocation;
	uint8_t		impUse[6];
} kernel_long_ad;

/* Extended Allocation Descriptor (ECMA 167r3 4/14.14.3) */
typedef struct {
	__le32		extLength;
	__le32		recordedLength;
	__le32		informationLength;
	lb_addr		extLocation;
} __attribute__ ((packed)) ext_ad;

typedef struct {
	uint32_t	extLength;
	uint32_t	recordedLength;
	uint32_t	informationLength;
	kernel_lb_addr	extLocation;
} kernel_ext_ad;

/* Descriptor Tag (ECMA 167r3 4/7.2 - See 3/7.2) */

/* Tag Identifier (ECMA 167r3 4/7.2.1) */
#define TAG_IDENT_FSD			0x0100
#define TAG_IDENT_FID			0x0101
#define TAG_IDENT_AED			0x0102
#define TAG_IDENT_IE			0x0103
#define TAG_IDENT_TE			0x0104
#define TAG_IDENT_FE			0x0105
#define TAG_IDENT_EAHD			0x0106
#define TAG_IDENT_USE			0x0107
#define TAG_IDENT_SBD			0x0108
#define TAG_IDENT_PIE			0x0109
#define TAG_IDENT_EFE			0x010A

/* File Set Descriptor (ECMA 167r3 4/14.1) */
struct fileSetDesc {
	tag		descTag;
	timestamp	recordingDateAndTime;
	__le16		interchangeLvl;
	__le16		maxInterchangeLvl;
	__le32		charSetList;
	__le32		maxCharSetList;
	__le32		fileSetNum;
	__le32		fileSetDescNum;
	charspec	logicalVolIdentCharSet;
	dstring		logicalVolIdent[128];
	charspec	fileSetCharSet;
	dstring		fileSetIdent[32];
	dstring		copyrightFileIdent[32];
	dstring		abstractFileIdent[32];
	long_ad		rootDirectoryICB;
	regid		domainIdent;
	long_ad		nextExt;
	long_ad		streamDirectoryICB;
	uint8_t		reserved[32];
} __attribute__ ((packed));

/* Partition Header Descriptor (ECMA 167r3 4/14.3) */
struct partitionHeaderDesc {
	short_ad	unallocSpaceTable;
	short_ad	unallocSpaceBitmap;
	short_ad	partitionIntegrityTable;
	short_ad	freedSpaceTable;
	short_ad	freedSpaceBitmap;
	uint8_t		reserved[88];
} __attribute__ ((packed));

/* File Identifier Descriptor (ECMA 167r3 4/14.4) */
struct fileIdentDesc {
	tag		descTag;
	__le16		fileVersionNum;
	uint8_t		fileCharacteristics;
	uint8_t		lengthFileIdent;
	long_ad		icb;
	__le16		lengthOfImpUse;
	uint8_t		impUse[0];
	uint8_t		fileIdent[0];
	uint8_t		padding[0];
} __attribute__ ((packed));

/* File Characteristics (ECMA 167r3 4/14.4.3) */
#define FID_FILE_CHAR_HIDDEN		0x01
#define FID_FILE_CHAR_DIRECTORY		0x02
#define FID_FILE_CHAR_DELETED		0x04
#define FID_FILE_CHAR_PARENT		0x08
#define FID_FILE_CHAR_METADATA		0x10

/* Allocation Ext Descriptor (ECMA 167r3 4/14.5) */
struct allocExtDesc {
	tag		descTag;
	__le32		previousAllocExtLocation;
	__le32		lengthAllocDescs;
} __attribute__ ((packed));

/* ICB Tag (ECMA 167r3 4/14.6) */
typedef struct {
	__le32		priorRecordedNumDirectEntries;
	__le16		strategyType;
	__le16		strategyParameter;
	__le16		numEntries;
	uint8_t		reserved;
	uint8_t		fileType;
	lb_addr		parentICBLocation;
	__le16		flags;
} __attribute__ ((packed)) icbtag;

/* Strategy Type (ECMA 167r3 4/14.6.2) */
#define ICBTAG_STRATEGY_TYPE_UNDEF	0x0000
#define ICBTAG_STRATEGY_TYPE_1		0x0001
#define ICBTAG_STRATEGY_TYPE_2		0x0002
#define ICBTAG_STRATEGY_TYPE_3		0x0003
#define ICBTAG_STRATEGY_TYPE_4		0x0004

/* File Type (ECMA 167r3 4/14.6.6) */
#define ICBTAG_FILE_TYPE_UNDEF		0x00
#define ICBTAG_FILE_TYPE_USE		0x01
#define ICBTAG_FILE_TYPE_PIE		0x02
#define ICBTAG_FILE_TYPE_IE		0x03
#define ICBTAG_FILE_TYPE_DIRECTORY	0x04
#define ICBTAG_FILE_TYPE_REGULAR	0x05
#define ICBTAG_FILE_TYPE_BLOCK		0x06
#define ICBTAG_FILE_TYPE_CHAR		0x07
#define ICBTAG_FILE_TYPE_EA		0x08
#define ICBTAG_FILE_TYPE_FIFO		0x09
#define ICBTAG_FILE_TYPE_SOCKET		0x0A
#define ICBTAG_FILE_TYPE_TE		0x0B
#define ICBTAG_FILE_TYPE_SYMLINK	0x0C
#define ICBTAG_FILE_TYPE_STREAMDIR	0x0D

/* Flags (ECMA 167r3 4/14.6.8) */
#define ICBTAG_FLAG_AD_MASK		0x0007
#define ICBTAG_FLAG_AD_SHORT		0x0000
#define ICBTAG_FLAG_AD_LONG		0x0001
#define ICBTAG_FLAG_AD_EXTENDED		0x0002
#define ICBTAG_FLAG_AD_IN_ICB		0x0003
#define ICBTAG_FLAG_SORTED		0x0008
#define ICBTAG_FLAG_NONRELOCATABLE	0x0010
#define ICBTAG_FLAG_ARCHIVE		0x0020
#define ICBTAG_FLAG_SETUID		0x0040
#define ICBTAG_FLAG_SETGID		0x0080
#define ICBTAG_FLAG_STICKY		0x0100
#define ICBTAG_FLAG_CONTIGUOUS		0x0200
#define ICBTAG_FLAG_SYSTEM		0x0400
#define ICBTAG_FLAG_TRANSFORMED		0x0800
#define ICBTAG_FLAG_MULTIVERSIONS	0x1000
#define ICBTAG_FLAG_STREAM		0x2000

/* Indirect Entry (ECMA 167r3 4/14.7) */
struct indirectEntry {
	tag		descTag;
	icbtag		icbTag;
	long_ad		indirectICB;
} __attribute__ ((packed));

/* Terminal Entry (ECMA 167r3 4/14.8) */
struct terminalEntry {
	tag		descTag;
	icbtag		icbTag;
} __attribute__ ((packed));

/* File Entry (ECMA 167r3 4/14.9) */
struct fileEntry {
	tag		descTag;
	icbtag		icbTag;
	__le32		uid;
	__le32		gid;
	__le32		permissions;
	__le16		fileLinkCount;
	uint8_t		recordFormat;
	uint8_t		recordDisplayAttr;
	__le32		recordLength;
	__le64		informationLength;
	__le64		logicalBlocksRecorded;
	timestamp	accessTime;
	timestamp	modificationTime;
	timestamp	attrTime;
	__le32		checkpoint;
	long_ad		extendedAttrICB;
	regid		impIdent;
	__le64		uniqueID;
	__le32		lengthExtendedAttr;
	__le32		lengthAllocDescs;
	uint8_t		extendedAttr[0];
	uint8_t		allocDescs[0];
} __attribute__ ((packed));

/* Permissions (ECMA 167r3 4/14.9.5) */
#define FE_PERM_O_EXEC			0x00000001U
#define FE_PERM_O_WRITE			0x00000002U
#define FE_PERM_O_READ			0x00000004U
#define FE_PERM_O_CHATTR		0x00000008U
#define FE_PERM_O_DELETE		0x00000010U
#define FE_PERM_G_EXEC			0x00000020U
#define FE_PERM_G_WRITE			0x00000040U
#define FE_PERM_G_READ			0x00000080U
#define FE_PERM_G_CHATTR		0x00000100U
#define FE_PERM_G_DELETE		0x00000200U
#define FE_PERM_U_EXEC			0x00000400U
#define FE_PERM_U_WRITE			0x00000800U
#define FE_PERM_U_READ			0x00001000U
#define FE_PERM_U_CHATTR		0x00002000U
#define FE_PERM_U_DELETE		0x00004000U

/* Record Format (ECMA 167r3 4/14.9.7) */
#define FE_RECORD_FMT_UNDEF		0x00
#define FE_RECORD_FMT_FIXED_PAD		0x01
#define FE_RECORD_FMT_FIXED		0x02
#define FE_RECORD_FMT_VARIABLE8		0x03
#define FE_RECORD_FMT_VARIABLE16	0x04
#define FE_RECORD_FMT_VARIABLE16_MSB	0x05
#define FE_RECORD_FMT_VARIABLE32	0x06
#define FE_RECORD_FMT_PRINT		0x07
#define FE_RECORD_FMT_LF		0x08
#define FE_RECORD_FMT_CR		0x09
#define FE_RECORD_FMT_CRLF		0x0A
#define FE_RECORD_FMT_LFCR		0x0B

/* Record Display Attributes (ECMA 167r3 4/14.9.8) */
#define FE_RECORD_DISPLAY_ATTR_UNDEF	0x00
#define FE_RECORD_DISPLAY_ATTR_1	0x01
#define FE_RECORD_DISPLAY_ATTR_2	0x02
#define FE_RECORD_DISPLAY_ATTR_3	0x03

/* Extended Attribute Header Descriptor (ECMA 167r3 4/14.10.1) */
struct extendedAttrHeaderDesc {
	tag		descTag;
	__le32		impAttrLocation;
	__le32		appAttrLocation;
} __attribute__ ((packed));

/* Generic Format (ECMA 167r3 4/14.10.2) */
struct genericFormat {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	uint8_t		attrData[0];
} __attribute__ ((packed));

/* Character Set Information (ECMA 167r3 4/14.10.3) */
struct charSetInfo {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le32		escapeSeqLength;
	uint8_t		charSetType;
	uint8_t		escapeSeq[0];
} __attribute__ ((packed));

/* Alternate Permissions (ECMA 167r3 4/14.10.4) */
struct altPerms {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le16		ownerIdent;
	__le16		groupIdent;
	__le16		permission;
} __attribute__ ((packed));

/* File Times Extended Attribute (ECMA 167r3 4/14.10.5) */
struct fileTimesExtAttr {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le32		dataLength;
	__le32		fileTimeExistence;
	uint8_t		fileTimes;
} __attribute__ ((packed));

/* FileTimeExistence (ECMA 167r3 4/14.10.5.6) */
#define FTE_CREATION			0x00000001
#define FTE_DELETION			0x00000004
#define FTE_EFFECTIVE			0x00000008
#define FTE_BACKUP			0x00000002

/* Information Times Extended Attribute (ECMA 167r3 4/14.10.6) */
struct infoTimesExtAttr {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le32		dataLength;
	__le32		infoTimeExistence;
	uint8_t		infoTimes[0];
} __attribute__ ((packed));

/* Device Specification (ECMA 167r3 4/14.10.7) */
struct deviceSpec {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le32		impUseLength;
	__le32		majorDeviceIdent;
	__le32		minorDeviceIdent;
	uint8_t		impUse[0];
} __attribute__ ((packed));

/* Implementation Use Extended Attr (ECMA 167r3 4/14.10.8) */
struct impUseExtAttr {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le32		impUseLength;
	regid		impIdent;
	uint8_t		impUse[0];
} __attribute__ ((packed));

/* Application Use Extended Attribute (ECMA 167r3 4/14.10.9) */
struct appUseExtAttr {
	__le32		attrType;
	uint8_t		attrSubtype;
	uint8_t		reserved[3];
	__le32		attrLength;
	__le32		appUseLength;
	regid		appIdent;
	uint8_t		appUse[0];
} __attribute__ ((packed));

#define EXTATTR_CHAR_SET		1
#define EXTATTR_ALT_PERMS		3
#define EXTATTR_FILE_TIMES		5
#define EXTATTR_INFO_TIMES		6
#define EXTATTR_DEV_SPEC		12
#define EXTATTR_IMP_USE			2048
#define EXTATTR_APP_USE			65536

/* Unallocated Space Entry (ECMA 167r3 4/14.11) */
struct unallocSpaceEntry {
	tag		descTag;
	icbtag		icbTag;
	__le32		lengthAllocDescs;
	uint8_t		allocDescs[0];
} __attribute__ ((packed));

/* Space Bitmap Descriptor (ECMA 167r3 4/14.12) */
struct spaceBitmapDesc {
	tag		descTag;
	__le32		numOfBits;
	__le32		numOfBytes;
	uint8_t		bitmap[0];
} __attribute__ ((packed));

/* Partition Integrity Entry (ECMA 167r3 4/14.13) */
struct partitionIntegrityEntry {
	tag		descTag;
	icbtag		icbTag;
	timestamp	recordingDateAndTime;
	uint8_t		integrityType;
	uint8_t		reserved[175];
	regid		impIdent;
	uint8_t		impUse[256];
} __attribute__ ((packed));

/* Short Allocation Descriptor (ECMA 167r3 4/14.14.1) */

/* Extent Length (ECMA 167r3 4/14.14.1.1) */
#define EXT_RECORDED_ALLOCATED		0x00000000
#define EXT_NOT_RECORDED_ALLOCATED	0x40000000
#define EXT_NOT_RECORDED_NOT_ALLOCATED	0x80000000
#define EXT_NEXT_EXTENT_ALLOCDECS	0xC0000000

/* Long Allocation Descriptor (ECMA 167r3 4/14.14.2) */

/* Extended Allocation Descriptor (ECMA 167r3 4/14.14.3) */

/* Logical Volume Header Descriptor (ECMA 167r3 4/14.15) */
struct logicalVolHeaderDesc {
	__le64		uniqueID;
	uint8_t		reserved[24];
} __attribute__ ((packed));

/* Path Component (ECMA 167r3 4/14.16.1) */
struct pathComponent {
	uint8_t		componentType;
	uint8_t		lengthComponentIdent;
	__le16		componentFileVersionNum;
	dstring		componentIdent[0];
} __attribute__ ((packed));

/* File Entry (ECMA 167r3 4/14.17) */
struct extendedFileEntry {
	tag		descTag;
	icbtag		icbTag;
	__le32		uid;
	__le32		gid;
	__le32		permissions;
	__le16		fileLinkCount;
	uint8_t		recordFormat;
	uint8_t		recordDisplayAttr;
	__le32		recordLength;
	__le64		informationLength;
	__le64		objectSize;
	__le64		logicalBlocksRecorded;
	timestamp	accessTime;
	timestamp	modificationTime;
	timestamp	createTime;
	timestamp	attrTime;
	__le32		checkpoint;
	__le32		reserved;
	long_ad		extendedAttrICB;
	long_ad		streamDirectoryICB;
	regid		impIdent;
	__le64		uniqueID;
	__le32		lengthExtendedAttr;
	__le32		lengthAllocDescs;
	uint8_t		extendedAttr[0];
	uint8_t		allocDescs[0];
} __attribute__ ((packed));

#endif /* _ECMA_167_H */

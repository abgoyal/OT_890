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

#ifndef __WINBOND_MDS_H
#define __WINBOND_MDS_H

#include <linux/timer.h>
#include <linux/types.h>
#include <asm/atomic.h>

#include "localpara.h"
#include "mac_structures.h"
#include "scan_s.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_USB_TX_DESCRIPTOR		15		// IS89C35 ability
#define MAX_USB_TX_BUFFER_NUMBER	4		// Virtual pre-buffer number of MAX_USB_TX_BUFFER
#define MAX_USB_TX_BUFFER			4096	// IS89C35 ability 4n alignment is required for hardware

#define AUTH_REQUEST_PAIRWISE_ERROR			0		// _F flag setting
#define AUTH_REQUEST_GROUP_ERROR			1		// _F flag setting

// For variable setting
#define CURRENT_BSS_TYPE				psBSS(psLOCAL->wConnectedSTAindex)->bBssType
#define CURRENT_WEP_MODE				psSME->_dot11PrivacyInvoked
#define CURRENT_BSSID					psBSS(psLOCAL->wConnectedSTAindex)->abBssID
#define CURRENT_DESIRED_WPA_ENABLE		((psSME->bDesiredAuthMode==WPA_AUTH)||(psSME->bDesiredAuthMode==WPAPSK_AUTH))
#ifdef _WPA2_
#define CURRENT_DESIRED_WPA2_ENABLE		((psSME->bDesiredAuthMode==WPA2_AUTH)||(psSME->bDesiredAuthMode==WPA2PSK_AUTH))
#endif //end def _WPA2_
#define CURRENT_PAIRWISE_KEY_OK			psSME->pairwise_key_ok
//[20040712 WS]
#define CURRENT_GROUP_KEY_OK			psSME->group_key_ok
#define CURRENT_PAIRWISE_KEY			psSME->tx_mic_key
#define CURRENT_GROUP_KEY				psSME->group_tx_mic_key
#define CURRENT_ENCRYPT_STATUS			psSME->encrypt_status
#define CURRENT_WEP_ID					adapter->sSmePara._dot11WEPDefaultKeyID
#define CURRENT_CONTROL_PORT_BLOCK		( psSME->wpa_ok!=1 || (adapter->Mds.boCounterMeasureBlock==1 && (CURRENT_ENCRYPT_STATUS==ENCRYPT_TKIP)) )
#define CURRENT_FRAGMENT_THRESHOLD		(adapter->Mds.TxFragmentThreshold & ~0x1)
#define CURRENT_PREAMBLE_MODE			psLOCAL->boShortPreamble?WLAN_PREAMBLE_TYPE_SHORT:WLAN_PREAMBLE_TYPE_LONG
#define CURRENT_TX_RATE					adapter->sLocalPara.CurrentTxRate
#define CURRENT_FALL_BACK_TX_RATE		adapter->sLocalPara.CurrentTxFallbackRate
#define CURRENT_TX_RATE_FOR_MNG			adapter->sLocalPara.CurrentTxRateForMng
#define CURRENT_PROTECT_MECHANISM		psLOCAL->boProtectMechanism
#define CURRENT_RTS_THRESHOLD			adapter->Mds.TxRTSThreshold

#define MIB_GS_XMIT_OK_INC				adapter->sLocalPara.GS_XMIT_OK++
#define MIB_GS_RCV_OK_INC				adapter->sLocalPara.GS_RCV_OK++
#define MIB_GS_XMIT_ERROR_INC			adapter->sLocalPara.GS_XMIT_ERROR

//---------- TX -----------------------------------
#define ETHERNET_TX_DESCRIPTORS         MAX_USB_TX_BUFFER_NUMBER

//---------- RX ------------------------------------
#define ETHERNET_RX_DESCRIPTORS			8	//It's not necessary to allocate more than 2 in sync indicate

//================================================================
// Configration default value
//================================================================
#define DEFAULT_MULTICASTLISTMAX		32 		// standard
#define DEFAULT_TX_BURSTLENGTH			3 		// 32 Longwords
#define DEFAULT_RX_BURSTLENGTH			3 		// 32 Longwords
#define DEFAULT_TX_THRESHOLD			0 		// Full Packet
#define DEFAULT_RX_THRESHOLD			0 		// Full Packet
#define DEFAULT_MAXTXRATE				6 		// 11 Mbps (Long)
#define DEFAULT_CHANNEL					3 		// Chennel 3
#define DEFAULT_RTSThreshold			2347 	// Disable RTS
//#define DEFAULT_PME						1 		// Enable
#define DEFAULT_PME						0 		// Disable
#define DEFAULT_SIFSTIME				10
#define DEFAULT_ACKTIME_1ML             304     // 148+44+112 911220 by LCC
#define DEFAULT_ACKTIME_2ML             248     // 148+44+56 911220 by LCC
#define DEFAULT_FRAGMENT_THRESHOLD      2346 	// No fragment
#define DEFAULT_PREAMBLE_LENGTH			72
#define DEFAULT_PLCPHEADERTIME_LENGTH	24

/*------------------------------------------------------------------------
 0.96 sec since time unit of the R03 for the current, W89C32 is about 60ns
 instead of 960 ns. This shall be fixed in the future W89C32
 -------------------------------------------------------------------------*/
#define DEFAULT_MAX_RECEIVE_TIME        16440000

#define RX_BUF_SIZE						2352        // 600 	// For 301 must be multiple of 8
#define MAX_RX_DESCRIPTORS              18         // Rx Layer 2
#define MAX_BUFFER_QUEUE	8 // The value is always equal 8 due to NDIS_PACKET's MiniportReserved field size


// For brand-new rx system
#define MDS_ID_IGNORE				ETHERNET_RX_DESCRIPTORS

// For Tx Packet status classify
#define PACKET_FREE_TO_USE						0
#define PACKET_COME_FROM_NDIS					0x08
#define PACKET_COME_FROM_MLME					0x80
#define PACKET_SEND_COMPLETE					0xff

typedef struct _MDS
{
	// For Tx usage
	u8	TxOwner[ ((MAX_USB_TX_BUFFER_NUMBER + 3) & ~0x03) ];
	u8	*pTxBuffer;
	u16	TxBufferSize[ ((MAX_USB_TX_BUFFER_NUMBER + 1) & ~0x01) ];
	u8	TxDesFrom[ ((MAX_USB_TX_DESCRIPTOR + 3) & ~0x03) ];//931130.4.u // 1: MLME 2: NDIS control 3: NDIS data
	u8	TxCountInBuffer[ ((MAX_USB_TX_DESCRIPTOR + 3) & ~0x03) ]; // 20060928

	u8	TxFillIndex;//the next index of TxBuffer can be used
	u8	TxDesIndex;//The next index of TxDes can be used
	u8	ScanTxPause;	//data Tx pause because the scanning is progressing, but probe request Tx won't.
	u8	TxPause;//For pause the Mds_Tx modult

	atomic_t	TxThreadCount;//For thread counting 931130.4.v
//950301 delete due to HW
//	atomic_t	TxConcurrentCount;//931130.4.w

	u16	TxResult[ ((MAX_USB_TX_DESCRIPTOR + 1) & ~0x01) ];//Collect the sending result of Mpdu

	u8	MicRedundant[8]; // For tmp use
	u8	*MicWriteAddress[2]; //The start address to fill the Mic, use 2 point due to Mic maybe fragment

	u16	MicWriteSize[2]; //931130.4.x

	u16	MicAdd; // If want to add the Mic, this variable equal to 8
	u16	MicWriteIndex;//The number of MicWriteAddress 931130.4.y

	u8	TxRate[ ((MAX_USB_TX_DESCRIPTOR+1)&~0x01) ][2]; // [0] current tx rate, [1] fall back rate
	u8	TxInfo[ ((MAX_USB_TX_DESCRIPTOR+1)&~0x01) ];	//Store information for callback function

	//WKCHEN added for scanning mechanism
	u8	TxToggle;		//It is TRUE if there are tx activities in some time interval
	u8	Reserved_[3];

	//---------- for Tx Parameter
	u16	TxFragmentThreshold;    	// For frame body only
	u16	TxRTSThreshold;

	u32		MaxReceiveTime;//911220.3 Add

	// depend on OS,
	u32					MulticastListNo;
	u32					PacketFilter; // Setting by NDIS, the current packet filter in use.
	u8					MulticastAddressesArray[DEFAULT_MULTICASTLISTMAX][MAC_ADDR_LENGTH];

	//COUNTERMEASURE
	u8		bMICfailCount;
	u8		boCounterMeasureBlock;
	u8		reserved_4[2];

	u32	TxTsc; // 20060214
	u32	TxTsc_2; // 20060214

} MDS, *PMDS;


typedef struct _RxBuffer
{
    u8 * pBufferAddress;     // Pointer the received data buffer.
	u16	BufferSize;
	u8	RESERVED;
	u8	BufferIndex;// Only 1 byte
} RXBUFFER, *PRXBUFFER;

//
// Reveive Layer 1 Format.
//----------------------------
typedef struct _RXLAYER1
{
    u16  SequenceNumber;     // The sequence number of the last received packet.
	u16	BufferTotalSize;

	u32	InUsed;
    u32   DecryptionMethod;   // The desired defragment number of the next incoming packet.

	u8	DeFragmentNumber;
	u8	FrameType;
    u8	TypeEncapsulated;
	u8	BufferNumber;

	u32	FirstFrameArrivedTime;

	RXBUFFER	BufferQueue[ MAX_BUFFER_QUEUE ];

	u8		LastFrameType; // 20061004 for fix intel 3945 's bug
	u8		RESERVED[3];  //@@ anson

	/////////////////////////////////////////////////////////////////////////////////////////////
	// For brand-new Rx system
	u8	ReservedBuffer[ 2400 ];//If Buffer ID is reserved one, it must copy the data into this area
	u8	*ReservedBufferPoint;// Point to the next availabe address of reserved buffer

}RXLAYER1, * PRXLAYER1;

#endif

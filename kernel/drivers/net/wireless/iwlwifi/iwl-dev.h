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

/******************************************************************************
 *
 * Copyright(c) 2003 - 2008 Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 *  Intel Linux Wireless <ilw@linux.intel.com>
 * Intel Corporation, 5200 N.E. Elam Young Parkway, Hillsboro, OR 97124-6497
 *
 *****************************************************************************/
/*
 * Please use this file (iwl-dev.h) for driver implementation definitions.
 * Please use iwl-commands.h for uCode API definitions.
 * Please use iwl-4965-hw.h for hardware-related definitions.
 */

#ifndef __iwl_dev_h__
#define __iwl_dev_h__

#include <linux/pci.h> /* for struct pci_device_id */
#include <linux/kernel.h>
#include <net/ieee80211_radiotap.h>

#define DRV_NAME        "iwlagn"
#include "iwl-rfkill.h"
#include "iwl-eeprom.h"
#include "iwl-4965-hw.h"
#include "iwl-csr.h"
#include "iwl-prph.h"
#include "iwl-debug.h"
#include "iwl-led.h"
#include "iwl-power.h"
#include "iwl-agn-rs.h"

/* configuration for the iwl4965 */
extern struct iwl_cfg iwl4965_agn_cfg;
extern struct iwl_cfg iwl5300_agn_cfg;
extern struct iwl_cfg iwl5100_agn_cfg;
extern struct iwl_cfg iwl5350_agn_cfg;
extern struct iwl_cfg iwl5100_bg_cfg;
extern struct iwl_cfg iwl5100_abg_cfg;
extern struct iwl_cfg iwl5150_agn_cfg;

/* CT-KILL constants */
#define CT_KILL_THRESHOLD	110 /* in Celsius */

/* Default noise level to report when noise measurement is not available.
 *   This may be because we're:
 *   1)  Not associated (4965, no beacon statistics being sent to driver)
 *   2)  Scanning (noise measurement does not apply to associated channel)
 *   3)  Receiving CCK (3945 delivers noise info only for OFDM frames)
 * Use default noise value of -127 ... this is below the range of measurable
 *   Rx dBm for either 3945 or 4965, so it can indicate "unmeasurable" to user.
 *   Also, -127 works better than 0 when averaging frames with/without
 *   noise info (e.g. averaging might be done in app); measured dBm values are
 *   always negative ... using a negative value as the default keeps all
 *   averages within an s8's (used in some apps) range of negative values. */
#define IWL_NOISE_MEAS_NOT_AVAILABLE (-127)

/*
 * RTS threshold here is total size [2347] minus 4 FCS bytes
 * Per spec:
 *   a value of 0 means RTS on all data/management packets
 *   a value > max MSDU size means no RTS
 * else RTS for data/management frames where MPDU is larger
 *   than RTS value.
 */
#define DEFAULT_RTS_THRESHOLD     2347U
#define MIN_RTS_THRESHOLD         0U
#define MAX_RTS_THRESHOLD         2347U
#define MAX_MSDU_SIZE		  2304U
#define MAX_MPDU_SIZE		  2346U
#define DEFAULT_BEACON_INTERVAL   100U
#define	DEFAULT_SHORT_RETRY_LIMIT 7U
#define	DEFAULT_LONG_RETRY_LIMIT  4U

struct iwl_rx_mem_buffer {
	dma_addr_t real_dma_addr;
	dma_addr_t aligned_dma_addr;
	struct sk_buff *skb;
	struct list_head list;
};

/*
 * Generic queue structure
 *
 * Contains common data for Rx and Tx queues
 */
struct iwl_queue {
	int n_bd;              /* number of BDs in this queue */
	int write_ptr;       /* 1-st empty entry (index) host_w*/
	int read_ptr;         /* last used entry (index) host_r*/
	dma_addr_t dma_addr;   /* physical addr for BD's */
	int n_window;	       /* safe queue window */
	u32 id;
	int low_mark;	       /* low watermark, resume queue if free
				* space more than this */
	int high_mark;         /* high watermark, stop queue if free
				* space less than this */
} __attribute__ ((packed));

/* One for each TFD */
struct iwl_tx_info {
	struct sk_buff *skb[IWL_NUM_OF_TBS - 1];
};

/**
 * struct iwl_tx_queue - Tx Queue for DMA
 * @q: generic Rx/Tx queue descriptor
 * @bd: base of circular buffer of TFDs
 * @cmd: array of command/Tx buffers
 * @dma_addr_cmd: physical address of cmd/tx buffer array
 * @txb: array of per-TFD driver data
 * @need_update: indicates need to update read/write index
 * @sched_retry: indicates queue is high-throughput aggregation (HT AGG) enabled
 *
 * A Tx queue consists of circular buffer of BDs (a.k.a. TFDs, transmit frame
 * descriptors) and required locking structures.
 */
struct iwl_tx_queue {
	struct iwl_queue q;
	struct iwl_tfd *tfds;
	struct iwl_cmd *cmd[TFD_TX_CMD_SLOTS];
	struct iwl_tx_info *txb;
	u8 need_update;
	u8 sched_retry;
	u8 active;
	u8 swq_id;
};

#define IWL_NUM_SCAN_RATES         (2)

struct iwl4965_channel_tgd_info {
	u8 type;
	s8 max_power;
};

struct iwl4965_channel_tgh_info {
	s64 last_radar_time;
};

/*
 * One for each channel, holds all channel setup data
 * Some of the fields (e.g. eeprom and flags/max_power_avg) are redundant
 *     with one another!
 */
struct iwl_channel_info {
	struct iwl4965_channel_tgd_info tgd;
	struct iwl4965_channel_tgh_info tgh;
	struct iwl_eeprom_channel eeprom;	/* EEPROM regulatory limit */
	struct iwl_eeprom_channel fat_eeprom;	/* EEPROM regulatory limit for
						 * FAT channel */

	u8 channel;	  /* channel number */
	u8 flags;	  /* flags copied from EEPROM */
	s8 max_power_avg; /* (dBm) regul. eeprom, normal Tx, any rate */
	s8 curr_txpow;	  /* (dBm) regulatory/spectrum/user (not h/w) limit */
	s8 min_power;	  /* always 0 */
	s8 scan_power;	  /* (dBm) regul. eeprom, direct scans, any rate */

	u8 group_index;	  /* 0-4, maps channel to group1/2/3/4/5 */
	u8 band_index;	  /* 0-4, maps channel to band1/2/3/4/5 */
	enum ieee80211_band band;

	/* FAT channel info */
	s8 fat_max_power_avg;	/* (dBm) regul. eeprom, normal Tx, any rate */
	s8 fat_curr_txpow;	/* (dBm) regulatory/spectrum/user (not h/w) */
	s8 fat_min_power;	/* always 0 */
	s8 fat_scan_power;	/* (dBm) eeprom, direct scans, any rate */
	u8 fat_flags;		/* flags copied from EEPROM */
	u8 fat_extension_channel; /* HT_IE_EXT_CHANNEL_* */
};


#define IWL_TX_FIFO_AC0	0
#define IWL_TX_FIFO_AC1	1
#define IWL_TX_FIFO_AC2	2
#define IWL_TX_FIFO_AC3	3
#define IWL_TX_FIFO_HCCA_1	5
#define IWL_TX_FIFO_HCCA_2	6
#define IWL_TX_FIFO_NONE	7

/* Minimum number of queues. MAX_NUM is defined in hw specific files */
#define IWL_MIN_NUM_QUEUES	4

/* Power management (not Tx power) structures */

enum iwl_pwr_src {
	IWL_PWR_SRC_VMAIN,
	IWL_PWR_SRC_VAUX,
};

#define IEEE80211_DATA_LEN              2304
#define IEEE80211_4ADDR_LEN             30
#define IEEE80211_HLEN                  (IEEE80211_4ADDR_LEN)
#define IEEE80211_FRAME_LEN             (IEEE80211_DATA_LEN + IEEE80211_HLEN)

struct iwl_frame {
	union {
		struct ieee80211_hdr frame;
		struct iwl_tx_beacon_cmd beacon;
		u8 raw[IEEE80211_FRAME_LEN];
		u8 cmd[360];
	} u;
	struct list_head list;
};

#define SEQ_TO_SN(seq) (((seq) & IEEE80211_SCTL_SEQ) >> 4)
#define SN_TO_SEQ(ssn) (((ssn) << 4) & IEEE80211_SCTL_SEQ)
#define MAX_SN ((IEEE80211_SCTL_SEQ) >> 4)

enum {
	/* CMD_SIZE_NORMAL = 0, */
	CMD_SIZE_HUGE = (1 << 0),
	/* CMD_SYNC = 0, */
	CMD_ASYNC = (1 << 1),
	/* CMD_NO_SKB = 0, */
	CMD_WANT_SKB = (1 << 2),
};

struct iwl_cmd;
struct iwl_priv;

struct iwl_cmd_meta {
	struct iwl_cmd_meta *source;
	union {
		struct sk_buff *skb;
		int (*callback)(struct iwl_priv *priv,
				struct iwl_cmd *cmd, struct sk_buff *skb);
	} __attribute__ ((packed)) u;

	/* The CMD_SIZE_HUGE flag bit indicates that the command
	 * structure is stored at the end of the shared queue memory. */
	u32 flags;
	DECLARE_PCI_UNMAP_ADDR(mapping)
	DECLARE_PCI_UNMAP_LEN(len)
} __attribute__ ((packed));

#define IWL_CMD_MAX_PAYLOAD 320

/**
 * struct iwl_cmd
 *
 * For allocation of the command and tx queues, this establishes the overall
 * size of the largest command we send to uCode, except for a scan command
 * (which is relatively huge; space is allocated separately).
 */
struct iwl_cmd {
	struct iwl_cmd_meta meta;	/* driver data */
	struct iwl_cmd_header hdr;	/* uCode API */
	union {
		u32 flags;
		u8 val8;
		u16 val16;
		u32 val32;
		struct iwl_tx_cmd tx;
		u8 payload[IWL_CMD_MAX_PAYLOAD];
	} __attribute__ ((packed)) cmd;
} __attribute__ ((packed));


struct iwl_host_cmd {
	u8 id;
	u16 len;
	struct iwl_cmd_meta meta;
	const void *data;
};

#define TFD_MAX_PAYLOAD_SIZE (sizeof(struct iwl_cmd) - \
			      sizeof(struct iwl_cmd_meta))

/*
 * RX related structures and functions
 */
#define RX_FREE_BUFFERS 64
#define RX_LOW_WATERMARK 8

#define SUP_RATE_11A_MAX_NUM_CHANNELS  8
#define SUP_RATE_11B_MAX_NUM_CHANNELS  4
#define SUP_RATE_11G_MAX_NUM_CHANNELS  12

/**
 * struct iwl_rx_queue - Rx queue
 * @read: Shared index to newest available Rx buffer
 * @write: Shared index to oldest written Rx packet
 * @free_count: Number of pre-allocated buffers in rx_free
 * @rx_free: list of free SKBs for use
 * @rx_used: List of Rx buffers with no SKB
 * @need_update: flag to indicate we need to update read/write index
 *
 * NOTE:  rx_free and rx_used are used as a FIFO for iwl_rx_mem_buffers
 */
struct iwl_rx_queue {
	__le32 *bd;
	dma_addr_t dma_addr;
	struct iwl_rx_mem_buffer pool[RX_QUEUE_SIZE + RX_FREE_BUFFERS];
	struct iwl_rx_mem_buffer *queue[RX_QUEUE_SIZE];
	u32 read;
	u32 write;
	u32 free_count;
	struct list_head rx_free;
	struct list_head rx_used;
	int need_update;
	struct iwl_rb_status *rb_stts;
	dma_addr_t rb_stts_dma;
	spinlock_t lock;
};

#define IWL_SUPPORTED_RATES_IE_LEN         8

#define MAX_TID_COUNT        9

#define IWL_INVALID_RATE     0xFF
#define IWL_INVALID_VALUE    -1

/**
 * struct iwl_ht_agg -- aggregation status while waiting for block-ack
 * @txq_id: Tx queue used for Tx attempt
 * @frame_count: # frames attempted by Tx command
 * @wait_for_ba: Expect block-ack before next Tx reply
 * @start_idx: Index of 1st Transmit Frame Descriptor (TFD) in Tx window
 * @bitmap0: Low order bitmap, one bit for each frame pending ACK in Tx window
 * @bitmap1: High order, one bit for each frame pending ACK in Tx window
 * @rate_n_flags: Rate at which Tx was attempted
 *
 * If REPLY_TX indicates that aggregation was attempted, driver must wait
 * for block ack (REPLY_COMPRESSED_BA).  This struct stores tx reply info
 * until block ack arrives.
 */
struct iwl_ht_agg {
	u16 txq_id;
	u16 frame_count;
	u16 wait_for_ba;
	u16 start_idx;
	u64 bitmap;
	u32 rate_n_flags;
#define IWL_AGG_OFF 0
#define IWL_AGG_ON 1
#define IWL_EMPTYING_HW_QUEUE_ADDBA 2
#define IWL_EMPTYING_HW_QUEUE_DELBA 3
	u8 state;
};


struct iwl_tid_data {
	u16 seq_number;
	u16 tfds_in_queue;
	struct iwl_ht_agg agg;
};

struct iwl_hw_key {
	enum ieee80211_key_alg alg;
	int keylen;
	u8 keyidx;
	u8 key[32];
};

union iwl4965_ht_rate_supp {
	u16 rates;
	struct {
		u8 siso_rate;
		u8 mimo_rate;
	};
};

#define CFG_HT_RX_AMPDU_FACTOR_DEF  (0x3)
#define CFG_HT_MPDU_DENSITY_2USEC   (0x5)
#define CFG_HT_MPDU_DENSITY_DEF CFG_HT_MPDU_DENSITY_2USEC

struct iwl_ht_info {
	/* self configuration data */
	u8 is_ht;
	u8 supported_chan_width;
	u8 sm_ps;
	u8 is_green_field;
	u8 sgf;			/* HT_SHORT_GI_* short guard interval */
	u8 max_amsdu_size;
	u8 ampdu_factor;
	u8 mpdu_density;
	struct ieee80211_mcs_info mcs;
	/* BSS related data */
	u8 extension_chan_offset;
	u8 tx_chan_width;
	u8 ht_protection;
	u8 non_GF_STA_present;
};

union iwl_qos_capabity {
	struct {
		u8 edca_count:4;	/* bit 0-3 */
		u8 q_ack:1;		/* bit 4 */
		u8 queue_request:1;	/* bit 5 */
		u8 txop_request:1;	/* bit 6 */
		u8 reserved:1;		/* bit 7 */
	} q_AP;
	struct {
		u8 acvo_APSD:1;		/* bit 0 */
		u8 acvi_APSD:1;		/* bit 1 */
		u8 ac_bk_APSD:1;	/* bit 2 */
		u8 ac_be_APSD:1;	/* bit 3 */
		u8 q_ack:1;		/* bit 4 */
		u8 max_len:2;		/* bit 5-6 */
		u8 more_data_ack:1;	/* bit 7 */
	} q_STA;
	u8 val;
};

/* QoS structures */
struct iwl_qos_info {
	int qos_active;
	union iwl_qos_capabity qos_cap;
	struct iwl_qosparam_cmd def_qos_parm;
};

#define STA_PS_STATUS_WAKE             0
#define STA_PS_STATUS_SLEEP            1

struct iwl_station_entry {
	struct iwl_addsta_cmd sta;
	struct iwl_tid_data tid[MAX_TID_COUNT];
	u8 used;
	u8 ps_status;
	struct iwl_hw_key keyinfo;
};

/* one for each uCode image (inst/data, boot/init/runtime) */
struct fw_desc {
	void *v_addr;		/* access by driver */
	dma_addr_t p_addr;	/* access by card's busmaster DMA */
	u32 len;		/* bytes */
};

/* uCode file layout */
struct iwl_ucode {
	__le32 ver;		/* major/minor/API/serial */
	__le32 inst_size;	/* bytes of runtime instructions */
	__le32 data_size;	/* bytes of runtime data */
	__le32 init_size;	/* bytes of initialization instructions */
	__le32 init_data_size;	/* bytes of initialization data */
	__le32 boot_size;	/* bytes of bootstrap instructions */
	u8 data[0];		/* data in same order as "size" elements */
};

struct iwl4965_ibss_seq {
	u8 mac[ETH_ALEN];
	u16 seq_num;
	u16 frag_num;
	unsigned long packet_time;
	struct list_head list;
};

struct iwl_sensitivity_ranges {
	u16 min_nrg_cck;
	u16 max_nrg_cck;

	u16 nrg_th_cck;
	u16 nrg_th_ofdm;

	u16 auto_corr_min_ofdm;
	u16 auto_corr_min_ofdm_mrc;
	u16 auto_corr_min_ofdm_x1;
	u16 auto_corr_min_ofdm_mrc_x1;

	u16 auto_corr_max_ofdm;
	u16 auto_corr_max_ofdm_mrc;
	u16 auto_corr_max_ofdm_x1;
	u16 auto_corr_max_ofdm_mrc_x1;

	u16 auto_corr_max_cck;
	u16 auto_corr_max_cck_mrc;
	u16 auto_corr_min_cck;
	u16 auto_corr_min_cck_mrc;
};


#define KELVIN_TO_CELSIUS(x) ((x)-273)
#define CELSIUS_TO_KELVIN(x) ((x)+273)


/**
 * struct iwl_hw_params
 * @max_txq_num: Max # Tx queues supported
 * @dma_chnl_num: Number of Tx DMA/FIFO channels
 * @scd_bc_tbls_size: size of scheduler byte count tables
 * @tx/rx_chains_num: Number of TX/RX chains
 * @valid_tx/rx_ant: usable antennas
 * @max_rxq_size: Max # Rx frames in Rx queue (must be power-of-2)
 * @max_rxq_log: Log-base-2 of max_rxq_size
 * @rx_buf_size: Rx buffer size
 * @max_stations:
 * @bcast_sta_id:
 * @fat_channel: is 40MHz width possible in band 2.4
 * BIT(IEEE80211_BAND_5GHZ) BIT(IEEE80211_BAND_5GHZ)
 * @sw_crypto: 0 for hw, 1 for sw
 * @max_xxx_size: for ucode uses
 * @ct_kill_threshold: temperature threshold
 * @calib_init_cfg: setup initial calibrations for the hw
 * @struct iwl_sensitivity_ranges: range of sensitivity values
 */
struct iwl_hw_params {
	u8 max_txq_num;
	u8 dma_chnl_num;
	u16 scd_bc_tbls_size;
	u8  tx_chains_num;
	u8  rx_chains_num;
	u8  valid_tx_ant;
	u8  valid_rx_ant;
	u16 max_rxq_size;
	u16 max_rxq_log;
	u32 rx_buf_size;
	u32 max_pkt_size;
	u8  max_stations;
	u8  bcast_sta_id;
	u8 fat_channel;
	u8 sw_crypto;
	u32 max_inst_size;
	u32 max_data_size;
	u32 max_bsm_size;
	u32 ct_kill_threshold; /* value in hw-dependent units */
	u32 calib_init_cfg;
	const struct iwl_sensitivity_ranges *sens;
};


/******************************************************************************
 *
 * Functions implemented in core module which are forward declared here
 * for use by iwl-[4-5].c
 *
 * NOTE:  The implementation of these functions are not hardware specific
 * which is why they are in the core module files.
 *
 * Naming convention --
 * iwl_         <-- Is part of iwlwifi
 * iwlXXXX_     <-- Hardware specific (implemented in iwl-XXXX.c for XXXX)
 * iwl4965_bg_      <-- Called from work queue context
 * iwl4965_mac_     <-- mac80211 callback
 *
 ****************************************************************************/
extern void iwl_update_chain_flags(struct iwl_priv *priv);
extern int iwl_set_pwr_src(struct iwl_priv *priv, enum iwl_pwr_src src);
extern const u8 iwl_bcast_addr[ETH_ALEN];
extern int iwl_rxq_stop(struct iwl_priv *priv);
extern void iwl_txq_ctx_stop(struct iwl_priv *priv);
extern int iwl_queue_space(const struct iwl_queue *q);
static inline int iwl_queue_used(const struct iwl_queue *q, int i)
{
	return q->write_ptr > q->read_ptr ?
		(i >= q->read_ptr && i < q->write_ptr) :
		!(i < q->read_ptr && i >= q->write_ptr);
}


static inline u8 get_cmd_index(struct iwl_queue *q, u32 index, int is_huge)
{
	/* This is for scan command, the big buffer at end of command array */
	if (is_huge)
		return q->n_window;	/* must be power of 2 */

	/* Otherwise, use normal size buffers */
	return index & (q->n_window - 1);
}


struct iwl_dma_ptr {
	dma_addr_t dma;
	void *addr;
	size_t size;
};

#define HT_SHORT_GI_20MHZ	(1 << 0)
#define HT_SHORT_GI_40MHZ	(1 << 1)

#define IWL_CHANNEL_WIDTH_20MHZ   0
#define IWL_CHANNEL_WIDTH_40MHZ   1

#define IWL_OPERATION_MODE_AUTO     0
#define IWL_OPERATION_MODE_HT_ONLY  1
#define IWL_OPERATION_MODE_MIXED    2
#define IWL_OPERATION_MODE_20MHZ    3

#define IWL_TX_CRC_SIZE 4
#define IWL_TX_DELIMITER_SIZE 4

#define TX_POWER_IWL_ILLEGAL_VOLTAGE -10000

/* Sensitivity and chain noise calibration */
#define INITIALIZATION_VALUE		0xFFFF
#define CAL_NUM_OF_BEACONS		20
#define MAXIMUM_ALLOWED_PATHLOSS	15

#define CHAIN_NOISE_MAX_DELTA_GAIN_CODE 3

#define MAX_FA_OFDM  50
#define MIN_FA_OFDM  5
#define MAX_FA_CCK   50
#define MIN_FA_CCK   5

#define AUTO_CORR_STEP_OFDM       1

#define AUTO_CORR_STEP_CCK     3
#define AUTO_CORR_MAX_TH_CCK   160

#define NRG_DIFF               2
#define NRG_STEP_CCK           2
#define NRG_MARGIN             8
#define MAX_NUMBER_CCK_NO_FA 100

#define AUTO_CORR_CCK_MIN_VAL_DEF    (125)

#define CHAIN_A             0
#define CHAIN_B             1
#define CHAIN_C             2
#define CHAIN_NOISE_DELTA_GAIN_INIT_VAL 4
#define ALL_BAND_FILTER			0xFF00
#define IN_BAND_FILTER			0xFF
#define MIN_AVERAGE_NOISE_MAX_VALUE	0xFFFFFFFF

#define NRG_NUM_PREV_STAT_L     20
#define NUM_RX_CHAINS           3

enum iwl4965_false_alarm_state {
	IWL_FA_TOO_MANY = 0,
	IWL_FA_TOO_FEW = 1,
	IWL_FA_GOOD_RANGE = 2,
};

enum iwl4965_chain_noise_state {
	IWL_CHAIN_NOISE_ALIVE = 0,  /* must be 0 */
	IWL_CHAIN_NOISE_ACCUMULATE,
	IWL_CHAIN_NOISE_CALIBRATED,
	IWL_CHAIN_NOISE_DONE,
};

enum iwl4965_calib_enabled_state {
	IWL_CALIB_DISABLED = 0,  /* must be 0 */
	IWL_CALIB_ENABLED = 1,
};


/*
 * enum iwl_calib
 * defines the order in which results of initial calibrations
 * should be sent to the runtime uCode
 */
enum iwl_calib {
	IWL_CALIB_XTAL,
	IWL_CALIB_DC,
	IWL_CALIB_LO,
	IWL_CALIB_TX_IQ,
	IWL_CALIB_TX_IQ_PERD,
	IWL_CALIB_BASE_BAND,
	IWL_CALIB_MAX
};

/* Opaque calibration results */
struct iwl_calib_result {
	void *buf;
	size_t buf_len;
};

enum ucode_type {
	UCODE_NONE = 0,
	UCODE_INIT,
	UCODE_RT
};

/* Sensitivity calib data */
struct iwl_sensitivity_data {
	u32 auto_corr_ofdm;
	u32 auto_corr_ofdm_mrc;
	u32 auto_corr_ofdm_x1;
	u32 auto_corr_ofdm_mrc_x1;
	u32 auto_corr_cck;
	u32 auto_corr_cck_mrc;

	u32 last_bad_plcp_cnt_ofdm;
	u32 last_fa_cnt_ofdm;
	u32 last_bad_plcp_cnt_cck;
	u32 last_fa_cnt_cck;

	u32 nrg_curr_state;
	u32 nrg_prev_state;
	u32 nrg_value[10];
	u8  nrg_silence_rssi[NRG_NUM_PREV_STAT_L];
	u32 nrg_silence_ref;
	u32 nrg_energy_idx;
	u32 nrg_silence_idx;
	u32 nrg_th_cck;
	s32 nrg_auto_corr_silence_diff;
	u32 num_in_cck_no_fa;
	u32 nrg_th_ofdm;
};

/* Chain noise (differential Rx gain) calib data */
struct iwl_chain_noise_data {
	u32 active_chains;
	u32 chain_noise_a;
	u32 chain_noise_b;
	u32 chain_noise_c;
	u32 chain_signal_a;
	u32 chain_signal_b;
	u32 chain_signal_c;
	u16 beacon_count;
	u8 disconn_array[NUM_RX_CHAINS];
	u8 delta_gain_code[NUM_RX_CHAINS];
	u8 radio_write;
	u8 state;
};

#define	EEPROM_SEM_TIMEOUT 10		/* milliseconds */
#define EEPROM_SEM_RETRY_LIMIT 1000	/* number of attempts (not time) */


enum {
	MEASUREMENT_READY = (1 << 0),
	MEASUREMENT_ACTIVE = (1 << 1),
};


#define IWL_MAX_NUM_QUEUES	20 /* FIXME: do dynamic allocation */

struct iwl_priv {

	/* ieee device used by generic ieee processing code */
	struct ieee80211_hw *hw;
	struct ieee80211_channel *ieee_channels;
	struct ieee80211_rate *ieee_rates;
	struct iwl_cfg *cfg;

	/* temporary frame storage list */
	struct list_head free_frames;
	int frames_count;

	enum ieee80211_band band;
	int alloc_rxb_skb;

	void (*rx_handlers[REPLY_MAX])(struct iwl_priv *priv,
				       struct iwl_rx_mem_buffer *rxb);

	struct ieee80211_supported_band bands[IEEE80211_NUM_BANDS];

#ifdef CONFIG_IWLAGN_SPECTRUM_MEASUREMENT
	/* spectrum measurement report caching */
	struct iwl_spectrum_notification measure_report;
	u8 measurement_status;
#endif
	/* ucode beacon time */
	u32 ucode_beacon_time;

	/* we allocate array of iwl4965_channel_info for NIC's valid channels.
	 *    Access via channel # using indirect index array */
	struct iwl_channel_info *channel_info;	/* channel info array */
	u8 channel_count;	/* # of channels */

	/* thermal calibration */
	s32 temperature;	/* degrees Kelvin */
	s32 last_temperature;

	/* init calibration results */
	struct iwl_calib_result calib_results[IWL_CALIB_MAX];

	/* Scan related variables */
	unsigned long last_scan_jiffies;
	unsigned long next_scan_jiffies;
	unsigned long scan_start;
	unsigned long scan_pass_start;
	unsigned long scan_start_tsf;
	struct iwl_scan_cmd *scan;
	int scan_bands;
	int one_direct_scan;
	u8 direct_ssid_len;
	u8 direct_ssid[IW_ESSID_MAX_SIZE];
	u8 scan_tx_ant[IEEE80211_NUM_BANDS];
	u8 mgmt_tx_ant;

	/* spinlock */
	spinlock_t lock;	/* protect general shared data */
	spinlock_t hcmd_lock;	/* protect hcmd */
	struct mutex mutex;

	/* basic pci-network driver stuff */
	struct pci_dev *pci_dev;

	/* pci hardware address support */
	void __iomem *hw_base;
	u32  hw_rev;
	u32  hw_wa_rev;
	u8   rev_id;

	/* uCode images, save to reload in case of failure */
	u32 ucode_ver;			/* version of ucode, copy of
					   iwl_ucode.ver */
	struct fw_desc ucode_code;	/* runtime inst */
	struct fw_desc ucode_data;	/* runtime data original */
	struct fw_desc ucode_data_backup;	/* runtime data save/restore */
	struct fw_desc ucode_init;	/* initialization inst */
	struct fw_desc ucode_init_data;	/* initialization data */
	struct fw_desc ucode_boot;	/* bootstrap inst */
	enum ucode_type ucode_type;
	u8 ucode_write_complete;	/* the image write is complete */


	struct iwl_rxon_time_cmd rxon_timing;

	/* We declare this const so it can only be
	 * changed via explicit cast within the
	 * routines that actually update the physical
	 * hardware */
	const struct iwl_rxon_cmd active_rxon;
	struct iwl_rxon_cmd staging_rxon;

	int error_recovering;
	struct iwl_rxon_cmd recovery_rxon;

	/* 1st responses from initialize and runtime uCode images.
	 * 4965's initialize alive response contains some calibration data. */
	struct iwl_init_alive_resp card_alive_init;
	struct iwl_alive_resp card_alive;
#ifdef CONFIG_IWLWIFI_RFKILL
	struct rfkill *rfkill;
#endif

#ifdef CONFIG_IWLWIFI_LEDS
	struct iwl_led led[IWL_LED_TRG_MAX];
	unsigned long last_blink_time;
	u8 last_blink_rate;
	u8 allow_blinking;
	u64 led_tpt;
#endif

	u16 active_rate;
	u16 active_rate_basic;

	u8 assoc_station_added;
	u8 start_calib;
	struct iwl_sensitivity_data sensitivity_data;
	struct iwl_chain_noise_data chain_noise_data;
	__le16 sensitivity_tbl[HD_TABLE_SIZE];

	struct iwl_ht_info current_ht_config;
	u8 last_phy_res[100];

	/* Rate scaling data */
	s8 data_retry_limit;
	u8 retry_rate;

	wait_queue_head_t wait_command_queue;

	int activity_timer_active;

	/* Rx and Tx DMA processing queues */
	struct iwl_rx_queue rxq;
	struct iwl_tx_queue txq[IWL_MAX_NUM_QUEUES];
	unsigned long txq_ctx_active_msk;
	struct iwl_dma_ptr  kw;	/* keep warm address */
	struct iwl_dma_ptr  scd_bc_tbls;

	u32 scd_base_addr;	/* scheduler sram base address */

	unsigned long status;

	int last_rx_rssi;	/* From Rx packet statistics */
	int last_rx_noise;	/* From beacon statistics */

	/* counts mgmt, ctl, and data packets */
	struct traffic_stats {
		u32 cnt;
		u64 bytes;
	} tx_stats[3], rx_stats[3];

	struct iwl_power_mgr power_data;

	struct iwl_notif_statistics statistics;
	unsigned long last_statistics_time;

	/* context information */
	u16 rates_mask;

	u32 power_mode;
	u32 antenna;
	u8 bssid[ETH_ALEN];
	u16 rts_threshold;
	u8 mac_addr[ETH_ALEN];

	/*station table variables */
	spinlock_t sta_lock;
	int num_stations;
	struct iwl_station_entry stations[IWL_STATION_COUNT];
	struct iwl_wep_key wep_keys[WEP_KEYS_MAX];
	u8 default_wep_key;
	u8 key_mapping_key;
	unsigned long ucode_key_table;

	/* Indication if ieee80211_ops->open has been called */
	u8 is_open;

	u8 mac80211_registered;

	/* Rx'd packet timing information */
	u32 last_beacon_time;
	u64 last_tsf;

	/* eeprom */
	u8 *eeprom;
	struct iwl_eeprom_calib_info *calib_info;

	enum nl80211_iftype iw_mode;

	struct sk_buff *ibss_beacon;

	/* Last Rx'd beacon timestamp */
	u64 timestamp;
	u16 beacon_int;
	struct ieee80211_vif *vif;

	struct iwl_hw_params hw_params;


	/* Current association information needed to configure the
	 * hardware */
	u16 assoc_id;
	u16 assoc_capability;

	struct iwl_qos_info qos_data;

	struct workqueue_struct *workqueue;

	struct work_struct up;
	struct work_struct restart;
	struct work_struct calibrated_work;
	struct work_struct scan_completed;
	struct work_struct rx_replenish;
	struct work_struct rf_kill;
	struct work_struct abort_scan;
	struct work_struct update_link_led;
	struct work_struct auth_work;
	struct work_struct report_work;
	struct work_struct request_scan;
	struct work_struct beacon_update;

	struct tasklet_struct irq_tasklet;

	struct delayed_work set_power_save;
	struct delayed_work init_alive_start;
	struct delayed_work alive_start;
	struct delayed_work scan_check;
	/* TX Power */
	s8 tx_power_user_lmt;
	s8 tx_power_channel_lmt;


#ifdef CONFIG_IWLWIFI_DEBUG
	/* debugging info */
	u32 debug_level;
	u32 framecnt_to_us;
	atomic_t restrict_refcnt;
#ifdef CONFIG_IWLWIFI_DEBUGFS
	/* debugfs */
	struct iwl_debugfs *dbgfs;
#endif /* CONFIG_IWLWIFI_DEBUGFS */
#endif /* CONFIG_IWLWIFI_DEBUG */

	struct work_struct txpower_work;
	u32 disable_sens_cal;
	u32 disable_chain_noise_cal;
	u32 disable_tx_power_cal;
	struct work_struct run_time_calib_work;
	struct timer_list statistics_periodic;
}; /*iwl_priv */

static inline void iwl_txq_ctx_activate(struct iwl_priv *priv, int txq_id)
{
	set_bit(txq_id, &priv->txq_ctx_active_msk);
}

static inline void iwl_txq_ctx_deactivate(struct iwl_priv *priv, int txq_id)
{
	clear_bit(txq_id, &priv->txq_ctx_active_msk);
}

#ifdef CONFIG_IWLWIFI_DEBUG
const char *iwl_get_tx_fail_reason(u32 status);
#else
static inline const char *iwl_get_tx_fail_reason(u32 status) { return ""; }
#endif


static inline struct ieee80211_hdr *iwl_tx_queue_get_hdr(struct iwl_priv *priv,
							 int txq_id, int idx)
{
	if (priv->txq[txq_id].txb[idx].skb[0])
		return (struct ieee80211_hdr *)priv->txq[txq_id].
				txb[idx].skb[0]->data;
	return NULL;
}


static inline int iwl_is_associated(struct iwl_priv *priv)
{
	return (priv->active_rxon.filter_flags & RXON_FILTER_ASSOC_MSK) ? 1 : 0;
}

static inline int is_channel_valid(const struct iwl_channel_info *ch_info)
{
	if (ch_info == NULL)
		return 0;
	return (ch_info->flags & EEPROM_CHANNEL_VALID) ? 1 : 0;
}

static inline int is_channel_radar(const struct iwl_channel_info *ch_info)
{
	return (ch_info->flags & EEPROM_CHANNEL_RADAR) ? 1 : 0;
}

static inline u8 is_channel_a_band(const struct iwl_channel_info *ch_info)
{
	return ch_info->band == IEEE80211_BAND_5GHZ;
}

static inline u8 is_channel_bg_band(const struct iwl_channel_info *ch_info)
{
	return ch_info->band == IEEE80211_BAND_2GHZ;
}

static inline int is_channel_passive(const struct iwl_channel_info *ch)
{
	return (!(ch->flags & EEPROM_CHANNEL_ACTIVE)) ? 1 : 0;
}

static inline int is_channel_ibss(const struct iwl_channel_info *ch)
{
	return ((ch->flags & EEPROM_CHANNEL_IBSS)) ? 1 : 0;
}

#endif				/* __iwl_dev_h__ */

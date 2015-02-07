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
 * rtc class driver for the Maxim MAX6900 chip
 *
 * Author: Dale Farnsworth <dale@farnsworth.org>
 *
 * based on previously existing rtc class drivers
 *
 * 2007 (c) MontaVista, Software, Inc.  This file is licensed under
 * the terms of the GNU General Public License version 2.  This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/bcd.h>
#include <linux/rtc.h>
#include <linux/delay.h>

#define DRV_VERSION "0.2"

/*
 * register indices
 */
#define MAX6900_REG_SC			0	/* seconds      00-59 */
#define MAX6900_REG_MN			1	/* minutes      00-59 */
#define MAX6900_REG_HR			2	/* hours        00-23 */
#define MAX6900_REG_DT			3	/* day of month 00-31 */
#define MAX6900_REG_MO			4	/* month        01-12 */
#define MAX6900_REG_DW			5	/* day of week   1-7  */
#define MAX6900_REG_YR			6	/* year         00-99 */
#define MAX6900_REG_CT			7	/* control */
						/* register 8 is undocumented */
#define MAX6900_REG_CENTURY		9	/* century */
#define MAX6900_REG_LEN			10

#define MAX6900_BURST_LEN		8	/* can burst r/w first 8 regs */

#define MAX6900_REG_CT_WP		(1 << 7)	/* Write Protect */

/*
 * register read/write commands
 */
#define MAX6900_REG_CONTROL_WRITE	0x8e
#define MAX6900_REG_CENTURY_WRITE	0x92
#define MAX6900_REG_CENTURY_READ	0x93
#define MAX6900_REG_RESERVED_READ	0x96
#define MAX6900_REG_BURST_WRITE		0xbe
#define MAX6900_REG_BURST_READ		0xbf

#define MAX6900_IDLE_TIME_AFTER_WRITE	3	/* specification says 2.5 mS */

static struct i2c_driver max6900_driver;

static int max6900_i2c_read_regs(struct i2c_client *client, u8 *buf)
{
	u8 reg_burst_read[1] = { MAX6900_REG_BURST_READ };
	u8 reg_century_read[1] = { MAX6900_REG_CENTURY_READ };
	struct i2c_msg msgs[4] = {
		{
		 .addr = client->addr,
		 .flags = 0,	/* write */
		 .len = sizeof(reg_burst_read),
		 .buf = reg_burst_read}
		,
		{
		 .addr = client->addr,
		 .flags = I2C_M_RD,
		 .len = MAX6900_BURST_LEN,
		 .buf = buf}
		,
		{
		 .addr = client->addr,
		 .flags = 0,	/* write */
		 .len = sizeof(reg_century_read),
		 .buf = reg_century_read}
		,
		{
		 .addr = client->addr,
		 .flags = I2C_M_RD,
		 .len = sizeof(buf[MAX6900_REG_CENTURY]),
		 .buf = &buf[MAX6900_REG_CENTURY]
		 }
	};
	int rc;

	rc = i2c_transfer(client->adapter, msgs, ARRAY_SIZE(msgs));
	if (rc != ARRAY_SIZE(msgs)) {
		dev_err(&client->dev, "%s: register read failed\n", __func__);
		return -EIO;
	}
	return 0;
}

static int max6900_i2c_write_regs(struct i2c_client *client, u8 const *buf)
{
	u8 i2c_century_buf[1 + 1] = { MAX6900_REG_CENTURY_WRITE };
	struct i2c_msg century_msgs[1] = {
		{
		 .addr = client->addr,
		 .flags = 0,	/* write */
		 .len = sizeof(i2c_century_buf),
		 .buf = i2c_century_buf}
	};
	u8 i2c_burst_buf[MAX6900_BURST_LEN + 1] = { MAX6900_REG_BURST_WRITE };
	struct i2c_msg burst_msgs[1] = {
		{
		 .addr = client->addr,
		 .flags = 0,	/* write */
		 .len = sizeof(i2c_burst_buf),
		 .buf = i2c_burst_buf}
	};
	int rc;

	/*
	 * We have to make separate calls to i2c_transfer because of
	 * the need to delay after each write to the chip.  Also,
	 * we write the century byte first, since we set the write-protect
	 * bit as part of the burst write.
	 */
	i2c_century_buf[1] = buf[MAX6900_REG_CENTURY];

	rc = i2c_transfer(client->adapter, century_msgs,
			  ARRAY_SIZE(century_msgs));
	if (rc != ARRAY_SIZE(century_msgs))
		goto write_failed;

	msleep(MAX6900_IDLE_TIME_AFTER_WRITE);

	memcpy(&i2c_burst_buf[1], buf, MAX6900_BURST_LEN);

	rc = i2c_transfer(client->adapter, burst_msgs, ARRAY_SIZE(burst_msgs));
	if (rc != ARRAY_SIZE(burst_msgs))
		goto write_failed;
	msleep(MAX6900_IDLE_TIME_AFTER_WRITE);

	return 0;

 write_failed:
	dev_err(&client->dev, "%s: register write failed\n", __func__);
	return -EIO;
}

static int max6900_i2c_read_time(struct i2c_client *client, struct rtc_time *tm)
{
	int rc;
	u8 regs[MAX6900_REG_LEN];

	rc = max6900_i2c_read_regs(client, regs);
	if (rc < 0)
		return rc;

	tm->tm_sec = bcd2bin(regs[MAX6900_REG_SC]);
	tm->tm_min = bcd2bin(regs[MAX6900_REG_MN]);
	tm->tm_hour = bcd2bin(regs[MAX6900_REG_HR] & 0x3f);
	tm->tm_mday = bcd2bin(regs[MAX6900_REG_DT]);
	tm->tm_mon = bcd2bin(regs[MAX6900_REG_MO]) - 1;
	tm->tm_year = bcd2bin(regs[MAX6900_REG_YR]) +
		      bcd2bin(regs[MAX6900_REG_CENTURY]) * 100 - 1900;
	tm->tm_wday = bcd2bin(regs[MAX6900_REG_DW]);

	return 0;
}

static int max6900_i2c_clear_write_protect(struct i2c_client *client)
{
	int rc;
	rc = i2c_smbus_write_byte_data(client, MAX6900_REG_CONTROL_WRITE, 0);
	if (rc < 0) {
		dev_err(&client->dev, "%s: control register write failed\n",
			__func__);
		return -EIO;
	}
	return 0;
}

static int
max6900_i2c_set_time(struct i2c_client *client, struct rtc_time const *tm)
{
	u8 regs[MAX6900_REG_LEN];
	int rc;

	rc = max6900_i2c_clear_write_protect(client);
	if (rc < 0)
		return rc;

	regs[MAX6900_REG_SC] = bin2bcd(tm->tm_sec);
	regs[MAX6900_REG_MN] = bin2bcd(tm->tm_min);
	regs[MAX6900_REG_HR] = bin2bcd(tm->tm_hour);
	regs[MAX6900_REG_DT] = bin2bcd(tm->tm_mday);
	regs[MAX6900_REG_MO] = bin2bcd(tm->tm_mon + 1);
	regs[MAX6900_REG_DW] = bin2bcd(tm->tm_wday);
	regs[MAX6900_REG_YR] = bin2bcd(tm->tm_year % 100);
	regs[MAX6900_REG_CENTURY] = bin2bcd((tm->tm_year + 1900) / 100);
	/* set write protect */
	regs[MAX6900_REG_CT] = MAX6900_REG_CT_WP;

	rc = max6900_i2c_write_regs(client, regs);
	if (rc < 0)
		return rc;

	return 0;
}

static int max6900_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	return max6900_i2c_read_time(to_i2c_client(dev), tm);
}

static int max6900_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	return max6900_i2c_set_time(to_i2c_client(dev), tm);
}

static int max6900_remove(struct i2c_client *client)
{
	struct rtc_device *rtc = i2c_get_clientdata(client);

	if (rtc)
		rtc_device_unregister(rtc);

	return 0;
}

static const struct rtc_class_ops max6900_rtc_ops = {
	.read_time = max6900_rtc_read_time,
	.set_time = max6900_rtc_set_time,
};

static int
max6900_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct rtc_device *rtc;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENODEV;

	dev_info(&client->dev, "chip found, driver version " DRV_VERSION "\n");

	rtc = rtc_device_register(max6900_driver.driver.name,
				  &client->dev, &max6900_rtc_ops, THIS_MODULE);
	if (IS_ERR(rtc))
		return PTR_ERR(rtc);

	i2c_set_clientdata(client, rtc);

	return 0;
}

static struct i2c_device_id max6900_id[] = {
	{ "max6900", 0 },
	{ }
};

static struct i2c_driver max6900_driver = {
	.driver = {
		   .name = "rtc-max6900",
		   },
	.probe = max6900_probe,
	.remove = max6900_remove,
	.id_table = max6900_id,
};

static int __init max6900_init(void)
{
	return i2c_add_driver(&max6900_driver);
}

static void __exit max6900_exit(void)
{
	i2c_del_driver(&max6900_driver);
}

MODULE_DESCRIPTION("Maxim MAX6900 RTC driver");
MODULE_AUTHOR("Dale Farnsworth <dale@farnsworth.org>");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);

module_init(max6900_init);
module_exit(max6900_exit);
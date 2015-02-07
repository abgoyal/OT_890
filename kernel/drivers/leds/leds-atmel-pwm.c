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

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/io.h>
#include <linux/atmel_pwm.h>


struct pwmled {
	struct led_classdev	cdev;
	struct pwm_channel	pwmc;
	struct gpio_led		*desc;
	u32			mult;
	u8			active_low;
};


/*
 * For simplicity, we use "brightness" as if it were a linear function
 * of PWM duty cycle.  However, a logarithmic function of duty cycle is
 * probably a better match for perceived brightness: two is half as bright
 * as four, four is half as bright as eight, etc
 */
static void pwmled_brightness(struct led_classdev *cdev, enum led_brightness b)
{
	struct pwmled		 *led;

	/* update the duty cycle for the *next* period */
	led = container_of(cdev, struct pwmled, cdev);
	pwm_channel_writel(&led->pwmc, PWM_CUPD, led->mult * (unsigned) b);
}

/*
 * NOTE:  we reuse the platform_data structure of GPIO leds,
 * but repurpose its "gpio" number as a PWM channel number.
 */
static int __init pwmled_probe(struct platform_device *pdev)
{
	const struct gpio_led_platform_data	*pdata;
	struct pwmled				*leds;
	int					i;
	int					status;

	pdata = pdev->dev.platform_data;
	if (!pdata || pdata->num_leds < 1)
		return -ENODEV;

	leds = kcalloc(pdata->num_leds, sizeof(*leds), GFP_KERNEL);
	if (!leds)
		return -ENOMEM;

	for (i = 0; i < pdata->num_leds; i++) {
		struct pwmled		*led = leds + i;
		const struct gpio_led	*dat = pdata->leds + i;
		u32			tmp;

		led->cdev.name = dat->name;
		led->cdev.brightness = LED_OFF;
		led->cdev.brightness_set = pwmled_brightness;
		led->cdev.default_trigger = dat->default_trigger;

		led->active_low = dat->active_low;

		status = pwm_channel_alloc(dat->gpio, &led->pwmc);
		if (status < 0)
			goto err;

		/*
		 * Prescale clock by 2^x, so PWM counts in low MHz.
		 * Start each cycle with the LED active, so increasing
		 * the duty cycle gives us more time on (== brighter).
		 */
		tmp = 5;
		if (!led->active_low)
			tmp |= PWM_CPR_CPOL;
		pwm_channel_writel(&led->pwmc, PWM_CMR, tmp);

		/*
		 * Pick a period so PWM cycles at 100+ Hz; and a multiplier
		 * for scaling duty cycle:  brightness * mult.
		 */
		tmp = (led->pwmc.mck / (1 << 5)) / 100;
		tmp /= 255;
		led->mult = tmp;
		pwm_channel_writel(&led->pwmc, PWM_CDTY,
				led->cdev.brightness * 255);
		pwm_channel_writel(&led->pwmc, PWM_CPRD,
				LED_FULL * tmp);

		pwm_channel_enable(&led->pwmc);

		/* Hand it over to the LED framework */
		status = led_classdev_register(&pdev->dev, &led->cdev);
		if (status < 0) {
			pwm_channel_free(&led->pwmc);
			goto err;
		}
	}

	platform_set_drvdata(pdev, leds);
	return 0;

err:
	if (i > 0) {
		for (i = i - 1; i >= 0; i--) {
			led_classdev_unregister(&leds[i].cdev);
			pwm_channel_free(&leds[i].pwmc);
		}
	}
	kfree(leds);

	return status;
}

static int __exit pwmled_remove(struct platform_device *pdev)
{
	const struct gpio_led_platform_data	*pdata;
	struct pwmled				*leds;
	unsigned				i;

	pdata = pdev->dev.platform_data;
	leds = platform_get_drvdata(pdev);

	for (i = 0; i < pdata->num_leds; i++) {
		struct pwmled		*led = leds + i;

		led_classdev_unregister(&led->cdev);
		pwm_channel_free(&led->pwmc);
	}

	kfree(leds);
	platform_set_drvdata(pdev, NULL);
	return 0;
}

/* work with hotplug and coldplug */
MODULE_ALIAS("platform:leds-atmel-pwm");

static struct platform_driver pwmled_driver = {
	.driver = {
		.name =		"leds-atmel-pwm",
		.owner =	THIS_MODULE,
	},
	/* REVISIT add suspend() and resume() methods */
	.remove =	__exit_p(pwmled_remove),
};

static int __init modinit(void)
{
	return platform_driver_probe(&pwmled_driver, pwmled_probe);
}
module_init(modinit);

static void __exit modexit(void)
{
	platform_driver_unregister(&pwmled_driver);
}
module_exit(modexit);

MODULE_DESCRIPTION("Driver for LEDs with PWM-controlled brightness");
MODULE_LICENSE("GPL");

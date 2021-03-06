/* linux/arch/arm/mach-exynos/board-universal_5420-led.c
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/
#include <linux/init.h>
#include <linux/export.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <plat/gpio-cfg.h>
#include "board-universal5420.h"

#ifdef CONFIG_LEDS_AN30259A
#include <linux/leds-an30259a.h>
#endif

#ifdef CONFIG_LEDS_LP5562
#include <linux/leds-lp55xx.h>
#endif

#define GPIO_S_LED_I2C_SCL	EXYNOS5420_GPG0(4)
#define GPIO_S_LED_I2C_SDA	EXYNOS5420_GPG0(5)

/* H/W revision to support Revision S */
static char force_an30259a = true;

extern unsigned int system_rev;

/* I2C21 */
#if defined(CONFIG_LEDS_AN30259A) || defined(CONFIG_LEDS_LP5562)
static struct i2c_gpio_platform_data gpio_i2c_data21 = {
	.scl_pin = GPIO_S_LED_I2C_SCL,
	.sda_pin = GPIO_S_LED_I2C_SDA,
};

struct platform_device s3c_device_i2c21 = {
	.name = "i2c-gpio",
	.id = 21,
	.dev.platform_data = &gpio_i2c_data21,
};
#endif

/* I2C21 */
#if defined(CONFIG_LEDS_AN30259A)
static struct i2c_board_info i2c_an30259a_emul[] __initdata = {
	{
		I2C_BOARD_INFO("an30259a", 0x30),
	},
};
#endif


#ifdef CONFIG_LEDS_LP5562

/* mode 1 (charging) : RGB(255,0,0) always on */
static u8 mode_charging[] = { 0x40, 0xFF, };

/* mode 2 (charging error) : RGB(255,0,0) 500ms off, 500ms on */
static u8 mode_charging_err[] = {
		0x40, 0x00, 0x60, 0x00, 0x40, 0xFF,
		0x60, 0x00,
		};

/* mode 3 (missed noti) : RGB(0,0,255) 5000ms off, 500ms on */
static u8 mode_missed_noti[] = {
		0x40, 0x00, 0x60, 0x00, 0xA4, 0xA1,
		0x40, 0xFF, 0x60, 0x00,
		};

/* mode 4 (low batt) : RGB(255,0,0) 5000ms off, 500ms on */
static u8 mode_low_batt[] = {
		0x40, 0x00, 0x60, 0x00, 0xA4, 0xA1,
		0x40, 0xFF, 0x60, 0x00,
		};

/* mode 5 (full charged) : RGB(0,255,0) always on */
static u8 mode_full_chg[] = { 0x40, 0xFF, };

/* mode 6 (power on) : RGB(0,17,204) -> RGB(0,140,255)
		ramp up 672ms, wait 328ms, ramp down 672ms, wait 328ms */
static u8 mode_powering_green[] = {
		0x40, 0x11, 0x0B, 0x7A, 0x40, 0x8C,
		0x55, 0x00, 0xE0, 0x08, 0x0B, 0xFA,
		0x40, 0x11, 0x55, 0x00, 0xE0, 0x08,
		};

static u8 mode_powering_blue[] = {
		0x40, 0xCC, 0x1B, 0x32, 0x40, 0xFF,
		0x55, 0x00, 0xE1, 0x00, 0x1B, 0xB2,
		0x40, 0xCC, 0x55, 0x00, 0xE1, 0x00,
		};

struct lp55xx_predef_pattern board_led_patterns[] = {
	{
		.r = mode_charging,
		.size_r = ARRAY_SIZE(mode_charging),
	},
	{
		.r = mode_charging_err,
		.size_r = ARRAY_SIZE(mode_charging_err),
	},
	{
		.b = mode_missed_noti,
		.size_b = ARRAY_SIZE(mode_missed_noti),
	},
	{
		.r = mode_low_batt,
		.size_r = ARRAY_SIZE(mode_low_batt),
	},
	{
		.g = mode_full_chg,
		.size_g = ARRAY_SIZE(mode_full_chg),
	},
	{
		.g = mode_powering_green,
		.b = mode_powering_blue,
		.size_g = ARRAY_SIZE(mode_powering_green),
		.size_b = ARRAY_SIZE(mode_powering_blue),
	},
};

static struct lp55xx_led_config lp5562_led_config[] = {
	{
		.name           = "led_r",
		.chan_nr        = 0,
		.led_current    = 40,
		.max_current    = 40,
	},
	{
		.name           = "led_g",
		.chan_nr        = 1,
		.led_current    = 40,
		.max_current    = 40,
	},
	{
		.name           = "led_b",
		.chan_nr        = 2,
		.led_current    = 40,
		.max_current    = 40,
	},
#if defined _CONFIG_W_CH
	{
		.name           = "white",
		.chan_nr        = 3,
		.led_current    = 20,
		.max_current    = 40,
	},
#endif
};

#define LP5562_CONFIGS	(LP5562_PWM_HF | LP5562_PWRSAVE_EN | \
			 LP5562_CLK_SRC_EXT)

struct lp55xx_platform_data lp5562_pdata = {
	.led_config    = lp5562_led_config,
	.num_channels  = ARRAY_SIZE(lp5562_led_config),
	.update_config = LP5562_CONFIGS,
	.patterns      = board_led_patterns,
	.num_patterns  = ARRAY_SIZE(board_led_patterns),
};

static struct i2c_board_info i2c_lp5562_emul[] __initdata = {
	{
		I2C_BOARD_INFO("lp5562", 0x30),
		.platform_data = &lp5562_pdata,
	},
};
#endif

static struct platform_device *universal5420_led_devices[] __initdata = {
#if defined(CONFIG_LEDS_AN30259A) || defined(CONFIG_LEDS_LP5562)
	&s3c_device_i2c21,
#endif
};

void __init exynos5_universal5420_led_init(void)
{
	printk("%s start\n", __func__);
	s3c_gpio_cfgpin(gpio_i2c_data21.scl_pin, S3C_GPIO_INPUT);
	s3c_gpio_setpull(gpio_i2c_data21.scl_pin, S3C_GPIO_PULL_NONE);
	s5p_gpio_set_drvstr(gpio_i2c_data21.scl_pin, S5P_GPIO_DRVSTR_LV1);

	s3c_gpio_cfgpin(gpio_i2c_data21.sda_pin, S3C_GPIO_INPUT);
	s3c_gpio_setpull(gpio_i2c_data21.sda_pin, S3C_GPIO_PULL_NONE);
	s5p_gpio_set_drvstr(gpio_i2c_data21.sda_pin, S5P_GPIO_DRVSTR_LV1);

#if defined(CONFIG_LEDS_AN30259A)
	if (force_an30259a)
		i2c_register_board_info(21, i2c_an30259a_emul,
					ARRAY_SIZE(i2c_an30259a_emul));
#endif

#if defined(CONFIG_LEDS_LP5562)
	if (!force_an30259a)
		i2c_register_board_info(21, i2c_lp5562_emul,
					ARRAY_SIZE(i2c_lp5562_emul));
#endif

	platform_add_devices(universal5420_led_devices,
			ARRAY_SIZE(universal5420_led_devices));

}

void exynos5_led_run_pattern(int mode)
{
#if defined(CONFIG_LEDS_AN30259A)
	if (force_an30259a)
		an30259a_start_led_pattern(mode);
#endif
#if defined(CONFIG_LEDS_LP5562)
	if (!force_an30259a)
		lp5562_run_pattern(mode);
#endif
}
EXPORT_SYMBOL(exynos5_led_run_pattern);

void exynos5_led_blink(int rgb, int on, int off)
{
#if defined(CONFIG_LEDS_AN30259A)
	if (force_an30259a)
		an30259a_led_blink(rgb, on, off);
#endif
#if defined(CONFIG_LEDS_LP5562)
	if (!force_an30259a)
		lp5562_blink(rgb, on, off);
#endif
}
EXPORT_SYMBOL(exynos5_led_blink);

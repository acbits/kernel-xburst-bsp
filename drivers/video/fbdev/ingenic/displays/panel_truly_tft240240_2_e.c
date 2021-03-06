/*
 * LCD panel support for ingenicfb
 *
 * Copyright (C) 2016 Ingenic Semiconductor Inc.
 *
 * Author:clwang<chunlei.wang@ingenic.com>
 * 	  qipengzhen<aric.pzqi@ingenic.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/fb.h>
#include <linux/lcd.h>
#include <linux/backlight.h>
#include <linux/gpio.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/of_gpio.h>
#include "../ingenicfb.h"
#define POWER_IS_ON(pwr)        ((pwr) <= FB_BLANK_NORMAL)
struct board_gpio {
	short gpio;
	short active_level;
};

struct truly_device {
	/* ingenic frame buffer */
	struct device *dev;
	struct ingenicfb_platform_data *panel;
	/* common lcd framework */
	struct lcd_device *lcd;
	struct backlight_device *backlight;
	int power;
	struct regulator *vcc;
	struct board_gpio cs;
	struct board_gpio rst;
	/*struct board_gpio pwmen;*/
	struct board_gpio blken;
};
struct fb_videomode ingenicfb0_videomode = {
	.name = "240x240",
	.refresh = 60,
	.xres = 240,
	.yres = 240,
	.pixclock = KHZ2PICOS(30000),
	.left_margin = 0,
	.right_margin = 0,
	.upper_margin = 0,
	.lower_margin = 0,
	.hsync_len = 0,
	.vsync_len = 0,
	.sync = FB_SYNC_HOR_HIGH_ACT | FB_SYNC_VERT_HIGH_ACT,
	.vmode = FB_VMODE_NONINTERLACED,
	.flag = 0,
};
static struct smart_lcd_data_table truly_tft240240_data_table[] = {
	/* LCD init code */
	{SMART_CONFIG_CMD, 0x01},  //soft reset, 120 ms = 120 000 us
	{SMART_CONFIG_UDELAY, 1000},
	{SMART_CONFIG_CMD, 0x11},
	{SMART_CONFIG_UDELAY, 5000},	  /* sleep out 5 ms  */
	{SMART_CONFIG_CMD, 0x36},
#ifdef	CONFIG_TRULY_240X240_ROTATE_180
	/*{0x36, 0xc0, 2, 0}, //40*/
	{SMART_CONFIG_DATA, 0xd0}, //40
#else
	{SMART_CONFIG_DATA, 0x00}, //40
#endif
	{SMART_CONFIG_CMD, 0x2a},
	{SMART_CONFIG_DATA, 0x00},
	{SMART_CONFIG_DATA, 0x00},
	{SMART_CONFIG_DATA, 0x00},
	{SMART_CONFIG_DATA, 0xef},
	{SMART_CONFIG_CMD, 0x2b},
	{SMART_CONFIG_DATA, 0x00},
	{SMART_CONFIG_DATA, 0x00},
	{SMART_CONFIG_DATA, 0x00},
	{SMART_CONFIG_DATA, 0xef},
	{SMART_CONFIG_CMD, 0x3a},
#if defined(CONFIG_SLCD_TRULY_18BIT)  //if 18bit/pixel unusual. try to use 16bit/pixel
	{SMART_CONFIG_DATA, 0x06}, //6-6-6
#else
	{SMART_CONFIG_DATA, 0x05}, //5-6-5
#endif
	//	{SMART_CONFIG_DATA, 0x55},
	{SMART_CONFIG_CMD, 0xb2},
	{SMART_CONFIG_DATA, 0x7f},
	{SMART_CONFIG_DATA, 0x7f},
	{SMART_CONFIG_DATA, 0x01},
	{SMART_CONFIG_DATA, 0xde},
	{SMART_CONFIG_DATA, 0x33},
	{SMART_CONFIG_CMD, 0xb3},
	{SMART_CONFIG_DATA, 0x10},
	{SMART_CONFIG_DATA, 0x05},
	{SMART_CONFIG_DATA, 0x0f},
	{SMART_CONFIG_CMD, 0xb4},
	{SMART_CONFIG_DATA, 0x0b},
	{SMART_CONFIG_CMD, 0xb7},
	{SMART_CONFIG_DATA, 0x35},
	{SMART_CONFIG_CMD, 0xbb},
	{SMART_CONFIG_DATA, 0x28}, //23
	{SMART_CONFIG_CMD, 0xbc},
	{SMART_CONFIG_DATA, 0xec},
	{SMART_CONFIG_CMD, 0xc0},
	{SMART_CONFIG_DATA, 0x2c},
	{SMART_CONFIG_CMD, 0xc2},
	{SMART_CONFIG_DATA, 0x01},
	{SMART_CONFIG_CMD, 0xc3},
	{SMART_CONFIG_DATA, 0x1e}, //14
	{SMART_CONFIG_CMD, 0xc4},
	{SMART_CONFIG_DATA, 0x20},
	{SMART_CONFIG_CMD, 0xc6},
	{SMART_CONFIG_DATA, 0x14},
	{SMART_CONFIG_CMD, 0xd0},
	{SMART_CONFIG_DATA, 0xa4},
	{SMART_CONFIG_DATA, 0xa1},
	{SMART_CONFIG_CMD, 0xe0},
	{SMART_CONFIG_DATA, 0xd0},
	{SMART_CONFIG_DATA, 0x00},
	{SMART_CONFIG_DATA, 0x00},
	{SMART_CONFIG_DATA, 0x08},
	{SMART_CONFIG_DATA, 0x07},
	{SMART_CONFIG_DATA, 0x05},
	{SMART_CONFIG_DATA, 0x29},
	{SMART_CONFIG_DATA, 0x54},
	{SMART_CONFIG_DATA, 0x41},
	{SMART_CONFIG_DATA, 0x3c},
	{SMART_CONFIG_DATA, 0x17},
	{SMART_CONFIG_DATA, 0x15},
	{SMART_CONFIG_DATA, 0x1a},
	{SMART_CONFIG_DATA, 0x20},
	{SMART_CONFIG_CMD, 0xe1},
	{SMART_CONFIG_DATA, 0xd0},
	{SMART_CONFIG_DATA, 0x00},
	{SMART_CONFIG_DATA, 0x00},
	{SMART_CONFIG_DATA, 0x08},
	{SMART_CONFIG_DATA, 0x07},
	{SMART_CONFIG_DATA, 0x04},
	{SMART_CONFIG_DATA, 0x29},
	{SMART_CONFIG_DATA, 0x44},
	{SMART_CONFIG_DATA, 0x42},
	{SMART_CONFIG_DATA, 0x3b},
	{SMART_CONFIG_DATA, 0x16},
	{SMART_CONFIG_DATA, 0x15},
	{SMART_CONFIG_DATA, 0x1b},
	{SMART_CONFIG_DATA, 0x1f},
	{SMART_CONFIG_CMD, 0x35}, // TE on
	{SMART_CONFIG_DATA, 0x00}, // TE mode: 0, mode1; 1, mode2
	//	{SMART_CONFIG_CMD, 0x34}, // TE off
	{SMART_CONFIG_CMD, 0x21},
	{SMART_CONFIG_CMD, 0x29}, //Display ON
	/* set window size*/
	//	{SMART_CONFIG_CMD, 0xcd},
	{SMART_CONFIG_CMD, 0x2a},
	{SMART_CONFIG_DATA, 0},
	{SMART_CONFIG_DATA, 0},
	{SMART_CONFIG_DATA, (239>> 8) & 0xff},
	{SMART_CONFIG_DATA, 239 & 0xff},
#ifdef	CONFIG_TRULY_240X240_ROTATE_180
	{SMART_CONFIG_CMD, 0x2b},
	{SMART_CONFIG_DATA, ((320-240)>>8)&0xff},
	{SMART_CONFIG_DATA, ((320-240)>>0)&0xff},
	{SMART_CONFIG_DATA, ((320-1)>>8) & 0xff},
	{SMART_CONFIG_DATA, ((320-1)>>0) & 0xff},
#else
	{SMART_CONFIG_CMD, 0x2b},
	{SMART_CONFIG_DATA, 0},
	{SMART_CONFIG_DATA, 0},
	{SMART_CONFIG_DATA, (239>> 8) & 0xff},
	{SMART_CONFIG_DATA, 239 & 0xff},
#endif
	{SMART_CONFIG_CMD, 0x2C},
	{SMART_CONFIG_CMD, 0x2C},
	{SMART_CONFIG_CMD, 0x2C},
	{SMART_CONFIG_CMD, 0x2C},
};

unsigned long truly_cmd_buf[]= {
	0x2C2C2C2C,
};

static struct ingenicfb_platform_data ingenicfb_pdata = {
	.num_modes = 1,
	.modes = &ingenicfb0_videomode,
	.lcd_type = LCD_TYPE_SLCD,
	.bpp    = 16,
	.width = 31,
	.height = 31,
	.pinmd  = 0,
	.smart_config.rsply_cmd_high       = 0,
	.smart_config.csply_active_high    = 0,
	.smart_config.newcfg_fmt_conv =  1,
	.smart_config.write_gram_cmd = truly_cmd_buf,
	.smart_config.length_cmd = ARRAY_SIZE(truly_cmd_buf),
	.smart_config.bus_width = 8,
	.smart_config.data_table_width = 8,
	.smart_config.length_data_table =  ARRAY_SIZE(truly_tft240240_data_table),
	.smart_config.data_table = truly_tft240240_data_table,
	.dither_enable = 0,
};

static void truly_power_reset(struct board_gpio *rst)
{
	gpio_direction_output(rst->gpio, rst->active_level);
	mdelay(20);
	gpio_direction_output(rst->gpio, !rst->active_level);
	mdelay(10);
}

static int truly_set_power(struct lcd_device *lcd, int power)
{
	struct truly_device *truly = lcd_get_data(lcd);
	struct board_gpio *cs = &truly->cs;
	struct board_gpio *rst = &truly->rst;
	if(POWER_IS_ON(power) && !POWER_IS_ON(truly->power)) {
		gpio_direction_output(cs->gpio, !cs->active_level);
		truly_power_reset(rst);
		mdelay(5);
		gpio_direction_output(cs->gpio, cs->active_level);
	}
	if(!POWER_IS_ON(power) && POWER_IS_ON(truly->power)) {
		gpio_direction_output(cs->gpio, cs->active_level);
		gpio_direction_output(rst->gpio, cs->active_level);
	}
/*	gpio_direction_output(truly->pwmen.gpio, 1);*/
	gpio_direction_output(truly->blken.gpio, 1);
	truly->power = power;
	return 0;
}

static int truly_get_power(struct lcd_device *lcd)
{
	struct truly_device *truly = lcd_get_data(lcd);
	return truly->power;
}

static struct lcd_ops truly_lcd_ops = {
	.set_power = truly_set_power,
	.get_power = truly_get_power,
};


static int of_truly_parse(struct device *dev)
{
	struct truly_device *truly = dev_get_drvdata(dev);
	struct device_node *np = dev->of_node;
	enum of_gpio_flags flags;
	int ret = 0;

	truly->cs.gpio = of_get_named_gpio_flags(np, "ingenic,cs-gpio", 0, &flags);
	if(gpio_is_valid(truly->cs.gpio)) {
		truly->cs.active_level = (flags == OF_GPIO_ACTIVE_LOW) ? 0 : 1;
		ret = gpio_request(truly->cs.gpio, "cs");
		if(ret < 0) {
			dev_err(dev, "Failed to request cs pin!\n");
			return -1;
		}
	} else {
		dev_warn(dev, "invalid gpio cs.gpio: %d\n", truly->cs.gpio);
		return -1;
	}

	truly->rst.gpio = of_get_named_gpio_flags(np, "ingenic,rst-gpio", 0, &flags);
	if(gpio_is_valid(truly->rst.gpio)) {
		truly->rst.active_level = (flags == OF_GPIO_ACTIVE_LOW) ? 0 : 1;
		ret = gpio_request(truly->rst.gpio, "rst");
		if(ret < 0) {
			dev_err(dev, "Failed to request rst pin!\n");
			goto err_request_rst;
		}
	} else {
		dev_warn(dev, "invalid gpio rst.gpio: %d\n", truly->cs.gpio);
		goto err_request_rst;
	}

	truly->blken.gpio = of_get_named_gpio_flags(np, "ingenic,blken-gpio", 0, &flags);
	if(gpio_is_valid(truly->blken.gpio)) {
		truly->blken.active_level = (flags == OF_GPIO_ACTIVE_LOW) ? 0 : 1;
		ret = gpio_request(truly->blken.gpio, "blken");
		if(ret < 0) {
			dev_err(dev, "Failed to request blken pin!\n");
			goto err_request_blken;
		}
	} else {
		dev_warn(dev, "invalid gpio blken.gpio: %d\n", truly->blken.gpio);
		goto err_request_blken;
	}

	return 0;
err_request_blken:
	gpio_free(truly->blken.gpio);
err_request_rst:
	gpio_free(truly->cs.gpio);
	return -1;
}

static int lcd_inited_by_uboot( void )
{
	if (*(unsigned int*)(0xb3050000 + 0x30) & (1 << 3))
		return 1;
	else
		return 0;
}

static int truly_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct truly_device *truly;

	truly = kzalloc(sizeof(struct truly_device), GFP_KERNEL);
	if(truly == NULL) {
		dev_err(&pdev->dev, "Faile to alloc memory!");
		return -ENOMEM;
	}
	truly->dev = &pdev->dev;
	dev_set_drvdata(&pdev->dev, truly);

	ret = of_truly_parse(&pdev->dev);
	if(ret < 0) {
		goto err_of_parse;
	}

	ret = ingenicfb_register_panel(&ingenicfb_pdata);
	if(ret < 0) {
		dev_err(&pdev->dev, "Failed to register lcd panel!\n");
	}

	truly->lcd = lcd_device_register("truly_tft240240_lcd", &pdev->dev, truly, &truly_lcd_ops);
	if(IS_ERR_OR_NULL(truly->lcd)) {
		dev_err(&pdev->dev, "Error register lcd!\n");
		ret = -EINVAL;
		goto err_lcd_register;
	}

	/* TODO: should this power status sync from uboot */
	truly->power = FB_BLANK_POWERDOWN;
	if(!lcd_inited_by_uboot())
		truly_set_power(truly->lcd, FB_BLANK_UNBLANK);
	truly->power = FB_BLANK_UNBLANK;

	return 0;

err_lcd_register:
err_of_parse:
	kfree(truly);
	return ret;
}

static int truly_remove(struct platform_device *pdev)
{
	struct truly_device *truly = dev_get_drvdata(&pdev->dev);

	truly_set_power(truly->lcd, FB_BLANK_POWERDOWN);
	return 0;
}

#ifdef CONFIG_PM
static int truly_suspend(struct device *dev)
{
	struct truly_device *truly = dev_get_drvdata(dev);
	truly_set_power(truly->lcd, FB_BLANK_POWERDOWN);
	return 0;
}

static int truly_resume(struct device *dev)
{
	struct truly_device *truly = dev_get_drvdata(dev);
	truly_set_power(truly->lcd, FB_BLANK_UNBLANK);
	return 0;
}

static const struct dev_pm_ops truly_pm_ops = {
	.suspend = truly_suspend,
	.resume = truly_resume,
};
#endif
static const struct of_device_id truly_of_match[] = {
	{ .compatible = "ingenic,truly_tft240240_2_e", },
	{},
};
static struct platform_driver truly_driver = {
	.probe		= truly_probe,
	.remove		= truly_remove,
	.driver		= {
		.name	= "truly_tft240240_2_e",
		.of_match_table = truly_of_match,
#ifdef CONFIG_PM
		.pm = &truly_pm_ops,
#endif
	},
};
module_platform_driver(truly_driver);

/*
 *  Board-specific setup code for the AT91SAM3X8H Evaluation Kit family
 *
 *  Covers: * AT91SAM3X8H-EK board
 *
*
 *  On-Chip devices setup code for the AT91SAM3X8H family
 *
 *
 * (C) Copyright 2012
 * Olivier Schonken <olivier.schonken@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <linux/types.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/leds.h>
#include <linux/atmel-mci.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <video/hx8347_fb.h>
#include <mach/hardware.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/irq.h>
#include <asm/hardware/nvic.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/board.h>
#include <mach/at91sam3_smc.h>
#include <mach/at91_shdwc.h>
#include <mach/system_rev.h>

#include "sam3_smc.h"
#include "generic.h"

static void __init ek_init_early(void)
{
	at91_map_io();
	/* Initialize processor: 12.000 MHz crystal */
	at91_initialize(12000000);

	/* DGBU on ttyS0. (Rx & Tx only) */
	at91_register_uart(0, 0, 0);

	/* set serial console to ttyS0 (ie, DBGU) */
	at91_set_serial_console(0);
}


/*
 * USB HS Device port
 */
static struct usba_platform_data __initdata ek_usba_udc_data = {
	.vbus_pin	= AT91_PIN_PB10,
};

/*
 * SPI devices.
 */
static struct spi_board_info ek_spi_devices[] = {
#if defined(CONFIG_TOUCHSCREEN_ADS7846) || defined(CONFIG_TOUCHSCREEN_ADS7846_MODULE)
	{
		.modalias	= "ads7846",
		.chip_select	= 0,
		.max_speed_hz	= 125000 * 26,	/* (max sample rate @ 3V) * (cmd + data + overhead) */
		.bus_num	= 0,
		.platform_data	= &ads_info,
		.irq		= AT91SAM3X_INT_SPI0,
		.controller_data = (void *) AT91_PIN_PA28,	/* CS pin */
	},
#endif

};
#if defined(CONFIG_SPI_ATMEL) || defined(CONFIG_SPI_ATMEL_MODULE)
/*
 * ADS7846 Touchscreen
 */
#if defined(CONFIG_TOUCHSCREEN_ADS7846) || defined(CONFIG_TOUCHSCREEN_ADS7846_MODULE)

static int ads7843_pendown_state(void)
{
	return !at91_get_gpio_value(AT91_PIN_PA31);	/* Touchscreen PENIRQ */
}

static struct ads7846_platform_data ads_info = {
	.model			= 7843,
	.x_min			= 150,
	.x_max			= 3830,
	.y_min			= 190,
	.y_max			= 3830,
	.vref_delay_usecs	= 100,
	.x_plate_ohms		= 450,
	.y_plate_ohms		= 250,
	.pressure_max		= 15000,
	.debounce_max		= 1,
	.debounce_rep		= 0,
	.debounce_tol		= (~0),
	.get_pendown_state	= ads7843_pendown_state,
};

static void __init ek_add_device_ts(void)
{
	at91_set_gpio_input(AT91_PIN_PA31, 1);	/* External IRQ0, with pullup */
	at91_set_gpio_input(AT91_PIN_PA30, 1);	/* Touchscreen BUSY signal */
}
#else
static void __init ek_add_device_ts(void) {}
#endif

#endif  //SPI Devices

/*
 * MCI (SD/MMC)
 */
static struct mci_platform_data __initdata mci0_data = {
	.slot[0] = {
		.bus_width	= 8,
		.detect_pin	= AT91_PIN_PE6,
		.wp_pin		= -EINVAL,
	},
};


/*
 * MACB Ethernet device
 */
static struct macb_platform_data __initdata ek_macb_data = {
	.phy_irq_pin	= AT91_PIN_PA5,
	.is_rmii	= 1,
};


/*
 * NAND flash
 */
static struct mtd_partition __initdata ek_nand_partition[] = {
	{
		.name	= "Partition 1",
		.offset	= 0,
		.size	= SZ_64M,
	},
	{
		.name	= "Partition 2",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= MTDPART_SIZ_FULL,
	},
};

/* det_pin is not connected */
static struct atmel_nand_data __initdata ek_nand_data = {
	.ale		= 21,
	.cle		= 22,
	.rdy_pin	= AT91_PIN_PA2,
	.enable_pin	= AT91_PIN_PA6,
	.det_pin	= -EINVAL,
	.ecc_mode	= NAND_ECC_SOFT,
	.bus_width_16	= 0,
	.parts		= ek_nand_partition,
	.num_parts	= ARRAY_SIZE(ek_nand_partition),
};

static struct sam3_smc_config __initdata ek_nand_smc_config = {
	.ncs_read_setup		= 0,
	.nrd_setup		= 2,
	.ncs_write_setup	= 0,
	.nwe_setup		= 2,

	.ncs_read_pulse		= 4,
	.nrd_pulse		= 4,
	.ncs_write_pulse	= 4,
	.nwe_pulse		= 4,

	.read_cycle		= 7,
	.write_cycle		= 7,

	.mode			= AT91_SMC_READMODE | AT91_SMC_WRITEMODE | AT91_SMC_EXNWMODE_DISABLE,
	.tdf_cycles		= 3,
};

static void __init ek_add_device_nand(void)
{
	ek_nand_data.bus_width_16 = 0;//board_have_nand_16bit();
	/* setup bus-width (8 or 16) */
	if (ek_nand_data.bus_width_16)
		ek_nand_smc_config.mode |= AT91_SMC_DBW_16;
	else
		ek_nand_smc_config.mode |= AT91_SMC_DBW_8;

	/* configure chip-select 3 (NAND) */
	sam3_smc_configure(0, 0, &ek_nand_smc_config);

	at91_add_device_nand(&ek_nand_data);
}

/*
 * Touchscreen
 */
static struct at91_tsadcc_data ek_tsadcc_data = {
	.adc_clock		= 300000,
	.pendet_debounce	= 0x0d,
	.ts_sample_hold_time	= 0x0a,
};


/*
 * GPIO Buttons
 */
#if defined(CONFIG_KEYBOARD_GPIO) || defined(CONFIG_KEYBOARD_GPIO_MODULE)
static struct gpio_keys_button ek_buttons[] = {
	{	/* BP1, "leftclic" */
		.code		= BTN_LEFT,
		.gpio		= AT91_PIN_PE7,
		.active_low	= 1,
		.desc		= "left_click",
		.wakeup		= 1,
	},
	{	/* BP2, "rightclic" */
		.code		= BTN_RIGHT,
		.gpio		= AT91_PIN_PB23,
		.active_low	= 1,
		.desc		= "right_click",
		.wakeup		= 1,
	},
};

static struct gpio_keys_platform_data ek_button_data = {
	.buttons	= ek_buttons,
	.nbuttons	= ARRAY_SIZE(ek_buttons),
};

static struct platform_device ek_button_device = {
	.name		= "gpio-keys",
	.id		= -1,
	.num_resources	= 0,
	.dev		= {
		.platform_data	= &ek_button_data,
	}
};

static void __init ek_add_device_buttons(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(ek_buttons); i++) {
		at91_set_GPIO_periph(ek_buttons[i].gpio, 1);
		at91_set_deglitch(ek_buttons[i].gpio, 1);
	}

	platform_device_register(&ek_button_device);
}
#else
static void __init ek_add_device_buttons(void) {}
#endif

/*
 * LEDs ... these could all be PWM-driven, for variable brightness
 */
static struct gpio_led ek_leds[] = {
		{	/* "top" led, red, powerled */
			.name			= "d2",
			.gpio			= AT91_PIN_PA12,
			.active_low		= 1,
			.default_trigger	= "heartbeat",
		},
		{	/* "left" led, green, userled2, pwm3 */
			.name			= "d3",
			.gpio			= AT91_PIN_PB12,
			.active_low		= 1,
			.default_trigger	= "nand-disk",
		},
		{	/* "right" led, green, userled1, pwm1 */
			.name			= "d4",
			.gpio			= AT91_PIN_PB13,
			.active_low		= 1,
			.default_trigger	= "mmc0",
		},
#if 0
		{	/* "right" led, green, userled1, pwm1 */
			.name			= "backlight",
			.gpio			= AT91_PIN_PB27,
			.active_low		= 0,
		},
#endif
};

/* OLED */
static struct hx8347_device_data __initdata hx8347_data =
{
	.cs_addr =		AT91_CHIPSELECT_2,
	.pin_cs  =		AT91_PIN_PB24,
	.pin_rs =		AT91_PIN_PC22,
	.pin_nwe =      AT91_PIN_PC18,
	.pin_nrd =      AT91_PIN_PA29,
	.pin_backlight = AT91_PIN_PB27,
};

static struct i2c_board_info __initdata ek_i2c_devices[] = {
        {
                I2C_BOARD_INFO("24c512", 0x50)
        },
        {
                I2C_BOARD_INFO("wm8731", 0x1b)
        },
};


static void __init ek_board_init(void)
{
	/* Serial */
	at91_set_gpio_output(AT91_PIN_PE14, 0); /*Enable USART 1*/
	at91_register_uart(AT91SAM3X_US0_ID,1,0);
	at91_add_device_serial();
	/* USB HS Device TODO: Change to OTG*/
	//at91_add_device_usba(&ek_usba_udc_data);
	/* SPI */
	//at91_add_device_spi(ek_spi_devices, ARRAY_SIZE(ek_spi_devices));  //TODO:Touchscreen
	/* MMC */
	at91_add_device_mci(0, &mci0_data);
	/* Ethernet */
	at91_add_device_eth(&ek_macb_data);
	/* NAND */
	ek_add_device_nand();
	/* I2C */
	at91_add_device_i2c(0, ek_i2c_devices, ARRAY_SIZE(ek_i2c_devices));
	/* Push Buttons */
	ek_add_device_buttons();
	/* LEDs */
	at91_gpio_leds(ek_leds, ARRAY_SIZE(ek_leds));
	/* Hx8347 */
	add_device_hx8347(&hx8347_data);
	at91_add_device_ssc(AT91SAM3X_SSC_ID, ATMEL_SSC_TX);

}

MACHINE_START(AT91SAM3X8HEK, "Atmel AT91SAM3X8H-EK")
	.timer		= &at91sam3x_timer,
	.map_io		= at91_map_io,
	.init_early	= ek_init_early,
	.init_irq	= at91_init_irq_default,
	.init_machine	= ek_board_init,
MACHINE_END

/*
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

#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <linux/dma-mapping.h>
#include <linux/gpio.h>
#include <linux/clk.h>
#include <linux/platform_device.h>
#include <linux/i2c-gpio.h>
#include <linux/atmel-mci.h>

#include <linux/fb.h>
#include <video/hx8347_fb.h>

#include <mach/board.h>
#include <mach/at91sam3x8h.h>
#include <mach/at91sam3x8h_matrix.h>
#include <mach/at91_matrix.h>
#include <mach/at91sam3_smc.h>
#include <mach/at_hdmac.h>
#include <mach/atmel-mci.h>

#include "generic.h"
#include "clock.h"


/* --------------------------------------------------------------------
 *  HDMAC - AHB DMA Controller
 * -------------------------------------------------------------------- */

#if defined(CONFIG_AT_HDMAC) || defined(CONFIG_AT_HDMAC_MODULE)
static u64 hdmac_dmamask = DMA_BIT_MASK(32);

static struct resource hdmac_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_DMAC,
		.end	= AT91SAM3X_BASE_DMAC + SZ_512 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_INT_DMAC,
		.end	= AT91SAM3X_INT_DMAC,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at_hdmac_device = {
	.name		= "at_hdmac",
	.id		= -1,
	.dev		= {
				.dma_mask		= &hdmac_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= hdmac_resources,
	.num_resources	= ARRAY_SIZE(hdmac_resources),
};

void __init at91_add_device_hdmac(void)
{
#if defined(CONFIG_OF)
	struct device_node *of_node =
		of_find_node_by_name(NULL, "dma-controller");

	if (of_node)
		of_node_put(of_node);
	else
#endif
		platform_device_register(&at_hdmac_device);
}
#else
void __init at91_add_device_hdmac(void) {}
#endif


/* --------------------------------------------------------------------
 *  Ethernet
 * -------------------------------------------------------------------- */

#if defined(CONFIG_MACB) || defined(CONFIG_MACB_MODULE)
static u64 eth_dmamask = DMA_BIT_MASK(32);
static struct macb_platform_data eth_data;

static struct resource eth_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_EMAC,
		.end	= AT91SAM3X_BASE_EMAC + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_EMAC_ID,
		.end	= AT91SAM3X_EMAC_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at91sam3x8h_eth_device = {
	.name		= "macb",
	.id		= -1,
	.dev		= {
				.dma_mask		= &eth_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
				.platform_data		= &eth_data,
	},
	.resource	= eth_resources,
	.num_resources	= ARRAY_SIZE(eth_resources),
};

void __init at91_add_device_eth(struct macb_platform_data *data)
{
	if (!data)
		return;

	if (gpio_is_valid(data->phy_irq_pin)) {
		at91_set_gpio_input(data->phy_irq_pin, 0);
		at91_set_deglitch(data->phy_irq_pin, 1);
	}

	at91_set_gpio_output(AT91_PIN_PD17, 1);
	/* Pins used for MII and RMII */
	at91_set_A_periph(AT91_PIN_PB0, 0);	/* ETXCK_EREFCK */
	at91_set_A_periph(AT91_PIN_PB4, 0);	/* ERXDV */
	at91_set_A_periph(AT91_PIN_PB5, 0);	/* ERX0 */
	at91_set_A_periph(AT91_PIN_PB6, 0);	/* ERX1 */
	at91_set_A_periph(AT91_PIN_PB7, 0);	/* ERXER */
	at91_set_A_periph(AT91_PIN_PB1, 0);	/* ETXEN */
	at91_set_A_periph(AT91_PIN_PB2, 0);	/* ETX0 */
	at91_set_A_periph(AT91_PIN_PB3, 0);	/* ETX1 */
	at91_set_A_periph(AT91_PIN_PB9, 0);	/* EMDIO */
	at91_set_A_periph(AT91_PIN_PB8, 0);	/* EMDC */

	eth_data = *data;
	platform_device_register(&at91sam3x8h_eth_device);
}
#else
void __init at91_add_device_eth(struct macb_platform_data *data) {}
#endif


/* --------------------------------------------------------------------
 *  MMC / SD
 * -------------------------------------------------------------------- */

#if defined(CONFIG_MMC_ATMELMCI) || defined(CONFIG_MMC_ATMELMCI_MODULE)
static u64 mmc_dmamask = DMA_BIT_MASK(32);
static struct mci_platform_data mmc0_data, mmc1_data;

static struct resource mmc0_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_HSMCI,
		.end	= AT91SAM3X_BASE_HSMCI + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_HSMCI_ID,
		.end	= AT91SAM3X_HSMCI_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at91sam3x8h_mmc0_device = {
	.name		= "atmel_mci",
	.id		= 0,
	.dev		= {
				.dma_mask		= &mmc_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
				.platform_data		= &mmc0_data,
	},
	.resource	= mmc0_resources,
	.num_resources	= ARRAY_SIZE(mmc0_resources),
};

/* Consider only one slot : slot 0 */
void __init at91_add_device_mci(short mmc_id, struct mci_platform_data *data)
{

	if (!data)
		return;

	/* Must have at least one usable slot */
	if (!data->slot[0].bus_width)
		return;

#if defined(CONFIG_AT_HDMAC) || defined(CONFIG_AT_HDMAC_MODULE)
	{
	struct at_dma_slave	*atslave;
	struct mci_dma_data	*alt_atslave;

	alt_atslave = kzalloc(sizeof(struct mci_dma_data), GFP_KERNEL);
	atslave = &alt_atslave->sdata;

	/* DMA slave channel configuration */
	atslave->dma_dev = &at_hdmac_device.dev;
	atslave->cfg = ATC_FIFOCFG_HALFFIFO
			| ATC_SRC_H2SEL_HW | ATC_DST_H2SEL_HW;
	atslave->ctrla = ATC_SCSIZE_16 | ATC_DCSIZE_16;
	if (mmc_id == 0)	/* MCI0 */
		atslave->cfg |= ATC_SRC_PER(AT_DMA_ID_MCI0)
			      | ATC_DST_PER(AT_DMA_ID_MCI0);

	data->dma_slave = alt_atslave;
	}
#endif


	/* input/irq */
	if (gpio_is_valid(data->slot[0].detect_pin)) {
		at91_set_gpio_input(data->slot[0].detect_pin, 1);
		at91_set_deglitch(data->slot[0].detect_pin, 1);
	}
	if (gpio_is_valid(data->slot[0].wp_pin))
		at91_set_gpio_input(data->slot[0].wp_pin, 1);

	if (mmc_id == 0) {		/* MCI0 */

		/* CLK */
		at91_set_A_periph(AT91_PIN_PA19, 0);

		/* CMD */
		at91_set_A_periph(AT91_PIN_PA20, 1);

		/* DAT0, maybe DAT1..DAT3 and maybe DAT4..DAT7 */
		at91_set_A_periph(AT91_PIN_PA21, 1);
		if (data->slot[0].bus_width >= 4) {
			at91_set_A_periph(AT91_PIN_PA22, 1);
			at91_set_A_periph(AT91_PIN_PA23, 1);
			at91_set_A_periph(AT91_PIN_PA24, 1);
			if (data->slot[0].bus_width == 8) {
				at91_set_B_periph(AT91_PIN_PD0, 1);
				at91_set_B_periph(AT91_PIN_PD1, 1);
				at91_set_B_periph(AT91_PIN_PD2, 1);
				at91_set_B_periph(AT91_PIN_PD3, 1);
			}
		}

		mmc0_data = *data;
		platform_device_register(&at91sam3x8h_mmc0_device);

	}
}
#else
void __init at91_add_device_mci(short mmc_id, struct mci_platform_data *data) {}
#endif


/* --------------------------------------------------------------------
 *  NAND / SmartMedia
 * -------------------------------------------------------------------- */

#if defined(CONFIG_MTD_NAND_ATMEL) || defined(CONFIG_MTD_NAND_ATMEL_MODULE)
static struct atmel_nand_data nand_data;

#define NAND_BASE	AT91_CHIPSELECT_0

static struct resource nand_resources[] = {
	[0] = {
		.start	= NAND_BASE,
		.end	= NAND_BASE + SZ_256M - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91_ECC,
		.end	= AT91_ECC + SZ_512 - 1,
		.flags	= IORESOURCE_MEM,
	}
};

static struct platform_device at91sam3x8h_nand_device = {
	.name		= "atmel_nand",
	.id		= -1,
	.dev		= {
				.platform_data	= &nand_data,
	},
	.resource	= nand_resources,
	.num_resources	= ARRAY_SIZE(nand_resources),
};

void __init at91_add_device_nand(struct atmel_nand_data *data)
{
	unsigned long csa;

	if (!data)
		return;

	/* enable pin */
	if (gpio_is_valid(data->enable_pin))
		at91_set_gpio_output(data->enable_pin, 1);

	/* ready/busy pin */
	if (gpio_is_valid(data->rdy_pin))
		at91_set_gpio_input(data->rdy_pin, 1);

	/* card detect pin */
	if (gpio_is_valid(data->det_pin))
		at91_set_gpio_input(data->det_pin, 1);

	nand_data = *data;
	platform_device_register(&at91sam3x8h_nand_device);
}
#else
void __init at91_add_device_nand(struct atmel_nand_data *data) {}
#endif

/* --------------------------------------------------------------------
 *  LCD Controller
 * -------------------------------------------------------------------- */

#if defined(CONFIG_FB_HX8347)

static u64 hx8347_dmamask = DMA_BIT_MASK(32);

static struct resource hx8347_resources[] = {
	[0] = {
		.start	= AT91_CHIPSELECT_2,
		.end	= AT91_CHIPSELECT_2 + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device hx8347_fb_device = {
	.name	= "hx8347_fb",
	.id		= 0,
	.dev	= {
		.dma_mask			= &hx8347_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource		= hx8347_resources,
	.num_resources	= ARRAY_SIZE(hx8347_resources),
};

static struct sam3_smc_config __initdata hx8347_smc_config = {
	.ncs_read_setup		= 7,
	.nrd_setup			= 7,
	.ncs_write_setup	= 2,
	.nwe_setup			= 2,

	.ncs_read_pulse		= 30,
	.nrd_pulse			= 30,
	.ncs_write_pulse	= 3,
	.nwe_pulse			= 3,

	.read_cycle			= 37,
	.write_cycle		= 6,

	.tdf_cycles			= 3,
	.mode				= AT91_SMC_READMODE | AT91_SMC_WRITEMODE |
						  AT91_SMC_EXNWMODE_DISABLE | AT91_SMC_BAT_SELECT |
						  AT91_SMC_DBW_16,
};

void __init add_device_hx8347(struct hx8347_device_data *data)
{
	int ret;

	/* Configure chip-select 2 (0x62000000) */
	sam3_smc_configure(0, 2, &hx8347_smc_config);

	/* Configure control lines */
	at91_set_B_periph(AT91_PIN_PA29, 1);/** EBI NRD pin */
	at91_set_A_periph(AT91_PIN_PC18, 1);/** EBI NWE pin */
	at91_set_B_periph(AT91_PIN_PB24, 1);/** EBI NCS2 pin */
	at91_set_A_periph(AT91_PIN_PC22, 1);/** EBI pin for LCD RS / A1*/

	hx8347_fb_device.dev.platform_data = data;

	ret = platform_device_register(&hx8347_fb_device);
	if (ret < 0)
		printk("HX8347 failed platform_device_register - %d\n", ret);
	else
		printk("HX8347 registered platform device\n");
}
#else
void __init add_device_hx8347(struct hx8347_device_data *data) {}
#endif

/* --------------------------------------------------------------------
 *  TWI (i2c)
 * -------------------------------------------------------------------- */

/*
 * Prefer the GPIO code since the TWI controller isn't robust
 * (gets overruns and underruns under load) and can only issue
 * repeated STARTs in one scenario (the driver doesn't yet handle them).
 */
#if defined(CONFIG_I2C_GPIO) || defined(CONFIG_I2C_GPIO_MODULE)
static struct i2c_gpio_platform_data pdata_i2c0 = {
	.sda_pin		= AT91_PIN_PA17,
	.sda_is_open_drain	= 1,
	.scl_pin		= AT91_PIN_PA18,
	.scl_is_open_drain	= 1,
	.udelay			= 5,		/* ~100 kHz */
};

static struct platform_device at91sam3x8h_twi0_device = {
	.name			= "i2c-gpio",
	.id			= 0,
	.dev.platform_data	= &pdata_i2c0,
};

static struct i2c_gpio_platform_data pdata_i2c1 = {
	.sda_pin		= AT91_PIN_PB12,
	.sda_is_open_drain	= 1,
	.scl_pin		= AT91_PIN_PB13,
	.scl_is_open_drain	= 1,
	.udelay			= 5,		/* ~100 kHz */
};

static struct platform_device at91sam3x8h_twi1_device = {
	.name			= "i2c-gpio",
	.id			= 1,
	.dev.platform_data	= &pdata_i2c1,
};

void __init at91_add_device_i2c(short i2c_id, struct i2c_board_info *devices, int nr_devices)
{
	i2c_register_board_info(i2c_id, devices, nr_devices);

	if (i2c_id == 0) {
		at91_set_GPIO_periph(AT91_PIN_PA17, 1);		/* TWD (SDA) */
		at91_set_multi_drive(AT91_PIN_PA17, 1);

		at91_set_GPIO_periph(AT91_PIN_PA18, 1);		/* TWCK (SCL) */
		at91_set_multi_drive(AT91_PIN_PA18, 1);

		platform_device_register(&at91sam3x8h_twi0_device);
	} else {
		at91_set_GPIO_periph(AT91_PIN_PB12, 1);		/* TWD (SDA) */
		at91_set_multi_drive(AT91_PIN_PB12, 1);

		at91_set_GPIO_periph(AT91_PIN_PB13, 1);		/* TWCK (SCL) */
		at91_set_multi_drive(AT91_PIN_PB13, 1);

		platform_device_register(&at91sam3x8h_twi1_device);
	}
}

#elif defined(CONFIG_I2C_AT91) || defined(CONFIG_I2C_AT91_MODULE)
static struct resource twi0_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_TWI0,
		.end	= AT91SAM3X_BASE_TWI0 + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_TWI0_ID,
		.end	= AT91SAM3X_TWI0_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at91sam3x8h_twi0_device = {
	.name		= "at91_i2c",
	.id		= 0,
	.resource	= twi0_resources,
	.num_resources	= ARRAY_SIZE(twi0_resources),
};

static struct resource twi1_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_TWI1,
		.end	= AT91SAM3X_BASE_TWI1 + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_TWI1_ID,
		.end	= AT91SAM3X_TWI1_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at91sam3x8h_twi1_device = {
	.name		= "at91_i2c",
	.id		= 1,
	.resource	= twi1_resources,
	.num_resources	= ARRAY_SIZE(twi1_resources),
};

void __init at91_add_device_i2c(short i2c_id, struct i2c_board_info *devices, int nr_devices)
{
	i2c_register_board_info(i2c_id, devices, nr_devices);

	/* pins used for TWI interface */
	if (i2c_id == 0) {
		at91_set_A_periph(AT91_PIN_PA17, 0);		/* TWD */
		at91_set_multi_drive(AT91_PIN_PA17, 1);

		at91_set_A_periph(AT91_PIN_PA18, 0);		/* TWCK */
		at91_set_multi_drive(AT91_PIN_PA18, 1);

		platform_device_register(&at91sam3x8h_twi0_device);
	} else {
		at91_set_A_periph(AT91_PIN_PB12, 0);		/* TWD */
		at91_set_multi_drive(AT91_PIN_PB12, 1);

		at91_set_A_periph(AT91_PIN_PB13, 0);		/* TWCK */
		at91_set_multi_drive(AT91_PIN_PB13, 1);

		platform_device_register(&at91sam3x8h_twi1_device);
	}
}
#else
void __init at91_add_device_i2c(short i2c_id, struct i2c_board_info *devices, int nr_devices) {}
#endif


/* --------------------------------------------------------------------
 *  SPI
 * -------------------------------------------------------------------- */

#if defined(CONFIG_SPI_ATMEL) || defined(CONFIG_SPI_ATMEL_MODULE)
static u64 spi_dmamask = DMA_BIT_MASK(32);

static struct resource spi0_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_SPI0,
		.end	= AT91SAM3X_BASE_SPI0 + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_SPI0_ID,
		.end	= AT91SAM3X_SPI0_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at91sam3x8h_spi0_device = {
	.name		= "atmel_spi",
	.id		= 0,
	.dev		= {
				.dma_mask		= &spi_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= spi0_resources,
	.num_resources	= ARRAY_SIZE(spi0_resources),
};

static const unsigned spi0_standard_cs[4] = { AT91_PIN_PA28, AT91_PIN_PA29, AT91_PIN_PA30, AT91_PIN_PA31 };

static struct resource spi1_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_SPI1,
		.end	= AT91SAM3X_BASE_SPI1 + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_SPI1_ID,
		.end	= AT91SAM3X_SPI1_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at91sam3x8h_spi1_device = {
	.name		= "atmel_spi",
	.id		= 1,
	.dev		= {
				.dma_mask		= &spi_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= spi1_resources,
	.num_resources	= ARRAY_SIZE(spi1_resources),
};

static const unsigned spi1_standard_cs[4] = { AT91_PIN_PE31, AT91_PIN_PF0, AT91_PIN_PF1, AT91_PIN_PF2 };

void __init at91_add_device_spi(struct spi_board_info *devices, int nr_devices)
{
	int i;
	unsigned long cs_pin;
	short enable_spi0 = 0;
	short enable_spi1 = 0;

	/* Choose SPI chip-selects */
	for (i = 0; i < nr_devices; i++) {
		if (devices[i].controller_data)
			cs_pin = (unsigned long) devices[i].controller_data;
		else if (devices[i].bus_num == 0)
			cs_pin = spi0_standard_cs[devices[i].chip_select];
		else
			cs_pin = spi1_standard_cs[devices[i].chip_select];

		if (devices[i].bus_num == 0)
			enable_spi0 = 1;
		else
			enable_spi1 = 1;

		/* enable chip-select pin */
		at91_set_gpio_output(cs_pin, 1);

		/* pass chip-select pin to driver */
		devices[i].controller_data = (void *) cs_pin;
	}

	spi_register_board_info(devices, nr_devices);

	/* Configure SPI bus(es) */
	if (enable_spi0) {
		at91_set_A_periph(AT91_PIN_PA25, 0);	/* SPI0_MISO */
		at91_set_A_periph(AT91_PIN_PA26, 0);	/* SPI0_MOSI */
		at91_set_A_periph(AT91_PIN_PA27, 0);	/* SPI0_SPCK */

		platform_device_register(&at91sam3x8h_spi0_device);
	}
	if (enable_spi1) {
		at91_set_A_periph(AT91_PIN_PE28, 0);	/* SPI1_MISO */
		at91_set_A_periph(AT91_PIN_PE29, 0);	/* SPI1_MOSI */
		at91_set_A_periph(AT91_PIN_PE30, 0);	/* SPI1_SPCK */

		platform_device_register(&at91sam3x8h_spi1_device);
	}
}
#else
void __init at91_add_device_spi(struct spi_board_info *devices, int nr_devices) {}
#endif

/* --------------------------------------------------------------------
 *  Timer/Counter block
 * -------------------------------------------------------------------- */

#ifdef CONFIG_ATMEL_TCLIB
static struct resource tcb1_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_TC1,
		.end	= AT91SAM3X_BASE_TC1 + SZ_256 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_TC1_ID,
		.end	= AT91SAM3X_TC1_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at91sam3x8h_tcb1_device = {
	.name		= "atmel_tcb",
	.id		= 0,
	.resource	= tcb1_resources,
	.num_resources	= ARRAY_SIZE(tcb0_resources),
};

/* TCB1 begins with TC3 */
static struct resource tcb2_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_TC2,
		.end	= AT91SAM3X_BASE_TC2 + SZ_256 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_TC2_ID,
		.end	= AT91SAM3X_TC2_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at91sam3x8h_tcb2_device = {
	.name		= "atmel_tcb",
	.id		= 1,
	.resource	= tcb2_resources,
	.num_resources	= ARRAY_SIZE(tcb2_resources),
};

#if defined(CONFIG_OF)
static struct of_device_id tcb_ids[] = {
	{ .compatible = "atmel,at91rm9200-tcb" },
	{ /*sentinel*/ }
};
#endif

static void __init at91_add_device_tc(void)
{
#if defined(CONFIG_OF)
	struct device_node *np;

	np = of_find_matching_node(NULL, tcb_ids);
	if (np) {
		of_node_put(np);
		return;
	}
#endif

	platform_device_register(&at91sam3x8h_tcb1_device);
	platform_device_register(&at91sam3x8h_tcb2_device);
}
#else
static void __init at91_add_device_tc(void) { }
#endif


/* --------------------------------------------------------------------
 *  RTC
 * -------------------------------------------------------------------- */

#if defined(CONFIG_RTC_DRV_AT91RM9200) || defined(CONFIG_RTC_DRV_AT91RM9200_MODULE)
static struct resource rtc_resources[] = {
	[0] = {
		.start	= AT91SAM3X8H_BASE_RTC,
		.end	= AT91SAM3X8H_BASE_RTC + SZ_256 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91_ID_SYS,
		.end	= AT91_ID_SYS,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at91sam3x8h_rtc_device = {
	.name		= "at91_rtc",
	.id		= -1,
	.resource	= rtc_resources,
	.num_resources	= ARRAY_SIZE(rtc_resources),
};

static void __init at91_add_device_rtc(void)
{
	platform_device_register(&at91sam3x8h_rtc_device);
}
#else
static void __init at91_add_device_rtc(void) {}
#endif


/* --------------------------------------------------------------------
 *  RTT
 * -------------------------------------------------------------------- */

static struct resource rtt_resources[] = {
	{
		.start	= AT91SAM3X_BASE_RTT,
		.end	= AT91SAM3X_BASE_RTT + SZ_32 - 1,
		.flags	= IORESOURCE_MEM,
	}, {
		.flags	= IORESOURCE_MEM,
	}
};

static struct platform_device at91sam3x8h_rtt_device = {
	.name		= "at91_rtt",
	.id		= 0,
	.resource	= rtt_resources,
};

#if IS_ENABLED(CONFIG_RTC_DRV_AT91SAM9)
static void __init at91_add_device_rtt_rtc(void)
{
	at91sam3x8h_rtt_device.name = "rtc-at91sam9";
	/*
	 * The second resource is needed:
	 * GPBR will serve as the storage for RTC time offset
	 */
	at91sam3x8h_rtt_device.num_resources = 2;
	rtt_resources[1].start = AT91SAM3X_BASE_GPBR +
				 4 * CONFIG_RTC_DRV_AT91SAM9_GPBR;
	rtt_resources[1].end = rtt_resources[1].start + 3;
}
#else
static void __init at91_add_device_rtt_rtc(void)
{
	/* Only one resource is needed: RTT not used as RTC */
	at91sam3x8h_rtt_device.num_resources = 1;
}
#endif

static void __init at91_add_device_rtt(void)
{
	at91_add_device_rtt_rtc();
	platform_device_register(&at91sam3x8h_rtt_device);
}


/* --------------------------------------------------------------------
 *  TRNG
 * -------------------------------------------------------------------- */

#if defined(CONFIG_HW_RANDOM_ATMEL) || defined(CONFIG_HW_RANDOM_ATMEL_MODULE)
static struct resource trng_resources[] = {
	{
		.start	= AT91SAM3X_BASE_TRNG,
		.end	= AT91SAM3X_BASE_TRNG + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device at91sam3x8h_trng_device = {
	.name		= "atmel-trng",
	.id		= -1,
	.resource	= trng_resources,
	.num_resources	= ARRAY_SIZE(trng_resources),
};

static void __init at91_add_device_trng(void)
{
	platform_device_register(&at91sam3x8h_trng_device);
}
#else
static void __init at91_add_device_trng(void) {}
#endif

/* --------------------------------------------------------------------
 *  Watchdog
 * -------------------------------------------------------------------- */

#if defined(CONFIG_AT91SAM9X_WATCHDOG) || defined(CONFIG_AT91SAM9X_WATCHDOG_MODULE)
static struct resource wdt_resources[] = {
	{
		.start	= AT91SAM3X8H_BASE_WDT,
		.end	= AT91SAM3X8H_BASE_WDT + SZ_16 - 1,
		.flags	= IORESOURCE_MEM,
	}
};

static struct platform_device at91sam3x8h_wdt_device = {
	.name		= "at91_wdt",
	.id		= -1,
	.resource	= wdt_resources,
	.num_resources	= ARRAY_SIZE(wdt_resources),
};

static void __init at91_add_device_watchdog(void)
{
	platform_device_register(&at91sam3x8h_wdt_device);
}
#else
static void __init at91_add_device_watchdog(void) {}
#endif


/* --------------------------------------------------------------------
 *  PWM
 * --------------------------------------------------------------------*/

#if defined(CONFIG_ATMEL_PWM) || defined(CONFIG_ATMEL_PWM_MODULE)
static u32 pwm_mask;

static struct resource pwm_resources[] = {
	[0] = {
		.start	= AT91SAM3X8H_BASE_PWMC,
		.end	= AT91SAM3X8H_BASE_PWMC + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X8H_ID_PWMC,
		.end	= AT91SAM3X8H_ID_PWMC,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at91sam3x8h_pwm0_device = {
	.name	= "atmel_pwm",
	.id	= -1,
	.dev	= {
		.platform_data		= &pwm_mask,
	},
	.resource	= pwm_resources,
	.num_resources	= ARRAY_SIZE(pwm_resources),
};

void __init at91_add_device_pwm(u32 mask)
{
	if (mask & (1 << AT91_PWM0))
		at91_set_B_periph(AT91_PIN_PD24, 1);	/* enable PWM0 */

	if (mask & (1 << AT91_PWM1))
		at91_set_B_periph(AT91_PIN_PD31, 1);	/* enable PWM1 */

	if (mask & (1 << AT91_PWM2))
		at91_set_B_periph(AT91_PIN_PD26, 1);	/* enable PWM2 */

	if (mask & (1 << AT91_PWM3))
		at91_set_B_periph(AT91_PIN_PD0, 1);	/* enable PWM3 */

	pwm_mask = mask;

	platform_device_register(&at91sam3x8h_pwm0_device);
}
#else
void __init at91_add_device_pwm(u32 mask) {}
#endif


/* --------------------------------------------------------------------
 *  SSC -- Synchronous Serial Controller
 * -------------------------------------------------------------------- */

#if defined(CONFIG_ATMEL_SSC) || defined(CONFIG_ATMEL_SSC_MODULE)
static u64 ssc0_dmamask = DMA_BIT_MASK(32);

static struct resource ssc0_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_SSC,
		.end	= AT91SAM3X_BASE_SSC + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_SSC_ID,
		.end	= AT91SAM3X_SSC_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device at91sam3x8h_ssc0_device = {
	.name	= "ssc",
	.id	= 0,
	.dev	= {
		.dma_mask		= &ssc0_dmamask,
		.coherent_dma_mask	= DMA_BIT_MASK(32),
	},
	.resource	= ssc0_resources,
	.num_resources	= ARRAY_SIZE(ssc0_resources),
};

static inline void configure_ssc0_pins(unsigned pins)
{
	if (pins & ATMEL_SSC_TF)
		at91_set_B_periph(AT91_PIN_PA15, 1);
	if (pins & ATMEL_SSC_TK)
		at91_set_B_periph(AT91_PIN_PA14, 1);
	if (pins & ATMEL_SSC_TD)
		at91_set_B_periph(AT91_PIN_PA16, 1);
	if (pins & ATMEL_SSC_RD)
		at91_set_A_periph(AT91_PIN_PB18, 1);
	if (pins & ATMEL_SSC_RK)
		at91_set_A_periph(AT91_PIN_PB19, 1);
	if (pins & ATMEL_SSC_RF)
		at91_set_A_periph(AT91_PIN_PB17, 1);
}

static u64 ssc1_dmamask = DMA_BIT_MASK(32);


/*
 * SSC controllers are accessed through library code, instead of any
 * kind of all-singing/all-dancing driver.  For example one could be
 * used by a particular I2S audio codec's driver, while another one
 * on the same system might be used by a custom data capture driver.
 */
void __init at91_add_device_ssc(unsigned id, unsigned pins)
{
	struct platform_device *pdev;

	/*
	 * NOTE: caller is responsible for passing information matching
	 * "pins" to whatever will be using each particular controller.
	 */
	switch (id) {
	case AT91SAM3X_SSC_ID:
		pdev = &at91sam3x8h_ssc0_device;
		configure_ssc0_pins(pins);
		break;
	default:
		return;
	}

	platform_device_register(pdev);
}

#else
void __init at91_add_device_ssc(unsigned id, unsigned pins) {}
#endif


/* --------------------------------------------------------------------
 *  UART
 * -------------------------------------------------------------------- */

#if defined(CONFIG_SERIAL_ATMEL)
static struct resource dbgu_resources[] = {
	[0] = {
		.start	= AT91_DBGU,
		.end	= AT91_DBGU + SZ_512 - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_UART0_ID,
		.end	= AT91SAM3X_UART0_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct atmel_uart_data dbgu_data = {
	.use_dma_tx	= 0,
	.use_dma_rx	= 0,
	.regs		= (void __iomem *)(AT91_DBGU),
};

static u64 dbgu_dmamask = DMA_BIT_MASK(32);

static struct platform_device at91sam3x8h_dbgu_device = {
	.name		= "atmel_usart",
	.id		= 0,
	.dev		= {
				.dma_mask		= &dbgu_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
				.platform_data		= &dbgu_data,
	},
	.resource	= dbgu_resources,
	.num_resources	= ARRAY_SIZE(dbgu_resources),
};

static inline void configure_dbgu_pins(void)
{
	//at91_set_A_periph(AT91_PIN_PB12, 0);		/* DRXD */
	//at91_set_A_periph(AT91_PIN_PB13, 1);		/* DTXD */
}

static struct resource uart0_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_USART0,
		.end	= AT91SAM3X_BASE_USART0 + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_US0_ID,
		.end	= AT91SAM3X_US0_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct atmel_uart_data uart0_data = {
	.use_dma_tx	= 1,
	.use_dma_rx	= 1,
};

static u64 uart0_dmamask = DMA_BIT_MASK(32);

static struct platform_device at91sam3x8h_uart0_device = {
	.name		= "atmel_usart",
	.id		= 1,
	.dev		= {
				.dma_mask		= &uart0_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
				.platform_data		= &uart0_data,
	},
	.resource	= uart0_resources,
	.num_resources	= ARRAY_SIZE(uart0_resources),
};

static inline void configure_usart0_pins(unsigned pins)
{
	at91_set_A_periph(AT91_PIN_PB19, 1);		/* TXD0 */
	at91_set_A_periph(AT91_PIN_PB18, 0);		/* RXD0 */

	if (pins & ATMEL_UART_RTS)
		at91_set_B_periph(AT91_PIN_PB17, 0);	/* RTS0 */
	if (pins & ATMEL_UART_CTS)
		at91_set_B_periph(AT91_PIN_PB15, 0);	/* CTS0 */
}

static struct resource uart1_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_USART1,
		.end	= AT91SAM3X_BASE_USART1 + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_US1_ID,
		.end	= AT91SAM3X_US1_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct atmel_uart_data uart1_data = {
	.use_dma_tx	= 1,
	.use_dma_rx	= 1,
};

static u64 uart1_dmamask = DMA_BIT_MASK(32);

static struct platform_device at91sam3x8h_uart1_device = {
	.name		= "atmel_usart",
	.id		= 2,
	.dev		= {
				.dma_mask		= &uart1_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
				.platform_data		= &uart1_data,
	},
	.resource	= uart1_resources,
	.num_resources	= ARRAY_SIZE(uart1_resources),
};

static inline void configure_usart1_pins(unsigned pins)
{
	at91_set_A_periph(AT91_PIN_PB4, 1);		/* TXD1 */
	at91_set_A_periph(AT91_PIN_PB5, 0);		/* RXD1 */

	if (pins & ATMEL_UART_RTS)
		at91_set_A_periph(AT91_PIN_PD16, 0);	/* RTS1 */
	if (pins & ATMEL_UART_CTS)
		at91_set_A_periph(AT91_PIN_PD17, 0);	/* CTS1 */
}

static struct resource uart2_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_USART2,
		.end	= AT91SAM3X_BASE_USART2 + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_US2_ID,
		.end	= AT91SAM3X_US2_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct atmel_uart_data uart2_data = {
	.use_dma_tx	= 1,
	.use_dma_rx	= 1,
};

static u64 uart2_dmamask = DMA_BIT_MASK(32);

static struct platform_device at91sam3x8h_uart2_device = {
	.name		= "atmel_usart",
	.id		= 3,
	.dev		= {
				.dma_mask		= &uart2_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
				.platform_data		= &uart2_data,
	},
	.resource	= uart2_resources,
	.num_resources	= ARRAY_SIZE(uart2_resources),
};

static inline void configure_usart2_pins(unsigned pins)
{
	at91_set_A_periph(AT91_PIN_PB6, 1);		/* TXD2 */
	at91_set_A_periph(AT91_PIN_PB7, 0);		/* RXD2 */

	if (pins & ATMEL_UART_RTS)
		at91_set_B_periph(AT91_PIN_PC9, 0);	/* RTS2 */
	if (pins & ATMEL_UART_CTS)
		at91_set_B_periph(AT91_PIN_PC11, 0);	/* CTS2 */
}

static struct resource uart3_resources[] = {
	[0] = {
		.start	= AT91SAM3X_BASE_USART3,
		.end	= AT91SAM3X_BASE_USART3 + SZ_16K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= AT91SAM3X_US3_ID,
		.end	= AT91SAM3X_US3_ID,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct atmel_uart_data uart3_data = {
	.use_dma_tx	= 1,
	.use_dma_rx	= 1,
};

static u64 uart3_dmamask = DMA_BIT_MASK(32);

static struct platform_device at91sam3x8h_uart3_device = {
	.name		= "atmel_usart",
	.id		= 4,
	.dev		= {
				.dma_mask		= &uart3_dmamask,
				.coherent_dma_mask	= DMA_BIT_MASK(32),
				.platform_data		= &uart3_data,
	},
	.resource	= uart3_resources,
	.num_resources	= ARRAY_SIZE(uart3_resources),
};

static inline void configure_usart3_pins(unsigned pins)
{
	at91_set_A_periph(AT91_PIN_PB8, 1);		/* TXD3 */
	at91_set_A_periph(AT91_PIN_PB9, 0);		/* RXD3 */

	if (pins & ATMEL_UART_RTS)
		at91_set_B_periph(AT91_PIN_PA23, 0);	/* RTS3 */
	if (pins & ATMEL_UART_CTS)
		at91_set_B_periph(AT91_PIN_PA24, 0);	/* CTS3 */
}

static struct platform_device *__initdata at91_uarts[ATMEL_MAX_UART];	/* the UARTs to use */

void __init at91_register_uart(unsigned id, unsigned portnr, unsigned pins)
{
	struct platform_device *pdev;
	struct atmel_uart_data *pdata;

	switch (id) {
		case 0:		/* DBGU */
			pdev = &at91sam3x8h_dbgu_device;
			configure_dbgu_pins();
			break;
		case AT91SAM3X_US0_ID:
			pdev = &at91sam3x8h_uart0_device;
			configure_usart0_pins(pins);
			break;
		case AT91SAM3X_US1_ID:
			pdev = &at91sam3x8h_uart1_device;
			configure_usart1_pins(pins);
			break;
		case AT91SAM3X_US2_ID:
			pdev = &at91sam3x8h_uart2_device;
			configure_usart2_pins(pins);
			break;
		case AT91SAM3X_US3_ID:
			pdev = &at91sam3x8h_uart3_device;
			configure_usart3_pins(pins);
			break;
		default:
			return;
	}
	pdata = pdev->dev.platform_data;
	pdata->num = portnr;		/* update to mapped ID */

	if (portnr < ATMEL_MAX_UART)
		at91_uarts[portnr] = pdev;
}

/*
void __init at91_set_serial_console(unsigned portnr)
{
	if (portnr < ATMEL_MAX_UART) {
		atmel_default_console_device = at91_uarts[portnr];
		at91sam3x8h_set_console_clock(at91_uarts[portnr]->id);
	}
}
*/
void __init at91_add_device_serial(void)
{
	int i;

	for (i = 0; i < ATMEL_MAX_UART; i++) {
		if (at91_uarts[i])
			platform_device_register(at91_uarts[i]);
	}

	if (!atmel_default_console_device)
		printk(KERN_INFO "AT91: No default serial console defined.\n");
}
#else
void __init at91_register_uart(unsigned id, unsigned portnr, unsigned pins) {}
//void __init at91_set_serial_console(unsigned portnr) {}
void __init at91_add_device_serial(void) {}
#endif


/* -------------------------------------------------------------------- */
/*
 * These devices are always present and don't need any board-specific
 * setup.
 */
static int __init at91_add_standard_devices(void)
{
	at91_add_device_hdmac();
	//at91_add_device_rtc();
	at91_add_device_rtt();
	//at91_add_device_trng();
	//at91_add_device_watchdog();
	at91_add_device_tc();
	return 0;
}

arch_initcall(at91_add_standard_devices);

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

#include <linux/module.h>
#include <linux/dma-mapping.h>

#include <asm/irq.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/system_misc.h>
#include <mach/at91sam3x8h.h>
#include <mach/at91_pmc.h>
#include <mach/cpu.h>
#include <asm/hardware/cortexm3.h>

#include "soc.h"
#include "generic.h"
#include "clock.h"
#include "sam3_smc.h"

/* --------------------------------------------------------------------
 *  Clocks
 * -------------------------------------------------------------------- */

/*
 * The peripheral clocks.
 */
static struct clk uart0_clk = {
	.name		= "uart0_clk",
	.pmc_mask	= 1 << AT91SAM3X_UART0_ID,
	.type		= CLK_TYPE_PERIPHERAL,
	.users		= 1,
};
static struct clk smc_sdramc_clk = {
	.name		= "smc_sdramc_clk",
	.pmc_mask	= 1 << AT91SAM3X_SMC_SDRAMC_ID,
	.type		= CLK_TYPE_PERIPHERAL,
	.users		= 1,
};
static struct clk sdramc_clk = {
	.name		= "sdramc_clk",
	.pmc_mask	= 1 << AT91SAM3X_SDRAMC_ID,
	.type		= CLK_TYPE_PERIPHERAL,
	.users		= 1,
};
static struct clk pioA_clk = {
	.name		= "pioA_clk",
	.pmc_mask	= 1 << AT91SAM3X_PIOA_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk pioB_clk = {
	.name		= "pioB_clk",
	.pmc_mask	= 1 << AT91SAM3X_PIOB_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk pioC_clk = {
	.name		= "pioC_clk",
	.pmc_mask	= 1 << AT91SAM3X_PIOC_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk pioD_clk = {
	.name		= "pioD_clk",
	.pmc_mask	= 1 << AT91SAM3X_PIOD_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk pioE_clk = {
	.name		= "pioE_clk",
	.pmc_mask	= 1 << AT91SAM3X_PIOE_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk pioF_clk = {
	.name		= "pioF_clk",
	.pmc_mask	= 1 << AT91SAM3X_PIOF_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk usart0_clk = {
	.name		= "usart0_clk",
	.pmc_mask	= 1 << AT91SAM3X_US0_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk usart1_clk = {
	.name		= "usart1_clk",
	.pmc_mask	= 1 << AT91SAM3X_US1_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk usart2_clk = {
	.name		= "usart2_clk",
	.pmc_mask	= 1 << AT91SAM3X_US2_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk usart3_clk = {
	.name		= "usart3_clk",
	.pmc_mask	= 1 << AT91SAM3X_US3_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk mmc0_clk = {
	.name		= "mci0_clk",
	.pmc_mask	= 1 << AT91SAM3X_HSMCI_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk twi0_clk = {
	.name		= "twi0_clk",
	.pmc_mask	= 1 << AT91SAM3X_TWI0_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk twi1_clk = {
	.name		= "twi1_clk",
	.pmc_mask	= 1 << AT91SAM3X_TWI1_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk spi0_clk = {
	.name		= "spi0_clk",
	.pmc_mask	= 1 << AT91SAM3X_SPI0_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk spi1_clk = {
	.name		= "spi1_clk",
	.pmc_mask	= 1 << AT91SAM3X_SPI1_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk ssc_clk = {
	.name		= "ssc_clk",
	.pmc_mask	= 1 << AT91SAM3X_SSC_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk tc0_clk = {
	.name		= "tc0_clk",
	.users		= 1,		/* always on */
	.pmc_mask	= 1 << AT91SAM3X_TC0_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk tc1_clk = {
	.name		= "tc1_clk",
	.pmc_mask	= 1 << AT91SAM3X_TC1_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk tc2_clk = {
	.name		= "tc2_clk",
	.pmc_mask	= 1 << AT91SAM3X_TC2_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk tc3_clk = {
	.name		= "tc3_clk",
	.pmc_mask	= 1 << AT91SAM3X_TC3_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk tc4_clk = {
	.name		= "tc4_clk",
	.pmc_mask	= 1 << AT91SAM3X_TC4_ID,
	.type		= CLK_TYPE_PERIPHERAL,
};
static struct clk tc5_clk = {
	.name		= "tc5_clk",
	.pmc_mask	= 1 << (AT91SAM3X_TC5_ID - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};
static struct clk tc6_clk = {
	.name		= "tc6_clk",
	.pmc_mask	= 1 << (AT91SAM3X_TC6_ID - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};
static struct clk tc7_clk = {
	.name		= "tc7_clk",
	.pmc_mask	= 1 << (AT91SAM3X_TC7_ID  - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};
static struct clk tc8_clk = {
	.name		= "tc8_clk",
	.pmc_mask	= 1 << (AT91SAM3X_TC8_ID - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};
static struct clk pwm_clk = {
	.name		= "pwm_clk",
	.pmc_mask	= 1 << (AT91SAM3X_PWM_ID - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};
static struct clk adc_clk = {
	.name		= "adc_clk",
	.pmc_mask	= 1 << (AT91SAM3X_ADC_ID - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};
static struct clk dac_clk = {
	.name		= "dac_clk",
	.pmc_mask	= 1 << (AT91SAM3X_DACC_ID - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};
static struct clk dma_clk = {
	.name		= "dma_clk",
	.pmc_mask	= 1 << (AT91SAM3X_DMAC_ID - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};
static struct clk uotghs_clk = {
	.name		= "uotghs_clk",
	.pmc_mask	= 1 << (AT91SAM3X_UOTGHS_ID - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};
static struct clk trng_clk = {
	.name		= "trng_clk",
	.pmc_mask	= 1 << (AT91SAM3X_TRNG_ID - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};
static struct clk macb_clk = {
	.name		= "pclk",
	.pmc_mask	= 1 << (AT91SAM3X_EMAC_ID - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};
static struct clk can0_clk = {
	.name		= "can0_clk",
	.pmc_mask	= 1 << (AT91SAM3X_CAN0_ID - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};
static struct clk can1_clk = {
	.name		= "can1_clk",
	.pmc_mask	= 1 << (AT91SAM3X_CAN1_ID - 32),
	.type		= CLK_TYPE_PERIPHERAL2,
};


static struct clk *periph_clocks[] __initdata = {
	&uart0_clk,
	&smc_sdramc_clk,
	&sdramc_clk,
	&pioA_clk,
	&pioB_clk,
	&pioC_clk,
	&pioD_clk,
	&pioE_clk,
	&pioF_clk,
	&usart0_clk,
	&usart1_clk,
	&usart2_clk,
	&usart3_clk,
	&mmc0_clk,
	&twi0_clk,
	&twi1_clk,
	&spi0_clk,
	&spi1_clk,
	&ssc_clk,
	&tc0_clk,
	&tc1_clk,
	&tc2_clk,
	&tc3_clk,
	&tc4_clk,
	&tc5_clk,
	&tc6_clk,
	&tc7_clk,
	&tc8_clk,
	&pwm_clk,
	&adc_clk,
	&dac_clk,
	&dma_clk,
	&uotghs_clk,
	&trng_clk,
	&macb_clk,
	&can0_clk,
	&can1_clk,
};

static struct clk_lookup periph_clocks_lookups[] = {
	/* One additional fake clock for macb_hclk */
	CLKDEV_CON_ID("hclk", &macb_clk),
	/* One additional fake clock for ohci */
	CLKDEV_CON_ID("uotghs_clk", &uotghs_clk),
	CLKDEV_CON_DEV_ID("mci_clk", "atmel_mci.0", &mmc0_clk),
	CLKDEV_CON_DEV_ID("spi_clk", "atmel_spi.0", &spi0_clk),
	CLKDEV_CON_DEV_ID("spi_clk", "atmel_spi.1", &spi1_clk),
	CLKDEV_CON_DEV_ID("pclk", "ssc.0", &ssc_clk),
	CLKDEV_CON_DEV_ID(NULL, "atmel-trng", &trng_clk),
	CLKDEV_CON_DEV_ID("t0_clk", "atmel_tcb.0", &tc3_clk),
	CLKDEV_CON_DEV_ID("t1_clk", "atmel_tcb.0", &tc4_clk),
	CLKDEV_CON_DEV_ID("t2_clk", "atmel_tcb.0", &tc5_clk),
	CLKDEV_CON_DEV_ID("t0_clk", "atmel_tcb.1", &tc6_clk),
	CLKDEV_CON_DEV_ID("t1_clk", "atmel_tcb.1", &tc7_clk),
	CLKDEV_CON_DEV_ID("t2_clk", "atmel_tcb.1", &tc8_clk),
	/* more usart lookup table for DT entries */
	CLKDEV_CON_DEV_ID("usart", "400e0800.serial", &mck),
	CLKDEV_CON_DEV_ID("usart", "40098000.serial", &usart0_clk),
	CLKDEV_CON_DEV_ID("usart", "4009C000.serial", &usart1_clk),
	CLKDEV_CON_DEV_ID("usart", "400A0000.serial", &usart2_clk),
	CLKDEV_CON_DEV_ID("usart", "400A4000.serial", &usart3_clk),
	//CLKDEV_CON_DEV_ID("t0_clk", "40084000.timer", &tc1_clk),
	//CLKDEV_CON_DEV_ID("t0_clk", "40088000.timer", &tc2_clk),
	CLKDEV_CON_ID("pioA", &pioA_clk),
	CLKDEV_CON_ID("pioB", &pioB_clk),
	CLKDEV_CON_ID("pioC", &pioC_clk),
	CLKDEV_CON_ID("pioD", &pioD_clk),
	CLKDEV_CON_ID("pioE", &pioE_clk),
	CLKDEV_CON_ID("pioF", &pioF_clk),
};

static struct clk_lookup usart_clocks_lookups[] = {
	CLKDEV_CON_DEV_ID("usart", "atmel_usart.0", &mck),
	CLKDEV_CON_DEV_ID("usart", "atmel_usart.1", &usart0_clk),
	CLKDEV_CON_DEV_ID("usart", "atmel_usart.2", &usart1_clk),
	CLKDEV_CON_DEV_ID("usart", "atmel_usart.3", &usart2_clk),
	CLKDEV_CON_DEV_ID("usart", "atmel_usart.4", &usart3_clk),
};

/*
 * The two programmable clocks.
 * You must configure pin multiplexing to bring these signals out.
 */
static struct clk pck0 = {
	.name		= "pck0",
	.pmc_mask	= AT91_PMC_PCK0,
	.type		= CLK_TYPE_PROGRAMMABLE,
	.id		= 0,
};
static struct clk pck1 = {
	.name		= "pck1",
	.pmc_mask	= AT91_PMC_PCK1,
	.type		= CLK_TYPE_PROGRAMMABLE,
	.id		= 1,
	.users		= 1,
};

static void __init at91sam3x8h_register_clocks(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(periph_clocks); i++)
		clk_register(periph_clocks[i]);

	clkdev_add_table(periph_clocks_lookups,
			 ARRAY_SIZE(periph_clocks_lookups));
	clkdev_add_table(usart_clocks_lookups,
			 ARRAY_SIZE(usart_clocks_lookups));

	clk_register(&pck0);
	clk_register(&pck1);
}

static struct clk_lookup console_clock_lookup;

void __init at91sam3x8h_set_console_clock(int id)
{
	if (id >= ARRAY_SIZE(usart_clocks_lookups))
		return;

	console_clock_lookup.con_id = "usart";
	console_clock_lookup.clk = usart_clocks_lookups[id].clk;
	clkdev_add(&console_clock_lookup);
}

/* --------------------------------------------------------------------
 *  GPIO
 * -------------------------------------------------------------------- */

static struct at91_gpio_bank at91sam3x8h_gpio[] __initdata = {
	{
		.id		= AT91SAM3X_PIOA_ID,
		.regbase	= AT91SAM3X_BASE_PIOA,
	}, {
		.id		= AT91SAM3X_PIOB_ID,
		.regbase	= AT91SAM3X_BASE_PIOB,
	}, {
		.id		= AT91SAM3X_PIOC_ID,
		.regbase	= AT91SAM3X_BASE_PIOC,
	}, {
		.id		= AT91SAM3X_PIOD_ID,
		.regbase	= AT91SAM3X_BASE_PIOD,
	}, {
		.id		= AT91SAM3X_PIOE_ID,
		.regbase	= AT91SAM3X_BASE_PIOE,
	}, {
		.id		= AT91SAM3X_PIOF_ID,
		.regbase	= AT91SAM3X_BASE_PIOF,
	}
};

/* --------------------------------------------------------------------
 *  AT91SAM9G45 processor initialization
 * -------------------------------------------------------------------- */

static void __init at91sam3x8h_map_io(void)
{
	at91_init_sram(0, AT91SAM3X_SRAM_BASE, AT91SAM3X_SRAM_SIZE);
}

static void __init at91sam3x8h_ioremap_registers(void)
{
	at91sam3_ioremap_smc(0, AT91SAM3X_BASE_SMC);
	at91_ioremap_shdwc(AT91SAM3X_BASE_SUPC);
	at91_ioremap_rstc(AT91SAM3X_BASE_RSTC);
}

static void __init at91sam3x8h_initialize(void)
{
	//arm_pm_idle = cortex_m3_idle;
	//arm_pm_restart = cortex_m3_reboot;

	/* Register GPIO subsystem */
	at91_gpio_init(at91sam3x8h_gpio, 6);
}

/* --------------------------------------------------------------------
 *  SoC initialization
 * -------------------------------------------------------------------- */

struct at91_init_soc __initdata at91sam3x8h_soc = {
	.map_io = at91sam3x8h_map_io,
	//.default_irq_priority = at91sam3x_default_irq_priority,
	.ioremap_registers = at91sam3x8h_ioremap_registers,
	.register_clocks = at91sam3x8h_register_clocks,
	.init = at91sam3x8h_initialize,
};

/*
 * (C) Copyright 2012
 * Olivier Schonken <olivier.schonken@gmail.com>
 *
 * Adapted from the Emcraft teams timer implementation
 * see arch/arm/common/cortexm3.c
 *
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

#include <linux/clockchips.h>
#include <linux/clk.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/mach/time.h>

#include <asm/hardware/cortexm3.h>
#include <asm/hardware/sam3x_clockevents.h>
#include <mach/at91sam3x8h.h>
#include <mach/board.h>
#include "generic.h"

/*
 * Source clock init
 */
static void clocksource_tmr_init(void)
{
	unsigned long	systick_rate;
	unsigned	bits;

	systick_rate = clk_get_rate(clk_get(NULL, "mck"));
	/*
	 * Use the Cortex-M3 SysTick timer
	 */
	cortex_m3_register_systick_clocksource(systick_rate);
}

/*
 * Clockevents init (sys timer)
 */
static void clockevents_tmr_init(void)
{
	unsigned long	systick_rate;
	systick_rate = clk_get_rate(clk_get(NULL, "mck"));
	/*
	 * Init clockevents (sys timer)
	 */
	sam3x_clockevents_tmr_init(AT91_TC, systick_rate, AT91SAM3X_INT_TC0);
}
static void at91sam3x_timer_reset(void)
{

}

static void at91sam3x_timer_suspend(void)
{
	/* Disable timer */

}
/*
 * Initialize the SAM3X timer systems
 */
void __init sam3x_timer_init(void)
{
	/*
	 * Init clocksource
	 */
	clocksource_tmr_init();

	/*
	 * Init clockevents (sys timer)
	 */
	clockevents_tmr_init();
}

struct sys_timer at91sam3x_timer = {
	.init		= sam3x_timer_init,
	.suspend	= at91sam3x_timer_suspend,
	.resume		= at91sam3x_timer_reset,
};

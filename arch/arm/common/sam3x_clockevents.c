/*
 * (C) Copyright 2012
 * Olivier Schonken <olivier.schonken@gmail.com>
 *
 * Adapted from the Emcraft teams clockevents implementation and the
 * pit clockevents timer implementation of the SAM9
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
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <mach/at91_tc.h>
#include <mach/at91_pmc.h>
/*Reserved*/


static void sam3x_timer_reset()
{
	/* Reset timer */
	at91_tc_write(AT91_TC_CCR, AT91_TC_SWTRG);
}

/*
 * Enable or disable a timer
 */
static void sam3x_timer_enable(int enable)
{
	if (enable)
	{
		at91_tc_write(AT91_TC_CCR, AT91_TC_CLKEN | AT91_TC_SWTRG);
	}
	else
	{
		at91_tc_write(AT91_TC_CCR, AT91_TC_CLKDIS);
	}
}

/*
 * Clock event device set mode function
 */
static void clockevent_tmr_set_mode(
	enum clock_event_mode mode, struct clock_event_device *clk)
{

		switch (mode) {
		case CLOCK_EVT_MODE_PERIODIC:
			sam3x_timer_enable(1);
			break;
		case CLOCK_EVT_MODE_ONESHOT:
		case CLOCK_EVT_MODE_UNUSED:
		case CLOCK_EVT_MODE_SHUTDOWN:
		default:
			sam3x_timer_enable(0);
			break;
		}
}

static struct clock_event_device	tick_tmr_clockevent = {
	.name		= "SAM3X System Timer",
	.rating		= 200,
	.irq		= AT91SAM3X_INT_TC0,
	.features	= CLOCK_EVT_FEAT_PERIODIC,
	.set_mode	= clockevent_tmr_set_mode,
	.cpumask	= cpu_all_mask,
};

/*
 * Timer IRQ handler
 */
static irqreturn_t clockevent_tmr_irq_handler(int irq, void *dev_id)
{

	unsigned int dummy = 0;
	static int led_on_off = 1;

	/*
	 * `dev_id` was defined during initialization of irqaction
	 */
	struct clock_event_device *evt = &tick_tmr_clockevent;
	/*
	 * Clear the interrupt
	 */
	dummy = at91_tc_read(AT91_TC_SR);
	/*
	 * Handle event
	 */
	if ((dummy & AT91_TC_CPCS) == AT91_TC_CPCS)
	{
		evt->event_handler(evt);
		return IRQ_HANDLED;
	}
	else
	{
		return IRQ_NONE;
	}
}
/*
 * System timer IRQ action
 */
static struct irqaction	tick_tmr_irqaction = {
	.name		= "Sam3x Kernel Time Tick",
	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
	.handler	= clockevent_tmr_irq_handler,
	.irq		= AT91SAM3X_INT_TC0,
};
/*
 * Clockevents init (sys timer)
 */
void sam3x_clockevents_tmr_init(u32 timer_regs_base, u32 src_clk, int irq)
{
	struct clock_event_device *tmr = &tick_tmr_clockevent;

	sam3x_timer_enable(0);
	at91_tc_write(AT91_TC_IDR, 0xFFFFFFFF);
	/* Enable necessary match channels and operations for them */
	at91_tc_write(AT91_TC_CMR, AT91_TC_CPCTRG | AT91_TC_TIMER_CLOCK1);
	/* Set match register */
	at91_tc_write(AT91_TC_RC, (src_clk/2) / HZ);

	at91_tc_read(AT91_TC_SR);
	setup_irq(irq, &tick_tmr_irqaction);
	at91_tc_write(AT91_TC_IER, AT91_TC_CPCS);
	/* Enable timer */
	at91_pmc_write(AT91_PMC_PCER, (1 << irq));
	/*
	 * For system timer we don't provide set_next_event method,
	 * so, I guess, setting mult, shift, max_delta_ns, min_delta_ns
	 * makes no sense (I verified that kernel works well without these).
	 * Nevertheless, some clocksource drivers with periodic-mode only do
	 * this. So, let's set them to some values too.
	 */
	clockevents_calc_mult_shift(tmr, (src_clk/2) / HZ, 400);
	tmr->max_delta_ns = clockevent_delta2ns(0xFFFFFFF0, tmr);
	tmr->min_delta_ns = clockevent_delta2ns(0xF, tmr);

	clockevents_register_device(tmr);
}

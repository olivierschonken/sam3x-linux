/*
 *  linux/arch/arm/common/nvic.c
 *
 *  Copyright (C) 2008 ARM Limited, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Support for the Nested Vectored Interrupt Controller found on the
 * ARMv7-M CPUs (Cortex-M3)
 */
#include <linux/export.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/smp.h>
#include <linux/module.h>
#include <linux/irqdomain.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <asm/mach/irq.h>
#include <asm/hardware/nvic.h>

static struct irq_domain *at91_nvic_domain;
static struct device_node *at91_nvic_np;

static DEFINE_RAW_SPINLOCK(irq_controller_lock);

/*
 * Routines to acknowledge, disable and enable interrupts
 *
 * Linux assumes that when we're done with an interrupt we need to
 * unmask it, in the same way we need to unmask an interrupt when
 * we first enable it.
 *
 * The NVIC has a separate notion of "end of interrupt" to re-enable
 * an interrupt after handling, in order to support hardware
 * prioritisation.
 *
 * We can make the NVIC behave in the way that Linux expects by making
 * our "acknowledge" routine disable the interrupt, then mark it as
 * complete.
 */
static void nvic_ack_irq(struct irq_data *d)
{
	u32 mask = 1 << (d->irq % 32);

	raw_spin_lock(&irq_controller_lock);
	writel(mask, NVIC_CLEAR_ENABLE + d->irq / 32 * 4);
	raw_spin_unlock(&irq_controller_lock);
}

static void nvic_mask_irq(struct irq_data *d)
{
	u32 mask = 1 << (d->irq % 32);

	raw_spin_lock(&irq_controller_lock);
	writel(mask, NVIC_CLEAR_ENABLE + d->irq / 32 * 4);
	raw_spin_unlock(&irq_controller_lock);
}

static void nvic_unmask_irq(struct irq_data *d)
{
	u32 mask = 1 << (d->irq % 32);

	raw_spin_lock(&irq_controller_lock);
	writel(mask, NVIC_SET_ENABLE + d->irq / 32 * 4);
	raw_spin_unlock(&irq_controller_lock);
}

static struct irq_chip nvic_chip = {
	.name = "NVIC",
	.irq_ack = nvic_ack_irq,
	.irq_mask = nvic_mask_irq,
	.irq_unmask = nvic_unmask_irq,
};

void __init nvic_init(void)
{
	unsigned int max_irq, i;
	int irq_base;

	max_irq = HARDWARE_IRQS;//max_irq = ((readl(NVIC_INTR_CTRL) & 0x1f) + 1) * 32;

	/*
	 * Disable all interrupts
	 */
	for (i = 0; i < max_irq / 32; i++)
		writel(~0, NVIC_CLEAR_ENABLE + i * 4);

	/*
	 * Set priority on all interrupts.
	 */
	for (i = 0; i < max_irq; i += 4)
		writel(0, NVIC_PRIORITY + i);

	/* Add irq domain for NVIC */
	irq_base = irq_alloc_descs(-1, 0, max_irq, 0);
	if (irq_base < 0) {
		WARN(1, "Cannot allocate irq_descs, assuming pre-allocated\n");
		irq_base = 0;
	}
	at91_nvic_domain = irq_domain_add_legacy(at91_nvic_np, max_irq,
						irq_base, 0,
						&irq_domain_simple_ops, NULL);

	if (!at91_nvic_domain)
		panic("Unable to add AIC irq domain\n");

	irq_set_default_host(at91_nvic_domain);

	/*
	 * Setup the Linux IRQ subsystem.
	 */
	for (i = 0; i < max_irq; i++) {
		irq_set_chip_and_handler(i, &nvic_chip, handle_level_irq);
		//irq_set_chip_data(irq, base);
		set_irq_flags(i, IRQF_VALID | IRQF_PROBE);
	}
}

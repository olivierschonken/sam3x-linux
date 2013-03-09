/*
 * Chip-specific header file for the AT91SAM9G45 family
 *
 *  Copyright (C) 2008-2009 Atmel Corporation.
 *
 * Common definitions.
 * Based on AT91SAM9G45 preliminary datasheet.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef AT91SAM3X8H_H
#define AT91SAM3X8H_H

/*
 * Peripheral identifiers/interrupts.
 */

#define AT91SAM3X_SUPC_ID        0   ///< Supply Controller (SUPC)
#define AT91SAM3X_RSTC_ID        1   ///< Reset Controller (RSTC)
#define AT91SAM3X_RTC_ID         2   ///< Real Time Clock (RTC)
#define AT91SAM3X_RTT_ID         3   ///< Real Time Timer (RTT)
#define AT91SAM3X_WDT_ID         4   ///< Watchdog Timer (WDT)
#define AT91SAM3X_PMC_ID         5   ///< Power Management Controller (PMC)
#define AT91SAM3X_EEFC0_ID       6   ///< Enhanced Flash Controller
#define AT91SAM3X_EEFC1_ID       7   ///< Enhanced Flash Controller
#define AT91SAM3X_UART0_ID       8   ///< UART 0 (UART0)
#define AT91SAM3X_SMC_SDRAMC_ID  9   ///< Satic memory controller / SDRAM controller
#define AT91SAM3X_SDRAMC_ID     10   ///< Satic memory controller / SDRAM controller
#define AT91SAM3X_PIOA_ID       11   ///< Parallel I/O Controller A
#define AT91SAM3X_PIOB_ID       12   ///< Parallel I/O Controller B
#define AT91SAM3X_PIOC_ID       13   ///< Parallel I/O Controller C
#define AT91SAM3X_PIOD_ID       14   ///< Parallel I/O Controller D
#define AT91SAM3X_PIOE_ID       15   ///< Parallel I/O Controller E
#define AT91SAM3X_PIOF_ID       16   ///< Parallel I/O Controller F
#define AT91SAM3X_US0_ID        17   ///< USART 0
#define AT91SAM3X_US1_ID        18   ///< USART 1
#define AT91SAM3X_US2_ID        19   ///< USART 2
#define AT91SAM3X_US3_ID        20   ///< USART 3
#define AT91SAM3X_HSMCI_ID      21   ///< High speed multimedia card interface
#define AT91SAM3X_TWI0_ID       22   ///< Two Wire Interface 0
#define AT91SAM3X_TWI1_ID       23   ///< Two Wire Interface 1
#define AT91SAM3X_SPI0_ID       24   ///< Serial Peripheral Interface
#define AT91SAM3X_SPI1_ID       25   ///< Serial Peripheral Interface
#define AT91SAM3X_SSC_ID        26   ///< Synchronous serial controller
#define AT91SAM3X_TC0_ID        27   ///< Timer/Counter 0
#define AT91SAM3X_TC1_ID        28   ///< Timer/Counter 1
#define AT91SAM3X_TC2_ID        29   ///< Timer/Counter 2
#define AT91SAM3X_TC3_ID        30   ///< Timer/Counter 3
#define AT91SAM3X_TC4_ID        31   ///< Timer/Counter 4
#define AT91SAM3X_TC5_ID        32   ///< Timer/Counter 5
#define AT91SAM3X_TC6_ID        33   ///< Timer/Counter 6
#define AT91SAM3X_TC7_ID        34   ///< Timer/Counter 7
#define AT91SAM3X_TC8_ID        35   ///< Timer/Counter 8
#define AT91SAM3X_PWM_ID        36   ///< Pulse width modulation controller
#define AT91SAM3X_ADC_ID        37   ///< ADC controller
#define AT91SAM3X_DACC_ID       38   ///< DAC controller
#define AT91SAM3X_DMAC_ID       39   ///< DMA controller
#define AT91SAM3X_UOTGHS_ID     40   ///< USB OTG high speed
#define AT91SAM3X_TRNG_ID       41   ///< True random number generator
#define AT91SAM3X_EMAC_ID       42   ///< Ethernet MAC
#define AT91SAM3X_CAN0_ID       43   ///< CAN controller 0
#define AT91SAM3X_CAN1_ID       44   ///< CAN controller 1


#define FAULT_NMI               2           ///< NMI fault
#define FAULT_HARD              3           ///< Hard fault
#define FAULT_MPU               4           ///< MPU fault
#define FAULT_BUS               5           ///< Bus fault
#define FAULT_USAGE             6           ///< Usage fault
#define FAULT_SVCALL            11          ///< SVCall
#define FAULT_DEBUG             12          ///< Debug monitor
#define FAULT_PENDSV            14          ///< PendSV
#define FAULT_SYSTICK           15          ///< System Tick

#define INT_PERIPH_BASE  0

#define AT91SAM3X_INT_SUPC        (INT_PERIPH_BASE + AT91SAM3X_SUPC_ID)
#define AT91SAM3X_INT_RSTC        (INT_PERIPH_BASE + AT91SAM3X_RSTC_ID)
#define AT91SAM3X_INT_RTC         (INT_PERIPH_BASE + AT91SAM3X_RTC_ID)
#define AT91SAM3X_INT_RTT         (INT_PERIPH_BASE + AT91SAM3X_RTT_ID)
#define AT91SAM3X_INT_WDT         (INT_PERIPH_BASE + AT91SAM3X_WDT_ID)
#define AT91SAM3X_INT_PMC         (INT_PERIPH_BASE + AT91SAM3X_PMC_ID)
#define AT91SAM3X_INT_EEFC0       (INT_PERIPH_BASE + AT91SAM3X_EEFC0_ID)
#define AT91SAM3X_INT_EEFC1       (INT_PERIPH_BASE + AT91SAM3X_EEFC1_ID)
#define AT91SAM3X_INT_UART0       (INT_PERIPH_BASE + AT91SAM3X_UART0_ID)
#define AT91SAM3X_INT_SMC_SDRAMC  (INT_PERIPH_BASE + AT91SAM3X_SMC_SDRAMC_ID)
#define AT91SAM3X_INT_SDRAMC      (INT_PERIPH_BASE + AT91SAM3X_SDRAMC_ID)
#define AT91SAM3X_INT_PIOA        (INT_PERIPH_BASE + AT91SAM3X_PIOA_ID)
#define AT91SAM3X_INT_PIOB        (INT_PERIPH_BASE + AT91SAM3X_PIOB_ID)
#define AT91SAM3X_INT_PIOC        (INT_PERIPH_BASE + AT91SAM3X_PIOC_ID)
#define AT91SAM3X_INT_PIOD        (INT_PERIPH_BASE + AT91SAM3X_PIOD_ID)
#define AT91SAM3X_INT_PIOE        (INT_PERIPH_BASE + AT91SAM3X_PIOE_ID)
#define AT91SAM3X_INT_PIOF        (INT_PERIPH_BASE + AT91SAM3X_PIOF_ID)
#define AT91SAM3X_INT_US0         (INT_PERIPH_BASE + AT91SAM3X_US0_ID)
#define AT91SAM3X_INT_US1         (INT_PERIPH_BASE + AT91SAM3X_US1_ID)
#define AT91SAM3X_INT_US2         (INT_PERIPH_BASE + AT91SAM3X_US2_ID)
#define AT91SAM3X_INT_US3         (INT_PERIPH_BASE + AT91SAM3X_US3_ID)
#define AT91SAM3X_INT_HSMCI       (INT_PERIPH_BASE + AT91SAM3X_HSMCI_ID)
#define AT91SAM3X_INT_TWI0        (INT_PERIPH_BASE + AT91SAM3X_TWI0_ID)
#define AT91SAM3X_INT_TWI1        (INT_PERIPH_BASE + AT91SAM3X_TWI1_ID)
#define AT91SAM3X_INT_SPI0        (INT_PERIPH_BASE + AT91SAM3X_SPI0_ID)
#define AT91SAM3X_INT_SPI1        (INT_PERIPH_BASE + AT91SAM3X_SPI1_ID)
#define AT91SAM3X_INT_SSC         (INT_PERIPH_BASE + AT91SAM3X_SSC_ID)
#define AT91SAM3X_INT_TC0         (INT_PERIPH_BASE + AT91SAM3X_TC0_ID)
#define AT91SAM3X_INT_TC1         (INT_PERIPH_BASE + AT91SAM3X_TC1_ID)
#define AT91SAM3X_INT_TC2         (INT_PERIPH_BASE + AT91SAM3X_TC2_ID)
#define AT91SAM3X_INT_TC3         (INT_PERIPH_BASE + AT91SAM3X_TC3_ID)
#define AT91SAM3X_INT_TC4         (INT_PERIPH_BASE + AT91SAM3X_TC4_ID)
#define AT91SAM3X_INT_TC5         (INT_PERIPH_BASE + AT91SAM3X_TC5_ID)
#define AT91SAM3X_INT_TC6         (INT_PERIPH_BASE + AT91SAM3X_TC6_ID)
#define AT91SAM3X_INT_TC7         (INT_PERIPH_BASE + AT91SAM3X_TC7_ID)
#define AT91SAM3X_INT_TC8         (INT_PERIPH_BASE + AT91SAM3X_TC8_ID)
#define AT91SAM3X_INT_PWM         (INT_PERIPH_BASE + AT91SAM3X_PWM_ID)
#define AT91SAM3X_INT_ADC         (INT_PERIPH_BASE + AT91SAM3X_ADC_ID)
#define AT91SAM3X_INT_DACC        (INT_PERIPH_BASE + AT91SAM3X_DACC_ID)
#define AT91SAM3X_INT_DMAC        (INT_PERIPH_BASE + AT91SAM3X_DMAC_ID)
#define AT91SAM3X_INT_UOTGHS      (INT_PERIPH_BASE + AT91SAM3X_UOTGHS_ID)
#define AT91SAM3X_INT_TRNG        (INT_PERIPH_BASE + AT91SAM3X_TRNG_ID)
#define AT91SAM3X_INT_EMAC        (INT_PERIPH_BASE + AT91SAM3X_EMAC_ID)
#define AT91SAM3X_INT_CAN0        (INT_PERIPH_BASE + AT91SAM3X_CAN0_ID)
#define AT91SAM3X_INT_CAN1        (INT_PERIPH_BASE + AT91SAM3X_CAN1_ID)


/*
 * User Peripheral physical base addresses.
 */
#define AT91SAM3X_BASE_HSMCI      0x40000000 /*  (HSMCI     ) Base Address */
#define AT91SAM3X_BASE_SSC        0x40004000 /*  (SSC       ) Base Address */
#define AT91SAM3X_BASE_SPI0       0x40008000 /*  (SPI       ) Base Address */
#define AT91SAM3X_BASE_PDC_SPI0   0x40008100 /*  (PDC_SPI   ) Base Address */
#define AT91SAM3X_BASE_SPI1       0x4000C000 /*  (SPI       ) Base Address */
#define AT91SAM3X_BASE_PDC_SPI1   0x4000C100 /*  (PDC_SPI   ) Base Address */
#define AT91SAM3X_BASE_TC0        0x40080000 /*  (TC0       ) Base Address */
#define AT91SAM3X_BASE_TC1        0x40084000 /*  (TC1       ) Base Address */
#define AT91SAM3X_BASE_TC2        0x40088000 /*  (TC2       ) Base Address */
#define AT91SAM3X_BASE_TWI0       0x4008C000 /*  (TWI0      ) Base Address */
#define AT91SAM3X_BASE_PDC_TWI0   0x4008C100 /*  (PDC_TWI0  ) Base Address */
#define AT91SAM3X_BASE_TWI1       0x40090000 /*  (TWI1      ) Base Address */
#define AT91SAM3X_BASE_PWM        0x40094000 /*  (PWM       ) Base Address */
#define AT91SAM3X_BASE_USART0     0x40098000 /*  (USART0    ) Base Address */
#define AT91SAM3X_BASE_PDC_USART0 0x40098100 /*  (PDC_USART0) Base Address */
#define AT91SAM3X_BASE_USART1     0x4009C000 /*  (USART1    ) Base Address */
#define AT91SAM3X_BASE_USART2     0x400A0000 /*  (USART2    ) Base Address */
#define AT91SAM3X_BASE_USART3     0x400A4000 /*  (USART3    ) Base Address */
#define AT91SAM3X_BASE_UOTGHS     0x400AC000 /*  (UOTGHS    ) Base Address */
#define AT91SAM3X_BASE_EMAC       0x400B0000 /*  (EMAC      ) Base Address */
#define AT91SAM3X_BASE_CAN0       0x400B4000 /*  (CAN0      ) Base Address */
#define AT91SAM3X_BASE_CAN1       0x400B8000 /*  (CAN1      ) Base Address */
#define AT91SAM3X_BASE_TRNG       0x400BC000 /*  (TRNG      ) Base Address */
#define AT91SAM3X_BASE_ADC        0x400C0000 /*  (ADC       ) Base Address */
#define AT91SAM3X_BASE_PDC_ADC    0x400C0100 /*  (PDC_ADC   ) Base Address */
#define AT91SAM3X_BASE_DMAC       0x400C4000 /*  (DMAC      ) Base Address */
#define AT91SAM3X_BASE_DACC       0x400C8000 /*  (DACC      ) Base Address */
#define AT91SAM3X_BASE_PDC_DACC   0x400C8100 /*  (PDC_DACC  ) Base Address */

/*
 * System Peripherals
 */
#define AT91SAM3X_BASE_SMC     	  0x400E0000 /*  (SMC       ) Base Address */
#define AT91SAM3X_BASE_SDRAM      0x400E0200 /*  (SDRAM     ) Base Address */
#define AT91SAM3X_BASE_MATRIX     0x400E0400 /*  (MATRIX    ) Base Address */
#define AT91SAM3X_BASE_PMC        0x400E0600 /*  (PMC       ) Base Address */
#define AT91SAM3X_BASE_UART0      0x400E0800 /*  (UART0     ) Base Address */
#define AT91SAM3X_BASE_PDC_UART0  0x400E0900 /*  (PDC_UART0 ) Base Address */
#define AT91SAM3X_BASE_CHIPID     0x400E0940 /*  (CHIPID    ) Base Address */
#define AT91SAM3X_BASE_EEFC0      0x400E0A00 /*  (EEFC0     ) Base Address */
#define AT91SAM3X_BASE_EEFC1      0x400E0C00 /*  (EEFC1     ) Base Address */
#define AT91SAM3X_BASE_PIOA       0x400E0E00 /*  (PIOA      ) Base Address */
#define AT91SAM3X_BASE_PIOB       0x400E1000 /*  (PIOB      ) Base Address */
#define AT91SAM3X_BASE_PIOC       0x400E1200 /*  (PIOC      ) Base Address */
#define AT91SAM3X_BASE_PIOD       0x400E1400 /*  (PIOD      ) Base Address */
#define AT91SAM3X_BASE_PIOE       0x400E1600 /*  (PIOE      ) Base Address */
#define AT91SAM3X_BASE_PIOF       0x400E1800 /*  (PIOF      ) Base Address */
#define AT91SAM3X_BASE_RSTC       0x400E1A00 /*  (RSTC      ) Base Address */
#define AT91SAM3X_BASE_SUPC       0x400E1A10 /*  (SUPC      ) Base Address */
#define AT91SAM3X_BASE_RTT        0x400E1A30 /*  (RTT       ) Base Address */
#define AT91SAM3X_BASE_WDT        0x400E1A50 /*  (WDT       ) Base Address */
#define AT91SAM3X_BASE_RTC        0x400E1A60 /*  (RTC       ) Base Address */
#define AT91SAM3X_BASE_GPBR       0x400E1A90 /*  (GPBR      ) Base Address */

#define AT91_USART0	AT91SAM3X_BASE_USART0
#define AT91_USART1	AT91SAM3X_BASE_USART1
#define AT91_USART2	AT91SAM3X_BASE_USART2
#define AT91_USART3	AT91SAM3X_BASE_USART3

#define AT91_SMC				  AT91SAM3X_BASE_SMC
#define AT91_DBGU				  AT91SAM3X_BASE_UART0
#define AT91_BASE_SYS			  AT91SAM3X_BASE_SMC
#define AT91_PMC				  AT91SAM3X_BASE_PMC
#define AT91_TC				  	  AT91SAM3X_BASE_TC0
#define AT91_ECC				  AT91SAM3X_BASE_SMC + 0x20
#define AT91_GPBR				  0x400E1A90
/*
 * Internal Memory.
 */
#define AT91SAM3X_SRAM_BASE	0x20071000	/* Internal SRAM base address */
#define AT91SAM3X_SRAM_SIZE	0x17800		/* Internal SRAM size (98Kb) */

#define AT91SAM3X_ROM_BASE	0x00180000	/* Internal ROM base address */
#define AT91SAM3X_ROM_SIZE	SZ_64K		/* Internal ROM size (64Kb) */

//#define AT91SAM3X_LCDC_BASE	0x00500000	/* LCD Controller */
//#define AT91SAM3X_UDPHS_FIFO	0x00600000	/* USB Device HS controller */
//#define AT91SAM3X_OHCI_BASE	0x00700000	/* USB Host controller (OHCI) */
//#define AT91SAM3X_EHCI_BASE	0x00800000	/* USB Host controller (EHCI) */
//#define AT91SAM3X_VDEC_BASE	0x00900000	/* Video Decoder Controller */

/*
 * DMA peripheral identifiers
 * for hardware handshaking interface
 */
#define AT_DMA_ID_MCI0		 0
#define AT_DMA_ID_SPI0_TX	 1
#define AT_DMA_ID_SPI0_RX	 2
#define AT_DMA_ID_SPI1_TX	 5
#define AT_DMA_ID_SPI1_RX	 6
#define AT_DMA_ID_SSC0_TX	 3
#define AT_DMA_ID_SSC0_RX	 4
#define AT_DMA_ID_TWI0_TX	 7
#define AT_DMA_ID_TWI0_RX	 8
#define AT_DMA_ID_USART0_TX	 11
#define AT_DMA_ID_USART0_RX	 12
#define AT_DMA_ID_USART1_TX	 13
#define AT_DMA_ID_USART1_RX	 14
#define AT_DMA_ID_PWM		 15


#endif

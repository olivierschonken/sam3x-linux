/*
 * Matrix-centric header file for the AT91SAM9G45 family
 *
 *  Copyright (C) 2008-2009 Atmel Corporation.
 *
 * Memory Controllers (MATRIX, EBI) - System peripherals registers.
 * Based on AT91SAM9G45 preliminary datasheet.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef AT91SAM3X8H_MATRIX_H
#define AT91SAM3X8H_MATRIX_H

#define AT91_MATRIX_MCFG0	0x00			/* Master Configuration Register 0 */
#define AT91_MATRIX_MCFG1	0x04			/* Master Configuration Register 1 */
#define AT91_MATRIX_MCFG2	0x08			/* Master Configuration Register 2 */
#define AT91_MATRIX_MCFG3	0x0C			/* Master Configuration Register 3 */
#define AT91_MATRIX_MCFG4	0x10			/* Master Configuration Register 4 */
#define AT91_MATRIX_MCFG5	0x14			/* Master Configuration Register 5 */
#define		AT91_MATRIX_ULBT	(7 << 0)	/* Undefined Length Burst Type */
#define			AT91_MATRIX_ULBT_INFINITE	(0 << 0)
#define			AT91_MATRIX_ULBT_SINGLE		(1 << 0)
#define			AT91_MATRIX_ULBT_FOUR		(2 << 0)
#define			AT91_MATRIX_ULBT_EIGHT		(3 << 0)
#define			AT91_MATRIX_ULBT_SIXTEEN	(4 << 0)

#define AT91_MATRIX_SCFG0	0x40			/* Slave Configuration Register 0 */
#define AT91_MATRIX_SCFG1	0x44			/* Slave Configuration Register 1 */
#define AT91_MATRIX_SCFG2	0x48			/* Slave Configuration Register 2 */
#define AT91_MATRIX_SCFG3	0x4C			/* Slave Configuration Register 3 */
#define AT91_MATRIX_SCFG4	0x50			/* Slave Configuration Register 4 */
#define AT91_MATRIX_SCFG5	0x54			/* Slave Configuration Register 5 */
#define AT91_MATRIX_SCFG6	0x58			/* Slave Configuration Register 6 */
#define AT91_MATRIX_SCFG7	0x5C			/* Slave Configuration Register 7 */
#define AT91_MATRIX_SCFG8	0x60			/* Slave Configuration Register 8 */
#define		AT91_MATRIX_SLOT_CYCLE		(0xff << 0)	/* Maximum Number of Allowed Cycles for a Burst */
#define		AT91_MATRIX_DEFMSTR_TYPE	(3    << 16)	/* Default Master Type */
#define			AT91_MATRIX_DEFMSTR_TYPE_NONE	(0 << 16)
#define			AT91_MATRIX_DEFMSTR_TYPE_LAST	(1 << 16)
#define			AT91_MATRIX_DEFMSTR_TYPE_FIXED	(2 << 16)
#define		AT91_MATRIX_FIXED_DEFMSTR	(0x7  << 18)	/* Fixed Index of Default Master */

#define AT91_MATRIX_PRAS0	0x80			/* Priority Register A for Slave 0 */
#define AT91_MATRIX_PRAS1	0x88			/* Priority Register A for Slave 1 */
#define AT91_MATRIX_PRAS2	0x90			/* Priority Register A for Slave 2 */
#define AT91_MATRIX_PRAS3	0x98			/* Priority Register A for Slave 3 */
#define AT91_MATRIX_PRAS4	0xA0			/* Priority Register A for Slave 4 */
#define AT91_MATRIX_PRAS5	0xA8			/* Priority Register A for Slave 5 */
#define AT91_MATRIX_PRAS6	0xB0			/* Priority Register A for Slave 6 */
#define AT91_MATRIX_PRAS7	0xB8			/* Priority Register A for Slave 7 */
#define		AT91_MATRIX_M0PR		(3 << 0)	/* Master 0 Priority */
#define		AT91_MATRIX_M1PR		(3 << 4)	/* Master 1 Priority */
#define		AT91_MATRIX_M2PR		(3 << 8)	/* Master 2 Priority */
#define		AT91_MATRIX_M3PR		(3 << 12)	/* Master 3 Priority */
#define		AT91_MATRIX_M4PR		(3 << 16)	/* Master 4 Priority */
#define		AT91_MATRIX_M5PR		(3 << 20)	/* Master 5 Priority */


#define AT91_MATRIX_MRCR	0x100			/* Master Remap Control Register */
#define		AT91_MATRIX_RCB0		(1 << 0)	/* Remap Command for AHB Master 0 (ARM926EJ-S Instruction Master) */
#define		AT91_MATRIX_RCB1		(1 << 1)	/* Remap Command for AHB Master 1 (ARM926EJ-S Data Master) */
#define		AT91_MATRIX_RCB2		(1 << 2)
#define		AT91_MATRIX_RCB3		(1 << 3)
#define		AT91_MATRIX_RCB4		(1 << 4)
#define		AT91_MATRIX_RCB5		(1 << 5) /*datasheet spec two RCB4??*/
#define		AT91_MATRIX_RCB6		(1 << 6)

#define AT91_CCFG_SYSIO		0x114
#define		AT91_SYSIO12		(1 << 12)


#define AT91_MATRIX_WPMR	0x1E4			/* Write Protect Mode Register */
#define		AT91_MATRIX_WPMR_WPEN		(1 << 0)	/* Write Protect ENable */
#define			AT91_MATRIX_WPMR_WP_WPDIS		(0 << 0)
#define			AT91_MATRIX_WPMR_WP_WPEN		(1 << 0)
#define		AT91_MATRIX_WPMR_WPKEY		(0xFFFFFF << 8)	/* Write Protect KEY */

#define AT91_MATRIX_WPSR	0x1E8			/* Write Protect Status Register */
#define		AT91_MATRIX_WPSR_WPVS		(1 << 0)	/* Write Protect Violation Status */
#define			AT91_MATRIX_WPSR_NO_WPV		(0 << 0)
#define			AT91_MATRIX_WPSR_WPV		(1 << 0)
#define		AT91_MATRIX_WPSR_WPVSRC		(0xFFFF << 8)	/* Write Protect Violation Source */

#endif

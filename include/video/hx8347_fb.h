
#ifndef _HX8347_FB_H_
#define _HX8347_FB_H_

#if defined(__KERNEL__)
#include <linux/types.h>
#else
#include <inttypes.h>
#endif
#include <linux/ioctl.h>

#define SCREEN_MAJOR	245
#define SCREEN_MINOR	0
#define HX8347_OPC_INDEX	0x70
#define HX8347_OPC_DATA	0x72

/* Color definitions (RGB) */
#define HX8347_WHITE		0xFFFF
#define HX8347_BLACK		0x0000
#define HX8347_RED		0x7800
#define HX8347_LRED		0xF800
#define HX8347_GREEN		0x03E0
#define HX8347_LGREEN		0x07E0
#define HX8347_BLUE		0x000F
#define HX8347_LBLUE		0x001F
#define HX8347_CYAN		0x07FF
#define HX8347_MAGENTA	0xF81F
#define HX8347_YELLOW		0xFFE0
#define HX8347_BROWN		0xB104
#define HX8347_ORANGE		0xFD00
#define HX8347_PINK		0xFB14

/* Ioctl definitions */
#define HX8347_IOCTL	0xCC

/* The driver's ioctl command values */
#define HX8347_SETREG		_IOW(HX8347_IOCTL, 0, int)
#define HX8347_SETDAT		_IOW(HX8347_IOCTL, 1, int)
#define HX8347_CLS		_IOW(HX8347_IOCTL, 2, int)
#define HX8347_DRAW		_IOW(HX8347_IOCTL, 3, unsigned short *)
#define HX8347_DIM		_IOW(HX8347_IOCTL, 4, int)
#define HX8347_ONOFF		_IOW(HX8347_IOCTL, 5, int)
#define HX8347_POWER		_IOW(HX8347_IOCTL, 6, int)

/* Ioctl command options */
#define HX8347_OP_RECT	1
#define HX8347_OP_FILL	2
#define HX8347_OP_PIXELS	3

/// HX8347 LCD Registers
#define HX8347_R00H        0x00
#define HX8347_R01H        0x01
#define HX8347_R02H        0x02
#define HX8347_R03H        0x03
#define HX8347_R04H        0x04
#define HX8347_R05H        0x05
#define HX8347_R06H        0x06
#define HX8347_R07H        0x07
#define HX8347_R08H        0x08
#define HX8347_R09H        0x09
#define HX8347_R0AH        0x0A
#define HX8347_R0CH        0x0C
#define HX8347_R0DH        0x0D
#define HX8347_R0EH        0x0E
#define HX8347_R0FH        0x0F
#define HX8347_R10H        0x10
#define HX8347_R11H        0x11
#define HX8347_R12H        0x12
#define HX8347_R13H        0x13
#define HX8347_R14H        0x14
#define HX8347_R15H        0x15
#define HX8347_R16H        0x16
#define HX8347_R18H        0x18
#define HX8347_R19H        0x19
#define HX8347_R1AH        0x1A
#define HX8347_R1BH        0x1B
#define HX8347_R1CH        0x1C
#define HX8347_R1DH        0x1D
#define HX8347_R1EH        0x1E
#define HX8347_R1FH        0x1F
#define HX8347_R20H        0x20
#define HX8347_R21H        0x21
#define HX8347_R22H        0x22
#define HX8347_R23H        0x23
#define HX8347_R24H        0x24
#define HX8347_R25H        0x25
#define HX8347_R26H        0x26
#define HX8347_R27H        0x27
#define HX8347_R28H        0x28
#define HX8347_R29H        0x29
#define HX8347_R2AH        0x2A
#define HX8347_R2BH        0x2B
#define HX8347_R2CH        0x2C
#define HX8347_R2DH        0x2D
#define HX8347_R35H        0x35
#define HX8347_R36H        0x36
#define HX8347_R37H        0x37
#define HX8347_R38H        0x38
#define HX8347_R39H        0x39
#define HX8347_R3AH        0x3A
#define HX8347_R3BH        0x3B
#define HX8347_R3CH        0x3C
#define HX8347_R3DH        0x3D
#define HX8347_R3EH        0x3E
#define HX8347_R40H        0x40
#define HX8347_R41H        0x41
#define HX8347_R42H        0x42
#define HX8347_R43H        0x43
#define HX8347_R44H        0x44
#define HX8347_R45H        0x45
#define HX8347_R46H        0x46
#define HX8347_R47H        0x47
#define HX8347_R48H        0x48
#define HX8347_R49H        0x49
#define HX8347_R4AH        0x4A
#define HX8347_R4BH        0x4B
#define HX8347_R4CH        0x4C
#define HX8347_R4DH        0x4D
#define HX8347_R4EH        0x4E
#define HX8347_R4FH        0x4F
#define HX8347_R50H        0x50
#define HX8347_R51H        0x51
#define HX8347_R64H        0x64
#define HX8347_R65H        0x65
#define HX8347_R66H        0x66
#define HX8347_R67H        0x67
#define HX8347_R70H        0x70
#define HX8347_R72H        0x72
#define HX8347_R90H        0x90
#define HX8347_R91H        0x91
#define HX8347_R93H        0x93
#define HX8347_R94H        0x94
#define HX8347_R95H        0x95

struct hx8347_device_data {
	unsigned cs_addr;	/* HX8347 chip select address */
	unsigned pin_cs;	/* HX8347 chip select pin */
	unsigned pin_rs;	/* HX8347 register select pin */
	unsigned pin_reset;	/* HX8347 reset pin */
	unsigned pin_spb;	/* HX8347 I/F select pin (0=parallel, 1=serial) */
	unsigned pin_mib;	/* HX8347 I/F bus type pin (0=80x, 1=68x)       */
	unsigned pin_pwr;	/* HX8347 power control */
	unsigned pin_nrd;	/* HX8347 NRD */
	unsigned pin_nwe;	/* HX8347 NWE */
	unsigned pin_backlight;
};

struct hx8347_draw_parms {
	int op;
	int xs;
	int ys;
	int xe;
	int ye;
	int fcol;
	int bcol;
	unsigned short *buf;
};

#endif // _HX8347_FB_H_

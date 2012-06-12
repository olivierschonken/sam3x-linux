#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/proc_fs.h>
#include <linux/sysctl.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>
#include <video/hx8347_fb.h>
#include <asm/uaccess.h>
#include <asm/errno.h>
#include <mach/gpio.h>

#define SCREEN_WIDTH		319
#define SCREEN_HEIGHT		239
#define BYTES_PER_PIXEL		2
#define BITS_PER_PIXEL		(BYTES_PER_PIXEL * 8)
#define ROWLEN				((SCREEN_WIDTH + 1) * BYTES_PER_PIXEL)

static struct fb_fix_screeninfo hx8347_fb_fix __devinitdata =
{
	.id				= "HX8347",
	.type			= FB_TYPE_PACKED_PIXELS,
	.visual			= FB_VISUAL_TRUECOLOR,
	.line_length	= ROWLEN,
	.accel			= FB_ACCEL_NONE,
};

static struct fb_var_screeninfo hx8347_fb_var __devinitdata =
{
	.xres			= SCREEN_WIDTH,
	.yres			= SCREEN_HEIGHT,
	.xres_virtual	= SCREEN_WIDTH,
	.yres_virtual	= SCREEN_HEIGHT,
	.xoffset		= 0,
	.yoffset		= 0,
	.bits_per_pixel	= BITS_PER_PIXEL,
	.red			= {11, 5, 0},
	.green			= {5, 6, 0},
	.blue			= {0, 5, 0},
	.activate		= FB_ACTIVATE_NOW,
	.height			= 0,
	.width          = 0,
};

struct hx8347_par
{
	struct fb_info*	info;		/* FB driver info record    */
	void __iomem*	mmio;		/* HX8347 chip select address */
	unsigned		dat;		/* HX8347 data select pin  */
	unsigned		reset;		/* HX8347 reset pin           */
	unsigned		pwr;		/* HX8347 power pin           */
	u16*			scr;		/* Screen buffer pointer    */
	u32				pseudo_palette[32];
};

/* Main driver allocated dynamically in probe function */
struct hx8347_par* fb_dev = NULL;

/**
 * hx8347_delay(). Machine dependant delay (nano-second range).
 * @param[in]  delay - No. of delay loop iterations
 */
static void hx8347_delay(int delay)
{
	int i;
	for (i = 0; i < delay; i++) ;
}

/**
 * hx8347_write_cmd(). Writes an HX8347 command word.
 * @param[in]  hx8347  - Pointer to HX8347 parameters structure
 * @param[in]  cmd   - Command word
 * @return     int   - 0 = SUCCESS
 */
static int hx8347_write_cmd(struct hx8347_par *hx8347, u32 cmd)
{
	//at91_set_gpio_value(hx8347->cmd, 0);
	__raw_writeb((u8)cmd, hx8347->mmio);
	//at91_set_gpio_value(hx8347->cmd, 1);
	udelay(1);

	return 0;
}

/**
 * hx8347_write_dat(). Writes an HX8347 data word.
 * @param[in]  hx8347  - Pointer to HX8347 parameters structure
 * @param[in]  data  - Data word
 * @return     int   - 0 = SUCCESS
 */
static int hx8347_write_dat(struct hx8347_par *hx8347, u32 data)
{
	__raw_writew((u16)data, hx8347->mmio + (1 << 1));
	return 0;
}

/**
 * hx8347_write_reg(). Writes to an HX8347 register.
 * @param[in]  hx8347  - Pointer to HX8347 parameters structure
 * @param[in]  cmd   - Command word
 * @param[in]  data  - Data word
 * @return     int   - 0 = SUCCESS
 */
static int hx8347_write_reg(struct hx8347_par *hx8347, u32 cmd, u32 data)
{
	hx8347_write_cmd(hx8347, cmd);
	hx8347_write_dat(hx8347, data);
	return 0;
}

/**
 * hx8347_setxy(). Sets the next X and Y pixel co-ordinate.
 * @param[in]  hx8347  - Pointer to HX8347 parameters structure
 * @param[in]  x     - X co-ordinate
 * @param[in]  y     - Y co-ordinate
 */
static void hx8347_setxy(struct hx8347_par *hx8347, int x, int y)
{
    hx8347_write_reg(hx8347, HX8347_R02H, (x & 0xff00) >>8); // column high
    hx8347_write_reg(hx8347, HX8347_R03H, x & 0xff); // column low
    hx8347_write_reg(hx8347, HX8347_R06H, (y & 0xff00) >>8); // row high
    hx8347_write_reg(hx8347, HX8347_R07H, y & 0xff); // row low
}

/**
 * hx8347_power(). Powers the HX8347 display on/off.
 * @param[in]  onoff - 1 = on, 0 = off
 */
static void hx8347_power(struct hx8347_par *hx8347, int onoff)
{

	if (onoff)
	{
	    // Display ON Setting
	    hx8347_write_reg(hx8347, HX8347_R90H, 0x7F ) ; // SAP=0111 1111
	    hx8347_write_reg(hx8347, HX8347_R26H, 0x04 ) ; // GON=0, DTE=0, D=01
	    mdelay( 100 ) ;
	    hx8347_write_reg(hx8347, HX8347_R26H, 0x24 ) ; // GON=1, DTE=0, D=01
	    hx8347_write_reg(hx8347, HX8347_R26H, 0x2C ) ; // GON=1, DTE=0, D=11
	    mdelay( 100 ) ;
	    hx8347_write_reg(hx8347, HX8347_R26H, 0x3C ) ; // GON=1, DTE=1, D=11
	}
	else
	{
		hx8347_write_reg(hx8347, HX8347_R90H, 0x00 ) ; // SAP=0000 0000
	hx8347_write_reg(hx8347, HX8347_R26H, 0x00 ) ; // GON=0, DTE=0, D=00
	}
}

/**
 * hx8347_setup(). Initialise the HX8347 display registers.
 * @param[in]  info  - Pointer to a frame buffer parameters structure
 */
static void hx8347_setup(struct fb_info* info)
{
	struct hx8347_par *hx8347 = (struct hx8347_par *)info->par;

	/* Reset HX8347 */
	//at91_set_gpio_value(hx8347->reset, 0);
	//hx8347_delay(10);
	//at91_set_gpio_value(hx8347->reset, 1);
	//hx8347_write_reg(hx8347, HX8347_R90H, 0x00 ) ; // SAP=0000 0000
	//hx8347_write_reg(hx8347, HX8347_R26H, 0x00 ) ; // GON=0, DTE=0, D=00


	/* Init HX8347 registers */
	// Start internal OSC
	hx8347_write_reg(hx8347, HX8347_R19H, 0x49); // OSCADJ=10 0000, OSD_EN=1 //60Hz
	hx8347_write_reg(hx8347, HX8347_R93H, 0x0C); // RADJ=1100

	// Power on flow
	hx8347_write_reg(hx8347, HX8347_R44H, 0x4D); // VCM=100 1101
	hx8347_write_reg(hx8347, HX8347_R45H, 0x11); // VDV=1 0001
	hx8347_write_reg(hx8347, HX8347_R20H, 0x40); // BT=0100
	hx8347_write_reg(hx8347, HX8347_R1DH, 0x07); // VC1=111
	hx8347_write_reg(hx8347, HX8347_R1EH, 0x00); // VC3=000
	hx8347_write_reg(hx8347, HX8347_R1FH, 0x04); // VRH=0100

	hx8347_write_reg(hx8347, HX8347_R1CH, 0x04); // AP=100
	hx8347_write_reg(hx8347, HX8347_R1BH, 0x10); // GASENB=0, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
	mdelay(50);

	hx8347_write_reg(hx8347, HX8347_R43H, 0x80); // Set VCOMG=1
	mdelay(50);
	/*
	// Gamma for CMO 2.8
	hx8347_write_reg(hx8347, HX8347_R46H, 0x95);
	hx8347_write_reg(hx8347, HX8347_R47H, 0x51);
	hx8347_write_reg(hx8347, HX8347_R48H, 0x00);
	hx8347_write_reg(hx8347, HX8347_R49H, 0x36);
	hx8347_write_reg(hx8347, HX8347_R4AH, 0x11);
	hx8347_write_reg(hx8347, HX8347_R4BH, 0x66);
	hx8347_write_reg(hx8347, HX8347_R4CH, 0x14);
	hx8347_write_reg(hx8347, HX8347_R4DH, 0x77);
	hx8347_write_reg(hx8347, HX8347_R4EH, 0x13);
	hx8347_write_reg(hx8347, HX8347_R4FH, 0x4C);
	hx8347_write_reg(hx8347, HX8347_R50H, 0x46);
	hx8347_write_reg(hx8347, HX8347_R51H, 0x46);
	*/
	//240x320 window setting
	hx8347_write_reg(hx8347, HX8347_R02H, 0x00); // Column address start2
	hx8347_write_reg(hx8347, HX8347_R03H, 0x00); // Column address start1
	hx8347_write_reg(hx8347, HX8347_R04H, 0x00); // Column address end2
	hx8347_write_reg(hx8347, HX8347_R05H, 0xEF); // Column address end1
	hx8347_write_reg(hx8347, HX8347_R06H, 0x00); // Row address start2
	hx8347_write_reg(hx8347, HX8347_R07H, 0x00); // Row address start1
	hx8347_write_reg(hx8347, HX8347_R08H, 0x01); // Row address end2
	hx8347_write_reg(hx8347, HX8347_R09H, 0x3F); // Row address end1

	// Display Setting
	hx8347_write_reg(hx8347, HX8347_R01H, 0x06); // IDMON=0, INVON=1, NORON=1, PTLON=0
	//hx8347_write_reg(hx8347, HX8347_R16H, 0xC8); // MY=1, MX=1, MV=0, BGR=1
	hx8347_write_reg(hx8347, HX8347_R16H, 0x68); // MY=0, MX=1, MV=1, RGB XY exchange X mirror
	hx8347_write_reg(hx8347, HX8347_R23H, 0x95); // N_DC=1001 0101
	hx8347_write_reg(hx8347, HX8347_R24H, 0x95); // P_DC=1001 0101
	hx8347_write_reg(hx8347, HX8347_R25H, 0xFF); // I_DC=1111 1111
	hx8347_write_reg(hx8347, HX8347_R27H, 0x06); // N_BP=0000 0110
	hx8347_write_reg(hx8347, HX8347_R28H, 0x06); // N_FP=0000 0110
	hx8347_write_reg(hx8347, HX8347_R29H, 0x06); // P_BP=0000 0110
	hx8347_write_reg(hx8347, HX8347_R2AH, 0x06); // P_FP=0000 0110
	hx8347_write_reg(hx8347, HX8347_R2CH, 0x06); // I_BP=0000 0110
	hx8347_write_reg(hx8347, HX8347_R2DH, 0x06); // I_FP=0000 0110
	hx8347_write_reg(hx8347, HX8347_R3AH, 0x01); // N_RTN=0000, N_NW=001
	hx8347_write_reg(hx8347, HX8347_R3BH, 0x01); // P_RTN=0000, P_NW=001
	hx8347_write_reg(hx8347, HX8347_R3CH, 0xF0); // I_RTN=1111, I_NW=000
	hx8347_write_reg(hx8347, HX8347_R3DH, 0x00); // DIV=00
	hx8347_write_reg(hx8347, HX8347_R3EH, 0x38); // SON=38h
	hx8347_write_reg(hx8347, HX8347_R40H, 0x0F); // GDON=0Fh
	hx8347_write_reg(hx8347, HX8347_R41H, 0xF0); // GDOF=F0h


	hx8347_setxy(hx8347, 0, 0);	/* Set xy pos to 0,0 */
}

/**
 * hx8347_window(). Set window for graphics writing. After this setting
 * all graphics writes are confined to the window.
 * @param[in]  info  - Pointer to a frame buffer parameters structure
 * @param[in]  x0    - Top-left X co-ordinate
 * @param[in]  y0    - Top-left Y co-ordinate
 * @param[in]  x1    - Bottom-right X co-ordinate
 * @param[in]  y1    - Bottom-right Y co-ordinate
 * @return     int   - 0 = SUCCESS
 */
static int hx8347_window(struct fb_info* info, int x0, int y0, int x1, int y1)
{
	int t;

	/* Force coordinates into ascending order  */
	if (x0 > x1) {t = x0; x0 = x1; x1 =t;}
	if (y0 > y1) {t = y0; y0 = y1; y1 =t;}
	//if (x1 >= SCREEN_WIDTH) {x1 = SCREEN_WIDTH - 1;}
	//if (y1 >= SCREEN_HEIGHT) {y1 = SCREEN_HEIGHT - 1;}

	hx8347_write_reg(info->par, HX8347_R02H, (x0 & 0xff00) >> 8); // Column address start2
	hx8347_write_reg(info->par, HX8347_R03H, x0 & 0xff); // Column address start1
	hx8347_write_reg(info->par, HX8347_R04H, (x1 & 0xff00) >>8); // Column address end2
	hx8347_write_reg(info->par, HX8347_R05H, x1 & 0xff); // Column address end1
	hx8347_write_reg(info->par, HX8347_R06H, (y0 & 0xff00) >>8); // Row address start2
	hx8347_write_reg(info->par, HX8347_R07H, y0 & 0xff); // Row address start1
	hx8347_write_reg(info->par, HX8347_R08H, (y1 & 0xff00) >>8 ); // Row address end2
	hx8347_write_reg(info->par, HX8347_R09H, y1 & 0xff); // Row address end1

	hx8347_setxy(info->par, x0, y0);	/* Set xy pos */

	return (x1 - x0 + 1) * (y1 - y0 + 1);
}

/**
 * hx8347_write_array(). Write a block of pixels to the HX8347 display.
 * @param[in]  info  - Pointer to a frame buffer parameters structure
 * @param[in]  pd    - Pointer to an HX8347 draw parameters structure
 * @return     int   - 0 = SUCCESS
 */
static int hx8347_write_array(struct fb_info *info, struct hx8347_draw_parms *pd)
{
	struct hx8347_par *hx8347 = (struct hx8347_par *)info->par;
	int i, j, len;
	int bufpos, lineWidth, lines, startline, startwidth;
	int actuallen;
	//bufpos =
	actuallen = 0;
	len = hx8347_window(info, pd->xs, pd->ys, pd->xe, pd->ye);
	//early_printk("len %d xs", len);
	if (pd->xs > pd->xe) {lineWidth = pd->xs - pd->xe + 1;  startwidth = pd->xe;}
	else {lineWidth = pd->xe - pd->xs + 1;  startwidth = pd->xs;}
	if (pd->ys > pd->ye) {lines = pd->ys - pd->ye + 1; startline = pd->ye;}
	else {lines = pd->ye - pd->ys + 1; startline = pd->ys;}
	early_printk("len %d x0 %d x1 %d y0 %d y1 %d\n",len, pd->xs, pd->xe, pd->ys, pd->ye);
    /* Prepare to write in GRAM */
    hx8347_write_cmd(hx8347, HX8347_R22H ) ;
    //early_printk("x-start %d   x-stop %d  y-start %d   y-stop %d\n", pd->xs, pd->xe , pd->ys, pd->ye);
	if (pd->op == HX8347_OP_PIXELS)
	{
	    for (j = 0; j < lines; j++)
	    {
		bufpos = (startline++ * (SCREEN_WIDTH + 1)) + startwidth;
			for (i = 0; i < lineWidth; i++)
			{
				actuallen++;
				hx8347_write_dat(hx8347, pd->buf[bufpos++]);
			}
	    }
	    //early_printk("len %d actual len %d", len, actuallen);
	}
	else if (pd->op == HX8347_OP_FILL)
	{
		for (i = 0; i < len; i++)
			hx8347_write_dat(hx8347, pd->fcol);
	}
	else
		return -EINVAL;

	return 0;
}

/**
 * hx8347_cls(). Clear the screen with a specified colour.
 * @param[in]  info  - Pointer to a frame buffer parameters structure
 * @param[in]  clr   - Pixel RGB colour
 */
static void hx8347_cls(struct fb_info *info, u16 clr)
{
	struct hx8347_draw_parms pd =
	{
		.op = HX8347_OP_FILL,
		.xs = 0,
		.ys = 0,
		.xe = SCREEN_WIDTH,
		.ye = SCREEN_HEIGHT,
		.fcol = (int)clr,
	};

	hx8347_write_array(info, &pd);
}

/**
 * hx8347_fb_fillrect(). Frame buffer compatible fill rectangle function.
 * Note that this is an external function
 * @param[in]  info  - Pointer to a frame buffer parameters structure
 * @param[in]  rect  - Pointer to rectangle parameters structure
 */
static void hx8347_fb_fillrect(struct fb_info *info, const struct fb_fillrect *rect)
{
	struct hx8347_draw_parms pd;

	cfb_fillrect(info, rect);

	if ((rect->dx > SCREEN_WIDTH) || (rect->dy > SCREEN_HEIGHT)
		|| ((rect->dx + rect->width) > SCREEN_WIDTH)
		|| ((rect->dy + rect->height) > SCREEN_HEIGHT))
	{
		dev_err(info->device, "hx8347_fb: Bad coordinates\n");
		return;
	}

	pd.op = HX8347_OP_FILL;
	pd.xs = rect->dx;
	pd.ys = rect->dy;
	pd.xe = rect->dx + rect->width;
	pd.ye = rect->dy + rect->height;
	pd.fcol = rect->color;

	hx8347_write_array(info, &pd);
}

/**
 * hx8347_fb_imageblit(). Frame buffer compatible image pixel dump.
 * @param[in]  info  - Pointer to a frame buffer parameters structure
 * @param[in]  image - Pointer to rectangle parameters structure
 */
static void hx8347_fb_imageblit(struct fb_info *info, const struct fb_image *image)
{
	struct hx8347_draw_parms pd;

	cfb_imageblit(info, image);

	if ((image->dx > SCREEN_WIDTH) || (image->dy > SCREEN_HEIGHT)
		|| ((image->dx + image->width) > SCREEN_WIDTH)
		|| ((image->dy + image->height) > SCREEN_HEIGHT))
	{
		dev_err(info->device, "hx8347_fb: Bad coordinates\n");
		return;
	}

	pd.op = HX8347_OP_PIXELS;
	pd.xs = image->dx;
	pd.ys = image->dy;
	pd.xe = image->dx + image->width;
	pd.ye = image->dy + image->height;
	pd.buf = (u16*)info->screen_base;

	hx8347_write_array(info, &pd);
}

/**
 * hx8347_fb_copyarea(). Frame buffer compatible copy screen area.
 * @param[in]  info  - Pointer to a frame buffer parameters structure
 * @param[in]  area  - Pointer to copy area parameters structure
 */
static void hx8347_fb_copyarea(struct fb_info *info, const struct fb_copyarea *area)
{
	struct hx8347_draw_parms pd;

	cfb_copyarea(info, area);

	if ((area->dx > SCREEN_WIDTH) || (area->dy > SCREEN_HEIGHT)
		|| ((area->dx + area->width) > SCREEN_WIDTH)
		|| ((area->dy + area->height) > SCREEN_HEIGHT))
	{
		dev_err(info->device, "hx8347_fb: Bad coordinates\n");
		return;
	}

	pd.op = HX8347_OP_PIXELS;
	pd.xs = area->dx;
	pd.ys = area->dy;
	pd.xe = area->dx + area->width;
	pd.ye = area->dy + area->height;
	pd.buf = (u16*)info->screen_base;

	hx8347_write_array(info, &pd);
}

/**
 * hx8347_fb_read(). Userland read function (currently not supported).
 * @param[in]  info  - Pointer to a frame buffer parameters structure
 * @param[out] buf   - Pointer to output buffer for read pixels
 * @param[in]  count - No. of pixels to read
 * @param[in]  ppos  - Pointer to loff_t structure (file seek pointer)
 * @return     ssize_t - 0 = SUCCESS
 */
static ssize_t hx8347_fb_read(struct fb_info *info, char *buf, size_t count, loff_t *ppos)
{
	return 0;
}

/**
 * hx8347_fb_ioctl(). IOCTL function for userland interaces.
 * @param[in]  info  - Pointer to a frame buffer parameters structure
 * @param[in]  cmd   - IOCTL command
 * @param[in/out] arg - Userland argument
 * @return     ssize_t - 0 = SUCCESS
 */
static ssize_t hx8347_fb_ioctl(struct fb_info *info, unsigned int cmd, unsigned long arg)
{
	struct hx8347_draw_parms *pd;
	int i, len;

	switch (cmd)
	{
		case HX8347_SETREG:
			hx8347_write_cmd(info->par, arg); /* Set reg addr */
			break;

		case HX8347_SETDAT:
			hx8347_write_dat(info->par, arg); /* Write reg data */
			break;

		case HX8347_CLS:
			hx8347_cls(info, (u16)arg);
			break;

		case HX8347_DRAW:
			pd = (struct hx8347_draw_parms __user *)arg;
			if (pd->op == HX8347_OP_RECT)
			{
				hx8347_setxy(info->par, pd->xs, pd->ys);
				for (i = pd->xs; i < pd->xe+1; i++)
					hx8347_write_dat(info->par, pd->fcol);

				hx8347_setxy(info->par, pd->xs, pd->ye);
				for (i = pd->xs; i < pd->xe+1; i++)
					hx8347_write_dat(info->par, pd->fcol);

				len = hx8347_window(info, pd->xs, pd->ys, pd->xs, pd->ye);
				for (i = 0; i < len; i++)
					hx8347_write_dat(info->par, pd->fcol);

				len = hx8347_window(info, pd->xe, pd->ys, pd->xe, pd->ye);
				for (i = 0; i < len; i++)
					hx8347_write_dat(info->par, pd->fcol);

				hx8347_window(info, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			}
			else if ((pd->op == HX8347_OP_FILL) || (pd->op == HX8347_OP_PIXELS))
				hx8347_write_array(info, pd);
			else
				return -EINVAL;
			break;

		case HX8347_DIM:
			//if (arg)
				//hx8347_write_reg(info->par, 0x43, 0x0008);	/* Gamma Set 9 */
			//else
				//hx8347_write_reg(info->par, 0x43, 0x0000);	/* Gamma Set 1 */
			//hx8347_write_cmd(info->par, 0x22);				/* Write Gram */
			break;

		case HX8347_ONOFF:
			//if (arg)
				//hx8347_write_reg(info->par, 0x05, 1);		/* Turn Display on */
			//else
				//hx8347_write_reg(info->par, 0x05, 0);		/* Turn Display off */
			//hx8347_write_cmd(info->par, 0x22);			/* Write Gram */
			break;

		case HX8347_POWER:
			if (arg)
			{
				hx8347_power(info->par, 1);
				hx8347_delay(10);
				hx8347_setup(info);
			}
			else
				hx8347_power(info->par, 0);

		default:
			return -EINVAL;
	}
	return 0;
}


/**
 *	hx8347_fb_open - Optional function. Called when the framebuffer is
 *		     first accessed.
 *	@info: frame buffer structure that represents a single frame buffer
 *	@user: tell us if the userland (value=1) or the console is accessing
 *	       the framebuffer.
 *
 *	This function is the first function called in the framebuffer api.
 *	Usually you don't need to provide this function. The case where it
 *	is used is to change from a text mode hardware state to a graphics
 * 	mode state.
 *
 *	Returns negative errno on error, or zero on success.
 */
static int hx8347_fb_open(struct fb_info *info, int user)
{
    return 0;
}

/**
 *	xxxfb_release - Optional function. Called when the framebuffer
 *			device is closed.
 *	@info: frame buffer structure that represents a single frame buffer
 *	@user: tell us if the userland (value=1) or the console is accessing
 *	       the framebuffer.
 *
 *	Thus function is called when we close /dev/fb or the framebuffer
 *	console system is released. Usually you don't need this function.
 *	The case where it is usually used is to go from a graphics state
 *	to a text mode state.
 *
 *	Returns negative errno on error, or zero on success.
 */
static int hx8347_fb_release(struct fb_info *info, int user)
{
    return 0;
}

/**
 * hx8347_fb_write(). Userland write function.
 * @param[in]  info  - Pointer to a frame buffer parameters structure
 * @param[in]  buf   - Pointer to pixel buffer
 * @param[in]  count - No. of pixels to write
 * @param[in]  ppos  - Pointer to loff_t structure (file seek pointer)
 * @return     ssize_t - 0 = SUCCESS
 */
static ssize_t hx8347_fb_write(struct fb_info *info,
	const char *buf, size_t count, loff_t *ppos)
{
	struct hx8347_draw_parms pd;
	unsigned long p = *ppos;
	unsigned int fb_mem_len = (SCREEN_WIDTH + 1) * (SCREEN_HEIGHT + 1) * BYTES_PER_PIXEL;
	int err;

	if (p > fb_mem_len)	/* Check range */
		return -ENOSPC;

	if (count >= fb_mem_len)
		count = fb_mem_len;

	err = 0;

	if ((count + p) > fb_mem_len)
	{
		count = fb_mem_len - p;
		err = -ENOSPC;
	}

	if (count)
	{
		char *base_addr;

		base_addr = info->screen_base;
		count -= copy_from_user(base_addr + p, buf, count);
		*ppos += count;
		err = -EFAULT;
	}

	if (count)
	{
		pd.op = HX8347_OP_PIXELS;
		pd.xs = 0;
		pd.ys = p / ROWLEN;
		pd.xe = SCREEN_WIDTH;
		pd.ye = pd.ys + ((count + ROWLEN-1) / ROWLEN);
		pd.buf = (u16*)info->screen_base;
		hx8347_write_array(info, &pd);
		return count;
	}
	return err;
}

static ssize_t hx8347_fb_flushwrite(struct fb_info *info)
{
	struct hx8347_draw_parms pd;
	unsigned int fb_mem_len = (SCREEN_WIDTH + 1) * (SCREEN_HEIGHT + 1) * BYTES_PER_PIXEL;
	int err;
	struct hx8347_par *hx8347 = (struct hx8347_par *)info->par;
	int i = 0;
	early_printk("mmio base : %8x", hx8347->mmio);
	hx8347_cls(info, HX8347_BLACK);
	//for (i = 0; i < SCREEN_HEIGHT; i+=12)
	//{
		pd.op = HX8347_OP_PIXELS;
		pd.xs = 0;
		pd.xe = SCREEN_WIDTH;
		pd.ys = 0;//i
		pd.ye = SCREEN_HEIGHT;//i + 11;
		early_printk("screen base : %8x", info->screen_base);
		pd.buf = (u16*)info->screen_base;
		hx8347_write_array(info, &pd);
	//}
	return fb_mem_len;
}

static inline unsigned int chan_to_field( unsigned int chan,
					const struct fb_bitfield *bf )
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}

/**
 * hx8347_fb_setcolreg(). Frame buffer compatible set colour registers.
 * @param[in]  regno  - Register number
 * @param[in]  red    - RGB red value
 * @param[in]  green  - RGB green value
 * @param[in]  blue   - RGB blue value
 * @param[in]  transp - Pixel transparency value
 * @param[in]  info   - Pointer to a frame buffer parameters structure
 * @return     int    - 0 = SUCCESS
 */
static int hx8347_fb_setcolreg(unsigned int regno, unsigned int red,
	unsigned int green, unsigned int blue,
	unsigned int transp, struct fb_info *info)
{
	unsigned int val;
	u32 *pal;
	int ret = 1;

	if (info->var.grayscale)
		red = green = blue = (19595 * red + 38470 * green
                  + 7471 * blue) >> 16;

	switch (info->fix.visual)
	{
		case FB_VISUAL_TRUECOLOR:
			if (regno < 16)
			{
				pal = info->pseudo_palette;
				val  = chan_to_field(red, &info->var.red);
				val |= chan_to_field(green, &info->var.green);
				val |= chan_to_field(blue, &info->var.blue);
				pal[regno] = val;
				ret = 0;
			}
			break;
	}
	return ret;
}

static int hx8347_fb_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
	unsigned long start = vma->vm_start;
	unsigned long size = vma->vm_end - vma->vm_start;
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	unsigned long page, pos;

	early_printk("\n\hx8347_fb_mmap\n\n");


	if (offset + size > info->fix.smem_len)
		return -EINVAL;

	pos = (unsigned long)info->fix.smem_start + offset;

	pr_notice("mmap() framebuffer addr:%lu size:%lu\n",
		  pos, size);

	while (size > 0) {
		//page = vmalloc_to_pfn((void *)pos);
		//if (remap_pfn_range(vma, start, page, PAGE_SIZE, PAGE_SHARED))
		//	return -EAGAIN;

		start += PAGE_SIZE;
		pos += PAGE_SIZE;
		if (size > PAGE_SIZE)
			size -= PAGE_SIZE;
		else
			size = 0;
	}

	//vma->vm_flags |= VM_RESERVED;	/* avoid to swap out this VMA */
	return 0;
}

static struct fb_ops hx8347_fb_ops =
{
	.owner			= THIS_MODULE,
	.fb_open		= hx8347_fb_open,
	.fb_read		= hx8347_fb_read,
	.fb_write		= hx8347_fb_write,
	.fb_release		= hx8347_fb_release,
	.fb_ioctl		= hx8347_fb_ioctl,
	.fb_fillrect	= hx8347_fb_fillrect,
	.fb_copyarea	= hx8347_fb_copyarea,
	.fb_imageblit	= hx8347_fb_imageblit,
	.fb_setcolreg	= hx8347_fb_setcolreg,
	.fb_write_flush = hx8347_fb_flushwrite,
//	.fb_mmap		= hx8347_fb_mmap,
};


/**
 * hx8347_probe(). Driver initialization and framebuffer setup.
 * @param[in]  pdev  - Pointer to a platform device structure
 * @return     int   - 0 = SUCCESS
 */
static int __init hx8347_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct fb_info* info = NULL;
	struct resource *pres = NULL;
	struct hx8347_device_data *pdat = NULL;
	struct hx8347_draw_parms oparms;
	unsigned char *vmem = NULL;
	unsigned int vsize = (SCREEN_WIDTH + 1) * (SCREEN_HEIGHT + 1) * BYTES_PER_PIXEL;
	int ret;

	fb_dev = NULL;

	/* Allocate video RAM */
	vmem = vmalloc(vsize);
	if (!vmem)
		return -ENOMEM;

	//early_printk("\n VMEM: %8x", vmem);
	memset(vmem, 0x00, vsize);
	hx8347_fb_fix.smem_start = (unsigned long)vmem;
	hx8347_fb_fix.smem_len = vsize;

	info = framebuffer_alloc(sizeof(struct hx8347_par), dev);
	if (!info)
		goto err;

	info->screen_base = (char __iomem *)vmem;
	info->fbops = &hx8347_fb_ops;
	info->var   = hx8347_fb_var;
	info->fix   = hx8347_fb_fix;
	info->flags = FBINFO_FLAG_DEFAULT;

	fb_dev = info->par;
	fb_dev->info = info;
	fb_dev->scr = kzalloc(vsize, GFP_KERNEL);
	if (fb_dev->scr == NULL)
		goto err;

	info->pseudo_palette = fb_dev->pseudo_palette;

	pres = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!pres) {
		dev_err(dev, "HX8347: Cannot access resources\n");
		goto err;
	}
	if (!request_mem_region(pres->start, pres->end - pres->start + 1, pdev->name)) {
		dev_err(dev, "HX8347: mem region request denied\n");
		goto err;
	}

	if (dev->platform_data)
	{
		pdat = (struct hx8347_device_data *)dev->platform_data;
		//fb_dev->dat = pdat->pin_rs;
		//fb_dev->reset = pdat->pin_reset;
		//fb_dev->pwr = pdat->pin_pwr;
		fb_dev->mmio = ioremap(pres->start, pres->end - pres->start + 1);
		if (!fb_dev->mmio) {
			dev_err(dev, "HX8347: Cannot map address\n");
			goto err;
		}
		//fb_dev->mmio = pdat->cs_addr;
		//early_printk("\n Cs addr: %8x", (unsigned int)fb_dev->mmio);
	}
	else
	{
		dev_err(dev, "HX8347: Cannot get default platform data\n");
		goto err;
	}

	/* Initialise HX8347 display */
	hx8347_power(info->par, 0);
	hx8347_delay(10);
	hx8347_power(info->par, 1);
	hx8347_delay(10);
	hx8347_setup(info);						/* Initialize HX8347  */
	hx8347_cls(info, HX8347_BLACK);
	/* Register new frame buffer */
	ret = register_framebuffer(fb_dev->info);
	if (ret) {
		dev_err(dev, "HX8347: Cannot register frame buffer\n");
		goto err;
	}

	dev_info(dev, "fb%d: %s frame buffer device, %dK of video memory\n",
			info->node, info->fix.id, vsize >> 10);
	return 0;

err:
	if (info) framebuffer_release(info);
	if (vmem) vfree(vmem);
	if (fb_dev) {
		if (fb_dev->scr) kfree(fb_dev->scr);
		if (fb_dev->mmio) iounmap(fb_dev->mmio);
	}
	return -1;
}

/**
 * hx8347_remove(). Unregisters and Removes driver module.
 * @param[in]  pdev  - Pointer to a platform device structure
 * @return     int   - 0 = SUCCESS
 */
static int __devexit hx8347_remove(struct platform_device *pdev)
{
	struct fb_info *info = platform_get_drvdata(pdev);
	struct hx8347_par *par;

	if (info) {
		par = (struct hx8347_par*)info->par;
		//at91_set_gpio_value(par->pwr, 0); /* Turn Display power off */

		unregister_framebuffer(info);
		vfree((void __force *)info->screen_base);
		vfree((void __force *)par->scr);
		framebuffer_release(info);
		printk("hx8347_fb: removed\n");
	}
	return 0;
}

#ifdef CONFIG_PM
/**
 * hx8347_suspend(). Called on system syspend (Display off).
 * @param[in]  pdev  - Pointer to a platform device structure
 * @param[in]  msg   - Power management message
 * @return     int   - 0 = SUCCESS
 */
static int hx8347_suspend(struct platform_device *pdev, pm_message_t msg)
{
	struct fb_info *info = platform_get_drvdata(pdev);
	struct hx8347_par *par;

	if (info) {
		printk("hx8347_fb: suspend\n");
		par = (struct hx8347_par*)info->par;
		//hx8347_write_reg(par, 0x05, 0);	/* Turn Display off */
	}
	return 0;
}

/**
 * hx8347_resume(). Called on system resume (Display on).
 * @param[in]  pdev  - Pointer to a platform device structure
 * @return     int   - 0 = SUCCESS
 */
static int hx8347_resume(struct platform_device *pdev)
{
	struct fb_info *info = platform_get_drvdata(pdev);
	struct hx8347_par *par;

	if (info) {
		printk("hx8347_fb: resume\n");
		par = (struct hx8347_par*)info->par;
		//hx8347_write_reg(par, 0x05, 1);	/* Turn Display On */
	}
	return 0;
}
#else
#define hx8347_suspend NULL
#define hx8347_resume NULL
#endif /* CONFIG_PM */

static struct platform_driver hx8347_driver = {
	.remove		= hx8347_remove,
	.suspend	= hx8347_suspend,
	.resume		= hx8347_resume,

	.driver		= {
		.name	= "hx8347_fb",
		.owner	= THIS_MODULE,
	},
};

static int __init hx8347_init(void)
{
	return platform_driver_probe(&hx8347_driver, hx8347_probe);
}

static void __exit hx8347_exit(void)
{
	platform_driver_unregister(&hx8347_driver);
}

module_init(hx8347_init);
module_exit(hx8347_exit);

MODULE_AUTHOR("Olivier Schonken <olivier.schonken@gmail.com>");
MODULE_DESCRIPTION("HX8347 Framebuffer Driver v0.10");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:hx8347_fb");

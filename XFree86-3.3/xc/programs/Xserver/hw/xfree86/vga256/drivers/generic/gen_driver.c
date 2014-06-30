/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/generic/gen_driver.c,v 3.15.2.2 1997/05/09 07:15:40 hohndel Exp $ */
/*
 * Stubs driver Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/* $XConsortium: gen_driver.c /main/11 1996/10/27 11:07:43 kaleb $ */

/*
 * Generic VGA 320x200x256 driver developed from stub driver.
 *
 * Limited to 320x200 physical and virtual resolution, 70 Hz refresh.
 * Should work on any 100% VGA compatible card (SVGA 132 column textmode
 * may break it on some cards).
 * This is a bit of a joke, but it doesn't look too bad, and a good virtual
 * window manager with mouse-push paging like fvwm works nicely. Should be
 * useful for playing Doom.
 *
 * Requires a mode with a 25 MHz clock to be defined. The timing doesn't
 * matter, it is hardcoded in the driver. Example mode line:
 * 
 *  "320x200"     25      320  344  376  400    200  204  206  225
 *
 * In SVGA timing terms, it is actually more of a 12.5 MHz dot clock 320x400
 * mode with each of 200 scanlines displayed twice.
 *
 * It doesn't use the mode timing abstractions used by the other drivers;
 * standard VGA 320x200x256 is a very weird mode. There's not much potential
 * for flexibility in modes anyway since the amount of addressable video
 * memory is limited to 64K. Note that CRTC[9], bit 7 (scanline doubling) is
 * not set, contrary to what one might expect; setting this bit would be
 * useful for supporting very low (200/240) vertical resolution modes in the
 * SVGA drivers (together with a true 12.5 MHz clock).
 *
 * Harm Hanemaayer <hhanemaa@cs.ruu.nl>
 */

#include "X.h"
#include "input.h"
#include "screenint.h"

#include "compiler.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "vga.h"

#ifdef XFreeXDGA 
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif


typedef struct {
	vgaHWRec std;               /* good old IBM VGA */
} vgaGenericRec, *vgaGenericPtr;

static Bool     GenericProbe();
static char *   GenericIdent();
static Bool     GenericClockSelect();
static void     GenericEnterLeave();
static Bool     GenericInit();
static int      GenericValidMode();
static void *   GenericSave();
static void     GenericRestore();
static void     GenericAdjust();

static DisplayModeRec Mode320x200 = {
	&Mode320x200,
	&Mode320x200,
	"320x200",
	0,	/* Clock index */
	320,
	344,
	376,
	400,
	200,
	204,
	206,
	225,
	0,
	0,
	320,
	344,
	376,
	400,
	200,
	204,
	206,
	225,
	FALSE,
	FALSE,
};
	

vgaVideoChipRec GENERIC = {
	/* 
	 * Function pointers
	 */
	GenericProbe,
	GenericIdent,
	GenericEnterLeave,
	GenericInit,
	GenericValidMode,
	GenericSave,
	GenericRestore,
	GenericAdjust,
	vgaHWSaveScreen,
	(void (*)())NoopDDA,
	(void (*)())NoopDDA,
	(void (*)())NoopDDA,	/* No banking. */
	(void (*)())NoopDDA,
	(void (*)())NoopDDA,
	0x10000,	/* 64K VGA window. */
	0x10000,
	16,
	0xFFFF,
	0x00000, 0x10000,
	0x00000, 0x10000,
	TRUE,		/* We have seperate read and write banks in a */
			/* funny sort of way (just one bank). */
	VGA_NO_DIVIDE_VERT,
	{0,},
	8,
	FALSE,
	0,
	0,
	FALSE,
	FALSE,
	FALSE,
	&Mode320x200,
	1,                         /* ChipClockMulFactor */
	1                          /* ChipClockDivFactor */
};

/* These are the fixed 100% VGA compatible CRTC register values used. */
static unsigned char vga320x200x256_CRTC[24] = {
	0x5F,0x4F,0x50,0x82,0x54,0x80,0xBF,0x1F,0x00,0x41,0x00,0x00,
	0x00,0x00,0x00,0x00,0x9C,0x8E,0x8F,0x28,0x40,0x96,0xB9,0xA3
};

#define new ((vgaGenericPtr)vgaNewVideoState)


static char *
GenericIdent(n)
int n;
{
	static char *chipsets[] = {"generic"};

	if (n + 1 > sizeof(chipsets) / sizeof(char *))
		return(NULL);
	else
		return(chipsets[n]);
}


static Bool
GenericClockSelect(no)
int no;
{
	static unsigned char save1, save2;
	unsigned char temp;

	switch(no)
	{
	case CLK_REG_SAVE:
		save1 = inb(0x3CC);
#if 0		
		/* SR2 holds the 'Dot Clock / 2' bit. */
		outb(0x3c4, 0x01);
		save2 = inb(0x3c5);
#endif
		break;
	case CLK_REG_RESTORE:
		outb(0x3C2, save1);
#if 0
		outw(0x3c4, 0x01 + ((unsigned)save2 << 8));
#endif
		break;
	default:
		/* We only support one clock, 25 MHz. */
		/* Always select clock 0 (25 MHz). */
		temp = inb(0x3CC);
		outb(0x3C2, (temp & 0xF3));
#if 0		
		/* Now set bit 3 of SR1 (Divide dot clock by 2). */
		outb(0x3c4, 0x01);
		temp = inb(0x3c5);
		outb(0x3c4, temp | 0x08);
#endif
	}
	return(TRUE);
}


static Bool
GenericProbe()
{
	unsigned char temp1, temp2;
	xf86ClearIOPortList(vga256InfoRec.scrnIndex);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);

	/*
	 * First we attempt to figure out if one of the supported chipsets
	 * is present.
	 */
	if (vga256InfoRec.chipset)
	{
		if (StrCaseCmp(vga256InfoRec.chipset, GenericIdent(0)))
			return (FALSE);
      		else
			GenericEnterLeave(ENTER);
    	}
  	else
	{
		unsigned char temp, origVal, newVal;
		GenericEnterLeave(ENTER);

		/*
		* Check if there is a VGA.  VGA has one more attribute register
		* than EGA, so see if we can read/write it.
		*/
		temp = inb(vgaIOBase + 0x0A); /* reset ATC flip-flop */
		outb(0x3C0, 0x14 | 0x20); origVal = inb(0x3C1);
		outb(0x3C0, origVal ^ 0x0F);
		outb(0x3C0, 0x14 | 0x20); newVal = inb(0x3C1);
		outb(0x3C0, origVal);
		if (newVal != (origVal ^ 0x0F))
		{
			GenericEnterLeave(LEAVE);
			return(FALSE);
		}
	}

  	if (!vga256InfoRec.videoRam)
    	{
      		vga256InfoRec.videoRam = 64;
    	}

  	if (!vga256InfoRec.clocks)
    	{
		vga256InfoRec.clocks = 1;
		vga256InfoRec.clock[0] = 25180;
    	}

	vga256InfoRec.maxClock = 25200;
  	vga256InfoRec.chipset = GenericIdent(0);
  	vga256InfoRec.bankedMono = FALSE;
#ifndef MONOVGA
#ifdef XFreeXDGA 
	vga256InfoRec.directMode = XF86DGADirectPresent;
#endif
#endif

  	return (TRUE);
}


static void 
GenericEnterLeave(enter)
Bool enter;
{
	unsigned char temp;

#ifndef MONOVGA
#ifdef XFreeXDGA 
	if (vga256InfoRec.directMode&XF86DGADirectGraphics && !enter)
		return;
#endif
#endif

  	if (enter)
    	{
		xf86EnableIOPorts(vga256InfoRec.scrnIndex);

      		vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;

      		/* Unprotect CRTC[0-7] */
      		outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
      		outb(vgaIOBase + 5, temp & 0x7F);
    	}
  	else
    	{
      		/* Protect CRTC[0-7] */
      		outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
      		outb(vgaIOBase + 5, (temp & 0x7F) | 0x80);

		xf86DisableIOPorts(vga256InfoRec.scrnIndex);
    	}
}


static void 
GenericRestore(restore)
vgaGenericPtr restore;
{
	vgaProtect(TRUE);
	vgaHWRestore((vgaHWPtr)restore);
	vgaProtect(FALSE);
}


static void *
GenericSave(save)
vgaGenericPtr save;
{
	save = (vgaGenericPtr)vgaHWSave((vgaHWPtr)save, sizeof(vgaGenericRec));

  	return ((void *) save);
}


static Bool
GenericInit(mode)
DisplayModePtr mode;
{
	int i;

	if (!vgaHWInit(mode,sizeof(vgaGenericRec)))
		return(FALSE);

	/* We override all CRTC registers. */
	for (i = 0; i < 24; i++)
		new->std.CRTC[i] = vga320x200x256_CRTC[i];

	/* We only support one clock, 25 MHz, in a funny sort of way. */
	/* Always select clock 0 (25 MHz). */
	new->std.MiscOutReg = (new->std.MiscOutReg & 0x01) | 0x62;
#if 0
	/* Set bit 3 of SR1 (Divide dot clock by 2). */
	new->std.Sequencer[1] |= 0x08;
#endif

	return(TRUE);
}


static void 
GenericAdjust(x, y)
int x, y;
{
	/* This isn't used. The best you would get is about 320x204 */
	/* (which doesn't work). */

	/* In standard VGA 320x200x256 (chain-4), the start address */
	/* is in pixel units. */
	int Base = (y * vga256InfoRec.displayWidth + x);

	outw(vgaIOBase + 4, (Base & 0x00FF00) | 0x0C);
  	outw(vgaIOBase + 4, ((Base & 0x00FF) << 8) | 0x0D);

#ifdef XFreeXDGA
	if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
		/* Wait until vertical retrace is in progress. */
		while (inb(vgaIOBase + 0xA) & 0x08);
		while (!(inb(vgaIOBase + 0xA) & 0x08));
	}
#endif
}

/*
 * GenericValidMode --
 *
 */
static int
GenericValidMode(mode, verbose,flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
return MODE_OK;
}


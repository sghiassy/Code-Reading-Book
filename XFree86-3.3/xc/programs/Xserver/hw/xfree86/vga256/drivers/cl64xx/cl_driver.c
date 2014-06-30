/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cl64xx/cl_driver.c,v 3.14.2.2 1997/05/09 07:15:35 hohndel Exp $ */
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
 *
 * Cirrus Logic GD-64XX Notebook Graphics Controller extensions,
 * Converted from Cirrus Logic GD-6420 only driver.
 * Copyright 1994 by Manfred Brands <mb@oceonics.nl>
 *
 * Setting maxclock based on current display type.
 * Scott S. Bertilson <scott@geom.umn.edu>
 *
 * Various 6440 additions,
 * Copyright 1994 by Randy Hendry <randy@sgi.com>
 * Copyright 1994 by Jeff Kirk <jeff@bambam.dsd.ES.COM>
 */
/* $XConsortium: cl_driver.c /main/11 1996/10/27 11:07:27 kaleb $ */

/*************************************************************************/

/*
 * These are X and server generic header files.
 */
#include "X.h"
#include "input.h"
#include "screenint.h"

/*
 * These are XFree86-specific header files
 */
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

/*
 *	'Supported' chip sets
 */
#define	CL6410		0
#define	CL6412		1
#define	CL6420		2
#define	CL6440		3

int	CL64XXset = -1;

/*
 *	'Supported' display types
 */
#define	CRT		0
#define	LCD		1
#define	SIM		2

int	CL64XXtype = -1;

/*
 * Driver data structures.
 */
typedef struct {
	vgaHWRec std;			/* good old IBM VGA */
	/* 
	 * Extension registers
	 */
	unsigned char HBEX, HREX;	/* Horizontal Extensions */
	unsigned char VOVFL;		/* Vertical Extension */
	unsigned char SADDX;		/* Start Address Extension */
	unsigned char DM, CCLK, WRC;	/* CRTC Extensions */
	unsigned char DMC;          	/* Display Memory Configuration */
	unsigned char CLK, CLKN, CLKD;	/* CLOCK Extensions */
	unsigned char CBC, DELTA;	/* VMC Extensions */
	unsigned char BIUTC;		/* BIU Extensions */
	unsigned char CPURC;		/* Banking Registers */
	unsigned char ATTC;		/* Attribute/Graphics Ext. */
	unsigned char DACPWC;		/* DAC Power Control Register */
	unsigned char ATTLCDC;		/* LCD Attributes */
} vgaCL64XXRec, *vgaCL64XXPtr;

/*
 * Forward definitions for the functions that make up the driver.  See
 * the definitions of these functions for the real scoop.
 */
static Bool	CL64XXProbe();
static char *	CL64XXIdent();
static Bool	CL64XXClockSelect();
static void	CL64XXEnterLeave();
static Bool	CL64XXInit();
static int	CL64XXValidMode();
static void *	CL64XXSave();
static void	CL64XXRestore();
static void	CL64XXAdjust();
/*
 * These are the bank select functions.  There are defined in bank.s
 */
extern	void	CL64XXSetRead();
extern	void	CL64XXSetWrite();
extern	void	CL64XXSetReadWrite();

#define	CL64XX_LOCK_REG		0x0A

/*
 * This data structure defines the driver itself.  The data structure is
 * initialized with the functions that make up the driver and some data 
 * that defines how the driver operates.
 */
vgaVideoChipRec CL64XX = {
	/* 
	 * Function pointers
	 */
	CL64XXProbe,
	CL64XXIdent,
	CL64XXEnterLeave,
	CL64XXInit,
	CL64XXValidMode,
	CL64XXSave,
	CL64XXRestore,
	CL64XXAdjust,
	vgaHWSaveScreen,
	(void (*)())NoopDDA,
	(void (*)())NoopDDA,
	CL64XXSetRead,
	CL64XXSetWrite,
	CL64XXSetReadWrite,
	/*
	 * This is the size of the mapped memory window, usually 64k.
	 */
	0x10000,		
	/*
	 * This is the size of a video memory bank for this chipset.
	 */
	0x08000,
	/*
	 * This is the number of bits by which an address is shifted
	 * right to determine the bank number for that address.
	 */
	15,
	/*
	 * This is the bitmask used to determine the address within a
	 * specific bank.
	 */
	0x7FFF,
	/*
	 * These are the bottom and top addresses for reads inside a
	 * given bank.
	 */
	0x00000, 0x08000,
	/*
	 * And corresponding limits for writes.
	 */
	0x08000, 0x10000,
	/*
	 * Whether this chipset supports a single bank register or
	 * seperate read and write bank registers.
	 */
	TRUE,
	/*
	 * If the chipset requires vertical timing numbers to be divided
	 * by two for interlaced modes, set this to VGA_DIVIDE_VERT.
	 */
	VGA_NO_DIVIDE_VERT, /* ? */
	/*
	 * This is a dummy initialization for the set of option flags
	 * that this driver supports.  It gets filled in properly in the
	 * probe function, if the probe succeeds (assuming the driver
	 * supports any such flags).
	 */
	{0,},
	/*
	 * This determines the multiple to which the virtual width of
	 * the display must be rounded for the 256-color server.
	 */
	8,
	FALSE,
	0,
	0,
	FALSE,
	FALSE,
	FALSE,
	NULL,
	1,      /* ClockMulFactor */
	1       /* ClockDivFactor */
};

/*
 * This is a convenience macro, so that entries in the driver structure
 * can simply be dereferenced with 'new->xxx'.
 */
#define new ((vgaCL64XXPtr)vgaNewVideoState)

/*
 * CL64XXIdent --
 *
 * Returns the string name for supported chipset 'n'.
 */
static char *
CL64XXIdent(n)
int n;
{
	static char *chipsets[] = {
		"cl6410", "cl6412", "cl6420", "cl6440"
	};

	if (n + 1 > sizeof(chipsets) / sizeof(char *))
		return(NULL);
	else
		return(chipsets[n]);
}

/*
 * CL64XXClockSelect --
 * 
 * This function selects the dot-clock with index 'no'.  In most cases
 * this is done by setting the correct bits in various registers (generic
 * VGA uses two bits in the Miscellaneous Output Register to select from
 * 4 clocks).  Care must be taken to protect any other bits in these
 * registers by fetching their values and masking off the other bits.
 *
 * This function returns FALSE if the passed index is invalid or if the
 * clock can't be set for some reason.
 *
 * The CLK register of the CL64[12]X can be programmed to overrule the
 * two bits in the Misc register.  Therefore we don't have to modify that one.
 *
 * Until I open my notebook to find out the real maximum dot clock
 * I'll limit it to the example mentioned in the documentation
 * The CL chip should tell something like:
 *	CL-GD64XX-45QC-A
 *                ||		Maximum Video Clock
 *		    |		Packaging (Q = Plastic Quad Flat Pack)
 *		     |		Temperature Range (C = Commercial)
 *		       |	Revision, B allows 1MB Video RAM
 *
 * If the active display is set to the LCD screen we should only
 * allow a single clock to be used: 16.257 MHz
 * Although the 25.172 MHz clock works, the display quality is worse
 * These clocks should only be allowed in SimulSCAN mode.
 *
 * The CL6440 uses a completely different clocking scheme which resembles
 * that one on the CL54XX series, using a nominator and denominator.
 */
#define	CL6420_MAX_CRT_CLOCK_IN_KHZ	45000
#define	CL6440_3V_MAX_CRT_CLOCK_IN_KHZ	45000
#define	CL6440_5V_MAX_CRT_CLOCK_IN_KHZ	65000
#define	CL64XX_MAX_LCD_CLOCK_IN_KHZ	28332
#define	CL6420_MAX_LCD_CLOCK_IN_KHZ	16257

#define	CL64XX_N_CLOCKS			16
#define	CL64XX_LCD_CLOCK		1
#define	CL64XX_SIM_CLOCK		4

/* 
 * This holds for the CL64[12]0:
 *
 * The generic bits (MISC register) of the clock select are overruled.
 * This allows a linear selection of all 16 clocks, however the first 2 clocks
 * are not anymore 25.172 and 28.332 MHz.  This breaks the vgaGetClocks routine.
 * From the source I assume that this function expects clock index 1 to be
 * 28.332 MHz and relates the found clocks appropriate.  Clock index 1 on the
 * ICS1394-30 clock chip is however 16.257 MHz so that all found clocks are
 * a factor 28.332 / 16.257 too high.
 * Therefore I use one indirection to map logical and physical clocks
 *
 * The clocks are (in MHz):
 *	14.318 16.257 Ext.   36.0   25.172 28.332 24.0   40.0
 *	44.9   50.344 65.028 32.514 56.664 20.000 50.000 80.000
 *
 *
 * This holds for the CL6440:
 *
 * The CRT clock is set as:  OSC x (N + 1) / (D + 1)
 * Where: OSC = 14.31818 MHz
 *			N = register 0xF6 bits 0-6
 *			D = register 0xF7 bits 0-3
 * Just like the CL6420 the MISC register can be overruled for clock setting
 * by setting bit 7 of register 0xF6
 * Bits 4-5 from register 0xF7 determine a clock divide by 1, 2 or 4.
 *
 * Values mentioned in the documentation:
 */
static struct CL6440clock {
	unsigned char	n, d;
} CL6440clocks[] = {
	{ 0x12, 0x08 },		/* 30.227 (~30.250) MHz */
	{ 0x3A, 0x0C },		/* 64.983 (~65.028) MHz */
	{ 0x58, 0x0E },		/* 84.954 (~85.000) MHz */
	{ 0x04, 0x01 },		/* 35.795 (~36.000) MHz */
	{ 0x06, 0x03 },		/* 25.057 (~25.172) MHz */
	{ 0x3A, 0x1E },		/* 28.159 (~28.322) MHz */
	{ 0x12, 0x07 },		/* 34.006 (~34.000) MHz */
	{ 0x42, 0x1B },		/* 39.970 (~40.000) MHz */
	{ 0x18, 0x07 },		/* 44.743 (~44.800) MHz */
	{ 0x06, 0x01 },		/* 50.113 (~50.350) MHz */
	{ 0x0A, 0x04 },		/* 31.500 MHz */
	{ 0x38, 0x1C },		/* 32.491 (~32.500) MHz */
	{ 0x15, 0x04 },		/* 62.999 (~63.000) MHz */
	{ 0x09, 0x01 },		/* 71.590 (~72.000) MHz */
	{ 0x43, 0x0C },		/* 74.895 (~75.000) MHz */
	{ 0x42, 0x0B },		/* 79.943 (~80.000) MHz */
};

static int ClockMapping[] = {
	4, 5, 2, 3, 0, 1, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
};
static Bool
CL64XXClockSelect(no)
int no;
{
	static unsigned char CLK, CLKN, CLKD;
	unsigned char mode;

	switch(no)
	{
	case CLK_REG_SAVE:
		/*
		 * Here all of the registers that can be affected by
		 * clock setting should be saved into static variables.
		 */
		if (CL64XXset == CL6440) {
			outb(0x3CE, 0xF6); CLKN = inb(0x3CF);
			outb(0x3CE, 0xF7); CLKD = inb(0x3CF);
		}
		else {
			outb(0x3CE, 0x84); CLK = inb(0x3CF);
		}
		break;
	case CLK_REG_RESTORE:
		/*
		 * Here all the previously saved registers are restored.
		 */
		if (CL64XXset == CL6440) {
			outb(0x3CE, 0xF6); outb(0x3CF, CLKN);
			outb(0x3CE, 0xF7); outb(0x3CF, CLKD);
		}
		else {
			outb(0x3CE, 0x84); outb(0x3CF, CLK);
		}
		break;
	default:
		if (no < 0 || no >= CL64XX_N_CLOCKS)
			return (FALSE);
		no = ClockMapping[no];
		if (CL64XXset == CL6440) {
			outb(0x3CE, 0xF6); outb(0x3CF, 0x80 | CL6440clocks[no].n);
			outb(0x3CE, 0xF7); outb(0x3CF, CL6440clocks[no].d);
		}
		else {
			outb(0x3CE, 0x84);
			outb(0x3CF, (0x80 | (no << 2)));
		}
	}
	return(TRUE);
}


/*
 * CL64XXProbe --
 *
 * This is the function that makes a yes/no decision about whether or not
 * a chipset supported by this driver is present or not.  The server will
 * call each driver's probe function in sequence, until one returns TRUE
 * or they all fail.
 *
 * Once the chipset has been successfully detected, then the developer needs 
 * to do some other work to find memory, and clocks, etc, and do any other
 * driver-level data-structure initialization may need to be done.
 */
static Bool
CL64XXProbe()
{
	unsigned char	id, revision, bus, display;
	char		*chipset;
	int		i;

	/*
	 * Set up I/O ports to be used by this card.
	 */
	xf86ClearIOPortList(vga256InfoRec.scrnIndex);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);

	/*
	 * First we attempt to figure out if one of the supported chipsets
	 * is present.
	 */
	if (vga256InfoRec.chipset)
	{
		/*
		 * This is the easy case.  The user has specified the
		 * chipset in the Xconfig file.  All we need to do here
		 * is a string comparison against each of the supported
		 * names available from the Ident() function.
		 */
		for (i = 0; (chipset = CL64XXIdent(i)) != NULL; i++) {
			if (!StrCaseCmp(vga256InfoRec.chipset, chipset))
				break;
		}
		if (chipset != NULL) {
			CL64XXset = i;
			CL64XXEnterLeave(ENTER);
		}
		else
			return (FALSE);
	}
	else
	{
		/*
		 * OK.  We have to actually test the hardware.  The
		 * EnterLeave() function (described below) unlocks access
		 * to registers that may be locked, and for OSs that require
		 * it, enables I/O access.  So we do this before we probe,
		 * even though we don't know for sure that this chipset
		 * is present.
		 */
		CL64XXEnterLeave(ENTER);

		/*
		 * If it's a supported card,
		 * we should be able to read back the lock register
		 * If it's not 1 then we're don't know what chip this is.
		 */
		outb(0x3CE, CL64XX_LOCK_REG);
		if (inb(0x3CF) != 0x01)
		{
			CL64XXEnterLeave(LEAVE);
			return(FALSE);
		}

		/*
		 * It might be a supported CHIP SET,
		 * Read the Design/Mask Revision Code Registers
		 * The second nibble is the CHIP SET ID
		 *	 4:	CL-GD6440
		 *	 5:	CL-GD6412
		 *	 7:	CL-GD6420
		 *	 8:	CL-GD6410
		 */
		outb(0x3CE, 0xAA); id = inb(0x3CF);

		revision = (id & 0x0F);
		switch ((id >> 4) & 0x0F) {
		case 4:	/* CL-GD6440 */
			CL64XXset = CL6440;
			outb(0x3CE, 0x99); bus = ((inb(0x3CF) >> 1) & 0x03);
			if (bus == 0)
				ErrorF("CL64XX: Chipset is on Local Bus\n");
			else if (bus == 2)
				ErrorF("CL64XX: Chipset is on ISA Bus\n");
			break;
		case 5:	/* CL-GD6412 */
			CL64XXset = CL6412;
			break;
		case 7:	/* CL-GD6420 */
			CL64XXset = CL6420;
			break;
		case 8:	/* CL-GD6410 */
			CL64XXset = CL6410;
			break;
		default:
			CL64XXEnterLeave(LEAVE);
			return(FALSE);
		}
	}

	/*
	 * If the user has specified the amount of memory in the Xconfig
	 * file, we respect that setting.
	 */
	if (!vga256InfoRec.videoRam)
	{
		/*
		 * Otherwise, do whatever chipset-specific things are 
		 * necessary to figure out how much memory (in kBytes) is 
		 * available.
		 *
		 * This seems to vary from machine to machine and from
		 * BIOS version to BIOS version. 
		 * Someone should contact Cirrus for the definite answer
		 *
		 * Until then just specify the known value in Xconfig.
		 */
		if (CL64XXset == CL6440)
			vga256InfoRec.videoRam = 1024;		/* Only value possible ? */
		else {
#if 0
			/*
			 * My BIOS does somthing like this (Manfred)
			 */
			outb(0x3CE, 0x9A);
			vga256InfoRec.videoRam = (1 << (inb(0x3CF) & 0x7)) * 256;
#else	
			/*
			 * vgadoc3.zip from Finn Thoegersen
			 * It is reported to work on at least one machine,
			 * not on mine though.
			 */
			outb(0x3CE, 0xBB);
			vga256InfoRec.videoRam = (((inb(0x3CF) >> 6) & 0x3) + 1) * 256;
#endif
		}
	}

	/* Find out the Display Type used */
	if (CL64XXset == CL6440) {
		outb(0x3CE, 0xEC);
		switch ((inb(0x3CF) >> 4) & 0x3) {
		case 0:
		case 1:
			CL64XXtype = CRT;
			break;
		case 2:
			CL64XXtype = LCD;
			break;
		case 3:
			CL64XXtype = SIM;
			break;
		}
	}
	else {
		outb(0x3CE, 0x81);
		display = inb(0x3CF);
		switch (display & 0x81) {
		case 0x00:
			CL64XXtype = CRT;
			break;
		case 0x01:
			CL64XXtype = LCD;
			break;
		case 0x80:
		case 0x81:
			CL64XXtype = SIM;
			break;
		}
		if (display & 0x08) {
			ErrorF("CL64XX: CL6340 detected\n");
#ifdef OPTION_MONOCHROME
			OFLG_CLR(OPTION_MONOCHROME, &vga256InfoRec.options);
		}
		else {
			OFLG_SET(OPTION_MONOCHROME, &vga256InfoRec.options);
#endif /* OPTION_MONOCHROME */
		}
	}
	/*
	 * It is recommended that you fill in the maximum allowable dot-clock
	 * rate for your chipset.  If you don't do this, the default of
	 * 90MHz will be used; this is likely too high for many chipsets.
	 * This is specified in KHz, so 90Mhz would be 90000 for this
	 * setting.
	 */
	switch (CL64XXtype) {
		case -1:
			break;
		case CRT:
			if (CL64XXset == CL6440) {
				outb(0x3CE, 0xE8);
				vga256InfoRec.maxClock = ((inb(0x3CF) & 0x08) ?
					CL6440_5V_MAX_CRT_CLOCK_IN_KHZ :
					CL6440_3V_MAX_CRT_CLOCK_IN_KHZ);
			}
			else
				vga256InfoRec.maxClock = CL6420_MAX_CRT_CLOCK_IN_KHZ;
			ErrorF("CL64XX: CRT display only\n");
			break;
		case LCD:
			vga256InfoRec.maxClock = CL64XX_MAX_LCD_CLOCK_IN_KHZ;
			ErrorF("CL64XX: LCD panel display only\n");
			break;
		case SIM:
			vga256InfoRec.maxClock = CL64XX_MAX_LCD_CLOCK_IN_KHZ;
			ErrorF("CL64XX: SimulSCAN operation (LCD + CRT)\n");
			break;
	}

	/*
	 * Again, if the user has specified the clock values in the Xconfig
	 * file, we respect those choices.
	 */
	if (!vga256InfoRec.clocks)
	{
		/*
		 * This utility function will probe for the clock values.
		 * It is passed the number of supported clocks, and a
		 * pointer to the clock-select function.
		 */
		vgaGetClocks(CL64XX_N_CLOCKS, CL64XXClockSelect);
	}

	/*
	 * Last we fill in the remaining data structures.  We specify
	 * the chipset name, using the Ident() function and an appropriate
	 * index.  We set a boolean for whether or not this driver supports
	 * banking for the Monochrome server.  And we set up a list of all
	 * the option flags that this driver can make use of.
	 */
	vga256InfoRec.chipset = CL64XXIdent(CL64XXset);
	vga256InfoRec.bankedMono = TRUE;
#ifndef MONOVGA
#ifdef XFreeXDGA
	vga256InfoRec.directMode = XF86DGADirectPresent;
#endif
#endif

#ifdef OPTION_MONOCHROME
	/*
	 * Initialize option flags known to this driver
	 */
	OFLG_SET(OPTION_MONOCHROME, &CL64XX.ChipOptionFlags);
#endif /* OPTION_MONOCHROME */

	return(TRUE);
}

/*
 * CL64XXEnterLeave --
 *
 * This function is called when the virtual terminal on which the server
 * is running is entered or left, as well as when the server starts up
 * and is shut down.  Its function is to obtain and relinquish I/O 
 * permissions for the SVGA device.  This includes unlocking access to
 * any registers that may be protected on the chipset, and locking those
 * registers again on exit.
 */
static void 
CL64XXEnterLeave(enter)
Bool enter;
{
	static unsigned char save1;
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

		/* 
		 * This is a global.  The CRTC base address depends on
		 * whether the VGA is functioning in color or mono mode.
		 * This is just a convenient place to initialize this
		 * variable.
		 */
		vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;

		/*
		 * Here we deal with register-level access locks.  This
		 * is a generic VGA protection; most SVGA chipsets have
		 * similar register locks for their extended registers
		 * as well.
		 */
		/* Unprotect CRTC[0-7] */
		outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
		outb(vgaIOBase + 5, temp & 0x7F);

		outb(0x3CE, CL64XX_LOCK_REG);
		if ((save1 = inb(0x3CF)) != 0x01)
			outb(0x3CF, 0xEC);
	}
	else
	{
		/*
		 * Here undo what was done above.
		 */

		/* Protect CRTC[0-7] */
		outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
		outb(vgaIOBase + 5, (temp & 0x7F) | 0x80);
		
		if (save1 != 0x01)
		{
			/*
			 * Only lock the extended registers if
			 * we've found them locked on entry
			 */
			outb(0x3CE, CL64XX_LOCK_REG);
			if (save1 == 0x00)
				outb(0x3CF, 0xCE);
			else
				outb(0x3CF, save1);	/* Not a CL64XX ? */
		}

		xf86DisableIOPorts(vga256InfoRec.scrnIndex);
	}
}

/*
 * CL64XXRestore --
 *
 * This function restores a video mode.  It basically writes out all of
 * the registers that have previously been saved in the vgaCL64XXRec data 
 * structure.
 *
 * Note that "Restore" is a little bit incorrect.  This function is also
 * used when the server enters/changes video modes.  The mode definitions 
 * have previously been initialized by the Init() function, below.
 */
static void 
CL64XXRestore(restore)
vgaCL64XXPtr restore;
{
	unsigned char	temp;

	vgaProtect(TRUE);

	/*
	 * Whatever code is needed to get things back to bank zero should be
	 * placed here.  Things should be in the same state as when the
	 * Save/Init was done.
	 */
	if (CL64XXset != CL6410) {
		outw(0x3CE, 0x000E);	/* CPU Base A */
		outw(0x3CE, 0x000F);	/* CPU Base B */
	}

	/*
	 * Code to restore any SVGA registers that have been saved/modified
	 * goes here.  Note that it is allowable, and often correct, to 
	 * only modify certain bits in a register by a read/modify/write cycle.
	 */
	outb(0x3CE, 0x62);
	outb(0x3CF, restore->HBEX);	/* Horizontal Blank End Extension */
	outb(0x3CE, 0x64);
	outb(0x3CF, restore->HREX);	/* Horizontal Retrace End Extension */

	outb(0x3CE, 0x79);
	outb(0x3CF, restore->VOVFL);	/* Vertical Overflow Beyond */
	outb(0x3CE, 0x7C);
	outb(0x3CF, restore->SADDX);	/* Start Address Extension */

	if (CL64XXset == CL6440) {
		/* Display Memory Configuration */
		outb(0x3CE, 0x9A);
		temp = inb(0x3CF);
		outb(0x3CF, (restore->DMC & 0x08) | (temp & ~0x08));  

		/* Display Mode (interlaced bit) */
		outb(0x3CE, 0x80);
		temp = inb(0x3CF);
		outb(0x3CF, (restore->DM & 0x02) | (temp & ~0x02));
	}
	else {
		/* Display Mode (interlaced bit) */
		outb(0x3CE, 0x81);
		temp = inb(0x3CF);
		outb(0x3CF, (restore->DM & 0x04) | (temp & ~0x04));
	}

	outb(0x3CE, 0x82);
	outb(0x3CF, restore->CCLK);	/* Character Clock Control */
	outb(0x3CE, 0x83);
	outb(0x3CF, restore->WRC);	/* Write Control */

	/*
	 * A special case - when using an external clock-setting program,
	 * this function must not change bits associated with the clock
	 * selection.  This condition can be checked by the condition:
	 */
	if (restore->std.NoClock >= 0) {
		/* Clock Select Registers */
		if (CL64XXset == CL6440) {
			outb(0x3CE, 0xF6); outb(0x3CF, restore->CLKN);
			outb(0x3CE, 0xF7); outb(0x3CF, restore->CLKD);
		}
		else {
			outb(0x3CE, 0x84); outb(0x3CF, restore->CLK);
		}
	}

	outb(0x3CE, 0x91);
	outb(0x3CF, 0x20 | restore->CBC);/* Reset FIFO */
	outb(0x3CE, 0x91);
	outb(0x3CF, restore->CBC);	/* Circular Buffer Policy Selection */
	outb(0x3CE, 0x95);
	outb(0x3CF, restore->DELTA);	/* Circular Buffer Delta and Burst */

	outb(0x3CE, 0xA1);
	outb(0x3CF, restore->BIUTC);	/* Bus Interface Unit Tri-state Cntrl */

	if (CL64XXset != CL6410) {
		outb(0x3CE, 0x0D);
		outb(0x3CF, restore->CPURC);	/* CPU Remap Control */
	}

	outb(0x3CE, 0xC0);
	outb(0x3CF, restore->ATTC);	/* Attributes and Graphics Control */

	outb(0x3CE, 0xC8);
	outb(0x3CF, restore->DACPWC);	/* RAMDAC Power Control */

	if (CL64XXset != CL6440) {
		outb(0x3CE, 0xD5);
		outb(0x3CF, restore->ATTLCDC);	/* Attributes LCD Control */
	}

	/*
	 * This function handles restoring the generic VGA registers.
	 */
	vgaHWRestore((vgaHWPtr)restore);
	vgaProtect(FALSE);
}

/*
 * CL64XXSave --
 *
 * This function saves the video state.  It reads all of the SVGA registers
 * into the vgaCL64XXRec data structure.  There is in general no need to
 * mask out bits here - just read the registers.
 */
static void *
CL64XXSave(save)
vgaCL64XXPtr save;
{
	/*
	 * Whatever code is needed to get back to bank zero goes here.
	 */
	if (CL64XXset != CL6410) {
		outw(0x3CE, 0x000E);	/* CPU Base A */
		outw(0x3CE, 0x000F);	/* CPU Base B */
	}

	/*
	 * This function will handle creating the data structure and filling
	 * in the generic VGA portion.
	 */
	save = (vgaCL64XXPtr)vgaHWSave((vgaHWPtr)save, sizeof(vgaCL64XXRec));

	/*
	 * The port I/O code necessary to read in the extended registers 
	 * into the fields of the vgaCL64XXRec structure goes here.
	 */
	outb(0x3CE, 0x62);
	save->HBEX = inb(0x3CF);	/* Horizontal Blank End Extension */
	outb(0x3CE, 0x64);
	save->HREX = inb(0x3CF);	/* Horizontal Retrace End Extension */

	outb(0x3CE, 0x79);
	save->VOVFL = inb(0x3CF);	/* Vertical Overflow Beyond */
	outb(0x3CE, 0x7C);
	save->SADDX = inb(0x3CF);	/* Start Address Extension */

	outb(0x3CE, (CL64XXset == CL6440 ? 0x80 : 0x81));
	save->DM = inb(0x3CF);		/* Display Mode */
	outb(0x3CE, 0x82);
	save->CCLK = inb(0x3CF);	/* Character Clock Control */
	outb(0x3CE, 0x83);
	save->WRC = inb(0x3CF);		/* Write Control */

	/* Clock Select Registers */
	if (CL64XXset == CL6440) {
		outb(0x3CE, 0xF6); save->CLKN = inb(0x3CF);
		outb(0x3CE, 0xF7); save->CLKD = inb(0x3CF);
	}
	else {
		outb(0x3CE, 0x84); save->CLK = inb(0x3CF);
	}

	if (CL64XXset == CL6440) {
		/* Display Memory Configuration */
		outb(0x3CE, 0x9A);
		save->DMC = inb(0x3CF);
	}

	outb(0x3CE, 0x91);
	save->CBC = inb(0x3CF);		/* Circular Buffer Policy Selection */
	outb(0x3CE, 0x95);
	save->DELTA = inb(0x3CF);	/* Circular Buffer Delta and Burst */

	outb(0x3CE, 0xA1);
	save->BIUTC = inb(0x3CF);	/* Bus Interface Unit Tri-state Cntrl */

	if (CL64XXset != CL6410) {
		outb(0x3CE, 0x0D);
		save->CPURC = inb(0x3CF);	/* CPU Remap Control */
	}

	outb(0x3CE, 0xC0);
	save->ATTC = inb(0x3CF);	/* Attributes and Graphics Control */

	outb(0x3CE, 0xC8);
	save->DACPWC = inb(0x3CF);	/* RAMDAC Power Control */

	if (CL64XXset != CL6440) {
		outb(0x3CE, 0xD5);
		save->ATTLCDC = inb(0x3CF);	/* Attributes LCD Control */
	}

	return ((void *) save);
}

/*
 * CL64XXInit --
 *
 * This is the most important function (after the Probe) function.  This
 * function fills in the vgaCL64XXRec with all of the register values needed
 * to enable either a 256-color mode (for the color server) or a 16-color
 * mode (for the monochrome server).
 */
static Bool
CL64XXInit(mode)
DisplayModePtr mode;
{
	int		no;

#if !defined(MONOVGA) && !defined(XF86VGA16)
	/*
	 * The character clock is set to 4 dots for 256 color modes
	 * we adapt them here so that the VGA module will initialise
	 * the CRTC fields correctly
	 */
	if (!mode->CrtcHAdjusted) {
		mode->CrtcHTotal <<= 1;
		mode->CrtcHDisplay <<= 1;
		mode->CrtcHSyncStart <<= 1;
		mode->CrtcHSyncEnd <<= 1;
		mode->CrtcHSkew <<= 1;
		mode->CrtcHAdjusted = TRUE;
	}
#endif

	/*
	 * This will allocate the datastructure and initialize all of the
	 * generic VGA registers.
	 */
	if (!vgaHWInit(mode,sizeof(vgaCL64XXRec)))
		return(FALSE);

	/*
	 * Here all of the other fields of 'new' get filled in, to
	 * handle the SVGA extended registers.  It is also allowable
	 * to override generic registers whenever necessary.
	 */
	/*
	 * The extions bits
	 */
	new->HBEX =
		(new->std.CRTC[3] & 0x1F) |
		((((mode->CrtcHSyncStart >> 3) - 1) & 0x100) >> 1);
	new->HREX =
		(new->std.CRTC[5] & 0x9F) |
		(((mode->CrtcHSyncStart >> 3) & 0x100) >> 2) |
		((((mode->CrtcHTotal >> 3) - 5) & 0x100) >> 3);

	/*
	 * For the vertical values only the extension bits have to be set
	 */
	new->VOVFL = 
		(((mode->CrtcVTotal - 2) & 0x400) >> 10) |
		(((mode->CrtcVDisplay - 1) & 0x400) >> 9) |
		((mode->CrtcVSyncStart & 0x600) >> 7) |
		((mode->CrtcVSyncStart & 0x400) >> 6);

	new->std.CRTC[19] = vga256InfoRec.virtualX >> 3;
	new->std.CRTC[23] = 0xE3;

	new->std.Attribute[16] = 0x01;

	if (mode->Flags & V_INTERLACE)
		new->DM = (CL64XXset == CL6440 ? 2 : 4);
	else
		new->DM = 0;

	new->BIUTC = 0x20;		/* CPU Address Shift Right by 2 */
	new->CCLK = 0x0A;		/* Set character clock to 4 dots */
	if (CL64XXtype != CRT) {
		if (CL64XXset != CL6440)
			new->CCLK |= 0x80;	/* Internal clock inversion */
		new->WRC = 0x01;	/* Protect Vertical Parameters */
	}
	else
		new->WRC = 0x00;	/* Obey passed Vertical parameters */

	/*
	 * Maybe this might be tuned for more speed?
	 *	CBC = 0x0B	->	Enable Burst mode and Full FIFO
	 *	DELTA = (<Burst> << 4) | <Delta>
	 */
#if 0
	new->CBC = 0x0B;		/* FIFO policy */
	new->DELTA = 0x5D;		/* Burst = 5, Delta = 2 */
#else
	new->CBC = 0x03;		/* FIFO policy */
	new->DELTA = 0x04;		/* Burst = 0, Delta = 12 */
#endif

	new->CPURC = 0x05;		/* Two pages of 32kB each */
	new->SADDX = 0x00;		/* Reset Start address */

	if (CL64XXset == CL6440)
		new->DACPWC = 0x04;	/* RAMDAC to 256-color mode */
	else
		new->DACPWC = 0x05;	/* Allow standard access to palette */

	if (CL64XXtype != LCD)
		new->DACPWC |= 0x20;	/* Don't blank RAMDAC */

	new->ATTC = 0x00;		/* Disable fixed fg/bg color mapping */
	if (CL64XXset != CL6440)
		if (CL64XXtype != CRT)
			new->DACPWC |= 0xA0;	/* VDCLK Phase Inversion */

#ifdef OPTION_MONOCHROME
	if (OFLG_ISSET(OPTION_MONOCHROME, &vga256InfoRec.options)) {
		if (CL64XXset != CL6440) {
			new->ATTLCDC = 0x30;	/* Reverse video in Graphics mode */
			if (CL64XXtype != CRT)
				new->ATTLCDC |= 0x80; /* Special color mapping for LCD */
		}
	}
#endif /* OPTION_MONOCHROME */

	if (CL64XXset == CL6440) {
		new->DMC = 0x08;        /* Enable memory above 256kB */
	}

	/*
	 * A special case - when using an external clock-setting program,
	 * this function must not change bits associated with the clock
	 * selection.  This condition can be checked by the condition:
	 */
	if (new->std.NoClock >= 0) {
		if (new->std.NoClock >= CL64XX_N_CLOCKS) {
			ErrorF("CL64XX: Invalid clock, there ain't that many");
			return (FALSE);
		}
		no = ClockMapping[new->std.NoClock];
		if (CL64XXset == CL6440) {
			new->CLKN = 0x80 | CL6440clocks[no].n;
			new->CLKD = CL6440clocks[no].d;
		}
		else {
			/*
			 * If the controller is driving the LCD display only,
			 * limit selection to 16.257 MHz only.
			 * If SimulSCAN (LCD + CRT) is selected
			 * allow only 25.172 MHz
			 */
			if (CL64XXtype == SIM) {
				if (no != CL64XX_SIM_CLOCK) {
					ErrorF("CL64XX: Clock not allowed for LCD display");
					return (FALSE);
				}
			}
			else if (CL64XXtype == LCD) {
				if (no != CL64XX_LCD_CLOCK  &&  no != CL64XX_SIM_CLOCK) {
					ErrorF("CL64XX: Clock not allowed for LCD display");
					return (FALSE);
				}
			}
			new->CLK = (0x80 | (no << 2));
		}
	}

	return (TRUE);
}

/*
 * CL64XXAdjust --
 *
 * This function is used to initialize the SVGA Start Address - the first
 * displayed location in the video memory.  This is used to implement the
 * virtual window.
 */
static void 
CL64XXAdjust(x, y)
int x, y;
{
	/*
	 * The calculation for Base works as follows:
	 *
	 *	(y * virtX) + x ==> the linear starting pixel
	 *
	 * This number is divided by 8 for the monochrome server, because
	 * there are 8 pixels per byte.
	 *
	 * For the color server, it's a bit more complex.  There is 1 pixel
	 * per byte.  In general, the 256-color modes are in word-mode 
	 * (16-bit words).  Word-mode vs byte-mode is will vary based on
	 * the chipset - refer to the chipset databook.  So the pixel address 
	 * must be divided by 2 to get a word address.  In 256-color modes, 
	 * the 4 planes are interleaved (i.e. pixels 0,3,7, etc are adjacent 
	 * on plane 0).  The starting address needs to be as an offset into 
	 * plane 0, so the Base address is divided by 4.
	 *
	 * So:
	 *    Monochrome: Base is divided by 8
	 *    Color:
	 *	if in word mode, Base is divided by 8
	 *	if in byte mode, Base is divided by 4
	 *
	 * The generic VGA only supports 16 bits for the Starting Address.
	 * But this is not enough for the extended memory.  SVGA chipsets
	 * will have additional bits in their extended registers, which
	 * must also be set.
	 */
	int Base = (y * vga256InfoRec.displayWidth + x), msb, tempin;

#ifdef MONOVGA
  	msb = (Base + 3) >> 3;
#else
	msb = (Base + 2) >> 2;
#endif

	/*
	 * These are the generic starting address registers.
	 */
	outw(vgaIOBase + 4, (msb & 0x00FF00) | 0x0C);
  	outw(vgaIOBase + 4, ((msb & 0x0000FF) << 8) | 0x0D);

	/* CL64XX specific bits 18,19,20  ??? */
	outb(0x3CE, 0x7C);
	tempin = inb(0x3CF);
	outb(0x3CF, (tempin & 0xF0) | ((msb >> 16) & 0x0F));

#ifdef XFreeXDGA
	if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
		/* Wait until vertical retrace is in progress. */
		while (inb(vgaIOBase + 0xA) & 0x08);
		while (!(inb(vgaIOBase + 0xA) & 0x08));
	}
#endif
}

/*
 * CL64XXValidMode --
 *
 */
static int
CL64XXValidMode(mode, verbose,flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
return MODE_OK;
}


/* $XConsortium: apm_driver.c /main/5 1996/10/25 10:27:55 kaleb $ */




/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/apm/apm_driver.c,v 3.7.2.2 1997/05/09 07:15:27 hohndel Exp $ */

/*
 * These are X and server generic header files.
 */
#include <math.h>
#include "X.h"
#include "input.h"
#include "screenint.h"
#include "dix.h"

/*
 * These are XFree86-specific header files
 */
#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "vga.h"
#include "vgaPCI.h"

#include "vga256.h"

#include "apm_cursor.h"

extern vgaHWCursorRec vgaHWCursor;

/*
 * Driver data structures.
 */
typedef struct {
	/*
	 * This structure defines all of the register-level information
	 * that must be stored to define a video mode for this chipset.
	 * The 'vgaHWRec' member must be first, and contains all of the
	 * standard VGA register information, as well as saved text and
	 * font data.
	 */
	vgaHWRec std;               /* good old IBM VGA */
  	/* 
	 * Any other registers or other data that the new chipset needs
	 * to be saved should be defined here.  The Init/Save/Restore
	 * functions will manipulate theses fields.  Examples of things
	 * that would go here are registers that contain bank select
	 * registers, or extended clock select bits, or extensions to 
	 * the timing registers.  Use 'unsigned char' as the type for
	 * these registers.
	 */
	unsigned char SR1B;
	unsigned char SR1C;
	unsigned char CR19;
	unsigned char CR1A;
	unsigned char CR1B;
	unsigned char CR1C;
	unsigned char CR1D;
	unsigned char CR1E;
	unsigned char XR80;
	unsigned char XRC0;
	unsigned char XRC6;
	unsigned long XRE8;
	unsigned long XREC;
	unsigned long XRF0;
	unsigned long XRF4;
	unsigned long XR140;
	unsigned short XR144;
	unsigned long XR148;
	unsigned short XR14C;
} vgaApmRec, *vgaApmPtr;


/*
 * Forward definitions for the functions that make up the driver.  See
 * the definitions of these functions for the real scoop.
 */
static Bool     ApmProbe();
static char *   ApmIdent();
static Bool     ApmClockSelect();
static void     ApmEnterLeave();
static Bool     ApmInit();
static int	ApmValidMode();
static void *   ApmSave();
static void     ApmRestore();
static void     ApmAdjust();
static void	ApmFbInit();
/*
 * These are the bank select functions.  There are defined in stub_bank.s
 */
void     ApmSetRead();
void     ApmSetWrite();
void     ApmSetReadWrite();

/*
 * This data structure defines the driver itself.  The data structure is
 * initialized with the functions that make up the driver and some data 
 * that defines how the driver operates.
 */
vgaVideoChipRec APM = {
	/* 
	 * Function pointers
	 */
	ApmProbe,
	ApmIdent,
	ApmEnterLeave,
	ApmInit,
	ApmValidMode,
	ApmSave,
	ApmRestore,
	ApmAdjust,
	vgaHWSaveScreen,
	(void (*)())NoopDDA,
	ApmFbInit,
	ApmSetRead,
	ApmSetWrite,
	ApmSetReadWrite,
	/*
	 * This is the size of the mapped memory window, usually 64k.
	 */
	0x10000,		
	/*
	 * This is the size of a video memory bank for this chipset.
	 */
	0x10000,
	/*
	 * This is the number of bits by which an address is shifted
	 * right to determine the bank number for that address.
	 */
	16,
	/*
	 * This is the bitmask used to determine the address within a
	 * specific bank.
	 */
	0xFFFF,
	/*
	 * These are the bottom and top addresses for reads inside a
	 * given bank.
	 */
	0x00000, 0x10000,
	/*
	 * And corresponding limits for writes.
	 */
	0x00000, 0x10000,
	/*
	 * Whether this chipset supports a single bank register or
	 * separate read and write bank registers.
	 */
	FALSE,
	/*
	 * If the chipset requires vertical timing numbers to be divided
	 * by two for interlaced modes, set this to VGA_DIVIDE_VERT.
	 */
	VGA_DIVIDE_VERT,
	/*
	 * This is a dummy initialization for the set of option flags
	 * that this driver supports.  It gets filled in properly in the
	 * probe function, if the probe succeeds (assuming the driver
	 * supports any such flags).
	 */
	{0,},
	/*
	 * This determines the multiple to which the virtual width of
	 * the display must be rounded for the 256-color server.  This
	 * will normally be 8, but may be 4 or 16 for some servers.
	 */
	8,
	/*
	 * If the driver includes support for a linear-mapped frame buffer
	 * for the detected configuration this should be set to TRUE in the
	 * Probe or FbInit function.  In most cases it should be FALSE.
	 */
	FALSE,
	/*
	 * This is the physical base address of the linear-mapped frame
	 * buffer (when used).  Set it to 0 when not in use.
	 */
	0,
	/*
	 * This is the size  of the linear-mapped frame buffer (when used).
	 * Set it to 0 when not in use.
	 */
	0,
	/*
	 * This is TRUE if the driver has support for 16bpp for the detected
	 * configuration. It must be set in the Probe function.
	 * It most cases it should be FALSE.
	 */
	FALSE,
	/*
	 * This is TRUE if the driver has support for 32bpp for the detected
	 * configuration.
	 */
	FALSE,
	FALSE,
	/*
	 * This is a pointer to a list of builtin driver modes.
	 * This is rarely used, and in must cases, set it to NULL
	 */
	NULL,
	/*
	 * This is a factor that can be used to scale the raw clocks
	 * to pixel clocks.  This is rarely used, and in most cases, set
	 * it to 1.
	 */
	1,       /* ClockMulFactor */
	1        /* ClockDivFactor */
};

/*
 * This is a convenience macro, so that entries in the driver structure
 * can simply be dereferenced with 'new->xxx'.
 */
#define new ((vgaApmPtr)vgaNewVideoState)

/*
 * If your chipset uses non-standard I/O ports, you need to define an
 * array of ports, and an integer containing the array size.  The
 * generic VGA ports are defined in vgaHW.c.
 */
/* This will allow access to all I/O ports */
static unsigned Apm_ExtPorts[] = { 0x400 };
static int Num_Apm_ExtPorts = (sizeof(Apm_ExtPorts)/sizeof(Apm_ExtPorts[0]));

#define VESA	0
#define PCI	1

static int apmChip, apmBus, apmRamdac;
static int apmDacPathWidth, apmMultiplexingThreshold;
static int apmUse8bitColorComponents;
static int apmDisplayableMemory;

#define AP6422  0
#define AT24    1

static SymTabRec chipsets[] = {
	{ AP6422,  "AP6422"},
	{ AT24,    "AT24" },
	{ -1,		"" },
};

static SymTabRec ramdacs[] = {
	{ -1,		"" },
};

static unsigned dflt_clocks[] = {
	25175,  28322,  36000,  40000,  42000,  44000,  44900,  48000,
	50400,  52800,  57270,  58800,  61600,  64000,  65000,  67200,
	70400,  72000,  75000,  77000,  80000,  88000,  90000,  98000,
	100000, 108000, 118000, 120000, 135000,
};
static unsigned num_dflt_clocks = sizeof(dflt_clocks) / sizeof(unsigned);

unsigned long   apm_xbase;

unsigned char
rdxb(addr)
unsigned addr;
{
	wrinx(0x3c4, 0x1d, addr >> 2);
	return inb(apm_xbase + (addr & 3));
}

unsigned short
rdxw(addr)
unsigned addr;
{
	wrinx(0x3c4, 0x1d, addr >> 2);
	return inw(apm_xbase + (addr & 2));
}

unsigned long
rdxl(addr)
unsigned addr;
{
	wrinx(0x3c4, 0x1d, addr >> 2);
	return inl(apm_xbase);
}

void
wrxb(addr, val)
unsigned addr;
unsigned char val;
{
	wrinx(0x3c4, 0x1d, addr >> 2);
	outb(apm_xbase + (addr & 3), val);
}

void
wrxw(addr, val)
unsigned addr;
unsigned short val;
{
	wrinx(0x3c4, 0x1d, addr >> 2);
	outw(apm_xbase + (addr & 2), val);
}

void
wrxl(addr, val)
unsigned addr;
unsigned long val;
{
	wrinx(0x3c4, 0x1d, addr >> 2);
	outl(apm_xbase, val);
}

#define WITHIN(v,c1,c2) (((v) > (c1)) && ((v) < (c2)))
static unsigned
comp_lmn(clock)
	unsigned clock;
{
	int     i, n, m, l, nv, mv;
	float   fout = (float) clock;
	float   fvco, d, r;

	for (l = 3; l >= 0; --l) {
		fvco = fout * pow(2.0, (double) l);
		if (WITHIN(fvco, 125000.0, 250000.0))
			break;
	}
	r = fvco / 14318.0;
	if (l < 0) {
		ErrorF("cannot find postscaler value for Fout = %6.2f\n");
		return 0;
	}
	d = 0.0;
	for (nv = 128; nv > 0; --nv) {
		if (!WITHIN(fvco / nv, 300.0, 30000.0))
			continue;
		mv = (int) rint(nv / r);
		if (!WITHIN(mv, 0, 129))
			continue;
		if (!WITHIN(14318.0 / mv, 300.0, 30000.0))
			continue;
		if (fabs((float) nv / mv - r) > fabs(d - r))
			continue;
		n = nv; m = mv; d = (float) nv / mv;
	}
	if (d == 0.0) {
		ErrorF("cannot find appropriate values for Fout = %6.2f\n");
		return 0;
	}
/*
	ErrorF("%6.2f\t%6.2f\t%d\t%d\t%d\n", fout / 1000.0,
		(n * 14318.0) / (m * pow(2.0, l) * 1000.0),
		n - 1, m - 1, l);
 */
	return ((n - 1) << 16) | ((m - 1) << 8) | (l << 2) | (4 << 4);
}
#undef WITHIN

/*
 * ApmIdent --
 *
 * Returns the string name for supported chipset 'n'.  Most drivers only
 * support one chipset, but multiple version may require that the driver
 * identify them individually (e.g. the Trident driver).  The Ident function
 * should return a string if 'n' is valid, or NULL otherwise.  The
 * server will call this function when listing supported chipsets, with 'n' 
 * incrementing from 0, until the function returns NULL.  The 'Probe'
 * function should call this function to get the string name for a chipset
 * and when comparing against an XF86Config-supplied chipset value.  This
 * cuts down on the number of places errors can creep in.
 */
static char *
ApmIdent(n)
int n;
{
	if (chipsets[n].token < 0)
		return NULL;
	else
		return chipsets[n].name;
}


/*
 * ApmClockSelect --
 * 
 * This function selects the dot-clock with index 'no'.  In most cases
 * this is done my setting the correct bits in various registers (generic
 * VGA uses two bits in the Miscellaneous Output Register to select from
 * 4 clocks).  Care must be taken to protect any other bits in these
 * registers by fetching their values and masking off the other bits.
 *
 * This function returns FALSE if the passed index is invalid or if the
 * clock can't be set for some reason.
 */
static Bool
ApmClockSelect(no)
int no;
{
	static unsigned char save1;
	static unsigned long save2, temp1;
	unsigned char temp;

	switch(no)
	{
	case CLK_REG_SAVE:
		/*
		 * Here all of the registers that can be affected by
		 * clock setting should be saved into static variables.
		 */
		save1 = inb(0x3CC);
		save2 = rdxl(0xec);
		break;
	case CLK_REG_RESTORE:
		/*
		 * Here all the previously saved registers are restored.
		 */
		outb(0x3C2, save1);
		wrxl(0xec, save2);
		break;
	default:
		/* 
	 	 * These are the generic two low-order bits of the clock select 
		 */
		if (no < 2) {
			temp = inb(0x3CC);
			outb(0x3C2, ( temp & 0xF3) | ((no << 2) & 0x0C));
		} else {
			temp1 = comp_lmn(vga256InfoRec.clock[no]);
			if (!temp1)
				return FALSE;
			temp = inb(0x3CC);
			outb(0x3C2, ( temp & 0xF3) | 0x0C);
			wrxl(0xec, temp1);
		}
	}
	return(TRUE);
}


/*
 * ApmProbe --
 *
 * This is the function that makes a yes/no decision about whether or not
 * a chipset supported by this driver is present or not.  The server will
 * call each driver's probe function in sequence, until one returns TRUE
 * or they all fail.
 *
 */
static Bool
ApmProbe()
{
	int maxclock8bpp, maxclock16bpp, maxclock32bpp;

	/*
	 * Set up I/O ports to be used by this card.  Only do the second
	 * xf86AddIOPorts() if there are non-standard ports for this
	 * chipset.
	 */
	xf86ClearIOPortList(vga256InfoRec.scrnIndex);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, 
		       Num_Apm_ExtPorts, Apm_ExtPorts);

	/*
	 * First we attempt to figure out if one of the supported chipsets
	 * is present.
	 */
	if (vga256InfoRec.chipset)
	{
		/*
		 * This is the easy case.  The user has specified the
		 * chipset in the XF86Config file.  All we need to do here
		 * is a string comparison against each of the supported
		 * names available from the Ident() function.  If this
		 * driver supports more than one chipset, there would be
		 * nested conditionals here (see the Trident and WD drivers
		 * for examples).
		 */
		apmChip = xf86StringToToken(chipsets, vga256InfoRec.chipset);
		if (apmChip >= 0)
			ApmEnterLeave(ENTER);
		else
			return FALSE;
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
		int     i;
		char    idstring[] = "Pro642";
		ApmEnterLeave(ENTER);
		for (i = 0; idstring[i]; ++i)
			if (rdinx(0x3c4, 0x11 + i) != idstring[i]) {
				ApmEnterLeave(LEAVE);
				return(FALSE);
			}
		switch (rdinx(0x3c4, 0x11 +i)) {
		case '0':
		case '2':
			apmChip = AP6422;
			break;
		case '4':
			apmChip = AT24;
			break;
		default:
			ApmEnterLeave(LEAVE);
			return(FALSE);
		}
		vga256InfoRec.chipset = ApmIdent(apmChip);
    	}


	/*
	 * If the user has specified the amount of memory in the XF86Config
	 * file, we respect that setting.
	 */
  	if (!vga256InfoRec.videoRam)
    	{
		/*
		 * Otherwise, do whatever chipset-specific things are 
		 * necessary to figure out how much memory (in kBytes) is 
		 * available.
		 */
		vga256InfoRec.videoRam = rdinx(0x3c4, 0x20) * 64;
    	}

	if (rdxb(0xca) & 1)
		apmBus = PCI;
	else
		apmBus = VESA;

	/*
	 * Again, if the user has specified the clock values in the XF86Config
	 * file, we respect those choices.
	 */
  	if (!vga256InfoRec.clocks)
    	{
		/*
		 * This utility function will probe for the clock values.
		 * It is passed the number of supported clocks, and a
		 * pointer to the clock-select function.
		 */
		/* vgaGetClocks(2, ApmClockSelect); */
		int     i;
		for (i = 0; i < num_dflt_clocks; ++i)
			vga256InfoRec.clock[i] = dflt_clocks[i];
		vga256InfoRec.clocks = num_dflt_clocks;
    	}

	vga256InfoRec.maxClock = 135000;

	/* Use linear addressing by default. */
	if (!OFLG_ISSET(OPTION_NOLINEAR_MODE, &vga256InfoRec.options)) {
		APM.ChipUseLinearAddressing = TRUE;
		if (vga256InfoRec.MemBase != 0)
			APM.ChipLinearBase =
				vga256InfoRec.MemBase;
		else
			if (apmBus == PCI)
				APM.ChipLinearBase = rdxb(0x193) << 24;
			else
				/* VESA local bus. */
				/* Pray that 2048MB works. */
				APM.ChipLinearBase = 0x80000000;
		APM.ChipLinearSize = vga256InfoRec.videoRam * 1024;
		APM.ChipHas16bpp = TRUE;
		APM.ChipHas32bpp = TRUE;
	}

	/*
	 * Last we fill in the remaining data structures.  We specify
	 * the chipset name, using the Ident() function and an appropriate
	 * index.  We set a boolean for whether or not this driver supports
	 * banking for the Monochrome server.  And we set up a list of all
	 * the option flags that this driver can make use of.
	 */
  	vga256InfoRec.bankedMono = FALSE;
 	OFLG_SET(OPTION_NOLINEAR_MODE, &APM.ChipOptionFlags);
  	return TRUE;
}

/*
 * ApmFbInit --
 *      enable speedups for the chips that support it
 */
static void
ApmFbInit()
{
	int offscreen_available;

	if (xf86Verbose && APM.ChipUseLinearAddressing)
		ErrorF("%s %s: %s: Using linear framebuffer at 0x%08X (%s)\n",
			XCONFIG_PROBED, vga256InfoRec.name,
			vga256InfoRec.chipset, APM.ChipLinearBase,
			apmBus == PCI ? "PCI bus" : "VL bus");

	apmDisplayableMemory = vga256InfoRec.displayWidth
		* vga256InfoRec.virtualY
		* (vgaBitsPerPixel / 8);
	offscreen_available = vga256InfoRec.videoRam * 1024 -
		apmDisplayableMemory;

	if (xf86Verbose)
		ErrorF("%s %s: %s: %d bytes off-screen video memory available\n",
			XCONFIG_PROBED, vga256InfoRec.name,
			vga256InfoRec.chipset, offscreen_available);
    
    	/*
    	 * Currently, the hardware cursor is not supported at 8bpp
    	 * due to a framebuffer code architecture issue.
    	 */
#ifdef HW_CURSOR
	if (!OFLG_ISSET(OPTION_SW_CURSOR, &vga256InfoRec.options)
	&& vgaBitsPerPixel != 8) {
		if (offscreen_available < 1024)
			ErrorF("%s %s: %s: Not enough off-screen video memory for hardware cursor\n",
				XCONFIG_PROBED, vga256InfoRec.name,
				vga256InfoRec.chipset);
		else {
			/*
			 * OK, there's at least 1024 bytes available
			 * at the end of video memory to store
			 * the cursor image, so we can use the hardware
			 * cursor.
			 */
			apmCursorWidth = 64;
			apmCursorHeight = 64;
			vgaHWCursor.Initialized = TRUE;
			vgaHWCursor.Init = ApmCursorInit;
			vgaHWCursor.Restore = ApmRestoreCursor;
			vgaHWCursor.Warp = ApmWarpCursor;
			vgaHWCursor.QueryBestSize = ApmQueryBestSize;
			if (xf86Verbose)
				ErrorF("%s %s: %s: Using hardware cursor\n",
					XCONFIG_PROBED, vga256InfoRec.name,
					vga256InfoRec.chipset);
		}
	}
#endif

}


/*
 * ApmEnterLeave --
 *
 * This function is called when the virtual terminal on which the server
 * is running is entered or left, as well as when the server starts up
 * and is shut down.  Its function is to obtain and relinquish I/O 
 * permissions for the SVGA device.  This includes unlocking access to
 * any registers that may be protected on the chipset, and locking those
 * registers again on exit.
 */
static void 
ApmEnterLeave(enter)
Bool enter;
{
	/*
	 * The value of the lock register is saved at the first
	 * "Enter" call, restored at a "Leave". This reduces the
	 * risk of messing up the registers of another chipset.
	 */
	static int enterCalled = FALSE;
	static int savedSR10;
	unsigned char temp;

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
      		if (enterCalled == FALSE) {
			savedSR10 = rdinx(0x3C4, 0x10);
			apm_xbase = (rdinx(0x3c4, 0x1f) << 8)
				| rdinx(0x3c4, 0x1e);
      			enterCalled = TRUE;
      		}
		outw(0x3C4, 0x1210);
    	}
  	else
    	{
		/*
		 * Here undo what was done above.
		 */

		/* Protect CRTC[0-7] */
		outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
		outb(vgaIOBase + 5, (temp & 0x7F) | 0x80);

		wrinx(0x3C4, 0x10, savedSR10);

		xf86DisableIOPorts(vga256InfoRec.scrnIndex);
	}
}

/*
 * ApmRestore --
 *
 * This function restores a video mode.  It basically writes out all of
 * the registers that have previously been saved in the vgaApmRec data 
 * structure.
 *
 * Note that "Restore" is a little bit incorrect.  This function is also
 * used when the server enters/changes video modes.  The mode definitions 
 * have previously been initialized by the Init() function, below.
 */
static void 
ApmRestore(restore)
vgaApmPtr restore;
{
	vgaProtect(TRUE);

	/*
	 * Whatever code is needed to get things back to bank zero should be
	 * placed here.  Things should be in the same state as when the
	 * Save/Init was done.
	 */

	/* Set aperture index to 0. */
	wrxw(0xC0, 0);

	/*
	 * Write the extended registers first
	 */
	wrinx(0x3C4, 0x1b, restore->SR1B);
	wrinx(0x3C4, 0x1c, restore->SR1C);

	/* Hardware cursor registers. */
	wrxl(0x140, restore->XR140);
	wrxw(0x144, restore->XR144);
	wrxl(0x148, restore->XR148);
	wrxw(0x14C, restore->XR14C);

	wrinx(vgaIOBase + 4, 0x19, restore->CR19);
	wrinx(vgaIOBase + 4, 0x1a, restore->CR1A);
	wrinx(vgaIOBase + 4, 0x1b, restore->CR1B);
	wrinx(vgaIOBase + 4, 0x1c, restore->CR1C);
	wrinx(vgaIOBase + 4, 0x1d, restore->CR1D);
	wrinx(vgaIOBase + 4, 0x1e, restore->CR1E);

	/* RAMDAC registers. */
	wrxl(0xec, restore->XREC);

	wrxb(0x80, restore->XR80);
	wrxb(0xc6, restore->XRC6);

	/*
	 * This function handles restoring the generic VGA registers.
	 */
	vgaHWRestore((vgaHWPtr)restore);

	vgaProtect(FALSE);
}

/*
 * ApmSave --
 *
 * This function saves the video state.  It reads all of the SVGA registers
 * into the vgaApmRec data structure.  There is in general no need to
 * mask out bits here - just read the registers.
 */
static void *
ApmSave(save)
vgaApmPtr save;
{
	/*
	 * Whatever code is needed to get back to bank zero goes here.
	 */

	/* Set aperture index to 0. */
	wrxw(0xC0, 0);

	/*
	 * This function will handle creating the data structure and filling
	 * in the generic VGA portion.
	 */
	save = (vgaApmPtr)vgaHWSave((vgaHWPtr)save, sizeof(vgaApmRec));

	save->SR1B = rdinx(0x3C4, 0x1b);
	save->SR1C = rdinx(0x3C4, 0x1c);

	/* Hardware cursor registers. */
	save->XR140 = rdxl(0x140);
	save->XR144 = rdxw(0x144);
	save->XR148 = rdxl(0x148);
	save->XR14C = rdxw(0x14C);

	save->CR19 = rdinx(vgaIOBase + 4,  0x19);
	save->CR1A = rdinx(vgaIOBase + 4,  0x1A);
	save->CR1B = rdinx(vgaIOBase + 4,  0x1B);
	save->CR1C = rdinx(vgaIOBase + 4,  0x1C);
	save->CR1D = rdinx(vgaIOBase + 4,  0x1D);
	save->CR1E = rdinx(vgaIOBase + 4,  0x1E);

	/* RAMDAC registers. */
	save->XREC = rdxl(0xec);

	save->XR80 = rdxb(0x80);
	save->XRC6 = rdxb(0xc6);

  	return ((void *) save);
}

/*
 * ApmInit --
 *
 * This is the most important function (after the Probe) function.  This
 * function fills in the vgaApmRec with all of the register values needed
 * to enable either a 256-color mode (for the color server) or a 16-color
 * mode (for the monochrome server).
 *
 * The 'mode' parameter describes the video mode.  The 'mode' structure 
 * as well as the 'vga256InfoRec' structure can be dereferenced for
 * information that is needed to initialize the mode.  The 'new' macro
 * (see definition above) is used to simply fill in the structure.
 */
static Bool
ApmInit(mode)
DisplayModePtr mode;
{
	/*
	 * This will allocate the datastructure and initialize all of the
	 * generic VGA registers.
	 */
	if (!vgaHWInit(mode,sizeof(vgaApmRec)))
		return(FALSE);

	/*
	 * Here all of the other fields of 'new' get filled in, to
	 * handle the SVGA extended registers.  It is also allowable
	 * to override generic registers whenever necessary.
	 *
	 */

	/*
	 * The APM chips have a scale factor of 8 for the 
	 * scanline offset. There are four extended bit in addition
	 * to the 8 VGA bits.
	 */
	{
		int offset;
		offset = (vga256InfoRec.displayWidth *
			vga256InfoRec.bitsPerPixel / 8)	>> 3;
		new->std.CRTC[0x13] = offset;
		/* Bit 8 resides at CR1C bits 7:4. */
		new->CR1C = (offset & 0xf00) >> 4;
	}

	/* Set pixel depth. */
	if (vga256InfoRec.bitsPerPixel == 8) {
		new->XR80 = 0x02;
		new->XRC6 = 0x08;
	}
	if (vga256InfoRec.bitsPerPixel == 16) {
		new->XR80 = 0x0d;
		new->XRC6 = 0x03;
	}
	if (vga256InfoRec.bitsPerPixel == 32) {
		new->XR80 = 0x0f;
		new->XRC6 = vga256InfoRec.displayWidth >= 800 ? 0x0c : 0x04;
	}

	/*
	 * Enable VESA Super VGA memory organisation.
	 * Also enable Linear Addressing.
	 */
	if (APM.ChipUseLinearAddressing) {
		new->SR1B = 0x24;
		new->SR1C = 0x2d;
	}
	else {
		/* Banking; Map aperture at 0xA0000. */
		new->SR1B = 0;
		new->SR1C = 0;
	}
	/* Set banking register to zero. */
	new->XRC0 = 0;

	/* Handle the CRTC overflow bits. */
	{
		unsigned char val;
		/* Vertical Overflow. */
		val = 0;
		if ((mode->CrtcVTotal - 2) & 0x400)
			val |= 0x01;
		if ((mode->CrtcVDisplay - 1) & 0x400)
			val |= 0x02;
		/* VBlankStart is equal to VSyncStart + 1. */
		if (mode->CrtcVSyncStart & 0x400)
			val |= 0x04;
		/* VRetraceStart is equal to VSyncStart + 1. */
		if (mode->CrtcVSyncStart & 0x400)
			val |= 0x08;
		new->CR1A = val;

		/* Horizontal Overflow. */
		val = 0;
		if ((mode->CrtcHTotal / 8 - 5) & 0x100)
			val |= 1;
		if ((mode->CrtcHDisplay / 8 - 1) & 0x100)
			val |= 2;
		/* HBlankStart is equal to HSyncStart - 1. */
		if ((mode->CrtcHSyncStart / 8 - 1) & 0x100)
			val |= 4;
		/* HRetraceStart is equal to HSyncStart. */
		if ((mode->CrtcHSyncStart / 8) & 0x100)
			val |= 8;
		new->CR1B = val;
	}
	new->CR1E = 1;          /* disable autoreset feature */
	/*
	 * A special case - when using an external clock-setting program,
	 * this function must not change bits associated with the clock
	 * selection.  This condition can be checked by the condition:
	 *
	 *	if (new->std.NoClock >= 0)
	 *		initialize clock-select bits.
	 */

	if (new->std.NoClock >= 0) {
		/* Program clock select. */
		if (new->std.NoClock < 2) {
			new->std.MiscOutReg &= ~0xC;
			new->std.MiscOutReg |= (new->std.NoClock & 3) << 2;
		} else {
			new->XREC = comp_lmn(vga256InfoRec.clock[new->std.NoClock]);
			if (!new->XREC)
				return FALSE;
			new->std.MiscOutReg |= 0xc;
		}
	}

	/* Set up the RAMDAC registers. */

	if (vgaBitsPerPixel > 8)
		/* Get rid of white border. */
		new->std.Attribute[0x11] = 0x00;

	/*
	 * Hardware cursor registers.
	 * Generally the SVGA server will take care of enabling the
	 * cursor after a mode switch.
	 */

	return TRUE;
}

/*
 * ApmAdjust --
 *
 * This function is used to initialize the SVGA Start Address - the first
 * displayed location in the video memory.  This is used to implement the
 * virtual window.
 */
static void 
ApmAdjust(x, y)
int x, y;
{
	int Base = ((y * vga256InfoRec.displayWidth + x)
		* (vgaBitsPerPixel / 8)) >> 2;

	/*
	 * These are the generic starting address registers.
	 */
	outw(vgaIOBase + 4, (Base & 0x00FF00) | 0x0C);
  	outw(vgaIOBase + 4, ((Base & 0x00FF) << 8) | 0x0D);

	/*
	 * Here the high-order bits are masked and shifted, and put into
	 * the appropriate extended registers.
	 */
	modinx(vgaIOBase + 4, 0x1c, 0x0f, (Base & 0x0f0000) >> 16);
}

/*
 * ApmValidMode --
 *
 */
static int
ApmValidMode(mode, verbose, flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
	/* Check for CRTC timing bits overflow. */
	if (mode->VTotal > 2047) {
	    if (verbose)
		ErrorF("%s %s: %s: Vertical mode timing overflow (%d)\n",
			XCONFIG_PROBED, vga256InfoRec.name,
			vga256InfoRec.chipset, mode->VTotal);
	    return MODE_BAD;
	}

	return MODE_OK;
}

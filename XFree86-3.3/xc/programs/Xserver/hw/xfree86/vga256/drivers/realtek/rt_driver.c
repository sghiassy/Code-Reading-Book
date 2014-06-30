/*
 * This is the RTVGA Driver for the realtek Chipset(s) RTG3106 (and maybe
 * others RTG3103, RTG3105 )
 * 
 * Copyright 1995 by Peter Trattler <peter@sbox.tu-graz.ac.at>
 * Copy Policy: as in the XFree86 copyright
 * 
 * I did not have a lot of the hardware info of the RealTek-SVGA chip, so
 * most of this code was developed by trial and error. (and by looking at
 * the ROM-Bios routines)
 */

/*
 * 
 * STUB-Driver Copyright 1993 by David Wexelblat <dwex@XFree86.org>
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
 * DAVID WEXELBLAT AND PETER TRATTLER DISCLAIMS ALL WARRANTIES WITH REGARD 
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS, IN NO EVENT SHALL DAVID WEXELBLAT AND PETER TRATTLER BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/realtek/rt_driver.c,v 3.9.2.2 1997/05/09 07:15:46 hohndel Exp $ */

/*************************************************************************/

/*
 * This is a stub SVGA driver for XFree86.  To make an actual driver,
 * all instances of 'STUB' below should be replaced with the chipset
 * name (e.g. 'ET4000').
 *
 * This one file can be used for both the color, VGA16 and monochrome servers.
 * Remember that the monochrome server is actually using a 16-color mode,
 * with only one bitplane active.  To distinguish between the two at
 * compile-time, use '#ifdef MONOVGA', etc.  For the VGA16 server, use
 * '#ifdef XF86VGA16'
 */
/* $XConsortium: rt_driver.c /main/9 1996/10/27 11:08:33 kaleb $ */

/*************************************************************************/

#ifndef PSZ
/* If i do not use the following command I will get division by zero
 * errors in #if ... in file cfbmskbits.h
 */
#  define PSZ 8
#endif

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

/*
 * If the driver makes use of XF86Config 'Option' flags, the following will be
 * required
 */
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

#ifdef XFreeXDGA 
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

/*
 * In many cases, this is sufficient for VGA16 support when VGA2 support is
 * already done
 */

#ifdef XF86VGA16
#define MONOVGA
#endif

#if 0
/*
 * This header is required for drivers that implement RTVGAFbInit().
 */
#if !defined(MONOVGA) && !defined(XF86VGA16)
#include "vga256.h"
#endif
#endif

/* #define DEBUG 1 */

/* Following is defined if linear 128kB Mode is used for 16bpp-modes.
 * Currently (XFree 3.1.1) in 16bpp no paged-mode is possible :-(
 */								 
/* #define USE_LINEAR_16BPP_MODE 1 */ /* Does *not* work -- crashes computer */

/*
 * Driver data structures.
 */
#ifdef DEBUG
typedef struct {
  unsigned char seq [0x10];
  unsigned char gdc [0x10];
  unsigned char crtc [0x1b];
  unsigned char hidden3c2;
} rtk_mode;
#endif

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
#ifdef DEBUG   
   	rtk_mode realtek;
#endif   
   	int GDCx0B, GDCx0C, SepRWBanks, CRTCx19, CRTCx1A, CRTCx1E;
} vgaRTVGARec, *vgaRTVGAPtr;

/*
 * Forward definitions for the functions that make up the driver.  See
 * the definitions of these functions for the real scoop.
 */
static Bool     RTVGAProbe();
static char *   RTVGAIdent();
static Bool     RTVGAClockSelect();
static void     RTVGAEnterLeave();
static Bool     RTVGAInit();
static int	RTVGAValidMode();
static void *   RTVGASave();
static void     RTVGARestore();
static void     RTVGAAdjust();
/*
static void     RTVGASaveScreen();
static void     RTVGAGetMode();
static void	RTVGAFbInit();
 */
/*
 * These are the bank select functions.  There are defined in RTVGA_bank.s
 */
extern void     RTVGASetRead();
extern void     RTVGASetWrite();
extern void     RTVGASetReadWrite();

/*
 * This data structure defines the driver itself.  The data structure is
 * initialized with the functions that make up the driver and some data 
 * that defines how the driver operates.
 */
vgaVideoChipRec REALTEK = {
	/* 
	 * Function pointers
	 */
	RTVGAProbe,
	RTVGAIdent,
	RTVGAEnterLeave,
	RTVGAInit,
   	RTVGAValidMode,
	RTVGASave,
	RTVGARestore,
	RTVGAAdjust,
	vgaHWSaveScreen      /* RTVGASaveScreen*/,
	(void (*)()) NoopDDA /* RTVGAGetMode */,
	(void (*)()) NoopDDA /* RTVGAFbInit */,
	RTVGASetRead,
	RTVGASetWrite,
	RTVGASetReadWrite,
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
	 * seperate read and write bank registers.  Almost all chipsets
	 * support two banks, and two banks are almost always faster
	 * (Trident 8900C and 9000 are odd exceptions).
	 */
	TRUE,
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
	 * for the detected configuratio this should be set to TRUE in the
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
	FALSE, /* sorry this will only with linear addressing */
	/*
	 * This is TRUE if the driver has support for 24bpp for the detected
	 * configuration.
	 */
	FALSE,
	/*
	 * This is TRUE if the driver has support for 32bpp for the detected
	 * configuration.
	 */
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
#define new ((vgaRTVGAPtr)vgaNewVideoState)

/*
 * If your chipset uses non-standard I/O ports, you need to define an
 * array of ports, and an integer containing the array size.  The
 * generic VGA ports are defined in vgaHW.c.
 */
static unsigned RTVGA_ExtPorts[] = { 0x3D6, 0x3D7 };
static int Num_RTVGA_ExtPorts =
	(sizeof(RTVGA_ExtPorts)/sizeof(RTVGA_ExtPorts[0]));


/*
 * RTVGAIdent --
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
RTVGAIdent(n)
int n;
{
	static char *chipsets[] = {"realtek"};

	if (n + 1 > sizeof(chipsets) / sizeof(char *))
		return(NULL);
	else
		return(chipsets[n]);
}

/*
 * RTVGAClockSelect --
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
/* I figuerd out following clocks for the Realtek-SVGA Chip:
 * To do so I have set the 800x600x256 Mode and mesaured the V-Sync using the
 * program 'speed' within the subdirectory .../rtvga/state. After that I
 * could calculate the CLK's for all other modes, when the GDC-Register
 * 0x0b and 0x0c are set to the specified values. The Timer value is used
 * for the two bits in the 0x3c2 (respectively 0x3cc) register.
 * 
 * GDC 0x0b GDC 0x0c	Timer	Vsync	CLK (calc)	CLK (guessed)
 *	02	14	0	36,4	25,8		25
 *			1	40,6	28,8		28,322
 *			2	32,5	23		22
 *R			3	51,7	36		36
 * 	02	34	0	53,19	37
 * 			1	46,72	32,5
 * 			2	60,6	42,1
 * 			3	57,47	39,9
 * 	04	14	0	71,9	50		50,35
 * 			1	81,3	56,5
 * 			2	64,5	44,8		44,9
 * 			3	103,09	71,6		72
 *H 	04	34	0	105	73
 * 			1	93	64
 *H 			2	121	84
 *H 			3	114	79
 * Following modes i did not use in RTVGAClockSelect.
 * 	01	14	0	48	33
 * 			1	54	37
 * 			2	42	29
 * 			3	69	48
 *N 	01	34	0	71,4	49,6
 *N 			1	63	43,8
 *N 			2	82	57
 *N 			3	76	52
 * 
 * R Reference mode (800x600x256) where Clk is specified to be 36Mhz (all
 *   other clk-values are calculated with respect to this mode)
 * N Noise on the screen -- so don't use this mode
 * H this Clock speed is too high for my RTVGA-Card (Probably the RAM's are
 *   too slow)
 */

static Bool
RTVGAClockSelect(no)
int no;
{
	static unsigned char save1, save2, save3;
	unsigned char temp;
#ifdef DEBUG
   ErrorF ("RTVGAClockSelect (%d)\n", no);
#endif
   
	switch(no)
	{
	case CLK_REG_SAVE:
		/*
		 * Here all of the registers that can be affected by
		 * clock setting should be saved into static variables.
		 */
		save1 = inb(0x3CC);
		/* Any extended registers would go here */
	   	outb(0x3CE, 0x0C);
		save2 = inb (0x3CF);
	   	outb(0x3CE, 0x0B);
	   	save3 = inb (0x3CF);
		break;
	case CLK_REG_RESTORE:
		/*
		 * Here all the previously saved registers are restored.
		 */
		outb(0x3C2, save1);
		/* Any extended registers would go here */
	        outb(0x3CE, 0x0C);
	   	outb(0x3CF, save2);
	   	outb(0x3CE, 0x0B);
	   	outb(0x3CF, save3);
		break;
	default:
		/* 
	 	 * These are the generic two low-order bits of the clock select 
		 */
		temp = inb(0x3CC);
		outb(0x3C2, ( temp & 0xF3) | ((no << 2) & 0x0C));
		/* 
	 	 * Here is where the high order bit(s) supported by the chipset 
	 	 * are set.  This is done by fetching the appropriate register,
	 	 * masking off bits that won't be changing, then shifting and
	 	 * masking 'no' to set the bits as appropriate.
	 	 */
		/* I did no masking as I did not know what bits to mask :-( */
	   	outb(0x3CE, 0x0C);
		outb(0x3CF, (no & 0x04) ? 0x34 : 0x14);
	   	outb(0x3CE, 0x0B);
		outb(0x3CF, (no & 0x08) ? 0x04 : 0x02);
	}
	return(TRUE);
}


/*
 * RTVGAProbe --
 *
 * This is the function that makes a yes/no decision about whether or not
 * a chipset supported by this driver is present or not.  The server will
 * call each driver's probe function in sequence, until one returns TRUE
 * or they all fail.
 *
 * Pretty much any mechanism can be used to determine the presence of the
 * chipset.  If there is a BIOS signature (e.g. ATI, GVGA), it can be read
 * via /dev/mem on most OSs, but some OSs (e.g. Mach) require special
 * handling, and others (e.g. Amoeba) don't allow reading  the BIOS at
 * all.  Hence, this mechanism is discouraged, if other mechanisms can be
 * found.  If the BIOS-reading mechanism must be used, examine the ATI and
 * GVGA drivers for the special code that is needed.  Note that the BIOS 
 * base should not be assumed to be at 0xC0000 (although most are).  Use
 * 'vga256InfoRec.BIOSbase', which will pick up any changes the user may
 * have specified in the XF86Config file.
 *
 * The preferred mechanism for doing this is via register identification.
 * It is important not only the chipset is detected, but also to
 * ensure that other chipsets will not be falsely detected by the probe
 * (this is difficult, but something that the developer should strive for).  
 * For testing registers, there are a set of utility functions in the 
 * "compiler.h" header file.  A good place to find example probing code is
 * in the SuperProbe program, which uses algorithms from the "vgadoc2.zip"
 * package (available on most PC/vga FTP mirror sites, like ftp.uu.net and
 * wuarchive.wustl.edu).
 *
 * Once the chipset has been successfully detected, then the developer needs 
 * to do some other work to find memory, and clocks, etc, and do any other
 * driver-level data-structure initialization may need to be done.
 */

static Bool
RTVGAProbe()
{
#ifdef DEBUG
   ErrorF ("RTVGAProbe\n");
#endif   
	/*
	 * Set up I/O ports to be used by this card.  Only do the second
	 * xf86AddIOPorts() if there are non-standard ports for this
	 * chipset.
	 */
	xf86ClearIOPortList(vga256InfoRec.scrnIndex);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, 
		       Num_RTVGA_ExtPorts, RTVGA_ExtPorts);

	/*
	 * First we attempt to figure out if one of the supported chipsets
	 * is present.
	 */
	if (vga256InfoRec.chipset)
	{
		/*
		 * This is the easy case.  The user has specified the
		 * chipset in the XF86config file.  All we need to do here
		 * is a string comparison against each of the supported
		 * names available from the Ident() function.  If this
		 * driver supports more than one chipset, there would be
		 * nested conditionals here (see the Trident and WD drivers
		 * for examples).
		 */
		if (StrCaseCmp(vga256InfoRec.chipset, RTVGAIdent(0)))
			return (FALSE);
      		else
			RTVGAEnterLeave(ENTER);
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
		RTVGAEnterLeave(ENTER);

		/*
		 * Here is where all of the probing code should be placed.  
		 * The best advice is to look at what the other drivers are 
		 * doing.  If you are lucky, the chipset reference will tell 
		 * how to do this.  Other resources include SuperProbe/vgadoc2,
		 * and the Ferraro book.
		 */

	   	/* Following test comes from vgadoc3.zip */
#ifdef DEBUG
	   ErrorF ("Testing hardware vgaIOBase = 0x%x\n", vgaIOBase);
	   ErrorF ("testinx2 () = %d\n", testinx2 (vgaIOBase+4, 0x1f, 0x3f));
	   ErrorF ("tstrg (0x3d6) = %d\n", testrg (0x3d6, 0x0f));
	   ErrorF ("tstrg (0x3d7) = %d\n", testrg (0x3d7, 0x0f));
#endif	   
      		if (!(testinx2 (vgaIOBase+4, 0x1f, 0x3f) && testrg (0x3d6, 0x0f)
		      && testrg (0x3d7, 0x0f)))
		{
			/*
			 * Turn things back off if the probe is going to fail.
			 * Returning FALSE implies failure, and the server
			 * will go on to the next driver.
			 */
	  		RTVGAEnterLeave(LEAVE);
	  		return(FALSE);
		}
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
	   static short int rtg3103mem [] = { 256, 512, 768, 1024 };
	   static short int rtgmem [] = { 256, 512, 1024, 2048 };

#ifdef DEBUG
	   ErrorF ("Checking Ramsize\n");
	   ErrorF ("rdinx (., 0x1a) = 0x%x\n", rdinx (vgaIOBase+4, 0x1a));
	   ErrorF ("rdinx (, 0x1e) = 0x%x\n", rdinx (vgaIOBase+4, 0x1e));
#endif	   
	   if ((rdinx (vgaIOBase+4, 0x1A) >> 6) == 0)    /* RTG3103 */
	     vga256InfoRec.videoRam =
	           rtg3103mem [rdinx (vgaIOBase+4, 0x1e) & 0x03];
	   else			       /* RTG31030 RTG3105 RTG3106 */
	     vga256InfoRec.videoRam =
	           rtgmem [rdinx (vgaIOBase+4, 0x1e) & 0x03];
    	}

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
      		vgaGetClocks(16 /* NUM */, RTVGAClockSelect);
    	}

	/*
	 * It is recommended that you fill in the maximum allowable dot-clock
	 * rate for your chipset.  If you don't do this, the default of
	 * 90MHz will be used; this is likely too high for many chipsets.
	 * This is specified in KHz, so 90Mhz would be 90000 for this
	 * setting.
	 */
	vga256InfoRec.maxClock = 72000;

	/*
	 * Last we fill in the remaining data structures.  We specify
	 * the chipset name, using the Ident() function and an appropriate
	 * index.  We set a boolean for whether or not this driver supports
	 * banking for the Monochrome server.  And we set up a list of all
	 * the option flags that this driver can make use of.
	 */
  	vga256InfoRec.chipset = RTVGAIdent(0);
  	vga256InfoRec.bankedMono = FALSE;
/*	OFLG_SET(OPTION_FLG1, &RTVGA.ChipOptionFlags); */
#ifdef USE_LINEAR_16BPP_MODE
	if (vgaBitsPerPixel == 16) {
	   REALTEK.ChipUseLinearAddressing = TRUE;
	   REALTEK.ChipLinearBase = 0xa0000000;
	   REALTEK.ChipLinearSize = 128 * 1024; /* standard vga maximum
						 * linear addressable
						 * size
						 */
	   REALTEK.ChipHas16bpp = TRUE;
	}
#endif
	if (vgaBitsPerPixel == 16)
	  REALTEK.ChipClockMulFactor *= 2;
	if (vga256InfoRec.videoRam < 1024)
	  REALTEK.ChipClockMulFactor *= 2;
#ifndef MONOVGA
#ifdef XFreeXDGA 
	vga256InfoRec.directMode = XF86DGADirectPresent;
#endif
#endif

  	return(TRUE);
}

/*
 * RTVGAEnterLeave --
 *
 * This function is called when the virtual terminal on which the server
 * is running is entered or left, as well as when the server starts up
 * and is shut down.  Its function is to obtain and relinquish I/O 
 * permissions for the SVGA device.  This includes unlocking access to
 * any registers that may be protected on the chipset, and locking those
 * registers again on exit.
 */
static void 
RTVGAEnterLeave(enter)
Bool enter;
{
	unsigned char temp;
#ifdef DEBUG
   ErrorF ("RTVGAEnter (%d)\n", enter);
#endif   

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
	   /*
      		outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
      		outb(vgaIOBase + 5, temp & 0x7F);
	    */
    	}
  	else
    	{
      		/* Protect CRTC[0-7] */
	   /*
      		outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
      		outb(vgaIOBase + 5, (temp & 0x7F) | 0x80);
	    */
		/*
		 * Here undo what was done above.
		 */
	   	outb (0x3D6, 0);
	   	outb (0x3D7, 0);
		xf86DisableIOPorts(vga256InfoRec.scrnIndex);
    	}
}

/*
 * RTVGARestore --
 *
 * This function restores a video mode.  It basically writes out all of
 * the registers that have previously been saved in the vgaRTVGARec data 
 * structure.
 *
 * Note that "Restore" is a little bit incorrect.  This function is also
 * used when the server enters/changes video modes.  The mode definitions 
 * have previously been initialized by the Init() function, below.
 */

#ifdef DEBUG

#define outportb(adr,dat) outb(adr,dat)
#define inportb(adr) inb(adr)

static void setpart (const unsigned char *m, int size, int port)
{
  int i;

  for (i=0; i<size; i++) {
    outportb (port, i);
    outportb (port+1, m[i]);
  }
}

static void rtk_restore_mode (const rtk_mode *s)
{
  int save, i;
  /* first the sequencer */
  setpart (s->seq, sizeof (s->seq) / sizeof (unsigned char), 0x3c4); 
  /* then the graphics ed char), 0x3c4); */
  /* then the graphics controller */
  setpart (s->gdc, sizeof (s->gdc) / sizeof (unsigned char), 0x3ce); 
  /* then the crtc */
  outportb (0x3d4, 0x11);
  outportb (0x3d5, inportb (0x3d5) & 0x7f); /* release lock */
  outportb (0x3d4, 0x1e);
  save = inportb (0x3d5);
  outportb (0x3d5, save | 0x80); /* must be also a lock */
  outportb (0x3c2, s->hidden3c2);
  setpart (s->crtc, sizeof (s->crtc) / sizeof (unsigned char), 0x3d4); 
  outportb (0x3d4, 0x1e);
  outportb (0x3d5, save);
}

static void getpart (unsigned char *m, int size, int port)
{
  int i;

  for (i=0; i<size; i++) {
    outportb (port, i);
    m[i] = inportb (port+1);
  }
}

static void rtk_save_mode (rtk_mode *s)
{
  /* first the sequencer */
  getpart (s->seq, sizeof (s->seq) / sizeof (unsigned char), 0x3c4);
  /* then the graphics controller */
  getpart (s->gdc, sizeof (s->gdc) / sizeof (unsigned char), 0x3ce);
  /* then the crtc */
  getpart (s->crtc, sizeof (s->crtc) / sizeof (unsigned char), vgaIOBase+4);
  s->hidden3c2 = inportb (0x3cc);
}

static void print_rtk_mode (rtk_mode *p)
{
   int i;
   ErrorF ("     00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17\n");
   ErrorF ("SEQ  ");
   for (i=0; i<sizeof (p->seq) / sizeof (p->seq[0]); i++)
     ErrorF ("%02x ", p->seq[i]);
   ErrorF ("\nGDC  ");
   for (i=0; i<sizeof (p->gdc) / sizeof (p->gdc[0]); i++)
     ErrorF ("%02x ", p->gdc[i]);
   ErrorF ("\nCRTC ");
   for (i=0; i<sizeof (p->crtc) / sizeof (p->crtc[0]); i++)
     ErrorF ("%02x ", p->crtc[i]);
   ErrorF ("\nPort 3c2 %02x\n", p->hidden3c2);
}

#endif /* DEBUG */

static void 
RTVGARestore(restore)
vgaRTVGAPtr restore;
{
   	int save, i, j, tmp;
#ifdef DEBUG
   ErrorF ("RTVGARestore\n");
   print_rtk_mode (&restore->realtek);
#endif   
	vgaProtect(TRUE);

	/*
	 * Whatever code is needed to get things back to bank zero should be
	 * placed here.  Things should be in the same state as when the
	 * Save/Init was done.
	 */
   	outb (0x3D6, 0);
   	outb (0x3D7, 0);

	/*
	 * This function handles restoring the generic VGA registers.
	 */
	vgaHWRestore((vgaHWPtr)restore);

	/*
	 * Code to restore any SVGA registers that have been saved/modified
	 * goes here.  Note that it is allowable, and often correct, to 
	 * only modify certain bits in a register by a read/modify/write cycle.
	 *
	 * A special case - when using an external clock-setting program,
	 * this function must not change bits associated with the clock
	 * selection.  This condition can be checked by the condition:
	 *
	 *	if (restore->std.NoClock >= 0)
	 *		restore clock-select bits.
	 */
   	outb (0x3CE, 0x0B);
   	outb (0x3CF, restore->GDCx0B);
   	outb (0x3CE, 0x0C);
   	outb (0x3CF, restore->GDCx0C);
   	outb (0x3CE, 0x0F);
	tmp = inb (0x3CF);
   	if (restore->SepRWBanks) outb (0x3CF, tmp | 0x04);
   	else outb (0x3CF, tmp & ~0x04);
   	outb (vgaIOBase+4, 0x19);
   	outb (vgaIOBase+5, 0x22); /* must be a lock as well */
   	outb (vgaIOBase+4, 0x11);
	tmp = inb (vgaIOBase+5);
   	outb (vgaIOBase+5, tmp & ~0x80); /* standard VGA-Lock */
   	outb (vgaIOBase+4, 0x1E);
	save = inb (vgaIOBase+5);
   	outb (vgaIOBase+5, save | 0x80); /* this must be a lock ??? */
   
   	outb (0x3C2, restore->std.MiscOutReg);
   	/* restore the normal CRTC-Registers vgaHWRestore seems to have
	 * problems with them (maybe because of the locks)
	 */
#ifdef DEBUG   
	ErrorF ("CRTC ");
#endif   
	for (i=0; i<0x19; i++) {
	   outb (vgaIOBase+4, i);
	   outb (vgaIOBase+5, restore->std.CRTC [i]);
#ifdef DEBUG	   
	ErrorF ("%02x ", restore->std.CRTC[i]);
#endif	   
	   if (restore->std.CRTC[i] != inb (vgaIOBase+5))
	     ErrorF ("(%d) ", inb (vgaIOBase+5));
	}
   	outb (vgaIOBase+4, 0x1A);
   	outb (vgaIOBase+5, restore->CRTCx1A);
#ifdef DEBUG   
	ErrorF ("%02x ", restore->CRTCx1A);
#endif   
   	outb (vgaIOBase+4, 0x19);
   	outb (vgaIOBase+5, restore->CRTCx19);
   	outb (vgaIOBase+4, 0x1E);
   	outb (vgaIOBase+5, restore->CRTCx1E);
        /* Now select HiColor DAC or normal DAC */
        if (restore->CRTCx1E == 0x0A)
          for (i=0; i<4; i++) outb (0x3C8, 0); /* HiColor-Mode */
        else
          for (i=0; i<4; i++) outb (0x3C7, 0); /* Normal-Mode */
#ifdef DEBUG   
	outb (vgaIOBase+4, 0x19);
	ErrorF ("%02x", inb (vgaIOBase+5));
	outb (vgaIOBase+4, 0x1A);
	ErrorF (" %02x\n", inb (vgaIOBase+5));
#endif
   	outb (vgaIOBase+4, 0x1E);
   	outb (vgaIOBase+5, save);      /* lock registers again */

   	vgaHWRestore((vgaHWPtr)restore); /* quick-hack: Now restore the font-data */

	vgaProtect(FALSE);
}

/*
 * RTVGASave --
 *
 * This function saves the video state.  It reads all of the SVGA registers
 * into the vgaRTVGARec data structure.  There is in general no need to
 * mask out bits here - just read the registers.
 */
static void *
RTVGASave(save)
vgaRTVGAPtr save;
{
#ifdef DEBUG
   rtk_mode saved;
   ErrorF ("RTVGASave\n");
#endif
	/*
	 * Whatever code is needed to get back to bank zero goes here.
	 */
#ifdef DEBUG
   rtk_save_mode (&saved); /* save before vgaHWSave messes up regs */
   print_rtk_mode (&saved);
#endif   
   	outb (0x3D6, 0);
   	outb (0x3D7, 0);

	/*
	 * This function will handle creating the data structure and filling
	 * in the generic VGA portion.
	 */
	save = (vgaRTVGAPtr)vgaHWSave((vgaHWPtr)save, sizeof(vgaRTVGARec));

	/*
	 * The port I/O code necessary to read in the extended registers 
	 * into the fields of the vgaRTVGARec structure goes here.
	 */
#ifdef DEBUG   
   	save->realtek = saved;
#endif   
   	outb (0x3CE, 0x0B);
   	save->GDCx0B = inb (0x3CF);
   	outb (0x3CE, 0x0C);
   	save->GDCx0C = inb (0x3CF);
   	outb (0x3CE, 0x0F);
   	save->SepRWBanks = (inb (0x3CF) & 0x04) != 0;
   	outb (vgaIOBase+4, 0x19);
   	save->CRTCx19 = inb (vgaIOBase+5);
   	outb (vgaIOBase+4, 0x1A);
   	save->CRTCx1A = inb (vgaIOBase+5);
   	outb (vgaIOBase+4, 0x1E);
   	save->CRTCx1E = inb (vgaIOBase+5);

  	return ((void *) save);
}

/*
 * RTVGAInit --
 *
 * This is the most important function (after the Probe) function.  This
 * function fills in the vgaRTVGARec with all of the register values needed
 * to enable either a 256-color mode (for the color server) or a 16-color
 * mode (for the monochrome server).
 *
 * The 'mode' parameter describes the video mode.  The 'mode' structure 
 * as well as the 'vga256InfoRec' structure can be dereferenced for
 * information that is needed to initialize the mode.  The 'new' macro
 * (see definition above) is used to simply fill in the structure.
 */
static Bool
RTVGAInit(mode)
DisplayModePtr mode;
{
   	int isSlowMode = 0, hiColorMode = 0;
#ifdef DEBUG
 static rtk_mode m800x600x256 = {
 { 0x03, 0x01, 0x0f, 0x00, 0x0e, 0x00, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
 { 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0f, 0xff, 0x00, 0x00,
   0x02, 0x14, 0x00, 0x00, /*0x00 Enable Sep R/W Port :*/ 0x04 },
 { 0x7a, 0x63, 0x64, 0x9d, 0x69, 0x97, 0x6f, 0xf0, 0x00, 0x60, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x59, 0x8b, 0x57, 0x32, 0x40, 0x59, 0x6f, 0xa3,
   0xff, 0x22 },
   0xef
 };

   int i;
   ErrorF ("RTVGAInit\n");
#endif   
	/*
	 * This will allocate the datastructure and initialize all of the
	 * generic VGA registers.
	 */
	if (vga256InfoRec.videoRam < 1024) {
	   /* poor slow vga with only 512kB */
	   mode->CrtcHDisplay <<= 1;
	   mode->CrtcHSyncStart <<= 1;
	   mode->CrtcHSyncEnd <<= 1;
	   mode->CrtcHTotal <<= 1;
	   mode->CrtcHSkew <<= 1;
	   isSlowMode = 1;
	}
        if (vgaBitsPerPixel == 16) {
	   mode->CrtcHDisplay <<= 1;
	   mode->CrtcHSyncStart <<= 1;
	   mode->CrtcHSyncEnd <<= 1;
	   mode->CrtcHTotal <<= 1;
	   mode->CrtcHSkew <<= 1;
	   hiColorMode = 1;
	}
	if (!vgaHWInit(mode,sizeof(vgaRTVGARec)))
		return(FALSE);
	if (isSlowMode) {
	   mode->CrtcHDisplay >>= 1;
	   mode->CrtcHSyncStart >>= 1;
	   mode->CrtcHSyncEnd >>= 1;
	   mode->CrtcHTotal >>= 1;
	   mode->CrtcHSkew >>= 1;
	}
	if (hiColorMode) {
	   mode->CrtcHDisplay >>= 1;
	   mode->CrtcHSyncStart >>= 1;
	   mode->CrtcHSyncEnd >>= 1;
	   mode->CrtcHTotal >>= 1;
	   mode->CrtcHSkew >>= 1;
#ifdef USE_LINEAR_16BPP_MODE
	   new->std.Graphics [6] &= ~0x0c; /* Reset bit 2-3: Map 128k Mode */
#endif	   
	}
#ifdef DEBUG
   ErrorF ("VGAIn00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17\n");
   ErrorF ("SEQ  ");
   for (i=0; i<5;i++)
     ErrorF ("%02x ", new->std.Sequencer[i]);
   ErrorF ("\nGDC  ");
   for (i=0; i<9; i++)
     ErrorF ("%02x ", new->std.Graphics[i]);
   ErrorF ("\nCRTC ");
   for (i=0; i<25; i++)
     ErrorF ("%02x ", new->std.CRTC[i]);
   ErrorF ("\nPort 3c2 %02x  ", new->std.MiscOutReg);
   ErrorF ("Clock %d\n", new->std.NoClock);
   new->realtek = m800x600x256;
#endif   

	/*
	 * Here all of the other fields of 'new' get filled in, to
	 * handle the SVGA extended registers.  It is also allowable
	 * to override generic registers whenever necessary.
	 *
	 * A special case - when using an external clock-setting program,
	 * this function must not change bits associated with the clock
	 * selection.  This condition can be checked by the condition:
	 *
	 *	if (new->std.NoClock >= 0)
	 *		initialize clock-select bits.
	 */
   	/* Set the clock */
	new->GDCx0B = (new->std.NoClock & 0x08) ? 0x04 : 0x02;
	new->GDCx0C = ((new->std.NoClock & 0x04) ? 0x24 : 0x04)
		      | (isSlowMode ? 0x00 : 0x10);
   	if (vga256InfoRec.clock [mode->Clock] > 68000)
     	  new->GDCx0C |= 0x40; /* This bit has to be set if the clock is
				* very high -- otherwise flicker may result
				* on the screen
				*/
   	/* Now the CRTC */
/*   	new->std.CRTC [0x02] = 0x64; */
/*   	new->std.CRTC [0x03] = 0x9d; */
/*   	new->std.CRTC [0x11] = 0x8b; */
	new->std.CRTC [0x14] = 0x40;
/*   	new->std.CRTC [0x16] = 0x6f; */
/*   	new->std.CRTC [0x17] = 0xa3; */
   	/* Enable memory below 512kB and set the interlace flag */
	new->CRTCx19 = 0x22;
	if (mode->Flags & V_INTERLACE) {
	   new->std.CRTC [0x13] *= 2;
	   new->CRTCx19 |= 0x01;
	}
   	if (isSlowMode) new->std.CRTC [0x13] *= 2;
   	new->CRTCx1A = 0x00;
        if (hiColorMode) new->CRTCx1E = 0x0A;
        else new->CRTCx1E = 0x02;
   	new->SepRWBanks = 1;
   
#ifdef DEBUG   
   ErrorF ("1CRTC");
   for (i=0; i<25; i++)
     ErrorF ("%02x ", new->std.CRTC[i]);
   ErrorF ("\n");
#endif   

	return(TRUE);
}

/*
 * RTVGAAdjust --
 *
 * This function is used to initialize the SVGA Start Address - the first
 * displayed location in the video memory.  This is used to implement the
 * virtual window.
 */
static void 
RTVGAAdjust(x, y)
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
	 * on plane 0). The starting address needs to be as an offset into 
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
	int Base = (y * vga256InfoRec.displayWidth + x) >> 3;
   /* xxx changed above from virtualX to displayWidth */
#ifdef DEBUG
/*   ErrorF ("RTVGAAdjust %d,%d\n", x, y); */
#endif   

	/*
	 * These are the generic starting address registers.
	 */
	outw(vgaIOBase + 4, (Base & 0x00FF00) | 0x0C);
  	outw(vgaIOBase + 4, ((Base & 0x00FF) << 8) | 0x0D);
  	outw(vgaIOBase + 4, ((Base & 0x030000) >> 8) | 0x33);

#ifdef XFreeXDGA
	if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
		/* Wait until vertical retrace is in progress. */
		while (inb(vgaIOBase + 0xA) & 0x08);
		while (!(inb(vgaIOBase + 0xA) & 0x08));
	}
#endif
}

#if 0

/*
 * RTVGASaveScreen --
 *
 * This function gets called before and after a synchronous reset is
 * performed on the SVGA chipset during a mode-changing operation.  Some
 * chipsets will reset registers that should not be changed during this.
 * If your function is one of these, then you can use this function to
 * save and restore the registers.
 *
 * Most chipsets do not require this function, and instead put
 * 'vgaHWSaveScreen' in the vgaVideoChipRec structure.
 */
static void
RTVGASaveScreen(mode)
int mode;
{
	if (mode == SS_START)
	{
		/*
		 * Save an registers that will be destroyed by the reset
		 * into static variables.
		 */

		/*
		 * Start sequencer reset.
		 */
		outw(0x3c4, 0x0100);
	}
	else
	{
		/*
		 * End sequencer reset.
		 */
		outw(0x3c4, 0x0300);

		/*
		 * Now restore those registers.
		 */
	}
}

/*
 * RTVGAGetMode --
 *
 * This function will read the current SVGA register settings and produce
 * a filled-in DisplayModeRec containing the current mode.
 *
 * Note that the is function is NOT used in XFree86 1.3, hence in a real
 * driver you should put 'NoopDDA' in the vgaVideoChipRec structure.  At
 * some point in the future, this function will be used to implement
 * interactive mode setting, and drivers will be required to supply it.
 */
static void
RTVGAGetMode(mode)
DisplayModePtr mode;
{
	/*
	 * Fill in the 'mode' stucture based on current register settings.
	 */
}

/*
 * RTVGAFbInit --
 *
 * This function is used to initialise chip-specific graphics functions.
 * It can be used to make use of the accelerated features of some chipsets.
 * For most drivers, this function is not required, and 'NoopDDA' is put
 * in the vgaVideoChipRec structure.
 */
static void
RTVGAFbInit()
{

	/*
	 * Fill in the fields of cfbLowlevFuncs for which there are
	 * accelerated versions.  This struct is defined in
	 * xc/programs/Xserver/hw/xfree86/vga256/cfb.banked/cfbfuncs.h.
	 */
	cfbLowlevFuncs.fillRectSolidCopy = RTVGAFillRectSolidCopy;
	cfbLowlevFuncs.doBitbltCopy = RTVGADoBitbltCopy;

	/*
	 * Some functions (eg, line drawing) are initialised via the
	 * cfbTEOps, cfbTEOps1Rect, cfbNonTEOps, cfbNonTEOps1Rect
	 * structs as well as in cfbLowlevFuncs.  These are of type
	 * 'struct GCFuncs' which is defined in mit/server/include/gcstruct.h.
	 */
	cfbLowlevFuncs.lineSS = RTVGALineSS;
	cfbTEOps1Rect.Polylines = RTVGALineSS;
	cfbTEOps.Polylines = RTVGALineSS;
	cfbNonTEOps1Rect.Polylines = RTVGALineSS;
	cfbNonTEOps.Polylines = RTVGALineSS;

	/*
	 * If hardware cursor is supported, the vgaHWCursor struct should
	 * be filled in here.
	 */
	vgaHWCursor.Initialized = TRUE;
	vgaHWCursor.Init = RTVGACursorInit;
	vgaHWCursor.Restore = RTVGACursorRestore;
	vgaHWCursor.Warp = RTVGACursorWarp;
	vgaHWCursor.QueryBestSize = RTVGAQueryBestSize;
	
}

#endif

static int
RTVGAValidMode(mode, verbose,flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
	/*
	 * Code to check if a mode is suitable for the selected chipset.
	 * In most cases this can just return TRUE.
	 */

	return(MODE_OK);
}

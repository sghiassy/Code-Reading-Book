/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/mx/mx_driver.c,v 3.18.2.2 1997/05/09 07:15:42 hohndel Exp $ */
/*
 *
 * Driver Stubs Copyright 1993 by David Wexelblat <dwex@goblin.org>
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
/* $XConsortium: mx_driver.c /main/13 1996/10/28 04:24:52 kaleb $ */


/*************************************************************************/

/*
 * This file is part of the mxic SVGA driver for XFree86. (MX68000, MX68010)
 *
 * Built from XFree86 stub file
 * 29/04/94 Frank Dikker (dikker@cs.utwente.nl)
 * 
 * Specific card settings are taken from Finn Thoegersen's VGADOC package.
 *
 * 04/12/94 bug with VC switching fixed
 */

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
	unsigned char mxa;             /* register 65h */
	unsigned char mxb;             /* register A7h */
	unsigned char mxc;             /* register C3h */
	unsigned char mxd;             /* register C5h */
	unsigned char mxe;             /* register F0h */
	unsigned char mxf;             /* register F1h */
	unsigned char mxg;             /* register F3h */
} vgaMXRec, *vgaMXPtr;

/*
 * Forward definitions for the functions that make up the driver.  See
 * the definitions of these functions for the real scoop.
 */
static Bool     MXProbe();
static char *   MXIdent();
static Bool     MXClockSelect();
static void     MXEnterLeave();
static Bool     MXInit();
static int      MXValidMode();
static void *   MXSave();
static void     MXRestore();
static void     MXAdjust();
/*
 * These are the bank select functions.  There are defined in mx_bank.s
 */
extern void     MXSetRead();
extern void     MXSetWrite();
extern void     MXSetReadWrite();

/*
 * This data structure defines the driver itself.  The data structure is
 * initialized with the functions that make up the driver and some data 
 * that defines how the driver operates.
 */
vgaVideoChipRec MX = {
	/* 
	 * Function pointers
	 */
	MXProbe,
	MXIdent,
	MXEnterLeave,
	MXInit,
	MXValidMode,
	MXSave,
	MXRestore,
	MXAdjust,
	vgaHWSaveScreen,
	(void (*)())NoopDDA,
	(void (*)())NoopDDA,
	MXSetRead,
	MXSetWrite,
	MXSetReadWrite,
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
#define new ((vgaMXPtr)vgaNewVideoState)

/*
 * If your chipset uses non-standard I/O ports, you need to define an
 * array of ports, and an integer containing the array size.  The
 * generic VGA ports are defined in vgaHW.c.
 */
static unsigned MX_ExtPorts[] = { 0x3c4,0x3c5 };
static int Num_MX_ExtPorts =
	(sizeof(MX_ExtPorts)/sizeof(MX_ExtPorts[0]));


/*
 * MXIdent --
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
MXIdent(n)
int n;
{
	static char *chipsets[] = {"mx"};

	if (n + 1 > sizeof(chipsets) / sizeof(char *))
		return(NULL);
	else
		return(chipsets[n]);
}

/*
 * MXClockSelect --
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
MXClockSelect(no)
int no;
{
	static unsigned char save1,save2;
	unsigned char temp;

	switch(no)
	{
	case CLK_REG_SAVE:
		/*
		 * Here all of the registers that can be affected by
		 * clock setting should be saved into static variables.
		 */
		save1 = inb(0x3CC);
		/* Any extended registers would go here */
                outb(0x3C4,0xC4);
		save2 = inb(0x3C5);
		break;
	case CLK_REG_RESTORE:
		/*
		 * Here all the previously saved registers are restored.
		 */
		outb(0x3C2, save1);
		/* Any extended registers would go here */
		outb(0x3C4,0xC4);
		outb(0x3C5,save2);
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
		 outb(0x3C4,0xC4);
		 temp = (inb(0x3C5) & 0xFE) | ((no & 4) >> 2);
		 outb(0x3C5,temp);
	}
	return(TRUE);
}


/*
 * MXProbe --
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
MXProbe()
{
	static Bool failed;
	unsigned char save;
	/*
	 * Set up I/O ports to be used by this card.  Only do the second
	 * xf86AddIOPorts() if there are non-standard ports for this
	 * chipset.
	 */
	xf86ClearIOPortList(vga256InfoRec.scrnIndex);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, 
		       Num_MX_ExtPorts, MX_ExtPorts);

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
		if (StrCaseCmp(vga256InfoRec.chipset, MXIdent(0)))
			return (FALSE);
      		else
			MXEnterLeave(ENTER);
    	}
  	else
	{
		unsigned char save;
		/*
		 * OK.  We have to actually test the hardware.  The
		 * EnterLeave() function (described below) unlocks access
		 * to registers that may be locked, and for OSs that require
		 * it, enables I/O access.  So we do this before we probe,
		 * even though we don't know for sure that this chipset
		 * is present.
		 */
		MXEnterLeave(ENTER);

		/*
		 * Here is where all of the probing code should be placed.  
		 * The best advice is to look at what the other drivers are 
		 * doing.  If you are lucky, the chipset reference will tell 
		 * how to do this.  Other resources include SuperProbe/vgadoc2,
		 * and the Ferraro book.
		 */
		failed = FALSE;
		save = rdinx(0x3C4,0xA7);
		wrinx(0x3C4,0xA7,0);
		if (testinx(0x3C4,0xC5)) {failed = TRUE;}
		if (!failed) 
		{
			wrinx(0x3C4,0xA7,0x87);
			if (!testinx(0x3C4,0xC5)) {failed = TRUE;}
		}
      		if (failed)
		{
			/*
			 * Turn things back off if the probe is going to fail.
			 * Returning FALSE implies failure, and the server
			 * will go on to the next driver.
			 */
			wrinx(0x3C4,0xA7,save);
	  		MXEnterLeave(LEAVE);
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
		switch(((rdinx(0x3C4,0xC2) >> 2) & 3)) {
		case 0: vga256InfoRec.videoRam = 256;
			break;
		case 1: vga256InfoRec.videoRam = 512;
			break;
		case 2: vga256InfoRec.videoRam = 1024;
			break;
		default: ErrorF("MX driver: unkown memory setting \n");
			 MXEnterLeave(LEAVE);
			 return(FALSE);
		}
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
      		vgaGetClocks(8, MXClockSelect);
    	}

	/*
	 * It is recommended that you fill in the maximum allowable dot-clock
	 * rate for your chipset.  If you don't do this, the default of
	 * 90MHz will be used; this is likely too high for many chipsets.
	 * This is specified in KHz, so 90Mhz would be 90000 for this
	 * setting.
	 */
	vga256InfoRec.maxClock = 65000 ;

	/*
	 * Last we fill in the remaining data structures.  We specify
	 * the chipset name, using the Ident() function and an appropriate
	 * index.  We set a boolean for whether or not this driver supports
	 * banking for the Monochrome server.  And we set up a list of all
	 * the option flags that this driver can make use of.
	 */
  	vga256InfoRec.chipset = MXIdent(0);
  	vga256InfoRec.bankedMono = TRUE;
#ifndef MONOVGA
#ifdef XFreeXDGA 
	vga256InfoRec.directMode = XF86DGADirectPresent;
#endif
#endif

  	return(TRUE);
}

/*
 * MXEnterLeave --
 *
 * This function is called when the virtual terminal on which the server
 * is running is entered or left, as well as when the server starts up
 * and is shut down.  Its function is to obtain and relinquish I/O 
 * permissions for the SVGA device.  This includes unlocking access to
 * any registers that may be protected on the chipset, and locking those
 * registers again on exit.
 */
static void 
MXEnterLeave(enter)
Bool enter;
{
	unsigned char temp, temp2;

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

	        /* enable access to extended registers */

		outb(0x3C4,0xA7); temp2 = inb(0x3C5);
		outb(0x3C5,0x87);
    	}
  	else
    	{	
		outb(0x3C4,0xA7);
		outb(0x3C5,temp2);

		/*
		 * Here undo what was done above.
		 */
		xf86DisableIOPorts(vga256InfoRec.scrnIndex);


    	}
}

/*
 * MXRestore --
 *
 * This function restores a video mode.  It basically writes out all of
 * the registers that have previously been saved in the vgaMXRec data 
 * structure.
 *
 * Note that "Restore" is a little bit incorrect.  This function is also
 * used when the server enters/changes video modes.  The mode definitions 
 * have previously been initialized by the Init() function, below.
 */
static void 
MXRestore(restore)
vgaMXPtr restore;
{
	unsigned char temp;

	vgaProtect(TRUE);

	/*
	 * Whatever code is needed to get things back to bank zero should be
	 * placed here.  Things should be in the same state as when the
	 * Save/Init was done.
	 */
	outw(0x3C4,0xC5);

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
	outb(0x3C4,0x65);
	temp = (inb(0x3C5) & 0xBF) | restore->mxa;
	outb(0x3C5,temp); 
	outb(0x3C4,0xC3);
	temp = (inb(0x3C5) & 0x73) | restore->mxc;
	outb(0x3C5,temp);
	outb(0x3C4,0xC5);
	outb(0x3C5,restore->mxd);
	outb(0x3C4,0xF0);
	temp = (inb(0x3C5) & 0xF4) | restore->mxe;
	outb(0x3C5,temp);
	outb(0x3C4,0xF1);
	temp = (inb(0x3C5) & 0xFC) | restore->mxf;
	outb(0x3C5,temp);
	outb(0x3C4,0xF3);
	outb(0x3C5,restore->mxg);

	vgaProtect(FALSE);
}

/*
 * MXSave --
 *
 * This function saves the video state.  It reads all of the SVGA registers
 * into the vgaMXRec data structure.  There is in general no need to
 * mask out bits here - just read the registers.
 */
static void *
MXSave(save)
vgaMXPtr save;
{
	/*
	 * Whatever code is needed to get back to bank zero goes here.
	 */
	outw(0x3C4,0xC5);
	/*
	 * This function will handle creating the data structure and filling
	 * in the generic VGA portion.
	 */
	save = (vgaMXPtr)vgaHWSave((vgaHWPtr)save, sizeof(vgaMXRec));

	/*
	 * The port I/O code necessary to read in the extended registers 
	 * into the fields of the vgaMXRec structure goes here.
	 */
	outb(0x3C4,0x65);
	save->mxa = inb(0x3C5);
	outb(0x3C4,0xC3);
	save->mxc = inb(0x3C5);
	outb(0x3C4,0xC5);
	save->mxd = inb(0x3C5);
	outb(0x3C4,0xF0);
	save->mxe = inb(0x3C5);
	outb(0x3C4,0xF1);
	save->mxf = inb(0x3C5);
	outb(0x3C4,0xF3);
	save->mxg = inb(0x3C5);
}

/*
 * MXInit --
 *
 * This is the most important function (after the Probe) function.  This
 * function fills in the vgaMXRec with all of the register values needed
 * to enable either a 256-color mode (for the color server) or a 16-color
 * mode (for the monochrome server).
 *
 * The 'mode' parameter describes the video mode.  The 'mode' structure 
 * as well as the 'vga256InfoRec' structure can be dereferenced for
 * information that is needed to initialize the mode.  The 'new' macro
 * (see definition above) is used to simply fill in the structure.
 */
static Bool
MXInit(mode)
DisplayModePtr mode;
{
	/*
	 * This will allocate the datastructure and initialize all of the
	 * generic VGA registers.
	 */
	if (!vgaHWInit(mode,sizeof(vgaMXRec)))
		return(FALSE);

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
	new->std.CRTC[19] = vga256InfoRec.displayWidth >> 3;
	new->std.CRTC[20] = 0x40; 
	new->mxa = 0x40;
	new->mxc = 0xCC;
	new->mxf = 0x0;
	new->mxd = 0x0;
 	if (mode->Flags & V_INTERLACE) new->mxe = 0xB; else new->mxe=0x8; 

	return(TRUE);
}

/*
 * MXAdjust --
 *
 * This function is used to initialize the SVGA Start Address - the first
 * displayed location in the video memory.  This is used to implement the
 * virtual window.
 */
static void 
MXAdjust(x, y)
int x, y;
{
unsigned char temp;
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
	int Base = (y * vga256InfoRec.displayWidth + x) >> 2;

	/*
	 * These are the generic starting address registers.
	 */
	outw(vgaIOBase + 4, (Base & 0x00FF00) | 0x0C);
  	outw(vgaIOBase + 4, ((Base & 0x00FF) << 8) | 0x0D);

	/*
	 * Here the high-order bits are masked and shifted, and put into
	 * the appropriate extended registers.
	 */
        outb(0x3C4,0xF1);
	temp = inb(0x3C5);
	temp &= 0xFC;
	temp |= ((Base & 0x30000) >> 16);
	outb(0x3C5,temp);

#ifdef XFreeXDGA
	if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
		/* Wait until vertical retrace is in progress. */
		while (inb(vgaIOBase + 0xA) & 0x08);
		while (!(inb(vgaIOBase + 0xA) & 0x08));
	}
#endif
}

/*
 * MXValidMode --
 *
 */
static int
MXValidMode(mode, verbose,flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
return MODE_OK;
}


/*
 * Copyright 1997
 * Digital Equipment Corporation. All rights reserved.
 * This software is furnished under license and may be used and copied only in 
 * accordance with the following terms and conditions.  Subject to these 
 * conditions, you may download, copy, install, use, modify and distribute 
 * this software in source and/or binary form. No title or ownership is 
 * transferred hereby.
 * 1) Any source code used, modified or distributed must reproduce and retain 
 *    this copyright notice and list of conditions as they appear in the 
 *    source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of Digital 
 *    Equipment Corporation. Neither the "Digital Equipment Corporation" name 
 *    nor any trademark or logo of Digital Equipment Corporation may be used 
 *    to endorse or promote products derived from this software without the 
 *    prior written permission of Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied warranties,
 *    including but not limited to, any implied warranties of merchantability,
 *    fitness for a particular purpose, or non-infringement are disclaimed. In
 *    no event shall DIGITAL be liable for any damages whatsoever, and in 
 *    particular, DIGITAL shall not be liable for special, indirect, 
 *    consequential, or incidental damages or damages for lost profits, loss 
 *    of revenue or loss of use, whether such damages arise in contract, 
 *    negligence, tort, under statute, in equity, at law or otherwise, even if
 *    advised of the possibility of such damage. 
 */
/*
 * The CyberPro2010 driver is based on sample code provided by IGS 
 * Technologies, Inc. http://www.integraphics.com.
 * IGS Technologies, Inc makes no representations about the suitability of 
 * this software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */
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
#include "igs_driver.h"

/*
 * for PCI probing etc.
 */
#include "xf86_PCI.h"
#include "vgaPCI.h"
extern vgaPCIInformation *vgaPCIInfo;

/*
 * If the driver makes use of XF86Config 'Option' flags, the following will be
 * required
 */
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

/* DGA includes */
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

/*
 * This header is required for drivers that implement FbInit().
 */
#if !defined(MONOVGA) && !defined(XF86VGA16)
#include "vga256.h"
#endif

/*
 * IGS internal specific variables 
 */
Bool igsLinearSupport = FALSE;

/* Pixmap Cache End */
unsigned int igsCacheEnd = 0;

/* Blitter related */
unsigned int igsXGABase = 0;
volatile unsigned char *igsCopControl;
Bool igsAvoidImageBLT = FALSE;
unsigned char *igsBltDataWindow = NULL;

/* Ping-pong buffer for XAA ScreenToScreen colour expansion */
unsigned int igsColorExpandScratchAddr = 0;
unsigned int igsColorExpandScratchSize = 0;

/* HW cursor related */
Bool igsHWCursor = FALSE;
extern Bool IGSCursorInit();
extern void IGSRestoreCursor();
extern void IGSWarpCursor();
extern void IGSQueryBestSize();
extern vgaHWCursorRec vgaHWCursor;
unsigned int igsCursorAddress = 0;  /* The address in video ram of the cursor */

/* Clock related */
typedef struct {
    unsigned char msr;
    int Clock;
} igsClockReg, *igsClockPtr;

int igsCurrentClock = 0;
static unsigned char igsConsole_clk[2];

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
    unsigned char RamdacCtrl;
    unsigned char ExtCrtReg[0x48];
    unsigned char ExtGraReg[0xFF];
    unsigned char XReg[0x10];
    unsigned char YReg[0x10];
    unsigned char WReg[0x10];
    unsigned char TReg[0x10];
    unsigned char JReg[0x10];
    unsigned char KReg[0x10];
    igsClockReg igsClock;
} vgaIGSRec, *vgaIGSPtr;

/*
 * Forward definitions for the functions that make up the driver.  See
 * the definitions of these functions for the real scoop.
 */
static Bool     IGSProbe();
static char *   IGSIdent();
static Bool     IGSClockSelect();
static void     IGSEnterLeave();
static Bool     IGSInit();
static int	IGSValidMode();
static void *   IGSSave();
static void     IGSRestore();
static void     IGSAdjust();
static void     IGSSaveScreen();
static void     IGSGetMode();
static void	IGSFbInit();
/*
 * These are the bank select functions.  There are defined in igs_bank.[cs]
 */
extern void     IGSSetRead();
extern void     IGSSetWrite();
extern void     IGSSetReadWrite();

/*
 * This data structure defines the driver itself.  The data structure is
 * initialized with the functions that make up the driver and some data 
 * that defines how the driver operates.
 */
vgaVideoChipRec IGS = {
    /* 
     * Function pointers
     */
    IGSProbe,
    IGSIdent,
    IGSEnterLeave,
    IGSInit,
    IGSValidMode,
    IGSSave,
    IGSRestore,
    IGSAdjust,
    IGSSaveScreen,
    IGSGetMode,
    IGSFbInit,
    IGSSetRead,
    IGSSetWrite,
    IGSSetReadWrite,
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
    VGA_NO_DIVIDE_VERT,
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
    FALSE,
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
     * ClockMulFactor can be used to scale the raw clocks to pixel
     * clocks. This is rarely used, and in most cases, set it to 1. The
     * "raw" pixel clock from the modeline is multiplied by this value,
     * and that value is used for clock selection.
     */
    1,
    /*
     * ClockDivFactor can be used to scale the raw clocks to pixel
     * clocks. This is rarely used, and in most cases, set it to 1. The
     * "raw" pixel clock from the modeline is divided by this value
     * (after having been multiplied by ClockMulFactor), and that value
     * is used for clock selection.
     *
     * When these values are set to a non-1 value, the server will
     * automatically scale pixel clocks (if a discrete clock generator
     * is used) and maximum allowed pixel clock. If e.g. a 16bpp mode
     * requires pixel clock multipled by 2 (to be able to transport data
     * over an 8-bit data bus), setting ClockMulFactor to 2 will
     * automatically scale the maximum allowed ramdac speed with the
     * same factor, and use a clock*2 for the pixel data transfer clock.
     *
     * ClockMulFactor and ClockDivFactor can only be used when the
     * "rules" don't change over time: dynamic changes of these
     * variables will cause inconsistent server behaviour (e.g. it will
     * report wrong pixel clocks and maximum pixel speeds. Pixel
     * multiplexing modes for example can mostly only be enabled if the
     * pixel clock is higher than a certain value, and these modes
     * cannot use ClockDivFactor (they don't need to: the maximum RAMDAC
     * speed remains the same, so there's no use in the server code
     * lowering it).
     *
     * For 24bpp modes over a 16-bit RAMDAC bus, one would set
     * ClockMulFactor to 3, and ClockDivFactor to 2.
     */
    1
};

static void igsClockSave();
static void igsClockLoad();
static Bool igsClockFind();
static void igsCalcClock();

unsigned char igsVgaIOBaseFlag = 0xFF;
unsigned int igsCRTIndex, igsCRTData;
unsigned int igsST01Reg;

/*
 * This is a convenience macro, so that entries in the driver structure
 * can simply be dereferenced with 'new->xxx'.
 */
#define new ((vgaIGSPtr)vgaNewVideoState)

/*
 * Maximum allowable dot clock rate.
 */
#define IGS_MAX_CLOCK_IN_KHZ 206000
#define FREF 14318180

/*
 * If your chipset uses non-standard I/O ports, you need to define an
 * array of ports, and an integer containing the array size.  The
 * generic VGA ports are defined in vgaHW.c.
 */
static unsigned IGS_ExtPorts[] = { 
    PORT46E8,
    PORT102,
};
static int Num_IGS_ExtPorts =
	(sizeof(IGS_ExtPorts)/sizeof(IGS_ExtPorts[0]));

static void igsDumpRegs(void);

/*
 * IGSIdent --
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
IGSIdent(n)
int n;
{
    static char *chipsets[] = {"CyberPro2010"};

    if (n + 1 > sizeof(chipsets) / sizeof(char *))
	return(NULL);
    else
	return(chipsets[n]);
}

/*
 * IGSClockSelect --
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
IGSClockSelect(no)
int no;
{
    static igsClockReg SaveClock;
    static igsClockPtr Clock = &SaveClock;
    igsClockReg TmpClock;

#ifdef DEBUG
    ErrorF("IGSClockSelect %d\n", no);
#endif

    switch(no)
    {
	case CLK_REG_SAVE:
	    /*
	     * Here all of the registers that can be affected by
	     * clock setting should be saved into static variables.
	     */
	    igsClockSave(Clock);
	    break;
	case CLK_REG_RESTORE:
	    /*
	     * Here all the previously saved registers are restored.
	     */
	    igsClockLoad(Clock);
	    break;
	default:
	    /* 
	     * These are the generic two low-order bits of the clock select 
	     */
	    if(!igsClockFind(no, &TmpClock))
		return(FALSE);
	    igsClockLoad(&TmpClock);
    }
    return(TRUE);
}


/*
 * IGSProbe --
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
IGSProbe()
{
    unsigned char temp;
    unsigned char chipId, chipRev;
    int failed = FALSE;
    int memtype;
    int addr;

#ifdef DEBUG
    ErrorF("IGSProbe\n");
#endif

    /*
     * Set up I/O ports to be used by this card.  Only do the second
     * xf86AddIOPorts() if there are non-standard ports for this
     * chipset.
     */
    xf86ClearIOPortList(vga256InfoRec.scrnIndex);
    xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
    xf86AddIOPorts(vga256InfoRec.scrnIndex, 
		    Num_IGS_ExtPorts, IGS_ExtPorts);

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
	if (StrCaseCmp(vga256InfoRec.chipset, IGSIdent(0)))
	    return (FALSE);
	else
	    IGSEnterLeave(ENTER);

#ifdef DUMPREG
	igsDumpRegs();
#endif
    }
    else
    {
#ifdef	notapplicable
	/*
	 * Start with PCI probing, this should get us quite far allready.
	 */

	if (vgaPCIInfo)
	{
	    if (vgaPCIInfo->Vendor!=PCI_VENDOR_SUPERDUPER)
		return(FALSE);

	    switch(vgaPCIInfo->ChipType)
	    {
		case PCI_CHIP_SUPERCHIP1:
		case PCI_CHIP_SUPERCHIP2:
		case PCI_CHIP_SUPERCHIP3:
		    break;  /* ok, known chip */
		default:
		    return(FALSE);
	    }
	}
#endif	/* notapplicable */

	/*
	 * If PCI probing isn't sufficient, we have to actually test
	 * the hardware.  The EnterLeave() function (described below)
	 * unlocks access to registers that may be locked, and for OSs
	 * that require it, enables I/O access.  So we do this before
	 * we probe, even though we don't know for sure that this chipset
	 * is present.
	 */
	IGSEnterLeave(ENTER);

#ifdef DUMPREG
	igsDumpRegs();
#endif

	/*
	 * Here is where all of the probing code should be placed.  
	 * The best advice is to look at what the other drivers are 
	 * doing.  If you are lucky, the chipset reference will tell 
	 * how to do this.  Other resources include SuperProbe/vgadoc2,
	 * and the Ferraro book.
	 */

	/*
	 * Probe the chip for extended chip ID values at port
	 * 3CF index 91, 92, 93 - Data = A1, 03(ChipID), 00(ChipRev) ?
	 */
	outb(EXTINDEX, 0x91); 
	temp = inb(EXTDATA);
#ifdef DEBUG
	ErrorF("IGSProbe: Read 0x%02x from GR91\n", temp);
#endif	
	if (temp != /*0xA1*/0xA4)
	{
	    failed = TRUE;
	}
	else
	{
	    /* Chip ID */
	    outb(EXTINDEX, 0x92);
	    chipId = inb(EXTDATA);
#ifdef DEBUG
	    ErrorF("IGSProbe: Read 0x%02x from GR92\n", chipId);
#endif
	    outb(EXTINDEX, 0x93);
	    chipRev = inb(EXTDATA);
#ifdef DEBUG
	    ErrorF("IGSProbe: Read 0x%02x from GR93\n", chipRev);
#endif
	    if (chipId != /*0x03*/0x08 || chipRev != 0x00)
		failed = TRUE;
	}       

	if (failed)
	{
	    /*
	     * Turn things back off if the probe is going to fail.
	     * Returning FALSE implies failure, and the server
	     * will go on to the next driver.
	     */
	    IGSEnterLeave(LEAVE);
#ifdef DEBUG
	    ErrorF("Bombing out!\n");
#endif
	    return(FALSE);
	}
    }

    /*
     * Just for interest, probe the memory type.
     */
    outb(EXTINDEX, 0x71);
    memtype = inb(EXTDATA) & 0x08;

    /*
     * If the user has specified the amount of memory in the XF86Config
     * file, we respect that setting.
     */
    if (!vga256InfoRec.videoRam)
    {
	/*
	 * Otherwise, probe the chip for memory size -
	 * index 0x72 bit 2-1: memory size   
	 *          0: 1 MB    
	 *          1: 2 MB    
	 *          2: 4 MB    
	 *          3: reserved   	
	 */
	outb(EXTINDEX, 0x72);
	temp = inb(EXTDATA);
	switch(temp & 0x03)
	{
	    case 0:
		vga256InfoRec.videoRam = 1024; 
		break;
	    case 1:
		vga256InfoRec.videoRam = 2048; 
		break;
	    case 2:
	    default:
		vga256InfoRec.videoRam = 4096; 
		break;
	}
	ErrorF("%s %s: CyberPro: %d kB %s\n", XCONFIG_PROBED,
	       vga256InfoRec.name, vga256InfoRec.videoRam,
	       (memtype? "VRAM" : "DRAM"));
    }
    else 
    {
	ErrorF("%s %s: CyberPro: %d kB %s\n", XCONFIG_GIVEN,
	       vga256InfoRec.name, vga256InfoRec.videoRam,
	       (memtype? "VRAM" : "DRAM"));
    }

    /* Linear addressing */
    if (OFLG_ISSET(OPTION_NOLINEAR_MODE, &vga256InfoRec.options))
    {
	ErrorF("%s %s: CyberPro: Disabling Linear Addressing\n",
	    XCONFIG_GIVEN, vga256InfoRec.name);
    }
    else if (!xf86LinearVidMem())
    {
	ErrorF("%s %s: CyberPro: Disabling Linear Addressing\n",
	    XCONFIG_PROBED, vga256InfoRec.name);
    }
    else
    {
	IGS.ChipUseLinearAddressing = TRUE;
	igsLinearSupport = TRUE;
	
	if (vga256InfoRec.MemBase) 
	{
	    IGS.ChipLinearBase = vga256InfoRec.MemBase;
	    ErrorF("%s %s: CyberPro: Linear addressing is enabled at 0x%X.\n",
		   XCONFIG_GIVEN, vga256InfoRec.name, IGS.ChipLinearBase);
	} 
	else 
	{
	    outb(EXTINDEX, 0x35);
	    IGS.ChipLinearBase = inb(EXTDATA) << 24;
	    ErrorF("%s %s: CyberPro: Linear addressing is enabled at 0x%X.\n",
		   XCONFIG_PROBED, vga256InfoRec.name, IGS.ChipLinearBase);    
	}
	IGS.ChipLinearSize = vga256InfoRec.videoRam * 1024;

	IGS.ChipHas16bpp = TRUE;
	IGS.ChipHas24bpp = TRUE;
	IGS.ChipHas32bpp = TRUE;
    }

    /* >8bpp requires linear addressing */
    if (!IGS.ChipUseLinearAddressing && vgaBitsPerPixel > 8) {
	ErrorF("%s %s: CyberPro: Depth %d only supported with linear addressing\n",
	       XCONFIG_PROBED, vga256InfoRec.name, vgaBitsPerPixel);
	return (FALSE);
    }

    /*
     * Again, if the user has specified the clock values in the XF86Config
     * file, we respect those choices.
     */
    if (!vga256InfoRec.clocks)
    {
	/*
	 ** Set up programmable clocks
	 */
	if (!vga256InfoRec.clockprog)
	{
	    vga256InfoRec.clocks = 0;
	}
	if (vga256InfoRec.textClockFreq > 0) 
	{
	    igsCurrentClock = vga256InfoRec.textClockFreq;
	    ErrorF("%s %s: CyberPro: using textclock freq: %7.3f.\n",
		   XCONFIG_GIVEN,
		   vga256InfoRec.name,igsCurrentClock/1000.0);
	} 
	else 
	{
	    igsCurrentClock =  0;
	}
	ErrorF("%s %s: CyberPro: using programmable clocks.\n",
	       XCONFIG_PROBED, vga256InfoRec.name);
/*	IGS.ChipClockScaleFactor = 1;*/	/* XXX - Katrina? */

#ifdef	notapplicable
	/*
	 * This utility function will probe for the clock values.
	 * It is passed the number of supported clocks, and a
	 * pointer to the clock-select function.
	 */
	vgaGetClocks(NUM, IGSClockSelect);
#endif	/* notapplicable */
    }
    else
    {
	FatalError("IGSProbe: Clocks line in config file not supported\n");
    }

    /*
     * It is recommended that you fill in the maximum allowable dot-clock
     * rate for your chipset.  If you don't do this, the default of
     * 90MHz will be used; this is likely too high for many chipsets.
     * This is specified in KHz, so 90Mhz would be 90000 for this
     * setting.
     */
    vga256InfoRec.maxClock = IGS_MAX_CLOCK_IN_KHZ;

    /*
     * Last we fill in the remaining data structures.  We specify
     * the chipset name, using the Ident() function and an appropriate
     * index.  We set a boolean for whether or not this driver supports
     * banking for the Monochrome server.  And we set up a list of all
     * the option flags that this driver can make use of.
     */
    vga256InfoRec.chipset = IGSIdent(0);
    vga256InfoRec.bankedMono = TRUE;

    /* allowed options */
    OFLG_SET(OPTION_NOLINEAR_MODE, &IGS.ChipOptionFlags);
    OFLG_SET(OPTION_NOACCEL, &IGS.ChipOptionFlags);
    OFLG_SET(OPTION_SW_CURSOR, &IGS.ChipOptionFlags);
    OFLG_SET(OPTION_NO_BITBLT, &IGS.ChipOptionFlags);
    OFLG_SET(OPTION_NO_IMAGEBLT, &IGS.ChipOptionFlags);
    OFLG_SET(OPTION_ENGINE_DELAY, &IGS.ChipOptionFlags);

    /* 
     * if your driver uses a programmable clockchip, you have
     * to set this option to avoid clock probing etc.
     */
    OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions);

#ifndef MONOVGA
#ifdef XFreeXDGA 
    vga256InfoRec.directMode = XF86DGADirectPresent;
#endif
#endif

    return(TRUE);
}

/*
 * IGSEnterLeave --
 *
 * This function is called when the virtual terminal on which the server
 * is running is entered or left, as well as when the server starts up
 * and is shut down.  Its function is to obtain and relinquish I/O 
 * permissions for the SVGA device.  This includes unlocking access to
 * any registers that may be protected on the chipset, and locking those
 * registers again on exit.
 */
static void 
IGSEnterLeave(enter)
Bool enter;
{
    unsigned char temp;

#ifdef DEBUG
    ErrorF("IGSEnterLeave(");
    if (enter)
	ErrorF("Enter)\n");
    else
	ErrorF("Leave)\n");
#endif

#ifndef MONOVGA
#ifdef XFreeXDGA 
    if ((vga256InfoRec.directMode & XF86DGADirectGraphics) && !enter)
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
	if (igsVgaIOBaseFlag == 0xFF)
	{ 
	    /*save: suspend/resume might mess it up*/
	    igsVgaIOBaseFlag = (inb(MISCREAD) & 0x01);

	    vgaIOBase = igsVgaIOBaseFlag ? 0x3D0 : 0x3B0;
	    igsCRTIndex = vgaIOBase + 4;
	    igsCRTData = vgaIOBase + 5;
	    igsST01Reg = vgaIOBase + 0xA;
#ifdef DEBUG
	    if (igsVgaIOBaseFlag)
		ErrorF("color\n");
	    else
		ErrorF("monochrome\n");
#endif			
	}
	else
	{
	    /* hack: if not 1st time fix it up as it might be messed up by s/r */
	    temp = inb(0x3CC);
	    outb(0x3C2, (temp & 0xFE) | igsVgaIOBaseFlag); 
	}

	/*
	 * Here we deal with register-level access locks.  This
	 * is a generic VGA protection; most SVGA chipsets have
	 * similar register locks for their extended registers
	 * as well.
	 */
	/* Unprotect CRTC[0-7] */
	outb(igsCRTIndex, 0x11); temp = inb(vgaIOBase + 5);
	outb(igsCRTData, temp & 0x7F);

	/* XXX Enable HW Cursor here */
    }
    else
    {
	/*
	 * Here undo what was done above.
	 */

	/* XXX Disable HW Cursor here */

	/* Protect CRTC[0-7] */
	outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
	outb(vgaIOBase + 5, (temp & 0x7F) | 0x80);

	xf86DisableIOPorts(vga256InfoRec.scrnIndex);
    }
}

/*
 * IGSRestore --
 *
 * This function restores a video mode.  It basically writes out all of
 * the registers that have previously been saved in the vgaIGSRec data 
 * structure.
 *
 * Note that "Restore" is a little bit incorrect.  This function is also
 * used when the server enters/changes video modes.  The mode definitions 
 * have previously been initialized by the Init() function, below.
 */
static void 
IGSRestore(restore)
vgaIGSPtr restore;
{
    int i;
    unsigned char tmp;

#ifdef DEBUG
    ErrorF("IGSRestore\n");
#endif

    vgaProtect(TRUE);		/* Blank the screen */

    /*
     * Whatever code is needed to get things back to bank zero should be
     * placed here.  Things should be in the same state as when the
     * Save/Init was done.
     */
    IGSSetRead(0);
    IGSSetWrite(0);

    /* fix things that could be messed up by suspend/resume */
    outb(MISCWRITE, (((((vgaHWPtr) restore)->MiscOutReg) & 0xFE) 
		     | igsVgaIOBaseFlag));
    outb(igsCRTIndex, 0x11);
    tmp = inb(igsCRTData);
    outb(igsCRTData, (tmp & 0x7F)); /*group 0 protection off */

    /* wait for vsync if sequencer is running - stop sequencer */
    igsWaitVSync();
    
    /*
     * Code to restore any SVGA registers that have been saved/modified
     * goes here.  Note that it is allowable, and often correct, to 
     * only modify certain bits in a register by a read/modify/write cycle.
     */

    /* Restore the Ramdac Control register */
    outb(EXTINDEX, 0x56);
    tmp = inb(EXTDATA);
    outb(EXTDATA, tmp | 0x04);  /* Access IGS 0x3C6 */
    outb(RAMDACMASK, (inb(RAMDACMASK) & ~0x10) | (restore->RamdacCtrl & 0x10));
    outb(EXTDATA, tmp & ~0x04);

    /* Restore modified extended registers */
    for (i = 0x10; i < 0x16; i++) {
	outb(EXTINDEX, i);
	outb(EXTDATA, restore->ExtGraReg[i]);
    }
    for (i = 0x30; i < 0x34; i++) {
	outb(EXTINDEX, i);
	outb(EXTDATA, restore->ExtGraReg[i]);
    }
    for (i = 0x50; i < 0x5B; i++) {
	outb(EXTINDEX, i);
	outb(EXTDATA, restore->ExtGraReg[i]);
    }
    for (i = 0x70; i < 0x78; i++) {
	outb(EXTINDEX, i);
	outb(EXTDATA, restore->ExtGraReg[i]);
    }
    outb(EXTINDEX, 0x7A);
    outb(EXTDATA, restore->ExtGraReg[0x7A]);
    outb(EXTINDEX, 0x90);
    outb(EXTDATA, restore->ExtGraReg[0x90]);

    /* Restore MCLK but don't touch the VCLK params */
    outb(EXTINDEX, 0xB2);
    outb(EXTDATA, restore->ExtGraReg[0xB2]);
    outb(EXTINDEX, 0xB3);
    outb(EXTDATA, restore->ExtGraReg[0xB3]);

    /* XXX - Undocumented magic from IGS! */
    outb(EXTINDEX, 0xBF);
    outb(EXTDATA, 0);

    /* Toggle B9 to latch values */
    outb(EXTINDEX, 0xB9);
    tmp = inb(EXTDATA);
    outb(EXTDATA, (tmp & ~0x80));
    outb(EXTDATA, (tmp | 0x80));
    outb(EXTDATA, (tmp & ~0x80));
    usleep(10000);		         /* XXX Not sure where the delay should be here  */

    /*
     * A special case - when using an external clock-setting program,
     * this function must not change bits associated with the clock
     * selection.  This condition can be checked by the condition:
     *
     *	if (restore->std.NoClock >= 0)
     *		restore clock-select bits.
     */
#ifdef IO_DEBUG
    ErrorF("1: 0x3CC: %X ", (unsigned char)inb(0x3CC));
#endif
    if (restore->std.NoClock >= 0) /* No external program */
    {
	igsClockLoad(&restore->igsClock);
    }
#ifdef IO_DEBUG
    ErrorF("-> %X\n", (unsigned char)inb(0x3CC));
#endif

    /*
     * This function handles restoring the generic VGA registers.
     */
    vgaHWRestore((vgaHWPtr)restore);

#ifdef DUMPREG
    ErrorF("Dumping restored registers...\n");
    igsDumpRegs();
#endif
    vgaProtect(FALSE);		/* Turn on screen */
}

/*
 * IGSSave --
 *
 * This function saves the video state.  It reads all of the SVGA registers
 * into the vgaIGSRec data structure.  There is in general no need to
 * mask out bits here - just read the registers.
 */
static void *
IGSSave(save)
vgaIGSPtr save;
{
    int i;
    unsigned char temp;

#ifdef DEBUG
    ErrorF("IGSSave\n");
#endif

    /*
     * Whatever code is needed to get back to bank zero goes here.
     */
    IGSSetRead(0);
    IGSSetWrite(0);

    /* fix things that could be messed up by suspend/resume */
    temp = inb(MISCREAD);
    outb(MISCWRITE, (temp & 0xFE) | igsVgaIOBaseFlag); 
    outb(igsCRTIndex, 0x11);
    temp = inb(igsCRTData);
    outb(igsCRTData, (temp & 0x7F));

    /*
     * This function will handle creating the data structure and filling
     * in the generic VGA portion.
     */
    save = (vgaIGSPtr)vgaHWSave((vgaHWPtr)save, sizeof(vgaIGSRec));

    /* save clock */
    igsClockSave(&save->igsClock);

    /*
     * The port I/O code necessary to read in the extended registers 
     * into the fields of the vgaIGSRec structure goes here.
     */
    /* Save the Ramdac Control register */
    outb(EXTINDEX, 0x56);
    temp = inb(EXTDATA);
    outb(EXTDATA, (temp | 0x04)); /* Access IGS 0x3C6 */
    save->RamdacCtrl = inb(RAMDACMASK);
    outb(EXTDATA, temp & ~0x04);
    
    /* Save the Extended CRT Registers */
    for (i = 0x22; i < 0x27; i+=2) {
	outb(igsCRTIndex, i);
	save->ExtCrtReg[i] = inb(igsCRTData);
    }
    for (i = 0x40; i < 0x48; i++) {
	outb(igsCRTIndex, i);
	save->ExtCrtReg[i] = inb(igsCRTData);
    }
    /* Save the Extended Graphics Registers */
    for (i = 0x10; i < 0xFE; i++) {
	outb(EXTINDEX, i);
	save->ExtGraReg[i] = inb(EXTDATA);
    }

    outb(EXTINDEX, 0xFA);
    outb(EXTDATA, 0x0);
    for (i = 0; i < 0x10; i++)
    {
	outb(EXTINDEX, 0x40 + i);
	save->XReg[i] = inb(EXTDATA);
    }
    outb(EXTINDEX, 0xFA);
    outb(EXTDATA, 0x1);
    for (i = 0; i < 0x10; i++)
    {
	outb(EXTINDEX, 0x40 + i);
	save->YReg[i] = inb(EXTDATA);
    }
    outb(EXTINDEX, 0xFA);
    outb(EXTDATA, 0x2);
    for (i = 0; i < 0x10; i++)
    {
	outb(EXTINDEX, 0x40 + i);
	save->WReg[i] = inb(EXTDATA);
    }
    outb(EXTINDEX, 0xFA);
    outb(EXTDATA, 0x3);
    for (i = 0; i < 0x10; i++)
    {
	outb(EXTINDEX, 0x40 + i);
	save->TReg[i] = inb(EXTDATA);
    }
    outb(EXTINDEX, 0xFA);
    outb(EXTDATA, 0x4);
    for (i = 0; i < 0x10; i++)
    {
	outb(EXTINDEX, 0x40 + i);
	save->JReg[i] = inb(EXTDATA);
    }
    outb(EXTINDEX, 0xFA);
    outb(EXTDATA, 0x5);
    for (i = 0; i < 0x10; i++)
    {
	outb(EXTINDEX, 0x40 + i);
	save->KReg[i] = inb(EXTDATA);
    }    
    /*
     * Put back original index value
     */
    outb(EXTINDEX, 0xFA);
    outb(EXTDATA, save->ExtGraReg[0xFA]);

    return ((void *) save);
}

/*
 * IGSInit --
 *
 * This is the most important function (after the Probe) function.  This
 * function fills in the vgaIGSRec with all of the register values needed
 * to enable either a 256-color mode (for the color server) or a 16-color
 * mode (for the monochrome server).
 *
 * The 'mode' parameter describes the video mode.  The 'mode' structure 
 * as well as the 'vga256InfoRec' structure can be dereferenced for
 * information that is needed to initialize the mode.  The 'new' macro
 * (see definition above) is used to simply fill in the structure.
 */
static Bool
IGSInit(mode)
DisplayModePtr mode;
{
    unsigned char tmp;
    int memfetch;
    int hoffset;
    
#ifdef DEBUG
    ErrorF("IGSInit\n");
#endif

    /* fix things that could be messed up by suspend/resume */
    tmp = inb(MISCREAD);
    outb(MISCWRITE, (tmp & 0xFE) | igsVgaIOBaseFlag); /* IO Address Select - MDA/CGA */
    outb(igsCRTIndex, 0x11);
    tmp = inb(igsCRTData);
    outb(igsCRTData, (tmp & 0x7F)); /* Enable writes to CR00-CR07 */

    /*
     * This will allocate the datastructure and initialize all of the
     * generic VGA registers.
     */
    if (!vgaHWInit(mode,sizeof(vgaIGSRec)))
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
    if (new->std.NoClock >= 0) /* No external program */
    { 		
	if (!igsClockFind(new->std.NoClock, &new->igsClock)) 
	{
	    ErrorF("IGSInit: ClockFind failed\n");
	    return (FALSE);
	}
    }

    /* Set-up CRTC offset register? -JJK */
    hoffset = (vga256InfoRec.displayWidth >> 3) * vgaBytesPerPixel;
    new->std.CRTC[0x13]	= hoffset & 0xFF;   /* top bits go into XR15 */

    /* XXX Override GR05 Shift register */
    new->std.Graphics[5] = 0x60; 
    /* XXX Override AR10 Pixel clock select */
    new->std.Attribute[0x10] =  0x01;

    
    /* Set Extended Registers - note: restore only uses bit 4 of Ramdac */
    if (vgaBitsPerPixel > 8)
	new->RamdacCtrl = 0x10;
    else
	new->RamdacCtrl = 0x00;
   
    new->ExtGraReg[0x10] = 0x10; /* Extended start address on */
    /* Vertical overflow */
    new->ExtGraReg[0x11] = (((mode->CrtcVTotal -2) & 0x400) >> 10) | 
	(((mode->CrtcVDisplay -1) & 0x400) >> 9) |
	    ((mode->CrtcVSyncStart & 0x400) >> 8) |
		((mode->CrtcVSyncStart & 0x400) >> 7) |
		    0x10; 
    if (mode->Flags & V_INTERLACE)
    {
	new->ExtGraReg[0x11] |= 0x20;          /*   set interlace */
    }
    
    new->ExtGraReg[0x12] = 0x00;	/* IRQ Disabled */
    new->ExtGraReg[0x13] = 0x00;	/* Testing off */
    /* Memory fetches per scan line */
    /* According to Bo, this should be hoffset + 1 */
    memfetch = hoffset + 1;
    new->ExtGraReg[0x14] = memfetch & 0xFF;	
    new->ExtGraReg[0x15] = ((memfetch & 0x300) >> 8) | ((hoffset & 0x300) >> 4);
    
    if(IGS.ChipUseLinearAddressing)
    {
	/*
	 * Set up XGA Coprocessor base address and data window for host
	 * to screen bitblt.
	 */
	igsXGABase = 
	    (unsigned int)xf86MapVidMem(vga256InfoRec.scrnIndex, 
					LINEAR_REGION,
					(pointer)(IGS.ChipLinearBase 
						  + IGS_PHYSMEM 
						  + IGS_BSEG_XGABASE),
					IGS_XGALEN);
	igsBltDataWindow = vgaLinearBase;
	igsCopControl = (unsigned char *)(igsXGABase + 0x11);
	    
	new->ExtGraReg[0x30] = 0x01; /* Linear address size = 2MB */
	new->ExtGraReg[0x33] = 0x19; /* Segment A, Linear address enabled */
				     /* Coprocessor at BFC00 */
	if ((vga256InfoRec.speedup & SPEEDUP_ANYWIDTH) && 
	    !OFLG_ISSET(OPTION_NO_BITBLT, &vga256InfoRec.options)) 
	{
	    new->ExtGraReg[0x33] |= 0x04;  /* Enable the coprocessor */
	}
    }
    else
    {
	/* XXX This has not been tested */
	igsXGABase = (unsigned int)vgaBase + IGS_BSEG_XGABASE;
	new->ExtGraReg[0x33] = 0x18; /* Segment A, Linear address disabled */;
	if ((vga256InfoRec.speedup & SPEEDUP_ANYWIDTH)
	    && !OFLG_ISSET(OPTION_NO_BITBLT, &vga256InfoRec.options))
	{
	    new->ExtGraReg[0x33] |= 0x24;  /* Enable the coprocessor */
	}
	igsBltDataWindow = vgaBase;
    }
        
    new->ExtGraReg[0x31] = 0x00; /* Extended segment write pointer */
    new->ExtGraReg[0x32] = 0x00; /* Extended segment read pointer */

    new->ExtGraReg[0x50] = 0x00; /* Sprite horizontal start low */
    new->ExtGraReg[0x51] = 0x00; /* Sprite horizontal start high */
    new->ExtGraReg[0x52] = 0x00; /* Sprite horizontal preset */
    new->ExtGraReg[0x53] = 0x00; /* Sprite vertical start low */
    new->ExtGraReg[0x54] = 0x00; /* Sprite vertical start high */
    new->ExtGraReg[0x55] = 0x00; /* Sprite vertical preset */
    new->ExtGraReg[0x56] = 0x00; /* Sprite control */

    new->ExtGraReg[0x57] = 0x01; /* Extended graphics mode */
    new->ExtGraReg[0x58] = 0x00; 
    new->ExtGraReg[0x59] = 0x00; 
    new->ExtGraReg[0x5A] = 0x00;

    new->ExtGraReg[0x70] = 0x0A; /* Memory cycle = 6 Mclk, RAS Precharge time extended 1 Mclk
				    Memory cycle can execute in multiple CAS mode,
				    Memory address in assymetrical mode */
    new->ExtGraReg[0x71] = 0x03; /* Memory chips layout = serial */
    new->ExtGraReg[0x72] = 0x01; /* 2MB memory, 32-bits memory  bandwidth */
    new->ExtGraReg[0x73] = 0x00; /* Hidden Control: XXX some examples set this to 0x03 */
    new->ExtGraReg[0x74] = 0x1B; /* FIFO Control - Delta M */
    new->ExtGraReg[0x75] = 0x1E; /* FIFO Control - Delta G */
    new->ExtGraReg[0x76] = 0x00; /* SEQ Programable */
    /* SL Signals between 2 CCLK */
    switch (vgaBitsPerPixel)
    {
	case 8:
	    new->ExtGraReg[0x77] = 0x01;
	    break;
	case 16:
	    new->ExtGraReg[0x77] = 0x02;
	    break;
	case 24:
	    new->ExtGraReg[0x77] = 0x04;
	    break;
	}
    
    new->ExtGraReg[0x7A] = 0xC8; /* Hidden control: MSCREENOFF gets full bandwidth for 
				    memory cycles, latching position = late to CAS */
    new->ExtGraReg[0x90] = 0x01; /* CPU Data written directly to display memory */

    /* MCLK Paramemters */
    if (OFLG_ISSET(OPTION_ENGINE_DELAY, &vga256InfoRec.options)) {
	/* Safety valve:  75MHz mclk */
	new->ExtGraReg[0xB2] = 0xDB; 
	new->ExtGraReg[0xB3] = 0x54;
    } else {
	/* Standard:  95MHz mclk */
	new->ExtGraReg[0xB2] = 0x91; 
	new->ExtGraReg[0xB3] = 0x6A;
    }

    return(TRUE);
}

/*
 * IGSAdjust --
 *
 * This function is used to initialize the SVGA Start Address - the first
 * displayed location in the video memory.  This is used to implement the
 * virtual window.
 */
static void 
IGSAdjust(x, y)
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
    int Base = (y * vga256InfoRec.displayWidth + x) >> 3;

#ifdef DEBUG
    ErrorF("IGSAdjust: width %d, x 0x%x, y 0x%x\n", 
	   vga256InfoRec.displayWidth, x, y);
#endif

    /* fix things that could be messed up by suspend/resume */
    temp = inb(MISCREAD);
    outb(MISCWRITE, (temp & 0xFE) | igsVgaIOBaseFlag); 
    outb(igsCRTIndex, 0x11);
    temp = inb(igsCRTData);
    outb(igsCRTData, (temp & 0x7F));

    /* calculate base bpp dep. */
    switch (vgaBitsPerPixel) {
    case 16:
	Base <<= 2;
	break;
    case 24:
	Base = (Base * 3) << 1;
	break;
    case 8:
	Base <<= 1;
	break;
    }

    /*
     * Write Base to chip: these are the generic starting address registers.
     */
    outb(igsCRTIndex, 0x0C);
    outb(igsCRTData, ((Base & 0x00FF00) >> 8));
    outb(igsCRTIndex, 0x0D);
    outw(igsCRTData, (Base & 0x00FF));
    
    /*
     * Here the high-order bits are masked and shifted, and put into
     * the appropriate extended registers.
     */
    outb(EXTINDEX, 0x10);
    outb(EXTDATA, (((Base & 0x0F0000) >> 16) | 0x10));    

#ifdef XFreeXDGA
    if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
	/* Wait until vertical retrace is in progress. */
	while (inb(vgaIOBase + 0xA) & 0x08);
	while (!(inb(vgaIOBase + 0xA) & 0x08));
    }
#endif
}

/*
 * IGSSaveScreen --
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
IGSSaveScreen(mode)
int mode;
{
#ifdef DEBUG
    ErrorF("IGSSaveScreen\n");
#endif

#ifdef	notapplicable
    if (mode == SS_START)
    {
	/*
	 * Save an registers that will be destroyed by the reset
	 * into static variables.
	 */

	/*
	 * Start sequencer reset.
	 */
	outw(0x3C4, 0x0100);
    }
    else
    {
	/*
	 * End sequencer reset.
	 */
	outw(0x3C4, 0x0300);

	/*
	 * Now restore those registers.
	 */
    }
#endif	/* notapplicable */
}

/*
 * IGSGetMode --
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
IGSGetMode(mode)
DisplayModePtr mode;
{
    /*
     * Fill in the 'mode' stucture based on current register settings.
     */
}

/*
 * IGSFbInit --
 *
 * This function is used to initialise chip-specific graphics functions.
 * It can be used to make use of the accelerated features of some chipsets.
 * For most drivers, this function is not required, and 'NoopDDA' is put
 * in the vgaVideoChipRec structure.
 */
static void
IGSFbInit()
{
    int useSpeedUp, offscreenSize;
    
#ifdef DEBUG
    ErrorF("IGSFbInit\n");
#endif

    if ((vga256InfoRec.displayWidth * vga256InfoRec.virtualY 
	 * vgaBytesPerPixel) > (vga256InfoRec.videoRam << 10)) 
    {
	ErrorF("%s %s: IGS: Virtual screen too large.\n",
	       XCONFIG_PROBED, vga256InfoRec.name);
	ErrorF("%s %s: IGS: Screen will be corrupted.\n",
	       XCONFIG_PROBED, vga256InfoRec.name);
    }
    if (OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options)) 
    {
	return;
    }
#if !defined(MONOVGA) && !defined(XF86VGA16)
    offscreenSize = igsInitializeAllocator();
    ErrorF("%s %s: IGS: %d bytes off-screen memory available\n",
	   XCONFIG_PROBED, vga256InfoRec.name, offscreenSize);

    /*
     * If hardware cursor is supported, the vgaHWCursor struct should
     * be filled in here.
     */
    if (!OFLG_ISSET(OPTION_SW_CURSOR, &vga256InfoRec.options)) 
    {
	ErrorF("%s %s: IGS: H/W cursor selected\n",
	       XCONFIG_PROBED, vga256InfoRec.name);

	if (igsLinearSupport)
	{
	    /* Allocate 1kB of vram to the cursor */
	    igsCursorAddress = igsAllocate(1024, 0xFF);
	}
	else
	{
	    /* Cursor is last 256 or 1024 bytes of video memory */
	    igsCursorAddress = 0xFC00;
	}
	vgaHWCursor.Initialized = TRUE;
	vgaHWCursor.Init = IGSCursorInit;
	vgaHWCursor.Restore = IGSRestoreCursor;
	vgaHWCursor.Warp = IGSWarpCursor;
	vgaHWCursor.QueryBestSize = IGSQueryBestSize;
    }
    
    useSpeedUp = vga256InfoRec.speedup & SPEEDUP_ANYWIDTH;
    if (useSpeedUp && !OFLG_ISSET(OPTION_NO_BITBLT, 
				  &vga256InfoRec.options)) 
    {
	ErrorF("%s %s: IGS: SpeedUps selected (Flags=0x%X)\n",
	       XCONFIG_PROBED, vga256InfoRec.name, useSpeedUp);
	
	if (OFLG_ISSET(OPTION_NO_IMAGEBLT, &vga256InfoRec.options)) 
	{
	    ErrorF("%s %s: IGS: Not using system-to-video BitBLT.\n",
		   XCONFIG_GIVEN, vga256InfoRec.name);
	    igsAvoidImageBLT = TRUE;
	}

#ifdef notyet /* ScanlineScreenToScreen not working properly yet */
	/* Setup the ping-pong buffer for ScreenToScreen colour expansion */
	igsColorExpandScratchAddr = igsAllocate(1024, 0xFF);
	if (igsColorExpandScratchAddr == -1) 
	{
	    ErrorF("%s %s: IGS: Too little space for ping-pong buffer.\n",
		   XCONFIG_PROBED, vga256InfoRec.name);
	    igsColorExpandScratchAddr = 0;
	    igsColorExpandScratchSize = 0;
	} 
	else
	{
	    igsColorExpandScratchSize = 1024;
	}
#endif	
	/* Use the allocation function to now get an address that
	 * points to the top of ram. As it won't be used again in
	 * this acceleration scheme, we really don't care about the
	 * mess this makes of the allocation
	 */
	igsCacheEnd = igsAllocate(0, 0x0);
	igsAccelInit();
    }
#endif /* !defined(MONOVGA) && !defined(XF86VGA16) */    
#ifdef DEBUG
    ErrorF("IGSFbInit: exit\n");
#endif	
}


static int
IGSValidMode(mode, verbose, flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
    /*
     * Code to check if a mode is suitable for the selected chipset.
     * In most cases this can just return MODE_OK.
     */

    return(MODE_OK);
}


/*
** igsDumpRegs - Prints complete dump of chip registers for debug
*/
static void
igsDumpRegs()
{
    int i, j;
    unsigned char temp;
        
    ErrorF("CyberPro 2010 Register Contents:\n");

    ErrorF(" POS102 - 0x%02x\n", inb(PORT102));
    ErrorF(" ST00   - 0x%02x\n", inb(MISCWRITE));
    ErrorF(" ST01   - 0x%02x\n", inb(igsST01Reg));
    ErrorF(" MSR    - 0x%02x\n", inb(MISCREAD));
    
    ErrorF("Sequencer Registers -\n");
    for (i = 0; i < 8; i++)
    {
	outb(SEQINDEX, i);
	ErrorF(" SR%02X   - 0x%02x\n", i, inb(SEQDATA));
    }
    
    ErrorF("Graphics Registers -\n");
    for (i = 0; i < 9; i++)
    {
	outb(GRAINDEX, i);
	ErrorF(" GR%02X   - 0x%02x\n", i, inb(GRADATA));
    }
    
    ErrorF("Attribute Registers -\n");
    for (i = 0; i < 0x14; i++)
    {
	inb(igsST01Reg); /* reset flip flop */
	outb(ATTRINDEX, i);
	ErrorF(" AR%02X   - 0x%02x\n", i, inb(ATTRDATAR));
    }
    /* Restore PAS! */
    inb(igsST01Reg);
    outb(ATTRINDEX, 0x20);
    
    ErrorF("DAC Registers -\n");
    /* Don't dump colours */
    ErrorF(" DACMASK - 0x%02x\n", inb(RAMDACMASK));
    ErrorF(" DACSTAT - 0x%02x\n", inb(RAMDACINDEXR));
    
    ErrorF("CRT Registers -\n");
    for (i = 0; i < 0x27; i++)
    {
	outb(igsCRTIndex, i);
	ErrorF(" CR%02X   - 0x%02x\n", i, inb(igsCRTData));
    }
    
    ErrorF("Extended CRT (TV) Registers -\n");
    for (i = 0x40; i < 0x48; i++)
    {
	outb(igsCRTIndex, i);
	ErrorF(" CR%02X   - 0x%02x\n", i, inb(igsCRTData));
    }
    
    ErrorF("Extended Registers -\n");
    for (i = 0x10; i < 0x40; i++)
    {
	outb(EXTINDEX, i);
	ErrorF(" EX%02X   - 0x%02x\n", i, inb(EXTDATA));
    }
    outb(EXTINDEX, 0xFA);
    temp = inb(EXTDATA);
    
    for (j = 0; j < 6; j++)
    {
	outb(EXTINDEX, 0xFA);
	outb(EXTDATA, j);
	switch (j)
	{
	    case 0:
		ErrorF("<X>\n");
		break;
	    case 1:
		ErrorF("<Y>\n");
		for(i = 0xAD; i < 0xB0; i++)
		{
		    outb(EXTINDEX, i);
		    ErrorF(" EX%02X  - 0x%02x\n", i, inb(EXTDATA));
		}
		break;
	    case 2:
		ErrorF("<W>\n");
		break;
	    case 3:
		ErrorF("<T>\n");
		break;
	    case 4:
		ErrorF("<J>\n");
		break;
	    default:
		ErrorF("<K>\n");
		break;
	}
	for (i = 0x40; i < 0x50; i++) 
	{
	    outb(EXTINDEX, i);
	    ErrorF(" EX%02X  - 0x%02x\n", i, inb(EXTDATA));
	}
    }
    outb(EXTINDEX, 0xFA);
    outb(EXTDATA, temp);

    for (i = 0x50; i < 0xFE; i++)
    {
	outb(EXTINDEX, i);
	ErrorF(" EX%02X  - 0x%02x\n", i, inb(EXTDATA));
    }
    
    ErrorF("End igsDumpRegs\n");
}


/*
** IgsClockSave: Saves registers related to clock parameters.
*/
static void
igsClockSave(Clock)
    igsClockPtr Clock;
{
    unsigned char temp;

#ifdef DEBUG
    ErrorF("igsClockSave\n");
#endif

    Clock->msr = (inb(MISCREAD) & 0xFE);/* save the standard VGA clock 
				      	 * registers */
    if (!igsCurrentClock) 
    {	     /* save console clock         */
	outb(EXTINDEX, 0xB0);
	igsConsole_clk[0] = inb(EXTDATA);
	outb(EXTINDEX, 0xB1);
	igsConsole_clk[1] = inb(EXTDATA);
    }
    
    Clock->Clock = igsCurrentClock;     /* save current clock frequency */
#ifdef DEBUG
    ErrorF("igsClockSave: params = 0x%02x, 0x%02x \n",
	   igsConsole_clk[0], igsConsole_clk[1]);
#endif
}


/*
** igsClockLoad: Writes clock values to appropriate registers
*/
static void
igsClockLoad(Clock)
    igsClockPtr Clock;
{
    volatile unsigned char temp;
    unsigned char vclk[2];

#ifdef DEBUG
    ErrorF("igsClockLoad\n");
#endif

    /* select fixed clock 0  before tampering with VCLK select */
    temp = inb(MISCREAD);
    outb(MISCWRITE, (temp & ~0x0D) | igsVgaIOBaseFlag);
		
    if (!Clock->Clock) {       /* Hack to load saved console clock */
	outb(EXTINDEX, 0xB0);
	outb(EXTDATA, (igsConsole_clk[0] & 0xFF));
	outb(EXTINDEX, 0xB1);
	outb(EXTDATA, (igsConsole_clk[1] & 0xFF));
    } else {
	igsCalcClock(Clock->Clock, vclk);
	outb(EXTINDEX, 0xB0);
	outb(EXTDATA, vclk[0]);
	outb(EXTINDEX, 0xB1);
	outb(EXTDATA, vclk[1]);
    }
	 
    /* Toggle B9 to latch values */
    outb(EXTINDEX, 0xB9);
    temp = inb(EXTDATA);
    outb(EXTDATA, (temp & ~0x80));
    outb(EXTDATA, (temp | 0x80));
    outb(EXTDATA, (temp & ~0x80));
    usleep(10000);		         /* Let VCO stabilise    */
    
#ifdef IO_DEBUG
    ErrorF("igsClockLoad: 0x3C2: %X ->",inb(MISCREAD));
#endif
    outb(MISCWRITE, (Clock->msr & 0xFE) | igsVgaIOBaseFlag);
#ifdef IO_DEBUG
    ErrorF(" %X\n",inb(MISCREAD));
#endif
    igsCurrentClock = Clock->Clock;
#ifdef DEBUG
    ErrorF("igsClockLoad: restored\n");
#endif
}


static Bool
igsClockFind(no, Clock)
    int no;
    igsClockPtr Clock;
{
#ifdef DEBUG
    ErrorF("igsClockFind %d\n", no);
#endif

    if (no > (vga256InfoRec.clocks - 1))
	return (FALSE);

    Clock->msr = 3 << 2;
    Clock->Clock = vga256InfoRec.clock[no];
    
    Clock->msr |= (inb(MISCREAD) & 0xF2);

#ifdef DEBUG
    ErrorF("igsClockFind: found\n");
#endif
    return (TRUE);
}


/* 
 * Calculates best clock values.
 * This is Ken Raeburn's <raeburn@raeburn.org> CT655XX clock
 * calculation code just modified a little bit to fit in here.
 * The IGS CyberPro2010 clock programming function differs from
 * the C&T one in the following ways:
 *   1. There is no reference clock divide selection (always 1)
 *   2. Don't multiply M by 4
 *   3. Subtract 1 from both M and N before setting registers
 *   4. Set vfsel according to VCO value (doesn't affect equation)
 * So the relevant rules for CyberPro2010 are:
 *   VCO = 14.31818 * (M + 1) / (N + 1)
 *   Vout = VCO / PD
 *   115MHz <= VCO <= 260MHz
 *   If VCO >= 180 then vfsel = 1 else vfsel = 0
 */
static void
igsCalcClock(Clock, vclk)
    int Clock; 
    unsigned char *vclk;
{
#ifndef	IGS_FIXED_CLOCKS
    int M, N, P, PSN;

    int bestM = 0;
    int bestN = 0;
    int bestP = 0;
    double bestError, abest = 42, bestFout, bestVco;
    double target;

    double Fvco, Fout;
    double Fvco_min = 115.0e6;
    double Fvco_max = 260.0e6;
    double error, aerror;
    register int Fref = FREF;
    
    int M_min = 4;
    int M_max = 255;
    int low_N, high_N;
    
    target = Clock * 1000;
    
    low_N = 2;
    high_N = 32;
    
    for (N = low_N; N <= high_N; N++) 
    {
	double tmp = Fref / N;
	
	for (P = 0; P <= 3; P++) 
	{	
	    double Fvco_desired = target * (P == 0 ? 1 : 2 * P);
	    
	    double M_desired = Fvco_desired / tmp;
	    
	    /* Which way will M_desired be rounded?  Do all three just to
	     * be safe.  */
	    int M_low = M_desired - 1;
	    int M_hi = M_desired + 1;
	    
	    if (M_hi < M_min || M_low > M_max)
		continue;
	    
	    if (M_low < M_min)
		M_low = M_min;
	    if (M_hi > M_max)
		M_hi = M_max;
	    
	    for (M = M_low; M <= M_hi; M++) 
	    {
		Fvco = tmp * M;
		if (Fvco < Fvco_min)
		{
		    continue;
		}
		
		if (Fvco > Fvco_max)
		{
		    break;
		}
		
		Fout = Fvco / (P == 0 ? 1 : 2 * P);
		
		error = (target - Fout) / target;
		
		aerror = (error < 0) ? -error : error;
		if (aerror < abest) 
		{
		    abest = aerror;
		    bestError = error;
		    bestM = M;
		    bestN = N;
		    bestP = P;
		    bestFout = Fout;
		    bestVco = Fvco;
		}
	    }
	}
    }
    
    vclk[0] = bestM - 1;
    vclk[1] = (((bestN - 1) & 0x1F) + (bestP << 6)) 
	| (bestVco >= 180.0e6 ? 0x20 : 0x00);
    
#ifdef DEBUG
    ErrorF("Freq. selected: %.2f MHz, vclk[0]=%X, vclk[1]=%X\n",
	   (float)(Clock / 1000.), vclk[0], vclk[1]);
    ErrorF(" M = %d, N = %d, PD = %d\n", bestM, bestN, bestP);
    ErrorF("Freq. set: %.2f MHz\n", bestFout / 1.0e6);
#endif
#else
    switch(Clock) {
	case 25175:
	case 25180:
	    vclk[0] = 0xD2;
	    vclk[1] = 0xD3;
	    break;

	case 28320:
	case 28322:
	    vclk[0] = 0xB1;
	    vclk[1] = 0xCE;
	    break;

	case 31500:
	    vclk[0] = 0x41;
	    vclk[1] = 0xE4;
	    break;

	case 50000:
	    vclk[0] = 0xFA;
	    vclk[1] = 0xB1;
	    break;

	case 80000:
	    vclk[0] = 0xBD;
	    vclk[1] = 0x50;
	    break;

	case 110000:
	    vclk[0] = 0xA8;
	    vclk[1] = 0x6A;
	    break;

	case 135000:
	    vclk[0] = 0x41;
	    vclk[1] = 0x06;
	    break;

	case 206000:
	    vclk[0] = 0xBA;
	    vclk[1] = 0x2C;
	    break;

	default:
	    FatalError("igsCalcClock:  unsupported fixed clock %d\n", Clock);
    }
    
#ifdef DEBUG
    ErrorF("Freq. selected: %.2f MHz, vclk[0]=%X, vclk[1]=%X\n",
	   (float)(Clock / 1000.), vclk[0], vclk[1]);
#endif
#endif
}

/* Wait for vertical Sync */
void igsWaitVSync()
{
    while (((inb(igsST01Reg)) & 0x08) == 0x08){};/* wait VSync off */
    while (((inb(igsST01Reg)) & 0x08) == 0 ) {}; /* wait VSync on */
    outw(0x3C4,0x07);              /* reset hsync - just in case...*/
}

/* $XConsortium: mga_driver.c /main/12 1996/10/28 05:13:26 kaleb $ */
/*
 * MGA Millennium (MGA2064W) with Ti3026 RAMDAC driver v.1.1
 *
 * The driver is written without any chip documentation. All extended ports
 * and registers come from tracing the VESA-ROM functions.
 * The BitBlt Engine comes from tracing the windows BitBlt function.
 *
 * Author:	Radoslaw Kapitan, Tarnow, Poland
 *			kapitan@student.uci.agh.edu.pl
 *		original source
 *
 * Now that MATROX has released documentation to the public, enhancing
 * this driver has become much easier. Nevertheless, this work continues
 * to be based on Radoslaw's original source
 *
 * Contributors:
 *		Andrew Vanderstock, Melbourne, Australia
 *			vanderaj@mail2.svhm.org.au
 *		additions, corrections, cleanups
 *
 *		Dirk Hohndel
 *			hohndel@XFree86.Org
 *		integrated into XFree86-3.1.2Gg
 *		fixed some problems with PCI probing and mapping
 *
 *		David Dawes
 *			dawes@XFree86.Org
 *		some cleanups, and fixed some problems
 *
 *		Andrew E. Mileski
 *			aem@ott.hookup.net
 *		RAMDAC timing, and BIOS stuff
 *
 *		Leonard N. Zubkoff
 *			lnz@dandelion.com
 *		Support for 8MB boards, RGB Sync-on-Green, and DPMS.
 */
 
/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/mga/mga_driver.c,v 1.1.2.11 1997/05/27 12:02:51 dawes Exp $ */

#include "X.h"
#include "input.h"
#include "screenint.h"

#include "compiler.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "xf86_PCI.h"
#include "vga.h"
#include "vgaPCI.h"

#ifdef XFreeXDGA
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

#include "mga_bios.h"
#include "mga_reg.h"
#include "mga.h"

extern vgaPCIInformation *vgaPCIInfo;

/*
 * Driver data structures.
 */
MGABiosInfo MGABios;
pciTagRec MGAPciTag;
int MGAchipset;
int MGAinterleave;
int MGABppShft;
int MGAusefbitblt;
int MGAydstorg;
unsigned char* MGAMMIOBase = NULL;
#ifdef __alpha__
unsigned char* MGAMMIOBaseDENSE = NULL;
#endif

/*
 * Forward definitions for the functions that make up the driver.
 */

static Bool		MGAProbe();
static char *		MGAIdent();
static void		MGAEnterLeave();
static Bool		MGAInit();
static Bool		MGAValidMode();
static void *		MGASave();
static void		MGARestore();
static void		MGAAdjust();
static void		MGAFbInit();
static int		MGAPitchAdjust();
static int		MGALinearOffset();
static void		MGADisplayPowerManagementSet();

/*
 * This data structure defines the driver itself.
 */
vgaVideoChipRec MGA = {
	/* 
	 * Function pointers
	 */
	MGAProbe,
	MGAIdent,
	MGAEnterLeave,
	MGAInit,
	MGAValidMode,
	MGASave,
	MGARestore,
	MGAAdjust,
	vgaHWSaveScreen,
	(void (*)())NoopDDA,	/* GetMode, */
	MGAFbInit,
	(void (*)())NoopDDA,	/* SetRead, */
	(void (*)())NoopDDA,	/* SetWrite, */
	(void (*)())NoopDDA,	/* SetReadWrite, */
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
	 * seperate read and write bank registers.	Almost all chipsets
	 * support two banks, and two banks are almost always faster
	 */
	FALSE,
	/*
	 * The chipset requires vertical timing numbers to be divided
	 * by two for interlaced modes
	 */
	VGA_DIVIDE_VERT,
	/*
	 * This is a dummy initialization for the set of option flags
	 * that this driver supports.	It gets filled in properly in the
	 * probe function, if the probe succeeds (assuming the driver
	 * supports any such flags).
	 */
	{0,},
	/*
	 * This determines the multiple to which the virtual width of
	 * the display must be rounded for the 256-color server. 
	 */
	0,
	/*
	 * If the driver includes support for a linear-mapped frame buffer
	 * for the detected configuratio this should be set to TRUE in the
	 * Probe or FbInit function. 
	 */
	TRUE,
	/*
	 * This is the physical base address of the linear-mapped frame
	 * buffer (when used).	Set it to 0 when not in use.
	 */
	0,
	/*
	 * This is the size of the linear-mapped frame buffer (when used).
	 * Set it to 0 when not in use.
	 */
	0,
	/*
	 * This is TRUE if the driver has support for the given depth for 
	 * the detected configuration. It must be set in the Probe function.
	 * It most cases it should be FALSE.
	 */
	TRUE,	/* 16bpp */
	TRUE,	/* 24bpp */
	TRUE,	/* 32bpp */
	/*
	 * This is a pointer to a list of builtin driver modes.
	 * This is rarely used, and in must cases, set it to NULL
	 */
	NULL,
	/*
	 * This is a factor that can be used to scale the raw clocks
	 * to pixel clocks.	 This is rarely used, and in most cases, set
	 * it to 1.
	 */
	1,		/* ChipClockMulFactor */
	1		/* ChipClockDivFactor */
};

/* 
 * ramdac info structure initialization
 */
MGARamdacRec MGAdac = {
	FALSE, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	90000, /* maxPixelClock */
	0
}; 

/*
 * array of ports
 */ 
static unsigned mgaExtPorts[] =
{
	0x400			/* This is enough to enable all ports */
};

static int Num_mgaExtPorts =
	(sizeof(mgaExtPorts)/sizeof(mgaExtPorts[0]));

/*
 * MGAReadBios - Read the video BIOS info block.
 *
 * DESCRIPTION
 *   Warning! This code currently does not detect a video BIOS.
 *   In the future, support for motherboards with the mga2064w
 *   will be added (no video BIOS) - this is not a huge concern
 *   for me today though.
 *
 * EXTERNAL REFERENCES
 *   vga256InfoRec.BIOSbase	IN	Physical address of video BIOS.
 *   MGABios			OUT	The video BIOS info block.
 *
 * HISTORY
 *   January 11, 1997 - [aem] Andrew E. Mileski
 *   Set default values for GCLK (= MCLK / pre-scale ).
 *
 *   October 7, 1996 - [aem] Andrew E. Mileski
 *   Written and tested.
 */ 
static void
MGAReadBios()
{
	CARD8 tmp[ 64 ];
	CARD16 offset;
	int i;

	/* Make sure the BIOS is present */
	xf86ReadBIOS( vga256InfoRec.BIOSbase, 0, tmp, sizeof( tmp ));
	if (
		tmp[ 0 ] != 0x55
		|| tmp[ 1 ] != 0xaa
		|| strncmp(( char * )( tmp + 45 ), "MATROX", 6 )
	) {
		ErrorF( "%s %s: Video BIOS info block not detected!" );
		return;
	}

	/* Get the info block offset */
	xf86ReadBIOS( vga256InfoRec.BIOSbase, 0x7ffc,
		( CARD8 * ) & offset, sizeof( offset ));

	/* Copy the info block */
	xf86ReadBIOS( vga256InfoRec.BIOSbase, offset,
		( CARD8 * ) & MGABios.StructLen, sizeof( MGABios ));

	/* Let the world know what we are up to */
	ErrorF( "%s %s: Video BIOS info block at 0x%08lx\n",
		XCONFIG_PROBED, vga256InfoRec.name,
		vga256InfoRec.BIOSbase + offset );	

	/* Set default MCLK values (scaled by 10 kHz) */
	if ( MGABios.ClkBase == 0 )
		MGABios.ClkBase = 4500;
	if ( MGABios.Clk4MB == 0 )
		MGABios.Clk4MB = MGABios.ClkBase;
	if ( MGABios.Clk8MB == 0 )
		MGABios.Clk8MB = MGABios.Clk4MB;
}

/*
 * MGACountRAM --
 *
 * Counts amount of installed RAM 
 */
static int
MGACountRam()
{
	if(MGA.ChipLinearBase)
	{
		volatile unsigned char* base;
		unsigned char tmp, tmp3, tmp5;
	
		base = xf86MapVidMem(vga256InfoRec.scrnIndex, LINEAR_REGION,
			      (pointer)((unsigned long)MGA.ChipLinearBase),
			      8192 * 1024);
	
		/* turn MGA mode on - enable linear frame buffer (CRTCEXT3) */
		outb(0x3DE, 3);
		tmp = inb(0x3DF);
		outb(0x3DF, tmp | 0x80);
	
		/* write, read and compare method */
		base[0x500000] = 0x55;
		base[0x300000] = 0x33;
		base[0x100000] = 0x11;
		tmp5 = base[0x500000];
		tmp3 = base[0x300000];

		/* restore CRTCEXT3 state */
		outb(0x3DE, 3);
		outb(0x3DF, tmp);
	
		xf86UnMapVidMem(vga256InfoRec.scrnIndex, LINEAR_REGION, 
				(pointer)base, 8192 * 1024);
	
		if(tmp5 == 0x55)
			return 8192;
		if(tmp3 == 0x33)
			return 4096;
	}
	return 2048;
}

/*
 * MGAIdent --
 *
 * Returns the string name for supported chipset 'n'. 
 */
static char *
MGAIdent(n)
int n;
{
	static char *chipsets[] = {"mga2064w", "mga1064sg"};

	if (n + 1 > sizeof(chipsets) / sizeof(char *))
		return(NULL);
	else
		return(chipsets[n]);
}

/*
 * MGAProbe --
 *
 * This is the function that makes a yes/no decision about whether or not
 * a chipset supported by this driver is present or not. 
 */
static Bool
MGAProbe()
{
	unsigned long MGAMMIOAddr = 0;
	pciConfigPtr pcr = NULL;
	int i;

	/*
	 * First we attempt to figure out if one of the supported chipsets
	 * is present.
	 */
	if (vga256InfoRec.chipset) {
		char *chipset;
		for (i = 0; (chipset = MGAIdent(i)); i++) {
			if (!StrCaseCmp(vga256InfoRec.chipset, chipset))
				break;
		}
		if (!chipset)
			return FALSE;
	}

	MGAchipset = 0;
	i = 0;
	if (vgaPCIInfo && vgaPCIInfo->AllCards) {
	  while (pcr = vgaPCIInfo->AllCards[i++]) {
		if (pcr->_vendor == PCI_VENDOR_MATROX) {
			switch(pcr->_device) {
				case PCI_CHIP_MGA2064:
					MGAchipset = pcr->_device;
					vga256InfoRec.chipset = MGAIdent(0);
				break;
				case PCI_CHIP_MGA1064:
					MGAchipset = pcr->_device;
					vga256InfoRec.chipset = MGAIdent(1);
				break;
			}
			if (MGAchipset)
				break;
		}
	  }
	} else return(FALSE);

	if (!MGAchipset) {
		if (vga256InfoRec.chipset)
			ErrorF("%s %s: MGA: unknown chipset\n",
				XCONFIG_PROBED, vga256InfoRec.name);
		return(FALSE);
	}

	/*
	 *	OK. It's MGA
	 */
	 
	MGAPciTag = pcibusTag(pcr->_bus, pcr->_cardnum, pcr->_func);

	/* ajv changes to reflect actual values. see sdk pp 3-2. */
	/* these masks just get rid of the crap in the lower bits */
	/* XXX - ajv I'm assuming that pcr->_base0 is pci config space + 0x10 */
	/*				and _base1 is another four bytes on */
	/* XXX - these values are Intel byte order I believe. */
	
	if ( pcr->_base0 )	/* details: mgabase1 sdk pp 4-11 */
		MGAMMIOAddr = pcr->_base0 & 0xffffc000;
	else
		MGAMMIOAddr = 0;
	
	if ( pcr->_base1 )	/* details: mgabase2 sdk pp 4-12 */
		MGA.ChipLinearBase = pcr->_base1 & 0xff800000;
	else
		MGA.ChipLinearBase = 0;

	/* Allow this to be overriden in the XF86Config file */
	if (vga256InfoRec.BIOSbase == 0) {
		if ( pcr->_baserom )	/* details: rombase sdk pp 4-15 */
			vga256InfoRec.BIOSbase = pcr->_baserom & 0xffff0000;
		else
			vga256InfoRec.BIOSbase = 0xc0000;
	}
	if (vga256InfoRec.MemBase)
		MGA.ChipLinearBase = vga256InfoRec.MemBase;
	if (vga256InfoRec.IObase)
		MGAMMIOAddr = vga256InfoRec.IObase;
		
	if (!MGA.ChipLinearBase)
		FatalError("MGA: Can't detect linear framebuffer address\n");
	if (!MGAMMIOAddr)
		FatalError("MGA: Can't detect IO registers address\n");
	
	if (xf86Verbose)
	{
		ErrorF("%s %s: Linear framebuffer at 0x%lX\n", 
			vga256InfoRec.MemBase? XCONFIG_GIVEN : XCONFIG_PROBED,
			vga256InfoRec.name, MGA.ChipLinearBase);
		ErrorF("%s %s: MMIO registers at 0x%lX\n", 
			vga256InfoRec.IObase? XCONFIG_GIVEN : XCONFIG_PROBED,
			vga256InfoRec.name, MGAMMIOAddr);
	}
	
	/*
	 * Map IO registers to virtual address space
	 */ 
	MGAMMIOBase =
#if defined(__alpha__)
			/* for Alpha, we need to map SPARSE memory,
	     		since we need byte/short access */
			  xf86MapVidMemSparse(
#else /* __alpha__ */
			  xf86MapVidMem(
#endif /* __alpha__ */
			    vga256InfoRec.scrnIndex, MMIO_REGION,
			    (pointer)(MGAMMIOAddr), 0x4000);
#ifdef __alpha__
	MGAMMIOBaseDENSE =
	  /* for Alpha, we need to map DENSE memory
	     as well, for setting CPUToScreenColorExpandBase
	   */
		  xf86MapVidMem(
			    vga256InfoRec.scrnIndex,
			    MMIO_REGION,
			    (pointer)(MGAMMIOAddr), 0x4000);
#endif /* __alpha__ */

	if (!MGAMMIOBase)
		FatalError("MGA: Can't map IO registers\n");
	
	/*
	 * Read the BIOS data struct
	 */
	MGAReadBios();
#ifdef DEBUG
	ErrorF("MGABios.RamdacType = 0x%x\n",MGABios.RamdacType);
#endif
	
	/*
	 * Set up I/O ports to be used by this card.
	 */
	xf86ClearIOPortList(vga256InfoRec.scrnIndex);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
	xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_mgaExtPorts,
				mgaExtPorts);

	/* enable IO ports, etc. */
	MGAEnterLeave(ENTER);
	
	/*
	 * If the user has specified the amount of memory in the XF86Config
	 * file, we respect that setting.
	 */
	if (!vga256InfoRec.videoRam)
		vga256InfoRec.videoRam = MGACountRam();
	
	MGA.ChipLinearSize = vga256InfoRec.videoRam * 1024;
	
	/*
	 * fill MGAdac struct
	 * Warning: currently, it should be after RAM counting
	 */
	switch (MGAchipset)
	{
	case PCI_CHIP_MGA2064:
		MGA3026RamdacInit();
		break;
	case PCI_CHIP_MGA1064:
		MGA1064RamdacInit();
		break;
	}
	
	/*
	 * If the user has specified ramdac speed in the XF86Config
	 * file, we respect that setting.
	 */
	if( vga256InfoRec.dacSpeeds[0] )
		vga256InfoRec.maxClock = vga256InfoRec.dacSpeeds[0];
	else
		vga256InfoRec.maxClock = MGAdac.maxPixelClock;

	/*
	 * Last we fill in the remaining data structures. 
	 */
	vga256InfoRec.bankedMono = FALSE;
	
#ifdef XFreeXDGA
    	vga256InfoRec.directMode = XF86DGADirectPresent;
#endif
 
	OFLG_SET(OPTION_NOACCEL, &MGA.ChipOptionFlags);
	OFLG_SET(OPTION_SYNC_ON_GREEN, &MGA.ChipOptionFlags);
	OFLG_SET(OPTION_DAC_8_BIT, &MGA.ChipOptionFlags);
	OFLG_SET(OPTION_SW_CURSOR, &MGA.ChipOptionFlags);

	OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions);
	OFLG_SET(OPTION_DAC_8_BIT, &vga256InfoRec.options);

	/* Moved width checking because virtualX isn't set until after
	   the probing.  Instead, make use of the newly added
	   PitchAdjust hook. */

	vgaSetPitchAdjustHook(MGAPitchAdjust);

	vgaSetLinearOffsetHook(MGALinearOffset);

#ifdef DPMSExtension
	vga256InfoRec.DPMSSet = MGADisplayPowerManagementSet;
#endif

	return(TRUE);
}

/*
 * TestAndSetRounding
 *
 * used in MGAPitchAdjust (see there) - ansi
 */

static int
TestAndSetRounding(pitch)
	int pitch;
{
	MGAinterleave = (vga256InfoRec.videoRam > 2048);
		
	/* we can't use interleave on Mystique */
	if (MGAchipset == PCI_CHIP_MGA1064) {
		MGAinterleave = 0;
	}
		
	switch (vgaBitsPerPixel)
	{
	case 8:
		if (MGAinterleave) {
			MGA.ChipRounding = 128;
			MGABppShft = 0;
		} else {
			MGA.ChipRounding = 64;
			MGABppShft = 1;
		}
		break;
	case 16:
		if (MGAinterleave) {
			MGA.ChipRounding = 64;
			MGABppShft = 1;
		} else {
			MGA.ChipRounding = 32;
			MGABppShft = 2;
		}
		break;
	case 32:
		if (MGAinterleave) {
			MGA.ChipRounding = 32;
			MGABppShft = 2;
		} else {
			MGA.ChipRounding = 16;
			MGABppShft = 3;
		}
		break;
	case 24:
		if (MGAinterleave) {
			MGA.ChipRounding = 128;
			MGABppShft = 0;
		} else {
			MGA.ChipRounding = 64;
			MGABppShft = 1;
		}
		break;
	}

	if (MGAchipset == PCI_CHIP_MGA1064) {
		MGABppShft--;
	}
	
	if (pitch % MGA.ChipRounding)
		pitch = pitch + MGA.ChipRounding - (pitch % MGA.ChipRounding);

#ifdef DEBUG
	ErrorF("pitch= %x MGA.ChipRounding= %x MGAinterleave= %x MGABppShft= %x\n",pitch ,MGA.ChipRounding,MGAinterleave,MGABppShft);
#endif
	return pitch;
}

/*
 * MGAPitchAdjust --
 *
 * This function adjusts the display width (pitch) once the virtual
 * width is known.  It returns the display width.
 */
static int
MGAPitchAdjust()
{
	int pitch = 0;
	int accel;
	
	/* XXX ajv - 512, 576, and 1536 may not be supported
	   virtual resolutions. see sdk pp 4-59 for more
	   details. Why anyone would want less than 640 is 
	   bizarre. (maybe lots of pixels tall?) */

#if 0		
	int width[] = { 512, 576, 640, 768, 800, 960, 
			1024, 1152, 1280, 1536, 1600, 1920, 2048, 0 };
#else
	int width[] = { 640, 768, 800, 960, 1024, 1152, 1280,
			1600, 1920, 2048, 0 };
#endif
	int i;

	if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))
	{
		accel = TRUE;
		
		for (i = 0; width[i]; i++)
		{
			if (width[i] >= vga256InfoRec.virtualX && 
			    TestAndSetRounding(width[i]) == width[i])
			{
				pitch = width[i];
				break;
			}
		}
	}
	else
	{
		accel = FALSE;
		pitch = TestAndSetRounding(vga256InfoRec.virtualX);
	}


	if (!pitch)
	{
		if(accel) 
		{
			FatalError("MGA: Can't find pitch, try using option"
				   "\"no_accel\"\n");
		}
		else
		{
			FatalError("MGA: Can't find pitch (Oups, should not"
				   "happen!)\n");
		}
	}

	if (pitch != vga256InfoRec.virtualX)
	{
		if (accel)
		{
			ErrorF("%s %s: Display pitch set to %d (a multiple "
			       "of %d & possible for acceleration)\n",
			       XCONFIG_PROBED, vga256InfoRec.name,
			       pitch, MGA.ChipRounding);
		}
		else
		{
			ErrorF("%s %s: Display pitch set to %d (a multiple "
			       "of %d)\n",
			       XCONFIG_PROBED, vga256InfoRec.name,
			       pitch, MGA.ChipRounding);
		}
	}
#ifdef DEBUG
	else
	{
		ErrorF("%s %s: pitch is %d, virtual x is %d, display width is %d\n", XCONFIG_PROBED, vga256InfoRec.name,
		       pitch, vga256InfoRec.virtualX, vga256InfoRec.displayWidth);
	}
#endif

	return pitch;
}

/*
 * MGALinearOffset --
 *
 * This function computes the byte offset into the linear frame buffer where
 * the frame buffer data should actually begin.  According to DDK misc.c line
 * 1023, if more than 4MB is to be displayed, YDSTORG must be set appropriately
 * to align memory bank switching, and this requires a corresponding offset
 * on linear frame buffer access.
 */
static int
MGALinearOffset()
{
	int BytesPerPixel = vgaBitsPerPixel / 8;
	int offset, offset_modulo, ydstorg_modulo;

	MGAydstorg = 0;
	if (vga256InfoRec.virtualX * vga256InfoRec.virtualY * BytesPerPixel
		<= 4*1024*1024)
	    return 0;

	offset = (4*1024*1024) % (vga256InfoRec.displayWidth * BytesPerPixel);
	offset_modulo = 4;
	ydstorg_modulo = 64;
	if (vgaBitsPerPixel == 24)
	    offset_modulo *= 3;
	if (MGAinterleave)
	{
	    offset_modulo <<= 1;
	    ydstorg_modulo <<= 1;
	}
	MGAydstorg = offset / BytesPerPixel;
	while ((offset % offset_modulo) != 0 ||
	       (MGAydstorg % ydstorg_modulo) != 0)
	{
	    offset++;
	    MGAydstorg = offset / BytesPerPixel;
	}

	return MGAydstorg * BytesPerPixel;
}


/*
 * MGAFbInit --
 *
 * This function is used to initialise chip-specific graphics functions.
 * It can be used to make use of the accelerated features of some chipsets.
 */
static void
MGAFbInit()
{
	if (MGAdac.MemoryClock && xf86Verbose)
	{
	    ErrorF("%s %s: MCLK set to %1.3f MHz\n",
	        /*vga256InfoRec.MemClk? XCONFIG_GIVEN :*/ XCONFIG_PROBED,
	        vga256InfoRec.name, MGAdac.MemoryClock / 1000.0);
	}
	
	if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))
	{
	        /*
		 * Hardware cursor
		 */
	        if ( !OFLG_ISSET(OPTION_SW_CURSOR, &vga256InfoRec.options)) {
		    if (MGAHwCursorInit())
		        ErrorF("%s %s: Using hardware cursor\n",
			   XCONFIG_PROBED, vga256InfoRec.name);
		}	
		else
		    ErrorF("%s %s: Disabling hardware cursor\n",
			   XCONFIG_GIVEN, vga256InfoRec.name);

		/*
		 * now call the new acc interface
		 */
		if (MGAchipset == PCI_CHIP_MGA1064 )  {
			MGAusefbitblt = 0;
			} else {
			MGAusefbitblt = !(MGABios.FeatFlag & 0x00000001);
		}
		MGAStormAccelInit();
	}
}

/*
 * MGAInit --
 *
 * The 'mode' parameter describes the video mode.	The 'mode' structure 
 * as well as the 'vga256InfoRec' structure can be dereferenced for
 * information that is needed to initialize the mode.	The 'new' macro
 * (see definition above) is used to simply fill in the structure.
 */
static Bool
MGAInit(mode)
DisplayModePtr mode;
{
	switch (MGAchipset)
	{
	case PCI_CHIP_MGA2064:
		return MGA3026Init(mode);
	case PCI_CHIP_MGA1064:                               
		return MGA1064Init(mode);
	}
}

/*
 * MGARestore --
 *
 * This function restores a video mode.	 It basically writes out all of
 * the registers that have previously been saved in the vgaMGARec data 
 * structure.
 */
static void 
MGARestore(restore)
vgaHWPtr restore;
{
	int i;

	vgaProtect(TRUE);
	
	switch (MGAchipset)
	{
	case PCI_CHIP_MGA2064:
		MGA3026Restore(restore);
		break;
	case PCI_CHIP_MGA1064:
		MGA1064Restore(restore);
		break;
	}

	MGAStormSync();
	MGAStormEngineInit();

	vgaProtect(FALSE);
}

/*
 * MGASave --
 *
 * This function saves the video state.	 It reads all of the SVGA registers
 * into the vgaMGARec data structure.
 */
static void *
MGASave(save)
vgaHWPtr save;
{
	switch (MGAchipset)
	{
	case PCI_CHIP_MGA2064:
		return (void *)MGA3026Save(save);
	case PCI_CHIP_MGA1064:
		return (void *)MGA1064Save(save);
	}
}

/*
 * MGAEnterLeave --
 *
 * This function is called when the virtual terminal on which the server
 * is running is entered or left, as well as when the server starts up
 * and is shut down.	Its function is to obtain and relinquish I/O 
 * permissions for the SVGA device.	 This includes unlocking access to
 * any registers that may be protected on the chipset, and locking those
 * registers again on exit.
 */
static void 
MGAEnterLeave(enter)
Bool enter;
{
	unsigned char misc_ctrl;
	unsigned char temp;

#ifdef XFreeXDGA
      	if (vga256InfoRec.directMode&XF86DGADirectGraphics && !enter) {
       		if (MGAdac.isHwCursor) {
       			MGAdac.CursorOff();
       		}
       		return;
   	}
#endif 

	if (enter)
	{
		xf86EnableIOPorts(vga256InfoRec.scrnIndex);
		if (MGAMMIOBase)
		{
			xf86MapDisplay(vga256InfoRec.scrnIndex,
					MMIO_REGION);
			MGAStormSync();
		}
		
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
		
		if (MGAMMIOBase)
		{
 			MGAStormSync();
			xf86UnMapDisplay(vga256InfoRec.scrnIndex,
					MMIO_REGION);
		}
		
		xf86DisableIOPorts(vga256InfoRec.scrnIndex);
	}
}

/*
 * MGAAdjust --
 *
 * This function is used to initialize the SVGA Start Address - the first
 * displayed location in the video memory.
 */
static void 
MGAAdjust(x, y)
int x, y;
{
	int Base = (y * vga256InfoRec.displayWidth + x + MGAydstorg) >>
			(3 - MGABppShft);
	int tmp;

	if (vgaBitsPerPixel == 24)
		Base *= 3;

	/* Wait for vertical retrace */
	while (!(inb(vgaIOBase + 0xA) & 0x08));
	
	outw(vgaIOBase + 4, (Base & 0x00FF00) | 0x0C);
	outw(vgaIOBase + 4, ((Base & 0x0000FF) << 8) | 0x0D);
	outb(0x3DE, 0x00);
	tmp = inb(0x3DF);
	outb(0x3DF, (tmp & 0xF0) | ((Base & 0x0F0000) >> 16));

#ifdef XFreeXDGA
	if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
	/* Wait for vertical retrace end */
		while (!(inb(vgaIOBase + 0xA) & 0x08));
		while (inb(vgaIOBase + 0xA) & 0x08);
	}
#endif
}

/*
 * MGAValidMode -- 
 *
 * Checks if a mode is suitable for the selected chipset.
 */
static Bool
MGAValidMode(mode,verbose,flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
	int lace = 1 + ((mode->Flags & V_INTERLACE) != 0);
	
	if ((mode->CrtcHDisplay <= 2048) &&
	    (mode->CrtcHSyncStart <= 4096) && 
	    (mode->CrtcHSyncEnd <= 4096) && 
	    (mode->CrtcHTotal <= 4096) &&
	    (mode->CrtcVDisplay <= 2048 * lace) &&
	    (mode->CrtcVSyncStart <= 4096 * lace) &&
	    (mode->CrtcVSyncEnd <= 4096 * lace) &&
	    (mode->CrtcVTotal <= 4096 * lace))
	{
		return(MODE_OK);
	}
	else
	{
		return(MODE_BAD);
	}
}

/*
 * MGADisplayPowerManagementSet --
 *
 * Sets VESA Display Power Management Signaling (DPMS) Mode.
 */
#ifdef DPMSExtension
static void MGADisplayPowerManagementSet(PowerManagementMode)
int PowerManagementMode;
{
	unsigned char seq1, crtcext1;
	if (!xf86VTSema) return;
	switch (PowerManagementMode)
	{
	case DPMSModeOn:
	    /* Screen: On; HSync: On, VSync: On */
	    seq1 = 0x00;
	    crtcext1 = 0x00;
	    break;
	case DPMSModeStandby:
	    /* Screen: Off; HSync: Off, VSync: On */
	    seq1 = 0x20;
	    crtcext1 = 0x10;
	    break;
	case DPMSModeSuspend:
	    /* Screen: Off; HSync: On, VSync: Off */
	    seq1 = 0x20;
	    crtcext1 = 0x20;
	    break;
	case DPMSModeOff:
	    /* Screen: Off; HSync: Off, VSync: Off */
	    seq1 = 0x20;
	    crtcext1 = 0x30;
	    break;
	}
	outb(0x3C4, 0x01);	/* Select SEQ1 */
	seq1 |= inb(0x3C5) & ~0x20;
	outb(0x3C5, seq1);
	outb(0x3DE, 0x01);	/* Select CRTCEXT1 */
	crtcext1 |= inb(0x3DF) & ~0x30;
	outb(0x3DF, crtcext1);
}
#endif

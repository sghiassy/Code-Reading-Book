/* $XConsortium: tga.c /main/11 1996/10/28 04:23:31 kaleb $ */
/*
 * Copyright 1995,96 by Alan Hourihane, Wigan, England.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Alan Hourihane not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Alan Hourihane makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALAN HOURIHANE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ALAN HOURIHANE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Alan Hourihane, <alanh@fairlite.demon.co.uk>
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/tga/tga.c,v 3.17.2.6 1997/06/01 12:33:32 dawes Exp $ */

#include "X.h"
#include "input.h"
#include "screenint.h"
#include "dix.h"
#include "cfb.h"
#include "cfb32.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Procs.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "tga.h"
#include "tga_presets.h"

#include "xf86xaa.h"

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

#ifdef DPMSExtension
#include "opaque.h"
#include "extensions/dpms.h"
#endif

static int tgaValidMode(
#if NeedFunctionPrototypes
    DisplayModePtr,
    Bool,
    int
#endif
);

ScrnInfoRec tgaInfoRec = {
    FALSE,		/* Bool configured */
    -1,			/* int tmpIndex */
    -1,			/* int scrnIndex */
    tgaProbe,      	/* Bool (* Probe)() */
    tgaInitialize,	/* Bool (* Init)() */
    tgaValidMode,	/* Bool (* ValidMode)() */
    tgaEnterLeaveVT,	/* void (* EnterLeaveVT)() */
    (void (*)())NoopDDA,/* void (* EnterLeaveMonitor)() */
    (void (*)())NoopDDA,/* void (* EnterLeaveCursor)() */
    tgaAdjustFrame,	/* void (* AdjustFrame)() */
    tgaSwitchMode,	/* Bool (* SwitchMode)() */
    tgaDPMSSet,		/* void (* DPMSSet)() */
    tgaPrintIdent,	/* void (* PrintIdent)() */
    8,			/* int depth */
    {5, 6, 5},          /* xrgb weight */
    8,			/* int bitsPerPixel */
    PseudoColor,       	/* int defaultVisual */
    -1, -1,		/* int virtualX,virtualY */
    -1,                 /* int displayWidth */
    -1, -1, -1, -1,	/* int frameX0, frameY0, frameX1, frameY1 */
    {0, },	       	/* OFlagSet options */
    {0, },	       	/* OFlagSet clockOptions */
    {0, },	       	/* OFlagSet xconfigFlag */
    NULL,	       	/* char *chipset */
    NULL,	       	/* char *ramdac */
    {0, 0, 0, 0},	/* int dacSpeeds[MAXDACSPEEDS] */
    0,			/* int dacSpeedBpp */
    0,			/* int clocks */
    {0, },		/* int clock[MAXCLOCKS] */
    0,			/* int maxClock */
    0,			/* int videoRam */
    0, 		        /* int BIOSbase */   
    0,			/* unsigned long MemBase */
    240, 180,		/* int width, height */
    0,                  /* unsigned long  speedup */
    NULL,	       	/* DisplayModePtr modes */
    NULL,	       	/* MonPtr monitor */
    NULL,               /* char *clockprog */
    -1,                 /* int textclock */   
    FALSE,              /* Bool bankedMono */
    "DEC_TGA",          /* char *name */
    {0, },		/* xrgb blackColour */
    {0, },		/* xrgb whiteColour */
    tgaValidTokens,	/* int *validTokens */
    TGA_PATCHLEVEL,	/* char *patchlevel */
    0,			/* int IObase */
    0,			/* int PALbase */
    0,			/* int COPbase */
    0,			/* int POSbase */
    0,			/* int instance */
    0,			/* int s3Madjust */
    0,			/* int s3Nadjust */
    0,			/* int s3MClk */
    0,			/* int chipID */
    0,			/* int chipRev */
    0,			/* unsigned long VGAbase */
    0,			/* int s3RefClk */
    -1,			/* int s3BlankDelay */
    0,			/* int textClockFreq */
    NULL,               /* char* DCConfig */
    NULL,               /* char* DCOptions */
    0			/* int MemClk */
#ifdef XFreeXDGA
    ,0,			/* int directMode */
    NULL,		/* Set Vid Page */
    0,			/* unsigned long physBase */
    0			/* int physSize */
#endif
};

extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern Bool xf86Exiting, xf86Resetting, xf86ProbeFailed;
ScreenPtr savepScreen = NULL;
Bool tgaDAC8Bit = FALSE;
Bool tgaBt485PixMux = FALSE;
Bool tgaReloadCursor, tgaBlockCursor;
unsigned char tgaSwapBits[256];
pointer tga_reg_base;
int tgahotX, tgahotY;
static PixmapPtr ppix = NULL;
int tga_type;
int tgaDisplayWidth;
pointer tgaVideoMem = NULL;
extern unsigned char *tgaVideoMemSave;
static tgaCRTCRegRec tgaCRTCRegs;
volatile unsigned long *VidBase;
#define tgaReorderSwapBits(a,b)		b = \
		(a & 0x80) >> 7 | \
		(a & 0x40) >> 5 | \
		(a & 0x20) >> 3 | \
		(a & 0x10) >> 1 | \
		(a & 0x08) << 1 | \
		(a & 0x04) << 3 | \
		(a & 0x02) << 5 | \
		(a & 0x01) << 7;

/*
 * tgaPrintIdent
 */

void
tgaPrintIdent()
{
	ErrorF("  %s: non-accelerated server for DEC 21030 TGA graphics adapters\n",
			tgaInfoRec.name);
	ErrorF("(Patchlevel %s)\n", tgaInfoRec.patchLevel);
}


/*
 * ICS1562ClockSelect --
 *      select one of the possible clocks ...
 */

Bool
ICS1562ClockSelect(freq)
     int freq;
{
  unsigned char pll_bits[7];
  unsigned long temp;
  int i, j;

  /*
   * For the DEC 21030 TGA, There lies an ICS1562 Clock Generator.
   * This requires the 55 clock bits be written in a serial manner to
   * bit 0 of the CLOCK register and on the 56th bit set the hold flag.
   */
  switch(freq)
  {
    case CLK_REG_SAVE:
      /* The clock register is a write only register */
      break;
    case CLK_REG_RESTORE:
      /* Therefore we don't know what the value is to read and restore */
      break;
    default:
      ICS1562_CalcClockBits(freq, pll_bits);
      for (i = 0;i <= 6; i++) {
	for (j = 0; j <= 7; j++) {
	  temp = (pll_bits[i] >> (7-j)) & 1;
	  if (i == 6 && j == 7)
	    temp |= 2;
	  TGA_WRITE_REG(temp, TGA_CLOCK_REG);
	}
      }
  }
  return(TRUE);
}

/*
 * tgaProbe --
 *      check up whether a TGA based board is installed
 */

Bool
tgaProbe()
{
  int i;
  Bool pModeInited = FALSE;
  pointer Base;
  DisplayModePtr pMode, pEnd;
  OFlagSet validOptions;
  int found_device = FALSE;
  char buf[80];
  char *find_dev = "DEC DC21030"; 
  char *find_string = "Prefetchable 32 bit memory";
  int fin = 0;
  FILE *fd;

  /* Find the base address of the TGA chip through /proc/pci */
  /* Not very elegant, but it does the job for now. */
  if (tgaInfoRec.MemBase == 0) {
  fd = fopen("/proc/pci", "r");
  if (fd != NULL) {
  	fgets(buf, 80, fd);	/* Grab first line */
  	do {
		if (strstr(buf, "Bus"))
			found_device = FALSE;
		if (strstr(buf, find_dev))
			found_device = TRUE;
		if (found_device)
	  		if (strstr(buf, find_string))
	   		    tgaInfoRec.MemBase = 
				strtoul(strstr(buf,"0x"),(char **)NULL,16);
		if (!fgets(buf, 80, fd)) fin = 1;	/* End of File */
  	} while (fin == 0);
  }
  }

  /*
   * DEC 21030 TGA is a memory mapped device only.....
   * Therefore we need to mmap device to do the probe.
   * We need PCI routines for the Alpha - We don't as yet have them.
   * Therefore we use MemBase from XF86Config to set base address.
   * But, if we have found it through /proc/pci - we use this.
   */
  if (tgaInfoRec.MemBase == 0)
  {
	FatalError("%s %s: MemBase needed for TGA support - "
		   "check /proc/pci for base address.\n",
			XCONFIG_PROBED, tgaInfoRec.name);
  }	

  Base = xf86MapVidMem(0,EXTENDED_REGION,(pointer)tgaInfoRec.MemBase,2097152);

  tga_reg_base = (pointer *)((char*)(Base) + TGA_REGS_OFFSET);
  tga_type = (*(unsigned int *)Base >> 12) & 0xf;

  /* Let's find out what kind of TGA chip we've got ! */
  /* We only support the 8 plane TGA with BT485 Ramdac - so there ! */
  switch (tga_type)
  {
	case TYPE_TGA_8PLANE:
		ErrorF("%s %s: DEC 21030 TGA 8 Plane Chip Found.\n",
			XCONFIG_PROBED, tgaInfoRec.name);
		break;
	case TYPE_TGA_24PLANE:
		ErrorF("%s %s: DEC 21030 TGA 24 Plane Chip Found.\n",
			XCONFIG_PROBED, tgaInfoRec.name);
		ErrorF("Sorry, but the 24 Plane Chip is not yet supported.\n");
		return(FALSE);
		break;
	case TYPE_TGA_24PLUSZ:
		ErrorF("%s %s: DEC 21030 TGA 24 Plane 3D Chip Found.\n",
			XCONFIG_PROBED, tgaInfoRec.name);
		ErrorF("Sorry, but the 24Plane3D Chip is not yet supported.\n");
		return(FALSE);
		break;
	default:
		ErrorF("%s %s: OUCH ! Found an unknown TGA Chip. Aborting..\n",
			XCONFIG_PROBED, tgaInfoRec.name);
		return(FALSE);
		break;
  }

  if (tgaInfoRec.videoRam == 0)
  {
	tgaInfoRec.videoRam = 2048;
  	ErrorF("%s %s: videoram : %dk\n", XCONFIG_PROBED, tgaInfoRec.name,
		tgaInfoRec.videoRam);
  }
  else
  {
  	ErrorF("%s %s: videoram : %dk\n", XCONFIG_GIVEN, tgaInfoRec.name,
		tgaInfoRec.videoRam);
  }

  /* There is an algorithm for the ICS 1562 */
  OFLG_SET(CLOCK_OPTION_ICS1562, &tgaInfoRec.clockOptions);
  OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &tgaInfoRec.clockOptions);
  ErrorF("%s %s: Using ICS1562 programmable clock\n", 
	 XCONFIG_PROBED, tgaInfoRec.name);


  /* Initialize options that reflect the TGA */
  OFLG_ZERO(&validOptions);

#ifdef NOTYET
  /* According to the 21030 manual - The Cursor of the 21030 is latched
   * through to the RAMDAC's own cursor, so it may be that both of these
   * are the same, but obviously we've got different methods of accessing
   * them. I guess that the UDB(Multia) has these latches.....
   */
  if (tga_type != TYPE_TGA_8PLANE)
	/* Use BT463 Ramdac cursor, utilizing 24Plane/24Plane3d chip */
  	OFLG_SET(OPTION_HW_CURSOR, &validOptions);
  else
  	/* Or - If we have an 8plane use BT485 HW cursor directly */
  	OFLG_SET(OPTION_BT485_CURS, &validOptions);
#endif

  OFLG_SET(OPTION_DAC_8_BIT, &validOptions);
  OFLG_SET(OPTION_DAC_8_BIT, &tgaInfoRec.options); /* Set 8bit by default */
  OFLG_SET(OPTION_DAC_6_BIT, &validOptions);
  OFLG_SET(OPTION_POWER_SAVER, &validOptions);

  xf86VerifyOptions(&validOptions, &tgaInfoRec);
  tgaInfoRec.chipset = "tga";
  xf86ProbeFailed = FALSE;

  tgaInfoRec.dacSpeeds[0] = 135000; 	/* 135MHz for the Bt485 */
  tgaInfoRec.maxClock = 135000;		/* 135MHz for the Bt485 */

  /* Let's grab the basic mode lines */
#ifdef NOTYET
  tx = tgaInfoRec.virtualX;
  ty = tgaInfoRec.virtualY;
#else
  if (tgaInfoRec.virtualX > 0) {
	ErrorF("%s %s: Virtual coordinates - Not yet supported. "
		   "Ignoring.\n", XCONFIG_GIVEN, tgaInfoRec.name);
  }
#endif
  pMode = tgaInfoRec.modes;
  if (pMode == NULL)
  { 
	ErrorF("No modes specified in the XF86Config file.\n");
	return (FALSE);
  }
  pEnd = (DisplayModePtr)NULL;
  do
  {
	DisplayModePtr pModeSv;

	pModeSv = pMode->next;
	
	/* Delete any invalid ones */
	if (xf86LookupMode(pMode, &tgaInfoRec, LOOKUP_DEFAULT) == FALSE) {
		pModeSv = pMode->next;
		xf86DeleteMode(&tgaInfoRec, pMode);
		pMode = pModeSv;
#ifdef NOTYET
	} else if (((tx > 0) && (pMode->HDisplay > tx)) ||
		   ((ty > 0) && (pMode->VDisplay > ty))) {
		pModeSv = pMode->next;
		ErrorF("%s %s: Resolution %dx%d too large for virtual %dx%d\n",
			XCONFIG_PROBED, tgaInfoRec.name,
			pMode->HDisplay, pMode->VDisplay, tx, ty);
		xf86DeleteMode(&tgaInfoRec, pMode);
		pMode = pModeSv;
#endif
	} else {
		if (pEnd == (DisplayModePtr) NULL)
			pEnd = pMode;

#ifdef NOTYET
		tgaInfoRec.virtualX = max(tgaInfoRec.virtualX,
						pMode->HDisplay);
		tgaInfoRec.virtualY = max(tgaInfoRec.virtualY,
						pMode->VDisplay);
#else
		if (pMode->HDisplay % 4)
		{
			pModeSv = pMode->next;
			ErrorF("%s %s: Horizontal Resolution %d not divisible"
				" by a factor of 4, removing modeline.\n",
				XCONFIG_GIVEN, tgaInfoRec.name,
				pMode->HDisplay);
			xf86DeleteMode(&tgaInfoRec, pMode);
			pMode = pModeSv;
		}
		else
		{
			tgaInfoRec.virtualX = pMode->HDisplay;
			tgaInfoRec.virtualY = pMode->VDisplay;
			pModeInited = TRUE; /* We have a mode - only 1 supported */
		}
#endif
		pMode = pMode->next;
	}
  } while (pModeInited == FALSE); /* (pMode != pEnd); */

  ErrorF("%s %s: TGA chipset currently only supports one modeline.\n",
		XCONFIG_PROBED, tgaInfoRec.name);

  tgaInfoRec.displayWidth = tgaInfoRec.virtualX;

  if (OFLG_ISSET(OPTION_DAC_8_BIT, &tgaInfoRec.options))
	tgaDAC8Bit = TRUE;

  if (OFLG_ISSET(OPTION_DAC_6_BIT, &tgaInfoRec.options))
	tgaDAC8Bit = FALSE;

#ifdef DPMSExtension
  if (DPMSEnabledSwitch ||
      (OFLG_ISSET(OPTION_POWER_SAVER, &tgaInfoRec.options) &&
       !DPMSDisabledSwitch))
	defaultDPMSEnabled = DPMSEnabled = TRUE;
#endif

  if (xf86bpp < 0)
	xf86bpp = tgaInfoRec.depth;
  if (xf86weight.red == 0 || xf86weight.green == 0 || xf86weight.blue == 0)
	xf86weight = tgaInfoRec.weight;
  switch (xf86bpp) {
	case 8:
		/* XAA uses this */
		xf86weight.green = (tgaDAC8Bit ? 8 : 6);
		break;
	default:
		ErrorF("Invalid value for bpp. 8bpp is only supported.\n");
		return(FALSE);
  }

#ifdef XFreeXDGA
#ifdef NOTYET
  tgaInfoRec.directMode = XF86DGADirectPresent;
#endif
#endif

  return(TRUE);
}

/*
 * tgaInitialize --
 *
 */

Bool
tgaInitialize (scr_index, pScreen, argc, argv)
	int		scr_index;
	ScreenPtr	pScreen;
	int		argc;
	char		**argv;
{
	int displayResolution = 75; 	/* default to 75dpi */
	int i;
	extern int monitorResolution;

	/* Init the screen */
	
	tgaInitAperture(scr_index);
	tgaInit(tgaInfoRec.modes);
	tgaCalcCRTCRegs(&tgaCRTCRegs, tgaInfoRec.modes);
	tgaSetCRTCRegs(&tgaCRTCRegs);
	tgaInitEnvironment();
	for (i = 0; i < 256; i++)
	{ 
		tgaReorderSwapBits(i, tgaSwapBits[i]);
	}

	/*
	 * Take display resolution from the -dpi flag 
	 */
	if (monitorResolution)
		displayResolution = monitorResolution;
	
#if 1
	/* Let's use the new XAA Architecture.....*/
 	TGAAccelInit();

	if (!xf86XAAScreenInit8bpp(pScreen,
#else
	if (!cfbScreenInit(pScreen,
#endif
			(pointer) tgaVideoMem,
			tgaInfoRec.virtualX, tgaInfoRec.virtualY,
			displayResolution, displayResolution,
			tgaInfoRec.displayWidth))
		return(FALSE);

	pScreen->whitePixel = (Pixel) 1;
	pScreen->blackPixel = (Pixel) 0;
	XF86FLIP_PIXELS();
	pScreen->CloseScreen = tgaCloseScreen;
	pScreen->SaveScreen = tgaSaveScreen;

	switch(tgaInfoRec.bitsPerPixel) {
		case 8:
			pScreen->InstallColormap = tgaInstallColormap;
			pScreen->UninstallColormap = tgaUninstallColormap;
			pScreen->ListInstalledColormaps = 
						tgaListInstalledColormaps;
			pScreen->StoreColors = tgaStoreColors;
			break;
		case 16:
		case 32:
			pScreen->InstallColormap = cfbInstallColormap;
			pScreen->UninstallColormap = cfbUninstallColormap;
			pScreen->ListInstalledColormaps = 
						cfbListInstalledColormaps;
			pScreen->StoreColors = (void (*)())NoopDDA;
			break;
	}

	if ( (OFLG_ISSET(OPTION_HW_CURSOR, &tgaInfoRec.options)) ||
	     (OFLG_ISSET(OPTION_BT485_CURS, &tgaInfoRec.options)) ) {
		pScreen->QueryBestSize = tgaQueryBestSize;
		xf86PointerScreenFuncs.WarpCursor = tgaWarpCursor;
		(void)tgaCursorInit(0, pScreen);
	} 
	else
	{
		miDCInitialize (pScreen, &xf86PointerScreenFuncs);
	}

	savepScreen = pScreen;
	return (cfbCreateDefColormap(pScreen));
}

/*
 *      Assign a new serial number to the window.
 *      Used to force GC validation on VT switch.
 */

/*ARGSUSED*/
static int
tgaNewSerialNumber(pWin, data)
    WindowPtr pWin;
    pointer data;
{
    pWin->drawable.serialNumber = NEXT_SERIAL_NUMBER;
    return WT_WALKCHILDREN;
}


/*
 * tgaEnterLeaveVT -- 
 *      grab/ungrab the current VT completely.
 */

void
tgaEnterLeaveVT(enter, screen_idx)
     Bool enter;
     int screen_idx;
{
    PixmapPtr pspix;
    ScreenPtr pScreen = savepScreen;

    if (!xf86Exiting && !xf86Resetting) {
        switch (tgaInfoRec.bitsPerPixel) {
        case 8:
            pspix = (PixmapPtr)pScreen->devPrivate;
            break;
        case 32:
	    pspix = (PixmapPtr)pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr;
            break;
        }
    }

    if (pScreen && !xf86Exiting && !xf86Resetting)
        WalkTree(pScreen, tgaNewSerialNumber, 0);

    if (enter) {
	xf86MapDisplay(screen_idx, LINEAR_REGION);
	if (!xf86Resetting) {
	    ScrnInfoPtr pScr = (ScrnInfoPtr)XF86SCRNINFO(pScreen);

	    tgaCalcCRTCRegs(&tgaCRTCRegs, tgaInfoRec.modes);
	    tgaSetCRTCRegs(&tgaCRTCRegs);
	    tgaInit(tgaInfoRec.modes);
	    tgaInitEnvironment();
	    tgaRestoreDACvalues();
	    tgaAdjustFrame(pScr->frameX0, pScr->frameY0);
	    tgaRestoreCursor(pScreen);

	    if (pspix->devPrivate.ptr != tgaVideoMem && ppix) {
		pspix->devPrivate.ptr = tgaVideoMem;
#if 1
		ppix->devPrivate.ptr = (pointer)tgaVideoMem;
#else
		(tgaImageWriteFunc)(0, 0, pScreen->width, pScreen->height,
				 ppix->devPrivate.ptr,
				 PixmapBytePad(pScreen->width,
					       pScreen->rootDepth),
				 0, 0, MIX_SRC, ~0);
#endif
	    }
	}
	if (ppix) {
	    (pScreen->DestroyPixmap)(ppix);
	    ppix = NULL;
	}
    } else {
	xf86MapDisplay(screen_idx, LINEAR_REGION);
	if (!xf86Exiting) {
	    ppix = (pScreen->CreatePixmap)(pScreen, tgaInfoRec.displayWidth,
					    pScreen->height,
					    pScreen->rootDepth);

	    if (ppix) {
#if 1
		ppix->devPrivate.ptr = (pointer)tgaVideoMemSave;
#else
		(tgaImageReadFunc)(0, 0, pScreen->width, pScreen->height,
				ppix->devPrivate.ptr,
				PixmapBytePad(pScreen->width,
					      pScreen->rootDepth),
				0, 0, ~0);
#endif
		pspix->devPrivate.ptr = ppix->devPrivate.ptr;
	    }
	}

	xf86InvalidatePixmapCache();

	if (!xf86Resetting) {
#ifdef XFreeXDGA
	    if (!(tgaInfoRec.directMode & XF86DGADirectGraphics))
#endif
		tgaCleanUp();
	}
	xf86UnMapDisplay(screen_idx, LINEAR_REGION);
    }
}

/*
 * tgaCloseScreen --
 *      called to ensure video is enabled when server exits.
 */

/*ARGSUSED*/
Bool
tgaCloseScreen(screen_idx, pScreen)
    int        screen_idx;
    ScreenPtr  pScreen;
{
    extern void tgaClearSavedCursor();

    /*
     * Hmm... The server may shut down even if it is not running on the
     * current vt. Let's catch this case here.
     */
    xf86Exiting = TRUE;
    if (xf86VTSema)
	tgaEnterLeaveVT(LEAVE, screen_idx);
    else if (ppix) {
    /* 7-Jan-94 CEG: The server is not running on the current vt.
     * Free the screen snapshot taken when the server vt was left.
     */
	    (savepScreen->DestroyPixmap)(ppix);
	    ppix = NULL;
    }

#ifdef NOTYET
    tgaClearSavedCursor(screen_idx);
#endif

    switch (tgaInfoRec.bitsPerPixel) {
    case 8:
        cfbCloseScreen(screen_idx, savepScreen);
	break;
    case 32:
        cfb32CloseScreen(screen_idx, savepScreen);
	break;
    }

    savepScreen = NULL;
    return(TRUE);
}

/*
 * tgaSaveScreen --
 *      blank the screen.
 */
Bool
tgaSaveScreen (pScreen, on)
     ScreenPtr     pScreen;
     Bool          on;
{
    if (on)
	SetTimeSinceLastInputEvent();

    if (xf86VTSema) {

#ifdef NOTYET
	if (on) {
	    if (tgaHWCursorSave != -1) {
		tgaSetRamdac(tgaCRTCRegs.color_depth, TRUE,
				tgaCRTCRegs.dot_clock);
		regwb(GEN_TEST_CNTL, tgaHWCursorSave);
		tgaHWCursorSave = -1;
	    }
	    tgaRestoreColor0(pScreen);
	    if (tgaRamdacSubType != DAC_ATI68875)
		outb(ioDAC_REGS+2, 0xff);
	} else {
	    outb(ioDAC_REGS, 0);
	    outb(ioDAC_REGS+1, 0);
	    outb(ioDAC_REGS+1, 0);
	    outb(ioDAC_REGS+1, 0);
	    outb(ioDAC_REGS+2, 0x00);

	    tgaSetRamdac(CRTC_PIX_WIDTH_8BPP, TRUE,
			    tgaCRTCRegs.dot_clock);
	    tgaHWCursorSave = regrb(GEN_TEST_CNTL);
	    regwb(GEN_TEST_CNTL, tgaHWCursorSave & ~HWCURSOR_ENABLE);

	    if (tgaRamdacSubType != DAC_ATI68875)
		outb(ioDAC_REGS+2, 0x00);
	}
#endif
    }

    return(TRUE);
}

/*
 * tgaDPMSSet -- Sets VESA Display Power Management Signaling (DPMS) Mode
 *
 * Only the Off and On modes are currently supported.
 */

void
tgaDPMSSet(PowerManagementMode)
    int PowerManagementMode;
{
#ifdef DPMSExtension
    int crtcGenCntl;
    if (!xf86VTSema) return;
    crtcGenCntl = TGA_READ_REG(TGA_VALID_REG);
    switch (PowerManagementMode)
    {
    case DPMSModeOn:
	/* HSync: On, VSync: On */
	crtcGenCntl |= 0x0001;
	break;
    case DPMSModeStandby:
	/* HSync: Off, VSync: On -- Not Supported */
	break;
    case DPMSModeSuspend:
	/* HSync: On, VSync: Off -- Not Supported */
	break;
    case DPMSModeOff:
	/* HSync: Off, VSync: Off */
	crtcGenCntl &= 0xFFFE;
	break;
    }
    usleep(10000);
    TGA_WRITE_REG(crtcGenCntl, TGA_VALID_REG);
#endif
}

/*
 * tgaAdjustFrame --
 *      Modify the CRT_OFFSET for panning the display.
 */
void
tgaAdjustFrame(x, y)
    int x, y;
{
	/* TGA needs much work for this to happen */
}

/*
 * tgaSwitchMode --
 *      Reinitialize the CRTC registers for the new mode.
 */
Bool
tgaSwitchMode(mode)
    DisplayModePtr mode;
{
    tgaCalcCRTCRegs(&tgaCRTCRegs, mode);
    tgaSetCRTCRegs(&tgaCRTCRegs);

    return(TRUE);
}

/*
 * tgaValidMode --
 *
 */
static int
tgaValidMode(mode, verbose, flag)
    DisplayModePtr mode;
    Bool verbose;
    int flag;
{
    if (mode->Flags & V_INTERLACE)
    {
	ErrorF("%s %s: Cannot support interlaced modes, deleting.\n",
			XCONFIG_GIVEN, tgaInfoRec.name);
	return MODE_BAD;
    }
    return MODE_OK;
}

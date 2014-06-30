/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/mono/mono.c,v 3.28.2.3 1997/05/11 02:56:21 dawes Exp $ */
/*
 * MONO: Driver family for interlaced and banked monochrome video adaptors
 * Pascal Haible 8/93, 3/94, 4/94 haible@IZFM.Uni-Stuttgart.DE
 *
 * mono/mono/mono.c
 *
 * derived from:
 * hga2/...
 * Author:  Davor Matic, dmatic@athena.mit.edu
 * and
 * vga256/...
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * see mono/COPYRIGHT for copyright and disclaimers.
 */
/* $XConsortium: mono.c /main/16 1996/10/23 18:46:08 kaleb $ */


#include "X.h"
#include "input.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "mipointer.h"
#include "cursorstr.h"
#include "gcstruct.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"
#include "xf86Procs.h"
#include "mono.h"

extern Bool xf86Exiting, xf86Resetting, xf86ProbeFailed;

void monoPrintIdent(
#if NeedFunctionPrototypes
void
#endif
);

Bool monoProbe(
#if NeedFunctionPrototypes
void
#endif
);

Bool monoScreenInit (
#if NeedFunctionPrototypes
int       scr_index,
ScreenPtr pScreen,
int       argc,
char    **argv
#endif
);

void monoEnterLeaveVT(
#if NeedFunctionPrototypes
Bool enter,
int screen_idx
#endif
);

void monoAdjustFrame(
#if NeedFunctionPrototypes
int x,
int y
#endif
);

Bool monoCloseScreen(
#if NeedFunctionPrototypes
int       screen_idx,
ScreenPtr pScreen
#endif
);

int monoValidMode(
#if NeedFunctionPrototypes
    DisplayModePtr,
    Bool,
    int
#endif 
);

/* ScrnInfoRec (common/xf86.h) describes the SCREEN */
/* This record is device and driver independent */
/* Used by the upper levels: xf86Init.c uses */
/*   ScrnInfoPtr xf86Screens[] defined in xf86Conf.c */
/*   Each entry there is a possibly configured device */
ScrnInfoRec monoInfoRec = {
  FALSE,		/* Bool configured */
  -1,			/* int tmpIndex */
  -1,			/* int scrnIndex */
  monoProbe,		/* Bool (* Probe)() */
  monoScreenInit,	/* Bool (* Init)() */
  monoValidMode,	/* int (* ValidMode)() */
  monoEnterLeaveVT,	/* void (* EnterLeaveVT)(int,int) */
  (void (*)())NoopDDA,	/* void (* EnterLeaveMonitor)(int) */
  (void (*)())NoopDDA,	/* void (* EnterLeaveCursor)(int) */
  monoAdjustFrame,	/* void (* AdjustFrame)(int,int) */
  (Bool (*)())NoopDDA,	/* Bool (* SwitchMode)() */
  (void (*)())NoopDDA,	/* void (* DPMSSet)() */
  monoPrintIdent,	/* void (* PrintIdent)() */
  1,			/* int depth */
  {0, 0, 0},            /* xrgb weight */
  1,			/* int bitsPerPixel */
  StaticGray,		/* int defaultVisual */
  -1, -1,		/* int virtualX,virtualY */
  -1,                   /* int displayWidth */
  -1, -1, -1, -1,	/* int frameX0, frameY0, frameX1, frameY1 */
  {0, },		/* OFlagSet options */
  {0, },		/* OFlagSet clockOptions */
  {0, },                /* OFlagSet xconfigFlag */
  NULL,			/* char *chipset */
  NULL,			/* char *ramdac */
  {0, 0, 0, 0},		/* int dacSpeeds[MAXDACSPEEDS] */
  0,			/* int dacSpeedBpp */
  0,			/* int clocks */
  {0, },		/* int clock[MAXCLOCKS] */
  0,			/* int maxClock */
  0,			/* int videoRam */
  0,			/* int BIOSbase */
  0,			/* int MemBase, memory base address of framebuffer */
  350, 285,		/* int width, height : Screen picture size in mm */
			/* This should be given from the lowlevel driver! */
  0,			/* unsigned long speedup */
  NULL,			/* DisplayModePtr modes */
  NULL,			/* MonPtr monitor */
  NULL,			/* char *clockprog */
  -1,			/* int textclock */
  TRUE,			/* Bool bankedMono */
  "MONO",		/* char *name */
  {0, },		/* xrgb blackColour */
  {0, },		/* xrgb whiteColour */
  monoValidTokens,	/* int *validTokens */
  MONO_PATCHLEVEL,	/* char *patchLevel */
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
  NULL,			/* char* DCConfig */
  NULL,			/* char* DCOptions */
  0	                /* int MemClk */
#ifdef XFreeXDGA
  ,0,			/* int directMode */
  NULL,			/* Set Vid Page */
  0,			/* unsigned long physBase */
  0			/* int physSize */
#endif
};

unsigned char *monoBase = NULL;
static unsigned char *monoVirtBase = NULL;

static pointer monoOrigVideoState = NULL;
static pointer monoNewVideoState = NULL;

int monoBankAseg;
int monoBankBseg;

/* Banking constants set from chipset values */
unsigned char *monoBankABottom;
unsigned char *monoBankATop;
unsigned char *monoBankBBottom;
unsigned char *monoBankBTop;
static unsigned char *monoMapBase;
static unsigned int monoMapSize;
static int monoScanLineWidth;

/*
 * hooks for communicating with the VideoChip on the MONO
 */
static pointer (* monoInitFunc)(
#if NeedFunctionPrototypes
DisplayModePtr
#endif
);
static void (* monoFbInitFunc)(
#if NeedFunctionPrototypes
void
#endif
);
static void (* monoEnterLeaveFunc)(
#if NeedFunctionPrototypes
Bool /* enter */
#endif
);
static pointer (* monoSaveFunc)(
#if NeedFunctionPrototypes
pointer
#endif
);
static void (* monoRestoreFunc)(
#if NeedFunctionPrototypes
pointer
#endif
);
static void (* monoAdjustFunc)(
#if NeedFunctionPrototypes
int /* x */,
int /* y */
#endif
);
static Bool (* monoSaveScreenFunc)(
#if NeedFunctionPrototypes
ScreenPtr /* pScreen */,
Bool      /* on */
#endif
);
static void (* monoClearScreenFunc)(
#if NeedFunctionPrototypes
void
#endif
);
PixelType * (* monoScanlineOffsetFuncA)(
#if NeedFunctionPrototypes
PixelType * /* pointer */,
int         /* offset */
#endif
);
PixelType * (* monoScanlineOffsetFuncB)(
#if NeedFunctionPrototypes
PixelType * /* pointer */,
int         /* offset */
#endif
);

static OFlagSet monoOptionFlags;

/* Pointer to the Screen while VT is switched out */
static ScreenPtr savepScreen = NULL;
static PixmapPtr ppix = NULL;

/* Banking functions etc. are saved and set to NoopDDA
 * while VT is switched out */
static void * (* saveInitFunc)(
#if NeedFunctionPrototypes
DisplayModePtr
#endif
);
static void (* saveFbInitFunc)(
#if NeedFunctionPrototypes
void
#endif
);
static void * (* saveSaveFunc)(
#if NeedFunctionPrototypes
pointer
#endif
);
static void (* saveRestoreFunc)(
#if NeedFunctionPrototypes
pointer
#endif
);
static void (* saveAdjustFunc)(
#if NeedFunctionPrototypes
int /* x */,
int /* y */
#endif
);
static Bool (* saveSaveScreenFunc)(
#if NeedFunctionPrototypes
ScreenPtr /* pScreen */,
Bool      /* on */
#endif
);

extern miPointerScreenFuncRec xf86PointerScreenFuncs;

extern monoVideoChipPtr monoDrivers[];

/*
 * monoPrintIdent
 *     Prints out identifying strings for drivers included in the server
 */
void
monoPrintIdent()
{
    int i, j, n=0, c=0;
    char* id;

    ErrorF("  %s: server for interlaced and banked monochrome graphics adaptors",
		monoInfoRec.name);
    ErrorF("\n        (Patchlevel %s):\n      ", monoInfoRec.patchLevel);

    for (i=0; monoDrivers[i]; i++)
	for (j = 0; (id = (monoDrivers[i]->ChipIdent)(j)); j++, n++) {
	    if (n) {
		ErrorF(",");
		c++;
		if (c + 1 + strlen(id) < 70) {
		    ErrorF(" ");
		    c++;
		} else {
		    ErrorF("\n      ");
		    c = 0;
		}
	    }
	    ErrorF("%s",id);
	    c += strlen(id);
	}
    ErrorF("\n");
}

/*
 * monoProbe --
 *     probe and initialize the hardware driver
 */
Bool
monoProbe()
{
    static int            i;

    if (monoInfoRec.depth != 1) {
	ErrorF("\n%s %s: Unsupported bpp for Mono server (%d)\n",
	       XCONFIG_GIVEN, monoInfoRec.name, monoInfoRec.depth);
	return(FALSE);
    }

    for (i=0; monoDrivers[i]; i++) {
	/* Probe every driver */
	if ((monoDrivers[i]->ChipProbe)()) {
	    /* The first that succeeded */
	    /* succeeding probe fills in:
	     * mono.InfoRec.chipset,
	     * mono.InfoRec.videoRam,
	     * monoInfoRec.virtualX,
	     * monoInfoRec.virtualY,
	     */
	    if (xf86Verbose) {
		ErrorF("%s %s: chipset:  %s\n",
			OFLG_ISSET(XCONFIG_CHIPSET,&monoInfoRec.xconfigFlag) ?
				XCONFIG_GIVEN : XCONFIG_PROBED ,
			monoInfoRec.name,
			monoInfoRec.chipset);
		ErrorF("%s %s: videoram: %dk\n",
			OFLG_ISSET(XCONFIG_VIDEORAM,&monoInfoRec.xconfigFlag) ?
				XCONFIG_GIVEN : XCONFIG_PROBED ,
			monoInfoRec.name,
			monoInfoRec.videoRam);
	    }
	    /* this drivers's functions and consts are now mono's functions */
	    monoEnterLeaveFunc   = monoDrivers[i]->ChipEnterLeave;
	    monoInitFunc         = monoDrivers[i]->ChipInit;
	    monoFbInitFunc       = monoDrivers[i]->ChipFbInit;
	    monoSaveFunc         = monoDrivers[i]->ChipSave;
	    monoRestoreFunc      = monoDrivers[i]->ChipRestore;
	    monoAdjustFunc       = monoDrivers[i]->ChipAdjust;
	    monoSaveScreenFunc   = monoDrivers[i]->ChipSaveScreen;
	    monoClearScreenFunc  = monoDrivers[i]->ChipClearScreen;
	    monoScanlineOffsetFuncA = monoDrivers[i]->ChipScanlineOffsetA;
	    monoScanlineOffsetFuncB = monoDrivers[i]->ChipScanlineOffsetB;
	    monoBankABottom      = monoDrivers[i]->ChipBankABottom;
	    monoBankATop         = monoDrivers[i]->ChipBankATop;
	    monoBankBBottom      = monoDrivers[i]->ChipBankBBottom;
	    monoBankBTop         = monoDrivers[i]->ChipBankBTop;
	    monoMapBase          = monoDrivers[i]->ChipMapBase;
	    monoMapSize          = monoDrivers[i]->ChipMapSize;
	    monoScanLineWidth    = monoDrivers[i]->ChipScanLineWidth;
	    monoOptionFlags      = monoDrivers[i]->ChipOptionFlags;

	    xf86VerifyOptions(&monoOptionFlags, &monoInfoRec);

	    /* Set up later (?) by xf86InitViewport in common/xf86Cursor.c */
	    /* (X0,Y0) = ViewPort */
	    monoInfoRec.frameX0 = 0;
	    monoInfoRec.frameY0 = 0;
	    monoInfoRec.frameX1 = 0;
	    monoInfoRec.frameY1 = 0;

	    /* Only one mode */
	    monoInfoRec.modes = (DisplayModePtr)xalloc(sizeof(DisplayModeRec));
	    /* display mode: real display size */
	    monoInfoRec.modes->HDisplay = monoDrivers[i]->ChipHDisplay;
	    monoInfoRec.modes->VDisplay = monoDrivers[i]->ChipVDisplay;
	    monoInfoRec.modes->next = monoInfoRec.modes;
	    monoInfoRec.modes->prev = monoInfoRec.modes;
	    /* Virtual screen size monoInfoRec.virtualX, ...virtualY
	     * is set up by ChipProbe() */
	    if (monoInfoRec.virtualX > monoInfoRec.modes->HDisplay ||
		monoInfoRec.virtualY > monoInfoRec.modes->VDisplay)
		if (xf86Verbose)
		    ErrorF("%s %s: Virtual resolution set to %dx%d\n",
			OFLG_ISSET(XCONFIG_VIRTUAL,&monoInfoRec.xconfigFlag) ?
			XCONFIG_GIVEN : XCONFIG_PROBED ,
			monoInfoRec.name,
			monoInfoRec.virtualX, monoInfoRec.virtualY);
	    (*monoFbInitFunc)();
	    /* exit after the first that succeeded */
	    return TRUE;
	} /* if probe succeeded */
    } /* for */
    if (monoInfoRec.chipset)
	ErrorF("%s: '%s' is an invalid chipset\n", monoInfoRec.name,
		monoInfoRec.chipset);
    return FALSE;
}

/*
 * monoScreenInit --
 *      Attempt to find and initialize a MONO framebuffer
 *      Driver is already detected, driver specific values are known.
 *      Most of the elements of the ScreenRec are filled in.  The
 *      video is enabled for the frame buffer...
 */
Bool
monoScreenInit (index, pScreen, argc, argv)
    int            index;        /* The index of pScreen in the ScreenInfo */
    ScreenPtr      pScreen;      /* The Screen to initialize */
    int            argc;         /* The number of the Server's arguments. */
    char           **argv;       /* The arguments themselves. Don't change! */
{
    int displayResolution = 92;    /* default to 92dpi */
    extern int monitorResolution;
    unsigned int mapSize;

    if (serverGeneration == 1) {
#if defined(CSRG_BASED) && !defined(__bsdi__)
	/* Hack for mmap() problem on 386bsd */
	if (monoMapSize < 0x18000)
	    mapSize = 0x18000;
	else
#endif /* CSRG_BASED && !__bsdi__ */
	    mapSize = monoMapSize;
	monoBase = (unsigned char *)xf86MapVidMem(index, VGA_REGION /* ?? */,
						 (pointer)monoMapBase, mapSize);
	if (monoInfoRec.bankedMono)
	    monoVirtBase = (unsigned char *)MONOBASE;
	else
	    monoVirtBase = (unsigned char *)monoBase;
	monoBankABottom = (unsigned char *)((unsigned long)monoBankABottom
			  + (unsigned long)monoBase);
	monoBankATop    = (unsigned char *)((unsigned long)monoBankATop
			  + (unsigned long)monoBase);
	monoBankBBottom = (unsigned char *)((unsigned long)monoBankBBottom
			  + (unsigned long)monoBase);
	monoBankBTop    = (unsigned char *)((unsigned long)monoBankBTop
			  + (unsigned long)monoBase);
    }

/* Warning: monoInit returns the video state (copy of it) that is to be set */
    if (!(monoNewVideoState=(pointer)(*monoInitFunc)(monoInfoRec.modes)))
	FatalError("%s: hardware initialisation failed\n", monoInfoRec.name);
    if (!monoOrigVideoState)
	monoOrigVideoState=(pointer)(*monoSaveFunc)(monoOrigVideoState);
    (*monoRestoreFunc)(monoNewVideoState);
#ifndef DIRTY_STARTUP
    (*monoSaveScreenFunc)(NULL, FALSE); /* blank the screen */
#endif
    (*monoAdjustFunc)(monoInfoRec.frameX0, monoInfoRec.frameY0);
  
    /*
     * Take display resolution from the -dpi flag if specified
     */

    if (monitorResolution)
	displayResolution = monitorResolution;

    /*
     * Inititalize the dragon to mono display
     */
    if (!mono_mfbScreenInit(pScreen,
			(pointer)monoVirtBase,
			monoInfoRec.virtualX,
			monoInfoRec.virtualY,
			displayResolution, displayResolution,
			monoScanLineWidth))
	return(FALSE);

    pScreen->blackPixel = 0;
    pScreen->whitePixel = 1;
    XF86FLIP_PIXELS();

    pScreen->CloseScreen = monoCloseScreen;
    pScreen->SaveScreen  = monoSaveScreenFunc;
  
    miDCInitialize (pScreen, &xf86PointerScreenFuncs);

    if (!mono_mfbCreateDefColormap(pScreen))
	return(FALSE);

#ifndef DIRTY_STARTUP
    /* Fill the screen with black */
    (*monoClearScreenFunc)();
    (*monoSaveScreenFunc)(NULL, TRUE); /* unblank the screen */
#endif /* ! DIRTY_STARTUP */

    savepScreen = pScreen;

    return(TRUE);
}

/*
 * monoEnterLeaveVT -- 
 *      grab/ungrab the current VT completely.
 */
void
monoEnterLeaveVT(enter, screen_idx)
    Bool enter;
    int screen_idx;
{
    BoxRec  pixBox;	/* Box: Region to copy ? */
    RegionRec pixReg;	/* */
    DDXPointRec pixPt;	/* Point: upper left corner */
    PixmapPtr   pspix;	/* Pointer to the pixmap of the saved screen */
    ScreenPtr   pScreen = savepScreen;	/* This is the 'old' Screen:
				real screen on leave, dummy on enter */

    /* Set up pointer to the saved pixmap (pspix) only if not resetting
						and not exiting */
    /* mono should treat this differently:
     * As this screen might be a secondary screen, it is better to clear
     * it on exit.
     * This makes VT switching on a primary mono card look better (& slower)
     */
    if (!xf86Resetting && !xf86Exiting ) {
	pixBox.x1 = 0; pixBox.x2 = pScreen->width;
	pixBox.y1 = 0; pixBox.y2 = pScreen->height;
	pixPt.x = 0; pixPt.y = 0;
	(pScreen->RegionInit)(&pixReg, &pixBox, 1);
	pspix = (PixmapPtr)pScreen->devPrivate;
    }

    if (enter) {
	monoInitFunc	= saveInitFunc;
	monoFbInitFunc	= saveFbInitFunc;
	monoSaveFunc	= saveSaveFunc;
	monoRestoreFunc	= saveRestoreFunc;
	monoAdjustFunc	= saveAdjustFunc;
	monoSaveScreenFunc = saveSaveScreenFunc;

	xf86MapDisplay(screen_idx, VGA_REGION);

	(*monoEnterLeaveFunc)(ENTER);
	monoOrigVideoState=(pointer)(*monoSaveFunc)(monoOrigVideoState);
	(*monoRestoreFunc)(monoNewVideoState);

        /*
         * point pspix back to monoVirtBase, and copy the dummy buffer to the
         * real screen.
         */
	if (!xf86Resetting)
	    if ((pointer)pspix->devPrivate.ptr!=(pointer)monoVirtBase && ppix) {
		pspix->devPrivate.ptr = (pointer)monoVirtBase;
		mono_mfbDoBitblt(&ppix->drawable, &pspix->drawable, GXcopy,
				&pixReg, &pixPt);
	    }
	if (ppix) {
	    (pScreen->DestroyPixmap)(ppix);
	    ppix = NULL;
	}
    } else { /* enter */
	/* -> leaving */
	/* Make sure that another driver hasn't disabeled IO */    
	xf86MapDisplay(screen_idx, VGA_REGION /* ??? */ );

	(*monoEnterLeaveFunc)(ENTER);

	/*
	 * Create a dummy pixmap to write to while VT is switched out.
	 * Copy the screen to that pixmap
	 *
	 * This pixmap has to be ScanLineWidth pixels wide !!
	 */
	if (!xf86Exiting) {
	    ppix = (pScreen->CreatePixmap)(pScreen, monoScanLineWidth,
					pScreen->height, pScreen->rootDepth);
            if (ppix) {
		mono_mfbDoBitblt(&pspix->drawable, &ppix->drawable, GXcopy, &pixReg,
				&pixPt);
		mono_mfbSolidBlackArea(&pspix->drawable, 1, &pixBox, GXcopy, NULL);
		pspix->devPrivate.ptr = ppix->devPrivate.ptr;
            }
	    monoNewVideoState=(pointer)(*monoSaveFunc)(monoNewVideoState);
	} else {
	/* If don't need the old graphics left in video mem, clear the screen */
#ifndef DIRTY_STARTUP
	(*monoClearScreenFunc)();
#endif
	}
	/*
	 * We come here in many cases, but one is special: When the server aborts
	 * abnormaly. Therefore there MUST be a check whether monoOrigVideoState
	 * is valid or not.
	 */
	if (monoOrigVideoState)
	    (*monoRestoreFunc)(monoOrigVideoState);

	(*monoEnterLeaveFunc)(LEAVE);
	xf86UnMapDisplay(screen_idx, VGA_REGION);
	saveInitFunc         = monoInitFunc;
	saveFbInitFunc       = monoFbInitFunc;
	saveSaveFunc         = monoSaveFunc;
	saveRestoreFunc      = monoRestoreFunc;
	saveAdjustFunc       = monoAdjustFunc;
	saveSaveScreenFunc   = monoSaveScreenFunc;

	monoInitFunc		= (void *(*)()) NoopDDA;
	monoFbInitFunc		= (void  (*)()) NoopDDA;
	monoSaveFunc		= (void *(*)()) NoopDDA;
	monoRestoreFunc		= (void  (*)()) NoopDDA;
	monoAdjustFunc		= (void  (*)()) NoopDDA;
	monoSaveScreenFunc	= (Bool  (*)()) NoopDDA;
    }
}

/*
 * monoCloseScreen --
 *      called to ensure video is enabled when server exits.
 */
Bool
monoCloseScreen(screen_idx, pScreen)
    int       screen_idx;
    ScreenPtr pScreen;
{
    /*
     * Hmm... The server may shut down even if it is not running on the
     * current vt. Let's catch this case here.
     */
    xf86Exiting = TRUE;
    if (xf86VTSema)
	monoEnterLeaveVT(LEAVE, screen_idx);
    else if (ppix) {
	/*
	 * 7-Jan-94 CEG: The server is not running on the current vt.
	 * Free the screen snapshot taken when the server vt was left.
	 */
	(savepScreen->DestroyPixmap)(ppix);
	ppix = NULL;
    }
    return(TRUE);
}

/*
 * monoAdjustFrame --
 *      Set a new viewport
 */
void
monoAdjustFrame(x, y)
    int x, y;
{
    /* Strange: gets called with
     * x > monoInfoRec.virtualX - monoInfoRec.modes->HDisplay
     * Shouldn't be. Can mess up the lowlevel driver (does no checking).
     * - Seems not to exceed maxshift+1 -
     */
    int maxshift;
    maxshift = monoInfoRec.virtualX - monoInfoRec.modes->HDisplay;
    if (x <= maxshift)
	(*monoAdjustFunc)(x, y);
    else {
	(*monoAdjustFunc)(maxshift, y);
#if 0
	ErrorF("%s: Warning: monoAdjustFrame() called with x=%d\n",
		monoInfoRec.name, x);
#endif
    }
}

int
monoValidMode(mode, verbose, flag)
     DisplayModePtr mode;
     Bool verbose;
     int flag;
{
  /* Maybe this should return MODE_BAD since no XF86Config modes are used? */
  return(MODE_OK);
}

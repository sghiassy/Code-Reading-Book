/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/ibm8514/ibm8514.c,v 3.29.2.3 1997/05/11 02:56:04 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL, KEVIN E. MARTIN AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL THOMAS ROELL, KEVIN E. MARTIN OR TIAGO GONS BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified by Tiago Gons (tiago@comosjn.hobby.nl)
 *
 */
/* $XConsortium: ibm8514.c /main/17 1996/10/23 18:41:41 kaleb $ */


#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "mipointer.h"
#include "cursorstr.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "ibm8514.h"
#include "reg8514.h"
#include "xf86bcache.h"
#include "xf86fcache.h"
#include "xf86text.h"
#include "8514cach.h"
#include "mi.h"
#include "cfb.h"

extern Bool xf86Exiting, xf86Resetting, xf86ProbeFailed;

static int ibm8514ValidMode(
#if NeedFunctionPrototypes 
    DisplayModePtr,
    Bool,
    int
#endif
);

ScrnInfoRec ibm8514InfoRec = {
    FALSE,		/* Bool configured */
    -1,			/* int tmpIndex */
    -1,			/* int scrnIndex */
    ibm8514Probe,      	/* Bool (* Probe)() */
    ibm8514Initialize,	/* Bool (* Init)() */
    ibm8514ValidMode,	/* int (* ValidMode)() */
    ibm8514EnterLeaveVT,/* void (* EnterLeaveVT)() */
    (void (*)())NoopDDA,/* void (* EnterLeaveMonitor)() */
    (void (*)())NoopDDA,/* void (* EnterLeaveCursor)() */
    (void (*)())NoopDDA,/* void (* AdjustFrame)() */
    (Bool (*)())NoopDDA,/* Bool (* SwitchMode)() */
    (void (*)())NoopDDA,/* void (* DPMSSet)() */
    ibm8514PrintIdent,  /* void (* PrintIdent)() */
    8,			/* int depth */
    {0, 0, 0},          /* xrgb weight */
    8,			/* int bitsPerPixel */
    PseudoColor,       	/* int defaultVisual */
    -1, -1,		/* int virtualX,virtualY */
    -1,                 /* displayWidth */
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
    0,			/* int BIOSbase, 1.3 new */
    0,			/* unsigned long MemBase, unused for this driver */
    240, 180,		/* int width, height */
    0,                  /* unsigned long  speedup */
    NULL,	       	/* DisplayModePtr modes */
    NULL,	       	/* MonPtr monitor */
    NULL,               /* char *clockprog */
    -1,			/* int textclock, 1.3 new */
    FALSE,              /* Bool bankedMono */
    "ibm8514",          /* char *name */
    {0, },		/* xrgb blackColour */
    {0, },		/* xrgb whiteColour */
    ibm8514ValidTokens,	/* int *validTokens */
    IBM8514_PATCHLEVEL,	/* char *patchlevel */
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

short ibm8514alu[16] = {
    MIX_0,
    MIX_AND,
    MIX_SRC_AND_NOT_DST,
    MIX_SRC,
    MIX_NOT_SRC_AND_DST,
    MIX_DST,
    MIX_XOR,
    MIX_OR,
    MIX_NOR,
    MIX_XNOR,
    MIX_NOT_DST,
    MIX_SRC_OR_NOT_DST,
    MIX_NOT_SRC,
    MIX_NOT_SRC_OR_DST,
    MIX_NAND,
    MIX_1
};

extern miPointerScreenFuncRec xf86PointerScreenFuncs;

static int AlreadyInited = FALSE;

static int ibm8514ScreenMode;

static ScreenPtr savepScreen = NULL;
static PixmapPtr ppix = NULL;

static unsigned ibm8514_IOPorts[] = {
	DAC_MASK, DAC_R_INDEX, DAC_W_INDEX, DAC_DATA, DISP_STAT,
	H_TOTAL, H_DISP, H_SYNC_STRT, H_SYNC_WID,
	V_TOTAL, V_DISP, V_SYNC_STRT, V_SYNC_WID,
	DISP_CNTL, ADVFUNC_CNTL, SUBSYS_CNTL, ROM_PAGE_SEL,
	CUR_Y, CUR_X, DESTY_AXSTP, DESTX_DIASTP, 
	ERR_TERM, MAJ_AXIS_PCNT, GP_STAT, CMD, SHORT_STROKE,
	BKGD_COLOR, FRGD_COLOR, WRT_MASK, RD_MASK,
	COLOR_CMP, BKGD_MIX, FRGD_MIX, MULTIFUNC_CNTL, PIX_TRANS,
};
static int Num_ibm8514_IOPorts = (sizeof(ibm8514_IOPorts)/
				  sizeof(ibm8514_IOPorts[0]));

/*
 * ibm8514Probe --
 *     probe and initialize the hardware driver
 */
Bool
ibm8514Probe()
{
    OFlagSet       validOptions;

    xf86ClearIOPortList(ibm8514InfoRec.scrnIndex);
    xf86AddIOPorts(ibm8514InfoRec.scrnIndex, Num_ibm8514_IOPorts,
		   ibm8514_IOPorts);

    if (ibm8514InfoRec.chipset) {
	if (StrCaseCmp(ibm8514InfoRec.chipset, "ibm8514")) {
	    ErrorF("Chipset specified in XF86Config is not \"ibm8514\" (%s)!\n",
		   ibm8514InfoRec.chipset);
	    return(FALSE);
	}
	xf86EnableIOPorts(ibm8514InfoRec.scrnIndex);
    }
    else
    {
	xf86EnableIOPorts(ibm8514InfoRec.scrnIndex);

	/* Reset the 8514/A, and disable all interrupts. */
	outw(SUBSYS_CNTL, GPCTRL_RESET | CHPTEST_NORMAL);
	outw(SUBSYS_CNTL, GPCTRL_ENAB | CHPTEST_NORMAL);

	/* Check to see if an 8514/A is actually installed by writing to
	 * the ERR_TERM register, and reading back.  The 0x5a5a value is
	 * entirely arbitrary.
	 */
	outw(ERR_TERM, 0x5a5a);
	if (inw(ERR_TERM) != 0x5a5a) {
	    xf86DisableIOPorts(ibm8514InfoRec.scrnIndex);
	    return(FALSE);
	}
	/* 6 mar 93 TCG : let's make certain */
	outw(ERR_TERM, 0x5555);
	if (inw(ERR_TERM) != 0x5555) {
	    xf86DisableIOPorts(ibm8514InfoRec.scrnIndex);
	    return(FALSE);
	}
    }
    ibm8514InfoRec.chipset = "ibm8514";
    xf86ProbeFailed = FALSE;

    OFLG_ZERO(&validOptions);
    xf86VerifyOptions(&validOptions, &ibm8514InfoRec);

    return(TRUE);
}

void
ibm8514PrintIdent()
{
    ErrorF("  %s: accelerated server for ", ibm8514InfoRec.name);
    ErrorF("8514/A graphics adaptors (Patchlevel %s)\n", 
	   ibm8514InfoRec.patchLevel);
}

/*
 * ibm8514Initialize --
 *      Attempt to find and initialize a VGA framebuffer
 *      Most of the elements of the ScreenRec are filled in.  The
 *      video is enabled for the frame buffer...
 */

Bool
ibm8514Initialize (scr_index, pScreen, argc, argv)
    int            scr_index;    /* The index of pScreen in the ScreenInfo */
    ScreenPtr      pScreen;      /* The Screen to initialize */
    int            argc;         /* The number of the Server's arguments. */
    char           **argv;       /* The arguments themselves. Don't change! */
{
  int displayResolution = 75;    /* default to 75dpi */
  extern int monitorResolution;

    if (ibm8514InfoRec.virtualX == 1024 && ibm8514InfoRec.virtualY == 768) {
	ibm8514ScreenMode = MODE_1024;
    } else if (ibm8514InfoRec.virtualX == 640 &&
	       ibm8514InfoRec.virtualY == 480) {
	ibm8514ScreenMode = MODE_640;
    } else {
	ErrorF("Illegal screen size: (%dx%d)\n",
	       ibm8514InfoRec.virtualX, ibm8514InfoRec.virtualY);
	return FALSE;
    }

    /* Map io ports, map registers, etc. */
    /* Initialize the screen, saving the original state for Save/Restore
       function */
    ibm8514Init(ibm8514ScreenMode);
    InitEnvironment();
    AlreadyInited = TRUE;

#ifdef testing_8514
    if (ibm8514ScreenMode == MODE_1024)
	ibm8514InfoRec.virtualY = 512;
#endif

    xf86InitCache( ibm8514CacheMoveBlock );
    ibm8514FontCache8Init();

    ibm8514ImageInit();

    /*
     * Take display resolution from the -dpi flag if specified
     */

    if (monitorResolution)
        displayResolution = monitorResolution;

    if (!ibm8514ScreenInit(pScreen,
			   (pointer) 0,
			   ibm8514InfoRec.virtualX, ibm8514InfoRec.virtualY,
			   displayResolution, displayResolution,
			   ibm8514InfoRec.virtualX))
	return(FALSE);

    pScreen->CloseScreen = ibm8514CloseScreen;
    pScreen->SaveScreen = ibm8514SaveScreen;
    pScreen->InstallColormap = ibm8514InstallColormap;
    pScreen->UninstallColormap = ibm8514UninstallColormap;
    pScreen->ListInstalledColormaps = ibm8514ListInstalledColormaps;
    pScreen->StoreColors = ibm8514StoreColors;

    mfbRegisterCopyPlaneProc (pScreen, miCopyPlane);
  
    miDCInitialize (pScreen, &xf86PointerScreenFuncs);

    savepScreen = pScreen;

    return (cfbCreateDefColormap(pScreen));
}

/*
 * ibm8514EnterLeaveVT -- 
 *      grab/ungrab the current VT completely.
 */

void
ibm8514EnterLeaveVT(enter, screen_idx)
     Bool enter;
     int screen_idx;
{
  PixmapPtr   pspix;
  ScreenPtr   pScreen = savepScreen;

    if (!xf86Resetting && !xf86Exiting)
    {
        pspix = (PixmapPtr)pScreen->devPrivate;
    }

    if (enter) {
	xf86EnableIOPorts(ibm8514InfoRec.scrnIndex);
	ibm8514Init(ibm8514ScreenMode);
	InitEnvironment();
	ibm8514RestoreDACvalues(); /* 25-4-93 TCG */
	ibm8514FontCache8Init();
	AlreadyInited = TRUE;

        /* 2-5-93 TCG */
        if (!xf86Resetting)
           if ((pointer)pspix->devPrivate.ptr != /*(pointer)*/NULL && ppix)
           {
              pspix->devPrivate.ptr = /*(pointer)*/NULL;
              ibm8514ImageWrite(0, 0, pScreen->width, pScreen->height,
                                ppix->devPrivate.ptr,
                                PixmapBytePad(pScreen->width, pScreen->rootDepth),
                                0, 0, ibm8514alu[GXcopy], 0xFF);

           }
        if (ppix) {
           (pScreen->DestroyPixmap)(ppix);
	   ppix = NULL;
	}
    } else {
        if (!xf86Exiting)
        {
           ppix = (pScreen->CreatePixmap)(pScreen, pScreen->width,
                                          pScreen->height, pScreen->rootDepth);
           if (ppix)
           {
              ibm8514ImageRead(0, 0, pScreen->width, pScreen->height,
                               ppix->devPrivate.ptr,
                               PixmapBytePad(pScreen->width, pScreen->rootDepth),
                               0, 0, 0xff);
              pspix->devPrivate.ptr = ppix->devPrivate.ptr;
           }
        }

	/*
	 * We come here in many cases, but one is special: When the server
	 * aborts abnormaly. Therefore there MUST be a check whether
	 * vgaOrigVideoState is valid or not.
	 */
	/* The above refers to the video state not being set up until the
	   Initialize function (above) is called.  If it has not been called
	   then "vgaOrigVideoState" will not be initialized and thus cannot be
	   restored!  A similar thing must be done in the 8514.
	   */
	if (AlreadyInited) {
	    ibm8514CleanUp();
	}
    }
}

/*
 * ibm8514CloseScreen --
 *      called to ensure video is enabled when server exits.
 */

Bool
ibm8514CloseScreen(screen_idx, pScreen)
     int screen_idx;
     ScreenPtr pScreen;
{
    /*
     * Hmm... The server may shut down even if it is not running on the
     * current vt. Let's catch this case here.
     */
    xf86Exiting = TRUE;
    if (xf86VTSema)
	ibm8514EnterLeaveVT(LEAVE, screen_idx);
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
 * ibm8514SaveScreen --
 *      blank the screen.
 */

Bool
ibm8514SaveScreen (pScreen, on)
     ScreenPtr     pScreen;
     Bool          on;
{
    if (on)
	SetTimeSinceLastInputEvent();
    if (xf86VTSema) {
	if (on) {
	    ibm8514RestoreColor0(pScreen);

	    outw(DAC_MASK, 0xff);
	} else {
	    outb(DAC_W_INDEX, 0);
	    outb(DAC_DATA, 0);
	    outb(DAC_DATA, 0);
	    outb(DAC_DATA, 0);

	    outw(DAC_MASK, 0x00);
	}
    }
    return(TRUE);
}

/*
 * ibm8514ValidMode --
 *
 */
static int
ibm8514ValidMode(mode, verbose, flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
return MODE_OK;
}


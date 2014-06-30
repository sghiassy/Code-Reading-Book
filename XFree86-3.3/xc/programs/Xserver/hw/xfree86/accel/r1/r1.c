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
#include "r1.h"

#include "xf86xaa.h"

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "xf86scrin.h"

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

static int r1ValidMode(
#if NeedFunctionPrototypes
    DisplayModePtr,
    Bool,
    int
#endif
);


ScrnInfoRec r1InfoRec = {
    FALSE,		/* Bool configured */
    -1,			/* int tmpIndex */
    -1,			/* int scrnIndex */
    r1Probe,      	/* Bool (* Probe)() */
    r1Initialize,	/* Bool (* Init)() */
    r1ValidMode,	/* Bool (* ValidMode)() */
    r1EnterLeaveVT,	/* void (* EnterLeaveVT)() */
    (void (*)())NoopDDA,/* void (* EnterLeaveMonitor)() */
    (void (*)())NoopDDA,/* void (* EnterLeaveCursor)() */
    r1AdjustFrame,	/* void (* AdjustFrame)() */
    r1SwitchMode,	/* Bool (* SwitchMode)() */
    r1DPMSSet,		/* void (* DPMSSet)() */
    r1PrintIdent,	/* void (* PrintIdent)() */
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
    "R1",		/* char *name */
    {0, },		/* xrgb blackColour */
    {0, },		/* xrgb whiteColour */
    r1ValidTokens,	/* int *validTokens */
    R1_PATCHLEVEL,	/* char *patchlevel */
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
pointer r1VideoMem = NULL;


/*
 * r1PrintIdent
 */
void
r1PrintIdent()
{
    ErrorF("  %s: non-accelerated server for R1 graphics adapters\n",
	    r1InfoRec.name);
    ErrorF("(Patchlevel %s)\n", r1InfoRec.patchLevel);
}


/*
 * r1Probe --
 *      check up whether an R1 based board is installed
 */
Bool
r1Probe()
{
    return(FALSE);  /* until routine is really implemented... */

    /* XXX -- R1-specific code goes here... */


    return(TRUE);
}


/*
 * r1Initialize --
 *
 */
Bool
r1Initialize (scr_index, pScreen, argc, argv)
int		scr_index;
ScreenPtr	pScreen;
int		argc;
char		**argv;
{
    int displayResolution = 75; 	/* default to 75dpi */
    extern int monitorResolution;
    Bool (*ScreenInitFunc)(register ScreenPtr, pointer, int, int, int, int, int);

    /* Init the screen. */
    /* XXX -- R1-specific code goes here... */
    /* TGA does the following -- you'll need to do something similar. */
/*
    tgaInitAperture(scr_index);
    tgaInit(tgaInfoRec.modes);
    tgaCalcCRTCRegs(&tgaCRTCRegs, tgaInfoRec.modes);
    tgaSetCRTCRegs(&tgaCRTCRegs);
    tgaInitEnvironment();
*/


    /* Take display resolution from the -dpi flag. */
    if (monitorResolution)
	displayResolution = monitorResolution;

    /* Let's use the new XAA Architecture.....*/
    R1AccelInit();
    switch(r1InfoRec.bitsPerPixel) {
	case 8:	    ScreenInitFunc = &xf86XAAScreenInit8bpp; break;
	case 16:    ScreenInitFunc = &xf86XAAScreenInit16bpp; break;
	case 24:    ScreenInitFunc = &xf86XAAScreenInit24bpp; break;
	case 32:    ScreenInitFunc = &xf86XAAScreenInit32bpp; break;
    }

    if (!ScreenInitFunc(pScreen, (pointer)r1VideoMem,
			 r1InfoRec.virtualX, r1InfoRec.virtualY,
			 displayResolution, displayResolution,
			 r1InfoRec.displayWidth))
	return(FALSE);

    pScreen->whitePixel = (Pixel)1;
    pScreen->blackPixel = (Pixel)0;
    XF86FLIP_PIXELS();
    pScreen->CloseScreen = r1CloseScreen;
    pScreen->SaveScreen = r1SaveScreen;

    switch(r1InfoRec.bitsPerPixel) {
	case 8:
	    pScreen->InstallColormap = r1InstallColormap;
	    pScreen->UninstallColormap = r1UninstallColormap;
	    pScreen->ListInstalledColormaps = r1ListInstalledColormaps;
	    pScreen->StoreColors = r1StoreColors;
	    break;

	case 16:
	case 24:
	case 32:
	    pScreen->InstallColormap = cfbInstallColormap;
	    pScreen->UninstallColormap = cfbUninstallColormap;
	    pScreen->ListInstalledColormaps = cfbListInstalledColormaps;
	    pScreen->StoreColors = (void (*)())NoopDDA;
	    break;
    }

    /* Use software cursor for now. */
    miDCInitialize(pScreen, &xf86PointerScreenFuncs);

    savepScreen = pScreen;
    return(cfbCreateDefColormap(pScreen));
}


/*
 * r1EnterLeaveVT -- 
 *      grab/ungrab the current VT completely.
 */
void
r1EnterLeaveVT(enter, screen_idx)
Bool enter;
int screen_idx;
{
    /* XXX -- R1-specific code goes here... */


    if (enter) {
	/* Restoring happens here... */

    } else {
	/* Saving happens here... */

    }
}


/*
 * r1CloseScreen --
 *      called to ensure video is enabled when server exits.
 */
Bool
r1CloseScreen(screen_idx, pScreen)
int        screen_idx;
ScreenPtr  pScreen;
{
    xf86Exiting = TRUE;
    if (xf86VTSema)
	r1EnterLeaveVT(LEAVE, screen_idx);

    switch (r1InfoRec.bitsPerPixel) {
	case 8:
	    cfbCloseScreen(screen_idx, savepScreen);
	    break;
	case 16:
	    cfb16CloseScreen(screen_idx, savepScreen);
	    break;
	case 24:
	    cfb24CloseScreen(screen_idx, savepScreen);
	    break;
	case 32:
	    cfb32CloseScreen(screen_idx, savepScreen);
	    break;
    }

    savepScreen = NULL;
    return(TRUE);
}


/*
 * r1SaveScreen --
 *      blank the screen.
 */
Bool
r1SaveScreen (pScreen, on)
ScreenPtr     pScreen;
Bool          on;
{
    if (on)
	SetTimeSinceLastInputEvent();

    if (xf86VTSema) {
	/* XXX -- R1-specific code goes here... */


    }

    return(TRUE);
}


/*
 * r1DPMSSet -- Sets VESA Display Power Management Signaling (DPMS) Mode
 *
 * Only the Off and On modes are currently supported.
 */
void
r1DPMSSet(PowerManagementMode)
int PowerManagementMode;
{
#ifdef DPMSExtension
    /* XXX -- R1-specific code goes here... */


#endif
}


/*
 * r1AdjustFrame --
 *      Modify the CRT_OFFSET for panning the display.
 */
void
r1AdjustFrame(x, y)
int x, y;
{
    /* XXX -- R1-specific code goes here... */


}


/*
 * r1SwitchMode --
 *      Reinitialize the CRTC registers for the new mode.
 */
Bool
r1SwitchMode(mode)
DisplayModePtr mode;
{
    /* XXX -- R1-specific code goes here... */


    return(TRUE);
}


/*
 * r1ValidMode --
 *
 */
static int
r1ValidMode(mode, verbose, flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
    /* XXX -- R1-specific code goes here... */


    return(MODE_OK);
}



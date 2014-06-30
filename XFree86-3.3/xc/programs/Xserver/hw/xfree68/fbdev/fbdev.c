/* $XConsortium: fbdev.c /main/1 1996/09/21 11:18:10 kaleb $ */




/* $XFree86: xc/programs/Xserver/hw/xfree68/fbdev/fbdev.c,v 3.4.2.2 1997/05/16 11:35:07 hohndel Exp $ */
/*
 *
 *  Author: Martin Schaller. Taken from hga2.c
 *
 *  Generic version by Geert Uytterhoeven (Geert.Uytterhoeven@cs.kuleuven.ac.be)
 *
 *  This version contains support for:
 *
 *	- Monochrome, 1 bitplane [mfb]
 *	- Color, 2/4/8 interleaved bitplanes with 2 bytes interleave [iplan2p?]
 *	- Color, interleaved bitplanes [ilbm]
 *	- Color, normal bitplanes [afb]
 *	- Color, chunky 8 bits per pixel [cfb8]
 *	- Color, chunky 16 bits per pixel [cfb16]
 *	- Color, chunky 32 bits per pixel [cfb32]
 */


#define fbdev_PATCHLEVEL "7"


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
#include "mfb.h"
#ifdef CONFIG_ILBM
#include "ilbm.h"
#endif /* CONFIG_ILBM */
#ifdef CONFIG_AFB
#include "afb.h"
#endif /* CONFIG_AFB */
#include <linux/fb.h>

#include "colormapst.h"
#include "resource.h"

extern char *fb_dev_name;	/* os-support/linux/lnx_init.c */


static int *fbdevPrivateIndexP;
static void (*fbdevBitBlt)();
static int (*CreateDefColormap)(ScreenPtr);

extern int mfbCreateDefColormap(ScreenPtr);

#if defined(CONFIG_IPLAN2p2) || defined(CONFIG_IPLAN2p4) || \
     defined(CONFIG_IPLAN2p8)
extern int iplCreateDefColormap(ScreenPtr);
#ifdef CONFIG_IPLAN2p2
extern int ipl2p2ScreenPrivateIndex;
extern void ipl2p2DoBitblt();
#endif /* CONFIG_IPLAN2p2 */
#ifdef CONFIG_IPLAN2p4
extern int ipl2p4ScreenPrivateIndex;
extern void ipl2p4DoBitblt();
#endif /* CONFIG_IPLAN2p4 */
#ifdef CONFIG_IPLAN2p8
extern int ipl2p8ScreenPrivateIndex;
extern void ipl2p8DoBitblt();
#endif /* CONFIG_IPLAN2p8 */
#endif /* defined(CONFIG_IPLAN2p2) || defined(CONFIG_IPLAN2p4) || \
	  defined(CONFIG_IPLAN2p8) */

#ifdef CONFIG_ILBM
extern int ilbmCreateDefColormap(ScreenPtr);
extern int ilbmScreenPrivateIndex;
extern void ilbmDoBitblt();
#endif /* CONFIG_ILBM */
#ifdef CONFIG_AFB
extern int afbCreateDefColormap(ScreenPtr);
extern int afbScreenPrivateIndex;
extern void afbDoBitblt();
#endif /* CONFIG_AFB */

#ifdef CONFIG_CFB8
extern int cfbCreateDefColormap(ScreenPtr);
extern void cfbDoBitblt();
#endif /* CONFIG_CFB8 */

#ifdef CONFIG_CFB16
extern int cfb16ScreenPrivateIndex;
extern void cfb16DoBitblt();
#endif /* CONFIG_CFB16 */

#ifdef CONFIG_CFB32
extern int cfb32ScreenPrivateIndex;
extern void cfb32DoBitblt();
#endif /* CONFIG_CFB32 */

extern int fbdevValidTokens[];


static Bool fbdevProbe(void);
static void fbdevPrintIdent(void);
static Bool fbdevSaveScreen(ScreenPtr pScreen, int on);
static Bool fbdevScreenInit(int scr_index, ScreenPtr pScreen, int argc,
			    char **argv);
static void fbdevEnterLeaveVT(Bool enter, int screen_idx);
static void fbdevAdjustFrame(int x, int y);
static Bool fbdevCloseScreen(int screen_idx, ScreenPtr screen);
static Bool fbdevValidMode(DisplayModePtr mode, Bool verbose, int flag);
static Bool fbdevSwitchMode(DisplayModePtr mode);


static void xfree2fbdev(DisplayModePtr mode, struct fb_var_screeninfo *var);
static void fbdev2xfree(struct fb_var_screeninfo *var, DisplayModePtr mode);


static struct fb_var_screeninfo initscrvar;

extern Bool xf86Exiting, xf86Resetting, xf86ProbeFailed;

ScrnInfoRec fbdevInfoRec = {
    FALSE,			/* Bool configured */
    -1,				/* int tmpIndex */
    -1,				/* int scrnIndex */
    fbdevProbe,			/* Bool (*Probe)() */
    fbdevScreenInit,		/* Bool (*Init)() */
    (int (*)())NoopDDA,		/* int (*ValidMode)() */
    fbdevEnterLeaveVT,		/* void (*EnterLeaveVT)() */
    (void (*)())NoopDDA,	/* void (*EnterLeaveMonitor)() */
    (void (*)())NoopDDA,	/* void (*EnterLeaveCursor)() */
    (void (*)())NoopDDA,	/* void (*AdjustFrame)() */
    (Bool (*)())NoopDDA,	/* Bool (*SwitchMode)() */
    (void (*)())NoopDDA,	/* void (*DPMSSet)() */
    fbdevPrintIdent,		/* void (*PrintIdent)() */
    8,				/* int depth */
    {0, },			/* xrgb weight */
    8,				/* int bitsPerPixel */
    PseudoColor,		/* int defaultVisual */
    -1, -1,			/* int virtualX,virtualY */
    -1,				/* int displayWidth */
    -1, -1, -1, -1,		/* int frameX0, frameY0, frameX1, frameY1 */
    {0, },			/* OFlagSet options */
    {0, },			/* OFlagSet clockOptions */
    {0, },			/* OFlagSet xconfigFlag */
    NULL,			/* char *chipset */
    NULL,			/* char *ramdac */
    {0, },			/* int dacSpeeds[MAXDACSPEEDS] */
    0,				/* int dacSpeedBpp */
    0,				/* int clocks */
    {0, },			/* int clock[MAXCLOCKS] */
    0,				/* int maxClock */
    0,				/* int videoRam */
    0,				/* int BIOSbase */
    0,				/* unsigned long MemBase */
    240, 180,			/* int width, height */
    0,				/* unsigned long speedup */
    NULL,			/* DisplayModePtr modes */
    NULL,			/* MonPtr monitor */
    NULL,			/* char *clockprog */
    -1,				/* int textclock */
    FALSE,			/* Bool bankedMono */
    "FBDev",			/* char *name */
    {0, },			/* xrgb blackColour */
    {0, },			/* xrgb whiteColour */
    fbdevValidTokens,		/* int *validTokens */
    fbdev_PATCHLEVEL,		/* char *patchLevel */
    0,				/* unsigned int IObase */
    0,				/* unsigned int DACbase */
    0,				/* unsigned int COPbase */
    0,				/* unsigned int POSbase */
    0,				/* unsigned int instance */
    0,				/* int s3Madjust */
    0,				/* int s3Nadjust */
    0,				/* int s3MClk */
    0,				/* int chipID */
    0,				/* int chipRev */
    0,				/* unsigned long VGAbase */
    0,				/* int s3RefClk */
    -1,				/* int s3BlankDelay */
    0,				/* int textClockFreq */
    NULL,			/* char *DCConfig */
    NULL,			/* char *DCOptions */
#ifdef XFreeXDGA
    0,				/* int directMode */
    NULL,			/* void (*setBank)() */
    0,				/* unsigned long physBase */
    0,				/* int physSize */
#endif
#ifdef XF86SETUP
    NULL,			/* void *device */
#endif


};

static pointer fbdevVirtBase = NULL;

static ScreenPtr savepScreen = NULL;
static PixmapPtr ppix = NULL;

extern miPointerScreenFuncRec xf86PointerScreenFuncs;

#define NOMAPYET	(ColormapPtr)0

static ColormapPtr InstalledMaps[MAXSCREENS];
	/* current colormap for each screen */


#define StaticGrayMask	(1 << StaticGray)
#define GrayScaleMask	(1 << GrayScale)
#define StaticColorMask	(1 << StaticColor)
#define PseudoColorMask	(1 << PseudoColor)
#define TrueColorMask	(1 << TrueColor)
#define DirectColorMask	(1 << DirectColor)

#define ALL_VISUALS	(StaticGrayMask|\
			 GrayScaleMask|\
			 StaticColorMask|\
			 PseudoColorMask|\
			 TrueColorMask|\
			 DirectColorMask)


static int fb_fd = -1; 
static struct fb_fix_screeninfo fb_fix;


static Bool UseModeDB = FALSE;


static void open_framebuffer(void)
{
    if (fb_fd == -1)
	if ((fb_fd = open(fb_dev_name, O_RDWR)) < 0)
	    FatalError("open_framebuffer: failed to open %s (%s)\n",
	    	       fb_dev_name, strerror(errno));
}

static void close_framebuffer(void)
{
    if (fb_fd != -1) {
	close(fb_fd);
	fb_fd = -1;
    }
}

pointer xf86MapVidMem(int ScreenNum, int Region, pointer Base,
		      unsigned long Size)
{
    pointer base;

    open_framebuffer();
    base = (pointer)mmap((caddr_t)0, fb_fix.smem_len, PROT_READ | PROT_WRITE,
			 MAP_SHARED, fb_fd, (off_t)0);
    if ((long)base == -1)
	FatalError("xf86MapVidMem: Could not mmap framebuffer (%s)\n",
		   strerror(errno));
    return(base);
}

void xf86UnMapVidMem(int ScreenNum, int Region, pointer Base,
		     unsigned long Size)
{
    munmap(fbdevVirtBase, fb_fix.smem_len);
    close_framebuffer();
}

static void fbdevUpdateColormap(ScreenPtr pScreen, int dex, int count,
				unsigned short *rmap, unsigned short *gmap,
				unsigned short *bmap)
{
    struct fb_cmap cmap;

    if (!xf86VTSema)
	/* Switched away from server vt, do nothing. */
	return;

    cmap.start = dex;
    cmap.len = count;
    cmap.red = rmap+dex;
    cmap.green = gmap+dex;
    cmap.blue = bmap+dex;
    cmap.transp = NULL;

    if (ioctl(fb_fd, FBIOPUTCMAP, &cmap) < 0)
	FatalError("fbdevUpdateColormap: FBIOPUTCMAP failed (%s)\n",
		   strerror(errno));
}

static int fbdevListInstalledColormaps(ScreenPtr pScreen, Colormap *pmaps)
{
    *pmaps = InstalledMaps[pScreen->myNum]->mid;
    return(1);
}

static void fbdevStoreColors(ColormapPtr pmap, int ndef, xColorItem *pdefs)
{
    unsigned short rmap[256], gmap[256], bmap[256];
    int i;

    if (pmap != InstalledMaps[pmap->pScreen->myNum])
	return;

    while (ndef--) {
	i = pdefs->pixel;
	rmap[i] = pdefs->red;
	gmap[i] = pdefs->green;
	bmap[i] = pdefs->blue;
	pdefs++;
	fbdevUpdateColormap(pmap->pScreen, i, 1, rmap, gmap, bmap);
    }
}

static void fbdevInstallColormap(ColormapPtr pmap)
{
    ColormapPtr oldmap = InstalledMaps[pmap->pScreen->myNum];
    int entries = pmap->pVisual->ColormapEntries;
    Pixel ppix[256];
    xrgb prgb[256];
    xColorItem defs[256];
    int i;

    if (pmap == oldmap)
	return;

    if (oldmap != NOMAPYET)
	WalkTree(pmap->pScreen, TellLostMap, &oldmap->mid);

    InstalledMaps[pmap->pScreen->myNum] = pmap;

    for (i = 0 ; i < entries; i++)
	ppix[i]=i;
    QueryColors(pmap, entries, ppix, prgb);

    for (i = 0 ; i < entries; i++) {
	defs[i].pixel = ppix[i];
	defs[i].red = prgb[i].red;
	defs[i].green = prgb[i].green;
	defs[i].blue = prgb[i].blue;
    }

    fbdevStoreColors(pmap, entries, defs);

    WalkTree(pmap->pScreen, TellGainedMap, &pmap->mid);

    return;
}

static void fbdevUninstallColormap(ColormapPtr pmap)
{
    ColormapPtr defColormap;

    if (pmap != InstalledMaps[pmap->pScreen->myNum])
	return;
    defColormap = (ColormapPtr)LookupIDByType(pmap->pScreen->defColormap,
    					      RT_COLORMAP);
    if (defColormap == InstalledMaps[pmap->pScreen->myNum])
	return;
    (*pmap->pScreen->InstallColormap)(defColormap);
}

/*
 *  fbdevPrintIdent -- Prints out identifying strings for drivers included in
 *		       the server
 */

static void fbdevPrintIdent(void)
{
    ErrorF("   %s: Server for frame buffer device\n", fbdevInfoRec.name);
    ErrorF("   (Patchlevel %s): mfb", fbdevInfoRec.patchLevel);
#ifdef CONFIG_IPLAN2p2
    ErrorF(", iplan2p2");
#endif
#ifdef CONFIG_IPLAN2p4
    ErrorF(", iplan2p4");
#endif
#ifdef CONFIG_IPLAN2p8
    ErrorF(", iplan2p8");
#endif
#ifdef CONFIG_ILBM
    ErrorF(", ilbm");
#endif
#ifdef CONFIG_AFB
    ErrorF(", afb");
#endif
#ifdef CONFIG_CFB8
    ErrorF(", cfb8");
#endif
#ifdef CONFIG_CFB16
    ErrorF(", cfb16");
#endif
#ifdef CONFIG_CFB32
    ErrorF(", cfb32");
#endif
    ErrorF("\n");
}


static Bool fbdevLookupMode(DisplayModePtr target)
{
    DisplayModePtr p;
    Bool found_mode = FALSE;

    for (p = fbdevInfoRec.monitor->Modes; p != NULL; p = p->next)
	if (!strcmp(p->name, target->name)) {
	    target->Clock          = p->Clock;
	    target->HDisplay       = p->HDisplay;
	    target->HSyncStart     = p->HSyncStart;
	    target->HSyncEnd       = p->HSyncEnd;
	    target->HTotal         = p->HTotal;
	    target->VDisplay       = p->VDisplay;
	    target->VSyncStart     = p->VSyncStart;
	    target->VSyncEnd       = p->VSyncEnd;
	    target->VTotal         = p->VTotal;
	    target->Flags          = p->Flags;
	    target->SynthClock     = p->SynthClock;
	    target->CrtcHDisplay   = p->CrtcHDisplay;
	    target->CrtcHSyncStart = p->CrtcHSyncStart;
	    target->CrtcHSyncEnd   = p->CrtcHSyncEnd;
	    target->CrtcHTotal     = p->CrtcHTotal;
	    target->CrtcVDisplay   = p->CrtcVDisplay;
	    target->CrtcVSyncStart = p->CrtcVSyncStart;
	    target->CrtcVSyncEnd   = p->CrtcVSyncEnd;
	    target->CrtcVTotal     = p->CrtcVTotal;
	    target->CrtcHAdjusted  = p->CrtcHAdjusted;
	    target->CrtcVAdjusted  = p->CrtcVAdjusted;
	    if (fbdevValidMode(target,FALSE,MODE_USED)) {
		found_mode = TRUE;
		break;
	    }
	}
    return(found_mode);
}


/*
 *  fbdevProbe -- Probe and initialize the hardware driver
 */

static Bool fbdevProbe(void)
{
    DisplayModePtr pMode, pEnd;

    open_framebuffer();

    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &initscrvar))
	FatalError("fbdevProbe: unable to get screen params (%s)\n",
		   strerror(errno));

    pMode = fbdevInfoRec.modes;
    if (pMode == NULL)
	FatalError("No modes supplied in XF86Config\n");

    if (!strcmp(pMode->name, "default")) {
	ErrorF("%s %s: Using default frame buffer video mode\n", XCONFIG_GIVEN,
	       fbdevInfoRec.name);
	fbdevInfoRec.depth = initscrvar.bits_per_pixel;
	fbdevInfoRec.bitsPerPixel = fbdevInfoRec.depth;
    } else {
	ErrorF("%s %s: Using XF86Config video mode database\n", XCONFIG_GIVEN,
	       fbdevInfoRec.name);
	UseModeDB = TRUE;
	pEnd = NULL;
	fbdevInfoRec.bitsPerPixel = fbdevInfoRec.depth;
	do {
	    DisplayModePtr pModeSv;

	    pModeSv = pMode->next;
	    if (!fbdevLookupMode(pMode))
		xf86DeleteMode(&fbdevInfoRec, pMode);
	    else {
		/*
		 *  Successfully looked up this mode.  If pEnd isn't
		 *  initialized, set it to this mode.
		 */
		if (pEnd == (DisplayModePtr)NULL)
		    pEnd = pMode;
	    }
	    pMode = pModeSv;
	} while (pMode != pEnd);
	fbdevInfoRec.SwitchMode = fbdevSwitchMode;
    }

    return(TRUE);
}

static void fbdevRestoreColors(ScreenPtr pScreen)
{
    ColormapPtr pmap = InstalledMaps[pScreen->myNum];
    int entries;
    Pixel ppix[256];
    xrgb prgb[256];
    xColorItem defs[256];
    int i;

    if (!pmap)
	pmap = (ColormapPtr) LookupIDByType(pScreen->defColormap, RT_COLORMAP);
    entries = pmap->pVisual->ColormapEntries;
    if (entries) {
	for (i = 0 ; i < entries; i++)
	    ppix[i] = i;
	QueryColors(pmap, entries, ppix, prgb);

	for (i = 0 ; i < entries; i++) {
	    defs[i].pixel = ppix[i];
	    defs[i].red = prgb[i].red;
	    defs[i].green = prgb[i].green;
	    defs[i].blue = prgb[i].blue;
	}

	fbdevStoreColors(pmap, entries, defs);
    }
}

static Bool fbdevSaveScreen(ScreenPtr pScreen, int on)
{
    switch (on) {
	case SCREEN_SAVER_ON:
	    {
		unsigned short map[256];
		int i;

		for (i = 0; i < 256; i++)
		    map[i] = 0;
		fbdevUpdateColormap(pScreen, 0, 256, map, map, map);
		return(TRUE);
	    }

	case SCREEN_SAVER_OFF:
	    fbdevRestoreColors(pScreen);
	    return(TRUE);
    }
    return(FALSE);
}

/*
 *  fbdevScreenInit -- Attempt to find and initialize a framebuffer
 *		       Most of the elements of the ScreenRec are filled in.
 *		       The video is enabled for the frame buffer...
 *
 *  Arguments:	scr_index	: The index of pScreen in the ScreenInfo
 *		pScreen		: The Screen to initialize
 *		argc		: The number of the Server's arguments.
 *		argv		: The arguments themselves. Don't change!
 */

static Bool fbdevScreenInit(int scr_index, ScreenPtr pScreen, int argc,
			    char **argv)
{
    int displayResolution = 75;	/* default to 75dpi */
    int dxres,dyres;
    extern int monitorResolution;
    struct fb_var_screeninfo *var = &initscrvar;
    static unsigned short bw[] = {
	0xffff, 0x0000
    };
    DisplayModePtr mode;
    int bpp, xsize, ysize, width;
    char *fbtype;

    open_framebuffer();

    /*
     *  Take display resolution from the -dpi flag if specified
     */

    if (monitorResolution)
	displayResolution = monitorResolution;

    dxres = displayResolution;
    dyres = displayResolution;

    mode = fbdevInfoRec.modes;
    if (!UseModeDB) {
	fbdev2xfree(var, mode);
	mode->name = "default";
	fbdevInfoRec.virtualX = var->xres_virtual;
	fbdevInfoRec.virtualY = var->yres_virtual;
    } else
	xfree2fbdev(mode, var);

    if (ioctl(fb_fd, FBIOPUT_VSCREENINFO, var))
	FatalError("fbdevScreenInit: unable to set screen params (%s)\n",
		   strerror(errno));
    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_fix))
	FatalError("ioctl(fd, FBIOGET_FSCREENINFO, ...)");

    fbdevInfoRec.chipset = fb_fix.id;
    fbdevInfoRec.videoRam = fb_fix.smem_len>>10;

    if (xf86Verbose) {
	ErrorF("%s %s: Frame buffer device: %s\n", XCONFIG_PROBED,
	       fbdevInfoRec.name, fbdevInfoRec.chipset);
	ErrorF("%s %s: Video memory: %dK\n", XCONFIG_PROBED, fbdevInfoRec.name,
	       fbdevInfoRec.videoRam);
	ErrorF("%s %s: Type %d type_aux %d bits_per_pixel %d\n", XCONFIG_PROBED,
	       fbdevInfoRec.name, fb_fix.type, fb_fix.type_aux,
	       var->bits_per_pixel);
    }

    if (serverGeneration == 1)
	fbdevVirtBase = xf86MapVidMem(scr_index, 0, 0, 0);

    /*
     *  Do we have a virtual screen, and support for panning?
     */
 
    if ((var->xres_virtual > var->xres && fb_fix.xpanstep) ||
	(var->yres_virtual > var->yres && fb_fix.ypanstep)) {
	fbdevInfoRec.AdjustFrame = fbdevAdjustFrame;
	fbdevAdjustFrame(fbdevInfoRec.frameX0, fbdevInfoRec.frameY0);
	ErrorF("%s %s: Enabled virtual desktop\n", XCONFIG_PROBED,
	       fbdevInfoRec.name, fbdevInfoRec.videoRam);
    } else
	ErrorF("%s %s: No virtual desktop\n", XCONFIG_PROBED,
	       fbdevInfoRec.name, fbdevInfoRec.videoRam);
 
    if (var->nonstd)
	FatalError("Can't handle non standard pixel format %d\n", var->nonstd);

    bpp = var->bits_per_pixel;
    xsize = var->xres_virtual;
    ysize = var->yres_virtual;
    width = fb_fix.line_length ? fb_fix.line_length<<3 : xsize;

    if (bpp == 1)
	switch (fb_fix.type) {
	    case FB_TYPE_INTERLEAVED_PLANES:
	    case FB_TYPE_PACKED_PIXELS:
	    case FB_TYPE_PLANES:
		switch(fb_fix.visual) {
		    case FB_VISUAL_MONO01:
			pScreen->blackPixel = 1;
			pScreen->whitePixel = 0;
			break;
		    case FB_VISUAL_MONO10:
			pScreen->blackPixel = 0;
			pScreen->whitePixel = 1;
			break;
		    case FB_VISUAL_PSEUDOCOLOR:
			pScreen->blackPixel = 1;
			pScreen->whitePixel = 0;
			fbdevUpdateColormap(NULL, 0, 2, bw, bw, bw);
			break;
		    default:
			FatalError("Can't handle visual %d\n", fb_fix.visual);
		}
		fbtype = "mfb";
		mfbScreenInit(pScreen, fbdevVirtBase, xsize, ysize, dxres,
			      dyres, width);
		fbdevPrivateIndexP = NULL;
		fbdevBitBlt = mfbDoBitblt;
		CreateDefColormap = mfbCreateDefColormap;
		break;

	    default:
		FatalError("Unknown format type %d\n", fb_fix.type);
	}
    else
	switch (fb_fix.type) {
	    case FB_TYPE_INTERLEAVED_PLANES:
		if (fb_fix.type_aux == 2)
		    switch(bpp) {
#ifdef CONFIG_IPLAN2p2
			case 2:
			    fbtype = "iplan2p2";
			    ipl2p2ScreenInit(pScreen, fbdevVirtBase, xsize,
					     ysize, dxres, dyres, width);
			    fbdevPrivateIndexP=&ipl2p2ScreenPrivateIndex;
			    fbdevBitBlt=ipl2p2DoBitblt;
			    CreateDefColormap=iplCreateDefColormap;
			    break;
#endif

#ifdef CONFIG_IPLAN2p4
			case 4:
			    fbtype = "iplan2p4";
			    ipl2p4ScreenInit(pScreen, fbdevVirtBase, xsize,
					     ysize, dxres, dyres, width);
			    fbdevPrivateIndexP=&ipl2p4ScreenPrivateIndex;
			    fbdevBitBlt=ipl2p4DoBitblt;
			    CreateDefColormap=iplCreateDefColormap;
			    break;
#endif

#ifdef CONFIG_IPLAN2p8
			case 8:
			    fbtype = "iplan2p8";
			    ipl2p8ScreenInit(pScreen, fbdevVirtBase, xsize,
			    		     ysize, dxres, dyres, width);
			    fbdevPrivateIndexP=&ipl2p8ScreenPrivateIndex;
			    fbdevBitBlt=ipl2p8DoBitblt;
			    CreateDefColormap=iplCreateDefColormap;
			    break;
#endif
			default:
			    FatalError("Can't handle interleaved planes with "
			    	       "%d planes\n", bpp);
		    }
		else
#ifdef CONFIG_ILBM
		if (bpp <= 8) {
		    int visuals = 0;

		    switch (fb_fix.visual) {
			case FB_VISUAL_MONO01:
			case FB_VISUAL_MONO10:
			    visuals = StaticGrayMask;
			    break;

			case FB_VISUAL_TRUECOLOR:
			    if (var->grayscale)
				visuals = StaticGrayMask|GrayScaleMask;
			    else
				visuals = ALL_VISUALS;
			    break;

			case FB_VISUAL_PSEUDOCOLOR:
			    if (var->grayscale)
				visuals = StaticGrayMask|GrayScaleMask;
			    else
				visuals = StaticGrayMask|GrayScaleMask|
					  StaticColorMask|PseudoColorMask;
			    break;

			case FB_VISUAL_DIRECTCOLOR:
			    if (var->grayscale)
				visuals = StaticGrayMask;
			    else
				visuals = DirectColorMask;
			    break;

			case FB_VISUAL_STATIC_PSEUDOCOLOR:
			case FB_VISUAL_STATIC_DIRECTCOLOR:
			    if (var->grayscale)
				visuals = StaticGrayMask;
			    else
				visuals = StaticColorMask;
			    break;

			default:
			    ErrorF("Unknown frame buffer visual %d. Trying "
				   "PSEUDOCOLOR\n", fb_fix.visual);
			    visuals = PseudoColorMask;
			    break;
		    }
		    if (!ilbmSetVisualTypes(bpp, visuals, var->red.length))
			FatalError("ilbmSetVisualTypes: FALSE\n");
		    width = fb_fix.line_length ? fb_fix.line_length<<3 :
						 (fb_fix.type_aux<<3)/bpp;
		    fbtype = "ilbm";
		    ilbmScreenInit(pScreen, fbdevVirtBase, xsize, ysize, dxres,
				   dyres, width);
		    fbdevPrivateIndexP = &ilbmScreenPrivateIndex;
		    fbdevBitBlt = ilbmDoBitblt;
		    CreateDefColormap = ilbmCreateDefColormap;
		} else
#endif /* CONFIG_ILBM */
		    FatalError("Can't handle interleaved planes with %d "
			       "planes\n", bpp);
		break;

	    case FB_TYPE_PLANES:
#ifdef CONFIG_AFB
		if (bpp <= 8) {
		    int visuals = 0;

		    switch (fb_fix.visual) {
			case FB_VISUAL_MONO01:
			case FB_VISUAL_MONO10:
			    visuals = StaticGrayMask;
			    break;

			case FB_VISUAL_TRUECOLOR:
			    if (var->grayscale)
				visuals = StaticGrayMask|GrayScaleMask;
			    else
				visuals = ALL_VISUALS;
			    break;

			case FB_VISUAL_PSEUDOCOLOR:
			    if (var->grayscale)
				visuals = StaticGrayMask|GrayScaleMask;
			    else
				visuals = StaticGrayMask|GrayScaleMask|
					  StaticColorMask|PseudoColorMask;
			    break;

			case FB_VISUAL_DIRECTCOLOR:
			    if (var->grayscale)
				visuals = StaticGrayMask;
			    else
				visuals = DirectColorMask;
			    break;

			case FB_VISUAL_STATIC_PSEUDOCOLOR:
			case FB_VISUAL_STATIC_DIRECTCOLOR:
			    if (var->grayscale)
				visuals = StaticGrayMask;
			    else
				visuals = StaticColorMask;
			    break;

			default:
			    ErrorF("Unknown frame buffer visual %d. Trying "
			    	   "PSEUDOCOLOR\n", fb_fix.visual);
			    visuals = PseudoColorMask;
			    break;
		    }
		    if (!afbSetVisualTypes(bpp, visuals, var->red.length))
			FatalError("afbSetVisualTypes: FALSE\n");
		    fbtype = "afb";
		    afbScreenInit(pScreen, fbdevVirtBase, xsize, ysize, dxres,
				  dyres, width);
		    fbdevPrivateIndexP = &afbScreenPrivateIndex;
		    fbdevBitBlt = afbDoBitblt;
		    CreateDefColormap = afbCreateDefColormap;
		} else
#endif /* CONFIG_AFB */
		    FatalError("Can't handle planes format with %d bits per "
		    	       "pixel\n", bpp);
		break;

	    case FB_TYPE_PACKED_PIXELS:
		width = fb_fix.line_length ? 8*fb_fix.line_length/bpp : xsize;
		switch (bpp) {
#ifdef CONFIG_CFB8
		    case 8:
			fbtype = "cfb8";
			cfbScreenInit(pScreen, fbdevVirtBase, xsize, ysize,
				      dxres, dyres, width);
			fbdevPrivateIndexP = NULL;
			fbdevBitBlt = cfbDoBitblt;
			CreateDefColormap = cfbCreateDefColormap;
			break;
#endif

#ifdef CONFIG_CFB16
		    case 16:
			fbtype = "cfb16";
			cfb16ScreenInit(pScreen, fbdevVirtBase, xsize, ysize,
					dxres, dyres, width);
			fbdevPrivateIndexP = &cfb16ScreenPrivateIndex;
			fbdevBitBlt = cfb16DoBitblt;
			CreateDefColormap = cfbCreateDefColormap;
			break;
#endif
    
#ifdef CONFIG_CFB32
		    case 32:
			fbtype = "cfb32";
			cfb32ScreenInit(pScreen, fbdevVirtBase, xsize, ysize,
					dxres, dyres, width);
			fbdevPrivateIndexP = &cfb32ScreenPrivateIndex;
			fbdevBitBlt = cfb32DoBitblt;
			CreateDefColormap = cfbCreateDefColormap;
			break;
#endif
    
		    default:
			FatalError("Can't handle packed pixels with %d bits "
				   "per pixel\n", bpp);
		}
		break;

	    default:
		FatalError("Unknown format type %d\n", fb_fix.type);
	}

    ErrorF("%s %s: Using %s driver\n", XCONFIG_PROBED, fbdevInfoRec.name,
	   fbtype);
    pScreen->CloseScreen = fbdevCloseScreen;
    pScreen->SaveScreen = (SaveScreenProcPtr)fbdevSaveScreen;

    if (fb_fix.visual != FB_VISUAL_MONO10 &&
	fb_fix.visual != FB_VISUAL_MONO01 &&
	fb_fix.visual != FB_VISUAL_DIRECTCOLOR &&
	fb_fix.visual != FB_VISUAL_TRUECOLOR) {
	pScreen->InstallColormap = fbdevInstallColormap;
	pScreen->UninstallColormap = fbdevUninstallColormap;
	pScreen->ListInstalledColormaps = fbdevListInstalledColormaps;
	pScreen->StoreColors = fbdevStoreColors;
    }
    miDCInitialize(pScreen, &xf86PointerScreenFuncs);

    if (!(*CreateDefColormap)(pScreen))
	return(FALSE);

    savepScreen = pScreen;

    return(TRUE);
}

/*
 *  fbdevEnterLeaveVT -- Grab/ungrab the current VT completely.
 */

static void fbdevEnterLeaveVT(Bool enter, int screen_idx)
{
    BoxRec pixBox;
    RegionRec pixReg;
    DDXPointRec pixPt;
    PixmapPtr pspix;
    ScreenPtr pScreen = savepScreen;

    if (!xf86Resetting && !xf86Exiting) {
	pixBox.x1 = 0; pixBox.x2 = pScreen->width;
	pixBox.y1 = 0; pixBox.y2 = pScreen->height;
	pixPt.x = 0; pixPt.y = 0;
	(pScreen->RegionInit)(&pixReg, &pixBox, 1);
	if (fbdevPrivateIndexP)
	    pspix = (PixmapPtr)pScreen->devPrivates[*fbdevPrivateIndexP].ptr;
	else
	    pspix = (PixmapPtr)pScreen->devPrivate;
    }

    if (enter) {
	fbdevVirtBase = xf86MapVidMem(screen_idx, 0, 0, 0);

	/*
	 *  point pspix back to fbdevVirtBase, and copy the dummy buffer to the
	 *  real screen.
	 */
	if (!xf86Resetting)
	    if (pspix->devPrivate.ptr != fbdevVirtBase && ppix) {
		pspix->devPrivate.ptr = fbdevVirtBase;
		(*fbdevBitBlt)(&ppix->drawable, &pspix->drawable, GXcopy,
			       &pixReg, &pixPt, ~0);
	    }
	if (ppix) {
	    (pScreen->DestroyPixmap)(ppix);
	    ppix = NULL;
	}

	if (!xf86Resetting) {
	    /* Update the colormap */
	    ColormapPtr pmap = InstalledMaps[pScreen->myNum];
	    int entries;
	    Pixel ppix[256];
	    xrgb prgb[256];
	    xColorItem defs[256];
	    int i;

	    if (!pmap)
		pmap = (ColormapPtr) LookupIDByType(pScreen->defColormap,
						    RT_COLORMAP);
	    entries = pmap->pVisual->ColormapEntries;
	    if (entries) {
		for (i = 0 ; i < entries; i++)
		ppix[i] = i;
		QueryColors (pmap, entries, ppix, prgb);

		for (i = 0 ; i < entries; i++) {
		    defs[i].pixel = ppix[i];
		    defs[i].red = prgb[i].red;
		    defs[i].green = prgb[i].green;
		    defs[i].blue = prgb[i].blue;
		}

		fbdevStoreColors(pmap, entries, defs);
	    }
	}
    } else {
	/*
	 *  Create a dummy pixmap to write to while VT is switched out.
	 *  Copy the screen to that pixmap
	 */
	if (!xf86Exiting) {
	    ppix = (pScreen->CreatePixmap)(pScreen, pScreen->width,
	    				   pScreen->height, pScreen->rootDepth);
	    if (ppix) {
		(*fbdevBitBlt)(&pspix->drawable, &ppix->drawable, GXcopy,
			       &pixReg, &pixPt, ~0);
		pspix->devPrivate.ptr = ppix->devPrivate.ptr;
	    }
	}
	xf86UnMapVidMem(screen_idx, 0, 0, 0);
    }
}
 
/*
 *  fbdevAdjustFrame -- Pan the display
 */
 
static void fbdevAdjustFrame(int x, int y)
{
    /*
     *  Some range checking first
     *
     *  It seems that XFree sometimes passes values that are too large.
     *  Maybe this is an indication that we're moving to a second display? :-)
     */
 
    if (x < 0)
	x = 0;
    else if (x > initscrvar.xres_virtual-initscrvar.xres)
	x = initscrvar.xres_virtual-initscrvar.xres;
    if (y < 0)
	y = 0;
    else if (y > initscrvar.yres_virtual-initscrvar.yres)
	y = initscrvar.yres_virtual-initscrvar.yres;
 
    initscrvar.xoffset = x;
    initscrvar.yoffset = y;
    ioctl(fb_fd, FBIOPAN_DISPLAY, &initscrvar);
}
 
/*
 *  fbdevCloseScreen -- Called to ensure video is enabled when server exits.
 */

static Bool fbdevCloseScreen(int screen_idx, ScreenPtr screen)
{
    /*
     *  Hmm... The server may shut down even if it is not running on the
     *  current vt. Let's catch this case here.
     */
    xf86Exiting = TRUE;
    if (xf86VTSema)
	fbdevEnterLeaveVT(LEAVE, screen_idx);
    else if (ppix) {
	/*
	 *  7-Jan-94 CEG: The server is not running on the current vt.
	 *  Free the screen snapshot taken when the server vt was left.
	 */
	(savepScreen->DestroyPixmap)(ppix);
	ppix = NULL;
    }
    return(TRUE);
}

/*
 *  fbdevValidMode -- Check whether a mode is valid. If necessary, values will
 *		      be rounded up by the Frame Buffer Device
 */

static Bool fbdevValidMode(DisplayModePtr mode, Bool verbose, int flag)
{
    struct fb_var_screeninfo var = initscrvar;
    Bool res = FALSE;

    xfree2fbdev(mode, &var);
    var.activate = FB_ACTIVATE_TEST;
    if (!ioctl(fb_fd, FBIOPUT_VSCREENINFO, &var) &&
    	var.bits_per_pixel == fbdevInfoRec.bitsPerPixel) {
	fbdev2xfree(&var, mode);
	if (var.xres_virtual > fbdevInfoRec.virtualX)
	    fbdevInfoRec.virtualX = var.xres_virtual;
	if (var.yres_virtual > fbdevInfoRec.virtualY)
	    fbdevInfoRec.virtualY = var.yres_virtual;
	res = TRUE;
    }
    return(res);
}

/*
 *  fbdevSwitchMode -- Change the video mode `on the fly'
 */

static Bool fbdevSwitchMode(DisplayModePtr mode)
{
    struct fb_var_screeninfo var = initscrvar;
    ScreenPtr pScreen = savepScreen;
    Bool res = FALSE;

    xfree2fbdev(mode, &var);
    var.xoffset = fbdevInfoRec.frameX0;
    var.yoffset = fbdevInfoRec.frameY0;
    var.activate = FB_ACTIVATE_NOW;

    if (!ioctl(fb_fd, FBIOPUT_VSCREENINFO, &var)) {
	/* Restore the colormap */
	fbdevRestoreColors(pScreen);
	initscrvar.xres = var.xres;
	initscrvar.yres = var.yres;
	res = TRUE;
    }

    return(res);
}


/*
 *  Convert timings between the XFree style and the Frame Buffer Device style
 */

static void xfree2fbdev(DisplayModePtr mode, struct fb_var_screeninfo *var)
{
    var->xres = mode->HDisplay;
    var->yres = mode->VDisplay;
    var->xres_virtual = fbdevInfoRec.virtualX;
    var->yres_virtual = fbdevInfoRec.virtualY;
    var->xoffset = var->yoffset = 0;
    var->bits_per_pixel = fbdevInfoRec.bitsPerPixel;
    var->pixclock = mode->Clock ? 1000000000/mode->Clock : 0;
    var->right_margin = mode->HSyncStart-mode->HDisplay;
    var->hsync_len = mode->HSyncEnd-mode->HSyncStart;
    var->left_margin = mode->HTotal-mode->HSyncEnd;
    var->lower_margin = mode->VSyncStart-mode->VDisplay;
    var->vsync_len = mode->VSyncEnd-mode->VSyncStart;
    var->upper_margin = mode->VTotal-mode->VSyncEnd;
    var->sync = 0;
    if (mode->Flags & V_PHSYNC)
	var->sync |= FB_SYNC_HOR_HIGH_ACT;
    if (mode->Flags & V_PVSYNC)
	var->sync |= FB_SYNC_VERT_HIGH_ACT;
    if (mode->Flags & V_PCSYNC)
	var->sync |= FB_SYNC_COMP_HIGH_ACT;
    if (mode->Flags & V_INTERLACE)
	var->vmode = FB_VMODE_INTERLACED;
    else if (mode->Flags & V_DBLSCAN)
	var->vmode = FB_VMODE_DOUBLE;
    else
    	var->vmode = FB_VMODE_NONINTERLACED;
}

static void fbdev2xfree(struct fb_var_screeninfo *var, DisplayModePtr mode)
{
    mode->Clock = var->pixclock ? 1000000000/var->pixclock : 28000000;
    mode->HDisplay = var->xres;
    mode->HSyncStart = mode->HDisplay+var->right_margin;
    mode->HSyncEnd = mode->HSyncStart+var->hsync_len;
    mode->HTotal = mode->HSyncEnd+var->left_margin;
    mode->VDisplay = var->yres;
    mode->VSyncStart = mode->VDisplay+var->lower_margin;
    mode->VSyncEnd = mode->VSyncStart+var->vsync_len;
    mode->VTotal = mode->VSyncEnd+var->upper_margin;
    mode->Flags = 0;
    mode->Flags |= var->sync & FB_SYNC_HOR_HIGH_ACT ? V_PHSYNC : V_NHSYNC;
    mode->Flags |= var->sync & FB_SYNC_VERT_HIGH_ACT ? V_PVSYNC : V_NVSYNC;
    mode->Flags |= var->sync & FB_SYNC_COMP_HIGH_ACT ? V_PCSYNC : V_NCSYNC;
    if ((var->vmode & FB_VMODE_MASK) == FB_VMODE_INTERLACED)
	mode->Flags |= V_INTERLACE;
    else if ((var->vmode & FB_VMODE_MASK) == FB_VMODE_DOUBLE)
	mode->Flags |= V_DBLSCAN;
    mode->SynthClock = mode->Clock;
    mode->CrtcHDisplay = mode->HDisplay;
    mode->CrtcHSyncStart = mode->HSyncStart;
    mode->CrtcHSyncEnd = mode->HSyncEnd;
    mode->CrtcHTotal = mode->HTotal;
    mode->CrtcVDisplay = mode->VDisplay;
    mode->CrtcVSyncStart = mode->VSyncStart;
    mode->CrtcVSyncEnd = mode->VSyncEnd;
    mode->CrtcVTotal = mode->VTotal;
    mode->CrtcHAdjusted = FALSE;
    mode->CrtcVAdjusted = FALSE;
}

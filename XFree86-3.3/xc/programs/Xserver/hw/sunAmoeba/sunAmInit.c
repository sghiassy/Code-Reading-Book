/* $XConsortium: sunAmInit.c,v 1.2 94/04/17 20:32:46 dpw Exp $ */
/*
 * sunAmInit.c --
 *	Amoeba implementation of initialization functions
 *      for screen/keyboard/mouse, etc.
 *
 * Copyright (c) 1987 by the Regents of the University of California
 * Copyright (c) 1994 by the Vrije Universiteit, Amsterdam.
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * and the Vrije Universiteit make no representations about
 * the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 *
 */

/* This file was partly derived from sunInit.c (5.49 94/02/21 10:21:02) */

/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or X Consortium
not be used in advertising or publicity pertaining to 
distribution  of  the software  without specific prior 
written permission. Sun and X Consortium make no 
representations about the suitability of this software for 
any purpose. It is provided "as is" without any express or 
implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

*******************************************************/

#include    "sun.h"
#include    "gcstruct.h"
#include    "mibstore.h"

Bool sunAutoRepeatHandlersInstalled;    /* FALSE each time InitOutput called */
Bool sunSwapLkeys = FALSE;
Bool sunFlipPixels = FALSE;
Bool sunFbInfo = FALSE;
Bool sunCG4Frob = FALSE;

/* maximum pixmap depth */
#ifndef SUNMAXDEPTH
#define SUNMAXDEPTH 8
#endif

typedef Bool initfunc (
#if NeedFunctionPrototypes
    int /* screen */,
    ScreenPtr /* pScreen */,
    int /* argc */,
    char** /* argv */
#endif
);

extern initfunc sunBW2Init;
extern initfunc sunCG6Init;
extern initfunc sunCG3Init;

extern Bool sunBW2Probe();
extern Bool sunCG3CProbe();
extern Bool sunCG6CProbe();

sunFbDataRec sunFbData[] = {
      sunBW2Init, "bwtwo0",   sunBW2Probe,
#if SUNMAXDEPTH > 1
      sunCG6Init, "cgsix0",   sunCG6CProbe, 
      sunCG3Init, "cgthree0", sunCG3CProbe,    
#endif
};

#define NUMSCREENS (sizeof(sunFbData)/sizeof(sunFbData[0]))

fbFd sunFbs[MAXSCREENS];

static PixmapFormatRec	formats[] = {
    { 1, 1, BITMAP_SCANLINE_PAD	} /* 1-bit deep */
#if SUNMAXDEPTH > 1
    ,{ 8, 8, BITMAP_SCANLINE_PAD} /* 8-bit deep */
#if SUNMAXDEPTH > 8
    ,{ 12, 24, BITMAP_SCANLINE_PAD } /* 12-bit deep */
    ,{ 24, 32, BITMAP_SCANLINE_PAD } /* 24-bit deep */
#endif
#endif
};
#define NUMFORMATS	(sizeof formats)/(sizeof formats[0])

void OsVendorInit(
#if NeedFunctionPrototypes
    void
#endif
)
{
    /* nothing special to do for Amoeba */
}

/*-
 *-----------------------------------------------------------------------
 * InitOutput --
 *	Initialize screenInfo for all actually accessible framebuffers.
 *	The
 *
 * Results:
 *	screenInfo init proc field set
 *
 * Side Effects:
 *	None
 *
 *-----------------------------------------------------------------------
 */

void InitOutput(pScreenInfo, argc, argv)
    ScreenInfo 	  *pScreenInfo;
    int     	  argc;
    char    	  **argv;
{
    static Bool sunDevsInited = FALSE;
    static int  screen_type[MAXSCREENS];
    static int  nscreens;
    int     	i, scr;

    if (!monitorResolution)
	monitorResolution = 90;
    pScreenInfo->imageByteOrder = IMAGE_BYTE_ORDER;
    pScreenInfo->bitmapScanlineUnit = BITMAP_SCANLINE_UNIT;
    pScreenInfo->bitmapScanlinePad = BITMAP_SCANLINE_PAD;
    pScreenInfo->bitmapBitOrder = BITMAP_BIT_ORDER;

    pScreenInfo->numPixmapFormats = NUMFORMATS;
    for (i=0; i< NUMFORMATS; i++)
        pScreenInfo->formats[i] = formats[i];
    sunAutoRepeatHandlersInstalled = FALSE;

    if (!sunDevsInited) {
	/* first time ever */
	nscreens = 0;
	for (i = 0;
	     sunFbData[i].probeProc != NULL && nscreens < NUMSCREENS;
	     i++)
	{
            if ((*sunFbData[i].probeProc)(pScreenInfo, nscreens,
					  sunFbData[i].name, argc, argv))
	    {
		printf("Found screen %s (%d)\n", sunFbData[i].name, i);
		screen_type[nscreens] = i;
		nscreens++;
		break; /* we only support one screen, currently */
	    }
	}
        sunDevsInited = TRUE;
    }

    for (scr = 0; scr < nscreens; scr++) {
	if (AddScreen (sunFbData[screen_type[scr]].init, argc, argv) < 0) {
	    extern void AbortDDX();

	    FatalError("AddScreen failed");
	    AbortDDX(); /* forcing loader not to ignore SunIo.o */
	}
    }
}

/*-
 *-----------------------------------------------------------------------
 * InitInput --
 *	Initialize all supported input devices...what else is there
 *	besides pointer and keyboard?
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	Two DeviceRec's are allocated and registered as the system pointer
 *	and keyboard devices.
 *
 *-----------------------------------------------------------------------
 */
void InitInput(argc, argv)
    int     	  argc;
    char    	  **argv;
{
    int		i;
    DevicePtr	p, k;
    extern Bool mieqInit();

    p = AddInputDevice(sunMouseProc, TRUE);
    k = AddInputDevice(sunKbdProc, TRUE);
    if (!p || !k)
	FatalError("failed to create input devices in InitInput");

    RegisterPointerDevice(p);
    RegisterKeyboardDevice(k);
    miRegisterPointerDevice(screenInfo.screens[0], p);
    (void) mieqInit (k, p);
}


/* The rest of this file is identical to the code in sunInit.c */

#if SUNMAXDEPTH == 8

Bool
sunCfbSetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width, bpp)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
    int	bpp;			/* bits per pixel of root */
{
    return cfbSetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy,
			  width);
}

Bool
sunCfbFinishScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width, bpp)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
    int bpp;			/* bits per pixel of root */
{
    return cfbFinishScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy,
			       width);
}

Bool
sunCfbScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width, bpp)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
    int bpp;			/* bits per pixel of root */
{
    return cfbScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width);
}

#else /* SUNMAXDEPTH != 8 */
#if SUNMAXDEPTH == 32

static Bool
sunCfbCreateGC(pGC)
    GCPtr   pGC;
{
    if (pGC->depth == 1)
    {
	return mfbCreateGC (pGC);
    }
    else if (pGC->depth <= 8)
    {
	return cfbCreateGC (pGC);
    }
    else if (pGC->depth <= 16)
    {
	return cfb16CreateGC (pGC);
    }
    else if (pGC->depth <= 32)
    {
	return cfb32CreateGC (pGC);
    }
    return FALSE;
}

static void
sunCfbGetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart)
    DrawablePtr		pDrawable;	/* drawable from which to get bits */
    int			wMax;		/* largest value of all *pwidths */
    register DDXPointPtr ppt;		/* points to start copying from */
    int			*pwidth;	/* list of number of bits to copy */
    int			nspans;		/* number of scanlines to copy */
    char		*pdstStart;	/* where to put the bits */
{
    switch (pDrawable->bitsPerPixel) {
    case 1:
	mfbGetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart);
	break;
    case 8:
	cfbGetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart);
	break;
    case 16:
	cfb16GetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart);
	break;
    case 32:
	cfb32GetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart);
	break;
    }
    return;
}

static void
sunCfbGetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine)
    DrawablePtr pDrawable;
    int		sx, sy, w, h;
    unsigned int format;
    unsigned long planeMask;
    char	*pdstLine;
{
    switch (pDrawable->bitsPerPixel)
    {
    case 1:
	mfbGetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
	break;
    case 8:
	cfbGetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
	break;
    case 16:
	cfb16GetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
	break;
    case 32:
	cfb32GetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
	break;
    }
}

Bool
sunCfbSetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width, bpp)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
    int	bpp;			/* bits per pixel of root */
{
    extern int		cfbWindowPrivateIndex;
    extern int		cfbGCPrivateIndex;
    int ret;

    switch (bpp) {
    case 8:
	ret = cfbSetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width);
	break;
    case 16:
	ret = cfb16SetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width);
	break;
    case 32:
	ret = cfb32SetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width);
	break;
    default:
	return FALSE;
    }
    pScreen->CreateGC = sunCfbCreateGC;
    pScreen->GetImage = sunCfbGetImage;
    pScreen->GetSpans = sunCfbGetSpans;
    return ret;
}

extern miBSFuncRec	cfbBSFuncRec, cfb16BSFuncRec, cfb32BSFuncRec;
extern int  cfb16ScreenPrivateIndex, cfb32ScreenPrivateIndex;
extern Bool cfbCloseScreen(), cfb16CloseScreen(), cfb32CloseScreen();

Bool
sunCfbFinishScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width, bpp)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
    int bpp;
{
    int		i;
    pointer	oldDevPrivate;
    VisualPtr	visuals;
    int		nvisuals;
    DepthPtr	depths;
    int		ndepths;
    VisualID	defaultVisual;
    int		rootdepth = 0;
    miBSFuncPtr	bsFuncs;

    if (!cfbInitVisuals(&visuals, &depths, &nvisuals, &ndepths,
			&rootdepth, &defaultVisual, 1 << (bpp - 1), 8))
	return FALSE;
    oldDevPrivate = pScreen->devPrivate;
    if (! miScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width,
			rootdepth, ndepths, depths,
			defaultVisual, nvisuals, visuals,
			(miBSFuncPtr) 0))
	return FALSE;
    switch (bpp)
    {
    case 8:
	pScreen->CloseScreen = cfbCloseScreen;
	bsFuncs = &cfbBSFuncRec;
	break;
    case 16:
	pScreen->CloseScreen = cfb16CloseScreen;
	pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr =
	    pScreen->devPrivate;
	pScreen->devPrivate = oldDevPrivate;
	bsFuncs = &cfb16BSFuncRec;
	break;
    case 32:
	pScreen->CloseScreen = cfb32CloseScreen;
	pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr =
	    pScreen->devPrivate;
	pScreen->devPrivate = oldDevPrivate;
	bsFuncs = &cfb32BSFuncRec;
	break;
    }
    miInitializeBackingStore (pScreen, bsFuncs);
    return TRUE;
}


Bool
sunCfbScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width, bpp)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
    int bpp;
{
    if (!sunCfbSetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy,
			   width, bpp))
	return FALSE;
    return sunCfbFinishScreenInit(pScreen, pbits, xsize, ysize, dpix,
				  dpiy, width, bpp);
}

#endif  /* SUNMAXDEPTH == 32 */
#endif  /* SUNMAXDEPTH */

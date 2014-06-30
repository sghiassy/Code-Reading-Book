/* $XConsortium: mdepthinit.c,v 1.7 94/04/17 20:29:56 dpw Exp $ */
/*

Copyright (c) 1992  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "resource.h"
#include "colormap.h"
#include "colormapst.h"
#include "mi.h"
#include "mistruct.h"
#include "dix.h"
#include "gcstruct.h"
#include "mibstore.h"

extern int defaultColorVisualClass;

#define BitsPerPixel(d) (\
    (1 << PixmapWidthPaddingInfo[d].padBytesLog2) * 8 / \
    (PixmapWidthPaddingInfo[d].padRoundUp+1))

#ifndef SINGLEDEPTH

Bool
mcfbCreateGC(pGC)
    GCPtr   pGC;
{
    switch (BitsPerPixel(pGC->depth)) {
    case 1:
	return mfbCreateGC (pGC);
    case 8:
	return cfbCreateGC (pGC);
    case 16:
	return cfb16CreateGC (pGC);
    case 32:
	return cfb32CreateGC (pGC);
    }
    return FALSE;
}

void
mcfbGetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart)
    DrawablePtr		pDrawable;	/* drawable from which to get bits */
    int			wMax;		/* largest value of all *pwidths */
    register DDXPointPtr ppt;		/* points to start copying from */
    int			*pwidth;	/* list of number of bits to copy */
    int			nspans;		/* number of scanlines to copy */
    char		*pdstStart;	/* where to put the bits */
{
    switch (BitsPerPixel(pDrawable->depth)) {
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

void
mcfbGetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine)
    DrawablePtr pDrawable;
    int		sx, sy, w, h;
    unsigned int format;
    unsigned long planeMask;
    char	*pdstLine;
{
    switch (BitsPerPixel(pDrawable->depth)) 
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
mcfbSetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width, bpp, depth)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
    int depth;			/* depth of root */
    int	bpp;			/* bits per pixel of root */
{
    extern int		cfbWindowPrivateIndex;
    extern int		cfbGCPrivateIndex;

    switch (bpp) {
    case 8:
	cfbSetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width);
	break;
    case 16:
	cfb16SetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width);
	break;
    case 32:
	cfb32SetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width);
	break;
    default:
	return FALSE;
    }
    pScreen->CreateGC = mcfbCreateGC;
    pScreen->GetImage = mcfbGetImage;
    pScreen->GetSpans = mcfbGetSpans;
    return TRUE;
}

extern int  cfb16ScreenPrivateIndex, cfb32ScreenPrivateIndex;

mcfbFinishScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width, bpp, depth)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
{
    int		i;
    pointer	oldDevPrivate;
    VisualPtr	visuals;
    int		nvisuals;
    DepthPtr	depths;
    int		ndepths;
    VisualID	defaultVisual;
    int		rootdepth;
    miBSFuncPtr	bsFuncs;
    extern miBSFuncRec	cfbBSFuncRec, cfb16BSFuncRec, cfb32BSFuncRec;
    extern Bool		cfbCloseScreen(), cfb16CloseScreen(), cfb32CloseScreen();

    rootdepth = depth;
    if (!cfbInitVisuals(&visuals, &depths, &nvisuals, &ndepths, &rootdepth, &defaultVisual, 1 << (bpp - 1), 8))
	return FALSE;
    rootdepth = depth;
    oldDevPrivate = pScreen->devPrivate;
    if (! miScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width,
			rootdepth, ndepths, depths,
			defaultVisual, nvisuals, visuals,
			(miBSFuncPtr) 0))
	return FALSE;
    switch(bpp)
    {
    case 8:
	pScreen->CloseScreen = cfbCloseScreen;
	bsFuncs = &cfbBSFuncRec;
	break;
    case 16:
	pScreen->CloseScreen = cfb16CloseScreen;
	pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr = pScreen->devPrivate;
	pScreen->devPrivate = oldDevPrivate;
	bsFuncs = &cfb16BSFuncRec;
	break;
    case 32:
	pScreen->CloseScreen = cfb32CloseScreen;
	pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr = pScreen->devPrivate;
	pScreen->devPrivate = oldDevPrivate;
	bsFuncs = &cfb32BSFuncRec;
	break;
    }
    miInitializeBackingStore (pScreen, bsFuncs);
    return TRUE;
}


/* dts * (inch/dot) * (25.4 mm / inch) = mm */

Bool
mcfbScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width, bpp, depth)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
{
    if (!mcfbSetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width, bpp, depth))
	return FALSE;
    return mcfbFinishScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width, bpp, depth);
}

void
mcfbFillInMissingPixmapDepths(bitsPerDepth)
    int *bitsPerDepth;
{
    int i, j;

    j = 0;
    for (i = 1; i <= 32; i++)
    {
	if (bitsPerDepth[i])
	    j |= 1 << (bitsPerDepth[i] - 1);
    }
    if (!(j & (1 << 7)))
	bitsPerDepth[8] = 8;
    if (!(j & (1 << 15)))
	bitsPerDepth[12] = 16;
    if (!(j & (1 << 31)))
	bitsPerDepth[24] = 32;
}

#else /* SINGLEDEPTH */

/* stuff for 8-bit only server */

Bool
mcfbScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width, bpp, depth)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
{
    return cfbScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy,
			 width, bpp, depth);
}

void
mcfbFillInMissingPixmapDepths(bitsPerDepth)
    int *bitsPerDepth;
{
    /* This does something useful in the multidepth case.  We don't
     * do anything in the single depth case, but we still have to provide
     * the function for linking.
     */
}

#endif /* SINGLEDEPTH */

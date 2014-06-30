/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86spans.c,v 3.1 1996/11/24 09:57:23 dawes Exp $ */

/*
 * Copyright 1996  The XFree86 Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * HARM HANEMAAYER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Written by Harm Hanemaayer (H.Hanemaayer@inter.nl.net).
 */


#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "regionstr.h"
/* PSZ doesn't matter. */
#define PSZ 8
#include "cfb.h"

#include "xf86.h"
#include "xf86xaa.h"
#include "xf86local.h"

/*
 * Fill solid spans using xf86AccelInfoRec.FillSpansSolid,
 * handles clipping.
 */

void
xf86FillSpans(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GC		*pGC;
    int		nInit;		/* number of spans to fill */
    DDXPointPtr pptInit;	/* pointer to list of start points */
    int *pwidthInit;		/* pointer to list of n widths */
    int fSorted;
{
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */

    if (!(pGC->planemask))
	return;

    n = nInit * miFindMaxBand( cfbGetCompositeClip(pGC) );
    if ( n == 0 )
	return;
    pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
	if (ppt) DEALLOCATE_LOCAL(ppt);
	if (pwidth) DEALLOCATE_LOCAL(pwidth);
	return;
    }
    n = miClipSpans( cfbGetCompositeClip(pGC),
		     pptInit, pwidthInit, nInit, 
		     ppt, pwidth, fSorted);

    xf86AccelInfoRec.FillSpansSolid(n, ppt, pwidth, fSorted, pGC->fgPixel,
        pGC->alu);

    DEALLOCATE_LOCAL(ppt);
    DEALLOCATE_LOCAL(pwidth);
}


/*
 * This is only called for solid spans, if there is no specific
 * FillSpans function, but there is a suitable low-level FillRectSolid
 * for the selected rop and planemask.
 *
 * It does a little optimization by combining spans on consecutive
 * scanlines that have the same position and width (this only happens
 * if the spans are sorted).
 */

void
xf86FillSpansAsRects(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GC		*pGC;
    int		nInit;		/* number of spans to fill */
    DDXPointPtr pptInit;	/* pointer to list of start points */
    int *pwidthInit;		/* pointer to list of n widths */
    int fSorted;
{
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    int i, h;

    if (!(pGC->planemask))
	return;

    n = nInit * miFindMaxBand( cfbGetCompositeClip(pGC) );
    if ( n == 0 )
	return;
    pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
	if (ppt) DEALLOCATE_LOCAL(ppt);
	if (pwidth) DEALLOCATE_LOCAL(pwidth);
	return;
    }
    n = miClipSpans( cfbGetCompositeClip(pGC),
		     pptInit, pwidthInit, nInit, 
		     ppt, pwidth, fSorted);

    if (n == 0) {
        DEALLOCATE_LOCAL(ppt);
        DEALLOCATE_LOCAL(pwidth);
        return;
    }

    xf86AccelInfoRec.SetupForFillRectSolid(pGC->fgPixel, pGC->alu,
        pGC->planemask);
    h = 1;
    for (i = 1; i < n; i++)
        if (ppt[i - 1].y == ppt[i].y - 1
        && ppt[i - 1].x == ppt[i].x && pwidth[i - 1] == pwidth[i])
            /* Combine these spans. */
            h++;
        else {
            if (pwidth[i - 1] > 0)
	        xf86AccelInfoRec.SubsequentFillRectSolid(
	            ppt[i - 1].x, ppt[i - 1].y - h + 1, pwidth[i - 1], h);
	    h = 1;
        }
    if (pwidth[i - 1] > 0)
        xf86AccelInfoRec.SubsequentFillRectSolid(
            ppt[i - 1].x, ppt[i - 1].y - h + 1, pwidth[i - 1], h);

    DEALLOCATE_LOCAL(ppt);
    DEALLOCATE_LOCAL(pwidth);

    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
        xf86AccelInfoRec.Sync();
}


/*
 * Stipple and tile-filled spans, using PolyFillRect (and thus using 
 * any accelerated code). Adapted from mach64fs.c.
 *
 * The Mach64 code defines a threshold of 12 spans (before clipping) for
 * using accelerated functions (when not doing stipples at > 8bpp). I wonder
 * whether this is optimal.
 */

#define MIN_SPANS 12  /* below this number, use cfb */

static void DoPatternedFillSpans();

void
xf86FillSpansTiled(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
    int         fSorted;
{
    int n;                      /* number of spans to fill */
    register DDXPointPtr ppt;   /* pointer to list of start points */
    register int *pwidth;       /* pointer to list of n widths */
    DDXPointPtr initPpt;
    int *initPwidth;
    cfbPrivGCPtr devPriv;

    if (nInit <= 0)
        return;
    if (!(pGC->planemask))
        return;

    devPriv = cfbGetGCPrivate(pGC);

    if (nInit <= MIN_SPANS) {
        xf86GCInfoRec.FillSpansFallBack(
            pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted);
	return;
    }

    n = nInit * miFindMaxBand(devPriv->pCompositeClip);
    initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    initPpt = ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans(devPriv->pCompositeClip, pptInit, pwidthInit, nInit,
                    ppt, pwidth, fSorted);

    if (n > 0)
        DoPatternedFillSpans (pDrawable, pGC, n, ppt, pwidth);

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}

/* The following function is for both stipples and opaque stipples. */

void
xf86FillSpansStippled(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
    int         fSorted;
{
    int n;                      /* number of spans to fill */
    register DDXPointPtr ppt;   /* pointer to list of start points */
    register int *pwidth;       /* pointer to list of n widths */
    DDXPointPtr initPpt;
    int *initPwidth;
    cfbPrivGCPtr devPriv;

    if (nInit <= 0)
        return;
    if (!(pGC->planemask))
        return;

    devPriv = cfbGetGCPrivate(pGC);

    if ((nInit <= MIN_SPANS) && (pDrawable->depth == 8)) {
        xf86GCInfoRec.FillSpansFallBack(
            pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted);
	return;
    }

    n = nInit * miFindMaxBand(devPriv->pCompositeClip);
    initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    initPpt = ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans(devPriv->pCompositeClip, pptInit, pwidthInit, nInit,
                    ppt, pwidth, fSorted);

    if (n > 0)
        DoPatternedFillSpans (pDrawable, pGC, n, ppt, pwidth);

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}

static void
DoPatternedFillSpans (pDrawable, pGC, nInit, pptInit, pwidthInit)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
{
    register DDXPointPtr pSpanPt;       /* start points for the current span */
    register int        *pSpanWidth;    /* width of the current span */
    register int         nSpans;
    register xRectangle *pRect;
    xRectangle          *pRectInit;
    BoxRec               nullBox;
    RegionPtr            oldpCompositeClip;
    RegionRec            nullClip;
    cfbPrivGC           *devPriv;

    /*
     * Basically, spin up the spans into a list of 1 pixel high 
     * rectangles and let PolyFillRect handle the tiling.
     */
    devPriv = cfbGetGCPrivate(pGC);

    pSpanPt = pptInit;
    pSpanWidth = pwidthInit;

    pRectInit = (xRectangle *)ALLOCATE_LOCAL(nInit * sizeof(xRectangle));
    if (!pRectInit)
        return;

    pRect = pRectInit;

    for(nSpans = nInit; nSpans > 0; nSpans--)
    {
        pRect->x = pSpanPt->x - pDrawable->x;
        pRect->y = pSpanPt++->y - pDrawable->y;
        pRect->width = *pSpanWidth++;
        pRect++->height = 1;
    }

    nullBox.x1 = 0;
    nullBox.y1 = 0;
    nullBox.x2 = 32767;
    nullBox.y2 = 32767;

    (*pGC->pScreen->RegionInit)(&nullClip, &nullBox, 1); 

    oldpCompositeClip = devPriv->pCompositeClip;
    devPriv->pCompositeClip = &nullClip;
    
    xf86PolyFillRect (pDrawable, pGC, nInit, pRectInit);

    devPriv->pCompositeClip = oldpCompositeClip;

    (*pGC->pScreen->RegionUninit)(&nullClip);
    DEALLOCATE_LOCAL(pRectInit);
}

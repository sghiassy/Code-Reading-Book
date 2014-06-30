/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64fs.c,v 3.3 1996/12/23 06:39:16 dawes Exp $ */
/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or MIT not be used in
advertising or publicity pertaining to distribution  of  the
software  without specific prior written permission. Sun and
M.I.T. make no representations about the suitability of this
software for any purpose. It is provided "as is" without any
express or implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/

/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.
Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)

******************************************************************/
/* $XConsortium: mach64fs.c /main/3 1996/02/21 17:28:35 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "cfb.h"
#include "cfb16.h"
#include "cfb32.h"
#include "mach64.h"

#define MIN_SPANS 12  /* below this number, use cfb */

static void DoPatternedFillSpans();


void
mach64SolidFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
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


    if (!(pGC->planemask))
        return;

    devPriv = cfbGetGCPrivate(pGC);

    if (!xf86VTSema)
    {
	switch (pDrawable->bitsPerPixel)
	{
	    case 8:
		cfbSolidSpansGeneral(pDrawable, pGC, nInit, pptInit, 
				     pwidthInit, fSorted);
		break;
	    case 16:
		cfb16SolidSpansGeneral(pDrawable, pGC, nInit, pptInit, 
				     pwidthInit, fSorted);
		break;
	    case 32:
		cfb32SolidSpansGeneral(pDrawable, pGC, nInit, pptInit, 
				     pwidthInit, fSorted);
		break;
	    default:
		ErrorF("mach64SolidFSpans: unsupported depth %d \n",
			pDrawable->bitsPerPixel);
	}
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

    WaitQueue(5);
    regw(DP_FRGD_CLR, pGC->fgPixel);
    regw(DP_MIX, (mach64alu[pGC->alu] << 16) | MIX_DST);
    regw(DP_WRITE_MASK, pGC->planemask);
    regw(DP_SRC, FRGD_SRC_FRGD_CLR);
    regw(DST_CNTL, DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM);

    while (n--) 
    {
        WaitQueue(2);
        regw(DST_Y_X, ((ppt->x << 16) | (ppt->y & 0x0000ffff)));
        regw(DST_HEIGHT_WIDTH, ((*pwidth << 16) | 1));

	ppt++;
	pwidth++;
    }

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);

    WaitIdleEmpty(); /* Make sure that all commands have finished */
}


void
mach64TiledFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
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


    if (!(pGC->planemask))
        return;

    devPriv = cfbGetGCPrivate(pGC);

    if ((!xf86VTSema) || (nInit <= MIN_SPANS))
    {
	switch (pDrawable->bitsPerPixel)
	{
	    case 8:
		cfbUnnaturalTileFS(pDrawable, pGC, nInit, pptInit, 
				     pwidthInit, fSorted);
		break;
	    case 16:
		cfb16UnnaturalTileFS(pDrawable, pGC, nInit, pptInit, 
				     pwidthInit, fSorted);
		break;
	    case 32:
		cfb32UnnaturalTileFS(pDrawable, pGC, nInit, pptInit, 
				     pwidthInit, fSorted);
		break;
	    default:
		ErrorF("mach64TiledFSpans: unsupported depth %d \n",
			pDrawable->bitsPerPixel);
	}
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

    DoPatternedFillSpans (pDrawable, pGC, n, ppt, pwidth);

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}

void
mach64StipFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
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


    if (!(pGC->planemask))
        return;

    devPriv = cfbGetGCPrivate(pGC);

    if ((!xf86VTSema) || ((nInit <= MIN_SPANS) && (pDrawable->depth == 8)))
    {
	switch (pDrawable->bitsPerPixel)
	{
	    case 8:
		if (devPriv->pRotatedPixmap)
		    cfb8Stipple32FS(pDrawable, pGC, nInit, pptInit,
				    pwidthInit, fSorted);
		else
		    cfbUnnaturalStippleFS(pDrawable, pGC, nInit, pptInit, 
				          pwidthInit, fSorted);
		break;
	    case 16:
		cfb16UnnaturalStippleFS(pDrawable, pGC, nInit, pptInit, 
				     pwidthInit, fSorted);
		break;
	    case 32:
		cfb32UnnaturalStippleFS(pDrawable, pGC, nInit, pptInit, 
				     pwidthInit, fSorted);
		break;
	    default:
		ErrorF("mach64StipFSpans: unsupported depth %d \n",
			pDrawable->bitsPerPixel);
	}
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

    DoPatternedFillSpans (pDrawable, pGC, n, ppt, pwidth);

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}

void
mach64OStipFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
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


    if (!(pGC->planemask))
        return;

    devPriv = cfbGetGCPrivate(pGC);

    if ((!xf86VTSema) || ((nInit <= MIN_SPANS) && (pDrawable->depth == 8)))
    {
	switch (pDrawable->bitsPerPixel)
	{
	    case 8:
		if (devPriv->pRotatedPixmap)
		    cfb8OpaqueStipple32FS(pDrawable, pGC, nInit, pptInit,
					  pwidthInit, fSorted);
		else
		    cfbUnnaturalStippleFS(pDrawable, pGC, nInit, pptInit, 
				          pwidthInit, fSorted);
		break;
	    case 16:
		cfb16UnnaturalStippleFS(pDrawable, pGC, nInit, pptInit, 
				     pwidthInit, fSorted);
		break;
	    case 32:
		cfb32UnnaturalStippleFS(pDrawable, pGC, nInit, pptInit, 
				     pwidthInit, fSorted);
		break;
	    default:
		ErrorF("mach64OStipFSpans: unsupported depth %d \n",
			pDrawable->bitsPerPixel);
	}
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
    nullBox.x2 = mach64VirtX;
    nullBox.y2 = mach64VirtY;

    (*pGC->pScreen->RegionInit)(&nullClip, &nullBox, 1); 

    oldpCompositeClip = devPriv->pCompositeClip;
    devPriv->pCompositeClip = &nullClip;
    
    mach64PolyFillRect (pDrawable, pGC, nInit, pRectInit);

    devPriv->pCompositeClip = oldpCompositeClip;

    (*pGC->pScreen->RegionUninit)(&nullClip);
    DEALLOCATE_LOCAL(pRectInit);
}

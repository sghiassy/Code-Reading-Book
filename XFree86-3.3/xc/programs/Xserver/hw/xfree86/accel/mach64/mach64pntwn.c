/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64pntwn.c,v 3.3 1996/12/23 06:39:25 dawes Exp $ */
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.
Copyright 1993,1994 by Kevin E. Martin, Chapel Hill, North Carolina.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)

DIGITAL AND KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL DIGITAL OR KEVIN E. MARTIN BE LIABLE FOR
ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

******************************************************************/
/* $XConsortium: mach64pntwn.c /main/3 1996/02/21 17:29:06 kaleb $ */

#include "X.h"

#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "cfb.h"
#include "cfb16.h"
#include "cfb32.h"
#include "cfbmskbits.h"
#include "mach64.h"

extern void miPaintWindow();
static void mach64FillBoxSolid();
static void mach64FillBoxTile();

void
mach64PaintWindow(pWin, pRegion, what)
    WindowPtr	pWin;
    RegionPtr	pRegion;
    int		what;
{
    register cfbPrivWin	*pPrivWin;

    if (!xf86VTSema)
    {
        miPaintWindow (pWin, pRegion, what);
	return;
    }

    pPrivWin = (cfbPrivWin *)(pWin->devPrivates[cfbWindowPrivateIndex].ptr);

    switch (what) {
    case PW_BACKGROUND:
	switch (pWin->backgroundState) {
	case None:
	    return;
	case ParentRelative:
	    do {
		pWin = pWin->parent;
	    } while (pWin->backgroundState == ParentRelative);
	    (*pWin->drawable.pScreen->PaintWindowBackground)(pWin, pRegion,
							     what);
	    return;
	case BackgroundPixmap:
	    mach64FillBoxTile ((DrawablePtr)pWin, pRegion,
		               pWin->background.pixmap);
	    return;
	case BackgroundPixel:
	    mach64FillBoxSolid ((DrawablePtr)pWin, pRegion,
				pWin->background.pixel);
	    return;
    	}
    	break;
    case PW_BORDER:
	if (pWin->borderIsPixel)
	{
	    mach64FillBoxSolid ((DrawablePtr)pWin, pRegion,
				pWin->border.pixel);
	    return;
	}
	else
	{
	    mach64FillBoxTile ((DrawablePtr)pWin, pRegion,
			       pWin->border.pixmap);
	    return;
	}
	break;
    }
    miPaintWindow (pWin, pRegion, what);
}


static void
mach64FillBoxSolid (pDrawable, pRegion, pixel)
    DrawablePtr     pDrawable;
    RegionPtr       pRegion;
    unsigned long   pixel;
{
    register int w,h;
    int nBox;
    BoxPtr pBox;

    nBox = REGION_NUM_RECTS(pRegion);
    pBox = REGION_RECTS(pRegion);

    WaitQueue(5);
    regw(DP_FRGD_CLR, pixel);
    regw(DP_MIX, (MIX_SRC << 16) | MIX_DST);
    regw(DP_WRITE_MASK, 0xffffffff);
    regw(DP_SRC, FRGD_SRC_FRGD_CLR);
    regw(DST_CNTL, DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM);

    for (; nBox; nBox--, pBox++) {
        h = pBox->y2 - pBox->y1;
        w = pBox->x2 - pBox->x1;

        if ((w >= 0) && (h >= 0)) {
            WaitQueue(2);
            regw(DST_Y_X, ((pBox->x1 << 16) | (pBox->y1 & 0x0000ffff)));
            regw(DST_HEIGHT_WIDTH, ((w << 16) | (h & 0x0000ffff)));
        }
    }
    WaitIdleEmpty(); /* Make sure that all commands have finished */
}


static void
mach64FillBoxTile (pDrawable, pRegion, pPixmap)
    DrawablePtr     pDrawable;
    RegionPtr       pRegion;
    PixmapPtr       pPixmap;
{
    ScreenPtr   pScreen = pDrawable->pScreen;
    GCPtr       pGC;
    cfbPrivGC  *devPriv;
    BoxPtr      pBox;
    BoxRec      box;
    int         n, nInit;
    xRectangle *pRectInit;
    xRectangle *pRect;
    RegionRec   NullClip;
    RegionPtr   oldpCompositeClip;
    int         old_fillStyle;
    PixUnion    old_tile;


    nInit = REGION_NUM_RECTS(pRegion);
    pBox = REGION_RECTS(pRegion);

    pRectInit = (xRectangle *)ALLOCATE_LOCAL(nInit * sizeof(xRectangle));
    if (!pRectInit)
        return;

    pGC = GetScratchGC(pDrawable->depth, pScreen);
    if (!pGC)
    {
	DEALLOCATE_LOCAL(pRectInit);
        return;
    }

    box.x1 = 0;
    box.y1 = 0;
    box.x2 = mach64VirtX;
    box.y2 = mach64VirtY;
    (*pGC->pScreen->RegionInit)(&NullClip, &box, 1);

    devPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr);
    oldpCompositeClip = devPriv->pCompositeClip;
    old_fillStyle = pGC->fillStyle;
    old_tile = pGC->tile;

    devPriv->pCompositeClip = &NullClip;
    pGC->fillStyle = FillTiled;
    pGC->tile.pixmap = pPixmap;

    for(pRect = pRectInit, n = 0; n < nInit; n++, pRect++, pBox++)
    {
        pRect->width =  max(pBox->x1,pBox->x2) - min(pBox->x1,pBox->x2);
        pRect->height = max(pBox->y1,pBox->y2) - min(pBox->y1,pBox->y2);
        pRect->x = min(pBox->x1,pBox->x2) - pDrawable->x;
        pRect->y = min(pBox->y1,pBox->y2) - pDrawable->y;
    }

    mach64PolyFillRect (pDrawable, pGC, nInit, pRectInit);

    devPriv->pCompositeClip = oldpCompositeClip;
    pGC->fillStyle = old_fillStyle;
    pGC->tile = old_tile;

    (*pGC->pScreen->RegionUninit)(&NullClip);
    FreeScratchGC(pGC);
    DEALLOCATE_LOCAL(pRectInit);
}

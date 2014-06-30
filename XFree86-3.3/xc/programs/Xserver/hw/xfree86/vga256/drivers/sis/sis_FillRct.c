/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/sis/sis_FillRct.c,v 1.2 1997/01/12 10:52:26 dawes Exp $ */

/*
 * 
 * Copyright (c) 1989  X Consortium
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of the X Consortium shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from the X Consortium.
 */

/* Modified for Cirrus by Harm Hanemaayer, <hhanemaa@cs.ruu.nl> */
/* Modified for Chips by David Bateman, <dbateman@ee.uts.edu.au> */
/* Modified for Sis by Xavier Ducoin (xavier@rd.lectra.fr) */

#define TILE_ACCEL
#define STIPPLE_ACCEL

/*
 * This file contains the high level PolyFillRect function.
 *
 * We need to reproduce this to be able to use our own non-GXcopy
 * solid fills, and tiles.
 *
 * Works for 8bpp, and linear framebuffer 16bpp and 24bpp (compiled once).
 */

#include "vga256.h"
#include "cfbrrop.h"
#include "mergerop.h"
#include "xf86.h"
#include "vga.h"
#include "vgaBank.h"		       /* For CHECKSCREEN. */
#include "sis_driver.h"

#if PPW == 4
extern void cfb8FillRectStippledUnnatural();

#endif

extern void siscfbFillRectTile();

extern void cfb16FillRectSolidCopy();
extern void cfb16FillRectSolidGeneral();
extern void cfb16FillRectTileOdd();
extern void cfb16FillRectTile32Copy();
extern void cfb16FillRectTile32General();
extern void cfb24FillRectSolidCopy();
extern void cfb24FillRectSolidGeneral();
extern void cfb24FillRectTileOdd();
extern void cfb24FillRectTile32Copy();
extern void cfb24FillRectTile32General();

#define NUM_STACK_RECTS	1024

void
siscfbPolyFillRect(pDrawable, pGC, nrectFill, prectInit)
    DrawablePtr pDrawable;
    register GCPtr pGC;
    int nrectFill;		       /* number of rectangles to fill */
    xRectangle *prectInit;	       /* Pointer to first rectangle to fill */
{
    xRectangle *prect;
    RegionPtr prgnClip;
    register BoxPtr pbox;
    register BoxPtr pboxClipped;
    BoxPtr pboxClippedBase;
    BoxPtr pextent;
    BoxRec stackRects[NUM_STACK_RECTS];
    cfbPrivGC *priv;
    int numRects;
    void (*BoxFill) ();
    int n;
    int xorg, yorg;
    unsigned long *pdstBase;
    int widthDst;

    RROP_DECLARE;

#ifdef DEBUG
    ErrorF("SIS:siscfbPolyFillRect \n ");
#endif

    if (!xf86VTSema || pDrawable->type != DRAWABLE_WINDOW) {
#ifdef DEBUG
	ErrorF("cfbPolyFillRect\n");
#endif
	if (vgaBitsPerPixel == 8) {
	    cfbPolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	    return;
	}
	if (vgaBitsPerPixel == 16) {
	    cfb16PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	    return;
	}
	if (vgaBitsPerPixel == 24) {
	    cfb24PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	    return;
	}
	return;
    }
    priv = (cfbPrivGC *) pGC->devPrivates[cfbGCPrivateIndex].ptr;
    prgnClip = priv->pCompositeClip;

    BoxFill = 0;
    switch (pGC->fillStyle) {
    case FillSolid:
	    BoxFill = sisMMIOFillRectSolid;	/* Optimized */
	break;

    case FillTiled:
	switch (vgaBitsPerPixel) {
	case 8:
#ifdef TILE_ACCEL		       /* disable acelerated tile fill */
	    if (pGC->alu == GXcopy && (pGC->planemask & 0xFF) == 0xFF ) {
		BoxFill = siscfbFillRectTile;
	    } else
#endif
	    {
		if (!((cfbPrivGCPtr) pGC->devPrivates[cfbGCPrivateIndex].ptr)->
		    pRotatedPixmap) {
		    BoxFill = vga256FillRectTileOdd;
		} else {
		    if (pGC->alu == GXcopy && (pGC->planemask & 0xFF)
			== 0xFF) {
			BoxFill = vga256FillRectTile32Copy;
		    } else {
			BoxFill = vga256FillRectTile32General;
		    }
		}
	    }
	    break;
	case 16:
#ifdef TILE_ACCEL		       /* disable acelerated tile fill */
	    if (pGC->alu == GXcopy && (pGC->planemask & 0xFFFF) == 0xFFFF ) {
		BoxFill = siscfbFillRectTile;
	    } else
#endif
	    {
		if (!((cfbPrivGCPtr) pGC->devPrivates[cfbGCPrivateIndex].ptr)->
		    pRotatedPixmap) {
		    BoxFill = cfb16FillRectTileOdd;
		} else {
		    if (pGC->alu == GXcopy && (pGC->planemask & 0xFFFF)
			== 0xFFFF) {
			BoxFill = cfb16FillRectTile32Copy;
		    } else {
			BoxFill = cfb16FillRectTile32General;
		    }
		}
	    }
	    break;
	case 24:
	    if (!((cfbPrivGCPtr) pGC->devPrivates[cfbGCPrivateIndex].ptr)->
		pRotatedPixmap) {
		BoxFill = cfb24FillRectTileOdd;
	    } else {
		if (pGC->alu == GXcopy && (pGC->planemask & 0xFFFFFF)
		    == 0xFFFFFF) {
		    BoxFill = cfb24FillRectTile32Copy;
		} else {
		    BoxFill = cfb24FillRectTile32General;
		}
	    }
	    break;
	}
	break;
    case FillStippled:
#ifdef STIPPLE_ACCEL			       /* disable acelerated fill */
	if ((!((cfbPrivGCPtr) pGC->devPrivates[cfbGCPrivateIndex].ptr)->
		pRotatedPixmap) || sisAvoidImageBLT || 
	        sisBLTPatternAddress <= 0 )
	    BoxFill = vga2568FillRectStippledUnnatural;
	else {
	    BoxFill = siscfbFillRectTransparentStippled32;
	}
#else
	    BoxFill = vga2568FillRectStippledUnnatural;
#endif
	break;
    case FillOpaqueStippled:
#ifdef STIPPLE_ACCEL			       /* disable acelerated fill */
	if ((!((cfbPrivGCPtr) pGC->devPrivates[cfbGCPrivateIndex].ptr)->
	     pRotatedPixmap) || sisAvoidImageBLT || 
	        sisBLTPatternAddress <= 0 )
	    BoxFill = vga2568FillRectStippledUnnatural;
	else {
	    BoxFill = siscfbFillRectOpaqueStippled32;
	}
#else
	    BoxFill = vga2568FillRectStippledUnnatural;
#endif
	break;

    }
    prect = prectInit;
    xorg = pDrawable->x;
    yorg = pDrawable->y;
    if (xorg || yorg) {
	prect = prectInit;
	n = nrectFill;
	while (n--) {
	    prect->x += xorg;
	    prect->y += yorg;
	    prect++;
	}
    }
    prect = prectInit;

    numRects = REGION_NUM_RECTS(prgnClip) * nrectFill;
    if (numRects > NUM_STACK_RECTS) {
	pboxClippedBase = (BoxPtr) ALLOCATE_LOCAL(numRects * sizeof(BoxRec));
	if (!pboxClippedBase)
	    return;
    } else
	pboxClippedBase = stackRects;

    pboxClipped = pboxClippedBase;

    if (REGION_NUM_RECTS(prgnClip) == 1) {
	int x1, y1, x2, y2, bx2, by2;

	pextent = REGION_RECTS(prgnClip);
	x1 = pextent->x1;
	y1 = pextent->y1;
	x2 = pextent->x2;
	y2 = pextent->y2;
	while (nrectFill--) {
	    if ((pboxClipped->x1 = prect->x) < x1)
		pboxClipped->x1 = x1;

	    if ((pboxClipped->y1 = prect->y) < y1)
		pboxClipped->y1 = y1;

	    bx2 = (int)prect->x + (int)prect->width;
	    if (bx2 > x2)
		bx2 = x2;
	    pboxClipped->x2 = bx2;

	    by2 = (int)prect->y + (int)prect->height;
	    if (by2 > y2)
		by2 = y2;
	    pboxClipped->y2 = by2;

	    prect++;
	    if ((pboxClipped->x1 < pboxClipped->x2) &&
		(pboxClipped->y1 < pboxClipped->y2)) {
		pboxClipped++;
	    }
	}
    } else {
	int x1, y1, x2, y2, bx2, by2;

	pextent = (*pGC->pScreen->RegionExtents) (prgnClip);
	x1 = pextent->x1;
	y1 = pextent->y1;
	x2 = pextent->x2;
	y2 = pextent->y2;
	while (nrectFill--) {
	    BoxRec box;

	    if ((box.x1 = prect->x) < x1)
		box.x1 = x1;

	    if ((box.y1 = prect->y) < y1)
		box.y1 = y1;

	    bx2 = (int)prect->x + (int)prect->width;
	    if (bx2 > x2)
		bx2 = x2;
	    box.x2 = bx2;

	    by2 = (int)prect->y + (int)prect->height;
	    if (by2 > y2)
		by2 = y2;
	    box.y2 = by2;

	    prect++;

	    if ((box.x1 >= box.x2) || (box.y1 >= box.y2))
		continue;

	    n = REGION_NUM_RECTS(prgnClip);
	    pbox = REGION_RECTS(prgnClip);

	    /* clip the rectangle to each box in the clip region
	     * this is logically equivalent to calling Intersect()
	     */
	    while (n--) {
		pboxClipped->x1 = max(box.x1, pbox->x1);
		pboxClipped->y1 = max(box.y1, pbox->y1);
		pboxClipped->x2 = min(box.x2, pbox->x2);
		pboxClipped->y2 = min(box.y2, pbox->y2);
		pbox++;

		/* see if clipping left anything */
		if (pboxClipped->x1 < pboxClipped->x2 &&
		    pboxClipped->y1 < pboxClipped->y2) {
		    pboxClipped++;
		}
	    }
	}
    }
    if (pboxClipped != pboxClippedBase)
	(*BoxFill) (pDrawable, pGC,
	    pboxClipped - pboxClippedBase, pboxClippedBase);
    if (pboxClippedBase != stackRects)
	DEALLOCATE_LOCAL(pboxClippedBase);

}











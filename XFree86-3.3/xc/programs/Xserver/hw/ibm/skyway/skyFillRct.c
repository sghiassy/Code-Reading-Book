/*
 * $XConsortium: skyFillRct.c,v 1.5 94/04/17 20:29:21 dpw Exp $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991 
 *
 * All Rights Reserved 
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of IBM not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS, IN NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 */

/*

Copyright (c) 1989  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.
*/

/*
 * Fill rectangles. Based on cfbfillrct.c.
 */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "misc.h"

#include "cfb.h"
#include "cfbmskbits.h"
#include "mergerop.h"

/* ------ */
#include "ibmTrace.h"
#include "skyHdwr.h"
#include "skyReg.h"
#include "skyPriv.h"
/* ------ */


#if PSZ == 8
extern void cfb8FillRectOpaqueStippled32();
extern void cfb8FillRectTransparentStippled32();
extern void cfb8FillRectStippledUnnatural();
#endif

extern void cfbFillRectSolidCopy(), cfbFillRectSolidXor (), cfbFillRectSolidGeneral ();

extern void cfbFillRectTile32Copy (), cfbFillRectTile32General ();

extern void cfbFillBoxTileOddCopy ();
extern void cfbFillBoxTileOddGeneral ();
extern void cfbFillBoxTile32sCopy ();
extern void cfbFillBoxTile32sGeneral ();

extern void cfbFillBoxTileOdd ();
extern void cfbFillRectTileOdd ();


/* ------ */
extern void skyFillRectSolid();
extern void skyFillRectTile();
extern void skyFillRectStipple();
/* ------ */



#define NUM_STACK_RECTS	1024

void
skyPolyFillRect(pDrawable, pGC, nrectFill, prectInit)
    DrawablePtr pDrawable;
    register GCPtr pGC;
    int		nrectFill; 	/* number of rectangles to fill */
    xRectangle	*prectInit;  	/* Pointer to first rectangle to fill */
{
    xRectangle	    *prect;
    RegionPtr	    prgnClip;
    register BoxPtr pbox;
    register BoxPtr pboxClipped;
    BoxPtr	    pboxClippedBase;
    BoxPtr	    pextent;
    BoxRec	    stackRects[NUM_STACK_RECTS];
    cfbPrivGC	    *priv;
    /* ------ */
    skyPrivGCPtr    pSkyPriv = skyGetGCPrivate(pGC);
    /* ------ */
    int		    numRects;
    void	    (*BoxFill)();
    int		    n;
    int		    xorg, yorg;

    priv = (cfbPrivGC *) pGC->devPrivates[cfbGCPrivateIndex].ptr;
    prgnClip = priv->pCompositeClip;

    BoxFill = 0;

    /* ------ */
    if (pSkyPriv->fillMode != punt)
    {
	switch (pSkyPriv->fillMode)
	{
	    case fill_solid:
		BoxFill = skyFillRectSolid;
		break;

	    case fill_tile:
		BoxFill = skyFillRectTile;
		break;

	    case fill_stipple:
		BoxFill = skyFillRectStipple;
		break;

	    default:
		FatalError("skyPolyFillRect: fillMode not supported");
	}
    }
    else
    /* ------ */

    switch (pGC->fillStyle)
    {
    case FillSolid:
	switch (priv->rop) {
	case GXcopy:
	    BoxFill = cfbFillRectSolidCopy;
	    break;
	case GXxor:
	    BoxFill = cfbFillRectSolidXor;
	    break;
	default:
	    BoxFill = cfbFillRectSolidGeneral;
	    break;
	}
	break;
    case FillTiled:
	if (!((cfbPrivGCPtr) pGC->devPrivates[cfbGCPrivateIndex].ptr)->
							pRotatedPixmap)
	    BoxFill = cfbFillRectTileOdd;
	else
	{
	    if (pGC->alu == GXcopy && (pGC->planemask & PMSK) == PMSK)
		BoxFill = cfbFillRectTile32Copy;
	    else
		BoxFill = cfbFillRectTile32General;
	}
	break;
#if (PSZ == 8)
    case FillStippled:
	if (!((cfbPrivGCPtr) pGC->devPrivates[cfbGCPrivateIndex].ptr)->
							pRotatedPixmap)
	    BoxFill = cfb8FillRectStippledUnnatural;
	else
	    BoxFill = cfb8FillRectTransparentStippled32;
	break;
    case FillOpaqueStippled:
	if (!((cfbPrivGCPtr) pGC->devPrivates[cfbGCPrivateIndex].ptr)->
							pRotatedPixmap)
	    BoxFill = cfb8FillRectStippledUnnatural;
	else
	    BoxFill = cfb8FillRectOpaqueStippled32;
	break;
#endif
    }
    prect = prectInit;
    xorg = pDrawable->x;
    yorg = pDrawable->y;
    if (xorg || yorg)
    {
	prect = prectInit;
	n = nrectFill;
	while(n--)
	{
	    prect->x += xorg;
	    prect->y += yorg;
	    prect++;
	}
    }

    prect = prectInit;

    numRects = REGION_NUM_RECTS(prgnClip) * nrectFill;
    if (numRects > NUM_STACK_RECTS)
    {
	pboxClippedBase = (BoxPtr)ALLOCATE_LOCAL(numRects * sizeof(BoxRec));
	if (!pboxClippedBase)
	    return;
    }
    else
	pboxClippedBase = stackRects;

    pboxClipped = pboxClippedBase;
	
    if (REGION_NUM_RECTS(prgnClip) == 1)
    {
	int x1, y1, x2, y2, bx2, by2;

	pextent = REGION_RECTS(prgnClip);
	x1 = pextent->x1;
	y1 = pextent->y1;
	x2 = pextent->x2;
	y2 = pextent->y2;
    	while (nrectFill--)
    	{
	    if ((pboxClipped->x1 = prect->x) < x1)
		pboxClipped->x1 = x1;
    
	    if ((pboxClipped->y1 = prect->y) < y1)
		pboxClipped->y1 = y1;
    
	    bx2 = (int) prect->x + (int) prect->width;
	    if (bx2 > x2)
		bx2 = x2;
	    pboxClipped->x2 = bx2;
    
	    by2 = (int) prect->y + (int) prect->height;
	    if (by2 > y2)
		by2 = y2;
	    pboxClipped->y2 = by2;

	    prect++;
	    if ((pboxClipped->x1 < pboxClipped->x2) &&
		(pboxClipped->y1 < pboxClipped->y2))
	    {
		pboxClipped++;
	    }
    	}
    }
    else
    {
	int x1, y1, x2, y2, bx2, by2;

	pextent = REGION_EXTENTS(pGC->pScreen, prgnClip);
	x1 = pextent->x1;
	y1 = pextent->y1;
	x2 = pextent->x2;
	y2 = pextent->y2;
    	while (nrectFill--)
    	{
	    BoxRec box;
    
	    if ((box.x1 = prect->x) < x1)
		box.x1 = x1;
    
	    if ((box.y1 = prect->y) < y1)
		box.y1 = y1;
    
	    bx2 = (int) prect->x + (int) prect->width;
	    if (bx2 > x2)
		bx2 = x2;
	    box.x2 = bx2;
    
	    by2 = (int) prect->y + (int) prect->height;
	    if (by2 > y2)
		by2 = y2;
	    box.y2 = by2;
    
	    prect++;
    
	    if ((box.x1 >= box.x2) || (box.y1 >= box.y2))
	    	continue;
    
	    n = REGION_NUM_RECTS (prgnClip);
	    pbox = REGION_RECTS(prgnClip);
    
	    /* clip the rectangle to each box in the clip region
	       this is logically equivalent to calling Intersect()
	    */
	    while(n--)
	    {
		pboxClipped->x1 = max(box.x1, pbox->x1);
		pboxClipped->y1 = max(box.y1, pbox->y1);
		pboxClipped->x2 = min(box.x2, pbox->x2);
		pboxClipped->y2 = min(box.y2, pbox->y2);
		pbox++;

		/* see if clipping left anything */
		if(pboxClipped->x1 < pboxClipped->x2 && 
		   pboxClipped->y1 < pboxClipped->y2)
		{
		    pboxClipped++;
		}
	    }
    	}
    }
    if (pboxClipped != pboxClippedBase)
	(*BoxFill) (pDrawable, pGC,
		    pboxClipped-pboxClippedBase, pboxClippedBase);
    if (pboxClippedBase != stackRects)
    	DEALLOCATE_LOCAL(pboxClippedBase);
}


/* Note: does not handle pGC->tileIsPixel, skyFillRectSolid does */

void
skyFillRectTile(pDrawable, pGC, nBox, pBox)
    DrawablePtr	    pDrawable;
    GCPtr	    pGC;
    int		    nBox;
    BoxPtr	    pBox;
{
    int             scrnNum = pDrawable->pScreen->myNum;
    ulong           PixelOp;
    int             xrot, yrot;
    int             pat_w, pat_h;

    TRACE(("skyFillRectTile(pD=0x%x, pGC=0x%x, nBox=%d, pBox=0x%x)\n",
	pDrawable, pGC, nBox, pBox));

    PixelOp = skySetTile(scrnNum, pGC, &pat_w, &pat_h);
    PixelOp |= POStepBlt | POModeAll | POOct0;

    TRACE(("\tPixelOp = 0x%x\n", PixelOp));

    xrot = pDrawable->x + pGC->patOrg.x;
    yrot = pDrawable->y + pGC->patOrg.y;

    for (; nBox; nBox--, pBox++)
    {
	int w = pBox->x2 - pBox->x1;
    	int h = pBox->y2 - pBox->y1;
	
	SKYTilePixelOp(scrnNum,
	    PixelOp,			/* Pixel Operation */
	    pBox->x1, pBox->y1,		/* Destination x,y */
	    w, h,			/* Width, Height   */
	    xrot, yrot,			/* TileStip origin */
	    pat_w, pat_h);		/* TileStip size   */
    }

    SKYBusyWait(scrnNum);
}


void
skyFillRectStipple(pDrawable, pGC, nBox, pBox)
    DrawablePtr	    pDrawable;
    GCPtr	    pGC;
    int		    nBox;
    BoxPtr	    pBox;
{
    int             scrnNum = pDrawable->pScreen->myNum;
    ulong           PixelOp;
    int             xrot, yrot;
    int             pat_w, pat_h;

    TRACE(("skyFillRectStipple(pD=0x%x, pGC=0x%x, nBox=%d, pBox=0x%x)\n",
	pDrawable, pGC, nBox, pBox));

    PixelOp = skySetStipple(scrnNum, pGC, &pat_w, &pat_h);
    PixelOp |= POStepBlt | POModeAll | POOct0;

    TRACE(("\tPixelOp = 0x%x\n", PixelOp));

    xrot = pDrawable->x + pGC->patOrg.x;
    yrot = pDrawable->y + pGC->patOrg.y;

    for (; nBox; nBox--, pBox++)
    {
	int w = pBox->x2 - pBox->x1;
    	int h = pBox->y2 - pBox->y1;
	
	SKYStipplePixelOp(scrnNum,
	    PixelOp,			/* Pixel Operation */
	    pBox->x1, pBox->y1,		/* Destination x,y */
	    w, h,			/* Width, Height   */
	    xrot, yrot,			/* TileStip origin */
	    pat_w, pat_h);		/* TileStip size   */
    }

    SKYBusyWait(scrnNum);
}

void
skyFillRectSolid(pDrawable, pGC, nBox, pBox)
    DrawablePtr	    pDrawable;
    GCPtr	    pGC;
    int		    nBox;
    BoxPtr	    pBox;
{
    int   scrnNum = pDrawable->pScreen->myNum;
    ulong PixelOp;

    TRACE(("skyFillRectSolid(pD=0x%x, pGC=0x%x, nBox=%d, pBox=0x%x)\n",
	pDrawable, pGC, nBox, pBox));


    PixelOp = skySetSolid(scrnNum, pGC);
    PixelOp |= POStepBlt | POModeAll | POOct0;

    TRACE(("\tPixelOp = 0x%x\n", PixelOp));

    for (; nBox; nBox--, pBox++)
    {
	int w = pBox->x2 - pBox->x1;
    	int h = pBox->y2 - pBox->y1;
	
	SKYSolidPixelOp(scrnNum,
	    PixelOp,			/* Pixel Operation */
	    pBox->x1, pBox->y1,		/* Destination x,y */
	    w, h);			/* Width, Height   */
    }

    SKYBusyWait(scrnNum);
}


/* This code was used to test performance when not software clipping. */

#if defined(SCISSORING_TEST) || defined(NOCLIP_TEST)
void
skyPolyFillRectSolid_1Rect(pDrawable, pGC, nrect, prect)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int		nrect;		/* number of rectangles to fill */
    xRectangle	*prect;		/* Pointer to first rectangle to fill */
{
    int             scrnNum = pDrawable->pScreen->myNum;
    ulong           PixelOp;
    int		    xorg, yorg;

    TRACE(("skyPolyFillRectSolid_1Rect(pD=0x%x,pGC=0x%x,nR=%d,pR=0x%x)\n",
		pDrawable, pGC, nrect, prect));

    PixelOp = skySetSolid(scrnNum, pGC);
    PixelOp |= POStepBlt | POModeAll | POOct0;

#ifdef SCISSORING_TEST
    /* Quick hack to test performance of boundary-enable clipping */

    {
	static DrawablePtr lastDrawable = 0;

	PixelOp |= POMaskBEn;

	/* Checking the last drawable is just a hack for this test.
	 * Won't work for real version. Need to use some kind of clip id.
	 */

        if (lastDrawable != pDrawable)
	{
	    cfbPrivGCPtr devPriv =
		((cfbPrivGCPtr) (pGC->devPrivates[cfbGCPrivateIndex].ptr));

	    BoxPtr pClipBox=REGION_RECTS(devPriv->pCompositeClip);

	    lastDrawable = pDrawable;

	    TRACE(("x1,y1=%d,%d\n", pClipBox->x1, pClipBox->y1));
	    TRACE(("w,h  =%d,%d\n", pClipBox->x2 - pClipBox->x1,
			pClipBox->y2 - pClipBox->y1));

	    /* Set boundary rectangle */

	    SKYSetMaskXY(scrnNum, pClipBox->x1, pClipBox->y1);
	    SKYSetPixmap(scrnNum, MaskMap, SKY_INVIS_VRAM_BASE(scrnNum),
		pClipBox->x2 - pClipBox->x1,
		pClipBox->y2 - pClipBox->y1,
		PixSize1);
	}
    }
#endif /* SCISSORING_TEST */

    TRACE(("\tPixelOp = 0x%x\n", PixelOp));

    xorg = pDrawable->x;
    yorg = pDrawable->y;

    for (; nrect; nrect--, prect++)
    {
	short x = xorg + prect->x;
	short y = yorg + prect->y;

	SKYSolidPixelOp(scrnNum,
	    PixelOp,				/* Pixel Operation */
	    x, y,				/* Destination x,y */
	    prect->width, prect->height);	/* Width, Height   */
    }

    SKYBusyWait(scrnNum);
}
#endif

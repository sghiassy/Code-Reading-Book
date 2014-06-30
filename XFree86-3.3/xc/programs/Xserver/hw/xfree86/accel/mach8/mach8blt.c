/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach8/mach8blt.c,v 3.6 1996/12/23 06:39:44 dawes Exp $ */
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

Author: Keith Packard

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN, RICKARD E. FAITH, AND TIAGO GONS DISCLAIM
ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
KEVIN E. MARTIN, RICKARD E. FAITH, OR TIAGO GONS BE LIABLE
FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Further modifications by Tiago Gons (tiago@comosjn.hobby.nl)
*/
/* $XConsortium: mach8blt.c /main/8 1996/02/21 17:29:45 kaleb $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"mi.h"
#include	"cfb.h"
#include	"cfbmskbits.h"
#include	"cfb8bit.h"
#include	"fastblt.h"

#include	"regmach8.h"
#include	"mach8.h"

extern RegionPtr cfbBitBlt();

RegionPtr
mach8CopyArea(pSrcDrawable, pDstDrawable,
		pGC, srcx, srcy, width, height, dstx, dsty)
    DrawablePtr pSrcDrawable;
    DrawablePtr pDstDrawable;
    GC *pGC;
    int srcx, srcy;
    int width, height;
    int dstx, dsty;
{
    RegionPtr prgnSrcClip;	/* may be a new region, or just a copy */
    Bool freeSrcClip = FALSE;

    RegionPtr prgnExposed;
    RegionRec rgnDst;
    register BoxPtr pbox;
    int i;
    register int dx;
    register int dy;
    xRectangle origSource;
    DDXPointRec origDest;
    int numRects;
    BoxRec fastBox;
    int fastClip = 0;		/* for fast clipping with pixmap source */
    int fastExpose = 0;		/* for fast exposures with pixmap source */

/* 11-jun-93 TCG : is VT visible */
    if (!xf86VTSema ||
	((pSrcDrawable->type != DRAWABLE_WINDOW) &&
	 (pDstDrawable->type != DRAWABLE_WINDOW)))
    {
        return cfbCopyArea(pSrcDrawable, pDstDrawable, pGC, 
			   srcx, srcy, width, height, dstx, dsty);
    }

    origSource.x = srcx;
    origSource.y = srcy;
    origSource.width = width;
    origSource.height = height;
    origDest.x = dstx;
    origDest.y = dsty;

    if ((pSrcDrawable != pDstDrawable) &&
	pSrcDrawable->pScreen->SourceValidate)
    {
	(*pSrcDrawable->pScreen->SourceValidate) (pSrcDrawable, srcx, srcy, width, height);
    }

    srcx += pSrcDrawable->x;
    srcy += pSrcDrawable->y;

    /* clip the source */

    if (pSrcDrawable->type == DRAWABLE_PIXMAP)
    {
	if ((pSrcDrawable == pDstDrawable) &&
	    (pGC->clientClipType == CT_NONE))
	{
	    prgnSrcClip = ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
	}
	else
	{
	    fastClip = 1;
	}
    }
    else
    {
	if (pGC->subWindowMode == IncludeInferiors)
	{
	    if (!((WindowPtr) pSrcDrawable)->parent)
	    {
		/*
		 * special case bitblt from root window in
		 * IncludeInferiors mode; just like from a pixmap
		 */
		fastClip = 1;
	    }
	    else if ((pSrcDrawable == pDstDrawable) &&
		(pGC->clientClipType == CT_NONE))
	    {
		prgnSrcClip = ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
	    }
	    else
	    {
		prgnSrcClip = NotClippedByChildren((WindowPtr)pSrcDrawable);
		freeSrcClip = TRUE;
	    }
	}
	else
	{
	    prgnSrcClip = &((WindowPtr)pSrcDrawable)->clipList;
	}
    }

    fastBox.x1 = srcx;
    fastBox.y1 = srcy;
    fastBox.x2 = srcx + width;
    fastBox.y2 = srcy + height;

    /* Don't create a source region if we are doing a fast clip */
    if (fastClip)
    {
	fastExpose = 1;
	/*
	 * clip the source; if regions extend beyond the source size,
 	 * make sure exposure events get sent
	 */
	if (fastBox.x1 < pSrcDrawable->x)
	{
	    fastBox.x1 = pSrcDrawable->x;
	    fastExpose = 0;
	}
	if (fastBox.y1 < pSrcDrawable->y)
	{
	    fastBox.y1 = pSrcDrawable->y;
	    fastExpose = 0;
	}
	if (fastBox.x2 > pSrcDrawable->x + (int) pSrcDrawable->width)
	{
	    fastBox.x2 = pSrcDrawable->x + (int) pSrcDrawable->width;
	    fastExpose = 0;
	}
	if (fastBox.y2 > pSrcDrawable->y + (int) pSrcDrawable->height)
	{
	    fastBox.y2 = pSrcDrawable->y + (int) pSrcDrawable->height;
	    fastExpose = 0;
	}
    }
    else
    {
	(*pGC->pScreen->RegionInit)(&rgnDst, &fastBox, 1);
	(*pGC->pScreen->Intersect)(&rgnDst, &rgnDst, prgnSrcClip);
    }

    dstx += pDstDrawable->x;
    dsty += pDstDrawable->y;

    if (pDstDrawable->type == DRAWABLE_WINDOW)
    {
	if (!((WindowPtr)pDstDrawable)->realized)
	{
	    if (!fastClip)
		(*pGC->pScreen->RegionUninit)(&rgnDst);
	    if (freeSrcClip)
		(*pGC->pScreen->RegionDestroy)(prgnSrcClip);
	    return NULL;
	}
    }

    dx = srcx - dstx;
    dy = srcy - dsty;

    /* Translate and clip the dst to the destination composite clip */
    if (fastClip)
    {
	RegionPtr cclip;

        /* Translate the region directly */
        fastBox.x1 -= dx;
        fastBox.x2 -= dx;
        fastBox.y1 -= dy;
        fastBox.y2 -= dy;

	/* If the destination composite clip is one rectangle we can
	   do the clip directly.  Otherwise we have to create a full
	   blown region and call intersect */

	/* XXX because CopyPlane uses this routine for 8-to-1 bit
	 * copies, this next line *must* also correctly fetch the
	 * composite clip from an mfb gc
	 */

	cclip = ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
        if (REGION_NUM_RECTS(cclip) == 1)
        {
	    BoxPtr pBox = REGION_RECTS(cclip);

	    if (fastBox.x1 < pBox->x1) fastBox.x1 = pBox->x1;
	    if (fastBox.x2 > pBox->x2) fastBox.x2 = pBox->x2;
	    if (fastBox.y1 < pBox->y1) fastBox.y1 = pBox->y1;
	    if (fastBox.y2 > pBox->y2) fastBox.y2 = pBox->y2;

	    /* Check to see if the region is empty */
	    if (fastBox.x1 >= fastBox.x2 || fastBox.y1 >= fastBox.y2)
		(*pGC->pScreen->RegionInit)(&rgnDst, NullBox, 0);
	    else
		(*pGC->pScreen->RegionInit)(&rgnDst, &fastBox, 1);
	}
        else
	{
	    /* We must turn off fastClip now, since we must create
	       a full blown region.  It is intersected with the
	       composite clip below. */
	    fastClip = 0;
	    (*pGC->pScreen->RegionInit)(&rgnDst, &fastBox,1);
	}
    }
    else
    {
        (*pGC->pScreen->TranslateRegion)(&rgnDst, -dx, -dy);
    }

    if (!fastClip)
    {
	(*pGC->pScreen->Intersect)(&rgnDst,
				   &rgnDst,
				 ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
    }

    /* Do bit blitting */
    numRects = REGION_NUM_RECTS(&rgnDst);
    if (numRects && width && height)
    {
	pbox = REGION_RECTS(&rgnDst);

	if (pSrcDrawable->type == DRAWABLE_WINDOW && pDstDrawable->type == DRAWABLE_WINDOW) {
	    /* Window --> Window */
	    unsigned int *ordering;
	    BoxPtr prect;
	    short direction = 0;
        
	    ordering = (unsigned int *) ALLOCATE_LOCAL(numRects * sizeof(unsigned int));
	    if(!ordering) {
		DEALLOCATE_LOCAL(ordering);
		return (RegionPtr)NULL;
	    }

	    mach8FindOrdering(pSrcDrawable, pDstDrawable, pGC, numRects, pbox, srcx, srcy, dstx, dsty, ordering);

	    if (dx > 0) direction |= INC_X;
	    if (dy > 0) direction |= INC_Y;

	    WaitQueue(3);
	    outw(FRGD_MIX, FSS_BITBLT | mach8alu[pGC->alu]);
	    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
	    outw(WRT_MASK, pGC->planemask);

	    if (direction == (INC_X | INC_Y)) {
		for (i = 0; i < numRects; i++) {
		    prect = &pbox[ordering[i]];

		    WaitQueue(7);
		    outw(CUR_X, (short)(prect->x1 + dx));
		    outw(CUR_Y, (short)(prect->y1 + dy));
		    outw(DESTX_DIASTP, (short)(prect->x1));
		    outw(DESTY_AXSTP, (short)(prect->y1));
		    outw(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
		    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
		    outw(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
		}
	    } else if (direction == INC_X) {
		for (i = 0; i < numRects; i++) {
		    prect = &pbox[ordering[i]];

		    WaitQueue(7);
		    outw(CUR_X, (short)(prect->x1 + dx));
		    outw(CUR_Y, (short)(prect->y2 + dy - 1));
		    outw(DESTX_DIASTP, (short)(prect->x1));
		    outw(DESTY_AXSTP, (short)(prect->y2 - 1));
		    outw(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
		    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
		    outw(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
		}
	    } else if (direction == INC_Y) {
		for (i = 0; i < numRects; i++) {
		    prect = &pbox[ordering[i]];

		    WaitQueue(7);
		    outw(CUR_X, (short)(prect->x2 + dx - 1));
		    outw(CUR_Y, (short)(prect->y1 + dy));
		    outw(DESTX_DIASTP, (short)(prect->x2 - 1));
		    outw(DESTY_AXSTP, (short)(prect->y1));
		    outw(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
		    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
		    outw(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
		}
	    } else {
		for (i = 0; i < numRects; i++) {
		    prect = &pbox[ordering[i]];

		    WaitQueue(7);
		    outw(CUR_X, (short)(prect->x2 + dx - 1));
		    outw(CUR_Y, (short)(prect->y2 + dy - 1));
		    outw(DESTX_DIASTP, (short)(prect->x2 - 1));
		    outw(DESTY_AXSTP, (short)(prect->y2 - 1));
		    outw(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
		    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
		    outw(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
		}
	    }
	    DEALLOCATE_LOCAL(ordering);

	    WaitQueue(3);
	    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
	    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
	    outw(WRT_MASK, 0xffff);
	} else if (pSrcDrawable->type == DRAWABLE_WINDOW && pDstDrawable->type != DRAWABLE_WINDOW) {
	    /* Window --> Pixmap */
	    int pixWidth = PixmapBytePad(pDstDrawable->width, pDstDrawable->depth);
	    char *pdst = ((PixmapPtr)pDstDrawable)->devPrivate.ptr;

	    for (i = numRects; --i >= 0; pbox++)
		(mach8ImageReadFunc)(pbox->x1 + dx, pbox->y1 + dy,
				     pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				     pdst, pixWidth, pbox->x1, pbox->y1, 0xff);
	} else if (pSrcDrawable->type != DRAWABLE_WINDOW && pDstDrawable->type == DRAWABLE_WINDOW) {
	    /* Pixmap --> Window */
	    int pixWidth = PixmapBytePad(pSrcDrawable->width, pSrcDrawable->depth);
	    char *psrc = ((PixmapPtr)pSrcDrawable)->devPrivate.ptr;

	    for (i = numRects; --i >= 0; pbox++)
		(mach8ImageWriteFunc)(pbox->x1, pbox->y1,
				      pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				      psrc, pixWidth,
				      pbox->x1 + dx, pbox->y1 + dy,
				      mach8alu[pGC->alu], pGC->planemask);
	} else {
	    /* Pixmap --> Pixmap */
	    ErrorF("mach8CopyArea:  Tried to do a Pixmap to Pixmap copy\n");
	}

/*
	(*localDoBitBlt) (pSrcDrawable, pDstDrawable, pGC->alu, &rgnDst, pptSrc, pGC->planemask);
*/
    }

    prgnExposed = NULL;
    if (((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->fExpose)
    {
	extern RegionPtr    miHandleExposures();

        /* Pixmap sources generate a NoExposed (we return NULL to do this) */
        if (!fastExpose)
	    prgnExposed =
		miHandleExposures(pSrcDrawable, pDstDrawable, pGC,
				  origSource.x, origSource.y,
				  (int)origSource.width,
				  (int)origSource.height,
				  origDest.x, origDest.y, 0);
    }
    (*pGC->pScreen->RegionUninit)(&rgnDst);
    if (freeSrcClip)
	(*pGC->pScreen->RegionDestroy)(prgnSrcClip);
    return prgnExposed;
}

void
mach8FindOrdering(pSrcDrawable, pDstDrawable, pGC, numRects, boxes, srcx, srcy, dstx, dsty, ordering)
    DrawablePtr pSrcDrawable;
    DrawablePtr pDstDrawable;
    GC *pGC;
    int numRects;
    BoxPtr boxes;
    int srcx;
    int srcy;
    int dstx;
    int dsty;
    unsigned int *ordering;
{
    int i, j, y;
    int xMax, yMin, yMax;

    /* If not the same drawable then order of move doesn't matter.
       Following assumes that boxes are sorted from top
       to bottom and left to right.
    */
    if ((pSrcDrawable != pDstDrawable) &&
        ((pGC->subWindowMode != IncludeInferiors) ||
         (pSrcDrawable->type == DRAWABLE_PIXMAP) ||
         (pDstDrawable->type == DRAWABLE_PIXMAP))) {
        for (i=0; i < numRects; i++)
	    ordering[i] = i;
    } else { /* within same drawable, must sequence moves carefully! */
	if (dsty <= srcy) { /* Scroll up or stationary vertical.  Vertical order OK */
	    if (dstx <= srcx) { /* Scroll left or stationary horizontal.  Horizontal order OK as well */
		for (i=0; i < numRects; i++)
		    ordering[i] = i;
	    } else { /* scroll right. must reverse horizontal banding of rects. */
		for (i=0, j=1, xMax=0; i < numRects; j=i+1, xMax=i) {
		    /* find extent of current horizontal band */
		    y=boxes[i].y1; /* band has this y coordinate */
		    while ((j < numRects) && (boxes[j].y1 == y))
			j++;
		    /* reverse the horizontal band in the output ordering */
		    for (j-- ; j >= xMax; j--, i++)
			ordering[i] = j;
		}
	    }
	} else { /* Scroll down. Must reverse vertical banding. */
	    if (dstx < srcx) { /* Scroll left. Horizontal order OK. */
		for (i=numRects-1, j=i-1, yMin=i, yMax=0; i >= 0; j=i-1, yMin=i) {
		    /* find extent of current horizontal band */
		    y=boxes[i].y1; /* band has this y coordinate */
		    while ((j >= 0) && (boxes[j].y1 == y))
			j--;
		    /* reverse the horizontal band in the output ordering */
		    for (j++ ; j <= yMin; j++, i--, yMax++)
			ordering[yMax] = j;
		}
	    } else { /* Scroll right or horizontal stationary.
		        Reverse horizontal order as well (if stationary, horizontal
		        order can be swapped without penalty and this is faster
		        to compute). */
		for (i=0, j=numRects-1; i < numRects; i++, j--)
		    ordering[i] = j;
	    }
	}
    }
}

RegionPtr
mach8CopyPlane(pSrcDrawable, pDstDrawable,
	       pGC, srcx, srcy, width, height, dstx, dsty, bitPlane)
    DrawablePtr		pSrcDrawable;
    DrawablePtr		pDstDrawable;
    GCPtr		pGC;
    int			srcx, srcy;
    int			width, height;
    int			dstx, dsty;
    unsigned long	bitPlane;
{
   PixmapPtr pBitmap = NULL;
   RegionPtr prgnSrcClip;       /* may be a new region, or just a copy */
   Bool  freeSrcClip = FALSE;

   RegionPtr prgnExposed;
   RegionRec rgnDst;
   register BoxPtr pbox;
   int   i;
   register int dx;
   register int dy;
   xRectangle origSource;
   DDXPointRec origDest;
   int   numRects;
   BoxRec fastBox;
   int   fastClip = 0;          /* for fast clipping with pixmap source */
   int   fastExpose = 0;        /* for fast exposures with pixmap source */

    if (!xf86VTSema || 
      ((pSrcDrawable->type != DRAWABLE_WINDOW) &&
       (pDstDrawable->type != DRAWABLE_WINDOW)))
    {
	return cfbCopyPlane(pSrcDrawable, pDstDrawable, pGC,
			    srcx, srcy, width, height, dstx, dsty, bitPlane);
    }

    if ((pSrcDrawable->type != DRAWABLE_WINDOW) &&
	(pDstDrawable->type == DRAWABLE_WINDOW) &&
	(pSrcDrawable->bitsPerPixel == 8)) {
	/*
	 * Shortcut - we can do Pixmap->Window when the source depth is
	 * 8 by using the handy-dandy cfbCopyPlane8to1 to create a bitmap
	 * for us to use.
	 */
	GCPtr pGC1;

	pBitmap=(*pSrcDrawable->pScreen->CreatePixmap)(pSrcDrawable->pScreen, 
						       pSrcDrawable->width,
						       pSrcDrawable->height, 1);
	if (!pBitmap)
	    return(NULL);
	pGC1 = GetScratchGC(1, pSrcDrawable->pScreen);
	if (!pGC1) {
	    (*pSrcDrawable->pScreen->DestroyPixmap)(pBitmap);
	    return(NULL);
	}
	ValidateGC((DrawablePtr)pBitmap, pGC1);
	(void) cfbBitBlt(pSrcDrawable, (DrawablePtr)pBitmap, pGC1, srcx, srcy,
			 width, height, srcx, srcy, cfbCopyPlane8to1, bitPlane);
        FreeScratchGC(pGC1);
	pSrcDrawable = (DrawablePtr)pBitmap;
    } else if ((pSrcDrawable->type == DRAWABLE_WINDOW) &&
 	       (pDstDrawable->type != DRAWABLE_WINDOW)) {
	/*
 	 * Shortcut - we can do Window->Pixmap by copying the window to
 	 * a pixmap, then we have a Pixmap->Pixmap operation
 	 */
 	GCPtr pGC1;
 	RegionPtr retval;
 	PixmapPtr pPixmap;

	pPixmap=(*pSrcDrawable->pScreen->CreatePixmap)(pSrcDrawable->pScreen, 
 						       width, height, 8);
 	if (!pPixmap)
 	    return(NULL);
 	pGC1 = GetScratchGC(8, pSrcDrawable->pScreen);
 	if (!pGC1) {
 	    (*pSrcDrawable->pScreen->DestroyPixmap)(pPixmap);
 	    return(NULL);
 	}
 	ValidateGC((DrawablePtr)pPixmap, pGC1);
 	mach8CopyArea(pSrcDrawable, (DrawablePtr)pPixmap, pGC1, srcx, srcy,
		      width, height, 0, 0);
 	retval = cfbCopyPlane((DrawablePtr)pPixmap, pDstDrawable, pGC,
                              0, 0, width, height, dstx, dsty, bitPlane);
        FreeScratchGC(pGC1);
 	(*pSrcDrawable->pScreen->DestroyPixmap)(pPixmap);
 	return(retval);
    } else if (((pSrcDrawable->type == DRAWABLE_WINDOW) && 
              (pDstDrawable->type != DRAWABLE_WINDOW)) ||
             ((pSrcDrawable->type != DRAWABLE_WINDOW) &&
              (pDstDrawable->type == DRAWABLE_WINDOW) &&
              (pSrcDrawable->bitsPerPixel != 1))) {
        /*
         * Cases we can't handle, and must do another way:
         *        - copy Window->Pixmap
         *        - copy Pixmap->Window, Pixmap depth != 1
         *
         * Simplest thing to do is to let miCopyPlane deal with it for us.
         */
        return miCopyPlane(pSrcDrawable, pDstDrawable, pGC, srcx, srcy, 
                           width, height, dstx, dsty, bitPlane);
    }

   origSource.x = srcx;
   origSource.y = srcy;
   origSource.width = width;
   origSource.height = height;
   origDest.x = dstx;
   origDest.y = dsty;

   if ((pSrcDrawable != pDstDrawable) &&
       pSrcDrawable->pScreen->SourceValidate) {
      (*pSrcDrawable->pScreen->SourceValidate)(pSrcDrawable, srcx, srcy, 
                                               width, height);
   }
   srcx += pSrcDrawable->x;
   srcy += pSrcDrawable->y;

   /* clip the source */
   if (pSrcDrawable->type == DRAWABLE_PIXMAP) {
      if ((pSrcDrawable == pDstDrawable) &&
          (pGC->clientClipType == CT_NONE)) {
         prgnSrcClip = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
      } else {
         fastClip = 1;
      }
   } else {
      if (pGC->subWindowMode == IncludeInferiors) {
         if (!((WindowPtr) pSrcDrawable)->parent) {

            /*
             * special case bitblt from root window in IncludeInferiors mode;
             * just like from a pixmap
             */
            fastClip = 1;
         } else if ((pSrcDrawable == pDstDrawable) &&
                    (pGC->clientClipType == CT_NONE)) {
            prgnSrcClip = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
         } else {
            prgnSrcClip = NotClippedByChildren((WindowPtr) pSrcDrawable);
            freeSrcClip = TRUE;
         }
      } else {
         prgnSrcClip = &((WindowPtr) pSrcDrawable)->clipList;
      }
   }

   fastBox.x1 = srcx;
   fastBox.y1 = srcy;
   fastBox.x2 = srcx + width;
   fastBox.y2 = srcy + height;

   /* Don't create a source region if we are doing a fast clip */
   if (fastClip) {
      fastExpose = 1;

      /*
       * clip the source; if regions extend beyond the source size, make sure
       * exposure events get sent
       */
      if (fastBox.x1 < pSrcDrawable->x) {
         fastBox.x1 = pSrcDrawable->x;
         fastExpose = 0;
      }
      if (fastBox.y1 < pSrcDrawable->y) {
         fastBox.y1 = pSrcDrawable->y;
         fastExpose = 0;
      }
      if (fastBox.x2 > pSrcDrawable->x + (int)pSrcDrawable->width) {
         fastBox.x2 = pSrcDrawable->x + (int)pSrcDrawable->width;
         fastExpose = 0;
      }
      if (fastBox.y2 > pSrcDrawable->y + (int)pSrcDrawable->height) {
         fastBox.y2 = pSrcDrawable->y + (int)pSrcDrawable->height;
         fastExpose = 0;
      }
   } else {
      (*pGC->pScreen->RegionInit) (&rgnDst, &fastBox, 1);
      (*pGC->pScreen->Intersect) (&rgnDst, &rgnDst, prgnSrcClip);
   }

   dstx += pDstDrawable->x;
   dsty += pDstDrawable->y;

   if (pDstDrawable->type == DRAWABLE_WINDOW) {
      if (!((WindowPtr) pDstDrawable)->realized) {
         if (!fastClip)
            (*pGC->pScreen->RegionUninit) (&rgnDst);
         if (freeSrcClip)
            (*pGC->pScreen->RegionDestroy) (prgnSrcClip);
         if (pBitmap)
            (*pSrcDrawable->pScreen->DestroyPixmap)(pBitmap);
         return NULL;
      }
   }
   dx = srcx - dstx;
   dy = srcy - dsty;

   /* Translate and clip the dst to the destination composite clip */
   if (fastClip) {
      RegionPtr cclip;

      /* Translate the region directly */
      fastBox.x1 -= dx;
      fastBox.x2 -= dx;
      fastBox.y1 -= dy;
      fastBox.y2 -= dy;

      /*
       * If the destination composite clip is one rectangle we can do the clip
       * directly.  Otherwise we have to create a full blown region and call
       * intersect
       */
      cclip = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;
      if (REGION_NUM_RECTS(cclip) == 1) {
         BoxPtr pBox = REGION_RECTS(cclip);

         if (fastBox.x1 < pBox->x1)
            fastBox.x1 = pBox->x1;
         if (fastBox.x2 > pBox->x2)
            fastBox.x2 = pBox->x2;
         if (fastBox.y1 < pBox->y1)
            fastBox.y1 = pBox->y1;
         if (fastBox.y2 > pBox->y2)
            fastBox.y2 = pBox->y2;

       /* Check to see if the region is empty */
         if (fastBox.x1 >= fastBox.x2 || fastBox.y1 >= fastBox.y2)
            (*pGC->pScreen->RegionInit) (&rgnDst, NullBox, 0);
         else
            (*pGC->pScreen->RegionInit) (&rgnDst, &fastBox, 1);
      } else {
         /*
          * We must turn off fastClip now, since we must create a full blown
          * region.  It is intersected with the composite clip below.
          */
         fastClip = 0;
         (*pGC->pScreen->RegionInit) (&rgnDst, &fastBox, 1);
      }
   } else {
      (*pGC->pScreen->TranslateRegion) (&rgnDst, -dx, -dy);
   }

   if (!fastClip) {
      (*pGC->pScreen->Intersect) (&rgnDst,
                                  &rgnDst,
                                  ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
   }

   /* Do bit blitting */
   numRects = REGION_NUM_RECTS(&rgnDst);
   if (numRects && width && height) {
      pbox = REGION_RECTS(&rgnDst);

      if (pSrcDrawable->type == DRAWABLE_WINDOW && 
          pDstDrawable->type == DRAWABLE_WINDOW) {
         /* Window --> Window */
         unsigned int *ordering;
         BoxPtr prect;
         short direction = 0;

         ordering = (unsigned int *)ALLOCATE_LOCAL(numRects * 
                                                   sizeof(unsigned int));

         if (!ordering) {
            DEALLOCATE_LOCAL(ordering);
            return (RegionPtr) NULL;
         }
         mach8FindOrdering(pSrcDrawable, pDstDrawable, pGC, numRects, pbox, 
                           srcx, srcy, dstx, dsty, ordering);

         if (dx > 0)
            direction |= INC_X;
         if (dy > 0)
            direction |= INC_Y;

         WaitQueue(6);
         outw(FRGD_MIX, FSS_FRGDCOL | mach8alu[pGC->alu]);
         outw(RD_MASK, (unsigned short) (((bitPlane & 0x7f) << 1)
                                       | ((bitPlane & 0x80) >> 7)));
         outw(WRT_MASK, pGC->planemask);
         outw(FRGD_COLOR, (short)pGC->fgPixel);
         outw(BKGD_COLOR, (short)pGC->bgPixel);
         outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPBLT);

         if (direction == (INC_X | INC_Y)) {
            for (i = 0; i < numRects; i++) {
               prect = &pbox[ordering[i]];

               WaitQueue(7);
               outw(CUR_X, (short)(prect->x1 + dx));
               outw(CUR_Y, (short)(prect->y1 + dy));
               outw(DESTX_DIASTP, (short)(prect->x1));
               outw(DESTY_AXSTP, (short)(prect->y1));
               outw(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
               outw(MULTIFUNC_CNTL, 
                    MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
               outw(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
            }
         } else if (direction == INC_X) {
            for (i = 0; i < numRects; i++) {
               prect = &pbox[ordering[i]];

               WaitQueue(7);
               outw(CUR_X, (short)(prect->x1 + dx));
               outw(CUR_Y, (short)(prect->y2 + dy - 1));
               outw(DESTX_DIASTP, (short)(prect->x1));
               outw(DESTY_AXSTP, (short)(prect->y2 - 1));
               outw(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
               outw(MULTIFUNC_CNTL, 
                    MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
               outw(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
            }
         } else if (direction == INC_Y) {
            for (i = 0; i < numRects; i++) {
               prect = &pbox[ordering[i]];

               WaitQueue(7);
               outw(CUR_X, (short)(prect->x2 + dx - 1));
               outw(CUR_Y, (short)(prect->y1 + dy));
               outw(DESTX_DIASTP, (short)(prect->x2 - 1));
               outw(DESTY_AXSTP, (short)(prect->y1));
               outw(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
               outw(MULTIFUNC_CNTL, 
                    MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
               outw(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
            }
         } else {
            for (i = 0; i < numRects; i++) {
               prect = &pbox[ordering[i]];

               WaitQueue(7);
               outw(CUR_X, (short)(prect->x2 + dx - 1));
               outw(CUR_Y, (short)(prect->y2 + dy - 1));
               outw(DESTX_DIASTP, (short)(prect->x2 - 1));
               outw(DESTY_AXSTP, (short)(prect->y2 - 1));
               outw(MAJ_AXIS_PCNT, (short)(prect->x2 - prect->x1 - 1));
               outw(MULTIFUNC_CNTL, 
                    MIN_AXIS_PCNT | (short)(prect->y2 - prect->y1 - 1));
               outw(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
            }
         }
         DEALLOCATE_LOCAL(ordering);

         WaitQueue(4);
         outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
         outw(RD_MASK, 0xffff);
         outw(WRT_MASK, 0xffff);
         outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);
      } else if (pSrcDrawable->type == DRAWABLE_WINDOW && 
                pDstDrawable->type != DRAWABLE_WINDOW) {
         /* Window --> Pixmap */
         /* THIS IS NOT IMPLEMENTED - IT GETS PASSED TO miCopyPlane ABOVE */
      } else if (pSrcDrawable->type != DRAWABLE_WINDOW && 
                 pDstDrawable->type == DRAWABLE_WINDOW) {
         /* Pixmap --> Window */
         PixmapPtr pix = (PixmapPtr) pSrcDrawable;
         int   pixWidth;
         char *psrc;

         pixWidth = PixmapBytePad(pSrcDrawable->width, pSrcDrawable->depth);
         psrc = pix->devPrivate.ptr;

         for (i = numRects; --i >= 0; pbox++) {
            (mach8ImageStippleFunc)(pbox->x1, pbox->y1,
				    pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				    psrc, pixWidth,
				    pix->drawable.width, pix->drawable.height,
				    -dx, -dy, pGC->fgPixel, pGC->bgPixel,
				    mach8alu[pGC->alu],
				    (short) pGC->planemask, 1);
         }
      } else {
         /* Pixmap --> Pixmap */
         ErrorF("mach8CopyPlane:  Tried to do a Pixmap to Pixmap copy\n");
      }
   }
   prgnExposed = NULL;
   if (((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->fExpose) {
      extern RegionPtr miHandleExposures();

      /* Pixmap sources generate a NoExposed (we return NULL to do this) */
      if (!fastExpose)
         prgnExposed =
            miHandleExposures(pSrcDrawable, pDstDrawable, pGC,
                              origSource.x, origSource.y,
                              (int)origSource.width,
                              (int)origSource.height,
                              origDest.x, origDest.y, 0);
   }
   (*pGC->pScreen->RegionUninit) (&rgnDst);
   if (freeSrcClip)
      (*pGC->pScreen->RegionDestroy) (prgnSrcClip);
   if (pBitmap)
      (*pSrcDrawable->pScreen->DestroyPixmap)(pBitmap);
   return prgnExposed;
}

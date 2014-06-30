/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64blt.c,v 3.8 1996/12/23 06:39:09 dawes Exp $ */
/*
 * Copyright 1989 by the Massachusetts Institute of Technology
 * Copyright 1993,1994,1995,1996 by Kevin E. Martin, Chapel Hill, North Carolina.
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  M.I.T. makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 * 
 * Author: Keith Packard
 * 
 * Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * 
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)
 * 
 */
/* $XConsortium: mach64blt.c /main/7 1996/10/19 17:53:30 kaleb $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"cfb.h"
#include	"cfb16.h"
#include	"cfb32.h"
#include	"cfbmskbits.h"
#include	"cfb8bit.h"
#include	"fastblt.h"
#include	"mach64.h"
#include	"mi.h"


static RegionPtr mach64BitBlt();
static void mach64FindOrdering();
static void mach64CopyPlane1toN();
static void mach64CopyPlaneNto1();
static void cfbCopyPlane32to1();
static void cfbCopyPlane16to1();
static void cfbCopyPlane1to32();
static void cfbCopyPlane1to16();


RegionPtr
mach64CopyArea(pSrcDrawable, pDstDrawable,
		pGC, srcx, srcy, width, height, dstx, dsty)
    register DrawablePtr pSrcDrawable;
    register DrawablePtr pDstDrawable;
    GC *pGC;
    int srcx, srcy;
    int width, height;
    int dstx, dsty;
{
    RegionPtr pRgnExposed;

    if (!xf86VTSema || (pSrcDrawable->type != DRAWABLE_WINDOW &&
			pDstDrawable->type != DRAWABLE_WINDOW))
    {
	switch (max(pSrcDrawable->bitsPerPixel, pDstDrawable->bitsPerPixel))
	{
	    case 8:
		return cfbCopyArea(pSrcDrawable, pDstDrawable, pGC,
			   	   srcx, srcy, width, height, dstx, dsty);
	    case 16:
		return cfb16CopyArea(pSrcDrawable, pDstDrawable, pGC,
			   	     srcx, srcy, width, height, dstx, dsty);
	    case 32:
		return cfb32CopyArea(pSrcDrawable, pDstDrawable, pGC,
			   	     srcx, srcy, width, height, dstx, dsty);
	    default:
		ErrorF("mach64CopyArea: unsupported depth %d \n",
			max(pSrcDrawable->bitsPerPixel, 
			    pDstDrawable->bitsPerPixel));
		return;
	}
    }
    else
    {
        pRgnExposed = mach64BitBlt (pSrcDrawable, pDstDrawable, pGC, 
				    srcx, srcy, width, height, dstx, dsty,
                                    mach64DoBitBlt, 0);
        return pRgnExposed;
    }
}

RegionPtr
mach64CopyPlane(pSrcDrawable, pDstDrawable,
	       pGC, srcx, srcy, width, height, dstx, dsty, bitPlane)
    DrawablePtr		pSrcDrawable;
    DrawablePtr		pDstDrawable;
    GCPtr		pGC;
    int			srcx, srcy;
    int			width, height;
    int			dstx, dsty;
    unsigned long	bitPlane;
{
    RegionPtr  ret;

    /* xf86VTSema is handled in mach64CopyPlane1toN and mach64CopyPlaneNto1 */

    if (pSrcDrawable->bitsPerPixel == 1)  /* 1 to N copy */
    {
	if (bitPlane == 1)
	{
	    ret =  mach64BitBlt (pSrcDrawable, pDstDrawable, 
				 pGC, srcx, srcy, width, height, 
				 dstx, dsty, (void *)mach64CopyPlane1toN,
				 bitPlane);
	}
	else
	{
            ret = miHandleExposures(pSrcDrawable, pDstDrawable, 
				    pGC, srcx, srcy, width, height,
				    dstx, dsty, bitPlane);
	}
    }
    else if (pDstDrawable->bitsPerPixel == 1) /* N to 1 copy */
    {
        extern  int InverseAlu[16];
        int oldalu = pGC->alu;

        if ((pGC->fgPixel & 1) == 0 && (pGC->bgPixel & 1) == 1)
	{
            pGC->alu = InverseAlu[pGC->alu];
	}
        else if ((pGC->fgPixel & 1) == (pGC->bgPixel & 1))
	{
            pGC->alu = mfbReduceRop(pGC->alu, pGC->fgPixel);
	}

        ret = mach64BitBlt (pSrcDrawable, pDstDrawable,
                            pGC, srcx, srcy, width, height, dstx, dsty, 
                            mach64CopyPlaneNto1, bitPlane);
        pGC->alu = oldalu;
    }
    else /* N to N copy */
    {
        PixmapPtr       pBitmap;
        ScreenPtr       pScreen = pSrcDrawable->pScreen;
        GCPtr           pGC1;

        pBitmap = (*pScreen->CreatePixmap) (pScreen, width, height, 1);
        if (!pBitmap)
            return NULL;
        pGC1 = GetScratchGC (1, pScreen);
        if (!pGC1)
        {
            (*pScreen->DestroyPixmap) (pBitmap);
            return NULL;
        }
        /*
         * don't need to set pGC->fgPixel,bgPixel as copyPlane8to1
         * ignores pixel values, expecting the rop to "do the
         * right thing", which GXcopy will.
         */
        ValidateGC ((DrawablePtr) pBitmap, pGC1);

        /* no exposures here, scratch GC's don't get graphics expose */
        (void) mach64BitBlt (pSrcDrawable, (DrawablePtr) pBitmap,
                             pGC1, srcx, srcy, width, height, 0, 0, 
                             mach64CopyPlaneNto1, bitPlane);

        /* no exposures here, copy bits from inside a pixmap */
        (void) mach64BitBlt ((DrawablePtr) pBitmap, pDstDrawable, pGC,
                             0, 0, width, height, dstx, dsty, 
			     mach64CopyPlane1toN, 1);

        FreeScratchGC (pGC1);
        (*pScreen->DestroyPixmap) (pBitmap);

        /* compute resultant exposures */
        ret = miHandleExposures (pSrcDrawable, pDstDrawable, pGC,
                                 srcx, srcy, width, height,
                                 dstx, dsty, bitPlane);
    }
    return (ret);
}

void
mach64GetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine)
    DrawablePtr pDrawable;
    int         sx, sy, w, h;
    unsigned int format;
    unsigned long planeMask;
    char        *pdstLine;
{
    BoxRec box;
    DDXPointRec ptSrc;
    RegionRec rgnDst;
    ScreenPtr pScreen;
    PixmapPtr pPixmap;
    GC dummyGC;

    if ((w == 0) || (h == 0))
        return;

    if ((format == ZPixmap) && (pDrawable->type == DRAWABLE_WINDOW))
    {
        pScreen = pDrawable->pScreen;

        pPixmap = GetScratchPixmapHeader(pScreen, w, h, 
                        pDrawable->depth, pDrawable->bitsPerPixel,
                        PixmapBytePad(w,pDrawable->depth), (pointer)pdstLine);
        if (!pPixmap)
            return;
        if ((planeMask & 0xffffffff) != 0xffffffff)
            bzero((char *)pdstLine, pPixmap->devKind * h);

	dummyGC.subWindowMode = ~IncludeInferiors;
	dummyGC.alu = GXcopy;
	dummyGC.planemask = planeMask;

        ptSrc.x = sx + pDrawable->x;
        ptSrc.y = sy + pDrawable->y;
        box.x1 = 0;
        box.y1 = 0;
        box.x2 = w;
        box.y2 = h;
        REGION_INIT(pScreen, &rgnDst, &box, 1);
        mach64DoBitBlt(pDrawable, (DrawablePtr)pPixmap, &dummyGC, &rgnDst,
                       &ptSrc, planeMask);
        REGION_UNINIT(pScreen, &rgnDst);
        FreeScratchPixmapHeader(pPixmap);
    }
    else
    {
	switch (pDrawable->bitsPerPixel)
	{
	    case 1:
		mfbGetImage (pDrawable, sx, sy, w, h, format, planeMask, 
			     pdstLine);
		break;
	    case 8:
		cfbGetImage (pDrawable, sx, sy, w, h, format, planeMask, 
			     pdstLine);
		break;
	    case 16:
		cfb16GetImage (pDrawable, sx, sy, w, h, format, planeMask, 
			       pdstLine);
		break;
	    case 32:
		cfb32GetImage (pDrawable, sx, sy, w, h, format, planeMask, 
			       pdstLine);
		break;
	    default:
		ErrorF("mach64GetImage: unsupported depth %d \n",
			pDrawable->bitsPerPixel);
	}
    }
}

static RegionPtr
mach64BitBlt (pSrc, pDst, pGC, srcX, srcY, copyWidth, copyHeight, 
	      dstX, dstY, doBitBlt, bitPlane)
    DrawablePtr   pSrc;	         /* Pointer to source drawable */
    DrawablePtr   pDst;	         /* Pointer to destination drawable */
    GCPtr  	  pGC;	         /* Pointer to current GC */
    int  	  srcX;          /* Upper left coord of source */
    int  	  srcY;          /* Upper left coord of source */
    int  	  copyWidth;     /* Width of area to copy */
    int  	  copyHeight;    /* Height of area to copy */
    int  	  dstX;          /* Upper left coord of destination */
    int  	  dstY;          /* Upper left coord of destination */
    void	 (*doBitBlt)();  /* Ptr to function to do the copy */
    unsigned long bitPlane;      /* Plane number to copy */
{
    RegionPtr       pRgnSrcClip;  /* may be a new region, or just a copy */
    RegionPtr       pRgnDstClip;  /* may be a new region, or just a copy */
    Bool            freeSrcClip;  /* flag stating if you can free pRgnSrcClip */
    RegionPtr       pRgnExposed;  /* region exposed */
    RegionRec       clippedDstRgn;/* destination region after clipping */
    DDXPointRec     clippedSrcPt; /* source point after clipping */
    register BoxPtr pClipRect;	  /* list of clip rectangles */
    register int    nClipRects;	  /* number of clip rectangles */
    register int    dx, dy;	  /* difference in x and y directions */
    register int    srcX1, srcY1; /* top left corner of source region */
    register int    srcX2, srcY2; /* bottom right corner of source region */
    register int    clipXMin;	  /* min X value */
    register int    clipXMax;	  /* max X value */
    register int    clipYMin;	  /* min Y value */
    register int    clipYMax;	  /* max Y value */
    cfbPrivGCPtr    pGCPriv;	  /* pointer to GC's dev private struct */
    Bool            fastClip;	  /* for fast clipping with pixmap source */
    Bool            fastExpose;	  /* for fast exposures with pixmap source */


    freeSrcClip = FALSE;
    fastClip = FALSE;
    fastExpose = FALSE;

    pGCPriv = (cfbPrivGCPtr)(pGC->devPrivates[cfbGCPrivateIndex].ptr);
    pRgnDstClip = pGCPriv->pCompositeClip; 

    if (pDst->type == DRAWABLE_WINDOW) 
    {
	if (!((WindowPtr)pDst)->realized)
	{
	    return NULL;
	}
    }
    /* clip the source */
    if (pSrc->type == DRAWABLE_WINDOW)
    {
	if (pGC->subWindowMode == IncludeInferiors)
	{
	    if (!((WindowPtr)pSrc)->parent)
	    {
                /*
                 * Special case bitblt from root window in IncludeInferiors 
		 * mode; just like from a pixmap
                 */
		pRgnSrcClip = NULL;
		fastClip = TRUE;
		clipXMin = pSrc->x;
		clipYMin = pSrc->y;
		clipXMax = pSrc->x + pSrc->width;
		clipYMax = pSrc->y + pSrc->height;
	    }
	    else if ((pSrc == pDst) && (pGC->clientClipType == CT_NONE))
	    {
	        pRgnSrcClip = pGCPriv->pCompositeClip;
	    }
	    else
	    {
		pRgnSrcClip = NotClippedByChildren((WindowPtr)pSrc);
		freeSrcClip = TRUE;
	    }
	}
	else
	{
	    pRgnSrcClip = &((WindowPtr)pSrc)->clipList;
	}
    }
    else  /* DRAWABLE_PIXMAP */
    {
	if ((pSrc == pDst) && (pGC->clientClipType == CT_NONE))
	{
	    pRgnSrcClip = pGCPriv->pCompositeClip;
	}
	else
	{
            /*
             * Pixmap is just one clipping rect so we can avoid allocating a 
             * full blown region.
             */
	    pRgnSrcClip = NULL;
	    fastClip = TRUE;
	    clipXMin = pSrc->x;
	    clipYMin = pSrc->y;
	    clipXMax = pSrc->x + pSrc->width;
	    clipYMax = pSrc->y + pSrc->height;
	}
    }

    if ((pRgnSrcClip) && (REGION_NUM_RECTS(pRgnSrcClip) == 1))
    {
        /*
         * Optimize if there is only one source clip rect.
         */
        fastClip = TRUE;
        pClipRect = REGION_RECTS(pRgnSrcClip);
        clipXMin = pClipRect->x1;
        clipYMin = pClipRect->y1;
        clipXMax = pClipRect->x2;
        clipYMax = pClipRect->y2;
    }

    srcX1 = srcX + pSrc->x;
    srcY1 = srcY + pSrc->y;
    srcX2 = srcX1 + copyWidth;
    srcY2 = srcY1 + copyHeight;

    dx = srcX1 - (dstX + pDst->x);
    dy = srcY1 - (dstY + pDst->y);

    if (fastClip)
    {
        /*
         * Don't create a source region if we are doing a fast clip.
         * Clip the source; if regions extend beyond the source size,
         * make sure exposure events get sent.
         */
	fastExpose = TRUE;

	if (srcX1 < clipXMin)
	{
	    srcX1 = clipXMin;
	    fastExpose = FALSE;
	}
	if (srcY1 < clipYMin)
	{
	    srcY1 = clipYMin;
	    fastExpose = FALSE;
	}
	if (srcX2 > clipXMax)
	{
	    srcX2 = clipXMax;
	    fastExpose = FALSE;
	}
	if (srcY2 > clipYMax)
	{
	    srcY2 = clipYMax;
	    fastExpose = FALSE;
	}

        /*
         * Translate and clip the dst to the destination composite clip 
         */
        srcX1 -= dx;
        srcX2 -= dx;
        srcY1 -= dy;
        srcY2 -= dy;

        /*
         * If the destination composite clip is one rectangle we can
         * do the clip directly.  Otherwise we have to create a full
         * blown region and call intersect.
         */
	nClipRects = REGION_NUM_RECTS(pRgnDstClip);
        if (nClipRects == 1)
        {
	    pClipRect = REGION_RECTS(pRgnDstClip);
	
	    clippedDstRgn.extents.x1 = max(srcX1, pClipRect->x1);
	    clippedDstRgn.extents.y1 = max(srcY1, pClipRect->y1);
	    clippedDstRgn.extents.x2 = min(srcX2, pClipRect->x2);
	    clippedDstRgn.extents.y2 = min(srcY2, pClipRect->y2);
	    clippedDstRgn.data = (RegDataPtr)NULL;
	    clippedSrcPt.x = clippedDstRgn.extents.x1 + dx;
	    clippedSrcPt.y = clippedDstRgn.extents.y1 + dy;
            if ((clippedDstRgn.extents.y2 > clippedDstRgn.extents.y1) &&
	        (clippedDstRgn.extents.x2 > clippedDstRgn.extents.x1))
	    {
                (*doBitBlt)(pSrc, pDst, pGC, &clippedDstRgn, &clippedSrcPt, 
			    bitPlane);
	    }
	}
        else
	{
            /*
             * We must turn off fastClip now, since we must create 
	     * a full blown region.  It is intersected with the 
	     * composite clip below. 
             */

	    fastClip = FALSE;
	    clippedDstRgn.extents.x1 = srcX1;
	    clippedDstRgn.extents.y1 = srcY1;
	    clippedDstRgn.extents.x2 = srcX2;
	    clippedDstRgn.extents.y2 = srcY2;
	    clippedDstRgn.data = (RegDataPtr)NULL;
	    (*pGC->pScreen->Intersect)(&clippedDstRgn, &clippedDstRgn, 
				       pRgnDstClip);
	}
    } /* end section that set up a dest region with fastClip turned on */
    else
    {
	clippedDstRgn.extents.x1 = srcX1;
	clippedDstRgn.extents.y1 = srcY1;
	clippedDstRgn.extents.x2 = srcX2;
	clippedDstRgn.extents.y2 = srcY2;
	clippedDstRgn.data = (RegDataPtr)NULL;
	(*pGC->pScreen->Intersect)(&clippedDstRgn, &clippedDstRgn,
				   pRgnSrcClip);
        (*pGC->pScreen->TranslateRegion)(&clippedDstRgn, -dx, -dy);
	(*pGC->pScreen->Intersect)(&clippedDstRgn, &clippedDstRgn, 
				   pRgnDstClip);
    } /* end section that set up a dest region with fastClip turned off */

    if (!fastClip) 
    {
	/*
 	 * Do bit blitting if fastClip is not set.
 	 */

        nClipRects = REGION_NUM_RECTS(&clippedDstRgn);

        if (nClipRects)
        {
            register int         i;	       /* counter */
    	    DDXPointPtr          pClippedSrcPt;/* source point after clipping */
            register DDXPointPtr pPt;          /* used to traverse 
						  pClippedSrcPt */

	    pClippedSrcPt =(DDXPointPtr)ALLOCATE_LOCAL(nClipRects *
						       sizeof(DDXPointRec));

	    if(!(pClippedSrcPt))
	    {
	        (*pGC->pScreen->RegionUninit)(&clippedDstRgn);
	        if (freeSrcClip)
		{
		    (*pGC->pScreen->RegionDestroy)(pRgnSrcClip);
		}
	        return NULL;
	    }

	    pClipRect = REGION_RECTS(&clippedDstRgn);
	    pPt = pClippedSrcPt;

	    for (i = nClipRects; i > 0; i--)
	    {
	        pPt->x = pClipRect->x1 + dx;
	        pPt->y = pClipRect->y1 + dy;
		pClipRect++;
		pPt++;
	    }
            (*doBitBlt)(pSrc, pDst, pGC, &clippedDstRgn, pClippedSrcPt,
		        bitPlane);
	    DEALLOCATE_LOCAL(pClippedSrcPt);
        }
        (*pGC->pScreen->RegionUninit)(&clippedDstRgn);
    }


    pRgnExposed = NULL;
    if ((!fastExpose) && (pGCPriv->fExpose))
    {
        /*
         * Pixmap sources generate a NoExposed. (we return NULL to do this) 
         */
	pRgnExposed = miHandleExposures(pSrc, pDst, pGC, srcX, srcY, 
					copyWidth, copyHeight,
				  	dstX, dstY, (unsigned long)0);
    }

    if (freeSrcClip)
    {
	(*pGC->pScreen->RegionDestroy)(pRgnSrcClip);
    }

    return pRgnExposed;
}

static void
mach64FindOrdering(pSrcDrawable, pDstDrawable, pGC, numRects, boxes, 
		   srcx, srcy, dstx, dsty, ordering)
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
     * Following assumes that boxes are sorted from top
     * to bottom and left to right.
     */
    if ((pSrcDrawable != pDstDrawable) &&
        ((pGC->subWindowMode != IncludeInferiors) ||
         (pSrcDrawable->type == DRAWABLE_PIXMAP) ||
         (pDstDrawable->type == DRAWABLE_PIXMAP))) {
        for (i=0; i < numRects; i++)
	    ordering[i] = i;
    } else { /* within same drawable, must sequence moves carefully! */
	if (dsty <= srcy) { 
	    /* Scroll up or stationary vertical.  Vertical order OK */
	    if (dstx <= srcx) { 
		/* Scroll left or stationary horizontal.  Horizontal order OK */
		for (i=0; i < numRects; i++)
		    ordering[i] = i;
	    } else { 
		/* scroll right. must reverse horizontal banding of rects. */
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
		        Reverse horizontal order as well (if stationary, 
			horizontal order can be swapped without penalty and 
			this is faster to compute). */
		for (i=0, j=numRects-1; i < numRects; i++, j--)
		    ordering[i] = j;
	    }
	}
    }
}

void
mach64DoBitBlt (pSrc, pDst, pGC, rgnDst, pptSrc, bitPlane)
    DrawablePtr	  pSrc;		/* Pointer to source drawable */
    DrawablePtr	  pDst;		/* Pointer to destination drawable */
    GCPtr	  pGC;		/* Pointer to current GC */
    RegionPtr	  rgnDst;	/* Destination region */
    DDXPointPtr	  pptSrc;	/* Source region */
    unsigned long bitPlane;   	/* Not used by this function but part of
				   function vector parameter list */
{
    int         numRects;	/* number of clip rectangles */
    BoxPtr      pbox;	        /* list of clip rectangles */


    pbox = REGION_RECTS(rgnDst);
    numRects = REGION_NUM_RECTS(rgnDst);

    /*
     * Copy from window to window.
     */
    if ((pSrc->type == DRAWABLE_WINDOW) && (pDst->type == DRAWABLE_WINDOW))
    {
        unsigned int *ordering;
        register BoxPtr prect;
	register int i;
	register int dx, dy;
        int direction = 0;

	dx = pptSrc->x - pbox->x1;
	dy = pptSrc->y - pbox->y1;

        ordering = (unsigned int *)
                    ALLOCATE_LOCAL(numRects * sizeof(unsigned int));
        if(!ordering)
            return;

        mach64FindOrdering(pSrc, pDst, pGC, numRects, pbox, 
			   pptSrc->x, pptSrc->y, 
			   pbox->x1, pbox->y1, ordering);

        if (dx > 0) direction |= INC_X;
        if (dy > 0) direction |= INC_Y;

        WaitQueue(4);
        regw(DP_MIX, (mach64alu[pGC->alu] << 16) | mach64alu[pGC->alu]);
        regw(DP_WRITE_MASK, pGC->planemask);
        regw(DP_SRC, FRGD_SRC_BLIT);
        regw(SRC_CNTL, 0x00000000);

        if (direction == (INC_X | INC_Y)) {
            for (i = 0; i < numRects; i++) {
                prect = &pbox[ordering[i]];
                MACH64_BIT_BLT((prect->x1 + dx), (prect->y1 + dy),
                                prect->x1, prect->y1,
                               (prect->x2 - prect->x1),
                               (prect->y2 - prect->y1),
                               (DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM));
            }
        } else if (direction == INC_X) {
            for (i = 0; i < numRects; i++) {
                prect = &pbox[ordering[i]];
                MACH64_BIT_BLT((prect->x1 + dx), (prect->y2 + dy - 1),
                                prect->x1, (prect->y2 - 1),
                               (prect->x2 - prect->x1),
                               (prect->y2 - prect->y1),
                               (DST_X_LEFT_TO_RIGHT | DST_Y_BOTTOM_TO_TOP));
            }
        } else if (direction == INC_Y) {
            for (i = 0; i < numRects; i++) {
                prect = &pbox[ordering[i]];
                MACH64_BIT_BLT((prect->x2 + dx - 1), (prect->y1 + dy),
                               (prect->x2 - 1), prect->y1,
                               (prect->x2 - prect->x1),
                               (prect->y2 - prect->y1),
                               (DST_X_RIGHT_TO_LEFT | DST_Y_TOP_TO_BOTTOM));
            }
        } else {
            for (i = 0; i < numRects; i++) {
                prect = &pbox[ordering[i]];
                MACH64_BIT_BLT((prect->x2 + dx - 1), (prect->y2 + dy - 1),
                               (prect->x2 - 1), (prect->y2 - 1),
                               (prect->x2 - prect->x1),
                               (prect->y2 - prect->y1),
                               (DST_X_RIGHT_TO_LEFT | DST_Y_BOTTOM_TO_TOP));
            }
        }
        WaitIdleEmpty(); /* Make sure that all commands have finished */

        /*
         * Make sure that the destination trajectory is correctly set
         * for subsequent calls.  MACH64_BIT_BLT is the only function that
         * currently changes the destination trajectory from L->R and T->B.
         */
        regw(DST_CNTL, (DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM));
    }
    /*
     * Copy from an pixmap to a window.
     */
    else if ((pSrc->type != DRAWABLE_WINDOW) && (pDst->type == DRAWABLE_WINDOW))
    {
	char *psrc = ((PixmapPtr)pSrc)->devPrivate.ptr;
	int pixWidth = PixmapBytePad(pSrc->width, pSrc->depth);

	while(numRects --)
	{
	    {
		(mach64ImageWriteFunc)(pbox->x1, pbox->y1,
				  pbox->x2 - pbox->x1, 
				  pbox->y2 - pbox->y1,
				  psrc, pixWidth, pptSrc->x, pptSrc->y,
				  mach64alu[pGC->alu], pGC->planemask);
		pbox++;
		pptSrc++;
	    }
	}
    }

    /*
     * Copy from a window to a pixmap.
     */
    else if ((pSrc->type == DRAWABLE_WINDOW) && (pDst->type != DRAWABLE_WINDOW))
    {
	if ((pGC->planemask & 0xffffffff) == 0xffffffff)
	{
	    int pixWidth = PixmapBytePad(pDst->width, pDst->depth);
	    char *pdst = ((PixmapPtr)pDst)->devPrivate.ptr;

	    while(numRects --)
	    {
	        (mach64ImageReadFunc)(pptSrc->x, pptSrc->y,
			              pbox->x2 - pbox->x1, 
				      pbox->y2 - pbox->y1,
			 	      pdst, pixWidth, 
				      pbox->x1, pbox->y1,
			 	      pGC->planemask);
                pbox++;
                pptSrc++;
	    }
	}
	else
	{
	    switch (max(pSrc->bitsPerPixel, pDst->bitsPerPixel))
	    {
	    	case 8:
		    cfbDoBitblt (pSrc, pDst, pGC->alu, 
		                 rgnDst, pptSrc, pGC->planemask);
		    break;
	    	case 16:
		    cfb16DoBitblt (pSrc, pDst, pGC->alu, 
		                   rgnDst, pptSrc, pGC->planemask);
		    break;
	    	case 32:
		    cfb32DoBitblt (pSrc, pDst, pGC->alu, 
		                   rgnDst, pptSrc, pGC->planemask);
		    break;
		default:
		    ErrorF("mach64DoBitBlt: unsupported depth %d \n",
			    max(pSrc->bitsPerPixel, pDst->bitsPerPixel));
	    }
	} 
    }
    else
    {
	ErrorF("mach64DoBitBlt: pixmap to pixmap copy not supported\n");
    }
    return;
}

static void
mach64CopyPlane1toN(pSrcDrawable, pDstDrawable, pGC, rgnDst, pptSrc, bitPlane)
    DrawablePtr   pSrcDrawable;
    DrawablePtr   pDstDrawable;
    GCPtr         pGC;
    RegionPtr     rgnDst;
    DDXPointPtr   pptSrc;
    unsigned long bitPlane;
{
    BoxPtr pbox;
    int pixWidth, i, numRects;
    char *psrc;

    if ((pSrcDrawable->type != DRAWABLE_WINDOW) && 
	(pDstDrawable->type == DRAWABLE_WINDOW) && xf86VTSema)
    {
        pixWidth = PixmapBytePad(pSrcDrawable->width, pSrcDrawable->depth);
        psrc = ((PixmapPtr)pSrcDrawable)->devPrivate.ptr;
        pbox = REGION_RECTS(rgnDst);
        numRects = REGION_NUM_RECTS(rgnDst);

        for (i = numRects; --i >= 0; pbox++, pptSrc++) 
        {
            mach64ImageStippleFunc(pbox->x1, pbox->y1,
                                   pbox->x2 - pbox->x1, 
			           pbox->y2 - pbox->y1,
                                   psrc, pixWidth,
                                   pptSrc->x, pptSrc->y, 
			           pGC->fgPixel, pGC->bgPixel,
                                   mach64alu[pGC->alu], pGC->planemask, 1);
        }
    }
    else
    {
	switch (pDstDrawable->bitsPerPixel)
	{
	    case 8:
                cfb8CheckOpaqueStipple (pGC->alu, pGC->fgPixel, pGC->bgPixel,
                                        pGC->planemask);
		cfbCopyPlane1to8(pSrcDrawable, pDstDrawable, pGC->alu, 
				 rgnDst, pptSrc, pGC->planemask, bitPlane);
		break;
	    case 16:
		cfbCopyPlane1to16(pSrcDrawable, pDstDrawable, pGC->alu, 
				  rgnDst, pptSrc, pGC->planemask, bitPlane, 
				  pGC);
		break;
	    case 32:
		cfbCopyPlane1to32(pSrcDrawable, pDstDrawable, pGC->alu, 
				  rgnDst, pptSrc, pGC->planemask, bitPlane, 
				  pGC);
		break;
	    default:
	        ErrorF("mach64CopyPlane1toN: unsupported function \n");
		break;
	}
    }
}

static void
mach64CopyPlaneNto1(pSrcDrawable, pDstDrawable, pGC, rgnDst, pptSrc, bitPlane)
    DrawablePtr   pSrcDrawable;
    DrawablePtr   pDstDrawable;
    GCPtr         pGC;
    RegionPtr     rgnDst;
    DDXPointPtr   pptSrc;
    unsigned long bitPlane;
{
    BoxPtr pbox;
    int pixWidth, i, numRects;
    unsigned char *psrc;

    if (!(pGC->planemask & bitPlane))
        return;

    switch (pSrcDrawable->bitsPerPixel)
    {
	case 8:
	    cfbCopyPlane8to1(pSrcDrawable, pDstDrawable, pGC->alu, 
			     rgnDst, pptSrc, pGC->planemask, bitPlane);
	    break;
	case 16:
	    cfbCopyPlane16to1(pSrcDrawable, pDstDrawable, pGC->alu, 
			      rgnDst, pptSrc, pGC->planemask, bitPlane);
	    break;
	case 32:
	    cfbCopyPlane32to1(pSrcDrawable, pDstDrawable, pGC->alu, 
			      rgnDst, pptSrc, pGC->planemask, bitPlane);
	    break;
	default:
	    ErrorF("mach64CopyPlaneNto1: unsupported function \n");
	    break;
    }
}

/******************************************************************/

#define mfbmaskbits(x, w, startmask, endmask, nlw) \
    startmask = starttab[(x)&0x1f]; \
    endmask = endtab[((x)+(w)) & 0x1f]; \
    if (startmask) \
	nlw = (((w) - (32 - ((x)&0x1f))) >> 5); \
    else \
	nlw = (w) >> 5;

#define mfbmaskpartialbits(x, w, mask) \
    mask = partmasks[(x)&0x1f][(w)&0x1f];

#define LeftMost    0
#define StepBit(bit, inc)  ((bit) += (inc))


#define GetBits(psrc, nBits, curBit, bitPos, bits) {\
    bits = 0; \
    while (nBits--) \
    { \
	bits |= ((*psrc++ >> bitPos) & 1) << curBit; \
	StepBit (curBit, 1); \
    } \
}

/******************************************************************/

static void
cfbCopyPlane1to32 (pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc, 
		   planemask, bitPlane, pGC)
    DrawablePtr pSrcDrawable;
    DrawablePtr pDstDrawable;
    int	rop;
    unsigned long planemask;
    RegionPtr prgnDst;
    DDXPointPtr pptSrc;
    int         bitPlane;
    GC          *pGC;
{
    int	srcx, srcy, dstx, dsty;
    int width, height;
    int xoffSrc;
    unsigned long *psrcBase, *pdstBase;
    int	widthSrc, widthDst;
    unsigned int *psrcLine, *pdstLine;
    register unsigned int *psrc, *pdst;
    register unsigned int  bits, tmp;
    register unsigned int  fgpixel, bgpixel;
    register unsigned int  src;
    register int  leftShift, rightShift;
    register int  i, nl;
    int nbox;
    BoxPtr pbox;
    int  result;

    if (pGC)
    {
        fgpixel = pGC->fgPixel & planemask;
        bgpixel = pGC->bgPixel & planemask;
    }
    else
    {
        fgpixel = ~0 & planemask;
        bgpixel = 0;
    }

    /* must explicitly ask for "int" widths, as code below expects it */
    /* on some machines (Alpha), "long" and "int" are not the same size */
    cfbGetTypedWidthAndPointer (pSrcDrawable, widthSrc, psrcBase, int, unsigned long)
    cfbGetTypedWidthAndPointer (pDstDrawable, widthDst, pdstBase, int, unsigned long)

    nbox = REGION_NUM_RECTS(prgnDst);
    pbox = REGION_RECTS(prgnDst);

    while (nbox--)
    {
	dstx = pbox->x1;
	dsty = pbox->y1;
	srcx = pptSrc->x;
	srcy = pptSrc->y;
	width = pbox->x2 - pbox->x1;
	height = pbox->y2 - pbox->y1;
	pbox++;
	pptSrc++;
	psrcLine = (unsigned int *)psrcBase + srcy * widthSrc + (srcx >> 5);
	pdstLine = (unsigned int *)pdstBase + dsty * widthDst + dstx;
	xoffSrc = srcx & 0x1f;

	/*
	 * compute constants for the first four bits to be
	 * copied.  This avoids troubles with partial first
	 * writes, and difficult shift computation
	 */
	leftShift = xoffSrc;
	rightShift = 32 - leftShift;

	if (rop == GXcopy && planemask == 0xffffffff)
	{
	    while (height--)
	    {
	        psrc = psrcLine;
	        pdst = pdstLine;
	        psrcLine += widthSrc;
	        pdstLine += widthDst;
	        bits = *psrc++;
	        nl = width;
   	        while (nl >= 32)
	        {
		    tmp = BitLeft(bits, leftShift);
		    bits = *psrc++;
		    if (rightShift != 32)
		        tmp |= BitRight(bits, rightShift);
		    i = 32;
		    while (i--)
		    {
		        *pdst = ((tmp >> (31 - i)) & 0x01) ? fgpixel : bgpixel;
		        pdst++;
		    }
		    nl -= 32;
	        }

	        if (nl)
	        {
		    tmp = BitLeft(bits, leftShift);
		    /*
		     * better condition needed -- mustn't run
		     * off the end of the source...
		     */
		    if (rightShift != 32)
		    {
		        bits = *psrc++;
		        tmp |= BitRight (bits, rightShift);
		    }
		    i = 32;
		    while (nl--)
		    {
		        --i;
		        *pdst = ((tmp >> (31 - i)) & 0x01) ? fgpixel : bgpixel;
		        pdst++;
		    }
	        }
            }
        }
	else
	{
	    while (height--)
	    {
	        psrc = psrcLine;
	        pdst = pdstLine;
	        psrcLine += widthSrc;
	        pdstLine += widthDst;
	        bits = *psrc++;
	        nl = width;
   	        while (nl >= 32)
	        {
		    tmp = BitLeft(bits, leftShift);
		    bits = *psrc++;
		    if (rightShift != 32)
		        tmp |= BitRight(bits, rightShift);
		    i = 32;
		    while (i--)
		    {
		        src = ((tmp >> (31 - i)) & 0x01) ? fgpixel : bgpixel;
                        DoRop (result, rop, src, *pdst);

		        *pdst = (*pdst & ~planemask) |
		  	        (result & planemask);
		        pdst++;
		    }
		    nl -= 32;
	        }

	        if (nl)
	        {
		    tmp = BitLeft(bits, leftShift);
		    /*
		     * better condition needed -- mustn't run
		     * off the end of the source...
		     */
		    if (rightShift != 32)
		    {
		        bits = *psrc++;
		        tmp |= BitRight (bits, rightShift);
		    }
		    i = 32;
		    while (nl--)
		    {
		        --i;
		        src = ((tmp >> (31 - i)) & 0x01) ? fgpixel : bgpixel;
                        DoRop (result, rop, src, *pdst);

		        *pdst = (*pdst & ~planemask) |
		    	    (result & planemask);
		        pdst++;
		    }
	        }
            }
        }
    }
}

static void
cfbCopyPlane32to1 (pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc, 
		   planemask, bitPlane)
    DrawablePtr pSrcDrawable;
    DrawablePtr pDstDrawable;
    int	rop;
    RegionPtr prgnDst;
    DDXPointPtr pptSrc;
    unsigned long planemask;
    unsigned long bitPlane;
{
    int			    srcx, srcy, dstx, dsty, width, height;
    unsigned long	    *psrcBase;
    unsigned long	    *pdstBase;
    int			    widthSrc, widthDst;
    unsigned int	    *psrcLine;
    unsigned int	    *pdstLine;
    register unsigned int   *psrc;
    register int	    i;
    register int	    curBit;
    register int	    bitPos;
    register unsigned int   bits;
    register unsigned int   *pdst;
    unsigned int	    startmask, endmask;
    int			    niStart, niEnd;
    int			    bitStart, bitEnd;
    int			    nl, nlMiddle;
    int			    nbox;
    BoxPtr		    pbox;
    int result;

    extern int starttab[32], endtab[32];
    extern unsigned int partmasks[32][32];


    if (!(planemask & 1))
	return;

    /* must explicitly ask for "int" widths, as code below expects it */
    /* on some machines (Alpha), "long" and "int" are not the same size */
    cfbGetTypedWidthAndPointer (pSrcDrawable, widthSrc, psrcBase, int, unsigned long)
    cfbGetTypedWidthAndPointer (pDstDrawable, widthDst, pdstBase, int, unsigned long)

    bitPos = ffs (bitPlane) - 1;

    nbox = REGION_NUM_RECTS(prgnDst);
    pbox = REGION_RECTS(prgnDst);
    while (nbox--)
    {
	dstx = pbox->x1;
	dsty = pbox->y1;
	srcx = pptSrc->x;
	srcy = pptSrc->y;
	width = pbox->x2 - pbox->x1;
	height = pbox->y2 - pbox->y1;
	pbox++;
	pptSrc++;
	psrcLine = (unsigned int *)psrcBase + srcy * widthSrc + srcx;
	pdstLine = (unsigned int *)pdstBase + dsty * widthDst + (dstx >> 5);
	if (dstx + width <= 32)
	{
	    mfbmaskpartialbits(dstx, width, startmask);
	    nlMiddle = 0;
	    endmask = 0;
	}
	else
	{
	    mfbmaskbits (dstx, width, startmask, endmask, nlMiddle);
	}
	if (startmask)
	{
	    niStart = 32 - (dstx & 0x1f);
	    bitStart = LeftMost;
	    StepBit (bitStart, (dstx & 0x1f));
	}
	if (endmask)
	{
	    niEnd = (dstx + width) & 0x1f;
	    bitEnd = LeftMost;
	}
	if (rop == GXcopy)
	{
	    while (height--)
	    {
	    	psrc = psrcLine;
	    	pdst = pdstLine;
	    	psrcLine += widthSrc;
	    	pdstLine += widthDst;
	    	if (startmask)
	    	{
		    i = niStart;
		    curBit = bitStart;
		    GetBits (psrc, i, curBit, bitPos, bits);

		    *pdst = (*pdst & ~startmask) | bits;
		    pdst++;
	    	}
	    	nl = nlMiddle;
	    	while (nl--)
	    	{
		    i = 32;
		    curBit = LeftMost;
		    GetBits (psrc, i, curBit, bitPos, bits);
		    *pdst++ = bits;
	    	}
	    	if (endmask)
	    	{
		    i = niEnd;
		    curBit = bitEnd;
		    GetBits (psrc, i, curBit, bitPos, bits);

		    *pdst = (*pdst & ~endmask) | bits;
	    	}
	    }
	}
	else
	{
	    while (height--)
	    {
	    	psrc = psrcLine;
	    	pdst = pdstLine;
	    	psrcLine += widthSrc;
	    	pdstLine += widthDst;
	    	if (startmask)
	    	{
		    i = niStart;
		    curBit = bitStart;
		    GetBits (psrc, i, curBit, bitPos, bits);
                    DoRop (result, rop, bits, *pdst);

		    *pdst = (*pdst & ~startmask) | 
			    (result & startmask);
		    pdst++;
	    	}
	    	nl = nlMiddle;
	    	while (nl--)
	    	{
		    i = 32;
		    curBit = LeftMost;
		    GetBits (psrc, i, curBit, bitPos, bits);
                    DoRop (result, rop, bits, *pdst);
		    *pdst = result;
		    ++pdst;
	    	}
	    	if (endmask)
	    	{
		    i = niEnd;
		    curBit = bitEnd;
		    GetBits (psrc, i, curBit, bitPos, bits);
                    DoRop (result, rop, bits, *pdst);

		    *pdst = (*pdst & ~endmask) |
			    (result & endmask);
	    	}
	    }
	}
    }
}

static void
cfbCopyPlane1to16 (pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc, 
		   planeMask, bitPlane, pGC)
    DrawablePtr pSrcDrawable;
    DrawablePtr pDstDrawable;
    int	rop;
    unsigned long planeMask;
    RegionPtr prgnDst;
    DDXPointPtr pptSrc;
    int         bitPlane;
    GC          *pGC;
{
    int	srcx, srcy, dstx, dsty;
    int width, height;
    int xoffSrc;
    unsigned long *psrcBase, *pdstBase;
    int	widthSrc, widthDst;
    unsigned int *psrcLine;
    unsigned short *pdstLine;
    register unsigned int *psrc;
    register unsigned short *pdst;
    register unsigned int  bits, tmp;
    register unsigned short  fgpixel, bgpixel;
    register unsigned short  src;
    register int  leftShift, rightShift;
    register int  i, nl;
    unsigned short planemask = (unsigned short)planeMask;
    int nbox;
    BoxPtr pbox;
    unsigned short  result;

    if (pGC)
    {
        fgpixel = (unsigned short)pGC->fgPixel & planemask;
        bgpixel = (unsigned short)pGC->bgPixel & planemask;
    }
    else
    {
        fgpixel = ~0 & planemask;
        bgpixel = 0;
    }

    /* must explicitly ask for "int" widths, as code below expects it */
    /* on some machines (Alpha), "long" and "int" are not the same size */
    cfbGetTypedWidthAndPointer (pSrcDrawable, widthSrc, psrcBase, int, unsigned long)
    cfbGetTypedWidthAndPointer (pDstDrawable, widthDst, pdstBase, int, unsigned long)

    widthDst = widthDst << 1;

    nbox = REGION_NUM_RECTS(prgnDst);
    pbox = REGION_RECTS(prgnDst);

    while (nbox--)
    {
	dstx = pbox->x1;
	dsty = pbox->y1;
	srcx = pptSrc->x;
	srcy = pptSrc->y;
	width = pbox->x2 - pbox->x1;
	height = pbox->y2 - pbox->y1;
	pbox++;
	pptSrc++;
	psrcLine = (unsigned int *)psrcBase + srcy * widthSrc + (srcx >> 5);
	pdstLine = (unsigned short *)pdstBase + dsty * widthDst + dstx;
	xoffSrc = srcx & 0x1f;

	/*
	 * compute constants for the first four bits to be
	 * copied.  This avoids troubles with partial first
	 * writes, and difficult shift computation
	 */
	leftShift = xoffSrc;
	rightShift = 32 - leftShift;

	if (rop == GXcopy && planemask == 0xffff)
	{
	    while (height--)
	    {
	        psrc = psrcLine;
	        pdst = pdstLine;
	        psrcLine += widthSrc;
	        pdstLine += widthDst;
	        bits = *psrc++;
	        nl = width;
   	        while (nl >= 32)
	        {
		    tmp = BitLeft(bits, leftShift);
		    bits = *psrc++;
		    if (rightShift != 32)
		        tmp |= BitRight(bits, rightShift);
		    i = 32;
		    while (i--)
		    {
		        *pdst = (((tmp >> (31-i)) & 0x01) ? fgpixel : bgpixel);
		        pdst++;
		    }
		    nl -= 32;
	        }

	        if (nl)
	        {
		    tmp = BitLeft(bits, leftShift);
		    /*
		     * better condition needed -- mustn't run
		     * off the end of the source...
		     */
		    if (rightShift != 32)
		    {
		        bits = *psrc++;
		        tmp |= BitRight (bits, rightShift);
		    }
		    i = 32;
		    while (nl--)
		    {
		        --i;
		        *pdst = (((tmp >> (31-i)) & 0x01) ? fgpixel : bgpixel);
		        pdst++;
		    }
	        }
            }
        }
	else
	{
	    while (height--)
	    {
	        psrc = psrcLine;
	        pdst = pdstLine;
	        psrcLine += widthSrc;
	        pdstLine += widthDst;
	        bits = *psrc++;
	        nl = width;
   	        while (nl >= 32)
	        {
		    tmp = BitLeft(bits, leftShift);
		    bits = *psrc++;
		    if (rightShift != 32)
		        tmp |= BitRight(bits, rightShift);
		    i = 32;
		    while (i--)
		    {
		        src = ((tmp >> (31-i)) & 0x01) ? fgpixel : bgpixel;
                        DoRop (result, rop, src, *pdst);

		        *pdst = (*pdst & ~planemask) |
		  	        (result & planemask);
		        pdst++;
		    }
		    nl -= 32;
	        }

	        if (nl)
	        {
		    tmp = BitLeft(bits, leftShift);
		    /*
		     * better condition needed -- mustn't run
		     * off the end of the source...
		     */
		    if (rightShift != 32)
		    {
		        bits = *psrc++;
		        tmp |= BitRight (bits, rightShift);
		    }
		    i = 32;
		    while (nl--)
		    {
		        --i;
		        src = ((tmp >> (31-i)) & 0x01) ? fgpixel : bgpixel;
                        DoRop (result, rop, src, *pdst);

		        *pdst = (*pdst & ~planemask) |
		    	    (result & planemask);
		        pdst++;
		    }
	        }
            }
        }
    }
}

static void
cfbCopyPlane16to1 (pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc, 
		   planemask, bitPlane)
    DrawablePtr pSrcDrawable;
    DrawablePtr pDstDrawable;
    int	rop;
    RegionPtr prgnDst;
    DDXPointPtr pptSrc;
    unsigned long planemask;
    unsigned long bitPlane;
{
    int			    srcx, srcy, dstx, dsty, width, height;
    unsigned long	    *psrcBase, *pdstBase;
    int			    widthSrc, widthDst;
    unsigned short	    *psrcLine;
    unsigned int	    *pdstLine;
    register unsigned short *psrc;
    register unsigned int   *pdst;
    register int	    i;
    register int	    curBit;
    register int	    bitPos;
    register unsigned int   bits;
    unsigned int	    startmask, endmask;
    int			    niStart, niEnd;
    int			    bitStart, bitEnd;
    int			    nl, nlMiddle;
    int			    nbox;
    BoxPtr		    pbox;
    int result;

    extern int starttab[32], endtab[32];
    extern unsigned int partmasks[32][32];


    if (!(planemask & 1))
	return;

    /* must explicitly ask for "int" widths, as code below expects it */
    /* on some machines (Alpha), "long" and "int" are not the same size */
    cfbGetTypedWidthAndPointer (pSrcDrawable, widthSrc, psrcBase, int, unsigned long)
    cfbGetTypedWidthAndPointer (pDstDrawable, widthDst, pdstBase, int, unsigned long)

    widthSrc = widthSrc << 1;

    bitPos = ffs (bitPlane) - 1;

    nbox = REGION_NUM_RECTS(prgnDst);
    pbox = REGION_RECTS(prgnDst);
    while (nbox--)
    {
	dstx = pbox->x1;
	dsty = pbox->y1;
	srcx = pptSrc->x;
	srcy = pptSrc->y;
	width = pbox->x2 - pbox->x1;
	height = pbox->y2 - pbox->y1;
	pbox++;
	pptSrc++;
	psrcLine = (unsigned short *)psrcBase + srcy * widthSrc + srcx;
	pdstLine = (unsigned int *)pdstBase + dsty * widthDst + (dstx >> 5);
	if (dstx + width <= 32)
	{
	    mfbmaskpartialbits(dstx, width, startmask);
	    nlMiddle = 0;
	    endmask = 0;
	}
	else
	{
	    mfbmaskbits (dstx, width, startmask, endmask, nlMiddle);
	}
	if (startmask)
	{
	    niStart = 32 - (dstx & 0x1f);
	    bitStart = LeftMost;
	    StepBit (bitStart, (dstx & 0x1f));
	}
	if (endmask)
	{
	    niEnd = (dstx + width) & 0x1f;
	    bitEnd = LeftMost;
	}
	if (rop == GXcopy)
	{
	    while (height--)
	    {
	    	psrc = psrcLine;
	    	pdst = pdstLine;
	    	psrcLine += widthSrc;
	    	pdstLine += widthDst;
	    	if (startmask)
	    	{
		    i = niStart;
		    curBit = bitStart;
		    GetBits (psrc, i, curBit, bitPos, bits);

		    *pdst = (*pdst & ~startmask) | bits;
		    pdst++;
	    	}
	    	nl = nlMiddle;
	    	while (nl--)
	    	{
		    i = 32;
		    curBit = LeftMost;
		    GetBits (psrc, i, curBit, bitPos, bits);
		    *pdst++ = bits;
	    	}
	    	if (endmask)
	    	{
		    i = niEnd;
		    curBit = bitEnd;
		    GetBits (psrc, i, curBit, bitPos, bits);

		    *pdst = (*pdst & ~endmask) | bits;
	    	}
	    }
	}
	else
	{
	    while (height--)
	    {
	    	psrc = psrcLine;
	    	pdst = pdstLine;
	    	psrcLine += widthSrc;
	    	pdstLine += widthDst;
	    	if (startmask)
	    	{
		    i = niStart;
		    curBit = bitStart;
		    GetBits (psrc, i, curBit, bitPos, bits);
                    DoRop (result, rop, bits, *pdst);

		    *pdst = (*pdst & ~startmask) | 
			    (result & startmask);
		    pdst++;
	    	}
	    	nl = nlMiddle;
	    	while (nl--)
	    	{
		    i = 32;
		    curBit = LeftMost;
		    GetBits (psrc, i, curBit, bitPos, bits);
                    DoRop (result, rop, bits, *pdst);
		    *pdst = result;
		    ++pdst;
	    	}
	    	if (endmask)
	    	{
		    i = niEnd;
		    curBit = bitEnd;
		    GetBits (psrc, i, curBit, bitPos, bits);
                    DoRop (result, rop, bits, *pdst);

		    *pdst = (*pdst & ~endmask) |
			    (result & endmask);
	    	}
	    }
	}
    }
}

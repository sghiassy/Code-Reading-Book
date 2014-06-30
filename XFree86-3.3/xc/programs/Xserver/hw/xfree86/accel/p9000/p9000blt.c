/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000blt.c,v 3.10 1996/12/23 06:40:36 dawes Exp $ */
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

Author of cfbblt.c: Keith Packard

ERIK NYGREN, KEVIN E. MARTIN, RICKARD E. FAITH, HENRIK HARMSEN AND
TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL ERIK NYGREN, KEVIN E. MARTIN, RICKARD E. FAITH, OR TIAGO
GONS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Further modifications by Tiago Gons (tiago@comosjn.hobby.nl)
Modified for P9000 by Erik Nygren (nygren@mit.edu)
Further modifications by Chris Mason (mason@mail.csh.rit.edu)
Further modifications by Henrik Harmsen (harmsen@eritel.se)
*/
/* $XConsortium: p9000blt.c /main/9 1996/02/21 17:32:06 kaleb $ */

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

#include	"p9000reg.h"
#include	"p9000.h"
#include	"cfb16.h"
#include	"cfb32.h"

/* #define P9000_DEBUG_BLT 1 */

#ifdef P9000_ACCEL

extern RegionPtr cfbBitBlt();

void
p9000FindOrdering(
#if NeedFunctionPrototypes
    DrawablePtr,
    DrawablePtr,
    GC *,
    int,
    BoxPtr,
    int,
    int,
    int,
    int,
    unsigned int *
#endif
);

RegionPtr
p9000CopyArea(pSrcDrawable, pDstDrawable,
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
    register int dx; /* the offset from the dest to the src in screen coords */
    register int dy;
    xRectangle origSource;
    DDXPointRec origDest;
    int numRects;
    BoxRec fastBox;
    int fastClip = 0;		/* for fast clipping with pixmap source */
    int fastExpose = 0;		/* for fast exposures with pixmap source */
    unsigned long engstatus;    /* the status of the drawing engine */

    /* 11-jun-93 TCG : is VT visible */
    /* Are the source and destination in video memory? */
    if (!xf86VTSema
	|| ((pSrcDrawable->type != DRAWABLE_WINDOW) &&
	    (pDstDrawable->type != DRAWABLE_WINDOW))
	/* p9000 has nothing special for this.  Use cfb */
	|| ((pSrcDrawable->type == DRAWABLE_WINDOW) &&
	    (pDstDrawable->type != DRAWABLE_WINDOW))
#ifndef P9000_IM_ACCEL
	/* Ends up calling ImageWrite */
	|| ((pSrcDrawable->type != DRAWABLE_WINDOW) && 
	    (pDstDrawable->type == DRAWABLE_WINDOW))
#endif /* P9000_IM_ACCEL */
	)	
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
                                 srcx, srcy, width, height, dstx, dsty) ;
	  }
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
	(*pSrcDrawable->pScreen->SourceValidate) (pSrcDrawable, srcx, srcy,
						  width, height);
    }

    /* Put srcx and srcy into screen coords */
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
    else /* Source is not a DRAWABLE_PIXMAP */
    {
	if (pGC->subWindowMode == IncludeInferiors)
	{
	    if (!((WindowPtr) pSrcDrawable)->parent) /* Root Window */
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
	else /* subWindowMode == ClipByChildren */
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

    /* Put dstx and dsty into screen coords */
    dstx += pDstDrawable->x;
    dsty += pDstDrawable->y;

    /* Clean up and exit if the destination drawable is a drawable window
     * and isn't realized */
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

    /* Offset from dest to source in screen coords */
    dx = srcx - dstx;
    dy = srcy - dsty;

    /* Translate and clip the dst to the destination composite clip */
    if (fastClip)
    {
	RegionPtr cclip;

        /* Translate the source region directly into the dest region */
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

	if (pSrcDrawable->type == DRAWABLE_WINDOW
	    && pDstDrawable->type == DRAWABLE_WINDOW)
	  {
	    /* Window --> Window */
	    unsigned int *ordering;
	    BoxPtr prect;
	    short direction = 0;
	    
	    ordering = (unsigned int *) ALLOCATE_LOCAL(numRects * sizeof(unsigned int));
	    if(!ordering)
	      {
		DEALLOCATE_LOCAL(ordering);
		return (RegionPtr)NULL;
	      }

	    p9000FindOrdering(pSrcDrawable, pDstDrawable, pGC, numRects,
			      pbox, srcx, srcy, dstx, dsty, ordering);

	    p9000NotBusy();

	    p9000Store(RASTER, CtlBase, p9000alu[pGC->alu]);
	    p9000Store(PMASK, CtlBase, pGC->planemask);

	    for (i = 0; i < numRects; i++)
	      {
		prect = &pbox[ordering[i]];
#ifdef P9000_DEBUG_BLT
  ErrorF("Moving rect: (%d,%d,%d,%d) -> (%d,%d,%d,%d) PM: 0x%x ALU: %d\n", 
          prect->x1+dx, prect->y1+dy, prect->x2+dx, prect->y2+dy-1, 
          prect->x1, prect->y1, prect->x2, prect->y2-1, pGC->planemask, pGC->alu);
#endif
		/* Load the coordinates */
		/* for blits, YX packing makes little difference */
		p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, 
			   CtlBase, prect->x1+dx);
		p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0,
			   CtlBase, prect->y1+dy);
		p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1,
			   CtlBase, prect->x2+dx-1);
		p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1,
			   CtlBase, prect->y2+dy-1);
		p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2,
			   CtlBase, prect->x1);
		p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2,
			   CtlBase, prect->y1);
		p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3,
			   CtlBase, prect->x2-1);
		p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3,
			   CtlBase, prect->y2-1);
		/* wait for engine and blit */
		do engstatus = p9000Fetch(CMD_BLIT, CtlBase);
		while (engstatus & SR_ISSUE_QBN);
	      }
	    DEALLOCATE_LOCAL(ordering);
	    /* wait for the quad/blit engine to finish */
	    p9000QBNotBusy(); 
	}
	else if (pSrcDrawable->type == DRAWABLE_WINDOW
		 && pDstDrawable->type != DRAWABLE_WINDOW)
	  {
	    /* THIS GETS PASSED TO CFB FUNCTIONS ABOVE. NEVER CALLED */
	    /* Window --> Pixmap */
	    int pixWidth = PixmapBytePad(pDstDrawable->width,
					 pDstDrawable->depth);
	    char *pdst = ((PixmapPtr)pDstDrawable)->devPrivate.ptr;
	    
#ifdef P9000_IM_ACCEL
	    for (i = numRects; --i >= 0; pbox++)
	      (p9000ImageReadFunc)(pbox->x1 + dx, pbox->y1 + dy,
				   pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				   pdst, pixWidth,
				   pbox->x1, pbox->y1,
				   pGC->planemask);
#else
	    ErrorF("Don't know how to window->pixmap\n");
#endif	  
	  }
	else if (pSrcDrawable->type != DRAWABLE_WINDOW
		 && pDstDrawable->type == DRAWABLE_WINDOW) 
	  {
	    /* Pixmap --> Window */
	    int pixWidth = PixmapBytePad(pSrcDrawable->width,
					 pSrcDrawable->depth);
	    char *psrc = ((PixmapPtr)pSrcDrawable)->devPrivate.ptr;
	    for (i = numRects; --i >= 0; pbox++)
	      (p9000ImageWriteFunc)(pbox->x1, pbox->y1,
				    pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				    psrc, pixWidth,
				    pbox->x1 + dx, pbox->y1 + dy,
				    pGC->alu, pGC->planemask);
	  } 
	else
	  {
	    /* Pixmap --> Pixmap */
	    ErrorF("p9000CopyArea:  Tried to do a Pixmap to Pixmap copy\n");
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

RegionPtr
p9000CopyArea16(pSrcDrawable, pDstDrawable,
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
    register int dx; /* the offset from the dest to the src in screen coords */
    register int dy;
    register int x1_16, x2_16, y1_16, y2_16, dx_16, dy_16;
    xRectangle origSource;
    DDXPointRec origDest;
    int numRects;
    BoxRec fastBox;
    int fastClip = 0;		/* for fast clipping with pixmap source */
    int fastExpose = 0;		/* for fast exposures with pixmap source */
    unsigned long engstatus;    /* the status of the drawing engine */

    /* 11-jun-93 TCG : is VT visible */
    /* Are the source and destination in video memory? */
    if (!xf86VTSema
	|| ((pSrcDrawable->type != DRAWABLE_WINDOW) && (pDstDrawable->type != DRAWABLE_WINDOW))
	|| ((pSrcDrawable->type == DRAWABLE_WINDOW) && (pDstDrawable->type != DRAWABLE_WINDOW))
	|| ((pSrcDrawable->type != DRAWABLE_WINDOW) && (pDstDrawable->type == DRAWABLE_WINDOW))
        || (pGC->planemask != ~0) /* only all planes involved allowed */
        || (pGC->alu != GXcopy) /* only COPY operations allowed */
	)	
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
                                 srcx, srcy, width, height, dstx, dsty) ;
	  }
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
	(*pSrcDrawable->pScreen->SourceValidate) (pSrcDrawable, srcx, srcy,
						  width, height);
    }

    /* Put srcx and srcy into screen coords */
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
    else /* Source is not a DRAWABLE_PIXMAP */
    {
	if (pGC->subWindowMode == IncludeInferiors)
	{
	    if (!((WindowPtr) pSrcDrawable)->parent) /* Root Window */
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
	else /* subWindowMode == ClipByChildren */
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

    /* Put dstx and dsty into screen coords */
    dstx += pDstDrawable->x;
    dsty += pDstDrawable->y;

    /* Clean up and exit if the destination drawable is a drawable window
     * and isn't realized */
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

    /* Offset from dest to source in screen coords */
    dx = srcx - dstx;
    dy = srcy - dsty;

    /* Translate and clip the dst to the destination composite clip */
    if (fastClip)
    {
	RegionPtr cclip;

        /* Translate the source region directly into the dest region */
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

	if (pSrcDrawable->type == DRAWABLE_WINDOW
	    && pDstDrawable->type == DRAWABLE_WINDOW)
	  {
	    /* Window --> Window */
	    unsigned int *ordering;
	    BoxPtr prect;
	    short direction = 0;
	    
	    ordering = (unsigned int *) ALLOCATE_LOCAL(numRects * sizeof(unsigned int));
	    if(!ordering)
	      {
		DEALLOCATE_LOCAL(ordering);
		return (RegionPtr)NULL;
	      }

	    p9000FindOrdering(pSrcDrawable, pDstDrawable, pGC, numRects,
			      pbox, srcx, srcy, dstx, dsty, ordering);

	    p9000NotBusy();

	    p9000Store(RASTER, CtlBase, p9000alu[pGC->alu]);
	    p9000Store(PMASK, CtlBase, pGC->planemask);

	    for (i = 0; i < numRects; i++)
	      {
		prect = &pbox[ordering[i]];
#ifdef P9000_DEBUG_BLT
		ErrorF("Moving rect: (%d,%d,%d,%d) -> (%d,%d,%d,%d) PM: 0x%x ALU: %d\n", prect->x1+dx, prect->y1+dy, prect->x2+dx, prect->y2+dy-1, prect->x1, prect->y1, prect->x2, prect->y2-1, pGC->planemask, pGC->alu);
#endif
		/* Load the coordinates */
		/* for blits, YX packing makes little difference */
		x1_16 = (prect->x1) << 1;
		x2_16 = (prect->x2) << 1;
		y1_16 = (prect->y1);
		y2_16 = (prect->y2);
		dx_16 = dx << 1;
		dy_16 = dy;
		p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, 
			   CtlBase, x1_16 + dx_16);
		p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0,
			   CtlBase, y1_16 + dy_16);
		p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1,
			   CtlBase, x2_16 + dx_16 - 1);
		p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1,
			   CtlBase, y2_16 + dy_16 - 1);
		p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2,
			   CtlBase, x1_16);
		p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2,
			   CtlBase, y1_16);
		p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3,
			   CtlBase, x2_16 - 1);
		p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3,
			   CtlBase, y2_16 - 1);
		/* wait for engine and blit */
		do engstatus = p9000Fetch(CMD_BLIT, CtlBase);
		while (engstatus & SR_ISSUE_QBN);
	      }
	    DEALLOCATE_LOCAL(ordering);
	    /* wait for the quad/blit engine to finish */
	    p9000QBNotBusy(); 
	}
	else if (pSrcDrawable->type == DRAWABLE_WINDOW
		 && pDstDrawable->type != DRAWABLE_WINDOW)
	  {
	    /* THIS GETS PASSED TO CFB FUNCTIONS ABOVE. NEVER CALLED */
	    /* Window --> Pixmap */
	    int pixWidth = PixmapBytePad(pDstDrawable->width,
					 pDstDrawable->depth);
	    char *pdst = ((PixmapPtr)pDstDrawable)->devPrivate.ptr;
	    
#ifdef P9000_IM_ACCEL
	    for (i = numRects; --i >= 0; pbox++)
	      (p9000ImageReadFunc)(pbox->x1 + dx, pbox->y1 + dy,
				   pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				   pdst, pixWidth,
				   pbox->x1, pbox->y1,
				   pGC->planemask);
#else
	    ErrorF("Don't know how to window->pixmap\n");
#endif	  
	  }
	else if (pSrcDrawable->type != DRAWABLE_WINDOW
		 && pDstDrawable->type == DRAWABLE_WINDOW) 
	  {
	    /* Pixmap --> Window */
	    int pixWidth = PixmapBytePad(pSrcDrawable->width,
					 pSrcDrawable->depth);
	    char *psrc = ((PixmapPtr)pSrcDrawable)->devPrivate.ptr;
	    for (i = numRects; --i >= 0; pbox++)
	      (p9000ImageWriteFunc)(pbox->x1, pbox->y1,
				    pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				    psrc, pixWidth,
				    pbox->x1 + dx, pbox->y1 + dy,
				    pGC->alu, pGC->planemask);
	  } 
	else
	  {
	    /* Pixmap --> Pixmap */
	    ErrorF("p9000CopyArea:  Tried to do a Pixmap to Pixmap copy\n");
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

RegionPtr
p9000CopyArea32(pSrcDrawable, pDstDrawable,
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
    register int dx; /* the offset from the dest to the src in screen coords */
    register int dy;
    register int x1_32, x2_32, y1_32, y2_32, dx_32, dy_32;
    xRectangle origSource;
    DDXPointRec origDest;
    int numRects;
    BoxRec fastBox;
    int fastClip = 0;		/* for fast clipping with pixmap source */
    int fastExpose = 0;		/* for fast exposures with pixmap source */
    unsigned long engstatus;    /* the status of the drawing engine */

    /* 11-jun-93 TCG : is VT visible */
    /* Are the source and destination in video memory? */
    if (!xf86VTSema
	|| ((pSrcDrawable->type != DRAWABLE_WINDOW) && (pDstDrawable->type != DRAWABLE_WINDOW))
	|| ((pSrcDrawable->type == DRAWABLE_WINDOW) && (pDstDrawable->type != DRAWABLE_WINDOW))
	|| ((pSrcDrawable->type != DRAWABLE_WINDOW) && (pDstDrawable->type == DRAWABLE_WINDOW))
        || (pGC->planemask != ~0) /* only all planes involved allowed */
        || (pGC->alu != GXcopy) /* only COPY operations allowed */
	)	
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
                                 srcx, srcy, width, height, dstx, dsty) ;
	  }
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
	(*pSrcDrawable->pScreen->SourceValidate) (pSrcDrawable, srcx, srcy,
						  width, height);
    }

    /* Put srcx and srcy into screen coords */
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
    else /* Source is not a DRAWABLE_PIXMAP */
    {
	if (pGC->subWindowMode == IncludeInferiors)
	{
	    if (!((WindowPtr) pSrcDrawable)->parent) /* Root Window */
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
	else /* subWindowMode == ClipByChildren */
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

    /* Put dstx and dsty into screen coords */
    dstx += pDstDrawable->x;
    dsty += pDstDrawable->y;

    /* Clean up and exit if the destination drawable is a drawable window
     * and isn't realized */
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

    /* Offset from dest to source in screen coords */
    dx = srcx - dstx;
    dy = srcy - dsty;

    /* Translate and clip the dst to the destination composite clip */
    if (fastClip)
    {
	RegionPtr cclip;

        /* Translate the source region directly into the dest region */
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

	if (pSrcDrawable->type == DRAWABLE_WINDOW
	    && pDstDrawable->type == DRAWABLE_WINDOW)
	  {
	    /* Window --> Window */
	    unsigned int *ordering;
	    BoxPtr prect;
	    short direction = 0;
	    
	    ordering = (unsigned int *) ALLOCATE_LOCAL(numRects * sizeof(unsigned int));
	    if(!ordering)
	      {
		DEALLOCATE_LOCAL(ordering);
		return (RegionPtr)NULL;
	      }

	    p9000FindOrdering(pSrcDrawable, pDstDrawable, pGC, numRects,
			      pbox, srcx, srcy, dstx, dsty, ordering);

	    p9000NotBusy();

	    p9000Store(RASTER, CtlBase, p9000alu[pGC->alu]);
	    p9000Store(PMASK, CtlBase, pGC->planemask);

	    for (i = 0; i < numRects; i++)
	      {
		prect = &pbox[ordering[i]];
#ifdef P9000_DEBUG_BLT
		ErrorF("Moving rect: (%d,%d,%d,%d) -> (%d,%d,%d,%d) PM: 0x%x ALU: %d\n", prect->x1+dx, prect->y1+dy, prect->x2+dx, prect->y2+dy-1, prect->x1, prect->y1, prect->x2, prect->y2-1, pGC->planemask, pGC->alu);
#endif
		/* Load the coordinates */
		/* for blits, YX packing makes little difference */
		x1_32 = (prect->x1) << 2;
		x2_32 = (prect->x2) << 2;
		y1_32 = (prect->y1);
		y2_32 = (prect->y2);
		dx_32 = dx << 2;
		dy_32 = dy;
		p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, 
			   CtlBase, x1_32 + dx_32);
		p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0,
			   CtlBase, y1_32 + dy_32);
		p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1,
			   CtlBase, x2_32 + dx_32 - 1);
		p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1,
			   CtlBase, y2_32 + dy_32 - 1);
		p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2,
			   CtlBase, x1_32);
		p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2,
			   CtlBase, y1_32);
		p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3,
			   CtlBase, x2_32 - 1);
		p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3,
			   CtlBase, y2_32 - 1);
		/* wait for engine and blit */
		do engstatus = p9000Fetch(CMD_BLIT, CtlBase);
		while (engstatus & SR_ISSUE_QBN);
	      }
	    DEALLOCATE_LOCAL(ordering);
	    /* wait for the quad/blit engine to finish */
	    p9000QBNotBusy(); 
	}
	else if (pSrcDrawable->type == DRAWABLE_WINDOW
		 && pDstDrawable->type != DRAWABLE_WINDOW)
	  {
	    /* THIS GETS PASSED TO CFB FUNCTIONS ABOVE. NEVER CALLED */
	    /* Window --> Pixmap */
	    int pixWidth = PixmapBytePad(pDstDrawable->width,
					 pDstDrawable->depth);
	    char *pdst = ((PixmapPtr)pDstDrawable)->devPrivate.ptr;
	    
#ifdef P9000_IM_ACCEL
	    for (i = numRects; --i >= 0; pbox++)
	      (p9000ImageReadFunc)(pbox->x1 + dx, pbox->y1 + dy,
				   pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				   pdst, pixWidth,
				   pbox->x1, pbox->y1,
				   pGC->planemask);
#else
	    ErrorF("Don't know how to window->pixmap\n");
#endif	  
	  }
	else if (pSrcDrawable->type != DRAWABLE_WINDOW
		 && pDstDrawable->type == DRAWABLE_WINDOW) 
	  {
	    /* Pixmap --> Window */
	    int pixWidth = PixmapBytePad(pSrcDrawable->width,
					 pSrcDrawable->depth);
	    char *psrc = ((PixmapPtr)pSrcDrawable)->devPrivate.ptr;
	    for (i = numRects; --i >= 0; pbox++)
	      (p9000ImageWriteFunc)(pbox->x1, pbox->y1,
				    pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				    psrc, pixWidth,
				    pbox->x1 + dx, pbox->y1 + dy,
				    pGC->alu, pGC->planemask);
	  } 
	else
	  {
	    /* Pixmap --> Pixmap */
	    ErrorF("p9000CopyArea:  Tried to do a Pixmap to Pixmap copy\n");
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
p9000FindOrdering(pSrcDrawable, pDstDrawable, pGC, numRects, boxes, srcx, srcy, dstx, dsty, ordering)
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

/* still not quite right. - CLM 5/22/95 */
RegionPtr
p9000CopyPlane(pSrcDrawable, pDstDrawable,
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
       ! (OnScreenDrawable(pDstDrawable->type) && 
          OnScreenDrawable(pSrcDrawable->type) )
       )
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
 	p9000CopyArea(pSrcDrawable, (DrawablePtr)pPixmap, pGC1, srcx, srcy,
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
         p9000FindOrdering(pSrcDrawable, pDstDrawable, pGC, numRects, pbox, 
                           srcx, srcy, dstx, dsty, ordering);

	 p9000NotBusy();

	 p9000Store(RASTER, CtlBase, p9000alu[pGC->alu]);
	 p9000Store(PMASK, CtlBase, pGC->planemask);

         for (i = 0; i < numRects; i++) {
	     prect = &pbox[ordering[i]];
#ifdef P9000_DEBUG_BLT
  ErrorF("Moving rect: (%d,%d,%d,%d) -> (%d,%d,%d,%d) PM: 0x%x ALU: %d\n", 
          prect->x1+dx, prect->y1+dy, prect->x2+dx, prect->y2+dy-1, 
          prect->x1, prect->y1, prect->x2, prect->y2-1, pGC->planemask, pGC->alu);
#endif
	      /* Load the coordinates */
	      /* for blits, YX packing makes little difference */
	      p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, 
			   CtlBase, prect->x1+dx);
	      p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0,
			   CtlBase, prect->y1+dy);
	      p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1,
			   CtlBase, prect->x2+dx-1);
	      p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1,
			   CtlBase, prect->y2+dy-1);
	      p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2,
			   CtlBase, prect->x1);
	      p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2,
			   CtlBase, prect->y1);
	      p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3,
			   CtlBase, prect->x2-1);
	      p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3,
			   CtlBase, prect->y2-1);
		/* wait for engine and blit */
	      while(p9000Fetch(CMD_BLIT, CtlBase) & SR_ISSUE_QBN) ;
         }
	 DEALLOCATE_LOCAL(ordering);
	 /* wait for the quad/blit engine to finish */
	 p9000QBNotBusy(); 

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
            p9000ImageOpStipple(pbox->x1, pbox->y1,
				pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				psrc, pixWidth,
				pix->drawable.width, pix->drawable.height,
				-dx, -dy, pGC->fgPixel, pGC->bgPixel,
				p9000alu[pGC->alu],
				(short) pGC->planemask);
         }
      } else {
         /* Pixmap --> Pixmap */
         ErrorF("p9000CopyPlane:  Tried to do a Pixmap to Pixmap copy\n");
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

#endif /* P9000_ACCEL */

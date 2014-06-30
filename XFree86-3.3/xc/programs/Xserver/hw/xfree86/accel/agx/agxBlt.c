/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxBlt.c,v 3.17 1996/12/23 06:32:31 dawes Exp $ */
/*
Copyright 1989 by the Massachusetts Institute of Technology
Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
Copyright 1994 by Henry A. Worth,  Sunnyvale, California.      

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of M.I.T. not be used in
advertising or publicity pertaining to distribution of the software
without specific, written prior permission.  M.I.T. makes no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

Author: Keith Packard

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN, RICKARD E. FAITH, AND HENRY A. WORTH DISCLAIM ALL 
WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL 
THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA 
OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
USE OR PERFORMANCE OF THIS SOFTWARE.

Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
Modified for the AGX    by Henry A. Worth  (haw30@eng.amdahl.com)

 */
/* $XConsortium: agxBlt.c /main/7 1996/02/21 17:16:37 kaleb $ */

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
#include	"cfb16.h"
#ifdef AGX_32BPP
#include	"cfb32.h"
#endif
#include	"fastblt.h"

#include	"regagx.h"
#include	"agx.h"

void agxFindOrdering();
#if 0
extern RegionPtr cfbBitBlt();
#endif

RegionPtr
agxCopyArea(pSrcDrawable, pDstDrawable,
		pGC, srcx, srcy, width, height, dstx, dsty)
    register DrawablePtr pSrcDrawable;
    register DrawablePtr pDstDrawable;
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

    if ( !xf86VTSema 
         || (pSrcDrawable->type != DRAWABLE_WINDOW 
             && pDstDrawable->type != DRAWABLE_WINDOW) ) {
        switch (max(pSrcDrawable->bitsPerPixel, pDstDrawable->bitsPerPixel)) {
           case 8:
               return cfbCopyArea(pSrcDrawable, pDstDrawable, pGC,
                                  srcx, srcy, width, height, dstx, dsty);
           case 16:
               return cfb16CopyArea(pSrcDrawable, pDstDrawable, pGC,
                                    srcx, srcy, width, height, dstx, dsty);
#ifdef AGX_32BPP
           case 32:
               return cfb32CopyArea(pSrcDrawable, pDstDrawable, pGC,
                                    srcx, srcy, width, height, dstx, dsty);
#endif
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
	(*pSrcDrawable->pScreen->SourceValidate) ( pSrcDrawable, 
                                                   srcx, srcy, width, height);
    }

    srcx += pSrcDrawable->x;
    srcy += pSrcDrawable->y;

    /* clip the source */

    if (pSrcDrawable->type == DRAWABLE_PIXMAP)
    {
	if ((pSrcDrawable == pDstDrawable) &&
	    (pGC->clientClipType == CT_NONE))
	{
	    prgnSrcClip = ((cfbPrivGC *)
                             (pGC->devPrivates[cfbGCPrivateIndex].ptr))
                                 ->pCompositeClip;
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
	    else if ( (pSrcDrawable == pDstDrawable)
		      && (pGC->clientClipType == CT_NONE) )
	    {
		prgnSrcClip = ((cfbPrivGC *)
                                 (pGC->devPrivates[cfbGCPrivateIndex].ptr))
                                     ->pCompositeClip;
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

	cclip = ((cfbPrivGC *)
                    (pGC->devPrivates[cfbGCPrivateIndex].ptr))
                        ->pCompositeClip;

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

    if (!fastClip) {
	(*pGC->pScreen->Intersect)( 
               &rgnDst,
	       &rgnDst,
	       ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))
                    ->pCompositeClip
               );
    }

    /* Do bit blitting */
    numRects = REGION_NUM_RECTS(&rgnDst);
    if (numRects && width && height)
    {
	pbox = REGION_RECTS(&rgnDst);

	if ( pSrcDrawable->type == DRAWABLE_WINDOW 
             && pDstDrawable->type == DRAWABLE_WINDOW ) {
	    /* Window --> Window */
            BoxPtr prect;
	    unsigned int *ordering;
	    short direction = 0;
        
	    ordering = (unsigned int *)
                           ALLOCATE_LOCAL( numRects * sizeof(unsigned int) );
	    if(!ordering) {
		DEALLOCATE_LOCAL(ordering);
		return (RegionPtr)NULL;
	    }

	    agxFindOrdering( pSrcDrawable, pDstDrawable, pGC, 
                             numRects, pbox, 
                             srcx, srcy, dstx, dsty, 
                             ordering );

            i = 0;
            prect = &pbox[ordering[0]];
            if ( dx > 0 && dy > 0 ) {
               register unsigned int srcCoOrd = (prect->y1+dy) << 16
                                                | (prect->x1+dx);
               register unsigned int dstCoOrd = prect->y1 << 16 | prect->x1;
               register unsigned int opDim = (prect->y2-prect->y1-1) << 16
                                             | (prect->x2-prect->x1-1);
               MAP_SET_SRC_AND_DST( GE_MS_MAP_A ); 
	       GE_WAIT_IDLE();
               GE_OUT_B(GE_FRGD_MIX, pGC->alu);
	       GE_OUT_D(GE_PIXEL_BIT_MASK, pGC->planemask);
               for(;;) {
	          GE_OUT_D( GE_SRC_MAP_X, srcCoOrd );
	          GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
                  GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
                  GE_START_CMD( GE_OP_BITBLT
                                | GE_OP_FRGD_SRC_MAP 
                                | GE_OP_SRC_MAP_A
                                | GE_OP_DEST_MAP_A  
                                | GE_OP_PAT_FRGD
                                | GE_OP_MASK_DISABLED
                                | GE_OP_INC_X
                                | GE_OP_INC_Y         );
                  if (++i >= numRects )
                     break;
                  prect = &pbox[ordering[i]];
                  srcCoOrd = (prect->y1+dy) << 16 | (prect->x1+dx);
                  dstCoOrd = prect->y1 << 16 | prect->x1;
                  opDim = (prect->y2-prect->y1-1) << 16
                          | (prect->x2-prect->x1-1);
	          GE_WAIT_IDLE();
               }
            }
            else if ( dx > 0 ) {
               register unsigned int srcCoOrd = (prect->y2+dy-1) << 16
                                                | (prect->x1+dx);
               register unsigned int dstCoOrd = (prect->y2-1) << 16 
                                                | prect->x1;
               register unsigned int opDim = (prect->y2-prect->y1-1) << 16
                                             | (prect->x2-prect->x1-1);
               MAP_SET_SRC_AND_DST( GE_MS_MAP_A ); 
	       GE_WAIT_IDLE();
               GE_OUT_B(GE_FRGD_MIX, pGC->alu);
	       GE_OUT_D(GE_PIXEL_BIT_MASK, pGC->planemask);
               for(;;){
	          GE_OUT_D( GE_SRC_MAP_X, srcCoOrd );
	          GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
                  GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
                  GE_START_CMD( GE_OP_BITBLT
                                | GE_OP_FRGD_SRC_MAP 
                                | GE_OP_SRC_MAP_A
                                | GE_OP_DEST_MAP_A 
                                | GE_OP_PAT_FRGD
                                | GE_OP_MASK_DISABLED
                                | GE_OP_INC_X
                                | GE_OP_DEC_Y         );
                  if (++i >= numRects )
                     break;
                  prect = &pbox[ordering[i]];
                  srcCoOrd = (prect->y2+dy-1) << 16 | (prect->x1+dx);
                  dstCoOrd = (prect->y2-1) << 16 | prect->x1;
                  opDim = (prect->y2-prect->y1-1) << 16
                          | (prect->x2-prect->x1-1);
	          GE_WAIT_IDLE();
               }
            }
            else if ( dy > 0 ) {
               register unsigned int srcCoOrd = (prect->y1+dy) << 16
                                                | (prect->x2+dx-1);
               register unsigned int dstCoOrd = prect->y1 << 16 
                                                | (prect->x2-1);
               register unsigned int opDim = (prect->y2-prect->y1-1) << 16
                                             | (prect->x2-prect->x1-1);
               MAP_SET_SRC_AND_DST( GE_MS_MAP_A );
	       GE_WAIT_IDLE();
               GE_OUT_B(GE_FRGD_MIX, pGC->alu);
               GE_OUT_D(GE_PIXEL_BIT_MASK, pGC->planemask);
               for(;;) { 
	          GE_OUT_D( GE_SRC_MAP_X, srcCoOrd );
	          GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
                  GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
                  GE_START_CMD( GE_OP_BITBLT
                                | GE_OP_FRGD_SRC_MAP
                                | GE_OP_SRC_MAP_A
                                | GE_OP_DEST_MAP_A
                                | GE_OP_PAT_FRGD
                                | GE_OP_MASK_DISABLED
                                | GE_OP_DEC_X
                                | GE_OP_INC_Y         );
                  if (++i >= numRects )
                     break;
                  prect = &pbox[ordering[i]];
                  srcCoOrd = (prect->y1+dy) << 16 | (prect->x2+dx-1);
                  dstCoOrd = prect->y1 << 16 | (prect->x2-1);
                  opDim = (prect->y2-prect->y1-1) << 16
                          | (prect->x2-prect->x1-1);
	          GE_WAIT_IDLE();
               } 
            }
            else {
               register unsigned int srcCoOrd = (prect->y2+dy-1) << 16
                                                | (prect->x2+dx-1);
               register unsigned int dstCoOrd = prect->y2-1 << 16 
                                                | (prect->x2-1);
               register unsigned int opDim = (prect->y2-prect->y1-1) << 16
                                             | (prect->x2-prect->x1-1);
               MAP_SET_SRC_AND_DST( GE_MS_MAP_A );
               GE_WAIT_IDLE();
               GE_OUT_B(GE_FRGD_MIX, pGC->alu);
               GE_OUT_D(GE_PIXEL_BIT_MASK, pGC->planemask);
               for(;;) { 
	          GE_OUT_D( GE_SRC_MAP_X, srcCoOrd );
	          GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
                  GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
                  GE_START_CMD( GE_OP_BITBLT
                                | GE_OP_FRGD_SRC_MAP
                                | GE_OP_SRC_MAP_A
                                | GE_OP_DEST_MAP_A
                                | GE_OP_PAT_FRGD
                                | GE_OP_MASK_DISABLED
                                | GE_OP_DEC_X
                                | GE_OP_DEC_Y         );
                  if (++i >= numRects )
                     break;
                  prect = &pbox[ordering[i]];
                  srcCoOrd = (prect->y2+dy-1) << 16 | (prect->x2+dx-1);
                  dstCoOrd = prect->y2-1 << 16 | (prect->x2-1);
                  opDim = (prect->y2-prect->y1-1) << 16
                          | (prect->x2-prect->x1-1);
	          GE_WAIT_IDLE();
               } 
            }

            DEALLOCATE_LOCAL(ordering);
	    GE_WAIT_IDLE_EXIT();

	}
        else if ( pSrcDrawable->type == DRAWABLE_WINDOW 
                  && pDstDrawable->type != DRAWABLE_WINDOW ) {

	    /* Window --> Pixmap */
	    int pixWidth = PixmapBytePad(pDstDrawable->width,
					 pDstDrawable->depth);
	    char *pdst = ((PixmapPtr)pDstDrawable)->devPrivate.ptr;

	    for (i = numRects; --i >= 0; pbox++)
		(agxImageReadFunc)( pbox->x1 + dx, pbox->y1 + dy,
				    pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				    pdst, pixWidth, 
                                    pbox->x1, pbox->y1, 
				    pGC->planemask );
	}
        else if ( pSrcDrawable->type != DRAWABLE_WINDOW 
                  && pDstDrawable->type == DRAWABLE_WINDOW ) {
	    /* Pixmap --> Window */
#if 1
	    int pixWidth = PixmapBytePad(pSrcDrawable->width,
					 pSrcDrawable->depth);
	    char *psrc = ((PixmapPtr)pSrcDrawable)->devPrivate.ptr;

	    for (i = numRects; --i >= 0; pbox++)
		(agxImageWriteFunc)( pbox->x1, pbox->y1,
				     pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
				     psrc, pixWidth, 
                                     pbox->x1 + dx, pbox->y1 + dy,
				     pGC->alu, pGC->planemask );
#else
            agxFillBoxTile( pDstDrawable, numRects, pbox,
                            (PixmapPtr) pSrcDrawable,
                            -dx, -dy,
                            pGC->alu,
                            pGC->planemask );
#endif
	} 
        else {
	    /* Pixmap --> Pixmap */
	    ErrorF("agxCopyArea:  Tried to do a Pixmap to Pixmap copy\n");
	}
    }

    prgnExposed = NULL;
    if (((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->fExpose)
    {
	extern RegionPtr    miHandleExposures();

        /* Pixmap sources generate a NoExposed (we return NULL to do this) */
        if (!fastExpose)
	    prgnExposed =
		miHandleExposures( pSrcDrawable, pDstDrawable, pGC,
				   origSource.x, origSource.y,
				   (int)origSource.width,
				   (int)origSource.height,
				   origDest.x, origDest.y, 0 );
    }
    (*pGC->pScreen->RegionUninit)(&rgnDst);
    if (freeSrcClip)
	(*pGC->pScreen->RegionDestroy)(prgnSrcClip);
    return prgnExposed;
}

void
agxFindOrdering( pSrcDrawable, pDstDrawable, pGC, numRects, boxes, 
                 srcx, srcy, dstx, dsty, ordering )
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

    /*
     *  If not the same drawable then order of move doesn't matter.
     *  Following assumes that boxes are sorted from top
     *  to bottom and left to right.
     */
    if ( (pSrcDrawable != pDstDrawable) 
         && ( (pGC->subWindowMode != IncludeInferiors)
              || (pSrcDrawable->type == DRAWABLE_PIXMAP)
              || (pDstDrawable->type == DRAWABLE_PIXMAP) ) ) {
       for (i=0; i < numRects; i++)
	  ordering[i] = i;
    }
    else { 
       /* within same drawable, must sequence moves carefully! */
       if (dsty <= srcy) { 
          /* Scroll up or stationary vertical.  Vertical order OK */
	  if (dstx <= srcx) { 
             /* 
              * Scroll left or stationary horizontal.  
              * Horizontal order OK as well
              */
             for (i=0; i < numRects; i++)
		ordering[i] = i;
	  }
          else { 
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
       }
       else {
          /* Scroll down. Must reverse vertical banding. */
          if (dstx < srcx) { 
             /* Scroll left. Horizontal order OK. */
   	  for ( i=numRects-1, j=i-1, yMin=i, yMax=0; 
                   i >= 0; 
                   j=i-1, yMin=i ) {
   	     /* find extent of current horizontal band */
   	     y=boxes[i].y1; /* band has this y coordinate */
          	     while ((j >= 0) && (boxes[j].y1 == y))
   		j--;
   	     /* reverse the horizontal band in the output ordering */
   	     for (j++ ; j <= yMin; j++, i--, yMax++)
   			ordering[yMax] = j;
              }
          }
          else { 
             /* 
              * Scroll right or horizontal stationary.
              * Reverse horizontal order as well (if stationary, horizontal
              * order can be swapped without penalty and this is faster
   	      * to compute).
              */
              for (i=0, j=numRects-1; i < numRects; i++, j--)
   	         ordering[i] = j;
          }
      }
   }
}

RegionPtr
agxCopyPlane(pSrcDrawable, pDstDrawable,
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

    if ( !xf86VTSema 
         || ( (pSrcDrawable->type != DRAWABLE_WINDOW)
              && (pDstDrawable->type != DRAWABLE_WINDOW) ) ) {
        switch (max(pSrcDrawable->bitsPerPixel, pDstDrawable->bitsPerPixel)) {
           case 8:
             return cfbCopyPlane(pSrcDrawable, pDstDrawable, pGC,
                                 srcx, srcy, width, height, 
                                 dstx, dsty, bitPlane);
           case 16:
             return cfb16CopyPlane(pSrcDrawable, pDstDrawable, pGC,
                                   srcx, srcy, width, height, 
                                   dstx, dsty, bitPlane);
#ifdef AGX_32BPP
           case 32:
              return cfb32CopyPlane(pSrcDrawable, pDstDrawable, pGC,
                                    srcx, srcy, width, height, 
                                    dstx, dsty, bitPlane);
#endif
        }
    }

    if( (pSrcDrawable->type != DRAWABLE_WINDOW) 
	&& (pSrcDrawable->bitsPerPixel == 8)    ) {
	 /*
	  * Shortcut - we can do Pixmap->Window when the source depth is
	  * 8 by using the handy-dandy cfbCopyPlane8to1 to create a bitmap
	  * for us to use.
	  */
	GCPtr pGC1;
        extern void cfbCopyPlane8to1();

	pBitmap=(*pSrcDrawable->pScreen->CreatePixmap)(pSrcDrawable->pScreen, 
						       width, height, 1);
	if (!pBitmap)
	    return(NULL);
	pGC1 = GetScratchGC(1, pSrcDrawable->pScreen);
	if (!pGC1) {
	    (*pSrcDrawable->pScreen->DestroyPixmap)(pBitmap);
	    return(NULL);
	}
	ValidateGC((DrawablePtr)pBitmap, pGC1);
	(void)  cfbBitBlt(pSrcDrawable, (DrawablePtr)pBitmap, pGC1, srcx, srcy,
                         width, height, srcx, srcy, cfbCopyPlane8to1, bitPlane);
        FreeScratchGC(pGC1);
        pSrcDrawable = (DrawablePtr)pBitmap;
    }
    else if (pSrcDrawable->type == DRAWABLE_WINDOW) {
        /*
         * The AGX doesn't support a planemask the source. In the
         * future might do a two step blt thru the scratchpad. For
         * now we'll just create a pixmap to use for the stipple
         * (this also includes the non-drawable window case which
         * simplifies the common case below).
         *
         * Shortcut - we can do Window->Pixmap by copying the window to
         * a pixmap, then we have a Pixmap->Pixmap operation that can be
         * handled by the above Pixmap->Bitmap case for 8bpp or micopyplane
         * for others.
         */
        GCPtr pGC1;
        RegionPtr retval;
        PixmapPtr pPixmap;

        pPixmap=(*pSrcDrawable->pScreen->CreatePixmap)(
                    pSrcDrawable->pScreen,
                    width, height,
                    pSrcDrawable->bitsPerPixel
                );
        if (!pPixmap)
            return(NULL);
        pGC1 = GetScratchGC( pSrcDrawable->bitsPerPixel,
                             pSrcDrawable->pScreen       );
        if (!pGC1) {
            (*pSrcDrawable->pScreen->DestroyPixmap)(pPixmap);
            return(NULL);
        }
        ValidateGC((DrawablePtr)pPixmap, pGC1);
        agxCopyArea( pSrcDrawable, (DrawablePtr)pPixmap, pGC1,
                     srcx, srcy, width, height, 0, 0);
        retval = agxCopyPlane((DrawablePtr)pPixmap, pDstDrawable, pGC,
                              0, 0, width, height, dstx, dsty, bitPlane);
        FreeScratchGC(pGC1);
        (*pSrcDrawable->pScreen->DestroyPixmap)(pPixmap);
        return(retval);
    } 
    else if ( pSrcDrawable->bitsPerPixel != 1 ) {
       /*
        * Have to punt to micopyplane -
        *  In future need to look into creating a usable bitmap.
        */
	return (RegionPtr)miCopyPlane( pSrcDrawable, pDstDrawable, 
                                       pGC, srcx, srcy, 
                                       width, height, 
                                       dstx, dsty, bitPlane);
    }

   /* 
    * At this point the source pixmap is know to have a depth of 1.
    */

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

      if ( pSrcDrawable->type != DRAWABLE_WINDOW
                && pDstDrawable->type == DRAWABLE_WINDOW ) {
#if 0
         /* Pixmap --> Window */
         PixmapPtr pix = (PixmapPtr) pSrcDrawable;
         int   pixWidth;
         unsigned char *psrc;

         pixWidth = PixmapBytePad(pSrcDrawable->width, pSrcDrawable->depth);
         psrc = pix->devPrivate.ptr;

         for (i = numRects; --i >= 0; pbox++) {
            agxImageStipple( pbox->x1, pbox->y1,
                             pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
                             psrc, pixWidth,
                             pix->drawable.width, pix->drawable.height,
                             -dx, -dy,
                             pGC->fgPixel, pGC->bgPixel,
                             pGC->alu, pGC->alu,
                             pGC->planemask );
         }
#else
         agxFillBoxStipple( pDstDrawable, numRects, pbox,
                            (PixmapPtr) pSrcDrawable,
                            -dx, -dy,
                             pGC->fgPixel, pGC->bgPixel,
                             pGC->alu, pGC->alu,
                             pGC->planemask );
#endif
      }
#if 0     /* not supported */
      else if ( pSrcDrawable->type == DRAWABLE_WINDOW
                && pDstDrawable->type == DRAWABLE_WINDOW ) {

         /* Window --> Window , src Window depth of 1 */

         unsigned int *ordering;
         short direction = 0;

         ordering = (unsigned int *)
                       ALLOCATE_LOCAL( numRects * sizeof(unsigned int) );
         if (!ordering) {
            DEALLOCATE_LOCAL(ordering);
            return (RegionPtr) NULL;
         }

         agxFindOrdering( pSrcDrawable, pDstDrawable, 
                          pGC, numRects, pbox, 
                          srcx, srcy, dstx, dsty, 
                          ordering );

         GE_WAIT_IDLE();

         MAP_SET_DST( GE_MS_MAP_A );
         GE_OUT_W(GE_FRGD_MIX, pGC->alu << 8 | pGC->alu);
         GE_OUT_D(GE_PIXEL_BIT_MASK, pGC->planemask);
         GE_OUT_D(GE_FRGD_CLR, pGC->fgPixel);
         GE_OUT_D(GE_BKGD_CLR, pGC->bgPixel);

         if ( dx > 0 && dy > 0 ) {
            GE_OUT_W( GE_PIXEL_OP, GE_OP_PAT_MAP_A
                                   | GE_OP_MASK_DISABLED
                                   | GE_OP_INC_X
                                   | GE_OP_INC_Y         );
            for (i = 0; i < numRects; i++) {
               BoxPtr prect = &pbox[ordering[i]];
               register unsigned int patCoOrd = (prect->y1+dy) << 16
                                                | (prect->x1+dx);
               register unsigned int dstCoOrd = prect->y1 << 16 | prect->x1;
               register unsigned int opDim = (prect->y2-prect->y1-1) << 16
                                             | (prect->x2-prect->x1-1);
               GE_WAIT_IDLE();
               GE_OUT_D( GE_PAT_MAP_X, patCoOrd );
               GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
               GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
               GE_START_CMDW( GE_OPW_BITBLT
                              | GE_OPW_FRGD_SRC_CLR 
                              | GE_OPW_BKGD_SRC_CLR 
                              | GE_OPW_DEST_MAP_A   );
            }
         } 
         else if ( dx > 0 ) {
            GE_OUT_W( GE_PIXEL_OP+2, GE_OP_PAT_MAP_A
                                     | GE_OP_MASK_DISABLED
                                     | GE_OP_INC_X
                                     | GE_OP_DEC_Y         );
            for (i = 0; i < numRects; i++) {
               BoxPtr prect = &pbox[ordering[i]];
               register unsigned int patCoOrd = (prect->y2+dy-1) << 16
                                                | (prect->x1+dx);
               register unsigned int dstCoOrd = (prect->y2-1) << 16 
                                                | prect->x1;
               register unsigned int opDim = (prect->y2-prect->y1-1) << 16
                                             | (prect->x2-prect->x1-1);

               GE_WAIT_IDLE();
               GE_OUT_D( GE_PAT_MAP_X, patCoOrd );
               GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
               GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
               GE_START_CMDW( GE_OPW_BITBLT
                              | GE_OPW_FRGD_SRC_CLR
                              | GE_OPW_BKGD_SRC_CLR
                              | GE_OPW_DEST_MAP_A   );
            }
         } 
         else if ( dy > 0 ) {
            GE_OUT_W( GE_PIXEL_OP, GE_OP_PAT_MAP_A
                                   | GE_OP_MASK_DISABLED
                                   | GE_OP_DEC_X
                                   | GE_OP_INC_Y         );
            for (i = 0; i < numRects; i++) {
               BoxPtr prect = &pbox[ordering[i]];
               register unsigned int patCoOrd = (prect->y1+dy) << 16
                                                | (prect->x2+dx-1);
               register unsigned int dstCoOrd = prect->y1 << 16 
                                                | (prect->x2-1);
               register unsigned int opDim = (prect->y2-prect->y1-1) << 16
                                             | (prect->x2-prect->x1-1);

               GE_WAIT_IDLE();
               GE_OUT_D( GE_PAT_MAP_X, patCoOrd );
               GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
               GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
               GE_START_CMDW( GE_OPW_BITBLT
                              | GE_OPW_FRGD_SRC_CLR
                              | GE_OPW_BKGD_SRC_CLR
                              | GE_OPW_DEST_MAP_A   );
            }
         } 
         else {
            GE_OUT_W( GE_PIXEL_OP, GE_OP_PAT_MAP_A
                                   | GE_OP_MASK_DISABLED
                                   | GE_OP_DEC_X
                                   | GE_OP_DEC_Y         );
            for (i = 0; i < numRects; i++) {
               BoxPtr prect = &pbox[ordering[i]];
               register unsigned int patCoOrd = (prect->y2+dy-1) << 16
                                                | (prect->x2+dx-1);
               register unsigned int dstCoOrd = prect->y2-1 << 16 
                                                | (prect->x2-1);
               register unsigned int opDim = (prect->y2-prect->y1-1) << 16
                                             | (prect->x2-prect->x1-1);


               GE_WAIT_IDLE();
               GE_OUT_D( GE_PAT_MAP_X, patCoOrd );
               GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
               GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
               GE_START_CMDW( GE_OPW_BITBLT
                              | GE_OPW_FRGD_SRC_CLR
                              | GE_OPW_BKGD_SRC_CLR
                              | GE_OPW_DEST_MAP_A   );
            }
         }
         DEALLOCATE_LOCAL(ordering);
	 GE_WAIT_IDLE_EXIT();
      } 
#endif
      else {
         /* Pixmap --> Pixmap */
         ErrorF("agxCopyPlane:  Tried to do a Pixmap to Pixmap copy\n");
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


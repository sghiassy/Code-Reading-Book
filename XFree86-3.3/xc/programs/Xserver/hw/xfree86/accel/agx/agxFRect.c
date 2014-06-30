/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxFRect.c,v 3.9 1996/12/23 06:32:40 dawes Exp $ */
/*
 * Fill rectangles.
 */

/*
 * Copyright 1989 by the Massachusetts Institute of Technology
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 * 
 * Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * 
 * KEVIN E. MARTIN AND HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE 
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY 
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS 
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Rewritten for the AGX by Henry A. Worth (haw30@eng.amdahl.com)
 * 
 */
/* $XConsortium: agxFRect.c /main/5 1996/02/21 17:17:23 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "cfb.h"
#include "cfbmskbits.h"
#include "mergerop.h"

#include "agx.h"
#include "regagx.h"

#define NUM_STACK_RECTS	1024
extern int agxMAX_SLOTS;

void
agxPolyFillRect(pDrawable, pGC, nrectFill, prectInit)
     DrawablePtr pDrawable;
     register GCPtr pGC;
     int   nrectFill;		/* number of rectangles to fill */
     xRectangle *prectInit;	/* Pointer to first rectangle to fill */
{
   xRectangle *prect;
   RegionPtr prgnClip;
   register BoxPtr pbox;
   register BoxPtr pboxClipped;
   BoxPtr pboxClippedBase;
   BoxPtr pextent;
   BoxRec stackRects[NUM_STACK_RECTS];
   cfbPrivGC *priv;
   int   numRects;
   int   n;
   int   xorg, yorg;
   int   xrot, yrot;

   if (!xf86VTSema)
   {
      switch (agxInfoRec.bitsPerPixel) {
      case 8:
         cfbPolyFillRect(pDrawable, pGC, nrectFill, prectInit);
         break;
      case 16:
         cfb16PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
         break;
#ifdef AGX_32BPP
      case 32:
         cfb32PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
         break;
#endif
      }
      return;
   }

   priv = (cfbPrivGC *) pGC->devPrivates[cfbGCPrivateIndex].ptr;
   prgnClip = priv->pCompositeClip;

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
      int   x1, y1, x2, y2, bx2, by2;

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
      int   x1, y1, x2, y2, bx2, by2;

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

       /*
        * clip the rectangle to each box in the clip region this is logically
        * equivalent to calling Intersect()
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

   if (pboxClipped != pboxClippedBase) {
      n = pboxClipped - pboxClippedBase;
      switch (pGC->fillStyle) {
	case FillSolid:
   	   pboxClipped = pboxClippedBase;
           {
              register unsigned int opDim 
                     = (pboxClipped->y2-pboxClipped->y1-1) << 16
                       | (pboxClipped->x2-pboxClipped->x1-1);
              register unsigned int dstCoOrd 
                     = pboxClipped->y1 << 16 | pboxClipped->x1;
   
              MAP_SET_DST( GE_MS_MAP_A );
   
              GE_WAIT_IDLE();
              GE_OUT_B(GE_FRGD_MIX, pGC->alu);
              GE_OUT_D(GE_PIXEL_BIT_MASK, pGC->planemask);
              GE_OUT_D(GE_FRGD_CLR, pGC->fgPixel);
#if 0   
              GE_OUT_D( GE_DEST_MAP_X, dstCoOrd ); 
              GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
              GE_START_CMD( GE_OP_BITBLT
                            | GE_OP_PAT_FRGD
                            | GE_OP_MASK_DISABLED
                            | GE_OP_INC_X
                            | GE_OP_INC_Y       
                            | GE_OP_FRGD_SRC_CLR
                            | GE_OP_DEST_MAP_A   );
#else
              GE_OUT_W( GE_PIXEL_OP,
                        GE_OP_PAT_FRGD
                        | GE_OP_MASK_DISABLED
                        | GE_OP_INC_X
                        | GE_OP_INC_Y         );
#endif
   
#if 0
   	      while (n--) {
	         pboxClipped++;
                 opDim = (pboxClipped->y2-pboxClipped->y1-1) << 16
                         | (pboxClipped->x2-pboxClipped->x1-1);
                 dstCoOrd = pboxClipped->y1 << 16 | pboxClipped->x1; 
                 GE_WAIT_IDLE();
#else
              for(;;) {
#endif
   
                 GE_OUT_D( GE_DEST_MAP_X, dstCoOrd ); 
                 GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
                 GE_START_CMDW( GE_OPW_BITBLT
                                | GE_OPW_FRGD_SRC_CLR
                                | GE_OPW_DEST_MAP_A   );
#if 1
                 
                 if (--n <= 0)  
                    break;
	         pboxClipped++;
                 opDim = (pboxClipped->y2-pboxClipped->y1-1) << 16
                         | (pboxClipped->x2-pboxClipped->x1-1);
                 dstCoOrd = pboxClipped->y1 << 16 | pboxClipped->x1;
                 GE_WAIT_IDLE();
#endif
	      }
           }
           GE_WAIT_IDLE_EXIT();
	   break;

	case FillTiled:
	   xrot = pDrawable->x + pGC->patOrg.x;
	   yrot = pDrawable->y + pGC->patOrg.y;

           agxFillBoxTile( pDrawable, n, pboxClippedBase,
                           pGC->tile.pixmap, xrot, yrot,
			   pGC->alu, pGC->planemask );
	   break;

	case FillStippled:
	   xrot = pDrawable->x + pGC->patOrg.x;
	   yrot = pDrawable->y + pGC->patOrg.y;

           agxFillBoxStipple( pDrawable, n, pboxClippedBase,
                              pGC->stipple, xrot, yrot,
			      pGC->fgPixel, 0,
			      pGC->alu, MIX_DST,
                              pGC->planemask);
	   break;
	case FillOpaqueStippled:
	   xrot = pDrawable->x + pGC->patOrg.x;
	   yrot = pDrawable->y + pGC->patOrg.y;

           agxFillBoxStipple( pDrawable, n, pboxClippedBase,
                              pGC->stipple, xrot, yrot,
		              pGC->fgPixel, pGC->bgPixel,
			      pGC->alu, pGC->alu, 
                              pGC->planemask );
	   break;
      }
   }
   if (pboxClippedBase != stackRects)
      DEALLOCATE_LOCAL(pboxClippedBase);
}

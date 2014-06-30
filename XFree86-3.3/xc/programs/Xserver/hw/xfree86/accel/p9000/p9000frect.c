/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000frect.c,v 3.2 1996/12/23 06:40:40 dawes Exp $ */
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
 * HENRIK HARMSEN, KEVIN E. MARTIN AND HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE 
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY 
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS 
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Rewritten for the AGX by Henry A. Worth (haw30@eng.amdahl.com)
 * Rewritten for the P9000 by Henrik Harmsen (harmsen@eritel.se)
 *    (with help from Chris Mason)
 */
/* $XConsortium: p9000frect.c /main/4 1996/02/21 17:32:22 kaleb $ */

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

#include "p9000reg.h"
#include "p9000.h"

#define NUM_STACK_RECTS	1024
/* #define P9000_DEBUG_FRECT 1 */

void
p9000PolyFillRect(pDrawable, pGC, nrectFill, prectInit)
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
   PixmapPtr pPix ;
   int   width, height, pixWidth ;


   if ( !xf86VTSema  || (!OnScreenDrawable(pDrawable->type)) 
        || (pGC->fillStyle == FillTiled) 
      )
   {
      cfbPolyFillRect(pDrawable, pGC, nrectFill, prectInit);
      return;
   }

#ifdef P9000_DEBUG_FRECT
  ErrorF("Accel 8 frect  PM: 0x%x ALU: %d\n", pGC->planemask, pGC->alu);
#endif

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
#if 0
/* done with bitblt: slower than quads */
        p9000NotBusy();
        p9000Store(PMASK, CtlBase, pGC->planemask);
        p9000Store(RASTER, CtlBase, IGM_F_MASK);
        p9000Store(FGROUND, CtlBase, pGC->fgPixel); 

        pboxClipped = pboxClippedBase;

        while(n--) {
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, CtlBase, (pboxClipped->x1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0, CtlBase, (pboxClipped->y1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1, CtlBase, (pboxClipped->x2-1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1, CtlBase, (pboxClipped->y2-1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2, CtlBase, (pboxClipped->x1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2, CtlBase, (pboxClipped->y1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3, CtlBase, (pboxClipped->x2-1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3, CtlBase, (pboxClipped->y2-1));

          while(p9000Fetch(CMD_BLIT, CtlBase) & SR_ISSUE_QBN);

          pboxClipped++;
        }
        break;
#endif
/* done with quads: */
        p9000NotBusy();
        p9000Store(PMASK, CtlBase, pGC->planemask);
        p9000Store(RASTER, CtlBase, p9000QuadAlu[pGC->alu]);
        p9000Store(FGROUND, CtlBase, pGC->fgPixel); 
        p9000Store(BGROUND, CtlBase, pGC->bgPixel); 

        pboxClipped = pboxClippedBase;

        while(n--) {
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, CtlBase, (pboxClipped->x1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0, CtlBase, (pboxClipped->y1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1, CtlBase, (pboxClipped->x2));
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1, CtlBase, (pboxClipped->y1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2, CtlBase, (pboxClipped->x2));
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2, CtlBase, (pboxClipped->y2));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3, CtlBase, (pboxClipped->x1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3, CtlBase, (pboxClipped->y2));

          while(p9000Fetch(CMD_QUAD, CtlBase) & SR_ISSUE_QBN);

          pboxClipped++;
        }
        break;
      case FillTiled:
        xrot = pDrawable->x + pGC->patOrg.x;
        yrot = pDrawable->y + pGC->patOrg.y;

        pPix = pGC->tile.pixmap;
        width = pPix->drawable.width;
        height = pPix->drawable.height;
        pixWidth = PixmapBytePad(width, pPix->drawable.depth);

        pboxClipped = pboxClippedBase;
        while (n--) {
          p9000ImageFill(pboxClipped->x1, pboxClipped->y1,
                            pboxClipped->x2 - pboxClipped->x1,
                            pboxClipped->y2 - pboxClipped->y1,
                            pPix->devPrivate.ptr, pixWidth,
                            width, height, xrot, yrot,
                            pGC->alu, pGC->planemask);
          pboxClipped++;
        }
        break ;

      case FillStippled:
        xrot = pDrawable->x + pGC->patOrg.x;
        yrot = pDrawable->y + pGC->patOrg.y;

        pPix = pGC->stipple;
        width = pPix->drawable.width;
        height = pPix->drawable.height;
        pixWidth = PixmapBytePad(width, pPix->drawable.depth);

        pboxClipped = pboxClippedBase;
        while (n--) {
          p9000ImageStipple(pboxClipped->x1, pboxClipped->y1,
                            pboxClipped->x2 - pboxClipped->x1,
                            pboxClipped->y2 - pboxClipped->y1,
                            pPix->devPrivate.ptr, pixWidth,
                            width, height, xrot, yrot,
                            pGC->fgPixel,
                            pGC->alu, pGC->planemask);
               pboxClipped++;
        }

        break ;
      case FillOpaqueStippled:
        xrot = pDrawable->x + pGC->patOrg.x;
        yrot = pDrawable->y + pGC->patOrg.y;

        pPix = pGC->stipple;
        width = pPix->drawable.width;
        height = pPix->drawable.height;
        pixWidth = PixmapBytePad(width, pPix->drawable.depth);

        pboxClipped = pboxClippedBase;
        while (n--) {
          p9000ImageOpStipple(pboxClipped->x1, pboxClipped->y1,
                            pboxClipped->x2 - pboxClipped->x1,
                            pboxClipped->y2 - pboxClipped->y1,
                            pPix->devPrivate.ptr, pixWidth,
                            width, height, xrot, yrot,
                            pGC->fgPixel,
                            pGC->bgPixel,
                            pGC->alu, pGC->planemask);
               pboxClipped++;
        }

        break ;
      }
      p9000QBNotBusy();
   }
   if (pboxClippedBase != stackRects)
      DEALLOCATE_LOCAL(pboxClippedBase);
}


void
p900016PolyFillRect(pDrawable, pGC, nrectFill, prectInit)
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
   int   n, i;
   int   xorg, yorg;
   int   xrot, yrot;

   if (!xf86VTSema 
       || (pGC->fillStyle != FillSolid) 
       || (pDrawable->type != DRAWABLE_WINDOW)
       || (pGC->alu != GXcopy)
       || (pGC->planemask != 0xffffffff)
      )
   {
      cfb16PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
      return;
   }

#ifdef P9000_DEBUG_FRECT
  ErrorF("Accel 16 frect  PM: 0x%x ALU: %d\n", pGC->planemask, pGC->alu);
#endif


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

        p9000NotBusy();
        p9000Store(PMASK, CtlBase, 0xFF);
        p9000Store(PAT_ORIGINX, CtlBase, 0);
        p9000Store(PAT_ORIGINY, CtlBase, 0);
        p9000Store(BGROUND, CtlBase, pGC->fgPixel & 0xFF); 
        p9000Store(FGROUND, CtlBase, (pGC->fgPixel & 0xFF00) >> 8); 
        p9000Store(RASTER, CtlBase, (IGM_S_MASK & IGM_B_MASK)
                                     | (~IGM_S_MASK & IGM_F_MASK)
                                     | USE_PATTERN);

        for (i = 0; i < 32; i += 4) {
          p9000Store((PATTERN01 + i), CtlBase, 0xAAAAAAAA);
        }

        pboxClipped = pboxClippedBase;

        while(n--) {
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, CtlBase, (pboxClipped->x1) << 1);
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0, CtlBase, (pboxClipped->y1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1, CtlBase, (pboxClipped->x2) << 1);
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1, CtlBase, (pboxClipped->y1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2, CtlBase, (pboxClipped->x2) << 1);
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2, CtlBase, (pboxClipped->y2));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3, CtlBase, (pboxClipped->x1) << 1);
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3, CtlBase, (pboxClipped->y2));

          while(p9000Fetch(CMD_QUAD, CtlBase) & SR_ISSUE_QBN);

          pboxClipped++;
        }
        break;
      }
      p9000QBNotBusy();
   }
   if (pboxClippedBase != stackRects)
      DEALLOCATE_LOCAL(pboxClippedBase);
}

void
p900032PolyFillRect(pDrawable, pGC, nrectFill, prectInit)
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
   int   n, i;
   int   xorg, yorg;
   int   xrot, yrot;
   int   nold;

   if (!xf86VTSema 
       || (pGC->fillStyle != FillSolid) 
       || (pDrawable->type != DRAWABLE_WINDOW)
       || (pGC->alu != GXcopy)
       || (pGC->planemask != 0xffffffff)
      )
   {
      cfb32PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
      return;
   }

#ifdef P9000_DEBUG_FRECT
  ErrorF("Accel 16 frect  PM: 0x%x ALU: %d\n", pGC->planemask, pGC->alu);
#endif


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

        p9000NotBusy();
        p9000Store(PMASK, CtlBase, 0xFF);
        p9000Store(PAT_ORIGINX, CtlBase, 0);
        p9000Store(PAT_ORIGINY, CtlBase, 0);
        p9000Store(BGROUND, CtlBase, (pGC->fgPixel & 0xFF00) >> 8);    
        p9000Store(FGROUND, CtlBase, (pGC->fgPixel & 0xFF0000) >> 16); 
        p9000Store(RASTER, CtlBase, (IGM_S_MASK & IGM_B_MASK)
                                  | (~IGM_S_MASK & IGM_F_MASK)
                                  | USE_PATTERN);

        for (i = 0; i < 32; i += 4) {
          p9000Store((PATTERN01 + i), CtlBase, 0x55555555);
        }

        pboxClipped = pboxClippedBase;

        nold = n;

        while(n--) {
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, CtlBase, (pboxClipped->x1) << 2);
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0, CtlBase, (pboxClipped->y1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1, CtlBase, (pboxClipped->x2) << 2);
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1, CtlBase, (pboxClipped->y1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2, CtlBase, (pboxClipped->x2) << 2);
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2, CtlBase, (pboxClipped->y2));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3, CtlBase, (pboxClipped->x1) << 2);
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3, CtlBase, (pboxClipped->y2));

          while(p9000Fetch(CMD_QUAD, CtlBase) & SR_ISSUE_QBN);

          pboxClipped++;
        }

        p9000QBNotBusy();
        p9000NotBusy();
        p9000Store(PMASK, CtlBase, 0xFF);
        p9000Store(PAT_ORIGINX, CtlBase, 0);
        p9000Store(PAT_ORIGINY, CtlBase, 0);
        p9000Store(FGROUND, CtlBase, (pGC->fgPixel & 0xFF));  
        p9000Store(RASTER, CtlBase, (IGM_S_MASK & IGM_F_MASK)
                                  | (~IGM_S_MASK & IGM_D_MASK)
                                  | USE_PATTERN);

        for (i = 0; i < 32; i += 4) {
          p9000Store((PATTERN01 + i), CtlBase, 0x88888888);
        }

        pboxClipped = pboxClippedBase;

        n = nold;

        while(n--) {
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, CtlBase, (pboxClipped->x1) << 2);
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0, CtlBase, (pboxClipped->y1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1, CtlBase, (pboxClipped->x2) << 2);
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1, CtlBase, (pboxClipped->y1));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2, CtlBase, (pboxClipped->x2) << 2);
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2, CtlBase, (pboxClipped->y2));
          p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3, CtlBase, (pboxClipped->x1) << 2);
          p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3, CtlBase, (pboxClipped->y2));

          while(p9000Fetch(CMD_QUAD, CtlBase) & SR_ISSUE_QBN);

          pboxClipped++;
        }
        break;
      }
      p9000QBNotBusy();
   }
   if (pboxClippedBase != stackRects)
      DEALLOCATE_LOCAL(pboxClippedBase);
}


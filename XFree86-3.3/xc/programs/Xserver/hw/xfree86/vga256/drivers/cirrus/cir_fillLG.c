/* $XConsortium: cir_fillLG.c /main/4 1996/10/26 11:17:07 kaleb $ */
/*
 * cir_fillLG.c
 *
 * Copyright 1996 by Corin Anderson, Bellevue, Washington
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Corin Anderson not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Corin Anderson makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * CORIN ANDERSON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORIN ANDERSON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Corin Anderson, <corina@bdc.cirrus.com>
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_fillLG.c,v 3.2 1996/12/27 07:05:23 dawes Exp $ */


#include "vga256.h"
#include "cfbrrop.h"
#include "mergerop.h"
#include "vgaBank.h"
#include "xf86.h"	/* For vgaBase. */
#include "vga.h"	/* For vgaBase. */
#include "compiler.h"

#include "cir_driver.h"
#include "cir_blitLG.h"

#define DEBUG_PAT_LG  0

#if DEBUG_PAT_LG
#include <stdio.h>
static FILE *patLGErr = NULL;
#endif

/* This file implements all functions related to solid color fills on a 
   Laguna family graphics accelerator.
   */


void
CirrusLgFillRectSolidCopy (pDrawable, pGC, nBox, pBox)
    DrawablePtr	    pDrawable;
    GCPtr	    pGC;
    int		    nBox;
    BoxPtr	    pBox;
{
  unsigned long rrop_xor,rrop_and;
  RROP_FETCH_GC(pGC);
  

  CirrusLgFillBoxSolid (pDrawable, nBox, pBox, rrop_xor, 0, pGC->alu);
}



void
CirrusLgFillBoxSolid (pDrawable, nBox, pBox, pixel1, pixel2, alu)
    DrawablePtr	    pDrawable;
    int		    nBox;
    BoxPtr	    pBox;
    unsigned long   pixel1;
    unsigned long   pixel2;
    int	            alu;
{
  unsigned char   *pdstBase;
  Bool            flag;
  int		  widthDst;
  int             h;
  int		  w;
  int             dstX, dstY;
  unsigned long   c;


#if DEBUG_PAT_LG
  if (!patLGErr)
    patLGErr = fopen("patLG.err", "wt");
  if (patLGErr) {
    fprintf(patLGErr, "pixel1 = 0x%08X  pixel2 = 0x%08X  alu = 0x%01X\n",
	    pixel1, pixel2, alu);
  }
#endif

  if (pDrawable->type == DRAWABLE_WINDOW)
    {
      pdstBase = (unsigned char *)
	(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
      widthDst = (int)
	(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind);
    }
  else
    {
      pdstBase = (unsigned char *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
      widthDst = (int)(((PixmapPtr)pDrawable)->devKind);
    }

  flag = CHECKSCREEN(pdstBase);	/* On screen? */

  if (!flag) {
    /* Shouldn't happen. */
    vga256FillBoxSolid(pDrawable, nBox, pBox, pixel1, pixel2, alu);
    
    return;
  }


  /* The only thing that changes between boxes is the destination.  */
  /* Set the color of the fill */
  if (vgaBitsPerPixel == 8) {
    c = pixel1 & 0x000000FF;
    c = (c<<24) | (c<<16) | (c<<8) | (c);
  } else
    c = pixel1;
  LgSETBACKGROUND(c);

  for (; nBox; nBox--, pBox++)
    {
      /* Calculate the extents of this box */
      h = pBox->y2 - pBox->y1;
      w = pBox->x2 - pBox->x1;
	  
      if (w <= 0 || h <= 0)
	continue;

      /* Where is this box on the screen? */
      dstX = pBox->x1;
      dstY = pBox->y1;

      /* Wait until the previous blit has finished */
      while (!LgReady())
	;

      LgSETDSTXY(dstX, dstY);           /* Destination on the screen */
      LgSETROP(lgCirrusRop[alu]);       /* The raster operation */
      LgSETMODE(SCR2SCR | COLORFILL);   /* screen-to-screen, color fill */
      LgSETEXTENTS(w, h);               /* Set the extents, start the blit */

#if DEBUG_PAT_LG
      if (patLGErr) {
	fprintf(patLGErr, "\tdstX = %d  dstY = %d  w = %d  h = %d\n",
		dstX, dstY, w, h);
      }
#endif
    }				/* for all boxes */


  /* Make sure any background blits are finished. */
  while (!LgReady())
    ;

  cirrusDoBackgroundBLT = FALSE;
}



/*
 * Fill rectangles.
 */

/*
 * We need to reproduce this to be able to use our own non-GXcopy
 * solid fills, and tiles.
 *
 * Works for 8bpp, and linear framebuffer 16bpp and 32bpp (compiled once).
 */


#if PPW == 4
extern void cfb8FillRectStippledUnnatural();
#endif

extern void cfb16FillRectSolidCopy();
extern void cfb16FillRectSolidGeneral();
extern void cfb16FillRectTileOdd();
extern void cfb24FillRectSolidCopy();
extern void cfb24FillRectSolidGeneral();
extern void cfb24FillRectTileOdd();
extern void cfb32FillRectSolidCopy();
extern void cfb32FillRectSolidGeneral();
extern void cfb32FillRectTileOdd();

#define NUM_STACK_RECTS	1024

void
CirrusLgPolyFillRect(pDrawable, pGC, nrectFill, prectInit)
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
    int		    numRects;
    void	    (*BoxFill)(DrawablePtr, GCPtr, int, BoxPtr);
    int		    n;
    int		    xorg, yorg;
    unsigned long   *pdstBase;
    int		    widthDst;
    RROP_DECLARE

#if 0	/* Can't use this for different depths. */
    cfbGetLongWidthAndPointer(pDrawable, widthDst, pdstBase)
#endif

    if (!xf86VTSema || pDrawable->type != DRAWABLE_WINDOW) {
      if (vgaBitsPerPixel == 8) {
	cfbPolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	return;
      }
      if (vgaBitsPerPixel == 16) {
	cfb16PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	return;
      }
      if (vgaBitsPerPixel == 24) {
	cfb32PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	return;
      }
      if (vgaBitsPerPixel == 32) {
	cfb32PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	return;
      }
      return;
    }
    
    priv = (cfbPrivGC *) pGC->devPrivates[cfbGCPrivateIndex].ptr;
    prgnClip = priv->pCompositeClip;

    BoxFill = 0;
    switch (pGC->fillStyle)
    {
    case FillSolid:
	RROP_FETCH_GCPRIV(priv);
	BoxFill = CirrusLgFillRectSolid;	/* Optimized. */
	break;
    case FillTiled:
    	/* Hmm, it seems FillRectTileOdd always gets called. --HH */
#if 0    
	if (!((cfbPrivGCPtr) pGC->devPrivates[cfbGCPrivateIndex].ptr)->
	pRotatedPixmap)
            BoxFill = cfbFillRectTileOdd;
	else
#endif
	if (vgaBitsPerPixel == 8) {
	    BoxFill = vga256FillRectTileOdd;
	} else if (vgaBitsPerPixel == 16) {
	    BoxFill = cfb16FillRectTileOdd;
	} else if (vgaBitsPerPixel == 24) {
	    BoxFill = cfb24FillRectTileOdd;
	} else {
	    BoxFill = cfb32FillRectTileOdd;
	}
	break;

#if (PPW == 4)
    case FillStippled:
	BoxFill = vga2568FillRectStippledUnnatural;
	break;

    case FillOpaqueStippled:
	BoxFill = vga2568FillRectStippledUnnatural;
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

	pextent = (*pGC->pScreen->RegionExtents)(prgnClip);
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




/*
 * Optimized accelerated solid filled rectangles for 8, 16 and 32bpp.
 * Requires Laguna family accelerator.
 * Command overhead is reduced as far as possible.
 */

void
CirrusLgFillRectSolid(pDrawable, pGC, nBox, pBox)
    DrawablePtr	    pDrawable;
    GCPtr	    pGC;
    int		    nBox;
    BoxPtr	    pBox;
{
  int pitch, busy, pixshift;
  unsigned long c;
  RROP_DECLARE;

  /* We only handle on-screen fills with full planemask. */
  if (vgaBitsPerPixel == 8 && 
      (!xf86VTSema || (pGC->planemask & 0xFF) != 0xFF || 
       pDrawable->type != DRAWABLE_WINDOW)) {
    vga256FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
    return;
  }

  if (vgaBitsPerPixel == 16 && 
      (!xf86VTSema || (pGC->planemask & 0xFFFF) != 0xFFFF ||
       pDrawable->type != DRAWABLE_WINDOW)) {
    cfb16FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
    return;
  }

  if (vgaBitsPerPixel == 24 && 
      (!xf86VTSema || (pGC->planemask & 0xFFFFFF) != 0xFFFFFF ||
       pDrawable->type != DRAWABLE_WINDOW)) {
    cfb24FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
    return;
  }

  if (vgaBitsPerPixel == 32 && 
      (!xf86VTSema || (pGC->planemask & 0xFFFFFFFF) != 0xFFFFFFFF ||
       pDrawable->type != DRAWABLE_WINDOW)) {
    cfb32FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
    return;
  }

  RROP_FETCH_GC(pGC);

  /*
   * Source bit pattern is irrelevant, background and foregound color
   * are set to the same value.
   */
   
  /* Set up the invariant BitBLT parameters. */
  if (vgaBitsPerPixel == 8) {
    c = pGC->fgPixel & 0x000000FF;
    c = (c<<24) | (c<<16) | (c<<8) | (c);
  } else
    c = pGC->fgPixel;
  LgSETBACKGROUND(c);


  while (nBox > 0) {
    int height, width;

    height = pBox->y2 - pBox->y1;
    width = pBox->x2 - pBox->x1;
      
    while (!LgReady())
      ;

    LgSETDSTXY(pBox->x1, pBox->y1);   /* Destination on the screen */
    LgSETROP(lgCirrusRop[pGC->alu]);       /* The raster operation */
    LgSETMODE(SCR2SCR | COLORFILL);   /* screen-to-screen, color fill */
    LgSETEXTENTS(width, height);       /* Set the extents, start the blit */
      
    
    nBox--;
    pBox++;
  }


  while (!LgReady())
    ;
}

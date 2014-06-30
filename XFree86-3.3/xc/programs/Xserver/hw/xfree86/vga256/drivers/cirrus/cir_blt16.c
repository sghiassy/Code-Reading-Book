/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_blt16.c,v 3.6 1996/12/23 06:56:35 dawes Exp $ */
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

*/
/* $XConsortium: cir_blt16.c /main/8 1996/10/26 11:16:53 kaleb $ */

/*
 * Harm Hanemaayer <hhanemaa@cs.ruu.nl>:
 *     Routines adapted from cfbbitblt.c, cfbblt.c and cfbwindow.c to
 *     accelerate BitBlt in linear-framebuffer 16/32-bits-per-pixel
 *     modes on Cirrus chipsets.
 */
 
/*
 * Hooks for cfb16/32 CopyArea and CopyWindow
 */


#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"

#include "compiler.h"
#include "vga256.h"
#include "xf86.h"
#include "vga.h"
#include "cir_driver.h"


/* Local functions. */

static void CirrusBppDoBitbltCopy();
static void CirrusBppPolyBitblt();

/* cfb16/32 functions that are referenced. */

extern RegionPtr cfb16BitBlt();
extern void cfb16DoBitbltCopy();
extern void cfb16DoBitbltXor();
extern void cfb16DoBitbltOr();
extern void cfb16DoBitbltGeneral();

extern RegionPtr cfb24BitBlt();
extern void cfb24DoBitbltCopy();
extern void cfb24DoBitbltXor();
extern void cfb24DoBitbltOr();
extern void cfb24DoBitbltGeneral();

extern RegionPtr cfb32BitBlt();
extern void cfb32DoBitbltCopy();
extern void cfb32DoBitbltXor();
extern void cfb32DoBitbltOr();
extern void cfb32DoBitbltGeneral();


/*
 * This function replaces the cfb16 CopyArea function. It catches plain
 * on-screen BitBlts in order to do them with the Cirrus BitBLT engine.
 */

RegionPtr
Cirrus16CopyArea(pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty)
    register DrawablePtr pSrcDrawable;
    register DrawablePtr pDstDrawable;
    GC *pGC;
    int srcx, srcy;
    int width, height;
    int dstx, dsty;
{
    void (*doBitBlt) ();
    
    doBitBlt = cfb16DoBitbltCopy;
    if (pGC->alu != GXcopy || (pGC->planemask & 0xFFFF) != 0xFFFF)
    {
	doBitBlt = cfb16DoBitbltGeneral;
	if ((pGC->planemask & 0xFFFF) == 0xFFFF)
	{
	    switch (pGC->alu) {
	    case GXxor:
		doBitBlt = cfb16DoBitbltXor;
		break;
	    case GXor:
		doBitBlt = cfb16DoBitbltOr;
		break;
	    }
	}
    }

    if (doBitBlt == cfb16DoBitbltCopy && pSrcDrawable->type == DRAWABLE_WINDOW
    && pDstDrawable->type == DRAWABLE_WINDOW && xf86VTSema)
        doBitBlt = CirrusBppDoBitbltCopy;

    return cfb16BitBlt (pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty, doBitBlt, 0L);
}


/*
 * This function replaces the cfb24 CopyArea function. It catches plain
 * on-screen BitBlts in order to do them with the Cirrus BitBLT engine.
 */

RegionPtr
Cirrus24CopyArea(pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty)
    register DrawablePtr pSrcDrawable;
    register DrawablePtr pDstDrawable;
    GC *pGC;
    int srcx, srcy;
    int width, height;
    int dstx, dsty;
{
    void (*doBitBlt) ();
    
    doBitBlt = cfb24DoBitbltCopy;
    if (pGC->alu != GXcopy || (pGC->planemask & 0xFFFFFF) != 0xFFFFFF)
    {
	doBitBlt = cfb24DoBitbltGeneral;
	if ((pGC->planemask & 0xFFFFFF) == 0xFFFFFF)
	{
	    switch (pGC->alu) {
	    case GXxor:
		doBitBlt = cfb24DoBitbltXor;
		break;
	    case GXor:
		doBitBlt = cfb24DoBitbltOr;
		break;
	    }
	}
    }

    if (doBitBlt == cfb24DoBitbltCopy && pSrcDrawable->type == DRAWABLE_WINDOW
    && pDstDrawable->type == DRAWABLE_WINDOW && xf86VTSema)
        doBitBlt = CirrusBppDoBitbltCopy;

    return cfb24BitBlt (pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty, doBitBlt, 0L);
}


/*
 * This function replaces the cfb32 CopyArea function. It catches plain
 * on-screen BitBlts in order to do them with the Cirrus BitBLT engine.
 */

RegionPtr
Cirrus32CopyArea(pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty)
    register DrawablePtr pSrcDrawable;
    register DrawablePtr pDstDrawable;
    GC *pGC;
    int srcx, srcy;
    int width, height;
    int dstx, dsty;
{
    void (*doBitBlt) ();
    
    doBitBlt = cfb32DoBitbltCopy;
    if (pGC->alu != GXcopy || (pGC->planemask & 0xFFFFFFFF) != 0xFFFFFFFF)
    {
	doBitBlt = cfb32DoBitbltGeneral;
	if ((pGC->planemask & 0xFFFFFFFF) == 0xFFFFFFFF)
	{
	    switch (pGC->alu) {
	    case GXxor:
		doBitBlt = cfb32DoBitbltXor;
		break;
	    case GXor:
		doBitBlt = cfb32DoBitbltOr;
		break;
	    }
	}
    }

    if (doBitBlt == cfb32DoBitbltCopy && pSrcDrawable->type == DRAWABLE_WINDOW
    && pDstDrawable->type == DRAWABLE_WINDOW && xf86VTSema)
        doBitBlt = CirrusBppDoBitbltCopy;

    return cfb32BitBlt (pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty, doBitBlt, 0L);
}


/*
 * This function replaces the ScreenRec CopyWindow function. It does the
 * plain BitBlt window moves with the Cirrus BitBLT engine.
 */

extern WindowPtr *WindowTable;

void 
CirrusCopyWindow(pWin, ptOldOrg, prgnSrc)
    WindowPtr pWin;
    DDXPointRec ptOldOrg;
    RegionPtr prgnSrc;
{
    DDXPointPtr pptSrc;
    register DDXPointPtr ppt;
    RegionRec rgnDst;
    register BoxPtr pbox;
    register int dx, dy;
    register int i, nbox;
    WindowPtr pwinRoot;

    if (!xf86VTSema)
        switch (vgaBitsPerPixel) {
        case 16 :
            cfb16CopyWindow(pWin, ptOldOrg, prgnSrc);
	    return;
        case 24 :
            cfb24CopyWindow(pWin, ptOldOrg, prgnSrc);
	    return;
        case 32 :
            cfb32CopyWindow(pWin, ptOldOrg, prgnSrc);
	    return;
        }

    pwinRoot = WindowTable[pWin->drawable.pScreen->myNum];

    REGION_INIT(pWin->drawable.pScreen, &rgnDst, NullBox, 0);

    dx = ptOldOrg.x - pWin->drawable.x;
    dy = ptOldOrg.y - pWin->drawable.y;
    REGION_TRANSLATE(pWin->drawable.pScreen, prgnSrc, -dx, -dy);
    REGION_INTERSECT(pWin->drawable.pScreen, &rgnDst, &pWin->borderClip, prgnSrc);

    pbox = REGION_RECTS(&rgnDst);
    nbox = REGION_NUM_RECTS(&rgnDst);
    if(!nbox || !(pptSrc = (DDXPointPtr )ALLOCATE_LOCAL(nbox * sizeof(DDXPointRec))))
    {
	REGION_UNINIT(pWin->drawable.pScreen, &rgnDst);
	return;
    }
    ppt = pptSrc;

    for (i = nbox; --i >= 0; ppt++, pbox++)
    {
	ppt->x = pbox->x1 + dx;
	ppt->y = pbox->y1 + dy;
    }

    CirrusBppDoBitbltCopy((DrawablePtr)pwinRoot, (DrawablePtr)pwinRoot,
	    GXcopy, &rgnDst, pptSrc, ~0L);
    DEALLOCATE_LOCAL(pptSrc);
    REGION_UNINIT(pWin->drawable.pScreen, &rgnDst);
}


/*
 * This is the local CirrusBppDoBitbltCopy function that handles 16/32-bit
 * pixel on-screen BitBlts, dispatching them to CirrusBppPolyBitblt.
 */
 
static void
CirrusBppDoBitbltCopy(pSrc, pDst, alu, prgnDst, pptSrc, planemask)
    DrawablePtr	    pSrc, pDst;
    int		    alu;
    RegionPtr	    prgnDst;
    DDXPointPtr	    pptSrc;
    unsigned long   planemask;
{
    unsigned long *psrcBase, *pdstBase;	
				/* start of src and dst bitmaps */
    int widthSrc, widthDst;	/* add to get to same position in next line */

    BoxPtr pbox;
    int nbox;

    BoxPtr pboxTmp, pboxNext, pboxBase, pboxNew1, pboxNew2;
				/* temporaries for shuffling rectangles */
    DDXPointPtr pptTmp, pptNew1, pptNew2;
				/* shuffling boxes entails shuffling the
				   source points too */
    int w, h;
    int xdir;			/* 1 = left right, -1 = right left/ */
    int ydir;			/* 1 = top down, -1 = bottom up */
    int careful;

#if 0
    /* This doesn't work because we need to deal with cfb16 and cfb32
     * at the same time. */
    cfbGetLongWidthAndPointer (pSrc, widthSrc, psrcBase)
    cfbGetLongWidthAndPointer (pDst, widthDst, pdstBase)
#endif
    if (vgaBitsPerPixel == 16)
        widthSrc = widthDst = vga256InfoRec.displayWidth * 2;
    else if (vgaBitsPerPixel == 24)
        widthSrc = widthDst = vga256InfoRec.displayWidth * 3;
    else
        widthSrc = widthDst = vga256InfoRec.displayWidth * 4;

    /* XXX we have to err on the side of safety when both are windows,
     * because we don't know if IncludeInferiors is being used.
     */
    careful = ((pSrc == pDst) ||
	       ((pSrc->type == DRAWABLE_WINDOW) &&
		(pDst->type == DRAWABLE_WINDOW)));

    pbox = REGION_RECTS(prgnDst);
    nbox = REGION_NUM_RECTS(prgnDst);

    pboxNew1 = NULL;
    pptNew1 = NULL;
    pboxNew2 = NULL;
    pptNew2 = NULL;
    if (careful && (pptSrc->y < pbox->y1))
    {
        /* walk source botttom to top */
	ydir = -1;
	widthSrc = -widthSrc;
	widthDst = -widthDst;

	if (nbox > 1)
	{
	    /* keep ordering in each band, reverse order of bands */
	    pboxNew1 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
	    if(!pboxNew1)
		return;
	    pptNew1 = (DDXPointPtr)ALLOCATE_LOCAL(sizeof(DDXPointRec) * nbox);
	    if(!pptNew1)
	    {
	        DEALLOCATE_LOCAL(pboxNew1);
	        return;
	    }
	    pboxBase = pboxNext = pbox+nbox-1;
	    while (pboxBase >= pbox)
	    {
	        while ((pboxNext >= pbox) &&
		       (pboxBase->y1 == pboxNext->y1))
		    pboxNext--;
	        pboxTmp = pboxNext+1;
	        pptTmp = pptSrc + (pboxTmp - pbox);
	        while (pboxTmp <= pboxBase)
	        {
		    *pboxNew1++ = *pboxTmp++;
		    *pptNew1++ = *pptTmp++;
	        }
	        pboxBase = pboxNext;
	    }
	    pboxNew1 -= nbox;
	    pbox = pboxNew1;
	    pptNew1 -= nbox;
	    pptSrc = pptNew1;
        }
    }
    else
    {
	/* walk source top to bottom */
	ydir = 1;
    }

    if (careful && (pptSrc->x < pbox->x1))
    {
	/* walk source right to left */
        xdir = -1;

	if (nbox > 1)
	{
	    /* reverse order of rects in each band */
	    pboxNew2 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
	    pptNew2 = (DDXPointPtr)ALLOCATE_LOCAL(sizeof(DDXPointRec) * nbox);
	    if(!pboxNew2 || !pptNew2)
	    {
		if (pptNew2) DEALLOCATE_LOCAL(pptNew2);
		if (pboxNew2) DEALLOCATE_LOCAL(pboxNew2);
		if (pboxNew1)
		{
		    DEALLOCATE_LOCAL(pptNew1);
		    DEALLOCATE_LOCAL(pboxNew1);
		}
	        return;
	    }
	    pboxBase = pboxNext = pbox;
	    while (pboxBase < pbox+nbox)
	    {
	        while ((pboxNext < pbox+nbox) &&
		       (pboxNext->y1 == pboxBase->y1))
		    pboxNext++;
	        pboxTmp = pboxNext;
	        pptTmp = pptSrc + (pboxTmp - pbox);
	        while (pboxTmp != pboxBase)
	        {
		    *pboxNew2++ = *--pboxTmp;
		    *pptNew2++ = *--pptTmp;
	        }
	        pboxBase = pboxNext;
	    }
	    pboxNew2 -= nbox;
	    pbox = pboxNew2;
	    pptNew2 -= nbox;
	    pptSrc = pptNew2;
	}
    }
    else
    {
	/* walk source left to right */
        xdir = 1;
    }

    CirrusBppPolyBitblt(pdstBase, psrcBase, widthSrc, widthDst, nbox,
        pptSrc, pbox, xdir, ydir);

    if (pboxNew2)
    {
	DEALLOCATE_LOCAL(pptNew2);
	DEALLOCATE_LOCAL(pboxNew2);
    }
    if (pboxNew1)
    {
	DEALLOCATE_LOCAL(pptNew1);
	DEALLOCATE_LOCAL(pboxNew1);
    }
}


/*
 * This functions performs the BitBlts, calling the low-level BitBLT
 * routines that talk to the hardware.
 */

static void
CirrusBppPolyBitblt(pdstBase, psrcBase, widthSrc, widthDst, nbox, pptSrc,
pbox, xdir, ydir)
	pointer pdstBase, psrcBase;
	int widthSrc, widthDst;
	int nbox;
	DDXPointPtr pptSrc;
	BoxPtr pbox;
	int xdir, ydir;
{
  unsigned int psrc, pdst;
  int i;

  for (; nbox; pbox++, pptSrc++, nbox--) {
      int x, y, x1, y1, w, h;
      int bltWidthSrc, bltWidthDst, bltxdir;
      x = pptSrc->x;
      y = pptSrc->y;
      x1 = pbox->x1;
      y1 = pbox->y1;
      w = pbox->x2 - x1;
      h = pbox->y2 - y1;

#if 0
      /*
       * The 'latch' framebuffer BitBlt could be used in 16bpp on the
       * 5422/24, and the newer chips if "no_bitblt" is enabled, but
       * it is currently disabled because of the unavailability of
       * a primitive cfb routine for cases that can not be handled.
       */
      if (!HAVEBITBLTENGINE()) {
          if (xdir == 1 && ydir == 1)
              /* Special case for extended write mode bitblt (scroll up). */
              if (w >= 32 && ((x * 2) & 7) == ((x1 * 2) & 7)) {
                  if (vgaBitsPerPixel == 16)
                      CirrusLatchedBitBlt(x * 2, y, x1 * 2, y1, w * 2, h,
                          widthDst);
                  else
                      CirrusLatchedBitBlt(x * 4, y, x1 * 4, y1, w * 4, h,
                          widthDst);
                  continue;
              }

	  if (xdir == 1 && ydir == -1) {
              /* Special case for reversed extended write mode bitblt */
              /* (scroll down). */
              if (w >= 32 && ((x * 2) & 7) == ((x1 * 2) & 7)) {
                  if (vgaBitsPerPixel == 16)
                      CirrusLatchedBitBltReversed(x * 2, y, x1 * 2, y1, w * 2,
                          h, -widthDst);
                  else
                      CirrusLatchedBitBltReversed(x * 4, y, x1 * 4, y1, w * 4,
                          h, -widthDst);
                  continue;
              }
          }

          /* Let cfb do remaining blits. */
	  /* Problem: there is no low-level cfb BitBlt primitive. */
          vgaBitBlt(pdstBase, psrcBase, widthSrc, widthDst, x, y,
	      x1, y1, w, h, xdir, ydir, GXcopy, 0xff);

	  continue;
      }
#endif

      /* We have a hardware BitBLT engine. */
      /* For small areas, use a normal framebuffer routine. */
#if 0
      /* Problem: there is no low-level cfb BitBlt primitive. */
      if (w * h < 100) {

	  if (cirrusBLTisBusy)
              /* We can't access video memory during a blit. */
	      CirrusBLTWaitUntilFinished();

          vgaBitBlt(pdstBase, psrcBase, widthSrc, widthDst, x, y,
              x1, y1, w, h, xdir, ydir, GXcopy, 0xff);
          continue;
      }
#endif

      /* Use the hardware blit. */

      bltWidthSrc = widthSrc < 0 ? -widthSrc : widthSrc;
      bltWidthDst = widthDst < 0 ? -widthDst : widthDst;

      bltxdir = xdir;
      if (xdir == 1 && ydir == -1 && y != y1) {
          /* Tranform to xdir = -1 blit. */
          bltxdir = -1;
      }

      /* Transform area to byte units. */
      if (vgaBitsPerPixel == 16) {
          x *= 2;
          x1 *= 2;
          w *= 2;
      }
      else
      if (vgaBitsPerPixel == 24) {
          x *= 3;
          x1 *= 3;
          w *= 3;
      }
      else { /* vgaBitsPerPixel == 32 */
          x *= 4;
          x1 *= 4;
          w *= 4;
      }

      if (bltxdir == 1)		/* left to right */
	{
	  if (ydir == 1)	/* top to bottom */
	    {
	      psrc = (y * bltWidthSrc) + x;
	      pdst = (y1 * bltWidthDst) + x1;
	    }
	  else
	    /* bottom to top */
	    {
	      psrc = ((y + h - 1) * bltWidthSrc) + x;
	      pdst = ((y1 + h - 1) * bltWidthDst) + x1;
	    }
	}
      else
	/* right to left */
	{
	  if (ydir == 1)	/* top to bottom */
	    {
	      psrc = (y * bltWidthSrc) + x + w - 1;
	      pdst = (y1 * bltWidthDst) + x1 + w - 1;
	    }
	  else
	    /* bottom to top */
	    {
	      psrc = ((y + h - 1) * bltWidthSrc) + x + w - 1;
	      pdst = ((y1 + h - 1) * bltWidthDst) + x1 + w - 1;
	    }
	}

      cirrusDoBackgroundBLT = TRUE;

      if (bltxdir == 1)
	{
	  if (ydir == 1)
	    {			/* Nothing special, straight blit */
	      if (cirrusUseMMIO)
	          CirrusMMIOBLTBitBlt(pdst, psrc, bltWidthDst, bltWidthSrc, w, h, 1);
	      else
	          CirrusBLTBitBlt(pdst, psrc, bltWidthDst, bltWidthSrc, w, h, 1);
	    }
	  else
	    /* Line by line, going up. */
	    {
	      int bltpsrc, bltpdst;
	      bltpsrc = psrc;
	      bltpdst = pdst;
	      for (i = 0; i < h; i++)
		{
		  if (cirrusUseMMIO)
		      CirrusMMIOBLTBitBlt(bltpdst, bltpsrc, bltWidthDst, bltWidthSrc, w, 1, 1);
		  else
		      CirrusBLTBitBlt(bltpdst, bltpsrc, bltWidthDst, bltWidthSrc, w, 1, 1);
		  bltpsrc -= bltWidthSrc;
		  bltpdst -= bltWidthDst;
		}
	    }
	}
      else
	{

	  if (ydir == 1)	/* Line by line, going down and to the left */
	    {
	      int bltpsrc, bltpdst;
	      bltpsrc = psrc;
	      bltpdst = pdst;
	      for (i = 0; i < h; i++)
		{
		  if (cirrusUseMMIO)
		      CirrusMMIOBLTBitBlt(bltpdst, bltpsrc, bltWidthDst, bltWidthSrc, w, 1, -1);
		  else
		      CirrusBLTBitBlt(bltpdst, bltpsrc, bltWidthDst, bltWidthSrc, w, 1, -1);
		  bltpsrc += bltWidthSrc;
		  bltpdst += bltWidthDst;
		}
	    }
	  else
	    /* Another stock blit, albeit backwards */
	    {
	      if (cirrusUseMMIO)
	          CirrusMMIOBLTBitBlt(pdst, psrc, bltWidthDst, bltWidthSrc, w, h, -1);
	      else
	          CirrusBLTBitBlt(pdst, psrc, bltWidthDst, bltWidthSrc, w, h, -1);
	    }
	}

      /* Next region. */
  }

  /* Make sure any background blits are finished. */
  if (cirrusBLTisBusy)
      CirrusBLTWaitUntilFinished();
  cirrusDoBackgroundBLT = FALSE;
}

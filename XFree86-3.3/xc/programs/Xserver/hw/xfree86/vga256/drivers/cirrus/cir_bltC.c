/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_bltC.c,v 3.12.2.1 1997/05/10 09:10:21 hohndel Exp $ */
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


/*
 * Author:  Bill Reynolds, bill@goshawk.lanl.gov
 *
 * Reworked by: Simon P. Cooper, <scooper@vizlab.rutgers.edu>
 * Modified: H. Hanemaayer, <hhanemaa@cs.ruu.nl>
 *	Added CirrusCopyPlane1to8.
 *
 */
/* $XConsortium: cir_bltC.c /main/9 1996/10/23 18:47:46 kaleb $ */

/*
 * This is the high-level BitBlt function. Calls CirrusBitBlt.
 */


#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include        "vgaBank.h"
#include	"vga256.h"

#include "cir_driver.h"

extern void (*ourvgaBitBlt)();

void
CirrusDoBitbltCopy(pSrc, pDst, alu, prgnDst, pptSrc, planemask)
    DrawablePtr	    pSrc, pDst;
    int		    alu;
    RegionPtr	    prgnDst;
    DDXPointPtr	    pptSrc;
    unsigned long   planemask;
{
  unsigned char *psrcBase, *pdstBase;	
  /* start of src and dst bitmaps */
  int widthSrc, widthDst;	/* add to get to same position in next line */
  register void (*fnp)();
  int NoCirrus = 0;
  BoxPtr pbox;
  int nbox;
  
  BoxPtr pboxTmp, pboxNext, pboxBase, pboxNew1, pboxNew2;
                                  /* temporaries for shuffling rectangles */
  DDXPointPtr pptTmp, pptNew1, pptNew2;
                                  /* shuffling boxes entails shuffling the
                                     source points too */

  int xdir;			/* 1 = left right, -1 = right left/ */
  int ydir;			/* 1 = top down, -1 = bottom up */
  int careful;

  void fastBitBltCopy();
  void vgaImageRead();
  void vgaImageWrite();
  void CirrusBLTImageWrite();
  void CirrusBLTImageRead();
  void CirrusPolyBitBlt();	/* Special, different args. */
  void vgaPixBitBlt();

  if (pSrc->type == DRAWABLE_WINDOW)
    {
      psrcBase = (unsigned char *)
	(((PixmapPtr)(pSrc->pScreen->devPrivate))->devPrivate.ptr);
      widthSrc = (int)((PixmapPtr)(pSrc->pScreen->devPrivate))->devKind;
    }
  else
    {
      psrcBase = (unsigned char *)(((PixmapPtr)pSrc)->devPrivate.ptr);
      widthSrc = (int)(((PixmapPtr)pSrc)->devKind);
    }
  
  if (pDst->type == DRAWABLE_WINDOW)
    {
      pdstBase = (unsigned char *)
	(((PixmapPtr)(pDst->pScreen->devPrivate))->devPrivate.ptr);
      widthDst = (int)
	((PixmapPtr)(pDst->pScreen->devPrivate))->devKind;
    }
  else
    {
      pdstBase = (unsigned char *)(((PixmapPtr)pDst)->devPrivate.ptr);
      widthDst = (int)(((PixmapPtr)pDst)->devKind);
    }


  /* XXX we have to err on the side of safety when both are windows,
   * because we don't know if IncludeInferiors is being used.
   */
  careful = ((pSrc == pDst) ||
	     ((pSrc->type == DRAWABLE_WINDOW) &&
	      (pDst->type == DRAWABLE_WINDOW)));
  
  pbox = REGION_RECTS(prgnDst);
  nbox = REGION_NUM_RECTS(prgnDst);
  
  if (careful && (pptSrc->y < pbox->y1)) NoCirrus = 1;
  if (careful && (pptSrc->x < pbox->x1)) NoCirrus = 1;
  
  if (CHECKSCREEN(psrcBase))
       {
       if (CHECKSCREEN(pdstBase)) /* Screen -> Screen */
	    {
	    /* Forget about non-GXcopy BitBLTs. We can do them, but */
	    /* I don't know how common they are. */
	    if (alu == GXcopy && (planemask & 0xff) == 0xff)
	    	/* NULL indicates potential hardware Cirrus BitBlt; we
	    	 * let that function traverse the list of regions.
	    	 * for efficiency. See end of function. */
	        fnp = NULL;
	    else
	        fnp = vgaBitBlt;
	    }
       else			/* Screen -> Mem */
	    {
/*	    if(NoCirrus || !HAVEBITBLTENGINE() || HAVE543X() || 
               HAVE75XX() || cirrusChip == CLGD5446 ||
	       cirrusChip == CLGD5480) */
	    if (1)	/* ImageRead is unreliable. */
		 {
		 fnp = vgaImageRead;
		 }
	    else 
		 {
		 fnp = CirrusBLTImageRead;
		 }
	    }
       }
  else 
       if (CHECKSCREEN(pdstBase)) /* Mem -> Screen */
	    {
	    if(NoCirrus || !HAVEBITBLTENGINE() || cirrusAvoidImageBLT)
	    /* ImageWrite seems prone to pixel errors. Probably
	     * caused by other function -- ImageWrite is used all
	     * the time by the cursor.
	     */
		 {
		 fnp = vgaImageWrite;
		 }
	    else
	        if (cirrusUseMMIO)
	            fnp = CirrusMMIOBLTImageWrite;
	        else
	            fnp = CirrusBLTImageWrite;
	    }
       else fnp = vgaPixBitBlt;	/* Don't need to change: Mem -> Mem */
       
       
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

  /* Avoid the calling overhead for (potential) hardware blits. */
  if (fnp == NULL)
  	/* Screen-to-screen, alu == GXcopy, (planemask & 0xff) == 0xff */
  	CirrusPolyBitBlt(pdstBase, psrcBase, widthSrc, widthDst, nbox,
  	    pptSrc, pbox, xdir, ydir);

  else
  while(nbox--) {
    (*fnp)(pdstBase, psrcBase,widthSrc,widthDst,
	   pptSrc->x, pptSrc->y, pbox->x1, pbox->y1,
	   pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
	   xdir, ydir, alu, planemask);
    pbox++;
    pptSrc++;
  }
  
  /* free up stuff */
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


#ifdef CIRRUS_INCLUDE_COPYPLANE1TO8

extern void cfbCopyPlane1to8();

void CirrusCopyPlane1to8(pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc,
planemask, bitplane)
	DrawablePtr pSrcDrawable;
	DrawablePtr pDstDrawable;
	int rop;
	unsigned long planemask;
	RegionPtr prgnDst;
	DDXPointPtr pptSrc;
	int bitplane;	/* Unused. */
{
    unsigned long *psrcBase, *pdstBase;
    int	widthSrc, widthDst;
    int pixwidth;
    int nbox;
    BoxPtr  pbox;

    cfbGetLongWidthAndPointer (pSrcDrawable, widthSrc, psrcBase)

    cfbGetLongWidthAndPointer (pDstDrawable, widthDst, pdstBase)

    if (!CHECKSCREEN(pdstBase) || cfb8StippleRRop != GXcopy) {
    	vga256CopyPlane1to8(pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc,
    		planemask, 1);
    	return;
    }

    nbox = REGION_NUM_RECTS(prgnDst);
    pbox = REGION_RECTS(prgnDst);

    /* The planemask is understood to be 1 for all cases in which */
    /* this function is called. */

    while (nbox--)
    {
	int srcx, srcy, dstx, dsty, width, height;
	int bg, fg;
	dstx = pbox->x1;
	dsty = pbox->y1;
	srcx = pptSrc->x;
	srcy = pptSrc->y;
	width = pbox->x2 - pbox->x1;
	height = pbox->y2 - pbox->y1;
	pbox++;
	pptSrc++;

	fg = cfb8StippleFg;
	bg = cfb8StippleBg;

	if (width >= 32)
		if (cirrusUseMMIO)
			CirrusMMIOBLTWriteBitmap(dstx, dsty, width, height,
				psrcBase, widthSrc * 4, srcx, srcy, bg, fg,
				widthDst * 4);
		else
			CirrusBLTWriteBitmap(dstx, dsty, width, height,
				psrcBase, widthSrc * 4, srcx, srcy, bg, fg,
				widthDst * 4);
	else {
		/* Create singular region. */
		RegionRec reg;
		(*pDstDrawable->pScreen->RegionInit)(&reg, pbox - 1, 1);
		vga256CopyPlane1to8(pSrcDrawable, pDstDrawable, rop,
			&reg, pptSrc - 1, planemask, 1);
		(*pDstDrawable->pScreen->RegionUninit)(&reg);
	}
    }
}

#endif

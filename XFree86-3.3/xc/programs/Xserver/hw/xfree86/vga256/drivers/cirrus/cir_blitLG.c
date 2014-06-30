/* $XConsortium: cir_blitLG.c /main/5 1996/10/26 11:16:49 kaleb $ */
/*
 * cir_blitLG.c
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

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_blitLG.c,v 3.3 1996/12/27 07:05:21 dawes Exp $ */

#include "vga256.h"
#include "cfbrrop.h"
#include "mergerop.h"
#include "vgaBank.h"
#include "xf86.h"	/* For vgaBase. */
#include "vga.h"	/* For vgaBase. */
#include "compiler.h"

#include "cir_driver.h"
#include "cir_blitLG.h"

/* This file implements all functions related to bitBlts on a Laguna
   family graphics accelerator.  These functions include:

   ** Screen-to-Screen blits

   */

static void
CirrusLgScreen2Screen(pointer pdstBase, pointer psrcBase, 
		      int widthSrc, int widthDst, 
		      int nbox, DDXPointPtr pptSrc,
		      BoxPtr pbox, int xdir, int ydir, int alu);

static void CirrusLgScr2ScrLeft2Right(int srcX, int dstX, int Y, 
				      int w, int h, int alu);

/* Cirrus raster operations. */
int lgCirrusRop[16] = {
	LGROP_0,		/* GXclear */
	LGROP_AND,		/* GXand */
	LGROP_SRC_AND_NOT_DST,	/* GXandReverse */
	LGROP_SRC,		/* GXcopy */
	LGROP_NOT_SRC_AND_DST,	/* GXandInverted */
	LGROP_DST,		/* GXnoop */
	LGROP_XOR,		/* GXxor */
	LGROP_OR,		/* GXor */
	LGROP_NOR,		/* GXnor */
	LGROP_XNOR,		/* GXequiv */
	LGROP_NOT_DST,		/* GXinvert */
	LGROP_SRC_OR_NOT_DST,	/* GXorReverse */
	LGROP_NOT_SRC,		/* GXcopyInverted */
	LGROP_NOT_SRC_OR_DST,	/* GXorInverted */
	LGROP_NAND,		/* GXnand */
	LGROP_1			/* GXset */
};


int LgReady(void)
{
  volatile unsigned char status;

  status = *(unsigned char *)(cirrusMMIOBase + 0x0400);
  if (status & 0x07)
    return 0;
  else
    return 1;
}




void
CirrusLgDoBitbltCopy(pSrc, pDst, alu, prgnDst, pptSrc, planemask)
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

  void vgaImageRead();            /* Default screen-to-host */
  void vgaImageWrite();           /* Default host-to-screen */
  void vgaPixBitBlt();            /* Default host-to-host (ick!) */

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
  
  if (CHECKSCREEN(psrcBase)) {
    if (CHECKSCREEN(pdstBase))         /* Screen -> Screen */
      fnp = CirrusLgScreen2Screen;
    else			       /* Screen -> Mem */
      fnp = vgaImageRead;
  } else {
    if (CHECKSCREEN(pdstBase)) {       /* Mem -> Screen */
      fnp = vgaImageWrite;
    }
    else fnp = vgaPixBitBlt;	       /* Mem -> Mem */
  }
       
       
  /* The VGA code does this box manipulation, so I will blindly copy it.
     The Apline code also does it, so I guess that it's necessary.  I haven't
     really looked through it very thoroughly, though. */
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
  if (fnp == CirrusLgScreen2Screen) {
    (*fnp)(pdstBase, psrcBase, widthSrc, widthDst, nbox,
	   pptSrc, pbox, xdir, ydir, alu);
  } else {
    /* Where using the default VGA functions.  Walk through all the boxes,
       and feed each to the function. */
    while(nbox--) {
      (*fnp)(pdstBase, psrcBase,widthSrc,widthDst,
	     pptSrc->x, pptSrc->y, pbox->x1, pbox->y1,
	     pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
	     xdir, ydir, alu, planemask);
      pbox++;
      pptSrc++;
    }
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




void
CirrusLgScreen2Screen(pdstBase, psrcBase, widthSrc, widthDst, nbox, pptSrc,
		      pbox, xdir, ydir, alu)
  pointer pdstBase, psrcBase;
  int widthSrc, widthDst;            /* Unused. */
  int nbox;
  DDXPointPtr pptSrc;
  BoxPtr pbox;
  int xdir, ydir;
  int alu;
{
  unsigned int psrc, pdst;
  int i;
  int srcX, srcY, dstX, dstY, w, h;
  unsigned short blitdir = 0x0000;

  for (; nbox; pbox++, pptSrc++, nbox--) {
    srcX = pptSrc->x;         /* Source X */
    srcY = pptSrc->y;         /* Source Y */
    dstX = pbox->x1;          /* Destination X */
    dstY = pbox->y1;          /* Destination Y */
    w = pbox->x2 - dstX;      /* Width */
    h = pbox->y2 - dstY;      /* Height */


    /* We have a hardware BitBLT engine. */

    /* Use the hardware blit. */

    if (dstY > srcY) {
      /* The destination of the blit overlaps the source.  The solution is
	 to start the blit from the bottom and work up.  To do this, we need
	 to set the MSB in the BLTDEF (or, | BLITUP in the LgSETMODE).
	 
	 We also need to adjust the source and destination Y values.  The 
	 Y values are always the *first* line that the blit engine will
	 copy.  Thus, if we're blitting bottom-up, the Y values must be
	 the bottom of the rectangle.  */

      blitdir = BLITUP;
      srcY += h - 1;
      dstY += h - 1;
    }
    
    if (dstY == srcY && dstX > srcX) {
      /* This is a purely horizontal blit.  The Laguna can't handle
	 this sort of blit correctly, so pawn it off to the default
	 vga handler. */

      CirrusLgScr2ScrLeft2Right(srcX, dstX, srcY, w, h, alu);
      continue;
    }

    /* Wait until the bitblit engine is idle.  In all actuallity, I could
       wait just until there is room for the following writes in the FIFO
       queue, and the bitblit engine will handle all the blits correctly.
       BUT, because it's a few lines shorter to code, and I'm more confident 
       with it, I'm going to simply wait for the 2D engine to become 
       completely idle before blasting the next blit. */

    /* Wait until the previous blit has finished */
    while (!LgReady())
      ;

    LgSETSRCXY(srcX, srcY);           /* Source on the screen */
    LgSETDSTXY(dstX, dstY);           /* Destination on the screen */
    LgSETROP(lgCirrusRop[alu]);       /* The raster operation */
    LgSETMODE(SCR2SCR | COLORSRC | blitdir);  /* screen-to-screen, color */
    LgSETEXTENTS(w, h);               /* Set the extents, start the blit */
    
    /* Next region. */
  }

  /* Wait until the previous blit has finished */
  while (!LgReady())
    ;
  cirrusDoBackgroundBLT = FALSE;
}



void CirrusLgScr2ScrLeft2Right(int srcX, int dstX, int Y, 
			       int w, int h, int alu)
{
  /* Special, weird case:  purely horizontal, left-to-right blit.  In this
     case, the laguna can't cope; the result is a horizontally repeating
     pattern of the source.  The solution is to stripe the blit into little 
     non-overlapping chunks. */

  int stripeWidth = 16;   /* Preferred stripe width */
  int fullStripes;
  int extra;
  int i;

  if (dstX - srcX < stripeWidth)        /* The idea is *non*overlapping */
    stripeWidth = dstX - srcX;

  if (stripeWidth <= 0)                 /* Avoid bogus blits */
    return;

  fullStripes = w / stripeWidth;
  extra = w % stripeWidth;

  /* First, take care of the little bit on the far right */
  if (extra) {
    LgSETSRCXY(srcX + fullStripes*stripeWidth, Y);  /* Source on the screen */
    LgSETDSTXY(dstX + fullStripes*stripeWidth, Y);  /* Dest on the screen */
    LgSETROP(lgCirrusRop[alu]);           /* The raster operation */
    LgSETMODE(SCR2SCR | COLORSRC);        /* screen-to-screen, color */
    LgSETEXTENTS(extra, h);               /* Set the extents, start the blit */
  
    /* Wait until the previous blit has finished */
    while (!LgReady())
      ;
  }

  /* Now, take care of the rest of the blit */
  for (i = fullStripes-1; i >= 0; i--) {
    /* Wait until the previous blit has finished */
    while (!LgReady())
      ;

    LgSETSRCXY(srcX + i*stripeWidth, Y);  /* Source on the screen */
    LgSETDSTXY(dstX + i*stripeWidth, Y);  /* Destination on the screen */
    LgSETROP(lgCirrusRop[alu]);           /* The raster operation */
    LgSETMODE(SCR2SCR | COLORSRC);        /* screen-to-screen, color */
    LgSETEXTENTS(stripeWidth, h);         /* Set the extents, start the blit */
  }

  /* Wait until the previous blit has finished */
  while (!LgReady())
    ;
}

/* cfb16/32 functions that are referenced. */

extern RegionPtr cfb16BitBlt();
extern void cfb16DoBitbltCopy();
extern void cfb16DoBitbltXor();
extern void cfb16DoBitbltOr();
extern void cfb16DoBitbltGeneral();
extern void cfb16CopyWindow();

extern RegionPtr cfb24BitBlt();
extern void cfb24DoBitbltCopy();
extern void cfb24DoBitbltXor();
extern void cfb24DoBitbltOr();
extern void cfb24DoBitbltGeneral();
extern void cfb24CopyWindow();

extern RegionPtr cfb32BitBlt();
extern void cfb32DoBitbltCopy();
extern void cfb32DoBitbltXor();
extern void cfb32DoBitbltOr();
extern void cfb32DoBitbltGeneral();
extern void cfb32CopyWindow();

static void CirrusLgBppDoBitbltCopy();


/*
 * These functions replaces the cfbXX CopyArea functions. They catch plain
 * on-screen BitBlts in order to do them with the Cirrus BitBLT engine.
 */

RegionPtr
CirrusLgCopyArea32(pSrcDrawable, pDstDrawable,
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
    if ((pGC->planemask & 0xFFFFFFFF) != 0xFFFFFFFF)
      doBitBlt = cfb32DoBitbltGeneral;

    if (doBitBlt == cfb32DoBitbltCopy && 
	pSrcDrawable->type == DRAWABLE_WINDOW && 
	pDstDrawable->type == DRAWABLE_WINDOW && xf86VTSema)
      doBitBlt = CirrusLgBppDoBitbltCopy;
    
    return cfb32BitBlt(pSrcDrawable, pDstDrawable, pGC, srcx, srcy, 
		       width, height, dstx, dsty, doBitBlt, 0L);
}

RegionPtr
CirrusLgCopyArea24(pSrcDrawable, pDstDrawable,
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
    if ((pGC->planemask & 0xFFFFFF) != 0xFFFFFF)
      doBitBlt = cfb24DoBitbltGeneral;

    if (doBitBlt == cfb24DoBitbltCopy && 
	pSrcDrawable->type == DRAWABLE_WINDOW && 
	pDstDrawable->type == DRAWABLE_WINDOW && xf86VTSema)
      doBitBlt = CirrusLgBppDoBitbltCopy;
    
    return cfb24BitBlt(pSrcDrawable, pDstDrawable, pGC, srcx, srcy, 
		       width, height, dstx, dsty, doBitBlt, 0L);
}

RegionPtr
CirrusLgCopyArea16(pSrcDrawable, pDstDrawable,
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
    if ((pGC->planemask & 0xFFFF) != 0xFFFF)
      doBitBlt = cfb16DoBitbltGeneral;

    if (doBitBlt == cfb16DoBitbltCopy && 
	pSrcDrawable->type == DRAWABLE_WINDOW && 
	pDstDrawable->type == DRAWABLE_WINDOW && xf86VTSema)
      doBitBlt = CirrusLgBppDoBitbltCopy;
    
    return cfb16BitBlt(pSrcDrawable, pDstDrawable, pGC, srcx, srcy, 
		       width, height, dstx, dsty, doBitBlt, 0L);
}


/*
 * This function replaces the ScreenRec CopyWindow function. It does the
 * plain BitBlt window moves with the Cirrus BitBLT engine.
 */

extern WindowPtr *WindowTable;

void 
CirrusLgCopyWindow(pWin, ptOldOrg, prgnSrc)
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
        case 8 :
            vga256CopyWindow(pWin, ptOldOrg, prgnSrc);
	    return;
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
    REGION_INTERSECT(pWin->drawable.pScreen, &rgnDst, 
		     &pWin->borderClip, prgnSrc);

    pbox = REGION_RECTS(&rgnDst);
    nbox = REGION_NUM_RECTS(&rgnDst);
    if(!nbox || 
       !(pptSrc = (DDXPointPtr )ALLOCATE_LOCAL(nbox * sizeof(DDXPointRec))))
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

    CirrusLgBppDoBitbltCopy((DrawablePtr)pwinRoot, (DrawablePtr)pwinRoot,
	    GXcopy, &rgnDst, pptSrc, ~0L);
    DEALLOCATE_LOCAL(pptSrc);
    REGION_UNINIT(pWin->drawable.pScreen, &rgnDst);
}





/*
 * This is the local CirrusBppDoBitbltCopy function that handles 16/32-bit
 * pixel on-screen BitBlts, dispatching them to CirrusLgBppPolyBitblt.
 */
 
static void
CirrusLgBppDoBitbltCopy(pSrc, pDst, alu, prgnDst, pptSrc, planemask)
    DrawablePtr	    pSrc, pDst;
    int		    alu;
    RegionPtr	    prgnDst;
    DDXPointPtr	    pptSrc;
    unsigned long   planemask;
{
  unsigned long *psrcBase, *pdstBase;	
				/* start of src and dst bitmaps */

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
  
  CirrusLgScreen2Screen(pdstBase, psrcBase, 0, 0, nbox,
			pptSrc, pbox, xdir, ydir, GXcopy);

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

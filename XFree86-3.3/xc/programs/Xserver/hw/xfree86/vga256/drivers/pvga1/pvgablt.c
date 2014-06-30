/* $XConsortium: pvgablt.c /main/10 1996/02/21 18:06:45 kaleb $ */
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
/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/pvga1/pvgablt.c,v 3.8 1996/12/23 06:58:07 dawes Exp $ */
/*
 * cfb copy area
 */


/* WD90C31 code: Mike Tierney <floyd@eng.umd.edu> */
/* WD90C33 accel code: Bill Morgart <wsm@morticia.ssw.com> */


#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"cfb.h"
#include	"cfbmskbits.h"
#include	"cfb8bit.h"
#include	"fastblt.h"
#include	"mergerop.h"
#include        "vgaBank.h"

#include "compiler.h"
#include "paradise.h"


void
pvgacfbDoBitbltCopy(pSrc, pDst, alu, prgnDst, pptSrc, planemask)
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
    int blit_dir;
    unsigned char *src, *dst;

    MROP_DECLARE_REG()

    int careful;
    int srcaddr, dstaddr;


    MROP_INITIALIZE(alu,planemask);

    src = (unsigned char *)psrcBase;
    cfbGetByteWidthAndPointer (pSrc, widthSrc, src)
    psrcBase = (unsigned long *)src;

    dst = (unsigned char *)pdstBase;
    cfbGetByteWidthAndPointer (pDst, widthDst, dst)
    pdstBase = (unsigned long *)dst;

    BANK_FLAG_BOTH(psrcBase,pdstBase)

    /* XXX we have to err on the side of safety when both are windows,
     * because we don't know if IncludeInferiors is being used.
     */
    careful = ((pSrc == pDst) ||
	       ((pSrc->type == DRAWABLE_WINDOW) &&
		(pDst->type == DRAWABLE_WINDOW)));

    if (!CHECKSCREEN(psrcBase) || !CHECKSCREEN(pdstBase))
    {
       pvga1_stdcfbDoBitbltCopy (pSrc, pDst, alu, prgnDst, pptSrc, planemask);
       return;
    }
    

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

    if (careful && (pptSrc->x < pbox->x1) && (pptSrc->y <= pbox->y1))
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

    blit_dir = ((xdir == -1 || ydir == -1) ? BLT_DIRECT : 0);

    while(nbox--)
    {
	w = pbox->x2 - pbox->x1;
	h = pbox->y2 - pbox->y1;

	switch (WDchipset)
	{
	case WD90C31:
	case WD90C24:
#if 0
	  /* Not sure why this causes problems (DHD) */
	  wd90c31BitBlt((unsigned char *)psrcBase, (unsigned char *)pdstBase,
			widthSrc, widthDst,
			pptSrc->x, pptSrc->y,
			pbox->x1, pbox->y1,
			w, h,
			xdir, ydir,
			ROP_SRC >> 8,
			planemask,
			BLT_SRC_COLR,
			0);
#else
	  if (xdir == -1 || ydir == -1)
	  {
	    dstaddr = (pbox->y1+h-1)*widthDst + pbox->x1+w-1;
	    srcaddr = (pptSrc->y+h-1)*widthSrc + pptSrc->x+w-1;
	  }
	  else
	  {
	    dstaddr = pbox->y1*widthDst + pbox->x1;
	    srcaddr = pptSrc->y*widthSrc + pptSrc->x;
	  }

	  /** handle the blit, this could actually handle and ROP **/
	  WAIT_BLIT;
	  SET_BLT_SRC_LOW ((srcaddr & 0xFFF));
	  SET_BLT_SRC_HGH (((srcaddr >> 12) & 0xFF));
	  SET_BLT_DST_LOW ((dstaddr & 0xFFF));
	  SET_BLT_DST_HGH (((dstaddr >> 12) & 0xFF));
	  SET_BLT_ROW_PTCH (widthDst);
	  SET_BLT_DIM_X   (w);
	  SET_BLT_DIM_Y   (h);
	  SET_BLT_MASK    ((planemask & 0xFF));
	  SET_BLT_RAS_OP  (ROP_SRC);
	  SET_BLT_CNTRL2  (0x00);
	  SET_BLT_CNTRL1  ((BLT_ACT_STAT | BLT_PACKED | BLT_SRC_COLR | blit_dir));
#endif
	  break;
	case WD90C33:
	  wd90c33BitBlt((unsigned char *)psrcBase, (unsigned char *)pdstBase,
			widthSrc, widthDst,
			pptSrc->x, pptSrc->y,
			pbox->x1, pbox->y1,
			w, h,
			xdir, ydir,
			ROP_SRC >> 8,
			planemask,
			COLOR_FORMAT,
			0);
	  break;
	}

        pbox++;
        pptSrc++;
    }

    /* the c33 bitblt code does its own waiting */
    if (WDchipset == WD90C31 || WDchipset == WD90C24)
      WAIT_BLIT; /* must wait, since memory writes can mess up as well */

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

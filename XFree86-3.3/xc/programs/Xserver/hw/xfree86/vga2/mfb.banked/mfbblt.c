/* $XFree86: xc/programs/Xserver/hw/xfree86/vga2/mfb.banked/mfbblt.c,v 3.5 1996/12/23 06:55:23 dawes Exp $ */
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
/* $XConsortium: mfbblt.c /main/4 1996/02/21 18:01:01 kaleb $ */

/*
 * mfb copy area
 */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"mfb.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"maskbits.h"
#include	"fastblt.h"
#include	"mergerop.h"

void
#ifdef TWO_BANKS_COPY
MROP_NAME(mfbDoBitbltTwoBanks)(pSrc, pDst, alu, prgnDst, pptSrc)
#else
MROP_NAME(mfbDoBitblt)(pSrc, pDst, alu, prgnDst, pptSrc)
#endif
    DrawablePtr	    pSrc, pDst;
    int		    alu;
    RegionPtr	    prgnDst;
    DDXPointPtr	    pptSrc;
{
    PixelType *psrcBase, *pdstBase;	
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

    PixelType *psrcLine, *pdstLine;	
				/* pointers to line with current src and dst */
    register PixelType *psrc;/* pointer to current src longword */
    register PixelType *pdst;/* pointer to current dst longword */

    MROP_DECLARE_REG()

				/* following used for looping through a line */
    PixelType startmask, endmask;	/* masks for writing ends of dst */
    int nlMiddle;		/* whole longwords in dst */
    int xoffSrc, xoffDst;
    register int leftShift, rightShift;
    register PixelType bits;
    register PixelType bits1;
    register int nl;		/* temp copy of nlMiddle */

				/* place to store full source word */
    int nstart;			/* number of ragged bits at start of dst */
    int nend;			/* number of ragged bits at end of dst */
    int srcStartOver;		/* pulling nstart bits from src
				   overflows into the next word? */
    int careful;
    int tmpSrc;

    MROP_INITIALIZE(alu,0);

    mfbGetPixelWidthAndPointer(pSrc, widthSrc, psrcBase);

    mfbGetPixelWidthAndPointer(pDst, widthDst, pdstBase);

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

    while(nbox--)
    {
      if (((unsigned long)VGABASE==(unsigned long)psrcBase)
          &&((unsigned long)VGABASE==(unsigned long)pdstBase))
      /* Both src and dst are on the VGA, do banking */
      {
	w = pbox->x2 - pbox->x1;
	h = pbox->y2 - pbox->y1;

	if (ydir == -1) /* start at last scanline of rectangle */
	{
	    psrcLine = mfbScanlineDeltaSrc(psrcBase, -(pptSrc->y+h-1), widthSrc);
	    pdstLine = mfbScanlineDeltaDst(pdstBase, -(pbox->y2-1), widthDst);
	}
	else /* start at first scanline */
	{
	    psrcLine = mfbScanlineDeltaSrc(psrcBase, pptSrc->y, widthSrc);
	    pdstLine = mfbScanlineDeltaDst(pdstBase, pbox->y1, widthDst);
	}
	if ((pbox->x1 & PIM) + w <= PPW)
	{
	    maskpartialbits (pbox->x1, w, startmask);
	    endmask = 0;
	    nlMiddle = 0;
	}
	else
	{
	    maskbits(pbox->x1, w, startmask, endmask, nlMiddle);
	}
	if (xdir == 1)
	{
	    xoffSrc = pptSrc->x & PIM;
	    xoffDst = pbox->x1 & PIM;
	    pdstLine += (pbox->x1 >> PWSH);
	    psrcLine += (pptSrc->x >> PWSH);

	    if (xoffSrc == xoffDst)
	    {
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
		    if (startmask)
		    {
			bits = *psrc;
			READ_WRITE_DST();
			*pdst = MROP_MASK(bits, *pdst, startmask);
			READ_SRC_WRITE_DST();
			psrc++;
			pdst++;
		    }
		    nl = nlMiddle;

#ifdef TWO_BANKS_COPY
		    DuffL(nl, label1,
			    *pdst = MROP_SOLID (*psrc,dummy/*notused*/);
			    pdst++; psrc++;)
#else
		    DuffL(nl, label1,
			    bits = *psrc;
			    READ_WRITE_DST();
			    *pdst = MROP_SOLID (bits, *pdst);
			    READ_SRC_WRITE_DST();
			    pdst++; psrc++;)
#endif
		    if (endmask)
		    {
			bits = *psrc;
			READ_WRITE_DST();
			*pdst = MROP_MASK(bits, *pdst, endmask);
			READ_SRC_WRITE_DST();
		    }
		    if (h)
		    {
			mfbScanlineIncDst(pdstLine, widthDst);
			mfbScanlineIncSrc(psrcLine, widthSrc);
		    }
		}
	    }
	    else
	    {
		if (xoffSrc > xoffDst)
		{
		    leftShift = (xoffSrc - xoffDst);
		    rightShift = PPW - leftShift;
		}
		else
		{
		    rightShift = (xoffDst - xoffSrc);
		    leftShift = PPW - rightShift;
		}
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
		    bits = 0;
		    if (xoffSrc > xoffDst)
			bits = *psrc++;
		    if (startmask)
		    {
			bits1 = BitLeft(bits,leftShift);
			if (BitLeft(startmask, rightShift)) {
				bits = *psrc++;
				bits1 |= BitRight(bits,rightShift);
			}
			READ_WRITE_DST();
			*pdst = MROP_MASK(bits1, *pdst, startmask);
			READ_SRC_WRITE_DST();
			pdst++;
		    }
		    nl = nlMiddle;

#ifdef TWO_BANKS_COPY
		    DuffL (nl,label2,
			bits1 = BitLeft(bits, leftShift);
			bits = *psrc++;
			*pdst = MROP_SOLID(bits1 | BitRight(bits, rightShift),
					   dummy/*notused*/);
			pdst++;
		    )
#else
		    DuffL (nl,label2,
			bits1 = BitLeft(bits, leftShift);
			bits = *psrc++;
			READ_WRITE_DST();
			*pdst = MROP_SOLID (bits1 | BitRight(bits, rightShift), *pdst);
			READ_SRC_WRITE_DST();
			pdst++;
		    )
#endif

		    if (endmask)
		    {
			bits1 = BitLeft(bits, leftShift);
			if (BitLeft(endmask, rightShift))
			{
			    bits = *psrc;
			    bits1 |= BitRight(bits, rightShift);
			}
			READ_WRITE_DST();
			*pdst = MROP_MASK (bits1, *pdst, endmask);
			READ_SRC_WRITE_DST();
		    }
		    if (h)
		    {
			mfbScanlineIncDst(pdstLine, widthDst);
			mfbScanlineIncSrc(psrcLine, widthSrc);
		    }
		}
	    }
	  }
	  else	/* xdir == -1 */
	  {
	    xoffSrc = (pptSrc->x + w - 1) & PIM;
	    xoffDst = (pbox->x2 - 1) & PIM;
	    /* this can wrap around to the next line */
	    /* First access will be with pre-decrement -- PHB 8/94 */
	    mfbScanlineIncDst(pdstLine, ((pbox->x2-1) >> PWSH) );
	    pdstLine++;
	    mfbScanlineIncSrc(psrcLine, ((pptSrc->x+w - 1) >> PWSH) );
	    psrcLine++;

	    if (xoffSrc == xoffDst)
	    {
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
		    if (endmask)
		    {
			pdst--;
			psrc--;
			bits = *psrc;
			READ_WRITE_DST();
			*pdst = MROP_MASK (bits, *pdst, endmask);
			READ_SRC_WRITE_DST();
		    }
		    nl = nlMiddle;

#ifdef TWO_BANKS_COPY
		    DuffL(nl,label3,
			--pdst; --psrc;
			*pdst = MROP_SOLID (*psrc, dummy/*notused*/);
		    )
#else
		    DuffL(nl,label3,
			--pdst; --psrc;
			bits = *psrc;
			READ_WRITE_DST();
			*pdst = MROP_SOLID (bits, *pdst);
			READ_SRC_WRITE_DST();
		    )
#endif

		    if (startmask)
		    {
			--pdst;
			--psrc;
			bits = *psrc;
			READ_WRITE_DST();
			*pdst = MROP_MASK(bits, *pdst, startmask);
			READ_SRC_WRITE_DST();
		    }
		    if (h)
		    {
			/* this can wrap around to the next line */
			/* First access will be with pre-decrement -- PHB 8/94 */
			pdstLine--; mfbScanlineIncDst(pdstLine, widthDst);
			pdstLine++;
			psrcLine--; mfbScanlineIncSrc(psrcLine, widthSrc);
			psrcLine++;
		    }
		}
	    }
	    else
	    {
		if (xoffDst > xoffSrc)
		{
		    rightShift = (xoffDst - xoffSrc);
		    leftShift = PPW - rightShift;
		}
		else
		{
		    leftShift = (xoffSrc - xoffDst);
		    rightShift = PPW - leftShift;
		}
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
		    bits = 0;
		    if (xoffDst > xoffSrc)
			bits = *--psrc;
		    if (endmask)
		    {
			bits1 = BitRight(bits, rightShift);
			if (BitRight(endmask, leftShift)) {
				bits = *--psrc;
				bits1 |= BitLeft(bits, leftShift);
			}
			pdst--;
			READ_WRITE_DST();
			*pdst = MROP_MASK(bits1, *pdst, endmask);
			READ_SRC_WRITE_DST();
		    }
		    nl = nlMiddle;

#ifdef TWO_BANKS_COPY
		    DuffL (nl, label4,
			bits1 = BitRight(bits, rightShift);
			bits = *--psrc;
			--pdst;
			*pdst = MROP_SOLID(bits1 | BitLeft(bits, leftShift),
					   dummy/*notused*/);
		    )
#else
		    DuffL (nl, label4,
			bits1 = BitRight(bits, rightShift);
			bits = *--psrc;
			--pdst;
			READ_WRITE_DST();
			*pdst = MROP_SOLID(bits1 | BitLeft(bits, leftShift),*pdst);
			READ_SRC_WRITE_DST();
		    )
#endif

		    if (startmask)
		    {
			bits1 = BitRight(bits, rightShift);
			if (BitRight (startmask, leftShift))
			{
			    bits = *--psrc;
			    bits1 |= BitLeft(bits, leftShift);
			}
			--pdst;
			READ_WRITE_DST();
			*pdst = MROP_MASK(bits1, *pdst, startmask);
			READ_SRC_WRITE_DST();
		    }
		    if (h)
		    {
			/* this can wrap around to the next line */
			/* First access will be with pre-decrement -- PHB 8/94 */
			pdstLine--; mfbScanlineIncDst(pdstLine, widthDst);
			pdstLine++;
			psrcLine--; mfbScanlineIncSrc(psrcLine, widthSrc);
			psrcLine++;
		    }
		}
	    }
	}
      } else /* max. one of src and dst is on the VGA */
      {
	w = pbox->x2 - pbox->x1;
	h = pbox->y2 - pbox->y1;

	if (ydir == -1) /* start at last scanline of rectangle */
	{
	    psrcLine = mfbScanlineDelta(psrcBase, -(pptSrc->y+h-1), widthSrc);
	    pdstLine = mfbScanlineDelta(pdstBase, -(pbox->y2-1), widthDst);
	}
	else /* start at first scanline */
	{
	    psrcLine = mfbScanlineDelta(psrcBase, pptSrc->y, widthSrc);
	    pdstLine = mfbScanlineDelta(pdstBase, pbox->y1, widthDst);
	}
	if ((pbox->x1 & PIM) + w <= PPW)
	{
	    maskpartialbits (pbox->x1, w, startmask);
	    endmask = 0;
	    nlMiddle = 0;
	}
	else
	{
	    maskbits(pbox->x1, w, startmask, endmask, nlMiddle);
	}
	if (xdir == 1)
	{
	    xoffSrc = pptSrc->x & PIM;
	    xoffDst = pbox->x1 & PIM;
	    pdstLine += (pbox->x1 >> PWSH);
	    psrcLine += (pptSrc->x >> PWSH);

	    if (xoffSrc == xoffDst)
	    {
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
		    if (startmask)
		    {
			*pdst = MROP_MASK(*psrc, *pdst, startmask);
			psrc++;
			pdst++;
		    }
		    nl = nlMiddle;

		    DuffL(nl, label1a,
			    *pdst = MROP_SOLID (*psrc, *pdst);
			    pdst++; psrc++;)

		    if (endmask)
			*pdst = MROP_MASK(*psrc, *pdst, endmask);
		    if (h)
		    {
			mfbScanlineInc(pdstLine, widthDst);
			mfbScanlineInc(psrcLine, widthSrc);
		    }
		}
	    }
	    else
	    {
		if (xoffSrc > xoffDst)
		{
		    leftShift = (xoffSrc - xoffDst);
		    rightShift = PPW - leftShift;
		}
		else
		{
		    rightShift = (xoffDst - xoffSrc);
		    leftShift = PPW - rightShift;
		}
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
		    bits = 0;
		    if (xoffSrc > xoffDst)
			bits = *psrc++;
		    if (startmask)
		    {
			bits1 = BitLeft(bits,leftShift);
			if (BitLeft(startmask, rightShift)) {
				bits = *psrc++;
				bits1 |= BitRight(bits,rightShift);
			}
			*pdst = MROP_MASK(bits1, *pdst, startmask);
			pdst++;
		    }
		    nl = nlMiddle;

		    DuffL (nl,label2a,
			bits1 = BitLeft(bits, leftShift);
			bits = *psrc++;
			*pdst = MROP_SOLID (bits1 | BitRight(bits, rightShift), *pdst);
			pdst++;
		    )

		    if (endmask)
		    {
			bits1 = BitLeft(bits, leftShift);
			if (BitLeft(endmask, rightShift))
			{
			    bits = *psrc;
			    bits1 |= BitRight(bits, rightShift);
			}
			*pdst = MROP_MASK (bits1, *pdst, endmask);
		    }
		    if (h)
		    {
			mfbScanlineInc(pdstLine, widthDst);
			mfbScanlineInc(psrcLine, widthSrc);
		    }
		}
	    }
	  }
	  else	/* xdir == -1 */
	  {
	    xoffSrc = (pptSrc->x + w - 1) & PIM;
	    xoffDst = (pbox->x2 - 1) & PIM;
	    pdstLine += ((pbox->x2-1) >> PWSH) + 1;
	    psrcLine += ((pptSrc->x+w - 1) >> PWSH) + 1;

	    if (xoffSrc == xoffDst)
	    {
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
		    if (endmask)
		    {
			pdst--;
			psrc--;
			*pdst = MROP_MASK (*psrc, *pdst, endmask);
		    }
		    nl = nlMiddle;

		    DuffL(nl,label3a,
			--pdst; --psrc;
			*pdst = MROP_SOLID (*psrc, *pdst);
		    )

		    if (startmask)
		    {
			--pdst;
			--psrc;
			*pdst = MROP_MASK(*psrc, *pdst, startmask);
		    }
		    if (h)
		    {
			mfbScanlineInc(pdstLine, widthDst);
			mfbScanlineInc(psrcLine, widthSrc);
		    }
		}
	    }
	    else
	    {
		if (xoffDst > xoffSrc)
		{
		    rightShift = (xoffDst - xoffSrc);
		    leftShift = PPW - rightShift;
		}
		else
		{
		    leftShift = (xoffSrc - xoffDst);
		    rightShift = PPW - leftShift;
		}
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
		    bits = 0;
		    if (xoffDst > xoffSrc)
			bits = *--psrc;
		    if (endmask)
		    {
			bits1 = BitRight(bits, rightShift);
			if (BitRight(endmask, leftShift)) {
				bits = *--psrc;
				bits1 |= BitLeft(bits, leftShift);
			}
			pdst--;
			*pdst = MROP_MASK(bits1, *pdst, endmask);
		    }
		    nl = nlMiddle;

		    DuffL (nl, label4a,
			bits1 = BitRight(bits, rightShift);
			bits = *--psrc;
			--pdst;
			*pdst = MROP_SOLID(bits1 | BitLeft(bits, leftShift),*pdst);
		    )

		    if (startmask)
		    {
			bits1 = BitRight(bits, rightShift);
			if (BitRight (startmask, leftShift))
			{
			    bits = *--psrc;
			    bits1 |= BitLeft(bits, leftShift);
			}
			--pdst;
			*pdst = MROP_MASK(bits1, *pdst, startmask);
		    }
		    if (h)
		    {
			mfbScanlineInc(pdstLine, widthDst);
			mfbScanlineInc(psrcLine, widthSrc);
		    }
		}
	    }
	}
      } /* if both on VGA */
      pbox++;
      pptSrc++;
    } /* while (nbox--) */
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

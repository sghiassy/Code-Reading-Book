/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgablt.c,v 3.5 1997/01/12 10:45:33 dawes Exp $ */
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
/* $XConsortium: vgablt.c /main/4 1996/02/21 18:10:30 kaleb $ */
/*
 * cfb copy area
 */


#include	"vga256.h"
#include	"fastblt.h"
#include	"mergerop.h"

#if PGSZ == 32
#define LEFTSHIFT_AMT (5 - PWSH)
#else /* PGSZ == 64 */
#define LEFTSHIFT_AMT (6 - PWSH)
#endif /* PGSZ */

#ifdef notdef /* XXX fails right now, walks off end of pixmaps */
#if defined (FAST_UNALIGNED_READS) && (PSZ == 8)
#define DO_UNALIGNED_BITBLT
#endif
#endif

#if (MROP == Mcopy)
#ifdef SPEEDUP
void
speedupvga256DoBitbltCopy(pSrc, pDst, alu, prgnDst, pptSrc, planemask, bitPlane)
#else
void
MROP_NAME(vga256DoBitblt)(pSrc, pDst, alu, prgnDst, pptSrc, planemask, bitPlane)
#endif
#else
void
MROP_NAME(vga256DoBitblt)(pSrc, pDst, alu, prgnDst, pptSrc, planemask, bitPlane)
#endif
    DrawablePtr	    pSrc, pDst;
    int		    alu;
    RegionPtr	    prgnDst;
    DDXPointPtr	    pptSrc;
    unsigned long   planemask;
    unsigned long   bitPlane;
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

    unsigned long *psrcLine, *pdstLine;	
				/* pointers to line with current src and dst */
    register unsigned long *psrc;/* pointer to current src longword */
    register unsigned long *pdst;/* pointer to current dst longword */

    MROP_DECLARE_REG()

				/* following used for looping through a line */
    unsigned long startmask, endmask;	/* masks for writing ends of dst */
    int nlMiddle;		/* whole longwords in dst */
    int xoffSrc, xoffDst;
    register int leftShift, rightShift;
    register unsigned long bits;
    register unsigned long bits1;
    register int nl;		/* temp copy of nlMiddle */

				/* place to store full source word */
    int careful;

#ifdef SPEEDUP
    void SpeedUpBitBlt();
#endif

#ifndef SPEEDUP
#if 0
    /*
     * If nothing is going to touch the frame buffer, then just use
     * the regular cfb routines. They should be faster.
     */
    /*
     * Doing this causes a problem saving/restoring the screen on VT switch.
     */
    if ( (pSrc->type != DRAWABLE_WINDOW) &&
	 (pDst->type != DRAWABLE_WINDOW))
	{
	MROP_NAME(cfbDoBitblt)(pSrc, pDst, alu, prgnDst, pptSrc, planemask);
	return;
	}
#endif
#endif

    MROP_INITIALIZE(alu,planemask);

    cfbGetLongWidthAndPointer (pSrc, widthSrc, psrcBase)

    cfbGetLongWidthAndPointer (pDst, widthDst, pdstBase)

    BANK_FLAG_BOTH(psrcBase,pdstBase)

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
	w = pbox->x2 - pbox->x1;
	h = pbox->y2 - pbox->y1;

#if (MROP) == Mcopy && defined(SPEEDUP)
        if ((planemask&0xFF) == 0xFF) {
            if (h | w)
                SpeedUpBitBlt(psrcBase, pdstBase, widthSrc << 2, widthDst << 2,
                             pptSrc->x, pptSrc->y, pbox->x1, pbox->y1,
                             w, h, xdir, ydir);
            pbox++;
            pptSrc++;
            continue;
        }
#endif


	if (ydir == -1) /* start at last scanline of rectangle */
	{
	    psrcLine = psrcBase + ((pptSrc->y+h-1) * -widthSrc);
	    pdstLine = pdstBase + ((pbox->y2-1) * -widthDst);
	}
	else /* start at first scanline */
	{
	    psrcLine = psrcBase + (pptSrc->y * widthSrc);
	    pdstLine = pdstBase + (pbox->y1 * widthDst);
	}
	if ((pbox->x1 & PIM) + w <= PPW)
	{
	    maskpartialbits (pbox->x1, w, endmask);
	    startmask = 0;
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
		    SETW(pdst);
		    SETR(psrc);
		    pdstLine += widthDst;
		    psrcLine += widthSrc;
		    if (startmask)
		    {
		        bits = *psrc;
		        PUSHR();
			*pdst = MROP_MASK(bits, *pdst, startmask);
			pdst++; CHECKWO(pdst);
			POPR();
			psrc++; CHECKRO(psrc);
		    }
		    nl = nlMiddle;

		    DuffL(nl, label1,
			    bits = *psrc;
			    PUSHR();
			    *pdst = MROP_SOLID (bits, *pdst);
			    pdst++; CHECKWO(pdst);
			    POPR();
			    psrc++; CHECKRO(psrc); )

		    if (endmask)
		      {
			bits = *psrc;
			PUSHR();
			*pdst = MROP_MASK(bits, *pdst, endmask);
			POPR();
		      }
		}
	    }
	    else
	    {
		if (xoffSrc > xoffDst)
		{
		    leftShift = (xoffSrc - xoffDst) << LEFTSHIFT_AMT;
		    rightShift = PGSZ - leftShift;
		}
		else
		{
		    rightShift = (xoffDst - xoffSrc) << LEFTSHIFT_AMT;
		    leftShift = PGSZ - rightShift;
		}
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
		    SETW(pdst);
		    SETR(psrc);
		    pdstLine += widthDst;
		    psrcLine += widthSrc;
		    bits = 0;
		    if (xoffSrc > xoffDst)
		      {
			bits = *psrc++;
			CHECKRO(psrc);
		      }
		    if (startmask)
		    {
			bits1 = BitLeft(bits,leftShift);
			bits = *psrc++;
			CHECKRO(psrc);
			bits1 |= BitRight(bits,rightShift);
			PUSHR();
			*pdst = MROP_MASK(bits1, *pdst, startmask);
			pdst++;
			CHECKWO(pdst);
			POPR();
		    }
		    nl = nlMiddle;

		    DuffL (nl,label2,
			bits1 = BitLeft(bits, leftShift);
			bits = *psrc++; CHECKRO(psrc);
			PUSHR();
			*pdst = MROP_SOLID (bits1 | BitRight(bits, rightShift),
                                            *pdst);
			pdst++; CHECKWO(pdst);
			POPR();
		    )

		    if (endmask)
		    {
			bits1 = BitLeft(bits, leftShift);
			if (BitLeft(endmask, rightShift))
			{
			    bits = *psrc;
			    bits1 |= BitRight(bits, rightShift);
			}
			PUSHR();
			*pdst = MROP_MASK (bits1, *pdst, endmask);
			POPR();
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
		    SETW(pdst);
		    SETR(psrc);
		    pdstLine += widthDst;
		    psrcLine += widthSrc;
		    if (endmask)
		    {
			psrc--; CHECKRU(psrc);
			bits = *psrc;
			PUSHR();
			pdst--; CHECKRWU(pdst);
			*pdst = MROP_MASK (bits, *pdst, endmask);
			POPR();
		    }
		    nl = nlMiddle;

		    DuffL(nl,label3,
			 --psrc; CHECKRU(psrc); 
			 bits = *psrc;
			 PUSHR();
			 --pdst; CHECKRWU(pdst);
			 *pdst = MROP_SOLID (bits, *pdst);
			 POPR();)

		    if (startmask)
		    {
			--psrc; CHECKRU(psrc);
			bits = *psrc;
			PUSHR();
			--pdst; CHECKRWU(pdst);
			*pdst = MROP_MASK(bits, *pdst, startmask);
			POPR();
		    }
		}
	    }
	    else
	    {
		if (xoffDst > xoffSrc)
		{
		    rightShift = (xoffDst - xoffSrc) << LEFTSHIFT_AMT;
		    leftShift = PGSZ - rightShift;
		}
		else
		{
		    leftShift = (xoffSrc - xoffDst) << LEFTSHIFT_AMT;
		    rightShift = PGSZ - leftShift;
		}
		while (h--)
		{
		    psrc = psrcLine;
		    pdst = pdstLine;
		    SETW(pdst);
		    SETR(psrc);
		    pdstLine += widthDst;
		    psrcLine += widthSrc;
		    bits = 0;
		    if (xoffDst > xoffSrc) {
		        --psrc; CHECKRU(psrc);
			bits = *psrc;
		      }
		    if (endmask)
		    {
			bits1 = BitRight(bits, rightShift);
			--psrc; CHECKRU(psrc);
			bits = *psrc;
			bits1 |= BitLeft(bits, leftShift);
			PUSHR();
			pdst--; CHECKRWU(pdst);
			*pdst = MROP_MASK(bits1, *pdst, endmask);
			POPR();
		    }
		    nl = nlMiddle;

		    DuffL (nl, label4,
			bits1 = BitRight(bits, rightShift);
			--psrc; CHECKRU(psrc);
			bits = *psrc;
			PUSHR();
			--pdst; CHECKRWU(pdst);
			*pdst = MROP_SOLID(bits1 | BitLeft(bits, leftShift),
                                           *pdst);
			POPR();
		    )

		    if (startmask)
		    {
			bits1 = BitRight(bits, rightShift);
			if (BitRight (startmask, leftShift))
			{
			    --psrc; CHECKRU(psrc);
			    bits = *psrc;
			    bits1 |= BitLeft(bits, leftShift);
			}
			PUSHR();
			--pdst; CHECKRWU(pdst);
			*pdst = MROP_MASK(bits1, *pdst, startmask);
			POPR();
		    }
		}
	    }
	}
	pbox++;
	pptSrc++;
    }
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

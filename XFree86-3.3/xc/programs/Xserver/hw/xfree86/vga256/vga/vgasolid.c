/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgasolid.c,v 3.4 1996/12/23 07:00:00 dawes Exp $ */
/*
 *
Copyright (c) 1990  X Consortium

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
 *
 * Author:  Keith Packard, MIT X Consortium
 */
/* $XConsortium: vgasolid.c /main/3 1996/02/21 18:11:58 kaleb $ */


#include "vga256.h"
#include "cfbrrop.h"
#include "mispans.h"

void
#if (RROP == GXcopy)
#ifdef SPEEDUP
speedupvga256FillRectSolidCopy (pDrawable, pGC, nBox, pBox)
#else
vga256FillRectSolidCopy (pDrawable, pGC, nBox, pBox)
#endif
#else
RROP_NAME(vga256FillRectSolid) (pDrawable, pGC, nBox, pBox)
#endif
    DrawablePtr	    pDrawable;
    GCPtr	    pGC;
    int		    nBox;
    BoxPtr	    pBox;
{
    RROP_DECLARE
#ifdef SPEEDUP
    unsigned long   *pdstBase, *pdstRect;
    int		    h;
    int		    w;
    int		    widthDst;

    cfbGetLongWidthAndPointer (pDrawable, widthDst, pdstBase)
#endif /* SPEEDUP */

    RROP_FETCH_GC(pGC)
#ifdef SPEEDUP    
#if (RROP == GXcopy)
    for (; nBox; nBox--, pBox++)
    {
    	pdstRect = pdstBase + pBox->y1 * widthDst;
    	h = pBox->y2 - pBox->y1;
	w = pBox->x2 - pBox->x1;

        SpeedUpBox((unsigned char*)pdstRect + pBox->x1,
			      rrop_xor, h, w, widthDst << PWSH);
    }
#endif /* GXcopy */
#else /* SPEEDUP */
#if RROP == GXcopy
    vga256FillBoxSolid (pDrawable, nBox, pBox, rrop_xor, 0, GXcopy);
#endif /* GXcopy */
#endif /* SPEEDUP */
#if RROP == GXxor
    vga256FillBoxSolid (pDrawable, nBox, pBox, rrop_xor, 0, GXxor);
#endif /* GXxor */
#if RROP == GXor
    vga256FillBoxSolid (pDrawable, nBox, pBox, rrop_or, 0, GXor);
#endif /* GXor */
#if RROP == GXand
    vga256FillBoxSolid (pDrawable, nBox, pBox, rrop_and, 0, GXand);
#endif /* GXand */
#if RROP == GXset
    vga256FillBoxSolid (pDrawable, nBox, pBox, rrop_and, rrop_xor, GXset);
#endif /* GXset */
}

#ifndef SPEEDUP

void
RROP_NAME(vga256SolidSpans) (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		nInit;			/* number of spans to fill */
    DDXPointPtr pptInit;		/* pointer to list of start points */
    int		*pwidthInit;		/* pointer to list of n widths */
    int 	fSorted;
{
    unsigned long   *pdstBase;
    int		    widthDst;

    RROP_DECLARE
    
    register unsigned long  *pdst EDI;
    int	    nlmiddle, nl;
    register unsigned long  startmask, endmask;
    int	    w;
    int			    x;
    
				/* next three parameters are post-clip */
    int		    n;		/* number of spans to fill */
    DDXPointPtr	    ppt;	/* pointer to list of start points */
    int		    *pwidthFree;/* copies of the pointers to free */
    DDXPointPtr	    pptFree;
    int		    *pwidth;
    cfbPrivGCPtr    devPriv;

    devPriv = cfbGetGCPrivate(pGC);
    RROP_FETCH_GCPRIV(devPriv)
    n = nInit * miFindMaxBand(devPriv->pCompositeClip);
    pwidthFree = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    pptFree = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!pptFree || !pwidthFree)
    {
	if (pptFree) DEALLOCATE_LOCAL(pptFree);
	if (pwidthFree) DEALLOCATE_LOCAL(pwidthFree);
	return;
    }
    pwidth = pwidthFree;
    ppt = pptFree;
    n = miClipSpans(devPriv->pCompositeClip,
		     pptInit, pwidthInit, nInit,
		     ppt, pwidth, fSorted);

    cfbGetLongWidthAndPointer (pDrawable, widthDst, pdstBase)

    BANK_FLAG(pdstBase)

    CLD;

    while (n--)
    {
	x = ppt->x;
	pdst = pdstBase + (ppt->y * widthDst);
	++ppt;
	w = *pwidth++;
	if (!w)
	    continue;
	if (w <= PGSZB)
	{
	    register char   *addrb;

	    addrb = ((char *) pdst) + x;
	    SETRW(addrb);
	    while (w--)
	    {
		RROP_SOLID (addrb);
		addrb++; CHECKRWO(addrb);
	    }
	}
	else
	{
	    pdst += x >> PWSH;
	    SETRW(pdst);
	    maskbits (x, w, startmask, endmask, nlmiddle);
	    if (startmask)
	    {
		RROP_SOLID_MASK (pdst, startmask);
		++pdst; CHECKRWO(pdst);
	    }
	    
	    RROP_SPAN_STD(pdst,nlmiddle,SO_1);
	    CHECKRWO(pdst);

	    if (endmask)
	    {
	        CHECKRWO(pdst);
		RROP_SOLID_MASK (pdst, endmask);
	    }
	}
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}
#endif /* SPEEDUP */

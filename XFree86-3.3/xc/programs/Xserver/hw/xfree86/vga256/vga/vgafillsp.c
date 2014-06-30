/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgafillsp.c,v 3.4 1996/12/23 06:59:41 dawes Exp $ */
/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or X Consortium
not be used in advertising or publicity pertaining to 
distribution  of  the software  without specific prior 
written permission. Sun and X Consortium make no 
representations about the suitability of this software for 
any purpose. It is provided "as is" without any express or 
implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/

/***********************************************************

Copyright (c) 1987  X Consortium

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


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $XConsortium: vgafillsp.c /main/4 1996/02/21 18:10:53 kaleb $ */

#include "vga256.h"
#include "mergerop.h"

#define MFB_CONSTS_ONLY
#include "maskbits.h"

#include "mispans.h"

extern void mfbInvertSolidFS(), mfbBlackSolidFS(), mfbWhiteSolidFS();

/* scanline filling for color frame buffer
   written by drewry, oct 1986 modified by smarks
   changes for compatibility with Little-endian systems Jul 1987; MIT:yba.

   these routines all clip.  they assume that anything that has called
them has already translated the points (i.e. pGC->miTranslate is
non-zero, which is howit gets set in cfbCreateGC().)

   the number of new scnalines created by clipping ==
MaxRectsPerBand * nSpans.

    FillSolid is overloaded to be used for OpaqueStipple as well,
if fgPixel == bgPixel.  
Note that for solids, PrivGC.rop == PrivGC.ropOpStip


    FillTiled is overloaded to be used for OpaqueStipple, if
fgPixel != bgPixel.  based on the fill style, it uses
{RotatedTile, gc.alu} or {RotatedStipple, PrivGC.ropOpStip}
*/

/* Fill spans with tiles that aren't 32 bits wide */
void
vga256UnnaturalTileFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable;
GC		*pGC;
int		nInit;		/* number of spans to fill */
DDXPointPtr pptInit;		/* pointer to list of start points */
int *pwidthInit;		/* pointer to list of n widths */
int fSorted;
{
    int n;			/* number of spans to fill */
    register DDXPointPtr ppt;	/* pointer to list of start points */
    register int *pwidth;	/* pointer to list of n widths */
    void    (*fill)();
    int	xrot, yrot;

    if (!(pGC->planemask))
	return;

    if (pGC->tile.pixmap->drawable.width & PIM)
    {
    	fill = vga256FillSpanTileOddGeneral;
    	if ((pGC->planemask & PMSK) == PMSK)
    	{
	    if (pGC->alu == GXcopy)
	    	fill = vga256FillSpanTileOddCopy;
    	}
    }
    else
    {
	fill = vga256FillSpanTile32sGeneral;
    	if ((pGC->planemask & PMSK) == PMSK)
    	{
	    if (pGC->alu == GXcopy)
		fill = vga256FillSpanTile32sCopy;
	}
    }
    n = nInit * miFindMaxBand( cfbGetCompositeClip(pGC) );
    if ( n == 0 )
	return;
    pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
	if (ppt) DEALLOCATE_LOCAL(ppt);
	if (pwidth) DEALLOCATE_LOCAL(pwidth);
	return;
    }
    n = miClipSpans( cfbGetCompositeClip(pGC),
		     pptInit, pwidthInit, nInit, 
		     ppt, pwidth, fSorted);

    xrot = pDrawable->x + pGC->patOrg.x;
    yrot = pDrawable->y + pGC->patOrg.y;

    (*fill) (pDrawable, n, ppt, pwidth, pGC->tile.pixmap, xrot, yrot, pGC->alu, pGC->planemask);

    DEALLOCATE_LOCAL(ppt);
    DEALLOCATE_LOCAL(pwidth);
}

void
vga256UnnaturalStippleFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
DrawablePtr pDrawable;
GC		*pGC;
int		nInit;		/* number of spans to fill */
DDXPointPtr pptInit;		/* pointer to list of start points */
int *pwidthInit;		/* pointer to list of n widths */
int fSorted;
{
				/* next three parameters are post-clip */
    int		    n;		/* number of spans to fill */
    DDXPointPtr	    ppt;	/* pointer to list of start points */
    int		    *pwidth;	/* pointer to list of n widths */
    int		    *pwidthFree;/* copies of the pointers to free */
    DDXPointPtr	    pptFree;
    unsigned long   *pdstBase;	/* pointer to start of bitmap */
    int		    nlwDst;	/* width in longwords of bitmap */
    register unsigned long    *pdst;	/* pointer to current word in bitmap */
    PixmapPtr	    pStipple;	/* pointer to stipple we want to fill with */
    int		    nlw;
    int		    x, y, w, xrem, xSrc, ySrc;
    int		    stwidth, stippleWidth;
    int		    stippleHeight;
    register unsigned long  bits, inputBits;
    register int    partBitsLeft;
    int		    nextPartBits;
    int		    bitsLeft, bitsWhole;
    unsigned long   *srcTemp, *srcStart;
    unsigned long   *psrcBase;
    unsigned long   startmask, endmask;

    /*
     * If nothing is going to actually touch the framebuffer, then
     * do the work with the cfb routines as they should be slightly faster.
     */

    if( pDrawable->type != DRAWABLE_WINDOW )
	{
	cfbUnnaturalStippleFS(pDrawable, pGC, nInit, pptInit,
				pwidthInit, fSorted);
	return;
	}

    if (pGC->fillStyle == FillStippled)
	cfb8CheckStipple (pGC->alu, pGC->fgPixel, pGC->planemask);
    else
	cfb8CheckOpaqueStipple (pGC->alu, pGC->fgPixel, pGC->bgPixel, pGC->planemask);

    if (cfb8StippleRRop == GXnoop)
	return;

    n = nInit * miFindMaxBand( cfbGetCompositeClip(pGC) );
    if ( n == 0 )
	return;
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
    n = miClipSpans( cfbGetCompositeClip(pGC),
		     pptInit, pwidthInit, nInit, 
		     ppt, pwidth, fSorted);

    /*
     *  OK,  so what's going on here?  We have two Drawables:
     *
     *  The Stipple:
     *		Depth = 1
     *		Width = stippleWidth
     *		Words per scanline = stwidth
     *		Pointer to pixels = pStipple->devPrivate.ptr
     */

    pStipple = pGC->stipple;

    stwidth = pStipple->devKind >> PWSH;
    stippleWidth = pStipple->drawable.width;
    stippleHeight = pStipple->drawable.height;
    psrcBase = (unsigned long *) pStipple->devPrivate.ptr;

    /*
     *	The Target:
     *		Depth = PSZ
     *		Width = determined from *pwidth
     *		Words per scanline = nlwDst
     *		Pointer to pixels = addrlBase
     */

    cfbGetLongWidthAndPointer (pDrawable, nlwDst, pdstBase)

    BANK_FLAG(pdstBase)

    /* this replaces rotating the stipple. Instead we just adjust the offset
     * at which we start grabbing bits from the stipple.
     * Ensure that ppt->x - xSrc >= 0 and ppt->y - ySrc >= 0,
     * so that iline and xrem always stay within the stipple bounds.
     */

    modulus (pGC->patOrg.x, stippleWidth, xSrc);
    xSrc += pDrawable->x - stippleWidth;
    modulus (pGC->patOrg.y, stippleHeight, ySrc);
    ySrc += pDrawable->y - stippleHeight;

    bitsWhole = stippleWidth;

    while (n--)
    {
	x = ppt->x;
	y = ppt->y;
	ppt++;
	w = *pwidth++;
	pdst = pdstBase + y * nlwDst + (x >> PWSH);
	SETRW(pdst);
	y = (y - ySrc) % stippleHeight;
	srcStart = psrcBase + y * stwidth;
	xrem = ((x & ~(PGSZB-1)) - xSrc) % stippleWidth;
	srcTemp = srcStart + (xrem >> MFB_PWSH);
	bitsLeft = stippleWidth - (xrem & ~MFB_PIM);
	xrem &= MFB_PIM;
	NextUnnaturalStippleWord
	if (partBitsLeft < xrem)
	    FatalError ("vga256UnnaturalStippleFS bad partBitsLeft %d xrem %d",
			partBitsLeft, xrem);
	NextSomeBits (inputBits, xrem);
	partBitsLeft -= xrem;
	if (((x & PIM) + w) <= PPW)
	{
	    maskpartialbits (x, w, startmask)
	    NextUnnaturalStippleBits
	    *pdst = MaskRRopPixels(*pdst,bits,startmask);
	}
	else
	{
	    maskbits (x, w, startmask, endmask, nlw);
	    nextPartBits = (x & (PGSZB-1)) + w;
	    if (nextPartBits < partBitsLeft)
	    {
		if (startmask)
		{
		    MaskRRopBitGroup(pdst,GetBitGroup(inputBits),startmask)
		    pdst++; CHECKRWO(pdst);
		    NextBitGroup (inputBits);
		}
		while (nlw--)
		{
		    RRopBitGroup (pdst, GetBitGroup (inputBits));
		    pdst++; CHECKRWO(pdst);
		    NextBitGroup (inputBits);
		}
		if (endmask)
		{
		    MaskRRopBitGroup(pdst,GetBitGroup(inputBits),endmask)
		}
	    }
	    else if (bitsLeft != bitsWhole && nextPartBits < partBitsLeft + bitsLeft)
	    {
	    	NextUnnaturalStippleBitsFast
	    	if (startmask)
	    	{
		    *pdst = MaskRRopPixels(*pdst,bits,startmask);
		    pdst++; CHECKRWO(pdst);
	    	    NextUnnaturalStippleBitsFast
	    	}
	    	while (nlw--)
	    	{
		    *pdst = RRopPixels(*pdst,bits);
		    pdst++; CHECKRWO(pdst);
	    	    NextUnnaturalStippleBitsFast
	    	}
	    	if (endmask)
		    *pdst = MaskRRopPixels (*pdst,bits,endmask);
	    }
	    else
	    {
	    	NextUnnaturalStippleBits
	    	if (startmask)
	    	{
		    *pdst = MaskRRopPixels(*pdst,bits,startmask);
		    pdst++; CHECKRWO(pdst);
	    	    NextUnnaturalStippleBits
	    	}
	    	while (nlw--)
	    	{
		    *pdst = RRopPixels(*pdst,bits);
		    pdst++; CHECKRWO(pdst);
	    	    NextUnnaturalStippleBits
	    	}
	    	if (endmask)
		    *pdst = MaskRRopPixels(*pdst,bits,endmask);
	    }
	}
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}

void
vga2568Stipple32FS (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		nInit;			/* number of spans to fill */
    DDXPointPtr pptInit;		/* pointer to list of start points */
    int		*pwidthInit;		/* pointer to list of n widths */
    int 	fSorted;
{
				/* next three parameters are post-clip */
    int		    n;			/* number of spans to fill */
    DDXPointPtr	    ppt;		/* pointer to list of start points */
    int		    *pwidth;		/* pointer to list of n widths */
    unsigned long   *src;		/* pointer to bits in stipple, if needed */
    int		    stippleHeight;	/* height of the stipple */
    PixmapPtr	    stipple;

    int		    nlwDst;		/* width in longwords of the dest pixmap */
    int		    x,y,w;		/* current span */
    unsigned long   startmask;
    unsigned long   endmask;
    register unsigned long *dst;	/* pointer to bits we're writing */
    register int    nlw;
    unsigned long   *dstTmp;
    int		    nlwTmp;

    unsigned long   *pbits;		/* pointer to start of pixmap */
    register unsigned long  xor;
    register unsigned long  mask;
    register unsigned long  bits;	/* bits from stipple */
    int		    wEnd;

    int		    *pwidthFree;	/* copies of the pointers to free */
    DDXPointPtr	    pptFree;
    cfbPrivGCPtr    devPriv;

    /*
     * If nothing is going to actually touch the framebuffer, then
     * do the work with the cfb routines as they should be slightly faster.
     */

    if( pDrawable->type != DRAWABLE_WINDOW )
	{
	cfb8Stipple32FS (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted);
	return;
	}

    devPriv = cfbGetGCPrivate(pGC);
    cfb8CheckStipple (pGC->alu, pGC->fgPixel, pGC->planemask);
    n = nInit * miFindMaxBand(devPriv->pCompositeClip);
    if ( n == 0 )
	return;
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

    stipple = devPriv->pRotatedPixmap;
    src = (unsigned long *)stipple->devPrivate.ptr;
    stippleHeight = stipple->drawable.height;

    cfbGetLongWidthAndPointer (pDrawable, nlwDst, pbits)

    BANK_FLAG(pbits)

    while (n--)
    {
    	w = *pwidth++;
	x = ppt->x;
    	y = ppt->y;
	ppt++;
    	dst = pbits + (y * nlwDst) + (x >> PWSH);
	if (((x & PIM) + w) <= PPW)
	{
	    maskpartialbits(x, w, startmask);
	    endmask = 0;
	    nlw = 0;
	}
	else
	{
	    maskbits (x, w, startmask, endmask, nlw);
	}
	bits = src[y % stippleHeight];
	RotBitsLeft (bits, (x & ((PGSZ-1) & ~PIM)));
#if PPW == 4
	if (cfb8StippleRRop == GXcopy)
	{
	    SETRW(dst);
	    xor = devPriv->xor;
	    if (w < (PGSZ*2))
	    {
		if (startmask)
		{
		    mask = cfb8PixelMasks[GetBitGroup(bits)];
		    *dst = (*dst & ~(mask & startmask)) |
			   (xor & (mask & startmask));
		    dst++; CHECKRWO(dst);
		    RotBitsLeft (bits, PGSZB);
		}
		while (nlw--)
		{
		    WriteBitGroup (dst,xor,GetBitGroup(bits))
		    dst++; CHECKRWO(dst);
		    RotBitsLeft (bits, PGSZB);
		}
		if (endmask)
		{
		    mask = cfb8PixelMasks[GetBitGroup(bits)];
		    *dst = (*dst & ~(mask & endmask)) |
			   (xor & (mask & endmask));
		}
	    }
	    else
	    {
		wEnd = 7 - (nlw & 7);
		nlw = (nlw >> 3) + 1;
		dstTmp = dst;
		nlwTmp = nlw;
		if (startmask)
		{
		    mask = cfb8PixelMasks[GetBitGroup(bits)];
		    *dstTmp = (*dstTmp & ~(mask & startmask)) |
			   (xor & (mask & startmask));
		    dstTmp++; CHECKRWO(dstTmp);
		    RotBitsLeft (bits, PGSZB);
		}
		w = 7 - wEnd;
		SAVE_BANK();
		while (w--)
		{
		    dst = dstTmp;
		    RESTORE_BANK();
		    SAVE_BANK();
		    CHECKRWO(dst);
		    dstTmp++;
		    nlw = nlwTmp;
		    mask = cfb8PixelMasks[GetBitGroup(bits)];
		    xor = xor & mask;
		    mask = ~mask;
		    while (nlw--)
		    {
			*dst = (*dst & mask) | xor;
			dst += 8; CHECKRWO(dst);
		    }
		    xor = devPriv->xor;
		    NextBitGroup (bits);
		}
		nlwTmp--;
		w = wEnd + 1;
		if (endmask)
		{
		    mask = cfb8PixelMasks[GetBitGroup(bits)];
		    dst = dstTmp + (nlwTmp << 3);
		    RESTORE_BANK();
		    SAVE_BANK();
		    CHECKRWO(dst);
		    *dst = (*dst & ~(mask & endmask)) |
			   (xor &  (mask & endmask));
		}
		while (w--)
		{
		    nlw = nlwTmp;
		    dst = dstTmp;
		    RESTORE_BANK();
		    SAVE_BANK();
		    CHECKRWO(dst);
		    dstTmp++;
		    mask = cfb8PixelMasks[GetBitGroup(bits)];
		    xor = xor & mask;
		    mask = ~mask;
		    while (nlw--)
		    {
			*dst = (*dst & mask) | xor;
			dst += 8; CHECKRWO(dst);
		    }
		    xor = devPriv->xor;
		    NextBitGroup (bits);
		}
	    }
	}
	else
#endif /* PPW == 4 */
	{
	    SETRW(dst);
	    if (startmask)
	    {
		xor = GetBitGroup(bits);
		*dst = MaskRRopPixels(*dst, xor, startmask);
		dst++; CHECKRWO(dst);
		RotBitsLeft (bits, PGSZB);
	    }
	    while (nlw--)
	    {
		RRopBitGroup(dst, GetBitGroup(bits));
		dst++; CHECKRWO(dst);
		RotBitsLeft (bits, PGSZB);
	    }
	    if (endmask)
	    {
		xor = GetBitGroup(bits);
		*dst = MaskRRopPixels(*dst, xor, endmask);
	    }
	}
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}

void
vga2568OpaqueStipple32FS (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		nInit;			/* number of spans to fill */
    DDXPointPtr pptInit;		/* pointer to list of start points */
    int		*pwidthInit;		/* pointer to list of n widths */
    int 	fSorted;
{
				/* next three parameters are post-clip */
    int		    n;			/* number of spans to fill */
    DDXPointPtr	    ppt;		/* pointer to list of start points */
    int		    *pwidth;		/* pointer to list of n widths */
    unsigned long   *src;		/* pointer to bits in stipple, if needed */
    int		    stippleHeight;	/* height of the stipple */
    PixmapPtr	    stipple;

    int		    nlwDst;		/* width in longwords of the dest pixmap */
    int		    x,y,w;		/* current span */
    unsigned long   startmask;
    unsigned long   endmask;
    register unsigned long *dst;	/* pointer to bits we're writing */
    register int    nlw;
    unsigned long   *dstTmp;
    int		    nlwTmp;

    unsigned long   *pbits;		/* pointer to start of pixmap */
    register unsigned long  xor;
    register unsigned long  bits;	/* bits from stipple */
    int		    wEnd;

    int		    *pwidthFree;	/* copies of the pointers to free */
    DDXPointPtr	    pptFree;
    cfbPrivGCPtr    devPriv;

    /*
     * If nothing is going to actually touch the framebuffer, then
     * do the work with the cfb routines as they should be slightly faster.
     */

    if( pDrawable->type != DRAWABLE_WINDOW )
	{
	cfb8OpaqueStipple32FS (pDrawable, pGC, nInit, pptInit,
				 pwidthInit, fSorted);
	return;
	}

    devPriv = cfbGetGCPrivate(pGC);

    cfb8CheckOpaqueStipple(pGC->alu, pGC->fgPixel, pGC->bgPixel, pGC->planemask);

    n = nInit * miFindMaxBand(devPriv->pCompositeClip);
    if ( n == 0 )
	return;
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

    stipple = devPriv->pRotatedPixmap;
    src = (unsigned long *)stipple->devPrivate.ptr;
    stippleHeight = stipple->drawable.height;

    cfbGetLongWidthAndPointer (pDrawable, nlwDst, pbits)

    BANK_FLAG(pbits)

    while (n--)
    {
    	w = *pwidth++;
	x = ppt->x;
    	y = ppt->y;
	ppt++;
    	dst = pbits + (y * nlwDst) + (x >> PWSH);
	if (((x & PIM) + w) <= PPW)
	{
	    maskpartialbits(x, w, startmask);
	    endmask = 0;
	    nlw = 0;
	}
	else
	{
	    maskbits (x, w, startmask, endmask, nlw);
	}
	bits = src[y % stippleHeight];
	RotBitsLeft (bits, (x & ((PGSZ-1) & ~PIM)));
#if PPW == 4
	if (cfb8StippleRRop == GXcopy)
	{
	    SETRW(dst);
	    xor = devPriv->xor;
	    if (w < PGSZ*2)
	    {
		if (startmask)
		{
		    *dst = *dst & ~startmask |
			    GetPixelGroup (bits) & startmask;
		    dst++; CHECKRWO(dst);
		    RotBitsLeft (bits, PGSZB);
		}
		while (nlw--)
		{
		    *dst++ = GetPixelGroup(bits);
		    RotBitsLeft (bits, PGSZB);
		    CHECKRWO(dst);
		}
		if (endmask)
		{
		    *dst = *dst & ~endmask |
			  GetPixelGroup (bits) & endmask;
		}
	    }
	    else
	    {
		wEnd = 7 - (nlw & 7);
		nlw = (nlw >> 3) + 1;
		dstTmp = dst;
		nlwTmp = nlw;
		if (startmask)
		{
		    *dstTmp = *dstTmp & ~startmask |
			   GetPixelGroup (bits) & startmask;
		    dstTmp++; CHECKRWO(dstTmp);
		    RotBitsLeft (bits, PGSZB);
		}
		w = 7 - wEnd;
		SAVE_BANK();
		while (w--)
		{
		    nlw = nlwTmp;
		    dst = dstTmp;
		    RESTORE_BANK();
		    SAVE_BANK();
		    CHECKRWO(dst);
		    dstTmp++;
		    xor = GetPixelGroup (bits);
		    while (nlw--)
		    {
			*dst = xor;
			dst += 8; CHECKRWO(dst);
		    }
		    NextBitGroup (bits);
		}
		nlwTmp--;
		w = wEnd + 1;
		if (endmask)
		{
		    dst = dstTmp + (nlwTmp << 3);
		    RESTORE_BANK();
		    SAVE_BANK();
		    CHECKRWO(dst);
		    *dst = (*dst & ~endmask) |
			   GetPixelGroup (bits) & endmask;
		}
		while (w--)
		{
		    nlw = nlwTmp;
		    dst = dstTmp;
		    RESTORE_BANK();
		    SAVE_BANK();
		    CHECKRWO(dst);
		    dstTmp++;
		    xor = GetPixelGroup (bits);
		    while (nlw--)
		    {
			*dst = xor;
			dst += 8; CHECKRWO(dst);
		    }
		    NextBitGroup (bits);
		}
	    }
	}
	else
#endif /* PPW == 4 */
	{
	    SETRW(dst);
	    if (startmask)
	    {
		xor = GetBitGroup(bits);
		*dst = MaskRRopPixels(*dst, xor, startmask);
		dst++; CHECKRWO(dst);
		RotBitsLeft (bits, PGSZB);
	    }
	    while (nlw--)
	    {
		RRopBitGroup(dst, GetBitGroup(bits));
		dst++; CHECKRWO(dst);
		RotBitsLeft (bits, PGSZB);
	    }
	    if (endmask)
	    {
		xor = GetBitGroup(bits);
		*dst = MaskRRopPixels(*dst, xor, endmask);
	    }
	}
    }
    DEALLOCATE_LOCAL(pptFree);
    DEALLOCATE_LOCAL(pwidthFree);
}

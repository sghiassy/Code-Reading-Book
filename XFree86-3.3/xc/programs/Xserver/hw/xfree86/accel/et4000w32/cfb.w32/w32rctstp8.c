/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/cfb.w32/w32rctstp8.c,v 3.10 1996/12/23 06:35:04 dawes Exp $ */
/*
 * Fill 32 bit stippled rectangles for 8 bit frame buffers
 */
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

Author: Keith Packard, MIT X Consortium

*/
/* $XConsortium: w32rctstp8.c /main/8 1996/10/25 14:10:19 kaleb $ */

#if PSZ == 8

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
#include "cfb8bit.h"

#define MFB_CONSTS_ONLY
#include "maskbits.h"
#include "w32stip.h"


#define STIPPLE   \
	while (nBox--)  \
	{  \
            long ACLDst;\
	    w = pBox->x2 - pBox->x1;  \
	    h = pBox->y2 - pBox->y1;  \
	    y = pBox->y1;  \
	    if (w == 0 || h == 0)  \
	    {  \
		pBox++;  \
		continue;  \
	    }  \
  \
  	    ACLDst = pBox->y1 * nlwDst + pBox->x1 * (PSZ >> 3); \
	    if (!W32et6000) \
	    { \
	        SET_XY(w, h)  \
	        START_ACL_CPU(ACLDst)  \
	    } \
	    else \
	    { \
	        SET_XY(w, 1) /* line-per-line */ \
	    } \
	    rot = pBox->x1 & (PGSZ-1);  \
	    pBox++;  \
	    y = y % stippleHeight;  \
	    w = (w + 7) >> 3;  \
	    w32_chunks = w >> 2;  \
	    w32_misc = w & 0x3;  \
  \
	    if (W32OrW32i)  \
		W32_STIPPLE  \
	    else if (W32et6000) \
	        ET6K_STIPPLE \
	    else /* W32p */ \
		W32P_STIPPLE  \
	}


void
W328FillRectOpaqueStippled32 (pDrawable, pGC, nBox, pBox)
    DrawablePtr	    pDrawable;
    GCPtr	    pGC;
    int		    nBox;	/* number of boxes to fill */
    register BoxPtr pBox;	/* pointer to list of boxes to fill */
{
    unsigned long   *src;
    int stippleHeight;

    int nlwDst;		/* width in longwords of the dest pixmap */
    int w;		/* width of current box */
    register int h;	/* height of current box */
    unsigned long startmask;
    unsigned long endmask;	/* masks for reggedy bits at either end of line */
    int nlwMiddle;	/* number of longwords between sides of boxes */
    register int nlw;			/* loop version of nlwMiddle */
    unsigned long *dstLine;
    register unsigned long *dst;	/* pointer to bits we're writing */
    unsigned long *dstTmp;
    int y;				/* current scan line */

    unsigned long *pbits;/* pointer to start of pixmap */
    register unsigned long bits;	/* bits from stipple */
    int	rot, lastStop, i;
    register unsigned long  xor, and;
    cfbPrivGCPtr	    devPriv;
    PixmapPtr		    stipple;
    int	    wEnd;

    int w32_chunks, w32_misc;

    cfbGetLongWidthAndPointer (pDrawable, nlwDst, pbits)
    if ((CARD32)pbits != VGABASE)
    {
	cfb8FillRectOpaqueStippled32 (pDrawable, pGC, nBox, pBox);
	return;
    }

    devPriv = cfbGetGCPrivate(pGC);
    stipple = devPriv->pRotatedPixmap;

    stippleHeight = stipple->drawable.height;
    src = (unsigned long *)stipple->devPrivate.ptr;

    if ((pGC->planemask & PMSK) == PMSK)
    {
	nlwDst <<= 2;
	if (W32OrW32i)
	    W32_INIT_OPAQUE_STIPPLE(pGC->alu,
				    PFILL(pGC->fgPixel), PFILL(pGC->bgPixel),
				    nlwDst - 1)
	else
	    W32P_INIT_OPAQUE_STIPPLE(pGC->alu,
				     PFILL(pGC->fgPixel), PFILL(pGC->bgPixel),
				     nlwDst - 1)
	STIPPLE
	return;
    }

    pbits = 0;
    cfb8CheckOpaqueStipple(pGC->alu, pGC->fgPixel, pGC->bgPixel, pGC->planemask);

    while (nBox--)
    {
	w = pBox->x2 - pBox->x1;
	h = pBox->y2 - pBox->y1;
	y = pBox->y1;
	dstLine = pbits + (pBox->y1 * nlwDst) + ((pBox->x1 & ~PIM) >> PWSH);
	if (((pBox->x1 & PIM) + w) <= PPW)
	{
	    maskpartialbits(pBox->x1, w, startmask);
	    nlwMiddle = 0;
	    endmask = 0;
	}
	else
	{
	    maskbits (pBox->x1, w, startmask, endmask, nlwMiddle);
	}
	rot = (pBox->x1 & ((PGSZ-1) & ~PIM));
	pBox++;
	y = y % stippleHeight;
	if (cfb8StippleRRop == GXcopy)
	{
	    if (w < PGSZ*2)
	    {
	    	while (h--)
	    	{
	    	    bits = src[y];
	    	    y++;
	    	    if (y == stippleHeight)
		    	y = 0;
	    	    if (rot)
		    	RotBitsLeft(bits,rot);
		    dst = dstLine;
		    W32_LONG(dst)
	    	    dstLine += nlwDst;
	    	    if (startmask)
	    	    {
		    	*dst = *dst & ~startmask |
				GetPixelGroup (bits) & startmask;
		    	dst++;
		    	RotBitsLeft (bits, PGSZB);
	    	    }
		    nlw = nlwMiddle;
	    	    while (nlw--)
	    	    {
			*dst++ = GetPixelGroup(bits);
		    	RotBitsLeft (bits, PGSZB);
	    	    }
	    	    if (endmask)
	    	    {
			*dst = *dst & ~endmask |
			      GetPixelGroup (bits) & endmask;
	    	    }
	    	}
	    }
	    else
	    {
		wEnd = 7 - (nlwMiddle & 7);
		nlwMiddle = (nlwMiddle >> 3) + 1;
	    	while (h--)
	    	{
		    bits = src[y];
		    y++;
		    if (y == stippleHeight)
			y = 0;
	    	    if (rot != 0)
			RotBitsLeft (bits, rot);
	    	    dstTmp = dstLine;
		    W32_LONG(dstTmp)
	    	    dstLine += nlwDst;
		    if (startmask)
		    {
			*dstTmp = *dstTmp & ~startmask |
			       GetPixelGroup (bits) & startmask;
			dstTmp++;
			RotBitsLeft (bits, PGSZB);
		    }
		    w = 7 - wEnd;
		    while (w--)
		    {
			nlw = nlwMiddle;
			dst = dstTmp;
			dstTmp++;
			xor = GetPixelGroup (bits);
			while (nlw--)
			{
			    *dst = xor;
			    dst += 8;
			}
			NextBitGroup (bits);
		    }
		    nlwMiddle--;
		    w = wEnd + 1;
		    if (endmask)
		    {
			dst = dstTmp + (nlwMiddle << 3);
			*dst = (*dst & ~endmask) |
			       GetPixelGroup (bits) & endmask;
		    }
		    while (w--)
		    {
			nlw = nlwMiddle;
			dst = dstTmp;
			dstTmp++;
			xor = GetPixelGroup (bits);
			while (nlw--)
			{
			    *dst = xor;
			    dst += 8;
			}
			NextBitGroup (bits);
		    }
		    nlwMiddle++;
		}
	    }
	}
	else
	{
	    while (h--)
	    {
	    	bits = src[y];
	    	y++;
	    	if (y == stippleHeight)
		    y = 0;
	    	if (rot)
		    RotBitsLeft(bits,rot);
		dst = dstLine;
		W32_LONG(dst)
	    	dstLine += nlwDst;
	    	if (startmask)
	    	{
		    xor = GetBitGroup(bits);
		    *dst = MaskRRopPixels(*dst, xor, startmask);
		    dst++;
		    RotBitsLeft (bits, PGSZB);
	    	}
		nlw = nlwMiddle;
	    	while (nlw--)
	    	{
		    RRopBitGroup(dst, GetBitGroup(bits));
		    dst++;
		    RotBitsLeft (bits, PGSZB);
	    	}
	    	if (endmask)
	    	{
		    xor = GetBitGroup(bits);
		    *dst = MaskRRopPixels(*dst, xor, endmask);
	    	}
	    }
	}
    }
}

void
W328FillRectTransparentStippled32 (pDrawable, pGC, nBox, pBox)
    DrawablePtr	    pDrawable;
    GCPtr	    pGC;
    int		    nBox;	/* number of boxes to fill */
    BoxPtr 	    pBox;	/* pointer to list of boxes to fill */
{
    int		    x, y, w, h;
    int		    nlwMiddle, nlwDst, nlwTmp;
    unsigned long   startmask, endmask;
    register unsigned long   *dst;
    unsigned long   *dstLine, *pbits, *dstTmp;
    unsigned long   *src;
    register unsigned long   xor;
    register unsigned long   bits, mask;
    int		    rot;
    int		    wEnd;
    cfbPrivGCPtr    devPriv;
    PixmapPtr	    stipple;
    int		    stippleHeight;
    register int    nlw;

    int w32_chunks, w32_misc;

    cfbGetLongWidthAndPointer (pDrawable, nlwDst, pbits)
    if ((CARD32)pbits != VGABASE)
    {
	cfb8FillRectTransparentStippled32 (pDrawable, pGC, nBox, pBox);
	return;
    }

    devPriv = cfbGetGCPrivate(pGC);
    stipple = devPriv->pRotatedPixmap;
    src = (unsigned long *)stipple->devPrivate.ptr;
    stippleHeight = stipple->drawable.height;

    nlwDst <<= 2;
    if (W32OrW32i)
	W32_INIT_TR_STIPPLE(pGC->alu, PFILL(pGC->planemask),
			    PFILL(pGC->fgPixel), PFILL(pGC->bgPixel),
			    nlwDst - 1)
    else
	W32P_INIT_TR_STIPPLE(pGC->alu, PFILL(pGC->planemask),
			     PFILL(pGC->fgPixel), PFILL(pGC->bgPixel),
			     nlwDst - 1)
    STIPPLE
}


void
W328FillRectStippledUnnatural (pDrawable, pGC, nBox, pBox)
    DrawablePtr	    pDrawable;
    GCPtr	    pGC;
    int		    nBox;
    register BoxPtr pBox;
{
    unsigned long   *pdstBase;	/* pointer to start of bitmap */
    unsigned long   *pdstLine;	/* current destination line */
    int		    nlwDst;	/* width in longwords of bitmap */
    PixmapPtr	    pStipple;	/* pointer to stipple we want to fill with */
    int		    nlwMiddle;
    register int    nlw;
    int		    x, y, w, h, xrem, xSrc, ySrc;
    int		    stwidth, stippleWidth;
    int		    stippleHeight;
    register unsigned long  bits, inputBits;
    register int    partBitsLeft;
    int		    nextPartBits;
    int		    bitsLeft, bitsWhole;
    register unsigned long    *pdst;	/* pointer to current word in bitmap */
    unsigned long   *srcTemp, *srcStart;
    unsigned long   *psrcBase;
    unsigned long   startmask, endmask;

    cfbGetLongWidthAndPointer (pDrawable, nlwDst, pdstBase)
    if ((CARD32)pdstBase != VGABASE)
    {
	cfb8FillRectStippledUnnatural (pDrawable, pGC, nBox, pBox);
	return;
    }
    pdstBase = 0;

    if (pGC->fillStyle == FillStippled)
	cfb8CheckStipple (pGC->alu, pGC->fgPixel, pGC->planemask);
    else
	cfb8CheckOpaqueStipple (pGC->alu, pGC->fgPixel, pGC->bgPixel, pGC->planemask);

    if (cfb8StippleRRop == GXnoop)
	return;

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

    xSrc = pDrawable->x;
    ySrc = pDrawable->y;

    /* this replaces rotating the stipple. Instead we just adjust the offset
     * at which we start grabbing bits from the stipple.
     * Ensure that ppt->x - xSrc >= 0 and ppt->y - ySrc >= 0,
     * so that iline and xrem always stay within the stipple bounds.
     */

    xSrc += (pGC->patOrg.x % stippleWidth) - stippleWidth;
    ySrc += (pGC->patOrg.y % stippleHeight) - stippleHeight;

    bitsWhole = stippleWidth;

    while (nBox--)
    {
	x = pBox->x1;
	y = pBox->y1;
	w = pBox->x2 - x;
	h = pBox->y2 - y;
	pBox++;
	pdstLine = pdstBase + y * nlwDst + (x >> PWSH);
	y = (y - ySrc) % stippleHeight;
	srcStart = psrcBase + y * stwidth;
	xrem = ((x & ~PIM) - xSrc) % stippleWidth;
	if (((x & PIM) + w) < PPW)
	{
	    maskpartialbits (x, w, startmask);
	    nlwMiddle = 0;
	    endmask = 0;
	}
	else
	{
	    maskbits (x, w, startmask, endmask, nlwMiddle);
	}
	while (h--)
	{
    	    srcTemp = srcStart + (xrem >> MFB_PWSH);
    	    bitsLeft = stippleWidth - (xrem & ~MFB_PIM);
	    NextUnnaturalStippleWord
	    NextSomeBits (inputBits, (xrem & MFB_PIM));
	    partBitsLeft -= (xrem & MFB_PIM);
	    NextUnnaturalStippleBits
	    nlw = nlwMiddle;
	    pdst = pdstLine;
	    W32_LONG(pdst)
	    if (startmask)
	    {
		*pdst = MaskRRopPixels(*pdst,bits,startmask);
		pdst++;
		NextUnnaturalStippleBits
	    }
	    while (nlw--)
	    {
		*pdst = RRopPixels(*pdst,bits);
		pdst++;
		NextUnnaturalStippleBits
	    }
	    if (endmask)
		*pdst = MaskRRopPixels(*pdst,bits,endmask);
	    pdstLine += nlwDst;
	    y++;
	    srcStart += stwidth;
	    if (y == stippleHeight)
	    {
		y = 0;
		srcStart = psrcBase;
	    }
	}
    }
}

#endif /* PSZ == 8 */

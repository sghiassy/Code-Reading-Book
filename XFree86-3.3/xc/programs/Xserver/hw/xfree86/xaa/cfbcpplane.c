/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/cfbcpplane.c,v 3.0 1996/11/18 13:22:05 dawes Exp $ */

/*
 * Copyright 1996  The XFree86 Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * HARM HANEMAAYER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 */

/*
 * CopyPlane1to16 and CopyPlane1to32 (and CopyPlane16to1, cfbCopyPlane32to1).
 * This code is taken from mach64/machblt.c. However, the mach64 version
 * was broken for a window destination. To fix this, there is now only
 * one version of CopyPlane1to16/32 and CopyPlane1to16/32 that is
 * compiled with PSZ == 16 and with PSZ == 32 like the rest of the cfb
 * code.
 *
 * This code is useful, not only because mi is slow at > 8bpp, but also
 * because a low-level generic bitmap expansion function is very handy
 * (for example, when uploading a stipple to the cache outside the visible
 * screen, you want to call a low-level function, avoiding mi and clipping
 * kludges).
 *
 * Actually, CopyPlane1to16 as taken from the mach64 code was actually
 * slower than what cfb manages. I have now optimized it along the lines
 * of the stippled spans function in cfbfillsp.c, which is a big win
 * especially for 16bpp.
 * This would also be the route to support 24bpp.
 * Note that these optimizations assume LSBFirst bitmap bit order.
 * I have some doubts about whether this is alpha-aware.
 *
 * Anyone want to add a CopyPlane1to24 for 24bpp?
 *
 * I've now added CopyPlane1to24, but it is only valid for GXcopy
 * and full planemask (which is enough for most purposes), and not
 * at all optimized (writing bytes at a time).
 */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"mi.h"
#include	"cfb.h"
#include	"cfbmskbits.h"
#include	"cfb8bit.h"
#include	"fastblt.h"


#define mfbmaskbits(x, w, startmask, endmask, nlw) \
    startmask = starttab[(x)&0x1f]; \
    endmask = endtab[((x)+(w)) & 0x1f]; \
    if (startmask) \
	nlw = (((w) - (32 - ((x)&0x1f))) >> 5); \
    else \
	nlw = (w) >> 5;

#define mfbmaskpartialbits(x, w, mask) \
    mask = partmasks[(x)&0x1f][(w)&0x1f];

#define LeftMost    0
#define StepBit(bit, inc)  ((bit) += (inc))


#define GetBits(psrc, nBits, curBit, bitPos, bits) {\
    bits = 0; \
    while (nBits--) \
    { \
	bits |= ((*psrc++ >> bitPos) & 1) << curBit; \
	StepBit (curBit, 1); \
    } \
}

/******************************************************************/

void
#if PSZ == 16
cfbCopyPlane1to16 (pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc, 
		   planemask, bitPlane, bg, fg)
#endif
#if PSZ == 32
cfbCopyPlane1to32 (pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc, 
		   planemask, bitPlane, bg, fg)
#endif
#if PSZ == 24
cfbCopyPlane1to24 (pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc, 
		   planemask, bitPlane, bg, fg)
#endif
    DrawablePtr pSrcDrawable;
    DrawablePtr pDstDrawable;
    int	rop;
    unsigned long planemask;
    RegionPtr prgnDst;
    DDXPointPtr pptSrc;
    int         bitPlane;
    int bg;
    int fg;
{
    int	srcx, srcy, dstx, dsty;
    int width, height;
    int xoffSrc;
    unsigned long *psrcBase, *pdstBase;
    int	widthSrc, widthDst;
    unsigned int *psrcLine;
    register unsigned int *psrc;
#if PSZ == 16
    unsigned short *pdstLine;
    register unsigned short *pdst;
#endif
#if PSZ == 32
    unsigned int *pdstLine;
    register unsigned int *pdst;
#endif
#if PSZ == 24
    unsigned char *pdstLine;
    register unsigned char *pdst;
#endif
    register unsigned int  bits, tmp;
    register unsigned int  fgpixel, bgpixel;
    register unsigned int  src;
    register int  leftShift, rightShift;
    register int  i, nl;
    int nbox;
    BoxPtr pbox;
    int  result;
#if PSZ == 16
    unsigned int doublet[4];	/* Pixel values for 16bpp expansion. */
#endif
#if PSZ == 32
    unsigned int doublet[8];	/* Pixel values for 32bpp expansion */
#endif

    fgpixel = fg & planemask;
    bgpixel = bg & planemask;

#if PSZ == 16
    if (rop == GXcopy && (planemask & PMSK) == PMSK) {
        doublet[0] = bgpixel | (bgpixel << 16);
        doublet[1] = fgpixel | (bgpixel << 16);
        doublet[2] = bgpixel | (fgpixel << 16);
        doublet[3] = fgpixel | (fgpixel << 16);
    }
#endif
#if PSZ == 32
    if (rop == GXcopy && (planemask & PMSK) == PMSK) {
        doublet[0] = bgpixel; doublet[1] = bgpixel;
        doublet[2] = fgpixel; doublet[3] = bgpixel;
        doublet[4] = bgpixel; doublet[5] = fgpixel;
        doublet[6] = fgpixel; doublet[7] = fgpixel;
    }
#endif

    /* must explicitly ask for "int" widths, as code below expects it */
    /* on some machines (Alpha), "long" and "int" are not the same size */
    cfbGetTypedWidthAndPointer (pSrcDrawable, widthSrc, psrcBase, int, unsigned long)
    cfbGetTypedWidthAndPointer (pDstDrawable, widthDst, pdstBase, int, unsigned long)

#if PSZ == 16
    widthDst <<= 1;
#endif
#if PSZ == 24
    widthDst *= 3;
#endif

    nbox = REGION_NUM_RECTS(prgnDst);
    pbox = REGION_RECTS(prgnDst);

    while (nbox--)
    {
	dstx = pbox->x1;
	dsty = pbox->y1;
	srcx = pptSrc->x;
	srcy = pptSrc->y;
	width = pbox->x2 - pbox->x1;
	height = pbox->y2 - pbox->y1;
	pbox++;
	pptSrc++;
	psrcLine = (unsigned int *)psrcBase + srcy * widthSrc + (srcx >> 5);
#if PSZ == 16
	pdstLine = (unsigned short *)pdstBase + dsty * widthDst + dstx;
#endif
#if PSZ == 32
	pdstLine = (unsigned int *)pdstBase + dsty * widthDst + dstx;
#endif
#if PSZ == 24
	pdstLine = (unsigned char *)pdstBase + dsty * widthDst + dstx * 3;
#endif
	xoffSrc = srcx & 0x1f;

	/*
	 * compute constants for the first four bits to be
	 * copied.  This avoids troubles with partial first
	 * writes, and difficult shift computation
	 */
	leftShift = xoffSrc;
	rightShift = 32 - leftShift;

	if (rop == GXcopy && (planemask & PMSK) == PMSK)
	{
	    while (height--)
	    {
	        psrc = psrcLine;
	        pdst = pdstLine;
	        psrcLine += widthSrc;
	        pdstLine += widthDst;
	        bits = *psrc++;
	        nl = width;
   	        while (nl >= 32)
	        {
		    tmp = BitLeft(bits, leftShift);
		    bits = *psrc++;
		    if (rightShift != 32)
		        tmp |= BitRight(bits, rightShift);
		    i = 0;
#if PSZ == 32
		    while (i <= 28) {
		        int pair;
		        pair = (tmp >> i) & 0x03;
		        *pdst = doublet[pair * 2];
		        *(pdst + 1) = doublet[pair * 2 + 1];
		        pair = (tmp >> (i + 2)) & 0x03;
		        *(pdst + 2) = doublet[pair * 2];
		        *(pdst + 3) = doublet[pair * 2 + 1];
		        pdst += 4;
		        i += 4;
		    }
		    while (i < 32) {
		        *pdst = ((tmp >> i) & 0x01) ? fgpixel : bgpixel;
		        pdst++;
		        i++;
		    }
#endif
#if PSZ == 16
		    /*
		     * I've thrown in some optimization to at least write
		     * some aligned 32-bit words instead of 16-bit shorts.
		     */
		    if ((unsigned int)psrc & 2) {
		        /* Write unaligned 16-bit word at left edge. */
		        if (tmp & 0x01)
		            *pdst = fgpixel;
		        else
		            *pdst = bgpixel;
		        pdst++;
		        i++;
		    }
		    while (i <= 24)
		    {
		        unsigned tmpbits = tmp >> i;
		        *(unsigned int *)pdst = doublet[tmpbits & 0x03];
	        	*(unsigned int *)(pdst + 2) =
		            doublet[(tmpbits >> 2) & 0x03];
	        	*(unsigned int *)(pdst + 4) =
		            doublet[(tmpbits >> 4) & 0x03];
	        	*(unsigned int *)(pdst + 6) =
		            doublet[(tmpbits >> 6) & 0x03];
		        pdst += 8;	/* Advance four 32-bit words. */
		        i += 8;
		    }
		    while (i <= 30)
		    {
		        *(unsigned int *)pdst =
		            doublet[(tmp >> i) & 0x03];
		        pdst += 2;	/* Advance one 32-bit word. */
		        i += 2;
		    }
		    if (i == 31) {
		        if ((tmp >> 31) & 0x01)
		            *pdst = fgpixel;
		        else
		            *pdst = bgpixel;
		        pdst++;
		    }
#endif		    
#if PSZ == 24
		    while (i <= 31) {
		        if ((tmp >> i) & 0x01) {
		            *pdst = fgpixel;
		            *(pdst + 1) = fgpixel >> 8;
		            *(pdst + 2) = fgpixel >> 16;
		        }
		        else {
		            *pdst = bgpixel;
		            *(pdst + 1) = bgpixel >> 8;
		            *(pdst + 2) = bgpixel >> 16;
		        }
		        pdst += 3;
		        i++;
		    }
#endif
		    nl -= 32;
	        }

	        if (nl)
	        {
		    tmp = BitLeft(bits, leftShift);
		    /*
		     * better condition needed -- mustn't run
		     * off the end of the source...
		     */
		    if (rightShift != 32)
		    {
		        bits = *psrc++;
		        tmp |= BitRight (bits, rightShift);
		    }
		    i = 32;
		    while (nl--)
		    {
		        --i;
#if PSZ == 24
		        if ((tmp >> (31 - i)) & 0x01) {
		            *pdst = fgpixel;
		            *(pdst + 1) = fgpixel >> 8;
		            *(pdst + 2) = fgpixel >> 16;
		        }
		        else {
		            *pdst = bgpixel;
		            *(pdst + 1) = bgpixel >> 8;
		            *(pdst + 2) = bgpixel >> 16;
		        }
		        pdst += 3;
#else
		        *pdst = ((tmp >> (31 - i)) & 0x01) ? fgpixel : bgpixel;
		        pdst++;
#endif
		    }
	        }
            }
        }
#if PSZ != 24
	else
	{
	    while (height--)
	    {
	        psrc = psrcLine;
	        pdst = pdstLine;
	        psrcLine += widthSrc;
	        pdstLine += widthDst;
	        bits = *psrc++;
	        nl = width;
   	        while (nl >= 32)
	        {
		    tmp = BitLeft(bits, leftShift);
		    bits = *psrc++;
		    if (rightShift != 32)
		        tmp |= BitRight(bits, rightShift);
		    i = 32;
		    while (i--)
		    {
		        src = ((tmp >> (31 - i)) & 0x01) ? fgpixel : bgpixel;
                        DoRop (result, rop, src, *pdst);

		        *pdst = (*pdst & ~planemask) |
		  	        (result & planemask);
		        pdst++;
		    }
		    nl -= 32;
	        }

	        if (nl)
	        {
		    tmp = BitLeft(bits, leftShift);
		    /*
		     * better condition needed -- mustn't run
		     * off the end of the source...
		     */
		    if (rightShift != 32)
		    {
		        bits = *psrc++;
		        tmp |= BitRight (bits, rightShift);
		    }
		    i = 32;
		    while (nl--)
		    {
		        --i;
		        src = ((tmp >> (31 - i)) & 0x01) ? fgpixel : bgpixel;
                        DoRop (result, rop, src, *pdst);

		        *pdst = (*pdst & ~planemask) |
		    	    (result & planemask);
		        pdst++;
		    }
	        }
            }
        }
#endif	/* PSZ != 24 */
    }
}

#if PSZ != 24

void
#if PSZ == 16
cfbCopyPlane16to1 (pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc, 
		   planemask, bitPlane)
#endif
#if PSZ == 32
cfbCopyPlane32to1 (pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc, 
		   planemask, bitPlane)
#endif
    DrawablePtr pSrcDrawable;
    DrawablePtr pDstDrawable;
    int	rop;
    RegionPtr prgnDst;
    DDXPointPtr pptSrc;
    unsigned long planemask;
    unsigned long bitPlane;
{
    int			    srcx, srcy, dstx, dsty, width, height;
    unsigned long	    *psrcBase;
    unsigned long	    *pdstBase;
    int			    widthSrc, widthDst;
#if PSZ == 16
    unsigned short	    *psrcLine;
#endif
#if PSZ == 32
    unsigned int	    *psrcLine;
#endif
    unsigned int	    *pdstLine;
#if PSZ == 16
    register unsigned short *psrc;
#endif
#if PSZ == 32
    register unsigned int   *psrc;
#endif
    register unsigned int   *pdst;
    register int	    i;
    register int	    curBit;
    register int	    bitPos;
    register unsigned int   bits;
    unsigned int	    startmask, endmask;
    int			    niStart, niEnd;
    int			    bitStart, bitEnd;
    int			    nl, nlMiddle;
    int			    nbox;
    BoxPtr		    pbox;
    int result;

    extern int starttab[32], endtab[32];
    extern unsigned int partmasks[32][32];


    if (!(planemask & 1))
	return;

    /* must explicitly ask for "int" widths, as code below expects it */
    /* on some machines (Alpha), "long" and "int" are not the same size */
    cfbGetTypedWidthAndPointer (pSrcDrawable, widthSrc, psrcBase, int, unsigned long)
    cfbGetTypedWidthAndPointer (pDstDrawable, widthDst, pdstBase, int, unsigned long)

#if PSZ == 16
    widthSrc <<= 1;
#endif

    bitPos = ffs (bitPlane) - 1;

    nbox = REGION_NUM_RECTS(prgnDst);
    pbox = REGION_RECTS(prgnDst);
    while (nbox--)
    {
	dstx = pbox->x1;
	dsty = pbox->y1;
	srcx = pptSrc->x;
	srcy = pptSrc->y;
	width = pbox->x2 - pbox->x1;
	height = pbox->y2 - pbox->y1;
	pbox++;
	pptSrc++;
#if PSZ == 16
	psrcLine = (unsigned short *)psrcBase + srcy * widthSrc + srcx;
#endif
#if PSZ == 32
	psrcLine = (unsigned int *)psrcBase + srcy * widthSrc + srcx;
#endif
	pdstLine = (unsigned int *)pdstBase + dsty * widthDst + (dstx >> 5);
	if (dstx + width <= 32)
	{
	    mfbmaskpartialbits(dstx, width, startmask);
	    nlMiddle = 0;
	    endmask = 0;
	}
	else
	{
	    mfbmaskbits (dstx, width, startmask, endmask, nlMiddle);
	}
	if (startmask)
	{
	    niStart = 32 - (dstx & 0x1f);
	    bitStart = LeftMost;
	    StepBit (bitStart, (dstx & 0x1f));
	}
	if (endmask)
	{
	    niEnd = (dstx + width) & 0x1f;
	    bitEnd = LeftMost;
	}
	if (rop == GXcopy)
	{
	    while (height--)
	    {
	    	psrc = psrcLine;
	    	pdst = pdstLine;
	    	psrcLine += widthSrc;
	    	pdstLine += widthDst;
	    	if (startmask)
	    	{
		    i = niStart;
		    curBit = bitStart;
		    GetBits (psrc, i, curBit, bitPos, bits);

		    *pdst = (*pdst & ~startmask) | bits;
		    pdst++;
	    	}
	    	nl = nlMiddle;
	    	while (nl--)
	    	{
		    i = 32;
		    curBit = LeftMost;
		    GetBits (psrc, i, curBit, bitPos, bits);
		    *pdst++ = bits;
	    	}
	    	if (endmask)
	    	{
		    i = niEnd;
		    curBit = bitEnd;
		    GetBits (psrc, i, curBit, bitPos, bits);

		    *pdst = (*pdst & ~endmask) | bits;
	    	}
	    }
	}
	else
	{
	    while (height--)
	    {
	    	psrc = psrcLine;
	    	pdst = pdstLine;
	    	psrcLine += widthSrc;
	    	pdstLine += widthDst;
	    	if (startmask)
	    	{
		    i = niStart;
		    curBit = bitStart;
		    GetBits (psrc, i, curBit, bitPos, bits);
                    DoRop (result, rop, bits, *pdst);

		    *pdst = (*pdst & ~startmask) | 
			    (result & startmask);
		    pdst++;
	    	}
	    	nl = nlMiddle;
 	    	while (nl--)
	    	{
		    i = 32;
		    curBit = LeftMost;
		    GetBits (psrc, i, curBit, bitPos, bits);
                    DoRop (result, rop, bits, *pdst);
		    *pdst = result;
		    ++pdst;
	    	}
	    	if (endmask)
	    	{
		    i = niEnd;
		    curBit = bitEnd;
		    GetBits (psrc, i, curBit, bitPos, bits);
                    DoRop (result, rop, bits, *pdst);

		    *pdst = (*pdst & ~endmask) |
			    (result & endmask);
	    	}
	    }
	}
    }
}

#endif

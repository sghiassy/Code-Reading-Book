/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgaglblt8.c,v 3.4 1996/12/23 06:59:48 dawes Exp $ */
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
*/
/* $XConsortium: vgaglblt8.c /main/3 1996/02/21 18:11:13 kaleb $ */

/*
 * Poly glyph blt for 8 bit displays.  Accepts
 * an arbitrary font <= 32 bits wide, in Copy mode only.
 */

#include	"vga256.h"

#define BOX_OVERLAP(box1, box2, xoffset, yoffset) \
 	((box1)->x1 <= ((int) (box2)->x2 + (xoffset)) && \
 	 ((int) (box2)->x1 + (xoffset)) <= (box1)->x2 && \
	 (box1)->y1 <= ((int) (box2)->y2 + (yoffset)) && \
 	 ((int) (box2)->y1 + (yoffset)) <= (box1)->y2)

#define BOX_CONTAINS(box1, box2, xoffset, yoffset) \
 	((box1)->x1 <= ((int) (box2)->x1 + (xoffset)) && \
 	 ((int) (box2)->x2 + (xoffset)) <= (box1)->x2 && \
	 (box1)->y1 <= ((int) (box2)->y1 + (yoffset)) && \
 	 ((int) (box2)->y2 + (yoffset)) <= (box1)->y2)

#if GLYPHPADBYTES != 4
#define USE_LEFTBITS
#endif

#ifdef USE_LEFTBITS
typedef	unsigned char	*glyphPointer;
extern unsigned long endtab[];

#define GlyphBits(bits,width,dst)	getleftbits(bits,width,dst); \
					(dst) &= widthMask; \
					(bits) += widthGlyph;
#define GlyphBitsS(bits,width,dst,off)	GlyphBits(bits,width,dst); \
					dst = BitRight (dst, off);
#else
typedef CARD32	*glyphPointer;

#define GlyphBits(bits,width,dst)	dst = *bits++;
#define GlyphBitsS(bits,width,dst,off)	dst = BitRight(*bits++, off);
#endif

#ifdef GLYPHROP
#define vga256PolyGlyphBlt8	vga256PolyGlyphRop8
#define vga256PolyGlyphBlt8Clipped	vga256PolyGlyphRop8Clipped

#undef WriteBitGroup
#define WriteBitGroup(dst,pixel,bits)	RRopBitGroup(dst,bits)

#endif

static void vga256PolyGlyphBlt8Clipped();

#if defined(HAS_STIPPLE_CODE) && !defined(GLYPHROP) && !defined(USE_LEFTBITS)
#define USE_STIPPLE_CODE
#endif

#if defined(__GNUC__) && !defined(GLYPHROP) && (defined(mc68020) || defined(mc68000) || defined(__mc68000__)) && !defined(USE_LEFTBITS)
#include    <stip68kgnu.h>
#endif

#if PSZ == 24
#define DST_INC     3
#else
#define DST_INC     (PGSZB >> PWSH)
#endif

void
vga256PolyGlyphBlt8 (pDrawable, pGC, x, y, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int 	x, y;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    pointer	pglyphBase;	/* start of array of glyphs */
{
    register unsigned long  c;
#ifndef GLYPHROP
    register unsigned long  pixel;
#endif
    register unsigned long  *dst;
    register glyphPointer   glyphBits;
    register int	    xoff;

    FontPtr		pfont = pGC->font;
    CharInfoPtr		pci;
    unsigned long	*dstLine;
    unsigned long	*pdstBase;
    int			hTmp;
    int			bwidthDst;
    int			widthDst;
    int			h;
    BoxRec		bbox;		/* for clipping */
    int			w;
    RegionPtr		clip;
    BoxPtr		extents;
#ifdef USE_LEFTBITS
    int			widthGlyph;
    unsigned long	widthMask;
#endif
#ifndef STIPPLE
#ifdef USE_STIPPLE_CODE
    void		(*stipple)();
    extern void		stipplestack (), stipplestackte ();

    stipple = stipplestack;
    if (FONTCONSTMETRICS(pfont))
	stipple = stipplestackte;
#endif
#endif
    
    x += pDrawable->x;
    y += pDrawable->y;

    /* compute an approximate (but covering) bounding box */
    bbox.x1 = 0;
    if ((ppci[0]->metrics.leftSideBearing < 0))
	bbox.x1 = ppci[0]->metrics.leftSideBearing;
    h = nglyph - 1;
    w = ppci[h]->metrics.rightSideBearing;
    while (--h >= 0)
	w += ppci[h]->metrics.characterWidth;
    bbox.x2 = w;
    bbox.y1 = -FONTMAXBOUNDS(pfont,ascent);
    bbox.y2 = FONTMAXBOUNDS(pfont,descent);

    clip = cfbGetCompositeClip(pGC);
    extents = &clip->extents;

    if (!clip->data) 
    {
	if (!BOX_CONTAINS(extents, &bbox, x, y))
	{
	    if (BOX_OVERLAP (extents, &bbox, x, y))
		vga256PolyGlyphBlt8Clipped(pDrawable, pGC, x, y,
					nglyph, ppci, pglyphBase);
	    return;
	}
    }
    else
    {
    	/* check to make sure some of the text appears on the screen */
    	if (!BOX_OVERLAP (extents, &bbox, x, y))
	    return;
    
    	bbox.x1 += x;
    	bbox.x2 += x;
    	bbox.y1 += y;
    	bbox.y2 += y;
    
    	switch (RECT_IN_REGION(pGC->pScreen, clip, &bbox))
    	{
      	  case rgnPART:
	    vga256PolyGlyphBlt8Clipped(pDrawable, pGC, x, y,
					nglyph, ppci, pglyphBase);
      	  case rgnOUT:
	    return;
    	}
    }

#ifdef GLYPHROP
    cfb8CheckStipple (pGC->alu, pGC->fgPixel, pGC->planemask);
#else
    pixel = cfbGetGCPrivate(pGC)->xor;
#endif

    cfbGetTypedWidthAndPointer (pDrawable, bwidthDst, pdstBase, char, unsigned long)

    BANK_FLAG(pdstBase)

    widthDst = bwidthDst / PGSZB;
    while (nglyph--)
    {
	pci = *ppci++;
	glyphBits = (glyphPointer) FONTGLYPHBITS(pglyphBase,pci);
	xoff = x + pci->metrics.leftSideBearing;
	dstLine = pdstBase +
	          (y - pci->metrics.ascent) * widthDst + (xoff >> PWSH);
	x += pci->metrics.characterWidth;
	if (hTmp = pci->metrics.descent + pci->metrics.ascent)
	{
	    xoff &= PIM;
#ifdef STIPPLE
	    STIPPLE(dstLine,glyphBits,pixel,bwidthDst,hTmp,xoff);
#else
#ifdef USE_STIPPLE_CODE
	    (*stipple)(dstLine,glyphBits,pixel,bwidthDst,hTmp,xoff);
#else
#ifdef USE_LEFTBITS
	    widthGlyph = GLYPHWIDTHBYTESPADDED (pci);
	    widthMask = endtab[w];
#endif
	    do {
	    	dst = dstLine; SETRW(dst);
	    	dstLine = (unsigned long *) (((char *) dstLine) + bwidthDst);
	    	GlyphBits(glyphBits, w, c)
	    	WriteBitGroup(dst, pixel, GetBitGroup(BitRight(c,xoff)));
	    	dst+= DST_INC; CHECKRWO(dst);
	    	c = BitLeft(c,PGSZB-xoff);
	    	while (c)
	    	{
		    WriteBitGroup(dst, pixel, GetBitGroup(c));
		    NextBitGroup(c);
		    dst+=DST_INC; CHECKRWO(dst);
	    	}
	    } while (--hTmp);
#endif /* USE_STIPPLE_CODE else */
#endif /* STIPPLE else */
	}
    }
}

static void
vga256PolyGlyphBlt8Clipped (pDrawable, pGC, x, y, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int 	x, y;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    unsigned char *pglyphBase;	/* start of array of glyphs */
{
    register unsigned long  c;
#ifndef GLYPHROP
    register unsigned long  pixel;
#endif
    register unsigned long  *dst;
    register glyphPointer   glyphBits;
    register int	    xoff;

    CharInfoPtr		pci;
    FontPtr		pfont = pGC->font;
    unsigned long	*dstLine;
    unsigned long	*pdstBase;
    CARD32		*clips;
    int			maxAscent, maxDescent;
    int			minLeftBearing;
    int			hTmp;
    int			widthDst;
    int			bwidthDst;
    int			xG, yG;
    BoxPtr		pBox;
    int			numRects;
    int			w;
    RegionPtr		pRegion;
    int			yBand;
#ifdef GLYPHROP
    unsigned long	bits;
#endif
#ifdef USE_LEFTBITS
    CARD32		*cTmp;
    int			widthGlyph;
    unsigned long	widthMask;
#endif

#ifdef GLYPHROP
    cfb8CheckStipple (pGC->alu, pGC->fgPixel, pGC->planemask);
#else
    pixel = cfbGetGCPrivate(pGC)->xor;
#endif
    
    cfbGetTypedWidthAndPointer (pDrawable, bwidthDst, pdstBase, char, unsigned long)

    BANK_FLAG(pdstBase)

    widthDst = bwidthDst / PGSZB;
    maxAscent = FONTMAXBOUNDS(pfont,ascent);
    maxDescent = FONTMAXBOUNDS(pfont,descent);
    minLeftBearing = FONTMINBOUNDS(pfont,leftSideBearing);

    pRegion = cfbGetCompositeClip(pGC);

    pBox = REGION_RECTS(pRegion);
    numRects = REGION_NUM_RECTS (pRegion);
    while (numRects && pBox->y2 <= y - maxAscent)
    {
	++pBox;
	--numRects;
    }
    if (!numRects || pBox->y1 >= y + maxDescent)
	return;
    yBand = pBox->y1;
    while (numRects && pBox->y1 == yBand && pBox->x2 <= x + minLeftBearing)
    {
	++pBox;
	--numRects;
    }
    if (!numRects)
	return;
    clips = (CARD32 *)ALLOCATE_LOCAL ((maxAscent + maxDescent) *
					     sizeof (CARD32));
    while (nglyph--)
    {
	pci = *ppci++;
	glyphBits = (glyphPointer) FONTGLYPHBITS(pglyphBase,pci);
	w = pci->metrics.rightSideBearing - pci->metrics.leftSideBearing;
	xG = x + pci->metrics.leftSideBearing;
	yG = y - pci->metrics.ascent;
	x += pci->metrics.characterWidth;
	if (hTmp = pci->metrics.descent + pci->metrics.ascent)
	{
	    dstLine = pdstBase + yG * widthDst + (xG >> PWSH);
	    xoff = xG & PIM;
#ifdef USE_LEFTBITS
	    w = pci->metrics.rightSideBearing - pci->metrics.leftSideBearing;
	    widthGlyph = PADGLYPHWIDTHBYTES(w);
	    widthMask = endtab[w];
#endif
	    switch (cfb8ComputeClipMasks32 (pBox, numRects, xG, yG, w, hTmp, clips))
 	    {
	    case rgnPART:
#ifdef USE_LEFTBITS
	    	cTmp = clips;
	    	do {
	    	    dst = dstLine; SETRW(dst);
	    	    dstLine = (unsigned long *) (((char *) dstLine) + bwidthDst);
	    	    GlyphBits(glyphBits, w, c)
		    c &= *cTmp++;
		    if (c)
		    {
	    	    	WriteBitGroup(dst, pixel, GetBitGroup(BitRight(c,xoff)));
	    	    	c = BitLeft(c,PGSZB - xoff);
	    	    	dst+=DST_INC; CHECKRWO(dst);
	    	    	while (c)
	    	    	{
		    	    WriteBitGroup(dst, pixel, GetBitGroup(c));
		    	    NextBitGroup(c);
		    	    dst+=DST_INC; CHECKRWO(dst);
	    	    	}
		    }
	    	} while (--hTmp);
	    	break;
#else
	    	{
		    int h;
    
		    h = hTmp;
		    do
		    {
			--h;
		    	clips[h] = clips[h] & glyphBits[h];
		    } while (h);
	    	}
	    	glyphBits = clips;
	    	/* fall through */
#endif
	    case rgnIN:
#ifdef STIPPLE
	    	STIPPLE(dstLine,glyphBits,pixel,bwidthDst,hTmp,xoff);
#else
#ifdef USE_STIPPLE_CODE
	    	stipplestackte(dstLine,glyphBits,pixel,bwidthDst,hTmp,xoff);
#else
	    	do {
	    	    dst = dstLine; SETRW(dst);
	    	    dstLine = (unsigned long *) (((char *) dstLine) + bwidthDst);
	    	    GlyphBits(glyphBits, w, c)
		    if (c)
		    {
		        /* This code originally could read memory locations
			 * that were not mapped. Hence we have to check the
			 * trailing bits to see whether they are zero and if
			 * then skip them correctly. This is no problem for
			 * the GXcopy case, since there only the pixels that
			 * are non-zero are written ...
			 */
#ifndef GLYPHROP
	    	    	WriteBitGroup(dst, pixel, GetBitGroup(BitRight(c,xoff)));
	    	    	c = BitLeft(c,PGSZB - xoff);
	    	    	dst+=DST_INC; CHECKRWO(dst);
#else /* GLYPHROP */
                        if (bits = GetBitGroup(BitRight(c,xoff)))
	    	    	  WriteBitGroup(dst, pixel, bits);
	    	    	c = BitLeft(c,PGSZB - xoff);
	    	    	dst+=DST_INC; CHECKRWO(dst);

			while (c && ((bits = GetBitGroup(c)) == 0))
		        {
		    	    NextBitGroup(c);
		    	    dst+=DST_INC; CHECKRWO(dst);
                        } 
#endif /* GLYPHROP */
	    	    	while (c)
	    	    	{
		    	    WriteBitGroup(dst, pixel, GetBitGroup(c));
		    	    NextBitGroup(c);
		    	    dst+=DST_INC; CHECKRWO(dst);
	    	    	}
		    }
	    	} while (--hTmp);
#endif /* USE_STIPPLE_CODE else */
#endif /* STIPPLE else */
	    	break;
	    }
	}
    }
    DEALLOCATE_LOCAL (clips);
}

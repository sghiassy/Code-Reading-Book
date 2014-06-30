/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/cfb.w32/w32teblt8.c,v 3.7 1996/12/23 06:35:05 dawes Exp $ */ 
/*
 * TEGblt - ImageText expanded glyph fonts only.  For
 * 8 bit displays, in Copy mode with no clipping.
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
*/
/* $XConsortium: w32teblt8.c /main/5 1996/10/23 18:40:33 kaleb $ */

#if PSZ == 8

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"cfb.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"cfbmskbits.h"
#include	"cfb8bit.h"
#include	"w32itext.h"

/*
 * this code supports up to 5 characters at a time.  The performance
 * differences between 4 and 5 is usually small (~7% on PMAX) and
 * frequently negative (SPARC and Sun3), so this file is compiled
 * only once for now.  If you want to use the other options, you'll
 * need to hack cfbgc.c as well.
 */

#define CFBTEGBLT8 W32TEGlyphBlt8

/*
 * On little-endian machines (or where fonts are padded to 32-bit
 * boundaries) we can use some magic to avoid the expense of getleftbits
 */

typedef unsigned int	*glyphPointer;

void
CFBTEGBLT8 (pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC 		*pGC;
    int 	xInit, yInit;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    pointer	pglyphBase;	/* start of array of glyphs */
{
    register unsigned long  c;
    register unsigned long  leftMask, rightMask;
    register int	    hTmp;
    register int	    xoff1;
    register glyphPointer   char1, char2, char3, char4;

    FontPtr		pfont = pGC->font;
    unsigned long	*pdstBase, stipple;
    int			widthDst;
    int			widthGlyph;
    int			h, i, j, k, dst, dst_pitch, string_width, bytes;
    int			x, y;
    BoxRec		bbox;		/* for clipping */
    int			widthGlyphs;
    long		text_buffer, text1, text2, line_hop, bits;
    LongP		p;


    cfbGetLongWidthAndPointer(pDrawable, widthDst, pdstBase)

    if ((CARD32)pdstBase != VGABASE)
    {
	cfbTEGlyphBlt8 (pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    }

    widthGlyph = FONTMAXBOUNDS(pfont,characterWidth);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);
    if (!h)
	return;

    line_hop = widthDst << 2;
    string_width = widthGlyph * nglyph;

    x = xInit + FONTMAXBOUNDS(pfont,leftSideBearing) + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;
    bbox.x1 = x;
    bbox.x2 = x + string_width;
    bbox.y1 = y;
    bbox.y2 = y + h;

    switch (RECT_IN_REGION(pGC->pScreen,  cfbGetCompositeClip(pGC), &bbox))
    {
      case rgnPART:
	cfbImageGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
      case rgnOUT:
	return;
    }
  
    dst = y * (widthDst << 2) + x;

    if (W32et6000)
    {
	W32P_INIT_IMAGE_TEXT(PFILL(pGC->fgPixel), PFILL(pGC->bgPixel),
			     line_hop - 1, widthGlyph, h);
	for (i = 0; i < nglyph; i++)
	{
	    char1 = (glyphPointer) FONTGLYPHBITS(pglyphBase, *ppci++);
	    if (i&1)
	    {
		*ACL_MIX_ADDRESS = MixDstPong;
		*MBP0 = W32MixPong;
	    }
	    else
	    {
		*ACL_MIX_ADDRESS = MixDstPing;
		*MBP0 = W32Mix;
	    }
	    memcpy(W32Buffer, char1, h<<2); /* is well-optimized in most OSses */
	    *ACL_DESTINATION_ADDRESS = dst;
	    dst += widthGlyph;
	}
    }
    else
    {
#define TEXT_IT(init, fetch) \
	init \
        switch (bytes) \
	{ \
	case 1: \
	    while (hTmp--) \
	    { \
	    	*ACL       = fetch; \
	    } \
	    break; \
	case 2: \
	    while (hTmp--) \
	    { \
                bits       = fetch; \
	    	*ACL       = bits; \
	    	*(ACL + 1) = bits >> 8; \
	    } \
	    break; \
	case 3: \
	    while (hTmp--) \
	    { \
                bits       = fetch; \
	    	*ACL       = bits; \
	    	*(ACL + 1) = bits >> 8; \
	    	*(ACL + 2) = bits >> 16; \
	    } \
	    break; \
	case 4: \
	    while (hTmp--) \
	    { \
                bits       = fetch; \
	    	*ACL       = bits; \
	    	*(ACL + 1) = bits >> 8; \
	    	*(ACL + 2) = bits >> 16; \
	    	*(ACL + 3) = bits >> 24; \
	    } \
	    break; \
	}

#define DO_IMAGE_TEXT(count, width, init, fetch) \
	SET_XY(width, h) \
        bytes = (width + 7) >> 3; \
    	while (nglyph >= count) \
    	{ \
	    nglyph -= count; \
	    hTmp = h; \
	    if (W32OrW32i) \
		*MBP2 = dst; \
	    else \
		*ACL_DESTINATION_ADDRESS = dst; \
            dst += width; \
	    TEXT_IT(init, fetch) \
    	}

    if (W32OrW32i)
	W32_INIT_IMAGE_TEXT(PFILL(pGC->fgPixel), PFILL(pGC->bgPixel),
			    line_hop - 1, widthGlyph, h)
    else
	W32P_INIT_IMAGE_TEXT(PFILL(pGC->fgPixel), PFILL(pGC->bgPixel),
			    line_hop - 1, widthGlyph, h);

    if (widthGlyph <= 8)
    {
	widthGlyphs = widthGlyph << 2;
	DO_IMAGE_TEXT(4, widthGlyphs,
		      char1 = (glyphPointer)FONTGLYPHBITS(pglyphBase, *ppci++);
		      char2 = (glyphPointer)FONTGLYPHBITS(pglyphBase, *ppci++);
		      char3 = (glyphPointer)FONTGLYPHBITS(pglyphBase, *ppci++);
		      char4 = (glyphPointer)FONTGLYPHBITS(pglyphBase, *ppci++);
		      ,
		      (*char1++ | ((*char2++ | ((*char3++ | (*char4++
							     << widthGlyph))
						<< widthGlyph))
				   << widthGlyph)));
    }
    else if (widthGlyph <= 10)
    {
	widthGlyphs = (widthGlyph << 1) + widthGlyph;
	DO_IMAGE_TEXT(3, widthGlyphs,
		      char1 = (glyphPointer)FONTGLYPHBITS(pglyphBase, *ppci++);
		      char2 = (glyphPointer)FONTGLYPHBITS(pglyphBase, *ppci++);
		      char3 = (glyphPointer)FONTGLYPHBITS(pglyphBase, *ppci++);
		      ,
		      (*char1++ | ((*char2++ |
				    (*char3++ << widthGlyph)) << widthGlyph)));
    }
    else if (widthGlyph <= 16)
    {
	widthGlyphs = widthGlyph << 1;
	DO_IMAGE_TEXT(2, widthGlyphs,
		      char1 = (glyphPointer)FONTGLYPHBITS(pglyphBase, *ppci++);
		      char2 = (glyphPointer)FONTGLYPHBITS(pglyphBase, *ppci++);
		      ,
		      (*char1++ | (*char2++ << widthGlyph)));
    }

    DO_IMAGE_TEXT(1, widthGlyph,
		  char1 = (glyphPointer)FONTGLYPHBITS(pglyphBase, *ppci++);,
		  *char1++);
    }
}
#endif /* PSZ == 8 */

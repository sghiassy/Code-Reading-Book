/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/sis/sis_teblt8.c,v 1.3 1997/01/18 06:56:53 dawes Exp $ */

/*
 * Copyright (c) 1989  X Consortium
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of the X Consortium shall not
 * be used in advertising or otherwise to promote the sale, use or other
 * dealings in this Software without prior written authorization from the X
 * Consortium.
 */

/*
 * Modified for Cirrus by Harm Hanemaayer (hhanemaa@cs.ruu.nl).
 *
 * We accelerate straightforward text writing for fonts with widths up to 32
 * pixels.
 */

/*
 * Modified for Chips by David Bateman (dbateman@ee.uts.edu.au)
 * Modified for Sis by Xavier Ducoin (xavier@rd.lectra.fr)
 */

/*
 * TEGblt - ImageText expanded glyph fonts only.  For
 * 8/16/24 bit displays, in Copy mode with no clipping.
 */

#include        "vga256.h"
#include	"xf86.h"
#include	"vga.h"		       /* For vgaBase. */
#include        "vgaBank.h"
#include	"xf86_OSlib.h"

#include "compiler.h"

#include "sis_driver.h"

#include "sis_Blitter.h"


/* 
 * This function collects normal character bitmaps from the server.
 */

static void 
CollectCharacters(glyphp, nglyph, pglyphBase, ppci)
    unsigned long **glyphp;
    unsigned int nglyph;
    CharInfoPtr *ppci;
    unsigned char *pglyphBase;
{
    int i;

    for (i = 0; i < nglyph; i++) {
	glyphp[i] = (unsigned long *)FONTGLYPHBITS(pglyphBase,
	    *ppci++);
    }
}

void
sisMMIOImageGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    unsigned int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    unsigned long *pdstBase;
    int widthDst;
    int widthGlyph;
    int h;
    int x, y;
    BoxRec bbox;		       /* for clipping */

    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    int i;

    /* Characters are padded to 4 bytes. */
    unsigned long **glyphp;
    int destaddr, blitwidth, fontwidthlimit;
    unsigned int op ;
    int		srcPitch ;

    int		maxglyph, glyphTodo, iglyph, glyphDone ;

#ifdef DEBUG
    ErrorF("CHIPS:sisMMIOImageGlyphBlt(%d, %d, %d)\n", xInit, yInit, nglyph);
#endif

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    widthDst = vga256InfoRec.displayWidth * vgaBytesPerPixel;

    fontwidthlimit = 32;


    /* We only accelerate fonts 32 or less pixels wide. */
    /* Let cfb handle writing into offscreen pixmap. */
    if (vgaBitsPerPixel == 8 && (pDrawable->type != DRAWABLE_WINDOW ||
	    !xf86VTSema)) {
	cfbImageGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
	    pglyphBase);
	return;
    }
    if (vgaBitsPerPixel == 16 && (pDrawable->type != DRAWABLE_WINDOW ||
	    !xf86VTSema)) {
	cfb16ImageGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
	    pglyphBase);
	return;
    }
    if (vgaBitsPerPixel == 24 && (pDrawable->type != DRAWABLE_WINDOW ||
	    !xf86VTSema)) {
	cfb24ImageGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
	    pglyphBase);
	return;
    }
    if (glyphWidthBytes != 4 || glyphWidth > fontwidthlimit || 
	sisBLTPatternAddress <= 0 ) {
	switch (vgaBitsPerPixel) {
	case 8:
	    vga256TEGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph,
		ppci, pglyphBase);
	    break;
	case 16:
	    cfb16TEGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph,
		ppci, pglyphBase);
	    break;
	case 24:
	    cfb24ImageGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph,
		ppci, pglyphBase);
	    break;
	}
	return;
    }
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    if ((h | glyphWidth) == 0)
	return;

    x = xInit + FONTMAXBOUNDS(pfont, leftSideBearing) + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;
    bbox.x1 = x;
    bbox.x2 = x + (glyphWidth * nglyph);
    bbox.y1 = y;
    bbox.y2 = y + h;

    switch (RECT_IN_REGION(pGC->pScreen, cfbGetCompositeClip(pGC), &bbox)) {
    case rgnPART:
	switch (vgaBitsPerPixel) {
	case 8:
	    vga256TEGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph,
		ppci, pglyphBase);
	    break;
	case 16:
	    cfb16TEGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph,
		ppci, pglyphBase);
	    break;
	case 24:
	    cfb24ImageGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph,
		ppci, pglyphBase);
	    break;
	}
    case rgnOUT:
	return;
    }

    /* Allocate list of pointers to glyph bitmaps. */
    glyphp = (unsigned long **)ALLOCATE_LOCAL(nglyph *
	sizeof(unsigned long *));

    switch (vgaBitsPerPixel) {
    case 16:
	sisSETBGCOLOR16(pGC->bgPixel);	/* need to inverse ??? */
	sisSETFGCOLOR16(pGC->fgPixel);
	break;
    case 24:
	sisSETBGCOLOR24(pGC->bgPixel);
	sisSETFGCOLOR24(pGC->fgPixel);
	break;
    default:			       /* 8 */
	sisSETBGCOLOR8(pGC->bgPixel);
	sisSETFGCOLOR8(pGC->fgPixel);
	break;
    }

    sisSETSRCADDR(sisBLTPatternAddress);
    sisSETROPFG(0xF0);   	/* pat copy */
    sisSETROPBG(0xCC); 		/* copy */
    op = sisCMDCOLEXP | sisCMDENHCOLEXP | sisPATFG | sisSRCBG |
	sisTOP2BOTTOM | sisLEFT2RIGHT;

    CollectCharacters(glyphp, nglyph, pglyphBase, ppci);

    destaddr = y * widthDst + (x * vgaBytesPerPixel);
    blitwidth = glyphWidth * nglyph;
    srcPitch =  ((blitwidth + 31)& ~31) /8 ;

    maxglyph = ( ((((sisBLTPatternOffscreenSize / h) * 8 ) - 31) & ~31) ) 
	/ glyphWidth ;
    /*
     * here can't use the whole bitmap cache to expand colors
     * good data in cache wrong data on screen.
     * problem appears at long #6
     * This seem to be good if I limit the srcPitch 
     *
     */
    { 
	int nmax = 160 / glyphWidth ;
	maxglyph = maxglyph < nmax ? maxglyph : nmax ;
    }

    glyphTodo = nglyph ;
    glyphDone = 0 ;
    do {
	iglyph = glyphTodo < maxglyph ? glyphTodo : maxglyph ;
	blitwidth = glyphWidth * iglyph;
	srcPitch =  ((blitwidth + 31)& ~31) /8 ;

	sisSETPITCH(srcPitch, widthDst);
	sisSETDSTADDR(destaddr);
	sisSETHEIGHTWIDTH(h-1, blitwidth * vgaBytesPerPixel-1);

	/* Write bitmap to video memory (for BitBlt engine to process). */
	/* Gather bytes until we have a dword to write. Doubleword is   */
	/* LSByte first, and MSBit first in each byte, as required for  */
	/* the blit data. */

	sisTransferText(iglyph, h, &glyphp[glyphDone], glyphWidth, 
			(unsigned char *)vgaLinearBase + sisBLTPatternAddress);
#ifdef DUMPDATA
	DumpData(iglyph, h, glyphWidth, 
		 (unsigned char *)vgaLinearBase + sisBLTPatternAddress, 6);
#endif

	sisSETCMD(op);    
	sisBLTWAIT;
	glyphDone += iglyph ;
	destaddr = y * widthDst + ( ( x + glyphDone * glyphWidth ) 
				    * vgaBytesPerPixel);
	glyphTodo -= iglyph ;

    } while ( glyphTodo );
	    

    DEALLOCATE_LOCAL(glyphp);
}

/*
 * Transparent text.
 */

void 
sisMMIOPolyGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    unsigned int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    unsigned long *pdstBase;
    int widthDst;
    int widthGlyph;
    int h;
    int x, y;
    BoxRec bbox;		       /* for clipping */

    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    int i;

    /* Characters are padded to 4 bytes. */
    unsigned long **glyphp;
    int shift, line;
    unsigned dworddata;
    int destaddr, blitwidth, fontwidthlimit;
    unsigned int	op;
    int			srcPitch;

    int		maxglyph, glyphTodo, iglyph, glyphDone ;

#ifdef DEBUG
    ErrorF("CHIPS:sisMMIOPolyGlyphBlt(%d, %d, %d)\n", xInit, yInit, nglyph);
#endif

    /* Let cfb handle writing into offscreen pixmap. */
    if (vgaBitsPerPixel == 8 && (pDrawable->type != DRAWABLE_WINDOW ||
	    !xf86VTSema)) {
	vga256PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
	    pglyphBase);
	return;
    }
    if (vgaBitsPerPixel == 16 && (pDrawable->type != DRAWABLE_WINDOW ||
	    !xf86VTSema)) {
	cfb16PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
	    pglyphBase);
	return;
    }
    if (vgaBitsPerPixel == 24 && (pDrawable->type != DRAWABLE_WINDOW ||
	    !xf86VTSema)) {
	cfb24PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
	    pglyphBase);
	return;
    }

#if 0 /* This doesn't work with multiple depths */
    cfbGetLongWidthAndPointer(pDrawable, widthDst, pdstBase)
    widthDst *= 4;		       /* Convert to bytes. */
    if (!CHECKSCREEN(pdstBase)) {
	switch (vgaBitsPerPixel) {
	  case 8:
	    cfbPolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
	    break;
	  case 16:
	    cfb16PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
	    break;
	  case 24:
	    cfb24PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
	    break;
	}
	return;
    }
#else
    widthDst = vga256InfoRec.displayWidth * vgaBytesPerPixel;
#endif

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);


    fontwidthlimit = 32;

    
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);
    if ((h | glyphWidth) == 0)
	return;

    /* We only do GXcopy polyglyph. */
    if ((pGC->alu != GXcopy) || (glyphWidthBytes != 4 || 
	    glyphWidth > fontwidthlimit || sisBLTPatternAddress <= 0 )) {
	if (pGC->alu == GXcopy) {
	    switch (vgaBitsPerPixel) {
	      case 8:
		vga256PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
		break;
	      case 16:
		cfb16PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
		break;
	      case 24:
		cfb24PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
		break;
	    }
	} else {
	    switch (vgaBitsPerPixel) {
	      case 8:
		vga256PolyGlyphRop8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
		break;
	      case 16:
		cfb16PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
		break;
	      case 24:
		cfb24PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
		break;
	    }
	}
	return;
    }

    if (pGC->fillStyle != FillSolid) {
	miPolyGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci,
	    pglyphBase);
	return;
    }
    
    if (FONTMAXBOUNDS(pGC->font, rightSideBearing) -
	FONTMINBOUNDS(pGC->font, leftSideBearing) > 32 ||
	FONTMINBOUNDS(pGC->font, characterWidth) < 0) {
	miPolyGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci,
	    pglyphBase);
	return;
    }

    x = xInit + FONTMAXBOUNDS(pfont, leftSideBearing) + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;
    bbox.x1 = x;
    bbox.x2 = x + (glyphWidth * nglyph);
    bbox.y1 = y;
    bbox.y2 = y + h;

    switch (RECT_IN_REGION(pGC->pScreen, cfbGetCompositeClip(pGC), &bbox)) {
    case rgnPART:
	if (pGC->alu == GXcopy) {
	    switch (vgaBitsPerPixel) {
	      case 8:
		vga256PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
		break;
	      case 16:
		cfb16PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
		break;
	      case 24:
		cfb24PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
		break;
	    }
	} else {
	    switch (vgaBitsPerPixel) {
	      case 8:
		vga256PolyGlyphRop8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
		break;
	      case 16:
		cfb16PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
		break;
	      case 24:
		cfb24PolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			     pglyphBase);
		break;
	    }
	}
    case rgnOUT:
	return;
    }

    /* Allocate list of pointers to glyph bitmaps. */
    glyphp = (unsigned long **)ALLOCATE_LOCAL(nglyph *
	sizeof(unsigned long *));

    destaddr = y * widthDst + (x * vgaBytesPerPixel);

    switch (vgaBitsPerPixel) {
    case 16:
	sisSETFGCOLOR16(pGC->fgPixel);
	break;
    case 24:
	sisSETFGCOLOR24(pGC->fgPixel);
	break;
    default:			       /* 8 */
	sisSETFGCOLOR8(pGC->fgPixel);
	break;
    }

    sisSETSRCADDR(sisBLTPatternAddress);
    sisSETROPFG(0xF0);		/* pat copy */
    sisSETROPBG(0xAA); 		/* dst */
    op =  sisCMDCOLEXP | sisCMDENHCOLEXP | sisTOP2BOTTOM | 
	sisLEFT2RIGHT | sisPATFG | sisSRCBG ;

    CollectCharacters(glyphp, nglyph, pglyphBase, ppci);

    destaddr = y * widthDst + (x * vgaBytesPerPixel);
    blitwidth = glyphWidth * nglyph;
    srcPitch =  ((blitwidth + 31)& ~31) /8 ;

    maxglyph = ( ((((sisBLTPatternOffscreenSize / h) * 8 ) - 31) & ~31) ) 
	/ glyphWidth ;
    /*
     * here can't use the whole bitmap cache to expand colors
     * good data in cache wrong data on screen.
     * problem appears at long #6
     * This seem to be good if I limit the srcPitch 
     *
     */
    { 
	int nmax = 160 / glyphWidth ;
	maxglyph = maxglyph < nmax ? maxglyph : nmax ;
    }

    glyphTodo = nglyph ;
    glyphDone = 0 ;
    do {
	iglyph = glyphTodo < maxglyph ? glyphTodo : maxglyph ;
	blitwidth = glyphWidth * iglyph;
	srcPitch =  ((blitwidth + 31)& ~31) /8 ;

	sisSETPITCH(srcPitch, widthDst);
	sisSETDSTADDR(destaddr);
	sisSETHEIGHTWIDTH(h-1, blitwidth * vgaBytesPerPixel-1);

	/* Write bitmap to video memory (for BitBlt engine to process). */
	/* Gather bytes until we have a dword to write. Doubleword is   */
	/* LSByte first, and MSBit first in each byte, as required for  */
	/* the blit data. */

	sisTransferText(iglyph, h, &glyphp[glyphDone], glyphWidth, 
			(unsigned char *)vgaLinearBase + sisBLTPatternAddress);
#ifdef DUMPDATA
	DumpData(iglyph, h, glyphWidth, 
		 (unsigned char *)vgaLinearBase + sisBLTPatternAddress, 6);
#endif

	sisSETCMD(op);    
	sisBLTWAIT;
	glyphDone += iglyph ;
	destaddr = y * widthDst + ( ( x + glyphDone * glyphWidth ) 
				    * vgaBytesPerPixel);
	glyphTodo -= iglyph ;

    } while ( glyphTodo );


    DEALLOCATE_LOCAL(glyphp);
    return;
}

/*
 * Low-level text transfer routines. Compile it only once
 */
#if 0
int
sisTransferText(nglyph, h, glyphp, glyphwidth, base)
    int nglyph;
    int h;
    unsigned long **glyphp;
    int glyphwidth;
    unsigned int *base;
{
    int shift;
    unsigned long dworddata;
    unsigned char *data;
    int i, line, databytes;

    /* Other character widths. Tricky, bit order is very awkward.  */
    /* We maintain a word straightforwardly LSB, and do the */
    /* bit order converting when writing 32-bit double words. */

#ifdef DEBUG
    ErrorF("CHIPS:sisTransferText(%d, %d, %d, 0x%X)\n", nglyph, h,
	glyphwidth, base);
#endif

    dworddata = 0;
    line = 0;
    shift = 0;
    data = (unsigned char *)ALLOCATE_LOCAL((nglyph * glyphwidth + 7) & ~7);
    while (line < h) {
	databytes = 0;
	i = 0;
	while (i < nglyph) {
	    dworddata += glyphp[i][line] << shift;
	    shift += glyphwidth;
	    if (shift >= 16) {
		/* Store the current word */
		*(unsigned short *)(data + databytes) =
		    byte_reversed[dworddata & 0xff] +
		    (byte_reversed[(dworddata & 0xff00) >> 8] << 8);
		databytes += 2;
		shift -= 16;
		dworddata >>= 16;
	    }
	    i++;
	}
	if (shift > 0) {
	    /* Make sure last bits of scanline are padded to byte
	     * boundary. */
	    shift = (shift + 7) & ~7;
	    *(unsigned short *)(data + databytes) =
	        byte_reversed[dworddata & 0xff] +
		(byte_reversed[(dworddata & 0xff00) >> 8] << 8);
	    databytes += 2;
	    shift = 0;
	    dworddata = 0;
	}
	if (databytes & 0x3) {
	    /* Pad the data line to a double word boundary */
	    *(unsigned short *)(data + databytes) = 0;
	    databytes += 2;
	}
        for(i = 0; i < (((glyphwidth * nglyph + 31 ) & ~31) >> 5); i++) {
            *base=*(unsigned int *)(data + 4 * i);
	    base++ ;
	}
	line++;
    }
    DEALLOCATE_LOCAL(data);
}
#endif

DumpData(nglyph, h, glyphwidth, base, startWord)
    int nglyph;
    int h;
    int glyphwidth;
    unsigned int *base;
    int startWord ;
{
#define DATA_ON "1"
#define DATA_OFF " "
    int i,j ;
    int show=0;
    for ( j=0;j<h;j++) {
        for(i = 0; i < (((glyphwidth * nglyph + 31 ) & ~31) >> 5); i++) {
	    if ( i >= startWord ) {
		int ii ;
		unsigned int tmp ;
		show = 1 ;
		tmp = *base ;
		for ( ii =0 ; ii< 8 ; ii++ )
		    ErrorF("%s",(tmp&(0x80L>>ii))?DATA_ON:DATA_OFF);
		tmp >>= 8 ;
		for ( ii  =0 ; ii< 8 ; ii++ )
		    ErrorF("%s",(tmp&(0x80L>>ii))?DATA_ON:DATA_OFF);
		tmp >>= 8 ;
		for ( ii =0 ; ii< 8 ; ii++ )
		    ErrorF("%s",(tmp&(0x80L>>ii))?DATA_ON:DATA_OFF);
		tmp >>= 8 ;
		for ( ii =0 ; ii< 8 ; ii++ )
		    ErrorF("%s",(tmp&(0x80L>>ii))?DATA_ON:DATA_OFF);
	    }
	    base++;
	}
	if (show) ErrorF("\n");
    }
}




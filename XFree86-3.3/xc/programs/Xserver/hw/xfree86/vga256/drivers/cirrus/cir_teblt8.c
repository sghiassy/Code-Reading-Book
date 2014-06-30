/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_teblt8.c,v 3.23.2.1 1997/05/10 09:10:23 hohndel Exp $ */
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

/*
 * Modified for Cirrus by Harm Hanemaayer (hhanemaa@cs.ruu.nl).
 *
 * We accelerate straightforward text writing for fonts with widths up to 32
 * pixels.
 */
/* $XConsortium: cir_teblt8.c /main/14 1996/10/26 11:17:14 kaleb $ */
/*
 * TEGblt - ImageText expanded glyph fonts only.  For
 * 8 bit displays, in Copy mode with no clipping.
 */
 
#include        "vga256.h"
#include	"xf86.h"
#include	"vga.h"	/* For vgaBase. */
#include        "vgaBank.h"

#include "compiler.h"

#include "cir_driver.h"
#ifdef CIRRUS_MMIO
#include "cirBlitMM.h"
#else
#include "cirBlitter.h"
#endif
#include "cir_inline.h"


void CirrusTransferTextWidth8();
void CirrusTransferTextWidth6();
void CirrusTransferText();		/* General, for widths <= 16. */
void CirrusTransferText32bit();		/* General, for widths <= 32. */
void CirrusColorExpandWriteText();
void CirrusColorExpandWriteTextWidth6();
void CirrusColorExpandWriteTextWidth8();


/*
 * Macros to efficiently check whether the font is supported by a
 * 'special' (hardcoded) text transfer routine.
 */

#define SPECIALACCELERATEDFONTS ( \
	(1 << 5) | (1 << 7) | (1 << 13) /* | (1 << 15) | (1 << 31) */ )
/* Widths:    6		 8	    14		16	    32 */

#define ISSPECIALWIDTH(w) (((unsigned)1 << (w - 1)) & SPECIALACCELERATEDFONTS)


/* 
 * This function collects normal character bitmaps from the server.
 */

static void CollectCharacters(glyphp, nglyph, pglyphBase, ppci)
	unsigned long **glyphp;
	unsigned int nglyph;
	CharInfoPtr *ppci;
	unsigned char *pglyphBase;
{
	int i;
	for (i = 0; i < nglyph; i++ ) {
		glyphp[i] = (unsigned long *)FONTGLYPHBITS(pglyphBase,
			*ppci++);
	}
}

#ifdef CIRRUS_MMIO
void CirrusMMIOImageGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
#else
void CirrusImageGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
#endif
    DrawablePtr pDrawable;
    GC 		*pGC;
    int 	xInit, yInit;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    unsigned char *pglyphBase;	/* start of array of glyphs */
{
    FontPtr		pfont = pGC->font;
    unsigned long	*pdstBase;
    int			widthDst;
    int			widthGlyph;
    int			h;
    int			x, y;
    BoxRec		bbox;		/* for clipping */

    	int glyphWidth;		/* Character width in pixels. */
    	int glyphWidthBytes;	/* Character width in bytes (padded). */
	int i;
	/* Characters are padded to 4 bytes. */
	unsigned long **glyphp;
	int shift, line;
	unsigned dworddata;
	int destaddr, blitwidth;



	glyphWidth = FONTMAXBOUNDS(pfont,characterWidth);
	glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);

#if 0	/* This cannot be used in a function used for 8, 16, 32bpp that */
	/* is compiled only once. */
	cfbGetLongWidthAndPointer(pDrawable, widthDst, pdstBase)
#else
	widthDst = vga256InfoRec.displayWidth << (vgaBitsPerPixel >> 4);
#endif

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
	        cfb16TEGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci,
	        	pglyphBase);
		return;
	}
	if (vgaBitsPerPixel == 24 && (pDrawable->type != DRAWABLE_WINDOW ||
	!xf86VTSema)) {
	        cfb24TEGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci,
	        	pglyphBase);
		return;
	}
	if (vgaBitsPerPixel == 24 && (pDrawable->type != DRAWABLE_WINDOW ||
	!xf86VTSema)) {
	        cfb24TEGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci,
	        	pglyphBase);
		return;
	}
	if (vgaBitsPerPixel == 32 && (pDrawable->type != DRAWABLE_WINDOW ||
	!xf86VTSema)) {
	        cfb32TEGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci,
	        	pglyphBase);
		return;
	}
	if (glyphWidthBytes != 4 || glyphWidth > 32) {
		switch (vgaBitsPerPixel) {
		case 8 :
	        	vga256TEGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph,
	        		ppci, pglyphBase);
	        	break;
	        case 16 :
	        	cfb16TEGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph,
	        		ppci, pglyphBase);
	        	break;
	        case 24 :
	        	cfb24TEGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph,
	        		ppci, pglyphBase);
	        	break;
	        case 32 :
	        	cfb32TEGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph,
	        		ppci, pglyphBase);
	        	break;
	        }
	        return;
	}

    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    if ((h | glyphWidth) == 0) return;

    x = xInit + FONTMAXBOUNDS(pfont,leftSideBearing) + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;
    bbox.x1 = x;
    bbox.x2 = x + (glyphWidth * nglyph);
    bbox.y1 = y;
    bbox.y2 = y + h;

        switch (RECT_IN_REGION(pGC->pScreen, cfbGetCompositeClip(pGC), &bbox))
        {
        case rgnPART:
		switch (vgaBitsPerPixel) {
		case 8 :
			vga256TEGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph,
				ppci, pglyphBase);
			break;
		case 16 :
			cfb16TEGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph,
				ppci, pglyphBase);
			break;
		case 24 :
			cfb24TEGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph,
				ppci, pglyphBase);
			break;
		case 32 :
			cfb32TEGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph,
				ppci, pglyphBase);
			break;
		}
        case rgnOUT:
		return;
       	}

	/* Allocate list of pointers to glyph bitmaps. */
	glyphp = (unsigned long **)ALLOCATE_LOCAL(nglyph * sizeof(unsigned long *));

	if (!HAVEBITBLTENGINE()) {
		/* For 5420/2/4, first clear area and then use color */
		/* expansion text write. */
		BoxRec box;

		CollectCharacters(glyphp, nglyph, pglyphBase, ppci);

		box.x1 = x;
		box.x2 = x + (glyphWidth * nglyph);
		box.y1 = y;
		box.y2 = y + h;
		CirrusFillBoxSolid(pDrawable, 1, &box, pGC->bgPixel,
			pGC->bgPixel, GXcopy);
		CirrusColorExpandWriteText(x, y, widthDst, nglyph, h,
			glyphp,	glyphWidth, pGC->fgPixel);
		DEALLOCATE_LOCAL(glyphp);
		return;
	}

	CollectCharacters(glyphp, nglyph, pglyphBase, ppci);

	destaddr = y * widthDst + (x << (vgaBitsPerPixel >> 4));
	SETDESTADDR(destaddr);
	SETDESTPITCH(widthDst);
	SETSRCADDR(0);
/*	SETSRCPITCH(0); */
	blitwidth = (glyphWidth * nglyph) << (vgaBitsPerPixel >> 4);
	SETWIDTH(blitwidth);
	SETHEIGHT(h);

	switch (vgaBitsPerPixel) {
	case 16 :
		SETBACKGROUNDCOLOR16(pGC->bgPixel);
		SETFOREGROUNDCOLOR16(pGC->fgPixel);
		break;
	case 24 :
	case 32 :
		SETBACKGROUNDCOLOR32(pGC->bgPixel);
		SETFOREGROUNDCOLOR32(pGC->fgPixel);
		break;
	default : /* 8 */
		SETBACKGROUNDCOLOR(pGC->bgPixel);
		SETFOREGROUNDCOLOR(pGC->fgPixel);
		break;
	}

	switch (vgaBitsPerPixel) {
	case 16 :
		SETBLTMODE(SYSTEMSRC | COLOREXPAND | PIXELWIDTH16);
		break;
	case 24 :
		SETBLTMODE(SYSTEMSRC | COLOREXPAND | PIXELWIDTH24);
		break;
	case 32 :
		SETBLTMODE(SYSTEMSRC | COLOREXPAND | PIXELWIDTH32);
		break;
	default : /* 8 */
		SETBLTMODE(SYSTEMSRC | COLOREXPAND);
		break;
	}
	SETROP(CROP_SRC);
	STARTBLT();

	/* Write bitmap to video memory (for BitBlt engine to process). */
	/* Gather bytes until we have a dword to write. Doubleword is   */
	/* LSByte first, and MSBit first in each byte, as required for  */
	/* the blit data. */

	if (ISSPECIALWIDTH(glyphWidth))
		CirrusTransferText32bitSpecial(nglyph, h, glyphp, glyphWidth,
			CIRRUSBASE());
	else
#if 0
		if (glyphWidth > 16 || HAVE543X() ||
		    HAVE755X() || cirrusChip == CLGD5446 ||
		    cirrusChip == CLGD5480)
			CirrusTransferText32bit(nglyph, h, glyphp, glyphWidth,
				CIRRUSBASE());
		else
			CirrusTransferText(nglyph, h, glyphp, glyphWidth,
				CIRRUSBASE());
#else
		/* The new 542x databook says to use DWORD transfers. */
		CirrusTransferText32bit(nglyph, h, glyphp, glyphWidth,
			CIRRUSBASE());
#endif

	WAITUNTILFINISHED();

	SETBACKGROUNDCOLOR(0x0f);
	SETFOREGROUNDCOLOR(0);
	
	DEALLOCATE_LOCAL(glyphp);
}


/*
 * Transparent text.
 */

#ifdef CIRRUS_MMIO
void CirrusMMIOPolyGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
#else
void CirrusPolyGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
#endif
    DrawablePtr pDrawable;
    GC 		*pGC;
    int 	xInit, yInit;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    unsigned char *pglyphBase;	/* start of array of glyphs */
{
    FontPtr		pfont = pGC->font;
    unsigned long	*pdstBase;
    int			widthDst;
    int			widthGlyph;
    int			h;
    int			x, y;
    BoxRec		bbox;		/* for clipping */

    	int glyphWidth;		/* Character width in pixels. */
    	int glyphWidthBytes;	/* Character width in bytes (padded). */
	int i;
	/* Characters are padded to 4 bytes. */
	unsigned long **glyphp;
	int shift, line;
	unsigned dworddata;
	int destaddr, blitwidth;
	int fontwidthlimit;

	void (*PolyGlyph)();


    /* I don't think that we can use this function in multiple pixel
       depths modes w/o compiling this file multiple times.  The fix
       is to not allow CirrusPolyGlyphBlt() be called when bpp > 8. */
    cfbGetLongWidthAndPointer(pDrawable, widthDst, pdstBase);

	if (!CHECKSCREEN(pdstBase)) {
		cfbPolyGlyphBlt8(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			pglyphBase);
		return;
	}

	widthDst *= 4;		/* Convert to bytes. */

	glyphWidth = FONTMAXBOUNDS(pfont,characterWidth);
	glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);

	h = FONTASCENT(pfont) + FONTDESCENT(pfont);
	if ((h | glyphWidth) == 0) return;

	PolyGlyph = NULL;

	if (HAVEBITBLTENGINE() && !cirrusAvoidImageBLT)
		fontwidthlimit = 32;	/* BitBLT transfer function used. */
	else
		fontwidthlimit = 16;	/* Color expansion function used. */
	if (glyphWidthBytes != 4 || glyphWidth > fontwidthlimit ||
	(fontwidthlimit == 16 && h > 96))
	        if (pGC->alu == GXcopy)
        		PolyGlyph = vga256PolyGlyphBlt8;
	        else
        		PolyGlyph = vga256PolyGlyphRop8;

	/* We only do GXcopy polyglyph. */
	if (pGC->alu != GXcopy)
		PolyGlyph = vga256PolyGlyphRop8;

	if (pGC->fillStyle != FillSolid)
		PolyGlyph = miPolyGlyphBlt;

        if (FONTMAXBOUNDS(pGC->font,rightSideBearing) -
            FONTMINBOUNDS(pGC->font,leftSideBearing) > 32 ||
	    FONTMINBOUNDS(pGC->font,characterWidth) < 0)
		PolyGlyph = miPolyGlyphBlt;

	if (PolyGlyph != NULL) {
		(*PolyGlyph)(pDrawable, pGC, xInit, yInit, nglyph, ppci,
			pglyphBase);
		return;
	}

    x = xInit + FONTMAXBOUNDS(pfont,leftSideBearing) + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;
    bbox.x1 = x;
    bbox.x2 = x + (glyphWidth * nglyph);
    bbox.y1 = y;
    bbox.y2 = y + h;

    switch (RECT_IN_REGION(pGC->pScreen, cfbGetCompositeClip(pGC), &bbox))
    {
      case rgnPART:
        if (pGC->alu == GXcopy)
        	PolyGlyph = vga256PolyGlyphBlt8;
        else
        	PolyGlyph = vga256PolyGlyphRop8;
	(*PolyGlyph)(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
      case rgnOUT:
	return;
    }

	/* Allocate list of pointers to glyph bitmaps. */
	glyphp = (unsigned long **)ALLOCATE_LOCAL(nglyph * sizeof(unsigned long *));

	if (HAVEBITBLTENGINE() && !cirrusAvoidImageBLT) {
		/* On the 543x, we can use BitBLT text transfer for
		 * transparent text. This is because on the 543x,
		 * transparency in the BitBLT engine is 'fixed' to be
		 * similar to write mode 4.
		 * On the 5426/8, it turns out it can also work.
		 * Same with a 5446 or a 5480.
		 */
		int destaddr, blitwidth;
		unsigned int color;

		CollectCharacters(glyphp, nglyph, pglyphBase, ppci);

		destaddr = y * widthDst + x;
		SETDESTADDR(destaddr);
		SETDESTPITCH(widthDst);
		SETSRCADDR(0);
/*		SETSRCPITCH(0); */
		blitwidth = glyphWidth * nglyph;
		SETWIDTH(blitwidth);
		SETHEIGHT(h);

		/* Transparency is a special case. All four foreground
		 * color registers must be loaded, and the background color
		 * registers must be loaded with the bitwise complement of
		 * the foreground color.
		 * MMIO implies a 5429 or 543x or 5446 or 5480.
		 */
		if (cirrusChip == CLGD5434) {
			color = (~pGC->fgPixel) & 0xff;
			color = color | (color << 8) | (color << 16)
				| (color << 24);
			SETBACKGROUNDCOLOR32(color);
			color = pGC->fgPixel;
			color = color | (color << 8) | (color << 16)
				| (color << 24);
			SETFOREGROUNDCOLOR32(color);
		}
#ifdef CIRRUS_MMIO
		else {
#else		
		else if (cirrusChip == CLGD5430 || cirrusChip == CLGD5429) {
#endif
			color = (~pGC->fgPixel) & 0xff;
			color = color | (color << 8);
			SETBACKGROUNDCOLOR16(color);
			color = pGC->fgPixel;
			color = color | (color << 8);
			SETFOREGROUNDCOLOR16(color);
		}
#ifndef CIRRUS_MMIO
		else {
			color = (~pGC->fgPixel) & 0xff;
			color = color | (color << 8);
			SETBACKGROUNDCOLOR16(color);
			SETTRANSPARENCYCOLOR16(color);
			SETTRANSPARENCYCOLORMASK16(0x0000);
			color = pGC->fgPixel;
			color = color | (color << 8);
			SETFOREGROUNDCOLOR16(color);
		}
#endif

		SETBLTMODE(SYSTEMSRC | COLOREXPAND | TRANSPARENCYCOMPARE);
		SETROP(CROP_SRC);
		STARTBLT();

		if (ISSPECIALWIDTH(glyphWidth))
			CirrusTransferText32bitSpecial(nglyph, h, glyphp,
				glyphWidth, CIRRUSBASE());
		else
			CirrusTransferText32bit(nglyph, h, glyphp, glyphWidth,
				CIRRUSBASE());

		WAITUNTILFINISHED();

		SETBACKGROUNDCOLOR(0x0f);
		SETFOREGROUNDCOLOR(0);

		DEALLOCATE_LOCAL(glyphp);
		return;
	}

	CollectCharacters(glyphp, nglyph, pglyphBase, ppci);

	CirrusColorExpandWriteText(x, y, widthDst, nglyph, h, glyphp,
		glyphWidth, pGC->fgPixel);

	DEALLOCATE_LOCAL(glyphp);
}


#ifndef CIRRUS_MMIO	/* Compile these functions only once. */

/*
 * Low-level text transfer routines.
 */

#if 0	/* Replaced by assembler routines in cir_textblt.s. */

void CirrusTransferText(nglyph, h, glyphp, glyphwidth, base)
	int nglyph;
	int h;
	unsigned long **glyphp;
	unsigned char *base;
{
	int shift;
	unsigned long dworddata;
	int i, line;

	/* Other character widths. Tricky, bit order is very awkward.  */
	/* We maintain a word straightforwardly LSB, and do the */
	/* bit order converting when writing 16-bit words. */

	dworddata = 0;
	line = 0;
	shift = 0;
	while (line < h) {
		i = 0;
		while (i < nglyph) {
			dworddata += glyphp[i][line] << shift;
			shift += glyphwidth;
			if (shift >= 16) {
				/* Write a 16-bit word. */
				*(unsigned short *)base =
					byte_reversed[dworddata & 0xff] +
					(byte_reversed[(dworddata & 0xff00) >> 8] << 8);
				shift -= 16;
				dworddata >>= 16;
			}
			i++;
		}
		if (shift > 0) {
			/* Make sure last bits of scanline are padded to byte
			 * boundary. */
			shift = (shift + 7) & ~7;
			if (shift >= 16) {
				/* Write a 16-bit word. */
				*(unsigned short *)base =
					byte_reversed[dworddata & 0xff] +
					(byte_reversed[(dworddata & 0xff00) >> 8] << 8);
				shift -= 16;
				dworddata >>= 16;
			}
		}
		line++;
	}

	{
		/* There are (shift) bits left. */
		unsigned data;
		int bytes;
		data = byte_reversed[dworddata & 0xff] +
			(byte_reversed[(dworddata & 0xff00) >> 8] << 8);
		/* Number of bytes of real bitmap data. */
		bytes = ((nglyph * glyphwidth + 7) >> 3) * h;
		/* We must transfer a multiple of 4 bytes in total. */
		if ((bytes - ((shift + 7) >> 3)) & 2)
			*(unsigned short *)base = data;
		else
			if (shift != 0)
				*(unsigned long *)base = data;
	}
}

#endif


/*
 * Color expansion framebuffer transparent text write routines.
 * Now uses 64K banking window (512K pixels in BY8 addressing mode)
 * without banking checks. The 16K granularity ensures correct operation
 * with fontheights <= 96 assuming a virtual width <= 4096.
 * Would benefit greatly from assembler routine.
 */
 
void CirrusColorExpandWriteText(x, y, destpitch, nglyph, h, glyphp,
glyphwidth, fg)
	int x, y, destpitch;
	int nglyph;
	int h;
	unsigned long **glyphp;
	int glyphwidth;
	int fg;
{
	int line, shift_init, destaddr;
	unsigned char *base;

	SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING | SINGLEBANKED);
	SETWRITEMODE(4);	/* Transparent. */
	SETFOREGROUNDCOLOR(fg);
	SETPIXELMASK(0xff);

	/* Other character widths. Tricky, bit order is very awkward.  */
	/* We maintain a word straightforwardly LSB, and do the */
	/* bit order converting when writing 16-bit words. */

	/* Address is rounded to nearest 16-bit boundary (BY8 addressing). */
	destaddr = y * destpitch + x;
	shift_init = destaddr & 15;
	destaddr = (destaddr >> 3) & ~0x1;

	CIRRUSSETSINGLE(destaddr);
	base = CIRRUSSINGLEBASE();

	line = 0;
	while (line < h) {
		unsigned long dworddata;
		int shift;
		int i;
		unsigned char *destp;
		shift = shift_init;
		destp = base + destaddr;
		dworddata = 0;
		i = 0;
		while (i < nglyph) {
			dworddata += glyphp[i][line] << shift;
			shift += glyphwidth;
			if (shift >= 16) {
				/* Write a 16-bit word. */
				*(unsigned short *)destp =
					byte_reversed[dworddata & 0xff] +
					(byte_reversed[(dworddata & 0xff00) >> 8] << 8);
				shift -= 16;
				dworddata >>= 16;
				destp += 2;
			}
			i++;
		}
		if (shift > 0) {
			*(unsigned short *)destp =
				byte_reversed[dworddata & 0xff] +
				(byte_reversed[(dworddata & 0xff00) >> 8] << 8);
		}
		line++;
		destaddr += destpitch >> 3;
	}

	/* Disable extended write modes and BY8 addressing. */
	if (cirrusUseLinear) {
		SETMODEEXTENSIONS(SINGLEBANKED);
	}
	else {
		SETMODEEXTENSIONS(DOUBLEBANKED);
	}
	SETWRITEMODE(0);
	SETFOREGROUNDCOLOR(0x00);	/* Disable set/reset. */
}

#endif /* !defined(CIRRUSMMIO) */

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

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_teblt8.c,v 3.1 1996/10/17 15:20:48 dawes Exp $ */

/*
 * Modified for Cirrus by Harm Hanemaayer (hhanemaa@cs.ruu.nl).
 *
 * We accelerate straightforward text writing for fonts with widths up to 32
 * pixels.
 */

/*
 * Modified for Chips by David Bateman (dbateman@ee.uts.edu.au)
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

#include "ct_driver.h"

#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
#include "ct_BltHiQV.h"
#else
#include "ct_BlitMM.h"
#endif
#else
#include "ct_Blitter.h"
#endif

#ifndef CHIPS_MMIO /* Only define this once */
unsigned int byte_reversed3[256] =
{
    0x000000, 0x0000E0, 0x00001C, 0x0000FC, 0x008003, 0x0080E3, 0x00801F, 0x0080FF,
    0x007000, 0x0070E0, 0x00701C, 0x0070FC, 0x00F003, 0x00F0E3, 0x00F01F, 0x00F0FF,
    0x000E00, 0x000EE0, 0x000E1C, 0x000EFC, 0x008E03, 0x008EE3, 0x008E1F, 0x008EFF,
    0x007E00, 0x007EE0, 0x007E1C, 0x007EFC, 0x00FE03, 0x00FEE3, 0x00FE1F, 0x00FEFF,
    0xC00100, 0xC001E0, 0xC0011C, 0xC001FC, 0xC08103, 0xC081E3, 0xC0811F, 0xC081FF,
    0xC07100, 0xC071E0, 0xC0711C, 0xC071FC, 0xC0F103, 0xC0F1E3, 0xC0F11F, 0xC0F1FF,
    0xC00F00, 0xC00FE0, 0xC00F1C, 0xC00FFC, 0xC08F03, 0xC08FE3, 0xC08F1F, 0xC08FFF,
    0xC07F00, 0xC07FE0, 0xC07F1C, 0xC07FFC, 0xC0FF03, 0xC0FFE3, 0xC0FF1F, 0xC0FFFF,
    0x380000, 0x3800E0, 0x38001C, 0x3800FC, 0x388003, 0x3880E3, 0x38801F, 0x3880FF,
    0x387000, 0x3870E0, 0x38701C, 0x3870FC, 0x38F003, 0x38F0E3, 0x38F01F, 0x38F0FF,
    0x380E00, 0x380EE0, 0x380E1C, 0x380EFC, 0x388E03, 0x388EE3, 0x388E1F, 0x388EFF,
    0x387E00, 0x387EE0, 0x387E1C, 0x387EFC, 0x38FE03, 0x38FEE3, 0x38FE1F, 0x38FEFF,
    0xF80100, 0xF801E0, 0xF8011C, 0xF801FC, 0xF88103, 0xF881E3, 0xF8811F, 0xF881FF,
    0xF87100, 0xF871E0, 0xF8711C, 0xF871FC, 0xF8F103, 0xF8F1E3, 0xF8F11F, 0xF8F1FF,
    0xF80F00, 0xF80FE0, 0xF80F1C, 0xF80FFC, 0xF88F03, 0xF88FE3, 0xF88F1F, 0xF88FFF,
    0xF87F00, 0xF87FE0, 0xF87F1C, 0xF87FFC, 0xF8FF03, 0xF8FFE3, 0xF8FF1F, 0xF8FFFF,
    0x070000, 0x0700E0, 0x07001C, 0x0700FC, 0x078003, 0x0780E3, 0x07801F, 0x0780FF,
    0x077000, 0x0770E0, 0x07701C, 0x0770FC, 0x07F003, 0x07F0E3, 0x07F01F, 0x07F0FF,
    0x070E00, 0x070EE0, 0x070E1C, 0x070EFC, 0x078E03, 0x078EE3, 0x078E1F, 0x078EFF,
    0x077E00, 0x077EE0, 0x077E1C, 0x077EFC, 0x07FE03, 0x07FEE3, 0x07FE1F, 0x07FEFF,
    0xC70100, 0xC701E0, 0xC7011C, 0xC701FC, 0xC78103, 0xC781E3, 0xC7811F, 0xC781FF,
    0xC77100, 0xC771E0, 0xC7711C, 0xC771FC, 0xC7F103, 0xC7F1E3, 0xC7F11F, 0xC7F1FF,
    0xC70F00, 0xC70FE0, 0xC70F1C, 0xC70FFC, 0xC78F03, 0xC78FE3, 0xC78F1F, 0xC78FFF,
    0xC77F00, 0xC77FE0, 0xC77F1C, 0xC77FFC, 0xC7FF03, 0xC7FFE3, 0xC7FF1F, 0xC7FFFF,
    0x3F0000, 0x3F00E0, 0x3F001C, 0x3F00FC, 0x3F8003, 0x3F80E3, 0x3F801F, 0x3F80FF,
    0x3F7000, 0x3F70E0, 0x3F701C, 0x3F70FC, 0x3FF003, 0x3FF0E3, 0x3FF01F, 0x3FF0FF,
    0x3F0E00, 0x3F0EE0, 0x3F0E1C, 0x3F0EFC, 0x3F8E03, 0x3F8EE3, 0x3F8E1F, 0x3F8EFF,
    0x3F7E00, 0x3F7EE0, 0x3F7E1C, 0x3F7EFC, 0x3FFE03, 0x3FFEE3, 0x3FFE1F, 0x3FFEFF,
    0xFF0100, 0xFF01E0, 0xFF011C, 0xFF01FC, 0xFF8103, 0xFF81E3, 0xFF811F, 0xFF81FF,
    0xFF7100, 0xFF71E0, 0xFF711C, 0xFF71FC, 0xFFF103, 0xFFF1E3, 0xFFF11F, 0xFFF1FF,
    0xFF0F00, 0xFF0FE0, 0xFF0F1C, 0xFF0FFC, 0xFF8F03, 0xFF8FE3, 0xFF8F1F, 0xFF8FFF,
    0xFF7F00, 0xFF7FE0, 0xFF7F1C, 0xFF7FFC, 0xFFFF03, 0xFFFFE3, 0xFFFF1F, 0xFFFFFF, 
};
#endif

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

#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
void
ctHiQVImageGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
#else
void
ctMMIOImageGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
#endif
#else
void
ctcfbImageGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
#endif
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

#ifdef DEBUG
#ifdef CHIPS_MMIO
    ErrorF("CHIPS:ctMMIOImageGlyphBlt(%d, %d, %d)\n", xInit, yInit, nglyph);
#else
    ErrorF("CHIPS:ctcfbImageGlyphBlt(%d, %d, %d)\n", xInit, yInit, nglyph);
#endif
#endif

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    widthDst = vga256InfoRec.displayWidth * vgaBytesPerPixel;
#ifdef CHIPS_HIQV
    fontwidthlimit = 32;
#else
    if (vgaBitsPerPixel == 24) {
	fontwidthlimit = 16;
    } else {
	fontwidthlimit = 32;
    }
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
    if (glyphWidthBytes != 4 || glyphWidth > fontwidthlimit) {
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

    destaddr = y * widthDst + (x * vgaBytesPerPixel);

    ctSETDSTADDR(destaddr);
    ctSETSRCADDR(0);
    ctSETPITCH(0, widthDst);

    switch (vgaBitsPerPixel) {
    case 16:
	ctSETBGCOLOR16(pGC->bgPixel);
	ctSETFGCOLOR16(pGC->fgPixel);
	break;
    case 24:
#ifdef CHIPS_HIQV
	    ctSETBGCOLOR24(pGC->bgPixel);
	    ctSETFGCOLOR24(pGC->fgPixel);
#else
	{
	    /* The 6554x Blitter can only handle 8/16bpp fills 
	     * directly, Though you can do a grey fill, by a little
	     * bit of magic with the 8bpp fill */
	    unsigned char red, green, blue;

	    red = pGC->bgPixel & 0xFF;
	    green = (pGC->bgPixel >> 8) & 0xFF;
	    blue = (pGC->bgPixel >> 16) & 0xFF;
	    if (red ^ green || green ^ blue || blue ^ red) {
		cfb24ImageGlyphBlt8(pDrawable, pGC, xInit, yInit,
		    nglyph, ppci, pglyphBase);
		return;
	    } else {
		ctSETBGCOLOR8(pGC->bgPixel);
	    }
	    red = pGC->fgPixel & 0xFF;
	    green = (pGC->fgPixel >> 8) & 0xFF;
	    blue = (pGC->fgPixel >> 16) & 0xFF;
	    if (red ^ green || green ^ blue || blue ^ red) {
		cfb24ImageGlyphBlt8(pDrawable, pGC, xInit, yInit,
		    nglyph, ppci, pglyphBase);
		return;
	    } else {
		ctSETFGCOLOR8(pGC->fgPixel);
	    }
	}
#endif
	break;
    default:			       /* 8 */
	ctSETBGCOLOR8(pGC->bgPixel);
	ctSETFGCOLOR8(pGC->fgPixel);
	break;
    }

    ctSETROP(ctAluConv[GXcopy & 0xf] | ctSRCMONO | ctSRCSYSTEM |
	ctTOP2BOTTOM | ctLEFT2RIGHT);
#ifdef CHIPS_HIQV
    ctSETMONOCTL(ctDWORDALIGN);
#endif

    blitwidth = glyphWidth * nglyph;
    ctSETHEIGHTWIDTHGO(h, blitwidth * vgaBytesPerPixel);

    /* Write bitmap to video memory (for BitBlt engine to process). */
    /* Gather bytes until we have a dword to write. Doubleword is   */
    /* LSByte first, and MSBit first in each byte, as required for  */
    /* the blit data. */

    CollectCharacters(glyphp, nglyph, pglyphBase, ppci);
    if (!ctisHiQV32 && (vgaBitsPerPixel == 24)) {
	ctTransferText24(nglyph, h, glyphp, glyphWidth, ctBltDataWindow);
    } else {
	ctTransferText(nglyph, h, glyphp, glyphWidth, ctBltDataWindow);
#ifdef CHIPS_HIQV
	if (((((blitwidth + 31) & ~31) >> 5) * h) & 0x1)
	    *(unsigned int *)ctBltDataWindow = 0;
#endif
    }
    
#ifdef CHIPS_HIQV	/* For debugging the code */
    usleep(50);
    while(*(volatile unsigned int *)(ctMMIOBase + 0x10) & (0x80000000)) {
	ErrorF("Flush\n");
	*(unsigned int *)ctBltDataWindow = 0;
	usleep(5);
    }
#else
    ctBLTWAIT;
#endif
    DEALLOCATE_LOCAL(glyphp);
}

/*
 * Transparent text.
 */

#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
void 
ctHiQVPolyGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
#else
void 
ctMMIOPolyGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
#endif
#else
void 
ctcfbPolyGlyphBlt(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
#endif
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

#ifdef DEBUG
#ifdef CHIPS_MMIO
    ErrorF("CHIPS:ctMMIOPolyGlyphBlt(%d, %d, %d)\n", xInit, yInit, nglyph);
#else
    ErrorF("CHIPS:ctcfbPolyGlyphBlt(%d, %d, %d)\n", xInit, yInit, nglyph);
#endif
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
#ifdef CHIPS_HIQV
    fontwidthlimit = 32;
#else
    if (vgaBitsPerPixel == 24) {
	fontwidthlimit = 16;
    } else {
	fontwidthlimit = 32;
    }
#endif
    
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);
    if ((h | glyphWidth) == 0)
	return;

    /* We only do GXcopy polyglyph. */
    if ((pGC->alu != GXcopy) || (glyphWidthBytes != 4 || 
	    glyphWidth > fontwidthlimit)) {
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
	ctSETFGCOLOR16(pGC->fgPixel);
	break;
    case 24:
#ifdef CHIPS_HIQV
	ctSETFGCOLOR24(pGC->fgPixel);
#else
	{
	    /* The 6554x Blitter can only handle 8/16bpp fills 
	     * directly, Though you can do a grey fill, by a little
	     * bit of magic with the 8bpp fill */
	    unsigned char red, green, blue;

	    red = pGC->fgPixel & 0xFF;
	    green = (pGC->fgPixel >> 8) & 0xFF;
	    blue = (pGC->fgPixel >> 16) & 0xFF;
	    if (red ^ green || green ^ blue || blue ^ red) {
		cfb24PolyGlyphBlt8(pDrawable, pGC, xInit, yInit,
		    nglyph, ppci, pglyphBase);
		return;
	    } else {
		ctSETFGCOLOR8(pGC->fgPixel);
	    }
	}
#endif
	break;
    default:			       /* 8 */
	ctSETFGCOLOR8(pGC->fgPixel);
	break;
    }

    ctSETDSTADDR(destaddr);
    ctSETSRCADDR(0);
    ctSETPITCH(0, widthDst);
    ctSETROP(ctAluConv[GXcopy & 0xf] | ctSRCMONO | ctSRCSYSTEM |
	ctBGTRANSPARENT | ctTOP2BOTTOM | ctLEFT2RIGHT);
#ifdef CHIPS_HIQV
    ctSETMONOCTL(ctDWORDALIGN);
#endif
    blitwidth = glyphWidth * nglyph;
    ctSETHEIGHTWIDTHGO(h, blitwidth * vgaBytesPerPixel);
    CollectCharacters(glyphp, nglyph, pglyphBase, ppci);
    if (!ctisHiQV32 && (vgaBitsPerPixel == 24)) {
	ctTransferText24(nglyph, h, glyphp, glyphWidth, ctBltDataWindow);
    } else {
	ctTransferText(nglyph, h, glyphp, glyphWidth, ctBltDataWindow);
#ifdef CHIPS_HIQV
	if (((((blitwidth + 31) & ~31) >> 5) * h) & 0x1)
	    *(unsigned int *)ctBltDataWindow = 0;
#endif
    }
#ifdef CHIPS_HIQV	/* For debugging the code */
    usleep(50);
    while(*(volatile unsigned int *)(ctMMIOBase + 0x10) & (0x80000000)) {
	ErrorF("Flush\n");
	*(unsigned int *)ctBltDataWindow = 0;
	usleep(5);
    }
#else
    ctBLTWAIT;
#endif
    DEALLOCATE_LOCAL(glyphp);
    return;
}

/*
 * Low-level text transfer routines. Compile it only once
 */

/* 
 * Note that the 6554x machines use 8bpp colour expansion for 24bpp.
 * Hence, 3 bits will need to be written for every bit in the image
 * glyph. I'm uncertain what effect this will have on the speed of
 * this routine. The test for the special case however slows up the
 * other colour expansions. Hence the 6554x 24bpp case is implemented
 * with its own text transfer routine
 */

#ifndef CHIPS_MMIO

#if 0 /* Not used, now in ct_textblt.s. Only for glyph < 16 width */
void
ctTransferText(nglyph, h, glyphp, glyphwidth, base)
    int nglyph;
    int h;
    unsigned long **glyphp;
    unsigned char *base;
{
    int shift;
    unsigned long dworddata;
    unsigned char *data;
    int i, line, databytes;

    /* Other character widths. Tricky, bit order is very awkward.  */
    /* We maintain a word straightforwardly LSB, and do the */
    /* bit order converting when writing 32-bit double words. */

#ifdef DEBUG
    ErrorF("CHIPS:ctTransferText(%d, %d, %d, 0x%X)\n", nglyph, h,
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
        for(i = 0; i < (((glyphwidth * nglyph + 31 ) & ~31) >> 5); i++)
            *(unsigned int *)base=*(unsigned int *)(data + 4 * i);
	line++;
    }
    DEALLOCATE_LOCAL(data);
}
#endif

void
ctTransferText24(nglyph, h, glyphp, glyphwidth, base)
    int nglyph;
    int h;
    unsigned long **glyphp;
    unsigned char *base;
{
    int shift;
    unsigned long dworddata;
    unsigned char *data;
    int i, line, databytes;

    /* Other character widths. Tricky, bit order is very awkward.  */
    /* We maintain a word straightforwardly LSB, and do the */
    /* bit order converting when writing 32-bit double words. */

#ifdef DEBUG
    ErrorF("CHIPS:ctTransferText24(%d, %d, %d, 0x%X)\n", nglyph, h,
	glyphwidth, base);
#endif

    line = 0;
    data = (unsigned char *)ALLOCATE_LOCAL(3 * ((nglyph * glyphwidth + 7) 
	    & ~7));
    while (line < h) {
	shift = 0;
	dworddata = 0;
	databytes = 0;
	i = 0;
	while (i < nglyph) {
	    dworddata += glyphp[i][line] << shift;
	    shift += glyphwidth;
	    if (shift >= 16) {
		/* Store the current word */
		*(unsigned int *)(data + databytes) =
		    byte_reversed3[dworddata & 0xff] +
		    ((byte_reversed3[(dworddata & 0xff00) >> 8] & 0xff)
		    << 24);
		*(unsigned short *)(data + databytes + 4) =
		    ((byte_reversed3[(dworddata & 0xff00) >> 8] & 0xffff00) 
		     >> 8) ;
		databytes += 6;
		shift -= 16;
		dworddata >>= 16;
	    }
	    i++;
	}
	if (shift > 0) {
	    *(unsigned int *)(data + databytes) =
	        byte_reversed3[dworddata & 0xff] +
	        ((byte_reversed3[(dworddata & 0xff00) >> 8] & 0xff)
		 << 24);
	    *(unsigned short *)(data + databytes + 4) =
	        ((byte_reversed3[(dworddata & 0xff00) >> 8] & 0xffff00) 
		 >> 8) ;
	}
        for(i = 0; i < (((3 * glyphwidth * nglyph + 31 ) & ~31) >> 5); i++)
            *(unsigned int *)base=*(unsigned int *)(data + 4 * i);
	line++;
    }
    DEALLOCATE_LOCAL(data);
}

#endif

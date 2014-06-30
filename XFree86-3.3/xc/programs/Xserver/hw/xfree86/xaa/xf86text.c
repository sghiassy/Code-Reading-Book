/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86text.c,v 3.6.2.1 1997/05/17 12:25:22 dawes Exp $ */

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
 * Written by Harm Hanemaayer (H.Hanemaayer@inter.nl.net).
 */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
/* PSZ doesn't matter. */
#define PSZ 8
#include	"cfb.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"

#include "xf86.h"
#include "xf86xaa.h"
#include "xf86local.h"
#include "xf86expblt.h"

/*
 * These functions check for clipping, and only dispatch to the
 * hardware accelerated function if the text string is not clipped.
 *
 * A nice improvement would be to determine which substring of
 * characters fall within a clipping region and call the accelerated
 * text functions for these substrings. This doesn't seem to hard
 * to do.
 *
 * Note that fall-back functions (usually cfb) are called for the
 * clipped case.
 *
 * When a font-cache is implemented (which is not necessarily the fastest
 * way to draw text), you would probably be clipping each character.
 */

void
xf86ImageGlyphBltTE(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GCPtr pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    int widthGlyph;
    int h;
    int x, y;
    BoxRec bbox;		       /* for clipping */
    int glyphWidth;		       /* Character width in pixels. */

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
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
        (*(xf86GCInfoRec.ImageGlyphBltFallBack))(
	    pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
    case rgnOUT:
	return;
    }

    (*(xf86AccelInfoRec.ImageTextTE))(
        pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
}


/*
 * Non-Terminal emulator fonts (any font).
 */

void
xf86ImageGlyphBltNonTE(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    int widthGlyph;
    int h;
    int x, y;
    BoxRec bbox;		       /* for clipping */
    int glyphWidth, width;
    int i;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    if ((h | glyphWidth) == 0)
	return;

    if (glyphWidth < 0) {   /* Catch right-to-left fonts */
        (*(xf86GCInfoRec.ImageGlyphBltFallBack))(
	    pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
        return;
    }

    width = 0;

    /* For ImageGlyph use the width of the backing rectangle */
    for (i = 0; i < nglyph; i++)
	width += ppci[i]->metrics.characterWidth;

    x = xInit + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;
    bbox.x1 = x;
    bbox.x2 = x + width;
    bbox.y1 = y;
    bbox.y2 = y + h;

    switch (RECT_IN_REGION(pGC->pScreen, cfbGetCompositeClip(pGC), &bbox)) {
    case rgnPART:
        (*(xf86GCInfoRec.ImageGlyphBltFallBack))(
	    pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    case rgnOUT:
	return;
    }

    (*(xf86AccelInfoRec.ImageTextNonTE))(pDrawable, pGC, xInit, yInit,
        nglyph, ppci, pglyphBase);
}


void
xf86PolyGlyphBltTE(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    unsigned int *pdstBase;
    int widthDst;
    int widthGlyph;
    int h;
    int x, y;
    BoxRec bbox;		       /* for clipping */

    int glyphWidth;		       /* Character width in pixels. */

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);

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
	(*(xf86GCInfoRec.PolyGlyphBltFallBack))(
	    pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
        return;
    case rgnOUT:
	return;
    }

    (*(xf86AccelInfoRec.PolyTextTE))(pDrawable, pGC, xInit, yInit,
        nglyph, ppci, pglyphBase);
}


void
xf86PolyGlyphBltNonTE(pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    unsigned int *pdstBase;
    int widthDst;
    int widthGlyph;
    int h;
    int x, y;
    BoxRec bbox;		       /* for clipping */
    int width, i;

    int glyphWidth;		       /* Character width in pixels. */

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);

    h = FONTASCENT(pfont) + FONTDESCENT(pfont);
    if ((h | glyphWidth) == 0)
	return;

    if (glyphWidth < 0) {  /* Catch here right-to-left glyphs */
	(*(xf86GCInfoRec.PolyGlyphBltFallBack))(
	    pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
        return;
    }

    width = 0;
    for (i = 0; i < nglyph - 1; i++)
	width += ppci[i]->metrics.characterWidth +
	    ppci[i + 1]->metrics.leftSideBearing -
	    ppci[i]->metrics.leftSideBearing;
    width += ppci[nglyph - 1]->metrics.rightSideBearing
	    - ppci[nglyph - 1]->metrics.leftSideBearing;

    x = xInit + (*ppci)->metrics.leftSideBearing + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;
    bbox.x1 = x;
    bbox.x2 = x + width;
    bbox.y1 = y;
    bbox.y2 = y + h;

    switch (RECT_IN_REGION(pGC->pScreen, cfbGetCompositeClip(pGC), &bbox)) {
    case rgnPART:
	(*(xf86GCInfoRec.PolyGlyphBltFallBack))(
	    pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
        return;
    case rgnOUT:
	return;
    }

    (*(xf86AccelInfoRec.PolyTextNonTE))(pDrawable, pGC, xInit, yInit,
	nglyph, ppci, pglyphBase);
}


/*
 * The code below implements non-clipped text drawing
 * using hardware bitmap color expansion.
 */

/*
 * Low level function declarations.
 */

static void DrawTextTECPUToScreenColorExpand(
#if NeedFunctionPrototypes
    int nglyph,
    int h,
    unsigned int **glyphp,
    int glyphwidth
#endif
);

static void DrawTextTECPUToScreenColorExpandBytePad(
#if NeedFunctionPrototypes
    int nglyph,
    int h,
    unsigned int **glyphp,
    int glyphwidth
#endif
);

static void DrawTextTECPUToScreenColorExpandNoPad(
#if NeedFunctionPrototypes
    int nglyph,
    int h,
    unsigned int **glyphp,
    int glyphwidth
#endif
);

static void DrawTextTEScreenToScreenColorExpand(
#if NeedFunctionPrototypes
    int nglyph,
    int w,
    int h,
    unsigned int **glyphp,
    int glyphwidth
#endif
);

static void DrawTextNonTECPUToScreenColorExpand(
#if NeedFunctionPrototypes
    int nglyph,
    int w,
    int h,
    NonTEGlyphInfo *glyphinfop
#endif
);

static void DrawTextNonTEScreenToScreenColorExpand(
#if NeedFunctionPrototypes
    int nglyph,
    int w,
    int h,
    NonTEGlyphInfo *glyphinfop
#endif
);

/* 
 * This function collects pointers to the character bitmaps.
 */

static void
CollectCharacters(glyphp, nglyph, pglyphBase, ppci)
    unsigned int **glyphp;
    unsigned int nglyph;
    unsigned char *pglyphBase;
    CharInfoPtr *ppci;
{
    int i;

    for (i = 0; i < nglyph; i++) {
        glyphp[i] = (unsigned int *)FONTGLYPHBITS(pglyphBase,
	    *ppci++);
    }
}

/* 
 * What follows are text functions for "TE" (fixed-size) fonts:
 * - ImageText using CPU-to-screen color expansion.
 * - ImageText using scanline screen-to-screen color expansion.
 * - PolyText using CPU-to-screen color expansion.
 * - PolyText using scanline screen-to-screen color expansion.
 */

void
xf86ImageTextTECPUToScreenColorExpand(pDrawable, pGC, xInit, yInit,
nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    unsigned int *pdstBase;
    int widthDst;
    int widthGlyph;
    int w, h;
    int x, y;

    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    int i;

    /* Characters are padded to 4 bytes. */
    unsigned int **glyphp;
    int destaddr, blitwidth;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    /*
     * Check for non-standard glyphs, glyphs that are too wide.
     */
    if (glyphWidthBytes != 4 || glyphWidth > 32) {
        xf86GCInfoRec.ImageGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    }

    if ((h | glyphWidth) == 0)
	return;

    x = xInit + FONTMAXBOUNDS(pfont, leftSideBearing) + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;
    w = nglyph * glyphWidth;

    /*
     * If only color expansion with transparency is supported, then
     * do it in two steps -- first do the background with a solid fill,
     * then draw the text with transparency.
     */
    if (xf86AccelInfoRec.ColorExpandFlags & ONLY_TRANSPARENCY_SUPPORTED) {
        /* First fill-in the background. */
        xf86AccelInfoRec.SetupForFillRectSolid(pGC->bgPixel, GXcopy,
            pGC->planemask);
        xf86AccelInfoRec.SubsequentFillRectSolid(x, y, w, h);
    }

    /* Allocate list of pointers to glyph bitmaps. */
    glyphp = (unsigned int **)ALLOCATE_LOCAL(nglyph * sizeof(unsigned int *));

    CollectCharacters(glyphp, nglyph, pglyphBase, ppci);

    if (xf86AccelInfoRec.ColorExpandFlags & ONLY_TRANSPARENCY_SUPPORTED) {
        if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
            xf86AccelInfoRec.Sync();
        xf86AccelInfoRec.SetupForCPUToScreenColorExpand(
            -1, pGC->fgPixel, GXcopy, pGC->planemask);
    }
    else
        xf86AccelInfoRec.SetupForCPUToScreenColorExpand(
            pGC->bgPixel, pGC->fgPixel, GXcopy, pGC->planemask);

    xf86AccelInfoRec.SubsequentCPUToScreenColorExpand(
        x, y, w, h, 0);

    if (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_BYTE)
        DrawTextTECPUToScreenColorExpandBytePad(
            nglyph, h, glyphp, glyphWidth);
    else
    if (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_NO_PAD)
        DrawTextTECPUToScreenColorExpandNoPad(
            nglyph, h, glyphp, glyphWidth);
    else
        DrawTextTECPUToScreenColorExpand(nglyph, h, glyphp, glyphWidth);

    DEALLOCATE_LOCAL(glyphp);
}


void
xf86ImageTextTEScreenToScreenColorExpand(pDrawable, pGC, xInit, yInit,
nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    unsigned int *pdstBase;
    int widthDst;
    int widthGlyph;
    int w, h;
    int x, y;

    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    int i;

    /* Characters are padded to 4 bytes. */
    unsigned int **glyphp;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);
    w = glyphWidth * nglyph;

    /*
     * Check for non-standard glyphs, glyphs that are too wide,
     * and for strings whose scanlines are too large to fit in the
     * bitmap buffer in video memory.
     */
    if (glyphWidthBytes != 4 || glyphWidth > 32 || ((w + 31)
    & ~31) / 8 * xf86AccelInfoRec.PingPongBuffers >
    xf86AccelInfoRec.ScratchBufferSize) {
        xf86GCInfoRec.ImageGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    }

    if ((h | glyphWidth) == 0)
	return;

    x = xInit + FONTMAXBOUNDS(pfont, leftSideBearing) + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;

    if (xf86AccelInfoRec.ColorExpandFlags & ONLY_TRANSPARENCY_SUPPORTED) {
        /* First fill-in the background. */
        xf86AccelInfoRec.SetupForFillRectSolid(pGC->bgPixel, GXcopy,
            pGC->planemask);
        xf86AccelInfoRec.SubsequentFillRectSolid(x, y, w, h);
    }

    /* Allocate list of pointers to glyph bitmaps. */
    glyphp = (unsigned int **)ALLOCATE_LOCAL(nglyph * sizeof(unsigned int *));

    CollectCharacters(glyphp, nglyph, pglyphBase, ppci);

    if (xf86AccelInfoRec.ColorExpandFlags & ONLY_TRANSPARENCY_SUPPORTED) {
        if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
            xf86AccelInfoRec.Sync();
        xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand(
            x, y, w, h, -1, pGC->fgPixel, GXcopy, pGC->planemask);
    }
    else
        xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand(
            x, y, w, h, pGC->bgPixel, pGC->fgPixel, GXcopy, pGC->planemask);

    DrawTextTEScreenToScreenColorExpand(nglyph, w, h, glyphp, glyphWidth);

    DEALLOCATE_LOCAL(glyphp);
}


void
xf86PolyTextTECPUToScreenColorExpand(pDrawable, pGC, xInit, yInit,
nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    unsigned int *pdstBase;
    int widthDst;
    int widthGlyph;
    int h;
    int x, y;

    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    int i;

    /* Characters are padded to 4 bytes. */
    unsigned int **glyphp;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    /*
     * Check for non-standard glyphs, glyphs that are too wide.
     */
    if (glyphWidthBytes != 4 || glyphWidth > 32) {
        xf86GCInfoRec.PolyGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    }

    if ((h | glyphWidth) == 0)
	return;

    x = xInit + FONTMAXBOUNDS(pfont, leftSideBearing) + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;

    /* Allocate list of pointers to glyph bitmaps. */
    glyphp = (unsigned int **)ALLOCATE_LOCAL(nglyph * sizeof(unsigned int *));

    CollectCharacters(glyphp, nglyph, pglyphBase, ppci);
    
    xf86AccelInfoRec.SetupForCPUToScreenColorExpand(
        -1, pGC->fgPixel, pGC->alu, pGC->planemask);

    xf86AccelInfoRec.SubsequentCPUToScreenColorExpand(
        x, y, nglyph * glyphWidth, h, 0);

    if (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_BYTE)
        DrawTextTECPUToScreenColorExpandBytePad(
            nglyph, h, glyphp, glyphWidth);
    else
    if (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_NO_PAD)
        DrawTextTECPUToScreenColorExpandNoPad(
            nglyph, h, glyphp, glyphWidth);
    else
        DrawTextTECPUToScreenColorExpand(nglyph, h, glyphp, glyphWidth);

    DEALLOCATE_LOCAL(glyphp);
}

void
xf86PolyTextTEScreenToScreenColorExpand(pDrawable, pGC, xInit, yInit,
nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    unsigned int *pdstBase;
    int widthDst;
    int widthGlyph;
    int w, h;
    int x, y;

    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    int i;

    /* Characters are padded to 4 bytes. */
    unsigned int **glyphp;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);
    w = glyphWidth * nglyph;

    /*
     * Check for non-standard glyphs, glyphs that are too wide,
     * and for strings whose scanlines are too large to fit in the
     * bitmap buffer in video memory.
     */
    if (glyphWidthBytes != 4 || glyphWidth > 32 || ((w + 31)
    & ~31) / 8 * xf86AccelInfoRec.PingPongBuffers >
    xf86AccelInfoRec.ScratchBufferSize) {
        xf86GCInfoRec.PolyGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    }

    if ((h | glyphWidth) == 0)
	return;

    x = xInit + FONTMAXBOUNDS(pfont, leftSideBearing) + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;

    /* Allocate list of pointers to glyph bitmaps. */
    glyphp = (unsigned int **)ALLOCATE_LOCAL(nglyph * sizeof(unsigned int *));

    CollectCharacters(glyphp, nglyph, pglyphBase, ppci);

    xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand(
        x, y, w, h, -1, pGC->fgPixel, pGC->alu, pGC->planemask);

    DrawTextTEScreenToScreenColorExpand(nglyph, w, h, glyphp, glyphWidth);

    DEALLOCATE_LOCAL(glyphp);
}

/* 
 * This function collects glyph info for Non-TE text strings.
 * It returns the total width of the string in pixels.
 */

static int
CollectCharacterInfo(glyphinfop, nglyph, pglyphBase, ppci, maxascent)
    NonTEGlyphInfo *glyphinfop;
    unsigned int nglyph;
    unsigned char *pglyphBase;
    CharInfoPtr *ppci;
    int maxascent;
{
    int i, w;

    w = 0;
    for (i = 0; i < nglyph; i++) {
        /*
         * Note that the glyph bitmap address is pre-adjusted, so that the
         * offset is the same for differently sized glyphs when writing a
         * text scanline.
         *
         * The width of each character should be carefully determined,
         * considering the leftSideBearing of the next character, since
         * characters can overlap horizontally.
         */
	glyphinfop[i].bitsp = (unsigned int *)FONTGLYPHBITS(
	    pglyphBase, ppci[i]) - (maxascent - ppci[i]->metrics.ascent);
        if (i < nglyph - 1)
	    glyphinfop[i].width = ppci[i]->metrics.characterWidth +
	        ppci[i + 1]->metrics.leftSideBearing -
	        ppci[i]->metrics.leftSideBearing;
	else
	    glyphinfop[i].width = max (ppci[i]->metrics.characterWidth,
		(ppci[i]->metrics.rightSideBearing
                - ppci[i]->metrics.leftSideBearing));
	w += glyphinfop[i].width;
	glyphinfop[i].firstline = maxascent - ppci[i]->metrics.ascent;
	glyphinfop[i].lastline = maxascent + ppci[i]->metrics.descent - 1;
    }
    return w;
}

/* 
 * What follows are equivalent functions for "Non-TE" ("proportional") fonts:
 * - ImageText using CPU-to-screen color expansion.
 * - ImageText using scanline screen-to-screen color expansion.
 * - PolyText using CPU-to-screen color expansion.
 * - PolyText using scanline screen-to-screen color expansion.
 */

void
xf86ImageTextNonTECPUToScreenColorExpand(pDrawable, pGC, xInit, yInit,
nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    unsigned int *pdstBase;
    int widthDst;
    int widthGlyph;
    int w, h;
    int x, y;
    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    int i;

    NonTEGlyphInfo *glyphinfop;

    xRectangle backrect;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    if ((h | glyphWidth) == 0)
	return;

    /*
     * Check for non-standard glyphs, glyphs that are too wide.
     */
    if (glyphWidthBytes != 4 || glyphWidth > 32) {
        xf86GCInfoRec.ImageGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    }

    /*
     * The "leftSideBearing" of the first character is added to the
     * x-coordinate.
     */
    x = xInit + (*ppci)->metrics.leftSideBearing + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;

    /* Allocate list of pointers to glyph info. */
    glyphinfop = (NonTEGlyphInfo *)ALLOCATE_LOCAL(nglyph *
        sizeof(NonTEGlyphInfo));

    w = CollectCharacterInfo(glyphinfop, nglyph, pglyphBase, ppci,
        FONTASCENT(pfont));

    /* Calculate the size of the backing rectangle */
    backrect.width = 0;
    for (i = 0; i < nglyph; i++)
	backrect.width += ppci[i]->metrics.characterWidth;
    backrect.x = xInit + pDrawable->x;
    backrect.y = y;
    backrect.height = h;

    /*
     * There is the possibility of overlapping characters with NonTE
     * fonts. Hence the only safe way to do them is to fill in the
     * backing rectangle first and then treat them as PolyText.
     */
    xf86AccelInfoRec.SetupForFillRectSolid(pGC->bgPixel, GXcopy,
        pGC->planemask);
    xf86AccelInfoRec.SubsequentFillRectSolid(backrect.x, backrect.y,
        backrect.width, backrect.height);
    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
        xf86AccelInfoRec.Sync();
    xf86AccelInfoRec.SetupForCPUToScreenColorExpand(
        -1, pGC->fgPixel, GXcopy, pGC->planemask);
    xf86AccelInfoRec.SubsequentCPUToScreenColorExpand(
        x, y, w, h, 0);
#if 0
    if (xf86AccelInfoRec.ColorExpandFlags & CPU_SCANLINE_PAD_BYTE)
        DrawTextNonTECPUToScreenColorExpandBytePad( nglyph, w, h, glyphinfop);
    else
#endif
        DrawTextNonTECPUToScreenColorExpand(nglyph, w, h, glyphinfop);

    DEALLOCATE_LOCAL(glyphinfop);
}


void
xf86ImageTextNonTEScreenToScreenColorExpand(pDrawable, pGC, xInit, yInit,
nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    unsigned int *pdstBase;
    int widthDst;
    int widthGlyph;
    int w, h;
    int x, y;

    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    int i;

    NonTEGlyphInfo *glyphinfop;
    xRectangle backrect;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    if ((h | glyphWidth) == 0)
	return;

    /* Allocate list of pointers to glyph info. */
    glyphinfop = (NonTEGlyphInfo *)ALLOCATE_LOCAL(nglyph *
        sizeof(NonTEGlyphInfo));

    w = CollectCharacterInfo(glyphinfop, nglyph, pglyphBase, ppci,
        FONTASCENT(pfont));

    /*
     * Check for non-standard glyphs, glyphs that are too wide,
     * and for strings whose scanlines are too large to fit in the
     * bitmap buffer in video memory.
     */
    if (glyphWidthBytes != 4 || glyphWidth > 32 || ((w + 31)
    & ~31) / 8 * xf86AccelInfoRec.PingPongBuffers >
    xf86AccelInfoRec.ScratchBufferSize) {
        xf86GCInfoRec.ImageGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	DEALLOCATE_LOCAL(glyphinfop);
	return;
    }

    /*
     * The "leftSideBearing" of the first character is added to the
     * x-coordinate.
     */
    x = xInit + (*ppci)->metrics.leftSideBearing + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;

    /* Calculate the size of the backing rectangle */
    backrect.width = 0;
    for (i = 0; i < nglyph; i++)
	backrect.width += ppci[i]->metrics.characterWidth;
    backrect.x = xInit + pDrawable->x;
    backrect.y = y;
    backrect.height = h;

    /*
     * There is the possibility of overlapping characters with NonTE
     * fonts. Hence the only safe way to do them is to fill in the
     * backing rectangle first and then treat them as PolyText.
     */
    xf86AccelInfoRec.SetupForFillRectSolid(pGC->bgPixel, GXcopy,
        pGC->planemask);
    xf86AccelInfoRec.SubsequentFillRectSolid(backrect.x, backrect.y,
        backrect.width, backrect.height);
    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
        xf86AccelInfoRec.Sync();
    xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand(
        x, y, w, h, -1, pGC->fgPixel, GXcopy, pGC->planemask);
#if 0
    if (xf86AccelInfoRec.ColorExpandFlags & CPU_SCANLINE_PAD_BYTE)
        DrawTextNonTEScreenToScreenColorExpandBytePad(
            nglyph, w, h, glyphinfop);
    else
#endif
        DrawTextNonTEScreenToScreenColorExpand(nglyph, w, h, glyphinfop);

    DEALLOCATE_LOCAL(glyphinfop);
}


void
xf86PolyTextNonTECPUToScreenColorExpand(pDrawable, pGC, xInit, yInit,
nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    unsigned int *pdstBase;
    int widthDst;
    int widthGlyph;
    int w, h;
    int x, y;

    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    int i;

    NonTEGlyphInfo *glyphinfop;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    /*
     * Check for non-standard glyphs, glyphs that are too wide.
     */
    if (glyphWidthBytes != 4 || glyphWidth > 32) {
        xf86GCInfoRec.PolyGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    }

    if ((h | glyphWidth) == 0)
	return;

    /*
     * The "leftSideBearing" of the first character is added to the
     * x-coordinate.
     */
    x = xInit + (*ppci)->metrics.leftSideBearing + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;

    /* Allocate list of pointers to glyph info. */
    glyphinfop = (NonTEGlyphInfo *)ALLOCATE_LOCAL(nglyph *
        sizeof(NonTEGlyphInfo));

    w = CollectCharacterInfo(glyphinfop, nglyph, pglyphBase, ppci,
        FONTASCENT(pfont));
    
    xf86AccelInfoRec.SetupForCPUToScreenColorExpand(
        -1, pGC->fgPixel, pGC->alu, pGC->planemask);

    xf86AccelInfoRec.SubsequentCPUToScreenColorExpand(
        x, y, w, h, 0);

#if 0
    if (xf86AccelInfoRec.ColorExpandFlags & CPU_SCANLINE_PAD_BYTE)
        DrawTextNonTECPUToScreenColorExpandBytePad(nglyph, w, h, glyphinfop);
    else
#endif
        DrawTextNonTECPUToScreenColorExpand(nglyph, w, h, glyphinfop);

    DEALLOCATE_LOCAL(glyphinfop);
}

void
xf86PolyTextNonTEScreenToScreenColorExpand(pDrawable, pGC, xInit, yInit,
nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    unsigned int *pdstBase;
    int widthDst;
    int widthGlyph;
    int w, h;
    int x, y;

    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    int i;

    NonTEGlyphInfo *glyphinfop;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    if ((h | glyphWidth) == 0)
	return;

    /* Allocate list of pointers to glyph info. */
    glyphinfop = (NonTEGlyphInfo *)ALLOCATE_LOCAL(nglyph *
        sizeof(NonTEGlyphInfo));

    w = CollectCharacterInfo(glyphinfop, nglyph, pglyphBase, ppci,
        FONTASCENT(pfont));

    /*
     * Check for non-standard glyphs, glyphs that are too wide,
     * and for strings whose scanlines are too large to fit in the
     * bitmap buffer in video memory.
     */
    if (glyphWidthBytes != 4 || glyphWidth > 32 || ((w + 31)
    & ~31) / 8 * xf86AccelInfoRec.PingPongBuffers >
    xf86AccelInfoRec.ScratchBufferSize) {
        xf86GCInfoRec.PolyGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
        DEALLOCATE_LOCAL(glyphinfop);
	return;
    }

    /*
     * The "leftSideBearing" of the first character is added to the
     * x-coordinate.
     */
    x = xInit + (*ppci)->metrics.leftSideBearing + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;

    xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand(
        x, y, w, h, -1, pGC->fgPixel, pGC->alu, pGC->planemask);

    DrawTextNonTEScreenToScreenColorExpand(nglyph, w, h, glyphinfop);

    DEALLOCATE_LOCAL(glyphinfop);
}



/*
 * The following are lower-level function that transfer the text
 * bitmap from the CPU to the screen to be color expanded.
 */
 
/*
 * This function is used for Imagetext/Polytext with TE font with
 * CPU-to-screen color expansion (DWORD padding at the end of scanlines).
 */

static void DrawTextTECPUToScreenColorExpand(nglyph, h, glyphp, glyphwidth)
    int nglyph;
    int h;
    unsigned int **glyphp;
    int glyphwidth;
{
    int bitmapwidth;
    int line;
    unsigned char *base;
    unsigned int *(*DrawTextScanlineFunc)(
#if NeedNestedPrototypes
        unsigned int *base,
        unsigned int **glyphp,
        int line,
        int nglyph,
        int glyphwidth
#endif
    );

    if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawTextScanlineFunc = xf86DrawTextScanline3MSBFirstFixedBase;
            else
                DrawTextScanlineFunc = xf86DrawTextScanline3MSBFirst;
        else
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawTextScanlineFunc = xf86DrawTextScanlineMSBFirstFixedBase;
            else
                DrawTextScanlineFunc = xf86DrawTextScanlineMSBFirst;
    else
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawTextScanlineFunc = xf86DrawTextScanline3FixedBase;
            else
                DrawTextScanlineFunc = xf86DrawTextScanline3;
        else
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawTextScanlineFunc = xf86DrawTextScanlineFixedBase;
            else
                DrawTextScanlineFunc = xf86DrawTextScanline;

    base = (unsigned char *)xf86AccelInfoRec.CPUToScreenColorExpandBase;

    line = 0;
    while (line < h) {
        if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
	    (*DrawTextScanlineFunc)(
	        (unsigned int *)base, glyphp, line, nglyph, glyphwidth);
	else {
	    base = (unsigned char *)(*DrawTextScanlineFunc)(
	        (unsigned int *)base, glyphp, line, nglyph, glyphwidth);
            if (base >=
            (unsigned char *)xf86AccelInfoRec.CPUToScreenColorExpandEndMarker)
                base = (unsigned char *)xf86AccelInfoRec.CPUToScreenColorExpandBase;
	}
	line++;
    }

    if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_PAD_QWORD) {
        int words;
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            words = ((glyphwidth * nglyph * 3 + 31) & ~31) * h / 32;
        else
            words = ((glyphwidth * nglyph + 31) & ~31) * h / 32;
        if (words & 1)
            *(unsigned int *)base = 0;
    }

    xf86AccelInfoRec.Sync();
}

/*
 * Used for Imagetext/Polytext with TE font with CPU-to-screen
 * color expansion (BYTE padding at the end of scanlines).
 *
 * This has not yet been tested. The ASM function in the old cirrus driver
 * implements similar functionality.
 */

static void DrawTextTECPUToScreenColorExpandBytePad(nglyph, h, glyphp,
glyphwidth)
    int nglyph;
    int h;
    unsigned int **glyphp;
    int glyphwidth;
{
    unsigned char *base;
    unsigned int *(*DrawTextFunc)(
#if NeedNestedPrototypes
        unsigned int *base,
        unsigned int **glyphp,
        int height,
        int nglyph,
        int glyphwidth
#endif
    );

    base = (unsigned char *)xf86AccelInfoRec.CPUToScreenColorExpandBase;

    if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
#if 0
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawTextFunc = xf86DrawTextScanline3MSBFirstFixedBase;
            else
                DrawTextFunc = xf86DrawTextScanline3MSBFirst;
        else
#endif
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawTextFunc = xf86DrawTextBytePadMSBFirstFixedBase;
            else
                DrawTextFunc = xf86DrawTextBytePadMSBFirst;
    else
#if 0
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawTextFunc = xf86DrawTextScanline3FixedBase;
            else
                DrawTextFunc = xf86DrawTextScanline3;
        else
#endif
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawTextFunc = xf86DrawTextBytePadFixedBase;
            else
                DrawTextFunc = xf86DrawTextBytePad;

    base = (unsigned char *)(*DrawTextFunc)(
	        (unsigned int *)base, glyphp, h, nglyph, glyphwidth);

    if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_PAD_QWORD) {
        int words;
        words = ((((glyphwidth * nglyph + 7) & ~7) * h + 31) & ~31) / 32;
        if (words & 1)
            *(unsigned int *)base = 0;
    }

    xf86AccelInfoRec.Sync();
}

/*
 * Used for Imagetext/Polytext with TE font with CPU-to-screen
 * color expansion (NO padding at the end of scanlines).
 *
 * This has not yet been tested.
 */

static void DrawTextTECPUToScreenColorExpandNoPad(nglyph, h, glyphp,
glyphwidth)
    int nglyph;
    int h;
    unsigned int **glyphp;
    int glyphwidth;
{
    unsigned char *base;
    unsigned int *(*DrawTextFunc)(
#if NeedNestedPrototypes
        unsigned int *base,
        unsigned int **glyphp,
        int height,
        int nglyph,
        int glyphwidth
#endif
    );

    base = (unsigned char *)xf86AccelInfoRec.CPUToScreenColorExpandBase;

    if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
#if 0
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawTextFunc = xf86DrawTextScanline3MSBFirstFixedBase;
            else
                DrawTextFunc = xf86DrawTextScanline3MSBFirst;
        else
#endif
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawTextFunc = xf86DrawTextNoPadMSBFirstFixedBase;
            else
                DrawTextFunc = xf86DrawTextNoPadMSBFirst;
    else
#if 0
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawTextFunc = xf86DrawTextScanline3FixedBase;
            else
                DrawTextFunc = xf86DrawTextScanline3;
        else
#endif
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawTextFunc = xf86DrawTextNoPadFixedBase;
            else
                DrawTextFunc = xf86DrawTextNoPad;

    base = (unsigned char *)(*DrawTextFunc)(
	        (unsigned int *)base, glyphp, h, nglyph, glyphwidth);

    if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_PAD_QWORD) {
        int words;
        words = ((glyphwidth * nglyph * h + 31) & ~31) / 32;
        if (words & 1)
            *(unsigned int *)base = 0;
    }

    xf86AccelInfoRec.Sync();
}


/*
 * The following function is used for Imagetext/Polytext with TE font
 * with buffered screen-to-screen color expansion ("ping pong
 * buffers"). Each scanline is first transferred from the CPU to a
 * scratch buffer in off-screen video memory, and then color expanded
 * to the on-screen destination.
 */

static void DrawTextTEScreenToScreenColorExpand(nglyph, w, h, glyphp, glyphwidth)
    int nglyph;
    int w;
    int h;
    unsigned int **glyphp;
    int glyphwidth;
{
    int bitmapwidth;
    int line;
    int offset, endoffset;
    unsigned int *(*DrawTextScanlineFunc)(
#if NeedNestedPrototypes
        unsigned int *base,
        unsigned int **glyphp,
        int line,
        int nglyph,
        int glyphwidth
#endif
    );

    if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            DrawTextScanlineFunc = xf86DrawTextScanline3MSBFirst;
        else
            DrawTextScanlineFunc = xf86DrawTextScanlineMSBFirst;
    else
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            DrawTextScanlineFunc = xf86DrawTextScanline3;
        else
            DrawTextScanlineFunc = xf86DrawTextScanline;

    /* Calculate the non-expanded bitmap width rounded up to 32-bit words, */
    /* in units of pixels. */
    if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
        bitmapwidth = ((w + 31) & ~31) * 3;
    else
        bitmapwidth = (w + 31) & ~31;
    endoffset = (bitmapwidth / 8) * xf86AccelInfoRec.PingPongBuffers;

    offset = 0;
    line = 0;
    while (line < h) {
	if (!(xf86AccelInfoRec.Flags & COP_FRAMEBUFFER_CONCURRENCY))
	    xf86AccelInfoRec.Sync();
	(*DrawTextScanlineFunc)((unsigned int *)
	    (xf86AccelInfoRec.ScratchBufferBase + offset),
	    glyphp, line, nglyph, glyphwidth);
	xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand(
	    (xf86AccelInfoRec.ScratchBufferAddr + offset) * 8);
	line++;
        /*
         * There is a number of buffers -- while the first one is being
         * blitted, the next one is initialized, and then the next,
         * and so on.
         */
	offset += bitmapwidth / 8;
	if (offset == endoffset)
	    offset = 0;
    }

    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
        xf86AccelInfoRec.Sync();
}

/*
 * This function is used for Imagetext/Polytext with Non-TE font with
 * CPU-to-screen color expansion (DWORD padding at the end of scanlines).
 */

static void
DrawTextNonTECPUToScreenColorExpand(nglyph, w, h, glyphinfop)
    int nglyph;
    int w;
    int h;
    NonTEGlyphInfo *glyphinfop;
{
    int bitmapwidth;
    int line;
    unsigned char *base;
    unsigned int *(*DrawTextScanlineFunc)(
#if NeedNestedPrototypes
        unsigned int *base,
        NonTEGlyphInfo *glyphinfop,
        int line,
        int nglyph
#endif
    );

    if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
#if 0
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            DrawTextScanlineFunc = xf86DrawNonTETextScanline3MSBFirst;
        else
#endif
            DrawTextScanlineFunc = xf86DrawNonTETextScanlineMSBFirst;
    else
#if 0
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            DrawTextScanlineFunc = xf86DrawNonTETextScanline3;
        else
#endif
            DrawTextScanlineFunc = xf86DrawNonTETextScanline;

    base = (unsigned char *)xf86AccelInfoRec.CPUToScreenColorExpandBase;

    line = 0;
    while (line < h) {
	base = (unsigned char *)(*DrawTextScanlineFunc)(
	    (unsigned int *)base, glyphinfop, line, nglyph);
        if (base >=
        (unsigned char *)xf86AccelInfoRec.CPUToScreenColorExpandEndMarker)
            base = (unsigned char *)xf86AccelInfoRec.CPUToScreenColorExpandBase;
	line++;
    }

    if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_PAD_QWORD) {
	int words;
	words = ((w + 31) & ~31) * h / 32;
	if (words & 1)
	    *(unsigned int *)base = 0;
    }

    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
        xf86AccelInfoRec.Sync();
}

/*
 * The following function is used for Imagetext/Polytext with Non-TE font
 * with buffered screen-to-screen color expansion ("ping pong
 * buffers"). Each scanline is first transferred from the CPU to a
 * scratch buffer in off-screen video memory, and then color expanded
 * to the on-screen destination.
 */

static void
DrawTextNonTEScreenToScreenColorExpand(nglyph, w, h, glyphinfop)
    int nglyph;
    int w;
    int h;
    NonTEGlyphInfo *glyphinfop;
{
    int bitmapwidth;
    int line;
    int offset, endoffset;
    /* Calculate the non-expanded bitmap width rounded up to 32-bit words, */
    /* in units of pixels. */
    bitmapwidth = (w + 31) & ~31;
    endoffset = (bitmapwidth / 8) * xf86AccelInfoRec.PingPongBuffers;

    offset = 0;
    line = 0;
    while (line < h) {
	if (!(xf86AccelInfoRec.Flags & COP_FRAMEBUFFER_CONCURRENCY))
	    xf86AccelInfoRec.Sync();
        if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
	    xf86DrawNonTETextScanlineMSBFirst((unsigned int *)
	        (xf86AccelInfoRec.ScratchBufferBase + offset),
	        glyphinfop, line, nglyph);
        else
	    xf86DrawNonTETextScanline((unsigned int *)
	        (xf86AccelInfoRec.ScratchBufferBase + offset),
	        glyphinfop, line, nglyph);
	xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand(
	    (xf86AccelInfoRec.ScratchBufferAddr + offset) * 8);
	line++;
        /*
         * There is a number of buffers -- while the first one is being
         * blitted, the next one is initialized, and then the next,
         * and so on.
         */
	offset += bitmapwidth / 8;
	if (offset == endoffset)
	    offset = 0;
    }

    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
        xf86AccelInfoRec.Sync();
}

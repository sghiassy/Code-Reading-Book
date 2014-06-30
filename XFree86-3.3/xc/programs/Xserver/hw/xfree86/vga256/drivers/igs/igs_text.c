/*
 * Copyright 1997
 * Digital Equipment Corporation. All rights reserved.
 * This software is furnished under license and may be used and copied only in 
 * accordance with the following terms and conditions.  Subject to these 
 * conditions, you may download, copy, install, use, modify and distribute 
 * this software in source and/or binary form. No title or ownership is 
 * transferred hereby.
 * 1) Any source code used, modified or distributed must reproduce and retain 
 *    this copyright notice and list of conditions as they appear in the 
 *    source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of Digital 
 *    Equipment Corporation. Neither the "Digital Equipment Corporation" name 
 *    nor any trademark or logo of Digital Equipment Corporation may be used 
 *    to endorse or promote products derived from this software without the 
 *    prior written permission of Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied warranties,
 *    including but not limited to, any implied warranties of merchantability,
 *    fitness for a particular purpose, or non-infringement are disclaimed. In
 *    no event shall DIGITAL be liable for any damages whatsoever, and in 
 *    particular, DIGITAL shall not be liable for special, indirect, 
 *    consequential, or incidental damages or damages for lost profits, loss 
 *    of revenue or loss of use, whether such damages arise in contract, 
 *    negligence, tort, under statute, in equity, at law or otherwise, even if
 *    advised of the possibility of such damage. 
 */
/*
 * The CyberPro2010 driver is based on sample code provided by IGS 
 * Technologies, Inc. http://www.integraphics.com.
 * IGS Technologies, Inc makes no representations about the suitability of 
 * this software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

/* This has been modified from sections of xf86text.c and xf86expblt.c */
#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#define PSZ 8
#include	"cfb.h"
#include        "cfbmskbits.h"
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
#include "vga.h"

#include "igs_accel.h"
#include "igs_driver.h"

void igsDrawTextTE();
void igsDrawTextNonTE();
static void igsDrawTextScanlineWidth6();
static void igsDrawTextScanlineWidth8();
static void igsDrawTextScanlineWidth10();
static void igsDrawTextScanlineWidth12();
static void igsDrawTextScanlineWidth14();
static void igsDrawTextScanlineWidth16();
static void igsDrawTextScanlineWidth18();
static void igsDrawTextScanlineWidth24();
static void igsCollectCharacters();
static int igsCollectCharacterInfo();

extern int glyphwidth_stretchsize[32];
extern __inline__ void igsSync();
extern __inline__ void igsWaitHostBltAck();

void
igsImageTextTE(pDrawable, pGC, xInit, yInit,nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    int w, h;
    int x, y;
    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    unsigned int CommandFlags;
    
    /* Characters are padded to 4 bytes. */
    unsigned int **glyphp;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    /*
     * Only support GXcopy no planemask
     */
    if(pGC->alu != GXcopy || (pGC->planemask & PMSK) != PMSK)
    {
        xf86GCInfoRec.ImageGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    }
    /*
     * Check for non-standard glyphs, glyphs that are too wide.
     */
    if (glyphWidthBytes != 4 || glyphWidth > 32) 
    {
        xf86GCInfoRec.ImageGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    }

    if ((h | glyphWidth) == 0)
	return;

    x = xInit + FONTMAXBOUNDS(pfont, leftSideBearing) + pDrawable->x;
    y = yInit - FONTASCENT(pfont) + pDrawable->y;

    /* Set up registers for text output */
    CommandFlags = igsPxBlt | igsTextOutOpq;
    
    igsSync();
    *igsCopControl = 0;
    
    *igsForeMix = pGC->alu;
    *igsBackMix = pGC->alu;
    *igsBackColor = pGC->bgPixel;
    *igsForeColor = pGC->fgPixel;

    *igsDstMapWidth = vga256InfoRec.displayWidth - 1;  
    *igsPixMapWidth = vga256InfoRec.displayWidth - 1;
    *igsSrc2MapWidth = vga256InfoRec.displayWidth - 1;
    *igsPixMapFormat = vgaBytesPerPixel - 1;

    *igsSrcStartPtr = 0x00;
    *igsSrc2StartPtr = 0x00;
    *igsOpDimension2 = h - 1;

    *igsDstStartPtr = (unsigned long)(y * vga256InfoRec.displayWidth 
				      + x);
    
    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16); 	

    /* Allocate list of pointers to glyph bitmaps. */
    glyphp = (unsigned int **)ALLOCATE_LOCAL(nglyph * sizeof(unsigned int *));

    igsCollectCharacters(glyphp, nglyph, pglyphBase, ppci);

    igsWaitHostBltAck();
    
    /* Output text  */
    igsDrawTextTE(glyphp, nglyph, glyphWidth, h);
    igsSync();
    
    DEALLOCATE_LOCAL(glyphp)
}

void
igsPolyTextTE(pDrawable, pGC, xInit, yInit,nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    int h;
    int x, y;
    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    unsigned int CommandFlags;

    /* Characters are padded to 4 bytes. */
    unsigned int **glyphp;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    /*
     * Only support GXcopy no planemask
     */
    if(pGC->alu != GXcopy || (pGC->planemask & PMSK) != PMSK)
    {
        xf86GCInfoRec.PolyGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    }
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

    /* Set up registers for text output */
    CommandFlags = igsPxBlt | igsTextOutTra;
    
    igsSync();
    *igsCopControl = 0;
    
    *igsForeMix = pGC->alu;
    *igsBackMix = pGC->alu;
    *igsBackColor = pGC->bgPixel;
    *igsForeColor = pGC->fgPixel;

    *igsDstMapWidth = vga256InfoRec.displayWidth - 1;  
    *igsPixMapWidth = vga256InfoRec.displayWidth - 1;
    *igsSrc2MapWidth = vga256InfoRec.displayWidth - 1;
    *igsPixMapFormat = vgaBytesPerPixel - 1;

    *igsSrcStartPtr = 0x00;
    *igsSrc2StartPtr = 0x00;
    *igsOpDimension2 = h - 1;

    *igsDstStartPtr = (unsigned long)(y * vga256InfoRec.displayWidth 
				      + x);
    
    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16); 	

    /* Allocate list of pointers to glyph bitmaps. */
    glyphp = (unsigned int **)ALLOCATE_LOCAL(nglyph * sizeof(unsigned int *));

    igsCollectCharacters(glyphp, nglyph, pglyphBase, ppci);

    igsWaitHostBltAck();
    
    /* Output text  */
    igsDrawTextTE(glyphp, nglyph, glyphWidth, h);
    igsSync();

    DEALLOCATE_LOCAL(glyphp);
}

void
igsImageTextNonTE(pDrawable, pGC, xInit, yInit,
nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    int w, h;
    int x, y;
    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    int i;
    unsigned int CommandFlags;

    NonTEGlyphInfo *glyphinfop;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    if ((h | glyphWidth) == 0)
	return;

    /*
     * Only support GXcopy no planemask
     */
    if(pGC->alu != GXcopy || (pGC->planemask & PMSK) != PMSK)
    {
        xf86GCInfoRec.ImageGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    }
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

    w = igsCollectCharacterInfo(glyphinfop, nglyph, pglyphBase, ppci,
        FONTASCENT(pfont));

   /* Set up registers for text output */
    CommandFlags = igsPxBlt | igsTextOutOpq;
    
    igsSync();
    *igsCopControl = 0;
    
    *igsForeMix = pGC->alu;
    *igsBackMix = pGC->alu;
    *igsBackColor = pGC->bgPixel;
    *igsForeColor = pGC->fgPixel;

    *igsDstMapWidth = vga256InfoRec.displayWidth - 1;  
    *igsPixMapWidth = vga256InfoRec.displayWidth - 1;
    *igsSrc2MapWidth = vga256InfoRec.displayWidth - 1;
    *igsPixMapFormat = vgaBytesPerPixel - 1;

    *igsSrcStartPtr = 0x00;
    *igsSrc2StartPtr = 0x00;
    *igsOpDimension2 = h - 1;

    *igsDstStartPtr = (unsigned long)(y * vga256InfoRec.displayWidth 
				      + x);
    
    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16); 	

    igsWaitHostBltAck();
    
    /* Output text  */
    igsDrawTextNonTE(glyphinfop, nglyph, h);
    igsSync();

    DEALLOCATE_LOCAL(glyphinfop);
}


void
igsPolyTextNonTE(pDrawable, pGC, xInit, yInit,
nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC *pGC;
    int xInit, yInit;
    int nglyph;
    CharInfoPtr *ppci;		       /* array of character info */
    unsigned char *pglyphBase;	       /* start of array of glyphs */
{
    FontPtr pfont = pGC->font;
    int w, h;
    int x, y;

    int glyphWidth;		       /* Character width in pixels. */
    int glyphWidthBytes;	       /* Character width in bytes (padded). */
    int i;
    unsigned int CommandFlags;

    NonTEGlyphInfo *glyphinfop;

    glyphWidth = FONTMAXBOUNDS(pfont, characterWidth);
    glyphWidthBytes = GLYPHWIDTHBYTESPADDED(*ppci);
    h = FONTASCENT(pfont) + FONTDESCENT(pfont);

    /*
     * Only support GXcopy no planemask
     */
    if(pGC->alu != GXcopy || (pGC->planemask & PMSK) != PMSK)
    {
        xf86GCInfoRec.PolyGlyphBltFallBack(
            pDrawable, pGC, xInit, yInit, nglyph, ppci, pglyphBase);
	return;
    }
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

    w = igsCollectCharacterInfo(glyphinfop, nglyph, pglyphBase, ppci,
        FONTASCENT(pfont));
    
    /* Set up registers for text output */
    CommandFlags = igsPxBlt | igsTextOutTra;
    
    igsSync();
    *igsCopControl = 0;
    
    *igsForeMix = pGC->alu;
    *igsBackMix = pGC->alu;
    *igsBackColor = pGC->bgPixel;
    *igsForeColor = pGC->fgPixel;

    *igsDstMapWidth = vga256InfoRec.displayWidth - 1;  
    *igsPixMapWidth = vga256InfoRec.displayWidth - 1;
    *igsSrc2MapWidth = vga256InfoRec.displayWidth - 1;
    *igsPixMapFormat = vgaBytesPerPixel - 1;

    *igsSrcStartPtr = 0x00;
    *igsSrc2StartPtr = 0x00;
    *igsOpDimension2 = h - 1;

    *igsDstStartPtr = (unsigned long)(y * vga256InfoRec.displayWidth 
				      + x);
    
    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16); 	

    igsWaitHostBltAck();
    
    /* Output text  */
    igsDrawTextNonTE(glyphinfop, nglyph, h);
    igsSync();

    DEALLOCATE_LOCAL(glyphinfop);
}

/*
** Text drawing routines
*/

void (*igs_glyphwidth_fn[32])(
#if NeedNestedPrototypes
    unsigned int **glyphp, int line, int nglyph
#endif
) = {
    NULL, NULL, NULL, NULL, NULL,
    igsDrawTextScanlineWidth6, NULL, igsDrawTextScanlineWidth8,
    NULL, igsDrawTextScanlineWidth10, NULL, igsDrawTextScanlineWidth12,
    NULL, igsDrawTextScanlineWidth14, NULL, igsDrawTextScanlineWidth16,
    NULL, igsDrawTextScanlineWidth18, NULL, NULL,
    NULL, NULL, NULL, igsDrawTextScanlineWidth24,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
};

/*
 * Bit output macros
 */
#ifdef __arm32__ 

#define IGS_WRITE_WORD(bits)  *(unsigned int *)vgaLinearBase =        \
            IGS_TXT_CHAR | IGS_TXT_WIDTH |                            \
	    byte_reversed[(bits & 0xFF00) >> 8] |                     \
	    (byte_reversed[(bits & 0xFF)] << 8);                      \
	                      *(unsigned int *)vgaLinearBase =        \
            IGS_TXT_CHAR | IGS_TXT_WIDTH |                            \
	    (byte_reversed[(bits & 0xFF000000) >> 24] ) |             \
	    (byte_reversed[(bits & 0xFF0000) >> 16] << 8)

#endif
/*
** Sends text to the coprocessor. The expected format is:
** | 31 --- 24 | 23 --- 16 | 15 ---------- 0 |
** | cmd field |  length   |       data      |
**
** where cmd field specifies TextForward, TextBackward, New Character,
** End of Line, or End of Data.
**
** Rather than send a character per word, I've modified the code from
** xf86expblt.c to send 16 bit sections of the scanline and pretend each
** one is a character with width = 16.
*/
void igsDrawTextTE(glyphp, nglyph, glyphwidth, glyphheight)
    unsigned int **glyphp;
    int nglyph;
    int glyphwidth;
{
    unsigned int lowBits, highBits;
    int shift, i;
    int line;
    void (*scanline_func)(
#if NeedNestedPrototypes
    unsigned int **glyphp, int line, int nglyph
#endif
) = 0;
    int glyphsleft = nglyph;
    unsigned int **glyphp_orig = glyphp;
    
    
    if (glyphwidth_stretchsize[glyphwidth - 1] > 0
	&& glyphwidth_stretchsize[glyphwidth - 1] <= nglyph) 
    {
	scanline_func = igs_glyphwidth_fn[glyphwidth - 1];
    }
        
    for(line = 0; line < glyphheight; line++)
    {
	if (scanline_func)
	{
	    /* 
	     * Draw the first stretches of characters using a function
	     * customized for the glyph width.
	     */
	    (*scanline_func)(glyphp, line, nglyph);
	    /* Now do the rest. */
	    glyphsleft = nglyph & (glyphwidth_stretchsize[glyphwidth - 1] - 1);
	    glyphp += nglyph - glyphsleft;
	}

	lowBits = highBits = 0;
	shift = 0;
	i = 0;

	while (i < glyphsleft) 
	{
	    lowBits |= glyphp[i][line] << shift;
	    if (shift > 0)
		highBits |= glyphp[i][line] >> (32 - shift);
	    
	    shift += glyphwidth;
		
	    if (shift >= 32) 
	    {
		/* Write 2 x 16 bit halfwords as 2 chars. */
		IGS_WRITE_WORD(lowBits);
		
		shift -= 32;
		lowBits = highBits;
		highBits = 0;
	    }
	    i++;
	}
	if (shift > 0) 
	{
#ifdef __arm32__
	    *(unsigned int *)vgaLinearBase = IGS_TXT_CHAR | 
		(shift >16 ? IGS_TXT_WIDTH : ((shift-1) << 16)) |          
		    byte_reversed[(lowBits & 0xFF00) >> 8] |   
			(byte_reversed[(lowBits & 0xFF)] << 8);
	    if(shift > 16)
	    {
		*(unsigned int *)vgaLinearBase = IGS_TXT_CHAR | 
		    ((shift - 17) << 16) |   
			(byte_reversed[(lowBits & 0xFF000000) >> 24] ) |
			    (byte_reversed[(lowBits & 0xFF0000) >> 16] << 8);
	    }
#endif
	}
	*(unsigned int *)vgaLinearBase = IGS_TXT_EOL;
	glyphp = glyphp_orig;
    }
    *(unsigned int *)vgaLinearBase = IGS_TXT_EOD;
}

/*
 * This function is used for Imagetext/Polytext with Non-TE font.
 */
void igsDrawTextNonTE(glyphinfop, nglyph, glyphheight)
    NonTEGlyphInfo *glyphinfop;
    int nglyph;
    int glyphheight;
{
    unsigned int lowBits, highBits;
    int shift, i;
    int line;
    
        
    for (line = 0; line < glyphheight; line ++)
    {
	lowBits = highBits = 0;
	shift = 0;
	i = 0;
	while (i < nglyph) 
	{
	    /* Check whether the current glyph has bits for this scanline. */
	    if (line >= glyphinfop[i].firstline
		&& line <= glyphinfop[i].lastline) 
	    {
		lowBits |=  glyphinfop[i].bitsp[line] << shift; 
		if (shift > 0) 
		    highBits |= glyphinfop[i].bitsp[line] >> (32 - shift);
	    }
	    shift += glyphinfop[i].width;
	    if (shift >= 32) 
	    {
		/* Write 2 16-bit halfwords. */
		IGS_WRITE_WORD(lowBits);
		shift -= 32;
		lowBits = highBits;
		highBits = 0;
	    }
	    i++;
	}
	if (shift > 0) 
	{
#ifdef __arm32__
	    *(unsigned int *)vgaLinearBase = IGS_TXT_CHAR | 
		(shift >16 ? IGS_TXT_WIDTH : ((shift-1) << 16)) |          
		    byte_reversed[(lowBits & 0xFF00) >> 8] |   
			(byte_reversed[(lowBits & 0xFF)] << 8);
	    if(shift > 16)
	    {
		*(unsigned int *)vgaLinearBase = IGS_TXT_CHAR | 
		    ((shift - 17) << 16) |   
			(byte_reversed[(lowBits & 0xFF000000) >> 24] ) |
			    (byte_reversed[(lowBits & 0xFF0000) >> 16] << 8);
	    }
#endif
	}
	*(unsigned int *)vgaLinearBase = IGS_TXT_EOL;
    }
    *(unsigned int *)vgaLinearBase = IGS_TXT_EOD;
}



/*
 * The functions below write stretches of glyphs for special font widths.
 * Actually, these functions could also be used for 24bpp color expansion
 * in 8bpp mode.
 */

static void igsDrawTextScanlineWidth6(glyphp, line, nglyph)
    unsigned int **glyphp;
    int line;
    int nglyph;
{
    while (nglyph >= 16) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 6;
        bits |= glyphp[2][line] << 12;
        bits |= glyphp[3][line] << 18;
        bits |= glyphp[4][line] << 24;
        bits |= glyphp[5][line] << 30;
	IGS_WRITE_WORD(bits);
	
        bits = glyphp[5][line] >> 2;
        bits |= glyphp[6][line] << 4;
        bits |= glyphp[7][line] << 10;
        bits |= glyphp[8][line] << 16;
        bits |= glyphp[9][line] << 22;
        bits |= glyphp[10][line] << 28;
	IGS_WRITE_WORD(bits);

        bits = glyphp[10][line] >> 4;
        bits |= glyphp[11][line] << 2;
        bits |= glyphp[12][line] << 8;
        bits |= glyphp[13][line] << 14;
        bits |= glyphp[14][line] << 20;
        bits |= glyphp[15][line] << 26;
	IGS_WRITE_WORD(bits);

        nglyph -= 16;
        glyphp += 16;
    }
}

static void igsDrawTextScanlineWidth8(glyphp, line, nglyph)
    unsigned int **glyphp;
    int line;
    int nglyph;
{
    while (nglyph >= 8) {
        unsigned int bits;
        bits = byte_reversed[glyphp[0][line]] << 8;
        bits |= byte_reversed[glyphp[1][line]];
        bits |= byte_reversed[glyphp[2][line]] << 24;
        bits |= byte_reversed[glyphp[3][line]] << 16;
#ifdef __arm32__
	*(unsigned int *)vgaLinearBase = IGS_TXT_CHAR | IGS_TXT_WIDTH | 
	    (bits & 0xFFFF); 
	*(unsigned int *)vgaLinearBase = IGS_TXT_CHAR | IGS_TXT_WIDTH | 
	    ((bits & 0xFFFF0000) >> 16);
#endif
	bits = byte_reversed[glyphp[4][line]] << 8;
        bits |= byte_reversed[glyphp[5][line]];
        bits |= byte_reversed[glyphp[6][line]] << 24;
        bits |= byte_reversed[glyphp[7][line]] << 16;
#ifdef __arm32__
	*(unsigned int *)vgaLinearBase = IGS_TXT_CHAR | IGS_TXT_WIDTH | 
	    (bits & 0xFFFF); 
	*(unsigned int *)vgaLinearBase = IGS_TXT_CHAR | IGS_TXT_WIDTH | 
	    ((bits & 0xFFFF0000) >> 16);
#endif
        nglyph -= 8;	    
        glyphp += 8;
    }
}

static void igsDrawTextScanlineWidth10(glyphp, line, nglyph)
    unsigned int **glyphp; 
    int line;
    int nglyph;
{
    while (nglyph >= 16) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 10;
        bits |= glyphp[2][line] << 20;
        bits |= glyphp[3][line] << 30;
	IGS_WRITE_WORD(bits);
	
        bits = glyphp[3][line] >> 2;
        bits |= glyphp[4][line] << 8;
        bits |= glyphp[5][line] << 18;
        bits |= glyphp[6][line] << 28;
	IGS_WRITE_WORD(bits);

        bits = glyphp[6][line] >> 4;
        bits |= glyphp[7][line] << 6;
        bits |= glyphp[8][line] << 16;
        bits |= glyphp[9][line] << 26;
	IGS_WRITE_WORD(bits);

        bits = glyphp[9][line] >> 6;
        bits |= glyphp[10][line] << 4;
        bits |= glyphp[11][line] << 14;
        bits |= glyphp[12][line] << 24;
	IGS_WRITE_WORD(bits);

        bits = glyphp[12][line] >> 8;
        bits |= glyphp[13][line] << 2;
        bits |= glyphp[14][line] << 12;
        bits |= glyphp[15][line] << 22;
	IGS_WRITE_WORD(bits);

        nglyph -= 16;
        glyphp += 16;
    }
}

static void igsDrawTextScanlineWidth12(glyphp, line, nglyph)
    unsigned int **glyphp; 
    int line;
    int nglyph;
{
    while (nglyph >= 8) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 12;
        bits |= glyphp[2][line] << 24;
	IGS_WRITE_WORD(bits);

        bits = glyphp[2][line] >> 8;
        bits |= glyphp[3][line] << 4;
        bits |= glyphp[4][line] << 16;
        bits |= glyphp[5][line] << 28;
	IGS_WRITE_WORD(bits);

        bits = glyphp[5][line] >> 4;
        bits |= glyphp[6][line] << 8;
        bits |= glyphp[7][line] << 20;
	IGS_WRITE_WORD(bits);

        nglyph -= 8;
        glyphp += 8;
    }
}

static void igsDrawTextScanlineWidth14(glyphp, line, nglyph)
    unsigned int **glyphp; 
    int line;
    int nglyph;
{
    while (nglyph >= 16) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 14;
        bits |= glyphp[2][line] << 28;
	IGS_WRITE_WORD(bits);

        bits = glyphp[2][line] >> 4;
        bits |= glyphp[3][line] << 10;
        bits |= glyphp[4][line] << 24;
	IGS_WRITE_WORD(bits);

        bits = glyphp[4][line] >> 8;
        bits |= glyphp[5][line] << 6;
        bits |= glyphp[6][line] << 20;
	IGS_WRITE_WORD(bits);

        bits = glyphp[6][line] >> 12;
        bits |= glyphp[7][line] << 2;
        bits |= glyphp[8][line] << 16;
        bits |= glyphp[9][line] << 30;
	IGS_WRITE_WORD(bits);

        bits = glyphp[9][line] >> 2;
        bits |= glyphp[10][line] << 12;
        bits |= glyphp[11][line] << 26;
	IGS_WRITE_WORD(bits);

        bits = glyphp[11][line] >> 6;
        bits |= glyphp[12][line] << 8;
        bits |= glyphp[13][line] << 22;
	IGS_WRITE_WORD(bits);

        bits = glyphp[13][line] >> 10;
        bits |= glyphp[14][line] << 4;
        bits |= glyphp[15][line] << 18;
	IGS_WRITE_WORD(bits);

        nglyph -= 16;
        glyphp += 16;
    }
}

static void igsDrawTextScanlineWidth16(glyphp, line, nglyph)
    unsigned int **glyphp;
    int line;
    int nglyph;
{
    while (nglyph >= 8) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 16;
	IGS_WRITE_WORD(bits);

        bits = glyphp[2][line];
        bits |= glyphp[3][line] << 16;
	IGS_WRITE_WORD(bits);

        bits = glyphp[4][line];
        bits |= glyphp[5][line] << 16;
	IGS_WRITE_WORD(bits);

        bits = glyphp[6][line];
        bits |= glyphp[7][line] << 16;
	IGS_WRITE_WORD(bits);

        nglyph -= 8;	    
        glyphp += 8;
    }
}

static void igsDrawTextScanlineWidth18(glyphp, line, nglyph)
    unsigned int **glyphp; 
    int line;
    int nglyph;
{
    while (nglyph >= 16) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 18;
	IGS_WRITE_WORD(bits);

        bits = glyphp[1][line] >> 14;
        bits |= glyphp[2][line] << 4;
        bits |= glyphp[3][line] << 22;
	IGS_WRITE_WORD(bits);

        bits = glyphp[3][line] >> 10;
        bits |= glyphp[4][line] << 8;
        bits |= glyphp[5][line] << 26;
	IGS_WRITE_WORD(bits);

        bits = glyphp[5][line] >> 6;
        bits |= glyphp[6][line] << 12;
        bits |= glyphp[7][line] << 30;
	IGS_WRITE_WORD(bits);

        bits = glyphp[7][line] >> 2;
        bits |= glyphp[8][line] << 16;
	IGS_WRITE_WORD(bits);

        bits = glyphp[8][line] >> 16;
        bits |= glyphp[9][line] << 2;
        bits |= glyphp[10][line] << 20;
	IGS_WRITE_WORD(bits);

        bits = glyphp[10][line] >> 12;
        bits |= glyphp[11][line] << 6;
        bits |= glyphp[12][line] << 24;
	IGS_WRITE_WORD(bits);

        bits = glyphp[12][line] >> 8;
        bits |= glyphp[13][line] << 10;
        bits |= glyphp[14][line] << 28;
	IGS_WRITE_WORD(bits);

        bits = glyphp[14][line] >> 4;
        bits |= glyphp[15][line] << 14;
	IGS_WRITE_WORD(bits);

        nglyph -= 16;
        glyphp += 16;
    }
}

static void igsDrawTextScanlineWidth24(glyphp, line, nglyph)
    unsigned int **glyphp; 
    int line;
    int nglyph;
{
    while (nglyph >= 4) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 24;
	IGS_WRITE_WORD(bits);

        bits = glyphp[1][line] >> 8;
        bits |= glyphp[2][line] << 16;
	IGS_WRITE_WORD(bits);

        bits = glyphp[2][line] >> 16;
        bits |= glyphp[3][line] << 8;
	IGS_WRITE_WORD(bits);

        nglyph -= 4;
        glyphp += 4;
    }
}

/* 
 * The next two functions are reproduced here because they are declared
 * static in xf86text.c
 * This function collects pointers to the character bitmaps.
 */

static void
igsCollectCharacters(glyphp, nglyph, pglyphBase, ppci)
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
 * This function collects glyph info for Non-TE text strings.
 * It returns the total width of the string in pixels.
 */

static int
igsCollectCharacterInfo(glyphinfop, nglyph, pglyphBase, ppci, maxascent)
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


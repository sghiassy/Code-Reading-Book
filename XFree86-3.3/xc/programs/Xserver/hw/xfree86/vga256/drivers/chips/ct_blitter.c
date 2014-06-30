/* $XConsortium: ct_blitter.c /main/2 1996/10/25 10:29:06 kaleb $ */
/*
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  David Bateman <dbateman@ee.uts.edu.au> from pvga1 and cirrus
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_blitter.c,v 3.5 1997/01/18 06:56:21 dawes Exp $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"cfb.h"
#include	"cfbmskbits.h"
#include	"cfbrrop.h"
#include	"cfb8bit.h"
#include	"fastblt.h"
#include	"mergerop.h"
#include        "vgaBank.h"

#include "compiler.h"
#include "xf86.h"
#include "vga.h"
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


/* Ugly hack to get around setting foreground colour with registers
 * in ct_BitBlt.c, or compiling it multiple times */
#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
#define _ctSetFGColorBPP ctHiQVSetFGColorBPP
#else
#define _ctSetFGColorBPP ctMMIOSetFGColorBPP
#endif
#else
#define _ctSetFGColorBPP ctSetFGColorBPP
#endif
int
_ctSetFGColorBPP(fgcolor, mask)
    unsigned long fgcolor;
    unsigned int mask;
{
    switch (vgaBitsPerPixel) {
    case 8:
	ctSETFGCOLOR8(fgcolor);
	mask = 0xFF;
	break;
    case 16:
	ctSETFGCOLOR16(fgcolor);
	mask = 0xFFFF;
	break;
    case 24:
#ifdef CHIPS_HIQV
	ctSETFGCOLOR24(fgcolor);
#else
	{
	    /* The 6554x Blitter can only handle 8/16bpp fills directly,
	     * Though you can do a grey fill, by a little bit of magic
	     * with the 8bpp fill */
	    unsigned char red, green, blue;

	    red = fgcolor & 0xFF;
	    green = (fgcolor >> 8) & 0xFF;
	    blue = (fgcolor >> 16) & 0xFF;
	    if (red ^ green || green ^ blue || blue ^ red) {
		return -1;
	    } else {
		ctSETFGCOLOR8(fgcolor);
	    }
	}
#endif
	mask = 0xFFFFFF;
	break;
    }
    return 0;
}

#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
#define _ctSetBGColorBPP ctHiQVSetBGColorBPP
#else
#define _ctSetBGColorBPP ctMMIOSetBGColorBPP
#endif
#else
#define _ctSetBGColorBPP ctSetBGColorBPP
#endif
int
_ctSetBGColorBPP(bgcolor, mask)
    unsigned long bgcolor;
    unsigned int mask;
{
    switch (vgaBitsPerPixel) {
    case 8:
	ctSETBGCOLOR8(bgcolor);
	mask = 0xFF;
	break;
    case 16:
	ctSETBGCOLOR16(bgcolor);
	mask = 0xFFFF;
	break;
    case 24:
#ifdef CHIPS_HIQV
	ctSETBGCOLOR24(bgcolor);
#else
	{
	    /* The 6554x Blitter can only handle 8/16bpp fills directly,
	     * Though you can do a grey fill, by a little bit of magic
	     * with the 8bpp fill */
	    unsigned char red, green, blue;

	    red = bgcolor & 0xFF;
	    green = (bgcolor >> 8) & 0xFF;
	    blue = (bgcolor >> 16) & 0xFF;
	    if (red ^ green || green ^ blue || blue ^ red) {
		return -1;
	    } else {
		ctSETBGCOLOR8(bgcolor);
	    }
	}
#endif
	mask = 0xFFFFFF;
	break;
    }
    return 0;
}

/*lowlevel function */
#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
#define _ctBitBlt ctHiQVBitBlt
#else
#define _ctBitBlt ctMMIOBitBlt
#endif
#else
#define _ctBitBlt ctBitBlt
#endif
void
_ctBitBlt(psrcBase, pdstBase, widthSrc, widthDst, x, y,
    x1, y1, w, h, xdir, ydir, alu, planemask)
    unsigned char *psrcBase, *pdstBase;		/* start of src and dst bitmaps */
    int widthSrc, widthDst;
    int x, y, x1, y1, w, h;
    int xdir, ydir;
    int alu;
    unsigned long planemask;
{
    unsigned long pSrc, pDst;
    unsigned int op;

#ifdef DEBUG
#ifdef CHIPS_MMIO
    ErrorF("CHIPS:ctMMIOBitBlt(0x%X, 0x%X, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %x, %x)\n",
	psrcBase, pdstBase, widthSrc, widthDst, x, y, x1, y1, w, h,
	xdir, ydir, alu, planemask);
#else
    ErrorF("CHIPS:ctBitBlt(0x%X, 0x%X, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %x, %x)\n",
	psrcBase, pdstBase, widthSrc, widthDst, x, y, x1, y1, w, h,
	xdir, ydir, alu, planemask);
#endif
#endif

    if (alu & 0xF == GXnoop)
	return;


    if (alu & 0x10) {
	/* Source is Background colour */
	op = ctAluConv2[alu & 0xF] | ctPATSOLID | ctPATMONO;
    } else {
	op = ctAluConv[alu & 0xF];
    }
    
    if (alu & 0x40)
	op |= ctBGTRANSPARENT;	       /*BG transparent */
    if (!CHECKSCREEN(psrcBase))
	op |= ctSRCSYSTEM;	       /*source is system memory */

#if 0
    if (xdir == 1 && ydir == 1) {      /* left to right */
	/* top to bottom */
	pSrc = ((y * widthSrc) + x) * vgaBytesPerPixel;
	pDst = ((y1 * widthDst) + x1) * vgaBytesPerPixel;
	op |= (ctTOP2BOTTOM | ctLEFT2RIGHT);
    } else {			       /* right to left */
	/* bottom to top */
	pSrc = (((y + h - 1) * widthSrc) + x + w - 1) * vgaBytesPerPixel;
	pDst = (((y1 + h - 1) * widthDst) + x1 + w - 1) * vgaBytesPerPixel;
	op |= (ctBOTTOM2TOP | ctRIGHT2LEFT);
    }
#else
    if (xdir == 1) {
	op |= ctLEFT2RIGHT;
	pSrc = x;
	pDst = x1;
    } else {
	op |= ctRIGHT2LEFT;
	pSrc = x + w - 1;
	pDst = x1 + w - 1;
    }
    if (ydir == 1) {
	op |= ctTOP2BOTTOM;
	pSrc += y * widthSrc;
	pDst += y1 * widthDst;
    } else {
	op |= ctBOTTOM2TOP;
	pSrc += (y + h - 1) * widthSrc;
	pDst += (y1 + h - 1) * widthSrc;
    }
    pSrc *= vgaBytesPerPixel;
    pDst *= vgaBytesPerPixel;
#endif

    ctBLTWAIT;
    ctSETROP(op);
#ifdef CHIPS_HIQV
    if (alu & 0x10) ctSETMONOCTL(0);	/* Paranoia */
#endif
    ctSETSRCADDR(pSrc);
    ctSETDSTADDR(pDst);
    ctSETPITCH(widthSrc * vgaBytesPerPixel, widthDst * vgaBytesPerPixel);
    ctSETHEIGHTWIDTHGO(h, w * vgaBytesPerPixel);
    ctBLTWAIT;

}

/*
 * This function performs a 8x8 tile fill with the BitBLT engine.
 */

#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
#define _ctcfbBLT8x8PatternFill ctHiQVBLT8x8PatternFill
#else
#define _ctcfbBLT8x8PatternFill ctMMIOBLT8x8PatternFill
#endif
#else
#define _ctcfbBLT8x8PatternFill ctcfbBLT8x8PatternFill
#endif

void
_ctcfbBLT8x8PatternFill(destaddr, w, h, pattern, yrot, patternpitch,
    destpitch)
    unsigned int destaddr;
    int w, h, yrot;
    unsigned char *pattern;
    int patternpitch;
    int destpitch;
{
    int i;
    unsigned int curreg;

#ifdef DEBUG
#ifdef CHIPS_MMIO
    ErrorF("CHIPS:ctMMIOBLT8x8PatterFill(%d, %d, %d, 0x%X, %d, %d)\n",
	destaddr, w, h, pattern, patternpitch, destpitch);
#else
    ErrorF("CHIPS:ctcfbBLT8x8PatterFill(%d, %d, %d, 0x%X, %d, %d)\n",
	destaddr, w, h, pattern, patternpitch, destpitch);
#endif
#endif

    /* The cursor must be hidden during this operation, otherwise 
     * with the depth set to the wrong value it might be corrupted
     */
    if (ctHWCursor){
#ifdef CHIPS_HIQV
	outb(0x3D6, 0xA0);
	curreg = inb(0x3D7);
	outb(0x3D7, curreg & 0xF7);
#else
	ctGETHWCUR(curreg);
	ctPUTHWCUR(curreg & 0xFFF7);
#endif
    }
    if (patternpitch == 8)
	memcpy((unsigned char *)vgaLinearBase + ctBLTPatternAddress,
	    pattern, 64 * vgaBytesPerPixel);
    else
	for (i = 0; i < 8; i++)
	    memcpy((unsigned char *)vgaLinearBase + ctBLTPatternAddress +
		i * 8 * vgaBytesPerPixel, (unsigned char *)pattern +
		patternpitch * i * vgaBytesPerPixel, 8 * vgaBytesPerPixel);

    ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT | ((yrot & 7) << 16));
    ctSETPITCH(8 * vgaBytesPerPixel, destpitch * vgaBytesPerPixel);
    ctSETDSTADDR(destaddr * vgaBytesPerPixel);
    ctSETPATSRCADDR(ctBLTPatternAddress);
    ctSETHEIGHTWIDTHGO(h, w * vgaBytesPerPixel);
    ctBLTWAIT;
    if (ctHWCursor) {
#ifdef CHIPS_HIQV
	outb(0x3D6, 0xA0);
	outb(0x3D7, curreg);
#else
	ctPUTHWCUR(curreg);
#endif
  }
}

/*
 * This function performs a 16x16 tile fill with the BitBLT engine,
 * using two 16x8 pattern fills, vertically interleaved.
 */

#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
#define _ctcfbBLT16x16PatternFill ctHiQVBLT16x16PatternFill
#else
#define _ctcfbBLT16x16PatternFill ctMMIOBLT16x16PatternFill
#endif
#else
#define _ctcfbBLT16x16PatternFill ctcfbBLT16x16PatternFill
#endif

void
_ctcfbBLT16x16PatternFill(destaddr, x, w, h, pattern, yrot, patternpitch,
    destpitch)
    unsigned int destaddr;
    int x, w, h, yrot;
    unsigned char *pattern;
    int patternpitch;
    int destpitch;
{
    int saved_destaddr, k, i, wleft;
    unsigned char bltreg;
    unsigned int curreg;

#ifdef DEBUG
#ifdef CHIPS_MMIO
    ErrorF("CHIPS:ctMMIOBLT16x16PatterFill(%d, %d, %d, 0x%X, %d, %d, %d)\n",
	destaddr, w, h, pattern, yrot, patternpitch, destpitch);
#else
    ErrorF("CHIPS:ctcfbBLT16x16PatterFill(%d, %d, %d, 0x%X, %d, %d, %d)\n",
	destaddr, w, h, pattern, yrot, patternpitch, destpitch);
#endif
#endif
    /* The cursor must be hidden during this operation, otherwise 
     * with the depth set to the wrong value it might be corrupted
     */
    if (ctHWCursor) {
#ifdef CHIPS_HIQV
	outb(0x3D6, 0xA0);
	curreg = inb(0x3D7);
	outb(0x3D7, curreg & 0xF7);
#else
	ctGETHWCUR(curreg);
	ctPUTHWCUR(curreg & 0xFFF7);
#endif
    }

    if (vgaBitsPerPixel == 8) {
	/* Two-way interleave */
	ctSETPITCH(16, destpitch * 2);

	/* Use 16bpp pixel width for 16x8 Tile at 8bpp.
	 */
	if (ctisHiQV32) {
	    outb(0x3D6, 0x20);
	    bltreg = inb(0x3D7);
	    outb(0x3D7, bltreg & 0xCF | 0x10);
	} else {
	    outb(0x3D6, 0x40);
	    bltreg = inb(0x3D7);
	    outb(0x3D7, bltreg & 0xFC | 0x02);
	}

	for (k = 0; k < 2; k++) {
	    /* Do the lines for which [index % 2 == k]. */
	    int blitheight;
	    int width_left;
	    int i;

	    blitheight = h / 2;
	    if ((h & 1) > k)
		blitheight++;

	    ctBLTWAIT;
	    for (i = 0; i < 8; i++)
		memcpy((unsigned char *)vgaLinearBase + ctBLTPatternAddress +
		    i * 16, (unsigned char *)pattern + patternpitch *
		    (i * 2 + k + yrot), 16);
	    ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
	    ctSETDSTADDR(destaddr);
	    ctSETPATSRCADDR(ctBLTPatternAddress);
	    ctSETHEIGHTWIDTHGO(blitheight, w);
	    destaddr += destpitch;
	}
	if (ctisHiQV32) {
	    ctBLTWAIT;
	    outb(0x3D6, 0x20);
	    outb(0x3D7, bltreg);
	} else {
	    ctBLTWAIT;
	    outb(0x3D6, 0x40);
	    outb(0x3D7, bltreg);
	}
    } else {			       /* 16bpp Tile */
	/* Two-way interleave */
	ctSETPITCH(8 * vgaBytesPerPixel, destpitch * vgaBytesPerPixel * 2);

	saved_destaddr = destaddr;

	for (k = 0; k < 2; k++) {
	    /* Do the lines for which [index % 2 == k]. */
	    int blitheight;
	    int width_left;
	    int i, halves_identical;

	    blitheight = h / 2;
	    if ((h & 1) > k)
		blitheight++;

	    /*
	     * Create two 8x8 patterns; one for the left half and one for
	     * the right half of the 16-byte wide pattern. We take the
	     * pattern lines for which [index % 2 == k].
	     *
	     * First check for the special case: left half of each 16-byte wide
	     * tile line is identical to right half.
	     */

	    halves_identical = 1;
	    for (i = 0; i < 8; i++) {
		int *p;

		p = (int *)((unsigned char *)pattern + patternpitch *
		    vgaBytesPerPixel * (i * 2 + k));
		if (p[0] != p[4] || p[1] != p[5] || p[2] != p[6] || p[3] !=
		    p[7]) {
		    halves_identical = 0;
		    break;
		}
	    }
	    ctBLTWAIT;

	    for (i = 0; i < 8; i++) {
		/* 
		 * It helps if this gets expanded into unrolled inline
		 * loads/stores. gcc -O2 does this.
		 */
		memcpy((unsigned char *)vgaLinearBase + ctBLTPatternAddress +
		    i * 8 * vgaBytesPerPixel, pattern + patternpitch *
		    vgaBytesPerPixel * (i * 2 + k + yrot), 8 *
		    vgaBytesPerPixel);
		if (!halves_identical)
		    memcpy((unsigned char *)vgaLinearBase + ctBLTPatternAddress +
			(i * 8 + 64) * vgaBytesPerPixel, pattern +
			vgaBytesPerPixel * (patternpitch * (i * 2 + k + yrot)
			    + 8), 8 * vgaBytesPerPixel);
	    }

	    if (halves_identical) {
		/*
		 * Shortcut, no bands necessary.
		 * Fill over the whole horizontal span with one BitBLT.
		 */

		ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
		ctSETDSTADDR(destaddr * vgaBytesPerPixel);
		ctSETPATSRCADDR(ctBLTPatternAddress);
		ctSETHEIGHTWIDTHGO(blitheight, w * vgaBytesPerPixel);
		goto k_finished;
	    }
	    /*
	     * Draw 8x8 patterns from the left to the right, alternating
	     * between the two source patterns. First treat left-hand edge
	     * seperately to ensure pattern alignment.
	     */

	    width_left = w;
	    x &= 0xF;
	    if (x < 8) {
		wleft = 8 - x;
		ctBLTWAIT;
		ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
		ctSETDSTADDR(destaddr * vgaBytesPerPixel);
		ctSETPATSRCADDR(ctBLTPatternAddress);
		ctSETHEIGHTWIDTHGO(blitheight, wleft * vgaBytesPerPixel);
		width_left -= wleft;
		destaddr += wleft;
		if (width_left == 0)
		    goto k_finished;
		ctBLTWAIT;
		ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
		ctSETDSTADDR(destaddr * vgaBytesPerPixel);
		ctSETPATSRCADDR(ctBLTPatternAddress + 64 * vgaBytesPerPixel);
		if (width_left >= 8) {
		    ctSETHEIGHTWIDTHGO(blitheight, 8 * vgaBytesPerPixel);
		    width_left -= 8;
		} else {
		    ctSETHEIGHTWIDTHGO(blitheight, width_left * vgaBytesPerPixel);
		    width_left = 0;
		}
		destaddr += 8;
	    } else {
		wleft = 16 - x;
		ctBLTWAIT;
		ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
		ctSETDSTADDR(destaddr * vgaBytesPerPixel);
		ctSETPATSRCADDR(ctBLTPatternAddress + 64 * vgaBytesPerPixel);
		ctSETHEIGHTWIDTHGO(blitheight, wleft * vgaBytesPerPixel);
		width_left -= wleft;
		destaddr += wleft;
	    }

	    for (;;) {
		if (width_left == 0)
		    break;
		ctBLTWAIT;
		ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
		ctSETDSTADDR(destaddr * vgaBytesPerPixel);
		ctSETPATSRCADDR(ctBLTPatternAddress);
		if (width_left >= 8) {
		    ctSETHEIGHTWIDTHGO(blitheight, 8 * vgaBytesPerPixel);
		    width_left -= 8;
		} else {
		    ctSETHEIGHTWIDTHGO(blitheight, width_left * vgaBytesPerPixel);
		    width_left = 0;
		}
		destaddr += 8;

		if (width_left == 0)
		    break;
		ctBLTWAIT;
		ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
		ctSETDSTADDR(destaddr * vgaBytesPerPixel);
		ctSETPATSRCADDR(ctBLTPatternAddress + 64 * vgaBytesPerPixel);
		if (width_left >= 8) {
		    ctSETHEIGHTWIDTHGO(blitheight, 8 * vgaBytesPerPixel);
		    width_left -= 8;
		} else {
		    ctSETHEIGHTWIDTHGO(blitheight, width_left * vgaBytesPerPixel);
		    width_left = 0;
		}
		destaddr += 8;
	    }

	  k_finished:
	    saved_destaddr += destpitch;
	    destaddr = saved_destaddr;
	}			       /* for (k = 0; k < 2; k++) */

	ctBLTWAIT;
    }
    if (ctHWCursor) {
#ifdef CHIPS_HIQV
	outb(0x3D6, 0xA0);
	outb(0x3D7, curreg);
#else
	ctPUTHWCUR(curreg);
#endif
    }
}

/*
 * This function performs a 32x32 tile fill with the BitBLT engine,
 * using multiple 16x8 pattern fills in vertical bands, and four-way
 * vertically interleaved. For a 256x256 area, it performs 16 * 4 = 64
 * individual BitBLT fills.
 * As an extra optimization, it combines the vertical bands if the
 * left part of a set of tile lines is identical to the right part.
 * This is always possible on chips that support 32x8 tile fill.
 * This function can only be called for 8bpp, because of the restrictions
 * on the interleave become to great for higher depths with a 4 way
 * interleave.
 */

#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
#define _ctcfbBLT32x32PatternFill ctHiQVBLT32x32PatternFill
#else
#define _ctcfbBLT32x32PatternFill ctMMIOBLT32x32PatternFill
#endif
#else
#define _ctcfbBLT32x32PatternFill ctcfbBLT32x32PatternFill
#endif

void
_ctcfbBLT32x32PatternFill(destaddr, x, w, h, pattern, yrot, patternpitch,
    destpitch)
    unsigned int destaddr;
    int x, w, h, yrot;
    unsigned char *pattern;
    int patternpitch;
    int destpitch;
{
    int saved_destaddr, k, i, wleft;
    unsigned char bltreg;
    unsigned int curreg;

#ifdef DEBUG
#ifdef CHIPS_MMIO
    ErrorF("CHIPS:ctMMIOBLT32x32PatterFill(%d, %d, %d, 0x%X, %d, %d, %d)\n",
	destaddr, w, h, pattern, yrot, patternpitch, destpitch);
#else
    ErrorF("CHIPS:ctcfbBLT32x32PatterFill(%d, %d, %d, 0x%X, %d, %d, %d)\n",
	destaddr, w, h, pattern, yrot, patternpitch, destpitch);
#endif
#endif

    /* Use 16bpp pixel width for 16x8 Tile at 8bpp.
     * The cursor must be hidden during this operation, otherwise 
     * with the depth set to the wrong value it might be corrupted
     */
    if (ctHWCursor) {
#ifdef CHIPS_HIQV
	outb(0x3D6, 0xA0);
	curreg = inb(0x3D7);
	outb(0x3D7, curreg & 0xF7);
#else
	ctGETHWCUR(curreg);
	ctPUTHWCUR(curreg & 0xFFF7);
#endif
    }

    ctSETPITCH(8, destpitch * 4);      /* Four-way interleave */

    if (ctisHiQV32) {
	outb(0x3D6, 0x20);
	bltreg = inb(0x3D7);
	outb(0x3D7, bltreg & 0xCF | 0x10);
    } else {
	outb(0x3D6, 0x40);
	bltreg = inb(0x3D7);
	outb(0x3D7, bltreg & 0xFC | 0x02);
    }

    saved_destaddr = destaddr;

    for (k = 0; k < 4; k++) {
	/* Do the lines for which [index % 2 == k]. */
	int blitheight;
	int width_left;
	int i, halves_identical;

	blitheight = h / 4;
	if ((h & 3) > k)
	    blitheight++;

	/*
	 * Create two 16x8 patterns; one for the left half and one for
	 * the right half of the 32-byte wide pattern. We take the
	 * pattern lines for which [index % 4 == k].
	 *
	 * First check for the special case: left half of each 16-byte wide
	 * tile line is identical to right half.
	 */

	halves_identical = 1;
	for (i = 0; i < 8; i++) {
	    int *p;

	    p = (int *)((unsigned char *)pattern + patternpitch * (i * 4 + k));
	    if (p[0] != p[4] || p[1] != p[5] || p[2] != p[6] || p[3] !=
		p[7]) {
		halves_identical = 0;
		break;
	    }
	}
	ctBLTWAIT;

	for (i = 0; i < 8; i++) {
	    /* 
	     * It helps if this gets expanded into unrolled inline
	     * loads/stores. gcc -O2 does this.
	     */
	    memcpy((unsigned char *)vgaLinearBase + ctBLTPatternAddress +
		i * 16, pattern + patternpitch * (i * 4 + k + yrot), 16);
	    if (!halves_identical)
		memcpy((unsigned char *)vgaLinearBase + ctBLTPatternAddress +
		    i * 16 + 128, pattern + patternpitch * (i * 4 + k + yrot)
		    + 16, 16);
	}

	if (halves_identical) {
	    /*
	     * Shortcut, no bands necessary.
	     * Fill over the whole horizontal span with one BitBLT.
	     */

	    ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
	    ctSETDSTADDR(destaddr * vgaBytesPerPixel);
	    ctSETPATSRCADDR(ctBLTPatternAddress);
	    ctSETHEIGHTWIDTHGO(blitheight, w * vgaBytesPerPixel);
	    goto k_finished;
	}
	/*
	 * Draw 16x8 patterns from the left to the right, alternating
	 * between the two source patterns. First treat left-hand edge
	 * seperately to ensure pattern alignment.
	 */

	width_left = w;
	x &= 0x1F;
	if (x < 16) {
	    wleft = 16 - x;
	    ctBLTWAIT;
	    ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
	    ctSETDSTADDR(destaddr * vgaBytesPerPixel);
	    ctSETPATSRCADDR(ctBLTPatternAddress);
	    ctSETHEIGHTWIDTHGO(blitheight, wleft * vgaBytesPerPixel);
	    width_left -= wleft;
	    destaddr += wleft;
	    if (width_left == 0)
		goto k_finished;
	    ctBLTWAIT;
	    ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
	    ctSETDSTADDR(destaddr * vgaBytesPerPixel);
	    ctSETPATSRCADDR(ctBLTPatternAddress + 128 * vgaBytesPerPixel);
	    if (width_left >= 16) {
		ctSETHEIGHTWIDTHGO(blitheight, 16 * vgaBytesPerPixel);
		width_left -= 16;
	    } else {
		ctSETHEIGHTWIDTHGO(blitheight, width_left * vgaBytesPerPixel);
		width_left = 0;
	    }
	    destaddr += 16;
	} else {
	    wleft = 32 - x;
	    ctBLTWAIT;
	    ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
	    ctSETDSTADDR(destaddr * vgaBytesPerPixel);
	    ctSETPATSRCADDR(ctBLTPatternAddress + 128 * vgaBytesPerPixel);
	    ctSETHEIGHTWIDTHGO(blitheight, wleft * vgaBytesPerPixel);
	    width_left -= wleft;
	    destaddr += wleft;
	}

	for (;;) {
	    if (width_left == 0)
		break;
	    ctBLTWAIT;
	    ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
	    ctSETDSTADDR(destaddr * vgaBytesPerPixel);
	    ctSETPATSRCADDR(ctBLTPatternAddress);
	    if (width_left >= 16) {
		ctSETHEIGHTWIDTHGO(blitheight, 16 * vgaBytesPerPixel);
		width_left -= 16;
	    } else {
		ctSETHEIGHTWIDTHGO(blitheight, width_left * vgaBytesPerPixel);
		width_left = 0;
	    }
	    destaddr += 16;

	    if (width_left == 0)
		break;
	    ctBLTWAIT;
	    ctSETROP(ctPATCOPY | ctTOP2BOTTOM | ctLEFT2RIGHT);
	    ctSETDSTADDR(destaddr * vgaBytesPerPixel);
	    ctSETPATSRCADDR(ctBLTPatternAddress + 128 * vgaBytesPerPixel);
	    if (width_left >= 16) {
		ctSETHEIGHTWIDTHGO(blitheight, 16 * vgaBytesPerPixel);
		width_left -= 16;
	    } else {
		ctSETHEIGHTWIDTHGO(blitheight, width_left * vgaBytesPerPixel);
		width_left = 0;
	    }
	    destaddr += 16;
	}

      k_finished:
	saved_destaddr += destpitch;
	destaddr = saved_destaddr;
    }				       /* for (k = 0; k < 4; k++) */

    ctBLTWAIT;
#ifdef CHIPS_HIQV
    outb(0x3D6, 0x20);
    outb(0x3D7, bltreg);
    if (ctHWCursor){
	outb(0x3D6, 0xA0);
	outb(0x3D7, curreg);
    }
#else
    outb(0x3D6, 0x40);
    outb(0x3D7, bltreg);
    if (ctHWCursor) 
        ctPUTHWCUR(curreg);	
#endif
}

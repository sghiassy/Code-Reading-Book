/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_blitter.c,v 3.15 1996/12/23 06:56:33 dawes Exp $ */
/*
 *
 * Copyright 1994 by H. Hanemaayer, Utrecht, The Netherlands
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of H. Hanemaayer not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  H. Hanemaayer makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * H. HANEMAAYER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL H. HANEMAAYER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  H. Hanemaayer, <hhanemaa@cs.ruu.nl>
 *
 */
/* $XConsortium: cir_blitter.c /main/7 1996/02/21 18:03:10 kaleb $ */


/*
 * This file contains all low-level functions that use the BitBLT engine
 * on the 5426/5428/5429/5434 (except for those that involve system memory,
 * they are in cir_im.c, and the text functions in cir_teblt8.c).
 */
 
/*
 * This file is compiled twice, once with CIRRUS_MMIO defined.
 */


#include "xf86.h"	/* For vgaBase. */
#include "vga.h"	/* For vgaBase. */
#include "compiler.h"

#include "cir_driver.h"
#ifdef CIRRUS_MMIO
#include "cirBlitMM.h"
#else
#include "cirBlitter.h"
#endif
#include "cir_inline.h"


#ifndef CIRRUS_MMIO	/* Define these only once. */

/* Cirrus raster operations. */

int cirrus_rop[16] = {
	CROP_0,			/* GXclear */
	CROP_AND,		/* GXand */
	CROP_SRC_AND_NOT_DST,	/* GXandReverse */
	CROP_SRC,		/* GXcopy */
	CROP_NOT_SRC_AND_DST,	/* GXandInverted */
	CROP_DST,		/* GXnoop */
	CROP_XOR,		/* GXxor */
	CROP_OR,		/* GXor */
	CROP_NOR,		/* GXnor */
	CROP_XNOR,		/* GXequiv */
	CROP_NOT_DST,		/* GXinvert */
	CROP_SRC_OR_NOT_DST,	/* GXorReverse */
	CROP_NOT_SRC,		/* GXcopyInverted */
	CROP_NOT_SRC_OR_DST,	/* GXorInverted */
	CROP_NAND,		/* GXnand */
	CROP_1			/* GXset */
};

/*
 * Some of the BLT functions defined in this file support exiting without
 * waiting for the BLT to finish. The calling function must set
 * cirrusDoBackgroundBLT, call the BLT functions, and when the
 * operations are done check whether a BLT is still in
 * progress (cirrusBLTisBusy) and if so call WAITUNTILFINISHED().
 * If the function does any other video memory access or BLT functions
 * not defined in this file, it must make sure no BLT is in progress
 * before drawing.
 *
 */

int cirrusBLTisBusy = FALSE;
int cirrusDoBackgroundBLT = FALSE;

int cirrusBLTPatternAddress;	/* Address for BitBLT filling pattern. */


/*
 * CirrusInvalidateShadowVariables
 * Invalidate values of shadow variables for I/O registers.
 * This helps very slightly on a VLB system. May be unoptimal on the fast
 * host bus interface the 543x (but it can use MMIO).
 * Should be a win for ISA bus.
 */

int cirrusWriteModeShadow,
    cirrusPixelMaskShadow,
    cirrusModeExtensionsShadow,
    cirrusBltSrcAddrShadow,
    cirrusBltDestPitchShadow,
    cirrusBltSrcPitchShadow,
    cirrusBltHeightShadow,
    cirrusBltModeShadow,
    cirrusBltRopShadow;
unsigned int
    cirrusForegroundColorShadow,
    cirrusBackgroundColorShadow;

void CirrusInvalidateShadowVariables() {
    cirrusWriteModeShadow = -1;
    cirrusPixelMaskShadow = -1;
    cirrusModeExtensionsShadow = -1;
    cirrusBltSrcAddrShadow = -1;
    cirrusBltDestPitchShadow = -1;
    cirrusBltSrcPitchShadow = -1;
    cirrusBltHeightShadow = -1;
    cirrusBltModeShadow = -1;
    cirrusBltRopShadow = -1;
    /* For the lower byte of the 32-bit color registers, there is no safe
     * invalid value. We just set them to a specific value (making sure
     * we don't write to non-existant color registers).
     */ 
    cirrusBackgroundColorShadow = 0xffffffff;	/* Defeat the macros. */
    cirrusForegroundColorShadow = 0xffffffff;
    if (cirrusChip >= CLGD5422 && cirrusChip <= CLGD5430) {
	SETBACKGROUNDCOLOR16(0x0000);
	SETFOREGROUNDCOLOR16(0x0000);
    }
    else
    if (cirrusChip == CLGD5434) {
    	SETBACKGROUNDCOLOR32(0x00000000);
    	SETFOREGROUNDCOLOR32(0x00000000);
    }
    else {
    	SETBACKGROUNDCOLOR(0x00);
    	SETFOREGROUNDCOLOR(0x00);
    }
 }

#endif	/* !defined(CIRRUS_MMIO) */


/*
 * 8x8 pattern fill with color expanded pattern.
 *
 * patternword1 contains the first 32 pixels (first pixel in LSByte, MSBit),
 * patternword2 the second. 
 */

#ifdef CIRRUS_MMIO
#define _CirrusBLTColorExpand8x8PatternFill CirrusMMIOBLTColorExpand8x8PatternFill
#else
#define _CirrusBLTColorExpand8x8PatternFill CirrusBLTColorExpand8x8PatternFill
#endif
 
void _CirrusBLTColorExpand8x8PatternFill(dstAddr, fgcolor, bgcolor,
fillWidth, fillHeight, dstPitch, rop, patternword1, patternword2)
     unsigned int dstAddr;
     int fgcolor, bgcolor;
     int fillHeight, fillWidth, dstPitch;
     int rop;
     unsigned long patternword1, patternword2;
{
  unsigned int srcAddr;
  int i;
  pointer pDst;

  if (fillHeight > 1024) {
      /* Split into two for 542x/5430/4. */
      _CirrusBLTColorExpand8x8PatternFill(dstAddr, fgcolor, bgcolor,
          fillWidth, 1024, dstPitch, rop, patternword1, patternword2);
      _CirrusBLTColorExpand8x8PatternFill(dstAddr + dstPitch * 1024,
          fgcolor, bgcolor, fillWidth, fillHeight - 1024, dstPitch, rop,
          patternword1, patternword2);
      return;
  }

  if (cirrusBLTisBusy)
    WAITUNTILFINISHED();

  /* NOTE:
   * I tried System memory source pattern fill, it's not explicitly
   * disallowed (nor documented) in the databook, but it doesn't work. 
   */

  /* Pattern fill with color expanded video memory source. */

  /* Write 8 byte monochrome pattern. */
  /* Because of 16K bank granularity and 32K window, we don't have to */
  /* check for bank boundaries. */
  srcAddr = cirrusBLTPatternAddress;
  CIRRUSSETWRITE(srcAddr);
#if defined(PC98_WAB) || defined(PC98_WABEP)
  *(unsigned long *)(CIRRUSREADBASE() + srcAddr) = patternword1;
  *(unsigned long *)(CIRRUSREADBASE() + srcAddr + 4) = patternword2;
#else
  *(unsigned long *)(CIRRUSWRITEBASE() + srcAddr) = patternword1;
  *(unsigned long *)(CIRRUSWRITEBASE() + srcAddr + 4) = patternword2;
#endif

  SETDESTADDR(dstAddr);
  SETSRCADDR(cirrusBLTPatternAddress);
  SETDESTPITCH(dstPitch);
  SETWIDTH(fillWidth);
  SETHEIGHT(fillHeight);

  /* 8x8 Pattern Copy, screen to screen blt, forwards, color expand. */
  SETBLTMODE(PATTERNCOPY | COLOREXPAND);
  SETROP(rop);
  SETFOREGROUNDCOLOR(fgcolor);
  SETBACKGROUNDCOLOR(bgcolor);

  /* Do it. */
  STARTBLT();

  if (cirrusDoBackgroundBLT)
    cirrusBLTisBusy = TRUE;
  else {
    WAITUNTILFINISHED();
    SETFOREGROUNDCOLOR(0);
  }

  /*
   * Serious problem: leaving the foreground color register at non-zero is
   * not a good idea for normal framebuffer access (Set/Reset enable).
   * Because of this, a calling function that uses background BLTs must
   * set it to zero when it is finished.
   */

#ifdef CIRRUS_MMIO
  cirrusMMIOFlag = TRUE;
#endif
}


/*
 * BitBLT. Direction can be forwards or backwards. Can be modified to
 * support special rops.
 */

#ifdef CIRRUS_MMIO
#define _CirrusBLTBitBlt CirrusMMIOBLTBitBlt
#else
#define _CirrusBLTBitBlt CirrusBLTBitBlt
#endif

void _CirrusBLTBitBlt(dstAddr, srcAddr, dstPitch, srcPitch, w, h, dir)
     unsigned int dstAddr, srcAddr;
     int dstPitch, srcPitch;
     int w, h;
     int dir;			/* >0, increase adrresses, <0, decrease */
{
  if (h > 1024) {
     /* Split into two. */
     if (dir > 0) {
         _CirrusBLTBitBlt(dstAddr, srcAddr, dstPitch, srcPitch, w, 1024, dir);
         _CirrusBLTBitBlt(dstAddr + dstPitch * 1024, srcAddr + srcPitch * 1024,
             dstPitch, srcPitch, w, h - 1024, dir);
     }
     else {
         _CirrusBLTBitBlt(dstAddr, srcAddr, dstPitch, srcPitch, w, 1024, dir);
         _CirrusBLTBitBlt(dstAddr - dstPitch * 1024, srcAddr - srcPitch * 1024,
             dstPitch, srcPitch, w, h - 1024, dir);
     }
     return;
  }

  if (cirrusBLTisBusy)
    WAITUNTILFINISHED();

  SETSRCADDR(srcAddr);
  SETSRCPITCH(srcPitch);
  SETDESTADDR(dstAddr);
  SETDESTPITCH(dstPitch);
  SETWIDTH(w);
  SETHEIGHT(h);

  if (dir > 0) {
      SETBLTMODE(FORWARDS);
  }
  else {
      SETBLTMODE(BACKWARDS);
  }

  SETROP(CROP_SRC);

  STARTBLT();
  if (cirrusDoBackgroundBLT)
    cirrusBLTisBusy = TRUE;
  else
    WAITUNTILFINISHED();

#ifdef CIRRUS_MMIO
  cirrusMMIOFlag = TRUE;
#endif
}


#if 0	/* Experimental, not used. */

void CirrusBLTColorExpandImageWriteFill(dstAddr, fgcolor, fillWidth,
fillHeight, dstPitch)
     unsigned int dstAddr;
     int fgcolor;
     int fillHeight, fillWidth, dstPitch;
{
  int size, i;

  if (fillHeight > 1024) {
      /* Split into two for 542x/30/34. */
      CirrusBLTColorExpandImageWriteFill(dstAddr, fgcolor, fillWidth, 1024,
          dstPitch);
      CirrusBLTColorExpandImageWriteFill(dstAddr + dstPitch * 1024, fgcolor,
          fillWidth, fillHeight - 1024, dstPitch);
      return;
  }

  if (cirrusBLTisBusy)
    WAITUNTILFINISHED();

  /* System-to-video memory blit with color expansion. */

  SETDESTADDR(dstAddr);
  SETSRCADDR(0);
  SETDESTPITCH(dstPitch);
  SETWIDTH(fillWidth);
  SETHEIGHT(fillHeight);

  SETBLTMODE(SYSTEMSRC | COLOREXPAND);
  SETROP(CROP_SRC);
  SETFOREGROUNDCOLOR(fgcolor);

  /* Do it. */
  STARTBLT();

  /* Calculate number of dwords to transfer. */
  size = ((((fillWidth + 7) / 8) * fillHeight) + 3) / 4;

  CirrusWordTransfer(size, 0xffffffff, CIRRUSBASE());

  WAITUNTILFINISHED();

  SETFOREGROUNDCOLOR(0);

#ifdef CIRRUS_MMIO
  cirrusMMIOFlag = TRUE;
#endif
}

#endif

#ifdef CIRRUS_MMIO
#define _CirrusBLTWaitUntilFinished CirrusMMIOBLTWaitUntilFinished
#else
#define _CirrusBLTWaitUntilFinished CirrusBLTWaitUntilFinished
#endif

void _CirrusBLTWaitUntilFinished() {
  int count, timeout;
#ifndef CIRRUS_MMIO  
  if (cirrusUseMMIO) {
  	/* If we have MMIO, better use it. */
  	CirrusMMIOBLTWaitUntilFinished();
  	return;
  }
#endif
  count = 0;
  timeout = 0;
  cirrusBLTisBusy = FALSE;	/* That will be the case on exit. */
  for (;;) {
    int busy;
    BLTBUSY(busy);
    if (!busy)
    	return;
    count++;
    if (count == 10000000) {
    	ErrorF("Cirrus: BitBLT engine time-out.\n");
    	*(unsigned long *)CIRRUSBASE() = 0;
    	count = 9990000;
    	timeout++;
    	if (timeout == 8) {
    		/* Reset BitBLT engine. */
    		BLTRESET();
    		return;
    	}
    }
  }
}

#ifdef CIRRUS_MMIO
#define _CirrusBLTWaitEmpty CirrusMMIOBLTWaitEmpty
#else
#define _CirrusBLTWaitEmpty CirrusBLTWaitEmpty
#endif

void _CirrusBLTWaitEmpty() {
  int count, timeout;
#ifndef CIRRUS_MMIO  
  if (cirrusUseMMIO) {
  	/* If we have MMIO, better use it. */
  	CirrusMMIOBLTWaitEmpty();
  	return;
  }
#endif
  count = 0;
  timeout = 0;
  for (;;) {
    int empty;
    BLTEMPTY(empty);
    if (empty)
    	return;
    count++;
    if (count == 10000000) {
    	ErrorF("Cirrus: BitBLT engine time-out.\n");
    	*(unsigned long *)CIRRUSBASE() = 0;
    	count = 9990000;
    	timeout++;
    	if (timeout == 8) {
    		/* Reset BitBLT engine. */
    		BLTRESET();
    		return;
    	}
    }
  }
}


/*
 * This function performs a 8x8 tile fill with the BitBLT engine.
 */

#ifdef CIRRUS_MMIO
#define _CirrusBLT8x8PatternFill CirrusMMIOBLT8x8PatternFill
#else
#define _CirrusBLT8x8PatternFill CirrusBLT8x8PatternFill
#endif

void _CirrusBLT8x8PatternFill(destaddr, w, h, pattern, patternpitch,
destpitch, rop)
    unsigned int destaddr;
    int w, h;
    unsigned char *pattern;
    int patternpitch;
    int destpitch, rop;
{
    unsigned char *base;
    int srcaddr, busy, i;

    if (h > 1024) {
        /* Split into two for 542x/30/34 */
        _CirrusBLT8x8PatternFill(destaddr, w, 1024, pattern, patternpitch,
            destpitch, rop);
        /* Vertical alignment is correct since 1024 is a multiple of 8. */
        _CirrusBLT8x8PatternFill(destaddr + destpitch * 1024, w, h - 1024,
            pattern, patternpitch, destpitch, rop);
        return;
    }

    srcaddr = cirrusBLTPatternAddress;
    CIRRUSSETWRITE(srcaddr);
#if defined(PC98_WAB) || defined(PC98_WABEP)
    base = CIRRUSREADBASE();
#else
    base = CIRRUSWRITEBASE();
#endif

    if (patternpitch == 8)
	memcpy(base + srcaddr, pattern, 64);
    else
	for (i = 0; i < 8; i++)
            memcpy(base + srcaddr + i * 8, (unsigned char *)pattern +
                patternpitch * i, 8);

    /* Set up the BitBLT parameters. */
    SETROP(rop);
    SETSRCADDR(cirrusBLTPatternAddress);
    SETDESTPITCH(destpitch);
    SETBLTMODE(PATTERNCOPY);
    SETDESTADDR(destaddr);
    SETWIDTH(w);
    SETHEIGHT(h);
    STARTBLT();

    do { BLTBUSY(busy); } while (busy);
    SETFOREGROUNDCOLOR(0);		/* Restore VGA Set/Reset register. */
}


/*
 * This function performs a 16x16 tile fill with the BitBLT engine,
 * using two 16x8 pattern fills, vertically interleaved.
 */

#ifdef CIRRUS_MMIO
#define _CirrusBLT16x16PatternFill CirrusMMIOBLT16x16PatternFill
#else
#define _CirrusBLT16x16PatternFill CirrusBLT16x16PatternFill
#endif

void _CirrusBLT16x16PatternFill(destaddr, w, h, pattern, patternpitch,
destpitch, rop)
    unsigned int destaddr;
    int w, h;
    unsigned char *pattern;
    int patternpitch;
    int destpitch, rop;
{
    unsigned char *base;
    int srcaddr, busy, k, i;

    if (h > 1024) {
        /* Split into two for 542x/30/34. */
        _CirrusBLT16x16PatternFill(destaddr, w, 1024, pattern, patternpitch,
            destpitch, rop);
        /* Vertical alignment is correct since 1024 is a multiple of 16. */
        _CirrusBLT16x16PatternFill(destaddr + destpitch * 1024, w, h - 1024,
            pattern, patternpitch, destpitch, rop);
        return;
    }

    srcaddr = cirrusBLTPatternAddress;
    CIRRUSSETWRITE(srcaddr);
#if defined(PC98_WAB) || defined(PC98_WABEP)
    base = CIRRUSREADBASE();
#else
    base = CIRRUSWRITEBASE();
#endif

    /* Set up the invariant BitBLT parameters. */
    SETROP(rop);
    SETDESTPITCH(destpitch * 2);	/* Two-way interleaving. */
    SETBLTMODE(PATTERNCOPY | PIXELWIDTH16);

    for (k = 0; k < 2; k++) {
        /* Do the lines for which [index % 2 == k]. */
        int blitheight;
        int width_left;
        int i;

        blitheight = h / 2;
        if ((h & 1) > k)
  	    blitheight++;

	do { BLTBUSY(busy); } while (busy);
        for (i = 0; i < 8; i++)
            memcpy(base + srcaddr + i * 16, (unsigned char *)pattern +
               patternpitch * (i * 2 + k), 16);
        SETDESTADDR(destaddr);
        SETSRCADDR(cirrusBLTPatternAddress);
        SETWIDTH(w);
        SETHEIGHT(blitheight);
	STARTBLT();

	destaddr += destpitch;
    }

    do { BLTBUSY(busy); } while (busy);
    SETFOREGROUNDCOLOR(0);		/* Restore VGA Set/Reset register. */
}


/*
 * This function performs a 32x32 tile fill with the BitBLT engine,
 * using multiple 16x8 pattern fills in vertical bands, and four-way
 * vertically interleaved. For a 256x256 area, it performs 16 * 4 = 64
 * individual BitBLT fills.
 * As an extra optimization, it combines the vertical bands if the
 * left part of a set of tile lines is identical to the right part.
 * This is always possible on chips that support 32x8 tile fill.
 */

#ifdef CIRRUS_MMIO
#define _CirrusBLT32x32PatternFill CirrusMMIOBLT32x32PatternFill
#else
#define _CirrusBLT32x32PatternFill CirrusBLT32x32PatternFill
#endif

void _CirrusBLT32x32PatternFill(destaddr, w, h, pattern, patternpitch,
destpitch, rop)
    unsigned int destaddr;
    int w, h;
    unsigned char *pattern;
    int patternpitch;
    int destpitch, rop;
{
    unsigned char *base;
    int srcaddr, saved_destaddr, x, busy, k, chip_supports_32byte_fill;

    if (h > 1024) {
        /* Split into two for 542x/30/34. */
        _CirrusBLT32x32PatternFill(destaddr, w, 1024, pattern, patternpitch,
            destpitch, rop);
        /* Vertical alignment is correct since 1024 is a multiple of 16. */
        _CirrusBLT32x32PatternFill(destaddr + destpitch * 1024, w, h - 1024,
            pattern, patternpitch, destpitch, rop);
        return;
    }

    chip_supports_32byte_fill = 0;

    /*
     * We need at most two 16 * 32 fill areas at the same time, so the
     * preallocated 256 byte pattern space at cirrusBLTPatternAddress is
     * sufficient.
     */
    srcaddr = cirrusBLTPatternAddress;
    CIRRUSSETWRITE(srcaddr);
#if defined(PC98_WAB) || defined(PC98_WABEP)
    base = CIRRUSREADBASE();
#else
    base = CIRRUSWRITEBASE();
#endif

    /* Set up the invariant BitBLT parameters. */
    SETROP(rop);
    SETDESTPITCH(destpitch * 4);	/* Four-way interleaving. */
    if (chip_supports_32byte_fill)
    	SETBLTMODE(PATTERNCOPY | PIXELWIDTH32)
    else
        SETBLTMODE(PATTERNCOPY | PIXELWIDTH16);

    saved_destaddr = destaddr;

    for (k = 0; k < 4; k++) {
        /* Do the lines for which [index % 4 == k]. */
        int blitheight;
        int width_left;
        int i, halves_identical;

        blitheight = h / 4;
        if ((h & 3) > k)
  	    blitheight++;

	/*
	 * On chips for which 32-byte wide pattern fill is not broken,
	 * this code can be used.
	 */
	if (chip_supports_32byte_fill) {
	    do { BLTBUSY(busy); } while (busy);
            for (i = 0; i < 8; i++)
                memcpy(base + srcaddr + i * 32, (unsigned char *)pattern +
                    patternpitch * (i * 4 + k), 32);
            SETDESTADDR(destaddr);
            SETSRCADDR(cirrusBLTPatternAddress);
            SETWIDTH(w);
            SETHEIGHT(blitheight);
	    STARTBLT();
	    goto k_finished;
	}

        /*
         * Create two 16x8 patterns; one for the left half and one for
         * the right half of the 32-byte wide pattern. We take the
         * pattern lines for which [index % 4 == k].
         */

	/*
	 * First check for the special case: left half of each 32-byte wide
	 * tile line is identical to right half.
	 */
        halves_identical = 1;
        for (i = 0; i < 8; i++) {
            int *p;
            p = (int *)((unsigned char *)pattern + patternpitch * (i * 4 + k));
            if (p[0] != p[4] || p[1] != p[5] || p[2] != p[6] || p[3] != p[7]) {
	        halves_identical = 0;
	        break;
	    }
	}

        do { BLTBUSY(busy); } while (busy);

        for (i = 0; i < 8; i++) {
            /* 
             * It helps if this gets expanded into unrolled inline
             * loads/stores. gcc -O2 does this.
             */
            memcpy(base + srcaddr + i * 16,
	        pattern + patternpitch * (i * 4 + k), 16);
	    if (!halves_identical)
                memcpy(base + srcaddr + i * 16 + 128,
                    pattern + patternpitch * (i * 4 + k) + 16, 16);
	}

	if (halves_identical) {
	    /*
	     * Shortcut, no bands necessary.
	     * Fill over the whole horizontal span with one BitBLT.
	     */
            SETDESTADDR(destaddr);
            SETSRCADDR(cirrusBLTPatternAddress);
            SETWIDTH(w);
            SETHEIGHT(blitheight);
	    STARTBLT();
	    goto k_finished;
	}

	/*
	 * Draw 16x8 patterns from the left to the right, alternating between
	 * the two source patterns.
	 */
	width_left = w;
	for (;;) {
	    if (width_left == 0)
	    	break;
	    do { BLTBUSY(busy); } while (busy);
            SETDESTADDR(destaddr);
            SETSRCADDR(cirrusBLTPatternAddress);
            if (width_left >= 16) {
                SETWIDTH(16);
                width_left -= 16;
            }
            else {
                SETWIDTH(width_left);
                width_left = 0;
            }
            SETHEIGHT(blitheight);
	    STARTBLT();
	    destaddr += 16;

	    if (width_left == 0)
	    	break;
	    do { BLTBUSY(busy); } while (busy);
            SETDESTADDR(destaddr);
            SETSRCADDR(cirrusBLTPatternAddress + 128);
            if (width_left >= 16) {
                SETWIDTH(16);
                width_left -= 16;
            }
            else {
                SETWIDTH(width_left);
                width_left = 0;
            }
/*	    SETHEIGHT(blitheight); */
	    STARTBLT();
	    destaddr += 16;
	}

k_finished:
	saved_destaddr += destpitch;
	destaddr = saved_destaddr;
    } /* for (k = 0; k < 4; k++) */

    do { BLTBUSY(busy); } while (busy);
    SETFOREGROUNDCOLOR(0);		/* Restore VGA Set/Reset register. */
}

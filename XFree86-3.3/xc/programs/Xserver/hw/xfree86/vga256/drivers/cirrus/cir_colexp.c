/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_colexp.c,v 3.13 1996/12/23 06:56:38 dawes Exp $ */
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
/* $XConsortium: cir_colexp.c /main/8 1996/10/25 10:31:28 kaleb $ */

/*
 * This file contains the low level accelerated functions that use color
 * expansion/extended write modes, which are supported on all chipsets.
 * Most functions divide the area into banking regions, taking advantage
 * of the 16K bank granularity, and have efficient inner loops without
 * bank checks. Tile and stipple fill draw in scanline interleaved order.
 *
 * Operations performed with these functions:
 * - Solid fill
 * - Transparent/Opaque 32 bits-wide stipple fill
 * - Multiple-of-8 wide tile fill
 * - Special case BitBlt (scrolling) for the 5420/2/4
 * - Small size plain framebuffer fill/BitBlt.
 *
 */
 

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "cfb.h"
#include "cfbmskbits.h"
#include "cfbrrop.h"
#include "mergerop.h"
#include "xf86.h"
#include "vgaBank.h"
#include "vga.h"	/* For vgaInfoRec. */
#include "xf86_HWlib.h"

#ifndef __GNUC__
#undef __volatile__
#define __volatile__ volatile
#endif

#include "compiler.h"


extern pointer vgaBase;

#include "cir_driver.h"
#include "cir_inline.h"
#include "cir_span.h"


/* Bit masks for left edge (indexed with first left-to-right bit number). */

static unsigned char leftbitmask[8] = {
	0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01
};

/* Bit masks for right edge (indexed with number of pixels left). */

static unsigned char rightbitmask[9] = {
	0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
};


/*
 * This is the special function for small widths (< 32).
 * Just uses the banked plain framebuffer.
 * It is somewhat faster than the cfb.banked equivalents (it takes
 * advantage of the 16K bank granularity).
 *
 * Currently not used because of bug (see xgc time percentage scrollbar).
 */

#ifdef __STDC__
void Cirrus32bitFillSmall( int x, int y, int w, int h,
unsigned long *bits_in, int sh, int sox, int soy, int bg, int fg,
int destpitch )
#else
void Cirrus32bitFillSmall( x, y, w, h, bits_in, sh, sox, soy, bg, fg,
destpitch )
	int x, y, w, h;
	unsigned long *bits_in;
	int sh, sox, soy, bg, fg, destpitch;
#endif
{
	int j;
	int destaddr;
	unsigned char *destp;
	int syindex;
	int bank;
	unsigned char *base;	/* Video window base address. */
	unsigned char color[2];

#if defined(PC98_WAB) || defined(PC98_WABEP)
	base = CIRRUSREADBASE();	/* Write window. */
#else
	base = CIRRUSWRITEBASE();	/* Write window. */
#endif
	destaddr = y * destpitch + x;

#if defined(PC98_WAB) || defined(PC98_WABEP)
	CIRRUSSETWRITEB(destaddr, bank); /* It's OK */
#else
	CIRRUSSETWRITEB(destaddr, bank);
#endif

	color[0] = bg;
	color[1] = fg;

	syindex = (y - soy) % sh;	/* y index into source bitmap. */

	for (j = 0; j < h; j++) {
		unsigned long bits;
		int count;
		
		bits = rotateleft(32 - (sox & 31), bits_in[syindex]);

#if defined(PC98_WAB) || defined(PC98_WABEP)
		CIRRUSCHECKWRITEB(destaddr, bank); /* It's OK */
#else
		CIRRUSCHECKWRITEB(destaddr, bank);
#endif
		destp = base + destaddr;

		count = w;
		if (bits == 0xffffffff)
			__memset(destp, fg, w);
		else {
			while (count > 8) {
				*destp = color[bits & 1]; bits >>= 1;
				*(destp + 1) = color[bits & 1]; bits >>= 1;
				*(destp + 2) = color[bits & 1]; bits >>= 1;
				*(destp + 3) = color[bits & 1]; bits >>= 1;
				*(destp + 4) = color[bits & 1]; bits >>= 1;
				*(destp + 5) = color[bits & 1]; bits >>= 1;
				*(destp + 6) = color[bits & 1]; bits >>= 1;
				*(destp + 7) = color[bits & 1]; bits >>= 1;
				destp += 8;
				count -= 8;
			}
			while (count > 0) {
				*destp = color[bits & 1];
				bits >>= 1;
				destp++;
				count--;
			}
		}

		destaddr += destpitch;

		syindex++;
		if (syindex == sh)
			syindex = 0;	/* Wrap pattern vertically. */
	}
}


/*
 * This is a bitblt function. It takes advantage of the 8 data latches that
 * can be enabled in BY8 addressing mode to do efficient vertical bitblts.
 * I believe this makes scrolling bearable on the chips that don't
 * have the bitblt engine, i.e. <= 5424. On a local bus, it may even
 * rival the bitblt engine in speed.
 *
 * Arguments:
 * x1, y1	Coordinates of source area.
 * x2, y2	Coordinates of destination area.
 * w, h		Size of area to be copied.
 * destpitch	Scanline width of screen in bytes.
 *
 * x1 must be equal to x2 (actually works if (x1 % 8 == x2 % 8)).
 * Copies from top to bottom. For overlapping areas, correct if
 * (y1 > y2 || (y1 == y2 && x1 > x2).
 *
 * The data latches work similar to VGA write mode 1 (for planar modes).
 * Basically, a read from display memory fills the 8 latches with 8 pixels,
 * and subsequent writes (CPU data written doesn't matter) will each write
 * the 8 pixels stored in the latches.
 *
 * This new version divides into unbanked regions, with efficient assembler
 * inner loop; the left and right edge are drawn seperately within each
 * region.
 *
 */

#ifdef AVOID_ASM_ROUTINES

static void CirrusLatchCopySpans(srcp, destp, bcount, n, destpitch)
	unsigned char *srcp;
	unsigned char *destp;
	int bcount;
	int n;
	int destpitch;
{
	int i;
	for (i = 0; i < n; i++) {
		int j;
		for (j = 0; j < bcount; j++)
			*(destp + j) = *(srcp + j);
		srcp += destpitch;
		destp += destpitch;
	}
}

#endif

void CirrusLatchedBitBlt(x1, y1, x2, y2, w, h, destpitch)
	int x1, y1, x2, y2, w, h, destpitch;
{
	int j;
	int destaddr, srcaddr;
	unsigned char *destp, *srcp;
	int writebank, readbank;
	int bitoffset;
	int nspans, bcount, leftbyte;

	unsigned char *readbase;	/* Video read window base address. */
	unsigned char *writebase;	/* Video write window */

	readbase = CIRRUSREADBASE();
	writebase = CIRRUSWRITEBASE();

	destaddr = y2 * destpitch + x2;
	srcaddr = y1 * destpitch + x1;

	/* Enable extended write modes, BY8 addressing, and 8 byte data */
	/* latches. Every addressing byte corresponds to 8 pixels. */
	if (cirrusUseLinear) {
		SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING |
			EIGHTDATALATCHES);
	}
	else {
		SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING |
			EIGHTDATALATCHES | DOUBLEBANKED);
	}

	SETWRITEMODE(1);

	SETPIXELMASK(0xff);
	SETFOREGROUNDCOLOR(0);		/* Disable set/reset. */

	/* Bitmask offset of leftmost byte (group of 8 pixels). */
	bitoffset = destaddr & 7;
	leftbyte = bitoffset == 0 ? 0 : 1;
	/* Number of full bytes (groups of 8 pixels). */
	bcount = bitoffset == 0 ? w / 8 : (w - (8 - bitoffset)) / 8;

	destaddr >>= 3;			/* Divide address by 8. */
	srcaddr >>= 3;

	nspans = h;	/* Number of spans to go. */

#if defined(PC98_WAB) || defined(PC98_WABEP)
	CIRRUSSETREADB_WAB(srcaddr, readbank);
	CIRRUSSETWRITEB_WAB(destaddr, writebank);
#else
	CIRRUSSETREADB(srcaddr, readbank);
	CIRRUSSETWRITEB(destaddr, writebank);
#endif

	for (;;) {
		int nread, nwrite, n;
		/* Calculate how many scanlines fit in the banking region. */
		nread = CIRRUSWRITEREGIONLINES(srcaddr, destpitch >> 3);
		nwrite = CIRRUSWRITEREGIONLINES(destaddr, destpitch >> 3);
		n = nread;
		if (nwrite < n)
			n = nwrite;
		if (n > nspans)
			n = nspans;
		nspans -= n;

		/* Do first byte (left edge). */
		if (bitoffset != 0) {
			int i;
			SETPIXELMASK(leftbitmask[bitoffset]);
			srcp = readbase + srcaddr;
			destp = writebase + destaddr;
			for (i = 0; i < n; i++) {
				/* Write mode 1 latch read/write. */
				*destp = *srcp;
				srcp += destpitch >> 3;
				destp += destpitch >> 3;
			}
			SETPIXELMASK(0xff);
		}

		CirrusLatchCopySpans(
			readbase + srcaddr + leftbyte,
			writebase + destaddr + leftbyte,
			bcount,
			n,
			destpitch >> 3
			);

		/* Do last byte (right edge). */
		if (((x1 + w) & 7) > 0) {
			int i;
			SETPIXELMASK(rightbitmask[(x1 + w) & 7]);
			srcp = readbase + srcaddr + leftbyte + bcount;
			destp = writebase + destaddr + leftbyte + bcount;
			for (i = 0; i < n; i++) {
				/* Write mode 1 latch read/write. */
				*destp = *srcp;
				srcp += destpitch >> 3;
				destp += destpitch >> 3;
			}
			SETPIXELMASK(0xff);
		}

		if (nspans == 0)
			break;
		srcaddr += n * (destpitch >> 3);
		destaddr += n * (destpitch >> 3);
#if defined(PC98_WAB) || defined(PC98_WABEP)
		CIRRUSCHECKREADB_WAB(srcaddr, readbank);
		CIRRUSCHECKWRITEB_WAB(destaddr, writebank);
#else
		CIRRUSCHECKREADB(srcaddr, readbank);
		CIRRUSCHECKWRITEB(destaddr, writebank);
#endif
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


void CirrusLatchedBitBltReversed(x1, y1, x2, y2, w, h, destpitch)
	int x1, y1, x2, y2, w, h, destpitch;
{
	int j;
	int destaddr, srcaddr;
	unsigned char *destp, *srcp;
	int writebank, readbank;
	int bitoffset;
	int nspans, bcount, leftbyte;

	unsigned char *readbase;	/* Video read window base address. */
	unsigned char *writebase;	/* Video write window */

	readbase = CIRRUSREADBASE();
	writebase = CIRRUSWRITEBASE();

	destaddr = (y2 + h - 1) * destpitch + x2;
	srcaddr = (y1 + h - 1) * destpitch + x1;

	/* Enable extended write modes, BY8 addressing, and 8 byte data */
	/* latches. Every addressing byte corresponds to 8 pixels. */
	if (cirrusUseLinear) {
		SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING |
			EIGHTDATALATCHES);
	}
	else {
		SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING |
			EIGHTDATALATCHES | DOUBLEBANKED);
	}

	SETWRITEMODE(1);

	SETPIXELMASK(0xff);
	SETFOREGROUNDCOLOR(0);		/* Disable set/reset. */

	/* Bitmask offset of leftmost byte (group of 8 pixels). */
	bitoffset = destaddr & 7;
	leftbyte = bitoffset == 0 ? 0 : 1;
	/* Number of full bytes (groups of 8 pixels). */
	bcount = bitoffset == 0 ? w / 8 : (w - (8 - bitoffset)) / 8;

	destaddr >>= 3;			/* Divide address by 8. */
	srcaddr >>= 3;

	nspans = h;	/* Number of spans to go. */

#if defined(PC98_WAB) || defined(PC98_WABEP)
	CIRRUSSETREADB_WAB(srcaddr, readbank);
	CIRRUSSETWRITEB_WAB(destaddr, writebank);
#else
	CIRRUSSETREADB(srcaddr, readbank);
	CIRRUSSETWRITEB(destaddr, writebank);
#endif

	for (;;) {
		int nread, nwrite, n;

		/* Adjust bank regions. */
#if defined(PC98_WAB) || defined(PC98_WABEP)
		CIRRUSCHECKREVERSEDREADB_WAB(srcaddr, readbank, destpitch >> 3);
		CIRRUSCHECKREVERSEDWRITEB_WAB(destaddr, writebank, destpitch >> 3);
#else
		CIRRUSCHECKREVERSEDREADB(srcaddr, readbank, destpitch >> 3);
		CIRRUSCHECKREVERSEDWRITEB(destaddr, writebank, destpitch >> 3);
#endif

		/* Calculate how many scanlines fit in the banking region. */
		nread = CIRRUSREVERSEDWRITEREGIONLINES(srcaddr, destpitch >> 3);
		nwrite = CIRRUSREVERSEDWRITEREGIONLINES(destaddr, destpitch >> 3);
		n = nread;
		if (nwrite < n)
			n = nwrite;
		if (n > nspans)
			n = nspans;
		nspans -= n;

		/* Do first byte (left edge). */
		if (bitoffset != 0) {
			int i;
			SETPIXELMASK(leftbitmask[bitoffset]);
			srcp = readbase + srcaddr;
			destp = writebase + destaddr;
			for (i = 0; i < n; i++) {
				/* Write mode 1 latch read/write. */
				*destp = *srcp;
				srcp -= destpitch >> 3;
				destp -= destpitch >> 3;
			}
			SETPIXELMASK(0xff);
		}

		CirrusLatchCopySpans(
			readbase + srcaddr + leftbyte,
			writebase + destaddr + leftbyte,
			bcount,
			n,
			- (destpitch >> 3)
			);

		/* Do last byte (right edge). */
		if (((x1 + w) & 7) > 0) {
			int i;
			SETPIXELMASK(rightbitmask[(x1 + w) & 7]);
			srcp = readbase + srcaddr + leftbyte + bcount;
			destp = writebase + destaddr + leftbyte + bcount;
			for (i = 0; i < n; i++) {
				/* Write mode 1 latch read/write. */
				*destp = *srcp;
				srcp -= destpitch >> 3;
				destp -= destpitch >> 3;
			}
			SETPIXELMASK(0xff);
		}

		if (nspans == 0)
			break;
		srcaddr -= n * (destpitch >> 3);
		destaddr -= n * (destpitch >> 3);
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


/*
 * Conventional framebuffer bitblt; no (x1 & 7 == x2 & 7) restriction.
 */

#ifdef __STDC__
void CirrusSimpleBitBlt( int x1, int y1, int x2, int y2, int w, int h,
int destpitch )
#else
void CirrusSimpleBitBlt( x1, y1, x2, y2, w, h, destpitch )
	int x1, y1, x2, y2, w, h, destpitch;
#endif
{
	int j;
	int destaddr, srcaddr;
	unsigned char *destp, *srcp;
	int writebank, readbank;
	int bitoffset;
	int syindex;
	unsigned char *readbase;	/* Video read window base address. */
	unsigned char *writebase;	/* Video write window */
	int saveGRB;

	readbase = CIRRUSREADBASE();
	writebase = CIRRUSWRITEBASE();

	destaddr = y2 * destpitch + x2;
	srcaddr = y1 * destpitch + x1;

#if defined(PC98_WAB) || defined(PC98_WABEP)
	CIRRUSSETREADB_WAB(srcaddr, readbank);
	CIRRUSSETWRITEB_WAB(destaddr, writebank);
#else
	CIRRUSSETREADB(srcaddr, readbank);
	CIRRUSSETWRITEB(destaddr, writebank);
#endif

	for (j = 0; j < h; j++) {
#if defined(PC98_WAB) || defined(PC98_WABEP)
		CIRRUSCHECKREADB_WAB(srcaddr, readbank);
		CIRRUSCHECKWRITEB_WAB(destaddr, writebank);
#else
		CIRRUSCHECKREADB(srcaddr, readbank);
		CIRRUSCHECKWRITEB(destaddr, writebank);
#endif

		/* Address in write window. */
		destp = writebase + destaddr;
		/* Address in read window. */
		srcp = readbase + srcaddr;

		__memcpy(destp, srcp, w);

		destaddr += destpitch;
		srcaddr += destpitch;
	}
}


/*
 * Optimized solid fill.
 * Divides area in regions within banks.
 * Uses the full 64K window in BY8 addressing mode, giving effective
 * bank regions of 512K.
 * w >= 32.
 */

#ifdef AVOID_ASM_ROUTINES

static void CirrusColorExpandWriteSpans(destp, leftmask, leftbcount,
midlcount, rightbcount, rightmask, h, destpitch)
	unsigned char *destp;
	int leftmask, leftbcount, midlcount, rightbcount, rightmask;
	int h;
	int destpitch;
{
	while (h > 0) {
		unsigned char *destpsave;
		int i;
		destpsave = destp;
		if (leftbcount > 0) {
			*destp = leftmask;
			destp++;
			for (i = 0; i < leftbcount - 1; i++) {
				*destp = 0xff;
				destp++;
			}
		}
		for (i = 0; i < midlcount; i++) {
			*(unsigned long *)destp = 0xffffffff;
			destp += 4;
		}
		if (rightbcount > 1) 
			for (i = 0; i < rightbcount - 1; i++) {
				*destp = 0xff;
				destp++;
			}
		if (rightbcount > 0)
			*destp = rightmask;
		destp = destpsave + destpitch;
		h--;
	}
}

#endif

void CirrusColorExpandSolidFill(x, y, w, h, fg, destpitch)
	int x, y, w, h, fg, destpitch;
{
	int destaddr;
	int bank;
	unsigned char *base;	/* Video window base address. */
	int nspans, bitoffset;
	int leftmask, leftbcount, midlcount, rightbcount, rightmask;

	base = CIRRUSSINGLEBASE();		/* Single Read/Write window. */
	destaddr = y * destpitch + x;

	/* Enable extended write modes and BY8 addressing. */
	/* Every addressing byte corresponds to 8 pixels. */
#if defined(PC98_WAB) || defined(PC98_WABEP)
	SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING | DOUBLEBANKED);
#else
	SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING | SINGLEBANKED);
#endif

	SETWRITEMODE(4);

	SETPIXELMASK(0xff);
	SETFOREGROUNDCOLOR(fg);

	/* Calculate masks and counts. */
	/* Bit offset of leftmost pixel of area to be filled. */
	bitoffset = destaddr & 7;
	destaddr >>= 3;			/* Divide address by 8. */
	leftmask = leftbitmask[bitoffset];
	w -= 8 - bitoffset;
	if (w < 0) {
		/* Just one byte. */
		leftmask &= rightbitmask[w + 8];
		leftbcount = 1;
		midlcount = 0;
		rightbcount = 0;
		goto masksdone;
	}
	leftbcount = 4 - (destaddr & 3);
	if ((leftbcount - 1) * 8 > w)
		leftbcount = w / 8 + 1;
	w -= (leftbcount - 1) * 8;
	midlcount = w / 32;
	w &= 31;
	rightbcount = (w + 7) / 8;
	if (rightbcount > 0)
		rightmask = rightbitmask[w - (rightbcount - 1) * 8];
masksdone:

#if defined(PC98_WAB) || defined(PC98_WABEP)
/*	CIRRUSSETREADB_WAB(destaddr, bank); */
	CIRRUSSETSINGLEB(destaddr, bank);
#else
	CIRRUSSETSINGLEB(destaddr, bank);
#endif

	nspans = h;	/* Number of spans to go. */
	for (;;) {
		int n;
		/* Calculate how many scanlines fit in this banking region. */
		n = CIRRUSSINGLEREGIONLINES(destaddr, destpitch >> 3);
		if (n > nspans)
			n = nspans;
		nspans -= n;

		CirrusColorExpandWriteSpans(base + destaddr,
			leftmask, leftbcount, midlcount, rightbcount,
			rightmask, n, destpitch >> 3);

		if (nspans == 0)
			break;
		destaddr += n * (destpitch >> 3);
#if defined(PC98_WAB) || defined(PC98_WABEP)
/*		CIRRUSCHECKSINGLEB_WAB(destaddr, bank); */
		CIRRUSCHECKSINGLEB(destaddr, bank);
#else
		CIRRUSCHECKSINGLEB(destaddr, bank);
#endif
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


/*
 * Optimized stipple, dividing into unbanked regions.
 * Scanlines are written interleaved, maximizing consecutive writes of the
 * same stipple bits word. Assumes a virtual screen width that is a multiple
 * 32 (otherwise video memory access will be largely unaligned).
 */

#ifdef AVOID_ASM_ROUTINES

static void CirrusColorExpandWriteStippleSpans(destp, leftmask, leftbcount,
midlcount, rightbcount, stipplerightmask, h, stippleword, destpitch)
	unsigned char *destp;
	int stippleleftmask, leftbcount, midlcount, rightbcount;
	int stipplerightmask;
	int h;
	unsigned long stippleword;
	int destpitch;
{
	while (h > 0) {
		unsigned char *destpsave;
		int i;
		destpsave = destp;
		switch (leftbcount) {
		case 1 :
			*destp = stippleleftmask;
			destp++;
			break;
		case 2 :
			*destp = stippleleftmask;
			*(destp + 1) = stippleword >> 24;
			destp += 2;
			break;
		case 3 :
			*destp = stippleleftmask;
			*(destp + 1) = stippleword >> 16;
			*(destp + 2) = stippleword >> 24;
			destp + = 3;
			break;
		case 4 :
			*destp = stippleleftmask;
			*(destp + 1) = stippleword >> 8;
			*(unsigned short *)(destp + 2) = stippleword >> 16;
			destp += 4;
			break;
		}
		for (i = 0; i < midlcount; i++) {
			*(unsigned long *)destp = stippleword;
			destp += 4;
		}
		switch (rightbcount) {
		case 1 :
			*destp = stipplerightmask;
			break;
		case 2 :
			*destp = stippleword;
			*(destp + 1) = stipplerightmask;
			break;
		case 3 :
			*(unsigned short *)destp = stippleword;
			*(destp + 2) = stipplerightmask;
			break;
		case 4 :
			*(unsigned short *)destp = stippleword;
			*(destp + 2) = stippleword >> 16;
			*(destp + 3) = stipplerightmask;
			break;
		}
		destp = destpsave + destpitch;
		h--;
	}
}

#endif

void CirrusColorExpandStippleFill(x, y, w, h, bits_in, sh, sox, soy, fg,
destpitch)
	int x, y, w, h;
	unsigned long *bits_in;
	int sh, sox, soy, fg, destpitch;
{
	int destaddr;
	int bank;
	unsigned char *base;	/* Video window base address. */
	int nspans, bitoffset;
	int leftmask, leftbcount, midlcount, rightbcount, rightmask;
	int bytealignment;

	unsigned long *new_bits;
	int i, syindex;

	/* Reverse per-byte bit order of the stipple. */
	new_bits = (unsigned long *)ALLOCATE_LOCAL(sh * 4);
	for (i = 0; i < sh; i++) {
		if (bits_in[i] == 0xffffffff)
			new_bits[i] = 0xffffffff;
		else {
			unsigned long bits;
			/* Rotate so that data is correctly aligned to */
			/* origin for writing dwords to framebuffer. */
			bits = rotateleft(32 - (sox & 31), bits_in[i]);
			/* Reverse each of the four bytes. */
			((unsigned char *)new_bits)[i * 4] =
				byte_reversed[(unsigned char)bits];
			((unsigned char *)new_bits)[i * 4 + 1] =
				byte_reversed[(unsigned char)(bits >> 8)];
			((unsigned char *)new_bits)[i * 4 + 2] =
				byte_reversed[(unsigned char)(bits >> 16)];
			((unsigned char *)new_bits)[i * 4 + 3] =
				byte_reversed[(unsigned char)(bits >> 24)];
		}
	}

	base = CIRRUSSINGLEBASE();	/* Read/write window. */
	destaddr = y * destpitch + x;

	/* Enable extended write modes and BY8 addressing. */
	/* Every addressing byte corresponds to 8 pixels. */
#if defined(PC98_WAB) || defined(PC98_WABEP)
	SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING | DOUBLEBANKED);
#else
	SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING | SINGLEBANKED);
#endif

	SETWRITEMODE(4);

	SETPIXELMASK(0xff);
	SETFOREGROUNDCOLOR(fg);

	/* Calculate masks and counts. */
	/* Bit offset of leftmost pixel of area to be filled. */
	bitoffset = destaddr & 7;
	destaddr >>= 3;			/* Divide address by 8. */
	leftmask = leftbitmask[bitoffset];
	w -= 8 - bitoffset;
	bytealignment = (x >> 3) & 3;
	if (w < 0) {
		/* Just one byte. */
		leftmask &= rightbitmask[w + 8];
		leftbcount = 1;
		midlcount = 0;
		rightbcount = 0;
		goto masksdone;
	}
	leftbcount = 4 - bytealignment;
	if ((leftbcount - 1) * 8 > w)
		/* If the area falls within a 32-bit word, leftbcount does */
		/* not indicate the byte alignment. */
		leftbcount = w / 8 + 1;
	w -= (leftbcount - 1) * 8;
	midlcount = w / 32;
	w &= 31;
	rightbcount = (w + 7) / 8;
	if (rightbcount > 0)
		rightmask = rightbitmask[w - (rightbcount - 1) * 8];
masksdone:

#if defined(PC98_WAB) || defined(PC98_WABEP)
/*	CIRRUSSETREADB_WAB(destaddr, bank); */
	CIRRUSSETSINGLEB(destaddr, bank);
#else
	CIRRUSSETSINGLEB(destaddr, bank);
#endif

	syindex = (y - soy) % sh;	/* y index into source bitmap. */

	nspans = h;	/* Number of spans to go. */

	for (;;) {
		int n, minlines, oneextralimit, startsyindex;
		unsigned char *destp;

		/* Calculate how many scanlines fit in this banking region. */
		n = CIRRUSSINGLEREGIONLINES(destaddr, destpitch >> 3);
		if (n > nspans)
			n = nspans;
		nspans -= n;

		destp = base + destaddr;
		startsyindex = syindex;
		minlines = n / sh;
		oneextralimit = n % sh;
		for (i = 0; i < sh && i < n; i++) {
			int linecount;
			int stippleleftmask, stipplerightmask;
			union {
				unsigned long dword;
				unsigned char byte[4];
			} stipplebits;
			stipplebits.dword = new_bits[syindex];

			if (leftbcount > 0)
				stippleleftmask = leftmask &
					stipplebits.byte[bytealignment];
			if (rightbcount > 0)
				stipplerightmask = rightmask &
					stipplebits.byte[
						(bytealignment + leftbcount +
						rightbcount - 1) & 3];
			linecount = minlines;
			if (i < oneextralimit)
				linecount++;

			CirrusColorExpandWriteStippleSpans(destp,
				stippleleftmask, leftbcount, midlcount,
				rightbcount, stipplerightmask, linecount,
				(destpitch >> 3) * sh, stipplebits.dword);

			destp += (destpitch >> 3);
			syindex++;
			if (syindex >= sh)
				syindex = 0;
		}
		syindex = startsyindex + oneextralimit;
		if (syindex >= sh)
			syindex -= sh;

		if (nspans == 0)
			break;
		destaddr += n * (destpitch >> 3);
#if defined(PC98_WAB) || defined(PC98_WABEP)
/*		CIRRUSCHECKSINGLEB_WAB(destaddr, bank); */
		CIRRUSCHECKSINGLEB(destaddr, bank);
#else
		CIRRUSCHECKSINGLEB(destaddr, bank);
#endif
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

	DEALLOCATE_LOCAL(new_bits);
}


/*
 * Optimized opaque stipple.
 * Within unbanked regions, first draws left edge, then middle, then right
 * edge.
 */

void CirrusColorExpandOpaqueStippleFill(x, y, w, h, bits_in, sh, sox, soy,
bg, fg, destpitch)
	int x, y, w, h;
	unsigned long *bits_in;
	int sh, sox, soy, fg, destpitch;
{
	int destaddr;
	int bank;
	unsigned char *base;	/* Video window base address. */
	int nspans, bitoffset;
	int leftmask, leftbcount, midlcount, rightbcount, rightmask;
	int bytealignment;

	unsigned long *new_bits;
	int i, syindex;
	int startsyindex;

	/* Reverse per-byte bit order of the stipple. */
	new_bits = (unsigned long *)ALLOCATE_LOCAL(sh * 4);
	for (i = 0; i < sh; i++) {
		if (bits_in[i] == 0xffffffff)
			new_bits[i] = 0xffffffff;
		else {
			unsigned long bits;
			/* Rotate so that data is correctly aligned to */
			/* origin for writing dwords to framebuffer. */
			bits = rotateleft(32 - (sox & 31), bits_in[i]);
			/* Reverse each of the four bytes. */
			((unsigned char *)new_bits)[i * 4] =
				byte_reversed[(unsigned char)bits];
			((unsigned char *)new_bits)[i * 4 + 1] =
				byte_reversed[(unsigned char)(bits >> 8)];
			((unsigned char *)new_bits)[i * 4 + 2] =
				byte_reversed[(unsigned char)(bits >> 16)];
			((unsigned char *)new_bits)[i * 4 + 3] =
				byte_reversed[(unsigned char)(bits >> 24)];
		}
	}

	base = CIRRUSSINGLEBASE();		/* Read/write window. */
	destaddr = y * destpitch + x;

	/* Enable extended write modes and BY8 addressing. */
	/* Every addressing byte corresponds to 8 pixels. */
#if defined(PC98_WAB) || defined(PC98_WABEP)
	SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING | DOUBLEBANKED);
#else
	SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING | SINGLEBANKED);
#endif

	SETWRITEMODE(5);	/* Opaque. */

	SETPIXELMASK(0xff);
	SETFOREGROUNDCOLOR(fg);
	SETBACKGROUNDCOLOR(bg);

	/* Calculate masks and counts. */
	/* Bit offset of leftmost pixel of area to be filled. */
	bitoffset = destaddr & 7;
	destaddr >>= 3;			/* Divide address by 8. */
	leftmask = leftbitmask[bitoffset];
	w -= 8 - bitoffset;
	bytealignment = (x >> 3) & 3;
	if (w < 0) {
		/* Just one byte. */
		leftmask &= rightbitmask[w + 8];
		leftbcount = 1;
		midlcount = 0;
		rightbcount = 0;
		goto masksdone;
	}
	leftbcount = 4 - bytealignment;
	if ((leftbcount - 1) * 8 > w)
		/* If the area falls within a 32-bit word, leftbcount does */
		/* not indicate the byte alignment. */
		leftbcount = w / 8 + 1;
	w -= (leftbcount - 1) * 8;
	midlcount = w / 32;
	w &= 31;
	rightbcount = (w + 7) / 8;
	if (rightbcount > 0)
		rightmask = rightbitmask[w - (rightbcount - 1) * 8];
masksdone:

#if defined(PC98_WAB) || defined(PC98_WABEP)
/*	CIRRUSSETREADB_WAB(destaddr, bank); */
	CIRRUSSETSINGLEB(destaddr, bank);
#else
	CIRRUSSETSINGLEB(destaddr, bank);
#endif

	syindex = (y - soy) % sh;	/* y index into source bitmap. */

	nspans = h;	/* Number of spans to go. */

	for (;;) {
		int n, minlines, oneextralimit;
		unsigned char *destp;

		/* Calculate how many scanlines fit in this banking region. */
		n = CIRRUSSINGLEREGIONLINES(destaddr, destpitch >> 3);
		if (n > nspans)
			n = nspans;
		nspans -= n;

		minlines = n / sh;
		oneextralimit = n % sh;

/* Left edge. */ 
		SETPIXELMASK(leftmask);
		destp = base + destaddr;
		startsyindex = syindex;

		for (i = 0; i < n; i++) {
			int linecount;
			union {
				unsigned long dword;
				unsigned char byte[4];
			} stipplebits;
			stipplebits.dword = new_bits[syindex];

			*destp = stipplebits.byte[bytealignment];

			destp += (destpitch >> 3);
			syindex++;
			if (syindex >= sh)
				syindex = 0;
		}

/* Middle part */
		if (midlcount == 0 && leftbcount <= 1 && rightbcount <= 1)
			goto skipmiddlepart;
		SETPIXELMASK(0xff);
		destp = base + destaddr + 1;
		syindex = startsyindex;

		for (i = 0; i < sh && i < n; i++) {
			int linecount;
			union {
				unsigned long dword;
				unsigned char byte[4];
			} stipplebits;
			stipplebits.dword = new_bits[syindex];

			linecount = minlines;
			if (i < oneextralimit)
				linecount++;

			CirrusColorExpandWriteStippleSpans(destp,
				stipplebits.byte[(bytealignment + 1) & 3],
				leftbcount - 1, midlcount,
				(rightbcount == 0 ? 0 : rightbcount - 1),
				stipplebits.byte[(bytealignment + leftbcount
					+ rightbcount - 2) & 3],
				linecount, (destpitch >> 3) * sh,
				stipplebits.dword);

			destp += (destpitch >> 3);
			syindex++;
			if (syindex >= sh)
				syindex = 0;
		}
skipmiddlepart:

/* Right edge */
		if (rightbcount == 0)
			goto skiprightpart;
		SETPIXELMASK(rightmask);
		destp = base + destaddr + leftbcount + midlcount * 4 +
			rightbcount - 1;
		syindex = startsyindex;
		for (i = 0; i < n; i++) {
			int linecount;
			union {
				unsigned long dword;
				unsigned char byte[4];
			} stipplebits;
			stipplebits.dword = new_bits[syindex];

			*destp = stipplebits.byte[(bytealignment + leftbcount
				+ rightbcount - 1) & 3];
			destp += (destpitch >> 3);
			syindex++;
			if (syindex >= sh)
				syindex = 0;
		}
skiprightpart:

		syindex = startsyindex + oneextralimit;
		if (syindex >= sh)
			syindex -= sh;

		if (nspans == 0)
			break;
		destaddr += n * (destpitch >> 3);
#if defined(PC98_WAB) || defined(PC98_WABEP)
/*		CIRRUSCHECKREADB_WAB(destaddr, bank); */
		CIRRUSCHECKSINGLEB(destaddr, bank);
#else
		CIRRUSCHECKSINGLEB(destaddr, bank);
#endif
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
	SETPIXELMASK(0xff);

	DEALLOCATE_LOCAL(new_bits);
}


/*
 * This function uses the 8 data latches for fast multiple-of-8 pixel wide
 * tile fill. Divides into banking regions.
 * Supports tilewidths of 8, 16, 24, 32, 40, 48, 56 and 64.
 *
 * Arguments:
 * x, y		Coordinates of the destination area.
 * w, h		Size of the area.
 * vtileaddr	Offset in video memory of tile.
 * tpitch	Width of a tile line in bytes.
 * tbytes	Width of tile / 8.
 * theight	Height of tile.
 * toy		Tile y-origin.
 *
 * w must be >= 32.
 * Tile must be aligned to start of scanline.
 */

#ifdef AVOID_ASM_ROUTINES

static void CirrusLatchWriteTileSpans(destp, count, tbytes, linecount, vpitch)
	unsigned char *destp;
	int tbytes, count, linecount, vpitch;
{
	int i;
	for (i = 0; i < linecount; i++) {
		int j;
		for (j = 0; j < count; j++)
			*(destp + j * tbytes) = 0;
		destp += vpitch;
	}
}

#endif

void CirrusColorExpandFillTile8(x, y, w, h, vtileaddr, tpitch, tbytes, theight,
toy, destpitch)
	int x, y, w, h;
	int vtileaddr;
	int tpitch;
	int tbytes;	/* Width of the tile in units of 8 pixels. */
	int theight, toy, destpitch;
{
	int i;
	int destlineaddr;
	int writebank;
	int tyindex;
	unsigned char *vtilep;
	int tcount;
	int *chunk8_tcount;
	int nspans;
	int bitoffset;
	int wbytes;
	int oneextracount;
	int tbyteindexleft, tbyteindexright, tbyteindexmiddle;
	unsigned char *readbase;	/* Video read window base address. */
	unsigned char *writebase;	/* Video write window */

	readbase = CIRRUSREADBASE();
	writebase = CIRRUSWRITEBASE();

	chunk8_tcount = (int *)ALLOCATE_LOCAL(tbytes * sizeof(int));

	/* Pointer to tile in read window in BY8 addressing mode. */
	vtileaddr >>= 3;
#if defined(PC98_WAB) || defined(PC98_WABEP)
/*	CIRRUSSETREAD_WAB(vtileaddr); */
	CIRRUSSETREAD(vtileaddr);
#else
	CIRRUSSETREAD(vtileaddr);
#endif
	vtilep = CIRRUSREADBASE() + vtileaddr;

	destlineaddr = y * destpitch;	/* x added later. */

	/* Enable extended write modes, BY8 addressing, and 8 byte data */
	/* latches. Every addressing byte corresponds to 8 pixels. */
	if (cirrusUseLinear) {
		SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING |
			EIGHTDATALATCHES);
	}
	else {
		SETMODEEXTENSIONS(EXTENDEDWRITEMODES | BY8ADDRESSING |
			EIGHTDATALATCHES | DOUBLEBANKED);
	}

	SETWRITEMODE(1);

	SETPIXELMASK(0xff);
	SETFOREGROUNDCOLOR(0);		/* Disable set/reset. */

	destlineaddr >>= 3;			/* Divide address by 8. */

#if defined(PC98_WAB) || defined(PC98_WABEP)
	CIRRUSSETWRITEB(destlineaddr, writebank); /* It's OK */
#else
	CIRRUSSETWRITEB(destlineaddr, writebank);
#endif

	/* Current tile line index. */
	tyindex = (y - toy) % theight;

	bitoffset = x & 7;
	/* Calculate the number of full 8-pixel bytes in each scanline. */
	wbytes = (w - (bitoffset == 0 ? 0 : 8 - bitoffset)) / 8;
	/* Number of full tile 'widths'. */
	tcount = wbytes / tbytes;
	oneextracount = wbytes % tbytes;
	/* Calculate how many full 8 pixels chunks we can write per */
	/* scanline for each of the tbytes chunks in the tile line. */
	for (i = 0; i < oneextracount; i++)
		chunk8_tcount[i] = tcount + 1;
	while (i < tbytes) {
		chunk8_tcount[i] = tcount;
		i++;
	}

	/* Precompute value used in left edge loop. */
	if (bitoffset > 0) {
		tbyteindexleft = (x >> 3) % tbytes;
		tbyteindexmiddle = ((x + 7) >> 3) % tbytes;
	}
	else
		tbyteindexmiddle = (x >> 3) % tbytes;
	if (((x + w) & 7) > 0)
		tbyteindexright = ((x + w) >> 3) % tbytes;

	nspans = h;	/* Number of spans to go. */

	for (;;) {
		int n, minlines, oneextralimit, starttyindex;
		unsigned char *destp, *vtilelinep, *startvtilelinep;

		/* Calculate how many scanlines fit in this banking region. */
		n = CIRRUSWRITEREGIONLINES(destlineaddr, destpitch >> 3);
		if (n > nspans)
			n = nspans;
		nspans -= n;

		minlines = n / theight;
		oneextralimit = n % theight;

		starttyindex = tyindex;
		startvtilelinep = vtilep + tyindex * tbytes;

/* Left edge. */
		if (bitoffset == 0)
			goto skipleftedge;
		SETPIXELMASK(leftbitmask[bitoffset]);
		/* Address in write window. */
		destp = writebase + destlineaddr + (x >> 3);
		vtilelinep = startvtilelinep + tbyteindexleft;
		for (i = 0; i < n; i++) {
			__volatile__ unsigned char tmp;
			tmp = *vtilelinep;
			*destp = 0;
			destp += destpitch >> 3;
			tyindex++;
			vtilelinep += tbytes;
			if (tyindex == theight) {
				tyindex = 0;
				vtilelinep = vtilep + tbyteindexleft;
			}
		}
		SETPIXELMASK(0xff);
skipleftedge:

/* Middle part. */
		destp = writebase + destlineaddr + (x >> 3);
		if (bitoffset > 0)
			destp++;
		vtilelinep = startvtilelinep;
		tyindex = starttyindex;
		for (i = 0; i < theight && i < n; i++) {
			int linecount, k, tbyteindex;

			linecount = minlines;
			if (i < oneextralimit)
				linecount++;

			tbyteindex = tbyteindexmiddle;

			if (linecount > 0)
			for (k = 0; k < tbytes; k++) {
				/* Handle tile line pixels (k * 8) to */
				/* ((k + 1) * 8 - 1). */
				__volatile__ unsigned char tmp;
				/* Read 8 tile pixels into latches. */
				tmp = vtilelinep[tbyteindex];

				/* Now write them at every tbytes-th pixel */
				/* offset in every theight-th scanline in */
				/* the banking region. */

				if (chunk8_tcount[k] > 0)
					CirrusLatchWriteTileSpans(
						destp + k,
						chunk8_tcount[k],
						tbytes,
						linecount,
						(destpitch >> 3) * theight
						);
				tbyteindex++;
				if (tbyteindex >= tbytes)
					tbyteindex = 0;
			}

			destp += (destpitch >> 3);
			tyindex++;
			vtilelinep += tbytes;
			if (tyindex == theight) {
				tyindex = 0;
				vtilelinep = vtilep;
			}
		}

/* Right part. */
		if (((x + w) & 7) == 0)
			goto skiprightpart;
		SETPIXELMASK(rightbitmask[(x + w) & 7]);
		destp = writebase + destlineaddr + ((x + w) >> 3);
		vtilelinep = startvtilelinep + tbyteindexright;
		tyindex = starttyindex;
		for (i = 0; i < n; i++) {
			__volatile__ unsigned char tmp;
			tmp = *vtilelinep;
			*destp = 0;
			destp += destpitch >> 3;
			tyindex++;
			vtilelinep += tbytes;
			if (tyindex == theight) {
				tyindex = 0;
				vtilelinep = vtilep + tbyteindexright;
			}
		}
		SETPIXELMASK(0xff);
skiprightpart:

		if (nspans == 0)
			break;

		/* Calculate tile line index for next region. */
		tyindex = starttyindex + oneextralimit;
		if (tyindex >= theight)
			tyindex -= theight;

		destlineaddr += n * (destpitch >> 3);
#if defined(PC98_WAB) || defined(PC98_WABEP)
		CIRRUSCHECKWRITEB(destlineaddr, writebank); /* It's OK */
#else
		CIRRUSCHECKWRITEB(destlineaddr, writebank);
#endif
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

	DEALLOCATE_LOCAL(chunk8_tcount);
}

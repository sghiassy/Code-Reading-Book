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
/*
 * Modified for use with Chips by David Bateman <dbateman@ee.uts.edu.au>
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_colexp.c,v 3.1 1996/10/17 15:20:42 dawes Exp $ */

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
#include "vga.h"		       /* For vgaInfoRec. */
#include "xf86_HWlib.h"
#include "xf86_OSlib.h"

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
/* Bit masks for left edge (indexed with first left-to-right bit number). */

static unsigned char leftbitmask[8] =
{
    0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01
};

/* Bit masks for right edge (indexed with number of pixels left). */

static unsigned char rightbitmask[9] =
{
    0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
};

/* Rotate the bits of double-word left */
#define rotateleft(n, bits) (((bits) << (n)) | ((bits) >> (32 - (n))))

void 
#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
ctHiQVColorExpandStippleFill(x, y, w, h, bits_in, sh, sox, soy, bg,
    fg, destpitch, isTransparent)
#else
ctMMIOColorExpandStippleFill(x, y, w, h, bits_in, sh, sox, soy, bg,
    fg, destpitch, isTransparent)
#endif
#else
ctcfbColorExpandStippleFill(x, y, w, h, bits_in, sh, sox, soy, bg,
    fg, destpitch, isTransparent)
#endif
    int x, y, w, h;
    unsigned long *bits_in;
    int sh, sox, soy, bg, fg, destpitch;
    int isTransparent;
{
    int destaddr;
    int bank;
    unsigned char *base;	       /* Video window base address. */
    int nspans, bitoffset;
    int leftmask, leftbcount, midlcount, rightbcount, rightmask;
    int bytealignment;

    unsigned long *new_bits;
    int i, syindex;
    int n, minlines, oneextralimit, startsyindex;
    unsigned char *destp;
    int line, srcline;

#ifdef DEBUG
#ifdef CHIPS_MMIO
    ErrorF("CHIPS:ctMMIOColorExpandStippleFill(%d, %d, %d, %d, %d, %d, %d, %d)\n",
	x, y, w, h, sh, sox, soy, destpitch);
#else
    ErrorF("CHIPS:ctcfbColorExpandStippleFill(%d, %d, %d, %d, %d, %d, %d, %d)\n",
	x, y, w, h, sh, sox, soy, destpitch);
#endif
#endif

    if (w == 0)
	return;

    /* Reverse per-byte bit order of the stipple. */
    new_bits = (unsigned long *)ALLOCATE_LOCAL(sh * 4);
    for (i = 0; i < sh; i++) {
	if (bits_in[i] == 0xffffffff)
	    new_bits[i] = 0xffffffff;
	else {
	    unsigned long bits;

	    /* Rotate so that data is correctly aligned to */
	    /* origin for writing dwords to framebuffer. */
	    bits = rotateleft(32 - ((x-sox)%32), bits_in[i]);
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
    destaddr = y * destpitch + x;

    ctSETDSTADDR(destaddr);
    ctSETSRCADDR(0);
    ctSETPITCH(0, destpitch);

    if (isTransparent) {
	ctSETFGCOLOR8(fg);
	ctSETROP(ctAluConv[GXcopy & 0xf] | ctSRCMONO | ctSRCSYSTEM |
	    ctBGTRANSPARENT | ctTOP2BOTTOM | ctLEFT2RIGHT);
    } else {
	ctSETBGCOLOR8(bg);
	ctSETFGCOLOR8(fg);
	ctSETROP(ctAluConv[GXcopy & 0xf] | ctSRCMONO | ctSRCSYSTEM |
	    ctTOP2BOTTOM | ctLEFT2RIGHT);
    }
#ifdef CHIPS_HIQV
    /* 
     * Set up the right-hand clipping of the source data for the
     * 65550, so that it reacts in a similar way to the 6554x's
     */
#if 0
    ctSETMONOCTL(ctCLIPRIGHT(32 - (w % 32)) | ctDWORDALIGN);
#else
    ctSETMONOCTL(ctDWORDALIGN);
#endif
#endif
    ctSETHEIGHTWIDTHGO(h, w);
    line = 0;
    srcline = (y - soy) % sh;

    while (line < h) {
	for (i = 0; i < (((w + 31) & ~31) >> 5); i++) {
	    *(unsigned int *)ctBltDataWindow = 
		((unsigned int *)new_bits)[srcline];
	}
	line++;
	srcline++;
	if (srcline == sh)
	    srcline = 0;
    }
#if CHIPS_HIQV
    /* Make sure that a multiple of qwords are transfer for the HiQV
     * architecture
     */
    if ((((((w + 31) & ~31) >> 5) - 1) * h) & 0x1)
	*(unsigned int *)ctBltDataWindow = 0;
#endif

#if CHIPS_HIQV
    usleep(50);
    while(*(volatile unsigned int *)(ctMMIOBase + 0x10) & (0x80000000)) {
	ErrorF("Flush\n");
	*(unsigned int *)ctBltDataWindow = 0;
	usleep(5);
    }
#else
    ctBLTWAIT;
#endif
    DEALLOCATE_LOCAL(new_bits);
}

/*
 * Auxilliary function for the left edge; width is contained within one
 * byte of the source bitmap.
 * Assumes that some of the BLT registers are already initialized.
 * Static function, no need for seperate MMIO version name.
 */

#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
#define _ctcfbBLTWriteBitmapLeftEdge ctHIQVBLTWriteBitmapLeftEdge
#else
#define _ctcfbBLTWriteBitmap ctMMIOBLTWriteBitmap
#define _ctcfbBLTWriteBitmapLeftEdge ctMMIOBLTWriteBitmapLeftEdge
#endif
#else
#define _ctcfbBLTWriteBitmapLeftEdge ctcfbBLTWriteBitmapLeftEdge
#endif

static __inline__ void _ctcfbBLTWriteBitmapLeftEdge(x, y, w, h, src, bwidth,
srcx, srcy, bg, fg, destpitch)
    int x, y, w, h;
    unsigned char *src;
    int bwidth, srcx, srcy, bg, fg, destpitch;
{
    int destaddr, i, shift;
    unsigned long dworddata;
    unsigned char *srcp;
    unsigned char *base;

    destaddr = y * destpitch + x;

    ctSETSRCADDR(0);
    ctSETDSTADDR(destaddr);
    ctSETROP(ctAluConv[GXcopy & 0xf] | ctSRCMONO | ctSRCSYSTEM |
	ctTOP2BOTTOM | ctLEFT2RIGHT);
#ifdef CHIPS_HIQV
    /* 
     * Set up the right-hand clipping of the source data for the
     * 65550, so that it reacts in a similar way to the 6554x's
     */
#if 0
    ctSETMONOCTL(ctCLIPRIGHT(32 - (w % 32)) | ctDWORDALIGN);
#else
    ctSETMONOCTL(ctDWORDALIGN);
#endif
#endif
    ctSETHEIGHTWIDTHGO(h,w);

    /* Calculate pointer to origin in bitmap. */
    srcp = bwidth * srcy + (srcx >> 3) + src;
    shift = (srcx & 7);

    for (i = 0; i < h; i++) {
	dworddata = (byte_reversed[*srcp] << shift);
	srcp += bwidth;
	*(unsigned int *)ctBltDataWindow = dworddata;
    }
#ifdef CHIPS_HIQV
    /* Make sure that a multiple of qwords are transfer for the HiQV
     * architecture
     */
    if (h & 0x1) *(unsigned int *)ctBltDataWindow = 0;
#endif
#ifdef CHIPS_HIQV
    usleep(50);
    while(*(volatile unsigned int *)(ctMMIOBase + 0x10) & (0x80000000)) {
	ErrorF("Flush\n");
	*(unsigned int *)ctBltDataWindow = 0;
	usleep(5);
    }
#else
    ctBLTWAIT;
#endif

}

static __inline__ void
ctBitmapTransfer(w, h, srcpitch, srcaddr, base)
    int w, h, srcpitch;
    unsigned char *srcaddr;
    unsigned char *base;
{
    unsigned char *data;
    int i, line;

#ifdef DEBUG
    ErrorF("CHIPS:ctBitmapTransferText(%d, %d, %d, 0x%X, 0x%X)\n", w, h,
	srcpitch, srcaddr, base);
#endif

    line = 0;
    data = (unsigned char *)ALLOCATE_LOCAL((w + 3) & ~3);
    while (line < h) {
	i = 0;
	while (i < w) {
	    *(data + i) = byte_reversed[*(srcaddr + i)];
	    i++;
	}
#if 1
        for(i = 0; i < (((w + 3) & ~3) >> 2); i++)
            *(unsigned int *)base=*(unsigned int *)(data + 4 * i);
#else
	memcpy(base, data, ((w + 3) & ~3));	/* Multiple of Double-Word */
#endif
	srcaddr += srcpitch;
	line++;
    }
    DEALLOCATE_LOCAL(data);
}

#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
#define _ctcfbBLTWriteBitmap ctHiQVBLTWriteBitmap
#else
#define _ctcfbBLTWriteBitmap ctMMIOBLTWriteBitmap
#endif
#else
#define _ctcfbBLTWriteBitmap ctcfbBLTWriteBitmap
#endif

void _ctcfbBLTWriteBitmap(x, y, w, h, src, bwidth, srcx, srcy, bg, fg, 
destpitch)
    int x, y, w, h;
    unsigned char *src;
    int bwidth, srcx, srcy, bg, fg, destpitch;
{
    int destaddr;
    int skipleft;
    int bytewidth;	/* Area width in bytes. */
    unsigned char *srcp;
    unsigned char *base;


#ifdef DEBUG
#ifdef CHIPS_MMIO
    ErrorF("CHIPS:ctMMIOBLTWriteBitmap(%d, %d, %d, %d, 0x%X, %d, %d, %d, %d)\n",
	   x, y, w, h, src, bwidth, srcx, srcy, destpitch);
#else
    ErrorF("CHIPS:ctcfbBLTWriteBitmap(%d, %d, %d, %d, 0x%X, %d, %d, %d, %d)\n",
	   x, y, w, h, src, bwidth, srcx, srcy, destpitch);
#endif
#endif

    ctSETBGCOLOR8(bg);
    ctSETFGCOLOR8(fg);
    ctSETPITCH(0,destpitch);

    /* Handle of the left edge (skipleft pixels wide). */ 
    skipleft = 8 - (srcx & 7);
    if (skipleft == 8)
        skipleft = 0;
    if (skipleft) {
	/* Draw left edge. */
	int done;
	done = 0;
	if (skipleft >= w) {
	    skipleft = w;
	    done = 1;
	}
#ifdef CHIPS_MMIO
#ifdef CHIPS_HIQV
	ctHIQVBLTWriteBitmapLeftEdge(x, y, skipleft, h, src, bwidth,
		srcx, srcy, bg, fg, destpitch);
#else
	ctMMIOBLTWriteBitmapLeftEdge(x, y, skipleft, h, src, bwidth,
		srcx, srcy, bg, fg, destpitch);
#endif
#else
	ctcfbBLTWriteBitmapLeftEdge(x, y, skipleft, h, src, bwidth,
		srcx, srcy, bg, fg, destpitch);
#endif
	if (done)
	    return;
    }

    x += skipleft;
    srcx += skipleft;
    w -= skipleft;

    /* Do the rest with a color-expand BitBLT transfer. */
    destaddr = y * destpitch + x;

    /* Calculate pointer to origin in bitmap. */
    srcp = bwidth * srcy + (srcx >> 3) + src;

    /* Number of bytes to be written for each bitmap scanline. */
    bytewidth = (w + 7) / 8;

    ctSETSRCADDR(0);
    ctSETPITCH(0,destpitch);
    ctSETDSTADDR(destaddr);
    ctSETROP(ctAluConv[GXcopy & 0xf] | ctSRCMONO | ctSRCSYSTEM |
	    ctTOP2BOTTOM | ctLEFT2RIGHT);
#ifdef CHIPS_HIQV
    /* 
     * Set up the right-hand clipping of the source data for the
     * 65550, so that it reacts in a similar way to the 6554x's
     */
#if 0
    ctSETMONOCTL(ctCLIPRIGHT(32 - (w % 32)) | ctDWORDALIGN);
#else
    ctSETMONOCTL(ctDWORDALIGN);
#endif
#endif
    ctSETHEIGHTWIDTHGO(h,w);
    ctBitmapTransfer(bytewidth, h, bwidth, srcp, ctBltDataWindow);
#ifdef CHIPS_HIQV
    if (((((w + 31) & ~31) >> 5) * h) & 0x1)
	*(unsigned int *)ctBltDataWindow = 0;
#endif
#ifdef CHIPS_HIQV
    usleep(50);
    while(*(volatile unsigned int *)(ctMMIOBase + 0x10) & (0x80000000)) {
	ErrorF("Flush\n");
	*(unsigned int *)ctBltDataWindow = 0;
	usleep(5);
    }
#else
    ctBLTWAIT;
#endif

}

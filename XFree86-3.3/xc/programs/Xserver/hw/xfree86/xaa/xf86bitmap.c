/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86bitmap.c,v 3.7.2.1 1997/05/21 15:02:55 dawes Exp $ */

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

#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "regionstr.h"
/* PSZ doesn't matter. */
#define PSZ 8
#include "cfb.h"

#include "xf86.h"
#include "xf86xaa.h"
#include "xf86local.h"
#include "xf86expblt.h"

extern unsigned char byte_reversed[256];


/*
 * Color expand monochrome bitmap. Can also be used to upload a stipple
 * to cache. It uses different methods depending on the hardware color
 * expansion functions available.
 *
 * Requirements for CPU-to-screen color expansion:
 *     CPU_SCANLINE_PAD_DWORD or CPU_SCANLINE_PAD_BYTE [disabled]
 *     !(CPU_SCANLINE_PAD_BYTE and BIT_ORDER_IN_BYTE_LSBFIRST)
 *
 * Requirements for buffered scanline-by-scanline screen-to-screen color
 * expansion:
 *     VIDEO_SOURCE_GRANULARITY_PIXEL
 *
 * These restriction can be removed when the respective cases are handled
 * correctly.
 *
 * If necessary, the left edge of the bitmap is written first so that
 * the rest can be drawn with byte-aligned source access.
 *
 * TRIPLE_BITS_24BPP is supported for scanline screen-to-screen color
 * expansion, and for CPU-to-screen with SCANLINE_PAD_DWORD.
 */

static void xf86WriteBitmapLeftEdge(x, y, w, h, src, srcwidth,
srcx, srcy)
    int x, y, w, h;
    unsigned char *src;
    int srcwidth, srcx, srcy;
{
    int destaddr, i, shift;
    unsigned long dworddata;
    unsigned char *srcp;
    unsigned char *base;
    int bytecount;

    xf86AccelInfoRec.SubsequentCPUToScreenColorExpand(x, y, w, h, 0);

    base = (unsigned char *)xf86AccelInfoRec.CPUToScreenColorExpandBase;

    /* Calculate pointer to origin in bitmap. */
    srcp = srcwidth * srcy + (srcx >> 3) + src;
    shift = (srcx & 7);

    if (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_BYTE) {
        bytecount = 0;
        for (i = 0; i < h; i++) {
            if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
	        dworddata = (byte_reversed[(*srcp >> shift) & 0xFF]);
	    else
	        dworddata = *srcp >> shift;
	    srcp += srcwidth;
	    bytecount++;
	    if (bytecount == 4) {
 	        *(unsigned int *)base = dworddata;
 	        dworddata = 0;
	        if (!(xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)) {
	            base += 4;
                    if (base >= (unsigned char *)
                    xf86AccelInfoRec.CPUToScreenColorExpandEndMarker)
                        base = (unsigned char *)
                            xf86AccelInfoRec.CPUToScreenColorExpandBase;
                }
            }
	}
	if (bytecount) {
	    *(unsigned int *)base = dworddata;
	    if (!(xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED))
	        base += 4;
	}
        if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_PAD_QWORD)
	    if ((unsigned long)((unsigned long)base -
	    (unsigned long)xf86AccelInfoRec.CPUToScreenColorExpandBase) & 0x4)
	        *(unsigned int *)base = 0;
    }
    else {
        /* DWORD padding of scanlines. */
        for (i = 0; i < h; i++) {
            if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
                if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
	            dworddata = (byte_reversed_expand3[(*srcp >> shift) & 0xFF]);
	        else
	            dworddata = byte_expand3[(*srcp >> shift) & 0xFF];
	    else
                if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
	            dworddata = (byte_reversed[(*srcp >> shift) & 0xFF]);
	        else
	            dworddata = *srcp >> shift;
	    srcp += srcwidth;
 	    *(unsigned int *)base = dworddata;
	    if (!(xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)) {
	        base += 4;
                if (base >= (unsigned char *)
                xf86AccelInfoRec.CPUToScreenColorExpandEndMarker)
                    base = (unsigned char *)
                        xf86AccelInfoRec.CPUToScreenColorExpandBase;
            }
	}
        if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_PAD_QWORD)
            if (h & 0x1)
                *(unsigned int *)base = 0;
    }
}

void xf86WriteBitmapCPUToScreenColorExpand(x, y, w, h, src, srcwidth, srcx,
srcy, bg, fg, rop, planemask)
    int x, y, w, h;
    unsigned char *src;
    int srcwidth;
    int srcx, srcy;
    int bg, fg;
    int rop;
    unsigned int planemask;
{
    int skipleft;
    int bytewidth;	/* Area width in bytes. */
    unsigned char *srcp;
    unsigned char *base;
    unsigned int *(*DrawBitmapScanlineFunc)(
#if NeedNestedPrototypes
        unsigned int *base,
        unsigned int *bits,
        int nbytes
#endif
    );

    if (xf86AccelInfoRec.ColorExpandFlags & ONLY_TRANSPARENCY_SUPPORTED) {
        /* First fill-in the background. */
        xf86AccelInfoRec.SetupForFillRectSolid(bg, rop, planemask);
        xf86AccelInfoRec.SubsequentFillRectSolid(x, y, w, h);
        if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
            xf86AccelInfoRec.Sync();
        xf86AccelInfoRec.SetupForCPUToScreenColorExpand(
            -1, fg, rop, planemask);
    }
    else
        xf86AccelInfoRec.SetupForCPUToScreenColorExpand(
            bg, fg, rop, planemask);

    if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawBitmapScanlineFunc = xf86DrawBitmapScanline3MSBFirstFixedBase;
            else
                DrawBitmapScanlineFunc = xf86DrawBitmapScanline3MSBFirst;
        else
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawBitmapScanlineFunc = xf86DrawBitmapScanlineMSBFirstFixedBase;
            else
                DrawBitmapScanlineFunc = xf86DrawBitmapScanlineMSBFirst;
    else
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawBitmapScanlineFunc = xf86DrawBitmapScanline3FixedBase;
            else
                DrawBitmapScanlineFunc = xf86DrawBitmapScanline3;
        else
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawBitmapScanlineFunc = xf86DrawBitmapScanlineFixedBase;
            else
                DrawBitmapScanlineFunc = xf86DrawBitmapScanline;

    if ((xf86AccelInfoRec.ColorExpandFlags & LEFT_EDGE_CLIPPING)
    && ((xf86AccelInfoRec.ColorExpandFlags & LEFT_EDGE_CLIPPING_NEGATIVE_X)
    || (x - (srcx & 7) >= 0))) {
        /* We can only allow this if the x-coordinate remains on-screen. */
	skipleft = (srcx & 7);
	srcx = srcx & (~7);	/* Aligned. */
	x -= skipleft;
	w += skipleft;
    }
    else {
        /* Handle the left edge (skipleft pixels wide). */ 
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
	    xf86WriteBitmapLeftEdge(x, y, skipleft, h, src, srcwidth,
	        srcx, srcy);
	    if (done) {
		xf86AccelInfoRec.Sync();
	        return;
	    }
	    if (!(xf86AccelInfoRec.Flags & NO_SYNC_AFTER_CPU_COLOR_EXPAND))
	        xf86AccelInfoRec.Sync();
        }
        x += skipleft;
        srcx += skipleft;
        w -= skipleft;
        skipleft = 0;
    }

    xf86AccelInfoRec.SubsequentCPUToScreenColorExpand(x, y, w, h, skipleft);

    base = (unsigned char *)xf86AccelInfoRec.CPUToScreenColorExpandBase;

    /* Calculate pointer to origin in bitmap. */
    srcp = srcwidth * srcy + (srcx >> 3) + src;

    /* Number of bytes to be written for each bitmap scanline. */
    if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
        /*
         * This is a hack. DrawTextScanline3 expects the third
         * argument to be in pixel units, rather than bytes.
         */
        bytewidth = w;
    else
        bytewidth = (w + 7) / 8;

    if (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_DWORD) {
        int i;
        for (i = 0; i < h; i++) {
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                (*DrawBitmapScanlineFunc)(
                    (unsigned int *)base, (unsigned int *)srcp, bytewidth);
            else {
                base = (unsigned char *)(*DrawBitmapScanlineFunc)(
                    (unsigned int *)base, (unsigned int *)srcp, bytewidth);
                if (base >= (unsigned char *)
                xf86AccelInfoRec.CPUToScreenColorExpandEndMarker)
                    base = (unsigned char *)
                        xf86AccelInfoRec.CPUToScreenColorExpandBase;
            }
            srcp += srcwidth;
        }
        if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_PAD_QWORD) {
            if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP) {
                if (((((w * 3 + 31) & ~31) >> 5) * h) & 0x1)
	            *(unsigned int *)base = 0;
	    }
	    else
                if (((((w + 31) & ~31) >> 5) * h) & 0x1)
	            *(unsigned int *)base = 0;
	}
    }
    else {
    	/* BYTE padding of scanlines. Not nice. */
    	ScanlineReturn sr;
    	int i;
    	sr.bits = 0;
    	sr.base = (unsigned int *)base;
        for (i = 0; i < h; i++) {
            sr = xf86DrawBitmapScanlineMSBFirstBytePadded(
                sr.base, (unsigned int *)srcp, sr.bits, bytewidth);
            if (sr.base >= xf86AccelInfoRec.CPUToScreenColorExpandEndMarker)
                sr.base = (unsigned int *)((unsigned char *)
                    xf86AccelInfoRec.CPUToScreenColorExpandBase +
                    ((unsigned long)sr.base & 3L));
            srcp += srcwidth;
        }
        if ((unsigned long)sr.base & 3) {
            *(unsigned int *)((unsigned long)sr.base & ~3L) = sr.bits;
            sr.base = (unsigned int *)(((unsigned long)base & ~3L) + 4);
        }
        if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_PAD_QWORD)
            if ((unsigned long)sr.base & 0x4)
	        *(unsigned int *)base = 0;
    }

    xf86AccelInfoRec.Sync();
}

/*
 * This function uses two buffers in video memory. It requires linear
 * addressing.
 */

void xf86WriteBitmapScreenToScreenColorExpand(x, y, w, h, src, srcwidth, srcx,
srcy, bg, fg, rop, planemask)
    int x, y, w, h;
    unsigned char *src;
    int srcwidth;
    int srcx, srcy;
    int bg, fg;
    int rop;
    unsigned int planemask;
{
    unsigned char *srcp;
    int bitmapwidth, bytewidth;
    int offset, endoffset;
    int i;
    unsigned int *(*DrawBitmapScanlineFunc)(
#if NeedNestedPrototypes
        unsigned int *base,
        unsigned int *bits,
        int nbytes
#endif
    );

    if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawBitmapScanlineFunc = xf86DrawBitmapScanline3MSBFirstFixedBase;
            else
                DrawBitmapScanlineFunc = xf86DrawBitmapScanline3MSBFirst;
        else
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawBitmapScanlineFunc = xf86DrawBitmapScanlineMSBFirstFixedBase;
            else
                DrawBitmapScanlineFunc = xf86DrawBitmapScanlineMSBFirst;
    else
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawBitmapScanlineFunc = xf86DrawBitmapScanline3FixedBase;
            else
                DrawBitmapScanlineFunc = xf86DrawBitmapScanline3;
        else
            if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED)
                DrawBitmapScanlineFunc = xf86DrawBitmapScanlineFixedBase;
            else
                DrawBitmapScanlineFunc = xf86DrawBitmapScanline;

    if (xf86AccelInfoRec.ColorExpandFlags & ONLY_TRANSPARENCY_SUPPORTED) {
        if (bg != -1) {
	    /* First fill-in the background. */
	    xf86AccelInfoRec.SetupForFillRectSolid(bg, rop, planemask);
	    xf86AccelInfoRec.SubsequentFillRectSolid(x, y, w, h);
	    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
	      xf86AccelInfoRec.Sync();
	}
        xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand(
            x, y, w, h, -1, fg, rop, planemask);
    }
    else
        xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand(
            x, y, w, h, bg, fg, rop, planemask);

    /* Calculate pointer to origin in bitmap. */
    srcp = srcwidth * srcy + (srcx >> 3) + src;

    /* Be careful about the offset into the leftmost source byte. */
    if ((srcx & 7) != 0)
        w += (srcx & 7);
    /* Number of bytes to be written for each bitmap scanline. */
    bytewidth = (w + 7) / 8;

    /* Calculate the non-expanded bitmap width rounded up to 32-bit words, */
    /* in units of pixels. */
    if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
        bitmapwidth = ((bytewidth + 3) / 4) * 32 * 3;
    else
        bitmapwidth = ((bytewidth + 3) / 4) * 32;

    if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
        /*
         * This is a hack. DrawTextScanline3 expects the third
         * argument to be in pixel units, rather than bytes.
         */
        bytewidth = w;

    endoffset = (bitmapwidth / 8) * xf86AccelInfoRec.PingPongBuffers;
    offset = 0;

    for (i = 0; i < h; i++) {
	if (!(xf86AccelInfoRec.Flags & COP_FRAMEBUFFER_CONCURRENCY))
	    xf86AccelInfoRec.Sync();
	if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
	    (*DrawBitmapScanlineFunc)((unsigned int *)
	        (xf86AccelInfoRec.ScratchBufferBase + offset),
	        (unsigned int *)srcp, bytewidth);
	else
	    (*DrawBitmapScanlineFunc)((unsigned int *)
	        (xf86AccelInfoRec.ScratchBufferBase + offset),
	        (unsigned int *)srcp, bytewidth);
	if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
	    xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand(
	        (xf86AccelInfoRec.ScratchBufferAddr + offset) * 8 +
	        (srcx & 7) * 3);
	else
	    xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand(
	        (xf86AccelInfoRec.ScratchBufferAddr + offset) * 8 +
	        (srcx & 7));
	srcp += srcwidth;
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
 * Two-pass color expand solid fill for 24bpp pixels with the coprocessor
 * in 8bpp mode. It performs a solid fill by first doing an opaque
 * color-expand fill to set the first two bytes of each pixel, and then
 * sets the third byte using a transparent color expansion operation.
 *
 * This is a win as far as bus bandwidth concerned (24 / (2 * 3) = 4 times
 * less data), but it does generally use more video memory bandwidth (2x).
 *
 * So it helps in situations where the PCI throughput that is at least
 * two times less than the available video memory bandwidth.
 *
 * Configurations where it helps in any case:
 * - Configurations with relatively poor PCI throughput.
 * - Configurations with LOTS of video memory bandwidth.
 *
 * This currently requires CPU-to-screen color expansion with
 * SCANLINE_PAD_DWORD.
 *
 * CPU_TRANSFER_BASE_FIXED is not yet supported.
 */

static int color24bpp;
static int rasterop;

/* LSBFirst, 11011011 01101101 10110110 11011011 ... */
static unsigned int first_pass_lsb[3] = { 0xDB6DB6DB, 0xB6DB6DB6, 0x6DB6DB6D };
/* MSBFirst, 11011011 10110110 01101101 11011011 ... */
static unsigned int first_pass_msb[3] = { 0xDBB66DDB, 0x6DDBB66D, 0xB66DDBB6 };
/* LSBFirst, 00100100 01001001 10010010 00100100 ... */
static unsigned int second_pass_lsb[3] = { 0x24924924, 0x49249249, 0x92492492 };
/* MSBFirst, 00100100 10010010 01001001 00100100 ... */
static unsigned int second_pass_msb[3] = { 0x24499224, 0x92244992, 0x49922449 };

void xf86SetupForFillRectSolid24bppColorExpand(fg, rop, planemask)
    int fg;
    int rop;
    unsigned int planemask;
{
    color24bpp = fg;
    rasterop = rop;
}

static void Do24bppFillPass(w, h, bits, base)
    int w, h;
    unsigned int *bits;
    unsigned int *base;
{
    int i;
    if (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_DWORD) {
        int words_per_scanline;
        words_per_scanline = (w * 3 + 31) / 32;
        for (i = 0; i < h; i++) {
            int n;
            n = words_per_scanline;
            while (n >= 3) {
                *base = bits[0];
                *(base + 1) = bits[1];
                *(base + 2) = bits[2];
                base += 3;
                n -= 3;
            }
            if (n >= 1) {
                *base = bits[0];
                base++;
                if (n >= 2) {
                    *base = bits[1];
                    base++;
                }
            }
            if (base >= xf86AccelInfoRec.CPUToScreenColorExpandEndMarker)
                base = xf86AccelInfoRec.CPUToScreenColorExpandBase;
        }
        if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_PAD_QWORD)
            if (((((w * 3 + 31) & ~31) >> 5) * h) & 0x1)
	        *(unsigned int *)base = 0;
    }
}

void xf86SubsequentFillRectSolid24bppColorExpand(x, y, w, h)
    int x, y, w, h;
{
    unsigned int *base;

    /* This should set up for 8bpp mode color expansion. */
    xf86AccelInfoRec.SetupForCPUToScreenColorExpand(
        color24bpp, color24bpp >> 8, rasterop, 0xFF);
 
    /*
     * This function should set up for a transfer of a number of bits that
     * is three times the number of pixels (plus alignment).
     */
    xf86AccelInfoRec.SubsequentCPUToScreenColorExpand(x, y, w, h, 0);

    base = xf86AccelInfoRec.CPUToScreenColorExpandBase;

    if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
        Do24bppFillPass(w, h, first_pass_msb, base);
    else
        Do24bppFillPass(w, h, first_pass_lsb, base);

    if (!(xf86AccelInfoRec.Flags & NO_SYNC_AFTER_CPU_COLOR_EXPAND))
        xf86AccelInfoRec.Sync();

    /* Now set up for the last byte of each pixel, with transparency. */
    xf86AccelInfoRec.SetupForCPUToScreenColorExpand(
        -1, color24bpp >> 16, rasterop, 0xFF);

    xf86AccelInfoRec.SubsequentCPUToScreenColorExpand(x, y, w, h, 0);

    if (xf86AccelInfoRec.ColorExpandFlags & BIT_ORDER_IN_BYTE_MSBFIRST)
        Do24bppFillPass(w, h, second_pass_msb, base);
    else
        Do24bppFillPass(w, h, second_pass_lsb, base);

    xf86AccelInfoRec.Sync();
}

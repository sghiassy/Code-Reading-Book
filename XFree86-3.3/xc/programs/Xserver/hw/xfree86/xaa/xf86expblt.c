/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86expblt.c,v 3.8.2.3 1997/05/17 12:25:20 dawes Exp $ */

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

#include "compiler.h"
#include "misc.h"
#include "windowstr.h"
#include "gcstruct.h"
#include "regionstr.h"

#include "xf86.h"
#include "xf86xaa.h"
#include "xf86local.h"

#include "xf86expblt.h"

/*
 * When compiled with MSBFIRST defined,
 * this file produces MSB-first versions of most of the functions.
 * When compiled with FIXEDBASE, fixed-base versions are produced.
 */

/* These are macros for 64-bit integer handling. */

#define UINT64_DECLARE(uint64) \
    unsigned int uint64##low32; \
    unsigned int uint64##high32
#define UINT64_ASSIGN(uint64, int32, int32_2) \
    uint64##low32 = int32; uint64##high32 = int32_2
#define UINT64_LOW32(uint64) uint64##low32
#define UINT64_SHIFTRIGHT32(uint64) uint64##low32 = uint64##high32; uint64##high32 = 0
/*
 * Something fishy is going on with ">> 32". This seems to be equivalent to
 * ">> 0" on my system. I'm not sure whether this is a CPU or compiler bug.
 * We check for this case (shift == 0).
 */
/* shift <= 32 */
#define UINT64_ORLEFTSHIFTEDINT(uint64, int32, shift) \
    uint64##low32 |= int32 << shift; \
    if (shift > 0) \
        uint64##high32 |= int32 >> (32 - shift)


extern unsigned char byte_reversed[256];

/* Macros to write a word, converting the per-byte bit order if required. */

#ifndef MSBFIRST

#ifdef FIXEDBASE

#define WRITE_IN_BITORDER(dest, offset, data) *(dest) = data;

#else  /* Increasing base address. */

#define WRITE_IN_BITORDER(dest, offset, data) *(dest + offset) = data;

#endif

#else	/* "Nasty" bit order within bytes. */

#if defined(__GNUC__) && defined(__i386__)
static __inline__ unsigned int reverse_bitorder(data) {
#if defined(Lynx) || (defined(SYSV) || defined(SVR4)) && !defined(ACK_ASSEMBLER) || (defined(linux) || defined (__OS2ELF__)) && defined(__ELF__)
	__asm__(
		"movl $0,%%ecx\n"
		"movb %%al,%%cl\n"
		"movb byte_reversed(%%ecx),%%al\n"
		"movb %%ah,%%cl\n"
		"movb byte_reversed(%%ecx),%%ah\n"
		"roll $16,%%eax\n"
		"movb %%al,%%cl\n"
		"movb byte_reversed(%%ecx),%%al\n"
		"movb %%ah,%%cl\n"
		"movb byte_reversed(%%ecx),%%ah\n"
		"roll $16,%%eax\n"
		: "=a" (data) : "0" (data)
		: "cx"
		);
#else
	__asm__(
		"movl $0,%%ecx\n"
		"movb %%al,%%cl\n"
		"movb _byte_reversed(%%ecx),%%al\n"
		"movb %%ah,%%cl\n"
		"movb _byte_reversed(%%ecx),%%ah\n"
		"roll $16,%%eax\n"
		"movb %%al,%%cl\n"
		"movb _byte_reversed(%%ecx),%%al\n"
		"movb %%ah,%%cl\n"
		"movb _byte_reversed(%%ecx),%%ah\n"
		"roll $16,%%eax\n"
		: "=a" (data) : "0" (data)
		: "cx"
		);
#endif
	return data;
}
#ifdef FIXEDBASE
#define WRITE_IN_BITORDER(dest, offset, data) *(dest) = reverse_bitorder(data);
#else
#define WRITE_IN_BITORDER(dest, offset, data) \
    *(dest + offset) = reverse_bitorder(data);
#endif
#else	/* If no (gcc on i386), don't use asm. */
#ifdef FIXEDBASE
#define WRITE_IN_BITORDER(dest, offset, data) \
	{ unsigned data2; \
	data2 = byte_reversed[data & 0xFF]; \
	data2 |= byte_reversed[(data & 0xFF00) >> 8] << 8; \
	data2 |= byte_reversed[(data & 0xFF0000) >> 16] << 16; \
	data2 |= byte_reversed[(data & 0xFF000000) >> 24] << 24; \
	*(dest) = data2; \
	}
#else
#define WRITE_IN_BITORDER(dest, offset, data) \
	{ unsigned data2; \
	data2 = byte_reversed[data & 0xFF]; \
	data2 |= byte_reversed[(data & 0xFF00) >> 8] << 8; \
	data2 |= byte_reversed[(data & 0xFF0000) >> 16] << 16; \
	data2 |= byte_reversed[(data & 0xFF000000) >> 24] << 24; \
	*(dest + offset) = data2; \
	}
#endif
#endif

#endif


/*
 * Macros for 24bpp color expansion in 8bpp pixel mode (each bit is
 * expanded to three bits).
 */

#define WRITE_IN_BITORDER3_FIRSTWORD_MACRO(dest, data, mapping) \
    { unsigned data2; \
	data2 = mapping[data & 0xFF]; \
	data2 |= mapping[(data & 0xFF00) >> 8] << 24; \
	*(dest) = data2; \
    }
#define WRITE_IN_BITORDER3_SECONDWORD_MACRO(dest, data, mapping) \
    { unsigned data2; \
	data2 = mapping[(data & 0xFF00) >> 8] >> 8; \
	data2 |= mapping[(data & 0xFF0000) >> 16] << 16; \
	*(dest) = data2; \
    }
#define WRITE_IN_BITORDER3_THIRDWORD_MACRO(dest, data, mapping) \
    { unsigned data2; \
	data2 = mapping[(data & 0xFF0000) >> 16] >> 16; \
	data2 |= mapping[(data & 0xFF000000) >> 24] << 8; \
	*(dest) = data2; \
    }
#define WRITE_IN_BITORDER3_MACRO(dest, data, mapping) \
	WRITE_IN_BITORDER3_FIRSTWORD_MACRO(dest, data, mapping) \
	WRITE_IN_BITORDER3_SECONDWORD_MACRO(dest + 1, data, mapping) \
	WRITE_IN_BITORDER3_THIRDWORD_MACRO(dest + 2, data, mapping)

#ifdef MSBFIRST
#define BYTE_EXPAND byte_reversed_expand3
#else
#define BYTE_EXPAND byte_expand3
#endif

#ifndef FIXEDBASE
#define WRITE_IN_BITORDER3(dest, offset, data) \
	WRITE_IN_BITORDER3_MACRO(dest + offset, data, BYTE_EXPAND)
#define WRITE_IN_BITORDER3_FIRSTWORD(dest, offset, data) \
	WRITE_IN_BITORDER3_FIRSTWORD_MACRO(dest + offset, data, BYTE_EXPAND)
#define WRITE_IN_BITORDER3_SECONDWORD(dest, offset, data) \
	WRITE_IN_BITORDER3_SECONDWORD_MACRO(dest + offset, data, BYTE_EXPAND)
#define WRITE_IN_BITORDER3_THIRDWORD(dest, offset, data) \
	WRITE_IN_BITORDER3_THIRDWORD_MACRO(dest + offset, data, BYTE_EXPAND)
#else
#define WRITE_IN_BITORDER3(dest, offset, data) \
	WRITE_IN_BITORDER3_MACRO(dest, data, BYTE_EXPAND)
#define WRITE_IN_BITORDER3_FIRSTWORD(dest, offset, data) \
	WRITE_IN_BITORDER3_FIRSTWORD_MACRO(dest, data, BYTE_EXPAND)
#define WRITE_IN_BITORDER3_SECONDWORD(dest, offset, data) \
	WRITE_IN_BITORDER3_SECONDWORD_MACRO(dest, data, BYTE_EXPAND)
#define WRITE_IN_BITORDER3_THIRDWORD(dest, offset, data) \
	WRITE_IN_BITORDER3_THIRDWORD_MACRO(dest, data, BYTE_EXPAND)
#endif

#ifdef MSBFIRST
#ifdef FIXEDBASE
#define MAPPEDNAME(x) x##MSBFirstFixedBase
#else
#define MAPPEDNAME(x) x##MSBFirst
#endif
#else
#ifdef FIXEDBASE
#define MAPPEDNAME(x) x##FixedBase
#else
#define MAPPEDNAME(x) x
#endif
#endif

/*
 * Overview of supported low-level color expansion bitmap transfer functions:
 *
 *			Bit order:	LSB	MSB	LSB	MSB(-first)
 *			FIXEDBASE?	No	No	Yes	Yes
 *
 * TE Text, SCANLINE_PAD_DWORD		X/O	X/O	u/o	u/o
 * TE Text, SCANLINE_PAD_BYTE		u/w	u/w	u/w	u/w
 * TE Text, SCANLINE_NO_PAD		u/w	u/w	u/w	u/w
 * TE Text3, SCANLINE_PAD_DWORD		u	u	u	u
 * TE Text3, SCANLINE_PAD_BYTE		-	-	-	-
 * TE Text3, SCANLINE_NO_PAD		-	-	-	-
 * Non-TE Text, SCANLINE_PAD_DWORD	d	d	d	d
 * Non-TE Text, other padding		-	-	-	-
 * Non-TE Text3, any padding		-	-	-	-
 * Bitmap, SCANLINE_PAD_DWORD		X	X	u	u
 * Bitmap, SCANLINE_PAD_BYTE		d	d	d	d
 * Bitmap, SCANLINE_NO_PAD		d	d	d	d
 * Stipple, any padding			d	d	d	d
 *
 * X = Supported, tested
 * u = Supported, untested
 * d = Disabled/unfinished
 * - = Not supported
 * w = Only supports transfer of the whole bitmap, not scanline-by-scanline.
 * o = Optimized
 * O = Really optimized
 *
 * Text3 = Bitmap transfer function for 24bpp with 8bpp color expansion.
 */

#define xf86DrawBitmapScanline \
    MAPPEDNAME(xf86DrawBitmapScanline)
#define xf86DrawBitmapScanline3 \
    MAPPEDNAME(xf86DrawBitmapScanline3)
#define xf86DrawTextScanline \
    MAPPEDNAME(xf86DrawTextScanline)
#define xf86DrawTextNoPad \
    MAPPEDNAME(xf86DrawTextNoPad)
#define xf86DrawTextBytePad \
    MAPPEDNAME(xf86DrawTextBytePad)
#define xf86DrawTextScanline3 \
    MAPPEDNAME(xf86DrawTextScanline3)
#define xf86DrawNonTETextScanline \
    MAPPEDNAME(xf86DrawNonTETextScanline)
#define xf86DrawStippleScanline \
    MAPPEDNAME(xf86DrawStippleScanline)

/* Functions for plain bitmap scanlines. */

unsigned int *xf86DrawBitmapScanline(base, src, nbytes)
    unsigned int *base;
    unsigned int *src;
    int nbytes;
{
    int i, nwords;
    nwords = nbytes / 4;
    while (nwords >= 4) {
    	/* Reshuffled a bit for performance. */
    	WRITE_IN_BITORDER(base, 0, ldl_u((unsigned int *)src));
    	WRITE_IN_BITORDER(base, 1, ldl_u((unsigned int *)src + 1));
    	nwords -= 4;
    	WRITE_IN_BITORDER(base, 2, ldl_u((unsigned int *)src + 2));
#ifndef FIXEDBASE
    	base += 4;
#endif
    	WRITE_IN_BITORDER(base, -1, ldl_u((unsigned int *)src + 3));
    	src += 4;
    }
    for (i = 0; i < nwords; i++) {
    	WRITE_IN_BITORDER(base, i, ldl_u((unsigned int *)src + i));
    }
    src += i;
#ifndef FIXEDBASE
    base += i;
#endif
    switch (nbytes % 4) {
    case 1 :
        WRITE_IN_BITORDER(base, 0, *(unsigned char *)src);
        return base + 1;
    case 2 :
        WRITE_IN_BITORDER(base, 0, ldw_u((unsigned short *)src));
        return base + 1;
    case 3 :
        WRITE_IN_BITORDER(base, 0,
            ldw_u((unsigned short *)src) | (*((unsigned char *)src + 2) << 16));
        return base + 1;
    default :
        break;
    }
    return base;
}

unsigned int *xf86DrawBitmapScanline3(base, src, w)
    unsigned int *base;
    unsigned int *src;
    int w;
{
    int i, nwords;
    unsigned int bits;
    /* Number of full words in the source. */
    nwords = w / 32;
    while (nwords >= 4) {
    	/* Reshuffled a bit for performance. */
    	WRITE_IN_BITORDER3(base, 0, ldl_u((unsigned int *)src));
    	WRITE_IN_BITORDER3(base, 3, ldl_u((unsigned int *)src + 1));
    	nwords -= 4;
    	WRITE_IN_BITORDER3(base, 6, ldl_u((unsigned int *)src + 2));
#ifndef FIXEDBASE
    	base += 12;
#endif
    	WRITE_IN_BITORDER3(base, -3, ldl_u((unsigned int *)src + 3));
    	src += 4;
    }
    while (nwords >= 1) {
    	WRITE_IN_BITORDER3(base, 0, ldl_u((unsigned int *)src));
    	src++;
#ifndef FIXEDBASE
    	base += 3;
#endif
    	nwords --;
    }
    w &= 31;
    if (w == 0)
        return base;
    /* Get the last source bits. */
    switch ((w + 7) / 8) {
    case 1 :
        bits = *(unsigned char *)src;
        break;
    case 2 :
        bits = ldw_u((unsigned short *)src);
        break;
    case 3 :
        bits = ldw_u((unsigned short *)src) | (*((unsigned char *)src + 2) << 16);
        break;
    default : /* 4 */
        bits = ldl_u((unsigned int *)src);
    }
    WRITE_IN_BITORDER3_FIRSTWORD(base, 0, bits);
#ifndef FIXEDBASE
    base++;
#endif
    if (w >= 11) {
        WRITE_IN_BITORDER3_SECONDWORD(base, 0, bits);
#ifndef FIXEDBASE
        base++;
#endif
        if (w >= 22) {
            WRITE_IN_BITORDER3_THIRDWORD(base, 0, bits);
#ifndef FIXEDBASE
            base++;
#endif
        }
    }
    return base;
}

/*
 * nbytes > 0, src access can be unaligned.
 * This isn't used yet. It doesn't work.
 */

#if defined(MSBFIRST) && !defined(FIXEDBASE)
ScanlineReturn xf86DrawBitmapScanlineMSBFirstBytePadded(base, src, bits, nbytes)
    unsigned int *base;
    unsigned int *src;
    unsigned int bits;
    int nbytes;
{
    int i, leftedgebytes, nwords;
    ScanlineReturn ret;
    if (nbytes <= 2) {
        if (nbytes == 1) {
           if (((unsigned long)base & 3L) == 3L)
               *(unsigned int *)((unsigned long)base & ~3L) = bits |
                   ((unsigned int)*(unsigned char *)src << 24);
           else
               ret.bits |= *(unsigned char *)src << 
                   (((unsigned long)base & 3L) * 8);
        }
        else {
            /* nbytes == 2 */
           if (((unsigned long)base & 3L) == 3L) {
               *(unsigned int *)((unsigned long)base & ~3L) = bits |
                   ((unsigned int)*(unsigned char *)src << 24);
               ret.bits = *((unsigned char *)src + 1);
           }
           else
           if (((unsigned long)base & 3L) == 2L)
               *(unsigned int *)((unsigned long)base & ~3L) = bits |
                   ((unsigned int)*(unsigned short *)src << 16);
           else
               ret.bits |= (unsigned int)*(unsigned short *)src << 
                   (((unsigned long)base & 3L) * 8);
        }
        ret.base = (unsigned int *)((unsigned long)base + nbytes);
        return ret;
    }
    /* First combine the left over bytes with new ones to form a word. */
    switch ((unsigned long)base & 3L) {
    case 0 :
        leftedgebytes = 0;
        break;
    case 1 :
        bits |= *(unsigned char *)src << 8;
        bits |= *(unsigned short *)((unsigned char *)src + 1) << 16;
        leftedgebytes = 3;
        break;
    case 2 :
        bits |= *(unsigned short *)src << 16;
        leftedgebytes = 2;
        break;
    case 3 :
        bits |= *(unsigned char *)src << 24;
        leftedgebytes = 1;
        break;
    }
    if (leftedgebytes > 0) {
        base = (unsigned int *)((unsigned long)base & ~3L);
        WRITE_IN_BITORDER(base, 0, bits);
        nbytes -= leftedgebytes;
#ifdef FIXEDBASE
        base++;
#endif
    }
    nwords = nbytes / 4;
    while (nwords >= 4) {
    	/* Reshuffled a bit for performance. */
    	WRITE_IN_BITORDER(base, 0, *src);
    	WRITE_IN_BITORDER(base, 1, *(src + 1));
    	nwords -= 4;
    	WRITE_IN_BITORDER(base, 2, *(src + 2));
    	base += 4;
    	WRITE_IN_BITORDER(base, -1, *(src + 3));
    	src += 4;
    }
    for (i = 0; i < nwords; i++) {
    	WRITE_IN_BITORDER(base, i, *(src + i));
    }
    switch (nbytes) {
    case 0 :
    	ret.base = base + i;
    	break;
    case 1 :
        ret.base = (unsigned int *)((unsigned char *)(base + i) + 1);
        ret.bits = *(unsigned char *)src;
        break;
    case 2 :
        ret.base = (unsigned int *)((unsigned char *)(base + i) + 2);
        ret.bits = *(unsigned short *)src;
        break;
    case 3 :
        ret.base = (unsigned int *)((unsigned char *)(base + i) + 3);
        ret.bits = *(unsigned short *)src;
        ret.bits |= *((unsigned char *)src + 2) << 16;
        break;
    }
    return ret;
}
#endif


/* Functions that generate text bitmap scanlines. */

static unsigned int *DrawTextScanlineWidth6();
unsigned int *DrawTextScanlineWidth6P();	/* Pentium-opt ASM version */
unsigned int *DrawTextScanlineWidth6PMSBFirst();
static unsigned int *DrawTextScanlineWidth8();
unsigned int *DrawTextScanlineWidth8P();
unsigned int *DrawTextScanlineWidth8PMSBFirst();
static unsigned int *DrawTextScanlineWidth10();
static unsigned int *DrawTextScanlineWidth12();
static unsigned int *DrawTextScanlineWidth14();
static unsigned int *DrawTextScanlineWidth16();
static unsigned int *DrawTextScanlineWidth18();
static unsigned int *DrawTextScanlineWidth24();

#if defined(MSBFIRST) || defined(FIXEDBASE)
/* Export just the non-MSBFirst version. */
static
#endif
unsigned int *(*glyphwidth_function[32])(
#if NeedNestedPrototypes
    unsigned int *base, unsigned int **glyphp, int line, int nglyph
#endif
) = {
    NULL, NULL, NULL, NULL, 
    NULL,
#if defined(__i386__) && !defined(FIXEDBASE) /* && defined(NEVER) */
#ifdef MSBFIRST
    DrawTextScanlineWidth6PMSBFirst,	/* Pentium-optimized ASM version. */
#else
    DrawTextScanlineWidth6P,		/* Pentium-optimized ASM version. */
#endif
#else
    DrawTextScanlineWidth6,
#endif
    NULL,
#if defined(__i386__) && !defined(FIXEDBASE) /* && defined(NEVER) */
#ifdef MSBFIRST
    DrawTextScanlineWidth8PMSBFirst,	/* Pentium-optimized ASM version. */
#else
    DrawTextScanlineWidth8P,		/* Pentium-optimized ASM version. */
#endif
#else
    DrawTextScanlineWidth8,
#endif
    NULL, DrawTextScanlineWidth10, NULL, DrawTextScanlineWidth12,
    NULL, DrawTextScanlineWidth14, NULL, DrawTextScanlineWidth16,
    NULL, DrawTextScanlineWidth18, NULL, NULL,
    NULL, NULL, NULL, DrawTextScanlineWidth24,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
};

#if defined(MSBFIRST) && !defined(FIXEDBASE)
/* This one needs to be defined only once. */
int glyphwidth_stretchsize[32] = {
    0, 0, 0, 0, 0, 16, 0, 8, 0, 16, 0, 8, 0, 16, 0, 8,
    0, 16, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0
};
#else
extern int glyphwidth_stretchsize[32];
#endif

unsigned int *xf86DrawTextScanline(base, glyphp, line, nglyph, glyphwidth)
    unsigned int *base;
    unsigned int **glyphp;
    int line;
    int nglyph;
    int glyphwidth;
{
    UINT64_DECLARE(bits);
    int shift, i;

    if (glyphwidth_stretchsize[glyphwidth - 1] > 0
    && glyphwidth_stretchsize[glyphwidth - 1] <= nglyph) {
        int glyphsleft;
        /* 
         * Draw the first stretches of characters using a function
         * customized for the glyph width.
         */
        base = (*glyphwidth_function[glyphwidth - 1])(base, glyphp, line,
            nglyph);
        glyphsleft = nglyph & (glyphwidth_stretchsize[glyphwidth - 1] - 1);
        /* Now do the rest. */
        glyphp += nglyph - glyphsleft;
        nglyph = glyphsleft;
    }

    UINT64_ASSIGN(bits, 0, 0);
    shift = 0;
    i = 0;
    while (i < nglyph) {
        UINT64_ORLEFTSHIFTEDINT(bits, glyphp[i][line], shift);
        shift += glyphwidth;
        if (shift >= 32) {
            /* Write a 32-bit word. */
            WRITE_IN_BITORDER(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
            base++;
#endif
            shift -= 32;
            UINT64_SHIFTRIGHT32(bits);
        }
        i++;
    }
    if (shift > 0) {
        WRITE_IN_BITORDER(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
        base++;
#endif
    }
    return base;
}

/*
 * This function does not transfer one scanline worth of bits like all the
 * other ones, but instead transfers the whole bitmap (padded to a DWORD
 * boundary), without any end-of-scanline padding (SCANLINE_NO_PAD).
 * This function violates the non-XAA dependency of the functions defined
 * in this file (it references the xf86AccelInfoRec).
 */

unsigned int *xf86DrawTextNoPad(base, glyphp, height, nglyph, glyphwidth)
    unsigned int *base;
    unsigned int **glyphp;
    int height;
    int nglyph;
    int glyphwidth;
{
    UINT64_DECLARE(bits);
    int shift, i, line;

    UINT64_ASSIGN(bits, 0, 0);
    shift = 0;
    for (line = 0; line < height; line++) {
        i = 0;
        while (i < nglyph) {
            UINT64_ORLEFTSHIFTEDINT(bits, glyphp[i][line], shift);
            shift += glyphwidth;
            if (shift >= 32) {
                /* Write a 32-bit word. */
                WRITE_IN_BITORDER(base, 0, UINT64_LOW32(bits));
    #ifndef FIXEDBASE
                base++;
    #endif
                shift -= 32;
                UINT64_SHIFTRIGHT32(bits);
            }
            i++;
        }
#ifndef FIXEDBASE
        if ((unsigned char *)base >= (unsigned char *)
                     xf86AccelInfoRec.CPUToScreenColorExpandEndMarker)
           base = (unsigned int *)
                     xf86AccelInfoRec.CPUToScreenColorExpandBase;
#endif
    }
    if (shift > 0) {
        WRITE_IN_BITORDER(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
        base++;
#endif
    }
    return base;
}

/*
 * Similarly, this is for BYTE-padded scanlines.
 */

unsigned int *xf86DrawTextBytePad(base, glyphp, height, nglyph, glyphwidth)
    unsigned int *base;
    unsigned int **glyphp;
    int height;
    int nglyph;
    int glyphwidth;
{
    UINT64_DECLARE(bits);
    int shift, i, line;

    UINT64_ASSIGN(bits, 0, 0);
    shift = 0;
    for (line = 0; line < height; line++) {
        i = 0;
        while (i < nglyph) {
            UINT64_ORLEFTSHIFTEDINT(bits, glyphp[i][line], shift);
            shift += glyphwidth;
            if (shift >= 32) {
                /* Write a 32-bit word. */
                WRITE_IN_BITORDER(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
                base++;
#endif
                shift -= 32;
                UINT64_SHIFTRIGHT32(bits);
            }
            i++;
        }
        /* Pad to nearest BYTE boundary. */
        shift = (shift + 7) & (~7);
        if (shift >= 32) {
            /* Write a 32-bit word. */
            WRITE_IN_BITORDER(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
            base++;
#endif
            shift -= 32;
            UINT64_SHIFTRIGHT32(bits);
        }
#ifndef FIXEDBASE
        if ((unsigned char *)base >= (unsigned char *)
                     xf86AccelInfoRec.CPUToScreenColorExpandEndMarker)
           base = (unsigned int *)
                     xf86AccelInfoRec.CPUToScreenColorExpandBase;
#endif
    }
    if (shift > 0) {
        WRITE_IN_BITORDER(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
        base++;
#endif
    }
    return base;
}


unsigned int *xf86DrawNonTETextScanline(base, glyphinfop, line, nglyph)
    unsigned int *base;
    NonTEGlyphInfo *glyphinfop;
    int line;
    int nglyph;
{
    UINT64_DECLARE(bits);
    int shift, i;

    UINT64_ASSIGN(bits, 0, 0);
    shift = 0;
    i = 0;
    while (i < nglyph) {
        /* Check whether the current glyph has bits for this scanline. */
        if (line >= glyphinfop[i].firstline
        && line <= glyphinfop[i].lastline) {
            UINT64_ORLEFTSHIFTEDINT(bits, glyphinfop[i].bitsp[line], shift);
        }
        shift += glyphinfop[i].width;
        if (shift >= 32) {
            /* Write a 32-bit word. */
            WRITE_IN_BITORDER(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
            base++;
#endif
            shift -= 32;
            UINT64_SHIFTRIGHT32(bits);
        }
        i++;
    }
    if (shift > 0) {
        WRITE_IN_BITORDER(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
        base++;
#endif
    }
    return base;
}


unsigned int *xf86DrawTextScanline3(base, glyphp, line, nglyph, glyphwidth)
    unsigned int *base;
    unsigned int **glyphp;
    int line;
    int nglyph;
    int glyphwidth;
{
    UINT64_DECLARE(bits);
    int shift, i;

    UINT64_ASSIGN(bits, 0, 0);
    shift = 0;
    i = 0;
    while (i < nglyph) {
        UINT64_ORLEFTSHIFTEDINT(bits, glyphp[i][line], shift);
        shift += glyphwidth;
        if (shift >= 32) {
            /* Write a 32-bit word. */
            WRITE_IN_BITORDER3(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
            base += 3;
#endif
            shift -= 32;
            UINT64_SHIFTRIGHT32(bits);
        }
        i++;
    }
    if (shift > 0) {
        WRITE_IN_BITORDER3_FIRSTWORD(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
        base++;
#endif
        if (shift >= 11) {
            WRITE_IN_BITORDER3_SECONDWORD(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
            base++;
#endif
            if (shift >= 22) {
                WRITE_IN_BITORDER3_THIRDWORD(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
                base++;
#endif
            }
        }
    }
    return base;
}


/*
 * The functions below write stretches of glyphs for special font widths.
 * Actually, these functions could also be used for 24bpp color expansion
 * in 8bpp mode.
 */

static unsigned int *DrawTextScanlineWidth6(base, glyphp, line, nglyph)
    unsigned int *base;
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
        WRITE_IN_BITORDER(base, 0, bits);
        bits = glyphp[5][line] >> 2;
        bits |= glyphp[6][line] << 4;
        bits |= glyphp[7][line] << 10;
        bits |= glyphp[8][line] << 16;
        bits |= glyphp[9][line] << 22;
        bits |= glyphp[10][line] << 28;
        WRITE_IN_BITORDER(base, 1, bits);
        bits = glyphp[10][line] >> 4;
        bits |= glyphp[11][line] << 2;
        bits |= glyphp[12][line] << 8;
        bits |= glyphp[13][line] << 14;
        bits |= glyphp[14][line] << 20;
        bits |= glyphp[15][line] << 26;
        WRITE_IN_BITORDER(base, 2, bits);
#ifndef FIXEDBASE
        base += 3;
#endif
        nglyph -= 16;
        glyphp += 16;
    }
    return base;
}

static unsigned int *DrawTextScanlineWidth8(base, glyphp, line, nglyph)
    unsigned int *base;
    unsigned int **glyphp;
    int line;
    int nglyph;
{
#ifdef MSBFIRST
    /*
     * For this font width (and only this one), there's a seperate
     * MSB-first version.
     */
    while (nglyph >= 8) {
        unsigned int bits;
        bits = byte_reversed[glyphp[0][line]];
        bits |= byte_reversed[glyphp[1][line]] << 8;
        bits |= byte_reversed[glyphp[2][line]] << 16;
        bits |= byte_reversed[glyphp[3][line]] << 24;
        *base = bits;
        bits = byte_reversed[glyphp[4][line]];
        bits |= byte_reversed[glyphp[5][line]] << 8;
        bits |= byte_reversed[glyphp[6][line]] << 16;
        bits |= byte_reversed[glyphp[7][line]] << 24;
#ifndef FIXEDBASE
        *(base + 1) = bits;
        base += 2;
#else
	*base = bits;
#endif
        nglyph -= 8;	    
        glyphp += 8;
    }
#else
    while (nglyph >= 8) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 8;
        bits |= glyphp[2][line] << 16;
        bits |= glyphp[3][line] << 24;
        WRITE_IN_BITORDER(base, 0, bits);
        bits = glyphp[4][line];
        bits |= glyphp[5][line] << 8;
        bits |= glyphp[6][line] << 16;
        bits |= glyphp[7][line] << 24;
        WRITE_IN_BITORDER(base, 1, bits);
#ifndef FIXEDBASE
        base += 2;
#endif
        nglyph -= 8;	    
        glyphp += 8;
    }
#endif
#ifdef WHOLE_SCANLINE
    if (nglyph == 0)
        return base;
    do {
        bits = glyphp[8][line];
        if (nglyph == 1)
    	    break;
        bits |= glyphp[9][line] << 16;
        if (nglyph == 2)
            break;
        bits |= glyphp[10][line] << 16;
        if (nglyph == 3)
            break;
        bits |= glyphp[11][line] << 24;
    } while (0);
    WRITE_IN_BITORDER(base, 0, bits);
    if (nglyph <= 4)
        return base + 1;
    do {
        bits = glyphp[8][line];
        if (nglyph == 5)
    	    break;
        bits |= glyphp[9][line] << 16;
        if (nglyph == 6)
            break;
        bits |= glyphp[10][line] << 16;
        if (nglyph == 7)
            break;
        bits |= glyphp[11][line] << 24;
    } while (0);
    WRITE_IN_BITORDER(base, 1, bits);
    return base + 2;
#endif
    return base;
}

static unsigned int *DrawTextScanlineWidth10(base, glyphp, line, nglyph)
    unsigned int *base;
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
        WRITE_IN_BITORDER(base, 0, bits);
        bits = glyphp[3][line] >> 2;
        bits |= glyphp[4][line] << 8;
        bits |= glyphp[5][line] << 18;
        bits |= glyphp[6][line] << 28;
        WRITE_IN_BITORDER(base, 1, bits);
        bits = glyphp[6][line] >> 4;
        bits |= glyphp[7][line] << 6;
        bits |= glyphp[8][line] << 16;
        bits |= glyphp[9][line] << 26;
        WRITE_IN_BITORDER(base, 2, bits);
        bits = glyphp[9][line] >> 6;
        bits |= glyphp[10][line] << 4;
        bits |= glyphp[11][line] << 14;
        bits |= glyphp[12][line] << 24;
        WRITE_IN_BITORDER(base, 3, bits);
        bits = glyphp[12][line] >> 8;
        bits |= glyphp[13][line] << 2;
        bits |= glyphp[14][line] << 12;
        bits |= glyphp[15][line] << 22;
        WRITE_IN_BITORDER(base, 4, bits);
#ifndef FIXEDBASE
        base += 5;
#endif
        nglyph -= 16;
        glyphp += 16;
    }
    return base;
}

static unsigned int *DrawTextScanlineWidth12(base, glyphp, line, nglyph)
    unsigned int *base;
    unsigned int **glyphp; 
    int line;
    int nglyph;
{
    while (nglyph >= 8) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 12;
        bits |= glyphp[2][line] << 24;
        WRITE_IN_BITORDER(base, 0, bits);
        bits = glyphp[2][line] >> 8;
        bits |= glyphp[3][line] << 4;
        bits |= glyphp[4][line] << 16;
        bits |= glyphp[5][line] << 28;
        WRITE_IN_BITORDER(base, 1, bits);
        bits = glyphp[5][line] >> 4;
        bits |= glyphp[6][line] << 8;
        bits |= glyphp[7][line] << 20;
        WRITE_IN_BITORDER(base, 2, bits);
#ifndef FIXEDBASE
        base += 3;
#endif
        nglyph -= 8;
        glyphp += 8;
    }
    return base;
}

static unsigned int *DrawTextScanlineWidth14(base, glyphp, line, nglyph)
    unsigned int *base;
    unsigned int **glyphp; 
    int line;
    int nglyph;
{
    while (nglyph >= 16) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 14;
        bits |= glyphp[2][line] << 28;
        WRITE_IN_BITORDER(base, 0, bits);
        bits = glyphp[2][line] >> 4;
        bits |= glyphp[3][line] << 10;
        bits |= glyphp[4][line] << 24;
        WRITE_IN_BITORDER(base, 1, bits);
        bits = glyphp[4][line] >> 8;
        bits |= glyphp[5][line] << 6;
        bits |= glyphp[6][line] << 20;
        WRITE_IN_BITORDER(base, 2, bits);
        bits = glyphp[6][line] >> 12;
        bits |= glyphp[7][line] << 2;
        bits |= glyphp[8][line] << 16;
        bits |= glyphp[9][line] << 30;
        WRITE_IN_BITORDER(base, 3, bits);
        bits = glyphp[9][line] >> 2;
        bits |= glyphp[10][line] << 12;
        bits |= glyphp[11][line] << 26;
        WRITE_IN_BITORDER(base, 4, bits);
        bits = glyphp[11][line] >> 6;
        bits |= glyphp[12][line] << 8;
        bits |= glyphp[13][line] << 22;
        WRITE_IN_BITORDER(base, 5, bits);
        bits = glyphp[13][line] >> 10;
        bits |= glyphp[14][line] << 4;
        bits |= glyphp[15][line] << 18;
        WRITE_IN_BITORDER(base, 6, bits);
#ifndef FIXEDBASE
        base += 7;
#endif
        nglyph -= 16;
        glyphp += 16;
    }
    return base;
}

static unsigned int *DrawTextScanlineWidth16(base, glyphp, line, nglyph)
    unsigned int *base;
    unsigned int **glyphp;
    int line;
    int nglyph;
{
    while (nglyph >= 8) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 16;
        WRITE_IN_BITORDER(base, 0, bits);
        bits = glyphp[2][line];
        bits |= glyphp[3][line] << 16;
        WRITE_IN_BITORDER(base, 1, bits);
        bits = glyphp[4][line];
        bits |= glyphp[5][line] << 16;
        WRITE_IN_BITORDER(base, 2, bits);
        bits = glyphp[6][line];
        bits |= glyphp[7][line] << 16;
        WRITE_IN_BITORDER(base, 3, bits);
#ifndef FIXEDBASE
        base += 4;
#endif
        nglyph -= 8;	    
        glyphp += 8;
    }
    return base;
}

static unsigned int *DrawTextScanlineWidth18(base, glyphp, line, nglyph)
    unsigned int *base;
    unsigned int **glyphp; 
    int line;
    int nglyph;
{
    while (nglyph >= 16) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 18;
        WRITE_IN_BITORDER(base, 0, bits);
        bits = glyphp[1][line] >> 14;
        bits |= glyphp[2][line] << 4;
        bits |= glyphp[3][line] << 22;
        WRITE_IN_BITORDER(base, 1, bits);
        bits = glyphp[3][line] >> 10;
        bits |= glyphp[4][line] << 8;
        bits |= glyphp[5][line] << 26;
        WRITE_IN_BITORDER(base, 2, bits);
        bits = glyphp[5][line] >> 6;
        bits |= glyphp[6][line] << 12;
        bits |= glyphp[7][line] << 30;
        WRITE_IN_BITORDER(base, 3, bits);
        bits = glyphp[7][line] >> 2;
        bits |= glyphp[8][line] << 16;
        WRITE_IN_BITORDER(base, 4, bits);
        bits = glyphp[8][line] >> 16;
        bits |= glyphp[9][line] << 2;
        bits |= glyphp[10][line] << 20;
        WRITE_IN_BITORDER(base, 5, bits);
        bits = glyphp[10][line] >> 12;
        bits |= glyphp[11][line] << 6;
        bits |= glyphp[12][line] << 24;
        WRITE_IN_BITORDER(base, 6, bits);
        bits = glyphp[12][line] >> 8;
        bits |= glyphp[13][line] << 10;
        bits |= glyphp[14][line] << 28;
        WRITE_IN_BITORDER(base, 7, bits);
        bits = glyphp[14][line] >> 4;
        bits |= glyphp[15][line] << 14;
        WRITE_IN_BITORDER(base, 8, bits);
#ifndef FIXEDBASE
        base += 9;
#endif
        nglyph -= 16;
        glyphp += 16;
    }
    return base;
}

static unsigned int *DrawTextScanlineWidth24(base, glyphp, line, nglyph)
    unsigned int *base;
    unsigned int **glyphp; 
    int line;
    int nglyph;
{
    while (nglyph >= 4) {
        unsigned int bits;
        bits = glyphp[0][line];
        bits |= glyphp[1][line] << 24;
        WRITE_IN_BITORDER(base, 0, bits);
        bits = glyphp[1][line] >> 8;
        bits |= glyphp[2][line] << 16;
        WRITE_IN_BITORDER(base, 1, bits);
        bits = glyphp[2][line] >> 16;
        bits |= glyphp[3][line] << 8;
        WRITE_IN_BITORDER(base, 2, bits);
#ifndef FIXEDBASE
        base += 3;
#endif
        nglyph -= 4;
        glyphp += 4;
    }
    return base;
}

/*
 * Stipple bitmap data transfer. This requires the scanline to start
 * with a horizontal stipple offset that is at a byte (multiple of 8 pixel)
 * boundary in the stipple bitmap scanline.
 * Source data access can be unaligned.
 */
 
unsigned int *xf86DrawStippleScanline(base, src, srcwidth, stipplewidth,
srcoffset, w)
    unsigned int *base;
    unsigned char *src;		/* Pointer to stipple bitmap. */
    int srcwidth;		/* Width of stipple bitmap in bytes. */
    int stipplewidth;		/* Width of stipple in pixels. */
    int srcoffset;		/* The offset in bytes into the stipple */
    				/* of the first pixel. */
    int w;			/* Width of scanline in pixels. */
{
    UINT64_DECLARE(bits);
    int shift, i, sw;
    unsigned char *srcp;

    UINT64_ASSIGN(bits, 0, 0);
    shift = 0;
    i = 0;
    sw = stipplewidth - srcoffset * 8;
    srcp = src + srcoffset;
    for (;;) {
        int dw;
        /*
         * If we can add the whole stipple width, do so.
         * If the stipple width is larger than the number of pixels left
         * to draw, only add part of the stipple.
         */
        dw = min(w, sw);
        if (dw >= 32) {
            UINT64_ORLEFTSHIFTEDINT(bits, ldl_u((unsigned int *)srcp), shift);
            shift += 32;
            sw -= 32;
            w -= 32;
            srcp += 4;
        }
        else {
            /* Make sure no source overrunning occurs. */
            if (dw > 24) {
                UINT64_ORLEFTSHIFTEDINT(bits, ldl_u((unsigned int *)srcp), shift);
            }
            else
            if (dw > 16) {
                unsigned int data;
                data = ldw_u((unsigned short *)srcp) +
                    (*(unsigned char *)(srcp + 2) << 16);
                UINT64_ORLEFTSHIFTEDINT(bits, data, shift);
            }
            else
            if (dw > 8) {
                UINT64_ORLEFTSHIFTEDINT(bits, ldw_u((unsigned short *)srcp), shift);
            }
            else {
                UINT64_ORLEFTSHIFTEDINT(bits, *(unsigned char *)srcp, shift);
            }
            shift += dw;
            sw = 0;
            w -= dw;
        }
        if (shift >= 32) {
            /* Write a 32-bit word. */
            WRITE_IN_BITORDER(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
            base++;
#endif
            shift -= 32;
            UINT64_SHIFTRIGHT32(bits);
        }
        if (w == 0)
            break;
        if (sw == 0) {
            srcp = src;
            sw = stipplewidth;
        }
    }
    if (shift > 0) {
        WRITE_IN_BITORDER(base, 0, UINT64_LOW32(bits));
#ifndef FIXEDBASE
        base++;
#endif
    }
    return base;
}

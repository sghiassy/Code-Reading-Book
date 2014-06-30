/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86bench.c,v 3.5.2.1 1997/05/16 11:35:23 hohndel Exp $ */

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

/*
 * This function implements the benchmark functions that are executed
 * at start-up.
 * This is experimental, and should probably be made optional.
 */

#include "servermd.h"
#include "windowstr.h"
#include "gcstruct.h"
#include "regionstr.h"
#include "mi.h"
#include "fcntl.h"
#include "time.h"
#ifndef Lynx
#include "sys/time.h"
#else
#include <time.h>
#endif

#include "xf86.h"
#include "xf86xaa.h"

static struct timeval tv;
static struct timezone tz;
static struct timeval startclock;
static struct timeval timetaken;

#ifndef DST_NONE
/* note: DST_NONE and struct timezone are neither ANSI-C nor POSIX! */
#define DST_NONE 0
#endif

static void start_timer() {
    struct timeval tv;
    struct timezone tz;
    tz.tz_minuteswest = 0;
    tz.tz_dsttime = DST_NONE;
    gettimeofday(&tv, &tz);
    startclock.tv_sec  = tv.tv_sec;
    startclock.tv_usec = tv.tv_usec;
}

static void stop_timer() {
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    timetaken.tv_sec  = tv.tv_sec  - startclock.tv_sec;
    timetaken.tv_usec = tv.tv_usec - startclock.tv_usec;
}

#define BENCH_START(n) { int bench_index; start_timer(); \
    for (bench_index = 0; bench_index < n; bench_index++) {
#define BENCH_STOP(syncfunc) } syncfunc; stop_timer(); }

static void Sync() {
    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
        xf86AccelInfoRec.Sync();
}

/* Fill with 32-bit words quickly, avoiding REP STOS. */

static void fastmemset(dest, data, nbytes) 
    unsigned char *dest;
    unsigned int data;
    int nbytes;
{
    unsigned char *end;
#ifdef __alpha__
    unsigned long data64;
    data64 = data | (data << 32);
#endif
    end = dest + nbytes;
    while (dest <= end - 128) {
#ifdef __alpha__
        *(unsigned long *)dest = data64;
        *(unsigned long *)(dest + 8) = data64;
        *(unsigned long *)(dest + 16) = data64;
        *(unsigned long *)(dest + 24) = data64;
        *(unsigned long *)(dest + 32) = data64;
        *(unsigned long *)(dest + 40) = data64;
        *(unsigned long *)(dest + 48) = data64;
        *(unsigned long *)(dest + 56) = data64;
        *(unsigned long *)(dest + 64) = data64;
        *(unsigned long *)(dest + 72) = data64;
        *(unsigned long *)(dest + 80) = data64;
        *(unsigned long *)(dest + 88) = data64;
        *(unsigned long *)(dest + 96) = data64;
        *(unsigned long *)(dest + 104) = data64;
        *(unsigned long *)(dest + 112) = data64;
        *(unsigned long *)(dest + 120) = data64;
#else
        *(unsigned int *)dest = data;
        *(unsigned int *)(dest + 4) = data;
        *(unsigned int *)(dest + 8) = data;
        *(unsigned int *)(dest + 12) = data;
        *(unsigned int *)(dest + 16) = data;
        *(unsigned int *)(dest + 20) = data;
        *(unsigned int *)(dest + 24) = data;
        *(unsigned int *)(dest + 28) = data;
        *(unsigned int *)(dest + 32) = data;
        *(unsigned int *)(dest + 36) = data;
        *(unsigned int *)(dest + 40) = data;
        *(unsigned int *)(dest + 44) = data;
        *(unsigned int *)(dest + 48) = data;
        *(unsigned int *)(dest + 52) = data;
        *(unsigned int *)(dest + 56) = data;
        *(unsigned int *)(dest + 60) = data;
        *(unsigned int *)(dest + 64) = data;
        *(unsigned int *)(dest + 68) = data;
        *(unsigned int *)(dest + 72) = data;
        *(unsigned int *)(dest + 76) = data;
        *(unsigned int *)(dest + 80) = data;
        *(unsigned int *)(dest + 84) = data;
        *(unsigned int *)(dest + 88) = data;
        *(unsigned int *)(dest + 92) = data;
        *(unsigned int *)(dest + 96) = data;
        *(unsigned int *)(dest + 100) = data;
        *(unsigned int *)(dest + 104) = data;
        *(unsigned int *)(dest + 108) = data;
        *(unsigned int *)(dest + 112) = data;
        *(unsigned int *)(dest + 116) = data;
        *(unsigned int *)(dest + 120) = data;
        *(unsigned int *)(dest + 124) = data;
#endif
        dest += 128;
    }
    while (dest < end) {
        *(unsigned int *)dest = data;
        dest += 4;
    }
}

static void DoBench(name, npixels, benchfunc)
    char *name;
    int npixels;
    void (*benchfunc)();
{
    int n;
    unsigned long pixrate, memrate;
    /* First execute once to load code, CPU caches etc. */
    (*benchfunc)();
    /* Calibrate and benchmark. */
    n = 1;
    for (;;) {
        BENCH_START(n);
        (*benchfunc)();
        BENCH_STOP(Sync());
        /* Satisfied when it takes at least 0.1 sec. */
        if (((double)timetaken.tv_sec * 1000000 
	     + (double)timetaken.tv_usec) >= 100000)
            break;
        n *= 2;
    }
    /* Calculate pixel rate in 1/100 Mpix/sec. */
    pixrate = (unsigned long)(((double)npixels * n * 100)
			      / ((double)timetaken.tv_sec * 1000000
				 +(double)timetaken.tv_usec));
    memrate = pixrate * (unsigned long)xf86AccelInfoRec.BitsPerPixel / 8;
    ErrorF("%-46s %4u.%02u Mpix/sec (%u.%02u MB/s)\n", name,
        pixrate / 100, pixrate % 100,
        memrate / 100, memrate % 100);
}

static void CPUToFramebuffer()
{
    int i;
    for (i = 0; i < 10; i++)
        fastmemset(xf86AccelInfoRec.FramebufferBase, 0, 40000);
}

static void SolidRectFill10x1()
{
    int i;
    xf86AccelInfoRec.SetupForFillRectSolid(0, GXcopy, ~0);
    for (i = 0; i < 1000; i++)
        xf86AccelInfoRec.SubsequentFillRectSolid(i & 255, i & 255, 10, 1);
}   

static void SolidRectFill40()
{
    int i;
    xf86AccelInfoRec.SetupForFillRectSolid(0, GXcopy, ~0);
    for (i = 0; i < 100; i++)
        xf86AccelInfoRec.SubsequentFillRectSolid(i, i, 40, 40);
}   

static void SolidRectFill400()
{
    int i;
    xf86AccelInfoRec.SetupForFillRectSolid(0, GXcopy, ~0);
    for (i = 0; i < 10; i++)
        xf86AccelInfoRec.SubsequentFillRectSolid(i, i, 400, 400);
}   

static void ScreenCopy10()
{
    int i;
    xf86AccelInfoRec.SetupForScreenToScreenCopy(1, 1, GXcopy, ~0, -1);
    for (i = 0; i < 400; i++)
        xf86AccelInfoRec.SubsequentScreenToScreenCopy(
            i + 1, i + 1, i, i, 10, 10);
}   

static void ScreenCopy40()
{
    int i;
    xf86AccelInfoRec.SetupForScreenToScreenCopy(1, 1, GXcopy, ~0, -1);
    for (i = 0; i < 50; i++)
        xf86AccelInfoRec.SubsequentScreenToScreenCopy(
            i + 1, i + 1, i, i, 40, 40);
}   

static void ScreenCopy400()
{
    int i;
    xf86AccelInfoRec.SetupForScreenToScreenCopy(1, 1, GXcopy, ~0, -1);
    for (i = 0; i < 2; i++)
        xf86AccelInfoRec.SubsequentScreenToScreenCopy(
            i + 1, i + 1, i, i, 400, 400);
}   

static void Scroll400()
{
    int i;
    xf86AccelInfoRec.SetupForScreenToScreenCopy(1, 1, GXcopy, ~0, -1);
    for (i = 0; i < 2; i++)
        xf86AccelInfoRec.SubsequentScreenToScreenCopy(
            0, i + 1, 0, i, 400, 400);
}

static void PatternFill10()
{
    int i;
    xf86AccelInfoRec.SetupForFill8x8Pattern(0, 400, GXcopy, ~0, -1);
    for (i = 0; i < 400; i++)
        xf86AccelInfoRec.SubsequentFill8x8Pattern(0, 400, i, i, 10, 10);
}

static void PatternFill400()
{
    int i;
    xf86AccelInfoRec.SetupForFill8x8Pattern(0, 400, GXcopy, ~0, -1);
    for (i = 0; i < 10; i++)
        xf86AccelInfoRec.SubsequentFill8x8Pattern(0, 400, i, i, 400, 400);
}

static void PatternColorExpand10()
{
    int i;
    xf86AccelInfoRec.SetupFor8x8PatternColorExpand(0, 400, 0, 1, GXcopy, ~0);
    for (i = 0; i < 400; i++)
        xf86AccelInfoRec.Subsequent8x8PatternColorExpand(
            0, 400, 0, 0, 10, 10);
}

static void PatternColorExpand400()
{
    int i;
    xf86AccelInfoRec.SetupFor8x8PatternColorExpand(0, 400, 0, 1, GXcopy, ~0);
    for (i = 0; i < 10; i++)
        xf86AccelInfoRec.Subsequent8x8PatternColorExpand(
            0, 400, 0, 0, 400, 400);
}

static void CPUToScreenColorExpand10()
{
    int i, dwords;
    xf86AccelInfoRec.SetupForCPUToScreenColorExpand(0, 1, GXcopy, ~0);
    if (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_NO_PAD) {
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            dwords = (10 * 3 * 10 + 31) / 32;	/* 10 */
        else
            dwords = (10 * 10 + 31) / 32;   /* 4 */
    }
    if (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_BYTE) {
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            dwords = (16 * 10 + 31) / 32;	/* 5 */
        else
            dwords = 10;
    }
    if (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_DWORD)
        dwords = 10;
    for (i = 0; i < 100; i++) {
        xf86AccelInfoRec.SubsequentCPUToScreenColorExpand(i, i, 10, 10, 0);
        if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED) {
            int j;
            j = dwords;
            while (j > 0) {
                *(xf86AccelInfoRec.CPUToScreenColorExpandBase) = 0xFF00FF00;
                j--;
            }
        }
        else {
            fastmemset(xf86AccelInfoRec.CPUToScreenColorExpandBase,
                    0xFF00FF00, dwords * 4);
        }
        if ((xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_PAD_QWORD)
        && (dwords & 1))
            *(xf86AccelInfoRec.CPUToScreenColorExpandBase) = 0;
            
        if (!(xf86AccelInfoRec.ColorExpandFlags & NO_SYNC_AFTER_CPU_COLOR_EXPAND))
            xf86AccelInfoRec.Sync();
    }
}

static void CPUToScreenColorExpand400()
{
    int i;
    xf86AccelInfoRec.SetupForCPUToScreenColorExpand(0, 1, GXcopy, ~0);
    for (i = 0; i < 2; i++) {
        xf86AccelInfoRec.SubsequentCPUToScreenColorExpand(0, 0, 416, 400, 0);
        if (xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED) {
            int j;
          if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
              for (j = 0; j < 3 * 416 * 400 / 32; j++)
                   *(xf86AccelInfoRec.CPUToScreenColorExpandBase) = 0xFF00FF00;
           else
              for (j = 0; j < 416 * 400 / 32; j++)
                   *(xf86AccelInfoRec.CPUToScreenColorExpandBase) = 0xFF00FF00;
        }
        else {
            int nbytes, windowsize;
           if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
               nbytes = 3 * 416 * 400 / 8;
            else
               nbytes = 416 * 400 / 8;
            windowsize = xf86AccelInfoRec.CPUToScreenColorExpandRange;
            while (nbytes >= windowsize) {
                fastmemset(xf86AccelInfoRec.CPUToScreenColorExpandBase,
                    0xFF00FF00, windowsize);
                nbytes -= windowsize;
            }
            if (nbytes > 0)
                fastmemset(xf86AccelInfoRec.CPUToScreenColorExpandBase,
                    0xFF00FF00, nbytes);
        }
        xf86AccelInfoRec.Sync();
    }
}

static void ScanlineScreenToScreenColorExpand400()
{
    int i;
    xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand(
        0, 0, 416, 400, 0, 1, GXcopy, ~0);
    for (i = 0; i < 400; i++) {
        xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand(
            (i & 1) * 1024);
    }
}

static void ScreenToScreenColorExpand10()
{
    int i;
    xf86AccelInfoRec.SetupForScreenToScreenColorExpand(
        0, 1, GXcopy, ~0);
    for (i = 0; i < 200; i++) {
        xf86AccelInfoRec.SubsequentScreenToScreenColorExpand(
            0, 400 + (i & 1), i & 255, i & 255, 10, 10);
    }
}

static void TransparentScreenToScreenColorExpand10()
{
    int i;
    xf86AccelInfoRec.SetupForScreenToScreenColorExpand(
        -1, 1, GXcopy, ~0);
    for (i = 0; i < 1000; i++) {
        xf86AccelInfoRec.SubsequentScreenToScreenColorExpand(
            0, 400 + (i & 1), i & 255, i & 255, 10, 10);
    }
}

void
xf86Bench()
{
    DoBench("CPU to framebuffer",
        40000 * 10 / (xf86AccelInfoRec.BitsPerPixel / 8), CPUToFramebuffer);
    if (xf86AccelInfoRec.SubsequentFillRectSolid) {
        DoBench("10x1 solid rectangle fill", 10 * 1 * 1000, SolidRectFill10x1);
        DoBench("40x40 solid rectangle fill", 40 * 40 * 100, SolidRectFill40);
        DoBench("400x400 solid rectangle fill", 400 * 400 * 10,
            SolidRectFill400);
    }
    if (xf86AccelInfoRec.SubsequentScreenToScreenCopy) {
        DoBench("10x10 screen copy", 10 * 10 * 400, ScreenCopy10);
        DoBench("40x40 screen copy", 40 * 40 * 50, ScreenCopy40);
        DoBench("400x400 screen copy", 400 * 400 * 2, ScreenCopy400);
        DoBench("400x400 aligned screen copy (scroll)", 400 * 400 * 2,
            Scroll400);
    }
    if (xf86AccelInfoRec.SubsequentFill8x8Pattern) {
        DoBench("10x10 8x8 pattern fill", 10 * 10 * 400, PatternFill10);
        DoBench("400x400 8x8 pattern fill", 400 * 400 * 10, PatternFill400);
    }
    if (!(OFLG_ISSET(OPTION_XAA_NO_COL_EXP, &(xf86AccelInfoRec.ServerInfoRec->options)))) {

    if (xf86AccelInfoRec.Subsequent8x8PatternColorExpand) {
        DoBench("10x10 8x8 color expand pattern fill", 10 * 10 * 400,
            PatternColorExpand10);
        DoBench("400x400 8x8 color expand pattern fill", 400 * 400 * 10,
            PatternColorExpand400);
    }
    if (xf86AccelInfoRec.SubsequentCPUToScreenColorExpand) {
        DoBench("10x10 CPU-to-screen color expand", 10 * 10 * 10,
            CPUToScreenColorExpand10);
        DoBench("416x400 CPU-to-screen color expand", 416 * 400 * 2,
            CPUToScreenColorExpand400);
    }
    if (xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand) {
        DoBench("416x400 scanline screen-to-screen color expand", 416 * 400,
            ScanlineScreenToScreenColorExpand400);
    }
    if (xf86AccelInfoRec.SubsequentScreenToScreenColorExpand) {
        DoBench("10x10 screen-to-screen color expand", 10 * 10 * 200,
            ScreenToScreenColorExpand10);
    }

    }
}

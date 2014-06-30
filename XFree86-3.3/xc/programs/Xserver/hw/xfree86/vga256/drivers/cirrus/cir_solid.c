/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_solid.c,v 3.8 1996/12/23 06:56:53 dawes Exp $ */

/*

Copyright (c) 1987  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.


Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the name of Sun not be used in advertising
or publicity pertaining to distribution  of  the software
without specific prior written permission. Sun makes no 
representations about the suitability of this software for 
any purpose. It is provided "as is" without any express or 
implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
Copyright 1994 by Henry A. Worth, Sunnyvale, California.
Copyright 1994 by Harm Hanemaayer, Utrecht, The Netherlands

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN, RICKARD E. FAITH, AND HENRY A. WORTH DISCLAIM ALL 
WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES 
OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
Modified for the AGX by Henry A. Worth (haw30@eng.amdahl.com)
Modified for Cirrus by Harm Hanemaayer (hhanemaa@cs.ruu.nl)

*/
/* $XConsortium: cir_solid.c /main/7 1996/10/26 11:17:11 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "xf86.h"
#include "vga256.h"
#include "vga.h"
#include "cfbrrop.h"
#include "mergerop.h"
#include "compiler.h"
#include "cir_driver.h"
#include "cirBlitMM.h"		/* MMIO BitBLT commands */


/*
 * Auxilliary function that writes a color-expand solid pattern
 * for solid filling with the color-expand BitBLT fill.
 */

void CirrusWriteSolidPattern() {
    unsigned char *base;
    int patternaddr;
    patternaddr = cirrusBLTPatternAddress;
    CIRRUSSETWRITE(patternaddr);
    base = CIRRUSWRITEBASE();
    *(unsigned long *)(base + patternaddr) =
        0xFFFFFFFF;
    *(unsigned long *)(base + patternaddr + 4) =
        0xFFFFFFFF;
}

/*
 * Auxilliary function that sets up the MMIO BitBLT foreground color,
 * and the MMIO BitBLT mode register, for a depth of 8, 16 or 32.
 */

static void SetForegroundColorAndBlitMode(fg, mode)
    unsigned int fg;
    int mode;
{
    if (vgaBitsPerPixel == 8) {
        SETFOREGROUNDCOLOR(fg);
	SETBLTMODE(mode);
	return;
    }
    if (vgaBitsPerPixel == 16) {
        SETFOREGROUNDCOLOR16(fg);
        SETBLTMODE(mode | PIXELWIDTH16);
        return;
    }
    /* vgaBitsPerPixel == 32 */
    SETFOREGROUNDCOLOR32(fg);
    SETBLTMODE(mode | PIXELWIDTH32);
}

/* This one also sets the background color. */

static void SetColorsAndBlitMode(fg, bg, mode)
    unsigned int fg;
    unsigned int bg;
    int mode;
{
    if (vgaBitsPerPixel == 8) {
        SETFOREGROUNDCOLOR(fg);
        SETBACKGROUNDCOLOR(bg);
	SETBLTMODE(mode);
	return;
    }
    if (vgaBitsPerPixel == 16) {
        SETFOREGROUNDCOLOR16(fg);
        SETBACKGROUNDCOLOR16(bg);
        SETBLTMODE(mode | PIXELWIDTH16);
        return;
    }
    /* vgaBitsPerPixel == 32 */
    SETFOREGROUNDCOLOR32(fg);
    SETBACKGROUNDCOLOR32(bg);
    SETBLTMODE(mode | PIXELWIDTH32);
}


/*
 * Accelerated solid filled spans for 8, 16 and 32bpp.
 * Requires memory-mapped I/O (5429, 543x) for acceptable performance.
 * Each span is drawn with a seperate BitBLT command.
 * Command overhead is reduced as far as possible.
 */

void CirrusFillSolidSpansGeneral(pDrawable, pGC, nInit, pptInit, pwidthInit,
fSorted)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
    int         fSorted;
{
    int n;                      /* number of spans to fill */
    register DDXPointPtr ppt;   /* pointer to list of start points */
    register int *pwidth;       /* pointer to list of n widths */
    DDXPointPtr initPpt;
    int *initPwidth;
    unsigned short width;
    int pitch, busy, pixshift;
    RROP_DECLARE

    /* We only handle on-screen spans with full planemask. */
    if (vgaBitsPerPixel == 8 && (!xf86VTSema ||
    (pGC->planemask & 0xFF) != 0xFF)) {
    	vga256SolidSpansGeneral(pDrawable, pGC,
                                nInit, pptInit, pwidthInit, fSorted);
        return;
    }
    else
    if (vgaBitsPerPixel == 16 && (!xf86VTSema ||
    (pGC->planemask & 0xFFFF) != 0xFFFF)) {
    	cfb16SolidSpansGeneral(pDrawable, pGC,
                               nInit, pptInit, pwidthInit, fSorted);
	return;
    }
    else
    if (vgaBitsPerPixel == 24 && (!xf86VTSema ||
    (pGC->planemask & 0x00FFFFFF) != 0x00FFFFFF)) {
        cfb24SolidSpansGeneral(pDrawable, pGC,
                               nInit, pptInit, pwidthInit, fSorted);
       return;
    }
    else
    if (vgaBitsPerPixel == 32 && (!xf86VTSema ||
    (pGC->planemask & 0xFFFFFFFF) != 0xFFFFFFFF)) {
        cfb32SolidSpansGeneral(pDrawable, pGC,
                               nInit, pptInit, pwidthInit, fSorted);
       return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
    	/* 
    	 * This would be handled better by writing decent ValidateGC
    	 * code for the SVGA server.
    	 */
        if (vgaBitsPerPixel == 8)
    	    cfbSolidSpansGeneral(pDrawable, pGC,
                                   nInit, pptInit, pwidthInit, fSorted);
        else
        if (vgaBitsPerPixel == 16)
    	    cfb16SolidSpansGeneral(pDrawable, pGC,
                                   nInit, pptInit, pwidthInit, fSorted);
        else
        if (vgaBitsPerPixel == 24)
    	    cfb24SolidSpansGeneral(pDrawable, pGC,
                                   nInit, pptInit, pwidthInit, fSorted);
        else
    	    cfb32SolidSpansGeneral(pDrawable, pGC,
                                   nInit, pptInit, pwidthInit, fSorted);
	return;
    }

    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand( ((cfbPrivGC *)
                                   (pGC->devPrivates[cfbGCPrivateIndex].ptr))
                                      ->pCompositeClip );
    initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    initPpt = ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans( ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))
                        ->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted);

    RROP_FETCH_GC(pGC);

    pitch = vga256InfoRec.displayWidth;
    pixshift = vgaBitsPerPixel >> 4;
    pitch <<= pixshift;

    /*
     * Source bit pattern is irrelevant, background and foregound color
     * are set to the same value.
     */

    /* Set up the invariant BitBLT parameters. */
    SETSRCADDR(0);
    SETROP(cirrus_rop[pGC->alu]);
    SETDESTPITCH(pitch);
    SETHEIGHT(1);
    SetColorsAndBlitMode(pGC->fgPixel, pGC->fgPixel,
        FORWARDS | PATTERNCOPY | COLOREXPAND);

    while (n--) {
    	int destaddr;
    	int width_in_bytes;

	/* Do the address calculation while the previous blit is running. */

	destaddr = ppt->y * pitch + (ppt->x << pixshift);
	width_in_bytes = *pwidth << pixshift;

	do { BLTBUSY(busy); } while (busy);

	SETDESTADDR(destaddr);
	SETWIDTH(width_in_bytes);
	STARTBLT();

        ppt++;
        pwidth++;
    }

    do { BLTBUSY(busy); } while (busy);

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}


/*
 * Optimized accelerated solid filled rectangles for 8, 16 and 32bpp.
 * Requires memory-mapped I/O (5429, 543x).
 * Command overhead is reduced as far as possible.
 */

void
CirrusMMIOFillRectSolid(pDrawable, pGC, nBox, pBox)
    DrawablePtr	    pDrawable;
    GCPtr	    pGC;
    int		    nBox;
    BoxPtr	    pBox;
{
    int pitch, busy, pixshift;
    RROP_DECLARE

    /* We only handle on-screen fills with full planemask. */
    if (vgaBitsPerPixel == 8 && (!xf86VTSema ||
    (pGC->planemask & 0xFF) != 0xFF || pDrawable->type != DRAWABLE_WINDOW)) {
    	vga256FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
        return;
    }
    else
    if (vgaBitsPerPixel == 16 && (!xf86VTSema ||
    (pGC->planemask & 0xFFFF) != 0xFFFF ||
    pDrawable->type != DRAWABLE_WINDOW)) {
    	cfb16FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
	return;
    }
    else
    if (vgaBitsPerPixel == 24 && (!xf86VTSema ||
    (pGC->planemask & 0xFFFFFF) != 0xFFFFFF ||
    pDrawable->type != DRAWABLE_WINDOW)) {
    	cfb24FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
	return;
    }
    else
    if (vgaBitsPerPixel == 32 && (!xf86VTSema ||
    (pGC->planemask & 0xFFFFFFFF) != 0xFFFFFFFF ||
    pDrawable->type != DRAWABLE_WINDOW)) {
   	cfb32FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
        return;
    }

    RROP_FETCH_GC(pGC);

    pitch = vga256InfoRec.displayWidth;
    pixshift = vgaBitsPerPixel >> 4;
    pitch <<= pixshift;

    /*
     * Source bit pattern is irrelevant, background and foregound color
     * are set to the same value.
     */
   
    /* Set up the invariant BitBLT parameters. */
    SETSRCADDR(0);
    SETROP(cirrus_rop[pGC->alu]);
    SETDESTPITCH(pitch);
    SetColorsAndBlitMode(pGC->fgPixel, pGC->fgPixel,
        FORWARDS | PATTERNCOPY | COLOREXPAND);

    while (nBox) {
        int height, width_in_bytes, destaddr;

	height = pBox->y2 - pBox->y1;
	width_in_bytes = (pBox->x2 - pBox->x1) << pixshift;
	destaddr = pBox->y1 * pitch + (pBox->x1 << pixshift);

	if (height > 1024) {
		/* @%$@!#! 5429/5430/4 only works with heights up to 1024. */
		do { BLTBUSY(busy); } while (busy);
		SETDESTADDR(destaddr);
		SETWIDTH(width_in_bytes);
		SETHEIGHT(1024);
		STARTBLT();
		destaddr += 1024 * pitch;
		height -= 1024;
		do { BLTBUSY(busy); } while (busy);
		SETDESTADDR(destaddr);
		SETWIDTH(width_in_bytes);
		SETHEIGHT(height);
		STARTBLT();
	}
	else {
		do { BLTBUSY(busy); } while (busy);
		SETDESTADDR(destaddr);
		SETWIDTH(width_in_bytes);
		SETHEIGHT(height);
		STARTBLT();
	}

	nBox--;
	pBox++;
    }

    do { BLTBUSY(busy); } while (busy);
}

/* Wrapper for vga256lowlevFuncs.fillBoxSolid(). Currently not used. */

void CirrusMMIOFillBoxSolid(pDrawable, nBox, pBox, pixel1, pixel2, alu) {
	struct _GC dummyGC;
	dummyGC.fgPixel = pixel1;
	dummyGC.bgPixel = pixel2;
	dummyGC.alu = alu;
	CirrusMMIOFillRectSolid(pDrawable, &dummyGC, nBox, pBox);
}

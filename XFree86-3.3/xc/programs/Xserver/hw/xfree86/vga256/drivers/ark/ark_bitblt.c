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
 * DAVID WEXELBLAT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Written by Harm Hanemaayer (H.Hanemaayer@inter.nl.net).
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/ark/ark_bitblt.c,v 3.1 1996/09/23 13:27:16 dawes Exp $ */

#include "compiler.h"
#include "vga256.h"
#include "xf86.h"
#include "vga.h"

#include "ark_driver.h"
#include "ark_cop.h"

/*
 * This function does a low-level ScreenCopy using the COP in 8bpp mode.
 * It is a replacement for vgaBitBlt in the vga256 code, by being
 * the function assigned to vga256LowlevFuncs.vgaBitblt.
 *
 * The alu is always GXcopy, and the planemask is 0xFF. This is can
 * be traced to vga256CopyArea in vgabitblt.c.
 */

void
ArkBitBlt(pdstBase, psrcBase, widthSrc, widthDst, x, y,
	    x1, y1, w, h, xdir, ydir, alu, planemask)
     unsigned char *psrcBase, *pdstBase;  /* start of src and dst bitmaps */
     int    widthSrc, widthDst;
     int    x, y, x1, y1, w, h;
     int    xdir, ydir;
     int    alu;
     unsigned long  planemask;

{
	int dir, srcaddr, destaddr;

	/*
	 * Watch out for these negative values -- it is a relic of the
	 * vga256 framebuffer code.
	 */
	if (widthSrc < 0)
		widthSrc = -widthSrc;
	if (widthDst < 0)
		widthDst = -widthDst;

	/* Standard screen-to-screen operation. */
	SETBITMAPCONFIG(LINEARSOURCEADDR | LINEARDESTADDR);
	dir = 0;
	if (ydir < 0) {
		srcaddr = (y + h - 1) * widthSrc;
		destaddr = (y1 + h - 1) * widthSrc;
		dir |= UP;
	}
	else {
		srcaddr = y * widthSrc;
		destaddr = y1 * widthSrc;
		dir |= DOWN;	/* no-op */
	}
	if (xdir < 0) {
		srcaddr += x + w - 1;
		destaddr += x1 + w - 1;
		dir |= LEFT;
	}
	else {
		srcaddr += x;
		destaddr += x1;
		dir |= RIGHT; /* no-op */
	}
	SETWIDTH(w);
	SETHEIGHT(h);
	if (w > h)
		dir |= DXGREATERTHANDY;	/* no-op */
	else
		dir |= DYGREATERTHANDX;
	SETSOURCEADDR(srcaddr);
	SETDESTADDR(destaddr);
	SETCOMMAND(BACKGROUNDBITMAP | FOREGROUNDBITMAP |
		DISABLEPLANEMASK | DISABLECLIPPING | dir | BITBLT);
	WAITUNTILFINISHED();
}

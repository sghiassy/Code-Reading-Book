/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_im.c,v 3.13 1996/12/23 06:56:48 dawes Exp $ */
/*
 *
 * Copyright 1993 by Bill Reynolds, Santa Fe, New Mexico
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Bill Reynolds not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Bill Reynolds makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * BILL REYNOLDS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL BILL REYNOLDS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Bill Reynolds, bill@goshawk.lanl.gov
 *
 * Reworked by: Simon P. Cooper, <scooper@vizlab.rutgers.edu>
 * Modifications: Harm Hanemaayer <hhanemaa@cs.ruu.nl>
 *
 */
/* $XConsortium: cir_im.c /main/8 1996/10/25 14:12:36 kaleb $ */


/*
 * Image read/write BLT functions.
 * WARNING: The scanline/total size byte padding for BitBLT engine functions
 * that involve system memory is EXTREMELY tricky. There are many different
 * cases. The databook can be misleading in places.
 */

/*
 * This file is compiled twice, once with CIRRUS_MMIO defined.
 */


#include "misc.h"
#include "xf86.h"
#include "X.h"
#include "Xos.h"
#include "Xmd.h"
#include "Xproto.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "cfb.h"
#include "cfbmskbits.h"
#include "cfb8bit.h"

#include "mergerop.h"
#include "vgaBank.h"
#include "compiler.h"
#include "vga.h"	/* For vgaBase. */

#include "cir_driver.h"
#ifdef CIRRUS_MMIO
#include "cirBlitMM.h"
#else
#include "cirBlitter.h"
#endif


#if 0	/* Replaced by assembler routine in cir_im.s */

static void transferdwords( unsigned char *base, unsigned char *srcp, int count ) {
	while (count >= 4) {
		unsigned long bits;
		unsigned long data;
		bits = *(unsigned *)srcp;
		data = byte_reversed[(unsigned char)bits] +
			(byte_reversed[(unsigned char)(bits >> 8)] << 8) + 
			(byte_reversed[(unsigned char)(bits >> 16)] << 16) +
			(byte_reversed[(unsigned char)(bits >> 24)] << 24);
		*(unsigned *)base = data;
		bits = *(unsigned *)(srcp + 4);
		data = byte_reversed[(unsigned char)bits] +
			(byte_reversed[(unsigned char)(bits >> 8)] << 8) + 
			(byte_reversed[(unsigned char)(bits >> 16)] << 16) +
			(byte_reversed[(unsigned char)(bits >> 24)] << 24);
		*(unsigned *)base = data;
		srcp += 8;
		count -= 2;
	}
	while (count > 0) {
		unsigned long bits;
		unsigned long data;
		bits = *(unsigned *)srcp;
		data = byte_reversed[(unsigned char)bits] +
			(byte_reversed[(unsigned char)(bits >> 8)] << 8) +
			(byte_reversed[(unsigned char)(bits >> 16)] << 16) +
			(byte_reversed[(unsigned char)(bits >> 24)] << 24);
		*(unsigned *)base = data;
		srcp += 4;
		count--;
	}
}

#endif



#ifdef CIRRUS_MMIO
#define _CirrusBLTImageWrite CirrusMMIOBLTImageWrite
#else
#define _CirrusBLTImageWrite CirrusBLTImageWrite
#endif 

/* This is actually currently disabled for the 543x (hence MMIO version is */
/* not used currently). */

void _CirrusBLTImageWrite(pdstBase, psrcBase, widthSrc, widthDst, x, y,
     x1, y1, w, h, xdir, ydir, alu, planemask)
     pointer pdstBase, psrcBase;	/* start of src bitmap */
     int widthSrc, widthDst;
     int x, y, x1, y1, w, h;	/* Src x,y; Dst x1,y1; Dst (w)idth,(h)eight */
     int xdir, ydir;
     int alu;
     unsigned long planemask;
{
  pointer psrc;
  unsigned int dstAddr;

  if ((planemask & 0xFF) == 0xFF)
    {
      int width, height;

      if (h > 1024) {
          /* Split into two. */
          _CirrusBLTImageWrite(pdstBase, psrcBase, widthSrc, widthDst, x, y,
              x1, y1, w, 1024, xdir, ydir, alu, planemask);
          _CirrusBLTImageWrite(pdstBase, psrcBase, widthSrc, widthDst, x, y +
              1024, x1, y1 + 1024, w, h - 1024, xdir, ydir, alu, planemask);
          return;
      }

      psrc = (unsigned char *)psrcBase + (y * widthSrc) + x;
      dstAddr = (y1 * widthDst) + x1;

      SETSRCADDR(0);	/* Required for 5430/40. */
      SETDESTADDR(dstAddr);
      SETDESTPITCH(widthDst);
      SETWIDTH(w);
      SETHEIGHT(h);
      /* Set the direction and source (System Memory) */
      SETBLTMODE(SYSTEMSRC);
      SETROP(cirrus_rop[alu]);
      STARTBLT();

      /*
       * We must transfer 4 bytes per blit line.  This is cautious code and I
       * do not read from outside of the pixmap... The 386/486 allows
       * unaligned memory acceses, and has little endian word ordering.  This
       * is used to our advantage when dealing with the 3 byte remainder.
       * Don't try this on your Sparc :-)
       */

      CirrusImageWriteTransfer(w, h, psrc, widthSrc, CIRRUSBASE());

      WAITUNTILFINISHED();

      #ifdef CIRRUS_MMIO
	  cirrusMMIOFlag = TRUE;
      #endif
    }
  else
    {
      vgaImageWrite(pdstBase, psrcBase, widthSrc, widthDst, x, y,
		    x1, y1, w, h, xdir, ydir, alu, planemask);
    }
  
}


/* The following function is not used on the 543x; it doesn't support */
/* video-to-system-memory BLTs. We assume having MMIO implies no ImageRead. */

#ifndef CIRRUS_MMIO

void
CirrusBLTImageRead (pdstBase, psrcBase, widthSrc, widthDst, x, y,
		 x1, y1, w, h, xdir, ydir, alu, planemask)
     pointer pdstBase, psrcBase;	/* start of src bitmap */
     int widthSrc, widthDst;
     int x, y, x1, y1, w, h;	/* Src x,y; Dst x1,y1; Dst (w)idth,(h)eight */
     int xdir, ydir;
     int alu;
     unsigned long planemask;
{
  pointer pdst;
  unsigned int srcAddr;

  if ((planemask & 0xFF) == 0xFF)
    {
      int width, height;

      if (h > 1024) {
          /* Split into two. */
          CirrusBLTImageRead(pdstBase, psrcBase, widthSrc, widthDst, x, y,
              x1, y1, w, 1024, xdir, ydir, alu, planemask);
          CirrusBLTImageRead(pdstBase, psrcBase, widthSrc, widthDst, x, y +
              1024, x1, y1 + 1024, w, h - 1024, xdir, ydir, alu, planemask);
          return;
      }

      pdst = (unsigned char *)pdstBase + (y1 * widthDst) + x1;
      srcAddr = (y * widthSrc) + x;

      SETSRCADDR(srcAddr);
      SETSRCPITCH(widthSrc);
      SETWIDTH(w);
      SETHEIGHT(h);
      /* Set the direction and destination (System Memory) */
      SETBLTMODE(SYSTEMDEST);
      SETROP(cirrus_rop[alu]);
      STARTBLT();

      /*
       * We must transfer 4 bytes per blit line.  This is cautious code and I
       * do not read from outside of the pixmap... The 386/486 allows
       * unaligned memory acceses, and has little endian word ordering.  This
       * is used to our advantage when dealing with the 3 byte remainder.
       * Don't try this on your Sparc :-)
       */
       
      /*
       * I doubt whether the reading of a multiple of 4 bytes *per
       * blit line* is right; I think we just need to read a multiple of
       * 4 bytes in total. We must not pad scanlines.
       * See 542x databook D8-6.
       *
       * The databook appears to have been wrong. We must pad scanlines.
       *
       */

      CirrusImageReadTransfer(w, h, pdst, widthDst, CIRRUSBASE());

      WAITUNTILFINISHED();
      #ifdef CIRRUS_MMIO
	  cirrusMMIOFlag = TRUE;
      #endif
    }
  else
    {
      vgaImageRead(pdstBase, psrcBase, widthSrc, widthDst, x, y,
		   x1, y1, w, h, xdir, ydir, alu, planemask);
    }
}

#endif	/* not defined(CIRRUS_MMIO) */


#ifdef CIRRUS_INCLUDE_COPYPLANE1TO8

/*
 * CirrusBLTWriteBitmap(x, y, w, h, src, bwidth, srcx, srcy, bg, fg,
 * destpitch)
 *
 * Cirrus two-color bitmap write function (used by CopyPlane1to8).
 * We use the BitBLT engine with color expansion system memory source,
 * reversing the per-byte bit order as we go.
 *
 * On the most chips, we draw the left edge seperately,
 * with the rest being done with an efficient transfer routine.
 * On the 5430/5429, we use the writemask register to do it all at once.
 *
 * bwidth is the 'pitch' of the bitmap in bytes.
 */

/*
 * Auxilliary function for the left edge; width is contained within one
 * byte of the source bitmap.
 * Assumes that some of the BLT registers are already initialized.
 * Static function, no need for seperate MMIO version name.
 */

static __inline__ void CirrusBLTWriteBitmapLeftEdge(x, y, w, h, src, bwidth,
srcx, srcy, bg, fg, destpitch)
	int x, y, w, h;
	unsigned char *src;
	int bwidth, srcx, srcy, bg, fg, destpitch;
{
	int destaddr, i, shift, bytecount;
	unsigned long dworddata;
	unsigned char *srcp;
	unsigned char *base;

	destaddr = y * destpitch + x;

	SETSRCADDR(0);
	SETSRCPITCH(0);
	SETDESTADDR(destaddr);
	SETWIDTH(w);
	SETHEIGHT(h);
	SETBLTMODE(SYSTEMSRC | COLOREXPAND);

	STARTBLT();

	base = CIRRUSBASE();	

	/* Calculate pointer to origin in bitmap. */
	srcp = bwidth * srcy + (srcx >> 3) + src;

	shift = (srcx & 7);

	/* We must make sure that a multiple of four bytes is transfered */
	/* in total. */

	bytecount = 0;
	dworddata = 0;

	for (i = 0; i < h; i++) {
		dworddata += (byte_reversed[*srcp] << shift)
			     << (bytecount * 8);
		srcp += bwidth;
		bytecount++;
		if (bytecount == 4) {
			bytecount = 0;
			*(unsigned long *)base = dworddata;
			dworddata = 0;
		}
	}
	if (bytecount)
		*(unsigned long *)base = dworddata;

	WAITUNTILFINISHED();
}

#ifdef CIRRUS_MMIO
#define _CirrusBLTWriteBitmap CirrusMMIOBLTWriteBitmap
#else
#define _CirrusBLTWriteBitmap CirrusBLTWriteBitmap
#endif

void _CirrusBLTWriteBitmap(x, y, w, h, src, bwidth, srcx, srcy, bg, fg, 
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

	if (h > 1024) {
		/* Split into two. */
		_CirrusBLTWriteBitmap(x, y, w, 1024, src, bwidth, srcx,
			srcy, bg, fg, destpitch);
		_CirrusBLTWriteBitmap(x, y + 1024, w, h - 1024, src, bwidth,
			srcx, srcy + 1024, bg, fg, destpitch);
		return;
	}

	/* Indicate that registers are changed via MMIO. */
	/* (This has no effect within this function). */
#ifdef CIRRUS_MMIO
	cirrusMMIOFlag = TRUE;
#endif

	SETROP(CROP_SRC);
	SETBACKGROUNDCOLOR(bg);
	SETFOREGROUNDCOLOR(fg);
	SETDESTPITCH(destpitch);

#if 0	/* Untested. */
	if (HAVEBLTWRITEMASK()) {
		/* On the 5430/29, use the writemask register which makes */
		/* it possible to read the bitmap aligned from the start at */
		/* all times. */
		skipleft = (srcx & 7);
		srcx = srcx & (~7);	/* Aligned. */
		x -= skipleft;
		w += skipleft;
		goto skipleftedge;
	}
#endif

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
		CirrusBLTWriteBitmapLeftEdge(x, y, skipleft, h, src, bwidth,
			srcx, srcy, bg, fg, destpitch);
		if (done)
			return;
	}

	x += skipleft;
	srcx += skipleft;
	w -= skipleft;

skipleftedge:

	/* Do the rest with a color-expand BitBLT transfer. */
	destaddr = y * destpitch + x;

	/* Calculate pointer to origin in bitmap. */
	srcp = bwidth * srcy + (srcx >> 3) + src;

	/* Number of bytes to be written for each bitmap scanline. */
	bytewidth = (w + 7) / 8;

	SETSRCADDR(0);
	SETSRCPITCH(0);
	SETDESTADDR(destaddr);
	SETWIDTH(w);
	SETHEIGHT(h);
#if 0	/* Untested. */
	if (HAVEBLTWRITEMASK())
		SETBLTWRITEMASK(skipleft);
#endif		
	SETBLTMODE(SYSTEMSRC | COLOREXPAND);

	STARTBLT();

	/* We must be very cautious here. For each bitmap line, we must pad
	 * to a byte boundary, but we may not transfer bytes; and for the
	 * total operation, we must transfer a multiple of 4 bytes. Failure
	 * to do this locks the machine.
	 */

	base = CIRRUSBASE();

#if 0	/* Disabled -- can overrun bitmap. */
	if (bytewidth == bwidth)
		CirrusAlignedBitmapTransfer(bytewidth * h / 4, srcp, base);
	else
#endif
		CirrusBitmapTransfer(bytewidth, h, bwidth, srcp, base);

	WAITUNTILFINISHED();

#if 0	/* Untested. */
	if (HAVEBLTWRITEMASK())
		SETBLTWRITEMASK(0x80);
#endif		
	SETBACKGROUNDCOLOR(0x0f);
	SETFOREGROUNDCOLOR(0);
}

#endif

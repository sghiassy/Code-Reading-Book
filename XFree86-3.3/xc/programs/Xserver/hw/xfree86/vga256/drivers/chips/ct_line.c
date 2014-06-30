/***********************************************************

Copyright (c) 1987  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

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

******************************************************************/
/*
 * Modified: Harm Hanemaayer (hhanemaa@cs.ruu.nl)
 *   Accelerate horizontal and vertical lines with MMIO BitBLT commands,
 *   and optimized linear framebuffer code if linear addressing is
 *   enabled. 8bpp only (vga256).
 */

/*
 * Modified: David Bateman (dbateman@ee.uts.edu.au)
 *   For use with Chips and Technology chipsets
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_line.c,v 3.2 1996/10/03 08:46:31 dawes Exp $ */

#include "xf86.h"
#include "vga256.h"
#include "vga.h"
#include "miline.h"
#include "compiler.h"

#include "ct_lline.h"
#include "ct_driver.h"

#ifdef CHIPS_HIQV
#include "ct_BltHiQV.h"
#else
#include "ct_BlitMM.h"
#endif

/*
 * The following define enables calls to the optimized linear framebuffer
 * sloped line code, which has not been tested for correctness.
 */
/* #define USE_LINEAR_SLOPED_LINES_CODE */

extern Bool vgaUseLinearAddressing;

/* single-pixel lines on a color frame buffer
 * 
 * NON-SLOPED LINES
 * horizontal lines are always drawn left to right; we have to
 * move the endpoints right by one after they're swapped.
 * horizontal lines will be confined to a single band of a
 * region.  the code finds that band (giving up if the lower
 * bound of the band is above the line we're drawing); then it
 * finds the first box in that band that contains part of the
 * line.  we clip the line to subsequent boxes in that band.
 * vertical lines are always drawn top to bottom (y-increasing.)
 * this requires adding one to the y-coordinate of each endpoint
 * after swapping.
 * 
 * SLOPED LINES
 * when clipping a sloped line, we bring the second point inside
 * the clipping box, rather than one beyond it, and then add 1 to
 * the length of the line before drawing it.  this lets us use
 * the same box for finding the outcodes for both endpoints.  since
 * the equation for clipping the second endpoint to an edge gives us
 * 1 beyond the edge, we then have to move the point towards the
 * first point by one step on the major axis.
 * eventually, there will be a diagram here to explain what's going
 * on.  the method uses Cohen-Sutherland outcodes to determine
 * outsideness, and a method similar to Pike's layers for doing the
 * actual clipping.
 * 
 */

void
#ifdef CHIPS_HIQV
#ifdef POLYSEGMENT
ctHiQVSegmentSS(pDrawable, pGC, nseg, pSeg)
    DrawablePtr pDrawable;
    GCPtr pGC;
    int nseg;
    register xSegment *pSeg;

#else
ctHiQVLineSS(pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable;
    GCPtr pGC;
    int mode;			       /* Origin or Previous */
    int npt;			       /* number of points */
    DDXPointPtr pptInit;

#endif
#else
#ifdef POLYSEGMENT
ctMMIOSegmentSS(pDrawable, pGC, nseg, pSeg)
    DrawablePtr pDrawable;
    GCPtr pGC;
    int nseg;
    register xSegment *pSeg;

#else
ctMMIOLineSS(pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable;
    GCPtr pGC;
    int mode;			       /* Origin or Previous */
    int npt;			       /* number of points */
    DDXPointPtr pptInit;

#endif
#endif
{
    int nboxInit;
    register int nbox;
    BoxPtr pboxInit;
    register BoxPtr pbox;

#ifndef POLYSEGMENT
    register DDXPointPtr ppt;	       /* pointer to list of translated points */

#endif

    unsigned int oc1;		       /* outcode of point 1 */
    unsigned int oc2;		       /* outcode of point 2 */

    unsigned long *addrl;	       /* address of destination pixmap */
    int nlwidth;		       /* width in longwords of destination pixmap */
    int xorg, yorg;		       /* origin of window */

    int adx;			       /* abs values of dx and dy */
    int ady;
    int signdx;			       /* sign of dx and dy */
    int signdy;
    int e, e1, e2;		       /* bresenham error and increments */
    int len;			       /* length of segment */
    int axis;			       /* major axis */

    /* a bunch of temporaries */
    int tmp;
    register int y1, y2;
    register int x1, x2;
    RegionPtr cclip;
    cfbPrivGCPtr devPriv;
    unsigned long xor, and;
    int alu;
    int adir, xdir, ydir;
    int destpitch;
    int octant;
    unsigned int bias = miGetZeroLineBias(pDrawable->pScreen);
    unsigned int op, mask;

#ifdef DEBUG
#ifdef POLYSEGMENT
    ErrorF("CHIPS:ctMMIOSegmentSS: ");
#else
    ErrorF("CHIPS:ctMMIOLineSS: ");
#endif
#endif

    switch (vgaBitsPerPixel) {
    case 8:
	mask = 0xFF;
	break;
    case 16:
	mask = 0xFFFF;
	break;
    case 24:
	mask = 0xFFFFFF;
	break;
    }

    if (!xf86VTSema || pDrawable->type != DRAWABLE_WINDOW ||
	(pGC->planemask & mask) != mask) {
	switch (vgaBitsPerPixel) {
	case 8:
#ifdef POLYSEGMENT
	    vga256SegmentSS(pDrawable, pGC, nseg, pSeg);
#else
	    vga256LineSS(pDrawable, pGC, mode, npt, pptInit);
#endif
	    break;
	case 16:
#ifdef POLYSEGMENT
	    cfb16SegmentSS(pDrawable, pGC, nseg, pSeg);
#else
	    cfb16LineSS(pDrawable, pGC, mode, npt, pptInit);
#endif
	    break;
	case 24:
#ifdef POLYSEGMENT
	    cfb24SegmentSS(pDrawable, pGC, nseg, pSeg);
#else
	    cfb24LineSS(pDrawable, pGC, mode, npt, pptInit);
#endif
	    break;
	}
	return;
    }
    addrl = 0;

    devPriv = cfbGetGCPrivate(pGC);
    cclip = devPriv->pCompositeClip;
    pboxInit = REGION_RECTS(cclip);
    nboxInit = REGION_NUM_RECTS(cclip);

    if (vgaBitsPerPixel == 8) {
	cfbGetLongWidthAndPointer(pDrawable, nlwidth, addrl)
    } else {
	nlwidth = vga256InfoRec.displayWidth / 4;
	addrl = (unsigned long *)VGABASE;       /* I'm not sure what these values should be yet */
	}
#ifdef DEBUG
    ErrorF(" %d,%d ", nlwidth, addrl);
#endif

    /*
     * This is a temporary hack to really use the non-bankchecking routines
     * in the fXF86 functions if linear addressing is enabled.
     */
    if (vgaUseLinearAddressing)
	addrl = (unsigned long *)((unsigned char *)vgaLinearBase +
	    (unsigned int)((unsigned char *)addrl - (unsigned int)VGABASE));

    BANK_FLAG(addrl);
    alu = devPriv->rop;
    xor = devPriv->xor;
    and = devPriv->and;
    xorg = pDrawable->x;
    yorg = pDrawable->y;
    destpitch = nlwidth * 4;

#ifdef DEBUG
    ErrorF("%d \n", destpitch);
#endif

    op = ctAluConv2[alu & 0xF] | ctTOP2BOTTOM | ctLEFT2RIGHT | ctPATSOLID
	| ctPATMONO;

/* Wait for BiTBLT to be free, then setup the blit parameters */
    ctBLTWAIT;
    ctSETSRCADDR(0x0);
    ctSETPITCH(0x0, destpitch * vgaBytesPerPixel);

    switch (vgaBitsPerPixel) {
    case 8:
	ctSETFGCOLOR8(pGC->fgPixel);
	ctSETBGCOLOR8(pGC->fgPixel);
	break;
    case 16:
	ctSETFGCOLOR16(pGC->fgPixel);
	ctSETBGCOLOR16(pGC->fgPixel);
	break;
    case 24:
#ifdef CHIPS_HIQV
	ctSETFGCOLOR24(pGC->fgPixel);
	ctSETBGCOLOR24(pGC->fgPixel);
#else
	{
	    /* The 6554x Blitter can only handle 8/16bpp fills directly,
	     * Though you can do a grey fill, by a little bit of magic
	     * with the 8bpp fill */
	    unsigned char red, green, blue;

	    red = (pGC->fgPixel) & 0xFF;
	    green = ((pGC->fgPixel) >> 8) & 0xFF;
	    blue = ((pGC->fgPixel) >> 16) & 0xFF;
	    if (red ^ green || green ^ blue || blue ^ red) {
#ifdef POLYSEGMENT
		cfb24SegmentSS(pDrawable, pGC, nseg, pSeg);
#else
		cfb24LineSS(pDrawable, pGC, mode, npt, pptInit);
#endif
		return;
	    } else {
		ctSETFGCOLOR8(pGC->fgPixel);
		ctSETBGCOLOR8(pGC->fgPixel);
	    }
	}
#endif
	break;
    }

#ifdef POLYSEGMENT
    while (nseg--)
#else
    ppt = pptInit;
    x2 = ppt->x + xorg;
    y2 = ppt->y + yorg;
    while (--npt)
#endif
    {
	nbox = nboxInit;
	pbox = pboxInit;

#ifdef POLYSEGMENT
	x1 = pSeg->x1 + xorg;
	y1 = pSeg->y1 + yorg;
	x2 = pSeg->x2 + xorg;
	y2 = pSeg->y2 + yorg;
	pSeg++;
#else
	x1 = x2;
	y1 = y2;
	++ppt;
	if (mode == CoordModePrevious) {
	    xorg = x1;
	    yorg = y1;
	}
	x2 = ppt->x + xorg;
	y2 = ppt->y + yorg;
#endif

	if (x1 == x2) {		       /* vertical line */
	    /* make the line go top to bottom of screen, keeping
	     * endpoint semantics
	     */
	    if (y1 > y2) {
		register int tmp;

		tmp = y2;
		y2 = y1 + 1;
		y1 = tmp + 1;
#ifdef POLYSEGMENT
		if (pGC->capStyle != CapNotLast)
		    y1--;
#endif
	    }
#ifdef POLYSEGMENT
	    else if (pGC->capStyle != CapNotLast)
		y2++;
#endif
	    /* get to first band that might contain part of line */
	    while ((nbox) && (pbox->y2 <= y1)) {
		pbox++;
		nbox--;
	    }

	    if (nbox) {
		/* stop when lower edge of box is beyond end of line */
		while ((nbox) && (y2 >= pbox->y1)) {
		    if ((x1 >= pbox->x1) && (x1 < pbox->x2)) {
			int y1t, y2t, height;

			/* this box has part of the line in it */
			y1t = max(y1, pbox->y1);
			y2t = min(y2, pbox->y2);
			height = y2t - y1t;
			if (y1t != y2t) {
			    if (height < 30) {
				int destaddr;

				destaddr = y1t * destpitch + x1;
				/*
				 * BitBLT is relatively slow with thin
				 * vertical areas for cirrus. We should
				 * check this for CHIPS before we assume
				 * the linear framebuffer is faster.
				 */
				ctBLTWAIT;
				ctSETROP(op);
#ifdef CHIPS_HIQV
				/* This operation involves colour expansion, */
				/* so being paranoid, set the monochrome     */
				/* control register to zero                  */
				ctSETMONOCTL(0);	
#endif
				ctSETDSTADDR(destaddr * vgaBytesPerPixel);
				ctSETHEIGHTWIDTHGO(height, vgaBytesPerPixel);
			    } else {
				if (alu == GXcopy && vgaUseLinearAddressing) {
				    unsigned char *destp;

				    destp = (unsigned char *)vgaLinearBase +
					(y1t * destpitch + x1) *
					vgaBytesPerPixel;
				    ctBLTWAIT;
				    switch (vgaBitsPerPixel) {
				    case 8:
					ctcfb8LinearFramebufferVerticalLine(
					    destp, xor, height, destpitch);
					break;
				    case 16:
					ctcfb16LinearFramebufferVerticalLine(
					    destp, xor, height, destpitch * 2);
					break;
				    case 24:
					ctcfb24LinearFramebufferVerticalLine(
					    destp, xor, height, destpitch * 3);
					break;
				    }
				} else {
				    ctBLTWAIT;
				    if (vgaBitsPerPixel == 8)
					fastvga256VertS(alu, and, xor,
					    addrl, nlwidth, x1, y1t, height);
				    else if (vgaBitsPerPixel == 16)
					cfb16VertS(alu, and, xor, addrl,
					    nlwidth * 2, x1, y1t, height);
				    else
					cfb24VertS(alu, and, xor, addrl,
					    nlwidth * 3, x1, y1t, height);
				}
			    }
			}
		    }
		    nbox--;
		    pbox++;
		}
	    }
#ifndef POLYSEGMENT
	    y2 = ppt->y + yorg;
#endif
	} else if (y1 == y2) {	       /* horizontal line */
	    /* force line from left to right, keeping
	     * endpoint semantics
	     */
	    if (x1 > x2) {
		register int tmp;

		tmp = x2;
		x2 = x1 + 1;
		x1 = tmp + 1;
#ifdef POLYSEGMENT
		if (pGC->capStyle != CapNotLast)
		    x1--;
#endif
	    }
#ifdef POLYSEGMENT
	    else if (pGC->capStyle != CapNotLast)
		x2++;
#endif

	    /* find the correct band */
	    while ((nbox) && (pbox->y2 <= y1)) {
		pbox++;
		nbox--;
	    }

	    /* try to draw the line, if we haven't gone beyond it */
	    if ((nbox) && (pbox->y1 <= y1)) {
		/* when we leave this band, we're done */
		tmp = pbox->y1;
		while ((nbox) && (pbox->y1 == tmp)) {
		    int x1t, x2t;

		    if (pbox->x2 <= x1) {
			/* skip boxes until one might contain start point */
			nbox--;
			pbox++;
			continue;
		    }
		    /* stop if left of box is beyond right of line */
		    if (pbox->x1 >= x2) {
			nbox = 0;
			break;
		    }
		    x1t = max(x1, pbox->x1);
		    x2t = min(x2, pbox->x2);
		    if (x1t != x2t) {
			int destaddr, width;

			destaddr = y1 * destpitch + x1t;
			width = x2t - x1t;
			ctBLTWAIT;
			ctSETROP(op);
#ifdef CHIPS_HIQV
			ctSETMONOCTL(0);	
#endif
			ctSETDSTADDR(destaddr * vgaBytesPerPixel);
			ctSETHEIGHTWIDTHGO(1, width * vgaBytesPerPixel);
		    }
		    nbox--;
		    pbox++;
		}
	    }
#ifndef POLYSEGMENT
	    x2 = ppt->x + xorg;
#endif
	} else {		       /* sloped line */
	    CalcLineDeltas(x1, y1, x2, y2, adx, ady, signdx, signdy, 1, 1, octant);
	    if (adx > ady) {
		axis = X_AXIS;
	        e1 = ady << 1;
		e2 = e1 - (adx << 1);
		e = e1 - adx;
	    } else {
		axis = Y_AXIS;
		e1 = adx << 1;
		e2 = e1 - (ady << 1);
		e = e1 - ady;
		SetYMajorOctant(octant);
	    }

	    FIXUP_ERROR(e, octant, bias);

	    /* we have bresenham parameters and two points.
	     * all we have to do now is clip and draw.
	     */

	    while (nbox--) {
		oc1 = 0;
		oc2 = 0;
		OUTCODES(oc1, x1, y1, pbox);
		OUTCODES(oc2, x2, y2, pbox);
		if ((oc1 | oc2) == 0) {
		    if (axis == X_AXIS)
			len = adx;
		    else
			len = ady;
#ifdef POLYSEGMENT
		    if (pGC->capStyle != CapNotLast)
			len++;
#endif
#ifdef USE_LINEAR_SLOPED_LINES_CODE
		    if (vgaUseLinearAddressing && alu == GXcopy) {
			unsigned char *destp;
			int pitch;

			destp = (y1 * destpitch + x1) * vgaBytesPerPixel +
			    (unsigned char *)vgaLinearBase;
			pitch = destpitch * vgaBytesPerPixel;
			if (signdy == -1)
			    pitch = -pitch;
			ctBLTWAIT;
			if (axis == X_AXIS) {
			    if (signdx > 0) {
				switch (vgaBitsPerPixel) {
				case 8:
				    ctcfb8LinearFramebufferSlopedLineRight(
					(unsigned char *)destp, xor, e, e1,
					e2, len, pitch);
				    break;
				case 16:
				    ctcfb16LinearFramebufferSlopedLineRight(
					destp, xor, e, e1, e2, len, pitch);
				    break;
				case 24:
				    ctcfb24LinearFramebufferSlopedLineRight(
					destp, xor, e, e1, e2, len, pitch);
				    break;
				}
			    } else {
				switch (vgaBitsPerPixel) {
				case 8:
				    ctcfb8LinearFramebufferSlopedLineLeft(
					destp, xor, e, e1, e2, len, pitch);
				    break;
				case 16:
				    ctcfb16LinearFramebufferSlopedLineLeft(
					destp, xor, e, e1, e2, len, pitch);
				    break;
				case 24:
				    ctcfb24LinearFramebufferSlopedLineLeft(
					destp, xor, e, e1, e2, len, pitch);
				    break;
				}
			    }
			} else {       /* axis == Y_AXIS */
			    if (signdx > 0) {
				switch (vgaBitsPerPixel) {
				case 8:
				    ctcfb8LinearFramebufferSlopedLineVerticalRight(
					destp, xor, e, e1, e2, len, pitch);
				    break;
				case 16:
				    ctcfb16LinearFramebufferSlopedLineVerticalRight(
					destp, xor, e, e1, e2, len, pitch);
				    break;
				case 24:
				    ctcfb24LinearFramebufferSlopedLineVerticalRight(
					destp, xor, e, e1, e2, len, pitch);
				    break;
				}
			    } else {
				switch (vgaBitsPerPixel) {
				case 8:
				    ctcfb8LinearFramebufferSlopedLineVerticalLeft(
					destp, xor, e, e1, e2, len, pitch);
				    break;
				case 16:
				    ctcfb16LinearFramebufferSlopedLineVerticalLeft(
					destp, xor, e, e1, e2, len, pitch);
				    break;
				case 24:
				    ctcfb24LinearFramebufferSlopedLineVerticalLeft(
					destp, xor, e, e1, e2, len, pitch);
				    break;
				}
			    }
			}
		    } else {
#endif
			ctBLTWAIT;
			if (vgaBitsPerPixel == 8)
			    fastvga256BresS(alu, and, xor, addrl,
				nlwidth, signdx, signdy, axis, x1,
				y1, e, e1, e2, len);
			else if (vgaBitsPerPixel == 16)
			    cfb16BresS(alu, and, xor, addrl,
				nlwidth * 2, signdx, signdy, axis, x1,
				y1, e, e1, e2, len);
			else
			    cfb24BresS(alu, and, xor, addrl,
				nlwidth * 3, signdx, signdy, axis, x1,
				y1, e, e1, e2, len);
#ifdef USE_LINEAR_SLOPED_LINES_CODE
		    }
#endif
		    break;
		} else if (oc1 & oc2) {
		    pbox++;
		} else {
		    int new_x1 = x1, new_y1 = y1, new_x2 = x2, new_y2 = y2;
		    int clip1 = 0, clip2 = 0;
		    int clipdx, clipdy;
		    int err;

		    if (miZeroClipLine(pbox->x1, pbox->y1, pbox->x2 - 1,
				       pbox->y2 - 1,
				       &new_x1, &new_y1, &new_x2, &new_y2,
				       adx, ady, 
				       &clip1, &clip2, 
				       octant, bias, oc1, oc2) == -1) {
			pbox++;
			continue;
		    }
		    if (axis == X_AXIS)
			len = abs(new_x2 - new_x1);
		    else
			len = abs(new_y2 - new_y1);
#ifdef POLYSEGMENT
		    if (clip2 != 0 || pGC->capStyle != CapNotLast)
			len++;
#else
		    len += (clip2 != 0);
#endif
		    if (len) {
			/* unwind bresenham error term to first point */
			if (clip1) {
			    clipdx = abs(new_x1 - x1);
			    clipdy = abs(new_y1 - y1);
			    if (axis == X_AXIS)
				err = e + ((clipdy * e2) + ((clipdx - clipdy) * e1));
			    else
				err = e + ((clipdx * e2) + ((clipdy - clipdx) * e1));
			} else
			    err = e;
#ifdef USE_LINEAR_SLOPED_LINES_CODE
			if (vgaUseLinearAddressing && alu == GXcopy) {
			    unsigned char *destp;
			    int pitch;

			    destp = (y1 * destpitch + x1) * vgaBytesPerPixel +
				(unsigned char *)vgaLinearBase;
			    pitch = destpitch * vgaBytesPerPixel;
			    if (signdy == -1)
				pitch = -pitch;

			    if (axis == X_AXIS) {
				if (signdx > 0) {
				    switch (vgaBitsPerPixel) {
				    case 8:
					ctcfb8LinearFramebufferSlopedLineRight(
					    destp, xor, e, e1, e2, len, pitch);
					break;
				    case 16:
					ctcfb16LinearFramebufferSlopedLineRight(
					    destp, xor, e, e1, e2, len, pitch);
					break;
				    case 24:
					ctcfb24LinearFramebufferSlopedLineRight(
					    destp, xor, e, e1, e2, len, pitch);
					break;
				    }
				} else {
				    switch (vgaBitsPerPixel) {
				    case 8:
					ctcfb8LinearFramebufferSlopedLineLeft(
					    destp, xor, e, e1, e2, len, pitch);
					break;
				    case 16:
					ctcfb16LinearFramebufferSlopedLineLeft(
					    destp, xor, e, e1, e2, len, pitch);
					break;
				    case 24:
					ctcfb24LinearFramebufferSlopedLineLeft(
					    destp, xor, e, e1, e2, len, pitch);
					break;
				    }
				}
			    } else {   /* axis == Y_AXIS */
				if (signdx > 0) {
				    switch (vgaBitsPerPixel) {
				    case 8:
					ctcfb8LinearFramebufferSlopedLineVerticalRight(
					    destp, xor, e, e1, e2, len, pitch);
					break;
				    case 16:
					ctcfb16LinearFramebufferSlopedLineVerticalRight(
					    destp, xor, e, e1, e2, len, pitch);
					break;
				    case 24:
					ctcfb24LinearFramebufferSlopedLineVerticalRight(
					    destp, xor, e, e1, e2, len, pitch);
					break;
				    }
				} else {
				    switch (vgaBitsPerPixel) {
				    case 8:
					ctcfb8LinearFramebufferSlopedLineVerticalLeft(
					    destp, xor, e, e1, e2, len, pitch);
					break;
				    case 16:
					ctcfb16LinearFramebufferSlopedLineVerticalLeft(
					    destp, xor, e, e1, e2, len, pitch);
					break;
				    case 24:
					ctcfb24LinearFramebufferSlopedLineVerticalLeft(
					    destp, xor, e, e1, e2, len, pitch);
					break;
				    }
				}
			    }
			} else {
#endif

			    ctBLTWAIT;
			    if (vgaBitsPerPixel == 8)
				fastvga256BresS(alu, and, xor, addrl,
				    nlwidth, signdx, signdy, axis, x1,
				    y1, e, e1, e2, len);
			    else if (vgaBitsPerPixel == 16)
				cfb16BresS(alu, and, xor, addrl,
				    nlwidth * 2, signdx, signdy, axis, x1,
				    y1, e, e1, e2, len);
			    else
				cfb24BresS(alu, and, xor, addrl,
				    nlwidth * 3, signdx, signdy, axis, x1,
				    y1, e, e1, e2, len);
#ifdef USE_LINEAR_SLOPED_LINES_CODE
			}
#endif
		    }
		    pbox++;
		}
	    }			       /* while (nbox--) */
	}			       /* sloped line */
    }				       /* while (nline--) */

#ifndef POLYSEGMENT

    /* paint the last point if the end style isn't CapNotLast.
     * (Assume that a projecting, butt, or round cap that is one
     * pixel wide is the same as the single pixel of the endpoint.)
     */

    if ((pGC->capStyle != CapNotLast) &&
	((ppt->x + xorg != pptInit->x + pDrawable->x) ||
	    (ppt->y + yorg != pptInit->y + pDrawable->y) ||
	    (ppt == pptInit + 1))) {
	ctBLTWAIT;
	nbox = nboxInit;
	pbox = pboxInit;
	while (nbox--) {
	    if ((x2 >= pbox->x1) &&
		(y2 >= pbox->y1) &&
		(x2 < pbox->x2) &&
		(y2 < pbox->y2)) {
		unsigned long mask;
		unsigned long scrbits;

		if (vgaUseLinearAddressing && alu == GXcopy &&
			vgaBitsPerPixel == 8)
		    *((unsigned char *)vgaLinearBase + y2 * destpitch + x2) =
			xor;
		else {
		    ctBLTWAIT;
		    ctSETROP(op);
#ifdef CHIPS_HIQV
		    ctSETMONOCTL(0);	
#endif
		    ctSETDSTADDR((y2 * destpitch + x2) * vgaBytesPerPixel);
		    ctSETHEIGHTWIDTHGO(1, (vgaBitsPerPixel >> 3));
		}
		break;
	    } else
		pbox++;
	}
    }
#endif
    ctBLTWAIT;
}

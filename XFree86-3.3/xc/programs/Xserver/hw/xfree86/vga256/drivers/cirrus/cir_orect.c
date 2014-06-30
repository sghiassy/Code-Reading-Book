/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_orect.c,v 3.8 1996/12/23 06:56:52 dawes Exp $ */
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.
Copyright 1993,1994 by Kevin E. Martin, Chapel Hill, North Carolina.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL AND KEVIN E. MARTIN AND RICKARD E. FAITH AND CRAIG E. GROESCHEL
DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL DIGITAL OR
KEVIN E. MARTIN OR RICKARD E. FAITH OR CRAIG E. GROESCHEL BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)
Modified for Cirrus by Harm Hanemaayer (hhanemaa@cs.ruu.nl)

******************************************************************/
/* $XConsortium: cir_orect.c /main/7 1996/10/25 10:32:10 kaleb $ */

#include "xf86.h"
#include "vga256.h"
#include "vga.h"
#include "linearline.h"
#include "cir_driver.h"
#include "cirBlitMM.h"		/* MMIO BitBLT commands */


/*
 * Accelerated rectangles with Cirrus BitBLT/linear addressing.
 * Horizontal edges are done with BitBLT fills, vertical edges
 * are done with framebuffer writes (if linear addressing is
 * enabled) since they are much faster than a thin BitBLT fill
 * because of the scanline-to-scanline overhead of the BitBLT
 * engine. Can be compiled seperately for 8, 16 and 32bpp.
 *
 * The BitBLT foreground/VGA set/reset register does not interfere with
 * plain framebuffer writes on the MMIO chips (5429, 543x).
 * Note that for heights > 1024, BLTs must be split into two because
 * of the stupid 1024 line restriction of the 5429/5430/4 (even though
 * the 543x databook says 2048).
 *
 */

#if PSZ == 8
#define CirrusPolyRectangle Cirrus8PolyRectangle
#endif
#if PSZ == 16
#define CirrusPolyRectangle Cirrus16PolyRectangle
#endif
#if PSZ == 32
#define CirrusPolyRectangle Cirrus32PolyRectangle
#endif

void
CirrusPolyRectangle (pDrawable, pGC, nRectsInit, pRectsInit)
    DrawablePtr  pDrawable;	/* Pointer to current drawable */
    GCPtr        pGC;    	/* Pointer to current GC */
    int	         nRectsInit; 	/* Number of rectangles to display */
    xRectangle  *pRectsInit;	/* List of rectangles to display */
{
    int         nClipRects;     /* number of clip rectangles */
    BoxPtr      pClipRects;     /* points to the list of clip rects */
    int         xOrigin;        /* Drawables x origin */
    int         yOrigin;        /* Drawables x origin */
    cfbPrivGC  *pGCPriv;        /* pointer to private GC */
    xRectangle *pRect;          /* list of rects */
    int         nRects;         /* running count of number of rects */
    int         origX1, origY1; /* original rectangle's U/L corner */
    int         origX2, origY2; /* original rectangle's L/R corner */
    int         clippedX1;      /* clipped rectangle's left x */
    int         clippedY1;      /* clipped rectangle's top y */
    int         clippedX2;      /* clipped rectangle's right x */
    int         clippedY2;      /* clipped rectangle's bottom y */
    int         clipXMin;       /* upper left corner of clip rect */
    int         clipYMin;       /* upper left corner of clip rect */
    int         clipXMax;       /* lower right corner of clip rect */
    int         clipYMax;       /* lower right corner of clip rect */
    int         width, height;  /* width and height of rect */
    int		destpitch;	/* screen scanline offset in bytes */
    int		busy;		/* temporary for BLT status */
    unsigned char *base;	/* Base address of screen framebuffer */


    if ((pGC->lineStyle != LineSolid) || (pGC->fillStyle != FillSolid) ||
	(pGC->lineWidth != 0) || (!xf86VTSema) ||
	((pGC->planemask & PMSK) != PMSK) ||
	(pDrawable->type != DRAWABLE_WINDOW)) 
    {
	miPolyRectangle(pDrawable, pGC, nRectsInit, pRectsInit);
	return;
    }

    pGCPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr);

    xOrigin = pDrawable->x;
    yOrigin = pDrawable->y;

#if PSZ == 8
    SETBLTMODE(FORWARDS | COLOREXPAND | PATTERNCOPY);
#endif
#if PSZ == 16     
    SETBLTMODE(FORWARDS | COLOREXPAND | PATTERNCOPY | PIXELWIDTH16);
#endif
#if PSZ == 32
    SETBLTMODE(FORWARDS | COLOREXPAND | PATTERNCOPY | PIXELWIDTH32);
#endif
    SETROP(cirrus_rop[pGC->alu]);
    destpitch = vga256InfoRec.displayWidth * (PSZ / 8);
    SETDESTPITCH(destpitch);
#if PSZ == 8    
    SETFOREGROUNDCOLOR(pGC->fgPixel);
    SETBACKGROUNDCOLOR(pGC->fgPixel);
#endif
#if PSZ == 16
    SETFOREGROUNDCOLOR16(pGC->fgPixel);
    SETBACKGROUNDCOLOR16(pGC->fgPixel);
#endif
#if PSZ == 32
    SETFOREGROUNDCOLOR32(pGC->fgPixel);
    SETBACKGROUNDCOLOR32(pGC->fgPixel);
#endif
    SETSRCADDR(0);

    base = vgaLinearBase;	/* Assume linear addressing. */

    for ( nClipRects = REGION_NUM_RECTS(pGCPriv->pCompositeClip),
          pClipRects = REGION_RECTS(pGCPriv->pCompositeClip);
	  nClipRects > 0; 
	  nClipRects--, pClipRects++ )
    {
        clipYMin = pClipRects->y1;
        clipYMax = pClipRects->y2 - 1;
        clipXMin = pClipRects->x1;
        clipXMax = pClipRects->x2 - 1;

	for (pRect = pRectsInit, nRects = nRectsInit; 
	     nRects > 0; 
	     nRects--, pRect++ )
        {
	    /* translate rectangle data over to the drawable */
            origX1 = pRect->x + xOrigin; 
	    origY1 = pRect->y + yOrigin;
            origX2 = origX1 + pRect->width; 
	    origY2 = origY1 + pRect->height; 

	    /* reject entire rectangle if completely outside clip rect */
	    if ((origX1 > clipXMax) || (origX2 < clipXMin) ||
		(origY1 > clipYMax) || (origY2 < clipYMin))
            {
	        continue;
            }

	    /* clip the rectangle */
	    clippedX1 = max (origX1, clipXMin);
	    clippedX2 = min (origX2, clipXMax);
	    clippedY1 = max (origY1, clipYMin);
	    clippedY2 = min (origY2, clipYMax);

	    /*
	     * IDEA: It may be better to do the vertical lines last
	     * since they are slower so it is a bigger win to have
	     * them drawn concurrently with the clipping checks for
	     * the next rectangle.
	     */

	    width = clippedX2 - clippedX1 + 1;

	    /* use bitblt to draw top edge if not clipped */
	    if (origY1 >= clipYMin)
	    {
	    	int destaddr;
	    	destaddr = clippedY1 * destpitch + clippedX1 * (PSZ / 8);
		do { BLTBUSY(busy); } while (busy);
		SETDESTADDR(destaddr);
		SETWIDTH(width * (PSZ / 8));
		SETHEIGHT(1);
		STARTBLT();
	    }

	    /* Don't overwrite the corners. */
	    clippedY1++;
	    clippedY2--;

	    height = clippedY2 - clippedY1 + 1;

	    /* draw vertical edges using lines if not clipped out */
            if (origX1 >= clipXMin) {
            	if (pGC->alu == GXcopy && cirrusUseLinear) {
		    unsigned char *destp;
	    	    int fg;
	    	    if ((origX2 <= clipXMax) && (origX2 != origX1)) {
	    	    	/* Do both vertical edges simultaneously. */
	    		destp = clippedY1 * destpitch + clippedX1 * (PSZ / 8)
	    		    + base;
	    		fg = pGC->fgPixel;
			do { BLTBUSY(busy); } while (busy);
			LinearFramebufferDualVerticalLine(destp, fg,
			    height, (clippedX2 - clippedX1) * (PSZ / 8),
			    destpitch);
			goto drawbottomedge;
		    }

	    	    destp = clippedY1 * destpitch + clippedX1 * (PSZ / 8)
	    	        + base;
	    	    fg = pGC->fgPixel;
		    do { BLTBUSY(busy); } while (busy);
		    LinearFramebufferVerticalLine(destp, fg, height,
		        destpitch);
	    	}
	    	else {
		    int destaddr;
		    destaddr = clippedY1 * destpitch + clippedX1 * (PSZ / 8);
		    if (height > 1024) {
			    do { BLTBUSY(busy); } while (busy);
			    SETDESTADDR(destaddr);
		            SETWIDTH(PSZ / 8);
			    SETHEIGHT(1024);
			    STARTBLT();
			    destaddr += destpitch * 1024;
			    do { BLTBUSY(busy); } while (busy);
			    SETDESTADDR(destaddr);
		            SETWIDTH(PSZ / 8);
			    SETHEIGHT(height - 1024);
			    STARTBLT();
		    }
		    else {
			    do { BLTBUSY(busy); } while (busy);
			    SETDESTADDR(destaddr);
		            SETWIDTH(PSZ / 8);
			    SETHEIGHT(height);
			    STARTBLT();
		    }
		}
	    }

            if ((origX2 <= clipXMax) && (origX2 != origX1))
	    {
            	if (pGC->alu == GXcopy && cirrusUseLinear) {
		    unsigned char *destp;
	    	    int fg;
	    	    destp = clippedY1 * destpitch + clippedX2 * (PSZ / 8)
	    	        + base;
	    	    fg = pGC->fgPixel;
		    do { BLTBUSY(busy); } while (busy);
		    LinearFramebufferVerticalLine(destp, fg, height,
		        destpitch);
	    	}
	    	else {
		    int destaddr;
		    destaddr = clippedY1 * destpitch + clippedX2 * (PSZ / 8);
		    if (height > 1024) {
			    do { BLTBUSY(busy); } while (busy);
			    SETDESTADDR(destaddr);
		            SETWIDTH(PSZ / 8);
			    SETHEIGHT(1024);
			    STARTBLT();
			    destaddr += destpitch * 1024;
			    do { BLTBUSY(busy); } while (busy);
			    SETDESTADDR(destaddr);
		            SETWIDTH(PSZ / 8);
			    SETHEIGHT(height - 1024);
			    STARTBLT();
		    }
		    else {
			    do { BLTBUSY(busy); } while (busy);
			    SETDESTADDR(destaddr);
		            SETWIDTH(PSZ / 8);
			    SETHEIGHT(height);
			    STARTBLT();
		    }
		}
	    }

drawbottomedge:

	    /* use bitblt to draw bottom edge if not clipped */
	    if ((origY2 <= clipYMax) && (origY1 != origY2))
	    {
	    	int destaddr;
		/*
		 * Corrected for decreased clippedY2.
		 */
	    	destaddr = (clippedY2 + 1) * destpitch + clippedX1 * (PSZ / 8);
		do { BLTBUSY(busy); } while (busy);
		SETDESTADDR(destaddr);
		SETWIDTH(width * (PSZ / 8));
		SETHEIGHT(1);
		STARTBLT();
	    }
	}
    }
    do { BLTBUSY(busy); } while (busy);
} 

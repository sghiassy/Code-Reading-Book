/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86orect.c,v 3.1 1997/01/12 10:48:13 dawes Exp $ */

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
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "miline.h"
/* PSZ doesn't matter. */
#define PSZ 8
#include "cfb.h"

#include "xf86.h"
#include "xf86xaa.h"
#include "xf86gcmap.h"
#include "xf86local.h"

/*
 * Rectangles, based on mach64orect.c.
 */

#define VLINE_FRAMEBUFFER 1
#define VLINE_BRESENHAMLINE 2
#define VLINE_TWOPOINTLINE 3

void
xf86PolyRectangleSolidZeroWidth(pDrawable, pGC, nRectsInit, pRectsInit)
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
    int		usevline;
    unsigned long *addrl;
    int		nlwidth;

    pGCPriv = cfbGetGCPrivate(pGC);

    xOrigin = pDrawable->x;
    yOrigin = pDrawable->y;

    /* This should also set up for general line drawing. */
    xf86AccelInfoRec.SetupForFillRectSolid(pGC->fgPixel, pGC->alu,
        pGC->planemask);

    {
        /*
         * Determine what function to use for vertical lines.
         * Detect special case where a framebuffer function might be
         * preferable for vertical lines.
         */
        int mask;
        mask = (1 << pDrawable->depth) - 1;
        if (xf86AccelInfoRec.SubsequentBresenhamLine)
            usevline = VLINE_BRESENHAMLINE;
        else
        if (xf86AccelInfoRec.SubsequentTwoPointLine)
            usevline = VLINE_TWOPOINTLINE;
        else
        if ((pGC->alu == GXcopy) && (pGC->planemask & mask) == mask) {
            usevline = VLINE_FRAMEBUFFER;
            xf86AccelInfoRec.xf86GetLongWidthAndPointer(pDrawable,
                &nlwidth, &addrl);
        }
        else
            usevline = 0;
    }

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
            int length;
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

	    width = clippedX2 - clippedX1 + 1;

	    /* use bitblt to draw top edge if not clipped */
	    if (origY1 >= clipYMin)
	    {
	        xf86AccelInfoRec.SubsequentFillRectSolid(
	            clippedX1, clippedY1, width, 1);

		/* don't overwrite corner */
		clippedY1++;
	    }

	    /* use bitblt to draw bottom edge if not clipped */
	    if ((origY2 <= clipYMax) && (origY1 != origY2))
	    {
	        xf86AccelInfoRec.SubsequentFillRectSolid(
	            clippedX1, clippedY2, width, 1);

		/* don't overwrite corner */
		clippedY2--; 
	    }

	    if (clippedY2 < clippedY1)
	        continue;

	    length = clippedY2 - clippedY1 + 1;

	    /* draw vertical edges using lines if not clipped out */
            if (origX1 >= clipXMin)
	    {
	        if (usevline == VLINE_TWOPOINTLINE)
	            xf86AccelInfoRec.SubsequentTwoPointLine(
	                clippedX1, clippedY1, clippedX1, clippedY2, 0);
	        else
	        if (usevline == VLINE_BRESENHAMLINE)
	            xf86AccelInfoRec.SubsequentBresenhamLine(
	                clippedX1, clippedY1,
	                YMAJOR,		/* octant */
	                - (clippedY2 - clippedY1),		 /* ERR */
	                0,					 /* INC */
	                - (2 * (clippedY2 - clippedY1)), 	 /* DEC */
	                length);
	        else
		/*
		 * Vertical lines via solid filled rectangles are pretty
		 * slow on many chips. For GXcopy, a framebuffer function
		 * might be preferable.
		 * It might be better do this only if the length is
		 * greater than 30 or so.
		 */
		if ((usevline == VLINE_FRAMEBUFFER) && length > 30) {
		    if (!(xf86AccelInfoRec.Flags & COP_FRAMEBUFFER_CONCURRENCY)
		    && (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS))
                        xf86AccelInfoRec.Sync();
		    xf86AccelInfoRec.VerticalLineGXcopyFallBack(
		        pGC->alu, 0, pGC->fgPixel, addrl, nlwidth,
		        clippedX1, clippedY1, length);
		    /*
		     * Maybe in some cases a chip needs a new SetUp here.
		     */
		}
		else
	            xf86AccelInfoRec.SubsequentFillRectSolid(
	                clippedX1, clippedY1, 1, length);
	    }

            if ((origX2 <= clipXMax) && (origX2 != origX1))
	    {
	        if (usevline == VLINE_BRESENHAMLINE)
	            xf86AccelInfoRec.SubsequentBresenhamLine(
	                clippedX2, clippedY1,
	                YMAJOR,		/* octant */
	                - (clippedY2 - clippedY1),		 /* ERR */
	                0,					 /* INC */
	                - (2 * (clippedY2 - clippedY1)), 	 /* DEC */
	                length);
	        else
	        if (usevline == VLINE_TWOPOINTLINE)
	            xf86AccelInfoRec.SubsequentTwoPointLine(
	                clippedX2, clippedY1, clippedX2, clippedY2, 0);
	        else
		if ((usevline == VLINE_FRAMEBUFFER) && length > 30) {
		    if (!(xf86AccelInfoRec.Flags & COP_FRAMEBUFFER_CONCURRENCY)
		    && (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS))
                        xf86AccelInfoRec.Sync();
		    xf86AccelInfoRec.VerticalLineGXcopyFallBack(
		        pGC->alu, 0, pGC->fgPixel, addrl, nlwidth,
		        clippedX2, clippedY1, length);
		}
		else
	            xf86AccelInfoRec.SubsequentFillRectSolid(
	                clippedX2, clippedY1, 1, length);
	    }
	}
    }

    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
        xf86AccelInfoRec.Sync();
} 

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64orect.c,v 3.4 1996/12/23 06:39:23 dawes Exp $ */
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

******************************************************************/
/* $XConsortium: mach64orect.c /main/4 1996/02/21 17:28:59 kaleb $ */

#include "X.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "cfb.h"
#include "mach64.h"

void
mach64PolyRectangle (pDrawable, pGC, nRectsInit, pRectsInit)
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


    if ((pGC->lineStyle != LineSolid) || (pGC->fillStyle != FillSolid) ||
	(pGC->lineWidth != 0) || (!xf86VTSema)) 
    {
	miPolyRectangle(pDrawable, pGC, nRectsInit, pRectsInit);
	return;
    }

    pGCPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr);

    xOrigin = pDrawable->x;
    yOrigin = pDrawable->y;

    WaitQueue(4);
    regw(DP_FRGD_CLR, pGC->fgPixel);
    regw(DP_MIX, (mach64alu[pGC->alu] << 16) | MIX_DST);
    regw(DP_WRITE_MASK, pGC->planemask);
    regw(DP_SRC, FRGD_SRC_FRGD_CLR);

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

	    width = clippedX2 - clippedX1 + 1;

	    /* use bitblt to draw top edge if not clipped */
	    if (origY1 >= clipYMin)
	    {
                WaitQueue(3);
                regw(DST_CNTL, (DST_X_LEFT_TO_RIGHT | 
                                DST_Y_TOP_TO_BOTTOM));
                regw(DST_Y_X, ((clippedX1 << 16) | clippedY1));
                regw(DST_HEIGHT_WIDTH, ((width << 16) | 1));

		/* don't overwrite corner */
		clippedY1++;
	    }

	    /* use bitblt to draw bottom edge if not clipped */
	    if ((origY2 <= clipYMax) && (origY1 != origY2))
	    {
                WaitQueue(3);
                regw(DST_CNTL, (DST_X_LEFT_TO_RIGHT | 
                                DST_Y_TOP_TO_BOTTOM));
                regw(DST_Y_X, ((clippedX1 << 16) | clippedY2));
                regw(DST_HEIGHT_WIDTH, ((width << 16) | 1));

		/* don't overwrite corner */
		clippedY2--; 
	    }

	    /* draw vertical edges using lines if not clipped out */
            if (origX1 >= clipXMin)
	    {
		WaitQueue(6);
                regw(DST_Y_X, (clippedX1 << 16) | clippedY1);
                regw(DST_CNTL, 0x00000007);
                regw(DST_BRES_ERR, (-(clippedY2 - clippedY1)));
                regw(DST_BRES_INC, 0x00000000);
                regw(DST_BRES_DEC, (0x3ffff - (2 * (clippedY2 - clippedY1))));
                regw(DST_BRES_LNTH, ((clippedY2 - clippedY1) + 2));
	    }

            if ((origX2 <= clipXMax) && (origX2 != origX1))
	    {
		WaitQueue(6);
                regw(DST_Y_X, (clippedX2 << 16) | clippedY1);
                regw(DST_CNTL, 0x00000007);
                regw(DST_BRES_ERR, (-(clippedY2 - clippedY1)));
                regw(DST_BRES_INC, 0x00000000);
                regw(DST_BRES_DEC, (0x3ffff - (2 * (clippedY2 - clippedY1))));
                regw(DST_BRES_LNTH, ((clippedY2 - clippedY1) + 2));
	    }
	}
    }
    WaitQueue(1);
    regw(DST_CNTL, (DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM));

    WaitIdleEmpty();
} 

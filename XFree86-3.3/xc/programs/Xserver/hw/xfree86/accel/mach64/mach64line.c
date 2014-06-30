/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64line.c,v 3.3 1996/12/23 06:39:22 dawes Exp $ */
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

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)

******************************************************************/
/* $XConsortium: mach64line.c /main/3 1996/02/21 17:28:55 kaleb $ */

#include "X.h"

#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "mistruct.h"
#include "miline.h"

#include "cfb.h"
#include "cfb16.h"
#include "cfb32.h"
#include "cfbmskbits.h"
#include "mach64.h"

/* single-pixel lines on a color frame buffer
 * 
 *    NON-SLOPED LINES
 *    horizontal lines are always drawn left to right; we have to
 * move the endpoints right by one after they're swapped.
 *    horizontal lines will be confined to a single band of a
 * region.  the code finds that band (giving up if the lower
 * bound of the band is above the line we're drawing); then it
 * finds the first box in that band that contains part of the
 * line.  we clip the line to subsequent boxes in that band.
 *    vertical lines are always drawn top to bottom (y-increasing.)
 * this requires adding one to the y-coordinate of each endpoint
 * after swapping.
 * 
 *    SLOPED LINES
 *    when clipping a sloped line, we bring the second point inside
 * the clipping box, rather than one beyond it, and then add 1 to
 * the length of the line before drawing it.  this lets us use
 * the same box for finding the outcodes for both endpoints.  since
 * the equation for clipping the second endpoint to an edge gives us
 * 1 beyond the edge, we then have to move the point towards the
 * first point by one step on the major axis.
 *    eventually, there will be a diagram here to explain what's going
 * on.  the method uses Cohen-Sutherland outcodes to determine
 * outsideness, and a method similar to Pike's layers for doing the
 * actual clipping.
 */


void
mach64Line(pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		mode;		/* Origin or Previous */
    int		npt;		/* number of points */
    DDXPointPtr pptInit;
{
    int nboxInit;
    register int nbox;
    BoxPtr pboxInit;
    register BoxPtr pbox;
    register DDXPointPtr ppt;	/* pointer to list of translated points */

    register unsigned int oc1;	/* outcode of point 1 */
    register unsigned int oc2;	/* outcode of point 2 */
    int xorg, yorg;		/* origin of window */
    register int y1, y2;
    register int x1, x2;
    register int tmp;
    cfbPrivGCPtr devPriv;


    devPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr); 

    if (!xf86VTSema)
    {
	switch (pDrawable->bitsPerPixel)
	{
	    case 8:
	        cfbLineSS(pDrawable, pGC, mode, npt, pptInit);
		break;
	    case 16:
	        cfb16LineSS(pDrawable, pGC, mode, npt, pptInit);
		break;
	    case 32:
	        cfb32LineSS(pDrawable, pGC, mode, npt, pptInit);
		break;
	    default:
		ErrorF("mach64Line: unsupported depth %d \n",
			pDrawable->bitsPerPixel);
	}
	return;
    }

    pboxInit = REGION_RECTS(devPriv->pCompositeClip);
    nboxInit = REGION_NUM_RECTS(devPriv->pCompositeClip);

    WaitQueue(4);
    regw(DP_FRGD_CLR, pGC->fgPixel);
    regw(DP_MIX, (mach64alu[pGC->alu] << 16) | MIX_DST);
    regw(DP_WRITE_MASK, pGC->planemask);
    regw(DP_SRC, FRGD_SRC_FRGD_CLR);

    xorg = pDrawable->x;
    yorg = pDrawable->y;
    ppt = pptInit;
    x2 = ppt->x + xorg;
    y2 = ppt->y + yorg;

    while(--npt)
    {
	x1 = x2;
	y1 = y2;
	++ppt;
	if (mode == CoordModePrevious)
	{
	    xorg = x1;
	    yorg = y1;
	}
	x2 = ppt->x + xorg;
	y2 = ppt->y + yorg;

	nbox = nboxInit;
	pbox = pboxInit;

	if (y1 == y2)
	{
	    /* force line from left to right, keeping
	       endpoint semantics
	    */
	    if (x1 > x2)
	    {
		tmp = x2;
		x2 = x1 + 1;
		x1 = tmp + 1;
	    }

	    /* find the correct band */
	    while( (nbox) && (pbox->y2 <= y1))
	    {
		pbox++;
		nbox--;
	    }

	    /* try to draw the line, if we haven't gone beyond it */
	    if ((nbox) && (pbox->y1 <= y1))
	    {

                WaitQueue(2);
                regw(SC_LEFT_RIGHT, ((mach64MaxX << 16) | 0 ));
                regw(SC_TOP_BOTTOM, ((mach64MaxY << 16) | 0 ));

		/* when we leave this band, we're done */
		tmp = pbox->y1;
		while((nbox) && (pbox->y1 == tmp))
		{
		    int	x1t, x2t;

		    if (pbox->x2 <= x1)
		    {
			/* skip boxes until one might contain start point */
			nbox--;
			pbox++;
			continue;
		    }

		    /* stop if left of box is beyond right of line */
		    if (pbox->x1 >= x2)
		    {
			nbox = 0;
			break;
		    }

		    x1t = max(x1, pbox->x1);
		    x2t = min(x2, pbox->x2);
		    if (x1t != x2t)
		    {
                        WaitQueue(3);
                        regw(DST_CNTL, (DST_X_LEFT_TO_RIGHT | 
                                        DST_Y_TOP_TO_BOTTOM));
                        regw(DST_Y_X, ((x1t << 16) | (y1 & 0x0000ffff)));
                        regw(DST_HEIGHT_WIDTH, (((x2t-x1t) << 16) | 1));
		    }
		    nbox--;
		    pbox++;
		}
	    }
	    x2 = ppt->x + xorg;
	}
	else	/* sloped line */
	{
	    register int dx, dy;
	    register int minDelta, maxDelta;
	    register int x_dir, y_dir, y_major;


            /* determine x & y deltas and x & y direction bits */
            if (x1 < x2)
            {
                dx = x2 - x1;
                x_dir = 1;
            }
            else
            {
                dx = x1 - x2;
		x_dir = 0;
            }

            if (y1 < y2)
            {
                dy = y2 - y1;
		y_dir = 2;
            }
            else
            {
                dy = y1 - y2;
		y_dir = 0;
            }
            /* determine x & y min and max values; also determine y major bit */
            if (dx < dy)
            {
                minDelta = dx;
                maxDelta = dy;
		y_major = 4;
            }
            else
            {
                minDelta = dy;
                maxDelta = dx;
		y_major = 0;
            }

            while(nbox--)
            {
                oc1 = 0;
                oc2 = 0;
                OUTCODES(oc1, x1, y1, pbox);
                OUTCODES(oc2, x2, y2, pbox);
                if (oc1 & oc2)
                {
                    pbox++;
                }
                else
                {
                    WaitQueue(8);
                    regw(SC_LEFT_RIGHT, (((pbox->x2 - 1) << 16) | pbox->x1));
                    regw(SC_TOP_BOTTOM, (((pbox->y2 - 1) << 16) | pbox->y1));
                    pbox++;

                    regw(DST_Y_X, (x1 << 16) | (y1 & 0x0000ffff));
                    regw(DST_CNTL, (y_major | y_dir | x_dir));
                    regw(DST_BRES_ERR, ((minDelta << 1) - maxDelta));
                    regw(DST_BRES_INC, (minDelta << 1));
                    regw(DST_BRES_DEC, (0x3ffff - ((maxDelta - minDelta)<< 1)));
                    regw(DST_BRES_LNTH, (maxDelta + 1));

                    if ((oc1 | oc2) == 0)
                        break;
                }
            } /* while (nbox--) */

        } /* sloped line */

    } /* while (--npt) */

    /* 
     * paint the last point if the end style isn't CapNotLast.
     * (Assume that a projecting, butt, or round cap that is one
     * pixel wide is the same as the single pixel of the endpoint.)
     */

    if ((pGC->capStyle != CapNotLast) &&
	((ppt->x + xorg != pptInit->x + pDrawable->x) ||
	 (ppt->y + yorg != pptInit->y + pDrawable->y) ||
	 (ppt == pptInit + 1)))
    {
        WaitQueue(2);
        regw(SC_LEFT_RIGHT, ((mach64MaxX << 16) | 0 ));
        regw(SC_TOP_BOTTOM, ((mach64MaxY << 16) | 0 ));

	nbox = nboxInit;
	pbox = pboxInit;
	while (nbox--)
	{
	    if ((x2 >= pbox->x1) &&
		(y2 >= pbox->y1) &&
		(x2 <  pbox->x2) &&
		(y2 <  pbox->y2))
	    {
                WaitQueue(3);
                regw(DST_CNTL, (DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM));
                regw(DST_Y_X, ((x2 << 16) | (y2 & 0x0000ffff)));
                regw(DST_HEIGHT_WIDTH, ((1 << 16) | 1));
		break;
	    }
	    else
		pbox++;
	}
    }

    WaitQueue(3);
    regw(SC_LEFT_RIGHT, ((mach64MaxX << 16) | 0 ));
    regw(SC_TOP_BOTTOM, ((mach64MaxY << 16) | 0 ));
    regw(DST_CNTL, (DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM));

    WaitIdleEmpty(); /* Make sure that all commands have finished */
}

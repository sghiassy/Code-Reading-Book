/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxLine.c,v 3.9 1996/12/23 06:32:53 dawes Exp $ */
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.
Copyright 1994 by Henry A. Worth, Sunnyvale, Ca.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL, KEVIN E. MARTIN, TIAGO GONS, AND HENRY A. WORTH  DISCLAIM ALL 
WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL DIGITAL, 
KEVIN E. MARTIN, TIAGO GONS, OR HENRY A. WORTH BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
Further modifications by Tiago Gons (tiago@comosjn.hobby.nl)
Modified for the AGX by Henry A. Worth (haw30@eng.amdahl.com)

******************************************************************/
/* $XConsortium: agxLine.c /main/6 1996/02/21 17:18:11 kaleb $  */

#include "X.h"

#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "mistruct.h"
#include "miline.h"

#include "cfb.h"
#include "cfbmskbits.h"
#include "regagx.h"
#include "agx.h"

/* single-pixel lines on a color frame buffer

   NON-SLOPED LINES
   horizontal lines are always drawn left to right; we have to
move the endpoints right by one after they're swapped.
   horizontal lines will be confined to a single band of a
region.  the code finds that band (giving up if the lower
bound of the band is above the line we're drawing); then it
finds the first box in that band that contains part of the
line.  we clip the line to subsequent boxes in that band.
   vertical lines are always drawn top to bottom (y-increasing.)
this requires adding one to the y-coordinate of each endpoint
after swapping.

   SLOPED LINES
   when clipping a sloped line, we bring the second point inside
the clipping box, rather than one beyond it, and then add 1 to
the length of the line before drawing it.  this lets us use
the same box for finding the outcodes for both endpoints.  since
the equation for clipping the second endpoint to an edge gives us
1 beyond the edge, we then have to move the point towards the
first point by one step on the major axis.
   eventually, there will be a diagram here to explain what's going
on.  the method uses Cohen-Sutherland outcodes to determine
outsideness, and a method similar to Pike's layers for doing the
actual clipping.

*/

/*
#define OUTCODES(result, x, y, pbox) \
    if (x < pbox->x1) \
	result |= OUT_LEFT; \
    else if (x >= pbox->x2) \
	result |= OUT_RIGHT; \
    if (y < pbox->y1) \
	result |= OUT_ABOVE; \
    else if (y >= pbox->y2) \
	result |= OUT_BELOW;
*/


void
agxLine(pDrawable, pGC, mode, npt, pptInit)
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

    unsigned int oc1;		/* outcode of point 1 */
    unsigned int oc2;		/* outcode of point 2 */

    int xorg, yorg;		/* origin of window */

    int adx;		/* abs values of dx and dy */
    int ady;
    int signdx;		/* sign of dx and dy */
    int signdy;
    int e, e1, e2;		/* bresenham error and increments */
    register int len;		/* length of segment */
    int axis;			/* major axis */
    int   octant;
    unsigned int bias = miGetZeroLineBias(pDrawable->pScreen);
    short cmd2;
    short fix;
    Bool  first = TRUE;

				/* a bunch of temporaries */
    int tmp;
    register int y1, y2;
    register int x1, x2;
    RegionPtr cclip;
    cfbPrivGCPtr    devPriv;

    if (!xf86VTSema)
    {
      switch (agxInfoRec.bitsPerPixel) {
      case 8:
         cfbLineSS(pDrawable, pGC, mode, npt, pptInit);
         break;
      case 16:
         cfb16LineSS(pDrawable, pGC, mode, npt, pptInit);
         break;
#ifdef AGX_32BPP
      case 32:
         cfb32LineSS(pDrawable, pGC, mode, npt, pptInit);
         break;
#endif
      }
      return;
    }

    devPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr); 
    cclip = devPriv->pCompositeClip;
    pboxInit = REGION_RECTS(cclip);
    nboxInit = REGION_NUM_RECTS(cclip);

    xorg = pDrawable->x;
    yorg = pDrawable->y;
    ppt = pptInit;
    x2 = ppt->x + xorg;
    y2 = ppt->y + yorg;
    while(--npt)
    {
	nbox = nboxInit;
	pbox = pboxInit;

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

	if (x1 == x2)
	{
	    /* make the line go top to bottom of screen, keeping
	       endpoint semantics
	    */
	    if (y1 > y2)
	    {
		register int tmp;

		tmp = y2;
		y2 = y1 + 1;
		y1 = tmp + 1;
	    }
	    /* get to first band that might contain part of line */
	    while ((nbox) && (pbox->y2 <= y1))
	    {
		pbox++;
		nbox--;
	    }

	    /* stop when lower edge of box is beyond end of line */
	    while((nbox) && (y2 >= pbox->y1)) {
	       if ((x1 >= pbox->x1) && (x1 < pbox->x2)) {
		  int y1t, y2t;
		  /* this box has part of the line in it */
		  y1t = max(y1, pbox->y1);
		  y2t = min(y2, pbox->y2);
		  if (y1t != y2t) {
                     register unsigned int opDim = (y2t-y1t-1) << 16;
                     register unsigned int mapCoOrd = y1t << 16 | x1;
                     GE_WAIT_IDLE();
                     if (first) {
                        first = FALSE;
                        MAP_SET_SRC_AND_DST( GE_MS_MAP_A );
                        GE_OUT_B(GE_FRGD_MIX, (char) pGC->alu);
                        GE_OUT_D(GE_PIXEL_BIT_MASK, (int) pGC->planemask);
                        GE_OUT_D(GE_FRGD_CLR, (int) pGC->fgPixel);
                     }
                     GE_OUT_D( GE_DEST_MAP_X, mapCoOrd );
                     GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
                     GE_START_CMD( GE_OP_BITBLT
                                   | GE_OP_MASK_DISABLED
                                   | GE_OP_PAT_FRGD
                                   | GE_OP_FRGD_SRC_CLR
                                   | GE_OP_SRC_MAP_A
                                   | GE_OP_DEST_MAP_A 
                                   | GE_OP_INC_X
                                   | GE_OP_INC_Y         );

		  }
               }
	       nbox--;
	       pbox++;
	    }
	    y2 = ppt->y + yorg;
	}
	else if (y1 == y2)
	{
	    /* force line from left to right, keeping
	       endpoint semantics
	    */
	    if (x1 > x2)
	    {
		register int tmp;

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

	    while((nbox) && (pbox->y1 <= y1) && (pbox->x1 < x2))
            {
	        int	x1t, x2t;

		if (pbox->x2 > x1)
		{
	            x1t = max(x1, pbox->x1);
		    x2t = min(x2, pbox->x2);
		    if (x1t != x2t) {
                       register unsigned int opDim = x2t-x1t-1;
                       register unsigned int mapCoOrd = y1 << 16 | x1t;
                       GE_WAIT_IDLE();
                       if (first) {
                          first = FALSE;
                          MAP_SET_SRC_AND_DST( GE_MS_MAP_A );
                          GE_OUT_B(GE_FRGD_MIX, (char) pGC->alu);
                          GE_OUT_D(GE_PIXEL_BIT_MASK, (int) pGC->planemask);
                          GE_OUT_D(GE_FRGD_CLR, (int) pGC->fgPixel);
                       }
                       GE_OUT_D( GE_DEST_MAP_X, mapCoOrd );
                       GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
                       GE_START_CMD( GE_OP_BITBLT
                                     | GE_OP_MASK_DISABLED
                                     | GE_OP_PAT_FRGD
                                     | GE_OP_FRGD_SRC_CLR
                                     | GE_OP_SRC_MAP_A
                                     | GE_OP_DEST_MAP_A 
                                     | GE_OP_INC_X
                                     | GE_OP_INC_Y         );
                   }
		}
		nbox--;
		pbox++;
	    }
	    x2 = ppt->x + xorg;
	}
	else	/* sloped line */
	{
	    cmd2 = 0;
            fix = -1;
	    if ((adx = x2 - x1) < 0) {
		fix    = 0;
		cmd2  |= GE_OP_DEC_X;
	    }
	    if ((ady = y2 - y1) < 0) {
		cmd2  |= GE_OP_DEC_Y;
	    }
	    CalcLineDeltas(x1, y1, x2, y2, adx, ady, signdx, signdy,
			   1, 1, octant);

	    if (adx > ady) {
		axis = X_AXIS;
                len  = adx;
		e1    = ady << 1;
	    }
	    else {
		axis  = Y_AXIS;
                len   = ady;
		e1    = adx << 1;
		cmd2 |= GE_OP_Y_MAJ; 
		SetYMajorOctant(octant);
	    }
	    e  = e1 - len;
	    e2 = e  - len;

	    FIXUP_ERROR(e, octant, bias);

	    /* we have bresenham parameters and two points.
	       all we have to do now is clip and draw.
	    */

	    while(nbox--)
	    {
		oc1 = 0;
		oc2 = 0;
		OUTCODES(oc1, x1, y1, pbox);
		OUTCODES(oc2, x2, y2, pbox);
		if ((oc1 | oc2) == 0)
		{
                    register unsigned int mapCoOrd = y1 << 16 | x1;
                    GE_WAIT_IDLE();
                    if (first) {
                       first = FALSE;
                       MAP_SET_SRC_AND_DST( GE_MS_MAP_A );
                       GE_OUT_B(GE_FRGD_MIX, (char) pGC->alu);
                       GE_OUT_D(GE_PIXEL_BIT_MASK, (int) pGC->planemask);
                       GE_OUT_D(GE_FRGD_CLR, (int) pGC->fgPixel);
                    }
                    GE_OUT_D( GE_DEST_MAP_X, mapCoOrd );
                    GE_OUT_W( GE_OP_DIM_LINE_MAJ, len );
                    GE_OUT_W( GE_BRES_ERROR_TERM, e+fix );
                    GE_OUT_W( GE_BRES_CONST_K1,   e1 );
                    GE_OUT_W( GE_BRES_CONST_K2,   e2 );
                    GE_START_CMD( GE_OP_LINE_DRAW_WR
                                  | GE_OP_FRGD_SRC_CLR
                                  | GE_OP_SRC_MAP_A
                                  | GE_OP_DEST_MAP_A
                                  | GE_OP_DRAW_LAST_NULL
                                  | GE_OP_PAT_FRGD
                                  | cmd2                );
		    break;
		}
		else if (oc1 & oc2)
		{
		    pbox++;
		}
		else
		{
	    	    /*
	     	     * let the mi helper routine do our work;
	     	     * better than duplicating code...
	     	     */
    	    	    int err;		   /* modified bresenham error term */
    	    	    int clip1=0, clip2=0;  /* clippedness of the endpoints */
    	    	
    	    	    int clipdx, clipdy;    /* difference between clipped and
				       	       	       unclipped start point */

                    int new_x1 = x1, new_y1 = y1, new_x2 = x2, new_y2 = y2;
    	

                    if (miZeroClipLine(pbox->x1, pbox->y1,
                                        pbox->x2-1, pbox->y2-1,
                                        &new_x1, &new_y1,
                                        &new_x2, &new_y2,
                                        adx, ady,
                                        &clip1, &clip2,
                                        octant, bias,
                                        oc1, oc2) == -1)
                    {
                        pbox++;
                        continue;
                    }
                    if (axis == X_AXIS)
                       len = abs(new_x2 - new_x1);
                    else
                       len = abs(new_y2 - new_y1);
    
                    len -= (clip2 == 0);
                    if (len > 0)
                    {
                       register unsigned int mapCoOrd = new_y1 << 16 | new_x1;
                        /* unwind bresenham error term to first point */
                        if (clip1)
                        {
                            clipdx = abs(new_x1 - x1);
                            clipdy = abs(new_y1 - y1);
                            if (axis == X_AXIS)
                                err = e+((clipdy*e2) + ((clipdx-clipdy)*e1));
                            else
                                err = e+((clipdx*e2) + ((clipdy-clipdx)*e1));
                        }
                        else
                            err = e;

                        if ( abs(err) > 8192
                             || abs(e1) > 8192
                             || abs(e2) > 8192 ) {
                           int div;
  
                           if (abs(err) > abs(e1))
                               div = (abs(err) > abs(e2)) ?
                               (abs(err) + 8192)/ 8192 : (abs(e2) + 8191)/ 8192;
                           else
                               div = (abs(e1) > abs(e2)) ?
                               (abs(e1) + 8191)/ 8192 : (abs(e2) + 8191)/ 8192;

                           err /= div;
                           e1 /= div;
                           e2 /= div;
                        }

                        GE_WAIT_IDLE();
                        if (first) {
                           first = FALSE;
                           MAP_SET_SRC_AND_DST( GE_MS_MAP_A );
                           GE_OUT_B(GE_FRGD_MIX, (char) pGC->alu);
                           GE_OUT_D(GE_PIXEL_BIT_MASK, (int) pGC->planemask);
                           GE_OUT_D(GE_FRGD_CLR, (int) pGC->fgPixel);
                           GE_OUT_W( GE_PIXEL_OP,
                                     GE_OP_PAT_FRGD
                                     | GE_OP_MASK_DISABLED
                                     | GE_OP_DRAW_LAST_NULL
                                     | cmd2                );
                        }

                        GE_OUT_D( GE_DEST_MAP_X, mapCoOrd );
                        GE_OUT_W( GE_OP_DIM_LINE_MAJ, (short) len );
                        GE_OUT_W( GE_BRES_ERROR_TERM, (short) err+fix );
                        GE_OUT_W( GE_BRES_CONST_K1,   (short) e1 );
                        GE_OUT_W( GE_BRES_CONST_K2,   (short) e2 );
                        GE_START_CMD( GE_OP_LINE_DRAW_WR
                                      | GE_OP_FRGD_SRC_CLR
                                      | GE_OP_SRC_MAP_A
                                      | GE_OP_DEST_MAP_A
                                      | GE_OP_DRAW_ALL
                                      | GE_OP_PAT_FRGD
                                      | cmd2                );
		    }
		    pbox++;
		}
	    } /* while (nbox--) */
	} /* sloped line */
    } /* while (nline--) */

    /* paint the last point if the end style isn't CapNotLast.
       (Assume that a projecting, butt, or round cap that is one
        pixel wide is the same as the single pixel of the endpoint.)
    */

    if ((pGC->capStyle != CapNotLast) &&
	((ppt->x + xorg != pptInit->x + pDrawable->x) ||
	 (ppt->y + yorg != pptInit->y + pDrawable->y) ||
	 (ppt == pptInit + 1)))
    {
	nbox = nboxInit;
	pbox = pboxInit;
	while (nbox--)
	{
	    if ((x2 >= pbox->x1) &&
		(y2 >= pbox->y1) &&
		(x2 <  pbox->x2) &&
		(y2 <  pbox->y2))
            {
                register unsigned int mapCoOrd = y2 << 16 | x2;
                GE_WAIT_IDLE();
                if (first) {
                   first = FALSE;
                   MAP_SET_SRC_AND_DST( GE_MS_MAP_A );
                   GE_OUT_B(GE_FRGD_MIX, (char) pGC->alu);
                   GE_OUT_D(GE_PIXEL_BIT_MASK, (int) pGC->planemask);
                   GE_OUT_D(GE_FRGD_CLR, (int) pGC->fgPixel);
                }
                GE_OUT_D( GE_DEST_MAP_X, mapCoOrd );
                GE_OUT_W( GE_OP_DIM_LINE_MAJ, (short) 0 );
                GE_START_CMD( GE_OP_LINE_DRAW_WR
                              | GE_OP_FRGD_SRC_CLR
                              | GE_OP_SRC_MAP_A
                              | GE_OP_DEST_MAP_A
                              | GE_OP_PAT_FRGD
                              | GE_OP_INC_X
                              | GE_OP_INC_Y          );

		break;
	    }
	    else
		pbox++;
	}
    }

    GE_WAIT_IDLE_EXIT();
}

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach8/mach8dssg.c,v 3.2 1996/12/23 06:39:49 dawes Exp $ */
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


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL, KEVIN E. MARTIN, AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL DIGITAL, KEVIN E. MARTIN, OR TIAGO GONS BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
Further modifications by Tiago Gons (tiago@comosjn.hobby.nl)

*/

/* 18-sep-93 TCG: mach8dssg.c from ibm8514dssg.c */
/* $XConsortium: mach8dssg.c /main/5 1996/02/21 17:30:03 kaleb $ */

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
#include "regmach8.h"
#include "mach8.h"

void
#ifdef Mach8DoubleDash
mach8DoubleDashSegment(pDrawable, pGC, nseg, pSeg)
#else
mach8DashSegment(pDrawable, pGC, nseg, pSeg)
#endif
    DrawablePtr	pDrawable;
    GCPtr	pGC;
    int		nseg;
    register xSegment	*pSeg;
{
    int nboxInit;
    register int nbox;
    BoxPtr pboxInit;
    register BoxPtr pbox;

    unsigned int oc1;           /* outcode of point 1 */
    unsigned int oc2;           /* outcode of point 2 */

    int xorg, yorg;             /* origin of window */

    int adx;                    /* abs values of dx and dy */
    int ady;
    int signdx;                 /* sign of dx and dy */
    int signdy;
    int e, e1, e2;              /* bresenham error and increments */
    int len;                    /* length of segment */
    int axis;                   /* major axis */
    int octant;
    unsigned int bias = miGetZeroLineBias(pDrawable->pScreen);
    short cmd = CMD_LINE | PLANAR | WRTDATA | LASTPIX;
    short cmd2;
    short fix;

    int dash0len, dashidx, dashstartidx, dashnum, dashrem;
    unsigned char *pDash;
                                /* a bunch of temporaries */
    register int y1, y2;
    register int x1, x2;
    RegionPtr cclip;
    cfbPrivGCPtr    devPriv;

/* 4-5-93 TCG : is VT visible */
    if (!xf86VTSema)
    {
        cfbSegmentSD(pDrawable, pGC, nseg, pSeg);
        return;
    }

    devPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr); 
    cclip = devPriv->pCompositeClip;
    pboxInit = REGION_RECTS(cclip);
    nboxInit = REGION_NUM_RECTS(cclip);

#ifdef Mach8DoubleDash
    WaitQueue(2);
#else
    WaitQueue(3);
    outw(FRGD_COLOR, (short)pGC->fgPixel);
#endif
    outw(FRGD_MIX, FSS_FRGDCOL | mach8alu[pGC->alu]);
    outw(WRT_MASK, (short)pGC->planemask);

    pDash = (unsigned char *) pGC->dash;
    dashnum = pGC->numInDashList;
    dashstartidx = 0;
    dash0len = 0;
    miStepDash((int)pGC->dashOffset, &dashstartidx, pDash, dashnum, &dash0len);
    dash0len = pDash[dashstartidx] - dash0len;

    xorg = pDrawable->x;
    yorg = pDrawable->y;

    while(nseg--)
    {
        nbox = nboxInit;
        pbox = pboxInit;

        x1 = pSeg->x1 + xorg;
        y1 = pSeg->y1 + yorg;
        x2 = pSeg->x2 + xorg;
        y2 = pSeg->y2 + yorg;
        pSeg++;

        /* sloped line */
        {
            cmd2 = cmd;
            if ((adx = x2 - x1) < 0)
            {
                fix = 0;
            }
            else
            {
                cmd2 |= INC_X;
                fix = -1;
            }
            if ((ady = y2 - y1) >= 0)
            {
                cmd2 |= INC_Y;
            }

	    CalcLineDeltas(x1, y1, x2, y2, adx, ady, signdx, signdy,
			   1, 1, octant);

            if (adx > ady)
            {
                axis = X_AXIS;
                e1 = ady << 1;
                e2 = e1 - (adx << 1);
                e = e1 - adx;
            }
            else
            {
                axis = Y_AXIS;
                e1 = adx << 1;
                e2 = e1 - (ady << 1);
                e = e1 - ady;
                cmd2 |= YMAJAXIS;
		SetYMajorOctant(octant);
            }

	    FIXUP_ERROR(e, octant, bias);

            /* we have bresenham parameters and two points.
             */

            while(nbox--)
            {
                oc1 = 0;
                oc2 = 0;
                OUTCODES(oc1, x1, y1, pbox);
                OUTCODES(oc2, x2, y2, pbox);
                if ((oc1 | oc2) == 0)
                {
                    if (axis == X_AXIS)
                        len = adx;
                    else
                        len = ady;

                    if (pGC->capStyle != CapNotLast)
                        len++;

                    /* setup 8514/A for (dashed) line */
                    WaitQueue(5);
                    outw(CUR_X, (short)x1);
                    outw(CUR_Y, (short)y1);
                    outw(ERR_TERM, (short)(e + fix));
                    outw(DESTY_AXSTP, (short)e1);
                    outw(DESTX_DIASTP, (short)e2);

                    dashidx = dashstartidx;
                    dashrem = dash0len;

                    if (len < dashrem)
                    {
#ifdef Mach8DoubleDash
                        WaitQueue(3);
                        if (dashidx & 1)
                            outw(FRGD_COLOR, (short)pGC->bgPixel);
                        else
                            outw(FRGD_COLOR, (short)pGC->fgPixel);
                        outw(MAJ_AXIS_PCNT, (short)len);
                        outw(CMD, cmd2 | DRAW);
#else
                        WaitQueue(2);
                        outw(MAJ_AXIS_PCNT, (short)len);
                        if (dashidx & 1)
                            outw(CMD, cmd2);
                        else
                            outw(CMD, cmd2 | DRAW);
#endif
                    } else
                    {
                        if (dashrem > 0)
                        {
#ifdef Mach8DoubleDash
                            WaitQueue(3);
                            if (dashidx & 1)
                                outw(FRGD_COLOR, (short)pGC->bgPixel);
                            else
                                outw(FRGD_COLOR, (short)pGC->fgPixel);
                            outw(MAJ_AXIS_PCNT, (short)dashrem);
                            outw(CMD, cmd2 | DRAW);
#else
                            WaitQueue(2);
                            outw(MAJ_AXIS_PCNT, (short)dashrem);
                            if (dashidx & 1)
                                outw(CMD, cmd2);
                            else
                                outw(CMD, cmd2 | DRAW);
#endif
                            len -= dashrem;
                            dashrem = 0;
                            dashidx++;
                            if (dashidx == dashnum)
                                dashidx = 0;
                        }
                        while (pDash[dashidx] <= len)
                        {
#ifdef Mach8DoubleDash
                            WaitQueue(3);
                            if (dashidx & 1)
                                outw(FRGD_COLOR, (short)pGC->bgPixel);
                            else
                                outw(FRGD_COLOR, (short)pGC->fgPixel);
                            outw(MAJ_AXIS_PCNT, (short)pDash[dashidx]);
                            outw(CMD, cmd2 | DRAW);
#else
                            WaitQueue(2);
                            outw(MAJ_AXIS_PCNT, (short)pDash[dashidx]);
                            if (dashidx & 1)
                                outw(CMD, cmd2);
                            else
                                outw(CMD, cmd2 | DRAW);
#endif
                            len -= pDash[dashidx];
                            dashidx++;
                            if (dashidx == dashnum)
                                dashidx = 0;
                        }
                        if (len > 0)
                        {
#ifdef Mach8DoubleDash
                            WaitQueue(3);
                            if (dashidx & 1)
                                outw(FRGD_COLOR, (short)pGC->bgPixel);
                            else
                                outw(FRGD_COLOR, (short)pGC->fgPixel);
                            outw(MAJ_AXIS_PCNT, (short)len);
                            outw(CMD, cmd2 | DRAW);
#else
                            WaitQueue(2);
                            outw(MAJ_AXIS_PCNT, (short)len);
                            if (dashidx & 1)
                                outw(CMD, cmd2);
                            else
                                outw(CMD, cmd2 | DRAW);
#endif
                        }
                    }

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
                    int err;             /* modified bresenham error term */
                    int clip1, clip2;    /* clippedness of the endpoints */
                    int clipdx = 0;      /* difference between clipped and */
                    int clipdy = 0;      /* unclipped start point */
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
   
                    if (clip2 != 0 || pGC->capStyle != CapNotLast)
                        len++;

                    if (len)
                    {
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

                        WaitQueue(5);
                        outw(CUR_X, (short)new_x1);
                        outw(CUR_Y, (short)new_y1);
                        outw(ERR_TERM, (short)(err + fix));
                        outw(DESTY_AXSTP, (short)e1);
                        outw(DESTX_DIASTP, (short)e2);

                        dashidx = dashstartidx;
                        dashrem = pDash[dashidx] - dash0len;
                        if (axis == X_AXIS)
                            miStepDash(clipdx, &dashidx, pDash,
                                       dashnum, &dashrem);
                        else
                            miStepDash(clipdy, &dashidx, pDash,
                                       dashnum, &dashrem);
                        dashrem = pDash[dashidx] - dashrem;

                        if (len < dashrem)
                        {
#ifdef Mach8DoubleDash
                            WaitQueue(3);
                            if (dashidx & 1)
                                outw(FRGD_COLOR, (short)pGC->bgPixel);
                            else
                                outw(FRGD_COLOR, (short)pGC->fgPixel);
                            outw(MAJ_AXIS_PCNT, (short)len);
                            outw(CMD, cmd2 | DRAW);
#else
                            WaitQueue(2);
                            outw(MAJ_AXIS_PCNT, (short)len);
                            if (dashidx & 1)
                                outw(CMD, cmd2);
                            else
                                outw(CMD, cmd2 | DRAW);
#endif
                        } else
                        {
                            if (dashrem > 0)
                            {
#ifdef Mach8DoubleDash
                                WaitQueue(3);
                                if (dashidx & 1)
                                    outw(FRGD_COLOR, (short)pGC->bgPixel);
                                else
                                    outw(FRGD_COLOR, (short)pGC->fgPixel);
                                outw(MAJ_AXIS_PCNT, (short)dashrem);
                                outw(CMD, cmd2 | DRAW);
#else
                                WaitQueue(2);
                                outw(MAJ_AXIS_PCNT, (short)dashrem);
                                if (dashidx & 1)
                                    outw(CMD, cmd2);
                                else
                                    outw(CMD, cmd2 | DRAW);
#endif
                                len -= dashrem;
                                dashrem = 0;
                                dashidx++;
                                if (dashidx == dashnum)
                                    dashidx = 0;
                            }
                            while (pDash[dashidx] <= len)
                            {
#ifdef Mach8DoubleDash
                                WaitQueue(3);
                                if (dashidx & 1)
                                    outw(FRGD_COLOR, (short)pGC->bgPixel);
                                else
                                    outw(FRGD_COLOR, (short)pGC->fgPixel);
                                outw(MAJ_AXIS_PCNT, (short)pDash[dashidx]);
                                outw(CMD, cmd2 | DRAW);
#else
                                WaitQueue(2);
                                outw(MAJ_AXIS_PCNT, (short)pDash[dashidx]);
                                if (dashidx & 1)
                                    outw(CMD, cmd2);
                                else
                                    outw(CMD, cmd2 | DRAW);
#endif
                                len -= pDash[dashidx];
                                dashidx++;
                                if (dashidx == dashnum)
                                    dashidx = 0;
                            }
                            if (len > 0)
                            {
#ifdef Mach8DoubleDash
                                WaitQueue(3);
                                if (dashidx & 1)
                                    outw(FRGD_COLOR, (short)pGC->bgPixel);
                                else
                                    outw(FRGD_COLOR, (short)pGC->fgPixel);
                                outw(MAJ_AXIS_PCNT, (short)len);
                                outw(CMD, cmd2 | DRAW);
#else
                                WaitQueue(2);
                                outw(MAJ_AXIS_PCNT, (short)len);
                                if (dashidx & 1)
                                    outw(CMD, cmd2);
                                else
                                    outw(CMD, cmd2 | DRAW);
#endif
                            }
                        }    
                    }
                    pbox++;
                }
            } /* while (nbox--) */
        } /* sloped line */
    } /* while (nseg--) */

    WaitQueue(2);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
}

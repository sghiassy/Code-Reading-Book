/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach8/mach8dsln.c,v 3.2 1996/12/23 06:39:48 dawes Exp $ */
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

/* 17-sep-93 TCG: mach8dsln.c from ibm8514dsln.c */
/* $XConsortium: mach8dsln.c /main/7 1996/02/21 17:30:00 kaleb $ */

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
mach8DoubleDashLine(pDrawable, pGC, mode, npt, pptInit)
#else
mach8DashLine(pDrawable, pGC, mode, npt, pptInit)
#endif
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         mode;           /* Origin or Previous */
    int         npt;            /* number of points */
    DDXPointPtr pptInit;
{
    int nboxInit;
    register int nbox;
    BoxPtr pboxInit;
    register BoxPtr pbox;
    register DDXPointPtr ppt;   /* pointer to list of translated points */

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

    int dashidx, dashnum, dashrem;
    unsigned char *pDash;
    Bool dashupdated;
                                /* a bunch of temporaries */
    register int y1, y2;
    register int x1, x2;
    RegionPtr cclip;
    cfbPrivGCPtr    devPriv;

/* 4-5-93 TCG : is VT visible */
    if (!xf86VTSema)
    {
        cfbLineSD(pDrawable, pGC, mode, npt, pptInit);
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
    dashidx = 0;
    dashrem = 0;
    miStepDash((int)pGC->dashOffset, &dashidx, pDash, dashnum, &dashrem);
    dashrem = pDash[dashidx] - dashrem;

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
            dashupdated = FALSE;

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

                    /* setup 8514/A for (dashed) line */
                    WaitQueue(5);
                    outw(CUR_X, (short)x1);
                    outw(CUR_Y, (short)y1);
                    outw(ERR_TERM, (short)(e + fix));
                    outw(DESTY_AXSTP, (short)e1);
                    outw(DESTX_DIASTP, (short)e2);

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
                        dashrem -= len;
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
                            dashrem = pDash[dashidx] - len;
                        }
                    }
                    dashupdated = TRUE;

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
                    int clip1=0, clip2=0;/* clippedness of the endpoints */
                    int clipdx = 0;      /* difference between clipped and */
                    int clipdy = 0;      /* unclipped start point */
                    int dashrem2, dashidx2;
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
                        len = abs(new_x1 - new_x1);
                    else
                        len = abs(new_y2 - new_y1);
    
                    len += (clip2 != 0);
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

                        dashidx2 = dashidx;
                        dashrem2 = pDash[dashidx2] - dashrem;
                        if (axis == X_AXIS)
                            miStepDash(clipdx, &dashidx2, pDash,
                                       dashnum, &dashrem2);
                        else
                            miStepDash(clipdy, &dashidx2, pDash,
                                       dashnum, &dashrem2);
                        dashrem2 = pDash[dashidx2] - dashrem2;

                        if (len < dashrem2)
                        {
#ifdef Mach8DoubleDash
                            WaitQueue(3);
                            if (dashidx2 & 1)
                                outw(FRGD_COLOR, (short)pGC->bgPixel);
                            else
                                outw(FRGD_COLOR, (short)pGC->fgPixel);
                            outw(MAJ_AXIS_PCNT, (short)len);
                            outw(CMD, cmd2 | DRAW);
#else
                            WaitQueue(2);
                            outw(MAJ_AXIS_PCNT, (short)len);
                            if (dashidx2 & 1)
                                outw(CMD, cmd2);
                            else
                                outw(CMD, cmd2 | DRAW);
#endif
                        } else
                        {
                            if (dashrem2 > 0)
                            {
#ifdef Mach8DoubleDash
                                WaitQueue(3);
                                if (dashidx2 & 1)
                                    outw(FRGD_COLOR, (short)pGC->bgPixel);
                                else
                                    outw(FRGD_COLOR, (short)pGC->fgPixel);
                                outw(MAJ_AXIS_PCNT, (short)dashrem2);
                                outw(CMD, cmd2 | DRAW);
#else
                                WaitQueue(2);
                                outw(MAJ_AXIS_PCNT, (short)dashrem2);
                                if (dashidx2 & 1)
                                    outw(CMD, cmd2);
                                else
                                    outw(CMD, cmd2 | DRAW);
#endif
                                len -= dashrem2;
                                dashrem2 = 0;
                                dashidx2++;
                                if (dashidx2 == dashnum)
                                    dashidx2 = 0;
                            }
                            while (pDash[dashidx2] <= len)
                            {
#ifdef Mach8DoubleDash
                                WaitQueue(3);
                                if (dashidx2 & 1)
                                    outw(FRGD_COLOR, (short)pGC->bgPixel);
                                else
                                    outw(FRGD_COLOR, (short)pGC->fgPixel);
                                outw(MAJ_AXIS_PCNT, (short)pDash[dashidx2]);
                                outw(CMD, cmd2 | DRAW);
#else
                                WaitQueue(2);
                                outw(MAJ_AXIS_PCNT, (short)pDash[dashidx2]);
                                if (dashidx2 & 1)
                                    outw(CMD, cmd2);
                                else
                                    outw(CMD, cmd2 | DRAW);
#endif
                                len -= pDash[dashidx2];
                                dashidx2++;
                                if (dashidx2 == dashnum)
                                    dashidx2 = 0;
                            }
                            if (len > 0)
                            {
#ifdef Mach8DoubleDash
                                WaitQueue(3);
                                if (dashidx2 & 1)
                                    outw(FRGD_COLOR, (short)pGC->bgPixel);
                                else
                                    outw(FRGD_COLOR, (short)pGC->fgPixel);
                                outw(MAJ_AXIS_PCNT, (short)len);
                                outw(CMD, cmd2 | DRAW);
#else
                                WaitQueue(2);
                                outw(MAJ_AXIS_PCNT, (short)len);
                                if (dashidx2 & 1)
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

            if (!dashupdated)
            {
                /* update dash position */
                dashrem = pDash[dashidx] - dashrem;
                if (axis == X_AXIS)
                    miStepDash(adx, &dashidx, pDash, dashnum, &dashrem);
                else
                    miStepDash(ady, &dashidx, pDash, dashnum, &dashrem);
                dashrem = pDash[dashidx] - dashrem;
            }

        } /* sloped line */
    } /* while (--npt) */

    /* paint the last point if the end style isn't CapNotLast.
       (Assume that a projecting, butt, or round cap that is one
        pixel wide is the same as the single pixel of the endpoint.)
    */

    if ((pGC->capStyle != CapNotLast) &&
#ifndef Mach8DoubleDash
        ((dashidx & 1) == 0) &&
#endif
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
#ifdef Mach8DoubleDash
                WaitQueue(5);
                if (dashidx & 1)
                    outw(FRGD_COLOR, (short)pGC->bgPixel);
                else
                    outw(FRGD_COLOR, (short)pGC->fgPixel);
#else
                WaitQueue(4);
#endif
                outw(CUR_X, (short)x2);
                outw(CUR_Y, (short)y2);
                outw(MAJ_AXIS_PCNT, 0);
                outw(CMD, CMD_LINE | DRAW | LINETYPE | PLANAR | WRTDATA);

                break;
            }
            else
                pbox++;
        }
    }

    WaitQueue(2);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
}

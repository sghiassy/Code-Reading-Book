/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach8/mach8seg.c,v 3.5 1996/12/23 06:40:10 dawes Exp $ */
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

DIGITAL, KEVIN E. MARTIN, RICKARD E. FAITH, AND TIAGO GONS DISCLAIM
ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL, KEVIN E. MARTIN, RICKARD E. FAITH, OR TIAGO GONS BE LIABLE
FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Further modifications by Tiago Gons (tiago@comosjn.hobby.nl)

*/
/* $XConsortium: mach8seg.c /main/6 1996/02/21 17:31:15 kaleb $ */

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

#define OUTCODES2(result, x, y, l, r, t, b) \
    if (x < l) \
        result |= OUT_LEFT; \
    else if (x >= r) \
        result |= OUT_RIGHT; \
    if (y < t) \
        result |= OUT_ABOVE; \
    else if (y >= b) \
        result |= OUT_BELOW;

void
mach8Segment(pDrawable, pGC, nseg, pSeg)
    DrawablePtr         pDrawable;
    GCPtr               pGC;
    int                 nseg;
    register xSegment   *pSeg;
{
    int nboxInit;
    register int nbox;
    BoxPtr pboxInit;
    register BoxPtr pbox;

    unsigned int oc1;       /* outcode of point 1 */
    unsigned int oc2;       /* outcode of point 2 */

    int xorg, yorg;         /* origin of window */

    int adx;                /* abs values of dx and dy */
    int ady;
    int signdx;             /* sign of dx and dy */
    int signdy;
    int e, e1, e2;          /* bresenham error and increments */
    int len;                /* length of segment */
    int axis;               /* major axis */
    int octant;
    unsigned int bias = miGetZeroLineBias(pDrawable->pScreen);
    short cmd = CMD_LINE | DRAW | PLANAR | WRTDATA | LASTPIX;
    short cmd2;
    short fix;

                            /* a bunch of temporaries */
    register xSegment *pSegtmp;
    int tmp, nsegtmp;
    register int y1, y2;
    register int x1, x2;
    RegionPtr cclip;
    cfbPrivGCPtr    devPriv;

/* 11-jun-93 TCG : is VT visible */
    if (!xf86VTSema)
    {
        cfbSegmentSS(pDrawable, pGC, nseg, pSeg);
        return;
    }

    devPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr); 
    cclip = devPriv->pCompositeClip;
    pboxInit = REGION_RECTS(cclip);
    nboxInit = REGION_NUM_RECTS(cclip);

    if ((nboxInit < 5) && (nboxInit < nseg)) {
        /* 14 sep 93 TCG: iterate segments against clipboxes using the Mach-8
                          direct linedraw registers */
        WaitQueue(5);
        outw(DP_CONFIG, 0x2211);
        outw(FRGD_MIX, FSS_FRGDCOL | mach8alu[pGC->alu]);
        outw(WRT_MASK, (short)pGC->planemask);
        outw(FRGD_COLOR, (short)pGC->fgPixel);

        if (pGC->capStyle != CapNotLast)
            outw(LINEDRAW_OPT, 0);
        else
            outw(LINEDRAW_OPT, 4);

        while (nboxInit--) {

            WaitQueue(4);
            outw(MULTIFUNC_CNTL, SCISSORS_L | pboxInit->x1);
            outw(MULTIFUNC_CNTL, SCISSORS_R | (pboxInit->x2 - 1));
            outw(MULTIFUNC_CNTL, SCISSORS_T | pboxInit->y1);
            outw(MULTIFUNC_CNTL, SCISSORS_B | (pboxInit->y2 - 1));

            xorg = pDrawable->x;
            yorg = pDrawable->y;
            pSegtmp = pSeg;
            nsegtmp = nseg;

            while (nsegtmp--)
            {
                x1 = pSegtmp->x1 + xorg;
                y1 = pSegtmp->y1 + yorg;
                x2 = pSegtmp->x2 + xorg;
                y2 = pSegtmp->y2 + yorg;
                pSegtmp++;
                oc1 = 0;
                OUTCODES2(oc1, x1, y1, 0, 0, 1024, 1024); /* screenboundaries */
                oc2 = 0;
                OUTCODES2(oc2, x2, y2, 0, 0, 1024, 1024); /* screenboundaries */

                if ((oc1 || oc2) && !(oc1 & oc2))
                {
                    /*
                     * let the mi helper routine do our work;
                     * better than duplicating code...
                     */
                    int err;             /* modified bresenham error term */
                    int clip1=0, clip2=0;/* clippedness of the endpoints */
                    int clipdx, clipdy;  /* difference between clipped and
                                            unclipped start point */
		    int new_x1 = x1, new_y1 = y1, new_x2 = x2, new_y2 = y2;

                    cmd2 = cmd;
                    signdx = 1;
                    if ((adx = x2 - x1) < 0)
                    {
                        adx = -adx;
                        signdx = -1;
                        fix = 0;
                    } else
                    {
                        cmd2 |= INC_X;
                        fix = -1;
                    }
                    signdy = 1;
                    if ((ady = y2 - y1) < 0)
                    {
                        ady = -ady;
                        signdy = -1;
                    } else
                        cmd2 |= INC_Y;

                    if (adx > ady)
                    {
                        axis = X_AXIS;
                        e1 = ady << 1;
                        e2 = e1 - (adx << 1);
                        e = e1 - adx;
                    } else
                    {
                        axis = Y_AXIS;
                        e1 = adx << 1;
                        e2 = e1 - (ady << 1);
                        e = e1 - ady;
                        cmd2 |= YMAJAXIS;
                    }

                    if (miZeroClipLine(pboxInit->x1, pboxInit->y1,
					pboxInit->x2-1, pboxInit->y2-1,
					&new_x1, &new_y1,
					&new_x2, &new_y2,
					adx, ady,
					&clip1, &clip2,
					axis, (signdx == signdy),
					oc1, oc2) == -1)
                    {
                        pboxInit++;
                        continue;
                    }
                    if (axis == X_AXIS)
                        len = abs(new_x2 - new_x1);
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

                        WaitQueue(7);
                        outw(CUR_X, (short)new_x1);
                        outw(CUR_Y, (short)new_y1);
                        outw(ERR_TERM, (short)(err + fix));
                        outw(DESTY_AXSTP, (short)e1);
                        outw(DESTX_DIASTP, (short)e2);
                        outw(MAJ_AXIS_PCNT, (short)len);
                        outw(CMD, cmd2);
                    }
                } else {
                    WaitQueue(5);
                    outw(LINEDRAW_INDEX, 0);
                    outw(LINEDRAW, x1);
                    outw(LINEDRAW, y1);
                    outw(LINEDRAW, x2);
                    outw(LINEDRAW, y2);
                }
            }
            pboxInit++;
        }

        WaitQueue(7);
        outw(LINEDRAW_OPT, 0);
        outw(MULTIFUNC_CNTL, SCISSORS_L | 0);
        outw(MULTIFUNC_CNTL, SCISSORS_R | 1023);
        outw(MULTIFUNC_CNTL, SCISSORS_T | 0);
        outw(MULTIFUNC_CNTL, SCISSORS_B | 1023);
        outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
        outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);

    } else { /* original routine iterates clipboxes against segments */

        WaitQueue(3);
        outw(FRGD_MIX, FSS_FRGDCOL | mach8alu[pGC->alu]);
        outw(WRT_MASK, (short)pGC->planemask);
        outw(FRGD_COLOR, (short)pGC->fgPixel);

        xorg = pDrawable->x;
        yorg = pDrawable->y;

        while (nseg--)
        {
            nbox = nboxInit;
            pbox = pboxInit;

            x1 = pSeg->x1 + xorg;
            y1 = pSeg->y1 + yorg;
            x2 = pSeg->x2 + xorg;
            y2 = pSeg->y2 + yorg;
            pSeg++;

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
                    if (pGC->capStyle != CapNotLast)
                        y1--;
                }
                else if (pGC->capStyle != CapNotLast)
                    y2++;
                /* get to first band that might contain part of line */
                while ((nbox) && (pbox->y2 <= y1))
                {
                    pbox++;
                    nbox--;
                }

                if (nbox)
                {
                    /* stop when lower edge of box is beyond end of line */
                    while((nbox) && (y2 >= pbox->y1))
                    {
                        if ((x1 >= pbox->x1) && (x1 < pbox->x2))
                        {
                            int y1t, y2t;
                            /* this box has part of the line in it */
                            y1t = max(y1, pbox->y1);
                            y2t = min(y2, pbox->y2);
                            if (y1t != y2t)
                            {
                                WaitQueue(4);
                                outw(CUR_X, (short)x1);
                                outw(CUR_Y, (short)y1t);
                                outw(MAJ_AXIS_PCNT, (short)(y2t-y1t-1));
                                outw(CMD, CMD_LINE | DRAW | LINETYPE | PLANAR |
                                          WRTDATA | (6 << 5));
                            }
                        }
                        nbox--;
                        pbox++;
                    }
                }
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
                    if (pGC->capStyle != CapNotLast)
                        x1--;
                }
                else if (pGC->capStyle != CapNotLast)
                    x2++;

                /* find the correct band */
                while( (nbox) && (pbox->y2 <= y1))
                {
                    pbox++;
                    nbox--;
                }

                /* try to draw the line, if we haven't gone beyond it */
                if ((nbox) && (pbox->y1 <= y1))
                {
                    /* when we leave this band, we're done */
                    tmp = pbox->y1;
                    while((nbox) && (pbox->y1 == tmp))
                    {
                        int x1t, x2t;

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
                            WaitQueue(4);
                            outw(CUR_X, (short)x1t);
                            outw(CUR_Y, (short)y1);
                            outw(MAJ_AXIS_PCNT, (short)(x2t-x1t-1));
                            outw(CMD, CMD_LINE | DRAW | LINETYPE | PLANAR |
                                      WRTDATA);
                        }
                        nbox--;
                        pbox++;
                    }
                }
            }
            else        /* sloped line */
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
                        if (axis == X_AXIS)
                            len = adx;
                        else
                            len = ady;
                        if (pGC->capStyle != CapNotLast)
                            len++;

                        /* NOTE:  The 8514/A hardware routines for generating
                           lines do not match the software generated lines
                           of mi, cfb, and mfb.  This is a problem, and if I
                           ever get time, I'll figure out the 8514/A algorithm
                           and implement it in software for mi, cfb, and mfb.
                           14-sep-93 TCG: apparently only change needed is
                                          addition of 'fix' stuff in cfbline.c
                        */
                        WaitQueue(7);
                        outw(CUR_X, (short)x1);
                        outw(CUR_Y, (short)y1);
                        outw(ERR_TERM, (short)(e + fix));
                        outw(DESTY_AXSTP, (short)e1);
                        outw(DESTX_DIASTP, (short)e2);
                        outw(MAJ_AXIS_PCNT, (short)len);
                        outw(CMD, cmd2);

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
                        int clipdx, clipdy;  /* difference between clipped and
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

                            WaitQueue(7);
                            outw(CUR_X, (short)new_x1);
                            outw(CUR_Y, (short)new_y1);
                            outw(ERR_TERM, (short)(err + fix));
                            outw(DESTY_AXSTP, (short)e1);
                            outw(DESTX_DIASTP, (short)e2);
                            outw(MAJ_AXIS_PCNT, (short)len);
                            outw(CMD, cmd2);
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
}

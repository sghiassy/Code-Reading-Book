/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgalined.c,v 3.3 1996/12/23 06:59:51 dawes Exp $ */
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
/* $XConsortium: vgalined.c /main/4 1996/02/21 18:11:24 kaleb $ */

#include "vga256.h"
#include "miline.h"

/*
 * Draw dashed 1-pixel lines.
 */

void
#ifdef POLYSEGMENT
vga256SegmentSD (pDrawable, pGC, nseg, pSeg)
    DrawablePtr	pDrawable;
    register GCPtr	pGC;
    int		nseg;
    register xSegment	*pSeg;
#else
vga256LineSD( pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable;
    register GCPtr pGC;
    int mode;		/* Origin or Previous */
    int npt;		/* number of points */
    DDXPointPtr pptInit;
#endif
{
    int nboxInit;
    register int nbox;
    BoxPtr pboxInit;
    register BoxPtr pbox;
#ifndef POLYSEGMENT
    register DDXPointPtr ppt;	/* pointer to list of translated points */
#endif

    register unsigned int oc1;		/* outcode of point 1 */
    register unsigned int oc2;		/* outcode of point 2 */

    unsigned long *addrl;	/* address of destination pixmap */
    int nlwidth;		/* width in longwords of destination pixmap */
    int xorg, yorg;		/* origin of window */

    int adx;		/* abs values of dx and dy */
    int ady;
    int signdx;		/* sign of dx and dy */
    int signdy;
    int e, e1, e2;		/* bresenham error and increments */
    int len;			/* length of segment */
    int axis;			/* major axis */
    int octant;
    unsigned int bias = miGetZeroLineBias(pDrawable->pScreen);
    int x1, x2, y1, y2;
    RegionPtr cclip;
    cfbRRopRec	    rrops[2];
    unsigned char   *pDash;
    int		    dashOffset;
    int		    numInDashList;
    int		    dashIndex;
    int		    isDoubleDash;
    int		    dashIndexTmp, dashOffsetTmp;
    int		    unclippedlen;
    cfbPrivGCPtr    devPriv;

    if( pDrawable->type != DRAWABLE_WINDOW )
	{
#ifdef POLYSEGMENT
	cfbSegmentSD(pDrawable, pGC, nseg, pSeg);
#else
	cfbLineSD(pDrawable, pGC, mode, npt, pptInit);
#endif
	return;
	}

    devPriv = cfbGetGCPrivate(pGC);
    cclip = devPriv->pCompositeClip;
    rrops[0].rop = devPriv->rop;
    rrops[0].and = devPriv->and;
    rrops[0].xor = devPriv->xor;
    if (pGC->alu == GXcopy)
    {
	rrops[1].rop = GXcopy;
	rrops[1].and = 0;
	rrops[1].xor = PFILL (pGC->bgPixel);
    }
    else
    {
    	rrops[1].rop = cfbReduceRasterOp (pGC->alu,
					  pGC->bgPixel, pGC->planemask,
					  &rrops[1].and, &rrops[1].xor);
    }
    pboxInit = REGION_RECTS(cclip);
    nboxInit = REGION_NUM_RECTS(cclip);

    cfbGetLongWidthAndPointer (pDrawable, nlwidth, addrl)

    BANK_FLAG(addrl)

    /* compute initial dash values */
     
    pDash = (unsigned char *) pGC->dash;
    numInDashList = pGC->numInDashList;
    isDoubleDash = (pGC->lineStyle == LineDoubleDash);
    dashIndex = 0;
    dashOffset = 0;
    miStepDash ((int)pGC->dashOffset, &dashIndex, pDash,
		numInDashList, &dashOffset);

    xorg = pDrawable->x;
    yorg = pDrawable->y;
#ifdef POLYSEGMENT
    while (nseg--)
#else
    ppt = pptInit;
    x2 = ppt->x + xorg;
    y2 = ppt->y + yorg;
    while(--npt)
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
	if (mode == CoordModePrevious)
	{
	    xorg = x1;
	    yorg = y1;
	}
	x2 = ppt->x + xorg;
	y2 = ppt->y + yorg;
#endif

	CalcLineDeltas(x1, y1, x2, y2, adx, ady, signdx, signdy, 1, 1, octant);

	if (adx > ady)
	{
	    axis = X_AXIS;
	    e1 = ady << 1;
	    e2 = e1 - (adx << 1);
	    e = e1 - adx;
	    unclippedlen = adx;
	}
	else
	{
	    axis = Y_AXIS;
	    e1 = adx << 1;
	    e2 = e1 - (ady << 1);
	    e = e1 - ady;
	    unclippedlen = ady;
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
#ifdef POLYSEGMENT
		if (pGC->capStyle != CapNotLast)
		    unclippedlen++;
		dashIndexTmp = dashIndex;
		dashOffsetTmp = dashOffset;
		vga256BresD (rrops,
		      &dashIndexTmp, pDash, numInDashList,
		      &dashOffsetTmp, isDoubleDash,
		      addrl, nlwidth,
		      signdx, signdy, axis, x1, y1,
		      e, e1, e2, unclippedlen);
		break;
#else
		vga256BresD (rrops,
		      &dashIndex, pDash, numInDashList,
		      &dashOffset, isDoubleDash,
		      addrl, nlwidth,
		      signdx, signdy, axis, x1, y1,
		      e, e1, e2, unclippedlen);
		goto dontStep;
#endif
	    }
	    else if (oc1 & oc2)
	    {
		pbox++;
	    }
	    else /* have to clip */
	    {
		int new_x1 = x1, new_y1 = y1, new_x2 = x2, new_y2 = y2;
		int clip1 = 0, clip2 = 0;
		int clipdx, clipdy;
		int err;
    
		if (miZeroClipLine (pbox->x1, pbox->y1, pbox->x2-1, 
				    pbox->y2-1,
				    &new_x1, &new_y1, &new_x2, &new_y2,
				    adx, ady, &clip1, &clip2,
				    octant, bias, oc1, oc2) == -1)
		{
		    pbox++;
		    continue;
		}
		dashIndexTmp = dashIndex;
		dashOffsetTmp = dashOffset;
		if (clip1)
		{
		    	int dlen;
    
		    	if (axis == X_AXIS)
			    dlen = abs(new_x1 - x1);
		    	else
			    dlen = abs(new_y2 - y1);
		    	miStepDash (dlen, &dashIndexTmp, pDash,
				    numInDashList, &dashOffsetTmp);
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
		    vga256BresD (rrops,
		   	  &dashIndexTmp, pDash, numInDashList,
		   	  &dashOffsetTmp, isDoubleDash,
		   	  addrl, nlwidth,
		   	  signdx, signdy, axis, new_x1, new_y1,
		   	  err, e1, e2, len);
		}
		pbox++;
	    }
	} /* while (nbox--) */
#ifndef POLYSEGMENT
	/*
	 * walk the dash list around to the next line
	 */
	miStepDash (unclippedlen, &dashIndex, pDash,
		    numInDashList, &dashOffset);
dontStep:	;
#endif
    } /* while (nline--) */

#ifndef POLYSEGMENT
    /* paint the last point if the end style isn't CapNotLast.
       (Assume that a projecting, butt, or round cap that is one
        pixel wide is the same as the single pixel of the endpoint.)
    */

    if ((pGC->capStyle != CapNotLast) &&
        ((dashIndex & 1) == 0 || isDoubleDash) &&
	((ppt->x + xorg != pptInit->x + pDrawable->x) ||
	 (ppt->y + yorg != pptInit->y + pDrawable->y) ||
	 (ppt == pptInit + 1)))
    {
	nbox = nboxInit;
	pbox = pboxInit;
	while (nbox--)
	{
	unsigned long *addrb;	/* address of destination pixmap */
	    if ((x2 >= pbox->x1) &&
		(y2 >= pbox->y1) &&
		(x2 <  pbox->x2) &&
		(y2 <  pbox->y2))
	    {
		unsigned long	mask;
		int		pix;

		pix = 0;
		if (dashIndex & 1)
		    pix = 1;
		mask = cfbmask[x2 & PIM];
		addrl += (y2 * nlwidth) + (x2 >> PWSH);
		addrb = addrl; SETRW(addrb);
		*addrb = DoMaskRRop (*addrb, rrops[pix].and, rrops[pix].xor, mask);
		break;
	    }
	    else
		pbox++;
	}
    }
#endif
}

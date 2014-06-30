/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/cfb.w32/w32zerarc.c,v 3.4 1996/12/23 06:35:06 dawes Exp $ */
/************************************************************

Copyright (c) 1989  X Consortium

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

********************************************************/
/* $XConsortium: w32zerarc.c /main/4 1996/02/21 17:21:36 kaleb $ */

/* Derived from:
 * "Algorithm for drawing ellipses or hyperbolae with a digital plotter"
 * by M. L. V. Pitteway
 * The Computer Journal, November 1967, Volume 10, Number 3, pp. 282-289
 */

#include "X.h"
#include "Xprotostr.h"
#include "miscstruct.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "cfb.h"
#include "cfbmskbits.h"
#include "mizerarc.h"
#include "cfbrrop.h"
#include "mi.h"

#include "w32.h"

#ifdef PIXEL_ADDR

#define __RROP_SOLID(DST) \
{ \
    W32_SET(DST) \
    RROP_SOLID(W32Ptr); \
}

static void
RROP_NAME(w32ZeroArcSS8) (pDraw, pGC, arc)
    DrawablePtr pDraw;
    GCPtr pGC;
    xArc *arc;
{
    miZeroArcRec info;
    Bool do360;
    register int x;
    PixelType *addrp;
    register PixelType *yorgp, *yorgop;
    RROP_DECLARE
    register int yoffset;
    int npwidth, dyoffset;
    register int y, a, b, d, mask;
    register int k1, k3, dx, dy;

    cfbGetPixelWidthAndPointer(pDraw,npwidth, addrp)
    addrp = 0;

    RROP_FETCH_GC (pGC);
    do360 = miZeroArcSetup(arc, &info, TRUE);
    yorgp = addrp + ((info.yorg + pDraw->y) * npwidth);
    yorgop = addrp + ((info.yorgo + pDraw->y) * npwidth);
    info.xorg += pDraw->x;
    info.xorgo += pDraw->x;
    MIARCSETUP();
    yoffset = y ? npwidth : 0;
    dyoffset = 0;
    mask = info.initialMask;
    if (!(arc->width & 1))
    {
	if (mask & 2)
	    __RROP_SOLID((yorgp + info.xorgo));
	if (mask & 8)
	    __RROP_SOLID((yorgop + info.xorgo));
    }
    if (!info.end.x || !info.end.y)
    {
	mask = info.end.mask;
	info.end = info.altend;
    }
    if (do360 && (arc->width == arc->height) && !(arc->width & 1))
    {
	register int xoffset = npwidth;
	PixelType *yorghb = yorgp + (info.h * npwidth) + info.xorg;
	PixelType *yorgohb = yorghb - info.h;

	yorgp += info.xorg;
	yorgop += info.xorg;
	yorghb += info.h;
	while (1)
	{
	    __RROP_SOLID(yorgp + yoffset + x);
	    __RROP_SOLID(yorgp + yoffset - x);
	    __RROP_SOLID(yorgop - yoffset - x);
	    __RROP_SOLID(yorgop - yoffset + x);
	    if (a < 0)
		break;
	    __RROP_SOLID(yorghb - xoffset - y);
	    __RROP_SOLID(yorgohb - xoffset + y);
	    __RROP_SOLID(yorgohb + xoffset + y);
	    __RROP_SOLID(yorghb + xoffset - y);
	    xoffset += npwidth;
	    MIARCCIRCLESTEP(yoffset += npwidth;);
	}
	yorgp -= info.xorg;
	yorgop -= info.xorg;
	x = info.w;
	yoffset = info.h * npwidth;
    }
    else if (do360)
    {
	while (y < info.h || x < info.w)
	{
	    MIARCOCTANTSHIFT(dyoffset = npwidth;);
	    __RROP_SOLID(yorgp + yoffset + info.xorg + x);
	    __RROP_SOLID(yorgp + yoffset + info.xorgo - x);
	    __RROP_SOLID(yorgop - yoffset + info.xorgo - x);
	    __RROP_SOLID(yorgop - yoffset + info.xorg + x);
	    MIARCSTEP(yoffset += dyoffset;, yoffset += npwidth;);
	}
    }
    else
    {
	while (y < info.h || x < info.w)
	{
	    MIARCOCTANTSHIFT(dyoffset = npwidth;);
	    if ((x == info.start.x) || (y == info.start.y))
	    {
		mask = info.start.mask;
		info.start = info.altstart;
	    }
	    if (mask & 1)
		__RROP_SOLID(yorgp + yoffset + info.xorg + x);
	    if (mask & 2)
		__RROP_SOLID(yorgp + yoffset + info.xorgo - x);
	    if (mask & 4)
		__RROP_SOLID(yorgop - yoffset + info.xorgo - x);
	    if (mask & 8)
		__RROP_SOLID(yorgop - yoffset + info.xorg + x);
	    if ((x == info.end.x) || (y == info.end.y))
	    {
		mask = info.end.mask;
		info.end = info.altend;
	    }
	    MIARCSTEP(yoffset += dyoffset;, yoffset += npwidth;);
	}
    }
    if ((x == info.start.x) || (y == info.start.y))
	mask = info.start.mask;
    if (mask & 1)
	__RROP_SOLID(yorgp + yoffset + info.xorg + x);
    if (mask & 4)
	__RROP_SOLID(yorgop - yoffset + info.xorgo - x);
    if (arc->height & 1)
    {
	if (mask & 2)
	    __RROP_SOLID(yorgp + yoffset + info.xorgo - x);
	if (mask & 8)
	    __RROP_SOLID(yorgop - yoffset + info.xorg + x);
    }
}

void
RROP_NAME (W32ZeroPolyArcSS8) (pDraw, pGC, narcs, parcs)
    register DrawablePtr	pDraw;
    GCPtr	pGC;
    int		narcs;
    xArc	*parcs;
{
    register xArc *arc;
    register int i;
    BoxRec box;
    RegionPtr cclip;
    PixelType *addrp;
    int npwidth;

    CHECK_NOOP

    cfbGetPixelWidthAndPointer(pDraw,npwidth, addrp)
    if ((CARD32)addrp != VGABASE)
    {
	RROP_NAME (cfbZeroPolyArcSS8) (pDraw, pGC, narcs, parcs);
        return;
    }

    cclip = cfbGetCompositeClip(pGC);

    for (arc = parcs, i = narcs; --i >= 0; arc++)
    {
	if (miCanZeroArc(arc))
	{
	    box.x1 = arc->x + pDraw->x;
	    box.y1 = arc->y + pDraw->y;
	    box.x2 = box.x1 + (int)arc->width + 1;
	    box.y2 = box.y1 + (int)arc->height + 1;
	    if (RECT_IN_REGION(pDraw->pScreen, cclip, &box) == rgnIN)
		RROP_NAME (w32ZeroArcSS8) (pDraw, pGC, arc);
	    else
		miZeroPolyArc(pDraw, pGC, 1, arc);
	}
	else
	    miPolyArc(pDraw, pGC, 1, arc);
    }
}

#endif

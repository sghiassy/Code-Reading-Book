/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgazerarc.c,v 3.3 1996/12/23 07:00:06 dawes Exp $ */
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

/* $XConsortium: vgazerarc.c /main/3 1996/02/21 18:12:21 kaleb $ */

/* Derived from:
 * "Algorithm for drawing ellipses or hyperbolae with a digital plotter"
 * by M. L. V. Pitteway
 * The Computer Journal, November 1967, Volume 10, Number 3, pp. 282-289
 */

#include "vga256.h"
#include "mizerarc.h"
#include "cfbrrop.h"

#define __RROP_SOLID(a) { register unsigned char *addrbt = (a); \
			  SETRW(addrbt); RROP_SOLID(addrbt); }
#ifdef PIXEL_ADDR

static void
RROP_NAME(vga256ZeroArcSS8) (pDraw, pGC, arc)
    DrawablePtr pDraw;
    GCPtr pGC;
    xArc *arc;
{
    miZeroArcRec info;
    Bool do360;
    register int x;
    unsigned char *addrb;
    register unsigned char *yorgb, *yorgob;
    RROP_DECLARE
    register int yoffset;
    int nbwidth, dyoffset;
    register int y, a, b, d, mask;
    register int k1, k3, dx, dy;

    cfbGetByteWidthAndPointer(pDraw,nbwidth, addrb)

    BANK_FLAG(addrb)

    RROP_FETCH_GC (pGC);
    do360 = miZeroArcSetup(arc, &info, TRUE);
    yorgb = addrb + ((info.yorg + pDraw->y) * nbwidth);
    yorgob = addrb + ((info.yorgo + pDraw->y) * nbwidth);
    info.xorg += pDraw->x;
    info.xorgo += pDraw->x;
    MIARCSETUP();
    yoffset = y ? nbwidth : 0;
    dyoffset = 0;
    mask = info.initialMask;
    if (!(arc->width & 1))
    {
	if (mask & 2)
	    __RROP_SOLID((yorgb + info.xorgo));
	if (mask & 8)
	    __RROP_SOLID((yorgob + info.xorgo));
    }
    if (!info.end.x || !info.end.y)
    {
	mask = info.end.mask;
	info.end = info.altend;
    }
    if (do360 && (arc->width == arc->height) && !(arc->width & 1))
    {
	register int xoffset = nbwidth;
	unsigned char *yorghb = yorgb + (info.h * nbwidth) + info.xorg;
	unsigned char *yorgohb = yorghb - info.h;

	yorgb += info.xorg;
	yorgob += info.xorg;
	yorghb += info.h;
	while (1)
	{
	    __RROP_SOLID(yorgb + yoffset + x);
	    __RROP_SOLID(yorgb + yoffset - x);
	    __RROP_SOLID(yorgob - yoffset - x);
	    __RROP_SOLID(yorgob - yoffset + x);
	    if (a < 0)
		break;
	    __RROP_SOLID(yorghb - xoffset - y);
	    __RROP_SOLID(yorgohb - xoffset + y);
	    __RROP_SOLID(yorgohb + xoffset + y);
	    __RROP_SOLID(yorghb + xoffset - y);
	    xoffset += nbwidth;
	    MIARCCIRCLESTEP(yoffset += nbwidth;);
	}
	yorgb -= info.xorg;
	yorgob -= info.xorg;
	x = info.w;
	yoffset = info.h * nbwidth;
    }
    else if (do360)
    {
	while (y < info.h || x < info.w)
	{
	    MIARCOCTANTSHIFT(dyoffset = nbwidth;);
	    __RROP_SOLID(yorgb + yoffset + info.xorg + x);
	    __RROP_SOLID(yorgb + yoffset + info.xorgo - x);
	    __RROP_SOLID(yorgob - yoffset + info.xorgo - x);
	    __RROP_SOLID(yorgob - yoffset + info.xorg + x);
	    MIARCSTEP(yoffset += dyoffset;, yoffset += nbwidth;);
	}
    }
    else
    {
	while (y < info.h || x < info.w)
	{
	    MIARCOCTANTSHIFT(dyoffset = nbwidth;);
	    if ((x == info.start.x) || (y == info.start.y))
	    {
		mask = info.start.mask;
		info.start = info.altstart;
	    }
	    if (mask & 1)
		__RROP_SOLID(yorgb + yoffset + info.xorg + x);
	    if (mask & 2)
		__RROP_SOLID(yorgb + yoffset + info.xorgo - x);
	    if (mask & 4)
		__RROP_SOLID(yorgob - yoffset + info.xorgo - x);
	    if (mask & 8)
		__RROP_SOLID(yorgob - yoffset + info.xorg + x);
	    if ((x == info.end.x) || (y == info.end.y))
	    {
		mask = info.end.mask;
		info.end = info.altend;
	    }
	    MIARCSTEP(yoffset += dyoffset;, yoffset += nbwidth;);
	}
    }
    if ((x == info.start.x) || (y == info.start.y))
	mask = info.start.mask;
    if (mask & 1)
	__RROP_SOLID(yorgb + yoffset + info.xorg + x);
    if (mask & 4)
	__RROP_SOLID(yorgob - yoffset + info.xorgo - x);
    if (arc->height & 1)
    {
	if (mask & 2)
	    __RROP_SOLID(yorgb + yoffset + info.xorgo - x);
	if (mask & 8)
	    __RROP_SOLID(yorgob - yoffset + info.xorg + x);
    }
}

void
RROP_NAME (vga256ZeroPolyArcSS8) (pDraw, pGC, narcs, parcs)
    register DrawablePtr	pDraw;
    GCPtr	pGC;
    int		narcs;
    xArc	*parcs;
{
    register xArc *arc;
    register int i;
    BoxRec box;
    RegionPtr cclip;

    cclip = cfbGetCompositeClip(pGC);
    for (arc = parcs, i = narcs; --i >= 0; arc++)
    {
	if (miCanZeroArc(arc))
	{
	    box.x1 = arc->x + pDraw->x;
	    box.y1 = arc->y + pDraw->y;
	    box.x2 = box.x1 + (int)arc->width + 1;
	    box.y2 = box.y1 + (int)arc->height + 1;
	    if ((*pDraw->pScreen->RectIn)(cclip, &box) == rgnIN)
		RROP_NAME (vga256ZeroArcSS8) (pDraw, pGC, arc);
	    else
		miZeroPolyArc(pDraw, pGC, 1, arc);
	}
	else
	    miPolyArc(pDraw, pGC, 1, arc);
    }
}

#endif

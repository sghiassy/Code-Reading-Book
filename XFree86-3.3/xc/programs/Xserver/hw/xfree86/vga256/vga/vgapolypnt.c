/* $XConsortium: vgapolypnt.c /main/4 1996/02/21 18:11:36 kaleb $ */
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

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgapolypnt.c,v 3.6 1996/12/23 06:59:54 dawes Exp $ */

#include "vga256.h"

#define isClipped(c,ul,lr)  ((((c) - (ul)) | ((lr) - (c))) & ClipMask)

#define PointLoop(fill) { \
    for (nbox = REGION_NUM_RECTS(cclip), pbox = REGION_RECTS(cclip); \
	 --nbox >= 0; \
	 pbox++) \
    { \
	c1 = *((INT32 *) &pbox->x1) - off; \
	c2 = *((INT32 *) &pbox->x2) - off - 0x00010001; \
	for (ppt = (INT32 *) pptInit, i = npt; --i >= 0;) \
	{ \
	    pt = *ppt++; \
	    if (!isClipped(pt,c1,c2)) { \
		fill \
	    } \
	} \
    } \
}

void
vga256PolyPoint(pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable;
    GCPtr pGC;
    int mode;
    int npt;
    xPoint *pptInit;
{
    register INT32   pt;
    register INT32   c1, c2;
#if defined(PC98_WAB) || defined(PC98_WABEP)
    register unsigned long   ClipMask = 0x40004000;
#else
    register unsigned long   ClipMask = 0x80008000;
#endif
    register unsigned long   xor;
    register unsigned char   *addrb;
    register int    nbwidth;
    unsigned char   *addrbt;
    register INT32  *ppt;
    RegionPtr	    cclip;
    int		    nbox;
    register int    i;
    register BoxPtr pbox;
    unsigned long   and;
    int		    rop = pGC->alu;
    int		    off;
    cfbPrivGCPtr    devPriv;
    xPoint	    *pptPrev;

    devPriv = cfbGetGCPrivate(pGC);
    rop = devPriv->rop;
    if (rop == GXnoop)
	return;
    cclip = devPriv->pCompositeClip;
    xor = devPriv->xor;
    if ((mode == CoordModePrevious) && (npt > 1))
    {
	for (pptPrev = pptInit + 1, i = npt - 1; --i >= 0; pptPrev++)
	{
	    pptPrev->x += (pptPrev-1)->x;
	    pptPrev->y += (pptPrev-1)->y;
	}
    }
    off = *((int *) &pDrawable->x);
#if defined(PC98_WAB) || defined(PC98_WABEP)
    off -= (off & 0x4000) << 1;
#else
    off -= (off & 0x8000) << 1;
#endif
    cfbGetByteWidthAndPointer(pDrawable, nbwidth, addrb);

    BANK_FLAG(addrb)

    addrb = addrb + pDrawable->y * nbwidth + pDrawable->x;
    if (rop == GXcopy)
    {
	if (!(nbwidth & (nbwidth - 1)))
	{
	    nbwidth = ffs(nbwidth) - 1;
	    PointLoop(addrbt = addrb + (intToY(pt) << nbwidth) + intToX(pt);
		      SETRW(addrbt); *addrbt = xor;)
	}
	else
	{
	    PointLoop(addrbt =  addrb + intToY(pt) * nbwidth + intToX(pt);
		      SETRW(addrbt); *addrbt = xor;)
	}
    }
    else
    {
	and = devPriv->and;
	PointLoop(  addrbt = addrb + intToY(pt) * nbwidth + intToX(pt);
		    SETRW(addrbt);
		    *addrbt = DoRRop (*addrbt, and, xor);)
    }
}

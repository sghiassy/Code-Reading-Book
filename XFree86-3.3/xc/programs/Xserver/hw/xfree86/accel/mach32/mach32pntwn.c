/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/mach32pntwn.c,v 3.8 1996/12/23 06:38:45 dawes Exp $ */
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
Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)

DIGITAL AND KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL DIGITAL OR KEVIN E. MARTIN BE LIABLE FOR
ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/
/* $XConsortium: mach32pntwn.c /main/6 1996/02/21 17:27:22 kaleb $ */

#include "X.h"

#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "cfb.h"
#include "cfb16.h"
#include "cfbmskbits.h"
#include "mach32.h"

extern void miPaintWindow();

void
mach32PaintWindow(pWin, pRegion, what)
    WindowPtr	pWin;
    RegionPtr	pRegion;
    int		what;
{
    register cfbPrivWin	*pPrivWin;
    void (*pcfbFillBoxTile32)(), (*pcfbFillBoxTileOdd)();
    WindowPtr pBgWin;

    if (!mach32Use4MbAperture)
    {
	miPaintWindow(pWin, pRegion, what);
	return;
    }

    pPrivWin = (cfbPrivWin *)(pWin->devPrivates[cfbWindowPrivateIndex].ptr);
    switch (pWin->drawable.bitsPerPixel) {
    case 8:
	pcfbFillBoxTile32 = cfbFillBoxTile32;
	pcfbFillBoxTileOdd = cfbFillBoxTileOdd;
	break;
    case 16:
	pcfbFillBoxTile32 = cfb16FillBoxTile32;
	pcfbFillBoxTileOdd = cfb16FillBoxTileOdd;
	break;
    }

    switch (what) {
    case PW_BACKGROUND:
	switch (pWin->backgroundState) {
	case None:
	    return;
	case ParentRelative:
	    do {
		pWin = pWin->parent;
	    } while (pWin->backgroundState == ParentRelative);
	    (*pWin->drawable.pScreen->PaintWindowBackground)(pWin, pRegion,
							     what);
	    return;
	case BackgroundPixmap:
	    if (pPrivWin->fastBackground)
	    {
		(*pcfbFillBoxTile32) ((DrawablePtr)pWin,
				  (int)REGION_NUM_RECTS(pRegion),
				  REGION_RECTS(pRegion),
				  pPrivWin->pRotatedBackground);
		return;
	    }
	    else
	    {
		(*pcfbFillBoxTileOdd) ((DrawablePtr)pWin,
				   (int)REGION_NUM_RECTS(pRegion),
				   REGION_RECTS(pRegion),
				   pWin->background.pixmap,
				   (int) pWin->drawable.x, (int) pWin->drawable.y);
		return;
	    }
	    break;
	case BackgroundPixel:
	    mach32FillBoxSolid ((DrawablePtr)pWin,
				(int)REGION_NUM_RECTS(pRegion),
				REGION_RECTS(pRegion),
				pWin->background.pixel);
	    return;
    	}
    	break;
    case PW_BORDER:
	if (pWin->borderIsPixel)
	{
	    mach32FillBoxSolid ((DrawablePtr)pWin,
				(int)REGION_NUM_RECTS(pRegion),
				REGION_RECTS(pRegion),
				pWin->border.pixel);
	    return;
	}
	else if (pPrivWin->fastBorder)
	{
	    (*pcfbFillBoxTile32) ((DrawablePtr)pWin,
			      (int)REGION_NUM_RECTS(pRegion),
			      REGION_RECTS(pRegion),
			      pPrivWin->pRotatedBorder);
	    return;
	}
	else if (pWin->border.pixmap->drawable.width >=
	    /* PPW/2 */ 16 / pWin->drawable.bitsPerPixel)
	{
	    for (pBgWin = pWin;
		 pBgWin->backgroundState == ParentRelative;
		 pBgWin = pBgWin->parent);

	    (*pcfbFillBoxTileOdd) ((DrawablePtr)pWin,
			       (int)REGION_NUM_RECTS(pRegion),
			       REGION_RECTS(pRegion),
			       pWin->border.pixmap,
			       (int) pBgWin->drawable.x,
			       (int) pBgWin->drawable.y);
	    return;
	}
	break;
    }
    miPaintWindow (pWin, pRegion, what);
}

void
mach32FillBoxSolid (pDrawable, nBox, pBox, pixel)
    DrawablePtr	    pDrawable;
    int		    nBox;
    BoxPtr	    pBox;
    unsigned long   pixel;
{
    register short w,h;

    WaitQueue(3);
    outw(FRGD_COLOR, (short)(pixel));
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(WRT_MASK, (short)~0);

    for (; nBox; nBox--, pBox++) {
	h = pBox->y2 - pBox->y1 - 1;
	w = pBox->x2 - pBox->x1 - 1;

	if ((w >= 0) && (h >= 0)) {
	    WaitQueue(5);
	    outw(CUR_X, (short)(pBox->x1));
	    outw(CUR_Y, (short)(pBox->y1));
	    outw(MAJ_AXIS_PCNT, (short)(w));
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(h));
	    outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);
	}
    }
    WaitQueue(2);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);

    WaitIdleEmpty(); /* Make sure that all commands have finished */
}

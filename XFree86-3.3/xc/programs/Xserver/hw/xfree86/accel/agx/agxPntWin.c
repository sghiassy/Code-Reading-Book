/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxPntWin.c,v 3.10 1996/12/23 06:32:55 dawes Exp $ */
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
Modified for the AGX by Henry A. Worth

DIGITAL, KEVIN E. MARTIN, AND HENRY A. WORTH DISCLAIM ALL WARRANTIES 
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE LIABLE
FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/
/* $XConsortium: agxPntWin.c /main/8 1996/02/21 17:18:19 kaleb $ */

#include "X.h"

#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "cfb.h"
#include "mi.h"

#include "agx.h"
#include "regagx.h"

extern void miPaintWindow();

static void agxFillBoxSolid ();

void
agxPaintWindow(pWin, pRegion, what)
    WindowPtr	pWin;
    RegionPtr	pRegion;
    int		what;
{
    register cfbPrivWin	*pPrivWin;
    void (*pcfbFillBoxTile32)(), (*pcfbFillBoxTileOdd)();

    if (!xf86VTSema)
    {
       miPaintWindow( pWin, pRegion, what );
       return;
    }

    pPrivWin = (cfbPrivWin *)(pWin->devPrivates[cfbWindowPrivateIndex].ptr);

    switch (what) {
    case PW_BACKGROUND:
	switch (pWin->backgroundState) {
	case None:
	    return;
	case ParentRelative:
	    do {
		pWin = pWin->parent;
	    } while (pWin->backgroundState == ParentRelative);
	    (*pWin->drawable.pScreen->
                   PaintWindowBackground)(pWin, pRegion, what);
	    return;
	case BackgroundPixmap:
	    if (pPrivWin->fastBackground) {
		agxFillBoxTile( (DrawablePtr)pWin,
				(int)REGION_NUM_RECTS(pRegion),
				REGION_RECTS(pRegion),
				pPrivWin->pRotatedBackground,
                                0, 0,
                                MIX_SRC, ~0 );
		return;
	    }
	    else {
		agxFillBoxTile( (DrawablePtr)pWin,
				(int)REGION_NUM_RECTS(pRegion),
				REGION_RECTS(pRegion),
				pWin->background.pixmap,
				(int) pWin->drawable.x, 
                                (int) pWin->drawable.y,
                                MIX_SRC, ~0 );
		return;
	    }
	    break;
	case BackgroundPixel:
	    agxFillBoxSolid( (DrawablePtr)pWin,
			     (int)REGION_NUM_RECTS(pRegion),
			     REGION_RECTS(pRegion),
			     pWin->background.pixel );
	    return;
    	}
    	break;
    case PW_BORDER:
	if (pWin->borderIsPixel)
	{
	    agxFillBoxSolid( (DrawablePtr)pWin,
			     (int)REGION_NUM_RECTS(pRegion),
			     REGION_RECTS(pRegion),
			     pWin->border.pixel );
	    return;
	}
	else if (pPrivWin->fastBorder)
	{
	    agxFillBoxTile( (DrawablePtr)pWin,
			    (int)REGION_NUM_RECTS(pRegion),
			    REGION_RECTS(pRegion),
			    pPrivWin->pRotatedBorder,
                            0, 0,
                            MIX_SRC, ~0 );
	    return;
	}
	else {
	    agxFillBoxTile( (DrawablePtr)pWin,
			    (int)REGION_NUM_RECTS(pRegion),
			    REGION_RECTS(pRegion),
			    pWin->border.pixmap,
			    (int) pWin->drawable.x, 
                            (int) pWin->drawable.y,
                            MIX_SRC, ~0 );
	    return;
	}
	break;
    }
    miPaintWindow (pWin, pRegion, what);
}

static void
agxFillBoxSolid (pDrawable, nBox, pBox, pixel)
    DrawablePtr	    pDrawable;
    int		    nBox;
    BoxPtr	    pBox;
    unsigned long   pixel;
{
    register short w,h;

    GE_WAIT_IDLE();

    MAP_SET_DST( GE_MS_MAP_A );

    GE_OUT_B( GE_FRGD_MIX, MIX_SRC );
    GE_OUT_D( GE_PIXEL_BIT_MASK, ~0 );
    GE_OUT_D( GE_FRGD_CLR, pixel );

    GE_OUT_W( GE_PIXEL_OP, 
              GE_OP_PAT_FRGD
              | GE_OP_MASK_DISABLED
              | GE_OP_INC_X
              | GE_OP_INC_Y         );

    for (; nBox; nBox--, pBox++) {
	h = pBox->y2 - pBox->y1 - 1;
	w = pBox->x2 - pBox->x1 - 1;

	if ((w >= 0) && (h >= 0)) {

           GE_WAIT_IDLE();
#ifndef NO_MULTI_IO
           GE_OUT_D( GE_DEST_MAP_X, pBox->y1 << 16
                                    | pBox->x1 );
           GE_OUT_D( GE_OP_DIM_WIDTH, h << 16 | w );
#else
           GE_OUT_W( GE_DEST_MAP_X, (short)(pBox->x1) );
           GE_OUT_W( GE_DEST_MAP_Y, (short)(pBox->y1) );
           GE_OUT_W( GE_OP_DIM_WIDTH, w );
           GE_OUT_W( GE_OP_DIM_HEIGHT, h );
#endif
           GE_START_CMDW( GE_OPW_BITBLT
                          | GE_OPW_FRGD_SRC_CLR
                          | GE_OPW_DEST_MAP_A   );
	}
    }
    GE_WAIT_IDLE_EXIT();
}

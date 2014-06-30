/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000win.c,v 3.4 1996/12/23 06:40:59 dawes Exp $ */
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

DIGITAL AND CHRIS MASON AND ERIK NYGREN AND KEVIN E. MARTIN AND
RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL DIGITAL OR ERIK NYGREN OR CHRIS MASON OR KEVIN E. MARTIN
BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
Modified for the P9000 by Chris Mason (mason@mail.csh.rit.edu)
Further modified for the P9000 by Erik Nygren (nygren@mit.edu)

*/
/* $XConsortium: p9000win.c /main/5 1996/02/21 17:33:29 kaleb $ */

#include "X.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "gcstruct.h"
#include "cfb.h"
#include "cfb16.h"
#include "mistruct.h"
#include "regionstr.h"
#include "cfbmskbits.h"

#include "p9000.h"
#include "p9000reg.h"

void 
p9000CopyWindow(pWin, ptOldOrg, prgnSrc)
    WindowPtr pWin;
    DDXPointRec ptOldOrg;
    RegionPtr prgnSrc;
{
    RegionPtr prgnDst;
    register BoxPtr prect, pboxOrig;
    register int dx, dy, engstatus ;
    register int i, nbox;
    short direction = 0;
    unsigned int *ordering;
    GC dummyGC;

    dummyGC.subWindowMode = ~IncludeInferiors;

    prgnDst = (* pWin->drawable.pScreen->RegionCreate)(NULL, 1);

    dx = ptOldOrg.x - pWin->drawable.x ;

    dy = ptOldOrg.y - pWin->drawable.y ;

    (* pWin->drawable.pScreen->TranslateRegion)(prgnSrc, -dx, -dy);
    (* pWin->drawable.pScreen->Intersect)(prgnDst, &pWin->borderClip, prgnSrc);

    pboxOrig = REGION_RECTS(prgnDst);
    nbox = REGION_NUM_RECTS(prgnDst);

    ordering = (unsigned int *) ALLOCATE_LOCAL(nbox * sizeof(unsigned int));
    if (!ordering) {
	(* pWin->drawable.pScreen->RegionDestroy)(prgnDst);
	return;
    }

    p9000FindOrdering(&pWin->drawable, &pWin->drawable, &dummyGC, nbox,
		       pboxOrig, ptOldOrg.x, ptOldOrg.y, pWin->drawable.x,
		       pWin->drawable.y, ordering);
    p9000NotBusy();

    p9000Store(RASTER, CtlBase, IGM_S_MASK);
    p9000Store(PMASK, CtlBase, ~0);

    for (i = 0; i < nbox; i++)
      {
	prect = &pboxOrig[ordering[i]];
#if 0
ErrorF("Moving rect: (%d,%d,%d,%d) -> (%d,%d,%d,%d) ", prect->x1+dx, prect->y1+dy, prect->x2+dx, prect->y2+dy-1, prect->x1, prect->y1, prect->x2, prect->y2-1 );
#endif
	/* Load the coordinates */
	/* for blits, YX_PACKing makes little difference */
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, 
		 CtlBase, p9000BytesPerPixel * (prect->x1+dx));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0,
		 CtlBase, prect->y1+dy);
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1,
		 CtlBase, (p9000BytesPerPixel * (prect->x2+dx))-1);
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1,
		 CtlBase, prect->y2+dy-1);
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2,
		 CtlBase, p9000BytesPerPixel * (prect->x1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2,
		 CtlBase, prect->y1);
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3,
		 CtlBase, (p9000BytesPerPixel * prect->x2)-1);
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3,
		 CtlBase, prect->y2-1);

	/* wait for engine and blit */
	do engstatus = p9000Fetch(CMD_BLIT, CtlBase);
	while (engstatus & SR_ISSUE_QBN);
    }
    DEALLOCATE_LOCAL(ordering);
    /* wait for last blit to finish */
    p9000QBNotBusy();
}

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/mach32win.c,v 3.7 1996/12/23 06:38:50 dawes Exp $ */
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

DIGITAL AND KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL DIGITAL OR KEVIN E. MARTIN
BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)

*/
/* $XConsortium: mach32win.c /main/5 1996/02/21 17:27:40 kaleb $ */

#include "X.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "gcstruct.h"
#include "cfb.h"
#include "cfb16.h"
#include "mistruct.h"
#include "regionstr.h"
#include "cfbmskbits.h"

#include "mach32.h"

void 
mach32CopyWindow(pWin, ptOldOrg, prgnSrc)
    WindowPtr pWin;
    DDXPointRec ptOldOrg;
    RegionPtr prgnSrc;
{
    RegionPtr prgnDst;
    register BoxPtr pbox, pboxOrig;
    register int dx, dy;
    register int i, nbox;
    short direction = 0;
    unsigned int *ordering;
    GC dummyGC;

    dummyGC.subWindowMode = ~IncludeInferiors;

    prgnDst = (* pWin->drawable.pScreen->RegionCreate)(NULL, 1);

    if ((dx = ptOldOrg.x - pWin->drawable.x) > 0)
	direction |= INC_X;

    if ((dy = ptOldOrg.y - pWin->drawable.y) > 0)
	direction |= INC_Y;

    (* pWin->drawable.pScreen->TranslateRegion)(prgnSrc, -dx, -dy);
    (* pWin->drawable.pScreen->Intersect)(prgnDst, &pWin->borderClip, prgnSrc);

    pboxOrig = REGION_RECTS(prgnDst);
    nbox = REGION_NUM_RECTS(prgnDst);

    ordering = (unsigned int *) ALLOCATE_LOCAL(nbox * sizeof(unsigned int));
    if (!ordering) {
	(* pWin->drawable.pScreen->RegionDestroy)(prgnDst);
	return;
    }

    mach32FindOrdering(&pWin->drawable, &pWin->drawable, &dummyGC, nbox,
		       pboxOrig, ptOldOrg.x, ptOldOrg.y, pWin->drawable.x,
		       pWin->drawable.y, ordering);

    WaitQueue(3);
    outw(FRGD_MIX, FSS_BITBLT | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
    outw(WRT_MASK, 0xffff);

    if (direction == (INC_X | INC_Y)) {
	for (i = 0; i < nbox; i++)
	{
	    pbox = &pboxOrig[ordering[i]];

	    WaitQueue(7);
	    outw(CUR_X, (short)(pbox->x1 + dx));
	    outw(CUR_Y, (short)(pbox->y1 + dy));
	    outw(DESTX_DIASTP, (short)(pbox->x1));
	    outw(DESTY_AXSTP, (short)(pbox->y1));
	    outw(MAJ_AXIS_PCNT, (short)(pbox->x2 - pbox->x1 - 1));
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(pbox->y2 - pbox->y1 - 1));
	    outw(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
	}
    } else if (direction == INC_X) {
	for (i = 0; i < nbox; i++)
	{
	    pbox = &pboxOrig[ordering[i]];

	    WaitQueue(7);
	    outw(CUR_X, (short)(pbox->x1 + dx));
	    outw(CUR_Y, (short)(pbox->y2 + dy - 1));
	    outw(DESTX_DIASTP, (short)(pbox->x1));
	    outw(DESTY_AXSTP, (short)(pbox->y2 - 1));
	    outw(MAJ_AXIS_PCNT, (short)(pbox->x2 - pbox->x1 - 1));
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(pbox->y2 - pbox->y1 - 1));
	    outw(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
	}
    } else if (direction == INC_Y) {
	for (i = 0; i < nbox; i++)
	{
	    pbox = &pboxOrig[ordering[i]];

	    WaitQueue(7);
	    outw(CUR_X, (short)(pbox->x2 + dx - 1));
	    outw(CUR_Y, (short)(pbox->y1 + dy));
	    outw(DESTX_DIASTP, (short)(pbox->x2 - 1));
	    outw(DESTY_AXSTP, (short)(pbox->y1));
	    outw(MAJ_AXIS_PCNT, (short)(pbox->x2 - pbox->x1 - 1));
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(pbox->y2 - pbox->y1 - 1));
	    outw(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
	}
    } else {
	for (i = 0; i < nbox; i++)
	{
	    pbox = &pboxOrig[ordering[i]];

	    WaitQueue(7);
	    outw(CUR_X, (short)(pbox->x2 + dx - 1));
	    outw(CUR_Y, (short)(pbox->y2 + dy - 1));
	    outw(DESTX_DIASTP, (short)(pbox->x2 - 1));
	    outw(DESTY_AXSTP, (short)(pbox->y2 - 1));
	    outw(MAJ_AXIS_PCNT, (short)(pbox->x2 - pbox->x1 - 1));
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(pbox->y2 - pbox->y1 - 1));
	    outw(CMD, CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
	}
    }

    WaitQueue(2);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);

    (* pWin->drawable.pScreen->RegionDestroy)(prgnDst);
    DEALLOCATE_LOCAL(ordering);

    WaitIdleEmpty(); /* Make sure that all commands have finished */
}


/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64win.c,v 3.3 1996/12/23 06:39:30 dawes Exp $ */
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.
Copyright 1993,1994 by Kevin E. Martin, Chapel Hill, North Carolina.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
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
Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)

******************************************************************/
/* $XConsortium: mach64win.c /main/3 1996/02/21 17:29:23 kaleb $ */

#include "X.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "gcstruct.h"
#include "cfb.h"
#include "mistruct.h"
#include "regionstr.h"
#include "cfbmskbits.h"
#include "cfb16.h"
#include "cfb32.h"
#include "mach64.h"

void 
mach64CopyWindow(pWin, ptOldOrg, prgnSrc)
    WindowPtr pWin;
    DDXPointRec ptOldOrg;
    RegionPtr prgnSrc;
{
    RegionPtr prgnDst;
    DDXPointPtr pptInit;
    register DDXPointPtr pPt;
    register BoxPtr pbox, pboxOrig;
    register int dx, dy;
    register int count; 
    register int nbox;
    WindowPtr pWinRoot;
    GC dummyGC;
    extern WindowPtr *WindowTable;

    if (!xf86VTSema)
    {
	switch(pWin->drawable.bitsPerPixel)
	{
	    case 8:
	        cfbCopyWindow(pWin, ptOldOrg, prgnSrc);
		break;
	    case 16:
	        cfb16CopyWindow(pWin, ptOldOrg, prgnSrc);
		break;
	    case 32:
	        cfb32CopyWindow(pWin, ptOldOrg, prgnSrc);
		break;
	    default:
		ErrorF("mach64CopyWindow: unsupported depth %d \n",
			pWin->drawable.bitsPerPixel);
	}
	return;
    }

    dummyGC.subWindowMode = ~IncludeInferiors;
    dummyGC.alu = GXcopy;
    dummyGC.planemask = 0xffffffff;

    pWinRoot = WindowTable[pWin->drawable.pScreen->myNum];

    prgnDst = (* pWin->drawable.pScreen->RegionCreate)(NULL, 1);

    dx = ptOldOrg.x - pWin->drawable.x;
    dy = ptOldOrg.y - pWin->drawable.y;

    (* pWin->drawable.pScreen->TranslateRegion)(prgnSrc, -dx, -dy);
    (* pWin->drawable.pScreen->Intersect)(prgnDst, &pWin->borderClip, prgnSrc);

    pboxOrig = REGION_RECTS(prgnDst);
    nbox = REGION_NUM_RECTS(prgnDst);

    pptInit = (DDXPointPtr )ALLOCATE_LOCAL(nbox * sizeof(DDXPointRec));
    if(!pptInit)
	return;

    pPt = pptInit;
    for (pbox = pboxOrig, count = nbox; --count >= 0; pPt++, pbox++)
    {
        pPt->x = pbox->x1 + dx;
        pPt->y = pbox->y1 + dy;
    }

    mach64DoBitBlt((DrawablePtr)pWinRoot, (DrawablePtr)pWinRoot,
                   &dummyGC, prgnDst, pptInit, 0);

    DEALLOCATE_LOCAL(pptInit);
    (* pWin->drawable.pScreen->RegionDestroy)(prgnDst);
}

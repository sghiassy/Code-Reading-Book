/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86window.c,v 3.0 1996/11/18 13:22:41 dawes Exp $ */


#include "X.h"
#include "gcstruct.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "mistruct.h"
#include "regionstr.h"

#include "xf86.h"
#include "xf86xaa.h"
#include "xf86local.h"

/*
 * This function replaces the ScreenRec CopyWindow function.
 * It is compiled only once, being depth-independent.
 *
 * It is only called if an intermediate screen-to-screen
 * BitBlt operations is defined (xf86AccelInfoRec.ScreenToScreenBitBlt
 * is valid).
 */

extern WindowPtr *WindowTable;

void 
xf86CopyWindow(pWin, ptOldOrg, prgnSrc)
    WindowPtr pWin;
    DDXPointRec ptOldOrg;
    RegionPtr prgnSrc;
{
    DDXPointPtr pptSrc;
    register DDXPointPtr ppt;
    RegionRec rgnDst;
    register BoxPtr pbox;
    register int dx, dy;
    register int i, nbox;
    WindowPtr pwinRoot;

    /* This is not a GC function, so we must be aware of the VT status. */
    if (!xf86VTSema) {
    	xf86GCInfoRec.OffScreenCopyWindowFallBack(pWin, ptOldOrg, prgnSrc);
    	return;
    }

    pwinRoot = WindowTable[pWin->drawable.pScreen->myNum];

    REGION_INIT(pWin->drawable.pScreen, &rgnDst, NullBox, 0);

    dx = ptOldOrg.x - pWin->drawable.x;
    dy = ptOldOrg.y - pWin->drawable.y;
    REGION_TRANSLATE(pWin->drawable.pScreen, prgnSrc, -dx, -dy);
    REGION_INTERSECT(pWin->drawable.pScreen, &rgnDst, &pWin->borderClip, prgnSrc);

    pbox = REGION_RECTS(&rgnDst);
    nbox = REGION_NUM_RECTS(&rgnDst);
    if(!nbox || !(pptSrc = (DDXPointPtr )ALLOCATE_LOCAL(nbox * sizeof(DDXPointRec))))
    {
	REGION_UNINIT(pWin->drawable.pScreen, &rgnDst);
	return;
    }
    ppt = pptSrc;

    for (i = nbox; --i >= 0; ppt++, pbox++)
    {
	ppt->x = pbox->x1 + dx;
	ppt->y = pbox->y1 + dy;
    }

    xf86DoBitBlt((DrawablePtr)pwinRoot, (DrawablePtr)pwinRoot,
        GXcopy, &rgnDst, pptSrc, ~0L, 1);
    DEALLOCATE_LOCAL(pptSrc);
    REGION_UNINIT(pWin->drawable.pScreen, &rgnDst);
}

/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86gc.c,v 3.7.2.1 1997/05/26 14:36:22 dawes Exp $ */

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

/*
 * The XFree86 Project, Inc.
 * xf86gc.c, based on cfbgc.c.
 *
 * This is compiled for 8, 16, 24, and 32bpp. Later support for vga256
 * can be added.
 *
 * Written by Harm Hanemaayer (H.Hanemaayer@inter.nl.net).
 */

/* $XConsortium: cfbgc.c,v 5.62 94/04/17 20:28:49 dpw Exp $ */

#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#ifdef VGA256
/*
 * VGA256 is a special case. We don't use cfb for non-accelerated
 * functions, but instead use their vga256 equivalents.
 */
#include "vga256.h"
#include "vga256map.h"
#else
#include "cfb.h"
#include "cfbmskbits.h"
#include "cfb8bit.h"
#endif
#include "fontstruct.h"
#include "dixfontstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "region.h"

#include "mistruct.h"
#include "mibstore.h"
#include "migc.h"

#include "xf86.h"
#include "xf86xaa.h"
#include "xf86gcmap.h"
#include "xf86maploc.h"
#include "xf86local.h"

static GCFuncs xf86GCFuncs = {
    xf86ValidateGC,
    miChangeGC,
    miCopyGC,
    miDestroyGC,
    miChangeClip,
    miDestroyClip,
    miCopyClip,
};

/*
 * The cfb Ops structures used by MatchCommon are not duplicated
 * here; they are defined by cfb16/24/32 or vga256. We also use the
 * externally defined version of MatchCommon. The name of cfbMatchCommon
 * is mapped here (cfb doesn't do it).
 */

#ifdef VGA256
#define MatchCommon vga256matchCommon
#define NonTEOps vga256NonTEOps
#else
#if PSZ == 8
#define MatchCommon cfbMatchCommon
#define NonTEOps cfbNonTEOps
#endif
#if PSZ == 16
#define MatchCommon cfb16MatchCommon
#define NonTEOps cfb16NonTEOps
#endif
#if PSZ == 24
#define MatchCommon cfb24MatchCommon
#define NonTEOps cfb24NonTEOps
#endif
#if PSZ == 32
#define MatchCommon cfb32MatchCommon
#define NonTEOps cfb32NonTEOps
#endif
#endif

extern GCOps NonTEOps;

extern GCOps *MatchCommon(
#if NeedFunctionPrototypes
    GCPtr	    pGC,
    cfbPrivGCPtr    devPriv
#endif
);

Bool
xf86CreateGC(pGC)
    register GCPtr pGC;
{
    cfbPrivGC  *pPriv;

    if (PixmapWidthPaddingInfo[pGC->depth].padPixelsLog2 == LOG2_BITMAP_PAD)
	return (mfbCreateGC(pGC));
    pGC->clientClip = NULL;
    pGC->clientClipType = CT_NONE;

    /*
     * some of the output primitives aren't really necessary, since they
     * will be filled in ValidateGC because of dix/CreateGC() setting all
     * the change bits.  Others are necessary because although they depend
     * on being a color frame buffer, they don't change 
     */

    pGC->ops = &NonTEOps;
    pGC->funcs = &xf86GCFuncs;

    /* cfb wants to translate before scan conversion */
    pGC->miTranslate = 1;

    pPriv = cfbGetGCPrivate(pGC);
    pPriv->rop = pGC->alu;
    pPriv->oneRect = FALSE;
    pPriv->fExpose = TRUE;
    pPriv->freeCompClip = FALSE;
    pPriv->pRotatedPixmap = (PixmapPtr) NULL;
    return TRUE;
}

/* Clipping conventions
	if the drawable is a window
	    CT_REGION ==> pCompositeClip really is the composite
	    CT_other ==> pCompositeClip is the window clip region
	if the drawable is a pixmap
	    CT_REGION ==> pCompositeClip is the translated client region
		clipped to the pixmap boundary
	    CT_other ==> pCompositeClip is the pixmap bounding box
*/

/* Setting this to FALSE should force full evaluation at initialization. */
static Bool last_xf86VTSema = FALSE;

void
xf86ValidateGC(pGC, changes, pDrawable)
    register GCPtr  pGC;
    unsigned long   changes;
    DrawablePtr	    pDrawable;
{
    int         mask;		/* stateChanges */
    int         index;		/* used for stepping through bitfields */
    int		new_rrop;
    Bool        new_line, new_text, new_fillspans, new_fillarea;
    int		new_rotate;
    int		xrot, yrot;
    /* flags for changing the proc vector */
    cfbPrivGCPtr devPriv;
    int		oneRect;
    int		drawableChanged;
    Bool	VTSwitch;
    Bool	new_cfb_rrop, new_cfb_fillspans;
    Bool	new_cfb_text, new_cfb_fillarea, new_cfb_line;
    Bool	match_common;

    /*
     * The devPrivate.val indicates what kind of of GC ops we are
     * dealing with:
     *
     *	0	cfb defaults structure
     *  1	modified cfb ops
     *  2	modified on-screen (accelerated) ops
     */
    VTSwitch = (last_xf86VTSema != xf86VTSema);
    if (!xf86VTSema || pDrawable->type != DRAWABLE_WINDOW) {
    	/*
    	 * We arrive here in two different cases:
    	 * - We are switched away to a different VT, which means the
    	 *   screen has been virtualized.
    	 * - We are writing to a pixmap.
    	 *
    	 * In either case, we need to use cfb defaults.
    	 *
    	 * As a stop-gap measure, just reset all GC ops and set the
    	 * changes mask to all-ones when switching away.
    	 */
    	if (pGC->ops->devPrivate.val == 2) {
    	    /*
    	     * The GC ops still point to on-screen functions, but
    	     * we have switched away. Replace the GC ops with cfb ones.
    	     * This will also catch the case of the drawable of an
    	     * on-screen GC being changed to a pixmap.
    	     */
	    miDestroyGCOps(pGC->ops);
    	    pGC->ops = &NonTEOps;

	    /*
	     * XXX Is the standard cfb code playing dangerous games?
	     *
	     * It defined "NonTEOps" etc. as a constant structure but
	     * does not include the devPrivate.val at the end in cfbgc.c
	     * (it clearly expects this value to be zero). How can it get
	     * away with this?
	     */
/*    	    pGC->ops->devPrivate.val = 0; */
    	    changes = 0xFFFFFF;
    	}
#ifdef VGA256
	vga256ValidateGC(pGC, changes, pDrawable);
#else
    	cfbValidateGC(pGC, changes, pDrawable);
#endif
        if (xf86VTSema && pDrawable->type != DRAWABLE_WINDOW)
            return;
        if (VTSwitch) {
            last_xf86VTSema = xf86VTSema;
#if 0
            ErrorF("Switch away detected\n");
#endif
        }
    	return;
    }
    last_xf86VTSema = xf86VTSema;
    if (VTSwitch) {
        xf86InvalidatePixmapCache();
#if 0
        ErrorF("Switch to graphics mode detected\n");
#endif
    }

    new_rotate = pGC->lastWinOrg.x != pDrawable->x ||
		 pGC->lastWinOrg.y != pDrawable->y;

    pGC->lastWinOrg.x = pDrawable->x;
    pGC->lastWinOrg.y = pDrawable->y;
    devPriv = cfbGetGCPrivate(pGC);

    new_rrop = FALSE;
    new_line = FALSE;
    new_text = FALSE;
    new_fillspans = FALSE;
    new_fillarea = FALSE;

    drawableChanged = pDrawable->serialNumber !=
        (pGC->serialNumber & (DRAWABLE_SERIAL_BITS));

    /*
     * if the client clip is different or moved OR the subwindowMode has
     * changed OR the window's clip has changed since the last validation
     * we need to recompute the composite clip 
     */

    if ((changes & (GCClipXOrigin|GCClipYOrigin|GCClipMask|GCSubwindowMode)) ||
	drawableChanged)
    {
	miComputeCompositeClip (pGC, pDrawable);
#ifdef NO_ONE_RECT
	devPriv->oneRect = FALSE;
#else
	oneRect = REGION_NUM_RECTS(devPriv->pCompositeClip) == 1;
	if (oneRect != devPriv->oneRect)
	    new_line = TRUE;
	devPriv->oneRect = oneRect;
#endif
    }

    mask = changes;
    while (mask) {
	index = lowbit (mask);
	mask &= ~index;

	/*
	 * this switch acculmulates a list of which procedures might have
	 * to change due to changes in the GC.  in some cases (e.g.
	 * changing one 16 bit tile for another) we might not really need
	 * a change, but the code is being paranoid. this sort of batching
	 * wins if, for example, the alu and the font have been changed,
	 * or any other pair of items that both change the same thing. 
	 */
	switch (index) {
	case GCForeground:
	    pGC->fgPixel &= PMSK;
	case GCFunction:
	    new_rrop = TRUE;
	    new_text = TRUE;
	    break;
	case GCPlaneMask:
	    new_rrop = TRUE;
	    new_text = TRUE;
	    break;
	case GCBackground:
	    pGC->bgPixel &= PMSK;
	    break;
	case GCLineStyle:
	case GCLineWidth:
	    new_line = TRUE;
	    break;
	case GCJoinStyle:
	case GCCapStyle:
	    break;
	case GCFillStyle:
	    new_text = TRUE;
	    new_fillspans = TRUE;
	    new_line = TRUE;
	    new_fillarea = TRUE;
	    break;
	case GCFillRule:
	    break;
	case GCTile:
	    new_fillspans = TRUE;
	    new_fillarea = TRUE;
	    break;

	case GCStipple:
	    if (pGC->stipple)
	    {
		int width = pGC->stipple->drawable.width;
		PixmapPtr nstipple;

		if ((width <= PGSZ) && !(width & (width - 1)) &&
		    (nstipple = cfbCopyPixmap(pGC->stipple)))
		{
		    cfbPadPixmap(nstipple);
		    (*pGC->pScreen->DestroyPixmap)(pGC->stipple);
		    pGC->stipple = nstipple;
		}
	    }
	    new_fillspans = TRUE;
	    new_fillarea = TRUE;
	    break;

	case GCTileStipXOrigin:
	    new_rotate = TRUE;
	    break;

	case GCTileStipYOrigin:
	    new_rotate = TRUE;
	    break;

	case GCFont:
	    new_text = TRUE;
	    break;
	case GCSubwindowMode:
	    break;
	case GCGraphicsExposures:
	    break;
	case GCClipXOrigin:
	    break;
	case GCClipYOrigin:
	    break;
	case GCClipMask:
	    break;
	case GCDashOffset:
	    break;
	case GCDashList:
	    break;
	case GCArcMode:
	    break;
	default:
	    break;
	}
    }

    /*
     * If the drawable has changed,  ensure suitable
     * entries are in the proc vector. 
     */
    if (drawableChanged) {
	new_fillspans = TRUE;	/* deal with FillSpans later */
    }

    if (new_rotate || new_fillspans)
    {
	Bool new_pix = FALSE;

	xrot = pGC->patOrg.x + pDrawable->x;
	yrot = pGC->patOrg.y + pDrawable->y;

	switch (pGC->fillStyle)
	{
	case FillTiled:
	    if (!pGC->tileIsPixel)
	    {
		int width = pGC->tile.pixmap->drawable.width * PSZ;

		if ((width <= PGSZ) && !(width & (width - 1)))
		{
		    cfbCopyRotatePixmap(pGC->tile.pixmap,
					&devPriv->pRotatedPixmap,
					xrot, yrot);
		    new_pix = TRUE;
		}
	    }
	    break;
#ifdef FOUR_BIT_CODE
	case FillStippled:
	case FillOpaqueStippled:
	    {
		int width = pGC->stipple->drawable.width;

		if ((width <= PGSZ) && !(width & (width - 1)))
		{
		    mfbCopyRotatePixmap(pGC->stipple,
					&devPriv->pRotatedPixmap, xrot, yrot);
		    new_pix = TRUE;
		}
	    }
	    break;
#endif
	}
	if (!new_pix && devPriv->pRotatedPixmap)
	{
	    (*pGC->pScreen->DestroyPixmap)(devPriv->pRotatedPixmap);
	    devPriv->pRotatedPixmap = (PixmapPtr) NULL;
	}
    }

    new_cfb_rrop = new_rrop;

    if (new_rrop)
    {
	int old_rrop;

	old_rrop = devPriv->rop;
	devPriv->rop = cfbReduceRasterOp (pGC->alu, pGC->fgPixel,
					   pGC->planemask,
					   &devPriv->and, &devPriv->xor);
	if (old_rrop == devPriv->rop)
	    new_cfb_rrop = FALSE;
	else
	{
#ifdef PIXEL_ADDR
	    new_line = TRUE;
#endif
#ifdef WriteBitGroup
	    new_text = TRUE;
#endif
	    new_fillspans = TRUE;
	    new_fillarea = TRUE;
	}
    }

    /*
     * It's pretty certain we'll be modifying the GC ops, so create
     * them if they still point to a defaults structure.
     */
    if (!pGC->ops->devPrivate.val)
    {
	pGC->ops = miCreateGCOps (pGC->ops);
	pGC->ops->devPrivate.val = 1;
    }

    if (pGC->ops->devPrivate.val == 1) {
        /*
         * The GC ops are still pointing to off-screen (cfb) functions.
         * Everything must be initialized.
         *
         * This happens (1) with new GC's that we just initialized above,
         *              (2) when previously VT-switched away.
         */
        new_rrop = new_line = new_text = new_fillspans = new_fillarea = TRUE;
	/* Assign 2 to indicate that we are writing to the screen. */
	pGC->ops->devPrivate.val = 2;
    }

    match_common = FALSE;

    if (new_cfb_rrop || new_fillspans || new_text || new_fillarea || new_line)
    {
	GCOps	*newops;

        new_cfb_fillspans = new_fillspans;
        new_cfb_text = new_text;
        new_cfb_fillarea = new_fillarea;
        new_cfb_line = new_line;

	if (newops = MatchCommon (pGC, devPriv))
 	{
 	    /*
 	     * We must make a copy of the NonTEOps etc.
 	     * because we might modify the GC ops further for
 	     * accelerated functions.
 	     */
#if 0
	    if (pGC->ops->devPrivate.val)
		miDestroyGCOps (pGC->ops);
#endif
	    *(pGC->ops) = *newops;
	    /* Make sure the devPrivate.val is correct! */
	    pGC->ops->devPrivate.val = 2;
	    /*
	     * These are special, the cfb logic relies on MatchCommon
	     * to select the right functions for the common case, and
	     * we must not re-evaluate the cfb functions later.
	     */
	    new_cfb_rrop = new_cfb_line = new_cfb_fillspans = new_cfb_text =
	    new_cfb_fillarea = FALSE;
	    /*
	     * Because the MatchCommon replaced all GCops with cfb functions,
	     * all accelerated functions must be re-evaluated, which is
	     * indicated with a flag.
	     */
	    match_common = TRUE;
	}
 	else
 	{
#if 0
	    if (!pGC->ops->devPrivate.val)
	    {
		pGC->ops = miCreateGCOps (pGC->ops);
		pGC->ops->devPrivate.val = 1;
	    }
#endif
	}
    }

    /* deal with the changes we've collected */
    /*
     * We must detect changes that affect the selection of acceleration
     * functions (only relevant if match_common is false), beyond what
     * is required for cfb.
     * If the planemask or rrop changed, new_rrop will be
     * true. It is also true if the foreground color is changed.
     * This is convenient, since this precisely matches the
     * raster-op, planemask and "RGB_EQUAL" restrictions
     * used for selecting accelerated functions.
     */
    if (new_rrop || (changes & GCBackground)) {
        new_line = TRUE;
        new_text = TRUE;
        new_fillspans = TRUE;
        new_fillarea = TRUE;
    }

    if (new_line || match_common) {
        xf86GCNewFillPolygon(pGC, new_cfb_line);

        xf86GCNewRectangle(pGC, new_cfb_line);
    }

    if (!new_line && (changes & GCCapStyle)) {
        /* Accelerated PolySegment may change with the CapStyle. */
        new_line = TRUE;
        new_cfb_line = FALSE;
    }
    if (new_line || match_common)
        xf86GCNewLine(pGC, pDrawable, new_cfb_line);

    if ((new_text || match_common) && (pGC->font))
        xf86GCNewText(pGC, new_cfb_text);

    if (new_fillspans || match_common)
        xf86GCNewFillSpans(pGC, new_cfb_fillspans);

    if (new_fillarea || match_common) {
        xf86GCNewFillArea(pGC, new_cfb_fillarea);
    }

    if (new_rrop || match_common)
        xf86GCNewCopyArea(pGC);

    /*
     * xf86CopyPlane contains faster unaccelerated functions, so we
     * want to use it in any case. That does mean xf86CopyPlane has
     * to do some checking.
     */
    pGC->ops->CopyPlane = xf86CopyPlane;
}

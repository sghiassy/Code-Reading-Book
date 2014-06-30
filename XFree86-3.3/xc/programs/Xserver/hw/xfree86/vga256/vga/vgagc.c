/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgagc.c,v 3.7 1996/12/23 06:59:46 dawes Exp $ */
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
/* $XConsortium: vgagc.c /main/6 1996/02/21 18:11:05 kaleb $ */

#include "vga256.h"
#include "migc.h"

void vga256ValidateGC();

static GCFuncs vga256Funcs = {
    vga256ValidateGC,
    miChangeGC,
    miCopyGC,
    miDestroyGC,
    miChangeClip,
    miDestroyClip,
    miCopyClip,
};

GCOps	vga256TEOps1Rect = {
    vga256SolidSpansCopy,
    vga256SetSpans,
    cfbPutImage,
    vga256CopyArea,
    vga256CopyPlane,
    vga256PolyPoint,
    vga256LineSS,
    vga256SegmentSS,
    miPolyRectangle,
    vga256ZeroPolyArcSS8Copy,
    vga256FillPoly1RectCopy,
    vga256PolyFillRect,
    vga256PolyFillArcSolidCopy,
    miPolyText8,
    miPolyText16,
    miImageText8,
    miImageText16,
    vga256TEGlyphBlt8,
    vga256PolyGlyphBlt8,
    vga256PushPixels8,
    NULL,
};

GCOps	vga256TEOps = {
    vga256SolidSpansCopy,
    vga256SetSpans,
    cfbPutImage,
    vga256CopyArea,
    vga256CopyPlane,
    vga256PolyPoint,
    vga256LineSS,
    vga256SegmentSS,
    miPolyRectangle,
    vga256ZeroPolyArcSS8Copy,
    miFillPolygon,
    vga256PolyFillRect,
    vga256PolyFillArcSolidCopy,
    miPolyText8,
    miPolyText16,
    miImageText8,
    miImageText16,
    vga256TEGlyphBlt8,
    vga256PolyGlyphBlt8,
    vga256PushPixels8,
    NULL,
};

GCOps	vga256NonTEOps1Rect = {
    vga256SolidSpansCopy,
    vga256SetSpans,
    cfbPutImage,
    vga256CopyArea,
    vga256CopyPlane,
    vga256PolyPoint,
    vga256LineSS,
    vga256SegmentSS,
    miPolyRectangle,
    vga256ZeroPolyArcSS8Copy,
    vga256FillPoly1RectCopy,
    vga256PolyFillRect,
    vga256PolyFillArcSolidCopy,
    miPolyText8,
    miPolyText16,
    miImageText8,
    miImageText16,
    cfbImageGlyphBlt8,
    vga256PolyGlyphBlt8,
    vga256PushPixels8,
    NULL,
};

GCOps	vga256NonTEOps = {
    vga256SolidSpansCopy,
    vga256SetSpans,
    cfbPutImage,
    vga256CopyArea,
    vga256CopyPlane,
    vga256PolyPoint,
    vga256LineSS,
    vga256SegmentSS,
    miPolyRectangle,
    vga256ZeroPolyArcSS8Copy,
    miFillPolygon,
    vga256PolyFillRect,
    vga256PolyFillArcSolidCopy,
    miPolyText8,
    miPolyText16,
    miImageText8,
    miImageText16,
    cfbImageGlyphBlt8,
    vga256PolyGlyphBlt8,
    vga256PushPixels8,
    NULL,
};

GCOps *
vga256matchCommon (pGC, devPriv)
    GCPtr	    pGC;
    cfbPrivGCPtr    devPriv;
{
    if (pGC->lineWidth != 0)
	return 0;
    if (pGC->lineStyle != LineSolid)
	return 0;
    if (pGC->fillStyle != FillSolid)
	return 0;
    if (devPriv->rop != GXcopy)
	return 0;
    if (pGC->font &&
	FONTMAXBOUNDS(pGC->font,rightSideBearing) -
        FONTMINBOUNDS(pGC->font,leftSideBearing) <= 32 &&
	FONTMINBOUNDS(pGC->font,characterWidth) >= 0)
    {

	if (TERMINALFONT(pGC->font)
	    && FONTMAXBOUNDS(pGC->font,characterWidth) >= PGSZB
	)
	    if (devPriv->oneRect)
		return &vga256TEOps1Rect;
	    else
		return &vga256TEOps;
	else
	    if (devPriv->oneRect)
		return &vga256NonTEOps1Rect;
	    else
		return &vga256NonTEOps;
    }
    return 0;
}

Bool
vga256CreateGC(pGC)
    register GCPtr pGC;
{
    cfbPrivGC  *pPriv;

    switch (pGC->depth) {
    case 1:
	return (mfbCreateGC(pGC));
    case PSZ:
	break;
    default:
	ErrorF("vga256CreateGC: unsupported depth: %d\n", pGC->depth);
	return FALSE;
    }
    pGC->clientClip = NULL;
    pGC->clientClipType = CT_NONE;

    /*
     * some of the output primitives aren't really necessary, since they
     * will be filled in ValidateGC because of dix/CreateGC() setting all
     * the change bits.  Others are necessary because although they depend
     * on being a color frame buffer, they don't change 
     */

    pGC->ops = &vga256NonTEOps;
    pGC->funcs = &vga256Funcs;

    /* cfb wants to translate before scan conversion */
    pGC->miTranslate = 1;

    pPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr);
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

void
vga256ValidateGC(pGC, changes, pDrawable)
    register GCPtr  pGC;
    Mask	    changes;
    DrawablePtr	    pDrawable;
{
    int         mask;		/* stateChanges */
    int         index;		/* used for stepping through bitfields */
    int		new_rrop;
    int         new_line, new_text, new_fillspans, new_fillarea;
    int		new_rotate;
    int		xrot, yrot;
    /* flags for changing the proc vector */
    cfbPrivGCPtr devPriv;
    int		oneRect;

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

    /*
     * if the client clip is different or moved OR the subwindowMode has
     * changed OR the window's clip has changed since the last validation
     * we need to recompute the composite clip 
     */

    if ((changes & (GCClipXOrigin|GCClipYOrigin|GCClipMask|GCSubwindowMode)) ||
	(pDrawable->serialNumber != (pGC->serialNumber & DRAWABLE_SERIAL_BITS))
	)
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
	case GCFunction:
	case GCForeground:
	    new_rrop = TRUE;
	    break;
	case GCPlaneMask:
	    new_rrop = TRUE;
	    new_text = TRUE;
	    break;
	case GCBackground:
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
     * If the drawable has changed,  check its depth & ensure suitable
     * entries are in the proc vector. 
     */
    if (pDrawable->serialNumber != (pGC->serialNumber & (DRAWABLE_SERIAL_BITS))) {
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
	}
	if (!new_pix && devPriv->pRotatedPixmap)
	{
	    (*pGC->pScreen->DestroyPixmap)(devPriv->pRotatedPixmap);
	    devPriv->pRotatedPixmap = (PixmapPtr) NULL;
	}
    }

    if (new_rrop)
    {
	int old_rrop;

	old_rrop = devPriv->rop;
	devPriv->rop = cfbReduceRasterOp (pGC->alu, pGC->fgPixel,
					   pGC->planemask,
					   &devPriv->and, &devPriv->xor);
	if (old_rrop == devPriv->rop)
	    new_rrop = FALSE;
	else
	{
	    new_line = TRUE;
	    new_text = TRUE;
	    new_fillspans = TRUE;
	    new_fillarea = TRUE;
	}
    }

    if (new_rrop || new_fillspans || new_text || new_fillarea || new_line)
    {
	GCOps	*newops;

	if (newops = vga256matchCommon (pGC, devPriv))
 	{
	    if (pGC->ops->devPrivate.val)
		miDestroyGCOps (pGC->ops);
	    pGC->ops = newops;
	    new_rrop = new_line = new_fillspans = new_text = new_fillarea = 0;
	}
 	else
 	{
	    if (!pGC->ops->devPrivate.val)
	    {
		pGC->ops = miCreateGCOps (pGC->ops);
		pGC->ops->devPrivate.val = 1;
	    }
	}
    }

    /* deal with the changes we've collected */
    if (new_line)
    {
	pGC->ops->FillPolygon = miFillPolygon;
	if (devPriv->oneRect && pGC->fillStyle == FillSolid)
	{
	    switch (devPriv->rop) {
	    case GXcopy:
		pGC->ops->FillPolygon = vga256FillPoly1RectCopy;
		break;
	    default:
		pGC->ops->FillPolygon = vga256FillPoly1RectGeneral;
		break;
	    }
	}
	if (pGC->lineWidth == 0)
	{
#ifdef PIXEL_ADDR
	    if ((pGC->lineStyle == LineSolid) && (pGC->fillStyle == FillSolid))
	    {
		switch (devPriv->rop)
		{
		case GXxor:
		    pGC->ops->PolyArc = vga256ZeroPolyArcSS8Xor;
		    break;
		case GXcopy:
		    pGC->ops->PolyArc = vga256ZeroPolyArcSS8Copy;
		    break;
		default:
		    pGC->ops->PolyArc = vga256ZeroPolyArcSS8General;
		    break;
		}
	    }
	    else
#endif
		pGC->ops->PolyArc = miZeroPolyArc;
	}
	else
	    pGC->ops->PolyArc = miPolyArc;
	pGC->ops->PolySegment = miPolySegment;
	switch (pGC->lineStyle)
	{
	case LineSolid:
	    if(pGC->lineWidth == 0)
	    {
		if (pGC->fillStyle == FillSolid)
		{
		    pGC->ops->Polylines = vga256LowlevFuncs.lineSS;
		    pGC->ops->PolySegment = vga256LowlevFuncs.segmentSS;
		}
 		else
		    pGC->ops->Polylines = miZeroLine;
	    }
	    else
		pGC->ops->Polylines = miWideLine;
	    break;
	case LineOnOffDash:
	case LineDoubleDash:
	    if (pGC->lineWidth == 0 && pGC->fillStyle == FillSolid)
	    {
		pGC->ops->Polylines = vga256LineSD;
		pGC->ops->PolySegment = vga256SegmentSD;
	    } else
		pGC->ops->Polylines = miWideDash;
	    break;
	}
    }

    if (new_text && (pGC->font))
    {
        if (FONTMAXBOUNDS(pGC->font,rightSideBearing) -
            FONTMINBOUNDS(pGC->font,leftSideBearing) > 32 ||
	    FONTMINBOUNDS(pGC->font,characterWidth) < 0)
        {
            pGC->ops->PolyGlyphBlt = miPolyGlyphBlt;
            pGC->ops->ImageGlyphBlt = miImageGlyphBlt;
        }
        else
        {
	    if (pGC->fillStyle == FillSolid)
	    {
		if (devPriv->rop == GXcopy)
		    pGC->ops->PolyGlyphBlt = vga256PolyGlyphBlt8;
		else
		    pGC->ops->PolyGlyphBlt = vga256PolyGlyphRop8;
	    }
	    else
		pGC->ops->PolyGlyphBlt = miPolyGlyphBlt;
            /* special case ImageGlyphBlt for terminal emulator fonts */
            if (TERMINALFONT(pGC->font) &&
		(pGC->planemask & PMSK) == PMSK
		&& FONTMAXBOUNDS(pGC->font,characterWidth) >= PGSZB
		)
	    {
                pGC->ops->ImageGlyphBlt = vga256LowlevFuncs.teGlyphBlt8;
	    }
            else
	    {
		if (devPriv->rop == GXcopy &&
		    pGC->fillStyle == FillSolid &&
		    (pGC->planemask & PMSK) == PMSK)
		    pGC->ops->ImageGlyphBlt = cfbImageGlyphBlt8;
		else
		    pGC->ops->ImageGlyphBlt = miImageGlyphBlt;
	    }
        }
    }    


    if (new_fillspans) {
	switch (pGC->fillStyle) {
	case FillSolid:
	    if (vga256LowlevFuncs.fillSolidSpans != vga256SolidSpansGeneral) {
	        pGC->ops->FillSpans = vga256LowlevFuncs.fillSolidSpans;
	        break;
	    }
	    switch (devPriv->rop) {
	    case GXcopy:
		pGC->ops->FillSpans = vga256SolidSpansCopy;
		break;
	    case GXxor:
		pGC->ops->FillSpans = vga256SolidSpansXor;
		break;
	    default:
		pGC->ops->FillSpans = vga256SolidSpansGeneral;
		break;
	    }
	    break;
	case FillTiled:
	    if (devPriv->pRotatedPixmap)
	    {
		if (pGC->alu == GXcopy && (pGC->planemask & PMSK) == PMSK)
		    pGC->ops->FillSpans = vga256Tile32FSCopy;
		else
		    pGC->ops->FillSpans = vga256Tile32FSGeneral;
	    }
	    else
		pGC->ops->FillSpans = vga256UnnaturalTileFS;
	    break;
	case FillStippled:
	    if (devPriv->pRotatedPixmap)
		pGC->ops->FillSpans = vga2568Stipple32FS;
	    else
		pGC->ops->FillSpans = vga256UnnaturalStippleFS;
	    break;
	case FillOpaqueStippled:
	    if (devPriv->pRotatedPixmap)
		pGC->ops->FillSpans = vga2568OpaqueStipple32FS;
	    else
		pGC->ops->FillSpans = vga256UnnaturalStippleFS;
	    break;
	default:
	    FatalError("vga256ValidateGC: illegal fillStyle\n");
	}
    } /* end of new_fillspans */

    if (new_fillarea) {
	pGC->ops->PushPixels = mfbPushPixels;
	if (pGC->fillStyle == FillSolid && devPriv->rop == GXcopy)
	    pGC->ops->PushPixels = vga256PushPixels8;
	pGC->ops->PolyFillArc = miPolyFillArc;
	if (pGC->fillStyle == FillSolid)
	{
	    switch (devPriv->rop)
	    {
	    case GXcopy:
		pGC->ops->PolyFillArc = vga256PolyFillArcSolidCopy;
		break;
	    default:
		pGC->ops->PolyFillArc = vga256PolyFillArcSolidGeneral;
		break;
	    }
	}
    }
}

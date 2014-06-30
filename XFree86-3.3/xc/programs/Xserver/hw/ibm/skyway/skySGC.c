/*
 * $XConsortium: skySGC.c /main/10 1995/12/12 16:58:42 matt $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991 
 *
 * All Rights Reserved 
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of IBM not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS, IN NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 */

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


#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "cfb.h"
#include "fontstruct.h"
#include "dixfontstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "region.h"

#include "mistruct.h"
#include "mibstore.h"

#include "cfbmskbits.h"
#include "cfb8bit.h"

/* ---- */
#include "skyPriv.h"
#include "skyHdwr.h"
#include "skyProcs.h"
#include <stdio.h>
/* ---- */


void skyValidateGC(), skyChangeGC(), skyCopyGC(), skyDestroyGC();
void skyChangeClip(), skyDestroyClip(), skyCopyClip();

GCFuncs skyGCFuncs = {
    skyValidateGC,
    skyChangeGC,
    skyCopyGC,
    skyDestroyGC,
    skyChangeClip,
    skyDestroyClip,
    skyCopyClip,
};

#ifdef NOCLIP_TEST
GCOps	skyTEOps1Rect_Win = {
    cfbSolidSpansCopy,
    cfbSetSpans,
    cfbPutImage,
    skyCopyArea,
    cfbCopyPlane,
    cfbPolyPoint,
#if PSZ == 8
    cfb8LineSS1Rect,
    cfb8SegmentSS1Rect,
#else
    cfbLineSS,
    cfbSegmentSS,
#endif
    miPolyRectangle,
#if PSZ == 8
    cfbZeroPolyArcSS8Copy,
#else
    miZeroPolyArc,
#endif
    cfbFillPoly1RectCopy,
    skyPolyFillRectSolid_1Rect,
    cfbPolyFillArcSolidCopy,
    miPolyText8,
    miPolyText16,
    miImageText8,
    miImageText16,
#if PSZ == 8
    cfbTEGlyphBlt8,
    cfbPolyGlyphBlt8,
    cfbPushPixels8,
#else
    cfbTEGlyphBlt,
    miPolyGlyphBlt,
    mfbPushPixels,
#endif
    NULL,
};
#endif

GCOps	skyTEOps1Rect = {
    cfbSolidSpansCopy,
    cfbSetSpans,
    cfbPutImage,
    skyCopyArea,
    cfbCopyPlane,
    cfbPolyPoint,
#if PSZ == 8
    cfb8LineSS1Rect,
    cfb8SegmentSS1Rect,
#else
    cfbLineSS,
    cfbSegmentSS,
#endif
    miPolyRectangle,
#if PSZ == 8
    cfbZeroPolyArcSS8Copy,
#else
    miZeroPolyArc,
#endif
    cfbFillPoly1RectCopy,
    skyPolyFillRect,
    cfbPolyFillArcSolidCopy,
    miPolyText8,
    miPolyText16,
    miImageText8,
    miImageText16,
#if PSZ == 8
    cfbTEGlyphBlt8,
    cfbPolyGlyphBlt8,
    cfbPushPixels8,
#else
    cfbTEGlyphBlt,
    miPolyGlyphBlt,
    mfbPushPixels,
#endif
    NULL,
};

GCOps	skyTEOps = {
    cfbSolidSpansCopy,
    cfbSetSpans,
    cfbPutImage,
    skyCopyArea,
    cfbCopyPlane,
    cfbPolyPoint,
    cfbLineSS,
    cfbSegmentSS,
    miPolyRectangle,
#if PSZ == 8
    cfbZeroPolyArcSS8Copy,
#else
    miZeroPolyArc,
#endif
    miFillPolygon,
    skyPolyFillRect,
    cfbPolyFillArcSolidCopy,
    miPolyText8,
    miPolyText16,
    miImageText8,
    miImageText16,
#if PSZ == 8
    cfbTEGlyphBlt8,
    cfbPolyGlyphBlt8,
    cfbPushPixels8,
#else
    cfbTEGlyphBlt,
    miPolyGlyphBlt,
    mfbPushPixels,
#endif
    NULL,
};

#ifdef NOCLIP_TEST
GCOps	skyNonTEOps1Rect_Win = {
    cfbSolidSpansCopy,
    cfbSetSpans,
    cfbPutImage,
    skyCopyArea,
    cfbCopyPlane,
    cfbPolyPoint,
#if PSZ == 8
    cfb8LineSS1Rect,
    cfb8SegmentSS1Rect,
#else
    cfbLineSS,
    cfbSegmentSS,
#endif
    miPolyRectangle,
#if PSZ == 8
    cfbZeroPolyArcSS8Copy,
#else
    miZeroPolyArc,
#endif
    cfbFillPoly1RectCopy,
    skyPolyFillRectSolid_1Rect,
    cfbPolyFillArcSolidCopy,
    miPolyText8,
    miPolyText16,
    miImageText8,
    miImageText16,
    cfbImageGlyphBlt8,
    cfbPolyGlyphBlt8,
    cfbPushPixels8,
    NULL,
};
#endif

GCOps	skyNonTEOps1Rect = {
    cfbSolidSpansCopy,
    cfbSetSpans,
    cfbPutImage,
    skyCopyArea,
    cfbCopyPlane,
    cfbPolyPoint,
#if PSZ == 8
    cfb8LineSS1Rect,
    cfb8SegmentSS1Rect,
#else
    cfbLineSS,
    cfbSegmentSS,
#endif
    miPolyRectangle,
#if PSZ == 8
    cfbZeroPolyArcSS8Copy,
#else
    miZeroPolyArc,
#endif
    cfbFillPoly1RectCopy,
    skyPolyFillRect,
    cfbPolyFillArcSolidCopy,
    miPolyText8,
    miPolyText16,
    miImageText8,
    miImageText16,
    cfbImageGlyphBlt8,
    cfbPolyGlyphBlt8,
    cfbPushPixels8,
    NULL,
};

GCOps	skyNonTEOps = {
    cfbSolidSpansCopy,
    cfbSetSpans,
    cfbPutImage,
    skyCopyArea,
    cfbCopyPlane,
    cfbPolyPoint,
    cfbLineSS,
    cfbSegmentSS,
    miPolyRectangle,
#if PSZ == 8
    cfbZeroPolyArcSS8Copy,
#else
    miZeroPolyArc,
#endif
    miFillPolygon,
    skyPolyFillRect,
    cfbPolyFillArcSolidCopy,
    miPolyText8,
    miPolyText16,
    miImageText8,
    miImageText16,
#if PSZ == 8
    cfbImageGlyphBlt8,
    cfbPolyGlyphBlt8,
    cfbPushPixels8,
#else
    miImageGlyphBlt,
    miPolyGlyphBlt,
    mfbPushPixels,
#endif
    NULL,
};

GCOps *
skyMatchCommon (pDrawable, pGC, devPriv)
    DrawablePtr     pDrawable;
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
#if PSZ == 8
	    && FONTMAXBOUNDS(pGC->font,characterWidth) >= 4
#endif
	)
	    if (devPriv->oneRect)
	    {
#ifdef NOCLIP_TEST
		if (pDrawable->type == DRAWABLE_WINDOW)
		    return &skyTEOps1Rect_Win;
		else
#endif
		    return &skyTEOps1Rect;
	    }
	    else
		return &skyTEOps;
	else
	    if (devPriv->oneRect)
	    {
#ifdef SKYWAY_CLIP
		if (pDrawable->type == DRAWABLE_WINDOW)
		    return &skyNonTEOps1Rect_Win;
		else
#endif
		    return &skyNonTEOps1Rect;
	    }
	    else
		return &skyNonTEOps;
    }
    return 0;
}

Bool
skyCreateGC(pGC)
    register GCPtr pGC;
{
    cfbPrivGC    *pPriv;
    /* ---- */
    skyPrivGCPtr  pSkyPriv = skyGetGCPrivate(pGC);
    /* ---- */

    if (pGC->depth == 1)
	return (mfbCreateGC(pGC));
    pGC->clientClip = NULL;
    pGC->clientClipType = CT_NONE;

    /*
     * some of the output primitives aren't really necessary, since they
     * will be filled in ValidateGC because of dix/CreateGC() setting all
     * the change bits.  Others are necessary because although they depend
     * on being a color frame buffer, they don't change 
     */

    pGC->ops = &skyNonTEOps;
    pGC->funcs = &skyGCFuncs;

    /* cfb wants to translate before scan conversion */
    pGC->miTranslate = 1;

    pPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr);
    pPriv->rop = pGC->alu;
    pPriv->oneRect = FALSE;
    pPriv->fExpose = TRUE;
    pPriv->freeCompClip = FALSE;
    pPriv->pRotatedPixmap = (PixmapPtr) NULL;

    /* ---- */
    pSkyPriv->fillMode         = punt;
    pSkyPriv->BgFgMix          = pGC->alu << 8 | pGC->alu;
    pSkyPriv->tileStipID       = 0;
    pSkyPriv->stipple.size     = 0;
    pSkyPriv->stipple.bits     = NULL;
    pSkyPriv->stipple.freeBits = FALSE;
    /* ---- */

    return TRUE;
}

/*ARGSUSED*/
void
skyChangeGC(pGC, mask)
    GC		    *pGC;
    BITS32	    mask;
{
    return;
}

void
skyDestroyGC(pGC)
    GC 			*pGC;
{
    cfbPrivGC *pPriv;

    pPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr);
    if (pPriv->pRotatedPixmap)
	cfbDestroyPixmap(pPriv->pRotatedPixmap);
    if (pPriv->freeCompClip)
	REGION_DESTROY(pGC->pScreen, pPriv->pCompositeClip);
    skyDestroyOps (pGC->ops);
}

/*
 * create a private op array for a gc
 */

GCOps *
skyCreateOps (prototype)
    GCOps	*prototype;
{
    GCOps	*ret;
    extern Bool	Must_have_memory;

    /* XXX */ Must_have_memory = TRUE;
    ret = (GCOps *) xalloc (sizeof(GCOps));
    /* XXX */ Must_have_memory = FALSE;
    if (!ret)
	return 0;
    *ret = *prototype;
    ret->devPrivate.val = 1;
    return ret;
}

skyDestroyOps (ops)
    GCOps   *ops;
{
    if (ops->devPrivate.val)
	xfree (ops);
}


/* ---- */
#define FreeStippleData(stipple)					\
{									\
    if (stipple->freeBits)						\
    {									\
	xfree(stipple->bits);						\
									\
	stipple->size     = 0;						\
	stipple->bits     = NULL;					\
	stipple->freeBits = TRUE;					\
    }									\
}

#if PSZ == 8
/* See if Tile is can be represented as OpaqueStipple. If so, convert tile
 * to a packed stipple (no scanline pad).  Code borrowed from sunGX.c.
 *
 * XXX - Only works for depth 8 pixmaps currently.
 */

skyCheckDegenerateTile (pPixmap, pGC)
    PixmapPtr	    pPixmap;
    GCPtr           pGC;
{
    skyPrivGCPtr    pSkyPriv = skyGetGCPrivate(pGC);
    skyStipplePtr   stipple = &pSkyPriv->stipple;

    unsigned int    fg = (unsigned int)~0, bg = (unsigned int)~0;
    unsigned char   *tilebitsLine, *tilebits, tilebit;
    int		    nbwidth;
    int		    w, h;
    int		    x, y;
    int		    size;
    Bool            expandTile;

    w = pPixmap->drawable.width;
    h = pPixmap->drawable.height;


    /* If tile width is less than 32 and evenly divisible into 32,
     * expand the tile to width of 32. Skyway fills faster if tile
     * is 32 or wider.
     */

#ifndef NOEXPAND_TEST
    expandTile = ((w < 32) && !(w & (w - 1)));
#else
    expandTile = FALSE;
#endif

    if (expandTile)
	size = h*4;
    else
	size = (h*w + 7) >> 3;

    if (size > SKY_MAX_TILESTIP_SIZE) 
	return FALSE;

    if (size > stipple->size)
    {
	if (stipple->freeBits)
	    stipple->bits = (char *) xrealloc(stipple->bits, size);
	else
	    stipple->bits = (char *) xalloc(size);

	if (stipple->bits == NULL)
	{
	    stipple->size     = 0;
	    stipple->freeBits = FALSE;
	    return (FALSE);
	}

	stipple->size     = size;
	stipple->freeBits = TRUE;
    }

    tilebitsLine = (unsigned char *) pPixmap->devPrivate.ptr;
    nbwidth = pPixmap->devKind;

    if (expandTile)
    {
	/* Code from sunGXCheckTile() */

	unsigned long  *sbits = (unsigned long *) stipple->bits;
	unsigned long   sbit, mask;
	int             s_y, s_x;

	for (y = 0; y < h; y++)
	{
	    tilebits = tilebitsLine;
	    tilebitsLine += nbwidth;
	    sbit = 0;
	    mask = 1 << 31;
	    for (x = 0; x < w; x++)
	    {
		tilebit = *tilebits++;
		if (tilebit == fg)
		    sbit |=  mask;
		else if (tilebit != bg)
		{
		    if (fg == ~0)
		    {
			fg = tilebit;
			sbit |= mask;
		    }
		    else if (bg == ~0)
		    {
			bg = tilebit;
		    }
		    else
		    {
			return FALSE;
		    }
		}
		mask >>= 1;
	    }
	    for (s_x = w; s_x < 32; s_x <<= 1)
		sbit = sbit | (sbit >> s_x);
	    sbits[y] = sbit;
	    /*
	    for (s_y = y; s_y < 32; s_y += h)
		sbits[s_y] = sbit;
	    */
	}
    }
    else
    {
	unsigned char  *dstBits = (unsigned char *) stipple->bits;
	unsigned char   mask;

	memset(dstBits, 0, size);

	mask = 1 << 7;

	for (y = 0; y < h; y++)
	{
	    tilebits = tilebitsLine;
	    tilebitsLine += nbwidth;
	    for (x = 0; x < w; x++)
	    {
		tilebit = *tilebits++;
		if (tilebit == fg)
		    *dstBits |=  mask;
		else if (tilebit != bg)
		{
		    if (bg == ~0)
		    {
			bg = tilebit;
		    }
		    else if (fg == ~0)
		    {
			fg = tilebit;
			*dstBits |= mask;
		    }
		    else
		    {
			FreeStippleData(stipple);
			return FALSE;
		    }
		}

		mask >>= 1;
		if (mask == 0)
		{
		    mask = 1 << 7;
		    dstBits++;
		}
	    }
	}
    } /* ! expandTile */

    stipple->fgPixel = fg;
    stipple->bgPixel = bg;
    stipple->width   = expandTile ? 32 : w;
    stipple->height  = h;
    stipple->packed = TRUE;

    return TRUE;
}
#endif /* PSZ == 8 */


/* See if tile is not too big. */

skyCheckTile(pPixmap, pGC)
    PixmapPtr	    pPixmap;
    GCPtr           pGC;
{
    skyPrivGCPtr    pSkyPriv = skyGetGCPrivate(pGC);
    skyStipplePtr   stipple = &pSkyPriv->stipple;
    int		    size;

    if (pGC->tileIsPixel)
    {
	pSkyPriv->fillMode = fill_solid;
	FreeStippleData(stipple);
	return (TRUE);
    }

#ifndef NO_DEGEN_TEST
#if PSZ == 8
    if (skyCheckDegenerateTile(pPixmap, pGC, stipple))
    {
	pSkyPriv->fillMode = fill_stipple;
	return (TRUE);
    }
#endif
#endif

    FreeStippleData(stipple);

    size = pPixmap->devKind * pPixmap->drawable.height;
    if (size <= SKY_MAX_TILESTIP_SIZE)
    {
	pSkyPriv->fillMode = fill_tile;
	return (TRUE);
    }

    return (FALSE);
}


/* See if stipple is not too big. */

skyCheckStipple (pPixmap, pGC)
    PixmapPtr	    pPixmap;
    GCPtr           pGC;
{
    skyPrivGCPtr    pSkyPriv = skyGetGCPrivate(pGC);
    skyStipplePtr   stipple = &pSkyPriv->stipple;

    int		    w, h;

    w = pPixmap->drawable.width;
    h = pPixmap->drawable.height;

    stipple->packed = (w & 31) == 0;

    if (pPixmap->devKind*h > SKY_MAX_TILESTIP_SIZE)
	return FALSE;

    FreeStippleData(stipple);

    pSkyPriv->fillMode = fill_stipple;

    stipple->bits     = (char *) pPixmap->devPrivate.ptr; 
    stipple->size     = pPixmap->devKind*h;

    stipple->fgPixel  = pGC->fgPixel;
    stipple->bgPixel  = pGC->bgPixel;
    stipple->width    = w;
    stipple->height   = h;
    stipple->freeBits = FALSE;

    return TRUE;
}


static  skyStipplePtr tmpStipple = NULL;

skyCheckFill (pGC, pDrawable)
    GCPtr           pGC;
    DrawablePtr     pDrawable;
{
    skyPrivGCPtr    pSkyPriv = skyGetGCPrivate(pGC);
    skyStipplePtr   stipple = &pSkyPriv->stipple;

    pSkyPriv->fillMode = punt;	/* punt == Hardware can't handle */

    if (pDrawable->type != DRAWABLE_WINDOW)
	return (FALSE);

    pSkyPriv->BgFgMix = pGC->alu << 8 | pGC->alu;

    switch (pGC->fillStyle)
    {
	case FillSolid:
	    pSkyPriv->fillMode = fill_solid;
	    FreeStippleData(stipple);
	    return TRUE;

	case FillTiled:
	    return skyCheckTile (pGC->tile.pixmap, pGC);
	    break;

	case FillStippled:
	    pSkyPriv->BgFgMix = GXnoop << 8 | pGC->alu;
	    /* Fall Thru */

	case FillOpaqueStippled:
	    return skyCheckStipple (pGC->stipple, pGC);
	    break;
    }

    return FALSE;
}
/* ---- */



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
skyValidateGC(pGC, changes, pDrawable)
    register GCPtr  pGC;
    Mask	    changes;
    DrawablePtr	    pDrawable;
{
    WindowPtr   pWin;
    int         mask;		/* stateChanges */
    int         index;		/* used for stepping through bitfields */
    int		new_rrop;
    int         new_line, new_text, new_fillspans, new_fillarea;
    int		new_rotate;
    int		xrot, yrot;
    /* flags for changing the proc vector */
    cfbPrivGCPtr devPriv;
    /* ---- */
    skyPrivGCPtr pSkyPriv = skyGetGCPrivate(pGC);
    int          scrnNum  = pGC->pScreen->myNum;
    int          new_tile_id, new_stipple_id;
    /* ---- */
    int		oneRect;

    new_rotate = pGC->lastWinOrg.x != pDrawable->x ||
		 pGC->lastWinOrg.y != pDrawable->y;

    pGC->lastWinOrg.x = pDrawable->x;
    pGC->lastWinOrg.y = pDrawable->y;
    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	pWin = (WindowPtr) pDrawable;
    }
    else
    {
	pWin = (WindowPtr) NULL;
    }

    devPriv = ((cfbPrivGCPtr) (pGC->devPrivates[cfbGCPrivateIndex].ptr));

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
	ScreenPtr pScreen = pGC->pScreen;

	if (pWin) {
	    RegionPtr   pregWin;
	    Bool        freeTmpClip, freeCompClip;

	    if (pGC->subWindowMode == IncludeInferiors) {
		pregWin = NotClippedByChildren(pWin);
		freeTmpClip = TRUE;
	    }
	    else {
		pregWin = &pWin->clipList;
		freeTmpClip = FALSE;
	    }
	    freeCompClip = devPriv->freeCompClip;

	    /*
	     * if there is no client clip, we can get by with just keeping
	     * the pointer we got, and remembering whether or not should
	     * destroy (or maybe re-use) it later.  this way, we avoid
	     * unnecessary copying of regions.  (this wins especially if
	     * many clients clip by children and have no client clip.) 
	     */
	    if (pGC->clientClipType == CT_NONE) {
		if (freeCompClip)
		    REGION_DESTROY(pScreen, devPriv->pCompositeClip);
		devPriv->pCompositeClip = pregWin;
		devPriv->freeCompClip = freeTmpClip;
	    }
	    else {
		/*
		 * we need one 'real' region to put into the composite
		 * clip. if pregWin the current composite clip are real,
		 * we can get rid of one. if pregWin is real and the
		 * current composite clip isn't, use pregWin for the
		 * composite clip. if the current composite clip is real
		 * and pregWin isn't, use the current composite clip. if
		 * neither is real, create a new region. 
		 */

		REGION_TRANSLATE(pScreen, pGC->clientClip,
					    pDrawable->x + pGC->clipOrg.x,
					    pDrawable->y + pGC->clipOrg.y);
						  
		if (freeCompClip)
		{
		    REGION_INTERSECT(pGC->pScreen, devPriv->pCompositeClip,
					       pregWin, pGC->clientClip);
		    if (freeTmpClip)
			REGION_DESTROY(pScreen, pregWin);
		}
		else if (freeTmpClip)
		{
		    REGION_INTERSECT(pScreen, pregWin, pregWin, pGC->clientClip);
		    devPriv->pCompositeClip = pregWin;
		}
		else
		{
		    devPriv->pCompositeClip = REGION_CREATE(pScreen, NullBox,
								       0);
		    REGION_INTERSECT(pScreen, devPriv->pCompositeClip,
					  pregWin, pGC->clientClip);
		}
		devPriv->freeCompClip = TRUE;
		REGION_TRANSLATE(pScreen, pGC->clientClip,
					    -(pDrawable->x + pGC->clipOrg.x),
					    -(pDrawable->y + pGC->clipOrg.y));
						  
	    }
	}			/* end of composite clip for a window */
	else {
	    BoxRec      pixbounds;

	    /* XXX should we translate by drawable.x/y here ? */
	    pixbounds.x1 = 0;
	    pixbounds.y1 = 0;
	    pixbounds.x2 = pDrawable->width;
	    pixbounds.y2 = pDrawable->height;

	    if (devPriv->freeCompClip)
	    {
		REGION_RESET(pScreen, devPriv->pCompositeClip, &pixbounds);
	    }
	    else {
		devPriv->freeCompClip = TRUE;
		devPriv->pCompositeClip = REGION_CREATE(pScreen, &pixbounds,
								   1);
	    }

	    if (pGC->clientClipType == CT_REGION)
	    {
		REGION_TRANSLATE(pScreen, devPriv->pCompositeClip,
					    -pGC->clipOrg.x, -pGC->clipOrg.y);
		REGION_INTERSECT(pScreen, devPriv->pCompositeClip,
				      devPriv->pCompositeClip,
				      pGC->clientClip);
		REGION_TRANSLATE(pScreen, devPriv->pCompositeClip,
					    pGC->clipOrg.x, pGC->clipOrg.y);
	    }
	}			/* end of composute clip for pixmap */
	oneRect = REGION_NUM_RECTS(devPriv->pCompositeClip) == 1;
	if (oneRect != devPriv->oneRect)
	    new_line = TRUE;
	devPriv->oneRect = oneRect;
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
	    /* ---- */
	    new_tile_id = (pGC->fillStyle == FillTiled);
	    /* ---- */
	    break;

	case GCStipple:
	    if (pGC->stipple)
	    {
		int width = pGC->stipple->drawable.width;
		PixmapPtr nstipple;

		if ((width <= 32) && !(width & (width - 1)) &&
		    (nstipple = cfbCopyPixmap(pGC->stipple)))
		{
		    cfbPadPixmap(nstipple);
		    cfbDestroyPixmap(pGC->stipple);
		    pGC->stipple = nstipple;
		}
	    }
	    new_fillspans = TRUE;
	    new_fillarea = TRUE;
	    /* ---- */
	    new_stipple_id = (pGC->fillStyle==FillStippled ||
			    pGC->fillStyle==FillOpaqueStippled);
	    /* ---- */
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



    /* ---- */
    /* If there is a new tile, assign it a new (unique) ID. */

    if (new_tile_id)
    {
	if ((!pGC->tileIsPixel) && (pGC->tile.pixmap))
	    skyNewTileStipID(scrnNum, &pSkyPriv->tileStipID);
    }

  
    /* If there is a new stipple, assign it a new (unique) ID. */

    if (new_stipple_id)
    {
	if (pGC->stipple)
	    skyNewTileStipID(scrnNum, &pSkyPriv->tileStipID);
    }
    /* ---- */


    /*
     * If the drawable has changed,  ensure suitable
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

	if (!skyCheckFill(pGC,pDrawable))
	{
	switch (pGC->fillStyle)
	{
	case FillTiled:
	    if (!pGC->tileIsPixel)
	    {
		int width = pGC->tile.pixmap->drawable.width * PSZ;

		if ((width <= 32) && !(width & (width - 1)))
		{
		    cfbCopyRotatePixmap(pGC->tile.pixmap,
					&devPriv->pRotatedPixmap,
					xrot, yrot);
		    new_pix = TRUE;
		}
	    }
	    break;
#if (PSZ == 8)
	case FillStippled:
	case FillOpaqueStippled:
	    {
		int width = pGC->stipple->drawable.width;

		if ((width <= 32) && !(width & (width - 1)))
		{
		    mfbCopyRotatePixmap(pGC->stipple,
					&devPriv->pRotatedPixmap, xrot, yrot);
		    new_pix = TRUE;
		}
	    }
	    break;
#endif
	}
	}

	if (!new_pix && devPriv->pRotatedPixmap)
	{
	    cfbDestroyPixmap(devPriv->pRotatedPixmap);
	    devPriv->pRotatedPixmap = (PixmapPtr) NULL;
	}
    }

    if (new_rrop)
    {
	int old_rrop;

	/* ---- */
	if (pGC->fillStyle == FillStippled)
	    pSkyPriv->BgFgMix = GXnoop << 8 | pGC->alu;
	else
	    pSkyPriv->BgFgMix = pGC->alu << 8 | pGC->alu;

	if (pSkyPriv->fillMode == fill_stipple)
	{
	    if (pGC->fillStyle != FillTiled)
	    {
		pSkyPriv->stipple.fgPixel = pGC->fgPixel;
		pSkyPriv->stipple.bgPixel = pGC->bgPixel;
	    }
	}
	/* ---- */

	old_rrop = devPriv->rop;
	devPriv->rop = cfbReduceRasterOp (pGC->alu, pGC->fgPixel,
					   pGC->planemask,
					   &devPriv->and, &devPriv->xor);
	if (old_rrop == devPriv->rop)
	    new_rrop = FALSE;
	else
	{
#if PSZ == 8
	    new_line = TRUE;
	    new_text = TRUE;
#endif
	    new_fillspans = TRUE;
	    new_fillarea = TRUE;
	}
    }

    if (new_rrop || new_fillspans || new_text || new_fillarea || new_line)
    {
	GCOps	*newops;

	if (newops = skyMatchCommon (pDrawable, pGC, devPriv))
 	{
	    if (pGC->ops->devPrivate.val)
		skyDestroyOps (pGC->ops);
	    pGC->ops = newops;
	    new_rrop = new_line = new_fillspans = new_text = new_fillarea = 0;
	}
 	else
 	{
	    if (!pGC->ops->devPrivate.val)
	    {
		pGC->ops = skyCreateOps (pGC->ops);
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
		pGC->ops->FillPolygon = cfbFillPoly1RectCopy;
		break;
	    default:
		pGC->ops->FillPolygon = cfbFillPoly1RectGeneral;
		break;
	    }
	}
	if (pGC->lineWidth == 0)
	{
#if PSZ == 8
	    if ((pGC->lineStyle == LineSolid) && (pGC->fillStyle == FillSolid))
	    {
		switch (devPriv->rop)
		{
		case GXxor:
		    pGC->ops->PolyArc = cfbZeroPolyArcSS8Xor;
		    break;
		case GXcopy:
		    pGC->ops->PolyArc = cfbZeroPolyArcSS8Copy;
		    break;
		default:
		    pGC->ops->PolyArc = cfbZeroPolyArcSS8General;
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
#if PSZ == 8
		    if (devPriv->oneRect)
		    {
			pGC->ops->Polylines = cfb8LineSS1Rect;
			pGC->ops->PolySegment = cfb8SegmentSS1Rect;
		    } else
#endif
		    {
		    	pGC->ops->Polylines = cfbLineSS;
		    	pGC->ops->PolySegment = cfbSegmentSS;
		    }
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
		pGC->ops->Polylines = cfbLineSD;
		pGC->ops->PolySegment = cfbSegmentSD;
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
#if PSZ == 8
	    if (pGC->fillStyle == FillSolid)
	    {
		if (devPriv->rop == GXcopy)
		    pGC->ops->PolyGlyphBlt = cfbPolyGlyphBlt8;
		else
		    pGC->ops->PolyGlyphBlt = cfbPolyGlyphRop8;
	    }
	    else
#endif
		pGC->ops->PolyGlyphBlt = miPolyGlyphBlt;
            /* special case ImageGlyphBlt for terminal emulator fonts */
            if (TERMINALFONT(pGC->font) &&
		(pGC->planemask & PMSK) == PMSK
#if PSZ == 8
		&& FONTMAXBOUNDS(pGC->font,characterWidth) >= 4
#endif
		)
	    {
#if PSZ == 8
                pGC->ops->ImageGlyphBlt = cfbTEGlyphBlt8;
#else
                pGC->ops->ImageGlyphBlt = cfbTEGlyphBlt;
#endif
	    }
            else
	    {
#if PSZ == 8
		if (devPriv->rop == GXcopy &&
		    pGC->fillStyle == FillSolid &&
		    (pGC->planemask & PMSK) == PMSK)
		    pGC->ops->ImageGlyphBlt = cfbImageGlyphBlt8;
		else
#endif
		    pGC->ops->ImageGlyphBlt = miImageGlyphBlt;
	    }
        }
    }    


    if (new_fillspans) {
	switch (pGC->fillStyle) {
	case FillSolid:
	    switch (devPriv->rop) {
	    case GXcopy:
		pGC->ops->FillSpans = cfbSolidSpansCopy;
		break;
	    case GXxor:
		pGC->ops->FillSpans = cfbSolidSpansXor;
		break;
	    default:
		pGC->ops->FillSpans = cfbSolidSpansGeneral;
		break;
	    }
	    break;
	case FillTiled:
	    if (devPriv->pRotatedPixmap)
	    {
		if (pGC->alu == GXcopy && (pGC->planemask & PMSK) == PMSK)
		    pGC->ops->FillSpans = cfbTile32FSCopy;
		else
		    pGC->ops->FillSpans = cfbTile32FSGeneral;
	    }
	    else
		pGC->ops->FillSpans = cfbUnnaturalTileFS;
	    break;
	case FillStippled:
#if PSZ == 8
	    if (devPriv->pRotatedPixmap)
		pGC->ops->FillSpans = cfb8Stipple32FS;
	    else
#endif
		pGC->ops->FillSpans = cfbUnnaturalStippleFS;
	    break;
	case FillOpaqueStippled:
#if PSZ == 8
	    if (devPriv->pRotatedPixmap)
		pGC->ops->FillSpans = cfb8OpaqueStipple32FS;
	    else
#endif
		pGC->ops->FillSpans = cfbUnnaturalStippleFS;
	    break;
	default:
	    FatalError("skyValidateGC: illegal fillStyle\n");
	}
    } /* end of new_fillspans */

    if (new_fillarea) {
#if PSZ != 8
	pGC->ops->PolyFillRect = miPolyFillRect;
	if (pGC->fillStyle == FillSolid || pGC->fillStyle == FillTiled
		|| pSkyPriv->fillMode != punt)
	{
	    pGC->ops->PolyFillRect = skyPolyFillRect;
	}
#endif
#if PSZ == 8
	pGC->ops->PushPixels = mfbPushPixels;
	if (pGC->fillStyle == FillSolid && devPriv->rop == GXcopy)
	    pGC->ops->PushPixels = cfbPushPixels8;
#endif
	pGC->ops->PolyFillArc = miPolyFillArc;
	if (pGC->fillStyle == FillSolid)
	{
	    switch (devPriv->rop)
	    {
	    case GXcopy:
		pGC->ops->PolyFillArc = cfbPolyFillArcSolidCopy;
		break;
	    default:
		pGC->ops->PolyFillArc = cfbPolyFillArcSolidGeneral;
		break;
	    }
	}
    }
}

void
skyDestroyClip(pGC)
    GCPtr	pGC;
{
    if(pGC->clientClipType == CT_NONE)
	return;
    else if (pGC->clientClipType == CT_PIXMAP)
    {
	cfbDestroyPixmap((PixmapPtr)(pGC->clientClip));
    }
    else
    {
	/* we know we'll never have a list of rectangles, since
	   ChangeClip immediately turns them into a region 
	*/
        REGION_DESTROY(pGC->pScreen, pGC->clientClip);
    }
    pGC->clientClip = NULL;
    pGC->clientClipType = CT_NONE;
}

void
skyChangeClip(pGC, type, pvalue, nrects)
    GCPtr	pGC;
    int		type;
    pointer	pvalue;
    int		nrects;
{
    skyDestroyClip(pGC);
    if(type == CT_PIXMAP)
    {
	pGC->clientClip = (pointer) BITMAP_TO_REGION(pGC->pScreen, (PixmapPtr)pvalue);
	(*pGC->pScreen->DestroyPixmap)(pvalue);
    }
    else if (type == CT_REGION) {
	/* stuff the region in the GC */
	pGC->clientClip = pvalue;
    }
    else if (type != CT_NONE)
    {
	pGC->clientClip = (pointer) RECTS_TO_REGION(pGC->pScreen, nrects,
						    (xRectangle *)pvalue,
						    type);
	xfree(pvalue);
    }
    pGC->clientClipType = (type != CT_NONE && pGC->clientClip) ? CT_REGION :
								 CT_NONE;
    pGC->stateChanges |= GCClipMask;
}

void
skyCopyClip (pgcDst, pgcSrc)
    GCPtr pgcDst, pgcSrc;
{
    RegionPtr prgnNew;

    switch(pgcSrc->clientClipType)
    {
      case CT_PIXMAP:
	((PixmapPtr) pgcSrc->clientClip)->refcnt++;
	/* Fall through !! */
      case CT_NONE:
        skyChangeClip(pgcDst, (int)pgcSrc->clientClipType, pgcSrc->clientClip,
		      0);
        break;
      case CT_REGION:
        prgnNew = REGION_CREATE(pgcSrc->pScreen, NULL, 1);
        REGION_COPY(pgcSrc->pScreen, prgnNew,
                                       (RegionPtr)(pgcSrc->clientClip));
        skyChangeClip(pgcDst, CT_REGION, (pointer)prgnNew, 0);
        break;
    }
}

/*ARGSUSED*/
void
skyCopyGC (pGCSrc, changes, pGCDst)
    GCPtr	pGCSrc;
    Mask 	changes;
    GCPtr	pGCDst;
{
    return;
}

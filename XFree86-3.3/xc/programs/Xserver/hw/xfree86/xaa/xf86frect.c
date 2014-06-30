/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86frect.c,v 3.10.2.2 1997/05/18 12:00:21 dawes Exp $ */

/*
 * Fill rectangles.
 */

/*

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
*/

/*
 * This implements a generic XFree86 PolyFillRect.
 * It dispatches to a lower-level function.
 *
 * Partly based on mach64frect.c.
 * Written by Harm Hanemaayer (H.Hanemaayer@inter.nl.net).
 *
*/

/* $XConsortium: cfbfillrct.c,v 5.18 94/04/17 20:28:47 dpw Exp $ */
/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86frect.c,v 3.10.2.2 1997/05/18 12:00:21 dawes Exp $ */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "regionstr.h"
/* PSZ doesn't matter. */
#define PSZ 8
#include "cfb.h"
#include "cfb16.h"
#include "cfb24.h"
#include "cfb32.h"

#include "xf86.h"
#include "xf86xaa.h"
#include "xf86local.h"
#include "xf86pcache.h"

#define NUM_STACK_RECTS	1024


static void
xf86FillRectTileCached(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    register GCPtr pGC,
    int		nBox,
    BoxPtr	pBoxInit
#endif
);

/* For performance: if only one box, see if it needs to be cached. */
/* Don't cache if area being filled is smaller than the pattern.   */

#define DO_CACHE_PATTERN(nBox, pBox, pPixmap)                     \
    (!(((nBox) == 1) &&                                           \
       ((pBox)->x2 - (pBox)->x1 <= (pPixmap)->drawable.width) &&  \
       ((pBox)->y2 - (pBox)->y1 <= (pPixmap)->drawable.height)))


static Bool UsingStippleFallBack = FALSE;

void
xf86PolyFillRect(pDrawable, pGC, nrectFill, prectInit)
    DrawablePtr pDrawable;
    register GCPtr pGC;
    int		nrectFill; 	/* number of rectangles to fill */
    xRectangle	*prectInit;  	/* Pointer to first rectangle to fill */
{
    xRectangle	    *prect;
    RegionPtr	    prgnClip;
    register BoxPtr pbox;
    register BoxPtr pboxClipped;
    BoxPtr	    pboxClippedBase;
    BoxPtr	    pextent;
    BoxRec	    stackRects[NUM_STACK_RECTS];
    cfbPrivGC	    *priv;
    int		    numRects;
    void	    (*BoxFill)();
    int		    n;
    int		    xorg, yorg;
    int		    nboxFill;

    /* No bullshit please. */
    if (nrectFill <= 0)
        return;

    /*
     * The cfb code at one point (in cfbigblt8.c) cheats by not
     * doing a ValidateGC when changing the foreground color.
     * To repair the damage we must explictly check any color
     * restrictions here.
     */
    if (pGC->fillStyle == FillSolid &&
    (xf86GCInfoRec.PolyFillRectSolidFlags & RGB_EQUAL) &&
    ((pGC->fgPixel & 0xFF) != ((pGC->fgPixel & 0xFF00) >> 8) ||
    (pGC->fgPixel & 0xFF) != ((pGC->fgPixel & 0xFF0000) >> 16))) {
        (*xf86GCInfoRec.PolyFillRectSolidFallBack)(pDrawable, pGC,
            nrectFill, prectInit);
        return;
    }

    /*
     * cfb does "priv = cfbGetGCPrivate(pGC);" here. But since we compile
     * this function only once shared for all depths, I am not sure it
     * is safe to use it. What confuses me is that cfbGCPrivateIndex seems
     * to be declared globally seperately in cfb8, cfb16 etc. with the same
     * name.
     */
    priv = cfbGetGCPrivate(pGC);
    prgnClip = priv->pCompositeClip;

    prect = prectInit;
    xorg = pDrawable->x;
    yorg = pDrawable->y;
    if (xorg || yorg)
    {
	prect = prectInit;
	n = nrectFill;
	while(n--)
	{
	    prect->x += xorg;
	    prect->y += yorg;
	    prect++;
	}
    }

    prect = prectInit;

    numRects = REGION_NUM_RECTS(prgnClip) * nrectFill;
    if (numRects > NUM_STACK_RECTS)
    {
	pboxClippedBase = (BoxPtr)ALLOCATE_LOCAL(numRects * sizeof(BoxRec));
	if (!pboxClippedBase)
	    return;
    }
    else
	pboxClippedBase = stackRects;

    pboxClipped = pboxClippedBase;
	
    if (REGION_NUM_RECTS(prgnClip) == 1)
    {
	int x1, y1, x2, y2, bx2, by2;

	pextent = REGION_RECTS(prgnClip);
	x1 = pextent->x1;
	y1 = pextent->y1;
	x2 = pextent->x2;
	y2 = pextent->y2;
    	while (nrectFill--)
    	{
	    if ((pboxClipped->x1 = prect->x) < x1)
		pboxClipped->x1 = x1;
    
	    if ((pboxClipped->y1 = prect->y) < y1)
		pboxClipped->y1 = y1;
    
	    bx2 = (int) prect->x + (int) prect->width;
	    if (bx2 > x2)
		bx2 = x2;
	    pboxClipped->x2 = bx2;
    
	    by2 = (int) prect->y + (int) prect->height;
	    if (by2 > y2)
		by2 = y2;
	    pboxClipped->y2 = by2;

	    prect++;
	    if ((pboxClipped->x1 < pboxClipped->x2) &&
		(pboxClipped->y1 < pboxClipped->y2))
	    {
		pboxClipped++;
	    }
    	}
    }
    else
    {
	int x1, y1, x2, y2, bx2, by2;

	pextent = REGION_EXTENTS(pGC->pScreen, prgnClip);
	x1 = pextent->x1;
	y1 = pextent->y1;
	x2 = pextent->x2;
	y2 = pextent->y2;
    	while (nrectFill--)
    	{
	    BoxRec box;
    
	    if ((box.x1 = prect->x) < x1)
		box.x1 = x1;
    
	    if ((box.y1 = prect->y) < y1)
		box.y1 = y1;
    
	    bx2 = (int) prect->x + (int) prect->width;
	    if (bx2 > x2)
		bx2 = x2;
	    box.x2 = bx2;
    
	    by2 = (int) prect->y + (int) prect->height;
	    if (by2 > y2)
		by2 = y2;
	    box.y2 = by2;
    
	    prect++;
    
	    if ((box.x1 >= box.x2) || (box.y1 >= box.y2))
	    	continue;
    
	    n = REGION_NUM_RECTS (prgnClip);
	    pbox = REGION_RECTS(prgnClip);
    
	    /* clip the rectangle to each box in the clip region
	       this is logically equivalent to calling Intersect()
	    */
	    while(n--)
	    {
		pboxClipped->x1 = max(box.x1, pbox->x1);
		pboxClipped->y1 = max(box.y1, pbox->y1);
		pboxClipped->x2 = min(box.x2, pbox->x2);
		pboxClipped->y2 = min(box.y2, pbox->y2);
		pbox++;

		/* see if clipping left anything */
		if(pboxClipped->x1 < pboxClipped->x2 && 
		   pboxClipped->y1 < pboxClipped->y2)
		{
		    pboxClipped++;
		}
	    }
    	}
    }
    if (pboxClipped == pboxClippedBase) {
        if (pboxClippedBase != stackRects)
    	    DEALLOCATE_LOCAL(pboxClippedBase);
        return;
    }
    nboxFill = pboxClipped - pboxClippedBase;

    /*
     * This seems to be a better place to set BoxFill. Here we know what
     * areas need to be filled, which might affect caching decisions.
     */
    /*
     * Now look out. We have already checked the fill style, plane mask
     * and raster operations in ValidateGC, so this function will
     * never be called if we can't actually accelerate it. We only need
     * a fall-back in case we do caching of tiles or stipples, but
     * can't use the cache.
     *
     * If it turns out that PolyFillRect may actually be called with
     * an unvalidated GC, then all bets are off.
     */
    switch (pGC->fillStyle)
    {
    case FillSolid:
        BoxFill = xf86AccelInfoRec.FillRectSolid;
	break;
    case FillTiled:
        if ((xf86AccelInfoRec.Flags & PIXMAP_CACHE)
        && DO_CACHE_PATTERN(nboxFill, pboxClippedBase, pGC->tile.pixmap)
        && xf86CacheTile(pGC->tile.pixmap)) {
            BoxFill = xf86FillRectTileCached;
        }
        else
            if (xf86AccelInfoRec.FillRectTiled)
                BoxFill = xf86AccelInfoRec.FillRectTiled;
            else
                BoxFill = xf86AccelInfoRec.FillRectTiledFallBack;
	break;
    case FillStippled:
        if ((xf86AccelInfoRec.Flags & PIXMAP_CACHE)
        && (!(xf86AccelInfoRec.Flags & DO_NOT_CACHE_STIPPLES))
        && DO_CACHE_PATTERN(nboxFill, pboxClippedBase, pGC->stipple)
        && xf86CacheStipple(pDrawable, pGC)) {
            BoxFill = xf86FillRectTileCached;
        }
        else
            if (xf86AccelInfoRec.FillRectStippled &&
                /*
                 * There might be a problem here. The restriction
                 * flags for entering this function come from
                 * CopyAreaFlags. Now that we are not using the
                 * cache, other restriction flags (such as those
                 * defined for color expansion) should have been
                 * checked, but have not.
                 * This has been temporarily solved by a secondary flag,
                 * which indicates the restrictions for color expansion
                 * acceleration of stipples, and is explicitly checked
                 * here.
                 */
            CHECKTRANSPARENCYROP(
                xf86GCInfoRec.SecondaryPolyFillRectStippledFlags) &&
            CHECKRGBEQUAL(xf86GCInfoRec.SecondaryPolyFillRectStippledFlags) &&
            CHECKPLANEMASK(xf86GCInfoRec.SecondaryPolyFillRectStippledFlags))
                BoxFill = xf86AccelInfoRec.FillRectStippled;
            else
                if (xf86AccelInfoRec.FillRectStippledFallBack) {
                    UsingStippleFallBack = TRUE;
                    BoxFill = xf86AccelInfoRec.FillRectStippledFallBack;
                }
                else {
                    ErrorF("No stipple function\n");
                    return;
                }
	break;
    case FillOpaqueStippled:
        if ((xf86AccelInfoRec.Flags & PIXMAP_CACHE)
        && (!(xf86AccelInfoRec.Flags & DO_NOT_CACHE_STIPPLES))
        && DO_CACHE_PATTERN(nboxFill, pboxClippedBase, pGC->stipple)
        && xf86CacheStipple(pDrawable, pGC)) {
            BoxFill = xf86FillRectTileCached;
        }
        else
            if (xf86AccelInfoRec.FillRectOpaqueStippled &&
            CHECKROP(xf86GCInfoRec.SecondaryPolyFillRectOpaqueStippledFlags) &&
            CHECKRGBEQUALBOTH(
                xf86GCInfoRec.SecondaryPolyFillRectOpaqueStippledFlags) &&
            CHECKPLANEMASK(
                xf86GCInfoRec.SecondaryPolyFillRectOpaqueStippledFlags))
                BoxFill = xf86AccelInfoRec.FillRectOpaqueStippled;
            else
                if (xf86AccelInfoRec.FillRectOpaqueStippledFallBack) {
                    UsingStippleFallBack = TRUE;
                    BoxFill = xf86AccelInfoRec.FillRectOpaqueStippledFallBack;
                }
                else {
                    ErrorF("No stipple function\n");
                    return;
                }
	break;
    }

    (*BoxFill) (pDrawable, pGC, nboxFill, pboxClippedBase);
    if (UsingStippleFallBack)
        UsingStippleFallBack = FALSE;
    if (pboxClippedBase != stackRects)
    	DEALLOCATE_LOCAL(pboxClippedBase);
}


void
xf86SimpleFillRectSolid(pDrawable, pGC, nBox, pBoxInit)
    DrawablePtr pDrawable;
    register GCPtr pGC;
    int		nBox; 		/* number of rectangles to fill */
    BoxPtr	pBoxInit;  	/* Pointer to first rectangle to fill */
{
    BoxPtr pBox;
    pBox = pBoxInit;
    xf86AccelInfoRec.SetupForFillRectSolid(
        pGC->fgPixel, pGC->alu, pGC->planemask);
    while (nBox > 0) {
        int w, h;
        w = pBox->x2 - pBox->x1;
        h = pBox->y2 - pBox->y1;
        if (w > 0 && h > 0)
            xf86AccelInfoRec.SubsequentFillRectSolid(
                pBox->x1, pBox->y1, w, h);
	pBox++;
	nBox--;
    }
    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
        xf86AccelInfoRec.Sync();
}

/*
 * This a low-level fall-back for stipples at > 8bpp, for which no
 * cfb function is available. It dispatches to miPolyFillRect.
 * The main reason for the existence of this function is that we don't
 * want to consider the depth in xf86PolyFillRect.
 *
 * The specified boxes are assumed to be already clipped.
 * This function may be called to draw a stipple into the cache, in which
 * case we must define a new clipping region.
 */

void
xf86miFillRectStippledFallBack(pDrawable, pGC, nBox, pBox)
    DrawablePtr	    pDrawable;
    GCPtr	    pGC;
    int		    nBox;
    BoxPtr	    pBox;
{
    int i;
    xRectangle *pRect;
    void (*SavedFillSpans)(
#if NeedNestedPrototypes
        DrawablePtr pDrawable,
        GCPtr	pGC,
        int	nInit,
        DDXPointPtr pptInit,
        int	*pwidthInit,
        int	fSorted
#endif
    );
    pRect = (xRectangle *)ALLOCATE_LOCAL(sizeof(xRectangle) * nBox);
    for (i = 0; i < nBox; i++) {
        if (UsingStippleFallBack) {
            /*
             * When xf86PolyFillRect clipped, it added the origin of the
             * drawable to the box coordinates. We must reverse that.
             */
            pRect[i].x = pBox[i].x1 - pDrawable->x;
            pRect[i].y = pBox[i].y1 - pDrawable->y;
        }
        else {
            /*
             * Normally this case happens when uploading a stipple to
             * the pixmap cache.
             */
            pRect[i].x = pBox[i].x1;
            pRect[i].y = pBox[i].y1;
        }
        pRect[i].width = pBox[i].x2 - pBox[i].x1;
        pRect[i].height = pBox[i].y2 - pBox[i].y1;
    }
    SavedFillSpans = pGC->ops->FillSpans;
    switch (pDrawable->bitsPerPixel) {
    case 16 :
        pGC->ops->FillSpans = cfb16UnnaturalStippleFS;
        break;
    case 24 :
        pGC->ops->FillSpans = cfb24UnnaturalStippleFS;
        break;
    case 32 :
        pGC->ops->FillSpans = cfb32UnnaturalStippleFS;
        break;
    }
    miPolyFillRect(pDrawable, pGC, nBox, pRect);
    pGC->ops->FillSpans = SavedFillSpans;
    DEALLOCATE_LOCAL(pRect);
}


/*
 * This is a non-clipping version of the cached pattern fill function.
 * It might be beneficial to do clipping on the fly like the Mach64 version.
 * The non-clipping version might be useful for some cases.
 *
 * There are differences with the Mach64 version; for example, the
 * origin of the drawable has already been added to the boxes specified.
 */

#define CLIP_X(adjLeftX, fillPatWidth, rectX1, rectX2, rectXOffset,	 \
	       rectLeftX, rectWidth)                 			 \
    {									 \
        if ((((adjLeftX) + (fillPatWidth)) < (rectX1)) || 		 \
	    ((adjLeftX) > (rectX2))) 					 \
	{ 								 \
            continue;                                                    \
	}  								 \
        if ((adjLeftX) < (rectX1))                                       \
        {                                                                \
            (rectLeftX) = (rectX1);                                      \
            (rectXOffset) = (rectX1) - (adjLeftX);                       \
        }                                                                \
        else                                                             \
        {                                                                \
            (rectLeftX) = (adjLeftX);                                    \
            (rectXOffset) = 0;                                           \
        }                                                                \
        if (((adjLeftX) + (fillPatWidth)) > (rectX2))                    \
        {                                                                \
            (rectWidth) = (rectX2) - (adjLeftX) - (rectXOffset);         \
        }                                                                \
        else                                                             \
        {                                                                \
            (rectWidth) = (fillPatWidth) - (rectXOffset);                \
        }                                                                \
    }

#define CLIP_Y(adjTopY, fillPatWidth, rectY1, rectY2, rectYOffset, 	 \
	       rectTopY, rectHeight)                   			 \
    {									 \
	int _rectBotY; /* used to find the rectHeight */		 \
									 \
        if ((((adjTopY) + (fillPatHeight)) < (rectY1)) || 		 \
	    ((adjTopY) > (rectY2)))  					 \
	{ 								 \
            continue;                                                    \
	} 								 \
        rectTopY = max((rectY1), (adjTopY));                             \
        _rectBotY = min((rectY2), ((adjTopY) + (fillPatHeight)));        \
        if ((adjTopY) < (rectTopY))                                      \
	{ 								 \
	    (rectYOffset) = ((rectTopY) - (adjTopY)) % (fillPatHeight);  \
	} 								 \
        else                                                             \
	{ 								 \
            (rectYOffset) = 0;                                           \
	} 								 \
        (rectHeight) = _rectBotY - (rectTopY);				 \
    }

static void RotatePattern(pattern, xoffset, yoffset)
    unsigned char *pattern;
    int xoffset;
    int yoffset;
{
    int y;
    unsigned char old_pattern[8];
    y = yoffset;
    memcpy(old_pattern, pattern, 8);
    if (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_BIT_ORDER_MSBFIRST) {
        pattern[0] = (old_pattern[y] << xoffset) | (old_pattern[y] >> (8 - xoffset));
        y = (y + 1) & 7;
        pattern[1] = (old_pattern[y] << xoffset) | (old_pattern[y] >> (8 - xoffset));
        y = (y + 1) & 7;
        pattern[2] = (old_pattern[y] << xoffset) | (old_pattern[y] >> (8 - xoffset));
        y = (y + 1) & 7;
        pattern[3] = (old_pattern[y] << xoffset) | (old_pattern[y] >> (8 - xoffset));
        y = (y + 1) & 7;
        pattern[4] = (old_pattern[y] << xoffset) | (old_pattern[y] >> (8 - xoffset));
        y = (y + 1) & 7;
        pattern[5] = (old_pattern[y] << xoffset) | (old_pattern[y] >> (8 - xoffset));
        y = (y + 1) & 7;
        pattern[6] = (old_pattern[y] << xoffset) | (old_pattern[y] >> (8 - xoffset));
        y = (y + 1) & 7;
        pattern[7] = (old_pattern[y] << xoffset) | (old_pattern[y] >> (8 - xoffset));
    }
    else {
        pattern[0] = (old_pattern[y] >> xoffset) | (old_pattern[y] << (8 - xoffset));
        y = (y + 1) & 7;
        pattern[1] = (old_pattern[y] >> xoffset) | (old_pattern[y] << (8 - xoffset));
        y = (y + 1) & 7;
        pattern[2] = (old_pattern[y] >> xoffset) | (old_pattern[y] << (8 - xoffset));
        y = (y + 1) & 7;
        pattern[3] = (old_pattern[y] >> xoffset) | (old_pattern[y] << (8 - xoffset));
        y = (y + 1) & 7;
        pattern[4] = (old_pattern[y] >> xoffset) | (old_pattern[y] << (8 - xoffset));
        y = (y + 1) & 7;
        pattern[5] = (old_pattern[y] >> xoffset) | (old_pattern[y] << (8 - xoffset));
        y = (y + 1) & 7;
        pattern[6] = (old_pattern[y] >> xoffset) | (old_pattern[y] << (8 - xoffset));
        y = (y + 1) & 7;
        pattern[7] = (old_pattern[y] >> xoffset) | (old_pattern[y] << (8 - xoffset));
    }
}

void
xf86FillRectTileCached(pDrawable, pGC, nBoxInit, pBoxInit)
    DrawablePtr pDrawable;
    register GCPtr pGC;
    int		nBoxInit;	/* number of rectangles to fill */
    BoxPtr	pBoxInit;  	/* Pointer to first rectangle to fill */
{
    cfbPrivGC           *pGCPriv;        /* Pointer to private GC */
    PixmapPtr    	 pPixmap;        /* Pixmap of the area to draw */
    register int  	 rectX1, rectX2; /* points used to find the width */
    register int  	 rectY1, rectY2; /* points used to find the height */
    register int  	 rectWidth;	 /* Width of the rect to be drawn */
    register int  	 rectHeight;     /* Height of the rect to be drawn */
    register BoxPtr      pBox;	         /* current rectangle to fill */
    register int  	 nBox;           /* Number of rectangles to fill */
    int  		 rectLeftX;      /* Upper left corner of rect to draw */
    int  		 rectTopY;       /* Upper left corner of rect to draw */
    int  		 rectXOffset;	 /* Difference between the clip rect
					    smallest x value and adjusted left 
					    corner of the rect being drawn, 
					    if inside clip rect, else 0 */
    int  		 rectYOffset;	 /* Difference between the clip rect
					    smallest y value and adjusted left 
					    corner of the rect being drawn, 
					    if inside clip rect, else 0 */
    int  		 patLeftX;	 /* Upper left corner of bounding 
					    box for alligned pattern */
    int  		 patTopY; 	 /* Upper left corner of bounding 
					    box for alligned pattern */
    int  		 patRightX;   	 /* Lower right corner of bounding 
					    box for alligned pattern */
    int  		 patBotY; 	 /* Lower right corner of bounding 
					    box for alligned pattern */
    int  		 adjLeftX;       /* adjusted up left corner of rect */
    int  		 adjTopY;        /* adjusted up left corner of rect */
    register int         drawableXOrg;   /* Drawables x origin */
    register int         drawableYOrg;   /* Drawables y origin */
    int 		 fillPatWidth;	 /* width of the fill pattern */
    int 		 fillPatHeight;  /* height of the fill pattern */
    int 		 fillPatXorg;    /* fill patterns x origin */
    int 		 fillPatYorg;    /* fill patterns y origin */
    CacheInfoPtr 	 pci;
    extern CacheInfoPtr xf86CacheInfo;

    if (pGC->fillStyle == FillTiled)
        pPixmap = pGC->tile.pixmap;
    else 
        pPixmap = pGC->stipple;

#ifdef PIXPRIV
    pci = &xf86CacheInfo[((xf86PixPrivPtr)(pPixmap->devPrivates[xf86PixmapIndex].ptr))->slot];
#else
    pci = &xf86CacheInfo[0];
#endif

    drawableXOrg = pDrawable->x;
    drawableYOrg = pDrawable->y;

   /* Check for 8x8 hardware pattern usage. */
        if (pci->flags == 1)
	{
	    int i, patternx, patterny;

            adjLeftX = ((pGC->patOrg.x + drawableXOrg) & 0x07);
            adjTopY = ((pGC->patOrg.y + drawableYOrg) & 0x07);

	    patternx = pci->x;
	    patterny = pci->y;
	    if (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_SCREEN_ORIGIN) {
	        /*
 	         * Since we have horizontally rotated copies on consecutive
	         * scanlines, rotation is very simple.
	         */
	        patterny += (- adjLeftX) & 7;
                /*
                 * Because we have two copies of the pattern on the scanline,
                 * vertical rotation is also easy.
                 */
	        patternx += ((- adjTopY) & 7) * 8;
	    }

            if (pGC->fillStyle == FillStippled)
    	        /* Setup for transparency compare. */
                xf86AccelInfoRec.SetupForFill8x8Pattern(
                    patternx, patterny, pGC->alu, pGC->planemask,
                    pci->bg_color);
            else
                /* Tiled and opaque stippled, no transparency. */
                xf86AccelInfoRec.SetupForFill8x8Pattern(
                    patternx, patterny, pGC->alu, pGC->planemask, -1);

            for (nBox = nBoxInit, pBox = pBoxInit; nBox > 0; nBox--, pBox++)
            {
                rectX1 = pBox->x1;
                rectY1 = pBox->y1;
                rectX2 = pBox->x2;
                rectY2 = pBox->y2;
    
                rectWidth = rectX2 - rectX1; 
                rectHeight = rectY2 - rectY1;

		if ((rectWidth > 0) && (rectHeight > 0)) {
	            if (xf86AccelInfoRec.Flags &
	            HARDWARE_PATTERN_PROGRAMMED_ORIGIN) {
	                /* Special case: origin offset is passed. */
                        if (xf86AccelInfoRec.Flags
                        & HARDWARE_PATTERN_SCREEN_ORIGIN)
                             xf86AccelInfoRec.SubsequentFill8x8Pattern(
                                 (- adjLeftX) & 7,
                                 (- adjTopY) & 7, 
                                 rectX1, rectY1, rectWidth,
                                 rectHeight);
                        else
                            xf86AccelInfoRec.SubsequentFill8x8Pattern(
                                (rectX1 - adjLeftX) & 7,
                                (rectY1 - adjTopY) & 7,
                                rectX1, rectY1, rectWidth,
                                rectHeight);
                    }
		    else
		    if (xf86AccelInfoRec.Flags
		    & HARDWARE_PATTERN_SCREEN_ORIGIN)
		        /* patternx, patterny are ignored in this case. */
		        xf86AccelInfoRec.SubsequentFill8x8Pattern(
		            patternx, patterny, rectX1, rectY1, rectWidth,
		            rectHeight);
		    else
		    if (xf86AccelInfoRec.Flags
		    & HARDWARE_PATTERN_NOT_LINEAR)
		        xf86AccelInfoRec.SubsequentFill8x8Pattern(
		            patternx + ((- adjLeftX) & 7),
			    patterny + ((- adjTopY) & 7), 
			    rectX1, rectY1, rectWidth,
		            rectHeight);
		    else
		        xf86AccelInfoRec.SubsequentFill8x8Pattern(
		            patternx + ((rectY1 - adjTopY) & 7) * 8,
		            patterny + ((rectX1 - adjLeftX) & 7),
		            rectX1, rectY1, rectWidth,
		            rectHeight);
		}

	    } /* end for loop through each rectangle to draw */

            if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
                xf86AccelInfoRec.Sync();
	    return;
	} /* end section to handle full-depth fixed patterns */

        if (pci->flags == 2)	/* Mono color-expanded pattern. */
	{
	    int i, patternx, patterny;

            adjLeftX = ((pGC->patOrg.x + drawableXOrg) & 0x07);
            adjTopY = ((pGC->patOrg.y + drawableYOrg) & 0x07);

	    if (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_PROGRAMMED_BITS) {
	        /*
	         * pattern data is passed in function arguments.
	         */
	        if (xf86AccelInfoRec.Flags &
	        HARDWARE_PATTERN_PROGRAMMED_ORIGIN) {
	            /*
	             * This is the sweet and easy case.
	             * Actually, if screen origin is also set,
	             * the origin offset could be passed here also
	             * since it will remain constant, but that would
	             * require extra arguments in the Setup function.
	             */
	            patternx = pci->pattern0;
	            patterny = pci->pattern1;
	        }
	        else
	            if (xf86AccelInfoRec.Flags
	            & HARDWARE_PATTERN_SCREEN_ORIGIN) {
	                /*
	                 * Programmed bits but no programmed origin,
	                 * but screen origin so that we only need to
	                 * rotate once.
	                 */
	                unsigned int pattern[2];
	                pattern[0] = pci->pattern0;
	                pattern[1] = pci->pattern1;
	                RotatePattern((unsigned char *)&pattern,
	                    (- adjLeftX) & 7,
	                    (- adjTopY) & 7);
	                patternx = pattern[0];
	                patterny = pattern[1];
	            }
	            /* Otherwise, rotate every time. */
	    }
            else {
                /*
                 * Video memory location of pattern data.
                 * Note that the x-coordinate is defined in "bit" or
                 * "stencil" units for a monochrome pattern.
                 */
	        patternx = pci->x * xf86AccelInfoRec.BitsPerPixel;
	        patterny = pci->y;
	        if ((xf86AccelInfoRec.Flags & HARDWARE_PATTERN_SCREEN_ORIGIN) 
		    && !(xf86AccelInfoRec.Flags & 
			 HARDWARE_PATTERN_PROGRAMMED_ORIGIN)) {
	            /*
 	             * Since we have horizontally rotated copies on consecutive
	             * scanlines, rotation is very simple.
	             */
	            patterny += (- adjLeftX) & 7;
                    /*
                     * Because we have 8 vertically rotated copies of the
                     * pattern on the scanline, vertical rotation is also easy.
                     */
	            patternx += ((- adjTopY) & 7) * 64;
	        }
	    }

            if (pGC->fillStyle == FillStippled)
    	        /* Setup for transparency compare. */
                xf86AccelInfoRec.SetupFor8x8PatternColorExpand(
                    patternx, patterny, -1, pGC->fgPixel,
                    pGC->alu, pGC->planemask);
            else
            if (pGC->fillStyle == FillTiled)
                /* Tiled. Colors from cache info. */
                xf86AccelInfoRec.SetupFor8x8PatternColorExpand(
                    patternx, patterny, pci->bg_color, pci->fg_color,
                    pGC->alu, pGC->planemask);
            else
                /* Opaque stippled, no transparency. */
                xf86AccelInfoRec.SetupFor8x8PatternColorExpand(
                    patternx, patterny, pGC->bgPixel, pGC->fgPixel,
                    pGC->alu, pGC->planemask);

            for (nBox = nBoxInit, pBox = pBoxInit; nBox > 0; nBox--, pBox++)
            {
                rectX1 = pBox->x1;
                rectY1 = pBox->y1;
                rectX2 = pBox->x2;
                rectY2 = pBox->y2;
    
                rectWidth = rectX2 - rectX1; 
                rectHeight = rectY2 - rectY1;

		if ((rectWidth > 0) && (rectHeight > 0)) {
	            if (xf86AccelInfoRec.Flags &
	            HARDWARE_PATTERN_PROGRAMMED_ORIGIN) {
	                /* Special case: origin offset is passed. */
	                if (xf86AccelInfoRec.Flags
	                & HARDWARE_PATTERN_SCREEN_ORIGIN)
		            xf86AccelInfoRec.Subsequent8x8PatternColorExpand(
		                (- adjLeftX) & 7,
		                (- adjTopY) & 7, 
		                rectX1, rectY1, rectWidth,
		                rectHeight);
		        else
		            xf86AccelInfoRec.Subsequent8x8PatternColorExpand(
		                (rectX1 - adjLeftX) & 7,
		                (rectY1 - adjTopY) & 7,
		                rectX1, rectY1, rectWidth,
		                rectHeight);
	            }
	            else
		    if (xf86AccelInfoRec.Flags
		    & HARDWARE_PATTERN_SCREEN_ORIGIN)
		        /* patternx, patterny are ignored in this case. */
		        xf86AccelInfoRec.Subsequent8x8PatternColorExpand(
		            patternx, patterny, rectX1, rectY1, rectWidth,
		            rectHeight);
		    else
	            if (xf86AccelInfoRec.Flags
	            & HARDWARE_PATTERN_PROGRAMMED_BITS) {
	                /*
	                 * Programmed bits but no programmed origin,
	                 * and no screen origin.
	                 * Rotate the bit pattern.
	                 */
	                unsigned int pattern[2];
	                pattern[0] = pci->pattern0;
	                pattern[1] = pci->pattern1;
	                RotatePattern((unsigned char *)&pattern,
	                    (rectX1 - adjLeftX) & 7,
	                    (rectY1 - adjTopY) & 7);
		        xf86AccelInfoRec.Subsequent8x8PatternColorExpand(
		            pattern[0], pattern[1],
		            rectX1, rectY1, rectWidth,
		            rectHeight);
	            }
	            else
		        xf86AccelInfoRec.Subsequent8x8PatternColorExpand(
		            patternx + ((rectY1 - adjTopY) & 7) * 64,
		            patterny + ((rectX1 - adjLeftX) & 7),
		            rectX1, rectY1, rectWidth,
		            rectHeight);
		}

	    } /* end for loop through each rectangle to draw */

            if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
                xf86AccelInfoRec.Sync();
	    return;
	} /* end section to handle color-expanded fixed patterns */

    if (pGC->fillStyle == FillStippled)
    	/* Setup for transparency compare. */
        xf86AccelInfoRec.SetupForScreenToScreenCopy(
            1, 1, pGC->alu, pGC->planemask, pci->bg_color);
    else
        /* Tiled and opaque stippled, no transparency. */
        xf86AccelInfoRec.SetupForScreenToScreenCopy(
            1, 1, pGC->alu, pGC->planemask, -1);

    fillPatWidth  = pci->w;
    fillPatHeight = pci->h;
    fillPatXorg   = pGC->patOrg.x + drawableXOrg;
    fillPatYorg   = pGC->patOrg.y + drawableYOrg;

    for (nBox = nBoxInit, pBox = pBoxInit; nBox > 0; nBox--, pBox++)
    {
        rectX1 = pBox->x1;
        rectY1 = pBox->y1;
        rectX2 = pBox->x2;
        rectY2 = pBox->y2;
        /*
         * set up x and y for the top left and bottom right
	 * corners of the alligned pattern 
	 */
        patLeftX = fillPatXorg;
        patTopY = fillPatYorg;

        while (patLeftX < rectX1) patLeftX += fillPatWidth;
        while (patLeftX > rectX1) patLeftX -= fillPatWidth;
        while (patTopY < rectY1) patTopY += fillPatHeight;
        while (patTopY > rectY1) patTopY -= fillPatHeight;

	patRightX = patLeftX;
        patBotY = patTopY;

        while (patRightX <= rectX2) patRightX += fillPatWidth;
        while (patBotY <= rectY2) patBotY += fillPatHeight;

	for (adjTopY = patTopY; 
	     adjTopY < patBotY; 
	     adjTopY += fillPatHeight)
	{
	    CLIP_Y(adjTopY, fillPatWidth, rectY1, rectY2,
		   rectYOffset, rectTopY, rectHeight);

            for (adjLeftX = patLeftX;
		 adjLeftX < patRightX;
		 adjLeftX += fillPatWidth)
            {
	        CLIP_X(adjLeftX, fillPatWidth, rectX1, rectX2,
		    rectXOffset, rectLeftX, rectWidth);

	        if ((rectWidth > 0) && (rectHeight > 0))
		    xf86AccelInfoRec.SubsequentScreenToScreenCopy(
			pci->x + rectXOffset, pci->y + rectYOffset,
			rectLeftX, rectTopY, rectWidth, rectHeight);
	    }
	}
    } /* end for loop through each rectangle to draw */

    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
        xf86AccelInfoRec.Sync();
}

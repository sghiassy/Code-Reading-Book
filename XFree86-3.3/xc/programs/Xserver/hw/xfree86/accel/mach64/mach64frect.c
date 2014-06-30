/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64frect.c,v 3.4 1996/12/23 06:39:15 dawes Exp $ */
/*
 * Copyright 1989 by the Massachusetts Institute of Technology
 * Copyright 1993,1994 by Kevin E. Martin, Chapel Hill, North Carolina.
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  M.I.T. makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 * 
 * Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * 
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)
 * 
 */
/* $XConsortium: mach64frect.c /main/3 1996/02/21 17:28:31 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "cfb.h"
#include "cfb16.h"
#include "cfb32.h"
#include "cfbmskbits.h"
#include "mergerop.h"
#include "mach64.h"
#include "mach64im.h"


static void mach64TileFillRect();

void
mach64PolyFillRect(pDrawable, pGC, nrectFill, prectInit)
    DrawablePtr pDrawable;
    register GCPtr pGC;
    int		nrectFill; 	/* number of rectangles to fill */
    xRectangle	*prectInit;  	/* Pointer to first rectangle to fill */
{

    if (!xf86VTSema)
    {
	switch (pDrawable->bitsPerPixel)
	{
	    case 8:
	        cfbPolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	        break;
	    case 16:
		if (pGC->fillStyle == FillSolid || pGC->fillStyle == FillTiled)
	        	cfb16PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
		else
	        	miPolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	        break;
	    case 32:
		if (pGC->fillStyle == FillSolid || pGC->fillStyle == FillTiled)
	        	cfb32PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
		else
	        	miPolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	        break;
	    default:
	        ErrorF("mach64PolyFillRect: unsupported depth %d \n",
		        pDrawable->bitsPerPixel);
	}
	return;
    }

    /*
     * Optimize for solid fills
     */
    if ((pGC->fillStyle == FillSolid) ||
        ((pGC->fillStyle == FillOpaqueStippled) &&
         (pGC->fgPixel == pGC->bgPixel)))
    { 
        int    nClipRects;     /* Number of clipping rectangles */
        BoxPtr pClipRects;     /* Current clipping box */
        int    clipXMin;       /* Upper left corner of clip rect */
        int    clipYMin;       /* Upper left corner of clip rect */
        int    clipXMax;       /* Lower right corner of clip rect */
        int    clipYMax;       /* Lower right corner of clip rect */
        int    drawableXOrg;   /* Drawables x origin */
        int    drawableYOrg;   /* Drawables y origin */
	cfbPrivGC *priv;

        drawableXOrg = pDrawable->x;
        drawableYOrg = pDrawable->y;

        priv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr);
        nClipRects = REGION_NUM_RECTS(priv->pCompositeClip);
	pClipRects = REGION_RECTS(priv->pCompositeClip);

        WaitQueue(5);
        regw(DP_FRGD_CLR, pGC->fgPixel);
	regw(DP_MIX, (mach64alu[pGC->alu] << 16) | MIX_DST);
        regw(DP_WRITE_MASK, pGC->planemask);
	regw(DP_SRC, FRGD_SRC_FRGD_CLR);
	regw(DST_CNTL, DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM);

	for(; nClipRects > 0; nClipRects--, pClipRects++)
        {
            xRectangle *pRect;   /* current rectangle to fill */
            int         nRects;  /* Number of rectangles to fill */

            clipYMin = pClipRects->y1;
            clipYMax = pClipRects->y2;
            clipXMin = pClipRects->x1;
            clipXMax = pClipRects->x2;

            for(nRects = nrectFill, pRect = prectInit;
                nRects > 0;
                nRects--, pRect++)
            {
                int rectX1;     /* points used to find the width */
                int rectX2;     /* points used to find the width */
                int rectY1;     /* points used to find the height */
                int rectY2;     /* points used to find the height */
                int rectWidth;  /* Width of the rect to be drawn */
                int rectHeight; /* Height of the rect to be drawn */

                /*
                 * Clip and translate each rect
                 */
                rectX1 = max((pRect->x + drawableXOrg), clipXMin);
                rectY1 = max((pRect->y + drawableYOrg), clipYMin);
                rectX2 = min((pRect->x + pRect->width + drawableXOrg),
                             clipXMax);
                rectY2 = min((pRect->y + pRect->height + drawableYOrg),
                             clipYMax);

                rectWidth = rectX2 - rectX1;
                rectHeight = rectY2 - rectY1;

                if ((rectWidth > 0) && (rectHeight > 0))
                {
                    WaitQueue(2);
		    regw(DST_Y_X, ((rectX1 << 16) | (rectY1 & 0x0000ffff)));
		    regw(DST_HEIGHT_WIDTH, ((rectWidth << 16) | rectHeight));
                }
            } /* end for loop through each rectangle to draw */
        } /* end for loop through each clip rectangle */

	WaitIdleEmpty(); /* Make sure that all commands have finished */

    } /* end section to draw solid patterns */
    else
    {
	mach64TileFillRect(pDrawable, pGC, nrectFill, prectInit);
    }
}

/****************************************************************************/
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


/****************************************************************************/
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

/***********************************************************************/
    /* For performance: if only one box, see if it needs to be cached. */
    /* Don't cache if area being filled is smaller than the pattern.   */

#define DO_CACHE_PATTERN(nRectInit, pRectInit, pPixmap)                 \
    (!(((nRectInit) == 1) &&                                            \
       ((pRectInit)->width <= (pPixmap)->drawable.width) &&             \
       ((pRectInit)->height <= (pPixmap)->drawable.height)))            \

/***********************************************************************/

static void
mach64TileFillRect(pDrawable, pGC, nRectInit, pRectInit)
    DrawablePtr	 pDrawable;
    GCPtr	 pGC;
    int		 nRectInit;
    xRectangle	*pRectInit;
{
    cfbPrivGC           *pGCPriv;        /* Pointer to private GC */
    int  		 nClipRects;     /* Number of clipping rectangles */
    BoxPtr 		 pClipRects;     /* Current clipping box */
    PixmapPtr    	 pPixmap;        /* Pixmap of the area to draw */
    register int  	 rectX1, rectX2; /* points used to find the width */
    register int  	 rectY1, rectY2; /* points used to find the height */
    register int  	 rectWidth;	 /* Width of the rect to be drawn */
    register int  	 rectHeight;     /* Height of the rect to be drawn */
    register int         clipXMin;       /* Upper left corner of clip rect */
    register int         clipYMin;       /* Upper left corner of clip rect */
    register int         clipXMax;       /* Lower right corner of clip rect */
    register int         clipYMax;       /* Lower right corner of clip rect */
    register xRectangle *pRect;	         /* current rectangle to fill */
    register int  	 nRects;         /* Number of rectangles to fill */
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
    int			 fgalu, bgalu;	 /* tmp field for the alu */
    int 		 opaque;
    int 		 pixWidth;
    CacheInfoPtr 	 pci;
    int 		 slot = 0;
    extern CacheInfoPtr mach64CacheInfo;


    drawableXOrg = pDrawable->x;
    drawableYOrg = pDrawable->y;

    pGCPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr);
    nClipRects = REGION_NUM_RECTS(pGCPriv->pCompositeClip);
    if (nClipRects < 1)
        return;

    pClipRects = REGION_RECTS(pGCPriv->pCompositeClip);

    fgalu = mach64alu[pGC->alu];

    switch (pGC->fillStyle)
    {
        case FillTiled:
            pPixmap = pGC->tile.pixmap;
	    if DO_CACHE_PATTERN(nRectInit, pRectInit, pPixmap){
	        slot = mach64CacheTile(pPixmap);
	    }
	    bgalu = fgalu;
	    opaque = 1;
            break;
        case FillStippled:
            pPixmap = pGC->stipple;
	    if DO_CACHE_PATTERN(nRectInit, pRectInit, pPixmap){
	        slot = mach64CacheStipple(pPixmap, pGC->fgPixel);
	    }
	    bgalu = MIX_SRC;
	    opaque = 0;
	    break;
        case FillOpaqueStippled:
            pPixmap = pGC->stipple;
	    if DO_CACHE_PATTERN(nRectInit, pRectInit, pPixmap){
	        slot = mach64CacheOpStipple(pPixmap, pGC->fgPixel, pGC->bgPixel);
	    }
	    bgalu = fgalu;
	    opaque = 1;
            break;
	default:
	    ErrorF("mach64TileFillRect: unknown fill style given in GC.\n");
	    return;
    }
    /*
     * See if pattern is in the offscreen cache 
     */
    if (slot) 
    {
#ifdef PIXPRIV
	pci = &mach64CacheInfo[((mach64PixPrivPtr)(pPixmap->devPrivates[mach64PixmapIndex].ptr))->slot];
	mach64IncrementCacheLRU(((mach64PixPrivPtr)(pPixmap->devPrivates[mach64PixmapIndex].ptr))->slot);
#else
	pci = &mach64CacheInfo[0];
	mach64IncrementCacheLRU(0);
#endif

        if (pci->flags)
	{
	    unsigned char pattern[8];
	    unsigned char rotated_pattern[8];
	    int i;
	    extern unsigned char reverseByteOrder[];

            adjLeftX = ((pGC->patOrg.x + drawableXOrg) & 0x07);
            adjTopY = ((pGC->patOrg.y + drawableYOrg) & 0x07);

	    *((int *)&pattern[0]) = pci->pattern0;
	    *((int *)&pattern[4]) = pci->pattern1;

	    for (i=0; i<8; i++)
	        pattern[i] = reverseByteOrder[pattern[i]];

	    /* rotate the pattern in the x direction */
	    for (i=0; i<8; i++)    
	        pattern[i] = (pattern[i] >> adjLeftX) | 
			     (pattern[i] << (8 - adjLeftX));

	    /* now rotate it in the y direction */
	    for (i=0; i<8; i++)
	        rotated_pattern[(i + adjTopY) % 8] = pattern[i];

	    WaitQueue(8);
	    regw(DP_FRGD_CLR, pci->fg_color);
	    regw(DP_BKGD_CLR, pci->bg_color);
	    regw(DP_WRITE_MASK, pGC->planemask);
	    if (opaque)
 	        regw(DP_MIX, (fgalu << 16) | bgalu);
	    else
	        regw(DP_MIX, (fgalu << 16) | MIX_DST);
	    regw(DP_SRC, MONO_SRC_PATTERN | FRGD_SRC_FRGD_CLR | BKGD_SRC_BKGD_CLR);

	    regw(PAT_REG0, *((int *)&rotated_pattern[0]));
	    regw(PAT_REG1, *((int *)&rotated_pattern[4]));

	    regw(GUI_TRAJ_CNTL, PAT_MONO_8x8_ENABLE | 
	                        DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM);

	    for(;nClipRects > 0; nClipRects--, pClipRects++)
            {
                clipYMin = pClipRects->y1;
                clipYMax = pClipRects->y2;
                clipXMin = pClipRects->x1;
                clipXMax = pClipRects->x2;

		for(nRects = nRectInit, pRect = pRectInit;
		    nRects > 0;
		    nRects--, pRect++)
	        {
    	            /*
     	             * Clip and translate each rect
     	             */
                    rectX1 = max((pRect->x + drawableXOrg), clipXMin);
                    rectY1 = max((pRect->y + drawableYOrg), clipYMin);
                    rectX2 = min((pRect->x + pRect->width + drawableXOrg),
				  clipXMax);
                    rectY2 = min((pRect->y + pRect->height + drawableYOrg),
				  clipYMax);
    
                    rectWidth = rectX2 - rectX1; 
                    rectHeight = rectY2 - rectY1;

		    if ((rectWidth > 0) && (rectHeight > 0))
		    {
			WaitQueue(2);
		        regw(DST_Y_X, (rectX1 << 16) | rectY1);
		        regw(DST_HEIGHT_WIDTH, (rectWidth << 16) | rectHeight);

		    } /* end if ((rectWidth > 0) && (rectHeight > 0)) */
	        } /* end for loop through each rectangle to draw */
	    } /* end for loop through each clip rectangle */ 

	    WaitQueue(1);
	    regw(GUI_TRAJ_CNTL, DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM);

	    WaitIdleEmpty(); /* Make sure that all commands have finished */

	} /* end section to handle fixed patterns */

	else 
       /*
        * handle non-fixed patterns 
        */
	{
            fillPatWidth  = pci->w;
            fillPatHeight = pci->h;
            fillPatXorg   = pGC->patOrg.x + drawableXOrg;
            fillPatYorg   = pGC->patOrg.y + drawableYOrg;

	    WaitQueue(6);
	    regw(DP_MIX, (fgalu << 16) | bgalu);
	    regw(DP_WRITE_MASK, pGC->planemask);
	    regw(DP_SRC, FRGD_SRC_BLIT);
	    regw(SRC_CNTL, 0x0);

	    if (!opaque)
	    {
	        /* enable source compare to do a transparent stipple */
	        regw(CLR_CMP_CNTL, COMPARE_SOURCE | COMPARE_EQUAL);
	        regw(CLR_CMP_CLR, pci->bg_color);
	    }

	    for(; nClipRects > 0; nClipRects--, pClipRects++)
            {
                clipYMin = pClipRects->y1;
                clipYMax = pClipRects->y2;
                clipXMin = pClipRects->x1;
                clipXMax = pClipRects->x2;

		for (nRects = nRectInit, pRect = pRectInit;
		     nRects > 0;
		     nRects--, pRect++)
	        {
    	            /*
     	             * Clip and translate each rect
     	             */
                    rectX1 = max((pRect->x + drawableXOrg), clipXMin);
                    rectY1 = max((pRect->y + drawableYOrg), clipYMin);
                    rectX2 = min((pRect->x + pRect->width + drawableXOrg),
				  clipXMax);
                    rectY2 = min((pRect->y + pRect->height + drawableYOrg), 
				  clipYMax);
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
			    {
			        /* copy portion of pattern not clipped */
				MACH64_BIT_BLT( pci->x + rectXOffset,
						pci->y + rectYOffset,
						rectLeftX, rectTopY,
						rectWidth, rectHeight,
						(DST_X_LEFT_TO_RIGHT | 
						 DST_Y_TOP_TO_BOTTOM));
			    }
		        }
	            }
	        } /* end for loop through each rectangle to draw */
	    } /* end for loop through each clip rectangle */ 

	    if (!opaque)
	    {
		WaitQueue(1);
	        regw(CLR_CMP_CNTL, COMPARE_FALSE);
	    }
	    WaitIdleEmpty(); /* Make sure that all commands have finished */

	} /* end section to handle non-fixed patterns */

	return;
    } /* end section to handle cachable patterns */

   /*
    * pattern is too large to use offscreen cache 
    */

    fillPatWidth  = pPixmap->drawable.width;
    fillPatHeight = pPixmap->drawable.height;
    fillPatXorg   = pGC->patOrg.x + drawableXOrg;
    fillPatYorg   = pGC->patOrg.y + drawableYOrg;
    pixWidth = PixmapBytePad(pPixmap->drawable.width, pPixmap->drawable.depth);

    for(; nClipRects > 0; nClipRects--, pClipRects++)
    {
        clipYMin = pClipRects->y1;
        clipYMax = pClipRects->y2;
        clipXMin = pClipRects->x1;
        clipXMax = pClipRects->x2;

	for (nRects = nRectInit, pRect = pRectInit;
	     nRects > 0;
	     nRects--, pRect++)
        {
    	    /*
     	     * Clip and translate each rect
     	     */
            rectX1 = max((pRect->x + drawableXOrg), clipXMin);
            rectY1 = max((pRect->y + drawableYOrg), clipYMin);
            rectX2 = min((pRect->x + pRect->width + drawableXOrg), clipXMax);
            rectY2 = min((pRect->y + pRect->height + drawableYOrg), clipYMax);
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
		    {
			switch(pGC->fillStyle)
			{
			    case FillTiled:
			        (*mach64ImageWriteFunc)(rectLeftX, rectTopY, 
					rectWidth, rectHeight,
                                        pPixmap->devPrivate.ptr, 
					pixWidth,
					rectXOffset, rectYOffset,
                                        mach64alu[pGC->alu], pGC->planemask);
			        break;

			    case FillStippled:
			        mach64ImageStippleFunc(rectLeftX, rectTopY, 
					rectWidth, rectHeight,
                                        pPixmap->devPrivate.ptr, 
					pixWidth,
					rectXOffset, rectYOffset,
					pGC->fgPixel, pGC->bgPixel,
                                        mach64alu[pGC->alu], pGC->planemask, 0);
			        break;

			    case FillOpaqueStippled:
			        mach64ImageStippleFunc(rectLeftX, rectTopY, 
					rectWidth, rectHeight,
                                        pPixmap->devPrivate.ptr, 
					pixWidth,
					rectXOffset, rectYOffset,
					pGC->fgPixel, pGC->bgPixel,
                                        mach64alu[pGC->alu], pGC->planemask, 1);
			        break;

			    default:
			        break;
			}
		    }
	        }
	    }
	} /* end for loop through each rectangle to draw */
    } /* end for loop through each clip rectangle */ 
}

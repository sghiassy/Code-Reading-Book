/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxFArc.c,v 3.4 1996/12/23 06:32:37 dawes Exp $ */
/************************************************************

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

Author:  Bob Scheifler, MIT X Consortium

********************************************************/

/* $XConsortium: agxFArc.c /main/5 1996/02/21 17:17:07 kaleb $ */

#include <math.h>
#include "X.h"
#include "Xprotostr.h"
#include "miscstruct.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "mifpoly.h"
#include "mi.h"
#include "cfb.h"
#include "mifillarc.h"
#include "agx.h"
#include "regagx.h"

#define AGXFILLSETUP() \
    GE_WAIT_IDLE(); \
    MAP_SET_SRC_AND_DST( GE_MS_MAP_A ); \
    GE_OUT_B(GE_FRGD_MIX, pGC->alu); \
    GE_OUT_D(GE_FRGD_CLR, pGC->fgPixel); \
    GE_OUT_D(GE_PIXEL_BIT_MASK, pGC->planemask); \
    GE_OUT_W( GE_PIXEL_OP, \
              GE_OP_PAT_FRGD \
              | GE_OP_MASK_DISABLED \
              | GE_OP_INC_X \
              | GE_OP_INC_Y         ); \
    GE_OUT_W( GE_OP_DIM_HEIGHT, 0 ); 

#define DRAWSPAN(x1,y1,w1) \
    GE_WAIT_IDLE(); \
    GE_OUT_W( GE_DEST_MAP_X, (x1) ); \
    GE_OUT_W( GE_DEST_MAP_Y, (y1) ); \
    GE_OUT_W( GE_OP_DIM_WIDTH, (w1)-1 ); \
    GE_START_CMDW( GE_OPW_BITBLT \
                   | GE_OPW_FRGD_SRC_CLR \
                   | GE_OPW_SRC_MAP_A \
                   | GE_OPW_DEST_MAP_A   ); 

#define DRAWSPANS() \
    if( clip ) { \
       n = miClipSpans( ((cfbPrivGC *) \
                           (pGC->devPrivates[cfbGCPrivateIndex].ptr)) \
                               ->pCompositeClip, \
                        &point, &width, 1, \
                        points, widths, TRUE ); \
       pts = points; \
       wids = widths; \
       for(i=0;i<n;i++) { \
          DRAWSPAN(pts->x,pts->y,*wids); \
          pts++; \
          wids++; \
       } \
    } \
    else { \
       DRAWSPAN(point.x,point.y,width); \
    }

#define ADDSPANS() \
    if( slw ) { \
       point.x = xorg - x; \
       point.y = yorg - y; \
       width = slw; \
       DRAWSPANS(); \
       if (miFillArcLower(slw)) { \
           point.x = xorg - x; \
           point.y = yorg + y + dy; \
           width = slw; \
           DRAWSPANS(); \
       } \
    }


static void
agxFillEllipseI(pDraw, pGC, arc, points, widths, priv)
    DrawablePtr pDraw;
    GCPtr pGC;
    xArc *arc;
    DDXPointPtr points;
    int *widths;
    cfbPrivGC *priv;
{
    int x, y, e;
    int yk, xk, ym, xm, dx, dy, xorg, yorg;
    int slw, n, i;
    miFillArcRec info;
    DDXPointRec point;
    DDXPointPtr pts;
    int width;
    int *wids;
    int halfWidth;
    int halfHeight;
    int xmin, xmax, ymin, ymax;
    RegionPtr prgnClip;
    BoxPtr clipbox;
    Bool clip;

    miFillArcSetup(arc, &info);
    MIFILLARCSETUP();

    if (pGC->miTranslate)
    {
	xorg += pDraw->x;
	yorg += pDraw->y;
    }

    /*
     * Avoid clipping on every scan line by seeing if the 
     * bounding box requires clipping.
     */
    clip = TRUE;
    prgnClip = priv->pCompositeClip;
    n = REGION_NUM_RECTS(prgnClip);
    clipbox = REGION_RECTS(prgnClip);
    halfWidth =  (arc->width + 1) >> 1;      
    halfHeight = (arc->height + 1) >> 1;
    xmin = xorg - halfWidth;
    xmax = xorg + halfWidth;
    ymin = yorg - halfHeight;
    ymax = yorg + halfHeight;
    while( clip && n-- ) {
       clip = xmin < clipbox->x1
              || xmax > clipbox->x2
              || ymin < clipbox->y1
              || ymax > clipbox->y2;
       clipbox++;
    }

    while (y > 0)
    {
       MIFILLARCSTEP(slw);
       ADDSPANS();
    }
}

static void
agxFillEllipseD(pDraw, pGC, arc, points, widths, priv)
    DrawablePtr pDraw;
    GCPtr pGC;
    xArc *arc;
    DDXPointPtr points;
    int *widths;
    cfbPrivGC *priv;
{
    int x, y, i;
    int xorg, yorg, dx, dy, slw, n;
    double e, yk, xk, ym, xm;
    miFillArcDRec info;
    DDXPointRec point;
    DDXPointPtr pts;
    int width;
    int *wids;
    int halfWidth;
    int halfHeight;
    int xmin, xmax, ymin, ymax;
    RegionPtr prgnClip;
    BoxPtr clipbox;
    Bool clip;

    miFillArcDSetup(arc, &info);
    MIFILLARCSETUP();

    if (pGC->miTranslate)
    {
	xorg += pDraw->x;
	yorg += pDraw->y;
    }

    /*
     * Avoid clipping on every scan line by seeing if the 
     * bounding box requires clipping.
     */
    clip = TRUE;
    prgnClip = priv->pCompositeClip;
    n = REGION_NUM_RECTS(prgnClip);
    clipbox = REGION_RECTS(prgnClip);
    halfWidth =  (arc->width + 1) >> 1;      
    halfHeight = (arc->height + 1) >> 1;
    xmin = xorg - halfWidth;
    xmax = xorg + halfWidth;
    ymin = yorg - halfHeight;
    ymax = yorg + halfHeight;
    while( clip && n-- ) {
       clip = xmin < clipbox->x1
              || xmax > clipbox->x2
              || ymin < clipbox->y1
              || ymax > clipbox->y2;
       clipbox++;
    }

    while (y > 0)
    {
       MIFILLARCSTEP(slw);
       ADDSPANS();
    }
}

#define ADDSPAN(l,r) \
    if ((r) >= (l)) { \
        point.x = (l); \
        point.y = (ya); \
        width = (r)-(l)+1; \
        DRAWSPANS(); \
    }
 
#define ADDSLICESPANS(flip) \
    if (!(flip)) { \
	ADDSPAN(xl, xr); \
    } \
    else { \
	xc = xorg - x; \
	ADDSPAN(xc, xr); \
	xc += slw - 1; \
	ADDSPAN(xl, xc); \
    }

static void
agxFillArcSliceI(pDraw, pGC, arc, points, widths, priv)
    DrawablePtr pDraw;
    GCPtr pGC;
    xArc *arc;
    DDXPointPtr points;
    int *widths;
    cfbPrivGC *priv;
{
    int yk, xk, ym, xm, dx, dy, xorg, yorg, slw, w;
    int x, y, e, i, n;
    int ya, xl, xr, xc;
    miFillArcRec info;
    miArcSliceRec slice;
    DDXPointRec point;
    DDXPointPtr pts;
    int width;
    int *wids;
    int halfWidth;
    int halfHeight;
    int xmin, xmax, ymin, ymax;
    RegionPtr prgnClip;
    BoxPtr clipbox;
    Bool clip;

    miFillArcSetup(arc, &info);
    miFillArcSliceSetup(arc, &slice, pGC);
    MIFILLARCSETUP();

    slw = arc->height;
    if (slice.flip_top || slice.flip_bot)
	slw += (arc->height >> 1) + 1;

    if (pGC->miTranslate)
    {
	xorg += pDraw->x;
	yorg += pDraw->y;
	slice.edge1.x += pDraw->x;
	slice.edge2.x += pDraw->x;
    }

    /*
     * Avoid clipping on every scan line by seeing if the 
     * bounding box requires clipping.
     */
    clip = TRUE;
    prgnClip = priv->pCompositeClip;
    n = REGION_NUM_RECTS(prgnClip);
    clipbox = REGION_RECTS(prgnClip);
    halfWidth =  (arc->width + 1) >> 1;      
    halfHeight = (arc->height + 1) >> 1;
    xmin = xorg - halfWidth;
    xmax = xorg + halfWidth;
    ymin = yorg - halfHeight;
    ymax = yorg + halfHeight;
    while( clip && n-- ) {
       clip = xmin < clipbox->x1
              || xmax > clipbox->x2
              || ymin < clipbox->y1
              || ymax > clipbox->y2;
       clipbox++;
    }

    while (y > 0)
    {
	MIFILLARCSTEP(slw);
	MIARCSLICESTEP(slice.edge1);
	MIARCSLICESTEP(slice.edge2);
	if (miFillSliceUpper(slice))
	{
	    ya = yorg - y;
	    MIARCSLICEUPPER(xl, xr, slice, slw);
	    ADDSLICESPANS(slice.flip_top);
	}
	if (miFillSliceLower(slice))
	{
	    ya = yorg + y + dy;
	    MIARCSLICELOWER(xl, xr, slice, slw);
	    ADDSLICESPANS(slice.flip_bot);
	}
    }
}

static void
agxFillArcSliceD(pDraw, pGC, arc, points, widths, priv)
    DrawablePtr pDraw;
    GCPtr pGC;
    xArc *arc;
    DDXPointPtr points;
    int *widths;
    cfbPrivGC *priv;
{
    int x, y, i;
    int dx, dy, xorg, yorg, slw, w;
    int ya, xl, xr, xc, n;
    double e, yk, xk, ym, xm;
    miFillArcDRec info;
    miArcSliceRec slice;
    DDXPointRec point;
    DDXPointPtr pts;
    int width;
    int *wids;
    int halfWidth;
    int halfHeight;
    int xmin, xmax, ymin, ymax;
    RegionPtr prgnClip;
    BoxPtr clipbox;
    Bool clip;

    miFillArcDSetup(arc, &info);
    miFillArcSliceSetup(arc, &slice, pGC);
    MIFILLARCSETUP();

    slw = arc->height;
    if (slice.flip_top || slice.flip_bot)
	slw += (arc->height >> 1) + 1;

    if (pGC->miTranslate)
    {
	xorg += pDraw->x;
	yorg += pDraw->y;
	slice.edge1.x += pDraw->x;
	slice.edge2.x += pDraw->x;
    }

    /*
     * Avoid clipping on every scan line by seeing if the 
     * bounding box requires clipping.
     */
    clip = TRUE;
    prgnClip = priv->pCompositeClip;
    n = REGION_NUM_RECTS(prgnClip);
    clipbox = REGION_RECTS(prgnClip);
    halfWidth =  (arc->width + 1) >> 1;      
    halfHeight = (arc->height + 1) >> 1;
    xmin = xorg - halfWidth;
    xmax = xorg + halfWidth;
    ymin = yorg - halfHeight;
    ymax = yorg + halfHeight;
    while( clip && n-- ) {
       clip = xmin < clipbox->x1
              || xmax > clipbox->x2
              || ymin < clipbox->y1
              || ymax > clipbox->y2;
       clipbox++;
    }

    while (y > 0)
    {
	MIFILLARCSTEP(slw);
	MIARCSLICESTEP(slice.edge1);
	MIARCSLICESTEP(slice.edge2);
	if (miFillSliceUpper(slice))
	{
	    ya = yorg - y;
	    MIARCSLICEUPPER(xl, xr, slice, slw);
	    ADDSLICESPANS(slice.flip_top);
	}
	if (miFillSliceLower(slice))
	{
	    ya = yorg + y + dy;
	    MIARCSLICELOWER(xl, xr, slice, slw);
	    ADDSLICESPANS(slice.flip_bot);
	}
    }
}

#define agxFillArcEmpty(arc) ( !(arc)->angle2 \
                               || !(arc)->width || !(arc)->height ) 


/* AGXPOLYFILLARC -- The public entry for the PolyFillArc request.
 * Since we don't have to worry about overlapping segments, we can just
 * fill each arc as it comes.
 */
void
agxPolyFillArc(pDraw, pGC, narcs, parcs)
    DrawablePtr	pDraw;
    GCPtr	pGC;
    int		narcs;
    xArc	*parcs;
{
    int i, n;
    cfbPrivGC *priv;
    xArc *arc;

    if( !xf86VTSema
        || pDraw->type != DRAWABLE_WINDOW
        || pGC->fillStyle != FillSolid 
        || !(pGC->planemask) ) {
    
          miPolyFillArc(pDraw, pGC, narcs, parcs);
    }
    else {
       DDXPointPtr points;
       int *widths;
       Bool clip;

       n = miFindMaxBand( ((cfbPrivGC *)
                              (pGC->devPrivates[cfbGCPrivateIndex].ptr))
                                 ->pCompositeClip);
       points = (DDXPointPtr)ALLOCATE_LOCAL(sizeof(DDXPointRec) * n);
       if (!points)
           return;
       widths = (int *)ALLOCATE_LOCAL(sizeof(int) * n);
       if (!widths)
       {
           DEALLOCATE_LOCAL(points);
           return;
       }

       priv = (cfbPrivGC *) pGC->devPrivates[cfbGCPrivateIndex].ptr;

       AGXFILLSETUP();

       for(i = narcs, arc = parcs; --i >= 0; arc++) {
	  if (agxFillArcEmpty(arc))
	     continue;;

          if ((arc->angle2 >= FULLCIRCLE) || (arc->angle2 <= -FULLCIRCLE)) {
             if (miCanFillArc(arc))
                agxFillEllipseI(pDraw, pGC, arc, points, widths, priv);
             else
                agxFillEllipseD(pDraw, pGC, arc, points, widths, priv);
          }
          else {
             if (miCanFillArc(arc))
                agxFillArcSliceI(pDraw, pGC, arc, points, widths, priv);
             else
                agxFillArcSliceD(pDraw, pGC, arc, points, widths, priv);
          }
       }
       DEALLOCATE_LOCAL(widths);
       DEALLOCATE_LOCAL(points);
       GE_WAIT_IDLE_EXIT();
    }
}

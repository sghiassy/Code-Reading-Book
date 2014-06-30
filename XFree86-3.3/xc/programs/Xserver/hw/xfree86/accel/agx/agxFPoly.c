/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxFPoly.c,v 3.3 1996/12/23 06:32:39 dawes Exp $ */
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
/* $XConsortium: agxFPoly.c /main/5 1996/02/21 17:17:18 kaleb $ */

#include "gcstruct.h"
#include "pixmap.h"
#include "miscanfill.h"
#include "mipoly.h"
#include "cfb.h"
#include "agx.h"
#include "regagx.h"

static int getPolyYBounds();

/*
 *     convexpoly.c
 *
 *     Written by Brian Kelleher; Dec. 1985.
 *
 *     Fill a convex polygon.  If the given polygon
 *     is not convex, then the result is undefined.
 *     The algorithm is to order the edges from smallest
 *     y to largest by partitioning the array into a left
 *     edge list and a right edge list.  The algorithm used
 *     to traverse each edge is an extension of Bresenham's
 *     line algorithm with y as the major axis.
 *     For a derivation of the algorithm, see the author of
 *     this code.
 */

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
       int j; \
       n = miClipSpans( ((cfbPrivGC *) \
                           (pGC->devPrivates[cfbGCPrivateIndex].ptr)) \
                               ->pCompositeClip, \
                        &point, &width, 1, \
                        points, widths, TRUE ); \
       pts = points; \
       wids = widths; \
       for(j=0;j<n;j++) { \
          DRAWSPAN(pts->x,pts->y,*wids); \
          pts++; \
          wids++; \
       } \
    } \
    else { \
       DRAWSPAN(point.x,point.y,width); \
    }

void
agxFillConvexPoly(pDraw, pGC, count, ptsIn)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		count;                /* number of points        */
    DDXPointPtr ptsIn;                /* the points              */
{
    int xl, xr;                 /* x vals of left and right edges */
    int dl, dr;                 /* decision variables             */
    int ml, m1l;                /* left edge slope and slope+1    */
    int mr, m1r;                /* right edge slope and slope+1   */
    int incr1l, incr2l;         /* left edge error increments     */
    int incr1r, incr2r;         /* right edge error increments    */
    int dy;                     /* delta y                        */
    int y;                      /* current scanline               */
    int left, right;            /* indices to first endpoints     */
    int i;                      /* loop counter                   */
    int nextleft, nextright;    /* indices to second endpoints    */
    DDXPointRec point;          /* current span start             */
    int width;                  /* current span width             */
    int imin;                   /* index of smallest vertex (in y) */
    int xmin;                   /* x-extents of polygon            */
    int xmax;
    int ymin;                   /* y-extents of polygon            */
    int ymax;
    int n;                      /* max number of clipped sub-spans */
    DDXPointPtr points;         /* points for clipped sub-spans    */
    DDXPointPtr pts;
    int *widths;                /* widths fot clipped sub-spans    */
    int *wids; 
    RegionPtr prgnClip;
    BoxPtr clipbox;
    cfbPrivGC *priv;
    Bool clip;                  /* do we need to clip              */
    Bool solid;                 /* solid fill                      */

    solid = pGC->fillStyle == FillSolid;
    if( solid ) {
       n = miFindMaxBand( ((cfbPrivGC *)
                             (pGC->devPrivates[cfbGCPrivateIndex].ptr))
                                ->pCompositeClip);
       points = (DDXPointPtr)ALLOCATE_LOCAL(sizeof(DDXPointRec) * n);
       if (!points)
          return;
       widths = (int *)ALLOCATE_LOCAL(sizeof(int) * n);
       if (!widths) {
          DEALLOCATE_LOCAL(points);
          return;
       }
       AGXFILLSETUP(); \
    }

    /*
     *  find leftx, bottomy, rightx, topy, and the index
     *  of bottomy. Also translate the points.
     */
    imin = getPolyYBounds(ptsIn, count, &xmin, &xmax, &ymin, &ymax);

    /*
     * Avoid clipping on every scan line by seeing if the polygon's
     * bounding box requires clipping.
     */
    clip = TRUE;
    priv = (cfbPrivGC *) pGC->devPrivates[cfbGCPrivateIndex].ptr;
    prgnClip = priv->pCompositeClip;
    n = REGION_NUM_RECTS(prgnClip);
    clipbox = REGION_RECTS(prgnClip);
    while( clip && n-- ) {
       clip = xmin < clipbox->x1
              || xmax > clipbox->x2
              || ymin < clipbox->y1
              || ymax > clipbox->y2;
       clipbox++;
    }

    dy = ymax - ymin + 1;
    if ((count < 3) || (dy < 0))
	return;

    nextleft = nextright = imin;
    y = ptsIn[nextleft].y;

    /*
     *  loop through all edges of the polygon
     */
    do {
        /*
         *  add a left edge if we need to
         */
        if (ptsIn[nextleft].y == y) {
            left = nextleft;

            /*
             *  find the next edge, considering the end
             *  conditions of the array.
             */
            nextleft++;
            if (nextleft >= count)
                nextleft = 0;

            /*
             *  now compute all of the random information
             *  needed to run the iterative algorithm.
             */
            BRESINITPGON(ptsIn[nextleft].y-ptsIn[left].y,
                         ptsIn[left].x,ptsIn[nextleft].x,
                         xl, dl, ml, m1l, incr1l, incr2l);
        }

        /*
         *  add a right edge if we need to
         */
        if (ptsIn[nextright].y == y) {
            right = nextright;

            /*
             *  find the next edge, considering the end
             *  conditions of the array.
             */
            nextright--;
            if (nextright < 0)
                nextright = count-1;

            /*
             *  now compute all of the random information
             *  needed to run the iterative algorithm.
             */
            BRESINITPGON(ptsIn[nextright].y-ptsIn[right].y,
                         ptsIn[right].x,ptsIn[nextright].x,
                         xr, dr, mr, m1r, incr1r, incr2r);
        }

        /*
         *  generate scans to fill while we still have
         *  a right edge as well as a left edge.
         */
        i = min(ptsIn[nextleft].y, ptsIn[nextright].y) - y;
	/* in case we're called with non-convex polygon */
	if(i < 0)
        {
	    return;
	}
        while (i-- > 0) 
        {
            point.y = y;

            /*
             *  reverse the edges if necessary
             */
            if (xl < xr) 
            {
                width = xr - xl;
                point.x = xl;
            }
            else 
            {
                width = xl - xr;
                point.x = xr;
            }
            y++;

            if( width > 0 ) {
               if( solid ) {
                  DRAWSPANS();
               }
               else {
                  (*pGC->ops->FillSpans)( pDraw, pGC, 1, &point, &width, TRUE );
               }
            }

            /* increment down the edges */
            BRESINCRPGON(dl, xl, ml, m1l, incr1l, incr2l);
            BRESINCRPGON(dr, xr, mr, m1r, incr1r, incr2r);
        }
    }  while (y != ymax);

    if( solid ) {
       DEALLOCATE_LOCAL(widths);
       DEALLOCATE_LOCAL(points);
       GE_WAIT_IDLE_EXIT();
    }
}

/*
 *     Find the index of the point with the smallest y.
 */
static
int
getPolyYBounds(pts, n, lx, rx, by, ty)
    DDXPointPtr pts;
    int n;
    int *rx, *lx;
    int *by, *ty;
{
    DDXPointPtr ptMin;
    int ymin, ymax;
    int xmin, xmax;
    DDXPointPtr ptsStart = pts;

    ptMin = pts;
    xmin = xmax = pts->x;
    ymin = ymax = (pts++)->y;

    while (--n > 0) {
        if (pts->x < xmin) 
            xmin = pts->x;
        if (pts->x > xmax) 
            xmax = pts->x;
        if (pts->y < ymin)
	{
            ptMin = pts;
            ymin = pts->y;
        }
	if(pts->y > ymax)
            ymax = pts->y;
        pts++;
    }

    *lx = xmin;
    *rx = xmax;
    *by = ymin;
    *ty = ymax;
    return(ptMin-ptsStart);
}


/*
 *
 *     Written by Brian Kelleher;  Oct. 1985
 *
 *     Routine to fill a polygon.  Two fill rules are
 *     supported: frWINDING and frEVENODD.
 *
 *     See fillpoly.h for a complete description of the algorithm.
 */

void
agxFillGeneralPoly( pDraw, pGC, count, ptsIn)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		count;              /* number of points        */
    DDXPointPtr ptsIn;              /* the points              */
{
    EdgeTableEntry *pAET;  /* the Active Edge Table   */
    int y;                 /* the current scanline    */
    int nPts = 0;          /* number of pts in buffer */
    EdgeTableEntry *pWETE; /* Winding Edge Table      */
    ScanLineList *pSLL;    /* Current ScanLineList    */
    EdgeTableEntry *pPrevAET;       /* previous AET entry      */
    EdgeTable ET;                   /* Edge Table header node  */
    EdgeTableEntry AET;             /* Active ET header node   */
    EdgeTableEntry *pETEs;          /* Edge Table Entries buff */
    ScanLineListBlock SLLBlock;     /* header for ScanLineList */
    int fixWAET = 0;
    DDXPointRec point;     /* current span start             */
    int width;             /* current span width             */
    DDXPointPtr points;         /* points for clipped sub-spans    */
    DDXPointPtr pts;
    int *widths;                /* widths fot clipped sub-spans    */
    int *wids; 
    int i, n;
    int xmin;                   /* x-extents of polygon            */
    int xmax;
    int ymin;                   /* y-extents of polygon            */
    int ymax;
    RegionPtr prgnClip;
    BoxPtr clipbox;
    cfbPrivGC *priv;
    Bool solid;                 /* solid fill                      */
    Bool clip;                  /* do we need to clip              */

    solid = pGC->fillStyle == FillSolid;
    if( solid ) {
       n = miFindMaxBand( ((cfbPrivGC *)
                             (pGC->devPrivates[cfbGCPrivateIndex].ptr))
                                ->pCompositeClip);
       points = (DDXPointPtr)ALLOCATE_LOCAL(sizeof(DDXPointRec) * n);
       if (!points)
          return;
       widths = (int *)ALLOCATE_LOCAL(sizeof(int) * n);
       if (!widths) {
          DEALLOCATE_LOCAL(points);
          return;
       }
       AGXFILLSETUP(); \
    }

    if(!(pETEs = (EdgeTableEntry *)
        ALLOCATE_LOCAL(sizeof(EdgeTableEntry) * count)))
	return;
    if (!miCreateETandAET(count, ptsIn, &ET, &AET, pETEs, &SLLBlock))
    {
	DEALLOCATE_LOCAL(pETEs);
	return;
    }
    pSLL = ET.scanlines.next;

    /*
     * Avoid clipping on every scan line by seeing if the polygon's
     * bounding box requires clipping.
     */    
    getPolyYBounds(ptsIn, count, &xmin, &xmax, &ymin, &ymax);
    clip = TRUE;
    priv = (cfbPrivGC *) pGC->devPrivates[cfbGCPrivateIndex].ptr;
    prgnClip = priv->pCompositeClip;
    n = REGION_NUM_RECTS(prgnClip);
    clipbox = REGION_RECTS(prgnClip);
    while( clip && n-- ) {
       clip = xmin < clipbox->x1
              || xmax > clipbox->x2
              || ymin < clipbox->y1
              || ymax > clipbox->y2;
       clipbox++;
    }

    if (pGC->fillRule == EvenOddRule) 
    {
        /*
         *  for each scanline
         */
        for (y = ET.ymin; y < ET.ymax; y++) 
        {
            /*
             *  Add a new edge to the active edge table when we
             *  get to the next edge.
             */
            if (pSLL && y == pSLL->scanline) 
            {
                miloadAET(&AET, pSLL->edgelist);
                pSLL = pSLL->next;
            }
            pPrevAET = &AET;
            pAET = AET.next;

            /*
             *  for each active edge
             */
            while (pAET) 
            {
                point.x = pAET->bres.minor;
		point.y = y;
                width = pAET->next->bres.minor - pAET->bres.minor;

                if( width > 0 ) {
                   if( solid ) {
                      DRAWSPANS();
                   }
                   else {
                      (*pGC->ops->FillSpans)( pDraw, pGC, 1, 
                                              &point, &width, TRUE );
                   }
                }

                EVALUATEEDGEEVENODD(pAET, pPrevAET, y)
                EVALUATEEDGEEVENODD(pAET, pPrevAET, y);
            }
            miInsertionSort(&AET);
        }
    }
    else      /* default to WindingNumber */
    {
        /*
         *  for each scanline
         */
        for (y = ET.ymin; y < ET.ymax; y++) 
        {
            /*
             *  Add a new edge to the active edge table when we
             *  get to the next edge.
             */
            if (pSLL && y == pSLL->scanline) 
            {
                miloadAET(&AET, pSLL->edgelist);
                micomputeWAET(&AET);
                pSLL = pSLL->next;
            }
            pPrevAET = &AET;
            pAET = AET.next;
            pWETE = pAET;

            /*
             *  for each active edge
             */
            while (pAET) 
            {
                /*
                 *  if the next edge in the active edge table is
                 *  also the next edge in the winding active edge
                 *  table.
                 */
                if (pWETE == pAET) 
                {
                    point.x = pAET->bres.minor;
		    point.y = y;
                    width = pAET->nextWETE->bres.minor - pAET->bres.minor;

                    if( width > 0 ) {
                       if( solid ) {
                          DRAWSPANS();
                       }
                       else {
                          (*pGC->ops->FillSpans)
                            ( pDraw, pGC, 1, &point, &width, TRUE );
                       }
                    }

                    pWETE = pWETE->nextWETE;
                    while (pWETE != pAET)
                        EVALUATEEDGEWINDING(pAET, pPrevAET, y, fixWAET);
                    pWETE = pWETE->nextWETE;
                }
                EVALUATEEDGEWINDING(pAET, pPrevAET, y, fixWAET);
            }

            /*
             *  reevaluate the Winding active edge table if we
             *  just had to resort it or if we just exited an edge.
             */
            if (miInsertionSort(&AET) || fixWAET) 
            {
                micomputeWAET(&AET);
                fixWAET = 0;
            }
        }
    }

    DEALLOCATE_LOCAL(pETEs);
    miFreeStorage(SLLBlock.next);
    if( solid ) {
       DEALLOCATE_LOCAL(widths);
       DEALLOCATE_LOCAL(points);
       GE_WAIT_IDLE_EXIT();
    }
}


/*
 *  mipoly.c
 *
 *  Written by Brian Kelleher; June 1986
 *
 *  Draw polygons.  This routine translates the point by the
 *  origin if pGC->miTranslate is non-zero, and calls
 *  to the appropriate routine to actually scan convert the
 *  polygon.
 */
#include "X.h"
#include "windowstr.h"
#include "gcstruct.h"
#include "pixmapstr.h"
#include "mi.h"
#include "miscstruct.h"
#include "agx.h"

void
agxFillPolygon(pDraw, pGC, shape, mode, count, pPts)
    DrawablePtr		pDraw;
    GCPtr       	pGC;
    int			shape, mode;
    int  	        count;
    DDXPointPtr		pPts;
{
    int		i;
    int	        xorg, yorg;
    DDXPointPtr ppt;

    if (count == 0)
	return;

    ppt = pPts;
    if (pGC->miTranslate)
    {
	xorg = pDraw->x;
	yorg = pDraw->y;

        if (mode == CoordModeOrigin) 
        {
	        for (i = 0; i<count; i++) 
                {    
	            ppt->x += xorg;
	            ppt++->y += yorg;
	        }
        }
        else 
        {
	    ppt->x += xorg;
	    ppt++->y += yorg;
	    for (i = 1; i<count; i++) 
            {
	        ppt->x += (ppt-1)->x;
	        ppt->y += (ppt-1)->y;
	        ppt++;
	    }
        }
    }
    else
    {
	if (mode == CoordModePrevious)
        {
	    ppt++;
	    for (i = 1; i<count; i++) 
            {
	        ppt->x += (ppt-1)->x;
	        ppt->y += (ppt-1)->y;
	        ppt++;
	    }
        }
    }
    if( !xf86VTSema
        || pDraw->type != DRAWABLE_WINDOW
        || pGC->fillStyle != FillSolid 
        || !(pGC->planemask) ) {
       if (shape == Convex)
	   miFillConvexPoly(pDraw, pGC, count, pPts);
       else
	   miFillGeneralPoly(pDraw, pGC, count, pPts);
    }
    else {
       if (shape == Convex)
	   agxFillConvexPoly(pDraw, pGC, count, pPts);
       else
	   agxFillGeneralPoly(pDraw, pGC, count, pPts);
    }
}

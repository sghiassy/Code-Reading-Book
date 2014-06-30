/*
 * $XConsortium: pgcPntWin.c,v 1.2 91/07/16 13:12:44 jap Exp $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

#include "X.h"

#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "cfb.h"
#include "ibmTrace.h"

#include "pgc.h"

extern pgcScreenRec pgcScreenInfo[] ; 

void pgcPaintWindowSolid();
void pgcPaintWindowTile();
void cfbPaintWindow();

void
pgcPaintWindow(pWin, pRegion, what)
    WindowPtr	pWin;
    RegionPtr	pRegion;
    int		what;
{

    register cfbPrivWin	*pPrivWin;
    pPrivWin = (cfbPrivWin *)(pWin->devPrivates[cfbWindowPrivateIndex].ptr);

    TRACE(("pgcPaintWindow( pWin= 0x%x, pRegion= 0x%x, what= %d )\n",
							pWin,pRegion,what));
    
    switch (what) {
    case PW_BACKGROUND:
	switch (pWin->backgroundState) {
	case None:
	    return;
	case ParentRelative:
	    do {
		pWin = pWin->parent;
	    } while (pWin->backgroundState == ParentRelative);
	    (*pWin->drawable.pScreen->PaintWindowBackground)(pWin, pRegion,
							     what);
	    return;
	case BackgroundPixmap:
#ifdef FUTURE
	    if (pPrivWin->fastBackground)
	    {
		pgcPaintWindowTile(pWin, pRegion, what);
		return;
	    }
#endif
	    break;
	case BackgroundPixel:
	    pgcPaintWindowSolid(pWin, pRegion, what);
	    return;
    	}
    	break;
    case PW_BORDER:
	if (pWin->borderIsPixel)
	{
	    pgcPaintWindowSolid(pWin, pRegion, what);
	    return;
	}
#ifdef FUTURE
	else if (pPrivWin->fastBorder)
	{
            pgcPaintWindowTile(pWin, pRegion, what);
	    return;
	}
#endif
	break;
    }
    cfbPaintWindow(pWin, pRegion, what);
}

void
pgcPaintWindowSolid(pWin, pRegion, what)
    register WindowPtr pWin;
    register RegionPtr pRegion;
    int what;		
{
    register int nbox;
    register BoxPtr pbox;
    register unsigned long int pixel;
    register unsigned long int pm ;
    void (*fnp)() ;
    pgcScreenPtr	pPGCScreen ;
    int index ;

    TRACE(("pgcPaintWindowSolid(pWin= 0x%x, pRegion= 0x%x, what= %d)\n", pWin, pRegion, what));

    if ( !( nbox = REGION_NUM_RECTS(pRegion)))
	return ;
    pbox = REGION_RECTS(pRegion);

    if (what == PW_BACKGROUND)
	pixel = pWin->background.pixel;
    else
	pixel = pWin->border.pixel; 

    pm = ( 1 << pWin->drawable.depth ) - 1 ;
    index = pWin->drawable.pScreen->myNum ;
    pPGCScreen = &pgcScreenInfo[index] ;
    fnp = pPGCScreen->solidFill ;

    for ( ; nbox-- ; pbox++ ) {
	/*
	 * call fill routine, the parms are:
	 * 	fill(color, alu, planes, x, y, width, height);
	 */
	(* fnp)( pixel, GXcopy, pm, pbox->x1, pbox->y1, 
	pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,index) ; 
    }
    return ;
}


void
pgcPaintWindowTile(pWin, pRegion, what)
    register WindowPtr pWin;
    register RegionPtr pRegion;
    int what;		
{
    register int nbox;
    register BoxPtr pbox;
    register PixmapPtr pTile;
    register void (*fnp)();
    register unsigned long int pm ;
    pgcScreenPtr	pPGCScreen ;
    int index ;

    TRACE(("pgcPaintWindowTile(pWin= 0x%x, pRegion= 0x%x, what= %d)\n", pWin, pRegion, what));

    if ( !( nbox = REGION_NUM_RECTS(pRegion)))
	return ;
    pbox = REGION_RECTS(pRegion);

    if (what == PW_BACKGROUND)
	pTile = pWin->background.pixmap;
    else
	pTile = pWin->border.pixmap;

    pm = ( 1 << pWin->drawable.depth ) - 1 ;

    index = pWin->drawable.pScreen->myNum ;
    pPGCScreen = &pgcScreenInfo[index] ;
    fnp = pPGCScreen->tileFill ;

    for ( ; nbox-- ; pbox++ ) {
	/*
	 * call tile routine, the parms are:
	 * 	tile(tile, alu, planes, x, y, width, height,xSrc,ySrc);
	 */
	(* fnp) (pTile, GXcopy, pm, 
		pbox->x1, pbox->y1, 
		pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
		pWin->drawable.x, pWin->drawable.y );
    }
    return ;
}

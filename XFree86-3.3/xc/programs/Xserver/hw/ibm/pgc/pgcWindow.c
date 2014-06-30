/*
 * $XConsortium: pgcWindow.c,v 1.3 94/01/07 09:51:04 dpw Exp $
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
#include "scrnintstr.h"
#include "windowstr.h"
#include "mfb.h"
#include "mistruct.h"
#include "regionstr.h"

#include "ibmTrace.h"

#include "pgc.h"

extern pgcScreenRec pgcScreenInfo[] ;

/*
pgcCopyWindow copies only the parts of the destination that are
visible in the source.
*/

void 
pgcCopyWindow(pWin, ptOldOrg, prgnSrc)
    register WindowPtr pWin ;
    DDXPointRec ptOldOrg ;
    RegionPtr prgnSrc ;
{
    RegionPtr prgnDst ;
    register BoxPtr pbox ;
    register int dx, dy ;
    register int nbox ;
    register int pm ;
    BoxPtr pboxTmp, pboxNext, pboxBase, pboxNew ;
    void (* fnp)() ;
    pgcScreenPtr pPGCScreen ;
    int index ;

    TRACE(("pgcCopyWindow(pWin= 0x%x, ptOldOrg= 0x%x, prgnSrc= 0x%x)\n", pWin, ptOldOrg, prgnSrc)) ;

    dx = ptOldOrg.x - pWin->drawable.x ;
    dy = ptOldOrg.y - pWin->drawable.y ;
    REGION_TRANSLATE(pWin->drawable.pScreen, prgnSrc, -dx, -dy) ;

    prgnDst = REGION_CREATE(pWin->drawable.pScreen, NULL, 1);
    REGION_INTERSECT(pWin->drawable.pScreen, prgnDst, &pWin->borderClip, prgnSrc) ;

    if ( !( nbox = REGION_NUM_RECTS(prgnDst) ) )
	return;

    pbox = REGION_RECTS(prgnDst);

    pboxNew = 0 ;
    if ( nbox > 1 ) {
	if ( dy < 0 ) {
	    if ( dx > 0 ) {
		/* walk source bottom to top */
		/* keep ordering in each band, reverse order of bands */
		if ( !( pboxNew =
			(BoxPtr) ALLOCATE_LOCAL( sizeof( BoxRec ) * nbox ) ) )
			return ;
		pboxBase = pboxNext = pbox+nbox - 1 ;
		while ( pboxBase >= pbox ) {
			while ( ( pboxNext >= pbox )
			     && ( pboxBase->y1 == pboxNext->y1 ) )
				pboxNext-- ;
			pboxTmp = pboxNext + 1 ;
			while ( pboxTmp <= pboxBase )
				*pboxNew++ = *pboxTmp++ ;
			pboxBase = pboxNext ;
		}
		pboxNew -= nbox ;
		pbox = pboxNew ;
	    }
	    else { /* dx <= 0 */
		/* we can just reverse the entire list in place */
		/* Do three-position swaps */
		BoxRec tmpBox ;

		pboxBase = pbox ;
		pboxNext = pbox + nbox - 1 ;
		while ( pboxBase < pboxNext ) {
			/* ****** Warning Structure Assignment !! ****** */
			tmpBox    = *pboxBase ;
			*pboxBase = *pboxNext ;
			*pboxNext = tmpBox ;
			pboxBase++ ;
			pboxNext-- ;
		}
	    }
	}
        else if ( dx < 0 ) {
	/* walk source right to left */
	    /* reverse order of rects in each band */
	    if ( !( pboxNew = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox) ) )
		return ;
	    pboxBase = pboxNext = pbox ;
	    while (pboxBase < pbox+nbox)
	    {
		while ((pboxNext < pbox+nbox) &&
		       (pboxNext->y1 == pboxBase->y1))
		    pboxNext++ ;
		pboxTmp = pboxNext ;
		while (pboxTmp != pboxBase)
		    *pboxNew++ = *--pboxTmp ;
		pboxBase = pboxNext ;
	    }
	    pboxNew -= nbox ;
	    pbox = pboxNew ;
	}
    } /* END if nbox > 1 */

    /*
     * call blit several times, the parms are:
     *   blit( alu,rplanes, wplanes, srcx, srcy, destx, desty, width, height ) ;
     */

    index = pWin->drawable.pScreen->myNum ;
    pPGCScreen = &pgcScreenInfo[index] ;
    fnp = pPGCScreen->blit ;

    pm = ( 1 << pWin->drawable.depth ) - 1 ;
    for ( ; nbox-- ; pbox++ )
	(* fnp)( GXcopy, pWin->drawable.pScreen,pm, 
		 pbox->x1 + dx, pbox->y1 + dy, 
		 pbox->x1, pbox->y1, 
		 pbox->x2 - pbox->x1, pbox->y2 - pbox->y1) ;

    /* free up stuff */
    if ( pboxNew )
	DEALLOCATE_LOCAL( pboxNew ) ;

    REGION_DESTROY(pWin->drawable.pScreen, prgnDst) ;
    return ;
}

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/emulStip.c,v 3.4 1996/12/23 06:52:38 dawes Exp $ */
/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/* $XConsortium: emulStip.c /main/4 1996/02/21 17:56:19 kaleb $ */

/* ppc Stipple
 *
 * Uses private monoFill a bunch of times
 */

#include "X.h"
#include "pixmapstr.h"
#include "pixmap.h"
#include "scrnintstr.h"

#include "ppc.h"
#include "ibmTrace.h"

extern PixmapPtr ppcCopyPixmap() ;

void
ppcStipple( pWin, pStipple, fg, merge, planes, x, y, w, h, xSrc, ySrc )
WindowPtr pWin; /* GJA */
PixmapPtr pStipple ;
unsigned long int fg ;
int merge ;
unsigned long int planes ;
int x, y, w, h ;
int xSrc, ySrc ;
{
register char 	*data ;
register int 	htarget, vtarget, tlx, tly ;
PixmapPtr 	pUseStipple, pTmpStipple ;
int 		savehcount, savevcount, hcount, vcount,
		xfits, xrot, yrot, Rtlx, Btly ;

    TRACE( ( "ppcStipple(pStipple = x%x, merge= x%x, planes=x%02x, x=%d, y=%d, w=%d, h=%d, xSrc=%d, ySrc=%d\n",
		pStipple, merge, planes, x, y, w, h, xSrc, ySrc ) ) ;

    pUseStipple = pStipple ;

    if ( xrot = ( ( x - xSrc ) % ( tlx = pStipple->drawable.width ) ) ) {
	if ( !( pTmpStipple = ppcCopyPixmap( pStipple ) ) ) {
		ErrorF("Fail ppcCopyPixmap \"pTmpStipple\" in ppcStipple\n" ) ;
		return ;
	}
	ppcRotBitmapRight( pTmpStipple, tlx - xrot ) ;
	pUseStipple = pTmpStipple ;
    }

    if ( yrot = ( ( y - ySrc ) % ( tly = pStipple->drawable.height ) ) ) {
	if ( !( pTmpStipple = ppcCopyPixmap( pUseStipple ) ) ) {
		ErrorF( "Fail ppcCopyPixmap 2 \"pTmpStipple\" in ppcStipple\n");
		return ;
	}
	ppcRotBitmapDown( pTmpStipple, tly - yrot ) ;
	if (xrot)
		mfbDestroyPixmap( pUseStipple ) ;

	pUseStipple = pTmpStipple ;
    }

/* By here, pUseStipple points to the (possibly rotated) tile. If it is rotated,
   then it is the only newly created PIxmap, which we must destroy at the end.*/

    savehcount = w / tlx ;
    savevcount = h / tly ;

/* FIRST DO COMPLETELY VISIBLE PORTIONS */
    data = (char *) pUseStipple->devPrivate.ptr ;
    for ( vcount = savevcount, vtarget = y ;
      vcount-- ;
      vtarget += tly )
	for ( htarget = x, hcount = savehcount ;
	      hcount-- ;
	      htarget += tlx )
		vgaDrawMonoImage( pWin, data, htarget, vtarget, tlx, tly, fg, merge, planes ) ;

/* NOW DO RIGHT HAND SIDE */
    if ( Rtlx = w % tlx ) {
	/* htarget is already set to ( x + ( savehcount * tlx ) ) from above */
	xfits = ( ( ( ( Rtlx + 31 ) & ~31 ) == ( ( tlx + 31 ) & ~31 ) )
		? -1 : 0 ) ;
	if (xfits)
		for ( vcount = savevcount, vtarget = y ;
	      		vcount-- ;
	      		vtarget += tly )
				vgaDrawMonoImage( pWin, data, htarget, vtarget, 
					Rtlx, tly, fg, merge, planes ) ;
	   else
		for ( vcount = savevcount, vtarget = y ;
	      		vcount-- ;
	      		vtarget += tly ) {
			int th = tly ;
			int tvtarget = vtarget ;
			unsigned char *tdata = data ;
			while ( th-- ) {
				vgaDrawMonoImage( pWin, tdata, htarget, tvtarget++, 
					Rtlx, 1, fg, merge, planes ) ;
				tdata += pStipple->devKind ;
			}
		}
    }

/* NOW DO BOTTOM */
    if ( Btly = h % tly )
	/* vtarget is already set to ( x + ( savevcount * tly ) ) from above */
	for ( hcount = savehcount, htarget = x ;
      		hcount-- ;
      		htarget += tlx )
		vgaDrawMonoImage( pWin, data, htarget, vtarget, tlx, Btly, fg, merge, planes ) ;

/* NOW DO BOTTOM RIGHT CORNER */
    if ( Btly && Rtlx )
	/* htarget is already set to ( x + ( savehcount * tlx ) ) from above */
	/* vtarget is already set to ( x + ( savevcount * tly ) ) from above */
	if (xfits)
		vgaDrawMonoImage( pWin, data, htarget, vtarget, Rtlx, Btly, fg, merge, planes) ;
	else
		while ( Btly-- ) {
			vgaDrawMonoImage( pWin, data, htarget, vtarget++, 
				Rtlx, 1, fg, merge, planes ) ;
			data += pStipple->devKind ;
		}

    if ( xrot || yrot )
	mfbDestroyPixmap( pUseStipple ) ;

    return ;
}

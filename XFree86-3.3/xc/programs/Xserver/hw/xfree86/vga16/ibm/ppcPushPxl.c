/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/ppcPushPxl.c,v 3.4 1996/12/23 06:53:12 dawes Exp $ */
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
/* $XConsortium: ppcPushPxl.c /main/4 1996/02/21 17:58:15 kaleb $ */

/* ppc PushPixels */

#include "X.h"
#include "misc.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "miscstruct.h"

#include "ppc.h"
#include "ppcBitMacs.h"

#include "OScompiler.h"
#include "ibmTrace.h"

extern int mfbGCPrivateIndex;

static void
BitMapMerge( pDst, pSrc, pStp, xOffset, yOffset, srcInvert )
PixmapPtr	pDst;
PixmapPtr	pSrc ;
PixmapPtr	pStp ;
int		xOffset, yOffset ; /* Offset To Stipple Tmp bitmap */
int		srcInvert ; /* used for opaqueness */
{
register unsigned long int	bits ;
register unsigned long int	*bitptr ;
register int			wordOffset ;
register unsigned long int	*pDstPriv =
			(unsigned long int *) pDst->devPrivate.ptr ;
unsigned long int	*pSrcPriv =
			(unsigned long int *) pSrc->devPrivate.ptr ;
register int			j ;
register int			i ;
register unsigned long int	*pStipSrc ;
unsigned long int	*pStpPriv = (unsigned long int *) pStp->devPrivate.ptr ;
unsigned long int	stipOffset ;
unsigned long int	stpWidth	= pStp->drawable.width ;
unsigned long int	stpPaddedWidth	= pStp->devKind >> 2 ;
unsigned long int	stpHeight	= pStp->drawable.height ;
unsigned long int	dstHeight	= pDst->drawable.height ;
unsigned long int	dstWidth	= pDst->devKind >> 2 ;

if ( ( xOffset %= stpWidth ) < 0 )
	xOffset += stpWidth ;
if ( ( yOffset %= stpHeight ) < 0 )
	yOffset += stpHeight ;

if ( stpWidth > 32 ) {
	/* **************************** */
	/* Only Works if stpWidth >= 32 */
	/* **************************** */
	if ( srcInvert )
		for ( i = 0 ; i < dstHeight ; i++ ) { /* For Each Line */
			pStipSrc = pStpPriv
				+ ( ( ( yOffset + i ) % stpHeight ) * stpPaddedWidth ) ;
			for ( j = dstWidth, stipOffset = xOffset ; j-- ; ) {
				wordOffset = stipOffset & 0x1F ;
				if ( stipOffset <= stpWidth - 32 ) {
					bitptr = pStipSrc + ( stipOffset >> 5 ) ;
				  bits = SCRLEFT( bitptr[0], wordOffset )
					 | SCRRIGHT( bitptr[1],
					      ( 32 - wordOffset )) ;
				}
				else { /* stipOffset > stpWidth - 32 */
				    bits = SCRLEFT(
					   pStipSrc[ stipOffset >> 5 ],
					    wordOffset )
					 | SCRRIGHT( pStipSrc[0],
						( 32 - wordOffset )) ;
					stipOffset -= stpWidth ;
				}
				*pDstPriv++ = *pSrcPriv++ & ~ bits ;
			}
		}
	else
		for ( i = 0 ; i < dstHeight ; i++ ) { /* For Each Line */
			pStipSrc = pStpPriv
				+ ( ( ( yOffset + i ) % stpHeight ) * stpPaddedWidth ) ;
			for ( j = dstWidth, stipOffset = xOffset ; j-- ; ) {
				wordOffset = stipOffset & 0x1F ;
				if ( stipOffset <= stpWidth - 32 ) {
					bitptr = pStipSrc + ( stipOffset >> 5 ) ;
					bits = SCRLEFT( bitptr[0], wordOffset )
					     | SCRRIGHT( bitptr[1],
							 ( 32 - wordOffset ) ) ;
				}
				else { /* stipOffset > stpWidth - 32 */
					bits = SCRLEFT(
						pStipSrc[ stipOffset >> 5 ],
						wordOffset )
					     | SCRRIGHT( pStipSrc[0],
							 ( 32 - wordOffset ) ) ;
					stipOffset -= stpWidth ;
				}
				*pDstPriv++ = *pSrcPriv++ & bits ;
			}
		}
}
else { /* stpWidth <= 32 */
	for ( i = stpWidth ; i < 32 ; i <<= 1 ) ; /* Test For A Power Of 2 */
	if ( i == 32 ) { /* Best Case - Started w/ Power Of 2 */
	   if (srcInvert) {
		for ( i = 0 ; i < dstHeight ; i++ ) { /* For Each Line */
				bits = pStpPriv[ ( yOffset + i ) % stpHeight ] ;
			/* Pad Out The Bit Mask To A Full Word */
			for ( j = stpWidth ; j != 32 ; j <<= 1 )
				bits |= SCRRIGHT( bits, j ) ;
			/* If Needed Rotate The Mask */
			if ( xOffset )
				bits = SCRLEFT( bits, xOffset )
				     | SCRRIGHT( bits, 32 - xOffset ) ;
			/* For Each Word In The Line */
			for ( j = dstWidth ; j-- ; )
				*pDstPriv++ = *pSrcPriv++ & ~bits ;
		}
	   } else {
		for ( i = 0 ; i < dstHeight ; i++ ) { /* For Each Line */
				bits = pStpPriv[ ( yOffset + i ) % stpHeight ] ;
			/* Pad Out The Bit Mask To A Full Word */
			for ( j = stpWidth ; j != 32 ; j <<= 1 )
				bits |= SCRRIGHT( bits, j ) ;
			/* If Needed Rotate The Mask */
			if ( xOffset )
				bits = SCRLEFT( bits, xOffset )
				     | SCRRIGHT( bits, 32 - xOffset ) ;
			/* For Each Word In The Line */
			for ( j = dstWidth ; j-- ; )
				*pDstPriv++ = *pSrcPriv++ & bits ;
		}
	   }
	} else { /* Case - Didn't Start w/ Power Of 2 */
		int goalWidth = i >> 1 ;
	   if (srcInvert) {

		for ( i = 0 ; i < dstHeight ; i++ ) { /* For Each Line */
				bits = pStpPriv[ ( yOffset + i ) % stpHeight ] ;
			/* Pad Out The Bit Mask To A Full Word */
			for ( j = stpWidth ; j <= goalWidth ; j <<= 1 )
				bits |= SCRRIGHT( bits, j ) ;
			/* If Needed Rotate The Mask */
			if ( xOffset )
				bits = ~(SCRLEFT( bits, xOffset )
				     | SCRRIGHT( bits, goalWidth - xOffset)) ;
			/* For Each Word In The Line */
			for ( j = dstWidth ; j-- ; ) {
				*pDstPriv++ = *pSrcPriv++ & bits ;
				bits = ~(SCRLEFT( bits, 32 - goalWidth )
				     | SCRRIGHT( bits,
						( goalWidth << 1 ) - 32 )) ;
			}
		}
	   } else {
		for ( i = 0 ; i < dstHeight ; i++ ) { /* For Each Line */
				bits = pStpPriv[ ( yOffset + i ) % stpHeight ] ;
			/* Pad Out The Bit Mask To A Full Word */
			for ( j = stpWidth ; j <= goalWidth ; j <<= 1 )
				bits |= SCRRIGHT( bits, j ) ;
			/* If Needed Rotate The Mask */
			if ( xOffset )
				bits = SCRLEFT( bits, xOffset )
				     | SCRRIGHT( bits, goalWidth - xOffset ) ;
			/* For Each Word In The Line */
			for ( j = dstWidth ; j-- ; ) {
				*pDstPriv++ = *pSrcPriv++ & bits ;
				bits = SCRLEFT( bits, 32 - goalWidth )
				     | SCRRIGHT( bits,
						( goalWidth << 1 ) - 32 ) ;
			}
		}
	   }
	}
}

return ;
}

/* Note: pushPixels operates with the current Fill Style
	 thus the ppc's colorRrop is initially valid.
*/

void
ppcPushPixels( pGC, pBitMap, pDrawable, dx, dy, xOrg, yOrg )
    GCPtr	pGC ;
    PixmapPtr	pBitMap ;
    DrawablePtr pDrawable ;
    int		dx, dy, xOrg, yOrg ;
{
ppcPrivGC		*gcPriv  = (ppcPrivGC *) ( pGC->devPrivates[mfbGCPrivateIndex].ptr ) ;
ScreenPtr		pScrn ;
ppcScrnPriv		*scrPriv ;
RegionPtr		prgnDst ;
int			alu ;
int			fillStyle ;
int			xSrc, ySrc ;
BoxPtr			pbox ;
unsigned int		nbox ;

if ( ( pDrawable->type != DRAWABLE_WINDOW )
  || ( fillStyle = gcPriv->colorRrop.fillStyle ) == FillTiled ) {
	miPushPixels( pGC, pBitMap, pDrawable, dx, dy, xOrg, yOrg ) ;
	return ;
}
pScrn   = (ScreenPtr) pDrawable->pScreen ;
scrPriv = (ppcScrnPriv *) ( pScrn->devPrivate ) ;

/* Note: pushPixels operates with the current Fill Style
	 thus the ppc's colorRrop is initially valid. */
if ( ( alu = gcPriv->colorRrop.alu ) == GXnoop )
	return ;

/* Find The Actual Regions To Fill
 * by intersecting the destination's clip-region with
 * the box defined by our arguments.
 */
{
	BoxRec		dstBox ;

	dstBox.x2 = ( dstBox.x1 = xOrg ) + dx ;
	dstBox.y2 = ( dstBox.y1 = yOrg ) + dy ;

	prgnDst = (* pScrn->RegionCreate )( &dstBox,
					    REGION_NUM_RECTS(gcPriv->pCompositeClip));
	(* pScrn->Intersect)( prgnDst, prgnDst, gcPriv->pCompositeClip ) ;
	if ( !( nbox = REGION_NUM_RECTS(prgnDst))) {
	   (* pScrn->RegionDestroy)( prgnDst ) ;
	   return;
        } 
 	pbox = REGION_RECTS(prgnDst);
}

/* ASSUME ( pDrawable->type == DRAWABLE_WINDOW ) */
xSrc = pGC->patOrg.x + pDrawable->x ;
ySrc = pGC->patOrg.y + pDrawable->y ;

{ /* Begin Sub-Block */
	PixmapPtr		ptmpBitmap = (PixmapPtr) 0 ;
				/* Temp bitmap Make Sure It's Zero'ed Here */
	int			BitmapIsCopy = FALSE ;
	unsigned long int	fg = gcPriv->colorRrop.fgPixel ;
	unsigned long int	bg = gcPriv->colorRrop.bgPixel ;
	unsigned long int	pm = gcPriv->colorRrop.planemask ;

	/* Now Do The Needed Fill */
	switch ( fillStyle ) {

		case FillOpaqueStippled : { /* Create two (2) Tmp stipples */
			/* Create The Scratch Bitmap */
			ptmpBitmap = (* pScrn->CreatePixmap)( pScrn,
							pBitMap->drawable.width,
							pBitMap->drawable.height, 1 ) ;

			/* Now take the logical "AND-Not" of the pBitmap argument
			 * and the GC's stipple */
			BitMapMerge( ptmpBitmap, pBitMap, pGC->stipple,
				     ( xOrg - xSrc ), ( yOrg - ySrc ), TRUE ) ;
			for ( nbox = REGION_NUM_RECTS(prgnDst), pbox = REGION_RECTS(prgnDst);
			      nbox-- ;
			      pbox++ ) {
				vgaFillStipple( (WindowPtr)pDrawable,
					      ptmpBitmap, bg, alu, pm,
					      pbox->x1,
					      pbox->y1,
					      pbox->x2 - pbox->x1,
					      pbox->y2 - pbox->y1,
					      xOrg, yOrg ) ;
				}
			} /* Now Fall-Though To Do Foreground */
		case FillStippled : /* Create Tmp stipple from pBitmap AND stipple */
			if ( !ptmpBitmap ) {
				ptmpBitmap = (* pScrn->CreatePixmap)( pScrn,
								pBitMap->drawable.width,
								pBitMap->drawable.height, 1 ) ;
			}
			/* Now take the logical "AND" of the GC's stipple
			 * and the pBitmap argument */
			BitMapMerge( ptmpBitmap, pBitMap, pGC->stipple,
				     ( xOrg - xSrc ), ( yOrg - ySrc ), FALSE ) ;

			/* Now Fall-Though To REALLY Do Foreground */
		case FillSolid : /* Easiest case -- Just Stipple it in ! */
			if ( !ptmpBitmap ) {
				ptmpBitmap = pBitMap ;
				BitmapIsCopy = TRUE ;
			}
			for ( nbox = REGION_NUM_RECTS(prgnDst), pbox = REGION_RECTS(prgnDst);
			      nbox-- ;
			      pbox++ ) {
				vgaFillStipple( (WindowPtr)pDrawable,
					      ptmpBitmap, fg, alu, pm,
					      pbox->x1,
					      pbox->y1,
					      pbox->x2 - pbox->x1,
					      pbox->y2 - pbox->y1,
					      xOrg, yOrg ) ;
				}
			if ( BitmapIsCopy == FALSE )
				(* pScrn->DestroyPixmap)( ptmpBitmap ) ;
			break ;

		case FillTiled : /* Hardest case */
			/* Not Yet Implimented Here -- Calls "mi" above */
			break ;
		default :
			ErrorF( "ppcPushPixels: Unknown fill Style\n" ) ;
			break ;
	}
} /* End Sub-Block */
(* pScrn->RegionDestroy)( prgnDst ) ;

return ;
}

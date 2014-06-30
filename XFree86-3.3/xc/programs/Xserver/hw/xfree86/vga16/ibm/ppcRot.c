/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/ppcRot.c,v 3.1 1996/12/23 06:53:14 dawes Exp $ */
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
/* $XConsortium: ppcRot.c /main/3 1996/02/21 17:58:22 kaleb $ */

/* ROTATE  and CLIP Z8 PixMaps
 * Demands that pixmaps be Z oriented, 8 bits per pixel
 * Note that depth doesn't matter ; only bits per pixel.
 *
 */

/* Seems that for depth
 * 1 non-frame-buffer displays, this is one of the last ppc things to prevent
 * the ppc from fully supporting depth 1. 
 * Anyway, Code added for 1-bit-per-pixel maps since mfb doesn't quite
 * make it. 
 *
 */

#include "X.h"
#include "misc.h"
#include "pixmapstr.h"

#include "OScompiler.h"
#include "ibmTrace.h"

extern int ppcDepthOK() ;
extern PixmapPtr ppcCreatePixmap() ;

void
ppcRotZ8mapUp( pSource, pDest, shift )
register PixmapPtr pSource ;
register PixmapPtr pDest ;
register int shift ;
{
register char *s, *d ;
register int i, j, w, h ;

if ( !ppcDepthOK( pSource, pSource->drawable.depth ) ) {
	ErrorF( "ppcRotZ8mapUp: Source depth not valid\n" ) ;
	return ;
}
if ( !ppcDepthOK( pDest, pDest->drawable.depth ) ) {
	ErrorF( "ppcRotZ8mapUp: Destination depth not valid\n" ) ;
	return ;
}
if ( ( pDest->drawable.width  != pSource->drawable.width )
  || ( pDest->drawable.height != pSource->drawable.height ) ) {
	ErrorF( "ppcRotZ8mapUp: Pixmaps different size\n" ) ;
	return ;
}

w = pDest->devKind ;
h = pDest->drawable.height ;

shift %= h ;
if ( shift < 0 )
	shift += h ; /* compilers don't REALLY do modulo */

s = (char *) pSource->devPrivate.ptr ;
d = (char *) pDest->devPrivate.ptr ;

for ( i = shift, j = 0 ; i < h ; i++, j++ )
	MOVE( &(s[ i * w ]), &d[ j * w ], w ) ; /* move Ith line up */
for ( i = 0 ; i < shift ; i++, j++ )
	MOVE( &(s[ i * w ]), &d[ j * w ], w ) ; /* move top rows down */

return ;
}

void
ppcRotZ8mapLeft( pSource, pDest, shift )
register PixmapPtr pSource ;
register PixmapPtr pDest ;
register int shift ;
{
register char *s, *d ;
register int i, j, w, h ;

if ( !ppcDepthOK( pSource, pSource->drawable.depth ) ) {
	ErrorF( "ppcRotZ8mapLeft: Source depth not valid\n" ) ;
	return ;
}
if ( !ppcDepthOK( pDest, pDest->drawable.depth ) ) {
	ErrorF( "ppcRotZ8mapLeft: Destination depth not valid\n" ) ;
	return ;
}
if ( ( ( w = pDest->drawable.width ) != pSource->drawable.width )
  || ( ( h = pDest->drawable.height ) != pSource->drawable.height ) ) {
	ErrorF( "ppcRotZ8mapLeft: Pixmaps different size\n" ) ;
	return ;
}

if ( ( shift %= w ) < 0 )
	shift += w ; /* compilers don't REALLY do modulo */

s = (char *) pSource->devPrivate.ptr ;
d = (char *) pDest->devPrivate.ptr ;
while ( h-- ) {
	for ( i = shift, j = 0 ; i < w ; i++, j++ )
		d[j] = s[i] ;
	for ( i = 0 ; i < shift ; i++, j++ )
		d[j] = s[i] ;
	d += pDest->devKind ;	/* next line */
	s += pSource->devKind ;	/* next line */
}

return ;
}

void
ppcClipZ8Pixmap( pSource, pDest )
register PixmapPtr pSource, pDest ;
{
register unsigned char		*dest, *source ;
register int			dh, bytesperSline, bytesperDline, j ;
int				dw ;

if ( !ppcDepthOK( pSource, pSource->drawable.depth ) ) {
	ErrorF( "ppcClipZ8Pixmap: Source depth not valid\n" ) ;
	return ;
}
if ( !ppcDepthOK( pDest, pDest->drawable.depth ) ) {
	ErrorF( "ppcClipZ8Pixmap: Destination depth not valid\n" ) ;
	return ;
}
if ( ( dw = pDest->drawable.width ) > pSource->drawable.width ) {
	ErrorF( "ppcClipZ8Pixmap: Destination witdh > src width\n" ) ;
	return ;
}
if ( ( dh = pDest->drawable.height ) > pSource->drawable.height ) {
	ErrorF( "ppcClipZ8Pixmap: Destination height > src height\n" ) ;
	return ;
}

bytesperSline = pSource->devKind - dw ;
bytesperDline = pDest->devKind - dw ;

for ( dest = (unsigned char *) pDest->devPrivate.ptr,
      source = (unsigned char *) pSource->devPrivate.ptr ;
      dh-- ;
      source += bytesperSline, dest += bytesperDline )
	for ( j = dw ; j-- ; )
		*dest++ = *source++ ;

return ;
}

PixmapPtr
ppcClipBitmap( pStipple, w, h )
PixmapPtr pStipple ;
int w, h ;
{
	register unsigned char *src, *dst ;
	register int bytesperline ;
	PixmapPtr pNewStipple ;

	pNewStipple = ppcCreatePixmap( pStipple->drawable.pScreen,
					w, h, 1 ) ;
	if (pNewStipple) {
		w = MIN( w, pStipple->drawable.width ) ;
		bytesperline = ((w+7)/8) ;
		h = MIN( h, pStipple->drawable.height ) ;
		src = (unsigned char *) pStipple->devPrivate.ptr ;
		dst = (unsigned char *) pNewStipple->devPrivate.ptr ;
		while (h--) {
			MOVE(src,dst,bytesperline) ;
			src += pStipple->devKind ;
			dst += pNewStipple->devKind ;
		}
	}
	return pNewStipple ;
}

void
ppcRotBitmapUp( pSource, pDest, shift )
register PixmapPtr pSource ;
register PixmapPtr pDest ;
register int shift ;
{
register char *s, *d ;
register int i, j, w, h ;

if ( pSource->drawable.depth != 1 ) {
	ErrorF( "ppcRotBitmapUp: source depth of %d != 1! Abandon ship!\n",
		pSource->drawable.depth ) ;
	return ;
}
if ( pDest->drawable.depth != 1 ) {
	ErrorF( "ppcRotBitmapUp: dest depth of %d != 1! Abandon ship!\n",
		pDest->drawable.depth ) ;
	return ;
}
if ( ( pDest->drawable.width != pSource->drawable.width )
  || ( pDest->drawable.height != pSource->drawable.height ) ) {
	ErrorF( "ppcRotBitmapUp: Pixmaps different size\n" ) ;
	return ;
}

/* holy cow, this code is the same as 8 bits-per-pixel.  *sigh* */

w = pDest->devKind ;
h = pDest->drawable.height ;

shift %= h ;
if ( shift < 0 )
	shift += h ; /* compilers don't REALLY do modulo */

s = (char *) pSource->devPrivate.ptr ;
d = (char *) pDest->devPrivate.ptr ;

for ( i = shift, j = 0 ; i < h ; i++, j++ )
	MOVE( &(s[ i * w ]), &d[ j * w ], w ) ; /* move Ith line up */
for ( i = 0 ; i < shift ; i++, j++ )
	MOVE( &(s[ i * w ]), &d[ j * w ], w ) ; /* move top rows down */

return ;
}

void
ppcRotBitmapLeft( pSource, pDest, shift )
register PixmapPtr pSource ;
register PixmapPtr pDest ;
register int shift ;
{
register unsigned long *src, *dst ;
register unsigned long leftpart, p1mask, p2mask ;
register int perbyte ;
register int perlong ;
int w,h, sx, dx ;
int numBytesOnRight, scanline, bytestodo, wholebytes ;
int longstodo, wholelongs ;
int srcLongsPerScanline, dstLongsPerScanline ;
unsigned long  perlongR, lastperlongR, lastlongmask, p1lastmask, p2lastmask ;

if ( ( ( w = pDest->drawable.width ) != pSource->drawable.width )
  || ( ( h = pDest->drawable.height ) != pSource->drawable.height ) ) {
	ErrorF( "ppcRotBitmapLeft: Pixmaps different size\n" ) ;
	return ;
}

if ( ( shift %= w ) < 0 )
	shift += w ; /* compilers don't REALLY do modulo */

/* OK, we've got 1 bit per pixel, padded to 32.  CAN I DO SOMETHING
   INTELLIGENT?  Since it's 2AM, probably not. */

wholebytes = shift >> 3 ; /* multiples of 8 to shift */
perbyte = shift & 7 ;
bytestodo = (w+7) >> 3 ;

wholelongs = shift >> 5 ; /* multiples of 32 to shift */
perlong = shift & 31 ;
perlongR = 32 - perlong ;
lastperlongR = (31 & w) - perlong ;
longstodo = (w+31) >> 5 ;

/* LEAVE THIS SECTION ALONE EVEN IF YOU MASK & ROTATE BY LONGS */
if ( perbyte == 0 ) {
	register unsigned char *bsrc, *bdst ;
	bsrc =  pSource->devPrivate.ptr ;
	bdst =  pDest->devPrivate.ptr ;
	numBytesOnRight = bytestodo-wholebytes ;
	for ( scanline = 0 ; scanline < h ; scanline++ ) {
		MOVE( &(bsrc[wholebytes]), bdst, numBytesOnRight ) ;
		MOVE( bsrc, &(bdst[numBytesOnRight]), wholebytes ) ;
		bsrc += pSource->devKind ;
		bdst += pDest->devKind ;
	}
	return ;
}

src =  (unsigned long *) pSource->devPrivate.ptr ;
dst =  (unsigned long *) pDest->devPrivate.ptr ;

srcLongsPerScanline = pSource->devKind >> 2 ;
dstLongsPerScanline = pDest->devKind >> 2 ;

p1mask = (~0 << perlong) ;
p2mask = ~p1mask ;
lastlongmask = ( ~0 << (32- (w&31))) ;
p1lastmask = lastlongmask << perlong ;
p2lastmask = (~p1lastmask) & lastlongmask ;

for ( scanline = 0 ; scanline < h ; scanline++ ) {
	leftpart = (src[wholelongs] << perlong) & p1mask ;
	for ( sx = wholelongs, dx=0 ; sx<longstodo-1 ; sx++, dx++ ) {
		dst[dx] = leftpart | ((src[sx+1] >> perlongR) & p2mask) ;
		leftpart = (src[sx+1] << perlong) & p1mask ;
	}

	/* do wraparound long */
	if ( dx >= srcLongsPerScanline - 1 )
		dst[dx] = leftpart | ((src[0] >> lastperlongR ) & p2lastmask) ;
	else
		dst[dx] = leftpart | ((src[0] >> perlongR) & p2mask) ;
	leftpart = (src[0] << perlong) & p1mask ;
	dx++ ;

	for (sx=0 ; sx<wholelongs ; sx++, dx++) {
		if (dx >= srcLongsPerScanline-1)
			dst[dx] = leftpart |
				((src[0] >> lastperlongR ) & p2lastmask) ;
	   	else
	   		dst[dx] = leftpart | ((src[0] >> perlongR) & p2mask) ;
		leftpart = (src[sx+1] << perlong) & p1mask ;
	}

	src += srcLongsPerScanline ;
	dst += dstLongsPerScanline ;
}

return ;
}

void
ppcClipZ1Pixmap( pSource, pDest )
register PixmapPtr pSource, pDest ;
{
register unsigned char		*dest, *source ;
register int			dh, bytesperSline, bytesperDline, j ;
int				dw ;

if ( ( dw = pDest->drawable.width ) > pSource->drawable.width ) {
	ErrorF("ppcClipZ1Pixmap: Destination witdh > src width\n") ;
	return ;
}
if ( ( dh = pDest->drawable.height ) > pSource->drawable.height ) {
	ErrorF("ppcClipZ1Pixmap: Destination height > src height\n") ;
	return ;
}

bytesperSline = pSource->devKind - dw ;
bytesperDline = pDest->devKind - dw ;

for ( dest   = (unsigned char *) pDest->devPrivate.ptr,
      source = (unsigned char *) pSource->devPrivate.ptr ;
      dh-- ;
      source += bytesperSline,
      dest += bytesperDline )
	for ( j = dw ; j-- ; )
		*dest++ = *source++ ;

return ;
}

#if defined(DEBUG)
void
ppcPrintZ8Pixmap( pMap )
register PixmapPtr pMap ;
{
register unsigned char          *map ;
register int                   j, h ;

ErrorF("width   %d\n", pMap->width) ;
ErrorF("devKind %d\n", pMap->devKind) ;
ErrorF("height  %d\n", pMap->height) ;

map   = (unsigned char *) pMap->devPrivate.ptr ;

for ( h = 0 ; h < pMap->height ; h++) {
    for ( j = 0 ; j < pMap->devKind ; j++)
	ErrorF("%2x ",map[j]) ;
    ErrorF("\n") ;
    map += pMap->devKind ;
}

return ;
}
#endif

/* Following functions are added since it seems that the ppc has to solve
just about everybody's problems these days */

void
ppcEndWorldHunger()
{
ErrorF("ppcEndWorldHunger: not implemented yet.\n") ;
ErrorF("\tBlame Paquin, Fortune, Shupak, Weinstein, or Gould.\n") ;
return ;
}

void
ppcRefinanceNationalDebt()
{
ErrorF("ppcRefinanceNationalDebt: not implemented yet.\n") ;
return ;
}

void
ppcEndWarForever()
{
ErrorF("ppcEndWarForever: not implemented yet.\n") ;
return ;
}

void
ppcPayServerHackersWhatTheyreWorth()
{
ErrorF("ppcPayServerHackersWhatTheyreWorth: cannot execute.\n") ;
return ;
}

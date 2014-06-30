/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/offscreen.c,v 3.5 1996/12/23 06:52:48 dawes Exp $ */
/*
 * Copyright 1993 Gerrit Jan Akkerman 
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Gerrit Jan Akkerman not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * GERRIT JAN AKKERMAN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL GERRIT JAN AKKERMAN BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
*/
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
/* $XConsortium: offscreen.c /main/4 1996/02/21 17:56:55 kaleb $ */

#include "X.h"
#include "compiler.h"
#include "vgaVideo.h"

#include "windowstr.h" /* GJA -- for pWin */
#include "scrnintstr.h" /* GJA -- for pWin */
#include "pixmapstr.h" /* GJA -- for pWin */
#include "ppc.h" /* GJA -- for pWin */

unsigned char *saved_screen;

#define SAVEDSCREEN(pWin, x, y) \
	(*(saved_screen + (y) * (BYTES_PER_LINE(pWin) << 3) + (x)))

#define DO_ROP(src,dst,alu,planes) \
	((dst) = do_rop((src),(dst),(alu),(planes)))

/* NOTE:
 * The following to functions don't do anything. They're just there to
 * provide a stable interface to the rest of the system.
 */

/* Save the screen pixels, and stow away a pointer to their data in
 * saved_screen.
 */
vgaSaveScreenPix(pScreen,ppix)
PixmapPtr ppix;
ScreenPtr pScreen;
{
          vgaReadColorImage( (WindowPtr)(pScreen->devPrivate),
			0,0,pScreen->width,pScreen->height,
                        ppix->devPrivate.ptr,ppix->devKind);
          saved_screen = ppix->devPrivate.ptr;
}

/* Restore the screen pixels
 */
vgaRestoreScreenPix(pScreen,ppix)
PixmapPtr ppix;
ScreenPtr pScreen;
{
          vgaDrawColorImage( (WindowPtr)(pScreen->devPrivate),
			0,0,pScreen->width,pScreen->height,
                        ppix->devPrivate.ptr,ppix->devKind,GXcopy,0x0F);

}

int do_rop(src,dst,alu,planes)
int src,dst,alu;
const unsigned long planes;
{ 
	int _dst;	/* New dst */

	switch ( alu ) { 
		case GXclear:		/* 0x0 Zero 0 */ 
			_dst = 0; break ; 
		case GXinvert:		/* 0xa NOT dst */ 
			_dst = ~dst; break ; 
		case GXset:		/* 0xf 1 */ 
			_dst = src; break ; 
		case GXnoop:		/* 0x5 dst */ 
			return dst; 
		case GXnor:		/* 0x8 NOT src AND NOT dst */ 
			_dst = ~src & ~dst; break ; 
		case GXandInverted:	/* 0x4 NOT src AND dst */ 
			_dst = ~src & dst; break ; 
		case GXand:		/* 0x1 src AND dst */ 
			_dst = src & dst; break ; 
		case GXequiv:		/* 0x9 NOT src XOR dst */ 
			_dst = ~src ^ dst; break ; 
		case GXxor:		/* 0x6 src XOR dst */ 
			_dst = src ^ dst; break ; 
		case GXandReverse:	/* 0x2 src AND NOT dst */ 
			_dst = src & ~dst; break ; 
		case GXnand:		/* 0xe NOT src OR NOT dst */ 
			_dst = ~src | ~dst; break ; 
		case GXorReverse:	/* 0xb src OR NOT dst */ 
			_dst = src | ~dst; break ; 
		case GXorInverted:	/* 0xd NOT src OR dst */ 
			_dst = ~src | dst; break ; 
		case GXor:		/* 0x7 src OR dst */ 
			_dst = src | dst; break ; 
		case GXcopyInverted:	/* 0xc NOT src */ 
			_dst = ~src; break ; 
		case GXcopy:		/* 0x3 src */ 
			_dst = src; break ; 
		default: 
			break ; 
	} 
	return (dst & ~planes) | (_dst & planes); 
}

/* File vgaBitBlt.c */
void
offBitBlt( pWin, alu, readplanes, writeplanes, x0, y0, x1, y1, w, h )
WindowPtr pWin; /* GJA */
const int alu, readplanes, writeplanes ;
register int x0 ;
int y0 ;
register int x1 ;
int y1 ;
register int w, h ;
{
	int x,y,tmp1,tmp2;

	switch ( alu ) {
		case GXclear:		/* 0x0 Zero 0 */
		case GXinvert:		/* 0xa NOT dst */
		case GXset:		/* 0xf 1 */
			offFillSolid( pWin, VGA_ALLPLANES, alu, writeplanes,
				x0, y0, w, h ) ;
		case GXnoop:		/* 0x5 dst */
			return ;
		default:
			break ;
	}

	if ( (w <= 0) || (h <= 0) ) return;

	for ( y = 0 ; y < h ; y++ ) {
		for ( x = 0 ; x < w ; x++ ) {
			DO_ROP(SAVEDSCREEN(pWin,x0+x,y0+y),SAVEDSCREEN(pWin,x1+x,y1+y),
				alu,writeplanes);
		}
	}
}

/* for file vgaImages.c */

void
offDrawColorImage( pWin, x, y, w, h, data, RowIncrement, alu, planes )
WindowPtr pWin; /* GJA */
int x, y ;
register int w, h ;
unsigned char *data ;
register int RowIncrement ;
const int alu ;
const unsigned long int planes ;
{
	int dx,dy;
	int src, dst;

	for ( dy = 0 ; dy < h ; dy++ ) {
		for ( dx = 0 ; dx < w ; dx++ ) {
			DO_ROP(	data[dy * RowIncrement + dx],
				SAVEDSCREEN(pWin,x+dx,y+dy), alu, planes);
		}
	}
}

void
offReadColorImage( pWin, x, y, lx, ly, data, RowIncrement )
WindowPtr pWin; /* GJA */
int x, y ;
int lx, ly ;
unsigned char *data ;
int RowIncrement ;
{
	int dx, dy;

	if ( ( lx <= 0 ) || ( ly <= 0 ) )
		return ;

	for ( dy = 0 ; dy < ly ; dy++ ) {
		for ( dx = 0 ; dx < lx ; dx++ ) {
			data[dy*RowIncrement+dx] = SAVEDSCREEN(pWin,x+dx,y+dy);
		}
	}
}

/* for file vgaLine.c */

/* X increasing */
static void fast_x_line_right( et, e1, e2, len, y_increment, addr, alu, color,
	planes )
register int et ;
const int e1 ;
const int e2 ;
unsigned int len ;
const int y_increment ;
register unsigned volatile char *addr ;
const int alu, color, planes ;
{

	while ( len ) {
		DO_ROP(color, *addr, alu, planes);
		if ( et < 0 ) {
			et += e1 ;
		}
		else {
			et += e2 ;
			addr += y_increment ;
		}
		addr++;
		len--;
	}
}

/* X decreasing */
static void fast_x_line_left( et, e1, e2, len, y_increment, addr, alu, color,
	planes )
register int et ;
const int e1 ;
const int e2 ;
unsigned int len ;
const int y_increment ;
register unsigned volatile char *addr ;
const int alu, color, planes ;
{

	while ( len ) {	
		DO_ROP(color, *addr, alu, planes);
		if ( et < 0 ) {
			et += e1 ;
		}
		else {
			et += e2 ;
			addr += y_increment ;
		}
		addr--;
		len--;
	}
}

/* X increasing */
static void fast_y_line_right( et, e1, e2, len, y_increment, addr, alu, color,
	planes )
register int et ;
const int e1 ;
const int e2 ;
unsigned int len ;
const int y_increment ;
register unsigned volatile char *addr ;
const int alu, color, planes ;
{

	while ( len ) {
		DO_ROP(color, *addr, alu, planes);
		addr += y_increment ;
		if ( et < 0 ) {
			et += e1 ;
		} else {
			et += e2 ;
			addr++;
		}
		len--;
	}
}


/* X decreasing */
static void fast_y_line_left( et, e1, e2, len, y_increment, addr, alu, color,
	planes )
register int et ;
const int e1 ;
const int e2 ;
unsigned int len ;
const int y_increment ;
register unsigned volatile char *addr ;
const int alu, color, planes ;
{
	while ( len ) {
		DO_ROP(color, *addr, alu, planes);
		addr += y_increment ;
		if ( et < 0 ) {
			et += e1 ;
		} else {
			et += e2 ;
			addr--;
		}
		len--;
	}
}

void offBresLine( pWin, color, alu, planes, signdx, signdy,
		  axis, x, y, et, e1, e2, len )
WindowPtr pWin; /* GJA */
unsigned long int color ;
int alu ;
unsigned long int planes ;
int signdx, signdy ;
int axis, x, y ;
int et, e1, e2 ;
unsigned long int len ;
{
if ( !( planes & VGA_ALLPLANES ) ) {
	return ;
} else if ( len == 1 ) {
	offFillSolid( pWin, color, alu, planes, x, y, 1, 1 ) ;
	return ;
}

/* Call the real workers */
(* ( ( signdx > 0 ) ? ( axis ? fast_y_line_right : fast_x_line_right )
		    : ( axis ? fast_y_line_left  : fast_x_line_left ) ) )
			( et, e1, e2, (unsigned int)len,
			  ( signdy > 0 ?   (BYTES_PER_LINE(pWin)<<3)
				       : - (BYTES_PER_LINE(pWin)<<3) ),
			  &SAVEDSCREEN( pWin, x, y ), alu,
			  (const int)color, (const int)planes ) ;

return ;
}

/* For file vgaSolid.c */

void offFillSolid( pWin, color, alu, planes, x0, y0, lx, ly )
WindowPtr pWin; /* GJA */
unsigned long int color ;
const int alu ;
unsigned long int planes ;
register int x0 ;
register const int y0 ;
register int lx ;
register const int ly ;		/* MUST BE > 0 !! */
{
	int dx, dy;

	if ( ( lx == 0 ) || ( ly == 0 ) )
		return;

	for ( dy = 0 ; dy < ly ; dy++ ) {
		for ( dx = 0 ; dx < lx ; dx++ ) {
			DO_ROP(color,SAVEDSCREEN(pWin, x0+dx,y0+dy),alu,planes);
		}
	}
}
	
/* For file vgaStipple.c */

/* GJA -- modified this to take both Width and Height, and to
 * reduce x and y to Width and Height by taking remainders.
 */
static unsigned char
xygetbits( x, y, Width, paddedByteWidth, Height, data )
register int x,y ;
register const unsigned int Width, paddedByteWidth, Height ;
register const unsigned char * const data ;
{
	register unsigned char bits ;
	unsigned const char *lineptr, *cptr ;
	register shift ;
	register wrap ;

	x = x % Width;
	y = y % Height;

	lineptr = data + (y * paddedByteWidth);
	cptr = lineptr + (x >> 3) ;
	bits = *cptr ;
	if ( shift = x & 7 )
		bits = SCRLEFT8( bits, shift ) |
			SCRRIGHT8( cptr[1], ( 8 - shift ) ) ;
	if ( ( wrap = x + 8 - Width ) > 0 ) {
		bits &= SCRLEFT8( 0xFF, wrap ) ;
		bits |= SCRRIGHT8( *lineptr, ( 8 - wrap ) ) ;
	}

	return bits ;
}

static void
DoMono( pWin, w, x, y, mastersrc, h, width, paddedByteWidth, height,
	      xshift, yshift, alu, planes, fg )
WindowPtr pWin; /* GJA */
int x, y ;
register const unsigned char *mastersrc ;
int w, h ;			/* width and height of area to be stippled */
unsigned int width, height ;	/* width and height of stipple */
register unsigned int paddedByteWidth;
int xshift ;
int yshift ;
int alu, planes, fg;
{
	int dy, dx, i;
	int byte;
	int color;

	for ( dy = 0 ; dy < h ; dy++ ) {
		for ( dx = 0; dx <= w - 8 ; dx += 8 ) {
			/* get next byte */
			byte = xygetbits(dx+xshift,dy+yshift,width,
					paddedByteWidth, height, mastersrc);
			for ( i = 0 ; i < 8 ; i++ ) {
				if ( byte & (128 >> i) ) {
					DO_ROP(fg,SAVEDSCREEN(pWin,x+dx+i,y+dy),
						alu,planes);
				}
			}
		}
		/* get last bits */
		byte = xygetbits(dx+xshift,dy+yshift,width,
				paddedByteWidth, height, mastersrc);
		for ( i = 0 ; i < (w - dx) ; i++ ) {
			if ( byte & (128 >> i) ) {
				DO_ROP(fg,SAVEDSCREEN(pWin,x+dx+i,y+dy),
					alu,planes);
			}
		}
	}
}

void
offDrawMonoImage( pWin, data, x, y, w, h, fg, alu, planes )
WindowPtr pWin; /* GJA */
unsigned char *data;
int x, y, w, h ;
unsigned long int fg ;
int alu ;
unsigned long int planes;
{

	if ( ( alu == GXnoop ) || !( planes &= VGA_ALLPLANES ) )
		return ;

	DoMono( pWin, w, x, y, (const unsigned char *) data, h,
		      w, ( ( w + 31 ) & ~31 ) >> 3, h, 0, 0, alu,
		      (int)planes, (int)fg) ;

}

void
offFillStipple( pWin, pStipple, fg, alu, planes, x, y, w, h, xSrc, ySrc )
WindowPtr pWin; /* GJA */
register PixmapPtr const pStipple ;
unsigned long int fg ;
const int alu ;
unsigned long int planes ;
int x, y, w, h ;
const int xSrc, ySrc ;
{
	unsigned int width ;
	unsigned int height ;
	int xshift ;
	int yshift ;

	if ( ( alu == GXnoop ) || !( planes &= VGA_ALLPLANES ) )
		return ;

	/* Figure Bit Offsets & Source Address */
	width = pStipple->drawable.width ;
	if ( ( xshift = ( x - xSrc ) ) < 0 )
		xshift = width - ( ( - xshift ) % width ) ;
	else
		xshift %= width ;

	height = pStipple->drawable.height ;
	if ( ( yshift = ( y - ySrc ) ) < 0 )
		yshift = height - ( ( - yshift ) % height ) ;
	else
		yshift %= height ;

	DoMono( pWin, w, x, y,
		(const unsigned char *) pStipple->devPrivate.ptr,
		h,
		width,
		( ( width + 31 ) & ~31 ) >> 3,
		height,
		xshift, yshift,
		alu, (int)planes, (int)fg ) ;
	return ;
}

/*
 * $XConsortium: ibmColor.c,v 1.4 94/02/23 15:55:19 dpw Exp $
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
#include "Xproto.h"
#include "screenint.h"
#include "scrnintstr.h"
#include "colormapst.h"

#include "ibmTrace.h"
#include "compiler.h"

char 	*ibmBlackPixelText = "black";
char 	*ibmWhitePixelText = "white";

void
ibmResolveColorMono(pred, pgreen, pblue, pVisual)
    unsigned short	*pred, *pgreen, *pblue;
    VisualPtr		pVisual;
{
    TRACE(("ibmResolveColorMono(pred=0x%x,pgreen=0x%x,pblue=0x%x,pVis=0x%x)\n",
						pred,pgreen,pblue,pVisual));
    /* Gets intensity from RGB.  If intensity is >= half, pick white, else
     * pick black.  This may well be more trouble than it's worth. */
    *pred = *pgreen = *pblue = 
        (((39L * *pred +
           50L * *pgreen +
           11L * *pblue) >> 8) >= (((1<<8)-1)*50)) ? ~0 : 0;
    return ;
}

void
ibmCreateColormapMono(pMap)
    ColormapPtr	pMap;
{
    unsigned short	red, green, blue;
    Pixel pix;

    TRACE(("ibmCreateColormapMono(pmap=0x%x)\n",pMap));
    /* this is a monochrome colormap, it only has two entries, just fill
     * them in by hand.  If it were a more complex static map, it would be
     * worth writing a for loop or three to initialize it */
    red = green = blue = 0;
    pix = pMap->pScreen->blackPixel;
    AllocColor(pMap, &red, &green, &blue, &pix, 0);
    red = green = blue = ~0;
    pix = pMap->pScreen->whitePixel;
    AllocColor(pMap, &red, &green, &blue, &pix, 0);
    return ;
}

void
ibmDestroyColormapMono(pmap)
    ColormapPtr	pmap;
{
    TRACE(("ibmDestroyColormapMono(pmap=0x%x)\n",pmap));
    return ;
}

static int
ibmGetColorFromString( pCmap, name, pRed, pGreen, pBlue )
ColormapPtr pCmap ;
char *name ;
unsigned short int *pRed ;
unsigned short int *pGreen ;
unsigned short int *pBlue ;
{
int ndx, ch, red, green, blue ;
int len = strlen( name ) ;

    if ( name[0] == '#' ) {
	len-- ;
	name++ ;
	if ( len % 3 ) /* length not multiple of 3 */
	    return 0 ;
	len /= 3 ;
	red = green = blue = 0 ;
	do {
	    red = green ;
	    green = blue ;
	    blue = 0 ;
	    for ( ndx = len ; ndx-- ; ) {
		blue *= 16 ;
		ch = *name++ ;
		if ( ch >= '0' && ch <= '9' )
			blue += ch - '0' ;
		else if ( ch >= 'A' && ch <= 'F' )
			blue += ch - 'A' + 10 ;
		else if ( ch >= 'a' && ch <= 'f' )
			blue += ch - 'a' + 10 ;
		else
			return 0 ;
	    }
	} while ( *name ) ;
	len = 16 - ( len * 4 ) ;
	*pRed = red << len ;
	*pGreen = green << len ;
	*pBlue = blue << len ;
	return 1 ;
    }
    else
	return OsLookupColor( pCmap->pScreen->myNum, name, len,
			     pRed, pGreen, pBlue ) ;
}

/* NOTE: ibmBlackPixelText & ibmWhitePixelText are ddx-arguments from
 *	 the command line. ( or so I hope )
 */

void
ibmAllocBlackAndWhitePixels( pCmap )
ColormapPtr pCmap ;
{
unsigned short int r, g, b ;

#define SERVER_ID 0

    /* Check and see if I can use the command-line BlackPixel */
    if ( !ibmGetColorFromString( pCmap, ibmBlackPixelText, &r, &g, &b ) ) {
	ibmInfoMsg( ( ibmBlackPixelText[0] == '#' )
			? "illegal numeric color specification \"%s\"\n"
			: "Couldn't find \"%s\" in rgb database\n",
			ibmBlackPixelText ) ;
	ibmInfoMsg( "using default BlackPixel\n" ) ;
	r = g = b = 0 ; /* zeros oughta be pretty black, I think */
    }
    AllocColor( pCmap, &r, &g, &b, &(pCmap->pScreen->blackPixel), SERVER_ID ) ;

    /* Check and see if I can use the command-line WhitePixel */
    if ( !ibmGetColorFromString( pCmap, ibmWhitePixelText, &r, &g, &b ) ) {
	ibmInfoMsg( ( ibmWhitePixelText[0] == '#' )
			? "illegal numeric color specification \"%s\"\n"
			: "Couldn't find \"%s\" in rgb database\n",
			ibmWhitePixelText ) ;
	ibmInfoMsg( "using default WhitePixel\n" ) ;
	r = g = b = 0xFFFF ; /* should be pretty white, I think */
    }
    AllocColor( pCmap, &r, &g, &b, &(pCmap->pScreen->whitePixel), SERVER_ID ) ;

return ;
}

unsigned long int
ibmFindColor( cmap, r, g, b )
ColormapPtr cmap ;
unsigned short int r, g, b ;
{
    register int i ;
    register EntryPtr pent ;
    register unsigned long int delta ;
    register unsigned long int bestdiff ;
    unsigned long int best ;
    VisualPtr pVisual ;
    Pixel retval ;

    TRACE( ( "ibmFindColor(0x%x,%d,%d,%d,0x%x)\n", cmap, r, g, b ) ) ;

    best = 0 ;
    bestdiff = (unsigned long int) ~ 0 ; /* MAXINT for unsigned */
    pVisual = cmap->pVisual ;

    switch ( cmap->class ) {
    case StaticColor:
    case PseudoColor:
    case StaticGray:
    case GrayScale:
	for ( i = pVisual->ColormapEntries ; i-- ; ) {
		pent = &(cmap->red[i]) ;
	    	if ( pent->refcnt > 0 ) {
			delta = ABS( ( (signed long int) pent->co.local.red )
				     - r )
			      + ABS( ( (signed long int) pent->co.local.green )
				     - g )
			      + ABS( ( (signed long int) pent->co.local.blue )
				     - b ) ;
			if ( !delta )
				return i ;
			else if ( delta < bestdiff ) {
				bestdiff = delta ;
				best = i ;
			}
		}
	}
	return best ;

    case TrueColor:
    case DirectColor:
	/* red */
	for ( i = pVisual->ColormapEntries ; i-- ; ) {
	    delta = ABS( cmap->red[i].co.local.red - r ) ;
	    if ( delta < bestdiff ) {
		bestdiff = delta ;
		best = i ;
	    }
	}
	retval = best << pVisual->offsetRed ;

	/* green */
	best = 0 ;
	bestdiff = (unsigned long int) ~ 0 ;
	for ( i = pVisual->ColormapEntries ; i-- ; ) {
	    delta = ABS( cmap->green[i].co.local.green - g ) ;
	    if ( delta < bestdiff ) {
		bestdiff = delta ;
		best = i ;
	    }
	}
	retval |= best << pVisual->offsetGreen ;

	/* blue */
	best = 0 ;
	bestdiff = (unsigned long int) ~ 0 ;
	for ( i = pVisual->ColormapEntries ; i-- ; ) {
	    delta = ABS( cmap->blue[i].co.local.blue - b ) ;
	    if ( delta < bestdiff ) {
		bestdiff = delta ;
		best = i ;
	    }
	}
	return retval | ( best << pVisual->offsetBlue ) ;

    default:
	ErrorF( "ibmFindColor: Unsupported Visual Class\n" ) ;
	return 0 ;
    }
    /*NOTREACHED*/
}

/*
   Remember that unallocated Colors are UNDEFINED in X, so this routine
   is merely being nice by existing at all.  It turns out that it makes
   life easier in general for a lot of applications which are subject
   to existing colormaps.

   You cannot say that these values are bad or wrong, even in the case
   of GrayScale (note ResolveColor is not called), because they are merely
   substitutes for indeterminate uninitialized hardware.  Even in gray,
   they make life nice, it turns out.
*/

#define LOWINTEN 0x8000
#define HI_INTEN 0xFFFF

void
ibmDefineDefaultColormapColors( pVisual, pCmap )
register VisualPtr pVisual ;
register ColormapPtr pCmap ;
{
register int i ;

/* Read The Note about classes in <X11/X.h> */

if ( pVisual->class & 1 ) /* Dynamic classes have odd values */
    if ( pVisual->class == DirectColor ) /* Can't Handle Yet */
	ibmInfoMsg(
     "ibmDefineDefaultColormapColors: No defaults for DirectColor Visual\n" ) ;
    else /* Must be PseudoColor or GrayScale */
	switch ( i = pVisual->ColormapEntries ) {
	   case 2: /* Monochrome */
		pCmap->red[0].co.local.red   = 0 ;
		pCmap->red[0].co.local.green = 0 ;
		pCmap->red[0].co.local.blue  = 0 ;
		pCmap->red[1].co.local.red   = HI_INTEN ;
		pCmap->red[1].co.local.green = HI_INTEN ;
		pCmap->red[1].co.local.blue  = HI_INTEN ;
		break ;
	   case 16: /* DEFINING bits to be IRGB */
		while ( i-- > 8 ) {
			pCmap->red[i].co.local.red   = (HI_INTEN*((i&4)>>2)) ;
			pCmap->red[i].co.local.green = (HI_INTEN*((i&2)>>1)) ;
			pCmap->red[i].co.local.blue  = (HI_INTEN*(i&1)) ;
		}
		while ( i-- ) {
			pCmap->red[i].co.local.red   = (LOWINTEN*((i&4)>>2)) ;
			pCmap->red[i].co.local.green = (LOWINTEN*((i&2)>>1)) ;
			pCmap->red[i].co.local.blue  = (LOWINTEN*(i&1)) ;
		}
		break ;
	   case 256: /* defining to be RRRGGGBB */
		while ( i-- ) {
			pCmap->red[i].co.local.red  =
				( ( i & 0xE0 ) >> 5 ) * ( HI_INTEN / 7 ) ;
			pCmap->red[i].co.local.green=
				( ( i & 0x1C ) >> 2 ) * ( HI_INTEN / 7 ) ;
			pCmap->red[i].co.local.blue =
				( i & 0x03 ) * ( HI_INTEN / 3 ) ;
		}
		break ;
	   default:
		ErrorF(
	"ibmDefineDefaultColormapColors: Bad num of map entries\n" ) ;
		/* But no big deal ; who cares? */
		break ;
	} /* end switch */

return ;
}

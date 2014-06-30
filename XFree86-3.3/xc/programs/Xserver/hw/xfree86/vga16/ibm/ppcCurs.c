/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/ppcCurs.c,v 3.3 1996/12/23 06:52:57 dawes Exp $ */
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

/* $XConsortium: ppcCurs.c /main/4 1996/02/21 17:57:28 kaleb $ */

/*
 * Realize and Unrealize cursor
 *
 * T. Paquin 8/87
 *
 * Takes the pCurs source and mask fields, and creates foreground and background
 * fields in the devPriv.
 *
 */
#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"
#include "pixmapstr.h"

#include "OScompiler.h"
#include "windowstr.h"	/* GJA */
#include "ppc.h"
#include "ppcBitMacs.h"
#include "ibmTrace.h"

#if !defined(IMAGE_BYTE_ORDER)
	******** ERROR ********
#endif

/* We want to access the cursor as a sequence of bytes.
 * However, the data is defined to be a sequence of longs
 * So, we convert if needed!
 */
#if defined(CURSOR_DEBUG)
void
print_ppc_cursor_data( src, width, height )
unsigned long int *src ;
unsigned int width ;
unsigned int height ;
{
register unsigned char *c_ptr ;
register unsigned long int *l_ptr ;
register unsigned int i ;

extern int puts(), printf() ;

(void) puts( "First Array" ) ;
l_ptr = src ;
for ( i = height ; i-- ; l_ptr++ ) {
	c_ptr = (unsigned char *) l_ptr ;
	(void) printf( " %08X	%02X %02X %02X %02X\n",
		       *l_ptr, c_ptr[0], c_ptr[1], c_ptr[2], c_ptr[3] ) ;
}
(void) puts( "Second Array" ) ;
l_ptr = src + 32 ;
for ( i = height ; i-- ; l_ptr++ ) {
	c_ptr = (unsigned char *) l_ptr ;
	(void) printf( " %08X	%02X %02X %02X %02X\n",
		       *l_ptr, c_ptr[0], c_ptr[1], c_ptr[2], c_ptr[3] ) ;
}

return ;
}
#endif

Bool
ppcRealizeCursor( pScr, pCurs )
    ScreenPtr	pScr ;
    CursorPtr	pCurs ;
{
    register unsigned long int *pFG, *pBG ;
    register int i ;
    register unsigned long int tmp ;
    unsigned long int endbits ;
    unsigned long int *psrcImage, *psrcMask ;
    int srcWidth ;
    int srcHeight ;
    int srcRealWidth ;

    TRACE(("ppcRealizeCursor( pScr= 0x%x, pCurs= 0x%x)\n",pScr,pCurs)) ;
    if ( ! ( pCurs->devPriv[ pScr->myNum ] = (pointer) xalloc( 256 ) ) )
	{
	ErrorF("ppcRealizeCursor: can't malloc\n") ;
	return FALSE ;
	}
    pFG = (unsigned long int *) pCurs->devPriv[pScr->myNum] ;
    pBG = pFG + 32 ; /* words */
    bzero( (char *) pFG, 256 ) ;
    psrcImage = (unsigned long int *) pCurs->bits->source ;
    psrcMask = (unsigned long int *) pCurs->bits->mask ;
    srcRealWidth = ( pCurs->bits->width + 0x1F ) >> 5 ;

    srcHeight = MIN( pCurs->bits->height, 32 ) ;

    endbits = ( ( srcWidth  = MIN( pCurs->bits->width, 32 ) ) == 32 )
	    ? 0xFFFFFFFF
	    : SCRLEFT( -1, ( 32 - srcWidth ) ) ;

    for ( i = srcHeight ; i-- ; ) {
	tmp = *psrcMask & endbits ;
	*pFG++ = ( tmp & *psrcImage ) ;
	*pBG++ = ( tmp & ~ *psrcImage ) ;
	psrcImage = psrcImage + srcRealWidth ;
	psrcMask = psrcMask + srcRealWidth ;
    }

#if defined(CURSOR_DEBUG)
print_ppc_cursor_data( pCurs->devPriv[pScr->myNum],
		       srcRealWidth << 5, srcHeight ) ;
#endif

    TRACE(("exiting ppcRealizeCursor\n")) ;
    return TRUE ;
}

Bool
ppcUnrealizeCursor( pScr, pCurs )
    register ScreenPtr 	pScr ;
    register CursorPtr 	pCurs ;
{

    TRACE(("ppcUnrealizeCursor( pScr= 0x%x, pCurs= 0x%x )\n",pScr,pCurs)) ;

    xfree( pCurs->devPriv[ pScr->myNum ] ) ;
    pCurs->devPriv[ pScr->myNum ] = 0 ;
    return TRUE ;
}

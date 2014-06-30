/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/ppcBitMacs.h,v 3.1 1996/12/23 06:52:52 dawes Exp $ */
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
/***********************************************************
		Copyright IBM Corporation 1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $XConsortium: ppcBitMacs.h /main/3 1996/02/21 17:57:09 kaleb $ */

/* Screen Bit-Order Macros */
#if !defined(BITMAP_BIT_ORDER)
#define BITMAP_BIT_ORDER MSBFirst
#endif

#if !defined(IMAGE_BYTE_ORDER)
#define IMAGE_BYTE_ORDER LSBFirst
#endif

/* Bit Ordering Macros */
#if !defined(BITMAP_BIT_ORDER) || !defined(IMAGE_BYTE_ORDER)
#endif

/* Byte Swap Macro */
#if IMAGE_BYTE_ORDER == BITMAP_BIT_ORDER
#define LONG2CHARS( x ) ( x )
#else
#define LONG2CHARS( x )	( ( ( ( x ) & 0x000000FF ) << 0x18 ) \
			| ( ( ( x ) & 0x0000FF00 ) << 0x08 ) \
			| ( ( ( x ) & 0x00FF0000 ) >> 0x08 ) \
			| ( ( ( x ) & 0xFF000000 ) >> 0x18 ) )
#endif

#if !defined(SCRLEFT)
#if (BITMAP_BIT_ORDER == MSBFirst)	/* pc/rt */
#define SCRLEFT( lw, n ) LONG2CHARS( LONG2CHARS( (unsigned ) lw ) << ( n ) )
#else
#if (BITMAP_BIT_ORDER == LSBFirst)	/* intel */
#define SCRLEFT( lw, n ) LONG2CHARS( LONG2CHARS( (unsigned ) lw ) >> ( n ) )
#else
	******** ERROR ********
#endif
#endif
#endif
#if !defined(SCRRIGHT)
#if (BITMAP_BIT_ORDER == MSBFirst)	/* pc/rt */
#define SCRRIGHT( lw, n ) LONG2CHARS( LONG2CHARS( (unsigned ) lw ) >> ( n ) )
#else
#if (BITMAP_BIT_ORDER == LSBFirst)	/* intel */
#define SCRRIGHT( lw, n ) LONG2CHARS( LONG2CHARS( (unsigned ) lw ) << ( n ) )
#else
	******** ERROR ********
#endif
#endif
#endif

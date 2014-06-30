/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach8/mach8fcach.c,v 3.7 1996/12/23 06:39:51 dawes Exp $ */
/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Kevin E. Martin makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN OR RICKARD E. FAITH BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: mach8fcach.c /main/5 1996/02/21 17:30:11 kaleb $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"cfb.h"
#include	"misc.h"
/*
#include	"windowstr.h"
#include	"gcstruct.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
*/
#include	"regmach8.h"
#include	"mach8.h"
#include	"xf86bcache.h"
#include	"xf86fcache.h"
#include	"xf86text.h"
#include	"mach8cache.h"

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
extern int xf86Verbose;

#define ALIGNMENT 8
#define N_PLANES 8
#define PIXMAP_WIDTH 64

void
mach8FontCache8Init()
{
    static int first = 1;
    int x, y, w, h;
    unsigned int BitPlane;
    CachePool FontPool;

    x = PIXMAP_WIDTH;
    y = mach8InfoRec.virtualY;
    w = ( mach8InfoRec.videoRam > 512 ? 1024 - x : mach8InfoRec.virtualX - x );
    h = ( mach8InfoRec.videoRam > 512 ? 1024 - y :
		  (mach8InfoRec.videoRam * 1024) / mach8InfoRec.virtualX - y );
    if( h >= PIXMAP_WIDTH && first ) {
      mach8InitFrect( 0, y, PIXMAP_WIDTH );
      ErrorF( "%s %s: Using a single %dx%d area for expanding pixmaps\n",
	      XCONFIG_PROBED, mach8InfoRec.name, PIXMAP_WIDTH, PIXMAP_WIDTH );
    }
    else if( first )
      ErrorF( "%s %s: No pixmap expanding area available\n",
	      XCONFIG_PROBED, mach8InfoRec.name );

    /*
     * Don't allow a font cache if we don't have room for at least
     * 2 complete 6x13 fonts.
     */
    if( w >= 6*32 && h >= 2*13 ) {
      if( first ) {
        FontPool = xf86CreateCachePool( ALIGNMENT );
        for( BitPlane = 0; BitPlane < N_PLANES; BitPlane++ )
	  xf86AddToCachePool( FontPool, x, y, w, h, BitPlane );

        xf86InitFontCache( FontPool, w, h, mach8FontOpStipple );
        xf86InitText( mach8GlyphWrite, mach8NoCPolyText, mach8NoCImageText );
        ErrorF( "%s %s: Using %d planes of %dx%d at %dx%d aligned %d as font cache\n",
	        XCONFIG_PROBED, mach8InfoRec.name,
		N_PLANES, w, h, x, y, ALIGNMENT );
      }
      else
        xf86ReleaseFontCache();
    }
    else if( first ) {
      /*
       * Crash and burn if the cached glyph write function gets called.
       */
      xf86InitText( NULL, mach8NoCPolyText, mach8NoCImageText );
      ErrorF( "%s %s: No font cache available\n",
	      XCONFIG_PROBED, mach8InfoRec.name );
    }
    first = 0;

}

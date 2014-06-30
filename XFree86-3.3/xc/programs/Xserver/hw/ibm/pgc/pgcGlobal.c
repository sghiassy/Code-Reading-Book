/*
 * $XConsortium: pgcGlobal.c,v 1.2 91/07/16 13:12:27 jap Exp $
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
#include "colormapst.h"
#include "colormap.h"
#include "pixmapstr.h"

#include "ibmTrace.h"
#include "pgc.h"

#define MAXDEV	8

pgcScreenRec pgcScreenInfo[MAXDEV] ;

pgcInitScreenProc()
{
}

void
pgcQueryBestSize( class, pwidth, pheight )
register int class ;
register short *pwidth ;
register short *pheight ;
{
if ( class == CursorShape )
  *pwidth = *pheight = 32 ;
else /* either TileShape or StippleShape */
  /* Round Up To Nearest Multiple Of 8 -- We don't care what height they use */
  *pwidth = ( *pwidth + 0x7 ) & ~ 0x7 ;

return ;

}

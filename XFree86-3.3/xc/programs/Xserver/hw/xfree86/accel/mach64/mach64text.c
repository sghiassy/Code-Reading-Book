/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64text.c,v 3.4 1996/12/23 06:39:28 dawes Exp $ */
/*
 * Copyright 1992,1993,1994 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)
 */
/* $XConsortium: mach64text.c /main/3 1996/02/21 17:29:16 kaleb $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"misc.h"
#include	"gcstruct.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"mi.h"
#include	"mach64.h"


int
mach64PolyText8(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int 	count;
    char	*chars;
{
    int plane;

    if (!xf86VTSema || (pGC->fillStyle != FillSolid) ||
	((plane = mach64CacheFont(pGC->font)) == -1))
	return miPolyText8(pDraw, pGC, x, y, count, chars);
    else
	return mach64DrawText(pDraw, pGC, x, y, count, (unsigned char *)chars,
				plane, POLY_TEXT_TYPE_8);
}


int
mach64PolyText16(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int		count;
    unsigned short *chars;
{
    return miPolyText16(pDraw, pGC, x, y, count, chars);
}


void
mach64ImageText8(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int		count;
    char	*chars;
{
    int plane;

    /* Don't need to check fill style here - it isn't used in image text */
    if (!xf86VTSema || ((plane = mach64CacheFont(pGC->font)) == -1))
	miImageText8(pDraw, pGC, x, y, count, chars);
    else
	mach64DrawText(pDraw, pGC, x, y, count, (unsigned char *)chars, plane, 
			IMAGE_TEXT_TYPE_8);
    return;
}


void
mach64ImageText16(pDraw, pGC, x, y, count, chars)
    DrawablePtr pDraw;
    GCPtr	pGC;
    int		x, y;
    int		count;
    unsigned short *chars;
{
    miImageText16(pDraw, pGC, x, y, count, chars);
}

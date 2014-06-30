/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/mach32font.c,v 3.5 1996/12/23 06:38:32 dawes Exp $ */
/*
 * Copyright 1992, 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 * Modified for the new cache by Mike Bernson (mike@mbsun.mlb.org)
 */
/* $XConsortium: mach32font.c /main/4 1996/02/21 17:26:36 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "mfb.h"
#include "fontstruct.h"
#include "dixfontstr.h"
#include "scrnintstr.h"
#include "mach32.h"

Bool
mach32RealizeFont(pScreen, font)
    ScreenPtr	pScreen;
    FontPtr	font;
{
    /* We _should_ probably be caching things here */
    /* (void)mach32CacheFont8(font); */
    return mfbRealizeFont(pScreen, font);
}

Bool
mach32UnrealizeFont(pScreen, font)
    ScreenPtr	pScreen;
    FontPtr	font;
{
    xf86UnCacheFont8(font);
    return mfbUnrealizeFont(pScreen, font);
}

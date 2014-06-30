/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxFont.c,v 3.3 1996/12/23 06:32:42 dawes Exp $ */
/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Kevin E. Martin makes no
 * representations about the suitability of this software for any purpose. It
 * is provided "as is" without express or implied warranty.
 * 
 * KEVIN E. MARTIN AND HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE 
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR 
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS 
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 * Modified by Amancio Hasty and Jon Tombs
 * Modified for the AGX by Henry A. Worth (haw30@eng.amdahl.com)
 * 
 */
/* $XConsortium: agxFont.c /main/5 1996/02/21 17:17:32 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "mfb.h"
#include "fontstruct.h"
#include "dixfontstr.h"
#include "scrnintstr.h"
#include "agx.h"

Bool
agxRealizeFont(pScreen, font)
     ScreenPtr pScreen;
     FontPtr font;
{
 /* We _should_ probably be caching things here */
 /* (void)agxCacheFont8(font); */
   return mfbRealizeFont(pScreen, font);
}

Bool
agxUnrealizeFont(pScreen, font)
     ScreenPtr pScreen;
     FontPtr font;
{
   agxUnCacheFont8(font);
   return mfbUnrealizeFont(pScreen, font);
}

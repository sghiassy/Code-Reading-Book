/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxGtImg.c,v 3.5 1996/12/23 06:32:48 dawes Exp $ */
/*
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in 
 * advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  David Wexelblat makes 
 * no representations about the suitability of this software for any 
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO 
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR 
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE 
 * OR PERFORMANCE OF THIS SOFTWARE.
 *
 * adapted to the AGX server by Henry A. Worth (haw30@eng.amdahl.com)
 */
/* $XConsortium: agxGtImg.c /main/5 1996/02/21 17:17:54 kaleb $ */

#include "X.h"
#include "windowstr.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "cfb.h"
#include "cfb16.h"
#ifdef AGX_32BPP
#include "cfb32.h"
#endif
#include "cfbmskbits.h"
#include "agx.h"

void
agxGetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine)
     DrawablePtr pDrawable;
     int         sx, sy, w, h;
     unsigned int format;
     Pixel planeMask;
     char *pdstLine;
{
   int width;

   if ((w == 0) || (h == 0))
      return;

   if (pDrawable->bitsPerPixel == 1)
   {
      mfbGetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
      return;
   }

   if (!xf86VTSema || pDrawable->type != DRAWABLE_WINDOW)
   {
      switch (pDrawable->bitsPerPixel) {
         case 1:
            mfbGetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
	    break;
         case 8:
            cfbGetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
	    break;
         case 16:
            cfb16GetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
	    break;
#ifdef AGX_32BPP
         case 32:
            cfb32GetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
	    break;
#endif
      }
      return;
   }

   width = PixmapBytePad(w, pDrawable->depth);
   if (format == ZPixmap && (planeMask & 0xff) == 0xff)
   {
      (*agxImageReadFunc)(sx+pDrawable->x, sy+pDrawable->y, w, h, 
			pdstLine, width, 0, 0, planeMask);
   }
   else
   {
      /*
       * Worry about this later (much!).  Should be straighforward, though.
       * Read an image into a dummy pixmap, then use cfbCopyPlane8to1 to
       * copy each plane in planeMask into the destination.  At least
       * this is the theory.
       */
      miGetImage(pDrawable, sx, sy, w, h, format, planeMask, pdstLine);
   }
}


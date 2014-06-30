/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxWin.c,v 3.6 1996/12/23 06:33:03 dawes Exp $ */
/*

Copyright (c) 1987  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL, KEVIN E. MARTIN, AND HENRY A. WORTH DISCLAIM ALL WARRANTIES 
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE LIABLE 
FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING 
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
Rewritten for the AGX by Henry A. Worth (haw30@eng.amdahl.com)

*/
/* $XConsortium: agxWin.c /main/5 1996/02/21 17:18:38 kaleb $ */

/*
 * Modified by Amancio Hasty and Jon Tombs
 * 
 */


#include "X.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "gcstruct.h"
#include "cfb.h"
#include "cfb16.h"
#ifdef AGX_32BPP
#include "cfb32.h"
#endif
#include "mistruct.h"
#include "regionstr.h"
#include "cfbmskbits.h"
#include "misc.h"
#include "xf86.h"
#include "agx.h"
#include "regagx.h"

void
agxCopyWindow(pWin, ptOldOrg, prgnSrc)
     WindowPtr pWin;
     DDXPointRec ptOldOrg;
     RegionPtr prgnSrc;
{
   RegionPtr prgnDst;
   BoxPtr    pboxOrig;
   BoxPtr    pbox;
   int dx, dy;
   int i, nbox;
   unsigned int *ordering;
   GC    dummyGC;
   unsigned int height;
   unsigned int width;
   unsigned int src_x;
   unsigned int src_y;
   Bool         incX = FALSE;
   Bool         incY = FALSE;

   if (!xf86VTSema)
   {
      switch (pWin->drawable.bitsPerPixel) {
        case 8:
          cfbCopyWindow(pWin, ptOldOrg, prgnSrc);
          break;
        case 16:
          cfb16CopyWindow(pWin, ptOldOrg, prgnSrc);
          break;
#ifdef AGX_32BPP
       case 32:
          cfb32CopyWindow(pWin, ptOldOrg, prgnSrc);
          break;
#endif
      }
      return;
   }

   dummyGC.subWindowMode = ~IncludeInferiors;

   prgnDst = (*pWin->drawable.pScreen->RegionCreate) (NULL, 1);

   incX = (dx = ptOldOrg.x - pWin->drawable.x) > 0;

   incY = (dy = ptOldOrg.y - pWin->drawable.y) > 0;

   (*pWin->drawable.pScreen->TranslateRegion) (prgnSrc, -dx, -dy);
   (*pWin->drawable.pScreen->Intersect) (prgnDst, &pWin->borderClip, prgnSrc);

   pboxOrig = REGION_RECTS(prgnDst);
   nbox = REGION_NUM_RECTS(prgnDst);

   ordering = (unsigned int *)ALLOCATE_LOCAL(nbox * sizeof(unsigned int));

   if (!ordering) {
      (*pWin->drawable.pScreen->RegionDestroy) (prgnDst);
      return;
   }
   agxFindOrdering((DrawablePtr)pWin, (DrawablePtr)pWin, &dummyGC, nbox,
			pboxOrig, ptOldOrg.x, ptOldOrg.y,
			pWin->drawable.x, pWin->drawable.y, ordering);

   GE_WAIT_IDLE();
   MAP_SET_SRC_AND_DST( GE_MS_MAP_A );
   GE_OUT_B( GE_FRGD_MIX, MIX_SRC );
   GE_OUT_D( GE_PIXEL_BIT_MASK, ~0 );

   if ( incX && incY ) {
      GE_OUT_W( GE_PIXEL_OP,
                GE_OP_PAT_FRGD
                | GE_OP_MASK_DISABLED
                | GE_OP_INC_X
                | GE_OP_INC_Y         );
      for (i = 0; i < nbox; i++) {
	 pbox = &pboxOrig[ordering[i]];
         width  = pbox->x2 - pbox->x1 - 1;
         height = pbox->y2 - pbox->y1 - 1;
         src_x = pbox->x1 + dx;
         src_y = pbox->y1 + dy;

         GE_WAIT_IDLE();
#ifndef NO_MULTI_IO
         GE_OUT_D( GE_SRC_MAP_X,  src_y << 16 | src_x );
         GE_OUT_D( GE_DEST_MAP_X, pbox->y1 << 16 | pbox->x1 );
         GE_OUT_D( GE_OP_DIM_WIDTH, height << 16 | width );
#else
         GE_OUT_W( GE_SRC_MAP_X, src_x );
         GE_OUT_W( GE_SRC_MAP_Y, src_y );
         GE_OUT_W( GE_DEST_MAP_X, pbox->x1 );
         GE_OUT_W( GE_DEST_MAP_Y, pbox->y1 );
         GE_OUT_W( GE_OP_DIM_WIDTH, width );
         GE_OUT_W( GE_OP_DIM_HEIGHT, height );
#endif
         GE_START_CMDW( GE_OPW_BITBLT
                        | GE_OPW_FRGD_SRC_MAP
                        | GE_OPW_SRC_MAP_A
                        | GE_OPW_DEST_MAP_A   );
      }
   } else if ( incX ) {
      GE_OUT_W( GE_PIXEL_OP,
                GE_OP_PAT_FRGD
                | GE_OP_MASK_DISABLED
                | GE_OP_INC_X
                | GE_OP_DEC_Y         );
      for (i = 0; i < nbox; i++) {
	 pbox = &pboxOrig[ordering[i]];
         width  = pbox->x2 - pbox->x1 - 1;
         height = pbox->y2 - pbox->y1 - 1;
         src_x = pbox->x1 + dx;
         src_y = pbox->y2 + dy - 1;

         GE_WAIT_IDLE();
#ifndef NO_MULTI_IO
         GE_OUT_D( GE_SRC_MAP_X,  src_y << 16 | src_x );
         GE_OUT_D( GE_DEST_MAP_X, (pbox->y2 - 1) << 16 | pbox->x1 );
         GE_OUT_D( GE_OP_DIM_WIDTH, height << 16 | width );
#else
         GE_OUT_W( GE_SRC_MAP_X, src_x );
         GE_OUT_W( GE_SRC_MAP_Y, src_y );
         GE_OUT_W( GE_DEST_MAP_X, pbox->x1 );
         GE_OUT_W( GE_DEST_MAP_Y, pbox->y2 - 1 );
         GE_OUT_W( GE_OP_DIM_WIDTH, width );
         GE_OUT_W( GE_OP_DIM_HEIGHT, height );
#endif
         GE_START_CMDW( GE_OPW_BITBLT
                        | GE_OPW_FRGD_SRC_MAP
                        | GE_OPW_SRC_MAP_A
                        | GE_OPW_DEST_MAP_A   );

      }
   } else if ( incY ) {
      GE_OUT_W( GE_PIXEL_OP,
                GE_OP_PAT_FRGD
                | GE_OP_MASK_DISABLED
                | GE_OP_DEC_X
                | GE_OP_INC_Y         );
      for (i = 0; i < nbox; i++) {
	 pbox = &pboxOrig[ordering[i]];
         width  = pbox->x2 - pbox->x1 - 1;
         height = pbox->y2 - pbox->y1 - 1;
         src_x = pbox->x2 + dx - 1;
         src_y = pbox->y1 + dy;

         GE_WAIT_IDLE();
#ifndef NO_MULTI_IO
         GE_OUT_D( GE_SRC_MAP_X,  src_y << 16 | src_x );
         GE_OUT_D( GE_DEST_MAP_X, pbox->y1 << 16 | (pbox->x2 - 1) );
         GE_OUT_D( GE_OP_DIM_WIDTH, height << 16 | width );
#else
         GE_OUT_W( GE_SRC_MAP_X, src_x );
         GE_OUT_W( GE_SRC_MAP_Y, src_y );
         GE_OUT_W( GE_DEST_MAP_X, pbox->x2 - 1 );
         GE_OUT_W( GE_DEST_MAP_Y, pbox->y1 );
         GE_OUT_W( GE_OP_DIM_WIDTH, width );
         GE_OUT_W( GE_OP_DIM_HEIGHT, height );
#endif
         GE_START_CMDW( GE_OPW_BITBLT
                        | GE_OPW_FRGD_SRC_MAP
                        | GE_OPW_SRC_MAP_A
                        | GE_OPW_DEST_MAP_A   );
      }
   } else {
      GE_OUT_W( GE_PIXEL_OP,
                GE_OP_PAT_FRGD
                | GE_OP_MASK_DISABLED
                | GE_OP_DEC_X
                | GE_OP_DEC_Y         );
      for (i = 0; i < nbox; i++) {
	 pbox = &pboxOrig[ordering[i]];
         width  = pbox->x2 - pbox->x1 - 1;
         height = pbox->y2 - pbox->y1 - 1;
         src_x = pbox->x2 + dx - 1;
         src_y = pbox->y2 + dy - 1;

         GE_WAIT_IDLE();
#ifndef NO_MULTI_IO
         GE_OUT_D( GE_SRC_MAP_X,  src_y << 16 | src_x );
         GE_OUT_D( GE_DEST_MAP_X, (pbox->y2 - 1) << 16 | (pbox->x2 - 1) );
         GE_OUT_D( GE_OP_DIM_WIDTH, height << 16 | width );
#else
         GE_OUT_W( GE_SRC_MAP_X, src_x );
         GE_OUT_W( GE_SRC_MAP_Y, src_y );
         GE_OUT_W( GE_DEST_MAP_X, pbox->x2 - 1 );
         GE_OUT_W( GE_DEST_MAP_Y, pbox->y2 - 1 );
         GE_OUT_W( GE_OP_DIM_WIDTH, width );
         GE_OUT_W( GE_OP_DIM_HEIGHT, height );
#endif
         GE_START_CMDW( GE_OPW_BITBLT
                        | GE_OPW_FRGD_SRC_MAP
                        | GE_OPW_SRC_MAP_A
                        | GE_OPW_DEST_MAP_A   );
      }
   }

   (*pWin->drawable.pScreen->RegionDestroy) (prgnDst);
   DEALLOCATE_LOCAL(ordering);
   
   GE_WAIT_IDLE_EXIT();
}

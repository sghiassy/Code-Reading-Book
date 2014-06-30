/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3win.c,v 3.10 1996/12/23 06:42:09 dawes Exp $ */
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

DIGITAL AND KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL DIGITAL OR KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

*/

/*
 * Modified by Amancio Hasty and Jon Tombs
 * 
 */
/* $XConsortium: s3win.c /main/6 1996/10/19 17:57:52 kaleb $ */


#include "X.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "gcstruct.h"
#include "cfb.h"
#include "cfb16.h"
#include "cfb24.h"
#include "cfb32.h"
#include "mistruct.h"
#include "regionstr.h"
#include "cfbmskbits.h"
#include "misc.h"
#include "xf86.h"
#include "s3.h"
#include "regs3.h"

void
s3CopyWindow(pWin, ptOldOrg, prgnSrc)
     WindowPtr pWin;
     DDXPointRec ptOldOrg;
     RegionPtr prgnSrc;
{
   RegionPtr prgnDst;
   register BoxPtr pbox, pboxOrig;
   register int dx, dy;
   register int i, nbox;
   short direction = 0;
   unsigned int *ordering;
   GC    dummyGC;

   if (!xf86VTSema)
   {
      switch (pWin->drawable.bitsPerPixel) {
	case 8:
	  cfbCopyWindow(pWin, ptOldOrg, prgnSrc);
          break;
        case 16:
	  cfb16CopyWindow(pWin, ptOldOrg, prgnSrc);
          break;
        case 24:
	  cfb24CopyWindow(pWin, ptOldOrg, prgnSrc);
          break;
        case 32:
	  cfb32CopyWindow(pWin, ptOldOrg, prgnSrc);
	  break;
      }
      return;
   }

   dummyGC.subWindowMode = ~IncludeInferiors;

   prgnDst = (*pWin->drawable.pScreen->RegionCreate) (NULL, 1);

   if ((dx = ptOldOrg.x - pWin->drawable.x) > 0)
      direction |= INC_X;

   if ((dy = ptOldOrg.y - pWin->drawable.y) > 0)
      direction |= INC_Y;

   (*pWin->drawable.pScreen->TranslateRegion) (prgnSrc, -dx, -dy);
   (*pWin->drawable.pScreen->Intersect) (prgnDst, &pWin->borderClip, prgnSrc);

   pboxOrig = REGION_RECTS(prgnDst);
   nbox = REGION_NUM_RECTS(prgnDst);

   ordering = (unsigned int *)ALLOCATE_LOCAL(nbox * sizeof(unsigned int));

   if (!ordering) {
      (*pWin->drawable.pScreen->RegionDestroy) (prgnDst);
      return;
   }
   s3FindOrdering((DrawablePtr)pWin, (DrawablePtr)pWin, &dummyGC, nbox,
			pboxOrig, ptOldOrg.x, ptOldOrg.y,
			pWin->drawable.x, pWin->drawable.y, ordering);

   BLOCK_CURSOR;
   WaitQueue16_32(3,4);
   SET_MIX(FSS_BITBLT | MIX_SRC, BSS_BKGDCOL | MIX_SRC);
   SET_WRT_MASK(~0);

   if (direction == (INC_X | INC_Y)) {
      for (i = 0; i < nbox; i++) {
	 pbox = &pboxOrig[ordering[i]];

	 WaitQueue(7);
	 SET_CURPT((short)(pbox->x1 + dx), (short)(pbox->y1 + dy));
	 SET_DESTSTP((short)(pbox->x1), (short)(pbox->y1));
         SET_AXIS_PCNT((short)(pbox->x2 - pbox->x1 - 1), \
	 				(short)(pbox->y2 - pbox->y1 - 1));
	 SET_CMD(CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
      }
   } else if (direction == INC_X) {
      for (i = 0; i < nbox; i++) {
	 pbox = &pboxOrig[ordering[i]];

	 WaitQueue(7);
	 SET_CURPT((short)(pbox->x1 + dx), (short)(pbox->y2 + dy - 1));
	 SET_DESTSTP((short)(pbox->x1), (short)(pbox->y2 - 1));
         SET_AXIS_PCNT((short)(pbox->x2 - pbox->x1 - 1), \
	 				(short)(pbox->y2 - pbox->y1 - 1));
	 SET_CMD(CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
      }
   } else if (direction == INC_Y) {
      for (i = 0; i < nbox; i++) {
	 pbox = &pboxOrig[ordering[i]];

	 WaitQueue(7);
	 SET_CURPT((short)(pbox->x2 + dx - 1), (short)(pbox->y1 + dy));
	 SET_DESTSTP((short)(pbox->x2 - 1), (short)(pbox->y1));
         SET_AXIS_PCNT((short)(pbox->x2 - pbox->x1 - 1), \
	 				(short)(pbox->y2 - pbox->y1 - 1));
	 SET_CMD(CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
      }
   } else {
      for (i = 0; i < nbox; i++) {
	 pbox = &pboxOrig[ordering[i]];

	 WaitQueue(7);
	 SET_CURPT((short)(pbox->x2 + dx - 1), (short)(pbox->y2 + dy - 1));
	 SET_DESTSTP((short)(pbox->x2 - 1), (short)(pbox->y2 - 1));
         SET_AXIS_PCNT((short)(pbox->x2 - pbox->x1 - 1), \
	 				(short)(pbox->y2 - pbox->y1 - 1));
	 SET_CMD(CMD_BITBLT | direction | DRAW | PLANAR | WRTDATA);
      }
   }

   WaitQueue(2);
   SET_MIX(FSS_FRGDCOL | MIX_SRC, BSS_BKGDCOL | MIX_SRC);
   UNBLOCK_CURSOR;

   (*pWin->drawable.pScreen->RegionDestroy) (prgnDst);
   DEALLOCATE_LOCAL(ordering);
}

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3bstor.c,v 3.2 1996/12/27 07:02:24 dawes Exp $ */
/*-
 * s3bstore.c --
 *	Functions required by the backing-store implementation in MI.
 *
 * Copyright (c) 1987 by the Regents of the University of California
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 *
 * KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Modified by Amancio Hasty and Jon Tombs
 *
 */
/* $XConsortium: s3bstor.c /main/2 1996/10/25 11:34:20 kaleb $ */


#include    "cfb.h"
#include    "cfb16.h"
#include    "cfb24.h"
#include    "cfb32.h"
#include    "X.h"
#include    "mibstore.h"
#include    "regionstr.h"
#include    "scrnintstr.h"
#include    "pixmapstr.h"
#include    "windowstr.h"
#include    "s3v.h"

void
s3SaveAreas(pPixmap, prgnSave, xorg, yorg, pWin)
     PixmapPtr pPixmap;		/* Backing pixmap */
     RegionPtr prgnSave;	/* Region to save (pixmap-relative) */
     int   xorg;		/* X origin of region */
     int   yorg;		/* Y origin of region */
     WindowPtr pWin;
{
   register BoxPtr pBox;
   register int i;
   int   pixWidth;

   if (!xf86VTSema)
   {
	switch (pPixmap->drawable.bitsPerPixel) {
	case 8:
	    cfbSaveAreas(pPixmap, prgnSave, xorg, yorg, pWin);
	    return;
	case 16:
	    cfb16SaveAreas(pPixmap, prgnSave, xorg, yorg, pWin);
	    return;
	case 24:
	    cfb24SaveAreas(pPixmap, prgnSave, xorg, yorg, pWin);
	    return;
	case 32:
	    cfb32SaveAreas(pPixmap, prgnSave, xorg, yorg, pWin);
	    return;
	}
   }

   i = REGION_NUM_RECTS(prgnSave);
   pBox = REGION_RECTS(prgnSave);

   pixWidth = PixmapBytePad(pPixmap->drawable.width, pPixmap->drawable.depth);

   while (i--) {
      (s3ImageReadFunc) (pBox->x1 + xorg, pBox->y1 + yorg,
			 pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
			 pPixmap->devPrivate.ptr, pixWidth,
			 pBox->x1, pBox->y1, ~0);
      pBox++;
   }
}

void
s3RestoreAreas(pPixmap, prgnRestore, xorg, yorg, pWin)
     PixmapPtr pPixmap;		/* Backing pixmap */
     RegionPtr prgnRestore;	/* Region to restore (screen-relative) */
     int   xorg;		/* X origin of window */
     int   yorg;		/* Y origin of window */
     WindowPtr pWin;
{
   register BoxPtr pBox;
   register int i;
   int   pixWidth;

   if (!xf86VTSema)
   {
	switch (pPixmap->drawable.bitsPerPixel) {
	case 8:
	    cfbRestoreAreas(pPixmap, prgnRestore, xorg, yorg, pWin);
	    return;
	case 16:
	    cfb16RestoreAreas(pPixmap, prgnRestore, xorg, yorg, pWin);
	    return;
	case 24:
	    cfb24RestoreAreas(pPixmap, prgnRestore, xorg, yorg, pWin);
	    return;
	 case 32:
	    cfb32RestoreAreas(pPixmap, prgnRestore, xorg, yorg, pWin);
	    return;
	}
   }

   i = REGION_NUM_RECTS(prgnRestore);
   pBox = REGION_RECTS(prgnRestore);

   pixWidth = PixmapBytePad(pPixmap->drawable.width, pPixmap->drawable.depth);

   while (i--) {
      (s3ImageWriteFunc) (pBox->x1, pBox->y1,
			  pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
			  pPixmap->devPrivate.ptr, pixWidth,
			  pBox->x1 - xorg, pBox->y1 - yorg,
			  s3alu[GXcopy], ~0);
      pBox++;
   }
}

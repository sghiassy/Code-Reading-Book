/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxBStor.c,v 3.5 1996/12/23 06:32:30 dawes Exp $ */
/*-
 * agxbstore.c --
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
 * Modified for the AGX by Henry A. Worth
 *
 * KEVIN E. MARTIN AND HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN 
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR 
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS 
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/* $XConsortium: agxBStor.c /main/4 1996/02/21 17:16:29 kaleb $ */

/*
 * Modified by Amancio Hasty and Jon Tombs
 * 
 * Id: s3bstor.c,v 2.2 1993/06/22 20:54:09 jon Exp jon
 */


#include    "cfb.h"
#include    "X.h"
#include    "mibstore.h"
#include    "regionstr.h"
#include    "scrnintstr.h"
#include    "pixmapstr.h"
#include    "windowstr.h"
#include    "agx.h"

void
agxSaveAreas(pPixmap, prgnSave, xorg, yorg, pWin)
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
#ifdef AGX_32BPP
        case 32:
            cfb32SaveAreas(pPixmap, prgnSave, xorg, yorg, pWin);
            return;
#endif
      }
   }

   i = REGION_NUM_RECTS(prgnSave);
   pBox = REGION_RECTS(prgnSave);

   pixWidth = PixmapBytePad(pPixmap->drawable.width, pPixmap->drawable.depth);

   while (i--) {
      (agxImageReadFunc) ( pBox->x1 + xorg, pBox->y1 + yorg,
	  		   pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
			   pPixmap->devPrivate.ptr, pixWidth,
			   pBox->x1, pBox->y1, 0xFF );
      pBox++;
   }
}

void
agxRestoreAreas(pPixmap, prgnRestore, xorg, yorg, pWin)
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
#ifdef AGX_32BPP
         case 32:
            cfb32RestoreAreas(pPixmap, prgnRestore, xorg, yorg, pWin);
            return;
#endif
      }
   }

   i = REGION_NUM_RECTS(prgnRestore);
   pBox = REGION_RECTS(prgnRestore);

   pixWidth = PixmapBytePad(pPixmap->drawable.width, pPixmap->drawable.depth);

   while (i--) {
      (agxImageWriteFunc) ( pBox->x1, pBox->y1,
			    pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
			    pPixmap->devPrivate.ptr, pixWidth,
			    pBox->x1 - xorg, pBox->y1 - yorg,
			    GXcopy, 0xffffffff );
      pBox++;
   }
}

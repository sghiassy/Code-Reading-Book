/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/mach32bstor.c,v 3.5 1996/12/23 06:38:24 dawes Exp $ */
/*-
 * mach32bstore.c --
 *	Functions required by the backing-store implementation in MI.
 *
 * Copyright (c) 1987 by the Regents of the University of California
 * Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 */
/* $XConsortium: mach32bstor.c /main/4 1996/02/21 17:26:13 kaleb $ */


#include    "cfb.h"
#include    "cfb16.h"
#include    "X.h"
#include    "mibstore.h"
#include    "regionstr.h"
#include    "scrnintstr.h"
#include    "pixmapstr.h"
#include    "windowstr.h"
#include    "mach32.h"

void
mach32SaveAreas(pPixmap, prgnSave, xorg, yorg, pWin)
    PixmapPtr	  	pPixmap;  	/* Backing pixmap */
    RegionPtr	  	prgnSave; 	/* Region to save (pixmap-relative) */
    int	    	  	xorg;	    	/* X origin of region */
    int	    	  	yorg;	    	/* Y origin of region */
    WindowPtr		pWin;
{
    register BoxPtr	pBox;
    register int	i;
    int			pixWidth;

    if (!xf86VTSema)
    {
	switch (pPixmap->drawable.bitsPerPixel) {
	case 8:
	    cfbSaveAreas(pPixmap, prgnSave, xorg, yorg, pWin);
	    return;
	case 16:
	    cfb16SaveAreas(pPixmap, prgnSave, xorg, yorg, pWin);
	    return;
	}
    }

    i = REGION_NUM_RECTS(prgnSave);
    pBox = REGION_RECTS(prgnSave);

    pixWidth = PixmapBytePad(pPixmap->drawable.width, pPixmap->drawable.depth);

    while (i--) {
	(mach32ImageReadFunc)(pBox->x1 + xorg, pBox->y1 + yorg,
			 pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
			 pPixmap->devPrivate.ptr, pixWidth,
			 pBox->x1, pBox->y1, ~0);
	pBox++;
    }

    WaitIdleEmpty(); /* Make sure that all commands have finished */
}

void
mach32RestoreAreas(pPixmap, prgnRestore, xorg, yorg, pWin)
    PixmapPtr	  	pPixmap;  	/* Backing pixmap */
    RegionPtr	  	prgnRestore; 	/* Region to restore (screen-relative)*/
    int	    	  	xorg;	    	/* X origin of window */
    int	    	  	yorg;	    	/* Y origin of window */
    WindowPtr		pWin;
{
    register BoxPtr	pBox;
    register int	i;
    int			pixWidth;

    if (!xf86VTSema)
    {
	switch (pPixmap->drawable.bitsPerPixel) {
	case 8:
	    cfbRestoreAreas(pPixmap, prgnRestore, xorg, yorg, pWin);
	    return;
	case 16:
	    cfb16RestoreAreas(pPixmap, prgnRestore, xorg, yorg, pWin);
	    return;
	}
    }

    i = REGION_NUM_RECTS(prgnRestore);
    pBox = REGION_RECTS(prgnRestore);

    pixWidth = PixmapBytePad(pPixmap->drawable.width, pPixmap->drawable.depth);

    while (i--) {
	(mach32ImageWriteFunc)(pBox->x1, pBox->y1,
			  pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
			  pPixmap->devPrivate.ptr, pixWidth,
			  pBox->x1 - xorg, pBox->y1 - yorg,
			  mach32alu[GXcopy], 0xffffffff);
	pBox++;
    }

    WaitIdleEmpty(); /* Make sure that all commands have finished */
}

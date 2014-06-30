/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/ibm8514/bstor.c,v 3.1 1996/12/23 06:37:41 dawes Exp $ */
/*-
 * ibm8514bstore.c --
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
 * KEVIN E. MARTIN AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL KEVIN E. MARTIN OR TIAGO GONS BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified by Tiago Gons (tiago@comosjn.hobby.nl)
 */
/* $XConsortium: bstor.c /main/3 1996/02/21 17:23:53 kaleb $ */


#include    "cfb.h"
#include    "X.h"
#include    "mibstore.h"
#include    "regionstr.h"
#include    "scrnintstr.h"
#include    "pixmapstr.h"
#include    "windowstr.h"
#include    "ibm8514.h"

void
ibm8514SaveAreas(pPixmap, prgnSave, xorg, yorg, pWin)
    PixmapPtr	  	pPixmap;  	/* Backing pixmap */
    RegionPtr	  	prgnSave; 	/* Region to save (pixmap-relative) */
    int	    	  	xorg;	    	/* X origin of region */
    int	    	  	yorg;	    	/* Y origin of region */
    WindowPtr	  	pWin;
{
    register BoxPtr	pBox;
    register int	i;
    int			pixWidth;

/* 4-5-93 TCG : is VT visible */
    if (!xf86VTSema)
    {
	cfbSaveAreas(pPixmap, prgnSave, xorg, yorg, pWin);
	return;
    }

    
    i = REGION_NUM_RECTS(prgnSave);
    pBox = REGION_RECTS(prgnSave);

    pixWidth = PixmapBytePad(pPixmap->drawable.width, pPixmap->drawable.depth);

    while (i--) {
	ibm8514ImageRead(pBox->x1 + xorg, pBox->y1 + yorg,
			 pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
			 pPixmap->devPrivate.ptr, pixWidth,
			 pBox->x1, pBox->y1, 0xff);
	pBox++;
    }
}

void
ibm8514RestoreAreas(pPixmap, prgnRestore, xorg, yorg, pWin)
    PixmapPtr	  	pPixmap;  	/* Backing pixmap */
    RegionPtr	  	prgnRestore; 	/* Region to restore (screen-relative)*/
    int	    	  	xorg;	    	/* X origin of window */
    int	    	  	yorg;	    	/* Y origin of window */
    WindowPtr	  	pWin;
{
    register BoxPtr	pBox;
    register int	i;
    int			pixWidth;
    
/* 4-5-93 TCG : is VT visible */
    if (!xf86VTSema)
    {
	cfbRestoreAreas(pPixmap, prgnRestore, xorg, yorg, pWin);
	return;
    }

    i = REGION_NUM_RECTS(prgnRestore);
    pBox = REGION_RECTS(prgnRestore);

    pixWidth = PixmapBytePad(pPixmap->drawable.width, pPixmap->drawable.depth);

    while (i--) {
	ibm8514ImageWrite(pBox->x1, pBox->y1,
			  pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
			  pPixmap->devPrivate.ptr, pixWidth,
			  pBox->x1 - xorg, pBox->y1 - yorg,
			  ibm8514alu[GXcopy], 0xffffffff);
	pBox++;
    }
}

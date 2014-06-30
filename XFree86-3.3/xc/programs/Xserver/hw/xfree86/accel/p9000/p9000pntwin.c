/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000pntwin.c,v 3.2 1996/12/23 06:40:50 dawes Exp $ */
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
Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
Modified for the AGX by Henry A. Worth
Modified for the P9000 by Henrik Harmsen (harmsen@eritel.se)

DIGITAL, KEVIN E. MARTIN, HENRY A. WORTH AND HENRIK HARMSEN DISCLAIM
ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES
OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

*/
/* $XConsortium: p9000pntwin.c /main/3 1996/02/21 17:32:59 kaleb $ */

#include "X.h"

#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "cfb.h"
#include "mi.h"

#include "p9000reg.h"
#include "p9000.h"

extern void miPaintWindow();

static __inline__ void p9000FillBoxSolid ();
static __inline__ void p900016FillBoxSolid ();
static __inline__ void p900032FillBoxSolid ();

/* 8 bit case: */
void
p9000PaintWindow(pWin, pRegion, what)
    WindowPtr	pWin;
    RegionPtr	pRegion;
    int		what;
{

    if (!xf86VTSema)
    {
       cfbPaintWindow( pWin, pRegion, what );
       return;
    }

    switch (what) {
    case PW_BACKGROUND:
	switch (pWin->backgroundState) {
	case None:
	    return;
	case ParentRelative:
	    do {
		pWin = pWin->parent;
	    } while (pWin->backgroundState == ParentRelative);
	    (*pWin->drawable.pScreen->
                   PaintWindowBackground)(pWin, pRegion, what);
	    return;
	case BackgroundPixel:
	    p9000FillBoxSolid((DrawablePtr)pWin,
			      (int)REGION_NUM_RECTS(pRegion),
			      REGION_RECTS(pRegion),
			      pWin->background.pixel );
	    return;
    	}
    	break;
    case PW_BORDER:
	if (pWin->borderIsPixel)
	{
	    p9000FillBoxSolid((DrawablePtr)pWin,
			      (int)REGION_NUM_RECTS(pRegion),
			      REGION_RECTS(pRegion),
			      pWin->border.pixel );
	    return;
	}
	break;
    }
    /* Things we don't handle end up here */
    cfbPaintWindow (pWin, pRegion, what);
}

/* 16 bit case: */
void
p900016PaintWindow(pWin, pRegion, what)
    WindowPtr	pWin;
    RegionPtr	pRegion;
    int		what;
{

    if (!xf86VTSema)
    {
       cfb16PaintWindow( pWin, pRegion, what );
       return;
    }

    switch (what) {
    case PW_BACKGROUND:
	switch (pWin->backgroundState) {
	case None:
	    return;
	case ParentRelative:
	    do {
		pWin = pWin->parent;
	    } while (pWin->backgroundState == ParentRelative);
	    (*pWin->drawable.pScreen->
                   PaintWindowBackground)(pWin, pRegion, what);
	    return;
	case BackgroundPixel:
	    p900016FillBoxSolid((DrawablePtr)pWin,
			      (int)REGION_NUM_RECTS(pRegion),
			      REGION_RECTS(pRegion),
			      pWin->background.pixel );
	    return;
    	}
    	break;
    case PW_BORDER:
	if (pWin->borderIsPixel)
	{
	    p900016FillBoxSolid((DrawablePtr)pWin,
			      (int)REGION_NUM_RECTS(pRegion),
			      REGION_RECTS(pRegion),
			      pWin->border.pixel );
	    return;
	}
	break;
    }
    /* Things we don't handle end up here */
    cfb16PaintWindow (pWin, pRegion, what);
}


/* 32 bit case: */
void
p900032PaintWindow(pWin, pRegion, what)
    WindowPtr	pWin;
    RegionPtr	pRegion;
    int		what;
{

    if (!xf86VTSema)
    {
       cfb32PaintWindow( pWin, pRegion, what );
       return;
    }

    switch (what) {
    case PW_BACKGROUND:
	switch (pWin->backgroundState) {
	case None:
	    return;
	case ParentRelative:
	    do {
		pWin = pWin->parent;
	    } while (pWin->backgroundState == ParentRelative);
	    (*pWin->drawable.pScreen->
                   PaintWindowBackground)(pWin, pRegion, what);
	    return;
	case BackgroundPixel:
	    p900032FillBoxSolid((DrawablePtr)pWin,
			      (int)REGION_NUM_RECTS(pRegion),
			      REGION_RECTS(pRegion),
			      pWin->background.pixel );
	    return;
    	}
    	break;
    case PW_BORDER:
	if (pWin->borderIsPixel)
	{
	    p900032FillBoxSolid((DrawablePtr)pWin,
			      (int)REGION_NUM_RECTS(pRegion),
			      REGION_RECTS(pRegion),
			      pWin->border.pixel );
	    return;
	}
	break;
    }
    /* Things we don't handle end up here */
    cfb32PaintWindow (pWin, pRegion, what);
}



/* Function below implemented with a Quad operation */
static __inline__ void
p9000FillBoxSolid (pDrawable, nBox, pBox, pixel)
    DrawablePtr	    pDrawable;
    int		    nBox;
    BoxPtr	    pBox;
    unsigned long   pixel;
{
    register short w,h;

    p9000NotBusy();
    p9000Store(PMASK, CtlBase, 0xFF);
    p9000Store(RASTER, CtlBase, IGM_F_MASK);
    p9000Store(FGROUND, CtlBase, pixel); 

    for (; nBox; nBox--, pBox++) {
	h = pBox->y2 - pBox->y1;
	w = pBox->x2 - pBox->x1;

        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, CtlBase, (pBox->x1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0, CtlBase, (pBox->y1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1, CtlBase, (pBox->x1 + w));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1, CtlBase, (pBox->y1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2, CtlBase, (pBox->x1 + w));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2, CtlBase, (pBox->y1 + h));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3, CtlBase, (pBox->x1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3, CtlBase, (pBox->y1 + h));

        while(p9000Fetch(CMD_QUAD, CtlBase) & SR_ISSUE_QBN);
    }
    p9000QBNotBusy();
}


#if 0
/* Function below implemented with bitblits, which turned out to
   be not as fast as a regular quad */
static __inline__ void
p9000FillBoxSolid (pDrawable, nBox, pBox, pixel)
    DrawablePtr	    pDrawable;
    int		    nBox;
    BoxPtr	    pBox;
    unsigned long   pixel;
{
    register short w,h;

    p9000NotBusy();
    p9000Store(PMASK, CtlBase, 0xFF);
    p9000Store(RASTER, CtlBase, IGM_F_MASK);
    p9000Store(FGROUND, CtlBase, pixel); 

    for (; nBox; nBox--, pBox++) {
	h = pBox->y2 - pBox->y1 - 1;
	w = pBox->x2 - pBox->x1 - 1;

        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, CtlBase, (pBox->x1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0, CtlBase, (pBox->y1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1, CtlBase, (pBox->x1 + w));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1, CtlBase, (pBox->y1 + h));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2, CtlBase, (pBox->x1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2, CtlBase, (pBox->y1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3, CtlBase, (pBox->x1 + w));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3, CtlBase, (pBox->y1 + h));

        while(p9000Fetch(CMD_BLIT, CtlBase) & SR_ISSUE_QBN);
    }
    p9000QBNotBusy();
}
#endif

static __inline__ void
p900016FillBoxSolid (pDrawable, nBox, pBox, pixel)
    DrawablePtr	    pDrawable;
    int		    nBox;
    BoxPtr	    pBox;
    unsigned long   pixel;
{
    register short w,h;
    int i;

    p9000NotBusy();
    p9000Store(PMASK, CtlBase, 0xFF);
    p9000Store(PAT_ORIGINX, CtlBase, 0);
    p9000Store(PAT_ORIGINY, CtlBase, 0);
    p9000Store(BGROUND, CtlBase, pixel & 0xFF); 
    p9000Store(FGROUND, CtlBase, (pixel & 0xFF00) >> 8); 
    p9000Store(RASTER, CtlBase, (IGM_S_MASK & IGM_B_MASK)
                              | (~IGM_S_MASK & IGM_F_MASK)
                              | USE_PATTERN);

    for (i = 0; i < 32; i += 4) {
      p9000Store((PATTERN01 + i), CtlBase, 0xAAAAAAAA);
    }

    for (; nBox; nBox--, pBox++) {
	h = pBox->y2 - pBox->y1;
	w = pBox->x2 - pBox->x1;

        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, CtlBase, (pBox->x1) << 1);
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0, CtlBase, (pBox->y1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1, CtlBase, ((pBox->x1 + w) << 1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1, CtlBase, (pBox->y1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2, CtlBase, ((pBox->x1 + w) << 1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2, CtlBase, (pBox->y1 + h));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3, CtlBase, (pBox->x1) << 1);
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3, CtlBase, (pBox->y1 + h));

        while(p9000Fetch(CMD_QUAD, CtlBase) & SR_ISSUE_QBN);
    }
    p9000QBNotBusy();
}


static __inline__ void
p900032FillBoxSolid (pDrawable, nBox, pBox, pixel)
    DrawablePtr	    pDrawable;
    int		    nBox;
    BoxPtr	    pBox;
    unsigned long   pixel;
{
    register short w,h;
    int i;
    int nBoxSave;
    BoxPtr pBoxSave;
    
    nBoxSave = nBox; 
    pBoxSave = pBox;
    

    p9000NotBusy();
    p9000Store(PMASK, CtlBase, 0xFF);
    p9000Store(PAT_ORIGINX, CtlBase, 0);
    p9000Store(PAT_ORIGINY, CtlBase, 0);
    p9000Store(BGROUND, CtlBase, (pixel & 0xFF00) >> 8); 
    p9000Store(FGROUND, CtlBase, (pixel & 0xFF0000) >> 16);
    p9000Store(RASTER, CtlBase, (IGM_S_MASK & IGM_B_MASK)
                              | (~IGM_S_MASK & IGM_F_MASK)
                              | USE_PATTERN);

    for (i = 0; i < 32; i += 4) {
      p9000Store((PATTERN01 + i), CtlBase, 0x55555555);
    }

    for (; nBox; nBox--, pBox++) {
	h = pBox->y2 - pBox->y1;
	w = pBox->x2 - pBox->x1;

        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, CtlBase, (pBox->x1) << 2);
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0, CtlBase, (pBox->y1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1, CtlBase, ((pBox->x1 + w) << 2));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1, CtlBase, (pBox->y1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2, CtlBase, ((pBox->x1 + w) << 2));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2, CtlBase, (pBox->y1 + h));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3, CtlBase, (pBox->x1) << 2);
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3, CtlBase, (pBox->y1 + h));

        while(p9000Fetch(CMD_QUAD, CtlBase) & SR_ISSUE_QBN);
    }
    p9000QBNotBusy();

    p9000NotBusy();
    p9000Store(PMASK, CtlBase, 0xFF);
    p9000Store(PAT_ORIGINX, CtlBase, 0);
    p9000Store(PAT_ORIGINY, CtlBase, 0);
    p9000Store(FGROUND, CtlBase, (pixel & 0xFF)); 
    p9000Store(RASTER, CtlBase, (IGM_S_MASK & IGM_F_MASK)
                              | (~IGM_S_MASK & IGM_D_MASK)
                              | USE_PATTERN);

    for (i = 0; i < 32; i += 4) {
      p9000Store((PATTERN01 + i), CtlBase, 0x88888888);
    }

    nBox = nBoxSave;
    pBox = pBoxSave;

    for (; nBox; nBox--, pBox++) {
	h = pBox->y2 - pBox->y1;
	w = pBox->x2 - pBox->x1;

        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_0, CtlBase, (pBox->x1) << 2);
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_0, CtlBase, (pBox->y1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_1, CtlBase, ((pBox->x1 + w) << 2));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_1, CtlBase, (pBox->y1));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_2, CtlBase, ((pBox->x1 + w) << 2));
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_2, CtlBase, (pBox->y1 + h));
        p9000Store(DEVICE_COORD | DC_ABS | DC_X | DC_3, CtlBase, (pBox->x1) << 2);
        p9000Store(DEVICE_COORD | DC_ABS | DC_Y | DC_3, CtlBase, (pBox->y1 + h));

        while(p9000Fetch(CMD_QUAD, CtlBase) & SR_ISSUE_QBN);
    }
    p9000QBNotBusy();

}



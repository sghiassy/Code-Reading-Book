/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000line.c,v 3.3 1996/12/23 06:40:47 dawes Exp $ */
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

DIGITAL, KEVIN E. MARTIN, RICKARD E. FAITH, CRAIG E. GROESCHEL AND
CHRIS MASON DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL DIGITAL OR KEVIN E. MARTIN OR RICKARD E. FAITH OR CRAIG
E. GROESCHEL OR CHRIS MASON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
mach32Line1Rect by Craig E. Groeschel (craig@adikia.sccsi.com),
  based on an ATI preclipping code example
Modfied for the p9000 by Chris Mason (mason@mail.csh.rit.edu)

*/
/* $XConsortium: p9000line.c /main/3 1996/02/21 17:32:48 kaleb $ */

#include "X.h"

#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "miline.h"
#include "mistruct.h"

#include "cfb.h"
#include "cfbmskbits.h"
#include "p9000.h"
#include "p9000reg.h"

#ifdef MINSHORT
#undef MINSHORT
#undef MAXSHORT
#endif

/*
	The p9000 considers a line a very skinny rectangle.  This code 
	works a little different than the mach32/s3/8514.  For each set of
	endpoints, the coord regs are loaded once.  For each clipping region,
	the clipping regs are loaded and a quad is drawn.  
        Software clipping would end up faster for more than one clipping 
	region...
*/

void
p9000Line(pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		mode;		/* Origin or Previous */
    int		npt;		/* number of points */
    DDXPointPtr pptInit;
{
    int nboxInit;
    register int nbox;
    BoxPtr pboxInit;
    register BoxPtr      pbox;
    register DDXPointPtr ppt;	/* pointer to list of translated points */
    int xorg, yorg;		/* origin of window */
    register int y1,  y2;
    register int x1,  x2;
    RegionPtr    cclip;
    cfbPrivGCPtr    devPriv;
    int 	    old_w_min, old_w_max ; /* prev values of clipping regs */
    int 	    oldpixel ;        /* saves last point for line capping */

    devPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr); 
    cclip = devPriv->pCompositeClip;

    /* if the drawable is off screen, let the cfb code do it */
    if (!OnScreenDrawable(pDrawable->type) || !xf86VTSema) {
        cfbLineSS(pDrawable, pGC, mode, npt, pptInit) ;
        return ;
    }

    pboxInit = REGION_RECTS(cclip);
    nboxInit = REGION_NUM_RECTS(cclip);

    p9000NotBusy() ;
    old_w_min = p9000Fetch(W_MIN,CtlBase) ;
    old_w_max = p9000Fetch(W_MAX,CtlBase) ;
/*
  p9000 does not properly update the meta coord index sometimes, so we must
  make sure it is right before hand
*/
    p9000Store(CINDEX,CtlBase,0) ;
    p9000Store(PMASK,CtlBase,pGC->planemask) ;
    p9000Store(RASTER,CtlBase,IGM_OVERSIZED | p9000QuadAlu[pGC->alu])  ;
    p9000Store(FGROUND,CtlBase,pGC->fgPixel) ;
    p9000Store(BGROUND,CtlBase,pGC->bgPixel) ;
    p9000Store(W_OFFSET_XY,CtlBase,(int)0) ;

    xorg = pDrawable->x; 
    yorg = pDrawable->y ;

    ppt = pptInit;
    x2 = ppt->x + xorg ;
    y2 = ppt->y + yorg ;
    while(--npt)
    {
	nbox = nboxInit ;
	pbox = pboxInit ;

	x1 = x2 ;
	y1 = y2 ;
	++ppt;
	if (mode == CoordModePrevious) 
	{
	    xorg = x1 ;
	    yorg = y1 ;
	}
	x2 = ppt->x + xorg;
	y2 = ppt->y + yorg;

        if ( (y1 < 0) && (y2 < 0) )  /* whole line is clipped */
            continue ;

	/* save pixel value of last point for CapNotLast */
        if ((pGC->capStyle == CapNotLast) && (npt == 1) && (x2 >= 0)) {
	    p9000QBNotBusy() ;
	    oldpixel = p9000Fetch(p9000InfoRec.virtualX*y2*p9000BytesPerPixel+
	    			  p9000BytesPerPixel * x2, VidBase) ;
        }

	/* don't use YX_PACK cause y's might be negative */
	p9000Store(META_COORD | MC_LINE | MC_X,CtlBase,x1) ;
	p9000Store(META_COORD | MC_LINE | MC_Y,CtlBase,y1) ;
	p9000Store(META_COORD | MC_LINE | MC_X,CtlBase,x2) ;
	p9000Store(META_COORD | MC_LINE | MC_Y,CtlBase,y2) ;

        while(nbox--) {
	    p9000NotBusy() ;
	    /* don't think I can get a negative clipping region? */
	    p9000Store(W_MIN, CtlBase, YX_PACK(pbox->x1,pbox->y1)) ;
	    p9000Store(W_MAX, CtlBase, YX_PACK(pbox->x2-1,pbox->y2-1)) ;

	    /* if the whole line is outside clipping region don't draw it */
	    if ( !(p9000Fetch(STATUS, CtlBase) & SR_Q_HIDDEN)) {
	        while(p9000Fetch(CMD_QUAD,CtlBase) & SR_ISSUE_QBN) ;
	    }
	    pbox++ ;

	} /* while(nbox--) */
    } /* while(--npt) */

    p9000NotBusy() ; 
    p9000Store(W_MIN,CtlBase,old_w_min) ;
    p9000Store(W_MAX,CtlBase,old_w_max) ;
    p9000QBNotBusy() ;

    /* restore last point if CapNotLast */
    if ((pGC->capStyle == CapNotLast) && (x2 >= 0)) {
	int tmp ;
	tmp = p9000Fetch(p9000InfoRec.virtualX * y2 * p9000BytesPerPixel +
	    			  p9000BytesPerPixel * x2, VidBase) ;

	/* pick the right byte for this pixel */
	switch(x2 & 3) {
	case 0:
	    oldpixel &=       0x000000FFL ;
	    oldpixel |= tmp & 0xFFFFFF00L ;
	    break ;
	case 1:
	    oldpixel &=       0x0000FF00L ;
	    oldpixel |= tmp & 0xFFFF00FFL ;
	    break ;
	case 2:
	    oldpixel &=       0x00FF0000L ;
	    oldpixel |= tmp & 0xFF00FFFFL ;
	    break ;
	case 3:
	    oldpixel &=       0xFF000000L ;
	    oldpixel |= tmp & 0x00FFFFFFL ;
	    break ;
	}
        p9000Store(p9000InfoRec.virtualX * y2 * p9000BytesPerPixel +
	    	   p9000BytesPerPixel * x2, VidBase, oldpixel) ;
    }
} 

/*
** same as p9000Line, but only meant for use when there is 1 clipping region.
** if you call it with more than 1 clipping region than  p9000line gets
** called.
*/
void
p9000Line1Rect (pDrawable, pGC, mode, npt, pptInit)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         mode;
    int         npt;
    DDXPointPtr pptInit;
{
    register BoxPtr pbox;
    register DDXPointPtr ppt;	/* pointer to list of translated points */

    int xorg, yorg;		/* origin of window */
    register int y1, y2;
    register int x1, x2;
    int oldpixel ;	  /* saves value of last point for line capping */
    int old_w_min, old_w_max ; /* previous values of clipping regs */

    RegionPtr cclip;
    cfbPrivGCPtr    devPriv;

    /* if the drawable is off screen, let the cfb code do it */
    if (!OnScreenDrawable(pDrawable->type) || !xf86VTSema) {
        cfbLineSS(pDrawable, pGC, mode, npt, pptInit) ;
        return ;
    }

    devPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr); 
    cclip = devPriv->pCompositeClip;

    if (REGION_NUM_RECTS(cclip) != 1) {
        p9000Line(pDrawable, pGC, mode, npt, pptInit);
        return ;
    }

    pbox = REGION_RECTS(cclip);

    p9000NotBusy() ;
    old_w_min = p9000Fetch(W_MIN,CtlBase) ;
    old_w_max = p9000Fetch(W_MAX,CtlBase) ;
/*
  p9000 does not properly update the meta coord index sometimes, so we must
  make sure it is right before hand
*/
    p9000Store(CINDEX,CtlBase,0) ;
    p9000Store(PMASK,CtlBase,pGC->planemask) ;
    p9000Store(RASTER,CtlBase,IGM_OVERSIZED | p9000QuadAlu[pGC->alu])  ;
    p9000Store(FGROUND,CtlBase,pGC->fgPixel) ;
    p9000Store(BGROUND,CtlBase,pGC->bgPixel) ;
    p9000Store(W_OFFSET_XY,CtlBase,(int)0) ;

    p9000Store(W_MIN, CtlBase, YX_PACK(pbox->x1,pbox->y1)) ;
    p9000Store(W_MAX, CtlBase, YX_PACK(pbox->x2-1,pbox->y2-1)) ;

    xorg = pDrawable->x; 
    yorg = pDrawable->y ;

    ppt = pptInit;
    x2 = ppt->x + xorg ;
    y2 = ppt->y + yorg ;

    while(--npt)
    {
	x1 = x2 ;
	y1 = y2 ;
	++ppt;
	if (mode == CoordModePrevious) 
	{
	    xorg = x1 ;
	    yorg = y1 ;
	}

	x2 = ppt->x + xorg;
	y2 = ppt->y + yorg;

        if ( (y1 < 0) && (y2 < 0) )  /* whole line is clipped */
            continue ;

	/* save pixel value of last point for CapNotLast */
        if ((pGC->capStyle == CapNotLast) && (npt == 1) && (x2 >= 0)) {
	    p9000QBNotBusy() ;
	    oldpixel = p9000Fetch(p9000InfoRec.virtualX*y2*p9000BytesPerPixel+
	    			  p9000BytesPerPixel * x2, VidBase) ;
        }

	p9000Store(META_COORD | MC_LINE | MC_X, CtlBase, x1) ;
	p9000Store(META_COORD | MC_LINE | MC_Y, CtlBase, y1) ;
	p9000Store(META_COORD | MC_LINE | MC_X, CtlBase, x2) ;
	p9000Store(META_COORD | MC_LINE | MC_Y, CtlBase, y2) ;

	/* don't bother if the whole line is outside clipping box */
	if ( !(p9000Fetch(STATUS, CtlBase) & SR_Q_HIDDEN)) {
	        while(p9000Fetch(CMD_QUAD,CtlBase) & SR_ISSUE_QBN) ;
	}

    } /* while(--npt) */

    p9000NotBusy() ;  
    p9000Store(W_MIN,CtlBase,old_w_min) ;
    p9000Store(W_MAX,CtlBase,old_w_max) ;
    p9000QBNotBusy() ;

    /* restore last point if CapNotLast */
    if ((pGC->capStyle == CapNotLast) && (x2 >= 0)) {
	int tmp ;
	tmp = p9000Fetch(p9000InfoRec.virtualX * y2 * p9000BytesPerPixel +
	    			  p9000BytesPerPixel * x2, VidBase) ;

	/* pick the right byte for this pixel */
	switch(x2 & 3) {
	case 0:
	    oldpixel &=       0x000000FFL ;
	    oldpixel |= tmp & 0xFFFFFF00L ;
	    break ;
	case 1:
	    oldpixel &=       0x0000FF00L ;
	    oldpixel |= tmp & 0xFFFF00FFL ;
	    break ;
	case 2:
	    oldpixel &=       0x00FF0000L ;
	    oldpixel |= tmp & 0xFF00FFFFL ;
	    break ;
	case 3:
	    oldpixel &=       0xFF000000L ;
	    oldpixel |= tmp & 0x00FFFFFFL ;
	    break ;
	}
        p9000Store(p9000InfoRec.virtualX * y2 * p9000BytesPerPixel +
	    	   p9000BytesPerPixel * x2, VidBase, oldpixel) ;
    }
}

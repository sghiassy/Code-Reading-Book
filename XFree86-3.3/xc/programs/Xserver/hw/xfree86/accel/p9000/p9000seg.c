/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000seg.c,v 3.3 1996/12/23 06:40:54 dawes Exp $ */
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
/* $XConsortium: p9000seg.c /main/3 1996/02/21 17:33:11 kaleb $ */

#include "X.h"

#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "mistruct.h"
#include "miline.h"

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
	Here too, software clipping should be better for more than one clipping
	region.  This is not bad, but....
*/

void
p9000Segment(pDrawable, pGC, nseg, pSeg) 
    DrawablePtr pDrawable;
    GCPtr	pGC;
    int		nseg ;
    register xSegment *pSeg ;
{
    int nboxInit;
    register int nbox;
    BoxPtr pboxInit;
    register BoxPtr pbox;

    int xorg, yorg;		/* origin of window */
    register int y1, y2;
    register int x1, x2;
    RegionPtr cclip;
    cfbPrivGCPtr    devPriv;
    int old_w_min, old_w_max ;  /* saves previous clipping regs    */
    register int oldpixel ;	/* saves last pixel for capnotlast */

    /* if the drawable is off screen, let the cfb code do it */
    if (!OnScreenDrawable(pDrawable->type) || !xf86VTSema) {
        cfbSegmentSS(pDrawable, pGC, nseg, pSeg ) ;
        return ;
    }

    devPriv = (cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr); 
    cclip = devPriv->pCompositeClip;

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

    while(nseg--)
    {
	nbox = nboxInit ;
	pbox = pboxInit ;

	x1 = pSeg -> x1 + xorg ;
	x2 = pSeg -> x2 + xorg ;
	y2 = pSeg -> y2 + yorg ;
	y1 = pSeg -> y1 + yorg ;
	pSeg++ ;

        if ( (y1 < 0) && (y2 < 0) )  /* whole line is clipped */
            continue ;

	/* save pixel value of last point for CapNotLast */
        if ((pGC->capStyle == CapNotLast) && (x2 >= 0)) {
	    oldpixel = p9000Fetch(p9000InfoRec.virtualX*y2*p9000BytesPerPixel+
	    			  p9000BytesPerPixel * x2, VidBase) ;
        }

/*
 * we only have to load up the coord regs once for every line we want to
 * clip against.  Once they are loaded, just change the clipping box and
 * redraw the quad.    
 */
	p9000Store(META_COORD | MC_LINE | MC_X, CtlBase, x1) ;
	p9000Store(META_COORD | MC_LINE | MC_Y, CtlBase, y1) ;
	p9000Store(META_COORD | MC_LINE | MC_X, CtlBase, x2) ;
	p9000Store(META_COORD | MC_LINE | MC_Y, CtlBase, y2) ;

        while(nbox--) {
	    p9000NotBusy() ;
	    p9000Store(W_MIN, CtlBase, YX_PACK(pbox->x1,pbox->y1)) ;
	    p9000Store(W_MAX, CtlBase, YX_PACK(pbox->x2-1,pbox->y2-1)) ;
	    if (!(p9000Fetch(STATUS, CtlBase) & SR_Q_HIDDEN))
	        while(p9000Fetch(CMD_QUAD,CtlBase) & SR_ISSUE_QBN) ;
	    pbox++ ;

	} /* while(nbox--) */
        /* restore last point if CapNotLast */
        if ((pGC->capStyle == CapNotLast) && (x2 >= 0)) {
	    register int tmp ;
		
	    p9000QBNotBusy() ; 
	    tmp = p9000Fetch(p9000InfoRec.virtualX*y2* p9000BytesPerPixel +
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

    } /* while(--npt) */

    p9000NotBusy() ; 
    p9000Store(W_MIN,CtlBase,old_w_min) ;
    p9000Store(W_MAX,CtlBase,old_w_max) ;
    p9000QBNotBusy() ;

} 


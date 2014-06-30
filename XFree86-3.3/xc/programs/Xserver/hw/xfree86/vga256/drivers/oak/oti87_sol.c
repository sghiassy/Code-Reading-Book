/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/oak/oti87_sol.c,v 3.2 1996/12/23 06:57:59 dawes Exp $ */
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
*/
/* $XConsortium: oti87_sol.c /main/2 1996/02/21 18:06:22 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "xf86.h"
#include "vga256.h"
#include "vga.h"
#include "compiler.h"
#include "oak_driver.h"
#include "xf86_Config.h"

/*
 * Accelerated solid filled spans for 8bpp.
 */

void OAKFillSolidSpansGeneral(pDrawable, pGC, nInit, pptInit, pwidthInit,
fSorted)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
    int         fSorted;
{
    int n;                      /* number of spans to fill */
    register DDXPointPtr ppt;   /* pointer to list of start points */
    register int *pwidth;       /* pointer to list of n widths */
    DDXPointPtr initPpt;
    int *initPwidth;
    unsigned short width;
    int pitch, busy, pixshift;

    /* We only handle on-screen spans with full planemask. */


    if ( (!xf86VTSema || (pGC->planemask & 0xFF) != 0xFF)) 
      {
    	vga256SolidSpansGeneral(pDrawable, 
				pGC,
                                nInit, 
				pptInit, 
				pwidthInit, 
				fSorted);
        return;
      }
    

    if (pDrawable->type != DRAWABLE_WINDOW) 
      {
    	/* 
    	 * This would be handled better by writing decent ValidateGC
    	 * code for the SVGA server.
    	 */
	cfbSolidSpansGeneral(pDrawable, 
			     pGC, 
			     nInit, 
			     pptInit, 
			     pwidthInit, 
			     fSorted);
      }

    if (!(pGC->planemask)) /* Seems that this means the spans is 'cloaked' */
      {
	return;
      }



/*    ErrorF ("%s %s: oti087: Actually using my routine \n \n",
	    XCONFIG_GIVEN,
	    vga256InfoRec.name);
  */  
    vga256SolidSpansGeneral(pDrawable, 
			    pGC,
			    nInit, 
			    pptInit, 
			    pwidthInit, 
			    fSorted);
}








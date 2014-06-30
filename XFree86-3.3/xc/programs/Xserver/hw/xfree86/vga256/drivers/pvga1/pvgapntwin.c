/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/pvga1/pvgapntwin.c,v 3.6 1996/12/23 06:58:08 dawes Exp $ */
/***********************************************************

Copyright (c) 1987  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* $XConsortium: pvgapntwin.c /main/7 1996/02/21 18:06:49 kaleb $ */

/* WD90C31 code: Mike Tierney <floyd@eng.umd.edu> */
/* WD90C33 accel code: Bill Morgart <wsm@morticia.ssw.com> */

#include "X.h"

#include "windowstr.h"
#include "regionstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "cfb.h"
#include "cfbmskbits.h"
#include "vgaBank.h"

#include "compiler.h"
#include "xf86.h"
#include "vga.h"
#include "paradise.h"



void
pvgacfbFillBoxSolid (pDrawable, nBox, pBox, pixel1, pixel2, alu)
    DrawablePtr	    pDrawable;
    int		    nBox;
    BoxPtr	    pBox;
    unsigned long   pixel1;
    unsigned long   pixel2;
    int	            alu;
{
    unsigned char   *pdstBase;
    unsigned long   pdst;
    unsigned long    widthDst;
    unsigned long    h;
    unsigned long   fill1;
    unsigned long   w;

    if (pDrawable->type == DRAWABLE_WINDOW)
    {
	pdstBase = (unsigned char *)
	(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	widthDst = (int)
		  (((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind);
    }
    else
    {
	pdstBase = (unsigned char *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
	widthDst = (int)(((PixmapPtr)pDrawable)->devKind);
    }

    if (!CHECKSCREEN(pdstBase))
    {
        pvga1_stdcfbFillBoxSolid (pDrawable, nBox, pBox, pixel1, pixel2, alu);
        return;
    }

    fill1 = PFILL(pixel1);

    for (; nBox; nBox--, pBox++)
    {
      switch (WDchipset)
	{
	case WD90C31:
	case WD90C24:
	  wd90c31BitBlt((unsigned char *)NULL, pdstBase,
			0, widthDst,
			0, 0,
			pBox->x1, pBox->y1,
			pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
			1, 1,
			alu,
			0xff,
			BLT_SRC_FCOL,
			fill1);

	  break;
	  
	case WD90C33:
	  wd90c33BitBlt((unsigned char *)NULL, pdstBase,
			0, widthDst,
			0, 0,
			pBox->x1, pBox->y1,
			pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
			1, 1,
			alu,
			0xff,
			FIXED_COLOR,
			fill1);
	  break;
	}
    }

    if (WDchipset == WD90C31 || WDchipset == WD90C24)
      WAIT_BLIT; /* must wait since memory writes can mess up as well */
}

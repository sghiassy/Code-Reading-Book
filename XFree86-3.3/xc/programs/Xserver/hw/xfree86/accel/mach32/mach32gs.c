/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/mach32gs.c,v 3.5 1996/12/23 06:38:37 dawes Exp $ */
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

DIGITAL AND KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL DIGITAL OR KEVIN E. MARTIN
BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)

*/
/* $XConsortium: mach32gs.c /main/5 1996/02/21 17:26:52 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"

#include "misc.h"
#include "region.h"
#include "gc.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "cfb.h"
#include "cfb16.h"
#include "cfbmskbits.h"
#include "mach32.h"

/* GetSpans -- for each span, gets bits from drawable starting at ppt[i]
 * and continuing for pwidth[i] bits
 * Each scanline returned will be server scanline padded, i.e., it will come
 * out to an integral number of words.
 */
void
mach32GetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart)
    DrawablePtr		pDrawable;	/* drawable from which to get bits */
    int			wMax;		/* largest value of all *pwidths */
    register DDXPointPtr ppt;		/* points to start copying from */
    int			*pwidth;	/* list of number of bits to copy */
    int			nspans;		/* number of scanlines to copy */
    char		*pdstStart;	/* where to put the bits */
{
    int			j;
    char		*pdst; /* where to put the bits */
    int			pixmapStride;

    if (pDrawable->type != DRAWABLE_WINDOW) {
	switch (pDrawable->bitsPerPixel) {
	    case 1:
		mfbGetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart);
		break;
	    case 8:
		cfbGetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart);
		break;
	    case 16:
		cfb16GetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart);
		break;
	    default:
		ErrorF("mach32GetSpans: Unsupported pixmap depth\n");
		break;
	}
	return;
    }

    pixmapStride = PixmapBytePad(wMax, pDrawable->depth);
    pdst = pdstStart;

    for (; nspans--; ppt++, pwidth++) {
	(mach32ImageReadFunc)(ppt->x, ppt->y, j = *pwidth, 1, pdst,
				pixmapStride, 0, 0, ~0);
	j *= mach32InfoRec.bitsPerPixel / 8;
	pdst += j;		/* width is in 32 bit words */
	j = (-j) & 3;
	while (j--)		/* Pad out to 32-bit boundary */
	    *pdst++ = 0;
    }

    WaitIdleEmpty(); /* Make sure that all commands have finished */
}

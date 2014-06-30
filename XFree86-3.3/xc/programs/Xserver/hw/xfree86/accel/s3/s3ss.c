/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3ss.c,v 3.8 1996/12/23 06:42:07 dawes Exp $ */
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
All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL AND KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL DIGITAL OR KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

*/

/*
 * Modified by Amancio Hasty and Jon Tombs
 * 
 */
/* $XConsortium: s3ss.c /main/6 1996/10/19 17:57:38 kaleb $ */


#include "X.h"
#include "Xmd.h"
#include "servermd.h"

#include "misc.h"
#include "regionstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"

#include "cfb.h"
#include "cfb16.h"
#include "cfb24.h"
#include "cfb32.h"
#include "cfbmskbits.h"

#include "s3.h"

/*
 * SetSpans -- for each span copy pwidth[i] bits from psrc to pDrawable at
 * ppt[i] using the raster op from the GC.  If fSorted is TRUE, the scanlines
 * are in increasing Y order. Source bit lines are server scanline padded so
 * that they always begin on a word boundary.
 */
void
s3SetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted)
     DrawablePtr pDrawable;
     GCPtr pGC;
     char *psrc;
     register DDXPointPtr ppt;
     int  *pwidth;
     int   nspans;
     int   fSorted;
{
   register BoxPtr pbox, pboxLast, pboxTest;
   register DDXPointPtr pptLast;
   int   alu;
   RegionPtr prgnDst;
   int   xStart, xEnd;
   int   yMax;

 /*
  * ErrorF("Set Spans: n = %d, (%d, %d, %d)\n", nspans, ppt->x, ppt->y,
  * *pwidth);
  */

   if (!xf86VTSema)
   {
      switch (s3InfoRec.bitsPerPixel) {
      case 8:
	 cfbSetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted);
	 break;
      case 16:
	 cfb16SetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted);
	 break;
      case 24:
	 cfb24SetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted);
	 break;
      case 32:
	 cfb32SetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted);
	 break;
      }
      return;
   }

   if (pDrawable->type != DRAWABLE_WINDOW) {
      switch (pDrawable->bitsPerPixel) {
	case 1:
	   mfbSetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted);
	   break;
	case 8:
	   cfbSetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted);
	   break;
	case 16:
	   cfb16SetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted);
	   break;
	case 24:
	   cfb24SetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted);
	   break;
        case 32:
	   cfb32SetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted);
	   break;
	default:
	   ErrorF("Unsupported pixmap depth\n");
	   break;
      }
      return;
   }
   alu = pGC->alu;
   prgnDst = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;

   pptLast = ppt + nspans;

   yMax = (int)pDrawable->y + (int)pDrawable->height;

   pbox = REGION_RECTS(prgnDst);
   pboxLast = pbox + REGION_NUM_RECTS(prgnDst);

   if (fSorted) {

    /*
     * scan lines sorted in ascending order. Because they are sorted, we
     * don't have to check each scanline against each clip box.  We can be
     * sure that this scanline only has to be clipped to boxes at or after
     * the beginning of this y-band
     */
      pboxTest = pbox;
      while (ppt < pptLast) {
	 pbox = pboxTest;
	 if (ppt->y >= yMax)
	    break;
	 while (pbox < pboxLast) {
	    if (pbox->y1 > ppt->y) {
	     /* scanline is before clip box */
	       break;
	    } else if (pbox->y2 <= ppt->y) {
	     /* clip box is before scanline */
	       pboxTest = ++pbox;
	       continue;
	    } else if (pbox->x1 > ppt->x + *pwidth) {
	     /* clip box is to right of scanline */
	       break;
	    } else if (pbox->x2 <= ppt->x) {
	     /* scanline is to right of clip box */
	       pbox++;
	       continue;
	    }
	  /* at least some of the scanline is in the current clip box */
	    xStart = max(pbox->x1, ppt->x);
	    xEnd = min(ppt->x + *pwidth, pbox->x2);
	  /***************
		cfbSetScanline(ppt->y, ppt->x, xStart, xEnd, psrc, alu,
		    pdstBase, widthDst, pGC->planemask);
***************/

	    (s3ImageWriteFunc) (xStart, ppt->y, xEnd - xStart, 1,
			      psrc + (xStart - ppt->x) * s3Bpp,
			      PixmapBytePad(xEnd-xStart, pDrawable->depth),
			      0, 0, s3alu[alu], pGC->planemask);

	    if (ppt->x + *pwidth <= pbox->x2) {
	     /* End of the line, as it were */
	       break;
	    } else
	       pbox++;
	 }

       /*
        * We've tried this line against every box; it must be outside them
        * all.  move on to the next point
        */
	 ppt++;
	 psrc += PixmapBytePad(*pwidth, PSZ);
	 pwidth++;
      }
   } else {
    /* scan lines not sorted. We must clip each line against all the boxes */
      while (ppt < pptLast) {
	 if (ppt->y >= 0 && ppt->y < yMax) {

	    for (pbox = REGION_RECTS(prgnDst); pbox < pboxLast; pbox++) {
	       if (pbox->y1 > ppt->y) {

		/*
		 * rest of clip region is above this scanline, skip it
		 */
		  break;
	       }
	       if (pbox->y2 <= ppt->y) {
		/* clip box is below scanline */
		  pbox++;
		  break;
	       }
	       if (pbox->x1 <= ppt->x + *pwidth &&
		   pbox->x2 > ppt->x) {
		  xStart = max(pbox->x1, ppt->x);
		  xEnd = min(pbox->x2, ppt->x + *pwidth);
		/**************
			cfbSetScanline(ppt->y, ppt->x, xStart, xEnd, psrc, alu,
			    pdstBase, widthDst, pGC->planemask);
**************/
		(s3ImageWriteFunc) (xStart, ppt->y, xEnd - xStart, 1,
			      psrc + (xStart - ppt->x) * s3Bpp,
			      PixmapBytePad(xEnd-xStart, pDrawable->depth),
			      0, 0, s3alu[alu], pGC->planemask);

	       }
	    }
	 }
	 psrc += PixmapBytePad(*pwidth, pDrawable->depth);
	 ppt++;
	 pwidth++;
      }
   }
}

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3frect.c,v 3.12 1996/12/23 06:41:44 dawes Exp $ */
/*

Copyright (c) 1989  X Consortium

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

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
 
*/

/*
 * Snarfed Hans Nasten's simple pixmap expansion cache from Mach-8 server
 * -- David Wexelblat <dwex@xfree86.org>, July 12, 1994
 */
/* $XConsortium: s3frect.c /main/9 1996/10/24 07:11:13 kaleb $ */


/*
 * Fill rectangles.
 */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "cfb.h"
#include "cfb16.h"
#include "cfb24.h"
#include "cfb32.h"
#include "cfbmskbits.h"
#include "mergerop.h"

#include "s3.h"
#include "regs3.h"

#define NUM_STACK_RECTS	1024
extern int s3MAX_SLOTS;

typedef struct _CacheInfo {
    int x;
    int y;
    int w;
    int h;
    int nx;
    int ny;
    int pix_w;
    int pix_h;
} CacheInfo, *CacheInfoPtr;

static CacheInfo cInfo;
static int pixmap_x;
static int pixmap_y;
static int pixmap_size = 0;

void s3InitFrect(x, y, size)
     int x;
     int y;
     int size;
{
   pixmap_x = x;
   pixmap_y = y;
   pixmap_size = size;
}

static void
DoCacheExpandPixmap(pci)
     CacheInfoPtr pci;
{
   int   cur_w = pci->pix_w;
   int   cur_h = pci->pix_h;

   BLOCK_CURSOR;
   WaitQueue16_32(7,8);
   SET_SCISSORS(0,0,s3DisplayWidth-1,s3ScissB);
   SET_MIX(FSS_BITBLT | MIX_SRC,BSS_BKGDCOL | MIX_SRC);
   SET_WRT_MASK(~0);

 /* Expand in the x direction */

   while (cur_w * 2 <= pci->w) {
      WaitQueue(7);
      SET_CURPT((short)pci->x,(short)pci->y);
      SET_DESTSTP((short)(pci->x + cur_w),(short)pci->y);
      SET_AXIS_PCNT((short)(cur_w - 1),(short)(cur_h - 1));
      SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

      cur_w *= 2;
   }

   if (cur_w != pci->w) {
      WaitQueue(7);
      SET_CURPT((short)pci->x,(short)pci->y);
      SET_DESTSTP((short)(pci->x + cur_w),(short)pci->y);
      SET_AXIS_PCNT((short)(pci->w - cur_w - 1),(short)(cur_h - 1));
      SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

      cur_w = pci->w;
   }
 /* Expand in the y direction */
   while (cur_h * 2 <= pci->h) {
      WaitQueue(7);
      SET_CURPT((short)pci->x, (short)pci->y);
      SET_DESTSTP((short)pci->x,(short)(pci->y + cur_h));
      SET_AXIS_PCNT((short)(cur_w - 1), (short)(cur_h - 1));
      SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

      cur_h *= 2;
   }

   if (cur_h != pci->h) {
      WaitQueue(7);
      SET_CURPT((short)pci->x,(short)pci->y);
      SET_DESTSTP((short)pci->x,(short)(pci->y + cur_h));
      SET_AXIS_PCNT((short)(cur_w - 1), (short)(pci->h - cur_h - 1));
      SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
   }
   WaitQueue(2);
   SET_MIX(FSS_FRGDCOL | MIX_SRC,BSS_BKGDCOL | MIX_SRC);
   UNBLOCK_CURSOR;
}

static CacheInfoPtr
DoCacheTile(pix)
     PixmapPtr pix;
{
   CacheInfoPtr pci = &cInfo;

   if( (pixmap_size) && (pix->drawable.width <= pixmap_size) &&
       (pix->drawable.height <= pixmap_size) ) {
      pci->pix_w = pix->drawable.width;
      pci->pix_h = pix->drawable.height;
      pci->nx = pixmap_size / pix->drawable.width;
      pci->ny = pixmap_size / pix->drawable.height;
      pci->x = pixmap_x;
      pci->y = pixmap_y;
      pci->w = pci->nx * pci->pix_w;
      pci->h = pci->ny * pci->pix_h;
      (s3ImageWriteFunc) (pci->x, pci->y, pci->pix_w, pci->pix_h,
		          pix->devPrivate.ptr, pix->devKind, 0, 0,
		          MIX_SRC, ~0);

      DoCacheExpandPixmap(pci);
      WaitIdleEmpty(); /* Make sure that all commands have finished */
      return(pci);
   }
   else
      return(NULL);
}

static CacheInfoPtr
DoCacheOpStipple(pix)
     PixmapPtr pix;
{
   CacheInfoPtr pci = &cInfo;

   if( (pixmap_size) && (pix->drawable.width <= pixmap_size) &&
       (pix->drawable.height <= pixmap_size) ) {
      pci->pix_w = pix->drawable.width;
      pci->pix_h = pix->drawable.height;
      pci->nx = pixmap_size / pix->drawable.width;
      pci->ny = pixmap_size / pix->drawable.height;
      pci->x = pixmap_x;
      pci->y = pixmap_y;
      pci->w = pci->nx * pci->pix_w;
      pci->h = pci->ny * pci->pix_h;

      s3ImageOpStipple(pci->x, pci->y, pci->pix_w, pci->pix_h,
		       pix->devPrivate.ptr, pix->devKind,
		       pci->pix_w, pci->pix_h, pci->x, pci->y,
		       255, 0, MIX_SRC, ~0);

      DoCacheExpandPixmap(pci);
      WaitIdleEmpty(); /* Make sure that all commands have finished */   
      return(pci);
   }
   else
      return(NULL);
}

static void
DoCacheImageFill(pci, x, y, w, h, pox, poy, fgalu, bgalu, 
		 fgmix, bgmix, planemask)
     CacheInfoPtr pci;
     int   x;
     int   y;
     int   w;
     int   h;
     int   pox;
     int   poy;
     short fgalu;
     short bgalu;
     short fgmix;
     short bgmix;
     Pixel planemask;
{
   int   xwmid, ywmid, orig_xwmid;
   int   startx, starty, endx, endy;
   int   orig_x = x;

   if (w == 0 || h == 0)
      return;

   modulus(x - pox, pci->w, startx);
   modulus(y - poy, pci->h, starty);
   modulus(x - pox + w - 1, pci->w, endx);
   modulus(y - poy + h - 1, pci->h, endy);

   orig_xwmid = xwmid = w - (pci->w - startx + endx + 1);
   ywmid = h - (pci->h - starty + endy + 1);

   BLOCK_CURSOR;
   WaitQueue16_32(7,8);
   SET_SCISSORS(0,0,s3DisplayWidth-1,s3ScissB);
   SET_MIX(fgmix | fgalu,bgmix | bgalu);
   SET_WRT_MASK(planemask);

   if (starty + h - 1 < pci->h) {
      if (startx + w - 1 < pci->w) {
	 WaitQueue(7);
         SET_CURPT((short)(pci->x + startx),(short)(pci->y + starty));
	 SET_DESTSTP((short)x,(short)y);
	 SET_AXIS_PCNT((short)(w - 1), (short)(h - 1));
	 SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
      } else {
	 WaitQueue(7);
	 SET_CURPT((short)(pci->x + startx), (short)(pci->y + starty));
	 SET_DESTSTP((short)x,(short)y);
	 SET_AXIS_PCNT((short)(pci->w - startx - 1), (short)(h - 1));
	 SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	 x += pci->w - startx;

	 while (xwmid > 0) {
	    WaitQueue(7);
	    SET_CURPT((short)pci->x, (short)(pci->y + starty));
	    SET_DESTSTP((short)x, (short)y);
	    SET_AXIS_PCNT((short)(pci->w - 1), (short)(h - 1));
	    SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
	    x += pci->w;
	    xwmid -= pci->w;
	 }

	 WaitQueue(7);
	 SET_CURPT((short)pci->x, (short)(pci->y + starty));
	 SET_DESTSTP((short)x, (short)y);
	 SET_AXIS_PCNT((short)endx,(short)(h - 1));
	 SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
      }
   } else if (startx + w - 1 < pci->w) {
      WaitQueue(7);
      SET_CURPT((short)(pci->x + startx), (short)(pci->y + starty));
      SET_DESTSTP((short)x, (short)y);
      SET_AXIS_PCNT((short)(w - 1), (short)(pci->h - starty - 1));
      SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

      y += pci->h - starty;

      while (ywmid > 0) {
	 WaitQueue(7);
	 SET_CURPT((short)(pci->x + startx), (short)pci->y);
	 SET_DESTSTP((short)x, (short)y);
	 SET_AXIS_PCNT((short)(w - 1), (short)(pci->h - 1));
	 SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	 y += pci->h;
	 ywmid -= pci->h;
      }

      WaitQueue(7);
      SET_CURPT((short)(pci->x + startx), (short)pci->y);
      SET_DESTSTP((short)x, (short)y);
      SET_AXIS_PCNT((short)(w - 1), (short)endy);
      SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
   } else {
      WaitQueue(7);
      SET_CURPT((short)(pci->x + startx), (short)(pci->y + starty));
      SET_DESTSTP((short)x, (short)y);
      SET_AXIS_PCNT((short)(pci->w - startx - 1), (short)(pci->h - starty - 1));
      SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

      x += pci->w - startx;

      while (xwmid > 0) {
	 WaitQueue(7);
	 SET_CURPT((short)pci->x, (short)(pci->y + starty));
	 SET_DESTSTP((short)x, (short)y);
	 SET_AXIS_PCNT((short)(pci->w - 1), (short)(pci->h - starty - 1));
	 SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	 x += pci->w;
	 xwmid -= pci->w;
      }

      WaitQueue(7);
      SET_CURPT((short)pci->x, (short)(pci->y + starty));
      SET_DESTSTP((short)x, (short)y);
      SET_AXIS_PCNT((short)endx, (short)(pci->h - starty - 1));
      SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

      y += pci->h - starty;

      while (ywmid > 0) {
	 x = orig_x;
	 xwmid = orig_xwmid;

	 WaitQueue(7);
	 SET_CURPT((short)(pci->x + startx), (short)pci->y);
	 SET_DESTSTP((short)x, (short)y);
	 SET_AXIS_PCNT((short)(pci->w - startx - 1), (short)(pci->h - 1));
	 SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	 x += pci->w - startx;

	 while (xwmid > 0) {
	    WaitQueue(7);
	    SET_CURPT((short)pci->x, (short)pci->y);
	    SET_DESTSTP((short)x, (short)y);
	    SET_AXIS_PCNT((short)(pci->w - 1), (short)(pci->h - 1));
	    SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	    x += pci->w;
	    xwmid -= pci->w;
	 }

	 WaitQueue(7);
	 SET_CURPT((short)pci->x, (short)pci->y);
	 SET_DESTSTP((short)x, (short)y);
	 SET_AXIS_PCNT((short)endx, (short)(pci->h - 1));
	 SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	 y += pci->h;
	 ywmid -= pci->h;
      }

      x = orig_x;
      xwmid = orig_xwmid;

      WaitQueue(7);
      SET_CURPT((short)(pci->x + startx), (short)pci->y);
      SET_DESTSTP((short)x, (short)y);
      SET_AXIS_PCNT((short)(pci->w - startx - 1), (short)endy);
      SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);      

      x += pci->w - startx;

      while (xwmid > 0) {
	 WaitQueue(7);
	 SET_CURPT((short)pci->x, (short)pci->y);
	 SET_DESTSTP((short)x, (short)y);
	 SET_AXIS_PCNT((short)(pci->w - 1), (short)endy);
	 SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	 x += pci->w;
	 xwmid -= pci->w;
      }

      WaitQueue(7);
      SET_CURPT((short)pci->x, (short)pci->y);
      SET_DESTSTP((short)x, (short)y);
      SET_AXIS_PCNT((short)endx, (short)endy);
      SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
   }

   WaitQueue(2);
   SET_MIX(FSS_FRGDCOL | MIX_SRC, BSS_BKGDCOL | MIX_SRC);
   UNBLOCK_CURSOR;
}

static void
s3CImageFill(pci, x, y, w, h, pox, poy, alu, planemask)
     CacheInfoPtr pci;
     int   x;
     int   y;
     int   w;
     int   h;
     int   pox;
     int   poy;
     short alu;
     Pixel planemask;
{

   DoCacheImageFill(pci, x, y, w, h, pox, poy, alu, 
		    MIX_SRC, FSS_BITBLT, BSS_BITBLT, planemask);

}

static void
s3CImageStipple(pci, x, y, w, h, pox, poy, fg, alu, planemask)
     CacheInfoPtr pci;
     int   x;
     int   y;
     int   w;
     int   h;
     int   pox;
     int   poy;
     Pixel  fg;
     short alu;
     Pixel planemask;
{
   BLOCK_CURSOR;
   if (s3Trio32FCBug) {
      WaitQueue16_32(5,8);
      SET_FRGD_COLOR(fg);
      SET_BKGD_COLOR(0);
      SET_PIX_CNTL(MIXSEL_EXPBLT | COLCMPOP_F);
      SET_RD_MASK(0x01);
      DoCacheImageFill(pci, x, y, w, h, pox, poy, alu, 
		       MIX_OR, FSS_FRGDCOL, BSS_BKGDCOL, planemask);
   } else {
      WaitQueue16_32(4,6);
      SET_FRGD_COLOR(fg);
      SET_PIX_CNTL(MIXSEL_EXPBLT | COLCMPOP_F);
      SET_RD_MASK(0x01);
      DoCacheImageFill(pci, x, y, w, h, pox, poy, alu, 
		       MIX_DST, FSS_FRGDCOL, BSS_BKGDCOL, planemask);
   }

   WaitQueue16_32(3,4);
   SET_RD_MASK(~0);
   SET_PIX_CNTL(MIXSEL_FRGDMIX | COLCMPOP_F);
   UNBLOCK_CURSOR;
}

static void
s3CImageOpStipple(pci, x, y, w, h, pox, poy, fg, bg, alu, planemask)
     CacheInfoPtr pci;
     int   x;
     int   y;
     int   w;
     int   h;
     int   pox;
     int   poy;
     Pixel  fg;
     Pixel   bg;
     short alu;
     Pixel planemask;
{
   BLOCK_CURSOR;
   WaitQueue16_32(5,8);
   SET_FRGD_COLOR(fg);
   SET_BKGD_COLOR(bg);
   SET_PIX_CNTL(MIXSEL_EXPBLT | COLCMPOP_F);
   SET_RD_MASK(0x01);

   DoCacheImageFill(pci, x, y, w, h, pox, poy, alu, alu, 
		    FSS_FRGDCOL, BSS_BKGDCOL, planemask);

   WaitQueue16_32(3,4);
   SET_RD_MASK(~0);
   SET_PIX_CNTL(MIXSEL_FRGDMIX | COLCMPOP_F);
   UNBLOCK_CURSOR;
}

void
s3PolyFillRect(pDrawable, pGC, nrectFill, prectInit)
     DrawablePtr pDrawable;
     register GCPtr pGC;
     int   nrectFill;		/* number of rectangles to fill */
     xRectangle *prectInit;	/* Pointer to first rectangle to fill */
{
   xRectangle *prect;
   RegionPtr prgnClip;
   register BoxPtr pbox;
   register BoxPtr pboxClipped;
   BoxPtr pboxClippedBase;
   BoxPtr pextent;
   BoxRec stackRects[NUM_STACK_RECTS];
   cfbPrivGC *priv;
   int   numRects;
   int   n;
   int   xorg, yorg;
   int   width, height;
   PixmapPtr pPix;
   int   pixWidth;
   int   xrot, yrot;
   CacheInfoPtr pci;

   if (!xf86VTSema)
   {
      switch (s3InfoRec.bitsPerPixel) {
      case 8:
	 cfbPolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	 break;
      case 16:
	 cfb16PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	 break;
      case 24:
	 cfb24PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	 break;
      case 32:
	 cfb32PolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	 break;
      }
      return;
   }

   priv = (cfbPrivGC *) pGC->devPrivates[cfbGCPrivateIndex].ptr;
   prgnClip = priv->pCompositeClip;

   prect = prectInit;
   xorg = pDrawable->x;
   yorg = pDrawable->y;
   if (xorg || yorg) {
      prect = prectInit;
      n = nrectFill;
      while (n--) {
	 prect->x += xorg;
	 prect->y += yorg;
	 prect++;
      }
   }
   prect = prectInit;

   numRects = REGION_NUM_RECTS(prgnClip) * nrectFill;
   if (numRects > NUM_STACK_RECTS) {
      pboxClippedBase = (BoxPtr) ALLOCATE_LOCAL(numRects * sizeof(BoxRec));
      if (!pboxClippedBase)
	 return;
   } else
      pboxClippedBase = stackRects;

   pboxClipped = pboxClippedBase;

   if (REGION_NUM_RECTS(prgnClip) == 1) {
      int   x1, y1, x2, y2, bx2, by2;

      pextent = REGION_RECTS(prgnClip);
      x1 = pextent->x1;
      y1 = pextent->y1;
      x2 = pextent->x2;
      y2 = pextent->y2;
      while (nrectFill--) {
	 if ((pboxClipped->x1 = prect->x) < x1)
	    pboxClipped->x1 = x1;

	 if ((pboxClipped->y1 = prect->y) < y1)
	    pboxClipped->y1 = y1;

	 bx2 = (int)prect->x + (int)prect->width;
	 if (bx2 > x2)
	    bx2 = x2;
	 pboxClipped->x2 = bx2;

	 by2 = (int)prect->y + (int)prect->height;
	 if (by2 > y2)
	    by2 = y2;
	 pboxClipped->y2 = by2;

	 prect++;
	 if ((pboxClipped->x1 < pboxClipped->x2) &&
	     (pboxClipped->y1 < pboxClipped->y2)) {
	    pboxClipped++;
	 }
      }
   } else {
      int   x1, y1, x2, y2, bx2, by2;

      pextent = (*pGC->pScreen->RegionExtents) (prgnClip);
      x1 = pextent->x1;
      y1 = pextent->y1;
      x2 = pextent->x2;
      y2 = pextent->y2;
      while (nrectFill--) {
	 BoxRec box;

	 if ((box.x1 = prect->x) < x1)
	    box.x1 = x1;

	 if ((box.y1 = prect->y) < y1)
	    box.y1 = y1;

	 bx2 = (int)prect->x + (int)prect->width;
	 if (bx2 > x2)
	    bx2 = x2;
	 box.x2 = bx2;

	 by2 = (int)prect->y + (int)prect->height;
	 if (by2 > y2)
	    by2 = y2;
	 box.y2 = by2;

	 prect++;

	 if ((box.x1 >= box.x2) || (box.y1 >= box.y2))
	    continue;

	 n = REGION_NUM_RECTS(prgnClip);
	 pbox = REGION_RECTS(prgnClip);

       /*
        * clip the rectangle to each box in the clip region this is logically
        * equivalent to calling Intersect()
        */
	 while (n--) {
	    pboxClipped->x1 = max(box.x1, pbox->x1);
	    pboxClipped->y1 = max(box.y1, pbox->y1);
	    pboxClipped->x2 = min(box.x2, pbox->x2);
	    pboxClipped->y2 = min(box.y2, pbox->y2);
	    pbox++;

	  /* see if clipping left anything */
	    if (pboxClipped->x1 < pboxClipped->x2 &&
		pboxClipped->y1 < pboxClipped->y2) {
	       pboxClipped++;
	    }
	 }
      }
   }

   if (pboxClipped != pboxClippedBase) {
      n = pboxClipped - pboxClippedBase;
      switch (pGC->fillStyle) {
	case FillSolid:
	   BLOCK_CURSOR;
	   WaitQueue16_32(3,5);
	   SET_FRGD_COLOR(pGC->fgPixel);
	   SET_FRGD_MIX(FSS_FRGDCOL | s3alu[pGC->alu]);
	   SET_WRT_MASK(pGC->planemask);

	   pboxClipped = pboxClippedBase;
	   while (n--) {
	      WaitQueue(5);
	      SET_CURPT((short)(pboxClipped->x1), (short)(pboxClipped->y1));
	      SET_AXIS_PCNT((short)(pboxClipped->x2 - pboxClipped->x1 - 1), 
		    (short)(pboxClipped->y2 - pboxClipped->y1 - 1));
	      SET_CMD(CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);

	      pboxClipped++;
	   }

	   WaitQueue(2);
	   SET_MIX(FSS_FRGDCOL | MIX_SRC, BSS_BKGDCOL | MIX_SRC);
	   UNBLOCK_CURSOR;
	   break;
	case FillTiled:
	   xrot = pDrawable->x + pGC->patOrg.x;
	   yrot = pDrawable->y + pGC->patOrg.y;

	   pPix = pGC->tile.pixmap;
	   width = pPix->drawable.width;
	   height = pPix->drawable.height;
	   pixWidth = PixmapBytePad(width, pPix->drawable.depth);

	   pboxClipped = pboxClippedBase;
	   if ((pci = DoCacheTile(pPix))) {
	      while (n--) {
		 int w, h;
		 w = pboxClipped->x2 - pboxClipped->x1;
		 h = pboxClipped->y2 - pboxClipped->y1;
		 s3CImageFill(pci, pboxClipped->x1, pboxClipped->y1,
			      w, h,
			      xrot, yrot,
			      s3alu[pGC->alu], pGC->planemask);
		 pboxClipped++;
	      }
	   } else {
	      while (n--) {
		 (s3ImageFillFunc) (pboxClipped->x1, pboxClipped->y1,
				    pboxClipped->x2 - pboxClipped->x1,
				    pboxClipped->y2 - pboxClipped->y1,
				    pPix->devPrivate.ptr, pixWidth,
				    width, height, xrot, yrot,
				    s3alu[pGC->alu], pGC->planemask);
		 pboxClipped++;
	      }
	   }
	   break;
	case FillStippled:
	   xrot = pDrawable->x + pGC->patOrg.x;
	   yrot = pDrawable->y + pGC->patOrg.y;

	   pPix = pGC->stipple;
	   width = pPix->drawable.width;
	   height = pPix->drawable.height;
	   pixWidth = PixmapBytePad(width, pPix->drawable.depth);

	   pboxClipped = pboxClippedBase;
	   if ((pci = DoCacheOpStipple(pPix))) {
	      while (n--) {
	         int w, h;
	         w = pboxClipped->x2 - pboxClipped->x1;
		 h = pboxClipped->y2 - pboxClipped->y1;
	       	 s3CImageStipple(pci, pboxClipped->x1, pboxClipped->y1,
				 w, h,
				 xrot, yrot, pGC->fgPixel,
				 s3alu[pGC->alu], pGC->planemask);
		 pboxClipped++;
	      }
	   } else {
	      while (n--) {
		 s3ImageStipple(pboxClipped->x1, pboxClipped->y1,
				pboxClipped->x2 - pboxClipped->x1,
				pboxClipped->y2 - pboxClipped->y1,
				pPix->devPrivate.ptr, pixWidth,
				width, height, xrot, yrot,
				pGC->fgPixel,
				s3alu[pGC->alu], pGC->planemask);
		 pboxClipped++;
	      }
	   }
	   break;
	case FillOpaqueStippled:
	   xrot = pDrawable->x + pGC->patOrg.x;
	   yrot = pDrawable->y + pGC->patOrg.y;

	   pPix = pGC->stipple;
	   width = pPix->drawable.width;
	   height = pPix->drawable.height;
	   pixWidth = PixmapBytePad(width, pPix->drawable.depth);

	   pboxClipped = pboxClippedBase;
	   if ((pci = DoCacheOpStipple(pPix))) {
	      while (n--) {
      	         int w, h;
	         w = pboxClipped->x2 - pboxClipped->x1;
		 h = pboxClipped->y2 - pboxClipped->y1;
		 s3CImageOpStipple(pci, pboxClipped->x1, pboxClipped->y1,
				   w, h,
				   xrot, yrot,
				   pGC->fgPixel, pGC->bgPixel,
				   s3alu[pGC->alu],
				   pGC->planemask);
		 pboxClipped++;
	      }
	   } else {
	      while (n--) {
		 s3ImageOpStipple(pboxClipped->x1, pboxClipped->y1,
				  pboxClipped->x2 - pboxClipped->x1,
				  pboxClipped->y2 - pboxClipped->y1,
				  pPix->devPrivate.ptr, pixWidth,
				  width, height, xrot, yrot,
				  pGC->fgPixel, pGC->bgPixel,
				  s3alu[pGC->alu],
				  pGC->planemask);
		 pboxClipped++;
	      }
	   }
	   break;
      }
   }
   if (pboxClippedBase != stackRects)
      DEALLOCATE_LOCAL(pboxClippedBase);
}

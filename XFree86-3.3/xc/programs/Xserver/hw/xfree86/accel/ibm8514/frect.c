/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/ibm8514/frect.c,v 3.4 1996/12/23 06:37:47 dawes Exp $ */
/*
 * Fill rectangles.
 */

/*

Copyright (c) 1989  X Consortium

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

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL KEVIN E. MARTIN OR TIAGO GONS BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified by Tiago Gons (tiago@comosjn.hobby.nl)

Simple expansion of tiles added by incorporating parts of the code from
mach8pcach.c 94-07-12, Hans Nasten ( nasten@everyware.se ).

*/
/* $XConsortium: frect.c /main/5 1996/02/21 17:24:19 kaleb $ */



#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "cfb.h"
#include "cfbmskbits.h"
#include "mergerop.h"

#include "reg8514.h"
#include "ibm8514.h"

#define NUM_STACK_RECTS	1024


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

void ibm8514InitFrect( x, y, size )
int x, y, size;
{

  pixmap_x = x;
  pixmap_y = y;
  pixmap_size = size;

}

static void DoCacheExpandPixmap( pci )
CacheInfoPtr pci;
{
    int cur_w = pci->pix_w;
    int cur_h = pci->pix_h;

    WaitQueue(7);
    outw(MULTIFUNC_CNTL, SCISSORS_T | 0);
    outw(MULTIFUNC_CNTL, SCISSORS_L | 0);
    outw(MULTIFUNC_CNTL, SCISSORS_R | 1023);
    outw(MULTIFUNC_CNTL, SCISSORS_B | 1023);
    outw(FRGD_MIX, FSS_BITBLT | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
    outw(WRT_MASK, 0xffff);

    /* Expand in the x direction */
    while (cur_w * 2 <= pci->w) {
	WaitQueue(7);
	outw(CUR_X, (short)pci->x);
	outw(CUR_Y, (short)pci->y);
	outw(DESTX_DIASTP, (short)(pci->x + cur_w));
	outw(DESTY_AXSTP, (short)pci->y);
	outw(MAJ_AXIS_PCNT, (short)(cur_w - 1));
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(cur_h -  1));
	outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	cur_w *= 2;
    }

    if (cur_w != pci->w) {
	WaitQueue(7);
	outw(CUR_X, (short)pci->x);
	outw(CUR_Y, (short)pci->y);
	outw(DESTX_DIASTP, (short)(pci->x + cur_w));
	outw(DESTY_AXSTP, (short)pci->y);
	outw(MAJ_AXIS_PCNT, (short)(pci->w - cur_w - 1));
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(cur_h -  1));
	outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	cur_w = pci->w;
    }

    /* Expand in the y direction */
    while (cur_h * 2 <= pci->h) {
	WaitQueue(7);
	outw(CUR_X, (short)pci->x);
	outw(CUR_Y, (short)pci->y);
	outw(DESTX_DIASTP, (short)pci->x);
	outw(DESTY_AXSTP, (short)(pci->y + cur_h));
	outw(MAJ_AXIS_PCNT, (short)(cur_w - 1));
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(cur_h -  1));
	outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	cur_h *= 2;
    }

    if (cur_h != pci->h) {
	WaitQueue(7);
	outw(CUR_X, (short)pci->x);
	outw(CUR_Y, (short)pci->y);
	outw(DESTX_DIASTP, (short)pci->x);
	outw(DESTY_AXSTP, (short)(pci->y + cur_h));
	outw(MAJ_AXIS_PCNT, (short)(cur_w - 1));
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(pci->h - cur_h -  1));
	outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
    }

    WaitQueue(2);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
}


static CacheInfoPtr DoCacheTile( pix )
PixmapPtr pix;
{
    CacheInfoPtr pci = &cInfo;

    if( pixmap_size && pix->drawable.width <= pixmap_size
    && pix->drawable.height <= pixmap_size ) {
      pci->pix_w = pix->drawable.width;
      pci->pix_h = pix->drawable.height;
      pci->nx = pixmap_size/pix->drawable.width;
      pci->ny = pixmap_size/pix->drawable.height;
      pci->x = pixmap_x;
      pci->y = pixmap_y;
      pci->w = pci->nx * pci->pix_w;
      pci->h = pci->ny * pci->pix_h;
      ibm8514ImageWrite( pci->x, pci->y, pci->pix_w, pci->pix_h,
			 pix->devPrivate.ptr, pix->devKind, 0, 0,
			 MIX_SRC, 0xffff );

      DoCacheExpandPixmap( pci );
      return( pci );
    }
    else
      return( NULL );
}

static CacheInfoPtr DoCacheOpStipple( pix )
PixmapPtr pix;
{
    CacheInfoPtr pci = &cInfo;

    if( pixmap_size && pix->drawable.width <= pixmap_size
    && pix->drawable.height <= pixmap_size ) {
      pci->pix_w = pix->drawable.width;
      pci->pix_h = pix->drawable.height;
      pci->nx = pixmap_size/pix->drawable.width;
      pci->ny = pixmap_size/pix->drawable.height;
      pci->x = pixmap_x;
      pci->y = pixmap_y;
      pci->w = pci->nx * pci->pix_w;
      pci->h = pci->ny * pci->pix_h;
      ibm8514ImageStipple( pci->x, pci->y, pci->pix_w, pci->pix_h,
			   pix->devPrivate.ptr, pix->devKind,
			   pci->pix_w, pci->pix_h, pci->x, pci->y,
			   255, 0, MIX_SRC, 0xffff, 1 );

      DoCacheExpandPixmap( pci );
      return( pci );
    }
    else
      return( NULL );
}


static void DoCacheImageFill( pci, x, y, w, h, pox, poy, fgalu, bgalu,
			      fgmix, bgmix, planemask)
CacheInfoPtr pci;
int x;
int y;
int w;
int h;
int pox;
int poy;
short fgalu;
short bgalu;
short fgmix;
short bgmix;
unsigned long planemask;
{
    int xwmid, ywmid, orig_xwmid;
    int startx, starty, endx, endy;
    int orig_x = x;

    if (w == 0 || h == 0)
	return;

    modulus(x - pox, pci->w, startx);
    modulus(y - poy, pci->h, starty);
    modulus(x - pox + w - 1, pci->w, endx);
    modulus(y - poy + h - 1, pci->h, endy);

    orig_xwmid = xwmid = w - (pci->w - startx + endx + 1);
    ywmid = h - (pci->h - starty + endy + 1);

    WaitQueue(7);
    outw(MULTIFUNC_CNTL, SCISSORS_T | 0);
    outw(MULTIFUNC_CNTL, SCISSORS_L | 0);
    outw(MULTIFUNC_CNTL, SCISSORS_R | 1023);
    outw(MULTIFUNC_CNTL, SCISSORS_B | 1023);
    outw(FRGD_MIX, fgmix | fgalu);
    outw(BKGD_MIX, bgmix | bgalu);
    outw(WRT_MASK, planemask);

    if (starty + h - 1 < pci->h) {
	if (startx + w - 1 < pci->w) {
	    WaitQueue(7);
	    outw(CUR_X, (short)(pci->x + startx));
	    outw(CUR_Y, (short)(pci->y + starty));
	    outw(DESTX_DIASTP, (short)x);
	    outw(DESTY_AXSTP, (short)y);
	    outw(MAJ_AXIS_PCNT, (short)(w - 1));
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(h -  1));
	    outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
	} else {
	    WaitQueue(7);
	    outw(CUR_X, (short)(pci->x + startx));
	    outw(CUR_Y, (short)(pci->y + starty));
	    outw(DESTX_DIASTP, (short)x);
	    outw(DESTY_AXSTP, (short)y);
	    outw(MAJ_AXIS_PCNT, (short)(pci->w - startx - 1));
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(h -  1));
	    outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	    x += pci->w - startx;

	    while (xwmid > 0) {
		WaitQueue(7);
		outw(CUR_X, (short)pci->x);
		outw(CUR_Y, (short)(pci-> y + starty));
		outw(DESTX_DIASTP, (short)x);
		outw(DESTY_AXSTP, (short)y);
		outw(MAJ_AXIS_PCNT, (short)(pci->w - 1));
		outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(h -  1));
		outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR |
			   WRTDATA);
		x += pci->w;
		xwmid -= pci->w;
	    }

	    WaitQueue(7);
	    outw(CUR_X, (short)pci->x);
	    outw(CUR_Y, (short)(pci->y + starty));
	    outw(DESTX_DIASTP, (short)x);
	    outw(DESTY_AXSTP, (short)y);
	    outw(MAJ_AXIS_PCNT, (short)endx);
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(h -  1));
	    outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
	}
    } else if (startx + w - 1 < pci->w) {
	WaitQueue(7);
	outw(CUR_X, (short)(pci->x + startx));
	outw(CUR_Y, (short)(pci->y + starty));
	outw(DESTX_DIASTP, (short)x);
	outw(DESTY_AXSTP, (short)y);
	outw(MAJ_AXIS_PCNT, (short)(w - 1));
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(pci->h - starty -  1));
	outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	y += pci->h - starty;

	while (ywmid > 0) {
	    WaitQueue(7);
	    outw(CUR_X, (short)(pci->x + startx));
	    outw(CUR_Y, (short)pci->y);
	    outw(DESTX_DIASTP, (short)x);
	    outw(DESTY_AXSTP, (short)y);
	    outw(MAJ_AXIS_PCNT, (short)(w - 1));
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(pci->h -  1));
	    outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	    y += pci->h;
	    ywmid -= pci->h;
	}

	WaitQueue(7);
	outw(CUR_X, (short)(pci->x + startx));
	outw(CUR_Y, (short)pci->y);
	outw(DESTX_DIASTP, (short)x);
	outw(DESTY_AXSTP, (short)y);
	outw(MAJ_AXIS_PCNT, (short)(w - 1));
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)endy);
	outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
    } else {
	WaitQueue(7);
	outw(CUR_X, (short)(pci->x + startx));
	outw(CUR_Y, (short)(pci->y + starty));
	outw(DESTX_DIASTP, (short)x);
	outw(DESTY_AXSTP, (short)y);
	outw(MAJ_AXIS_PCNT, (short)(pci->w - startx - 1));
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(pci->h - starty -  1));
	outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	x += pci->w - startx;

	while (xwmid > 0) {
	    WaitQueue(7);
	    outw(CUR_X, (short)pci->x);
	    outw(CUR_Y, (short)(pci->y + starty));
	    outw(DESTX_DIASTP, (short)x);
	    outw(DESTY_AXSTP, (short)y);
	    outw(MAJ_AXIS_PCNT, (short)(pci->w - 1));
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT |
				  (short)(pci->h - starty - 1));
	    outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	    x += pci->w;
	    xwmid -= pci->w;
	}

	WaitQueue(7);
	outw(CUR_X, (short)pci->x);
	outw(CUR_Y, (short)(pci->y + starty));
	outw(DESTX_DIASTP, (short)x);
	outw(DESTY_AXSTP, (short)y);
	outw(MAJ_AXIS_PCNT, (short)endx);
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(pci->h - starty - 1));
	outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	y += pci->h - starty;

	while (ywmid > 0) {
	    x = orig_x;
	    xwmid = orig_xwmid;

	    WaitQueue(7);
	    outw(CUR_X, (short)(pci-> x + startx));
	    outw(CUR_Y, (short)pci->y);
	    outw(DESTX_DIASTP, (short)x);
	    outw(DESTY_AXSTP, (short)y);
	    outw(MAJ_AXIS_PCNT, (short)(pci->w - startx - 1));
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(pci->h - 1));
	    outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	    x += pci->w - startx;

	    while (xwmid > 0) {
		WaitQueue(7);
		outw(CUR_X, (short)pci->x);
		outw(CUR_Y, (short)pci->y);
		outw(DESTX_DIASTP, (short)x);
		outw(DESTY_AXSTP, (short)y);
		outw(MAJ_AXIS_PCNT, (short)(pci->w - 1));
		outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(pci->h - 1));
		outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR |
			   WRTDATA);

		x += pci->w;
		xwmid -= pci->w;
	    }

	    WaitQueue(7);
	    outw(CUR_X, (short)pci->x);
	    outw(CUR_Y, (short)pci->y);
	    outw(DESTX_DIASTP, (short)x);
	    outw(DESTY_AXSTP, (short)y);
	    outw(MAJ_AXIS_PCNT, (short)endx);
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(pci->h - 1));
	    outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	    y += pci->h;
	    ywmid -= pci->h;
	}

	x = orig_x;
	xwmid = orig_xwmid;

	WaitQueue(7);
	outw(CUR_X, (short)(pci->x + startx));
	outw(CUR_Y, (short)pci->y);
	outw(DESTX_DIASTP, (short)x);
	outw(DESTY_AXSTP, (short)y);
	outw(MAJ_AXIS_PCNT, (short)(pci->w - startx - 1));
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)endy);
	outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	x += pci->w - startx;

	while (xwmid > 0) {
	    WaitQueue(7);
	    outw(CUR_X, (short)pci->x);
	    outw(CUR_Y, (short)pci->y);
	    outw(DESTX_DIASTP, (short)x);
	    outw(DESTY_AXSTP, (short)y);
	    outw(MAJ_AXIS_PCNT, (short)(pci->w - 1));
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)endy);
	    outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	    x += pci->w;
	    xwmid -= pci->w;
	}

	WaitQueue(7);
	outw(CUR_X, (short)pci->x);
	outw(CUR_Y, (short)pci->y);
	outw(DESTX_DIASTP, (short)x);
	outw(DESTY_AXSTP, (short)y);
	outw(MAJ_AXIS_PCNT, (short)endx);
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)endy);
	outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
    }

    WaitQueue(2);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
}


static void ibm8514CImageFill( pci, x, y, w, h, pox, poy, alu, planemask )
CacheInfoPtr pci;
int x;
int y;
int w;
int h;
int pox;
int poy;
short alu;
unsigned long planemask;
{

    DoCacheImageFill( pci, x, y, w, h, pox, poy, alu,
		      MIX_SRC, FSS_BITBLT, BSS_BITBLT, planemask );

}

static void ibm8514CImageStipple( pci, x, y, w, h, pox, poy,
				  fg, alu, planemask )
CacheInfoPtr pci;
int x;
int y;
int w;
int h;
int pox;
int poy;
unsigned long fg;
short alu;
unsigned long planemask;
{

    WaitQueue(3);
    outw(FRGD_COLOR, fg);
    outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPBLT | COLCMPOP_F);
    outw(RD_MASK, 0x01);

    DoCacheImageFill( pci, x, y, w, h, pox, poy, alu,
		      MIX_DST, FSS_FRGDCOL, BSS_BKGDCOL, planemask );

    WaitQueue(2);
    outw(RD_MASK, 0xff);
    outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);

}

static void ibm8514CImageOpStipple( pci, x, y, w, h, pox, poy,
				    fg, bg, alu, planemask )
CacheInfoPtr pci;
int x;
int y;
int w;
int h;
int pox;
int poy;
unsigned long fg;
unsigned long bg;
short alu;
unsigned long planemask;
{

    WaitQueue(4);
    outw(FRGD_COLOR, fg);
    outw(BKGD_COLOR, bg);
    outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPBLT | COLCMPOP_F);
    outw(RD_MASK, 0x01);

    DoCacheImageFill( pci, x, y, w, h, pox, poy, alu, alu,
		      FSS_FRGDCOL, BSS_BKGDCOL, planemask );

    WaitQueue(2);
    outw(RD_MASK, 0xff);
    outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);

}

void
ibm8514PolyFillRect(pDrawable, pGC, nrectFill, prectInit)
    DrawablePtr pDrawable;
    register GCPtr pGC;
    int		nrectFill; 	/* number of rectangles to fill */
    xRectangle	*prectInit;  	/* Pointer to first rectangle to fill */
{
    xRectangle	    *prect;
    RegionPtr	    prgnClip;
    register BoxPtr pbox;
    register BoxPtr pboxClipped;
    BoxPtr	    pboxClippedBase;
    BoxPtr	    pextent;
    BoxRec	    stackRects[NUM_STACK_RECTS];
    cfbPrivGC	    *priv;
    int		    numRects;
    int		    n;
    int		    xorg, yorg;
    int		    width, height;
    PixmapPtr	    pPix;
    int		    pixWidth;
    int		    xrot, yrot;
    CacheInfoPtr    pci;

/* 4-5-93 TCG : is VT visible */
    if (!xf86VTSema)
    {
        cfbPolyFillRect(pDrawable, pGC, nrectFill, prectInit);
	return;
    }

    priv = (cfbPrivGC *) pGC->devPrivates[cfbGCPrivateIndex].ptr;
    prgnClip = priv->pCompositeClip;

    prect = prectInit;
    xorg = pDrawable->x;
    yorg = pDrawable->y;
    if (xorg || yorg)
    {
	prect = prectInit;
	n = nrectFill;
	while(n--)
	{
	    prect->x += xorg;
	    prect->y += yorg;
	    prect++;
	}
    }

    prect = prectInit;

    numRects = REGION_NUM_RECTS(prgnClip) * nrectFill;
    if (numRects > NUM_STACK_RECTS)
    {
	pboxClippedBase = (BoxPtr)ALLOCATE_LOCAL(numRects * sizeof(BoxRec));
	if (!pboxClippedBase)
	    return;
    }
    else
	pboxClippedBase = stackRects;

    pboxClipped = pboxClippedBase;
	
    if (REGION_NUM_RECTS(prgnClip) == 1)
    {
	int x1, y1, x2, y2, bx2, by2;

	pextent = REGION_RECTS(prgnClip);
	x1 = pextent->x1;
	y1 = pextent->y1;
	x2 = pextent->x2;
	y2 = pextent->y2;
    	while (nrectFill--)
    	{
	    if ((pboxClipped->x1 = prect->x) < x1)
		pboxClipped->x1 = x1;
    
	    if ((pboxClipped->y1 = prect->y) < y1)
		pboxClipped->y1 = y1;
    
	    bx2 = (int) prect->x + (int) prect->width;
	    if (bx2 > x2)
		bx2 = x2;
	    pboxClipped->x2 = bx2;
    
	    by2 = (int) prect->y + (int) prect->height;
	    if (by2 > y2)
		by2 = y2;
	    pboxClipped->y2 = by2;

	    prect++;
	    if ((pboxClipped->x1 < pboxClipped->x2) &&
		(pboxClipped->y1 < pboxClipped->y2))
	    {
		pboxClipped++;
	    }
    	}
    }
    else
    {
	int x1, y1, x2, y2, bx2, by2;

	pextent = (*pGC->pScreen->RegionExtents)(prgnClip);
	x1 = pextent->x1;
	y1 = pextent->y1;
	x2 = pextent->x2;
	y2 = pextent->y2;
    	while (nrectFill--)
    	{
	    BoxRec box;
    
	    if ((box.x1 = prect->x) < x1)
		box.x1 = x1;
    
	    if ((box.y1 = prect->y) < y1)
		box.y1 = y1;
    
	    bx2 = (int) prect->x + (int) prect->width;
	    if (bx2 > x2)
		bx2 = x2;
	    box.x2 = bx2;
    
	    by2 = (int) prect->y + (int) prect->height;
	    if (by2 > y2)
		by2 = y2;
	    box.y2 = by2;
    
	    prect++;
    
	    if ((box.x1 >= box.x2) || (box.y1 >= box.y2))
	    	continue;
    
	    n = REGION_NUM_RECTS (prgnClip);
	    pbox = REGION_RECTS(prgnClip);
    
	    /* clip the rectangle to each box in the clip region
	       this is logically equivalent to calling Intersect()
	    */
	    while(n--)
	    {
		pboxClipped->x1 = max(box.x1, pbox->x1);
		pboxClipped->y1 = max(box.y1, pbox->y1);
		pboxClipped->x2 = min(box.x2, pbox->x2);
		pboxClipped->y2 = min(box.y2, pbox->y2);
		pbox++;

		/* see if clipping left anything */
		if(pboxClipped->x1 < pboxClipped->x2 && 
		   pboxClipped->y1 < pboxClipped->y2)
		{
		    pboxClipped++;
		}
	    }
    	}
    }

    if (pboxClipped != pboxClippedBase) {
	n = pboxClipped-pboxClippedBase;
	switch (pGC->fillStyle) {
	  case FillSolid:
	    WaitQueue(3);
	    outw(FRGD_COLOR, (short)(pGC->fgPixel));
	    outw(FRGD_MIX, FSS_FRGDCOL | ibm8514alu[pGC->alu]);
	    outw(WRT_MASK, (short)pGC->planemask);

	    pboxClipped = pboxClippedBase;
	    while (n--) {
		WaitQueue(5);
		outw(CUR_X, (short)(pboxClipped->x1));
		outw(CUR_Y, (short)(pboxClipped->y1));
		outw(MAJ_AXIS_PCNT,
		      (short)(pboxClipped->x2 - pboxClipped->x1 - 1));
		outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT |
		      (short)(pboxClipped->y2 - pboxClipped->y1 - 1));
		outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);

		pboxClipped++;
	    }

	    WaitQueue(2);
	    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
	    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
 	    break;
	  case FillTiled:
	    xrot = pDrawable->x + pGC->patOrg.x;
	    yrot = pDrawable->y + pGC->patOrg.y;

	    pPix = pGC->tile.pixmap;
	    width = pPix->drawable.width;
	    height = pPix->drawable.height;
	    pixWidth = PixmapBytePad(width, pPix->drawable.depth);

	    pboxClipped = pboxClippedBase;
	    if( pci = DoCacheTile( pPix ) ) {
		while (n--) {
		    ibm8514CImageFill(pci,
				      pboxClipped->x1, pboxClipped->y1,
				      pboxClipped->x2 - pboxClipped->x1,
				      pboxClipped->y2 - pboxClipped->y1,
				      xrot, yrot,
				      ibm8514alu[pGC->alu], pGC->planemask);
		    pboxClipped++;
		}
	    } else {
		while (n--) {
		    ibm8514ImageFill(pboxClipped->x1, pboxClipped->y1,
				     pboxClipped->x2 - pboxClipped->x1,
				     pboxClipped->y2 - pboxClipped->y1,
				     pPix->devPrivate.ptr, pixWidth,
				     width, height, xrot, yrot,
				     ibm8514alu[pGC->alu], pGC->planemask);
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
	    if( pci = DoCacheOpStipple( pPix ) ) {
		while (n--) {
		    ibm8514CImageStipple(pci,
					 pboxClipped->x1, pboxClipped->y1,
					 pboxClipped->x2 - pboxClipped->x1,
					 pboxClipped->y2 - pboxClipped->y1,
					 xrot, yrot, pGC->fgPixel,
					 ibm8514alu[pGC->alu], pGC->planemask);
		    pboxClipped++;
		}
	    } else {
		while (n--) {
		    ibm8514ImageStipple(pboxClipped->x1, pboxClipped->y1,
					pboxClipped->x2 - pboxClipped->x1,
					pboxClipped->y2 - pboxClipped->y1,
					pPix->devPrivate.ptr, pixWidth,
					width, height, xrot, yrot,
					pGC->fgPixel, 0,
					ibm8514alu[pGC->alu],
					pGC->planemask, 0);
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
	    if( pci = DoCacheOpStipple( pPix ) ) {
		while (n--) {
		    ibm8514CImageOpStipple(pci,
					   pboxClipped->x1, pboxClipped->y1,
					   pboxClipped->x2 - pboxClipped->x1,
					   pboxClipped->y2 - pboxClipped->y1,
					   xrot, yrot,
					   pGC->fgPixel, pGC->bgPixel,
					   ibm8514alu[pGC->alu],
					   pGC->planemask);
		    pboxClipped++;
		}
	    } else {
		while (n--) {
		    ibm8514ImageStipple(pboxClipped->x1, pboxClipped->y1,
					pboxClipped->x2 - pboxClipped->x1,
					pboxClipped->y2 - pboxClipped->y1,
					pPix->devPrivate.ptr, pixWidth,
					width, height, xrot, yrot,
					pGC->fgPixel, pGC->bgPixel,
					ibm8514alu[pGC->alu],
					pGC->planemask, 1);
		    pboxClipped++;
		}
	    }
	    break;
	}
    }
    if (pboxClippedBase != stackRects)
    	DEALLOCATE_LOCAL(pboxClippedBase);
}

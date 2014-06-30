/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/ibm8514/fs.c,v 3.4 1996/12/23 06:37:48 dawes Exp $ */
/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or X Consortium
not be used in advertising or publicity pertaining to 
distribution  of  the software  without specific prior 
written permission. Sun and X Consortium make no 
representations about the suitability of this software for 
any purpose. It is provided "as is" without any express or 
implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/

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

Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL KEVIN E. MARTIN OR TIAGO GONS BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified by Tiago Gons (tiago@comosjn.hobby.nl)

******************************************************************/
/* $XConsortium: fs.c /main/4 1996/02/21 17:24:23 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "cfb.h"

#include "reg8514.h"
#include "ibm8514.h"

void
ibm8514SolidFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
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

/* 4-5-93 TCG : is VT visible */
    if (!xf86VTSema)
    {
        cfbSolidSpansGeneral(pDrawable, pGC, 
			     nInit, pptInit, pwidthInit, fSorted);
	return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
        switch (pDrawable->depth) {
            case 1:
		ErrorF("should call mfbSolidFillSpans\n");
                break;
            case 8:
		ErrorF("should call cfbSolidFillSpans\n");
                break;
            default:
                ErrorF("Unsupported pixmap depth\n");
                break;
        }
        return;
    }

    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
    initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    initPpt = ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted);

    WaitQueue(3);
    outw(FRGD_COLOR, (short)(pGC->fgPixel));
    outw(FRGD_MIX, FSS_FRGDCOL | ibm8514alu[pGC->alu]);
    outw(WRT_MASK, (short)pGC->planemask);

    while (n--) {
	WaitQueue(5);
	outw(CUR_X, (short)(ppt->x));
	outw(CUR_Y, (short)(ppt->y));
	outw(MAJ_AXIS_PCNT, ((short)*pwidth)-1);
	outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | 0);
	outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);

	ppt++;
	pwidth++;
    }

    WaitQueue(2);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}

void
ibm8514TiledFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
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
    int xrot, yrot, width, height, pixWidth;
    PixmapPtr pPix = pGC->tile.pixmap;
    DDXPointPtr initPpt;
    int *initPwidth;

/* 4-5-93 TCG : is VT visible */
    if (!xf86VTSema)
    {
        cfbUnnaturalTileFS(pDrawable, pGC, 
			   nInit, pptInit, pwidthInit, fSorted);
	return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
        switch (pDrawable->depth) {
            case 1:
                ErrorF("should call mfbTiledFillSpans\n");
                break;
            case 8:
                ErrorF("should call cfbTiledFillSpans\n");
                break;
            default:
                ErrorF("Unsupported pixmap depth\n");
                break;
        }
        return;
    }

    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
    initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    initPpt = ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted);

    xrot = pDrawable->x + pGC->patOrg.x;
    yrot = pDrawable->y + pGC->patOrg.y;

    if (pPix == (PixmapPtr)0) {
	ErrorF("ibm8514TiledFSpans:  PixmapPtr tile.pixmap == NULL\n");
	return;
    }

    width = pPix->drawable.width;
    height = pPix->drawable.height;
    pixWidth = PixmapBytePad(width, pPix->drawable.depth);

    while (n--) {
	ibm8514ImageFill(ppt->x, ppt->y, *pwidth, 1,
			 pPix->devPrivate.ptr, pixWidth,
			 width, height, xrot, yrot,
			 ibm8514alu[pGC->alu], pGC->planemask);
	ppt++;
	pwidth++;
    }

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}

void
ibm8514StipFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
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
    int xrot, yrot, width, height, pixWidth;
    PixmapPtr pPix = pGC->stipple;
    DDXPointPtr initPpt;
    int *initPwidth;

/* 4-5-93 TCG : is VT visible */
    if (!xf86VTSema)
    {
        cfbUnnaturalStippleFS(pDrawable, pGC, 
			      nInit, pptInit, pwidthInit, fSorted);
	return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
        switch (pDrawable->depth) {
            case 1:
                ErrorF("should call mfbStippleFillSpans\n");
                break;
            case 8:
                ErrorF("should call cfbStippleFillSpans\n");
                break;
            default:
                ErrorF("Unsupported pixmap depth\n");
                break;
        }
        return;
    }

    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
    initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    initPpt = ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted);

    xrot = pDrawable->x + pGC->patOrg.x;
    yrot = pDrawable->y + pGC->patOrg.y;

    if (pPix == (PixmapPtr)0) {
	ErrorF("ibm8514StipFSpans:  PixmapPtr stipple == NULL\n");
	return;
    }

    width = pPix->drawable.width;
    height = pPix->drawable.height;
    pixWidth = PixmapBytePad(width, pPix->drawable.depth);

    while (n--) {
	ibm8514ImageStipple(ppt->x, ppt->y, *pwidth, 1,
			    pPix->devPrivate.ptr, pixWidth, width, height,
			    xrot, yrot, pGC->fgPixel, 0,
			    ibm8514alu[pGC->alu], pGC->planemask, 0);
	ppt++;
	pwidth++;
    }

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}

void
ibm8514OStipFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
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
    int xrot, yrot, width, height, pixWidth;
    PixmapPtr pPix = pGC->stipple;
    DDXPointPtr initPpt;
    int *initPwidth;

/* 4-5-93 TCG : is VT visible */
    if (!xf86VTSema)
    {
        cfbUnnaturalStippleFS(pDrawable, pGC, 
			      nInit, pptInit, pwidthInit, fSorted);
	return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
        switch (pDrawable->depth) {
            case 1:
                ErrorF("should call mfbOpStippleFillSpans\n");
                break;
            case 8:
                ErrorF("should call cfbOpStippleFillSpans\n");
                break;
            default:
                ErrorF("Unsupported pixmap depth\n");
                break;
        }
        return;
    }

    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
    initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    initPpt = ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans(((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted);

    xrot = pDrawable->x + pGC->patOrg.x;
    yrot = pDrawable->y + pGC->patOrg.y;

    if (pPix == (PixmapPtr)0) {
	ErrorF("ibm8514StipFSpans:  PixmapPtr stipple == NULL\n");
	return;
    }

    width = pPix->drawable.width;
    height = pPix->drawable.height;
    pixWidth = PixmapBytePad(width, pPix->drawable.depth);

    while (n--) {
	ibm8514ImageStipple(ppt->x, ppt->y, *pwidth, 1,
			    pPix->devPrivate.ptr, pixWidth,
			    width, height,
			    xrot, yrot, pGC->fgPixel, pGC->bgPixel,
			    ibm8514alu[pGC->alu], pGC->planemask, 1);
	ppt++;
	pwidth++;
    }

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}

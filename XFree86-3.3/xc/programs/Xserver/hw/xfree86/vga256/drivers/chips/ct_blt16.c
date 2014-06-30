/* $XConsortium: ct_blt16.c /main/3 1996/10/25 10:29:10 kaleb $ */
/*
 * Hooks for cfb16/24 CopyArea
 */

/*
 * 
 * Copyright (c) 1989  X Consortium
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of the X Consortium shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from the X Consortium.
 * 
 * Author: Keith Packard
 * 
 */

/*
 * Harm Hanemaayer <hhanemaa@cs.ruu.nl>:
 *     Routines adapted from cfbbitblt.c, cfbblt.c and cfbwindow.c to
 *     accelerate BitBlt in linear-framebuffer 16/32-bits-per-pixel
 *     modes on Cirrus chipsets.
 */

/* David Bateman <dbateman@ee.uts.edu.au>
 *    Modified again for use with Chips chipsets
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_blt16.c,v 3.3 1996/12/27 07:05:07 dawes Exp $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"

#include "compiler.h"
#include "vga256.h"
#include "xf86.h"
#include "vga.h"
#include "ct_driver.h"

/* Local functions. */

static void ctcfbBppDoBitbltCopy();
static void ctcfbBppPolyBitblt();

/* cfb16/24 functions that are referenced. */

extern RegionPtr cfb16BitBlt();
extern void cfb16DoBitbltCopy();
extern void cfb16DoBitbltXor();
extern void cfb16DoBitbltOr();
extern void cfb16DoBitbltGeneral();

extern RegionPtr cfb24BitBlt();
extern void cfb24DoBitbltCopy();
extern void cfb24DoBitbltXor();
extern void cfb24DoBitbltOr();
extern void cfb24DoBitbltGeneral();

/*
 * This function replaces the cfb16 CopyArea function. It catches plain
 * on-screen BitBlts in order to do them with the Chips BitBLT engine.
 */

RegionPtr
ctcfb16CopyArea(pSrcDrawable, pDstDrawable,
    pGC, srcx, srcy, width, height, dstx, dsty)
    register DrawablePtr pSrcDrawable;
    register DrawablePtr pDstDrawable;
    GC *pGC;
    int srcx, srcy;
    int width, height;
    int dstx, dsty;
{
    void (*doBitBlt) ();

#ifdef DEBUG
    ErrorF("ctcfb16CopyArea\n");
#endif

    doBitBlt = cfb16DoBitbltCopy;
    if (pGC->alu != GXcopy || (pGC->planemask & 0xFFFF) != 0xFFFF) {
	doBitBlt = cfb16DoBitbltGeneral;
	if ((pGC->planemask & 0xFFFF) == 0xFFFF) {
	    switch (pGC->alu) {
	    case GXxor:
		doBitBlt = cfb16DoBitbltXor;
		break;
	    case GXor:
		doBitBlt = cfb16DoBitbltOr;
		break;
	    }
	}
    }
    if (doBitBlt == cfb16DoBitbltCopy && pSrcDrawable->type == DRAWABLE_WINDOW
	&& pDstDrawable->type == DRAWABLE_WINDOW)
	doBitBlt = ctcfbBppDoBitbltCopy;

    return cfb16BitBlt(pSrcDrawable, pDstDrawable,
	pGC, srcx, srcy, width, height, dstx, dsty, doBitBlt, 0L);
}

/*
 * This function replaces the cfb24 CopyArea function. It catches plain
 * on-screen BitBlts in order to do them with the Chips BitBLT engine.
 */

RegionPtr
ctcfb24CopyArea(pSrcDrawable, pDstDrawable,
    pGC, srcx, srcy, width, height, dstx, dsty)
    register DrawablePtr pSrcDrawable;
    register DrawablePtr pDstDrawable;
    GC *pGC;
    int srcx, srcy;
    int width, height;
    int dstx, dsty;
{
    void (*doBitBlt) ();

#ifdef DEBUG
    ErrorF("ctcfb24CopyArea: ");
#endif

    doBitBlt = cfb24DoBitbltCopy;
    if (pGC->alu != GXcopy || (pGC->planemask & 0xFFFFFF) != 0xFFFFFF) {
#ifdef DEBUG
	ErrorF(":");
#endif
	doBitBlt = cfb24DoBitbltGeneral;
	if ((pGC->planemask & 0xFFFFFF) == 0xFFFFFF) {
	    switch (pGC->alu) {
	    case GXxor:
		doBitBlt = cfb24DoBitbltXor;
#ifdef DEBUG
		ErrorF("ctcfb24DoBitbltXor");
#endif
		break;
	    case GXor:
		doBitBlt = cfb24DoBitbltOr;
#ifdef DEBUG
		ErrorF("ctcfb24DoBitbltOr");
#endif
		break;
	    }
	}
    }
#ifdef DEBUG
    ErrorF("\n");
#endif

    if (doBitBlt == cfb24DoBitbltCopy && pSrcDrawable->type == DRAWABLE_WINDOW
	&& pDstDrawable->type == DRAWABLE_WINDOW)
	doBitBlt = ctcfbBppDoBitbltCopy;

    return cfb24BitBlt(pSrcDrawable, pDstDrawable,
	pGC, srcx, srcy, width, height, dstx, dsty, doBitBlt, 0L);
}

/*
 * This function replaces the ScreenRec CopyWindow function. It does the
 * plain BitBlt window moves with the Chips BitBLT engine.
 */

extern WindowPtr *WindowTable;

void
ctcfbCopyWindow(pWin, ptOldOrg, prgnSrc)
    WindowPtr pWin;
    DDXPointRec ptOldOrg;
    RegionPtr prgnSrc;
{
    DDXPointPtr pptSrc;
    register DDXPointPtr ppt;
    RegionRec rgnDst;
    register BoxPtr pbox;
    register int dx, dy;
    register int i, nbox;
    WindowPtr pwinRoot;

#ifdef DEBUG
    ErrorF("ctcfbCopyWindow\n");
#endif

    pwinRoot = WindowTable[pWin->drawable.pScreen->myNum];

    REGION_INIT(pWin->drawable.pScreen, &rgnDst, NullBox, 0);

    dx = ptOldOrg.x - pWin->drawable.x;
    dy = ptOldOrg.y - pWin->drawable.y;
    REGION_TRANSLATE(pWin->drawable.pScreen, prgnSrc, -dx, -dy);
    REGION_INTERSECT(pWin->drawable.pScreen, &rgnDst, &pWin->borderClip, prgnSrc);

    pbox = REGION_RECTS(&rgnDst);
    nbox = REGION_NUM_RECTS(&rgnDst);
    if (!nbox || !(pptSrc = (DDXPointPtr) ALLOCATE_LOCAL(nbox * sizeof(DDXPointRec)))) {
	REGION_UNINIT(pWin->drawable.pScreen, &rgnDst);
	return;
    }
    ppt = pptSrc;

    for (i = nbox; --i >= 0; ppt++, pbox++) {
	ppt->x = pbox->x1 + dx;
	ppt->y = pbox->y1 + dy;
    }

    ctcfbBppDoBitbltCopy((DrawablePtr) pwinRoot, (DrawablePtr) pwinRoot,
	GXcopy, &rgnDst, pptSrc, ~0L);
    DEALLOCATE_LOCAL(pptSrc);
    REGION_UNINIT(pWin->drawable.pScreen, &rgnDst);
}

/*
 * This is the local ctcfbBppDoBitbltCopy function that handles 16/24-bit
 * pixel on-screen BitBlts, dispatching them to ctcfbBppPolyBitblt.
 */

static void
ctcfbBppDoBitbltCopy(pSrc, pDst, alu, prgnDst, pptSrc, planemask)
    DrawablePtr pSrc, pDst;
    int alu;
    RegionPtr prgnDst;
    DDXPointPtr pptSrc;
    unsigned long planemask;
{
    unsigned long *psrcBase, *pdstBase;

    /* start of src and dst bitmaps */
    int widthSrc, widthDst;	       /* add to get to same position in next line */

    BoxPtr pbox;
    int nbox;

    BoxPtr pboxTmp, pboxNext, pboxBase, pboxNew1, pboxNew2;

    /* temporaries for shuffling rectangles */
    DDXPointPtr pptTmp, pptNew1, pptNew2;

    /* shuffling boxes entails shuffling the
     * source points too */
    int w, h;
    int xdir;			       /* 1 = left right, -1 = right left/ */
    int ydir;			       /* 1 = top down, -1 = bottom up */
    int careful;

#ifdef DEBUG
    ErrorF("ctcfbBppDoBitbltCopy\n");
#endif

#if 0
    /* This doesn't work because we need to deal with cfb16 and cfb24
     * at the same time. */
    cfbGetLongWidthAndPointer(pSrc, widthSrc, psrcBase);
    cfbGetLongWidthAndPointer(pDst, widthDst, pdstBase);
#else
    widthSrc = widthDst = vga256InfoRec.displayWidth;
    /* Hack!!! I'm assuming that if you get here you want a screen to
     * screen blit.  */
    psrcBase = pdstBase = (unsigned long *)VGABASE;
#endif

    /* XXX we have to err on the side of safety when both are windows,
     * because we don't know if IncludeInferiors is being used.
     */
    careful = ((pSrc == pDst) ||
	((pSrc->type == DRAWABLE_WINDOW) &&
	    (pDst->type == DRAWABLE_WINDOW)));

    pbox = REGION_RECTS(prgnDst);
    nbox = REGION_NUM_RECTS(prgnDst);

    pboxNew1 = NULL;
    pptNew1 = NULL;
    pboxNew2 = NULL;
    pptNew2 = NULL;
    if (careful && (pptSrc->y < pbox->y1)) {
	/* walk source bottom to top */
	ydir = -1;

	if (nbox > 1) {
	    /* keep ordering in each band, reverse order of bands */
	    pboxNew1 = (BoxPtr) ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
	    if (!pboxNew1)
		return;
	    pptNew1 = (DDXPointPtr) ALLOCATE_LOCAL(sizeof(DDXPointRec) * nbox);
	    if (!pptNew1) {
		DEALLOCATE_LOCAL(pboxNew1);
		return;
	    }
	    pboxBase = pboxNext = pbox + nbox - 1;
	    while (pboxBase >= pbox) {
		while ((pboxNext >= pbox) &&
		    (pboxBase->y1 == pboxNext->y1))
		    pboxNext--;
		pboxTmp = pboxNext + 1;
		pptTmp = pptSrc + (pboxTmp - pbox);
		while (pboxTmp <= pboxBase) {
		    *pboxNew1++ = *pboxTmp++;
		    *pptNew1++ = *pptTmp++;
		}
		pboxBase = pboxNext;
	    }
	    pboxNew1 -= nbox;
	    pbox = pboxNew1;
	    pptNew1 -= nbox;
	    pptSrc = pptNew1;
	}
    } else {
	/* walk source top to bottom */
	ydir = 1;
    }

    if (careful && (pptSrc->x < pbox->x1)) {
	/* walk source right to left */
	xdir = -1;

	if (nbox > 1) {
	    /* reverse order of rects in each band */
	    pboxNew2 = (BoxPtr) ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
	    pptNew2 = (DDXPointPtr) ALLOCATE_LOCAL(sizeof(DDXPointRec) * nbox);
	    if (!pboxNew2 || !pptNew2) {
		if (pptNew2)
		    DEALLOCATE_LOCAL(pptNew2);
		if (pboxNew2)
		    DEALLOCATE_LOCAL(pboxNew2);
		if (pboxNew1) {
		    DEALLOCATE_LOCAL(pptNew1);
		    DEALLOCATE_LOCAL(pboxNew1);
		}
		return;
	    }
	    pboxBase = pboxNext = pbox;
	    while (pboxBase < pbox + nbox) {
		while ((pboxNext < pbox + nbox) &&
		    (pboxNext->y1 == pboxBase->y1))
		    pboxNext++;
		pboxTmp = pboxNext;
		pptTmp = pptSrc + (pboxTmp - pbox);
		while (pboxTmp != pboxBase) {
		    *pboxNew2++ = *--pboxTmp;
		    *pptNew2++ = *--pptTmp;
		}
		pboxBase = pboxNext;
	    }
	    pboxNew2 -= nbox;
	    pbox = pboxNew2;
	    pptNew2 -= nbox;
	    pptSrc = pptNew2;
	}
    } else {
	/* walk source left to right */
	xdir = 1;
    }

    ctcfbBppPolyBitblt(pdstBase, psrcBase, widthSrc, widthDst, nbox,
	pptSrc, pbox, xdir, ydir, planemask);

    if (pboxNew2) {
	DEALLOCATE_LOCAL(pptNew2);
	DEALLOCATE_LOCAL(pboxNew2);
    }
    if (pboxNew1) {
	DEALLOCATE_LOCAL(pptNew1);
	DEALLOCATE_LOCAL(pboxNew1);
    }
}

/*
 * This functions performs the BitBlts, calling the low-level BitBLT
 * routines that talk to the hardware.
 */

static void
ctcfbBppPolyBitblt(pdstBase, psrcBase, widthSrc, widthDst, nbox, pptSrc,
    pbox, xdir, ydir, planemask)
    pointer pdstBase, psrcBase;
    int widthSrc, widthDst;
    int nbox;
    DDXPointPtr pptSrc;
    BoxPtr pbox;
    int xdir, ydir;
    unsigned long planemask;
{
#ifdef DEBUG
    ErrorF("ctcfbBppPolyBitblt");
#endif
#ifdef DEBUG
    ErrorF("planemask: 0x%X \n", planemask);
#endif

    for (; nbox; pbox++, pptSrc++, nbox--) {
	if (ctUseMMIO) {
	    if (ctisHiQV32)
		ctHiQVBitBlt((unsigned char *)psrcBase,
		    (unsigned char *)pdstBase, widthSrc, widthDst, pptSrc->x,
		    pptSrc->y, pbox->x1, pbox->y1, pbox->x2 - pbox->x1,
		    pbox->y2 - pbox->y1, xdir, ydir, 0x03, planemask);
	    else
		ctMMIOBitBlt((unsigned char *)psrcBase,
		    (unsigned char *)pdstBase, widthSrc, widthDst, pptSrc->x,
		    pptSrc->y, pbox->x1, pbox->y1, pbox->x2 - pbox->x1,
		    pbox->y2 - pbox->y1, xdir, ydir, 0x03, planemask);
	} else
	    ctBitBlt((unsigned char *)psrcBase, (unsigned char *)pdstBase,
		widthSrc, widthDst, pptSrc->x, pptSrc->y,
		pbox->x1, pbox->y1, pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
		xdir, ydir, 0x03, planemask);
    }
}

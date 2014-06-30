/* $XConsortium: ct_BitBlt.c /main/3 1996/10/25 10:28:24 kaleb $ */
/*
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  David Bateman <dbateman@ee.uts.edu.au> from pvgaBitBlt.c
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_BitBlt.c,v 3.3 1996/12/27 07:04:59 dawes Exp $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"cfb.h"
#include	"cfbmskbits.h"
#include	"cfbrrop.h"
#include	"cfb8bit.h"
#include	"fastblt.h"
#include	"mergerop.h"
#include        "vgaBank.h"

#include "compiler.h"
#include "xf86.h"
#include "vga.h"

#include "ct_driver.h"

void
ctcfbDoBitbltCopy(pSrc, pDst, alu, prgnDst, pptSrc, planemask)
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
    int srcaddr, dstaddr;

    MROP_DECLARE_REG();

#ifdef DEBUG
    ErrorF("CHIPS:ctcfbDoBitbltCopy(ctBitBlt(%x, %x, %x, %x, %x, %x)\n", pSrc, pDst, alu, prgnDst, pptSrc, planemask);
#endif

    MROP_INITIALIZE(alu, planemask);

    /* This function is only called in 8bpp and so using this below is ok */
    {
	unsigned char *t_psrcBase, *t_pdstBase;

	cfbGetByteWidthAndPointer(pSrc, widthSrc, t_psrcBase);
	cfbGetByteWidthAndPointer(pDst, widthDst, t_pdstBase);
	psrcBase = (unsigned long *)t_psrcBase;
	pdstBase = (unsigned long *)t_pdstBase;
    }

    BANK_FLAG_BOTH(psrcBase, pdstBase);

    /* XXX we have to err on the side of safety when both are windows,
     * because we don't know if IncludeInferiors is being used.
     */
    careful = ((pSrc == pDst) ||
	((pSrc->type == DRAWABLE_WINDOW) &&
	    (pDst->type == DRAWABLE_WINDOW)));

    if (!CHECKSCREEN(psrcBase) || !CHECKSCREEN(pdstBase)) {
	vga256DoBitbltCopy(pSrc, pDst, alu, prgnDst, pptSrc, planemask);
	return;
    }
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

    if (careful && (pptSrc->x < pbox->x1) && (pptSrc->y <= pbox->y1)) {
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

    while (nbox--) {
	w = pbox->x2 - pbox->x1;
	h = pbox->y2 - pbox->y1;
	if (ctUseMMIO) {
	    if (ctisHiQV32) {
	        ctHiQVBitBlt((unsigned char *)psrcBase, 
		    (unsigned char *)pdstBase, widthSrc, widthDst, pptSrc->x,
		    pptSrc->y, pbox->x1, pbox->y1, w, h, xdir, ydir, alu,
		    planemask);
	    } else {
	        ctMMIOBitBlt((unsigned char *)psrcBase, 
		    (unsigned char *)pdstBase, widthSrc, widthDst, pptSrc->x,
		    pptSrc->y, pbox->x1, pbox->y1, w, h, xdir, ydir, alu,
		    planemask);
	    }
	} else {
	    ctBitBlt((unsigned char *)psrcBase, (unsigned char *)pdstBase,
		widthSrc, widthDst, pptSrc->x, pptSrc->y, pbox->x1,
		pbox->y1, w, h, xdir, ydir, alu, planemask);
	}
	pbox++;
	pptSrc++;
    }

    if (pboxNew2) {
	DEALLOCATE_LOCAL(pptNew2);
	DEALLOCATE_LOCAL(pboxNew2);
    }
    if (pboxNew1) {
	DEALLOCATE_LOCAL(pptNew1);
	DEALLOCATE_LOCAL(pboxNew1);
    }
}

void
ctcfbFillBoxSolid(pDrawable, nBox, pBox, pixel1, pixel2, alu)
    DrawablePtr pDrawable;
    int nBox;
    BoxPtr pBox;
    unsigned long pixel1;
    unsigned long pixel2;
    int alu;
{
    unsigned char *pdstBase;
    unsigned long widthDst;
    unsigned int mask;

#ifdef DEBUG
    ErrorF("CHIPS:ctcfbFillBoxSolid ");
#endif
    if (pDrawable->type == DRAWABLE_WINDOW) {
#ifdef DEBUG
	ErrorF("DRAWABLE_WINDOW\n");
#endif
	if (vgaBitsPerPixel == 8) {    /* This only works for 8bpp */
	    pdstBase = (unsigned char *)
		(((PixmapPtr) (pDrawable->pScreen->devPrivate))->devPrivate.ptr);
	    widthDst = (int)
		(((PixmapPtr) (pDrawable->pScreen->devPrivate))->devKind);
	} else
	    /* 16/24 bpp can only get here for screen-screen operation.
	     * Hence we don't need pdstBase */
	    widthDst = vga256InfoRec.displayWidth;
    } else {
#ifdef DEBUG
	ErrorF("!DRAWABLE_WINDOW\n");
#endif
	if (vgaBitsPerPixel == 8) {    /* This only works for 8bpp */
	    pdstBase = (unsigned char *)(((PixmapPtr) pDrawable)->devPrivate.ptr);
	    widthDst = (int)(((PixmapPtr) pDrawable)->devKind);
	} else {
	    pdstBase = (unsigned char *)VGABASE;
	    widthDst = vga256InfoRec.displayWidth;
	}
    }

    if (vgaBitsPerPixel == 8 && !CHECKSCREEN(pdstBase)) {
#ifdef DEBUG
	ErrorF("!CHECKSCREEN(0x%X)\n", pdstBase);
#endif
	switch (vgaBitsPerPixel) {
	case 16:
	    cfb16FillBoxSolid(pDrawable, nBox, pBox, pixel1, pixel2, alu);
	    break;
	case 24:
	    cfb24FillBoxSolid(pDrawable, nBox, pBox, pixel1, pixel2, alu);
	    break;
	default:
	case 8:
	    vga256FillBoxSolid(pDrawable, nBox, pBox, pixel1, pixel2, alu);
	    break;
	}
	return;
    }
    /* This is an ugly hack to get around compiling this file
     * multiple times. This should probably be done more neatly */
    if (ctisHiQV32) {
	ctHiQVSetFGColorBPP(pixel1, mask);
	ctHiQVSetBGColorBPP(pixel1, mask);
    } else {
	int ret;
	if (ctUseMMIO) {
	    ret = ctMMIOSetFGColorBPP(pixel1, mask);
	    ret += ctMMIOSetBGColorBPP(pixel1, mask);
	} else {
	    ret = ctSetFGColorBPP(pixel1, mask);
	    ret += ctSetBGColorBPP(pixel1, mask);
	}
	if (ret) {
	    cfb24FillBoxSolid(pDrawable, nBox, pBox, pixel1, pixel2, alu);
	    return;
	}
    }

    for (; nBox; nBox--, pBox++) {
	if (ctUseMMIO) {
	    if (ctisHiQV32) {
		ctHiQVBitBlt((unsigned char *)NULL, (unsigned char *)pdstBase,
		    0, widthDst, 0, 0, pBox->x1, pBox->y1,
		    pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
		    1, 1, (alu & 0x0F) | 0x10, mask);
	    } else {
		ctMMIOBitBlt((unsigned char *)NULL, (unsigned char *)pdstBase,
		    0, widthDst, 0, 0, pBox->x1, pBox->y1,
		    pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
		    1, 1, (alu & 0x0F) | 0x10, mask);
	    }
	} else {
	    ctBitBlt((unsigned char *)NULL, (unsigned char *)pdstBase,
		0, widthDst, 0, 0, pBox->x1, pBox->y1,
		pBox->x2 - pBox->x1, pBox->y2 - pBox->y1,
		1, 1, (alu & 0x0F) | 0x10, mask);
	}
    }
}

#if 0				       /* These functions are no longer used */
void
ctcfbFillRectSolidCopy(pDrawable, pGC, nBox, pBox)
    DrawablePtr pDrawable;
    GCPtr pGC;
    int nBox;
    BoxPtr pBox;
{
    RROP_DECLARE;

    RROP_FETCH_GC(pGC);
#ifdef DEBUG
    ErrorF("CHIPS:ctcfbFillSolidCopy\n");
#endif
    ctcfbFillBoxSolid(pDrawable, nBox, pBox, rrop_xor, 0, GXcopy);
}

/* This is what ctcfbPolyFillRect uses for non-GXcopy fills. */

void
ctcfbFillRectSolidGeneral(pDrawable, pGC, nBox, pBox)
    DrawablePtr pDrawable;
    GCPtr pGC;
    int nBox;
    BoxPtr pBox;
{
    unsigned long rrop_xor, rrop_and;

    RROP_FETCH_GC(pGC);

#ifdef DEBUG
    ErrorF("CHIPS:ctcfbFillRectSolidGeneral: op 0x%X \n", pGC->alu);
#endif

    switch (vgaBitsPerPixel) {
    case 8:
	if ((pGC->planemask & 0xff) == 0xff || pGC->alu == GXcopy)
	    ctcfbFillBoxSolid(pDrawable, nBox, pBox, pGC->fgPixel, pGC->bgPixel,
		pGC->alu);
	else
	    vga256FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
	break;
    case 16:
	if ((pGC->planemask & 0xffff) == 0xffff || pGC->alu == GXcopy)
	    ctcfbFillBoxSolid(pDrawable, nBox, pBox, pGC->fgPixel, pGC->bgPixel,
		pGC->alu);
	else
	    cfb16FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
	break;
    case 24:
	if ((pGC->planemask & 0xffffff) == 0xffffff || pGC->alu == GXcopy)
	    ctcfbFillBoxSolid(pDrawable, nBox, pBox, pGC->fgPixel, pGC->bgPixel,
		pGC->alu);
	else
	    cfb24FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
	break;
    }
}
#endif

extern void cfbCopyPlane1to8();

void ctcfbCopyPlane1to8(pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc,
planemask, bitplane)
	DrawablePtr pSrcDrawable;
	DrawablePtr pDstDrawable;
	int rop;
	unsigned long planemask;
	RegionPtr prgnDst;
	DDXPointPtr pptSrc;
	int bitplane;	/* Unused. */
{
    unsigned long *psrcBase, *pdstBase;
    int	widthSrc, widthDst;
    int pixwidth;
    int nbox;
    BoxPtr  pbox;

    cfbGetLongWidthAndPointer (pSrcDrawable, widthSrc, psrcBase)

    cfbGetLongWidthAndPointer (pDstDrawable, widthDst, pdstBase)

    if (!CHECKSCREEN(pdstBase) || cfb8StippleRRop != GXcopy) {
    	vga256CopyPlane1to8(pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc,
    		planemask, 1);
    	return;
    }

    nbox = REGION_NUM_RECTS(prgnDst);
    pbox = REGION_RECTS(prgnDst);

    /* The planemask is understood to be 1 for all cases in which */
    /* this function is called. */

    while (nbox--)
    {
	int srcx, srcy, dstx, dsty, width, height;
	int bg, fg;
	dstx = pbox->x1;
	dsty = pbox->y1;
	srcx = pptSrc->x;
	srcy = pptSrc->y;
	width = pbox->x2 - pbox->x1;
	height = pbox->y2 - pbox->y1;
	pbox++;
	pptSrc++;

	fg = cfb8StippleFg;
	bg = cfb8StippleBg;

	if (width >= 32) {
	    if (ctUseMMIO) {
		if (ctisHiQV32) {
		    ctHiQVBLTWriteBitmap(dstx, dsty, width, height,
			    psrcBase, widthSrc * 4, srcx, srcy, bg, fg,
			    widthDst * 4);
		} else {
		    ctMMIOBLTWriteBitmap(dstx, dsty, width, height,
			    psrcBase, widthSrc * 4, srcx, srcy, bg, fg,
			    widthDst * 4);
		}
		
	    } else {
		ctcfbBLTWriteBitmap(dstx, dsty, width, height,
			psrcBase, widthSrc * 4, srcx, srcy, bg, fg,
			widthDst * 4);
	    }
	} else {
		/* Create singular region. */
		RegionRec reg;
		(*pDstDrawable->pScreen->RegionInit)(&reg, pbox - 1, 1);
		vga256CopyPlane1to8(pSrcDrawable, pDstDrawable, rop,
			&reg, pptSrc - 1, planemask, 1);
		(*pDstDrawable->pScreen->RegionUninit)(&reg);
	}
    }
}

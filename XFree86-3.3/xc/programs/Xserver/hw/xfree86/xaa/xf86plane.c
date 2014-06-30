/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86plane.c,v 3.4.2.1 1997/05/26 14:36:23 dawes Exp $ */

/*
 * Copyright 1996  The XFree86 Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * HARM HANEMAAYER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 */
/*
 * This implements a CopyPlane hook to accelerate writing bitmaps to the
 * screen. The hook is always installed because of the improved cfb
 * functions (except at 24bpp).
 *
 * This file is compiled seperately for VGA256 (all other depths share
 * the same functions).
 *
 * There something nasty going on with the DoBitBlt function as called
 * from the cfbBitBlt helper function: cfb does not pass the foreground
 * and background colors. So we must use a global mechanism for passing
 * these (as indeed cfb itself does). The alternative would be to
 * duplicate the cfbBitBlt helper function, like many other servers do,
 * but that seems a little unnecessary. Also note that the correct
 * cfbDoBitBlt helper function is called, corresponding to the depth.
 *
 * Anyone want to add a CopyPlane1to24 for 24bpp?
 */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"mi.h"
#ifdef VGA256
#include	"vga256.h"
#else
/* PSZ doesn't matter. */
#define PSZ 8
#include	"cfb.h"
#include	"cfb16.h"
#include	"cfb24.h"
#include	"cfb32.h"
#include	"cfbmskbits.h"
#include	"cfb8bit.h"
#endif
#include	"fastblt.h"

#include "xf86.h"
#include "xf86xaa.h"
#include "xf86gcmap.h"
#include "xf86local.h"


static void xf86CopyPlane1toNLocal();
static void xf86CopyPlaneNto1();

static int bgPixel, fgPixel;

static int CopyPlane1ToNIsAccelerated(pSrcDrawable, pDstDrawable, alu,
planemask, bg, fg)
    DrawablePtr	pSrcDrawable;
    DrawablePtr	pDstDrawable;
    int alu;
    unsigned long planemask;
    int bg, fg;
{
    if ((pSrcDrawable->type != DRAWABLE_WINDOW) && 
	(pDstDrawable->type == DRAWABLE_WINDOW) &&
	xf86VTSema &&	/* This shouldn't be necessary. */
	xf86AccelInfoRec.WriteBitmap &&
	(!(xf86AccelInfoRec.ColorExpandFlags & NO_PLANEMASK) ||
	(planemask & ((1 << pDstDrawable->depth) - 1)) ==
	((1 << pDstDrawable->depth) - 1)) &&
	(!(xf86AccelInfoRec.ColorExpandFlags & GXCOPY_ONLY) ||
	alu == GXcopy) &&
	(alu != GXinvert) && /* Exclude ROP's that don't include the src */
	(alu != GXclear) &&
	(alu != GXnoop) &&
	(alu != GXset) &&
        (!(xf86AccelInfoRec.ColorExpandFlags & RGB_EQUAL) ||
        ((fg & 0xFF) == ((fg & 0xFF00) >> 8) &&
        (fg & 0xFF) == ((fg & 0xFF0000) >> 16)) &&
        ((bg & 0xFF) == ((bg & 0xFF00) >> 8) &&
        (bg & 0xFF) == ((bg & 0xFF0000) >> 16)))
	)
	return TRUE;
    return FALSE;
}


RegionPtr
xf86CopyPlane(pSrcDrawable, pDstDrawable,
	       pGC, srcx, srcy, width, height, dstx, dsty, bitPlane)
    DrawablePtr		pSrcDrawable;
    DrawablePtr		pDstDrawable;
    GCPtr		pGC;
    int			srcx, srcy;
    int			width, height;
    int			dstx, dsty;
    unsigned long	bitPlane;
{
    RegionPtr  ret;

    bgPixel = pGC->bgPixel;
    fgPixel = pGC->fgPixel;

    if (pSrcDrawable->bitsPerPixel == 24 || pDstDrawable->bitsPerPixel == 24) {
    	/* Check to see if the 24bpp operation can be accelerated. */
        if (!(pSrcDrawable->bitsPerPixel == 1 && CopyPlane1ToNIsAccelerated(
        pSrcDrawable, pDstDrawable, pGC->alu, pGC->planemask,
        bgPixel, fgPixel)))
            return miCopyPlane(pSrcDrawable, pDstDrawable,
	        pGC, srcx, srcy, width, height, dstx, dsty, bitPlane);
	/*
	 * We continue for the case of a 1 to N copy that is accelerated
	 * at 24bpp. It will be handled by the first case below.
	 */
    }

    if (pSrcDrawable->bitsPerPixel == 1)  /* 1 to N copy */
    {
	if (bitPlane == 1)
	{
	    ret =  (*xf86GCInfoRec.cfbBitBltDispatch) (
	        pSrcDrawable, pDstDrawable, 
		pGC, srcx, srcy, width, height, 
		dstx, dsty, xf86CopyPlane1toNLocal, bitPlane);
	}
	else
	{
            ret = miHandleExposures(pSrcDrawable, pDstDrawable, 
				    pGC, srcx, srcy, width, height,
				    dstx, dsty, bitPlane);
	}
    }
    else if (pDstDrawable->bitsPerPixel == 1) /* N to 1 copy */
    {
        extern  int InverseAlu[16];
        int oldalu = pGC->alu;

        if ((pGC->fgPixel & 1) == 0 && (pGC->bgPixel & 1) == 1)
	{
            pGC->alu = InverseAlu[pGC->alu];
	}
        else if ((pGC->fgPixel & 1) == (pGC->bgPixel & 1))
	{
            pGC->alu = mfbReduceRop(pGC->alu, pGC->fgPixel);
	}

        ret = (*xf86GCInfoRec.cfbBitBltDispatch) (
            pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty, 
            xf86CopyPlaneNto1, bitPlane);
        pGC->alu = oldalu;
    }
    else /* N to N copy */
    {
        PixmapPtr       pBitmap;
        ScreenPtr       pScreen = pSrcDrawable->pScreen;
        GCPtr           pGC1;

        pBitmap = (*pScreen->CreatePixmap) (pScreen, width, height, 1);
        if (!pBitmap)
            return NULL;
        pGC1 = GetScratchGC (1, pScreen);
        if (!pGC1)
        {
            (*pScreen->DestroyPixmap) (pBitmap);
            return NULL;
        }
        /*
         * don't need to set pGC->fgPixel,bgPixel as copyPlane8to1
         * ignores pixel values, expecting the rop to "do the
         * right thing", which GXcopy will.
         */
        ValidateGC ((DrawablePtr) pBitmap, pGC1);

        /* no exposures here, scratch GC's don't get graphics expose */
        (void) (*xf86GCInfoRec.cfbBitBltDispatch) (
            pSrcDrawable, (DrawablePtr) pBitmap,
            pGC1, srcx, srcy, width, height, 0, 0, 
            xf86CopyPlaneNto1, bitPlane);

        /* no exposures here, copy bits from inside a pixmap */
        (void) (*xf86GCInfoRec.cfbBitBltDispatch) (
            (DrawablePtr) pBitmap, pDstDrawable, pGC,
            0, 0, width, height, dstx, dsty, 
	    xf86CopyPlane1toNLocal, 1);

        FreeScratchGC (pGC1);
        (*pScreen->DestroyPixmap) (pBitmap);

        /* compute resultant exposures */
        ret = miHandleExposures (pSrcDrawable, pDstDrawable, pGC,
                                 srcx, srcy, width, height,
                                 dstx, dsty, bitPlane);
    }
    return (ret);
}

/*
 * This kludge is required to be able to call xf86CopyPlane1toN
 * from other places.
 */

void
xf86CopyPlane1toN(pSrcDrawable, pDstDrawable, alu, rgnDst, pptSrc,
planemask, bitPlane, bg, fg)
    DrawablePtr   pSrcDrawable;
    DrawablePtr   pDstDrawable;
    int		  alu;
    RegionPtr     rgnDst;
    DDXPointPtr   pptSrc;
    unsigned int  planemask;
    unsigned long bitPlane;
    int bg;
    int fg;
{
    bgPixel = bg;
    fgPixel = fg;
    xf86CopyPlane1toNLocal(pSrcDrawable, pDstDrawable, alu, rgnDst, pptSrc,
        planemask, bitPlane);
}

static void
xf86CopyPlane1toNLocal(pSrcDrawable, pDstDrawable, alu, rgnDst, pptSrc,
planemask, bitPlane)
    DrawablePtr   pSrcDrawable;
    DrawablePtr   pDstDrawable;
    int		  alu;
    RegionPtr     rgnDst;
    DDXPointPtr   pptSrc;
    unsigned int  planemask;
    unsigned long bitPlane;
{
    BoxPtr pbox;
    int pixWidth, i, numRects;
    unsigned char *psrc;
    Bool accel;

    /*
     * Use any accelerated bitmap function if available.
     * These checks are ugly, they belong in ValidateGC.
     */
    accel = FALSE;
    if (CopyPlane1ToNIsAccelerated(pSrcDrawable, pDstDrawable, alu, planemask,
    bgPixel, fgPixel))
	accel = TRUE;
    
    if (accel) {
        pixWidth = PixmapBytePad(pSrcDrawable->width, pSrcDrawable->depth);
        psrc = ((PixmapPtr)pSrcDrawable)->devPrivate.ptr;
        pbox = REGION_RECTS(rgnDst);
        numRects = REGION_NUM_RECTS(rgnDst);

        for (i = numRects; --i >= 0; pbox++, pptSrc++) 
        {
            xf86AccelInfoRec.WriteBitmap(pbox->x1, pbox->y1,
                                   pbox->x2 - pbox->x1, 
			           pbox->y2 - pbox->y1,
                                   psrc, pixWidth,
                                   pptSrc->x, pptSrc->y, 
			           bgPixel, fgPixel,
                                   alu, planemask);
        }
        return;
    }
    /*
     * Note that in the case of 24bpp, checks have done at a higher level
     * that guarantee that we never get here (the function is only called
     * when the accelerated WriteBitmap can be used).
     */

#ifdef VGA256
    cfb8CheckOpaqueStipple(alu, fgPixel, bgPixel, planemask);
    vga256CopyPlane1to8(pSrcDrawable, pDstDrawable, alu, 
		        rgnDst, pptSrc, planemask, bitPlane);
#else
    switch (pDstDrawable->bitsPerPixel)
    {
        case 8:
            cfb8CheckOpaqueStipple(alu, fgPixel, bgPixel, planemask);
	    cfbCopyPlane1to8(pSrcDrawable, pDstDrawable, alu, 
		             rgnDst, pptSrc, planemask, bitPlane);
            break;
	case 16:
	    cfbCopyPlane1to16(pSrcDrawable, pDstDrawable, alu, 
			      rgnDst, pptSrc, planemask, bitPlane, 
			      bgPixel, fgPixel);
	    break;
	case 32:
	    cfbCopyPlane1to32(pSrcDrawable, pDstDrawable, alu, 
			      rgnDst, pptSrc, planemask, bitPlane, 
			      bgPixel, fgPixel);
	    break;
	default:
	    ErrorF("xf86CopyPlane1toN: unsupported function \n");
	    break;
    }
#endif
}

static void
xf86CopyPlaneNto1(pSrcDrawable, pDstDrawable, alu, rgnDst, pptSrc, planemask,
bitPlane)
    DrawablePtr   pSrcDrawable;
    DrawablePtr   pDstDrawable;
    int		  alu;
    RegionPtr     rgnDst;
    DDXPointPtr   pptSrc;
    unsigned int  planemask;
    unsigned long bitPlane;
{
    BoxPtr pbox;
    int pixWidth, i, numRects;
    unsigned char *psrc;

    if (!(planemask & bitPlane))
        return;

#ifdef VGA256
    vga256CopyPlane8to1(pSrcDrawable, pDstDrawable, alu, 
		     rgnDst, pptSrc, planemask, bitPlane);
#else
    switch (pSrcDrawable->bitsPerPixel)
    {
	case 8:
	    cfbCopyPlane8to1(pSrcDrawable, pDstDrawable, alu, 
			     rgnDst, pptSrc, planemask, bitPlane);
	    break;
	case 16:
	    cfbCopyPlane16to1(pSrcDrawable, pDstDrawable, alu, 
			      rgnDst, pptSrc, planemask, bitPlane);
	    break;
	case 32:
	    cfbCopyPlane32to1(pSrcDrawable, pDstDrawable, alu, 
			      rgnDst, pptSrc, planemask, bitPlane);
	    break;
	default:
	    ErrorF("xf86CopyPlaneNto1: unsupported function \n");
	    break;
    }
#endif
}

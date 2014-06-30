/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cirFillSt.c,v 3.14.2.2 1997/05/21 15:02:43 dawes Exp $ */
/*
 *
 * Copyright 1993 by H. Hanemaayer, Utrecht, The Netherlands
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of H. Hanemaayer not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  H. Hanemaayer makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * H. HANEMAAYER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL H. HANEMAAYER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  H. Hanemaayer, <hhanemaa@cs.ruu.nl>
 *
 */
/* $XConsortium: cirFillSt.c /main/6 1996/10/25 10:30:52 kaleb $ */

/*
 * These are the functions for stipples and tiles, which call low-level
 * functions.
 */

#include "vga256.h"
#include "cfbrrop.h"
#include "mergerop.h"
#include "vgaBank.h"
#include "xf86.h"
#include "vga.h"	/* For vga256InfoRec */

#include "compiler.h"

#include "cir_driver.h"
#include "cir_alloc.h"


/*
 * This function uses the color expand fill for opaque stipples.
 * I'm not entirely sure about the origin of the stipple; I assume
 * it should be (0, 0).
 */

void CirrusFillRectOpaqueStippled32(pDrawable, pGC, nBox, pBox)
	DrawablePtr pDrawable;
	GCPtr pGC;
	int nBox;		/* number of boxes to fill */
	register BoxPtr pBox;	/* pointer to list of boxes to fill */
{
	unsigned long *src;
	int stippleHeight;
	cfbPrivGCPtr devPriv;
	PixmapPtr stipple;
	int destPitch;

	devPriv = ((cfbPrivGCPtr) pGC->devPrivates[cfbGCPrivateIndex].ptr);
	stipple = devPriv->pRotatedPixmap;

	destPitch = (int)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind);

	cfb8CheckOpaqueStipple(pGC->alu, pGC->fgPixel, pGC->bgPixel,
		pGC->planemask);

	stippleHeight = stipple->drawable.height;
	src = (unsigned long *)stipple->devPrivate.ptr;

	for (;nBox; nBox--, pBox++) {
		int w = pBox->x2 - pBox->x1;
		int h = pBox->y2 - pBox->y1;
		if (cfb8StippleRRop == GXcopy && w >= 32 && h >= 10)
			CirrusColorExpandOpaqueStippleFill(pBox->x1, pBox->y1,
				w, h, src, stippleHeight,
				0, 0, pGC->bgPixel, pGC->fgPixel, destPitch);
		else {
			/* Special raster op. */
			/* Let cfb do this one. */
			speedupvga2568FillRectOpaqueStippled32(
				pDrawable, pGC, 1, pBox);
		}
	}
}


/*
 * This function uses the color expand fill for transparent stipples.
 */

void CirrusFillRectTransparentStippled32(pDrawable, pGC, nBox, pBox)
	DrawablePtr pDrawable;
	GCPtr pGC;
	int nBox;		/* number of boxes to fill */
	register BoxPtr pBox;	/* pointer to list of boxes to fill */
{
	unsigned long *src;
	int stippleHeight;
	cfbPrivGCPtr devPriv;
	PixmapPtr stipple;
	int destPitch;

	devPriv = ((cfbPrivGCPtr) pGC->devPrivates[cfbGCPrivateIndex].ptr);
	stipple = devPriv->pRotatedPixmap;

	destPitch = (int)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind);

	cfb8CheckStipple(pGC->alu, pGC->fgPixel, pGC->planemask);

	stippleHeight = stipple->drawable.height;
	src = (unsigned long *)stipple->devPrivate.ptr;

	for (;nBox; nBox--, pBox++) {
		int w = pBox->x2 - pBox->x1;
		int h = pBox->y2 - pBox->y1;
		if (cfb8StippleRRop == GXcopy && w >= 32 && h >= 10)
			CirrusColorExpandStippleFill(pBox->x1, pBox->y1,
				w, h, src, stippleHeight,
				0, 0, pGC->fgPixel, destPitch);
		else {
			/* Special raster op. */
			/* Let cfb do this one. */
			speedupvga2568FillRectTransparentStippled32(
				pDrawable, pGC, 1, pBox);
		}
	}
}


/*
 * Cirrus Tile fill.
 * There's no clear way to do tiling efficiently. There are lots of
 * different ways.
 * Uses a specific latched-write function for multiple-of-8 pixels wide tiles.
 * For 8x8 and 16x16 tiles, the BitBLT engine fill is used. For larger
 * tiles, we use repeated BitBlt.
 *
 * There's potential for using the 8x8 BitBLT engine pattern fill for
 * (1, 2, 4, 8, 16) x (1, 2, 4, 8) tiles, and more sizes by splitting
 * the area into 'bands' (the BitBLT engine should also work for any rop).
 * Also you can 'interleave' by using a doubled pitch (with a virtual screen
 * width < 2048, this works for height 16 because the pitch range goes up to
 * 4095).
 * On the 5434 (which can handle a pitch up to 8191) 4-way interleaving
 * can be used, so that we can use the BitBlt engine for 32x32 tiling
 * with 4 vertical subdivisions, and 16 pixels wide bands horizontally.
 * The 5434 may support a 32 byte wide pattern (for truecolor 8x8 pattern
 * fill), which would make it easier.
 *
 * Currently the accel routine is very restrictive.
 * I think for the 5434, the cfb routine (which does a very good
 * job) may be more efficient than the 32xH latched write fill (h != 32)
 * because of the 32-bit host interface.
 */

void rotatepattern( unsigned char *pattern, unsigned char *src, int pixWidth,
int width, int height, int hrot, int vrot ) {
	int i;
	for (i = 0; i < height; i++) {
		int j;
		unsigned char *srcline;
		srcline = src + ((i + vrot) & (height - 1)) * pixWidth;
		for (j = 0; j < width; j++) {
			*pattern = *(srcline + ((j + hrot) & (width - 1)));
			pattern++;
		}
	}
}

void CirrusFillRectTile(pDrawable, pGC, nBox, pBox)
	DrawablePtr pDrawable;
	GCPtr pGC;
	int nBox;		/* number of boxes to fill */
	register BoxPtr pBox;	/* pointer to list of boxes to fill */
{
	unsigned long *src;
	int stippleHeight;
	PixmapPtr pPix;
	int destPitch;
	int width, height;
	int pixWidth;
	int xrot, yrot;
	unsigned char *pattern;
	int vidpattern;

	pPix = pGC->tile.pixmap;
	width = pPix->drawable.width;
	height = pPix->drawable.height;
	pixWidth = PixmapBytePad(width, pPix->drawable.bitsPerPixel);

	destPitch = (int)
		(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind);

	src = (unsigned long *)pPix->devPrivate.ptr;
        xrot = pDrawable->x + pGC->patOrg.x;
	yrot = pDrawable->y + pGC->patOrg.y;

	if (HAVEBITBLTENGINE()) {
		if (width == 8 && height == 8)
			goto tile8x8;
		/* For 16x16 tile, prefer the extended write mode function
		 * to the 5426 BitBLT engine, except for small sizes.
		 * Always use BitBLT on the 543x.
		 */
		if (width == 16 && height == 16 && 
		((!(HAVE543X() || HAVE75XX() || cirrusChip == CLGD5446
		    || cirrusChip == CLGD5480) &&
		vga256InfoRec.displayWidth < 2048) ||
		((HAVE543X() || HAVE75XX() || cirrusChip == CLGD5446
		  || cirrusChip == CLGD5480) && 
		 vga256InfoRec.displayWidth < 4096)))
			goto tile16x16;
		if (width == 32 && height == 32 && 
		(HAVE543X() || HAVE75XX() || cirrusChip == 5446
		 || cirrusChip == CLGD5480) &&
		vga256InfoRec.displayWidth < 2048 && cirrusUseMMIO)
			goto tile32x32;
#if 0	/* broken. */
		if (width * height >= 500 && (width != 32 || height > 32 ||
		cirrusBusType == CIRRUS_SLOWBUS || HAVE543X() || HAVE755X() ||
		cirrusChip == CLGD5446 || cirrusChip == CLGD5480))
			goto tileblit;
#endif			
	}
	/* Use the extended write mode function for (?*8)xH tiles. 
	 */
	if (width == 8 || width == 16 || width == 32 || width == 64)
		goto tile8mx;

	vga256FillRectTileOdd(pDrawable, pGC, nBox, pBox);
	return;

tile8x8:
	/*
	 * This function does 8x8 tile fills with the BitBLT engine.
	 */
	pattern = NULL;
	for (; nBox; nBox--, pBox++) {
		int w, h;
		w = pBox->x2 - pBox->x1;
		h = pBox->y2 - pBox->y1;
		if (w * h < 5000) {
			vga256FillRectTileOdd(pDrawable, pGC, 1, pBox);
			continue;
		}
		if (pattern == NULL) {
			int i;
			/*
			 * Generate a square of four copies of the tile in
			 * system memory, so that any rotated position can
			 * be copied out of it.
			 */
			pattern = (unsigned char *)ALLOCATE_LOCAL(16 * 16);
			for (i = 0; i < 8; i++) {
			    	memcpy(pattern + i * 16,
				       (unsigned char *)src + i * pixWidth, 8);
			    	memcpy(pattern + i * 16 + 8,
				       (unsigned char *)src + i * pixWidth, 8);
			    	memcpy(pattern + (i + 8) * 16,
				       (unsigned char *)src + i * pixWidth, 8);
			    	memcpy(pattern + (i + 8) * 16 + 8,
				       (unsigned char *)src + i * pixWidth, 8);
			}
		}
		if (cirrusUseMMIO)
			CirrusMMIOBLT8x8PatternFill(pBox->y1 * destPitch + pBox->x1,
				w, h, pattern + ((pBox->y1 - yrot) & 7) * 16 +
				((pBox->x1 - xrot) & 7), 16, destPitch, CROP_SRC);
		else
			CirrusBLT8x8PatternFill(pBox->y1 * destPitch + pBox->x1,
				w, h, pattern + ((pBox->y1 - yrot) & 7) * 16 +
				((pBox->x1 - xrot) & 7), 16, destPitch, CROP_SRC);
	}
	if (pattern != NULL)
		DEALLOCATE_LOCAL(pattern);
	return;

tile16x16:
	/*
	 * This function does 16x16 tile fills with the BitBLT engine
	 * by dividing them into two-way vertically interleaved
	 * 16x8 pattern fills.
	 */
	pattern = NULL;
	for (; nBox; nBox--, pBox++) {
		int w, h;
		w = pBox->x2 - pBox->x1;
		h = pBox->y2 - pBox->y1;
		if (w * h < 5000) {
			vga256FillRectTileOdd(pDrawable, pGC, 1, pBox);
			continue;
		}
		if (pattern == NULL) {
			int i;
			/*
			 * Generate a square of four copies of the tile in
			 * system memory, so that any rotated position can
			 * be copied out of it.
			 */
			pattern = (unsigned char *)ALLOCATE_LOCAL(32 * 32);
			for (i = 0; i < 16; i++) {
			    	memcpy(pattern + i * 32,
				       (unsigned char *)src + i * pixWidth, 16);
			    	memcpy(pattern + i * 32 + 16,
				       (unsigned char *)src + i * pixWidth, 16);
			    	memcpy(pattern + (i + 16) * 32,
				       (unsigned char *)src + i * pixWidth, 16);
			    	memcpy(pattern + (i + 16) * 32 + 16,
				       (unsigned char *)src + i * pixWidth, 16);
			}
		}
		if (cirrusUseMMIO)
			CirrusMMIOBLT16x16PatternFill(pBox->y1 * destPitch + pBox->x1,
				w, h, pattern + ((pBox->y1 - yrot) & 15) * 32 +
				((pBox->x1 - xrot) & 15), 32, destPitch, CROP_SRC);
		else
			CirrusBLT16x16PatternFill(pBox->y1 * destPitch + pBox->x1,
				w, h, pattern + ((pBox->y1 - yrot) & 15) * 32 +
				((pBox->x1 - xrot) & 15), 32, destPitch, CROP_SRC);
	}
	if (pattern != NULL)
		DEALLOCATE_LOCAL(pattern);
	return;

tile32x32:
	/*
	 * This function does 32x32 tile fills with the BitBLT engine
	 * by dividing them into vertical bands of 16x8 tile fills
	 * (four-way vertically interleaved).
	 * Current revisions of the 5434 do not work correctly with
	 * 32-byte wide BitBLT pattern copy without color expansion;
	 * for chips that do support it the vertical bands are not
	 * necessary.
	 */
	pattern = NULL;
	for (; nBox; nBox--, pBox++) {
		int w, h;
		w = pBox->x2 - pBox->x1;
		h = pBox->y2 - pBox->y1;
		if (w * h < 5000) {
			vga256FillRectTileOdd(pDrawable, pGC, 1, pBox);
			continue;
		}
		if (pattern == NULL) {
			int i;
			/*
			 * Generate a square of four copies of the tile in
			 * system memory, so that any rotated position can
			 * be copied out of it.
			 */
			pattern = (unsigned char *)ALLOCATE_LOCAL(64 * 64);
			for (i = 0; i < 32; i++) {
			    	memcpy(pattern + i * 64,
				       (unsigned char *)src + i * pixWidth, 32);
			    	memcpy(pattern + i * 64 + 32,
				       (unsigned char *)src + i * pixWidth, 32);
			    	memcpy(pattern + (i + 32) * 64,
				       (unsigned char *)src + i * pixWidth, 32);
			    	memcpy(pattern + (i + 32) * 64 + 32,
				       (unsigned char *)src + i * pixWidth, 32);
			}
		}
		if (cirrusUseMMIO)
			CirrusMMIOBLT32x32PatternFill(pBox->y1 * destPitch + pBox->x1,
				w, h, pattern + ((pBox->y1 - yrot) & 31) * 64 +
				((pBox->x1 - xrot) & 31), 64, destPitch, CROP_SRC);
		else
			CirrusBLT32x32PatternFill(pBox->y1 * destPitch + pBox->x1,
				w, h, pattern + ((pBox->y1 - yrot) & 31) * 64 +
				((pBox->x1 - xrot) & 31), 64, destPitch, CROP_SRC);
	}
	if (pattern != NULL)
		DEALLOCATE_LOCAL(pattern);
	return;

#if 0
tileblit:
	/* Tile with repeated BitBLT. For largish tiles. */
	for (;nBox; nBox--, pBox++) {
		int w, h, x, y;
		int blitx, blith, blity;
		BoxRec box;
		x = pBox->x1;
		y = pBox->y1;
		w = pBox->x2 - x;
		h = pBox->y2 - y;
		/* Don't use the blitter for small tile fills. */
		if (w * h < 250) {
			vga256FillRectTileOdd(pDrawable, pGC, 1, pBox);
			continue;
		}
		box.x1 = x;
		box.y1 = y;
		box.x2 = x + min(width, w);
		box.y2 = y + height;
		/* Draw first tile. */
		vga256FillRectTileOdd(pDrawable, pGC, 1, &box);
		/* Repeat tile horizontally. */
		blitx = x + width;	/* Will skip if width > w. */
		blith = height;
		if (h < height)
			blith = h;
		while (blitx <= x + w - width) {
			CirrusBLTBitBlt(y * destPitch + blitx, y * destPitch +
				x, destPitch, destPitch, width, blith, 1);
			blitx += width;
		}
		/* Right edge. */
		if (blitx < x + w)
			CirrusBLTBitBlt(y * destPitch + blitx, y * destPitch +
				x, destPitch, destPitch, x + w - blitx,
				blith, 1);
		/* Repeat row of tiles vertically. */
		blity = y + height;	/* Will skip if height > h. */
		while (blity < y + h - height) {
			CirrusBLTBitBlt(blity * destPitch + x, y * destPitch +
				x, destPitch, destPitch, w, height, 1);
			blity += height;
		}
		/* Bottom edge. */
		if (blity < y + h)
			CirrusBLTBitBlt(blity * destPitch + x, y * destPitch +
				x, destPitch, destPitch, w, y + h - blity, 1);
	}
	return;
#endif	

tile8mx:
	/* For width == multiple of 8, max. 64. */

	/* Only rotate the pattern and allocate it in video memory when */
	/* we actually start to use the accelerated routine. */
	vidpattern = -1;

	for (;nBox; nBox--, pBox++) {
		int w, h, x, y;
		w = pBox->x2 - pBox->x1;
		h = pBox->y2 - pBox->y1;
		if (w * h < 10000)
			vga256FillRectTileOdd(pDrawable, pGC, 1, pBox);
		else {
			/* Check if we have not already uploaded the pattern
			 * to video memory. */
			if (vidpattern == -1) {
				/* Allocate space for the pattern in video
				 * memory. */
				vidpattern = (CirrusAllocate(width * height)).addr;
				if (vidpattern == -1) {
					/* Do the rest with vga256. */
					vga256FillRectTileOdd(pDrawable, pGC,
						nBox, pBox);
					return;
				}

				/* Do the work. */
				pattern = (unsigned char *)
					    ALLOCATE_LOCAL(width * height);
				/* Align to start of screen. */
				rotatepattern(pattern, (unsigned char *)src,
					pixWidth, width, height,
					(0 - xrot) & (width - 1), 0);

				CirrusUploadPattern(pattern, width, height,
					vidpattern, width);
			}

			x = pBox->x1;
			y = pBox->y1;
			CirrusColorExpandFillTile8(x, y, w, h, vidpattern,
				width, width / 8, height, yrot, destPitch);
		}
	}
	if (vidpattern != -1) {
		CirrusFree(vidpattern);
		DEALLOCATE_LOCAL(pattern);
	}
}

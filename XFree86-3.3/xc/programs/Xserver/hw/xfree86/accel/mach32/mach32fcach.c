/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/mach32fcach.c,v 3.14 1996/12/23 06:38:29 dawes Exp $ */
/*
 * Copyright 1992, 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Kevin E. Martin makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 *
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN OR RICKARD E. FAITH BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Dynamic cache allocation added by Rickard E. Faith (faith@cs.unc.edu)
 * Modified by Mike Bernson (mike@mbsun.mlb.org) for mach32 from s3 version
 */
/* $XConsortium: mach32fcach.c /main/7 1996/10/25 16:22:34 kaleb $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"cfb.h"
#include	"misc.h"
#include        "xf86.h"
#include	"windowstr.h"
#include	"gcstruct.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"mach32.h"
#include        "xf86bcache.h"
#include	"xf86fcache.h"
#include	"xf86text.h"

#define XCONFIG_FLAGS_ONLY
#include        "xf86_Config.h"

static unsigned long mach32FontAge;
#define NEXT_FONT_AGE  ++mach32FontAge

extern int xf86Verbose;

#define ALIGNMENT 8
#define PIXMAP_WIDTH 64

unsigned short mach32cachemask[16] = { 0x01, 0x02, 0x04, 0x08,
				       0x10, 0x20, 0x40, 0x80,
				       0x100, 0x200, 0x400, 0x800,
				       0x1000, 0x2000, 0x4000, 0x8000
};

unsigned short mach32cachemaskswapped8[8] = { 0x02, 0x04, 0x08, 0x10, 
					      0x20, 0x40, 0x80, 0x01
};

unsigned short *mach32cachereadmask;

void
mach32FontCache8Init()
{
   static int first = TRUE;
   int x, y, w, h;
   int BitPlane;
   CachePool FontPool;

   x = 0;
   y = mach32InfoRec.virtualY;
   h = mach32MaxY + 1 - mach32InfoRec.virtualY;
   w = mach32InfoRec.virtualX - PIXMAP_WIDTH;

   if (mach32InfoRec.bitsPerPixel == 8) {
      mach32cachereadmask = mach32cachemaskswapped8;
   } else {
      mach32cachereadmask = mach32cachemask;
   }
   /* Note, mach32InfoRec.virtual is always >= 1024 */
   if (h < PIXMAP_WIDTH) {
      w = mach32InfoRec.virtualX;
      ErrorF("%s %s: No pixmap expanding area available\n",
	     XCONFIG_PROBED, mach32InfoRec.name);
   } else {
	 if (first) {
	    mach32InitFrect(w, y, PIXMAP_WIDTH);
	    ErrorF("%s %s: Using a single %dx%d area for expanding pixmaps\n",
		   XCONFIG_PROBED, mach32InfoRec.name, PIXMAP_WIDTH,
		   PIXMAP_WIDTH);
	 }
   }

   /*
    * Don't allow a font cache if we don't have room for at least
    * 2 mcomplete 6x13 fonts.
    */
   if (w >= 6*32 && h >= 2*13) {
      if (first)  {
         FontPool = xf86CreateCachePool(ALIGNMENT);
         for (BitPlane = mach32InfoRec.bitsPerPixel-1; BitPlane >= 0;
	      BitPlane--) {
            xf86AddToCachePool(FontPool, x, y, w, h, BitPlane);
	 }

         xf86InitFontCache(FontPool, w, h, mach32FontOpStipple);
         xf86InitText(mach32GlyphWrite, mach32NoCPolyText, mach32NoCImageText );
         ErrorF("%s %s: Using %d planes of %dx%d at (%d,%d) aligned %d as font cache\n",
                XCONFIG_PROBED, mach32InfoRec.name,
                mach32InfoRec.bitsPerPixel, w, h, x, y, ALIGNMENT);
      }
      else {
        xf86ReleaseFontCache();
      }
   } else if (first) {

      /*
       * Crash and burn if the cached glyph write function gets called.
       */
      xf86InitText(NULL, mach32NoCPolyText, mach32NoCImageText);
      ErrorF("%s %s: No font cache available\n",
              XCONFIG_PROBED, mach32InfoRec.name);
   }
   first = 0;
   return;
}

static __inline__ void
Domach32CPolyText8(x, y, count, chars, fentry, pGC, pBox)
     int   x, y, count;
     unsigned char *chars;
     CacheFont8Ptr fentry;
     GCPtr pGC;
     BoxPtr pBox;
{
   int   gHeight, gWidth;
   int   w = fentry->w;
   int blocki = 255;
   unsigned short height = 0;
   unsigned short width = 0;
   Pixel pmsk = 0;

   for (;count > 0; count--, chars++) {
      CharInfoPtr pci;
      short xoff;

      pci = fentry->pci[(int)*chars];

      if (pci != NULL) {

	 gHeight = GLYPHHEIGHTPIXELS(pci);
	 gWidth = GLYPHWIDTHPIXELS(pci);
	 if (gHeight
	 && x + pci->metrics.leftSideBearing + gWidth-1 >= pBox->x1
	 && x + pci->metrics.leftSideBearing            <  pBox->x2
	 && y - pci->metrics.ascent         + gHeight-1 >= pBox->y1
	 && y - pci->metrics.ascent                     <  pBox->y2 ) {

	    if ((int) (*chars / 32) != blocki) {
	       bitMapBlockPtr block;
	       
	       blocki = (int) (*chars / 32);
	       block = fentry->fblock[blocki];
	       if (block == NULL) {
		  /*
		   * Reset the GE context to a known state before calling
		   * the xf86loadfontblock function.
		   */
		  WaitQueue(9);
		  outw(EXT_SCISSOR_T, 0);
		  outw(EXT_SCISSOR_L, 0);
		  outw(EXT_SCISSOR_R, mach32MaxX); 
		  outw(EXT_SCISSOR_B, mach32MaxY);
		  outw(RD_MASK, 0xFFFF);
		  outw(WRT_MASK, 0xFFFF);
		  outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);
                  outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
                  outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
		  xf86loadFontBlock(fentry, blocki);
		  block = fentry->fblock[blocki];		  

		  /*
		   * Restore the GE context.
		   */
		  WaitQueue(9);
		  outw(EXT_SCISSOR_L, (short)pBox->x1);
		  outw(EXT_SCISSOR_T, (short)pBox->y1);
		  outw(EXT_SCISSOR_R, (short)(pBox->x2 - 1));
		  outw(EXT_SCISSOR_B, (short)(pBox->y2 - 1));
		  outw(FRGD_COLOR, (short)pGC->fgPixel);
		  outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPBLT | COLCMPOP_F);
		  outw(FRGD_MIX, FSS_FRGDCOL | mach32alu[pGC->alu]);
		  outw(BKGD_MIX, BSS_BKGDCOL | MIX_DST);
		  outw(WRT_MASK, pGC->planemask);
		  height = width = pmsk = 0;
	       }
	       WaitQueue(2);
	       outw(CUR_Y, (short)block->y);	       

	       /*
		* Is the readmask altered
		*/
	       if (mach32cachereadmask[block->id] != pmsk) {
		  pmsk = mach32cachereadmask[block->id];
	       	  outw(RD_MASK, (unsigned short)pmsk);		  	       
	       }
	       xoff = block->x;
	       block->lru = NEXT_FONT_AGE;
	    }

	    WaitQueue(6);
	    outw(CUR_X, (short) (xoff + (*chars & 0x1f) * w));

	    outw(DESTX_DIASTP,
		  (short)(x + pci->metrics.leftSideBearing));
	    outw(DESTY_AXSTP, (short)(y - pci->metrics.ascent));

	    if (!width || (short)(GLYPHWIDTHPIXELS(pci)) != width) {
	       width = (short)(GLYPHWIDTHPIXELS(pci));
	       outw(MAJ_AXIS_PCNT, (short)(width - 1));
	    }
	    if (!height || (short)(gHeight) != height) {
	       height = (short)(gHeight);
	       outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (short)(height - 1));
	    }
	    outw(CMD, CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);
	 }
	 x += pci->metrics.characterWidth;
      }
   }
}

/*
 * Set the hardware scissors to match the clipping rectables and 
 * call the glyph output routine.
 */
void 
mach32GlyphWrite(x, y, count, chars, fentry, pGC, pBox, numRects)
     int   x, y, count;
     unsigned char *chars;
     CacheFont8Ptr fentry;
     GCPtr pGC;
     BoxPtr pBox;
     int numRects;
{
   int maxAscent, maxDescent;

   maxAscent = FONTMAXBOUNDS(pGC->font, ascent);
   maxDescent = FONTMAXBOUNDS(pGC->font, descent);

   WaitQueue(5);
   outw(FRGD_COLOR, (short)pGC->fgPixel);
   outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPBLT | COLCMPOP_F);
   outw(FRGD_MIX, FSS_FRGDCOL | mach32alu[pGC->alu]);
   outw(BKGD_MIX, BSS_BKGDCOL | MIX_DST);
   outw(WRT_MASK, pGC->planemask);


   for (; --numRects >= 0; ++pBox) {
      /*
       * Skip all boxes that are completley above or below the text string.
       */
      if( pBox->y2 >= y - maxAscent && pBox->y1 <= y + maxDescent ) {
         WaitQueue(4);
         outw(EXT_SCISSOR_L, (short)pBox->x1);
         outw(EXT_SCISSOR_T, (short)pBox->y1);
         outw(EXT_SCISSOR_R, (short)(pBox->x2 - 1));
         outw(EXT_SCISSOR_B, (short)(pBox->y2 - 1));

         Domach32CPolyText8(x, y, count, chars, fentry, pGC, pBox);
      }
   }

   WaitQueue(9);
   outw(EXT_SCISSOR_T, 0);
   outw(EXT_SCISSOR_L, 0);
   outw(EXT_SCISSOR_R, mach32MaxX);
   outw(EXT_SCISSOR_B, mach32MaxY);
   outw(RD_MASK, 0xFFFF);
   outw(WRT_MASK, 0xFFFF);
   outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);
   outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
   outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);

   return;
}

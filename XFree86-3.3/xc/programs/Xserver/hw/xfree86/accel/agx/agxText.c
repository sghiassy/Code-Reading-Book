/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxText.c,v 3.7 1996/12/23 06:33:02 dawes Exp $ */
/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Kevin E. Martin makes no
 * representations about the suitability of this software for any purpose. It
 * is provided "as is" without express or implied warranty.
 * 
 * KEVIN E. MARTIN AND HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE LIABLE 
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING 
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Modified by Amancio Hasty and Jon Tombs
 * Modified for the AGX by Henry A. Worth (haw30@eng.amdahl.com)
 * 
 */
/* $XConsortium: agxText.c /main/6 1996/02/21 17:18:34 kaleb $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"cfb.h"
#include	"misc.h"
#include	"xf86.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"mi.h"
#include	"agx.h"
#include	"regagx.h"
#include	"agxBCach.h"

extern CacheFont8Ptr agxCacheFont8();

static int  agxNoCPolyText(
#if NeedFunctionPrototypes
     DrawablePtr,
     GCPtr,
     int,
     int,
     int,
     char*,
     Bool,
     int
#endif
);

static __inline__ void DoagxPolyGlyphBlt(
#if NeedFunctionPrototypes
     GCPtr,
     int,
     int,
     unsigned int,
     CharInfoPtr*,
     unsigned int
#endif
);

int
agxPolyText8(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x, y;
     int   count;
     char *chars;
{
   CacheFont8Ptr fentry;

   if (!xf86VTSema)
   {
      return(miPolyText8(pDraw, pGC, x, y, count, chars));
   }

   if ((fentry = agxCacheFont8(pGC->font)) == NULL)
       return agxNoCPolyText(pDraw, pGC, x, y, count, chars, TRUE, FALSE );
   else
       return agxCPolyText8(pDraw, pGC, x, y, count, chars, fentry, FALSE);
}

void
agxImageText8(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x, y;
     int   count;
     char *chars;
{
   CacheFont8Ptr fentry;

   if (!xf86VTSema)
   {
      miImageText8(pDraw, pGC, x, y, count, chars);
      return;
   } 

   if ((fentry = agxCacheFont8(pGC->font)) == NULL)
      agxNoCPolyText(pDraw, pGC, x, y, count, chars, TRUE, TRUE);
   else
      agxCPolyText8(pDraw, pGC, x, y, count, chars, fentry, TRUE);
}

int
agxPolyText16(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x, y;
     int   count;
     unsigned short *chars;
{
   CacheFont8Ptr ret;

   if (!xf86VTSema)
   {
      return(miPolyText16(pDraw, pGC, x, y, count, chars));
   }

   return agxNoCPolyText(pDraw, pGC, x, y, count, (char *)chars, FALSE, FALSE);
}

void
agxImageText16(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x, y;
     int   count;
     unsigned short *chars;
{
   CacheFont8Ptr ret;

   if (!xf86VTSema)
   {
      miImageText16(pDraw, pGC, x, y, count, chars);
      return;
   } 

   agxNoCPolyText(pDraw, pGC, x, y, count, (char *)chars, FALSE, TRUE);
}

int
agxNoCPolyText(pDraw, pGC, x, y, count, chars, is8bit, opaque)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x;
     int   y;
     int   count;
     char *chars;
     Bool  is8bit;
     int   opaque;
{
   register BoxPtr pBox;
   int   numRects;
   int   block;
   RegionPtr pRegion;
   int   maxAscent, maxDescent;
   int   minX, maxX, minY, maxY;
   unsigned long n;
   int width = 0;
   int ret_x;
   unsigned int mapDim, mapCoOrd;
   xRectangle backrect;
   FontPtr pfont = pGC->font;
#if 0
   Bool terminalFont = pfont->info.terminalFont;
   Bool noVertOverlap = FALSE;
#endif
   Bool first = TRUE;
   unsigned int backDim; 
   unsigned int backCoOrd;
   CharInfoPtr *charinfo;
   int  maxPadGlyWidth, maxGlyHeight;
   int  scrPadWidth;

   maxAscent = FONTMAXBOUNDS(pfont, ascent);
   maxDescent = FONTMAXBOUNDS(pfont, descent);
   maxPadGlyWidth = PixmapBytePad( FONTMAXBOUNDS(pfont, rightSideBearing) 
                                   - FONTMINBOUNDS(pfont, leftSideBearing), 1 );
   maxGlyHeight   = maxAscent + maxDescent; 
   scrPadWidth    = agxBytePadScratchMapPow2( maxPadGlyWidth, 0 );

   if( maxGlyHeight * scrPadWidth > agxScratchSize ) {
      /* largest glyph won't fit, let mi routines handle it */
      if( opaque )
         if( is8bit )
            miImageText8(pDraw, pGC, x, y, count, chars );
         else
            miImageText16(pDraw, pGC, x, y, count, (unsigned short *)chars );
      else
         if( is8bit )
            return miPolyText8(pDraw, pGC, x, y, count, chars );
         else
            return miPolyText16(pDraw, pGC, x, y, count, (unsigned short *)chars );
      return;
   }

   if( !(charinfo = (CharInfoPtr *)ALLOCATE_LOCAL(count*sizeof(CharInfoPtr)) ) )
        return x ;

   if (is8bit)
      GetGlyphs(pfont, (unsigned long)count, (unsigned char *)chars,
              Linear8Bit, &n, charinfo);
   else
      GetGlyphs(pfont, (unsigned long)count, (unsigned char *)chars,
                (FONTLASTROW(pGC->font) == 0) ? Linear16Bit : TwoD16Bit,
                &n, charinfo);
  
   /*
    * If miPolyText8() is to be believed, the returned new X value is
    * completely independent of what happens during rendering.
    */
   ret_x = x;
   if (pfont->info.constantWidth) {
      width = pfont->info.maxbounds.characterWidth;
      width *= n;
   } 
   else {
      register int i = 0;
      register char *ch = chars;
      for (; i < n; i++, ch++) {
         width += charinfo[i]->metrics.characterWidth; 
      }
   }
   ret_x += width;

   if (n == 0) {
      DEALLOCATE_LOCAL(charinfo);
      return ret_x;
   }

   x += pDraw->x;
   y += pDraw->y;
   minY = y - maxAscent;
   maxY = y + maxDescent;

   if( opaque ) {
      if (width >= 0) {
         backrect.x =  x;
         backrect.width = width;
      } 
      else {
         backrect.x = x + width;
         -width;
         backrect.width = width;
      }
   }
   else {
      if (width < 0) {
         -width;
      }
   }

   maxX = x + width;
   minX = x + FONTMINBOUNDS(pfont, leftSideBearing);

   if( opaque ) {
      backrect.y = y - FONTASCENT(pfont);
      backrect.height = FONTASCENT(pfont) + FONTDESCENT(pfont); 
#if 0
      noVertOverlap = backrect.y == minY
                      && backrect.height == (maxY - minY);
#endif
      backDim = (backrect.height-1)<< 16 | backrect.width-1; 
      backCoOrd = backrect.y << 16 | backrect.x;
   }

   pRegion = ((cfbPrivGC *) 
                 (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;

   pBox = REGION_RECTS(pRegion);
   numRects = REGION_NUM_RECTS(pRegion);
   while (numRects && pBox->y2 <= minY ) {
      ++pBox;
      --numRects;
   }
   if (!numRects || pBox->y1 >= maxY ) {
      DEALLOCATE_LOCAL(charinfo);
      return ret_x;
   }
   while (numRects && (pBox->x1 >= maxX || pBox->x2 <= minX )) {
      ++pBox;
      --numRects;
   }

   for (; numRects-- > 0; ++pBox) {
      unsigned short mixes;

      if (pBox->x1 < maxX && pBox->x2 > minX 
         && pBox->y1 < maxY && pBox->y2 > minY ) {
         /* mask off clipped areas of the destination */
         register unsigned int mapCoOrd = (pBox->y1 << 16) | pBox->x1;
         register unsigned int mapDim   = ((pBox->y2 - pBox->y1)-1) << 16 
                                          | ((pBox->x2 - pBox->x1)-1);
         if (first) {
            first = FALSE;
            MAP_INIT( GE_MS_MAP_B,
                      GE_MF_1BPP | GE_MF_INTEL_FORMAT,
                      agxMemBase + agxScratchOffset,
                      (scrPadWidth<<3)-1,  /* padded width in pixels */
                      maxGlyHeight-1,
                      FALSE, FALSE, FALSE );
            GE_WAIT_IDLE_SHORT();
            GE_SET_MAP( GE_MS_MAP_B );
            MAP_SET_DST( GE_MS_MAP_A );
            GE_OUT_D( GE_PIXEL_BIT_MASK, pGC->planemask );
            GE_OUT_D( GE_FRGD_CLR, pGC->fgPixel );
            GE_OUT_D( GE_BKGD_CLR, pGC->bgPixel );
            if (opaque) {
#if 0
               /* opaque stipples are faster, so if possible: opaque it */ 
               if (terminalFont && noVertOverlap) 
                  mixes = MIX_SRC << 8 | MIX_SRC; 
               else
#endif
                  mixes = MIX_DST << 8 | MIX_SRC; 
            }
            else {
               mixes = MIX_DST << 8 | pGC->alu; 
            }
            GE_OUT_W( GE_FRGD_MIX, mixes );
            GE_OUT_B( GE_PIXEL_MAP_SEL, GE_MS_MASK_MAP );
            GE_OUT_D( GE_MASK_MAP_X, mapCoOrd );
            GE_OUT_D( GE_PIXEL_MAP_WIDTH, mapDim ); 
         }
         else {
            GE_WAIT_IDLE_SHORT();
            GE_OUT_B( GE_PIXEL_MAP_SEL, GE_MS_MASK_MAP );
            GE_OUT_D( GE_MASK_MAP_X, mapCoOrd );
            GE_OUT_D( GE_PIXEL_MAP_WIDTH, mapDim ); 
         }
         if ( opaque 
#if 0
              && !(terminalFont && noVertOverlap) 
#endif
            ) {
               /* have to seperate the opaque from the character draw */
               GE_OUT_D( GE_FRGD_CLR, pGC->bgPixel );
               GE_OUT_D( GE_DEST_MAP_X, backCoOrd );
               GE_OUT_D( GE_OP_DIM_WIDTH, backDim );
               GE_START_CMD( GE_OP_BITBLT
                             | GE_OP_PAT_FRGD
                             | GE_OP_MASK_BOUNDARY
                             | GE_OP_INC_X
                             | GE_OP_INC_Y
                             | GE_OP_FRGD_SRC_CLR
                             | GE_OP_DEST_MAP_A   );
               GE_WAIT_IDLE_SHORT(); 
               GE_OUT_D( GE_FRGD_CLR, pGC->fgPixel );
         }
         DoagxPolyGlyphBlt( pGC, x, y, n, charinfo, scrPadWidth );
      }
   }
   DEALLOCATE_LOCAL(charinfo);
   return ret_x;
}


static __inline__ void
DoagxPolyGlyphBlt(pGC, x, y, count, ppci, scrW )
     GCPtr pGC; 
     int   x, y; 
     unsigned int count;
     CharInfoPtr   *ppci;
     unsigned int  scrW;
{
   unsigned char *pgBase = FONTGLYPHS(pGC->font);

   GE_OUT_W( GE_PIXEL_OP,
             GE_OP_PAT_MAP_B
             | GE_OP_MASK_BOUNDARY 
             | GE_OP_INC_X
             | GE_OP_INC_Y         );

   for (;count > 0; count--, ppci++) {
      register CharInfoPtr pci;
      CharInfoPtr pciLast = NULL;
      register unsigned int gh, gw;
      pci = *ppci;
      if ( (gh = GLYPHHEIGHTPIXELS(pci)) > 0 
           && (gw = GLYPHWIDTHPIXELS(pci)) > 0 ) {
         register unsigned int dstCoOrd;
         register unsigned int opDim;

         dstCoOrd = (y - pci->metrics.ascent) << 16
                    | (x + pci->metrics.leftSideBearing);

         if( pci != pciLast ) {
            register char *glyphBase;
            register int   glyphWidth; 

            glyphBase = (char*)FONTGLYPHBITS(pgBase, pci);
            glyphWidth = GLYPHWIDTHBYTESPADDED(pci);
            opDim = (gh-1) << 16 | (gw-1);

            GE_WAIT_IDLE_SHORT();
            agxPartMemToVid( agxScratchOffset, scrW,
                             glyphBase, glyphWidth,
                             gw, gh );
          }
          else {
            GE_WAIT_IDLE_SHORT();
          }

          GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
          GE_OUT_D( GE_PAT_MAP_X, 0 ); 
          GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
          GE_START_CMDW( GE_OPW_BITBLT
                         | GE_OPW_FRGD_SRC_CLR 
                         | GE_OPW_BKGD_SRC_CLR 
                         | GE_OPW_DEST_MAP_A   );
          pciLast = pci;
      }
      x += pci->metrics.characterWidth;
   }

   GE_WAIT_IDLE_EXIT();
   return;
}

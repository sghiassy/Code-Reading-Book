/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxFCach.c,v 3.19 1996/12/23 06:32:38 dawes Exp $ */
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
 * KEVIN E. MARTIN AND HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH REGARD 
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Modified by Amancio Hasty and Jon Tombs
 * Rewritten for the AGX by Henry A. Worth (haw30@eng.amdahl.com
 * 
 */
/* $XConsortium: agxFCach.c /main/11 1996/10/22 10:33:24 kaleb $ */

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
#include	"agx.h"
#include	"regagx.h"
#include        "agxBCach.h"

extern CacheFont8Ptr agxHeadFont;
unsigned long agxFontAge;
#define NEXT_FONT_AGE  ++agxFontAge

static void agxloadFontBlock();
static __inline__ void DoagxCPolyText8();
static __inline__ void DoagxConstMetrics();
extern CacheFont8Ptr agxFontCache;

Bool geBlockMove = FALSE;
bitMapBlockPtr  blockInUse = NULL;

void
agxUnCacheFont8(font)
     FontPtr font;
{
   int   i;
   CacheFont8Ptr ptr, last;

   ERROR_F(("UnCach 0x%x\n", font));
   last = agxHeadFont;
   for (ptr = agxHeadFont; ptr != NULL; ptr = ptr->next) {      
      if (ptr->font == font) {
	 for (i = 0; i < BLOCKS_PER_FONT; i++) 
	    if (ptr->fblock[i] != NULL) {
	        agxCReturnBlock(ptr->fblock[i]);
	    }

	 if (ptr != agxHeadFont) {
	    last->next = ptr->next;
	    xfree(ptr);
	 }
         else {
	    if (ptr->next != NULL) { /* move the head down */
	       agxHeadFont=ptr->next;
	       xfree(ptr);		  
	    }
            else { /* one and only entry */
	       agxHeadFont->font = NULL;
	    }
         }
#ifdef DEBUG_FCACHE
         for (ptr = agxHeadFont; ptr != NULL; ptr = ptr->next)
	    ErrorF("fonts 0x%x\n", ptr->font);
#endif	       
      	 return;	 
      }
      last=ptr;
   }
}

CacheFont8Ptr
agxCacheFont8(font)
     FontPtr font;
{
   int   c;
   unsigned long n;
   unsigned char chr;
   int   width, height;
   int   bWidth;
   int   blockSize;
   int   gper;
   CharInfoPtr pci;

   CacheFont8Ptr last, ret = agxHeadFont;

   if( agxHeadFont == NULL )
      return NULL;

   while (ret != NULL) {
      if (ret->font == font)
	 return ret;
      last = ret;	 
      ret = ret->next;
   } 

   width = FONTMAXBOUNDS(font, rightSideBearing) 
            - FONTMINBOUNDS(font, leftSideBearing);
   height = FONTMAXBOUNDS(font, ascent) 
             + FONTMAXBOUNDS(font, descent);

   bWidth = PixmapBytePad(width,1); 
   gper = CACHE_LINE_WIDTH_BYTES / bWidth;
   blockSize =  (((BLOCK_NUM_CHAR - 1) / gper) + 1) * height;

   if ( (width > CACHE_LINE_WIDTH_PIXELS) 
        || (FONTFIRSTROW(font) != 0) 
        || (FONTLASTROW(font) != 0)
        || (FONTLASTCOL(font) > 255)  
        || blockSize > ROW_NUM_LINES )
       return NULL;

   if (agxHeadFont->font == NULL)
       ret = agxHeadFont;
   else
       ret = (CacheFont8Ptr) xcalloc(1,sizeof(CacheFont8Rec));
   if (ret == NULL)
      return NULL;

   ret->wPix = width;
   ret->hPix = height;
   ret->wBytes = bWidth;
   ret->gper = gper;
   ret->blockSize = blockSize;
   ret->font = font;

   /*
    * We load all the font infos now, and the fonts themselves are demand
    * loaded into the cache as 32 font bitmaps. This way we can load alot
    * more things into cache at the expense of the cache management.
    */

   for (c = 0; c < 256; c++) {
      chr = (unsigned char)c;
      GetGlyphs(font, 1, &chr, Linear8Bit, &n, &pci);
      if (n == 0) {
	 ret->pci[c] = NULL;
      }
      else {
	 ret->pci[c] = pci;
      }
   }
   if (ret != agxHeadFont)
      last->next = ret;

   return ret;
}

static void
agxloadFontBlock(fentry, block)
     CacheFont8Ptr fentry;
     int   block;
{
   unsigned int   i, j, c;
   unsigned char  chr;
   unsigned int   nbyWidth;
   unsigned char *pb, *pbits;
   unsigned char *pglyph;
   unsigned int   gWidth, gHeight, gSize;
   unsigned int   nbyGlyphWidth;
   unsigned int   nbyPadGlyph;

   ERROR_F(("loading 0x%x (0x%x) 0x%x\n", 
             fentry->font, block, fentry->fblock[block]));


   nbyWidth = fentry->wBytes;  		/* glyph width in bytes */
   gSize = nbyWidth * fentry->hPix;  	 /*  font cache glyph size in bytes */

   pbits = (unsigned char *)ALLOCATE_LOCAL(gSize);   /* buffer for copy */

   if ( pbits != NULL 
        && (fentry->fblock[block]
             = agxCGetBlock( fentry,
                             block,
                             fentry->blockSize )) != NULL ) {

      unsigned int first = block << BLOCK_NUM_SHIFT;  /* first char in block */
      unsigned int last  = first + BLOCK_NUM_CHAR;    /* last+1 char in block */

      /* for each character in the block */
      for ( c = first; c < last; c++ ) {

	 if (fentry->pci[c] != NULL) {

	    pglyph = FONTGLYPHBITS(pglyphBase, fentry->pci[c]);
	    gWidth = GLYPHWIDTHPIXELS(fentry->pci[c]);
	    gHeight = GLYPHHEIGHTPIXELS(fentry->pci[c]);
	    if (gWidth && gHeight) {
	       nbyGlyphWidth = GLYPHWIDTHBYTESPADDED(fentry->pci[c]);
               nbyPadGlyph = PixmapBytePad(gWidth, 1);

	       if ( nbyWidth == nbyPadGlyph
#if GLYPHPADBYTES != 4
		    && (((int)pglyph) & 3) == 0
#endif
		  ) {
                  /*
                   * glyph is in acceptable format, no need to copy 
                   */
		  pb = pglyph;
               }
	       else
               {
                  unsigned char *pg;
                  /*
                   * copy glyph into pbits, padded to font w 
                   * This could be optimized to use word and dword moves.
                   * Also should optimize to build a list of pointers 
                   * to the glyphs and use those to eliminate the 
                   * buffer and allow benchmarking different load
                   * sequences.
                   */
		  pb = pbits;
		  for ( i = 0; i < gHeight; i++ ) {
                     pb = pbits + (i * nbyWidth);
                     pg = pglyph + (i * nbyGlyphWidth);
		     for (j = 0; 
                          j < nbyGlyphWidth && j < nbyWidth;
                          j++)
			*pb++ = *pg++;
                     while ( j++ < nbyWidth )
                        *pb++ = 0x00;
                  }
		  pb = pbits;
	       }
               /* 
                * Copy into appropriate position in the cache block.
                * Note: unused glyph rows in the glyph will be 
                *       garbage in the cache. 
                */
               {  
                  unsigned int  vidBase;
                  unsigned int  offset;
                  unsigned int  idx;
                  unsigned int  linePos;
                  unsigned int  lineOff;
                  unsigned char bank;
                  pointer  base;

                  idx     = (c & BLOCK_IDX_MASK); 
                  linePos = idx % fentry->gper;
                  lineOff = (idx / fentry->gper) * fentry->hPix; 
                  vidBase = fentry->fblock[block]->daddy->offset 
                            + ( (fentry->fblock[block]->line + lineOff)
                                * CACHE_LINE_WIDTH_BYTES )
                            + ( linePos * nbyWidth );
                  
                  bank = (vidBase >> 16) & 0x3F ;
                  base = (pointer)((unsigned char *)agxVideoMem +
				   (vidBase & 0xFFFF));

                  /* cache is laid out so that blocks don't cross banks */ 
                  outb( agxApIdxReg, bank );
                  for( i=0, offset = 0; i < gHeight; i++ ) {
                     MemToBus( (void *)((unsigned char *)base+offset), 
                               pb,
			       nbyWidth );
                     offset += CACHE_LINE_WIDTH_BYTES;
                     pb+=nbyWidth;
                  }
               }
	    }
	 }
      }
      DEALLOCATE_LOCAL(pbits);
   } else {
      CacheFont8Ptr fptr;
      Bool found = FALSE;
   /*
    * If we get here we are in deep trouble, half way through printing a
    * string we have been unable to load a font block into the cache, the
    * get Block function found no block of the right size, this can occur
    * if the font is filled with smaller fonts, requiring the flushing
    * of multiple blocks in a row to make room.
    *
    * Need to flush smaller lru blocks, but for now try flushing    
    * one font at a time, and if that fails everything.
    *
    */

      if (pbits) DEALLOCATE_LOCAL(pbits);
      for (fptr = agxHeadFont; fptr != NULL; fptr= fptr->next)
	 if (fptr != fentry) {
            ERROR_F(("Flushing Font!\n"));
	    for (i = 0; i < BLOCKS_PER_FONT; i++) {
	       if (fptr->fblock[i] != NULL) {
	         agxCReturnBlock(fptr->fblock[i]);
                 found = TRUE; 
	       }
          }
           if (found)
              break;
	 }

      /* getting real desperate - this doesn't work with pre-loading */
      if (!found) { 
         ERROR_F(("Flushing Current Font!\n"));
	 for (i = 0; i < BLOCKS_PER_FONT; i++)
 	    if (fentry->fblock[i] != NULL) 
	       agxCReturnBlock(fentry->fblock[i]);	    
      }
      agxloadFontBlock(fentry, block);
      return;
   }
   for (i = 0; i < BLOCKS_PER_FONT; i++)
      ERROR_F(("got 0x%x(0x%x) 0x%x\n", fentry->font, i, fentry->fblock[i]));
}

int
agxCPolyText8(pDraw, pGC, x, y, count, chars, fentry, opaque)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x;
     int   y;
     int   count;
     unsigned char *chars;
     CacheFont8Ptr fentry;
     int   opaque;
{
   register BoxPtr pBox;
   int   numRects;
   int   block;
   RegionPtr pRegion;
   int   maxAscent, maxDescent;
   int   minX, maxX, minY, maxY;
   FontPtr pfont = pGC->font;
   int width = 0;
   int ret_x;
   unsigned int mapDim, mapCoOrd;
   char  toload[BLOCKS_PER_FONT];
   xRectangle backrect;
#ifndef NOFONTCONSTMETRIC
   Bool terminalFont = fentry->font->info.terminalFont;
   Bool constantMetrics = fentry->font->info.constantMetrics;
   Bool noVertOverlap = FALSE;
#endif
   Bool first = TRUE;
   unsigned int backDim; 
   unsigned int backCoOrd;

   ret_x = x;

   /*
    * If miPolyText8() is to be believed, the returned new X value is
    * completely independent of what happens during rendering.
    */

   if (fentry->font->info.constantWidth) {
      width = fentry->font->info.maxbounds.characterWidth;
      width *= count;
   } 
   else {
      register int i = 0;
      register unsigned char *ch = chars;
      for (; i < count; i++, ch++) {
         register CharInfoPtr info = fentry->pci[*ch];
         if (info) {
            width += info->metrics.characterWidth; 
         }
      }
   }
   ret_x += width;
   x += pDraw->x;
   y += pDraw->y;
   maxAscent = FONTMAXBOUNDS(pfont, ascent);
   maxDescent = FONTMAXBOUNDS(pfont, descent);
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
#ifndef NOFONTCONSTMETRIC
      noVertOverlap = backrect.y == minY
                      && backrect.height == (maxY - minY);
#endif
      backDim = (backrect.height-1)<< 16 | backrect.width-1; 
      backCoOrd = backrect.y << 16 | backrect.x;
   }

   /* since GE may be busy, preload first block if needed */
   block = (*chars >> BLOCK_NUM_SHIFT) & BLOCK_IDX_MASK;
   if (fentry->fblock[block] == NULL)
      agxloadFontBlock(fentry, block);

   pRegion = ((cfbPrivGC *) 
                 (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;

   pBox = REGION_RECTS(pRegion);
   numRects = REGION_NUM_RECTS(pRegion);
   while (numRects && pBox->y2 <= minY ) {
      ++pBox;
      --numRects;
   }
   if (!numRects || pBox->y1 >= maxY )
      return ret_x;
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
            GE_WAIT_IDLE_SHORT();
            GE_OUT_B( GE_PIXEL_MAP_SEL, GE_MS_MAP_C );
            GE_OUT_D( GE_PIXEL_MAP_WIDTH, ROW_NUM_LINES-1 << 16
                                          | CACHE_LINE_WIDTH_PIXELS-1 );
            GE_OUT_B( GE_PIXEL_MAP_FORMAT, GE_MF_1BPP | GE_MF_INTEL_FORMAT );
            MAP_SET_DST( GE_MS_MAP_A );
            GE_OUT_D( GE_PIXEL_BIT_MASK, pGC->planemask );
            GE_OUT_D( GE_FRGD_CLR, pGC->fgPixel );
            GE_OUT_D( GE_BKGD_CLR, pGC->bgPixel );
            if (opaque) {
               /* opaque stipples are faster, so if terminalFont: opaque it */ 
#ifndef NOFONTCONSTMETRIC
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
         if (opaque) {
#ifndef NOFONTCONSTMETRIC
           if (terminalFont && noVertOverlap) {
               /* we can do just an opaque stipple */
               DoagxConstMetrics( x, y, count, chars, 
                                  fentry, pGC, maxAscent, TRUE, mixes );
           }
           else 
#endif
           {
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

#ifndef NOFONTCONSTMETRIC
              if( constantMetrics ) {
                 GE_WAIT_IDLE_SHORT(); 
                 GE_OUT_D( GE_FRGD_CLR, pGC->fgPixel );
                 DoagxConstMetrics( x, y, count, chars, 
                                    fentry, pGC, maxAscent, FALSE, mixes );
              }
              else
#endif
	         DoagxCPolyText8( x, y, count, chars, fentry,
		                  pGC, TRUE, mixes );
           }
         }
         else {
#ifndef NOFONTCONSTMETRIC
              if( constantMetrics ) {
                 DoagxConstMetrics( x, y, count, chars,
                                    fentry, pGC, maxAscent, FALSE, mixes );
              }
              else
#endif
	         DoagxCPolyText8( x, y, count, chars, fentry,
		                  pGC, FALSE, mixes );
         }
      }
   }
   return ret_x;
}


static __inline__ void
DoagxCPolyText8(x, y, count, chars, fentry, pGC, opaque, mixes)
     int   x, y, count;
     unsigned char *chars;
     CacheFont8Ptr fentry;
     GCPtr pGC; 
     int           opaque;
     unsigned int  mixes;
{
   register unsigned int dstCoOrd, patCoOrd;
   register unsigned int opDim;
   register CharInfoPtr pci;
   register unsigned int idx;
   unsigned int h = fentry->hPix;
   unsigned int w = fentry->wBytes<<3;
   unsigned int opHeight;
   CharInfoPtr pciLast = NULL;
   unsigned int blocki = 0xFFFFFFFF;
   bitMapBlockPtr block;
   unsigned int blockBase;
   unsigned int oldBlockBase = 0;

   if (opaque) {
      GE_WAIT_IDLE_SHORT(); 
      GE_OUT_D( GE_FRGD_CLR, pGC->fgPixel );
   }
      
   GE_OUT_W( GE_PIXEL_OP,
             GE_OP_PAT_MAP_C
             | GE_OP_MASK_BOUNDARY 
             | GE_OP_INC_X
             | GE_OP_INC_Y         );

   for (;count > 0; count--, chars++) {
      if ( (pci = fentry->pci[(int)*chars]) != NULL ) {
         if ( (opHeight = GLYPHHEIGHTPIXELS(pci)) > 0 ) {
	    if ((int)(*chars >> BLOCK_NUM_SHIFT) != blocki) {
	       blocki = *chars >> BLOCK_NUM_SHIFT;
	       block = fentry->fblock[blocki];
	       if (block == NULL) {
                  geBlockMove = FALSE;
		  agxloadFontBlock(fentry, blocki);
		  block = fentry->fblock[blocki];
                  if( geBlockMove ) {
                     GE_WAIT_IDLE_SHORT();
                     MAP_SET_DST( GE_MS_MAP_A );
                     GE_OUT_W(GE_FRGD_MIX, mixes);
                     GE_OUT_D( GE_FRGD_CLR, pGC->fgPixel );
                     GE_OUT_D( GE_BKGD_CLR, pGC->bgPixel );
                     GE_OUT_D(GE_PIXEL_BIT_MASK, pGC->planemask);
                     GE_OUT_W( GE_PIXEL_OP,
                               GE_OP_PAT_MAP_C
                               | GE_OP_MASK_BOUNDARY 
                               | GE_OP_INC_X
                               | GE_OP_INC_Y         );
                  }
	       }
               blockInUse = block;
               block->lru = NEXT_FONT_AGE;
               blockBase = agxMemBase + block->daddy->offset;
               if( blockBase != oldBlockBase ) {
                  GE_WAIT_IDLE_SHORT();
                  GE_OUT_B( GE_PIXEL_MAP_SEL, GE_MS_MAP_C );
                  GE_OUT_D( GE_PIXEL_MAP_BASE, blockBase );
                  oldBlockBase = blockBase;
               }
               pciLast = NULL;
   	    }

            if( pci != pciLast ) {
               pciLast = pci;
               opDim = (opHeight-1) << 16 | (GLYPHWIDTHPIXELS(pci)-1);
               idx =  (*chars) & BLOCK_IDX_MASK;
               patCoOrd = (block->line + ((idx / fentry->gper) * h)) << 16
                          | (w * (idx % fentry->gper));
            }
            dstCoOrd = (y - pci->metrics.ascent) << 16
                       | (x + pci->metrics.leftSideBearing);
            {
               GE_WAIT_IDLE_SHORT();
               GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
               GE_OUT_D( GE_PAT_MAP_X, patCoOrd ); 
               GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
               GE_START_CMDW( GE_OPW_BITBLT
                              | GE_OPW_FRGD_SRC_CLR 
                              | GE_OPW_BKGD_SRC_CLR 
                              | GE_OPW_DEST_MAP_A   );
            }
	 }
         x += pci->metrics.characterWidth;
      }
   }

   GE_WAIT_IDLE_EXIT();
   return;
}

#ifndef NOFONTCONSTMETRIC
static __inline__ void
DoagxConstMetrics(x, y, count, chars, fentry, pGC, maxAscent, opaque, mixes )
     int   x, y, count;
     unsigned char *chars;
     CacheFont8Ptr fentry;
     GCPtr pGC; 
     int   maxAscent;
     Bool  opaque;
     unsigned int  mixes;
{
   register unsigned int dstCoOrd, patCoOrd;
   register CharInfoPtr pci = fentry->pci[(int)*chars];
   register unsigned int idx;
   unsigned int h = fentry->hPix;
   unsigned int w = fentry->wBytes<<3;
   unsigned int width = fentry->font->info.maxbounds.characterWidth;
   int leftBear  = fentry->font->info.maxbounds.leftSideBearing;
   int rightBear = fentry->font->info.maxbounds.rightSideBearing;
   unsigned int opDim = (h-1) << 16 | ((rightBear-leftBear)-1);
   CharInfoPtr pciLast = NULL;
   unsigned int blocki = 0xFFFFFFFF;
   bitMapBlockPtr block;
   unsigned int blockBase;
   unsigned int oldBlockBase = 0;

   y = (y - maxAscent) << 16;

   /* no GE command is running so no need to wait */     
   GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
   GE_OUT_W( GE_PIXEL_OP,
             GE_OP_PAT_MAP_C
             | GE_OP_MASK_BOUNDARY 
             | GE_OP_INC_X
             | GE_OP_INC_Y         );

   for (;count > 0; count--, chars++) {
      if ( (pci = fentry->pci[(int)*chars]) != NULL ) {
	    if ((int)(*chars >> BLOCK_NUM_SHIFT) != blocki) {
	       blocki = *chars >> BLOCK_NUM_SHIFT;
	       block = fentry->fblock[blocki];
	       if (block == NULL) {
                  geBlockMove = FALSE;
		  agxloadFontBlock(fentry, blocki);
		  block = fentry->fblock[blocki];
                  if( geBlockMove ) {
                     /* GE was used -- restore needed state */
                     GE_WAIT_IDLE_SHORT();
                     MAP_SET_DST( GE_MS_MAP_A );
                     GE_OUT_W(GE_FRGD_MIX, mixes );
                     GE_OUT_W( GE_PIXEL_OP,
                               GE_OP_PAT_MAP_C
                               | GE_OP_MASK_BOUNDARY 
                               | GE_OP_INC_X
                               | GE_OP_INC_Y         );
                     GE_OUT_D( GE_FRGD_CLR, pGC->fgPixel );
                     GE_OUT_D( GE_BKGD_CLR, pGC->bgPixel );
                     GE_OUT_D( GE_PIXEL_BIT_MASK, pGC->planemask );
                     GE_OUT_D( GE_OP_DIM_WIDTH, opDim );

                  }
	       }
               blockInUse = block;
               block->lru = NEXT_FONT_AGE;
               blockBase = agxMemBase + block->daddy->offset;
               if( blockBase != oldBlockBase ) {
                  GE_WAIT_IDLE_SHORT();
                  GE_OUT_B( GE_PIXEL_MAP_SEL, GE_MS_MAP_C );
                  GE_OUT_D( GE_PIXEL_MAP_BASE, blockBase );
                  oldBlockBase = blockBase;
               }
   	    }
            if( pci != pciLast ) {
               pciLast = pci;
               idx =  (*chars) & BLOCK_IDX_MASK;
               patCoOrd = (block->line + ((idx / fentry->gper) * h)) << 16
                          | (w * (idx % fentry->gper));
            }
            dstCoOrd = y | (x + leftBear);

            GE_WAIT_IDLE_SHORT();
            GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
            GE_OUT_D( GE_PAT_MAP_X, patCoOrd ); 
            GE_START_CMDW( GE_OPW_BITBLT
                           | GE_OPW_FRGD_SRC_CLR 
                           | GE_OPW_BKGD_SRC_CLR 
                           | GE_OPW_DEST_MAP_A   );

	 }
         else if( opaque ) {
            /* shouldn't happen - but just in case */
            pciLast = NULL;
            dstCoOrd = y | x;
            GE_WAIT_IDLE_SHORT();
            GE_OUT_W( GE_FRGD_CLR, pGC->bgPixel );
            GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
            GE_START_CMD( GE_OP_BITBLT
                          | GE_OP_PAT_FRGD
                          | GE_OP_MASK_BOUNDARY 
                          | GE_OP_INC_X
                          | GE_OP_INC_Y
                          | GE_OP_FRGD_SRC_CLR
                          | GE_OP_DEST_MAP_A   );
            GE_WAIT_IDLE();
            GE_OUT_W( GE_FRGD_CLR, pGC->fgPixel );
            GE_OUT_W( GE_PIXEL_OP,
                      GE_OP_PAT_MAP_C
                      | GE_OP_MASK_BOUNDARY
                      | GE_OP_INC_X
                      | GE_OP_INC_Y         );
         }
         x += width;
   }

   GE_WAIT_IDLE_EXIT();
   return;
}
#endif

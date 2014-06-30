/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/cache/xf86fcache.c,v 3.11 1996/12/23 06:33:20 dawes Exp $ */
/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 * KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 */
/* $XConsortium: xf86fcache.c /main/11 1996/10/22 10:33:28 kaleb $ */

/*
 * Extracted from s3fcach.c and adapted to XFree86 in X11R6 by
 * Hans Nasten. ( nasten@everyware.se ).
 */

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
#include	"xf86bcache.h"
#include	"xf86fcache.h"

static CacheFont8Ptr xf86HeadFont = NULL;
static CachePool xf86FontPool = NULL;
static int xf86MaxWidth;
static int xf86MaxHeight;
static void (*xf86FontOpStippleFunc)(
#if NeedFunctionPrototypes
int, int, int, int, unsigned char *, int, Pixel
#endif
);

/*
 * Init the font cache.
 * The Cache Pool pointer is stored together with  various other information
 */

void 
#if NeedFunctionPrototypes
xf86InitFontCache(
    CachePool FontCache,
    int MaxWidth, 
    int MaxHeight,
    void (*FontOpStippleFunc)(
#if NeedNestedPrototypes
	int, int, int, int, unsigned char *, int, Pixel
#endif
    )
)
#else
xf86InitFontCache( FontCache, MaxWidth, MaxHeight, FontOpStippleFunc )
    CachePool FontCache;
    int MaxWidth, MaxHeight;
    void (*FontOpStippleFunc)();
#endif

{
    xf86FontPool = FontCache;
    xf86MaxWidth = MaxWidth / 32;
    xf86MaxHeight = MaxHeight;
    xf86HeadFont = (CacheFont8Ptr) xcalloc(1,sizeof(CacheFont8Rec));
    xf86FontOpStippleFunc = FontOpStippleFunc;
}

/*
 * Release all cache blocks to the block allocator.
 */
void xf86ReleaseFontCache()
{
  CacheFont8Ptr CFptr;
  int i;

    /*
     * The screen has been destroyed so we must go through
     * all cached font blocks and return them to the cache
     */
    ERROR_F(("Releasing font cache.\n"));
    for (CFptr = xf86HeadFont; CFptr != NULL; CFptr=CFptr->next) {
      if (CFptr->font) {
        for (i = 0; i < 8; i++) {
          if (CFptr->fblock[i] != NULL)
	    xf86ReleaseToCachePool( xf86FontPool, CFptr->fblock[i] );

        }
      }
    }

}

/*
 * Remove a font from the font cache.
 */
void
xf86UnCacheFont8(font)
     FontPtr font;
{
   int   i;
   CacheFont8Ptr ptr, last;

   ERROR_F(("UnCach %d\n", font));
   last = xf86HeadFont;
   for (ptr = xf86HeadFont; ptr != NULL; ptr = ptr->next) {      
      if (ptr->font == font) {
	 for (i = 0; i < 8; i++) 
	    if (ptr->fblock[i] != NULL)
	        xf86ReleaseToCachePool( xf86FontPool, ptr->fblock[i] );

	 if (ptr != xf86HeadFont) {
	    last->next = ptr->next;
	    xfree(ptr);
	 } else {
	    if (ptr->next != NULL) { /* move the head down */
	       xf86HeadFont=ptr->next;
	       xfree(ptr);		  
	    } else { /* one and only entry */
	       xf86HeadFont->font = NULL;
	    }
	 }
#ifdef DEBUG_FCACHE
         for (ptr = xf86HeadFont; ptr != NULL; ptr = ptr->next)
	    ErrorF("fonts %d\n", ptr->font);
#endif	       
      	 return;	 
      }
      last=ptr;
    }
}

/*
 * Add a new font to the font cache.
 */
CacheFont8Ptr
xf86CacheFont8(font)
     FontPtr font;
{
   int   c;
   unsigned long n;
   unsigned char chr;
   int   width, height;
   CharInfoPtr pci;
   CacheFont8Ptr last, ret = xf86HeadFont;


   if( !xf86FontPool )	/* No font cache available */
      return( NULL );

   while (ret != NULL) {
      if (ret->font == font)
	 return ret;
      last = ret;	 
      ret = ret->next;
   } 

   width = FONTMAXBOUNDS(font, rightSideBearing) -
      FONTMINBOUNDS(font, leftSideBearing);
   height = FONTMAXBOUNDS(font, ascent) + FONTMAXBOUNDS(font, descent);

   if ((width > xf86MaxWidth) || (height > xf86MaxHeight) ||
       (FONTFIRSTROW(font) != 0) || (FONTLASTROW(font) != 0) ||
       (FONTLASTCOL(font) > 255))
       return NULL;

   if (xf86HeadFont->font == NULL)
     ret = xf86HeadFont;
   else
     ret = (CacheFont8Ptr) xcalloc(1,sizeof(CacheFont8Rec));

   if (ret == NULL)
     return NULL;

   ret->w = width;
   ret->h = height;
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
     } else {
       ret->pci[c] = pci;
     }
   }
   if (ret != xf86HeadFont)
     last->next = ret;

   return ret;
}

/*
 * Load a block of 32 glyphs into a cache block requested
 * from the block allocator.
 */
void
xf86loadFontBlock(fentry, block)
     CacheFont8Ptr fentry;
     int   block;
{
   int   i, j, c;
   unsigned char chr;
   int   nbyLine;
   unsigned char *pb, *pbits;
   unsigned char *pglyph;
   int   gWidth, gHeight;
   int   nbyGlyphWidth;
   int   nbyPadGlyph;

   nbyLine = PixmapBytePad(fentry->w, 1);
   ERROR_F(("loading %d (%d) %d\n", fentry->font, block, fentry->fblock[block]));

   pbits = (unsigned char *)ALLOCATE_LOCAL(nbyLine * fentry->h);
   if (pbits != NULL &&
    (fentry->fblock[block] = xf86AllocFromCachePool( xf86FontPool,
						     32 * fentry->w,
						     fentry->h )) != NULL) {
      fentry->fblock[block]->reference = (pointer *) &(fentry->fblock[block]); 
      fentry->fblock[block]->lru = 0x7fffffff;
      for (c = block * 32; c < (block * 32) + 32; c++) {

	 if (fentry->pci[c] != NULL) {
	    chr = (unsigned char)c;

	    pglyph = FONTGLYPHBITS(pglyphBase, fentry->pci[c]);
	    gWidth = GLYPHWIDTHPIXELS(fentry->pci[c]);
	    gHeight = GLYPHHEIGHTPIXELS(fentry->pci[c]);
	    if (gWidth && gHeight) {
	       nbyGlyphWidth = GLYPHWIDTHBYTESPADDED(fentry->pci[c]);
	       nbyPadGlyph = PixmapBytePad(gWidth, 1);

	       if (nbyGlyphWidth == nbyPadGlyph
#if GLYPHPADBYTES != 4
		   && (((int)pglyph) & 3) == 0
#endif
		  )
		  pb = pglyph;
	       else {
		  for (i = 0, pb = pbits;
		       i < gHeight;
		       i++, pb = pbits + (i * nbyPadGlyph))
		     for (j = 0; j < nbyGlyphWidth; j++)
			*pb++ = *pglyph++;
		  pb = pbits;
	       }
	       (xf86FontOpStippleFunc)( fentry->fblock[block]->x +
					 		(c % 32) * fentry->w,
					fentry->fblock[block]->y,
					gWidth, gHeight,
					pb, nbyPadGlyph,
					fentry->fblock[block]->id);
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
    * get Block function found no block of the right size, this is probably
    * impossible but just to stop potential core dumps we shall do something
    * stupid about it anyway we just throw away the font blocks of another
    * font. Or even ourselves in desperate times!
    * Unfortunatly this doesn't work if we use the preload code so the
    * demand load makes more sense.
    */
      if (pbits) DEALLOCATE_LOCAL(pbits);
      ERROR_F(("Time to write new font cache management\n"));
      for (fptr = xf86HeadFont; fptr != NULL; fptr= fptr->next)
	 if (fptr != fentry) {
	    for (i = 0; i < 8; i++)
	       if (fptr->fblock[i] != NULL) {
		 xf86ReleaseToCachePool( xf86FontPool, fptr->fblock[i] );
		 fptr->fblock[i] = NULL;
		 found = TRUE;
	       }
	    if (found)
	       break;
	 }


      /* getting real desperate - this doesn't work with pre-loading */
      if (!found) { 
	 for (i = 0; i < 8; i++)
	    if (fentry->fblock[i] != NULL) {
	       xf86ReleaseToCachePool( xf86FontPool, fentry->fblock[i] );
	       fentry->fblock[i] = NULL;
	    }
      }
      xf86loadFontBlock(fentry, block);
      return;
   }
#ifdef DEBUG_FCACHE
for(fentry=xf86HeadFont;fentry != NULL;fentry=fentry->next)
  if( fentry->font != NULL )
    for (i = 0; i < 8; i++)
      ERROR_F(("got %d (%d) %d x=%d y=%d w=%d h=%d\n",
	       fentry->font, i, fentry->fblock[i],
	       fentry->fblock[i] != NULL ? fentry->fblock[i]->x : 0,
	       fentry->fblock[i] != NULL ? fentry->fblock[i]->y : 0,
	       fentry->fblock[i] != NULL ? fentry->fblock[i]->w : 0,
	       fentry->fblock[i] != NULL ? fentry->fblock[i]->h : 0));
#endif
}

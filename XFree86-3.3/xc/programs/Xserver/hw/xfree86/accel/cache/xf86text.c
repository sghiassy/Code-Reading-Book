/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/cache/xf86text.c,v 3.1 1996/12/23 06:33:22 dawes Exp $ */
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
/* $XConsortium: xf86text.c /main/3 1996/02/21 17:19:27 kaleb $ */

/*
 * Extracted from s3fcach.c and adapted to XFree86 in X11R6 by
 * Hans Nasten. ( nasten@everyware.se ).
 */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"mi.h"
#include	"cfb.h"
#include	"misc.h"
#include        "xf86.h"
#include	"xf86bcache.h"
#include	"xf86fcache.h"
#include	"xf86text.h"

static void (*xf86GlyphWriteFunc)();
static int (*xf86NoCPolyTextFunc)();
static int (*xf86NoCImageTextFunc)();

/*
 * Init the text code by storing the callback pointers.
 */
void xf86InitText( GlyphWriteFunc, NoCPolyTextFunc, NoCImageTextFunc )
void (*GlyphWriteFunc)();
int (*NoCPolyTextFunc)();
int (*NoCImageTextFunc)();

{
    xf86GlyphWriteFunc = GlyphWriteFunc;
    xf86NoCPolyTextFunc = NoCPolyTextFunc;
    xf86NoCImageTextFunc = NoCImageTextFunc;
}

/*
 * General cached PolyText8 function.
 */
static int xf86CPolyText8(pDraw, pGC, x, y, count, chars, fentry)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x;
     int   y;
     int   count;
     char *chars;
     CacheFont8Ptr fentry;
{
   int   i;
   BoxPtr pBox;
   int   numRects;
   RegionPtr pRegion;
   int   yBand;
   int   maxAscent, maxDescent;
   int   minLeftBearing;
   FontPtr pfont = pGC->font;
   int ret_x;

   {
   char  toload[8];

   for (i = 0; i < 8; i++)
      toload[i] = 0;

   /*
    * If miPolyText8() is to be believed, the returned new X value is
    * completely independent of what happens during rendering.
    */
   ret_x = x;
   for (i = 0; i < count; i++) {
      toload[(unsigned char)(chars[i]) / 32] = 1;
      ret_x += fentry->pci[(unsigned char)(chars[i])] ? 
	       fentry->pci[(unsigned char)(chars[i])]->metrics.characterWidth : 0; 
   }
   for (i = 0; i < 8; i++)
      if (toload[i]) {
	 if ((fentry->fblock[i]) == NULL) {
	    xf86loadFontBlock(fentry, i);
	 }
      }
   }

   x += pDraw->x;
   y += pDraw->y;
   maxAscent = FONTMAXBOUNDS(pfont, ascent);
   maxDescent = FONTMAXBOUNDS(pfont, descent);
   minLeftBearing = FONTMINBOUNDS(pfont, leftSideBearing);
   pRegion = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;

   pBox = REGION_RECTS(pRegion);
   numRects = REGION_NUM_RECTS(pRegion);
   while (numRects && pBox->y2 <= y - maxAscent) {
      ++pBox;
      --numRects;
   }
   if (!numRects || pBox->y1 >= y + maxDescent)
      return ret_x;
   yBand = pBox->y1;
   while (numRects && pBox->y1 == yBand && pBox->x2 <= x + minLeftBearing) {
      ++pBox;
      --numRects;
   }
   if (numRects)
	(xf86GlyphWriteFunc)(x, y, count, chars, fentry, pGC, pBox, numRects);

   return ret_x;
}

/*
 * General cached ImageText8 function.
 */
static int xf86CImageText8(pDraw, pGC, x, y, count, chars, fentry)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x;
     int   y;
     int   count;
     char *chars;
     CacheFont8Ptr fentry;
{
   ExtentInfoRec info;		/* used by QueryGlyphExtents() */
   XID   gcvals[3];
   int   oldAlu, oldFS;
   unsigned long oldFG;
   xRectangle backrect;
   CharInfoPtr *ppci;
   unsigned long n;

   if (!(ppci = (CharInfoPtr *) ALLOCATE_LOCAL(count * sizeof(CharInfoPtr))))
      return 0;

   GetGlyphs(pGC->font, (unsigned long)count, (unsigned char *)chars,
	     Linear8Bit, &n, ppci);

   QueryGlyphExtents(pGC->font, ppci, n, &info);

   DEALLOCATE_LOCAL(ppci);

   if (info.overallWidth >= 0) {
      backrect.x = x;
      backrect.width = info.overallWidth;
   } else {
      backrect.x = x + info.overallWidth;
      backrect.width = -info.overallWidth;
   }
   backrect.y = y - FONTASCENT(pGC->font);
   backrect.height = FONTASCENT(pGC->font) + FONTDESCENT(pGC->font);

   oldAlu = pGC->alu;
   oldFG = pGC->fgPixel;
   oldFS = pGC->fillStyle;

 /* fill in the background */
   gcvals[0] = GXcopy;
   gcvals[1] = pGC->bgPixel;
   gcvals[2] = FillSolid;
   DoChangeGC(pGC, GCFunction | GCForeground | GCFillStyle, gcvals, 0);
   ValidateGC(pDraw, pGC);
   (*pGC->ops->PolyFillRect) (pDraw, pGC, 1, &backrect);

 /* put down the glyphs */
   gcvals[0] = oldFG;
   DoChangeGC(pGC, GCForeground, gcvals, 0);
   ValidateGC(pDraw, pGC);
   (void)xf86CPolyText8(pDraw, pGC, x, y, count, chars, fentry);

 /* put all the toys away when done playing */
   gcvals[0] = oldAlu;
   gcvals[1] = oldFG;
   gcvals[2] = oldFS;
   DoChangeGC(pGC, GCFunction | GCForeground | GCFillStyle, gcvals, 0);
   return 0;
}

/*
 * General PolyText8 function.
 */
int xf86PolyText8(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x, y;
     int   count;
     char *chars;
{
   CacheFont8Ptr ret;

   if (!xf86VTSema)
   {
      return(miPolyText8(pDraw, pGC, x, y, count, chars));
   }

   /*
    * The S3 graphics engine apparently can't handle these ROPs for the
    * BLT operations used to render text.  The 8514 and ATI don't have a
    * problem using the same code that S3 uses.  The common feature of these
    * ROPs is that they don't reference the source pixel.
    */
   if ((pGC->fillStyle != FillSolid) || 
       (pGC->alu == GXclear || pGC->alu == GXinvert || pGC->alu == GXset)) {
      return miPolyText8(pDraw, pGC, x, y, count, chars);       
   } else {
      if ((ret = xf86CacheFont8(pGC->font)) == NULL)
	 return (xf86NoCPolyTextFunc)(pDraw, pGC, x, y, count, chars, TRUE);
      else
	 return xf86CPolyText8(pDraw, pGC, x, y, count, chars, ret);
   }
}

/*
 * General PolyText16 function.
 */
int xf86PolyText16(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x, y;
     int   count;
     unsigned short *chars;
{

   if (!xf86VTSema || 
       (pGC->fillStyle != FillSolid) || 
       (pGC->alu == GXclear || pGC->alu == GXinvert ||	pGC->alu == GXset)) {
      return miPolyText16(pDraw, pGC, x, y, count, chars);
  }
   return (xf86NoCPolyTextFunc)(pDraw, pGC, x, y, count, chars, FALSE);
}

/*
 * General ImageText8 function.
 */
void xf86ImageText8(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x, y;
     int   count;
     char *chars;
{
   CacheFont8Ptr ret;

   if (!xf86VTSema)
   {
      miImageText8(pDraw, pGC, x, y, count, chars);
      return;
   } 

   /* Don't need to check fill style here - it isn't used in image text */
   if ((ret = xf86CacheFont8(pGC->font)) == NULL)
     (xf86NoCImageTextFunc)(pDraw, pGC, x, y, count, chars, TRUE);
   else
      xf86CImageText8(pDraw, pGC, x, y, count, chars, ret);
}

/*
 * General ImageText16 function.
 */
void xf86ImageText16(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x, y;
     int   count;
     unsigned short *chars;
{
   if (!xf86VTSema) 
	miImageText16(pDraw, pGC, x, y, count, chars);
   else
        (xf86NoCImageTextFunc)(pDraw, pGC, x, y, count, chars, FALSE);
}

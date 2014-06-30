/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000text.c,v 3.3 1996/12/23 06:40:55 dawes Exp $ */
/*
 * Copyright 1992,1993 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for Mach32 By Mike Bernson (mike@mbsun.mlb.org) from mach8 version
 * Modified for the p9000 by chris mason (clmtch@osfmail.isc.rit.edu)
 */
/* $XConsortium: p9000text.c /main/4 1996/02/21 17:33:15 kaleb $ */

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
#include	"p9000reg.h"
#include	"p9000.h"

__inline__ static int
p9000NoCPolyText() ;
__inline__ static int
p9000NoCImageText() ;

/*
 * The guts of this should possibly be tidied up and put in mach32im.c.
 * The generic Stipple functions in mach32im.c have quite a large unnecessary
 * overhead for bitmap copies in the case that we are doing an exacy copy
 * with no tiling and starting from (0,0) in the source bitmap. - Jon.
 */
__inline__ static void
p9000PolyGlyphBlt(pDrawable, pGC, x, y, nglyph, ppci, pglyphBase)
    DrawablePtr pDrawable;
    GC 		*pGC;
    int 	x, y;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    unsigned char *pglyphBase;	/* start of array of glyphs */
{
    int width, height;
    int nbyLine;			/* bytes per line of padded pixmap */
    FontPtr pfont;
    register int i;
    register int j;
    unsigned char *pbits;		/* buffer for PutImage */
    register unsigned char *pb;		/* temp pointer into buffer */
    register CharInfoPtr pci;		/* currect char info */
    register unsigned char *pglyph;	/* pointer bits in glyph */
    int gWidth, gHeight;		/* width and height of glyph */
    register int nbyGlyphWidth;		/* bytes per scanline of glyph */
    int nbyPadGlyph;			/* server padded line of glyph */
    
    pfont = pGC->font;
    width = FONTMAXBOUNDS(pfont,rightSideBearing) - 
	    FONTMINBOUNDS(pfont,leftSideBearing);
    height = FONTMAXBOUNDS(pfont,ascent) +
	     FONTMAXBOUNDS(pfont,descent);

    nbyLine = PixmapBytePad(width, 1);
    pbits = (unsigned char *)ALLOCATE_LOCAL(height*nbyLine);
    if (!pbits)
    {
        return;
    }

    while(nglyph--)
    {
	pci = *ppci++ ;
	pglyph = FONTGLYPHBITS(pglyphBase, pci);
	gWidth = GLYPHWIDTHPIXELS(pci);
	gHeight = GLYPHHEIGHTPIXELS(pci);
	if (gWidth && gHeight)
	{
	    nbyGlyphWidth = GLYPHWIDTHBYTESPADDED(pci);
	    nbyPadGlyph = PixmapBytePad(gWidth, 1);

	    if (nbyGlyphWidth == nbyPadGlyph
#if GLYPHPADBYTES != 4
	        && (((int) pglyph) & 3) == 0
#endif
		)
	    {
		pb = pglyph;
	    }
	    else
	    {
		for (i=0, pb = pbits; i<gHeight; i++, pb = pbits+(i*nbyPadGlyph))
		    for (j = 0; j < nbyGlyphWidth; j++)
			*pb++ = *pglyph++;
		pb = pbits;
	    }

	    p9000Store(DEVICE_COORD | DC_0 | DC_X, CtlBase, 
	               x + pci->metrics.leftSideBearing) ;
	    p9000Store(DEVICE_COORD | DC_1 | DC_X, CtlBase, 
	               x + pci->metrics.leftSideBearing) ;
	    p9000Store(DEVICE_COORD | DC_1 | DC_Y, CtlBase,
	    	       y - pci->metrics.ascent) ;
	    p9000Store(DEVICE_COORD | DC_2 | DC_X, CtlBase,
	    	       x + pci->metrics.leftSideBearing + gWidth) ;
	    p9000Store(DEVICE_COORD | DC_3 | DC_Y, CtlBase, 1) ;

	    {
	        register unsigned char *pix ;

	        for (i = 0 ; i < gHeight; ) {
	            j = gWidth ;
	            pix = pb + i * nbyPadGlyph ;
	            i++ ;

	            while (j > 32) {
	                p9000Store(CMD_PIXEL1_32 | HBBSWAP, CtlBase, 
	                           *(int *)pix) ;
	                pix += 4 ;
	                j -= 32 ;
	            }

	            if (j > 0) {
	                p9000Store(CMD_PIXEL1(j) | HBBSWAP, CtlBase, 
	                           *(int *)pix) ;
	            }
                }
            }
	}
	x += pci->metrics.characterWidth; 
    }
    DEALLOCATE_LOCAL(pbits);
}




static __inline__ int
p9000NoCPolyText(pDraw, pGC, x, y, count, chars, is8bit)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x;
     int   y;
     int   count;
     char *chars;
     Bool is8bit;
{
   int   i;
   BoxPtr pBox;
   int   numRects;
   RegionPtr pRegion;
   int   yBand;
   int   maxAscent, maxDescent;
   int   minLeftBearing;
   int   old_w_min, old_w_max ;
   FontPtr pfont = pGC->font;
   int ret_x;
   unsigned long n;
   CharInfoPtr *charinfo;

   if (!OnScreenDrawable(pDraw->type) || !xf86VTSema ||
       (pGC->fillStyle != FillSolid)
      ) {
       if (is8bit) 
           return miPolyText8(pDraw, pGC, x, y, count, chars) ;
       else
           return miPolyText16(pDraw, pGC, x,y,count, (unsigned short *)chars) ;
   }

    old_w_min = p9000Fetch(W_MIN, CtlBase) ;
    old_w_max = p9000Fetch(W_MAX, CtlBase) ;

    if(!(charinfo = (CharInfoPtr *)ALLOCATE_LOCAL(count*sizeof(CharInfoPtr ))))
	return x ;

    if (is8bit)
      GetGlyphs(pGC->font, (unsigned long)count, (unsigned char *)chars,
	      Linear8Bit, &n, charinfo);
    else
      GetGlyphs(pGC->font, (unsigned long)count, (unsigned char *)chars,
		(FONTLASTROW(pGC->font) == 0) ? Linear16Bit : TwoD16Bit,
		&n, charinfo);

   /*
    * If miPolyText8() is to be believed, the returned new X value is
    * completely independent of what happens during rendering.
    */
   ret_x = x;
   for (i = 0; i < n; i++) {
      ret_x += charinfo[i]->metrics.characterWidth;
   }

   if (n == 0) {
      DEALLOCATE_LOCAL(charinfo);
      return ret_x;
   }
   
   x += pDraw->x;
   y += pDraw->y;
   maxAscent = FONTMAXBOUNDS(pfont, ascent);
   maxDescent = FONTMAXBOUNDS(pfont, descent);
   minLeftBearing = FONTMINBOUNDS(pfont, leftSideBearing);
   pRegion = ((cfbPrivGC *) 
              (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;

   pBox = REGION_RECTS(pRegion);
   numRects = REGION_NUM_RECTS(pRegion);
   while (numRects && pBox->y2 <= y - maxAscent) {
      ++pBox;
      --numRects;
   }
   if (!numRects || pBox->y1 >= y + maxDescent) {
      DEALLOCATE_LOCAL(charinfo);
      return ret_x;
   }
   yBand = pBox->y1;
   while (numRects && pBox->y1 == yBand && pBox->x2 <= x + minLeftBearing) {
      ++pBox;
      --numRects;
   }
   if (!numRects) {
      DEALLOCATE_LOCAL(charinfo);   
      return ret_x;
   }

   p9000NotBusy() ;

  p9000Store(RASTER, CtlBase, p9000PixAlu[pGC->alu]) ;

   p9000Store(FGROUND, CtlBase, pGC->fgPixel) ;
   p9000Store(PMASK, CtlBase, pGC->planemask) ;

   for (; --numRects >= 0; ++pBox) {
      p9000NotBusy() ;
      p9000Store(W_MIN, CtlBase, YX_PACK(pBox->x1, pBox->y1)) ;
      p9000Store(W_MAX, CtlBase, YX_PACK(pBox->x2-1, pBox->y2-1)) ;
      p9000PolyGlyphBlt(pDraw, pGC, x, y, (unsigned int)n, charinfo,
						FONTGLYPHS(pGC->font));
   }

   DEALLOCATE_LOCAL(charinfo);

   p9000NotBusy() ;
   p9000Store(W_MIN, CtlBase, old_w_min) ;
   p9000Store(W_MAX, CtlBase, old_w_max) ;

   return ret_x;
}

/*
 * Imagetext is all in one function to avoid calling GetGlyphs() twice.
 * - Jon.
 */
 
static __inline__ int
p9000NoCImageText(pDraw, pGC, x, y, count, chars, is8bit)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x;
     int   y;
     int   count;
     char *chars;
     Bool is8bit;
{
   ExtentInfoRec info;		/* used by QueryGlyphExtents() */
   XID   gcvals[3];
   int   oldAlu, oldFS;
   unsigned long oldFG;
   xRectangle backrect;
   register CharInfoPtr *charinfo;
   unsigned long n;
   int   old_w_min, old_w_max ;

   if (!OnScreenDrawable(pDraw->type) || !xf86VTSema || 
       (pGC->fillStyle != FillSolid)
      ) {
       if (is8bit) 
           miImageText8(pDraw, pGC, x, y, count, chars) ;
       else 
           miImageText16(pDraw, pGC, x, y, count, (unsigned short *)chars) ;
       return 1 ;
   }

   old_w_min = p9000Fetch(W_MIN, CtlBase) ;
   old_w_max = p9000Fetch(W_MAX, CtlBase) ;

   if (!(charinfo = (CharInfoPtr *) ALLOCATE_LOCAL(count * sizeof(CharInfoPtr))))
      return 0;


    if (is8bit)
      GetGlyphs(pGC->font, (unsigned long)count, (unsigned char *)chars,
	      Linear8Bit, &n, charinfo);
    else
      GetGlyphs(pGC->font, (unsigned long)count, (unsigned char *)chars,
		(FONTLASTROW(pGC->font) == 0) ? Linear16Bit : TwoD16Bit,
		&n, charinfo);

   QueryGlyphExtents(pGC->font, charinfo, n, &info);

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

   gcvals[0] = GXcopy;
   gcvals[1] = pGC->bgPixel;
   gcvals[2] = FillSolid;
   DoChangeGC(pGC, GCFunction | GCForeground | GCFillStyle, gcvals, 0);
   ValidateGC(pDraw, pGC);
   (*pGC->ops->PolyFillRect) (pDraw, pGC, 1, &backrect);

 /* put down the glyphs */
   gcvals[0] = oldAlu;
   gcvals[1] = oldFG;
   gcvals[2] = oldFS;
   DoChangeGC(pGC, GCFunction | GCForeground | GCFillStyle, gcvals, 0);
   ValidateGC(pDraw, pGC);
   /* begin the font blitting */
   {
      register BoxPtr pBox;
      register int   numRects;
      register RegionPtr pRegion;
      int   maxAscent, maxDescent;
      int   yBand;
      int   minLeftBearing;
      register FontPtr pfont = pGC->font;
      
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
   if (!numRects || pBox->y1 >= y + maxDescent) {
      DEALLOCATE_LOCAL(charinfo);
      return 0;
   }
   yBand = pBox->y1;
   while (numRects && pBox->y1 == yBand && pBox->x2 <= x + minLeftBearing) {
      ++pBox;
      --numRects;
   }
   if (!numRects) {
      DEALLOCATE_LOCAL(charinfo);   
      return 0;
   }

   p9000NotBusy() ;


   p9000Store(FGROUND, CtlBase, pGC->fgPixel) ;
   p9000Store(BGROUND, CtlBase, pGC->bgPixel) ;
   p9000Store(PMASK, CtlBase, pGC->planemask) ;
   p9000Store(RASTER, CtlBase, (IGM_S_MASK & IGM_F_MASK) | 
                               (~IGM_S_MASK & IGM_D_MASK));

   for (; --numRects >= 0; ++pBox) {
      p9000NotBusy() ;
      p9000Store(W_MIN, CtlBase, YX_PACK(pBox->x1, pBox->y1)) ;
      p9000Store(W_MAX, CtlBase, YX_PACK(pBox->x2-1, pBox->y2-1)) ;
      p9000PolyGlyphBlt(pDraw, pGC, x, y, (unsigned int)n, charinfo,
						FONTGLYPHS(pGC->font));
   }

   } /* end font blitting */
   
 /* put all the toys away when done playing */
 /* note the GC was changed back previously */
   DEALLOCATE_LOCAL(charinfo);

   p9000NotBusy() ;
   p9000Store(W_MIN, CtlBase, old_w_min) ;
   p9000Store(W_MAX, CtlBase, old_w_max) ;
   return 0;
}

int
p9000PolyText8(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x;
     int   y;
     int   count;
     char *chars;
{
    return p9000NoCPolyText(pDraw, pGC, x, y, count, chars, 1) ;
}

int
p9000PolyText16(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x;
     int   y;
     int   count;
     unsigned short *chars;
{
    return p9000NoCPolyText(pDraw, pGC, x, y, count, chars, 0) ;
}

int
p9000ImageText8(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x;
     int   y;
     int   count;
     char  *chars;
{
    return p9000NoCImageText(pDraw, pGC, x, y, count, chars, 1) ;
}

int
p9000ImageText16(pDraw, pGC, x, y, count, chars)
     DrawablePtr pDraw;
     GCPtr pGC;
     int   x;
     int   y;
     int   count;
     unsigned short *chars;
{
    return p9000NoCImageText(pDraw, pGC, x, y, count, chars, 0) ;
}


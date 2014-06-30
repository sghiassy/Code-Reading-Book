/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/ibm8514/text.c,v 3.2 1996/12/23 06:38:06 dawes Exp $ */
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

/*
 * Extracted from s3text.c
 * Modified for 8514, Hans Nasten. (nasten@everyware.se)
 */
/* $XConsortium: text.c /main/5 1996/10/25 16:22:31 kaleb $ */

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
#include	"ibm8514.h"
#include	"reg8514.h"

extern unsigned short ibm8514stipple_tab[];


/*
 * The guts of this should possibly be tidied up and put in xxxxim.c.
 * The generic Stipple functions in xxxxim.c have quite a large unnecessary
 * overhead for bitmap copies in the case that we are doing an exacy copy
 * with no tiling and starting from (0,0) in the source bitmap. - Jon.
 */
__inline__ static void
ibm8514PolyGlyphBlt(pDrawable, pGC, x, y, nglyph, ppci, pglyphBase, pBox)
    DrawablePtr pDrawable;
    GC 		*pGC;
    int 	x, y;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    unsigned char *pglyphBase;	/* start of array of glyphs */
    BoxPtr pBox;                /* clipping box */
{
    int width, height;
    int nbyLine;		/* bytes per line of padded pixmap */
    FontPtr pfont;
    int i;
    int j;
    unsigned char *pbits;	/* buffer for PutImage */
    unsigned char *pb;		/* temp pointer into buffer */
    CharInfoPtr pci;		/* currect char info */
    unsigned char *pglyph;	/* pointer bits in glyph */
    int gWidth, gHeight;	/* width and height of glyph */
    register int nbyGlyphWidth;	/* bytes per scanline of glyph */
    int nbyPadGlyph;		/* server padded line of glyph */


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
      pci = *ppci++;
      pglyph = FONTGLYPHBITS(pglyphBase, pci);
      gWidth = GLYPHWIDTHPIXELS(pci);
      gHeight = GLYPHHEIGHTPIXELS(pci);
      if (gWidth && gHeight)
      {
	if ( x + pci->metrics.leftSideBearing + gWidth-1 >= pBox->x1
	     && x + pci->metrics.leftSideBearing            <  pBox->x2
	     && y - pci->metrics.ascent         + gHeight-1 >= pBox->y1
	     && y - pci->metrics.ascent                     <  pBox->y2 ) {
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
	    WaitQueue(5);
	    outw(CUR_X, (short)(x+pci->metrics.leftSideBearing));
	    outw(CUR_Y, (short)(y - pci->metrics.ascent));
	    outw(MAJ_AXIS_PCNT, (short)(gWidth - 1));
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (gHeight-1));
	    outw(CMD, CMD_RECT | PCDATA | INC_Y | INC_X |
		 DRAW | PLANAR | WRTDATA);
	    
	    { /* The stipple code */
	       int h;
	       register int xp = x+pci->metrics.leftSideBearing;
	       int w = gWidth + (xp & 3);
	       register unsigned char ui;
	       unsigned char *cp;

	       for (h = 0; h < gHeight; h++) {
		  cp = pb+(h*nbyPadGlyph);
		  for (i = 0; i < w; i += 8) {
		     ui = (ui >> 4 | (*cp << 4));
		     outw(PIX_TRANS,
			  ibm8514stipple_tab[(ui << (xp & 3)) & 0xf0]);
		     if( i + 4 < w ) {
		         ui = (ui >> 4 | (*cp++ & 0xf0));
		         outw(PIX_TRANS,
			      ibm8514stipple_tab[(ui << (xp & 3)) & 0xf0]);
		     }
		  }
	       }
	    }
	}
      }
      x += pci->metrics.characterWidth;
    }
    DEALLOCATE_LOCAL(pbits);
}

int ibm8514NoCPolyText(pDraw, pGC, x, y, count, chars, is8bit)
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
   FontPtr pfont = pGC->font;
   int ret_x;
   unsigned long n;
   CharInfoPtr *charinfo;


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
   pRegion = ((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip;

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

   WaitQueue (6);
   outw (WRT_MASK, pGC->planemask);   
   outw (FRGD_MIX, FSS_FRGDCOL | ibm8514alu[pGC->alu]);
   outw (BKGD_MIX, BSS_BKGDCOL | MIX_DST);
   outw (MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPPC | COLCMPOP_F);
   outw (FRGD_COLOR, (short) pGC->fgPixel);
   outw (BKGD_COLOR, (short) pGC->bgPixel);

   for (; --numRects >= 0; ++pBox) {
     /*
      * Skip all boxes that are completely above or below the text string.
      */
     if( pBox->y2 >= y - maxAscent && pBox->y1 <= y + maxDescent ) {
       WaitQueue(4);
       outw (MULTIFUNC_CNTL, SCISSORS_L | (short)pBox->x1);
       outw(MULTIFUNC_CNTL, SCISSORS_T | (short)pBox->y1);
       outw(MULTIFUNC_CNTL, SCISSORS_R | (short)(pBox->x2 - 1));
       outw(MULTIFUNC_CNTL, SCISSORS_B | (short)(pBox->y2 - 1));

       ibm8514PolyGlyphBlt(pDraw, pGC, x, y, (unsigned int)n, charinfo,
			   FONTGLYPHS(pGC->font), pBox);
     }
   }

   WaitQueue(7);
   outw (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
   outw (BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
   outw (MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);
   outw(MULTIFUNC_CNTL, SCISSORS_T | 0);
   outw(MULTIFUNC_CNTL, SCISSORS_L | 0);
   outw(MULTIFUNC_CNTL, SCISSORS_R | 1023);
   outw(MULTIFUNC_CNTL, SCISSORS_B | 1023);

   return ret_x;
}


/*
 * Imagetext is all in one function to avoid calling GetGlyphs() twice.
 * - Jon.
 */
 
int ibm8514NoCImageText(pDraw, pGC, x, y, count, chars, is8bit)
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
   CharInfoPtr *charinfo;
   unsigned long n;

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

   /* begin the font blitting */
   {
      BoxPtr pBox;
      int   numRects;
      RegionPtr pRegion;
      int   maxAscent, maxDescent;
      int   yBand;
      int   minLeftBearing;
      FontPtr pfont = pGC->font;
      
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

   WaitQueue (6);
   outw (WRT_MASK, pGC->planemask);   
   outw (FRGD_MIX, FSS_FRGDCOL | ibm8514alu[pGC->alu]);
   outw (BKGD_MIX, BSS_BKGDCOL | MIX_DST);
   outw (MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPPC | COLCMPOP_F);
   outw (FRGD_COLOR, (short) pGC->fgPixel);

   outw (BKGD_COLOR, (short) pGC->bgPixel);

   for (; --numRects >= 0; ++pBox) {
     /*
      * Skip all boxes that are completely above or below the text string.
      */
     if( pBox->y2 >= y - maxAscent && pBox->y1 <= y + maxDescent ) {
       WaitQueue(4);
       outw (MULTIFUNC_CNTL, SCISSORS_L | (short)pBox->x1);
       outw(MULTIFUNC_CNTL, SCISSORS_T | (short)pBox->y1);
       outw(MULTIFUNC_CNTL, SCISSORS_R | (short)(pBox->x2 - 1));
       outw(MULTIFUNC_CNTL, SCISSORS_B | (short)(pBox->y2 - 1));

       ibm8514PolyGlyphBlt(pDraw, pGC, x, y, (unsigned int)n, charinfo,
			   FONTGLYPHS(pGC->font), pBox);
     }
   }

   WaitQueue(7);
   outw (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
   outw (BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
   outw (MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);
   outw(MULTIFUNC_CNTL, SCISSORS_T | 0);
   outw(MULTIFUNC_CNTL, SCISSORS_L | 0);
   outw(MULTIFUNC_CNTL, SCISSORS_R | 1023);
   outw(MULTIFUNC_CNTL, SCISSORS_B | 1023);
   }
   
 /* put all the toys away when done playing */
   gcvals[0] = oldAlu;
   gcvals[1] = oldFG;
   gcvals[2] = oldFS;
   DoChangeGC(pGC, GCFunction | GCForeground | GCFillStyle, gcvals, 0);
   return 0;
}

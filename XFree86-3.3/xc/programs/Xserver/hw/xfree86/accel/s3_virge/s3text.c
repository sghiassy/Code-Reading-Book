/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3text.c,v 3.11.2.1 1997/05/24 08:36:03 dawes Exp $ */
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
 * Modified by Amancio Hasty and Jon Tombs
 *
 */
/* $XConsortium: s3text.c /main/10 1996/10/28 04:55:24 kaleb $ */


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
#include	"s3v.h"

extern unsigned char s3SwapBits[256];


__inline__ void s3SimpleStipple(x, y, width, height, pb, pwidth, clip_l, clip_r, alu, planemask)
     int x, y;
     int width, height, pwidth;
     unsigned char *pb;
     int clip_l, clip_r;
     int alu;
     unsigned long planemask;
{
   int newwidth;
   int S_in_ROP;

   if (alu == GXnoop)
      return;  /* avoid lockup with ROP_D */

   if ((planemask & s3BppPMask) == s3BppPMask) {
      alu = s3alu[alu];
      S_in_ROP = (alu != ROP_0 && alu != ROP_1 && alu != ROP_D && alu != ROP_Dn);
   }
   else {
      alu = s3alu_pat[alu];
      S_in_ROP = (alu != ROP_0PaDPnao && alu != ROP_1PaDPnao && alu != ROP_DPaDPnao && alu != ROP_DnPaDPnao);   
      WaitQueue(3);
      SETB_PAT_FG_CLR(planemask);
      SETB_MONO_PAT0(~0);  /* set all pattern bits to use planemask */
      SETB_MONO_PAT1(~0);  /* for all pixels */
   }

   newwidth = s3CheckLSPN(width, 1);
   if (newwidth != width) {
      WaitQueue(5);
      SETB_CLIP_L_R(max(clip_l,x), min(clip_r,x + width-1));
   }
   else {
      WaitQueue(3);
   }
	    
   SETB_RDEST_XY((short) x, (short) y);
   SETB_RWIDTH_HEIGHT((short) (newwidth - 1), (short)(height));
   WaitIdle();
   SETB_CMD_SET(s3_gcmd | CMD_BITBLT | INC_Y | INC_X
		| CMD_ITA_DWORD | MIX_MONO_TRANSP | MIX_MONO_PATT
		| MIX_CPUDATA | MIX_MONO_SRC | alu);

   if (S_in_ROP)  {			/* The stipple code */
#define SWPBIT(s) (s3SwapBits[pb[(s)]])

      int i, h, pix;
      unsigned int getbuf;

      for (h = 0; h < height; h++) {
	 pix = 0;

	 for (i = 0; i < newwidth; i += 32) {
	    getbuf = SWPBIT(pix+3)<<24 | SWPBIT(pix+2)<<16
	       |     SWPBIT(pix+1)<< 8 | SWPBIT(pix+0)<< 0;
	    *IMG_TRANS = getbuf;
	    write_mem_barrier();
	    pix += 4;
	 }
	 pb += pwidth;
      }
   }
   WaitIdle();
   if (newwidth != width) {
      SETB_CLIP_L_R(clip_l, clip_r);
   }
}

/*
 * The guts of this should possibly be tidied up and put in s3im.c.
 * The generic Stipple functions in s3im.c have quite a large unnecessary
 * overhead for bitmap copies in the case that we are doing an exact copy
 * with no tiling and starting from (0,0) in the source bitmap. - Jon.
 */
__inline__ static void
s3PolyGlyphBlt(pDrawable, pGC, x, y, nglyph, ppci, pglyphBase, pBox)
    DrawablePtr pDrawable;
    GC 		*pGC;
    int 	x, y;
    unsigned int nglyph;
    CharInfoPtr *ppci;		/* array of character info */
    unsigned char *pglyphBase;	/* start of array of glyphs */
    BoxPtr pBox;		/* clipping box */
{
    int width, height, d;
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
	pci = *ppci++;
	gWidth = GLYPHWIDTHPIXELS(pci);
	gHeight = GLYPHHEIGHTPIXELS(pci);
	if (gWidth && gHeight) 	{
	   if (   x + pci->metrics.leftSideBearing + gWidth-1 >= pBox->x1
	       && x + pci->metrics.leftSideBearing            <  pBox->x2
	       && y - pci->metrics.ascent         + gHeight-1 >= pBox->y1
	       && y - pci->metrics.ascent                     <  pBox->y2 ) {
	     
	      pglyph = FONTGLYPHBITS(pglyphBase, pci);
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
	     
	      if (y - pci->metrics.ascent + gHeight-1 >=  pBox->y2 ) {
		 gHeight -= (y - pci->metrics.ascent + gHeight-1) - (pBox->y2-1);
	      }
	      if (y - pci->metrics.ascent < pBox->y1) {
		 d = pBox->y1 - (y - pci->metrics.ascent);
		 pb += d * nbyPadGlyph;
		 gHeight -= d;
	      }
	      else { 
		 d = 0;
	      }

	      s3SimpleStipple(x + pci->metrics.leftSideBearing,
			      y - pci->metrics.ascent + d,
			      gWidth, gHeight, pb, nbyPadGlyph,
			      pBox->x1, pBox->x2 - 1, pGC->alu, pGC->planemask);
	   }
        }
	
	x += pci->metrics.characterWidth;
    }
    DEALLOCATE_LOCAL(pbits);
}

void s3FontStipple(x, y, width, height, pb, pwidth, id)
int x, y;
int width, height, pwidth;
Pixel id;
unsigned char *pb;
{
   BLOCK_CURSOR;

   WaitQueue(2);
   SETB_SRC_FG_CLR(~0);
   SETB_SRC_BG_CLR(0);
   ;;;SET_WRT_MASK(id);
   ;SET_MIX(FSS_FRGDCOL | s3alu[GXcopy], BSS_BKGDCOL | s3alu[GXcopy]);
   ;SET_PIX_CNTL(MIXSEL_EXPPC | COLCMPOP_F);

   s3SimpleStipple(x, y, width, height, pb, pwidth, 0, s3DisplayWidth - 1,
		 GXcopy, /*~0*/ 0xffFFffFF);

   UNBLOCK_CURSOR;
}

int
s3NoCPolyText(pDraw, pGC, x, y, count, chars, is8bit)
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

   BLOCK_CURSOR;
   WaitQueue(1);
   SETB_SRC_FG_CLR(pGC->fgPixel);
   ;SET_WRT_MASK(pGC->planemask);
   ;SET_MIX(FSS_FRGDCOL | s3alu[pGC->alu], BSS_BKGDCOL | ROP_D);
   ;SET_PIX_CNTL(MIXSEL_EXPPC | COLCMPOP_F);

   for (; --numRects >= 0; ++pBox) {
      /*
       * Skip all boxes that are completely above or below the text string.
       */
      if( pBox->y2 >= y - maxAscent && pBox->y1 <= y + maxDescent ) {
         WaitQueue(2);
         SETB_CLIP_L_R(pBox->x1, pBox->x2 - 1);
         SETB_CLIP_T_B(pBox->y1, pBox->y2 - 1);
         s3PolyGlyphBlt(pDraw, pGC, x, y, (unsigned int)n, charinfo,
		        FONTGLYPHS(pGC->font), pBox);
      }
   }

   WaitQueue(2);
   SETB_CLIP_L_R(0, s3DisplayWidth - 1);
   SETB_CLIP_T_B(0, s3ScissB);
   UNBLOCK_CURSOR;
   DEALLOCATE_LOCAL(charinfo);

   return ret_x;
}


/*
 * Imagetext is all in one function to avoid calling GetGlyphs() twice.
 * - Jon.
 */

int
s3NoCImageText(pDraw, pGC, x, y, count, chars, is8bit)
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

   /* begin font BLT */
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

   BLOCK_CURSOR;
   WaitQueue(1);
   SETB_SRC_FG_CLR(pGC->fgPixel);
   ;SET_WRT_MASK(pGC->planemask);
   ;SET_MIX(FSS_FRGDCOL | s3alu[pGC->alu], BSS_BKGDCOL | ROP_D);
   ;SET_PIX_CNTL(MIXSEL_EXPPC | COLCMPOP_F);

   for (; --numRects >= 0; ++pBox) {
      /*
       * Skip all boxes that are completely above or below the text string.
       */
      if( pBox->y2 >= y - maxAscent && pBox->y1 <= y + maxDescent ) {
         WaitQueue(2);
         SETB_CLIP_L_R(pBox->x1, pBox->x2 - 1);
         SETB_CLIP_T_B(pBox->y1, pBox->y2 - 1);
         s3PolyGlyphBlt(pDraw, pGC, x, y, (unsigned int)n, charinfo,
		        FONTGLYPHS(pGC->font), pBox);
      }
   }

   WaitQueue(2);
   SETB_CLIP_L_R(0, s3DisplayWidth - 1);
   SETB_CLIP_T_B(0, s3ScissB);
   ;SET_MIX(FSS_FRGDCOL | ROP_S, BSS_BKGDCOL | ROP_S);
   ;SET_PIX_CNTL(MIXSEL_FRGDMIX | COLCMPOP_F);
   UNBLOCK_CURSOR;
   }

 /* put all the toys away when done playing */
   gcvals[0] = oldAlu;
   gcvals[1] = oldFG;
   gcvals[2] = oldFS;
   DoChangeGC(pGC, GCFunction | GCForeground | GCFillStyle, gcvals, 0);
   DEALLOCATE_LOCAL(charinfo);
   return 0;
}

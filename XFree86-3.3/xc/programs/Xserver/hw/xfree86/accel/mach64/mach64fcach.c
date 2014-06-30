/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64fcach.c,v 3.8 1996/12/23 06:39:13 dawes Exp $ */
/*
 * Copyright 1992,1993,1994 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 * Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)
 */
/* $XConsortium: mach64fcach.c /main/6 1996/05/07 17:10:57 kaleb $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"cfb.h"
#include	"misc.h"
#include	"windowstr.h"
#include	"gcstruct.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"mach64.h"

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

extern int xf86Verbose;

#define MAX_FONTS      	16	/* For static structure */
#define FC_MAX_WIDTH	24
#define FC_MAX_HEIGHT	32

typedef struct {
    FontPtr		font;
    unsigned int	lru;
    CharInfoPtr		pci[256];
    int                 x, y;
} CacheFontRec;

CacheFontRec	mach64FontCache[MAX_FONTS];
static int      NumFonts;


void
mach64FontCache8Init(x,y)
    int x, y;
{
    int        i;
    int        free_ram    = mach64InfoRec.videoRam * 1024
                             - x * y * (mach64InfoRec.bitsPerPixel / 8); 
    int        lines       = free_ram / (x * (mach64InfoRec.bitsPerPixel / 8));
    int        cache_sets;
    static int initialized = 0;

    if (OFLG_ISSET(OPTION_NO_FONT_CACHE, &mach64InfoRec.options)) {
	if (!initialized) {
	    ErrorF("%s %s: Font cache disabled\n", XCONFIG_GIVEN,
		   mach64InfoRec.name);
	    NumFonts = 0;
	    initialized++;
	}
	return;
    }

    if (!initialized) {
       ++initialized;

       if (lines < 0 || x < 1024)
	     lines = 0;
    
       cache_sets = lines / 256;
       if (cache_sets > MAX_FONTS / 8)
	     cache_sets = MAX_FONTS / 8;
       
       NumFonts = 8 * cache_sets;

       if (xf86Verbose)
	     ErrorF("%s %s: Font cache: %d fonts\n", XCONFIG_PROBED,
		    mach64InfoRec.name, NumFonts );
    }
    for (i = 0; i < NumFonts; i++) {
	mach64FontCache[i].font = (FontPtr)0;
	mach64FontCache[i].lru = 0xffffffff;
	mach64FontCache[i].x = 256 + (96 * (i % 8));
	mach64FontCache[i].y = y + 256 * (i >> 3);
    }
}

void
mach64UnCacheFont(font)
    FontPtr font;
{
    int i;

    if (!xf86VTSema)
	return;

    for (i = 0; i < NumFonts; i++)
	if (mach64FontCache[i].font == font) {
	    mach64FontCache[i].font = (FontPtr)0;
	    mach64FontCache[i].lru = 0xffffffff;
	    return;
	}
}

/* 
 * The mach64 does not support a sparse monochrome expansion blit, therefore
 * the font glyphs are cached linearly.  This is a bonus, since packed 
 * monochrome blits are significantly faster than sparse blits.
 */ 
static void
mach64CacheGlyph(x, y, width, height, psrc, pwidth)
    int                 x;
    int                 y;
    int                 width;
    int                 height;
    unsigned char       *psrc;
    int                 pwidth;
{
    int old_DST_OFF_PITCH;
    int old_DP_PIX_WIDTH;
    int offset;
    int srcline;
    int srcbit;
    int dstbit;
    int tmpword;
    int pword;

    offset = (((y * mach64VirtX) + x) * (mach64InfoRec.bitsPerPixel / 8)) >> 3;

    WaitQueue(13);
    old_DST_OFF_PITCH = regr(DST_OFF_PITCH);
    old_DP_PIX_WIDTH = regr(DP_PIX_WIDTH);

    regw(SC_LEFT_RIGHT, ((mach64MaxX << 16) | 0 ));
    regw(SC_TOP_BOTTOM, ((mach64MaxY << 16) | 0 ));

    regw(DP_MIX, (MIX_SRC << 16) | MIX_SRC);
    regw(DP_WRITE_MASK, 0xffffffff);
    regw(DP_FRGD_CLR, 0xffffffff);
    regw(DP_BKGD_CLR, 0);

    regw(DP_SRC, (MONO_SRC_HOST | FRGD_SRC_FRGD_CLR | BKGD_SRC_BKGD_CLR));
    regw(DP_PIX_WIDTH, (BYTE_ORDER_LSB_TO_MSB |HOST_1BPP |SRC_1BPP |DST_1BPP));

    regw(DST_OFF_PITCH, (old_DST_OFF_PITCH & 0xffc00000) | offset);
    regw(DST_Y_X, 0);
    regw(DST_HEIGHT_WIDTH, (((width * height) << 16) | 1));

    /* Pack the glyph as tightly as possible in the cache */

    dstbit=0;
    tmpword = 0;
    for (srcline=0; srcline < height; srcline++) 
    {
        pword = *(int *)psrc;
        for (srcbit=0; srcbit < width; srcbit++) 
	{
            if (pword & (1L << srcbit))
                tmpword |= (1L << dstbit);
            dstbit++;
            if (dstbit >= 32)
            {
                WaitQueue(1);
                regw(HOST_DATA0, tmpword);
                dstbit = 0;
                tmpword = 0;
            }
        }
	psrc += pwidth;
    }
    if (dstbit > 0)
    {
	WaitQueue(1);
	regw(HOST_DATA0, tmpword);
    }
    
    WaitQueue(3);
    regw(DP_PIX_WIDTH, old_DP_PIX_WIDTH);
    regw(DST_OFF_PITCH, old_DST_OFF_PITCH);

    WaitIdleEmpty();
}


int
mach64CacheFont(font)
    FontPtr font;
{
    int i, j, c;
    int ret = -1;
    unsigned long n;
    unsigned char chr;
    int width, height;
    int nbyLine;
    unsigned char *pb, *pbits;
    CharInfoPtr pci;
    unsigned char *pglyph;
    int gWidth, gHeight;
    int nbyGlyphWidth;
    int nbyPadGlyph;

    if (NumFonts == 0)
	return -1;

    for (i = 0; i < NumFonts; i++)
	if (mach64FontCache[i].font == font)
	    ret = i;

    width = FONTMAXBOUNDS(font,rightSideBearing) -
	    FONTMINBOUNDS(font,leftSideBearing);
    height = FONTMAXBOUNDS(font,ascent) + FONTMAXBOUNDS(font,descent);

    if ((ret == -1) && (width <= FC_MAX_WIDTH) && (height <= FC_MAX_HEIGHT) &&
	(FONTFIRSTROW(font) == 0) && (FONTLASTROW(font) == 0) &&
	(FONTLASTCOL(font) < 256)) {
       
        int mach64FC_X, mach64FC_Y;
	
	ret = 0;
	for (i = 1; i < NumFonts; i++)
	    if (mach64FontCache[i].lru > mach64FontCache[ret].lru)
		ret = i;

	nbyLine = PixmapBytePad(width, 1);
	pbits = (unsigned char *)ALLOCATE_LOCAL(height*nbyLine);
	if (!pbits) {
	    return -1;
	}

	mach64FontCache[ret].font = font;
	mach64FontCache[ret].lru = 0;
	mach64FC_X = mach64FontCache[ret].x;
	mach64FC_Y = mach64FontCache[ret].y;

	for (c = 0; c < 256; c++) {
	    chr = (unsigned char)c;
	    GetGlyphs(font, 1, &chr, Linear8Bit, &n, &pci);
	    if (n == 0) {
		mach64FontCache[ret].pci[c] = NULL;
	    } else {
		mach64FontCache[ret].pci[c] = pci;
		pglyph = FONTGLYPHBITS(pglyphBase, pci);
		gWidth = GLYPHWIDTHPIXELS(pci);
		gHeight = GLYPHHEIGHTPIXELS(pci);
		if (gWidth && gHeight) {
		    nbyGlyphWidth = GLYPHWIDTHBYTESPADDED(pci);
		    nbyPadGlyph = PixmapBytePad(gWidth, 1);
		    
		    if (nbyGlyphWidth == nbyPadGlyph
#if GLYPHPADBYTES != 4
			&& (((int) pglyph) & 3) == 0
#endif
			) {
			pb = pglyph;
		    } else {
			for (i = 0, pb = pbits;
			     i < gHeight;
			     i++, pb = pbits+(i*nbyPadGlyph))
			    for (j = 0; j < nbyGlyphWidth; j++)
				*pb++ = *pglyph++;
			pb = pbits;
		    }
		    mach64CacheGlyph(mach64FC_X, (mach64FC_Y + c),
				     gWidth, gHeight, pb, nbyPadGlyph);
		}
	    }
	}
    }

    return ret;
}


int
mach64DrawText(pDraw, pGC, x, y, count, chars, slot, texttype)
    DrawablePtr pDraw;
    GCPtr pGC;
    int x;
    int y;
    int count;
    unsigned char *chars;
    int slot;
    int texttype;

{
    int           screenX;              /* destination coords */
    int           screenY;              /* destination coords */
    int           charWidth,            /* char width (pixels) */
                  charHeight;           /* char height (pixels) */
    int           fontAscent,           /* font ascent (pixels) */
                  fontDescent;          /* font descent (pixels) */
    int           rectX1;
    int           rectX2;               /* image text clipped rect X */
    int           rectY1;
    int           rectY2;               /* image text clipped rect Y */
    int           width;
    int           nClipRects;           /* counter # of clip rects */
    BoxPtr        pClipRects;           /* ptr to clip rects */
    int           offset;
    int           i;                    /* local counter index */
    int           nClipRectsInit;       /* Initial # of clip rects */
    BoxPtr        pClipRectsInit;       /* Initial ptr to clip rects */
    FontPtr       pfont;
    CharInfoPtr   pci;
    CacheFontRec *mach64FCP;
    int           ret_val;
    int 	  old_SRC_OFF_PITCH;
    int 	  old_DP_PIX_WIDTH;


    if (count == 0)
	return 0;

    pfont = pGC->font;
    mach64FCP = &(mach64FontCache[slot]);

    if ((texttype & IMAGE_TEXT_TYPE) && (FONTMAXBOUNDS(pfont,characterWidth) ==
         FONTMINBOUNDS(pfont,characterWidth)))
    {
        width = count * FONTMAXBOUNDS(pfont,characterWidth);
    }
    else
    {
        for (width = 0, i = 0; i < count; i++)
        {
            width += mach64FCP->pci[(int)chars[i]] ?
                     mach64FCP->pci[(int)chars[i]]->metrics.characterWidth : 0;
        }
    }
    ret_val = x + width;

    for (i = 0; i < NumFonts; i++) 
    {
        if (i != slot && mach64FontCache[i].lru != 0xffffffff)
            mach64FontCache[i].lru++;
    }

    x += pDraw->x;
    y += pDraw->y;

    nClipRectsInit = REGION_NUM_RECTS(((cfbPrivGC *)
                (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
    pClipRectsInit = REGION_RECTS(((cfbPrivGC *)
                (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);

    fontAscent  = y - pfont->info.fontAscent;
    fontDescent = y + pfont->info.fontDescent;

    WaitQueue(4);
    old_SRC_OFF_PITCH = regr(SRC_OFF_PITCH);
    old_DP_PIX_WIDTH = regr(DP_PIX_WIDTH);
    regw(DP_WRITE_MASK, pGC->planemask);
    regw(DST_CNTL, DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM);

    if (texttype & IMAGE_TEXT_TYPE)
    {
        if (!(TERMINALFONT(pfont)))
        {
            xRectangle    imageRect;          /* image text filled rect */

	    WaitQueue(3);
	    regw(DP_FRGD_CLR, pGC->bgPixel);
	    regw(DP_MIX, (MIX_SRC << 16) | MIX_DST);
	    regw(DP_SRC, FRGD_SRC_FRGD_CLR);

            imageRect.x = x;
            imageRect.y = fontAscent;
            imageRect.width = width;
            imageRect.height = fontDescent - fontAscent;

            /* clip and draw the background rectangle */

            for (nClipRects = nClipRectsInit, pClipRects = pClipRectsInit;
                 nClipRects > 0;
                 nClipRects--, pClipRects++)
            {
                rectX1 = max(imageRect.x, pClipRects->x1);
                rectY1 = max(imageRect.y, pClipRects->y1);
                rectX2 = min((imageRect.x + imageRect.width), pClipRects->x2);
                rectY2 = min((imageRect.y + imageRect.height), pClipRects->y2);
    
                charWidth = rectX2 - rectX1;
                charHeight = rectY2 - rectY1;

                if ((charWidth > 0) && (charHeight > 0))
                {
	            WaitQueue(2);
	            regw(DST_Y_X, ((rectX1 << 16) | (rectY1 & 0x0000ffff)));
	            regw(DST_HEIGHT_WIDTH, ((charWidth << 16) | 
					    (charHeight & 0x0000ffff)));
	        }
            } 

	    WaitQueue(6);
	    regw(DP_FRGD_CLR, pGC->fgPixel);
	    regw(DP_BKGD_CLR, 0xffffffff);
	    regw(DP_MIX, (MIX_SRC << 16) | MIX_NOT_SRC_OR_DST);

        } /* end if !TERMINALFONT */
	else
	{
	    WaitQueue(6);
	    regw(DP_FRGD_CLR, pGC->fgPixel);
	    regw(DP_BKGD_CLR, pGC->bgPixel);
	    regw(DP_MIX, (MIX_SRC << 16) | MIX_SRC);
	}
    }
    else  /* POLY_TEXT_TYPE */
    {
	WaitQueue(6);
	regw(DP_FRGD_CLR, pGC->fgPixel);
        regw(DP_BKGD_CLR, 0xffffffff);
	regw(DP_MIX, (mach64alu[pGC->alu] << 16) | MIX_NOT_SRC_OR_DST);
    }

    switch(mach64InfoRec.bitsPerPixel)
    {
        case 8:
            regw(DP_PIX_WIDTH, BYTE_ORDER_LSB_TO_MSB | SRC_1BPP | DST_8BPP);
            break;
        case 16:
            regw(DP_PIX_WIDTH, BYTE_ORDER_LSB_TO_MSB | SRC_1BPP | DST_16BPP);
            break;
        case 32:
            regw(DP_PIX_WIDTH, BYTE_ORDER_LSB_TO_MSB | SRC_1BPP | DST_32BPP);
            break;
    }
    regw(DP_SRC, MONO_SRC_BLIT | FRGD_SRC_FRGD_CLR | BKGD_SRC_BKGD_CLR);
    regw(SRC_CNTL, SRC_LINEAR_ENABLE);

    for (nClipRects = nClipRectsInit, pClipRects = pClipRectsInit;
         nClipRects > 0;
         nClipRects--, pClipRects++)
    {
        rectX1 = pClipRects->x1;
        rectY1 = pClipRects->y1;
        rectY2 = pClipRects->y2;
        rectX2 = pClipRects->x2;
        
	/* check to see if entire string is outside of clipping rect */
	if ((fontAscent > rectY2) || (fontDescent < rectY1) || 
            (x > rectX2) || ((x + width) < rectX1))
        {
	    continue; 
        }
        
        screenX = x;
        screenY = y;

        WaitQueue(2);
        regw(SC_LEFT_RIGHT, (((rectX2 - 1) << 16) | rectX1 ));
        regw(SC_TOP_BOTTOM, (((rectY2 - 1) << 16) | rectY1 ));

        for (i = 0; i < count; i++)  
        {
	    pci = mach64FCP->pci[(int)chars[i]];
	    if (pci == NULL) 
		continue;

            charWidth = GLYPHWIDTHPIXELS(pci);
            charHeight = GLYPHHEIGHTPIXELS(pci);

	    /* Perform trivial reject on a per char basis */
            if ((screenX > rectX2) || ((screenX+charWidth) < rectX1))
	    {
		screenX += pci->metrics.characterWidth;
                continue;
	    }

            if((charWidth <= 0) || (charHeight <= 0))
	    {
		screenX += pci->metrics.characterWidth;
                continue;
	    }

	    offset = ((((mach64FCP->y + (int)chars[i]) * mach64VirtX) + 
			mach64FCP->x) * (mach64InfoRec.bitsPerPixel / 8)) >> 3;

            WaitQueue(3);
            regw(SRC_OFF_PITCH, (old_SRC_OFF_PITCH & 0xffc00000) | offset);
            regw(DST_Y_X, (((screenX + pci->metrics.leftSideBearing) << 16) |
			   ((screenY - pci->metrics.ascent) & 0x0000ffff)));
            regw(DST_HEIGHT_WIDTH, ((charWidth << 16) | charHeight));

	    screenX += pci->metrics.characterWidth;
        } 
    }

    WaitQueue(5);
    regw(SC_LEFT_RIGHT, ((mach64MaxX << 16) | 0 ));
    regw(SC_TOP_BOTTOM, ((mach64MaxY << 16) | 0 ));
    regw(SRC_CNTL, 0x00000000);
    regw(DP_PIX_WIDTH, old_DP_PIX_WIDTH);
    regw(SRC_OFF_PITCH, old_SRC_OFF_PITCH);

    WaitIdleEmpty(); /* Make sure that all commands have finished */

    return ret_val;
}

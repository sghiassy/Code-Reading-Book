/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86im.c,v 3.2 1997/01/12 10:48:09 dawes Exp $ */

/*
 * Copyright 1996  The XFree86 Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * HARM HANEMAAYER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Written by Harm Hanemaayer (hhanemaa@inter.nl.net).
 */

/*
 * Contents:
 *
 * xf86ImageWriteFallBack()
 *     Low-level ImageWrite fall-back, uses cfb.
 * xf86WriteBitmapFallBack()
 *     Low-level WriteBitmap fall-back, uses cfb CopyPlane1toN.
 *     Not defined for 24bpp.
 * xf86cfbGetLongWidthAndPointer()
 *     Function version of cfb macro (required to use different-depth
 *     versions from a source that is only compiled once).
 *
 * This files is compiled seperately for each depth.
 */

#include "xf86.h"
#ifdef VGA256
#include "vga256.h"
#include "vga.h"
#else
#include "cfb.h"
#include "cfbmskbits.h"
#include "cfb8bit.h"
#endif
#include "scrnintstr.h"
#include "windowstr.h"
#include "dixstruct.h"
#include "regionstr.h"
#include "pixmap.h"
#include "gcstruct.h"

#include "xf86xaa.h"
#include "xf86gcmap.h"
#include "xf86maploc.h"
#include "xf86local.h"
#include "xf86Priv.h"


/*
 * This is a low-level fall-back for ImageWrite, which calls cfb.
 * It is used when no hardware ImageWrite function is available.
 * It's used when writing a tile to the pixmap cache.
 */

extern WindowPtr *WindowTable;

void xf86ImageWriteFallBack(x, y, w, h, src, srcwidth, rop, planemask)
    int x;
    int y;
    int w;
    int h;
    void *src;
    int srcwidth;
    int rop;
    unsigned planemask;
{
    ScreenPtr pScreen;
    PixmapPtr pix;
    WindowPtr rootWin;
    BoxRec box;
    DDXPointRec ptSrc;
    RegionRec rgnDst;
    pScreen = screenInfo.screens[xf86ScreenIndex];
    rootWin = WindowTable[pScreen->myNum];
    pix = GetScratchPixmapHeader(pScreen, w, h, rootWin->drawable.depth,
        rootWin->drawable.bitsPerPixel, srcwidth, src);
    ptSrc.x = 0;
    ptSrc.y = 0;
    box.x1 = x;
    box.y1 = y;
    box.x2 = x + w;
    box.y2 = y + h;
    REGION_INIT(pScreen, &rgnDst, &box, 1);
#ifdef VGA256
    vga256DoBitblt((DrawablePtr)pix, (DrawablePtr)rootWin, rop, &rgnDst, &ptSrc,
        planemask);
#else    
    cfbDoBitblt((DrawablePtr)pix, (DrawablePtr)rootWin, rop, &rgnDst, &ptSrc,
        planemask);
#endif
    REGION_UNINIT(pScreen, &rgnDst);
    FreeScratchPixmapHeader(pix);
}

/*
 * In a similar vein, this is for expanding a monochrome bitmap.
 */

void xf86WriteBitmapFallBack(x, y, w, h, src, srcwidth, srcx, srcy,
bg, fg, rop, planemask)
    int x;
    int y;
    int w;
    int h;
    unsigned char *src;
    int srcwidth;
    int srcx;
    int srcy;
    int bg;
    int fg;
    int rop;
    unsigned planemask;
{
    ScreenPtr pScreen;
    PixmapPtr pix;
    WindowPtr rootWin;
    BoxRec box;
    DDXPointRec ptSrc;
    RegionRec rgnDst;
    pScreen = screenInfo.screens[xf86ScreenIndex];
    rootWin = WindowTable[pScreen->myNum];
    pix = GetScratchPixmapHeader(pScreen, w, h, 1, 1, srcwidth, src);
    ptSrc.x = srcx;
    ptSrc.y = srcy;
    box.x1 = x;
    box.y1 = y;
    box.x2 = x + w;
    box.y2 = y + h;
    REGION_INIT(pScreen, &rgnDst, &box, 1);
#ifdef VGA256
    cfb8CheckOpaqueStipple(rop, fg, bg, planemask);
    vga256CopyPlane1to8((DrawablePtr)pix, (DrawablePtr)rootWin, rop, &rgnDst,
        &ptSrc, planemask, 1);
#else
#if PSZ == 8
    cfb8CheckOpaqueStipple(rop, fg, bg, planemask);
    cfbCopyPlane1to8((DrawablePtr)pix, (DrawablePtr)rootWin, rop, &rgnDst,
        &ptSrc, planemask, 1);
#endif
#if PSZ == 16
    cfbCopyPlane1to16((DrawablePtr)pix, (DrawablePtr)rootWin, rop, &rgnDst,
        &ptSrc, planemask, 1, bg, fg);
#endif
#if PSZ == 24
    cfbCopyPlane1to24((DrawablePtr)pix, (DrawablePtr)rootWin, rop, &rgnDst,
        &ptSrc, planemask, 1, bg, fg);
#endif
#if PSZ == 32
    cfbCopyPlane1to32((DrawablePtr)pix, (DrawablePtr)rootWin, rop, &rgnDst,
        &ptSrc, planemask, 1, bg, fg);
#endif
#endif
    REGION_UNINIT(pScreen, &rgnDst);
    FreeScratchPixmapHeader(pix);
}

#ifdef VGA256
extern Bool vgaUseLinearAddressing;
#endif

void xf86cfbGetLongWidthAndPointer(pDrawable, nlwidth, addrl)
    DrawablePtr pDrawable;
    int *nlwidth;
    unsigned long **addrl;
{
    cfbGetLongWidthAndPointer(pDrawable, *nlwidth, *addrl);
#ifdef VGA256
    /*
     * This is a temporary hack to really use the non-bankchecking routines
     * in the fXF86 functions if linear addressing is enabled.
     *
     * We do this only if vga256 is actually used.
     */
    if (xf86AccelInfoRec.UsingVGA256 && vgaUseLinearAddressing)
         *addrl = (unsigned long *)((unsigned char *)vgaLinearBase +
             (unsigned long)((unsigned char *)(*addrl) - (unsigned long)VGABASE));
#endif
}

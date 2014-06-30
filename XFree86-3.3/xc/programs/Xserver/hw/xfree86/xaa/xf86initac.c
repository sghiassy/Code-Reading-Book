/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86initac.c,v 3.10.2.2 1997/05/18 12:00:21 dawes Exp $ */

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
 * Written by Harm Hanemaayer (H.Hanemaayer@inter.nl.net).
 */

#include "servermd.h"
#include "windowstr.h"
#include "gcstruct.h"
#include "regionstr.h"
#include "mi.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86xaa.h"
#include "xf86gcmap.h"
#include "xf86local.h"
#include "xf86pcache.h"

/*
 * This is just for the messages.
 */
#include "xf86_Config.h"

#ifdef PIXPRIV
/*
 * This is a convenient place to declare this variable (this file is only
 * compiled once).
 */
int xf86PixmapIndex;
#endif

#define STIPPLE_COLOR_EXPANSION

void
xf86InitializeAcceleration(pScreen)
    ScreenPtr pScreen;
{
    int SimpleFillRectSolid = FALSE;
    int SimpleScreenToScreenCopy = FALSE;
    int CPUToScreenColorExpand = FALSE;
    int ScreenToScreenColorExpand = FALSE;
    int ScanlineScreenToScreenColorExpand = FALSE;
 
    if (serverGeneration != 1)
        goto do_not_touch_xf86AccelInfoRec;

    ErrorF("%s %s: Using XAA (XFree86 Acceleration Architecture)\n",
        XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);

    if ((xf86AccelInfoRec.Flags & PIXMAP_CACHE) &&
    OFLG_ISSET(OPTION_NO_PIXMAP_CACHE,
    &(xf86AccelInfoRec.ServerInfoRec->options))) {
        xf86AccelInfoRec.Flags &= ~PIXMAP_CACHE;
        ErrorF("%s %s: Pixmap cache disabled\n",
            XCONFIG_GIVEN, xf86AccelInfoRec.ServerInfoRec->name);
    }

    /*
     * Probe the low-level accelerated functions supported.
     */

    if (xf86AccelInfoRec.SetupForFillRectSolid &&
    xf86AccelInfoRec.SubsequentFillRectSolid) {
        SimpleFillRectSolid = TRUE;
        if (xf86Verbose)
            ErrorF("%s %s: XAA: Solid filled rectangles\n",
                XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
    }
    if (xf86AccelInfoRec.SetupForScreenToScreenCopy &&
    xf86AccelInfoRec.SubsequentScreenToScreenCopy) {
        SimpleScreenToScreenCopy = TRUE;
        if (xf86Verbose)
            ErrorF("%s %s: XAA: Screen-to-screen copy\n",
                XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
    }
    if (xf86AccelInfoRec.SetupForCPUToScreenColorExpand &&
    xf86AccelInfoRec.SubsequentCPUToScreenColorExpand &&
    !OFLG_ISSET(OPTION_XAA_NO_COL_EXP,
    &(xf86AccelInfoRec.ServerInfoRec->options))) {
        CPUToScreenColorExpand = TRUE;
#if 0
        /* There's a more useful message below. */
        ErrorF("%s %s: XAA: CPU-to-screen color expand\n",
            XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
#endif
    }
    if (xf86AccelInfoRec.SetupForScreenToScreenColorExpand &&
    xf86AccelInfoRec.SubsequentScreenToScreenColorExpand &&
    !OFLG_ISSET(OPTION_XAA_NO_COL_EXP,
    &(xf86AccelInfoRec.ServerInfoRec->options))) {
        ScreenToScreenColorExpand = TRUE;
#if 0
        ErrorF("%s %s: XAA: Screen-to-screen color expand\n",
            XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
#endif
    }
    if (xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand &&
    xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand) {
        ScanlineScreenToScreenColorExpand = TRUE;
#if 0
        if (xf86Verbose)
            ErrorF("%s %s: XAA: Scanline screen-to-screen color expand\n",
                XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
#endif
    }
    if (xf86AccelInfoRec.SetupForFill8x8Pattern &&
    xf86AccelInfoRec.SubsequentFill8x8Pattern) {
        if (xf86Verbose)
            ErrorF("%s %s: XAA: 8x8 pattern fill",
                XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
        if (((xf86AccelInfoRec.Flags & HARDWARE_PATTERN_ALIGN_64) &&
        !(xf86AccelInfoRec.Flags & HARDWARE_PATTERN_PROGRAMMED_ORIGIN))
        || ((xf86AccelInfoRec.Flags & HARDWARE_PATTERN_MOD_64_OFFSET) &&
        (xf86AccelInfoRec.FramebufferWidth & 63) != 0))
            ErrorF(" (not usable)");
        ErrorF("\n");
    }
    if (xf86AccelInfoRec.SetupFor8x8PatternColorExpand &&
    xf86AccelInfoRec.Subsequent8x8PatternColorExpand) {
        if (xf86Verbose)
            ErrorF("%s %s: XAA: 8x8 color expand pattern fill\n",
                XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
    }

    if (!SimpleFillRectSolid && !SimpleScreenToScreenCopy &&
    !CPUToScreenColorExpand && !ScreenToScreenColorExpand &&
    !ScanlineScreenToScreenColorExpand &&
    !xf86AccelInfoRec.SubsequentTwoPointLine &&
    !xf86AccelInfoRec.SubsequentBresenhamLine &&
    !xf86AccelInfoRec.SubsequentFill8x8Pattern &&
    !xf86AccelInfoRec.Subsequent8x8PatternColorExpand)
        ErrorF("%s %s: XAA: No acceleration primitives defined.\n",
            XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);

    /*
     * Set up the higher-level accelerated functions to make use of
     * the available lower-level functions.
     */

    /* Solid filled rectangles. */
    if (SimpleFillRectSolid && !xf86AccelInfoRec.FillRectSolid)
        xf86AccelInfoRec.FillRectSolid = xf86SimpleFillRectSolid;
    if (xf86AccelInfoRec.FillRectSolid && !xf86GCInfoRec.PolyFillRectSolid)
        xf86GCInfoRec.PolyFillRectSolid = xf86PolyFillRect;

    /* Various solid fills. */
    if (SimpleFillRectSolid) {
        if (!xf86GCInfoRec.FillSpansSolid) {
            xf86GCInfoRec.FillSpansSolid = xf86FillSpansAsRects;
            xf86GCInfoRec.FillSpansSolidFlags =
                 xf86GCInfoRec.PolyFillRectSolidFlags;
        }
        /* Solid filled arcs. */
        if (!xf86GCInfoRec.PolyFillArcSolid) {
            xf86GCInfoRec.PolyFillArcSolid = xf86PolyFillArcSolid;
            xf86GCInfoRec.PolyFillArcSolidFlags =
                xf86GCInfoRec.PolyFillRectSolidFlags;
        }
        /* Solid filled polygons. */
        if (!xf86GCInfoRec.FillPolygonSolid) {
            xf86GCInfoRec.FillPolygonSolid = xf86FillPolygonSolid1Rect;
            xf86GCInfoRec.FillPolygonSolidFlags =
                xf86GCInfoRec.PolyFillRectSolidFlags | ONE_RECT_CLIPPING;
        }
        /* Line rectangles. */
        if (!xf86GCInfoRec.PolyRectangleSolidZeroWidth) {
            xf86GCInfoRec.PolyRectangleSolidZeroWidth =
                xf86PolyRectangleSolidZeroWidth;
            xf86GCInfoRec.PolyRectangleSolidZeroWidthFlags =
                xf86GCInfoRec.PolyFillRectSolidFlags;
	}
    }

    /* Screen-to-screen BitBLT. */
    if (SimpleScreenToScreenCopy && !xf86AccelInfoRec.ScreenToScreenBitBlt)
        xf86AccelInfoRec.ScreenToScreenBitBlt = xf86ScreenToScreenBitBlt;
    if (xf86AccelInfoRec.ScreenToScreenBitBlt && !xf86GCInfoRec.CopyArea) {
        xf86GCInfoRec.CopyArea = xf86CopyArea;
    }

    /* Color expansion. */
    
    /*
     * Make decisions based on the specified range for CPU-to-screen
     * color expansion. We need at least a scanline worth of range.
     * The EndMarker is defined as the highest possible base address
     * where there's still space for a complete scanline within the
     * allowed range.
     */
    if (CPUToScreenColorExpand) {
        int scanlinebytes;
        if (xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)
            scanlinebytes = ((xf86AccelInfoRec.FramebufferWidth * 3 + 31)
                & ~31) / 8;
	else
            scanlinebytes = ((xf86AccelInfoRec.FramebufferWidth + 31)
                & ~31) / 8;
        if (!xf86AccelInfoRec.CPUToScreenColorExpandBase)
            /* No base specified, assume start of framebuffer. */
            xf86AccelInfoRec.CPUToScreenColorExpandBase =
                (unsigned int *)xf86AccelInfoRec.FramebufferBase;
        if (!xf86AccelInfoRec.CPUToScreenColorExpandRange) {
            /* No range specified, assume a 64K range. */
            xf86AccelInfoRec.CPUToScreenColorExpandRange = 65536;
            xf86AccelInfoRec.CPUToScreenColorExpandEndMarker = (unsigned int *)
                ((unsigned char *)xf86AccelInfoRec.CPUToScreenColorExpandBase
                + 65536 - scanlinebytes);
        }
        else {
            if (xf86AccelInfoRec.CPUToScreenColorExpandRange >= scanlinebytes)
                /*
                 * OK, the range is large enough for a scanline, so we can
                 * use color expansion.
                 */
                xf86AccelInfoRec.CPUToScreenColorExpandEndMarker =
                    (unsigned int *)((unsigned char *)
                    xf86AccelInfoRec.CPUToScreenColorExpandBase +
                    xf86AccelInfoRec.CPUToScreenColorExpandRange
                    - scanlinebytes);
            else
                /* Alas, not a large enough range. */
                xf86AccelInfoRec.ColorExpandFlags |= CPU_TRANSFER_BASE_FIXED;
        }
    }

    if (CPUToScreenColorExpand
/*    && (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_DWORD) */
/*    && !(xf86AccelInfoRec.ColorExpandFlags & CPU_TRANSFER_BASE_FIXED) */
        /* TRIPLE_BITS_24BPP requires SCANLINE_PAD_DWORD. */
        && ((xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_DWORD)
        || !(xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP))
    ) {
        if (xf86Verbose)
            ErrorF("%s %s: XAA: CPU to screen color expansion (",	
                XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
        if ((!(xf86AccelInfoRec.ColorExpandFlags & ONLY_TRANSPARENCY_SUPPORTED)
        || SimpleFillRectSolid)
        && (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_DWORD)) {
            xf86AccelInfoRec.WriteBitmap =
                xf86WriteBitmapCPUToScreenColorExpand;
            if (xf86Verbose)
                ErrorF("bitmap, ");
#ifdef STIPPLE_COLOR_EXPANSION
            if ((xf86AccelInfoRec.ColorExpandFlags & LEFT_EDGE_CLIPPING) &&
            (xf86AccelInfoRec.ColorExpandFlags & LEFT_EDGE_CLIPPING_NEGATIVE_X)
            && !(xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)) {
                xf86AccelInfoRec.FillRectOpaqueStippled =
                    xf86FillRectStippledCPUToScreenColorExpand;
                xf86GCInfoRec.PolyFillRectOpaqueStippledFlags =
                    xf86AccelInfoRec.ColorExpandFlags;
                xf86GCInfoRec.SecondaryPolyFillRectOpaqueStippledFlags =
                    xf86AccelInfoRec.ColorExpandFlags;
                if (xf86Verbose)
                    ErrorF("stipple, ");
            }
#endif
        }
#ifdef STIPPLE_COLOR_EXPANSION
        if (!(xf86AccelInfoRec.ColorExpandFlags & NO_TRANSPARENCY)
        && (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_DWORD) &&
        (xf86AccelInfoRec.ColorExpandFlags & LEFT_EDGE_CLIPPING) &&
        (xf86AccelInfoRec.ColorExpandFlags & LEFT_EDGE_CLIPPING_NEGATIVE_X) &&
        !(xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)) {
            xf86AccelInfoRec.FillRectStippled =
                xf86FillRectStippledCPUToScreenColorExpand;
            xf86GCInfoRec.PolyFillRectStippledFlags =
                xf86AccelInfoRec.ColorExpandFlags;
            xf86GCInfoRec.SecondaryPolyFillRectStippledFlags =
                xf86AccelInfoRec.ColorExpandFlags;
        }
#endif
        if (!xf86AccelInfoRec.ImageTextTE && !xf86GCInfoRec.ImageGlyphBltTE &&
        (!(xf86AccelInfoRec.ColorExpandFlags & ONLY_TRANSPARENCY_SUPPORTED)
        || SimpleFillRectSolid)
        && !(xf86AccelInfoRec.Flags & NO_TEXT_COLOR_EXPANSION)
        ) {
            xf86AccelInfoRec.ImageTextTE =
                xf86ImageTextTECPUToScreenColorExpand;
            xf86GCInfoRec.ImageGlyphBltTEFlags =
                xf86AccelInfoRec.ColorExpandFlags;
            if (xf86Verbose)
                ErrorF("TE");
            /* Only DWORD padding currently supported */
            if (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_DWORD &&
		SimpleFillRectSolid) {
                xf86AccelInfoRec.ImageTextNonTE =
                     xf86ImageTextNonTECPUToScreenColorExpand;
                xf86GCInfoRec.ImageGlyphBltNonTEFlags =
                     xf86AccelInfoRec.ColorExpandFlags;
                if (xf86Verbose)
                    ErrorF("/NonTE");
            }
            if (xf86Verbose)
                ErrorF(" imagetext");
        }
        if (!xf86AccelInfoRec.PolyTextTE && !xf86GCInfoRec.PolyGlyphBltTE
        && !(xf86AccelInfoRec.ColorExpandFlags & NO_TRANSPARENCY)
        && !(xf86AccelInfoRec.Flags & NO_TEXT_COLOR_EXPANSION)
        ) {
            if (xf86Verbose)
                ErrorF(", TE");
            xf86AccelInfoRec.PolyTextTE =
                xf86PolyTextTECPUToScreenColorExpand;
            xf86GCInfoRec.PolyGlyphBltTEFlags =
                xf86AccelInfoRec.ColorExpandFlags;
            /* Only DWORD padding currently supported */
            if (xf86AccelInfoRec.ColorExpandFlags & SCANLINE_PAD_DWORD) {
                xf86AccelInfoRec.PolyTextNonTE =
                     xf86PolyTextNonTECPUToScreenColorExpand;
                xf86GCInfoRec.PolyGlyphBltNonTEFlags =
                     xf86AccelInfoRec.ColorExpandFlags;
                if (xf86Verbose)
                    ErrorF("/NonTE");
            }
            if (xf86Verbose)
                ErrorF(" polytext");
        }
        if (xf86Verbose)
            ErrorF(")\n");
    }
    else
    if (ScanlineScreenToScreenColorExpand
    && xf86AccelInfoRec.ScratchBufferAddr
    && (xf86AccelInfoRec.FramebufferBase
    || xf86AccelInfoRec.ScratchBufferBase)) {
        if (xf86Verbose)
            ErrorF("%s %s: XAA: Indirect CPU to screen color expansion "
                "(", XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
        /*
         * The default address of the scratch buffer is FramebufferBase
         * (pointer to linear framebuffer) + ScratchBufferAddr (offset
         * in bytes into framebuffer).
         * However, if ScratchBufferBase is defined, ScratchBufferBase is
         * taken to be the pointer to the scratch buffer.
         * In both cases, ScratchBufferAddr is passed to the
         * SubsequentScreenToScreenColorExpand primitive.
         */
        if (!xf86AccelInfoRec.ScratchBufferBase)
            xf86AccelInfoRec.ScratchBufferBase =
                xf86AccelInfoRec.FramebufferBase
                + xf86AccelInfoRec.ScratchBufferAddr;
        if ((xf86AccelInfoRec.ColorExpandFlags &
        VIDEO_SOURCE_GRANULARITY_PIXEL) &&
        (!(xf86AccelInfoRec.ColorExpandFlags & ONLY_TRANSPARENCY_SUPPORTED)
        || SimpleFillRectSolid)
        ) {
            xf86AccelInfoRec.WriteBitmap =
                xf86WriteBitmapScreenToScreenColorExpand;
            if (xf86Verbose)
                ErrorF("bitmap, ");
#ifdef STIPPLE_COLOR_EXPANSION
            if (!(xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)) {
                xf86AccelInfoRec.FillRectOpaqueStippled =
                    xf86FillRectStippledScreenToScreenColorExpand;
                xf86GCInfoRec.PolyFillRectOpaqueStippledFlags =
                    xf86AccelInfoRec.ColorExpandFlags;
                if (xf86Verbose)
                    ErrorF("stipple, ");
            }
#endif
        }
#ifdef STIPPLE_COLOR_EXPANSION
        if ((xf86AccelInfoRec.ColorExpandFlags &
        VIDEO_SOURCE_GRANULARITY_PIXEL) &&
        !(xf86AccelInfoRec.ColorExpandFlags & NO_TRANSPARENCY)
        && !(xf86AccelInfoRec.ColorExpandFlags & TRIPLE_BITS_24BPP)) {
            xf86AccelInfoRec.FillRectStippled =
                xf86FillRectStippledScreenToScreenColorExpand;
            xf86GCInfoRec.PolyFillRectStippledFlags =
                xf86AccelInfoRec.ColorExpandFlags;
        }
#endif
        if (!xf86AccelInfoRec.ImageTextTE && !xf86GCInfoRec.ImageGlyphBltTE &&
        (!(xf86AccelInfoRec.ColorExpandFlags & ONLY_TRANSPARENCY_SUPPORTED)
        || SimpleFillRectSolid)
        && !(xf86AccelInfoRec.Flags & NO_TEXT_COLOR_EXPANSION)
        ) {
            xf86AccelInfoRec.ImageTextTE =
                xf86ImageTextTEScreenToScreenColorExpand;
            xf86GCInfoRec.ImageGlyphBltTEFlags =
                xf86AccelInfoRec.ColorExpandFlags;
            xf86AccelInfoRec.ImageTextNonTE =
                xf86ImageTextNonTEScreenToScreenColorExpand;
            xf86GCInfoRec.ImageGlyphBltNonTEFlags =
                xf86AccelInfoRec.ColorExpandFlags;
            if (xf86Verbose)
                ErrorF("imagetext");
        }
        if (!xf86AccelInfoRec.PolyTextTE && !xf86GCInfoRec.PolyGlyphBltTE &&
        !(xf86AccelInfoRec.ColorExpandFlags & NO_TRANSPARENCY)
        && !(xf86AccelInfoRec.Flags & NO_TEXT_COLOR_EXPANSION)
        ) {
            xf86AccelInfoRec.PolyTextTE =
                xf86PolyTextTEScreenToScreenColorExpand;
            xf86GCInfoRec.PolyGlyphBltTEFlags =
                xf86AccelInfoRec.ColorExpandFlags;
            xf86AccelInfoRec.PolyTextNonTE =
                xf86PolyTextNonTEScreenToScreenColorExpand;
            xf86GCInfoRec.PolyGlyphBltNonTEFlags =
                xf86AccelInfoRec.ColorExpandFlags;
            if (xf86Verbose)
                ErrorF(", polytext");
        }
        if (xf86Verbose)
            ErrorF(")\n");
    }

    /* Tile and stipple filled rectangles (non-cached). */
    if (xf86AccelInfoRec.FillRectTiled && !xf86GCInfoRec.PolyFillRectTiled)
        xf86GCInfoRec.PolyFillRectTiled = xf86PolyFillRect;
    if (xf86AccelInfoRec.FillRectStippled
    && !xf86GCInfoRec.PolyFillRectStippled)
        xf86GCInfoRec.PolyFillRectStippled = xf86PolyFillRect;
    if (xf86AccelInfoRec.FillRectOpaqueStippled
    && !xf86GCInfoRec.PolyFillRectOpaqueStippled)
        xf86GCInfoRec.PolyFillRectOpaqueStippled = xf86PolyFillRect;

    /*
     * Text. Has a dependency on color expansion.
     * This part just installs the high level hooks that check
     * for clipping, if lower-level hooks that don't check for clipping
     * exist.
     */
    if (!xf86GCInfoRec.ImageGlyphBltTE && xf86AccelInfoRec.ImageTextTE)
        xf86GCInfoRec.ImageGlyphBltTE = xf86ImageGlyphBltTE;
    if (!xf86GCInfoRec.ImageGlyphBltNonTE && xf86AccelInfoRec.ImageTextNonTE)
        xf86GCInfoRec.ImageGlyphBltNonTE = xf86ImageGlyphBltNonTE;
    if (!xf86GCInfoRec.PolyGlyphBltTE && xf86AccelInfoRec.PolyTextTE)
        xf86GCInfoRec.PolyGlyphBltTE = xf86PolyGlyphBltTE;
    if (!xf86GCInfoRec.PolyGlyphBltNonTE && xf86AccelInfoRec.PolyTextNonTE)
        xf86GCInfoRec.PolyGlyphBltNonTE = xf86PolyGlyphBltNonTE;

    /* Pixmap caching. */
    if ((xf86AccelInfoRec.Flags & PIXMAP_CACHE) && SimpleScreenToScreenCopy) {
        /* Perform the actual pixmap cache initialization/allocation. */
        xf86InitPixmapCacheSlots();
        if (xf86Verbose)
            ErrorF("%s %s: XAA: Caching tiles ",
	        XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
        xf86GCInfoRec.PolyFillRectTiled = xf86PolyFillRect;
        xf86GCInfoRec.PolyFillRectTiledFlags = xf86GCInfoRec.CopyAreaFlags;
        if (!xf86GCInfoRec.FillSpansTiled) {
            xf86GCInfoRec.FillSpansTiled = xf86FillSpansTiled;
            xf86GCInfoRec.FillSpansTiledFlags = xf86GCInfoRec.CopyAreaFlags;
        }
        /* Caching of stipples requires WriteBitmap or WriteBitmapFallBack. */
        if (xf86AccelInfoRec.WriteBitmap
        || xf86AccelInfoRec.WriteBitmapFallBack) {
            xf86GCInfoRec.PolyFillRectOpaqueStippled = xf86PolyFillRect;
            xf86GCInfoRec.PolyFillRectOpaqueStippledFlags =
                xf86GCInfoRec.CopyAreaFlags;
            /*
             * Caching of transparent stipples requires screen-to-screen
             * copy with transparency color compare. However, if there
             * is a color-expand 8x8 hardware pattern fill that supports
             * transparency, we might hope to use it.
             */
            if (!(xf86GCInfoRec.CopyAreaFlags & NO_TRANSPARENCY) ||
            (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_MONO_TRANSPARENCY)) {
                xf86GCInfoRec.PolyFillRectStippled = xf86PolyFillRect;
                xf86GCInfoRec.PolyFillRectStippledFlags =
                    xf86GCInfoRec.CopyAreaFlags;
                if (!xf86GCInfoRec.FillSpansStippled) {
                    xf86GCInfoRec.FillSpansStippled =
                        xf86FillSpansStippled;
                    xf86GCInfoRec.FillSpansStippledFlags =
                        xf86GCInfoRec.CopyAreaFlags;
                }
                if (xf86Verbose)
                    ErrorF("and stipples\n");
            }
            else
                if (xf86Verbose)
                    ErrorF("and non-transparent stipples\n");
        }
        else {
            /*
             * If we cache tiles but not stipples we must set
             * this flag, since stipples may be routed to
             * xf86PolyFillRect because of other types of
             * acceleration.
             */
            xf86AccelInfoRec.Flags |= DO_NOT_CACHE_STIPPLES;
            if (xf86Verbose)
                ErrorF("\n");
        }
    }

    /*
     * If there's any kind of accelerated stippled rectangle fill,
     * we'll assume it is beneficial to convert stippled spans to
     * rectangle fills.
     */
    if (xf86GCInfoRec.PolyFillRectOpaqueStippled &&
    !xf86GCInfoRec.FillSpansOpaqueStippled) {
         xf86GCInfoRec.FillSpansOpaqueStippled = xf86FillSpansStippled;
         xf86GCInfoRec.FillSpansOpaqueStippledFlags =
             xf86GCInfoRec.PolyFillRectOpaqueStippledFlags;
    }
    if (xf86GCInfoRec.PolyFillRectStippled &&
    !xf86GCInfoRec.FillSpansStippled) {
         xf86GCInfoRec.FillSpansStippled = xf86FillSpansStippled;
         xf86GCInfoRec.FillSpansStippledFlags =
             xf86GCInfoRec.PolyFillRectStippledFlags;
    }

    /*
     * General lines. At the moment, hardware clipping of lines
     * is a requirement for TwoPointLine.
     * For BresenhamLine, we can do software clipping if ErrorTermBits
     * is defined.
     * If no general lines are available, a function is used that
     * that speeds up horizontal and vertical lines using FillRectSolid,
     * and also takes advantage of any TwoPointLine for vertical and
     * non-clipped lines.
     */
        if ((xf86AccelInfoRec.SubsequentBresenhamLine
        && ((xf86AccelInfoRec.Flags & HARDWARE_CLIP_LINE) ||
        xf86AccelInfoRec.ErrorTermBits))
        || (xf86AccelInfoRec.SubsequentTwoPointLine
        && (xf86AccelInfoRec.Flags & HARDWARE_CLIP_LINE))) {
            if (!xf86GCInfoRec.PolyLineSolidZeroWidth)
                xf86GCInfoRec.PolyLineSolidZeroWidth = xf86PolyLine;
            if (!xf86GCInfoRec.PolySegmentSolidZeroWidth)
                xf86GCInfoRec.PolySegmentSolidZeroWidth = xf86PolySegment;
            /*
             * FillRectSolid is used for some lines, so register its
             * restrictions in the operation flags.
             */
	    xf86GCInfoRec.PolyLineSolidZeroWidthFlags |=
	        xf86GCInfoRec.PolyFillRectSolidFlags;
	    xf86GCInfoRec.PolySegmentSolidZeroWidthFlags |=
	        xf86GCInfoRec.PolyFillRectSolidFlags;
            if (xf86AccelInfoRec.SubsequentTwoPointLine)
                xf86AccelInfoRec.Flags |= USE_TWO_POINT_LINE;
            if (!xf86AccelInfoRec.SubsequentBresenhamLine &&
            !(xf86AccelInfoRec.Flags & TWO_POINT_LINE_NOT_LAST))
                /*
                 * If there's only TwoPointLine, and it doesn't support
                 * skipping of the last pixel, then PolySegment cannot
                 * be supported with the CapNotLast line style.
                 */
                xf86GCInfoRec.PolySegmentSolidZeroWidthFlags |=
                    NO_CAP_NOT_LAST;
            if (xf86Verbose)
                ErrorF("%s %s: XAA: General lines and segments\n",
	            XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
        }
        else
        if (SimpleFillRectSolid) {
            /* This one will make use of any available TwoPointLine. */
            if (!xf86GCInfoRec.PolyLineSolidZeroWidth)
                xf86GCInfoRec.PolyLineSolidZeroWidth = xf86PolyLine2;
            if (!xf86GCInfoRec.PolySegmentSolidZeroWidth)
                xf86GCInfoRec.PolySegmentSolidZeroWidth = xf86PolySegment2;
	    xf86GCInfoRec.PolyLineSolidZeroWidthFlags |=
	        xf86GCInfoRec.PolyFillRectSolidFlags;
	    xf86GCInfoRec.PolySegmentSolidZeroWidthFlags |=
	        xf86GCInfoRec.PolyFillRectSolidFlags;
	    if (xf86Verbose) {
                if (xf86AccelInfoRec.SubsequentTwoPointLine)
                    ErrorF("%s %s: XAA: Non-clipped general lines and segments\n",
                        XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
                else
                    ErrorF("%s %s: XAA: Horizontal and vertical lines and segments\n",
                        XCONFIG_PROBED, xf86AccelInfoRec.ServerInfoRec->name);
            }
        }

do_not_touch_xf86AccelInfoRec:

    /*
     * If there is a replacement for screen-to-screen BitBlt, then we
     * want to use it for CopyWindow.
     */
    if (xf86AccelInfoRec.SetupForScreenToScreenCopy &&
    xf86AccelInfoRec.SubsequentScreenToScreenCopy) {
        pScreen->CopyWindow = xf86CopyWindow;
    }
    /*
     * For PaintWindow, just use the mi version, which does everything
     * with GC ops, which will automatically use the most efficient
     * functions.
     */
    pScreen->PaintWindowBackground = miPaintWindow;
    pScreen->PaintWindowBorder = miPaintWindow;
}

/*
 * This function just initializes some fields of the xf86AccelInfoRec.
 */

void xf86InitPixmapCache(infoRec, memoryStart, memoryEnd)
    ScrnInfoPtr infoRec;
    int memoryStart, memoryEnd;
{
    xf86AccelInfoRec.ServerInfoRec = infoRec;
    xf86AccelInfoRec.PixmapCacheMemoryStart = memoryStart;
    xf86AccelInfoRec.PixmapCacheMemoryEnd = memoryEnd;
}

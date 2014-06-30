/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86defs.c,v 3.4 1997/01/20 12:38:19 dawes Exp $ */


#include "windowstr.h"
#include "gcstruct.h"
#include "regionstr.h"

#include "xf86.h"
#include "xf86xaa.h"


xf86GCInfoRecType xf86GCInfoRec = {
    NULL, 0,	/* PolyFillRectSolid() */
    NULL, 0,	/* PolyFillRectTiled() */
    NULL, 0,	/* PolyFillRectStippled() */
    0,		/* SecondaryPolyFillRectStippledFlags */
    NULL, 0,	/* PolyFillRectOpaqueStippled() */
    0,		/* SecondaryPolyFillRectOpaqueStippledFlags */
    NULL, 0,	/* PolyFillArcSolid() */
    NULL, 0,	/* FillPolygonSolid() */
    NULL, 0,	/* PolyRectangleSolidZeroWidth() */
    NULL, 0,	/* PolyLineSolidZeroWidth() */
    NULL, 0,	/* PolySegmentSolidZeroWidth() */
    NULL, 0,	/* PolyGlyphBltNonTE() */
    NULL, 0,	/* PolyGlyphBltTE() */
    NULL, 0,	/* ImageGlyphBltNonTE() */
    NULL, 0,	/* ImageGlyphBltTE() */
    NULL, 0,	/* FillSpansSolid() */
    NULL, 0,	/* FillSpansTiled() */
    NULL, 0,	/* FillSpansStippled() */
    NULL, 0,	/* FillSpansOpaqueStippled() */
    NULL, 0,	/* CopyArea() */
    NULL,	/* FillSpansFallBack */
    NULL,	/* PolyGlyphBltFallBack() */
    NULL,	/* ImageGlyphBltFallBack() */
    NULL,	/* OffScreenCopyWindowFallBack() */
    NULL,	/* CopyAreaFallBack() */
    NULL,	/* cfbBitBltDispatch() */
    NULL,	/* CopyPlane1toNFallBack() */
    NULL,	/* PolyFillRectSolidFallBack() */
};

xf86AccelInfoRecType xf86AccelInfoRec = {
    NULL,	/* FillRectSolid() */
    NULL,	/* FillRectTiled() */
    NULL,	/* FillRectTiledFallBack() */
    NULL,	/* FillRectStippled() */
    NULL,	/* FillRectStippledFallBack() */
    NULL,	/* FillRectOpaqueStippled() */
    NULL,	/* FillRectOpaqueStippledFallBack() */
    NULL,	/* PolyTextTE() */
    NULL,	/* PolyTextNonTE() */
    NULL,	/* ImageTextTE() */
    NULL,	/* ImageTextNonTE() */
    NULL,	/* FillSpansSolid() */
    NULL,	/* ScreenToScreenBitBlt() */
    NULL,	/* WriteBitmap() */
    NULL,	/* WriteBitmapFallBack() */
    NULL,	/* SetupForFillRectSolid() */
    NULL,	/* SubsequentFillRectSolid() */
    NULL,	/* SetupForScreenToScreenCopy() */
    NULL,	/* SubsequentScreenToScreenCopy() */
    NULL,	/* SubsequentBresenhamLine() */
    NULL,	/* SubsequentTwoPointLine() */
    NULL,	/* SetClippingRectangle() */
    NULL,	/* SetupForFill8x8Pattern() */
    NULL,	/* SubsequentFill8x8Pattern() */
    NULL,	/* SetupFor8x8PatternColorExpand() */
    NULL,	/* SetupFor8x8PatternColorExpand() */
    NULL,	/* SetupForCPUToScreenColorExpand() */
    NULL,	/* SubsequentCPUToScreenColorExpand() */
    NULL,	/* SetupForScreenToScreenColorExpand() */
    NULL,	/* SubsequentScreenToScreenColorExpand() */
    NULL,	/* SetupForScanlineCPUToScreenColorExpand() */
    NULL,	/* SubsequentScanlineCPUToScreenColorExpand() */
    NULL,	/* SetupForScanlineScreenToScreenColorExpand() */
    NULL,	/* SubsequentScanlineScreenToScreenColorExpand() */
    NULL,	/* ImageWrite() */
    NULL,	/* ImageWriteFallBack() (could be moved elsewhere) */
    NULL,	/* VerticalLineGXcopyFallBack() */
    NULL,	/* BresenhamLineFallBack() */
    NULL,	/* xf86GetLongWidthAndPointer() */
    NULL,	/* Sync() */
    0,		/* Flags */
    0,		/* ColorExpandFlags */
    NULL,	/* CPUToScreenColorExpandBase */
    NULL,	/* CPUToScreenColorExpandEndMarker */
    0,		/* CPUToScreenColorExpandRange */
    NULL,	/* FramebufferBase */
    0,		/* FramebufferWidth */
    0,		/* BitsPerPixel */
    0xFF,	/* FullPlanemask */
    0,		/* ScratchBufferAddr */
    NULL,	/* ScratchBufferBase */
    0,		/* ScratchBufferSize */
    2,		/* PingPongBuffers */
    0,		/* ErrorTermBits */
    0,		/* UsingVGA256 */
    NULL,	/* ServerInfoRec */
    0,		/* PixmapCacheMemoryStart */
    0,		/* PixmapCacheMemoryEnd */
};

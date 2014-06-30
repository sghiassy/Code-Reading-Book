/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86gcmap.h,v 3.3 1997/01/12 10:48:08 dawes Exp $ */


#ifdef VGA256
#define XF86NAME(x) x##vga256
#else
#if PSZ == 8
#define XF86NAME(x) x##8
#endif
#if PSZ == 16
#define XF86NAME(x) x##16
#endif
#if PSZ == 24
#define XF86NAME(x) x##24
#endif
#if PSZ == 32
#define XF86NAME(x) x##32
#endif
#endif

/* xf86gc.c */
#define xf86ValidateGC XF86NAME(xf86ValidateGC)
#define xf86CreateGC XF86NAME(xf86CreateGC)
/* xf86gcmisc.c */
#define xf86GCNewFillPolygon XF86NAME(xf86GCNewFillPolygon)
#define xf86GCNewRectangle XF86NAME(xf86GCNewRectangle)
#define xf86GCNewLine XF86NAME(xf86GCNewLine)
#define xf86GCNewText XF86NAME(xf86GCNewText)
#define xf86GCNewFillSpans XF86NAME(xf86GCNewFillSpans)
#define xf86GCNewFillArea XF86NAME(xf86GCNewFillArea)
#define xf86GCNewCopyArea XF86NAME(xf86GCNewCopyArea)
#define xf86ImageGlyphBltFallBack XF86NAME(xf86ImageGlyphBltFallBack)
#define xf86PolyGlyphBltFallBack XF86NAME(xf86PolyGlyphBltFallBack)
#define xf86FillSpansFallBack XF86NAME(xf86FillSpansFallBack)
#define xf86FillRectTileFallBack XF86NAME(xf86FillRectTileFallBack)
#define xf86FillRectStippledFallBack XF86NAME(xf86FillRectStippledFallBack)
#define xf86FillRectOpaqueStippledFallBack XF86NAME(xf86FillRectOpaqueStippledFallBack)
/* xf86im.c */
#define xf86ImageWriteFallBack XF86NAME(xf86ImageWriteFallBack)
#define xf86WriteBitmapFallBack XF86NAME(xf86WriteBitmapFallBack)
#define xf86cfbGetLongWidthAndPointer XF86NAME(xf86cfbGetLongWidthAndPointer)
/* xf86cpplane.c */
#ifdef VGA256
#define xf86CopyPlane XF86NAME(xf86CopyPlane)
#define xf86CopyPlane1toN XF86NAME(xf86CopyPlane1toN)
#define xf86PassColorsToCopyPlane1toN XF86NAME(xf86PassColorsToCopyPlane1toN)
#endif

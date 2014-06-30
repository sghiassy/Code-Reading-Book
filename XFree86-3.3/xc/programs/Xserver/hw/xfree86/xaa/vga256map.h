/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/vga256map.h,v 3.0 1996/11/18 13:22:07 dawes Exp $ */


/*
 * This may still be incomplete. If you get a core dump with vga256,
 * chances are it's because of a missing mapping.
 */

#define cfbTEGlyphBlt8 vga256LowlevFuncs.teGlyphBlt8
/* #define cfbImageGlyphBlt8 cfbImageGlyphBlt8 */
#define cfbPolyGlyphBlt8 vga256PolyGlyphBlt8
#define cfbPolyGlyphRop8 vga256PolyGlyphRop8
#define cfbSolidSpansCopy vga256SolidSpansCopy
#define cfbSolidSpansXor vga256SolidSpansXor
#define cfbSolidSpansGeneral vga256SolidSpansGeneral
#define cfbTile32FSCopy vga256Tile32FSCopy
#define cfbTile32FSGeneral vga256Tile32FSGeneral
#define cfbUnnaturalTileFS vga256UnnaturalTileFS
#define cfb8Stipple32FS vga2568Stipple32FS
#define cfbUnnaturalStippleFS vga256UnnaturalStippleFS
#define cfb8OpaqueStipple32FS vga2568OpaqueStipple32FS
#define cfbUnnaturalStippleFS vga256UnnaturalStippleFS
#define cfbPolyFillRect vga256PolyFillRect
#define cfbCopyArea vga256CopyArea
#define cfbFillRectTileOdd vga256FillRectTileOdd
#define cfbFillRectTile32Copy vga256FillRectTile32Copy
#define cfbFillRectTile32General vga256FillRectTile32General
#define cfb8FillRectStippledUnnatural vga2568FillRectStippledUnnatural
#define cfb8FillRectTransparentStippled32 vga2568FillRectTransparentStippled32
#define cfb8FillRectOpaqueStippled32 vga2568FillRectOpaqueStippled32
#define cfbFillPoly1RectCopy vga256FillPoly1RectCopy
#define cfbFillPoly1RectGeneral vga256FillPoly1RectGeneral
#define cfbPolyFillArcSolidCopy vga256PolyFillArcSolidCopy
#define cfbPolyFillArcSolidGeneral vga256PolyFillArcSolidGeneral
#define cfbZeroPolyArcSS8Xor vga256ZeroPolyArcSS8Xor
#define cfbZeroPolyArcSS8Copy vga256ZeroPolyArcSS8Copy
#define cfbZeroPolyArcSS8General vga256ZeroPolyArcSS8General
#define cfb8LineSS1Rect vga256LineSS
#define cfb8SegmentSS1Rect vga256SegmentSS
#define cfbLineSS vga256LineSS
#define cfbSegmentSS vga256SegmentSS
#define cfbLineSD vga256LineSD
#define cfbSegmentSD vga256SegmentSD
#define cfbCopyArea vga256CopyArea
#define cfbPushPixels8 vga256PushPixels8

#define cfbTEOps1Rect vga256TEOps1Rect
#define cfbNonTEOps1Rect vga256NonTEOps1Rect
#define cfbTEOps vga256TEOps
#define cfbNonTEOps vga256NonTEOps
#define cfbSaveAreas vga256SaveAreas
#define cfbRestoreAreas vga256RestoreAreas

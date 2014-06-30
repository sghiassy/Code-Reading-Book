/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/w32map.h,v 3.4 1996/12/23 06:35:26 dawes Exp $ */
/*
 *
Copyright (c) 1991  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.
 *
 * Author:  Keith Packard, MIT X Consortium
 */
/* $XConsortium: w32map.h /main/5 1996/02/21 17:22:32 kaleb $ */

/*
 * Map names around so that multiple depths can be supported simultaneously
 */

/* a losing vendor cpp dumps core if we define NAME in terms of CATNAME */

#if PSZ != 8
#if PSZ == 32
#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)
#define NAME(subname) W3232##subname
#else
#define NAME(subname) W3232/**/subname
#endif
#endif

#if PSZ == 16
#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)
#define NAME(subname) W3216##subname
#else
#define NAME(subname) W3216/**/subname
#endif
#endif

#if PSZ == 4
#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)
#define NAME(subname) W324##subname
#else
#define NAME(subname) W324/**/subname
#endif
#endif

#ifndef NAME
W32 can not hack PSZ yet
#endif

#if (defined(__STDC__) && !defined(UNIXCPP)) || defined(ANSICPP)
#define CATNAME(prefix,subname) prefix##subname
#else
#define CATNAME(prefix,subname) prefix/**/subname
#endif

#define W32ScreenPrivateIndex NAME(ScreenPrivateIndex)
#define QuartetBitsTable NAME(QuartetBitsTable)
#define QuartetPixelMaskTable NAME(QuartetPixelMaskTable)
#define W32AllocatePrivates NAME(AllocatePrivates)
#define W32BSFuncRec NAME(BSFuncRec)
#define W32BitBlt NAME(BitBlt)
#define W32BresD NAME(BresD)
#define W32BresS NAME(BresS)
#define W32ChangeWindowAttributes NAME(ChangeWindowAttributes)
#define W32CloseScreen NAME(CloseScreen)
#define W32CopyArea NAME(CopyArea)
#define W32CopyImagePlane NAME(CopyImagePlane)
#define W32CopyPixmap NAME(CopyPixmap)
#define W32CopyPlane NAME(CopyPlane)
#define W32CopyRotatePixmap NAME(CopyRotatePixmap)
#define W32CopyWindow NAME(CopyWindow)
#define W32CreateGC NAME(CreateGC)
#define W32CreatePixmap NAME(CreatePixmap)
#define W32CreateWindow NAME(CreateWindow)
#define W32CreateScreenResources NAME(CreateScreenResoures)
#define W32DestroyPixmap NAME(DestroyPixmap)
#define W32DestroyWindow NAME(DestroyWindow)
#define W32DoBitblt NAME(DoBitblt)
#define W32DoBitbltCopy NAME(DoBitbltCopy)
#define W32DoBitbltGeneral NAME(DoBitbltGeneral)
#define W32DoBitbltOr NAME(DoBitbltOr)
#define W32DoBitbltXor NAME(DoBitbltXor)
#define W32FillBoxSolid NAME(FillBoxSolid)
#define W32FillBoxTile32 NAME(FillBoxTile32)
#define W32FillBoxTile32sCopy NAME(FillBoxTile32sCopy)
#define W32FillBoxTile32sGeneral NAME(FillBoxTile32sGeneral)
#define W32FillBoxTileOdd NAME(FillBoxTileOdd)
#define W32FillBoxTileOddCopy NAME(FillBoxTileOddCopy)
#define W32FillBoxTileOddGeneral NAME(FillBoxTileOddGeneral)
#define W32FillPoly1RectCopy NAME(FillPoly1RectCopy)
#define W32FillPoly1RectGeneral NAME(FillPoly1RectGeneral)
#define W32FillRectSolidCopy NAME(FillRectSolidCopy)
#define W32FillRectSolidGeneral NAME(FillRectSolidGeneral)
#define W32FillRectSolidXor NAME(FillRectSolidXor)
#define W32FillRectTile32Copy NAME(FillRectTile32Copy)
#define W32FillRectTile32General NAME(FillRectTile32General)
#define W32FillRectTileOdd NAME(FillRectTileOdd)
#define W32FillSpanTile32sCopy NAME(FillSpanTile32sCopy)
#define W32FillSpanTile32sGeneral NAME(FillSpanTile32sGeneral)
#define W32FillSpanTileOddCopy NAME(FillSpanTileOddCopy)
#define W32FillSpanTileOddGeneral NAME(FillSpanTileOddGeneral)
#define W32FinishScreenInit NAME(FinishScreenInit)
#define W32GCFuncs NAME(GCFuncs)
#define W32GetImage NAME(GetImage)
#define W32GetSpans NAME(GetSpans)
#define W32HorzS NAME(HorzS)
#define W32ImageGlyphBlt8 NAME(ImageGlyphBlt8)
#define W32LineSD NAME(LineSD)
#define W32LineSS NAME(LineSS)
#define W32MapWindow NAME(MapWindow)
#define W32MatchCommon NAME(MatchCommon)
#define W32NonTEOps NAME(NonTEOps)
#define W32NonTEOps1Rect NAME(NonTEOps1Rect)
#define W32PadPixmap NAME(PadPixmap)
#define W32PaintWindow NAME(PaintWindow)
#define W32PolyGlyphBlt8 NAME(PolyGlyphBlt8)
#define W32PolyGlyphRop8 NAME(PolyGlyphRop8)
#define W32PolyFillArcSolidCopy NAME(PolyFillArcSolidCopy)
#define W32PolyFillArcSolidGeneral NAME(PolyFillArcSolidGeneral)
#define W32PolyFillRect NAME(PolyFillRect)
#define W32PolyPoint NAME(PolyPoint)
#define W32PositionWindow NAME(PositionWindow)
#define W32PutImage NAME(PutImage)
#define W32ReduceRasterOp NAME(ReduceRasterOp)
#define W32RestoreAreas NAME(RestoreAreas)
#define W32SaveAreas NAME(SaveAreas)
#define W32ScreenInit NAME(ScreenInit)
#define W32SegmentSD NAME(SegmentSD)
#define W32SegmentSS NAME(SegmentSS)
#define W32SetScanline NAME(SetScanline)
#define W32SetSpans NAME(SetSpans)
#define W32SetupScreen NAME(SetupScreen)
#define W32SolidSpansCopy NAME(SolidSpansCopy)
#define W32SolidSpansGeneral NAME(SolidSpansGeneral)
#define W32SolidSpansXor NAME(SolidSpansXor)
#define W32StippleStack NAME(StippleStack)
#define W32StippleStackTE NAME(StippleStackTE)
#define W32TEGlyphBlt NAME(TEGlyphBlt)
#define W32TEOps NAME(TEOps)
#define W32TEOps1Rect NAME(TEOps1Rect)
#define W32Tile32FSCopy NAME(Tile32FSCopy)
#define W32Tile32FSGeneral NAME(Tile32FSGeneral)
#define W32UnmapWindow NAME(UnmapWindow)
#define W32UnnaturalStippleFS NAME(UnnaturalStippleFS)
#define W32UnnaturalTileFS NAME(UnnaturalTileFS)
#define W32ValidateGC NAME(ValidateGC)
#define W32VertS NAME(VertS)
#define W32XRotatePixmap NAME(XRotatePixmap)
#define W32YRotatePixmap NAME(YRotatePixmap)
#define W32endpartial NAME(endpartial)
#define W32endtab NAME(endtab)
#define W32mask NAME(mask)
#define W32rmask NAME(rmask)
#define W32startpartial NAME(startpartial)
#define W32starttab NAME(starttab)
#define W328LineSS1Rect NAME(LineSS1Rect)
#define W328SegmentSS1Rect NAME(SegmentSS1Rect)
#define W328ClippedLineCopy NAME(ClippedLineCopy)
#define W328ClippedLineXor NAME(ClippedLineXor)
#define W328ClippedLineGeneral  NAME(ClippedLineGeneral )
#define W328SegmentSS1RectCopy NAME(SegmentSS1RectCopy)
#define W328SegmentSS1RectXor NAME(SegmentSS1RectXor)
#define W328SegmentSS1RectGeneral  NAME(SegmentSS1RectGeneral )
#define W328SegmentSS1RectShiftCopy NAME(SegmentSS1RectShiftCopy)
#define W328LineSS1RectCopy NAME(LineSS1RectCopy)
#define W328LineSS1RectXor NAME(LineSS1RectXor)
#define W328LineSS1RectGeneral  NAME(LineSS1RectGeneral )
#define W328LineSS1RectPreviousCopy NAME(LineSS1RectPreviousCopy)
#define W32ZeroPolyArcSS8Copy NAME(ZeroPolyArcSSCopy)
#define W32ZeroPolyArcSS8Xor NAME(ZeroPolyArcSSXor)
#define W32ZeroPolyArcSS8General NAME(ZeroPolyArcSSGeneral)

#endif /* PSZ != 8 */

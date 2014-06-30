/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/w32funcs.h,v 3.4 1996/12/23 06:35:24 dawes Exp $ */
/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or X Consortium
not be used in advertising or publicity pertaining to 
distribution  of  the software  without specific prior 
written permission. Sun and X Consortium make no 
representations about the suitability of this software for 
any purpose. It is provided "as is" without any express or 
implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/
/* $XConsortium: w32funcs.h /main/5 1996/10/23 18:41:23 kaleb $ */

#include "w32map.h"

/* W328bit.c */

extern int W32SetStipple(
#if NeedFunctionPrototypes
    int /*alu*/,
    unsigned long /*fg*/,
    unsigned long /*planemask*/
#endif
);

extern int W32SetOpaqueStipple(
#if NeedFunctionPrototypes
    int /*alu*/,
    unsigned long /*fg*/,
    unsigned long /*bg*/,
    unsigned long /*planemask*/
#endif
);

extern int W32ComputeClipMasks32(
#if NeedFunctionPrototypes
    BoxPtr /*pBox*/,
    int /*numRects*/,
    int /*x*/,
    int /*y*/,
    int /*w*/,
    int /*h*/,
    CARD32 * /*clips*/
#endif
);
/* W328cppl.c */

extern void W32CopyImagePlane(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrcDrawable*/,
    DrawablePtr /*pDstDrawable*/,
    int /*rop*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/
#endif
);

extern void W32CopyPlane8to1(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrcDrawable*/,
    DrawablePtr /*pDstDrawable*/,
    int /*rop*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/,
    unsigned long /*bitPlane*/
#endif
);
/* W328lineCO.c */

extern int W328LineSS1RectCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*mode*/,
    int /*npt*/,
    DDXPointPtr /*pptInit*/,
    DDXPointPtr /*pptInitOrig*/,
    int * /*x1p*/,
    int * /*y1p*/,
    int * /*x2p*/,
    int * /*y2p*/
#endif
);

extern void W328LineSS1Rect(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*mode*/,
    int /*npt*/,
    DDXPointPtr /*pptInit*/
#endif
);

extern void W328ClippedLineCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*x1*/,
    int /*y1*/,
    int /*x2*/,
    int /*y2*/,
    BoxPtr /*boxp*/,
    Bool /*shorten*/
#endif
);
/* W328lineCP.c */

extern int W328LineSS1RectPreviousCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*mode*/,
    int /*npt*/,
    DDXPointPtr /*pptInit*/,
    DDXPointPtr /*pptInitOrig*/,
    int * /*x1p*/,
    int * /*y1p*/,
    int * /*x2p*/,
    int * /*y2p*/

#endif
);
/* W328lineG.c */

extern int W328LineSS1RectGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*mode*/,
    int /*npt*/,
    DDXPointPtr /*pptInit*/,
    DDXPointPtr /*pptInitOrig*/,
    int * /*x1p*/,
    int * /*y1p*/,
    int * /*x2p*/,
    int * /*y2p*/
#endif
);

extern void W328ClippedLineGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*x1*/,
    int /*y1*/,
    int /*x2*/,
    int /*y2*/,
    BoxPtr /*boxp*/,
    Bool /*shorten*/
#endif
);
/* W328lineX.c */

extern int W328LineSS1RectXor(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*mode*/,
    int /*npt*/,
    DDXPointPtr /*pptInit*/,
    DDXPointPtr /*pptInitOrig*/,
    int * /*x1p*/,
    int * /*y1p*/,
    int * /*x2p*/,
    int * /*y2p*/
#endif
);

extern void W328ClippedLineXor(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*x1*/,
    int /*y1*/,
    int /*x2*/,
    int /*y2*/,
    BoxPtr /*boxp*/,
    Bool /*shorten*/
#endif
);
/* W328segC.c */

extern int W328SegmentSS1RectCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSegInit*/
#endif
);
/* W328segCS.c */

extern int W328SegmentSS1RectShiftCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSegInit*/
#endif
);

extern void W328SegmentSS1Rect(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSegInit*/
#endif
);
/* W328segG.c */

extern int W328SegmentSS1RectGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSegInit*/
#endif
);
/* W32segX.c */

extern int W328SegmentSS1RectXor(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSegInit*/
#endif
);
/* W32allpriv.c */

extern Bool W32AllocatePrivates(
#if NeedFunctionPrototypes
    ScreenPtr /*pScreen*/,
    int * /*window_index*/,
    int * /*gc_index*/
#endif
);
/* W32bitblt.c */

extern RegionPtr W32BitBlt(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrcDrawable*/,
    DrawablePtr /*pDstDrawable*/,
    GCPtr/*pGC*/,
    int /*srcx*/,
    int /*srcy*/,
    int /*width*/,
    int /*height*/,
    int /*dstx*/,
    int /*dsty*/,
    void (* /*doBitBlt*/)(),
    unsigned long /*bitPlane*/
#endif
);

extern void W32DoBitblt(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrc*/,
    DrawablePtr /*pDst*/,
    int /*alu*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/
#endif
);

extern RegionPtr W32CopyArea(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrcDrawable*/,
    DrawablePtr /*pDstDrawable*/,
    GCPtr/*pGC*/,
    int /*srcx*/,
    int /*srcy*/,
    int /*width*/,
    int /*height*/,
    int /*dstx*/,
    int /*dsty*/
#endif
);

extern void W32CopyPlane1to8(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrcDrawable*/,
    DrawablePtr /*pDstDrawable*/,
    int /*rop*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/,
    unsigned long /*bitPlane*/
#endif
);

extern RegionPtr W32CopyPlane(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrcDrawable*/,
    DrawablePtr /*pDstDrawable*/,
    GCPtr /*pGC*/,
    int /*srcx*/,
    int /*srcy*/,
    int /*width*/,
    int /*height*/,
    int /*dstx*/,
    int /*dsty*/,
    unsigned long /*bitPlane*/
#endif
);
/* W32bltC.c */

extern void W32DoBitbltCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrc*/,
    DrawablePtr /*pDst*/,
    int /*alu*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/
#endif
);
/* W32bltG.c */

extern void W32DoBitbltGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrc*/,
    DrawablePtr /*pDst*/,
    int /*alu*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/
#endif
);
/* W32bltO.c */

extern void W32DoBitbltOr(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrc*/,
    DrawablePtr /*pDst*/,
    int /*alu*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/
#endif
);
/* W32bltX.c */

extern void W32DoBitbltXor(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrc*/,
    DrawablePtr /*pDst*/,
    int /*alu*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/
#endif
);
/* W32bres.c */

extern void W32BresS(
#if NeedFunctionPrototypes
    int /*rop*/,
    unsigned long /*and*/,
    unsigned long /*xor*/,
    unsigned long * /*addrl*/,
    int /*nlwidth*/,
    int /*signdx*/,
    int /*signdy*/,
    int /*axis*/,
    int /*x1*/,
    int /*y1*/,
    int /*e*/,
    int /*e1*/,
    int /*e2*/,
    int /*len*/
#endif
);
/* W32bresd.c */

extern void W32BresD(
#if NeedFunctionPrototypes
    cfbRRopPtr /*rrops*/,
    int * /*pdashIndex*/,
    unsigned char * /*pDash*/,
    int /*numInDashList*/,
    int * /*pdashOffset*/,
    int /*isDoubleDash*/,
    unsigned long * /*addrl*/,
    int /*nlwidth*/,
    int /*signdx*/,
    int /*signdy*/,
    int /*axis*/,
    int /*x1*/,
    int /*y1*/,
    int /*e*/,
    int /*e1*/,
    int /*e2*/,
    int /*len*/
#endif
);
/* W32bstore.c */

extern void W32SaveAreas(
#if NeedFunctionPrototypes
    PixmapPtr /*pPixmap*/,
    RegionPtr /*prgnSave*/,
    int /*xorg*/,
    int /*yorg*/,
    WindowPtr /*pWin*/
#endif
);

extern void W32RestoreAreas(
#if NeedFunctionPrototypes
    PixmapPtr /*pPixmap*/,
    RegionPtr /*prgnRestore*/,
    int /*xorg*/,
    int /*yorg*/,
    WindowPtr /*pWin*/
#endif
);
/* W32cmap.c */

extern int W32ListInstalledColormaps(
#if NeedFunctionPrototypes
    ScreenPtr	/*pScreen*/,
    Colormap	* /*pmaps*/
#endif
);

extern void W32InstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr	/*pmap*/
#endif
);

extern void W32UninstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr	/*pmap*/
#endif
);

extern void W32ResolveColor(
#if NeedFunctionPrototypes
    unsigned short * /*pred*/,
    unsigned short * /*pgreen*/,
    unsigned short * /*pblue*/,
    VisualPtr /*pVisual*/
#endif
);

extern Bool W32InitializeColormap(
#if NeedFunctionPrototypes
    ColormapPtr /*pmap*/
#endif
);

extern int W32ExpandDirectColors(
#if NeedFunctionPrototypes
    ColormapPtr /*pmap*/,
    int /*ndef*/,
    xColorItem * /*indefs*/,
    xColorItem * /*outdefs*/
#endif
);

extern Bool W32CreateDefColormap(
#if NeedFunctionPrototypes
    ScreenPtr /*pScreen*/
#endif
);

extern Bool W32SetVisualTypes(
#if NeedFunctionPrototypes
    int /*depth*/,
    int /*visuals*/,
    int /*bitsPerRGB*/
#endif
);

extern Bool W32InitVisuals(
#if NeedFunctionPrototypes
    VisualPtr * /*visualp*/,
    DepthPtr * /*depthp*/,
    int * /*nvisualp*/,
    int * /*ndepthp*/,
    int * /*rootDepthp*/,
    VisualID * /*defaultVisp*/,
    unsigned long /*sizes*/,
    int /*bitsPerRGB*/
#endif
);
/* W32fillarcC.c */

extern void W32PolyFillArcSolid(
#if NeedFunctionPrototypes
    DrawablePtr /*pDraw*/,
    GCPtr /*pGC*/,
    int /*narcs*/,
    xArc * /*parcs*/
#endif
);

/* W32fillrct.c */

extern void W32FillBoxTileOdd(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*n*/,
    BoxPtr /*rects*/,
    PixmapPtr /*tile*/,
    int /*xrot*/,
    int /*yrot*/
#endif
);

extern void W32FillRectTileOdd(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void W32PolyFillRect(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nrectFill*/,
    xRectangle * /*prectInit*/
#endif
);
/* W32fillsp.c */

extern void W32UnnaturalTileFS(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr/*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);

extern void W32UnnaturalStippleFS(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr/*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);

extern void W328Stipple32FS(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);

extern void W328OpaqueStipple32FS(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);
/* W32gc.c */

extern GCOpsPtr W32MatchCommon(
#if NeedFunctionPrototypes
    GCPtr /*pGC*/,
    cfbPrivGCPtr /*devPriv*/
#endif
);

extern Bool W32CreateGC(
#if NeedFunctionPrototypes
    GCPtr /*pGC*/
#endif
);

extern void W32ValidateGC(
#if NeedFunctionPrototypes
    GCPtr /*pGC*/,
    unsigned long /*changes*/,
    DrawablePtr /*pDrawable*/
#endif
);

/* W32getsp.c */

extern void W32GetSpans(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*wMax*/,
    DDXPointPtr /*ppt*/,
    int * /*pwidth*/,
    int /*nspans*/,
    char * /*pdstStart*/
#endif
);
/* W32glblt8.c */

extern void W32PolyGlyphBlt8(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*x*/,
    int /*y*/,
    unsigned int /*nglyph*/,
    CharInfoPtr * /*ppci*/,
    pointer /*pglyphBase*/
#endif
);
/* W32glrop8.c */

extern void W32PolyGlyphRop8(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*x*/,
    int /*y*/,
    unsigned int /*nglyph*/,
    CharInfoPtr * /*ppci*/,
    pointer /*pglyphBase*/
#endif
);
/* W32hrzvert.c */

extern int W32HorzS(
#if NeedFunctionPrototypes
    int /*rop*/,
    unsigned long /*and*/,
    unsigned long /*xor*/,
    unsigned long * /*addrl*/,
    int /*nlwidth*/,
    int /*x1*/,
    int /*y1*/,
    int /*len*/
#endif
);

extern int W32VertS(
#if NeedFunctionPrototypes
    int /*rop*/,
    unsigned long /*and*/,
    unsigned long /*xor*/,
    unsigned long * /*addrl*/,
    int /*nlwidth*/,
    int /*x1*/,
    int /*y1*/,
    int /*len*/
#endif
);
/* W32igblt8.c */

extern void W32ImageGlyphBlt8(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*x*/,
    int /*y*/,
    unsigned int /*nglyph*/,
    CharInfoPtr * /*ppci*/,
    pointer /*pglyphBase*/
#endif
);
/* W32image.c */

extern void W32PutImage(
#if NeedFunctionPrototypes
    DrawablePtr /*pDraw*/,
    GCPtr /*pGC*/,
    int /*depth*/,
    int /*x*/,
    int /*y*/,
    int /*w*/,
    int /*h*/,
    int /*leftPad*/,
    int /*format*/,
    char * /*pImage*/
#endif
);

extern void W32GetImage(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*sx*/,
    int /*sy*/,
    int /*w*/,
    int /*h*/,
    unsigned int /*format*/,
    unsigned long /*planeMask*/,
    char * /*pdstLine*/
#endif
);
/* W32line.c */

extern void W32LineSS(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*mode*/,
    int /*npt*/,
    DDXPointPtr /*pptInit*/
#endif
);

extern void W32LineSD(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*mode*/,
    int /*npt*/,
    DDXPointPtr /*pptInit*/
#endif
);
/* W32mskbits.c */
/* W32pixmap.c */

extern PixmapPtr W32CreatePixmap(
#if NeedFunctionPrototypes
    ScreenPtr /*pScreen*/,
    int /*width*/,
    int /*height*/,
    int /*depth*/
#endif
);

extern Bool W32DestroyPixmap(
#if NeedFunctionPrototypes
    PixmapPtr /*pPixmap*/
#endif
);

extern PixmapPtr W32CopyPixmap(
#if NeedFunctionPrototypes
    PixmapPtr /*pSrc*/
#endif
);

extern void W32PadPixmap(
#if NeedFunctionPrototypes
    PixmapPtr /*pPixmap*/
#endif
);

extern void W32XRotatePixmap(
#if NeedFunctionPrototypes
    PixmapPtr /*pPix*/,
    int /*rw*/
#endif
);

extern void W32YRotatePixmap(
#if NeedFunctionPrototypes
    PixmapPtr /*pPix*/,
    int /*rh*/
#endif
);

extern void W32CopyRotatePixmap(
#if NeedFunctionPrototypes
    PixmapPtr /*psrcPix*/,
    PixmapPtr * /*ppdstPix*/,
    int /*xrot*/,
    int /*yrot*/
#endif
);
/* W32ply1rctC.c */

extern void W32FillPoly1RectCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*shape*/,
    int /*mode*/,
    int /*count*/,
    DDXPointPtr /*ptsIn*/
#endif
);
/* W32ply1rctG.c */

extern void W32FillPoly1RectGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*shape*/,
    int /*mode*/,
    int /*count*/,
    DDXPointPtr /*ptsIn*/
#endif
);
/* W32pntwin.c */

extern void W32PaintWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWin*/,
    RegionPtr /*pRegion*/,
    int /*what*/
#endif
);

extern void W32FillBoxSolid(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*nBox*/,
    BoxPtr /*pBox*/,
    unsigned long /*pixel*/
#endif
);

extern void W32FillBoxTile32(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*nBox*/,
    BoxPtr /*pBox*/,
    PixmapPtr /*tile*/
#endif
);
/* W32polypnt.c */

extern void W32PolyPoint(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*mode*/,
    int /*npt*/,
    xPoint * /*pptInit*/
#endif
);
/* W32push8.c */

extern void W32PushPixels8(
#if NeedFunctionPrototypes
    GCPtr /*pGC*/,
    PixmapPtr /*pBitmap*/,
    DrawablePtr /*pDrawable*/,
    int /*dx*/,
    int /*dy*/,
    int /*xOrg*/,
    int /*yOrg*/
#endif
);
/* W32rctstp8.c */

extern void W328FillRectOpaqueStippled32(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void W328FillRectTransparentStippled32(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void W328FillRectStippledUnnatural(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);
/* W32rrop.c */

extern int W32ReduceRasterOp(
#if NeedFunctionPrototypes
    int /*rop*/,
    unsigned long /*fg*/,
    unsigned long /*pm*/,
    unsigned long * /*andp*/,
    unsigned long * /*xorp*/
#endif
);
/* W32scrinit.c */

extern Bool W32CloseScreen(
#if NeedFunctionPrototypes
    int /*index*/,
    ScreenPtr /*pScreen*/
#endif
);

extern Bool W32SetupScreen(
#if NeedFunctionPrototypes
    ScreenPtr /*pScreen*/,
    pointer /*pbits*/,
    int /*xsize*/,
    int /*ysize*/,
    int /*dpix*/,
    int /*dpiy*/,
    int /*width*/
#endif
);

extern int W32FinishScreenInit(
#if NeedFunctionPrototypes
    ScreenPtr /*pScreen*/,
    pointer /*pbits*/,
    int /*xsize*/,
    int /*ysize*/,
    int /*dpix*/,
    int /*dpiy*/,
    int /*width*/
#endif
);

extern Bool W32ScreenInit(
#if NeedFunctionPrototypes
    ScreenPtr /*pScreen*/,
    pointer /*pbits*/,
    int /*xsize*/,
    int /*ysize*/,
    int /*dpix*/,
    int /*dpiy*/,
    int /*width*/
#endif
);
/* W32seg.c */

extern void W32SegmentSS(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSeg*/
#endif
);

extern void W32SegmentSD(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSeg*/
#endif
);
/* W32setsp.c */

extern int W32SetScanline(
#if NeedFunctionPrototypes
    int /*y*/,
    int /*xOrigin*/,
    int /*xStart*/,
    int /*xEnd*/,
    unsigned int * /*psrc*/,
    int /*alu*/,
    int * /*pdstBase*/,
    int /*widthDst*/,
    unsigned long /*planemask*/
#endif
);

extern void W32SetSpans(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    char * /*psrc*/,
    DDXPointPtr /*ppt*/,
    int * /*pwidth*/,
    int /*nspans*/,
    int /*fSorted*/
#endif
);
/* W32solidC.c */

extern void W32FillRectSolidCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void W32SolidSpansCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);
/* W32solidG.c */

extern void W32FillRectSolidGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void W32SolidSpansGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);
/* W32solidX.c */

extern void W32FillRectSolidXor(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void W32SolidSpansXor(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);
/* W32teblt8.c */

extern void W32TEGlyphBlt8(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr/*pGC*/,
    int /*xInit*/,
    int /*yInit*/,
    unsigned int /*nglyph*/,
    CharInfoPtr * /*ppci*/,
    pointer /*pglyphBase*/
#endif
);
/* W32tegblt.c */

extern void W32TEGlyphBlt(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr/*pGC*/,
    int /*x*/,
    int /*y*/,
    unsigned int /*nglyph*/,
    CharInfoPtr * /*ppci*/,
    pointer /*pglyphBase*/
#endif
);
/* W32tile32C.c */

extern void W32FillRectTile32Copy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void W32Tile32FSCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);
/* W32tile32G.c */

extern void W32FillRectTile32General(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void W32Tile32FSGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);
/* W32tileoddC.c */

extern void W32FillBoxTileOddCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*nBox*/,
    BoxPtr /*pBox*/,
    PixmapPtr /*tile*/,
    int /*xrot*/,
    int /*yrot*/,
    int /*alu*/,
    unsigned long /*planemask*/
#endif
);

extern void W32FillSpanTileOddCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*n*/,
    DDXPointPtr /*ppt*/,
    int * /*pwidth*/,
    PixmapPtr /*tile*/,
    int /*xrot*/,
    int /*yrot*/,
    int /*alu*/,
    unsigned long /*planemask*/
#endif
);

extern void W32FillBoxTile32sCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*nBox*/,
    BoxPtr /*pBox*/,
    PixmapPtr /*tile*/,
    int /*xrot*/,
    int /*yrot*/,
    int /*alu*/,
    unsigned long /*planemask*/
#endif
);

extern void W32FillSpanTile32sCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*n*/,
    DDXPointPtr /*ppt*/,
    int * /*pwidth*/,
    PixmapPtr /*tile*/,
    int /*xrot*/,
    int /*yrot*/,
    int /*alu*/,
    unsigned long /*planemask*/
#endif
);
/* W32tileoddG.c */

extern void W32FillBoxTileOddGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*nBox*/,
    BoxPtr /*pBox*/,
    PixmapPtr /*tile*/,
    int /*xrot*/,
    int /*yrot*/,
    int /*alu*/,
    unsigned long /*planemask*/
#endif
);

extern void W32FillSpanTileOddGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*n*/,
    DDXPointPtr /*ppt*/,
    int * /*pwidth*/,
    PixmapPtr /*tile*/,
    int /*xrot*/,
    int /*yrot*/,
    int /*alu*/,
    unsigned long /*planemask*/
#endif
);

extern void W32FillBoxTile32sGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*nBox*/,
    BoxPtr /*pBox*/,
    PixmapPtr /*tile*/,
    int /*xrot*/,
    int /*yrot*/,
    int /*alu*/,
    unsigned long /*planemask*/
#endif
);

extern void W32FillSpanTile32sGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*n*/,
    DDXPointPtr /*ppt*/,
    int * /*pwidth*/,
    PixmapPtr /*tile*/,
    int /*xrot*/,
    int /*yrot*/,
    int /*alu*/,
    unsigned long /*planemask*/
#endif
);
/* W32window.c */

extern Bool W32CreateWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWin*/
#endif
);

extern Bool W32DestroyWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWin*/
#endif
);

extern Bool W32MapWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWindow*/
#endif
);

extern Bool W32PositionWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWin*/,
    int /*x*/,
    int /*y*/
#endif
);

extern Bool W32UnmapWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWindow*/
#endif
);

extern void W32CopyWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWin*/,
    DDXPointRec /*ptOldOrg*/,
    RegionPtr /*prgnSrc*/
#endif
);

extern Bool W32ChangeWindowAttributes(
#if NeedFunctionPrototypes
    WindowPtr /*pWin*/,
    unsigned long /*mask*/
#endif
);
/* W32zerarcC.c */

extern void W32ZeroPolyArcSS8Copy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDraw*/,
    GCPtr /*pGC*/,
    int /*narcs*/,
    xArc * /*parcs*/
#endif
);
/* W32zerarcG.c */

extern void W32ZeroPolyArcSS8General(
#if NeedFunctionPrototypes
    DrawablePtr /*pDraw*/,
    GCPtr /*pGC*/,
    int /*narcs*/,
    xArc * /*parcs*/
#endif
);
/* W32zerarcX.c */

extern void W32ZeroPolyArcSS8Xor(
#if NeedFunctionPrototypes
    DrawablePtr /*pDraw*/,
    GCPtr /*pGC*/,
    int /*narcs*/,
    xArc * /*parcs*/
#endif
);

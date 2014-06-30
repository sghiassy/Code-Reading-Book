/* $XConsortium: cfb24.h /main/1 1996/09/21 12:55:08 kaleb $ */




/* $XFree86: xc/programs/Xserver/hw/xfree86/common/cfb24.h,v 3.3 1997/01/08 20:34:26 dawes Exp $ */
/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or MIT not be used in
advertising or publicity pertaining to distribution  of  the
software  without specific prior written permission. Sun and
M.I.T. make no representations about the suitability of this
software for any purpose. It is provided "as is" without any
express or implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/

/* cfb8bit.c */

extern void cfb24CopyImagePlane(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrcDrawable*/,
    DrawablePtr /*pDstDrawable*/,
    int /*rop*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/
#endif
);

/* cfb8lineCO.c */

extern int cfb24LineSS1RectCopy(
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

extern void cfb24LineSS1Rect(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*mode*/,
    int /*npt*/,
    DDXPointPtr /*pptInit*/
#endif
);

extern void cfb24ClippedLineCopy(
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
/* cfb8lineCP.c */

extern int cfb24LineSS1RectPreviousCopy(
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
/* cfb8lineG.c */

extern int cfb24LineSS1RectGeneral(
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

extern void cfb24ClippedLineGeneral(
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
/* cfb8lineX.c */

extern int cfb24LineSS1RectXor(
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

extern void cfb24ClippedLineXor(
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
/* cfb8segC.c */

extern int cfb24SegmentSS1RectCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSegInit*/
#endif
);
/* cfb8segCS.c */

extern int cfb24SegmentSS1RectShiftCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSegInit*/
#endif
);

extern void cfb24SegmentSS1Rect(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSegInit*/
#endif
);
/* cfb8segG.c */

extern int cfb24SegmentSS1RectGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSegInit*/
#endif
);
/* cfbsegX.c */

extern int cfb24SegmentSS1RectXor(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSegInit*/
#endif
);
/* cfballpriv.c */

extern Bool cfb24AllocatePrivates(
#if NeedFunctionPrototypes
    ScreenPtr /*pScreen*/,
    int * /*window_index*/,
    int * /*gc_index*/
#endif
);
/* cfbbitblt.c */

extern RegionPtr cfb24BitBlt(
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
    void (* /*doBitBlt*/)(
#if NeedNestedPrototypes
	DrawablePtr /*pSrc*/,
	DrawablePtr /*pDst*/,
	int /*alu*/,
	RegionPtr /*prgnDst*/,
	DDXPointPtr /*pptSrc*/,
	unsigned long /*planemask*/
#endif
	),
    unsigned long /*bitPlane*/
#endif
);

extern void cfb24DoBitblt(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrc*/,
    DrawablePtr /*pDst*/,
    int /*alu*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/
#endif
);

extern RegionPtr cfb24CopyArea(
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

extern RegionPtr cfb24CopyPlane(
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
/* cfbbltC.c */

extern void cfb24DoBitbltCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrc*/,
    DrawablePtr /*pDst*/,
    int /*alu*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/
#endif
);
/* cfbbltG.c */

extern void cfb24DoBitbltGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrc*/,
    DrawablePtr /*pDst*/,
    int /*alu*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/
#endif
);
/* cfbbltO.c */

extern void cfb24DoBitbltOr(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrc*/,
    DrawablePtr /*pDst*/,
    int /*alu*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/
#endif
);
/* cfbbltX.c */

extern void cfb24DoBitbltXor(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrc*/,
    DrawablePtr /*pDst*/,
    int /*alu*/,
    RegionPtr /*prgnDst*/,
    DDXPointPtr /*pptSrc*/,
    unsigned long /*planemask*/
#endif
);
/* cfbbres.c */

extern void cfb24BresS(
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
/* cfbbresd.c */

extern void cfb24BresD(
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
/* cfbbstore.c */

extern void cfb24SaveAreas(
#if NeedFunctionPrototypes
    PixmapPtr /*pPixmap*/,
    RegionPtr /*prgnSave*/,
    int /*xorg*/,
    int /*yorg*/,
    WindowPtr /*pWin*/
#endif
);

extern void cfb24RestoreAreas(
#if NeedFunctionPrototypes
    PixmapPtr /*pPixmap*/,
    RegionPtr /*prgnRestore*/,
    int /*xorg*/,
    int /*yorg*/,
    WindowPtr /*pWin*/
#endif
);
/* cfbfillarcC.c */

extern void cfb24PolyFillArcSolidCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDraw*/,
    GCPtr /*pGC*/,
    int /*narcs*/,
    xArc * /*parcs*/
#endif
);
/* cfbfillarcG.c */

extern void cfb24PolyFillArcSolidGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDraw*/,
    GCPtr /*pGC*/,
    int /*narcs*/,
    xArc * /*parcs*/
#endif
);
/* cfbfillrct.c */

extern void cfb24FillBoxTileOdd(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*n*/,
    BoxPtr /*rects*/,
    PixmapPtr /*tile*/,
    int /*xrot*/,
    int /*yrot*/
#endif
);

extern void cfb24FillRectTileOdd(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void cfb24PolyFillRect(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nrectFill*/,
    xRectangle * /*prectInit*/
#endif
);
/* cfbfillsp.c */

extern void cfb24UnnaturalTileFS(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr/*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);

extern void cfb24UnnaturalStippleFS(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr/*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);
/* cfbgc.c */

extern GCOpsPtr cfb24MatchCommon(
#if NeedFunctionPrototypes
    GCPtr /*pGC*/,
    cfbPrivGCPtr /*devPriv*/
#endif
);

extern Bool cfb24CreateGC(
#if NeedFunctionPrototypes
    GCPtr /*pGC*/
#endif
);

extern void cfb24ValidateGC(
#if NeedFunctionPrototypes
    GCPtr /*pGC*/,
    unsigned long /*changes*/,
    DrawablePtr /*pDrawable*/
#endif
);

/* cfbgetsp.c */

extern void cfb24GetSpans(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*wMax*/,
    DDXPointPtr /*ppt*/,
    int * /*pwidth*/,
    int /*nspans*/,
    char * /*pdstStart*/
#endif
);
/* cfbglblt8.c */

extern void cfb24PolyGlyphBlt8(
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
/* cfbglrop8.c */

extern void cfb24PolyGlyphRop8(
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
/* cfbhrzvert.c */

extern int cfb24HorzS(
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

extern void cfb24VertS(
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
/* cfbigblt8.c */

extern void cfb24ImageGlyphBlt8(
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
/* cfbimage.c */

extern void cfb24PutImage(
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

extern void cfb24GetImage(
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
/* cfbline.c */

extern void cfb24LineSS(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*mode*/,
    int /*npt*/,
    DDXPointPtr /*pptInit*/
#endif
);

extern void cfb24LineSD(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*mode*/,
    int /*npt*/,
    DDXPointPtr /*pptInit*/
#endif
);
/* cfbmskbits.c */
/* cfbpixmap.c */

extern PixmapPtr cfb24CreatePixmap(
#if NeedFunctionPrototypes
    ScreenPtr /*pScreen*/,
    int /*width*/,
    int /*height*/,
    int /*depth*/
#endif
);

extern Bool cfb24DestroyPixmap(
#if NeedFunctionPrototypes
    PixmapPtr /*pPixmap*/
#endif
);

extern PixmapPtr cfb24CopyPixmap(
#if NeedFunctionPrototypes
    PixmapPtr /*pSrc*/
#endif
);

extern void cfb24PadPixmap(
#if NeedFunctionPrototypes
    PixmapPtr /*pPixmap*/
#endif
);

extern void cfb24XRotatePixmap(
#if NeedFunctionPrototypes
    PixmapPtr /*pPix*/,
    int /*rw*/
#endif
);

extern void cfb24YRotatePixmap(
#if NeedFunctionPrototypes
    PixmapPtr /*pPix*/,
    int /*rh*/
#endif
);

extern void cfb24CopyRotatePixmap(
#if NeedFunctionPrototypes
    PixmapPtr /*psrcPix*/,
    PixmapPtr * /*ppdstPix*/,
    int /*xrot*/,
    int /*yrot*/
#endif
);
/* cfbply1rctC.c */

extern void cfb24FillPoly1RectCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*shape*/,
    int /*mode*/,
    int /*count*/,
    DDXPointPtr /*ptsIn*/
#endif
);
/* cfbply1rctG.c */

extern void cfb24FillPoly1RectGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*shape*/,
    int /*mode*/,
    int /*count*/,
    DDXPointPtr /*ptsIn*/
#endif
);
/* cfbpntwin.c */

extern void cfb24PaintWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWin*/,
    RegionPtr /*pRegion*/,
    int /*what*/
#endif
);

extern void cfb24FillBoxSolid(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*nBox*/,
    BoxPtr /*pBox*/,
    unsigned long /*pixel*/
#endif
);

extern void cfb24FillBoxTile32(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    int /*nBox*/,
    BoxPtr /*pBox*/,
    PixmapPtr /*tile*/
#endif
);
/* cfbpolypnt.c */

extern void cfb24PolyPoint(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*mode*/,
    int /*npt*/,
    xPoint * /*pptInit*/
#endif
);
/* cfbrrop.c */

extern int cfb24ReduceRasterOp(
#if NeedFunctionPrototypes
    int /*rop*/,
    unsigned long /*fg*/,
    unsigned long /*pm*/,
    unsigned long * /*andp*/,
    unsigned long * /*xorp*/
#endif
);
/* cfbscrinit.c */

extern Bool cfb24CloseScreen(
#if NeedFunctionPrototypes
    int /*index*/,
    ScreenPtr /*pScreen*/
#endif
);

extern Bool cfb24SetupScreen(
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

extern int cfb24FinishScreenInit(
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

extern Bool cfb24ScreenInit(
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

extern Bool cfb24CreateScreenResources(
#if NeedFunctionPrototypes
    ScreenPtr /*pScreen*/
#endif
);

/* cfbseg.c */

extern void cfb24SegmentSS(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSeg*/
#endif
);

extern void cfb24SegmentSD(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nseg*/,
    xSegment * /*pSeg*/
#endif
);
/* cfbsetsp.c */

extern int cfb24SetScanline(
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

extern void cfb24SetSpans(
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
/* cfbsolidC.c */

extern void cfb24FillRectSolidCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void cfb24SolidSpansCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);
/* cfbsolidG.c */

extern void cfb24FillRectSolidGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void cfb24SolidSpansGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);
/* cfbsolidX.c */

extern void cfb24FillRectSolidXor(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void cfb24SolidSpansXor(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);
/* cfbtegblt.c */

extern void cfb24TEGlyphBlt(
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
/* cfbtile32C.c */

extern void cfb24FillRectTile32Copy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void cfb24Tile32FSCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);
/* cfbtile32G.c */

extern void cfb24FillRectTile32General(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nBox*/,
    BoxPtr /*pBox*/
#endif
);

extern void cfb24Tile32FSGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDrawable*/,
    GCPtr /*pGC*/,
    int /*nInit*/,
    DDXPointPtr /*pptInit*/,
    int * /*pwidthInit*/,
    int /*fSorted*/
#endif
);
/* cfbtileoddC.c */

extern void cfb24FillBoxTileOddCopy(
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

extern void cfb24FillSpanTileOddCopy(
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

extern void cfb24FillBoxTile32sCopy(
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

extern void cfb24FillSpanTile32sCopy(
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
/* cfbtileoddG.c */

extern void cfb24FillBoxTileOddGeneral(
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

extern void cfb24FillSpanTileOddGeneral(
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

extern void cfb24FillBoxTile32sGeneral(
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

extern void cfb24FillSpanTile32sGeneral(
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
/* cfbwindow.c */

extern Bool cfb24CreateWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWin*/
#endif
);

extern Bool cfb24DestroyWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWin*/
#endif
);

extern Bool cfb24MapWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWindow*/
#endif
);

extern Bool cfb24PositionWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWin*/,
    int /*x*/,
    int /*y*/
#endif
);

extern Bool cfb24UnmapWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWindow*/
#endif
);

extern void cfb24CopyWindow(
#if NeedFunctionPrototypes
    WindowPtr /*pWin*/,
    DDXPointRec /*ptOldOrg*/,
    RegionPtr /*prgnSrc*/
#endif
);

extern Bool cfb24ChangeWindowAttributes(
#if NeedFunctionPrototypes
    WindowPtr /*pWin*/,
    unsigned long /*mask*/
#endif
);
/* cfbzerarcC.c */

extern void cfb24ZeroPolyArcSSCopy(
#if NeedFunctionPrototypes
    DrawablePtr /*pDraw*/,
    GCPtr /*pGC*/,
    int /*narcs*/,
    xArc * /*parcs*/
#endif
);
/* cfbzerarcG.c */

extern void cfb24ZeroPolyArcSSGeneral(
#if NeedFunctionPrototypes
    DrawablePtr /*pDraw*/,
    GCPtr /*pGC*/,
    int /*narcs*/,
    xArc * /*parcs*/
#endif
);
/* cfbzerarcX.c */

extern void cfb24ZeroPolyArcSSXor(
#if NeedFunctionPrototypes
    DrawablePtr /*pDraw*/,
    GCPtr /*pGC*/,
    int /*narcs*/,
    xArc * /*parcs*/
#endif
);

extern int cfb24ScreenPrivateIndex;






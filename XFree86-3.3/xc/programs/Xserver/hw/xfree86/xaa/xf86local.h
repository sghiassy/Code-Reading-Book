/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86local.h,v 3.6 1997/01/23 11:04:24 dawes Exp $ */


/* Functions that are only referenced from within this directory. */

/* These prototypes do not depend on any depth-specific mapping. */

void 
xf86CopyWindow(
#if NeedFunctionPrototypes
    WindowPtr	pWin,
    DDXPointRec	ptOldOrg,
    RegionPtr	prgnSrc
#endif
);

void
xf86DoBitBlt(
#if NeedFunctionPrototypes
    DrawablePtr		pSrc,
    DrawablePtr		pDst,
    int			alu,
    RegionPtr		prgnDst,
    DDXPointPtr		pptSrc,
    unsigned int 	planemask,
    int			bitplane
#endif
);

void
xf86miFillRectStippledFallBack(
#if NeedFunctionPrototypes
    DrawablePtr	    pDrawable,
    GCPtr	    pGC,
    int		    nBox,
    BoxPtr	    pBox
#endif
);

RegionPtr
xf86CopyPlane(
#if NeedFunctionPrototypes
    DrawablePtr		pSrcDrawable,
    DrawablePtr		pDstDrawable,
    GCPtr		pGC,
    int			srcx,
    int			srcy,
    int			width,
    int			height,
    int			dstx,
    int			dsty,
    unsigned long	bitPlane
#endif
);

RegionPtr
xf86CopyPlanevga256(
#if NeedFunctionPrototypes
    DrawablePtr		pSrcDrawable,
    DrawablePtr		pDstDrawable,
    GCPtr		pGC,
    int			srcx,
    int			srcy,
    int			width,
    int			height,
    int			dstx,
    int			dsty,
    unsigned long	bitPlane
#endif
);

void xf86CopyPlane1toN(
#if NeedFunctionPrototypes
    DrawablePtr		pSrcDrawable,
    DrawablePtr		pDstDrawable,
    int			alu,
    RegionPtr		rgnDst,
    DDXPointPtr		pptSrc,
    unsigned int	planemask,
    unsigned long	bitPlane,
    int			bg,
    int			fg
#endif
);

void xf86CopyPlane1toNvga256(
#if NeedFunctionPrototypes
    DrawablePtr		pSrcDrawable,
    DrawablePtr		pDstDrawable,
    int			alu,
    RegionPtr		rgnDst,
    DDXPointPtr		pptSrc,
    unsigned int	planemask,
    unsigned long	bitPlane,
    int			bg,
    int			fg
#endif
);

void
cfbCopyPlane1to16(
#if NeedFunctionPrototypes
    DrawablePtr pSrcDrawable,
    DrawablePtr pDstDrawable,
    int	rop,
    RegionPtr prgnDst,
    DDXPointPtr pptSrc,
    unsigned long planemask,
    int bitPlane,
    int bg,
    int fg
#endif
);

void
cfbCopyPlane1to32(
#if NeedFunctionPrototypes
    DrawablePtr pSrcDrawable,
    DrawablePtr pDstDrawable,
    int	rop,
    RegionPtr prgnDst,
    DDXPointPtr pptSrc,
    unsigned long planemask,
    int bitPlane,
    int bg,
    int fg
#endif
);

void
cfbCopyPlane16to1(
#if NeedFunctionPrototypes
    DrawablePtr pSrcDrawable,
    DrawablePtr pDstDrawable,
    int	rop,
    RegionPtr prgnDst,
    DDXPointPtr pptSrc,
    unsigned long planemask,
    int bitPlane
#endif
);

void
cfbCopyPlane32to1(
#if NeedFunctionPrototypes
    DrawablePtr pSrcDrawable,
    DrawablePtr pDstDrawable,
    int	rop,
    RegionPtr prgnDst,
    DDXPointPtr pptSrc,
    unsigned long planemask,
    int bitPlane
#endif
);

void
xf86PolyRectangleSolidZeroWidth(
#if NeedFunctionPrototypes
    DrawablePtr  pDrawable,
    GCPtr        pGC,
    int	         nRectsInit,
    xRectangle  *pRectsInit
#endif
);

/* More generic acceleration functions. */

void
xf86FillSpans(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr	pGC,
    int		nInit,
    DDXPointPtr pptInit,
    int		*pwidthInit,
    int		fSorted
#endif
);

void
xf86FillSpansAsRects(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr	pGC,
    int		nInit,
    DDXPointPtr pptInit,
    int		*pwidthInit,
    int		fSorted
#endif
);

void
xf86FillSpansTiled(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr	pGC,
    int		nInit,
    DDXPointPtr pptInit,
    int		*pwidthInit,
    int		fSorted
#endif
);

void
xf86FillSpansStippled(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr	pGC,
    int		nInit,
    DDXPointPtr pptInit,
    int		*pwidthInit,
    int		fSorted
#endif
);

void
xf86PolyFillArcSolid(
#if NeedFunctionPrototypes
    DrawablePtr	pDraw,
    GCPtr	pGC,
    int		narcs,
    xArc	*parcs
#endif
);

void
xf86FillPolygonSolid1Rect(
#if NeedFunctionPrototypes
    DrawablePtr	pDrawable,
    GCPtr	pGC,
    int		shape,
    int		mode,
    int		count,
    DDXPointPtr	ptsIn
#endif
);

RegionPtr
xf86CopyArea(
#if NeedFunctionPrototypes
    DrawablePtr pSrcDrawable,
    DrawablePtr pDstDrawable,
    GCPtr	pGC,
    int		srcx,
    int		srcy,
    int		width,
    int		height,
    int		dstx,
    int		dsty
#endif
);

void
xf86ScreenToScreenBitBlt(
#if NeedFunctionPrototypes
    int		nBox,
    DDXPointPtr	pptSrc,
    BoxPtr	pBox,
    int		xdir,
    int		ydir,
    int		rop,
    unsigned	planemask
#endif
);

void
xf86PolyFillRect(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    register GCPtr	pGC,
    int			nrectFill,
    xRectangle		*prectInit
#endif
);

void
xf86SimpleFillRectSolid(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    register GCPtr	pGC,
    int			nBox,
    BoxPtr		pBoxInit
#endif
);

void
xf86ImageGlyphBltTE(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    GCPtr		pGC,
    int			xInit,
    int			yInit,
    int			nglyph,
    CharInfoPtr		*ppci,
    unsigned char *	pglyphBase
#endif
);

void
xf86ImageGlyphBltNonTE(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    GCPtr		pGC,
    int			xInit,
    int			yInit,
    int			nglyph,
    CharInfoPtr		*ppci,
    unsigned char	*pglyphBase
#endif
);

void
xf86PolyGlyphBltTE(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    GCPtr		pGC,
    int			xInit,
    int			yInit,
    int			nglyph,
    CharInfoPtr		*ppci,
    unsigned char *	pglyphBase
#endif
);

void
xf86PolyGlyphBltNonTE(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    GCPtr		pGC,
    int			xInit,
    int			yInit,
    int			nglyph,
    CharInfoPtr		*ppci,
    unsigned char *	pglyphBase
#endif
);

void
xf86ImageTextTECPUToScreenColorExpand(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    GCPtr		pGC,
    int			xInit,
    int			yInit,
    int			nglyph,
    CharInfoPtr		*ppci,
    unsigned char *	pglyphBase
#endif
);

void
xf86ImageTextTEScreenToScreenColorExpand(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    GCPtr		pGC,
    int			xInit,
    int			yInit,
    int			nglyph,
    CharInfoPtr		*ppci,
    unsigned char *	pglyphBase
#endif
);

void
xf86PolyTextTECPUToScreenColorExpand(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    GCPtr		pGC,
    int			xInit,
    int			yInit,
    int			nglyph,
    CharInfoPtr		*ppci,
    unsigned char *	pglyphBase
#endif
);

void
xf86PolyTextTEScreenToScreenColorExpand(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    GCPtr		pGC,
    int			xInit,
    int			yInit,
    int			nglyph,
    CharInfoPtr		*ppci,
    unsigned char *	pglyphBase
#endif
);

void
xf86ImageTextNonTECPUToScreenColorExpand(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    GCPtr		pGC,
    int			xInit,
    int			yInit,
    int			nglyph,
    CharInfoPtr		*ppci,
    unsigned char *	pglyphBase
#endif
);

void
xf86ImageTextNonTEScreenToScreenColorExpand(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    GCPtr		pGC,
    int			xInit,
    int			yInit,
    int			nglyph,
    CharInfoPtr		*ppci,
    unsigned char *	pglyphBase
#endif
);

void
xf86PolyTextNonTECPUToScreenColorExpand(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    GCPtr		pGC,
    int			xInit,
    int			yInit,
    int			nglyph,
    CharInfoPtr		*ppci,
    unsigned char *	pglyphBase
#endif
);

void
xf86PolyTextNonTEScreenToScreenColorExpand(
#if NeedFunctionPrototypes
    DrawablePtr		pDrawable,
    GCPtr		pGC,
    int			xInit,
    int			yInit,
    int			nglyph,
    CharInfoPtr		*ppci,
    unsigned char *	pglyphBase
#endif
);

void xf86WriteBitmapCPUToScreenColorExpand(
#if NeedFunctionPrototypes
    int x,
    int y,
    int w,
    int h,
    unsigned char *src,
    int srcwidth,
    int srcx,
    int srcy,
    int bg,
    int fg,
    int rop,
    unsigned planemask
#endif
);

void xf86WriteBitmapScreenToScreenColorExpand(
#if NeedFunctionPrototypes
    int x,
    int y,
    int w, 
    int h,
    unsigned char *src,
    int srcwidth,
    int srcx,
    int srcy,
    int bg,
    int fg,
    int rop,
    unsigned planemask
#endif
);

void
xf86PolyLine(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr	pGC,
    int		mode,
    int		npt,
    DDXPointPtr pptInit
#endif
);

void
xf86PolySegment(
#if NeedFunctionPrototypes
    DrawablePtr	pDrawable,
    GCPtr	pGC,
    int		nseg,
    register xSegment	*pSeg
#endif
);

void
xf86PolyLine2(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr	pGC,
    int		mode,
    int		npt,
    DDXPointPtr pptInit
#endif
);

void
xf86PolySegment2(
#if NeedFunctionPrototypes
    DrawablePtr	pDrawable,
    GCPtr	pGC,
    int		nseg,
    register xSegment	*pSeg
#endif
);

void
xf86FillRectStippledCPUToScreenColorExpand(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    register GCPtr pGC,
    int nBoxInit,
    BoxPtr pBoxInit
#endif
);

void
xf86FillRectStippledScreenToScreenColorExpand(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    register GCPtr pGC,
    int nBoxInit,
    BoxPtr pBoxInit
#endif
);

void
xf86Bench();

extern int xf86PixmapIndex;
extern unsigned int byte_expand3[256], byte_reversed_expand3[256];

/*
 * Macros to check graphics operation restrictions.
 */

#define CHECKPLANEMASK(flag) \
    (!(flag & NO_PLANEMASK) || (pGC->planemask & \
    xf86AccelInfoRec.FullPlanemask) == xf86AccelInfoRec.FullPlanemask)
#define CHECKROP(flag) \
    (!(flag & GXCOPY_ONLY) || pGC->alu == GXcopy)
#define CHECKTRANSPARENCYROP(flag) \
    (!((flag & GXCOPY_ONLY) || (flag & TRANSPARENCY_GXCOPY)) \
    || pGC->alu == GXcopy)
#define CHECKRGBEQUAL(flag) \
    (!(flag & RGB_EQUAL) || ((pGC->fgPixel & 0xFF) == ((pGC->fgPixel \
    & 0xFF00) >> 8) && (pGC->fgPixel & 0xFF) == ((pGC->fgPixel \
    & 0xFF0000) >> 16)))
#define CHECKRGBEQUALBOTH(flag) \
    (!(flag & RGB_EQUAL) || ((pGC->fgPixel & 0xFF) == ((pGC->fgPixel \
    & 0xFF00) >> 8) && (pGC->fgPixel & 0xFF) == ((pGC->fgPixel \
    & 0xFF0000) >> 16)) && ((pGC->bgPixel & 0xFF) == ((pGC->bgPixel \
    & 0xFF00) >> 8) && (pGC->bgPixel & 0xFF) == ((pGC->bgPixel \
    & 0xFF0000) >> 16)))
/* Check for rops that don't access the source */
#define CHECKSOURCEROP() \
    (pGC->alu != GXclear && pGC->alu != GXnoop && pGC->alu != GXinvert && \
    pGC->alu != GXset)

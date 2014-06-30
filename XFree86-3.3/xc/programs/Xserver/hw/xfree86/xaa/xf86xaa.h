/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86xaa.h,v 3.7.2.1 1997/05/18 12:00:22 dawes Exp $ */


/* AccelInfoRec flags */

#define BACKGROUND_OPERATIONS		0x1
#define PIXMAP_CACHE			0x2
#define COP_FRAMEBUFFER_CONCURRENCY	0x4
#define DO_NOT_CACHE_STIPPLES		0x8
#define TWO_PASS_24BPP_SOLID_FILL	0x10
#define HARDWARE_CLIP_LINE		0x20
#define USE_TWO_POINT_LINE		0x40
#define TWO_POINT_LINE_NOT_LAST		0x80
#define HARDWARE_PATTERN_SCREEN_ORIGIN  0x100
#define HARDWARE_PATTERN_TRANSPARENCY   0x200
#define HARDWARE_PATTERN_ALIGN_64	0x400
#define ONLY_TWO_BITBLT_DIRECTIONS	0x800
#define ONLY_LEFT_TO_RIGHT_BITBLT	0x1000
#define NO_SYNC_AFTER_CPU_COLOR_EXPAND  0x2000
#define TWO_POINT_LINE_ERROR_TERM	0x4000
#define HARDWARE_PATTERN_MOD_64_OFFSET  0x8000
#define HARDWARE_PATTERN_PROGRAMMED_BITS	0x10000
#define HARDWARE_PATTERN_PROGRAMMED_ORIGIN	0x20000
#define HARDWARE_PATTERN_BIT_ORDER_MSBFIRST	0x40000
#define HARDWARE_PATTERN_MONO_TRANSPARENCY	0x80000
#define NO_TEXT_COLOR_EXPANSION		0x100000
#define HARDWARE_PATTERN_NOT_LINEAR	0x200000

/* Graphics operation flags */

#define GXCOPY_ONLY		0x1
#define NO_PLANEMASK		0x2
#define ONE_RECT_CLIPPING	0x4
#define RGB_EQUAL		0x8
#define NO_TRANSPARENCY		0x10
#define NO_CAP_NOT_LAST		0x20
#define TRANSPARENCY_GXCOPY     0x40

/* Color expansion flags */

#define SCANLINE_NO_PAD			0x800
#define SCANLINE_PAD_BYTE		0x1000
#define SCANLINE_PAD_DWORD		0x2000
#define CPU_TRANSFER_PAD_DWORD		0x0
#define CPU_TRANSFER_PAD_QWORD		0x4000
#define CPU_TRANSFER_BASE_FIXED		0x8000
#define ONLY_TRANSPARENCY_SUPPORTED	0x10000
#define TRIPLE_BITS_24BPP		0x20000
#define VIDEO_SOURCE_GRANULARITY_PIXEL	0x40000
#define VIDEO_SOURCE_GRANULARITY_BYTE	0x80000
#define VIDEO_SOURCE_GRANULARITY_DWORD  0x100000
#define BIT_ORDER_IN_BYTE_LSBFIRST	0x0
#define BIT_ORDER_IN_BYTE_MSBFIRST	0x200000
#define LEFT_EDGE_CLIPPING		0x400000
#define LEFT_EDGE_CLIPPING_NEGATIVE_X   0x800000

/* Highest level, GC ops and some ScreenRec fall-backs. */

typedef struct {
    void (*PolyFillRectSolid)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
        int		nrectFill,
        xRectangle	*prectInit
#endif
    );
    int PolyFillRectSolidFlags;
    void (*PolyFillRectTiled)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
        int		nrectFill,
        xRectangle	*prectInit
#endif
    );
    int PolyFillRectTiledFlags;
    void (*PolyFillRectStippled)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
        int		nrectFill,
        xRectangle	*prectInit
#endif
    );
    int PolyFillRectStippledFlags;
    int SecondaryPolyFillRectStippledFlags;
    void (*PolyFillRectOpaqueStippled)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
        int		nrectFill,
        xRectangle	*prectInit
#endif
    );
    int PolyFillRectOpaqueStippledFlags;
    int SecondaryPolyFillRectOpaqueStippledFlags;
    void (*PolyFillArcSolid)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
	int		nArcs,
	xArc		*pArcs
#endif
    );
    int PolyFillArcSolidFlags;
    void (*FillPolygonSolid)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		shape,
        int		mode,
        int		count,
        DDXPointPtr	ptsIn
#endif
    );
    int FillPolygonSolidFlags;
    void (*PolyRectangleSolidZeroWidth)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		nRectInit,
        xRectangle *	pRectInit
#endif
    );
    int PolyRectangleSolidZeroWidthFlags;
    void (*PolyLineSolidZeroWidth)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		mode,
        int		npt,
        DDXPointPtr	pptInit
#endif
    );
    int PolyLineSolidZeroWidthFlags;
    void (*PolySegmentSolidZeroWidth)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		nseg,
        xSegment	*pSeg
#endif
    );
    int PolySegmentSolidZeroWidthFlags;
    void (*PolyGlyphBltNonTE)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		xInit,
        int		yInit,
        int		nglyph,
        CharInfoPtr	*ppci,
        unsigned char   *pglyphBase
#endif
    );
    int PolyGlyphBltNonTEFlags;
    void (*PolyGlyphBltTE)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		xInit,
        int		yInit,
        int		nglyph,
        CharInfoPtr	*ppci,
        unsigned char   *pglyphBase
#endif
    );
    int PolyGlyphBltTEFlags;
    void (*ImageGlyphBltNonTE)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		xInit,
        int		yInit,
        int		nglyph,
        CharInfoPtr	*ppci,
        unsigned char   *pglyphBase
#endif
    );
    int ImageGlyphBltNonTEFlags;
    void (*ImageGlyphBltTE)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		xInit,
        int		yInit,
        int		nglyph,
        CharInfoPtr	*ppci,
        unsigned char   *pglyphBase
#endif
    );
    int ImageGlyphBltTEFlags;
    void (*FillSpansSolid)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		nInit,
        DDXPointPtr	pptInit,
        int		*pwidthInit,
        int		fSorted
#endif
    );
    int FillSpansSolidFlags;
    void (*FillSpansTiled)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		nInit,
        DDXPointPtr	pptInit,
        int		*pwidthInit,
        int		fSorted
#endif
    );
    int FillSpansTiledFlags;
    void (*FillSpansStippled)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		nInit,
        DDXPointPtr	pptInit,
        int		*pwidthInit,
        int		fSorted
#endif
    );
    int FillSpansStippledFlags;
    void (*FillSpansOpaqueStippled)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		nInit,
        DDXPointPtr	pptInit,
        int		*pwidthInit,
        int		fSorted
#endif
    );
    int FillSpansOpaqueStippledFlags;
    RegionPtr (*CopyArea)(
#if NeedNestedPrototypes
        DrawablePtr	pSrcDrawable,
        DrawablePtr	pDstDrawable,
        GC		*pGC,
        int		srcx,
        int		srcy,
        int		width,
        int		height,
        int		dstx,
        int		dsty
#endif
    );
    int CopyAreaFlags;
   
/*
 * These fall-back functions are set during screen initialization.
 */
    void (*FillSpansFallBack)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		nInit,
        DDXPointPtr	pptInit,
        int		*pwidthInit,
        int		fSorted
#endif
    );
    void (*PolyGlyphBltFallBack)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		xInit,
        int		yInit,
        int		nglyph,
        CharInfoPtr	*ppci,
        unsigned char   *pglyphBase
#endif
    );
    void (*ImageGlyphBltFallBack)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		xInit,
        int		yInit,
        int		nglyph,
        CharInfoPtr	*ppci,
        unsigned char   *pglyphBase
#endif
    );
    /* This is a ScreenRec function fall-back when VT-switched away */
    void (*OffScreenCopyWindowFallBack)(
#if NeedNestedPrototypes
	WindowPtr	pWin,
	DDXPointRec	ptOldOrg,
	RegionPtr	prgnSrc
    );
#endif
    RegionPtr (*CopyAreaFallBack)(
#if NeedNestedPrototypes
        DrawablePtr	pSrcDrawable,
        DrawablePtr	pDstDrawable,
        GC		*pGC,
        int		srcx,
        int		srcy,
        int		width,
        int		height,
        int		dstx,
        int		dsty
#endif
    );
    RegionPtr (*cfbBitBltDispatch)(
#if NeedNestedPrototypes
        DrawablePtr	pSrcDrawable,
        DrawablePtr	pDstDrawable,
        GC 		*pGC,
        int		srcx,
        int		srcy,
        int		width,
        int		height,
        int		dstx,
        int		dsty,
        void		(*doBitBlt)(),
        unsigned long	bitPlane
#endif
    );
    void (*CopyPlane1toNFallBack)(
#if NeedFunctionPrototypes
        DrawablePtr	pSrcDrawable,
        DrawablePtr	pDstDrawable,
        int		alu,
        RegionPtr	rgnDst,
        DDXPointPtr	pptSrc,
        unsigned int	planemask,
        unsigned long	bitPlane,
        int		bg,
        int		fg
#endif
    );
    void (*PolyFillRectSolidFallBack)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
        int		nrectFill,
        xRectangle	*prectInit
#endif
    );
} xf86GCInfoRecType;

extern xf86GCInfoRecType xf86GCInfoRec;


/* Intermediate level, no GC ops. */

typedef struct {
    /* These rectangle fill functions are post-clipping. */
    void (*FillRectSolid)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
	int		nBox,
	BoxPtr		pBox
#endif
    );
    void (*FillRectTiled)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
	int		nBox,
	BoxPtr		pBox
#endif
    );
    void (*FillRectTiledFallBack)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
	int		nBox,
	BoxPtr		pBox
#endif
    );
    void (*FillRectStippled)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
	int		nBox,
	BoxPtr		pBox
#endif
    );
    void (*FillRectStippledFallBack)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
	int		nBox,
	BoxPtr		pBox
#endif
    );
    void (*FillRectOpaqueStippled)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
	int		nBox,
	BoxPtr		pBox
#endif
    );
    void (*FillRectOpaqueStippledFallBack)(
#if NeedNestedPrototypes
	DrawablePtr	pDrawable,
	GCPtr		pGC,
	int		nBox,
	BoxPtr		pBox
#endif
    );
    void (*PolyTextTE)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		xInit,
        int		yInit,
        int		nglyph,
        CharInfoPtr	*ppci,
        unsigned char   *pglyphBase
#endif
    );
    void (*PolyTextNonTE)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		xInit,
        int		yInit,
        int		nglyph,
        CharInfoPtr	*ppci,
        unsigned char   *pglyphBase
#endif
    );
    void (*ImageTextTE)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		xInit,
        int		yInit,
        int		nglyph,
        CharInfoPtr	*ppci,
        unsigned char   *pglyphBase
#endif
    );
    void (*ImageTextNonTE)(
#if NeedNestedPrototypes
        DrawablePtr	pDrawable,
        GCPtr		pGC,
        int		xInit,
        int		yInit,
        int		nglyph,
        CharInfoPtr	*ppci,
        unsigned char   *pglyphBase
#endif
    );
    void (*FillSpansSolid)(
#if NeedNestedPrototypes
        int		nInit,
        DDXPointPtr	pptInit,
        int		*pwidthInit,
        int		fSorted,
        int		color,
        int		rop
#endif
    );
    void (*ScreenToScreenBitBlt)(
#if NeedNestedPrototypes
        int		nBox,
        DDXPointPtr	pptSrc,
        BoxPtr		pBox,
        int		xdir,
        int		ydir,
        int		rop,
        unsigned	planemask
#endif
    );
    void (*WriteBitmap)(
#if NeedNestedPrototypes
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
    void (*WriteBitmapFallBack)(
#if NeedNestedPrototypes
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
/* Low-level driver functions. */
    void (*SetupForFillRectSolid)(
#if NeedNestedPrototypes
        int color,
        int rop,
        unsigned planemask
#endif
    );
    void (*SubsequentFillRectSolid)(
#if NeedNestedPrototypes
        int x,
        int y,
        int width,
        int height
#endif
    );
    void (*SetupForScreenToScreenCopy)(
#if NeedNestedPrototypes
        int xdir,
        int dir,
        int rop,
        unsigned planemask,
        int transparency_color
#endif
    );        
    void (*SubsequentScreenToScreenCopy)(
#if NeedNestedPrototypes
        int x1,
        int y1,
        int x2,
        int y2,
        int w,
        int h
#endif
    );
    void (*SubsequentBresenhamLine)(
#if NeedNestedPrototypes
        int x1,
        int y1,
        int octant,
        int e,
        int e1,
        int e2,
        int len
#endif
    );
    void (*SubsequentTwoPointLine)(
#if NeedNestedPrototypes
        int x1,
        int y1,
        int x2,		/* excl. */
        int y2,
        int bias
#endif
    );
    void (*SetClippingRectangle)(
#if NeedNestedPrototypes
        int x1,
        int y1,
        int x2,		/* incl. */
        int y2
#endif
     );
    void (*SetupForFill8x8Pattern)(
#if NeedNestedPrototypes
        int patternx,
        int patterny,
        int rop,
        unsigned int planemask,
        int transparency_color
#endif
    );
    void (*SubsequentFill8x8Pattern)(
#if NeedNestedPrototypes
        int patternx,
        int patterny,
        int x,
        int y,
        int w,
        int h
#endif
    );
    void (*SetupFor8x8PatternColorExpand)(
#if NeedNestedPrototypes
	int patternx,
	int patterny,
	int bg,
	int fg,
	int rop,
	unsigned int planemask
#endif
    );
    void (*Subsequent8x8PatternColorExpand)(
#if NeedNestedPrototypes
        int patternx,
        int patterny,
        int x,
        int y,
        int w,
        int h
#endif
    );
    void (*SetupForCPUToScreenColorExpand)(
#if NeedNestedPrototypes
        int bg,
        int fg,
        int rop,
        unsigned int planemask
#endif
    );
    void (*SubsequentCPUToScreenColorExpand)(
#if NeedNestedPrototypes
        int x,
        int y,
        int w,
        int h,
        int skipleft
#endif
    );
    void (*SetupForScreenToScreenColorExpand)(
#if NeedNestedPrototypes
        int bg,
        int fg,
        int rop,
        unsigned int planemask
#endif
    );
    void (*SubsequentScreenToScreenColorExpand)(
#if NeedNestedPrototypes
	int srcx,
	int srcy,
        int x,
        int y,
        int w,
        int h
#endif
    );
    void (*SetupForScanlineCPUToScreenColorExpand)(
#if NeedNestedPrototypes
        int x,
        int y,
        int w,
        int bg,
        int fg,
        int rop,
        unsigned int planemask
#endif
    );
    void (*SubsequentScanlineCPUToScreenColorExpand)(
#if NeedNestedPrototypes
#endif
    );
    void (*SetupForScanlineScreenToScreenColorExpand)(
#if NeedNestedPrototypes
        int x,
        int y,
        int w,
        int h,
        int bg,
        int fg,
        int rop,
        unsigned int planemask
#endif
    );
    void (*SubsequentScanlineScreenToScreenColorExpand)(
#if NeedNestedPrototypes
	int srcaddr	/* Linear pixel (bit) address. */
#endif
    );
    void (*ImageWrite)(
#if NeedNestedPrototypes
        int x,
        int y,
        int w,
        int h,
        void *src,
        int srcwidth,
        int rop,
        unsigned planemask
#endif
    );
    void (*ImageWriteFallBack)(
#if NeedNestedPrototypes
        int x,
        int y,
        int w,
        int h,
        void *src,
        int srcwidth,
        int rop,
        unsigned planemask
#endif
    );
    void (*VerticalLineGXcopyFallBack)(
#if NeedNestedPrototypes
        int rop,
        unsigned long and,
        unsigned long xor,
        unsigned long *addrl,
        int nlwidth,
        int x1,
        int y1,
        int len
#endif
    );
    void (*BresenhamLineFallBack)(
#if NeedNestedPrototypes
        int rop,
        unsigned long and,
        unsigned long xor,
        unsigned long *addrl,
        int nlwidth,
        int signdx,
        int signdy,
        int axis,
        int x1,
        int y1,
        int e,
        int e1,
        int e2,
        int len
#endif
    );
    void (*xf86GetLongWidthAndPointer)(
#if NeedNestedPrototypes
        DrawablePtr pDrawable,
        int *nlwidth,
        unsigned long **addrl
#endif
    );
    void (*Sync)();
    int Flags;
    int ColorExpandFlags;
    unsigned int *CPUToScreenColorExpandBase;
    unsigned int *CPUToScreenColorExpandEndMarker;
    int CPUToScreenColorExpandRange;
    unsigned char *FramebufferBase;
    int FramebufferWidth;
    int BitsPerPixel;
    unsigned int FullPlanemask;
    int ScratchBufferAddr;	/* Framebuffer address in byte units. */
    unsigned char *ScratchBufferBase;	/* Framebuffer address pointer. */
    int ScratchBufferSize;
    int PingPongBuffers;
    int ErrorTermBits;
    int UsingVGA256;
    ScrnInfoPtr ServerInfoRec;
    int PixmapCacheMemoryStart;
    int PixmapCacheMemoryEnd;
} xf86AccelInfoRecType;

extern xf86AccelInfoRecType xf86AccelInfoRec;


void xf86InitPixmapCache(
#if NeedFunctionPrototypes
    ScrnInfoPtr infoRec,
    int memoryStart,
    int memoryEnd
#endif
);

void xf86InitializeAcceleration(
#if NeedFunctionPrototypes
    ScreenPtr pScreen
#endif
);

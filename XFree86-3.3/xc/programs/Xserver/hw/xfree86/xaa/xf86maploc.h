/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86maploc.h,v 3.3 1997/01/12 10:48:13 dawes Exp $ */


/* Functions that are only referenced from within this directory. */

/*
 * These prototypes depend on xf86gcmap.h being included.
 * There are seperate versions for each depth.
 */

Bool
xf86CreateGC(
#if NeedFunctionPrototypes
    GCPtr	pGC
#endif
);

void
xf86ValidateGC(
#if NeedFunctionPrototypes
    GCPtr		pGC,
    unsigned long	changes,
    DrawablePtr		pDrawable
#endif
);

void
xf86GCNewFillPolygon(
#if NeedFunctionPrototypes
    GCPtr	pGC,
    Bool	new_cfb_line
#endif
);

void
xf86GCNewRectangle(
#if NeedFunctionPrototypes
    GCPtr	pGC,
    Bool	new_cfb_line
#endif
);

void
xf86GCNewLine(
#if NeedFunctionPrototypes
    GCPtr	pGC,
    DrawablePtr pDrawable,
    Bool	new_cfb_line
#endif
);

void
xf86GCNewText(
#if NeedFunctionPrototypes
    GCPtr	pGC,
    Bool	new_cfb_text
#endif
);

void
xf86GCNewFillSpans(
#if NeedFunctionPrototypes
    GCPtr	pGC,
    Bool	new_cfb_fillspans
#endif
);

void
xf86GCNewFillArea(
#if NeedFunctionPrototypes
    GCPtr	pGC,
    Bool	new_cfb_fillarea
#endif
);

void
xf86GCNewCopyArea(
#if NeedFunctionPrototypes
    GCPtr	pGC
#endif
);

void
xf86ImageGlyphBltFallBack(
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
xf86PolyGlyphBltFallBack(
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
xf86FillSpansFallBack(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GC		*pGC,
    int		nInit,
    DDXPointPtr pptInit,
    int *pwidthInit,
    int fSorted
#endif
);

void
xf86FillRectTileFallBack(
#if NeedFunctionPrototypes
    DrawablePtr	    pDrawable,
    GCPtr	    pGC,
    int		    nBox,
    BoxPtr	    pBox
#endif
);

void
xf86FillRectStippledFallBack(
#if NeedFunctionPrototypes
    DrawablePtr	    pDrawable,
    GCPtr	    pGC,
    int		    nBox,
    BoxPtr	    pBox
#endif
);

void
xf86FillRectOpaqueStippledFallBack(
#if NeedFunctionPrototypes
    DrawablePtr	    pDrawable,
    GCPtr	    pGC,
    int		    nBox,
    BoxPtr	    pBox
#endif
);

void
xf86ImageWriteFallBack(
#if NeedFunctionPrototypes
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

void
xf86WriteBitmapFallBack(
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
xf86cfbGetLongWidthAndPointer(
#if NeedFunctionPrototypes
    DrawablePtr	pDrawable,
    int		*nlwidth,
    unsigned long **addrl
#endif
);

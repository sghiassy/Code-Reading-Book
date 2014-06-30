/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/mfb.banked/mfblinebank.h,v 3.3 1996/12/23 06:48:37 dawes Exp $ */
/* mfblinebank.h */
/* included from mfb.h if MFB_LINE_BANK is defined */



/* $XConsortium: mfblinebank.h /main/3 1996/02/21 17:50:03 kaleb $ */

extern PixelType *(* monoScanlineOffsetFuncA)(
#if NeedFunctionPrototypes
PixelType * /* pointer */,
int         /* offset */
#endif
);

extern PixelType *(* monoScanlineOffsetFuncB)(
#if NeedFunctionPrototypes
PixelType * /* pointer */,
int         /* offset */
#endif
);


#define mfbScanlineOffset(_pointer, offset) \
	(*monoScanlineOffsetFuncA)((_pointer), (offset))

#define mfbScanlineInc(_pointer, offset) \
	(_pointer) = mfbScanlineOffset((_pointer), (offset))


#define mfbScanlineOffsetSrc(_pointer, offset) \
	(*monoScanlineOffsetFuncA)((_pointer), (offset))

#define mfbScanlineIncSrc(_pointer, offset) \
	(_pointer) = mfbScanlineOffsetSrc((_pointer), (offset))

#define mfbScanlineDeltaSrc(pointer, y, width) \
	mfbScanlineOffsetSrc(pointer, (y) * (width))
    
#define mfbScanlineSrc(pointer, x, y, width) \
	mfbScanlineOffsetSrc(pointer, (y) * (width) + ((x) >> MFB_PWSH))


#define mfbScanlineOffsetDst(_pointer, offset) \
	(*monoScanlineOffsetFuncB)((_pointer), (offset))

#define mfbScanlineIncDst(_pointer, offset) \
	(_pointer) = mfbScanlineOffsetDst((_pointer), (offset))

#define mfbScanlineDeltaDst(pointer, y, width) \
	mfbScanlineOffsetDst(pointer, (y) * (width))
    
#define mfbScanlineDst(pointer, x, y, width) \
	mfbScanlineOffsetDst(pointer, (y) * (width) + ((x) >> MFB_PWSH))

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/ibm8514/8514cach.h,v 3.4 1996/12/23 06:37:36 dawes Exp $ */





/* $XConsortium: 8514cach.h /main/3 1996/02/21 17:23:35 kaleb $ */

void ibm8514GlyphWrite(
#if NeedFunctionPrototypes
    int /*x*/,
    int /*y*/,
    int /*count*/,
    unsigned char */*chars*/,
    CacheFont8Ptr /*fentry*/,
    GCPtr /*pGC*/,
    BoxPtr /*pBox*/,
    int /*numRects*/
#endif
);

int ibm8514NoCPolyText(
#if NeedFunctionPrototypes
    DrawablePtr /*pDraw*/,
    GCPtr /*pGC*/,
    int /*x*/,
    int /*y*/,
    int /*count*/,
    char */*chars*/,
    Bool /*is8bit*/
#endif
);

int ibm8514NoCImageText(
#if NeedFunctionPrototypes
    DrawablePtr /*pDraw*/,
    GCPtr /*pGC*/,
    int /*x*/,
    int /*y*/,
    int /*count*/,
    char */*chars*/,
    Bool /*is8bit*/
#endif
);

void ibm8514CacheMoveBlock(
#if NeedFunctionPrototypes
    int /*srcx*/,
    int /*srcy*/,
    int /*dstx*/,
    int /*dsty*/,
    int /*h*/,
    int /*len*/,
    unsigned int /*id*/
#endif
);


void ibm8514FontOpStipple(
#if NeedFunctionPrototypes
    int /*x*/,
    int /*y*/,
    int /*w*/,
    int /*h*/,
    unsigned char */*psrc*/,
    int /*pwidth*/,
    Pixel /*id*/
#endif
);

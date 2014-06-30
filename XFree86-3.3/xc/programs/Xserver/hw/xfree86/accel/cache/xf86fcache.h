/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/cache/xf86fcache.h,v 3.8 1997/01/08 20:33:31 dawes Exp $ */

/*
 * Data structures and function prototypes for the font cache.
 */

/* $XConsortium: xf86fcache.h /main/7 1996/02/21 17:19:23 kaleb $ */

#ifndef _XF86_FCACHE_H
#define _XF86_FCACHE_H

/*
 * This struct contains all info for a cached font.
 */
typedef struct _CacheFont8 {
     FontPtr font;		       /* font */
     CharInfoPtr pci[256];	       /* font infos */
     short w;			       /* font cache spacing */
     short h;			       /* font max height */
     struct _bitMapBlock *fblock[8];   /* 8 * 32 cache block chars */
     struct _CacheFont8 *next;	       /* next */
}
CacheFont8Rec;

typedef struct _CacheFont8 *CacheFont8Ptr;

/*
 * Entrypoints into cache code.
 */

void xf86ReleaseFontCache(
#if NeedFunctionPrototypes
    void
#endif
);

void xf86InitFontCache(
#if NeedFunctionPrototypes
    CachePool /*FontCache*/,
    int /*RowWidth*/,
    int /*RowHeight*/,
    void (* /*FontOpStippleFunc*/)(
#if NeedNestedPrototypes
	int, int, int, int, unsigned char *, int, Pixel
#endif
    )
#endif
);

void xf86UnCacheFont8(
#if NeedFunctionPrototypes
    FontPtr /*font*/
#endif
);
     
CacheFont8Ptr xf86CacheFont8(
#if NeedFunctionPrototypes
    FontPtr /*font*/
#endif
);

void xf86loadFontBlock(
#if NeedFunctionPrototypes
    CacheFont8Ptr /*fentry*/,
    int /*block*/
#endif
);

#endif /* _XF86_FCACHE_H */

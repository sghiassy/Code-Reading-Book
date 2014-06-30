/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86pcache.h,v 3.2 1997/01/18 06:57:28 dawes Exp $ */


typedef struct _CacheInfo {
    int id;             /* cache id */
    int x;              /* x position in offscreen memory */
    int y;              /* y position in offscreen memory */
    int w;              /* expanded width */
    int h;              /* expanded height */
    int nx;             /* number of times expanded in x */
    int ny;             /* number of times expanded in y */
    int pix_w;          /* original pixmap width */
    int pix_h;          /* original pixmap height */
    int fg_color;       /* stipple foreground color */
    int bg_color;       /* stipple background color */
    int cache_width;    /* width of cache slot (typically 64, 128, or 256) */
    int cache_height;   /* height of cache slot (typically 64, 128, or 256) */
    int flags;          /* non zero if fixed pattern */
    int pattern0;       /* if fixed pattern, pattern 0 register contents */
    int pattern1;       /* if fixed pattern, pattern 1 register contents */
    unsigned int lru;   /* lru counter */
} CacheInfo, *CacheInfoPtr;

#ifdef PIXPRIV
typedef struct _xf86PixPriv {
    int slot;		/* Offscreen cache slot number */
} xf86PixPrivRec, *xf86PixPrivPtr;
#endif

int xf86CacheTile(
#if NeedFunctionPrototypes
    PixmapPtr pix
#endif
);

int xf86CacheStipple(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr	pGC
#endif
);

void xf86InitPixmapCacheSlots();

void xf86InvalidatePixmapCache();

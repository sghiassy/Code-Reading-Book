/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86pcache.c,v 3.9.2.6 1997/05/29 14:01:06 dawes Exp $ */

/*
 * Copyright 1996  The XFree86 Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * HARM HANEMAAYER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Written by Harm Hanemaayer (H.Hanemaayer@inter.nl.net).
 *
 * Pixmap caching, based on mach64pcach.c.
 *
 * Currently uses one set of pixmap slots with a maximum size of 128x128,
 * plus some wide slots (disabled).
 * The current scheme is superior to that of S3 and other accel servers,
 * which always reload the tiling pixmap, but inferior to the Mach64
 * server, which has a larger set of slots.
 *
 * The pixmap cache is initialized by
 *     xf86InitPixmapCache(infoRec, memoryStart, memoryEnd)
 * where infoRec is a pointer to the ScrnInfoRec, and memoryStart and 
 * memoryEnd delimit addressable video memory available to the cache.
 *
 * The numbering of slots is slightly different from the Mach64 server,
 * slot 0 is the first valid slot.
 *
 * Future improvements:
 * - Fix stipple caching for 24bpp (problem is with fall-back function to
 *   load stipple into cache).
 * - Increase the number of slots if video memory is available, and vary
 *   the sizes of them.
 * - When searching for a slot, start with appropriately sized ones.
 * - Implement non-sparse stipple caching (with color expansion).
 * - Allow patterns to be stored linearly in video memory, which
 *   reduces the mount of non-page mode video memory accesses.
 * - Use a hardware 8x8 pattern for more cases (not just small tiles).
 *
 *
 *     ----------------------------     --------------------------------
 *     |    128    |    128    |           |    128    |    128    |
 *     | 128       |           |           |           |           |
 *     | or        |           |   <--->   |           |           |
 *     | < 128     |           |           |           |           |
 *     |           |           |           |           |           |
 *     -----------------------------------------------------------------
 *     | 32        width / 2           |           width / 2           |
 *     -----------------------------------------------------------------
 *     | 32        width / 2           |           width / 2           |
 *     -----------------------------------------------------------------
 *    ...                             ...                             ...
 *     -----------------------------------------------------------------
 *     | < 32      width / 2           |           width / 2           |
 *     -----------------------------------------------------------------
 */

#include "scrnintstr.h"
#include "pixmapstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "region.h"
#include "servermd.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_Config.h"
#include "xf86xaa.h"
#include "xf86gcmap.h"
#include "xf86local.h"
#include "xf86pcache.h"

extern WindowPtr *WindowTable;
extern unsigned char byte_reversed[256];

static void DoCacheTile(
#if NeedFunctionPrototypes
    PixmapPtr pix
#endif
);

static int DoCacheStipple(
#if NeedFunctionPrototypes
    int slot,
    DrawablePtr pDrawable,
    GCPtr pGC
#endif
);

static int FindCacheSlot(
#if NeedFunctionPrototypes
    PixmapPtr pix
#endif
);

static void IncrementCacheLRU(
#if NeedFunctionPrototypes
    int slot
#endif
);

static void DoCacheExpandPixmap(
#if NeedFunctionPrototypes
    CacheInfoPtr pci
#endif
);

static Bool xf86CachableTile(
#if NeedFunctionPrototypes
    PixmapPtr pix
#endif
);

CacheInfoPtr xf86CacheInfo = NULL;
static int MaxSlot;
static int MaxWidth;
static int MaxHeight;
static unsigned int pixmap_cache_clock = 1;
static int FirstWideSlot;
static int MaxWideSlotHeight;

void xf86InitPixmapCacheSlots()
{
    ScrnInfoPtr infoRec;
    int memoryStart, memoryEnd;
    int width_in_bytes, cache_start_y, i;
    int height_left, standard_slots, wide_slots, wide_slot_width;

    infoRec = xf86AccelInfoRec.ServerInfoRec;
    memoryStart = xf86AccelInfoRec.PixmapCacheMemoryStart;
    memoryEnd = xf86AccelInfoRec.PixmapCacheMemoryEnd;

    width_in_bytes = infoRec->displayWidth * infoRec->bitsPerPixel / 8;
    standard_slots = infoRec->displayWidth / 128;
    cache_start_y = (memoryStart + width_in_bytes - 1) / width_in_bytes;
    MaxWidth = infoRec->displayWidth;
    MaxHeight = memoryEnd / width_in_bytes - cache_start_y;
    if (MaxWidth > 128)
        MaxWidth = 128;
    height_left = 0;
    if (MaxHeight > 128) {
        height_left = MaxHeight - 128;
        MaxHeight = 128;
   }
   /* XXXX DEBUGGING ONLY */
/*   cache_start_y = infoRec->virtualY - MaxHeight; */
   if (MaxHeight < 1) {
       /* Disable pixmap caching if the memory parameters are wrong. */
       xf86AccelInfoRec.Flags &= ~PIXMAP_CACHE;
       ErrorF("%s %s: Pixmap cache disabled - no video memory available\n",
               XCONFIG_PROBED, infoRec->name);
       return;
    }

   if ((MaxHeight < 8) && 
       (!(xf86AccelInfoRec.Flags & HARDWARE_PATTERN_PROGRAMMED_ORIGIN))) {
       if ((xf86AccelInfoRec.Flags & HARDWARE_PATTERN_PROGRAMMED_BITS) &&
	     (xf86AccelInfoRec.SubsequentFill8x8Pattern)) {
	   ErrorF("%s %s: XAA: 8x8 Pattern fill disabled - insufficient video memory available\n", 
		  XCONFIG_PROBED, infoRec->name);
	   xf86AccelInfoRec.SubsequentFill8x8Pattern = 0;
       } else if ((xf86AccelInfoRec.Subsequent8x8PatternColorExpand) ||
	     (xf86AccelInfoRec.SubsequentFill8x8Pattern)) {
	   ErrorF("%s %s: XAA: 8x8 Pattern fill disabled - insufficient video memory available\n", 
		  XCONFIG_PROBED, infoRec->name);
	   xf86AccelInfoRec.Subsequent8x8PatternColorExpand = 0;
	   xf86AccelInfoRec.SubsequentFill8x8Pattern = 0;
       }
   }

    /*
     * If space is available, put a few wide, low slots in for small
     * patterns. They are (displayWidth / 2) x 32 big. If there are
     * less than 32 scanlines left, just add 2 wide slots.
     * They are physically placed below the first bunch of slots.
     */
    /* XXX DISABLED */
    wide_slots = (height_left / 32) * 2;
    MaxWideSlotHeight = 0;
    if (wide_slots == 0 && height_left > 0) {
        wide_slots = 2;		/* Limited height. */
        MaxWideSlotHeight = height_left;
    }
#if 1
    wide_slots = 0;
#endif
    if (height_left >= 32)
        MaxWideSlotHeight = 32;
    if (wide_slots > 8)
        wide_slots = 8;
    if (wide_slots > 0) {
        wide_slot_width = infoRec->displayWidth / 2;
        /* Make width a power of two. */
        for (i = 1;; i++) {
            if (wide_slot_width < (1 << i)) {
                wide_slot_width = 1 << (i - 1);
                break;
            }
        }
    }

    MaxSlot = standard_slots + wide_slots - 1;
    xf86CacheInfo = (CacheInfoPtr)xcalloc(MaxSlot + 1, sizeof(CacheInfo));
    FirstWideSlot = standard_slots;

    ErrorF("%s %s: XAA: Using %d %dx%d ",
        XCONFIG_PROBED, infoRec->name, standard_slots, MaxWidth, MaxHeight);
    if (wide_slots > 0)
        ErrorF("and %d %dx%d ", wide_slots, (infoRec->displayWidth / 2),
            min(32, height_left));
    ErrorF("areas for pixmap caching\n");

    for (i = 0; i < standard_slots; i++) {
        xf86CacheInfo[i].x = i * MaxWidth;
        xf86CacheInfo[i].y = cache_start_y;
        xf86CacheInfo[i].cache_width = MaxWidth;
        xf86CacheInfo[i].cache_height = MaxHeight;
    }
    for (i = 0; i < wide_slots; i ++) {
        xf86CacheInfo[i + standard_slots].x =
            (i % 2) * infoRec->displayWidth / 2;
        xf86CacheInfo[i + standard_slots].y =
            cache_start_y + MaxHeight + (i / 2) * 32;
        xf86CacheInfo[i + standard_slots].cache_width =
            infoRec->displayWidth / 2;
        xf86CacheInfo[i + standard_slots].cache_height = min(32, height_left);
    }

    for (i = 0; i <= MaxSlot; i++) {
        xf86CacheInfo[i].id = -1;
        xf86CacheInfo[i].lru = pixmap_cache_clock;
        xf86CacheInfo[i].fg_color = 0;
        xf86CacheInfo[i].bg_color = 0;
        xf86CacheInfo[i].flags = 0;
    }
}

void xf86InvalidatePixmapCache() {
    int i;

    if (xf86CacheInfo != NULL)
       for (i = 0; i <= MaxSlot; i++)
           xf86CacheInfo[i].id = -1;
}

int xf86CacheTile(pix)
    PixmapPtr pix;
{
#ifdef PIXPRIV
    int slot;
    xf86PixPrivPtr devPriv = 
	(xf86PixPrivPtr)(pix->devPrivates[xf86PixmapIndex].ptr);

    /* First see if tile is already cached */
    if ((devPriv->slot >= 0 && devPriv->slot <= MaxSlot) &&
	(xf86CacheInfo[devPriv->slot].id == pix->drawable.serialNumber)) 
    {
        IncrementCacheLRU(devPriv->slot);
	return (1);
    } 
    else if (xf86CachableTile(pix)) {
        slot = FindCacheSlot(pix);

	devPriv->slot = slot;
        IncrementCacheLRU(slot);
	DoCacheTile(pix);
	return (1);
    }
#endif
    return (0);
}

/*
 * It's handy to pass the Drawable and GC since we might need them
 * to draw the stipple into the cache.
 */

int xf86CacheStipple(pDrawable, pGC)
    DrawablePtr pDrawable;
    GCPtr pGC;
{
    PixmapPtr pix;
    xf86PixPrivPtr devPriv;
    int slot;
    pix = pGC->stipple;
#ifdef PIXPRIV
    devPriv = (xf86PixPrivPtr)(pix->devPrivates[xf86PixmapIndex].ptr);

    if (devPriv->slot == MaxSlot + 1)
        /*
         * Special value indicates that this pattern was found to be
         * non-cachable.
         */
        return 0;

    /* First see if stipple is already cached */
    if ((devPriv->slot >= 0 && devPriv->slot <= MaxSlot) &&
	(xf86CacheInfo[devPriv->slot].id == pix->drawable.serialNumber) &&
	/* No need to match colors when using mono pattern. */
	(xf86CacheInfo[devPriv->slot].flags == 2 ||
	((xf86CacheInfo[devPriv->slot].fg_color == pGC->fgPixel) &&
	(xf86CacheInfo[devPriv->slot].bg_color == pGC->bgPixel ||
	pGC->fillStyle == FillStippled))))
    {
        IncrementCacheLRU(devPriv->slot);
	return (1);
    } 
    else if (xf86CachableTile(pix)) {
        slot = FindCacheSlot(pix);

        if (!DoCacheStipple(slot, pDrawable, pGC)) {
            /*
             * Mark stipple as "non-cacheable".
             * This happens when we are hoping to use the 8x8 hardware
             * pattern, but can't, and ScreenToScreencopy doesn't have
             * the required support for transparency.
             */
            devPriv->slot = MaxSlot + 1;
            return 0;
        }
        devPriv->slot = slot;
        IncrementCacheLRU(slot);
	return (1);
    }
#endif
    return 0;
}

static int FindCacheSlot(pix)
    PixmapPtr pix;
{
    int i, j, best;
    if (pix->drawable.height <= MaxWideSlotHeight && FirstWideSlot <= MaxSlot)
    	/*
    	 * Start with the most appropriate slots.
    	 * It might be a good idea to force using a wide slot if there
    	 * is a bunch of them.
    	 */
        j = FirstWideSlot;
    else
        j = 0;
    best = j;
    for (i = 0; i <= MaxSlot; i++) {
         if (xf86CacheInfo[j].cache_width >= pix->drawable.width
         && xf86CacheInfo[j].cache_height >= pix->drawable.height) {
             if (xf86CacheInfo[j].id == -1)
                 return j;
             else
                 if (xf86CacheInfo[j].lru < xf86CacheInfo[best].lru)
                     best = j;
         }
         j++;
         if (j > MaxSlot)
             j = 0;
    }
    return best;
}

static void
IncrementCacheLRU(slot)
    int slot;
{
    int i;

    /* Checks for integer overflow. */
    if (!(xf86CacheInfo[slot].lru = ++pixmap_cache_clock))
    {
	for (i = 0; i <= MaxSlot; i++)
	    xf86CacheInfo[i].lru = pixmap_cache_clock;
    }
}

/*
 * Write an 8x8 pattern linearly to video memory.
 * The width and height can be 1, 2, 4, or 8, and the pattern is
 * automatically expanded to 8x8 if necessary. 
 * Two copies are written consecutively to allow vertical rotation.
 * Horizontal rotation must be handled by writing 7 extra versions
 * on different scanlines for a total of 16 copies.
 * This needs a 128 pixels wide and 8 pixels high cache slot.
 *
 * There are trouble makers; some chipsets require the pattern to be
 * on a 64 pixel boundary. This is incompatible with the vertical
 * rotation. For these chips, we would have to write 64 copies of the
 * pattern. The chip might support left-edge clipping, which can
 * take care of horizontal rotation but that causes problems at the
 * left edge of the screen.
 *
 * A curious case occurs with chips that require 64-pixel alignment,
 * but allow a vertical pattern origin offset multiplied by 8 to be
 * added to the pattern pixel address. They match exactly the default
 * vertical rotation scheme that stores two copies on a scanline.
 *
 * Finally, if the chip supports a programmable origin, there's no
 * need to create pre-rotated copies. 
 */

static void Write8x8Pattern(pci, w, h, pSrc, srcwidth)
    CacheInfoPtr pci;
    int w, h;
    unsigned char *pSrc;
    int srcwidth;
{
    int x, y;
    unsigned char *buf, *bufp, *buf2;
    int i, bytespp, nh;
    x = pci->x;
    y = pci->y;
    w = min(w, 8);
    h = min(h, 8);
    bytespp = xf86AccelInfoRec.BitsPerPixel / 8;
    buf = (unsigned char *)ALLOCATE_LOCAL(bytespp * 128);
    buf2 = (unsigned char *)ALLOCATE_LOCAL(bytespp * 128);
    bufp = buf;
    /* Write and expand horizontally. */
    for (i = 0; i < h; i++) {
         int nw;
         memcpy(bufp, pSrc, w * bytespp);
         nw = w;
         while (nw != 8) {
            memcpy(bufp + nw * bytespp, bufp, nw * bytespp);
            nw *= 2;
         }
         pSrc += srcwidth;
         bufp += 8 * bytespp;
    }
    nh = h;
    /* Expand vertically. */
    while (nh != 8) {
        memcpy(buf + nh * 8 * bytespp, buf, nh * 8 * bytespp);
        nh *= 2;
    }
    if (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_PROGRAMMED_ORIGIN) {
    	/*
    	 * This is easy. Just one copy required. Also consider alignment
    	 * constraints.
    	 */
    	if (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_ALIGN_64) {
    	    /*
    	     * We have a 128 pixel wide cache slot, so there is a horizontal
    	     * offset into the cache slot where we can store the 64-pixel
    	     * long pattern on 64-pixel boundary.
    	     */
    	    int mod64;
    	    mod64 = (xf86AccelInfoRec.FramebufferWidth * y + x) & 63;
    	    if (mod64 > 0) {
    	        x += 64 - mod64;
    	        pci->x += 64 - mod64;
    	    }
    	}
        xf86AccelInfoRec.ImageWrite(x, y, 64, 1, buf, 64 * bytespp,
            GXcopy, 0xFFFFFFFF);
        DEALLOCATE_LOCAL(buf2);
        DEALLOCATE_LOCAL(buf);
        return;
    }
    /* Make it two copies. */
    memcpy(buf + 64 * bytespp, buf, 64 * bytespp);
    /* Write to video memory. */
    xf86AccelInfoRec.ImageWrite(x, y, 128, 1, buf, 128 * bytespp,
        GXcopy, 0xFFFFFFFF);
    /* Make 7 horizontally rotated versions. */
    for (i = 1; i < 8; i++) {
        int j;
        for (j = 0; j < 8; j++) {
            memcpy(buf2 + (j * 8) * bytespp, buf + (j * 8 + i) * bytespp,
                (8 - i) * bytespp);
            memcpy(buf2 + (j * 8 + 8 - i) * bytespp, buf + j * 8 * bytespp,
                i * bytespp);
        }
        memcpy(buf2 + 64 * bytespp, buf2, 64 * bytespp);
        xf86AccelInfoRec.ImageWrite(x, y + i, 128, 1, buf2,
            128 * bytespp, GXcopy, 0xFFFFFFFF);
    }
    DEALLOCATE_LOCAL(buf2);
    DEALLOCATE_LOCAL(buf);
}

/*
 * Coprocessor version. Assumes tile written at (x, y + 8).
 * This is probably not a good idea, the above function is used instead.
 */

static void Expand8x8Pattern(x, y, w, h, pSrc, srcwidth)
    int x, y, w, h;
    unsigned char *pSrc;
    int srcwidth;
{
    int i, nh;

    xf86AccelInfoRec.SetupForScreenToScreenCopy(1, 1, GXcopy, 0xFFFFFFFF, -1);

    /* First make it a 8x8 pattern. */
    w = min(w, 8);
    h = min(h, 8);
    /* Write and expand horizontally. */
    for (i = 0; i < h; i++) {
         int nw;
         xf86AccelInfoRec.SubsequentScreenToScreenCopy(
             x, y + 8 + i, x + 8 * i, y, w, 1);
         nw = w;
         while (nw != 8) {
            xf86AccelInfoRec.SubsequentScreenToScreenCopy(
                x + 8 * i, y, x + 8 * i + nw, y, nw, 1);
            nw *= 2;
         }
    }
    nh = h;
    /* Replicate vertically (horizontally on-screen). */
    while (nh != 8) {
        xf86AccelInfoRec.SubsequentScreenToScreenCopy(
            x, y, x + nh * 8, y, nh * 8, 1);
        nh *= 2;
    }
    /* Make it two copies. */
    xf86AccelInfoRec.SubsequentScreenToScreenCopy(
        x, y, x + 64, y, 64, 1);
    /* Make 7 horizontally rotated versions, on different scanlines. */
    for (i = 1; i < 8; i++) {
        int j;
        for (j = 0; j < 8; j++) {
            xf86AccelInfoRec.SubsequentScreenToScreenCopy(
                x + j * 8 + i, y, x + j * 8, y + i, 8 - i, 1);
            xf86AccelInfoRec.SubsequentScreenToScreenCopy(
                x + j * 8, y, x + j * 8 + 8 - i, y + i, i, 1);
        }
        /* Make it two copies. */
        xf86AccelInfoRec.SubsequentScreenToScreenCopy(
            x, y + i, x + 64, y + i, 64, 1);
    }
    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
        xf86AccelInfoRec.Sync();
}


/*
 * Convert a stipple to a monochrome 8x8 pattern, expanding if
 * necessary.
 */

static void ExpandStippleTo8x8MonoPattern(w, h, pSrc, srcwidth, pattern)
    int w, h;
    unsigned char *pSrc;
    int srcwidth;
    unsigned char *pattern;	/* 8 bytes of resulting pattern data */
{
    int i, nh;
    w = min(w, 8);
    h = min(h, 8);
    /* Write to pattern[i] and expand horizontally. */
    for (i = 0; i < h; i++) {
	 pattern[i] = *pSrc;
	 if (w < 8) {
	     int j;
             pattern[i] &= (1 << w) - 1;
	     for (j = w; j < 8; j += w)
	         pattern[i] |= pattern[i] << j;
	 }
         pSrc += srcwidth;
    }
    nh = h;
    /* Expand vertically. */
    while (nh != 8) {
        int j;
        for (j = 0; j < nh; j++)
            pattern[nh + j] = pattern[j];
        nh *= 2;
    }
}

/*
 * Write a monochrome 8x8 pattern for use with color-expand pattern fill.
 * On each scanline, 8 vertically rotated versions are written.
 * There are 8 scanlines with horizontally rotated versions, for a total
 * of 64 patterns.
 */

static void WriteRotatedMonoPatterns(x, y, pattern)
    int x, y;
    unsigned char *pattern;
{
    unsigned char *buf;
    int i;

    /* Note extra bytes to support 24bpp ImageWrite. */
    buf = (unsigned char *)ALLOCATE_LOCAL(8 * 8 + 8);
    /* For each horizontal rotation position. */
    for (i = 0; i < 8; i++) {
        /* Write 8 vertically rotated versions. */
        int j;
        if (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_BIT_ORDER_MSBFIRST) {
            int k;
            for (k = 0; k < 8; k++)
                buf[k] = byte_reversed[pattern[k]];
        }
        else
            memcpy(buf, pattern, 8);
        if (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_PROGRAMMED_ORIGIN) {
            /* Special case; we need just one copy. */
            xf86AccelInfoRec.ImageWrite(x, y,
                64 / (xf86AccelInfoRec.BitsPerPixel / 8) + 1, 1, buf,
                64 / (xf86AccelInfoRec.BitsPerPixel / 8) + 1, GXcopy,
                0xFFFFFFFF);
            DEALLOCATE_LOCAL(buf);
            return;
        }
        for (j = 1; j < 8; j++) {
            memcpy(buf + j * 8, buf + j, 8 - j);
            memcpy(buf + j * 8 + 8 - j, buf, j);
        }
        xf86AccelInfoRec.ImageWrite(x, y + i,
            64 / (xf86AccelInfoRec.BitsPerPixel / 8) + 1, 1, buf,
            64 / (xf86AccelInfoRec.BitsPerPixel / 8) + 1, GXcopy, 0xFFFFFFFF);
        if (i < 7) {
            /* Rotate. */
            int k;
            if (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_BIT_ORDER_MSBFIRST)
                for (k = 0; k < 8; k++)
                    pattern[k] = (pattern[k] >> 1) | (pattern[k] << 7);
            else
                for (k = 0; k < 8; k++)
                    pattern[k] = (pattern[k] << 1) | (pattern[k] >> 7);
        }
    }
    DEALLOCATE_LOCAL(buf);
}

static void WriteStippleAs8x8MonoPattern(x, y, w, h, pSrc, srcwidth)
    int x, y, w, h;
    unsigned char *pSrc;
    int srcwidth;
{
    unsigned char pattern[8];

    ExpandStippleTo8x8MonoPattern(w, h, pSrc, srcwidth, pattern);
    WriteRotatedMonoPatterns(x, y, pattern);
}

/*
 * Convert a tile to a mono pattern, which is guaranteed to be possible.
 * Since the dectection routine doesn't support 24bpp, we don't need
 * to support it either.
 */

static void ConvertTileTo8x8MonoPattern(w, h, pSrc, srcwidth, pattern, bg, fg)
    int w, h;
    unsigned char *pSrc;
    int srcwidth;
    unsigned char *pattern;
    int *bg, *fg;
{
    unsigned char *srcp;
    int bytespp;
    int color1, color2;
    int y;
    bytespp = xf86AccelInfoRec.BitsPerPixel / 8;
    srcp = pSrc;
    /* The first pixel will be the foreground color. */
    switch (bytespp) {
    case 1 :
        color1 = *((unsigned char *)srcp);
        break;
    case 2 :
        color1 = *((unsigned short *)srcp);
        break;
    case 4 :
        color1 = *((unsigned int *)srcp);
        break;
    }
    for (y = 0; y < min(8, h); y++) {
        int i;
        pattern[y] = 0;
        for (i = 0; i < min(8, w); i++) {
            int c;
            switch (bytespp) {
            case 1 :
                c = *((unsigned char *)srcp + i);
                break;
            case 2 :
                c = *((unsigned short *)srcp + i);
                break;
            case 4 :
                c = *((unsigned int *)srcp + i);
                break;
            }
            if (c == color1)
                pattern[y] |= 1 << i;
            else
                color2 = c;
        }
        srcp += srcwidth;
    }
    /* Now expand it to 8x8 if required. */
    if (w < 8 || h < 8)
        /* This function fits our needs. */
        ExpandStippleTo8x8MonoPattern(w, h, pattern, 1, pattern);
    *fg = color1;
    *bg = color2;
}

static void WriteTileAs8x8MonoPattern(pci, w, h, pSrc, srcwidth)
    CacheInfoPtr pci;
    int w, h;
    unsigned char *pSrc;
    int srcwidth;
{
    unsigned char pattern[8];
    int bg, fg;

    ConvertTileTo8x8MonoPattern(w, h, pSrc, srcwidth, pattern, &bg, &fg);
    pci->bg_color = bg;
    pci->fg_color = fg;
    WriteRotatedMonoPatterns(pci->x, pci->y, pattern);
}

/*
 * Determine whether tile can be reduced to 8 wide/high, and whether
 * it can be reduced to a mono pattern.
 * This does not support 24bpp.
 */

#define TILE_REDUCIBLE	1
#define TILE_MONOCHROME	2
#define TILE_UNKNOWN 4

static int ReduceTileToSize8(pci, pix, checkmono)
    CacheInfoPtr pci;
    PixmapPtr pix;
    int checkmono;
{
    int w, h;
    int y;
    unsigned char *srcp;
    int bytespp;
    Bool mono;
    int color1, color2;
    w = pci->pix_w;
    h = pci->pix_h;
    /* w and h must be power of 2. */
    if ((w & (w - 1)) || (h & (h - 1)))
        return 0;
    /* Small tiles are OK. */
    if (!checkmono && w <= 8 && h <= 8)
        return TILE_REDUCIBLE;
    if (pix->drawable.bitsPerPixel == 24)
        /* Reducing larger tiles is not supported for 24pp right now. */
    	return 0;
    /* w and h must be reducible to 8. */
    if ((w > 8 && w != 16 && w != 32) || (h > 8 && h != 16 && h != 32))
        return 0;
    bytespp = pix->drawable.bitsPerPixel / 8;
    srcp = pix->devPrivate.ptr;
    mono = TRUE;
    if (checkmono) {
        color1 = -1;
        color2 = -1;
    }
    for (y = 0; y < min(8, h); y++) {
        /* We can check for "mono" tiles here. */
        if (checkmono && mono) {
            int i;
            for (i = 0; i < w; i++) {
                int c;
                switch (pix->drawable.bitsPerPixel / 8) {
                case 1 :
                    c = *((unsigned char *)srcp + i);
                    break;
                case 2 :
                    c = *((unsigned short *)srcp + i);
                    break;
                case 4 :
                    c = *((unsigned int *)srcp + i);
                    break;
                }
                if (color1 == -1)
                    color1 = c;
                else
                    if (c != color1)
                        if (color2 == -1)
                            color2 = c;
                        else
                            if (c != color2)
                                mono = FALSE;
            }
        }
        /* Make sure that the tile is horizontally repeated. */
        if (w > 8) {
            int i, j, bytewidth8, bytewidth8_times_i;
            bytewidth8 = bytespp * 8;
            /* gcc seems too stupid to eliminate mul in inner loop. */
            bytewidth8_times_i = bytewidth8;
            /* For each extra horizontal repetition. */
            for (i = 1; i < w / 8; i++) {
                /* For each word in the horizontal repetion. */
                for (j = 0; j < bytewidth8; j += 4)
                    /* Check that the i-th repetion matches the first. */
                    if (*(unsigned int *)(srcp + j) !=
                    *(unsigned int *)(srcp + bytewidth8_times_i + j))
                        return 0;
                bytewidth8_times_i += bytewidth8;
            }
        }
        srcp += pix->devKind;
    }
    if (w <= 8 && h <= 8) {
        if (checkmono && mono)
            return TILE_REDUCIBLE | TILE_MONOCHROME;
        else
            return TILE_REDUCIBLE;
    }
    /*
     * When the height is 16 or 32, check whether the whole tile is
     * repeated vertically.
     */
    for (y = 8; y < h; y++) {
        int i;
        unsigned char *srcp2;
        /* Calculate pointer to top part of tile to compare to. */
        srcp2 = (unsigned char *)(pix->devPrivate.ptr) + pix->devKind * (y & 7);
        switch (w * bytespp) {
        case 1 :
            if (*(unsigned char *)(srcp) != *(unsigned char *)srcp2)
                return 0;
            break;
        case 2 :
            if (*(unsigned short *)(srcp) != *(unsigned short *)srcp2)
                return 0;
            break;
        default : /* width 4, 8, 16 or 32 at 8bpp */
            for (i = 0; i < w * bytespp; i += 4)
                if (*(unsigned int *)(srcp + i) != *(unsigned int *)(srcp2 + i))
                    return 0;
            break;
        }
        srcp += pix->devKind;
    }
    if (w > 8)
        pci->pix_w = 8;
    if (h > 8)
        pci->pix_h = 8;
    if (checkmono && mono)
        return TILE_REDUCIBLE | TILE_MONOCHROME;
    else
        return TILE_REDUCIBLE;
}

static void DoCacheTile(pix)
    PixmapPtr pix;
{
#ifdef PIXPRIV
    int reducible_status;
    xf86PixPrivPtr devPriv = 
	(xf86PixPrivPtr)(pix->devPrivates[xf86PixmapIndex].ptr);
    CacheInfoPtr pci = &xf86CacheInfo[devPriv->slot];

    pci->pix_w = pix->drawable.width;
    pci->pix_h = pix->drawable.height;
    pci->nx = pci->cache_width / pix->drawable.width;
    pci->ny = pci->cache_height / pix->drawable.height;
    pci->w = pci->nx * pci->pix_w;
    pci->h = pci->ny * pci->pix_h;
    pci->fg_color = 0;
    pci->bg_color = 0;
    pci->flags = 0;
    pci->lru = pixmap_cache_clock;
    pci->id = pix->drawable.serialNumber;

    reducible_status = TILE_UNKNOWN;

    /* See if we can use a mono (color-expanded) pattern. */
    if (xf86AccelInfoRec.Subsequent8x8PatternColorExpand)
        /*
         * Check whether the tile can be reduced to 8x8 and uses only
         * two colors.
         */
        if ((reducible_status = ReduceTileToSize8(pci, pix, TRUE))
            & TILE_MONOCHROME) {
            /*
             * The width of the tile is 1, 2, 4, or 8.
             * The height is 1, 2, 4, or 8.
             * We know it only uses two colors.
	     *
	     * For a "mono" pattern, we'll be writing 64 rotated
	     * versions (unless we have special support as checked for
	     * below, and if there is a pattern origin offset, we'll
	     * only write one version).
	     */
	    if (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_PROGRAMMED_BITS) {
	        /*
	         * Easy, just put the 8x8 stipple data into two ints.
	         * No need to use the pixmap cache slot at all.
	         */
	        ConvertTileTo8x8MonoPattern(
                    pix->drawable.width, pix->drawable.height,
                    pix->devPrivate.ptr, pix->devKind,
	            &(pci->pattern0), &(pci->bg_color), &(pci->fg_color));
                /*
                 * and reverse when msb-first [rk] 
                 */ 
                if (xf86AccelInfoRec.Flags & 
                           HARDWARE_PATTERN_BIT_ORDER_MSBFIRST) {
                    int k;
                    for (k = 0; k < 8; k++)
                        ((unsigned char*)&pci->pattern0)[k] = 
                            byte_reversed[((unsigned char*)&pci->pattern0)[k]];
                }
	    }
	    else {
                WriteTileAs8x8MonoPattern(pci,
                    pix->drawable.width, pix->drawable.height,
                    pix->devPrivate.ptr, pix->devKind);
            }
            pci->flags = 2;
            return;
        }

    /* See if we can use any hardware pattern feature. */
    if (xf86AccelInfoRec.SubsequentFill8x8Pattern &&
    (!(xf86AccelInfoRec.Flags & HARDWARE_PATTERN_ALIGN_64)
    || (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_PROGRAMMED_ORIGIN)) &&
    /*
     * As long as cache slots are positioned at multiple-of-128 x-coords,
     * 64 pixel aligment requirement for 8x8 pattern can be guaranteed
     * for certain values of FramebufferWidth (such as 1024, 1280).
     * This only helps when the chip uses HARDWARE_PATTERN_MOD_64_OFFSET.
     */
    (!(xf86AccelInfoRec.Flags & HARDWARE_PATTERN_MOD_64_OFFSET)
    || (xf86AccelInfoRec.FramebufferWidth & 63) == 0)
    )
        /*
         * If the reduce function has been called before, then it must
         * have returned FALSE. So continue if it hasn't been
         * called yet, or it was called and found to be reducible.
         */
        if ((reducible_status & TILE_REDUCIBLE) ||
            ((reducible_status & TILE_UNKNOWN) &&
            (ReduceTileToSize8(pci, pix, FALSE) & TILE_REDUCIBLE))) {
            /*
             * The width of the tile is 1, 2, 4, or 8.
             * The height is 1, 2, 4, or 8.
             */
            pci->flags = 1;
	    if(xf86AccelInfoRec.Flags & HARDWARE_PATTERN_NOT_LINEAR)
		goto REGULAR_TILE;

            Write8x8Pattern(pci,
                pix->drawable.width, pix->drawable.height,
                pix->devPrivate.ptr, pix->devKind);
            return;
        }

REGULAR_TILE:

    xf86AccelInfoRec.ImageWrite(pci->x, pci->y, pci->pix_w, pci->pix_h,
         pix->devPrivate.ptr, pix->devKind, GXcopy, 0xFFFFFFFF);

    DoCacheExpandPixmap(pci);
    return;
#endif
}

/* Determine if stipple can be reduced to 8 wide/high. */

static Bool ReduceStippleToSize8(pci, pix)
    CacheInfoPtr pci;
    PixmapPtr pix;
{
    int w, h;
    int y;
    unsigned char *srcp;
    unsigned int bits[8];
    w = pci->pix_w;
    h = pci->pix_h;
    /* w and h must be power of 2. */
    if ((w & (w - 1)) || (h & (h - 1)))
        return FALSE;
    if (w <= 8 && h <= 8)
        return TRUE;
    /* w and h must be reducible to 8. */
    if ((w > 8 && w != 16 && w != 32) || (h > 8 && h != 16 && h != 32))
        return FALSE;
    srcp = pix->devPrivate.ptr;
    for (y = 0; y < min(8, h); y++) {
        switch ((w + 7) / 8) {
        case 1 : /* width 1, 2, 4 or 8 */
            bits[y] = *(unsigned char *)srcp;
            break;
        case 2 : /* width 16 */
            bits[y] = *(unsigned short *)srcp;
            if ((bits[y] & 0x00FF) != ((bits[y] & 0xFF00) >> 8))
                return FALSE;
            break;
        case 4 : /* width 32 */
            bits[y] = *(unsigned int *)srcp;
            if ((bits[y] & 0x000000FF) != ((bits[y] & 0x0000FF00) >> 8))
                return FALSE;
            if ((bits[y] & 0x000000FF) != ((bits[y] & 0x00FF0000) >> 16))
                return FALSE;
            if ((bits[y] & 0x000000FF) != ((bits[y] & 0xFF000000) >> 24))
                return FALSE;
        }
        srcp += pix->devKind;
    }
    for (y = 8; y < h; y++) {
        switch ((w + 7) / 8) {
        case 1 : /* width 1, 2, 4 or 8 */
            if (*(unsigned char *)srcp != bits[y & 7])
                return FALSE;
            break;
        case 2 : /* width 16 */
            if (*(unsigned short *)srcp != bits[y & 7])
                return FALSE;
            break;
        case 4 : /* width 32 */
            if (*(unsigned int *)srcp != bits[y & 7])
                return FALSE;
            break;
        }
        srcp += pix->devKind;
    }
    if (w > 8)
        pci->pix_w = 8;
    if (h > 8)
        pci->pix_h = 8;
    return TRUE;
}

static int DoCacheStipple(slot, pDrawable, pGC)
    int slot;
    DrawablePtr pDrawable;
    GCPtr pGC;
{
#ifdef PIXPRIV
    CacheInfoPtr pci, realpci;
    PixmapPtr pix, scratchpix;
    unsigned char *scratchpixptr;
    GCPtr scratchGC;
    BoxRec box;
    ScreenPtr pScreen;
    WindowPtr rootWin;
    RegionRec rgnDst;
    DDXPointRec ptSrc;
    int check_reducible;
    void (*WriteBitmapFunc)();
    pix = pGC->stipple;

    realpci = &xf86CacheInfo[slot];
    /*
     * Use a temporary copy of the cache info structure, because
     * we might not be able to cache the pattern after all.
     */
    pci = ALLOCATE_LOCAL(sizeof(CacheInfo));
    *pci = *realpci;
    pci->pix_w = pix->drawable.width;
    pci->pix_h = pix->drawable.height;
    pci->nx = pci->cache_width / pix->drawable.width;
    pci->ny = pci->cache_height / pix->drawable.height;
    pci->w = pci->nx * pci->pix_w;
    pci->h = pci->ny * pci->pix_h;
    pci->fg_color = pGC->fgPixel;
    if (pGC->fillStyle == FillStippled)
        /*
         * Make sure the transparency color is different (but
         * avoid two's complement representation of -1L).
         */
        pci->bg_color = pci->fg_color ^ 0xFF;
    else
        pci->bg_color = pGC->bgPixel;
    pci->flags = 0;
    pci->lru = pixmap_cache_clock;
    pci->id = pix->drawable.serialNumber;

    /* See if we can use any hardware pattern feature. */
    check_reducible = TRUE;
    /* See if we can use a mono (color-expanded) pattern. */
    if (xf86AccelInfoRec.Subsequent8x8PatternColorExpand
    && ((xf86AccelInfoRec.Flags & HARDWARE_PATTERN_MONO_TRANSPARENCY)
    || pGC->fillStyle == FillOpaqueStippled))
        if (check_reducible = ReduceStippleToSize8(pci, pix)) {
            /*
             * The width of the tile is 1, 2, 4, or 8.
             * The height is 1, 2, 4, or 8.
	     *
	     * For a "mono" pattern, we'll be writing 64 rotated
	     * versions (unless we have special support as checked for
	     * below).
	     */
	    if (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_PROGRAMMED_BITS) {
	        /*
	         * Easy, just put the 8x8 stipple data into two ints.
	         * No need to use the pixmap cache slot at all.
	         */
	        ExpandStippleTo8x8MonoPattern(pix->drawable.width,
	            pix->drawable.height, pix->devPrivate.ptr,
	            pix->devKind, &(pci->pattern0));
	        /* 
	         * and reverse when msb-first [rk] 
	         */ 
	        if (xf86AccelInfoRec.Flags & 
	                   HARDWARE_PATTERN_BIT_ORDER_MSBFIRST) {
	            int k;
	            for (k = 0; k < 8; k++)
	                ((unsigned char*)&pci->pattern0)[k] = 
	                    byte_reversed[((unsigned char*)&pci->pattern0)[k]];
	        }
	    }
	    else {
#if 0
	        ErrorF("Writing rotated mono patterns (%dx%d).\n",
	            pci->pix_w, pci->pix_h);
#endif
                WriteStippleAs8x8MonoPattern(pci->x, pci->y, pix->drawable.width,
                    pix->drawable.height, pix->devPrivate.ptr, pix->devKind);
#if 0
                ErrorF("Finished writing mono patterns.\n");
#endif
            }
            pci->flags = 2;
            *realpci = *pci;
            DEALLOCATE_LOCAL(pci);
            return 1;
        }

    if (xf86AccelInfoRec.SubsequentFill8x8Pattern &&
    (!(xf86AccelInfoRec.Flags & HARDWARE_PATTERN_ALIGN_64)
    || (xf86AccelInfoRec.Flags & HARDWARE_PATTERN_PROGRAMMED_ORIGIN)) &&
    (!(xf86AccelInfoRec.Flags & HARDWARE_PATTERN_MOD_64_OFFSET)
    || (xf86AccelInfoRec.FramebufferWidth & 63) == 0) &&
    ((xf86AccelInfoRec.Flags & HARDWARE_PATTERN_TRANSPARENCY)
    || pGC->fillStyle == FillOpaqueStippled) &&
    xf86AccelInfoRec.BitsPerPixel != 24)
        /*
         * Stipples are often 32 pixels wide. However, we are only
         * able to use the hardware pattern if the 8 pixels are
         * repeated four times horizontally. More sneakily, if the
         * stipple is 16 or 32 pixels high, we can check whether
         * it repeats 8 lines vertically. This is called benchmark
         * rigging...if a certain X server that shall not be named here
         * does it, why shouldn't we.
         */
        if (check_reducible && ReduceStippleToSize8(pci, pix)) {
            /*
             * The width of the stipple is 1, 2, 4, or 8.
             * The height is 1, 2, 4, or 8.
             *
             * We write the stipple to a full-depth scratch pixmap now
             * using CopyPlane1ToN, and then call Write8x8Pattern.
             */
            pci->flags = 1;
	    if(xf86AccelInfoRec.Flags & HARDWARE_PATTERN_NOT_LINEAR)
		goto REGULAR_STIPPLE;

	    pScreen = screenInfo.screens[xf86ScreenIndex];
	    rootWin = WindowTable[pScreen->myNum];
	    scratchpixptr = (unsigned char *)ALLOCATE_LOCAL(
		PixmapBytePad(pix->drawable.width,
	        rootWin->drawable.depth) * pix->drawable.height);
            scratchpix = GetScratchPixmapHeader(
                pScreen, pix->drawable.width, pix->drawable.height,
                rootWin->drawable.depth, rootWin->drawable.bitsPerPixel,
                PixmapBytePad(pix->drawable.width, rootWin->drawable.depth),
                scratchpixptr);
	    ptSrc.x = 0;
	    ptSrc.y = 0;
	    box.x1 = 0;
	    box.y1 = 0;
	    box.x2 = pix->drawable.width;
	    box.y2 = pix->drawable.height;
	    REGION_INIT(pScreen, &rgnDst, &box, 1);
            xf86CopyPlane1toN((DrawablePtr)pix, (DrawablePtr)scratchpix,
                GXcopy, &rgnDst, &ptSrc, 0xFFFFFFFF, 1,
                pci->bg_color, pci->fg_color);
            REGION_UNINIT(pScreen, &rgnDst);
            Write8x8Pattern(pci,
                pix->drawable.width, pix->drawable.height,
                scratchpix->devPrivate.ptr, scratchpix->devKind);
            DEALLOCATE_LOCAL(scratchpixptr);
            FreeScratchPixmapHeader(scratchpix);
            *realpci = *pci;
            DEALLOCATE_LOCAL(pci);
            return 1;
        }

REGULAR_STIPPLE:

    if (pGC->fillStyle == FillStippled &&
    ((xf86GCInfoRec.CopyAreaFlags & NO_TRANSPARENCY)
    || ((xf86GCInfoRec.CopyAreaFlags & TRANSPARENCY_GXCOPY)
    && pGC->alu != GXcopy))) {
        /*
         * We were hoping to use the hardware pattern, but that's
         * not possible. ScreenToScreenCopy doesn't support transparency
         * with the current raster-operation, so we can't cache the
         * stipple at all.
         */
        DEALLOCATE_LOCAL(pci);
        return 0;
    }
    
    *realpci = *pci;
    DEALLOCATE_LOCAL(pci);
    pci = realpci;

    WriteBitmapFunc = NULL;
    if (xf86AccelInfoRec.WriteBitmap)
        WriteBitmapFunc = xf86AccelInfoRec.WriteBitmap;
    else
        if (xf86AccelInfoRec.WriteBitmapFallBack)
            WriteBitmapFunc = xf86AccelInfoRec.WriteBitmapFallBack;
    
    if (WriteBitmapFunc)
        (*WriteBitmapFunc)(pci->x, pci->y, pci->pix_w,
            pci->pix_h, pix->devPrivate.ptr,
            pix->devKind, 0, 0, pci->bg_color, pci->fg_color, GXcopy, ~0);

    DoCacheExpandPixmap(pci);
    return 1;
#endif
}

static void DoCacheExpandPixmap(pci)
    CacheInfoPtr pci;
{
    int cur_w = pci->pix_w;
    int cur_h = pci->pix_h;

    xf86AccelInfoRec.SetupForScreenToScreenCopy(1, 1, GXcopy, 0xFFFFFFFF, -1);

    /* Expand in the x direction */
    while (cur_w * 2 <= pci->w) 
    {
        xf86AccelInfoRec.SubsequentScreenToScreenCopy(pci->x, pci->y,
            pci->x + cur_w, pci->y, cur_w, cur_h);
	cur_w *= 2;
    }

    if (cur_w != pci->w) 
    {
        xf86AccelInfoRec.SubsequentScreenToScreenCopy((pci->x), (pci->y),
            (pci->x + cur_w), pci->y, (pci->w - cur_w), cur_h);
	cur_w = pci->w;
    }

    /* Expand in the y direction */
    while (cur_h * 2 <= pci->h) 
    {
        xf86AccelInfoRec.SubsequentScreenToScreenCopy((pci->x), (pci->y),
            (pci->x), (pci->y + cur_h), cur_w, cur_h);
	cur_h *= 2;
    }

    if (cur_h != pci->h) 
    {
        xf86AccelInfoRec.SubsequentScreenToScreenCopy((pci->x), (pci->y),
            (pci->x), (pci->y + cur_h), cur_w, (pci->h - cur_h));
    }

    if (xf86AccelInfoRec.Flags & BACKGROUND_OPERATIONS)
        xf86AccelInfoRec.Sync();
}

static Bool
xf86CachableTile(pix)
    PixmapPtr pix;
{
   return (pix->drawable.width <= MaxWidth &&
       pix->drawable.height <= MaxHeight);
}

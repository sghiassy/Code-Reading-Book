/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64pcach.c,v 3.14 1997/01/05 11:53:44 dawes Exp $ */
/*
 * Copyright 1992,1993,1994 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Kevin E. Martin makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN OR RICKARD E. FAITH BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Dynamic cache allocation added by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)
 *
 */
/* $XConsortium: mach64pcach.c /main/11 1996/12/28 14:44:13 kaleb $ */


/*       Offscreen memory organization for one 256-line cache set:
 *
 *       128      128                         768
 *     +-------+-------+-----------------------------------------------+
 *     |       |       |                                               |
 * 128 |   0   |   1   |                                               |
 *     |       |       |                                               |
 *     +---+---+---+---+                   8 FONTS                     | 256
 *  64 | 2 | 3 | 4 | 5 |                                               |
 *     +---+---+---+---+                                               |
 *  64 | 6 | 7 | 8 | 9 |                                               |
 *     +---+---+---+---+-----------------------------------------------+
 *
 *
 *
 *       Offscreen memory organization for two 256-line cache sets:
 *
 *            256                             768
 *     +---------------+-----------------------------------------------+
 *     |               |                                               |
 *     |               |                                               |
 *     |               |                                               |
 * 256 |       0       |                                               |
 *     |               |                                               |
 *     |               |                                               |
 *     |               |                                               |
 *     +-------+-------+                  16 FONTS                     | 512
 *     |       |       |                                               |
 * 128 |   1   |   2   |                                               |
 *     |       |       |                                               |
 *     +---+---+---+---+                                               |
 *  64 | 3 | 4 | 5 | 6 |                                               |
 *     +---+---+---+---+                                               |
 *  64 | 7 | 8 | 9 | A |                                               |
 *     +---+---+---+---+-----------------------------------------------+
 *
 *
 *
 *       Offscreen memory organization for three 256-line cache sets:
 *
 *            256                            768
 *     +---------------+-----------------------------------------------+
 *     |               |                                               |
 *     |               |                                               |
 *     |               |                                               |
 * 256 |       0       |                                               |     
 *     |               |                                               |
 *     |               |                                               |
 *     |               |                                               |
 *     +---------------+                  16 FONTS                     | 512
 *     |               |                                               |
 *     |               |                                               |
 *     |               |                                               |
 * 256 |       1       |                                               |    
 *     |               |                                               |
 *     |               |                                               |
 *     |               |                                               |
 *     +-------+-------+-------+-------+-------+-------+-------+-------+
 *     |       |       |       |       |       |       |       |       |
 * 128 |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |
 *     |       |       |       |       |       |       |       |       |
 *     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+    
 *  64 | A | B | C | D | E | F | 10| 11| 12| 13| 14| 15| 16| 17| 18| 19|
 *     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *  64 | 1A| 1B| 1C| 1D| 1E| 1F| 20| 21| 22| 23| 24| 25| 26| 27| 28| 29|
 *     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
 *
 */

#include "mach64im.h"
#include "pixmapstr.h"
#include "mach64.h"

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

extern int xf86Verbose;

unsigned char reverseByteOrder[0x100] = {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
        0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

CacheInfoPtr mach64CacheInfo = NULL;
static int CacheIDNum = 1;

static int MaxSlots;
static int First256Slot;
static int First128Slot;
static int First64Slot;

static unsigned int pixmap_cache_clock = 1;

static void DoCacheExpandPixmap();
static void DoCacheTile();
static void DoCacheOpStipple();

void
mach64CacheInit(w, h)
    int w;
    int h;
{
    int i;
    int free_ram = mach64InfoRec.videoRam * 1024
                   - w * h * (mach64InfoRec.bitsPerPixel / 8); 
    int lines    = free_ram / (w * (mach64InfoRec.bitsPerPixel / 8));
    int cache_sets;
    static Bool first = TRUE;


    if (OFLG_ISSET(OPTION_NO_PIXMAP_CACHE, &mach64InfoRec.options)) {
	if (first) {
	    ErrorF("%s %s: Pixmap cache disabled\n", XCONFIG_GIVEN,
	           mach64InfoRec.name);
	    First256Slot = -1;
	    First128Slot = -1;
	    First64Slot  = -1;
	    MaxSlots     = -1;
	    first = FALSE;
	}
	return;
    }

    if (lines < 0 || w < 1024)
	  lines = 0;

				/* If cache_sets is changed, remember
				 * to change mach64MaxY in mach64c.im.
				 */
    if (!mach64CacheInfo) {
       cache_sets = lines / 256;
       switch (cache_sets) {
       case 0:
	  First256Slot = -1;
	  First128Slot = -1;
	  First64Slot  = -1;
	  MaxSlots     = -1;
	  break;
       case 1:
	  First256Slot = -1;
	  First128Slot =  1;
	  First64Slot  =  First128Slot  +  2;
	  MaxSlots     =  First64Slot   +  8;
	  break;
       case 2:
	  First256Slot =  1;
	  First128Slot =  First256Slot  +  1;
	  First64Slot  =  First128Slot  +  2;
	  MaxSlots     =  First64Slot   +  8;
	  break;
       case 3:
       default:
	  First256Slot = 1;
	  First128Slot = First256Slot  +  2;
	  First64Slot  = First128Slot  +  8;
	  MaxSlots     = First64Slot   + 32;
	  break;
       }

       if (cache_sets > 0)
          mach64CacheInfo = (CacheInfoPtr)xcalloc(MaxSlots, sizeof(CacheInfo));

       switch (cache_sets) {
       case 0:
	  break;
       case 1:
				/* No 256x256 pixel cache */
	  for (i = First128Slot; i < First64Slot; i++) {
	     mach64CacheInfo[i].cache_size = 128;
	     mach64CacheInfo[i].x = ((i - First128Slot) & 1) * 128;
	     mach64CacheInfo[i].y = h 
		   + 128 * ((i - First128Slot) >> 1);
	  }
	  
	  for (i = First64Slot; i < MaxSlots; i++) {
	     mach64CacheInfo[i].cache_size = 64;
	     mach64CacheInfo[i].x = ((i - First64Slot) & 3) * 64;
	     mach64CacheInfo[i].y = h 
		   + 128 * ((First64Slot - 1) >> 1)
		   + 64 * ((i - First64Slot) >> 2);
	  }
	  break;
       case 2:
	  for (i = First256Slot; i < First128Slot; i++) {
	     mach64CacheInfo[i].cache_size = 256;
	     mach64CacheInfo[i].x = 0;
	     mach64CacheInfo[i].y = h
		   + 256 * (i - First256Slot);
	  }
	  
	  for (i = First128Slot; i < First64Slot; i++) {
	     mach64CacheInfo[i].cache_size = 128;
	     mach64CacheInfo[i].x = ((i - First128Slot) & 1) * 128;
	     mach64CacheInfo[i].y = h
		   + 256 * (First128Slot - 1)
		   + 128 * ((i - First128Slot) >> 1);
	  }
	  
	  for (i = First64Slot; i < MaxSlots; i++) {
	     mach64CacheInfo[i].cache_size = 64;
	     mach64CacheInfo[i].x = ((i - First64Slot) & 3) * 64;
	     mach64CacheInfo[i].y = h +
		   256 * (First128Slot - 1)
		   + 128 * ((First64Slot - First128Slot) >> 1)
		   + 64 * ((i - First64Slot) >> 2);
	  }
	  break;
      case 3:
      default:
	  for (i = First256Slot; i < First128Slot; i++) {
	     mach64CacheInfo[i].cache_size = 256;
	     mach64CacheInfo[i].x = 0;
	     mach64CacheInfo[i].y = h
		   + 256 * (i - First256Slot);
	  }
	  
	  for (i = First128Slot; i < First64Slot; i++) {
	     mach64CacheInfo[i].cache_size = 128;
	     mach64CacheInfo[i].x = ((i - First128Slot) & 7) * 128;
	     mach64CacheInfo[i].y = h
		   + 256 * (First128Slot - 1)
		   + 128 * ((i - First128Slot) >> 3);
	  }
	  
	  for (i = First64Slot; i < MaxSlots; i++) {
	     mach64CacheInfo[i].cache_size = 64;
	     mach64CacheInfo[i].x = ((i - First64Slot) & 15) * 64;
	     mach64CacheInfo[i].y = h +
		   256 * (First128Slot - 1)
		   + 128 * ((First64Slot - First128Slot) >> 3)
		   + 64 * ((i - First64Slot) >> 4);
	  }
	  break;
       }
       
       if (xf86Verbose) {
	  ErrorF("%s %s: Pixmap cache: %d 256x256 slots, %d 128x128 slots, %d 64x64 slots\n",
		  XCONFIG_PROBED, mach64InfoRec.name,
		  First256Slot == -1 ? 0 : First128Slot - First256Slot,
		  First64Slot - First128Slot,
		  MaxSlots - First64Slot );
       }
    }
    
    for (i = 0; i < MaxSlots; i++) {
       mach64CacheInfo[i].id = -1;
       mach64CacheInfo[i].lru = pixmap_cache_clock;
       mach64CacheInfo[i].fg_color = 0;
       mach64CacheInfo[i].bg_color = 0;
       mach64CacheInfo[i].flags = 0;
       mach64CacheInfo[i].pattern0 = 0;
       mach64CacheInfo[i].pattern1 = 0;
    }
}

void
mach64CacheFreeSlot(pix)
    PixmapPtr pix;
{
#ifdef PIXPRIV
    mach64PixPrivPtr devPriv;

    /* This is not really necessary but for completeness... */
    /* Anyway, the pixmap cache is reinitialized upon VTEnter */
    if (!xf86VTSema)
	return;

    devPriv = (mach64PixPrivPtr)(pix->devPrivates[mach64PixmapIndex].ptr);

    if (devPriv->slot > 0 && devPriv->slot <= MaxSlots &&
	mach64CacheInfo[devPriv->slot].id == pix->drawable.serialNumber) {
	mach64CacheInfo[devPriv->slot].id = 0;
	devPriv->slot = 0;
    }
#endif
}

/*
 * FindCacheSlot() returns a cache slot number if the pixmap is cachable.
 */
static int
FindCacheSlot(pix)
    PixmapPtr pix;
{
    int i, next = (First256Slot != -1) ? First256Slot : First128Slot;

#ifdef PIXMAP_CACHE_OFF
    return 0;
#endif
#ifndef PIXPRIV
    return 0;
#endif

    if (MaxSlots == -1) {
	return 0;
    } else if (First256Slot == -1) {
       if (pix->drawable.width > 128 || pix->drawable.height > 128)
	     return 0;
    } else {
       if (pix->drawable.width > 256 || pix->drawable.height > 256)
	     return 0;
    }
				/* The pixmap is of cachable size, so we
				 * want to cache it.  If there are available
				 * cache slots, we use the largest, since this
				 * optimizes tiling.  If, however, there are
				 * no available cache slots, we use the least
				 * recently used, *regardless* of size (except,
				 * of course, that the pixmap has to fit into
				 * the cache slot.  Rik Faith, 22July93. */

    if (pix->drawable.width <= 64 && pix->drawable.height <= 64) {
        next = MaxSlots - 1;	/* Last 64Slot */
	for (i = First64Slot; i < MaxSlots; i++)
	    if (mach64CacheInfo[i].id == -1) {
		return (i);
	    } else if (mach64CacheInfo[i].lru < mach64CacheInfo[next].lru)
		next = i;
    } else {
        next = First64Slot - 1;	/* Last 128Slot */
    }

    if (pix->drawable.width <= 128 && pix->drawable.height <= 128) {
	for (i = First128Slot; i < First64Slot; i++)
	    if (mach64CacheInfo[i].id == -1) {
		return (i);
	    } else if (mach64CacheInfo[i].lru < mach64CacheInfo[next].lru)
		next = i;
    } else if (First256Slot != -1) {
        next = First128Slot - 1; /* Last 256Slot */
    }

    if (First256Slot != -1
	&& pix->drawable.width <= 256 && pix->drawable.height <= 256) {
       
       for (i = First256Slot; i < First128Slot; i++)
	     if (mach64CacheInfo[i].id == -1) {
		return (i);
	     } else if (mach64CacheInfo[i].lru < mach64CacheInfo[next].lru)
		   next = i;
    } 

#ifdef DEBUG_LRU
    for (i = 0; i < MaxSlots; i++)
	  ErrorF( "lru[%d]=%d, ", i, mach64CacheInfo[i].lru );
    ErrorF( "Using %d for %dx%d\n", next, pix->drawable.width,
	    pix->drawable.height );
#endif
    
    return (next);
}

void
mach64IncrementCacheLRU(slot)
    int slot;
{
    int i;

    if (!(mach64CacheInfo[slot].lru = ++pixmap_cache_clock))
    {
	for (i = 0; i < MaxSlots; i++)
	    mach64CacheInfo[i].lru = pixmap_cache_clock;
    }
}

static void
DoCacheExpandPixmap(pci)
    CacheInfoPtr pci;
{
    int cur_w = pci->pix_w;
    int cur_h = pci->pix_h;

    WaitQueue(3);
    regw(DP_MIX, (MIX_SRC << 16) | MIX_SRC);
    regw(DP_WRITE_MASK, 0xffffffff);
    regw(DP_SRC, FRGD_SRC_BLIT);

    /* Expand in the x direction */
    while (cur_w * 2 <= pci->w) 
    {
	/* This extra wait had to be added to work around a timing
	 * problem I encountered with certain [CVG]T chip sets.  This
	 * only happens when you are repeatedly blt'ing an image
	 * smaller than 16x1 or 12x2 from one area to another.
	 */
	if (mach64IntegratedController) WaitIdleEmpty();
        MACH64_BIT_BLT((pci->x), (pci->y), (pci->x + cur_w), pci->y, 
			cur_w, cur_h,
		       (DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM));
	cur_w *= 2;
    }

    if (cur_w != pci->w) 
    {
        MACH64_BIT_BLT((pci->x), (pci->y), (pci->x + cur_w), pci->y, 
			(pci->w - cur_w), cur_h,
		       (DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM));
	cur_w = pci->w;
    }

    /* Expand in the y direction */
    while (cur_h * 2 <= pci->h) 
    {
        MACH64_BIT_BLT((pci->x), (pci->y), (pci->x), (pci->y + cur_h), 
			cur_w, cur_h,
		       (DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM));
	cur_h *= 2;
    }

    if (cur_h != pci->h) 
    {
        MACH64_BIT_BLT((pci->x), (pci->y), (pci->x), (pci->y + cur_h), 
			cur_w, (pci->h - cur_h),
		       (DST_X_LEFT_TO_RIGHT | DST_Y_TOP_TO_BOTTOM));
    }

    WaitIdleEmpty(); /* Make sure that all commands have finished */
}

static void
DoCacheTile(pix)
    PixmapPtr pix;
{
#ifdef PIXPRIV
    mach64PixPrivPtr devPriv = 
	(mach64PixPrivPtr)(pix->devPrivates[mach64PixmapIndex].ptr);

    CacheInfoPtr pci = &mach64CacheInfo[devPriv->slot];
    int fixed_pattern = 0;
    unsigned char pattern[8];

    pci->pix_w = pix->drawable.width;
    pci->pix_h = pix->drawable.height;
    pci->nx = pci->cache_size/pix->drawable.width;
    pci->ny = pci->cache_size/pix->drawable.height;
    pci->w = pci->nx * pci->pix_w;
    pci->h = pci->ny * pci->pix_h;
    pci->fg_color = 0;
    pci->bg_color = 0;
    pci->flags = 0;
    pci->lru = pixmap_cache_clock;
    pci->id = pix->drawable.serialNumber;

    /* see if we can use the fixed pattern registers */
    if ((mach64InfoRec.bitsPerPixel == 8) && (pci->pix_w == 4) &&
	(pci->pix_h <= 4) && !(pci->pix_h & (pci->pix_h-1)))
    {
	int i, j;
	int fg, bg;
	int bg_defined = 0;
	unsigned char *psrc = (unsigned char *)pix->devPrivate.ptr;

	for (i=0; i<8; i++)
	    pattern[i] = 0;

	fg = psrc[0];
	fixed_pattern = 1;
	for (i=0; i<pci->pix_h; i++)
	{
	    for (j=0; j<pci->pix_w; j++)
	    {
		if (psrc[j] == fg)
		{
		    pattern[i] |= (0x11 << j);
		}
		else
		{
		    if (!bg_defined)
		    {
			bg = psrc[j];
			bg_defined = 1;
		    }
		    if (psrc[j] != bg)
		    {
			fixed_pattern = 0;
			break;
		    }
		}
	    }
	    psrc += pix->devKind;
	}
        if (fixed_pattern)
        {
	    pci->flags = 1;
	    pci->fg_color = fg;
	    pci->bg_color = bg;

	    for (i=pci->pix_h; i<8; i++)
	    {
		pattern[i] = pattern[i % pci->pix_h];
	    }
	    pci->pattern0 = *((int *)&pattern[0]);
	    pci->pattern1 = *((int *)&pattern[4]);

	    return;
        }
    }

    (mach64ImageWriteFunc)(pci->x, pci->y, pci->pix_w, pci->pix_h,
		           pix->devPrivate.ptr, pix->devKind, 0, 0,
		           MIX_SRC, 0xffffffff);

    DoCacheExpandPixmap(pci);

    WaitIdleEmpty(); /* Make sure that all commands have finished */
#endif
}

static void
DoCacheOpStipple(pix, fg, bg)
    PixmapPtr pix;
    int fg;
    int bg;
{
#ifdef PIXPRIV
    mach64PixPrivPtr devPriv = 
	(mach64PixPrivPtr)(pix->devPrivates[mach64PixmapIndex].ptr);

    CacheInfoPtr pci = &mach64CacheInfo[devPriv->slot];
    int fixed_pattern = 0;

    pci->pix_w = pix->drawable.width;
    pci->pix_h = pix->drawable.height;
    pci->nx = pci->cache_size/pix->drawable.width;
    pci->ny = pci->cache_size/pix->drawable.height;
    pci->w = pci->nx * pci->pix_w;
    pci->h = pci->ny * pci->pix_h;
    pci->fg_color = fg;
    pci->bg_color = bg;
    pci->flags = 0;
    pci->lru = pixmap_cache_clock;
    pci->id = pix->drawable.serialNumber;

    /* see if we can use the 8x8 mono pattern registers */
    if ((mach64InfoRec.bitsPerPixel == 8) && (pci->pix_w == 32) &&
	(pci->pix_h <= 32) && !(pci->pix_h & (pci->pix_h-1)))
    {
	int *psrc = (int *)pix->devPrivate.ptr;
	int i, j;

	fixed_pattern = 1;
	for (i=0; i<pci->pix_h; i++)
	{
	    if (((psrc[i] & 0x000000ff) != ((psrc[i] >> 8) & 0x000000ff)) ||
		((psrc[i] & 0x000000ff) != ((psrc[i] >> 16) & 0x000000ff)) ||
		((psrc[i] & 0x000000ff) != ((psrc[i] >> 24) & 0x000000ff)))
	    {
		fixed_pattern = 0;
		break;
	    }
	}
	if (pci->pix_h > 8) /* can only be 16 or 32 */
	{
	    for (i=0; i<(pci->pix_h >> 1); i++)
	    {
		if (psrc[i] != psrc[i+(pci->pix_h >> 1)])
		{
		    fixed_pattern = 0;
		    break;
		}
	    }
	}
        if (fixed_pattern)
        {
	    pci->flags = 1;
	    pci->pattern0 = 0;
	    pci->pattern1 = 0;
	    for (i=0, j=0; i<4; i++, j+=(pix->devKind >> 2))
	    {
	        pci->pattern0 |= 
			(psrc[j % pci->pix_h] & 0x000000ff) << (i * 8);
	        pci->pattern1 |= 
			(psrc[(j+4) % pci->pix_h] & 0x000000ff) << (i * 8);
	    }
	    return;
        }
    }

    mach64ImageStippleFunc(pci->x, pci->y, pci->pix_w, pci->pix_h,
			   pix->devPrivate.ptr, pix->devKind, 0, 0,
			   fg, bg, MIX_SRC, 0xffffffff, 1);

    DoCacheExpandPixmap(pci);

    WaitIdleEmpty(); /* Make sure that all commands have finished */
#endif
}

int
mach64CacheTile(pix)
    PixmapPtr pix;
{
#ifdef PIXPRIV
    int slot;
    mach64PixPrivPtr devPriv = 
	(mach64PixPrivPtr)(pix->devPrivates[mach64PixmapIndex].ptr);

    /* First see if tile is already cached */
    if ((devPriv->slot > 0 && devPriv->slot <= MaxSlots) &&
	(mach64CacheInfo[devPriv->slot].id == pix->drawable.serialNumber)) 
    {
	return (1);
    } 
    else if (mach64CachableTile(pix))
    {
        slot = FindCacheSlot(pix);

        if (slot) 
	{
	    devPriv->slot = slot;
	    DoCacheTile(pix);
	    return (1);
        } 
    }
#endif
    return (0);
}

int
mach64CacheStipple(pix, fg)
    PixmapPtr pix;
    int fg;
{
#ifdef PIXPRIV
    int slot;
    mach64PixPrivPtr devPriv = 
	(mach64PixPrivPtr)(pix->devPrivates[mach64PixmapIndex].ptr);

    /* First see if stipple is already cached */
    if ((devPriv->slot > 0 && devPriv->slot <= MaxSlots) &&
	(mach64CacheInfo[devPriv->slot].id == pix->drawable.serialNumber) &&
	(mach64CacheInfo[devPriv->slot].fg_color == fg)) 
    {
	return (1);
    } 
    else if (mach64CachableStipple(pix))
    {
        slot = FindCacheSlot(pix);

        if (slot) 
	{
	    devPriv->slot = slot;
	    DoCacheOpStipple(pix, fg, (fg ? 0 : 0xffffffff));
	    return (1);
        }
    }
#endif
    return (0);
}

int
mach64CacheOpStipple(pix, fg, bg)
    PixmapPtr pix;
    int fg;
    int bg;
{
#ifdef PIXPRIV
    int slot;
    mach64PixPrivPtr devPriv = 
	(mach64PixPrivPtr)(pix->devPrivates[mach64PixmapIndex].ptr);

    /* First see if stipple is already cached */
    if ((devPriv->slot > 0 && devPriv->slot <= MaxSlots) &&
	(mach64CacheInfo[devPriv->slot].id == pix->drawable.serialNumber) &&
	(mach64CacheInfo[devPriv->slot].fg_color == fg) &&
	(mach64CacheInfo[devPriv->slot].bg_color == bg)) 
    {
	return (1);
    } 
    else if (mach64CachableOpStipple(pix))
    {
        slot = FindCacheSlot(pix);

        if (slot) 
	{
	    devPriv->slot = slot;
	    DoCacheOpStipple(pix, fg, bg);
	    return (1);
        }
    }
#endif
    return (0);
}

Bool
mach64CachableTile(pix)
    PixmapPtr pix;
{
   if (First256Slot != -1)
	 return (pix->drawable.width <= 256 && pix->drawable.height <= 256);
   return (pix->drawable.width <= 128 && pix->drawable.height <= 128);
}

Bool
mach64CachableStipple(pix)
    PixmapPtr pix;
{
   return (pix->drawable.width <= 128 && pix->drawable.height <= 128);
}

Bool
mach64CachableOpStipple(pix)
    PixmapPtr pix;
{
   return (pix->drawable.width <= 128 && pix->drawable.height <= 128);
}

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64im.h,v 3.5 1996/12/23 06:39:19 dawes Exp $ */
/*
 * Copyright 1992,1993,1994 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Kevin E. Martin not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Kevin E. Martin
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: mach64im.h /main/3 1996/02/21 17:28:46 kaleb $ */

#ifndef MACH64IM_H
#define MACH64IM_H

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
    int cache_size;     /* size of cache slot 64, 128, or 256 */
    int flags;          /* non zero if fixed pattern */
    int pattern0;       /* if fixed pattern, pattern 0 register contents */
    int pattern1;       /* if fixed pattern, pattern 1 register contents */
    unsigned int lru;   /* lru counter */
} CacheInfo, *CacheInfoPtr;

#ifdef PIXPRIV
typedef struct _mach64PixPriv {
    int slot;		/* Offscreen cache slot number */
} mach64PixPrivRec, *mach64PixPrivPtr;
#endif

#endif

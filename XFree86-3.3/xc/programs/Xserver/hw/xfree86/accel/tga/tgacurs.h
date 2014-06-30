/* $XConsortium: tgacurs.h /main/3 1996/10/27 11:47:39 kaleb $ */




/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/tga/tgacurs.h,v 3.2 1996/12/27 07:03:49 dawes Exp $ */

/*
 * Copyright 1995,96 by Alan Hourihane, Wigan, England.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Alan Hourihane not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Alan Hourihane makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALAN HOURIHANE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ALAN HOURIHANE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Alan Hourihane, <alanh@fairlite.demon.co.uk>
 */

extern Bool tgaBlockCursor;
extern Bool tgaReloadCursor;

#define BLOCK_CURSOR    tgaBlockCursor = TRUE;

#define UNBLOCK_CURSOR  { mem_barrier(); \
			    if (tgaReloadCursor) \
			       tgaRestoreCursor(savepScreen); \
			    tgaBlockCursor = FALSE; \
                        }

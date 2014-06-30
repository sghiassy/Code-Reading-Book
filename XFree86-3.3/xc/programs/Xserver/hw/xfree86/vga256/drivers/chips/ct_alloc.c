/* $XConsortium: ct_alloc.c /main/2 1996/10/25 10:29:02 kaleb $ */
/*
 * 
 * Copyright 1993 by H. Hanemaayer, Utrecht, The Netherlands
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of H. Hanemaayer not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  H. Hanemaayer makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * H. HANEMAAYER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL H. HANEMAAYER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  H. Hanemaayer, <hhanemaa@cs.ruu.nl>
 * Modified: Simon P. Cooper <scooper@vizlab.rutgers.edu>
 * Modified: David Bateman <dbateman@ee.uts.edu.au> for use with CHIPS
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_alloc.c,v 3.2 1996/12/27 07:05:05 dawes Exp $ */

/*
 * This file implements functions a simple allocator of video memory for
 * scratch space, patterns etc.
 *
 * The allocator can currently only free the last request. More advanced
 * functionality can be added when it is actually required.
 */

#include "vga256.h"
#include "cfbrrop.h"
#include "mergerop.h"
#include "vgaBank.h"
#include "xf86.h"
#include "vga.h"		       /* For vga256InfoRec */
#include "compiler.h"
#include "ct_driver.h"

static int allocatebase;	       /* Video memory address of allocation space. */
static int freespace;		       /* Number of bytes left for allocation. */
static int currentaddr;		       /* Video address of first available space. */
static int lastaddr;		       /* Video address of last allocation. */
static int lastsize;		       /* Size of the last request. */

/*
 * Initialize the allocator. This is called once at server start-up.
 */

int
ctInitializeAllocator()
{
    /* We want to align on a multiple of 8 bytes, should be OK. 
     * Allocate from the top of ram. */

    allocatebase = (vga256InfoRec.videoRam<<10) - ctFrameBufferSize;
    freespace = allocatebase - vga256InfoRec.displayWidth *
	vga256InfoRec.virtualY * vgaBytesPerPixel;
    currentaddr = allocatebase;
    lastaddr = -1;
    return freespace;
}

/*
 * Allocate a number of bytes of video memory.
 */

int
ctAllocate(size, align)
    int size;
    unsigned int align;
{
    /* Align at required boundary */
    size = currentaddr - ((currentaddr - size) & ~align);

    if (size > freespace)
	return -1;
    lastaddr = currentaddr;
    lastsize = size;
    freespace -= size;
    currentaddr -= size;
    return currentaddr;
}

/*
 * Free allocated space. Currently needs to be the last allocated one.
 */

void
ctFree(vidaddr)
    int vidaddr;
{
    if (vidaddr != lastaddr)	       /* If not allocated right before, */
	return;			       /* do nothing. */
    /* Undo the last allocation. */
    currentaddr += lastsize;
    freespace += lastsize;
    lastaddr = -2;
}

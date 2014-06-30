/*
 * Copyright 1997
 * Digital Equipment Corporation. All rights reserved.
 * This software is furnished under license and may be used and copied only in 
 * accordance with the following terms and conditions.  Subject to these 
 * conditions, you may download, copy, install, use, modify and distribute 
 * this software in source and/or binary form. No title or ownership is 
 * transferred hereby.
 * 1) Any source code used, modified or distributed must reproduce and retain 
 *    this copyright notice and list of conditions as they appear in the 
 *    source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of Digital 
 *    Equipment Corporation. Neither the "Digital Equipment Corporation" name 
 *    nor any trademark or logo of Digital Equipment Corporation may be used 
 *    to endorse or promote products derived from this software without the 
 *    prior written permission of Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied warranties,
 *    including but not limited to, any implied warranties of merchantability,
 *    fitness for a particular purpose, or non-infringement are disclaimed. In
 *    no event shall DIGITAL be liable for any damages whatsoever, and in 
 *    particular, DIGITAL shall not be liable for special, indirect, 
 *    consequential, or incidental damages or damages for lost profits, loss 
 *    of revenue or loss of use, whether such damages arise in contract, 
 *    negligence, tort, under statute, in equity, at law or otherwise, even if
 *    advised of the possibility of such damage. 
 */
/*
 * The CyberPro2010 driver is based on sample code provided by IGS 
 * Technologies, Inc. http://www.integraphics.com.
 * IGS Technologies, Inc makes no representations about the suitability of 
 * this software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

/*
** Modified from ct_alloc.c for use with
** IGS CyberPro2010.
*/

/*
 * This file implements functions a simple allocator of video memory for
 * scratch space, patterns etc.
 *
 * The allocator can currently only free the last request. More advanced
 * functionality can be added when it is actually required.
 */
#include "vga256.h"
#include "xf86.h"
#include "vga.h"	       /* For vga256InfoRec */

static int allocatebase;       /* Video memory address of allocation space. */
static int freespace;	       /* Number of bytes left for allocation. */
static int currentaddr;	       /* Video address of first available space. */
static int lastaddr;	       /* Video address of last allocation. */
static int lastsize;	       /* Size of the last request. */

/*
 * Initialize the allocator. This is called once at server start-up.
 */

int
igsInitializeAllocator()
{
    /* We want to align on a multiple of 8 bytes, should be OK. 
     * Allocate from the top of ram. */

    allocatebase = (vga256InfoRec.videoRam<<10);
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
igsAllocate(size, align)
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
igsFree(vidaddr)
    int vidaddr;
{
    if (vidaddr != lastaddr)	       /* If not allocated right before, */
	return;			       /* do nothing. */
    /* Undo the last allocation. */
    currentaddr += lastsize;
    freespace += lastsize;
    lastaddr = -2;
}

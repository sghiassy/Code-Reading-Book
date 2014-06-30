/* $XConsortium: sunAmScreen.c,v 1.1 94/04/01 17:55:13 dpw Exp $ */
/*
 * Copyright 1993 Vrije Universiteit, The Netherlands
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the Vrije Universiteit not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The Vrije Universiteit makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * THE VRIJE UNIVERSITEIT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE VRIJE UNIVERSITEIT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This is the code that Amoeba uses to probe and map in a sun bitmap display.
 */

#include "sun.h"

#include "amoeba.h"
#include "cmdreg.h"
#include "stderr.h"
#include "ampolicy.h"
#include "proc.h"
#include "server/iop/iop.h"

bufptr
sunMapAmoebaScreen(cap, nbytes)
    capability	*cap;
    int		nbytes;
{
    static char	*addr;
    segid	seg;
    char	*findhole();

    if (addr) return addr;

    /* First find a place to map in the segment: */
    addr = findhole(nbytes);
    seg = seg_map(cap, addr, nbytes, MAP_TYPEDATA|MAP_READWRITE|MAP_INPLACE);
    if (seg < 0) {
	ErrorF("Xsun: map_segment(bitmap) %x %x: %s\n",
				addr, nbytes, err_why(ERR_CONVERT(seg)));
	return NULL;
    }

    return addr;
}

extern capability iopcap;

int
sunAmoebaScreen(screen, name, depth)
    IOPFrameBufferInfo	*screen;
    char		*name;	 /* The name of the device we want to find */
    unsigned short	depth;	 /* framebuffer depth we expect to find */
{
    errstat err;

    if ((err = iop_framebufferinfo(&iopcap, screen)) != STD_OK) {
	ErrorF("Xsun: get frame buffer info failed: %s\n", err_why(err));
	return 0;
    }

    if (screen->xmm <= 0 || screen->ymm <= 0) {
	ErrorF("X or Y dimensions of screen <= 0\n");
	return 0;
    }
    if (screen->depth != depth ||
		strncmp(name, screen->name, IOPNAMELEN) != 0) {
	return 0;
    }

    /* printf("Screen information: width %d, height %d, stride %d\n",
	      screen->width, screen->height, screen->stride); */

    return 1;
}

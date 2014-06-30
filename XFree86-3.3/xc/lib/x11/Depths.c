/* $XConsortium: Depths.c,v 1.7 94/04/17 20:19:05 gildea Exp $ */
/*

Copyright (c) 1989  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

*/

#include "Xlibint.h"
#include <stdio.h>

/*
 * XListDepths - return info from connection setup
 */
int *XListDepths (dpy, scrnum, countp)
    Display *dpy;
    int scrnum;
    int *countp;
{
    Screen *scr;
    int count;
    int *depths;

    if (scrnum < 0 || scrnum >= dpy->nscreens) return NULL;

    scr = &dpy->screens[scrnum];
    if ((count = scr->ndepths) > 0) {
	register Depth *dp;
	register int i;

	depths = (int *) Xmalloc (count * sizeof(int));
	if (!depths) return NULL;
	for (i = 0, dp = scr->depths; i < count; i++, dp++) 
	  depths[i] = dp->depth;
    } else {
	/* a screen must have a depth */
	return NULL;
    }
    *countp = count;
    return depths;
}

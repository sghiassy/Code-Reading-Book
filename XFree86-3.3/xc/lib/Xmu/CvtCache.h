/* $XConsortium: CvtCache.h,v 1.7 94/04/17 20:15:56 converse Exp $ */

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

/*
 *			       Public Interfaces
 * 
 * XmuCvtCache *XmuCvtCacheLookupDisplay (dpy)
 *     Display *dpy;
 */

#ifndef _XMU_CVTCACHE_H_
#define _XMU_CVTCACHE_H_

#include <X11/Xmu/DisplayQue.h>
#include <X11/Xfuncproto.h>

typedef struct _XmuCvtCache {
    struct {
	char **bitmapFilePath;
    } string_to_bitmap;
    /* add other per-display data that needs to be cached */
} XmuCvtCache;

_XFUNCPROTOBEGIN

extern XmuCvtCache *_XmuCCLookupDisplay(
#if NeedFunctionPrototypes
    Display*	/* dpy */
#endif
);

_XFUNCPROTOEND

#endif /* _XMU_CVTCACHE_H_ */

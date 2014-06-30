/* $XConsortium: Drawing.h,v 1.11 94/04/17 20:16:03 converse Exp $ */

/*
 
Copyright (c) 1988  X Consortium

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
 * The interfaces described by this header file are for miscellaneous utilities
 * and are not part of the Xlib standard.
 */

#ifndef _XMU_DRAWING_H_
#define _XMU_DRAWING_H_

#include <X11/Xfuncproto.h>

#if NeedFunctionPrototypes
#include <stdio.h>
#if ! defined(_XtIntrinsic_h) && ! defined(PIXEL_ALREADY_TYPEDEFED)
typedef unsigned long Pixel;
#endif
#endif

_XFUNCPROTOBEGIN

extern void XmuDrawRoundedRectangle(
#if NeedFunctionPrototypes
    Display*	/* dpy */,
    Drawable 	/* draw */,
    GC 		/* gc */,
    int		/* x */,
    int		/* y */,
    int		/* w */,
    int		/* h */,
    int		/* ew */,
    int		/* eh */
#endif
);

extern void XmuFillRoundedRectangle(
#if NeedFunctionPrototypes
    Display*	/* dpy */,
    Drawable 	/* draw */,
    GC 		/* gc */,
    int		/* x */,
    int		/* y */,
    int		/* w */,
    int		/* h */,
    int		/* ew */,
    int		/* eh */
#endif
);

extern void XmuDrawLogo(
#if NeedFunctionPrototypes
    Display*	/* dpy */,
    Drawable 	/* drawable */,
    GC		/* gcFore */,
    GC		/* gcBack */,
    int		/* x */,
    int		/* y */,
    unsigned int /* width */,
    unsigned int /* height */
#endif
);

extern Pixmap XmuCreatePixmapFromBitmap(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    Drawable 		/* d */,
    Pixmap 		/* bitmap */,
    unsigned int	/* width */,
    unsigned int	/* height */,
    unsigned int	/* depth */,
    unsigned long	/* fore */,
    unsigned long	/* back */
#endif
);

extern Pixmap XmuCreateStippledPixmap(
#if NeedFunctionPrototypes
    Screen*		/* screen */,
    Pixel		/* fore */,
    Pixel		/* back */,
    unsigned int	/* depth */
#endif
);

extern void XmuReleaseStippledPixmap(
#if NeedFunctionPrototypes
    Screen*		/* screen */,
    Pixmap 		/* pixmap */
#endif
);

extern Pixmap XmuLocateBitmapFile(
#if NeedFunctionPrototypes
    Screen*		/* screen */,
    _Xconst char*	/* name */,
    char*		/* srcname_return */,
    int 		/* srcnamelen */,
    int*		/* width_return */,
    int*		/* height_return, */,
    int*		/* xhot_return */,
    int*		/* yhot_return */
#endif
);

extern Pixmap XmuLocatePixmapFile(
#if NeedFunctionPrototypes
    Screen*		/* screen */,
    _Xconst char*	/* name */,
    unsigned long	/* fore */,
    unsigned long	/* back */,
    unsigned int	/* depth */,
    char*		/* srcname_return */,
    int 		/* srcnamelen */,
    int*		/* width_return */,
    int*		/* height_return, */,
    int*		/* xhot_return */,
    int*		/* yhot_return */
#endif
);

extern int XmuReadBitmapData(
#if NeedFunctionPrototypes
    FILE*		/* fstream */,
    unsigned int*	/* width_return */,
    unsigned int*	/* height_return */,
    unsigned char**	/* datap_return */,
    int*		/* xhot_return */,
    int*		/* yhot_return */
#endif
);

extern int XmuReadBitmapDataFromFile(
#if NeedFunctionPrototypes
    _Xconst char*	/* filename */,
    unsigned int*	/* width_return */,
    unsigned int*	/* height_return */,
    unsigned char**	/* datap_return */,
    int*		/* xhot_return */,
    int*		/* yhot_return */
#endif
);

_XFUNCPROTOEND

#endif /* _XMU_DRAWING_H_ */

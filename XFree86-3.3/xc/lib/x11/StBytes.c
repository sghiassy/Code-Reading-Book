/* $XConsortium: StBytes.c /main/12 1996/10/22 14:23:01 kaleb $ */
/*

Copyright (c) 1986  X Consortium

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

#include <X11/Xlibint.h>
#include <X11/Xatom.h>

/* insulate predefined atom numbers from cut routines */
static Atom n_to_atom[8] = { 
	XA_CUT_BUFFER0,
	XA_CUT_BUFFER1,
	XA_CUT_BUFFER2,
	XA_CUT_BUFFER3,
	XA_CUT_BUFFER4,
	XA_CUT_BUFFER5,
	XA_CUT_BUFFER6,
	XA_CUT_BUFFER7};

XRotateBuffers (dpy, rotate)
    register Display *dpy;
    int rotate;
{
    return XRotateWindowProperties(dpy, RootWindow(dpy, 0), n_to_atom, 8, rotate);
}
    
char *XFetchBuffer (dpy, nbytes, buffer)
    register Display *dpy;
    int *nbytes;
    register int buffer;
{
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long leftover;
    unsigned char *data;
    *nbytes = 0;
    if ((buffer < 0) || (buffer > 7)) return (NULL);
/* XXX should be (sizeof (maxint) - 1)/4 */
    if (XGetWindowProperty(dpy, RootWindow(dpy, 0), n_to_atom[buffer], 
	0L, 10000000L, False, XA_STRING, 
	&actual_type, &actual_format, &nitems, &leftover, &data) != Success) {
	return (NULL);
	}
    if ( (actual_type == XA_STRING) &&  (actual_format != 32) ) {
	*nbytes = nitems;
	return((char *)data);
	}
    if ((char *) data != NULL) Xfree ((char *)data);
    return(NULL);
}

char *XFetchBytes (dpy, nbytes)
    register Display *dpy;
    int *nbytes;
{
    return (XFetchBuffer (dpy, nbytes, 0));
}

#if NeedFunctionPrototypes
XStoreBuffer (
    register Display *dpy,
    _Xconst char *bytes,
    int nbytes,
    register int buffer)
#else
XStoreBuffer (dpy, bytes, nbytes, buffer)
    register Display *dpy;
    char *bytes;
    int nbytes;
    register int buffer;
#endif
{
    if ((buffer < 0) || (buffer > 7)) return 0;
    return XChangeProperty(dpy, RootWindow(dpy, 0), n_to_atom[buffer], 
	XA_STRING, 8, PropModeReplace, (unsigned char *) bytes, nbytes);
}

#if NeedFunctionPrototypes
XStoreBytes (
    register Display *dpy,
    _Xconst char *bytes,
    int nbytes)
#else
XStoreBytes (dpy, bytes, nbytes)
    register Display *dpy;
    char *bytes;
    int nbytes;
#endif
{
    return XStoreBuffer (dpy, bytes, nbytes, 0);
}

/* $XConsortium: FetchName.c,v 11.26 94/04/17 20:19:17 gildea Exp $ */
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
#include <X11/Xos.h>
#include <stdio.h>


Status XFetchName (dpy, w, name)
    register Display *dpy;
    Window w;
    char **name;
{
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long leftover;
    unsigned char *data = NULL;
    if (XGetWindowProperty(dpy, w, XA_WM_NAME, 0L, (long)BUFSIZ, False, XA_STRING, 
	&actual_type,
	&actual_format, &nitems, &leftover, &data) != Success) {
        *name = NULL;
	return (0);
	}
    if ( (actual_type == XA_STRING) &&  (actual_format == 8) ) {

	/* The data returned by XGetWindowProperty is guarranteed to
	contain one extra byte that is null terminated to make retrieveing
	string properties easy. */

	*name = (char *)data;
	return(1);
	}
    if (data) Xfree ((char *)data);
    *name = NULL;
    return(0);
}

Status XGetIconName (dpy, w, icon_name)
    register Display *dpy;
    Window w;
    char **icon_name;
{
    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long leftover;
    unsigned char *data = NULL;
    if (XGetWindowProperty(dpy, w, XA_WM_ICON_NAME, 0L, (long)BUFSIZ, False,
        XA_STRING, 
	&actual_type,
	&actual_format, &nitems, &leftover, &data) != Success) {
        *icon_name = NULL;
	return (0);
	}
    if ( (actual_type == XA_STRING) &&  (actual_format == 8) ) {

	/* The data returned by XGetWindowProperty is guarranteed to
	contain one extra byte that is null terminated to make retrieveing
	string properties easy. */

	*icon_name = (char*)data;
	return(1);
	}
    if (data) Xfree ((char *)data);
    *icon_name = NULL;
    return(0);
}

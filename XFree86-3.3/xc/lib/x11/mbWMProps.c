/* $XConsortium: mbWMProps.c,v 1.5 94/04/17 20:22:14 rws Exp $ */
/*

Copyright (c) 1991  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

#include <X11/Xlibint.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xlocale.h>

#if NeedFunctionPrototypes
void XmbSetWMProperties (
    Display *dpy,
    Window w,
    _Xconst char *windowName,
    _Xconst char *iconName,
    char **argv,
    int argc,
    XSizeHints *sizeHints,
    XWMHints *wmHints,
    XClassHint *classHints)
#else
void XmbSetWMProperties (dpy, w, windowName, iconName, argv, argc, sizeHints,
			 wmHints, classHints)
     Display *dpy;
     Window w;			/* window to decorate */
     char *windowName;		/* name of application */
     char *iconName;		/* name string for icon */
     char **argv;		/* command line */
     int argc;			/* size of command line */
     XSizeHints *sizeHints;	/* size hints for window in its normal state */
     XWMHints *wmHints;		/* miscelaneous window manager hints */
     XClassHint *classHints;	/* resource name and class */
#endif
{
    XTextProperty wname, iname;
    XTextProperty *wprop = NULL;
    XTextProperty *iprop = NULL;
    char *locale;

    if (windowName &&
	XmbTextListToTextProperty(dpy, (char**)&windowName, 1,
				   XStdICCTextStyle, &wname) >= Success)
	wprop = &wname;
    if (iconName &&
	XmbTextListToTextProperty(dpy, (char**)&iconName, 1,
				   XStdICCTextStyle, &iname) >= Success)
	iprop = &iname;
    XSetWMProperties(dpy, w, wprop, iprop, argv, argc,
		     sizeHints, wmHints, classHints);
    if (wprop)
	Xfree((char *)wname.value);
    if (iprop)
	Xfree((char *)iname.value);
    locale = setlocale(LC_CTYPE, (char *)NULL);
    if (locale)
	XChangeProperty (dpy, w, XInternAtom(dpy, "WM_LOCALE_NAME", False),
			 XA_STRING, 8, PropModeReplace,
			 (unsigned char *)locale, strlen(locale));
}

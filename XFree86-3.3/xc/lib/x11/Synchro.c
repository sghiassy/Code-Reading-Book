/* $XConsortium: Synchro.c,v 11.14 94/04/17 20:21:16 rws Exp $ */
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

#include "Xlibint.h"


int _XSyncFunction(dpy)
register Display *dpy;
{
	XSync(dpy,0);
	return 0;
}

#if NeedFunctionPrototypes
int (*XSynchronize(Display *dpy, int onoff))()
#else
int (*XSynchronize(dpy,onoff))()
     register Display *dpy;
     int onoff;
#endif
{
        int (*temp)();
	int (*func)() = NULL;

	if (onoff)
	    func = _XSyncFunction;

	LockDisplay(dpy);
	if (dpy->flags & XlibDisplayPrivSync) {
	    temp = dpy->savedsynchandler;
	    dpy->savedsynchandler = func;
	} else {
	    temp = dpy->synchandler;
	    dpy->synchandler = func;
	}
	UnlockDisplay(dpy);
	return (temp);
}

#if NeedFunctionPrototypes
int (*XSetAfterFunction(
     Display *dpy,
     int (*func)(
#if NeedNestedPrototypes
		 Display*
#endif
		 )
))()
#else
int (*XSetAfterFunction(dpy,func))()
     register Display *dpy;
     int (*func)(
#if NeedNestedPrototypes
		 Display*
#endif
		 );
#endif
{
        int (*temp)();

	LockDisplay(dpy);
	if (dpy->flags & XlibDisplayPrivSync) {
	    temp = dpy->savedsynchandler;
	    dpy->savedsynchandler = func;
	} else {
	    temp = dpy->synchandler;
	    dpy->synchandler = func;
	}
	UnlockDisplay(dpy);
	return (temp);
}


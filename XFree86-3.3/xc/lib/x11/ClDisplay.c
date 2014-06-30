/* $XConsortium: ClDisplay.c,v 11.31 95/06/05 19:03:09 gildea Exp $ */

/*

Copyright (c) 1985, 1990  X Consortium

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

#include "Xlibint.h"

extern void _XFreeDisplayStructure();

/* 
 * XCloseDisplay - XSync the connection to the X Server, close the connection,
 * and free all associated storage.  Extension close procs should only free
 * memory and must be careful about the types of requests they generate.
 */

XCloseDisplay (dpy)
	register Display *dpy;
{
	register _XExtension *ext;
	register int i;

	if (!(dpy->flags & XlibDisplayClosing))
	{
	    dpy->flags |= XlibDisplayClosing;
	    for (i = 0; i < dpy->nscreens; i++) {
		    register Screen *sp = &dpy->screens[i];
		    XFreeGC (dpy, sp->default_gc);
	    }
	    if (dpy->cursor_font != None) {
		XUnloadFont (dpy, dpy->cursor_font);
	    }
	    XSync(dpy, 1);  /* throw away pending events, catch errors */
	    /* call out to any extensions interested */
	    for (ext = dpy->ext_procs; ext; ext = ext->next) {
		if (ext->close_display)
		    (*ext->close_display)(dpy, &ext->codes);
	    }
	    /* if the closes generated more protocol, sync them up */
	    if (dpy->request != dpy->last_request_read)
		XSync(dpy, 1);
	}
	_XDisconnectDisplay(dpy->trans_conn);
	_XFreeDisplayStructure (dpy);
	return 0;
}

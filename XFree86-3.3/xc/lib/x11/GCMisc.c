/* $XConsortium: GCMisc.c /main/5 1996/10/22 14:19:06 kaleb $ */
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

XSetArcMode (dpy, gc, arc_mode)
register Display *dpy;
register GC gc;
int arc_mode;
{
    LockDisplay(dpy);
    if (gc->values.arc_mode != arc_mode) {
	gc->values.arc_mode = arc_mode;
	gc->dirty |= GCArcMode;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}

XSetFillRule (dpy, gc, fill_rule)
register Display *dpy;
register GC gc;
int fill_rule;
{
    LockDisplay(dpy);
    if (gc->values.fill_rule != fill_rule) {
	gc->values.fill_rule = fill_rule;
	gc->dirty |= GCFillRule;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}

XSetFillStyle (dpy, gc, fill_style)
register Display *dpy;
register GC gc;
int fill_style;
{
    LockDisplay(dpy);
    if (gc->values.fill_style != fill_style) {
	gc->values.fill_style = fill_style;
	gc->dirty |= GCFillStyle;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}

XSetGraphicsExposures (dpy, gc, graphics_exposures)
register Display *dpy;
register GC gc;
Bool graphics_exposures;
{
    LockDisplay(dpy);
    if (gc->values.graphics_exposures != graphics_exposures) {
	gc->values.graphics_exposures = graphics_exposures;
	gc->dirty |= GCGraphicsExposures;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}

XSetSubwindowMode (dpy, gc, subwindow_mode)
register Display *dpy;
register GC gc;
int subwindow_mode;
{
    LockDisplay(dpy);
    if (gc->values.subwindow_mode != subwindow_mode) {
	gc->values.subwindow_mode = subwindow_mode;
	gc->dirty |= GCSubwindowMode;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
}

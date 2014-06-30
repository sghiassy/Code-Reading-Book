/* $XConsortium: ShapeWidg.c,v 1.6 94/04/17 20:16:13 converse Exp $ */

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

#include <X11/IntrinsicP.h>
#include <X11/extensions/shape.h>
#include "Converters.h"
#include "Drawing.h"

static ShapeError();
static  ShapeRectangle(), ShapeOval(), ShapeEllipseOrRoundedRectangle();

Boolean XmuReshapeWidget(w, shape_style, corner_width, corner_height)
    Widget w;
    int shape_style;
    int corner_width, corner_height;
{
    switch (shape_style) {

      case XmuShapeRectangle:
	ShapeRectangle(w);
	break;

      case XmuShapeOval:
	ShapeOval(w);
	break;

      case XmuShapeEllipse:
      case XmuShapeRoundedRectangle:
	ShapeEllipseOrRoundedRectangle
	    (w,
	     ((shape_style == XmuShapeEllipse) ? True : False),
	     corner_width,
	     corner_height);
	break;

      default:
	ShapeError(w);
	return False;
    }
    return True;
}

static ShapeError(w)
    Widget w;
{
    String params[1];
    Cardinal num_params = 1;
    params[0] = XtName(w);
    XtAppWarningMsg( XtWidgetToApplicationContext(w),
		     "shapeUnknown", "xmuReshapeWidget", "XmuLibrary",
		     "Unsupported shape style for Command widget \"%s\"",
		     params, &num_params
		   );
}


static ShapeRectangle(w)
    Widget w;
{
    XShapeCombineMask( XtDisplay(w), XtWindow(w),
		       ShapeBounding, 0, 0, None, ShapeSet );
    XShapeCombineMask( XtDisplay(w), XtWindow(w),
		       ShapeClip, 0, 0, None, ShapeSet );
}


static ShapeOval(w)
    Widget w;
{
    Display *dpy = XtDisplay(w);
    unsigned width = w->core.width + (w->core.border_width<<1);
    unsigned height = w->core.height + (w->core.border_width<<1);
    Pixmap p = XCreatePixmap( dpy, XtWindow(w), width, height, 1 );
    XGCValues values;
    GC gc;
    int rad;

    values.foreground = 0;
    values.background = 1;
    values.cap_style = CapRound;
    values.line_width = height;
    gc = XCreateGC (dpy, p,
		    GCForeground | GCBackground | GCLineWidth | GCCapStyle,
		    &values);
    XFillRectangle( dpy, p, gc, 0, 0, width, height );
    XSetForeground( dpy, gc, 1 );
    if (width <= height) {
	/* cannot be oval, fall back to ellipse */
	XFillArc( dpy, p, gc, 0, 0, width, height, 0, 360*64 );
    } else {
	rad = height >> 1;
	XDrawLine( dpy, p, gc, rad, rad, (int)width - rad - 1, rad );
    }
    XShapeCombineMask( dpy, XtWindow(w), ShapeBounding, 
		       -(w->core.border_width), -(w->core.border_width),
		       p, ShapeSet );
    if (w->core.border_width) {
	XSetForeground( dpy, gc, 0 );
	XFillRectangle( dpy, p, gc, 0, 0, width, height );
	values.line_width = w->core.height;
	values.foreground = 1;
	XChangeGC (dpy, gc, GCLineWidth|GCForeground, &values);
	if (w->core.width <= w->core.height) {
	    /* cannot be oval, fall back to ellipse */
	    XFillArc( dpy, p, gc, 0, 0, w->core.width, w->core.height,
		      0, 360*64 );
	} else {
	    rad = w->core.height >> 1;
	    XDrawLine( dpy, p, gc, rad, rad,
		       (int)w->core.width - rad - 1, rad );
	}
	XShapeCombineMask( dpy, XtWindow(w), ShapeClip, 0, 0, p, ShapeSet );
    } else {
	XShapeCombineMask( XtDisplay(w), XtWindow(w),
			  ShapeClip, 0, 0, None, ShapeSet );
    }
    XFreePixmap( dpy, p );
    XFreeGC (dpy, gc );
}


static ShapeEllipseOrRoundedRectangle(w, ellipse, ew, eh)
    Widget w;
    Boolean ellipse;
    int ew, eh;
{
    Display *dpy = XtDisplay(w);
    unsigned width = w->core.width + (w->core.border_width<<1);
    unsigned height = w->core.height + (w->core.border_width<<1);
    Pixmap p = XCreatePixmap( dpy, XtWindow(w), width, height, 1 );
    XGCValues values;
    GC gc;

    values.foreground = 0;
    gc = XCreateGC (dpy, p, GCForeground, &values );
    XFillRectangle( dpy, p, gc, 0, 0, width, height );
    XSetForeground (dpy, gc, 1);
    if (!ellipse)
	XmuFillRoundedRectangle( dpy, p, gc, 0, 0, (int)width, (int)height,
				 ew, eh );
    else
	XFillArc( dpy, p, gc, 0, 0, width, height, 0, 360*64 );
    XShapeCombineMask( dpy, XtWindow(w), ShapeBounding, 
		       -(w->core.border_width), -(w->core.border_width),
		       p, ShapeSet );
    if (w->core.border_width) {
	XSetForeground (dpy, gc, 0);
	XFillRectangle( dpy, p, gc, 0, 0, width, height );
	XSetForeground (dpy, gc, 1);
	if (!ellipse)
	    XmuFillRoundedRectangle( dpy, p, gc, 0, 0,
				     (int)w->core.width, (int)w->core.height,
				     ew, eh );
	else
	    XFillArc( dpy, p, gc, 0, 0, w->core.width, w->core.height,
		      0, 360*64 );
	XShapeCombineMask( dpy, XtWindow(w), ShapeClip, 0, 0, p, ShapeSet );
    } else {
	XShapeCombineMask( XtDisplay(w), XtWindow(w),
			   ShapeClip, 0, 0, None, ShapeSet );
    }
    XFreePixmap( dpy, p );
    XFreeGC (dpy, gc);
}

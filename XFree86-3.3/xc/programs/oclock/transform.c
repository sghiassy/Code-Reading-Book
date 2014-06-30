/* $XConsortium: transform.c,v 1.4 94/04/17 20:37:59 gildea Exp $ */
/* $XFree86: xc/programs/oclock/transform.c,v 3.0 1996/08/25 14:14:13 dawes Exp $ */
/*

Copyright (c) 1993  X Consortium

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


/*
 * transformed coordinate system objects for X
 */

# include	<X11/Xlib.h>
# include	"transform.h"

#ifndef X_NOT_STDC_ENV
#include	<stdlib.h>
#else
char *malloc();
#endif

static XPoint *
TranslatePoints (points, n_points, t, mode)
TPoint	*points;
int	n_points;
Transform	*t;
int	mode;
{
	XPoint	*xpoints;
	int	i;
	double	xoff = 0.0, yoff = 0.0;

	xpoints = (XPoint *) malloc ((unsigned)n_points * sizeof (*xpoints));
	if (!xpoints)
		return 0;
	for (i = 0; i < n_points; i++) {
		xpoints[i].x = Xx(points[i].x + xoff, points[i].y + yoff, t);
		xpoints[i].y = Xy(points[i].x + xoff, points[i].y + yoff, t);
		if (mode == CoordModePrevious) {
			xoff += points[i].x;
			yoff += points[i].y;
		}
	}
	return xpoints;
}

TFillPolygon (dpy, d, gc, t, points, n_points, shape, mode)
register Display	*dpy;
Drawable		d;
GC			gc;
Transform		*t;
TPoint			*points;
int			n_points;
int			shape;
int			mode;
{
	XPoint	*xpoints;

	xpoints = TranslatePoints (points, n_points, t, mode);
	if (xpoints) {
		XFillPolygon (dpy, d, gc, xpoints, n_points, shape,
				CoordModeOrigin);
		free (xpoints);
	}
}

TDrawArc (dpy, d, gc, t, x, y, width, height, angle1, angle2)
	register Display	*dpy;
	Drawable		d;
	GC			gc;
	Transform		*t;
	double			x, y, width, height;
	int			angle1, angle2;
{
	int	xx, xy, xw, xh;

	xx = Xx(x,y,t);
	xy = Xy(x,y,t);
	xw = Xwidth (width, height, t);
	xh = Xheight (width, height, t);
	if (xw < 0) {
		xx += xw;
		xw = -xw;
	}
	if (xh < 0) {
		xy += xh;
		xh = -xh;
	}
	XDrawArc (dpy, d, gc, xx, xy, xw, xh, angle1, angle2);
}

TFillArc (dpy, d, gc, t, x, y, width, height, angle1, angle2)
	register Display	*dpy;
	Drawable		d;
	GC			gc;
	Transform		*t;
	double			x, y, width, height;
	int			angle1, angle2;
{
	int	xx, xy, xw, xh;

	xx = Xx(x,y,t);
	xy = Xy(x,y,t);
	xw = Xwidth (width, height, t);
	xh = Xheight (width, height, t);
	if (xw < 0) {
		xx += xw;
		xw = -xw;
	}
	if (xh < 0) {
		xy += xh;
		xh = -xh;
	}
	XFillArc (dpy, d, gc, xx, xy, xw, xh, angle1, angle2);
}

SetTransform (t, xx1, xx2, xy1, xy2, tx1, tx2, ty1, ty2)
Transform	*t;
int		xx1, xx2, xy1, xy2;
double		tx1, tx2, ty1, ty2;
{
	t->mx = ((double) xx2 - xx1) / (tx2 - tx1);
	t->bx = ((double) xx1) - t->mx * tx1;
	t->my = ((double) xy2 - xy1) / (ty2 - ty1);
	t->by = ((double) xy1) - t->my * ty1;
}

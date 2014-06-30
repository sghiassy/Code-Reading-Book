/* $XConsortium: SetRGBCMap.c,v 1.5 94/04/17 20:21:02 gildea Exp $ */
/*

Copyright (c) 1989  X Consortium

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
#include "Xatomtype.h"
#include <X11/Xatom.h>

void XSetRGBColormaps (dpy, w, cmaps, count, property)
    Display *dpy;
    Window w;
    XStandardColormap *cmaps;
    int count;
    Atom property;			/* XA_RGB_BEST_MAP, etc. */
{
    register int i;			/* iterator variable */
    register xPropStandardColormap *map;  /* tmp variable, data in prop */
    register XStandardColormap *cmap;	/* tmp variable, user data */
    xPropStandardColormap *data, tmpdata;  /* scratch data */
    int mode = PropModeReplace;		/* for partial writes */
    Bool alloced_scratch_space;		/* do we need to free? */
	

    if (count < 1) return;

    /*
     * if doing more than one, allocate scratch space for it
     */
    if ((count > 1) && ((data = ((xPropStandardColormap *)
				 Xmalloc(count*sizeof(xPropStandardColormap))))
			 != NULL)) {
	alloced_scratch_space = True;
    } else {
	data = &tmpdata;
	alloced_scratch_space = False;
    }


    /*
     * Do the iteration.  If using temp space put out each part of the prop;
     * otherwise, wait until the end and blast it all at once.
     */
    for (i = count, map = data, cmap = cmaps; i > 0; i--, cmap++) {
	map->colormap	= cmap->colormap;
	map->red_max	= cmap->red_max;
	map->red_mult	= cmap->red_mult;
	map->green_max	= cmap->green_max;
	map->green_mult	= cmap->green_mult;
	map->blue_max	= cmap->blue_max;
	map->blue_mult	= cmap->blue_mult;
	map->base_pixel	= cmap->base_pixel;
	map->visualid	= cmap->visualid;
	map->killid	= cmap->killid;

	if (alloced_scratch_space) {
	    map++;
	} else {
	    XChangeProperty (dpy, w, property, XA_RGB_COLOR_MAP, 32, mode,
			     (unsigned char *) data,
			     NumPropStandardColormapElements);
	    mode = PropModeAppend;
	}
    }

    if (alloced_scratch_space) {
	XChangeProperty (dpy, w, property, XA_RGB_COLOR_MAP, 32,
			 PropModeReplace, (unsigned char *) data,
			 (int) (count * NumPropStandardColormapElements));
	Xfree ((char *) data);
    }
}

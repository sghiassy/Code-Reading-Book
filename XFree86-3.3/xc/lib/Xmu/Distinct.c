/* $XConsortium: Distinct.c,v 1.4 94/04/17 20:15:59 gildea Exp $ */
/* $XFree86: xc/lib/Xmu/Distinct.c,v 3.0 1996/05/06 05:54:32 dawes Exp $ */

/*

Copyright (c) 1990  X Consortium

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
 * Author:  Keith Packard, MIT X Consortium
 */

# include   <X11/Xlib.h>
# ifndef X_NOT_STDC_ENV
# include   <stdlib.h>
# endif

/*
 * Distinguishable colors routine.  Determines if two colors are
 * distinguishable or not.  Somewhat arbitrary meaning.
 */

#define MIN_DISTINGUISH	10000.0

Bool
XmuDistinguishableColors (colors, count)
XColor	*colors;
int	count;
{
    double	    deltaRed, deltaGreen, deltaBlue;
    double	    dist;
    int		    i, j;

    for (i = 0; i < count - 1; i++)
	for (j = i + 1; j < count; j++)
	{
     	    deltaRed = (double)colors[i].red - (double)colors[j].red;
    	    deltaGreen = (double)colors[i].green - (double)colors[j].green;
    	    deltaBlue = (double)colors[i].blue - (double)colors[j].blue;
    	    dist = deltaRed * deltaRed +
	       	   deltaGreen * deltaGreen +
 	       	   deltaBlue * deltaBlue;
	    if (dist <= MIN_DISTINGUISH * MIN_DISTINGUISH)
		return False;
	}
    return True;
}

Bool
XmuDistinguishablePixels (dpy, cmap, pixels, count)
    Display	    *dpy;
    Colormap	    cmap;
    unsigned long   *pixels;
    int		    count;
{
    XColor  *defs;
    int	    i, j;
    Bool    ret;

    for (i = 0; i < count - 1; i++)
	for (j = i + 1; j < count; j++)
	    if (pixels[i] == pixels[j])
		return False;
    defs = (XColor *) malloc (count * sizeof (XColor));
    if (!defs)
	return False;
    for (i = 0; i < count; i++)
	defs[i].pixel = pixels[i];
    XQueryColors (dpy, cmap, defs, count);
    ret = XmuDistinguishableColors (defs, count);
    free ((char *) defs);
    return ret;
}

/* $XConsortium: gridP.h,v 1.13 94/04/17 20:24:14 gildea Exp $ */
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


#ifndef _FontGridP_h_
#define _FontGridP_h_

#include "grid.h"

typedef struct _FontGridClassPart { int dummy; } FontGridClassPart;

typedef struct _FontGridClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    FontGridClassPart	grid_class;
} FontGridClassRec;
extern FontGridClassRec fontgridClassRec;

typedef struct _FontGridPart {
    XFontStruct *	text_font;		/* font to display */
    int			cell_cols, cell_rows;  /* number of cells */
    int			cell_width, cell_height;  /* size of cell */
    Pixel		foreground_pixel;	/* color of text */
    Pixel		box_pixel;	/* for box_chars */
    Boolean		center_chars;	/* center characters in grid */
    Boolean		box_chars;	/* put box around logical width */
    XtCallbackList	callbacks;	/* for notifying caller */
    int			internal_pad;	/* extra padding inside grid */
    Dimension		start_char;	/* first character of grid */
    int			grid_width;	/* width of grid lines */
    /* private data */
    GC			text_gc;	/* printing text */
    GC			box_gc;		/* for box_chars */
    int			xoff, yoff;	/* extra offsets within grid */
} FontGridPart;

typedef struct _FontGridRec {
    CorePart		core;
    SimplePart		simple;
    FontGridPart	fontgrid;
} FontGridRec;

#define DefaultCellWidth(fgw) (((fgw)->fontgrid.text_font->max_bounds.width) \
			       + ((fgw)->fontgrid.internal_pad * 2))
#define DefaultCellHeight(fgw) ((fgw)->fontgrid.text_font->ascent + \
				(fgw)->fontgrid.text_font->descent + \
				((fgw)->fontgrid.internal_pad * 2))


#define CellWidth(fgw) (((int)(fgw)->core.width - (fgw)->fontgrid.grid_width) \
			/ (fgw)->fontgrid.cell_cols \
			- (fgw)->fontgrid.grid_width)
#define CellHeight(fgw) (((int)(fgw)->core.height - (fgw)->fontgrid.grid_width)\
			 / (fgw)->fontgrid.cell_rows \
			 - (fgw)->fontgrid.grid_width)

#endif /* !_FontGridP_h_ */

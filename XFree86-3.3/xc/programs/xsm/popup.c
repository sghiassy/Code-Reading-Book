/* $XConsortium: popup.c,v 1.2 94/12/27 19:28:16 mor Exp $ */

/*

Copyright (c) 1994  X Consortium

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

#include "xsm.h"
#include <X11/Shell.h>


void
PopupPopup (parent, popup, transient, first_time,
    offset_x, offset_y, delAction)

Widget parent;
Widget popup;
Bool transient;
Bool first_time;
int offset_x;
int offset_y;
String delAction;

{
    if (!transient && !first_time)
    {
	/*
	 * For non-transient windows, if this isn't the first time
	 * it's being popped up, just pop it up in the old position.
	 */

	XtPopup (popup, XtGrabNone);
	return;
    }
    else
    {
	Position parent_x, parent_y;
	Position root_x, root_y;
	Position popup_x, popup_y;
	Dimension parent_width, parent_height, parent_border;
	Dimension popup_width, popup_height, popup_border;
	char geom[16];
	Bool repos = 0;

	/*
	 * The window we pop up must be visible on the screen.  We first
	 * try to position it at the desired location (relative to the
	 * parent widget).  Once we are able to compute the popup's
	 * geometry (after it's realized), we can determine if we need
	 * to reposition it.
	 */

	XtVaGetValues (parent,
	    XtNx, &parent_x,
	    XtNy, &parent_y,
	    XtNwidth, &parent_width,
	    XtNheight, &parent_height,
	    XtNborderWidth, &parent_border,
	    NULL);

	XtTranslateCoords (parent, parent_x, parent_y, &root_x, &root_y);

	popup_x = root_x + offset_x;
	popup_y = root_y + offset_y;

	if (transient)
	{
	    XtVaSetValues (popup,
	        XtNx, popup_x,
	        XtNy, popup_y,
                NULL);
	}
	else
	{
	    sprintf (geom, "+%d+%d", popup_x, popup_y);

	    XtVaSetValues (popup,
	        XtNgeometry, geom,
                NULL);
	}

	if (first_time)
	{
	    /*
	     * Realize it for the first time
	     */

	    XtRealizeWidget (popup);


	    /*
	     * Set support for WM_DELETE_WINDOW
	     */

	    (void) SetWM_DELETE_WINDOW (popup, delAction);
	}

	/*
	 * Now make sure it's visible.
	 */

	XtVaGetValues (popup,
	    XtNwidth, &popup_width,
	    XtNheight, &popup_height,
	    XtNborderWidth, &popup_border,
	    NULL);

	popup_border <<= 1;

	if ((int) (popup_x + popup_width + popup_border) >
	    WidthOfScreen (XtScreen (topLevel)))
	{
	    popup_x = WidthOfScreen (XtScreen (topLevel)) -
		popup_width - popup_border - parent_width - parent_border;

	    repos = 1;
	}

	if ((int) (popup_y + popup_height + popup_border) >
	    HeightOfScreen (XtScreen (topLevel)))
	{
	    popup_y = HeightOfScreen (XtScreen (topLevel)) -
		popup_height - popup_border - parent_height - parent_border;

	    repos = 1;
	}

	if (repos)
	{
	    if (transient)
	    {
		XtVaSetValues (popup,
	            XtNx, popup_x,
	            XtNy, popup_y,
                    NULL);
	    }
	    else
	    {
		/*
		 * The only way we can reposition a non-transient
		 * is by unrealizing it, setting the position, then
		 * doing a realize.
		 */

		XtUnrealizeWidget (popup);

		sprintf (geom, "+%d+%d", popup_x, popup_y);

		XtVaSetValues (popup,
	            XtNgeometry, geom,
                    NULL);

		XtRealizeWidget (popup);

		(void) SetWM_DELETE_WINDOW (popup, delAction);
	    }
	}

	XtPopup (popup, XtGrabNone);
    }
}

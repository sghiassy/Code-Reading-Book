/* $XConsortium: CutPaste.c,v 1.9 94/04/17 20:23:41 rws Exp $ */
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

/*
 * Author:  Davor Matic, MIT X Consortium
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include "BitmapP.h"
    
#include <stdio.h>
#include <math.h>

#ifndef abs
#define abs(x)                        (((x) > 0) ? (x) : -(x))
#endif
#define min(x, y)                     (((x) < (y)) ? (x) : (y))
#define max(x, y)                     (((x) > (y)) ? (x) : (y))


extern Boolean DEBUG;

/*****************************************************************************
 *                               Cut and Paste                               *
 *****************************************************************************/

/* ARGSUSED */
Boolean ConvertSelection(w, selection, target, type, val_ret, length, format)
    Widget w;
    Atom *selection, *target, *type;
    XtPointer *val_ret;
    unsigned long *length;
    int *format;
{
    XPointer *value = (XPointer *)val_ret;
    BitmapWidget BW = (BitmapWidget) w;
    Pixmap *pixmap;
    char *data;
    XImage *image;
    Dimension width, height;
 
    switch (*target) {

/*  XA_TARGETS undefined ?!?

    case XA_TARGETS:
	*type = XA_ATOM;
	*value = (XPointer) bitmapClassRec.bitmap_class.targets;
	*length = bitmapClassRec.bitmap_class.num_targets;
	*format = 32;
	return True;

*/

    case XA_BITMAP:
    case XA_PIXMAP:
	if (BWQueryMarked(w)) {
	  width = BW->bitmap.mark.to_x - BW->bitmap.mark.from_x + 1;
	  height = BW->bitmap.mark.to_y - BW->bitmap.mark.from_y + 1;
	  data = CreateCleanData(Length(width, height));
	  image = CreateBitmapImage(BW, data, width, height);
	  CopyImageData(BW->bitmap.image, image, 
			BW->bitmap.mark.from_x, BW->bitmap.mark.from_y,
			BW->bitmap.mark.to_x, BW->bitmap.mark.to_y, 0, 0);
	  pixmap = (Pixmap *) XtMalloc(sizeof(Pixmap));
	  *pixmap = GetPixmap(BW, image);
	  DestroyBitmapImage(&image);
	}
	else if (BWQueryStored(w)) {
	  pixmap = (Pixmap *) XtMalloc(sizeof(Pixmap));
	  *pixmap = GetPixmap(BW, BW->bitmap.storage);
	}
	else return False;
	*type = XA_PIXMAP;
	*value = (XPointer) pixmap;
	*length = 1;
	*format = 32;
	return True;

    default:
	return False;
    }
}

/* ARGSUSED */
void LoseSelection(w, selection)
    Widget w;
    Atom *selection;
{
    BitmapWidget BW = (BitmapWidget) w;

    if (DEBUG)
	fprintf(stderr, "Lost Selection\n");
    BW->bitmap.selection.own = False;

    BWUnmark(w);
}

/* ARGSUSED */
void SelectionDone(w, selection, target)
    Widget w;
    Atom *selection, *target;
{
/*  Done Automatically ?!?

    BitmapWidget BW = (BitmapWidget) w; 

    if (*target != XA_TARGETS)
	XtFree(BW->bitmap.value);

*/
}

void BWGrabSelection(w, btime)
    Widget w;
    Time btime;
{
    BitmapWidget BW = (BitmapWidget) w;

    BW->bitmap.selection.own = XtOwnSelection(w, XA_PRIMARY, btime,
					      ConvertSelection, 
					      LoseSelection, 
					      SelectionDone);
	if (DEBUG && BW->bitmap.selection.own)
	    fprintf(stderr, "Own the selection\n");
}

XImage *GetImage();

/* ARGSUSED */
void SelectionCallback(w, cldat, selection, type, val, length, format)
    Widget w;
    XtPointer cldat;
    Atom *selection, *type;
    XtPointer val;
    unsigned long *length;
    int *format;
{
    XPointer value = (XPointer)val;
    BitmapWidget BW = (BitmapWidget) w;
    Pixmap *pixmap;

   switch (*type) {
	
    case XA_BITMAP:
    case XA_PIXMAP:
	DestroyBitmapImage(&BW->bitmap.storage);
	pixmap = (Pixmap *) value;
	BW->bitmap.storage = GetImage(BW, *pixmap);
	XFree((char *)pixmap);
	break;
	
    default:
	XtWarning(" selection request failed.  BitmapWidget");
	break;
    }

    BW->bitmap.selection.limbo = FALSE;
}

void BWRequestSelection(w, btime, wait)
    Widget w;
    Time btime;
    Boolean wait;
{
  BitmapWidget BW = (BitmapWidget) w;
  
  if (BW->bitmap.selection.own)
    BWStore(w);
  else {
    XtGetSelectionValue(w, XA_PRIMARY, XA_PIXMAP,
			SelectionCallback, NULL, btime);
    
    BW->bitmap.selection.limbo = TRUE;
    
    if (wait)
      while (BW->bitmap.selection.limbo) {
	XEvent event;
	XtNextEvent(&event);
	XtDispatchEvent(&event);
      }
  }
}

/* ARGSUSED */
/* Returns true if there is a transferable selection */
Boolean BWQuerySelection(w, btime)
    Widget w;
    Time btime;
{
/* To be written.  XA_TARGETS to be used.  So far undefined ?!? */

  return True;
}
/*****************************************************************************/

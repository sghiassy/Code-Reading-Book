/* $XConsortium: RegstFlt.c,v 1.12 94/04/17 20:20:47 rws Exp $ */

 /*
  * Copyright 1990, 1991 by OMRON Corporation
  *
  * Permission to use, copy, modify, distribute, and sell this software and its
  * documentation for any purpose is hereby granted without fee, provided that
  * the above copyright notice appear in all copies and that both that
  * copyright notice and this permission notice appear in supporting
  * documentation, and that the name OMRON not be used in
  * advertising or publicity pertaining to distribution of the software without
  * specific, written prior permission.  OMRON makes no representations
  * about the suitability of this software for any purpose.  It is provided
  * "as is" without express or implied warranty.
  *
  * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
  * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
  * EVENT SHALL OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
  * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
  * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
  * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
  * PERFORMANCE OF THIS SOFTWARE. 
  *
  *	Author:	Seiji Kuwari	OMRON Corporation
  *				kuwa@omron.co.jp
  *				kuwa%omron.co.jp@uunet.uu.net
  */				
/*

Copyright (c) 1990, 1991  X Consortium

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
#include "Xlcint.h"

static void
_XFreeIMFilters(display)
    Display *display;
{
    register XFilterEventList fl;

    while (fl = display->im_filters) {
        display->im_filters = fl->next;
        Xfree((char *)fl);
    }
}

/*
 * Register a filter with the filter machinery by event mask.
 */
void
_XRegisterFilterByMask(display, window, event_mask, filter, client_data)
    Display *display;
    Window window;
    unsigned long event_mask;
    Bool (*filter)(
#if NeedNestedPrototypes
		   Display*, Window, XEvent*, XPointer
#endif
		   );
    XPointer client_data;
{
    XFilterEventRec		*rec;

    rec = (XFilterEventList)Xmalloc(sizeof(XFilterEventRec));
    if (!rec)
	return;
    rec->window = window;
    rec->event_mask = event_mask;
    rec->start_type = 0;
    rec->end_type = 0;
    rec->filter = filter;
    rec->client_data = client_data;
    LockDisplay(display);
    rec->next = display->im_filters;
    display->im_filters = rec;
    display->free_funcs->im_filters = _XFreeIMFilters;
    UnlockDisplay(display);
}

/*
 * Register a filter with the filter machinery by type code.
 */
void
_XRegisterFilterByType(display, window, start_type, end_type,
		       filter, client_data)
    Display *display;
    Window window;
    int start_type;
    int end_type;
    Bool (*filter)(
#if NeedNestedPrototypes
		   Display*, Window, XEvent*, XPointer
#endif
		   );
    XPointer client_data;
{
    XFilterEventRec		*rec;

    rec = (XFilterEventList)Xmalloc(sizeof(XFilterEventRec));
    if (!rec)
	return;
    rec->window = window;
    rec->event_mask = 0;
    rec->start_type = start_type;
    rec->end_type = end_type;
    rec->filter = filter;
    rec->client_data = client_data;
    LockDisplay(display);
    rec->next = display->im_filters;
    display->im_filters = rec;
    display->free_funcs->im_filters = _XFreeIMFilters;
    UnlockDisplay(display);
}

void
_XUnregisterFilter(display, window, filter, client_data)
    Display *display;
    Window window;
    Bool (*filter)(
#if NeedNestedPrototypes
		   Display*, Window, XEvent*, XPointer
#endif
		   );
    XPointer client_data;
{
    register XFilterEventList	*prev, fl;

    for (prev = &display->im_filters; fl = *prev; ) {
	if (fl->window == window &&
	    fl->filter == filter && fl->client_data == client_data) {
	    *prev = fl->next;
	    Xfree((char *)fl);
	} else
	    prev = &fl->next;
    }
}

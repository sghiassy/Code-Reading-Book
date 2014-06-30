/* $XConsortium: XGtSelect.c /main/10 1995/12/05 11:25:34 dpw $ */
/* $XFree86: xc/lib/Xi/XGtSelect.c,v 3.0 1996/08/25 13:52:54 dawes Exp $ */

/************************************************************

Copyright (c) 1989  X Consortium

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

Copyright (c) 1989 by Hewlett-Packard Company, Palo Alto, California.

			All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Hewlett-Packard not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
HEWLETT-PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

********************************************************/

/***********************************************************************
 *
 * XGetSelectedExtensionEvents - return a list of currently selected events.
 *
 */

#include "XI.h"
#include "XIproto.h"
#include "Xlibint.h"
#include "XInput.h"
#include "extutil.h"
#include "XIint.h"

int
XGetSelectedExtensionEvents (dpy, w, this_client_count, this_client_list, 
	all_clients_count, all_clients_list)
    register 	Display *dpy;
    Window 	w;
    int		*this_client_count;
    XEventClass	**this_client_list;
    int		*all_clients_count;
    XEventClass	**all_clients_list;
    {
    int		tlen, alen;
    register 	xGetSelectedExtensionEventsReq *req;
    xGetSelectedExtensionEventsReply rep;
    XExtDisplayInfo *info = XInput_find_display (dpy);

    LockDisplay (dpy);
    if (_XiCheckExtInit(dpy, XInput_Initial_Release) == -1)
	return (NoSuchExtension);
    GetReq(GetSelectedExtensionEvents,req);		

    req->reqType = info->codes->major_opcode;
    req->ReqType = X_GetSelectedExtensionEvents;
    req->window = w;

    if (! _XReply (dpy, (xReply *) &rep, 0, xFalse)) 
	{
	UnlockDisplay(dpy);
	SyncHandle();
	return Success;
	}

    *this_client_count = rep.this_client_count;
    *all_clients_count = rep.all_clients_count;

    if (rep.length)
	{
	int i;
	CARD32 ec;
	tlen = (*this_client_count) * sizeof(CARD32);
	alen = (rep.length << 2) - tlen;

	if (tlen)
	    {
	    *this_client_list = (XEventClass *) Xmalloc (
				    *this_client_count * sizeof(XEventClass));
	    if (!*this_client_list) 
		{
	        _XEatData (dpy, (unsigned long) tlen+alen);
		return (Success);
		}
	    for (i = 0; i < *this_client_count; i++)
	        {
		    _XRead (dpy, (char *)(&ec), sizeof(CARD32));
		    (*this_client_list)[i] = (XEventClass)ec;
		}
	    }
	else
	    *this_client_list = (XEventClass *) NULL;
	if (alen)
	    {
	    *all_clients_list = (XEventClass *) Xmalloc (
				    *all_clients_count * sizeof(XEventClass));
	    if (!*all_clients_list) 
		{
		Xfree((char *)*this_client_list);
		*this_client_list = NULL;
	        _XEatData (dpy, (unsigned long) alen);
		return (Success);
		}
	    for (i = 0; i < *all_clients_count; i++)
	        {
		    _XRead (dpy, (char *)(&ec), sizeof(CARD32));
		    (*all_clients_list)[i] = (XEventClass)ec;
		}
	    }
	else
	    *all_clients_list = (XEventClass *) NULL;

	}

    UnlockDisplay(dpy);
    SyncHandle();
    return (Success);
    }

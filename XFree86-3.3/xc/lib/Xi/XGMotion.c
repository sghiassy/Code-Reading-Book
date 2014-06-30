/* $XConsortium: XGMotion.c,v 1.15 94/04/17 20:18:01 rws Exp $ */
/* $XFree86: xc/lib/Xi/XGMotion.c,v 3.0 1996/08/25 13:52:37 dawes Exp $ */

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
 * XGetDeviceMotionEvents - Get the motion history of an input device.
 *
 */

#include "XI.h"
#include "XIproto.h"
#include "Xlibint.h"
#include "XInput.h"
#include "extutil.h"
#include "XIint.h"

XDeviceTimeCoord
*XGetDeviceMotionEvents (dpy, dev, start, stop, nEvents, mode, axis_count)
    register 	Display	*dpy;
    XDevice		*dev;
    Time 		start;
    Time 		stop;
    int 		*nEvents;
    int 		*mode;
    int 		*axis_count;
    {       
    xGetDeviceMotionEventsReq 	*req;
    xGetDeviceMotionEventsReply 	rep;
    XDeviceTimeCoord *tc;
    int *data, *bufp, *readp, *savp;
    long size, size2;
    int	 i, j;
    XExtDisplayInfo *info = XInput_find_display (dpy);

    LockDisplay (dpy);
    if (_XiCheckExtInit(dpy, XInput_Initial_Release) == -1)
	return ((XDeviceTimeCoord *) NoSuchExtension);

    GetReq(GetDeviceMotionEvents,req);		
    req->reqType = info->codes->major_opcode;
    req->ReqType = X_GetDeviceMotionEvents;
    req->start = start;
    req->stop = stop;
    req->deviceid = dev->device_id;

    if (!_XReply (dpy, (xReply *)&rep, 0, xFalse)) {
	UnlockDisplay(dpy);
        SyncHandle();
	*nEvents = 0;
	return (NULL);
	}

    *mode = rep.mode;
    *axis_count = rep.axes;
    *nEvents = rep.nEvents;
    if (!rep.nEvents)
	{
	UnlockDisplay(dpy);
        SyncHandle();
	return (NULL);
	}
    size = rep.length << 2;
    size2 = rep.nEvents * 
	(sizeof (XDeviceTimeCoord) + (rep.axes * sizeof (int)));
    savp = readp = (int *) Xmalloc (size);
    bufp = (int *) Xmalloc (size2);
    if (!bufp || !savp)
	{
	*nEvents = 0;
	_XEatData (dpy, (unsigned long) size);
	UnlockDisplay(dpy);
	SyncHandle();
	return (NULL);
	}
    _XRead (dpy, (char *) readp, size);

    tc = (XDeviceTimeCoord *) bufp;
    data = (int *) (tc + rep.nEvents);
    for (i=0; i<*nEvents; i++,tc++)
	{
	tc->time = *readp++;
	tc->data = data;
	for (j=0; j<*axis_count; j++)
	    *data++ = *readp++;
	}
    XFree ((char *)savp);
    UnlockDisplay(dpy);
    SyncHandle();
    return ((XDeviceTimeCoord *) bufp);
    }

XFreeDeviceMotionEvents (events)
    XDeviceTimeCoord *events;
    {
    XFree ((char *)events);
    }

/* $TOG: events.c /main/6 1997/05/28 12:33:20 barstow $ */
/*
 * event handling stuff
 */
/*
 
Copyright (c) 1990, 1991  X Consortium

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

 * Copyright 1990, 1991 Network Computing Devices;
 * Portions Copyright 1987 by Digital Equipment Corporation 
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Network Computing Devices, or Digital
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 *
 * NETWORK COMPUTING DEVICES, AND DIGITAL DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL NETWORK COMPUTING DEVICES,
 * OR DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/* $XFree86: xc/programs/xfs/difs/events.c,v 3.0.6.1 1997/05/29 14:01:07 dawes Exp $ */

#include	"clientstr.h"
#include	"FSproto.h"
#include	"globals.h"
#include	"fsevents.h"

extern void (*EventSwapVector[NUM_EVENT_VECTORS]) ();

static Mask lastEventMask = FontChangeNotifyMask;

#define	AllEventMasks	(lastEventMask | (lastEventMask - 1))

void
WriteErrorToClient(client, error)
    ClientPtr   client;
    fsError    *error;
{
    if (client->swapped) {
	fsError     errorTo;

	SErrorEvent(error, &errorTo);
	(void) WriteToClient(client, SIZEOF(fsError), (char *) &errorTo);
    } else {
	(void) WriteToClient(client, SIZEOF(fsError),
			     (char *) error);
    }
}

int
ProcSetEventMask(client)
    ClientPtr   client;
{
    REQUEST(fsSetEventMaskReq);
    REQUEST_AT_LEAST_SIZE(fsSetEventMaskReq);

    if (stuff->event_mask & ~AllEventMasks) {
	SendErrToClient(client, FSBadEventMask, (pointer) stuff->event_mask);
	return FSBadEventMask;
    }
    client->eventmask = stuff->event_mask;
    return client->noClientException;
}

int
ProcGetEventMask(client)
    ClientPtr   client;
{
    fsGetEventMaskReply rep;

    REQUEST(fsGetEventMaskReq);
    REQUEST_AT_LEAST_SIZE(fsGetEventMaskReq);

    rep.type = FS_Reply;
    rep.sequenceNumber = client->sequence;
    rep.length = SIZEOF(fsGetEventMaskReply) >> 2;
    rep.event_mask = client->eventmask;

    return client->noClientException;
}

void
SendKeepAliveEvent(client)
    ClientPtr   client;
{
    fsKeepAliveEvent ev;

    ev.type = FS_Event;
    ev.event_code = KeepAlive;
    ev.sequenceNumber = client->sequence;
    ev.length = SIZEOF(fsKeepAliveEvent) >> 2;
    ev.timestamp = GetTimeInMillis();

#ifdef DEBUG
    fprintf(stderr, "client #%d is getting a KeepAlive\n", client->index);
#endif

    if (client->swapped) {
	/* SErrorEvent requires two fsError pointers */
	fsError evTo;
	
	SErrorEvent((fsError *) & ev, (fsError *) &evTo);
	(void) WriteToClient(client, SIZEOF(fsKeepAliveEvent), (char *) &evTo);
    } else {
	(void) WriteToClient(client, SIZEOF(fsKeepAliveEvent), (char *) &ev);
    }
}

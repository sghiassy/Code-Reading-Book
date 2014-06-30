/* $XConsortium: queryst.c,v 1.13 94/04/17 20:33:19 dpw Exp $ */
/* $XFree86: xc/programs/Xserver/Xi/queryst.c,v 3.2 1996/04/15 11:18:59 dawes Exp $ */
/*

Copyright (c) 1988  X Consortium

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

/***********************************************************************
 *
 * Request to query the state of an extension input device.
 *
 */

#define	 NEED_EVENTS
#define	 NEED_REPLIES
#include "X.h"				/* for inputstr.h    */
#include "Xproto.h"			/* Request macro     */
#include "inputstr.h"			/* DeviceIntPtr	     */
#include "windowstr.h"			/* window structure  */
#include "XI.h"
#include "XIproto.h"
#include "extnsionst.h"
#include "extinit.h"			/* LookupDeviceIntRec */
#include "exevents.h"
#include "exglobals.h"

#include "queryst.h"

/***********************************************************************
 *
 * This procedure allows a client to query the state of a device.
 *
 */

int
SProcXQueryDeviceState(client)
    register ClientPtr client;
    {
    register char n;

    REQUEST(xQueryDeviceStateReq);
    swaps(&stuff->length, n);
    return(ProcXQueryDeviceState(client));
    }

/***********************************************************************
 *
 * This procedure allows frozen events to be routed.
 *
 */

int
ProcXQueryDeviceState(client)
    register ClientPtr client;
    {
    register char 		n;
    int 			i;
    int 			num_classes = 0;
    int 			total_length = 0;
    char			*buf, *savbuf;
    KeyClassPtr 		k;
    xKeyState			*tk;
    ButtonClassPtr 		b;
    xButtonState		*tb;
    ValuatorClassPtr 		v;
    xValuatorState		*tv;
    xQueryDeviceStateReply	rep;
    DeviceIntPtr		dev;
    int				*values;

    REQUEST(xQueryDeviceStateReq);
    REQUEST_SIZE_MATCH(xQueryDeviceStateReq);

    rep.repType = X_Reply;
    rep.RepType = X_QueryDeviceState;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;

    dev = LookupDeviceIntRec (stuff->deviceid);
    if (dev == NULL)
	{
	SendErrorToClient(client, IReqCode, X_QueryDeviceState, 0, 
		BadDevice);
	return Success;
	}

    v = dev->valuator;
    if (v != NULL && v->motionHintWindow != NULL)
	MaybeStopDeviceHint(dev, client);

    k = dev->key;
    if (k != NULL)
	{
	total_length += sizeof (xKeyState);
	num_classes++;
	}

    b = dev->button;
    if (b != NULL)
	{
	total_length += sizeof (xButtonState);
	num_classes++;
	}

    if (v != NULL)
	{
	total_length += (sizeof(xValuatorState) + 
			(v->numAxes * sizeof(int)));
	num_classes++;
	}
    buf = (char *) xalloc (total_length);
    if (!buf)
	{
	SendErrorToClient(client, IReqCode, X_QueryDeviceState, 0, 
		BadAlloc);
	return Success;
	}
    savbuf = buf;

    if (k != NULL)
	{
	tk = (xKeyState *) buf;
	tk->class = KeyClass;
	tk->length = sizeof (xKeyState);
	tk->num_keys = k->curKeySyms.maxKeyCode - k->curKeySyms.minKeyCode + 1;
	for (i = 0; i<32; i++)
	    tk->keys[i] = k->down[i];
	buf += sizeof (xKeyState);
	}

    if (b != NULL)
	{
	tb = (xButtonState *) buf;
	tb->class = ButtonClass;
	tb->length = sizeof (xButtonState);
	tb->num_buttons = b->numButtons;
	for (i = 0; i<32; i++)
	    tb->buttons[i] = b->down[i];
	buf += sizeof (xButtonState);
	}

    if (v != NULL)
	{
	tv = (xValuatorState *) buf;
	tv->class = ValuatorClass;
	tv->length = sizeof (xValuatorState);
	tv->num_valuators = v->numAxes;
	tv->mode = v->mode;
	buf += sizeof(xValuatorState);
	for (i=0, values=v->axisVal; i<v->numAxes; i++)
	    {
	    *((int *) buf) = *values++;
	    if (client->swapped)
		{
		swapl ((int *) buf, n);/* macro - braces needed */
		}
	    buf += sizeof(int);
	    }
	}

    rep.num_classes = num_classes;
    rep.length = (total_length + 3) >> 2;
    WriteReplyToClient (client, sizeof(xQueryDeviceStateReply), &rep);
    if (total_length > 0)
	WriteToClient (client, total_length, savbuf);
    xfree (savbuf);
    return Success;
    }

/***********************************************************************
 *
 * This procedure writes the reply for the XQueryDeviceState function,
 * if the client and server have a different byte ordering.
 *
 */

void
SRepXQueryDeviceState (client, size, rep)
    ClientPtr	client;
    int		size;
    xQueryDeviceStateReply	*rep;
    {
    register char n;

    swaps(&rep->sequenceNumber, n);
    swapl(&rep->length, n);
    WriteToClient(client, size, (char *)rep);
    }

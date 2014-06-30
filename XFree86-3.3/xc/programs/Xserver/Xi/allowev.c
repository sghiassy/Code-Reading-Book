/* $XConsortium: allowev.c,v 1.8 94/04/17 20:33:02 rws Exp $ */
/* $XFree86: xc/programs/Xserver/Xi/allowev.c,v 3.1 1996/04/15 11:18:22 dawes Exp $ */

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
 * Function to allow frozen events to be routed from extension input devices.
 *
 */

#define	 NEED_EVENTS
#define	 NEED_REPLIES
#include "X.h"				/* for inputstr.h    */
#include "Xproto.h"			/* Request macro     */
#include "inputstr.h"			/* DeviceIntPtr	     */
#include "XI.h"
#include "XIproto.h"

#include "extnsionst.h"
#include "extinit.h"			/* LookupDeviceIntRec */
#include "exglobals.h"

#include "allowev.h"
#include "dixevents.h"

/***********************************************************************
 *
 * This procedure allows frozen events to be routed.
 *
 */

int
SProcXAllowDeviceEvents(client)
    register ClientPtr client;
    {
    register char n;

    REQUEST(xAllowDeviceEventsReq);
    swaps(&stuff->length, n);
    REQUEST_SIZE_MATCH(xAllowDeviceEventsReq);
    swapl(&stuff->time, n);
    return(ProcXAllowDeviceEvents(client));
    }

/***********************************************************************
 *
 * This procedure allows frozen events to be routed.
 *
 */

int
ProcXAllowDeviceEvents(client)
    register ClientPtr client;
    {
    TimeStamp		time;
    DeviceIntPtr	thisdev;

    REQUEST(xAllowDeviceEventsReq);
    REQUEST_SIZE_MATCH(xAllowDeviceEventsReq);

    thisdev = LookupDeviceIntRec (stuff->deviceid);
    if (thisdev == NULL)
	{
	SendErrorToClient(client, IReqCode, X_AllowDeviceEvents, 0, BadDevice);
	return Success;
	}
    time = ClientTimeToServerTime(stuff->time);

    switch (stuff->mode)
        {
	case ReplayThisDevice:
	    AllowSome(client, time, thisdev, NOT_GRABBED);
	    break;
	case SyncThisDevice: 
	    AllowSome(client, time, thisdev, FREEZE_NEXT_EVENT);
	    break;
	case AsyncThisDevice: 
	    AllowSome(client, time, thisdev, THAWED);
	    break;
	case AsyncOtherDevices: 
	    AllowSome(client, time, thisdev, THAW_OTHERS);
	    break;
	case SyncAll:
	    AllowSome(client, time, thisdev, FREEZE_BOTH_NEXT_EVENT);
	    break;
	case AsyncAll:
	    AllowSome(client, time, thisdev, THAWED_BOTH);
	    break;
	default: 
	    SendErrorToClient(client, IReqCode, X_AllowDeviceEvents, 0, 
		BadValue);
	    client->errorValue = stuff->mode;
	    return Success;
        }
    return Success;
    }

/* $XConsortium: ungrdevk.c,v 1.10 94/04/17 20:33:25 rws Exp $ */
/* $XFree86: xc/programs/Xserver/Xi/ungrdevk.c,v 3.1 1996/04/15 11:19:11 dawes Exp $ */

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
 * Request to release a grab of a key on an extension device.
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
#include "exglobals.h"
#include "dixgrabs.h"

#include "ungrdevk.h"

#define AllModifiersMask ( \
	ShiftMask | LockMask | ControlMask | Mod1Mask | Mod2Mask | \
	Mod3Mask | Mod4Mask | Mod5Mask )

/***********************************************************************
 *
 * Handle requests from a client with a different byte order.
 *
 */

int
SProcXUngrabDeviceKey(client)
    register ClientPtr client;
    {
    register char n;

    REQUEST(xUngrabDeviceKeyReq);
    swaps(&stuff->length, n);
    REQUEST_SIZE_MATCH(xUngrabDeviceKeyReq);
    swapl(&stuff->grabWindow, n);
    swaps(&stuff->modifiers, n);
    return(ProcXUngrabDeviceKey(client));
    }

/***********************************************************************
 *
 * Release a grab of a key on an extension device.
 *
 */

int
ProcXUngrabDeviceKey(client)
    ClientPtr client;
    {
    DeviceIntPtr	dev;
    DeviceIntPtr	mdev;
    WindowPtr 		pWin;
    GrabRec 		temporaryGrab;

    REQUEST(xUngrabDeviceKeyReq);
    REQUEST_SIZE_MATCH(xUngrabDeviceKeyReq);

    dev = LookupDeviceIntRec (stuff->grabbed_device);
    if (dev == NULL)
	{
	SendErrorToClient(client, IReqCode, X_UngrabDeviceKey, 0, 
	    BadDevice);
	return Success;
	}
    if (dev->key == NULL)
	{
	SendErrorToClient(client, IReqCode, X_UngrabDeviceKey, 0, BadMatch);
	return Success;
	}

    if (stuff->modifier_device != UseXKeyboard)
	{
	mdev = LookupDeviceIntRec (stuff->modifier_device);
	if (mdev == NULL)
	    {
	    SendErrorToClient(client, IReqCode, X_UngrabDeviceKey, 0, 
	        BadDevice);
	    return Success;
	    }
	if (mdev->key == NULL)
	    {
	    SendErrorToClient(client, IReqCode, X_UngrabDeviceKey, 0, 
		BadMatch);
	    return Success;
	    }
	}
    else
	mdev = (DeviceIntPtr) LookupKeyboardDevice();

    pWin = LookupWindow(stuff->grabWindow, client);
    if (!pWin)
	{
	SendErrorToClient(client, IReqCode, X_UngrabDeviceKey, 0, 
	    BadWindow);
	return Success;
	}
    if (((stuff->key > dev->key->curKeySyms.maxKeyCode) ||
	 (stuff->key < dev->key->curKeySyms.minKeyCode))
	&& (stuff->key != AnyKey))
	{
	SendErrorToClient(client, IReqCode, X_UngrabDeviceKey, 0, 
	    BadValue);
	return Success;
	}
    if ((stuff->modifiers != AnyModifier) &&
	(stuff->modifiers & ~AllModifiersMask))
	{
	SendErrorToClient(client, IReqCode, X_UngrabDeviceKey, 0, 
	    BadValue);
	return Success;
	}

    temporaryGrab.resource = client->clientAsMask;
    temporaryGrab.device = dev;
    temporaryGrab.window = pWin;
    temporaryGrab.type  = DeviceKeyPress;
    temporaryGrab.modifierDevice = mdev;
    temporaryGrab.modifiersDetail.exact = stuff->modifiers;
    temporaryGrab.modifiersDetail.pMask = NULL;
    temporaryGrab.detail.exact = stuff->key;
    temporaryGrab.detail.pMask = NULL;

    DeletePassiveGrabFromList(&temporaryGrab);
    return Success;
    }

/* $XConsortium: replywait.c,v 1.4 94/04/17 20:15:39 mor Exp $ */
/******************************************************************************


Copyright (c) 1993  X Consortium

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

Author: Ralph Mor, X Consortium
******************************************************************************/

#include <X11/ICE/ICElib.h>
#include "ICElibint.h"


void
_IceAddReplyWait (iceConn, replyWait)

IceConn			iceConn;
IceReplyWaitInfo	*replyWait;

{
    /*
     * Add this replyWait to the end of the list (only if the
     * replyWait is not already in the list).
     */

    _IceSavedReplyWait	*savedReplyWait;
    _IceSavedReplyWait	*prev, *last;

    prev = NULL;
    last = iceConn->saved_reply_waits;

    while (last)
    {
	if (last->reply_wait == replyWait)
	    return;

	prev = last;
	last = last->next;
    }
	
    savedReplyWait = (_IceSavedReplyWait *) malloc (
	sizeof (_IceSavedReplyWait));

    savedReplyWait->reply_wait = replyWait;
    savedReplyWait->reply_ready = False;
    savedReplyWait->next = NULL;

    if (prev == NULL)
	iceConn->saved_reply_waits = savedReplyWait;
    else
	prev->next = savedReplyWait;
}



IceReplyWaitInfo *
_IceSearchReplyWaits (iceConn, majorOpcode)

IceConn	iceConn;
int	majorOpcode;

{
    /*
     * Return the first replyWait in the list with the given majorOpcode
     */

    _IceSavedReplyWait	*savedReplyWait = iceConn->saved_reply_waits;

    while (savedReplyWait && !savedReplyWait->reply_ready &&
	savedReplyWait->reply_wait->major_opcode_of_request != majorOpcode)
    {
	savedReplyWait = savedReplyWait->next;
    }

    return (savedReplyWait ? savedReplyWait->reply_wait : NULL);
}



void
_IceSetReplyReady (iceConn, replyWait)

IceConn			iceConn;
IceReplyWaitInfo	*replyWait;

{
    /*
     * The replyWait specified has a reply ready.
     */

    _IceSavedReplyWait	*savedReplyWait = iceConn->saved_reply_waits;

    while (savedReplyWait && savedReplyWait->reply_wait != replyWait)
	savedReplyWait = savedReplyWait->next;

    if (savedReplyWait)
	savedReplyWait->reply_ready = True;
}



Bool
_IceCheckReplyReady (iceConn, replyWait)

IceConn			iceConn;
IceReplyWaitInfo	*replyWait;

{
    _IceSavedReplyWait	*savedReplyWait = iceConn->saved_reply_waits;
    _IceSavedReplyWait	*prev = NULL;
    Bool		found = False;
    Bool		ready;

    while (savedReplyWait && !found)
    {
	if (savedReplyWait->reply_wait == replyWait)
	    found = True;
	else
	{
	    prev = savedReplyWait;
	    savedReplyWait = savedReplyWait->next;
	}
    }

    ready = found && savedReplyWait->reply_ready;

    if (ready)
    {
	if (prev == NULL)
	    iceConn->saved_reply_waits = savedReplyWait->next;
	else
	    prev->next = savedReplyWait->next;
	
	free ((char *) savedReplyWait);
    }

    return (ready);
}

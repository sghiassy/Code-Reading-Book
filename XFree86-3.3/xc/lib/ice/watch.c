/* $XConsortium: watch.c,v 1.6 94/04/17 20:15:41 mor Exp $ */
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


Status
IceAddConnectionWatch (watchProc, clientData)

IceWatchProc	watchProc;
IcePointer	clientData;

{
    /*
     * watchProc will be called each time an ICE connection is
     * created/destroyed by ICElib.
     */

    _IceWatchProc	*ptr = _IceWatchProcs;
    _IceWatchProc	*newWatchProc;
    int			i;

    if ((newWatchProc = (_IceWatchProc *) malloc (
	sizeof (_IceWatchProc))) == NULL)
    {
	return (0);
    }

    newWatchProc->watch_proc = watchProc;
    newWatchProc->client_data = clientData;
    newWatchProc->watched_connections = NULL;
    newWatchProc->next = NULL;

    while (ptr && ptr->next)
	ptr = ptr->next;

    if (ptr == NULL)
	_IceWatchProcs = newWatchProc;
    else
	ptr->next = newWatchProc;


    /*
     * Invoke the watch proc with any previously opened ICE connections.
     */
     
    for (i = 0; i < _IceConnectionCount; i++)
    {
	_IceWatchedConnection *newWatchedConn = (_IceWatchedConnection *)
	    malloc (sizeof (_IceWatchedConnection));

	newWatchedConn->iceConn = _IceConnectionObjs[i];
	newWatchedConn->next = NULL;

	newWatchProc->watched_connections = newWatchedConn;

	(*newWatchProc->watch_proc) (_IceConnectionObjs[i],
	    newWatchProc->client_data, True, &newWatchedConn->watch_data);
    }

    return (1);
}



void
IceRemoveConnectionWatch (watchProc, clientData)

IceWatchProc	watchProc;
IcePointer	clientData;

{
    _IceWatchProc	*currWatchProc = _IceWatchProcs;
    _IceWatchProc	*prevWatchProc = NULL;

    while (currWatchProc && (currWatchProc->watch_proc != watchProc ||
        currWatchProc->client_data != clientData))
    {
	prevWatchProc = currWatchProc;
	currWatchProc = currWatchProc->next;
    }

    if (currWatchProc)
    {
	_IceWatchProc		*nextWatchProc = currWatchProc->next;
	_IceWatchedConnection 	*watchedConn;

	watchedConn = currWatchProc->watched_connections;
	while (watchedConn)
	{
	    _IceWatchedConnection *nextWatchedConn = watchedConn->next;
	    free ((char *) watchedConn);
	    watchedConn = nextWatchedConn;
	}

	if (prevWatchProc == NULL)
	    _IceWatchProcs = nextWatchProc;
	else
	    prevWatchProc->next = nextWatchProc;

	free ((char *) currWatchProc);
    }
}



void
_IceConnectionOpened (iceConn)

IceConn	iceConn;

{
    _IceWatchProc *watchProc = _IceWatchProcs;

    while (watchProc)
    {
	_IceWatchedConnection *newWatchedConn = (_IceWatchedConnection *)
	    malloc (sizeof (_IceWatchedConnection));
	_IceWatchedConnection *watchedConn;

	watchedConn = watchProc->watched_connections;
	while (watchedConn && watchedConn->next)
	    watchedConn = watchedConn->next;

	newWatchedConn->iceConn = iceConn;
	newWatchedConn->next = NULL;

	if (watchedConn == NULL)
	    watchProc->watched_connections = newWatchedConn;
	else
	    watchedConn->next = newWatchedConn;

	(*watchProc->watch_proc) (iceConn,
	    watchProc->client_data, True, &newWatchedConn->watch_data);

	watchProc = watchProc->next;
    }
}



void
_IceConnectionClosed (iceConn)

IceConn	iceConn;

{
    _IceWatchProc *watchProc = _IceWatchProcs;

    while (watchProc)
    {
	_IceWatchedConnection *watchedConn = watchProc->watched_connections;
	_IceWatchedConnection *prev = NULL;

	while (watchedConn && watchedConn->iceConn != iceConn)
	{
	    prev = watchedConn;
	    watchedConn = watchedConn->next;
	}

	if (watchedConn)
	{
	    (*watchProc->watch_proc) (iceConn,
	        watchProc->client_data, False, &watchedConn->watch_data);

	    if (prev == NULL)
		watchProc->watched_connections = watchedConn->next;
	    else
		prev->next = watchedConn->next;

	    free ((char *) watchedConn);
	}

	watchProc = watchProc->next;
    }
}

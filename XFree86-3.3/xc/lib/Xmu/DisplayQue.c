/* $XConsortium: DisplayQue.c,v 1.6 94/04/17 20:15:58 rws Exp $ */
/* $XFree86: xc/lib/Xmu/DisplayQue.c,v 3.0 1996/05/06 05:54:31 dawes Exp $ */

/*
 
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

*/

/*
 * Author:  Jim Fulton, MIT X Consortium
 */

#include <stdio.h>
#include <X11/Xlib.h>
#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#endif
#include <X11/Xmu/DisplayQue.h>

static int _DQCloseDisplay();

#define CallCloseCallback(q,e) (void) (*((q)->closefunc)) ((q), (e))
#define CallFreeCallback(q) (void) (*((q)->freefunc)) ((q))

/*
 * XmuDQCreate - create a display queue
 */
XmuDisplayQueue *XmuDQCreate (closefunc, freefunc, data)
    XmuCloseDisplayQueueProc closefunc;
    XmuFreeDisplayQueueProc freefunc;
    XPointer data;
{
    XmuDisplayQueue *q = (XmuDisplayQueue *) malloc (sizeof (XmuDisplayQueue));
    if (q) {
	q->nentries = 0;
	q->head = q->tail = NULL;
	q->closefunc = closefunc;
	q->freefunc = freefunc;
	q->data = data;
    }
    return q;
}


/*
 * XmuDQDestroy - free all storage associated with this display queue, 
 * optionally invoking the close callbacks.
 */

Bool XmuDQDestroy (q, docallbacks)
    XmuDisplayQueue *q;
    Bool docallbacks;
{
    XmuDisplayQueueEntry *e = q->head;

    while (e) {
	XmuDisplayQueueEntry *nexte = e->next;
	if (docallbacks && q->closefunc) CallCloseCallback (q, e);
	free ((char *) e);
	e = nexte;
    }
    free ((char *) q);
    return True;
}


/*
 * XmuDQLookupDisplay - finds the indicated display on the given queue
 */
XmuDisplayQueueEntry *XmuDQLookupDisplay (q, dpy)
    XmuDisplayQueue *q;
    Display *dpy;
{
    XmuDisplayQueueEntry *e;

    for (e = q->head; e; e = e->next) {
	if (e->display == dpy) return e;
    }
    return NULL;
}


/*
 * XmuDQAddDisplay - add the specified display to the queue; set data as a
 * convenience.  Does not ensure that dpy hasn't already been added.
 */
XmuDisplayQueueEntry *XmuDQAddDisplay (q, dpy, data)
    XmuDisplayQueue *q;
    Display *dpy;
    XPointer data;
{
    XmuDisplayQueueEntry *e;

    if (!(e = (XmuDisplayQueueEntry *) malloc (sizeof (XmuDisplayQueueEntry)))) {
	return NULL;
    }
    if (!(e->closehook = XmuAddCloseDisplayHook (dpy, _DQCloseDisplay,
						 (XPointer) q))) {
	free ((char *) e);
	return NULL;
    }

    e->display = dpy;
    e->next = NULL;
    e->data = data;

    if (q->tail) {
	q->tail->next = e;
	e->prev = q->tail;
    } else {
	q->head = e;
	e->prev = NULL;
    }
    q->tail = e;
    q->nentries++;
    return e;
}


/*
 * XmuDQRemoveDisplay - remove the specified display from the queue
 */
Bool XmuDQRemoveDisplay (q, dpy)
    XmuDisplayQueue *q;
    Display *dpy;
{
    XmuDisplayQueueEntry *e;

    for (e = q->head; e; e = e->next) {
	if (e->display == dpy) {
	    if (q->head == e)
	      q->head = e->next;	/* if at head, then bump head */
	    else
	      e->prev->next = e->next;	/* else splice out */
	    if (q->tail == e)
	      q->tail = e->prev;	/* if at tail, then bump tail */
	    else
	      e->next->prev = e->prev;	/* else splice out */
	    (void) XmuRemoveCloseDisplayHook (dpy, e->closehook,
					      _DQCloseDisplay, (XPointer) q);
	    free ((char *) e);
	    q->nentries--;
	    return True;
	}
    }
    return False;
}


/*****************************************************************************
 *			       private functions                             *
 *****************************************************************************/

/*
 * _DQCloseDisplay - upcalled from CloseHook to notify this queue; remove the
 * display when finished
 */
static int _DQCloseDisplay (dpy, arg)
    Display *dpy;
    XPointer arg;
{
    XmuDisplayQueue *q = (XmuDisplayQueue *) arg;
    XmuDisplayQueueEntry *e;

    for (e = q->head; e; e = e->next) {
	if (e->display == dpy) {
	    if (q->closefunc) CallCloseCallback (q, e);
	    (void) XmuDQRemoveDisplay (q, dpy);
	    if (q->nentries == 0 && q->freefunc) CallFreeCallback (q);
	    return 1;
	}
    }

    return 0;
}

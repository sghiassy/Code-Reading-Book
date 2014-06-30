/* $XConsortium: Handlers.c,v 1.14 94/04/17 20:23:45 kaleb Exp $ */
/*

Copyright (c) 1989  X Consortium

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

/*
 * Author:  Davor Matic, MIT X Consortium
 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "BitmapP.h"
    
#include <stdio.h>
#include <math.h>

#ifndef abs
#define abs(x)                        (((int)(x) > 0) ? (x) : -(x))
#endif
#define min(x, y)                     (((int)(x) < (int)(y)) ? (x) : (y))
#define max(x, y)                     (((int)(x) > (int)(y)) ? (x) : (y))

#include "Requests.h"

extern Boolean DEBUG;

/*****************************************************************************
 *                                  Handlers                                 *
 *****************************************************************************/

#define QueryInSquare(BW, x, y, square_x, square_y)\
    ((InBitmapX(BW, x) == (square_x)) &&\
     (InBitmapY(BW, y) == (square_y)))


void DragOnePointHandler(w, client_data, event, cont) /* ARGSUSED */
     Widget       w;
     XtPointer    client_data;
     XEvent      *event;
     Boolean     *cont;
{
    BWStatus *status = (BWStatus *)client_data;
    BitmapWidget BW = (BitmapWidget) w;

    if (DEBUG)
	fprintf(stderr, "D1PH ");

    switch (event->type) {
    
    case ButtonPress:
	if (event->xbutton.state != status->state) return;
	if (!QuerySet(status->at_x, status->at_y)) {
	    BWStoreToBuffer(w);
	    status->value = Value(BW, event->xbutton.button);
	    status->btime = event->xbutton.time;
	    status->at_x = InBitmapX(BW, event->xbutton.x);
	    status->at_y = InBitmapY(BW, event->xbutton.y);
	    status->success = status->draw ? True : False;
	    if (status->draw)
		(*status->draw)(w,
				status->at_x, status->at_y, status->value);
	}
	break;
	
    case ButtonRelease:
	if (QuerySet(status->at_x, status->at_y)) {
	    status->value = Value(BW, event->xbutton.button);
	    status->btime = event->xbutton.time;
	    status->at_x = InBitmapX(BW, event->xbutton.x);
	    status->at_y = InBitmapY(BW, event->xbutton.y);
	    status->success = status->draw ? True : False;
	    /* SUPPRESS 701 */
	    BWTerminateRequest(w, TRUE); 
	}
	break;

    case MotionNotify:
	if (QuerySet(status->at_x, status->at_y)) {
	    if (!QueryInSquare(BW, event->xmotion.x, event->xmotion.y,
			       status->at_x, status->at_y)) {
		status->at_x = InBitmapX(BW, event->xmotion.x);
		status->at_y = InBitmapY(BW, event->xmotion.y);
		if (status->draw)
		    (*status->draw)(w,
				    status->at_x, status->at_y, status->value);
	    }
	}
	break;

    }
}

void DragOnePointEngage(w, status, draw, state)
    Widget      w;
    BWStatus   *status;
    void      (*draw)();
    int        *state;
{
    
    status->at_x = NotSet;
    status->at_y = NotSet;
    status->draw = draw;
    status->success = False;
    status->state = *state;
    
    XtAddEventHandler(w,
		      ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
		      FALSE, DragOnePointHandler, (XtPointer)status);
}

/* ARGSUSED */
void DragOnePointTerminate(w, status, client_data)
    Widget     w;
    BWStatus  *status;
    XtPointer  client_data;
{
    
    if (status->success) {
	BWChangeNotify(w, NULL, NULL);
	BWSetChanged(w);
    }
    
    XtRemoveEventHandler(w,
		 ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
		 FALSE, DragOnePointHandler, (XtPointer)status);
    
}

void OnePointHandler(w, client_data, event, cont) /* ARGSUSED */
    Widget       w;
    XtPointer    client_data;
    XEvent      *event;
    Boolean     *cont;
{
    BWStatus    *status = (BWStatus *)client_data;
    BitmapWidget BW = (BitmapWidget) w;
    
    if (DEBUG)
	fprintf(stderr, "1PH ");

    switch (event->type) {
	
    case Expose:
	if (QuerySet(status->at_x, status->at_y)) {
	    BWClip(w, event->xexpose.x, event->xexpose.y,
		   event->xexpose.width, event->xexpose.height);
	    if (status->draw)
		(*status->draw)(w,
				status->at_x, status->at_y, Highlight);
	    
	    BWUnclip(w);
	}
	break;
	
    case ButtonPress:
	if (event->xbutton.state != status->state) return;
	if (!QuerySet(status->at_x, status->at_y)) {
	    status->value = Value(BW, event->xbutton.button);
	    status->btime = event->xbutton.time;
	    status->at_x = InBitmapX(BW, event->xbutton.x);
	    status->at_y = InBitmapY(BW, event->xbutton.y);
	    if (status->draw)
		(*status->draw)(w,
				status->at_x, status->at_y, Highlight);
	}
	break;
	
    case ButtonRelease:
	if (QuerySet(status->at_x, status->at_y)) {
	    if (status->draw)
		(*status->draw)(w,
				status->at_x, status->at_y, Highlight);
	    
	    status->value = Value(BW, event->xbutton.button);
	    status->btime = event->xbutton.time;
	    status->at_x = InBitmapX(BW, event->xbutton.x);
	    status->at_y = InBitmapY(BW, event->xbutton.y);
	    status->success = True;
	    
	    BWTerminateRequest(w, TRUE);
	}
	break;
	
    case MotionNotify:
	if (QuerySet(status->at_x, status->at_y)) {
	    if (!QueryInSquare(BW, event->xmotion.x, event->xmotion.y,
			       status->at_x, status->at_y)) {
		if (status->draw)
		    (*status->draw)(w,
				    status->at_x, status->at_y, Highlight);
		status->at_x = InBitmapX(BW, event->xmotion.x);
		status->at_y = InBitmapY(BW, event->xmotion.y);
		if (status->draw)
		    (*status->draw)(w,
				    status->at_x, status->at_y, Highlight);
	    }
	}      
	break;
    }
}

void OnePointEngage(w, status, draw, state)
    Widget      w;
    BWStatus   *status;
    void      (*draw)();
    int        *state;
{
    status->at_x = NotSet;
    status->at_y = NotSet;
    status->draw = draw;
    status->success = False;
    status->state = *state;

    XtAddEventHandler(w,
		      ButtonPressMask | ButtonReleaseMask | 
		      ExposureMask | PointerMotionMask,
		      FALSE, OnePointHandler, (XtPointer)status);
}

void OnePointImmediateEngage(w, status, draw, state)
    Widget      w;
    BWStatus   *status;
    void      (*draw)();
    int        *state;
{
    status->at_x = 0;
    status->at_y = 0;
    status->draw = draw;
    status->success = False;
    status->state = *state;
    
    if (status->draw)
	(*status->draw)(w,
			status->at_x, status->at_y, Highlight);
    
    XtAddEventHandler(w,
		      ButtonPressMask | ButtonReleaseMask | 
		      ExposureMask | PointerMotionMask,
		      FALSE, OnePointHandler, (XtPointer)status);
}

void OnePointTerminate(w, status, draw)
    Widget     w;
    BWStatus  *status;
    void     (*draw)();
{
    
    if (status->success && draw) {
	BWStoreToBuffer(w);
	(*draw)(w,
		status->at_x, status->at_y,
		status->value);
	BWChangeNotify(w, NULL, NULL);
	BWSetChanged(w);
    }    
    else
	if (QuerySet(status->at_x, status->at_y))
	    if (status->draw)
		(*status->draw)(w,
				status->at_x, status->at_y, Highlight);
    
    XtRemoveEventHandler(w,
			 ButtonPressMask | ButtonReleaseMask | 
			 ExposureMask | PointerMotionMask,
			 FALSE, OnePointHandler, (XtPointer)status);
}

void OnePointTerminateTransparent(w, status, draw)
    Widget     w;
    BWStatus  *status;
    void     (*draw)();
{
    
    if (status->success && draw)
	(*draw)(w,
		status->at_x, status->at_y,
		status->value);
    else
	if (QuerySet(status->at_x, status->at_y))
	    if (status->draw)
		(*status->draw)(w,
				status->at_x, status->at_y, Highlight);
    
    XtRemoveEventHandler(w,
			 ButtonPressMask | ButtonReleaseMask | 
			 ExposureMask | PointerMotionMask,
			 FALSE, OnePointHandler, (XtPointer)status);
    
}


void TwoPointsHandler(w, client_data, event, cont) /* ARGSUSED */
    Widget      w;
    XtPointer  client_data;
    XEvent     *event;
    Boolean    *cont;
{
    BitmapWidget BW = (BitmapWidget) w;
    BWStatus   *status = (BWStatus *)client_data;
    if (DEBUG)
	fprintf(stderr, "2PH ");
    
    switch (event->type) {
	
    case Expose:
	if (QuerySet(status->from_x, status->from_y) && 
	    QuerySet(status->to_x, status->to_y)) {
	    BWClip(w, event->xexpose.x, event->xexpose.y,
		   event->xexpose.width, event->xexpose.height);
	    if (status->draw)
		(*status->draw)(w,
				status->from_x, status->from_y, 
				status->to_x, status->to_y, Highlight);
	    BWUnclip(w);
	}
	break;
	
    case ButtonPress:
	if (event->xbutton.state != status->state) return;
	if (!QuerySet(status->from_x, status->from_y)) {
	    status->value = Value(BW, event->xbutton.button);
	    status->btime = event->xbutton.time;
	    status->from_x = InBitmapX(BW, event->xbutton.x);
	    status->from_y = InBitmapY(BW, event->xbutton.y);
	    status->to_x = InBitmapX(BW, event->xbutton.x);
	    status->to_y = InBitmapY(BW, event->xbutton.y);
	    if (status->draw)
		(*status->draw)(w,
				status->from_x, status->from_y, 
				status->to_x, status->to_y, Highlight);
	}
	break;
	
    case ButtonRelease:
	if (QuerySet(status->from_x, status->from_y)) {
	    if (status->draw)
		(*status->draw)(w,
				status->from_x, status->from_y, 
				status->to_x, status->to_y, Highlight);
	    status->value = Value(BW, event->xbutton.button);
	    status->btime = event->xbutton.time;	    
	    status->to_x = InBitmapX(BW, event->xbutton.x);
	    status->to_y = InBitmapY(BW, event->xbutton.y);
	    status->success = True;
	    
	    BWTerminateRequest(w, TRUE);
	}
	break;
	
    case MotionNotify:
	if (QuerySet(status->from_x, status->from_y)) {
	    if (QuerySet(status->to_x, status->to_y)) {
		if (!QueryInSquare(BW, event->xmotion.x, event->xmotion.y,
				   status->to_x, status->to_y)) {
		    if (status->draw)
			(*status->draw)(w,
					status->from_x, status->from_y, 
					status->to_x, status->to_y, Highlight);
		    status->to_x = InBitmapX(BW, event->xmotion.x);
		    status->to_y = InBitmapY(BW, event->xmotion.y);
		    if (status->draw)
			(*status->draw)(w,
					status->from_x, status->from_y, 
					status->to_x, status->to_y, Highlight);
		}
	    }
	    else {
		status->to_x = InBitmapX(BW, event->xmotion.x);
		status->to_y = InBitmapY(BW, event->xmotion.y);
		if (status->draw)
		    (*status->draw)(w,
				    status->from_x, status->from_y, 
				    status->to_x, status->to_y, Highlight);
	    }
	}
	break;
    }
}

void TwoPointsEngage(w, status, draw, state)
    Widget     w;
    BWStatus  *status;
    void     (*draw)();
    int       *state;
{
    
    status->from_x = NotSet;
    status->from_y = NotSet;
    status->to_x = NotSet;
    status->to_y = NotSet;
    status->draw = draw;
    status->success = False;
    status->state = *state;

    XtAddEventHandler(w,
		      ButtonPressMask | ButtonReleaseMask | 
		      ExposureMask | PointerMotionMask,
		      FALSE, TwoPointsHandler, (XtPointer)status);
}

void TwoPointsTerminate(w, status, draw)
    Widget    w;
    BWStatus *status;
    void    (*draw)();
{
    
    if (status->success && draw) {
	BWStoreToBuffer(w);
	(*draw)(w,
		status->from_x, status->from_y,
		status->to_x, status->to_y,
		status->value);
	BWChangeNotify(w, NULL, NULL);
	BWSetChanged(w);
    }
    else
	if (QuerySet(status->from_x, status->from_y) && 
	    QuerySet(status->to_x, status->to_y))
	    if (status->draw)
		(*status->draw)(w,
				status->from_x, status->from_y, 
				status->to_x, status->to_y, Highlight);
    
    XtRemoveEventHandler(w,
			 ButtonPressMask | ButtonReleaseMask | 
			 ExposureMask | PointerMotionMask,
			 FALSE, TwoPointsHandler, (XtPointer)status);
}

void TwoPointsTerminateTransparent(w, status, draw)
    Widget    w;
    BWStatus *status;
    void    (*draw)();
{
    
    if (status->success && draw)
	(*draw)(w,
		status->from_x, status->from_y,
		status->to_x, status->to_y,
		status->value);
    else
	if (QuerySet(status->from_x, status->from_y) && 
	    QuerySet(status->to_x, status->to_y))
	    if (status->draw)
		(*status->draw)(w,
				status->from_x, status->from_y, 
				status->to_x, status->to_y, Highlight);
    
    XtRemoveEventHandler(w,
			 ButtonPressMask | ButtonReleaseMask | 
			 ExposureMask | PointerMotionMask,
			 FALSE, TwoPointsHandler, (XtPointer)status);
}

void TwoPointsTerminateTimed(w, status, draw)
    Widget    w;
    BWStatus *status;
    void    (*draw)();
{
    
    if (status->success && draw)
	(*draw)(w,
		status->from_x, status->from_y,
		status->to_x, status->to_y,
		status->btime);
    else
	if (QuerySet(status->from_x, status->from_y) && 
	    QuerySet(status->to_x, status->to_y))
	    if (status->draw)
		(*status->draw)(w,
				status->from_x, status->from_y, 
				status->to_x, status->to_y, Highlight);
    
    XtRemoveEventHandler(w,
			 ButtonPressMask | ButtonReleaseMask | 
			 ExposureMask | PointerMotionMask,
			 FALSE, TwoPointsHandler, (XtPointer)status);
}

/* ARGSUSED */
void Interface(w, status, action)
    Widget     w;
    XtPointer  status;
    void     (*action)();
{
 	(*action)(w);
}

void Paste(w, at_x, at_y, value)
    Widget    w;
    Position  at_x, at_y;
    int       value;
{
    BitmapWidget    BW = (BitmapWidget) w;
    BWStatus       *my_status;
    BWRequest       request;

    my_status = (BWStatus *) 
	BW->bitmap.request_stack[BW->bitmap.current].status;

    my_status->draw = NULL;

   request = (BWRequest)
   BW->bitmap.request_stack[BW->bitmap.current].request->terminate_client_data;
	
    BWTerminateRequest(w, FALSE);
    
    if ((at_x == max(BW->bitmap.mark.from_x, min(at_x, BW->bitmap.mark.to_x)))
	&&
      (at_y == max(BW->bitmap.mark.from_y, min(at_y, BW->bitmap.mark.to_y)))) {
	
	BWStatus *status;
	
	if (DEBUG)
	    fprintf(stderr, "Prepaste request: %s\n", request);
	
	BWEngageRequest(w, request, False, (char *)&(my_status->state), sizeof(int));
	
	status = (BWStatus *) 
	    BW->bitmap.request_stack[BW->bitmap.current].status;
	
	status->at_x = at_x;
	status->at_y = at_y;
	status->value = value;
	(*status->draw) (w, at_x, at_y, Highlight);	
    }
    else {

	BWStatus *status;
	
      BWEngageRequest(w, MarkRequest, False, (char *)&(my_status->state), sizeof(int));
	
	status = (BWStatus *) 
	    BW->bitmap.request_stack[BW->bitmap.current].status;
	
	status->from_x = status->to_x = at_x;
	status->from_y = status->to_y = at_y;
	status->value = value;
	(*status->draw) (w, at_x, at_y, at_x, at_y, Highlight);
    }
}


void DragTwoPointsHandler(w, client_data, event, cont) /* ARGSUSED */
    Widget      w;
    XtPointer   client_data;
    XEvent     *event;
    Boolean    *cont;
{
    BitmapWidget BW = (BitmapWidget) w;
    BWStatus   *status = (BWStatus *)client_data;

    if (DEBUG)
	fprintf(stderr, "D2PH ");

    switch (event->type) {
	
    case ButtonPress:
	if (event->xbutton.state != status->state) return;
	if (!QuerySet(status->from_x, status->from_y)) {
	    BWStoreToBuffer(w);
	    status->value = Value(BW, event->xbutton.button);
	    status->btime = event->xbutton.time;
	    status->from_x = InBitmapX(BW, event->xbutton.x);
	    status->from_y = InBitmapY(BW, event->xbutton.y);
	    status->to_x = InBitmapX(BW, event->xbutton.x);
	    status->to_y = InBitmapY(BW, event->xbutton.y);
	    status->success = status->draw ? True : False;
	    if (status->draw)
		(*status->draw)(w,
				status->from_x, status->from_y, 
				status->to_x, status->to_y, status->value);
	}
	break;
	
    case ButtonRelease:
	if (QuerySet(status->from_x, status->from_y)) {
	    status->value = Value(BW, event->xbutton.button);
	    status->btime = event->xbutton.time;	    
	    status->from_x = status->to_x;
	    status->from_y = status->to_y;
	    status->to_x = InBitmapX(BW, event->xbutton.x);
	    status->to_y = InBitmapY(BW, event->xbutton.y);
	    status->success = True;
	    
	    BWTerminateRequest(w, TRUE);
	}
	break;
	
    case MotionNotify:
	if (QuerySet(status->from_x, status->from_y)) {
	    if (QuerySet(status->to_x, status->to_y)) {
		if (!QueryInSquare(BW, event->xmotion.x, event->xmotion.y,
				   status->to_x, status->to_y)) {
		    status->from_x = status->to_x;
		    status->from_y = status->to_y;
		    status->to_x = InBitmapX(BW, event->xmotion.x);
		    status->to_y = InBitmapY(BW, event->xmotion.y);
		    if (status->draw)
			(*status->draw)(w,
					status->from_x, status->from_y, 
					status->to_x, status->to_y, status->value);
		}
	    }
	}
	break;
    }
}

void DragTwoPointsEngage(w, status, draw, state)
    Widget     w;
    BWStatus  *status;
    void     (*draw)();
    int       *state;
{
    
    status->from_x = NotSet;
    status->from_y = NotSet;
    status->to_x = NotSet;
    status->to_y = NotSet;
    status->draw = draw;
    status->success = False;
    status->state = *state;

    XtAddEventHandler(w,
		      ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
		      FALSE, DragTwoPointsHandler, (XtPointer)status);
}

void DragTwoPointsTerminate(w, status, draw)
    Widget     w;
    BWStatus  *status;
    void     (*draw)();
{
    
    if (status->success && draw) {
	if ((status->from_x != status->to_x) 
	    || 
	    (status->from_y != status->to_y))
	    (*draw)(w,
		    status->from_x, status->from_y,
		    status->to_x, status->to_y,
		    status->value);
	BWChangeNotify(w, NULL, NULL);
	BWSetChanged(w);
    }
    
    XtRemoveEventHandler(w,
		         ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
			 FALSE, DragTwoPointsHandler, (XtPointer)status);
}

/*****************************************************************************/

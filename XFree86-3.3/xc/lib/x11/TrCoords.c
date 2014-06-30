/* $XConsortium: TrCoords.c,v 11.15 94/04/17 20:21:20 rws Exp $ */
/*

Copyright (c) 1986  X Consortium

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

#define NEED_REPLIES
#include "Xlibint.h"

Bool XTranslateCoordinates(dpy, src_win, dest_win, src_x, src_y, 
		      dst_x, dst_y, child)
     register Display *dpy;
     Window src_win, dest_win;
     int src_x, src_y;
     int *dst_x, *dst_y;
     Window *child;
{       
    register xTranslateCoordsReq *req;
    xTranslateCoordsReply rep;

    LockDisplay(dpy);
    GetReq(TranslateCoords, req);
    req->srcWid = src_win;
    req->dstWid = dest_win;
    req->srcX = src_x;
    req->srcY = src_y;
    if (_XReply (dpy, (xReply *)&rep, 0, xTrue) == 0) {
	    UnlockDisplay(dpy);
	    SyncHandle();
	    return(False);
	}
	
    *child = rep.child;
    *dst_x = cvtINT16toInt (rep.dstX);
    *dst_y = cvtINT16toInt (rep.dstY);
    UnlockDisplay(dpy);
    SyncHandle();
    return ((int)rep.sameScreen);
}


/* $XConsortium: GetHColor.c,v 11.12 94/04/17 20:19:34 rws Exp $ */
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

Status XAllocColor(dpy, cmap, def)
register Display *dpy;
Colormap cmap;
XColor *def;
{
    Status status;
    xAllocColorReply rep;
    register xAllocColorReq *req;
    LockDisplay(dpy);
    GetReq(AllocColor, req);

    req->cmap = cmap;
    req->red = def->red;
    req->green = def->green;
    req->blue = def->blue;

    status = _XReply(dpy, (xReply *) &rep, 0, xTrue);
    if (status) {
      def->pixel = rep.pixel;
      def->red = rep.red;
      def->green = rep.green;
      def->blue = rep.blue;
      }
    UnlockDisplay(dpy);
    SyncHandle();
    return(status);
}

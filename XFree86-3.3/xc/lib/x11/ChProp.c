/* $XConsortium: ChProp.c /main/16 1996/10/22 14:16:10 kaleb $ */
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

#include "Xlibint.h"

#if NeedFunctionPrototypes
XChangeProperty (
    register Display *dpy,
    Window w,
    Atom property,
    Atom type,
    int format,  /* 8, 16, or 32 */
    int mode,  /* PropModeReplace, PropModePrepend, PropModeAppend */
    _Xconst unsigned char *data,
    int nelements)
#else
XChangeProperty (dpy, w, property, type, format, mode, data, nelements)
    register Display *dpy;
    Window w;
    Atom property, type;
    int format;  /* 8, 16, or 32 */
    int mode;  /* PropModeReplace, PropModePrepend, PropModeAppend */
    unsigned char *data;
    int nelements;
#endif
    {
    register xChangePropertyReq *req;
    register long len;

    LockDisplay(dpy);
    GetReq (ChangeProperty, req);
    req->window = w;
    req->property = property;
    req->type = type;
    req->mode = mode;
    if (nelements < 0) {
	req->nUnits = 0;
	req->format = 0; /* ask for garbage, get garbage */
    } else {
	req->nUnits = nelements;
	req->format = format;
    }

    switch (req->format) {
      case 8:
	len = ((long)nelements + 3)>>2;
	if (dpy->bigreq_size || req->length + len <= (unsigned) 65535) {
	    SetReqLen(req, len, len);
	    Data (dpy, (char *)data, nelements);
	} /* else force BadLength */
        break;
 
      case 16:
	len = ((long)nelements + 1)>>1;
	if (dpy->bigreq_size || req->length + len <= (unsigned) 65535) {
	    SetReqLen(req, len, len);
	    len = (long)nelements << 1;
	    Data16 (dpy, (short *) data, len);
	} /* else force BadLength */
	break;

      case 32:
	len = nelements;
	if (dpy->bigreq_size || req->length + len <= (unsigned) 65535) {
	    SetReqLen(req, len, len);
	    len = (long)nelements << 2;
	    Data32 (dpy, (long *) data, len);
	} /* else force BadLength */
	break;

      default:
        /* BadValue will be generated */ ;
      }

    UnlockDisplay(dpy);
    SyncHandle();
    return 1;
    }






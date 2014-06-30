/* $XConsortium: LiHosts.c,v 11.22 94/04/17 20:20:06 rws Exp $ */
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

/* This can really be considered an os dependent routine */

#define NEED_REPLIES
#include "Xlibint.h"
/*
 * can be freed using XFree.
 */

XHostAddress *XListHosts (dpy, nhosts, enabled)
    register Display *dpy;
    int *nhosts;	/* RETURN */
    Bool *enabled;	/* RETURN */
    {
    register XHostAddress *outbuf = 0, *op;
    xListHostsReply reply;
    long nbytes;
    unsigned char *buf, *bp;
    register unsigned i;
    register xListHostsReq *req;

    LockDisplay(dpy);
    GetReq (ListHosts, req);

    if (!_XReply (dpy, (xReply *) &reply, 0, xFalse)) {
       UnlockDisplay(dpy);
       SyncHandle();
       return (XHostAddress *) NULL;
    }

    if (reply.nHosts) {
	nbytes = reply.length << 2;	/* compute number of bytes in reply */
	op = outbuf = (XHostAddress *)
	    Xmalloc((unsigned) (nbytes + reply.nHosts * sizeof(XHostAddress)));

	if (! outbuf) {	
	    _XEatData(dpy, (unsigned long) nbytes);
	    UnlockDisplay(dpy);
	    SyncHandle();
	    return (XHostAddress *) NULL;
	}
	bp = buf = 
	    ((unsigned char  *) outbuf) + reply.nHosts * sizeof(XHostAddress);

	_XRead (dpy, (char *) buf, nbytes);

	for (i = 0; i < reply.nHosts; i++) {
#ifdef WORD64
	    xHostEntry xhe;
	    memcpy((char *)&xhe, bp, SIZEOF(xHostEntry));
	    op->family = xhe.family;
	    op->length = xhe.length;
#else
	    op->family = ((xHostEntry *) bp)->family;
	    op->length =((xHostEntry *) bp)->length; 
#endif
	    op->address = (char *) (bp + SIZEOF(xHostEntry));
	    bp += SIZEOF(xHostEntry) + (((op->length + 3) >> 2) << 2);
	    op++;
	}
    }

    *enabled = reply.enabled;
    *nhosts = reply.nHosts;
    UnlockDisplay(dpy);
    SyncHandle();
    return (outbuf);
}


    



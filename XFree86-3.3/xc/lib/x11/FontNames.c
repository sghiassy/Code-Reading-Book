/* $XConsortium: FontNames.c /main/12 1996/10/22 14:18:43 kaleb $ */
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

#if NeedFunctionPrototypes
char **XListFonts(
register Display *dpy,
_Xconst char *pattern,  /* null-terminated */
int maxNames,
int *actualCount)	/* RETURN */
#else
char **XListFonts(dpy, pattern, maxNames, actualCount)
register Display *dpy;
char *pattern;  /* null-terminated */
int maxNames;
int *actualCount;	/* RETURN */
#endif
{       
    register long nbytes;
    register unsigned i;
    register int length;
    char **flist;
    char *ch;
    xListFontsReply rep;
    register xListFontsReq *req;
    register long rlen;

    LockDisplay(dpy);
    GetReq(ListFonts, req);
    req->maxNames = maxNames;
    nbytes = req->nbytes = pattern ? strlen (pattern) : 0;
    req->length += (nbytes + 3) >> 2;
    _XSend (dpy, pattern, nbytes);
    /* use _XSend instead of Data, since following _XReply will flush buffer */

    (void) _XReply (dpy, (xReply *)&rep, 0, xFalse);

    if (rep.nFonts) {
	flist = (char **)Xmalloc ((unsigned)rep.nFonts * sizeof(char *));
	rlen = rep.length << 2;
	ch = (char *) Xmalloc((unsigned) (rlen + 1));
	    /* +1 to leave room for last null-terminator */

	if ((! flist) || (! ch)) {
	    if (flist) Xfree((char *) flist);
	    if (ch) Xfree(ch);
	    _XEatData(dpy, (unsigned long) rlen);
	    UnlockDisplay(dpy);
	    SyncHandle();
	    return (char **) NULL;
	}

	_XReadPad (dpy, ch, rlen);
	/*
	 * unpack into null terminated strings.
	 */
	length = *(unsigned char *)ch;
	*ch = 1; /* make sure it is non-zero for XFreeFontNames */
	for (i = 0; i < rep.nFonts; i++) {
	    flist[i] = ch + 1;  /* skip over length */
	    ch += length + 1;  /* find next length ... */
	    length = *(unsigned char *)ch;
	    *ch = '\0';  /* and replace with null-termination */
	}
    }
    else flist = (char **) NULL;
    *actualCount = rep.nFonts;
    UnlockDisplay(dpy);
    SyncHandle();
    return (flist);
}

XFreeFontNames(list)
char **list;
{       
	if (list) {
		if (!*(list[0]-1)) { /* from ListFontsWithInfo */
			register char **names;
			for (names = list+1; *names; names++)
				Xfree (*names);
		}
		Xfree (list[0]-1);
		Xfree ((char *)list);
	}
	return 1;
}

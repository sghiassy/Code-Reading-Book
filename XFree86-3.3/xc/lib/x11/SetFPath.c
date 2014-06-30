/* $TOG: SetFPath.c /main/9 1997/04/22 15:55:16 barstow $ */
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

#define safestrlen(s) ((s) ? strlen(s) : 0)

XSetFontPath (dpy, directories, ndirs)
register Display *dpy;
char **directories;
int ndirs;
{
	register int n = 0;
	register int i;
	register int nbytes;
	char *p;
	register xSetFontPathReq *req;
	int retCode;

        LockDisplay(dpy);
	GetReq (SetFontPath, req);
	req->nFonts = ndirs;
	for (i = 0; i < ndirs; i++) {
		n += safestrlen (directories[i]) + 1;
	}
	nbytes = (n + 3) & ~3;
	req->length += nbytes >> 2;
	if (p = (char *) Xmalloc ((unsigned) nbytes)) {
		/*
	 	 * pack into counted strings.
	 	 */
		char	*tmp = p;

		for (i = 0; i < ndirs; i++) {
			register int length = safestrlen (directories[i]);
			*p = length;
			memcpy (p + 1, directories[i], length);
			p += length + 1;
		}
		Data (dpy, tmp, nbytes);
		Xfree ((char *) tmp);
		retCode = 1;
	}
	else
		retCode = 0;

        UnlockDisplay(dpy);
	SyncHandle();
	return (retCode);
}

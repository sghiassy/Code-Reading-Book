/* $XConsortium: lut.c,v 1.2 94/04/17 20:18:26 mor Exp $ */

/*

Copyright (c) 1993  X Consortium

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

#include "XIElibint.h"


XieLut
XieCreateLUT (display)

Display	*display;

{
    xieCreateLUTReq	*req;
    char		*pBuf;
    XieLut		id;

    LockDisplay (display);

    id = XAllocID (display);

    GET_REQUEST (CreateLUT, pBuf);

    BEGIN_REQUEST_HEADER (CreateLUT, pBuf, req);

    STORE_REQUEST_HEADER (CreateLUT, req);
    req->lut = id;

    END_REQUEST_HEADER (CreateLUT, pBuf, req);

    UnlockDisplay (display);
    SYNC_HANDLE (display);

    return (id);
}


void
XieDestroyLUT (display, lut)

Display	*display;
XieLut	lut;

{
    xieDestroyLUTReq	*req;
    char		*pBuf;

    LockDisplay (display);

    GET_REQUEST (DestroyLUT, pBuf);

    BEGIN_REQUEST_HEADER (DestroyLUT, pBuf, req);

    STORE_REQUEST_HEADER (DestroyLUT, req);
    req->lut = lut;

    END_REQUEST_HEADER (DestroyLUT, pBuf, req);

    UnlockDisplay (display);
    SYNC_HANDLE (display);
}

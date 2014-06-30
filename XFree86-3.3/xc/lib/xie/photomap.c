/* $XConsortium: photomap.c,v 1.6 94/04/17 20:18:27 rws Exp $ */

/*

Copyright (c) 1993, 1994  X Consortium

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


XiePhotomap
XieCreatePhotomap (display)

Display	*display;

{
    xieCreatePhotomapReq	*req;
    char			*pBuf;
    XiePhotomap			id;

    LockDisplay (display);

    id = XAllocID (display);

    GET_REQUEST (CreatePhotomap, pBuf);

    BEGIN_REQUEST_HEADER (CreatePhotomap, pBuf, req);

    STORE_REQUEST_HEADER (CreatePhotomap, req);
    req->photomap = id;

    END_REQUEST_HEADER (CreatePhotomap, pBuf, req);

    UnlockDisplay (display);
    SYNC_HANDLE (display);

    return (id);
}


void
XieDestroyPhotomap (display, photomap)

Display		*display;
XiePhotomap	photomap;

{
    xieDestroyPhotomapReq	*req;
    char			*pBuf;

    LockDisplay (display);

    GET_REQUEST (DestroyPhotomap, pBuf);

    BEGIN_REQUEST_HEADER (DestroyPhotomap, pBuf, req);

    STORE_REQUEST_HEADER (DestroyPhotomap, req);
    req->photomap = photomap;

    END_REQUEST_HEADER (DestroyPhotomap, pBuf, req);

    UnlockDisplay (display);
    SYNC_HANDLE (display);
}


Status
XieQueryPhotomap (display, photomap, populated_ret, datatype_ret,
    dataclass_ret, decode_technique_ret, width_ret, height_ret, levels_ret)

Display      		*display;
XiePhotomap    		photomap;
Bool         		*populated_ret;
XieDataType   		*datatype_ret;
XieDataClass		*dataclass_ret;
XieDecodeTechnique	*decode_technique_ret;
XieLTriplet     	width_ret;
XieLTriplet     	height_ret;
XieLTriplet     	levels_ret;

{
    xieQueryPhotomapReq		*req;
    xieQueryPhotomapReply	rep;
    char			*pBuf;

    LockDisplay (display);

    GET_REQUEST (QueryPhotomap, pBuf);

    BEGIN_REQUEST_HEADER (QueryPhotomap, pBuf, req);

    STORE_REQUEST_HEADER (QueryPhotomap, req);
    req->photomap = photomap;

    END_REQUEST_HEADER (QueryPhotomap, pBuf, req);

    if (_XReply (display, (xReply *)&rep,
	(SIZEOF (xieQueryPhotomapReply) - 32) >> 2, xTrue) == 0)
    {
        UnlockDisplay (display);
	SYNC_HANDLE (display);

	return (0);
    }

    *populated_ret 	  = rep.populated;
    *datatype_ret         = rep.dataType;
    *dataclass_ret	  = rep.dataClass;
    *decode_technique_ret = rep.decodeTechnique;
    width_ret[0]      	  = rep.width0;
    width_ret[1]      	  = rep.width1;
    width_ret[2]          = rep.width2;
    height_ret[0]         = rep.height0;
    height_ret[1]         = rep.height1;
    height_ret[2]         = rep.height2;
    levels_ret[0]         = rep.levels0;
    levels_ret[1]         = rep.levels1;
    levels_ret[2]         = rep.levels2;

    UnlockDisplay (display);
    SYNC_HANDLE (display);

    return (1);
}

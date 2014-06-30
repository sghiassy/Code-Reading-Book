/* $XConsortium: photoflo.c,v 1.6 94/05/13 10:30:17 mor Exp $ */

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


XiePhotoElement *
XieAllocatePhotofloGraph (count)

unsigned int count;

{
    XiePhotoElement	*ptr;
    unsigned		size;

    size = count * sizeof (XiePhotoElement);
    ptr = (XiePhotoElement *) Xmalloc (size);
    bzero ((char *) ptr, size);

    return (ptr);
}


XiePhotoflo
XieCreatePhotoflo (display, elem_list, elem_count)

Display		*display;
XiePhotoElement	*elem_list;
int		elem_count;

{
    xieCreatePhotofloReq	*req;
    char			*pBuf, *pStart;
    unsigned			size;
    XiePhotoflo			id;
    int				i;

    LockDisplay (display);

    id = XAllocID (display);

    size = _XiePhotofloSize (elem_list, elem_count);

    GET_REQUEST (CreatePhotoflo, pBuf);

    BEGIN_REQUEST_HEADER (CreatePhotoflo, pBuf, req);

    STORE_REQUEST_EXTRA_HEADER (CreatePhotoflo, size, req);
    req->floID = id;
    req->numElements = elem_count;

    END_REQUEST_HEADER (CreatePhotoflo, pBuf, req);

    pBuf = pStart = _XAllocScratch (display, size);

    for (i = 0; i < elem_count; i++)
	(*_XieElemFuncs[elem_list[i].elemType - 1]) (&pBuf, &elem_list[i]);

    Data (display, pStart, size);

    UnlockDisplay (display);
    SYNC_HANDLE (display);

    return (id);
}


void
XieDestroyPhotoflo (display, photoflo)

Display		*display;
XiePhotoflo	photoflo;

{
    xieDestroyPhotofloReq	*req;
    char			*pBuf;

    LockDisplay (display);

    GET_REQUEST (DestroyPhotoflo, pBuf);

    BEGIN_REQUEST_HEADER (DestroyPhotoflo, pBuf, req);

    STORE_REQUEST_HEADER (DestroyPhotoflo, req);
    req->floID = photoflo;

    END_REQUEST_HEADER (DestroyPhotoflo, pBuf, req);

    UnlockDisplay (display);
    SYNC_HANDLE (display);
}


void
XieExecutePhotoflo (display, photoflo, notify)

Display		*display;
XiePhotoflo	photoflo;
Bool		notify;

{
    xieExecutePhotofloReq	*req;
    char			*pBuf;

    LockDisplay (display);

    GET_REQUEST (ExecutePhotoflo, pBuf);

    BEGIN_REQUEST_HEADER (ExecutePhotoflo, pBuf, req);

    STORE_REQUEST_HEADER (ExecutePhotoflo, req);
    req->floID = photoflo;
    req->notify = notify;

    END_REQUEST_HEADER (ExecutePhotoflo, pBuf, req);

    UnlockDisplay (display);
    SYNC_HANDLE (display);
}


void
XieModifyPhotoflo (display, photoflo, start, elem_list, elem_count)

Display		*display;
XiePhotoflo	photoflo;
int		start;
XiePhotoElement	*elem_list;
int		elem_count;

{
    xieModifyPhotofloReq	*req;
    char			*pBuf, *pStart;
    unsigned			size;
    int				i;

    LockDisplay (display);

    size = _XiePhotofloSize (elem_list, elem_count);

    GET_REQUEST (ModifyPhotoflo, pBuf);

    BEGIN_REQUEST_HEADER (ModifyPhotoflo, pBuf, req);

    STORE_REQUEST_EXTRA_HEADER (ModifyPhotoflo, size, req);
    req->floID = photoflo;
    req->start = start;
    req->numElements = elem_count;

    END_REQUEST_HEADER (ModifyPhotoflo, pBuf, req);

    pBuf = pStart = _XAllocScratch (display, size);

    for (i = 0; i < elem_count; i++)
	(*_XieElemFuncs[elem_list[i].elemType - 1]) (&pBuf, &elem_list[i]);

    Data (display, pStart, size);

    UnlockDisplay (display);
    SYNC_HANDLE (display);
}


void
XieRedefinePhotoflo (display, photoflo, elem_list, elem_count)

Display		*display;
XiePhotoflo	photoflo;
XiePhotoElement	*elem_list;
int		elem_count;

{
    xieRedefinePhotofloReq	*req;
    char			*pBuf, *pStart;
    unsigned			size;
    int				i;

    LockDisplay (display);

    size = _XiePhotofloSize (elem_list, elem_count);

    GET_REQUEST (RedefinePhotoflo, pBuf);

    BEGIN_REQUEST_HEADER (RedefinePhotoflo, pBuf, req);

    STORE_REQUEST_EXTRA_HEADER (RedefinePhotoflo, size, req);
    req->floID = photoflo;
    req->numElements = elem_count;

    END_REQUEST_HEADER (RedefinePhotoflo, pBuf, req);

    pBuf = pStart = _XAllocScratch (display, size);

    for (i = 0; i < elem_count; i++)
	(*_XieElemFuncs[elem_list[i].elemType - 1]) (&pBuf, &elem_list[i]);

    Data (display, pStart, size);

    UnlockDisplay (display);
    SYNC_HANDLE (display);
}


Status
XieQueryPhotoflo (display, name_space, flo_id,
    state_ret, data_expected_ret, nexpected_ret,
    data_available_ret, navailable_ret)

Display      		*display;
unsigned long		name_space;
unsigned long		flo_id;
XiePhotofloState	*state_ret;
XiePhototag		**data_expected_ret;
unsigned int    	*nexpected_ret;
XiePhototag		**data_available_ret;
unsigned int    	*navailable_ret;

{
    xieQueryPhotofloReq		*req;
    xieQueryPhotofloReply	rep;
    char			*pBuf;
    int				i;

    LockDisplay (display);

    GET_REQUEST (QueryPhotoflo, pBuf);

    BEGIN_REQUEST_HEADER (QueryPhotoflo, pBuf, req);

    STORE_REQUEST_HEADER (QueryPhotoflo, req);
    req->nameSpace = name_space;
    req->floID = flo_id;

    END_REQUEST_HEADER (QueryPhotoflo, pBuf, req);

    *state_ret = 0;
    *nexpected_ret = 0;
    *data_expected_ret = NULL;
    *navailable_ret = 0;
    *data_available_ret = NULL;
	
    if (_XReply (display, (xReply *)&rep, 0, xFalse) == 0)
    {
        UnlockDisplay (display);
	SYNC_HANDLE (display);

	return (0);
    }

    *state_ret = rep.state;

    if (rep.expectedCount > 0)
    {
	short *temp = (short *) _XAllocTemp (display,
	    sizeof (short) * rep.expectedCount);

	*nexpected_ret = rep.expectedCount;
	*data_expected_ret = (XiePhototag *) Xmalloc (
	    sizeof (XiePhototag) * rep.expectedCount);
			
	_XRead16Pad (display, temp,
	    rep.expectedCount * SIZEOF (xieTypPhototag));

	for (i = 0; i < rep.expectedCount; i++)
	    (*data_expected_ret)[i] = (XiePhototag) temp[i];

	_XFreeTemp (display, (char *) temp,
	    sizeof (short) * rep.expectedCount);
    }

    if (rep.availableCount > 0)
    {
	short *temp = (short *) _XAllocTemp (display,
	    sizeof (short) * rep.availableCount);

	*navailable_ret = rep.availableCount;
	*data_available_ret = (XiePhototag *) Xmalloc (
	    sizeof (XiePhototag) * rep.availableCount);
			
	_XRead16Pad (display, temp,
	    rep.availableCount * SIZEOF (xieTypPhototag));

	for (i = 0; i < rep.availableCount; i++)
	    (*data_available_ret)[i] = (XiePhototag) temp[i];

	_XFreeTemp (display, (char *) temp,
	    sizeof (short) * rep.availableCount);
    }

    UnlockDisplay (display);
    SYNC_HANDLE (display);

    return (1);
}

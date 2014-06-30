/* $XConsortium: utils.c,v 1.2 94/04/17 20:18:30 mor Exp $ */

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


Status
_XieRegisterTechFunc (group, technique, techfunc)

int group;
int technique;
XieTechFuncPtr techfunc;

{
    XieTechFuncRec *ptr = _XieTechFuncs[group - 1];
    XieTechFuncRec *prev = NULL, *newrec;
    int	status = 1;

    newrec = (XieTechFuncRec *) Xmalloc (sizeof (XieTechFuncRec));
    newrec->technique = technique;
    newrec->techfunc = techfunc;

    if (ptr == NULL)
    {
	_XieTechFuncs[group - 1] = newrec;
	newrec->next = NULL;
    }
    else
    {
	while (ptr && ptr->technique < technique)
	{
	    prev = ptr;
	    ptr = ptr->next;
	}

	if (ptr == NULL)
	{
	    prev->next = newrec;
	    newrec->next = NULL;
	}
	else
	{
	    if (ptr->technique == technique)
	    {
		Xfree ((char *) newrec);
		status = 0;
	    }
	    else if (prev = NULL)
	    {
		newrec->next = _XieTechFuncs[group - 1];
		_XieTechFuncs[group - 1] = newrec;
	    }
	    else
	    {
		newrec->next = prev->next;
		prev->next = newrec;
	    }
	}
    }

    return (status);
}



XieTechFuncPtr
_XieLookupTechFunc (group, technique)

int group;
int technique;

{
    XieTechFuncRec *ptr = _XieTechFuncs[group - 1];
    int found = 0;

    while (ptr && !found)
    {
	if (ptr->technique == technique)
	    found = 1;
	else
	    ptr = ptr->next;
    }

    if (found)
	return (ptr->techfunc);
    else
	return (NULL);
}



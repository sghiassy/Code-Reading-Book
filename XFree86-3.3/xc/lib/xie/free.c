/* $XConsortium: free.c,v 1.3 94/04/17 20:18:23 mor Exp $ */

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

#define CHECK_AND_FREE(_ptr) if (_ptr) Xfree (_ptr)


void
XieFreeTechniques (techs, count)

XieTechnique 	*techs;
unsigned int	count;

{
    int i;

    if (techs)
    {
	for (i = 0; i < count; i++)
	    CHECK_AND_FREE (techs[i].name);

	Xfree ((char *) techs);
    }
}


void
XieFreePhotofloGraph (elements, count)

XiePhotoElement *elements;
unsigned int    count;

{
    /*
     * NOTE: We do not free the technique parameters here.
     * Most of the technique parameters should be freed by the
     * client using Xfree (exception: EncodeJPEGBaseline and
     * EncodeJPEGLossless, see functions below).  This is so
     * the client can reuse technique parameters between photoflos.
     */

    int i;

    if (!elements)
	return;

    for (i = 0; i < count; i++)
    {
	switch (elements[i].elemType)
	{
	case xieElemConvolve:
	    CHECK_AND_FREE ((char *) elements[i].data.Convolve.kernel);
	    break;
	case xieElemPasteUp:
	    CHECK_AND_FREE ((char *) elements[i].data.PasteUp.tiles);
	    break;
	default:
	    break;
	}
    }

    Xfree ((char *) elements);
}


void
XieFreeEncodeJPEGBaseline (param)

XieEncodeJPEGBaselineParam *param;

{
    if (param)
    {
	CHECK_AND_FREE ((char *) param->q_table);
	CHECK_AND_FREE ((char *) param->ac_table);
	CHECK_AND_FREE ((char *) param->dc_table);
	Xfree ((char *) param);
    }
}


void
XieFreeEncodeJPEGLossless (param)

XieEncodeJPEGLosslessParam *param;

{
    if (param)
    {
	CHECK_AND_FREE ((char *) param->table);
	Xfree ((char *) param);
    }
}


void 
XieFreePasteUpTiles (element)

XiePhotoElement	*element;

{
    XieTile *tiles= element->data.PasteUp.tiles;

    if (tiles)
    {
	Xfree (tiles);
	element->data.PasteUp.tiles=NULL;
    }
}

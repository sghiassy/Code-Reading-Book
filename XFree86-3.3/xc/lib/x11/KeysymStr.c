/* $XConsortium: KeysymStr.c,v 11.10 95/06/08 23:20:39 gildea Exp $ */

/*

Copyright (c) 1990  X Consortium

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
#include <X11/Xresource.h>
#include <X11/keysymdef.h>

#ifdef __STDC__
#define Const const
#else
#define Const /**/
#endif

typedef unsigned long Signature;

#define NEEDVTABLE
#include "ks_tables.h"

extern XrmDatabase _XInitKeysymDB();
extern Const unsigned char _XkeyTable[];


typedef struct _GRNData {
    char *name;
    XrmRepresentation type;
    XrmValuePtr value;
} GRNData;

#if NeedFunctionPrototypes
/*ARGSUSED*/
static Bool SameValue(
    XrmDatabase*	db,
    XrmBindingList      bindings,
    XrmQuarkList	quarks,
    XrmRepresentation*  type,
    XrmValuePtr		value,
    XPointer		data
)
#else
static Bool SameValue(db, bindings, quarks, type, value, data)
    XrmDatabase		*db;
    XrmBindingList      bindings;
    XrmQuarkList	quarks;
    XrmRepresentation   *type;
    XrmValuePtr		value;
    XPointer		data;
#endif
{
    GRNData *gd = (GRNData *)data;

    if ((*type == gd->type) && (value->size == gd->value->size) &&
	!strncmp((char *)value->addr, (char *)gd->value->addr, value->size))
    {
	gd->name = XrmQuarkToString(*quarks); /* XXX */
	return True;
    }
    return False;
}

char *XKeysymToString(ks)
    KeySym ks;
{
    register int i, n;
    int h;
    register int idx;
    Const unsigned char *entry;
    unsigned char val1, val2;
    XrmDatabase keysymdb;

    if (!ks)
	return ((char *)NULL);
    if (ks == XK_VoidSymbol)
	ks = 0;
    if (ks <= 0xffff)
    {
	val1 = ks >> 8;
	val2 = ks & 0xff;
	i = ks % VTABLESIZE;
	h = i + 1;
	n = VMAXHASH;
	while (idx = hashKeysym[i])
	{
	    entry = &_XkeyTable[idx];
	    if ((entry[0] == val1) && (entry[1] == val2))
		return ((char *)entry + 2);
	    if (!--n)
		break;
	    i += h;
	    if (i >= VTABLESIZE)
		i -= VTABLESIZE;
	}
    }

    if (keysymdb = _XInitKeysymDB())
    {
	char buf[9];
	XrmValue resval;
	XrmQuark empty = NULLQUARK;
	GRNData data;

	sprintf(buf, "%lX", ks);
	resval.addr = (XPointer)buf;
	resval.size = strlen(buf) + 1;
	data.name = (char *)NULL;
	data.type = XrmPermStringToQuark("String");
	data.value = &resval;
	(void)XrmEnumerateDatabase(keysymdb, &empty, &empty, XrmEnumAllLevels,
				   SameValue, (XPointer)&data);
	return data.name;
    }
    return ((char *) NULL);
}

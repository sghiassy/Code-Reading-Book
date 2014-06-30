/* $XConsortium: Atoms.h,v 1.9 94/04/17 20:15:49 converse Exp $ */

/* 

Copyright (c) 1988  X Consortium

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

/*
 * The interfaces described by this header file are for miscellaneous utilities
 * and are not part of the Xlib standard.
 */

#ifndef _XMU_ATOMS_H_
#define _XMU_ATOMS_H_

#include <X11/Intrinsic.h>
#include <X11/Xfuncproto.h>

typedef struct _AtomRec *AtomPtr;

extern AtomPtr
    _XA_ATOM_PAIR,
    _XA_CHARACTER_POSITION,
    _XA_CLASS,
    _XA_CLIENT_WINDOW,
    _XA_CLIPBOARD,
    _XA_COMPOUND_TEXT,
    _XA_DECNET_ADDRESS,
    _XA_DELETE,
    _XA_FILENAME,
    _XA_HOSTNAME,
    _XA_IP_ADDRESS,
    _XA_LENGTH,
    _XA_LIST_LENGTH,
    _XA_NAME,
    _XA_NET_ADDRESS,
    _XA_NULL,
    _XA_OWNER_OS,
    _XA_SPAN,
    _XA_TARGETS,
    _XA_TEXT,
    _XA_TIMESTAMP,
    _XA_USER;

#define XA_ATOM_PAIR(d)		XmuInternAtom(d, _XA_ATOM_PAIR)
#define XA_CHARACTER_POSITION(d) XmuInternAtom(d, _XA_CHARACTER_POSITION)
#define XA_CLASS(d)		XmuInternAtom(d, _XA_CLASS)
#define XA_CLIENT_WINDOW(d)	XmuInternAtom(d, _XA_CLIENT_WINDOW)
#define XA_CLIPBOARD(d)		XmuInternAtom(d, _XA_CLIPBOARD)
#define XA_COMPOUND_TEXT(d)	XmuInternAtom(d, _XA_COMPOUND_TEXT)
#define XA_DECNET_ADDRESS(d)	XmuInternAtom(d, _XA_DECNET_ADDRESS)
#define XA_DELETE(d)		XmuInternAtom(d, _XA_DELETE)
#define XA_FILENAME(d)		XmuInternAtom(d, _XA_FILENAME)
#define XA_HOSTNAME(d)		XmuInternAtom(d, _XA_HOSTNAME)
#define XA_IP_ADDRESS(d)	XmuInternAtom(d, _XA_IP_ADDRESS)
#define XA_LENGTH(d)		XmuInternAtom(d, _XA_LENGTH)
#define XA_LIST_LENGTH(d)	XmuInternAtom(d, _XA_LIST_LENGTH)
#define XA_NAME(d)		XmuInternAtom(d, _XA_NAME)
#define XA_NET_ADDRESS(d)	XmuInternAtom(d, _XA_NET_ADDRESS)
#define XA_NULL(d)		XmuInternAtom(d, _XA_NULL)
#define XA_OWNER_OS(d)		XmuInternAtom(d, _XA_OWNER_OS)
#define XA_SPAN(d)		XmuInternAtom(d, _XA_SPAN)
#define XA_TARGETS(d)		XmuInternAtom(d, _XA_TARGETS)
#define XA_TEXT(d)		XmuInternAtom(d, _XA_TEXT)
#define XA_TIMESTAMP(d)		XmuInternAtom(d, _XA_TIMESTAMP)
#define XA_USER(d)		XmuInternAtom(d, _XA_USER)

_XFUNCPROTOBEGIN

extern char *XmuGetAtomName(
#if NeedFunctionPrototypes
    Display *	/* dpy */,
    Atom	/* atom */
#endif
);

extern Atom XmuInternAtom(
#if NeedFunctionPrototypes
    Display *	/* dpy */,
    AtomPtr	/* atom_ptr */
#endif
);

extern void XmuInternStrings(
#if NeedFunctionPrototypes
    Display *		/* dpy */,
    String *		/* names */,
    Cardinal    	/* count */,
    Atom *		/* atoms_return */
#endif
);

extern AtomPtr XmuMakeAtom(
#if NeedFunctionPrototypes
    _Xconst char *		/* name	*/
#endif
);

extern char *XmuNameOfAtom(
#if NeedFunctionPrototypes
    AtomPtr	/* atom_ptr */
#endif
);

_XFUNCPROTOEND

#endif /* _XMU_ATOMS_H_ */

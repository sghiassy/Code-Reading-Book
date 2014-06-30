/* $XConsortium: WidgetNode.h,v 1.8 94/04/17 20:16:24 converse Exp $ */

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

/*
 * Author:  Jim Fulton, MIT X Consortium
 */

#ifndef _XmuWidgetNode_h
#define _XmuWidgetNode_h

#include <X11/Xfuncproto.h>

/*
 * This is usually initialized by setting the first two fields and letting
 * rest be implicitly nulled (by genlist.sh, for example)
 */
typedef struct _XmuWidgetNode {
    char *label;			/* mixed case name */
    WidgetClass *widget_class_ptr;	/* addr of widget class */
    struct _XmuWidgetNode *superclass;	/* superclass of widget_class */
    struct _XmuWidgetNode *children, *siblings;	/* subclass links */
    char *lowered_label;		/* lowercase version of label */
    char *lowered_classname;		/* lowercase version of class_name */
    Bool have_resources;		/* resources have been fetched */
    XtResourceList resources;		/* extracted resource database */
    struct _XmuWidgetNode **resourcewn;	/* where resources come from */
    Cardinal nresources;		/* number of resources */
    XtResourceList constraints;		/* extracted constraint resources */
    struct _XmuWidgetNode **constraintwn;  /* where constraints come from */
    Cardinal nconstraints;		/* number of constraint resources */
    XtPointer data;			/* extra data */
} XmuWidgetNode;

#define XmuWnClass(wn) ((wn)->widget_class_ptr[0])
#define XmuWnClassname(wn) (XmuWnClass(wn)->core_class.class_name)
#define XmuWnSuperclass(wn) ((XmuWnClass(wn))->core_class.superclass)

					/* external interfaces */
_XFUNCPROTOBEGIN

extern void XmuWnInitializeNodes (
#if NeedFunctionPrototypes
    XmuWidgetNode *	/* nodearray */,
    int			/* nnodes */
#endif
);

extern void XmuWnFetchResources (
#if NeedFunctionPrototypes
    XmuWidgetNode *	/* node */,
    Widget		/* toplevel */,
    XmuWidgetNode *	/* topnode */
#endif
);

extern int XmuWnCountOwnedResources (
#if NeedFunctionPrototypes
    XmuWidgetNode *	/* node */,
    XmuWidgetNode *	/* ownernode */,
    Bool		/* constraints */
#endif
);

extern XmuWidgetNode *XmuWnNameToNode (
#if NeedFunctionPrototypes
    XmuWidgetNode *	/* nodelist */,
    int			/* nnodes */,
    _Xconst char *	/* name */
#endif
);

_XFUNCPROTOEND

#endif /* _XmuWidgetNode_h */


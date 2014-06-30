/* $XConsortium: EventI.h,v 1.24 94/04/17 20:14:03 kaleb Exp $ */
/* $oHeader: EventI.h,v 1.3 88/08/24 09:21:11 asente Exp $ */

/***********************************************************

Copyright (c) 1987, 1988  X Consortium

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


Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* 
 * Event.h - exported types and functions for toolkit event handler
 * 
 * Author:	Charles Haynes
 * 		Digital Equipment Corporation
 * 		Western Software Laboratory
 * Date:	Sun Dec  6 1987
 */

#ifndef _Event_h_
#define _Event_h_

typedef struct _XtGrabRec  *XtGrabList;

extern void _XtEventInitialize(
#if NeedFunctionPrototypes
    void
#endif
);

typedef struct _XtEventRec {
     XtEventTable	next;
     EventMask		mask;	/*  also select_data count for RecExt */
     XtEventHandler	proc;
     XtPointer		closure;
     unsigned int	select:1;
     unsigned int	has_type_specifier:1;
     unsigned int	async:1; /* not used, here for Digital extension? */
} XtEventRec;

typedef struct _XtGrabRec {
    XtGrabList next;
    Widget   widget;
    unsigned int exclusive:1;
    unsigned int spring_loaded:1;
}XtGrabRec;

typedef struct _BlockHookRec {
    struct _BlockHookRec* next;
    XtAppContext app;
    XtBlockHookProc proc;
    XtPointer closure;
} BlockHookRec, *BlockHook;

extern void _XtFreeEventTable(
#if NeedFunctionPrototypes
    XtEventTable*	/* event_table */
#endif
);

extern Boolean _XtOnGrabList(
#if NeedFunctionPrototypes
    Widget	/* widget */,
    XtGrabRec*	/* grabList */
#endif
);

extern void _XtRemoveAllInputs(
#if NeedFunctionPrototypes
    XtAppContext /* app */
#endif
);

extern void _XtRefreshMapping(
#if NeedFunctionPrototypes
    XEvent*	/* event */,
    _XtBoolean	/* dispatch */
#endif
);

extern EventMask _XtConvertTypeToMask(
#if NeedFunctionPrototypes
    int		/* eventType */
#endif
);

#endif /* _Event_h_ */

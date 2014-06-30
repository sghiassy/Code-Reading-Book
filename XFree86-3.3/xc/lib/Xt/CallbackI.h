/* $XConsortium: CallbackI.h,v 1.18 94/04/17 20:13:39 converse Exp $ */
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

/****************************************************************
 *
 * Callbacks
 *
 ****************************************************************/

typedef XrmResource **CallbackTable;

#define _XtCBCalling 1
#define _XtCBFreeAfterCalling 2

typedef struct internalCallbackRec {
    unsigned short count;
    char	   is_padded;	/* contains NULL padding for external form */
    char	   call_state;  /* combination of _XtCB{FreeAfter}Calling */
#ifdef LONG64
    unsigned int   align_pad;	/* padding to align callback list */
#endif
    /* XtCallbackList */
} InternalCallbackRec, *InternalCallbackList;

typedef Boolean (*_XtConditionProc)(
#if NeedFunctionPrototypes
    XtPointer	/* data */
#endif
);

extern void _XtAddCallback(
#if NeedFunctionPrototypes
    InternalCallbackList*	/* callbacks */,
    XtCallbackProc		/* callback */,
    XtPointer 			/* closure */
#endif
);

extern void _XtAddCallbackOnce(
#if NeedFunctionPrototypes
    InternalCallbackList*	/* callbacks */,
    XtCallbackProc		/* callback */,
    XtPointer 			/* closure */
#endif
);

extern InternalCallbackList _XtCompileCallbackList(
#if NeedFunctionPrototypes
    XtCallbackList		/* xtcallbacks */
#endif
);

extern XtCallbackList _XtGetCallbackList(
#if NeedFunctionPrototypes
    InternalCallbackList*	/* callbacks */
#endif
);

extern void _XtRemoveAllCallbacks(
#if NeedFunctionPrototypes
    InternalCallbackList*	/* callbacks */
#endif
);

extern void _XtRemoveCallback(
#if NeedFunctionPrototypes
    InternalCallbackList*	/* callbacks */,
    XtCallbackProc		/* callback */,
    XtPointer			/* closure */
#endif
);

extern void _XtPeekCallback(
#if NeedFunctionPrototypes
    Widget			/* widget */,
    XtCallbackList		/* callbacks */,
    XtCallbackProc *		/* callback */,
    XtPointer *			/* closure */
#endif
);

extern void _XtCallConditionalCallbackList(
#if NeedFunctionPrototypes
    Widget			/* widget */,
    XtCallbackList		/* callbacks */,
    XtPointer			/* call_data */,
    _XtConditionProc		/* cond_proc */
#endif
);

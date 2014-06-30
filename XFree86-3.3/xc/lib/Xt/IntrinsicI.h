/* $TOG: IntrinsicI.h /main/47 1997/05/15 17:30:09 kaleb $ */

/***********************************************************

Copyright (c) 1987, 1988, 1994  X Consortium

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
/* $XFree86: xc/lib/Xt/IntrinsicI.h,v 3.2.2.1 1997/05/17 12:24:55 dawes Exp $ */

#ifndef _XtintrinsicI_h
#define _XtintrinsicI_h

#include "Xtos.h"
#include "IntrinsicP.h"
#ifdef WIN32
#define _WILLWINSOCK_
#endif
#include <X11/Xos.h>

#include "Object.h"
#include "RectObj.h"
#include "ObjectP.h"
#include "RectObjP.h"

#include "ConvertI.h"
#include "TranslateI.h"
#include "CallbackI.h"
#include "EventI.h"
#include "HookObjI.h"
#include "PassivGraI.h"
#include "ThreadsI.h"
#include "InitialI.h"
#include "ResourceI.h"
#include "StringDefs.h"

#define RectObjClassFlag	0x02
#define WidgetClassFlag		0x04
#define CompositeClassFlag	0x08
#define ConstraintClassFlag	0x10
#define ShellClassFlag		0x20
#define WMShellClassFlag	0x40
#define TopLevelClassFlag	0x80

/*
 * The following macros, though very handy, are not suitable for
 * IntrinsicP.h as they violate the rule that arguments are to
 * be evaluated exactly once.
 */

#define XtDisplayOfObject(object) \
    (XtIsWidget(object) ? (object)->core.screen->display : \
    _XtIsHookObject(object) ? ((HookObject)(object))->hooks.screen->display : \
    _XtWindowedAncestor(object)->core.screen->display)

#define XtScreenOfObject(object) \
    (XtIsWidget(object) ? (object)->core.screen : \
    _XtIsHookObject(object) ? ((HookObject)(object))->hooks.screen : \
    _XtWindowedAncestor(object)->core.screen)

#define XtWindowOfObject(object) \
    ((XtIsWidget(object) ? (object) : _XtWindowedAncestor(object)) \
     ->core.window)

#define XtIsManaged(object) \
    (XtIsRectObj(object) ? (object)->core.managed : False)

#define XtIsSensitive(object) \
    (XtIsRectObj(object) ? ((object)->core.sensitive && \
			    (object)->core.ancestor_sensitive) : False)


/****************************************************************
 *
 * Byte utilities
 *
 ****************************************************************/

#define _XBCOPYFUNC _XtBcopy
#include <X11/Xfuncs.h>

/* If the alignment characteristics of your machine are right, these may be
   faster */

#ifdef UNALIGNED

#define XtMemmove(dst, src, size)			    \
    if (size == sizeof(int))				    \
	*((int *) (dst)) = *((int *) (src));		    \
    else if (size == sizeof(char))			    \
	*((char *) (dst)) = *((char *) (src));		    \
    else if (size == sizeof(short))			    \
	*((short *) (dst)) = *((short *) (src));	    \
    else						    \
	(void) memcpy((char *) (dst), (char *) (src), (int) (size))

#define XtBZero(dst, size)			    \
    if (size == sizeof(int))				    \
	*((int *) (dst)) = 0;				    \
    else						    \
	bzero((char *) (dst), (int) (size))

#define XtMemcmp(b1, b2, size)				    \
    (size == sizeof(int) ?				    \
	*((int *) (b1)) != *((int *) (b2))		    \
    :   memcmp((char *) (b1), (char *) (b2), (int) (size))  \
    )

#else

#define XtMemmove(dst, src, size)	\
	(void) memcpy((char *) (dst), (char *) (src), (int) (size))

#define XtBZero(dst, size) 	\
	bzero((char *) (dst), (int) (size))

#define XtMemcmp(b1, b2, size) 		\
	memcmp((char *) (b1), (char *) (b2), (int) (size))

#endif


/****************************************************************
 *
 * Stack cache allocation/free
 *
 ****************************************************************/

#define XtStackAlloc(size, stack_cache_array)     \
    ((size) <= sizeof(stack_cache_array)	  \
    ?  (XtPointer)(stack_cache_array)		  \
    :  XtMalloc((unsigned)(size)))

#define XtStackFree(pointer, stack_cache_array) \
    if ((pointer) != ((XtPointer)(stack_cache_array))) XtFree(pointer); else

/***************************************************************
 *
 * Filename defines
 *
 **************************************************************/

/* used by XtResolvePathname */
#ifndef XFILESEARCHPATHDEFAULT
#define XFILESEARCHPATHDEFAULT "/usr/lib/X11/%L/%T/%N%S:/usr/lib/X11/%l/%T/%N%S:/usr/lib/X11/%T/%N%S"
#endif

/* the following two were both "X Toolkit " prior to R4 */
#ifndef XTERROR_PREFIX
#define XTERROR_PREFIX ""
#endif

#ifndef XTWARNING_PREFIX
#define XTWARNING_PREFIX ""
#endif

#ifndef ERRORDB
#define ERRORDB "/usr/lib/X11/XtErrorDB"
#endif

extern String XtCXtToolkitError;

extern void _XtAllocError(
#if NeedFunctionPrototypes
    String	/* alloc_type */
#endif
);

extern void _XtCompileResourceList(
#if NeedFunctionPrototypes
    XtResourceList 	/* resources */,
    Cardinal 		/* num_resources */
#endif
);

extern XtGeometryResult _XtMakeGeometryRequest(
#if NeedFunctionPrototypes
    Widget 		/* widget */,
    XtWidgetGeometry*	/* request */,
    XtWidgetGeometry*	/* reply_return */,
    Boolean*		/* clear_rect_obj */
#endif
);

extern Boolean _XtIsHookObject(
#if NeedFunctionPrototypes
    Widget      /* widget */
#endif
);

extern void _XtAddShellToHookObj(
#if NeedFunctionPrototypes
    Widget      /* widget */
#endif
);

/** GeoTattler stuff */

#ifdef XT_GEO_TATTLER

extern void _XtGeoTab ();
extern void _XtGeoTrace (
#if NeedVarargsPrototypes
			    Widget widget, 
			    ...
#endif
);

#define CALLGEOTAT(f) f

#else /* XT_GEO_TATTLER */

#define CALLGEOTAT(f) 

#endif /* XT_GEO_TATTLER */

#ifndef XTTRACEMEMORY

extern char* __XtMalloc (
#if NeedFunctionPrototypes
    unsigned	/* size */
#endif
);
extern char* __XtCalloc (
#if NeedFunctionPrototypes
    unsigned	/* num */,
    unsigned	/* size */
#endif
);

#else

#define __XtMalloc XtMalloc
#define __XtCalloc XtCalloc
#endif

#endif /* _XtintrinsicI_h */
/* DON'T ADD STUFF AFTER THIS #endif */

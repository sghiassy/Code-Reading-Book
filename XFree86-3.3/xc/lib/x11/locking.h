/* $XConsortium: locking.h,v 1.15 94/04/17 20:22:14 gildea Exp $ */
/* 

Copyright (c) 1992  X Consortium

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
 * Author: Stephen Gildea, MIT X Consortium
 *
 * locking.h - data types for C Threads locking.
 * Used by XlibInt.c, locking.c, LockDis.c
 */

#ifndef _X_locking_H_
#define _X_locking_H_

#define xmalloc(s) Xmalloc(s)
#define xfree(s) Xfree(s)
#include <X11/Xthreads.h>

struct _XCVList {
    xcondition_t cv;
    xReply *buf;
    struct _XCVList *next;
};

extern xthread_t (*_Xthread_self_fn)( /* in XlibInt.c */
#if NeedFunctionPrototypes
    void
#endif
);

/* Display->lock is a pointer to one of these */

struct _XLockInfo {
	xmutex_t mutex;		/* mutex for critical sections */
	int reply_bytes_left;	/* nbytes of the reply still to read */
	Bool reply_was_read;	/* _XReadEvents read a reply for _XReply */
	struct _XCVList *reply_awaiters; /* list of CVs for _XReply */
	struct _XCVList **reply_awaiters_tail;
	struct _XCVList *event_awaiters; /* list of CVs for _XReadEvents */
	struct _XCVList **event_awaiters_tail;
	Bool reply_first;	/* who may read, reply queue or event queue */
	/* for XLockDisplay */
	int locking_level;	/* how many times into XLockDisplay we are */
	xthread_t locking_thread; /* thread that did XLockDisplay */
	xcondition_t cv;	/* wait if another thread has XLockDisplay */
	xthread_t reading_thread; /* cache */
	xthread_t conni_thread;	/* thread in XProcessInternalConnection */
	xcondition_t writers;	/* wait for writable */
	int num_free_cvls;
	struct _XCVList *free_cvls;
	/* used only in XlibInt.c */
	void (*pop_reader)(
#if NeedNestedPrototypes
			   Display* /* dpy */,
			   struct _XCVList** /* list */,
			   struct _XCVList*** /* tail */
#endif
			   );
	struct _XCVList *(*push_reader)(
#if NeedNestedPrototypes
					Display *	   /* dpy */,
					struct _XCVList*** /* tail */
#endif
					);
	void (*condition_wait)(
#if NeedNestedPrototypes
			       xcondition_t /* cv */,
			       xmutex_t /* mutex */
#if defined(XTHREADS_WARN) || defined(XTHREADS_FILE_LINE)
			       , char* /* file */,
			       int /* line */
#endif
#endif
			       );
	void (*internal_lock_display)(
#if NeedNestedPrototypes
				      Display* /* dpy */,
				      Bool /* wskip */
#if defined(XTHREADS_WARN) || defined(XTHREADS_FILE_LINE)
				      , char* /* file */,
				      int /* line */
#endif
#endif
				      );
	/* used in XlibInt.c and locking.c */
	void (*condition_signal)(
#if NeedNestedPrototypes
				 xcondition_t /* cv */
#if defined(XTHREADS_WARN) || defined(XTHREADS_FILE_LINE)
				 , char* /* file */,
				 int /* line */
#endif
#endif
				 );
	void (*condition_broadcast)(
#if NeedNestedPrototypes
				 xcondition_t /* cv */
#if defined(XTHREADS_WARN) || defined(XTHREADS_FILE_LINE)
				 , char* /* file */,
				 int /* line */
#endif
#endif
				    );
	/* used in XlibInt.c and XLockDis.c */
	void (*lock_wait)(
#if NeedNestedPrototypes
			  Display* /* dpy */
#endif
			  );
	void (*user_lock_display)(
#if NeedNestedPrototypes
				  Display* /* dpy */
#endif
				  );
	void (*user_unlock_display)(
#if NeedNestedPrototypes
				    Display* /* dpy */
#endif
				    );
	struct _XCVList *(*create_cvl)(
#if NeedNestedPrototypes
				       Display * /* dpy */
#endif
				       );
};

#define UnlockNextEventReader(d) if ((d)->lock) \
    (*(d)->lock->pop_reader)((d),&(d)->lock->event_awaiters,&(d)->lock->event_awaiters_tail)

#if defined(XTHREADS_WARN) || defined(XTHREADS_FILE_LINE)
#define ConditionWait(d,c) if ((d)->lock) \
	(*(d)->lock->condition_wait)(c, (d)->lock->mutex,__FILE__,__LINE__)
#define ConditionSignal(d,c) if ((d)->lock) \
	(*(d)->lock->condition_signal)(c,__FILE__,__LINE__)
#define ConditionBroadcast(d,c) if ((d)->lock) \
	(*(d)->lock->condition_broadcast)(c,__FILE__,__LINE__)
#else
#define ConditionWait(d,c) if ((d)->lock) \
	(*(d)->lock->condition_wait)(c, (d)->lock->mutex)
#define ConditionSignal(d,c) if ((d)->lock) \
	(*(d)->lock->condition_signal)(c)
#define ConditionBroadcast(d,c) if ((d)->lock) \
	(*(d)->lock->condition_broadcast)(c)
#endif

typedef struct _LockInfoRec {
	xmutex_t	lock;
} LockInfoRec;

#endif /* _X_locking_H_ */

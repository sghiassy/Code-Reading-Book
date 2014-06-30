/* $XConsortium: util.h /main/5 1996/11/15 21:29:45 rws $ */
/*
 * Copyright 1994 Network Computing Devices, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name Network Computing Devices, Inc. not be
 * used in advertising or publicity pertaining to distribution of this 
 * software without specific, written prior permission.
 * 
 * THIS SOFTWARE IS PROVIDED `AS-IS'.  NETWORK COMPUTING DEVICES, INC.,
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING WITHOUT
 * LIMITATION ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NONINFRINGEMENT.  IN NO EVENT SHALL NETWORK
 * COMPUTING DEVICES, INC., BE LIABLE FOR ANY DAMAGES WHATSOEVER, INCLUDING
 * SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES, INCLUDING LOSS OF USE, DATA,
 * OR PROFITS, EVEN IF ADVISED OF THE POSSIBILITY THEREOF, AND REGARDLESS OF
 * WHETHER IN AN ACTION IN CONTRACT, TORT OR NEGLIGENCE, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */
#ifndef	_UTIL_H_
#define	_UTIL_H_

#ifdef SIGNALRETURNSINT
#define SIGVAL int
#else
#define SIGVAL void
#endif

typedef SIGVAL (*OsSigHandlerPtr)(
#if NeedFunctionPrototypes
    int /* sig */
#endif
);

extern OsSigHandlerPtr OsSignal(
#if NeedFunctionPrototypes
    int /*sig*/,
    OsSigHandlerPtr /*handler*/
#endif
);

extern void AutoResetServer(
#if NeedFunctionPrototypes
    int /*sig*/
#endif
);

extern void GiveUp(
#if NeedFunctionPrototypes
    int /*sig*/
#endif
);

extern void Error(
#if NeedFunctionPrototypes
    char * /*str*/
#endif
);

extern CARD32 GetTimeInMillis(
#if NeedFunctionPrototypes
    void
#endif
);

extern int AdjustWaitForDelay(
#if NeedFunctionPrototypes
    pointer /*waitTime*/,
    unsigned long /*newdelay*/
#endif
);

extern void UseMsg(
#if NeedFunctionPrototypes
    void
#endif
);

extern void ProcessCommandLine(
#if NeedFunctionPrototypes
    int /*argc*/,
    char * /*argv*/[]
#endif
);

#define xalloc(size) Xalloc((unsigned long)(size))
#define xcalloc(size) Xcalloc((unsigned long)(size))
#define xrealloc(ptr, size) Xrealloc((pointer)(ptr), (unsigned long)(size))
#define xfree(ptr) Xfree((pointer)(ptr))

extern unsigned long *Xalloc(
#if NeedFunctionPrototypes
    unsigned long /*amount*/
#endif
);

extern unsigned long *Xcalloc(
#if NeedFunctionPrototypes
    unsigned long /*amount*/
#endif
);

extern unsigned long *Xrealloc(
#if NeedFunctionPrototypes
    pointer /*ptr*/,
    unsigned long /*amount*/
#endif
);

extern void Xfree(
#if NeedFunctionPrototypes
    pointer /*ptr*/
#endif
);

extern void OsInitAllocator(
#if NeedFunctionPrototypes
    void
#endif
);

extern void AuditF(
#if NeedVarargsPrototypes
    char * /*f*/,
    ...
#endif
);

extern void FatalError(
#if NeedVarargsPrototypes
    char * /*f*/,
    ...
#endif
);

extern void ErrorF(
#if NeedVarargsPrototypes
    char * /*f*/,
    ...
#endif
);

extern char *strnalloc(
#if NeedFunctionPrototypes
    char * /*str*/,
    int /*len*/
#endif
);

typedef struct _WorkQueue	*WorkQueuePtr;

extern void ProcessWorkQueue(
#if NeedFunctionPrototypes
    void
#endif
);

extern Bool QueueWorkProc(
#if NeedFunctionPrototypes
    Bool (* /*function*/)(),
    ClientPtr /*client*/,
    pointer /*closure*/
#endif
);

extern Bool ClientSleep(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    Bool (* /*function*/)(),
    pointer /*closure*/
#endif
);

extern Bool ClientSignal(
#if NeedFunctionPrototypes
    ClientPtr /*client*/
#endif
);

extern void ClientWakeup(
#if NeedFunctionPrototypes
    ClientPtr /*client*/
#endif
);

extern Bool ClientIsAsleep(
#if NeedFunctionPrototypes
    ClientPtr /*client*/
#endif
);

#endif				/* _UTIL_H_ */

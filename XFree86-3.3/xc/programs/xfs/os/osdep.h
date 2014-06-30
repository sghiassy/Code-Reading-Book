/* $XConsortium: osdep.h /main/10 1996/08/30 14:22:08 kaleb $ */
/* $XFree86: xc/programs/xfs/os/osdep.h,v 3.3 1996/12/23 07:11:20 dawes Exp $ */
/*
Copyright (c) 1987  X Consortium

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
 * Copyright 1990, 1991 Network Computing Devices;
 * Portions Copyright 1987 by Digital Equipment Corporation 
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Network Computing Devices,
 * or Digital not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Network Computing Devices, or Digital
 * make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * NETWORK COMPUTING DEVICES, AND DIGITAL DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL NETWORK COMPUTING DEVICES, OR DIGITAL BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * @(#)osdep.h	4.1	5/2/91
 *
 */

#ifndef _OSDEP_H_
#define	_OSDEP_H_

#define	BOTIMEOUT	200	/* in milliseconds */
#define	BUFSIZE		4096
#define	BUFWATERMARK	8192
#define	MAXBUFSIZE	(1 << 18)

#ifndef sgi	    /* SGI defines OPEN_MAX in a useless way */
#ifndef X_NOT_POSIX
#ifdef _POSIX_SOURCE
#include <limits.h>
#else
#define _POSIX_SOURCE
#include <limits.h>
#undef _POSIX_SOURCE
#endif
#endif
#endif

#ifndef OPEN_MAX
#ifdef __EMX__
#define OPEN_MAX 256
#else
#ifdef SVR4
#ifdef SCO324
#define OPEN_MAX sysconf(_SC_OPEN_MAX)
#else
#define OPEN_MAX 128
#endif
#else
#include <sys/param.h>
#ifndef OPEN_MAX
#if defined(NOFILE) && !defined(NOFILES_MAX)
#define OPEN_MAX NOFILE
#else
#define OPEN_MAX NOFILES_MAX
#endif
#endif
#endif
#endif
#endif

#if OPEN_MAX <= 128		/* 128 is value of MAXCLIENTS */
#define MAXSOCKS (OPEN_MAX - 1)
#else
#define MAXSOCKS 128
#endif

#ifndef NULL
#define NULL 0
#endif

typedef struct _connectionInput {
    struct _connectionInput *next;
    char       *buffer;		/* contains current client input */
    char       *bufptr;		/* pointer to current start of data */
    int         bufcnt;		/* count of bytes in buffer */
    int         lenLastReq;
    int         size;
}           ConnectionInput, *ConnectionInputPtr;

typedef struct _connectionOutput {
    struct _connectionOutput *next;
    int         size;
    unsigned char *buf;
    int         count;
}           ConnectionOutput, *ConnectionOutputPtr;

typedef struct _osComm {
    int         fd;
    ConnectionInputPtr input;
    ConnectionOutputPtr output;
    long        conn_time;	/* timestamp if not established, else 0  */
    struct _XtransConnInfo *trans_conn; /* transport connection object */
}           OsCommRec, *OsCommPtr;

extern Bool CloneSelf;
extern Bool UseSyslog;

#endif				/* _OSDEP_H_ */

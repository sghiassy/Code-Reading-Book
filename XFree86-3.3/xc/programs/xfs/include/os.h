/* $XConsortium: os.h /main/11 1995/10/04 07:51:34 dpw $ */
/* $XFree86: xc/programs/xfs/include/os.h,v 3.2 1996/01/05 13:21:30 dawes Exp $ */
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
 * $NCDId: @(#)os.h,v 4.2 1991/05/10 07:59:16 lemke Exp $
 *
 */

#ifndef	_OS_H_
#define	_OS_H_

#include "FSproto.h"
#include "misc.h"
#define ALLOCATE_LOCAL_FALLBACK(_size) FSalloc((unsigned long)_size)
#define DEALLOCATE_LOCAL_FALLBACK(_ptr) FSfree((pointer)_ptr)
#include "X11/Xalloca.h"

#define	MAX_REQUEST_SIZE	16384

extern unsigned long *FSalloc();
extern unsigned long *FSrealloc();
extern void FSfree();

#define	fsalloc(size)		FSalloc((unsigned long)size)
#define	fsrealloc(ptr, size)	FSrealloc((pointer)ptr, (unsigned long)size)
#define	fsfree(ptr)		FSfree((pointer)ptr)

int         ReadRequest();

Bool        CloseDownConnection();
void        CreateSockets();
void        FlushAllOuput();
long        GetTimeInMIllis();
void        Error();
void        InitErrors();
void        CloseErrors();
void        NoticeF();
void        ErrorF();
void        FatalError();
void        SetConfigValues();

typedef pointer FID;
typedef struct _FontPathRec *FontPathPtr;

FontPathPtr expand_font_name_pattern();

typedef struct _alt_server *AlternateServerPtr;
typedef struct _auth *AuthPtr;

extern int  ListCatalogues();
extern int  ListAlternateServers();

#endif				/* _OS_H_ */

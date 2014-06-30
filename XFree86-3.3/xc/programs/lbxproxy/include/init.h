/* $XConsortium: init.h /main/3 1996/12/03 14:22:04 kaleb $ */
/*
 * Copyright 1992 Network Computing Devices
 * Copyright 1996 X Consortium, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of NCD. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  NCD. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * NCD. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL NCD.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#ifndef _INIT_H_
#define _INIT_H_

extern struct _XDisplay* DisplayOpen(
#if NeedFunctionPrototypes
    char * /*dpy_name*/,
    int * /*request*/,
    int * /*event*/,
    int * /*error*/,
    int * /*sequencep*/
#endif
);

extern int DisplayConnectionNumber(
#if NeedFunctionPrototypes
    struct _XDisplay* /* dpy */
#endif
);

extern void DisplayGetConnSetup (
#if NeedFunctionPrototypes
    struct _XDisplay* /* dpy */,
    xConnSetup** /* tag_data */,
    int* /* len */,
    int /* change_type */,
    CARD32* /* changes */,
    int /* change_len */
#endif
);

#endif

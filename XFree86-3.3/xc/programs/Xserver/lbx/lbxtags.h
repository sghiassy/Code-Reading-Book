/* $XConsortium: lbxtags.h /main/7 1996/11/12 09:31:19 rws $ */
/*
 * Copyright 1993 Network Computing Devices, Inc.
 * Copyright 1996 X Consortium, Inc.
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

#ifndef _LBXTAGS_H_
#define _LBXTAGS_H_
#include	"lbxserve.h"

#include	"os.h"
#include	"opaque.h"
#include	"resource.h"
#include	"X.h"
#include	"Xproto.h"

typedef struct _tagdata {
    XID         tid;
    short       data_type;
    unsigned char sent_to_proxy[(MAX_NUM_PROXIES + 7) / 8];
    int		size;
    pointer     tdata;
    XID		*global;
}           TagDataRec;

typedef struct _tagdata *TagData;

extern TagData TagGetTag(
#if NeedFunctionPrototypes
    XID         /*tid*/
#endif
);

extern XID  TagNewTag(
#if NeedFunctionPrototypes
    void
#endif
);

extern void TagClearProxy(
#if NeedFunctionPrototypes
    XID         /*tid*/,
    int         /*pid*/
#endif
);

extern void TagMarkProxy(
#if NeedFunctionPrototypes
    XID         /*tid*/,
    int         /*pid*/
#endif
);

extern Bool TagProxyMarked(
#if NeedFunctionPrototypes
    XID         /*tid*/,
    int         /*pid*/
#endif
);

extern void TagDeleteTag(
#if NeedFunctionPrototypes
    XID         /*tid*/
#endif
);

extern XID TagSaveTag(
#if NeedFunctionPrototypes
    int         /*dtype*/,
    int         /*size*/,
    pointer     /*data*/,
    XID*        /*global*/
#endif
);

#endif				/* _LBXTAGS_H_ */

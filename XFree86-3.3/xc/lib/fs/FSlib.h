/* $XConsortium: FSlib.h,v 1.9 94/04/17 20:15:20 mor Exp $ */

/*
 * Copyright 1990 Network Computing Devices;
 * Portions Copyright 1987 by Digital Equipment Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software 
 * and its documentation for any purpose is hereby granted without fee, 
 * provided that the above copyright notice appear in all copies and 
 * that both that copyright notice and this permission notice appear 
 * in supporting documentation, and that the names of Network Computing 
 * Devices or Digital not be used in advertising or publicity pertaining 
 * to distribution of the software without specific, written prior 
 * permission. Network Computing Devices or Digital make no representations 
 * about the suitability of this software for any purpose.  It is provided 
 * "as is" without express or implied warranty.
 *
 * NETWORK COMPUTING DEVICES AND  DIGITAL DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL NETWORK COMPUTING DEVICES
 * OR DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES 
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
 * SOFTWARE.
 */

/*

Copyright (c) 1987, 1994  X Consortium

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
 * Font server C interface library
 */

#ifndef _FSLIB_H_
#define _FSLIB_H_

#include	<FS.h>

#define	Bool	int
#define	Status	int
#define	True	1
#define	False	0

#define QueuedAlready 0
#define QueuedAfterReading 1
#define QueuedAfterFlush 2

#define	FSServerString(svr)	((svr)->server_name)
#define	FSVendorRelease(svr)	((svr)->release)
#define	FSProtocolVersion(svr)	((svr)->proto_version)
#define	FSServerVendor(svr)	((svr)->vendor)
#define	FSAuthorizationData(svr)	((svr)->auth_data)
#define	FSAlternateServers(svr)	((svr)->alternate_servers)
#define	FSNumAlternateServers(svr)	((svr)->num_alternates)

#ifdef QLength
#undef QLength
#endif
#define	QLength(svr)		((svr)->qlen)
#ifdef NextRequest
#undef NextRequest
#endif
#define	NextRequest(svr)	((svr)->request + 1)
#ifdef LastKnownRequestProcessed
#undef LastKnownRequestProcessed
#endif
#define LastKnownRequestProcessed(svr)   ((svr)->last_request_read)

#define	FSAllocID(svr)		((*(svr)->resource_alloc)((svr)))

typedef struct _alternate {
    Bool        subset;
    char       *name;
}           AlternateServer;

/* extension stuff */
typedef struct _FSExtData {
    int         number;		/* number returned by FSRegisterExtension */
    struct _FSExtData *next;	/* next item on list of data for structure */
    int         (*free_private) ();	/* called to free private storage */
    char       *private_data;	/* data private to this extension. */
}           FSExtData;


typedef struct {		/* public to extension, cannot be changed */
    int         extension;	/* extension number */
    int         major_opcode;	/* major op-code assigned by server */
    int         first_event;	/* first event number for the extension */
    int         first_error;	/* first error number for the extension */
}           FSExtCodes;

typedef struct _FSExtent {
    struct _FSExtent *next;	/* next in list */
    FSExtCodes  codes;		/* public information, all extension told */
    int         (*close_server) ();	/* routine to call when connection
					 * closed */
    int         (*error) ();	/* who to call when an error occurs */
    int         (*error_string) ();	/* routine to supply error string */
    char       *name;
}           _FSExtension;


/* server data structure */
typedef struct _FSServer {
    struct _FSServer *next;
    int         fd;
    int         proto_version;
    char       *vendor;
    int         byte_order;
    int         vnumber;
    int         release;
    int         resource_id;
    struct _FSQEvent *head,
               *tail;
    int         qlen;
    unsigned long last_request_read;
    unsigned long request;
    char       *last_req;
    char       *buffer;
    char       *bufptr;
    char       *bufmax;
    unsigned    max_request_size;
    char       *server_name;
    char       *auth_data;
    AlternateServer *alternate_servers;
    int         num_alternates;
    FSExtData  *ext_data;
    _FSExtension *ext_procs;
    int         ext_number;
    Bool        (*event_vec[132]) ();
                Status(*wire_vec[132]) ();
    char       *scratch_buffer;
    unsigned long scratch_length;
    int         (*synchandler) ();
    unsigned long flags;
    struct _XtransConnInfo *trans_conn; /* transport connection object */
}           FSServer;

typedef struct {
    int         type;
    unsigned long serial;
    Bool        send_event;
    FSServer   *server;
}           FSAnyEvent;

typedef struct {
    int         type;
    FSServer   *server;
    FSID        resourceid;
    unsigned long serial;
    unsigned char error_code;
    unsigned char request_code;
    unsigned char minor_code;
}           FSErrorEvent;

typedef union _FSEvent {
    int         type;
    FSAnyEvent  fsany;
}           FSEvent;

typedef struct _FSQEvent {
    struct _FSQEvent *next;
    FSEvent     event;
}           _FSQEvent;


/* protocol-related stuctures */

typedef unsigned long  FSBitmapFormat;
typedef unsigned long  FSBitmapFormatMask;

typedef struct _FSChar2b {
    unsigned char       high;
    unsigned char       low;
} FSChar2b;

typedef struct _FSRange {
    FSChar2b    min_char;
    FSChar2b    max_char;
} FSRange;

typedef struct _FSOffset {
    unsigned int position;
    unsigned int length;
} FSOffset;

/* use names as in xCharInfo? */
typedef struct _FSXCharInfo {
    short 	left;
    short       right;
    short 	width;
    short 	ascent;
    short       descent;
    unsigned short 	attributes;
} FSXCharInfo;

typedef struct _FSPropOffset {
    FSOffset	name;
    FSOffset	value;
    unsigned char 	type;
} FSPropOffset;

typedef struct _FSPropInfo {
    unsigned int	num_offsets;
    unsigned int	data_len;
} FSPropInfo;

/* should names match FontInfoRec? */
typedef struct _FSXFontInfoHeader {
    int		flags;
    FSRange     char_range;
    unsigned	draw_direction;
    FSChar2b    default_char;
    FSXCharInfo	min_bounds;
    FSXCharInfo	max_bounds;
    short 	font_ascent;
    short	font_descent;
} FSXFontInfoHeader;


/* function decls */

FSServer   *FSOpenServer();

extern int  (*FSSynchronize()) ();
extern int  (*FSSetAfterFunction()) ();

char       *FSServerName();
char      **FSListExtensions();
Bool        FSQueryExtension();

char      **FSListCatalogues();
char      **FSGetCatalogues();

long        FSMaxRequestSize();

char      **FSListFonts();
char      **FSListFontsWithXInfo();

Font        FSOpenBitmapFont();

#endif				/* _FSLIB_H_ */

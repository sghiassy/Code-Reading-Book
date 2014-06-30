/* $XConsortium: wire.h /main/15 1996/12/19 19:11:37 rws $ */
/*
 * Copyright 1992 Network Computing Devices
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
#include "misc.h"
#include "lbx.h"
#include "lbxdeltastr.h"

typedef struct _extinfo *ExtensionInfoPtr;

typedef struct _XServer {
    int			index;
    int			fd;
    int			lbxReq;
    int			lbxEvent;
    int			lbxError;
    Bool		initialized;
    ClientPtr		prev_exec;
    ClientPtr		send, recv;
    ClientPtr		serverClient;
    int			recv_expect;
    int			motion_allowed;
    Bool		wm_running;
    pointer		compHandle;
    lbxMotionCache	motionCache;
    ExtensionInfoPtr	extensions;
    LBXDeltasRec	indeltas;
    LBXDeltasRec	outdeltas;
    struct _XDisplay*	dpy;
} XServerRec;

#define MAX_SERVERS 128

extern XServerPtr   servers[];

extern void WriteReqToServer(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    int /*len*/,
    char * /*buf*/
#endif
);

extern void WriteToServer(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    int /*len*/,
    char * /*buf*/,
    Bool /* startOfRequest */
#endif
);

extern void _write_to_server(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    Bool /*compressed*/,
    int /*len*/,
    char* /*buf*/,
    Bool /*checkLarge*/,
    Bool /*startOfRequest*/
#endif
);

extern void WriteToServerUncompressed(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    int /*len*/,
    char * /*buf*/,
    Bool /* startOfRequest */
#endif
);

extern Bool NewClient(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    int /*setuplen*/
#endif
);

extern void CloseClient(
#if NeedFunctionPrototypes
    ClientPtr /*client*/
#endif
);

extern void ModifySequence(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    int /*num*/
#endif
);

extern void AllowMotion(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    int /*num*/
#endif
);

extern void SendIncrementPixel(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    XID /*cmap*/,
    unsigned long /*pixel*/
#endif
);

extern void SendGetModifierMapping(
#if NeedFunctionPrototypes
    ClientPtr /*client*/
#endif
);

extern void SendGetKeyboardMapping(
#if NeedFunctionPrototypes
    ClientPtr /*client*/
#endif
);

extern void SendQueryFont(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    XID /*fid*/
#endif
);

extern void SendChangeProperty(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    Window /*win*/,
    Atom /*prop*/,
    Atom /*type*/,
    int /*format*/,
    int /*mode*/,
    unsigned long /*num*/
#endif
);

extern void SendGetProperty(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    Window /*win*/,
    Atom /*prop*/,
    Atom /*type*/,
    Bool /*delete*/,
    unsigned long /*off*/,
    unsigned long /*len*/
#endif
);

extern void SendInvalidateTag(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    XID /*tag*/
#endif
);

extern void SendTagData(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    XID /*tag*/,
    unsigned long /*len*/,
    pointer /*data*/
#endif
);

extern void SendGetImage(
#if NeedFunctionPrototypes
    ClientPtr /*client*/,
    Drawable /*drawable*/,
    int /*x*/,
    int /*y*/,
    unsigned int /*width*/,
    unsigned int /*height*/,
    unsigned long /*planeMask*/,
    int /*format*/
#endif
);

extern int ServerProcStandardEvent(
#if NeedFunctionPrototypes
    ClientPtr /*sc*/
#endif
);

extern void CloseServer(
#if NeedFunctionPrototypes
    ClientPtr /*client*/
#endif
);

extern Bool ConnectToServer(
#if NeedFunctionPrototypes
    char * /*dpy_name*/
#endif
);

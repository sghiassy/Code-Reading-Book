/* $XConsortium: XF86VMode.c /main/2 1995/11/14 18:17:58 kaleb $ */
/* $XFree86: xc/lib/Xxf86vm/XF86VMode.c,v 3.20.2.2 1997/05/26 14:36:11 dawes Exp $ */
/*

Copyright (c) 1995  Kaleb S. KEITHLEY

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL Kaleb S. KEITHLEY BE LIABLE FOR ANY CLAIM, DAMAGES 
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of Kaleb S. KEITHLEY 
shall not be used in advertising or otherwise to promote the sale, use 
or other dealings in this Software without prior written authorization
from Kaleb S. KEITHLEY.

*/
/* $XConsortium: XF86VMode.c /main/4 1996/01/16 07:52:25 kaleb CHECKEDOUT $ */

/* THIS IS NOT AN X CONSORTIUM STANDARD */

#define NEED_EVENTS
#define NEED_REPLIES
#ifndef XBUILD_IN_CLIENT
#include "Xlibint.h"
#include "xf86vmstr.h"
#include "Xext.h"
#include "extutil.h"
#else
#include "lib/X11/Xlibint.h"
#include "include/extensions/xf86vmstr.h"
#include "include/extensions/Xext.h"
#include "include/extensions/extutil.h"
#endif

#ifdef DEBUG
#include <stdio.h>
#endif

#ifndef MODE_BAD
#define MODE_BAD 255
#endif

static XExtensionInfo _xf86vidmode_info_data;
static XExtensionInfo *xf86vidmode_info = &_xf86vidmode_info_data;
static char *xf86vidmode_extension_name = XF86VIDMODENAME;

#define XF86VidModeCheckExtension(dpy,i,val) \
  XextCheckExtension (dpy, i, xf86vidmode_extension_name, val)

/*****************************************************************************
 *                                                                           *
 *			   private utility routines                          *
 *                                                                           *
 *****************************************************************************/

static int close_display();
static /* const */ XExtensionHooks xf86vidmode_extension_hooks = {
    NULL,				/* create_gc */
    NULL,				/* copy_gc */
    NULL,				/* flush_gc */
    NULL,				/* free_gc */
    NULL,				/* create_font */
    NULL,				/* free_font */
    close_display,			/* close_display */
    NULL,				/* wire_to_event */
    NULL,				/* event_to_wire */
    NULL,				/* error */
    NULL,				/* error_string */
};

static XEXT_GENERATE_FIND_DISPLAY (find_display, xf86vidmode_info, 
				   xf86vidmode_extension_name, 
				   &xf86vidmode_extension_hooks, 
				   0, NULL)

static XEXT_GENERATE_CLOSE_DISPLAY (close_display, xf86vidmode_info)


/*****************************************************************************
 *                                                                           *
 *		    public XFree86-VidMode Extension routines                *
 *                                                                           *
 *****************************************************************************/

Bool XF86VidModeQueryExtension (dpy, event_basep, error_basep)
    Display *dpy;
    int *event_basep, *error_basep;
{
    XExtDisplayInfo *info = find_display (dpy);

    if (XextHasExtension(info)) {
	*event_basep = info->codes->first_event;
	*error_basep = info->codes->first_error;
	return True;
    } else {
	return False;
    }
}

Bool XF86VidModeQueryVersion(dpy, majorVersion, minorVersion)
    Display* dpy;
    int* majorVersion; 
    int* minorVersion;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeQueryVersionReply rep;
    xXF86VidModeQueryVersionReq *req;

    XF86VidModeCheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86VidModeQueryVersion, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeQueryVersion;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return False;
    }
    *majorVersion = rep.majorVersion;
    *minorVersion = rep.minorVersion;
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Bool XF86VidModeGetModeLine(dpy, screen, dotclock, modeline)
    Display* dpy;
    int screen;
    int* dotclock; 
    XF86VidModeModeLine* modeline;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeGetModeLineReply rep;
    xXF86VidModeGetModeLineReq *req;

    XF86VidModeCheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86VidModeGetModeLine, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeGetModeLine;
    req->screen = screen;
    if (!_XReply(dpy, (xReply *)&rep, 
        (SIZEOF(xXF86VidModeGetModeLineReply) - SIZEOF(xReply)) >> 2, xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return False;
    }
    *dotclock = rep.dotclock;
    modeline->hdisplay   = rep.hdisplay;
    modeline->hsyncstart = rep.hsyncstart;
    modeline->hsyncend   = rep.hsyncend;
    modeline->htotal     = rep.htotal;
    modeline->vdisplay   = rep.vdisplay;
    modeline->vsyncstart = rep.vsyncstart;
    modeline->vsyncend   = rep.vsyncend;
    modeline->vtotal     = rep.vtotal;
    modeline->flags      = rep.flags;
    modeline->privsize   = rep.privsize;
    if (rep.privsize > 0) {
	if (!(modeline->private = Xcalloc(rep.privsize, sizeof(INT32)))) {
	    _XEatData(dpy, (rep.privsize) * sizeof(INT32));
	    Xfree(modeline->private);
	    return False;
	}
	_XRead32(dpy, modeline->private, rep.privsize * sizeof(INT32));
    } else {
	modeline->private = NULL;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Bool XF86VidModeGetAllModeLines(dpy, screen, modecount, modelinesPtr)
    Display* dpy;
    int screen;
    int* modecount; 
    XF86VidModeModeInfo ***modelinesPtr;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeGetAllModeLinesReply rep;
    xXF86VidModeGetAllModeLinesReq *req;
    XF86VidModeModeInfo *mdinfptr, **modelines;
    xXF86VidModeModeInfo xmdline;
    int i;
    int majorVersion, minorVersion;
    Bool protocolBug = False;

    XF86VidModeCheckExtension (dpy, info, False);

    /*
     * Note: There was a bug in the protocol implementation in versions
     * 0.x with x < 8 (the .private field wasn't being passed over the wire).
     * Check the server's version, and accept the old format if appropriate.
     */

    XF86VidModeQueryVersion(dpy, &majorVersion, &minorVersion);
    if (majorVersion == 0 && minorVersion < 8) {
	protocolBug = True;
#ifdef DEBUG
	fprintf(stderr, "XF86VidModeGetAllModeLines: Warning: Xserver is"
		"running an old version (%d.%d)\n", majorVersion,
		minorVersion);
#endif
    }
    
    LockDisplay(dpy);
    GetReq(XF86VidModeGetAllModeLines, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeGetAllModeLines;
    req->screen = screen;
    if (!_XReply(dpy, (xReply *)&rep, 
        (SIZEOF(xXF86VidModeGetAllModeLinesReply) - SIZEOF(xReply)) >> 2, xFalse)) {
        UnlockDisplay(dpy);
        SyncHandle();
        return False;
    }
    *modecount = rep.modecount;

    if (!(modelines = (XF86VidModeModeInfo **) Xcalloc(rep.modecount,
                                          sizeof(XF86VidModeModeInfo *)
                                          +sizeof(XF86VidModeModeInfo)))) {
        _XEatData(dpy, (rep.modecount) * sizeof(xXF86VidModeModeInfo));
        Xfree(modelines);
        return False;
    }
    mdinfptr = (XF86VidModeModeInfo *) (
			    (char *) modelines
			    + rep.modecount*sizeof(XF86VidModeModeInfo *)
		    );

    for (i = 0; i < rep.modecount; i++) {
        modelines[i] = mdinfptr++;
        _XRead32(dpy, &xmdline, sizeof(xXF86VidModeModeInfo));
        modelines[i]->dotclock   = xmdline.dotclock;
        modelines[i]->hdisplay   = xmdline.hdisplay;
        modelines[i]->hsyncstart = xmdline.hsyncstart;
        modelines[i]->hsyncend   = xmdline.hsyncend;
        modelines[i]->htotal     = xmdline.htotal;
        modelines[i]->vdisplay   = xmdline.vdisplay;
        modelines[i]->vsyncstart = xmdline.vsyncstart;
        modelines[i]->vsyncend   = xmdline.vsyncend;
        modelines[i]->vtotal     = xmdline.vtotal;
        modelines[i]->flags      = xmdline.flags;
	if (protocolBug) {
	    modelines[i]->privsize = 0;
	    modelines[i]->private = NULL;
	} else {
            modelines[i]->privsize   = xmdline.privsize;
	    if (xmdline.privsize > 0) {
	        if (!(modelines[i]->private =
			    Xcalloc(xmdline.privsize, sizeof(INT32)))) {
		    _XEatData(dpy, (xmdline.privsize) * sizeof(INT32));
		    Xfree(modelines[i]->private);
	        } else {
		    _XRead32(dpy, modelines[i]->private,
			     xmdline.privsize * sizeof(INT32));
	        }
	    } else {
                modelines[i]->private = NULL;
	    }
	}
    }
    *modelinesPtr = modelines;
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Bool XF86VidModeAddModeLine (dpy, screen, newmodeline, aftermodeline)
    Display *dpy;
    int screen;
    XF86VidModeModeInfo* newmodeline;
    XF86VidModeModeInfo* aftermodeline;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeAddModeLineReq *req;

    XF86VidModeCheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86VidModeAddModeLine, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeAddModeLine;
    req->screen = screen;
    req->dotclock =	newmodeline->dotclock;
    req->hdisplay =	newmodeline->hdisplay;
    req->hsyncstart =	newmodeline->hsyncstart;
    req->hsyncend =	newmodeline->hsyncend;
    req->htotal =	newmodeline->htotal;
    req->vdisplay =	newmodeline->vdisplay;
    req->vsyncstart =	newmodeline->vsyncstart;
    req->vsyncend =	newmodeline->vsyncend;
    req->vtotal =	newmodeline->vtotal;
    req->flags =	newmodeline->flags;
    req->privsize =	newmodeline->privsize;
    if (aftermodeline != NULL) {
	req->after_dotclock =	aftermodeline->dotclock;
	req->after_hdisplay =	aftermodeline->hdisplay;
	req->after_hsyncstart =	aftermodeline->hsyncstart;
	req->after_hsyncend =	aftermodeline->hsyncend;
	req->after_htotal =	aftermodeline->htotal;
	req->after_vdisplay =	aftermodeline->vdisplay;
	req->after_vsyncstart =	aftermodeline->vsyncstart;
	req->after_vsyncend =	aftermodeline->vsyncend;
	req->after_vtotal =	aftermodeline->vtotal;
	req->after_flags =	aftermodeline->flags;
    } else {
	req->after_dotclock =	0;
	req->after_hdisplay =	0;
	req->after_hsyncstart =	0;
	req->after_hsyncend =	0;
	req->after_htotal =	0;
	req->after_vdisplay =	0;
	req->after_vsyncstart =	0;
	req->after_vsyncend =	0;
	req->after_vtotal =	0;
	req->after_flags =	0;
    }
    if (newmodeline->privsize) {
	req->length += newmodeline->privsize;
	Data32(dpy, (long *) newmodeline->private,
	       newmodeline->privsize * sizeof(INT32));
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Bool XF86VidModeDeleteModeLine (dpy, screen, modeline)
    Display *dpy;
    int screen;
    XF86VidModeModeInfo* modeline;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeDeleteModeLineReq *req;

    XF86VidModeCheckExtension (dpy, info, 0);

    LockDisplay(dpy);
    GetReq(XF86VidModeDeleteModeLine, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeDeleteModeLine;
    req->screen = screen;
    req->dotclock =	modeline->dotclock;
    req->hdisplay =	modeline->hdisplay;
    req->hsyncstart =	modeline->hsyncstart;
    req->hsyncend =	modeline->hsyncend;
    req->htotal =	modeline->htotal;
    req->vdisplay =	modeline->vdisplay;
    req->vsyncstart =	modeline->vsyncstart;
    req->vsyncend =	modeline->vsyncend;
    req->vtotal =	modeline->vtotal;
    req->flags =	modeline->flags;
    req->privsize =	modeline->privsize;
    if (modeline->privsize) {
	req->length += modeline->privsize;
	Data32(dpy, (long *) modeline->private,
	       modeline->privsize * sizeof(INT32));
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Bool XF86VidModeModModeLine (dpy, screen, modeline)
    Display *dpy;
    int screen;
    XF86VidModeModeLine* modeline;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeModModeLineReq *req;

    XF86VidModeCheckExtension (dpy, info, 0);

    LockDisplay(dpy);
    GetReq(XF86VidModeModModeLine, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeModModeLine;
    req->screen = screen;
    req->hdisplay =	modeline->hdisplay;
    req->hsyncstart =	modeline->hsyncstart;
    req->hsyncend =	modeline->hsyncend;
    req->htotal =	modeline->htotal;
    req->vdisplay =	modeline->vdisplay;
    req->vsyncstart =	modeline->vsyncstart;
    req->vsyncend =	modeline->vsyncend;
    req->vtotal =	modeline->vtotal;
    req->flags =	modeline->flags;
    req->privsize =	modeline->privsize;
    if (modeline->privsize) {
	req->length += modeline->privsize;
	Data32(dpy, (long *) modeline->private,
	       modeline->privsize * sizeof(INT32));
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Status XF86VidModeValidateModeLine (dpy, screen, modeline)
    Display *dpy;
    int screen;
    XF86VidModeModeInfo* modeline;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeValidateModeLineReq *req;
    xXF86VidModeValidateModeLineReply rep;

    XF86VidModeCheckExtension (dpy, info, 0);

    LockDisplay(dpy);
    GetReq(XF86VidModeValidateModeLine, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeValidateModeLine;
    req->screen = screen;
    req->dotclock =	modeline->dotclock;
    req->hdisplay =	modeline->hdisplay;
    req->hsyncstart =	modeline->hsyncstart;
    req->hsyncend =	modeline->hsyncend;
    req->htotal =	modeline->htotal;
    req->vdisplay =	modeline->vdisplay;
    req->vsyncstart =	modeline->vsyncstart;
    req->vsyncend =	modeline->vsyncend;
    req->vtotal =	modeline->vtotal;
    req->flags =	modeline->flags;
    req->privsize =	modeline->privsize;
    if (modeline->privsize) {
	req->length += modeline->privsize;
	Data32(dpy, (long *) modeline->private,
	       modeline->privsize * sizeof(INT32));
    }
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return MODE_BAD;
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return rep.status;
}

Bool XF86VidModeSwitchMode(dpy, screen, zoom)
    Display* dpy;
    int screen;
    int zoom;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeSwitchModeReq *req;

    XF86VidModeCheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86VidModeSwitchMode, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeSwitchMode;
    req->screen = screen;
    req->zoom = zoom;
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}
    
Bool XF86VidModeSwitchToMode(dpy, screen, modeline)
    Display* dpy;
    int screen;
    XF86VidModeModeInfo* modeline;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeSwitchToModeReq *req;
    int majorVersion, minorVersion;
    Bool protocolBug = False;

    XF86VidModeCheckExtension (dpy, info, False);

    /*
     * Note: There was a bug in the protocol implementation in versions
     * 0.x with x < 8 (the .private field wasn't expected to be sent over
     * the wire).  Check the server's version, and accept the old format
     * if appropriate.
     */

    XF86VidModeQueryVersion(dpy, &majorVersion, &minorVersion);
    if (majorVersion == 0 && minorVersion < 8) {
	protocolBug = True;
#ifdef DEBUG
	fprintf(stderr, "XF86VidModeSwitchToMode: Warning: Xserver is"
		"running an old version (%d.%d)\n", majorVersion,
		minorVersion);
#endif
    }
    
    LockDisplay(dpy);
    GetReq(XF86VidModeSwitchToMode, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeSwitchToMode;
    req->screen = screen;
    req->dotclock =	modeline->dotclock;
    req->hdisplay =	modeline->hdisplay;
    req->hsyncstart =	modeline->hsyncstart;
    req->hsyncend =	modeline->hsyncend;
    req->htotal =	modeline->htotal;
    req->vdisplay =	modeline->vdisplay;
    req->vsyncstart =	modeline->vsyncstart;
    req->vsyncend =	modeline->vsyncend;
    req->vtotal =	modeline->vtotal;
    req->flags =	modeline->flags;
    if (protocolBug) {
	req->privsize = 0;
    } else {
	req->privsize =	modeline->privsize;
	if (modeline->privsize) {
	    req->length += modeline->privsize;
	    Data32(dpy, (long *) modeline->private,
	           modeline->privsize * sizeof(INT32));
	}
    }
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}
    
Bool XF86VidModeLockModeSwitch(dpy, screen, lock)
    Display* dpy;
    int screen;
    int lock;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeLockModeSwitchReq *req;

    XF86VidModeCheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86VidModeLockModeSwitch, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeLockModeSwitch;
    req->screen = screen;
    req->lock = lock;
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}
    
Bool XF86VidModeGetMonitor(dpy, screen, monitor)
    Display* dpy;
    int screen;
    XF86VidModeMonitor* monitor;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeGetMonitorReply rep;
    xXF86VidModeGetMonitorReq *req;
    CARD32 syncrange;
    int i;

    XF86VidModeCheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86VidModeGetMonitor, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeGetMonitor;
    req->screen = screen;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return False;
    }
    monitor->nhsync = rep.nhsync;
    monitor->nvsync = rep.nvsync;
#if 0
    monitor->bandwidth = (float)rep.bandwidth / 1e6;
#endif
    if (rep.vendorLength) {
	if (!(monitor->vendor = (char *)Xcalloc(rep.vendorLength + 1, 1))) {
	    _XEatData(dpy, (rep.nhsync + rep.nvsync) * 4 +
		      (rep.vendorLength + 3 & ~3) + (rep.modelLength + 3 & ~3));
	    return False;
	}
    } else {
	monitor->vendor = NULL;
    }
    if (rep.modelLength) {
	if (!(monitor->model = Xcalloc(rep.modelLength + 1, 1))) {
	    _XEatData(dpy, (rep.nhsync + rep.nvsync) * 4 +
		      (rep.vendorLength + 3 & ~3) + (rep.modelLength + 3 & ~3));
	    if (monitor->vendor)
		Xfree(monitor->vendor);
	    return False;
	}
    } else {
	monitor->model = NULL;
    }
    if (!(monitor->hsync = Xcalloc(rep.nhsync, sizeof(XF86VidModeSyncRange)))) {
	_XEatData(dpy, (rep.nhsync + rep.nvsync) * 4 +
		  (rep.vendorLength + 3 & ~3) + (rep.modelLength + 3 & ~3));
	
	if (monitor->vendor)
	    Xfree(monitor->vendor);
	if (monitor->model)
	    Xfree(monitor->model);
	return False;
    }
    if (!(monitor->vsync = Xcalloc(rep.nvsync, sizeof(XF86VidModeSyncRange)))) {
	_XEatData(dpy, (rep.nhsync + rep.nvsync) * 4 +
		  (rep.vendorLength + 3 & ~3) + (rep.modelLength + 3 & ~3));
	if (monitor->vendor)
	    Xfree(monitor->vendor);
	if (monitor->model)
	    Xfree(monitor->model);
	Xfree(monitor->hsync);
	return False;
    }
    for (i = 0; i < rep.nhsync; i++) {
	_XRead32(dpy, (long *)&syncrange, 4);
	monitor->hsync[i].lo = (float)(syncrange & 0xFFFF) / 100.0;
	monitor->hsync[i].hi = (float)(syncrange >> 16) / 100.0;
    }
    for (i = 0; i < rep.nvsync; i++) {
	_XRead32(dpy, (long *)&syncrange, 4);
	monitor->vsync[i].lo = (float)(syncrange & 0xFFFF) / 100.0;
	monitor->vsync[i].hi = (float)(syncrange >> 16) / 100.0;
    }
    if (rep.vendorLength)
	_XReadPad(dpy, monitor->vendor, rep.vendorLength);
    else
	monitor->vendor = "";
    if (rep.modelLength)
	_XReadPad(dpy, monitor->model, rep.modelLength);
    else
	monitor->model = "";
	
    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Bool XF86VidModeGetViewPort(dpy, screen, x, y)
    Display* dpy;
    int screen;
    int *x, *y;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeGetViewPortReply rep;
    xXF86VidModeGetViewPortReq *req;
    CARD32 syncrange;
    int i;
    int majorVersion, minorVersion;
    Bool protocolBug = False;

    XF86VidModeCheckExtension (dpy, info, False);

    /*
     * Note: There was a bug in the protocol implementation in versions
     * 0.x with x < 8 (no reply was sent, so the client would hang)
     * Check the server's version, and don't wait for a reply with older
     * versions.
     */

    XF86VidModeQueryVersion(dpy, &majorVersion, &minorVersion);
    if (majorVersion == 0 && minorVersion < 8) {
	protocolBug = True;
#ifdef DEBUG
	fprintf(stderr, "XF86VidModeGetViewPort: Warning: Xserver is"
		"running an old version (%d.%d)\n", majorVersion,
		minorVersion);
#endif
    }
    LockDisplay(dpy);
    GetReq(XF86VidModeGetViewPort, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeGetViewPort;
    req->screen = screen;
    if (protocolBug) {
	*x = 0;
	*y = 0;
    } else {
	if (!_XReply(dpy, (xReply *)&rep, 0, xFalse)) {
	    UnlockDisplay(dpy);
	    SyncHandle();
	    return False;
	}
	*x = rep.x;
	*y = rep.y;
    }

    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Bool XF86VidModeSetViewPort(dpy, screen, x, y)
    Display* dpy;
    int screen;
    int x, y;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86VidModeSetViewPortReq *req;
    CARD32 syncrange;
    int i;

    XF86VidModeCheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86VidModeSetViewPort, req);
    req->reqType = info->codes->major_opcode;
    req->xf86vidmodeReqType = X_XF86VidModeSetViewPort;
    req->screen = screen;
    req->x = x;
    req->y = y;

    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}


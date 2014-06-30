/* $XFree86: xc/lib/Xxf86misc/XF86Misc.c,v 3.4 1997/01/18 06:52:26 dawes Exp $ */

/*
 * Copyright (c) 1995, 1996  The XFree86 Project, Inc
 */

/* THIS IS NOT AN X CONSORTIUM STANDARD */

#define NEED_EVENTS
#define NEED_REPLIES
#include "Xlibint.h"
#include "xf86mscstr.h"
#include "Xext.h"
#include "extutil.h"

static XExtensionInfo _xf86misc_info_data;
static XExtensionInfo *xf86misc_info = &_xf86misc_info_data;
static char *xf86misc_extension_name = XF86MISCNAME;

#define XF86MiscCheckExtension(dpy,i,val) \
  XextCheckExtension (dpy, i, xf86misc_extension_name, val)

/*****************************************************************************
 *                                                                           *
 *			   private utility routines                          *
 *                                                                           *
 *****************************************************************************/

static int close_display();
static /* const */ XExtensionHooks xf86misc_extension_hooks = {
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

static XEXT_GENERATE_FIND_DISPLAY (find_display, xf86misc_info, 
				   xf86misc_extension_name, 
				   &xf86misc_extension_hooks, 
				   0, NULL)

static XEXT_GENERATE_CLOSE_DISPLAY (close_display, xf86misc_info)


/*****************************************************************************
 *                                                                           *
 *		    public XFree86-Misc Extension routines                *
 *                                                                           *
 *****************************************************************************/

Bool XF86MiscQueryExtension (dpy, event_basep, error_basep)
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

Bool XF86MiscQueryVersion(dpy, majorVersion, minorVersion)
    Display* dpy;
    int* majorVersion; 
    int* minorVersion;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86MiscQueryVersionReply rep;
    xXF86MiscQueryVersionReq *req;

    XF86MiscCheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86MiscQueryVersion, req);
    req->reqType = info->codes->major_opcode;
    req->xf86miscReqType = X_XF86MiscQueryVersion;
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

Bool XF86MiscGetMouseSettings(dpy, mouseinfo)
    Display* dpy;
    XF86MiscMouseSettings *mouseinfo;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86MiscGetMouseSettingsReply rep;
    xXF86MiscGetMouseSettingsReq *req;
    int i;

    XF86MiscCheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86MiscGetMouseSettings, req);
    req->reqType = info->codes->major_opcode;
    req->xf86miscReqType = X_XF86MiscGetMouseSettings;
    if (!_XReply(dpy, (xReply *)&rep,
		(SIZEOF(xXF86MiscGetMouseSettingsReply) - SIZEOF(xReply))>>2,
		xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return False;
    }

    mouseinfo->type = rep.mousetype;
    mouseinfo->baudrate = rep.baudrate;
    mouseinfo->samplerate = rep.samplerate;
    mouseinfo->emulate3buttons = rep.emulate3buttons;
    mouseinfo->emulate3timeout = rep.emulate3timeout;
    mouseinfo->chordmiddle = rep.chordmiddle;
    mouseinfo->flags = rep.flags;
    if (rep.devnamelen > 0) {
        if (!(mouseinfo->device = Xcalloc(rep.devnamelen + 1, 1))) {
            _XEatData(dpy, (rep.devnamelen+3) & ~3);
            Xfree(mouseinfo->device);
            return False;
        }
        _XReadPad(dpy, mouseinfo->device, rep.devnamelen);
    } else
	mouseinfo->device = NULL;

    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Bool XF86MiscGetKbdSettings(dpy, kbdinfo)
    Display* dpy;
    XF86MiscKbdSettings *kbdinfo;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86MiscGetKbdSettingsReply rep;
    xXF86MiscGetKbdSettingsReq *req;
    int i;

    XF86MiscCheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86MiscGetKbdSettings, req);
    req->reqType = info->codes->major_opcode;
    req->xf86miscReqType = X_XF86MiscGetKbdSettings;
    if (!_XReply(dpy, (xReply *)&rep, 0, xFalse)) {
	UnlockDisplay(dpy);
	SyncHandle();
	return False;
    }

    kbdinfo->type = rep.kbdtype;
    kbdinfo->rate = rep.rate;
    kbdinfo->delay = rep.delay;
    kbdinfo->servnumlock = rep.servnumlock;

    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Bool XF86MiscSetMouseSettings(dpy, mouseinfo)
    Display* dpy;
    XF86MiscMouseSettings *mouseinfo;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86MiscSetMouseSettingsReq *req;
    int i;

    XF86MiscCheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86MiscSetMouseSettings, req);
    req->reqType = info->codes->major_opcode;
    req->xf86miscReqType = X_XF86MiscSetMouseSettings;
    req->mousetype = mouseinfo->type;
    req->baudrate = mouseinfo->baudrate;
    req->samplerate = mouseinfo->samplerate;
    req->emulate3buttons = mouseinfo->emulate3buttons;
    req->emulate3timeout = mouseinfo->emulate3timeout;
    req->chordmiddle = mouseinfo->chordmiddle;
    req->flags = mouseinfo->flags;

    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}

Bool XF86MiscSetKbdSettings(dpy, kbdinfo)
    Display* dpy;
    XF86MiscKbdSettings *kbdinfo;
{
    XExtDisplayInfo *info = find_display (dpy);
    xXF86MiscSetKbdSettingsReq *req;
    int i;

    XF86MiscCheckExtension (dpy, info, False);

    LockDisplay(dpy);
    GetReq(XF86MiscSetKbdSettings, req);
    req->reqType = info->codes->major_opcode;
    req->xf86miscReqType = X_XF86MiscSetKbdSettings;
    req->kbdtype = kbdinfo->type;
    req->rate = kbdinfo->rate;
    req->delay = kbdinfo->delay;
    req->servnumlock = kbdinfo->servnumlock;

    UnlockDisplay(dpy);
    SyncHandle();
    return True;
}


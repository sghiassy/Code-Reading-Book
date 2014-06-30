/* $XConsortium: XSecurity.c /main/5 1996/11/12 12:13:50 swick $ */
/*
Copyright (c) 1996  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.
*/

#include <X11/Xlibint.h>
#include "Xext.h"
#include "extutil.h"
#include "securstr.h"

static XExtensionInfo _Security_info_data;
static XExtensionInfo *Security_info = &_Security_info_data;
static char *Security_extension_name = SECURITY_EXTENSION_NAME;

#define SecurityCheckExtension(dpy,i,val) \
  XextCheckExtension (dpy, i, Security_extension_name, val)
#define SecuritySimpleCheckExtension(dpy,i) \
  XextSimpleCheckExtension (dpy, i, Security_extension_name)

#define SecurityGetReq(name,req,info) GetReq (name, req); \
        req->reqType = info->codes->major_opcode; \
        req->securityReqType = X_##name;

/*****************************************************************************
 *                                                                           *
 *			   private utility routines                          *
 *                                                                           *
 *****************************************************************************/

/*
 * find_display - locate the display info block
 */
static int close_display();
static Bool     wire_to_event();
static Status   event_to_wire();
static char *error_string();
static XExtensionHooks Security_extension_hooks = {
    NULL,                               /* create_gc */
    NULL,                               /* copy_gc */
    NULL,                               /* flush_gc */
    NULL,                               /* free_gc */
    NULL,                               /* create_font */
    NULL,                               /* free_font */
    close_display,                      /* close_display */
    wire_to_event,                      /* wire_to_event */
    event_to_wire,                      /* event_to_wire */
    NULL,                               /* error */
    error_string                        /* error_string */
};

static char    *security_error_list[] = {
    "BadAuthorization"
    "BadAuthorizationProtocol"
};

static XEXT_GENERATE_FIND_DISPLAY (find_display, Security_info,
				   Security_extension_name, 
				   &Security_extension_hooks, 
				   XSecurityNumberEvents, NULL)

static XEXT_GENERATE_CLOSE_DISPLAY (close_display, Security_info)

static 
XEXT_GENERATE_ERROR_STRING(error_string, Security_extension_name,
			   XSecurityNumberErrors, security_error_list)

static Bool     
wire_to_event(dpy, event, wire)
    Display        *dpy;
    XEvent         *event;
    xEvent         *wire;
{
    XExtDisplayInfo *info = find_display(dpy);

    SecurityCheckExtension (dpy, info, False);

    switch ((wire->u.u.type & 0x7F) - info->codes->first_event)
    {
	case XSecurityAuthorizationRevoked:
	{
	    xSecurityAuthorizationRevokedEvent *rwire =
		(xSecurityAuthorizationRevokedEvent *)wire;
	    XSecurityAuthorizationRevokedEvent *revent = 
		(XSecurityAuthorizationRevokedEvent *)event;

	  revent->type = rwire->type & 0x7F;
	  revent->serial = _XSetLastRequestRead(dpy,
						(xGenericReply *) wire);
	  revent->send_event = (rwire->type & 0x80) != 0;
	  revent->display = dpy;
	  revent->auth_id = rwire->authId;
	  return True;
	}
    }
    return False;
}

static Status 
event_to_wire(dpy, event, wire)
    Display        *dpy;
    XEvent         *event;
    xEvent         *wire;
{
    XExtDisplayInfo *info = find_display(dpy);

    SecurityCheckExtension(dpy, info, False);

    switch ((event->type & 0x7F) - info->codes->first_event)
    {
	case XSecurityAuthorizationRevoked:
	{
	    xSecurityAuthorizationRevokedEvent *rwire =
		(xSecurityAuthorizationRevokedEvent *)wire;
	    XSecurityAuthorizationRevokedEvent *revent = 
		(XSecurityAuthorizationRevokedEvent *)event;
	    rwire->type = revent->type | (revent->send_event ? 0x80 : 0);
	    rwire->sequenceNumber = revent->serial & 0xFFFF;
	    return True;
	}
    }
    return False;
}

/*****************************************************************************
 *                                                                           *
 *		       Security public interfaces                            *
 *                                                                           *
 *****************************************************************************/

Status XSecurityQueryExtension (
    Display *dpy,
    int *major_version_return,
    int *minor_version_return)
{
    XExtDisplayInfo *info = find_display (dpy);
    xSecurityQueryVersionReply rep;
    register xSecurityQueryVersionReq *req;

    if (!XextHasExtension (info))
        return (Status)0; /* failure */

    LockDisplay (dpy);
    SecurityGetReq (SecurityQueryVersion, req, info);
    req->majorVersion = SECURITY_MAJOR_VERSION;
    req->minorVersion = SECURITY_MINOR_VERSION;

    if (!_XReply (dpy, (xReply *) &rep, 0, xTrue)) {
	UnlockDisplay (dpy);
	SyncHandle ();
	return (Status)0; /* failure */
    }
    *major_version_return = rep.majorVersion;
    *minor_version_return = rep.minorVersion;
    UnlockDisplay (dpy);

    SyncHandle ();

    if (*major_version_return != SECURITY_MAJOR_VERSION)
        return (Status)0; /* failure */
    else
        return (Status)1; /* success */
}

Xauth *
XSecurityAllocXauth(void)
{
    return Xcalloc(1, sizeof(Xauth));
}

void
XSecurityFreeXauth(Xauth *auth)
{
    XFree(auth);
}

static int
Ones(Mask mask)
{
    register Mask y;

    y = (mask >> 1) &033333333333;
    y = mask - y - ((y >>1) & 033333333333);
    return (((y + (y >> 3)) & 030707070707) % 077);
}

Xauth *
XSecurityGenerateAuthorization(
    Display *dpy,
    Xauth *auth_in,
    unsigned long valuemask,
    XSecurityAuthorizationAttributes *attributes,
    XSecurityAuthorization *auth_id_return)
{
    XExtDisplayInfo *info = find_display (dpy);
    register xSecurityGenerateAuthorizationReq *req;
    xSecurityGenerateAuthorizationReply rep;
    char *auth_data;
    Xauth *auth_return;
    unsigned long values[3];
    unsigned long *value = values;
    unsigned int nvalues;

    *auth_id_return = 0;  /* in case we fail */

    /* make sure extension is available */

    SecurityCheckExtension (dpy, info, (Xauth *)NULL);

    LockDisplay(dpy);
    SecurityGetReq(SecurityGenerateAuthorization, req, info);

    req->nbytesAuthProto = auth_in->name_length;
    req->nbytesAuthData  = auth_in->data_length;

    /* adjust length to account for auth name and data */
    req->length += (auth_in->name_length + (unsigned)3) >> 2;
    req->length += (auth_in->data_length + (unsigned)3) >> 2;

    /* adjust length to account for list of values */
    req->valueMask = valuemask & XSecurityAllAuthorizationAttributes;
    nvalues = Ones(req->valueMask);
    req->length += nvalues;

    /* send auth name and data */
    Data(dpy, auth_in->name, auth_in->name_length);
    Data(dpy, auth_in->data, auth_in->data_length);

    /* send values */
    if (valuemask & XSecurityTimeout)	 *value++ = attributes->timeout;
    if (valuemask & XSecurityTrustLevel) *value++ = attributes->trust_level;
    if (valuemask & XSecurityGroup)	 *value++ = attributes->group;
    if (valuemask & XSecurityEventMask)	 *value++ = attributes->event_mask;

    nvalues <<= 2;
    Data32(dpy, (long *)values, (long)nvalues);

    if (!_XReply (dpy, (xReply *) &rep, 0, xFalse)) {
	UnlockDisplay (dpy);
	SyncHandle ();
	return (Xauth *)NULL;
    }

    *auth_id_return = rep.authId;

    /* Allocate space for the Xauth struct and the auth name and data all
     * in one hunk.  This lets XSecurityFreeXauth not have to care
     * about whether the auth was allocated here or in
     * XSecurityAllocXauth; in both cases, you just free one pointer.
     */

    if (auth_return = (Xauth *)Xcalloc(1,
		(sizeof(Xauth) + auth_in->name_length + rep.dataLength)))
    {
	auth_return->data_length = rep.dataLength;
	auth_return->data = (char *)&auth_return[1];
	_XReadPad(dpy, auth_return->data, (long)rep.dataLength);

	auth_return->name_length = auth_in->name_length;
	auth_return->name = auth_return->data + auth_return->data_length;
	memcpy(auth_return->name, auth_in->name, auth_return->name_length);
    }
    else
    {
	_XEatData(dpy, (unsigned long) (rep.dataLength + 3) & ~3);
    }

    UnlockDisplay (dpy);
    SyncHandle ();
    return auth_return;

} /* XSecurityGenerateAuthorization */

Status
XSecurityRevokeAuthorization(
    Display *dpy,
    XSecurityAuthorization auth_id)
{
    XExtDisplayInfo *info = find_display (dpy);
    xSecurityRevokeAuthorizationReq *req;
    
    SecurityCheckExtension (dpy, info, 0);
    LockDisplay(dpy);
    SecurityGetReq(SecurityRevokeAuthorization, req, info);
    req->authId = auth_id;
    UnlockDisplay (dpy);
    SyncHandle ();
    return 1;
} /* XSecurityRevokeAuthorization */

/* $XConsortium: wire.c /main/48 1996/12/19 19:13:03 rws $ */
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
/* $XFree86: xc/programs/lbxproxy/di/wire.c,v 1.3 1997/01/18 07:18:54 dawes Exp $ */

#include "lbx.h"
#include <stdio.h>
#include "wire.h"
#include "init.h"
#ifndef Lynx
#include <sys/uio.h>
#else
#include <uio.h>
#endif
#include <errno.h>
#include "proxyopts.h"
#include "swap.h"
#include "assert.h"
#include "os.h"
#include "resource.h"
#include "colormap.h"
#include "lbxext.h"
#include "atomcache.h"
#include <X11/ICE/ICElib.h>
#ifdef BIGREQS
#include "bigreqstr.h"
#endif

#define PM_Unable  0
#define PM_Success 1
#define PM_Failure 2


extern int  NewOutputPending;

#ifdef LBX_STATS
extern int  delta_out_total;
extern int  delta_out_attempts;
extern int  delta_out_hits;
extern int  delta_in_total;
extern int  delta_in_attempts;
extern int  delta_in_hits;

#endif

#ifdef DEBUG
int         lbxDebug = 0;

#endif

#define MAXBYTESDIFF		8

static unsigned char tempdeltabuf[256];

extern int ProcStandardRequest ();
static void LbxOnlyListenToOneClient();
static void LbxListenToAllClients();

/*
 * Any request that could be delta compressed comes through here
 */
void
WriteReqToServer(client, len, buf)
    ClientPtr   client;
    int         len;
    char       *buf;
{
    XServerPtr  server = client->server;
    xLbxDeltaReq *p = (xLbxDeltaReq *) tempdeltabuf;
    int         diffs;
    int         cindex;
    int         newlen;
    Bool        written = FALSE;

#ifdef LBX_STATS
    delta_out_total++;
#endif

    if (DELTA_CACHEABLE(&server->outdeltas, len)) {

#ifdef LBX_STATS
	delta_out_attempts++;
#endif

	if ((diffs = LBXDeltaMinDiffs(&server->outdeltas, buf, len,
			     min(MAXBYTESDIFF, (len - sz_xLbxDeltaReq) >> 1),
				      &cindex)) >= 0) {

#ifdef LBX_STATS
	    delta_out_hits++;
#endif

	    LBXEncodeDelta(&server->outdeltas, buf, diffs, cindex,
			   &tempdeltabuf[sz_xLbxDeltaReq]);
	    p->reqType = server->lbxReq;
	    p->lbxReqType = X_LbxDelta;
	    p->diffs = diffs;
	    p->cindex = cindex;
	    newlen = sz_xLbxDeltaReq + sz_xLbxDiffItem * diffs;
	    p->length = (newlen + 3) >> 2;
	    /* Don't byte swap -- lengths are always in proxy order */
	    WriteToServer(client, newlen, (char *) p, TRUE);
	    written = TRUE;
	}
	LBXAddDeltaOut(&server->outdeltas, buf, len);
    }
    if (!written) {
#ifdef BIGREQS
	if (len > (0xffff << 2)) {
	    xBigReq bigreq;
	    bigreq.reqType = ((xReq *)buf)->reqType;
	    bigreq.data = ((xReq *)buf)->data;
	    bigreq.zero = 0;
	    bigreq.length = (len + sizeof(xBigReq) - sizeof(xReq)) >> 2;
	    WriteToServer(client, sizeof(xBigReq), (char *)&bigreq, TRUE);
	    WriteToServer(client, len - sizeof(xReq), buf + sizeof(xReq),
			  FALSE);
	    return;
	}
#endif
	WriteToServer(client, len, buf, TRUE);
    }
}

void
_write_to_server(client, compressed, len, buf, checkLarge, startOfRequest)
    ClientPtr   client;
    Bool        compressed;
    int         len;
    char       *buf;
    Bool	checkLarge;
    Bool	startOfRequest;
{
    XServerPtr  server = client->server;
    unsigned reqSize;
    extern int nClients;

    if (server->serverClient->clientGone)
	return;

    if (checkLarge && client != clients[0] && nClients > 1 &&
	((client != server->prev_exec) || numLargeRequestsInQueue)) {
	/*
	 * Check if this is a large request only if there is more than
	 * one client and a different client was the last to execute or
	 * there are already large requests queued.
	 *
	 * If it is a large request, and there is room in the large request
	 * queue, add it to the queue.  lbxproxy will send the large request
	 * in chunks, preventing this client from hogging the wire.
	 *
	 * By checking that the previous client was someone else,
	 * we hope that we can prevent splitting a large request
	 * when the other clients appear to be idle (based on their past
	 * history).
	 */

	if (startOfRequest &&
	    (!(reqSize = (((xReq *) buf)->length) << 2) ||
	     reqSize >= LBX_LARGE_REQUEST_MIN_SIZE) &&
	    numLargeRequestsInQueue < LARGE_REQUEST_QUEUE_LEN) {
	    LbxLargeRequestRec *largeRequest;

	    if (!reqSize)
		reqSize = ((xBigReq *)buf)->length << 2;
	    largeRequest = (LbxLargeRequestRec *)
		xalloc (sizeof (LbxLargeRequestRec) + reqSize);

	    /*
	     * Add this large request to the queue
	     */

	    largeRequest->client = client;
	    largeRequest->compressed = compressed;
	    largeRequest->buf = (char *) largeRequest +
		sizeof (LbxLargeRequestRec);
	    memcpy (largeRequest->buf, buf, len);
	    largeRequest->totalBytes = reqSize;
	    largeRequest->bytesRead = len;
	    largeRequest->bytesWritten = 0;
	    client->largeRequest = largeRequest;

	    largeRequestQueue[numLargeRequestsInQueue++] = largeRequest;

	    /*
	     * Once we have the whole large request, we want to disable
	     * input from this client - we don't want to read new requests
	     * until we are done sending the whole large request.
	     */

	    if (reqSize == len)
		IgnoreClient(client);
	    
	    return;
	} else if (client->largeRequest) {

	    /*
	     * Append to the large request
	     */

	    char *dst = client->largeRequest->buf +
	        client->largeRequest->bytesRead;
	    memcpy (dst, buf, len);
	    client->largeRequest->bytesRead += len;

	    /*
	     * Once we have the whole large request, we want to disable
	     * input from this client - we don't want to read new requests
	     * until we are done sending the whole large request.
	     */

	    if (client->largeRequest->bytesRead ==
		client->largeRequest->totalBytes)
		IgnoreClient(client);

	    return;
	}
    }

    if (server->send != client) {
	xLbxSwitchReq s;

	DBG(DBG_SWITCH, (stderr, "switch downstream to %d\n", client->index));
	s.reqType = server->lbxReq;
	s.lbxReqType = X_LbxSwitch;
	s.length = 2;
	s.client = client->index;
	WriteToClient(server->serverClient, sizeof(s), &s);
	server->send = client;
    }
    DBG(DBG_IO, (stderr, "downstream %d len %d\n", client->index, len));
    if (compressed || !server->compHandle)
	WriteToClient(server->serverClient, len, buf);
    else
	UncompressWriteToClient(server->serverClient, len, buf);
}

void
WriteToServer(client, len, buf, startOfRequest)
    ClientPtr   client;
    int         len;
    char       *buf;
    Bool	startOfRequest;
{
    _write_to_server(client, TRUE, len, buf, TRUE, startOfRequest);
}

void
WriteToServerUncompressed(client, len, buf, startOfRequest)
    ClientPtr   client;
    int         len;
    char       *buf;
    Bool	startOfRequest;
{
    _write_to_server(client, FALSE, len, buf, TRUE, startOfRequest);
}

/* all these requests may need to be swapped back to the order of
 * ther client they're being executed for
 */
Bool
NewClient(client, setuplen)
    ClientPtr   client;
    int         setuplen;
{
    xLbxNewClientReq n;
    XServerPtr  server = client->server;

    DBG(DBG_CLIENT, (stderr, "new client %d\n", client->index));
    n.reqType = server->lbxReq;
    n.lbxReqType = X_LbxNewClient;
    n.length = 2 + (setuplen >> 2);
    n.client = client->index;
    if (clients[0]->swapped) {
	SwapNewClient(&n);
    }
    WriteToServer(clients[0], sizeof(n), (char *) &n, TRUE);
    ++server->serverClient->sequence;
    return TRUE;
}

void
CloseClient(client)
    ClientPtr   client;
{
    xLbxCloseClientReq n;
    XServerPtr  server = client->server;

    if (!client->server)
	return;

    if (client->server->serverClient == client)
	return;
    if (client->server->serverClient->clientGone)
	return;
    DBG(DBG_CLIENT, (stderr, "closing down client %d\n", client->index));
    if (client->closeDownMode != DestroyAll) {
	n.reqType = server->lbxReq;
	n.lbxReqType = X_LbxCloseClient;
	n.length = 2;
	n.client = client->index;
	if (client->swapped) {
	    SwapCloseClient(&n);
	}
	WriteReqToServer(client, sizeof(n), (char *) &n);
    }
}

void
ModifySequence(client, num)
    ClientPtr   client;
    int         num;
{
    xLbxModifySequenceReq req;
    XServerPtr  server = client->server;

    if (client->server->serverClient == client)
	return;
    req.reqType = server->lbxReq;
    req.lbxReqType = X_LbxModifySequence;
    req.length = 2;
    req.adjust = num;
    if (client->swapped) {
	SwapModifySequence(&req);
    }
    WriteReqToServer(client, sizeof(req), (char *) &req);
}

void
AllowMotion(client, num)
    ClientPtr   client;
    int         num;
{
    client->server->motion_allowed += num;
}

void
SendIncrementPixel(client, cmap, pixel)
    ClientPtr   client;
    XID         cmap;
    unsigned long pixel;
{
    xLbxIncrementPixelReq req;
    XServerPtr  server = client->server;

    if (client->server->serverClient == client)
	return;
    req.reqType = server->lbxReq;
    req.lbxReqType = X_LbxIncrementPixel;
    req.length = 3;
    req.cmap = cmap;
    req.pixel = pixel;
    if (client->swapped) {
	SwapIncrementPixel(&req);
    }
    WriteReqToServer(client, sizeof(req), (char *) &req);
}

void
SendAllocColor(client, cmap, pixel, red, green, blue)
    ClientPtr   client;
    XID         cmap;
    CARD16	red, green, blue;
{
    xLbxAllocColorReq req;

    req.reqType = client->server->lbxReq;
    req.lbxReqType = X_LbxAllocColor;
    req.length = sz_xLbxAllocColorReq >> 2;
    req.cmap = cmap;
    req.pixel = pixel;
    req.red = red;
    req.green = green;
    req.blue = blue;
    req.pad = 0;

    if (client->swapped)
	SwapAllocColor (&req);

    WriteReqToServer (client, sizeof(req), (char *) &req);
}

void
SendGetModifierMapping(client)
    ClientPtr   client;
{
    xLbxGetModifierMappingReq req;
    XServerPtr  server = client->server;

    if (client->server->serverClient == client)
	return;

    req.reqType = server->lbxReq;
    req.lbxReqType = X_LbxGetModifierMapping;
    req.length = 1;
    if (client->swapped) {
	SwapGetModifierMapping(&req);
    }
    WriteReqToServer(client, sizeof(req), (char *) &req);
}

void
SendGetKeyboardMapping(client)
    ClientPtr   client;
{
    xLbxGetKeyboardMappingReq req;
    XServerPtr  server = client->server;

    if (client->server->serverClient == client)
	return;

    /*
     * always ask for entire thing so tag always works, and pass on requested
     * subset
     */
    req.reqType = server->lbxReq;
    req.lbxReqType = X_LbxGetKeyboardMapping;
    req.length = 2;
    req.firstKeyCode = LBXMinKeyCode(client);
    req.count = LBXMaxKeyCode(client) - LBXMinKeyCode(client) + 1;
    req.pad1 = 0;
    if (client->swapped) {
	SwapGetKeyboardMapping(&req);
    }
    WriteReqToServer(client, sizeof(req), (char *) &req);
}

void
SendQueryFont(client, fid)
    ClientPtr   client;
    XID         fid;
{
    xLbxQueryFontReq req;
    XServerPtr  server = client->server;

    if (client->server->serverClient == client)
	return;

    req.reqType = server->lbxReq;
    req.lbxReqType = X_LbxQueryFont;
    req.length = 2;
    req.fid = fid;
    if (client->swapped) {
	SwapQueryFont(&req);
    }
    WriteReqToServer(client, sizeof(req), (char *) &req);
}

void
SendChangeProperty(client, win, prop, type, format, mode, num)
    ClientPtr   client;
    Window      win;
    Atom        prop,
                type;
    int         format,
                mode;
    unsigned long num;
{
    xLbxChangePropertyReq req;
    XServerPtr  server = client->server;

    if (client->server->serverClient == client)
	return;

    req.reqType = server->lbxReq;
    req.lbxReqType = X_LbxChangeProperty;
    req.length = 6;
    req.window = win;
    req.property = prop;
    req.type = type;
    req.format = format;
    req.mode = mode;
    req.nUnits = num;
    req.pad[0] = req.pad[1] = 0;
    if (client->swapped) {
	SwapChangeProperty(&req);
    }
    WriteReqToServer(client, sizeof(req), (char *) &req);
}

void
SendGetProperty(client, win, prop, type, delete, off, len)
    ClientPtr   client;
    Window      win;
    Atom        prop,
                type;
    Bool        delete;
    unsigned long off,
                len;
{
    xLbxGetPropertyReq req;
    XServerPtr  server = client->server;

    if (client->server->serverClient == client)
	return;

    req.reqType = server->lbxReq;
    req.lbxReqType = X_LbxGetProperty;
    req.length = 7;
    req.window = win;
    req.property = prop;
    req.type = type;
    req.delete = delete;
    req.longOffset = off;
    req.longLength = len;
    req.pad[0] = req.pad[1] = req.pad[2] = 0;
    if (client->swapped) {
	SwapGetProperty(&req);
    }
    WriteReqToServer(client, sizeof(req), (char *) &req);
}

void
SendInvalidateTag(client, tag)
    ClientPtr   client;
    XID         tag;
{
    xLbxInvalidateTagReq req;
    XServerPtr  server;

    if (!servers[0])		/* proxy resetting */
	return;
    if (!client)
	client = clients[0];	/* XXX watch multi-proxy */
    server = client->server;

    req.reqType = server->lbxReq;
    req.lbxReqType = X_LbxInvalidateTag;
    req.length = 2;
    req.tag = tag;
    /* need tag type ? */
    if (client->swapped) {
	SwapInvalidateTag(&req);
    }
    WriteReqToServer(client, sizeof(req), (char *) &req);
}

void
SendTagData(client, tag, len, data)
    ClientPtr   client;
    XID         tag;
    unsigned long len;
    pointer     data;
{
    xLbxTagDataReq req,
               *reqp;
    int         req_len;
    XServerPtr  server;

    client = clients[0];	/* XXX watch multi proxy */
    server = client->server;

    req_len = 3 + ((len + 3) >> 2);
    if (DELTA_CACHEABLE(&server->outdeltas, req_len << 2)) {
	reqp = (xLbxTagDataReq *) xalloc(req_len << 2);
	memcpy((pointer) (reqp + 1), data, len);
    } else {
	reqp = &req;
    }
    reqp->reqType = server->lbxReq;
    reqp->lbxReqType = X_LbxTagData;
    reqp->length = req_len;
    reqp->real_length = len;
    reqp->tag = tag;
    /* need tag type ? */
    if (client->swapped) {
	SwapTagData(reqp);
    }
    if (reqp == &req) {
	WriteToServer(client, sizeof(req), (char *) &req, TRUE);
	if (len)
	    WriteToServer(client, len, (char *) data, FALSE);
    } else {
	WriteReqToServer(client, req_len << 2, (char *) reqp);
	xfree(reqp);
    }
}

void
SendGetImage(client, drawable, x, y, width, height, planeMask, format)
    ClientPtr   client;
    Drawable    drawable;
    int         x;
    int         y;
    unsigned int width;
    unsigned int height;
    unsigned long planeMask;
    int         format;
{
    xLbxGetImageReq req;
    XServerPtr  server = client->server;

    if (client->server->serverClient == client)
	return;

    req.reqType = server->lbxReq;
    req.lbxReqType = X_LbxGetImage;
    req.length = 6;
    req.drawable = drawable;
    req.x = x;
    req.y = y;
    req.width = width;
    req.height = height;
    req.planeMask = planeMask;
    req.format = format;
    req.pad1 = 0;
    req.pad2 = 0;

    if (client->swapped) {
	SwapGetImage(&req);
    }
    WriteReqToServer(client, sizeof(req), (char *) &req);
}

static Bool
SendInternAtoms (client)
    ClientPtr client;
{
    xLbxInternAtomsReq *req;
    XServerPtr  server = client->server;
    int reqSize, i, num;
    char lenbuf[2];
    char *ptr;

    if (client->server->serverClient == client)
	return FALSE;

    reqSize = sz_xLbxInternAtomsReq;
    num = 0;
    for (i = 0; i < atom_control_count; i++) {
	if (atom_control[i].flags & AtomPreInternFlag) {
	    reqSize += (2 + atom_control[i].len);
	    num++;
	}
    }
    if (!num)
	return FALSE;

    if (!(req = (xLbxInternAtomsReq *) xalloc (reqSize)))
	return FALSE;

    req->reqType = server->lbxReq;
    req->lbxReqType = X_LbxInternAtoms;
    req->length = (reqSize + 3) >> 2;
    req->num = num;

    ptr = (char *) req + sz_xLbxInternAtomsReq;

    for (i = 0; i < atom_control_count; i++)
    {
	if (atom_control[i].flags & AtomPreInternFlag) {
	    *((CARD16 *) lenbuf) = atom_control[i].len;
	    ptr[0] = lenbuf[0];
	    ptr[1] = lenbuf[1];
	    ptr += 2;
	    memcpy (ptr, atom_control[i].name, atom_control[i].len);
	    ptr += atom_control[i].len;
	}
    }

    if (client->swapped)
	SwapInternAtoms (req);

    WriteToServer (client, reqSize, (char *) req, TRUE);

    xfree (req);
    return TRUE;
}

/*ARGSUSED*/
static void
InternAtomsReply (server, rep)
    XServerPtr  server;
    xLbxInternAtomsReply *rep;
{
    Atom *atoms = (Atom *) ((char *) rep + sz_xLbxInternAtomsReplyHdr);
    int i;

    for (i = 0; i < atom_control_count; i++) {
	if (atom_control[i].flags & AtomPreInternFlag)
	    (void) LbxMakeAtom (atom_control[i].name, atom_control[i].len,
				*atoms++, TRUE);
    }

    SendInitLBXPackets();

    /*
     * Now the proxy is ready to accept connections from clients.
     */

    (void) ListenWellKnownSockets ();
}


static unsigned long pendingServerReplySequence;
static void (*serverReplyFunc) ();

static void
ServerReply(server, rep)
    XServerPtr  server;
    xReply     *rep;
{
    if (serverReplyFunc &&
	    rep->generic.sequenceNumber == pendingServerReplySequence) {
	/*
	 * We got the reply we were waiting from the server
	 */

	(*serverReplyFunc) (server, rep);

	/*
	 * ExpectServerReply() might have been called within the server
	 * reply func just processed.
	 */

	if (rep->generic.sequenceNumber == pendingServerReplySequence)
	    serverReplyFunc = 0;
    }
}

static void
ExpectServerReply(server, func)
    XServerPtr  server;
    void        (*func) ();
{
    pendingServerReplySequence = server->serverClient->sequence;
    serverReplyFunc = func;
}

extern int  (*ServerVector[]) ();

static unsigned long
ServerRequestLength(req, sc, gotnow, partp)
    xReq       *req;
    ClientPtr   sc;
    int         gotnow;
    Bool       *partp;
{
    XServerPtr  server = servers[sc->lbxIndex];
    ClientPtr   client = server->recv;
    xReply     *rep;
    xConnSetupPrefix *pre;

    if (!req)
	req = (xReq *) sc->requestBuffer;
    if (gotnow < sizeof(xReq)) {
	*partp = TRUE;
	return sizeof(xReq);
    }
    if (req->reqType == server->lbxEvent && req->data == LbxDeltaEvent) {
	*partp = FALSE;
	return req->length << 2;
    }
    if (req->reqType == server->lbxEvent && req->data == LbxSwitchEvent) {
	*partp = FALSE;
	return sz_xLbxSwitchEvent;
    }
    if (req->reqType == server->lbxEvent + LbxQuickMotionDeltaEvent) {
	*partp = FALSE;
	return sz_lbxQuickMotionDeltaEvent;
    }
    if (req->reqType == server->lbxEvent && req->data == LbxMotionDeltaEvent) {
	*partp = FALSE;
	return sz_lbxMotionDeltaEvent;
    }
    if (client->awaitingSetup) {
	if (gotnow < 8) {
	    *partp = TRUE;
	    return 8;
	}
	pre = (xConnSetupPrefix *) req;
	*partp = FALSE;
	return 8 + (pre->length << 2);
    }
    if (gotnow < 8) {
	*partp = TRUE;
	return 8;
    }
    *partp = FALSE;
    rep = (xReply *) req;
    if (rep->generic.type != X_Reply) {
	return EventLength((xEvent *)rep);
    }
    return sz_xReply + (rep->generic.length << 2);
}

int
ServerProcStandardEvent(sc)
    ClientPtr   sc;
{
    xReply     *rep;
    XServerPtr  server = servers[sc->lbxIndex];
    ClientPtr   client = server->recv;
    int         len;
    Bool        part;
    Bool        cacheable = (server->initialized) ? TRUE : FALSE;

    rep = (xReply *) sc->requestBuffer;

    /* need to calculate length up from for Delta cache */
    len = RequestLength(rep, sc, 8, &part);

#ifdef LBX_STATS
    delta_in_total++;
#endif

    if (rep->generic.type == server->lbxEvent &&
	    rep->generic.data1 == LbxDeltaEvent) {
	xLbxDeltaReq *delta = (xLbxDeltaReq *) rep;

#ifdef LBX_STATS
	delta_in_attempts++;
	delta_in_hits++;
#endif

	/* Note that LBXDecodeDelta decodes and adds current msg to the cache */
	len = LBXDecodeDelta(&server->indeltas, ((char *) rep) + sz_xLbxDeltaReq,
			     delta->diffs, delta->cindex, &rep);

	/* Make local copy in case someone writes to the request buffer */
	memcpy(tempdeltabuf, (char *) rep, len);
	rep = (xReply *) tempdeltabuf;

	cacheable = FALSE;
    }

    /* stick in delta buffer before LBX code modified things */
    if (cacheable && DELTA_CACHEABLE(&server->indeltas, len)) {

#ifdef LBX_STATS
	delta_in_attempts++;
#endif

	LBXAddDeltaIn(&server->indeltas, (char *) rep, len);
    }
    if (rep->generic.type == server->lbxEvent &&
	rep->generic.data1 != LbxMotionDeltaEvent) {
	switch (rep->generic.data1) {
	case LbxSwitchEvent:
	    DBG(DBG_SWITCH, (stderr, "switch upstream to %d\n",
			     ((xLbxSwitchEvent *)rep)->client));
	    client = clients[((xLbxSwitchEvent *)rep)->client];
	    server->recv = client;
	    (void) CheckPendingClientInput(sc);
	    break;
	case LbxCloseEvent:
	    DBG(DBG_CLIENT, (stderr, "close client %d\n",
			     ((xLbxCloseEvent *)rep)->client));
	    client = clients[((xLbxCloseEvent *)rep)->client];
	    client->closeDownMode = DestroyAll;
	    CloseDownClient(client);
	    break;
	case LbxInvalidateTagEvent:
	    DBG(DBG_CLIENT, (stderr, "invalidate tag %d type %d\n",
			     ((xLbxInvalidateTagEvent *)rep)->tag,
			     ((xLbxInvalidateTagEvent *)rep)->tagType));
	    LbxFreeTag(((xLbxInvalidateTagEvent *)rep)->tag,
		       ((xLbxInvalidateTagEvent *)rep)->tagType);
	    break;
	case LbxSendTagDataEvent:
	    DBG(DBG_CLIENT, (stderr, "send tag data %d type %d\n",
			     ((xLbxSendTagDataEvent *)rep)->tag,
			     ((xLbxSendTagDataEvent *)rep)->tagType));
	    LbxSendTagData(((xLbxSendTagDataEvent *)rep)->tag,
			   ((xLbxSendTagDataEvent *)rep)->tagType);
	    break;
	case LbxListenToOne:
	    DBG(DBG_CLIENT, (stderr, "listen to one client %d\n",
			     ((xLbxListenToOne *)rep)->client));
	    if (((xLbxListenToOneEvent *)rep)->client == 0xffffffff)
		LbxOnlyListenToOneClient(server->serverClient);
	    else
		LbxOnlyListenToOneClient(clients[((xLbxListenToOneEvent *)rep)->client]);
	    break;
	case LbxListenToAll:
	    DBG(DBG_CLIENT, (stderr, "listen to all clients\n"));
	    LbxListenToAllClients();
	    break;
	case LbxReleaseCmapEvent:
	{
	    Colormap cmap = ((xLbxReleaseCmapEvent *)rep)->colormap;
	    ColormapPtr pmap;

	    pmap = (ColormapPtr) LookupIDByType (cmap, RT_COLORMAP);
#ifdef COLOR_DEBUG
	    fprintf (stderr, "\nGot LbxReleaseCmapEvent, cmap = 0x%x\n\n", cmap);
#endif
	    if (pmap && pmap->grab_status == CMAP_GRABBED)
		ReleaseCmap (client, pmap);
	    break;
	}
	case LbxFreeCellsEvent:
	{
	    Colormap cmap = ((xLbxFreeCellsEvent *)rep)->colormap;
	    Pixel start = ((xLbxFreeCellsEvent *)rep)->pixelStart;
	    Pixel end = ((xLbxFreeCellsEvent *)rep)->pixelEnd;
	    ColormapPtr pmap;

	    pmap = (ColormapPtr) LookupIDByType (cmap, RT_COLORMAP);
#ifdef COLOR_DEBUG
	    fprintf (stderr, "\nGot LbxFreeCellsEvent, cmap = 0x%x, ", cmap);
	    fprintf (stderr, "startPixel = %d, endPixel = %d\n\n",
		     start, end);
#endif
	    if (pmap && pmap->grab_status == CMAP_GRABBED)
		GotServerFreeCellsEvent (pmap, start, end, 1);
	    break;
	}
	}
    } else if ((rep->generic.type == server->lbxEvent &&
		rep->generic.data1 == LbxMotionDeltaEvent) ||
	       (rep->generic.type == server->lbxEvent + LbxQuickMotionDeltaEvent))
    {
	lbxMotionCache *motionCache = &server->motionCache;

	/*
	 * We use the motion delta event to generate a real MotionNotify event.
	 *
	 * The motion cache contains the last motion event we got from
	 * the server.
	 *
	 * The following are always stored in the cache in the proxy's
	 * byte order:
	 *     sequenceNumber, time, rootX, rootY, eventX, eventY
	 * This is because when constructing the MotionNotify event using
	 * the delta event, we must do arithmetic in the proxy's byte order.
	 *
	 * The following are stored in the byte order of the latest client
	 * receiving a motion event (indicated by motionCache->swapped):
	 *     root, event, child, state
	 * The assumption is that a client will receive a series of motion
	 * events, and we don't want to unnecessarily swap these fields.
	 * If the next motion event goes to a client with a byte order
	 * different from the previous client, we will have to swap these
	 * fields.
	 */

	AllowMotion(client, 1);

	if (rep->generic.type == server->lbxEvent)
	{
	    lbxMotionDeltaEvent *mev = (lbxMotionDeltaEvent *) rep;

	    motionCache->sequenceNumber += mev->deltaSequence;
	    motionCache->time += mev->deltaTime;
	    motionCache->rootX += mev->deltaX;
	    motionCache->rootY += mev->deltaY;
	    motionCache->eventX += mev->deltaX;
	    motionCache->eventY += mev->deltaY;
	}
	else
	{
	    lbxQuickMotionDeltaEvent *qmev = (lbxQuickMotionDeltaEvent *) rep;

	    motionCache->time += qmev->deltaTime;
	    motionCache->rootX += qmev->deltaX;
	    motionCache->rootY += qmev->deltaY;
	    motionCache->eventX += qmev->deltaX;
	    motionCache->eventY += qmev->deltaY;
	}

	if (!client->clientGone) {
	    xEvent ev;
	    int n;

	    if (motionCache->swapped != client->swapped)
	    {
		swapl (&motionCache->root, n);
		swapl (&motionCache->event, n);
		swapl (&motionCache->child, n);
		swaps (&motionCache->state, n);
		motionCache->swapped = client->swapped;
	    }

	    ev.u.u.type = MotionNotify;
	    ev.u.u.detail = motionCache->detail;
	    ev.u.u.sequenceNumber = motionCache->sequenceNumber;
	    ev.u.keyButtonPointer.time = motionCache->time;
	    ev.u.keyButtonPointer.rootX = motionCache->rootX;
	    ev.u.keyButtonPointer.rootY = motionCache->rootY;
	    ev.u.keyButtonPointer.eventX = motionCache->eventX;
	    ev.u.keyButtonPointer.eventY = motionCache->eventY;
	    ev.u.keyButtonPointer.root = motionCache->root;
	    ev.u.keyButtonPointer.event = motionCache->event;
	    ev.u.keyButtonPointer.child = motionCache->child;
	    ev.u.keyButtonPointer.state = motionCache->state;
	    ev.u.keyButtonPointer.sameScreen = motionCache->sameScreen;

	    if (client->swapped) {
		swaps (&ev.u.u.sequenceNumber, n);
		swapl (&ev.u.keyButtonPointer.time, n);
		swaps (&ev.u.keyButtonPointer.rootX, n);
		swaps (&ev.u.keyButtonPointer.rootY, n);
		swaps (&ev.u.keyButtonPointer.eventX, n);
		swaps (&ev.u.keyButtonPointer.eventY, n);
	    }
	    /*
	     * Write the reply
	     */

	    DoLBXReply (client, (char *) &ev, sz_xEvent);
	}

    } else {
	len = RequestLength(rep, sc, 8, &part);
	DBG(DBG_IO, (stderr, "upstream %d len %d\n", client->index, len));
	if (client->index == 0) {
	    ServerReply(server, rep);
	} else {
	    xEvent      ev;
	    char       *rp;
	    int		n;

	    if (!client->awaitingSetup && UnsquishEvent(rep, &ev, &len))
		rp = (char *) &ev;
	    else
		rp = (char *) rep;

	    if (rep->generic.type == MotionNotify) {
		xEvent *mev = (xEvent *) rp;
		lbxMotionCache *motionCache = &server->motionCache;

		AllowMotion(client, 1);
		motionCache->swapped = client->swapped;
		motionCache->detail = mev->u.u.detail;
		motionCache->root = mev->u.keyButtonPointer.root;
		motionCache->event = mev->u.keyButtonPointer.event;
		motionCache->child = mev->u.keyButtonPointer.child;
		motionCache->state = mev->u.keyButtonPointer.state;
		motionCache->sameScreen = mev->u.keyButtonPointer.sameScreen;
		motionCache->sequenceNumber = mev->u.u.sequenceNumber;
		motionCache->time = mev->u.keyButtonPointer.time;
		motionCache->rootX = mev->u.keyButtonPointer.rootX;
		motionCache->rootY = mev->u.keyButtonPointer.rootY;
		motionCache->eventX = mev->u.keyButtonPointer.eventX;
		motionCache->eventY = mev->u.keyButtonPointer.eventY;
		if (client->swapped)
		{
		    swaps(&motionCache->sequenceNumber, n);
		    swapl(&motionCache->time, n);
		    swaps(&motionCache->rootX, n);
		    swaps(&motionCache->rootY, n);
		    swaps(&motionCache->eventX, n);
		    swaps(&motionCache->eventY, n);
		}
	    }

	    /*
	     * Write the reply
	     */

	    if (!client->clientGone)
		DoLBXReply (client, rp, len);
	    client->awaitingSetup = FALSE;
	}
    }

    return Success;
}

extern int  GrabInProgress;
static int  lbxIgnoringAll;
static int  lbxGrabInProgress;

static void
LbxIgnoreAllClients(server)
    XServerPtr  server;
{
    if (!lbxIgnoringAll) {
	if (GrabInProgress) {
	    lbxGrabInProgress = GrabInProgress;
	    ListenToAllClients();
	}
	OnlyListenToOneClient(server->serverClient);
	lbxIgnoringAll = TRUE;
    }
}

/* ARGSUSED */
static void
LbxAttendAllClients(server)
    XServerPtr  server;
{
    if (lbxIgnoringAll) {
	ListenToAllClients();
	lbxIgnoringAll = FALSE;
	if (lbxGrabInProgress) {
	    OnlyListenToOneClient(clients[lbxGrabInProgress]);
	    lbxGrabInProgress = 0;
	}
    }
}

static void
LbxOnlyListenToOneClient(client)
    ClientPtr   client;
{
    if (lbxIgnoringAll)
	lbxGrabInProgress = client->index;
    else {
	if (GrabInProgress)
	    ListenToAllClients();
	OnlyListenToOneClient(client);
    }
}

static void
LbxListenToAllClients()
{
    if (lbxGrabInProgress)
	lbxGrabInProgress = 0;
    else if (!lbxIgnoringAll)
	ListenToAllClients();
}

/* ARGSUSED */
static Bool
ProxyWorkProc(dummy, index)
    pointer     dummy;
    int         index;
{
    XServerPtr  server;
    xLbxAllowMotionReq req;

    if ((server = servers[index]) == 0)
	return TRUE;
    if (!server->initialized)
	return TRUE;

    if (server->motion_allowed) {
	DBG(DBG_CLIENT, (stderr, "allow %d motion events\n",
			 server->motion_allowed));
	req.reqType = server->lbxReq;
	req.lbxReqType = X_LbxAllowMotion;
	req.length = 2;
	req.num = server->motion_allowed;
	server->motion_allowed = 0;
	WriteToClient(server->serverClient, sizeof(req), &req);
    }
    /* Need to flush the output buffers before we flush compression buffer */
    if (NewOutputPending)
	FlushAllOutput();

    if (server->compHandle) {
	if (lbxNegOpt.streamOpts.streamCompInputAvail(server->fd))
	    AvailableClientInput(server->serverClient);
	if (lbxNegOpt.streamOpts.streamCompFlush(server->fd) != 0)
	    MarkConnectionWriteBlocked(server->serverClient);
    }
    /*
     * If we've got stuff remaining in the output buffers to the server, then
     * don't allow reads from any other clients, otherwise we could overflow.
     */
    if (PendingClientOutput(server->serverClient))
	LbxIgnoreAllClients(server);
    else
	LbxAttendAllClients(server);

    return FALSE;
}

Bool reconnectAfterCloseServer = FALSE;

void
CloseServer(client)
    ClientPtr   client;
{
    XServerPtr  server;
    int         i;
    extern int	proxyMngr;

    server = client->server;
    servers[server->index] = 0;
    LBXFreeDeltaCache(&server->indeltas);
    LBXFreeDeltaCache(&server->outdeltas);
    if (server->compHandle)
	lbxNegOpt.streamOpts.streamCompFreeHandle(server->compHandle);
    /* remove all back pointers */
    for (i = 1; i < currentMaxClients; i++) {
	if (clients[i] && clients[i]->server == server)
	    clients[i]->server = NULL;
    }
    xfree(server);
    CloseDownFileDescriptor(client);
    DBG(DBG_CLOSE, (stderr, "closing down server\n"));
    if (reconnectAfterCloseServer)
	dispatchException |= DE_RESET;
    else
	dispatchException |= DE_TERMINATE;
    isItTimeToYield = 1;
}


static void
StartProxyReply(server, rep)
    XServerPtr  server;
    xLbxStartReply *rep;
{
    int         replylen;

    replylen = (rep->length << 2) + sz_xLbxStartReply - sz_xLbxStartReplyHdr;
    if (rep->nOpts == 0xff) {
	fprintf(stderr, "WARNING: option negotiation failed - using defaults\n");
	LbxOptInit();
    } else if (LbxOptParseReply(rep->nOpts,
			(unsigned char *)&rep->optDataStart, replylen) < 0) {
	FatalError("Bad options from server");
    }

#ifdef OPTDEBUG
    fprintf(stderr, "server: N = %d, maxlen = %d, proxy: N = %d, maxlen = %d\n",
	    lbxNegOpt.serverDeltaN, lbxNegOpt.serverDeltaMaxLen,
	    lbxNegOpt.proxyDeltaN, lbxNegOpt.proxyDeltaMaxLen);
#endif

    LBXInitDeltaCache(&server->indeltas, lbxNegOpt.serverDeltaN,
		      lbxNegOpt.serverDeltaMaxLen);
    LBXInitDeltaCache(&server->outdeltas, lbxNegOpt.proxyDeltaN,
		      lbxNegOpt.proxyDeltaMaxLen);
    QueueWorkProc(ProxyWorkProc, NULL, (pointer) server->index);

#ifdef OPTDEBUG
    fprintf(stderr, "squishing = %d\n", lbxNegOpt.squish);
    fprintf(stderr, "useTags = %d\n", lbxNegOpt.useTags);
#endif

    TagsInit(lbxNegOpt.useTags);

    if (!lbxNegOpt.useTags)
    {
	ProcVector[X_GetModifierMapping] = ProcStandardRequest;
	ProcVector[X_GetKeyboardMapping] = ProcStandardRequest;
	ProcVector[X_QueryFont] = ProcStandardRequest;
	ProcVector[X_ChangeProperty] = ProcStandardRequest;
	ProcVector[X_GetProperty] = ProcStandardRequest;
    }

    if (lbxNegOpt.streamOpts.streamCompInit) {
	unsigned char   *extra = (unsigned char *) rep;
	int		len = sizeof(xReply) + (rep->length << 2);
	int		left = BytesInClientBuffer(server->serverClient);

	server->compHandle =
	    (*lbxNegOpt.streamOpts.streamCompInit) (
		server->fd, lbxNegOpt.streamOpts.streamCompArg);
	SwitchConnectionFuncs(server->serverClient,
	    lbxNegOpt.streamOpts.streamCompRead,
	    lbxNegOpt.streamOpts.streamCompWriteV);
	extra += len;
	lbxNegOpt.streamOpts.streamCompStuffInput(server->fd, extra, left);
	SkipInClientBuffer(server->serverClient, left + len, 0);
	StartOutputCompression(server->serverClient,
	    lbxNegOpt.streamOpts.streamCompOn,
	    lbxNegOpt.streamOpts.streamCompOff);
    }
    server->initialized = TRUE;
    MakeClientGrabImpervious(server->serverClient);

    if (SendInternAtoms(clients[0]))
	ExpectServerReply (server, InternAtomsReply);
    else
    {
	SendInitLBXPackets();

	/*
	 * Now the proxy is ready to accept connections from clients.
	 */

	(void) ListenWellKnownSockets ();
    }
}

static void
StartProxy(server)
    XServerPtr  server;
{
    char        buf[1024];
    int         reqlen;
    xLbxStartProxyReq *n = (xLbxStartProxyReq *) buf;

    LbxOptInit();
    n->reqType = server->lbxReq;
    n->lbxReqType = X_LbxStartProxy;
    reqlen = LbxOptBuildReq(buf + sz_xLbxStartProxyReq);
    assert(reqlen > 0 && reqlen + sz_xLbxStartProxyReq <= 1024);
    n->length = (reqlen + sz_xLbxStartProxyReq + 3) >> 2;
    WriteToServer(clients[0], n->length << 2, (char *) n, TRUE);
    server->serverClient->sequence++;
    ExpectServerReply(server, StartProxyReply);
    while (NewOutputPending)
	FlushAllOutput();
}

static Bool
InitServer (dpy_name, i, server, sequencep)
    char*	dpy_name;
    int		i;
    XServerPtr	server;
    int*	sequencep;
{
    server->index = i;
    server->dpy = DisplayOpen(dpy_name, &server->lbxReq, &server->lbxEvent, &server->lbxError, sequencep);
    if (!server->dpy) return FALSE;
    server->fd = DisplayConnectionNumber (server->dpy);
    DBG(DBG_IO, (stderr, "making server connection\n"));
    server->compHandle = NULL;
    server->initialized = FALSE;
    server->prev_exec = clients[0];
    server->send = clients[0];
    server->recv = clients[0];
    clients[0]->server = server;
    server->motion_allowed = 0;
    server->wm_running = FALSE;
    server->extensions = NULL;
    return TRUE;
}

Bool
ConnectToServer(dpy_name)
    char       *dpy_name;
{
    int         i;
    XServerPtr  server;
    int         sequence;
    ClientPtr   sc;
    extern int	proxyMngr;
    extern IceConn PM_iceConn;
/*  extern int	gotFirstGetProxyAddr; */
    extern void SendGetProxyAddrReply();
#if defined(NCR) || (defined(sun) && !defined(SVR4)) || defined(__ultrix__)
    extern int writev(int, struct iovec*, int);
#endif

    /* assert( !proxyMngr || gotFirstGetProxyAddr ); */
    /* Proxy Manager (if present) is awaiting a reply... */

    for (i = 0; i < MAX_SERVERS; i++)
	if (!servers[i])
	    break;
    if (i == MAX_SERVERS) {
	if (proxyMngr)
	    SendGetProxyAddrReply( PM_iceConn, PM_Unable, NULL,
				   "too many servers" );
	return FALSE;
    }
    server = (XServerPtr) xalloc(sizeof(XServerRec));
    if (!server) {
	if (proxyMngr)
	    SendGetProxyAddrReply( PM_iceConn, PM_Unable, NULL,
				   "memory allocation failure");
	return FALSE;
    }
    bzero(server, sizeof(XServerRec));
    if (!InitServer (dpy_name, i, server, &sequence)) {
	if (proxyMngr)
	    SendGetProxyAddrReply( PM_iceConn, PM_Failure, NULL,
				   "unable to connect to server" );
	xfree(server);
	return FALSE;
    }

    sc = AllocNewConnection(server->fd, TRUE);
    sc->server = server;
    sc->public.requestLength = ServerRequestLength;
    sc->lbxIndex = i;
    sc->requestVector = ServerVector;
    sc->awaitingSetup = FALSE;
    sc->sequence = sequence;

    server->serverClient = sc;
    servers[i] = server;
    StartProxy(server);

    if (proxyMngr) {
	extern char proxyAddress[];
	SendGetProxyAddrReply( PM_iceConn, PM_Success, proxyAddress, NULL );
    }

    return TRUE;
}

/* $XConsortium: streams.c,v 1.5 94/06/03 16:34:41 mor Exp $ */
/* $XFree86: xc/programs/xdm/streams.c,v 3.1 1994/06/09 10:56:15 dawes Exp $ */
/*

Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
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

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 *
 * streams.c - Support for STREAMS
 */

#include "dm.h"

#ifdef XDMCP
#ifdef STREAMSCONN

#include <fcntl.h>
#include <tiuser.h>
#include <netconfig.h>
#include <netdir.h>

extern int	xdmcpFd;
extern int	chooserFd;

extern FD_TYPE	WellKnownSocketsMask;
extern int	WellKnownSocketsMax;

CreateWellKnownSockets ()
{
    struct t_bind bind_addr;
    struct netconfig *nconf;
    struct nd_hostserv service;
    struct nd_addrlist *servaddrs;
    char *name, *localHostname();
    char bindbuf[15];
    int it;

    if (request_port == 0)
	return;
    Debug ("creating UDP stream %d\n", request_port);

    nconf = getnetconfigent("udp");
    if (!nconf) {
	t_error("getnetconfigent udp");
	return;
    }

    xdmcpFd = t_open(nconf->nc_device, O_RDWR, NULL);
    if (xdmcpFd == -1) {
	LogError ("XDMCP stream creation failed\n");
	t_error ("CreateWellKnownSockets(xdmcpFd): t_open failed");
	return;
    }
    name = localHostname ();
    registerHostname (name, strlen (name));
    RegisterCloseOnFork (xdmcpFd);

    service.h_host = HOST_SELF;
    sprintf(bindbuf, "%d", request_port);
    service.h_serv = bindbuf;
    netdir_getbyname(nconf, &service, &servaddrs);
    freenetconfigent(nconf);

    bind_addr.qlen = 5;
    bind_addr.addr.buf = servaddrs->n_addrs[0].buf;
    bind_addr.addr.len = servaddrs->n_addrs[0].len;
    bind_addr.addr.maxlen = servaddrs->n_addrs[0].len;
    it = t_bind(xdmcpFd, &bind_addr, &bind_addr);
    netdir_free((char *)servaddrs, ND_ADDRLIST);
    if (it < 0)
    {
	LogError ("error binding STREAMS address %d\n", request_port);
	t_error("CreateWellKNowSocket(xdmcpFd): t_bind failed");
	t_close (xdmcpFd);
	xdmcpFd = -1;
	return;
    }
    WellKnownSocketsMax = xdmcpFd;
    FD_SET (xdmcpFd, &WellKnownSocketsMask);

    chooserFd = t_open ("/dev/tcp", O_RDWR, NULL);
    Debug ("Created chooser fd %d\n", chooserFd);
    if (chooserFd == -1)
    {
	LogError ("chooser stream creation failed\n");
	t_error("CreateWellKnowSockets(chooserFd): t_open failed");
	return;
    }
    bind_addr.qlen = 5;
    bind_addr.addr.len = 0;
    bind_addr.addr.maxlen = 0;
    if( t_bind( chooserFd, &bind_addr, NULL ) < 0 )
    {
        t_error("CreateWellKnowSockets(chooserFd): t_bind failed");
    }

    if (chooserFd > WellKnownSocketsMax)
	WellKnownSocketsMax = chooserFd;
    FD_SET (chooserFd, &WellKnownSocketsMask);
}

GetChooserAddr (addr, lenp)
    char	*addr;		/* return */
    int		*lenp;		/* size of addr, returned as amt used */
{
    struct netbuf nbuf;
    int retval;

    nbuf.buf = addr;
    nbuf.len = *lenp;
    nbuf.maxlen = *lenp;
    retval = t_getname (chooserFd, &nbuf, LOCALNAME);
    if (retval < 0) {
	if (debugLevel > 0)
	    t_error("t_getname on chooser fd");
    }
    *lenp = nbuf.len;
    return retval;
}

#endif /* STREAMSCONN */
#endif /* XDMCP */

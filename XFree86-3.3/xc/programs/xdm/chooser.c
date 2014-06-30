/*
 * $XConsortium: chooser.c /main/27 1996/01/25 18:45:41 kaleb $
 * $XFree86: xc/programs/xdm/chooser.c,v 3.13 1997/01/18 07:02:20 dawes Exp $
 *
Copyright (c) 1990  X Consortium

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
 *
 * Author:  Keith Packard, MIT X Consortium
 */

/*
 * Chooser - display a menu of names and let the user select one
 */

/*
 * Layout:
 *
 *  +--------------------------------------------------+
 *  |             +------------------+                 |
 *  |             |      Label       |                 |
 *  |             +------------------+                 |
 *  |    +-+--------------+                            |
 *  |    |^| name-1       |                            |
 *  |    ||| name-2       |                            |
 *  |    |v| name-3       |                            |
 *  |    | | name-4       |                            |
 *  |    | | name-5       |                            |
 *  |    | | name-6       |                            |
 *  |    +----------------+                            |
 *  |    cancel  accept  ping                          |
 *  +--------------------------------------------------+
 */

#include    <X11/Intrinsic.h>
#include    <X11/StringDefs.h>
#include    <X11/Xatom.h>

#include    <X11/Xaw/Paned.h>
#include    <X11/Xaw/Label.h>
#include    <X11/Xaw/Viewport.h>
#include    <X11/Xaw/List.h>
#include    <X11/Xaw/Box.h>
#include    <X11/Xaw/Command.h>

#include    "dm.h"

#include    <X11/Xdmcp.h>

#include    <sys/types.h>
#include    <stdio.h>
#include    <ctype.h>

#if defined(SVR4) && !defined(SCO325)
#include    <sys/sockio.h>
#endif
#if defined(SYSV) && defined(i386)
#include    <sys/stream.h>
#ifdef ISC
#include    <sys/sioctl.h>
#include    <sys/stropts.h>
#endif
#endif
#ifndef MINIX
#ifndef Lynx
#include    <sys/socket.h>
#else
#include    <socket.h>
#endif
#include    <netinet/in.h>
#else /* MINIX */
#include <net/hton.h>
#include <net/netlib.h>
#include <net/gen/in.h>
#include <net/gen/netdb.h>
#include <net/gen/tcp.h>
#include <net/gen/tcp_io.h>
#include <net/gen/udp.h>
#include <net/gen/udp_io.h>
#include <sys/nbio.h>
#endif /* !MINIX */
#include    <sys/ioctl.h>
#ifdef STREAMSCONN
#ifdef WINTCP /* NCR with Wollongong TCP */
#include    <netinet/ip.h>
#endif
#include    <stropts.h>
#include    <tiuser.h>
#include    <netconfig.h>
#include    <netdir.h>
#endif

#ifdef CSRG_BASED
#include <sys/param.h>
#if (BSD >= 199103)
#define VARIABLE_IFREQ
#endif
#endif

#ifdef XKB
#include <X11/extensions/XKBbells.h>
#endif

#define BROADCAST_HOSTNAME  "BROADCAST"

#ifndef ishexdigit
#define ishexdigit(c)	(isdigit(c) || 'a' <= (c) && (c) <= 'f')
#endif

#ifdef hpux
# include <sys/utsname.h>
# ifdef HAS_IFREQ
#  include <net/if.h>
# endif
#else
#ifdef __convex__
# include <sync/queue.h>
# include <sync/sema.h>
#endif
#ifndef MINIX
# include <net/if.h>
#endif
#endif /* hpux */

#ifndef MINIX
#include    <netdb.h>
#endif

#ifdef MINIX
static char read_buffer[XDM_MAX_MSGLEN+sizeof(udp_io_hdr_t)];
static int read_inprogress;
static int read_size;
static void read_cb(nbio_ref_t ref, int res, int err);
#endif

Widget	    toplevel, label, viewport, paned, list, box, cancel, acceptit, ping;

static void	CvtStringToARRAY8();

static struct _app_resources {
    ARRAY8Ptr   xdmAddress;
    ARRAY8Ptr	clientAddress;
    int		connectionType;
} app_resources;

#define offset(field) XtOffsetOf(struct _app_resources, field)

#define XtRARRAY8   "ARRAY8"

static XtResource  resources[] = {
    {"xdmAddress",	"XdmAddress",  XtRARRAY8,	sizeof (ARRAY8Ptr),
	offset (xdmAddress),	    XtRString,	NULL },
    {"clientAddress",	"ClientAddress",  XtRARRAY8,	sizeof (ARRAY8Ptr),
	offset (clientAddress),	    XtRString,	NULL },
    {"connectionType",	"ConnectionType",   XtRInt,	sizeof (int),
	offset (connectionType),    XtRImmediate,	(XtPointer) 0 }
};
#undef offset

static XrmOptionDescRec options[] = {
    "-xdmaddress",	"*xdmAddress",	    XrmoptionSepArg,	NULL,
    "-clientaddress",	"*clientAddress",   XrmoptionSepArg,	NULL,
    "-connectionType",	"*connectionType",  XrmoptionSepArg,	NULL,
};

typedef struct _hostAddr {
    struct _hostAddr	*next;
    struct sockaddr	*addr;
    int			addrlen;
    xdmOpCode		type;
} HostAddr;

static HostAddr    *hostAddrdb;

typedef struct _hostName {
    struct _hostName	*next;
    char		*fullname;
    int			willing;
    ARRAY8		hostname, status;
    CARD16		connectionType;
    ARRAY8		hostaddr;
} HostName;

static HostName    *hostNamedb;

static int  socketFD;

static int  pingTry;

#define PING_INTERVAL	2000
#define TRIES		3

static XdmcpBuffer	directBuffer, broadcastBuffer;
static XdmcpBuffer	buffer;

#if ((defined(SVR4) && !defined(sun) && !defined(NCR)) || defined(ISC)) && defined(SIOCGIFCONF)

/* Deal with different SIOCGIFCONF ioctl semantics on these OSs */

static int
ifioctl (fd, cmd, arg)
    int fd;
    int cmd;
    char *arg;
{
    struct strioctl ioc;
    int ret;

    bzero((char *) &ioc, sizeof(ioc));
    ioc.ic_cmd = cmd;
    ioc.ic_timout = 0;
    if (cmd == SIOCGIFCONF)
    {
	ioc.ic_len = ((struct ifconf *) arg)->ifc_len;
	ioc.ic_dp = ((struct ifconf *) arg)->ifc_buf;
#ifdef ISC
	/* SIOCGIFCONF is somewhat brain damaged on ISC. The argument
	 * buffer must contain the ifconf structure as header. Ifc_req
	 * is also not a pointer but a one element array of ifreq
	 * structures. On return this array is extended by enough
	 * ifreq fields to hold all interfaces. The return buffer length
	 * is placed in the buffer header.
	 */
        ((struct ifconf *) ioc.ic_dp)->ifc_len =
                                         ioc.ic_len - sizeof(struct ifconf);
#endif
    }
    else
    {
	ioc.ic_len = sizeof(struct ifreq);
	ioc.ic_dp = arg;
    }
    ret = ioctl(fd, I_STR, (char *) &ioc);
    if (ret >= 0 && cmd == SIOCGIFCONF)
#ifdef SVR4
	((struct ifconf *) arg)->ifc_len = ioc.ic_len;
#endif
#ifdef ISC
    {
	((struct ifconf *) arg)->ifc_len =
				 ((struct ifconf *)ioc.ic_dp)->ifc_len;
	((struct ifconf *) arg)->ifc_buf = 
			(caddr_t)((struct ifconf *)ioc.ic_dp)->ifc_req;
    }
#endif
    return(ret);
}
#else /* ((SVR4 && !sun && !NCR) || ISC) && SIOCGIFCONF */
#define ifioctl ioctl
#endif /* ((SVR4 && !sun) || ISC) && SIOCGIFCONF */


/* ARGSUSED */
static void
PingHosts (closure, id)
    XtPointer closure;
    XtIntervalId *id;
{
    HostAddr	*hosts;

    for (hosts = hostAddrdb; hosts; hosts = hosts->next)
    {
	if (hosts->type == QUERY)
	    XdmcpFlush (socketFD, &directBuffer, hosts->addr, hosts->addrlen);
	else
	    XdmcpFlush (socketFD, &broadcastBuffer, hosts->addr, hosts->addrlen);
    }
    if (++pingTry < TRIES)
	XtAddTimeOut (PING_INTERVAL, PingHosts, (XtPointer) 0);
}

char	**NameTable;
int	NameTableSize;

static int
HostnameCompare (a, b)
#ifdef __STDC__
    const void *a, *b;
#else
    char *a, *b;
#endif
{
    return strcmp (*(char **)a, *(char **)b);
}

static void
RebuildTable (size)
    int size;
{
    char	**newTable = 0;
    HostName	*names;
    int		i;

    if (size)
    {
	newTable = (char **) malloc (size * sizeof (char *));
	if (!newTable)
	    return;
	for (names = hostNamedb, i = 0; names; names = names->next, i++)
	    newTable[i] = names->fullname;
	qsort (newTable, size, sizeof (char *), HostnameCompare);
    }
    XawListChange (list, newTable, size, 0, TRUE);
    if (NameTable)
	free ((char *) NameTable);
    NameTable = newTable;
    NameTableSize = size;
}

static int
AddHostname (hostname, status, addr, willing)
    ARRAY8Ptr	    hostname, status;
    struct sockaddr *addr;
    int		    willing;
{
    HostName	*new, **names, *name;
    ARRAY8	hostAddr;
    CARD16	connectionType;
    int		fulllen;

    switch (addr->sa_family)
    {
    case AF_INET:
	hostAddr.data = (CARD8 *) &((struct sockaddr_in *) addr)->sin_addr;
	hostAddr.length = 4;
	connectionType = FamilyInternet;
	break;
    default:
	hostAddr.data = (CARD8 *) "";
	hostAddr.length = 0;
	connectionType = FamilyLocal;
	break;
    }
    for (names = &hostNamedb; *names; names = & (*names)->next)
    {
	name = *names;
	if (connectionType == name->connectionType &&
	    XdmcpARRAY8Equal (&hostAddr, &name->hostaddr))
	{
	    if (XdmcpARRAY8Equal (status, &name->status))
	    {
		return 0;
	    }
	    break;
	}
    }
    if (!*names)
    {
	new = (HostName *) malloc (sizeof (HostName));
    	if (!new)
	    return 0;
	if (hostname->length)
	{
	    switch (addr->sa_family)
	    {
	    case AF_INET:
	    	{
	    	    struct hostent  *hostent;
		    char	    *host;
    	
	    	    hostent = gethostbyaddr ((char *)hostAddr.data, hostAddr.length, AF_INET);
	    	    if (hostent)
	    	    {
			XdmcpDisposeARRAY8 (hostname);
		    	host = hostent->h_name;
			XdmcpAllocARRAY8 (hostname, strlen (host));
			memmove( hostname->data, host, hostname->length);
	    	    }
	    	}
	    }
	}
    	if (!XdmcpAllocARRAY8 (&new->hostaddr, hostAddr.length))
    	{
	    free ((char *) new->fullname);
	    free ((char *) new);
	    return 0;
    	}
    	memmove( new->hostaddr.data, hostAddr.data, hostAddr.length);
	new->connectionType = connectionType;
	new->hostname = *hostname;

    	*names = new;
    	new->next = 0;
	NameTableSize++;
    }
    else
    {
	new = *names;
	free (new->fullname);
	XdmcpDisposeARRAY8 (&new->status);
	XdmcpDisposeARRAY8 (hostname);
    }
    new->willing = willing;
    new->status = *status;

    hostname = &new->hostname;
    fulllen = hostname->length;
    if (fulllen < 30)
	fulllen = 30;
    new->fullname = malloc (fulllen + status->length + 10);
    if (!new->fullname)
    {
	new->fullname = "Unknown";
    }
    else
    {
	sprintf (new->fullname, "%-30.*s %*.*s",
		 hostname->length, hostname->data,
		 status->length, status->length, status->data);
    }
    RebuildTable (NameTableSize);
    return 1;
}

static void
DisposeHostname (host)
    HostName	*host;
{
    XdmcpDisposeARRAY8 (&host->hostname);
    XdmcpDisposeARRAY8 (&host->hostaddr);
    XdmcpDisposeARRAY8 (&host->status);
    free ((char *) host->fullname);
    free ((char *) host);
}

static void
RemoveHostname (host)
    HostName	*host;
{
    HostName	**prev, *hosts;

    prev = &hostNamedb;;
    for (hosts = hostNamedb; hosts; hosts = hosts->next)
    {
	if (hosts == host)
	    break;
	prev = &hosts->next;
    }
    if (!hosts)
	return;
    *prev = host->next;
    DisposeHostname (host);
    NameTableSize--;
    RebuildTable (NameTableSize);
}

static void
EmptyHostnames ()
{
    HostName	*hosts, *next;

    for (hosts = hostNamedb; hosts; hosts = next)
    {
	next = hosts->next;
	DisposeHostname (hosts);
    }
    NameTableSize = 0;
    hostNamedb = 0;
    RebuildTable (NameTableSize);
}

/* ARGSUSED */
static void
ReceivePacket (closure, source, id)
    XtPointer	closure;
    int		*source;
    XtInputId	*id;
{
    XdmcpHeader	    header;
    ARRAY8	    authenticationName;
    ARRAY8	    hostname;
    ARRAY8	    status;
    int		    saveHostname = 0;
    struct sockaddr addr;
    int		    addrlen;
#ifdef MINIX
    int r;
#endif

#ifdef MINIX
    if (read_inprogress) abort();
    if (read_size == 0)
    {
    	r= read(socketFD, read_buffer, sizeof(read_buffer));
    	if (r == -1 && errno == EINPROGRESS)
    	{
    		read_inprogress= 1;
    		nbio_inprogress(socketFD, ASIO_READ, 1 /* read */,
    			0 /* write */, 0 /* exception */);
    	}
    	else if (r <= 0)
    	{
    		fprintf(stderr, "chooser: read error: %s\n", r == 0 ?
    			"EOF" : strerror(errno));
		return;
	}
    }
#endif

    addrlen = sizeof (addr);
#ifdef MINIX
    if (!MNX_XdmcpFill (socketFD, &buffer, &addr, &addrlen,
    	read_buffer, read_size))
    {
	return;
    }
    read_size= 0;
#else
    if (!XdmcpFill (socketFD, &buffer, &addr, &addrlen))
	return;
#endif
    if (!XdmcpReadHeader (&buffer, &header))
	return;
    if (header.version != XDM_PROTOCOL_VERSION)
	return;
    hostname.data = 0;
    status.data = 0;
    authenticationName.data = 0;
    switch (header.opcode) {
    case WILLING:
    	if (XdmcpReadARRAY8 (&buffer, &authenticationName) &&
	    XdmcpReadARRAY8 (&buffer, &hostname) &&
	    XdmcpReadARRAY8 (&buffer, &status))
    	{
	    if (header.length == 6 + authenticationName.length +
	    	hostname.length + status.length)
	    {
		if (AddHostname (&hostname, &status, &addr, header.opcode == (int) WILLING))
		    saveHostname = 1;
	    }
    	}
	XdmcpDisposeARRAY8 (&authenticationName);
	break;
    case UNWILLING:
    	if (XdmcpReadARRAY8 (&buffer, &hostname) &&
	    XdmcpReadARRAY8 (&buffer, &status))
    	{
	    if (header.length == 4 + hostname.length + status.length)
	    {
		if (AddHostname (&hostname, &status, &addr, header.opcode == (int) WILLING))
		    saveHostname = 1;

	    }
    	}
	break;
    default:
	break;
    }
    if (!saveHostname)
    {
    	XdmcpDisposeARRAY8 (&hostname);
    	XdmcpDisposeARRAY8 (&status);
    }
}

static void
RegisterHostaddr (addr, len, type)
    struct sockaddr *addr;
    int		    len;
    xdmOpCode	    type;
{
    HostAddr		*host, **prev;

    host = (HostAddr *) malloc (sizeof (HostAddr));
    if (!host)
	return;
    host->addr = (struct sockaddr *) malloc (len);
    if (!host->addr)
    {
	free ((char *) host);
	return;
    }
    memmove( (char *) host->addr, (char *) addr, len);
    host->addrlen = len;
    host->type = type;
    for (prev = &hostAddrdb; *prev; prev = &(*prev)->next)
	;
    *prev = host;
    host->next = NULL;
}

/*
 * Register the address for this host.
 * Called with each of the names on the command line.
 * The special name "BROADCAST" looks up all the broadcast
 *  addresses on the local host.
 */

#ifndef MINIX

/* Handle variable length ifreq in BNR2 and later */
#ifdef VARIABLE_IFREQ
#define ifr_size(p) (sizeof (struct ifreq) + \
		     (p->ifr_addr.sa_len > sizeof (p->ifr_addr) ? \
		      p->ifr_addr.sa_len - sizeof (p->ifr_addr) : 0))
#else
#define ifr_size(p) (sizeof (struct ifreq))
#endif

static void
RegisterHostname (name)
    char    *name;
{
    struct hostent	*hostent;
    struct sockaddr_in	in_addr;
    struct ifconf	ifc;
    register struct ifreq *ifr;
    struct sockaddr	broad_addr;
    char		buf[2048], *cp, *cplim;
    int			n;

    if (!strcmp (name, BROADCAST_HOSTNAME))
    {
#ifdef WINTCP /* NCR with Wollongong TCP */
    int                 ipfd;
    struct ifconf       *ifcp;
    struct strioctl     ioc;

	ifcp = (struct ifconf *)buf;
	ifcp->ifc_buf = buf+4;
	ifcp->ifc_len = sizeof (buf) - 4;

	if ((ipfd=open( "/dev/ip", O_RDONLY )) < 0 )
	    {
	    t_error( "RegisterHostname() t_open(/dev/ip) failed" );
	    return;
	    }

	ioc.ic_cmd = IPIOC_GETIFCONF;
	ioc.ic_timout = 60;
	ioc.ic_len = sizeof( buf );
	ioc.ic_dp = (char *)ifcp;

	if (ioctl (ipfd, (int) I_STR, (char *) &ioc) < 0)
	    {
	    perror( "RegisterHostname() ioctl(I_STR(IPIOC_GETIFCONF)) failed" );
	    close( ipfd );
	    return;
	    }

	for (ifr = ifcp->ifc_req, n = ifcp->ifc_len / sizeof (struct ifreq);
	    --n >= 0;
	    ifr++)
#else /* WINTCP */
	ifc.ifc_len = sizeof (buf);
	ifc.ifc_buf = buf;
	if (ifioctl (socketFD, (int) SIOCGIFCONF, (char *) &ifc) < 0)
	    return;

#ifdef ISC
#define IFC_IFC_REQ (struct ifreq *) ifc.ifc_buf
#else
#define IFC_IFC_REQ ifc.ifc_req
#endif

	cplim = (char *) IFC_IFC_REQ + ifc.ifc_len;

	for (cp = (char *) IFC_IFC_REQ; cp < cplim; cp += ifr_size (ifr))
#endif /* WINTCP */
	{
#ifndef WINTCP
	    ifr = (struct ifreq *) cp;
#endif
	    if (ifr->ifr_addr.sa_family != AF_INET)
		continue;

	    broad_addr = ifr->ifr_addr;
	    ((struct sockaddr_in *) &broad_addr)->sin_addr.s_addr =
		htonl (INADDR_BROADCAST);
#ifdef SIOCGIFBRDADDR
	    {
		struct ifreq    broad_req;
    
		broad_req = *ifr;
#ifdef WINTCP /* NCR with Wollongong TCP */
		ioc.ic_cmd = IPIOC_GETIFFLAGS;
		ioc.ic_timout = 0;
		ioc.ic_len = sizeof( broad_req );
		ioc.ic_dp = (char *)&broad_req;

		if (ioctl (ipfd, I_STR, (char *) &ioc) != -1 &&
#else /* WINTCP */
		if (ifioctl (socketFD, SIOCGIFFLAGS, (char *) &broad_req) != -1 &&
#endif /* WINTCP */
		    (broad_req.ifr_flags & IFF_BROADCAST) &&
		    (broad_req.ifr_flags & IFF_UP)
		    )
		{
		    broad_req = *ifr;
#ifdef WINTCP /* NCR with Wollongong TCP */
		    ioc.ic_cmd = IPIOC_GETIFBRDADDR;
		    ioc.ic_timout = 0;
		    ioc.ic_len = sizeof( broad_req );
		    ioc.ic_dp = (char *)&broad_req;

		    if (ioctl (ipfd, I_STR, (char *) &ioc) != -1)
#else /* WINTCP */
		    if (ifioctl (socketFD, SIOCGIFBRDADDR, &broad_req) != -1)
#endif /* WINTCP */
			broad_addr = broad_req.ifr_addr;
		    else
			continue;
		}
		else
		    continue;
	    }
#endif
	    in_addr = *((struct sockaddr_in *) &broad_addr);
	    in_addr.sin_port = htons (XDM_UDP_PORT);
#ifdef BSD44SOCKETS
	    in_addr.sin_len = sizeof(in_addr);
#endif
	    RegisterHostaddr ((struct sockaddr *)&in_addr, sizeof (in_addr),
			      BROADCAST_QUERY);
	}
    }
    else
    {

	/* address as hex string, e.g., "12180022" (depreciated) */
	if (strlen(name) == 8 &&
	    FromHex(name, (char *)&in_addr.sin_addr, strlen(name)) == 0)
	{
	    in_addr.sin_family = AF_INET;
	}
	/* Per RFC 1123, check first for IP address in dotted-decimal form */
	else if ((in_addr.sin_addr.s_addr = inet_addr(name)) != -1)
	    in_addr.sin_family = AF_INET;
	else
	{
	    hostent = gethostbyname (name);
	    if (!hostent)
		return;
	    if (hostent->h_addrtype != AF_INET || hostent->h_length != 4)
	    	return;
	    in_addr.sin_family = hostent->h_addrtype;
	    memmove( &in_addr.sin_addr, hostent->h_addr, 4);
	}
	in_addr.sin_port = htons (XDM_UDP_PORT);
#ifdef BSD44SOCKETS
	in_addr.sin_len = sizeof(in_addr);
#endif
	RegisterHostaddr ((struct sockaddr *)&in_addr, sizeof (in_addr),
			  QUERY);
    }
}

#else /* MINIX */

RegisterHostname (name)
    char    *name;
{
    struct hostent	*hostent;
    struct sockaddr_in	in_addr;

    if (!strcmp (name, BROADCAST_HOSTNAME))
    {
	    in_addr.sin_addr.s_addr= htonl(0xFFFFFFFF);
	    in_addr.sin_port = htons (XDM_UDP_PORT);
	    RegisterHostaddr ((struct sockaddr *)&in_addr, sizeof (in_addr),
			      BROADCAST_QUERY);
    }
    else
    {

	/* address as hex string, e.g., "12180022" (depreciated) */
	if (strlen(name) == 8 &&
	    FromHex(name, (char *)&in_addr.sin_addr, strlen(name)) == 0)
	{
	    in_addr.sin_family = AF_INET;
	}
	/* Per RFC 1123, check first for IP address in dotted-decimal form */
	else if ((in_addr.sin_addr.s_addr = inet_addr(name)) != -1)
	    in_addr.sin_family = AF_INET;
	else
	{
	    hostent = gethostbyname (name);
	    if (!hostent)
		return;
	    if (hostent->h_addrtype != AF_INET || hostent->h_length != 4)
	    	return;
	    in_addr.sin_family = hostent->h_addrtype;
	    memmove( &in_addr.sin_addr, hostent->h_addr, 4);
	}
	in_addr.sin_port = htons (XDM_UDP_PORT);
	RegisterHostaddr ((struct sockaddr *)&in_addr, sizeof (in_addr),
			  QUERY);
    }
}
#endif /* !MINIX */

static ARRAYofARRAY8	AuthenticationNames;

static void
RegisterAuthenticationName (name, namelen)
    char    *name;
    int	    namelen;
{
    ARRAY8Ptr	authName;
    if (!XdmcpReallocARRAYofARRAY8 (&AuthenticationNames,
				    AuthenticationNames.length + 1))
	return;
    authName = &AuthenticationNames.data[AuthenticationNames.length-1];
    if (!XdmcpAllocARRAY8 (authName, namelen))
	return;
    memmove( authName->data, name, namelen);
}

int
InitXDMCP (argv)
    char    **argv;
{
    int	soopts = 1;
    XdmcpHeader	header;
    int	i;
#ifdef MINIX
    char *udp_device;
    nwio_udpopt_t udpopt;
    int flags;
    nbio_ref_t ref;
#endif

    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) BROADCAST_QUERY;
    header.length = 1;
    for (i = 0; i < (int)AuthenticationNames.length; i++)
	header.length += 2 + AuthenticationNames.data[i].length;
    XdmcpWriteHeader (&broadcastBuffer, &header);
    XdmcpWriteARRAYofARRAY8 (&broadcastBuffer, &AuthenticationNames);

    header.version = XDM_PROTOCOL_VERSION;
    header.opcode = (CARD16) QUERY;
    header.length = 1;
    for (i = 0; i < (int)AuthenticationNames.length; i++)
	header.length += 2 + AuthenticationNames.data[i].length;
    XdmcpWriteHeader (&directBuffer, &header);
    XdmcpWriteARRAYofARRAY8 (&directBuffer, &AuthenticationNames);
#if defined(STREAMSCONN)
    if ((socketFD = t_open ("/dev/udp", O_RDWR, 0)) < 0)
	return 0;

    if (t_bind( socketFD, NULL, NULL ) < 0)
	{
	t_close(socketFD);
	return 0;
	}

    /*
     * This part of the code looks contrived. It will actually fit in nicely
     * when the CLTS part of Xtrans is implemented.
     */
    {
    struct netconfig *nconf;

    if( (nconf=getnetconfigent("udp")) == NULL )
	{
	t_unbind(socketFD);
	t_close(socketFD);
	return 0;
	}

    if( netdir_options(nconf, ND_SET_BROADCAST, socketFD, NULL) )
	{
	freenetconfigent(nconf);
	t_unbind(socketFD);
	t_close(socketFD);
	return 0;
	}

    freenetconfigent(nconf);
    }
#else
#ifdef MINIX
    udp_device= getenv("UDP_DEVICE");
    if (udp_device == NULL)
    	udp_device= UDP_DEVICE;
    if ((socketFD = open(udp_device, O_RDWR)) == -1)
    	return 0;
    udpopt.nwuo_flags= NWUO_SHARED | NWUO_LP_SEL | NWUO_EN_LOC |
	NWUO_EN_BROAD | NWUO_RP_ANY | NWUO_RA_ANY | NWUO_RWDATALL |
	NWUO_DI_IPOPT;
    if (ioctl(socketFD, NWIOSUDPOPT, &udpopt) == -1)
    {
    	close(socketFD);
    	return 0;
    }
    if ((flags= fcntl(socketFD, F_GETFD)) == -1)
    {
    	close(socketFD);
    	return 0;
    }
    if (fcntl(socketFD, F_SETFD, flags | FD_ASYNCHIO) == -1)
    {
    	close(socketFD);
    	return 0;
    }
    nbio_register(socketFD);
    ref.ref_int= socketFD;
    nbio_setcallback(socketFD, ASIO_READ, read_cb, ref);
#else /* !MINIX */
    if ((socketFD = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	return 0;
#endif /* MINIX */
#endif
#ifndef STREAMSCONN
#ifdef SO_BROADCAST
    soopts = 1;
    if (setsockopt (socketFD, SOL_SOCKET, SO_BROADCAST, (char *)&soopts, sizeof (soopts)) < 0)
	perror ("setsockopt");
#endif
#endif
    
    XtAddInput (socketFD, (XtPointer) XtInputReadMask, ReceivePacket,
		(XtPointer) 0);
    while (*argv)
    {
	RegisterHostname (*argv);
	++argv;
    }
    pingTry = 0;
    PingHosts ((XtPointer)NULL, (XtIntervalId *)NULL);
    return 1;
}

static void
Choose (h)
    HostName	*h;
{
    if (app_resources.xdmAddress)
    {
	struct sockaddr_in  in_addr;
	struct sockaddr	*addr;
	int		family;
	int		len;
	int		fd;
	char		buf[1024];
	XdmcpBuffer	buffer;
	char		*xdm;
#if defined(STREAMSCONN)
        struct  t_call  call, rcv;
#endif
#ifdef MINIX
	char		*tcp_device;
	nwio_tcpconf_t	tcpconf;
	nwio_tcpcl_t	tcpcl;
#endif

	xdm = (char *) app_resources.xdmAddress->data;
	family = (xdm[0] << 8) + xdm[1];
	switch (family) {
	case AF_INET:
#ifdef BSD44SOCKETS
	    in_addr.sin_len = sizeof(in_addr);
#endif
	    in_addr.sin_family = family;
	    memmove( &in_addr.sin_port, xdm + 2, 2);
	    memmove( &in_addr.sin_addr, xdm + 4, 4);
	    addr = (struct sockaddr *) &in_addr;
	    len = sizeof (in_addr);
	    break;
	}
#if defined(STREAMSCONN)
	if ((fd = t_open ("/dev/tcp", O_RDWR, NULL)) == -1)
	{
	    fprintf (stderr, "Cannot create response endpoint\n");
	    fflush(stderr);
	    exit (REMANAGE_DISPLAY);
	}
	if (t_bind (fd, NULL, NULL) == -1)
	{
	    fprintf (stderr, "Cannot bind response endpoint\n");
	    fflush(stderr);
	    t_close (fd);
	    exit (REMANAGE_DISPLAY);
	}
	call.addr.buf=(char *)addr;
	call.addr.len=len;
	call.addr.maxlen=len;
	call.opt.len=0;
	call.opt.maxlen=0;
	call.udata.len=0;
	call.udata.maxlen=0;
	if (t_connect (fd, &call, NULL) == -1)
	{
	    t_error ("Cannot connect to xdm\n");
	    fflush(stderr);
	    t_unbind (fd);
	    t_close (fd);
	    exit (REMANAGE_DISPLAY);
	}
#else
#ifdef MINIX
	tcp_device= getenv("TCP_DEVICE");
	if (tcp_device == NULL)
		tcp_device= TCP_DEVICE;
	if ((fd= open(tcp_device, O_RDWR)) == -1)
	{
	    fprintf (stderr, "Cannot open '%s': %s\n", tcp_device,
	    	strerror(errno));
	    exit (REMANAGE_DISPLAY);
	}
	tcpconf.nwtc_flags= NWTC_EXCL | NWTC_LP_SEL | NWTC_SET_RA | NWTC_SET_RP;
	tcpconf.nwtc_remport= in_addr.sin_port;
	tcpconf.nwtc_remaddr= in_addr.sin_addr.s_addr;
	if (ioctl(fd, NWIOSTCPCONF, &tcpconf) == -1)
	{
	    fprintf (stderr, "NWIOSTCPCONF failed: %s\n", strerror(errno));
	    exit (REMANAGE_DISPLAY);
	}
	tcpcl.nwtcl_flags= 0;
	if (ioctl(fd, NWIOTCPCONN, &tcpcl) == -1)
	{
	    fprintf (stderr, "NWIOTCPCONN failed: %s\n", strerror(errno));
	    exit (REMANAGE_DISPLAY);
	}
#else /* !MINIX */
	if ((fd = socket (family, SOCK_STREAM, 0)) == -1)
	{
	    fprintf (stderr, "Cannot create response socket\n");
	    exit (REMANAGE_DISPLAY);
	}
	if (connect (fd, addr, len) == -1)
	{
	    fprintf (stderr, "Cannot connect to xdm\n");
	    exit (REMANAGE_DISPLAY);
	}
#endif /* MINIX */
#endif
	buffer.data = (BYTE *) buf;
	buffer.size = sizeof (buf);
	buffer.pointer = 0;
	buffer.count = 0;
	XdmcpWriteARRAY8 (&buffer, app_resources.clientAddress);
	XdmcpWriteCARD16 (&buffer, (CARD16) app_resources.connectionType);
	XdmcpWriteARRAY8 (&buffer, &h->hostaddr);
#if defined(STREAMSCONN)
	if( t_snd (fd, (char *)buffer.data, buffer.pointer, 0) < 0 )
	{
	    fprintf (stderr, "Cannot send to xdm\n");
	    fflush(stderr);
	    t_unbind (fd);
	    t_close (fd);
	    exit (REMANAGE_DISPLAY);
	}
	sleep(5);	/* Hack because sometimes the connection gets
			   closed before the data arrives on the other end. */
	t_snddis (fd,NULL);
	t_unbind (fd);
	t_close (fd);
#else
	write (fd, (char *)buffer.data, buffer.pointer);
	close (fd);
#endif
    }
    else
    {
	int i;

    	printf ("%u\n", h->connectionType);
    	for (i = 0; i < (int)h->hostaddr.length; i++)
	    printf ("%u%s", h->hostaddr.data[i],
		    i == h->hostaddr.length - 1 ? "\n" : " ");
    }
}

/* ARGSUSED */
static void
DoAccept (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    XawListReturnStruct	*r;
    HostName		*h;

    r = XawListShowCurrent (list);
    if (r->list_index == XAW_LIST_NONE)
#ifdef XKB
	XkbStdBell(XtDisplay(toplevel),XtWindow(w),0,XkbBI_MinorError);
#else
	XBell (XtDisplay (toplevel), 0);
#endif
    else
    {
	for (h = hostNamedb; h; h = h->next)
	    if (!strcmp (r->string, h->fullname))
	    {
		Choose (h);
	    }
	exit (OBEYSESS_DISPLAY);
    }
}

/* ARGSUSED */
static void
DoCheckWilling (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    XawListReturnStruct	*r;
    HostName		*h;
    
    r = XawListShowCurrent (list);
    if (r->list_index == XAW_LIST_NONE)
	return;
    for (h = hostNamedb; h; h = h->next)
	if (!strcmp (r->string, h->fullname))
	    if (!h->willing)
		XawListUnhighlight (list);
}

/* ARGSUSED */
static void
DoCancel (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    exit (OBEYSESS_DISPLAY);
}

/* ARGSUSED */
static void
DoPing (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    EmptyHostnames ();
    pingTry = 0;
    PingHosts ((XtPointer)NULL, (XtIntervalId *)NULL);
}

static XtActionsRec app_actions[] = {
    "Accept",	    DoAccept,
    "Cancel",	    DoCancel,
    "CheckWilling", DoCheckWilling,
    "Ping",	    DoPing,
};

main (argc, argv)
    int     argc;
    char    **argv;
{
    Arg		position[3];
    Dimension   width, height;
    Position	x, y;

    toplevel = XtInitialize (argv[0], "Chooser", options, XtNumber(options), &argc, argv);

    XtAddConverter(XtRString, XtRARRAY8, CvtStringToARRAY8, NULL, 0);

    XtGetApplicationResources (toplevel, (XtPointer) &app_resources, resources,
			       XtNumber (resources), NULL, (Cardinal) 0);

    XtAddActions (app_actions, XtNumber (app_actions));
    paned = XtCreateManagedWidget ("paned", panedWidgetClass, toplevel, NULL, 0);
    label = XtCreateManagedWidget ("label", labelWidgetClass, paned, NULL, 0);
    viewport = XtCreateManagedWidget ("viewport", viewportWidgetClass, paned, NULL, 0);
    list = XtCreateManagedWidget ("list", listWidgetClass, viewport, NULL, 0);
    box = XtCreateManagedWidget ("box", boxWidgetClass, paned, NULL, 0);
    cancel = XtCreateManagedWidget ("cancel", commandWidgetClass, box, NULL, 0);
    acceptit = XtCreateManagedWidget ("accept", commandWidgetClass, box, NULL, 0);
    ping = XtCreateManagedWidget ("ping", commandWidgetClass, box, NULL, 0);

    /*
     * center ourselves on the screen
     */
    XtSetMappedWhenManaged(toplevel, FALSE);
    XtRealizeWidget (toplevel);

    XtSetArg (position[0], XtNwidth, &width);
    XtSetArg (position[1], XtNheight, &height);
    XtGetValues (toplevel, position, (Cardinal) 2);
    x = (Position)(WidthOfScreen (XtScreen (toplevel)) - width) / 2;
    y = (Position)(HeightOfScreen (XtScreen (toplevel)) - height) / 3;
    XtSetArg (position[0], XtNx, x);
    XtSetArg (position[1], XtNy, y);
    XtSetValues (toplevel, position, (Cardinal) 2);

    /*
     * Run
     */
    XtMapWidget(toplevel);
    InitXDMCP (argv + 1);
    XtMainLoop ();
    exit(0);
    /*NOTREACHED*/
}

/* Converts the hex string s of length len into the byte array d.
   Returns 0 if s was a legal hex string, 1 otherwise.
   */
int
FromHex (s, d, len)
    char    *s, *d;
    int	    len;
{
    int	t;
    int ret = len&1;		/* odd-length hex strings are illegal */
    while (len >= 2)
    {
#define HexChar(c)  ('0' <= (c) && (c) <= '9' ? (c) - '0' : (c) - 'a' + 10)

	if (!ishexdigit(*s))
	    ret = 1;
	t = HexChar (*s) << 4;
	s++;
	if (!ishexdigit(*s))
	    ret = 1;
	t += HexChar (*s);
	s++;
	*d++ = t;
	len -= 2;
    }
    return ret;
}

/*ARGSUSED*/
static void
CvtStringToARRAY8 (args, num_args, fromVal, toVal)
    XrmValuePtr	args;
    Cardinal	*num_args;
    XrmValuePtr	fromVal;
    XrmValuePtr	toVal;
{
    static ARRAY8Ptr	dest;
    char	*s;
    int		len;

    dest = (ARRAY8Ptr) XtMalloc (sizeof (ARRAY8));
    len = fromVal->size;
    s = (char *) fromVal->addr;
    if (!XdmcpAllocARRAY8 (dest, len >> 1))
	XtStringConversionWarning ((char *) fromVal->addr, XtRARRAY8);
    else
    {
	FromHex (s, (char *) dest->data, len);
    }
    toVal->addr = (caddr_t) &dest;
    toVal->size = sizeof (ARRAY8Ptr);
}

#ifdef MINIX
static void read_cb(nbio_ref_t ref, int res, int err)
{
	if (!read_inprogress)
		abort();
	if (res > 0)
	{
		read_size= res;
	}
	else
    	{
    		fprintf(stderr, "chooser: read error: %s\n", res == 0 ?
    			"EOF" : strerror(err));
		read_size= 0;
	}
	read_inprogress= 0;
}
#endif

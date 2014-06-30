/* $XConsortium: connection.c /main/27 1996/12/26 18:53:34 rws $ */
/***********************************************************

Copyright (c) 1987, 1989  X Consortium

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


Copyright 1987, 1989 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/*
 *
 * The connection code/ideas for SVR4/Intel environments was contributed by
 * the following companies/groups:
 *
 *	MetroLink Inc
 *	NCR
 *	Pittsburgh Powercomputing Corporation (PPc)/Quarterdeck Office Systems
 *	SGCS
 *	Unix System Laboratories (USL) / Novell
 *	XFree86
 *
 * The goal is to have common connection code among all SVR4/Intel vendors.
 *
 * ALL THE ABOVE COMPANIES DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, 
 * IN NO EVENT SHALL THESE COMPANIES BE LIABLE FOR ANY SPECIAL, INDIRECT 
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS 
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE 
 * OR PERFORMANCE OF THIS SOFTWARE.
 */

/*****************************************************************
 *  Stuff to create connections --- OS dependent
 *
 *      EstablishNewConnections, CreateWellKnownSockets, ResetWellKnownSockets,
 *      CloseDownConnection, CheckConnections, AddEnabledDevice,
 *	RemoveEnabledDevice, OnlyListToOneClient,
 *      ListenToAllClients,
 *
 *      (WaitForSomething is in its own file)
 *
 *      In this implementation, a client socket table is not kept.
 *      Instead, what would be the index into the table is just the
 *      file descriptor of the socket.  This won't work for if the
 *      socket ids aren't small nums (0 - 2^8)
 *
 *****************************************************************/

#ifdef STREAMSCONN
#define TCPCONN
#endif

#include "misc.h"
#include "Xos.h"			/* for strings, file, time */
#include <sys/param.h>
#include <errno.h>
#ifdef X_NOT_STDC_ENV
extern int errno;
#endif
#include <sys/socket.h>

#include <signal.h>
#include <setjmp.h>

#ifdef hpux
#include <sys/utsname.h>
#include <sys/ioctl.h>
#endif

#ifdef AIXV3
#include <sys/ioctl.h>
#endif

#ifdef TCPCONN
# include <netinet/in.h>
# ifndef hpux
#  ifdef apollo
#   ifndef NO_TCP_H
#    include <netinet/tcp.h>
#   endif
#  else
#   include <netinet/tcp.h>
#  endif
# endif
#endif

#if defined(SO_DONTLINGER) && defined(SO_LINGER)
#undef SO_DONTLINGER
#endif

#ifdef UNIXCONN
/*
 * sites should be careful to have separate /tmp directories for diskless nodes
 */
#include <sys/un.h>
#include <sys/stat.h>
static int unixDomainConnection = -1;
#endif

#include <stdio.h>
#include <sys/uio.h>
#include <X11/Xpoll.h>
#include "osdep.h"
#include "os.h"
#include "lbx.h"
#include "util.h"

#ifdef DNETCONN
#include <netdnet/dn.h>
#endif /* DNETCONN */

typedef long CCID;      /* mask of indices into client socket table */

#ifndef X_NOT_POSIX
#include <unistd.h>
#else
extern int read(), close();
#endif

#ifndef X_UNIX_PATH
#ifdef hpux
#define X_UNIX_DIR	"/usr/spool/sockets/X11"
#define X_UNIX_PATH	"/usr/spool/sockets/X11/"
#define OLD_UNIX_DIR	"/tmp/.X11-unix"
#else
#define X_UNIX_DIR	"/tmp/.X11-unix"
#define X_UNIX_PATH	"/tmp/.X11-unix/X"
#endif
#endif

extern char *display;		/* The display number */
int lastfdesc;			/* maximum file descriptor */

fd_set WellKnownConnections;	/* Listener mask */
fd_set AllSockets;		/* select on this */
fd_set AllClients;		/* available clients */
fd_set LastSelectMask;		/* mask returned from last select call */
fd_set ClientsWithInput;	/* clients with FULL requests in buffer */
fd_set ClientsWriteBlocked;	/* clients who cannot receive output */
fd_set OutputPending;		/* clients with reply/event data ready to go */
int MaxClients = MAXSOCKS;
Bool NewOutputPending;		/* not yet attempted to write some new output */
Bool AnyClientsWriteBlocked;	/* true if some client blocked on write */

Bool RunFromSmartParent;	/* send SIGUSR1 to parent process */
Bool PartialNetwork;		/* continue even if unable to bind all addrs */
static int ParentProcess;

static Bool debug_conns = FALSE;

static fd_set IgnoredClientsWithInput;
static fd_set GrabImperviousClients;
static fd_set SavedAllClients;
static fd_set SavedAllSockets;
static fd_set SavedClientsWithInput;
int GrabInProgress = 0;

int ConnectionTranslation[MAXSOCKS];
int ConnectionOutputTranslation[MAXSOCKS];
extern int auditTrailLevel;

unsigned long raw_stream_out;	/* out to server, in from client */
unsigned long raw_stream_in;	/* in from server, out to client */
extern unsigned long  stream_out_plain;
extern unsigned long  stream_in_plain;
extern void CloseServer();

static void ErrorConnMax(
#if NeedFunctionPrototypes
    register int /*fd*/
#endif
);

static void
PickNewListenDisplay (displayP)

char **displayP;

{
    static char newDisplay[16];
    sprintf (newDisplay, "%d", atoi (*displayP) - 1);
    *displayP = newDisplay;
}


#ifdef TCPCONN
static int
open_tcp_socket (retry)

int retry;  /* boolean - retry if addr busy */

{
    struct sockaddr_in insock;
    int request;
    int retryCount;
#ifndef SO_DONTLINGER
#ifdef SO_LINGER
    static int linger[2] = { 0, 0 };
#endif /* SO_LINGER */
#endif /* SO_DONTLINGER */

    if ((request = socket (AF_INET, SOCK_STREAM, 0)) < 0) 
    {
	Error ("Creating TCP socket");
	return -1;
    } 
#ifdef SO_REUSEADDR
    /* Necesary to restart the server without a reboot */
    {
	int one = 1;
	setsockopt(request, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
    }
#endif /* SO_REUSEADDR */
    {
    bzero ((char *)&insock, sizeof (insock));
#ifdef BSD44SOCKETS
    insock.sin_len = sizeof(insock);
#endif
    insock.sin_family = AF_INET;
    insock.sin_port = htons ((unsigned short)(X_TCP_PORT + atoi (display)));
    insock.sin_addr.s_addr = htonl(INADDR_ANY);

    retryCount = retry ? 19 : 0;

    while (bind(request, (struct sockaddr *) &insock, sizeof (insock)))
    {
	if (retryCount-- == 0) {
	    Error ("Binding TCP socket");
	    close (request);
	    return -1;
	}
	fprintf (stderr, "port %s is busy, retrying\n", display);
#ifdef SO_REUSEADDR
	sleep (1);
#else
	sleep (10);
#endif /* SO_REUSEDADDR */
    }
    }
#ifdef SO_DONTLINGER
    if(setsockopt (request, SOL_SOCKET, SO_DONTLINGER, (char *)NULL, 0))
	Error ("Setting TCP SO_DONTLINGER");
#else
#ifdef SO_LINGER
    if(setsockopt (request, SOL_SOCKET, SO_LINGER,
		   (char *)linger, sizeof(linger)))
	Error ("Setting TCP SO_LINGER");
#endif /* SO_LINGER */
#endif /* SO_DONTLINGER */
    if (listen (request, 5)) {
	Error ("TCP Listening");
	close (request);
	return -1;
    }
    return request;
}
#endif /* TCPCONN */

#if defined(UNIXCONN) && !defined(LOCALCONN)

static struct sockaddr_un unsock;

static int
open_unix_socket ()
{
    int oldUmask;
    int request;

    bzero ((char *) &unsock, sizeof (unsock));
    unsock.sun_family = AF_UNIX;
    oldUmask = umask (0);
#ifdef X_UNIX_DIR
    if (!mkdir (X_UNIX_DIR, 0777))
	chmod (X_UNIX_DIR, 0777);
#endif
    strcpy (unsock.sun_path, X_UNIX_PATH);
    strcat (unsock.sun_path, display);
#ifdef BSD44SOCKETS
    unsock.sun_len = strlen(unsock.sun_path);
#endif
#ifdef hpux
    {  
        /*    The following is for backwards compatibility
         *    with old HP clients. This old scheme predates the use
 	 *    of the /usr/spool/sockets directory, and uses hostname:display
 	 *    in the /tmp/.X11-unix directory
         */
        struct utsname systemName;
	static char oldLinkName[256];

        uname(&systemName);
        strcpy(oldLinkName, OLD_UNIX_DIR);
        if (!mkdir(oldLinkName, 0777))
	    chown(oldLinkName, 2, 3);
        strcat(oldLinkName, "/");
        strcat(oldLinkName, systemName.nodename);
        strcat(oldLinkName, display);
        unlink(oldLinkName);
        symlink(unsock.sun_path, oldLinkName);
    }
#endif	/* hpux */
    unlink (unsock.sun_path);
    if ((request = socket (AF_UNIX, SOCK_STREAM, 0)) < 0) 
    {
	Error ("Creating Unix socket");
	return -1;
    } 
#ifdef BSD44SOCKETS
    if (bind(request, (struct sockaddr *)&unsock, SUN_LEN(&unsock)))
#else
    if (bind(request, (struct sockaddr *)&unsock, strlen(unsock.sun_path)+2))
#endif
    {
	Error ("Binding Unix socket");
	close (request);
	return -1;
    }
    if (listen (request, 5))
    {
	Error ("Unix Listening");
	close (request);
	return -1;
    }
    (void)umask(oldUmask);
    return request;
}
#endif /*UNIXCONN */

#ifdef LOCALCONN
/*
 * This code amply demonstrates why vendors need to talk to each other
 * earlier rather than later.
 *
 * The following is an implementation of the X-server-half of a variety
 * of SYSV386 local connection methods.  This includes compatability
 * with ISC STREAMS, SCO STREAMS, ATT pts connections, ATT/USL named-pipes,
 * and SVR4.2 UNIX DOMAIN connections.  To enable the output of various
 * connection-related information messages to stderr, compile with
 * -DXLOCAL_VERBOSE and set the environment variable XLOCAL_VERBOSE.
 *	XLOCAL_VERBOSE	= 0	print connection availability
 *			= 1	also, print message for each connection
 *			= 2	also, print cleanup information
 *			= 3	even more...
 *
 * See note below about the conflict between ISC and UNIX nodes.
 */

#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/utsname.h>
#ifndef UNIXCONN
#include <sys/stat.h>
#endif

#define X_STREAMS_DIR	"/dev/X"

#ifdef UNIXCONN
#define XLOCAL_UNIX
#define X_UNIX_DEVDIR	"/dev/X/UNIXCON"
#define X_UNIX_DEVPATH	"/dev/X/UNIXCON/X"
#endif

#if !defined(SVR4) || defined(SVR4_ACP)
#define XLOCAL_ISC
#define XLOCAL_SCO
#define X_ISC_DIR	"/dev/X/ISCCONN"
#define X_ISC_PATH	"/dev/X/ISCCONN/X"
#define X_SCO_PATH	"/dev/X"
#define DEV_SPX		"/dev/spx"
static int iscFd = -1;
static int scoFd = -1;
#endif

#ifdef unix
#define XLOCAL_PTS
#define X_PTS_PATH	"/dev/X/server."
#define DEV_PTMX	"/dev/ptmx"
extern char *ptsname();
static int ptsFd = -1;
#endif

#ifdef SVR4
#define XLOCAL_NAMED
static int namedFd = -1;
#define X_NAMED_PATH	"/dev/X/Nserver."
#endif

static fd_set AllStreams; /* bitmap of STREAMS file descriptors */

#ifndef XLOCAL_VERBOSE
#define XLOCAL_MSG(x)	/* as nothing */
#else
static void xlocalMsg();
#define XLOCAL_MSG(x)	xlocalMsg x;
#endif

static int useSlashTmpForUNIX=0;

#ifdef XLOCAL_VERBOSE
/*PRINTFLIKE1*/
static void
xlocalMsg(lvl,str,a,b,c,d,e,f,g,h)
  int lvl;
  char *str;
  int a,b,c,d,e,f,g,h;
{
    static int xlocalMsgLvl = -2;

    if (xlocalMsgLvl == -2) {
	char *tmp;
	if ((tmp = (char *)getenv("XLOCAL_VERBOSE")) != NULL) {
	    xlocalMsgLvl = atoi(tmp);
	} else {
	    xlocalMsgLvl = -1; 
	}
    }
    if (xlocalMsgLvl >= lvl) {
	fprintf(stderr,"X: XLOCAL - ");
	fprintf(stderr,str,a,b,c,d,e,f,g,h);
    }
}
#endif /* XLOCAL_VERBOSE */

/*
 * We have a conflict between ISC and UNIX connections over the use
 * of the /tmp/.X11-unix/Xn path.  Therefore, whichever connection type
 * is specified first in the XLOCAL environment variable gets to use this
 * path for its own device nodes.  The default is ISC.
 *
 * Note that both connection types are always available using their
 * alternate paths at /dev/X/ISCCONN/Xn and /dev/X/UNIXCON/Xn respectively.
 *
 * To make an older client or library use these alternate paths, you
 * need to edit the binary and replace /tmp/.X11-unix with either
 * /dev/X/ISCCONN or /dev/X/UNIXCON depending on its preference.
 */
#define WHITE	" :\t\n\r"

static void
ChooseLocalConnectionType()
{
    char *name,*nameList;

    XLOCAL_MSG((3,"Choosing ISC vs. UNIXDOMAIN connections...\n"));

    useSlashTmpForUNIX=0;

    if ((nameList = (char *)getenv("XLOCAL")) != NULL) {
	nameList = (char *)strdup(nameList);
	name = strtok(nameList,WHITE);
	while (name) {
	    if (!strncmp(name,"SP",2) ||
		!strncmp(name,"ISC",3) ||
		!strncmp(name,"STREAMS",7)) {
		break;
	    } else if (!strncmp(name,"UNIX",4)) {
		useSlashTmpForUNIX=1;
		break;
	    }
	    name = strtok(NULL,WHITE);
	}
	xfree(nameList); 
    } else {
	XLOCAL_MSG((3,"XLOCAL not set in environment.\n"));
    }

    XLOCAL_MSG((3,"Using %s for local connections in /tmp/.X11-unix.\n",
      useSlashTmpForUNIX ? "UNIXCONN" : "ISC"));
}
#undef WHITE

static int
xlocal_unlink(path)
  char *path;
{
    int ret;

    ret = unlink(path);
    if (ret == -1 && errno == EINTR)
      ret = unlink(path);
    if (ret == -1 && errno == ENOENT)
      ret = 0;
    return(ret);
}

#ifdef XLOCAL_UNIX
static struct sockaddr_un unsock;

/* UNIX: UNIX domain sockets as used in SVR4.2 */
static int
open_unix_local()
{
    int oldUmask;
    int request;

    bzero((char *) &unsock, sizeof (unsock));
    unsock.sun_family = AF_UNIX;
    oldUmask = umask (0);

    mkdir(X_STREAMS_DIR, 0777); /* "/dev/X" */
    chmod(X_STREAMS_DIR, 0777);
    if (!mkdir(X_UNIX_DEVDIR, 0777))
      chmod(X_UNIX_DEVDIR, 0777);
    strcpy(unsock.sun_path, X_UNIX_DEVPATH);
    strcat(unsock.sun_path, display);
    xlocal_unlink(unsock.sun_path);
    if ((request = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
	Error("Creating Unix socket");
	return -1;
    } 
    if (bind(request, (struct sockaddr *)&unsock, strlen(unsock.sun_path)+2)) {
	Error("Binding Unix socket");
	close(request);
	return -1;
    }
    if (listen(request, 5)) {
	Error("Unix Listening");
	close(request);
	return -1;
    }
    XLOCAL_MSG((0,"UNIX connections available at [%s]\n", unsock.sun_path));

    if (useSlashTmpForUNIX) {
	char tmpPath[64];
	if (!mkdir(X_UNIX_DIR, 0777))
	  chmod(X_UNIX_DIR, 0777);
	strcpy(tmpPath, X_UNIX_PATH);
	strcat(tmpPath, display);
	xlocal_unlink(tmpPath);
	if (link(unsock.sun_path,tmpPath) == 0) {
	    XLOCAL_MSG((0,"UNIX connections available at [%s]\n", tmpPath));
	}
    }
    (void)umask(oldUmask);
    return request;
}

static void
close_unix_local()
{
    char path[64];

    if (unixDomainConnection != -1) {
	close(unixDomainConnection);
	FD_CLR (unixDomainConnection, &WellKnownConnections);
	unixDomainConnection = -1;
    }
    strcpy(path, X_UNIX_DEVPATH);
    strcat(path, display);
    XLOCAL_MSG((2,"removing [%s] and [%s]\n",path,X_UNIX_DEVDIR)); 
    xlocal_unlink(path);
    xlocal_unlink(X_UNIX_DEVDIR);
    if (useSlashTmpForUNIX) {
	strcpy(path, X_UNIX_PATH);
	strcat(path, display);
	XLOCAL_MSG((2,"removing [%s] and [%s]\n",path,X_UNIX_DIR)); 
	xlocal_unlink(path);
	xlocal_unlink(X_UNIX_DIR);
    }
}

static void
reset_unix_local()
{
    char path[64];
    struct stat statb;
    int need_reset = 0;

    if (unixDomainConnection != -1) {
	strcpy(path, X_UNIX_DEVPATH);
	strcat(path, display);
	if ((stat(path, &statb) == -1) ||
	    ((statb.st_mode & S_IFMT) != 
#if (defined (sun) && defined(SVR4)) || defined(NCR) || defined(sco)
	  		S_IFIFO))
#else
			S_IFSOCK))
#endif
	  need_reset = 1;

	if (useSlashTmpForUNIX) {
	    strcpy(path, X_UNIX_PATH);
	    strcat(path, display);
	    if ((stat(path, &statb) == -1) ||
		((statb.st_mode & S_IFMT) !=
#if (defined (sun) && defined(SVR4)) || defined(NCR) || defined(sco)
	  		S_IFIFO))
#else
			S_IFSOCK))
#endif
	      need_reset = 1;
	}

	if (need_reset) {
	    close_unix_local();
	    unixDomainConnection = open_unix_local();
	    if (unixDomainConnection != -1)
	      FD_SET (unixDomainConnection, &WellKnownConnections);
	}
    }
}

#endif /* XLOCAL_UNIX */

#if defined(XLOCAL_ISC) || defined(XLOCAL_SCO)
static int
connect_spipe(fd1, fd2)
  int fd1, fd2;
{
    long temp;
    struct strfdinsert sbuf;

    sbuf.databuf.maxlen = -1;
    sbuf.databuf.len = -1;
    sbuf.databuf.buf = NULL;
    sbuf.ctlbuf.maxlen = sizeof(long);
    sbuf.ctlbuf.len = sizeof(long);
    sbuf.ctlbuf.buf = (caddr_t)&temp;
    sbuf.offset = 0;
    sbuf.fildes = fd2;
    sbuf.flags = 0;
    if (ioctl(fd1, I_FDINSERT, &sbuf) == -1) return (-1);
  
    return(0);
}

static int
named_spipe(fd, path)
  int fd;
  char *path;
{
    int oldUmask, ret;
    struct stat sbuf;

    oldUmask = umask(0);

    (void) fstat(fd, &sbuf);
    ret = mknod(path, 0020666, sbuf.st_rdev);

    umask(oldUmask);

    if (ret < 0) {
	ret = -1;
    } else {
	ret = fd;
    }
    return(ret);
}
#endif /* defined(XLOCAL_ISC) || defined(XLOCAL_SCO) */

#ifdef XLOCAL_ISC
/*
 * ISC: ISC UNIX V.3.2 compatible streams at /dev/X/ISCCONN/Xn.
 * It will link this to /tmp/.X11-unix/Xn if there are no UNIXDOMAIN
 * connection nodes required.
 */
static int
open_isc_local()
{
    int fd = -1,fds = -1;
    char pathISC[64],pathX11[64];

    mkdir(X_STREAMS_DIR, 0777); /* "/dev/X" */
    chmod(X_STREAMS_DIR, 0777);
    mkdir(X_ISC_DIR, 0777); /* "/dev/X/ISCCONN" */
    chmod(X_ISC_DIR, 0777);

    strcpy(pathISC, X_ISC_PATH);
    strcat(pathISC, display);
  
    if (xlocal_unlink(pathISC) < 0) {
	ErrorF("open_isc_local(): Can't unlink node (%s),\n", pathISC);
	return(-1);
    }

    if ((fds = open(DEV_SPX, O_RDWR)) >= 0 &&
	(fd  = open(DEV_SPX, O_RDWR)) >= 0 ) {

	if (connect_spipe(fds, fd) != -1 &&
	    named_spipe(fds, pathISC) != -1) {

	    XLOCAL_MSG((0,"ISC connections available at [%s]\n", pathISC));

	    if (!useSlashTmpForUNIX) {
		mkdir(X_UNIX_DIR, 0777);
		chmod(X_UNIX_DIR, 0777);
		strcpy(pathX11, X_UNIX_PATH);
		strcat(pathX11, display);
		if (xlocal_unlink(pathX11) < 0) {
		    /*EMPTY*/
		    /* can't get the /tmp node, just return the good one...*/
		} else {
#ifdef SVR4 
		    /* we prefer symbolic links as hard links can't
		       cross filesystems */
		    if (symlink(pathISC,pathX11) == 0) {
			XLOCAL_MSG((0,"ISC connections available at [%s]\n",
				    pathX11));
		    }
#else
		    if (link(pathISC,pathX11) == 0) {
			XLOCAL_MSG((0,"ISC connections available at [%s]\n",
				    pathX11));
		    }
		      
#endif
		}
	    }
	    return(fd);
	} else {
	    Error("open_isc_local(): Can't set up listener pipes");
	}
    } else {
	XLOCAL_MSG((0,"open_isc_local(): can't open %s\n",DEV_SPX));
#ifndef SVR4
	/*
	 * At this point, most SVR4 versions will fail on this, so leave out the
	 * warning
	 */
	ErrorF("open_isc_local(): can't open \"%s\"",DEV_SPX);
	return(-1);
#endif
    }

    (void) close(fds);
    (void) close(fd);
    return(-1);
}

static int
accept_isc_local()
{
    struct strrecvfd buf;

    while (ioctl(iscFd, I_RECVFD, &buf) < 0)
      if (errno != EAGAIN) {
	  Error("accept_isc_local(): Can't read fildes");
	  return(-1);
      }

    XLOCAL_MSG((1,"new ISC connection accepted (%d)\n",buf.fd));
    FD_SET(buf.fd, &AllStreams);
    return(buf.fd);
}

static void
close_isc_local()
{
    char path[64];

    if (iscFd != -1) {
	close(iscFd);
	FD_CLR (iscFd, &WellKnownConnections);
	iscFd = -1;
    }
    strcpy(path, X_ISC_PATH);
    strcat(path, display);
    XLOCAL_MSG((2,"removing [%s] and [%s]\n",path,X_ISC_DIR)); 
    xlocal_unlink(path);
    xlocal_unlink(X_ISC_DIR);
    if (!useSlashTmpForUNIX) {
	strcpy(path, X_UNIX_PATH);
	strcat(path, display);
	XLOCAL_MSG((2,"removing [%s] and [%s]\n",path,X_UNIX_DIR)); 
	xlocal_unlink(path);
	xlocal_unlink(X_UNIX_DIR);
    }
}
#endif /* XLOCAL_ISC */

#ifdef XLOCAL_SCO
/* SCO: SCO/XSIGHT style using /dev/spx */
static int
open_sco_local()
{
    int fds = -1,fdr = -1;
    char pathS[64], pathR[64];

    sprintf(pathS, "%s%sS",X_SCO_PATH, display);
    sprintf(pathR, "%s%sR",X_SCO_PATH, display);
  
    if ((xlocal_unlink(pathS) < 0) || (xlocal_unlink(pathR) < 0)) {

	ErrorF("open_sco_local(): can't unlink node (%s)\n",pathR);
	return(-1);
    }
  
    if ((fds = open(DEV_SPX, O_RDWR)) >= 0 &&
	(fdr = open(DEV_SPX, O_RDWR)) >= 0 ) {

	if (connect_spipe(fds, fdr) != -1 &&
	    named_spipe(fds, pathS) != -1 &&
	    named_spipe(fdr, pathR) != -1) {
      
	    XLOCAL_MSG((0,"SCO connections available at [%s]\n",pathR));

	    return(fds);
	} else {
	    Error("open_sco_local(): can't set up listener pipes");
	}  
    } else {
	XLOCAL_MSG((0,"open_sco_local(): can't open %s",DEV_SPX));
#ifndef SVR4
	/*
	 * At this point, most SVR4 versions will fail on this, so
	 * leave out the warning
	 */
	ErrorF("open_sco_local(): can't open \"%s\"",DEV_SPX);
	return(-1);
#endif
    }

    (void) close(fds);
    (void) close(fdr);
    return(-1);
}


static int
accept_sco_local()
{
    char c;
    int fd;

    if (read(scoFd, &c, 1) < 0) {
	Error("accept_sco_local(): can't read from client");
	return(-1);
    }

    if ((fd = open(DEV_SPX, O_RDWR)) < 0) {
	ErrorF("accept_sco_local(): can't open \"%s\"",DEV_SPX);
	return(-1);
    }

    if (connect_spipe(scoFd, fd) < 0) {
	Error("accept_sco_local(): can't connect pipes");
	(void) close(fd);
	return(-1);
    }

    XLOCAL_MSG((1,"new SCO connection accepted (%d)\n",fd));
    FD_SET(fd, &AllStreams);
    return(fd);
}

static void
close_sco_local()
{
    char pathS[64], pathR[64];

    if (scoFd != -1) {
	close(scoFd);
	FD_CLR (scoFd, &WellKnownConnections);
	scoFd = -1;
    }

    sprintf(pathS, "%s%sS",X_SCO_PATH, display);
    sprintf(pathR, "%s%sR",X_SCO_PATH, display);
  
    XLOCAL_MSG((2,"removing [%s] and [%s]\n",pathS,pathR)); 
    xlocal_unlink(pathS);
    xlocal_unlink(pathR);
}

#endif /* XLOCAL_SCO */

#ifdef XLOCAL_PTS
/* PTS: AT&T style using /dev/ptmx */
static int
open_pts_local()
{
    char *slave;
    int fd;
    char path[64];

    mkdir(X_STREAMS_DIR, 0777);
    chmod(X_STREAMS_DIR, 0777);
  
    strcpy(path, X_PTS_PATH);
    strcat(path, display);
  
    if (open(path, O_RDWR) >= 0 || (xlocal_unlink(path) < 0)) {
	ErrorF("open_pts_local(): server is already running (%s)\n", path);
	return(-1);
    }
  
    if ((fd = open(DEV_PTMX, O_RDWR)) < 0) {
	Error("open_pts_local(): open failed");
	ErrorF("open_pts_local(): can't open \"%s\"",DEV_PTMX);
	return(-1);
    }
  
    grantpt(fd);
    unlockpt(fd);
    slave = ptsname(fd);
    if (link(slave, path) < 0 || chmod(path, 0666) < 0) {
	Error("open_pts_local(): can't set up local listener");
	return(-1);
    }

    if (open(path, O_RDWR) < 0) {
	Error("open_pts_local(): open failed");
	ErrorF("open_pts_local(): can't open %s\n", path);
	close(fd);
	return(-1);
    }
    XLOCAL_MSG((0,"PTS connections available at [%s]\n",path));

    return(fd);
}

static int
accept_pts_local()
{
    int newconn;
    char length;
    char path[64];

    /*
     * first get device-name
     */
    if(read(ptsFd, &length, 1) <= 0 ) {
	Error("accept_pts_local(): can't read slave name length");
	return(-1);
    }

    if(read(ptsFd, path, length) <= 0 ) {
	Error("accept_pts_local(): can't read slave name");
	return(-1);
    }

    path[ length ] = '\0';
      
    if((newconn = open(path,O_RDWR)) < 0) {
	Error("accept_pts_local(): can't open slave");
	return(-1);
    }

    (void) write(newconn, "1", 1); /* send an acknowledge to the client */

    XLOCAL_MSG((1,"new PTS connection accepted (%d)\n",newconn));
    FD_SET(newconn, &AllStreams);
    return(newconn);
}

static void
close_pts_local()
{
    char path[64];

    if (ptsFd != -1) {
	close(ptsFd);
	FD_CLR (ptsFd, &WellKnownConnections);
	ptsFd = -1;
    }
    strcpy(path, X_PTS_PATH);
    strcat(path, display);
    XLOCAL_MSG((2,"removing [%s]\n",path)); 
    xlocal_unlink(path);
}
#endif /* XLOCAL_PTS */

#ifdef XLOCAL_NAMED
/* NAMED: USL style using bi-directional named pipes */
static int
open_named_local()
{
    int fd,fld[2];
    char path[64];
    struct stat sbuf;

    mkdir(X_STREAMS_DIR, 0777);
    chmod(X_STREAMS_DIR, 0777);
  
    strcpy(path, X_NAMED_PATH);
    strcat(path, display);
  
    if (stat(path, &sbuf) != 0) {
	if (errno == ENOENT) {
	    if ((fd = creat(path, (mode_t)0666)) == -1) {
		ErrorF("open_named_local(): can't create %s\n",path);
		return(-1);
	    }
	    close(fd);
	    if (chmod(path, (mode_t)0666) < 0) {
		ErrorF("open_named_local(): can't chmod %s\n",path);
		return(-1);
	    }
	} else {
	    ErrorF("open_named_local(): unknown stat error, %d\n",errno);	
	    return(-1);
	}
    }

    if (pipe(fld) != 0) {
	ErrorF("open_named_local(): pipe failed, errno=%d\n",errno);
	return(-1);
    }

    if (ioctl(fld[0], I_PUSH, "connld") != 0) {
	ErrorF("open_named_local(): ioctl error %d\n",errno);
	return(-1);
    }

    if (fattach(fld[0], path) != 0) {
	ErrorF("open_named_local(): fattach failed, errno=%d\n",errno);
	return(-1);
    } 

    XLOCAL_MSG((0,"NAMED connections available at [%s]\n", path));

    return(fld[1]);
}

static int
accept_named_local()
{
    struct strrecvfd str;

    if (ioctl(namedFd, I_RECVFD, &str) < 0) {
	ErrorF("accept_named_local(): I_RECVFD failed\n");
	return(-1);
    }

    XLOCAL_MSG((1,"new NAMED connection accepted (%d)\n",str.fd));
    FD_SET(str.fd, &AllStreams);
    return(str.fd);
}

static void
close_named_local()
{
    char path[64];

    if (namedFd != -1) {
	close(namedFd);
	FD_CLR (namedFd, &WellKnownConnections);
	namedFd = -1;
    }
    strcpy(path, X_NAMED_PATH);
    strcat(path, display);
    XLOCAL_MSG((2,"removing [%s]\n",path)); 
    xlocal_unlink(path);
}
#endif /* XLOCAL_NAMED */

static int
xlocal_create_sockets(findPort)

int findPort;

{
    int request;

    ChooseLocalConnectionType();

    FD_ZERO(&AllStreams);
#ifdef XLOCAL_PTS
    if ((ptsFd = open_pts_local()) == -1) {
	XLOCAL_MSG((0,"open_pts_local(): failed.\n"));
    } else {
	FD_SET (ptsFd, &WellKnownConnections);
    }
#endif
#ifdef XLOCAL_NAMED
    if ((namedFd = open_named_local()) == -1) {
	XLOCAL_MSG((0,"open_named_local(): failed.\n"));
    } else {
	FD_SET (namedFd, &WellKnownConnections);
    }
#endif
#ifdef XLOCAL_ISC
    if ((iscFd = open_isc_local()) == -1) {
	XLOCAL_MSG((0,"open_isc_local(): failed.\n"));
    } else {
	FD_SET (iscFd, &WellKnownConnections);
    }
#endif
#ifdef XLOCAL_SCO
    if ((scoFd = open_sco_local()) == -1) {
	XLOCAL_MSG((0,"open_sco_local(): failed.\n"));
    } else {
	FD_SET (scoFd, &WellKnownConnections);
    } 
#endif
#ifdef XLOCAL_UNIX
    if ((request = open_unix_local()) == -1) {
	XLOCAL_MSG((0,"open_unix_local(): failed.\n"));
    } else {
	FD_SET (request, &WellKnownConnections);
	unixDomainConnection = request;
    }
#endif
#ifdef TCPCONN
    if ((request = open_tcp_socket(!findPort)) == -1) {
	XLOCAL_MSG((0,"open_tcp_socket(): failed.\n"));
	if (PartialNetwork == FALSE && !findPort) {
	    FatalError("Cannot establish tcp listening socket");
	}
	return -1;
    } else {
	XLOCAL_MSG((0,"TCP connections available at port %d\n",
		    X_TCP_PORT + atoi(display)));
	FD_SET (request, &WellKnownConnections);
	return 0;
    }
#endif /* TCPCONN */
}

static void
xlocal_reset_sockets()
{
#ifdef XLOCAL_UNIX
    reset_unix_local();
#endif
    FD_ZERO(&AllStreams);
}

static int
xlocal_close_sockets()
{
#ifdef XLOCAL_UNIX
    close_unix_local();
#endif /* XLOCAL_UNIX */
#ifdef XLOCAL_ISC
    close_isc_local();
#endif /* XLOCAL_ISC */
#ifdef XLOCAL_SCO
    close_sco_local();
#endif /* XLOCAL_SCO */
#ifdef XLOCAL_PTS
    close_pts_local();
#endif /* XLOCAL_PTS */
#ifdef XLOCAL_NAMED
    close_named_local();
#endif /* XLOCAL_NAMED */
    return(0);
}

int
xlocal_getpeername(fd, from, fromlen)
  int fd;
  struct sockaddr *from;
  int *fromlen;
{
    /* special case for local connections ( ISC, SCO, PTS, NAMED... ) */
    if (FD_SET(fd, &AllStreams)) {
	from->sa_family = AF_UNSPEC;
	*fromlen = 0;
	return 0;
    }
#if defined(TCPCONN) || defined(DNETCONN) || defined(UNIXCONN)
    return getpeername(fd, from, fromlen);
#else
    return(-1);
#endif
}
#define	getpeername	xlocal_getpeername

static int
xlocal_accept(fd, from, fromlen)
  int fd;
  struct sockaddr *from;
  int *fromlen;
{
    int ret;

#ifdef XLOCAL_PTS
    if (fd == ptsFd) return accept_pts_local();
#endif
#ifdef XLOCAL_NAMED
    if (fd == namedFd) return accept_named_local();
#endif
#ifdef XLOCAL_ISC
    if (fd == iscFd) return accept_isc_local();
#endif
#ifdef XLOCAL_SCO
    if (fd == scoFd)  return accept_sco_local(); 
#endif
    /*
     * else we are handling the normal accept case
     */
#if defined(TCPCONN) || defined(DNETCONN) || defined(XLOCAL_UNIX)
    ret = accept (fd, from, fromlen);

#ifdef XLOCAL_UNIX
    if (fd == unixDomainConnection) {
	XLOCAL_MSG((1,"new UNIX connection accepted (%d)\n",ret));
    } else
#endif
      {
	  XLOCAL_MSG((1,"new TCP connection accepted (%d)\n",ret));
      }
#endif
    return(ret);
}
#define	accept		xlocal_accept

#endif /* LOCALCONN */


#ifdef hpux
/*
 * hpux returns EOPNOTSUPP when using getpeername on a unix-domain
 * socket.  In this case, smash the socket address with the address
 * used to bind the connection socket and return success.
 */
hpux_getpeername(fd, from, fromlen)
    int	fd;
    struct sockaddr *from;
    int		    *fromlen;
{
    int	    ret;
    int	    len;

    ret = getpeername(fd, from, fromlen);
    if (ret == -1 && errno == EOPNOTSUPP)
    {
	ret = 0;
	len = strlen(unsock.sun_path)+2;
	if (len > *fromlen)
	    len = *fromlen;
	memmove((char *) from, (char *) &unsock, len);
	*fromlen = len;
    }
    return ret;
}

#define getpeername(fd, from, fromlen)	hpux_getpeername(fd, from, fromlen)

#endif

#ifdef DNETCONN
static int
open_dnet_socket ()
{
    int request;
    struct sockaddr_dn dnsock;

    if ((request = socket (AF_DECnet, SOCK_STREAM, 0)) < 0) 
    {
	Error ("Creating DECnet socket");
	return -1;
    } 
    bzero ((char *)&dnsock, sizeof (dnsock));
    dnsock.sdn_family = AF_DECnet;
    sprintf(dnsock.sdn_objname, "X$X%d", atoi (display));
    dnsock.sdn_objnamel = strlen(dnsock.sdn_objname);
    if (bind (request, (struct sockaddr *) &dnsock, sizeof (dnsock)))
    {
	Error ("Binding DECnet socket");
	close (request);
	return -1;
    }
    if (listen (request, 5))
    {
	Error ("DECnet Listening");
	close (request);
	return -1;
    }
    return request;
}
#endif /* DNETCONN */

/*****************
 * CreateWellKnownSockets
 *    At initialization, create the sockets to listen on for new clients.
 *****************/

extern Bool proxyMngr;

void
CreateWellKnownSockets()
{
    int		tcp_request = -1, dnet_request = - 1, unix_request;
    int		i, done = 0;
    int		findPort = proxyMngr;

    FD_ZERO(&AllSockets);
    FD_ZERO(&AllClients);
    FD_ZERO(&LastSelectMask);
    FD_ZERO(&ClientsWithInput);

    for (i=0; i<MAXSOCKS; i++) ConnectionTranslation[i] = 0;
    for (i=0; i<MAXSOCKS; i++) ConnectionOutputTranslation[i] = 0;
#ifdef XNO_SYSCONF
#undef _SC_OPEN_MAX
#endif
#ifdef _SC_OPEN_MAX
    lastfdesc = sysconf(_SC_OPEN_MAX) - 1;
#else
#ifdef hpux
    lastfdesc = _NFILE - 1;
#else
    lastfdesc = getdtablesize() - 1;
#endif
#endif

    if (lastfdesc > MAXSOCKS)
    {
	lastfdesc = MAXSOCKS;
	if (debug_conns)
	    ErrorF( "GOT TO END OF SOCKETS %d\n", MAXSOCKS);
    }

    while (!done)
    {
	FD_ZERO(&WellKnownConnections);

#ifdef LOCALCONN
	if (xlocal_create_sockets() == -1 && findPort)
	{
	    PickNewListenDisplay (&display);
	    continue;
	}
#else  /* LOCALCONN */
#ifdef TCPCONN
	if ((tcp_request = open_tcp_socket (!findPort)) != -1) {
	    FD_SET (tcp_request, &WellKnownConnections);
	}
	else if (findPort)
	{
	    PickNewListenDisplay (&display);
	    continue;
	}
	else if (!PartialNetwork) 
	{
	    FatalError ("Cannot establish tcp listening socket");
	}
#endif /* TCPCONN */
#ifdef DNETCONN
	if ((dnet_request = open_dnet_socket ()) != -1) {
	    FD_SET (dnet_request, &WellKnownConnections);
	}
	else if (findPort)
	{
	    PickNewListenDisplay (&display);
	    if (tcp_request >= 0)
		close (tcp_request);
	    continue;
	}
	else if (!PartialNetwork) 
	{
	    FatalError ("Cannot establish dnet listening socket");
	}
#endif /* DNETCONN */
#ifdef UNIXCONN
	if ((unix_request = open_unix_socket ()) != -1) {
	    FD_SET (unix_request, &WellKnownConnections);
	    unixDomainConnection = unix_request;
	}
	else if (findPort)
	{
	    PickNewListenDisplay (&display);
	    if (tcp_request >= 0)
		close (tcp_request);
	    if (dnet_request >= 0)
		close (dnet_request);
	    continue;
	}
	else if (!PartialNetwork) 
	{
	    FatalError ("Cannot establish unix listening socket");
	}
#endif /* UNIXCONN */

#endif /* LOCALCONN */

	done = 1;
    }

    /*
     * We will start listening on the well known connections
     * after the proxy finishes connecting to the server.
     */

    if (!XFD_ANYSET (&WellKnownConnections))
        FatalError ("Cannot establish any listening sockets");
    OsSignal (SIGPIPE, SIG_IGN);
    OsSignal (SIGHUP, AutoResetServer);
    OsSignal (SIGINT, GiveUp);
    OsSignal (SIGTERM, GiveUp);
    /*
     * Magic:  If SIGUSR1 was set to SIG_IGN when
     * the server started, assume that either
     *
     *  a- The parent process is ignoring SIGUSR1
     *
     * or
     *
     *  b- The parent process is expecting a SIGUSR1
     *     when the server is ready to accept connections
     *
     * In the first case, the signal will be harmless,
     * in the second case, the signal will be quite
     * useful
     */
    if (OsSignal (SIGUSR1, SIG_IGN) == SIG_IGN)
	RunFromSmartParent = TRUE;
    ParentProcess = getppid ();
    if (RunFromSmartParent) {
	if (ParentProcess > 0) {
	    kill (ParentProcess, SIGUSR1);
	}
    }
}


extern int proxy_manager_fd;

ListenToProxyManager ()

{
    FD_SET(proxy_manager_fd, &AllSockets);
}

void
ListenWellKnownSockets ()

{
    XFD_ORSET (&AllSockets, &AllSockets, &WellKnownConnections);
}

void
ResetWellKnownSockets ()
{
    ResetOsBuffers();

#ifdef LOCALCONN
	xlocal_reset_sockets();
#else /* LOCALCONN */
#if defined(UNIXCONN) && !defined(SVR4)
    if (unixDomainConnection != -1)
    {
	/*
	 * see if the unix domain socket has disappeared
	 */
	struct stat	statb;

	if (stat (unsock.sun_path, &statb) == -1 ||
	    (statb.st_mode & S_IFMT) != S_IFSOCK)
	{
	    ErrorF ("Unix domain socket %s trashed, recreating\n",
	    	unsock.sun_path);
	    (void) unlink (unsock.sun_path);
	    (void) close (unixDomainConnection);
	    FD_CLR(unixDomainConnection, &WellKnownConnections);
	    unixDomainConnection = open_unix_socket ();
	    if (unixDomainConnection != -1)
		FD_SET (unixDomainConnection, &WellKnownConnections);
	}
    }
#endif /* UNIXCONN */

#endif /* LOCALCONN */


    /*
     * See above in CreateWellKnownSockets about SIGUSR1
     */
    if (RunFromSmartParent) {
	if (ParentProcess > 0) {
	    kill (ParentProcess, SIGUSR1);
	}
    }
}

void
AvailableClientInput (client)
    ClientPtr	client;
{
    OsCommPtr oc = (OsCommPtr)client->osPrivate;

    if (FD_ISSET(oc->fd, &AllSockets))
	FD_SET(oc->fd, &ClientsWithInput);
}

static int
ClientRead (fd, buf, len)
    int fd;
    char *buf;
    int len;
{
    int n;
    n = read(fd, buf, len);
    if (n > 0)
	raw_stream_out += n;
    return n;
}

static int
ClientWritev(fd, iov, iovcnt)
    int fd;
    struct iovec *iov;
    int iovcnt;
{
    int n;

    n = writev(fd, iov, iovcnt);
    if (n > 0)
	raw_stream_in += n;
    return n;
}

static int
ServerRead (fd, buf, len)
    int fd;
    char *buf;
    int len;
{
    int n;
    n = read(fd, buf, len);
    if (n > 0)
	stream_in_plain += n;
    return n;
}

static int
ServerWritev(fd, iov, iovcnt)
    int fd;
    struct iovec *iov;
    int iovcnt;
{
    int n;

    n = writev(fd, iov, iovcnt);
    if (n > 0)
	stream_out_plain += n;
    return n;
}

ClientPtr
AllocNewConnection (fd, to_server)
    int	    fd;
    Bool    to_server;
{
    OsCommPtr	oc;
    ClientPtr	client;
    
    if (fd >= lastfdesc)
	return NullClient;
    oc = (OsCommPtr)xalloc(sizeof(OsCommRec));
    if (!oc)
	return NullClient;
    oc->fd = fd;
    oc->input = (ConnectionInputPtr)NULL;
    oc->output = (ConnectionOutputPtr)NULL;
    if (to_server) {
	oc->Read = ServerRead;
	oc->Writev = ServerWritev;
	oc->Close = CloseServer;
    } else {
	oc->Read = ClientRead;
	oc->Writev = ClientWritev;
	oc->Close = CloseDownFileDescriptor;
    }
    oc->flushClient = StandardFlushClient;
    oc->ofirst = (ConnectionOutputPtr) NULL;
    if (!(client = NextAvailableClient((pointer)oc)))
    {
	xfree (oc);
	return NullClient;
    }
    if (!ConnectionTranslation[fd])
    {
	ConnectionTranslation[fd] = client->index;
	ConnectionOutputTranslation[fd] = client->index;
	if (GrabInProgress)
	{
	    FD_SET(fd, &SavedAllClients);
	    FD_SET(fd, &SavedAllSockets);
	}
	else
	{
	    FD_SET(fd, &AllClients);
	    FD_SET(fd, &AllSockets);
	}
    }
    client->public.readRequest = StandardReadRequestFromClient;
    client->public.writeToClient = StandardWriteToClient;
    client->public.requestLength = StandardRequestLength;
    return client;
}

void
SwitchConnectionFuncs (client, Read, Writev)
    ClientPtr	client;
    int		(*Read)();
    int		(*Writev)();
{
    OsCommPtr	oc = (OsCommPtr) client->osPrivate;

    oc->Read = Read;
    oc->Writev = Writev;
}

void
StartOutputCompression(client, CompressOn, CompressOff)
    ClientPtr	client;
    void	(*CompressOn)();
    void	(*CompressOff)();
{
    OsCommPtr	oc = (OsCommPtr) client->osPrivate;

    oc->compressOn = CompressOn;
    oc->compressOff = CompressOff;
    oc->flushClient = LbxFlushClient;
}

/*****************
 * EstablishNewConnections
 *    If anyone is waiting on listened sockets, accept them.
 *    Returns a mask with indices of new clients.  Updates AllClients
 *    and AllSockets.
 *****************/

/*ARGSUSED*/
Bool
EstablishNewConnections(clientUnused, closure)
    ClientPtr clientUnused;
    pointer closure;
{
    fd_mask readyconnections;     /* mask of listeners that are ready */
    int curconn;                  /* fd of listener that's ready */
    register int newconn;         /* fd of new client */
    register int i;
    register ClientPtr client;
    register OsCommPtr oc;
    fd_set tmask;

#ifdef TCP_NODELAY
    union {
	struct sockaddr sa;
#ifdef UNIXCONN
	struct sockaddr_un un;
#endif /* UNIXCONN */
#ifdef TCPCONN
	struct sockaddr_in in;
#endif /* TCPCONN */
#ifdef DNETCONN
	struct sockaddr_dn dn;
#endif /* DNETCONN */
    } from;
    int	fromlen;
#endif /* TCP_NODELAY */

    XFD_ANDSET (&tmask, (fd_set*)closure, &WellKnownConnections);
    readyconnections = tmask.fds_bits[0];
    if (!readyconnections)
	return TRUE;
    while (readyconnections) 
    {
	curconn = ffs (readyconnections) - 1;
	readyconnections &= ~(1 << curconn);
	if ((newconn = accept (curconn,
			      (struct sockaddr *) NULL, 
			      (int *)NULL)) < 0) 
	    continue;
#ifdef TCP_NODELAY
	fromlen = sizeof (from);
	if (!getpeername (newconn, &from.sa, &fromlen))
	{
	    if (fromlen && (from.sa.sa_family == AF_INET)) 
	    {
		int mi = 1;
		setsockopt (newconn, IPPROTO_TCP, TCP_NODELAY,
			   (char *)&mi, sizeof (int));
	    }
	}
#endif /* TCP_NODELAY */
    /* ultrix reads hang on Unix sockets, hpux reads fail, AIX fails too */
#if defined(O_NONBLOCK) && (!defined(ultrix) && !defined(hpux) && !defined(AIXV3) && !defined(uniosu))
	(void) fcntl (newconn, F_SETFL, O_NONBLOCK);
#else
#ifdef FIOSNBIO
	{
	    int	arg;
	    arg = 1;
	    ioctl(newconn, FIOSNBIO, &arg);
	}
#else
#if (defined(AIXV3) || defined(uniosu)) && defined(FIONBIO)
	{
	    int arg;
	    arg = 1;
	    ioctl(newconn, FIONBIO, &arg);
	}
#else
	fcntl (newconn, F_SETFL, FNDELAY);
#endif
#endif
#endif

	client = AllocNewConnection (newconn, FALSE);
	if (!client)
	{
	    ErrorConnMax(newconn);
	    close(newconn);
	    continue;
	}
    }
    return TRUE;
}

#define NOROOM "Maximum number of clients reached"

/************
 *   ErrorConnMax
 *     Fail a connection due to lack of client or file descriptor space
 ************/

static void
ErrorConnMax(fd)
    register int fd;
{
    xConnSetupPrefix csp;
    char pad[3];
    struct iovec iov[3];
    char byteOrder = 0;
    int whichbyte = 1;
    struct timeval waittime;
    fd_set mask;

    /* if these seems like a lot of trouble to go to, it probably is */
    waittime.tv_sec = BOTIMEOUT / MILLI_PER_SECOND;
    waittime.tv_usec = (BOTIMEOUT % MILLI_PER_SECOND) *
		       (1000000 / MILLI_PER_SECOND);
    FD_ZERO(&mask);
    FD_SET(fd, &mask);
    (void)Select(fd + 1, &mask, NULL, NULL, &waittime);
    /* try to read the byte-order of the connection */
    (void)read(fd, &byteOrder, 1);
    if ((byteOrder == 'l') || (byteOrder == 'B'))
    {
	csp.success = xFalse;
	csp.lengthReason = sizeof(NOROOM) - 1;
	csp.length = (sizeof(NOROOM) + 2) >> 2;
	csp.majorVersion = X_PROTOCOL;
	csp.minorVersion = X_PROTOCOL_REVISION;
	if (((*(char *) &whichbyte) && (byteOrder == 'B')) ||
	    (!(*(char *) &whichbyte) && (byteOrder == 'l')))
	{
	    swaps(&csp.majorVersion, whichbyte);
	    swaps(&csp.minorVersion, whichbyte);
	    swaps(&csp.length, whichbyte);
	}
	iov[0].iov_len = sz_xConnSetupPrefix;
	iov[0].iov_base = (char *) &csp;
	iov[1].iov_len = csp.lengthReason;
	iov[1].iov_base = NOROOM;
	iov[2].iov_len = (4 - (csp.lengthReason & 3)) & 3;
	iov[2].iov_base = pad;
	(void)ClientWritev(fd, iov, 3);
    }
}

/************
 *   CloseDownFileDescriptor:
 *     Remove this file descriptor and it's I/O buffers, etc.
 ************/

void
CloseDownFileDescriptor(client)
    ClientPtr	client;
{
    register OsCommPtr oc = (OsCommPtr) client->osPrivate;
    int connection = oc->fd;

    close(connection);
    ConnectionTranslation[connection] = 0;
    ConnectionOutputTranslation[connection] = 0;
    FD_CLR(connection, &AllSockets);
    FD_CLR(connection, &AllClients);
#ifdef LOCALCONN
    FD_CLR(connection, &AllStreams);
#endif
    FD_CLR(connection, &ClientsWithInput);
    FD_CLR(connection, &GrabImperviousClients);
    if (GrabInProgress)
    {
	FD_CLR(connection, &SavedAllSockets);
	FD_CLR(connection, &SavedAllClients);
	FD_CLR(connection, &SavedClientsWithInput);
    }
    FD_CLR(connection, &ClientsWriteBlocked);
    if (!XFD_ANYSET(&ClientsWriteBlocked))
    	AnyClientsWriteBlocked = FALSE;
    FD_CLR(connection, &OutputPending);
}

/*****************
 * CheckConections
 *    Some connection has died, go find which one and shut it down 
 *    The file descriptor has been closed, but is still in AllClients.
 *    If would truly be wonderful if select() would put the bogus
 *    file descriptors in the exception mask, but nooooo.  So we have
 *    to check each and every socket individually.
 *****************/

void
CheckConnections()
{
    fd_mask		mask;
    fd_set		tmask; 
    register int	curclient, curoff;
    int			i;
    struct timeval	notime;
    int r;

    notime.tv_sec = 0;
    notime.tv_usec = 0;

    for (i=0; i<howmany(XFD_SETSIZE, NFDBITS); i++)
    {
	mask = AllClients.fds_bits[i];
        while (mask)
    	{
	    curoff = ffs (mask) - 1;
 	    curclient = curoff + (i << 5);
            FD_ZERO(&tmask);
            FD_SET(curclient, &tmask);
            r = Select (curclient + 1, &tmask, NULL, NULL, &notime);
            if (r < 0)
		CloseDownClient(clients[ConnectionTranslation[curclient]]);
	    mask &= ~(1 << curoff);
	}
    }	
}


/*****************
 * CloseDownConnection
 *    Delete client from AllClients and free resources 
 *****************/

void
CloseDownConnection(client)
    ClientPtr client;
{
    OsCommPtr oc = (OsCommPtr)client->osPrivate;

    if (oc->output && oc->output->count)
	FlushClient(client, oc, (char *)NULL, 0);
    ConnectionTranslation[oc->fd] = 0;
    (*oc->Close) (client);
    FreeOsBuffers(oc);
    xfree(oc);
    client->osPrivate = (pointer)NULL;
    if (auditTrailLevel > 1)
	AuditF("client %d disconnected\n", client->index);
}

/*****************
 * OnlyListenToOneClient:
 *    Only accept requests from  one client.  Continue to handle new
 *    connections, but don't take any protocol requests from the new
 *    ones.  Note that if GrabInProgress is set, EstablishNewConnections
 *    needs to put new clients into SavedAllSockets and SavedAllClients.
 *    Note also that there is no timeout for this in the protocol.
 *    This routine is "undone" by ListenToAllClients()
 *****************/

void
OnlyListenToOneClient(client)
    ClientPtr client;
{
    OsCommPtr oc = (OsCommPtr)client->osPrivate;
    int connection = oc->fd;

    if (! GrabInProgress)
    {
	XFD_COPYSET(&ClientsWithInput, &SavedClientsWithInput);
	XFD_ANDSET(&ClientsWithInput,
		       &ClientsWithInput, &GrabImperviousClients);
	if (FD_ISSET(connection, &SavedClientsWithInput))
	{
	    FD_CLR(connection, &SavedClientsWithInput);
	    FD_SET(connection, &ClientsWithInput);
	}
	XFD_UNSET(&SavedClientsWithInput, &GrabImperviousClients);
	FD_CLR(connection, &AllSockets);
	XFD_COPYSET(&AllSockets, &SavedAllSockets);
	FD_CLR(connection, &AllClients);
	XFD_COPYSET(&AllClients, &SavedAllClients);
	XFD_UNSET(&AllSockets, &AllClients);
	XFD_ANDSET(&AllClients, &AllClients, &GrabImperviousClients);
	FD_SET(connection, &AllClients);
	XFD_ORSET(&AllSockets, &AllSockets, &AllClients);
	GrabInProgress = client->index;
    }
}

/****************
 * ListenToAllClients:
 *    Undoes OnlyListentToOneClient()
 ****************/

void
ListenToAllClients()
{
    if (GrabInProgress)
    {
	XFD_ORSET(&AllSockets, &AllSockets, &SavedAllSockets);
	XFD_ORSET(&AllClients, &AllClients, &SavedAllClients);
	XFD_ORSET(&ClientsWithInput, &ClientsWithInput, &SavedClientsWithInput);
	GrabInProgress = 0;
    }	
}

/****************
 * IgnoreClient
 *    Removes one client from input masks.
 *    Must have cooresponding call to AttendClient.
 ****************/

void
IgnoreClient (client)
    ClientPtr	client;
{
    OsCommPtr oc = (OsCommPtr)client->osPrivate;
    int connection = oc->fd;

    if (!GrabInProgress || FD_ISSET (connection, &AllClients))
    {
    	if (FD_ISSET (connection, &ClientsWithInput))
	    FD_SET(connection, &IgnoredClientsWithInput);
    	else
	    FD_CLR(connection, &IgnoredClientsWithInput);
    	FD_CLR(connection, &ClientsWithInput);
    	FD_CLR(connection, &AllSockets);
    	FD_CLR(connection, &AllClients);
	FD_CLR(connection, &LastSelectMask);
    }
    else
    {
    	if (FD_ISSET (connection, &SavedClientsWithInput))
	    FD_SET(connection, &IgnoredClientsWithInput);
    	else
	    FD_CLR(connection, &IgnoredClientsWithInput);
	FD_CLR(connection, &SavedClientsWithInput);
	FD_CLR(connection, &SavedAllSockets);
	FD_CLR(connection, &SavedAllClients);
    }
    isItTimeToYield = TRUE;
}

/****************
 * AttendClient
 *    Adds one client back into the input masks.
 ****************/

void
AttendClient (client)
    ClientPtr	client;
{
    OsCommPtr oc = (OsCommPtr)client->osPrivate;
    int connection = oc->fd;

    if (!GrabInProgress || GrabInProgress == client->index ||
	FD_ISSET(connection, &GrabImperviousClients))
    {
    	FD_SET(connection, &AllClients);
    	FD_SET(connection, &AllSockets);
	FD_SET(connection, &LastSelectMask);
    	if (FD_ISSET (connection, &IgnoredClientsWithInput))
	    FD_SET(connection, &ClientsWithInput);
    }
    else
    {
	FD_SET(connection, &SavedAllClients);
	FD_SET(connection, &SavedAllSockets);
	if (FD_ISSET(connection, &IgnoredClientsWithInput))
	    FD_SET(connection, &SavedClientsWithInput);
    }
}

/* make client impervious to grabs; assume only executing client calls this */

void
MakeClientGrabImpervious(client)
    ClientPtr client;
{
    OsCommPtr oc = (OsCommPtr)client->osPrivate;
    int connection = oc->fd;

    FD_SET(connection, &GrabImperviousClients);
}

/* make client pervious to grabs; assume only executing client calls this */

void
MakeClientGrabPervious(client)
    ClientPtr client;
{
    OsCommPtr oc = (OsCommPtr)client->osPrivate;
    int connection = oc->fd;

    FD_CLR(connection, &GrabImperviousClients);
    if (GrabInProgress && (GrabInProgress != client->index))
    {
	if (FD_ISSET(connection, &ClientsWithInput))
	{
	    FD_SET(connection, &SavedClientsWithInput);
	    FD_CLR(connection, &ClientsWithInput);
	}
	FD_CLR(connection, &AllSockets);
	FD_CLR(connection, &AllClients);
	isItTimeToYield = TRUE;
    }
}

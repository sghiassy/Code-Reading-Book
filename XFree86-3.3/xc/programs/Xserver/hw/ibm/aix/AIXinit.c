/*
 * $XConsortium: AIXinit.c /main/6 1995/12/05 15:43:24 matt $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

#include <fcntl.h>

#ifndef _IBM_LFT
#include <sys/hft.h>
#endif

#include "Xmd.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursor.h"
#include "ibmScreen.h"
#include "OSio.h"

#ifndef _IBM_LFT
#include "hftQueue.h"
#endif

#include "ibmTrace.h"

void
ibmMachineDependentInit()
{
    TRACE(("ibmMachineDependentInit()\n"));

#ifndef _IBM_LFT
    hftInitQueue(AIXDefaultDisplay,FALSE);
    RemoveEnabledDevice(hftQFD);
#endif
    return;
}

/***==================================================================***/

#if !defined(FORCE_DISPLAY_NUMBER)

#if !defined(MAX_SERVER_NUM)
#define MAX_SERVER_NUM 255
#endif

#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

#include <errno.h>
#include <sys/wait.h>

#include "Xproto.h"

int AIXTCPSocket= -1;
static char dummy[20];

extern char *display ;

void
AIXFindServerIndex()
{
struct sockaddr_in insock ;
int sockNum, sockFD ;

    if ((sockFD = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
	Error("creating TCP socket (in FindATCPSocket)\n");
	display= "0";
	return;
    }

    bzero( (char *)&insock, sizeof insock );
#ifdef BSD44SOCKETS
    insock.sin_len = sizeof(insock);
#endif
    insock.sin_family = AF_INET;
    insock.sin_addr.s_addr = htonl(INADDR_ANY);

    for ( sockNum = 0 ; sockNum < MAX_SERVER_NUM ; sockNum++ ) {
	insock.sin_port = htons( X_TCP_PORT + sockNum ) ;
	if ( bind( sockFD, (struct sockaddr *) &insock, sizeof insock ) ) {
	    if (errno!=EADDRINUSE) {
		display= "0";
		return;
	    }
	}
	else break;
    }
    AIXTCPSocket= sockFD;
    (void) sprintf( display = dummy, "%d", sockNum ) ;
    return;
}
#endif /* FORCE_DISPLAY_NUMBER */

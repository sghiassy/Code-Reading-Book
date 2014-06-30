/* $XConsortium: pm.c /main/21 1996/12/01 00:39:04 swick $ */

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

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xfuncs.h>
#include <X11/Xmd.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/ICE/ICElib.h>
#include <X11/ICE/ICEmsg.h>
#include <X11/ICE/ICEproto.h>
#include <X11/PM/PM.h>
#include <X11/PM/PMproto.h>
#include "pm.h"

#define ERROR_STRING_SIZE 256

void PMprocessMessages ();

int PMopcode;
int PMversionCount = 1;
IcePoVersionRec	PMversions[] =
	{{PM_MAJOR_VERSION, PM_MINOR_VERSION, PMprocessMessages}};
int gotFirstGetProxyAddr = 0;
extern char *display_name;
IceConn	PM_iceConn;
int proxy_manager_fd = -1;

char proxyAddress[40];

Bool
CheckForProxyManager ()

{
    if (getenv ("PROXY_MANAGER"))
	return 1;
    else
	return 0;
}


ConnectToProxyManager ()

{
    char    *proxyManagerAddr;
    char    errorString[ERROR_STRING_SIZE];

    proxyManagerAddr = (char *) getenv ("PROXY_MANAGER");

    if (proxyManagerAddr)
    {
	if (!_ConnectToProxyManager (proxyManagerAddr, errorString))
	{
	    fprintf (stderr, "%s\n", errorString);
	    exit (1);
	}
    }
}


Status
_ConnectToProxyManager (pmAddr, errorString)

char *pmAddr;
char *errorString;

{
    IceProtocolSetupStatus	setupstat;
    char			*vendor = NULL;
    char			*release = NULL;
    pmStartProxyMsg		*pMsg;
    char 			*pData;
    int				len, i;
    int				majorVersion, minorVersion;
    Bool			gotReply, ioErrorOccured;
    char			iceError[ERROR_STRING_SIZE];
    char			*serviceName = NULL, *serverAddress = NULL;
    char			*hostAddress = NULL, *startOptions = NULL;


    /*
     * Register support for PROXY_MANAGEMENT.
     */

    if ((PMopcode = IceRegisterForProtocolSetup (
	PM_PROTOCOL_NAME,
	"X Consortium, Inc.", "1.0",
	PMversionCount, PMversions,
	0, /* authcount */
	NULL, /* authnames */ 
        NULL, /* authprocs */
	NULL  /* IceIOErrorProc */ )) < 0)
    {
	strcpy (errorString,
	    "Could not register PROXY_MANAGEMENT protocol with ICE");
	return 0;
    }


    if ((PM_iceConn = IceOpenConnection (
	pmAddr,	NULL, 0, 0, sizeof(iceError), iceError)) == NULL)
    {
	sprintf (errorString,
	    "Could not open ICE connection to proxy manager: %s", iceError);
	return 0;
    }

    setupstat = IceProtocolSetup (PM_iceConn, PMopcode, NULL,
	False /* mustAuthenticate */,
	&majorVersion, &minorVersion,
	&vendor, &release, ERROR_STRING_SIZE, errorString);

    if (setupstat != IceProtocolSetupSuccess)
    {
	IceCloseConnection (PM_iceConn);
	sprintf (errorString,
	    "Could not initialize proxy management protocol: %s",
	    iceError);
	return 0;
    }


    /*
     * Now send the StartProxy message.
     */

    len = STRING_BYTES ("LBX");

    IceGetHeaderExtra (PM_iceConn, PMopcode, PM_StartProxy,
	SIZEOF (pmStartProxyMsg), WORD64COUNT (len),
	pmStartProxyMsg, pMsg, pData);

    STORE_STRING (pData, "LBX");

    IceFlush (PM_iceConn);

    while (!gotFirstGetProxyAddr)
    {
	/* wait for a GetProxyAddr request before continuing... */
	int status = IceProcessMessages( PM_iceConn, NULL, NULL );
	if (status == IceProcessMessagesIOError)
	{
	    fprintf( stderr, "IO error occured connecting to proxy manager");
	    return FALSE;
	}
    }

    proxy_manager_fd = IceConnectionNumber (PM_iceConn);

    return 1;
}


void
SendGetProxyAddrReply (
    IceConn requestor_iceConn,
    int status,
    char *addr,
    char *error)

{
    int len = STRING_BYTES (addr) + STRING_BYTES (error);
    pmGetProxyAddrReplyMsg *pReply;
    char *pData;

    IceGetHeaderExtra (requestor_iceConn,
	PMopcode, PM_GetProxyAddrReply,
	SIZEOF (pmGetProxyAddrReplyMsg), WORD64COUNT (len),
	pmGetProxyAddrReplyMsg, pReply, pData);

    pReply->status = status;

    STORE_STRING (pData, addr);
    STORE_STRING (pData, error);

    IceFlush (requestor_iceConn);
}


static int
casecmp (str1, str2)
    char *str1, *str2;
{
    char buf1[512],buf2[512];
    char c, *s;
    register int n;

    for (n=0, s = buf1; c = *str1++; n++) {
	if (isupper(c))
	    c = tolower(c);
	if (n>510)
	    break;
	*s++ = c;
    }
    *s = '\0';
    for (n=0, s = buf2; c = *str2++; n++) {
	if (isupper(c))
	    c = tolower(c);
	if (n>510)
	    break;
	*s++ = c;
    }
    *s = '\0';
    return (strcmp(buf1, buf2));
}


void
PMprocessMessages (iceConn, clientData, opcode,
    length, swap, replyWait, replyReadyRet)

IceConn		 iceConn;
IcePointer       clientData;
int		 opcode;
unsigned long	 length;
Bool		 swap;
IceReplyWaitInfo *replyWait;
Bool		 *replyReadyRet;

{
    switch (opcode)
    {
    case PM_GetProxyAddr:
    {
	pmGetProxyAddrMsg *pMsg;
	char *pData, *pStart;
	char *serviceName = NULL, *serverAddress = NULL;
	char *hostAddress = NULL, *startOptions = NULL;
	char *authName = NULL, *authData = NULL;
	int len, authLen;
	extern char *display;
	
	CHECK_AT_LEAST_SIZE (iceConn, PMopcode, opcode,
	    length, SIZEOF (pmGetProxyAddrMsg), IceFatalToProtocol);

	IceReadCompleteMessage (iceConn, SIZEOF (pmGetProxyAddrMsg),
	    pmGetProxyAddrMsg, pMsg, pStart);

	if (!IceValidIO (iceConn))
	{
	    IceDisposeCompleteMessage (iceConn, pStart);
	    return;
	}

	authLen = swap ? lswaps (pMsg->authLen) : pMsg->authLen;

	pData = pStart;

	SKIP_STRING (pData, swap);	/* proxy-service */
	SKIP_STRING (pData, swap);	/* server-address */
	SKIP_STRING (pData, swap);	/* host-address */
	SKIP_STRING (pData, swap);	/* start-options */
	if (authLen > 0)
	{
	    SKIP_STRING (pData, swap);		    /* auth-name */
	    pData += (authLen +  PAD64 (authLen));  /* auth-data */
	}

	CHECK_COMPLETE_SIZE (iceConn, PMopcode, opcode,
	    length, pData - pStart + SIZEOF (pmGetProxyAddrMsg),
	    pStart, IceFatalToProtocol);

	pData = pStart;

	EXTRACT_STRING (pData, swap, serviceName);
	EXTRACT_STRING (pData, swap, serverAddress);
	EXTRACT_STRING (pData, swap, hostAddress);
	EXTRACT_STRING (pData, swap, startOptions);
	if (authLen > 0)
	{
	    EXTRACT_STRING (pData, swap, authName);
	    authData = (char *) malloc (authLen);
	    memcpy (authData, pData, authLen);
	}

	if (casecmp (serviceName, "LBX") != 0)
	{
	    SendGetProxyAddrReply (iceConn, PM_Unable,
		NULL, "Incorrect proxy service, should be LBX");
	}
	else
	{
	    int status;
	    char *addr;
	    char *error = NULL;

	    if (!gotFirstGetProxyAddr)
	    {
		char port[6];

		gotFirstGetProxyAddr = 1;
		display_name = serverAddress;
		if (authLen > 0)
		    XSetAuthorization (authName, strlen (authName),
		        authData, authLen);
		_IceTransGetHostname (proxyAddress,
				      sizeof (proxyAddress) - 6);
		sprintf (port, ":%s", display);
		strcat (proxyAddress, port);
		/* don't reply now; wait for ConnectToServer() */
	    }
	    else
	    {
		if (casecmp (serverAddress, display_name) == 0)
		{
		    status = PM_Success;
		    addr = proxyAddress;
		}
		else
		{
		    status = PM_Unable;
		    addr = NULL;
		    error = "lbxproxy can not service more than one display";
		}

		SendGetProxyAddrReply (iceConn, status, addr, error);
	    }
	}

	IceDisposeCompleteMessage (iceConn, pStart);

	if (serviceName)
	    free (serviceName);
	if (hostAddress)
	    free (hostAddress);
	if (startOptions)
	    free (startOptions);
	if (authName)
	    free (authName);
	if (authData)
	    free (authData);

	break;
    }

    default:
    {
	_IceErrorBadMinor (iceConn, PMopcode, opcode, IceCanContinue);
	_IceReadSkip (iceConn, length << 3);
	break;
    }
    }
}


HandleProxyManagerConnection ()

{
    IceProcessMessagesStatus	status;

    status = IceProcessMessages (PM_iceConn, NULL, NULL);

    if (status == IceProcessMessagesIOError)
    {
	fprintf (stderr, "IO error occured on proxy management connection\n");
    }
}

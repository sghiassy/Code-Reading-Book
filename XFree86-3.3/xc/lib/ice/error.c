/* $XConsortium: error.c /main/16 1995/09/15 13:54:01 mor $ */
/******************************************************************************


Copyright (c) 1993  X Consortium

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

Author: Ralph Mor, X Consortium
******************************************************************************/

#include <X11/ICE/ICElib.h>
#include "ICElibint.h"
#include <stdio.h>

#include <errno.h>

#ifdef X_NOT_STDC_ENV
extern int errno;
#endif


void
_IceErrorBadMinor (iceConn, majorOpcode, offendingMinor, severity)

IceConn	iceConn;
int	majorOpcode;
int	offendingMinor;
int	severity;

{
    IceErrorHeader (iceConn,
	majorOpcode, offendingMinor,
	iceConn->receive_sequence,
	severity,
	IceBadMinor,
	0);

    IceFlush (iceConn);
}


void
_IceErrorBadState (iceConn, majorOpcode, offendingMinor, severity)

IceConn	iceConn;
int	majorOpcode;
int	offendingMinor;
int	severity;

{
    IceErrorHeader (iceConn,
	majorOpcode, offendingMinor,
	iceConn->receive_sequence,
	severity,
	IceBadState,
	0);

    IceFlush (iceConn);
}


void
_IceErrorBadLength (iceConn, majorOpcode, offendingMinor, severity)

IceConn	iceConn;
int	majorOpcode;
int	offendingMinor;
int	severity;

{
    IceErrorHeader (iceConn,
	majorOpcode, offendingMinor,
	iceConn->receive_sequence,
	severity,
	IceBadLength,
	0);

    IceFlush (iceConn);
}


void
_IceErrorBadValue (iceConn, majorOpcode, offendingMinor, offset, length, value)

IceConn		iceConn;
int		majorOpcode;
int		offendingMinor;
int		offset;
int		length;		/* in bytes */
IcePointer	value;

{
    IceErrorHeader (iceConn,
	majorOpcode, offendingMinor,
	iceConn->receive_sequence,
	IceCanContinue,
	IceBadValue,
	WORD64COUNT (8 + length));

    IceWriteData32 (iceConn, 4, &offset);
    IceWriteData32 (iceConn, 4, &length);
    IceWriteData (iceConn, length, (char *) value);

    if (PAD64 (length))
	IceWritePad (iceConn, PAD64 (length));
    
    IceFlush (iceConn);
}


void
_IceErrorNoAuthentication (iceConn, offendingMinor)

IceConn	iceConn;
int	offendingMinor;

{
    int severity = (offendingMinor == ICE_ConnectionSetup) ?
	IceFatalToConnection : IceFatalToProtocol;

    IceErrorHeader (iceConn,
	0, offendingMinor,
	iceConn->receive_sequence,
	severity,
	IceNoAuth,
	0);

    IceFlush (iceConn);
}


void
_IceErrorNoVersion (iceConn, offendingMinor)

IceConn	iceConn;
int	offendingMinor;

{
    int severity = (offendingMinor == ICE_ConnectionSetup) ?
	IceFatalToConnection : IceFatalToProtocol;

    IceErrorHeader (iceConn,
	0, offendingMinor,
	iceConn->receive_sequence,
	severity,
	IceNoVersion,
	0);

    IceFlush (iceConn);
}


void
_IceErrorSetupFailed (iceConn, offendingMinor, reason)

IceConn	iceConn;
int	offendingMinor;
char	*reason;

{
    char *pBuf, *pStart;
    int bytes;
    int severity = (offendingMinor == ICE_ConnectionSetup) ?
	IceFatalToConnection : IceFatalToProtocol;

    if (!reason)
	reason = "";
    bytes = STRING_BYTES (reason);

    IceErrorHeader (iceConn,
	0, offendingMinor,
	iceConn->receive_sequence,
	severity,
	IceSetupFailed,
	WORD64COUNT (bytes));

    pBuf = pStart = IceAllocScratch (iceConn, PADDED_BYTES64 (bytes));
    STORE_STRING (pBuf, reason);

    IceWriteData (iceConn, PADDED_BYTES64 (bytes), pStart);
    IceFlush (iceConn);
}


void
_IceErrorAuthenticationRejected (iceConn, offendingMinor, reason)

IceConn	iceConn;
int	offendingMinor;
char	*reason;

{
    char *pBuf, *pStart;
    int bytes;

    if (!reason)
	reason = "";
    bytes = STRING_BYTES (reason);

    IceErrorHeader (iceConn,
	0, offendingMinor,
	iceConn->receive_sequence,
	IceFatalToProtocol,
	IceAuthRejected,
	WORD64COUNT (bytes));

    pBuf = pStart = IceAllocScratch (iceConn, PADDED_BYTES64 (bytes));
    STORE_STRING (pBuf, reason);

    IceWriteData (iceConn, PADDED_BYTES64 (bytes), pStart);
    IceFlush (iceConn);
}


void
_IceErrorAuthenticationFailed (iceConn, offendingMinor, reason)

IceConn	iceConn;
int	offendingMinor;
char	*reason;

{
    char *pBuf, *pStart;
    int bytes;

    if (!reason)
	reason = "";
    bytes = STRING_BYTES (reason);

    IceErrorHeader (iceConn,
	0, offendingMinor,
	iceConn->receive_sequence,
	IceFatalToProtocol,
	IceAuthFailed,
	WORD64COUNT (bytes));

    pBuf = pStart = IceAllocScratch (iceConn, PADDED_BYTES64 (bytes));
    STORE_STRING (pBuf, reason);

    IceWriteData (iceConn, PADDED_BYTES64 (bytes), pStart);
    IceFlush (iceConn);
}


void
_IceErrorProtocolDuplicate (iceConn, protocolName)

IceConn	iceConn;
char	*protocolName;

{
    char *pBuf, *pStart;
    int bytes;

    if (!protocolName)
	protocolName = "";
    bytes = STRING_BYTES (protocolName);

    IceErrorHeader (iceConn,
	0, ICE_ProtocolSetup,
	iceConn->receive_sequence,
	IceFatalToProtocol,
	IceProtocolDuplicate,
	WORD64COUNT (bytes));

    pBuf = pStart = IceAllocScratch (iceConn, PADDED_BYTES64 (bytes));
    STORE_STRING (pBuf, protocolName);

    IceWriteData (iceConn, PADDED_BYTES64 (bytes), pStart);
    IceFlush (iceConn);
}


void
_IceErrorMajorOpcodeDuplicate (iceConn, majorOpcode)

IceConn	iceConn;
int	majorOpcode;

{
    char mOp = (char) majorOpcode;

    IceErrorHeader (iceConn,
	0, ICE_ProtocolSetup,
	iceConn->receive_sequence,
	IceFatalToProtocol,
	IceMajorOpcodeDuplicate,
	1 /* length */);

    IceWriteData (iceConn, 8, &mOp);
    IceFlush (iceConn);
}


void
_IceErrorUnknownProtocol (iceConn, protocolName)

IceConn	iceConn;
char	*protocolName;

{
    char *pBuf, *pStart;
    int bytes;

    if (!protocolName)
	protocolName = "";
    bytes = STRING_BYTES (protocolName);

    IceErrorHeader (iceConn,
	0, ICE_ProtocolSetup,
	iceConn->receive_sequence,
	IceFatalToProtocol,
	IceUnknownProtocol,
	WORD64COUNT (bytes));

    pBuf = pStart = IceAllocScratch (iceConn, PADDED_BYTES64 (bytes));
    STORE_STRING (pBuf, protocolName);

    IceWriteData (iceConn, PADDED_BYTES64 (bytes), pStart);
    IceFlush (iceConn);
}


void
_IceErrorBadMajor (iceConn, offendingMajor, offendingMinor, severity)

IceConn	iceConn;
int     offendingMajor;
int     offendingMinor;
int	severity;

{
    char maj = (char) offendingMajor;

    IceErrorHeader (iceConn,
	0, offendingMinor,
	iceConn->receive_sequence,
	severity,
	IceBadMajor,
	1 /* length */);

    IceWriteData (iceConn, 8, &maj);
    IceFlush (iceConn);
}



/*
 * Default error handler.
 */

void
_IceDefaultErrorHandler (iceConn, swap,
    offendingMinorOpcode, offendingSequence, errorClass, severity, values)

IceConn		iceConn;
Bool		swap;
int		offendingMinorOpcode;
unsigned long	offendingSequence;
int 		errorClass;
int		severity;
IcePointer	values;

{
    char *str;
    char *pData = (char *) values;

    switch (offendingMinorOpcode)
    {
        case ICE_ConnectionSetup:
            str = "ConnectionSetup";
	    break;
        case ICE_AuthRequired:
            str = "AuthRequired";
	    break;
        case ICE_AuthReply:
            str = "AuthReply";
	    break;
        case ICE_AuthNextPhase:
            str = "AuthNextPhase";
	    break;
        case ICE_ConnectionReply:
            str = "ConnectionReply";
	    break;
        case ICE_ProtocolSetup:
            str = "ProtocolSetup";
	    break;
        case ICE_ProtocolReply:
            str = "ProtocolReply";
	    break;
        case ICE_Ping:
            str = "Ping";
	    break;
        case ICE_PingReply:
            str = "PingReply";
	    break;
        case ICE_WantToClose:
            str = "WantToClose";
	    break;
        case ICE_NoClose:
            str = "NoClose";
	    break;
	default:
	    str = "";
	}

    fprintf (stderr, "\n");

    fprintf (stderr, "ICE error:  Offending minor opcode    = %d (%s)\n",
	offendingMinorOpcode, str);

    fprintf (stderr, "            Offending sequence number = %d\n",
	offendingSequence);

    switch (errorClass)
    {
        case IceBadMinor:
            str = "BadMinor";
            break;
        case IceBadState:
            str = "BadState";
            break;
        case IceBadLength:
            str = "BadLength";
            break;
        case IceBadValue:
            str = "BadValue";
            break;
        case IceBadMajor:
            str = "BadMajor";
            break;
        case IceNoAuth:
            str = "NoAuthentication";
            break;
        case IceNoVersion:
            str = "NoVersion";
            break;
        case IceSetupFailed:
            str = "SetupFailed";
            break;
        case IceAuthRejected:
            str = "AuthenticationRejected";
            break;
        case IceAuthFailed:
            str = "AuthenticationFailed";
            break;
        case IceProtocolDuplicate:
            str = "ProtocolDuplicate";
            break;
        case IceMajorOpcodeDuplicate:
            str = "MajorOpcodeDuplicate";
            break;
        case IceUnknownProtocol:
            str = "UnknownProtocol";
            break;
	default:
	    str = "???";
    }

    fprintf (stderr, "            Error class               = %s\n", str);

    if (severity == IceCanContinue)
	str = "CanContinue";
    else if (severity == IceFatalToProtocol)
	str = "FatalToProtocol";
    else if (severity == IceFatalToConnection)
	str = "FatalToConnection";
    else
	str = "???";

    fprintf (stderr, "            Severity                  = %s\n", str);

    switch (errorClass)
    {
        case IceBadValue:
        {
	    int offset, length, val;

	    EXTRACT_CARD32 (pData, swap, offset);
	    EXTRACT_CARD32 (pData, swap, length);

	    fprintf (stderr,
		"            BadValue Offset           = %d\n", offset);
	    fprintf (stderr,
		"            BadValue Length           = %d\n", length);

	    if (length <= 4)
	    {
		if (length == 1)
		    val = (int) *pData;
		else if (length == 2)
		{
		    EXTRACT_CARD16 (pData, swap, val);
		}
		else
		{
		    EXTRACT_CARD32 (pData, swap, val);
		}

		fprintf (stderr,
	            "            BadValue                  = %d\n", val);
	    }
            break;
	}

        case IceBadMajor:

	    fprintf (stderr, "Major opcode : %d\n", (int) *pData);
            break;

        case IceSetupFailed:

	    EXTRACT_STRING (pData, swap, str);
	    fprintf (stderr, "Reason : %s\n", str);
            break;

        case IceAuthRejected:

	    EXTRACT_STRING (pData, swap, str);
	    fprintf (stderr, "Reason : %s\n", str);
            break;

        case IceAuthFailed:

	    EXTRACT_STRING (pData, swap, str);
	    fprintf (stderr, "Reason : %s\n", str);
            break;

        case IceProtocolDuplicate:

	    EXTRACT_STRING (pData, swap, str);
	    fprintf (stderr, "Protocol name : %s\n", str);
            break;

        case IceMajorOpcodeDuplicate:

	    fprintf (stderr, "Major opcode : %d\n", (int) *pData);
            break;

        case IceUnknownProtocol:

	    EXTRACT_STRING (pData, swap, str);
	    fprintf (stderr, "Protocol name : %s\n", str);
            break;

	default:
	    break;
    }

    fprintf (stderr, "\n");

    if (severity != IceCanContinue)
	exit (1);
}



/* 
 * This procedure sets the ICE error handler to be the specified
 * routine.  If NULL is passed in the default error handler is restored.
 * The function's return value is the previous error handler.
 */
 
IceErrorHandler
IceSetErrorHandler (handler)

IceErrorHandler handler;

{
    IceErrorHandler oldHandler = _IceErrorHandler;

    if (handler != NULL)
	_IceErrorHandler = handler;
    else
	_IceErrorHandler = _IceDefaultErrorHandler;

    return (oldHandler);
}



/*
 * Default IO error handler.
 */

void
_IceDefaultIOErrorHandler (iceConn)

IceConn iceConn;

{
    fprintf (stderr,
	"ICE default IO error handler doing an exit(), pid = %d, errno = %d\n",
	getpid(), errno);

    exit (1);
}



/* 
 * This procedure sets the ICE fatal I/O error handler to be the
 * specified routine.  If NULL is passed in the default error
 * handler is restored.   The function's return value is the
 * previous error handler.
 */
 
IceIOErrorHandler
IceSetIOErrorHandler (handler)

IceIOErrorHandler handler;

{
    IceIOErrorHandler oldHandler = _IceIOErrorHandler;

    if (handler != NULL)
	_IceIOErrorHandler = handler;
    else
	_IceIOErrorHandler = _IceDefaultIOErrorHandler;

    return (oldHandler);
}

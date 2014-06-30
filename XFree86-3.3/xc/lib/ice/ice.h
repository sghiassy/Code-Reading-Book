/* $XConsortium: ICE.h,v 1.6 94/04/17 20:15:23 mor Exp $ */
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

#ifndef _ICE_H_
#define _ICE_H_

/*
 * Protocol Version
 */

#define IceProtoMajor 1
#define IceProtoMinor 0


/*
 * Byte Order
 */

#define IceLSBfirst		0
#define IceMSBfirst		1


/*
 * ICE minor opcodes
 */

#define ICE_Error 		0
#define ICE_ByteOrder		1
#define ICE_ConnectionSetup	2
#define ICE_AuthRequired	3
#define ICE_AuthReply 		4
#define ICE_AuthNextPhase	5
#define ICE_ConnectionReply	6
#define ICE_ProtocolSetup	7
#define ICE_ProtocolReply	8
#define ICE_Ping		9
#define ICE_PingReply		10
#define ICE_WantToClose		11
#define ICE_NoClose		12


/*
 * Error severity
 */

#define IceCanContinue		0
#define IceFatalToProtocol	1
#define IceFatalToConnection	2


/*
 * ICE error classes that are common to all protocols
 */

#define IceBadMinor	0x8000
#define IceBadState	0x8001
#define IceBadLength	0x8002
#define IceBadValue	0x8003


/*
 * ICE error classes that are specific to the ICE protocol
 */

#define IceBadMajor			0
#define IceNoAuth			1
#define IceNoVersion			2
#define IceSetupFailed			3
#define IceAuthRejected			4
#define IceAuthFailed			5
#define IceProtocolDuplicate		6
#define IceMajorOpcodeDuplicate		7
#define IceUnknownProtocol		8

#endif /* _ICE_H_ */

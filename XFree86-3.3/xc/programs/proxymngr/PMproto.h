/* $XConsortium: PMproto.h /main/1 1996/11/30 23:57:48 swick $ */

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

/*		Proxy Management Protocol		*/

#ifndef _PMPROTO_H_
#define _PMPROTO_H_

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;	/* == 1 */
    CARD16	authLen B16;
    CARD32	length B32;
    /* STRING	   proxy-service */
    /* STRING	   server-address */
    /* STRING	   host-address */
    /* STRING	   start-options */
    /* STRING	   auth-name (if authLen > 0) */
    /* LISTofCARD8 auth-data (if authLen > 0) */
} pmGetProxyAddrMsg;

#define sz_pmGetProxyAddrMsg 8


typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;	/* == 2 */
    CARD8	status;
    CARD8	unused;
    CARD32	length B32;
    /* STRING	proxy-address */
    /* STRING	failure-reason */
} pmGetProxyAddrReplyMsg;

#define sz_pmGetProxyAddrReplyMsg 8


typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;	/* == 3 */
    CARD16	unused B16;
    CARD32	length B32;
    /* STRING	  proxy-service */
} pmStartProxyMsg;

#define sz_pmStartProxyMsg 8


#endif /* _PMPROTO_H_ */

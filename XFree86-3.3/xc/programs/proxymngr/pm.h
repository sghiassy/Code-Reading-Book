/* $XConsortium: PM.h /main/1 1996/11/30 23:57:44 swick $ */

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

#ifndef _PM_H_
#define _PM_H_

#define PM_PROTOCOL_NAME "PROXY_MANAGEMENT"

#define PM_MAJOR_VERSION 1
#define PM_MINOR_VERSION 0

/*
 * PM minor opcodes
 */
#define PM_Error		ICE_Error /* == 0 */
#define PM_GetProxyAddr		1
#define PM_GetProxyAddrReply	2
#define PM_StartProxy		3

/*
 * status return codes for GetProxyAddrReply
 */
#define		   PM_Unable	0
#define		   PM_Success	1
#define            PM_Failure	2

#endif /* _PM_H_ */

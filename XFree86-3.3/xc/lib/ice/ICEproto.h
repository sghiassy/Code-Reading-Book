/* $XConsortium: ICEproto.h,v 1.7 94/04/17 20:15:27 mor Exp $ */
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

#ifndef _ICEPROTO_H_
#define _ICEPROTO_H_

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	data[2];
    CARD32	length B32;
} iceMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD16	errorClass B16;
    CARD32	length B32;
    CARD8	offendingMinorOpcode;
    CARD8	severity;
    CARD16	unused B16;
    CARD32	offendingSequenceNum B32;
    /* n	varying values */
    /* p	p = pad (n, 8) */
} iceErrorMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	byteOrder;
    CARD8	unused;
    CARD32	length B32;
} iceByteOrderMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	versionCount;
    CARD8	authCount;
    CARD32	length B32;
    CARD8	mustAuthenticate;
    CARD8	unused[7];
    /* i	STRING		vendor */
    /* j	STRING		release */
    /* k	LIST of STRING	authentication-protocol-names */
    /* m	LIST of VERSION version-list */
    /* p	p = pad (i+j+k+m, 8) */
} iceConnectionSetupMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	authIndex;
    CARD8	unused1;
    CARD32	length B32;
    CARD16	authDataLength B16;
    CARD8	unused2[6];
    /* n	varying data */
    /* p	p = pad (n, 8) */
} iceAuthRequiredMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused1[2];
    CARD32	length B32;
    CARD16	authDataLength B16;
    CARD8	unused2[6];
    /* n	varying data */
    /* p	p = pad (n, 8) */
} iceAuthReplyMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused1[2];
    CARD32	length B32;
    CARD16	authDataLength B16;
    CARD8	unused2[6];
    /* n	varying data */
    /* p	p = pad (n, 8) */
} iceAuthNextPhaseMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	versionIndex;
    CARD8	unused;
    CARD32	length B32;
    /* i	STRING		vendor */
    /* j	STRING		release */
    /* p	p = pad (i+j, 8) */
} iceConnectionReplyMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	protocolOpcode;
    CARD8	mustAuthenticate;
    CARD32	length B32;
    CARD8	versionCount;
    CARD8	authCount;
    CARD8	unused[6];
    /* i	STRING		protocol-name */
    /* j	STRING		vendor */
    /* k	STRING		release */
    /* m	LIST of STRING	authentication-protocol-names */
    /* n	LIST of VERSION version-list */
    /* p        p = pad (i+j+k+m+n, 8) */
} iceProtocolSetupMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	versionIndex;
    CARD8	protocolOpcode;
    CARD32	length B32;
    /* i	STRING		vendor */
    /* j	STRING		release */
    /* p	p = pad (i+j, 8) */
} iceProtocolReplyMsg;

typedef iceMsg  icePingMsg;
typedef iceMsg  icePingReplyMsg;
typedef iceMsg  iceWantToCloseMsg;
typedef iceMsg  iceNoCloseMsg;


/*
 * SIZEOF values.  These better be multiples of 8.
 */

#define sz_iceMsg			8
#define sz_iceErrorMsg			16
#define sz_iceByteOrderMsg		8
#define sz_iceConnectionSetupMsg        16
#define sz_iceAuthRequiredMsg		16
#define sz_iceAuthReplyMsg		16
#define sz_iceAuthNextPhaseMsg		16
#define sz_iceConnectionReplyMsg	8
#define sz_iceProtocolSetupMsg		16
#define sz_iceProtocolReplyMsg		8
#define sz_icePingMsg			8
#define sz_icePingReplyMsg		8
#define sz_iceWantToCloseMsg		8
#define sz_iceNoCloseMsg		8

#endif /* _ICEPROTO_H_ */

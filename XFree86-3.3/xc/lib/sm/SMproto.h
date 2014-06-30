/* $XConsortium: SMproto.h,v 1.9 94/04/17 20:16:52 mor Exp $ */

/*

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

*/

/*
 * Author: Ralph Mor, X Consortium
 */

#ifndef _SMPROTO_H_
#define _SMPROTO_H_

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
    /* n	ARRAY8		previousId */
} smRegisterClientMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
    /* n	ARRAY8		clientId */
} smRegisterClientReplyMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused1[2];
    CARD32	length B32;
    CARD8	saveType;
    CARD8	shutdown;
    CARD8	interactStyle;
    CARD8	fast;
    CARD8	unused2[4];
} smSaveYourselfMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused1[2];
    CARD32	length B32;
    CARD8	saveType;
    CARD8	shutdown;
    CARD8	interactStyle;
    CARD8	fast;
    CARD8	global;
    CARD8	unused2[3];
} smSaveYourselfRequestMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	dialogType;
    CARD8	unused;
    CARD32	length B32;
} smInteractRequestMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
} smInteractMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	cancelShutdown;
    CARD8	unused;
    CARD32	length B32;
} smInteractDoneMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8       success;
    CARD8	unused;
    CARD32	length B32;
} smSaveYourselfDoneMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
} smDieMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
} smShutdownCancelledMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
    /* b	LISTofARRAY8	reasons */
} smCloseConnectionMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
    /* a	LISTofPROPERTY	properties */
} smSetPropertiesMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
    /* a	LISTofARRAY8	property names */
} smDeletePropertiesMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
} smGetPropertiesMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
    /* a	LISTofPROPERTY	properties */
} smPropertiesReplyMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
} smSaveYourselfPhase2RequestMsg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
} smSaveYourselfPhase2Msg;

typedef struct {
    CARD8	majorOpcode;
    CARD8	minorOpcode;
    CARD8	unused[2];
    CARD32	length B32;
} smSaveCompleteMsg;


/*
 * SIZEOF values.  These better be multiples of 8.
 */

#define sz_smRegisterClientMsg 			8
#define sz_smRegisterClientReplyMsg 		8
#define sz_smSaveYourselfMsg 			16
#define sz_smSaveYourselfRequestMsg		16
#define sz_smInteractRequestMsg 		8
#define sz_smInteractMsg 			8
#define sz_smInteractDoneMsg 			8
#define sz_smSaveYourselfDoneMsg 		8
#define sz_smDieMsg 				8
#define sz_smShutdownCancelledMsg 		8
#define sz_smCloseConnectionMsg 		8
#define sz_smSetPropertiesMsg 			8
#define sz_smDeletePropertiesMsg 		8
#define sz_smGetPropertiesMsg 			8
#define sz_smPropertiesReplyMsg 		8
#define sz_smSaveYourselfPhase2RequestMsg	8
#define sz_smSaveYourselfPhase2Msg 		8
#define sz_smSaveCompleteMsg 			8

#endif /* _SMPROTO_H_ */

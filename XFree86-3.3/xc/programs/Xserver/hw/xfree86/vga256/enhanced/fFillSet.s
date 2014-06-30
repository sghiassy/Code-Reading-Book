/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/enhanced/fFillSet.s,v 3.5 1996/12/23 06:59:03 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 * This file was derived from a similar X11R4 file (X386 1.1) for X11R5
 * (X386 1.2) by James Tsillas.  This file was further derived to its current
 * form by David Wexelblat (dwex@goblin.org).
 *
 */
/* $XConsortium: fFillSet.s /main/5 1996/02/21 18:08:46 kaleb $ */

#include "assyntax.h"

	FILE("fFillSet.s")
	AS_BEGIN

/*
 *
 * This routine implements a fast Solid Fill in GXset mode.
 * no segment checking is done.
 *
 * SYNTAX:
 * unchar * fastFillSolidGXset(pdst,fill1,fill2,hcount,count,width,widthPitch);
 * 
 *  (7/27/90 TR)
 *  (4/92 JT)
 *  (5/92 DW)
 */

#define pdst        EBX
#define lp          ECX
#define count       EDX
#define hcount      EDI
#define width       REGOFF(28,EBP)
#define widthPitch  ESI
#define tmp         EAX
#define tmpw        AX
#define tmpb	    AL
#define fill1       REGOFF(12,EBP)
#define fill2       REGOFF(16,EBP)

	SEG_TEXT
	ALIGNTEXT4
GLOBL	GLNAME(fastFillSolidGXset)

GLNAME(fastFillSolidGXset):
	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	PUSH_L	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)
	MOV_L	(REGOFF(8,EBP),pdst)
	MOV_L	(REGOFF(20,EBP),hcount)
	MOV_L	(REGOFF(24,EBP),count)
	MOV_L	(REGOFF(32,EBP),widthPitch)
	OR_L	(hcount,hcount)
	JZ	(.finish)
	OR_L	(count,count)
	JZ	(.finish)
	CMP_L	(CONST(3),count)
	JG	(.blockloop)
	JE	(.tribbleloop)
	CMP_L	(CONST(2),count)
	JE	(.wordloop)
/*
 * do a fast vertical line
 */
	ALIGNTEXT4ifNOP
.byteloop:
	MOV_B	(fill1,tmpb)
	AND_B	(REGIND(pdst),tmpb)
	XOR_B	(fill2,tmpb)
	MOV_B	(tmpb,REGIND(pdst))
	LEA_L	(REGBID(widthPitch,pdst,1),pdst)
	DEC_L	(hcount)
	JNZ	(.byteloop)
	JMP	(.finish)

	ALIGNTEXT4
.wordloop:
	MOV_W	(fill1,tmpw)
	AND_W	(REGIND(pdst),tmpw)
	XOR_W	(fill2,tmpw)
	MOV_W	(tmpw,REGIND(pdst))
	LEA_L	(REGBID(widthPitch,pdst,2),pdst)
	DEC_L	(hcount)
	JNZ	(.wordloop)
	JMP	(.finish)

	ALIGNTEXT4
.tribbleloop:
	MOV_W	(fill1,tmpw)
	AND_W	(REGIND(pdst),tmpw)
	XOR_W	(fill2,tmpw)
	MOV_W	(tmpw,REGIND(pdst))
	MOV_B	(fill1,tmpb)
	AND_B	(REGOFF(2,pdst),tmpb)
	XOR_B	(fill2,tmpb)
	MOV_B	(tmpb,REGOFF(2,pdst))
	LEA_L	(REGBID(widthPitch,pdst,3),pdst)
	DEC_L	(hcount)
	JNZ	(.tribbleloop)
	JMP	(.finish)

	ALIGNTEXT4
.blockloop:
	TEST_L	(CONST(1),pdst)
	JZ	(.alignword)
	MOV_B	(fill1,tmpb)
	AND_B	(REGIND(pdst),tmpb)
	XOR_B	(fill2,tmpb)
	MOV_B	(tmpb,REGIND(pdst))
	INC_L	(pdst)
	DEC_L	(count)
.alignword:
	TEST_L	(CONST(2),pdst)
	JZ	(.aligneddword)
	MOV_W	(fill1,tmpw)
	AND_W	(REGIND(pdst),tmpw)
	XOR_W	(fill2,tmpw)
	MOV_W	(tmpw,REGIND(pdst))
	LEA_L	(REGOFF(2,pdst),pdst)
	LEA_L	(REGOFF(-2,count),count)
.aligneddword:
	MOV_L	(count,lp)
	SHR_L	(CONST(5),lp)
	JZ	(.fixupdword)

	ALIGNTEXT4ifNOP
.dwordloop:
	MOV_L	(REGOFF(0,pdst),tmp)
	AND_L	(fill1,tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(0,pdst))
	MOV_L	(REGOFF(4,pdst),tmp)
	AND_L	(fill1,tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(4,pdst))
	MOV_L	(REGOFF(8,pdst),tmp)
	AND_L	(fill1,tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(8,pdst))
	MOV_L	(REGOFF(12,pdst),tmp)
	AND_L	(fill1,tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(12,pdst))
	MOV_L	(REGOFF(16,pdst),tmp)
	AND_L	(fill1,tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(16,pdst))
	MOV_L	(REGOFF(20,pdst),tmp)
	AND_L	(fill1,tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(20,pdst))
	MOV_L	(REGOFF(24,pdst),tmp)
	AND_L	(fill1,tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(24,pdst))
	MOV_L	(REGOFF(28,pdst),tmp)
	AND_L	(fill1,tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(28,pdst))
	LEA_L	(REGOFF(32,pdst),pdst)
	DEC_L	(lp)
	JNZ	(.dwordloop)

.fixupdword:
	MOV_L	(count,lp)
	AND_L	(CONST(28),lp)
	LEA_L	(REGBI(lp,pdst),pdst)
	MOV_L	(REGDB(.jumptab1,lp),tmp)
	JMP	(CODEPTR(tmp))

	SEG_DATA
	ALIGNDATA4
.jumptab1:
	D_LONG	.Lnoop, .L0, .L1, .L2, .L3, .L4, .L5, .L6

	SEG_TEXT
	ALIGNTEXT4
.L6:	MOV_L	(fill1,tmp)
	AND_L	(REGOFF(-28,pdst),tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(-28,pdst))
.L5:	MOV_L	(fill1,tmp)
	AND_L	(REGOFF(-24,pdst),tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(-24,pdst))
.L4:	MOV_L	(fill1,tmp)
	AND_L	(REGOFF(-20,pdst),tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(-20,pdst))
.L3:	MOV_L	(fill1,tmp)
	AND_L	(REGOFF(-16,pdst),tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(-16,pdst))
.L2:	MOV_L	(fill1,tmp)
	AND_L	(REGOFF(-12,pdst),tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(-12,pdst))
.L1:	MOV_L	(fill1,tmp)
	AND_L	(REGOFF(-8,pdst),tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(-8,pdst))
.L0:	MOV_L	(fill1,tmp)
	AND_L	(REGOFF(-4,pdst),tmp)
	XOR_L	(fill2,tmp)
	MOV_L	(tmp,REGOFF(-4,pdst))
.Lnoop:

	TEST_L	(CONST(2),count)
	JZ	(.fixupbyte)
	MOV_W	(fill1,tmpw)
	AND_W	(REGIND(pdst),tmpw)
	XOR_W	(fill2,tmpw)
	MOV_W	(tmpw,REGIND(pdst))
	LEA_L	(REGOFF(2,pdst),pdst)
.fixupbyte:
	TEST_L	(CONST(1),count)
	JZ	(.enditeration)
	MOV_B	(fill1,tmpb)
	AND_B	(REGIND(pdst),tmpb)
	XOR_B	(fill2,tmpb)
	MOV_B	(tmpb,REGIND(pdst))
	INC_L	(pdst)

.enditeration:
	LEA_L	(REGBI(widthPitch,pdst),pdst)
	MOV_L	(width,count)
	DEC_L	(hcount)
	JNZ	(.blockloop)

.finish:
	MOV_L	(pdst,EAX)
	LEA_L	(REGOFF(-12,EBP),ESP)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET


/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/enhanced/fLineBres.s,v 3.7 1996/12/23 06:59:04 dawes Exp $ */
/* Copyright 1992 by James Tsillas, Arlington, Massachusetts.

		All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation.

JAMES TSILLAS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
*/
/* $XConsortium: fLineBres.s /main/5 1996/02/21 18:08:50 kaleb $ */

#include "assyntax.h"
#include "vgaAsm.h"

	FILE("fLineBres.s")
	AS_BEGIN

#define rop	REGOFF(8,EBP)
#define andv	DH
#define xorv	DL
#define addrl   REGOFF(20,EBP)
#define nlwidth REGOFF(24,EBP)
#define signdx	REGOFF(28,EBP)
#define signdy	REGOFF(32,EBP)
#define axis	REGOFF(36,EBP)
#define x1	REGOFF(40,EBP)
#define y1	REGOFF(44,EBP)
#define e	EDI
#define e1	ESI
#define e2	REGOFF(56,EBP)
#define len     ECX

#define addrb   EBX
#define tmp	EAX

#define GXcopy	CONST(3)
#define Y_AXIS	CONST(1)

	SEG_DATA
	ALIGNDATA4
e3:
	D_LONG 0

	SEG_TEXT
	ALIGNTEXT4
GLOBL	GLNAME(fastvga256BresS)

GLNAME(fastvga256BresS):
	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	PUSH_L	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)
	MOV_L	(REGOFF(60,EBP),len)
	CMP_L	(CONST(0),len)		/* check for zero length first */
	JZ	(.allfinish)
	MOV_B	(REGOFF(12,EBP),andv)
	MOV_B	(REGOFF(16,EBP),xorv)
	MOV_L	(REGOFF(48,EBP),e)
	MOV_L	(REGOFF(52,EBP),e1)
	MOV_L	(e2,tmp)
	SUB_L	(e1,tmp)
	MOV_L	(tmp,CONTENT(e3))
	SHL_L	(CONST(2),nlwidth)
	MOV_L	(nlwidth,tmp)
	IMUL_L	(y1,tmp)
	ADD_L	(x1,tmp)
	ADD_L	(addrl,tmp)
	MOV_L	(tmp,addrb)
	CMP_L	(CONST(0),signdy)
	JGE	(.L1)
	NEG_L	(nlwidth)
.L1:	SUB_L	(e1,e)
	CMP_L	(Y_AXIS,axis)
	JNZ	(.L2)
	PUSH_L	(nlwidth)
	PUSH_L	(signdx)
	POP_L	(nlwidth)
	POP_L	(signdx)
.L2:	CMP_L	(GXcopy,rop)
	JNZ	(.LSet)

.LCopy:	CMP_L	(VGABASE,addrl)
	JB	(.nocheckloopC)
	PUSH_L	(addrb)
	CALL	(GLNAME(vgaSetWrite))
	MOV_L	(tmp,addrb)
	ADD_L	(CONST(4),ESP)

	ALIGNTEXT4ifNOP
.writeloopC:
	MOV_B	(xorv,REGIND(addrb))
	ADD_L	(e1,e)
	JS	(.L3)
	ADD_L	(nlwidth,addrb)
	ADD_L	(CONTENT(e3),e)
.L3:	ADD_L	(signdx,addrb)
	CMP_L	(CONTENT(GLNAME(vgaWriteBottom)),addrb)
	/* was JBE */
	JB	(.L4)
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)),addrb)
	JAE	(.L9)
	DEC_L	(ECX)
	JNZ	(.writeloopC)
	JMP	(.allfinish)
.L9:	PUSH_L	(addrb)
	CALL	(GLNAME(vgaWriteNext))
	MOV_L	(tmp,addrb)
	ADD_L	(CONST(4),ESP)
	DEC_L	(ECX)
	JNZ	(.writeloopC)
	JMP	(.allfinish)
.L4:	PUSH_L	(addrb)
	CALL	(GLNAME(vgaWritePrev))
	MOV_L	(tmp,addrb)
	ADD_L	(CONST(4),ESP)
	DEC_L	(ECX)
	JNZ	(.writeloopC)
	JMP	(.allfinish)

	ALIGNTEXT4
.nocheckloopC:
	MOV_B	(xorv,REGIND(addrb))
	ADD_L	(e1,e)
	JS	(.L5)
	ADD_L	(nlwidth,addrb)
	ADD_L	(CONTENT(e3),e)
.L5:	ADD_L	(signdx,addrb)
	DEC_L	(ECX)
	JNZ	(.nocheckloopC)

	ALIGNTEXT4ifNOP
.allfinish:
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET

.LSet:	CMP_L	(VGABASE,addrl)
	JB	(.nocheckloopS)
	PUSH_L	(addrb)
	CALL	(GLNAME(vgaSetReadWrite))
	MOV_L	(tmp,addrb)
	ADD_L	(CONST(4),ESP)

	ALIGNTEXT4ifNOP
.writeloopS:
	MOV_B	(REGIND(addrb),AL)	/* Minimize slow memory access */
	AND_B	(andv,AL)
	XOR_B	(xorv,AL)
	MOV_B	(AL,REGIND(addrb))
	ADD_L	(e1,e)
	JS	(.L6)
	ADD_L	(nlwidth,addrb)
	ADD_L	(CONTENT(e3),e)
.L6:	ADD_L	(signdx,addrb)
	CMP_L	(CONTENT(GLNAME(vgaWriteBottom)),addrb)
	/* was JBE */
	JB	(.L7)
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)),addrb)
	JAE	(.L10)
	DEC_L	(ECX)
	JNZ	(.writeloopS)
	JMP	(.allfinish)
.L10:	PUSH_L	(addrb)
	CALL	(GLNAME(vgaReadWriteNext))
	MOV_L	(tmp,addrb)
	ADD_L	(CONST(4),ESP)
	DEC_L	(ECX)
	JNZ	(.writeloopS)
	JMP	(.allfinish)
.L7:	PUSH_L	(addrb)
	CALL	(GLNAME(vgaReadWritePrev))
	MOV_L	(EAX,addrb)
	ADD_L	(CONST(4),ESP)
	DEC_L	(ECX)
	JNZ	(.writeloopS)
	JMP	(.allfinish)

	ALIGNTEXT4
.nocheckloopS:
	MOV_B	(REGIND(addrb),AL)	/* Minimize slow memory access */
	AND_B	(andv,AL)
	XOR_B	(xorv,AL)
	MOV_B	(AL,REGIND(addrb))
	ADD_L	(e1,e)
	JS	(.L8)
	ADD_L	(nlwidth,addrb)
	ADD_L	(CONTENT(e3),e)
.L8:	ADD_L	(signdx,addrb)
	DEC_L	(ECX)
	JNZ	(.nocheckloopS)
	JMP	(.allfinish)



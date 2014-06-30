/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/enhanced/fLineH.s,v 3.6 1996/12/23 06:59:05 dawes Exp $ */
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
/* $XConsortium: fLineH.s /main/5 1996/02/21 18:08:54 kaleb $ */

#include "assyntax.h"
#include "vgaAsm.h"

	FILE("fLineH.s")
	AS_BEGIN

#define rop		REGOFF(8,EBP)
#define andv		EBX
#define andvb		BL
#define	andvw		BX
#define xorv		EDX
#define xorvb		DL
#define	xorvw		DX
#define addrl		EDI
#define nlwidth		REGOFF(24,EBP)
#define	x1		REGOFF(28,EBP)
#define	y1		REGOFF(32,EBP)
#define	len		ESI

#define count		ECX
#define tmp		EAX

#define GXcopy  	CONST(3)
#define GXxor		CONST(6)

SEG_TEXT
	ALIGNTEXT4
GLOBL	GLNAME(fastvga256HorzS)

GLNAME(fastvga256HorzS):
	PUSH_L		(EBP)
	MOV_L		(ESP,EBP)
	PUSH_L		(ESI)
	PUSH_L		(EDI)
	PUSH_L		(EBX)
	MOV_L		(REGOFF(16,EBP),xorv)
	MOV_L		(REGOFF(20,EBP),addrl)
	MOV_L		(REGOFF(36,EBP),len)
	CLD
	MOV_L		(nlwidth,tmp)
	SHL_L		(CONST(2),tmp)
	IMUL_L		(y1,tmp)
	ADD_L		(x1,tmp)
	ADD_L		(tmp,addrl)
	CMP_L		(VGABASE,addrl)
	JB		(.noClongL)
	PUSH_L		(addrl)
	CALL		(GLNAME(vgaSetReadWrite))
	MOV_L		(tmp,addrl)
	ADD_L		(CONST(4),ESP)
	MOV_L		(CONTENT(GLNAME(vgaWriteTop)),tmp)
	SUB_L		(addrl,tmp)
	CMP_L		(len,tmp)
	JAE		(.noClongL)

.ClongL:
	CMP_L		(GXcopy,rop)
	JNE		(.XlongL)
	CMP_L		(CONST(2),len)
	JE		(.L14)
	JB		(.L15)
.L13:	TEST_L		(CONST(3),addrl)
	JZ		(.L8)
	MOV_B		(xorvb,REGIND(addrl))
	INC_L		(addrl)
	DEC_L		(len)
.L14:	TEST_L		(CONST(3),addrl)
	JZ		(.L8A)
	MOV_B		(xorvb,REGIND(addrl))
	INC_L		(addrl)
	DEC_L		(len)
.L15:	TEST_L		(CONST(3),addrl)
	JZ		(.L8A)
	MOV_B		(xorvb,REGIND(addrl))
	INC_L		(addrl)
	DEC_L		(len)
.L8A:	CMP_L		(CONTENT(GLNAME(vgaWriteTop)),addrl)
	JB		(.L8)
	PUSH_L		(addrl)
	CALL		(GLNAME(vgaWriteNext))
	MOV_L		(tmp,addrl)
	ADD_L		(CONST(4),ESP)
.L8:	MOV_L		(len,count)
	AND_L		(CONST(0xfffffffc),count)
	MOV_L		(CONTENT(GLNAME(vgaWriteTop)),tmp)
	SUB_L		(addrl,tmp)
	CMP_L		(tmp,count)
	JBE		(.L16)
	MOV_L		(tmp,count)
.L16:	SUB_L		(count,len)
	SHR_L		(CONST(2),count)
	JZ		(.L10)
	MOV_L		(xorv,tmp)
	REPZ
	STOS_L
	CMP_L		(CONST(3),len)
	JBE		(.L17)
	PUSH_L		(addrl)
	CALL		(GLNAME(vgaWriteNext))
	MOV_L		(tmp,addrl)
	ADD_L		(CONST(4),ESP)
	MOV_L		(len,count)
	AND_L		(CONST(0xfffffffc),count)
	SUB_L		(count,len)
	SHR_L		(CONST(2),count)
	MOV_L		(xorv,tmp)
	REPZ
	STOS_L
.L17:	CMP_L		(CONTENT(GLNAME(vgaWriteTop)),addrl)
	JB		(.L10)
	PUSH_L		(addrl)
	CALL		(GLNAME(vgaWriteNext))
	MOV_L		(tmp,addrl)
	ADD_L		(CONST(4),ESP)
.L10:	CMP_L		(CONST(2),len)
	JA		(.L11)
	JE		(.L12)
	CMP_L		(CONST(0),len)
	JE		(.allfinish)
	MOV_B		(xorvb,REGIND(addrl))
	JMP		(.allfinish)
.L11:	MOV_W		(xorvw,REGIND(addrl))
	MOV_B		(xorvb,REGOFF(2,addrl))		/**/
	JMP		(.allfinish)
.L12:	MOV_W		(xorvw,REGIND(addrl))		/**/
.allfinish:
	POP_L		(EBX)
	POP_L		(EDI)
	POP_L		(ESI)
	LEAVE
	RET

.XlongL:
	MOV_L		(REGOFF(12,EBP),andv)
	CMP_L		(GXxor,rop)
	JNE		(.SlongL)
	CMP_L		(CONST(2),len)
	JE		(.LX14)
	JB		(.LX15)
.LX13:	TEST_L		(CONST(3),addrl)
	JZ		(.LX8)
	XOR_B		(xorvb,REGIND(addrl))		/**/
	INC_L		(addrl)
	DEC_L		(len)
.LX14:	TEST_L		(CONST(3),addrl)
	JZ		(.LX8A)
	XOR_B		(xorvb,REGIND(addrl))		/**/
	INC_L		(addrl)
	DEC_L		(len)
.LX15:	TEST_L		(CONST(3),addrl)
	JZ		(.LX8A)
	XOR_B		(xorvb,REGIND(addrl))		/**/
	INC_L		(addrl)
	DEC_L		(len)
.LX8A:	CMP_L		(CONTENT(GLNAME(vgaWriteTop)),addrl)
	JB		(.LX8)
	PUSH_L		(addrl)
	CALL		(GLNAME(vgaReadWriteNext))
	MOV_L		(tmp,addrl)
	ADD_L		(CONST(4),ESP)
.LX8:	MOV_L		(len,count)
	AND_L		(CONST(0xfffffffc),count)
	MOV_L		(CONTENT(GLNAME(vgaWriteTop)),tmp)
	SUB_L		(addrl,tmp)
	CMP_L		(tmp,count)
	JBE		(.LX16)
	MOV_L		(tmp,count)
.LX16:	SUB_L		(count,len)
	SHR_L		(CONST(2),count)
	JZ		(.LX10)
.LX9:	XOR_L		(xorv,REGIND(addrl))		/**/
	ADD_L		(CONST(4),addrl)
	DEC_L		(ECX)
	JNZ		(.LX9)
	CMP_L		(CONST(3),len)
	JBE		(.LX17)
	PUSH_L		(addrl)
	CALL		(GLNAME(vgaReadWriteNext))
	MOV_L		(tmp,addrl)
	ADD_L		(CONST(4),ESP)
	MOV_L		(len,count)
	AND_L		(CONST(0xfffffffc),count)
	SUB_L		(count,len)
	SHR_L		(CONST(2),count)
.LX18:	XOR_L		(xorv,REGIND(addrl))		/**/
	ADD_L		(CONST(4),addrl)
	DEC_L		(ECX)
	JNZ		(.LX18)
.LX17:	CMP_L		(CONTENT(GLNAME(vgaWriteTop)),addrl)
	JB		(.LX10)
	PUSH_L		(addrl)
	CALL		(GLNAME(vgaReadWriteNext))
	MOV_L		(tmp,addrl)
	ADD_L		(CONST(4),ESP)
.LX10:	CMP_L		(CONST(2),len)
	JA		(.LX11)
	JE		(.LX12)
	CMP_L		(CONST(0),len)
	JE		(.allfinish)
	XOR_B		(xorvb,REGIND(addrl))		/**/
	JMP		(.allfinish)
.LX11:	XOR_W		(xorvw,REGIND(addrl))		/**/
	XOR_B		(xorvb,REGOFF(2,addrl))		/**/
	JMP		(.allfinish)
.LX12:	XOR_W		(xorvw,REGIND(addrl))		/**/
	JMP		(.allfinish)

.SlongL:
	CMP_L		(CONST(2),len)
	JE		(.LS14)
	JB		(.LS15)
.LS13:	TEST_L		(CONST(3),addrl)
	JZ		(.LS8)
	MOV_B		(REGIND(addrl),AL)
	AND_B		(andvb,AL)
	XOR_B		(xorvb,AL)		/**/
	STOS_B
	DEC_L		(len)
.LS14:	TEST_L		(CONST(3),addrl)
	JZ		(.LS8A)
	MOV_B		(REGIND(addrl),AL)
	AND_B		(andvb,AL)
	XOR_B		(xorvb,AL)		/**/
	STOS_B
	DEC_L		(len)
.LS15:	TEST_L		(CONST(3),addrl)
	JZ		(.LS8A)
	MOV_B		(REGIND(addrl),AL)
	AND_B		(andvb,AL)
	XOR_B		(xorvb,AL)		/**/
	STOS_B
	DEC_L		(len)
.LS8A:	CMP_L		(CONTENT(GLNAME(vgaWriteTop)),addrl)
	JB		(.LS8)
	PUSH_L		(addrl)
	CALL		(GLNAME(vgaReadWriteNext))
	MOV_L		(tmp,addrl)
	ADD_L		(CONST(4),ESP)
.LS8:	MOV_L		(len,count)
	AND_L		(CONST(0xfffffffc),count)
	MOV_L		(CONTENT(GLNAME(vgaWriteTop)),tmp)
	SUB_L		(addrl,tmp)
	CMP_L		(tmp,count)
	JBE		(.LS16)
	MOV_L		(tmp,count)
.LS16:	SUB_L		(count,len)
	SHR_L		(CONST(2),count)
	JZ		(.LS10)
.LS9:	MOV_L		(REGIND(addrl),tmp)
	AND_L		(andv,tmp)
	XOR_L		(xorv,tmp)		/**/
	STOS_L
	DEC_L		(ECX)
	JNZ		(.LS9)
	CMP_L		(CONST(3),len)
	JBE		(.LS17)
	PUSH_L		(addrl)
	CALL		(GLNAME(vgaReadWriteNext))
	MOV_L		(tmp,addrl)
	ADD_L		(CONST(4),ESP)
	MOV_L		(len,count)
	AND_L		(CONST(0xfffffffc),count)
	SUB_L		(count,len)
	SHR_L		(CONST(2),count)
.LS18:	MOV_L		(REGIND(addrl),tmp)
	AND_L		(andv,tmp)
	XOR_L		(xorv,tmp)		/**/
	STOS_L
	DEC_L		(ECX)
	JNZ		(.LS18)
.LS17:	CMP_L		(CONTENT(GLNAME(vgaWriteTop)),addrl)
	JB		(.LS10)
	PUSH_L		(addrl)
	CALL		(GLNAME(vgaReadWriteNext))
	MOV_L		(tmp,addrl)
	ADD_L		(CONST(4),ESP)
.LS10:	CMP_L		(CONST(2),len)
	JA		(.LS11)
	JE		(.LS12)
	CMP_L		(CONST(0),len)
	JE		(.allfinish)
	MOV_B		(REGIND(addrl),AL)
	AND_B		(andvb,AL)
	XOR_B		(xorvb,AL)		/**/
	MOV_B		(AL,REGIND(addrl))
	JMP		(.allfinish)
.LS11:	MOV_W		(REGIND(addrl),AX)
	AND_W		(andvw,AX)
	XOR_W		(xorvw,AX)		/**/
	STOS_W
	MOV_B		(REGIND(addrl),AL)
	AND_B		(andvb,AL)
	XOR_B		(xorvb,AL)		/**/
	MOV_B		(AL,REGIND(addrl))
	JMP		(.allfinish)
.LS12:	MOV_W		(REGIND(addrl),AX)
	AND_W		(andvw,AX)
	XOR_W		(xorvw,AX)		/**/
	MOV_W		(AX,REGIND(addrl))
	JMP		(.allfinish)

.noClongL:
	CMP_L		(GXcopy,rop)
	JNE		(.noXlongL)
	CMP_L		(CONST(2),len)
	JE		(.LC6)
	JB		(.LC7)
.LC5:	TEST_L		(CONST(3),addrl)
	JZ		(.LC0)
	MOV_B		(xorvb,REGIND(addrl))
	INC_L		(addrl)
	DEC_L		(len)
.LC6:	TEST_L		(CONST(3),addrl)
	JZ		(.LC0)
	MOV_B		(xorvb,REGIND(addrl))
	INC_L		(addrl)
	DEC_L		(len)
.LC7:	TEST_L		(CONST(3),addrl)
	JZ		(.LC0)
	MOV_B		(xorvb,REGIND(addrl))
	INC_L		(addrl)
	DEC_L		(len)
.LC0:	MOV_L		(len,count)
	AND_L		(CONST(0xfffffffc),count)
	SUB_L		(count,len)
	SHR_L		(CONST(2),count)
	JZ		(.LC2)
	MOV_L		(xorv,tmp)
	CLD
	REPZ
	STOS_L
.LC2:	CMP_L		(CONST(2),len)
	JA		(.LC3)
	JE		(.LC4)
	CMP_L		(CONST(0),len)
	JE		(.allfinish)
	MOV_B		(xorvb,REGIND(addrl))		/**/
	JMP		(.allfinish)
.LC3:	MOV_W		(xorvw,REGIND(addrl))		/**/
	MOV_B		(xorvb,REGOFF(2,addrl))		/**/
	JMP		(.allfinish)
.LC4:	MOV_W		(xorvw,REGIND(addrl))		/**/
	JMP		(.allfinish)


.noXlongL:
	MOV_L		(REGOFF(12,EBP),andv)
	CMP_L		(GXxor,rop)
	JNE		(.noSlongL)
	CMP_L		(CONST(2),len)
	JE		(.LX6)
	JB		(.LX7)
.LX5:	TEST_L		(CONST(3),addrl)
	JZ		(.LX0)
	XOR_B		(xorvb,REGIND(addrl))		/**/
	INC_L		(addrl)
	DEC_L		(len)
.LX6:	TEST_L		(CONST(3),addrl)
	JZ		(.LX0)
	XOR_B		(xorvb,REGIND(addrl))		/**/
	INC_L		(addrl)
	DEC_L		(len)
.LX7:	TEST_L		(CONST(3),addrl)
	JZ		(.LX0)
	XOR_B		(xorvb,REGIND(addrl))		/**/
	INC_L		(addrl)
	DEC_L		(len)
.LX0:	MOV_L		(len,count)
	AND_L		(CONST(0xfffffffc),count)
	SUB_L		(count,len)
	SHR_L		(CONST(2),count)
	JZ		(.LX2)
.LX1:	XOR_L		(xorv,REGIND(addrl))		/**/
	ADD_L		(CONST(4),addrl)
	DEC_L		(ECX)
	JNZ		(.LX1)
.LX2:	CMP_L		(CONST(2),len)
	JA		(.LX3)
	JE		(.LX4)
	CMP_L		(CONST(0),len)
	JE		(.allfinish)
	XOR_B		(xorvb,REGIND(addrl))		/**/
	JMP		(.allfinish)
.LX3:	XOR_W		(xorvw,REGIND(addrl))		/**/
	XOR_B		(xorvb,REGOFF(2,addrl))		/**/
	JMP		(.allfinish)
.LX4:	XOR_W		(xorvw,REGIND(addrl))		/**/
	JMP		(.allfinish)

.noSlongL:
	CMP_L		(CONST(2),len)
	JE		(.LS6)
	JB		(.LS7)
.LS5:	TEST_L		(CONST(3),addrl)
	JZ		(.LS0)
	MOV_B		(REGIND(addrl),AL)
	AND_B		(andvb,AL)
	XOR_B		(xorvb,AL)		/**/
	STOS_B
	DEC_L		(len)
.LS6:	TEST_L		(CONST(3),addrl)
	JZ		(.LS0)
	MOV_B		(REGIND(addrl),AL)
	AND_B		(andvb,AL)
	XOR_B		(xorvb,AL)		/**/
	STOS_B
	DEC_L		(len)
.LS7:	TEST_L		(CONST(3),addrl)
	JZ		(.LS0)
	MOV_B		(REGIND(addrl),AL)
	AND_B		(andvb,AL)
	XOR_B		(xorvb,AL)		/**/
	STOS_B
	DEC_L		(len)
.LS0:	MOV_L		(len,count)
	AND_L		(CONST(0xfffffffc),count)
	SUB_L		(count,len)
	SHR_L		(CONST(2),count)
	JZ		(.LS2)
.LS1:	MOV_L		(REGIND(addrl),tmp)
	AND_L		(andv,tmp)
	XOR_L		(xorv,tmp)		/**/
	STOS_L
	DEC_L		(ECX)
	JNZ		(.LS1)
.LS2:	CMP_L		(CONST(2),len)
	JA		(.LS3)
	JE		(.LS4)
	CMP_L		(CONST(0),len)
	JE		(.allfinish)
	MOV_B		(REGIND(addrl),AL)
	AND_B		(andvb,AL)
	XOR_B		(xorvb,AL)		/**/
	MOV_B		(AL,REGIND(addrl))
	JMP		(.allfinish)
.LS3:	MOV_W		(REGIND(addrl),AX)
	AND_W		(andvw,AX)
	XOR_W		(xorvw,AX)		/**/
	STOS_W
	MOV_B		(REGIND(addrl),AL)
	AND_B		(andvb,AL)
	XOR_B		(xorvb,AL)		/**/
	MOV_B		(AL,REGIND(addrl))
	JMP		(.allfinish)
.LS4:	MOV_W		(REGIND(addrl),AX)
	AND_W		(andvw,AX)
	XOR_W		(xorvw,AX)		/**/
	MOV_W		(AX,REGIND(addrl))
	JMP		(.allfinish)


/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/asm/w32line.s,v 3.3 1996/12/23 06:34:12 dawes Exp $ */
/*******************************************************************************
                        Copyright 1994 by Glenn G. Lai

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyr notice appear in all copies and that
both that copyr notice and this permission notice appear in
supporting documentation, and that the name of Glenn G. Lai not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

Glenn G. Lai DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

Glenn G. Lai
P.O. Box 4314
Austin, Tx 78765
glenn@cs.utexas.edu
10/17/94
*******************************************************************************/
/* $XConsortium: w32line.s /main/4 1996/02/21 17:19:46 kaleb $ */
 
#include "assyntax.h"

	FILE("w32line.s")
	AS_BEGIN

#include "vgaAsm.h"

/*  W32BresS(rop, and, xor, addrl, nlwidth, signdx, signdy, axis,
		 x1, y1, e, e1, e2, len);
*/

#define rop	REGOFF(8,EBP)
#define AND	REGOFF(12,EBP)
#define XOR	REGOFF(16,EBP)
#define addrl   REGOFF(20,EBP)
#define nlwidth	REGOFF(24,EBP)
#define signdx	REGOFF(28,EBP)
#define signdy	REGOFF(32,EBP)
#define axis	REGOFF(36,EBP)
#define x1	REGOFF(40,EBP)
#define y1	REGOFF(44,EBP)
#define e	REGOFF(48,EBP)
#define e1	REGOFF(52,EBP)
#define e2	REGOFF(56,EBP)
#define len	REGOFF(60,EBP)
#define GXcopy	CONST(3)
#define Y_AXIS	CONST(1)

	SEG_DATA
copyright:
	STRING("Copyright 1992, 1993, 10/17/94 by Glenn G. Lai" )

	ALIGNDATA4
speedUpBound1:
	D_LONG	0
speedUpBound2:
	D_LONG	0
jump:
	D_LONG	0
e3:
	D_LONG	0
allowance:
	D_LONG	0
tmp:
	D_LONG	0
tmp1:
	D_LONG	0
carefulJump:
	D_LONG	0
divisor:
	D_LONG	0
segment:
	D_BYTE	0
segment1:
	D_BYTE	0
/****************************/
	SEG_TEXT
	ALIGNTEXT4
	GLOBL	GLNAME(W32BresS)
GLNAME(W32BresS): 
	CMP_L 	(CONST(0), REGOFF(56,ESP))
	JZ	(return)

	PUSH_L	(EBP)
	MOV_L	(ESP, EBP)
	PUSH_L	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)

	MOV_L	(e2, EAX)
	SUB_L	(e1, EAX)
	MOV_L	(EAX, CONTENT(e3))

	MOV_L	(nlwidth, EAX)
	SHL_L	(CONST(2), EAX)
	MOV_L	(EAX, nlwidth)
	MOV_L	(EAX, CONTENT(divisor))

	IMUL_L	(y1, EAX)
	ADD_L	(addrl, EAX)
	ADD_L	(x1, EAX)
	MOV_L	(EAX, EDI)

	MOV_L	(nlwidth, EAX)
	MOV_L	(EAX, EBX)

	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), EAX)
	MOV_L	(EAX, CONTENT(speedUpBound2))

	NEG_L	(EBX)
	MOV_L	(EBX, EAX)
	ADD_L	(CONTENT(GLNAME(vgaWriteTop)), EBX)
	INC_L	(EBX)
	MOV_L	(EBX, CONTENT(speedUpBound1))

	CMP_L	(CONST(0), signdy)
	JGE	(goingDown)

	MOV_L	(EAX, nlwidth)
goingDown:
	MOV_L	(e, EAX)
	SUB_L	(e1, EAX)
	MOV_L	(EAX, e)

/*GGLGGL
	SUB_L	(VGABASE, EDI)
	JC	(pixmap)
*/
/****************************/
window:
	MOV_L	(EDI, EAX)
	SHR_L	(CONST(16), EAX)

	MOV_B	(AL, AH)
	MOV_B	(AL, BL)
	SHR_B	(CONST(4), BL)
	AND_B	(CONST(15), AL)
	SHL_B	(CONST(4), AH)
	OR_B	(AH, AL)
	MOV_B	(AL, CONTENT(segment))
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	MOV_B	(BL, AL)
	SHL_B	(CONST(4), BL)
	OR_B	(BL, AL)
	MOV_B	(AL, CONTENT(segment1))
	MOV_W	(CONST(0x3cb), DX)
	OUT_B

	AND_L	(CONST(0xffff), EDI)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)

	MOV_L	(e, EBX)
	MOV_L	(ADDR(downLoop2), CONTENT(carefulJump))

	CMP_L	(CONST(1), signdy)
	JE	(downLoop0)

	MOV_L	(ADDR(upLoop2), CONTENT(carefulJump))
	JMP	(upLoop0)
/****************************/
	ALIGNTEXT4
upLoop:
	CMP_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
	JNC	(upLoop0)

        MOV_L   (ADDR(upLoop0), CONTENT(jump))
        JMP     (prev)
	ALIGNTEXT4
upLoop0:
	CMP_L	(CONTENT(speedUpBound1), EDI)
	JNC	(careful)
upLoop2:
	MOV_L	(EDI, EAX)
	INC_L	(EAX)
	SUB_L	(CONTENT(GLNAME(vgaWriteBottom)), EAX)
	XOR_L	(EDX, EDX)
	DIV_L	(CONTENT(divisor))

        CMP_L   (EDX, EAX)
        JNC     (upLoop3)
        INC_L   (EAX)
upLoop3:
	MOV_L	(len, ECX)
	CMP_L	(ECX, EAX)
	JC	(upLoop1)

	MOV_L	(ECX, EAX)
upLoop1:
	SUB_L	(EAX, ECX)
	MOV_L	(ECX, len)

	CALL	(line)

	CMP_L	(CONST(0), len)
	JNZ	(upLoop)
done:
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
return:
	RET
/****************************/
	ALIGNTEXT4
downLoop:
        CMP_L   (CONTENT(GLNAME(vgaWriteTop)), EDI)
        JC	(downLoop0)

        MOV_L   (ADDR(downLoop0), CONTENT(jump))
        JMP     (next)
        ALIGNTEXT4
downLoop0:
	CMP_L	(CONTENT(speedUpBound2), EDI)
	JC	(careful)
downLoop2:
	MOV_L	(CONTENT(GLNAME(vgaWriteTop)), EAX)
	SUB_L	(EDI, EAX)
	XOR_L	(EDX, EDX)
	DIV_L	(CONTENT(divisor))

	CMP_L	(EDX, EAX)
	JNC	(downLoop3)
	INC_L	(EAX)
downLoop3:
	MOV_L	(len, ECX)
	CMP_L	(ECX, EAX)
	JC	(downLoop1)

	MOV_L	(ECX, EAX)
downLoop1:
	SUB_L	(EAX, ECX)
	MOV_L	(ECX, len)

	CALL	(line)

	CMP_L	(CONST(0), len)
	JNZ	(downLoop)

	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/****************************/
	ALIGNTEXT4
line:
	MOV_L	(e1, ECX)
	MOV_L	(nlwidth, ESI)

	CMP_L	(GXcopy, rop)
	JNE	(wMix)
/****************************/
	MOV_L	(EAX, EDX)
	MOV_L	(XOR, EAX)
	CMP_L	(Y_AXIS, axis)
	JE	(setY)
/****************************/
setX:
	CMP_L	(CONST(1), signdx)
	JNE	(setXLeft)
	JMP	(setXRight)
/****************************/
	ALIGNTEXT4
setXRight:
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(ECX, EBX)
	JL	(setXRight1)

	ADD_L	(ESI, EDI)
	ADD_L	(CONTENT(e3), EBX)
setXRight1:
	INC_L	(EDI)
	DEC_L	(EDX)
	JNZ	(setXRight)
	RET
/****************************/
	ALIGNTEXT4
setXLeft:
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(ECX, EBX)
	JL	(setXLeft1)

	ADD_L	(ESI, EDI)
	ADD_L	(CONTENT(e3), EBX)
setXLeft1:
	DEC_L	(EDI)
	DEC_L	(EDX)
	JNZ	(setXLeft)
	RET
/****************************/
	ALIGNTEXT4
setY:
	CMP_L	(CONST(1), signdx)
	JNE	(setYLeft)
	JMP	(setYRight)
/****************************/
	ALIGNTEXT4
setYRight:
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(ECX, EBX)
	JL	(setYRight1)

	INC_L	(EDI)
	ADD_L	(CONTENT(e3), EBX)
setYRight1:
	ADD_L	(ESI, EDI)
	DEC_L	(EDX)
	JNZ	(setYRight)
	RET
/****************************/
	ALIGNTEXT4
setYLeft:
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(ECX, EBX)
	JL	(setYLeft1)

	DEC_L	(EDI)
	ADD_L	(CONTENT(e3), EBX)
setYLeft1:
	ADD_L	(ESI, EDI)
	DEC_L	(EDX)
	JNZ	(setYLeft)
	RET
/****************************/
	ALIGNTEXT4
wMix:
	MOV_L	(EAX, CONTENT(allowance))
	MOV_L	(XOR, EAX)
	MOV_L	(AND, EDX)
	MOV_B	(AL, DL)

	CMP_L	(Y_AXIS, axis)
	JE	(mixY)
/****************************/
mixX:
	CMP_L	(CONST(1), signdx)
	JNE	(mixXLeft)
	JMP	(mixXRight)
/****************************/
	ALIGNTEXT4
mixXRight:
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))

	ADD_L	(ECX, EBX)
	JL	(mixXRight1)

	ADD_L	(ESI, EDI)
	ADD_L	(CONTENT(e3), EBX)
mixXRight1:
	INC_L	(EDI)
	DEC_L	(CONTENT(allowance))
	JNZ	(mixXRight)
	RET
/****************************/
	ALIGNTEXT4
mixXLeft:
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))

	ADD_L	(ECX, EBX)
	JL	(mixXLeft1)

	ADD_L	(ESI, EDI)
	ADD_L	(CONTENT(e3), EBX)
mixXLeft1:
	DEC_L	(EDI)
	DEC_L	(CONTENT(allowance))
	JNZ	(mixXLeft)
	RET
/****************************/
	ALIGNTEXT4
mixY:
	CMP_L	(CONST(1), signdx)
	JNE	(mixYLeft)
	JMP	(mixYRight)
/****************************/
	ALIGNTEXT4
mixYRight:
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))

	ADD_L	(ECX, EBX)
	JL	(mixYRight1)

	INC_L	(EDI)
	ADD_L	(CONTENT(e3), EBX)
mixYRight1:
	ADD_L	(ESI, EDI)
	DEC_L	(CONTENT(allowance))
	JNZ	(mixYRight)
	RET
/****************************/
	ALIGNTEXT4
mixYLeft:
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))

	ADD_L	(ECX, EBX)
	JL	(mixYLeft1)

	DEC_L	(EDI)
	ADD_L	(CONTENT(e3), EBX)
mixYLeft1:
	ADD_L	(ESI, EDI)
	DEC_L	(CONTENT(allowance))
	JNZ	(mixYLeft)
	RET
/****************************/
	ALIGNTEXT4
careful:
	MOV_L	(XOR, EAX)
	MOV_L	(e1, ECX)
	MOV_L	(CONST(0), CONTENT(tmp))

	CMP_L	(Y_AXIS, axis)
	JE	(carefulY)
/****************************/
carefulX:
	MOV_L	(signdx, ESI)

	CMP_L	(GXcopy, rop)
	JNE	(cXMix0)

	MOV_L	(len, EDX)
	MOV_L	(ADDR(cXSet2), CONTENT(jump))
	JMP	(cXSet)
/****************************/
	ALIGNTEXT4
cXSet:
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(ECX, EBX)
	JL	(cXSet1)

        MOV_L   (CONST(1), CONTENT(tmp))
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
cXSet1:
	ADD_L	(ESI, EDI)

	CMP_L	(CONTENT(GLNAME(vgaWriteTop)), EDI)
	JNC	(next)

	CMP_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
	JC	(prev)
cXSet2:
	DEC_L	(EDX)
	JZ	(done)

	CMP_L	(CONST(1), CONTENT(tmp))
	JNE	(cXSet)

	MOV_L	(EDX, len)
	JMP	(VARINDIRECT(carefulJump))
/****************************/
	ALIGNTEXT4
cXMix0:
	MOV_L	(AND, EDX)
	MOV_B	(AL, DL)
	MOV_L	(ADDR(cXMix2), CONTENT(jump))
cXMix:
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))

	ADD_L	(ECX, EBX)
	JL	(cXMix1)

	MOV_L	(CONST(1), CONTENT(tmp))
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
cXMix1:
	ADD_L	(ESI, EDI)

	CMP_L	(CONTENT(GLNAME(vgaWriteTop)), EDI)
	JNC	(next)

	CMP_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
	JC	(prev)
cXMix2:
	DEC_L	(len)
	JZ	(done)

	CMP_L	(CONST(1), CONTENT(tmp))
	JNE	(cXMix)

	JMP	(VARINDIRECT(carefulJump))
/****************************/
	ALIGNTEXT4
carefulY:
	MOV_L	(nlwidth, ESI)

	CMP_L	(GXcopy, rop)
	JNE	(cYMix0)

	MOV_L	(len, EDX)
	MOV_L	(ADDR(cYSet2), CONTENT(jump))
	JMP	(cYSet)
/****************************/
	ALIGNTEXT4
cYSet:
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(ECX, EBX)
	JL	(cYSet1)

	ADD_L	(signdx, EDI)
	ADD_L	(CONTENT(e3), EBX)
cYSet1:
	ADD_L	(ESI, EDI)

	CMP_L	(CONTENT(GLNAME(vgaWriteTop)), EDI)
	JNC	(next)

	CMP_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
	JC	(prev)
cYSet2:
	DEC_L	(EDX)
	JZ	(done)

	MOV_L	(EDX, len)
	JMP	(VARINDIRECT(carefulJump))
/****************************/
	ALIGNTEXT4
cYMix0:
	MOV_L	(AND, EDX)
	MOV_B	(AL, DL)
	MOV_L	(ADDR(cYMix2), CONTENT(jump))
cYMix:
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))

	ADD_L	(ECX, EBX)
	JL	(cYMix1)

	ADD_L	(signdx, EDI)
	ADD_L	(CONTENT(e3), EBX)
cYMix1:
	ADD_L	(ESI, EDI)

	CMP_L	(CONTENT(GLNAME(vgaWriteTop)), EDI)
	JNC	(next)

	CMP_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
	JC	(prev)
cYMix2:
	DEC_L	(len)
	JZ	(done)

	JMP	(VARINDIRECT(carefulJump))
/****************************/
	ALIGNTEXT4
next:
	MOV_L	(EDX, CONTENT(tmp1))
	SUB_L	(CONST(0x10000), EDI)

	MOV_B	(CONTENT(segment), AL)
	CMP_B	(CONST(255), AL)
	JNE	(next1)

	MOV_B	(CONTENT(segment1), AL)
	ADD_B	(CONST(17), AL)
	MOV_B	(AL, CONTENT(segment1))
	MOV_W	(CONST(0x3cb), DX)
	OUT_B
	MOV_B	(CONST(239), AL)
next1:
	ADD_B	(CONST(17), AL)
	MOV_B	(AL, CONTENT(segment))
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	MOV_L	(XOR, EAX)
	MOV_L	(CONTENT(tmp1), EDX)
	JMP	(VARINDIRECT(jump))
/****************************/
	ALIGNTEXT4
prev:
	MOV_L	(EDX, CONTENT(tmp1))
	ADD_L	(CONST(0x10000), EDI)

	MOV_B	(CONTENT(segment), AL)
	AND_B	(AL, AL)
	JNZ	(prev1)

	MOV_B	(CONTENT(segment1), AL)
	SUB_B	(CONST(17), AL)
	MOV_B	(AL, CONTENT(segment1))
	MOV_W	(CONST(0x3cb), DX)
	OUT_B
	MOV_B	(CONST(16), AL)
prev1:
	SUB_B	(CONST(17), AL)
	MOV_B	(AL, CONTENT(segment))
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	MOV_L	(XOR, EAX)
	MOV_L	(CONTENT(tmp1), EDX)
	JMP	(VARINDIRECT(jump))
/****************************/
	ALIGNTEXT4
pixmap:
	CMP_L	(Y_AXIS, axis)
	JNE	(pixmap1)

	MOV_L	(nlwidth, EAX)
	MOV_L	(signdx, EBX)
	MOV_L	(EBX, nlwidth)
	MOV_L	(EAX, signdx)
pixmap1:
	ADD_L	(VGABASE, EDI)
	MOV_L	(len, ECX)
	CMP_L	(GXcopy, rop)
	JNE	(mmLine2)
mmLine1:
	MOV_L	(ECX, EAX)
	AND_L	(CONST(3), EAX)
	MOV_L	(REGDIS(fsTable,EAX,4), EAX)
	MOV_L	(EAX, CONTENT(jump) )
	MOV_L	(XOR, EAX)
	MOV_L	(e, EBX)
	MOV_L	(e1, EDX)
	MOV_L	(signdx, ESI)
	SHR_L	(CONST(2), ECX)
	JZ	(fs1)
fs6:
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(EDX, EBX)
	JL	(fs2)
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
fs2:
	ADD_L	(ESI, EDI)
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(EDX, EBX)
	JL	(fs3)
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
fs3:
	ADD_L	(ESI, EDI)
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(EDX, EBX)
	JL	(fs4)
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
fs4:
	ADD_L	(ESI, EDI)
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(EDX, EBX)
	JL	(fs5)
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
fs5:
	ADD_L	(ESI, EDI)
	DEC_L	(ECX)
	JNZ	(fs6)
fs1:
	JMP	(VARINDIRECT(jump))
	SEG_DATA
	ALIGNDATA4
fsTable:
	D_LONG	done, fs8, fs9, fs10
	SEG_TEXT
	ALIGNTEXT4
fs10:
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(EDX, EBX)
	JL	(fs11)
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
fs11:
	ADD_L	(ESI, EDI)
fs9:
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(EDX, EBX)
	JL	(fs12)
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
fs12:
	ADD_L	(ESI, EDI)
fs8:
	MOV_B	(AL, REGIND(EDI))
	JMP	(done)
/****************************/
	ALIGNTEXT4
mmLine2:
	MOV_L	(ECX, EAX)
	AND_L	(CONST(3), EAX)
	MOV_L	(REGDIS(fmTable,EAX,4), EAX)
	MOV_L	(EAX, CONTENT(jump))
	MOV_L	(XOR, EAX)
	MOV_L	(AND, EDX)
	MOV_B	(AL, DL)
	MOV_L	(e, EBX)
	MOV_L	(signdx, ESI)
	SHR_L	(CONST(2), ECX)
	JZ	(fm1)
fm6:
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(e1, EBX)
	JL	(fm2)
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
fm2:
	ADD_L	(ESI, EDI)
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(e1, EBX)
	JL	(fm3)
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
fm3:
	ADD_L	(ESI, EDI)
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(e1, EBX)
	JL	(fm4)
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
fm4:
	ADD_L	(ESI, EDI)
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(e1, EBX)
	JL	(fm5)
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
fm5:
	ADD_L	(ESI, EDI)
	DEC_L	(ECX)
	JNZ	(fm6)
fm1:
	JMP	(VARINDIRECT(jump))
	SEG_DATA
	ALIGNDATA4
fmTable:
	D_LONG	done, fm8, fm9, fm10
	SEG_TEXT
	ALIGNTEXT4
fm10:
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(e1, EBX)
	JL	(fm11)
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
fm11:
	ADD_L	(ESI, EDI)
fm9:
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))
	ADD_L	(e1, EBX)
	JL	(fm12)
	ADD_L	(nlwidth, EDI)
	ADD_L	(CONTENT(e3), EBX)
fm12:
	ADD_L	(ESI, EDI)
fm8:
	MOV_B	(REGIND(EDI), AL)
	AND_B	(DH, AL)
	XOR_B	(DL, AL)
	MOV_B	(AL, REGIND(EDI))
	JMP	(done)


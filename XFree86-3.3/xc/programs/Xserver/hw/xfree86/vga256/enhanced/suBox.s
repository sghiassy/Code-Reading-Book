/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/enhanced/suBox.s,v 3.2 1996/12/23 06:59:10 dawes Exp $ */
/*******************************************************************************
			Copyright 1992 by Glenn G. Lai 

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
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
(glenn@cs.utexas.edu)
7/27/92
*******************************************************************************/
/* $XConsortium: suBox.s /main/5 1996/02/21 18:09:14 kaleb $ */

#include "assyntax.h"

	FILE("suBox.s")
	AS_BEGIN

#include "vgaAsm.h"

/*
    void SpeedUpBox(dst, fill, h, w, screen)
*/

#define dst	REGOFF(8,EBP)
#define fill	REGOFF(12,EBP)
#define height	REGOFF(16,EBP)
#define width	REGOFF(20,EBP)
#define screen	REGOFF(24,EBP)

	SEG_DATA
copyright:
	STRING("Copyright 7/27/1992 by Glenn G. Lai")
	ALIGNDATA4
pixTable:
	D_LONG	pix0D, pix1D, pix2D, pix3D
pixMiddle:
	D_LONG	0
/****************************************/
	SEG_TEXT
	ALIGNTEXT4
	GLOBL GLNAME(SpeedUpBox)
GLNAME(SpeedUpBox):
	MOV_L	(REGOFF(12,ESP), EAX)
	MOV_L	(REGOFF(16,ESP), ECX)
	MOV_L	(EAX, EDX)
	OR_L	(ECX, EDX)
	JZ	(return)

	CMP_L	(CONST(1), EAX)
	JNE	(notHLine)

	MOV_L	(ECX, REGOFF(12,ESP))
	MOV_L	(REGOFF(20,ESP), EAX)
	MOV_L	(EAX, REGOFF(16,ESP))
	JMP	(GLNAME(SpeedUpHLine1))
	ALIGNTEXT4
notHLine:
	CMP_L	(CONST(1), ECX)
	JNE	(notVHLine)

	MOV_L	(EAX, REGOFF(12,ESP))
	MOV_L	(EAX, ECX)
	MOV_L	(REGOFF(20,ESP), EAX)
	MOV_L	(EAX, REGOFF(16,ESP))
	JMP	(GLNAME(SpeedUpVLine1))
/****************************************/
	ALIGNTEXT4
notVHLine:
	MOV_L	(REGOFF(4,ESP), EAX)
	CMP_L	(VGABASE, EAX)
	JNC	(window)

	PUSH_L 	(EBP)
	MOV_L 	(ESP, EBP)
	PUSH_L 	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)
pix:
	MOV_L	(fill, EBX)
	MOV_L	(width, ECX)
	MOV_L	(height, EDX)
	MOV_L	(screen, ESI)
	MOV_L	(EAX, EDI)

	ADD_L	(CONST(3), EAX)
	AND_L	(CONST(0xfffffffc), EAX)
	MOV_L	(EAX, CONTENT(pixMiddle))

	SUB_L	(EDI, EAX)
	CMP_L	(EAX, ECX)
	JNC	(fillLeft)
	MOV_L	(ECX, EAX)
fillLeft:
	SUB_L	(EAX, ECX)
	CALL	(CODEPTR(REGDIS(pixTable,EAX,4)))
fillRight:
	MOV_L	(ECX, EAX)
	AND_L	(CONST(3), EAX)
	JZ	(fillMiddle)

	MOV_L	(ECX, EDI)
	AND_L	(CONST(0xfffffffc), EDI)
	ADD_L	(CONTENT(pixMiddle), EDI)
	CALL	(CODEPTR(REGDIS(pixTable,EAX,4)))
fillMiddle:
	SAR_L	(CONST(2), ECX)
	JZ	(done)
	MOV_L	(ECX, CONTENT(tmp))
	MOV_L	(EBX, EAX)
	MOV_L	(CONTENT(pixMiddle), EBX)
fillMiddle1:
	MOV_L	(CONTENT(tmp), ECX)
	MOV_L	(EBX, EDI)
	REP
	STOS_L
	ADD_L	(ESI, EBX)
	DEC_L	(EDX)
	JNZ	(fillMiddle1)
	JMP	(done)
/****************/
	ALIGNTEXT4
pix0D:
	RET
/****************/
	ALIGNTEXT4
pix1D:
	MOV_L	(EDX, EAX)
p1D:
	MOV_B	(BL, REGIND(EDI))
	ADD_L	(ESI, EDI)
	DEC_L	(EAX)
	JNZ	(p1D)
	RET
/****************/
	ALIGNTEXT4
pix2D:
	MOV_L	(EDX, EAX)
p2D:
	MOV_W	(BX, REGIND(EDI))
	ADD_L	(ESI, EDI)
	DEC_L	(EAX)
	JNZ	(p2D)
	RET
/****************/
	ALIGNTEXT4
pix3D:
	MOV_L	(EDX, EAX)
p3D:
	MOV_B	(BL, REGIND(EDI))
	MOV_W	(BX, REGOFF(1,EDI))
	ADD_L	(ESI, EDI)
	DEC_L	(EAX)
	JNZ	(p3D)
	RET
/****************************************/
	SEG_DATA
	ALIGNDATA4
npTable:
	D_LONG	npM, npMR, npLM, npLMR
/****************************************/
	SEG_TEXT
	ALIGNTEXT4
window:
	SUB_L	(VGABASE, EAX)
	PUSH_L 	(EBP)
	MOV_L 	(ESP,EBP)
	PUSH_L 	(EDI)
	MOV_L	(EAX, EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)

/* Change THRESHOLD at your own risk!!! */ 
#define THRESHOLD	CONST(22)
	CMP_L	(THRESHOLD, ECX)
	JGE	(pMode)

	MOV_L	(screen, EAX)
	SUB_L	(ECX, EAX)
	MOV_L	(EAX, CONTENT(offset))
	ADD_L	(CONTENT(GLNAME(vgaWriteTop)), EAX)
	MOV_L	(EAX, CONTENT(speedUpTop))

	MOV_L	(ADDR(npTable), EAX)
	TEST_L	(CONST(1), EDI)
	JZ	(window1)
	ADD_L	(CONST(8), EAX)
	DEC_L	(ECX)
window1:
	SHR_L	(CONST(1), ECX)
	JZ	(window2)
	JNC	(window3)
	ADD_L	(CONST(4), EAX)
	JMP	(window3)
window2:
	MOV_L	(ADDR(npTable), EAX)
	MOV_L	(CONST(1), ECX)
window3:
	MOV_L	(REGIND(EAX), EAX)
	MOV_L	(EAX, CONTENT(func))
	MOV_L	(ECX, CONTENT(mCount))

	MOV_L	(EDI, EAX)
	SHR_L	(CONST(16), EAX)
	MOV_B	(AL, CONTENT(segment))
	
	AND_L	(CONST(0xffff), EDI)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
	JMP	(npLoop3)
	ALIGNTEXT4
npLoop:
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)), EDI)
	JC	(npLoop1)
	SUB_L	(CONST(0x10000), EDI)
	MOV_B	(CONTENT(segment), AL)
	INC_B	(AL)
	MOV_B	(AL, CONTENT(segment))
npLoop3:
	MOV_W	(CONST(0x3cd), DX)
	OUT_B
npLoop1:
	MOV_L	(CONTENT(speedUpTop), EAX)
	SUB_L	(EDI, EAX)
	XOR_L	(EDX, EDX )
	DIV_L	(screen)

	OR_L	(EAX, EAX)
	JZ 	(npPartial)

	MOV_L	(height, EBX)
	CMP_L	(EBX, EAX)
	JC	(npLoop2)
	MOV_L	(EBX, EAX)
npLoop2:
	SUB_L	(EAX, EBX)
	MOV_L	(EBX, height)
	MOV_L	(EAX, EDX)

	MOV_L	(fill, EAX)
	MOV_L	(CONTENT(mCount), EBX)
	MOV_L	(CONTENT(offset), ESI)

	CALL	(VARINDIRECT(func))

	CMP_L	(CONST(0), height)
	JNZ	(npLoop)

	POP_L 	(EBX)
	POP_L 	(ESI)
	POP_L 	(EDI)
	LEAVE
	RET
/****************************************/
	ALIGNTEXT4
npM:
	MOV_L	(EBX, ECX)
	REP
	STOS_W
	ADD_L	(ESI, EDI)
	DEC_L	(EDX)
	JNZ	(npM)
	RET
/****************************************/
	ALIGNTEXT4
npMR:
	MOV_L	(EBX, ECX)
	REP
	STOS_W
	STOS_B
	ADD_L	(ESI, EDI)
	DEC_L	(EDX)
	JNZ	(npMR)
	RET
/****************************************/
	ALIGNTEXT4
npLM:
	MOV_L	(EBX, ECX)
	STOS_B
	REP
	STOS_W
	ADD_L	(ESI, EDI)
	DEC_L	(EDX)
	JNZ	(npLM)
	RET
/****************************************/
	ALIGNTEXT4
npLMR:
	MOV_L	(EBX, ECX)
	STOS_B
	REP
	STOS_W
	STOS_B
	ADD_L	(ESI, EDI)
	DEC_L	(EDX)
	JNZ	(npLMR)
	RET
/****************************************/
	ALIGNTEXT4
npPartial:
	SUB_L	(CONTENT(offset), EDX)

	MOV_L	(fill, EAX)
	MOV_L	(width, EBX)
	MOV_L	(EDX, ECX)
	SUB_L	(ECX, EBX)

	TEST_L	(CONST(1), EDI)
	JZ	(npPartial1)
	STOS_B
	DEC_L	(ECX)
	JZ	(npPartial2)
npPartial1:
	SHR_L	(CONST(1), ECX)
	REP
	STOS_W
npPartial2:
	SUB_L	(CONST(0x10000), EDI)
	MOV_B	(CONTENT(segment), AL)
	INC_B	(AL)
	MOV_B	(AL, CONTENT(segment))
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	MOV_L	(fill, EAX)
	MOV_L	(EBX, ECX)
	SHR_L	(CONST(1), ECX)
	JZ	(npPartial3)
	REP
	STOS_W
	JNC	(npPartial4)
npPartial3:
	STOS_B
npPartial4:
	ADD_L	(CONTENT(offset), EDI)
	DEC_L	(height)
	JNZ	(npLoop1)

	POP_L 	(EBX)
	POP_L 	(ESI)
	POP_L 	(EDI)
	LEAVE
	RET
/****************************************/
	SEG_DATA
	ALIGNDATA4
lCount:
	D_LONG	0
mCount:
	D_LONG	0
rCount:
	D_LONG	0
offset:
	D_LONG	0
allowance:
	D_LONG	0
func:
	D_LONG	0
speedUpTop:
	D_LONG	0
tmp:
	D_LONG	0
pCorrect:
	D_LONG	0
pTable:
	D_LONG	pM, pMR, pLM, pLMR
lMaskTable:
	D_WORD	0x0f02, 0x0802, 0x0c02, 0x0e02
rMaskTable:
	D_WORD	0x0f02, 0x0102, 0x0302, 0x0702
lMask:
	D_WORD	0
rMask:
	D_WORD	0
segment:
	D_BYTE	0
/****************************************/
	SEG_TEXT
	ALIGNTEXT4
pMode:
	MOV_L	(EDI, ESI)
	ADD_L	(CONST(3), ESI)
	AND_L	(CONST(0xfffffffc), ESI)
	SUB_L	(EDI, ESI)

	SUB_L	(ESI, ECX)
	MOV_L	(ECX, EAX)
	AND_L	(CONST(3), EAX)

	MOV_L	(CONST(0), CONTENT(tmp))
	MOV_L	(ADDR(pTable), EBX)
	XOR_L	(EDX, EDX)
	SAL_L	(CONST(1), ESI)
	JZ	(pNoL)

	ADD_L	(CONST(8), EBX)
	MOV_L	(CONST(1), EDX)
	MOV_L	(CONST(1), CONTENT(tmp))
pNoL:
	MOV_L	(EDX, CONTENT(pCorrect))
	MOV_W	(lMaskTable(ESI), DX)
	MOV_W	(DX, CONTENT(lMask))

	SAR_L	(CONST(2), ECX)
	MOV_L	(ECX, CONTENT(mCount))

	SAL_L	(CONST(1), EAX)
	JZ	(pNoR)
	ADD_L	(CONST(4), EBX)
	INC_L	(CONTENT(tmp))
pNoR:
	MOV_L	(REGIND(EBX), EBX)
	MOV_L	(EBX, CONTENT(func))
	MOV_W	(rMaskTable(EAX), DX)
	MOV_W	(DX, CONTENT(rMask))

	MOV_L	(screen, EAX)
	SAR_L	(CONST(2), EAX)
	MOV_L	(EAX, screen)
	SUB_L	(ECX, EAX)
	MOV_L	(EAX, CONTENT(offset))
	SUB_L	(CONTENT(tmp), EAX)
	ADD_L	(CONTENT(GLNAME(vgaWriteTop)), EAX)
	MOV_L	(EAX, CONTENT(speedUpTop))
/***************************/
	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONST(0x0604), AX )
	OUT_W
	MOV_W	(CONST(0x3ce), DX)
	MOV_W	(CONST(0x0001), AX)
	OUT_W
	MOV_W	(CONST(0x0003), AX)
	OUT_W
	MOV_W	(CONST(0x4005), AX)
	OUT_W
	MOV_W	(CONST(0xff08), AX)
	OUT_W
/***************************/
	MOV_L	(EDI, EAX)
	SHR_L	(CONST(18), EAX)
	MOV_B	(AL, CONTENT(segment))

	AND_L	(CONST(0x3ffff), EDI)
	SAR_L	(CONST(2), EDI)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
	JMP	(pLoop3)
/***************************/
	ALIGNTEXT4
pLoop:
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)), EDI)
	JC	(pLoop1)
	SUB_L	(CONST(0x10000), EDI)
	MOV_B	(CONTENT(segment), AL)
	INC_B	(AL)
	MOV_B	(AL, CONTENT(segment))
pLoop3:
	MOV_W	(CONST(0x3cd), DX)
	OUT_B
pLoop1:
	MOV_L	(CONTENT(speedUpTop), EAX)
	SUB_L	(EDI, EAX)
	XOR_L	(EDX, EDX)
	DIV_L	(screen)

	OR_L	(EAX, EAX)
	JZ	(pPartial)

	MOV_L	(height, EBX)
	CMP_L	(EBX, EAX)
	JC	(pLoop2)
	MOV_L	(EBX, EAX)
pLoop2:
	MOV_L	(EAX, CONTENT(allowance))
	MOV_L	(CONTENT(mCount), EBX)
	MOV_L	(CONTENT(offset), ESI)

	CALL	(VARINDIRECT(func))

	SUB_L	(CONTENT(pCorrect), EDI)
pLoop4:
	MOV_L	(height, EBX)
	SUB_L	(CONTENT(allowance), EBX)
	MOV_L	(EBX, height)
	JNZ	(pLoop)

	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONST(0x0f02), AX)
	OUT_W
	MOV_W	(CONST(0x0c04), AX)
	OUT_W

	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/****************************************/
	ALIGNTEXT4
pPartial:
	MOV_L	(CONTENT(GLNAME(vgaWriteTop)), ECX)
	SUB_L	(EDI, ECX)

	MOV_W	(CONTENT(lMask), AX)
	MOV_W	(CONST(0x3c4), DX)
	OUT_W

	MOV_L	(fill, EAX)
	MOV_L	(CONTENT(mCount), EBX)

	CMP_L	(CONST(0), CONTENT(pCorrect))
	JE	(pPartial1)

	STOS_B
	DEC_L	(ECX)
	MOV_W	(CONST(0x0f02), AX)
	OUT_W
	MOV_L	(fill, EAX)
pPartial1:
	SUB_L	(ECX, EBX)
	OR_L	(ECX, ECX)
	JZ	(pPartial2)
	SHR_L	(CONST(1), ECX)
	JNC	(pPartial3)
	STOS_B
	JZ	(pPartial2)
pPartial3:
	REP
	STOS_W
pPartial2:
	SUB_L	(CONST(0x10000), EDI)
	MOV_B	(CONTENT(segment), AL)
	INC_B	(AL)
	MOV_B	(AL, CONTENT(segment))
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	MOV_L	(fill, EAX)
	MOV_L	(EBX, ECX)
	OR_L	(ECX, ECX)
	JZ	(pPartial4)

	SHR_L	(CONST(1), ECX)
	JZ	(pPartial6)
	REP
	STOS_W
	JNC	(pPartial4)
pPartial6:
	STOS_B
pPartial4:
	MOV_W	(CONTENT(rMask), AX)
	CMP_W	(CONST(0x0f02), AX)
	JE	(pPartial5)
	MOV_W	(CONST(0x3c4), DX)
	OUT_W
	MOV_L	(fill, EAX)
	MOV_B	(AL, REGIND(EDI))
pPartial5:
	ADD_L	(CONTENT(offset), EDI)
	SUB_L	(CONTENT(pCorrect), EDI)
	DEC_L	(height)
	JNZ	(pLoop1)

	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONST(0x0f02), AX)
	OUT_W
	MOV_W	(CONST(0x0c04), AX)
	OUT_W
done:
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
return:
	RET
/****************************/
	ALIGNTEXT4
pMR:
	MOV_W	(CONTENT(rMask), AX)
	MOV_W	(CONST(0x3c4), DX)
	OUT_W

	MOV_L	(fill, EAX)
	MOV_L	(CONTENT(allowance), EBX)
	MOV_L	(EDI, EDX)
	ADD_L	(CONTENT(mCount), EDX)
	MOV_L	(screen, ESI)
pMR1:
	MOV_B	(AL, REGIND(EDX))
	ADD_L	(ESI, EDX)
	DEC_L	(EBX)
	JNZ	(pMR1)
	JMP	(pM)
/****************************/
	ALIGNTEXT4
pLMR:
	MOV_W	(CONTENT(rMask), AX)
	MOV_W	(CONST(0x3c4), DX)
	OUT_W

	MOV_L	(fill, EAX)
	MOV_L	(CONTENT(allowance), EBX)
	LEA_L	(REGOFF(1,EDI), EDX)
	ADD_L	(CONTENT(mCount), EDX)
	MOV_L	(screen, ESI)
pLMR1:
	MOV_B	(AL, REGIND(EDX))
	ADD_L	(ESI, EDX)
	DEC_L	(EBX)
	JNZ	(pLMR1)
/****************************/
pLM:
	MOV_W	(CONTENT(lMask), AX)
	MOV_W	(CONST(0x3c4), DX)
	OUT_W

	MOV_L	(fill, EAX)
	MOV_L	(CONTENT(allowance), EBX)
	MOV_L	(EDI, EDX)
	MOV_L	(screen, ESI)
pLM1:
	MOV_B	(AL, REGIND(EDX))
	ADD_L	(ESI, EDX)
	DEC_L	(EBX)
	JNZ	(pLM1)

	INC_L	(EDI)
/****************************/
pM:
	MOV_W	(CONST(0x0f02), AX)
	MOV_W	(CONST(0x3c4), DX)
	OUT_W

	MOV_L	(fill, EAX)
	MOV_L	(CONTENT(mCount), EBX)
	MOV_L	(CONTENT(allowance), EDX)
	MOV_L	(CONTENT(offset), ESI)
	TEST_L	(CONST(1), EDI)
	JNZ	(pM1)
	SHR_L	(CONST(1), EBX)
	JC	(pM2)
pM3:
	MOV_L	(EBX, ECX)
	REP
	STOS_W
	ADD_L	(ESI, EDI)
	DEC_L	(EDX)
	JNZ	(pM3)
	RET
/****************************/
	ALIGNTEXT4
pM2:
	MOV_L	(EBX, ECX)
	REP
	STOS_W
	STOS_B
	ADD_L	(ESI, EDI)
	DEC_L	(EDX)
	JNZ	(pM2)
	RET
/****************************/
	ALIGNTEXT4
pM1:
	DEC_L	(EBX)
	SHR_L	(CONST(1), EBX)
	JC	(pM4)
pM5:
	STOS_B
	MOV_L	(EBX, ECX)
	REP
	STOS_W
	ADD_L	(ESI, EDI)
	DEC_L	(EDX)
	JNZ	(pM5)
	RET
/****************************/
	ALIGNTEXT4
pM4:
	STOS_B
	MOV_L	(EBX, ECX)
	REP
	STOS_W
	STOS_B
	ADD_L	(ESI, EDI)
	DEC_L	(EDX)
	JNZ	(pM4)
	RET

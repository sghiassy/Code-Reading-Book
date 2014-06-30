/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/enhanced/suVHLine.s,v 3.2 1996/12/23 06:59:14 dawes Exp $ */
/*******************************************************************************
			Copyright 1992, 1993 by Glenn G. Lai 

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
8/21/92
7/10/93
*******************************************************************************/
/* $XConsortium: suVHLine.s /main/5 1996/02/21 18:09:22 kaleb $ */

#include "assyntax.h"

	FILE("suVHLine.s")
	AS_BEGIN

#include "vgaAsm.h"

/*
	void SpeedUpHLine(dst, fill, len, screen);
	void SpeedUpVLine(dst, fill, len, screen);
*/

#define dst     REGOFF(8,EBP)
#define fill    REGOFF(12,EBP)
#define len	REGOFF(16,EBP)
#define screen  REGOFF(20,EBP)
/****************************************/
	SEG_DATA
copyright:
	STRING("Copyright 1992, 7/10/1993 by Glenn G. Lai")
	ALIGNDATA4
speedUpTop:
	D_LONG	0
lMaskTable:
	D_WORD	0x0f02
	D_WORD	0x0802
	D_WORD	0x0c02
	D_WORD	0x0e02
rMaskTable:
	D_WORD	0x0f02
	D_WORD	0x0102
	D_WORD	0x0302
	D_WORD	0x0702
segment:
	D_BYTE	0
/****************************************/
	SEG_TEXT
	ALIGNTEXT4
	GLOBL GLNAME(SpeedUpVLine)
	GLOBL GLNAME(SpeedUpVLine1)
GLNAME(SpeedUpVLine):
	MOV_L	(REGOFF(12,ESP), ECX)
	OR_L	(ECX, ECX)
	JZ	(return)
GLNAME(SpeedUpVLine1):
	MOV_L	(REGOFF(4,ESP), EAX)
	CMP_L	(VGABASE, EAX)
	JC	(pixmapV)
windowV:
	CMP_L	(CONST(1), ECX)
	JNE	(moreThanOnePoint)
	MOV_L	(EAX, ECX)
	SHR_L	(CONST(16), EAX)
	MOV_L	(CONST(0x3cd), EDX)
	OUT_B
	AND_L	(CONST(0xffff), ECX)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), ECX)
	MOV_L	(REGOFF(8,ESP), EAX)
	MOV_B	(AL, REGIND(ECX))
	RET
/****************************************/
	ALIGNTEXT4
moreThanOnePoint:
	PUSH_L 	(EBP)
	MOV_L 	(ESP, EBP)
	PUSH_L 	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)

	MOV_L	(screen, ESI)
	MOV_L	(ESI, EBX)
	DEC_L	(EBX)
	ADD_L	(CONTENT(GLNAME(vgaWriteTop)), EBX)
	MOV_L	(EBX, CONTENT(speedUpTop))

	MOV_L	(EAX, EDI)
	SHR_L	(CONST(16), EAX)
	MOV_B	(AL, CONTENT(segment))

	AND_L	(CONST(0xffff), EDI)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)

	MOV_L	(fill, EBX)
	MOV_L	(len, ECX)
wVLoop:
	MOV_L	(CONST(0x3cd), EDX)
	OUT_B

	XOR_L	(EDX, EDX)
	MOV_L	(CONTENT(speedUpTop), EAX)
	SUB_L	(EDI, EAX)
	DIV_L	(ESI)

	CMP_L	(ECX, EAX)
	JC	(wVLoop1)
	MOV_L	(ECX, EAX)
wVLoop1:
	SUB_L	(EAX, ECX)
wvLoop2:
	MOV_B	(BL, REGIND(EDI))
	ADD_L	(ESI, EDI)
	DEC_L	(EAX)
	JNZ	(wvLoop2)

	OR_L	(ECX, ECX)
	JZ	(npDone)

	SUB_L	(CONST(0x10000), EDI)
	MOV_B	(CONTENT(segment), AL)
	INC_B	(AL)
	MOV_B	(AL, CONTENT(segment))
	JMP	(wVLoop)
/*****************************************/
	ALIGNTEXT4
pixmapV:
	PUSH_L	(EDI)
	PUSH_L	(ESI)
	MOV_L	(REGOFF(16,ESP), EDX)
	MOV_L	(REGOFF(24,ESP), ESI)
	MOV_L	(ECX, EDI)
	AND_L	(CONST(3), EDI)
	MOV_L	(REGDIS(pixmapVTable,EDI,4), EDI)
	SHR_L	(CONST(2), ECX)
	JZ	(pixmapV4)
pixmapV5:
	MOV_B	(DL, REGIND(EAX))
	ADD_L	(ESI, EAX)
	MOV_B	(DL, REGIND(EAX))
	ADD_L	(ESI, EAX)
	MOV_B	(DL, REGIND(EAX))
	ADD_L	(ESI, EAX)
	MOV_B	(DL, REGIND(EAX))
	ADD_L	(ESI, EAX)
	DEC_L	(ECX)
	JNZ	(pixmapV5)
pixmapV4:
	JMP	(CODEPTR(EDI))
/*******************/
	SEG_DATA
	ALIGNDATA4
pixmapVTable:
	D_LONG	pixmapV0, pixmapV1, pixmapV2, pixmapV3
/*******************/
	SEG_TEXT
	ALIGNTEXT4
pixmapV3:
	MOV_B	(DL, REGIND(EAX))
	ADD_L	(ESI, EAX)
pixmapV2:
	MOV_B	(DL, REGIND(EAX))
	ADD_L	(ESI, EAX)
pixmapV1:
	MOV_B	(DL, REGIND(EAX))
pixmapV0:
	POP_L	(ESI)
	POP_L	(EDI)
	RET
/*****************************************/
	GLOBL	GLNAME(SpeedUpHLine)
	GLOBL	GLNAME(SpeedUpHLine1)
	ALIGNTEXT4
GLNAME(SpeedUpHLine):
	MOV_L	(REGOFF(12,ESP), ECX)
	OR_L	(ECX, ECX)
	JZ	(return)
GLNAME(SpeedUpHLine1):
	MOV_L	(REGOFF(4,ESP), EAX)
	SUB_L	(VGABASE, EAX)
	JC	(pixmapH)
windowH:
	CMP_L	(CONST(1), ECX)
	JNE	(windowH1)
	MOV_L	(EAX, ECX)
	SHR_L	(CONST(16), EAX)
	MOV_W	(CONST(0x3cd), DX)
	OUT_B
	AND_L	(CONST(0xffff), ECX)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), ECX)
	MOV_L	(REGOFF(8,ESP), EAX)
	MOV_B	(AL, REGIND(ECX))
	RET
/****************/
	ALIGNTEXT4
windowH1:
/* Change THRESHOLD at YOUR OWN RISK!!! */ 
#define THRESHOLD	CONST(80)
	CMP_L	(THRESHOLD, ECX)
	JNC	(pMode)
windowH8:
	PUSH_L	(EDI)
	MOV_L	(EAX, EDI)
	SHR_L	(CONST(16), EAX)
	MOV_B	(AL, CONTENT(segment))
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	AND_L	(CONST(0xffff), EDI)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
	MOV_L	(CONTENT(GLNAME(vgaWriteTop)), EAX)
	SUB_L	(EDI, EAX)

	XOR_L	(EDX, EDX)
	CMP_L	(ECX, EAX)
	JNC	(windowH7)

	MOV_L	(ECX, EDX)
	MOV_L	(EAX, ECX)
	SUB_L	(EAX, EDX)
windowH7:
	MOV_L	(REGOFF(12,ESP), EAX)
	TEST_L	(CONST(1), EDI)
	JZ	(windowH2)
	STOS_B
	DEC_L	(ECX)
	JZ	(windowH5)
windowH2:
	SHR_L	(CONST(1), ECX)
	JZ	(windowH3)
	REP
	STOS_W
	JNC	(windowH4)
windowH3:
	STOS_B
windowH4:
	OR_L	(EDX, EDX)
	JNZ	(windowH5)
	POP_L	(EDI)
	RET
	ALIGNTEXT4
windowH5:
	MOV_L	(EDX, ECX)
	MOV_B	(CONTENT(segment), AL)
	INC_B	(AL)
	MOV_L	(CONST(0x3cd), EDX)
	OUT_B
	MOV_L	(REGOFF(12,ESP), EAX)
	SUB_L	(CONST(0x10000), EDI)
	SHR_L	(CONST(1), ECX)
	JZ	(windowH6)
	REP
	STOS_W
	JNC	(windowH9)
windowH6:
	STOS_B
windowH9:
	POP_L	(EDI)
	RET
/****************************/
	ALIGNTEXT4
pixmapH:
	ADD_L	(VGABASE, EAX)
	CMP_L	(CONST(7), ECX)
	JC	(pixmapH0)

	PUSH_L	(EDI)
	MOV_L	(EAX, EDI)
	MOV_L	(REGOFF(12,ESP), EAX)

	TEST_L	(CONST(1), EDI)
	JZ	(pixmapH1)
	STOS_B
	DEC_L	(ECX)
pixmapH1:
	TEST_L	(CONST(2), EDI)
	JZ	(pixmapH2)
	STOS_W
	SUB_L	(CONST(2), ECX)
pixmapH2:
	MOV_L	(ECX, EDX)
	SHR_L	(CONST(2), ECX)
	REP
	STOS_L
	TEST_L	(CONST(2), EDX)
	JZ	(pixmapH4)
	STOS_W
pixmapH4:
	TEST_L	(CONST(1), EDX)
	JZ	(pixmapH5)
	MOV_B	(AL, REGIND(EDI))
pixmapH5:
	POP_L	(EDI)
	RET
/****************/
	ALIGNTEXT4
pixmapH0:
	MOV_L	(REGOFF(8,ESP), EDX)
pixmapH01:
	MOV_B	(DL, REGIND(EAX))
	INC_L	(EAX)
	DEC_L	(ECX)
	JNZ	(pixmapH01)
	RET
/****************************/
	ALIGNTEXT4
pMode:
	MOV_L	(EAX, EDX)
	AND_L	(CONST(0x3ffff), EDX)
	SUB_L	(CONST(0x40000), EDX)
	NEG_L	(EDX)
	CMP_L	(ECX, EDX)
	JC	(windowH8)

	PUSH_L 	(EBP)
	MOV_L 	(ESP,EBP)
	PUSH_L 	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)
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
	MOV_L	(fill, EBX)
	MOV_L 	(dst, EDI)
	MOV_L	(EDI, EAX)
	SHR_L	(CONST(18), EAX)
	DEC_W	(DX)
	OUT_B
	MOV_W	(CONST(0x3c4), DX)

	AND_L	(CONST(0x3ffff), EDI)
	MOV_L	(EDI, EAX)
	SHR_L	(CONST(2), EDI)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)

	MOV_L	(EAX, ESI)
	ADD_L	(CONST(3), ESI)
	AND_L	(CONST(0xfffffffc), ESI)
	SUB_L	(EAX, ESI)
	JZ	(pMode1)

	MOV_W	(REGDIS(lMaskTable,ESI,2), AX)
	OUT_W
	SUB_L	(ESI, ECX)
	MOV_B	(BL, REGIND(EDI))
	INC_L	(EDI)
pMode1:
	MOV_W	(CONST(0x0f02), AX)
	OUT_W

	MOV_L	(EBX, EAX)
	MOV_L	(ECX, ESI)
	AND_L	(CONST(3), ESI)
	SHR_L	(CONST(2), ECX)

	TEST_L	(CONST(1), EDI)
	JZ	(pMode2)
	STOS_B
	DEC_L	(ECX)
pMode2:
	SHR_L	(CONST(1), ECX)
	REP
	STOS_W
	JNC	(pMode3)
	STOS_B
pMode3:
	SHL_L	(CONST(1), ESI)
	JZ	(pMode4)
	MOV_W	(rMaskTable(ESI), AX)
	OUT_W
	MOV_B	(BL, REGIND(EDI))
	MOV_W	(CONST(0x0f02), AX)
	OUT_W
pMode4:
	MOV_W	(CONST(0x0c04), AX)
	OUT_W
npDone:
	POP_L 	(EBX)
	POP_L 	(ESI)
	POP_L 	(EDI)
	LEAVE
return:
	RET

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/enhanced/suBBlt2.s,v 3.2 1996/12/23 06:59:08 dawes Exp $ */
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
/* $XConsortium: suBBlt2.s /main/4 1996/02/21 18:09:05 kaleb $ */

#include "assyntax.h"

	FILE("suBitBlt2.s")
	AS_BEGIN

#include "vgaAsm.h"

/*  void PixWin(src, dst, height, width, sWidth, dWidth) */

#define src	REGOFF(8,EBP)
#define dst	REGOFF(12,EBP)
#define height	REGOFF(16,EBP)
#define width	REGOFF(20,EBP)
#define sWidth	REGOFF(24,EBP)
#define dWidth	REGOFF(28,EBP)

	SEG_DATA
copyright:
	STRING("Copyright 7/27/1992 by Glenn G. Lai")
	ALIGNDATA4
speedUpTop:
	D_LONG	0
sOffset:
	D_LONG	0
dOffset:
	D_LONG	0
allowance:
	D_LONG	0
lCount:
	D_LONG	0
mCount:
	D_LONG	0
rCount:
	D_LONG	0
heightCount:
	D_LONG	0
func:
	D_LONG	0
sPixWinTable:
	D_LONG	0, sPixWin1, sPixWin2, sPixWin3, sPixWin4
pixWinTable:
	D_LONG	pwM, pwMR, pwLM, pwLMR
segment:
	D_BYTE	0
/*************************************/
	SEG_TEXT
	ALIGNTEXT4
	GLOBL	GLNAME(PixWin)
GLNAME(PixWin):
	MOV_L	(REGOFF(12,ESP), EAX)
	OR_L	(REGOFF(16,ESP), EAX)
	JZ	(return)

	CLD
	PUSH_L	(EBP)
	MOV_L	(ESP, EBP)
	PUSH_L	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)

	MOV_L	(dWidth, EAX)
	MOV_L	(width, ECX)
	SUB_L	(ECX, EAX)
	MOV_L	(EAX, CONTENT(dOffset))
	ADD_L	(CONTENT(GLNAME(vgaWriteTop)), EAX)
	MOV_L	(EAX, CONTENT(speedUpTop))

	MOV_L	(sWidth, EAX)
	SUB_L	(ECX, EAX)
	MOV_L	(EAX, CONTENT(sOffset))
	
	MOV_L	(src, ESI)
	MOV_L	(dst, EDI)
	SUB_L	(VGABASE, EDI)
	CMP_L	(CONST(5), ECX)
	JNC	(pixWin1)
	MOV_L	(REGDIS(sPixWinTable,ECX,4), EAX)
	MOV_L	(EAX, CONTENT(func))
	JMP	(pixWin2)
/*************************************/
	ALIGNTEXT4
pixWin1:
	MOV_L	(ADDR(pixWinTable), EAX)
	TEST_L	(CONST(1), EDI)
	JZ	(pixWin3)
	ADD_L	(CONST(8), EAX)
	DEC_L	(ECX)
pixWin3:
	MOV_L	(ECX, EBX)
	AND_L	(CONST(3), EBX)
	JZ	(pixWin4)
	ADD_L	(CONST(4), EAX)
pixWin4:
	MOV_L	(REGIND(EAX), EAX)
	MOV_L	(EAX, CONTENT(func))
	MOV_L	(EBX, CONTENT(rCount))
	SHR_L	(CONST(2), ECX)
	MOV_L	(ECX, CONTENT(mCount))
pixWin2:
	MOV_L	(EDI, EAX)
	SHR_L	(CONST(16), EAX)
	MOV_B	(AL, CONTENT(segment))

	AND_L	(CONST(0xffff), EDI)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
	JMP	(pwLoop3)
/*************************************/
	ALIGNTEXT4
pwLoop:
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)), EDI)
	JC	(pwLoop1)
	SUB_L	(CONST(0x10000), EDI)
	MOV_B	(CONTENT(segment), AL)
	INC_B	(AL)
	MOV_B	(AL, CONTENT(segment))
pwLoop3:
	MOV_W	(CONST(0x3cd), DX)
	OUT_B
pwLoop1:
	MOV_L	(CONTENT(speedUpTop), EAX)
	SUB_L	(EDI, EAX)
	XOR_L	(EDX, EDX)
	DIV_L	(dWidth)

	OR_L	(EAX, EAX)
	JZ	(pwPartial)

	MOV_L	(height, EBX)
	CMP_L	(EBX, EAX)
	JC	(pwLoop2)
	MOV_L	(EBX, EAX)
pwLoop2:
	MOV_L	(EAX, CONTENT(allowance))
	MOV_L	(EAX, EDX)
	MOV_L	(CONTENT(sOffset), EAX)
	MOV_L	(CONTENT(dOffset), EBX)

	CALL	(VARINDIRECT(func))

	MOV_L	(height, EAX)
	SUB_L	(CONTENT(allowance), EAX)
	MOV_L	(EAX, height)
	JNZ	(pwLoop)

	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
return:
	RET
/********************************/
	ALIGNTEXT4
pwM:
wpM:
	MOV_L	(CONTENT(mCount), ECX)
	REP
	MOVS_L
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(pwM)
	RET
/********************************/
	ALIGNTEXT4
pwMR:
wpMR:
	MOV_L	(CONTENT(mCount), ECX)
	REP
	MOVS_L
	MOV_L	(CONTENT(rCount), ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(pwMR)
	RET
/********************************/
	ALIGNTEXT4
pwLM:
wpLM:
	MOVS_B
	MOV_L	(CONTENT(mCount), ECX)
	REP
	MOVS_L
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(pwLM)
	RET
/********************************/
	ALIGNTEXT4
pwLMR:
wpLMR:
	MOVS_B
	MOV_L	(CONTENT(mCount), ECX)
	REP
	MOVS_L
	MOV_L	(CONTENT(rCount), ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(pwLMR)
	RET
/********************************/
pwPartial:
	SUB_L	(CONTENT(dOffset), EDX)
	MOV_L	(EDX, ECX)
	MOV_L	(width, EBX)
	SUB_L	(ECX, EBX)

	TEST_L	(CONST(1), EDI)
	JZ	(pwPartial1)
	MOVS_B
	DEC_L	(ECX)
	JZ	(pwPartial2)
pwPartial1:
	SHR_L	(CONST(1), ECX)
	REP
	MOVS_W
pwPartial2:
	SUB_L	(CONST(0x10000), EDI)
	MOV_B	(CONTENT(segment), AL)
	INC_B	(AL)
	MOV_B	(AL, CONTENT(segment))
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	MOV_L	(EBX, ECX)
	SHR_L	(CONST(1), ECX)
	JZ	(pwPartial3)
	REP
	MOVS_W
	JNC	(pwPartial4)
pwPartial3:
	MOVS_B
pwPartial4:
	ADD_L	(CONTENT(sOffset), ESI)
	ADD_L	(CONTENT(dOffset), EDI)
	DEC_L	(height)
	JNZ	(pwLoop1)

	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/********************************/
	ALIGNTEXT4
sPixWin1:
sWinPix1:
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(sPixWin1)
	RET
/********************************/
	ALIGNTEXT4
sPixWin2:
sWinPix2:
	MOVS_W
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(sPixWin2)
	RET
/********************************/
	ALIGNTEXT4
sPixWin3:
sWinPix3:
	TEST_L	(CONST(1), EDI)
	JNZ	(sPixWin31)
sPixWin32:
	MOVS_W
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(sPixWin32)
	RET
/********************************/
	ALIGNTEXT4
sPixWin31:
sWinPix31:
	MOVS_B
	MOVS_W
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(sPixWin31)
	RET
/********************************/
	ALIGNTEXT4
sPixWin4:
sWinPix4:
	MOVS_L
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(sPixWin4)
	RET
/********************************/
	ALIGNTEXT4
winPixDone:
pixWinDone:
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET

/*  void WinPix(src, dst, h, w, srcOffset, dstOffset) */
	SEG_DATA
	ALIGNDATA4
sWinPixTable:
	D_LONG	0, sWinPix1, sWinPix2, sWinPix3, sWinPix4
winPixTable:
	D_LONG	wpM, wpMR, wpLM, wpLMR
/********************************/
	SEG_TEXT
	ALIGNTEXT4
	GLOBL	GLNAME(WinPix)
GLNAME(WinPix):
	MOV_L	(REGOFF(12,ESP), EAX)
	OR_L	(REGOFF(16,ESP), EAX)
	JZ	(return)

	CLD
	PUSH_L	(EBP)
	MOV_L	(ESP, EBP)
	PUSH_L	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)

	MOV_L	(sWidth, EAX)
	MOV_L	(width, ECX)
	SUB_L	(ECX, EAX)
	MOV_L	(EAX, CONTENT(sOffset))
	ADD_L	(CONTENT(GLNAME(vgaWriteTop)), EAX)
	MOV_L	(EAX, CONTENT(speedUpTop))

	MOV_L	(dWidth, EAX)
	SUB_L	(ECX, EAX)
	MOV_L	(EAX, CONTENT(dOffset))
	
	MOV_L	(src, ESI)
	MOV_L	(dst, EDI)
	SUB_L	(VGABASE, ESI)
	CMP_L	(CONST(5), ECX)
	JNC	(winPix1)
	MOV_L	(REGDIS(sWinPixTable,ECX,4), EAX)
	MOV_L	(EAX, CONTENT(func))
	JMP	(winPix2)
/********************************/
	ALIGNTEXT4
winPix1:
	MOV_L	(ADDR(winPixTable), EAX)
	TEST_L	(CONST(1), ESI)
	JZ	(winPix3)
	ADD_L	(CONST(8), EAX)
	DEC_L	(ECX)
winPix3:
	MOV_L	(ECX, EBX)
	AND_L	(CONST(3), EBX)
	JZ	(winPix4)
	ADD_L	(CONST(4), EAX)
winPix4:
	MOV_L	(REGIND(EAX), EAX)
	MOV_L	(EAX, CONTENT(func))
	MOV_L	(EBX, CONTENT(rCount))
	SHR_L	(CONST(2), ECX)
	MOV_L	(ECX, CONTENT(mCount))
winPix2:
	MOV_L	(ESI, EAX)
	SHR_L	(CONST(16), EAX)
	SHL_B	(CONST(4), AL)
	MOV_B	(AL, CONTENT(segment))

	AND_L	(CONST(0xffff), ESI)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), ESI)
	JMP	(wpLoop3)
/********************************/
	ALIGNTEXT4
wpLoop:
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)), ESI)
	JC	(wpLoop1)
	SUB_L	(CONST(0x10000), ESI)
	MOV_B	(CONTENT(segment), AL)
	ADD_B	(CONST(16), AL)
	MOV_B	(AL, CONTENT(segment))
wpLoop3:
	MOV_W	(CONST(0x3cd), DX)
	OUT_B
wpLoop1:
	MOV_L	(CONTENT(speedUpTop), EAX)
	SUB_L	(ESI, EAX)
	XOR_L	(EDX, EDX)
	DIV_L	(sWidth)

	OR_L	(EAX, EAX)
	JZ	(wpPartial)

	MOV_L	(height, EBX)
	CMP_L	(EBX, EAX)
	JC	(wpLoop2)
	MOV_L	(EBX, EAX)
wpLoop2:
	MOV_L	(EAX, CONTENT(allowance))
	MOV_L	(EAX, EDX)
	MOV_L	(CONTENT(sOffset), EAX)
	MOV_L	(CONTENT(dOffset), EBX)

	CALL	(VARINDIRECT(func))

	MOV_L	(height, EAX)
	SUB_L	(CONTENT(allowance), EAX)
	MOV_L	(EAX, height)
	JNZ	(wpLoop)

	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/********************************/
wpPartial:
	SUB_L	(CONTENT(sOffset), EDX)
	MOV_L	(EDX, ECX)
	MOV_L	(width, EBX)
	SUB_L	(ECX, EBX)

	TEST_L	(CONST(1), ESI)
	JZ	(wpPartial1)
	MOVS_B
	DEC_L	(ECX)
	JZ	(wpPartial2)
wpPartial1:
	SHR_L	(CONST(1), ECX)
	REP
	MOVS_W
wpPartial2:
	SUB_L	(CONST(0x10000), ESI)
	MOV_B	(CONTENT(segment), AL)
	ADD_B	(CONST(16), AL)
	MOV_B	(AL, CONTENT(segment))
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	MOV_L	(EBX, ECX)
	SHR_L	(CONST(1), ECX)
	JZ	(wpPartial3)
	REP
	MOVS_W
	JNC	(wpPartial4)
wpPartial3:
	MOVS_B
wpPartial4:
	ADD_L	(CONTENT(sOffset), ESI)
	ADD_L	(CONTENT(dOffset), EDI)
	DEC_L	(height)
	JNZ	(wpLoop1)

	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/********************************/
	SEG_DATA
	ALIGNDATA4
sPixPixTable:
	D_LONG	0, sPixPix1, sPixPix2, sPixPix3, sPixPix4
/********************************/
	SEG_TEXT
	ALIGNTEXT4
/*  void PixPix(src, dst, h, w, srcOffset, dstOffset, ydir) */
	GLOBL	GLNAME(PixPix)
GLNAME(PixPix):
	MOV_L	(REGOFF(12,ESP), EAX)
	OR_L	(REGOFF(16,ESP), EAX)
	JZ	(return)
	PUSH_L	(EBP)
	MOV_L	(ESP, EBP)
	PUSH_L	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)
	MOV_L	(REGOFF(8,EBP), ESI)
	MOV_L	(REGOFF(12,EBP), EDI)
	CMP_L	(ESI, EDI)
	JZ	(pixPixDone)
	MOV_L	(REGOFF(20,EBP), EDX)
	MOV_L	(REGOFF(28,EBP), EBX)
	CMP_L	(CONST(4), EDX)
	JLE	(sPixPix)
	MOV_L	(REGOFF(16,EBP), EAX)
	MOV_L	(EAX, CONTENT(heightCount))
	CMP_L	(CONST(1), REGOFF(32,EBP))
	JNE	(pixPixRL)
	CLD
	MOV_L	(ESI, EAX)
	ADD_L	(CONST(3), EAX)
	AND_L	(CONST(0xfffffffc), EAX)
	SUB_L	(ESI, EAX)
	MOV_L	(EAX, CONTENT(lCount))
	SUB_L	(EAX, EDX)
	MOV_L	(EDX, EAX)
	SAR_L	(CONST(2), EDX)
	JZ	(pixPixLRLR)
	AND_L	(CONST(3), EAX)
	JZ	(pixPixLRLMorM)
	MOV_L	(EAX, CONTENT(rCount))
	MOV_L	(REGOFF(24,EBP), EAX)
	CMP_L	(CONST(0), CONTENT(lCount))
	JE	(pixPixLRMR)
/********************************/
pixPixLRLMR:
	MOV_L	(CONTENT(lCount), ECX)
	REP
	MOVS_B
	MOV_L	(EDX, ECX)
	REP
	MOVS_L
	MOV_L	(CONTENT(rCount), ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(heightCount))
	JNZ	(pixPixLRLMR)
pixPixDone:
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/********************************/
	ALIGNTEXT4
pixPixLRMR:
	MOV_L	(EDX, ECX)
	REP
	MOVS_L
	MOV_L	(CONTENT(rCount), ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(heightCount))
	JNZ	(pixPixLRMR)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/********************************/
	ALIGNTEXT4
pixPixLRLMorM:
	MOV_L	(REGOFF(24,EBP), EAX)
	CMP_L	(CONST(0), CONTENT(lCount))
	JE	(pixPixLRM)
pixPixLRLM:
	MOV_L	(CONTENT(lCount), ECX)
	REP
	MOVS_B
	MOV_L	(EDX, ECX)
	REP
	MOVS_L
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(heightCount))
	JNZ	(pixPixLRLM)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET	
/********************************/
	ALIGNTEXT4
pixPixLRM:
	MOV_L	(EDX, ECX)
	REP
	MOVS_L
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(heightCount))
	JNZ	(pixPixLRM)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/********************************/
	ALIGNTEXT4
pixPixLRLR:
	MOV_L	(REGOFF(24,EBP), EAX)
	MOV_L	(REGOFF(20,EBP), EDX)
	SUB_L	(CONST(4), EDX)
pixPixLRLR1:
	MOVS_L
	MOV_L	(EDX, ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(heightCount))
	JNZ	(pixPixLRLR1)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/********0************************/
	ALIGNTEXT4
pixPixRL:
	STD
	MOV_L	(ESI, EAX)
	INC_L	(EAX)
	AND_L	(CONST(3), EAX)
	MOV_L	(EAX, CONTENT(rCount))
	SUB_L	(EAX, EDX)
	MOV_L	(EDX, EAX)
	SAR_L	(CONST(2), EDX)
	JZ	(pixPixRLLR)
	AND_L	(CONST(3), EAX)
	JZ	(pixPixRLMRorM)
	MOV_L	(EAX, CONTENT(lCount))
	MOV_L	(REGOFF(24,EBP), EAX)
	CMP_L	(CONST(0), CONTENT(rCount))
	JE	(pixPixRLLM)
pixPixRLLMR:
	MOV_L	(CONTENT(rCount), ECX)
	REP
	MOVS_B
	SUB_L	(CONST(3), ESI)
	SUB_L	(CONST(3), EDI)
	MOV_L	(EDX, ECX)
	REP
	MOVS_L
	ADD_L	(CONST(3), ESI)
	ADD_L	(CONST(3), EDI)
	MOV_L	(CONTENT(lCount), ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(heightCount))
	JNZ	(pixPixRLLMR)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	CLD
	LEAVE
	RET	
/********************************/
	ALIGNTEXT4
pixPixRLLM:
	SUB_L	(CONST(3), ESI)
	SUB_L	(CONST(3), EDI)
	SUB_L	(CONST(3), EAX)
	SUB_L	(CONST(3), EBX)
pixPixRLLM1:
	MOV_L	(EDX, ECX)
	REP
	MOVS_L
	MOV_L	(CONTENT(lCount), ECX)
	ADD_L	(CONST(3), ESI)
	ADD_L	(CONST(3), EDI)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(heightCount))
	JNZ	(pixPixRLLM1)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	CLD
	LEAVE
	RET		
/********************************/
	ALIGNTEXT4
pixPixRLMRorM:
	MOV_L	(REGOFF(24,EBP), EAX)
	CMP_L	(CONST(0), CONTENT(rCount))
	JE	(pixPixRLM)
	ADD_L	(CONST(3), EAX)
	ADD_L	(CONST(3), EBX)
pixPixRLMR:
	MOV_L	(CONTENT(rCount), ECX)
	REP
	MOVS_B
	SUB_L	(CONST(3), ESI)
	SUB_L	(CONST(3), EDI)
	MOV_L	(EDX, ECX)
	REP
	MOVS_L
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(heightCount))
	JNZ	(pixPixRLMR)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	CLD
	LEAVE
	RET
/********************************/
	ALIGNTEXT4
pixPixRLM:
	SUB_L	(CONST(3), ESI)
	SUB_L	(CONST(3), EDI)
pixPixRLM1:
	MOV_L	(EDX, ECX)
	REP
	MOVS_L
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(heightCount))
	JNZ	(pixPixRLM1)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	CLD
	LEAVE
	RET	
/********************************/
	ALIGNTEXT4
pixPixRLLR:
	MOV_L	(REGOFF(24,EBP), EAX)
	MOV_L	(REGOFF(20,EBP), EDX)
pixPixRLLR1:
	MOV_L	(EDX, ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(heightCount))
	JNZ	(pixPixRLLR1)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	CLD
	LEAVE
	RET
/********************************/
	ALIGNTEXT4
sPixPix:
	CLD
	MOV_L	(REGOFF(16,EBP), ECX)
	MOV_L	(REGOFF(24,EBP), EAX)
	JMP	(CODEPTR(REGDIS(sPixPixTable,EDX,4)))
/********************************/
	ALIGNTEXT4
sPixPix4:
	MOVS_L
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(ECX)
	JNZ	(sPixPix4)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/********************************/
	ALIGNTEXT4
sPixPix3:
	TEST_L	(CONST(1), ESI)
	JNZ	(sPixPix3U)
	INC_L	(EAX)
	INC_L	(EBX)
sPixPix31:
	MOV_B	(REGOFF(2,ESI), DL)
	MOVS_W
	MOV_B	(DL, REGIND(EDI))
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(ECX)
	JNZ	(sPixPix31)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/********************************/
	ALIGNTEXT4
sPixPix3U:
	ADD_L	(CONST(2), EAX)
	ADD_L	(CONST(2), EBX)
sPixPix3U1:
	MOV_W	(REGOFF(1,ESI), DX)
	MOVS_B
	MOV_W	(DX, REGIND(EDI))
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(ECX)
	JNZ	(sPixPix3U1)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/********************************/
	ALIGNTEXT4
sPixPix2:
	MOVS_W
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(ECX)
	JNZ	(sPixPix2)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/********************************/
	ALIGNTEXT4
sPixPix1:
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(ECX)
	JNZ	(sPixPix1)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET

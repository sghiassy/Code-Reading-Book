/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/enhanced/suBitBlt.s,v 3.2 1996/12/23 06:59:09 dawes Exp $ */
/*******************************************************************************
			Copyr 1992 by Glenn G. Lai 

                        All Rs Reserved

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
(glenn@cs.utexas.edu)
8/17/92
*******************************************************************************/
/* $XConsortium: suBitBlt.s /main/5 1996/02/21 18:09:09 kaleb $ */

#include "assyntax.h"

	FILE("suBitBlt.s")
	AS_BEGIN

#include "vgaAsm.h"

/*  WinWin(src, dst, h, w, xdir, ydir, screen, special) */

#define src	REGOFF(8,EBP)
#define dst	REGOFF(12,EBP)
#define height	REGOFF(16,EBP)
#define width	REGOFF(20,EBP)
#define xdir	REGOFF(24,EBP)
#define ydir	REGOFF(28,EBP)
#define screen	REGOFF(32,EBP)
#define special	REGOFF(36,EBP)

	SEG_DATA
copyright:
	STRING("Copyright 8/17/1992 by Glenn G. Lai")
	ALIGNDATA4
tmp:
	D_LONG	0
speedUpBound:
	D_LONG	0
sM:
	D_LONG	0
dM:
	D_LONG	0
sM1:
	D_LONG	0
dM1:
	D_LONG	0
func:
	D_LONG	0
partial:
	D_LONG	0
npPartialJump:
	D_LONG	0
pPartialJump:
	D_LONG	0
pMOffset:
	D_LONG	0
pLROffset:
	D_LONG	0
allowance:
	D_LONG	0
lCount:
	D_LONG	0
mCount:
	D_LONG	0
rCount:
	D_LONG	0
hCount:
	D_LONG	0
offset:
	D_LONG	0
wwLRTable:
	D_LONG	wBoxLR, wbBoxLR, bwBoxLR, bBoxLR
wwRLTable:
	D_LONG	bBoxRL, bwBoxRL, wbBoxRL, wBoxRL
pTable:
        D_LONG  pM, pMR, pLM, pLMR
pTable1:
        D_LONG  pM, pMRa, pLMa, pLMRa
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

	SEG_TEXT
	ALIGNTEXT4
	GLOBL GLNAME(WinWin)
#define	THRESHOLD CONST(10)
GLNAME(WinWin):
	MOV_L	(REGOFF(4,ESP), EAX)
	CMP_L	(REGOFF(8,ESP), EAX)
	JE	(return)

	MOV_L	(REGOFF(16,ESP), EAX)
	OR_L	(REGOFF(20,ESP), EAX)
	JZ	(return)

	PUSH_L	(EBP)
	MOV_L	(ESP, EBP)
	PUSH_L	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)
	CLD

	MOV_L	(width, ECX)
	MOV_L	(src, ESI)
	MOV_L	(dst, EDI)

	CMP_L	(CONST(1), special)
	JE	(nP)

	MOV_L	(ESI, EAX)
	MOV_L	(EDI, EBX)
	SUB_L	(EBX, EAX)
	AND_L	(CONST(3), EAX)
	JNZ	(nP)

	CMP_L	(THRESHOLD, ECX)
	JNC	(pMode)
nP:
	MOV_L	(ESI, EAX)
	SHR_L	(CONST(12), EAX)
	AND_B	(CONST(0x0f0), AL)
	MOV_L	(EDI, EBX)
	SHR_L	(CONST(16), EBX)
	OR_B	(BL, AL)
	MOV_B	(AL, CONTENT(segment))

	AND_L	(CONST(0xffff), ESI)
	AND_L	(CONST(0xffff), EDI)
	ADD_L	(CONTENT(GLNAME(vgaReadBottom)), ESI)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)

	MOV_L	(ADDR(wwLRTable), EAX)
	CMP_L	(CONST(1), special)
	JNE	(wwLeftToRight)
	STD
	MOV_L	(ADDR(wwRLTable), EAX)
wwLeftToRight:
	TEST_L	(CONST(1), ESI)
	JZ	(ww3)
	ADD_L	(CONST(8), EAX)
ww3:
	TEST_L	(CONST(1), EDI)
	JZ	(ww4)
	ADD_L	(CONST(4), EAX)
ww4:
	CMP_L	(CONST(6), ECX)
	JNC	(ww2)

	MOV_L	(ADDR(sWW), CONTENT(func))
	JMP	(ww5)
	ALIGNTEXT4
ww2:
	MOV_L	(REGIND(EAX), EAX)
	MOV_L	(EAX, CONTENT(func))
ww5:
	CMP_L	(CONST(1), special)
	JNE	(npUpOrDown)
npRL:
	MOV_L	(screen, EAX)
	NEG_L	(EAX)
	ADD_L	(ECX, EAX)
	MOV_L	(EAX, CONTENT(offset))
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), EAX)
	DEC_L	(EAX)
	MOV_L	(EAX, CONTENT(speedUpBound))
	MOV_B	(CONTENT(segment), AL)
	JMP	(npRL2)
/***************************/
        ALIGNTEXT4
npRL5:
	MOV_B	(CONTENT(segment), AL)
	CMP_L	(CONTENT(GLNAME(vgaReadBottom)), ESI)
	JNC	(npRL1)
	SUB_B	(CONST(16), AL)
	ADD_L	(CONST(0x10000), ESI)
npRL1:
	CMP_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
	JNC	(npRL7)
	DEC_B	(AL)
	ADD_L	(CONST(0x10000), EDI)
npRL7:
	MOV_B	(AL, CONTENT(segment))
npRL2:
	MOV_W	(CONST(0x3cd), DX)
	OUT_B
npRL6:
	MOV_L	(ESI, EAX)
	MOV_L	(EDI, EBX)
	SUB_L	(CONTENT(speedUpBound), EAX)
	SUB_L	(CONTENT(speedUpBound), EBX)

	MOV_L	(screen, ECX)
	XOR_L	(EDX, EDX)
	DIV_L	(ECX)
	XCHG_L	(EAX, EBX)

	XOR_L	(EDX, EDX)
	DIV_L	(ECX)
	
	CMP_L	(EBX, EAX)
	JC	(npRL3)

	MOV_L	(EBX, EAX)
npRL3:
	OR_L	(EAX, EAX)
	JZ	(npRLPartial)

	MOV_L	(height, EBX)
	CMP_L	(EBX, EAX)
	JC	(npRL4)

	MOV_L	(EBX, EAX)
npRL4:
	SUB_L	(EAX, EBX)
	MOV_L	(EBX, height)

	MOV_L	(CONTENT(offset), EBX)
	MOV_L	(width, ECX)
	MOV_L	(EAX, EDX)
	CALL	(VARINDIRECT(func))

	CMP_L	(CONST(0), height)
	JNE	(npRL5)

        CLD
        POP_L	(EBX)
        POP_L	(ESI)
        POP_L	(EDI)
        LEAVE
return:
        RET
/***************************/
        ALIGNTEXT4
npRLPartial:
	MOV_L	(width, EBX)
	MOV_W	(CONST(0x3cd), DX)
npRLPartial5:
	MOV_L	(ESI, EAX)
	MOV_L	(EDI, ECX)
	SUB_L	(CONTENT(GLNAME(vgaReadBottom)), EAX)
	SUB_L	(CONTENT(GLNAME(vgaWriteBottom)), ECX)

	CMP_L	(EAX, ECX)
	JC	(npRLPartial1)

	MOV_L	(EAX, ECX)
npRLPartial1:
	INC_L	(ECX)
	CMP_L	(EBX, ECX)
	JC	(npRLPartial2)

	MOV_L	(EBX, ECX)
npRLPartial2:
	SUB_L	(ECX, EBX)

	REP
	MOVS_B

	MOV_B	(CONTENT(segment), AL)
	CMP_L	(CONTENT(GLNAME(vgaReadBottom)), ESI)
	JNC	(npRLPartial3)

	ADD_L	(CONST(0x10000), ESI)
	SUB_B	(CONST(16), AL)
npRLPartial3:
	CMP_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
	JNC	(npRLPartial4)

	ADD_L	(CONST(0x10000), EDI)
	DEC_B	(AL)
npRLPartial4:
	MOV_B	(AL, CONTENT(segment))
	OUT_B

	OR_L	(EBX, EBX)
	JNZ	(npRLPartial5)

	MOV_L	(CONTENT(offset), EAX)
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	
	DEC_L	(height)
	JNZ	(npRL5)

        CLD
        POP_L	(EBX)
        POP_L	(ESI)
        POP_L	(EDI)
        LEAVE
        RET
/***************************/
npUpOrDown:
	CMP_L	(CONST(1), ydir)
	JE	(npDown)
npUp:
	MOV_L	(screen, EAX)
	MOV_L	(EAX, EBX)
	NEG_L	(EBX)
	MOV_L	(EBX, screen)
	MOV_L	(width, ECX)
	SUB_L	(ECX, EAX)
	MOV_L	(EAX, CONTENT(offset))
	MOV_L	(ADDR(npUp3), CONTENT(npPartialJump))
        MOV_B	(CONTENT(segment), AL)
	JMP	(npUp5)
/***************************/
        ALIGNTEXT4
npUp3:
        MOV_B	(CONTENT(segment), AL)
        CMP_L	(CONTENT(GLNAME(vgaReadBottom)), ESI)
        JNC     (npUp4)
        SUB_B	(CONST(16), AL)
	ADD_L	(CONST(0x10000), ESI)
npUp4:
        CMP_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
        JNC     (npUp6)
        DEC_B	(AL)
	ADD_L	(CONST(0x10000), EDI)
npUp6:
	MOV_B	(AL, CONTENT(segment))
npUp5:
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	MOV_L	(width, EBX)
	MOV_L	(CONTENT(GLNAME(vgaReadTop)), EAX)
	SUB_L	(ESI, EAX)
	CMP_L	(EBX, EAX)
	JC	(npPartial)

	MOV_L	(CONTENT(GLNAME(vgaWriteTop)), EAX)
	SUB_L	(EDI, EAX)
	CMP_L	(EBX, EAX)
	JC	(npPartial)

	MOV_L	(ESI, EAX)
	MOV_L	(EDI, EBX)
	SUB_L	(CONTENT(GLNAME(vgaReadBottom)), EAX)
	SUB_L	(CONTENT(GLNAME(vgaWriteBottom)), EBX)

	MOV_L	(screen, ECX)
	XOR_L	(EDX, EDX)
	DIV_L	(ECX)
	XCHG_L	(EAX, EBX)

	XOR_L	(EDX, EDX)
	DIV_L	(ECX)

	CMP_L	(EBX, EAX)
	JC	(npUp1)

	MOV_L	(EBX, EAX)
npUp1:
	INC_L	(EAX)
	MOV_L	(height, EBX)
	CMP_L	(EBX, EAX)
	JC	(npUp2)

	MOV_L	(EBX, EAX)
npUp2:
	SUB_L	(EAX, EBX)
	MOV_L	(EBX, height)

	MOV_L	(CONTENT(offset), EBX)
	MOV_L	(width, ECX)
	MOV_L	(EAX, EDX)
	CALL	(VARINDIRECT(func))

	CMP_L	(CONST(0), height)
	JNE	(npUp3)

	CLD
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/***************************/
	ALIGNTEXT4
npDown:
	MOV_L	(screen, EAX)
	SUB_L	(width, EAX)
	MOV_L	(EAX, CONTENT(offset))
	ADD_L	(CONTENT(GLNAME(vgaWriteTop)), EAX)
	MOV_L	(EAX, CONTENT(speedUpBound))
	MOV_L	(ADDR(npDown3), CONTENT(npPartialJump))
	MOV_B	(CONTENT(segment), AL)
	JMP	(npDown5)
/***************************/
	ALIGNTEXT4
npDown3:
	MOV_B	(CONTENT(segment), AL)
	CMP_L	(CONTENT(GLNAME(vgaReadTop)), ESI)
	JC	(npDown4)
	ADD_B	(CONST(16), AL)
	SUB_L	(CONST(0x10000), ESI)
npDown4:
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)), EDI)
	JC	(npDown6)
	INC_B	(AL)
	SUB_L	(CONST(0x10000), EDI)
npDown6:
	MOV_B	(AL, CONTENT(segment))
npDown5:
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	MOV_L	(CONTENT(speedUpBound), EAX)
	MOV_L	(CONTENT(speedUpBound), EBX)
	SUB_L	(ESI, EAX)
	SUB_L	(EDI, EBX)

	MOV_L	(screen, ECX)
	XOR_L	(EDX, EDX)
	DIV_L	(ECX)
	XCHG_L	(EAX, EBX)

	XOR_L	(EDX, EDX)
	DIV_L	(ECX)

	CMP_L	(EBX, EAX)
	JC	(npDown1)

	MOV_L	(EBX, EAX)
npDown1:
	OR_L	(EAX, EAX)
	JZ	(npPartial)

	MOV_L	(height, EBX)
	CMP_L	(EBX, EAX)
	JC	(npDown2)

	MOV_L	(EBX, EAX)
npDown2:
	SUB_L	(EAX, EBX)
	MOV_L	(EBX, height)

	MOV_L	(CONTENT(offset), EBX)
	MOV_L	(width, ECX)
	MOV_L	(EAX, EDX)
	CALL	(VARINDIRECT(func))

	CMP_L	(CONST(0), height)
	JNE	(npDown3)

	CLD
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/***************************/
	ALIGNTEXT4
bBoxLR:
	DEC_L	(ECX)
	MOV_L	(ECX, EAX)
	SHR_L	(CONST(1), EAX)
	AND_L	(CONST(1), ECX)
	JNZ	(bBoxLRR)
bBoxLR1:
	MOVS_B
	MOV_L	(EAX, ECX)
	REP
	MOVS_W
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(bBoxLR1)
	RET
/***************************/
	ALIGNTEXT4
bBoxLRR:
	MOVS_B
	MOV_L	(EAX, ECX)
	REP
	MOVS_W
	MOVS_B
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(bBoxLRR)
	RET
/***************************/
	ALIGNTEXT4
wBoxLR:
	MOV_L	(ECX, EAX)
	SHR_L	(CONST(1), EAX)
	AND_L	(CONST(1), ECX)
	JNZ	(wBoxLRR)
wBoxLR1:
	MOV_L	(EAX, ECX)
	REP
	MOVS_W
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(wBoxLR1)
	RET
/***************************/
	ALIGNTEXT4
wBoxLRR:
	MOV_L	(EAX, ECX)
	REP
	MOVS_W
	MOVS_B
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(wBoxLRR)
	RET
/***************************/
	ALIGNTEXT4
bwBoxLR:
	MOV_L	(EDX, CONTENT(hCount))
	DEC_L	(ECX)
	MOV_L	(ECX, EAX)
	SAR_L	(CONST(2), ECX)
	MOV_L	(ECX, CONTENT(mCount))
	AND_L	(CONST(3), EAX)
	JNZ	(bwBoxLRR)
bwBoxLR1:
	MOV_B	(REGIND(ESI), DL)
	INC_L	(ESI)
	MOV_L	(CONTENT(mCount), ECX)
bwBoxLR2:
	LODS_L
	ROL_L	(CONST(8), EAX)
	XCHG_B	(AL, DL)
	STOS_L
	DEC_L	(ECX)
	JNZ	(bwBoxLR2)
	MOV_B	(DL, REGIND(EDI))
	INC_L	(EDI)
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(hCount))
	JNZ	(bwBoxLR1)
	RET
/***************************/
	ALIGNTEXT4
bwBoxLRR:
	MOV_L	(EAX, CONTENT(rCount))
bwBoxLRR1:
	MOV_B	(REGIND(ESI), DL)
	INC_L	(ESI)
	MOV_L	(CONTENT(mCount), ECX)
bwBoxLRR2:
	LODS_L
	ROL_L	(CONST(8), EAX)
	XCHG_B	(AL, DL)
	STOS_L
	DEC_L	(ECX)
	JNZ	(bwBoxLRR2)
	MOV_B	(DL, REGIND(EDI))
	INC_L	(EDI)
	MOV_L	(CONTENT(rCount), ECX)
	REP
	MOVS_B
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(hCount))
	JNZ	(bwBoxLRR1)
	RET
/***************************/
	ALIGNTEXT4
wbBoxLR:
	MOV_L	(EDX, CONTENT(hCount))
	SUB_L	(CONST(2), ECX)
	MOV_L	(ECX, EAX)
	SHR_L	(CONST(2), ECX)
	MOV_L	(ECX, CONTENT(mCount))
	AND_L	(CONST(3), EAX)
	JNZ	(wbBoxLRR)
wbBoxLR2:
	LODS_W
	STOS_B
	MOV_B	(AH, DL)
	MOV_L	(CONTENT(mCount), ECX)
wbBoxLR1:
	LODS_L
	ROL_L	(CONST(8), EAX)
	XCHG_B	(AL, DL)
	STOS_L
	DEC_L	(ECX)
	JNZ	(wbBoxLR1)
	MOV_B	(DL, REGIND(EDI))
	INC_L	(EDI)
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(hCount))
	JNZ	(wbBoxLR2)
	RET
/***************************/
	ALIGNTEXT4
wbBoxLRR:
	MOV_L	(EAX, CONTENT(rCount))
wbBoxLRR1:
	LODS_W
	STOS_B
	MOV_B	(AH, DL)
	MOV_L	(CONTENT(mCount), ECX)
wbBoxLRR2:
	LODS_L
	ROL_L	(CONST(8), EAX)
	XCHG_B	(AL, DL)
	STOS_L
	DEC_L	(ECX)
	JNZ	(wbBoxLRR2)
	MOV_B	(DL, REGIND(EDI))
	INC_L	(EDI)
	MOV_L	(CONTENT(rCount), ECX)
	REP
	MOVS_B
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(hCount))
	JNZ	(wbBoxLRR1)
	RET
/***************************/
	ALIGNTEXT4
bBoxRL:
	DEC_L	(ECX)
	MOV_L	(ECX, EAX)
	SAR_L	(CONST(2), EAX)
	AND_L	(CONST(3), ECX)
	JNZ	(bBoxRLL)
	ADD_L	(CONST(3), EBX)
bBoxRL1:
	MOVS_B
	SUB_L	(CONST(3), ESI)
	SUB_L	(CONST(3), EDI)
	MOV_L	(EAX, ECX)
	REP
	MOVS_L
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(bBoxRL1)
	RET
/***************************/
	ALIGNTEXT4
bBoxRLL:
	MOV_L	(ECX, CONTENT(lCount))
bBoxRLL1:
	MOVS_B
	SUB_L	(CONST(3), ESI)
	SUB_L	(CONST(3), EDI)
	MOV_L	(EAX, ECX)
	REP
	MOVS_L
	ADD_L	(CONST(3), ESI)
	ADD_L	(CONST(3), EDI)
	MOV_L	(CONTENT(lCount), ECX)
	REP
	MOVS_B
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(bBoxRLL1)
	RET
/***************************/
	ALIGNTEXT4
wBoxRL:
	MOV_L	(ECX, EAX)
	SAR_L	(CONST(2), EAX)
	AND_L	(CONST(3), ECX)
	JNZ	(wBoxRLL)
	ADD_L	(CONST(3), EBX)
wBoxRL1:
	SUB_L	(CONST(3), ESI)
	SUB_L	(CONST(3), EDI)
	MOV_L	(EAX, ECX)
	REP
	MOVS_L
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(wBoxRL1)
	RET
/***************************/
	ALIGNTEXT4
wBoxRLL:
	MOV_L	(ECX, CONTENT(lCount))
wBoxRLL1:
	SUB_L	(CONST(3), ESI)
	SUB_L	(CONST(3), EDI)
	MOV_L	(EAX, ECX)
	REP
	MOVS_L
	ADD_L	(CONST(3), ESI)
	ADD_L	(CONST(3), EDI)
	MOV_L	(CONTENT(lCount), ECX)
	REP
	MOVS_B
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(wBoxRLL1)
	RET
/***************************/
	ALIGNTEXT4
bwBoxRL:
	MOV_L	(EDX, CONTENT(hCount))
	DEC_L	(ECX)
	MOV_L	(ECX, EAX)
	SAR_L	(CONST(2), ECX)
	MOV_L	(ECX, CONTENT(mCount))
	AND_L	(CONST(3), EAX)
	JNZ	(bwBoxRLL)
	ADD_L	(CONST(3), EBX)
bwBoxRL1:
	MOV_B	(REGIND(ESI), DL)
	SUB_L	(CONST(4), ESI)
	SUB_L	(CONST(3), EDI)
	MOV_L	(CONTENT(mCount), ECX)
bwBoxRL2:
	LODS_L
	XCHG_B	(AL, DL)
	ROR_L	(CONST(8), EAX)
	STOS_L
	DEC_L	(ECX)
	JNZ	(bwBoxRL2)
	MOV_B	(DL, REGOFF(3,EDI))
	DEC_L	(EDI)
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(hCount))
	JNZ	(bwBoxRL1)
	RET
/***************************/
	ALIGNTEXT4
bwBoxRLL:
	MOV_L	(EAX, CONTENT(lCount))
bwBoxRLL1:
	MOV_B	(REGIND(ESI), DL)
	SUB_L	(CONST(4), ESI)
	SUB_L	(CONST(3), EDI)
	MOV_L	(CONTENT(mCount), ECX)
bwBoxRLL2:
	LODS_L
	XCHG_B	(AL, DL)
	ROR_L	(CONST(8), EAX)
	STOS_L
	DEC_L	(ECX)
	JNZ	(bwBoxRLL2)
	MOV_B	(DL, REGOFF(3,EDI))
	ADD_L	(CONST(3), ESI)
	ADD_L	(CONST(2), EDI)
	MOV_L	(CONTENT(lCount), ECX)
	REP
	MOVS_B
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(hCount))
	JNZ	(bwBoxRLL1)
	RET
/***************************/
	ALIGNTEXT4
wbBoxRL:
	MOV_L	(EDX, CONTENT(hCount))
	SUB_L	(CONST(2), ECX)
	MOV_L	(ECX, EAX)
	SAR_L	(CONST(2), ECX)
	MOV_L	(ECX, CONTENT(mCount))
	AND_L	(CONST(3), EAX)
	JNZ	(wbBoxRLL)
	ADD_L	(CONST(3), EBX)
wbBoxRL1:
	DEC_L	(ESI)
	MOV_W	(REGIND(ESI), AX)
	MOV_B	(AH, REGIND(EDI))
	MOV_B	(AL, DL)
	SUB_L	(CONST(4), ESI)
	SUB_L	(CONST(4), EDI)
	MOV_L	(CONTENT(mCount), ECX)
wbBoxRL2:
	LODS_L
	XCHG_B	(AL, DL)
	ROR_L	(CONST(8), EAX)
	STOS_L
	DEC_L	(ECX)
	JNZ	(wbBoxRL2)
	MOV_B	(DL, REGOFF(3,EDI))
	DEC_L	(EDI)
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(hCount))
	JNZ	(wbBoxRL1)
	RET
/***************************/
	ALIGNTEXT4
wbBoxRLL:
	MOV_L	(EAX, CONTENT(lCount))
wbBoxRLL1:
	DEC_L	(ESI)
	MOV_W	(REGIND(ESI), AX)
	MOV_B	(AH, REGIND(EDI))
	MOV_B	(AL, DL)
	SUB_L	(CONST(4), ESI)
	SUB_L	(CONST(4), EDI)
	MOV_L	(CONTENT(mCount), ECX)
wbBoxRLL2:
	LODS_L
	XCHG_B	(AL, DL)
	ROR_L	(CONST(8), EAX)
	STOS_L
	DEC_L	(ECX)
	JNZ	(wbBoxRLL2)
	MOV_B	(DL, REGOFF(3,EDI))
	ADD_L	(CONST(3), ESI)
	ADD_L	(CONST(2), EDI)
	MOV_L	(CONTENT(lCount), ECX)
	REP
	MOVS_B
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(CONTENT(hCount))
	JNZ	(wbBoxRLL1)
	RET
/***************************/
	ALIGNTEXT4
npPartial:
	MOV_L	(width, EBX)
	MOV_W	(CONST(0x3cd), DX)
npPartial5:
	MOV_L	(CONTENT(GLNAME(vgaReadTop)), EAX)
	MOV_L	(CONTENT(GLNAME(vgaWriteTop)), ECX)
	SUB_L	(ESI, EAX)
	SUB_L	(EDI, ECX)

	CMP_L	(EAX, ECX)
	JC	(npPartial1)

	MOV_L	(EAX, ECX)
npPartial1:
	CMP_L	(EBX, ECX)
	JC	(npPartial2)

	MOV_L	(EBX, ECX)
npPartial2:
	SUB_L	(ECX, EBX)

	REP
	MOVS_B

	MOV_B	(CONTENT(segment), AL)
	CMP_L	(CONTENT(GLNAME(vgaReadTop)), ESI)
	JC	(npPartial3)

	SUB_L	(CONST(0x10000), ESI)
	ADD_B	(CONST(16), AL)
npPartial3:
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)), EDI)
	JC	(npPartial4)

	SUB_L	(CONST(0x10000), EDI)
	INC_B	(AL)
npPartial4:
	MOV_B	(AL, CONTENT(segment))
	OUT_B

	OR_L	(EBX, EBX)
	JNZ	(npPartial5)

	MOV_L	(CONTENT(offset), EAX)
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	
	DEC_L	(height)
	JZ	(npPartial6)
	JMP	(VARINDIRECT(npPartialJump))
	ALIGNTEXT4
npPartial6:
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/***************************/
	ALIGNTEXT4
sWW:
	MOV_L	(ECX, EAX)
sWW1:
	MOV_L	(EAX, ECX)
	REP
	MOVS_B
	ADD_L	(EBX, ESI)
	ADD_L	(EBX, EDI)
	DEC_L	(EDX)
	JNZ	(sWW1)
	RET
/***************************/
	ALIGNTEXT4
pMode:
	MOV_W	(CONST(0x0604), AX)
	MOV_W	(CONST(0x3c4), DX)
	OUT_W
	MOV_W	(CONST(0x4105), AX)
	MOV_W	(CONST(0x3ce), DX)
	OUT_W

	MOV_L	(ESI, EAX)
	MOV_L	(ADDR(pTable), EBX)
	MOV_L	(width, ECX)

	ADD_L	(CONST(3), EAX)
	AND_L	(CONST(0xfffffffc), EAX)
	SUB_L	(ESI, EAX)
	SUB_L	(EAX, ECX)

	XOR_L	(EDX, EDX)
	SAL_L	(CONST(1), EAX)
	JZ	(pNoL)

	ADD_L	(CONST(8), EBX)
	INC_L	(EDX)
pNoL:
	MOV_W	(lMaskTable(EAX), AX)
	MOV_W	(AX, CONTENT(lMask))
	
	MOV_L	(ECX, EAX)
	AND_L	(CONST(3), EAX)

	SAL_L	(CONST(1), EAX)
	JZ	(pNoR)
	
	ADD_L	(CONST(4), EBX)
	INC_L	(EDX)
pNoR:
	CMP_L	(CONST(1), xdir)
	JE	(pModeLR)
	ADD_L	(CONST(16), EBX)
pModeLR:
	MOV_L	(REGIND(EBX), EBX)
	MOV_L	(EBX, CONTENT(func))

	MOV_W	(rMaskTable(EAX), AX)
	MOV_W	(AX, CONTENT(rMask))

	SHR_L	(CONST(2), ECX)
	MOV_L	(ECX, CONTENT(mCount))

	ADD_L	(ECX, EDX)
	MOV_L	(EDX, width)

	MOV_L	(ESI, EAX)
	SHR_L	(CONST(14), EAX)
	AND_B	(CONST(0xf0), AL)
	MOV_L	(EDI, EBX)
	SHR_L	(CONST(18), EBX)
	OR_B	(BL, AL)
	MOV_B	(AL, CONTENT(segment))

	SHR_L	(CONST(2), ESI)
	AND_L	(CONST(0xffff), ESI)
	ADD_L	(CONTENT(GLNAME(vgaReadBottom)), ESI)
	SHR_L	(CONST(2), EDI)
	AND_L	(CONST(0xffff), EDI)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)

	MOV_L	(screen, EBX)
	SAR_L	(CONST(2), EBX)
	MOV_L	(EBX, EAX)

	DEC_L	(EBX)
	MOV_L	(EBX, CONTENT(pLROffset))

	INC_L	(EBX)
	SUB_L	(ECX, EBX)
	MOV_L	(EBX, CONTENT(pMOffset))
/***************************/
	CMP_L	(CONST(1), ydir)
	JE	(pDown)
pUp:
	MOV_L	(EAX, EBX)
	NEG_L	(EBX)
	MOV_L	(EBX, screen)
	SUB_L	(width, EAX)
	MOV_L	(EAX, CONTENT(offset))
	MOV_L	(ADDR(pUp3), CONTENT(pPartialJump))
	MOV_B	(CONTENT(segment), AL)
	JMP	(pUp5)
/***************************/
	ALIGNTEXT4
pUp3:
	MOV_B	(CONTENT(segment), AL)
	CMP_L	(CONTENT(GLNAME(vgaReadBottom)), ESI)
	JNC	(pUp4)
	SUB_B	(CONST(16), AL)
	ADD_L	(CONST(0x10000), ESI)
pUp4:
	CMP_L	(CONTENT(GLNAME(vgaWriteBottom)), EDI)
	JNC	(pUp6)
	DEC_B	(AL)
	ADD_L	(CONST(0x10000), EDI)
pUp6:
	MOV_B	(AL, CONTENT(segment))
pUp5:
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	MOV_L	(ESI, CONTENT(sM))
	MOV_L	(EDI, CONTENT(dM))

	MOV_L	(width, EBX)
	MOV_L	(CONTENT(GLNAME(vgaReadTop)), EAX)
	SUB_L	(ESI, EAX)
	CMP_L	(EBX, EAX)
	JC	(pPartial)

	MOV_L	(CONTENT(GLNAME(vgaWriteTop)), EAX)
	SUB_L	(EDI, EAX)
	CMP_L	(EBX, EAX)
	JC	(pPartial)

	MOV_L	(ESI, EAX)
	MOV_L	(EDI, EBX)
	SUB_L	(CONTENT(GLNAME(vgaReadBottom)), EAX)
	SUB_L	(CONTENT(GLNAME(vgaWriteBottom)), EBX)

	MOV_L	(screen, ECX)
	XOR_L	(EDX, EDX)
	DIV_L	(ECX)
	XCHG_L  (EAX, EBX)

	XOR_L	(EDX, EDX)
	DIV_L	(ECX)

	CMP_L	(EBX, EAX)
	JC	(pUp1)

	MOV_L	(EBX, EAX)
pUp1:
	INC_L	(EAX)
	MOV_L	(height, EBX)
	CMP_L	(EBX, EAX)
	JC	(pUp2)

	MOV_L	(EBX, EAX)
pUp2:
	SUB_L	(EAX, EBX)
	MOV_L	(EBX, height)

	MOV_L	(EAX, CONTENT(allowance))
	CALL	(VARINDIRECT(func))

	CMP_L	(CONST(0), height)
	JNE	(pUp3)

	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONST(0x0f02), AX)
	OUT_W
	MOV_W	(CONST(0x0c04), AX)
	OUT_W
	MOV_W	(CONST(0x4005), AX)
	MOV_W	(CONST(0x3ce), DX)
	OUT_W

	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/***************************/
	ALIGNTEXT4
pDown:
	MOV_L	(EAX, screen)
	SUB_L	(width, EAX)
	MOV_L	(EAX, CONTENT(offset))
	ADD_L	(CONTENT(GLNAME(vgaWriteTop)), EAX)
	MOV_L	(EAX, CONTENT(speedUpBound))
	MOV_L	(ADDR(pDown3), CONTENT(pPartialJump))
	MOV_B	(CONTENT(segment), AL)
	JMP	(pDown5)
/***************************/
	ALIGNTEXT4
pDown3:
	MOV_B	(CONTENT(segment), AL)
	CMP_L	(CONTENT(GLNAME(vgaReadTop)), ESI)
	JC	(pDown4)
	ADD_B	(CONST(16), AL)
	SUB_L	(CONST(0x10000), ESI)
pDown4:
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)), EDI)
	JC	(pDown6)
	INC_B	(AL)
	SUB_L	(CONST(0x10000), EDI)
pDown6:
	MOV_B	(AL, CONTENT(segment))
pDown5:
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	MOV_L	(ESI, CONTENT(sM))
	MOV_L	(EDI, CONTENT(dM))

	MOV_L	(CONTENT(speedUpBound), EAX)
	MOV_L	(CONTENT(speedUpBound), EBX)
	SUB_L	(ESI, EAX)
	SUB_L	(EDI, EBX)

	MOV_L	(screen, ECX)
	XOR_L	(EDX, EDX)
	DIV_L	(ECX)
	XCHG_L	(EAX, EBX)

	XOR_L	(EDX, EDX)
	DIV_L	(ECX)

	CMP_L	(EBX, EAX)
	JC	(pDown1)

	MOV_L	(EBX, EAX)
pDown1:
	OR_L	(EAX, EAX)
	JZ	(pPartial)

	MOV_L	(height, EBX)
	CMP_L	(EBX, EAX)
	JC	(pDown2)

	MOV_L	(EBX, EAX)
pDown2:
	SUB_L	(EAX, EBX)
	MOV_L	(EBX, height)

	MOV_L	(EAX, CONTENT(allowance))
	CALL	(VARINDIRECT(func))

	CMP_L	(CONST(0), height)
	JNE	(pDown3)

	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONST(0x0f02), AX)
	OUT_W
	MOV_W	(CONST(0x0c04), AX)
	OUT_W
	MOV_W	(CONST(0x4005), AX)
	MOV_W	(CONST(0x3ce), DX)
	OUT_W

	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET
/************************************/
	ALIGNTEXT4
pLMR:
	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONTENT(lMask), AX)
	OUT_W

	MOV_L	(CONTENT(pLROffset), EAX)
	MOV_L	(CONTENT(allowance), EDX)
pLMR1:
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pLMR1)

	MOV_L	(ESI, CONTENT(sM1))
	MOV_L	(EDI, CONTENT(dM1))

	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONST(0x0f02), AX)
	OUT_W

	MOV_L	(CONTENT(sM), ESI)
	MOV_L	(CONTENT(dM), EDI)
	INC_L	(ESI)
	INC_L	(EDI)

	MOV_L	(CONTENT(pMOffset), EAX)
	MOV_L	(CONTENT(mCount), EBX)
	MOV_L	(CONTENT(allowance), EDX)
pLMR2:
	MOV_L	(EBX, ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pLMR2)

	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONTENT(rMask), AX)
	OUT_W

	MOV_L	(CONTENT(pLROffset), EAX)
	MOV_L	(CONTENT(allowance), EDX)

	MOV_L	(CONTENT(sM), ESI)
	MOV_L	(CONTENT(dM), EDI)
	MOV_L	(CONTENT(mCount), ECX)
	INC_L	(ECX)
	ADD_L	(ECX, ESI)
	ADD_L	(ECX, EDI)
pLMR3:
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pLMR3)

	MOV_L	(CONTENT(sM1), ESI)
	MOV_L	(CONTENT(dM1), EDI)

	RET
/************************************/
	ALIGNTEXT4
pLMRa:
	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONTENT(rMask), AX)
	OUT_W

	MOV_L	(CONTENT(pLROffset), EAX)
	MOV_L	(CONTENT(allowance), EDX)

	MOV_L	(CONTENT(mCount), ECX)
	INC_L	(ECX)
	ADD_L	(ECX, ESI)
	ADD_L	(ECX, EDI)
pLMRa3:
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pLMRa3)

	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONST(0x0f02), AX)
	OUT_W

	MOV_L	(CONTENT(sM), ESI)
	MOV_L	(CONTENT(dM), EDI)
	INC_L	(ESI)
	INC_L	(EDI)

	MOV_L	(CONTENT(pMOffset), EAX)
	MOV_L	(CONTENT(mCount), EBX)
	MOV_L	(CONTENT(allowance), EDX)
pLMRa2:
	MOV_L	(EBX, ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pLMRa2)

	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONTENT(lMask), AX)
	OUT_W

	MOV_L	(CONTENT(pLROffset), EAX)
	MOV_L	(CONTENT(allowance), EDX)

	MOV_L	(CONTENT(sM), ESI)
	MOV_L	(CONTENT(dM), EDI)
pLMRa1:
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pLMRa1)

	RET
/************************************/
	ALIGNTEXT4
pLM:
	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONTENT(lMask), AX)
	OUT_W

	MOV_L	(CONTENT(pLROffset), EAX)
	MOV_L	(CONTENT(allowance), EDX)
pLM1:
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pLM1)

	MOV_L	(ESI, CONTENT(sM1))
	MOV_L	(EDI, CONTENT(dM1))

	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONST(0x0f02), AX)
	OUT_W

	MOV_L	(CONTENT(sM), ESI)
	MOV_L	(CONTENT(dM), EDI)
	INC_L	(ESI)
	INC_L	(EDI)

	MOV_L	(CONTENT(pMOffset), EAX)
	MOV_L	(CONTENT(mCount), EBX)
	MOV_L	(CONTENT(allowance), EDX)
pLM2:
	MOV_L	(EBX, ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pLM2)

	MOV_L	(CONTENT(sM1), ESI)
	MOV_L	(CONTENT(dM1), EDI)

	RET
/************************************/
	ALIGNTEXT4
pLMa:
	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONST(0x0f02), AX)
	OUT_W

	INC_L	(ESI)
	INC_L	(EDI)

	MOV_L	(CONTENT(pMOffset), EAX)
	MOV_L	(CONTENT(mCount), EBX)
	MOV_L	(CONTENT(allowance), EDX)
pLMa2:
	MOV_L	(EBX, ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pLMa2)

	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONTENT(lMask), AX)
	OUT_W

	MOV_L	(CONTENT(pLROffset), EAX)
	MOV_L	(CONTENT(allowance), EDX)

	MOV_L	(CONTENT(sM), ESI)
	MOV_L	(CONTENT(dM), EDI)
pLMa1:
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pLMa1)

	RET
/************************************/
	ALIGNTEXT4
pMR:
	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONST(0x0f02), AX)
	OUT_W

	MOV_L	(CONTENT(pMOffset), EAX)
	MOV_L	(CONTENT(mCount), EBX)
	MOV_L	(CONTENT(allowance), EDX)
pMR2:
	MOV_L	(EBX, ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pMR2)

	MOV_L	(ESI, CONTENT(sM1))
	MOV_L	(EDI, CONTENT(dM1))

	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONTENT(rMask), AX)
	OUT_W

	MOV_L	(CONTENT(pLROffset), EAX)
	MOV_L	(CONTENT(allowance), EDX)

	MOV_L	(CONTENT(sM), ESI)
	MOV_L	(CONTENT(dM), EDI)
	MOV_L	(CONTENT(mCount), ECX)
	ADD_L	(ECX, ESI)
	ADD_L	(ECX, EDI)
pMR3:
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pMR3)

	MOV_L	(CONTENT(sM1), ESI)
	MOV_L	(CONTENT(dM1), EDI)

	RET
/************************************/
	ALIGNTEXT4
pMRa:
	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONTENT(rMask), AX)
	OUT_W

	MOV_L	(CONTENT(pLROffset), EAX)
	MOV_L	(CONTENT(allowance), EDX)

	MOV_L	(CONTENT(mCount), ECX)
	ADD_L	(ECX, ESI)
	ADD_L	(ECX, EDI)
pMRa3:
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pMRa3)

	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONST(0x0f02), AX)
	OUT_W

	MOV_L	(CONTENT(sM), ESI)
	MOV_L	(CONTENT(dM), EDI)

	MOV_L	(CONTENT(pMOffset), EAX)
	MOV_L	(CONTENT(mCount), EBX)
	MOV_L	(CONTENT(allowance), EDX)
pMRa2:
	MOV_L	(EBX, ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pMRa2)

	RET
/************************************/
	ALIGNTEXT4
pM:
	MOV_L	(CONTENT(pMOffset), EAX)
	MOV_L	(CONTENT(mCount), EBX)
	MOV_L	(CONTENT(allowance), EDX)
pM2:
	MOV_L	(EBX, ECX)
	REP
	MOVS_B
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)
	DEC_L	(EDX)
	JNZ	(pM2)

	RET
/************************************/
	ALIGNTEXT4
pPartial:
	MOV_L	(CONTENT(mCount), EBX)
	MOV_L	(CONST(0), CONTENT(tmp))

	MOV_W	(CONTENT(lMask), AX)
	CMP_W	(CONST(0x0f02), AX)
	JE	(pPartial7)
pPartial9:
	MOV_W	(CONST(0x3c4), DX)
	OUT_W
	MOVS_B
	
	MOV_W	(CONST(0x0f02), AX)
	OUT_W
	JMP	(pPartial6)
	ALIGNTEXT4
pPartial7:
	MOV_W	(CONST(0x3c4), DX)
	OUT_W
pPartial5:
	MOV_L	(CONTENT(GLNAME(vgaReadTop)), EAX)
	MOV_L	(CONTENT(GLNAME(vgaWriteTop)), ECX)
	SUB_L	(ESI, EAX)
	SUB_L	(EDI, ECX)

	CMP_L	(EAX, ECX)
	JC	(pPartial1)

	MOV_L	(EAX, ECX)
pPartial1:
	CMP_L	(EBX, ECX)
	JC	(pPartial2)

	MOV_L	(EBX, ECX)
pPartial2:
	SUB_L	(ECX, EBX)

	REP
	MOVS_B
pPartial6:
	MOV_B	(CONTENT(segment), AL)
	CMP_L	(CONTENT(GLNAME(vgaReadTop)), ESI)
	JC	(pPartial3)

	SUB_L	(CONST(0x10000), ESI)
	ADD_B	(CONST(16), AL)
pPartial3:
	CMP_L	(CONTENT(GLNAME(vgaWriteTop)), EDI)
	JC	(pPartial4)

	SUB_L	(CONST(0x10000), EDI)
	INC_B	(AL)
pPartial4:
	MOV_B	(AL, CONTENT(segment))
	MOV_W	(CONST(0x3cd), DX)
	OUT_B

	OR_L	(EBX, EBX)
	JNZ	(pPartial5)

	CMP_L	(CONST(1), CONTENT(tmp))
	JE	(pPartial8)

	MOV_L	(CONST(1), CONTENT(tmp))
	MOV_W	(CONTENT(rMask), AX)
	CMP_W	(CONST(0x0f02), AX)
	JNE	(pPartial9)
pPartial8:
	MOV_L	(CONTENT(GLNAME(vgaReadTop)), EAX)
	MOV_L	(CONTENT(offset), EAX)
	ADD_L	(EAX, ESI)
	ADD_L	(EAX, EDI)

	DEC_L	(height)
	JZ	(pPartial10)
	JMP	(VARINDIRECT(pPartialJump))
	ALIGNTEXT4
pPartial10:
	MOV_W	(CONST(0x3c4), DX)
	MOV_W	(CONST(0x0f02), AX)
	OUT_W
	MOV_W	(CONST(0x0c04), AX)
	OUT_W
	MOV_W	(CONST(0x4005), AX)
	MOV_W	(CONST(0x3ce), DX)
	OUT_W

	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET

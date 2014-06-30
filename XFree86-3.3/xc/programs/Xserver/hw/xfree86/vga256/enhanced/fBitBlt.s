/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/enhanced/fBitBlt.s,v 3.1 1996/12/23 06:59:01 dawes Exp $ */
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
 */
/* $XConsortium: fBitBlt.s /main/4 1996/02/21 18:08:39 kaleb $ */


/*
 * Just copy the fastest way you can !!!!
 * Not questions of style here, please.
 * Since our dragon is SSOOOOOOOOOOOOOO slow don't compute timings the normal
 * way.
 * ----->>>>> a 'movsl' takes about 144(!!!) cycles on my 33MHz 386 <<<<<-----
 *
 * But cause of the BRAINDAMAGED 386 you cann't use movsl -- it does only
 * post-decrement/increment !!!
 *
 * (7/28/90 TR)
 */

#include "assyntax.h"

	FILE("fBitBlt.s")
	AS_BEGIN

#define tmp         EBX
#define xdir        REGOFF(8,EBP)
#define psrc        ESI
#define pdst        EDI
#define hcount      EDX
#define count       REGOFF(24,EBP)
#define srcPitch    REGOFF(28,EBP)
#define dstPitch    REGOFF(32,EBP)
#define srcPitchReg ECX
#define dstPitchReg EAX
#define xSrc        REGOFF(-4,EBP)
#define xDst        REGOFF(-8,EBP)
#define countS      REGOFF(-12,EBP)
#define Shift       ECX
#define low         EAX
#define lowb        AH
#define lowbl       AL
#define loww        AX
#define high        EDX
#define highb       DH
#define highbl      DL
#define highw       DX
#define hcountS     REGOFF(20,EBP)

SEG_TEXT
	ALIGNTEXT4
GLOBL	GLNAME(fastBitBltCopy)
GLNAME(fastBitBltCopy):
	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	SUB_L	(CONST(12),ESP)
	PUSH_L	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)
	MOV_L	(REGOFF(12,EBP),psrc)
	MOV_L	(REGOFF(16,EBP),pdst)
	MOV_L	(REGOFF(20,EBP),hcount)
	MOV_L	(count,tmp)
	
	CMP_L	(CONST(1),xdir)
	JE	(.fastmove)
	std
.fastmove:

	CMP_L	(CONST(5),tmp)
	JG	(.BlockSetup)
	MOV_L	(srcPitch,srcPitchReg)
	MOV_L	(dstPitch,dstPitchReg)
	CMP_L	(CONST(1),xdir)
	JNE	(.RightFast)

.LeftFast:
	ADD_L	(tmp,srcPitchReg)
	ADD_L	(tmp,dstPitchReg)
	CMP_L	(CONST(4),tmp)
	JG	(.LeftFiveLoop)
	JE	(.LeftFourLoop)
	CMP_L	(CONST(2),tmp)
	JG	(.LeftThreeLoop)
	JE	(.LeftTwoLoop)
	JMP	(.LeftOneLoop)

	ALIGNTEXT4
.LeftOneBody:
	MOV_B	(REGIND(psrc),BL)
	MOV_B	(BL,REGIND(pdst))
	ADD_L	(srcPitchReg,psrc)
	ADD_L	(dstPitchReg,pdst)
.LeftOneLoop:
	DEC_L	(hcount)
	JNS	(.LeftOneBody)
	JMP	(.finish)

	ALIGNTEXT4
.LeftTwoBody:
	MOV_W	(REGIND(psrc),BX)
	MOV_W	(BX,REGIND(pdst))
	ADD_L	(srcPitchReg,psrc)
	ADD_L	(dstPitchReg,pdst)
.LeftTwoLoop:
	DEC_L	(hcount)
	JNS	(.LeftTwoBody)
	JMP	(.finish)

	ALIGNTEXT4
.LeftThreeBody:
	MOV_W	(REGIND(psrc),BX)
	MOV_W	(BX,REGIND(pdst))
	MOV_B	(REGOFF(2,psrc),BL)
	MOV_B	(BL,REGOFF(2,pdst))
	ADD_L	(srcPitchReg,psrc)
	ADD_L	(dstPitchReg,pdst)
.LeftThreeLoop:
	DEC_L	(hcount)
	JNS	(.LeftThreeBody)
	JMP	(.finish)

	ALIGNTEXT4
.LeftFourBody:
	MOV_L	(REGIND(psrc),EBX)
	MOV_L	(EBX,REGIND(pdst))
	ADD_L	(srcPitchReg,psrc)
	ADD_L	(dstPitchReg,pdst)
.LeftFourLoop:
	DEC_L	(hcount)
	JNS	(.LeftFourBody)
	JMP	(.finish)

	ALIGNTEXT4
.LeftFiveBody:
	MOV_L	(REGIND(psrc),EBX)
	MOV_L	(EBX,REGIND(pdst))
	MOV_B	(REGOFF(4,psrc),BL)
	MOV_B	(BL,REGOFF(4,pdst))
	ADD_L	(srcPitchReg,psrc)
	ADD_L	(dstPitchReg,pdst)
.LeftFiveLoop:
	DEC_L	(hcount)
	JNS	(.LeftFiveBody)
	JMP	(.finish)
		
.RightFast:
	SUB_L	(tmp,srcPitchReg)
	SUB_L	(tmp,dstPitchReg)
	CMP_L	(CONST(4),tmp)
	JG	(.RightFiveLoop)
	JE	(.RightFourLoop)
	CMP_L	(CONST(2),tmp)
	JG	(.RightThreeLoop)
	JE	(.RightTwoLoop)
	JMP	(.RightOneLoop)

	ALIGNTEXT4
.RightOneBody:
	MOV_B	(REGOFF(-1,psrc),BL)
	MOV_B	(BL,REGOFF(-1,pdst))
	ADD_L	(srcPitchReg,psrc)
	ADD_L	(dstPitchReg,pdst)
.RightOneLoop:
	DEC_L	(hcount)
	JNS	(.RightOneBody)
	JMP	(.finish)

	ALIGNTEXT4
.RightTwoBody:
	MOV_W	(REGOFF(-2,psrc),BX)
	MOV_W	(BX,REGOFF(-2,pdst))
	ADD_L	(srcPitchReg,psrc)
	ADD_L	(dstPitchReg,pdst)
.RightTwoLoop:
	DEC_L	(hcount)
	JNS	(.RightTwoBody)
	JMP	(.finish)

	ALIGNTEXT4
.RightThreeBody:
	MOV_W	(REGOFF(-3,psrc),BX)
	MOV_W	(BX,REGOFF(-3,pdst))
	MOV_B	(REGOFF(-1,psrc),BL)
	MOV_B	(BL,REGOFF(-1,pdst))
	ADD_L	(srcPitchReg,psrc)
	ADD_L	(dstPitchReg,pdst)
.RightThreeLoop:
	DEC_L	(hcount)
	JNS	(.RightThreeBody)
	JMP	(.finish)

	ALIGNTEXT4
.RightFourBody:
	MOV_L	(REGOFF(-4,psrc),EBX)
	MOV_L	(EBX,REGOFF(-4,pdst))
	ADD_L	(srcPitchReg,psrc)
	ADD_L	(dstPitchReg,pdst)
.RightFourLoop:
	DEC_L	(hcount)
	JNS	(.RightFourBody)
	JMP	(.finish)

	ALIGNTEXT4
.RightFiveBody:
	MOV_L	(REGOFF(-5,psrc),EBX)
	MOV_L	(EBX,REGOFF(-5,pdst))
	MOV_B	(REGOFF(-1,psrc),BL)
	MOV_B	(BL,REGOFF(-1,pdst))
	ADD_L	(srcPitchReg,psrc)
	ADD_L	(dstPitchReg,pdst)
.RightFiveLoop:
	DEC_L	(hcount)
	JNS	(.RightFiveBody)
	JMP	(.finish)


.BlockSetup:
	MOV_L	(psrc,tmp)
	AND_L	(CONST(3),tmp)
	MOV_L	(pdst,Shift)
	AND_L	(CONST(3),Shift)
	CMP_L	(Shift,tmp)
	JNE	(.UnalignedBlock)

	CMP_L	(CONST(1),xdir)
	JE	(.LeftBlockLoop)
	JMP	(.RightBlockLoop)

.LeftBlockBody:
	TEST_L	(CONST(1),pdst)		/* align to word ? */
	JZ	(.LeftAlignWord)
	MOVS_B
	DEC_L	(tmp)
.LeftAlignWord:
	TEST_L	(CONST(2),pdst)		/* align to dword ? */
	JZ	(.LeftStartBlock)
	MOVS_W
	SUB_L	(CONST(2),tmp)
.LeftStartBlock:
	MOV_L	(tmp,ECX)
	SHR_L	(CONST(2),ECX)
	REPZ
	MOVS_L   		/* tricky: this is really the fastest way !! */
	TEST_L	(CONST(2),tmp)		/* finish requires a word ? */
	JZ	(.LeftFixupByte)
	MOVS_W
.LeftFixupByte:
	TEST_L	(CONST(1),tmp)		/* finish requires a byte ? */
	JZ	(.LeftBlockEnd)
	MOVS_B
.LeftBlockEnd:
	ADD_L	(srcPitch,psrc)		/* ok, goto next line */
	ADD_L	(dstPitch,pdst)
.LeftBlockLoop:
	MOV_L	(count,tmp)		/* reload linecounter */
	DEC_L	(hcount)
	JNS	(.LeftBlockBody)
	JMP	(.finish)

.RightBlockBody:
	TEST_L	(CONST(1),pdst)		/* align to word ? */
	JZ	(.RightAlignWord)
	DEC_L	(psrc)
	DEC_L	(pdst)
	MOV_B	(REGIND(psrc),AL)
	MOV_B	(AL,REGIND(pdst))
	DEC_L	(tmp)
.RightAlignWord:
	TEST_L	(CONST(2),pdst)		/* align to dword ? */
	JZ	(.RightStartBlock)
	SUB_L	(CONST(2),psrc)
	SUB_L	(CONST(2),pdst)
	MOV_W	(REGIND(psrc),AX)
	MOV_W	(AX,REGIND(pdst))
	SUB_L	(CONST(2),tmp)
.RightStartBlock:
	MOV_L	(tmp,ECX)
	SHR_L	(CONST(2),ECX)
	SUB_L	(CONST(4),psrc)
	SUB_L	(CONST(4),pdst)
	REPZ
	MOVS_L	/* tricky: this is really the fastest way !! */
	ADD_L	(CONST(4),psrc)
	ADD_L	(CONST(4),pdst)
	TEST_L	(CONST(2),tmp)	/* finish requires a word ? */
	JZ	(.RightFixupByte)
	SUB_L	(CONST(2),psrc)
	SUB_L	(CONST(2),pdst)
	MOV_W	(REGIND(psrc),AX)
	MOV_W	(AX,REGIND(pdst))
.RightFixupByte:
	TEST_L	(CONST(1),tmp)	/* finish requires a byte ? */
	JZ	(.RightBlockEnd)
	DEC_L	(psrc)
	DEC_L	(pdst)
	MOV_B	(REGIND(psrc),AL)
	MOV_B	(AL,REGIND(pdst))
.RightBlockEnd:
	ADD_L	(srcPitch,psrc)	/* ok, goto next line */
	ADD_L	(dstPitch,pdst)
.RightBlockLoop:
	MOV_L	(count,tmp)	/* reload linecounter */
	DEC_L	(hcount)
	JNS	(.RightBlockBody)
	JMP	(.finish)

/*
 * ok, now the cases when Src & Dst are not at the same offset
 */
.UnalignedBlock:

	MOV_L	(tmp,xSrc)
	MOV_L	(Shift,xDst)
	CMP_L	(CONST(1),xdir)
	JNE	(.rightShiftSetup)

	SUB_L	(tmp,Shift)
	JS	(.lsAdjustShift)
	SUB_L	(CONST(4),Shift)
.lsAdjustShift:
	NEG_L	(Shift)
	SHL_L	(CONST(3),Shift)
	JMP	(.leftShiftLoop)


.leftShiftBody:
	MOV_L	(CONST(0),low)
	MOV_L	(CONST(0),high)
/*
 * first load (4-xSrc) Pixels, so that psrc is dword-aligned
 */
	MOV_L	(xSrc,tmp)
	MOV_L	(REGDIS(.lseTab,tmp,4),tmp)
	JMP	(CODEPTR(tmp))
	SEG_DATA
	ALIGNDATA4
.lseTab:	D_LONG	.lse0,	.lse1,	.lse2,	.lse3
	SEG_TEXT
	ALIGNTEXT4
.lse0:	MOV_L	(REGIND(psrc),low)
	ADD_L	(CONST(4),psrc)
	SUB_L	(CONST(4),countS)
	JMP	(.lse)
.lse1:	MOV_W	(REGOFF(1,psrc),loww)
	SHL_L	(CONST(16),low)
	MOV_B	(REGIND(psrc),lowb)
	ADD_L	(CONST(3),psrc)
	SUB_L	(CONST(3),countS)
	JMP	(.lse)
.lse2:	MOV_W	(REGIND(psrc),loww)
	SHL_L	(CONST(16),low)
	ADD_L	(CONST(2),psrc)
	SUB_L	(CONST(2),countS)
	JMP	(.lse)
.lse3:	MOV_B	(REGIND(psrc),lowb)
	SHL_L	(CONST(16),low)
	INC_L	(psrc)
	DEC_L	(countS)
/*
 * get now the rest of Pixels neccessary to align pdst to a dword
 * i.e  if (4 - xDst) > (4 - xSrc) get 4 additional Pixels
 * i.e  if xDst < xSrc !!!!
 */
.lse:	MOV_L	(xDst,tmp)
	OR_L	(tmp,tmp)
	JZ	(.lsf0)
	CMP_L	(tmp,xSrc)
	JL	(.lsfirst)
	MOV_L	(low,high)
	MOV_L	(REGIND(psrc),low)
	ADD_L	(CONST(4),psrc)
	SUB_L	(CONST(4),countS)
/*
 * now store (4- xDst) Pixel
 */
.lsfirst:
	SHRD2_L	(low,high)
	MOV_L	(REGDIS(.lsfTab,tmp,4),tmp)
	JMP	(CODEPTR(tmp))
	SEG_DATA
	ALIGNDATA4
.lsfTab:	D_LONG	.lsf0,	.lsf1,	.lsf2,	.lsf3
	SEG_TEXT
	ALIGNTEXT4
.lsf1:	MOV_B	(highb,REGIND(pdst))
	SHR_L	(CONST(16),high)
	MOV_W	(highw,REGOFF(1,pdst))
	ADD_L	(CONST(3),pdst)
	JMP	(.lsf0)
.lsf2:	SHR_L	(CONST(16),high)
	MOV_W	(highw,REGIND(pdst))
	ADD_L	(CONST(2),pdst)
	JMP	(.lsf0)
.lsf3:	SHR_L	(CONST(16),high)
	MOV_B	(highb,REGIND(pdst))
	INC_L	(pdst)
/*
 * we have countS Pixel to load. Get them as dword, i.e as 4 Pixel group.
 * that means we can get (countS >> 2) dwords !
 * since psrc and pdst are aligned dword, this is the fast case.
 */
.lsf0:	MOV_L	(low,high)
	MOV_L	(countS,tmp)
	SHR_L	(CONST(2),tmp)
	JMP	(.lsdl)

	ALIGNTEXT4
.lsdb:	LODS_L
	SHRD2_L	(low,high)
	XCHG_L	(low,high)
	STOS_L
.lsdl:	DEC_L	(tmp)
	JNS	(.lsdb)
/*
 * the are (countS & 3) Pixles to get.
 * but be carefull, these pixels are now aligned LEFT !!!! ( on the screen)
 */
	XOR_L	(low,low)
	MOV_L	(countS,tmp)
	AND_L	(CONST(3),tmp)
	ADD_L	(tmp,psrc)
	MOV_L	(REGDIS(.lsaTab,tmp,4),tmp)
	JMP	(CODEPTR(tmp))
	SEG_DATA
	ALIGNDATA4
.lsaTab:	D_LONG	.lsa0,	.lsa1,	.lsa2,	.lsa3
	SEG_TEXT
	ALIGNTEXT4
.lsa3:	MOV_B	(REGOFF(-1,psrc),lowbl)
	SHL_L	(CONST(16),low)
	MOV_W	(REGOFF(-3,psrc),loww)
	JMP	(.lsa0)
.lsa2:	MOV_W	(REGOFF(-2,psrc),loww)
	JMP	(.lsa0)
.lsa1:	MOV_B	(REGOFF(-1,psrc),lowbl)

.lsa0:	SHRD2_L	(low,high)
	SHR_L	(CL,low)
/*
 * at this point we have read all Pixels, but there are still some to store
 * the number of Pixel in [low,high] seems to be ((countS&3)+(4 - Shift))
 * that maens we have here to store 1,2,3,4,5,6 Pixel (0 & 7 are impossible)
 */
	MOV_L	(countS,tmp)
	AND_L	(CONST(3),tmp)
	ADD_L	(CONST(4),tmp)
	SHL_L	(CONST(3),tmp)
	SUB_L	(Shift,tmp)
	SHR_L	(CONST(3),tmp)
	ADD_L	(tmp,pdst)
	MOV_L	(REGDIS(.lsgTab,tmp,4),tmp)
	JMP	(CODEPTR(tmp))
	SEG_DATA
	ALIGNDATA4
.lsgTab:	D_LONG	.lsg0,	.lsg1,	.lsg2,	.lsg3,	.lsg4,	.lsg5,	.lsg6,	.lsg0
	SEG_TEXT
	ALIGNTEXT4
.lsg6:	MOV_L	(high,REGOFF(-6,pdst))
	MOV_W	(loww,REGOFF(-2,pdst))
	JMP	(.lsg0)

.lsg5:	MOV_L	(high,REGOFF(-5,pdst))
	MOV_B	(lowbl,REGOFF(-1,pdst))
	JMP	(.lsg0)

.lsg4:	MOV_L	(high,REGOFF(-4,pdst))
	JMP	(.lsg0)

.lsg3:	MOV_W	(highw,REGOFF(-3,pdst))
	SHR_L	(CONST(16),high)
	MOV_B	(highbl,REGOFF(-1,pdst))
	JMP	(.lsg0)

.lsg2:	MOV_W	(highw,REGOFF(-2,pdst))
	JMP	(.lsg0)

.lsg1:	MOV_B	(highbl,REGOFF(-1,pdst))

.lsg0:	MOV_L	(countS,tmp)
	AND_L	(CONST(3),tmp)
	ADD_L	(CONST(4),tmp)
	SHL_L	(CONST(3),tmp)
	SUB_L	(Shift,tmp)
	SHR_L	(CONST(3),tmp)


	ADD_L	(srcPitch,psrc)
	ADD_L	(dstPitch,pdst)

.leftShiftLoop:
	MOV_L	(count,tmp)
	MOV_L	(tmp,countS)
	DEC_L	(hcountS)
	JNS	(.leftShiftBody)
	JMP	(.finish)

/*
 * now, descending x-direction
 */
.rightShiftSetup:

	SUB_L	(tmp,Shift)
	JNS	(.rsAdjustShift)
	ADD_L	(CONST(4),Shift)
.rsAdjustShift:
	SHL_L	(CONST(3),Shift)
	JMP	(.rightShiftLoop)

.rightShiftBody:
	MOV_L	(CONST(0),low)
	MOV_L	(CONST(0),high)
/*
 * first load (xSrc) Pixels, so that psrc is dword-aligned
 */
	MOV_L	(xSrc,tmp)
	MOV_L	(REGDIS(.rseTab,tmp,4),tmp)
	JMP	(CODEPTR(tmp))
	SEG_DATA
	ALIGNDATA4
.rseTab:	D_LONG	.rse0,	.rse1,	.rse2,	.rse3
	SEG_TEXT
	ALIGNTEXT4
.rse3:	SUB_L	(CONST(3),psrc)
	SUB_L	(CONST(3),countS)
	MOV_B	(REGOFF(2,psrc),lowbl)
	SHL_L	(CONST(16),low)
	MOV_W	(REGIND(psrc),loww)
	JMP	(.rse0)
.rse2:	SUB_L	(CONST(2),psrc)
	SUB_L	(CONST(2),countS)
	MOV_W	(REGIND(psrc),loww)
	JMP	(.rse0)
.rse1:	DEC_L	(psrc)
	DEC_L	(countS)
	MOV_B	(REGIND(psrc),lowbl)
/*
 * get now the rest of Pixels neccessary to align pdst to a dword
 * i.e  if (xDst > xSrc) get 4 additional Pixels
 */
.rse0:	MOV_L	(xDst,tmp)
	OR_L	(tmp,tmp)
	JZ	(.rsf0)
	CMP_L	(tmp,xSrc)
	JG	(.rsfirst)
	SUB_L	(CONST(4),psrc)
	SUB_L	(CONST(4),countS)
	MOV_L	(low,high)
	MOV_L	(REGIND(psrc),low)
/*
 * now store (xDst) Pixels
 */
.rsfirst:
	SHLD2_L	(low,high)
	MOV_L	(REGDIS(.rsfTab,tmp,4),tmp)
	JMP	(CODEPTR(tmp))
	SEG_DATA
	ALIGNDATA4
.rsfTab:	D_LONG	.rsf0,	.rsf1,	.rsf2,	.rsf3
	SEG_TEXT
	ALIGNTEXT4
.rsf3:	SUB_L	(CONST(3),pdst)
	MOV_W	(highw,REGIND(pdst))
	SHR_L	(CONST(16),high)
	MOV_B	(highbl,REGOFF(2,pdst))
	JMP	(.rsf0)
.rsf2:	SUB_L	(CONST(2),pdst)
	MOV_W	(highw,REGIND(pdst))
	JMP	(.rsf0)
.rsf1:	DEC_L	(pdst)
	MOV_B	(highbl,REGIND(pdst))
/*
 * we have countS Pixel to load. Get them as dword, i.e as 4 Pixel group.
 * that means we can get (countS >> 2) dwords !
 * since psrc and pdst are aligned dword, this is the fast case.
 */
.rsf0:	MOV_L	(low,high)
	MOV_L	(countS,tmp)
	SHR_L	(CONST(2),tmp)
	JMP	(.rsdl)

	ALIGNTEXT4
.rsdb:	SUB_L	(CONST(4),psrc)
	SUB_L	(CONST(4),pdst)
	MOV_L	(REGIND(psrc),low)
	SHLD2_L	(low,high)
	MOV_L	(high,REGIND(pdst))
	MOV_L	(low,high)
.rsdl:	DEC_L	(tmp)
	JNS	(.rsdb)
/*
 * the are (countS & 3) Pixles to get.
 * but be carefull, these pixels are now aligned RIGHT !!!! ( on the screen)
 */
	XOR_L	(low,low)
	MOV_L	(countS,tmp)
	AND_L	(CONST(3),tmp)
	SUB_L	(tmp,psrc)
	MOV_L	(REGDIS(.rsaTab,tmp,4),tmp)
	JMP	(CODEPTR(tmp))
	SEG_DATA
	ALIGNDATA4
.rsaTab:	D_LONG	.rsa0,	.rsa1,	.rsa2,	.rsa3
	SEG_TEXT
	ALIGNTEXT4
.rsa3:	MOV_W	(REGOFF(1,psrc),loww)
	SHL_L	(CONST(16),low)
	MOV_B	(REGIND(psrc),lowb)
	JMP	(.rsa0)
.rsa2:	MOV_W	(REGIND(psrc),loww)
	SHL_L	(CONST(16),low)
	JMP	(.rsa0)
.rsa1:	MOV_B	(REGIND(psrc),lowb)
	SHL_L	(CONST(16),low)
.rsa0:	SHLD2_L	(low,high)
	SHL_L	(CL,low)
/*
 * at this point we have read all Pixels, but there are still some to store
 * the number of Pixel in [low,high] seems to be ((countS&3)+(4 - Shift))
 * that maens we have here to store 1,2,3,4,5,6 Pixel (0 & 7 are impossible)
 */
	MOV_L	(countS,tmp)
	AND_L	(CONST(3),tmp)
	ADD_L	(CONST(4),tmp)
	SHL_L	(CONST(3),tmp)
	SUB_L	(Shift,tmp)
	SHR_L	(CONST(3),tmp)
	SUB_L	(tmp,pdst)
	MOV_L	(REGDIS(.rsgTab,tmp,4),tmp)
	JMP	(CODEPTR(tmp))
	SEG_DATA
	ALIGNDATA4
.rsgTab:	D_LONG	.rsg0,	.rsg1,	.rsg2,	.rsg3,	.rsg4,	.rsg5,	.rsg6,	.rsg0
	SEG_TEXT
	ALIGNTEXT4
.rsg6:	MOV_L	(high,REGOFF(2,pdst))
	SHR_L	(CONST(16),low)
	MOV_W	(loww,REGIND(pdst))
	JMP	(.rsg0)
.rsg5:	MOV_L	(high,REGOFF(1,pdst))
	SHR_L	(CONST(16),low)
	MOV_B	(lowb,REGIND(pdst))
	JMP	(.rsg0)
.rsg4:	MOV_L	(high,REGIND(pdst))
	JMP	(.rsg0)
.rsg3:	MOV_B	(highb,REGIND(pdst))
	SHR_L	(CONST(16),high)
	MOV_W	(highw,REGOFF(1,pdst))
	JMP	(.rsg0)
.rsg2:	SHR_L	(CONST(16),high)
	MOV_W	(highw,REGIND(pdst))
	JMP	(.rsg0)
.rsg1:	SHR_L	(CONST(16),high)
	MOV_B	(highb,REGIND(pdst))

.rsg0:	ADD_L	(srcPitch,psrc)
	ADD_L	(dstPitch,pdst)
	
.rightShiftLoop:
	MOV_L	(count,tmp)
	MOV_L	(tmp,countS)
	DEC_L	(hcountS)
	JNS	(.rightShiftBody)
	JMP	(.finish)
	
.finish:
	CLD
	LEA_L	(REGOFF(-24,EBP),ESP)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET


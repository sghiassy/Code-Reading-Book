/*
 * Copyright 1996  The XFree86 Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * HARM HANEMAAYER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Written by Harm Hanemaayer (H.Hanemaayer@inter.nl.net).
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86txtblt.s,v 3.3 1997/01/12 10:48:17 dawes Exp $ */

/*
 * Intel Pentium-optimized versions of "terminal emulator font" text
 * bitmap transfer routines.
 *
 * SCANLINE_PAD_DWORD.
 *
 * Only for glyphs with a fixed width of 6 pixels or 8 pixels.
 */

#include "assyntax.h"

 	FILE("xf86txtblt.s")

	AS_BEGIN

/*
 * Definition of stack frame function arguments.
 * All functions have the same arguments (some don't have glyphwidth,
 * but that's OK, since it comes last and doesn't affect the offset
 * of the other arguments).
 */

#define base_arg	REGOFF(20,ESP)
#define glyphp_arg	REGOFF(24,ESP)
#define line_arg	REGOFF(28,ESP)
#define nglyph_arg	REGOFF(32,ESP)
#define glyphwidth_arg	REGOFF(36,ESP)

#define BYTE_REVERSED	GLNAME(byte_reversed)
#define GLYPHWIDTH_STRETCHSIZE GLNAME(glyphwidth_stretchsize)
#define GLYPHWIDTH_FUNCTION GLNAME(glyphwidth_function)

/* I assume %eax and %edx can be trashed. */

	SEG_TEXT

	ALIGNTEXT4

#ifdef MSBFIRST
	GLOBL GLNAME(xf86DrawTextScanlinePMSBFirst)
GLNAME(xf86DrawTextScanlinePMSBFirst):
#else
	GLOBL GLNAME(xf86DrawTextScanlineP)
GLNAME(xf86DrawTextScanlineP):
#endif

	SUB_L	(CONST(16),ESP)
	MOV_L	(EBP,REGOFF(12,ESP))	/* PUSH EBP */
	MOV_L	(EBX,REGOFF(8,ESP))	/* PUSH EBX */
	MOV_L	(ESI,REGOFF(4,ESP))	/* PUSH ESI */
	MOV_L	(EDI,REGOFF(0,ESP))	/* PUSH EDI */

	MOV_L	(glyphwidth_arg,ECX)
	MOV_L	(base_arg,EDI)
	MOV_L	(glyphp_arg,ESI)
	MOV_L	(REGBISD(,ECX,4,GLYPHWIDTH_STRETCHSIZE-4),EAX)
	MOV_L	(nglyph_arg,EBX)
	MOV_L	(line_arg,EBP)

	TEST_L	(EAX,EAX)
	JZ	(.main_loop)
	CMP_L	(EBX,EAX)
	JGE	(.main_loop)

	/* Accelerated stretch of specific glyph width. */
	SUB_L	(CONST(16),ESP)
	MOV_L	(REGBISD(,ECX,4,GLYPHWIDTH_FUNCTION-4),EAX)
	MOV_L	(EBX,REGOFF(12,ESP))	/* PUSH nglyph_arg */
	MOV_L	(EBP,REGOFF(8,ESP))	/* PUSH line_arg */
	MOV_L	(ESI,REGOFF(4,ESP))	/* PUSH glyphp_arg */
	MOV_L	(EDI,REGOFF(0,ESP))	/* PUSH base_arg */
	CALL	(CODEPTR(EAX))
	ADD_L	(CONST(16),ESP)

	MOV_L	(glyphwidth_arg,ECX)
	MOV_L	(REGBISD(,ECX,4,GLYPHWIDTH_STRETCHSIZE-4),EAX)
	DEC_L	(EAX)
	MOV_L	(EBX,EDX)
	AND_L	(EAX,EBX)			/* glyphsleft */
	SUB_L	(EBX,EDX)			/* nglyph - glyphsleft */
	LEA_L	(REGBISD(ESI,EDX,4,0),ESI)	/* glyphp += nglyph - glyphsleft */
	/* EBX = nglyph = glyphsleft */

.main_loop:

	/* EDI = base */
	/* EDX:EBP = bits (high32:low32) */
	/* CL = shift */
	/* ESI = glyphp */

	MOV_L	(line_arg,EBP)
	MOV_L	(REGIND(ESI),EBX)		/* glyphp[i] */
	MOV_L	(REGBISD(EBX,EBP,4,0),EBX)	/* glyphp[i][line] */
	MOV_B	(CONST(32),CH)
	MOV_L	(EBX,EAX)
	SUB_B	(CL,CH)
	SAL_L	(CL,EBX)
	OR_L	(EBX,EBP)	/* low32 |= glyphp[i][line] << shift */
	TEST_B	(CL,CL)
	JNZ	(.shiftnotzero)	/* if (shift > 0) */
	XCHG_B	(CL,CH)
	SHR_L	(CL,EAX)
	XCHG_B	(CL,CH)
	OR_L	(EAX,EDX)	/* high32 |= glyphp[i][line] >> (32 - shift) */
.shiftnotzero:
	ADD_B	(glyphwidth_arg,CL)	/* shift += glyphwidth */
	ADD_L	(CONST(4),ESI)
	CMP_B	(CONST(32),CL)
	JL	(.skipwritedword)	/* if (shift >= 32) */
	MOV_L	(EBP,REGIND(EDI))	/* *base = low32 */
	SUB_B	(CONST(32),CL)		/* shift -= 32 */
	ADD_L	(CONST(4),EDI)		/* base++ */
	MOV_L	(EDX,EBP)
	MOV_L	(CONST(0),EDX)		/* UINT64_SHIFTRIGHT32(bits) */
.skipwritedword:

	DEC_L	(nglyph_arg)
	JNZ	(.main_loop)

	TEST_B	(CL,CL)
	JNZ	(.skipwritelastdword)
	MOV_L	(EBP,REGIND(EDI))
	ADD_L	(CONST(4),EDI)	
.skipwritelastdword:

	MOV_L	(EDI,EAX)		/* return base */
	MOV_L	(REGOFF(0,ESP),EDI)	/* POPL EDI */
	MOV_L	(REGOFF(4,ESP),ESI)	/* POPL ESI */
	MOV_L	(REGOFF(8,ESP),EBX)	/* POPL EBX */
	MOV_L	(REGOFF(12,ESP),EBP)	/* POPL EBP */
	ADD_L	(CONST(16),ESP)
	RET


	ALIGNTEXT4

#ifdef MSBFIRST
	GLOBL GLNAME(DrawTextScanlineWidth6PMSBFirst)
GLNAME(DrawTextScanlineWidth6PMSBFirst):
#else
	GLOBL GLNAME(DrawTextScanlineWidth6P)
GLNAME(DrawTextScanlineWidth6P):
#endif

/* Definition of stack frame function arguments. */

#define base_arg	REGOFF(20,ESP)
#define glyphp_arg	REGOFF(24,ESP)
#define line_arg	REGOFF(28,ESP)
#define nglyph_arg	REGOFF(32,ESP)

	SUB_L	(CONST(16),ESP)
	MOV_L	(EBP,REGOFF(12,ESP))	/* PUSH EBP */
	MOV_L	(EBX,REGOFF(8,ESP))	/* PUSH EBX */
	MOV_L	(ESI,REGOFF(4,ESP))	/* PUSH ESI */
	MOV_L	(EDI,REGOFF(0,ESP))	/* PUSH EDI */

	MOV_L	(line_arg,EBP)
	MOV_L	(base_arg,EDI)
	MOV_L	(glyphp_arg,ESI)

	CMP_L	(CONST(15),nglyph_arg)
	JLE	(.L2)

.L1:
	/* Pentium-optimized instruction pairing. */
	/* EBX = bits */
	MOV_L	(REGOFF(4,ESI),EDX)		/* glyphp[1] */
	MOV_L	(REGIND(ESI),EAX)		/* glyphp[0] */
	MOV_L	(REGBISD(EDX,EBP,4,0),EDX)	/* glyphp[1][line] */
	MOV_L	(REGBISD(EAX,EBP,4,0),EAX)	/* glyphp[0][line] */
	SAL_L	(CONST(6),EDX)			/* glyphp[1][line] << 6 */
	MOV_L	(EAX,EBX)			/* bits = glyph[0][line] */

	MOV_L	(REGOFF(8,ESI),EAX)		/* glyphp[2] */
	MOV_L	(REGOFF(12,ESI),ECX)		/* glyphp[3] */
	MOV_L	(REGBISD(EAX,EBP,4,0),EAX)	/* glyphp[2][line] */
	MOV_L	(REGBISD(ECX,EBP,4,0),ECX)	/* glyphp[3][line] */
	SAL_L	(CONST(12),EAX)			/* glyphp[2][line] << 12 */
	OR_L	(EDX,EBX)			/* bits |= ..[1].. << 6 */
	SAL_L	(CONST(18),ECX)			/* glyphp[3][line] << 18 */
	OR_L	(EAX,EBX)			/* bits |= ..[2].. << 12 */

	MOV_L	(REGOFF(16,ESI),EAX)		/* glyphp[4] */
	MOV_L	(REGOFF(20,ESI),EDX)		/* glyphp[5] */
	MOV_L	(REGBISD(EAX,EBP,4,0),EAX)	/* glyphp[4][line] */
	MOV_L	(REGBISD(EDX,EBP,4,0),EDX)	/* glyphp[5][line] */
	SAL_L	(CONST(24),EAX)			/* glyphp[4][line] << 24 */
	OR_L	(ECX,EBX)			/* bits |= ..[3].. << 18 */
	SAL_L	(CONST(30),EDX)			/* glyphp[5][line] << 30 */
	OR_L	(EAX,EBX)			/* bits |= ..[4].. << 12 */

	/* Note that glyphp[5][line] is read again. */
	MOV_L	(REGOFF(20,ESI),EAX)		/* glyphp[5] */
	MOV_L	(REGOFF(24,ESI),ECX)		/* glyphp[6] */
	MOV_L	(REGBISD(EAX,EBP,4,0),EAX)	/* glyphp[5][line] */
	MOV_L	(REGBISD(ECX,EBP,4,0),ECX)	/* glyphp[6][line] */
	SHR_L	(CONST(2),EAX)			/* glyphp[5][line] >> 2 */
	OR_L	(EDX,EBX)			/* bits |= ..[5].. << 30 */
	SAL_L	(CONST(4),ECX)			/* glyphp[6][line] << 4 */
#ifndef MSBFIRST
	MOV_L	(EBX,REGIND(EDI))	/* WRITE_IN_BIT_ORDER(base, bits) */
#else
	MOV_L	(EAX,line_arg)			/* save EAX */
	MOV_L	(CONST(0),EAX)
	MOV_L	(CONST(0),EDX)
	MOV_B	(BL,AL)
	MOV_B	(BH,DL)
	MOV_B	(REGOFF(BYTE_REVERSED,EAX),BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EDX),BH)
	ROL_L	(CONST(16),EBX)
	MOV_B	(BL,AL)
	MOV_B	(BH,DL)
	MOV_B	(REGOFF(BYTE_REVERSED,EAX),BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EDX),BH)
	ROL_L	(CONST(16),EBX)
	MOV_L	(EBX,REGIND(EDI))
	MOV_L	(line_arg,EAX)			/* restore EAX */
#endif

	/* ECX = bits = glyphp[6][line] << 4 */	
	/* EAX = glyphp[5][line] >> 2 */
	MOV_L	(REGOFF(28,ESI),EBX)		/* glyphp[7] */
	MOV_L	(REGOFF(32,ESI),EDX)		/* glyphp[8] */
	MOV_L	(REGBISD(EBX,EBP,4,0),EBX)	/* glyphp[7][line] */
	MOV_L	(REGBISD(EDX,EBP,4,0),EDX)	/* glyphp[8][line] */
	SAL_L	(CONST(10),EBX)			/* glyphp[7][line] << 10 */
	OR_L	(EAX,ECX)			/* bits |= ..[5].. >> 2 */
	SAL_L	(CONST(16),EDX)			/* glyphp[8][line] << 16 */
	OR_L	(EBX,ECX)			/* bits |= ..[7].. << 10 */

	/* EDX = glyphp[8][line] << 16 */
	MOV_L	(REGOFF(36,ESI),EAX)		/* glyphp[9] */
	MOV_L	(REGOFF(40,ESI),EBX)		/* glyphp[10] */
	MOV_L	(REGBISD(EAX,EBP,4,0),EAX)	/* glyphp[9][line] */
	MOV_L	(REGBISD(EBX,EBP,4,0),EBX)	/* glyphp[10][line] */
	SAL_L	(CONST(22),EAX)			/* glyphp[9][line] << 22 */
	OR_L	(EDX,ECX)			/* bits |= ..[8].. << 16 */
	SAL_L	(CONST(28),EBX)			/* glyphp[10][line] << 28 */
	OR_L	(EAX,ECX)			/* bits |= ..[9].. << 22 */

	/* EBX = glyphp[10][line] << 28 */
	/* Note that glyphp[10][line] is read again. */
	MOV_L	(REGOFF(40,ESI),EAX)		/* glyphp[10] */
	MOV_L	(REGOFF(44,ESI),EDX)		/* glyphp[11] */
	MOV_L	(REGBISD(EAX,EBP,4,0),EAX)	/* glyphp[10][line] */
	MOV_L	(REGBISD(EDX,EBP,4,0),EDX)	/* glyphp[11][line] */
	SHR_L	(CONST(4),EAX)			/* glyphp[10][line] >> 4 */
	OR_L	(EBX,ECX)			/* bits |= ..[10].. << 28 */
	SAL_L	(CONST(2),EDX)			/* glyphp[11][line] << 2 */
#ifndef MSBFIRST
	MOV_L	(ECX,REGOFF(4,EDI))	/* WRITE_IN_BIT_ORDER(base + 1, bits) */
#else
	MOV_L	(EAX,line_arg)			/* save EAX */
	MOV_L	(CONST(0),EAX)
	MOV_L	(CONST(0),EBX)
	MOV_B	(CL,AL)
	MOV_B	(CH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EAX),CL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CH)
	ROL_L	(CONST(16),ECX)
	MOV_B	(CL,AL)
	MOV_B	(CH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EAX),CL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CH)
	ROL_L	(CONST(16),ECX)
	MOV_L	(ECX,REGOFF(4,EDI))
	MOV_L	(line_arg,EAX)			/* restore EAX */
#endif

	/* EDX = bits = glyphp[11][line] << 4 */
	/* EAX = glyphp[10][line] >> 4 */
	MOV_L	(REGOFF(48,ESI),EBX)		/* glyphp[12] */
	MOV_L	(REGOFF(52,ESI),ECX)		/* glyphp[13] */
	MOV_L	(REGBISD(EBX,EBP,4,0),EBX)	/* glyphp[12][line] */
	MOV_L	(REGBISD(ECX,EBP,4,0),ECX)	/* glyphp[13][line] */
	SAL_L	(CONST(8),EBX)			/* glyphp[12][line] << 8 */
	OR_L	(EAX,EDX)			/* bits |= ..[10].. >> 4 */
	SAL_L	(CONST(14),ECX)			/* glyphp[13][line] << 14 */
	OR_L	(EBX,EDX)			/* bits |= ..[12].. << 8 */

	/* ECX = glyphp[13][line] << 14 */
	MOV_L	(REGOFF(56,ESI),EAX)		/* glyphp[14] */
	MOV_L	(REGOFF(60,ESI),EBX)		/* glyphp[15] */
	MOV_L	(REGBISD(EAX,EBP,4,0),EAX)	/* glyphp[14][line] */
	MOV_L	(REGBISD(EBX,EBP,4,0),EBX)	/* glyphp[15][line] */
	SAL_L	(CONST(20),EAX)			/* glyphp[14][line] << 20 */
	OR_L	(ECX,EDX)			/* bits |= ..[13].. << 14 */
	SAL_L	(CONST(26),EBX)			/* glyphp[15][line] << 26 */
	OR_L	(EAX,EDX)			/* bits |= ..[14].. << 20 */

	/* EBX = glyphp[15][line] << 26 */
	OR_L	(EBX,EDX)			/* bits |= ..[15].. << 26 */
	ADD_L	(CONST(64),ESI)		/* glyphp += 16 */
#ifndef MSBFIRST
	MOV_L	(EDX,REGOFF(8,EDI))	/* WRITE_IN_BIT_ORDER(base + 2, bits) */
#else
	MOV_L	(CONST(0),EAX)
	MOV_L	(CONST(0),EBX)
	MOV_B	(DL,AL)
	MOV_B	(DH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EAX),DL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),DH)
	ROL_L	(CONST(16),EDX)
	MOV_B	(DL,AL)
	MOV_B	(DH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EAX),DL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),DH)
	ROL_L	(CONST(16),EDX)
	MOV_L	(EDX,REGOFF(8,EDI))
#endif

	ADD_L	(CONST(12),EDI)		/* base += 3 */
	SUB_L	(CONST(16),nglyph_arg)
	CMP_L	(CONST(16),nglyph_arg)
	JGE	(.L1)

.L2:
	MOV_L	(EDI,EAX)		/* return base */
	MOV_L	(REGOFF(0,ESP),EDI)	/* POPL EDI */
	MOV_L	(REGOFF(4,ESP),ESI)	/* POPL ESI */
	MOV_L	(REGOFF(8,ESP),EBX)	/* POPL EBX */
	MOV_L	(REGOFF(12,ESP),EBP)	/* POPL EBP */
	ADD_L	(CONST(16),ESP)
	RET

	ALIGNTEXT4

#ifdef MSBFIRST
	GLOBL GLNAME(DrawTextScanlineWidth8PMSBFirst)
GLNAME(DrawTextScanlineWidth8PMSBFirst):
#else
	GLOBL GLNAME(DrawTextScanlineWidth8P)
GLNAME(DrawTextScanlineWidth8P):
#endif

	SUB_L	(CONST(16),ESP)
	MOV_L	(EBP,REGOFF(12,ESP))	/* PUSH EBP */
	MOV_L	(EBX,REGOFF(8,ESP))	/* PUSH EBX */
	MOV_L	(ESI,REGOFF(4,ESP))	/* PUSH ESI */
	MOV_L	(EDI,REGOFF(0,ESP))	/* PUSH EDI */

	MOV_L	(line_arg,EBP)
	MOV_L	(base_arg,EDI)
	MOV_L	(glyphp_arg,ESI)

	CMP_L	(CONST(7),nglyph_arg)
	JLE	(.L4)

.L3:
	/* Pentium-optimized instruction pairing. */
	/* EBX = bits */
	MOV_L	(REGOFF(4,ESI),EDX)		/* glyphp[1] */
	MOV_L	(REGIND(ESI),EAX)		/* glyphp[0] */
	MOV_L	(REGBISD(EDX,EBP,4,0),EDX)	/* glyphp[1][line] */
	MOV_L	(REGBISD(EAX,EBP,4,0),EAX)	/* glyphp[0][line] */
#ifdef MSBFIRST
	MOV_B	(REGOFF(BYTE_REVERSED,EDX),DL)
	MOV_B	(REGOFF(BYTE_REVERSED,EAX),AL)
#endif
	SAL_L	(CONST(8),EDX)			/* glyphp[1][line] << 8 */
	MOV_L	(EAX,EBX)			/* bits = glyph[0][line] */

	MOV_L	(REGOFF(8,ESI),EAX)		/* glyphp[2] */
	MOV_L	(REGOFF(12,ESI),ECX)		/* glyphp[3] */
	MOV_L	(REGBISD(EAX,EBP,4,0),EAX)	/* glyphp[2][line] */
	MOV_L	(REGBISD(ECX,EBP,4,0),ECX)	/* glyphp[3][line] */
#ifdef MSBFIRST
	MOV_B	(REGOFF(BYTE_REVERSED,EAX),AL)
	MOV_B	(REGOFF(BYTE_REVERSED,ECX),CL)
#endif
	SAL_L	(CONST(16),EAX)			/* glyphp[2][line] << 16 */
	OR_L	(EDX,EBX)			/* bits |= ..[1].. << 8 */
	SAL_L	(CONST(24),ECX)			/* glyphp[3][line] << 24 */
	OR_L	(EAX,EBX)			/* bits |= ..[2].. << 16 */

	MOV_L	(REGOFF(16,ESI),EAX)		/* glyphp[4] */
	MOV_L	(REGOFF(20,ESI),EDX)		/* glyphp[5] */
	MOV_L	(REGBISD(EAX,EBP,4,0),EAX)	/* glyphp[4][line] */
	MOV_L	(REGBISD(EDX,EBP,4,0),EDX)	/* glyphp[5][line] */
#ifdef MSBFIRST
	MOV_B	(REGOFF(BYTE_REVERSED,EAX),AL)
	MOV_B	(REGOFF(BYTE_REVERSED,EDX),DL)
#endif
	SAL_L	(CONST(8),EDX)			/* glyphp[5][line] << 8 */
	OR_L	(ECX,EBX)			/* bits |= ..[3].. << 24 */

	/* EDX = bits = glyphp[5][line] >> 8 */
	OR_L	(EAX,EDX)			/* bits |= ..[4].. << 0 */
	TEST_L	(EBX,EBX)

	MOV_L	(REGOFF(24,ESI),EAX)		/* glyphp[6] */
	MOV_L	(REGOFF(28,ESI),ECX)		/* glyphp[7] */
	MOV_L	(REGBISD(EAX,EBP,4,0),EAX)	/* glyphp[6][line] */
	MOV_L	(REGBISD(ECX,EBP,4,0),ECX)	/* glyphp[7][line] */
#ifdef MSBFIRST
	MOV_B	(REGOFF(BYTE_REVERSED,EAX),AL)
	MOV_B	(REGOFF(BYTE_REVERSED,ECX),CL)
#endif
	SAL_L	(CONST(16),EAX)			/* glyphp[6][line] << 16 */
	ADD_L	(CONST(32),ESI)
	SAL_L	(CONST(24),ECX)			/* glyphp[7][line] << 24 */
	OR_L	(EAX,EDX)
	OR_L	(ECX,EDX)

	MOV_L	(EBX,REGIND(EDI))	/* WRITE_IN_BIT_ORDER(base, bits) */
	MOV_L	(EDX,REGOFF(4,EDI))	/* WRITE_IN_BIT_ORDER(base + 1, bits) */

	ADD_L	(CONST(8),EDI)		/* base += 2 */
	SUB_L	(CONST(8),nglyph_arg)
	CMP_L	(CONST(8),nglyph_arg)
	JGE	(.L3)

.L4:
	MOV_L	(EDI,EAX)		/* return base */
	MOV_L	(REGOFF(0,ESP),EDI)	/* POPL EDI */
	MOV_L	(REGOFF(4,ESP),ESI)	/* POPL ESI */
	MOV_L	(REGOFF(8,ESP),EBX)	/* POPL EBX */
	MOV_L	(REGOFF(12,ESP),EBP)	/* POPL EBP */
	ADD_L	(CONST(16),ESP)
	RET

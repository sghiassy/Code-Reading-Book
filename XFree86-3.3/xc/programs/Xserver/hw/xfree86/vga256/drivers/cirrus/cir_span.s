/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_span.s,v 3.4 1996/12/23 06:56:55 dawes Exp $ */
/*
 *
 * Copyright 1994 by H. Hanemaayer, Utrecht, The Netherlands
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of H. Hanemaayer not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  H. Hanemaayer makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * H. HANEMAAYER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL H. HANEMAAYER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  H. Hanemaayer, <hhanemaa@cs.ruu.nl>
 *
 */
/* $XConsortium: cir_span.s /main/6 1996/02/21 18:04:13 kaleb $ */

#include "assyntax.h"

 	FILE("cir_span.S")

	AS_BEGIN

/*
 * Stack frame argument definitions for solid and stipple span functions.
 */

#define vaddr_arg	REGOFF(8,EBP)
#define leftmask_arg	REGOFF(12,EBP)
#define leftbcount_arg	REGOFF(16,EBP)
#define midlcount_arg	REGOFF(20,EBP)
#define rightbcount_arg REGOFF(24,EBP)
#define rightmask_arg	REGOFF(28,EBP)
#define nspans_arg	REGOFF(32,EBP)
#define vpitch_arg	REGOFF(36,EBP)
#define bits_arg	REGOFF(40,EBP)
#define bitsplus2_arg	REGOFF(42,EBP)
#define bitsplus3_arg	REGOFF(43,EBP)


/* 
 * Many-case expanded routine to draw solid spans in write mode 4 with
 * BY8 addressing. Easily beats the 5426 BitBLT engine at 100x100 filled
 * rectangles on a local bus 486. No bank checking.
 *
 * Prototype:
 * CirrusColorExpandWriteSpans( void *vaddr, int leftmask, int leftbcount,
 *	int midlcount, int rightbcount, int rightmask, int nspans,
 *	int vpitch );
 *
 * vaddr is the byte video address of the leftmost pixel of the span in
 *	BY8 addressing mode.
 * leftmask is the value to be written to the first byte address.
 * leftbcount is the number of bytes to be written initially (including
 *	leftmask byte).
 * midlcount is the number of 32-bit words to be written after that.
 * rightbcount is the number of bytes of 0xff's to be written after that
 *	(including rightmask byte).
 * rightmask is the value to written to the last byte address.
 * nspans is the number of spans to be written (height).
 * vpitch is the logical scanline length in bytes in BY8 addressing mode.
 *
 * A virtual width (screen pitch) that is a multiple of 32 is assumed
 * (otherwise a good part of video memory access is unaligned, which is
 * a lot slower).
 *
 */

/*
 * C equivalent:

void WriteSpans(destp, leftmask, leftbcount, midlcount, rightbcount,
rightmask, h, destpitch)
	unsigned char *destp;
	int leftmask, leftbcount, midlcount, rightbcount, rightmask;
	int h;
	int destpitch;
{
	while (h > 0) {
		unsigned char *destpsave;
		int i;
		destpsave = destp;
		if (leftbcount > 0) {
			*destp = leftmask;
			destp++;
			for (i = 0; i < leftbcount - 1; i++) {
				*destp = 0xff;
				destp++;
			}
		}
		for (i = 0; i < midlcount; i++) {
			*(unsigned long *)destp = 0xffffffff;
			destp += 4;
		}
		if (rightbcount > 1) 
			for (i = 0; i < rightbcount - 1; i++) {
				*destp = 0xff;
				destp++;
			}
		if (rightbcount > 0)
			*destp = rightmask;
		destp = destpsave + destpitch;
		h--;
	}
}

*/


	SEG_TEXT
	ALIGNTEXT4

.jumptable:
	D_LONG .span00
	D_LONG .span10
	D_LONG .span20
	D_LONG .span30
	D_LONG .span40
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .span01
	D_LONG .span11
	D_LONG .span21
	D_LONG .span31
	D_LONG .span41
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .span02
	D_LONG .span12
	D_LONG .span22
	D_LONG .span32
	D_LONG .span42
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .span03
	D_LONG .span13
	D_LONG .span23
	D_LONG .span33
	D_LONG .span43
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .span04
	D_LONG .span14
	D_LONG .span24
	D_LONG .span34
	D_LONG .span44
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .span0z0
	D_LONG .span1z0
	D_LONG .span2z0
	D_LONG .span3z0
	D_LONG .span4z0
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .span0z1
	D_LONG .span1z1
	D_LONG .span2z1
	D_LONG .span3z1
	D_LONG .span4z1
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .span0z2
	D_LONG .span1z2
	D_LONG .span2z2
	D_LONG .span3z2
	D_LONG .span4z2
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .span0z3
	D_LONG .span1z3
	D_LONG .span2z3
	D_LONG .span3z3
	D_LONG .span4z3
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .span0z4
	D_LONG .span1z4
	D_LONG .span2z4
	D_LONG .span3z4
	D_LONG .span4z4


	ALIGNTEXT4

	GLOBL GLNAME(CirrusColorExpandWriteSpans)
GLNAME(CirrusColorExpandWriteSpans):

	PUSH_L	(EBP)
#ifdef USE_LAZY_ARGS
	MOV_L	(REGOFF(8,ESP),EBP)
	SUB_L	(CONST(8),EBP)
#else
	MOV_L	(ESP,EBP)
#endif
	PUSH_L	(ECX)
	PUSH_L	(EBX)
	PUSH_L	(EDI)
	PUSH_L	(ESI)

	MOV_L	(vaddr_arg,EDI)
	MOV_L	(nspans_arg,EDX)
	TEST_L	(EDX,EDX)
	JZ 	(.end)

	MOV_L	(CONST(0),EAX)
	MOV_B	(rightbcount_arg,AL)
	SHL_B	(CONST(3),AL)
	ADD_B	(leftbcount_arg,AL)

	MOV_L	(midlcount_arg,EBX)
	TEST_L	(EBX,EBX)
	JNZ	(.notzero)
	ADD_B	(CONST(40),AL)
	MOV_B	(leftmask_arg,BL)
	MOV_B	(rightmask_arg,BH)
	MOV_L	(vpitch_arg,ECX)

.notzero:
	MOV_L	(EDI,ESI)
	/* Call span function for specific leftbcount and rightbcount. */
	JMP	(CODEPTR(REGDIS(.jumptable,EAX,4)))

.end:
	POP_L	(ESI)
	POP_L	(EDI)
	POP_L	(EBX)
	POP_L	(ECX)
	POP_L	(EBP)
	RET

/*
 * Span function names are encoded as follows:
 * The first digit is equal to leftbytecount, the second to rightbytecount.
 * ESI = EDI = destination address,
 * EBX = midlcount
 * EDX = #spans
 */

.span00:
	MOV_L	(EBX,ECX)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span00)
	JMP	(.end)

.span10:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	INC_L	(EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span10)
	JMP	(.end)

.span20:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(CONST(2),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span20)
	JMP	(.end)

.span30:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	MOV_W	(CONST(0xffff),AX)
	MOV_W	(AX,REGOFF(1,EDI))
	ADD_L	(CONST(3),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span30)
	JMP	(.end)

.span40:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(CONST(0xff),AL)
	MOV_W	(AX,REGOFF(2,EDI))
	ADD_L	(CONST(4),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span40)
	JMP	(.end)

.span01:
	MOV_L	(EBX,ECX)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span01)
	JMP	(.end)

.span11:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	INC_L	(EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span11)
	JMP	(.end)

.span21:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(CONST(2),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span21)
	JMP	(.end)

.span31:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	MOV_W	(CONST(0xffff),AX)
	MOV_W	(AX,REGOFF(1,EDI))
	ADD_L	(CONST(3),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span31)
	JMP	(.end)

.span41:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(CONST(0xff),AL)
	MOV_W	(AX,REGOFF(2,EDI))
	ADD_L	(CONST(4),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span41)
	JMP	(.end)

.span02:
	MOV_L	(EBX,ECX)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span02)
	JMP	(.end)

.span12:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	INC_L	(EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span12)
	JMP	(.end)

.span22:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(CONST(2),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span22)
	JMP	(.end)

.span32:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	MOV_W	(CONST(0xffff),AX)
	MOV_W	(AX,REGOFF(1,EDI))
	ADD_L	(CONST(3),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span32)
	JMP	(.end)

.span42:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(CONST(0xff),AL)
	MOV_W	(AX,REGOFF(2,EDI))
	ADD_L	(CONST(4),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span42)
	JMP	(.end)

.span03:
	MOV_L	(EBX,ECX)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(CONST(0),AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span03)
	JMP	(.end)

.span13:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	INC_L	(EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(CONST(0),AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span13)
	JMP	(.end)

.span23:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(CONST(2),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(CONST(0),AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span23)
	JMP	(.end)

.span33:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	MOV_W	(CONST(0xffff),AX)
	MOV_W	(AX,REGOFF(1,EDI))
	ADD_L	(CONST(3),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(CONST(0),AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span33)
	JMP	(.end)

.span43:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(CONST(0xff),AL)
	MOV_W	(AX,REGOFF(2,EDI))
	ADD_L	(CONST(4),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(CONST(0),AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span43)
	JMP	(.end)

.span04:
	MOV_L	(EBX,ECX)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span04)
	JMP	(.end)

.span14:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	INC_L	(EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span14)
	JMP	(.end)

.span24:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(CONST(2),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span24)
	JMP	(.end)

.span34:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	MOV_W	(CONST(0xffff),AX)
	MOV_W	(AX,REGOFF(1,EDI))
	ADD_L	(CONST(3),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span34)
	JMP	(.end)

.span44:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(CONST(0xff),AL)
	MOV_W	(AX,REGOFF(2,EDI))
	ADD_L	(CONST(4),EDI)
	MOV_L	(CONST(0xffffffff),EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.span44)
	JMP	(.end)

/*
 * These are for spans with midlcount == 0.
 * ESI = EDI = destination address,
 * EDX = #spans
 * BL = leftmask_arg, BH = rightmask_arg
 * ECX = vpitch_arg
 *
 */

.span0z0:
	JMP	(.end)

.span1z0:
	MOV_B	(BL,REGIND(EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span1z0)
	JMP	(.end)

.span2z0:
	MOV_B	(BL,AL)
	MOV_B	(CONST(0xff),AH)
.span2z0loop:
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span2z0loop)
	JMP	(.end)

.span3z0:
	MOV_W	(CONST(0xffff),AX)
.span3z0loop:
	MOV_B	(BL,REGIND(EDI))
	MOV_W	(AX,REGOFF(1,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span3z0loop)
	JMP	(.end)

.span4z0:
	MOV_L	(CONST(0xffffffff),EAX)
	MOV_B	(BL,AL)
.span4z0loop:
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span4z0loop)
	JMP	(.end)

.span0z1:
	MOV_B	(BL,REGIND(EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span0z1)
	JMP	(.end)

.span1z1:
	MOV_B	(BL,REGIND(EDI))
	MOV_B	(BH,REGOFF(1,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span1z1)
	JMP	(.end)

.span2z1:
	MOV_B	(BL,AL)
	MOV_B	(CONST(0xff),AH)
.span2z1loop:
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(BH,REGOFF(2,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span2z1loop)
	JMP	(.end)

.span3z1:
	MOV_W	(CONST(0xffff),AX)
.span3z1loop:
	MOV_B	(BL,REGIND(EDI))
	MOV_W	(AX,REGOFF(1,EDI))
	MOV_B	(BH,REGOFF(3,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span3z1loop)
	JMP	(.end)

.span4z1:
	MOV_L	(CONST(0xffffffff),EAX)
	MOV_B	(BL,AL)
.span4z1loop:
	MOV_L	(EAX,REGIND(EDI))
	MOV_B	(BH,REGOFF(4,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span4z1loop)
	JMP	(.end)

.span0z2:
	MOV_B	(CONST(0xff),AL)
	MOV_B	(BH,AH)
.span0z2loop:
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
 	JNZ 	(.span0z2loop)
	JMP	(.end)

.span1z2:
	MOV_B	(CONST(0xff),AL)
	MOV_B	(BH,AH)
.span1z2loop:
	MOV_B	(BL,REGIND(EDI))
	MOV_W	(AX,REGOFF(1,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span1z2loop)
	JMP	(.end)

.span2z2:
	MOV_B 	(BL,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_B	(CONST(0xff),BL)
.span2z2loop:
	MOV_W	(AX,REGIND(EDI))
	MOV_W	(BX,REGOFF(2,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span2z2loop)
	JMP	(.end)

.span3z2:
.span3z2loop:
	MOV_B	(BL,REGIND(EDI))
	MOV_W	(CONST(0xffff),AX)
	MOV_W	(AX,REGOFF(1,EDI))
	MOV_B	(BH,AH)
	MOV_W	(AX,REGOFF(3,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span3z2loop)
	JMP	(.end)

.span4z2:
.span4z2loop:
	MOV_B	(BL,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(CONST(0xff),AL)
	MOV_W	(AX,REGOFF(2,EDI))
	MOV_B	(BH,AH)
	MOV_W	(AX,REGOFF(4,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span4z2loop)
	JMP	(.end)

.span0z3:
	MOV_L	(CONST(0xffffffff),EAX)
	MOV_B	(BH,AL)
	MOV_B	(CONST(0),AH)
	ROL_L	(CONST(16),EAX)
.span0z3loop:
	MOV_L	(EAX,REGIND(EDI))	/* overrun */
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span0z3loop)
	JMP	(.end)

.span1z3:
	MOV_L	(CONST(0xffffffff),EAX)
	MOV_B	(BH,AL)
	MOV_B	(CONST(0),AH)
	ROL_L	(CONST(16),EAX)
.span1z3loop:
	MOV_B	(BL,REGIND(EDI))
	MOV_L	(EAX,REGOFF(1,EDI))	/* overrun */
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span1z3loop)
	JMP	(.end)

.span2z3:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_W	(AX,REGIND(EDI))
	MOV_L	(CONST(0xffffffff),EAX)
	MOV_B	(rightmask_arg,AL)
	MOV_B	(CONST(0),AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGOFF(2,EDI))	/* overrun */
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span2z3)
	JMP	(.end)

.span3z3:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(AL,REGIND(EDI))
	MOV_L	(CONST(0xffffffff),EAX)
	MOV_W	(AX,REGOFF(1,EDI))
	MOV_B	(rightmask_arg,AL)
	MOV_B	(CONST(0),AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGOFF(3,EDI))	/* overrun */
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span3z3)
	JMP	(.end)

.span4z3:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(CONST(0xff),AL)
	MOV_W	(AX,REGOFF(2,EDI))
	MOV_L	(CONST(0xffffffff),EAX)
	MOV_B	(rightmask_arg,AL)
	MOV_B	(CONST(0),AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGOFF(4,EDI))	/* overrun */
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span4z3)
	JMP	(.end)

.span0z4:
	MOV_L	(CONST(0xffffffff),EAX)
	MOV_B	(BH,AH)
	ROL_L	(CONST(16),EAX)
.span0z4loop:
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span0z4loop)
	JMP	(.end)

.span1z4:
	MOV_L	(CONST(0xffffffff),EAX)
	MOV_B	(BH,AH)
	ROL_L	(CONST(16),EAX)
.span1z4loop:
	MOV_B	(BL,REGIND(EDI))
	MOV_L	(EAX,REGOFF(1,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span1z4loop)
	JMP	(.end)

.span2z4:
	MOV_L	(CONST(0xffffffff),EAX)
	MOV_B	(BH,AH)
	ROL_L	(CONST(16),EAX)
	MOV_B	(CONST(0xff),BH)
.span2z4loop:
	MOV_W	(BX,REGIND(EDI))
	MOV_L	(EAX,REGOFF(2,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span2z4loop)
	JMP	(.end)

.span3z4:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(AL,REGIND(EDI))
	MOV_L	(CONST(0xffffffff),EAX)
	MOV_W	(AX,REGOFF(1,EDI))
	MOV_B	(rightmask_arg,AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGOFF(3,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span3z4)
	JMP	(.end)

.span4z4:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(CONST(0xff),AH)
	MOV_W	(AX,REGIND(EDI))
	MOV_L	(CONST(0xffffffff),EAX)
	MOV_W	(AX,REGOFF(2,EDI))
	MOV_B	(rightmask_arg,AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGOFF(4,EDI))
	ADD_L	(ECX,EDI)
	DEC_L	(EDX)
	JNZ 	(.span4z4)
	JMP	(.end)

/*
 * Equivalent function for stipple filling.
 *
 * Prototype:
 * CirrusColorExpandWriteStippleSpans( void *vaddr, int leftmask,
 *	int leftbcount, int midlcount, int rightbcount, int rightmask,
 *	int nspans, unsigned long stippleword, int vpitch );
 *
 * stippleword is the 32-bit stipple data.
 * leftmask and rightmask are already masked with the stipple.
 *
 */

.sjumptable:
	D_LONG .sspan00
	D_LONG .sspan10
	D_LONG .sspan20
	D_LONG .sspan30
	D_LONG .sspan40
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .sspan01
	D_LONG .sspan11
	D_LONG .sspan21
	D_LONG .sspan31
	D_LONG .sspan41
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .sspan02
	D_LONG .sspan12
	D_LONG .sspan22
	D_LONG .sspan32
	D_LONG .sspan42
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .sspan03
	D_LONG .sspan13
	D_LONG .sspan23
	D_LONG .sspan33
	D_LONG .sspan43
	D_LONG 0
	D_LONG 0
	D_LONG 0
	D_LONG .sspan04
	D_LONG .sspan14
	D_LONG .sspan24
	D_LONG .sspan34
	D_LONG .sspan44
	D_LONG 0
	D_LONG 0
	D_LONG 0

	ALIGNTEXT4

	GLOBL GLNAME(CirrusColorExpandWriteStippleSpans)
GLNAME(CirrusColorExpandWriteStippleSpans):

	PUSH_L	(EBP)
#ifdef USE_LAZY_ARGS
	MOV_L	(REGOFF(8,ESP),EBP)
	SUB_L	(CONST(8),EBP)
#else
	MOV_L	(ESP,EBP)
#endif
	PUSH_L	(ECX)
	PUSH_L	(EBX)
	PUSH_L	(EDI)
	PUSH_L	(ESI)

	MOV_L	(vaddr_arg,EDI)
	MOV_L	(nspans_arg,EDX)
	TEST_L	(EDX,EDX)
	JZ 	(.end2)

	MOV_L	(CONST(0),EAX)
	MOV_B	(rightbcount_arg,AL)
	SHL_B	(CONST(3),AL)
	ADD_B	(leftbcount_arg,AL)

	MOV_L	(midlcount_arg,EBX)

	MOV_L	(EDI,ESI)
	/* Call span function for specific leftbcount and rightbcount. */
	JMP	(CODEPTR(REGDIS(.sjumptable,EAX,4)))

.end2:
	POP_L	(ESI)
	POP_L	(EDI)
	POP_L	(EBX)
	POP_L	(ECX)
	POP_L	(EBP)
	RET

/*
 * Stippled spans.
 * ESI = EDI = destination address,
 * EBX = midlcount
 * EDX = #spans
 */

.sspan00:
	MOV_L	(EBX,ECX)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan00)
	JMP	(.end2)

.sspan10:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	INC_L	(EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan10)
	JMP	(.end2)

.sspan20:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(bitsplus3_arg,AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(CONST(2),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan20)
	JMP	(.end2)

.sspan30:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	MOV_W	(bitsplus2_arg,AX)
	MOV_W	(AX,REGOFF(1,EDI))
	ADD_L	(CONST(3),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan30)
	JMP	(.end2)

.sspan40:
	MOV_L	(bits_arg,EAX)
	MOV_L	(EBX,ECX)
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(CONST(4),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan40)
	JMP	(.end2)

.sspan01:
	MOV_L	(EBX,ECX)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan01)
	JMP	(.end2)

.sspan11:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	INC_L	(EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan11)
	JMP	(.end2)

.sspan21:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(bitsplus3_arg,AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(CONST(2),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan21)
	JMP	(.end2)

.sspan31:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	MOV_W	(bitsplus2_arg,AX)
	MOV_W	(AX,REGOFF(1,EDI))
	ADD_L	(CONST(3),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan31)
	JMP	(.end2)

.sspan41:
	MOV_L	(bits_arg,EAX)
	MOV_L	(EBX,ECX)
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(CONST(4),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan41)
	JMP	(.end2)

.sspan02:
	MOV_L	(EBX,ECX)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan02)
	JMP	(.end2)

.sspan12:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	INC_L	(EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan12)
	JMP	(.end2)

.sspan22:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(bitsplus3_arg,AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(CONST(2),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan22)
	JMP	(.end2)

.sspan32:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	MOV_W	(bitsplus2_arg,AX)
	MOV_W	(AX,REGOFF(1,EDI))
	ADD_L	(CONST(3),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan32)
	JMP	(.end2)

.sspan42:
	MOV_L	(bits_arg,EAX)
	MOV_L	(EBX,ECX)
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(CONST(4),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_B	(rightmask_arg,AH)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan42)
	JMP	(.end2)

.sspan03:
	MOV_L	(EBX,ECX)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGOFF(2,EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan03)
	JMP	(.end2)

.sspan13:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	INC_L	(EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGOFF(2,EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan13)
	JMP	(.end2)

.sspan23:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(bitsplus3_arg,AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(CONST(2),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGOFF(2,EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan23)
	JMP	(.end2)

.sspan33:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	MOV_W	(bitsplus2_arg,AX)
	MOV_W	(AX,REGOFF(1,EDI))
	ADD_L	(CONST(3),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGOFF(2,EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan33)
	JMP	(.end2)

.sspan43:
	MOV_L	(bits_arg,EAX)
	MOV_L	(EBX,ECX)
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(CONST(4),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	MOV_W	(AX,REGIND(EDI))
	MOV_B	(rightmask_arg,AL)
	MOV_B	(AL,REGOFF(2,EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan43)
	JMP	(.end2)

.sspan04:
	MOV_L	(EBX,ECX)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	ROL_L	(CONST(16),EAX)
	MOV_B	(rightmask_arg,AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan04)
	JMP	(.end2)

.sspan14:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	INC_L	(EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	ROL_L	(CONST(16),EAX)
	MOV_B	(rightmask_arg,AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan14)
	JMP	(.end2)

.sspan24:
	MOV_B	(leftmask_arg,AL)
	MOV_B	(bitsplus3_arg,AH)
	MOV_L	(EBX,ECX)
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(CONST(2),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	ROL_L	(CONST(16),EAX)
	MOV_B	(rightmask_arg,AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan24)
	JMP	(.end2)

.sspan34:
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EBX,ECX)
	MOV_B	(AL,REGIND(EDI))
	MOV_W	(bitsplus2_arg,AX)
	MOV_W	(AX,REGOFF(1,EDI))
	ADD_L	(CONST(3),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	ROL_L	(CONST(16),EAX)
	MOV_B	(rightmask_arg,AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan34)
	JMP	(.end2)

.sspan44:
	MOV_L	(bits_arg,EAX)
	MOV_L	(EBX,ECX)
	MOV_B	(leftmask_arg,AL)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(CONST(4),EDI)
	MOV_L	(bits_arg,EAX)
	REP
	STOS_L
	ROL_L	(CONST(16),EAX)
	MOV_B	(rightmask_arg,AH)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGIND(EDI))
	ADD_L	(vpitch_arg,ESI)
	MOV_L	(ESI,EDI)
	DEC_L	(EDX)
	JNZ 	(.sspan44)
	JMP	(.end2)


/*
 * Function to help writing tile spans in BY8 addressing mode with the latch
 * registers. Only needs to write bytes. CPU data doesn't matter, latch
 * registers are already loaded.
 *
 * destp	Destination pointer.
 * count	Number of bytes to be written on each scanline.
 * tbytes	Offset in bytes between each byte written.
 * linecount	Number of scanlines to write.
 * vpitch	Screen width.
 *
 * tbytes has a range from 1 to 8, corresponding to tiles of widths between
 * 8 and 64.
 * count >= 1
 *
 * C equivalent:

static void CirrusLatchWriteTileSpans(destp, count, tbytes, linecount, vpitch)
	unsigned char *destp;
	int tbytes, count, linecount, vpitch;
{
	int i;
	for (i = 0; i < linecount; i++) {
		int j;
		for (j = 0; j < count; j++)
			*(destp + j * tbytes) = 0;
		destp += vpitch;
	}
}

*/

#define destp_arg	REGOFF(8,EBP)
#define count_arg	REGOFF(12,EBP)
#define tbytes_arg	REGOFF(16,EBP)
#define linecount_arg	REGOFF(20,EBP)
#undef vpitch_arg
#define vpitch_arg	REGOFF(24,EBP)

.tilejumptable:
	D_LONG	0	/* Can't happen. */
	D_LONG	.tile1
	D_LONG	.tile2
	D_LONG	.tile3
	D_LONG	.tile4
	D_LONG	.tile5
	D_LONG	.tile6
	D_LONG	.tile7
	D_LONG	.tile8

	ALIGNTEXT4

	GLOBL GLNAME(CirrusLatchWriteTileSpans)
GLNAME(CirrusLatchWriteTileSpans):

	PUSH_L	(EBP)
#ifdef USE_LAZY_ARGS
	MOV_L	(REGOFF(8,ESP),EBP)
	SUB_L	(CONST(8),EBP)
#else
	MOV_L	(ESP,EBP)
#endif
	PUSH_L	(ECX)
	PUSH_L	(EDI)
	PUSH_L	(ESI)

	MOV_L	(linecount_arg,EDX)
	MOV_L	(destp_arg,EDI)
	MOV_L	(EDI,ESI)

	MOV_L	(tbytes_arg,EAX)
	JMP	(CODEPTR(REGDIS(.tilejumptable,EAX,4)))

.tileend:
	POP_L	(ESI)
	POP_L	(EDI)
	POP_L	(ECX)
	POP_L	(EBP)
	RET

/*
 * First digit indicates offset between each byte written.
 */

.tile1:
	MOV_L	(count_arg,ECX)
	MOV_L	(ESI,EDI)
	REP
	STOS_B
	ADD_L	(vpitch_arg,ESI)
	DEC_L	(EDX)
	JNZ	(.tile1)
	JMP	(.tileend)

.tile2:
	MOV_L	(count_arg,ECX)
	MOV_L	(ESI,EDI)
	CMP_L	(CONST(8),ECX)
	JL	(.tile2loop2)
.tile2loop:
	MOV_B	($0,REGIND(EDI))
	MOV_B	($0,REGOFF(2,EDI))
	MOV_B	($0,REGOFF(4,EDI))
	MOV_B	($0,REGOFF(6,EDI))
	MOV_B	($0,REGOFF(8,EDI))
	ADD_L	($16,EDI)		/* blend */
	SUB_L	(CONST(8),ECX)
	MOV_B	($0,REGOFF(-6,EDI))
	MOV_B	($0,REGOFF(-4,EDI))
	MOV_B	($0,REGOFF(-2,EDI))
	JZ	(.tile2cont)
	CMP_L	(CONST(8),ECX)
	JGE	(.tile2loop)
.tile2loop2:
	MOV_B	($0,REGIND(EDI))
	ADD_L	($2,EDI)
	DEC_L	(ECX)
	JNZ	(.tile2loop2)
.tile2cont:
	ADD_L	(vpitch_arg,ESI)
	DEC_L	(EDX)
	JNZ	(.tile2)
	JMP	(.tileend)

.tile3:
	MOV_L	(count_arg,ECX)
	MOV_L	(ESI,EDI)
.tile3loop:
	MOV_B	($0,REGIND(EDI))
	ADD_L	($3,EDI)
	DEC_L	(ECX)
	JNZ	(.tile3loop)
	ADD_L	(vpitch_arg,ESI)
	DEC_L	(EDX)
	JNZ	(.tile3)
	JMP	(.tileend)

.tile4:
	MOV_L	(count_arg,ECX)
	MOV_L	(ESI,EDI)
	CMP_L	(CONST(8),ECX)
	JL	(.tile4loop2)
.tile4loop:
	MOV_B	($0,REGIND(EDI))
	MOV_B	($0,REGOFF(4,EDI))
	MOV_B	($0,REGOFF(8,EDI))
	MOV_B	($0,REGOFF(12,EDI))
	MOV_B	($0,REGOFF(16,EDI))
	ADD_L	(CONST(32),EDI)		/* blend */
	SUB_L	(CONST(8),ECX)
	MOV_B	($0,REGOFF(-12,EDI))
	MOV_B	($0,REGOFF(-8,EDI))
	MOV_B	($0,REGOFF(-4,EDI))
	JZ	(.tile4cont)
	CMP_L	(CONST(8),ECX)
	JGE	(.tile4loop)
.tile4loop2:
	MOV_B	($0,REGIND(EDI))
	ADD_L	($4,EDI)
	DEC_L	(ECX)
	JNZ	(.tile4loop2)
.tile4cont:
	ADD_L	(vpitch_arg,ESI)
	DEC_L	(EDX)
	JNZ	(.tile4)
	JMP	(.tileend)

.tile5:
	MOV_L	(count_arg,ECX)
	MOV_L	(ESI,EDI)
.tile5loop:
	MOV_B	($0,REGIND(EDI))
	ADD_L	($5,EDI)
	DEC_L	(ECX)
	JNZ	(.tile5loop)
	ADD_L	(vpitch_arg,ESI)
	DEC_L	(EDX)
	JNZ	(.tile5)
	JMP	(.tileend)

.tile6:
	MOV_L	(count_arg,ECX)
	MOV_L	(ESI,EDI)
.tile6loop:
	MOV_B	($0,REGIND(EDI))
	ADD_L	($6,EDI)
	DEC_L	(ECX)
	JNZ	(.tile6loop)
	ADD_L	(vpitch_arg,ESI)
	DEC_L	(EDX)
	JNZ	(.tile6)
	JMP	(.tileend)

.tile7:
	MOV_L	(count_arg,ECX)
	MOV_L	(ESI,EDI)
.tile7loop:
	MOV_B	($0,REGIND(EDI))
	ADD_L	($7,EDI)
	DEC_L	(ECX)
	JNZ	(.tile7loop)
	ADD_L	(vpitch_arg,ESI)
	DEC_L	(EDX)
	JNZ	(.tile7)
	JMP	(.tileend)

.tile8:
	MOV_L	(count_arg,ECX)
	MOV_L	(ESI,EDI)
.tile8loop:
	MOV_B	($0,REGIND(EDI))
	ADD_L	($8,EDI)
	DEC_L	(ECX)
	JNZ	(.tile8loop)
	ADD_L	(vpitch_arg,ESI)
	DEC_L	(EDX)
	JNZ	(.tile8)
	JMP	(.tileend)


/*
 * Function to help vertical BitBlt in BY8 addressing mode with the latch
 * registers.
 *
 * srcp 	Source pointer.
 * destp	Destination pointer.
 * count	Number of bytes to be copied for each scanline.
 * linecount	Number of scanlines to write.
 * vpitch	Screen width.
 *
 * count >= 1
 *
 * C equivalent:

void CirrusLatchCopySpans(srcp, destp, bcount, n, destpitch)
	unsigned char *srcp;
	unsigned char *destp;
	int bcount;
	int n;
	int destpitch;
{
	int i;
	for (i = 0; i < n; i++) {
		for (j = 0; j < bcount; j++)
			*(destp + j) = *(srcp + j);
		srcp += destpitch;
		destp += destpitch;
	}
}

*/

#define srcp_arg	REGOFF(8,EBP)
#undef destp_arg
#define destp_arg	REGOFF(12,EBP)
#define bcount_arg	REGOFF(16,EBP)
#define linecount_arg	REGOFF(20,EBP)
#undef vpitch_arg
#define vpitch_arg	REGOFF(24,EBP)


.latchcopyjumptable:
	D_LONG	.latchcopy0
	D_LONG	.latchcopy1
	D_LONG	.latchcopy2
	D_LONG	.latchcopy3
	D_LONG	.latchcopy4
	D_LONG	.latchcopy5
	D_LONG	.latchcopy6
	D_LONG	.latchcopy7

	ALIGNTEXT4

	GLOBL GLNAME(CirrusLatchCopySpans)
GLNAME(CirrusLatchCopySpans):

	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	PUSH_L	(EBX)
	PUSH_L	(ECX)
	PUSH_L	(EDI)
	PUSH_L	(ESI)

	MOV_L	(linecount_arg,EDX)
	MOV_L	(srcp_arg,ESI)
	MOV_L	(destp_arg,EDI)

	MOV_L	(vpitch_arg,EBX)
	MOV_L	(bcount_arg,EAX)
	AND_L	(CONST(0xfffffff8),EAX)
	SUB_L	(EAX,EBX)		/* Value to add each scanline. */

	MOV_L	(bcount_arg,EAX)
	AND_L	(CONST(7),EAX)
	JMP	(CODEPTR(REGDIS(.latchcopyjumptable,EAX,4)))

.latchcopyend:
	POP_L	(ESI)
	POP_L	(EDI)
	POP_L	(ECX)
	POP_L	(EBX)
	POP_L	(EBP)
	RET

/*
 * First digit indicates <number of bytes to copy> MOD 8.
 */

.latchcopy0:
	MOV_L	(bcount_arg,ECX)
	SHR_L	(CONST(3),ECX)
	JZ	(.latchcopyend)
.latchcopy0loop:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	MOV_B	(REGOFF(3,ESI),AL)
	MOV_B	(CONST(0),REGOFF(3,EDI))
	MOV_B	(REGOFF(4,ESI),AL)
	MOV_B	(CONST(0),REGOFF(4,EDI))
	MOV_B	(REGOFF(5,ESI),AL)
	MOV_B	(CONST(0),REGOFF(5,EDI))
	MOV_B	(REGOFF(6,ESI),AL)
	MOV_B	(CONST(0),REGOFF(6,EDI))
	ADD_L	(CONST(8),EDI)			/* blend */
	MOV_B	(REGOFF(7,ESI),AL)
	MOV_B	(CONST(0),REGOFF(-1,EDI))
	ADD_L	(CONST(8),ESI)
	DEC_L	(ECX)
	JNZ	(.latchcopy0loop)
	ADD_L	(EBX,ESI)
	ADD_L	(EBX,EDI)
	DEC_L	(EDX)
	JNZ	(.latchcopy0)
	JMP	(.latchcopyend)

.latchcopy1:
	MOV_L	(bcount_arg,ECX)
	SHR_L	(CONST(3),ECX)
	JZ	(.latchcopy1cont)
.latchcopy1loop:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	MOV_B	(REGOFF(3,ESI),AL)
	MOV_B	(CONST(0),REGOFF(3,EDI))
	MOV_B	(REGOFF(4,ESI),AL)
	MOV_B	(CONST(0),REGOFF(4,EDI))
	MOV_B	(REGOFF(5,ESI),AL)
	MOV_B	(CONST(0),REGOFF(5,EDI))
	MOV_B	(REGOFF(6,ESI),AL)
	MOV_B	(CONST(0),REGOFF(6,EDI))
	ADD_L	(CONST(8),EDI)			/* blend */
	MOV_B	(REGOFF(7,ESI),AL)
	MOV_B	(CONST(0),REGOFF(-1,EDI))
	ADD_L	(CONST(8),ESI)
	DEC_L	(ECX)
	JNZ	(.latchcopy1loop)
.latchcopy1cont:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	ADD_L	(EBX,ESI)
	ADD_L	(EBX,EDI)
	DEC_L	(EDX)
	JNZ	(.latchcopy1)
	JMP	(.latchcopyend)

.latchcopy2:
	MOV_L	(bcount_arg,ECX)
	SHR_L	(CONST(3),ECX)
	JZ	(.latchcopy2cont)
.latchcopy2loop:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	MOV_B	(REGOFF(3,ESI),AL)
	MOV_B	(CONST(0),REGOFF(3,EDI))
	MOV_B	(REGOFF(4,ESI),AL)
	MOV_B	(CONST(0),REGOFF(4,EDI))
	MOV_B	(REGOFF(5,ESI),AL)
	MOV_B	(CONST(0),REGOFF(5,EDI))
	MOV_B	(REGOFF(6,ESI),AL)
	MOV_B	(CONST(0),REGOFF(6,EDI))
	ADD_L	(CONST(8),EDI)			/* blend */
	MOV_B	(REGOFF(7,ESI),AL)
	MOV_B	(CONST(0),REGOFF(-1,EDI))
	ADD_L	(CONST(8),ESI)
	DEC_L	(ECX)
	JNZ	(.latchcopy2loop)
.latchcopy2cont:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	ADD_L	(EBX,ESI)
	ADD_L	(EBX,EDI)
	DEC_L	(EDX)
	JNZ	(.latchcopy2)
	JMP	(.latchcopyend)

.latchcopy3:
	MOV_L	(bcount_arg,ECX)
	SHR_L	(CONST(3),ECX)
	JZ	(.latchcopy3cont)
.latchcopy3loop:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	MOV_B	(REGOFF(3,ESI),AL)
	MOV_B	(CONST(0),REGOFF(3,EDI))
	MOV_B	(REGOFF(4,ESI),AL)
	MOV_B	(CONST(0),REGOFF(4,EDI))
	MOV_B	(REGOFF(5,ESI),AL)
	MOV_B	(CONST(0),REGOFF(5,EDI))
	MOV_B	(REGOFF(6,ESI),AL)
	MOV_B	(CONST(0),REGOFF(6,EDI))
	ADD_L	(CONST(8),EDI)			/* blend */
	MOV_B	(REGOFF(7,ESI),AL)
	MOV_B	(CONST(0),REGOFF(-1,EDI))
	ADD_L	(CONST(8),ESI)
	DEC_L	(ECX)
	JNZ	(.latchcopy3loop)
.latchcopy3cont:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	ADD_L	(EBX,ESI)
	ADD_L	(EBX,EDI)
	DEC_L	(EDX)
	JNZ	(.latchcopy3)
	JMP	(.latchcopyend)

.latchcopy4:
	MOV_L	(bcount_arg,ECX)
	SHR_L	(CONST(3),ECX)
	JZ	(.latchcopy4cont)
.latchcopy4loop:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	MOV_B	(REGOFF(3,ESI),AL)
	MOV_B	(CONST(0),REGOFF(3,EDI))
	MOV_B	(REGOFF(4,ESI),AL)
	MOV_B	(CONST(0),REGOFF(4,EDI))
	MOV_B	(REGOFF(5,ESI),AL)
	MOV_B	(CONST(0),REGOFF(5,EDI))
	MOV_B	(REGOFF(6,ESI),AL)
	MOV_B	(CONST(0),REGOFF(6,EDI))
	ADD_L	(CONST(8),EDI)			/* blend */
	MOV_B	(REGOFF(7,ESI),AL)
	MOV_B	(CONST(0),REGOFF(-1,EDI))
	ADD_L	(CONST(8),ESI)
	DEC_L	(ECX)
	JNZ	(.latchcopy4loop)
.latchcopy4cont:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	MOV_B	(REGOFF(3,ESI),AL)
	MOV_B	(CONST(0),REGOFF(3,EDI))
	ADD_L	(EBX,ESI)
	ADD_L	(EBX,EDI)
	DEC_L	(EDX)
	JNZ	(.latchcopy4)
	JMP	(.latchcopyend)

.latchcopy5:
	MOV_L	(bcount_arg,ECX)
	SHR_L	(CONST(3),ECX)
	JZ	(.latchcopy5cont)
.latchcopy5loop:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	MOV_B	(REGOFF(3,ESI),AL)
	MOV_B	(CONST(0),REGOFF(3,EDI))
	MOV_B	(REGOFF(4,ESI),AL)
	MOV_B	(CONST(0),REGOFF(4,EDI))
	MOV_B	(REGOFF(5,ESI),AL)
	MOV_B	(CONST(0),REGOFF(5,EDI))
	MOV_B	(REGOFF(6,ESI),AL)
	MOV_B	(CONST(0),REGOFF(6,EDI))
	ADD_L	(CONST(8),EDI)			/* blend */
	MOV_B	(REGOFF(7,ESI),AL)
	MOV_B	(CONST(0),REGOFF(-1,EDI))
	ADD_L	(CONST(8),ESI)
	DEC_L	(ECX)
	JNZ	(.latchcopy5loop)
.latchcopy5cont:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	MOV_B	(REGOFF(3,ESI),AL)
	MOV_B	(CONST(0),REGOFF(3,EDI))
	MOV_B	(REGOFF(4,ESI),AL)
	MOV_B	(CONST(0),REGOFF(4,EDI))
	ADD_L	(EBX,ESI)
	ADD_L	(EBX,EDI)
	DEC_L	(EDX)
	JNZ	(.latchcopy5)
	JMP	(.latchcopyend)

.latchcopy6:
	MOV_L	(bcount_arg,ECX)
	SHR_L	(CONST(3),ECX)
	JZ	(.latchcopy6cont)
.latchcopy6loop:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	MOV_B	(REGOFF(3,ESI),AL)
	MOV_B	(CONST(0),REGOFF(3,EDI))
	MOV_B	(REGOFF(4,ESI),AL)
	MOV_B	(CONST(0),REGOFF(4,EDI))
	MOV_B	(REGOFF(5,ESI),AL)
	MOV_B	(CONST(0),REGOFF(5,EDI))
	MOV_B	(REGOFF(6,ESI),AL)
	MOV_B	(CONST(0),REGOFF(6,EDI))
	ADD_L	(CONST(8),EDI)			/* blend */
	MOV_B	(REGOFF(7,ESI),AL)
	MOV_B	(CONST(0),REGOFF(-1,EDI))
	ADD_L	(CONST(8),ESI)
	DEC_L	(ECX)
	JNZ	(.latchcopy6loop)
.latchcopy6cont:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	MOV_B	(REGOFF(3,ESI),AL)
	MOV_B	(CONST(0),REGOFF(3,EDI))
	MOV_B	(REGOFF(4,ESI),AL)
	MOV_B	(CONST(0),REGOFF(4,EDI))
	MOV_B	(REGOFF(5,ESI),AL)
	MOV_B	(CONST(0),REGOFF(5,EDI))
	ADD_L	(EBX,ESI)
	ADD_L	(EBX,EDI)
	DEC_L	(EDX)
	JNZ	(.latchcopy6)
	JMP	(.latchcopyend)

.latchcopy7:
	MOV_L	(bcount_arg,ECX)
	SHR_L	(CONST(3),ECX)
	JZ	(.latchcopy7cont)
.latchcopy7loop:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	MOV_B	(REGOFF(3,ESI),AL)
	MOV_B	(CONST(0),REGOFF(3,EDI))
	MOV_B	(REGOFF(4,ESI),AL)
	MOV_B	(CONST(0),REGOFF(4,EDI))
	MOV_B	(REGOFF(5,ESI),AL)
	MOV_B	(CONST(0),REGOFF(5,EDI))
	MOV_B	(REGOFF(6,ESI),AL)
	MOV_B	(CONST(0),REGOFF(6,EDI))
	ADD_L	(CONST(8),EDI)			/* blend */
	MOV_B	(REGOFF(7,ESI),AL)
	MOV_B	(CONST(0),REGOFF(-1,EDI))
	ADD_L	(CONST(8),ESI)
	DEC_L	(ECX)
	JNZ	(.latchcopy7loop)
.latchcopy7cont:
	MOV_B	(REGIND(ESI),AL)
	MOV_B	(CONST(0),REGIND(EDI))
	MOV_B	(REGOFF(1,ESI),AL)
	MOV_B	(CONST(0),REGOFF(1,EDI))
	MOV_B	(REGOFF(2,ESI),AL)
	MOV_B	(CONST(0),REGOFF(2,EDI))
	MOV_B	(REGOFF(3,ESI),AL)
	MOV_B	(CONST(0),REGOFF(3,EDI))
	MOV_B	(REGOFF(4,ESI),AL)
	MOV_B	(CONST(0),REGOFF(4,EDI))
	MOV_B	(REGOFF(5,ESI),AL)
	MOV_B	(CONST(0),REGOFF(5,EDI))
	MOV_B	(REGOFF(6,ESI),AL)
	MOV_B	(CONST(0),REGOFF(6,EDI))
	ADD_L	(EBX,ESI)
	ADD_L	(EBX,EDI)
	DEC_L	(EDX)
	JNZ	(.latchcopy7)
	JMP	(.latchcopyend)

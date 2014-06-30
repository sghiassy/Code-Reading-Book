/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_textblt.s,v 3.9 1996/12/23 06:56:58 dawes Exp $ */
/*
 *
 * Copyright 1993 by H. Hanemaayer, Utrecht, The Netherlands
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
/* $XConsortium: cir_textblt.s /main/7 1996/10/23 18:48:34 kaleb $ */

/*
 * This low-level routine writes a text string bitmap video memory for the
 * blitter, which must be setup for system-memory-to-video-memory BLT.
 * The video address where the data is written doesn't matter. Each bitmap
 * scanline transmitted is padded to a byte boundary; the bitmap is
 * transfered in 16-bit words. This means that bytes from different scanlines
 * have to be combined if necessary.
 *
 * This function is used by the 5426 and 5428.
 *
 * Prototype:
 * CirrusTextTransfer( int nglyph, int height, unsigned long **glyphp,
 *     int glyphwidth, void *vaddr )
 *
 * nglyph is the number of characters
 * height is the height of the area.
 * glyphp is an array of pointers to character bitmaps (stored as one 32-bit
 *     word per line)
 * glyphwidth is the width of the font in pixels.
 * vaddr is a video memory address (doesn't really matter).
 *
 * Optimized for 486 pipeline (somewhat out-of-place instructions are
 * labeled with 'blend').
 */

#include "assyntax.h"

 	FILE("cir_textblt.S")

	AS_BEGIN

/* Definition of stack frame function arguments. */

#define nglyph_arg	REGOFF(8,EBP)
#define height_arg	REGOFF(12,EBP)
#define glyphp_arg	REGOFF(16,EBP)
#define glyphwidth_arg	REGOFF(20,EBP)
#define vaddr_arg	REGOFF(24,EBP)

#define BYTE_REVERSED	GLNAME(byte_reversed)

/* I assume %eax and %edx can be trashed. */
/* Saving %ebx and %ecx may be unnecessary. */

	SEG_TEXT

/*
 * This is version of the above function that exclusively does 32-bit
 * transfers, as required by the 543x (also works on the 5426). It can
 * handle font widths up to 32. 
 *
 * September 1996: Changed to use PCI burst accesses.
 */

#define bound_var	REGIND(ESP)

	ALIGNTEXT4

	GLOBL GLNAME(CirrusTransferText32bit)
GLNAME(CirrusTransferText32bit):

	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	PUSH_L	(EBX)
	PUSH_L	(ECX)
	PUSH_L	(ESI)
	PUSH_L	(EDI)
	SUB_L	(CONST(4),ESP)		/* one local variable */

	XOR_L   (EDX,EDX)		/* line = 0 */
	MOV_B	(CONST(0),CL)		/* shift = 0 */
	MOV_B	(glyphwidth_arg,CH)
	MOV_L	(EDX,EAX)		/* dworddata = 0 */

.line_loop2:
	CMP_L	(height_arg,EDX)
	JGE	(.finished2)

	MOV_L	(glyphp_arg,EDI)	/* glyphp */
	MOV_L	(nglyph_arg,ESI)
	LEA_L	(REGBISD(EDI,ESI,4,0),ESI) /* &(glyphp[nglyph]) */
	MOV_L	(ESI,bound_var)

.char_loop2:
	CMP_L	(bound_var,EDI)
	JGE 	(.line_finished2)

	MOV_L	(REGIND(EDI),EBX)	/* glyphp[chari] */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX) /* data = glyphp[chari][line] */
	MOV_L	(EBX,ESI)
	SHL_L	(CL,EBX)
	ADD_L	(EBX,EAX)		/* dworddata += low(data << shift) */
	ADD_B	(CH,CL)			/* shift += glyphwidth */
	ADD_L	(CONST(4),EDI)		/* glyphp += 4 */
	CMP_B	(CONST(32),CL)		/* shift < 32? */
	JL	(.char_loop2)

	/* At this point, high32(data << old_shift) is equal to */
	/* ESI >> (32 - (shift - glyphwidth)) */
	MOV_B   (CL,BL)
	SUB_B	(CH,CL)		/* shift - glyphwidth */
	NEG_B	(CL)
	ADD_B	(CONST(32),CL)
	SHR_L	(CL,ESI)
	MOV_B	(BL,CL)

	/* Write a 32-bit word. */
	XOR_L	(EBX,EBX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	ROL_L	(CONST(16),EAX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	MOV_L	(vaddr_arg,EBX)
	ROL_L	(CONST(16),EAX)
	SUB_B	(CONST(32),CL)		/* shift -= 32 (blend) */
	MOV_L	(EAX,REGIND(EBX))	/* *(short)vaddr = dworddata */
	MOV_L	(ESI,EAX)		/* dworddata = high(data << shift) */
	JMP	(.char_loop2)

.line_finished2:
	INC_L	(EDX)			/* line++ */
	AND_B	(CL,CL)
	JZ 	(.line_loop2)

	/* Make sure last bits of scanline are padded to a byte boundary. */
	ADD_B	(CONST(7),CL)
	AND_B	(CONST(56),CL)
	CMP_B	(CONST(32),CL)		/* extra 32-bit word to write? */
	JL 	(.line_loop2)

	/* Write a 32-bit word. */
	XOR_L	(EBX,EBX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	ROL_L	(CONST(16),EAX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	MOV_L	(vaddr_arg,EBX)
	ROL_L	(CONST(16),EAX)
	SUB_B	(CONST(32),CL)		/* shift -= 32 (blend) */
	MOV_L	(EAX,REGOFF(4,EBX))	/* *vaddr = dworddata */
	MOV_L	(CONST(0),EAX)		/* dworddata = 0 */
	JMP	(.line_loop2)

.finished2:
	/* Handle the last fews bits and alignment. */
	TEST_B	(CL,CL)
	JZ	(.end2)
	XOR_L	(EBX,EBX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),DL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),DH)
	SHR_L	(CONST(16),EAX)		/* shr preferred over rol */
	ROL_L	(CONST(16),EDX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),DL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),DH)
	ROL_L	(CONST(16),EDX)
 
	/* No need to check for alignment, multiple of 32-bit words is */
	/* guaranteed. */
	MOV_L	(vaddr_arg,EBX)
	MOV_L	(EDX,REGOFF(4,EBX))	/* write 32-bit word */

.end2:
	ADD_L	(CONST(4),ESP)
	POP_L	(EDI)
	POP_L	(ESI)
	POP_L	(ECX)
	POP_L	(EBX)
	POP_L	(EBP)
	RET

/*
 * This a 32-bit transfer function that checks for special font widths,
 * with fontwidth-specific routines that write stretches of characters
 * efficiently.
 *
 * September 1996: Changed to use PCI burst accesses.
 */

#define stretchsize_var REGIND(ESP)
#define widthcode_var	REGOFF(4,ESP)
#define glyphsleft_var	REGOFF(8,ESP)

	ALIGNTEXT4

	GLOBL GLNAME(CirrusTransferText32bitSpecial)
GLNAME(CirrusTransferText32bitSpecial):

	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	PUSH_L	(EBX)
	PUSH_L	(ECX)
	PUSH_L	(ESI)
	PUSH_L	(EDI)
	SUB_L	(CONST(12),ESP)		/* three local variables */

	XOR_L   (EDX,EDX)		/* line = 0 */
	MOV_B	(CONST(0),CL)		/* shift = 0 */
	MOV_B	(glyphwidth_arg,CH)
	MOV_L	(EDX,EAX)		/* dworddata = 0 */

	CMP_B 	(CONST(8),CH)
	JZ	(.fontwidth8)
	CMP_B	(CONST(16),CH)
	JZ	(.fontwidth16)
	CMP_B	(CONST(32),CH)
	JZ	(.fontwidth32)
	CMP_B	(CONST(6),CH)
	JZ	(.fontwidth6)
	CMP_B	(CONST(14),CH)
	JZ	(.fontwidth14)
#if 0
	CMP_B	(CONST(12),CH)
	JZ	(.fontwidth12)
	CMP_B	(CONST(24),CH)
	JZ	(.fontwidth24)
#endif

	/* not reached */
	JMP	(.line_loop5)

.fontwidth6:
	MOV_L	(CONST(16),stretchsize_var)
	MOV_L	(CONST(.width6code),widthcode_var)
	JMP 	(.line_loop5)

.fontwidth8:
	MOV_L	(CONST(4),stretchsize_var)
	MOV_L	(CONST(.width8code),widthcode_var)
	JMP 	(.line_loop5)

.fontwidth16:
	MOV_L	(CONST(2),stretchsize_var)
	MOV_L	(CONST(.width16code),widthcode_var)
	JMP 	(.line_loop5)

.fontwidth32:
	MOV_L	(CONST(1),stretchsize_var)
	MOV_L	(CONST(.width16code),widthcode_var)
	JMP 	(.line_loop5)

.fontwidth14:
	MOV_L	(CONST(5),stretchsize_var)
	MOV_L	(CONST(.width14code),widthcode_var)
	JMP 	(.line_loop5)

.line_loop5:
	CMP_L	(height_arg,EDX)
	JGE	(.finished5)

	MOV_L	(glyphp_arg,EDI)	/* glyphp */
	MOV_L	(nglyph_arg,ESI)
#if 0
	LEA_L	(REGBISD(EDI,ESI,4,0),EBX) /* &(glyphp[nglyph]) */
	MOV_L	(EBX,bound_var)
#endif
	INC_L	(ESI)
	MOV_L	(ESI,glyphsleft_var)

.char_loop5:
	MOV_L	(glyphsleft_var,EBX)
	DEC_L	(EBX)
	JZ	(.line_finished5)
	MOV_L	(EBX,glyphsleft_var)
	TEST_B	(CL,CL)
	JNZ	(.continue_loop5)		/* If shift == 0 */
	CMP_L	(stretchsize_var,EBX)		/* check for stretch. */
	JGE	(.dostretch)

.continue_loop5:
	MOV_L	(REGIND(EDI),EBX)	/* glyphp[chari] */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX) /* data = glyphp[chari][line] */
	MOV_L	(EBX,ESI)
	SHL_L	(CL,EBX)
	ADD_L	(EBX,EAX)		/* dworddata += low(data << shift) */
	ADD_B	(CH,CL)			/* shift += glyphwidth */
	ADD_L	(CONST(4),EDI)		/* glyphp += 4 */
	CMP_B	(CONST(32),CL)		/* shift < 32? */
	JL	(.char_loop5)

	/* At this point, high32(data << old_shift) is equal to */
	/* ESI >> (32 - (shift - glyphwidth)) */
	MOV_B   (CL,BL)
	SUB_B	(CH,CL)		/* shift - glyphwidth */
	NEG_B	(CL)
	ADD_B	(CONST(32),CL)
	SHR_L	(CL,ESI)
	MOV_B	(BL,CL)

	/* Write a 32-bit word. */
	XOR_L	(EBX,EBX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	ROL_L	(CONST(16),EAX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	MOV_L	(vaddr_arg,EBX)
	ROL_L	(CONST(16),EAX)
	SUB_B	(CONST(32),CL)		/* shift -= 32 (blend) */
	MOV_L	(EAX,REGIND(EBX))	/* *(short)vaddr = dworddata */
	MOV_L	(ESI,EAX)		/* dworddata = high(data << shift) */
	JMP	(.char_loop5)

.line_finished5:
	INC_L	(EDX)			/* line++ */
	AND_B	(CL,CL)
	JZ 	(.line_loop5)

	/* Make sure last bits of scanline are padded to a byte boundary. */
	ADD_B	(CONST(7),CL)
	AND_B	(CONST(56),CL)
	CMP_B	(CONST(32),CL)		/* extra 32-bit word to write? */
	JL 	(.line_loop5)

	/* Write a 32-bit word. */
	XOR_L	(EBX,EBX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	ROL_L	(CONST(16),EAX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	MOV_L	(vaddr_arg,EBX)
	ROL_L	(CONST(16),EAX)
	SUB_B	(CONST(32),CL)		/* shift -= 32 (blend) */
	MOV_L	(EAX,REGIND(EBX))	/* *vaddr = dworddata */
	MOV_L	(CONST(0),EAX)		/* dworddata = 0 */
	JMP	(.line_loop5)

.finished5:
	/* Handle the last fews bits and alignment. */
	TEST_B	(CL,CL)
	JZ	(.end5)
	XOR_L	(EBX,EBX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),DL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),DH)
	SHR_L	(CONST(16),EAX)		/* shr preferred over rol */
	ROL_L	(CONST(16),EDX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),DL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),DH)
	ROL_L	(CONST(16),EDX)
 
	/* No need to check for alignment, multiple of 32-bit words is */
	/* guaranteed. */
	MOV_L	(vaddr_arg,EBX)
	MOV_L	(EDX,REGIND(EBX))	/* write 32-bit word */

.end5:
	ADD_L	(CONST(12),ESP)
	POP_L	(EDI)
	POP_L	(ESI)
	POP_L	(ECX)
	POP_L	(EBX)
	POP_L	(EBP)
	RET

.dostretch:
	JMP	(CODEPTR(widthcode_var))

/* Routines for specific font widths; ECX can be used as long as it is */
/* properly restored. */

.width8code:	/* Speedup ~ 80% */
	MOV_L	(glyphsleft_var,ESI)
.width8loop:
	/* Width 8; process 4 chars. */
	MOV_L	(REGIND(EDI),EBX)	/* glyphp[chari] */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX) /* data = glyphp[chari][line] */
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_L	(REGOFF(4,EDI),EBX)	/* Next char */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	SHL_L	(CONST(16),EAX)
	MOV_L	(REGOFF(8,EDI),EBX)	/* Next char */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	ADD_L	(CONST(16),EDI)		/* glyphp += 16 (blend) */
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_L	(REGOFF(-4,EDI),EBX)	/* Next char */
	MOV_L	(vaddr_arg,ECX)		/* blend */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	SUB_L	(CONST(4),ESI)		/* blend */
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	ROL_L	(CONST(16),EAX)

	MOV_L	(EAX,REGIND(ECX))	/* Write dword. */

	CMP_L	(CONST(4),ESI)
	JGE	(.width8loop)

	MOV_W	(CONST(0x0800),CX)	/* shift = 0, glyphwidth = 8 */
	XOR_L	(EAX,EAX)		/* dworddata = 0 */
	/* Ugly loop semantics. */
	TEST_L	(ESI,ESI)
	JZ	(.line_finished5)
	MOV_L	(ESI,glyphsleft_var)
	JMP	(.continue_loop5)

.width16code:
	MOV_L	(glyphsleft_var,ESI)
.width16loop:
	/* Width 16; process 2 chars. */
	XOR_L	(ECX,ECX)
	MOV_L	(REGIND(EDI),EBX)	/* glyphp[chari] */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX) /* data = glyphp[chari][line] */
	MOV_B	(BL,CL)
	MOV_B	(REGOFF(BYTE_REVERSED,ECX),AL)
	MOV_B	(BH,CL)
	MOV_B	(REGOFF(BYTE_REVERSED,ECX),AH)
	SHL_L	(CONST(16),EAX)
	MOV_L	(REGOFF(8,EDI),EBX)	/* Next char */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(BL,CL)
	MOV_B	(REGOFF(BYTE_REVERSED,ECX),AL)
	MOV_B	(BH,CL)
	MOV_B	(REGOFF(BYTE_REVERSED,ECX),AH)
	ROL_L	(CONST(16),EAX)

	MOV_L	(vaddr_arg,EBX)
	MOV_L	(EAX,REGIND(EBX))	/* Write dword. */

	ADD_L	(CONST(8),EDI)		/* glyphp += 8 */
	SUB_L	(CONST(2),ESI)
	CMP_L	(CONST(2),ESI)
	JGE	(.width16loop)

	MOV_W	(CONST(0x1000),CX)	/* shift = 0, glyphwidth = 16 */
	XOR_L	(EAX,EAX)		/* dworddata = 0 */
	/* Ugly loop semantics. */
	TEST_L	(ESI,ESI)
	JZ	(.line_finished5)
	MOV_L	(ESI,glyphsleft_var)
	JMP	(.continue_loop5)

.width32code:
	MOV_L	(glyphsleft_var,ESI)
.width32loop:
	/* Width 32; process 4 chars. */
	XOR_L	(ECX,ECX)
	MOV_L	(REGIND(EDI),EBX)	/* glyphp[chari] */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX) /* data = glyphp[chari][line] */
	MOV_B	(BL,CL)
	MOV_B	(REGOFF(BYTE_REVERSED,ECX),AL)
	MOV_B	(BH,CL)
	MOV_B	(REGOFF(BYTE_REVERSED,ECX),AH)
	SHL_L	(CONST(16),EAX)
	SHR_L	(CONST(16),EBX)
	MOV_B	(BL,CL)
	MOV_B	(REGOFF(BYTE_REVERSED,ECX),AL)
	MOV_B	(BH,CL)
	MOV_B	(REGOFF(BYTE_REVERSED,ECX),AH)
	ROL_L	(CONST(16),EAX)

	MOV_L	(vaddr_arg,EBX)
	MOV_L	(EAX,REGIND(EBX))	/* Write dword. */

	ADD_L	(CONST(4),EDI)		/* glyphp += 4 */
	DEC_L	(ESI)
	JNZ	(.width32loop)

	MOV_W	(CONST(0x2000),CX)	/* shift = 0, glyphwidth = 32 */
	XOR_L	(EAX,EAX)		/* dworddata = 0 */
	JMP	(.line_finished5)

.width6code:	/* Speedup ~ 35% */
	MOV_L	(glyphsleft_var,ESI)
.width6loop:
	/* Width 6; process 16 chars. */
	MOV_L	(REGIND(EDI),EBX)	/* glyphp[chari] */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX) /* data = glyphp[chari][line] */
	MOV_L	(ECX,EAX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_L	(REGOFF(4,EDI),EBX)	/* Next char */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	MOV_B	(CL,BL)
	SHR_L	(CONST(6),ECX)		/* byte2 >> 6 */
	ADD_L	(ECX,EAX)
	AND_B	(CONST(0x3c),BL)
	SHL_L	(CONST(10),EBX)		/* (byte2 & 0x3c) << 10 */
	ADD_L	(EBX,EAX)
	MOV_L	(REGOFF(8,EDI),EBX)	/* Next char */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	MOV_B	(CL,BL)
	AND_B	(CONST(0xf0),CL)
	SHL_L	(CONST(4),ECX)		/* (byte3 & 0xf0) << 4 */
	ADD_L	(ECX,EAX)
	AND_B	(CONST(0x0c),BL)
	SHL_L	(CONST(20),EBX)		/* (byte3 & 0x0c) << 20 */
	ADD_L	(EBX,EAX)
	MOV_L	(REGOFF(12,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	SHL_L	(CONST(14),ECX)		/* byte4 << 14 */
	ADD_L	(ECX,EAX)
	MOV_L	(REGOFF(16,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	SHL_L	(CONST(24),ECX)		/* byte5 << 24 */
	ADD_L	(ECX,EAX)
	MOV_L	(REGOFF(20,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	MOV_B	(CL,BL)
	AND_B	(CONST(0x0c0),CL)
	SHL_L	(CONST(18),ECX)		/* (byte6 & 0xc0) << 18 */
	ADD_L	(ECX,EAX)
	MOV_L	(vaddr_arg,ECX)
	MOV_L	(EAX,REGOFF(4,ECX))	/* Write dword. */

	AND_B	(CONST(0x3c),BL)
	SHL_L	(CONST(2),EBX)		/* (byte6 & 0x3c) << 2 */
	MOV_L	(EBX,EAX)
	MOV_L	(REGOFF(24,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	MOV_B	(CL,BL)
	AND_B	(CONST(0xf0),CL)
	SHR_L	(CONST(4),ECX)		/* (byte7 & 0xf0) >> 4 */
	ADD_L	(ECX,EAX)
	AND_B	(CONST(0x0c),BL)
	SHL_L	(CONST(12),EBX)		/* (byte7 & 0x0c) << 12 */
	ADD_L	(EBX,EAX)
	MOV_L	(REGOFF(28,EDI),EBX)	/* Next char */
	SUB_L	(CONST(16),ESI)		/* blend */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	SHL_L	(CONST(6),ECX)		/* byte8 << 6 */
	ADD_L	(ECX,EAX)

	MOV_L	(REGOFF(32,EDI),EBX)	/* Next char */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	XOR_L	(ECX,ECX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	SHL_L	(CONST(16),ECX)		/* byte9 << 16 */
	ADD_L	(ECX,EAX)
	MOV_L	(REGOFF(36,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	MOV_B	(CL,BL)
	AND_B	(CONST(0xc0),CL)
	SHL_L	(CONST(10),ECX)		/* (byte10 & 0xc0) << 10 */
	ADD_L	(ECX,EAX)
	AND_B	(CONST(0x3c),BL)
	SHL_L	(CONST(26),EBX)		/* (byte10 & 0x3c) << 26 */
	ADD_L	(EBX,EAX)
	MOV_L	(REGOFF(40,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	/* get four bits */
	MOV_B	(CL,BL)
	AND_B	(CONST(0xf0),CL)
	SHL_L	(CONST(20),ECX)		/* (byte11 & 0xf0) << 20 */
	ADD_L	(ECX,EAX)
	MOV_L	(vaddr_arg,ECX)
	MOV_L	(EAX,REGOFF(8,ECX))	/* Write dword. */

	/* get two bits */
	AND_B	(CONST(0x0c),BL)
	SHL_L	(CONST(4),EBX)		/* (byte11 & 0x0c) << 4 */
	MOV_L	(EBX,EAX)
	MOV_L	(REGOFF(44,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	SHR_L	(CONST(2),ECX)		/* byte12 >> 2 */
	ADD_L	(ECX,EAX)
	MOV_L	(REGOFF(48,EDI),EBX)	/* Next char */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	SHL_L	(CONST(8),ECX)		/* byte13 << 8 */
	ADD_L	(ECX,EAX)
	MOV_L	(REGOFF(52,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	/* get two bits */
	MOV_B	(CL,BL)
	AND_B	(CONST(0xc0),CL)
	SHL_L	(CONST(2),ECX)		/* (byte14 & 0xc0) << 2 */
	ADD_L	(ECX,EAX)
	/* get four bits */
	AND_B	(CONST(0x3c),BL)
	SHL_L	(CONST(18),EBX)
	ADD_L	(EBX,EAX)		/* (byte14 & 0x3c) << 18 */
	MOV_L	(REGOFF(56,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	/* get four bits */
	MOV_B	(CL,BL)
	AND_B	(CONST(0xf0),CL)
	SHL_L	(CONST(12),ECX)		/* (byte15 & 0xf0) << 12 */
	ADD_L	(ECX,EAX)
	/* get two bits */
	AND_B	(CONST(0x0c),BL)
	SHL_L	(CONST(28),EBX)		/* (byte15 & 0x0c) << 28 */
	ADD_L	(EBX,EAX)
	MOV_L	(REGOFF(60,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	ADD_L	(CONST(64),EDI)		/* (blend) glyphp += 16 */
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	SHL_L	(CONST(22),ECX)		/* byte16 << 22 */
	ADD_L	(ECX,EAX)
	MOV_L	(vaddr_arg,EBX)
	CMP_L	(CONST(16),ESI)		/* blend */
	MOV_L	(EAX,REGOFF(12,EBX))	/* Write dword. */

	JGE	(.width6loop)

	XOR_L	(EAX,EAX)		/* dworddata = 0 */
	MOV_W	(CONST(0x0600),CX)	/* shift = 0, glyphwidth = 6 */
	CMP_L	(CONST(6),ESI)
	JL	(.width6end)

	/* Process another 6 chars for good measure. */
	MOV_L	(REGIND(EDI),EBX)	/* glyphp[chari] */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX) /* data = glyphp[chari][line] */
	MOV_L	(ECX,EAX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_L	(REGOFF(4,EDI),EBX)	/* Next char */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	MOV_B	(CL,BL)
	SHR_L	(CONST(6),ECX)		/* byte2 >> 6 */
	ADD_L	(ECX,EAX)
	AND_B	(CONST(0x3c),BL)
	SHL_L	(CONST(10),EBX)		/* (byte2 & 0x3c) << 10 */
	ADD_L	(EBX,EAX)
	MOV_L	(REGOFF(8,EDI),EBX)	/* Next char */
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	MOV_B	(CL,BL)
	AND_B	(CONST(0xf0),CL)
	SHL_L	(CONST(4),ECX)		/* (byte3 & 0xf0) << 4 */
	ADD_L	(ECX,EAX)
	AND_B	(CONST(0x0c),BL)
	SHL_L	(CONST(20),EBX)		/* (byte3 & 0x0c) << 20 */
	ADD_L	(EBX,EAX)
	MOV_L	(REGOFF(12,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	SHL_L	(CONST(14),ECX)		/* byte4 << 14 */
	ADD_L	(ECX,EAX)
	MOV_L	(REGOFF(16,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),CL)
	SHL_L	(CONST(24),ECX)		/* byte5 << 24 */
	ADD_L	(ECX,EAX)
	MOV_L	(REGOFF(20,EDI),EBX)	/* Next char */
	XOR_L	(ECX,ECX)
	MOV_L	(REGBISD(EBX,EDX,4,0),EBX)
	MOV_L	(EBX,ECX)		/* save lsb byte6 bits */
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),BL)
	AND_B	(CONST(0xc0),BL)
	SHL_L	(CONST(18),EBX)		/* (byte6 & 0xc0) << 18 */
	ADD_L	(EBX,EAX)
	MOV_L	(vaddr_arg,EBX)
	MOV_L	(EAX,REGOFF(16,EBX))	/* Write dword. */

	MOV_L	(ECX,EAX)
	SHR_L	(CONST(2),EAX)
	ADD_L	(CONST(24),EDI)
	SUB_L	(CONST(6),ESI)
	MOV_W	(CONST(0x0604),CX)	/* shift = 4, glyphwidth = 6 */

.width6end:
	/* Ugly loop semantics. */
	TEST_L	(ESI,ESI)
	JZ	(.line_finished5)
	MOV_L	(ESI,glyphsleft_var)
	JMP	(.continue_loop5)

.width14code:	/* Speedup ~25% */
	/* Width 14; process 5 chars, no loop. */
	MOV_L	(REGIND(EDI),EBX)	/* glyphp[chari] */
	MOV_L	(REGBISD(EBX,EDX,4,0),EAX) /* data = glyphp[chari][line] */
	MOV_L	(REGOFF(4,EDI),EBX)	/* Next char */
	MOV_L	(REGBISD(EBX,EDX,4,0),ECX)
	SHL_L	(CONST(14),ECX)
	MOV_L	(REGOFF(8,EDI),EBX)	/* Next char */
	ADD_L	(ECX,EAX)
	MOV_L	(REGBISD(EBX,EDX,4,0),ECX)
	MOV_L	(ECX,EBX)
	SHL_L	(CONST(28),EBX)
	ADD_L	(EBX,EAX)
	/* write dword */
	XOR_L	(EBX,EBX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	ROL_L	(CONST(16),EAX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	MOV_L	(vaddr_arg,EBX)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGOFF(4,EBX))	/* Write dword. */

	SHR_L	(CONST(4),ECX)
	MOV_L	(ECX,EAX)
	MOV_L	(REGOFF(12,EDI),EBX)	/* Next char */
	MOV_L	(REGBISD(EBX,EDX,4,0),ECX)
	SHL_L	(CONST(10),ECX)
	ADD_L	(ECX,EAX)
	MOV_L	(REGOFF(16,EDI),EBX)	/* Next char */
	MOV_L	(REGBISD(EBX,EDX,4,0),ECX)
	MOV_L	(ECX,EBX)
	SHL_L	(CONST(24),EBX)		/* (was 22) */
	ADD_L	(EBX,EAX)
	/* write dword */
	XOR_L	(EBX,EBX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	ROL_L	(CONST(16),EAX)
	MOV_B	(AL,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AL)
	MOV_B	(AH,BL)
	MOV_B	(REGOFF(BYTE_REVERSED,EBX),AH)
	MOV_L	(vaddr_arg,EBX)
	ROL_L	(CONST(16),EAX)
	MOV_L	(EAX,REGOFF(8,EBX))	/* Write dword. */

	SHR_L	(CONST(8),ECX)
	MOV_L	(ECX,EAX)

	ADD_L	(CONST(20),EDI)		/* glyphp += 8 */
	MOV_L	(glyphsleft_var,ESI)
	SUB_L	(CONST(5),ESI)

	MOV_W	(CONST(0x0e06),CX)	/* shift = 6, glyphwidth = 14 */
	/* Ugly loop semantics. */
	TEST_L	(ESI,ESI)
	JZ	(.line_finished5)
	MOV_L	(ESI,glyphsleft_var)
	JMP	(.continue_loop5)

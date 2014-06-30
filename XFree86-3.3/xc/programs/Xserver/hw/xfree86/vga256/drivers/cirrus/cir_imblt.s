/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_imblt.s,v 3.9 1996/12/23 06:56:49 dawes Exp $ */
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
/* $XConsortium: cir_imblt.s /main/7 1996/10/23 18:48:21 kaleb $ */


/*
 * This low-level routine copies bitmap data to video memory for the
 * blitter, which must be setup for system-memory-to-video-memory BLT.
 * The video address where the data is written doesn't matter. Each bitmap
 * scanline transmitted is padded to a multiple of 4 bytes; the bitmap is
 * transfered in dwords (this is effectively 16-bit words because of the
 * 16-bit host interface of the 5426/28).
 *
 * This function is used by the 5426 and 5428.
 *
 * Prototype:
 * CirrusImageWriteTransfer( int width, int height, void *srcaddr,
 *     int srcwidth, void *vaddr )
 *
 * width is the bitmap width in bytes.
 * height is the height of the bitmap.
 * srcaddr is the address of the bitmap data.
 * srcwidth is the length of a bitmap scanline in bytes.
 * vaddr is a video memory address (doesn't really matter).
 * 
 * REP MOVS could be used here, but it is not a very optimal instruction
 * for this type of stuff (high setup cost).
 *
 * With a fast CPU, this function doesn't seem to be very much faster than
 * the C loop in the 2.0 server (~30% on a VLB 486 at 40 MHz).
 * For a slower CPU, it helps a lot.
 *
 * September 1996: Changed to generate PCI burst accesses. The destination
 * address is increased by at most 32 bytes each scanline, which will keep
 * it inside the 64K VGA addressing window.
 */

#include "assyntax.h"

 	FILE("cir_imageblt.S")

	AS_BEGIN

/* Definition of stack frame function arguments. */

#define width_arg	REGOFF(8,EBP)
#define height_arg	REGOFF(12,EBP)
#define srcaddr_arg	REGOFF(16,EBP)
#define srcwidth_arg	REGOFF(20,EBP)
#define vaddr_arg	REGOFF(24,EBP)

#define nu_dwords_var	REGOFF(-4,EBP)

/* I assume %eax and %edx can be trashed. */
/* Saving %ebx and %ecx may be unnecessary. */

	SEG_TEXT

	ALIGNTEXT4

	GLOBL GLNAME(CirrusImageWriteTransfer)
GLNAME(CirrusImageWriteTransfer):

	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	SUB_L 	(CONST(0x04),ESP)	/* Allocate one local variable. */
	PUSH_L	(EBX)
	PUSH_L	(ECX)
	PUSH_L	(ESI)
	PUSH_L	(EDI)

	MOV_L	(width_arg,EAX)
	MOV_L   (EAX,EBX)
	SHR_L	(CONST(0x02),EAX)	/* #dwords. */
	MOV_L	(EAX,nu_dwords_var)	/* Store in local variable. */
	AND_B   (CONST(0x03),BL)	/* Remainder. */

	MOV_L   (srcaddr_arg,ESI)	/* Source address. */
	MOV_L	(vaddr_arg,EDI)		/* Video address for blit. */

	MOV_L	(height_arg,EDX)
	TEST_L	(EDX,EDX)
	JMP	(.loop_entry)

.line_loop:
	MOV_L	(nu_dwords_var,ECX)	/* ECX = #dwords. */

.unrolled_word_loop:
	CMP_L	(CONST(0x08),ECX)	/* Do we have 8 dwords left? */
	JL	(.word_loop_check)	/* If not, jump over unrolled loop. */

	MOV_L	(REGIND(ESI),EAX)	/* Unrolled loop. */
	MOV_L	(EAX,REGIND(EDI))	/* Transfer 8 dwords. */
	MOV_L	(REGOFF(4,ESI),EAX)
	MOV_L	(EAX,REGOFF(4,EDI))
	MOV_L	(REGOFF(8,ESI),EAX)
	MOV_L	(EAX,REGOFF(8,EDI))
	MOV_L	(REGOFF(12,ESI),EAX)
	MOV_L	(EAX,REGOFF(12,EDI))
	MOV_L	(REGOFF(16,ESI),EAX)
	MOV_L	(EAX,REGOFF(16,EDI))
	MOV_L	(REGOFF(20,ESI),EAX)
	MOV_L	(EAX,REGOFF(20,EDI))
	MOV_L	(REGOFF(24,ESI),EAX)
	MOV_L	(EAX,REGOFF(24,EDI))
	MOV_L	(REGOFF(28,ESI),EAX)
	MOV_L	(EAX,REGOFF(28,EDI))

	ADD_L	(CONST(0x20),ESI)
	SUB_L	(CONST(0x08),ECX)
	JMP	(.unrolled_word_loop)

.word_loop_check:
	CMP_L	(CONST(0),ECX)
	JZ	(.do_remainder)		/* No dwords left. */

.word_loop:
	MOV_L	(REGIND(ESI),EAX)	/* Transfer a dword. */
	MOV_L	(EAX,REGOFF(32,EDI))

	/* Check-and-unroll could be used here. */

	ADD_L	(CONST(0x04),EDI)
	ADD_L	(CONST(0x04),ESI)
	DEC_L	(ECX)
	JNZ	(.word_loop)

.do_remainder:
	CMP_B	(CONST(0),BL)
	JE 	(.line_finished)	/* No bytes left, line finished. */
	CMP_B	(CONST(1),BL)
	JE	(.one_byte_remaining)
	CMP_B	(CONST(2),BL)
	JE	(.two_bytes_remaining)

	/* Three bytes remaining. */
	MOV_B	(REGOFF(2,ESI),AL)
	SHL_L	(CONST(16),EAX)
	MOV_W	(REGIND(ESI),AX)
	ADD_L	(CONST(0x03),ESI)
	MOV_L	(EAX,REGOFF(32,EDI))		/* Write dword. */
	JMP	(.line_finished)

.one_byte_remaining:
	MOV_B	(REGIND(ESI),AL)
	INC_L	(ESI)
	MOV_L	(EAX,REGOFF(32,EDI))	/* Write dword with remainder. */
	JMP	(.line_finished)

.two_bytes_remaining:
	MOV_W	(REGIND(ESI),AX)
	ADD_L	(CONST(0x02),ESI)
	MOV_L	(EAX,REGOFF(32,EDI))	/* Write dword with remainder. */

.line_finished:
	ADD_L	(srcwidth_arg,ESI)	/* Adjust source pointer for */
	SUB_L	(width_arg,ESI)		/* bitmap pitch. */
	DEC_L	(EDX)
.loop_entry:
	JNZ 	(.line_loop)

	POP_L	(EDI)
	POP_L	(ESI)
	POP_L	(ECX)
	POP_L	(EBX)
	ADD_L 	(CONST(0x04),ESP)	/* De-allocate local variable. */
	POP_L	(EBP)
	RET

/* 
 * This may be wrong. For image reads, scanlines are not padded two a multiple
 * of 4 bytes (if I understand the databook correctly).
 */

/* 
 * This is the equivalent function for image read blits.
 *
 * Prototype:
 * CirrusImageReadTransfer( int width, int height, void *destaddr,
 *     int destwidth, void *vaddr )
 *
 * width is the bitmap width in bytes.
 * height is the height of the bitmap.
 * destaddr is the address of the bitmap data.
 * destwidth is the length of a bitmap scanline in bytes.
 * vaddr is a video memory address (doesn't really matter).
 * 
 * Digit '2' appended to labels to avoid clashes.
 */

/* These stack frame arguments correspond with srcaddr and srcwidth. */
/* The other arguments are in the same place on the stack. */

#define destaddr_arg	REGOFF(16,EBP)
#define destwidth_arg	REGOFF(20,EBP)


	GLOBL GLNAME(CirrusImageReadTransfer)
GLNAME(CirrusImageReadTransfer):

	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	SUB_L 	(CONST(0x04),ESP)	/* Allocate one local variable. */
	PUSH_L	(EBX)
	PUSH_L	(ECX)
	PUSH_L	(ESI)
	PUSH_L	(EDI)

	MOV_L	(width_arg,EAX)
	MOV_L   (EAX,EBX)
	SHR_L	(CONST(0x02),EAX)	/* #dwords. */
	MOV_L	(EAX,nu_dwords_var)	/* Store in local variable. */
	AND_B   (CONST(0x03),BL)	/* Remainder. */

	MOV_L   (destaddr_arg,ESI)	/* Destination address. */
	MOV_L	(vaddr_arg,EDI)		/* Video address for blit. */

	MOV_L	(height_arg,EDX)
	TEST_L	(EDX,EDX)
	JMP	(.loop_entry2)

.line_loop2:
	MOV_L	(nu_dwords_var,ECX)	/* ECX = #dwords. */

.unrolled_word_loop2:
	CMP_L	(CONST(0x08),ECX)	/* Do we have 8 dwords left? */
	JL	(.word_loop_check2)	/* If not, jump over unrolled loop. */

	MOV_L	(REGIND(EDI),EAX)	/* Unrolled loop. */
	MOV_L	(EAX,REGIND(ESI))	/* Transfer 8 dwords. */
	MOV_L	(REGIND(EDI),EAX)
	MOV_L	(EAX,REGOFF(4,ESI))
	MOV_L	(REGIND(EDI),EAX)
	MOV_L	(EAX,REGOFF(8,ESI))
	MOV_L	(REGIND(EDI),EAX)
	MOV_L	(EAX,REGOFF(12,ESI))
	MOV_L	(REGIND(EDI),EAX)
	MOV_L	(EAX,REGOFF(16,ESI))
	MOV_L	(REGIND(EDI),EAX)
	MOV_L	(EAX,REGOFF(20,ESI))
	MOV_L	(REGIND(EDI),EAX)
	MOV_L	(EAX,REGOFF(24,ESI))
	MOV_L	(REGIND(EDI),EAX)
	MOV_L	(EAX,REGOFF(28,ESI))

	ADD_L	(CONST(0x20),ESI)
	SUB_L	(CONST(0x08),ECX)
	JMP	(.unrolled_word_loop2)

.word_loop_check2:
	CMP_L	(CONST(0),ECX)
	JZ	(.do_remainder2)	/* No dwords left. */

.word_loop2:
	MOV_L	(REGIND(EDI),EAX)	/* Transfer a dword. */
	MOV_L	(EAX,REGIND(ESI))

	/* Check-and-unroll could be used here. */

	ADD_L	(CONST(0x04),ESI)
	DEC_L	(ECX)
	JNZ	(.word_loop2)

.do_remainder2:
	CMP_B	(CONST(0),BL)
	JE 	(.line_finished2)	/* No bytes left, line finished. */
	CMP_B	(CONST(1),BL)
	JE	(.one_byte_remaining2)
	CMP_B	(CONST(2),BL)
	JE	(.two_bytes_remaining2)

	/* Three bytes remaining. */
	MOV_L	(REGIND(EDI),EAX)
	MOV_W	(AX,REGIND(ESI))
	SHR_L	(CONST(16),EAX)
	MOV_B	(AL,REGOFF(2,ESI))
	ADD_L	(CONST(0x03),ESI)
	JMP	(.line_finished2)

.one_byte_remaining2:
	MOV_L	(REGIND(EDI),EAX)
	MOV_B	(AL,REGIND(ESI))
	INC_L	(ESI)
	JMP	(.line_finished2)

.two_bytes_remaining2:
	MOV_L	(REGIND(EDI),EAX)
	MOV_W	(AX,REGIND(ESI))
	ADD_L	(CONST(0x02),ESI)

.line_finished2:
	ADD_L	(destwidth_arg,ESI)	/* Adjust source pointer for */
	SUB_L	(width_arg,ESI)		/* bitmap pitch. */
	DEC_L	(EDX)
.loop_entry2:
	JNZ 	(.line_loop2)

	POP_L	(EDI)
	POP_L	(ESI)
	POP_L	(ECX)
	POP_L	(EBX)
	ADD_L 	(CONST(0x04),ESP)	/* De-allocate local variable. */
	POP_L	(EBP)
	RET


/* 
 * Function to transfer monochrome bitmap data to the blitter.
 * Has to reverse the per-byte bit order.
 * Assumes source data can be transfered as-is, i.e. scanlines are in a
 * format suitable for the BitBLT engine (not padded).
 *
 * Prototype:
 * CirrusAlignedBitmapTransfer( int count, void *srcaddr, void *vaddr )
 *
 * count is the number of 32-bit words to transfer.
 * srcaddr is the address of the bitmap data.
 * vaddr is a video memory address (doesn't really matter).
 *
 * Optimized for 486 pipeline.
 *
 * September 1996: Changed to generate PCI burst accesses. Destination
 * address is increased by at most 16 bytes per scanline.
 */

#define count3_arg	REGOFF(8,EBP)
#define srcaddr3_arg	REGOFF(12,EBP)
#define vaddr3_arg	REGOFF(16,EBP)

	GLOBL GLNAME(CirrusAlignedBitmapTransfer)
GLNAME(CirrusAlignedBitmapTransfer):

	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	PUSH_L	(EBX)
	PUSH_L	(ECX)
	PUSH_L	(ESI)
	PUSH_L	(EDI)

	MOV_L	(count3_arg,ECX)
	MOV_L	(srcaddr3_arg,ESI)
	MOV_L	(vaddr3_arg,EDI)
	XOR_L	(EDX,EDX)

.unrolled_loop3:
	CMP_L	(CONST(4),ECX)
	JL	(.word_loop_check3)

	/* Handle four dwords. */
	MOV_B	(REGOFF(2,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(3,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_B	(REGIND(ESI),DL)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(1,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)

	MOV_B	(REGOFF(6,ESI),DL)
	MOV_L	(EAX,REGIND(EDI))		/* Delayed. */
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(7,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_B	(REGOFF(4,ESI),DL)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(5,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)

	MOV_B	(REGOFF(10,ESI),DL)
	MOV_L	(EAX,REGOFF(4,EDI))		/* Delayed. */
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(11,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_B	(REGOFF(8,ESI),DL)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(9,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)

	MOV_B	(REGOFF(14,ESI),DL)
	MOV_L	(EAX,REGOFF(8,EDI))		/* Delayed. */
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(15,ESI),DL)
	SUB_L	(CONST(4),ECX)			/* Blended in from loop end. */
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_B	(REGOFF(12,ESI),DL)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(13,ESI),DL)
	ADD_L	(CONST(16),ESI)			/* Blended in from loop end. */
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_L	(EAX,REGOFF(12,EDI))

	JMP	(.unrolled_loop3)

.word_loop_check3:
	TEST_L	(ECX,ECX)
	JZ	(.end3)

.word_loop3:
	MOV_B	(REGOFF(2,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(3,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_B	(REGIND(ESI),DL)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(1,ESI),DL)
	DEC_L	(ECX)			/* Blended in from loop end. */
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_L	(EAX,REGOFF(16,EDI))
	ADD_L	(CONST(4),EDI)

	JNZ	(.word_loop3)

.end3:
	POP_L	(EDI)
	POP_L	(ESI)
	POP_L	(ECX)
	POP_L	(EBX)
	POP_L	(EBP)
	RET



/* 
 * Function to transfer monochrome bitmap data to the blitter.
 * Has to reverse the per-byte bit order. 
 * Because the BitBLT engine wants only dword transfers and cannot accept
 * scanline padding, bytes from different scanlines have to be combined.
 *
 * Prototype:
 * CirrusBitmapTransfer( int bytewidth, int h, int bwidth, void *srcaddr,
 * 	void *vaddr )
 *
 * bytewidth is the number of bytes to be transferred per scanline.
 * h is the number of bitmap scanlines to be transfered.
 * bwidth is the total (padded) width of a bitmap scanline.
 * srcaddr is the address of the bitmap data.
 * vaddr is a video memory address (doesn't really matter).
 *
 * Optimized for 486 pipeline.
 *
 * September 1996: Changed to generate PCI burst accesses. Destination
 * address is increased by at most 16 bytes per scanline.
 */

#define bytewidth4_arg	REGOFF(8,EBP)
#define height4_arg	REGOFF(12,EBP)
#define bwidth4_arg	REGOFF(16,EBP)	/* Overwritten with #padding bytes. */
#define srcaddr4_arg	REGOFF(20,EBP)
#define vaddr4_arg	REGOFF(24,EBP)

#define scanline_pad_bytes4 bwidth4_arg

	GLOBL GLNAME(CirrusBitmapTransfer)
GLNAME(CirrusBitmapTransfer):

	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	PUSH_L	(EBX)
	PUSH_L	(ECX)
	PUSH_L	(ESI)
	PUSH_L	(EDI)

	MOV_L	(srcaddr4_arg,ESI)
	MOV_L	(vaddr4_arg,EDI)
	XOR_L	(EDX,EDX)
	MOV_L	(height4_arg,EBX)	/* Scanline counter. */

	MOV_L	(bytewidth4_arg,ECX)

	/* Calculate bitmap scanline padding, overwrite bwidth. */
	SUB_L	(ECX,bwidth4_arg)

	CMP_L   (CONST(0),EBX)		/* Sanity check. */
	JLE	(.end4)

.scanline_loop4:

.unrolled_loop4:
	CMP_L	(CONST(16),ECX)
	JL	(.word_loop_check4)

	/* Handle four dwords. */
	MOV_B	(REGOFF(2,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(3,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_B	(REGIND(ESI),DL)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(1,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)

	MOV_B	(REGOFF(6,ESI),DL)
	MOV_L	(EAX,REGIND(EDI))		/* Delayed. */
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(7,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_B	(REGOFF(4,ESI),DL)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(5,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)

	MOV_B	(REGOFF(10,ESI),DL)
	MOV_L	(EAX,REGOFF(4,EDI))		/* Delayed. */
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(11,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_B	(REGOFF(8,ESI),DL)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(9,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)

	MOV_B	(REGOFF(14,ESI),DL)
	MOV_L	(EAX,REGOFF(8,EDI))		/* Delayed. */
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(15,ESI),DL)
	SUB_L	(CONST(16),ECX)			/* Blended in from loop end. */
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_B	(REGOFF(12,ESI),DL)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(13,ESI),DL)
	ADD_L	(CONST(16),ESI)			/* Blended in from loop end. */
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_L	(EAX,REGOFF(12,EDI))

	JMP	(.unrolled_loop4)

.word_loop_check4:
	TEST_B	(CL,CL)
	JZ	(.zero_bytes_remaining4)
	CMP_B	(CONST(4),CL)
	JL	(.handle_remainder4)

.word_loop4:
	MOV_B	(REGOFF(2,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(3,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_B	(REGIND(ESI),DL)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(1,ESI),DL)
	ADD_L	(CONST(4),ESI)		/* Blended in. */
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_L	(EAX,REGOFF(16,EDI))
	ADD_L	(CONST(4),EDI)

	SUB_B	(CONST(4),CL)
	CMP_B	(CONST(4),CL)
	JGE	(.word_loop4)

.handle_remainder4:
	TEST_B	(CL,CL)
	JZ	(.zero_bytes_remaining4)
	CMP_B	(CONST(2),CL)
	JE	(.two_bytes_remaining4)
	CMP_B	(CONST(1),CL)
	JE	(.one_byte_remaining4)

	/* Three bytes remaining. */
	MOV_B	(REGIND(ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(1,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(2,ESI),DL)
	ADD_L	(CONST(3),ESI)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)

	CMP_L	(CONST(1),EBX)
	JE	(.three_remaining_lastscanline4)

	ADD_L	(scanline_pad_bytes4,ESI)

	MOV_B	(REGIND(ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	ROL_L	(CONST(16),EAX)
.three_remaining_lastscanline4:
	MOV_L	(EAX,REGOFF(16,EDI))

	MOV_L	(bytewidth4_arg,ECX)
	DEC_L	(ECX)

	INC_L	(ESI)
	DEC_L	(EBX)
	JNZ	(.scanline_loop4)
	JMP	(.end4)

.zero_bytes_remaining4:
	ADD_L	(scanline_pad_bytes4,ESI)

	MOV_L	(bytewidth4_arg,ECX)

	DEC_L	(EBX)
	JNZ	(.scanline_loop4)
	JMP	(.end4)

.one_byte_remaining4:
	MOV_B	(REGIND(ESI),DL)
	INC_L	(ESI)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)

	CMP_L	(CONST(1),EBX)
	JE	(.one_remaining_lastscanline4)

	ADD_L	(scanline_pad_bytes4,ESI)

	MOV_B	(REGIND(ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	MOV_B	(REGOFF(1,ESI),DL)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(2,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	ROL_L	(CONST(16),EAX)
.one_remaining_lastscanline4:
	MOV_L	(EAX,REGOFF(16,EDI))

	MOV_L	(bytewidth4_arg,ECX)
	SUB_L	(CONST(3),ECX)

	ADD_L	(CONST(3),ESI)
	DEC_L	(EBX)
	JNZ	(.scanline_loop4)
	JMP	(.end4)

.two_bytes_remaining4:
	MOV_B	(REGIND(ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(1,ESI),DL)
	ADD_L	(CONST(2),ESI)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)

	CMP_L	(CONST(1),EBX)
	JE	(.two_remaining_lastscanline4)

	ADD_L	(scanline_pad_bytes4,ESI)

	MOV_B	(REGIND(ESI),DL)
	SHL_L	(CONST(16),EAX)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AL)
	MOV_B	(REGOFF(1,ESI),DL)
	MOV_B	(REGOFF(GLNAME(byte_reversed),EDX),AH)
	ROL_L	(CONST(16),EAX)
.two_remaining_lastscanline4:
	MOV_L	(EAX,REGOFF(16,EDI))

	MOV_L	(bytewidth4_arg,ECX)
	SUB_L	(CONST(2),ECX)

	ADD_L	(CONST(2),ESI)
	DEC_L	(EBX)
	JNZ	(.scanline_loop4)

.end4:
	POP_L	(EDI)
	POP_L	(ESI)
	POP_L	(ECX)
	POP_L	(EBX)
	POP_L	(EBP)
	RET

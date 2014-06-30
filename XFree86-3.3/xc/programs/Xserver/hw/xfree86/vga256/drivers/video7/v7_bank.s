/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/video7/v7_bank.s,v 3.3 1996/12/23 06:58:54 dawes Exp $ */
/*
 * Stubs Driver Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/* $XConsortium: v7_bank.s /main/3 1996/02/21 18:08:26 kaleb $ */

/*
 * These are here the very lowlevel VGA bankswitching routines.
 * The segment to switch to is passed via %eax. Only %eax and %edx my be used
 * without saving the original contents.
 *
 * WHY ASSEMBLY LANGUAGE ???
 *
 * These routines must be callable by other assembly routines. But I don't
 * want to have the overhead of pushing and poping the normal stack-frame.
 */

/*
 * All XFree86 assembler-language files are written in a macroized assembler
 * format.  This is done to allow a single .s file to be assembled under
 * the USL-syntax assemblers with SVR3/4, the GAS-syntax used by 386BSD,
 * Linux, and Mach, and the Intel-syntax used by Amoeba and Minix.
 *
 * All of the 386 instructions, addressing modes, and appropriate assembler
 * pseudo-ops are definined in the "assyntax.h" header file.
 */
#include "assyntax.h"

/*
 * Three entry points must be defined in this file:
 *
 *	VIDEO7SetRead      - Set the read-bank pointer
 *	VIDEO7SetWrite     - Set the write-bank pointer
 *	VIDEO7SetReadWrite - Set both bank pointers to the same bank
 *
 * Most SVGA chipsets have two bank pointers - a read bank and a write bank.
 * In general, the server sets the read and write banks to the same bank,
 * via the SetReadWrite function.  For BitBlt operations, it is much more
 * efficient to use two banks independently.
 *
 * If the new chipset only has one bank pointer, then all three entry
 * points must still be defined, but they will be all the same function.
 * In this case, make sure the ChipUse2Banks field of the VIDEO7ChipRec is
 * set to FALSE in the video7_driver.c file.
 */

	FILE("video7_bank.s")	/* Define the file name for the .o file */

	AS_BEGIN		/* This macro does all generic setup */

/*
 * Some chipsets maintain both bank pointers in a single register.  To
 * avoid having to do a read/modify/write cycle on the register, it is
 * best to maintain a copy of the register in memory, modify the 
 * appropriate part of it, and then do a single 'OUT_B' to set it.
 */
	SEG_DATA		/* Switch to the data segment */
Segment:			/* Storage for efficiency */
	D_BYTE 0

	SEG_TEXT		/* Switch to the text segment */

/* 
 * The SetReadWrite function sets both bank pointers.  The bank will be
 * passed in AL.  As an example, this video7 assumes that the read bank 
 * register is register 'base', index 'idx_r', and the write bank register
 * is index 'idx_w'.
 */
	ALIGNTEXT4		
	GLOBL	GLNAME(VIDEO7SetReadWrite)
	GLOBL	GLNAME(VIDEO7SetWrite)
	GLOBL	GLNAME(VIDEO7SetRead)
GLNAME(VIDEO7SetReadWrite):
GLNAME(VIDEO7SetWrite):
GLNAME(VIDEO7SetRead):
	AND_W	(CONST(15),AX)
	MOV_B	(AL,CONTENT(Segment))
	MOV_W	(CONST(0x03C4),DX)
	MOV_B	(CONTENT(Segment),AH)
	AND_B	(CONST(1),AH)
	MOV_B	(CONST(0xF9),AL)
	OUT_W
	MOV_B	(CONTENT(Segment),AL)
	AND_B	(CONST(12),AL)
	MOV_B	(AL,AH)
	SHR_B	(CONST(1),AH)
	SHR_B	(CONST(1),AH)
	OR_B	(AL,AH)
	MOV_B	(CONST(0xF6),AL)
	OUT_B
	INC_W	(DX)
	IN_B
	DEC_W	(DX)
	AND_B	(CONST(0xF0),AL)
	OR_B	(AL,AH)
	MOV_B	(CONST(0xF6),AL)
	OUT_W
	MOV_B	(CONTENT(Segment),AH)
	SHL_B	(CONST(4),AH)
	AND_B	(CONST(0x20),AH)
	MOV_B	(CONST(0xCC),DL)
	IN_B
	MOV_B	(CONST(0xC2),DL)
	AND_B	(CONST(0xDF),AL)
	OR_B	(AH,AL)
	OUT_B
	RET

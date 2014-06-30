/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/et4000/bank.s,v 3.6 1996/12/23 06:57:25 dawes Exp $ */
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
 *
 */
/* $XConsortium: bank.s /main/6 1996/02/21 18:05:10 kaleb $ */

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
 * first we have here a mirror for the segment register. That's because a
 * I/O read costs so much more time, that is better to keep the value of it
 * in memory.
 */

#include "assyntax.h"

 	FILE("et4000bank.s")

	AS_BEGIN

	SEG_DATA
Segment:		/* shadow of 0x3CD */
	D_BYTE 0
Segment2:		/* shadow of 0x3CB, W32 only */
	D_BYTE 0

	SEG_TEXT

	ALIGNTEXT4
	GLOBL	GLNAME(ET4000SetRead)
GLNAME(ET4000SetRead):
	MOV_B	(CONTENT(Segment),AH)
	AND_B	(CONST(0x0f),AH)
	SHL_B	(CONST(4),AL)
	OR_B	(AH,AL)
	MOV_B	(AL,CONTENT(Segment))
	MOV_L	(CONST(0x3CD),EDX)
	OUT_B
	RET

        ALIGNTEXT4
	GLOBL	GLNAME(ET4000SetWrite)
GLNAME(ET4000SetWrite):
	MOV_B	(CONTENT(Segment),AH)
	AND_B	(CONST(0xf0),AH)
	OR_B	(AH,AL)
	MOV_B	(AL,CONTENT(Segment))
	MOV_L	(CONST(0x3CD),EDX)
	OUT_B
	RET
	
	ALIGNTEXT4
	GLOBL	GLNAME(ET4000SetReadWrite)
GLNAME(ET4000SetReadWrite):
	MOV_B	(AL,AH)
	SHL_B	(CONST(4),AH)
	OR_B	(AH,AL)
	MOV_B	(AL,CONTENT(Segment))
	MOV_L	(CONST(0x3CD),EDX)
        OUT_B
        RET

/*
 * ET4000/W32 specific functions that also set bits 4-5 of the 64K page
 * number, addressing 4MB.
 */

	ALIGNTEXT4
	GLOBL	GLNAME(ET4000W32SetRead)
GLNAME(ET4000W32SetRead):
	PUSH_L	(EAX)
	MOV_B	(CONTENT(Segment),AH)
	AND_B	(CONST(0x0f),AH)	/* clear all but 3:0 in shadow */
	SHL_B	(CONST(4),AL)		/* get 3:0 of read segment into 7:4 */
	OR_B	(AH,AL)			/* merge read and write */
	MOV_B	(AL,CONTENT(Segment))
	MOV_L	(CONST(0x3CD),EDX)
	OUT_B				/* 0:3 of AL -> 7:4 at 0x3CD */
	POP_L	(EAX)
	AND_B	(CONST(0x30),AL)	/* Mask out all but 5:4 */
	MOV_B	(CONTENT(Segment2),AH)
	AND_B	(CONST(0xCF),AH)	/* Mask out 5:4 */
	OR_B	(AH,AL)
	MOV_B	(AL,CONTENT(Segment2))
	MOV_L	(CONST(0x3CB),EDX)
	OUT_B				/* 5:4 of AL -> 5:4 at 0x3CB */
	RET

        ALIGNTEXT4
	GLOBL	GLNAME(ET4000W32SetWrite)
GLNAME(ET4000W32SetWrite):
	PUSH_L	(EAX)
	AND_B	(CONST(0x0f),AL)
	MOV_B	(CONTENT(Segment),AH)
	AND_B	(CONST(0xf0),AH)
	OR_B	(AH,AL)
	MOV_B	(AL,CONTENT(Segment))
	MOV_L	(CONST(0x3CD),EDX)
	OUT_B				/* 3:0 of AL -> 3:0 at 0x3CD */
	POP_L	(EAX)
	MOV_B	(CONTENT(Segment2),AH)
	AND_B	(CONST(0xFC),AH)	/* Mask out 1:0 */
	AND_B	(CONST(0x30),AL)	/* Mask out all but 5:4 */
	SHR_B	(CONST(4),AL)
	OR_B	(AH,AL)
	MOV_B	(AL,CONTENT(Segment2))
	MOV_L	(CONST(0x3CB),EDX)
	OUT_B
	RET
	
	ALIGNTEXT4
	GLOBL	GLNAME(ET4000W32SetReadWrite)
GLNAME(ET4000W32SetReadWrite):
	PUSH_L	(EAX)
	AND_B	(CONST(0xf),AL)
	MOV_B	(AL,AH)
	SHL_B	(CONST(4),AH)
	OR_B	(AH,AL)
	MOV_B	(AL,CONTENT(Segment))
	MOV_L	(CONST(0x3CD),EDX)
        OUT_B
	POP_L	(EAX)
	AND_B	(CONST(0x30),AL)
	MOV_B	(AL,AH)
	SHR_B	(CONST(4),AH)
	OR_B	(AH,AL)
	MOV_B	(AL,CONTENT(Segment2))
	MOV_L	(CONST(0x3CB),EDX)
	OUT_B
        RET


/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/pvga1/bank.s,v 3.6 1996/12/23 06:58:03 dawes Exp $ */
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
/* $XConsortium: bank.s /main/5 1996/02/21 18:06:29 kaleb $ */

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
 * what happens really here ?
 *
 * PRA and PRB are segmentpointers to out two segments. They have a granularity
 * of 4096. That means we have to multiply the segmentnumber by 8, if we are
 * working with 32k segments. But since PRA and PRB are 'indexed' registers,
 * the index must be emitted first. This is accomplished by loading %al with
 * the index and %ah with the value. Therefor we must shift the logical
 * segmentnumber by 11.
 *
 * Another quirk is PRA. It's physical VGA mapping starts at 0xA0000, but it is
 * only visible starting form 0xA8000 to 0xAFFFF. That means PRA has to be
 * loaded with a value that points to the previous logical segment.
 *
 * The latter FEATURE was mentioned correctly (but somewhat not understandable)
 * in the registerdoc of the PVGA1A. But it was COMPLETELY WRONG shown in their
 * programming examples....
 */

#include "assyntax.h"

	FILE("pvga1bank.s")

	AS_BEGIN
	SEG_TEXT

/* 
 * for ReadWrite operations, we are using only PR0B as pointer to a 32k
 * window.
 */
	ALIGNTEXT4
	GLOBL	GLNAME(PVGA1SetReadWrite)
GLNAME(PVGA1SetReadWrite):
	SHL_L	(CONST(11),EAX)            /* combined %al*8 & movb %al,%ah */
	MOV_B	(CONST(10),AL)
	MOV_L	(CONST(0x3CE),EDX)
	OUT_W
	RET

/* 
 * for Write operations, we are using PR0B as write pointer to a 32k
 * window.
 */
	ALIGNTEXT4
	GLOBL	GLNAME(PVGA1SetWrite)
GLNAME(PVGA1SetWrite):
	SHL_L	(CONST(11),EAX)
	MOV_B	(CONST(10),AL)
	MOV_L	(CONST(0x3CE),EDX)
	OUT_W
	RET

/* 
 * for Read operations, we are using PR0A as read pointer to a 32k
 * window.
 */
	ALIGNTEXT4
	GLOBL	GLNAME(PVGA1SetRead)
GLNAME(PVGA1SetRead):
	DEC_L	(EAX)			/* segment wrap ... */
	SHL_L	(CONST(11),EAX)
	MOV_B	(CONST(9),AL)
	MOV_L	(CONST(0x3CE),EDX)
	OUT_W
	RET

	AS_BEGIN

	SEG_DATA
oldpr34:
	D_WORD	0x14
newpr34:
 	D_WORD	0x14		

	SEG_TEXT

/* 
 * for ReadWrite operations, we are using only PR0B as pointer to a 32k
 * window.
 */
	ALIGNTEXT4
	GLOBL	GLNAME(WD90C33SetReadWrite)
GLNAME(WD90C33SetReadWrite):
	SHL_W	(CONST(11),AX)            /* combined %al*8 & movb %al,%ah */
	JC	(LA0) 
	/* Carry is not set */
	AND_W	(CONST(0X7FFF),CONTENT(newpr34))
	JP	(LA1)
LA0:
	/* Carry is set */
	OR_W	(CONST(0X8000),CONTENT(newpr34))
LA1:
	
	/* Write PRB */
	MOV_B	(CONST(10),AL)
	MOV_L	(CONST(0x3CE),EDX)
	OUT_W

	/* Write extra bit if necessary */
	MOV_W	(CONTENT(newpr34),AX)
	CMP_W	(CONTENT(oldpr34),AX)
	JZ	(LA2)
	MOV_L	(CONST(0x3C4),EDX)
	OUT_W
        /* Remember old value */
	MOV_W	(AX,CONTENT(oldpr34))
LA2:
	RET

/* 
 * for Write operations, we are using PR0B as write pointer to a 32k
 * window.
 */
	ALIGNTEXT4
	GLOBL	GLNAME(WD90C33SetWrite)
GLNAME(WD90C33SetWrite):
	SHL_W	(CONST(11),AX)
	JC	(LB0) 
	/* Carry is not set */
	AND_W	(CONST(0X7FFF),CONTENT(newpr34))
	JP	(LB1)
LB0:
	/* Carry is set */
	OR_W	(CONST(0X8000),CONTENT(newpr34))
LB1:
	
	/* Write PRB */
	MOV_B	(CONST(10),AL)
	MOV_L	(CONST(0x3CE),EDX)
	OUT_W

	/* Write extra bit if necessary */
	MOV_W	(CONTENT(newpr34),AX)
	CMP_W	(CONTENT(oldpr34),AX)
	JZ	(LB2)
	MOV_B	(CONST(0x14),AL)
	MOV_L	(CONST(0x3C4),EDX)
	OUT_W
        /* Remember old value */
	MOV_W	(AX,CONTENT(oldpr34))
LB2:
	RET

/* 
 * for Read operations, we are using PR0A as read pointer to a 32k
 * window.
 */
	ALIGNTEXT4
	GLOBL	GLNAME(WD90C33SetRead)
GLNAME(WD90C33SetRead):
	DEC_L	(EAX)			/* segment wrap ... */
	SHL_W	(CONST(11),AX)
	JC	(L0) 
	/* Carry is not set */
	AND_W	(CONST(0XBFFF),CONTENT(newpr34))
	JP	(L1)
L0:
	/* Carry is set */
	OR_W	(CONST(0X4000),CONTENT(newpr34))
L1:
	
	/* Write PRA */
	MOV_B	(CONST(9),AL)
	MOV_L	(CONST(0x3CE),EDX)
	OUT_W

	/* Write extra bit if necessary */
	MOV_W	(CONTENT(newpr34),AX)
	CMP_W	(CONTENT(oldpr34),AX)
	JZ	(L2)
	MOV_B	(CONST(0x14),AL)
	MOV_L	(CONST(0x3C4),EDX)
	OUT_W

        /* Remember old value */
	MOV_W	(AX,CONTENT(oldpr34))
L2:
	RET

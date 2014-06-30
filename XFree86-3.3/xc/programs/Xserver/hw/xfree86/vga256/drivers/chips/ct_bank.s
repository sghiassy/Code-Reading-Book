/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_bank.s,v 3.8 1997/01/22 11:08:45 dawes Exp $ */
/*
 * Copyright 1994 by Régis Cridlig <cridlig@dmi.ens.fr>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Jon Block not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Jon Block makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * JON BLOCK DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL JON BLOCK BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/* $XConsortium: ct_bank.s /main/5 1996/10/19 18:10:18 kaleb $ */


#include "assyntax.h"

	FILE("chips_bank.s")

	AS_BEGIN

	SEG_TEXT

	ALIGNTEXT4		
	GLOBL	GLNAME(CHIPSSetReadWrite)
	GLOBL	GLNAME(CHIPSSetWrite)
	GLOBL	GLNAME(CHIPSSetRead)
	GLOBL	GLNAME(CHIPSWINSetReadWrite)
	GLOBL	GLNAME(CHIPSWINSetWrite)
	GLOBL	GLNAME(CHIPSWINSetRead)
	GLOBL	GLNAME(CHIPSHiQVSetReadWrite)
	GLOBL	GLNAME(CHIPSHiQVSetWrite)
	GLOBL	GLNAME(CHIPSHiQVSetRead)
#if 0
	GLOBL	GLNAME(ctPrintReadWrite)
	GLOBL	GLNAME(ctPrintWrite)		
	GLOBL	GLNAME(ctPrintRead)
#endif
	
GLNAME(CHIPSSetReadWrite):
	MOV_B	(AL, AH)		/* Move bank to high half */
	SHL_B	(CONST(3), AH)
	MOV_B	(CONST(0x10),AL)	/* Put read index in low byte */
	MOV_L	(CONST(0x3D6),EDX)	/* Store 0x3D6 register */
	OUT_W				/* Output read bank */
	MOV_B	(CONST(0x11),AL)	/* Put write index in low byte */
	MOV_L	(CONST(0x3D6),EDX)	/* Store 0x3D6 register */
	OUT_W				/* Output read bank */
	RET

GLNAME(CHIPSSetWrite):
	MOV_B	(AL, AH)		/* Move bank to high half */
	SHL_B	(CONST(3), AH)
	MOV_B	(CONST(0x11),AL)	/* Put write index in low byte */
	MOV_L	(CONST(0x3D6),EDX)	/* Store 0x3D6 register */
	OUT_W				/* Output read bank */
	RET

GLNAME(CHIPSSetRead):
	MOV_B	(AL, AH)		/* Move bank to high half */
	SHL_B	(CONST(3), AH)
	MOV_B	(CONST(0x10),AL)	/* Put read index in low byte */
	MOV_L	(CONST(0x3D6),EDX)	/* Store 0x3D6 register */
	OUT_W				/* Output read bank */
	RET

GLNAME(CHIPSWINSetReadWrite):
	PUSH_W  (BX)
	MOV_B   (AL,BL)
	SHL_B	(CONST(1), AL)		/* shift left 1 bits (high map ovflw)*/
	AND_B	(CONST(0x40), AL)	/* mask out bit 6 */
	MOV_B	(AL, AH)		/* move to high byte */
	MOV_B	(CONST(0x0C), AL)	/* address overflow register */	
	MOV_L	(CONST(0x3D6), EDX)	/* Store 0x3D6 register */
	OUT_B				/* write index */	
	MOV_L	(CONST(0x3D7), EDX)	/* Store 0x3D7 register */
	IN_B				/* read data */
	AND_B	(CONST(0xAF), AL)	/* mask out overflow bits */
	OR_B	(AH, AL)		/* set overflow bits */
	SHR_B	(CONST(2), AH)          /* shift right 2 bits (low map ovflw)*/
	OR_B	(AH, AL)		/* include bit 4 */
	MOV_B	(AL, AH)		/* move to high byte */
	MOV_B	(CONST(0x0C), AL)	/* address overflow register */	
	MOV_L	(CONST(0x3D6), EDX)	/* Store 0x3D6 register */
	OUT_W				/* write them *//*!!*/
	MOV_B	(BL, AL)
	MOV_B	(AL, AH)		/* Move bank to high half */
	SHL_B	(CONST(3), AH)
	MOV_B	(CONST(0x10),AL)	/* Put read index in low byte */
	MOV_L	(CONST(0x3D6),EDX)	/* Store 0x3D6 register */
	OUT_W				/* Output read bank */
	MOV_B	(CONST(0x11),AL)	/* Put write index in low byte */
	MOV_L	(CONST(0x3D6),EDX)	/* Store 0x3D6 register */
	OUT_W				/* Output read bank */
	POP_W	(BX)
	RET

GLNAME(CHIPSWINSetWrite):
	PUSH_W	(BX)
	MOV_B   (AL,BL)
	SHL_B	(CONST(1),AL)           /* Shift left 1 bit (high map) */
	AND_B	(CONST(0x40),AL)	/* Mask out bit 6 (high map) */
	MOV_B	(AL, AH)		/* Move to high half */
	MOV_B	(CONST(0x0C),AL)	/* Address overflow register */	
	MOV_L	(CONST(0x3D6),EDX)	/* Store 0x3D6 register */
	OUT_B				/* Write index */	
	MOV_L	(CONST(0x3D7),EDX)	/* Store 0x3D7 register */
	IN_B				/* Read data */
	AND_B	(CONST(0xBF),AL)	/* Zero bit 6 */
	OR_B	(AH, AL)		/* set bit 6 */
	MOV_B	(AL, AH)		/* move to high byte */
	MOV_B	(CONST(0x0C), AL)	/* address overflow register */	
	MOV_L	(CONST(0x3D6), EDX)	/* Store 0x3D6 register */
	OUT_W				/* write them *//*!!*/
	MOV_B	(BL,AL)
	MOV_B	(AL, AH)		/* Move bank to high half */
	SHL_B	(CONST(3), AH)
	MOV_B	(CONST(0x11),AL)	/* Put write index in low byte */
	MOV_L	(CONST(0x3D6),EDX)	/* Store 0x3D6 register */
	OUT_W				/* Output read bank */
	POP_W	(BX)			/* Get from stack */
	RET

GLNAME(CHIPSWINSetRead):
	PUSH_W  (BX)
	MOV_B	(AL,BL)
	SHR_B	(CONST(1),AL)           /* Shift right 1 bit (low map) */
	AND_B	(CONST(0x10),AL)	/* Mask out bit 4 (low map) */
	MOV_B	(AL, AH)		/* Move to high half */
	MOV_B	(CONST(0x0C),AL)	/* Address overflow register */	
	MOV_L	(CONST(0x3D6),EDX)	/* Store 0x3D6 register */
	OUT_B				/* Write index */	
	MOV_L	(CONST(0x3D7),EDX)	/* Store 0x3D7 register */
	IN_B				/* Read data */
	AND_B	(CONST(0xEF),AL)	/* Zero bit 4 */
	OR_B	(AH, AL)		/* set bit 4 */
	OUT_B
	MOV_B	(AL, AH)		/* move to high byte */
	MOV_B	(CONST(0x0C), AL)	/* address overflow register */	
	MOV_L	(CONST(0x3D6), EDX)	/* Store 0x3D6 register */
	OUT_W				/* write them *//*!!*/
	MOV_B	(BL,AL)
	MOV_B	(AL, AH)		/* Move bank to high half */
	SHL_B	(CONST(3), AH)
	MOV_B	(CONST(0x10),AL)	/* Put read index in low byte */
	MOV_L	(CONST(0x3D6),EDX)	/* Store 0x3D6 register */
	OUT_W				/* Output read bank */
	POP_W	(BX)			/* Get from stack */
	RET
	
GLNAME(CHIPSHiQVSetReadWrite):
GLNAME(CHIPSHiQVSetWrite):
GLNAME(CHIPSHiQVSetRead):
	MOV_B	(AL, AH)		/* Move bank to high half */
	AND_B	(CONST(0x7F), AH)       /* Mask out bits 6-0 */
	MOV_B	(CONST(0x0E),AL)	/* Put read/write index in low byte */
	MOV_L	(CONST(0x3D6),EDX)	/* Store 0x3D6 register */
	OUT_W				/* Output read/write bank */
	RET

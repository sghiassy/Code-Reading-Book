/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/ark/ark_bank.s,v 3.2 1996/12/23 06:55:59 dawes Exp $ */
/*
 * Copyright 1994  The XFree86 Project
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
 * DAVID WEXELBLAT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Written by Harm Hanemaayer (hhanemaa@cs.ruu.nl).
 */
/* $XConsortium: ark_bank.s /main/4 1996/02/21 18:01:55 kaleb $ */

#include "assyntax.h"

	FILE("ark_bank.s")	/* Define the file name for the .o file */

	AS_BEGIN		/* This macro does all generic setup */

	SEG_TEXT		/* Switch to the text segment */

/* 
 * The SetReadWrite function sets both bank pointers.  The bank will be
 * passed in AL.  As an example, this stub assumes that the read bank 
 * register is register 'base', index 'idx_r', and the write bank register
 * is index 'idx_w'.
 */
	ALIGNTEXT4		
	GLOBL	GLNAME(ArkSetReadWrite)
GLNAME(ArkSetReadWrite):
	MOV_B	(AL, AH)		/* Move bank to high half */
	MOV_B	(CONST(0x16),AL)	/* Put read index in low byte */
	MOV_L	(CONST(0x3C4),EDX)	/* Store base register */
	OUT_W				/* Output read bank */
	MOV_B	(CONST(0x15),AL)	/* Put write index in low byte */
	OUT_W				/* Output write bank */
	RET

/* 
 * The SetWrite function sets just the write bank pointer
 */
	ALIGNTEXT4
	GLOBL	GLNAME(ArkSetWrite)
GLNAME(ArkSetWrite):
	MOV_B	(AL, AH)		/* Move bank to high half */
	MOV_L	(CONST(0x3C4),EDX)	/* Store base register */
	MOV_B	(CONST(0x15),AL)	/* Put write index in low byte */
	OUT_W				/* Output write bank */
	RET

/* 
 * The SetRead function sets just the read bank pointer
 */
	ALIGNTEXT4
	GLOBL	GLNAME(ArkSetRead)
GLNAME(ArkSetRead):
	MOV_B	(AL, AH)		/* Move bank to high half */
	MOV_B	(CONST(0x16),AL)	/* Put read index in low byte */
	MOV_L	(CONST(0x3C4),EDX)	/* Store base register */
	OUT_W				/* Output read bank */
	RET

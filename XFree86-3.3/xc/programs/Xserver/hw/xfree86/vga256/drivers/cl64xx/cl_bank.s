/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cl64xx/cl_bank.s,v 3.3 1996/12/23 06:57:05 dawes Exp $ */
/*
 * Stubs driver Copyright 1993 by David Wexelblat <dwex@goblin.org>
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
 *
 * Cirrus Logic GD-64XX Notebook Graphics Controller extensions,
 * Copyright 1994 by Manfred Brands <mb@oceonics.nl>
 */
/* $XConsortium: cl_bank.s /main/3 1996/02/21 18:04:39 kaleb $ */

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
 *	CL64XXSetRead      - Set the read-bank pointer
 *	CL64XXSetWrite     - Set the write-bank pointer
 *	CL64XXSetReadWrite - Set both bank pointers to the same bank
 *
 * We must convert the 4K banks of the CL64XX into the 32K banks used
 * in the driver -> shift left 3 places
 * To use one outw instruction we must shift another 8 places -> 11 total
 */

	FILE("bank.s")		/* Define the file name for the .o file */

	AS_BEGIN		/* This macro does all generic setup */

	SEG_TEXT		/* Switch to the text segment */

	ALIGNTEXT4
	GLOBL	GLNAME(CL64XXSetRead)
GLNAME(CL64XXSetRead):
	SHL_L	(CONST(11),EAX)
	MOV_B	(CONST(0x0E),AL)	/* Put read index in low byte */
	MOV_L	(CONST(0x3CE),EDX)	/* Store base register */
	OUT_W				/* Output read bank */
	RET

	ALIGNTEXT4		
	GLOBL	GLNAME(CL64XXSetReadWrite)
	GLOBL	GLNAME(CL64XXSetWrite)
GLNAME(CL64XXSetReadWrite):
GLNAME(CL64XXSetWrite):
	SHL_L	(CONST(11),EAX)
	MOV_B	(CONST(0x0F),AL)	/* Put write index in low byte */
	MOV_L	(CONST(0x3CE),EDX)	/* Store base register */
	OUT_W				/* Output write bank */
	RET


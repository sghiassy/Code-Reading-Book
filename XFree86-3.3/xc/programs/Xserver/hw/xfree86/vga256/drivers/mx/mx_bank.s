/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/mx/mx_bank.s,v 3.2 1996/12/23 06:57:42 dawes Exp $ */
/*
 * Driver Stubs Copyright 1993 by David Wexelblat <dwex@goblin.org>
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
/* $XConsortium: mx_bank.s /main/3 1996/02/21 18:05:36 kaleb $ */


#include "assyntax.h"

/*
 * This file is part of the mxic SVGA driver for XFree86. (MX68000, MX68010)
 *
 * Built from XFree86 stub file
 * 29/04/94 Frank Dikker (dikker@cs.utwente.nl)
 * 
 * Specific card settings are taken from Finn Thoegersen's VGADOC package.
 *
 */

	
	FILE("mx_bank.s")	/* Define the file name for the .o file */

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
 * passed in AL.  As an example, this  assumes that the read bank 
 * register is register 'base', index 'idx_r', and the write bank register
 * is index 'idx_w'.
 */
	ALIGNTEXT4		
	GLOBL	GLNAME(MXSetReadWrite)
GLNAME(MXSetReadWrite):
	MOV_B	(AL,AH)		/* Move bank to high half */
	SHL_B	(CONST(4),AH)           
	OR_B    (AL,AH)	                /* both in AH */
	MOV_B   (AH,CONTENT(Segment))
	MOV_B   (CONST(0xc5),AL)	
	MOV_L   (CONST(0x3c4),EDX)
	OUT_W				/* Output readwrite bank */
	RET

/* 
 * The SetWrite function sets just the write bank pointer
 */
	ALIGNTEXT4
	GLOBL	GLNAME(MXSetWrite)
GLNAME(MXSetWrite):
	MOV_B   (CONTENT(Segment),AH)
	AND_B   (CONST(0xf0),AH)
	OR_B    (AL,AH)
	MOV_B   (AH,CONTENT(Segment))
	MOV_L	(CONST(0x3c4),EDX)	/* Store base register */
	MOV_B	(CONST(0xc5),AL)	/* Put write index in low byte */
	OUT_W				/* Output write bank */
	RET

/* 
 * The SetRead function sets just the read bank pointer
 */
	ALIGNTEXT4
	GLOBL	GLNAME(MXSetRead)
GLNAME(MXSetRead):
	MOV_B   (CONTENT(Segment),AH)
	AND_B   (CONST(0xf),AH)
	SHL_B   (CONST(4),AL)
	OR_B    (AL,AH)
	MOV_B   (AH,CONTENT(Segment))
        MOV_L   (CONST(0x3c4),EDX)
	MOV_B	(CONST(0xc5),AL)	/* Put read index in low byte */
	OUT_W				/* Output read bank */
	RET


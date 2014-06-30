/* $XConsortium: apm_bank.s /main/2 1996/10/23 13:23:59 kaleb $ */




/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/apm/apm_bank.s,v 3.1 1996/12/27 07:04:45 dawes Exp $ */

#include "assyntax.h"

	FILE("apm_bank.s")	/* Define the file name for the .o file */

	AS_BEGIN		/* This macro does all generic setup */

	SEG_TEXT		/* Switch to the text segment */

/* 
 * The SetReadWrite function sets both bank pointers.  The bank will be
 * passed in AL.  As an example, this stub assumes that the read bank 
 * register is register 'base', index 'idx_r', and the write bank register
 * is index 'idx_w'.
 */
	ALIGNTEXT4		
	GLOBL	GLNAME(ApmSetWrite)
GLNAME(ApmSetWrite):
	GLOBL	GLNAME(ApmSetReadWrite)
GLNAME(ApmSetReadWrite):
	GLOBL	GLNAME(ApmSetRead)
GLNAME(ApmSetRead):
	PUSH_L  (EBX)
	MOV_B	(AL, AH)		/* Move bank to high half */
	MOV_B   (CONST(0x1d),AL)
	MOV_W   (CONST(0x3C4),DX)
	OUT_B
	INC_W   (DX)
	MOV_B   (CONST(0x30),AL)
	OUT_B
	DEC_W   (DX)
	MOV_B   (CONST(0x1e),AL)
	OUT_B
	INC_W   (DX)
	IN_B
	MOV_B   (AL,BL)
	DEC_W   (DX)
	MOV_B   (CONST(0x1f),AL)
	OUT_B
	INC_W   (DX)
	IN_B
	MOV_B   (AL,DH)
	MOV_B   (BL,DL)
	XOR_B   (AL,AL)
	SHR_W   (CONST(4),AX)
	OUT_W
	POP_L   (EBX)
	RET

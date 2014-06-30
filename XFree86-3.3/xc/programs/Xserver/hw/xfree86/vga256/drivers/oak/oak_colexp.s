/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/oak/oak_colexp.s,v 3.2 1996/12/23 06:57:53 dawes Exp $ */





/* $XConsortium: oak_colexp.s /main/2 1996/02/21 18:06:02 kaleb $ */

#include "assyntax.h"

 	FILE("oak_colexp.S")

	AS_BEGIN
/*
 * stack frame argument definitions for solid fill 
 */

#define address		REGOFF(8,EBP)
#define width		REGOFF(12,EBP)
#define height		REGOFF(16,EBP)
#define mask_height     REGOFF(12,EBP)
	
	SEG_TEXT
	ALIGNTEXT4

	GLOBL GLNAME(oti087ColorExpandFill)
GLNAME(oti087ColorExpandFill):

	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)

	PUSH_L	(EDI)
	PUSH_L	(EAX)
	PUSH_L	(EBX)
	PUSH_L  (ECX)
	PUSH_L	(EDX)
	
	MOV_L	(address,EBX)
	MOV_L	(width,ECX)
	MOV_L	(height,EDX)
.externloop:
	MOV_L	(EBX,EDI)
	MOV_L   (ECX,EAX)
.internloop:
	MOV_W	(AX,REGIND(EDI))
	ADD_L	(CONST(8),EDI)
	DEC_L	(EAX)
	JNE	(.internloop)
	ADD_L	(CONST(1024),EBX)
	DEC_L	(EDX)
	JNE	(.externloop)

	POP_L	(EDX)
	POP_L   (ECX)
	POP_L	(EBX)
	POP_L	(EAX)
	POP_L	(EDI)
	POP_L   (EBP)

	RET

/* This one is for the sides, uses masked expansion */
	
	SEG_TEXT
	ALIGNTEXT4

	GLOBL GLNAME(oti087ColorMaskedFill)
GLNAME(oti087ColorMaskedFill):

	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)

	PUSH_L	(EDI)
	PUSH_L	(EAX)
	PUSH_L	(EBX)
		
	MOV_L	(address,EDI)
	MOV_L	(mask_height,EBX)
	

.loop:	
	MOV_W	(AX,REGIND(EDI))	
	ADD_L	(CONST(1024),EDI)
	DEC_L	(EBX)
	JNE	(.loop) 

	POP_L	(EBX)
	POP_L	(EAX)
	POP_L	(EDI)

	POP_L   (EBP)

	RET

/* $XConsortium: bank.s /main/1 1996/09/21 14:56:29 kaleb $ */





/* $XFree86: xc/programs/Xserver/hw/xfree98/vga256/drivers/nec480/bank.s,v 3.1 1996/12/27 07:07:25 dawes Exp $ */

#include "assyntax.h"

	FILE("nec480bank.s")
	AS_BEGIN

	SEG_DATA

	SEG_TEXT

	ALIGNTEXT4
	GLOBL GLNAME(readseg)
	GLOBL GLNAME(writeseg)
	GLOBL GLNAME(vramwindow_r)
	GLOBL GLNAME(vramwindow_w)

	ALIGNTEXT4
GLNAME(NEC480SetReadWrite):
	GLOBL GLNAME(NEC480SetReadWrite)
	MOV_L	(GLNAME(writeseg),EAX)
	MOV_L	(GLNAME(vramwindow_r),EDX)
	MOV_W	(AX,REGIND(EDX))
	MOV_L	(GLNAME(vramwindow_w),EDX)
	MOV_W	(AX,REGIND(EDX))
	RET

	ALIGNTEXT4
GLNAME(NEC480SetRead):
	GLOBL GLNAME(NEC480SetRead)
	MOV_L	(GLNAME(readseg),EAX)
	MOV_L	(GLNAME(vramwindow_r),EDX)
	MOV_W	(AX,REGIND(EDX))
	RET

	ALIGNTEXT4
GLNAME(NEC480SetWrite):
	GLOBL GLNAME(NEC480SetWrite)
	MOV_L	(GLNAME(writeseg),EAX)
	MOV_L	(GLNAME(vramwindow_w),EDX)
	MOV_W	(AX,REGIND(EDX))
	RET

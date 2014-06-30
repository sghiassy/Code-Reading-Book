/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/sis/sis_bank.s,v 3.2 1996/12/23 06:58:30 dawes Exp $ */
/*
 * Copyright 1995 by Alan Hourihane, Wigan, England.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Alan Hourihane not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Alan Hourihane makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALAN HOURIHANE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ALAN HOURIHANE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Alan Hourihane, alanh@fairlite.demon.co.uk
 */
/* $XConsortium: sis_bank.s /main/2 1996/02/21 18:07:50 kaleb $ */

/*
 * These are here the very lowlevel VGA bankswitching routines.
 * The segment to switch to is passed via %eax. Only %eax and %edx my be used
 * without saving the original contents.
 */

#include "assyntax.h"

	FILE("sisbank.s")
	AS_BEGIN

	SEG_DATA

	SEG_TEXT

	ALIGNTEXT4
	GLOBL GLNAME(SISSetReadWrite)
GLNAME(SISSetReadWrite):
	MOV_L	(CONST(0x3CB),EDX)
	OUT_B
	MOV_L	(CONST(0x3CD),EDX)
	OUT_B
	RET

	ALIGNTEXT4
	GLOBL GLNAME(SISSetRead)
GLNAME(SISSetRead):
	MOV_L	(CONST(0x3CB),EDX)
	OUT_B
	RET
	
	ALIGNTEXT4
	GLOBL GLNAME(SISSetWrite)
GLNAME(SISSetWrite):
	MOV_L	(CONST(0x3CD),EDX)
	OUT_B
	RET

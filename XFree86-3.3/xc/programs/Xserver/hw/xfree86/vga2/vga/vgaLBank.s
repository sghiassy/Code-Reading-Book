/* $XFree86: xc/programs/Xserver/hw/xfree86/vga2/vga/vgaLBank.s,v 3.3 1996/12/23 06:55:32 dawes Exp $ */
/* The original file was: */
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

/* This version is from Pascal Haible, haible@IZFM.Uni-Stuttgart.DE
 * The same permissions and disclaimer apply to this version. */
/* $XConsortium: vgaLBank.s /main/3 1996/02/21 18:01:15 kaleb $ */

#include "assyntax.h"

	FILE("vgaLBank.s")
	AS_BEGIN

#include "vgaAsm.h"

/*
	SEG_DATA
	GLOBL GLNAME(vgalinebankdummy)
GLNAME(vgalinebankdummy):
	D_LONG 0
*/
	SEG_TEXT
/*
 *-----------------------------------------------------------------------
 * vgaBankReadWrite ---
 *     select a memory bank of the VGA board for read & write access
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 * void
 * vgaBankReadWrite(n)
 *     register int n;
 * {
 *   (vgaSetReadWriteFunc)(n);
 * }
 *
 */
	ALIGNTEXT4
	GLOBL GLNAME(vgaBankReadWrite)
GLNAME(vgaBankReadWrite):
	MOV_L	(REGOFF(4,ESP),EAX)
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(vgaSetReadWriteFunc)),EDX)
	CALL	(CODEPTR(EDX))
	POP_L	(EDX)
 	RET

/*
 *-----------------------------------------------------------------------
 * vgaBankRead ---
 *     select a memory bank of the VGA board for read access
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 * void
 * vgaBankRead(n)
 *     register int n;
 * {
 *   (vgaSetReadFunc)(n);
 * }
 *
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaBankRead)
GLNAME(vgaBankRead):
	MOV_L	(REGOFF(4,ESP),EAX)
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(vgaSetReadFunc)),EDX)
	CALL	(CODEPTR(EDX))
	POP_L	(EDX)
 	RET

/*
 *-----------------------------------------------------------------------
 * vgaBankWrite ---
 *     select a memory bank of the VGA board for write access
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 * void
 * vgaBankWrite(n)
 *     register int n;
 * {
 *   (vgaSetWriteFunc)(n);
 * }
 *
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaBankWrite)
GLNAME(vgaBankWrite):
	MOV_L	(REGOFF(4,ESP),EAX)
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(vgaSetWriteFunc)),EDX)
	CALL	(CODEPTR(EDX))
	POP_L	(EDX)
 	RET

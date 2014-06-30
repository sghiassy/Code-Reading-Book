/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_bank.s,v 3.4 1996/12/23 06:56:32 dawes Exp $ */
/*
 *
 * Copyright 1990,91 by Bill Reynolds, Santa Fe, New Mexico
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Bill Reynolds not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Bill Reynolds makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * BILL REYNOLDS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL BILL REYNOLDS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Bill Reynolds, bill@goshawk.lanl.gov
 * Changes: Piercarlo Grandi, Aberystwyth (pcg@aber.ac.uk)
 *
 */
/* $XConsortium: cir_bank.s /main/5 1996/02/21 18:03:06 kaleb $ */

/*
 * These are here the very lowlevel VGA bankswitching routines.
 * The segment to switch to is passed via %eax. Only %eax and %edx my be used
 * without saving the original contents.
 */

/*
 * We always have 256 granules, each either 4KB if total memory is 1MB,
 * or 16KB if total memory is 2 (-4MB?). Segments addresses are expressed
 * in granules, and the granule address is set in AH. Segment numbers
 * passed to these routines are expressed in multiples of 32KB. Thus
 * we must convert this number to a granule, and then shift it into
 * AH.
 */

#ifdef PC98_WAB
#define cirrus1MBSHIFT CONST(10)	/* 8 + (lg(16KB) - lg(4KB))     */
#define cirrus2MBSHIFT CONST(8)		/* 8 + (lg(16KB) - lg(16KB))    */
#else
#ifdef PC98_WABEP
#define cirrus1MBSHIFT CONST(10)	/* 8 + (lg(16KB) - lg(4KB))     */
#define cirrus2MBSHIFT CONST(9)		/* 8 + (lg(32KB) - lg(16KB))    */
#else
#define cirrus1MBSHIFT CONST(11)	/* 8 + (lg(32KB) - lg(4KB))     */
#define cirrus2MBSHIFT CONST(9)		/* 8 + (lg(32KB) - lg(16KB))    */
#endif
#endif

#if defined(PC98_WAB) || defined(PC98_GANB_WAP)
#define GRX	CONST(0x4ee0)
#else
#ifdef PC98_WSNA
#define GRX	CONST(0x4ee2)
#else
#ifdef PC98_WABEP
#define GRX	CONST(0xf4e)
#else
#ifdef PC98_NKVNEC
#define GRX	CONST(0x0cae)
#else
#define GRX	CONST(0x3ce)
#endif
#endif
#endif
#endif

#include "assyntax.h"

 	FILE("cirrusbank.s")

	AS_BEGIN

	SEG_TEXT

	ALIGNTEXT4
	GLOBL GLNAME(cirrusSetRead)
GLNAME(cirrusSetRead):
	SHL_L	(cirrus1MBSHIFT,EAX)
	MOV_B   (CONST(0x09),AL)
	MOV_L	(GRX,EDX)
	OUT_W
	RET

	ALIGNTEXT4
	GLOBL GLNAME(cirrusSetReadWrite)
	GLOBL GLNAME(cirrusSetWrite)
GLNAME(cirrusSetReadWrite):
GLNAME(cirrusSetWrite):
	SHL_L	(cirrus1MBSHIFT,EAX)
	MOV_B   (CONST(0x0A),AL)
	MOV_L	(GRX,EDX)
	OUT_W
	RET

	ALIGNTEXT4
	GLOBL GLNAME(cirrusSetRead2MB)
GLNAME(cirrusSetRead2MB):
	SHL_L	(cirrus2MBSHIFT,EAX)
	MOV_B   (CONST(0x09),AL)
	MOV_L	(GRX,EDX)
	OUT_W
	RET

	ALIGNTEXT4
	GLOBL GLNAME(cirrusSetReadWrite2MB)
	GLOBL GLNAME(cirrusSetWrite2MB)
GLNAME(cirrusSetReadWrite2MB):
GLNAME(cirrusSetWrite2MB):
	SHL_L	(cirrus2MBSHIFT,EAX)
	MOV_B   (CONST(0x0A),AL)
	MOV_L	(GRX,EDX)
	OUT_W
	RET

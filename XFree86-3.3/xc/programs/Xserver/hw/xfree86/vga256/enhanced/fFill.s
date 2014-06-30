/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/enhanced/fFill.s,v 3.6 1996/12/23 06:59:02 dawes Exp $ */
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
 * This file was derived from a similar X11R4 file (X386 1.1) for X11R5
 * (X386 1.2) by James Tsillas.  This file was further derived to its current
 * form by David Wexelblat (dwex@goblin.org).
 *
 */
/* $XConsortium: fFill.s /main/6 1996/02/21 18:08:43 kaleb $ */

#include "assyntax.h"

#if __STDC__ && !defined(UNIXCPP)
#define RROP_NAME_CAT(prefix,suffix)    prefix##suffix
#else
#define RROP_NAME_CAT(prefix,suffix)    prefix/**/suffix
#endif

#define GXAnd	1
#define GXCopy	2
#define GXOr	3
#define GXXor	4

#if RROP == GXAnd
	FILE("fFillAnd.s")
#  define RROPB	AND_B
#  define RROPW AND_W
#  define RROPL AND_L
#  define RROP_NAME(pref)	RROP_NAME_CAT(pref,GXand)
#elif RROP == GXOr
	FILE("fFillOr.s")
#  define RROPB	OR_B
#  define RROPW OR_W
#  define RROPL OR_L
#  define RROP_NAME(pref)	RROP_NAME_CAT(pref,GXor)
#elif RROP == GXXor
	FILE("fFillXor.s")
#  define RROPB	XOR_B
#  define RROPW XOR_W
#  define RROPL XOR_L
#  define RROP_NAME(pref)	RROP_NAME_CAT(pref,GXxor)
#elif RROP == GXCopy
	FILE("fFillCopy.s")
#  define RROPB	MOV_B
#  define RROPW MOV_W
#  define RROPL MOV_L
#  define RROP_NAME(pref)	RROP_NAME_CAT(pref,GXcopy)
#endif

	AS_BEGIN

#define GL_RROP_NAME    RROP_NAME(GLNAME(fastFillSolid))

/*
 *
 * This routine implements a fast Solid Fill in GXcopy
 * No segment checking is done.
 *
 * SYNTAX:
 * unchar * fastFillSolid<RROP>(pdst,fill,dummy,hcount,count,width,widthPitch);
 * 
 *  (7/27/90 TR)
 *  (4/92 JT)
 *  (5/92 DW)
 */

#define pdst        EDI
#define fill        EAX
#define fillw       AX
#define fillb       AL
#define count       ESI
#define hcount      ECX
#define width       REGOFF(28,EBP)
#define widthPitch  EBX
#define tmp         EAX


	SEG_DATA
	ALIGNDATA4
countt:	
	D_LONG 0
	SEG_TEXT
	ALIGNTEXT4

#define rrop_name	/**/RROP_NAME(fastFillSolid)

GLOBL	GL_RROP_NAME

GL_RROP_NAME:
	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	PUSH_L	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)
	MOV_L	(REGOFF(8,EBP),pdst)
	MOV_L	(REGOFF(12,EBP),fill)
	MOV_L	(REGOFF(20,EBP),hcount)
	MOV_L	(REGOFF(24,EBP),count)
	MOV_L	(REGOFF(32,EBP),widthPitch)
	OR_L	(hcount,hcount)
	JZ	(.finish)
	OR_L	(count,count)
	JZ	(.finish)
	CMP_L	(CONST(3),count)
	JG	(.startblockloop)
	JE	(.tribbleloop)
	CMP_L	(CONST(2),count)
	JE	(.wordloop)
/*
 * do a fast vertical line
 */
	ALIGNTEXT4ifNOP
.byteloop:
	RROPB	(fillb,REGIND(pdst))
	LEA_L	(REGBID(widthPitch,pdst,1),pdst)
	DEC_L	(ECX)	
	JNZ	(.byteloop)
	JMP	(.finish)

	ALIGNTEXT4
.wordloop:
	RROPW	(fillw,REGIND(pdst))
	LEA_L	(REGBID(widthPitch,pdst,2),pdst)
	DEC_L	(ECX)	
	JNZ	(.wordloop)
	JMP	(.finish)

	ALIGNTEXT4
.tribbleloop:
	RROPW	(fillw,REGIND(pdst))
	RROPB	(fillb,REGOFF(2,pdst))
	LEA_L	(REGBID(widthPitch,pdst,3),pdst)
	DEC_L	(ECX)	
	JNZ	(.tribbleloop)
	JMP	(.finish)

#undef count
#define count ECX
#undef hcount
#define hcount ESI

.startblockloop:
	XCHG_L	(ECX, ESI)
	ALIGNTEXT4ifNOP
.blockloop:
	TEST_L	(CONST(1),pdst)
	JZ	(.alignword)
#if RROP == GXCopy
	STOS_B
#else
	RROPB	(fillb,REGIND(pdst))
	INC_L	(pdst)
#endif
	DEC_L	(count)
.alignword:
	TEST_L	(CONST(2),pdst)
	JZ	(.aligneddword)
#if RROP == GXCopy
	STOS_W
#else
	RROPW	(fillw,REGIND(pdst))
	ADD_L	(CONST(2),pdst)
#endif
	LEA_L	(REGOFF(-2,count),count)
.aligneddword:
	MOV_L	(count, CONTENT(countt))
	AND_L	(CONST(3), CONTENT(countt))
	SHR_L	(CONST(2), count)
#if RROP == GXCopy
	REPZ
	STOS_L
#else
	OR_L	(ECX, ECX)
	JZ	(.endloop)
	ALIGNTEXT4ifNOP
.loop0: RROPL	(fill,REGIND(pdst))
	ADD_L	(CONST(4),pdst)
	DEC_L	(ECX)	
	JNZ	(.loop0)
.endloop:
#endif
	TEST_L	(CONST(2),CONTENT(countt))
	JZ	(.fixupbyte)
#if RROP == GXCopy
	STOS_W
#else
	RROPW	(fillw,REGIND(pdst))
	ADD_L	(CONST(2),pdst)
#endif
.fixupbyte:
	TEST_L	(CONST(1),CONTENT(countt))
	JZ	(.enditeration)
#if RROP == GXCopy
	STOS_B
#else
	RROPB	(fillb,REGIND(pdst))
	INC_L	(pdst)
#endif
.enditeration:
	LEA_L	(REGBI(widthPitch,pdst),pdst)
	MOV_L	(width,count)
	DEC_L	(hcount)
	JNZ	(.blockloop)
.finish:
	MOV_L	(pdst,EAX)
	LEA_L	(REGOFF(-12,EBP),ESP)
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	LEAVE
	RET



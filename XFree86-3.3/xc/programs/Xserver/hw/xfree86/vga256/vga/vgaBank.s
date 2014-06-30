/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgaBank.s,v 3.7 1996/12/23 06:59:29 dawes Exp $ */
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
/* $XConsortium: vgaBank.s /main/7 1996/02/21 18:10:07 kaleb $ */

#include "assyntax.h"

	FILE("vgaBank.s")
	AS_BEGIN

/*
 * Because a modern VGA has more than 128kBytes (which are mappable into the
 * 386' memory some logic is required. The VGA's memory (logical VGA
 * address space) is devided into smaller parts (called logical segments). 
 * These segments are mapped to logical areas.
 *
 * There are there different logical mapping areas:
 *
 * Read:       an area which can be read from
 * Write:      an area which can be written to
 * ReadWrite:  here is both read an write possible
 *
 * It is permissable to use simultaneously a Read and a Write, but you can use
 * ReadWrite only as a single. 
 * For example the bitblitting code uses a Read area as source and a Write
 * area as destination. Most other routines use only a ReadWrite.
 *
 * A logical mapping area is described by some parameters (here I will for
 * example describe a Read area:
 *
 * ReadBottom     lowest accessable byte relative to the beginning of the
 *                VGA boards mapped memory.
 * 
 * ReadTop        highes accessable byte plus one.
 *
 * SegmentSize    size of such an mapped area (common for all three)
 *
 * SegmentShift   log2(SegmentSize) (used to compute the logical segment)
 *
 * SegmentMask    SegmentSize - 1 (used to mask the offset inter an area)
 *
 * 
 * All that the following routines are doing is computing for a given offset
 * into the logical VGA adress space the offset into such an logical area
 * and the logical segment number. By the way they call also the VGA board's
 * driver to set up the VGA's physical memory mapping according to the logical
 * that was requested by the calliie.
 *
 * For shake of simplicity Write and ReadWrite share the same Bottom & Top.
 * NOTE: Read & Write may have differnt starting addresses, or even common.
 *
 * There are multible routines present for the same effect. This was made
 * for effectivly interface lowlevel assembly language best.
 */

/*
 * BUGALERT: this should be gotten from vga.h. But since there some C lang.
 *           things, too ...
 */

#include "vgaAsm.h"


	SEG_DATA
	GLOBL GLNAME(writeseg)
#ifdef PC98_NEC480
	GLOBL GLNAME(readseg)	/* PC98_NEC480 */
#endif
GLNAME(writeseg):
	D_LONG 0
GLNAME(readseg):
	D_LONG 0
GLNAME(saveseg):
	D_LONG 0

	SEG_TEXT
/*
 *-----------------------------------------------------------------------
 * vgaSetReadWrite ---
 *     select a memory bank of the VGA board for read & write access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 * pointer
 * vgaSetReadWrite(p)
 *     register pointer p;
 * {
 * #ifdef XF86VGA16
 *   writeseg = ((unsigned long)p - vgaBase) >> vgaSegmentShift;
 *   (vgaSetReadWriteFunc)(writeseg);
 *   return (vgaWriteBottom + (((unsigned int)p - vgaBase) & vgaSegmentMask));
 * #else
 *   writeseg = ((unsigned long)p - VGABASE) >> vgaSegmentShift;
 *   (vgaSetReadWriteFunc)(writeseg);
 *   return (vgaWriteBottom + ((unsigned int)p & vgaSegmentMask));
 * #endif
 * }
 *
 */
	ALIGNTEXT4
	GLOBL GLNAME(vgaSetReadWrite)
GLNAME(vgaSetReadWrite):
	MOV_L	(REGOFF(4,ESP),EAX)
	PUSH_L	(ECX)
	PUSH_L	(EDX)
#ifdef XF86VGA16
	SUB_L	(CONTENT(GLNAME(vgaBase)),EAX)
#else
	SUB_L	(VGABASE,EAX)
#endif
	MOV_L	(CONTENT(GLNAME(vgaSegmentShift)),ECX)
	SHR_L	(CL,EAX)
	MOV_L	(EAX,CONTENT(GLNAME(writeseg)))
	MOV_L	(CONTENT(GLNAME(vgaSetReadWriteFunc)),EDX)
	CALL	(CODEPTR(EDX))
	POP_L	(EDX)
	POP_L	(ECX)
	MOV_L	(REGOFF(4,ESP),EAX)
#ifdef XF86VGA16
	SUB_L	(CONTENT(GLNAME(vgaBase)),EAX)
#endif
	AND_L	(CONTENT(GLNAME(vgaSegmentMask)),EAX)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)),EAX)
 	RET

/*
 *-----------------------------------------------------------------------
 * vgaReadWriteNext ---
 *     switch to next memory bank of the VGA board for read & write access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 *
 *
 * pointer
 * vgaReadWriteNext(p)
 *      register pointer p;
 * {
 *   (vgaSetReadWriteFunc)(++writeseg);
 *   return (p - vgaSegmentSize);
 * }
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaReadWriteNext)
GLNAME(vgaReadWriteNext):
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(writeseg)),EAX)
	INC_L	(EAX)
	MOV_L	(EAX,CONTENT(GLNAME(writeseg)))
	MOV_L 	(CONTENT(GLNAME(vgaSetReadWriteFunc)),EDX)
	CALL 	(CODEPTR(EDX))
	POP_L	(EDX)
	MOV_L	(REGOFF(4,ESP),EAX)
	SUB_L	(CONTENT(GLNAME(vgaSegmentSize)),EAX)
	RET

/*
 *-----------------------------------------------------------------------
 * vgaReadWritePrev ---
 *     switch to previous memory bank of the VGA board for read & write access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 *
 * pointer
 * vgaReadWritePrev(p)
 *      register pointer p;
 * {
 *   (vgaSetReadWriteFunc)(--writeseg); 
 *   return (p + vgaSegmentSize);
 * }
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaReadWritePrev)
GLNAME(vgaReadWritePrev):
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(writeseg)),EAX)
	DEC_L	(EAX)
	MOV_L	(EAX,CONTENT(GLNAME(writeseg)))
	MOV_L 	(CONTENT(GLNAME(vgaSetReadWriteFunc)),EDX)
	CALL 	(CODEPTR(EDX))
	POP_L	(EDX)
	MOV_L	(REGOFF(4,ESP),EAX)
	ADD_L	(CONTENT(GLNAME(vgaSegmentSize)),EAX)
	RET

/*
 *-----------------------------------------------------------------------
 * vgaSetRead ---
 *     select a memory bank of the VGA board for read access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 * pointer
 * vgaSetRead(p)
 *     register pointer p;
 * {
 * #ifdef XF86VGA16
 *   readseg = ((unsigned long)p - vgaBase) >> vgaSegmentShift;
 *   (vgaSetReadFunc)(readseg);
 *   return (vgaReadBottom + (((unsigned int)p - vgaBase) & vgaSegmentMask));
 * #else
 *   readseg = ((unsigned long)p - VGABASE) >> vgaSegmentShift;
 *   (vgaSetReadFunc)(readseg);
 *   return (vgaReadBottom + ((unsigned int)p & vgaSegmentMask));
 * #endif
 * }
 *
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaSetRead)
GLNAME(vgaSetRead):
	MOV_L	(REGOFF(4,ESP),EAX)
	PUSH_L  (ECX)
	PUSH_L	(EDX)
#ifdef XF86VGA16
	SUB_L	(CONTENT(GLNAME(vgaBase)),EAX)
#else
	SUB_L	(VGABASE,EAX)
#endif
	MOV_L	(CONTENT(GLNAME(vgaSegmentShift)),ECX)
	SHR_L	(CL,EAX)
	MOV_L	(EAX,CONTENT(GLNAME(readseg)))
	MOV_L	(CONTENT(GLNAME(vgaSetReadFunc)),EDX)
	CALL	(CODEPTR(EDX))
	POP_L	(EDX)
	POP_L	(ECX)
	MOV_L	(REGOFF(4,ESP),EAX)
#ifdef XF86VGA16
	SUB_L	(CONTENT(GLNAME(vgaBase)),EAX)
#endif
	AND_L	(CONTENT(GLNAME(vgaSegmentMask)),EAX)
	ADD_L	(CONTENT(GLNAME(vgaReadBottom)),EAX)
 	RET

/*
 *-----------------------------------------------------------------------
 * vgaReadNext ---
 *     switch to next memory bank of the VGA board for read access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 *
 *
 * pointer
 * vgaReadNext(p)
 *      register pointer p;
 * {
 *   (vgaSetReadFunc)(++readseg);
 *   return (p - vgaSegmentSize);
 * }
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaReadNext)
GLNAME(vgaReadNext):
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(readseg)),EAX)
	INC_L	(EAX)
	MOV_L	(EAX,CONTENT(GLNAME(readseg)))
	MOV_L 	(CONTENT(GLNAME(vgaSetReadFunc)),EDX)
	CALL 	(CODEPTR(EDX))
	POP_L	(EDX)
	MOV_L	(REGOFF(4,ESP),EAX)
	SUB_L	(CONTENT(GLNAME(vgaSegmentSize)),EAX)
	RET

/*
 *-----------------------------------------------------------------------
 * vgaReadPrev ---
 *     switch to previous memory bank of the VGA board for read access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 *
 * pointer
 * vgaReadPrev(p)
 *      register pointer p;
 * {
 *   (vgaSetReadFunc)(--readseg); 
 *   return (p + vgaSegmentSize);
 * }
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaReadPrev)
GLNAME(vgaReadPrev):
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(readseg)),EAX)
	DEC_L	(EAX)
	MOV_L	(EAX,CONTENT(GLNAME(readseg)))
	MOV_L 	(CONTENT(GLNAME(vgaSetReadFunc)),EDX)
	CALL 	(CODEPTR(EDX))
	POP_L	(EDX)
	MOV_L	(REGOFF(4,ESP),EAX)
	ADD_L	(CONTENT(GLNAME(vgaSegmentSize)),EAX)
	RET

/*
 *-----------------------------------------------------------------------
 * vgaSetWrite ---
 *     select a memory bank of the VGA board for write access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 * pointer
 * vgaSetWrite(p)
 *     register pointer p;
 * {
 * #ifdef XF86VGA16
 *   writeseg = ((unsigned long)p - vgaBase) >> vgaSegmentShift;
 *   (vgaSetWriteFunc)(writeseg);
 *   return (vgaWriteBottom + (((unsigned int)p - vgaBase) & vgaSegmentMask));
 * #else
 *   writeseg = ((unsigned long)p - VGABASE) >> vgaSegmentShift;
 *   (vgaSetWriteFunc)(writeseg);
 *   return (vgaWriteBottom + ((unsigned int)p & vgaSegmentMask));
 * #endif
 * }
 *
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaSetWrite)
GLNAME(vgaSetWrite):
	MOV_L	(REGOFF(4,ESP),EAX)
	PUSH_L  (ECX)
	PUSH_L	(EDX)
#ifdef XF86VGA16
	SUB_L	(CONTENT(GLNAME(vgaBase)),EAX)
#else
	SUB_L	(VGABASE,EAX)
#endif
	MOV_L	(CONTENT(GLNAME(vgaSegmentShift)),ECX)
	SHR_L	(CL,EAX)
	MOV_L	(EAX,CONTENT(GLNAME(writeseg)))
	MOV_L	(CONTENT(GLNAME(vgaSetWriteFunc)),EDX)
	CALL	(CODEPTR(EDX))
	POP_L	(EDX)
	POP_L	(ECX)
	MOV_L	(REGOFF(4,ESP),EAX)
#ifdef XF86VGA16
	SUB_L	(CONTENT(GLNAME(vgaBase)),EAX)
#endif
	AND_L	(CONTENT(GLNAME(vgaSegmentMask)),EAX)
	ADD_L	(CONTENT(GLNAME(vgaWriteBottom)),EAX)
 	RET

/*
 *-----------------------------------------------------------------------
 * vgaWriteNext ---
 *     switch to next memory bank of the VGA board for write access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 *
 *
 * pointer
 * vgaWriteNext(p)
 *      register pointer p;
 * {
 *   (vgaSetWriteFunc)(++writeseg);
 *   return (p - vgaSegmentSize);
 * }
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaWriteNext)
GLNAME(vgaWriteNext):
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(writeseg)),EAX)
	INC_L	(EAX)
	MOV_L	(EAX,CONTENT(GLNAME(writeseg)))
	MOV_L 	(CONTENT(GLNAME(vgaSetWriteFunc)),EDX)
	CALL 	(CODEPTR(EDX))
	POP_L	(EDX)
	MOV_L	(REGOFF(4,ESP),EAX)
	SUB_L	(CONTENT(GLNAME(vgaSegmentSize)),EAX)
	RET

/*
 *-----------------------------------------------------------------------
 * vgaWritePrev ---
 *     switch to previous memory bank of the VGA board for write access
 * Results:
 *      The adjusted pointer into the memory.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 *
 * pointer
 * vgaWritePrev(p)
 *      register pointer p;
 * {
 *   (vgaSetWriteFunc)(--writeseg); 
 *   return (p + vgaSegmentSize);
 * }
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaWritePrev)
GLNAME(vgaWritePrev):
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(writeseg)),EAX)
	DEC_L	(EAX)
	MOV_L	(EAX,CONTENT(GLNAME(writeseg)))
	MOV_L 	(CONTENT(GLNAME(vgaSetWriteFunc)),EDX)
	CALL 	(CODEPTR(EDX))
	POP_L	(EDX)
	MOV_L	(REGOFF(4,ESP),EAX)
	ADD_L	(CONTENT(GLNAME(vgaSegmentSize)),EAX)
	RET

/*
 *-----------------------------------------------------------------------
 * vgaSaveBank --
 *     save Banking-state
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 *
 * void
 * vgaSaveBank()
 * {
 *   saveseg = writeseg;
 * }
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaSaveBank)
GLNAME(vgaSaveBank):
	MOV_L	(CONTENT(GLNAME(writeseg)),EAX)
	MOV_L	(EAX,CONTENT(GLNAME(saveseg)))
	RET

/*
 *-----------------------------------------------------------------------
 * vgaRestoreBank --
 *     restore the banking after vgaSaveBank was called
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 *
 * void
 * vgaRestoreBank()
 * {
 *   (vgaSetWriteFunc)(saveseg);
 *   (vgaSetReadFunc)(saveseg);
 * }
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaRestoreBank)
GLNAME(vgaRestoreBank):
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(saveseg)),EAX)
	MOV_L	(EAX,CONTENT(GLNAME(writeseg)))
	MOV_L	(CONTENT(GLNAME(vgaSetWriteFunc)),EDX)
	CALL	(CODEPTR(EDX))
	MOV_L	(CONTENT(GLNAME(saveseg)),EAX)
	MOV_L	(EAX,CONTENT(GLNAME(readseg)))
	MOV_L	(CONTENT(GLNAME(vgaSetReadFunc)),EDX)
	CALL	(CODEPTR(EDX))
	POP_L	(EDX)
	RET


/*
 *-----------------------------------------------------------------------
 * vgaPushRead ---
 *     make the write-bank also readable. no acces to the former read bank !
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 *
 * void
 * vgaPushRead(p)
 * {
 *   (vgaSetReadWriteFunc)(writeseg);
 * }
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaPushRead)
GLNAME(vgaPushRead):
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(writeseg)),EAX)
	MOV_L	(CONTENT(GLNAME(vgaSetReadWriteFunc)),EDX)
	CALL	(CODEPTR(EDX))
	POP_L	(EDX)
	RET

/*
 *-----------------------------------------------------------------------
 * vgaPopRead ---
 *     restore the banking after vgaPushRead was called
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 *
 * void
 * vgaPopRead(p)
 * {
 *   (vgaSetWriteFunc)(writeseg);
 *   (vgaSetReadFunc)(readseg);
 * }
 */
	ALIGNTEXT4
	GLOBL	GLNAME(vgaPopRead)
GLNAME(vgaPopRead):
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(writeseg)),EAX)
	MOV_L	(CONTENT(GLNAME(vgaSetWriteFunc)),EDX)
	CALL	(CODEPTR(EDX))
	MOV_L	(CONTENT(GLNAME(readseg)),EAX)
	MOV_L	(CONTENT(GLNAME(vgaSetReadFunc)),EDX)
	CALL	(CODEPTR(EDX))
	POP_L	(EDX)
	RET

/*
 *-----------------------------------------------------------------------
 * vgaSetVidPage ---
 *     select a memory bank of the VGA board for read & write access
 * Results:
 *      None.
 *
 * Side Effects:
 *      None.
 *-----------------------------------------------------------------------
 * void
 * vgaSetVidPage(page)
 *     register int page;
 * {
 *   (vgaSetReadWriteFunc)(page);
 *   return;
 * }
 *
 */
	ALIGNTEXT4
	GLOBL GLNAME(vgaSetVidPage)
GLNAME(vgaSetVidPage):
	MOV_L	(REGOFF(4,ESP),EAX)
	PUSH_L	(EDX)
	MOV_L	(CONTENT(GLNAME(vgaSetReadWriteFunc)),EDX)
	CALL	(CODEPTR(EDX))
	POP_L	(EDX)
 	RET


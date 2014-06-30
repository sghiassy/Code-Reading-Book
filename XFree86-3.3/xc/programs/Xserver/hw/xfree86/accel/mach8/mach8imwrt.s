/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach8/mach8imwrt.s,v 3.1 1996/12/23 06:40:05 dawes Exp $ */






/* $XConsortium: mach8imwrt.s /main/4 1996/02/21 17:30:57 kaleb $ */
/******************************************************************************

This is a assembly language version of the mach8ImageWrite routine.
Except for the extended fifo status register, everything should be
8514 compatible.
Written by Hans Nasten ( nasten@everyware.se ) AUG 28, 1993.

void
mach8ImageWrite(x, y, w, h, psrc, pwidth, px, py, alu, planemask)
    int			x;
    int			y;
    int			w;
    int			h;
    unsigned char	*psrc;
    int			pwidth;
    int			px;
    int			py;
    short		alu;
    short		planemask;

******************************************************************************/

#define _MACH8_ASM_
#include "assyntax.h"
#include "regmach8.h"

	AS_BEGIN

/*
 * Defines for in arguments.
 */
#define x_arg		REGOFF(8,EBP)
#define y_arg		REGOFF(12,EBP)
#define w_arg		REGOFF(16,EBP)
#define h_arg		REGOFF(20,EBP)
#define psrc_arg	REGOFF(24,EBP)
#define pwidth_arg	REGOFF(28,EBP)
#define px_arg		REGOFF(32,EBP)
#define py_arg		REGOFF(36,EBP)
#define alu_arg		REGOFF(40,EBP)
#define planemask_arg	REGOFF(44,EBP)

#ifdef DRAM_VERSION
#define IMAGEWRITE	GLNAME(mach8ImageWriteDram)
#else
#define IMAGEWRITE	GLNAME(mach8ImageWrite)
#endif

	SEG_TEXT
	ALIGNTEXT4

GLOBL	IMAGEWRITE

IMAGEWRITE:
	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	PUSH_L	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)
/*
 * Check if height or width is 0.
 */
	MOV_L	(w_arg,EDI)
	MOV_L	(h_arg,EBX)
	OR_L	(EDI,EDI)
	JZ	(.finish)
	OR_L	(EBX,EBX)
	JZ	(.finish)
/*
 * Wait for 9 queue entries
 */
        MOV_L   (EXT_FIFO_STATUS,EDX)
.wait_queue_0:
        IN_W
        TEST_W  (CONST(0x80),AX)
        JNZ     (.wait_queue_0)
/*
 * Init 8514/Mach registers.
 */
	MOV_L	(FRGD_MIX,EDX)
	MOV_W	(alu_arg,AX)
	OR_W	(FSS_PCDATA,AX)
	OUT_W

	MOV_L	(WRT_MASK,EDX)
	MOV_W	(planemask_arg,AX)
	OUT_W

	MOV_L	(CUR_X,EDX)
	MOV_W	(x_arg,AX)
	OUT_W

	MOV_L	(CUR_Y,EDX)
	MOV_W	(y_arg,AX)
	OUT_W
/*
 * If the width is odd, program the 8514/Mach registers for width+1.
 */
	MOV_L	(MAJ_AXIS_PCNT,EDX)
	MOV_W	(w_arg,AX)
	TEST_W	(CONST(1),AX)
	JNZ	(.odd)

	DEC_W	(AX)
.odd:
	OUT_W

.cont1:
/*
 * Set height and scissors registers.
 * The scissors is used to clip the last unwanted pixel on
 * lines with a odd length.
 */
	MOV_L	(MULTIFUNC_CNTL,EDX)
	MOV_W	(h_arg,AX)
	DEC_W	(AX)
/*	OR_W	(MIN_AXIS_PCNT,AX)*/
	OUT_W
	MOV_W	(x_arg,AX)
	OR_W	(SCISSORS_L,AX)
	OUT_W
	MOV_W	(x_arg,AX)
	ADD_W	(w_arg,AX)
	DEC_W	(AX)
	OR_W	(SCISSORS_R,AX)
	OUT_W

/*
 * Give command to 8514/Mach.
 * The command is : CMD_RECT | INC_Y | INC_X | DRAW
 *                  | PCDATA | WRTDATA | _16BIT | BYTSEQ);
 */
	MOV_L	(CMD,EDX)
	MOV_W	(GP_WRITE_CMD,AX)
	OUT_W
/*
 * Calculate a pointer to the first pixel on the first line.
 */
	MOV_L	(pwidth_arg,EAX)
	MUL_L	(py_arg)
	ADD_L	(px_arg,EAX)
	MOV_L	(psrc_arg,EDI)
	ADD_L	(EAX,EDI)
/*
 * Wait until the fifo is empty.
 */
	MOV_L	(GP_STAT,EDX)
.wait_queue_1:
	IN_W
	TEST_W	(CONST(1),AX)
	JNZ	(.wait_queue_1)
/*
 * Copy the pixels line by line to the fifo.
 */
#ifndef DRAM_VERSION
	CLD
	MOV_L	(PIX_TRANS,EDX)
	MOV_W	(h_arg,BX)
	MOV_L	(pwidth_arg,EAX)
	MOV_L	(w_arg,EBP)
	INC_L	(EBP)
	SHR_L	(CONST(1),EBP)
.next_line:
	MOV_L	(EDI,ESI)
	MOV_L	(EBP,ECX)
	REP
	OUTS_W
	ADD_L	(EAX,EDI)
	DEC_W	(BX)
	JNZ	(.next_line)
#else
	CLD
	MOV_L	(w_arg,EBX)
	INC_L	(EBX)
	SHR_L	(CONST(1),EBX)
.next_line:
	MOV_L	(EDI,ESI)
	MOV_L	(EBX,ECX)

.next_line_2:
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_a:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_a)

	MOV_L	(PIX_TRANS,EDX)
	LODS_W
	OUT_W
	LOOP	(.next_line_2)

	ADD_L	(pwidth_arg,EDI)
	DEC_W	(h_arg)
	JNZ	(.next_line)
#endif
/*
 * Wait until room for 3 entries in the fifo.
 */
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_2:
	IN_W
	TEST_W	(CONST(0x2000),AX)
	JNZ	(.wait_queue_2)
/*
 * Reset FRGD_MIX to default.
 */
	MOV_L	(FRGD_MIX,EDX)
	MOV_W	(GP_DEF_FRGD_MIX,AX)
	OUT_W

/*
 * Reset the scissors regsiters.
 */
	MOV_L	(MULTIFUNC_CNTL,EDX)
	MOV_W	(SCISSORS_L,AX)
	OUT_W
	MOV_W	(SCISSORS_R,AX)
	OR_W	(MACH8_MAX_X,AX)
	OUT_W

.finish:
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	POP_L	(EBP)
	RET



/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/ibm8514/imwrite.s,v 3.1 1996/12/23 06:37:58 dawes Exp $ */





/* $XConsortium: imwrite.s /main/3 1996/02/21 17:25:00 kaleb $ */
/******************************************************************************

This is a assembly language version of the ibm8514ImageWrite routine.
Written by Hans Nasten ( nasten@everyware.se ) AUG 28, 1993.

void
ibm8514ImageWrite(x, y, w, h, psrc, pwidth, px, py, alu, planemask)
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

#define _8514_ASM_
#include "assyntax.h"
#include "reg8514.h"

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

	SEG_TEXT
	ALIGNTEXT4

GLOBL	GLNAME(ibm8514ImageWrite)

GLNAME(ibm8514ImageWrite):
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
 * Wait for 2 queue entries
 */
        MOV_L   (GP_STAT,EDX)
.wait_queue_0:
        IN_B
        TEST_B  (CONST(0x40),AL)
        JNZ     (.wait_queue_0)
/*
 * Init 8514 registers.
 */
	MOV_L	(FRGD_MIX,EDX)
	MOV_W	(alu_arg,AX)
	OR_W	(FSS_PCDATA,AX)
	OUT_W

	MOV_L	(WRT_MASK,EDX)
	MOV_W	(planemask_arg,AX)
	OUT_W
/*
 * Wait for 7 queue entries
 */
        MOV_L   (GP_STAT,EDX)
.wait_queue_1:
        IN_B
        TEST_B  (CONST(0x02),AL)
        JNZ     (.wait_queue_1)

	MOV_L	(CUR_X,EDX)
	MOV_W	(x_arg,AX)
	OUT_W

	MOV_L	(CUR_Y,EDX)
	MOV_W	(y_arg,AX)
	OUT_W
/*
 * If the width is odd, program the 8514 registers for width+1.
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
 * Give command to 8514.
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
.wait_queue_2:
	IN_B
	TEST_B	(CONST(1),AL)
	JNZ	(.wait_queue_2)
/*
 * Copy the pixels line by line to the fifo.
 */
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
/*
 * Wait until room for 3 entries in the fifo.
 */
	MOV_L	(GP_STAT,EDX)
.wait_queue_3:
	IN_B
	TEST_B	(CONST(0x20),AL)
	JNZ	(.wait_queue_3)

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
	OR_W	(CONST(1023),AX)
	OUT_W

.finish:
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	POP_L	(EBP)
	RET

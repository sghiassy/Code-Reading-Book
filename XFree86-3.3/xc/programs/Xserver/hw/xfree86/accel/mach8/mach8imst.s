/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach8/mach8imst.s,v 3.1 1996/12/23 06:40:04 dawes Exp $ */





/* $XConsortium: mach8imst.s /main/4 1996/02/21 17:30:54 kaleb $ */
/******************************************************************************

This is a assembly language version of the mach8ImageStipple routine.

Except for the extended fifo status register, everything should be
8514 compatible.
Written by Hans Nasten ( nasten@everyware.se ) SEP 16, 1993.

This is a assembly language version of the C code in mach8im.c

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
#define pw_arg		REGOFF(32,EBP)
#define ph_arg		REGOFF(36,EBP)
#define pox_arg		REGOFF(40,EBP)
#define poy_arg		REGOFF(44,EBP)
#define fgpix_arg	REGOFF(48,EBP)
#define bgpix_arg	REGOFF(52,EBP)
#define alu_arg		REGOFF(56,EBP)
#define planemask_arg	REGOFF(60,EBP)
#define opaque_arg	REGOFF(64,EBP)

/*
 * Defines for local variables.
 */
#define srcx_loc	REGOFF(-4,EBP)
#define srch_loc	REGOFF(-8,EBP)
#define dstw_loc	REGOFF(-12,EBP)

#ifdef DRAM_VERSION
#define IMAGESTIPPLE	GLNAME(mach8ImageStippleDram)
#else
#define IMAGESTIPPLE	GLNAME(mach8ImageStipple)
#endif

	SEG_TEXT
	ALIGNTEXT4

.mskbit:
	D_WORD	0x0000
	D_WORD	0x0001
	D_WORD	0x0003
	D_WORD	0x0007
	D_WORD	0x000f
	D_WORD	0x001f
	D_WORD	0x003f
	D_WORD	0x007f
	D_WORD	0x00ff
	D_WORD	0x01ff
	D_WORD	0x03ff
	D_WORD	0x07ff
	D_WORD	0x0fff
	D_WORD	0x1fff
	D_WORD	0x3fff
	D_WORD	0x7fff
	D_WORD	0xffff
	D_WORD	0x0000


GLOBL	IMAGESTIPPLE

IMAGESTIPPLE:
	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	SUB_L	(CONST(12),ESP)
	PUSH_L	(EDI)
	PUSH_L	(ESI)
	PUSH_L	(EBX)
/*
 * Check if alu = MIX_DST or height == 0 or width == 0.
 */
	MOV_L	(alu_arg,EAX)
	CMP_L	(MIX_DST,EAX)
	JZ	(.finish)
	MOV_L	(w_arg,EDI)
	OR_L	(EDI,EDI)
	JZ	(.finish)
	MOV_L	(h_arg,EBX)
	OR_L	(EBX,EBX)
	JZ	(.finish)
/*
 * Wait for 13 queue entries
 */
        MOV_L   (EXT_FIFO_STATUS,EDX)
.wait_queue_0:
        IN_W
        TEST_W  (CONST(0x08),AX)
        JNZ     (.wait_queue_0)
/*
 * Set scissors, pixcntl and height registers.
 */
	MOV_L	(MULTIFUNC_CNTL,EDX)
	MOV_W	(x_arg,AX)
	OR_W	(SCISSORS_L,AX)
	OUT_W
	MOV_W	(x_arg,AX)
	ADD_W	(w_arg,AX)
	DEC_W	(AX)
	OR_W	(SCISSORS_R,AX)
	OUT_W
	MOV_W	(PIXCNTL_STIP,AX)
	OUT_W
	MOV_W	(h_arg,AX)
	DEC_W	(AX)
/*	OR_W	(MIN_AXIS_PCNT,AX)*/
	OUT_W
/*
 * Init write mask and frgd mix.
 */
	MOV_L	(WRT_MASK,EDX)
	MOV_W	(planemask_arg,AX)
	OUT_W
	MOV_L	(FRGD_MIX,EDX)
	MOV_W	(alu_arg,AX)
	OR_W	(FSS_FRGDCOL,AX)
	OUT_W
/*
 * Init bkgd mix and bkgd colour according to opaque argument.
 */
	CMP_L	(CONST(0),opaque_arg)
	JZ	(.not_opaque)

	MOV_L	(BKGD_MIX,EDX)
	MOV_W	(alu_arg,AX)
/*	OR_W	(BSS_BKGDCOL,AX)*/
	OUT_W
	MOV_L	(BKGD_COLOR,EDX)
	MOV_W	(bgpix_arg,AX)
	OUT_W
	JMP	(.set_frgdcol)
.not_opaque:
	MOV_L	(BKGD_MIX,EDX)
	MOV_W	(MIX_DST,AX)
/*	OR_W	(BSS_BKGDCOL,AX)*/
	OUT_W
.set_frgdcol:
	MOV_L	(FRGD_COLOR,EDX)
	MOV_W	(fgpix_arg,AX)
	OUT_W
/*
 * Adjust width and x arguments to even nibbles.
 */
	MOV_L	(x_arg,EAX)
	AND_L	(CONST(3),EAX)
	ADD_L	(w_arg,EAX)
	MOV_L	(EAX,w_arg)
	MOV_L	(x_arg,EBX)
	AND_L	(CONST(0xfffffffc),EBX)
	MOV_L	(EBX,x_arg)
/*
 * Set major axis count, x and y registers.
 */
	MOV_L	(MAJ_AXIS_PCNT,EDX)
	ADD_L	(CONST(7),EAX)
	AND_L	(CONST(0xfffffff8),EAX)
	DEC_L	(EAX)
	OUT_W
	MOV_L	(CUR_X,EDX)
	MOV_W	(BX,AX)
	OUT_W
	MOV_L	(CUR_Y,EDX)
	MOV_W	(y_arg,AX)
	OUT_W
/*
 * Give command to 8514/Mach.
 * The command is : CMD_RECT | INC_Y | INC_X | DRAW
 *                  | PCDATA | WRTDATA | _16BIT | YMAJAXIS | PLANAR);
 */
	MOV_L	(CMD,EDX)
	MOV_W	(GP_STIP_CMD,AX)
	OUT_W
/*
 * Do the modulo trick for the x coordinate.
 */
	MOV_L	(x_arg,EAX)
	SUB_L	(pox_arg,EAX)
	CDQ
	IDIV_L	(pw_arg)
	TEST_L	(EDX,EDX)
	JGE	(.mod2)
	ADD_L	(pw_arg,EDX)
.mod2:
	MOV_L	(EDX,x_arg)
/*
 * Do the modulo trick for the y coordinate.
 */
	MOV_L	(y_arg,EAX)
	SUB_L	(poy_arg,EAX)
	CDQ
	IDIV_L	(ph_arg)
	TEST_L	(EDX,EDX)
	JGE	(.mod1)
	ADD_L	(ph_arg,EDX)
.mod1:
	MOV_L	(EDX,y_arg)
/*
 * Wait until the fifo is empty.
 */
	MOV_L	(GP_STAT,EDX)
.wait_queue_1:
	IN_W
	TEST_W	(CONST(1),AX)
	JNZ	(.wait_queue_1)
/*
 * When the source bitmap is properly aligned, max 16 pixels wide,
 * and nonrepeating use this faster loop instead.
 * This speeds up all copying to the font cache.
 */
	MOV_L	(x_arg,EAX)
	AND_L	(CONST(7),EAX)
	JNZ	(.next_bitmap_vertical)

	MOV_L	(w_arg,EAX)
	CMP_L	(CONST(16),EAX)
	JG	(.next_bitmap_vertical)	

	ADD_L	(x_arg,EAX)
	CMP_L	(pw_arg,EAX)
	JG	(.next_bitmap_vertical)	

	MOV_L	(y_arg,EAX)
	ADD_L	(h_arg,EAX)
	CMP_L	(ph_arg,EAX)
	JG	(.next_bitmap_vertical)	

	MOV_L	(y_arg,EAX)
	MUL_L	(pwidth_arg)
	MOV_L	(x_arg,EDX)
	SHR_L	(CONST(3),EDX)
	ADD_L	(EDX,EAX)
	MOV_L	(psrc_arg,ESI)
	ADD_L	(EAX,ESI)
#ifndef DRAM_VERSION
	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_L	(h_arg,EDI)
	CMP_L	(CONST(8),w_arg)
	JLE	(.fast_loop_8)
/*
 * This loop is for max 16 pixels wide bitmaps.
 */
.fast_loop_16:
	MOV_W	(REGIND(ESI),CX)
	XOR_L	(EBX,EBX)
	MOV_B	(CL,BL)
	SHL_L	(CONST(1),EBX)
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_a:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_a)

	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_W	(REGOFF(GLNAME(mach8stipple_tab),EBX),AX)
	OUT_W
	XOR_L	(EBX,EBX)
	MOV_B	(CH,BL)
	SHL_L	(CONST(1),EBX)
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_b:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_b)

	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_W	(REGOFF(GLNAME(mach8stipple_tab),EBX),AX)
	OUT_W
	ADD_L	(pwidth_arg,ESI)
	DEC_L	(EDI)
	JNZ	(.fast_loop_16)

	JMP	(.stipple_exit)
/*
 * This loop is for max 8 pixels wide bitmaps.
 */
.fast_loop_8:
	XOR_L	(EBX,EBX)
	MOV_B	(REGIND(ESI),BL)
	SHL_L	(CONST(1),EBX)
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_c:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_c)

	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_W	(REGOFF(GLNAME(mach8stipple_tab),EBX),AX)
	OUT_W
	ADD_L	(pwidth_arg,ESI)
	DEC_L	(EDI)
	JNZ	(.fast_loop_8)

	JMP	(.stipple_exit)

/*
 * Process all lines on screen repeating the bitmap if needed.
 * This loop paints from the present y location to the end of
 * the bitmap. ( or to the end of the screen area if the bitmap
 * is nonrepeating ).
 * Start by checking if the bitmap is high enough to completely
 * cover the screen area vertically.
 */
.next_bitmap_vertical:
	MOV_L	(y_arg,EAX)
	ADD_L	(h_arg,EAX)
	MOV_L	(ph_arg,EBX)
	CMP_L	(EBX,EAX)
	JLE	(.cmp_get_h)

	SUB_L	(y_arg,EBX)
	JMP	(.cmp2)
.cmp_get_h:
	MOV_L	(h_arg,EBX)
.cmp2:
	MOV_L	(EBX,srch_loc)
/*
 * Process one line of pixels in the bitmap,
 * repeating the bitmap horisontally if needed.
 */
.next_bitmap_line:
	MOV_L	(w_arg,EAX)
	MOV_L	(EAX,dstw_loc)
	MOV_L	(x_arg,ESI)
	MOV_L	(ESI,srcx_loc)
	MOV_L	(y_arg,EAX)
	MUL_L	(pwidth_arg)
	MOV_L	(psrc_arg,EDI)
	ADD_L	(EAX,EDI)
/*
 * Copy one line of the bitmap to the screen.
 * This loop paints one line of pixels from the bitmap onto the screen.
 */
.next_byte_horizontal:
	MOV_L	(ESI,ECX)
	MOV_L	(pw_arg,EBX)
	MOV_L	(EBX,EDX)
	SUB_L	(ESI,EBX)

	SHR_L	(CONST(3),ESI)
	ADD_L	(EDI,ESI)

	AND_B	(CONST(7),CL)

	CMP_L	(CONST(16),EBX)
	JL	(.less_than_16_left)
/*
 * In this case we have at least 16 pixels left in the bitmap.
 */
	MOV_L	(REGIND(ESI),EAX)
	SHR_L	(CL,EAX)

	MOV_L	(srcx_loc,ESI)
	ADD_L	(CONST(16),ESI)
	CMP_L	(EDX,ESI)
	JL	(.no_wrap2)

	SUB_L	(EDX,ESI)

.no_wrap2:
	MOV_L	(ESI,srcx_loc)
	MOV_L	(PIX_TRANS,EDX)
	MOV_W	(AX,CX)
	XOR_L	(EBX,EBX)
	MOV_B	(AL,BL)
	SHL_L	(CONST(1),EBX)
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_d:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_d)

	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_W	(REGOFF(GLNAME(mach8stipple_tab),EBX),AX)
	OUT_W
	CMP_L	(CONST(8),dstw_loc)
	JLE	(.chk_next_line)

	XOR_L	(EBX,EBX)
	MOV_B	(CH,BL)
	SHL_L	(CONST(1),EBX)
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_e:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_e)

	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_W	(REGOFF(GLNAME(mach8stipple_tab),EBX),AX)
	OUT_W
	SUB_L	(CONST(16),dstw_loc)
	JG	(.next_byte_horizontal)

	JMP	(.chk_next_line)
/*
 * Here we have less than 16 pixels left in the bitmap.
 */
.less_than_16_left:
	CMP_L	(CONST(16),EDX)
	JL	(.less_than_16_wide)

	MOV_L	(REGIND(ESI),EAX)
	SHR_L	(CL,EAX)
	MOV_B	(BL,CL)
	SHL_L	(CONST(1),EBX)
	AND_W	(REGOFF(.mskbit,EBX),AX)
	MOV_W	(REGIND(EDI),BX)
	SHL_W	(CL,BX)
	OR_W	(BX,AX)

	MOV_L	(srcx_loc,ESI)
	ADD_L	(CONST(16),ESI)
	CMP_L	(EDX,ESI)
	JL	(.no_wrap3)

	SUB_L	(EDX,ESI)

.no_wrap3:
	MOV_L	(ESI,srcx_loc)
	MOV_L	(PIX_TRANS,EDX)
	MOV_W	(AX,CX)
	XOR_L	(EBX,EBX)
	MOV_B	(AL,BL)
	SHL_L	(CONST(1),EBX)
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_f:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_f)

	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_W	(REGOFF(GLNAME(mach8stipple_tab),EBX),AX)
	OUT_W
	CMP_L	(CONST(8),dstw_loc)
	JLE	(.chk_next_line)

	XOR_L	(EBX,EBX)
	MOV_B	(CH,BL)
	SHL_L	(CONST(1),EBX)
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_g:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_g)

	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_W	(REGOFF(GLNAME(mach8stipple_tab),EBX),AX)
	OUT_W
	SUB_L	(CONST(16),dstw_loc)
	JG	(.next_byte_horizontal)

	JMP	(.chk_next_line)
/*
 * Here the entire pixmap is less than 16 pixels wide.
 */
.less_than_16_wide:
	CMP_L	(CONST(8),EDX)
	JL	(.less_than_8_wide)

	MOV_W	(REGIND(ESI),AX)
	SHR_W	(CL,AX)
	MOV_B	(BL,CL)
	SHL_L	(CONST(1),EBX)
	AND_W	(REGOFF(.mskbit,EBX),AX)
	MOV_W	(REGIND(EDI),BX)
	SHL_W	(CL,BX)
	OR_W	(BX,AX)
	MOV_W	(REGIND(ESI),BX)
	ADD_B	(DL,CL)
	SHL_W	(CL,BX)
	OR_W	(BX,AX)

	MOV_L	(srcx_loc,ESI)
	ADD_L	(CONST(16),ESI)
	CMP_L	(EDX,ESI)
	JL	(.no_wrap4)

	SUB_L	(EDX,ESI)

.no_wrap4:
	MOV_L	(ESI,srcx_loc)
	MOV_L	(PIX_TRANS,EDX)
	MOV_W	(AX,CX)
	XOR_L	(EBX,EBX)
	MOV_B	(AL,BL)
	SHL_L	(CONST(1),EBX)
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_h:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_h)

	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_W	(REGOFF(GLNAME(mach8stipple_tab),EBX),AX)
	OUT_W
	CMP_L	(CONST(8),dstw_loc)
	JLE	(.chk_next_line)

	XOR_L	(EBX,EBX)
	MOV_B	(CH,BL)
	SHL_L	(CONST(1),EBX)
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_i:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_i)

	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_W	(REGOFF(GLNAME(mach8stipple_tab),EBX),AX)
	OUT_W
	SUB_L	(CONST(16),dstw_loc)
	JG	(.next_byte_horizontal)

	JMP	(.chk_next_line)
/*
 * Here the entire pixmap is less than 8 pixels wide.
 */
.less_than_8_wide:
	MOV_B	(REGIND(EDI),AL)
	MOV_L	(EDX,ESI)
	MOV_B	(AL,DH)
	SHR_B	(CL,AL)
	MOV_L	(EBX,ECX)
	SHL_L	(CONST(1),EBX)
	AND_W	(REGOFF(.mskbit,EBX),AX)
	MOV_B	(DH,BL)

.thin_loop:
	CMP_L	(CONST(16),ECX)
	JGE	(.store_pix)

	CMP_L	(dstw_loc,ECX)
	JGE	(.store_pix)

	XOR_W	(DX,DX)
	MOV_B	(BL,DL)
	SHL_W	(CL,DX)
	OR_W	(DX,AX)
	ADD_L	(ESI,ECX)
	JMP	(.thin_loop)

.store_pix:
	MOV_L	(ESI,EDX)
	MOV_L	(srcx_loc,ESI)
	ADD_L	(CONST(16),ESI)
	CMP_L	(EDX,ESI)
	JL	(.no_wrap5)

	SUB_L	(EDX,ESI)

.no_wrap5:
	MOV_L	(ESI,srcx_loc)
	MOV_L	(PIX_TRANS,EDX)
	MOV_W	(AX,CX)
	XOR_L	(EBX,EBX)
	MOV_B	(AL,BL)
	SHL_L	(CONST(1),EBX)
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_j:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_j)

	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_W	(REGOFF(GLNAME(mach8stipple_tab),EBX),AX)
	OUT_W
	CMP_L	(CONST(8),dstw_loc)
	JLE	(.chk_next_line)

	XOR_L	(EBX,EBX)
	MOV_B	(CH,BL)
	SHL_L	(CONST(1),EBX)
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_k:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_k)

	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_W	(REGOFF(GLNAME(mach8stipple_tab),EBX),AX)
	OUT_W
	SUB_L	(CONST(16),dstw_loc)
	JG	(.next_byte_horizontal)

.chk_next_line:
	INC_L	(y_arg)
	DEC_L	(h_arg)
	DEC_L	(srch_loc)
	JNZ	(.next_bitmap_line)

	SUB_L	(EDX,EDX)
	MOV_L	(EDX,y_arg)
	CMP_L	(EDX,h_arg)
	JNZ	(.next_bitmap_vertical)

/*
 * Wait until room for 5 entries in the fifo.
 */
.stipple_exit:
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_2:
	IN_W
	TEST_W	(CONST(0x0800),AX)
	JNZ	(.wait_queue_2)
/*
 * Reset FRGD_MIX and BKGD_MIX to default.
 */
	MOV_L	(FRGD_MIX,EDX)
	MOV_W	(GP_DEF_FRGD_MIX,AX)
	OUT_W
	MOV_L	(BKGD_MIX,EDX)
	MOV_W	(GP_DEF_BKGD_MIX,AX)
	OUT_W
/*
 * Reset the scissors and pixcntl registers.
 */
	MOV_L	(MULTIFUNC_CNTL,EDX)
	MOV_W	(SCISSORS_L,AX)
	OUT_W
	MOV_W	(SCISSORS_R,AX)
	OR_W	(MACH8_MAX_X,AX)
	OUT_W
	MOV_W	(GP_DEF_PIXCNTL,AX)
	OUT_W

.finish:
	POP_L	(EBX)
	POP_L	(ESI)
	POP_L	(EDI)
	ADD_L	(CONST(12),ESP)
	POP_L	(EBP)
	RET

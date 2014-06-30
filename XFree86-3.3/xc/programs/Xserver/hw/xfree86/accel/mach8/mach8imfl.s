/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach8/mach8imfl.s,v 3.1 1996/12/23 06:40:00 dawes Exp $ */






/* $XConsortium: mach8imfl.s /main/4 1996/02/21 17:30:47 kaleb $ */
/******************************************************************************

This is a assembly language version of the mach8ImageFill routine.
It is renamed to enable a filter routine to catch the simplest
cases and dispatch them to the more efficent mach8Imagewrite.

Except for the extended fifo status register, everything should be
8514 compatible.
Written by Hans Nasten ( nasten@everyware.se ) AUG 29, 1993.

The equivalent C-code looks like this.

void
mach8RealImageFill(x, y, w, h, psrc, pwidth, pw, ph, pox, poy, alu, planemask)
    int			x;
    int			y;
    int			w;
    int			h;
    unsigned char	*psrc;
    int			pwidth;
    int			pw;
    int			ph;
    int			pox;
    int			poy;
    short		alu;
    short		planemask;
{
    int srcxsave, srcx, srcy, dstw, srcw, srch;
    int wtemp, count, i, j;
    unsigned short btemp, *p;


    WaitQueue(9);
    outw(FRGD_MIX, FSS_PCDATA | alu);
    outw(WRT_MASK, planemask);
    outw(CUR_X, (short)x);
    outw(CUR_Y, (short)y);
    if (w&1)
	outw(MAJ_AXIS_PCNT, (short)w);
    else
	outw(MAJ_AXIS_PCNT, (short)w-1);
    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | h-1);
    outw(MULTIFUNC_CNTL, SCISSORS_L | x);
    outw(MULTIFUNC_CNTL, SCISSORS_R | (x+w-1));
    outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW |
	      PCDATA | WRTDATA | _16BIT | BYTSEQ);
    WaitQueue(16);

    modulus(y-poy,ph,srcy);
    while( h > 0 ) {
	srch = ( srcy+h > ph ? ph - srcy : h );
	modulus(x-pox,pw,srcxsave);
	for( i = 0; i < srch; i++ ) {
	    dstw = w;
	    srcx = srcxsave;
	    srcw = ( srcx+w > pw ? pw - srcx : w );
	    wtemp = 0;
	    while( dstw > 0 ) {
		p = (unsigned short *)((unsigned char *)(psrc + pwidth * srcy + srcx));
		if( wtemp & 1 ) {
		    outw( PIX_TRANS, (btemp & 0x00ff) | (*p << 8 ) );
		    p = (unsigned short *)((unsigned char *)(p)++);
		    wtemp = srcw - 1;
		}
		else
		    wtemp = srcw;

		count = wtemp / 2;
		for( j = 0; j < count; j++ )
		    outw( PIX_TRANS, *p++ );

		dstw -= srcw;
		srcx = 0;
		if( wtemp & 1 ) {
		    if( dstw != 0 ) {
			btemp = *p;
		    }
		    else
			outw( PIX_TRANS, *p );
		}
		srcw = ( dstw < pw ? dstw : pw );
	    }
	    srcy++;
	    h--;
	}
	srcy = 0;
    }
    WaitQueue(3);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(MULTIFUNC_CNTL, SCISSORS_L);
    outw(MULTIFUNC_CNTL, SCISSORS_R | MACH8_MAX_X);
}

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
#define alu_arg		REGOFF(48,EBP)
#define planemask_arg	REGOFF(52,EBP)

/*
 * Defines for local variables.
 */
#define srcy_loc	REGOFF(-4,EBP)
#define srch_loc	REGOFF(-8,EBP)
#define srcx_loc	REGOFF(-12,EBP)
#define srcxsave_loc	REGOFF(-16,EBP)

#ifdef DRAM_VERSION
#define tempcount_loc	REGOFF(-20,EBP)
#define STACK_USG	CONST(20)
#define IMAGEFILL	GLNAME(mach8RealImageFillDram)
#else
#define tempcount_loc	EAX
#define STACK_USG	CONST(16)
#define IMAGEFILL	GLNAME(mach8RealImageFill)
#endif


	SEG_TEXT
	ALIGNTEXT4
GLOBL	IMAGEFILL

IMAGEFILL:
	PUSH_L	(EBP)
	MOV_L	(ESP,EBP)
	SUB_L	(STACK_USG,ESP)
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
 * Do the modulo trick for the y coordinate.
 * This is stolen from the gcc 2.4.5 output.
 */
	MOV_L	(y_arg,EAX)
	SUB_L	(poy_arg,EAX)
	CDQ
	IDIV_L	(ph_arg)
	TEST_L	(EDX,EDX)
	JGE	(.mod1)
	ADD_L	(ph_arg,EDX)
.mod1:
	MOV_L	(EDX,srcy_loc)
/*
 * Wait until the fifo is empty.
 */
	MOV_L	(GP_STAT,EDX)
.wait_queue_1:
	IN_W
	TEST_W	(CONST(1),AX)
	JNZ	(.wait_queue_1)

	CLD
/*
 * Process all lines on screen repeating the pixmap if needed.
 * This loop paints from the present y location to the end of
 * the pixmap. ( or to the end of the screen area if the pixmap
 * is nonrepeating ).
 * Start by checking if the pixmap is high enough to completely
 * cover the screen area vertically.
 */
.next_pixmap_vertical:
	MOV_L	(srcy_loc,EAX)
	ADD_L	(h_arg,EAX)
	MOV_L	(ph_arg,EBX)
	CMP_L	(EBX,EAX)
	JLE	(.cmp_get_h)

	SUB_L	(srcy_loc,EBX)
	JMP	(.cmp2)
.cmp_get_h:
	MOV_L	(h_arg,EBX)
.cmp2:
	MOV_L	(EBX,srch_loc)
	SUB_L	(EBX,h_arg)
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
	MOV_L	(EDX,srcxsave_loc)
/*
 * Process one line of pixels in the pixmap,
 * repeating the pixmap horisontally if needed.
 * Start by checking if the pixmap is wide enough to completely
 * cover the screen area horisontally.
 */
.next_pixmap_line:
	MOV_L	(w_arg,EBX)
	MOV_L	(srcxsave_loc,EAX)
	ADD_L	(EBX,EAX)
	MOV_L	(pw_arg,EDI)
	CMP_L	(EDI,EAX)
	JLE	(.cmp_get_w)

	SUB_L	(srcxsave_loc,EDI)
	JMP	(.cmp4)

.cmp_get_w:
	MOV_L	(EBX,EDI)
.cmp4:
	SUB_L	(ECX,ECX)
/*
 * Calculate and save a pointer to the first pixel on this line in
 * the pixmap. Add the x offset for the first lap and then skip the
 * first part of the loop.
 * ( all this just to avoid one multiplication and two add's in the loop ).
 */
	MOV_L	(pwidth_arg,EAX)
	MUL_L	(srcy_loc)
	ADD_L	(psrc_arg,EAX)
	MOV_L	(EAX,srcx_loc)
	MOV_L	(srcxsave_loc,ESI)
	ADD_L	(EAX,ESI)
	JMP	(.previously_even)
/*
 * Copy one line of the pixmap to the screen.
 * This loop paints one line of pixels from the pixmap onto the screen.
 */
.next_pixmap_horizontal:
	MOV_L	(srcx_loc,ESI)
/*
 * Is there a pixel leftover from the previous lap in the loop ?
 * ( since CH is used to store the saved pixel, only CL is in scope ).
 */
	TEST_B	(CONST(1),CL)
	JZ	(.previously_even)
/*
 * Yes there was, combine it with the first pixel and write them
 * to the 8514/Mach engine.
 */
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_a:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_a)
#endif
	MOV_B	(CH,AL)
	MOV_B	(REGIND(ESI),AH)
	MOV_L	(PIX_TRANS,EDX)
	OUT_W
	INC_L	(ESI)
	MOV_L	(EDI,ECX)
	DEC_L	(ECX)
	JMP	(.move_pixels)

.previously_even:
	MOV_L	(EDI,ECX)
	MOV_L	(PIX_TRANS,EDX)
/*
 * Move the rest of the line using a "rep outsw" instruction.
 */
.move_pixels:
	MOV_L	(ECX,tempcount_loc)
	SHR_L	(CONST(1),ECX)

#ifndef DRAM_VERSION
	REP
	OUTS_W
#else
	AND_L	(ECX,ECX)
	JZ	(.move_pixels_3)

.move_pixels_2:
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_b:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_b)

	MOV_L 	(PIX_TRANS,EDX)
	LODS_W
	OUT_W
	LOOP	(.move_pixels_2)
#endif
.move_pixels_3:
	MOV_L	(tempcount_loc,ECX)
	SUB_L	(EDI,EBX)
/*
 * Is there a pixel left unwritten ?
 */
	TEST_W	(CONST(1),CX)
	JZ	(.all_written)
/*
 * Yes, check if this is the last ( or only ) repetition of the pixmap.
 */
	AND_L	(EBX,EBX)
	JZ	(.write_pixel)
/*
 * No, there is more to come. Save this pixel for later.
 */
	MOV_B	(REGIND(ESI),CH)
	JMP	(.all_written)
/*
 * Yes, this is the last pixel displayed on this line.
 * Write it and let the scissors cut the unwanted extra pixel.
 */
.write_pixel:
#ifdef DRAM_VERSION
	MOV_L	(EXT_FIFO_STATUS,EDX)
.wait_queue_c:
	IN_W
	TEST_W	(CONST(0x8000),AX)
	JNZ	(.wait_queue_c)
	MOV_L	(PIX_TRANS,EDX)
#endif
	MOV_B	(REGIND(ESI),AL)
	OUT_W
/*
 * Check if the next repetition of the pixmap is using the entire
 * pixmap width.
 */
.all_written:
	MOV_L	(pw_arg,EAX)
	CMP_L	(EAX,EBX)
	JG	(.cmp6)

	MOV_L	(EBX,EAX)
.cmp6:
	MOV_L	(EAX,EDI)
	AND_L	(EBX,EBX)
	JNZ	(.next_pixmap_horizontal)

	INC_L	(srcy_loc)
	DEC_L	(srch_loc)
	JNZ	(.next_pixmap_line)

	SUB_L	(EDX,EDX)
	MOV_L	(EDX,srcy_loc)
	CMP_L	(EDX,h_arg)
	JNZ	(.next_pixmap_vertical)

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
	ADD_L	(STACK_USG,ESP)
	POP_L	(EBP)
	RET

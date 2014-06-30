/* $XConsortium: tga_regs.h /main/4 1996/10/27 18:07:29 kaleb $ */
/*
 * Copyright 1995,96 by Alan Hourihane, Wigan, England.
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
 * Author:  Alan Hourihane, <alanh@fairlite.demon.co.uk>
 */
/* TGA hardware description (minimal)
 *
 * Offsets within Memory Space
 *
 * Portions taken from linux's own tga driver...
 * Courtesy of Jay Estabrook.
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/tga/tga_regs.h,v 3.4 1997/01/05 11:54:33 dawes Exp $ */

#ifndef TGA_REGS_H
#define TGA_REGS_H

#define TGA_PATCHLEVEL			"0"

#define TYPE_TGA_8PLANE			0
#define TYPE_TGA_24PLANE		1
#define TYPE_TGA_24PLUSZ		3

#define TGA_WRITE_REG(v,r) \
	{ *(unsigned int *)((char*)(tga_reg_base)+(r)) = v; mb(); }

#define TGA_READ_REG(r) \
	( *(unsigned int *)((char*)(tga_reg_base)+(r)))

#define BT485_WRITE(v,r) \
	TGA_WRITE_REG((r),TGA_RAMDAC_SETUP_REG);		\
	TGA_WRITE_REG(((v)&0xff)|((r)<<8),TGA_RAMDAC_REG);

#define mb() \
	__asm__ __volatile__("mb": : :"memory")

#define	TGA_ROM_OFFSET			0x0000000
#define	TGA_REGS_OFFSET			0x0100000
#define	TGA_8PLANE_FB_OFFSET		0x0200000
#define	TGA_24PLANE_FB_OFFSET		0x0804000
#define	TGA_24PLUSZ_FB_OFFSET		0x1004000

#define TGA_FOREGROUND_REG		0x0020
#define TGA_BACKGROUND_REG		0x0024
#define	TGA_PLANEMASK_REG		0x0028
#define	TGA_MODE_REG			0x0030
#define		SIMPLE			0x00
#define		Z3D			0x10
#define		OPAQUESTIPPLE		0x01
#define		FILL			0x20
#define		TRANSPARENTSTIPPLE	0x05
#define		BLOCKSTIPPLE		0x0D
#define		BLOCKFILL		0x2D
#define		OPAQUELINE		0x02
#define		TRANSPARENTLINE		0x06
#define		BPP8PACKED		(0x00 << 8)
#define		BPP8UNPACK		(0x01 << 8)
#define		BPP12LOW		(0x02 << 8)
#define		BPP12HIGH		(0x06 << 8)
#define		BPP24			(0x03 << 8)
#define		CAP_ENDS		0x8000
#define		X11			0x0000
#define		WIN32			0x2000
#define	TGA_RASTEROP_REG		0x0034
#define TGA_ADDRESS_REG			0x003c
#define	TGA_DEEP_REG			0x0050
#define	TGA_PIXELMASK_REG		0x005c
#define	TGA_CURSOR_BASE_REG		0x0060
#define	TGA_HORIZ_REG			0x0064
#define	TGA_VERT_REG			0x0068
#define	TGA_BASE_ADDR_REG		0x006c
#define	TGA_VALID_REG			0x0070
#define	TGA_CURSOR_XY_REG		0x0074
#define	TGA_INTR_STAT_REG		0x007c
#define TGA_SPAN_REG			0x00bc
#define	TGA_RAMDAC_SETUP_REG		0x00c0
#define	TGA_BLOCK_COLOR0_REG		0x0140
#define	TGA_BLOCK_COLOR1_REG		0x0144
#define	TGA_CLOCK_REG			0x01e8
#define	TGA_RAMDAC_REG			0x01f0
#define	TGA_CMD_STAT_REG		0x01f8

/*
 * useful defines for managing the BT485 on the 8-plane TGA
 */
#define	BT485_READ_BIT			0x01
#define	BT485_WRITE_BIT			0x00

#define	BT485_ADDR_PAL_WRITE		0x00
#define	BT485_DATA_PAL			0x02
#define	BT485_PIXEL_MASK		0x04
#define	BT485_ADDR_PAL_READ		0x06
#define	BT485_ADDR_CUR_WRITE		0x08
#define	BT485_DATA_CUR			0x0a
#define	BT485_CMD_0			0x0c
#define	BT485_ADDR_CUR_READ		0x0e
#define	BT485_CMD_1			0x10
#define	BT485_CMD_2			0x12
#define	BT485_STATUS			0x14
#define	BT485_CMD_3			0x14
#define	BT485_CUR_RAM			0x16
#define	BT485_CUR_LOW_X			0x18
#define	BT485_CUR_HIGH_X		0x1a
#define	BT485_CUR_LOW_Y			0x1c
#define	BT485_CUR_HIGH_Y		0x1e

/*
 * useful defines for managing the BT463 on the 24-plane TGAs
 */
#define	BT463_ADDR_LO		0x0
#define	BT463_ADDR_HI		0x1
#define	BT463_REG_ACC		0x2
#define	BT463_PALETTE		0x3

#define	BT463_CUR_CLR_0		0x0100
#define	BT463_CUR_CLR_1		0x0101

#define	BT463_CMD_REG_0		0x0201
#define	BT463_CMD_REG_1		0x0202
#define	BT463_CMD_REG_2		0x0203

#define	BT463_READ_MASK_0	0x0205
#define	BT463_READ_MASK_1	0x0206
#define	BT463_READ_MASK_2	0x0207
#define	BT463_READ_MASK_3	0x0208

#define	BT463_BLINK_MASK_0	0x0209
#define	BT463_BLINK_MASK_1	0x020a
#define	BT463_BLINK_MASK_2	0x020b
#define	BT463_BLINK_MASK_3	0x020c

#define	BT463_WINDOW_TYPE_BASE	0x0300

/* Raster Operations */

#define MIX_0			0x00
#define MIX_AND			0x01
#define MIX_SRC_AND_NOT_DST	0x02
#define MIX_SRC			0x03
#define MIX_NOT_SRC_AND_DST	0x04
#define MIX_DST			0x05
#define MIX_XOR			0x06
#define MIX_OR			0x07
#define MIX_NOR			0x08
#define MIX_XNOR		0x09
#define MIX_NOT_DST		0x0A
#define MIX_SRC_OR_NOT_DST	0x0B
#define MIX_NOT_SRC		0x0C
#define MIX_NOT_SRC_OR_DST	0x0D
#define MIX_NAND		0x0E
#define MIX_1			0x0F

typedef struct {
	unsigned char r, g, b;
} LUTENTRY;

typedef struct {
	unsigned int h_active, h_fporch, h_sync, h_bporch;
	unsigned int v_active, v_fporch, v_sync, v_bporch;
	unsigned int h_pol, v_pol; 
	unsigned int clock_sel;
} tgaCRTCRegRec, *tgaCRTCRegPtr;

#endif

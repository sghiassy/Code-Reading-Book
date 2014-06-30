/* $XConsortium: tgui_ger.h /main/3 1996/02/21 18:08:19 kaleb $ */
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
/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/tvga8900/tgui_ger.h,v 3.9.2.1 1997/05/03 09:50:02 dawes Exp $ */

/* Graphics Engine for 9420/9430 */

#define GER_INDEX	0x210A
#define GER_BYTE0	0x210C
#define GER_BYTE1	0x210D
#define GER_BYTE2	0x210E
#define GER_BYTE3	0x210F
#define MMIOBASE	0x7C
#define OLDGER_STATUS	0x90
#define OLDGER_MWIDTH	0xB8
#define OLDGER_MFORMAT	0xBC
#define OLDGER_STYLE	0xC4
#define OLDGER_FMIX	0xC8
#define OLDGER_BMIX	0xC8
#define OLDGER_FCOLOUR	0xD8
#define OLDGER_BCOLOUR	0xDC
#define OLDGER_DIMXY	0xE0
#define OLDGER_DESTLINEAR	0xE4
#define OLDGER_DESTXY	0xF8
#define OLDGER_COMMAND	0xFC
#define		OLDGE_FILL	0x000A0000	/* Area Fill */

/* Graphics Engine for 9440/9660/9680 */

#define GER_STATUS	0x20		
#define		GE_BUSY	0x80
#define GER_OPERMODE	0x22		/* Byte for 9440, Word for 96xx */
#define GER_COMMAND	0x24		
#define		GE_NOP		0x00	/* No Operation */
#define		GE_BLT		0x01	/* BitBLT ROP3 only */
#define		GE_BLT_ROP4	0x02	/* BitBLT ROP4 (96xx only) */
#define		GE_SCANLINE	0x03	/* Scan Line */
#define		GE_BRESLINE	0x04	/* Bresenham Line */
#define		GE_SHVECTOR	0x05	/* Short Vector */
#define		GE_FASTLINE	0x06	/* Fast Line (96xx only) */
#define		GE_TRAPEZ	0x07	/* Trapezoidal fill (96xx only) */
#define		GE_ELLIPSE	0x08	/* Ellipse (96xx only) (RES) */
#define		GE_ELLIP_FILL	0x09	/* Ellipse Fill (96xx only) (RES)*/
#define	GER_FMIX	0x27
#define GER_DRAWFLAG	0x28		/* long */
#define		STENCIL		0x8000
#define		SOLIDFILL	0x4000
#define		TRANS_ENABLE	0x1000
#define 	TRANS_REVERSE	0x2000
#define		YMAJ		0x0400
#define		XNEG		0x0200
#define		YNEG		0x0100
#define		SRCMONO		0x0040
#define		PATMONO		0x0020
#define		SCR2SCR		0x0004
#define		PAT2SCR		0x0002
#define GER_FCOLOUR	0x2C		/* Word for 9440, long for 96xx */
#define GER_BCOLOUR	0x30		/* Word for 9440, long for 96xx */
#define GER_PATLOC	0x34		/* Word */
#define GER_DEST_XY	0x38
#define GER_DEST_X	0x38		/* Word */
#define GER_DEST_Y	0x3A		/* Word */
#define GER_SRC_XY	0x3C
#define GER_SRC_X	0x3C		/* Word */
#define GER_SRC_Y	0x3E		/* Word */
#define GER_DIM_XY	0x40
#define GER_DIM_X	0x40		/* Word */
#define GER_DIM_Y	0x42		/* Word */
#define GER_PATTERN	0x80		/* from 0x2180 to 0x21FF */

/* Additional - Graphics Engine for 96xx */
#define GER_SRCCLIP_XY	0x48
#define GER_SRCCLIP_X	0x48		/* Word */
#define GER_SRCCLIP_Y	0x4A		/* Word */
#define GER_DSTCLIP_XY	0x4C
#define GER_DSTCLIP_X	0x4C		/* Word */
#define GER_DSTCLIP_Y	0x4E		/* Word */

/* ROPS */
#define TGUIROP_0		0x00		/* 0 */
#define TGUIROP_1		0xFF		/* 1 */
#define TGUIROP_SRC		0xCC		/* S */
#define TGUIROP_DST		0xAA		/* D */
#define TGUIROP_NOT_SRC		0x33		/* Sn */
#define TGUIROP_NOT_DST		0x55		/* Dn */
#define TGUIROP_AND		0x88		/* DSa */
#define TGUIROP_SRC_AND_NOT_DST	0x44		/* SDna */
#define TGUIROP_NOT_SRC_AND_DST 0x22		/* DSna */
#define TGUIROP_NAND		0x77		/* DSan */
#define TGUIROP_OR		0xEE		/* DSo */
#define TGUIROP_SRC_OR_NOT_DST	0xDD		/* SDno */
#define TGUIROP_NOT_SRC_OR_DST	0xBB		/* DSno */
#define TGUIROP_NOR		0x11		/* DSon */
#define TGUIROP_XOR		0x66		/* DSx */
#define TGUIROP_XNOR		0x99		/* SDxn */

#define TGUIROP_PAT		0xF0		/* P */
#define TGUIROP_NOT_PAT		0x0F		/* Pn */
#define TGUIROP_AND_PAT		0xA0		/* DPa */
#define TGUIROP_PAT_AND_NOT_DST	0x50		/* PDna */
#define TGUIROP_NOT_PAT_AND_DST 0x0A		/* DPna */
#define TGUIROP_NAND_PAT	0x5F		/* DPan */
#define TGUIROP_OR_PAT		0xFA		/* DPo */
#define TGUIROP_PAT_OR_NOT_DST	0xF5		/* PDno */
#define TGUIROP_NOT_PAT_OR_DST	0xAF		/* DPno */
#define TGUIROP_NOR_PAT		0x05		/* DPon */
#define TGUIROP_XOR_PAT		0x5A		/* DPx */
#define TGUIROP_XNOR_PAT	0xA5		/* PDxn */

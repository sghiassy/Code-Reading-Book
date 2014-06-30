/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000Bt485.h,v 3.4 1996/12/23 06:40:33 dawes Exp $ */
/*
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: p9000Bt485.h /main/4 1996/02/21 17:31:59 kaleb $ */

#include "compiler.h"
#include <X11/Xfuncproto.h>
#include "p9000reg.h"

/* bt485 Registers */
#define BT_PIXEL_MASK   	0x03C6
#define BT_READ_ADDR    	0x03C7
#define BT_WRITE_ADDR   	0x03C8
#define BT_RAMDAC_DATA  	0x03C9
#define BT_COMMAND_REG_0	0x43C6
#define BT_CURS_RD_ADDR 	0x43C7
#define BT_CURS_WR_ADDR 	0x43C8
#define BT_CURS_DATA    	0x43C9
#define BT_STATUS_REG   	0x83C6
#define BT_COMMAND_REG_3	0x83C6
#define BT_CURS_RAM_DATA	0x83C7
#define BT_COMMAND_REG_1	0x83C8
#define BT_COMMAND_REG_2	0x83C9
#define BT_CURS_Y_LOW   	0xC3C6
#define BT_CURS_Y_HIGH  	0xC3C7
#define BT_CURS_X_LOW   	0xC3C8
#define BT_CURS_X_HIGH  	0xC3C9

/* bt485 Command Register 0 Bits */
#define BT_CR0_POWERDOWN        0x01  /* Powerdown the RAMDAC */
#define BT_CR0_8BIT             0x02  /* 8 bit operation as opposed to 6 bit */
#define BT_CR0_SYNC_R           0x04  /* Sync on Red */
#define BT_CR0_SYNC_G           0x08  /* Sync on Green */
#define BT_CR0_SYNC_B           0x10  /* Sync on Blue */
#define BT_CR0_ENABLE_CR3       0x80

/* bt485 Command Register 1 Bits */
#define BT_CR1_BP24             0x00  /* 24 bits/pixel */
#define BT_CR1_BP16             0x20  /* 16 bits/pixel */
#define BT_CR1_BP8              0x40  /* 8 bits/pixel */
#define BT_CR1_BP4              0x60  /* 4 bits/pixel */
#define BT_CR1_BYPASS_PAL       0x10  /* TrueColor palette bypass enable */
#define BT_CR1_565RGB           0x08  /* Use 5:6:5 for the 16 bit color */
#define BT_CR1_555RGB           0x00  /* Use 5:5:5 for the 16 bit color */
#define BT_CR1_16B_11MUX        0x04  /* Use 1:1 mux for 16 bit color */
#define BT_CR1_16B_21MUX        0x00  /* Use 2:1 mux for 16 bit color */

/* bt485 Command Register 2 Bits */
#define BT_CR2_PORTSEL_NONMASK  0x20
#define BT_CR2_PCLK1            0x10  /* CLKSEL - select PCLK1 */
#define BT_CR2_PCLK0            0x00  /* CLKSEL - select PCLK0 */
#define BT_CR2_INTERLACED       0x08  /* Display mode is interlaced */
#define BT_CR2_CURSOR_DISABLE   0x00  /* Disabled cursor */
#define BT_CR2_CURSOR_ENABLE    0x03  /* Two color X Windows cursor */

/* bt485 Command Register 3 Bits */
#define BT_CR3_2X_CLOCKMUL      0x08  /* 2x Clock Multiplier */
#define BT_CR3_1X_CLOCKMUL      0x00  /* 1x Clock Multiplier */
#define BT_CR3_64SQ_CURSOR      0x04  /* 64x64x2 cursor */
#define BT_CR3_32SQ_CURSOR      0x00  /* 32x32x2 cursor */


_XFUNCPROTOBEGIN

extern void p9000OutBtReg(
#if NeedFunctionPrototypes
	unsigned short,
	unsigned char,
	unsigned char
#endif
);

extern unsigned char p9000InBtReg(
#if NeedFunctionPrototypes
	unsigned short
#endif
);

extern void p9000OutBtRegCom3(
#if NeedFunctionPrototypes
	unsigned char,
	unsigned char
#endif
);

extern unsigned char p9000InBtRegCom3(
#if NeedFunctionPrototype
	void
#endif
);

extern unsigned char p9000InBtStatReg(
#if NeedFunctionPrototype
	void
#endif
);

extern void p9000BtEnable(
#if NeedFunctionPrototype
	p9000CRTCRegPtr
#endif
);

extern void p9000BtRestore(
#if NeedFunctionPrototype
	void
#endif
);

_XFUNCPROTOEND





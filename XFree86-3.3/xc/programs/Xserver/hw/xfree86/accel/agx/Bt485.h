/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/Bt485.h,v 3.5 1996/12/23 06:32:21 dawes Exp $ */
/*
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 * Copyright 1994 by Henry A. Worth  <haw30@eng.amdahl.com>
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
 * DAVID WEXELBLAT AND HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE 
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR 
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the AGX by Henry A. Worth
 *
 */
/* $XConsortium: Bt485.h /main/4 1996/02/21 17:15:46 kaleb $ */

#include "compiler.h"
#include <X11/Xfuncproto.h>

/* Directly Accessable RamDac Registers */

#define BT485_WRITE_ADDR		0x00
#define BT485_RAMDAC_DATA		0x01	
#define BT485_PIXEL_MASK		0x02
#define BT485_READ_ADDR			0x03
#define BT485_CURS_WR_ADDR		0x04
#define BT485_CURS_DATA			0x05
#define BT485_COMMAND_REG_0		0x06
#define BT485_CURS_RD_ADDR		0x07
#define BT485_COMMAND_REG_1		0x08
#define BT485_COMMAND_REG_2		0x09
#define BT485_STATUS_REG		0x0A
#define BT485_CURS_RAM_DATA		0x0B
#define BT485_CURS_X_LOW		0x0C
#define BT485_CURS_X_HIGH		0x0D
#define BT485_CURS_Y_LOW		0x0E
#define BT485_CURS_Y_HIGH		0x0F

/* Indirectly Accessable RamDac Registers */

#define BT485_COMMAND_REG_3		0x01

/* Command Register 0 */
#define BT485_CR0_EXTENDED_REG_ACCESS   0x80
#define BT485_CR0_SCLK_SLEEP_DISABLE    0x40
#define BT485_CR0_BLANK_PEDESTAL        0x20
#define BT485_CR0_SYNC_ON_BLUE          0x10
#define BT485_CR0_SYNC_ON_GREEN         0x08
#define BT485_CR0_SYNC_ON_RED           0x04
#define BT485_CR0_8_BIT_DAC             0x02
#define BT485_CR0_SLEEP_ENABLE          0x01


/* Command Register 1 */
#define BT485_CR1_24BPP                 0x00
#define BT485_CR1_16BPP			0x20
#define BT485_CR1_8BPP			0x40
#define BT485_CR1_4BPP			0x60
#define BT485_CR1_1BPP			0x80
#define BT485_CR1_BYPASS_CLUT		0x10
#define BT485_CR1_565_16BPP		0x08
#define BT485_CR1_555_16BPP		0x00
#define BT485_CR1_1_TO_1_16BPP		0x04
#define BT485_CR1_2_TO_1_16BPP		0x00
#define BT485_CR1_PD7_PIXEL_SWITCH	0x02
#define BT485_CR1_PIXEL_PORT_CD		0x01
#define BT485_CR1_PIXEL_PORT_AB 	0x00

/* Command Register 2 */
#define BT485_CR2_SCLK_DISABLE		0x80
#define BT485_TEST_PATH_SELECT		0x40
#define BT485_PIXEL_INPUT_GATE		0x20
#define BT485_PIXEL_CLK_SELECT		0x10
#define BT485_INTERLACE_SELECT		0x08
#define BT485_16BPP_CLUT_PACKED         0x04
#define BT485_X_WINDOW_CURSOR		0x03
#define BT485_2_COLOR_CURSOR		0x02
#define BT485_3_COLOR_CURSOR		0x01
#define BT485_DISABLE_CURSOR		0x00

/* Command Register 3 */
#define BT485_4BPP_NIBBLE_SWAP		0x10
#define BT485_CLOCK_DOUBLER		0x08
#define BT485_64_BY_64_CURSOR		0x04
#define BT485_32_BY_32_CURSOR		0x00

_XFUNCPROTOBEGIN

extern void xf86OutBt485IndReg(
#if NeedFunctionPrototypes
	unsigned char,
	unsigned char,
	unsigned char
#endif
);

extern unsigned char xf86InBt485IndReg(
#if NeedFunctionPrototypes
	unsigned char
#endif
);

extern unsigned char xf86InBt485StatReg(
#if NeedFunctionPrototypes
	void
#endif
);

extern void xf86Bt485HWSave(
#if NeedFunctionPrototypes
        union xf86RamDacSave *
#endif
);

extern void xf86Bt485HWRestore(
#if NeedFunctionPrototypes
        union xf86RamDacSave *
#endif
);

extern void xf86Bt485Init(
#if NeedFunctionPrototypes
        void
#endif
);

_XFUNCPROTOEND

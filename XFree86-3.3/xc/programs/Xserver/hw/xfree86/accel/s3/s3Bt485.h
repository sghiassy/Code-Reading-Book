/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3Bt485.h,v 3.2 1997/01/08 20:33:39 dawes Exp $ */
/*
 * Copyright 1993 by David Wexelblat <dwex@XFree86.org>
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
/* $XConsortium: s3Bt485.h /main/3 1996/02/21 17:34:23 kaleb $ */

#include "compiler.h"
#include <X11/Xfuncproto.h>

#define BT_WRITE_ADDR		0x00
#define BT_RAMDAC_DATA		0x01	
#define BT_PIXEL_MASK		0x02
#define BT_READ_ADDR		0x03
#define BT_CURS_WR_ADDR		0x04
#define BT_CURS_DATA		0x05
#define BT_COMMAND_REG_0	0x06
#define BT_CURS_RD_ADDR		0x07
#define BT_COMMAND_REG_1	0x08
#define BT_COMMAND_REG_2	0x09
#define BT_STATUS_REG		0x0A
#define BT_CURS_RAM_DATA	0x0B
#define BT_CURS_X_LOW		0x0C
#define BT_CURS_X_HIGH		0x0D
#define BT_CURS_Y_LOW		0x0E
#define BT_CURS_Y_HIGH		0x0F

_XFUNCPROTOBEGIN

extern void s3OutBtReg(
#if NeedFunctionPrototypes
	unsigned short,
	unsigned char,
	unsigned char
#endif
);

extern unsigned char s3InBtReg(
#if NeedFunctionPrototypes
	unsigned short
#endif
);

extern void s3OutBtRegCom3(
#if NeedFunctionPrototypes
	unsigned char,
	unsigned char
#endif
);

extern unsigned char s3InBtRegCom3(
#if NeedFunctionPrototypes
	void
#endif
);

extern unsigned char s3InBtStatReg(
#if NeedFunctionPrototypes
	void
#endif
);

_XFUNCPROTOEND

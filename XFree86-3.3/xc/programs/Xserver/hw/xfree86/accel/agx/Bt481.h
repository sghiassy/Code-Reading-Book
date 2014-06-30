/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/Bt481.h,v 3.4 1996/12/23 06:32:19 dawes Exp $ */
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
 * DAVID WEXELBLAT AND HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH REGARD 
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTORS BE LIABLE 
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER 
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the AGX and Bt481/2 by Henry. A. Worth
 *
 */
/* $XConsortium: Bt481.h /main/4 1996/02/21 17:15:32 kaleb $ */

#ifndef XF86_BT481_H
#define XF86_BT481_H

#include "compiler.h"
#include <X11/Xfuncproto.h>

/* directly addressed registers */

#define BT481_WRITE_ADDR	0x00
#define BT481_RAMDAC_DATA	0x01	
#define BT481_PIXEL_MASK	0x02
#define BT481_READ_ADDR		0x03
#define BT482_CURS_WR_ADDR	0x04
#define BT482_CURS_RAM_DATA	0x05
#define BT481_COMMAND_REG_A	0x06
#define BT482_CURS_RD_ADDR	0x07

/* indirectly addressed register addresses */

#define BT481_IND_PIXEL_MASK    0x00
#define BT481_OVERLAY_MASK      0x01
#define BT481_COMMAND_REG_B	0x02
#define BT482_CURSOR_REG        0x03
#define BT482_CURS_X_LOW     	0x04
#define BT482_CURS_X_HIGH	0x05
#define BT482_CURS_Y_LOW	0x06
#define BT482_CURS_Y_HIGH	0x07

#define BT481_8BPP_PSUEDO_COLOR 0x00
#define BT481_15BPP_EDGE_TRIGGR 0x80
#define BT481_16BPP_EDGE_TRIGGR 0xC0
#define BT481_15BPP_LEVL_TRIGGR 0xA0
#define BT481_16BPP_LEVL_TRIGGR 0xE0
#define BT481_24BPP_EDGE_TRIGGR 0x90    /* RGBP unpacked 32BPP/8BPP-Psuedo */
#define BT481_24BPP_LEVL_TRIGGR 0xF0	/* RGB packed 24BPP */


_XFUNCPROTOBEGIN


extern void xf86OutBt481IndReg(
#if NeedFunctionPrototypes
	unsigned char,
	unsigned char,
	unsigned char
#endif
);

extern unsigned char xf86InBt481IndReg(
#if NeedFunctionPrototypes
	unsigned char
#endif
);

extern void xf86Bt481HWSave(
#if NeedFunctionPrototypes
        union xf86RamDacSave *
#endif
);

extern void xf86Bt481HWRestore(
#if NeedFunctionPrototypes
        union xf86RamDacSave *
#endif
);

extern void xf86Bt481Init(
#if NeedFunctionPrototypes
	void
#endif
);

_XFUNCPROTOEND

#endif /* XF86_BT481_H */

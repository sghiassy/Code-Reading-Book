/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/Att490.h,v 3.2 1996/12/23 06:32:17 dawes Exp $ */
/*
 *
 */

/* $XConsortium: Att490.h /main/3 1996/02/21 17:15:20 kaleb $ */

#ifndef XF86_ATT490_H
#define XF86_ATT490_H

#include "compiler.h"
#include <X11/Xfuncproto.h>

/* directly addressed registers */

#define ATT490_WRITE_ADDR	0x00
#define ATT490_RAMDAC_DATA	0x01	
#define ATT490_PIXEL_MASK	0x02
#define ATT490_READ_ADDR	0x03
#define ATT490_CURS_WR_ADDR	0x04
#define ATT490_CURS_RAM_DATA	0x05
#define ATT490_COMMAND_REG_A	0x06
#define ATT490_CURS_RD_ADDR	0x07

/* command register fields */

#define ATT490_8BPP_PSUEDO_COLOR 0x00
#define ATT490_15BPP_EDGE_TRIGGR 0x80
#define ATT490_15BPP_LEVL_TRIGGR 0xA0
#define ATT490_16BPP_LEVL_TRIGGR 0xC0

#define ATT490_8BPP_DAC		0x02

#define ATT490_SYNC_MASK   	0x1C
#define ATT490_SYNC_ON_BLUE	0x04
#define ATT490_SYNC_ON_GREEN	0x08
#define ATT490_SYNC_ON_RED	0x10

#define ATT490_SLEEP            0x01

_XFUNCPROTOBEGIN


extern void xf86Att490HWSave(
#if NeedFunctionPrototypes
        union xf86RamDacSave *
#endif
);

extern void xf86Att490HWRestore(
#if NeedFunctionPrototypes
        union xf86RamDacSave *
#endif
);

extern void xf86Att490Init(
#if NeedFunctionPrototypes
	void
#endif
);

_XFUNCPROTOEND

#endif /* XF86_ATT490_H */

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/xf861502x.h,v 3.5 1996/12/23 06:33:08 dawes Exp $ */
/*
 * Copyright 1994 by Henry A. Worth  <haw30@eng.amdahl.com>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Henry Worth not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Henry Worth makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * HENRY A. WORTH DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, 
 * IN NO EVENT SHALL THE HENRY A. WORTH BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: xf861502x.h /main/5 1996/02/21 17:18:58 kaleb $ */

#ifndef XF86_SC1502X_H
#define XF86_SC1502X_H

#include "compiler.h"
#include <X11/Xfuncproto.h>

extern Bool SC15021;
extern Bool SC1502X_EPRF_SET;
extern Bool SC1502X_IPF_ACCESS;


/* directly addressed registers */

#define SC1502X_WRITE_ADDR	0x00     
#define SC1502X_RAMDAC_DATA	0x01	
#define SC1502X_PIXEL_MASK	0x02
#define SC1502X_READ_ADDR	0x03
#define SC1502X_OVR_WRT_ADDR    0x04
#define SC1502X_OVR_DATA        0x05 
#define SC1502X_COMMAND		0x06
#define SC1502X_OVR_RD_ADDR     0x07 

/* indirectly addressed register addresses */

#define SC1502X_COMMAND_IPF   	0x02    

/*
 *  Command Register:
 *
 *   The command register can be accessed three different ways:
 *
 *         1 - Directly via RS0-2 when the ERPF bit is clear.
 *         2 - Indirectly at the pixel mask address by setting the 
 *             IPF bit (for video chips that don't support the 
 *             RAMDAC RS2 addr bit).
 *         3 - Directly as one of the extended registers when 
 *             the ERPF bit is set.
 * 
 *   The IPF bit is set by four consective reads of the pixel mask
 *   register. The command register can then be read at the pixel
 *   mask resister address and written ONCE. A read or write to any
 *   other register will clear the IPF bit. The IPF indirect read-
 *   modify-write acceses can be used to set the ERPF bit in the 
 *   command. Once the ERPF bit is set the extened register overlay
 *   the VGA RAMDAC registers.
 *
 *   Bit definition for the command register follow
 */

#define SC1502X_CMD_EPRF		0x10
#define SC15025_CMD_GAMMA_CORRECT	0x08
#define SC1502X_CMD_8BPP_PSUEDO		0x00
#define SC15021_CMD_4BPP_PSUEDO		0x01
#define SC1502X_CMD_15BPP_EDGE_TRGR	0x80
#define SC1502X_CMD_15BPP		0xA0
#define SC15021_CMD_8BPP_15BPP_MIX      0x01
#define SC1502X_CMD_16BPP_EDGE_TRGR	0xC0
#define SC1502X_CMD_16BPP		0xE0
#define SC15025_CMD_24BPP_RGB_EDGE_TRGR 0x41
#define SC15025_CMD_24BPP_BGR_EDGE_TRGR 0x40
#define SC1502X_CMD_24BPP_RGB		0x60
#define SC1502X_CMD_24BPP_BGR		0x61


/* registers accessed by setting EPRF bit */

#define SC1502X_EXT_DATA	0x00
#define SC1502X_EXT_IDX_R0     	0x01
#define SC1502X_COMMAND_EPRF    0x02
#define SC1502X_EXT_IDX_WO     	0x03

/* extended, indexed, registers */
#define SC1502X_AUX_CNTL       	0x08
#define SC1502X_AUX_8_BIT_DAC		0x01
#define SC15021_AUX_PED_75_IRE		0x02
#define SC15021_AUX_POWER_DOWN		0x04

#define SC1502X_ID1		0x09
#define SC1502X_ID1_SIERRA		0x53

#define SC1502X_ID2		0x0A
#define SC1502X_ID2_1502X		0x36

#define SC1502X_ID3		0x0B
#define SC1502X_ID3_15025		0xB1
#define SC1502X_ID3_15026		0xB1
#define SC1502X_ID3_15021		0xAC

#define SC1502X_ID4		0x0C
#define SC1502X_ID4_1502X		0x41

#define SC1502X_PIXEL_MASK_LOW	0x0D
#define SC1502X_PIXEL_MASK_MID	0x0E
#define SC1502X_PIXEL_MASK_HI	0x0F

#define SC1502X_PIXEL_REPACK	0x10
#define SC1502X_RP_8X1_TO_8X1		0x00
#define SC1502X_RP_8X2_TO_16X1		0x00
#define SC1502X_RP_8X3_TO_24X1		0x00
#define SC1502X_RP_8X4_TO_24X1		0x01
#define SC15021_RP_16X1_TO_16X1		0x00
#define SC15021_RP_16X1_TO_16X1_ALT	0x08
#define SC15021_RP_8X1_TO_4X2		0x02
#define SC15021_RP_16X1_TO_8X2		0x04
#define SC15021_RP_16X3_TO_24X2		0x05	/* 2/3 clock */
#define SC15021_RP_16X2_TO_24X1		0x06	/* 1/2 clock */

#define SC15021_CURSOR		0x11
#define SC15021_CUR_DISABLED		0x00
#define SC15021_CUR_3_CLR		0x08
#define SC15021_CUR_2_CLR_WIN		0x10 
#define SC15021_CUR_2_CLR_X11   	0x18 
#define SC15021_CUR_POLARITY    	0x80 
#define SC15021_CUR_DELAY_ZERO		0x00
#define SC15021_CUR_DELAY_PLUS_TWO	0x04
#define SC15021_CUR_DELAY_PLUS_ONE	0x05
#define SC15021_CUR_DELAY_MINUS_ONE	0x06
#define SC15021_CUR_DELAY_MINUS_TWO	0x07

#define SC15021_SEC_CNTL	0x12
#define SC15021_SC_8BPP_PSUEDO		0x00
#define SC15021_SC_4BPP_PSUEDO		0x00
#define SC15021_SC_15BPP_DIRECT		0x00
#define SC15021_SC_16BPP_DIRECT		0x00
#define SC15021_SC_8_TO_15_MIX 		0x00
#define SC15021_SC_24BPP_RGB   		0x00
#define SC15021_SC_24BPP_BGR   		0x00
#define SC15021_SC_8_TO_16_MIX_A	0x01
#define SC15021_SC_8_TO_16_MIX_B	0x02
#define SC15021_SC_8_TO_15_MIX_A	0x02
#define SC15021_SC_8_TO_15_MIX_B	0x01

_XFUNCPROTOBEGIN

extern void xf86OutSc1502xIndReg(
#if NeedFunctionPrototypes
	unsigned short,
	unsigned char,
	unsigned char
#endif
);

extern unsigned char xf86InSc1502xIndReg(
#if NeedFunctionPrototypes
	unsigned short 
#endif
);

extern void xf86OutSc1502xCmd(
#if NeedFunctionPrototypes
        unsigned char,
        unsigned char
#endif
);

extern unsigned char xf86InSc1502xCmd(
#if NeedFunctionPrototypes
        void
#endif
);


extern void xf86Sc1502xHWSave(
#if NeedFunctionPrototypes
        union xf86RamDacSave *
#endif
);

extern void xf86Sc1502xHWRestore(
#if NeedFunctionPrototypes
        union xf86RamDacSave *
#endif
);

extern void xf86Sc1502xInit(
#if NeedFunctionPrototypes
	void
#endif
);

extern void xf86Sc1502xPrintId(
#if NeedFunctionPrototypes
	void
#endif
);

_XFUNCPROTOEND

#endif /* XF86_SC1502X_H */

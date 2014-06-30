/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/regmach32.h,v 3.7 1996/12/23 06:38:51 dawes Exp $ */
/* regmach32.h
 *
 * Written by Jake Richter
 * Copyright (c) 1989, 1990 Panacea Inc., Londonderry, NH - All Rights Reserved
 * Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * This code may be freely incorporated in any program without royalty, as
 * long as the copyright notice stays intact.
 *
 * Additions by Kevin E. Martin (martin@cs.unc.edu)
 *
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 */
/* $XConsortium: regmach32.h /main/5 1996/10/19 17:53:05 kaleb $ */

#ifndef REGMACH32_H
#define REGMACH32_H

#include "compiler.h"
#include "Xos.h"

/* VESA Approved Register Definitions */
#define	DAC_MASK		0x02ea
#define	DAC_R_INDEX		0x02eb
#define	DAC_W_INDEX		0x02ec
#define	DAC_DATA		0x02ed
#define	DISP_STAT		0x02e8
#define	H_TOTAL			0x02e8
#define	H_DISP			0x06e8
#define	H_SYNC_STRT		0x0ae8
#define	CURSOR_OFFSET_LO	0x0aee
#define	H_SYNC_WID		0x0ee8
#define	CURSOR_OFFSET_HI	0x0eee
#define	CURSOR_COLOR_0		0x1aee
#define CURSOR_COLOR_1		0x1aef
#define	V_TOTAL			0x12e8
#define	HORZ_CURSOR_POSN	0x12ee

#define	V_DISP			0x16e8
#define	VERT_CURSOR_POSN	0x16ee
#define	V_SYNC_STRT		0x1ae8
#define	CURSOR_COLOR_0		0x1aee
#define CURSOR_COLOR_1		0x1aef
#define	V_SYNC_WID		0x1ee8
#define	HORZ_CURSOR_OFFSET	0x1eee
#define	VERT_CURSOR_OFFSET	0x1eef

#define	DISP_CNTL		0x22e8
#define	ADVFUNC_CNTL		0x4ae8
#define	SUBSYS_STAT		0x42e8
#define	SUBSYS_CNTL		0x42e8
#define	ROM_PAGE_SEL		0x46e8
#define	CUR_Y			0x82e8
#define	CUR_X			0x86e8
#define	DESTY_AXSTP		0x8ae8
#define	DESTX_DIASTP		0x8ee8
#define	ERR_TERM		0x92e8
#define	MAJ_AXIS_PCNT		0x96e8
#define BRES_COUNT		0x96ee
#define	GP_STAT			0x9ae8
#define	GE_STAT			GP_STAT
#define	CMD			0x9ae8
#define	SHORT_STROKE		0x9ee8
#define	BKGD_COLOR		0xa2e8
#define	FRGD_COLOR		0xa6e8
#define	WRT_MASK		0xaae8
#define	RD_MASK			0xaee8
#define	COLOR_CMP		0xb2e8
#define	BKGD_MIX		0xb6e8
#define	FRGD_MIX		0xbae8
#define	MULTIFUNC_CNTL		0xbee8
#define	MIN_AXIS_PCNT		0x0000
#define	SCISSORS_T		0x1000
#define	SCISSORS_L		0x2000
#define	SCISSORS_B		0x3000
#define	SCISSORS_R		0x4000
#define	MEM_CNTL		0x5000
#define	PATTERN_L		0x8000
#define	PATTERN_H		0x9000
#define	PIX_CNTL		0xa000
#define	PIX_TRANS		0xe2e8

/* Display Status Bit Fields */
#define	HORTOG			0x0004
#define	VBLANK			0x0002
#define	SENSE			0x0001

/* Horizontal Sync Width Bit Field */
#define HSYNCPOL_NEG		0x0020
#define	HSYNCPOL_POS		0x0000

/* Vertical Sync Width Bit Field */
#define	VSYNCPOL_NEG		0x0020
#define	VSYNCPOL_POS		0x0000

/* Display Control Bit Field */
#define	DISPEN_NC		0x0000
#define	DISPEN_DISAB		0x0040
#define	DISPEN_ENAB		0x0020
#define	INTERLACE		0x0010
#define	DBLSCAN			0x0008
#define	MEMCFG_2		0x0000
#define	MEMCFG_4		0x0002
#define	MEMCFG_6		0x0004
#define	MEMCFG_8		0x0006
#define	ODDBNKENAB		0x0001

/* Subsystem Status Register */
#define	_8PLANE			0x0080
#define	MONITORID_8503		0x0050
#define	MONITORID_8507		0x0010
#define	MONITORID_8512		0x0060
#define	MONITORID_8513		0x0060
#define	MONITORID_8514		0x0020
#define	MONITORID_NONE		0x0070
#define	MONITORID_MASK		0x0070
#define	GPIDLE			0x0008
#define	INVALIDIO		0x0004
#define	PICKFLAG		0x0002
#define	VBLNKFLG		0x0001

/* Subsystem Control Register */
#define	GPCTRL_NC		0x0000
#define	GPCTRL_ENAB		0x4000
#define	GPCTRL_RESET		0x8000
#define CHPTEST_NC		0x0000
#define CHPTEST_NORMAL		0x1000
#define CHPTEST_ENAB		0x2000
#define	IGPIDLE			0x0800
#define	IINVALIDIO		0x0400
#define	IPICKFLAG		0x0200
#define	IVBLNKFLG		0x0100
#define	RGPIDLE			0x0008
#define	RINVALIDIO		0x0004
#define	RPICKFLAG		0x0002
#define	RVBLNKFLG		0x0001

/* Current X, Y & Dest X, Y Mask */
#define	COORD_MASK		0x07ff

#ifdef CLKSEL
#undef CLKSEL
#endif

/* Advanced Function Control Regsiter */
#define	CLKSEL			0x0004
#define	DISABPASSTHRU		0x0001

/* Graphics Processor Status Register */
#define	GPBUSY			0x0200
#define	DATDRDY			0x0100
#define	DATA_READY		DATDRDY

/* Command Register */
#define	CMD_NOP			0x0000
#define	CMD_LINE		0x2000
#define	CMD_RECT		0x4000
#define	CMD_RECTV1		0x6000
#define	CMD_RECTV2		0x8000
#define	CMD_LINEAF		0xa000
#define	CMD_BITBLT		0xc000
#define	CMD_OP_MSK		0xf000
#define	BYTSEQ			0x1000
#define	_16BIT			0x0200
#define	PCDATA			0x0100
#define	INC_Y			0x0080
#define	YMAJAXIS		0x0040
#define	INC_X			0x0020
#define	DRAW			0x0010
#define	LINETYPE		0x0008
#define	LASTPIX			0x0004
#define	PLANAR			0x0002
#define	WRTDATA			0x0001

/* Short Stroke Vector Transfer Register (The angular Defs also apply to the Command Register */
#define	VECDIR_000		0x0000
#define	VECDIR_045		0x0020
#define	VECDIR_090		0x0040
#define	VECDIR_135		0x0060
#define	VECDIR_180		0x0080
#define	VECDIR_225		0x00a0
#define	VECDIR_270		0x00c0
#define	VECDIR_315		0x00e0
#define	SSVDRAW			0x0010

/* Background Mix Register */
#define	BSS_BKGDCOL		0x0000
#define	BSS_FRGDCOL		0x0020
#define	BSS_PCDATA		0x0040
#define	BSS_BITBLT		0x0060

/* Foreground Mix Register */
#define	FSS_BKGDCOL		0x0000
#define	FSS_FRGDCOL		0x0020
#define	FSS_PCDATA		0x0040
#define	FSS_BITBLT		0x0060

/* The Mixes */
#define	MIX_MASK			0x001f

#define	MIX_NOT_DST			0x0000
#define	MIX_0				0x0001
#define	MIX_1				0x0002
#define	MIX_DST				0x0003
#define	MIX_NOT_SRC			0x0004
#define	MIX_XOR				0x0005
#define	MIX_XNOR			0x0006
#define	MIX_SRC				0x0007
#define	MIX_NAND			0x0008
#define	MIX_NOT_SRC_OR_DST		0x0009
#define	MIX_SRC_OR_NOT_DST		0x000a
#define	MIX_OR				0x000b
#define	MIX_AND				0x000c
#define	MIX_SRC_AND_NOT_DST		0x000d
#define	MIX_NOT_SRC_AND_DST		0x000e
#define	MIX_NOR				0x000f

#define	MIX_MIN				0x0010
#define	MIX_DST_MINUS_SRC		0x0011
#define	MIX_SRC_MINUS_DST		0x0012
#define	MIX_PLUS			0x0013
#define	MIX_MAX				0x0014
#define	MIX_HALF__DST_MINUS_SRC		0x0015
#define	MIX_HALF__SRC_MINUS_DST		0x0016
#define	MIX_AVERAGE			0x0017
#define	MIX_DST_MINUS_SRC_SAT		0x0018
#define	MIX_SRC_MINUS_DST_SAT		0x001a
#define	MIX_HALF__DST_MINUS_SRC_SAT	0x001c
#define	MIX_HALF__SRC_MINUS_DST_SAT	0x001e
#define	MIX_AVERAGE_SAT			0x001f
#define	MIX_FN_PAINT			MIX_SRC

/* Memory Control Register */
#define	BUFSWP			0x0010
#define	VRTCFG_2		0x0000
#define	VRTCFG_4		0x0004
#define	VRTCFG_6		0x0008
#define	VRTCFG_8		0x000C
#define	HORCFG_4		0x0000
#define	HORCFG_5		0x0001
#define	HORCFG_8		0x0002
#define	HORCFG_10		0x0003

/* Pixel Control Register */
#define	MIXSEL_FRGDMIX		0x0000
#define	MIXSEL_PATT		0x0040
#define	MIXSEL_EXPPC		0x0080
#define	MIXSEL_EXPBLT		0x00c0
#define COLCMPOP_F		0x0000
#define COLCMPOP_T		0x0008
#define COLCMPOP_GE		0x0010
#define COLCMPOP_LT		0x0018
#define COLCMPOP_NE		0x0020
#define COLCMPOP_EQ		0x0028
#define COLCMPOP_LE		0x0030
#define COLCMPOP_GT		0x0038
#define	PLANEMODE		0x0004

/* Mach32 Regsiters */
#define GE_OFFSET_LO		0x6eee
#define GE_OFFSET_HI		0x72ee
#define CRT_OFFSET_LO		0x2aee
#define CRT_OFFSET_HI		0x2eee
#define CLOCK_SEL		0x4aee
#define SHADOW_SET		0x5aee
#define SHADOW_CTL		0x46ee
#define CRT_PITCH		0x26ee
#define GE_PITCH		0x76ee
#define EXT_SCISSOR_L		0xdaee
#define EXT_SCISSOR_R		0xe2ee
#define EXT_SCISSOR_T		0xdeee
#define EXT_SCISSOR_B		0xe6ee
#define MISC_OPTIONS		0x36ee
#define CONFIG_STATUS_1		0x12ee
#define CONFIG_STATUS_2		0x16ee
#define MEM_CFG			0x5eee
#define MEM_BNDRY		0x42ee
#define ROM_ADDR_1		0x52ee
#define READ_SRC_X		0xdaee
#define CHIP_ID			0xfaee
#define EXT_CURSOR_COLOR_0	0x3aee
#define EXT_CURSOR_COLOR_1	0x3eee
#define EXT_FIFO_STATUS		0x9aee
#define EXT_GE_CONFIG		0x7aee
#define R_EXT_GE_CONFIG		0x8eee
#define EXT_GE_STATUS		0x62ee
#define MISC_CNTL		0x7eee
#define R_MISC_CNTL		0x92ee
#define DP_CONFIG		0xceee
#define DEST_X_START		0xa6ee
#define DEST_X_END		0xaaee
#define DEST_Y_END		0xaeee
#define ALU_FG_FN		0xbaee
#define HORZ_OVERSCAN		0x62ee
#define VERT_OVERSCAN		0x66ee
#define LINEDRAW		0xfeee
#define LINEDRAW_INDEX		0x9aee
#define LINEDRAW_OPT		0xa2ee

/* Mach8 config status 1 */
#define CLK_MODE		0x0001
#define BUS_16			0x0002
#define MC_BUS			0x0004
#define EEPROM_ENA		0x0008
#define DRAM_ENA		0x0010
#define MEM_INSTALLED		0x0060
#define ROM_ENA			0x0080
#define ROM_PAGE_ENA		0x0100
#define ROM_LOCATION		0xfe00

/* Mach8 config status 2 */
#define SHARE_CLOCK		0x0001
#define HIRES_BOOT		0x0002
#define EPROM_16_ENA		0x0004
#define WRITE_PER_BIT		0x0008
#define FLASH_ENA		0x0010

/* Mach32 config status 1 */
#define _8514_ONLY		0x0001
#define BUS_TYPE		0x000e
#define MEM_TYPE		0x0070
#define CHIP_DIS		0x0080
#define TST_VCTR_ENA		0x0100
#define DACTYPE			0x0e00
#define MC_ADR_DECODE		0x1000
#define CARD_ID			0xe000

/* Mach32 config status 2 */
#define SLOW_SEQ_EN		0x0001
#define MEM_ADDR_DIS		0x0002
#define ISA_16_ENA		0x0004
#define KOR_TXT_MODE_ENA	0x0008
#define LOCAL_BUS_SUPPORT	0x0030
#define LOCAL_BUS_CONFIG_2	0x0040
#define LOCAL_BUS_RD_DLY_ENA	0x0080
#define LOCAL_DAC_EN		0x0100
#define LOCAL_RDY_EN		0x0200
#define EEPROM_ADR_SEL		0x0400
#define GE_STRAP_SEL		0x0800
#define VESA_RDY		0x1000
#define Z4GB			0x2000
#define LOC2_MDRAM		0x4000

/* Mach32 Bus types */
#define ISA_16_BIT		0x0000
#define EISA			0x0002
#define MICRO_C_16_BIT		0x0004
#define MICRO_C_8_BIT		0x0006
#define LOCAL_386SX		0x0008
#define LOCAL_386DX		0x000a
#define LOCAL_486		0x000c
#define PCI			0x000e

/* Mach32 RAMDAC types */
#define DAC_ATI68830		0x00
#define DAC_SC11483		0x01
#define DAC_TLC34075		0x02
#define DAC_BT476		0x03
#define DAC_BT481		0x04
#define DAC_ATI68860		0x05
#define DAC_ATI68875		DAC_TLC34075
/* RAMDACs not in the base docs but listed for completeness */
#define DAC_STG1700             0x06
#define DAC_ATT21C498           0x07
/* RAMDACs that are distinct but aliased */
#define DAC_ATT20C490           0x14

/* Mach32 CRT Polarity */
#define H_POLARITY_POS		0x0000
#define H_POLARITY_NEG		0x0020
#define V_POLARITY_POS		0x0000
#define V_POLARITY_NEG		0x0020

/* Mach32 Composite Sync */
#define COMPOSITE_SYNC		0x1000

/* Mach32 Number of Video FIFO entries */
#define VFIFO_DEPTH_1		0x0100
#define VFIFO_DEPTH_2		0x0200
#define VFIFO_DEPTH_3		0x0300
#define VFIFO_DEPTH_4		0x0400
#define VFIFO_DEPTH_5		0x0500
#define VFIFO_DEPTH_6		0x0600
#define VFIFO_DEPTH_7		0x0700
#define VFIFO_DEPTH_8		0x0800
#define VFIFO_DEPTH_9		0x0900
#define VFIFO_DEPTH_A		0x0a00
#define VFIFO_DEPTH_B		0x0b00
#define VFIFO_DEPTH_C		0x0c00
#define VFIFO_DEPTH_D		0x0d00
#define VFIFO_DEPTH_E		0x0e00
#define VFIFO_DEPTH_F		0x0f00

/* Mach32 Misc Register */
#define W_STATE_ENA		0x0000
#define HOST_8_ENA		0x0001
#define MEM_SIZE_ALIAS		0x000c
#define DISABLE_VGA		0x0010
#define _16_BIT_IO		0x0020
#define DISABLE_DAC		0x0040
#define DLY_LATCH_ENA		0x0080
#define TEST_MODE		0x0100
#define BLK_WR_ENA		0x0400
#define _64_DRAW_ENA		0x0800
#define MEM_SIZE_512K		0x0000
#define MEM_SIZE_1M		0x0004
#define MEM_SIZE_2M		0x0008
#define MEM_SIZE_4M		0x000c

/* Mach32 Extended Graphics Engine Configuration Register */
#define PIXEL_WIDTH_4		0x0000
#define PIXEL_WIDTH_8		0x0010
#define PIXEL_WIDTH_16		0x0020
#define PIXEL_WIDTH_24		0x0030
#define RGB16_555		0x0000
#define RGB16_565		0x0040
#define RGB16_655		0x0080
#define RGB16_664		0x00c0
#define MULTIPLEX_PIXELS	0x0100
#define RGB24			0x0000
#define RGBx24			0x0200
#define BGR24			0x0400
#define xBGR24			0x0600
#define DAC_RS0                 0x0000
#define DAC_RS1                 0x1000
#define DAC_RS2                 0x2000
#define DAC_RS3                 0x3000
#define DAC_8_BIT_EN		0x4000
#define PIX_WIDTH_16BPP		PIXEL_WIDTH_16
#define ORDER_16BPP_565		RGB16_565

/* Mach32 DP_CONFIG register */
#define FG_COLOR_SRC_BLIT	0x6000
#define FG_COLOR_SRC_FG		0x2000
#define DATA_WIDTH		0x0200
#define DATA_ORDER		0x1000
#define READ_WRITE		0x0001

/* Mach8/Mach32 Linedraw Options register */
#define BOUNDS_RESET		0x0100
#define CLIP_MODE_0		0x0000		/* clip exception disabled */
#define CLIP_MODE_1		0x0200		/* line segments */
#define CLIP_MODE_2		0x0400		/* polygon boundary lines */
#define CLIP_MODE_3		0x0600		/* patterned lines */

/* ATI VGA Extended Regsiters */
#define ATIEXT		0x1ce
#define ATI2E		0xae
#define ATI32		0xb2
#define ATI36		0xb6

/* VGA Graphics Controller Registers */
#define VGAGRA		0x3ce
#define GRA06		0x06

/* VGA Seququencer Registers */
#define VGASEQ		0x3c4
#define SEQ02		0x02
#define SEQ04		0x04

#define MACH32_MAX_X	2047
#define MACH32_MAX_Y	2047

/* RAMDAC type 2 registers */
#define	OUTPUT_CLK_SEL  0x2ea
#define	MUX_CNTL	0x2eb
#define	INPUT_CLK_SEL	0x2ed

/* RAMDAC type 4 registers */
#define ATT_MODE        0x2ea

typedef struct {
    unsigned char r, g, b;
} LUTENTRY;

typedef struct {
    unsigned short h_total, h_disp, h_sync_strt, h_sync_wid;
    unsigned short v_total, v_disp, v_sync_strt, v_sync_wid;
    unsigned short disp_cntl;
    unsigned short clock_sel;
} mach32CRTCRegRec, *mach32CRTCRegPtr;

/* Wait until "v" queue entries are free */
#define	WaitQueue(v)	{ while (inw(EXT_FIFO_STATUS) & (0x10000 >> (v))); }

/* Wait until GP is idle and queue is empty */
#define	WaitIdleEmpty() { while (inw(GP_STAT) & (GPBUSY | 1)); }

#ifdef NO_PROBEWAIT_PROBLEM
/*
 * This version is for use in mach8Probe() to prevent a server hang if
 * there is no ATI chip present
 */
#define	ProbeWaitIdleEmpty() { int i; \
			       for (i = 0; i < 100000; i++) \
			          if (!(inw(GP_STAT) & (GPBUSY | 1))) \
				     break; \
			     }
#else
#define ProbeWaitIdleEmpty WaitIdleEmpty
#endif

#define SKIP_2(_v) ((((_v)<<1)&0xfff8)|((_v)&0x3)|(((_v)&0x80)>>5))

#ifndef NULL
#define NULL	0
#endif

#endif /* REGMACH32_H */

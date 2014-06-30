/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/ati/regati.h,v 3.12 1996/12/28 08:16:48 dawes Exp $ */
/*
 * Copyright 1994 through 1996 by Marc Aurele La France (TSI @ UQV), tsi@ualberta.ca
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of Marc Aurele La France not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Marc Aurele La France makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as-is" without express or implied warranty.
 *
 * MARC AURELE LA FRANCE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.  IN NO
 * EVENT SHALL MARC AURELE LA FRANCE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Acknowledgements:
 *    Jake Richter, Panacea Inc., Londonderry, New Hampshire, U.S.A.
 *    Kevin E. Martin, martin@cs.unc.edu
 *    Tiago Gons, tiago@comosjn.hobby.nl
 *    Rickard E. Faith, faith@cs.unc.edu
 *    Scott Laird, lair@kimbark.uchicago.edu
 *
 * The intent here is to list all I/O ports for VGA (and its predecessors),
 * ATI VGA Wonder, 8514/A, ATI Mach8, ATI Mach32 and ATI Mach64 video adapters,
 * not just the ones in use by the VGA Wonder driver.
 */
/* $XConsortium: regati.h /main/7 1996/03/17 22:17:50 kaleb $ */

#ifndef _REGATI_H_
#define _REGATI_H_

/* I/O decoding definitions */
#define SPARSE_IO_BASE		0x03fcu
#define SPARSE_IO_SELECT	0xfc00u

#define BLOCK_IO_BASE		0xff00u
#define BLOCK_IO_SELECT		0x00fcu

#define IO_BYTE_SELECT		0x0003u

/*
 * Macros to get/set a contiguous bit field.  'Mask' should not be
 * self-modifying.
 */
#define GetBits(_Value, _Mask)	\
	(((_Value) & (_Mask)) / ((((_Mask) ^ ((_Mask) - 1)) + 1) >> 1))
#define SetBits(_Value, _Mask)	\
	(((_Value) * ((((_Mask) ^ ((_Mask) - 1)) + 1) >> 1)) & (_Mask))

#define IOPortTag(_SparseIOSelect, _BlockIOSelect)	\
	(SetBits((_SparseIOSelect), SPARSE_IO_SELECT) | \
	 SetBits((_BlockIOSelect), BLOCK_IO_SELECT))

/* MDA/CGA/EGA/VGA I/O ports */
#define GENVS			0x0102u		/* Write (and Read on uC only) */

#define R_GENLPS		0x03b9u		/* Read */

#define GENHP			0x03bfu

#define ATTRX			0x03c0u
#define ATTRD			0x03c1u
#define GENS0			0x03c2u		/* Read */
#define GENMO			0x03c2u		/* Write */
#define GENENB			0x03c3u		/* Read */
#define SEQX			0x03c4u
#define SEQD			0x03c5u
#define VGA_DAC_MASK		0x03c6u
#define VGA_DAC_R_I		0x03c7u
#define VGA_DAC_W_I		0x03c8u
#define VGA_DAC_DATA		0x03c9u
#define R_GENFC			0x03cau		/* Read */
/*	?			0x03cbu */
#define R_GENMO			0x03ccu		/* Read */
/*	?			0x03cdu */
#define GRAX			0x03ceu
#define GRAD			0x03cfu

#define GENB			0x03d9u

#define GENLPS			0x03dcu		/* Write */
#define KCX			0x03ddu
#define KCD			0x03deu

#define GENENA			0x46e8u		/* Write */

/* I/O port base numbers */
#define MonochromeIOBase	0x03b0u
#define ColourIOBase		0x03d0u

/* Other EGA/CGA/VGA I/O ports */
/*	?(_IOBase)		(_IOBase + 0x00u) */
/*	?(_IOBase)		(_IOBase + 0x01u) */
/*	?(_IOBase)		(_IOBase + 0x02u) */
/*	?(_IOBase)		(_IOBase + 0x03u) */
#define CRTX(_IOBase)		(_IOBase + 0x04u)
#define CRTD(_IOBase)		(_IOBase + 0x05u)
/*	?(_IOBase)		(_IOBase + 0x06u) */
/*	?(_IOBase)		(_IOBase + 0x07u) */
#define GENMC(_IOBase)		(_IOBase + 0x08u)
/*	?(_IOBase)		(_IOBase + 0x09u) */
#define GENS1(_IOBase)		(_IOBase + 0x0au)	/* Read */
#define GENFC(_IOBase)		(_IOBase + 0x0au)	/* Write */
#define GENLPC(_IOBase)		(_IOBase + 0x0bu)
/*	?(_IOBase)		(_IOBase + 0x0cu) */
/*	?(_IOBase)		(_IOBase + 0x0du) */
/*	?(_IOBase)		(_IOBase + 0x0eu) */
/*	?(_IOBase)		(_IOBase + 0x0fu) */

/* 8514/A VESA approved register definitions */
#define DISP_STAT		0x02e8u		/* Read */
#define SENSE				0x0001u	/* Presumably belong here */
#define VBLANK				0x0002u
#define HORTOG				0x0004u
#define H_TOTAL			0x02e8u		/* Write */
#define DAC_MASK		0x02eau
#define DAC_R_INDEX		0x02ebu
#define DAC_W_INDEX		0x02ecu
#define DAC_DATA		0x02edu
#define H_DISP			0x06e8u		/* Write */
#define H_SYNC_STRT		0x0ae8u		/* Write */
#define H_SYNC_WID		0x0ee8u		/* Write */
#define HSYNCPOL_POS			0x0000u
#define HSYNCPOL_NEG			0x0020u
#define H_POLARITY_POS			HSYNCPOL_POS	/* Sigh */
#define H_POLARITY_NEG			HSYNCPOL_NEG	/* Sigh */
#define V_TOTAL			0x12e8u		/* Write */
#define V_DISP			0x16e8u		/* Write */
#define V_SYNC_STRT		0x1ae8u		/* Write */
#define V_SYNC_WID		0x1ee8u		/* Write */
#define VSYNCPOL_POS			0x0000u
#define VSYNCPOL_NEG			0x0020u
#define V_POLARITY_POS			VSYNCPOL_POS	/* Sigh */
#define V_POLARITY_NEG			VSYNCPOL_NEG	/* Sigh */
#define DISP_CNTL		0x22e8u		/* Write */
#define ODDBNKENAB			0x0001u
#define MEMCFG_2			0x0000u
#define MEMCFG_4			0x0002u
#define MEMCFG_6			0x0004u
#define MEMCFG_8			0x0006u
#define DBLSCAN				0x0008u
#define INTERLACE			0x0010u
#define DISPEN_NC			0x0000u
#define DISPEN_ENAB			0x0020u
#define DISPEN_DISAB			0x0040u
#define R_H_TOTAL		0x26e8u		/* Read */
/*	?			0x2ae8u */
/*	?			0x2ee8u */
/*	?			0x32e8u */
/*	?			0x36e8u */
/*	?			0x3ae8u */
/*	?			0x3ee8u */
#define SUBSYS_STAT		0x42e8u		/* Read */
#define VBLNKFLG			0x0001u
#define PICKFLAG			0x0002u
#define INVALIDIO			0x0004u
#define GPIDLE				0x0008u
#define MONITORID_MASK			0x0070u
/*	MONITORID_?				0x0000u */
#define MONITORID_8507				0x0010u
#define MONITORID_8514				0x0020u
/*	MONITORID_?				0x0030u */
/*	MONITORID_?				0x0040u */
#define MONITORID_8503				0x0050u
#define MONITORID_8512				0x0060u
#define MONITORID_8513				0x0060u
#define MONITORID_NONE				0x0070u
#define _8PLANE				0x0080u
#define SUBSYS_CNTL		0x42e8u		/* Write */
#define RVBLNKFLG			0x0001u
#define RPICKFLAG			0x0002u
#define RINVALIDIO			0x0004u
#define RGPIDLE				0x0008u
#define IVBLNKFLG			0x0100u
#define IPICKFLAG			0x0200u
#define IINVALIDIO			0x0400u
#define IGPIDLE				0x0800u
#define CHPTEST_NC			0x0000u
#define CHPTEST_NORMAL			0x1000u
#define CHPTEST_ENAB			0x2000u
#define GPCTRL_NC			0x0000u
#define GPCTRL_ENAB			0x4000u
#define GPCTRL_RESET			0x8000u
#define ROM_PAGE_SEL		0x46e8u		/* Write */
#define ADVFUNC_CNTL		0x4ae8u		/* Write */
#define DISABPASSTHRU			0x0001u
#define CLOKSEL				0x0004u
/*	?			0x4ee8u */
#define EXT_CONFIG_0		0x52e8u		/* C & T 82C480 */
#define EXT_CONFIG_1		0x56e8u		/* C & T 82C480 */
#define EXT_CONFIG_2		0x5ae8u		/* C & T 82C480 */
#define EXT_CONFIG_3		0x5ee8u		/* C & T 82C480 */
/*	?			0x62e8u */
/*	?			0x66e8u */
/*	?			0x6ae8u */
/*	?			0x6ee8u */
/*	?			0x72e8u */
/*	?			0x76e8u */
/*	?			0x7ae8u */
/*	?			0x7ee8u */
#define CUR_Y			0x82e8u
#define CUR_X			0x86e8u
#define DESTY_AXSTP		0x8ae8u		/* Write */
#define DESTX_DIASTP		0x8ee8u		/* Write */
#define ERR_TERM		0x92e8u
#define MAJ_AXIS_PCNT		0x96e8u		/* Write */
#define GP_STAT			0x9ae8u		/* Read */
#define GE_STAT			0x9ae8u		/* Alias */
#define DATARDY				0x0100u
#define DATA_READY			DATARDY	/* Alias */
#define GPBUSY				0x0200u
#define CMD			0x9ae8u		/* Write */
#define WRTDATA				0x0001u
#define PLANAR				0x0002u
#define LASTPIX				0x0004u
#define LINETYPE			0x0008u
#define DRAW				0x0010u
#define INC_X				0x0020u
#define YMAJAXIS			0x0040u
#define INC_Y				0x0080u
#define PCDATA				0x0100u
#define _16BIT				0x0200u
#define CMD_NOP				0x0000u
#define CMD_OP_MSK			0xf000u
#define BYTSEQ					0x1000u
#define CMD_LINE				0x2000u
#define CMD_RECT				0x4000u
#define CMD_RECTV1				0x6000u
#define CMD_RECTV2				0x8000u
#define CMD_LINEAF				0xa000u
#define CMD_BITBLT				0xc000u
#define SHORT_STROKE		0x9ee8u		/* Write */
#define SSVDRAW				0x0010u
#define VECDIR_000			0x0000u
#define VECDIR_045			0x0020u
#define VECDIR_090			0x0040u
#define VECDIR_135			0x0060u
#define VECDIR_180			0x0080u
#define VECDIR_225			0x00a0u
#define VECDIR_270			0x00c0u
#define VECDIR_315			0x00e0u
#define BKGD_COLOR		0xa2e8u			/* Write */
#define FRGD_COLOR		0xa6e8u			/* Write */
#define WRT_MASK		0xaae8u			/* Write */
#define RD_MASK			0xaee8u			/* Write */
#define COLOR_CMP		0xb2e8u			/* Write */
#define BKGD_MIX		0xb6e8u			/* Write */
/*					0x001fu	See MIX_* definitions below */
#define BSS_BKGDCOL			0x0000u
#define BSS_FRGDCOL			0x0020u
#define BSS_PCDATA			0x0040u
#define BSS_BITBLT			0x0060u
#define FRGD_MIX		0xbae8u			/* Write */
/*					0x001fu	See MIX_* definitions below */
#define FSS_BKGDCOL			0x0000u
#define FSS_FRGDCOL			0x0020u
#define FSS_PCDATA			0x0040u
#define FSS_BITBLT			0x0060u
#define MULTIFUNC_CNTL		0xbee8u		/* Write */
#define MIN_AXIS_PCNT			0x0000u
#define SCISSORS_T			0x1000u
#define SCISSORS_L			0x2000u
#define SCISSORS_B			0x3000u
#define SCISSORS_R			0x4000u
#define MEM_CNTL			0x5000u
#define HORCFG_4				0x0000u
#define HORCFG_5				0x0001u
#define HORCFG_8				0x0002u
#define HORCFG_10				0x0003u
#define VRTCFG_2				0x0000u
#define VRTCFG_4				0x0004u
#define VRTCFG_6				0x0008u
#define VRTCFG_8				0x000cu
#define BUFSWP					0x0010u
#define PATTERN_L			0x8000u
#define PATTERN_H			0x9000u
#define PIX_CNTL			0xa000u
#define PLANEMODE				0x0004u
#define COLCMPOP_F				0x0000u
#define COLCMPOP_T				0x0008u
#define COLCMPOP_GE				0x0010u
#define COLCMPOP_LT				0x0018u
#define COLCMPOP_NE				0x0020u
#define COLCMPOP_EQ				0x0028u
#define COLCMPOP_LE				0x0030u
#define COLCMPOP_GT				0x0038u
#define	MIXSEL_FRGDMIX				0x0000u
#define	MIXSEL_PATT				0x0040u
#define	MIXSEL_EXPPC				0x0080u
#define	MIXSEL_EXPBLT				0x00c0u
/*	?			0xc2e8u */
/*	?			0xc6e8u */
/*	?			0xcae8u */
/*	?			0xcee8u */
/*	?			0xd2e8u */
/*	?			0xd6e8u */
/*	?			0xdae8u */
/*	?			0xdee8u */
#define PIX_TRANS		0xe2e8u
/*	?			0xe6e8u */
/*	?			0xeae8u */
/*	?			0xeee8u */
/*	?			0xf2e8u */
/*	?			0xf6e8u */
/*	?			0xfae8u */
/*	?			0xfee8u */

/* ATI Mach8 & Mach32 register definitions */
#define OVERSCAN_COLOR_8	0x02eeu		/* Write */	/* Mach32 */
#define OVERSCAN_BLUE_24	0x02efu		/* Write */	/* Mach32 */
#define OVERSCAN_GREEN_24	0x06eeu		/* Write */	/* Mach32 */
#define OVERSCAN_RED_24		0x06efu		/* Write */	/* Mach32 */
#define CURSOR_OFFSET_LO	0x0aeeu		/* Write */	/* Mach32 */
#define CURSOR_OFFSET_HI	0x0eeeu		/* Write */	/* Mach32 */
#define CONFIG_STATUS_1		0x12eeu		/* Read */
#define CLK_MODE			0x0001u			/* Mach8 */
#define BUS_16				0x0002u			/* Mach8 */
#define MC_BUS				0x0004u			/* Mach8 */
#define EEPROM_ENA			0x0008u			/* Mach8 */
#define DRAM_ENA			0x0010u			/* Mach8 */
#define MEM_INSTALLED			0x0060u			/* Mach8 */
#define ROM_ENA				0x0080u			/* Mach8 */
#define ROM_PAGE_ENA			0x0100u			/* Mach8 */
#define ROM_LOCATION			0xfe00u			/* Mach8 */
#define _8514_ONLY			0x0001u			/* Mach32 */
#define BUS_TYPE			0x000eu			/* Mach32 */
#define ISA_16_BIT				0x0000u		/* Mach32 */
#define EISA					0x0002u		/* Mach32 */
#define MICRO_C_16_BIT				0x0004u		/* Mach32 */
#define MICRO_C_8_BIT				0x0006u		/* Mach32 */
#define LOCAL_386SX				0x0008u		/* Mach32 */
#define LOCAL_386DX				0x000au		/* Mach32 */
#define LOCAL_486				0x000cu		/* Mach32 */
#define PCI					0x000eu		/* Mach32 */
#define MEM_TYPE			0x0070u			/* Mach32 */
#define CHIP_DIS			0x0080u			/* Mach32 */
#define TST_VCTR_ENA			0x0100u			/* Mach32 */
#define DACTYPE				0x0e00u			/* Mach32 */
#define MC_ADR_DECODE			0x1000u			/* Mach32 */
#define CARD_ID				0xe000u			/* Mach32 */
#define HORZ_CURSOR_POSN	0x12eeu		/* Write */	/* Mach32 */
#define CONFIG_STATUS_2		0x16eeu		/* Read */
#define SHARE_CLOCK			0x0001u			/* Mach8 */
#define HIRES_BOOT			0x0002u			/* Mach8 */
#define EPROM_16_ENA			0x0004u			/* Mach8 */
#define WRITE_PER_BIT			0x0008u			/* Mach8 */
#define FLASH_ENA			0x0010u			/* Mach8 */
#define SLOW_SEQ_EN			0x0001u			/* Mach32 */
#define MEM_ADDR_DIS			0x0002u			/* Mach32 */
#define ISA_16_ENA			0x0004u			/* Mach32 */
#define KOR_TXT_MODE_ENA		0x0008u			/* Mach32 */
#define LOCAL_BUS_SUPPORT		0x0030u			/* Mach32 */
#define LOCAL_BUS_CONFIG_2		0x0040u			/* Mach32 */
#define LOCAL_BUS_RD_DLY_ENA		0x0080u			/* Mach32 */
#define LOCAL_DAC_EN			0x0100u			/* Mach32 */
#define LOCAL_RDY_EN			0x0200u			/* Mach32 */
#define EEPROM_ADR_SEL			0x0400u			/* Mach32 */
#define GE_STRAP_SEL			0x0800u			/* Mach32 */
#define VESA_RDY			0x1000u			/* Mach32 */
#define Z4GB				0x2000u			/* Mach32 */
#define LOC2_MDRAM			0x4000u			/* Mach32 */
#define VERT_CURSOR_POSN	0x16eeu		/* Write */	/* Mach32 */
#define FIFO_TEST_DATA		0x1aeeu		/* Read */	/* Mach32 */
#define CURSOR_COLOR_0		0x1aeeu		/* Write */	/* Mach32 */
#define CURSOR_COLOR_1		0x1aefu		/* Write */	/* Mach32 */
#define HORZ_CURSOR_OFFSET	0x1eeeu		/* Write */	/* Mach32 */
#define VERT_CURSOR_OFFSET	0x1eefu		/* Write */	/* Mach32 */
#define PCI_CNTL		0x22eeu				/* Mach32-PCI */
#define CRT_PITCH		0x26eeu		/* Write */
#define CRT_OFFSET_LO		0x2aeeu		/* Write */
#define CRT_OFFSET_HI		0x2eeeu		/* Write */
#define LOCAL_CNTL		0x32eeu				/* Mach32 */
#define FIFO_OPT		0x36eeu		/* Write */	/* Mach8 */
#define MISC_OPTIONS		0x36eeu				/* Mach32 */
#define W_STATE_ENA			0x0000u			/* Mach32 */
#define HOST_8_ENA			0x0001u			/* Mach32 */
#define MEM_SIZE_ALIAS			0x000cu			/* Mach32 */
#define MEM_SIZE_512K				0x0000u		/* Mach32 */
#define MEM_SIZE_1M				0x0004u		/* Mach32 */
#define MEM_SIZE_2M				0x0008u		/* Mach32 */
#define MEM_SIZE_4M				0x000cu		/* Mach32 */
#define DISABLE_VGA			0x0010u			/* Mach32 */
#define _16_BIT_IO			0x0020u			/* Mach32 */
#define DISABLE_DAC			0x0040u			/* Mach32 */
#define DLY_LATCH_ENA			0x0080u			/* Mach32 */
#define TEST_MODE			0x0100u			/* Mach32 */
#define BLK_WR_ENA			0x0400u			/* Mach32 */
#define _64_DRAW_ENA			0x0800u			/* Mach32 */
#define FIFO_TEST_TAG		0x3aeeu		/* Read */	/* Mach32 */
#define EXT_CURSOR_COLOR_0	0x3aeeu		/* Write */	/* Mach32 */
#define EXT_CURSOR_COLOR_1	0x3eeeu		/* Write */	/* Mach32 */
#define MEM_BNDRY		0x42eeu				/* Mach32 */
#define MEM_PAGE_BNDRY			0x000fu			/* Mach32 */
#define MEM_BNDRY_ENA			0x0010u			/* Mach32 */
#define SHADOW_CTL		0x46eeu		/* Write */
#define CLOCK_SEL		0x4aeeu
/*	DISABPASSTHRU			0x0001u	See ADVFUNC_CNTL */
#define VFIFO_DEPTH_1			0x0100u			/* Mach32 */
#define VFIFO_DEPTH_2			0x0200u			/* Mach32 */
#define VFIFO_DEPTH_3			0x0300u			/* Mach32 */
#define VFIFO_DEPTH_4			0x0400u			/* Mach32 */
#define VFIFO_DEPTH_5			0x0500u			/* Mach32 */
#define VFIFO_DEPTH_6			0x0600u			/* Mach32 */
#define VFIFO_DEPTH_7			0x0700u			/* Mach32 */
#define VFIFO_DEPTH_8			0x0800u			/* Mach32 */
#define VFIFO_DEPTH_9			0x0900u			/* Mach32 */
#define VFIFO_DEPTH_A			0x0a00u			/* Mach32 */
#define VFIFO_DEPTH_B			0x0b00u			/* Mach32 */
#define VFIFO_DEPTH_C			0x0c00u			/* Mach32 */
#define VFIFO_DEPTH_D			0x0d00u			/* Mach32 */
#define VFIFO_DEPTH_E			0x0e00u			/* Mach32 */
#define VFIFO_DEPTH_F			0x0f00u			/* Mach32 */
#define COMPOSITE_SYNC			0x1000u
/*	?			0x4eeeu */
#define ROM_ADDR_1		0x52eeu
#define BIOS_BASE_SEGMENT		0x007fu			/* Mach32 */
/*	?				0xff80u */		/* Mach32 */
#define ROM_ADDR_2		0x56eeu		/* Sick ... */
#define SHADOW_SET		0x5aeeu		/* Write */
#define MEM_CFG			0x5eeeu				/* Mach32 */
#define MEM_APERT_SEL			0x0003u			/* Mach32 */
#define MEM_APERT_PAGE			0x000cu			/* Mach32 */
#define MEM_APERT_LOC			0xfff0u			/* Mach32 */
#define EXT_GE_STATUS		0x62eeu		/* Read */	/* Mach32 */
#define HORZ_OVERSCAN		0x62eeu		/* Write */	/* Mach32 */
#define VERT_OVERSCAN		0x66eeu		/* Write */	/* Mach32 */
#define MAX_WAITSTATES		0x6aeeu
#define GE_OFFSET_LO		0x6eeeu		/* Write */
#define BOUNDS_LEFT		0x72eeu		/* Read */
#define GE_OFFSET_HI		0x72eeu		/* Write */
#define BOUNDS_TOP		0x76eeu		/* Read */
#define GE_PITCH		0x76eeu		/* Write */
#define BOUNDS_RIGHT		0x7aeeu		/* Read */
#define EXT_GE_CONFIG		0x7aeeu		/* Write */	/* Mach32 */
#define MONITOR_ALIAS			0x0007u			/* Mach32 */
/*	MONITOR_?				0x0000u */	/* Mach32 */
#define MONITOR_8507				0x0001u		/* Mach32 */
#define MONITOR_8514				0x0002u		/* Mach32 */
/*	MONITOR_?				0x0003u */	/* Mach32 */
/*	MONITOR_?				0x0004u */	/* Mach32 */
#define MONITOR_8503				0x0005u		/* Mach32 */
#define MONITOR_8512				0x0006u		/* Mach32 */
#define MONITOR_8513				0x0006u		/* Mach32 */
#define MONITOR_NONE				0x0007u		/* Mach32 */
#define ALIAS_ENA			0x0008u			/* Mach32 */
#define PIXEL_WIDTH_4			0x0000u			/* Mach32 */
#define PIXEL_WIDTH_8			0x0010u			/* Mach32 */
#define PIXEL_WIDTH_16			0x0020u			/* Mach32 */
#define PIXEL_WIDTH_24			0x0030u			/* Mach32 */
#define RGB16_555			0x0000u			/* Mach32 */
#define RGB16_565			0x0040u			/* Mach32 */
#define RGB16_655			0x0080u			/* Mach32 */
#define RGB16_664			0x00c0u			/* Mach32 */
#define MULTIPLEX_PIXELS		0x0100u			/* Mach32 */
#define RGB24				0x0000u			/* Mach32 */
#define RGBx24				0x0200u			/* Mach32 */
#define BGR24				0x0400u			/* Mach32 */
#define xBGR24				0x0600u			/* Mach32 */
#define DAC_8_BIT_EN			0x4000u			/* Mach32 */
#define PIX_WIDTH_16BPP			PIXEL_WIDTH_16		/* Mach32 */
#define ORDER_16BPP_565			RGB16_565		/* Mach32 */
#define BOUNDS_BOTTOM		0x7eeeu		/* Read */
#define MISC_CNTL		0x7eeeu		/* Write */	/* Mach32 */
#define PATT_DATA_INDEX		0x82eeu
/*	?			0x86eeu */
/*	?			0x8aeeu */
#define R_EXT_GE_CONFIG		0x8eeeu		/* Read */	/* Mach32 */
#define PATT_DATA		0x8eeeu		/* Write */
#define R_MISC_CNTL		0x92eeu		/* Read */	/* Mach32 */
#define BRES_COUNT		0x96eeu
#define EXT_FIFO_STATUS		0x9aeeu		/* Read */
#define LINEDRAW_INDEX		0x9aeeu		/* Write */
/*	?			0x9eeeu */
#define LINEDRAW_OPT		0xa2eeu
#define BOUNDS_RESET			0x0100u
#define CLIP_MODE_0			0x0000u	/* Clip exception disabled */
#define CLIP_MODE_1			0x0200u	/* Line segments */
#define CLIP_MODE_2			0x0400u	/* Polygon boundary lines */
#define CLIP_MODE_3			0x0600u	/* Patterned lines */
#define DEST_X_START		0xa6eeu		/* Write */
#define DEST_X_END		0xaaeeu		/* Write */
#define DEST_Y_END		0xaeeeu		/* Write */
#define R_H_TOTAL_DISP		0xb2eeu		/* Read */	/* Mach32 */
#define SRC_X_START		0xb2eeu		/* Write */
#define R_H_SYNC_STRT		0xb6eeu		/* Read */	/* Mach32 */
#define ALU_BG_FN		0xb6eeu		/* Write */
#define R_H_SYNC_WID		0xbaeeu		/* Read */	/* Mach32 */
#define ALU_FG_FN		0xbaeeu		/* Write */
#define SRC_X_END		0xbeeeu		/* Write */
#define R_V_TOTAL		0xc2eeu		/* Read */
#define SRC_Y_DIR		0xc2eeu		/* Write */
#define R_V_DISP		0xc6eeu		/* Read */	/* Mach32 */
#define EXT_SHORT_STROKE	0xc6eeu		/* Write */
#define R_V_SYNC_STRT		0xcaeeu		/* Read */	/* Mach32 */
#define SCAN_X			0xcaeeu		/* Write */
#define VERT_LINE_CNTR		0xceeeu		/* Read */	/* Mach32 */
#define DP_CONFIG		0xceeeu		/* Write */
#define READ_WRITE			0x0001u
#define DATA_WIDTH			0x0200u
#define DATA_ORDER			0x1000u
#define FG_COLOR_SRC_FG			0x2000u
#define FG_COLOR_SRC_BLIT		0x6000u
#define R_V_SYNC_WID		0xd2eeu		/* Read */
#define PATT_LENGTH		0xd2eeu		/* Write */
#define PATT_INDEX		0xd6eeu		/* Write */
#define READ_SRC_X		0xdaeeu		/* Read */	/* Mach32 */
#define EXT_SCISSOR_L		0xdaeeu		/* Write */
#define READ_SRC_Y		0xdeeeu		/* Read */	/* Mach32 */
#define EXT_SCISSOR_T		0xdeeeu		/* Write */
#define EXT_SCISSOR_R		0xe2eeu		/* Write */
#define EXT_SCISSOR_B		0xe6eeu		/* Write */
/*	?			0xeaeeu */
#define DEST_COMP_FN		0xeeeeu		/* Write */
#define DEST_COLOR_CMP_MASK	0xf2eeu		/* Write */	/* Mach32 */
/*	?			0xf6eeu */
#define CHIP_ID			0xfaeeu		/* Read */	/* Mach32 */
#define CHIP_CODE_0			0x001fu			/* Mach32 */
#define CHIP_CODE_1			0x03e0u			/* Mach32 */
#define CHIP_CLASS			0x0c00u			/* Mach32 */
#define CHIP_REV			0xf000u			/* Mach32 */
#define LINEDRAW		0xfeeeu		/* Write */

/* ATI Mach64 register definitions */
#define CRTC_H_TOTAL_DISP	IOPortTag(0x00u, 0x00u)
#define CRTC_H_SYNC_STRT_WID	IOPortTag(0x01u, 0x01u)
#define CRTC_V_TOTAL_DISP	IOPortTag(0x02u, 0x02u)
#define CRCT_V_SYNC_STRT_WID	IOPortTag(0x03u, 0x03u)
#define CRTC_VLINE_CRNT_VLINE	IOPortTag(0x04u, 0x04u)
#define CRTC_OFF_PITCH		IOPortTag(0x05u, 0x05u)
#define CRTC_OFFSET			0x000ffffful
/*	?				0x00300000ul */
#define CRTC_PITCH			0xffc00000ul
#define CRTC_INT_CNTL		IOPortTag(0x06u, 0x06u)
#define CRTC_GEN_CNTL		IOPortTag(0x07u, 0x07u)
#define CRTC_DBL_SCAN_EN		0x00000001ul
#define CRTC_INTERLACE_EN		0x00000002ul
#define CRTC_HSYNC_DIS			0x00000004ul
#define CRTC_VSYNC_DIS			0x00000008ul
#define CRTC_CSYNC_EN			0x00000010ul
#define CRTC_PIX_BY_2_EN		0x00000020ul
#define CRTC_DISPLAY_DIS		0x00000040ul
#define CRTC_VGA_XOVERSCAN		0x00000080ul
#define CRTC_PIX_WIDTH			0x00000700ul
#define CRTC_BYTE_PIX_ORDER		0x00000800ul
/*	?				0x0000f000ul */
#define CRTC_FIFO_LWM			0x000f0000ul
/*	?				0x00f00000ul */
#define CRTC_EXT_DISP_EN		0x01000000ul
#define CRTC_EN				0x02000000ul
#define CRTC_DISP_REQ_EN		0x04000000ul
#define CRTC_VGA_LINEAR			0x08000000ul
#define CRTC_VSYNC_FALL_EDGE		0x10000000ul
#define CRTC_VGA_TEXT_132		0x20000000ul
#define CRTC_CNT_EN			0x40000000ul
#define CRTC_CUR_B_TEST			0x80000000ul
#define OVR_CLR			IOPortTag(0x08u, 0x10u)
#define OVR_WID_LEFT_RIGHT	IOPortTag(0x09u, 0x11u)
#define OVR_WID_TOP_BOTTOM	IOPortTag(0x0au, 0x12u)
#define CUR_CLR0		IOPortTag(0x0bu, 0x18u)
#define CUR_CLR1		IOPortTag(0x0cu, 0x19u)
#define CUR_OFFSET		IOPortTag(0x0du, 0x1au)
#define CUR_HORZ_VERT_POSN	IOPortTag(0x0eu, 0x1bu)
#define CUR_HORZ_VERT_OFF	IOPortTag(0x0fu, 0x1cu)
#define SCRATCH_REG0		IOPortTag(0x10u, 0x20u)
#define SCRATCH_REG1		IOPortTag(0x11u, 0x21u)
/*	BIOS_BASE_SEGMENT		0x0000007ful */	/* As above */
/*	?				0x00000f80ul */
#define BIOS_INIT_DAC_SUBTYPE		0x0000f000ul
/*	?				0xffff0000ul */
#define CLOCK_CNTL		IOPortTag(0x12u, 0x24u)
#define CLOCK_BIT			0x00000004ul	/* For ICS2595 */
#define CLOCK_PULSE			0x00000008ul	/* For ICS2595 */
#define CLOCK_SELECT			0x0000000ful
#define CLOCK_DIVIDER			0x00000030ul
#define CLOCK_STROBE			0x00000040ul
#define CLOCK_DATA			0x00000080ul
/*	?				0x00000100ul */
#define PLL_WR_EN			0x00000200ul	/* For internal PLL */
#define PLL_ADDR			0x00003c00ul	/* For internal PLL */
/*	?				0x0000c000ul */
#define PLL_DATA			0x00ff0000ul	/* For internal PLL */
/*	?				0xff000000ul */
#define BUS_CNTL		IOPortTag(0x13u, 0x28u)
#define BUS_WS				0x0000000ful
#define BUS_ROM_WS			0x000000f0ul
#define BUS_ROM_PAGE			0x00000f00ul
#define BUS_ROM_DIS			0x00001000ul
#define BUS_IO_16_EN			0x00002000ul
#define BUS_DAC_SNOOP_EN		0x00004000ul
/*	?				0x00008000ul */
#define BUS_FIFO_WS			0x000f0000ul
#define BUS_FIFO_ERR_INT_EN		0x00100000ul
#define BUS_FIFO_ERR_INT		0x00200000ul
#define BUS_HOST_ERR_INT_EN		0x00400000ul
#define BUS_HOST_ERR_INT		0x00800000ul
#define BUS_PCI_DAC_WS			0x07000000ul
#define BUS_PCI_DAC_DLY			0x08000000ul
#define BUS_PCI_MEMW_WS			0x10000000ul
#define BUS_PCI_BURST_DEC		0x20000000ul
#define BUS_RDY_READ_DLY		0xc0000000ul
#define MEM_INFO		IOPortTag(0x14u, 0x2cu)
#define CTL_MEM_SIZE			0x00000007ul
/*	?				0x00000008ul */
#define CTL_MEM_RD_LATCH_EN		0x00000010ul
#define CTL_MEM_RD_LATCH_DLY		0x00000020ul
#define CTL_MEM_SD_LATCH_EN		0x00000040ul
#define CTL_MEM_SD_LATCH_DLY		0x00000080ul
#define CTL_MEM_FULL_PLS		0x00000100ul
#define CTL_MEM_CYC_LNTH		0x00000600ul
#define CTL_MEM_REFRESH_RATE		0x00001800ul	/* Mach64xT */
#define CTL_MEM_WR_RDY_SEL		0x00000800ul	/* Mach64xX */
#define CTL_MEM_EXT_RMW_CYC_EN		0x00001000ul	/* Mach64xX */
/*	?				0x0000e000ul */
#define CTL_MEM_BNDRY			0x00030000ul
#define CTL_MEM_BNDRY_0K			0x00000000ul
#define CTL_MEM_BNDRY_256K			0x00010000ul
#define CTL_MEM_BNDRY_512K			0x00020000ul
#define CTL_MEM_BNDRY_1024K			0x00030000ul
#define CTL_MEM_BNDRY_EN		0x00040000ul
/*	?				0x00f80000ul */
#define CTL_MEM_PIX_WIDTH		0x07000000ul
/*	?				0xf8000000ul */
#define MEM_VGA_WP_SEL		IOPortTag(0x15u, 0x2du)
#define MEM_VGA_RP_SEL		IOPortTag(0x16u, 0x2eu)
#define DAC_REGS		IOPortTag(0x17u, 0x30u)	/* 4 separate bytes */
#define DAC_CNTL		IOPortTag(0x18u, 0x31u)
#define DAC_EXT_SEL			0x00000003ul
#define DAC_EXT_SEL_RS2				0x000000001ul
#define DAC_EXT_SEL_RS3				0x000000002ul
#define DAC_BLANKING			0x00000004ul	/* Mach64xT */
#define DAC_CMP_DIS			0x00000008ul	/* Mach64xT */
/*	?				0x00000070ul */
#define DAC_CMP_OUTPUT			0x00000080ul	/* Mach64xT */
#define DAC_8BIT_EN			0x00000100ul
#define DAC_PIX_DLY			0x00000600ul
#define DAC_BLANK_ADJ			0x00001800ul
#define DAC_VGA_ADR_EN			0x00002000ul
#define DAC_FEA_CON_EN			0x00004000ul	/* Mach64xT */
#define DAC_PDMN			0x00008000ul	/* Mach64xT */
#define DAC_TYPE			0x00070000ul
/*	?				0x00f80000ul */
#define DAC_MON_ID_STATE0		0x01000000ul	/* Mach64GX-E+/CX */
#define DAC_GIO_STATE_1			0x01000000ul	/* Mach64xT */
#define DAC_MON_ID_STATE1		0x02000000ul	/* Mach64GX-E+/CX */
#define DAC_GIO_STATE_0			0x02000000ul	/* Mach64xT */
#define DAC_MON_ID_STATE2		0x04000000ul	/* Mach64GX-E+/CX */
#define DAC_GIO_STATE_4			0x04000000ul	/* Mach64xT */
#define DAC_MON_ID_DIR0			0x08000000ul	/* Mach64GX-E+/CX */
#define DAC_GIO_DIR_1			0x08000000ul	/* Mach64xT */
#define DAC_MON_ID_DIR1			0x10000000ul	/* Mach64GX-E+/CX */
#define DAC_GIO_DIR_0			0x10000000ul	/* Mach64xT */
#define DAC_MON_ID_DIR2			0x20000000ul	/* Mach64GX-E+/CX */
#define DAC_GIO_DIR_4			0x20000000ul	/* Mach64xT */
#define DAC_MAN_CMP_STATE		0x40000000ul	/* Mach64GX-E+ */
/*	?				0x80000000ul */
#define GEN_TEST_CNTL		IOPortTag(0x19u, 0x34u)
#define GEN_EE_DATA_OUT			0x00000001ul	/* Mach64xX */
#define GEN_GIO2_DATA_OUT		0x00000001ul	/* Mach64xT */
#define GEN_EE_CLOCK			0x00000002ul	/* Mach64xX */
/*	?				0x00000002ul */	/* Mach64xT */
#define GEN_EE_CHIP_SEL			0x00000004ul	/* Mach64xX */
#define GEN_GIO3_DATA_OUT		0x00000004ul	/* Mach64xT */
#define GEN_EE_DATA_IN			0x00000008ul	/* Mach64xX */
#define GEN_GIO2_DATA_IN		0x00000008ul	/* Mach64xT */
#define GEN_EE_EN			0x00000010ul	/* Mach64xX */
#define GEN_GIO2_ENABLE			0x00000010ul	/* Mach64xT */
#define GEN_OVR_OUTPUT_EN		0x00000020ul	/* Mach64xX */
#define GEN_GIO2_WRITE			0x00000020ul	/* Mach64xT */
#define GEN_OVR_POLARITY		0x00000040ul	/* Mach64xX */
/*	?				0x00000040ul */	/* Mach64xT */
#define GEN_CUR_EN			0x00000080ul
#define GEN_GUI_EN			0x00000100ul	/* Mach64xX */
#define GEN_GUI_RESETB			0x00000100ul	/* Mach64xT */
#define GEN_BLOCK_WR_EN			0x00000200ul	/* Mach64GX */
/*	?				0x00000200ul */	/* Mach64CX/xT */
/*	?				0x0000fc00ul */
#define GEN_TEST_FIFO_EN		0x00010000ul	/* Mach64xX */
#define GEN_TEST_GUI_REGS_EN		0x00020000ul	/* Mach64xX */
#define GEN_TEST_VECT_EN		0x00040000ul	/* Mach64xX */
#define GEN_TEST_CRC_STR		0x00080000ul	/* Mach64GX-C/-D */
/*	?				0x00080000ul */	/* Mach64GX-E+/CX */
#define GEN_TEST_MODE_T			0x000f0000ul	/* Mach64xT */
#define GEN_TEST_MODE			0x00700000ul	/* Mach64xX */
#define GEN_TEST_CNT_EN			0x00100000ul	/* Mach64xT */
#define GEN_TEST_CRC_EN			0x00200000ul	/* Mach64xT */
/*	?				0x00400000ul */	/* Mach64xT */
/*	?				0x00800000ul */
#define GEN_TEST_MEM_WR			0x01000000ul	/* Mach64GX-C/-D */
#define GEN_TEST_MEM_STROBE		0x02000000ul	/* Mach64GX-C/-D */
#define GEN_TEST_DST_SS_EN		0x04000000ul	/* Mach64xX */
#define GEN_TEST_DST_SS_STROBE		0x08000000ul	/* Mach64xX */
#define GEN_TEST_SRC_SS_EN		0x10000000ul	/* Mach64xX */
#define GEN_TEST_SRC_SS_STROBE		0x20000000ul	/* Mach64xX */
#define GEN_TEST_CNT_VALUE		0x3f000000ul	/* Mach64xT */
#define GEN_TEST_CC_EN			0x40000000ul	/* Mach64xX */
#define GEN_TEST_CC_STROBE		0x80000000ul	/* Mach64xX */
/*	?				0xc0000000ul */	/* Mach64xT */
#define CONFIG_CNTL		IOPortTag(0x1au, 0x37u)
#define CFG_MEM_AP_SIZE			0x00000003ul
#define CFG_MEM_VGA_AP_EN		0x00000004ul
/*	?				0x00000008ul */
#define CFG_MEM_AP_LOC			0x00003ff0ul
/*	?				0x0000c000ul */
#define CFG_CARD_ID			0x00070000ul
#define CFG_VGA_DIS			0x00080000ul
/*	?				0xfff00000ul */
#define CONFIG_CHIP_ID		IOPortTag(0x1bu, 0x38u) /* Read */
#define CFG_CHIP_TYPE0			0x000000fful
#define CFG_CHIP_TYPE1			0x0000ff00ul
#define CFG_CHIP_TYPE			0x0000fffful
#define CFG_CHIP_CLASS			0x00ff0000ul
#define CFG_CHIP_REV			0xff000000ul
#define CONFIG_STATUS64_0	IOPortTag(0x1cu, 0x39u) /* Read (R/W (xT)) */
#define CFG_BUS_TYPE			0x00000007ul	/* Mach64xX */
#define CFG_MEM_TYPE_T			0x00000007ul	/* Mach64xT */
#define CFG_MEM_TYPE			0x00000038ul	/* Mach64xX */
#define CFG_DUAL_CAS_EN_T		0x00000008ul	/* Mach64xT */
/*	?				0x00000010ul */	/* Mach64xT */
#define CFG_CLOCK_EN			0x00000020ul	/* Mach64xT */
#define CFG_DUAL_CAS_EN			0x00000040ul	/* Mach64xX */
#define CFG_LOCAL_BUS_OPTION		0x00000180ul	/* Mach64xX */
#define CFG_INIT_DAC_TYPE		0x00000e00ul	/* Mach64xX */
#define CFG_INIT_CARD_ID		0x00007000ul	/* Mach64GX-C/-D */
#define CFG_BLK_WR_SIZE			0x00001000ul	/* Mach64GX-E+ */
#define CFG_INT_QSF_EN			0x00002000ul	/* Mach64GX-E+ */
/*	?				0x00004000ul */	/* Mach64GX-E+ */
/*	?				0x00007000ul */	/* Mach64CX */
#define CFG_TRI_BUF_DIS			0x00008000ul	/* Mach64xX */
#define CFG_EXT_RAM_ADDR		0x003f0000ul	/* Mach64xX */
#define CFG_ROM_DIS			0x00400000ul	/* Mach64xX */
#define CFG_VGA_EN			0x00800000ul	/* Mach64xX */
#define CFG_LOCAL_BUS_CFG		0x01000000ul	/* Mach64xX */
#define CFG_CHIP_EN			0x02000000ul	/* Mach64xX */
#define CFG_LOCAL_READ_DLY_DIS		0x04000000ul	/* Mach64xX */
#define CFG_ROM_OPTION			0x08000000ul	/* Mach64xX */
#define CFG_BUS_OPTION			0x10000000ul	/* Mach64xX */
#define CFG_LOCAL_DAC_WR_EN		0x20000000ul	/* Mach64xX */
#define CFG_VLB_RDY_DIS			0x40000000ul	/* Mach64xX */
#define CFG_AP_4GBYTE_DIS		0x80000000ul	/* Mach64xX */
/*	?				0xffffffc0ul */	/* Mach64xT */
#define	CONFIG_STATUS64_1	IOPortTag(0x1du, 0x3au) /* Read */
#define CFG_PCI_DAC_CFG			0x00000001ul	/* Mach64xX */
/*	?				0x0000001eul */	/* Mach64xX */
#define CFG_1C8_IO_SEL			0x00000020ul	/* Mach64xX */
/*	?				0xffffffc0ul */	/* Mach64xX */
#define CRC_SIG				0xfffffffful	/* Mach64xT */
/*	?			IOPortTag(0x1eu, ?) */
/*	CRTC_H_TOTAL_DISP	IOPortTag(0x1fu, 0x00u) */	/* Duplicate */

/* Definitions for internal PLL registers on a Mach64xT */
/*	?			0x00u */
#define PLL_MACRO_CNTL		0x01u
#define PLL_PC_GAIN			0x07u
#define PLL_VC_GAIN			0x18u
#define PLL_DUTY_CYCLE			0xe0u
#define PLL_REF_DIV		0x02u
#define PLL_GEN_CNTL		0x03u
#define PLL_OVERRIDE			0x01u
#define PLL_MCLK_RESET			0x02u
#define PLL_OSC_EN			0x04u
#define PLL_EXT_CLK_EN			0x08u
#define PLL_MCLK_POST_DIV		0x10u
#define PLL_MCLK_SRC_SEL		0x60u
#define PLL_EXT_CLK_CNTL		0x80u	/* CT/ET */
#define PLL_MCLK_FB_DIV		0x04u
#define PLL_VCLK_CNTL		0x05u
#define PLL_VCLK_SRC_SEL		0x03u
#define PLL_VCLK_RESET			0x04u
#define PLL_VCLK_INVERT			0x08u
#define PLL_ECP_DIV			0x30u	/* VT/GT */
#define PLL_ERATE_GT_XRATE		0x40u	/* VT/GT */
#define PLL_SCALER_LOCK_EN		0x80u	/* VT/GT */
#define PLL_VCLK_POST_DIV	0x06u
#define PLL_VCLK0_POST_DIV		0x03u
#define PLL_VCLK1_POST_DIV		0x0cu
#define PLL_VCLK2_POST_DIV		0x30u
#define PLL_VCLK3_POST_DIV		0xc0u
#define PLL_VCLK0_FB_DIV	0x07u
#define PLL_VCLK1_FB_DIV	0x08u
#define PLL_VCLK2_FB_DIV	0x09u
#define PLL_VCLK3_FB_DIV	0x0au
#define PLL_XCLK_CNTL		0x0bu		/* VT/GT */
#define PLL_XCLK_MCLK_RATIO		0x03u
#define PLL_MFB_TIMES_4_2B		0x04u
#define PLL_XCLK_MCKL_TST		0x08u
/*	?				0xf0u */
#define PLL_FCP_CNTL		0x0cu		/* VT/GT */
#define	PLL_FCP_POST_DIV		0x0fu
#define PLL_FCP_SRC_SEL			0x70u
#define PLL_DCLK_BY2_EN			0x80u
#define PLL_VFC_CNTL		0x0du		/* VT/GT */
#define PLL_DCLK_INVB			0x01u
#define PLL_DCLKBY2_EN			0x02u
#define PLL_VFC_2PHASE			0x04u
#define PLL_VFC_DELAY			0x18u
/*	?				0xe0u */
#define PLL_TEST_CNTL		0x0eu
#define	PLL_TST_SRC_SEL			0x1fu
#define PLL_TST_DIVIDERS		0x20u
#define PLL_TST_MASK_READ		0x40u
/*	?				0x80u */
#define PLL_TEST_COUNT		0x0fu

/* Miscellaneous */

/* Current X, Y & Dest X, Y mask */
#define COORD_MASK	0x07ffu

/* The Mixes */
#define	MIX_MASK			0x001fu

#define	MIX_NOT_DST			0x0000u
#define	MIX_0				0x0001u
#define	MIX_1				0x0002u
#define	MIX_DST				0x0003u
#define	MIX_NOT_SRC			0x0004u
#define	MIX_XOR				0x0005u
#define	MIX_XNOR			0x0006u
#define	MIX_SRC				0x0007u
#define	MIX_NAND			0x0008u
#define	MIX_NOT_SRC_OR_DST		0x0009u
#define	MIX_SRC_OR_NOT_DST		0x000au
#define	MIX_OR				0x000bu
#define	MIX_AND				0x000cu
#define	MIX_SRC_AND_NOT_DST		0x000du
#define	MIX_NOT_SRC_AND_DST		0x000eu
#define	MIX_NOR				0x000fu

#define	MIX_MIN				0x0010u
#define	MIX_DST_MINUS_SRC		0x0011u
#define	MIX_SRC_MINUS_DST		0x0012u
#define	MIX_PLUS			0x0013u
#define	MIX_MAX				0x0014u
#define	MIX_HALF__DST_MINUS_SRC		0x0015u
#define	MIX_HALF__SRC_MINUS_DST		0x0016u
#define	MIX_AVERAGE			0x0017u
#define	MIX_DST_MINUS_SRC_SAT		0x0018u
#define	MIX_SRC_MINUS_DST_SAT		0x001au
#define	MIX_HALF__DST_MINUS_SRC_SAT	0x001cu
#define	MIX_HALF__SRC_MINUS_DST_SAT	0x001eu
#define	MIX_AVERAGE_SAT			0x001fu
#define MIX_FN_PAINT			MIX_SRC

/* Wait until "n" queue entries are free */
#define ibm8514WaitQueue(_n)						\
	{								\
		while (inw(GP_STAT) & (0x0100U >> (_n)));		\
	}
#define ATIWaitQueue(_n)						\
	{								\
		while (inw(EXT_FIFO_STATUS) & (0x10000U >> (_n)));	\
	}

/* Wait until GP is idle and queue is empty */
#define WaitIdleEmpty()							\
	{								\
		while (inw(GP_STAT) & (GPBUSY | 1));			\
	}
#define ProbeWaitIdleEmpty()						\
	{								\
		int i;							\
		for (i = 0; i < 100000; i++)				\
			if (!(inw(GP_STAT) & (GPBUSY | 1)))		\
				break;					\
	}

/* Wait until GP has data available */
#define WaitDataReady()							\
	{								\
		while (!(inw(GP_STAT) & DATARDY));			\
	}

#endif /* _REGATI_H_ */

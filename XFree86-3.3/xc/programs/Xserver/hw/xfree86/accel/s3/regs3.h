/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/regs3.h,v 3.24.2.2 1997/05/09 07:15:13 hohndel Exp $ */
/*
 * regs3.h
 * 
 * Written by Jake Richter Copyright (c) 1989, 1990 Panacea Inc., Londonderry,
 * NH - All Rights Reserved
 * 
 * This code may be freely incorporated in any program without royalty, as long
 * as the copyright notice stays intact.
 * 
 * Additions by Kevin E. Martin (martin@cs.unc.edu)
 * 
 * KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * 
 */
/* $XConsortium: regs3.h /main/11 1996/10/25 10:22:07 kaleb $ */

#ifndef _REGS3_H
#define _REGS3_H

/* for OUT instructions */
#include "compiler.h"

/* for new trio64V+ and 968 mmio */
#include "newmmio.h"


/* S3 chipset definitions */

/*
 * Modified by Amancio Hasty and Jon Tombs
 * 
 * Id: reg8514.h,v 2.2 1993/06/22 20:54:09 jon Exp jon
 */

/*
 * S3 chipset definitions Provided by Jim Nichols <nichols@felix.cmd.usf.edu>
 * intended use: if (S3_801_928_SERIES(chip_id)) { <S3 801 and 928 specific
 * code here> } Note:  The S3 801 and 928 chipsets are very similar.  The
 * primary difference is that the 801 is DRAM based and the 928 is VRAM
 * based.  If you add code for the 801, chances are it will also work fine
 * with the 928, so please use S3_801_928_SERIES and have someone with a 928
 * test it. The 801 and the 805 appear to be indistinguishable from each
 * other using just the chip_id byte.  The 805 is the localbus and EISA
 * version of the 801. S3_911_SERIES will match both the 911 and 924 (as well
 * as any future versions of the 911/924 chips, but not the 801 or 928).
 */

/*
 * The early 928's have a nasty bug where memory access can alter io registers.
 * By locking them hopefully this won't happen.
 */

#define UNLOCK_SYS_REGS	          do { \
				   outb(vgaCRIndex, 0x39); \
				   outb(vgaCRReg, 0xa5); } while (0)

/*
 * I haven't had time to put the lock and unlocks in all the right places, for
 * now things will need to stay locked.
 */
#define LOCK_SYS_REGS	        /**/
#ifdef notyet
				 do { \
				   outb(vgaCRIndex, 0x39); \
				   outb(vgaCRReg, 0x50); } while (0)
#endif				   

#define S3_911_ONLY(chip)       (chip==0x81)
#define S3_924_ONLY(chip)       (chip==0x82)
#define S3_911_SERIES(chip)     ((chip&0xf0)==0x80)
#define S3_801_SERIES(chip)     ((chip&0xf0)==0xa0)
#define S3_805_I_SERIES(chip)   ((chip&0xf8)==0xa8)  /* (S3_801_SERIES(chip) && ((chip) & 0x08)) */
#define S3_801_REV_C(chip)      (S3_801_SERIES(chip) && ((chip) & 0x07) >= 2)
#define S3_928_P(chip)          ((chip&0xf0)==0xb0)
#define S3_928_ONLY(chip)       ((chip&0xd0)==0x90)  /* (((chip&0xf0)==0x90)||S3_928_P(chip)) */
#define S3_928_REV_E(chip)      (S3_928_ONLY(chip) && ((chip) & 0x0F) >= 4)
#define S3_801_928_SERIES(chip) (S3_801_SERIES(chip)||S3_928_ONLY(chip)||S3_x64_SERIES(chip))
#define S3_8XX_9XX_SERIES(chip) (S3_911_SERIES(chip)||S3_801_928_SERIES(chip))
#define S3_864_SERIES(chip)     ((chip&0xf0)==0xc0)
#define S3_964_SERIES(chip)     ((chip&0xf0)==0xd0)

#define S3_866_SERIES(chip)     (chip==PCI_868)
#define S3_868_SERIES(chip)     (chip==(PCI_868 | 0x10))
#define S3_86x_SERIES(chip)	((chip&0xf0)==0xc0 || (chip&0xffef)==PCI_868)
#undef S3_864_SERIES
#define S3_864_SERIES(chip)	S3_86x_SERIES(chip)
#define S3_968_SERIES(chip)     (chip==PCI_968)
#define S3_964_ONLY(chip)	S3_964_SERIES(chip)
#undef S3_964_SERIES
#define S3_964_SERIES(chip)     (((chip&0xf0)==0xd0) || S3_968_SERIES(chip))
#define S3_x66_SERIES(chip)     S3_866_SERIES(chip)
#define S3_x68_SERIES(chip)     ((chip&0xff9f)==0x8890)  /* ((S3_868_SERIES(chip) || S3_968_SERIES(chip)) */
#define S3_x6x_SERIES(chip)     ((chip&0xff8f)==0x8880)  /* ((S3_x66_SERIES(chip) || S3_x68_SERIES(chip)) */
#define S3_TRIO32_SERIES(chip)  (chip==(PCI_TRIO_32_64 & ~1))
#define S3_TRIO64_ONLY(chip)    (chip==PCI_TRIO_32_64 && !(s3ChipRev & 0x40))
#define S3_TRIO64V_SERIES(chip) (chip==PCI_TRIO_32_64 &&  (s3ChipRev & 0x40))
#define S3_TRIO64_SERIES(chip)  (chip==PCI_TRIO_32_64 || S3_TRIO64UVP_SERIES(chip) || S3_AURORA64VP_SERIES(chip) || S3_TRIO64V2_SERIES(chip) || S3_PLATO_PX_SERIES(chip))
#define S3_AURORA64VP_SERIES(chip)  (chip==PCI_AURORA64VP)
#define S3_TRIO64UVP_SERIES(chip)  (chip==PCI_TRIO64UVP)
#define S3_TRIO64V2_SERIES(chip)  (chip==PCI_TRIO64V2_DXGX)
#define S3_PLATO_PX_SERIES(chip)  (chip==PCI_PLATO_PX)
#define S3_TRIOxx_SERIES(chip)  (S3_TRIO32_SERIES(chip) || \
				 S3_TRIO64_SERIES(chip))
#define S3_ViRGE_SERIES(chip)      (chip==PCI_ViRGE)
#define S3_ViRGE_VX_SERIES(chip)   (chip==PCI_ViRGE_VX)
#define S3_ViRGE_DXGX_SERIES(chip) (chip==PCI_ViRGE_DXGX)
#define S3_ANY_ViRGE_SERIES(chip) (    S3_ViRGE_SERIES(chip)		\
				    || S3_ViRGE_VX_SERIES(chip)		\
				    || S3_ViRGE_DXGX_SERIES(chip))
#define S3_x64_SERIES(chip)	(((chip&0xe0)==0xc0) || S3_x6x_SERIES(chip) ||  S3_TRIOxx_SERIES(chip))
#define S3_928_SERIES(chip)     (S3_928_ONLY(chip) || S3_x64_SERIES(chip))
#define S3_ANY_SERIES(chip)     (S3_8XX_9XX_SERIES(chip) || S3_x64_SERIES(chip) || S3_x66_SERIES(chip) || S3_x68_SERIES(chip))

/* PCI data */
#define PCI_S3_VENDOR_ID	0x5333
#define PCI_TRIO_32_64		0x8811
#define PCI_AURORA64VP		0x8812
#define PCI_TRIO64UVP		0x8814
#define PCI_TRIO64V2_DXGX	0x8901
#define PCI_PLATO_PX		0x8902
#define PCI_928			0x88B0
#define PCI_864_0		0x88C0
#define PCI_864_1		0x88C1
#define PCI_868			0x8880
#define PCI_964_0		0x88D0
#define PCI_964_1		0x88D1
#define PCI_968			0x88F0
#define PCI_ViRGE		0x5631
#define PCI_ViRGE_VX		0x883d
#define PCI_ViRGE_DXGX		0x8A01

/* Chip tags */
#define S3_UNKNOWN		 0
#define	S3_911			 1
#define S3_924			 2
#define S3_801			 3
#define S3_805			 4
#define S3_928			 5
#define S3_TRIO_32_64		 6
#define S3_864			 7
#define S3_868			 8
#define S3_964			 9
#define S3_968			10
#define S3_TRIO32		11
#define S3_TRIO64		12
#define S3_TRIO64VP		13
#define S3_TRIO64UVP		14
#define S3_AURORA64VP		15
#define S3_TRIO64V2		16
#define S3_PLATO_PX		17
#define S3_ViRGE		18
#define S3_ViRGE_VX		19
#define S3_ViRGE_DXGX		20

/* VESA Approved Register Definitions */
#define	DAC_MASK	0x03c6
#define	DAC_R_INDEX	0x03c7
#define	DAC_W_INDEX	0x03c8
#define	DAC_DATA	0x03c9

#define	DISP_STAT	0x02e8
#define	H_TOTAL		0x02e8
#define	H_DISP		0x06e8
#define	H_SYNC_STRT	0x0ae8
#define	H_SYNC_WID	0x0ee8
#define	V_TOTAL		0x12e8
#define	V_DISP		0x16e8
#define	V_SYNC_STRT	0x1ae8
#define	V_SYNC_WID	0x1ee8
#define	DISP_CNTL	0x22e8
#if !defined(PC98_PW) && !defined(PC98_XKB) && !defined(PC98_NEC)
#define	ADVFUNC_CNTL	0x4ae8
#define	SUBSYS_STAT	0x42e8
#define	SUBSYS_CNTL	0x42e8
#define	ROM_PAGE_SEL	0x46e8
#define	CUR_Y		0x82e8
#define	CUR_X		0x86e8
#define	DESTY_AXSTP	0x8ae8
#define	DESTX_DIASTP	0x8ee8
#define	ERR_TERM	0x92e8
#define	MAJ_AXIS_PCNT	0x96e8
#define	GP_STAT		0x9ae8
#define	CMD		0x9ae8
#define	SHORT_STROKE	0x9ee8
#define	BKGD_COLOR	0xa2e8
#define	FRGD_COLOR	0xa6e8
#define	WRT_MASK	0xaae8
#define	RD_MASK		0xaee8
#define	COLOR_CMP	0xb2e8
#define	BKGD_MIX	0xb6e8
#define	FRGD_MIX	0xbae8
#define	MULTIFUNC_CNTL	0xbee8
#define	PIX_TRANS	0xe2e8
#define	PIX_TRANS_EXT	0xe2ea
#else /* PC98_PW || PC98_XKB || PC98_NEC */
#ifdef S3_MMIO
#define	ADVFUNC_CNTL	0x4a
#define	SUBSYS_STAT	0x42
#define	SUBSYS_CNTL	0x42
#define	ROM_PAGE_SEL	0x46
#define	CUR_Y		0x82e8
#define	CUR_X		0x86e8
#define	DESTY_AXSTP	0x8ae8
#define	DESTX_DIASTP	0x8ee8
#define	ERR_TERM	0x92e8
#define	MAJ_AXIS_PCNT	0x96e8
#define	GP_STAT		0x9a
#define	CMD		0x9ae8
#define	SHORT_STROKE	0x9ee8
#define	BKGD_COLOR	0xa2e8
#define	FRGD_COLOR	0xa6e8
#define	WRT_MASK	0xaae8
#define	RD_MASK		0xaee8
#define	COLOR_CMP	0xb2e8
#define	BKGD_MIX	0xb6e8
#define	FRGD_MIX	0xbae8
#define	MULTIFUNC_CNTL	0xbee8
#define	PIX_TRANS	0xe2e8
#define	PIX_TRANS_EXT	0xe2ea
#else	/*	regs3>>8	*/
#define ADVFUNC_CNTL    0x4a
#define SUBSYS_STAT     0x42
#define SUBSYS_CNTL     0x42
#define ROM_PAGE_SEL    0x46
#define CUR_Y           0x82
#define CUR_X           0x86
#define DESTY_AXSTP     0x8a
#define DESTX_DIASTP    0x8e
#define ERR_TERM        0x92
#define MAJ_AXIS_PCNT   0x96
#define GP_STAT         0x9a
#define CMD             0x9a
#define SHORT_STROKE    0x9e
#define BKGD_COLOR      0xa2
#define FRGD_COLOR      0xa6
#define WRT_MASK        0xaa
#define RD_MASK         0xae
#define COLOR_CMP       0xb2
#define BKGD_MIX        0xb6
#define FRGD_MIX        0xba
#define MULTIFUNC_CNTL  0xbe
#define	PIX_TRANS	0xe2
#define	PIX_TRANS_EXT	0xe2 /* (0xe2ea) not used? ignored for PC98 */ 
#endif /* S3MMIO */
#endif	/* PC98_PW || PC98_XKB || PC98_NEC */
#define	MIN_AXIS_PCNT	0x0000
#define	SCISSORS_T	0x1000
#define	SCISSORS_L	0x2000
#define	SCISSORS_B	0x3000
#define	SCISSORS_R	0x4000
#define	MEM_CNTL	0x5000
#define	PATTERN_L	0x8000
#define	PATTERN_H	0x9000
#define	PIX_CNTL	0xa000
#define	MULT_MISC2	0xd000
#define	MULT_MISC	0xe000
#define	READ_SEL	0xf000


/* Display Status Bit Fields */
#define	HORTOG		0x0004
#define	VBLANK		0x0002
#define	SENSE		0x0001

/* Horizontal Sync Width Bit Field */
#define HSYNCPOL_NEG	0x0020
#define	HSYNCPOL_POS	0x0000

/* Vertical Sync Width Bit Field */
#define	VSYNCPOL_NEG	0x0020
#define	VSYNCPOL_POS	0x0000

/* Display Control Bit Field */
#define	DISPEN_NC	0x0000
#define	DISPEN_DISAB	0x0040
#define	DISPEN_ENAB	0x0020
#define	INTERLACE	0x0010
#define	DBLSCAN		0x0008
#define	MEMCFG_2	0x0000
#define	MEMCFG_4	0x0002
#define	MEMCFG_6	0x0004
#define	MEMCFG_8	0x0006
#define	ODDBNKENAB	0x0001

/* Subsystem Status Register */
#define	_8PLANE		0x0080
#define	MONITORID_8503	0x0050
#define	MONITORID_8507	0x0010
#define	MONITORID_8512	0x0060
#define	MONITORID_8513	0x0060
#define	MONITORID_8514	0x0020
#define	MONITORID_NONE	0x0070
#define	MONITORID_MASK	0x0070
#define	GPIDLE		0x0008
#define	INVALIDIO	0x0004
#define	PICKFLAG	0x0002
#define	VBLNKFLG	0x0001

/* Subsystem Control Register */
#define	GPCTRL_NC	0x0000
#define	GPCTRL_ENAB	0x4000
#define	GPCTRL_RESET	0x8000
#define CHPTEST_NC	0x0000
#define CHPTEST_NORMAL	0x1000
#define CHPTEST_ENAB	0x2000
#define	IGPIDLE		0x0800
#define	IINVALIDIO	0x0400
#define	IPICKFLAG	0x0200
#define	IVBLNKFLG	0x0100
#define	RGPIDLE		0x0008
#define	RINVALIDIO	0x0004
#define	RPICKFLAG	0x0002
#define	RVBLNKFLG	0x0001

/* Current X, Y & Dest X, Y Mask */
#define	COORD_MASK	0x07ff

#ifdef CLKSEL
#undef CLKSEL
#endif

/* Advanced Function Control Regsiter */
#define	CLKSEL		0x0004
#define	DISABPASSTHRU	0x0001

/* Graphics Processor Status Register */
#define	GPBUSY		0x0200
#define	DATDRDY		0x0100

/* Command Register */
#define	CMD_NOP		0x0000
#define	CMD_LINE	0x2000
#define	CMD_RECT	0x4000
#define	CMD_RECTV1	0x6000
#define	CMD_RECTV2	0x8000
#define	CMD_LINEAF	0xa000
#define	CMD_BITBLT	0xc000
#define	CMD_OP_MSK	0xf000
#define	BYTSEQ		0x1000
#define	_16BIT		0x0200
#define	PCDATA		0x0100
#define	INC_Y		0x0080
#define	YMAJAXIS	0x0040
#define	INC_X		0x0020
#define	DRAW		0x0010
#define	LINETYPE	0x0008
#define	LASTPIX		0x0004
#define	PLANAR		0x0002
#define	WRTDATA		0x0001

/*
 * Short Stroke Vector Transfer Register (The angular Defs also apply to the
 * Command Register
 */
#define	VECDIR_000	0x0000
#define	VECDIR_045	0x0020
#define	VECDIR_090	0x0040
#define	VECDIR_135	0x0060
#define	VECDIR_180	0x0080
#define	VECDIR_225	0x00a0
#define	VECDIR_270	0x00c0
#define	VECDIR_315	0x00e0
#define	SSVDRAW		0x0010

/* Background Mix Register */
#define	BSS_BKGDCOL	0x0000
#define	BSS_FRGDCOL	0x0020
#define	BSS_PCDATA	0x0040
#define	BSS_BITBLT	0x0060

/* Foreground Mix Register */
#define	FSS_BKGDCOL	0x0000
#define	FSS_FRGDCOL	0x0020
#define	FSS_PCDATA	0x0040
#define	FSS_BITBLT	0x0060

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

/* Memory Control Register */
#define	BUFSWP		0x0010
#define	VRTCFG_2	0x0000
#define	VRTCFG_4	0x0004
#define	VRTCFG_6	0x0008
#define	VRTCFG_8	0x000C
#define	HORCFG_4	0x0000
#define	HORCFG_5	0x0001
#define	HORCFG_8	0x0002
#define	HORCFG_10	0x0003

/* Pixel Control Register */
#define	MIXSEL_FRGDMIX	0x0000
#define	MIXSEL_PATT	0x0040
#define	MIXSEL_EXPPC	0x0080
#define	MIXSEL_EXPBLT	0x00c0
#define COLCMPOP_F	0x0000
#define COLCMPOP_T	0x0008
#define COLCMPOP_GE	0x0010
#define COLCMPOP_LT	0x0018
#define COLCMPOP_NE	0x0020
#define COLCMPOP_EQ	0x0028
#define COLCMPOP_LE	0x0030
#define COLCMPOP_GT	0x0038
#define	PLANEMODE	0x0004

  typedef struct {
     unsigned char r, g, b;
#define       _16BIT          0x0200
#define       _32BIT          0x0400

  }
LUTENTRY;

/* Wait until "v" queue entries are free */
#define WaitQueue(v)  do { mem_barrier(); while (inb(GP_STAT) & (0x0100 >> (v))); } while(0)

/* x64: Wait until "v" queue entries are free, v>8 for 864/964 */
#define	WaitQueue16(v)	do { mem_barrier(); while (inw(GP_STAT) & (0x8000 >> (v-9))); } while(0)

/* Wait until GP is idle and queue is empty */
/* x64: bits 15-11 are reserved in 928 and should be zero,
        for 864/964 these are FIFO-STATUS bits 9-13 */
#define	WaitIdleEmpty() \
   do { int fx86=(S3_x64_SERIES(s3ChipId)?0xF800:0); mem_barrier(); while (inw(GP_STAT) & (GPBUSY | 1 | fx86)); } while (0)

/* Wait until GP is idle */
#define WaitIdle() do { mem_barrier(); while (inw(GP_STAT) & GPBUSY) ; } while (0)

#define	MODE_800	1
#define	MODE_1024	2
#define	MODE_1280	3
#define MODE_1152	4	/* 801/805 C-step, 928 E-step */
#define MODE_1600	5	/* 928 E-step */

#ifndef NULL
#define NULL	0
#endif

#define RGB8_PSEUDO      (-1)
#define RGB16_565         0
#define RGB16_555         1
#define RGB32_888         2

#endif /* _REGS3_H */

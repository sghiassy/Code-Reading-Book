/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/regs3v.h,v 3.4.2.1 1997/05/06 13:26:29 dawes Exp $ */
/*
 * regs3v.h
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
/* $XConsortium: regs3v.h /main/6 1996/10/27 18:06:49 kaleb $ */

#ifndef _REGS3V_H
#define _REGS3V_H

/* for OUT instructions */
#include "compiler.h"

/* for new trio64V+ and 968 mmio */
#include "newmmio.h"


/* S3 chipset definitions */


#define UNLOCK_SYS_REGS	          do { \
				   outb(vgaCRIndex, 0x39); \
				   outb(vgaCRReg, 0xa5); } while (0)


#define S3_ViRGE_SERIES(chip)      (chip==PCI_ViRGE)
#define S3_ViRGE_VX_SERIES(chip)   (chip==PCI_ViRGE_VX)
#define S3_ViRGE_DXGX_SERIES(chip) (chip==PCI_ViRGE_DXGX)
#define S3_ANY_ViRGE_SERIES(chip) (    S3_ViRGE_SERIES(chip)		\
				    || S3_ViRGE_VX_SERIES(chip)		\
				    || S3_ViRGE_DXGX_SERIES(chip))
#define S3_ANY_SERIES(chip)       (    S3_ANY_ViRGE_SERIES(chip)  )

/* PCI data */
#define PCI_S3_VENDOR_ID	0x5333
#define PCI_ViRGE		0x5631
#define PCI_ViRGE_VX		0x883D
#define PCI_ViRGE_DXGX		0x8A01

/* Chip tags */
#define S3_UNKNOWN		 0
#define S3_ViRGE		 1
#define S3_ViRGE_VX		 2
#define S3_ViRGE_DXGX		 3

/* VESA Approved Register Definitions */
#define	DAC_MASK	0x03c6
#define	DAC_R_INDEX	0x03c7
#define	DAC_W_INDEX	0x03c8
#define	DAC_DATA	0x03c9

#if 0 /* ==================== old S3 defs ==================== */

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

#endif  /* ============================== */

/* Subsystem Control Register */
#define	GPCTRL_NC	0x0000
#define	GPCTRL_ENAB	0x4000
#define	GPCTRL_RESET	0x8000


/* Command Register */
#define	CMD_OP_MSK	(0xf << 27)
#define	CMD_BITBLT	(0x0 << 27)
#define	CMD_RECT       ((0x2 << 27) | 0x0100)
#define	CMD_LINE	(0x3 << 27)
#define	CMD_POLYFILL	(0x5 << 27)
#define	CMD_NOP		(0xf << 27)

#define	BYTSEQ		0
#define	_16BIT		0
#define	PCDATA		0x80
#define	INC_Y		CMD_YP
#define	YMAJAXIS	0
#define	INC_X		CMD_XP
#define	DRAW		0x0020
#define	LINETYPE	0x0008
#define	LASTPIX		0
#define	PLANAR		0 /* MIX_MONO_SRC */
#define	WRTDATA		0

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

/* Command AutoExecute */
#define CMD_AUTOEXEC	0x01

/* Command Hardware Clipping Enable */
#define CMD_HWCLIP	0x02

/* Destination Color Format */
#define DST_8BPP	0x00
#define DST_16BPP	0x04
#define DST_24BPP	0x08

/* BLT Mix modes */
#define	MIX_BITBLT	0x0000
#define	MIX_MONO_SRC	0x0040
#define	MIX_CPUDATA	0x0080
#define	MIX_MONO_PATT	0x0100
#define	MIX_MONO_TRANSP	0x0200

/* Image Transfer Alignments */
#define CMD_ITA_BYTE	0x0000
#define CMD_ITA_WORD	0x0400
#define CMD_ITA_DWORD	0x0800

/* First Doubleword Offset (Image Transfer) */
#define CMD_FDO_BYTE0	0x0000
#define CMD_FDO_BYTE1	0x0100
#define CMD_FDO_BYTE2	0x0200
#define CMD_FDO_BYTE3	0x0400

/* X Positive, Y Positive (Bit BLT) */
#define CMD_XP		0x2000000
#define CMD_YP		0x4000000

/* 2D or 3D Select */
#define CMD_2D		0x00000000
#define CMD_3D		0x80000000

/* The Mix ROPs (selected ones, not all 256)  */

#define	ROP_0				(0x00<<17)
#define	ROP_DSon			(0x11<<17)
#define	ROP_DSna			(0x22<<17)
#define	ROP_Sn				(0x33<<17)
#define	ROP_SDna			(0x44<<17)
#define	ROP_Dn				(0x55<<17)
#define	ROP_DSx				(0x66<<17)
#define	ROP_DSan			(0x77<<17)
#define	ROP_DSa				(0x88<<17)
#define	ROP_DSxn			(0x99<<17)
#define	ROP_D				(0xaa<<17)
#define	ROP_DSno			(0xbb<<17)
#define	ROP_S				(0xcc<<17)
#define	ROP_SDno			(0xdd<<17)
#define	ROP_DSo				(0xee<<17)
#define	ROP_1				(0xff<<17)

/* ROP  ->  (ROP & P) | (D & ~P) */
#define	ROP_0PaDPnao    /* DPna     */	(0x0a<<17)
#define	ROP_DSonPaDPnao /* PDSPaox  */	(0x1a<<17)
#define	ROP_DSnaPaDPnao /* DPSana   */	(0x2a<<17)
#define	ROP_SnPaDPnao   /* SPDSxox  */	(0x3a<<17)
#define	ROP_SDnaPaDPnao /* DPSDoax  */	(0x4a<<17)
#define	ROP_DnPaDPnao   /* DPx      */	(0x5a<<17)
#define	ROP_DSxPaDPnao  /* DPSax    */	(0x6a<<17)
#define	ROP_DSanPaDPnao /* DPSDnoax */	(0x7a<<17)
#define	ROP_DSaPaDPnao  /* DSPnoa   */	(0x8a<<17)
#define	ROP_DSxnPaDPnao /* DPSnax   */	(0x9a<<17)
#define	ROP_DPaDPnao    /* D        */	(0xaa<<17)
#define	ROP_DSnoPaDPnao /* DPSnao   */	(0xba<<17)
#define	ROP_SPaDPnao    /* DPSDxax  */	(0xca<<17)
#define	ROP_SDnoPaDPnao /* DPSDanax */	(0xda<<17)
#define	ROP_DSoPaDPnao  /* DPSao    */  (0xea<<17)
#define	ROP_1PaDPnao    /* DPo      */	(0xfa<<17)


/* S -> P */
#define	ROP_DPon			(0x05<<17)
#define	ROP_DPna			(0x0a<<17)
#define	ROP_Pn				(0x0f<<17)
#define	ROP_PDna			(0x50<<17)
#define	ROP_DPx				(0x5a<<17)
#define	ROP_DPan			(0x5f<<17)
#define	ROP_DPa				(0xa0<<17)
#define	ROP_DPxn			(0xa5<<17)
#define	ROP_DPno			(0xaf<<17)
#define	ROP_P				(0xf0<<17)
#define	ROP_PDno			(0xf5<<17)
#define	ROP_DPo				(0xfa<<17)

typedef struct {
   unsigned char r, g, b;
}
LUTENTRY;

/* Wait until "v" queue entries are free */
#define	WaitQueue(v)	 do { mem_barrier(); while (((IN_SUBSYS_STAT()) & 0x1f00) < (((v)+2) << 8)); } while (0)

/* Wait until GP is idle and queue is empty */
#define	WaitIdleEmpty()  do { mem_barrier(); while ((IN_SUBSYS_STAT() & 0x3f00) != 0x3000); } while (0)

/* Wait until GP is idle */
#define WaitIdle()       do { mem_barrier(); while (!(IN_SUBSYS_STAT() & 0x2000)); } while (0)

#ifndef NULL
#define NULL	0
#endif

#define RGB8_PSEUDO      (-1)
#define RGB16_565         0
#define RGB16_555         1
#define RGB32_888         2

#endif /* _REGS3V_H */

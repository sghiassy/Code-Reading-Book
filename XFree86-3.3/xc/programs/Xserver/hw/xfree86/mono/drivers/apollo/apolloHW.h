/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/drivers/apollo/apolloHW.h,v 3.3 1996/12/23 06:47:54 dawes Exp $ */
/*
 * MONO: Driver family for interlaced and banked monochrome video adaptors
 * Pascal Haible 8/93, 3/94, 4/94 haible@IZFM.Uni-Stuttgart.DE
 *
 * mono/driver/apollo/apolloHW.h
 * Hamish Coleman 11/93 hamish@zot.apana.org.au 
 *
 * derived from:
 * bdm2/hgc1280/...
 * Pascal Haible 8/93, haible@izfm.uni-stuttgart.de
 *
 * see mono/COPYRIGHT for copyright and disclaimers.
 */
/* $XConsortium: apolloHW.h /main/3 1996/02/21 17:49:20 kaleb $ */

/* Ports */

#define AP_PORT_BASE		0x3b0
#define AP_PORT_ALTERNATE	0x3d0

/* Read  Byte  Status Reg */
#define AP_PORT_STATUS		(Apollo_IOBASE)
/* Write Word  Write Enable Reg */
#define AP_PORT_WRITE_ENABLE	(Apollo_IOBASE)
/* Read  Byte  Device ID reg */
#define AP_PORT_DEVICE_ID	(Apollo_IOBASE+0x01)
/* Write Word  Raster Operation Reg */
#define AP_PORT_ROP		(Apollo_IOBASE+0x02)
/* Write Word  Diagnostic Memory Request */
#define AP_PORT_DIAG_MEM_REQ	(Apollo_IOBASE+0x04)
/* Byte  Control Reg 0 - blit Mode & Shift reg */
#define AP_PORT_CONTROL_0	(Apollo_IOBASE+0x08)
/* Byte  Control Reg 1 - operation modes */
#define AP_PORT_CONTROL_1	(Apollo_IOBASE+0x0a)
/* Byte  Control Reg 2 - Plane acces mode */
#define AP_PORT_CONTROL_2	(Apollo_IOBASE+0x0c)
/* Byte  Control Reg 3 - Reset?? */
#define AP_PORT_CONTROL_3	(Apollo_IOBASE+0x0e)

/* CREG 0 settings */
#define AP_MODE_VECTOR		0x40	/* 010x.xxxx */
#define AP_MODE_DBLT		0x80	/* 100x.xxxx */
#define AP_MODE_NORMAL		0xE0	/* 111x.xxxx */
#define AP_SHIFT_BITS		0x1F	/* xxx1.1111 */

/* CREG 1 settings */
#define AP_BLT_NORMAL		0x80	/* 1xxx.xxxx */
#define AP_BLT_INVERSE		0x00	/* 0xxx.xxxx */
#define AP_BLT_HIBIT		0x40	/* x!xx.xxxx */
#define AP_ROP_ENABLE		0x10	/* xxx1.xxxx */
#define AP_ROP_DISABLE		0x00	/* xxx0.xxxx */
#define AP_SYNC_nRESET		0x08	/* xxxx.1xxx */
#define AP_SYNC_ENABLE		0x02	/* xxxx.xx1x */
#define AP_DISP_ENABLE		0x01	/* xxxx.xxx1 */
#define AP_DISP_DISABLE		0x00	/* xxxx.xxx0 */
#define AP_CREG1_NORMAL		(AP_SYNC_nRESET | AP_SYNC_ENABLE | AP_DISP_ENABLE)

#define AP_BIT_INV		(7<<1)
#define AP_BIT_HIBIT		(6<<1)
#define AP_BIT_ROP		(4<<1)
#define AP_BIT_RESET		(3<<1)
#define AP_BIT_SYNC		(1<<1)
#define AP_BIT_DISP		(0<<1)

#define AP_ON			1
#define AP_OFF			0

/* CREG 2 settings */
#define AP_DATA_1s		0x00	/* 00xx.xxxx */
#define AP_DATA_PIX		0x40	/* 01xx.xxxx */
#define AP_DATA_PLN		0xC0	/* 11xx.xxxx */

/* CREG 3 settings */
#define AP_RESET_CREG		0x80	/* 1000.0000 */

/* ROP's */
#define AP_ROP_SRC		0x3

/* ID Numbers */
#define AP_MONO_HIRES		0x09


/* Memory */

#define AP_MEM_BASE		(0xFA0000L)
#define AP_MEM_ALTERNATE	(0xA0000L)

#define AP_BANK_SIZE		(0x40000L)

/* banking disabled */
#define AP_BANK_BOTTOM		0
#define AP_BANK_TOP		0

#define AP_MAP_BASE		(AP_MEM_BASE)
#define AP_MAP_ALTERNATE	(AP_MEM_ALTERNATE)

#define AP_MAP_SIZE		(AP_BANK_SIZE)

/* 0 means banking disabled */
#define AP_SEGMENT_SIZE		0
#define AP_SEGMENT_SHIFT	0
#define AP_SEGMENT_MASK		0

#define AP_HDISPLAY		(1280)
#define AP_VDISPLAY		(1024)
#define AP_MAX_VIRTUAL_X	(1280)
#define AP_MAX_VIRTUAL_Y	(1024)
#define AP_SCAN_LINE_WIDTH	(2048)

#define apollo_Textmode		(1411)
#define apollo_Graphmode	(1412)

#define Word_Width		128
/* Kludge alert! - only for monochrome adaptors */

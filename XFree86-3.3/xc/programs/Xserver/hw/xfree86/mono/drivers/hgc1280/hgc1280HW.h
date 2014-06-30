/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/drivers/hgc1280/hgc1280HW.h,v 3.2 1996/12/23 06:48:19 dawes Exp $ */
/*
 * MONO: Driver family for interlaced and banked monochrome video adaptors
 * Pascal Haible 8/93, 3/94, 4/94 haible@IZFM.Uni-Stuttgart.DE
 *
 * mono/driver/hgc1280/hgc1280HW.h
 *
 * see mono/COPYRIGHT for copyright and disclaimers.
 */
/* $XConsortium: hgc1280HW.h /main/4 1996/02/21 17:49:41 kaleb $ */


#include "compiler.h"	/*	void outb(port,val);
				void outw(port,val);
				unsigned int inb(port);
				unsigned int inw(port);
				void intr_disable();
				void intr_enable();
				short port;
				short val;
			*/

/*
 * Primary I/O Base		0x3B0
 * Secondary I/O Base		0x390
 */

#define HGC_PRIM_PORT_BASE           0x3B0
#define HGC_PRIM_PORT_INDEX          0x3B0
#define HGC_PRIM_PORT_DATA           0x3B1
#define HGC_PRIM_PORT_CONTROL        0x3B8
#define HGC_PRIM_PORT_CRT_STATUS     0x3BA
#define HGC_PRIM_PORT_CONFIG         0x3BF

#define HGC_SEC_PORT_BASE           0x390
#define HGC_SEC_PORT_INDEX          0x390
#define HGC_SEC_PORT_DATA           0x391
#define HGC_SEC_PORT_CONTROL        0x398
#define HGC_SEC_PORT_CRT_STATUS     0x39A
#define HGC_SEC_PORT_CONFIG         0x39F

#define HGC_REG_BANK1           0x22
#define HGC_REG_BANK2           0x24
#define HGC_REG_SHIFT_DISPLAY   0x2D
#define HGC_REG_LEFT_BORDER     0x2A
#define HGC_REG_RIGHT_BORDER    0x2B


#define HGC_NUM_REGS		(0x40)

#define HGC_PRIM_MEM_BASE	(0xB0000L)
#define HGC_SEC_MEM_BASE	(0xC8000L)

#define HGC_BANK_SIZE		(0x4000L)

#define HGC_MEM_BASE_BANK1	(0L)
#define HGC_MEM_BASE_BANK2	(HGC_BANK_SIZE)

/* rel. to HGC_XXX_MAP_BASE */
#define HGC_BANK1_BOTTOM	(HGC_MEM_BASE_BANK1)
#define HGC_BANK1_TOP		(HGC_BANK1_BOTTOM+HGC_BANK_SIZE)
#define HGC_BANK2_BOTTOM	(HGC_MEM_BASE_BANK2)
#define HGC_BANK2_TOP		(HGC_BANK2_BOTTOM+HGC_BANK_SIZE)

#define HGC_PRIM_MAP_BASE	(HGC_PRIM_MEM_BASE)
#define HGC_SEC_MAP_BASE	(HGC_SEC_MEM_BASE)

#define HGC_MAP_SIZE		(2*HGC_BANK_SIZE)

#define HGC_SEGMENT_SIZE	(HGC_BANK_SIZE)
#define HGC_SEGMENT_SHIFT	(14)
#define HGC_SEGMENT_MASK	(0x3FFFL)

#define HGC_HDISPLAY		(1280)
#define HGC_VDISPLAY		(1024)
#define HGC_MAX_VIRTUAL_X	(1472)
#define HGC_MAX_VIRTUAL_Y	(1024)
#define HGC_SCAN_LINE_WIDTH	(2048)

/* HGC-1280 needs some delay between the setting of at least some of
 * the registers. I'm not sure if it is the usual I/O recovery or
 * something else. Anyway, on my setup I need both inb() when setting
 * the mode registers. Setting the banking regs seems to work without
 * this delay.
 */
#define IO_RECOVER_PRIM		{ volatile unsigned char io_recover_dummy; \
				  io_recover_dummy=inb(HGC_PRIM_PORT_INDEX);    \
				  io_recover_dummy=inb(HGC_PRIM_PORT_INDEX); }

#define IO_RECOVER_SEC		{ volatile unsigned char io_recover_dummy; \
				  io_recover_dummy=inb(HGC_SEC_PORT_INDEX);    \
				  io_recover_dummy=inb(HGC_SEC_PORT_INDEX); }

#define HGC_PRIM_SET_REG(reg,val) { outb(HGC_PRIM_PORT_INDEX,reg);	\
				    IO_RECOVER_PRIM;			\
				    outb(HGC_PRIM_PORT_DATA,val); }	\
				    IO_RECOVER_PRIM;

#define HGC_PRIM_GET_REG(reg,pval) { outb(HGC_PRIM_PORT_INDEX,reg);	\
				     IO_RECOVER_PRIM;			\
				     *(pval)=inb(HGC_PRIM_PORT_DATA); }	\
				     IO_RECOVER_PRIM;

#define HGC_SEC_SET_REG(reg,val)  { outb(HGC_SEC_PORT_INDEX,reg);	\
				    IO_RECOVER_SEC;			\
				    outb(HGC_SEC_PORT_DATA,val); }	\
				    IO_RECOVER_SEC;

#define HGC_SEC_GET_REG(reg,pval) { outb(HGC_SEC_PORT_INDEX,reg);	\
				     IO_RECOVER_SEC;			\
				     *(pval)=inb(HGC_SEC_PORT_DATA); }	\
				     IO_RECOVER_SEC;

#if 0
#define HGC_SHIFT_DISPLAY(offs)	{ HGC_SET_REG(45, 16 - ((offs)>>4) ) }
#endif

#if 0
#define HGC_SET_BANK1(b)	HGC_SET_REG(HGC_REG_BANK1,b)
#define HGC_SET_BANK2(b)	HGC_SET_REG(HGC_REG_BANK2,b)
#endif

#define HGC_PROBE_REG_RW	(56)
#define HGC_PROBE_VAL_WRITE	(0xFF)
#define HGC_PROBE_VAL_READ	(0xFF)
#define HGC_PROBE_VAL_RESET	(0x00)
#define HGC_PROBE_REG_FIX	(61)
#define HGC_PROBE_VAL_FIX1	(16)
#define HGC_PROBE_VAL_FIX2	(85)

#define HGC_Graphmode		(1407)
static unsigned char hgcRegsGraf1280x1024[HGC_NUM_REGS] = {
/***     0    1    2    3    4    5    6    7    8    9 ***/
/*0*/    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
/*1*/    0,   0,   0,   0, 255, 245,   0,   0,   0,   0,
/*2*/    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
/*3*/    0,   0, 214, 212,   0, 212,   1, 212,  10, 213,
/*4*/  107, 153, 138,  90,  33,  16, 191,   2,   9, 192,
/*5*/  224,  68,  65,   1,  65,  79,   0,  85,   3,   0,
/*6*/    0,  85,   3,   0 };

#define HGC_Textmode		(1408)
static unsigned char hgcRegsText[HGC_NUM_REGS] = {
/***     0    1    2    3    4    5    6    7    8    9 ***/
/*0*/    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
/*1*/   12,  14,   0,   0,   7, 128,   0,   0,   0,   0,
/*2*/    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
/*3*/    0,   0, 135, 132, 135, 132,   5, 132, 200, 134,
/*4*/  107, 153,  10,  90,  36,  74, 195,  98,  13,  34,
/*5*/   28,  64,  30,   2,  27, 143,   0,  85,   3,   0,
/*6*/    0,  85,   3,   0 };

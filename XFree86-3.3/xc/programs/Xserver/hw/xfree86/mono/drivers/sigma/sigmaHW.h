/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/drivers/sigma/sigmaHW.h,v 3.3 1996/12/23 06:48:29 dawes Exp $ */
/*
 * MONO: Driver family for interlaced and banked monochrome video adaptors
 * Pascal Haible 8/93, 3/94, 4/94 haible@IZFM.Uni-Stuttgart.DE
 *
 * mono/driver/sigma/sigmaHW.c
 * Register definitions for Sigma L-View and Sigma LaserView PLUS
 *
 * see mono/COPYRIGHT for copyright and disclaimers.
 */
/* $XConsortium: sigmaHW.h /main/3 1996/02/21 17:49:52 kaleb $ */

/* Many thanks to Rich Murphey (rich@rice.edu) who sent me the docs */


/*
 * Name   r/w I/O  Bit Reset Description
 * -------------------------------------
 * Expanded Memory System Window Address Registers
 * EN1    r/w 0249 D7  0     EMS Frame Access Enable
 * W16    r/w 4249 D7  0     EMS Frame Pointer A16 (Location of 64k Frame)
 * W17    r/w 8249 D7  0     EMS Frame Pointer A17 (8000:0000 - E000:0000)
 * W18    r/w C249 D7  0     EMS Frame Pointer A18 (on 64k boundarys)
 * High Resolution Mode Control Registers
 * BLANK  r/w 0649 D7  0     HiRes Video Display Enable
 * ZOOM   r/w 4649 D7  0     Multiplane Write Enable
 * GR0    r/w 8649 D7  0     Gray 0
 * GR1    r/w C649 D7  0     Gray 1
 * Expanded Memory System Window Page Registers
 * BANK0  r/w 0248 D7  x     16k Page at EMS Frame Pointer +  0k
 * BANK1  r/w 4248 D7  x     16k Page at EMS Frame Pointer + 16k
 * BANK2  r/w 8248 D7  x     16k Page at EMS Frame Pointer + 32k
 * BANK3  r/w C248 D7  x     16k Page at EMS Frame Pointer + 48k
 * Emulation Control/Configuration Register
 * HIRES  r/w 0A48 D7  0     Emulation Display Disable
 * MONOEN ro  0A48 D6        Emulation Hardware Enable (Jumper)
 * Emulation Mode Control Register
 * BOLD   r/w 8A48 D1  0     Dither/Bold Text Intensity Representation
 * WOB    r/w 8A48 D0  0     Black Text on White Background/White Text on
 *                           Black Background
 */

#define SLV_EN1    0x0249
#define SLV_W16    0x4249
#define SLV_W17    0x8249
#define SLV_W18    0xC249
#define SLV_BLANK  0x0649
#define SLV_ZOOM   0x4649
#define SLV_GR0    0x8649
#define SLV_GR1    0xC649
#define SLV_BANK0  0x0248
#define SLV_BANK1  0x4248
#define SLV_BANK2  0x8248
#define SLV_BANK3  0xC248
#define SLV_HIRES  0x0A48
#define SLV_MONOEN 0x0A48
#define SLV_BOLD   0x8A48
#define SLV_WOB    0x8A48


/* Memory Base Address */
/* According to the docs, this can be anywhere (on a 64k boundary)
 * in the second half MB, but show me the PC running *N*X where
 * 0x8000, 0x9000 or 0xF0000 is available!
 * On Systems without VGA, 0xA0000, 0xB0000 and 0xC0000 is available */
/* So: valid choices for mono are 0x{ABCDE}0000L */
/* 0xE0000 is the default, other values are selected in XF86Config
 * with ' membase "xxxx" ' */
#define SIGMA_DEF_MEM_BASE	(0xE0000L)


/* Sigma LaserView [PLUS] has its video mem 'high address aligned',
 * the first line is line 848. Unfortunately, this is not on a bank boundary.
 * Following trick to cope with this:
 * first two banking windows (out of four) for the first 'logical' bank:
 *       16k from SIGMA_MEM_BASE+4k to SIGMA_MEM_BASE+20k.
 * Quite ugly: this way I have to bank two times 16k to have once 16k;
 * 'logical' banks do not match real banks.
 */

#define SIGMA_BANK_SIZE		(0x4000L)	/* 16k */

#define SIGMA_DEF_MAP_BASE	(SIGMA_DEF_MEM_BASE)

#define SIGMA_MAP_SIZE		(4*SIGMA_BANK_SIZE)

/* alignement gap at the beginning of the line, in bytes */
#define SIGMA_X_GAP		(48)

/* alignement gap between the beginning of the 'logical' bank 0
 *	(real bank 13) and the first visible line (16 lines), in bytes
 */
#define SIGMA_Y_GAP		(0x1000L)	/* 4k */

/* rel. to SIGMA_MAP_BASE */
#define SIGMA_MEM_BASE_BANK1	(SIGMA_Y_GAP + SIGMA_X_GAP)
#define SIGMA_MEM_BASE_BANK2	(2*SIGMA_BANK_SIZE+SIGMA_Y_GAP+SIGMA_X_GAP)
#define SIGMA_BANK1_BOTTOM	(SIGMA_MEM_BASE_BANK1)
#define SIGMA_BANK1_TOP		(SIGMA_BANK1_BOTTOM+SIGMA_BANK_SIZE)
#define SIGMA_BANK2_BOTTOM	(SIGMA_MEM_BASE_BANK2)
#define SIGMA_BANK2_TOP		(SIGMA_BANK2_BOTTOM+SIGMA_BANK_SIZE)

#define SIGMA_SEGMENT_SIZE	(SIGMA_BANK_SIZE)
#define SIGMA_SEGMENT_SHIFT	(14)		/* 16k */
#define SIGMA_SEGMENT_MASK	(0x3FFFL)

#define SIGMA_HDISPLAY		(1664)
#define SIGMA_VDISPLAY		(1200)

#define SIGMA_SCAN_LINE_WIDTH	(2048)

#define sigma_Textmode		(1409)
#define sigma_Graphmode		(1410)

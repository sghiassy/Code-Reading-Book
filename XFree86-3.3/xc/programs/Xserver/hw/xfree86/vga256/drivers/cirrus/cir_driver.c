/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_driver.c,v 3.80.2.8 1997/05/21 15:02:44 dawes Exp $ */
/*
 * cir_driver.c,v 1.10 1994/09/14 13:59:50 scooper Exp
 *
 * Copyright 1993 by Bill Reynolds, Santa Fe, New Mexico
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Bill Reynolds not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Bill Reynolds makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * BILL REYNOLDS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL BILL REYNOLDS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Bill Reynolds, bill@goshawk.lanl.gov
 * Modifications: David Dawes, <dawes@physics.su.oz.au>
 * Modifications: Piercarlo Grandi, Aberystwyth (pcg@aber.ac.uk)
 * Modifications: Simon P. Cooper, <scooper@vizlab.rutgers.edu>
 * Modifications: Wolfgang Jung, <wong@cs.tu-berlin.de>
 * Modifications: Harm Hanemaayer, <hhanemaa@cs.ruu.nl>
 * Modifications: Corin Anderson, <corina@bdc.cirrus.com>
 *
 */
/* $XConsortium: cir_driver.c /main/28 1996/10/27 11:07:18 kaleb $ */

/* 
 * Modifications to this file for the Cirrus 62x5 chips and color LCD
 * displays were made by Prof. Hank Dietz, Purdue U. School of EE, W.
 * Lafayette, IN, 47907-1285.  These modifications were made very
 * quickly and tested only on a Sager 8200 laptop running Linux SLS
 * 1.03, where they appear to work.  In any case, both Hank and Purdue
 * offer these modifications with the same disclaimers and conditions
 * invoked by Bill Reynolds above:  use these modifications at your
 * own risk and don't blame us.  Neither should you infer that Purdue
 * endorses anything.
 *
 *					hankd@ecn.purdue.edu
 */

/*
 * Note: defining ALLOW_OUT_OF_SPEC_CLOCKS will allow the driver to program
 * clock frequencies higher than those recommended in the Cirrus data book.
 * If you enable this, you do so at your OWN risk, and YOU RISK DAMAGING
 * YOUR HARDWARE.  You have been warned.
 */

#undef ALLOW_OUT_OF_SPEC_CLOCKS
#ifdef ALLOW_OUT_OF_SPEC_CLOCKS
#define MAX_OUT_OF_SPEC_CLOCK	100500
#endif

/* Allow pixel multiplexing for the 5434 in 256 color modes to support */
/* dot clocks up to 110 MHz (later chip versions may go up to 135 MHz). */

#define ALLOW_8BPP_MULTIPLEXING


/* Allow optional Memory-Mapped I/O on 543x. */

#if defined(__GNUC__) || defined(__STDC__)

#define CIRRUS_SUPPORT_MMIO

#endif

/* Allow optional linear addressing. */

#define CIRRUS_SUPPORT_LINEAR

/*
 * If this is defined, 16bpp does not use VCLK at pixel rate mode,
 * but rather VCLK * 2 = pixel rate, on a 1MB 5434, fixing severe
 * screen refresh problems. This reduces the max allowed dot clock
 * from about 45-50 MHz to 40 MHz. The only explanation seems to be
 * a hardware problem. I don't know whether this applies for any other
 * chips, such as the CL-GD5430.
 */

#define DOUBLE_16BPP_VCLK_FOR_1MB_5434

#ifdef DOUBLE_16BPP_VCLK_FOR_1MB_5434
#define MUST_DOUBLE_VCLK_FOR_16BPP (cirrusChip <= CLGD5424 || \
    (cirrusChip == CLGD5434 && vga256InfoRec.videoRam == 1024))
#else
#define MUST_DOUBLE_VCLK_FOR_16BPP (cirrusChip <= CLGD5424)
#endif


#include "X.h"
#include "input.h"
#include "screenint.h"
#include "dix.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Procs.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "vga.h"
#include "region.h"
#include "vgaPCI.h"

#ifdef XFreeXDGA
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif


#ifdef XF86VGA16
#define MONOVGA
#endif

static void cirrusHack1024Mode(void);



/*
 * This driver supports 16-color (mono, vga16), and in the SVGA server
 * 8bpp, 16bpp, 24bpp and 32bpp depending on vgaBitsPerPixel.
 */

#include "cir_driver.h"
#include "cir_alloc.h"
#ifndef MONOVGA
#include "cir_blitLG.h"
#include "vga256.h"
#endif

int cirrusChip;
int cirrusChipRevision;
int cirrusBusType;
Bool cirrusUseBLTEngine = FALSE;
Bool cirrusUseMMIO = FALSE;
Bool cirrusMMIOFlag = FALSE;
unsigned char *cirrusMMIOBase = NULL;
Bool cirrusUseLinear = FALSE;
Bool cirrusFavourBLT = FALSE;
Bool cirrusAvoidImageBLT = FALSE;
Bool cirrus128KDevices = FALSE;
int cirrusDRAMBandwidth;
int cirrusDRAMBandwidthLimit;
int cirrusReprogrammedMCLK = 0;
unsigned char cirrusLgBCLK = 0; /* Used by 546X chips */
int cirrusTilesPerLineIndex; /* Used by 546X chips */
int cirrusMemoryInterleave; /* Used by 546X chips */
int cirrusLgCursorXOffset;  /* Used by 546X chips */
int cirrusLCDVerticalSize; /* Used for 754X. */
int cirrusBufferSpaceAddr;
int cirrusBufferSpaceSize;

#define CLAVGA2_ID  0x06
#define CLGD5420_ID 0x22
#define CLGD5422_ID 0x23
#define CLGD5424_ID 0x25
#define CLGD5426_ID 0x24
#define CLGD5428_ID 0x26
#define CLGD5429_ID 0x27
#define CLGD6205_ID 0x02
#define CLGD6215_ID 0x22  /* Hmmm... looks like a 5420 or 5422 */
#define CLGD6225_ID 0x32
#define CLGD6235_ID 0x06	/* It's not 0x12. */
				/* XXXX need to add 6245. */
#define CLGD5434_OLD_ID 0x29
#define CLGD5434_ID 0x2A	/* CL changed the ID at the last minute. */
#define CLGD5430_ID 0x28
#define CLGD5436_ID 0x2B

#define CLGD5446_ID 0x2E
#define CLGD5480_ID 0x2F

#define CLGD7541_ID 0x0a	/* guess */
#define CLGD7542_ID 0x0b
#define CLGD7543_ID 0x0c
#define CLGD7548_ID 0x37

#define Is_62x5(x)  ((x) >= CLGD6205 && (x) <= CLGD6235)
#define Is_75xx(x)  ((x) >= CLGD7541 && (x) <= CLGD7555)

/* <scooper>
 * The following will need updating for other chips in the cirrus
 * family that support a hardware cursor.  I only have data for the 542x
 * series.
 * The 543x should be compatible. -HH
 */

/* The Laguna chip has a HW cursor, but I don't want to support it just
   yet.  Let's get some pixels up on the screen first, then we'll talk about
   how to make the cursor fast!  */
#define Has_HWCursor(x) (((x) >= CLGD5422 && (x) <= CLGD5429) || \
			 ((x) == CLGD5430 || (x) == CLGD5434 || \
			  (x) == CLGD5436) || \
			 (x) == CLGD5446 || (x) == CLGD5480 || \
			 ((x) == CLGD5462 || (x) == CLGD5464) || \
			 (x) == CLGD5465)

#define HasLargeHWCursor(x) ((x) == CLGD5462 || (x) == CLGD5464 || \
			     (x) == CLGD5465)
			 

/* Define a structure for the HIDDEN DAC cursor colours */
typedef struct {
  unsigned char red;		/* DAC red */
  unsigned char green;		/* DAC green */
  unsigned char blue;		/* DAC blue */
} DACcolourRec;
				/* For now, only save a couple of the */
				/* extensions. */
typedef struct {
  vgaHWRec std;               /* good old IBM VGA */
  unsigned char GR9;		/* Graphics Offset1 */
  unsigned char GRA;		/* Graphics Offset2 */
  unsigned char GRB;		/* Graphics Extensions Control */
  unsigned char GRF;		/* Display Compression Control */
  unsigned char SR7;		/* Extended Sequencer */
  unsigned char SRE;		/* VCLK Numerator (546X's denominator) */
  unsigned char SRF;		/* DRAM Control */
  unsigned char SR10;		/* Graphics Cursor X Position [7:0]         */
  unsigned char SR10E;		/* Graphics Cursor X Position [7:5] | 10000 */
  unsigned char SR11;		/* Graphics Cursor Y Position [7:0]         */
  unsigned char SR11E;		/* Graphics Cursor Y Position [7:5] | 10001 */
  unsigned char SR12;		/* Graphics Cursor Attributes Register */
  unsigned char SR13;		/* Graphics Cursor Pattern Address */
  unsigned char SR16;		/* Performance Tuning Register */
  unsigned char SR17;		/* Configuration/Extended Control Register */
  unsigned char SR1E;		/* VCLK Denominator (546X's numerator) */
  unsigned char SR1F;		/* MCLK Register */
  unsigned char CR19;		/* Interlace End */
  unsigned char CR1A;		/* Miscellaneous Control */
  unsigned char CR1B;		/* Extended Display Control */
  unsigned char CR1D;		/* Overlay Extended Control Register */
  unsigned char CR1E;           /* CRTC Timing Overflow (546X) */
  unsigned char CR2A;		/* 754x */
  unsigned char CR2B;		/* 754x */
  unsigned char CR2C;		/* 754x */
  unsigned char CR2D, CR2E;	/* 754x */
  unsigned char RAX;		/* 62x5 LCD Timing -- TFT HSYNC */
  unsigned char HIDDENDAC;	/* Hidden DAC Register */
  DACcolourRec  FOREGROUND;     /* Hidden DAC cursor foreground colour */
  DACcolourRec  BACKGROUND;     /* Hidden DAC cursor background colour */
#ifdef FORMAT
#undef FORMAT
#endif
  unsigned short FORMAT;        /* Video format (546X) */
  unsigned long VSC;            /* Vendor specific control (546X) */
  unsigned short DTTC;          /* Display Threshold and Tiling Control */
  unsigned short TileCtrl;      /* Tiling control (5465) */
  unsigned char TILE;           /* Tile control (546X) */
  unsigned short CONTROL;       /* Control (546X) */
  unsigned char BCLK;           /* Rambus clock (546X) */
  unsigned short cursorX;
  unsigned short cursorY;
} vgacirrusRec, *vgacirrusPtr;

unsigned char SavedExtSeq;

static Bool lcd_is_on = FALSE;	/* for 62x5 */

static Bool     cirrusProbe();
static char *   cirrusIdent();
static Bool     cirrusClockSelect();
static void     cirrusEnterLeave();
static Bool     cirrusInit();
static int      cirrusValidMode();
static void *   cirrusSave();
static void     cirrusRestore();
static Bool     cirrusScreenInit();
static void     cirrusAdjust();
static int      cirrusFindPitchPadding();
static int      cirrusPitchAdjust();
static void	cirrusFbInit();
static void CirrusDisplayPowerManagementSet(int PowerManagementMode);

extern void     cirrusSetRead();
extern void     cirrusSetWrite();
extern void     cirrusSetReadWrite();

extern void     cirrusSetRead2MB();
extern void     cirrusSetWrite2MB();
extern void     cirrusSetReadWrite2MB();

extern Bool 	cirrusCursorInit();
extern void 	cirrusRestoreCursor();
extern void 	cirrusWarpCursor();
extern void 	cirrusQueryBestSize();

extern vgaHWCursorRec vgaHWCursor;
extern cirrusCurRec cirrusCur;

int	CirrusMemTop;

#ifdef PC98
extern void crtswitch();
#endif

vgaVideoChipRec CIRRUS = {
#if !defined(PC98_WAB) && !defined(PC98_WABEP)
  cirrusProbe,			/* ChipProbe()*/
  cirrusIdent,			/* ChipIdent(); */
  cirrusEnterLeave,		/* ChipEnterLeave() */
  cirrusInit,			/* ChipInit() */
  cirrusValidMode,		/* ChipValidMode() */
  cirrusSave,			/* ChipSave() */
  cirrusRestore,		/* ChipRestore() */
  cirrusAdjust,			/* ChipAdjust() */
  vgaHWSaveScreen,		/* ChipSaveScreen() */
  (void (*)())NoopDDA,		/* ChipGetMode() */
  cirrusFbInit,			/* ChipFbInit() */
  cirrusSetRead,		/* ChipSetRead() */
  cirrusSetWrite,		/* ChipSetWrite() */
  cirrusSetReadWrite,	        /* ChipSetReadWrite() */
  0x10000,			/* ChipMapSize */
  0x08000,			/* ChipSegmentSize, 32k*/
  15,				/* ChipSegmentShift */
  0x7FFF,			/* ChipSegmentMask */
  0x00000, 0x08000,		/* ChipReadBottom, ChipReadTop  */
  0x08000, 0x10000,		/* ChipWriteBottom,ChipWriteTop */
  TRUE,				/* ChipUse2Banks, Uses 2 bank */
  VGA_DIVIDE_VERT,		/* ChipInterlaceType -- don't divide verts */
  {0,},				/* ChipOptionFlags */
  8,				/* ChipRounding */
  FALSE,			/* ChipUseLinearAddressing */
  0,				/* ChipLinearBase */
  0x100000,			/* ChipLinearSize */
  TRUE,				/* ChipHas16bpp */
  TRUE,				/* ChipHas24bpp */
  TRUE,				/* ChipHas32bpp */
  NULL,				/* ChipBuiltinModes */
  1,				/* ChipClockMulFactor */
  1				/* ChipClockDivFactor */
#else
  cirrusProbe,			/* ChipProbe()*/
  cirrusIdent,			/* ChipIdent(); */
  cirrusEnterLeave,		/* ChipEnterLeave() */
  cirrusInit,			/* ChipInit() */
  cirrusValidMode,		/* ChipValidMode() */
  cirrusSave,			/* ChipSave() */
  cirrusRestore,		/* ChipRestore() */
  cirrusAdjust,			/* ChipAdjust() */
  (void (*)())NoopDDA,		/* ChipSaveScreen() */
  (void (*)())NoopDDA,		/* ChipGetMode() */
  cirrusFbInit,			/* ChipFbInit() */
  cirrusSetRead,		/* ChipSetRead() */
  cirrusSetWrite,		/* ChipSetWrite() */
  cirrusSetReadWrite,	        /* ChipSetReadWrite() */
#ifndef CIRRUS_SUPPORT_MMIO  
  0x08000,			/* ChipMapSize */
#else
  0x20000,
#endif
  0x04000,			/* ChipSegmentSize, 16k*/
  14,				/* ChipSegmentShift */
  0x3FFF,			/* ChipSegmentMask */
  0x00000, 0x04000,		/* ChipReadBottom, ChipReadTop  */
  0x04000, 0x08000,		/* ChipWriteBottom,ChipWriteTop */
  TRUE,				/* ChipUse2Banks, Uses 2 bank */
  VGA_DIVIDE_VERT,		/* ChipInterlaceType -- don't divide verts */
  {0,},				/* ChipOptionFlags */
  /*8*/1,				/* ChipRounding */
  FALSE,			/* ChipUseLinearAddressing */
  0,				/* ChipLinearBase */
  0x10000,			/* ChipLinearSize */
  TRUE,				/* ChipHas16bpp */
  TRUE,				/* ChipHas24bpp */
  TRUE,				/* ChipHas32bpp */
  NULL,				/* ChipBuiltinModes */
  1,				/* ChipClockMulFactor */
  1				/* ChipClockDivFactor */
#endif
};

/*
 * This exists only to force some OS's not to use a 386 I/O bitmap for
 * I/O protection, which is MUCH slower than full I/O permissions.
 */
static unsigned Cirrus_IOPorts[] = { 0x400 };

/*
 * Note: To be able to use 16K bank granularity, we would have to half the
 * read and write window sizes, because (it seems) cfb.banked can't handle
 * a bank granularity different from the segment size.
 * This means that we have to define a seperate set of banking routines in
 * accel functions where the 16K hardware granularity is used.
 */
int cirrusBankShift = 10;

typedef struct {
  unsigned char numer;
  unsigned char denom;
  } cirrusClockRec;

static cirrusClockRec cirrusClockTab[] = {
  { 0x4A, 0x2B },		/* 25.227 */
  { 0x5B, 0x2F },		/* 28.325 */
  { 0x45, 0x30 }, 		/* 41.164 */
  { 0x7E, 0x33 },		/* 36.082 */
  { 0x42, 0x1F },		/* 31.500 */
  { 0x51, 0x3A },		/* 39.992 */
  { 0x55, 0x36 },		/* 45.076 */
  { 0x65, 0x3A },		/* 49.867 */
  { 0x76, 0x34 },		/* 64.983 */
  { 0x7E, 0x32 },		/* 72.163 */
  { 0x6E, 0x2A },		/* 75.000 */
  { 0x5F, 0x22 },		/* 80.013 */   /* all except 5420 */
  { 0x7D, 0x2A },		/* 85.226 */   /* 5426 and 5428 */
  /* These are all too high according to the databook.  They can be enabled
     with the "16clocks" option  *if* this driver has been compiled with
     ALLOW_OUT_OF_SPEC_CLOCKS defined. [542x only] */
  { 0x58, 0x1C },		/* 89.998 */
  { 0x49, 0x16 },		/* 95.019 */
  { 0x46, 0x14 },		/* 100.226 */
  { 0x53, 0x16 },		/* 108.035 */
  { 0x5C, 0x18 },		/* 110.248 */
  { 0x6D, 0x1A },		/* 120.050 */
  { 0x58, 0x14 },		/* 125.998 */
  { 0x6D, 0x18 },		/* 130.055 */
  { 0x42, 0x0E },		/* 134.998 */
 
  { 0x69, 0x14 },               /* 150.341 */
  { 0x5E, 0x10 },               /* 168.239 */
  { 0x5C, 0x0E },               /* 188.182 */
  { 0x67, 0x0E },               /* 210.682 */
  { 0x60, 0x0C },               /* 229.091 */
};

/* Doubled clocks for 16-bit clocking mode with pixel clock ~< 45 MHz. */

static cirrusClockRec cirrusDoubleClockTab[] = {
  { 0x51, 0x2E },		/* 50.424 =~ 2 * 25.227 */
  { 0x5B, 0x2E },		/* 55.649 =~ 2 * 28.325 */
  { 0x5C, 0x20 }, 		/* 82.328 =~ 2 * 41.164 */
  { 0x7E, 0x32 },		/* 72.163 =~ 2 * 36.082 */
  { 0x42, 0x1E },		/* 62.999 =~ 2 * 31.500 */
  { 0x5F, 0x22 }		/* 80.012 =~ 2 * 39.992 */
};

static cirrusClockRec cirrusTripleClockTab[] = {
  { 0x6E, 0x2A },		/* 75.000 =~ 3 * 25.227 */
  { 0x7D, 0x2A },		/* 85.226 =~ 3 * 28.325 */
};

static cirrusClockRec cirrusDivide2ClockTab[] = {
/* Values up to 85 MHz are irrelevant. */
  { 0x4A, 0x2B },		/* 25.227 */
  { 0x5B, 0x2F },		/* 28.325 */
  { 0x45, 0x30 }, 		/* 41.164 */
  { 0x7E, 0x33 },		/* 36.082 */
  { 0x42, 0x1F },		/* 31.500 */
  { 0x51, 0x3A },		/* 39.992 */
  { 0x55, 0x36 },		/* 45.076 */
  { 0x65, 0x3A },		/* 49.867 */
  { 0x76, 0x34 },		/* 64.983 */
  { 0x7E, 0x32 },		/* 72.163 */
  { 0x6E, 0x2A },		/* 75.000 */
  { 0x5F, 0x22 },		/* 80.013 */
  { 0x7D, 0x2A },		/* 85.226 */
/* Values that are actually used start here. */
  { 0x55, 0x36 },		/* 45.076 =~ 89.998 / 2 */
  { 0x49, 0x17 },		/* 47.509 =~ 95.019 / 2 */
  { 0x54, 0x30 },		/* 50.113 =~ 100.226 / 2 */
  { 0x53, 0x17 },		/* 54.017 =~ 108.035 / 2 */
  { 0x4D, 0x28 },		/* 55.124 =~ 110.248 / 2 */
  { 0x6D, 0x34 },		/* 60.025 =~ 120.050 / 2 */
  { 0x6E, 0x32 },		/* 62.999 =~ 125.998 / 2 */
  { 0x6D, 0x30 },		/* 65.027 =~ 130.055 / 2 */
  { 0x42, 0x1C },		/* 67.499 =~ 134.998 / 2 */
 
  { 0x69, 0x28 },               /* 75.170 =~ 150.341 / 2 */
  { 0x5E, 0x20 },               /* 84.119 =~ 168.239 / 2 */
  { 0x5C, 0x1C },               /* 94.091 =~ 188.182 / 2 */
  { 0x67, 0x1C },               /* 105.341 =~ 210.682 / 2 */
  { 0x60, 0x18 },               /* 114.545 =~ 229.091 / 2 */
};


/* Lowest clock for which palette clock doubling is required on the 5434. */
#define CLOCK_PALETTE_DOUBLING_5434 85500

#define NUM_CIRRUS_CLOCKS (sizeof(cirrusClockTab)/sizeof(cirrusClockRec))

/* CLOCK_FACTOR is double the osc freq in kHz (osc = 14.31818 MHz) */
#define CLOCK_FACTOR 28636

/* clock in kHz is (numer * CLOCK_FACTOR / (denom & 0x3E)) >> (denom & 1) */
#define CLOCKVAL(n, d) \
     ((((n) & 0x7F) * CLOCK_FACTOR / ((d) & 0x3E)) >> ((d) & 1))

static int cirrusClockLimit[LASTCLGD];

#ifdef MONOVGA
static int cirrusClockLimit4bpp[] = {
  /* Clock limits for 16-color (planar) mode. */
  80100,	/* 5420 */
  80100,	/* 5422 */
  80100,	/* 5424 */
  85500,	/* 5426 */
  85500,	/* 5428 */
  85500,	/* 5429 */
  65100,	/* 6205  The 62x5 are speced for 65 MHz at 5V, and */
  65100,	/* 6215  40 MHz at 3.3V. */
  65100,	/* 6225 */
  65100,	/* 6235 */
  85500,	/* 5434 */
  85500,	/* 5430 */
  85500,	/* 5434 */
  135100,	/* 5436 */
  135100,       /* 5446 */
  135100,       /* 5480 */
  170000,       /* 5462 */
  170000,       /* 5464 */
  170000,       /* 5465 */
  80100,	/* 7541 */
  80100,	/* 7542 */
  80100,	/* 7543 */
  80100,	/* 7548 */
  80100,	/* 7555 */
};

#else

static int cirrusClockLimit8bpp[] = {
  /* Clock limits for 256-color mode. */
  50200,	/* 5420 */
  80100,	/* 5422 */
  80100,	/* 5424 */
  85500,	/* 5426 */
  85500,	/* 5428 */
  85500,	/* 5429 */
  45100,	/* 6205 */
  45100,	/* 6215 */
  45100,	/* 6225 */
  45100,	/* 6235 */
#if defined(ALLOW_8BPP_MULTIPLEXING)
  110300,	/* 5434 */
#else
  85500,	/* 5434 */
#endif
  85500,	/* 5430 */
  135100,	/* 5436 */
  135100,       /* 5446 */
  200000,       /* 5480 */
  170000,       /* 5462 */
  250000,       /* 5464 */
  250000,       /* 5465 */
  80100,	/* 7541 */
  80100,	/* 7542 */
  80100,	/* 7543 */
  80100,	/* 7548 */
  80100,	/* 7555 */	/* 3.3v; Actually supports 135 MHz at 5.0v */
};

static int cirrusClockLimit16bpp[] = {
  /* Clock limits for 16bpp mode. */
  0,		/* 5420 */
  /* VCLK is doubled automatically for clocks < 42.300. */
  40100,	/* 5422 */
  40100,	/* 5424 */
  45100,	/* 5426 */
  45100,	/* 5428 */
  50000,	/* 5429 */
  0, 0, 0, 0,	/* 62x5 */
  85500,	/* 5434 (with >= 2MB DRAM) */
  50000,	/* 5430 */
  85500,	/* 5436 */
  85500,        /* 5446 */
  100000,       /* 5480 */
  135100,       /* 5462 */
  170000,       /* 5464 */     /* Increased from 135.1MHz. */
  170000,       /* 5464 */     /* --corey (3/24/97) */
  40100,	/* 7541 */
  40100,	/* 7542 */
  40100,	/* 7543 */
  40100,	/* 7548 (probably too low) */
  80100,	/* 7555 */
};

static int cirrusClockLimit24bpp[] = {
  /* Clock limits for 24bpp mode. */
  0,		/* 5420 */
  80100 / 3,	/* 5422 */
  80100 / 3,	/* 5424 */
  85500 / 3,	/* 5426 */
  85500 / 3,	/* 5428 */
  85500 / 3,	/* 5429 */
  0, 0, 0, 0,	/* 62x5 */
  85500 / 3,	/* 5434 */
  85500 / 3,	/* 5430 */
  85500,	/* 5436 */
  85500,        /* 5446 */
  100000,       /* 5480 */
  135100,      	/* 5462 */
  170000,       /* 5464 */  /* Used to be 135.1M --corey (3/24/97) */
  170000,       /* 5465 */
  80100 / 3,	/* 7541 XXXX Don't know for 754x. */ /* 80100 / 3 is a guess */
  0,		/* 7542 */
  80100 / 3,	/* 7543 */
  80100 / 3,    /* 7548 */
  80100		/* 7555 */
};

static int cirrusClockLimit32bpp[] = {
  /* Clock limits for 32bpp mode (5434-only). */
  0, 0, 0,	/* 5420/2/4 */
  0, 0, 0,	/* 5426/8/9 */
  0, 0, 0, 0,	/* 62x5 */
  45100,	/* 5434 */
  0,		/* 5430 */
  0,		/* 5436 */  /* 32bpp support for 5436/46 is broken */
  0,		/* 5446 */  /* (not sure if it's hardware or driver) */
  50000,	/* 5480 */
  85500,	/* 5462 */  /* Hmm... I wonder if this will break something */
  135100,       /* 5464 */
  135100,       /* 5465 */
  0,		/* 7541 */
  0,		/* 7542 */
  0,		/* 7543 */
  0,		/* 7548 */
  0,		/* 7555 */
};

#endif /* MONOVGA */

#define new ((vgacirrusPtr)vgaNewVideoState)

static SymTabRec chipsets[] = {
  { CLGD5420,	"clgd5420" },
  { CLGD5422,	"clgd5422" },
  { CLGD5424,	"clgd5424" },
  { CLGD5426,	"clgd5426" },
  { CLGD5428,	"clgd5428" },
  { CLGD5429,	"clgd5429" },
  { CLGD5430,	"clgd5430" },
  { CLGD5434,	"clgd5434" },
  { CLGD5436,	"clgd5436" },
  { CLGD5446,   "clgd5446" },
  { CLGD5480,   "clgd5480" },
  { CLGD5462,   "clgd5462" },
  { CLGD5464,   "clgd5464" },
  { CLGD5465,   "clgd5465" },
  { CLGD6205,	"clgd6205" },
  { CLGD6215,	"clgd6215" },
  { CLGD6225,	"clgd6225" },
  { CLGD6235,	"clgd6235" },
  { CLGD7541,	"clgd7541" },
  { CLGD7542,	"clgd7542" },
  { CLGD7543,	"clgd7543" },
  { CLGD7548,	"clgd7548" },
  { CLGD7555,	"clgd7555" },
  { -1,		"" },
};


/* The 546X uses RDRAM on a RAMBUS.  To be most effecient, the memory is
   organized in tiles.  See the 546X TRM for a discussion on issues regarding
   tiled memory.  One issue, though, is that only a discrete number of 
   tiles/line values are allowed.  These come from section 10.30 of the 
   546X TRM, dated September 1995. */
/* NOTE:  When adding/updating this table, all pitch entries _must_ be 
   placed in increasing order.  More exactly, the first time a new pitch
   occurs in the table must be _before_ all pitches which are greater than
   this pitch.  Duplicates are allowed, and may be placed anywhere in the
   table. */
cirrusTilesPerLine cirrusTilesPerLineTab[] = {
  {5, 640, 0},       /* We're rather use skinny tiles, so put all of */
  {8, 1024, 0},      /* them at the head of the table */
  {10, 1280, 0},
  {13, 1664, 0},
  {16, 2048, 0},
  {20, 2560, 0},
  {10, 2560, 1},
  {26, 3328, 0},
  {5, 1280, 1},
  {8, 2048, 1},
  {13, 3328, 1},
  {16, 4096, 1},
  {20, 5120, 1},
  {26, 6656, 1},
  {-1, -1, -1}     /* Sentinal to indicate end of table */
};
int defaultTilesPerLineIndex = 5; /* 2048 byte, skinny tiles */




/*
 * cirrusIdent -- 
 */
static char *
cirrusIdent(n)
     int n;
{
  if (chipsets[n].token < 0)
    return(NULL);
  else 
    return(chipsets[n].name);
}

/*
 * cirrusCheckClock --
 *	check if the clock is supported by the chipset
 */
static Bool
cirrusCheckClock(chip, clockno)
  int chip;
  int clockno;
{
  unsigned clockval;

  if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
      clockval = vga256InfoRec.clock[clockno];
  else
      clockval = CLOCKVAL(cirrusClockTab[clockno].numer,
		          cirrusClockTab[clockno].denom);

  if (clockval > cirrusClockLimit[chip])
  {
    ErrorF("CIRRUS: clock %7.3f is too high for %s (max is %7.3f)\n",
	   clockval / 1000.0, xf86TokenToString(chipsets, chip),
	   cirrusClockLimit[chip] / 1000.0);

#ifdef ALLOW_OUT_OF_SPEC_CLOCKS
    if (OFLG_ISSET(OPTION_16CLKS, &vga256InfoRec.options))
      {
	ErrorF ("CIRRUS: Out of spec. clocks option is enabled\n");
	return (TRUE);
      }
#endif

    return(FALSE);
  }
  return(TRUE);
}

/*
 * cirrusClockSelect --
 *      select one of the possible clocks ...
 */
static Bool
cirrusClockSelect(no)
     int no;
{
  static unsigned char save1, save2, save3;
  unsigned char temp;
  int SR,SR1;


#ifdef DEBUG_CIRRUS
  fprintf(stderr,"Clock NO = %d\n",no);
#endif

#if 0
  SR = 0x7E; SR1 = 0x33;	/* Just in case.... */
#endif

  switch(no)
       {
     case CLK_REG_SAVE:
       save1 = inb(0x3CC);
       outb(0x3C4, 0x0E);
       save2 = inb(0x3C5);
       outb(0x3C4, 0x1E);
       save3 = inb(0x3C5);
       break;
     case CLK_REG_RESTORE:
       outb(0x3C2, save1);
       outw(0x3C4, (save2 << 8) | 0x0E);
       outw(0x3C4, (save3 << 8) | 0x1E);
       break;
     default:
       if (!cirrusCheckClock(cirrusChip, no))
	    return(FALSE);

       if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) {
           if (vgaBitsPerPixel == 16 && MUST_DOUBLE_VCLK_FOR_16BPP)
           	/* Use the clocking mode whereby the programmed VCLK */
           	/* is double the pixel rate. */
               CirrusSetClock(vga256InfoRec.clock[no] * 2);
           else
           if (vgaBitsPerPixel == 24 && cirrusChip <= CLGD5434)
           	/* Use the clocking mode whereby the programmed VCLK */
           	/* is triple the pixel rate. */
               CirrusSetClock(vga256InfoRec.clock[no] * 2);
           else
               CirrusSetClock(vga256InfoRec.clock[no]);
           return TRUE;
       }

       SR = cirrusClockTab[no].numer;
       SR1 = cirrusClockTab[no].denom;

#ifndef MONOVGA
       if (vgaBitsPerPixel == 16 && MUST_DOUBLE_VCLK_FOR_16BPP) {
	   /* Use the clocking mode whereby the programmed VCLK */
	   /* is double the pixel rate. */
	   SR = cirrusDoubleClockTab[no].numer;
	   SR1 = cirrusDoubleClockTab[no].denom;
       }
       if (vgaBitsPerPixel == 24 && cirrusChip <= CLGD5434) {
	   /* Use the clocking mode whereby the programmed VCLK */
	   /* is triple the pixel rate. */
	   SR = cirrusTripleClockTab[no].numer;
	   SR1 = cirrusTripleClockTab[no].denom;
       }
#endif
				/*  Use VCLK3 for these extended clocks */
       temp = inb(0x3CC);
       outb(0x3C2, temp | 0x0C );
  
#ifdef DEBUG_CIRRUS
       fprintf(stderr,"Misc = %x\n",temp);
       fprintf(stderr,"Miscactual = %x\n",(temp & 0xF3) | 0x0C);
#endif
  
				/* Set SRE and SR1E */
       outb(0x3C4,0x0E);
       temp = inb(0x3C5);
       outb(0x3C5,(temp & 0x80) | (SR & 0x7F));
  
#ifdef DEBUG_CIRRUS
       fprintf(stderr,"SR = %x\n",temp);
       fprintf(stderr,"SRactual = %x\n",(temp & 0x80) | (SR & 0x7F));
#endif

       outb(0x3C4,0x1E);
       temp = inb(0x3C5);
       outb(0x3C5,(temp & 0xC0) | (SR1 & 0x3F));
  
#ifdef DEBUG_CIRRUS
       fprintf(stderr,"SR1 = %x\n",temp);
       fprintf(stderr,"SR1actual = %x\n",(temp & 0xC0) | (SR1 & 0x3F));
#endif
       break;
       }
       return(TRUE);
}

/*
 * cirrusNumClocks --
 *	returns the number of clocks available for the chip
 */
static int
cirrusNumClocks(chip)
     int chip;
{
     cirrusClockRec *rec, *end = cirrusClockTab + NUM_CIRRUS_CLOCKS;

     /* 
      * The 62x5 chips can do marvelous things, but the
      * LCD panels connected to them don't leave much
      * option.  The following forces the cirrus chip to
      * use the slowest clock -- which appears to be what
      * my LCD panel likes best.  Faster clocks seem to
      * cause the LCD display to show noise when things are
      * moved around on the screen.
      */
     /* XXXX might be better/safer to reduce the value in clock limit tab */
     if (lcd_is_on) 
       {
	 return(1);
       }

#ifdef ALLOW_OUT_OF_SPEC_CLOCKS
     if (OFLG_ISSET(OPTION_16CLKS, &vga256InfoRec.options))
       {
	 return (NUM_CIRRUS_CLOCKS);
       }
#endif
     
     for (rec = cirrusClockTab; rec < end; rec++)
          if (CLOCKVAL(rec->numer, rec->denom) > cirrusClockLimit[chip])
               return(rec - cirrusClockTab);
     return(NUM_CIRRUS_CLOCKS);
}

/*
 * cirrusProbe -- 
 *      check up whether a cirrus based board is installed
 */
static Bool
cirrusProbe()
{  
     int cirrusClockNo, i;
     unsigned char lockreg,IdentVal;
     unsigned char id, rev, partstatus;
     unsigned char temp;

     /*
      * Set up I/O ports to be used by this card
      */
     xf86ClearIOPortList(vga256InfoRec.scrnIndex);
     xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
     xf86AddIOPorts(vga256InfoRec.scrnIndex, sizeof(Cirrus_IOPorts) /
         sizeof(Cirrus_IOPorts[0]), Cirrus_IOPorts);

     if (vga256InfoRec.chipset)
	  {
	  if (!StrCaseCmp(vga256InfoRec.chipset, "cirrus"))
	       {
               ErrorF("\ncirrus is no longer valid.  Use one of\n");
	       ErrorF("the names listed by the -showconfig option\n");
	       return(FALSE);
               }
          if (!StrCaseCmp(vga256InfoRec.chipset, "clgd543x"))
	       {
               ErrorF("\nclgd543x is no longer valid.  Use one of\n");
	       ErrorF("the names listed by the -showconfig option\n");
	       return(FALSE);
               }
	  cirrusChip = xf86StringToToken(chipsets, vga256InfoRec.chipset);
	  if (cirrusChip >= 0)
	       {
	       cirrusEnterLeave(ENTER); /* Make the timing regs writable */
	       }
	  else
	       {
	       return(FALSE);
	       }
	  }
     else
	  {
	  unsigned char old;
	  xf86EnableIOPorts(vga256InfoRec.scrnIndex);
	  old = rdinx(0x3c4, 0x06);
	  cirrusEnterLeave(ENTER); /* Make the timing regs writable */
	  
	  /* Kited the following from the Cirrus */
	  /* Databook */
	  
	  /* If it's a Cirrus at all, we should be */
	  /* able to read back the lock register */
	  /* we wrote in cirrusEnterLeave() */
	  
	  outb(0x3C4,0x06);
	  lockreg = inb(0x3C5);
	  
	  /* Ok, if it's not 0x12, we're not a Cirrus542X or 62x5. */
	  if (lockreg != 0x12)
	       {
	       wrinx(0x3c4, 0x06, old);
	       cirrusEnterLeave(LEAVE);
	       return(FALSE);
	       }
	  
	  /* OK, it's a Cirrus. Now, what kind of */
	  /* Cirrus? We read in the ident reg, */
	  /* CRTC index 27 */
	  
	  
	  outb(vgaIOBase+0x04, 0x27); IdentVal = inb(vgaIOBase+0x05);
	  
	  cirrusChip = -1;
	  id  = (IdentVal & 0xFc) >> 2;
	  rev = (IdentVal & 0x03);

	  outb(vgaIOBase + 0x04, 0x25); partstatus = inb(vgaIOBase + 0x05);
          cirrusChipRevision = 0x00;

	  switch( id )
	       {
	     case CLGD7541_ID:
	       cirrusChip = CLGD7541;
	       break;
	     case CLGD7542_ID:
	       cirrusChip = CLGD7542;
	       break;
	     case CLGD7543_ID:
	       cirrusChip = CLGD7543;
	       break;
	     case CLGD7548_ID:
	       cirrusChip = CLGD7548;
	       break;
	     case CLGD5420_ID:
#if 0	/* Conflicts with CL-GD6235. */
	     case CLAVGA2_ID:		/* AVGA2 uses 5402 */
#endif
	       cirrusChip = CLGD5420;	/* 5420 or 5402 */
	       /* Check for CL-GD5420-75QC-B */
	       /* It has a Hidden-DAC register. */
	       outb(0x3C6, 0x00);
	       outb(0x3C6, 0xFF);
	       inb(0x3C6); inb(0x3c6); inb(0x3C6); inb(0x3C6);
	       if (inb(0x3C6) != 0xFF)
	           cirrusChipRevision = 0x01;	/* 5420-75QC-B */
	       break;
	     case CLGD5422_ID:
	       cirrusChip = CLGD5422;
	       break;
	     case CLGD5424_ID:
	       cirrusChip = CLGD5424;
	       break;
	     case CLGD5426_ID:
	       cirrusChip = CLGD5426;
	       break;
	     case CLGD5428_ID:
	       cirrusChip = CLGD5428;
	       break;
	     case CLGD5429_ID:
	       if (partstatus >= 0x67)
	           cirrusChipRevision = 0x01;	/* >= Rev. B, fixes BLT */
	       cirrusChip = CLGD5429;
	       break;

	     /* 
	      * LCD driver chips...  the +1 options are because
	      * these chips have one more bit of chip rev level
	      */
	     case CLGD6205_ID:
	     case CLGD6205_ID + 1:
	       cirrusChip = CLGD6205;
	       break;
#if 0
	     /* looks like a 5420...  oh well...  close enough for now */
	     case CLGD6215_ID:
	     /* looks like a 5422...  oh well...  close enough for now */
	     case CLGD6215_ID + 1:
	       cirrusChip = CLGD6215;
	       break;
#endif
	     case CLGD6225_ID:
	     case CLGD6225_ID + 1:
	       cirrusChip = CLGD6225;
	       break;
	     case CLGD6235_ID:
	     case CLGD6235_ID + 1:
	       cirrusChip = CLGD6235;
	       break;

	     /* 'Alpine' family. */
	     case CLGD5434_ID:
	       if ((partstatus & 0xC0) == 0xC0) {
	          /*
	           * Better than Rev. ~D/E/F.
	           * Handles 60 MHz MCLK and 135 MHz VCLK.
	           */
	          cirrusChipRevision = 0x01;
		  cirrusClockLimit[CLGD5434] = 135100;
	       }
	       else
	       if (partstatus == 0x8E)
	       	  /* Intermediate revision, supports 135 MHz VCLK. */
		  cirrusClockLimit[CLGD5434] = 135100;
	       cirrusChip = CLGD5434;
	       break;

	     case CLGD5430_ID:
	       cirrusChip = CLGD5430;
	       break;

	     case CLGD5436_ID:
	       cirrusChip = CLGD5436;
	       break;

	     case CLGD5446_ID:
	       cirrusChip = CLGD5446;
	       break;

 	     case CLGD5480_ID:
 	       cirrusChip = CLGD5480;
 	       break;
	       
	     default:
	       /* The Laguna family (546x) doesn't respond to CR27 writes
		  like all of its cousins.  So instead, query the PCI bus to 
		  see if there's a Laguna chip there. */
	       if (vgaPCIInfo && vgaPCIInfo->Vendor == PCI_VENDOR_CIRRUS) {
		 switch (vgaPCIInfo->ChipType) {
		 case PCI_CHIP_GD5462:
		   cirrusChip = CLGD5462;
		   break;

		 case PCI_CHIP_GD5464:
		   cirrusChip = CLGD5464;
		   break;

		 case PCI_CHIP_GD5465:
		   cirrusChip = CLGD5465;
		   break;

		 case PCI_CHIP_GD7548:
		   cirrusChip = CLGD7548;
		   break;
		 }
		 
		 if (HAVE546X() && vgaBitsPerPixel < 8) {
		   /* The mono and 16 color support isn't working
		      yet for the Laguna driver.  As a work around,
		      punt these modes off to the generic driver.
		      We wouldn't do anything fancier, anyway.  And
		      besides, if you have a Laguna card, what are 
		      you doing in mono or 16 color mode?!? 
		      --corey 5/20/97 */

		   return FALSE;
		 }

		 /* If we've found the chip on the PCI bus, jump out of the
		    chip-switch statement */
		 if (cirrusChip > 0)
		   break;
	       }

	     case CLGD5434_OLD_ID:
	       ErrorF("Unknown Cirrus chipset: type 0x%02x, rev %d\n", id, rev);
	       if (id == CLGD5434_OLD_ID)
	          ErrorF("Old pre-production ID for clgd5434 -- please report\n");
	       cirrusEnterLeave(LEAVE);
	       return(FALSE);
	       break;
	       }
	  
	  if (cirrusChip == CLGD5430 || cirrusChip == CLGD5434 ||
	      cirrusChip == CLGD5436 || cirrusChip == CLGD5446 ||
	      cirrusChip == CLGD5480) {
	      /* Write sane value to Display Compression Control */
	      /* Register, which may be corrupted by pvga1 driver */
	      /* probe. */
	      outb(0x3ce, 0x0f);
	      temp = inb(0x3cf) & 0xc0;
	      outb(0x3cf, temp);
	  }

	  }
     
     /* OK, we are a Cirrus */

     vga256InfoRec.chipset = xf86TokenToString(chipsets, cirrusChip);

#ifndef MONOVGA
#ifdef XFreeXDGA
     /* we support direct Video mode */

     vga256InfoRec.directMode = XF86DGADirectPresent;
#endif

     if ((vgaBitsPerPixel == 16 || vgaBitsPerPixel == 24) &&
     (Is_62x5(cirrusChip) || cirrusChip == CLGD5420)) {
         ErrorF("%s %s: %s: Cirrus 62x5 and 5420 chipsets not supported "
             "in %dbpp mode\n",
             XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset,
	     vgaBitsPerPixel);
         CIRRUS.ChipHas16bpp = FALSE;
         CIRRUS.ChipHas24bpp = FALSE;
     }
#endif

    cirrusBusType = CIRRUS_BUS_FAST;
    if (HAVE546X() || cirrusChip == CLGD5446 || cirrusChip == CLGD5480)
        /* The Laguna has support for the VL bus, but I don't know of any
           VL boards out there with a 546X.  */
        cirrusBusType = CIRRUS_BUS_PCI;
    else if (cirrusChip >= CLGD5422) {
  	/*
  	 * The bus type configuration can be read back from a register.
  	 * This is only implemented on later 542x cards and on the 543x.
  	 * The only function that uses this is solid filling on the 5426,
  	 * for which framebuffer color expansion is much faster than the
  	 * BitBLT engine on a local bus. The 754x use a different register.
  	 */
        if (HAVE75XX()) {
  	    outb(0x3c4, 0x22);
  	    if (inb(0x3c5) & 0x1)
  	        cirrusBusType = CIRRUS_BUS_PCI;
  	    else
  	        cirrusBusType = CIRRUS_BUS_VLB;
  	}
  	else {
  	    outb(0x3c4, 0x17);
            switch ((inb(0x3c5) >> 3) & 7) {
  	    case 2 :	/* VLB > 33 MHz */
  	    case 6 :	/* VLB at 33 MHz or less */
  		cirrusBusType = CIRRUS_BUS_VLB;
  		break;
  	    case 4 :	/* PCI */
  		cirrusBusType = CIRRUS_BUS_PCI;
  		break;
  	    case 7 :	/* ISA */
  		cirrusBusType = CIRRUS_BUS_ISA;
  		break;
            /* In other cases (e.g. undefined), assume 'fast' bus. */
            }
  	}
    }

     /* 
      * Try to determine special LCD-oriented stuff...
      *
      * [1] LCD only, CRT only, or simultaneous
      * [2] Panel type (if LCD is enabled)
      *
      * Currently, this isn't used for much, but I've put it
      * into this driver so that you'll at least have a clue
      * that the driver isn't the right one for your chipset
      * if it incorrectly identifies the panel configuration.
      * Later versions of this driver will probably do more
      * with this info than just print it out....
      */
     if (Is_62x5(cirrusChip) || Is_75xx(cirrusChip)) 
	  {
	  /*
	   * Unlock the LCD registers...
           * For the 754x, this enables access to extension registers
           * R2X to REX, which are mapped from CRTC index 0x2 to 0xE.
           * I have my doubts about accessing regular CRTC registers
           * beyond the RXX mapping range while RXX mapping is enabled,
           * as the current code does, but I'm afraid to break the 62x5
           * if I change this.
	   */
	  if( Is_75xx(cirrusChip) )
	      outb(vgaIOBase + 4, 0x2d);
	  else
	      outb(vgaIOBase + 4, 0x1D);
	  temp = inb(vgaIOBase + 5);
	  outb(vgaIOBase + 5, (temp | 0x80));

	  /* LCD only, CRT only, or simultaneous? */
	  outb(vgaIOBase + 4, 0x20);
	  switch (inb(vgaIOBase + 5) & 0x60) {
	     case 0x60:
	        lcd_is_on = TRUE;
                ErrorF("%s %s: %s: Simultaneous LCD and CRT display\n",
                    XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
	        break;
	     case 0x40:
                ErrorF("%s %s: %s: CRT display only\n",
                    XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
	        break;
	     case 0x20:
	        lcd_is_on = TRUE;
                ErrorF("%s %s: %s: LCD display only\n",
                    XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
	        break;
	     default:
                ErrorF("%s %s: %s: Neither LCD nor CRT display\n",
                    XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
                ErrorF("%s %s: %s: Probably not a supported CL-GD62x5/754x!\n",
                    XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
                ErrorF("%s %s: %s: Use this driver at your own risk!\n",
                    XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
          }

          /* What type of LCD panel do we have? */
	  if (lcd_is_on) {
	       if (Is_75xx(cirrusChip)) {
	           char *type, *size;
	           int h;
		   outb(vgaIOBase + 4, 0x43); /* access fine dotclk delay */
		   outb(vgaIOBase + 5, 0x2a); /* set to 2 pixels */
		   outb(vgaIOBase + 4, 0x2c); /* this is the 754x register */
		   temp = (inb(vgaIOBase + 5) | 0x20) & 0xF7;
		   outb(vgaIOBase + 5, temp); /* enable shadow write */
	           /*
	            * On the 754x, the LCD type register resides at
	            * CRTC controller index 0x2C.
	            */
	           outb(vgaIOBase + 4, 0x2c);
	           switch (inb(vgaIOBase + 5) & 0xc0) {
		   case 0xc0:
		       type = "TFT-color";
		       break;
		   case 0x80:
		       type = "Color STN";
		       break;
		   case 0x00:
		       type = "Monochrome";
		       break;
		   default:
		       type = "Unknown-type";
                       break;
		   }

		   /* Read LCD size at R9X [3:2]. */
	           outb(vgaIOBase + 4, 0x09);
	           switch (((temp = inb(vgaIOBase + 5)) & 0x0C) >> 2) {
	           case 0x0 :
	           	size = "640x480";
	           	cirrusLCDVerticalSize = 480;
	           	break;
	           case 0x1 :
	           	size = "800x600";
	           	cirrusLCDVerticalSize = 600;
	           	break;
	           case 0x2 :
	           	size = "1024x768";
	           	cirrusLCDVerticalSize = 768;
	           	break;
	           case 0x3 :
	                size = "Unknown-size (default to 640x480)";
	                cirrusLCDVerticalSize = 480;
	                break;
	           }
                   ErrorF("%s %s: %s: %s %s ",
                       XCONFIG_PROBED, vga256InfoRec.name,
                       vga256InfoRec.chipset, size, type);
                   if (type[0] == 'T') {
                       /*
                        * TFT-color. LCD color resolution is defined by
                        * R9X [1:0].
                        */
                       int colorbits;
                       switch (temp & 0x3) {
                       case 0 : colorbits = 9; break;
                       case 1 : colorbits = 12; break;
                       case 2 : colorbits = 18; break;
                       case 3 : colorbits = 24; break;
                       }
                       ErrorF("(%d-bit color) ", colorbits);
                   }
                   ErrorF("LCD detected\n");
	       }
	       else { 
	           /* 62x5 only. */
	           outb(vgaIOBase + 4, 0x1c);
	           switch (inb(vgaIOBase + 5) & 0xc0) {
		   case 0xc0:
                       ErrorF("%s %s: %s: TFT active color LCD detected\n",
                           XCONFIG_PROBED, vga256InfoRec.name,
                           vga256InfoRec.chipset);
		       break;
		   case 0x80:
                       ErrorF("%s %s: %s: STF passive color LCD detected\n",
                           XCONFIG_PROBED, vga256InfoRec.name,
                           vga256InfoRec.chipset);
		       break;
		   case 0x40:
                       ErrorF("%s %s: %s: Grayscale plasma LCD detected\n",
                           XCONFIG_PROBED, vga256InfoRec.name,
                           vga256InfoRec.chipset);
		       break;
		   default:
                       ErrorF("%s %s: %s: Monochrome LCD detected, "
                           "enabling option clgd6225_lcd\n",
                           XCONFIG_PROBED, vga256InfoRec.name,
                           vga256InfoRec.chipset);
		       OFLG_SET(OPTION_CLGD6225_LCD, &vga256InfoRec.options);
		   }
	       }
	  } /* lcd_is_on */

	  /* Lock the LCD registers... */
	  if(Is_75xx(cirrusChip) )
	      outb(vgaIOBase + 4, 0x2d);
	  else
	      outb(vgaIOBase + 4, 0x1D);
	  temp = inb(vgaIOBase + 5);
	  outb(vgaIOBase + 5, (temp & 0x7f));
	} /* End LCD stuff */


     if (!vga256InfoRec.videoRam) 
	  {
	  if (Is_62x5(cirrusChip)) 
	       {
	       /* 
		* According to Ed Strauss at Cirrus, the 62x5 has 512k.
		* That's it.  Period.
		*/
	       vga256InfoRec.videoRam = 512;
	       }
	  else 
	  if (HAVEALPINE() || cirrusChip == CLGD7543) {
	  	/* The scratch register method does not work on the 543x. */
	  	/* Use the DRAM bandwidth bit and the DRAM bank switching */
	  	/* bit to figure out the amount of memory. */
	  	unsigned char SRF;
	  	vga256InfoRec.videoRam = 512;
	  	outb(0x3c4, 0x0f);
	  	SRF = inb(0x3c5);
	        if (cirrusChip == CLGD5446) {
	            /*
	             * The CL-GD5446 has many different DRAM
	             * configurations.
	             */
	            unsigned char SR17;
	            outb(0x3c4, 0x17);
	            SR17 = inb(0x3c5);
  	            vga256InfoRec.videoRam = 1024;
	  	    if ((SRF & 0x18) == 0x18) {
	  	        /* 64-bit DRAM bus. */
	  	        if (SRF & 0x02) {
	  	            /* Second bank is present. */
	  	            if (SR17 & 0x80) {
	  	                /* Two 1Mbyte banks; 128Kx16 devices. */
	  	                cirrus128KDevices = TRUE;
	  	                vga256InfoRec.videoRam = 2048;
	  	            }
	  	            else
	  	                if (SR17 & 0x02)
	  	                    /* Bank Swap, mixed devices (128K/256K). */
	  	                    vga256InfoRec.videoRam = 3072;
	  	                else
	  	                    /*
	  	                     * No bank swap, two banks of
	  	                     * 256Kx16 devices.
	  	                     */
	  	                    vga256InfoRec.videoRam = 4096;
	  	        }
	  	        else {
	  	            /* Only one bank. */
	  	            if ((SR17 & 0x80) == 0)
	  	                /* Not a 1MByte bank; 256Kx16 devices. */
	  	                vga256InfoRec.videoRam = 2048;
	  	            else
	  	                /* Otherwise, 128Kx16, 1MB. */
	  	                cirrus128KDevices = TRUE;
	  	        }
	  	    }
	  	    /* Otherwise, 32-bit DRAM bus, 1Mbyte, 256Kx16 devices. */
	        }
	        else {
	            /*
	             * For the CL-GD7555, this will come up with 2Mbytes
	             * most of the time (which is expected to be the
	             * predominant configuration). However, there does
	             * not seem to be an easy way to detect 1Mbyte
	             * (128K devices) configurations for this chip. 
	             */
	  	    if (SRF & 0x10)
	  		/* 32-bit DRAM bus. */
	  		vga256InfoRec.videoRam *= 2;
	  	    if ((SRF & 0x18) == 0x18)
	  		/* 64-bit DRAM data bus width; assume 2MB. */
	  		/* Also indicates 2MB memory on the 5430. */
	  		vga256InfoRec.videoRam *= 2;
	  	    if (cirrusChip != CLGD5430 && (SRF & 0x80))
	  		/* If DRAM bank switching is enabled, there */
	  		/* must be twice as much memory installed. */
	  		/* (4MB on the 5434) */
	  		vga256InfoRec.videoRam *= 2;
	        }
	  }
	  else 
	    if (HAVE546X()) {
	      int memreg;

	      /* The ROM BIOS scratchpad registers contain, 
		 among other things, the amount of installed
		 RDRAM on the laguna chip. */
#if defined(DEBUG_CIRRUS) && defined(DEBUG_LG)
	      outb(0x3C4, 0x09);
	      memreg = inb(0x3C5);
	      ErrorF("SR09: 0x%02X  ", memreg);
	      outb(0x3C4, 0x0A);
	      memreg = inb(0x3C5);
	      ErrorF("SR0A: 0x%02X  ", memreg);
	      outb(0x3C4, 0x14);
	      memreg = inb(0x3C5);
	      ErrorF("SR14: 0x%02X  ", memreg);
	      outb(0x3C4, 0x15);
	      memreg = inb(0x3C5);
	      ErrorF("SR15: 0x%02X\n", memreg);
#endif

	      outb(0x3C4, 0x14);
	      memreg = inb(0x3C5);

	      vga256InfoRec.videoRam = 1024 * ((memreg&0x7) + 1);
	    }
	  else
	       {
	       unsigned char memreg;

				/* Thanks to Brad Hackenson at Cirrus for */
				/* this bit of undocumented black art....*/
	       outb(0x3C4,0x0A);
	       memreg = inb(0x3C5);
	  
	       switch( (memreg & 0x18) >> 3 )
		    {
		  case 0:
		    vga256InfoRec.videoRam = 256;
		    break;
		  case 1:
		    vga256InfoRec.videoRam = 512;
		    break;
		  case 2:
		    vga256InfoRec.videoRam = 1024;
		    break;
		  case 3:
		    vga256InfoRec.videoRam = 2048;
		    break;
		    }

	       if (cirrusChip >= CLGD5422 && cirrusChip <= CLGD5429 &&
	       vga256InfoRec.videoRam < 512) {
	       		/* Invalid amount for 542x -- scratch register may */
	       		/* not be set by some BIOSes. */
		  	unsigned char SRF;
		  	vga256InfoRec.videoRam = 512;
	  		outb(0x3c4, 0x0f);
		  	SRF = inb(0x3c5);
		  	if (SRF & 0x10)
		  		/* 32-bit DRAM bus. */
		  		vga256InfoRec.videoRam *= 2;
		  	if ((SRF & 0x18) == 0x18)
		  		/* 2MB memory on the 5426/8/9 (not sure). */
		  		vga256InfoRec.videoRam *= 2;
		  	}
	       }
	  }
     else
	  {
	  /*
	   * Some cards don't initialise SRF correctly, so do it here if the
	   * user has specified the videoram amount.
	   */
	  if (!(HAVE543X() || HAVE75XX()))
	       {
	       unsigned char SRF = 0;
	       if (vga256InfoRec.videoRam > 1024)
		    SRF = 0x18;
	       else if (vga256InfoRec.videoRam > 512)
		    SRF = 0x10;
	       outb(0x3c4, 0x0f);
	       SRF |= (inb(0x3c5) & ~0x18);
	       outb(0x3c5, SRF);
	       }
	  }

#ifndef MONOVGA
     if (vgaBitsPerPixel == 32 &&
     ((cirrusChip != CLGD5434 && cirrusChip != CLGD5436 && 
       cirrusChip != CLGD5446 && cirrusChip != CLGD5480 && !HAVE546X()) ||
     vga256InfoRec.videoRam < 2048)) {
         ErrorF("%s %s: %s: Only clgd5434, clgd5436, clgd5446, clgd5480, \n",
             XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
	 ErrorF("%s %s: %s: and clgd546X with 2048K or more support 32bpp\n",
             XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
         CIRRUS.ChipHas32bpp = FALSE;
     }
#endif

     /* 
      * Banking granularity is 16k for the 5426, 5428 or 5429
      * when allowing access to 2MB, and 4k otherwise 
      */
     if (vga256InfoRec.videoRam > 1024)
          {
          CIRRUS.ChipSetRead = cirrusSetRead2MB;
          CIRRUS.ChipSetWrite = cirrusSetWrite2MB;
          CIRRUS.ChipSetReadWrite = cirrusSetReadWrite2MB;
	  cirrusBankShift = 8;
          }


#ifdef MONOVGA

     memcpy(cirrusClockLimit, cirrusClockLimit4bpp, LASTCLGD * sizeof(int));

#else

     if (!HAVE546X()) {
     /*
      * Determine the MCLK that will be used (possibly reprogrammed).
      * Calculate the available DRAM bandwidth from the MCLK setting.
      */
         unsigned char MCLK, SRF;
         outb(0x3c4, 0x0f);
         SRF = inb(0x3c5);
         if (cirrusChip == CLGD5434 && cirrusChipRevision >= 0x01)
             /* 5434 rev. E+ supports 60 MHz MCLK in packed-pixel mode. */
             cirrusReprogrammedMCLK = 0x22;
         if ((cirrusChip >= CLGD5424 && cirrusChip <= CLGD5429) ||
         HAVE543X() || HAVE75XX() || cirrusChip == CLGD5446 ||
	     cirrusChip == CLGD5480) {
             outb(0x3c4, 0x1f);
             MCLK = inb(0x3c5) & 0x3f;
             if (OFLG_ISSET(OPTION_SLOW_DRAM, &vga256InfoRec.options))
                 cirrusReprogrammedMCLK = 0x1c;
             if (OFLG_ISSET(OPTION_MED_DRAM, &vga256InfoRec.options))
                 cirrusReprogrammedMCLK = 0x1f;
             if (OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options))
                 cirrusReprogrammedMCLK = 0x22;
         }
         else
             /* 5420/22/62x5 have fixed MCLK settings. */
             switch (SRF & 0x03) {
             case 0 : MCLK = 0x1c; break;
             case 1 : MCLK = 0x19; break;
             case 2 : MCLK = 0x17; break;
             case 3 : MCLK = 0x15; break;
             }
         if (cirrusReprogrammedMCLK > 0)
             MCLK = cirrusReprogrammedMCLK;

         /* Approximate DRAM bandwidth in K/s (8-bit page mode accesses),
          * corresponds with MCLK frequency / 2 (2 cycles per access). */
         cirrusDRAMBandwidth = 14318 * MCLK / 16;
         if (vga256InfoRec.videoRam >= 512)
             /* At least 16-bit access. */
             cirrusDRAMBandwidth *= 2;
         if (cirrusChip != CLGD5420 &&
         (cirrusChip < CLGD6205 || cirrusChip > CLGD6235) &&
         vga256InfoRec.videoRam >= 1024)
             /* At least 32-bit access. */
             cirrusDRAMBandwidth *= 2;
         if (((cirrusChip == CLGD5434 || cirrusChip == CLGD5436 ||
	      cirrusChip == CLGD5446 || cirrusChip == CLGD5480) && 
	      vga256InfoRec.videoRam >= 2048)
	 || (cirrusChip == CLGD5446 && cirrus128KDevices)
	 || cirrusChip == CLGD7555)
             /* 64-bit access. */
             cirrusDRAMBandwidth *= 2;
         /*
          * Calculate highest acceptable DRAM bandwidth to be taken up
          * by screen refresh. Satisfies
          *	total bandwidth >= refresh bandwidth * 1.1
          */
         cirrusDRAMBandwidthLimit = (cirrusDRAMBandwidth * 10) / 11;
     }


     /*
      * Adjust the clock limits for inadequate amounts of memory
      * and for 16bpp/32bpp modes.
      * In cases where DRAM bandwidth is the limiting factor, we
      * require the total bandwidth to be at least 10% higher than the
      * bandwidth required for screen refresh (which is what the Cirrus
      * databook recommends for 'acceptable' performance).
      */

     if (vgaBitsPerPixel == 8) {
         memcpy(cirrusClockLimit, cirrusClockLimit8bpp,
             LASTCLGD * sizeof(int));
         if (cirrusChip >= CLGD5420 && cirrusChip <= CLGD5429 &&
         vga256InfoRec.videoRam <= 512)
             cirrusClockLimit[cirrusChip] = cirrusDRAMBandwidthLimit;
#ifdef ALLOW_8BPP_MULTIPLEXING
         if ((cirrusChip == CLGD5434 || cirrusChip == CLGD5436 ||
	      cirrusChip == CLGD5446 || cirrusChip == CLGD5480)
         && vga256InfoRec.videoRam <= 1024)
             /* DRAM bandwidth limited. This translates to allowing
              * 90 MHz with MCLK = 0x1c, 95 MHz with MCLK = 0x1f,
              * 100 MHz with 0x22. */
             cirrusClockLimit[cirrusChip] = min(cirrusDRAMBandwidthLimit,
                 cirrusClockLimit[cirrusChip]);
#endif
     }

     if (vgaBitsPerPixel == 16) {
         memcpy(cirrusClockLimit, cirrusClockLimit16bpp,
             LASTCLGD * sizeof(int));
         if (cirrusChip >= CLGD5422 && cirrusChip <= CLGD5429
         && vga256InfoRec.videoRam <= 512)
                 cirrusClockLimit[cirrusChip] = 0;
#ifdef DOUBLE_16BPP_VCLK_FOR_1MB_5434
         if (cirrusChip == CLGD5434 && vga256InfoRec.videoRam == 1024)
             cirrusClockLimit[cirrusChip] = 42600;
         else
#endif
         if ((cirrusChip >= CLGD5426 && cirrusChip <= CLGD5429) ||
	      cirrusChip == CLGD5430 || HAVE75XX() ||
	     ((cirrusChip == CLGD5434 || cirrusChip == CLGD5436 || 
	       cirrusChip == CLGD5446 || cirrusChip == CLGD5480) && 
	      vga256InfoRec.videoRam <= 1024))
                 /* Allow 45 MHz with MCLK = 0x1c, 50 MHz with 0x1f+. */
                 cirrusClockLimit[cirrusChip] = cirrusDRAMBandwidthLimit / 2;
     }

     if (vgaBitsPerPixel == 24) {
         memcpy(cirrusClockLimit, cirrusClockLimit24bpp,
             LASTCLGD * sizeof(int));
         if (vga256InfoRec.videoRam <= 512)
                 cirrusClockLimit[cirrusChip] = 0;
         if (cirrusChip == CLGD5436 || cirrusChip == CLGD5446 ||
	     cirrusChip == CLGD5480)
		/* Only on the 5436/46/80, with packed-24 at pixel rate 
		   support, is DRAM bandwidth the limiting factor. */
                cirrusClockLimit[cirrusChip] = cirrusDRAMBandwidthLimit / 3;
     }

     if (vgaBitsPerPixel == 32) {
         memcpy(cirrusClockLimit, cirrusClockLimit32bpp,
             LASTCLGD * sizeof(int));
         if (vga256InfoRec.videoRam <= 1024)
             cirrusClockLimit[cirrusChip] = 0;
         else if (!HAVE546X()) {
	   /* Allow 45 MHz with MCLK = 0x1c, 50 MHz with MCLK = 0x1f+. */
	   /* This definition makes sense only with a chip that *has* an
	      MCLK! */
	   cirrusClockLimit[cirrusChip] = cirrusDRAMBandwidthLimit / 4;
	 }
     }

#endif

     /* 
      * The 62x5 chips can do marvelous things, but the
      * LCD panels connected to them don't leave much
      * option.  The following forces the cirrus chip to
      * use the slowest clock -- which appears to be what
      * my LCD panel likes best.  Faster clocks seem to
      * cause the LCD display to show noise when things are
      * moved around on the screen.
      *
      * XXX For later laptop chips for which lcd_is_on is set
      *     (e.g. 754x), this may be an artificial limit.
      *
      * The 754x laptop support seems to have been based on a
      * mistaken idea for clock selection, where the driver
      * needed to be hoaxed with a "Clocks" line.
      *
      * I have no idea what the real limits should be.
      */
     if (lcd_is_on && vgaBitsPerPixel <= 8 && !HAVE75XX()) {
         cirrusClockLimit[cirrusChip] = 44000;
     }

     cirrusClockNo = cirrusNumClocks(cirrusChip);
     if (vga256InfoRec.clocks) {
         ErrorF("%s %s: %s: Specifying a Clocks line makes no sense "
             "for this driver\n", XCONFIG_PROBED, vga256InfoRec.name,
	     vga256InfoRec.chipset);
	 vga256InfoRec.clocks = 0;
     }
     if (!vga256InfoRec.clocks)
          if (OFLG_ISSET(OPTION_PROBE_CLKS, &vga256InfoRec.options))
	       vgaGetClocks(cirrusClockNo, cirrusClockSelect);
	  else {
	       if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE,
	       &vga256InfoRec.clockOptions)) {
	           int last_valid_clock;
	           last_valid_clock = -1;
	           for (i = 0; i < NUM_CIRRUS_CLOCKS; i++) {
	               int freq;
	               freq = CLOCKVAL(cirrusClockTab[i].numer,
	                   cirrusClockTab[i].denom);
	               if (freq <= cirrusClockLimit[cirrusChip]) {
	                   vga256InfoRec.clock[i] = freq;
	                   last_valid_clock = i;
	               }
	               else
	                   vga256InfoRec.clock[i] = 0;
		   }
	           vga256InfoRec.clocks = last_valid_clock + 1;
	       }
	  }

#ifdef DPMSExtension
    if (HAVEALPINE() || HAVE75XX())
        vga256InfoRec.DPMSSet = CirrusDisplayPowerManagementSet;
#endif

     vga256InfoRec.bankedMono = TRUE;
#ifdef ALLOW_OUT_OF_SPEC_CLOCKS
     vga256InfoRec.maxClock = MAX_OUT_OF_SPEC_CLOCK;
#else
     vga256InfoRec.maxClock = cirrusClockLimit[cirrusChip];
#endif
     /* Initialize option flags allowed for this driver */
#ifdef ALLOW_OUT_OF_SPEC_CLOCKS
     OFLG_SET(OPTION_16CLKS, &CIRRUS.ChipOptionFlags);
     ErrorF("CIRRUS: Warning: Out of spec clocks can be enabled\n");
#endif
     OFLG_SET(OPTION_NOACCEL, &CIRRUS.ChipOptionFlags);
     OFLG_SET(OPTION_PROBE_CLKS, &CIRRUS.ChipOptionFlags);
     OFLG_SET(OPTION_LINEAR, &CIRRUS.ChipOptionFlags);
     OFLG_SET(OPTION_NOLINEAR_MODE, &CIRRUS.ChipOptionFlags);
     if ((cirrusChip >= CLGD5424 && cirrusChip <= CLGD5429) || HAVE543X() ||
	 HAVE75XX() || cirrusChip == CLGD5446 || cirrusChip == CLGD5480 ||
	 HAVE546X()) {
         OFLG_SET(OPTION_SLOW_DRAM, &CIRRUS.ChipOptionFlags);
         OFLG_SET(OPTION_MED_DRAM, &CIRRUS.ChipOptionFlags);
         OFLG_SET(OPTION_FAST_DRAM, &CIRRUS.ChipOptionFlags);
	 if (!HAVE546X()) {
	   OFLG_SET(OPTION_FIFO_CONSERV, &CIRRUS.ChipOptionFlags);
	   OFLG_SET(OPTION_FIFO_AGGRESSIVE, &CIRRUS.ChipOptionFlags);
	 }
#ifdef PC98
	 OFLG_SET(OPTION_EPSON_MEM_WIN,&CIRRUS.ChipOptionFlags);
	 OFLG_SET(OPTION_NEC_CIRRUS,&CIRRUS.ChipOptionFlags);
	 OFLG_SET(OPTION_GA98NB1,&CIRRUS.ChipOptionFlags);
	 OFLG_SET(OPTION_GA98NB2,&CIRRUS.ChipOptionFlags);
	 OFLG_SET(OPTION_GA98NB4,&CIRRUS.ChipOptionFlags);
	 OFLG_SET(OPTION_WAP,&CIRRUS.ChipOptionFlags);
#endif
     }
     if ((cirrusChip >= CLGD5426 && cirrusChip <= CLGD5429) || HAVE543X() ||
	 HAVE75XX() || cirrusChip == CLGD5446 || cirrusChip == CLGD5480) {
         OFLG_SET(OPTION_NO_2MB_BANKSEL, &CIRRUS.ChipOptionFlags);
         OFLG_SET(OPTION_NO_BITBLT, &CIRRUS.ChipOptionFlags);
         OFLG_SET(OPTION_FAVOUR_BITBLT, &CIRRUS.ChipOptionFlags);
         OFLG_SET(OPTION_NO_IMAGEBLT, &CIRRUS.ChipOptionFlags);
     }
#ifdef CIRRUS_SUPPORT_MMIO
     if (cirrusChip == CLGD5429 || HAVEALPINE() || HAVE546X()) {
         OFLG_SET(OPTION_MMIO, &CIRRUS.ChipOptionFlags);
         if (HAVEALPINE() && !(cirrusChip == CLGD7548 ||
         cirrusChip == CLGD7555))
             OFLG_SET(OPTION_NO_MMIO, &CIRRUS.ChipOptionFlags);
     }
#endif
     if (HAVE75XX()) {
         OFLG_SET(OPTION_LCD_STRETCH, &CIRRUS.ChipOptionFlags);
     }

     /* <scooper>
      *	The Hardware cursor, if the chip is capable, can be turned off using
      * the "sw_cursor" option.
      * Exception:  The HW cursor on the Laguna chip will not be allowed to
      * be turned off.  The reason is that there is a tight coupling between
      * the HW cursor and the virtual desktop (cirrusAdjust).  Without HW
      * cursor, the virtual desktop won't work.
      */

     if (Has_HWCursor(cirrusChip) && !HAVE546X()) {
        OFLG_SET(OPTION_SW_CURSOR, &CIRRUS.ChipOptionFlags);
     }


     if (HAVE546X()) {
       /* Initialize the interleaving variables right now.  The cursor code
	  (in cir_alloc:CirrusCursorAllocate()) will need to know what the
	  memory interleave, etc. are to properly allocate the cursor. */
       int screenPitch;    /* Padded screen pitch */

       screenPitch = cirrusFindPitchPadding(&i);
       if (screenPitch > 0)
	 cirrusTilesPerLineIndex = i;

       /* Set up the proper memory interleave.  The interleave is dependent
	  only upon how much memory there is installed. */
       switch (vga256InfoRec.videoRam) {
       case 1024:
       case 3072:
       case 5120:
       case 6144:
       case 7168:
	 /* One-way interleaving */
	 cirrusMemoryInterleave = 0x00;
	 break;
	      
       case 2048:
	 /* Two-way interleaving */
	 cirrusMemoryInterleave = 0x40;
	 break;

       case 4096:
       case 8192:
	 /* Four-way interleaving */
	 cirrusMemoryInterleave = 0x80;
	 break;
       } /* memory size switch statement */

     }

   
     /* If we found a laguna, install the pitch adjust hook.  If we don't,
	then we can't pad the screen pitch, and the display won't be correct
	for both cfb and accelerated output.  */
     if (HAVE546X())
       vgaSetPitchAdjustHook(cirrusPitchAdjust);

     return(TRUE);
}



static int cirrusFindPitchPadding(int *index)
{
  /* Find the smallest tile-line-pitch such that the total byte pitch
     is greater than or equal to virtualX*Bpp. */
  int i;
  int bestPitch = 0;

  /* Some pitch sizes are duplicates in the table.  BUT, the invariant is 
     that the _first_ time a pitch occurs in the table is always _before_
     all other pitches greater than it.  Said in another way... if all 
     duplicate entries from the table were removed, then the resulting pitch
     values are strictly increasing. */

  for (i = 0; cirrusTilesPerLineTab[i].pitch > 0; i++)
    if (cirrusTilesPerLineTab[i].pitch >= 
	vga256InfoRec.virtualX*vgaBitsPerPixel>>3) {
      bestPitch = cirrusTilesPerLineTab[i].pitch;
      break;
    }

  if (index)
    *index = i;

  return bestPitch;
}


int cirrusPitchAdjust(void)
{
  /* In the Laguna family, where memory is tiled, each scanline must be
     an integer number of tiles wide (tile width =  128 or 256
     bytes).  Furthermore, there are only a few tile pitches
     allowed.  It just so happens that none of these tile pitches
     yield a screen byte pitch that is divisible by 3.  The upshot:
     you can't have 24bpp if the screen pitch is not a multiple of 
     three -- i.e., if the screen pitch is not an integer number
     of _pixels_. */

  int bestPitch = cirrusFindPitchPadding(NULL);

  ErrorF("%s %s: %s: Display width padded to %d bytes.\n",
	 XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset,
	 bestPitch);

/*
  return vga256InfoRec.virtualX;
  */
  return bestPitch / (vgaBitsPerPixel>>3);
}

#ifndef MONOVGA
extern GCOps cfb16TEOps1Rect, cfb16TEOps, cfb16NonTEOps1Rect, cfb16NonTEOps;
extern GCOps cfb24TEOps1Rect, cfb24TEOps, cfb24NonTEOps1Rect, cfb24NonTEOps;
extern GCOps cfb32TEOps1Rect, cfb32TEOps, cfb32NonTEOps1Rect, cfb32NonTEOps;
#endif

/*
 * cirrusFbInit --
 *      enable speedups for the chips that support it
 */
static void
cirrusFbInit()
{
  int size;

#ifndef MONOVGA
  int useSpeedUp;

  vgaSetScreenInitHook(cirrusScreenInit);

  useSpeedUp = vga256InfoRec.speedup & SPEEDUP_ANYWIDTH;
  

  cirrusUseBLTEngine = FALSE;
  if (cirrusChip == CLGD5426 || cirrusChip == CLGD5428 ||
      cirrusChip == CLGD5429 || HAVE543X() || HAVE75XX() ||
      cirrusChip == CLGD5446 || cirrusChip == CLGD5480)
      {
      cirrusUseBLTEngine = TRUE;
      if (OFLG_ISSET(OPTION_NO_BITBLT, &vga256InfoRec.options))
          cirrusUseBLTEngine = FALSE;
      else {
          if (cirrusChip == CLGD5429 && cirrusChipRevision == 0) {
              ErrorF("%s %s: %s: CL-GD5429 Rev A detected\n",
                XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.chipset);
              cirrusAvoidImageBLT = TRUE;
              }
          }
          if (OFLG_ISSET(OPTION_NO_IMAGEBLT, &vga256InfoRec.options))
              cirrusAvoidImageBLT = TRUE;
          if (xf86Verbose && cirrusAvoidImageBLT)
              ErrorF("%s %s: %s: Not using system-to-video BitBLT\n",
                XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.chipset);
     }

#endif

    if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
        if (xf86Verbose)
            ErrorF("%s %s: %s: Using programmable clocks\n",
	        XCONFIG_PROBED, vga256InfoRec.name,
	        vga256InfoRec.chipset);

  /*
   * Report the internal MCLK value of the card, and change it if the
   * "fast_dram" or "slow_dram" option is defined.
   */
  if (cirrusChip == CLGD5424 || cirrusChip == CLGD5426 ||
      cirrusChip == CLGD5428 || cirrusChip == CLGD5429 ||
      cirrusChip == CLGD5446 || cirrusChip == CLGD5480 ||
      HAVE543X() || HAVE75XX())
      {
      unsigned char SRF, SR1F;
      outb(0x3c4, 0x0f);
      SRF = inb(0x3c5);
      outb(0x3c4, 0x1f);
      SR1F = inb(0x3c5);
      if (xf86Verbose)
          ErrorF(
              "%s %s: %s: Internal memory clock register is 0x%02x (%s RAS)\n",
              XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset,
              SR1F & 0x3f, (SRF & 4) ? "Standard" : "Extended");
      
      if (cirrusReprogrammedMCLK > 0)
      	  /*
      	   * The MCLK will be programmed to a different value.
      	   * 
      	   * 0x1c, 51 MHz	Option "slow_dram"
      	   * 0x1f, 55 MHz	Option "med_dram"
      	   * 0x22, 61 MHz	Option "fast_dram"
      	   * 0x25, 66 MHz
      	   * 
	   * The official spec for the 542x is 50 MHz, but some cards are
	   * overclocked.
      	   *
      	   * The 5434 is specified for 50 MHz, but new revisions can do
      	   * 60 MHz in packed-pixel mode. The 5429 and 5430 are probably
      	   * speced for 60 MHz.
      	   */
	  if (xf86Verbose) {
	      if (cirrusChip == CLGD5434 && cirrusChipRevision >= 0x01) {
                  ErrorF("%s %s: %s: CL-GD5434 rev. E+, will program 0x22 MCLK\n",
                      XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
	          if (OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options) ||
	          OFLG_ISSET(OPTION_MED_DRAM, &vga256InfoRec.options) ||
	          OFLG_ISSET(OPTION_SLOW_DRAM, &vga256InfoRec.options))
                  ErrorF("%s %s: %s: Memory clock overridden by option\n",
                      XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.chipset);
	      }
              ErrorF("%s %s: %s: Internal memory clock register set to 0x%02x\n",
                XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.chipset,
                cirrusReprogrammedMCLK);
	  }
      }

#ifndef MONOVGA

    if (xf86Verbose && !HAVE546X()) {
        ErrorF("%s %s: %s: Approximate DRAM bandwidth for drawing: %d of %d MB/s\n",
            XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.chipset,
            (cirrusDRAMBandwidth - vga256InfoRec.clock[
            vga256InfoRec.modes->Clock] * vgaBitsPerPixel / 8) / 1000,
            cirrusDRAMBandwidth / 1000);
      }

#ifdef CIRRUS_SUPPORT_LINEAR
    /*
     * Linear memory is required for the laguna chips, so it's not an
     * 'option'. We now enable linear addressing as a default for all
     * chips on PCI bus.
     */
    if (xf86LinearVidMem() &&
        (!OFLG_ISSET(OPTION_NOLINEAR_MODE, &vga256InfoRec.options)) &&
	(OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options) ||
	cirrusBusType == CIRRUS_BUS_PCI)) {
        cirrusUseLinear = TRUE;
        if (vga256InfoRec.MemBase != 0)
            CIRRUS.ChipLinearBase = vga256InfoRec.MemBase;
        else
            if (HAVEALPINE() || cirrusChip == CLGD7543 ||
		HAVE546X()) {
                if (cirrusBusType == CIRRUS_BUS_ISA) {
		    ErrorF("%s %s: %s: Must specify MemBase for ISA bus linear "
		           "addressing\n", XCONFIG_PROBED,
		           vga256InfoRec.name, vga256InfoRec.chipset);
		    cirrusUseLinear = FALSE;
                    goto nolinear;
		}
                if (cirrusBusType == CIRRUS_BUS_VLB) {
                    /* 64MB is standard. */
        	    /* A documented jumper option is 2048MB. */
        	    /* 32MB is an option for more recent cards. */
        	    if (cirrusChip == CLGD7543)
        	        CIRRUS.ChipLinearBase = 0x03E00000; /* Like 5429. */
        	    else
        	        CIRRUS.ChipLinearBase = 0x04000000;	/* 64MB */
        	}
                if (cirrusBusType == CIRRUS_BUS_PCI) {
		  cirrusUseLinear = FALSE;
		  if (vgaPCIInfo && vgaPCIInfo->Vendor == PCI_VENDOR_CIRRUS) {
		    /* The later Cirrus PCI chips don't set the lower
		       order bit of the PCI base registers as expected by
		       XFree86.  This was done (by Cirrus) by design.  It's 
		       okay, though, because we know how the registers 
		       should be decoded in these cases. */
		    if ((vgaPCIInfo->ChipType == PCI_CHIP_GD5462) ||
			(vgaPCIInfo->ChipType == PCI_CHIP_GD5464) ||
			(vgaPCIInfo->ChipType == PCI_CHIP_GD5464BD) ||
			(vgaPCIInfo->ChipType == PCI_CHIP_GD5465) ||
			(vgaPCIInfo->ChipType == PCI_CHIP_GD7548)) {

		      if (vgaPCIInfo->ChipType == PCI_CHIP_GD5465) {
			/* Swapped in the '65, by design. */
			vgaPCIInfo->IOBase = vgaPCIInfo->ThisCard->_base1;
			vgaPCIInfo->MemBase = vgaPCIInfo->ThisCard->_base0;
		      } else {
			vgaPCIInfo->IOBase = vgaPCIInfo->ThisCard->_base0;
			vgaPCIInfo->MemBase = vgaPCIInfo->ThisCard->_base1;
		      }

		      ErrorF("%s %s: PCI: ", XCONFIG_PROBED, 
			     vga256InfoRec.name);
		      if (vgaPCIInfo->MemBase)
			ErrorF("Memory @ 0x%08x", vgaPCIInfo->MemBase);
		      if (vgaPCIInfo->IOBase)
			ErrorF(", I/O @ 0x%08x", vgaPCIInfo->IOBase);
		      ErrorF("\n");
		      
		    }

                    	/*
                    	 * Known devices:
                    	 * 0x00A0	5430, 5440
                    	 * 0x00A8	5434
                    	 * 0x00AC	5436
			 * 0x00B8       5446
 			 * 0x00BC       5480
			 * 0x00D0       5462
			 * 0x00D4       5464
			 * 0x00D5       5464BD
			 * 0x00D6       5465
      			 */
                    	if (vgaPCIInfo->MemBase != 0) {
			  CIRRUS.ChipLinearBase =
			    vgaPCIInfo->MemBase & 0xFF000000;
			  cirrusUseLinear = TRUE;
                    	}
			else
			    ErrorF("%s %s: %s: Can't find valid PCI "
			        "Base Address\n", XCONFIG_PROBED,
			        vga256InfoRec.name, vga256InfoRec.chipset);
                    } else
		        ErrorF("%s %s: %s: Can't find PCI device in "
		               "configuration space\n", XCONFIG_PROBED,
		               vga256InfoRec.name, vga256InfoRec.chipset);
		    if (!cirrusUseLinear)
			goto nolinear;
		}
	    }
            else {
                /* Some recent 542x-based cards should map at 64MB, others */
                /* can only map at 14MB. */
                if (cirrusChip == CLGD5429)
        	    CIRRUS.ChipLinearBase = 0x03E00000;		/* 62MB */
        	else {
	            ErrorF("%s %s: %s: Must specify MemBase for 542x linear "
		           "addressing\n", XCONFIG_PROBED,
		           vga256InfoRec.name, vga256InfoRec.chipset);
		    cirrusUseLinear = FALSE;
                    goto nolinear;
                }
            }
        CIRRUS.ChipLinearSize = vga256InfoRec.videoRam * 1024;
        if (xf86Verbose)
            ErrorF("%s %s: %s: Using linear framebuffer at 0x%08x (%dMB)\n",
	        OFLG_ISSET(XCONFIG_MEMBASE, &vga256InfoRec.xconfigFlag) ?
		XCONFIG_GIVEN : XCONFIG_PROBED,
		vga256InfoRec.name, vga256InfoRec.chipset,
	        CIRRUS.ChipLinearBase, (unsigned int)CIRRUS.ChipLinearBase
	        / (1024 * 1024));
	if (cirrusChip >= CLGD5422 && cirrusChip <= CLGD5428 &&
	OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options))
            ErrorF("%s %s: %s: Warning: fast_dram option not recommended "
                "with linear addressing\n",
                XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.chipset);
    }
nolinear:
    if (cirrusUseLinear)
        CIRRUS.ChipUseLinearAddressing = TRUE;
#endif

  CirrusMemTop = vga256InfoRec.displayWidth * vga256InfoRec.virtualY
      * (vgaBitsPerPixel / 8);
  size = CirrusInitializeAllocator(CirrusMemTop);

  if (xf86Verbose)
    ErrorF("%s %s: %s: %d bytes off-screen memory available\n",
	   XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset, size);
    
  if (Has_HWCursor(cirrusChip) &&
      !OFLG_ISSET(OPTION_SW_CURSOR, &vga256InfoRec.options) &&
      (vgaBitsPerPixel != 24 || cirrusChip == CLGD5436 || 
       cirrusChip == CLGD5446 || cirrusChip == CLGD5480))
    {
#if 1
      if (HasLargeHWCursor(cirrusChip))
	{
	  cirrusCur.cur_size = 1;
	  cirrusCur.width = 64;
	  cirrusCur.height = 64;
	}
      else
#endif
	{
	  cirrusCur.cur_size = 0;
	  cirrusCur.width = 32;
	  cirrusCur.height = 32;
	}

      if (!CirrusCursorAllocate(&cirrusCur))
	{	
	  vgaHWCursor.Initialized = TRUE;
	  vgaHWCursor.Init = cirrusCursorInit;
	  vgaHWCursor.Restore = cirrusRestoreCursor;
	  vgaHWCursor.Warp = cirrusWarpCursor;  
	  vgaHWCursor.QueryBestSize = cirrusQueryBestSize;

	  if (xf86Verbose)
	    {
	      ErrorF( "%s %s: %s: Using hardware cursor\n",
		     XCONFIG_PROBED, vga256InfoRec.name,
		     vga256InfoRec.chipset);
	    }
	}
      else
	{
	  ErrorF( "%s %s: %s: Failed to allocate hardware cursor in offscreen ram,\n\ttry reducing the virtual screen size\n",
		 XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
	}
    }	  

  if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options)
      && !(cirrusChip == CLGD5420 && cirrusChipRevision == 1)) {
    if (xf86Verbose)
      {
        ErrorF ("%s %s: %s: Using accelerator functions\n",
		XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
      }

    /* Accel functions are available on all chips except 5420-75QC-B; some
     * use the BitBLT engine if available. */

    if (HAVE546X()) {
      /* The 2D engine in Laguna family is radically different than that in 
	 the Alpine family chips.  Instead of having a huge tangle of if's 
	 and switch statements in one monolithic code module, the Laguna
	 accelerated functions are split off on their own.  Module names
	 are cir_*LG.c; the common header is cir_blitLG.h */

      /* Disable old-style acceleration for Laguna chips. */
#if 0
      if (vgaBitsPerPixel == 8) {
	vga256LowlevFuncs.doBitbltCopy = CirrusLgDoBitbltCopy;
	vga256LowlevFuncs.fillRectSolidCopy = CirrusLgFillRectSolidCopy;
	vga256LowlevFuncs.fillBoxSolid = CirrusLgFillBoxSolid;

	/* Hook special op. fills (and tiles): */
	vga256TEOps1Rect.PolyFillRect = CirrusLgPolyFillRect;
	vga256NonTEOps1Rect.PolyFillRect = CirrusLgPolyFillRect;
	vga256TEOps.PolyFillRect = CirrusLgPolyFillRect;
	vga256NonTEOps.PolyFillRect = CirrusLgPolyFillRect;

/*
	vga256TEOps1Rect.PolyGlyphBlt = CirrusLgPolyGlyphBlt;
        vga256TEOps.PolyGlyphBlt = CirrusLgPolyGlyphBlt;
*/

	/*
	 * If using the BitBLT engine but avoiding image blit,
	 * prefer the framebuffer routines for ImageGlyphBlt.
	 * The color-expand text functions would be allowable, but
	 * the use of "no_imageblt" generally implies a fast host bus.
	 */
/*
	if (!cirrusAvoidImageBLT) {
	  vga256LowlevFuncs.teGlyphBlt8 = CirrusLgImageGlyphBlt;
	  vga256TEOps1Rect.ImageGlyphBlt = CirrusLgImageGlyphBlt;
	  vga256TEOps.ImageGlyphBlt = CirrusLgImageGlyphBlt;
	}
*/
      } else if (vgaBitsPerPixel == 16 ||
		 vgaBitsPerPixel == 24 ||
		 vgaBitsPerPixel == 32) {

	/* Hook special op. fills (and tiles): */
	cfb32TEOps1Rect.PolyFillRect = CirrusLgPolyFillRect;
	cfb32NonTEOps1Rect.PolyFillRect = CirrusLgPolyFillRect;
	cfb32TEOps.PolyFillRect = CirrusLgPolyFillRect;
	cfb32NonTEOps.PolyFillRect = CirrusLgPolyFillRect;
      }
#endif	/* Disable legacy acceleration for Laguna chips. */

    /*
     * Old-style acceleration for chips that don't have a BitBLT
     * engine. Only supported at 8bpp.
     */

    } 
    else if (vgaBitsPerPixel == 8 && !cirrusUseBLTEngine) {
      vga256LowlevFuncs.doBitbltCopy = CirrusDoBitbltCopy;
      vga256LowlevFuncs.fillRectSolidCopy = CirrusFillRectSolidCopy;
      vga256LowlevFuncs.fillBoxSolid = CirrusFillBoxSolid;

      /* Hook special op. fills (and tiles): */
      vga256TEOps1Rect.PolyFillRect = CirrusPolyFillRect;
      vga256NonTEOps1Rect.PolyFillRect = CirrusPolyFillRect;
      vga256TEOps.PolyFillRect = CirrusPolyFillRect;
      vga256NonTEOps.PolyFillRect = CirrusPolyFillRect;

      vga256TEOps1Rect.PolyGlyphBlt = CirrusPolyGlyphBlt;
      vga256TEOps.PolyGlyphBlt = CirrusPolyGlyphBlt;
      vga256LowlevFuncs.teGlyphBlt8 = CirrusImageGlyphBlt;
      vga256TEOps1Rect.ImageGlyphBlt = CirrusImageGlyphBlt;
      vga256TEOps.ImageGlyphBlt = CirrusImageGlyphBlt;
    }

#if 0
    CirrusInvalidateShadowVariables();
#endif

    if (cirrusUseBLTEngine || HAVE546X())
	if (xf86Verbose) {
	    ErrorF("%s %s: %s: Using BitBLT engine\n",
	        XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset);
	}

    /* Optional Memory-Mapped I/O. */
    /* MMIO is _not_ optional for 546X chips.  But it doesn't hurt anything
       anyway, unless you happen to have about 4GB of RAM */
    /* Register is set in init function. */
    if (((cirrusChip == CLGD5429 || cirrusChip == CLGD7548 ||
    cirrusChip == CLGD7555) &&
    OFLG_ISSET(OPTION_MMIO, &vga256InfoRec.options))
    || ((HAVEALPINE() && !(cirrusChip == CLGD7548 || cirrusChip == CLGD7555)) &&
    !(OFLG_ISSET(OPTION_NO_MMIO, &vga256InfoRec.options)))
    || HAVE546X()) {
        cirrusUseMMIO = TRUE;
	
	if (cirrusBusType == CIRRUS_BUS_PCI && HAVE546X()) {
	  /* The MMIO address lives in a PCI base address register */
	  
	  /* !!! what's scr_index? */
	  if (!cirrusMMIOBase)
	    cirrusMMIOBase = 
	      xf86MapVidMem(0, EXTENDED_REGION, (pointer)vgaPCIInfo->IOBase, 0x4000);
	  ErrorF("%s %s: %s: Using memory-mapped I/O at address 0x%08X\n",
		 XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.chipset,
		 (unsigned char *)vgaPCIInfo->IOBase);
	} else {
	    /*
	     * Map the memory-mapped I/O space at 0xB800. Newer Alpine
	     * family chips also support mapping at the end of the linear
	     * framebuffer. This might be preferable for performance.
	     */
 	    if (!cirrusMMIOBase)
                cirrusMMIOBase = xf86MapVidMem(0, EXTENDED_REGION,
                    (pointer)0xB8000, 0x1000);
	    ErrorF("%s %s: %s: Using memory-mapped I/O\n",
		 XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.chipset);
	}

 	/* Should the Rambus memory clock be adjusted?  */
 	if (HAVE546X()) {
 	  /* By default, don't do anything to the BCLK register.  However,
 	     if the {slow,med,fast}_dram options have been set, respect that
 	     option. */
 	  if (cirrusChip == CLGD5465) {
 	    cirrusLgBCLK = *(unsigned char *)(cirrusMMIOBase + 0x2C0) & 0x1F;
 	  } else {
 	    cirrusLgBCLK = *(unsigned char *)(cirrusMMIOBase + 0x8C) & 0x1F;
 	  }
 
 	  /*** WARNING! WARNING! DANGER, WILL ROBINSON! DANGER! *****/
 	  /*
 	     Setting the Rambus BCLK too high for your video card is 
 	     _very_ bad.  Doing so will cause an undue amount of heat
 	     to be produced by the part, and may very likely boil it to 
 	     pieces, frying your video board, motherboard, and maybe 
 	     even your monitor.
        
 	     I, Corin Anderson, take absolutely _no_ responsibility
 	     for any damages which might come from pushing the BCLK
 	     too high.
 	     */
 
 	  /* 
 	     That warning said, I honestly don't think anyone will
 	     be melting their Laguna chips very easily.  The 5462 and
 	     5464 are rated up to 300MHz, but the Rambus memory 
 	     sometimes is flaky at that point.  286MHz is usually a
 	     safe value.  For the 5465, the Rambus memory seems to be 
 	     more stable at the 300MHz value, which is I think the 
 	     BIOS default speed during power-on setup.
        
 	     I include this warning only to future programmers who
 	     wonder what might happen if the BCLK values are
 	     pushed up a little.  You do so at your own risk.  8)
 	     */
 	  
 	  if (OFLG_ISSET(OPTION_SLOW_DRAM, &vga256InfoRec.options)) {
 	    /* Use a safe (maybe even default) BCLK:  258MHz */
 	    cirrusLgBCLK = 0x12;  /*  18*14.31818 = 257.727 */
 	  } else if (OFLG_ISSET(OPTION_MED_DRAM, &vga256InfoRec.options)) {
 	    /* Middle of the road speed.  Pretty safe: 272MHz */
 	    cirrusLgBCLK = 0x13;  /*  19*14.31818 = 272.046 */
 	  } else if (OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options)) {
 	    /* Fastest mode that I've tested without melting the
 	       chip. */
 	    if (cirrusChip == CLGD5465)
 	      cirrusLgBCLK = 0x15;  /*  21*14.31818 = 300.682 */
 	    else
 	      cirrusLgBCLK = 0x14;  /*  20*14.31818 = 286.364 */
 	  }
 
 	  /* Overload the cirrusDRAMBandwidth variable to record the 
 	     bandwidth available.  Rambus memory samples the clock at 
 	     both the rising and falling edges of the BCLK, to the bandwidth
 	     is twice BCLK * 14.31818MHz. */
 	  cirrusDRAMBandwidth = 14318 * cirrusLgBCLK * 2;
 	  
 
 	  if (xf86Verbose) {
 	    if (OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options) ||
 		OFLG_ISSET(OPTION_MED_DRAM, &vga256InfoRec.options) ||
 		OFLG_ISSET(OPTION_SLOW_DRAM, &vga256InfoRec.options))
 	      ErrorF("%s %s: %s: Memory clock overridden by option\n",
 		     XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.chipset);
 	    ErrorF("%s %s: %s: Internal memory clock register set to 0x%02x\n",
 		   XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.chipset,
 		   cirrusLgBCLK);
 	    ErrorF("%s %s: %s: Approximate DRAM bandwidth for drawing: "
 		   "%d of %d MB/s\n", XCONFIG_GIVEN, vga256InfoRec.name, 
 		   vga256InfoRec.chipset, (cirrusDRAMBandwidth - 
 		   vga256InfoRec.clock[vga256InfoRec.modes->Clock] * 
 		   vgaBitsPerPixel / 8) / 1000, cirrusDRAMBandwidth / 1000);
 	  }
 	} /* HAVE546X() */
 
 
	/* Disable legacy acceleration using MMIO. */
#if 0
        if (cirrusUseBLTEngine) {
            if (vgaBitsPerPixel == 8) {
                vga256TEOps1Rect.PolyGlyphBlt = CirrusMMIOPolyGlyphBlt;
                vga256TEOps.PolyGlyphBlt = CirrusMMIOPolyGlyphBlt;
		if (!cirrusAvoidImageBLT) {
                    vga256LowlevFuncs.teGlyphBlt8 = CirrusMMIOImageGlyphBlt;
                    vga256TEOps1Rect.ImageGlyphBlt = CirrusMMIOImageGlyphBlt;
                    vga256TEOps.ImageGlyphBlt = CirrusMMIOImageGlyphBlt;
                }
		/* These functions need to be initialized in the GC handling */
		/* of vga256. */
	        vga256TEOps1Rect.FillSpans = CirrusFillSolidSpansGeneral;
	        vga256TEOps.FillSpans = CirrusFillSolidSpansGeneral;
	        vga256LowlevFuncs.fillSolidSpans = CirrusFillSolidSpansGeneral;

		vga256TEOps1Rect.Polylines = CirrusMMIOLineSS;
		vga256NonTEOps1Rect.Polylines = CirrusMMIOLineSS;
		vga256TEOps.Polylines = CirrusMMIOLineSS;
		vga256NonTEOps.Polylines = CirrusMMIOLineSS;
		vga256TEOps1Rect.PolySegment = CirrusMMIOSegmentSS;
		vga256NonTEOps1Rect.PolySegment = CirrusMMIOSegmentSS;
		vga256TEOps.PolySegment = CirrusMMIOSegmentSS;
		vga256TEOps.PolySegment = CirrusMMIOSegmentSS;

		vga256TEOps1Rect.PolyRectangle = Cirrus8PolyRectangle;
		vga256NonTEOps1Rect.PolyRectangle = Cirrus8PolyRectangle;
		vga256TEOps.PolyRectangle = Cirrus8PolyRectangle;
		vga256NonTEOps.PolyRectangle = Cirrus8PolyRectangle;
#if 0
	        vga256LowlevFuncs.fillRectSolidCopy = CirrusMMIOFillRectSolid;
	        vga256LowlevFuncs.fillBoxSolid = CirrusMMIOFillBoxSolid;
#endif
             }
            else
            if (vgaBitsPerPixel == 16) {
                if (!cirrusAvoidImageBLT) {
		    cfb16TEOps1Rect.ImageGlyphBlt = CirrusMMIOImageGlyphBlt;
	            cfb16TEOps.ImageGlyphBlt = CirrusMMIOImageGlyphBlt;
	        }
	        cfb16TEOps1Rect.FillSpans = CirrusFillSolidSpansGeneral;
	        cfb16TEOps.FillSpans = CirrusFillSolidSpansGeneral;
	        cfb16NonTEOps1Rect.FillSpans = CirrusFillSolidSpansGeneral;
	        cfb16NonTEOps.FillSpans = CirrusFillSolidSpansGeneral;
	        cfb16TEOps1Rect.PolyFillRect = CirrusPolyFillRect;
	        cfb16TEOps.PolyFillRect = CirrusPolyFillRect;
	        cfb16NonTEOps1Rect.PolyFillRect = CirrusPolyFillRect;
	        cfb16NonTEOps.PolyFillRect = CirrusPolyFillRect;
#if 0
	        cfb16TEOps1Rect.PolyRectangle = Cirrus16PolyRectangle;
	        cfb16TEOps.PolyRectangle = Cirrus16PolyRectangle;
	        cfb16NonTEOps1Rect.PolyRectangle = Cirrus16PolyRectangle;
	        cfb16NonTEOps.PolyRectangle = Cirrus16PolyRectangle;
#endif
            }
            else
	    if (vgaBitsPerPixel == 32) {
                if (!cirrusAvoidImageBLT) {
		    cfb32TEOps1Rect.ImageGlyphBlt = CirrusMMIOImageGlyphBlt;
	            cfb32TEOps.ImageGlyphBlt = CirrusMMIOImageGlyphBlt;
	        }
	        cfb32TEOps1Rect.FillSpans = CirrusFillSolidSpansGeneral;
	        cfb32TEOps.FillSpans = CirrusFillSolidSpansGeneral;
	        cfb32NonTEOps1Rect.FillSpans = CirrusFillSolidSpansGeneral;
	        cfb32NonTEOps.FillSpans = CirrusFillSolidSpansGeneral;
	        cfb32TEOps1Rect.PolyFillRect = CirrusPolyFillRect;
	        cfb32TEOps.PolyFillRect = CirrusPolyFillRect;
	        cfb32NonTEOps1Rect.PolyFillRect = CirrusPolyFillRect;
	        cfb32NonTEOps.PolyFillRect = CirrusPolyFillRect;
#if 0
	        cfb32TEOps1Rect.PolyRectangle = Cirrus32PolyRectangle;
	        cfb32TEOps.PolyRectangle = Cirrus32PolyRectangle;
	        cfb32NonTEOps1Rect.PolyRectangle = Cirrus32PolyRectangle;
	        cfb32NonTEOps.PolyRectangle = Cirrus32PolyRectangle;
#endif
            }
        }
#endif	/* Disable legacy acceleration using MMIO. */
    }

      /*
       * We get here if Option "noaccel" is not set.
       * Enable XAA acceleration for chips with a BitBLT engine.
       * If linear addressing is enabled, also enable the scratch buffer
       * that is used for some operations.
       */
    if (HAVE546X()) {
      LagunaAccelInit();
    } else if (cirrusUseBLTEngine) {
          cirrusBufferSpaceAddr = 0;
          cirrusBufferSpaceSize = 0;
#if 0
	  /*
	   * The scratch buffer is not used anymore (post 3.2A).
	   * Don't allocate it.
	   */
          if (cirrusUseLinear &&
          vga256InfoRec.videoRam * 1024 - CirrusMemTop >= 16384) {
              cirrusBufferSpaceAddr = vga256InfoRec.videoRam * 1024 - 16384;
	      cirrusBufferSpaceSize = 16384 - 1024;
	  }
#endif
	  if (cirrusUseMMIO)
              CirrusAccelInitMMIO();
          else
              CirrusAccelInit();
      }
  }
  else { /* OPTION_NOACCEL */
	/* MMIO is _not_ optional for 546X chips.  But it doesn't hurt anything
	   anyway, unless you happen to have about 4GB of RAM */
	/* Register is set in init function. */
	if (HAVE546X()) {
	  cirrusUseMMIO = TRUE;
	  
	  if (cirrusBusType == CIRRUS_BUS_PCI) {
	    /* The MMIO address lives in a PCI base address register */
	  
	    /* !!! what's scr_index? */
	    if (!cirrusMMIOBase)
	      cirrusMMIOBase = 
		xf86MapVidMem(0, EXTENDED_REGION, (pointer)vgaPCIInfo->IOBase, 0x4000);
	    ErrorF("%s %s: %s: Using memory-mapped I/O at address 0x%08X\n"
		   "\tmapped to 0x%08X\n",
		   XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.chipset,
		   (unsigned char *)vgaPCIInfo->IOBase, cirrusMMIOBase);
	  } else {
	    /* We shouldn't get here.  This case is where we (1) have
	       a 546X, but (2) don't have a PCI card.  We won't handle
	       that case right now, and hope that it never comes to 
	       haunt us. */

	    ErrorF("%s: %s: CL-GD546X found on a non-PCI bus.  Please report\n"
		   "card information (name, manufacturer, bus) to "
		   "corina@bdc.cirrus.com.\n",
		   vga256InfoRec.name, vga256InfoRec.chipset);
	  }
	}
      }

#endif		/* not MONOVGA */
}

/*
 * cirrusEnterLeave -- 
 *      enable/disable io-mapping
 */
static void 
cirrusEnterLeave(enter)
     Bool enter;
{
  static unsigned char temp;

#ifndef MONOVGA
#ifdef XFreeXDGA
  if (vga256InfoRec.directMode&XF86DGADirectGraphics && !enter) {
      cirrusHideCursor();
      return;
  }
#endif
#endif
  if (enter)
       {

       xf86EnableIOPorts(vga256InfoRec.scrnIndex);

#ifdef PC98
       crtswitch(1);
#else
				/* Are we Mono or Color? */
       vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;

       if (HAVE546X()) {
	 /* Set up the mapping for the Memory-mapped IO */
	 if (!cirrusMMIOBase)
	   cirrusMMIOBase = xf86MapVidMem(0, EXTENDED_REGION, 
					  (pointer)vgaPCIInfo->IOBase, 0x4000);
	 /* Unlock the laguna with a PCI command register write */

	 if (cirrusMMIOBase) {
	   unsigned short *pW = (unsigned short *)(cirrusMMIOBase + 0x304);
	   unsigned short temp; /* 16 bits */
	   temp = *pW;
	   temp |= 0x0003;  /* Set bits 0,1 */
	   *pW = temp;
	 }
       } else {
	 outb(0x3C4,0x06);
	 outb(0x3C5,0x12);	 /* unlock cirrus special */
       }
#endif

				/* Put the Vert. Retrace End Reg in temp */

       outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);

				/* Put it back with PR bit set to 0 */
				/* This unprotects the 0-7 CRTC regs so */
				/* they can be modified, i.e. we can set */
				/* the timing. */

       outb(vgaIOBase + 5, temp & 0x7F);

    }
  else /* !enter == leave */
    {

      if (HAVE546X()) {
	/* Don't lock the PCI command register, because then
	   we won't get our good old text screen back!  The 
	   problem is that, by locking the PCI command register,
	   the I/O space is locked, and normal VGA I/O is 
	   ignored. */

#ifndef MONOVGA
	cirrusHideCursor();	
#endif

	if (cirrusMMIOBase) {
	  /* Unmap the MMIO space */
	  xf86UnMapVidMem(0, EXTENDED_REGION, cirrusMMIOBase, 0x4000);
	  cirrusMMIOBase = NULL;
	}
      } else {
	outb(0x3C4,0x06);
	outb(0x3C5,0x0F);	 /*relock cirrus special */
      }

#ifdef PC98
       crtswitch(0);
#endif

       xf86DisableIOPorts(vga256InfoRec.scrnIndex);
    }
}

static void cirrusDumpRegs(vgacirrusPtr mode) ;
/*
 * cirrusRestore -- 
 *      restore a video mode
 */
static void 
cirrusRestore(restore)
  vgacirrusPtr restore;
{
  unsigned char i;
#if 0
  static int count = 0;

  count++;

  if (count == 3) {
    Blt();
    sleep(5);
  }
#endif

  vgaProtect(TRUE);

  outw(0x3CE, 0x0009);	/* select bank 0 */
  outw(0x3CE, 0x000A);

  outb(0x3C4,0x0F);		/* Restoring this registers avoids */
  outb(0x3C5,restore->SRF);	/* textmode corruption on 2Mb cards. */

  outb(0x3C4,0x07);		/* This will disable linear addressing */
  outb(0x3C5,restore->SR7);	/* if enabled. */

#ifndef MONOVGA
#ifdef ALLOW_8BPP_MULTIPLEXING
  if (((cirrusChip == CLGD5434 || cirrusChip == CLGD5436 ||
	cirrusChip == CLGD5446 || cirrusChip == CLGD5480) ||
       vgaBitsPerPixel != 8) && !HAVE546X()) {
      outb(0x3c6, 0x00);
      outb(0x3c6, 0xff);
      inb(0x3c6); inb(0x3c6); inb(0x3c6); inb(0x3c6);
      outb(0x3c6, restore->HIDDENDAC);
  }
#else
  if (vgaBitsPerPixel != 8 && !HAVE546X()) {
      /*
       * Write to DAC. This is very delicate, and the it can lock up
       * the bus if not done carefully. The access count for the DAC
       * register can be such that the first write accesses either the
       * VGA LUT pixel mask register or the Hidden DAC register.
       */
      outb(0x3c6, 0x00);	/* Reset access count. */
      outb(0x3c6, 0xff);	/* Write 0xff to pixel mask. */
      inb(0x3c6); inb(0x3c6); inb(0x3c6); inb(0x3c6);
      outb(0x3c6, restore->HIDDENDAC);
  }
#endif
#endif

  /*
   * Restore a few standard VGA register to help textmode font restoration.
   */
  outb(0x3ce, 0x08);
  outb(0x3cf, restore->std.Graphics[0x08]);
  outb(0x3ce, 0x00);
  outb(0x3cf, restore->std.Graphics[0x00]);
  outb(0x3ce, 0x01);
  outb(0x3cf, restore->std.Graphics[0x01]);
  outb(0x3ce, 0x02);
  outb(0x3cf, restore->std.Graphics[0x02]);
  outb(0x3ce, 0x05);
  outb(0x3cf, restore->std.Graphics[0x05]);

  vgaHWRestore((vgaHWPtr)restore);

/*  unsigned char GR9;		 Graphics Offset1 */
/*  unsigned char GRA;		 Graphics Offset2 */
/*  unsigned char GRB;		 Graphics Extensions Control */
/*  unsigned char SR7;		 Extended Sequencer */
/*  unsigned char SRE;		 VCLK Numerator (546X's denominator) */
/*  unsigned char SRF;		 DRAM Control */
/*  unsigned char SR10;		 Graphics Cursor X Position [7:0]         */
/*  unsigned char SR10E;	 Graphics Cursor X Position [7:5] | 10000 */
/*  unsigned char SR11;		 Graphics Cursor Y Position [7:0]         */
/*  unsigned char SR11E;	 Graphics Cursor Y Position [7:5] | 10001 */
/*  unsigned char SR12;		 Graphics Cursor Attributes Register */
/*  unsigned char SR13;		 Graphics Cursor Pattern Address */
/*  unsigned char SR16;		 Performance Tuning Register */
/*  unsigned char SR17;		 Configuration/Extended Control */
/*  unsigned char SR1E;		 VCLK Denominator */
/*  unsigned char CR19;		 Interlace End */
/*  unsigned char CR1A;		 Miscellaneous Control */
/*  unsigned char CR1B;		 Extended Display Control */
/*  unsigned char CR1D;		 Overlay Extended Control Register */
/*  unsigned char CR1E;          CRTC Timing Overflow (546X) */
/*  unsigned char HIDDENDAC;	 Hidden DAC register */
/*  DACcolourRec  FOREGROUND;    Hidden DAC cursor foreground colour */
/*  DACcolourRec  BACKGROUND;    Hidden DAC cursor background colour */
/*  unsigned short FORMAT;       Video format (546X) */
/*  unsigned long VSC;           Vendor specific control (546X) */
/*  unsigned short DTTC;         Display Threshold and Tiling Control (546X) */
/*  unsigned short TileCtrl;     Tiling control (5465) */
/*  unsigned char TILE;          Tile control (546X) */
/*  unsigned char BCLK;          Rambus clock (546X) */
/*  unsigned short CONTROL;      Control (546X) */
/*  unsigned short cursorX; */
/*  unsigned short cursorY; */
  
  /* SR0 */
  outw(0x3C4, 0x0100);				/* disable timing sequencer */
  
  outb(0x3CE,0x09);
  outb(0x3CF,restore->GR9);

  outb(0x3CE,0x0A);
  outb(0x3CF,restore->GRA);

  outb(0x3CE,0x0B);
  outb(0x3CF,restore->GRB);

  if (HAVEALPINE()) {
       outb(0x3ce, 0x0f);
       outb(0x3cf, restore->GRF);
  }

  if (restore->std.NoClock >= 0)
       {
       outb(0x3C4,0x0E);
       outb(0x3C5,restore->SRE);
       }

  if (Has_HWCursor(cirrusChip))
    {
	/* Restore the hardware cursor */
	outb (0x3C4, 0x13);
	outb (0x3C5, restore->SR13);
      
	outb (0x3C4, restore->SR10E);
	outb (0x3C5, restore->SR10);
      
	outb (0x3C4, restore->SR11E);
	outb (0x3C5, restore->SR11);

	outb (0x3C4, 0x12);
	outb (0x3C5, restore->SR12);
    }

  if ((cirrusChip >= CLGD5424 && cirrusChip <= CLGD5429) || HAVE543X() ||
      HAVE75XX() || cirrusChip == CLGD5446 || cirrusChip == CLGD5480)
       {
       /* Restore the Performance Tuning Register on these chips only. */
       outb(0x3C4,0x16);
       outb(0x3C5,restore->SR16);
       }

  if (HAVEALPINE()) {
       outb(0x3c4, 0x17);
       outb(0x3c5, restore->SR17);
  }

  if (restore->std.NoClock >= 0)
       {
       outb(0x3C4,0x1E);
       outb(0x3C5,restore->SR1E);
       }

  if ((cirrusChip >= CLGD5424 && cirrusChip <= CLGD5429) || HAVE543X() ||
      HAVE75XX() || cirrusChip == CLGD5446 || cirrusChip == CLGD5480) {
      outb(0x3c4, 0x1f);	/* MCLK register */
      outb(0x3c5, restore->SR1F);
  }

  outb(vgaIOBase + 4,0x19);
  outb(vgaIOBase + 5,restore->CR19);

  outb(vgaIOBase + 4,0x1A);
  outb(vgaIOBase + 5,restore->CR1A);

  outb(vgaIOBase + 4, 0x1B);
  outb(vgaIOBase + 5,restore->CR1B);

  if (cirrusChip == CLGD5434 || cirrusChip == CLGD5436 ||
      cirrusChip == CLGD5446 || cirrusChip == CLGD5480 || HAVE546X()) {
      outb(vgaIOBase + 4, 0x1D);
      outb(vgaIOBase + 5, restore->CR1D);
  }

  if (HAVE546X()) {
    outb(vgaIOBase + 4, 0x1E);
    outb(vgaIOBase + 5, restore->CR1E);
#if defined(DEBUG_CIRRUS) && defined(DEBUG_LG)
    ErrorF("restore:  CR1E = 0x%04X\n", restore->CR1E);
#endif
  }

  if (HAVE546X()) {
    if (!cirrusMMIOBase) {
      ErrorF("No memory-mapped IO base address found.  Cannot restore mode.\n");
      return;
    }
    else {
      unsigned char *pTILE, *pBCLK;
      unsigned short *pFORMAT, *pDTTC, *pCONTROL, *pTileCtrl;
      unsigned int *pVSC;

      pFORMAT = (unsigned short *)(cirrusMMIOBase + 0xC0);
      *pFORMAT = restore->FORMAT;

      pVSC = (unsigned int *)(cirrusMMIOBase + 0x3FC);
      *pVSC = (*pVSC & 0xEFFFFFFF) | (restore->VSC & 0x10000000);

      pDTTC = (unsigned short *)(cirrusMMIOBase + 0xEA);
      *pDTTC = (restore->DTTC & 0x3FFF) | (cirrusMemoryInterleave << 8);

      if (cirrusChip == CLGD5465) {
	pTileCtrl = (unsigned short *)(cirrusMMIOBase + 0x2C4);
	*pTileCtrl = (*pTileCtrl & 0x003F) | (restore->TileCtrl & 0xFFC0) |
	  (cirrusMemoryInterleave << 8);
      }

      pTILE = (unsigned char *)(cirrusMMIOBase + 0x407);
      *pTILE = (restore->TILE & 0x3F) | (cirrusMemoryInterleave);

      if (cirrusChip == CLGD5465)
 	pBCLK = (unsigned char *)(cirrusMMIOBase + 0x2C0);
      else
 	pBCLK = (unsigned char *)(cirrusMMIOBase + 0x8C);
      *pBCLK = (*pBCLK & 0xE0) | (restore->BCLK & 0x1F);
 
      pCONTROL = (unsigned short *)(cirrusMMIOBase + 0x402);
      *pCONTROL = restore->CONTROL;

#if defined(DEBUG_CIRRUS) && defined(DEBUG_LG)
      ErrorF("Restored:  FORMAT = 0x%04X  DTTC = 0x%04X  TILE = 0x%02X  "
	     "CONTROL = 0x%04X\n\tVSC = 0x%08X\n", restore->FORMAT, 
	     restore->DTTC, restore->TILE, restore->CONTROL, restore->VSC);
      ErrorF("\tFORMAT = 0x%04X  DTTC = 0x%04X  TILE = 0x%02X  "
	     "CONTROL = 0x%04X\n\tVSC = 0x%08X\n", *pFORMAT, *pDTTC, *pTILE, 
	     *pCONTROL, *pVSC);
#endif
    }
  }

  if (Is_75xx(cirrusChip)) {
      /* Does something need to be unlocked here?? */
      outb(vgaIOBase + 4, 0x2d);
      /*
       * XXX Bit 0 of CR2D used to be the only bit modified here
       * with a read-modify-write operation.
       */
      outb(vgaIOBase + 5, restore->CR2D);
      outb(vgaIOBase + 4, 0x2e);
      outb(vgaIOBase + 5, restore->CR2E);
  }

  if (cirrusChip == CLGD6225) 
       {
       /* Unlock the LCD registers... */
       outb(vgaIOBase + 4, 0x1D);
       i = inb(vgaIOBase + 5);
       outb(vgaIOBase + 5, (i | 0x80));

       /* Restore LCD HSYNC value */
       outb(vgaIOBase + 4, 0x0A);
       outb(vgaIOBase + 5, restore->RAX);
#if 0
       fprintf(stderr, "RAX restored to %d\n", restore->RAX);
#endif

       /* Lock the LCD registers... */
       outb(vgaIOBase + 4, 0x1D);
       i = inb(vgaIOBase + 5);
       outb(vgaIOBase + 5, (i & 0x7f));
       }

#ifndef MONOVGA
#ifdef DEBUG_CIRRUS
#if 0
  cirrusDumpRegs(restore);
#endif
#endif

  CirrusInvalidateShadowVariables();
  if (cirrusUseBLTEngine) {
      /* Initialize BitBLT engine for new-style (XAA) acceleration. */
      if (cirrusUseMMIO)
          CirrusInitializeBitBLTEngineMMIO();
      else
          CirrusInitializeBitBLTEngine();
  }

#endif

  vgaProtect(FALSE);
}

/*
 * cirrusSave -- 
 *      save the current video mode
 */
static void *
cirrusSave(save)
     vgacirrusPtr save;
{
  unsigned char             temp1, temp2;

  
#if defined(PC98_WAB) || defined(PC98_WABEP) || defined(PC98_GANB_WAP) || \
    defined(PC98_WSNA)
  vgaIOBase = 0x3D0;
#else
  vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
#endif

  outb(0x3CE, 0x09);
  temp1 = inb(0x3CF);
  outb(0x3CF, 0x00);	/* select bank 0 */
  outb(0x3CE, 0x0A);
  temp2 = inb(0x3CF);
  outb(0x3CF, 0x00);	/* select bank 0 */

  save = (vgacirrusPtr)vgaHWSave((vgaHWPtr)save, sizeof(vgacirrusRec));


/*  unsigned char GR9;		 Graphics Offset1 */
/*  unsigned char GRA;		 Graphics Offset2 */
/*  unsigned char GRB;		 Graphics Extensions Control */
/*  unsigned char SR7;		 Extended Sequencer */
/*  unsigned char SRE;		 VCLK Numerator (546X's denominator) */
/*  unsigned char SRF;		 DRAM Control */
/*  unsigned char SR10;		 Graphics Cursor X Position [7:0]         */
/*  unsigned char SR10E;	 Graphics Cursor X Position [7:5] | 10000 */
/*  unsigned char SR11;		 Graphics Cursor Y Position [7:0]         */
/*  unsigned char SR11E;	 Graphics Cursor Y Position [7:5] | 10001 */
/*  unsigned char SR12;		 Graphics Cursor Attributes Register */
/*  unsigned char SR13;		 Graphics Cursor Pattern Address */
/*  unsigned char SR1E;		 VCLK Denominator (546X's numerator) */
/*  unsigned char CR19;		 Interlace End */
/*  unsigned char CR1A;		 Miscellaneous Control */
/*  unsigned char CR1B;		 Extended Display Control */
/*  unsigned char CR1D;		 Overlay Extended Control Register */
/*  unsigned char CR1E;          CRTC Timing Overflow (546X) */
/*  unsigned char HIDDENDAC;	 Hidden DAC register */
/*  DACcolourRec  FOREGROUND;    Hidden DAC cursor foreground colour */
/*  DACcolourRec  BACKGROUND;    Hidden DAC cursor background colour */
/*  unsigned short FORMAT;       Video format (546X) */
/*  unsigned long VSC;           Vendor specific control (546X) */
/*  unsigned short TileCtrl;     Tiling control (5465) */
/*  unsigned short DTTC;         Display Threshold and Tiling Control (546X) */
/*  unsigned char TILE;          Tile control (546X) */
/*  unsigned char BCLK;          Rambus memory clock (546X) */
/*  unsigned short CONTROL;      Control (546X) */

  save->GR9 = temp1;

  save->GRA = temp2;

  outb(0x3CE,0x0B);		
  save->GRB = inb(0x3CF); 

  if (HAVEALPINE()) {
      outb(0x3ce, 0x0f);
      save->GRF = inb(0x3cf);
  }

  outb(0x3C4,0x07);
  save->SR7 = inb(0x3C5);

  outb(0x3C4,0x0E);
  save->SRE = inb(0x3C5);

  outb(0x3C4,0x0F);
  save->SRF = inb(0x3C5);

  if (Has_HWCursor(cirrusChip))
    {
      /* Hardware cursor */
      outb (0x3C4, 0x10);
      save->SR10E = inb (0x3C4);
      save->SR10  = inb (0x3C5);

      outb (0x3C4, 0x11);
      save->SR11E = inb (0x3C4);
      save->SR11  = inb (0x3C5);
  
      outb (0x3C4, 0x12);
      save->SR12 = inb (0x3C5);
  
      outb (0x3C4, 0x13);
      save->SR13 = inb (0x3C5);
    }  

  if ((cirrusChip >= CLGD5424 && cirrusChip <= CLGD5429) || HAVE543X() ||
      HAVE75XX() || cirrusChip == CLGD5446 || cirrusChip == CLGD5480) 
       {
       /* Save the Performance Tuning Register on these chips only. */
        outb(0x3C4,0x16);
        save->SR16 = inb(0x3C5);
       }

  if (HAVEALPINE())
       {
       outb(0x3c4,0x17);
       save->SR17 = inb(0x3c5);
       }

  outb(0x3C4,0x1E);
  save->SR1E = inb(0x3C5);

  if ((cirrusChip >= CLGD5424 && cirrusChip <= CLGD5429) || HAVE543X() ||
      HAVE75XX() || cirrusChip == CLGD5446 || cirrusChip == CLGD5480) {
      outb(0x3c4, 0x1f);		/* Save the MCLK register. */
      save->SR1F = inb(0x3c5);
  }

  outb(vgaIOBase + 4,0x19);
  save->CR19 = inb(vgaIOBase + 5);

  outb(vgaIOBase + 4,0x1A);
  save->CR1A = inb(vgaIOBase + 5);

  outb(vgaIOBase + 4, 0x1B);
  save->CR1B = inb(vgaIOBase + 5);

  if (cirrusChip == CLGD5434 || cirrusChip == CLGD5436 ||
      cirrusChip == CLGD5446 || cirrusChip == CLGD5480 || HAVE546X()) {
      outb(vgaIOBase + 4, 0x1D);
      save->CR1D = inb(vgaIOBase + 5);
  }

  if (HAVE546X()) {
    outb(vgaIOBase + 4, 0x1E);
    save->CR1E = inb(vgaIOBase + 5);
#ifdef DEBUG_CIRRUS
    ErrorF("save:  CR1E = 0x%04X\n", save->CR1E);
#endif
  }

  if (Is_75xx(cirrusChip)) {
      outb(vgaIOBase + 4, 0x2D);
      save->CR2D = inb(vgaIOBase + 5);
      outb(vgaIOBase + 4, 0x2E);
      save->CR2E = inb(vgaIOBase + 5);
  }

#ifndef MONOVGA
#ifdef ALLOW_8BPP_MULTIPLEXING
  if ((cirrusChip == CLGD5434 || cirrusChip == CLGD5436 ||
       cirrusChip == CLGD5446 || cirrusChip == CLGD5480 ||
       vgaBitsPerPixel != 8) && !HAVE546X()) {
      outb(0x3c6, 0x00);
      outb(0x3c6, 0xff);
      inb(0x3c6); inb(0x3c6); inb(0x3c6); inb(0x3c6);
      save->HIDDENDAC = inb(0x3c6);
  }
#else
  if (vgaBitsPerPixel != 8 && !HAVE546X()) {
      outb(0x3c6, 0x00);	/* Reset access count. */
      outb(0x3c6, 0xff);	/* Write 0xff to pixel mask. */
      inb(0x3c6); inb(0x3c6); inb(0x3c6); inb(0x3c6);
      save->HIDDENDAC = inb(0x3c6);
  }
#endif
#endif

  if (HAVE546X()) {
    if (!cirrusMMIOBase) {
      ErrorF("No memory-mapped IO base address found.  Cannot save mode.\n");
      return;
    }
    else {
#if 1
      unsigned char *pB;
      unsigned short *pW;
      unsigned long *pL;

      pW = (unsigned short *)(cirrusMMIOBase + 0xC0);
      save->FORMAT = *pW;

      pL = (unsigned long *)(cirrusMMIOBase + 0x3FC);
      save->VSC = *pL;

      /* Don't bother saving the bank interleaving value.  Interleaving
	 depends upon video memory only, not whether the display is in
	 a text or graphics mode */
      pW = (unsigned short *)(cirrusMMIOBase + 0xEA);
      save->DTTC = *pW;

      if (cirrusChip == CLGD5465) {
	pW = (unsigned short *)(cirrusMMIOBase + 0x2C4);
	save->TileCtrl = *pW;
      }

      pB = (unsigned char *)(cirrusMMIOBase + 0x407);
      save->TILE = *pB;
      
      if (cirrusChip == CLGD5465)
 	pB = (unsigned char *)(cirrusMMIOBase + 0x2C0);
      else
 	pB = (unsigned char *)(cirrusMMIOBase + 0x8C);
      save->BCLK = *pB;
 
      pW = (unsigned short *)(cirrusMMIOBase + 0x402);
      save->CONTROL = *pW;

#ifdef DEBUG_CIRRUS
      ErrorF("Saved:  FORMAT = 0x%04X  DTTC = 0x%04X  TILE = 0x%02X  "
	     "CONTROL = 0x%04X\nVSC = 0x%08X\n", save->FORMAT, save->DTTC, 
	     save->TILE, save->CONTROL, save->VSC);
#endif
#endif
    }
  }


  if (cirrusChip == CLGD6225) 
       {
       /* Unlock the LCD registers... */
       outb(vgaIOBase + 4, 0x1D);
       temp1 = inb(vgaIOBase + 5);
       outb(vgaIOBase + 5, (temp1 | 0x80));

       /* Save current LCD HSYNC value */
       outb(vgaIOBase + 4, 0x0A);
       save->RAX = inb(vgaIOBase + 5);
#if 0
       fprintf(stderr, "RAX saved as %d\n", save->RAX);
#endif

       /* Lock the LCD registers... */
       outb(vgaIOBase + 4, 0x1D);
       temp1 = inb(vgaIOBase + 5);
       outb(vgaIOBase + 5, (temp1 & 0x7f));
       }

  return ((void *) save);
}


/*
 * cirrusInit -- 
 *      Handle the initialization, etc. of a screen.
 */
static Bool
cirrusInit(mode)
     DisplayModePtr mode;
{
  int i;

#ifndef MONOVGA
  int multiplexing;

#ifdef ALLOW_8BPP_MULTIPLEXING

#if defined(DEBUG_CIRRUS) && defined(DEBUG_LG)
  fprintf(stderr, "cirrusInit(\"%s\")\n", mode->name);
  fprintf(stderr, "\tSynthClock = %d\n", mode->SynthClock);
  fprintf(stderr, "\tCrtcHDisplay = %d\n", mode->CrtcHDisplay);
  fprintf(stderr, "\tCrtcHTotal = %d\n", mode->CrtcHTotal);
  fprintf(stderr, "\tCtrcVDisplay = %d\n", mode->CrtcVDisplay);
  fprintf(stderr, "\tCrtcVTotal = %d\n", mode->CrtcVTotal);
  fprintf(stderr, "\tCrtcHAdjusted = %d\n", mode->CrtcHAdjusted);
  fprintf(stderr, "\tCrtcVAdjusted = %d\n", mode->CrtcVAdjusted);
#endif


  multiplexing = 0;
  if (vgaBitsPerPixel == 8 
      && (cirrusChip == CLGD5434 || cirrusChip == CLGD5436 ||
	  cirrusChip == CLGD5446 || cirrusChip == CLGD5480) && 
      vga256InfoRec.clock[mode->Clock] > CLOCK_PALETTE_DOUBLING_5434) {
    /* On the 5434, enable palette doubling mode for clocks > 85.5 MHz. */
    multiplexing = 1;
    /* The actual DAC register value is set later. */
    /* The CRTC is clocked at VCLK / 2, so we must half the */
    /* horizontal timings. */
    if (!mode->CrtcHAdjusted) {
      mode->CrtcHDisplay >>= 1;
      mode->CrtcHSyncStart >>= 1;
      mode->CrtcHTotal >>= 1;
      mode->CrtcHSyncEnd >>= 1;
      mode->CrtcHAdjusted = TRUE;
    }
  }
#endif
#endif

  if (mode->VTotal >= 1024 && !(mode->Flags & V_INTERLACE)
      && !mode->CrtcVAdjusted) {
    /* For non-interlaced vertical timing >= 1024, the vertical timings */
    /* are divided by 2 and VGA CRTC 0x17 bit 2 is set. */
    mode->CrtcVDisplay >>= 1;
    mode->CrtcVSyncStart >>= 1;
    mode->CrtcVSyncEnd >>= 1;
    mode->CrtcVTotal >>= 1;
    mode->CrtcVAdjusted = TRUE;
  }

  if (!vgaHWInit(mode,sizeof(vgacirrusRec)))
    return(FALSE);

  /*  unsigned char GR9;		 Graphics Offset1 */
  /*  unsigned char GRA;		 Graphics Offset2 */
  /*  unsigned char GRB;		 Graphics Extensions Control */
  /*  unsigned char SR7;		 Extended Sequencer */
  /*  unsigned char SRE;		 VCLK Numerator (546X's denominator) */
  /*  unsigned char SRF;		 DRAM Control */
  /*  unsigned char SR10;		 Graphics Cursor X Position [7:0] */
  /*  unsigned char SR10E;	         Graphics Cursor X Pos [7:5] | 10000 */
  /*  unsigned char SR11;		 Graphics Cursor Y Position [7:0] */
  /*  unsigned char SR11E;	         Graphics Cursor Y Pos [7:5] | 10001 */
  /*  unsigned char SR12;		 Graphics Cursor Attributes Register */
  /*  unsigned char SR13;		 Graphics Cursor Pattern Address */
  /*  unsigned char SR16;		 Performance Tuning Register */
  /*  unsigned char SR17;		 Configuration/Extended Control */
  /*  unsigned char SR1E;		 VCLK Denominator (546X's numerator) */
  /*  unsigned char CR19;		 Interlace End */
  /*  unsigned char CR1A;		 Miscellaneous Control */
  /*  unsigned char CR1B;		 Extended Display Control */
  /*  unsigned char CR1D;		 Overlay Extended Control Register */
  /*  unsigned char CR1E;                CRTC Timing Overflow (546X) */
  /*  unsigned char HIDDENDAC;	         Hidden DAC register */
  /*  DACcolourRec  FOREGROUND;          Hidden DAC cursor foreground colour */
  /*  DACcolourRec  BACKGROUND;          Hidden DAC cursor background colour */
  /*  unsigned short FORMAT;             Video format (546X) */
  /*  unsigned long VSC;                 Vendor specific control (546X) */
  /*  unsigned short DTTC;               Display Threshold and Tiling Ctrl */
  /*  unsigned short TileCtrl;           Tiling control (5465) */
  /*  unsigned char TILE;                Tile control (546X) */
  /*  unsigned char BCLK;                Rambus memory clock (546X) */
  /*  unsigned short CONTROL;            Control (546X) */
  
				/* Set the clock regs */

  if (new->std.NoClock >= 0)
    {
      unsigned char tempreg;
      int SRE, SR1E;
      int usemclk;
      
      if (new->std.NoClock >= NUM_CIRRUS_CLOCKS)
	{
	  ErrorF("Invalid clock index -- too many clocks in XF86Config\n");
	  return(FALSE);
	}
				/* Always use VLCK3 */
      
      new->std.MiscOutReg |= 0x0C;

      if (!cirrusCheckClock(cirrusChip, new->std.NoClock))
	return (FALSE);

      if (HAVE546X()) {
	new->BCLK = cirrusLgBCLK;
      } else {
	if (cirrusReprogrammedMCLK > 0)
	  new->SR1F = cirrusReprogrammedMCLK;
	else {
	  outb(0x3c4, 0x1f);		/* MCLK register. */
	  new->SR1F = inb(0x3c5);
	}
      }

      if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) {
#ifndef MONOVGA
	if (vgaBitsPerPixel == 8 && multiplexing && cirrusChip >= CLGD5436)
	  CirrusFindClock(vga256InfoRec.clock[new->std.NoClock] / 2,
			  vga256InfoRec.maxClock, &SRE, &SR1E, &usemclk);
        else
	if (vgaBitsPerPixel == 16 && MUST_DOUBLE_VCLK_FOR_16BPP)
	  CirrusFindClock(vga256InfoRec.clock[new->std.NoClock] * 2,
			  vga256InfoRec.maxClock, &SRE, &SR1E, &usemclk);
        else
	if (vgaBitsPerPixel == 24 && (cirrusChip <= CLGD5430 || HAVE75XX()))
	  /* 542x, 5434, 5430, 754x */
	  CirrusFindClock(vga256InfoRec.clock[new->std.NoClock] * 3,
			  vga256InfoRec.maxClock, &SRE, &SR1E, &usemclk);
	else
#endif
	if (!HAVE546X())
	  CirrusFindClock(vga256InfoRec.clock[new->std.NoClock],
			  vga256InfoRec.maxClock, &SRE, &SR1E, &usemclk);
	else {
	  /* For the 546X chips, the numerator and denominator registers
	     are swapped */
	  CirrusFindClock(vga256InfoRec.clock[new->std.NoClock],
			  vga256InfoRec.maxClock, &SR1E, &SRE, &usemclk);
	}
	if (usemclk && (cirrusChip == CLGD5428 || cirrusChip == CLGD5429
			|| HAVE543X() || HAVE75XX()
			|| cirrusChip == CLGD5446 
			|| cirrusChip == CLGD5480)) {
	  new->SR1F |= 0x40;	/* Use MCLK as VLCK. */
	  SR1E &= 0xfe;	        /* Clear bit 0 of SR1E. */
	}
      }
      else {
	if (HAVE546X()) {
	  SR1E = cirrusClockTab[new->std.NoClock].numer;
	  SRE = cirrusClockTab[new->std.NoClock].denom;
	} else {
	  SRE = cirrusClockTab[new->std.NoClock].numer;
	  SR1E = cirrusClockTab[new->std.NoClock].denom;
	}
#ifndef MONOVGA
	if (vgaBitsPerPixel == 8 && multiplexing && cirrusChip >= CLGD5436) {
	  SRE = cirrusDivide2ClockTab[new->std.NoClock].numer;
	  SR1E = cirrusDivide2ClockTab[new->std.NoClock].denom;
	}
	if (vgaBitsPerPixel == 16 && MUST_DOUBLE_VCLK_FOR_16BPP) {
	  /* Use the clocking mode whereby the programmed VCLK */
	  /* is double the pixel rate. */
	  SRE = cirrusDoubleClockTab[new->std.NoClock].numer;
	  SR1E = cirrusDoubleClockTab[new->std.NoClock].denom;
	}
	if (vgaBitsPerPixel == 24 && (cirrusChip <= CLGD5430 ||
	HAVE75XX())) {
	  /* Use the clocking mode whereby the programmed VCLK */
	  /* is triple the pixel rate. */
	  SRE = cirrusTripleClockTab[new->std.NoClock].numer;
	  SR1E = cirrusTripleClockTab[new->std.NoClock].denom;
	}
#endif
      }
				/* Be nice to the reserved bits... */
      if (HAVE546X()) {
	/* there are no reserved bits for the 546X's denominator */
	outb(0x3C4,0x0E);
	new->SRE = SRE;
      } else {
	outb(0x3C4,0x0E);
	tempreg = inb(0x3C5);
	new->SRE = (tempreg & 0x80) | (SRE & 0x7F);
      }
      
      if (HAVE546X()) {
	/* The only bit that is 'reserved' is the high bit.  It's not
	   really reserved, but we don't want to set it */
	outb(0x3C4,0x1E);
	tempreg = inb(0x3C5);
	new->SR1E = (tempreg & 0x80) | (SR1E & 0x7F);
      } else {
	outb(0x3C4,0x1E);
	tempreg = inb(0x3C5);
	new->SR1E = (tempreg & 0xC0) | (SR1E & 0x3F);
      }
    }


     
#ifndef MONOVGA
  if (vgaBitsPerPixel == 16) {
    new->std.CRTC[0x13] = (vga256InfoRec.displayWidth >> 2) & 0xFF;
  } else if (vgaBitsPerPixel == 32) {
    new->std.CRTC[0x13] = (vga256InfoRec.displayWidth >> 1) & 0xFF;
  } else if (vgaBitsPerPixel == 24 && !HAVE546X()) {
    new->std.CRTC[0x13] = (vga256InfoRec.displayWidth * 3) >> 3;
  } else if (vgaBitsPerPixel == 24 && HAVE546X()) {
    /* Round _up_ to pad the scanline out with extra bytes */
    new->std.CRTC[0x13] = (vga256InfoRec.displayWidth * 3 + 3) >> 3;
  } else if (vgaBitsPerPixel == 8) {
    new->std.CRTC[0x13] = vga256InfoRec.displayWidth >> 3;
  }
#endif

				/* Enable Dual Banking */
  new->GRB = 0x01;
#ifdef CIRRUS_SUPPORT_LINEAR
  if (cirrusUseLinear)
    /* Linear addressing requires single-banking to be set. */
    new->GRB = 0x00;
#endif         

  /* Initialize the read and write bank such a way that we initially */
  /* have an effective 64K window at the start of video memory. */
  new->GR9 = 0x00;
  new->GRA = (CIRRUS.ChipSetRead != cirrusSetRead) ? 0x02 : 0x08;

  /* Write sane value to Display Compression Control register, which */
  /* might be corrupted by other chipset probes. */
  if (HAVEALPINE()) {
    outb(0x3ce, 0x0f);
    new->GRF = inb(0x3cf) & 0xc0;
  }

  outb(0x3C4,0x0F);
  new->SRF = inb(0x3C5);

#ifdef PC98_WAB
  new->SRF &= 0xF7;
  new->SRF |= 0x10;
  outb(0x3C4,0x0F);
  outb(0x3C5,new->SRF);
#endif /* PC98_WAB || PC98_WABEP */

  /* This following bit was not set correctly. */
  /* It is vital for correct operation at high dot clocks. */
 
  if ((cirrusChip >= CLGD5422 && cirrusChip <= CLGD5429)
      || HAVE543X() || HAVE75XX() || cirrusChip == CLGD5446
      || cirrusChip == CLGD5480)
    {
      new->SRF |= 0x20;	/* Enable 64 byte FIFO. */
    }

#ifndef MONOVGA
  if ((cirrusChip >= CLGD5424 && cirrusChip <= CLGD5429) || HAVE543X() ||
      HAVE75XX() || cirrusChip == CLGD5446 || cirrusChip == CLGD5480)
    {
      int fifoshift_5430;
      int pixelrate, bandwidthleft, bandwidthused, percent;
      /* Now set the CRT FIFO threshold (in 4 byte words). */
      outb(0x3C4,0x16);
      new->SR16 = inb(0x3C5) & 0xF0;

      /* The 5430 has extra 4 levels of buffering; adjust the FIFO */
      /* threshold values for that chip. */
      fifoshift_5430 = 0;
      if (cirrusChip == CLGD5430 || cirrusChip == CLGD7548)
	fifoshift_5430 = 4;

      /*
       * Calculate the relative amount of video memory bandwidth
       * taken up by screen refresh to help with FIFO threshold
       * setting.
       */
      pixelrate = vga256InfoRec.clock[new->std.NoClock];
      bandwidthused = pixelrate * (vgaBitsPerPixel / 8);
      bandwidthleft = cirrusDRAMBandwidth - bandwidthused;
      /* Relative amount of bandwidth left for drawing. */
      percent = bandwidthleft * 100 / cirrusDRAMBandwidth;
      
      /* We have an option for conservative, or aggressive setting. */
      /* The default is something in between. */
      
      if (cirrusChip == CLGD5434) {
	int threshold;
	int hsyncdelay;
	/*
	 * Small difference between HTotal and HSyncStart
	 * (first and second horizontal numbers) requires
	 * much higher FIFO threshold setting. It might be
	 * worthwhile to automatically modify the timing.
	 */
	hsyncdelay = mode->CrtcHSyncStart - mode->CrtcHDisplay;
	/* The 5434 has 16 extra levels of buffering. */
	if (OFLG_ISSET(OPTION_FIFO_AGGRESSIVE, &vga256InfoRec.options))
	  /* Aggressive setting, effectively 16. */
	  threshold = 0;
      else {
	/* Default FIFO setting for 5434. */
	threshold = 1;	/* Effectively 17. */
	/* 1MB, MCLK < 55 MHz */
	if (bandwidthused >= 67500)
	  threshold = 8;
	if (bandwidthused >= 77500)
	  threshold = 12;
	if (hsyncdelay <= 16)
	  threshold = 12;
	if (cirrusDRAMBandwidth >= 110000) {
	  /* 1MB, MCLK > 55 MHz */
	  threshold = 1;
	  if (bandwidthused >= 67500)
	    threshold = 4;
	  if (bandwidthused >= 77500)
	    threshold = 8;
	  if (hsyncdelay <= 16)
	    threshold = 10;
	}
	if (cirrusDRAMBandwidth >= 165000) {
	  /* 2MB, MCLK < 55 MHz */
	  threshold = 1;
	  if (bandwidthused >= 125000)
	    threshold = 8;
	  if (bandwidthused >= 155000)
	    threshold = 12;
	  if (hsyncdelay <= 16)
	    threshold = 12;
	}
	if (cirrusDRAMBandwidth >= 220000) {
	  /* 2MB, MCLK > 55 MHz. */
	  threshold = 1;
	  if (bandwidthused >= 125000)
	    threshold = 4;
	  if (bandwidthused >= 155000)
	    threshold = 8;
	  if (hsyncdelay <= 16)
	    threshold = 10;
	}
	if (OFLG_ISSET(OPTION_FIFO_CONSERV,
		       &vga256InfoRec.options)) {
	  threshold = 8;
	  /* 1MB */
	  if (bandwidthused >= 67500)
	    threshold = 12;
	  if (bandwidthused >= 77500)
	    threshold = 14;
	  if (cirrusDRAMBandwidth >= 165000) {
	    /* 2MB */
	    threshold = 8;
	    if (bandwidthused >= 125000)
	      threshold = 12;
	    if (bandwidthused >= 155000)
	      threshold = 14;
	  }
	}
      }

	new->SR16 |= threshold;
      } /* endif cirrusChip == CLGD5434 */
      else
      if (cirrusChip == CLGD5436 || cirrusChip == CLGD5446 ||
	  cirrusChip == CLGD5480 || cirrusChip == CLGD7555) {
          int threshold;
          if (OFLG_ISSET(OPTION_FIFO_CONSERV, &vga256InfoRec.options)) {
              threshold = 8;
              if (percent < 20)
                  threshold = 10;
          }
          else {
              if (cirrusDRAMBandwidth >= 170000) {
                  /* 2MB, based on 5446 testing (57 MHz MCLK). */
                  threshold = 2;
                  if (bandwidthused >= 32000)
                      threshold = 3;
                  if (bandwidthused >= 75000)
                      threshold = 4;
                  if (bandwidthused >= 100000)
                      threshold = 5;
                  if (bandwidthused >= 130000)
                      threshold = 6;
                  if (bandwidthused >= 175000)
                      threshold = 8;
              }
              else {
                  /* 1MB; these are guesses. */
                  threshold = 4;
                  if (bandwidthused >= 50000)
                      threshold = 5;
                  if (bandwidthused >= 65000)
                      threshold = 6;
                  if (bandwidthused >= 85000)
                      threshold = 8;
              }
          }
          new->SR16 |= threshold;
      }
      else {
	/* XXXX Is 0 required for interlaced modes on some chips? */
	int threshold;
	threshold = 8;
	if (OFLG_ISSET(OPTION_FIFO_CONSERV, &vga256InfoRec.options)) {
	  /* Conservative FIFO threshold setting. */
	  if (bandwidthused >= 59000)
	    threshold = 12;
	  if (bandwidthused >= 64000)
	    threshold = 14;
	  if (bandwidthused >= 79000)
	    threshold = 15;
	}
	else
	  if (!OFLG_ISSET(OPTION_FIFO_AGGRESSIVE, &vga256InfoRec.options)) {
	    /* Default FIFO threshold setting. */
	    if (bandwidthused >= 64000)
	      threshold = 10;
	    if (bandwidthused >= 71000)
	      threshold = 12;
	    if (cirrusChip < CLGD5428) {
	      if (bandwidthused >= 74000)
		threshold = 14;
	      if (bandwidthused >= 83000)
		threshold = 15;
	    }
	    else {
	      /* Based on the observation that the 5428 */
	      /* BIOS 77 MHz 1024x768 mode uses 12. */
	      if (bandwidthused >= 79000)
		threshold = 14;
	      if (bandwidthused >= 88000)
		threshold = 15;
	    }
	  }
	  else {
	    /* Aggressive setting. */
	    if (bandwidthused >= 69000)
	      threshold = 10;
	    if (bandwidthused >= 79000)
	      threshold = 12;
	    if (bandwidthused >= 88000)
	      threshold = 14;
	  }
	/* Agressive FIFO threshold setting is always 8. */
	new->SR16 |= threshold - fifoshift_5430;
      } /* endelse */
    } /* endif */
#endif

  if (cirrusChip == CLGD5430
      && !OFLG_ISSET(OPTION_NO_2MB_BANKSEL, &vga256InfoRec.options))
    /* The 5430 always uses DRAM 'bank switching' bit. */
    new->SRF |= 0x80;

  if (CIRRUS.ChipSetRead != cirrusSetRead)
    {
      new->GRB |= 0x20;	/* Set 16k bank granularity */
      if (cirrusChip != CLGD5434 && cirrusChip != CLGD5436 &&
	  cirrusChip != CLGD5446 && cirrusChip != CLGD5480 &&
	  !HAVE75XX()) {
#ifdef MONOVGA
	if (vga256InfoRec.displayWidth * vga256InfoRec.virtualY / 2 > (1024*1024)
		&& !OFLG_ISSET(OPTION_NO_2MB_BANKSEL, &vga256InfoRec.options))
	    new->SRF |= 0x80;	/* Enable the second MB. */
	                        /* This may be a bad thing for some */
	  			/* 2Mb cards. */

	    /* We want to be able to use all off-screen memory in */
	    /* principle. */
#else
	if (vga256InfoRec.videoRam == 2048 
		&& !OFLG_ISSET(OPTION_NO_2MB_BANKSEL, &vga256InfoRec.options))
	    new->SRF |= 0x80;	/* Enable the second MB. */
	                        /* This may be a bad thing for some */
	  			/* 2Mb cards. */
#endif
	}
    }

#ifdef MONOVGA
    new->SR7 = 0x00;		/* Use 16 color mode. */
#else
      /*
       * There are two 16bpp clocking modes.
       * 'Clock / 2 for 16-bit/Pixel Data' clocking mode (0x03).
       * This works on all 542x chips, but requires VCLK to be twice
       * the pixel rate.
       * The alternative method, '16-bit/Pixel Data at Pixel Rate' (0x07),
       * is supported from the 5426 (this is the way to go for high clocks
       * on the 5434), with VCLK at pixel rate.
       * Both modes use normal 8bpp CRTC timing.
       *
       * On the 5434, there's also a 32bpp mode (0x09), that appears to want
       * the VCLK at pixel rate and the same CRTC timings as 8bpp (i.e.
       * nicely compatible).
       *
       * 24bpp uses a triple-clocking mode on all chips except the 5436
       * and 5446 and 5480, which do 24bpp with VCLK at pixel rate.
       */
      if (vgaBitsPerPixel == 8) {
	if (HAVE546X())
	  new->SR7 = 0x00;
	else
#ifdef ALLOW_8BPP_MULTIPLEXING
	if (multiplexing)
	  new->SR7 = 0x07;	/* 5434 palette clock doubling mode */
	else
#endif
	  new->SR7 = 0x01;	/* Tell it to use 256 Colors */
      }
      if (vgaBitsPerPixel == 16) {
	if (MUST_DOUBLE_VCLK_FOR_16BPP)
	  /* Use the double VCLK mode. */
	  new->SR7 = 0x03;
	else if (HAVE546X())
	  new->SR7 = 0x00;
	else
	  new->SR7 = 0x07;
      }
      if (vgaBitsPerPixel == 24) {
	if (HAVE546X())
	  new->SR7 = 0x00;
	else
	  new->SR7 = 0x05;
      }
      if (vgaBitsPerPixel == 32) {
	if (HAVE546X())
	  new->SR7 = 0x00;
	else
	  new->SR7 = 0x09;
      }
#endif

#ifdef CIRRUS_SUPPORT_LINEAR
      if (cirrusUseLinear && !HAVE546X())
	new->SR7 |= 0x0E << 4;	/* Map at 14Mb. */
#endif

      if (mode->VTotal >= 1024 && !(mode->Flags & V_INTERLACE))
	/* For non-interlaced vertical timing >= 1024, the programmed */
	/* vertical timings are multiplied by 2 by setting this bit. */
	new->std.CRTC[0x17] |= 0x04;

				/* Fill up all the overflows - ugh! */
#if defined(DEBUG_CIRRUS) && defined(DEBUG_CIRRUS_LG)
      fprintf(stderr,"Init: CrtcVSyncStart + 1 = %x\n"
	      "CrtcHsyncEnd>>3 = %x\n"
	      "CrtcHDisplay>>3 -1 = %x\n"
	      "VirtX = %x\n",
	      mode->CrtcVSyncStart + 1,
	      mode->CrtcHSyncEnd >> 3, 
	      (mode->CrtcHDisplay >> 3) - 1,
	      vga256InfoRec.virtualX>>4);
#endif
     
      new->CR1A = (((mode->CrtcVSyncStart + 1) & 0x300 ) >> 2)
	| (((mode->CrtcHSyncEnd >> 3) & 0xC0) >> 2);

      if (mode->Flags & V_INTERLACE) 
	{
				/* ``Half the Horizontal Total'' which is */
				/* really half the value in CR0 */

	  new->CR19 = ((mode->CrtcHTotal >> 3) - 5) >> 1;
	  new->CR1A |= 0x01;
	}
      else new->CR19 = 0x00;

     /* Extended logical scanline length bit. */
#ifdef MONOVGA
      new->CR1B = (((vga256InfoRec.displayWidth>>4) & 0x100) >> 4)
	| 0x22;
#else
#if 1
       if (HAVE546X()) {
 	 if (vgaBitsPerPixel == 8)
 	   new->CR1B = (((vga256InfoRec.displayWidth>>3) & 0x100) >> 4) | 0x22;
 	 else if (vgaBitsPerPixel == 16)
 	   new->CR1B = (((vga256InfoRec.displayWidth>>2) & 0x100) >> 4) | 0x22;
 	 else if (vgaBitsPerPixel == 24)
 	   new->CR1B = 
 	     (((vga256InfoRec.displayWidth * 3 + 3) & 0x800) >> 7) | 0x22;
 	 else if (vgaBitsPerPixel == 32)
 	   new->CR1B = (((vga256InfoRec.displayWidth>>1) & 0x100) >> 4) | 0x22;
       } else {                           /* !HAVE546X() */
 	 if (vgaBitsPerPixel == 8)
 	   new->CR1B = (((vga256InfoRec.displayWidth>>3) & 0x100) >> 4) | 0x22;
 	 else if (vgaBitsPerPixel == 16)
 	   new->CR1B = (((vga256InfoRec.displayWidth>>2) & 0x100) >> 4) | 0x22;
 	 else if (vgaBitsPerPixel == 24)
 	   new->CR1B = 
 	     (((vga256InfoRec.displayWidth * 3) & 0x800) >> 7) | 0x22;
 	 else if (vgaBitsPerPixel == 32)       /* !!! Is this right ??? */
 	   new->CR1B = (((vga256InfoRec.displayWidth>>1) & 0x100) >> 4) | 0x22;
       }
#elif 1
      if ((vgaBitsPerPixel > 8 && vgaBitsPerPixel != 24 && !HAVE546X()) ||
	  (vgaBitsPerPixel == 16 && HAVE546X()))
	new->CR1B = (((vga256InfoRec.displayWidth>>2) & 0x100) >> 4) | 0x22;
      else if (vgaBitsPerPixel == 32 && HAVE546X())
	new->CR1B = (((vga256InfoRec.displayWidth>>1) & 0x100) >> 4) | 0x22;
      else if (vgaBitsPerPixel == 24 && !HAVE546X())
	new->CR1B = (((vga256InfoRec.displayWidth * 3) & 0x800) >> 7) | 0x22;
      else if (vgaBitsPerPixel == 24 && HAVE546X())
	new->CR1B = (((vga256InfoRec.displayWidth * 3 + 3) & 0x800) >> 7) | 0x22;
      else
	new->CR1B = (((vga256InfoRec.displayWidth>>3) & 0x100) >> 4) | 0x22;
#else
      if (vgaBitsPerPixel == 8)
	new->CR1B = (((vga256InfoRec.displayWidth>>3) & 0x100) >> 4) | 0x22;
      else if (vgaBitsPerPixel == 16)
	new->CR1B = (((vga256InfoRec.displayWidth>>2) & 0x100) >> 4) | 0x22;
      else if (vgaBitsPerPixel == 24 && !HAVE546X())
	new->CR1B = (((vga256InfoRec.displayWidth * 3) & 0x800) >> 7) | 0x22;
      else if (vgaBitsPerPixel == 24 && HAVE546X())
	new->CR1B = (((vga256InfoRec.displayWidth * 3 + 3) & 0x800) >> 7) | 0x22;
      else /* 32 bpp */
      	/* XXX This can't be right for the 5434. */
	new->CR1B = (((vga256InfoRec.displayWidth>>1) & 0x100) >> 4) | 0x22;
#endif
#endif

      if (cirrusChip == CLGD5434 || cirrusChip == CLGD5436 ||
	  cirrusChip == CLGD5446 || cirrusChip == CLGD5480) {
	outb(vgaIOBase + 4, 0x1D);
	/* Set display start address bit 19 to 0. */
	new->CR1D = inb(vgaIOBase + 5) & 0x7f;
      } else if (HAVE546X()) {
	outb(vgaIOBase + 4, 0x1D);
	new->CR1D = inb(vgaIOBase + 5) & 0xF7;
      }

  /* The 546X chips have an extra offset bit (bit 9) in the CR1D register */
  if (HAVE546X()) {
    if (vgaBitsPerPixel == 8)
      new->CR1D = (new->CR1D & 0xF7) | 
	(((vga256InfoRec.displayWidth>>3) & 0x200) >> 9);
    else if (vgaBitsPerPixel == 16)
      new->CR1D = (new->CR1D & 0xF7) | 
	(((vga256InfoRec.displayWidth>>2) & 0x200) >> 9);
    else if (vgaBitsPerPixel == 24)
      new->CR1D = (new->CR1D & 0xF7) | 
	(((vga256InfoRec.displayWidth * 3 + 3) & 0x1000) >> 12);
    else if (vgaBitsPerPixel == 32)
      new->CR1D = (new->CR1D & 0xF7) | 
	(((vga256InfoRec.displayWidth>>1) & 0x200) >> 9);
  }


     if (Is_75xx(cirrusChip)) {
	  /*
	   * CR2D: Includes controls for 640x480 LCDs.
	   * Clearing bit 0 disables automatic centering for 640x480 LCDs.
	   * Clearing bit 1 disables automatic vertical expansion for
	   * 640x480 LCDs.
	   * Bit 4 controls the clock source for suspend mode.
	   * CR2E: Includes controls for 800x600 LCDs.
	   * Clearing bit 1 disables vertical expansion in graphics modes.
	   * Clearing bit 3 disables horizontal expansion in graphics
	   * modes (this kind of expansion does not work at 16/24bpp).
	   * Clearing bit 5 disables automatic horizontal centering (also
	   * for 1024x768 LCDs).
	   */
	  new->CR2D &= ~(0x1);	/* Only touch bit 0. */
	  new->CR2E &= ~(0x20 | 0x8 | 0x2 | 0x1);
	  if (cirrusLCDVerticalSize >= 600) {
	      /* 800x600 or 1024x768 LCD */
	      /* Clear CR2D bit 0: No automatic (640x480 LCD?) centering. */
	      if (!OFLG_ISSET(OPTION_LCD_STRETCH, &vga256InfoRec.options)) {
	          /*
	           * If the flag to disable stretching is not set,
	           * enable stetching (horizontal and vertical expansion).
	           */
	          if (mode->HDisplay <= 640 && vga256InfoRec.bitsPerPixel <= 8)
	              new->CR2E |= 0x8;	/* Horizontal expansion. */
	          if (mode->VDisplay <= 480)
	              new->CR2E |= 0x1;	/* Vertical expansion. */
	      }
	      new->CR2E |= 0x20; /* Enable automatic horizontal centering. */
	  }
	  else  {
	      /* 640x480 LCD */
	      if (mode->HDisplay <= 640)
	          new->CR2D |= 0x01;	/* Enable automatic centering. */
	  }
     }

     if (cirrusChip == CLGD5429 || HAVEALPINE()) {
          outb(0x3c4, 0x17);
          new->SR17 = inb(0x3c5);
#ifdef CIRRUS_SUPPORT_MMIO
          /* Optionally enable Memory-Mapped I/O. */
          if (cirrusUseMMIO) {
     	       /* Set SR17 bit 2. */
               new->SR17 |= 0x04;
               if (cirrusChip != CLGD5434)
               	   /* Clear bit 6 to select mmio address space at 0xB8000. */
                   new->SR17 &= ~0x40;
          }
#endif
     }

     new->HIDDENDAC = 0;
#ifndef MONOVGA
     /*
      * Set the Hidden DAC register to the proper value for 16bpp, 24bpp,
      * 32bpp and high dot clock 8bpp mode.
      */
     if (vgaBitsPerPixel == 16) {
         if (xf86weight.red == 5 && xf86weight.green == 5
         && xf86weight.blue == 5)
             /* 5-5-5 RGB mode */
             if (!(MUST_DOUBLE_VCLK_FOR_16BPP))
                 new->HIDDENDAC = 0xd0;	/* Double edge mode. */
             else
                 new->HIDDENDAC = 0xf0; /* Single edge mode (double VLCK). */
         if (xf86weight.red == 5 && xf86weight.green == 6
         && xf86weight.blue == 5)
             /* 5-6-5 RGB mode */
             if (!(MUST_DOUBLE_VCLK_FOR_16BPP))
                 new->HIDDENDAC = 0xc1;
             else
                 new->HIDDENDAC = 0xe1;
     }
     if (vgaBitsPerPixel == 24 || vgaBitsPerPixel == 32)
         /* Set 24-bit color 8-8-8 RGB mode. */
         new->HIDDENDAC = 0xc5;

#ifdef ALLOW_8BPP_MULTIPLEXING
     if (multiplexing) {
         new->HIDDENDAC = 0x4A;
     }
#endif
#endif

      if (HAVE546X()) {
	unsigned short *pDTTC;
	unsigned short *pTileCtrl;
	unsigned char *pTILE;

	/* Only 28th bit matters */
	new->VSC = 0x10000000;

	new->CR1E = 0x00;
	new->CR1E |= ((mode->CrtcHTotal>>3 & 0x0100)?1:0)<<7;
	new->CR1E |= ((mode->CrtcHDisplay>>3 & 0x0100)?1:0)<<6;
	new->CR1E |= ((mode->CrtcHSyncStart>>3 & 0x0100)?1:0)<<5;
	new->CR1E |= ((mode->CrtcHSyncStart>>3 & 0x0100)?1:0)<<4;
	new->CR1E |= ((mode->CrtcVTotal & 0x0400)?1:0)<<3;
	new->CR1E |= ((mode->CrtcVDisplay & 0x0400)?1:0)<<2;
	new->CR1E |= ((mode->CrtcVSyncStart & 0x0400)?1:0)<<1;
	new->CR1E |= ((mode->CrtcVSyncStart & 0x0400)?1:0)<<0;
	
 	if (cirrusFindPitchPadding(&i) > 0)
 	  cirrusTilesPerLineIndex = i;
 	else
 	  cirrusTilesPerLineIndex = defaultTilesPerLineIndex;

	switch (vgaBitsPerPixel) {
	default:
	  ErrorF("****** DANGEROUS WARNING! ******\n");
	  ErrorF("* Unknown pixel bit depth %2d  *\n", vgaBitsPerPixel);
	case 8:
	  new->FORMAT = 0x0000;

 	  new->TILE = cirrusTilesPerLineTab[cirrusTilesPerLineIndex].
 	    tilesPerLine;
	  new->DTTC = (new->TILE << 8) | 0x0080 | 
	    (cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width << 6);
	  new->CONTROL = 0x0000 | 
	    (cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width << 11);


	  /* There is an optimal FIFO threshold value (lower 5 bits of DTTC)
	     for every resolution and color depth combination.  We'll hit
	     the highlights here, and get close for anything that's not 
	     covered. */
	  switch (mode->CrtcHDisplay) {
	  case 640:
	    /* BAD numbers:  0x1E */
	    /* GOOD numbers:  0x14 */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0014);
	    break;
	  default:
	  case 800:
	    /* BAD numbers:  0x16 */
	    /* GOOD numbers:  0x13 */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0013);
	    break;
	  case 1024:
	    /* BAD numbers:  */
	    /* GOOD numbers: 0x15 */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0015);
	    break;
	  case 1280:
	    /* BAD numbers:  */
	    /* GOOD numbers:  0x16 */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0016);
	    break;
	  }
	  break;

	case 16:
	  if (HAVE546X() && cirrusChip != CLGD5462 &&
	      xf86weight.red == 5 && xf86weight.green == 5
	      && xf86weight.blue == 5) {
	    /* 5-5-5 RGB mode */
	    new->FORMAT = 0x1600;
	  } else {
	    /* 5-6-5 RGB mode */	    
	    new->FORMAT = 0x1400;
	  }

	  new->TILE = cirrusTilesPerLineTab[cirrusTilesPerLineIndex].
	    tilesPerLine;
	  new->DTTC = (new->TILE << 8) | 0x0080 | 
	    (cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width << 6);
	  new->CONTROL = 0x2000 | 
	    (cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width << 11);


	  switch (mode->CrtcHDisplay) {
	  case 640:
	    /* BAD numbers:  0x12 */
	    /* GOOD numbers: 0x10 */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0010);
	    break;
	  default:
	  case 800:
	    /* BAD numbers:  0x13 */
	    /* GOOD numbers:  0x11 */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0011);
	    break;
	  case 1024:
	    /* BAD numbers:  0x14 */
	    /* GOOD numbers: 0x12  */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0012);
	    break;
	  case 1280:
	    /* BAD numbers:   0x08 0x10 */
	    /* GOOD numbers:  0x12 */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0012);
	    break;
	  }
	  break;


	case 24:
	  new->FORMAT = 0x2400;

 	  new->TILE = cirrusTilesPerLineTab[cirrusTilesPerLineIndex].
 	    tilesPerLine;
	  new->DTTC = (new->TILE << 8) | 0x0080 | 
	    (cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width << 6);
	  new->CONTROL = 0x4000 | 
	    (cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width << 11);


	  switch (mode->CrtcHDisplay) {
	  case 640:
	    /* BAD numbers:   */
	    /* GOOD numbers:  0x10 */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0010);
	    break;
	  default:
	  case 800:
	    /* BAD numbers:   */
	    /* GOOD numbers:   0x11 */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0011);
	    break;
	  case 1024:
	    /* BAD numbers:  0x12 0x13 */
	    /* Borderline numbers:  0x15 */
	    /* GOOD numbers:  */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0017);
	    break;
	  case 1280:
	    /* BAD numbers:   */
	    /* GOOD numbers:  */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0019);
	    break;
	  }
	  break;


	case 32:
	  new->FORMAT = 0x3400;

 	  new->TILE = cirrusTilesPerLineTab[cirrusTilesPerLineIndex].
 	    tilesPerLine;
	  new->DTTC = (new->TILE << 8) | 0x0080 | 
	    (cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width << 6);
	  new->CONTROL = 0x6000 | 
	    (cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width << 11);


	  switch (mode->CrtcHDisplay) {
	  case 640:
	    /* GOOD numbers:  0x0E */
	    /* BAD numbers:  */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x000E);
	    break;
	  default:
	  case 800:
	    /* GOOD numbers:  0x17 */
	    /* BAD numbers:  */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0017);
	    break;
	  case 1024:
	    /* GOOD numbers: */
	    /* OKAY numbers:  0x15 0x14 0x16 0x18 */
	    /* BAD numbers:  0x0E 0x12 0x13 0x0D */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x0019);
	    break;
	  case 1280:
	    /* GOOD numbers:  */
	    /* BAD numbers:  */
	    new->DTTC = (new->DTTC & 0xFFE0) | (0x000E); /* 10 */
	    break;
	  }
	  break;
	} /* vgaBitsPerPixel switch statement */


	/* Does the user have a better idea about how the FIFO should
	   be set?  */
	if (OFLG_ISSET(OPTION_FIFO_CONSERV, &vga256InfoRec.options)) {
	  /* Increase the DTTC threshold value -- request a Rambus
	     refresh _sooner_. */
	  int oldThresh = new->DTTC & 0x003F;
	  oldThresh += 2;
	  if (oldThresh > 0x3F)
	    oldThresh = 0x3F;
	  new->DTTC = (new->DTTC & 0xFFE0) | (oldThresh & 0x003F);
	} else if (OFLG_ISSET(OPTION_FIFO_AGGRESSIVE, 
			      &vga256InfoRec.options)) {
	  /* Decrease the DTTC threshold value -- wait until the FIFO
	     is fuller before requesting a Rambus refresh. */
	  int oldThresh = new->DTTC & 0x003F;
	  oldThresh -= 2;
	  if (oldThresh < 0)
	    oldThresh = 0x0;
	  new->DTTC = (new->DTTC & 0xFFE0) | (oldThresh & 0x003F);
	}
	  

	/* Set up the proper memory interleave.  The interleave is dependent
	   only upon how much memory there is installed. */
	switch (vga256InfoRec.videoRam) {
	case 1024:
	case 3072:
	case 5120:
	case 6144:
	case 7168:
	  /* One-way interleaving */
	  cirrusMemoryInterleave = 0x00;
	  break;

	case 2048:
	  /* Two-way interleaving */
	  cirrusMemoryInterleave = 0x40;
	  break;

	case 4096:
	case 8192:
	  /* Four-way interleaving */
	  cirrusMemoryInterleave = 0x80;
	  break;
	} /* memory size switch statement */


	if (cirrusChip == CLGD5465)
	  new->TileCtrl = new->DTTC & 0xFFC0;
      } /* if HAVE546X() */


     if (cirrusChip == CLGD6225) 
	  {
	  /* Don't ask me why the following number works, but it
	   * does work for a Sager 8200 using the BIOS initialization
	   * of the LCD for all other functions.  Without this, the
	   * Sager's display is 8 pixels left and 1 down from where
	   * it should be....  If things are shifted on your display,
	   * the documentation says to +1 for each 8 columns you want
	   * to move left...  but it seems to work in the opposite
	   * direction on my screen.  Anyway, this works for me, and
	   * it is easy to play with if it doesn't work for you.
	   */
	  new->RAX = 12;
          }

#ifdef CIRRUS_SUPPORT_MMIO
	/*
	 * Ugly hack to get MMIO base address.
         * Registers are mapped at 0xb8000; that is at an offset of
         * 0x18000 from vgaBase (0xa0000).
         */
	if (cirrusUseMMIO && cirrusMMIOBase == NULL && !HAVE546X())
		cirrusMMIOBase = (unsigned char *)vgaBase + 0x18000;
#endif

  return(TRUE);
}



static int screenStartX;
static int screenStartY;
void
cirrusCursorAdjust(int deltaXTiles)
{
  /* This function is called from cir_cursor.c:cirrusMoveCursor().  Because
     of the tiled memory of the '6x family, the desktop is always aligned 
     with a tile boundry.  This is not how X envisions the virtual desktop, 
     though; X thinks that the desktop can be aligned with any pixel.  Thus,
     when X moves the cursor and changes the cursor pattern, it does so w.r.t.
     what *it* thinks the virtual desktop is.

     What does this all mean?  It means that, if left alone, the mouse will
     become misaligned with the screen (in particular, the cursor pattern will
     not change when the cursor passes over windows, etc.).  To solve this
     problem, we must artificially offset the cursor position on the '6x HW 
     cursor.  (This is why the '6x can't use a SW cursor -- the virtual 
     desktop won't work correctly).  The downside of offsetting the cursor is
     that the cursor may wander off the visable screen, but X won't know 
     (e.g., 800x600 display, cursor offset +20, X asks for cursor at (790,10).
     790 + 20 > 800, so off screen).  To remedy this, the moveCursor routine
     explicitly asks that the virtual desktop be adjusted when it *knows* that
     the mouse has wandered off screen (when x + offset > displayWidth).
     */
  cirrusAdjust(screenStartX, screenStartY);
}


/*
 * cirrusAdjust --
 *      adjust the current video frame to display the mousecursor
 */
static void 
cirrusAdjust(x, y)
     int x, y;
{
     unsigned char CR1B, CR1D, tmp;
     int Base;
     static char onLeftSide = 0;
     unsigned char lsb;
     const int bumpThresh = 4;

#ifdef MONOVGA
     /* Remember where X thinks the screen is at. */
     screenStartX = x;
     screenStartY = y;

     if (HAVE546X()) {
       /* If the screen isn't aligned to a tile boundry, the first tile
	  is hosed.  Tiles on the 546X are either 128 or 256 bytes wide.
	  The width can be determined by looking at cirrusTilesPerLineTab
	  at index cirrusTilesPerLineIndex and examining the width field.
 
 	  NOTE:  This problem has been fixed in the CL-GD5465.
	  */

       int pixelsPerTile;
       int wideTiles = cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width;
       unsigned short *pX = (unsigned short *)(cirrusMMIOBase + 0xE0);
       int screenWidth;

       if (vgaBitsPerPixel == 8)
	 pixelsPerTile = wideTiles?256:128;
       else if (vgaBitsPerPixel == 16)
	 pixelsPerTile = wideTiles?128:64;
       else if (vgaBitsPerPixel == 24)       /* Screen boundary must be tile */
	 pixelsPerTile = wideTiles?256:128;  /* boundary and pixel boundary. */
       else if (vgaBitsPerPixel == 32)
	 pixelsPerTile = wideTiles?64:32;

       screenWidth = vga256InfoRec.frameX1 - vga256InfoRec.frameX0;
       if (cirrusChip != CLGD5465 ||
	   (cirrusChip == CLGD5465 && vgaBitsPerPixel == 24)) {
	 if (*pX < bumpThresh) {
	   /* Bumping up against left edge.  Bias screen to left. */
	   x = (x / pixelsPerTile) * pixelsPerTile;
	   onLeftSide = 1;
	 } else if (*pX > screenWidth - bumpThresh) {
	   /* Bumping up against right edge.  Bias screen to right. */
	   x = ((x+pixelsPerTile-1) / pixelsPerTile) * pixelsPerTile;
	   onLeftSide = 0;
	 } else if (onLeftSide) {
	   /* Scrolling vertically, but screen is biased to left edge. */
	   x = (x / pixelsPerTile) * pixelsPerTile;
	 } else /* !onLeftSide */ {
	   /* Scrolling vertically, but screen is biased to right edge. */
	   x = ((x+pixelsPerTile-1) / pixelsPerTile) * pixelsPerTile;
	 }

	 /* Don't scroll the virtual desktop too far right.  If the desktop
	    gets aligned with one tile too far right, then you see the screen
	    wrapped horizontally on the far right of the display. */
	 if (x > vga256InfoRec.displayWidth - screenWidth)
	   x -= pixelsPerTile;
       }

       cirrusLgCursorXOffset = screenStartX - x;
     }

     Base = (y * vga256InfoRec.displayWidth + x);
     lsb = Base & 7;
     Base >>= 3;
#else

     /* Remember where X thinks the screen is at. */
     screenStartX = x;
     screenStartY = y;


     if (HAVE546X()) {
       /* If the screen isn't aligned to a tile boundry, the first tile
	  is hosed.  Tiles on the 546X are either 128 or 256 bytes wide.
	  The width can be determined by looking at cirrusTilesPerLineTab
	  at index cirrusTilesPerLineIndex and examining the width field. 
	  
	  NOTE:  This problem has been fixed in the CL-GD5465.
	  */

       int pixelsPerTile;
       int wideTiles = cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width;
       unsigned short *pX = (unsigned short *)(cirrusMMIOBase + 0xE0);
       int screenWidth;

       if (vgaBitsPerPixel == 8)
	 pixelsPerTile = wideTiles?256:128;
       else if (vgaBitsPerPixel == 16)
	 pixelsPerTile = wideTiles?128:64;
       else if (vgaBitsPerPixel == 24)       /* Screen boundary must be tile */
	 pixelsPerTile = wideTiles?256:128;  /* boundary and pixel boundary. */
       else if (vgaBitsPerPixel == 32)
	 pixelsPerTile = wideTiles?64:32;


       screenWidth = vga256InfoRec.frameX1 - vga256InfoRec.frameX0;

       if (cirrusChip != CLGD5465 ||
	   (cirrusChip == CLGD5465 && vgaBitsPerPixel == 24)) {
	 if (*pX < 2) {
	   /* Bumping up against left edge.  Bias screen to left. */
	   x = (x / pixelsPerTile) * pixelsPerTile;
	   onLeftSide = 1;
	 } else if (*pX > screenWidth - 2) {
	   /* Bumping up against right edge.  Bias screen to right. */
	   x = ((x+pixelsPerTile-1) / pixelsPerTile) * pixelsPerTile;
	   onLeftSide = 0;
	 } else if (onLeftSide) {
	   /* Scrolling vertically, but screen is biased to left edge. */
	   x = (x / pixelsPerTile) * pixelsPerTile;
	 } else /* !onLeftSide */ {
	   /* Scrolling vertically, but screen is biased to right edge. */
	   x = ((x+pixelsPerTile-1) / pixelsPerTile) * pixelsPerTile;
	 }

	 /* Don't scroll the virtual desktop too far right.  If the desktop
	    gets aligned with one tile too far right, then you see the screen
	    wrapped horizontally on the far right of the display. */
	 if (x > vga256InfoRec.displayWidth - screenWidth)
	   x -= pixelsPerTile;
       }

       cirrusLgCursorXOffset = screenStartX - x;
     }
       
     Base = (y * vga256InfoRec.displayWidth + x);
     if (vgaBitsPerPixel == 16)
         Base <<= 1;
     else if (vgaBitsPerPixel == 24 && HAVE546X()) {
#if 0
       screenBytePitch = vga256InfoRec.displayWidth*3;  /* not incl. padding */
       screenBytePitch = (screenBytePitch + 3) / 4;     /* # dwords padded */
       screenBytePitch *= 4;                            /* # bytes incl. pad */
#endif
       Base = y*cirrusTilesPerLineTab[cirrusTilesPerLineIndex].pitch + x*3;
     }
     else if (vgaBitsPerPixel == 24 && !HAVE546X())
     	 Base *= 3;
     else if (vgaBitsPerPixel == 32)
         Base <<= 2;
     lsb = Base & 0x03;
     Base >>= 2;
     if (vgaBitsPerPixel == 24 && !HAVE546X())
         Base -= Base % 6;
#endif
     outw(vgaIOBase + 4, (Base & 0x00FF00) | 0x0C);
     outw(vgaIOBase + 4, ((Base & 0x00FF) << 8) | 0x0D);

     outb(vgaIOBase + 4,0x1B); CR1B = inb(vgaIOBase + 5);
     outb(vgaIOBase + 5,(CR1B & 0xF2) | ((Base & 0x060000) >> 15)
	  | ((Base & 0x010000) >> 16) );
     if (cirrusChip == CLGD5434 || cirrusChip == CLGD5436 ||
	 cirrusChip == CLGD5446 || cirrusChip == CLGD5480) {
         outb(vgaIOBase + 4, 0x1d); CR1D = inb(vgaIOBase + 5);
         outb(vgaIOBase + 5, (CR1D & 0x7f) | ((Base & 0x080000) >> 12));
     } else if (HAVE546X()) {
         outb(vgaIOBase + 4, 0x1d); CR1D = inb(vgaIOBase + 5);
         outb(vgaIOBase + 5, (CR1D & 0xE7) | ((Base & 0x180000) >> 16));
     }       

#ifdef XFreeXDGA
    if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
	/* Wait until vertical retrace is in progress. */
	while (inb(vgaIOBase + 0xA) & 0x08);
	while (!(inb(vgaIOBase + 0xA) & 0x08));
    }
#endif
}

/*
 * cirrusValidMode --
 *
 */
static int
cirrusValidMode(mode, verbose, flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
return MODE_OK;
}



/*
   cirrusScreenInit
   Hook the various graphics operations.
*/
static Bool
cirrusScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width)
     register ScreenPtr pScreen;
     pointer pbits;		/* pointer to screen bitmap */
     int xsize, ysize;		/* in pixels */
     int dpix, dpiy;		/* dots per inch */
     int width;			/* pixel width of frame buffer */
{
  return TRUE;
}





#ifdef DEBUG_CIRRUS
static void cirrusDumpRegs(vgacirrusPtr mode) 
{
  int i;

  if (!HAVE546X() || !cirrusMMIOBase)
    return;

  for (i = 0; i < 0x18; i++)
    fprintf(stderr, "CRTC[0x%02X] = 0x%02X = 0x%02X\n", i, 
	    mode->std.CRTC[i], cirrusMMIOBase[i<<2]);
  fprintf(stderr, "CRTC[0x1A] = 0x%02X = 0x%02X\n", mode->CR1A, 
	  cirrusMMIOBase[0x1A<<2]);
  fprintf(stderr, "CRTC[0x1B] = 0x%02X = 0x%02X\n", mode->CR1B,
	  cirrusMMIOBase[0x1B<<2]);
  fprintf(stderr, "CRTC[0x1D] = 0x%02X = 0x%02X\n", mode->CR1D,
	  cirrusMMIOBase[0x1D<<2]);
  fprintf(stderr, "CRTC[0x1E] = 0x%02X = 0x%02X\n", mode->CR1E,
	  cirrusMMIOBase[0x1E<<2]);
  fprintf(stderr, "SRE = 0x%02X\n", mode->SRE);
  fprintf(stderr, "SR1E = 0x%02X\n", mode->SR1E);
  fprintf(stderr, "FORMAT = 0x%04X\n", mode->FORMAT);
  fprintf(stderr, "VSC = 0x%08X\n", mode->VSC);
  fprintf(stderr, "DTTC = 0x%04X\n", mode->DTTC);
  fprintf(stderr, "TILE = 0x%02X\n", mode->TILE);
  fprintf(stderr, "CONTROL = 0x%04X\n", mode->CONTROL);
}
#endif /* DEBUG_CIRRUS */


/*
 * MGADisplayPowerManagementSet --
 *
 * Sets VESA Display Power Management Signaling (DPMS) Mode.
 */
#ifdef DPMSExtension
static void CirrusDisplayPowerManagementSet(PowerManagementMode)
int PowerManagementMode;
{
    unsigned char GRE;
    if (!xf86VTSema)
        return;
    GRE = rdinx(0x3CE, 0x0E);
    /* Mask out the DPMS mode bits. */
    GRE &= ~0x06;
    switch (PowerManagementMode) {
    case DPMSModeOn:
        /* Screen: On; HSync: On, VSync: On */
        /* Program zero into GRE [2:1]. */
        break;
    case DPMSModeStandby:
        /* Screen: Off; HSync: Off, VSync: On */
        GRE |= 0x2;		/* Program 01 (binary) into GRE [2:1]. */
        break;
     case DPMSModeSuspend:
        /* Screen: Off; HSync: On, VSync: Off */
        GRE |= 0x4;
        break;
    case DPMSModeOff:
        /* Screen: Off; HSync: Off, VSync: Off */
        GRE |= 0x6;
        break;
    }
    wrinx(0x3CE, 0x0E, GRE);
}
#endif

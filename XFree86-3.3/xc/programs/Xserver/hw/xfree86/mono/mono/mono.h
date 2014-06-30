/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/mono/mono.h,v 3.4 1996/12/23 06:48:47 dawes Exp $ */
/*
 * MONO: Driver family for interlaced and banked monochrome video adaptors
 * Pascal Haible 8/93, 3/94, 4/94 haible@IZFM.Uni-Stuttgart.DE
 *
 * mono/mono/mono.h
 *
 * derived from:
 * hga2/...
 * Author:  Davor Matic, dmatic@athena.mit.edu
 * and
 * vga256/...
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * see mono/COPYRIGHT for copyright and disclaimers.
 */
/* $XConsortium: mono.h /main/5 1996/02/21 17:50:21 kaleb $ */

#include "mfb.h"	/* PixelType */

#define MONO_PATCHLEVEL "0"

#include "X.h"
#include "misc.h"
#include "xf86.h"

extern int     monoValidTokens[];

/*
 * structure for accessing the video chip`s functions
 *
 * We are doing a total encapsulation of the driver's functions.
 * Banking (monoSetReadWrite(p) etc.) is done in monoBank.c
 *   using the chip's function pointed to by bmpSetReadWriteFunc(bank)
 */
typedef struct {
  Bool (* ChipProbe)(
#if NeedFunctionPrototypes
		void
#endif
  );
  char * (* ChipIdent)(
#if NeedFunctionPrototypes
                 int
#endif
  );
  pointer (* ChipInit)(
#if NeedFunctionPrototypes
		DisplayModePtr
#endif
  );
  void (* ChipFbInit)(
#if NeedFunctionPrototypes
		void
#endif
  );
  void (* ChipEnterLeave)(
#if NeedFunctionPrototypes
		Bool /* enter */
#endif
  );
  pointer (* ChipSave)(
#if NeedFunctionPrototypes
		pointer
#endif
  );
  void (* ChipRestore)(
#if NeedFunctionPrototypes
		pointer
#endif
  );
  void (* ChipAdjust)(
#if NeedFunctionPrototypes
		int /* x */,
		int /* y */
#endif
  );
  Bool (* ChipSaveScreen)(
#if NeedFunctionPrototypes
		ScreenPtr /* pScreen */,
		Bool      /* on */
#endif
  );
  void (* ChipClearScreen)(
#if NeedFunctionPrototypes
		void
#endif
  );
  PixelType * (* ChipScanlineOffsetA)(
#if NeedFunctionPrototypes
		PixelType * /* pointer */,
		int         /* offset */
#endif
  );
  PixelType * (* ChipScanlineOffsetB)(
#if NeedFunctionPrototypes
		PixelType * /* pointer */,
		int         /* offset */
#endif
  );
  /* Bottom and top of the banking window (rel. to ChipMapBase)	*/
  /* Note: Top = highest accessable byte + 1			*/
  unsigned char *ChipBankABottom;
  unsigned char *ChipBankATop;
  unsigned char *ChipBankBBottom;
  unsigned char *ChipBankBTop;
  /* Memory to map */
  unsigned char *ChipMapBase;
  unsigned int ChipMapSize;	/* replaces MEMTOMAP */
  /* Display size is given by the driver */
  int ChipHDisplay;
  int ChipVDisplay;
  /* In case scan line in mfb is longer than HDisplay */
  int ChipScanLineWidth;	/* in pixels */
  /* option flags support by this driver */
  OFlagSet ChipOptionFlags;
} monoVideoChipRec, *monoVideoChipPtr;

/* variables in mono.c */
extern unsigned char *monoBase;
extern unsigned char *monoBankABottom;
extern unsigned char *monoBankATop;
extern unsigned char *monoBankBBottom;
extern unsigned char *monoBankBTop;
extern int  monoBankAseg;
extern int  monoBankBseg;

#ifdef CSRG_BASED
#define MONOBASE 0xFF000000
#else
#define MONOBASE 0xF0000000
#endif

#define BITS_PER_GUN 1
#define COLORMAP_SIZE 2

extern ScrnInfoRec monoInfoRec;

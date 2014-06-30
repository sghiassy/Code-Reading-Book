/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64curs.c,v 3.16.2.1 1997/05/31 13:34:38 dawes Exp $ */
/*
 * 
 * Copyright 1991 MIPS Computer Systems, Inc.
 * Copyright 1994,1995,1996 by Kevin E. Martin, Chapel Hill, North Carolina.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of MIPS not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  MIPS makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 * 
 * MIPS, AMANCIO HASTY, JON TOMBS, MIKE BERNSON, MARK WEAVER, AND KEVIN
 * E. MARTIN DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL MIPS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 *
 * Modified by Amancio Hasty and Jon Tombs
 * Modified for mach32 by Mike Bernson mike@mbsun.mlb.org
 * Modified for VTs and oversized cursors by Mark_Weaver@brown.edu
 * Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)
 *
 */
/* $XConsortium: mach64curs.c /main/15 1996/10/27 18:06:18 kaleb $ */

#include <signal.h>

#define NEED_EVENTS
#include <X.h>
#include "Xproto.h"
#include <misc.h>
#include <input.h>
#include <cursorstr.h>
#include <regionstr.h>
#include <scrnintstr.h>
#include <servermd.h>
#include <windowstr.h>
#include <mipointer.h>
#include "mfb.h"
#include "xf86.h"
#include "inputstr.h"
#include "xf86Priv.h"
#include "xf86_Option.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "mach64.h"

typedef struct _Mach64CursPriv {
    unsigned short data[MACH64_CURSBYTES/2]; /* cursor definition */
    int yExtra;	/* number of empty pixels from the bottom */
} Mach64CursPriv;

unsigned char *mach64CursorMemory;
unsigned int  mach64CursorOffset;

extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern Bool mach64InDoubleScanMode;

static int xhot, yhot;
static int mach64CursGeneration = -1;
static CursorPtr mach64SaveCursors[MAXSCREENS];
static int mach64CursYExtra;
static int mach64CursLastEnabled = -1;

static miPointerSpriteFuncRec mach64PointerSpriteFuncs = {
  mach64RealizeCursor,
  mach64UnrealizeCursor,
  mach64SetCursor,
  mach64MoveCursor,
};

unsigned short cursor_lookup[256] = {
	0x0000, 0x0001, 0x0004, 0x0005, 0x0010, 0x0011, 0x0014, 0x0015, 
	0x0040, 0x0041, 0x0044, 0x0045, 0x0050, 0x0051, 0x0054, 0x0055, 
	0x0100, 0x0101, 0x0104, 0x0105, 0x0110, 0x0111, 0x0114, 0x0115, 
	0x0140, 0x0141, 0x0144, 0x0145, 0x0150, 0x0151, 0x0154, 0x0155, 
	0x0400, 0x0401, 0x0404, 0x0405, 0x0410, 0x0411, 0x0414, 0x0415, 
	0x0440, 0x0441, 0x0444, 0x0445, 0x0450, 0x0451, 0x0454, 0x0455, 
	0x0500, 0x0501, 0x0504, 0x0505, 0x0510, 0x0511, 0x0514, 0x0515, 
	0x0540, 0x0541, 0x0544, 0x0545, 0x0550, 0x0551, 0x0554, 0x0555, 
	0x1000, 0x1001, 0x1004, 0x1005, 0x1010, 0x1011, 0x1014, 0x1015, 
	0x1040, 0x1041, 0x1044, 0x1045, 0x1050, 0x1051, 0x1054, 0x1055, 
	0x1100, 0x1101, 0x1104, 0x1105, 0x1110, 0x1111, 0x1114, 0x1115, 
	0x1140, 0x1141, 0x1144, 0x1145, 0x1150, 0x1151, 0x1154, 0x1155, 
	0x1400, 0x1401, 0x1404, 0x1405, 0x1410, 0x1411, 0x1414, 0x1415, 
	0x1440, 0x1441, 0x1444, 0x1445, 0x1450, 0x1451, 0x1454, 0x1455, 
	0x1500, 0x1501, 0x1504, 0x1505, 0x1510, 0x1511, 0x1514, 0x1515, 
	0x1540, 0x1541, 0x1544, 0x1545, 0x1550, 0x1551, 0x1554, 0x1555, 
	0x4000, 0x4001, 0x4004, 0x4005, 0x4010, 0x4011, 0x4014, 0x4015, 
	0x4040, 0x4041, 0x4044, 0x4045, 0x4050, 0x4051, 0x4054, 0x4055, 
	0x4100, 0x4101, 0x4104, 0x4105, 0x4110, 0x4111, 0x4114, 0x4115, 
	0x4140, 0x4141, 0x4144, 0x4145, 0x4150, 0x4151, 0x4154, 0x4155, 
	0x4400, 0x4401, 0x4404, 0x4405, 0x4410, 0x4411, 0x4414, 0x4415, 
	0x4440, 0x4441, 0x4444, 0x4445, 0x4450, 0x4451, 0x4454, 0x4455, 
	0x4500, 0x4501, 0x4504, 0x4505, 0x4510, 0x4511, 0x4514, 0x4515, 
	0x4540, 0x4541, 0x4544, 0x4545, 0x4550, 0x4551, 0x4554, 0x4555, 
	0x5000, 0x5001, 0x5004, 0x5005, 0x5010, 0x5011, 0x5014, 0x5015, 
	0x5040, 0x5041, 0x5044, 0x5045, 0x5050, 0x5051, 0x5054, 0x5055, 
	0x5100, 0x5101, 0x5104, 0x5105, 0x5110, 0x5111, 0x5114, 0x5115, 
	0x5140, 0x5141, 0x5144, 0x5145, 0x5150, 0x5151, 0x5154, 0x5155, 
	0x5400, 0x5401, 0x5404, 0x5405, 0x5410, 0x5411, 0x5414, 0x5415, 
	0x5440, 0x5441, 0x5444, 0x5445, 0x5450, 0x5451, 0x5454, 0x5455, 
	0x5500, 0x5501, 0x5504, 0x5505, 0x5510, 0x5511, 0x5514, 0x5515, 
	0x5540, 0x5541, 0x5544, 0x5545, 0x5550, 0x5551, 0x5554, 0x5555
};


unsigned short cursor_mask[256] = {
	0xaaaa, 0xaaa8, 0xaaa2, 0xaaa0, 0xaa8a, 0xaa88, 0xaa82, 0xaa80, 
	0xaa2a, 0xaa28, 0xaa22, 0xaa20, 0xaa0a, 0xaa08, 0xaa02, 0xaa00, 
	0xa8aa, 0xa8a8, 0xa8a2, 0xa8a0, 0xa88a, 0xa888, 0xa882, 0xa880, 
	0xa82a, 0xa828, 0xa822, 0xa820, 0xa80a, 0xa808, 0xa802, 0xa800, 
	0xa2aa, 0xa2a8, 0xa2a2, 0xa2a0, 0xa28a, 0xa288, 0xa282, 0xa280, 
	0xa22a, 0xa228, 0xa222, 0xa220, 0xa20a, 0xa208, 0xa202, 0xa200, 
	0xa0aa, 0xa0a8, 0xa0a2, 0xa0a0, 0xa08a, 0xa088, 0xa082, 0xa080, 
	0xa02a, 0xa028, 0xa022, 0xa020, 0xa00a, 0xa008, 0xa002, 0xa000, 
	0x8aaa, 0x8aa8, 0x8aa2, 0x8aa0, 0x8a8a, 0x8a88, 0x8a82, 0x8a80, 
	0x8a2a, 0x8a28, 0x8a22, 0x8a20, 0x8a0a, 0x8a08, 0x8a02, 0x8a00, 
	0x88aa, 0x88a8, 0x88a2, 0x88a0, 0x888a, 0x8888, 0x8882, 0x8880, 
	0x882a, 0x8828, 0x8822, 0x8820, 0x880a, 0x8808, 0x8802, 0x8800, 
	0x82aa, 0x82a8, 0x82a2, 0x82a0, 0x828a, 0x8288, 0x8282, 0x8280, 
	0x822a, 0x8228, 0x8222, 0x8220, 0x820a, 0x8208, 0x8202, 0x8200, 
	0x80aa, 0x80a8, 0x80a2, 0x80a0, 0x808a, 0x8088, 0x8082, 0x8080, 
	0x802a, 0x8028, 0x8022, 0x8020, 0x800a, 0x8008, 0x8002, 0x8000, 
	0x2aaa, 0x2aa8, 0x2aa2, 0x2aa0, 0x2a8a, 0x2a88, 0x2a82, 0x2a80, 
	0x2a2a, 0x2a28, 0x2a22, 0x2a20, 0x2a0a, 0x2a08, 0x2a02, 0x2a00, 
	0x28aa, 0x28a8, 0x28a2, 0x28a0, 0x288a, 0x2888, 0x2882, 0x2880, 
	0x282a, 0x2828, 0x2822, 0x2820, 0x280a, 0x2808, 0x2802, 0x2800, 
	0x22aa, 0x22a8, 0x22a2, 0x22a0, 0x228a, 0x2288, 0x2282, 0x2280, 
	0x222a, 0x2228, 0x2222, 0x2220, 0x220a, 0x2208, 0x2202, 0x2200, 
	0x20aa, 0x20a8, 0x20a2, 0x20a0, 0x208a, 0x2088, 0x2082, 0x2080, 
	0x202a, 0x2028, 0x2022, 0x2020, 0x200a, 0x2008, 0x2002, 0x2000, 
	0x0aaa, 0x0aa8, 0x0aa2, 0x0aa0, 0x0a8a, 0x0a88, 0x0a82, 0x0a80, 
	0x0a2a, 0x0a28, 0x0a22, 0x0a20, 0x0a0a, 0x0a08, 0x0a02, 0x0a00, 
	0x08aa, 0x08a8, 0x08a2, 0x08a0, 0x088a, 0x0888, 0x0882, 0x0880, 
	0x082a, 0x0828, 0x0822, 0x0820, 0x080a, 0x0808, 0x0802, 0x0800, 
	0x02aa, 0x02a8, 0x02a2, 0x02a0, 0x028a, 0x0288, 0x0282, 0x0280, 
	0x022a, 0x0228, 0x0222, 0x0220, 0x020a, 0x0208, 0x0202, 0x0200, 
	0x00aa, 0x00a8, 0x00a2, 0x00a0, 0x008a, 0x0088, 0x0082, 0x0080, 
	0x002a, 0x0028, 0x0022, 0x0020, 0x000a, 0x0008, 0x0002, 0x0000, 
};

Bool checkCursorColor = FALSE;

Bool
mach64CursorInit(pm, pScr)
     char *pm;
     ScreenPtr pScr;
{
  if (mach64CursGeneration != serverGeneration) {
      if (!(miPointerInitialize(pScr, &mach64PointerSpriteFuncs,
				&xf86PointerScreenFuncs, FALSE)))
	  return FALSE;

      xhot = 0;
      yhot = 0;
      pScr->RecolorCursor = mach64RecolorCursor;
      mach64CursGeneration = serverGeneration;
  }

  if (mach64RamdacSubType != DAC_IBMRGB514) {
      mach64CursorOffset = mach64InfoRec.videoRam * 1024;
      mach64CursorMemory = (unsigned char *)mach64VideoMem +
	  mach64CursorOffset;
  }

  mach64CursLastEnabled = FALSE;
  return TRUE;
}

Bool
mach64RealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
  register int    i;
  register int	  j;
  unsigned char   *pServMsk;
  unsigned char   *pServSrc;
  int             index = pScr->myNum;
  Mach64CursPriv  *cursPriv;
  pointer         *pPriv = &pCurs->bits->devPriv[index];
  int             w;
  int             h;
  unsigned short  *ram;
  CursorBitsPtr   bits = pCurs->bits;

  if (pCurs->bits->refcnt > 1)
	return TRUE;

  cursPriv = (Mach64CursPriv *)xcalloc(1,sizeof(Mach64CursPriv));
  *pPriv = (pointer)cursPriv;
  if (!cursPriv) 
     return (FALSE);
  ram = cursPriv->data;

  pServSrc = (unsigned char *)bits->source;
  pServMsk = (unsigned char *)bits->mask;

  h = bits->height;
  if (h > MACH64_CURSMAX)
      h = MACH64_CURSMAX;

  w = PixmapBytePad(bits->width, 1);
  if (w > MACH64_CURSMAX / 8)
      w = MACH64_CURSMAX / 8;

  for(i = 0; i < h; i++) {
      for(j = 0; j < w; j++) {
	  *ram++ = cursor_mask[*pServMsk] |
		   cursor_lookup[*pServSrc & *pServMsk];
	  pServMsk++;
	  pServSrc++;
      }
      for(; j < MACH64_CURSMAX / 8; j++)
	  *ram++ = 0xaaaa;
  }
  memset(ram, 0xaa, (MACH64_CURSMAX-h)*16);

  cursPriv->yExtra = MACH64_CURSMAX - h;
  return(TRUE);
}


Bool
mach64UnrealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
  pointer priv;

  if (pCurs->bits->refcnt <= 1 &&
      (priv = pCurs->bits->devPriv[pScr->myNum]))
    xfree(priv);

  return (TRUE);
}

static void 
mach64LoadCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int x, y;
{
  int  index = pScr->myNum;
  Mach64CursPriv *cursPriv;
  unsigned char *data;
  int i;

  if (!xf86VTSema)
      return;
  
  if (!pCurs)
      return;

  cursPriv = (Mach64CursPriv *)pCurs->bits->devPriv[index];
  data = (unsigned char *)cursPriv->data;
  if (mach64RamdacSubType == DAC_IBMRGB514)
      for (i = 0; i < 1024; i++)
	  mach64P_RGB514Index(0x100 + i, data[i]);
  else
      MemToBus(mach64CursorMemory, cursPriv->data, MACH64_CURSBYTES);
  mach64CursYExtra = cursPriv->yExtra;

  mach64MoveCursor(0, x, y);
  mach64RecolorCursor(pScr, pCurs, TRUE);
}

void
mach64SetCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int       x, y;
{
  int  index = pScr->myNum;

  if(!pCurs)
	return;

  xhot = pCurs->bits->xhot;
  yhot = pCurs->bits->yhot;
  mach64SaveCursors[index] = pCurs;
  
  mach64LoadCursor(pScr, pCurs, x, y);
}

void
mach64RepositionCursor(pScr)
    ScreenPtr pScr;
{
    int x, y;
    
    if(pScr && mach64SaveCursors[pScr->myNum]) {
	miPointerPosition(&x, &y);
	mach64CursLastEnabled = FALSE;
	mach64MoveCursor(pScr, x, y);
    }
}

void
mach64RestoreCursor(pScr)
   ScreenPtr pScr;
{
   int index = pScr->myNum;
   int x, y;

   miPointerPosition(&x, &y);
   mach64LoadCursor(pScr, mach64SaveCursors[index], x, y);
}

void
mach64MoveCursor(pScr, x, y)
    ScreenPtr pScr;
    int       x, y;
{
    int xoff;
    int yoff;
    int enabled;

    if (!xf86VTSema)
	return;

    if (mach64RamdacSubType == DAC_IBMRGB514)
    {
	x -= mach64InfoRec.frameX0;
	y -= mach64InfoRec.frameY0;

	if (x <= mach64InfoRec.frameX1 - mach64InfoRec.frameX0 &&
	    y <= mach64InfoRec.frameY1 - mach64InfoRec.frameY0)
	{
	    mach64P_RGB514Index(0x31, x & 0xff);
	    mach64P_RGB514Index(0x32, x >> 8);
	    mach64P_RGB514Index(0x33, y & 0xff);
	    mach64P_RGB514Index(0x34, y >> 8);
	    mach64P_RGB514Index(0x35, xhot);
	    mach64P_RGB514Index(0x36, yhot);
	    if (!mach64CursLastEnabled)
		mach64P_RGB514Index(0x30, 0x06);
	    mach64CursLastEnabled = TRUE;
	}
	else if (mach64CursLastEnabled)
	{
	    mach64P_RGB514Index(0x30, 0x00);
	    mach64CursLastEnabled = FALSE;
	}

	WaitIdleEmpty();
	return;
    }

    x -= xhot + (mach64InfoRec.frameX0 & ~0x07);
    y -= yhot + mach64InfoRec.frameY0;

    if (x < 0) {
	xoff = -x;
	x = 0;
    } else {
	xoff = 0;
    }

    if (y < 0) {
	yoff = -y;
	y = 0;
    } else {
	yoff = 0;
    }

    /*
     * Only display the cursor if we can see at least one pixel of the
     * cursor definition will appear on the screen.  Without this
     * check, display errors can result when the hot spot is too far
     * off the screen.  This can happen during scrolling when the
     * virtual screen is larger than the real screen.
     */
    enabled = (x <= mach64InfoRec.frameX1 - mach64InfoRec.frameX0 &&
	       y <= mach64InfoRec.frameY1 - mach64InfoRec.frameY0 &&
	       xoff < MACH64_CURSMAX && yoff < MACH64_CURSMAX);

    if (enabled) 
    {
	WaitQueue(4);
	regw(CUR_OFFSET, (mach64CursorOffset >> 3) + (yoff << 1));
	switch (mach64RamdacSubType) {
	case DAC_CH8398:
	case DAC_STG1702:
	case DAC_STG1703:
	case DAC_ATT20C408:
	    if (mach64InfoRec.bitsPerPixel == 32) {
		regw(CUR_HORZ_VERT_OFF,
		     (((mach64CursYExtra + yoff) << 16) | (xoff & 0x1e)));
		regw(CUR_HORZ_VERT_POSN, ((y << 16) | (x & 0x03fe)));
	    } else {
		regw(CUR_HORZ_VERT_OFF,
		     (((mach64CursYExtra + yoff) << 16) | xoff));
		regw(CUR_HORZ_VERT_POSN, ((y << 16) | x));
	    }
	    break;
	default:
	    regw(CUR_HORZ_VERT_OFF,
		 (((mach64CursYExtra + yoff) << 16) | xoff));
	    if (mach64InDoubleScanMode)
		regw(CUR_HORZ_VERT_POSN, ((y << 17) | x));
	    else
		regw(CUR_HORZ_VERT_POSN, ((y << 16) | x));
	    break;
	}
	if (!mach64CursLastEnabled)
	    regw(GEN_TEST_CNTL, regr(GEN_TEST_CNTL) | HWCURSOR_ENABLE);
    }
    else if (mach64CursLastEnabled)
    {
	WaitQueue(4);
	regw(GEN_TEST_CNTL, regr(GEN_TEST_CNTL) & (~HWCURSOR_ENABLE));
    }

    mach64CursLastEnabled = enabled;
    WaitIdleEmpty();
}

void
mach64RenewCursorColor(pScr)
     ScreenPtr pScr;
{
    if (!xf86VTSema)
	return;

    if (mach64SaveCursors[pScr->myNum])
	mach64RecolorCursor(pScr, mach64SaveCursors[pScr->myNum], TRUE);
}

void
mach64RecolorCursor(pScr, pCurs, displayed)
     ScreenPtr pScr;
     CursorPtr pCurs;
     Bool      displayed;
{
    ColormapPtr	pmap;
    xColorItem	sourceColor;
    xColorItem	maskColor;
    VisualPtr   pVisual;
    int old_DAC_CNTL;

    if (!xf86VTSema) {
	miRecolorCursor(pScr, pCurs, displayed);
	return;
    }

    if (!displayed)
	return;

    mach64GetInstalledColormaps(pScr, &pmap);

    sourceColor.red = pCurs->foreRed;
    sourceColor.green = pCurs->foreGreen;
    sourceColor.blue = pCurs->foreBlue;

    maskColor.red = pCurs->backRed;
    maskColor.green = pCurs->backGreen;
    maskColor.blue = pCurs->backBlue;

    if (pScr->rootDepth > 8) 
    {
	maskColor.red >>= 8;
	maskColor.green >>= 8;
	maskColor.blue >>= 8;
	sourceColor.red >>= 8;
	sourceColor.green >>= 8;
	sourceColor.blue >>= 8;
	sourceColor.pixel = 0;
	maskColor.pixel = 0;
    } 
    else
    {
        FakeAllocColor(pmap, &sourceColor);
        FakeAllocColor(pmap, &maskColor);

        FakeFreeColor(pmap, sourceColor.pixel);
        FakeFreeColor(pmap, maskColor.pixel);

        maskColor.red = maskColor.red >> 8;
        maskColor.green = maskColor.green >> 8;
        maskColor.blue = maskColor.blue >> 8;

        sourceColor.red = sourceColor.red >> 8;
        sourceColor.green = sourceColor.green >> 8;
        sourceColor.blue = sourceColor.blue >> 8;
    }

    WaitIdleEmpty(); 

    if (mach64Ramdac == DAC_ATI68860_B || mach64Ramdac == DAC_ATI68860_C) {
	/* Access cursor color registers */
	outb(ioDAC_CNTL, 1);

	outb(ioDAC_REGS, 0);

	outb(ioDAC_REGS+1, maskColor.red);
	outb(ioDAC_REGS+1, maskColor.green);
	outb(ioDAC_REGS+1, maskColor.blue);

	outb(ioDAC_REGS+1, sourceColor.red);
	outb(ioDAC_REGS+1, sourceColor.green);
	outb(ioDAC_REGS+1, sourceColor.blue);

	/* Return DAC register set to palette registers */
	outb(ioDAC_CNTL, 0);
    } else if (mach64Ramdac == DAC_IBMRGB514) {
	mach64P_RGB514Index(0x40, maskColor.red);
	mach64P_RGB514Index(0x41, maskColor.green);
	mach64P_RGB514Index(0x42, maskColor.blue);

	mach64P_RGB514Index(0x43, sourceColor.red);
	mach64P_RGB514Index(0x44, sourceColor.green);
	mach64P_RGB514Index(0x45, sourceColor.blue);
    } else {
	regw(CUR_CLR0, ((maskColor.red << 24) | (maskColor.green << 16) |
			(maskColor.blue << 8) | maskColor.pixel));
	regw(CUR_CLR1, ((sourceColor.red << 24) | (sourceColor.green << 16) |
			(sourceColor.blue << 8) | sourceColor.pixel));
    }

    WaitIdleEmpty();
    checkCursorColor = FALSE;
}

void
mach64BlockHandler(i, blockData, pTimeout, pReadmask)
    int     i;
    pointer blockData;
    struct timeval **pTimeout;
    pointer pReadmask;
{
  if(checkCursorColor)
    mach64RenewCursorColor(screenInfo.screens[i]);
}

void
mach64WarpCursor(pScr, x, y)
     ScreenPtr pScr;
     int       x, y;
{
    miPointerWarpCursor(pScr, x, y);
    xf86Info.currentScreen = pScr;
}

void
mach64QueryBestSize(class, pwidth, pheight, pScr)
     int class;
     unsigned short *pwidth;
     unsigned short *pheight;
     ScreenPtr pScr;
{
    if (*pwidth > 0) {

	switch(class) {

	case CursorShape:
		if (*pwidth > 64)
		    *pwidth = 64;
		if (*pheight > 64)
		    *pheight = 64;
		break;
	default:
		mfbQueryBestSize(class, pwidth, pheight, pScr);
		break;
	}
    }
}

void
mach64CursorOff()
{
    if (mach64CursLastEnabled != -1) {
	WaitIdleEmpty();
	if (mach64RamdacSubType == DAC_IBMRGB514)
	    mach64P_RGB514Index(0x30, 0x00);
	else
	    regw(GEN_TEST_CNTL, regr(GEN_TEST_CNTL) & (~HWCURSOR_ENABLE));
	mach64CursLastEnabled = FALSE;
    }
}

void
mach64ClearSavedCursor(scr_index)
	int scr_index;
{
    mach64SaveCursors[scr_index] = NULL;
}

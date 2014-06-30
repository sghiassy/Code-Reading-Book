/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3BtCursor.c,v 3.14 1997/01/08 20:33:40 dawes Exp $ */
/*
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: s3BtCursor.c /main/8 1996/04/01 18:06:23 kaleb $ */

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
#include "xf86.h"
#include "inputstr.h"
#include "xf86Priv.h"
#include "xf86_Option.h"
#include "xf86_OSlib.h"
#include "s3.h"
#include "regs3.h"
#include "s3Bt485.h"

static unsigned short s3BtLowBits[] = { 0x3C8, 0x3C9, 0x3C6, 0x3C7 };
static unsigned char s3BtYPosMask = 0xFF;

#define MAX_CURS 64

extern Bool tmp_useSWCursor;

#ifndef __GNUC__
# define __inline__ /**/
#endif

/*
 * The Bt485 has 16 command registers.  The low-order two bits of the
 * register address follow normal VGA DAC addressing conventions (which 
 * for some reason aren't in numeric order, so we remap them through 
 * an array).  The S3 provides access to the two high-order bits via
 * the two low-order bits of CR55.  This is a fairly obnoxious way to
 * go about doing this kind of thing, but it's not all that common
 * an operation, so I don't feel too bad about wasting cycles.  We
 * can optimize it later, if need be.
 */
#if NeedFunctionPrototypes
void s3OutBtReg(unsigned short reg, unsigned char mask, unsigned char data)
#else
void s3OutBtReg(reg, mask, data)
unsigned short reg;
unsigned char mask;
unsigned char data;
#endif
{
   unsigned char tmp;
   unsigned char tmp1 = 0x00;

   /* High 2 bits of reg in CR55 bits 0-1 */
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | ((reg & 0x0C) >> 2));

   /* Have to map the low two bits to the correct DAC register */
   if (mask != 0x00)
      tmp1 = inb(s3BtLowBits[reg & 0x03]) & mask;
   outb(s3BtLowBits[reg & 0x03], tmp1 | data);
   
   /* Now clear 2 high-order bits so that other things work */
   outb(vgaCRReg, tmp);
}

#if NeedFunctionPrototypes
unsigned char s3InBtReg(unsigned short reg)
#else
unsigned char s3InBtReg(reg)
unsigned short reg;
#endif
{
   unsigned char tmp, ret;

   /* High 2 bits of reg in CR55 bits 0-1 */
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | ((reg & 0x0C) >> 2));

   /* Have to map the low two bits to the correct DAC register */
   ret = inb(s3BtLowBits[reg & 0x03]);

   /* Now clear 2 high-order bits so that other things work */
   outb(vgaCRReg, tmp);

   return(ret);
}

/*
 * The Command Register 3 register in the Bt485 must be accessed through
 * a very odd sequence, as the older RAMDACs had already defined 16
 * registers.  Apparently this overlays the Status register.
 */
#if NeedFunctionPrototypes
void s3OutBtRegCom3(unsigned char mask, unsigned char data)
#else
void s3OutBtRegCom3(mask, data)
unsigned char mask;
unsigned char data;
#endif
{
   s3OutBtReg(BT_COMMAND_REG_0, 0x7F, 0x80);
   s3OutBtReg(BT_WRITE_ADDR, 0x00, 0x01);
   s3OutBtReg(BT_STATUS_REG, mask, data);
}

unsigned char s3InBtRegCom3()
{
   unsigned char ret;

   s3OutBtReg(BT_COMMAND_REG_0, 0x7F, 0x80);
   s3OutBtReg(BT_WRITE_ADDR, 0x00, 0x01);
   ret = s3InBtReg(BT_STATUS_REG);
   return(ret);
}

/*
 * Status register may be hidden behind Command Register 3; need to check
 * both possibilities.
 */
unsigned char s3InBtStatReg()
{
   unsigned char tmp, ret;

   tmp = s3InBtReg(BT_COMMAND_REG_0);
   if ((tmp & 0x80) == 0x80) {
      /* Behind Command Register 3 */
      tmp = s3InBtReg(BT_WRITE_ADDR);
      s3OutBtReg(BT_WRITE_ADDR, 0x00, 0x00);
      ret = s3InBtReg(BT_STATUS_REG);
      s3OutBtReg(BT_WRITE_ADDR, 0x00, tmp);
   } else {
      ret = s3InBtReg(BT_STATUS_REG);
   }
   return(ret);
}

/*
 * These three functions partition the work so it can be done more
 * efficiently.
 */
#if NeedFunctionPrototypes
static __inline__ void s3StartBtData(
	unsigned short addr_reg,
	unsigned char addr,
	unsigned short data_reg)
#else
static __inline__ void s3StartBtData(addr_reg, addr, data_reg)
unsigned short addr_reg;
unsigned char addr;
unsigned short data_reg;
#endif
{
   unsigned char tmp;

   s3OutBtReg(addr_reg, 0x00, addr);

   /* High 2 bits of reg in CR55 bits 0-1 */
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | ((data_reg & 0x0C) >> 2));
}

#if NeedFunctionPrototypes
static __inline__ void s3OutBtData(
	unsigned short reg,
	unsigned char data)
#else
static __inline__ void s3OutBtData(reg, data)
unsigned short reg;
unsigned char data;
#endif
{
   /* Output data to RAMDAC */
   outb(s3BtLowBits[reg & 0x03], data);
}

#if NeedFunctionPrototypes
static __inline__ void s3EndBtData(void)
#else
static __inline__ void s3EndBtData()
#endif
{
   unsigned char tmp;

   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp);
}

/*
 * Convert the cursor from server-format to hardware-format.  The Bt485
 * has two planes, output sequentially.
 */
Bool
s3BtRealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   extern unsigned char s3SwapBits[256];
   register int i, j;
   unsigned char *pServMsk;
   unsigned char *pServSrc;
   int   indx2 = pScr->myNum;
   pointer *pPriv = &pCurs->bits->devPriv[indx2];
   int   wsrc, h;
   unsigned char *ram, *plane0, *plane1;
   CursorBitsPtr bits = pCurs->bits;

   if (bits->height > MAX_CURS || bits->width > MAX_CURS) {
      extern miPointerSpriteFuncRec miSpritePointerFuncs;
      return (miSpritePointerFuncs.RealizeCursor)(pScr, pCurs);
   }

   if (pCurs->bits->refcnt > 1)
      return TRUE;

   ram = (unsigned char *)xalloc(1024);
   *pPriv = (pointer) ram;
   plane0 = ram;
   plane1 = ram+512;

   if (!ram)
      return FALSE;

   pServSrc = (unsigned char *)bits->source;
   pServMsk = (unsigned char *)bits->mask;

   h = bits->height;
   if (h > MAX_CURS)
      h = MAX_CURS;

   wsrc = PixmapBytePad(bits->width, 1);	/* bytes per line */

   for (i = 0; i < MAX_CURS; i++) {
      for (j = 0; j < MAX_CURS / 8; j++) {
	 unsigned char mask, source;

	 if (i < h && j < wsrc) {
	    source = *pServSrc++;
	    mask = *pServMsk++;

	    source = s3SwapBits[source];
	    mask = s3SwapBits[mask];

	    if (j < MAX_CURS / 8) {
	       *plane0++ = source & mask;
	       *plane1++ = mask;
	    }
	 } else {
	    *plane0++ = 0x00;
	    *plane1++ = 0x00;
	 }
      }
      /*
       * if we still have more bytes on this line (j < wsrc),
       * we have to ignore the rest of the line.
       */
       while (j++ < wsrc) pServMsk++,pServSrc++;
   }
   return TRUE;
}

void 
s3BtCursorOn()
{
   unsigned char tmp;

   UNLOCK_SYS_REGS;

   /* turn on external cursor */
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xDF;
   outb(vgaCRReg, tmp | 0x20);

   /* Enable Bt485 */
   if(!S3_x64_SERIES(s3ChipId)) {
     outb(vgaCRIndex, 0x45);
     tmp = inb(vgaCRReg) & 0xDF;
     outb(vgaCRReg, tmp | 0x20);
   }
   
   /* Enable cursor mode 3 - X11 mode */
   s3OutBtReg(BT_COMMAND_REG_2, 0xFC, 0x03);

   LOCK_SYS_REGS;
   return;
}

void
s3BtCursorOff()
{
   UNLOCK_SYS_REGS;

   /*
    * Don't need to undo the S3 registers here; they will be undone when
    * the mode is restore from save registers.  If it is done here, it
    * causes the cursor to flash each time it is loaded, so don't do that.
    */

   /* Disable cursor */
   s3OutBtReg(BT_COMMAND_REG_2, 0xFC, 0x00);

   LOCK_SYS_REGS;
   return;
}

void
s3BtMoveCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
   extern int s3hotX, s3hotY;
   extern Bool s3PixelMultiplexing;
   extern int s3AdjustCursorXPos;

   if (!xf86VTSema)
      return;

   if (tmp_useSWCursor) {
      extern miPointerSpriteFuncRec miSpritePointerFuncs;
      (miSpritePointerFuncs.MoveCursor)(pScr, x, y);
      return;
   }

   if (s3BlockCursor)
      return;
   
   x -= s3InfoRec.frameX0 - s3AdjustCursorXPos;
   x += 64;
   x -= s3hotX;
   /* Compensate for using Bt485 Cursor without pixel multiplexing. */
   if ((OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options) ||
	OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options)) &&
       !s3PixelMultiplexing)
     x -= 2;

   if (x < 0)
      return;

   y -= s3InfoRec.frameY0;

   if (s3InfoRec.modes->Flags & V_DBLSCAN)
      y <<= 1;

   y += 64;
   y -= s3hotY;
   if (y < 0)
      return;


   UNLOCK_SYS_REGS;

   /* Output position - "only" 12 bits of location documented */
   s3OutBtReg(BT_CURS_X_LOW, 0x00, x & 0xFF);
   s3OutBtReg(BT_CURS_X_HIGH, 0x00, (x >> 8) & 0x0F);
   s3OutBtReg(BT_CURS_Y_LOW, 0x00, y & s3BtYPosMask);
   s3OutBtReg(BT_CURS_Y_HIGH, 0x00, (y >> 8) & 0x0F);

   LOCK_SYS_REGS;
   return;
}

void
s3BtRecolorCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   extern Bool s3DAC8Bit;

   UNLOCK_SYS_REGS;

   /* Start writing at address 1 (0 is overscan color) */
   s3StartBtData(BT_CURS_WR_ADDR, 0x01, BT_CURS_DATA);

   /* Background color */
   if (s3DAC8Bit) {
      s3OutBtData(BT_CURS_DATA, (pCurs->backRed >> 8) & 0xFF);
      s3OutBtData(BT_CURS_DATA, (pCurs->backGreen >> 8) & 0xFF);
      s3OutBtData(BT_CURS_DATA, (pCurs->backBlue >> 8) & 0xFF);
   } else {
      s3OutBtData(BT_CURS_DATA, (pCurs->backRed >> 10) & 0xFF);
      s3OutBtData(BT_CURS_DATA, (pCurs->backGreen >> 10) & 0xFF);
      s3OutBtData(BT_CURS_DATA, (pCurs->backBlue >> 10) & 0xFF);
   }

   /* Foreground color */
   if (s3DAC8Bit) {
      s3OutBtData(BT_CURS_DATA, (pCurs->foreRed >> 8) & 0xFF);
      s3OutBtData(BT_CURS_DATA, (pCurs->foreGreen >> 8) & 0xFF);
      s3OutBtData(BT_CURS_DATA, (pCurs->foreBlue >> 8) & 0xFF);
   } else {
      s3OutBtData(BT_CURS_DATA, (pCurs->foreRed >> 10) & 0xFF);
      s3OutBtData(BT_CURS_DATA, (pCurs->foreGreen >> 10) & 0xFF);
      s3OutBtData(BT_CURS_DATA, (pCurs->foreBlue >> 10) & 0xFF);
   }

   /* Now clean up */
   s3EndBtData();

   LOCK_SYS_REGS;
   return;
}

void 
s3BtLoadCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int x, y;
{
   int   indx2 = pScr->myNum;
   register int   i, j;
   unsigned char *ram, *p, tmpcurs;
   extern int s3InitCursorFlag;

   if (!xf86VTSema)
      return;

   if (!pCurs)
      return;

   /* turn the cursor off */
   if ((tmpcurs = s3InBtReg(BT_COMMAND_REG_2)) & 0x03)
      s3BtCursorOff();

   /* load colormap */
   s3BtRecolorCursor(pScr, pCurs);

   ram = (unsigned char *)pCurs->bits->devPriv[indx2];

   UNLOCK_SYS_REGS;
   BLOCK_CURSOR;

   /* 
    * Bit 2 == 1 ==> 64x64 cursor; 2 low-order bits are extensions to the
    * address register, and must be cleared since we're going to start
    * writing data at address 0.
    */
   s3OutBtRegCom3(0xF8, 0x04);

   if (s3InfoRec.modes->Flags & V_INTERLACE) {
      s3StartBtData(BT_WRITE_ADDR, 0x00, BT_CURS_RAM_DATA);

      for (i=0; i < 64; i++) {		/* 64 rows in cursor */
	 p = ram + (((i % 2) ? i-1 : i+1)*8);
	 for (j=0; j < 8; j++,p++) {	/* 8 bytes per row */
            s3OutBtData(BT_CURS_RAM_DATA, *p);
	 }
      }
      ram += 512;
      for (i=0; i < 64; i++) {		/* 64 rows in cursor */
	 p = ram + (((i % 2) ? i-1 : i+1)*8);
	 for (j=0; j < 8; j++,p++) {	/* 8 bytes per row */
            s3OutBtData(BT_CURS_RAM_DATA, *p);
	 }
      }

      s3EndBtData();

      s3BtYPosMask = 0xFE;
      s3OutBtReg(BT_COMMAND_REG_2, 0xF7, 0x08);
   } else if (s3InfoRec.modes->Flags & V_DBLSCAN) {
      s3StartBtData(BT_WRITE_ADDR, 0x00, BT_CURS_RAM_DATA);

      for (i=0; i < 32; i++) {		/* 64 rows in cursor */
	 p = ram + i * 8;
	 for (j=0; j < 8; j++,p++) {	/* 8 bytes per row */
            s3OutBtData(BT_CURS_RAM_DATA, *p);
	 }
	 p = ram + i * 8;
	 for (j=0; j < 8; j++,p++) {	/* 8 bytes per row */
            s3OutBtData(BT_CURS_RAM_DATA, *p);
	 }
      }
      ram += 512;
      for (i=0; i < 32; i++) {		/* 64 rows in cursor */
	 p = ram + i * 8;
	 for (j=0; j < 8; j++,p++) {	/* 8 bytes per row */
            s3OutBtData(BT_CURS_RAM_DATA, *p);
	 }
	 p = ram + i * 8;
	 for (j=0; j < 8; j++,p++) {	/* 8 bytes per row */
            s3OutBtData(BT_CURS_RAM_DATA, *p);
	 }
      }

      s3EndBtData();

      s3BtYPosMask = 0xFF;
      s3OutBtReg(BT_COMMAND_REG_2, 0xF7, 0x00);
   } else {
      /* Start data output */
      s3StartBtData(BT_WRITE_ADDR, 0x00, BT_CURS_RAM_DATA);

      /* 
       * Output the cursor data.  The realize function has put the planes into
       * their correct order, so we can just blast this out.
       */
      p = ram;
      for (i = 0; i < 1024; i++,p++)
         s3OutBtData(BT_CURS_RAM_DATA, *p);

      s3EndBtData();

      s3BtYPosMask = 0xFF;
      s3OutBtReg(BT_COMMAND_REG_2, 0xF7, 0x00);
   }

   UNBLOCK_CURSOR;
   LOCK_SYS_REGS;

   /* position cursor */
   s3BtMoveCursor(0, x, y);

   /* turn the cursor on */
   if ((tmpcurs & 0x03) || s3InitCursorFlag)
      s3BtCursorOn();

   if (s3InitCursorFlag)
      s3InitCursorFlag = FALSE;

   return;
}

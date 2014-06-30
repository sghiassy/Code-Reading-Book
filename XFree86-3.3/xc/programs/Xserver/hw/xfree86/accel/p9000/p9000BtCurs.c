/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000BtCurs.c,v 3.9 1996/12/23 06:40:34 dawes Exp $ */
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
 * Modified by Erik Nygren (nygren@mit.edu) for use with P9000
 *
 */
/* $XConsortium: p9000BtCurs.c /main/5 1996/03/09 11:17:21 kaleb $ */

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
#include "xf86_OSlib.h"
#include "p9000.h"
#include "p9000reg.h"
#include "p9000Bt485.h"
#include "p9000curs.h"

static unsigned char p9000BtYPosMask = 0xFF;


/*
 * These three functions partition the work so it can be done more
 * efficiently.
 */
static __inline__ void p9000StartBtData(addr_reg, addr, data_reg)
unsigned short addr_reg;
unsigned char addr;
unsigned short data_reg;
{
   unsigned char tmp;

   p9000OutBtReg(addr_reg, 0x00, addr);
}

static __inline__ void p9000OutBtData(reg, data)
unsigned short reg;
unsigned char data;
{
  /* Output data to RAMDAC */
  if ((p9000VendorPtr->Label == P9000_VENDOR_VIPERPCI)
      && (0xf000 & reg))
    {
      reg = ((unsigned long)((reg & 0x00ff) | ((reg & 0xf000)>>4)))
	+ (unsigned long)p9000InfoRec.IObase;
    }
   outb(reg, data);
}

static __inline__ void p9000EndBtData()
{
}

/*
 * Convert the cursor from server-format to hardware-format.  The Bt485
 * has two planes, output sequentially.
 */
Bool
p9000BtRealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   extern unsigned char p9000SwapBits[256];
   register int i, j;
   unsigned char *pServMsk;
   unsigned char *pServSrc;
   int   index = pScr->myNum;
   pointer *pPriv = &pCurs->bits->devPriv[index];
   int   wsrc, h;
   unsigned char *ram, *plane0, *plane1;
   CursorBitsPtr bits = pCurs->bits;

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

#define MAX_CURS 64

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

	    source = p9000SwapBits[source];
	    mask = p9000SwapBits[mask];

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
p9000BtCursorOn()
{
   /* Enable cursor mode 3 - X11 mode */
  if (xf86VTSema)
    p9000OutBtReg(BT_COMMAND_REG_2, 0xFC, 0x03);
  return;
}

void
p9000BtCursorOff()
{
   /* Disable cursor */
  if (xf86VTSema)
    p9000OutBtReg(BT_COMMAND_REG_2, 0xFC, 0x00);
  return;
}

void
p9000BtMoveCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
   extern int p9000hotX, p9000hotY;

  if (!xf86VTSema) return;

   if (p9000BlockCursor)
      return;
   
   x -= p9000InfoRec.frameX0;
   x += 64;
   x -= p9000hotX;
   if (x < 0)
      return;

   y -= p9000InfoRec.frameY0;
   y += 64;
   y -= p9000hotY;
   if (y < 0)
      return;

   /* Output position - "only" 12 bits of location documented */
   p9000OutBtReg(BT_CURS_X_LOW, 0x00, x & 0xFF);
   p9000OutBtReg(BT_CURS_X_HIGH, 0x00, (x >> 8) & 0x0F);
   p9000OutBtReg(BT_CURS_Y_LOW, 0x00, y & p9000BtYPosMask);
   p9000OutBtReg(BT_CURS_Y_HIGH, 0x00, (y >> 8) & 0x0F);

   return;
}

void
p9000BtRecolorCursor(pScr, pCurs, displayed)
     ScreenPtr pScr;
     CursorPtr pCurs;
     Bool displayed;
{
   extern Bool p9000DAC8Bit;

   if (!xf86VTSema) {
      miRecolorCursor(pScr, pCurs, displayed);
      return;
   }

   if (!displayed)
      return;

   /* Start writing at address 1 (0 is overscan color) */
   p9000StartBtData(BT_CURS_WR_ADDR, 0x01, BT_CURS_DATA);

   /* Background color */
   if (p9000DAC8Bit) {
      p9000OutBtData(BT_CURS_DATA, (pCurs->backRed >> 8) & 0xFF);
      p9000OutBtData(BT_CURS_DATA, (pCurs->backGreen >> 8) & 0xFF);
      p9000OutBtData(BT_CURS_DATA, (pCurs->backBlue >> 8) & 0xFF);
   } else {
      p9000OutBtData(BT_CURS_DATA, (pCurs->backRed >> 10) & 0xFF);
      p9000OutBtData(BT_CURS_DATA, (pCurs->backGreen >> 10) & 0xFF);
      p9000OutBtData(BT_CURS_DATA, (pCurs->backBlue >> 10) & 0xFF);
   }

   /* Foreground color */
   if (p9000DAC8Bit) {
      p9000OutBtData(BT_CURS_DATA, (pCurs->foreRed >> 8) & 0xFF);
      p9000OutBtData(BT_CURS_DATA, (pCurs->foreGreen >> 8) & 0xFF);
      p9000OutBtData(BT_CURS_DATA, (pCurs->foreBlue >> 8) & 0xFF);
   } else {
      p9000OutBtData(BT_CURS_DATA, (pCurs->foreRed >> 10) & 0xFF);
      p9000OutBtData(BT_CURS_DATA, (pCurs->foreGreen >> 10) & 0xFF);
      p9000OutBtData(BT_CURS_DATA, (pCurs->foreBlue >> 10) & 0xFF);
   }

   /* Now clean up */
   p9000EndBtData();

   return;
}

void 
p9000BtLoadCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int x, y;
{
   int   index = pScr->myNum;
   register int   i, j;
   unsigned char *ram, *p, tmpcurs;
   extern int p9000InitCursorFlag;

   if (!xf86VTSema)
      return;

   if (!pCurs)
      return;

   /* turn the cursor off */
   if ((tmpcurs = p9000InBtReg(BT_COMMAND_REG_2)) & 0x03)
      p9000BtCursorOff();

   /* load colormap */
   p9000BtRecolorCursor(pScr, pCurs, TRUE);

   ram = (unsigned char *)pCurs->bits->devPriv[index];

   BLOCK_CURSOR;

   /* 
    * Bit 2 == 1 ==> 64x64 cursor; 2 low-order bits are extensions to the
    * address register, and must be cleared since we're going to start
    * writing data at address 0.
    */
   p9000OutBtRegCom3(0xF8, 0x04);

   if (p9000InfoRec.modes->Flags & V_INTERLACE) {
      p9000StartBtData(BT_WRITE_ADDR, 0x00, BT_CURS_RAM_DATA);

      for (i=0; i < 64; i++) {		/* 64 rows in cursor */
	 p = ram + (((i % 2) ? i-1 : i+1)*8);
	 for (j=0; j < 8; j++,p++) {	/* 8 bytes per row */
            p9000OutBtData(BT_CURS_RAM_DATA, *p);
	 }
      }
      ram += 512;
      for (i=0; i < 64; i++) {		/* 64 rows in cursor */
	 p = ram + (((i % 2) ? i-1 : i+1)*8);
	 for (j=0; j < 8; j++,p++) {	/* 8 bytes per row */
            p9000OutBtData(BT_CURS_RAM_DATA, *p);
	 }
      }

      p9000EndBtData();

      p9000BtYPosMask = 0xFE;
      p9000OutBtReg(BT_COMMAND_REG_2, 0xF7, 0x08);
   } else {
      /* Start data output */
      p9000StartBtData(BT_WRITE_ADDR, 0x00, BT_CURS_RAM_DATA);

      /* 
       * Output the cursor data.  The realize function has put the planes into
       * their correct order, so we can just blast this out.
       */
      p = ram;
      for (i = 0; i < 1024; i++,p++)
         p9000OutBtData(BT_CURS_RAM_DATA, *p);

      p9000EndBtData();

      p9000BtYPosMask = 0xFF;
      p9000OutBtReg(BT_COMMAND_REG_2, 0xF7, 0x00);
   }

   UNBLOCK_CURSOR;

   /* position cursor */
   p9000BtMoveCursor(0, x, y);

   /* turn the cursor on */
   if ((tmpcurs & 0x03) || p9000InitCursorFlag)
      p9000BtCursorOn();

   if (p9000InitCursorFlag)
      p9000InitCursorFlag = FALSE;

   return;
}

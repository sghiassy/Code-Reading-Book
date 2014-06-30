/* $XConsortium: tgaBtCurs.c /main/4 1996/10/27 11:47:33 kaleb $ */
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
 * Modified by Alan Hourihane (alanh@fairlite.demon.co.uk) for use with TGA
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/tga/tgaBtCurs.c,v 3.3 1996/12/27 07:03:44 dawes Exp $ */

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
#include "tga.h"
#include "tga_regs.h"
#include "tga_presets.h"
#include "tgacurs.h"

static unsigned char tgaBtYPosMask = 0xFF;
static unsigned char oldBt485;

/*
 * Convert the cursor from server-format to hardware-format.  The Bt485
 * has two planes, output sequentially.
 */
Bool
tgaBtRealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   extern unsigned char tgaSwapBits[256];
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

	    source = tgaSwapBits[source];
	    mask = tgaSwapBits[mask];

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
tgaBtCursorOn()
{
   /* Enable cursor mode 3 - X11 mode */
  if (xf86VTSema)
#if 0
    tgaOutBtReg(BT_COMMAND_REG_2, 0xFC, 0x03);
#endif
  {
#if 0
    TGA_WRITE_REG(BT485_CMD_2, TGA_RAMDAC_SETUP_REG);
    oldBt485 = TGA_READ_REG(TGA_RAMDAC_REG) & 0xFC;
    BT485_WRITE(oldBt485|0x03, BT485_CMD_2);
#endif
    BT485_WRITE(0x23, BT485_CMD_2);
  }
  return;
}

void
tgaBtCursorOff()
{
   /* Disable cursor */
  if (xf86VTSema)
#if 0
    tgaOutBtReg(BT_COMMAND_REG_2, 0xFC, 0x00);
#endif
  {
#if 0
    TGA_WRITE_REG(BT485_CMD_2, TGA_RAMDAC_SETUP_REG);
    oldBt485 = TGA_READ_REG(TGA_RAMDAC_REG) & 0xFC;
    BT485_WRITE(oldBt485|0x00, BT485_CMD_2);
#endif
    BT485_WRITE(0x20, BT485_CMD_2);
  }
  return;
}

void
tgaBtMoveCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
   extern int tgahotX, tgahotY;

  if (!xf86VTSema) return;

   if (tgaBlockCursor)
      return;
   
   x -= tgaInfoRec.frameX0;
   x += 64;
   x -= tgahotX;
   if (x < 0)
      return;

   y -= tgaInfoRec.frameY0;
   y += 64;
   y -= tgahotY;
   if (y < 0)
      return;

   /* Output position - "only" 12 bits of location documented */
#if 0
   tgaOutBtReg(BT_CURS_X_LOW, 0x00, x & 0xFF);
   tgaOutBtReg(BT_CURS_X_HIGH, 0x00, (x >> 8) & 0x0F);
   tgaOutBtReg(BT_CURS_Y_LOW, 0x00, y & tgaBtYPosMask);
   tgaOutBtReg(BT_CURS_Y_HIGH, 0x00, (y >> 8) & 0x0F);
#endif
   BT485_WRITE(x & 0xFF, BT485_CUR_LOW_X);
   BT485_WRITE((x >> 8) & 0x0F, BT485_CUR_HIGH_X);
   BT485_WRITE(y & tgaBtYPosMask, BT485_CUR_LOW_Y);
   BT485_WRITE((y >> 8) & 0x0F, BT485_CUR_HIGH_Y);

   return;
}

void
tgaBtRecolorCursor(pScr, pCurs, displayed)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   extern Bool tgaDAC8Bit;

   if (!xf86VTSema) {
	miRecolorCursor(pScr, pCurs, displayed);
	return;
   }

   if (!displayed)
	return;

   /* Start writing at address 1 (0 is overscan color) */
#if 0
   tgaStartBtData(BT_CURS_WR_ADDR, 0x01, BT_CURS_DATA);
#endif
   BT485_WRITE(0x00, BT485_ADDR_CUR_WRITE);

   /* Background color */
   if (tgaDAC8Bit) {
#if 0
      tgaOutBtData(BT_CURS_DATA, (pCurs->backRed >> 8) & 0xFF);
      tgaOutBtData(BT_CURS_DATA, (pCurs->backGreen >> 8) & 0xFF);
      tgaOutBtData(BT_CURS_DATA, (pCurs->backBlue >> 8) & 0xFF);
#endif
      BT485_WRITE((pCurs->backRed >> 8) & 0xFF, BT485_DATA_CUR);
      BT485_WRITE((pCurs->backGreen >> 8) & 0xFF, BT485_DATA_CUR);
      BT485_WRITE((pCurs->backBlue >> 8) & 0xFF, BT485_DATA_CUR);
   } else {
#if 0
      tgaOutBtData(BT_CURS_DATA, (pCurs->backRed >> 10) & 0xFF);
      tgaOutBtData(BT_CURS_DATA, (pCurs->backGreen >> 10) & 0xFF);
      tgaOutBtData(BT_CURS_DATA, (pCurs->backBlue >> 10) & 0xFF);
#endif
      BT485_WRITE((pCurs->backRed >> 10) & 0xFF, BT485_DATA_CUR);
      BT485_WRITE((pCurs->backGreen >> 10) & 0xFF, BT485_DATA_CUR);
      BT485_WRITE((pCurs->backBlue >> 10) & 0xFF, BT485_DATA_CUR);
   }

   /* Foreground color */
   if (tgaDAC8Bit) {
#if 0
      tgaOutBtData(BT_CURS_DATA, (pCurs->foreRed >> 8) & 0xFF);
      tgaOutBtData(BT_CURS_DATA, (pCurs->foreGreen >> 8) & 0xFF);
      tgaOutBtData(BT_CURS_DATA, (pCurs->foreBlue >> 8) & 0xFF);
#endif
      BT485_WRITE((pCurs->foreRed >> 8) & 0xFF, BT485_DATA_CUR);
      BT485_WRITE((pCurs->foreGreen >> 8) & 0xFF, BT485_DATA_CUR);
      BT485_WRITE((pCurs->foreBlue >> 8) & 0xFF, BT485_DATA_CUR);
   } else {
#if 0
      tgaOutBtData(BT_CURS_DATA, (pCurs->foreRed >> 10) & 0xFF);
      tgaOutBtData(BT_CURS_DATA, (pCurs->foreGreen >> 10) & 0xFF);
      tgaOutBtData(BT_CURS_DATA, (pCurs->foreBlue >> 10) & 0xFF);
#endif
      BT485_WRITE((pCurs->foreRed >> 10) & 0xFF, BT485_DATA_CUR);
      BT485_WRITE((pCurs->foreGreen >> 10) & 0xFF, BT485_DATA_CUR);
      BT485_WRITE((pCurs->foreBlue >> 10) & 0xFF, BT485_DATA_CUR);
   }
   return;
}

void 
tgaBtLoadCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int x, y;
{
   int   index = pScr->myNum;
   register int   i, j;
   unsigned char *ram, *p, tmpcurs;
   extern int tgaInitCursorFlag;

   if (!xf86VTSema)
      return;

   if (!pCurs)
      return;

   /* turn the cursor off */
#if 0
   if ((tmpcurs = tgaInBtReg(BT_COMMAND_REG_2)) & 0x03)
      tgaBtCursorOff();
#endif
#if 0
   TGA_WRITE_REG(BT485_CMD_2, TGA_RAMDAC_SETUP_REG);
   oldBt485 = TGA_READ_REG(TGA_RAMDAC_REG);
   if ((tmpcurs = oldBt485) & 0x03)
#endif
      tgaBtCursorOff();

   /* load colormap */
   tgaBtRecolorCursor(pScr, pCurs, TRUE);

   ram = (unsigned char *)pCurs->bits->devPriv[index];

   BLOCK_CURSOR;

   /* 
    * Bit 2 == 1 ==> 64x64 cursor; 2 low-order bits are extensions to the
    * address register, and must be cleared since we're going to start
    * writing data at address 0.
    */
#if 0
   tgaOutBtRegCom3(0xF8, 0x04);
#endif
#if 0
   TGA_WRITE_REG(BT485_CMD_3, TGA_RAMDAC_SETUP_REG);
   oldBt485 = TGA_READ_REG(TGA_RAMDAC_REG) & 0xF8;
   BT485_WRITE(oldBt485|0x04, BT485_CMD_3);
#endif
   BT485_WRITE(0x14, BT485_CMD_3);

   if (tgaInfoRec.modes->Flags & V_INTERLACE) {
#if 0
      tgaStartBtData(BT_WRITE_ADDR, 0x00, BT_CURS_RAM_DATA);
#endif
      BT485_WRITE(0x00, BT485_ADDR_PAL_WRITE);

      for (i=0; i < 64; i++) {		/* 64 rows in cursor */
	 p = ram + (((i % 2) ? i-1 : i+1)*8);
	 for (j=0; j < 8; j++,p++) {	/* 8 bytes per row */
#if 0
            tgaOutBtData(BT_CURS_RAM_DATA, *p);
#endif
	    BT485_WRITE(*p, BT485_CUR_RAM);
	 }
      }
      ram += 512;
      for (i=0; i < 64; i++) {		/* 64 rows in cursor */
	 p = ram + (((i % 2) ? i-1 : i+1)*8);
	 for (j=0; j < 8; j++,p++) {	/* 8 bytes per row */
#if 0
            tgaOutBtData(BT_CURS_RAM_DATA, *p);
#endif
	    BT485_WRITE(*p, BT485_CUR_RAM);
	 }
      }

      tgaBtYPosMask = 0xFE;
#if 0
      tgaOutBtReg(BT_COMMAND_REG_2, 0xF7, 0x08);
#endif
#if 0
      TGA_WRITE_REG(BT485_CMD_2, TGA_RAMDAC_SETUP_REG);
      oldBt485 = TGA_READ_REG(TGA_RAMDAC_REG) & 0xF7;
      BT485_WRITE(oldBt485|0x08, BT485_CMD_2);
#endif
   } else {
      /* Start data output */
#if 0
      tgaStartBtData(BT_WRITE_ADDR, 0x00, BT_CURS_RAM_DATA);
#endif
      BT485_WRITE(0x00, BT485_ADDR_PAL_WRITE);

      /* 
       * Output the cursor data.  The realize function has put the planes into
       * their correct order, so we can just blast this out.
       */
      p = ram;
      for (i = 0; i < 1024; i++,p++)
#if 0
         tgaOutBtData(BT_CURS_RAM_DATA, *p);
#endif
         BT485_WRITE(*p, BT485_CUR_RAM);

      tgaBtYPosMask = 0xFF;
#if 0
      tgaOutBtReg(BT_COMMAND_REG_2, 0xF7, 0x00);
#endif
#if 0
      TGA_WRITE_REG(BT485_CMD_2, TGA_RAMDAC_SETUP_REG);
      oldBt485 = TGA_READ_REG(TGA_RAMDAC_REG) & 0xF7;
      BT485_WRITE(oldBt485|0x00, BT485_CMD_2);
#endif
   }

   UNBLOCK_CURSOR;

   /* position cursor */
   tgaBtMoveCursor(0, x, y);

#if 0
   /* turn the cursor on */
   if ((tmpcurs & 0x03) || tgaInitCursorFlag)
#endif
      tgaBtCursorOn();

   if (tgaInitCursorFlag)
      tgaInitCursorFlag = FALSE;

   return;
}

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/Bt485.c,v 3.6 1996/12/23 06:32:20 dawes Exp $ */
/*
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 * Copyright 1994 by Henry A. Worth  <haw30@eng.amdahl.com>
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
 * DAVID WEXELBLAT AND HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS 
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR 
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the AGX server and to increase portability by Henry A. Worth 
 *
 */
/* $XConsortium: Bt485.c /main/5 1996/10/19 17:49:27 kaleb $ */

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
#include "xf86RamDac.h"
#include "Bt485.h"

static unsigned char xf86BtYPosMask = 0xFF;

#ifndef __GNUC__
# define __inline__ /**/
#endif

/*
 * The indirect registers in the Bt485, like "Command Reg 3", 
 * must be accessed by setting a bit CR0, placing the indirect
 * registers address into the the write address register, and
 * then reading/writeing the status register.
 */
#ifdef __STDC__
void xf86OutBt485IndReg( unsigned char reg,
                         unsigned char mask, 
                         unsigned char data )
#else
void xf86OutBt485IndReg( reg, mask, data )
   unsigned char reg;
   unsigned char mask;
   unsigned char data;
#endif
{
   xf86OutRamDacReg(BT485_COMMAND_REG_0, 0x7F, 0x80);
   xf86OutRamDacData(BT485_WRITE_ADDR, reg);
   xf86OutRamDacReg(BT485_STATUS_REG, mask, data);
   xf86OutRamDacReg(BT485_COMMAND_REG_0, 0x7F, 0x00);
}

#ifdef __STDC__
unsigned char
xf86InBt485IndReg( unsigned char reg )
#else
unsigned char
xf86InBt485IndReg( reg )
   unsigned char reg;
#endif
{
   unsigned char ret;

   xf86OutRamDacReg(BT485_COMMAND_REG_0, 0x7F, 0x80);
   xf86OutRamDacData(BT485_WRITE_ADDR, reg);
   ret = xf86InRamDacReg(BT485_STATUS_REG);
   xf86OutRamDacReg(BT485_COMMAND_REG_0, 0x7F, 0x00);
   return(ret);
}

/*
 * Status register may be hidden behind Command Register 3; need to check
 * both possibilities. (is this really needed now?)
 */
unsigned char xf86InBt485StatReg()
{
   unsigned char tmp, ret;

   tmp = xf86InRamDacReg(BT485_COMMAND_REG_0);
   if ((tmp & 0x80) == 0x80) {
      /* Behind Command Register 3 */
      tmp = xf86InRamDacReg(BT485_WRITE_ADDR);
      xf86OutRamDacReg(BT485_WRITE_ADDR, 0x00, 0x00);
      ret = xf86InRamDacReg(BT485_STATUS_REG);
      xf86OutRamDacReg(BT485_WRITE_ADDR, 0x00, tmp);
   } else {
      ret = xf86InRamDacReg(BT485_STATUS_REG);
   }
   return(ret);
}

#if 0
/*
 * Convert the cursor from server-format to hardware-format.  The Bt485
 * has two planes, output sequentially.
 */
Bool
xf86Bt485RealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   extern unsigned char xf86SwapBits[256];
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

   h = bits->height;
   if (h > xf86MaxCurs)
      h = xf86MaxCurs;

   wsrc = PixmapBytePad(bits->width, 1);	/* bytes per line */

   for (i = 0; i < xf86MaxCurs; i++) {
      for (j = 0; j < xf86MaxCurs / 8; j++) {
	 unsigned char mask, source;

	 if (i < h && j < wsrc) {
	    source = *pServSrc++;
	    mask = *pServMsk++;

	    source = xf86SwapBits[source];
	    mask = xf86SwapBits[mask];

	    if (j < xf86MaxCurs / 8) {
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
xf86Bt485CursorOn()
{
   /* Enable cursor mode 3 - X11 mode */
   xf86OutRamDacReg(BT485_COMMAND_REG_2, 0xFC, 0x03);

   return;
}

void
xf86Bt485CursorOff()
{
   /* Disable cursor */
   xf86OutRamDacReg(BT485_COMMAND_REG_2, 0xFC, 0x00);

   return;
}

void
xf86Bt485MoveCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
   extern int xf86hotX, xf86hotY;

   if (!xf86VTSema)
      return;
   
   x -= xf86FrameX0;
   x += 64;
   x -= xf86hotX;
   if (x < 0)
      return;

   y -= xf86FrameY0;
   y += 64;
   y -= xf86hotY;
   if (y < 0)
      return;

   /* Output position - "only" 12 bits of location documented */
   xf86OutRamDacReg(BT485_CURS_X_LOW, 0x00, x & 0xFF);
   xf86OutRamDacReg(BT485_CURS_X_HIGH, 0x00, (x >> 8) & 0x0F);
   xf86OutRamDacReg(BT485_CURS_Y_LOW, 0x00, y & xf86BtYPosMask);
   xf86OutRamDacReg(BT485_CURS_Y_HIGH, 0x00, (y >> 8) & 0x0F);

   return;
}

void
xf86Bt485RecolorCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   /* Start writing at address 1 (0 is overscan color) */
   xf86OutRamDacData(BT485_CURS_WR_ADDR, 0x01);

   /* Background color */
   xf86OutRamDacData(BT485_CURS_DATA, (pCurs->backRed >> 8) & 0xFF);
   xf86OutRamDacData(BT485_CURS_DATA, (pCurs->backGreen >> 8) & 0xFF);
   xf86OutRamDacData(BT485_CURS_DATA, (pCurs->backBlue >> 8) & 0xFF);

   /* Foreground color */
   xf86OutRamDacData(BT485_CURS_DATA, (pCurs->foreRed >> 8) & 0xFF);
   xf86OutRamDacData(BT485_CURS_DATA, (pCurs->foreGreen >> 8) & 0xFF);
   xf86OutRamDacData(BT485_CURS_DATA, (pCurs->foreBlue >> 8) & 0xFF);

   return;
}

void 
xf86Bt485LoadCursor(pScr, pCurs, modesFlags, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int modesFlags;
     int x, y;
{
   int   index = pScr->myNum;
   register int   i, j;
   unsigned char *ram, *p;

   if (!xf86VTSema)
      return;

   if (!pCurs)
      return;

   /* turn the cursor off */
   xf86Bt485CursorOff();

   /* load colormap */
   xf86Bt485RecolorCursor(pScr, pCurs);

   ram = (unsigned char *)pCurs->bits->devPriv[index];

   /* 
    * Bit 2 == 1 ==> 64x64 cursor; 2 low-order bits are extensions to the
    * address register, and must be cleared since we're going to start
    * writing data at address 0.
    */
   xf86OutBt485IndReg(BT485_COMMAND_REG_3, 0xF8, 0x04);

   if (modesFlags & V_INTERLACE) {
      xf86OutRamDacReg(BT485_WRITE_ADDR, 0x00, 0x00);

      for (i=0; i < 64; i++) {		/* 64 rows in cursor */
	 p = ram + (((i % 2) ? i-1 : i+1)*8);
	 for (j=0; j < 8; j++,p++) {	/* 8 bytes per row */
            xf86OutRamDacData(BT485_CURS_RAM_DATA, *p);
	 }
      }
      ram += 512;
      for (i=0; i < 64; i++) {		/* 64 rows in cursor */
	 p = ram + (((i % 2) ? i-1 : i+1)*8);
	 for (j=0; j < 8; j++,p++) {	/* 8 bytes per row */
            xf86OutRamDacData(BT485_CURS_RAM_DATA, *p);
	 }
      }

      xf86BtYPosMask = 0xFE;
      xf86OutRamDacReg(BT485_COMMAND_REG_2, 0xF7, 0x08);
   } else {
      /* Start data output */
      xf86OutRamDacReg(BT485_WRITE_ADDR, 0x00, 0x00);

      /* 
       * Output the cursor data.  The realize function has put the planes into
       * their correct order, so we can just blast this out.
       */
      p = ram;
      for (i = 0; i < 1024; i++,p++)
         xf86OutRamDacData(BT485_CURS_RAM_DATA, *p);

      xf86BtYPosMask = 0xFF;
      xf86OutRamDacReg(BT485_COMMAND_REG_2, 0xF7, 0x00);
   }

   /* position cursor */
   xf86Bt485MoveCursor(0, x, y);

   /* turn the cursor on */
   xf86Bt485CursorOn();

   return;
}
#endif

#ifdef __STDC__
void
xf86Bt485HWSave( union xf86RamDacSave * save )
#else
void
xf86Bt485HWSave( save )
   union xf86RamDacSave *save;
#endif
{
   save->Bt485.Com0 = xf86InRamDacReg( BT485_COMMAND_REG_0 );
   save->Bt485.Com1 = xf86InRamDacReg( BT485_COMMAND_REG_1 );
   save->Bt485.Com2 = xf86InRamDacReg( BT485_COMMAND_REG_2 );
   save->Bt485.Com3 = xf86InBt485IndReg( BT485_COMMAND_REG_3 );
}

#ifdef __STDC__
void
xf86Bt485HWRestore( union xf86RamDacSave * save )
#else
void
xf86Bt485HWRestore( save )
   union xf86RamDacSave *save;
#endif
{
   xf86OutBt485IndReg( BT485_COMMAND_REG_3, 0x00, save->Bt485.Com3 );
   xf86OutRamDacReg( BT485_COMMAND_REG_2, 0x00, save->Bt485.Com2 );
   xf86OutRamDacReg( BT485_COMMAND_REG_1, 0x00, save->Bt485.Com1 );
   xf86OutRamDacReg( BT485_COMMAND_REG_0, 0x00, save->Bt485.Com0 );
}

#ifdef __STDC__
void
xf86Bt485Init( void )
#else
void
xf86Bt485Init()
#endif
{
   if (xf86Dac8Bit)
      xf86OutRamDacReg( BT485_COMMAND_REG_0, 0xFC, 0x02 );
   else
      xf86OutRamDacReg( BT485_COMMAND_REG_0, 0xFC, 0x00 );

   if (xf86DacSyncOnGreen)
      xf86OutRamDacReg( BT485_COMMAND_REG_0, 0xE3, 0x08 );
   else
      xf86OutRamDacReg( BT485_COMMAND_REG_0, 0xE3, 0x00 );
}


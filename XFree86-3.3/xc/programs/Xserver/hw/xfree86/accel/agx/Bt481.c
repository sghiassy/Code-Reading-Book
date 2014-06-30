/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/Bt481.c,v 3.6 1996/12/23 06:32:18 dawes Exp $ */
/*
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
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
 * DAVID WEXELBLAT AND HENRY A. WORTH DISCLAIMS ALL WARRANTIES 
 * WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS 
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES 
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Rewritten for the AGX and BT481/2 by Henry A. Worth <haw30@eng.amdahl.com> 
 *
 */
/* $XConsortium: Bt481.c /main/4 1996/02/21 17:15:26 kaleb $ */

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
#include "Bt481.h"

static unsigned char xf86BtYPosMask = 0xFF;

#ifndef __GNUC__
# define __inline__ /**/
#endif

/*
 * Some registers in the Bt481, like "Command Register B", 
 * must be accessed indirectly. 
 */
#ifdef __STDC__
void 
xf86OutBt481IndReg( unsigned char reg, 
                      unsigned char mask, 
                      unsigned char data )
#else
void 
xf86OutBt481IndReg(reg, mask, data)
   unsigned char reg;
   unsigned char mask;
   unsigned char data;
#endif
{
   unsigned char tmp;

   tmp = xf86InRamDacReg(BT481_COMMAND_REG_A) & 0xFE;
   xf86OutRamDacData(BT481_COMMAND_REG_A, 0x01);
   xf86OutRamDacData(BT481_WRITE_ADDR, reg);
   xf86OutRamDacReg(BT481_PIXEL_MASK, mask, data);
   xf86OutRamDacData(BT481_COMMAND_REG_A, tmp);
}

#ifdef __STDC__
unsigned char 
xf86InBt481IndReg( unsigned char reg ) 
#else
unsigned char
xf86InBt481IndReg(reg)
   unsigned char reg;
#endif
{
   unsigned char ret;
   unsigned char tmp;

   tmp = xf86InRamDacReg(BT481_COMMAND_REG_A) & 0xFE;
   xf86OutRamDacData(BT481_COMMAND_REG_A, 0x01);
   xf86OutRamDacData(BT481_WRITE_ADDR, reg);
   ret = xf86InRamDacReg(BT481_PIXEL_MASK);
   xf86OutRamDacData(BT481_COMMAND_REG_A, tmp);
   return(ret);
}

#ifdef __STDC__
void
xf86Bt481HWSave( union xf86RamDacSave * save )
#else
void
xf86Bt481HWSave( save )
   union xf86RamDacSave *save;
#endif
{
   save->Bt481.ComA = xf86InRamDacReg( BT481_COMMAND_REG_A );
   save->Bt481.ComB = xf86InBt481IndReg( BT481_COMMAND_REG_B );
}

#ifdef __STDC__
void
xf86Bt481HWRestore( union xf86RamDacSave * save )
#else
void
xf86Bt481HWRestore( save )
   union xf86RamDacSave *save;
#endif
{
   xf86OutBt481IndReg( BT481_COMMAND_REG_B, 0x00, save->Bt481.ComB );
   xf86OutRamDacData( BT481_COMMAND_REG_A, save->Bt481.ComA ); 
}

#ifdef __STDC__
void
xf86Bt481Init( void )
#else
void
xf86Bt481Init()
#endif
{
   xf86OutBt481IndReg( BT481_COMMAND_REG_B, 0x00, 0x00 ); /* powerup */

   GlennsIODelay();
   switch( xf86RamDacBPP ) {
   
      case 8:
         xf86OutRamDacData( BT481_COMMAND_REG_A, BT481_8BPP_PSUEDO_COLOR );
         break;

      case 15:
         xf86OutRamDacData( BT481_COMMAND_REG_A, BT481_15BPP_EDGE_TRIGGR );
         break;
   
      case 16:
         xf86OutRamDacData( BT481_COMMAND_REG_A, BT481_16BPP_EDGE_TRIGGR );
         break;
   
      case 24:
      case 32:
         xf86OutRamDacData( BT481_COMMAND_REG_A, BT481_24BPP_EDGE_TRIGGR );
   }

   GlennsIODelay();
   xf86OutBt481IndReg( BT481_COMMAND_REG_B, 0x00, 0x00 ); /* powerup */

   GlennsIODelay();
   if (xf86Dac8Bit)
      xf86OutBt481IndReg( BT481_COMMAND_REG_B, 0x7C, 0x2 );

   GlennsIODelay();
   if (xf86DacSyncOnGreen)
      xf86OutBt481IndReg( BT481_COMMAND_REG_B, 0x73, 0x4 );

}

/*
 * Convert the cursor from server-format to hardware-format.  The Bt482
 * has two planes, output sequentially.
 */
Bool
xf86Bt482RealizeCursor(pScr, pCurs)
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

   wsrc = PixmapBytePad(bits->width, 1);        /* bytes per line */

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
xf86Bt482CursorOn()
{
   /* Enable cursor mode 3 - X11 mode */
   xf86OutBt481IndReg(BT482_CURSOR_REG, 0xFC, 0x03);

   return;
}

void
xf86Bt482CursorOff()
{
   /* Disable cursor */
   xf86OutBt481IndReg(BT482_CURSOR_REG, 0xFC, 0x00);

   return;
}

void
xf86Bt482MoveCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
   extern int xf86hotX, xf86hotY;

   if (!xf86VTSema)
      return;

   x -= xf86FrameX0;
   x += xf86MaxCurs;
   x -= xf86hotX;
   if (x < 0)
      return;

   y -= xf86FrameY0;
   y += xf86MaxCurs;
   y -= xf86hotY;
   if (y < 0)
      return;

   /* Output position - "only" 12 bits of location documented */
   xf86OutBt481IndReg(BT482_CURS_X_LOW, 0x00, x & 0xFF);
   xf86OutBt481IndReg(BT482_CURS_X_HIGH, 0x00, (x >> 8) & 0x0F);
   xf86OutBt481IndReg(BT482_CURS_Y_LOW, 0x00, y & xf86BtYPosMask);
   xf86OutBt481IndReg(BT482_CURS_Y_HIGH, 0x00, (y >> 8) & 0x0F);

   return;
}

void
xf86Bt482RecolorCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   /* Start writing at address 1 (0 is overscan color) */
   xf86OutBt481IndReg(BT482_CURSOR_REG, 0xF7, 0x00);
   xf86OutRamDacData(BT482_CURS_WR_ADDR, 0x01);

   /* Background color */
   xf86OutRamDacData(BT482_CURS_RAM_DATA, (pCurs->backRed >> 8) & 0xFF);
   xf86OutRamDacData(BT482_CURS_RAM_DATA, (pCurs->backGreen >> 8) & 0xFF);
   xf86OutRamDacData(BT482_CURS_RAM_DATA, (pCurs->backBlue >> 8) & 0xFF);

   /* Foreground color */
   xf86OutRamDacData(BT482_CURS_RAM_DATA, (pCurs->foreRed >> 8) & 0xFF);
   xf86OutRamDacData(BT482_CURS_RAM_DATA, (pCurs->foreGreen >> 8) & 0xFF);
   xf86OutRamDacData(BT482_CURS_RAM_DATA, (pCurs->foreBlue >> 8) & 0xFF);

   return;
}

void
xf86Bt482LoadCursor(pScr, pCurs, modesFlags, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int modesFlags;
     int x, y;
{
   int   index = pScr->myNum;
   register int   i, j;
   unsigned char *ram, *p;
   unsigned char  bytesPerRow = xf86MaxCurs >> 3;

   if (!xf86VTSema)
      return;

   if (!pCurs)
      return;

   /* turn the cursor off */
   xf86Bt482CursorOff();

   /* load colormap */
   xf86Bt482RecolorCursor(pScr, pCurs);

   ram = (unsigned char *)pCurs->bits->devPriv[index];

   if (modesFlags & V_INTERLACE) {
      /* Start data output */
      xf86OutBt481IndReg(BT482_CURSOR_REG, 0xF7, 0x08);
      xf86OutRamDacReg(BT481_WRITE_ADDR, 0x00, 0x00);

      for (i=0; i < xf86MaxCurs; i++) {          /* rows in cursor */
         p = ram + (((i % 2) ? i-1 : i+1)*8);
         for (j=0; j < bytesPerRow; j++,p++) {    /* bytes per row */
            xf86OutRamDacData(BT482_CURS_RAM_DATA, *p);
         }
      }
      ram += 512;
      for (i=0; i < xf86MaxCurs; i++) {          /* rows in cursor */
         p = ram + (((i % 2) ? i-1 : i+1)*8);
         for (j=0; j < bytesPerRow; j++,p++) {    /* bytes per row */
            xf86OutRamDacData(BT482_CURS_RAM_DATA, *p);
         }
      }

      xf86BtYPosMask = 0xFE;
      xf86OutBt481IndReg(BT482_CURSOR_REG, 0xCF, 0x10);
   } else {
      /* Start data output */
      xf86OutBt481IndReg(BT482_CURSOR_REG, 0xF7, 0x08);
      xf86OutRamDacReg(BT481_WRITE_ADDR, 0x00, 0x00);

      /*
       * Output the cursor data.  The realize function has put the planes into
       * their correct order, so we can just blast this out.
       */
      p = ram;
      for (i = 0; i < 1024; i++,p++)
         xf86OutRamDacData(BT482_CURS_RAM_DATA, *p);

      xf86BtYPosMask = 0xFF;
      xf86OutBt481IndReg(BT482_CURSOR_REG, 0xCF, 0x00);
   }

   /* position cursor */
   xf86Bt482MoveCursor(0, x, y);

   /* turn the cursor on */
   xf86Bt482CursorOn();

   return;
}



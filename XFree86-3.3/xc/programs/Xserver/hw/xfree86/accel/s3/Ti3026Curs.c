/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/Ti3026Curs.c,v 3.6 1996/12/23 06:41:20 dawes Exp $ */
/*
 * Copyright 1994 by Robin Cutshaw <robin@XFree86.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Robin Cutshaw not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Robin Cutshaw makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ROBIN CUTSHAW DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ROBIN CUTSHAW BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * Modified for TVP3026 by Harald Koenig <koenig@tat.physik.uni-tuebingen.de>
 *
 */
/* $XConsortium: Ti3026Curs.c /main/5 1996/10/24 07:10:49 kaleb $ */


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
#include "s3.h"
#include "regs3.h"
#include "Ti302X.h"

#define MAX_CURS_HEIGHT 64   /* 64 scan lines */
#define MAX_CURS_WIDTH  64   /* 64 pixels     */

extern Bool tmp_useSWCursor;

#ifndef __GNUC__
# define __inline__ /**/
#endif


/*
 * s3OutTi3026IndReg() and s3InTi3026IndReg() are used to access the indirect
 * 3026 registers only.
 */

#ifdef __STDC__
void s3OutTi3026IndReg(unsigned char reg, unsigned char mask, unsigned char data)
#else
void s3OutTi3026IndReg(reg, mask, data)
unsigned char reg;
unsigned char mask;
unsigned char data;
#endif
{
   unsigned char tmp, tmp0, tmp1, tmp2 = 0x00;

   outb(vgaCRIndex, 0x55);
   tmp0 = inb(vgaCRReg);
   tmp  = tmp0 & 0xFC;
   outb(vgaCRReg, tmp | 0x00);
   tmp1 = inb(0x3c8);
   outb(0x3c8, reg);
   outb(vgaCRReg, tmp | 0x02);

   if (mask != 0x00)
      tmp2 = inb(0x3c6) & mask;
   outb(0x3c6, tmp2 | data);
   
   outb(vgaCRReg, tmp | 0x00);
   outb(0x3c8, tmp1);  /* just in case anyone relies on this */
   outb(vgaCRReg, tmp0);

#ifdef EXTENDED_DEBUG
   ErrorF("Set Ti Ind 0x%x to 0x%x\n",reg,tmp2|data);
#endif
}

#ifdef __STDC__
unsigned char s3InTi3026IndReg(unsigned char reg)
#else
unsigned char s3InTi3026IndReg(reg)
unsigned char reg;
#endif
{
   unsigned char tmp, tmp0, tmp1, ret;

   outb(vgaCRIndex, 0x55);
   tmp0 = inb(vgaCRReg);
   tmp  = tmp0 & 0xFC;
   outb(vgaCRReg, tmp | 0x00);
   tmp1 = inb(0x3c8);
   outb(0x3c8, reg);
   outb(vgaCRReg, tmp | 0x02);

   ret = inb(0x3c6);

   outb(vgaCRReg, tmp | 0x00);
   outb(0x3c8, tmp1);  /* just in case anyone relies on this */
   outb(vgaCRReg, tmp0);

   return(ret);
}

/*
 * Convert the cursor from server-format to hardware-format.  The Ti3020
 * has two planes, plane 0 selects cursor color 0 or 1 and plane 1
 * selects transparent or display cursor.  The bits of these planes
 * loaded sequentially so that one byte has 8 pixels. The organization
 * looks like:
 *             Byte 0x000 - 0x007    top scan line, left to right plane 0
 *                  0x008 - 0x00F
 *                    .       .
 *                  0x1F8 - 0x1FF    bottom scan line plane 0
 *
 *                  0x200 - 0x207    top scan line, left to right plane 1
 *                  0x208 - 0x20F
 *                    .       .
 *                  0x3F8 - 0x3FF    bottom scan line plane 1
 *
 *             Byte/bit map - D7,D6,D5,D4,D3,D2,D1,D0  eight pixels each
 *             Pixel/bit map - P1P0  (plane 1) == 1 maps to cursor color
 *                                   (plane 1) == 0 maps to transparent
 *                                   (plane 0) maps to cursor colors 0 and 1
 */

Bool
s3Ti3026RealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   extern unsigned char s3SwapBits[256];
   register int i, j;
   unsigned char *pServMsk;
   unsigned char *pServSrc;
   int   index = pScr->myNum;
   pointer *pPriv = &pCurs->bits->devPriv[index];
   int   wsrc, h;
   unsigned char *ram, *plane0, *plane1;
   CursorBitsPtr bits = pCurs->bits;

   if (bits->height > MAX_CURS_HEIGHT || bits->width > MAX_CURS_WIDTH) {
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
   if (h > MAX_CURS_HEIGHT)
      h = MAX_CURS_HEIGHT;

   wsrc = PixmapBytePad(bits->width, 1);	/* bytes per line */

   for (i = 0; i < MAX_CURS_HEIGHT; i++) {
      for (j = 0; j < MAX_CURS_WIDTH / 8; j++) {
	 unsigned char mask, source;

	 if (i < h && j < wsrc) {
	    source = *pServSrc++;
	    mask = *pServMsk++;

	    source = s3SwapBits[source];
	    mask = s3SwapBits[mask];

	    if (j < MAX_CURS_WIDTH / 8) {
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
s3Ti3026CursorOn()
{
   unsigned char tmp;

   UNLOCK_SYS_REGS;

   /* turn on external cursor */
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xDF;
   outb(vgaCRReg, tmp | 0x20);

   /* Enable Ti3026 */
   outb(vgaCRIndex, 0x45);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp & ~0x20);
   
   /* Enable cursor - X11 mode */
   s3OutTi3026IndReg(TI_CURS_CONTROL, 0x6c, 0x13);

   LOCK_SYS_REGS;
   return;
}

void
s3Ti3026CursorOff()
{
   UNLOCK_SYS_REGS;

   /*
    * Don't need to undo the S3 registers here; they will be undone when
    * the mode is restored from save registers.  If it is done here, it
    * causes the cursor to flash each time it is loaded, so don't do that.
    */

   /* Disable cursor */
   s3OutTi3026IndReg(TI_CURS_CONTROL, 0xfc, 0x00);

   LOCK_SYS_REGS;
   return;
}

void
s3Ti3026MoveCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
   extern int s3AdjustCursorXPos;
   extern int s3hotX, s3hotY;
   unsigned char tmp;

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
   x += 64 - s3hotX;
   if (x < 0)
      return;

   y -= s3InfoRec.frameY0;
   y += 64 - s3hotY;
   if (y < 0)
      return;

   UNLOCK_SYS_REGS;

   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x03);

   /* Output position - "only" 12 bits of location documented */
   outb(0x3c8, x & 0xFF);
   outb(0x3c9, (x >> 8) & 0x0F);
   outb(0x3c6, y & 0xFF);
   outb(0x3c7, (y >> 8) & 0x0F);

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, tmp | 0x00);

   LOCK_SYS_REGS;
   return;
}

void
s3Ti3026RecolorCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   unsigned char tmp;
   UNLOCK_SYS_REGS;

   /* The TI 3026 cursor is always 8 bits so shift 8, not 10 */

   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01);

   /* Background color */
   outb(0x3c8, 0x01); /* cursor color write address */
   outb(0x3c9, (pCurs->backRed >> 8)   & 0xFF);
   outb(0x3c9, (pCurs->backGreen >> 8) & 0xFF);
   outb(0x3c9, (pCurs->backBlue >> 8)  & 0xFF);

   /* Foreground color */
   outb(0x3c8, 0x02); /* cursor color write address */
   outb(0x3c9, (pCurs->foreRed >> 8)   & 0xFF);
   outb(0x3c9, (pCurs->foreGreen >> 8) & 0xFF);
   outb(0x3c9, (pCurs->foreBlue >> 8)  & 0xFF);

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, tmp | 0x00);

   LOCK_SYS_REGS;
   return;
}

void 
s3Ti3026LoadCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int x, y;
{
   int   index = pScr->myNum;
   register int   i;
   unsigned char *ram, *p, tmp, tmpcurs;
   extern int s3InitCursorFlag;

   if (!xf86VTSema)
      return;

   if (!pCurs)
      return;

   /* turn the cursor off */
   if ((tmpcurs = s3InTi3026IndReg(TI_CURS_CONTROL)) & 0x03)
      s3Ti3026CursorOff();

   /* load colormap */
   s3Ti3026RecolorCursor(pScr, pCurs);

   ram = (unsigned char *)pCurs->bits->devPriv[index];

   UNLOCK_SYS_REGS;
   BLOCK_CURSOR;

   /* position cursor off screen */
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x03);

   outb(0x3c8, 0);
   outb(0x3c9, 0);
   outb(0x3c6, 0);
   outb(0x3c7, 0);

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, tmp | 0x00);

   s3OutTi3026IndReg(TI_CURS_CONTROL, 0xf3, 0x00); /* reset A9,A8 */
   outb(0x3c8, 0x00); /* reset cursor RAM load address A7..A0 */

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, tmp | 0x02);

   /* 
    * Output the cursor data.  The realize function has put the planes into
    * their correct order, so we can just blast this out.
    */
   p = ram;
   for (i = 0; i < 1024; i++,p++)
      outb(0x3c7, (*p));

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, tmp);

   UNBLOCK_CURSOR;
   LOCK_SYS_REGS;

   /* position cursor */
   s3Ti3026MoveCursor(0, x, y);

   /* turn the cursor on */
   if ((tmpcurs & 0x03) || s3InitCursorFlag)
      s3Ti3026CursorOn();

   if (s3InitCursorFlag)
      s3InitCursorFlag = FALSE;

   return;
}



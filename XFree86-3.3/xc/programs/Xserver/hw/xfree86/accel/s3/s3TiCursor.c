/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3TiCursor.c,v 3.11 1996/12/23 06:41:35 dawes Exp $ */
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
 */
/* $XConsortium: s3TiCursor.c /main/7 1996/02/21 17:34:44 kaleb $ */

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
 * TI ViewPoint 3020/3025 support - Robin Cutshaw
 *
 * The Ti3020 has 8 direct command registers and indirect registers
 * 0x00-0x3F and 0xFF.  The low-order two bits of the direct register
 * address follow normal VGA DAC addressing conventions (which 
 * for some reason aren't in numeric order, so we remap them through 
 * an array).  The S3 provides access to the high-order bit via
 * the low-order bit of CR55.  The indirect registers are accessed
 * through the direct index and data registers.  See s3Ti3020.h for
 * details.
 *
 * The Ti3025 is both Ti3020 and Bt485 compatable.  The mode of
 * operation is set via RS4 using S3 register 0x5C and the 3020
 * cursor control register.  The 3025 also has a built in PLL
 * clock generator.
 */

static Bool s3In3020mode = FALSE;             /* starts in Bt485 mode */

void s3set3020mode()
{
   unsigned char tmp, tmp1, tmp2;

   outb(vgaCRIndex, 0x5C);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp & 0xDF);           /* clear RS4 - use 3020 mode */

   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01);  /* toggle to upper 4 direct registers */
   tmp1 = inb(TI_INDEX_REG);
   outb(TI_INDEX_REG, TI_CURS_CONTROL);
   tmp2 = inb(TI_DATA_REG);
   outb(TI_DATA_REG, tmp2 & 0x7F);      /* clear TI_PLANAR_ACCESS bit */

   outb(TI_INDEX_REG, tmp1);
   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, tmp);
   s3In3020mode = TRUE;
}

void s3set485mode()
{
   unsigned char tmp, tmp1;

   outb(vgaCRIndex, 0x5C);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp & 0xDF);           /* clear RS4 - use 3020 mode */

   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01);  /* toggle to upper 4 direct registers */
   outb(TI_INDEX_REG, TI_CURS_CONTROL);
   tmp1 = inb(TI_DATA_REG);
   outb(TI_DATA_REG, tmp1 | 0x80);        /* set TI_PLANAR_ACCESS bit */

   outb(vgaCRIndex, 0x5C);
   outb(vgaCRReg, tmp | 0x20);              /* set RS4 - use 485 mode */

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, tmp);
   s3In3020mode = FALSE;
}

/*
 * s3OutTiIndReg() and s3InTiIndReg() are used to access the indirect
 * 3020 registers only.
 */

#ifdef __STDC__
void s3OutTiIndReg(unsigned char reg, unsigned char mask, unsigned char data)
#else
void s3OutTiIndReg(reg, mask, data)
unsigned char reg;
unsigned char mask;
unsigned char data;
#endif
{
   unsigned char tmp, tmp1, tmp2 = 0x00;

   /* High 2 bits of reg in CR55 bits 0-1 (1 is cleared for the TI ramdac) */
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01);  /* toggle to upper 4 direct registers */
   tmp1 = inb(TI_INDEX_REG);
   outb(TI_INDEX_REG, reg);

   /* Have to map the low two bits to the correct DAC register */
   if (mask != 0x00)
      tmp2 = inb(TI_DATA_REG) & mask;
   outb(TI_DATA_REG, tmp2 | data);
   
   /* Now clear 2 high-order bits so that other things work */
   outb(TI_INDEX_REG, tmp1);  /* just in case anyone relies on this */
   outb(vgaCRReg, tmp);
}

#ifdef __STDC__
unsigned char s3InTiIndReg(unsigned char reg)
#else
unsigned char s3InTiIndReg(reg)
unsigned char reg;
#endif
{
   unsigned char tmp, tmp1, ret;

   /* High 2 bits of reg in CR55 bits 0-1 (1 is cleared for the TI ramdac) */
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01);  /* toggle to upper 4 direct registers */
   tmp1 = inb(TI_INDEX_REG);
   outb(TI_INDEX_REG, reg);

   /* Have to map the low two bits to the correct DAC register */
   ret = inb(TI_DATA_REG);

   /* Now clear 2 high-order bits so that other things work */
   outb(TI_INDEX_REG, tmp1);  /* just in case anyone relies on this */
   outb(vgaCRReg, tmp);

   return(ret);
}

/*
 * Convert the cursor from server-format to hardware-format.  The Ti3020
 * has two planes, plane 0 selects cursor color 0 or 1 and plane 1
 * selects transparent or display cursor.  The bits of these planes
 * are packed together so that one byte has 4 pixels. The organization
 * looks like:
 *             Byte 0x000 - 0x00F    top scan line, left to right
 *                  0x010 - 0x01F
 *                    .       .
 *                  0x3F0 - 0x3FF    bottom scan line
 *
 *             Byte/bit map - D7D6,D5D4,D3D2,D1D0  four pixels, two planes each
 *             Pixel/bit map - P1P0  (plane 1) == 1 maps to cursor color
 *                                   (plane 1) == 0 maps to transparent
 *                                   (plane 0) maps to cursor colors 0 and 1
 */

Bool
s3TiRealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   register int i, j;
   unsigned char *pServMsk;
   unsigned char *pServSrc;
   pointer *pPriv;
   int   wsrc, h;
   unsigned char *ram;

   if (pCurs->bits->height > MAX_CURS_HEIGHT || pCurs->bits->width > MAX_CURS_WIDTH) {
      extern miPointerSpriteFuncRec miSpritePointerFuncs;
      return (miSpritePointerFuncs.RealizeCursor)(pScr, pCurs);
   }

   if (pCurs->bits->refcnt > 1)
      return TRUE;

   ram = (unsigned char *)xalloc(1024);  /* 64x64x2 bits */
   pPriv = &pCurs->bits->devPriv[pScr->myNum];
   *pPriv = (pointer) ram;

   if (!ram)
      return FALSE;

   pServSrc = (unsigned char *)pCurs->bits->source;
   pServMsk = (unsigned char *)pCurs->bits->mask;

   h = pCurs->bits->height;
   if (h > MAX_CURS_HEIGHT)
      h = MAX_CURS_HEIGHT;

   wsrc = PixmapBytePad(pCurs->bits->width, 1);	/* bytes per line */

   for (i = 0; i < MAX_CURS_HEIGHT; i++,ram+=16) {
      for (j = 0; j < MAX_CURS_WIDTH / 8; j++) {
	 register unsigned char mask, source;

	 if (i < h && j < wsrc) {
	    /*
	     * normally we would use s3SwapBits to quickly reverse bits
	     * but since have to do bit twiddles anyway, there is no need.
	     * mask byte ABCDEFGH and source byte 12345678 map to two byte
	     * cursor data H8G7F6E5 D4C3B2A1
	     */
	    mask = *pServMsk++;
	    source = *pServSrc++ & mask;

	    /* map 1 byte source and mask into two byte cursor data */
	    ram[j*2] =     ((mask&0x01) << 7) | ((source&0x01) << 6) |
		           ((mask&0x02) << 4) | ((source&0x02) << 3) |
		           ((mask&0x04) << 1) | (source&0x04)        |
		           ((mask&0x08) >> 2) | ((source&0x08) >> 3) ;
	    ram[(j*2)+1] = ((mask&0x10) << 3) | ((source&0x10) << 2) |
		           (mask&0x20)        | ((source&0x20) >> 1) |
		           ((mask&0x40) >> 3) | ((source&0x40) >> 4) |
		           ((mask&0x80) >> 6) | ((source&0x80) >> 7) ;
	 } else {
	    ram[j*2]     = 0x00;
	    ram[(j*2)+1] = 0x00;
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
s3TiCursorOn()
{
   unsigned char tmp;

   UNLOCK_SYS_REGS;

   /* turn on external cursor */
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xDF;
   outb(vgaCRReg, tmp | 0x20);

   /* Enable Ti3020 */
   outb(vgaCRIndex, 0x45);
   tmp = inb(vgaCRReg) & 0xDF;
   outb(vgaCRReg, tmp | 0x20);
   
   /* Enable cursor - sprite enable, X11 mode */
   s3OutTiIndReg(TI_CURS_CONTROL,
		 (unsigned char )~TI_CURS_CTRL_MASK,
	         TI_CURS_SPRITE_ENABLE | TI_CURS_X_WINDOW_MODE);

   LOCK_SYS_REGS;
   return;
}

void
s3TiCursorOff()
{
   UNLOCK_SYS_REGS;

   /*
    * Don't need to undo the S3 registers here; they will be undone when
    * the mode is restored from save registers.  If it is done here, it
    * causes the cursor to flash each time it is loaded, so don't do that.
    */

   /* Disable cursor */
   s3OutTiIndReg(TI_CURS_CONTROL,
		 (unsigned char )~TI_CURS_CTRL_MASK, 0x00);

   LOCK_SYS_REGS;
   return;
}

void
s3TiMoveCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
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
   if (x < 0)
      return;

   y -= s3InfoRec.frameY0;
   if (y < 0)
      return;

   UNLOCK_SYS_REGS;

   /* Output position - "only" 12 bits of location documented */
   s3OutTiIndReg(TI_CURS_X_LOW, 0x00, x & 0xFF);
   s3OutTiIndReg(TI_CURS_X_HIGH, 0x00, (x >> 8) & 0x0F);
   s3OutTiIndReg(TI_CURS_Y_LOW, 0x00, y & 0xFF);
   s3OutTiIndReg(TI_CURS_Y_HIGH, 0x00, (y >> 8) & 0x0F);

   LOCK_SYS_REGS;
   return;
}

void
s3TiRecolorCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   UNLOCK_SYS_REGS;

   /* The TI 3020 cursor is always 8 bits so shift 8, not 10 */

   /* Background color */
   s3OutTiIndReg(TI_CURSOR_COLOR_0_RED,   0, (pCurs->backRed >> 8) & 0xFF);
   s3OutTiIndReg(TI_CURSOR_COLOR_0_GREEN, 0, (pCurs->backGreen >> 8) &0xFF);
   s3OutTiIndReg(TI_CURSOR_COLOR_0_BLUE,  0, (pCurs->backBlue >> 8) & 0xFF);

   /* Foreground color */
   s3OutTiIndReg(TI_CURSOR_COLOR_1_RED,   0, (pCurs->foreRed >> 8) & 0xFF);
   s3OutTiIndReg(TI_CURSOR_COLOR_1_GREEN, 0, (pCurs->foreGreen >> 8) & 0xFF);
   s3OutTiIndReg(TI_CURSOR_COLOR_1_BLUE,  0, (pCurs->foreBlue >> 8) & 0xFF);

   LOCK_SYS_REGS;
   return;
}

void 
s3TiLoadCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int x, y;
{
   extern int s3hotX, s3hotY;
   int   index = pScr->myNum;
   register int   i;
   unsigned char *ram, *p, tmp, tmp1, tmpcurs;
   extern int s3InitCursorFlag;

   if (!xf86VTSema)
      return;

   if (!pCurs)
      return;

   /* turn the cursor off */
   if ((tmpcurs = s3InTiIndReg(TI_CURS_CONTROL)) & TI_CURS_SPRITE_ENABLE)
      s3TiCursorOff();

   /* load colormap */
   s3TiRecolorCursor(pScr, pCurs);

   ram = (unsigned char *)pCurs->bits->devPriv[index];

   UNLOCK_SYS_REGS;
   BLOCK_CURSOR;

   /* The hardware cursor is not supported in interlaced mode */

   /* High 2 bits of reg in CR55 bits 0-1 (1 is cleared for the TI ramdac) */
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01); /* toggle to the high four direct registers */
   tmp1 = inb(TI_INDEX_REG);

   outb(TI_INDEX_REG, TI_CURS_RAM_ADDR_LOW); /* must be first */
   outb(TI_DATA_REG, 0x00);
   outb(TI_INDEX_REG, TI_CURS_RAM_ADDR_HIGH);
   outb(TI_DATA_REG, 0x00);
   outb(TI_INDEX_REG, TI_CURS_RAM_DATA);

   /* 
    * Output the cursor data.  The realize function has put the planes into
    * their correct order, so we can just blast this out.
    */
   p = ram;
   for (i = 0; i < 1024; i++,p++)
      outb(TI_DATA_REG, *p);

   if (s3hotX >= MAX_CURS_WIDTH)
      s3hotX = MAX_CURS_WIDTH - 1;
   else if (s3hotX < 0)
      s3hotX = 0;
   if (s3hotY >= MAX_CURS_HEIGHT)
      s3hotY = MAX_CURS_HEIGHT - 1;
   else if (s3hotY < 0)
      s3hotY = 0;

   outb(TI_INDEX_REG, TI_SPRITE_ORIGIN_X); /* offset into cursor data */
   outb(TI_DATA_REG, s3hotX);
   outb(TI_INDEX_REG, TI_SPRITE_ORIGIN_Y);
   outb(TI_DATA_REG, s3hotY);
   outb(TI_INDEX_REG, tmp1);

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, tmp);

   UNBLOCK_CURSOR;
   LOCK_SYS_REGS;

   /* position cursor */
   s3TiMoveCursor(0, x, y);

   /* turn the cursor on */
   if ((tmpcurs & TI_CURS_SPRITE_ENABLE) || s3InitCursorFlag)
      s3TiCursorOn();

   if (s3InitCursorFlag)
      s3InitCursorFlag = FALSE;

   return;
}

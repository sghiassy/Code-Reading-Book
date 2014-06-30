/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/IBMRGBCurs.c,v 3.9 1997/01/08 20:33:36 dawes Exp $ */
/*
 *
 * Copyright 1995 The XFree86 Project, Inc.
 *
 */
/* $XConsortium: IBMRGBCurs.c /main/7 1996/10/24 07:10:45 kaleb $ */

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
#include "IBMRGB.h"

#define MAX_CURS_HEIGHT 64   /* 64 scan lines */
#define MAX_CURS_WIDTH  64   /* 64 pixels     */

extern Bool tmp_useSWCursor;

#ifndef __GNUC__
# define __inline__ /**/
#endif



/*
 * Convert the cursor from server-format to hardware-format.  
 * The IBM RGB52x has one array of two-bit-pixels. The MSB contains
 * the transparency information (mask) and the LSB is the color bit.
 * The pixel order within a byte can be selected and is MSB-to-LSB
 * or "00112233".
 */

Bool
s3IBMRGBRealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   static unsigned short s3SwapExpandBits[256], s3SwapExpandBits_init=1;
   register int i, j;
   unsigned char *pServMsk;
   unsigned char *pServSrc;
   int   indx2 = pScr->myNum;
   pointer *pPriv = &pCurs->bits->devPriv[indx2];
   int   wsrc, h;
   unsigned char *ram, *plane0, *plane1;
   CursorBitsPtr bits = pCurs->bits;

   if (bits->height > MAX_CURS_HEIGHT || bits->width > MAX_CURS_WIDTH) {
      extern miPointerSpriteFuncRec miSpritePointerFuncs;
      return (miSpritePointerFuncs.RealizeCursor)(pScr, pCurs);
   }

   if (pCurs->bits->refcnt > 1)
      return TRUE;

   if (s3SwapExpandBits_init) {
      int k;
      unsigned short s = 0;
      s3SwapExpandBits_init = 0;
      for(i=0; i<256; i++) {
	 j = i;
	 for (k=0; k<8; k++) {
	    s <<= 1;
	    s <<= 1;
	    s |= (j&1);
	    j >>= 1;
	 }
	 s3SwapExpandBits[i] = s;
      }
   }
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

	    if (j < MAX_CURS_WIDTH / 8) {
	       unsigned short s;
#if 1
	       s = s3SwapExpandBits[source] | (s3SwapExpandBits[mask]<<1);
	       *plane0++ = s>>8;
	       *plane0++ = s;
#else
	       int k;
	       for (k=0; k<8; k++) {
		  s = (s<<2) | ((mask&1)<<1) | (source&1);
		  source >>= 1;
		  mask   >>= 1;
	       }
	       *plane0++ = s>>8;
	       *plane0++ = s;
#endif
	    }
	 } else {
	    *plane0++ = 0x00;
	    *plane0++ = 0x00;
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
s3IBMRGBCursorOn()
{
   unsigned char tmp;

   UNLOCK_SYS_REGS;

   /* turn on external cursor */
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xDF;
   outb(vgaCRReg, tmp | 0x20);

   /* Enable IBMRGB */
   outb(vgaCRIndex, 0x45);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp & ~0x20);
   
   /* Enable cursor - X11 mode */
   s3OutIBMRGBIndReg(IBMRGB_curs, 0, 0x27);

   LOCK_SYS_REGS;
   return;
}

void
s3IBMRGBCursorOff()
{
   UNLOCK_SYS_REGS;

   /*
    * Don't need to undo the S3 registers here; they will be undone when
    * the mode is restored from save registers.  If it is done here, it
    * causes the cursor to flash each time it is loaded, so don't do that.
    */

   /* Disable cursor */
   s3OutIBMRGBIndReg(IBMRGB_curs, ~3, 0x00);

   LOCK_SYS_REGS;
   return;
}

void
s3IBMRGBMoveCursor(pScr, x, y)
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
/*   x += 64 - s3hotX; */
   if (x < 0)
      return;

   y -= s3InfoRec.frameY0;
/*   y += 64 - s3hotY; */
   if (y < 0)
      return;

   if (s3InfoRec.modes->Flags & V_DBLSCAN)
      y <<= 1;

   if (s3InfoRec.modes->Flags & V_INTERLACE)
      y >>= 1;

   UNLOCK_SYS_REGS;

#if 0
   s3OutIBMRGBIndReg(IBMRGB_curs_hot_x, 0, s3hotX);
   s3OutIBMRGBIndReg(IBMRGB_curs_hot_y, 0, s3hotY);
   s3OutIBMRGBIndReg(IBMRGB_curs_xl, 0, x);
   s3OutIBMRGBIndReg(IBMRGB_curs_xh, 0, x>>8);
   s3OutIBMRGBIndReg(IBMRGB_curs_yl, 0, y);
   s3OutIBMRGBIndReg(IBMRGB_curs_yh, 0, y>>8);
#else   
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_hot_x);  outb(IBMRGB_INDEX_DATA, s3hotX);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_hot_y);  outb(IBMRGB_INDEX_DATA, s3hotY);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_xl);     outb(IBMRGB_INDEX_DATA, x);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_xh);     outb(IBMRGB_INDEX_DATA, x>>8);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_yl);     outb(IBMRGB_INDEX_DATA, y);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_yh);     outb(IBMRGB_INDEX_DATA, y>>8);
   outb(vgaCRReg, tmp);
#endif

   LOCK_SYS_REGS;
   return;
}

void
s3IBMRGBRecolorCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   unsigned char tmp;
   UNLOCK_SYS_REGS;

   /* The IBM RGB52x cursor is always 8 bits so shift 8, not 10 */

#if 0
   /* Background color */
   s3OutIBMRGBIndReg(IBMRGB_curs_col1_r, 0, (pCurs->backRed   >> 8));
   s3OutIBMRGBIndReg(IBMRGB_curs_col1_g, 0, (pCurs->backGreen >> 8));
   s3OutIBMRGBIndReg(IBMRGB_curs_col1_b, 0, (pCurs->backBlue  >> 8));

   /* Foreground color */
   s3OutIBMRGBIndReg(IBMRGB_curs_col2_r, 0, (pCurs->foreRed   >> 8));
   s3OutIBMRGBIndReg(IBMRGB_curs_col2_g, 0, (pCurs->foreGreen >> 8));
   s3OutIBMRGBIndReg(IBMRGB_curs_col2_b, 0, (pCurs->foreBlue  >> 8));
#else   
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_col1_r); 
   outb(IBMRGB_INDEX_DATA, (pCurs->backRed   >> 8));
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_col1_g); 
   outb(IBMRGB_INDEX_DATA, (pCurs->backGreen >> 8));
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_col1_b); 
   outb(IBMRGB_INDEX_DATA, (pCurs->backBlue  >> 8));
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_col2_r); 
   outb(IBMRGB_INDEX_DATA, (pCurs->foreRed   >> 8));
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_col2_g); 
   outb(IBMRGB_INDEX_DATA, (pCurs->foreGreen >> 8));
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_col2_b); 
   outb(IBMRGB_INDEX_DATA, (pCurs->foreBlue  >> 8));
   outb(vgaCRReg, tmp);
#endif

   LOCK_SYS_REGS;
   return;
}

void 
s3IBMRGBLoadCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int x, y;
{
   int   indx2 = pScr->myNum;
   register int   i;
   unsigned char *ram, *p, tmp, tmp2, tmpcurs;
   extern int s3InitCursorFlag;

   if (!xf86VTSema)
      return;

   if (!pCurs)
      return;

   /* turn the cursor off */
   if ((tmpcurs = s3InIBMRGBIndReg(IBMRGB_curs)) & 0x03)
      s3IBMRGBCursorOff();

   /* load colormap */
   s3IBMRGBRecolorCursor(pScr, pCurs);

   ram = (unsigned char *)pCurs->bits->devPriv[indx2];

   UNLOCK_SYS_REGS;
   BLOCK_CURSOR;

#if 0
   /* position cursor off screen */
   s3OutIBMRGBIndReg(IBMRGB_curs_hot_x, 0, 0);
   s3OutIBMRGBIndReg(IBMRGB_curs_hot_y, 0, 0);
   s3OutIBMRGBIndReg(IBMRGB_curs_xl, 0, 0xff);
   s3OutIBMRGBIndReg(IBMRGB_curs_xh, 0, 0x7f);
   s3OutIBMRGBIndReg(IBMRGB_curs_yl, 0, 0xff);
   s3OutIBMRGBIndReg(IBMRGB_curs_yh, 0, 0x7f);

   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01);

   tmp2 = inb(IBMRGB_INDEX_CONTROL) & 0xfe;
   outb(IBMRGB_INDEX_CONTROL, tmp2 | 1);  /* enable auto-increment */
   
   outb(IBMRGB_INDEX_HIGH, IBMRGB_curs_array >> 8);
   outb(IBMRGB_INDEX_LOW,  IBMRGB_curs_array);

   p = ram;
   for (i = 0; i < 1024; i++,p++)
      outb(IBMRGB_INDEX_DATA, (*p));

   outb(IBMRGB_INDEX_HIGH, 0);
   outb(IBMRGB_INDEX_CONTROL, tmp2);  /* disable auto-increment */
   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, tmp);
#else
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_hot_x);  outb(IBMRGB_INDEX_DATA, 0);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_hot_y);  outb(IBMRGB_INDEX_DATA, 0);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_xl);     outb(IBMRGB_INDEX_DATA, 0xff);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_xh);     outb(IBMRGB_INDEX_DATA, 0x7f);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_yl);     outb(IBMRGB_INDEX_DATA, 0xff);
   outb(IBMRGB_INDEX_LOW,IBMRGB_curs_yh);     outb(IBMRGB_INDEX_DATA, 0x7f);

   /* 
    * Output the cursor data.  The realize function has put the planes into
    * their correct order, so we can just blast this out.
    */
   tmp2 = inb(IBMRGB_INDEX_CONTROL) & 0xfe;
   outb(IBMRGB_INDEX_CONTROL, tmp2 | 1);  /* enable auto-increment */
   
   outb(IBMRGB_INDEX_HIGH, (unsigned char) (IBMRGB_curs_array >> 8));
   outb(IBMRGB_INDEX_LOW,  (unsigned char) IBMRGB_curs_array);

   p = ram;
   for (i = 0; i < 1024; i++,p++)
      outb(IBMRGB_INDEX_DATA, (*p));

   outb(IBMRGB_INDEX_HIGH, 0);
   outb(IBMRGB_INDEX_CONTROL, tmp2);  /* disable auto-increment */
   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, tmp);
#endif
   UNBLOCK_CURSOR;
   LOCK_SYS_REGS;

   /* position cursor */
   s3IBMRGBMoveCursor(0, x, y);

   /* turn the cursor on */
   if ((tmpcurs & 0x03) || s3InitCursorFlag)
      s3IBMRGBCursorOn();

   if (s3InitCursorFlag)
      s3InitCursorFlag = FALSE;

   return;
}



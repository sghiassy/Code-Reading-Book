/*
 * $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3Cursor.c,v 3.5.2.2 1997/05/16 11:35:16 hohndel Exp $
 *
 * Copyright 1991 MIPS Computer Systems, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of MIPS not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  MIPS makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * MIPS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL MIPS
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Modified by Amancio Hasty and Jon Tombs
 *
 */
/* $XConsortium: s3Cursor.c /main/3 1996/10/25 14:10:33 kaleb $ */

/*
 * Device independent (?) part of HW cursor support
 */

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
#include "xf86.h"
#include "inputstr.h"
#include "mfb.h"
#include "mi.h"
#include "xf86Priv.h"
#include "xf86_Option.h"
#include "xf86_OSlib.h"
#include "vga.h"
#include "s3v.h"

#define MAX_CURS 64

static Bool s3RealizeCursor();
static Bool s3UnrealizeCursor();
static void s3SetCursor();
static void s3MoveCursor();

static miPointerSpriteFuncRec s3PointerSpriteFuncs =
{
   s3RealizeCursor,
   s3UnrealizeCursor,
   s3SetCursor,
   s3MoveCursor,
};

extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern xf86InfoRec xf86Info;
extern unsigned char s3SwapBits[256];

static int s3CursGeneration = -1;
static CursorPtr s3SaveCursors[MAXSCREENS];
static Bool useSWCursor = FALSE;
extern Bool tmp_useSWCursor;

extern int s3hotX, s3hotY;

#define VerticalRetraceWait() \
{ \
   outb(vgaCRIndex, 0x17); \
   if ( inb(vgaCRReg) & 0x80 ) { \
       while ((inb(vgaIOBase + 0x0A) & 0x08) == 0x00) ; \
       while ((inb(vgaIOBase + 0x0A) & 0x08) == 0x08) ; \
       while ((inb(vgaIOBase + 0x0A) & 0x08) == 0x00) ; \
       }\
}

Bool
s3CursorInit(pm, pScr)
     char *pm;
     ScreenPtr pScr;
{
   s3BlockCursor = FALSE;
   s3ReloadCursor = FALSE;

   if (s3CursGeneration != serverGeneration) {
      s3hotX = 0;
      s3hotY = 0;
      miDCInitialize (pScr, &xf86PointerScreenFuncs);
      if (OFLG_ISSET(OPTION_SW_CURSOR, &s3InfoRec.options)) {
	 useSWCursor = TRUE;
      } else {
         if (!(miPointerInitialize(pScr, &s3PointerSpriteFuncs,
				   &xf86PointerScreenFuncs, FALSE)))
            return FALSE;
      }
      if (!useSWCursor)
	 pScr->RecolorCursor = s3RecolorCursor;

      s3CursGeneration = serverGeneration;
   }

   return TRUE;
}

void
s3ShowCursor()
{
   if (useSWCursor)
      return;

   /* Nothing to do for integrated cursor */
}

void
s3HideCursor()
{
   unsigned char tmp;

   if (useSWCursor || tmp_useSWCursor)
      return;

   /* turn cursor off */
   outb(vgaCRIndex, 0x45);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp & 0xFE);
}

static Bool
s3RealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;

{
   register int i, j;
   unsigned short *pServMsk;
   unsigned short *pServSrc;
   int   index = pScr->myNum;
   pointer *pPriv = &pCurs->bits->devPriv[index];
   int   wsrc, h;
   unsigned short *ram;
   CursorBitsPtr bits = pCurs->bits;
   int new_useSWCursor;

   if (OFLG_ISSET(OPTION_SW_CURSOR, &s3InfoRec.options))
      return TRUE;

   if (bits->height > MAX_CURS || bits->width > MAX_CURS) {
      extern miPointerSpriteFuncRec miSpritePointerFuncs;
      return (miSpritePointerFuncs.RealizeCursor)(pScr, pCurs);
   }

   if (pCurs->bits->refcnt > 1)
      return TRUE;

   ram = (unsigned short *)xalloc(1024);
   *pPriv = (pointer) ram;

   if (!ram)
      return FALSE;

   pServSrc = (unsigned short *)bits->source;
   pServMsk = (unsigned short *)bits->mask;

   h = bits->height;
   if (h > MAX_CURS)
      h = MAX_CURS;

   wsrc = PixmapBytePad(bits->width, 1);	/* words per line */

   for (i = 0; i < MAX_CURS; i++) {
      for (j = 0; j < MAX_CURS / 16; j++) {
	 unsigned short mask, source;

	 if (i < h && j < wsrc / 2) {
	    mask = *pServMsk++;
	    source = *pServSrc++;

	    ((char *)&mask)[0] = s3SwapBits[((unsigned char *)&mask)[0]];
	    ((char *)&mask)[1] = s3SwapBits[((unsigned char *)&mask)[1]];

	    ((char *)&source)[0] = s3SwapBits[((unsigned char *)&source)[0]];
	    ((char *)&source)[1] = s3SwapBits[((unsigned char *)&source)[1]];

	    if (j < MAX_CURS / 8) { /* j < MAX_CURS / 16 implies this */
	       *ram++ = ~mask;
	       *ram++ = source & mask;
	    }
	 } else {
	    *ram++ = 0xffff;
	    *ram++ = 0x0;
	 }
      }
      if (j < wsrc / 2) {
	 pServMsk += (wsrc/2 - j);
	 pServSrc += (wsrc/2 - j);
      }
   }
   return TRUE;
}

static Bool
s3UnrealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   pointer priv;

   if (pCurs->bits->height > MAX_CURS || pCurs->bits->width > MAX_CURS) {
      extern miPointerSpriteFuncRec miSpritePointerFuncs;
      return (miSpritePointerFuncs.UnrealizeCursor)(pScr, pCurs);
   }

   if (pCurs->bits->refcnt <= 1 &&
       (priv = pCurs->bits->devPriv[pScr->myNum]))
      xfree(priv);
   return TRUE;
}

static void
s3LoadCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int x, y;
{
   int   index = pScr->myNum;
   int   i;
   int   n, ram_loc;
   unsigned short *ram;
   unsigned char tmp;
   int cpos;
   char * videobuffer = (char *) s3VideoMem;
   char * cursorbuffer;

   if (!xf86VTSema)
      return;

   if (!pCurs)
      return;

   outb(vgaCRIndex, 0x39);
   outb(vgaCRReg, 0xa5);

   /* Wait for vertical retrace */
   VerticalRetraceWait();

   /* turn cursor off */
   outb(vgaCRIndex, 0x45);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp & 0xFE);

   /* move cursor off-screen */
   outb(vgaCRIndex, 0x46);
   outb(vgaCRReg, 0xff);
   outb(vgaCRIndex, 0x47);
   outb(vgaCRReg, 0x7f);
   outb(vgaCRIndex, 0x49);
   outb(vgaCRReg, 0xff);
   outb(vgaCRIndex, 0x4e);
   outb(vgaCRReg, 0x3f);
   outb(vgaCRIndex, 0x4f);
   outb(vgaCRReg, 0x3f);
   outb(vgaCRIndex, 0x48);
   outb(vgaCRReg, 0x7f);

   /* Load storage location.  */
   cpos = (s3BppDisplayWidth * s3CursorStartY + s3CursorStartX) / 1024;
   outb(vgaCRIndex, 0x4d);
   outb(vgaCRReg, (0xff & cpos));
   outb(vgaCRIndex, 0x4c);
   outb(vgaCRReg, (0xff00 & cpos) >> 8);

   ram = (unsigned short *)pCurs->bits->devPriv[index];

   BLOCK_CURSOR;
   /* s3 stuff */
   WaitIdle();

   WaitQueue(4);
   SET_SCISSORS(0,0,(s3DisplayWidth - 1), s3ScissB);

   WaitIdle();

   /*
    * SM [07/03/97]:
    * We change the cursor location now to be at the top of vidmem - 1K.
    * This solves problems on OS/2.
    * We also now use MemToBus to copy the cursor bits, as this eliminates alignement
    * and size problems with 24bpp (1024/3 *3 != 1024!!)
    * 
    */
   cursorbuffer = &videobuffer[s3CursorStartX + s3CursorStartY*s3BppDisplayWidth];
   n = 1024;

   if (!(s3InfoRec.modes->Flags & V_DBLSCAN)) {
       MemToBus(cursorbuffer, (char *) ram, n);
       }
/* The following code is for doublesacn modes.
 */
#define S3CURSORRAMWIDTH 16   
   else {
      ram_loc = 0;
      for(i = 0; i < n; i += 2*S3CURSORRAMWIDTH){
	MemToBus((char *) (cursorbuffer + i),(char *)(ram + ram_loc), 
		S3CURSORRAMWIDTH);
 	MemToBus((char *) (cursorbuffer + i + S3CURSORRAMWIDTH),
		(char *)(ram + ram_loc), S3CURSORRAMWIDTH);
	ram_loc += S3CURSORRAMWIDTH;
	}
      }

   UNBLOCK_CURSOR;

   /* Wait for vertical retrace */
   VerticalRetraceWait();

   /* position cursor */
   s3MoveCursor(0, x, y);

   s3RecolorCursor(pScr, pCurs, TRUE);

   /* turn cursor on */
   outb(vgaCRIndex, 0x45);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp | 0x01);
}

static void
s3SetCursor(pScr, pCurs, x, y, generateEvent)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int   x, y;
     Bool  generateEvent;

{
   int index = pScr->myNum;

   if (!pCurs)
      return;

   if (useSWCursor)
      return;

   if (pCurs->bits->height > MAX_CURS || pCurs->bits->width > MAX_CURS) {
      extern miPointerSpriteFuncRec miSpritePointerFuncs;
      s3HideCursor();
      tmp_useSWCursor = TRUE;
      (miSpritePointerFuncs.SetCursor)(pScr, pCurs, x, y);
      return;
   }
   if (tmp_useSWCursor) {  /* hide mi cursor */
      extern miPointerSpriteFuncRec miSpritePointerFuncs;
      (miSpritePointerFuncs.MoveCursor)(pScr, -9999, -9999);  /* XXX */
   }
   tmp_useSWCursor = FALSE;

   s3hotX = pCurs->bits->xhot;
   s3hotY = pCurs->bits->yhot;
   s3SaveCursors[index] = pCurs;

   if (!s3BlockCursor) {
      s3LoadCursor(pScr, pCurs, x, y);
   } else
      s3ReloadCursor = TRUE;
}

void
s3RestoreCursor(pScr)
     ScreenPtr pScr;
{
   int index = pScr->myNum;
   int x, y;

   if (useSWCursor || tmp_useSWCursor)
      return;

   s3ReloadCursor = FALSE;
   miPointerPosition(&x, &y);
   s3LoadCursor(pScr, s3SaveCursors[index], x, y);
}

void
s3RepositionCursor(pScr)
     ScreenPtr pScr;
{
   int x, y;

   if (useSWCursor || tmp_useSWCursor)
      return;

   miPointerPosition(&x, &y);
   /* Wait for vertical retrace */
   VerticalRetraceWait();
   s3MoveCursor(pScr, x, y);
}

static void
s3MoveCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
   unsigned char xoff, yoff;
   extern int s3AdjustCursorXPos;

   if (useSWCursor)
      return;

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
   y -= s3InfoRec.frameY0;

   x -= s3hotX;
   y -= s3hotY;

   if (s3Bpp > 2)
      x &= ~1;

   /*
    * Make these even when used.  There is a bug/feature on at least
    * some chipsets that causes a "shadow" of the cursor in interlaced
    * mode.  Making this even seems to have no visible effect, so just
    * do it for the generic case.
    */
   if (x < 0) {
     xoff = ((-x) & 0xFE);
     x = 0;
   } else {
     xoff = 0;
   }

   if (y < 0) {
      yoff = ((-y) & 0xFE);
      y = 0;
   } else {
      yoff = 0;
   }

   if (s3InfoRec.modes->Flags & V_DBLSCAN)
	y *= 2;
   WaitIdle();

   /* This is the recomended order to move the cursor */
   outb(vgaCRIndex, 0x46);
   outb(vgaCRReg, (x & 0xff00)>>8);

   outb(vgaCRIndex, 0x47);
   outb(vgaCRReg, (x & 0xff));

   outb(vgaCRIndex, 0x49);
   outb(vgaCRReg, (y & 0xff));

   outb(vgaCRIndex, 0x4e);
   outb(vgaCRReg, xoff);

   outb(vgaCRIndex, 0x4f);
   outb(vgaCRReg, yoff);

   outb(vgaCRIndex, 0x48);
   outb(vgaCRReg, (y & 0xff00)>>8);
}

void
s3RenewCursorColor(pScr)
   ScreenPtr pScr;
{
   if (!xf86VTSema)
      return;

   if (s3SaveCursors[pScr->myNum])
      s3RecolorCursor(pScr, s3SaveCursors[pScr->myNum], TRUE);
}

void
s3RecolorCursor(pScr, pCurs, displayed)
     ScreenPtr pScr;
     CursorPtr pCurs;
     Bool displayed;
{
   ColormapPtr pmap;
   unsigned short packedcolfg, packedcolbg;
   xColorItem sourceColor, maskColor;

   if (!xf86VTSema || tmp_useSWCursor) {
      miRecolorCursor(pScr, pCurs, displayed);
      return;
   }

   if (useSWCursor)
      return;

   if (!displayed)
      return;

   switch (s3InfoRec.bitsPerPixel) {
   case 8:
      s3GetInstalledColormaps(pScr, &pmap);
      sourceColor.red = pCurs->foreRed;
      sourceColor.green = pCurs->foreGreen;
      sourceColor.blue = pCurs->foreBlue;
      FakeAllocColor(pmap, &sourceColor);
      maskColor.red = pCurs->backRed;
      maskColor.green = pCurs->backGreen;
      maskColor.blue = pCurs->backBlue;
      FakeAllocColor(pmap, &maskColor);
      FakeFreeColor(pmap, sourceColor.pixel);
      FakeFreeColor(pmap, maskColor.pixel);

      outb(vgaCRIndex, 0x45);
      inb(vgaCRReg);	/* reset stack pointer */
      outb(vgaCRIndex, 0x4A);
      outb(vgaCRReg, sourceColor.pixel);
      outb(vgaCRReg, sourceColor.pixel);
      outb(vgaCRIndex, 0x45);
      inb(vgaCRReg);	/* reset stack pointer */
      outb(vgaCRIndex, 0x4B);
      outb(vgaCRReg, maskColor.pixel);
      outb(vgaCRReg, maskColor.pixel);
      break;
   case 16:
      if (s3InfoRec.depth == 15 && !S3_ViRGE_VX_SERIES(s3ChipId)) {
	 packedcolfg = ((pCurs->foreRed   & 0xf800) >>  1)
	    | ((pCurs->foreGreen & 0xf800) >>  6)
	       | ((pCurs->foreBlue  & 0xf800) >> 11);
	 packedcolbg = ((pCurs->backRed   & 0xf800) >>  1)
	    | ((pCurs->backGreen & 0xf800) >>  6)
	       | ((pCurs->backBlue  & 0xf800) >> 11);
      } else {
	 packedcolfg = ((pCurs->foreRed   & 0xf800) >>  0)
	    | ((pCurs->foreGreen & 0xfc00) >>  5)
	       | ((pCurs->foreBlue  & 0xf800) >> 11);
	 packedcolbg = ((pCurs->backRed   & 0xf800) >>  0)
	    | ((pCurs->backGreen & 0xfc00) >>  5)
	       | ((pCurs->backBlue  & 0xf800) >> 11);
      }
      outb(vgaCRIndex, 0x45);
      inb(vgaCRReg);		/* reset stack pointer */
      outb(vgaCRIndex, 0x4A);
      outb(vgaCRReg, packedcolfg);
      outb(vgaCRReg, packedcolfg>>8);
      outb(vgaCRIndex, 0x45);
      inb(vgaCRReg);		/* reset stack pointer */
      outb(vgaCRIndex, 0x4B);
      outb(vgaCRReg, packedcolbg);
      outb(vgaCRReg, packedcolbg>>8);
      break;
   case 24:
   case 32:
      outb(vgaCRIndex, 0x45);
      inb(vgaCRReg);		/* reset stack pointer */
      outb(vgaCRIndex, 0x4A);
      outb(vgaCRReg, pCurs->foreBlue >>8);
      outb(vgaCRReg, pCurs->foreGreen>>8);
      outb(vgaCRReg, pCurs->foreRed  >>8);

      outb(vgaCRIndex, 0x45);
      inb(vgaCRReg);		/* reset stack pointer */
      outb(vgaCRIndex, 0x4B);
      outb(vgaCRReg, pCurs->backBlue >>8);
	 outb(vgaCRReg, pCurs->backGreen>>8);
      outb(vgaCRReg, pCurs->backRed  >>8);
      break;
   }
}

void
s3WarpCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
   if (xf86VTSema) {
      /* Wait for vertical retrace */
      VerticalRetraceWait();
   }
   miPointerWarpCursor(pScr, x, y);
   xf86Info.currentScreen = pScr;
}

void
s3QueryBestSize(class, pwidth, pheight, pScreen)
     int class;
     unsigned short *pwidth;
     unsigned short *pheight;
     ScreenPtr pScreen;
{
   if (*pwidth > 0) {
      switch (class) {
         case CursorShape:
	    if (*pwidth > 64)
	       *pwidth = 64;
	    if (*pheight > 64)
	       *pheight = 64;
	    break;
         default:
	    mfbQueryBestSize(class, pwidth, pheight, pScreen);
	    break;
      }
   }
}

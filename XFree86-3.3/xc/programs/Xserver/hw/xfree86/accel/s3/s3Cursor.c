/*
 * $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3Cursor.c,v 3.32 1997/01/08 20:33:41 dawes Exp $
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
/* $XConsortium: s3Cursor.c /main/13 1996/10/24 07:11:02 kaleb $ */

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
#include "s3.h"
#include "regs3.h"

#define MAX_CURS 64

static void 
s3LoadCursor(
#if NeedFunctionPrototypes
     ScreenPtr	/* pScr */,
     CursorPtr	/* pCurs */,
     int	/* x */,
     int	/* y */
#endif
);
static Bool s3RealizeCursor(
#if NeedFunctionPrototypes
     ScreenPtr	/* pScr */,
     CursorPtr	/* pCurs */
#endif
);
static Bool s3UnrealizeCursor(
#if NeedFunctionPrototypes
     ScreenPtr	/* pScr */,
     CursorPtr	/* pCurs */
#endif
);
static void s3SetCursor(
#if NeedFunctionPrototypes
     ScreenPtr	/* pScr */,
     CursorPtr	/* pCurs */,
     int  	/* x */,
     int  	/* y */
#endif
);
static void s3MoveCursor(
#if NeedFunctionPrototypes
     ScreenPtr	/* pScr */,
     int  	/* x */,
     int  	/* y */
#endif
);

static miPointerSpriteFuncRec s3PointerSpriteFuncs =
{
   s3RealizeCursor,
   s3UnrealizeCursor,
   s3SetCursor,
   s3MoveCursor,
};

static miPointerSpriteFuncRec s3BtPointerSpriteFuncs =
{
   s3BtRealizeCursor,
   s3UnrealizeCursor,
   s3SetCursor,
   s3BtMoveCursor,
};

static miPointerSpriteFuncRec s3TiPointerSpriteFuncs =
{
   s3TiRealizeCursor,
   s3UnrealizeCursor,
   s3SetCursor,
   s3TiMoveCursor,
};

static miPointerSpriteFuncRec s3Ti3026PointerSpriteFuncs =
{
   s3Ti3026RealizeCursor,
   s3UnrealizeCursor,
   s3SetCursor,
   s3Ti3026MoveCursor,
};

static miPointerSpriteFuncRec s3IBMRGBPointerSpriteFuncs =
{
   s3IBMRGBRealizeCursor,
   s3UnrealizeCursor,
   s3SetCursor,
   s3IBMRGBMoveCursor,
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
      } else if (OFLG_ISSET(OPTION_BT485_CURS, &s3InfoRec.options)) {
         if (!(miPointerInitialize(pScr, &s3BtPointerSpriteFuncs,
				   &xf86PointerScreenFuncs, FALSE)))
            return FALSE;
      } else if (OFLG_ISSET(OPTION_TI3020_CURS, &s3InfoRec.options)) {
         if (!(miPointerInitialize(pScr, &s3TiPointerSpriteFuncs,
				   &xf86PointerScreenFuncs, FALSE)))
            return FALSE;
      } else if (OFLG_ISSET(OPTION_TI3026_CURS, &s3InfoRec.options)) {
         if (!(miPointerInitialize(pScr, &s3Ti3026PointerSpriteFuncs,
				   &xf86PointerScreenFuncs, FALSE)))
            return FALSE;
      } else if (OFLG_ISSET(OPTION_IBMRGB_CURS, &s3InfoRec.options)) {
         if (!(miPointerInitialize(pScr, &s3IBMRGBPointerSpriteFuncs,
				   &xf86PointerScreenFuncs, FALSE)))
            return FALSE;
      } else if (s3InfoRec.bitsPerPixel == 32 
		 && S3_928_SERIES(s3ChipId) && !S3_x64_SERIES(s3ChipId)) {
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
   if (useSWCursor || tmp_useSWCursor) 
      return;

   if (OFLG_ISSET(OPTION_BT485_CURS, &s3InfoRec.options))
      s3BtCursorOn();
   else if (OFLG_ISSET(OPTION_TI3020_CURS, &s3InfoRec.options))
      s3TiCursorOn();
   else if (OFLG_ISSET(OPTION_TI3026_CURS, &s3InfoRec.options))
      s3Ti3026CursorOn();
   else if (OFLG_ISSET(OPTION_IBMRGB_CURS, &s3InfoRec.options))
      s3IBMRGBCursorOn();
   /* Nothing to do for integrated cursor */
}

void
s3HideCursor()
{
   unsigned char tmp;

   if (useSWCursor || tmp_useSWCursor) 
      return;

   if (OFLG_ISSET(OPTION_BT485_CURS, &s3InfoRec.options))
      s3BtCursorOff();
   else if (OFLG_ISSET(OPTION_TI3020_CURS, &s3InfoRec.options))
      s3TiCursorOff();
   else if (OFLG_ISSET(OPTION_TI3026_CURS, &s3InfoRec.options))
      s3Ti3026CursorOff();
   else if (OFLG_ISSET(OPTION_IBMRGB_CURS, &s3InfoRec.options))
      s3IBMRGBCursorOff();
   else {
   /* turn cursor off */
   outb(vgaCRIndex, 0x45);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp & 0xFE);
   }
}

static Bool
s3RealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;

{
   register int i, j;
   unsigned short *pServMsk;
   unsigned short *pServSrc;
   int   indx2 = pScr->myNum;
   pointer *pPriv = &pCurs->bits->devPriv[indx2];
   int   wsrc, h;
   unsigned short *ram;
   CursorBitsPtr bits = pCurs->bits;

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
   int   indx2 = pScr->myNum;
   int   i;
   int   n, bytes_remaining, xpos, ypos, ram_loc;
   unsigned short *ram;
   unsigned char tmp;
   int cpos;

   if (!xf86VTSema)
      return;

   if (!pCurs)
      return;

   UNLOCK_SYS_REGS;

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

   ram = (unsigned short *)pCurs->bits->devPriv[indx2];

   BLOCK_CURSOR;
   /* s3 stuff */
   WaitIdle();

   WaitQueue(4);
   SET_SCISSORS(0,0,(s3DisplayWidth - 1), s3ScissB);

   WaitIdle();

   /*
    * This form is general enough for any valid DisplayWidth.  The only
    * assumption is that it is even.
    */
   xpos = s3CursorStartX;
   ypos = s3CursorStartY;
   bytes_remaining = 1024;
   ram_loc = 0;
   while (bytes_remaining > 0) {
      if (s3BppDisplayWidth - xpos < bytes_remaining)
         n = s3BppDisplayWidth - xpos;
      else
         n = bytes_remaining;

#define S3CURSORRAMWIDTH 16
   if (s3InfoRec.modes->Flags & V_DBLSCAN) {
      char *ram_tmp = (char *) (ram + ram_loc);
      for (i = 0; i < n/2; i += S3CURSORRAMWIDTH) {
      (*s3ImageWriteFunc)((xpos+i*2)/s3Bpp, ypos, S3CURSORRAMWIDTH/s3Bpp, 1,
			  ram_tmp, S3CURSORRAMWIDTH, 0, 0, MIX_SRC, ~0);
      (*s3ImageWriteFunc)((xpos+S3CURSORRAMWIDTH+i*2)/s3Bpp, ypos, S3CURSORRAMWIDTH/s3Bpp, 1,
			  ram_tmp, S3CURSORRAMWIDTH, 0, 0, MIX_SRC, ~0);
      ram_tmp += S3CURSORRAMWIDTH;
      }
   } else
      (*s3ImageWriteFunc)(xpos/s3Bpp, ypos, n/s3Bpp, 1,
			  (char *)(ram + ram_loc), n, 0, 0,
			  MIX_SRC, ~0);
      if (s3InfoRec.modes->Flags & V_DBLSCAN) 
         ram_loc += n/4;
      else
         ram_loc += n/2;
      ypos++;
      xpos = 0;
      bytes_remaining -= n;
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

   LOCK_SYS_REGS;
}

static void
s3SetCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int   x, y;
{
   int indx2 = pScr->myNum;

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
      tmp_useSWCursor = FALSE;      
      s3ShowCursor();
   }

   s3hotX = pCurs->bits->xhot;
   s3hotY = pCurs->bits->yhot;
   s3SaveCursors[indx2] = pCurs;

   if (!s3BlockCursor) {
      if (OFLG_ISSET(OPTION_BT485_CURS, &s3InfoRec.options))
         s3BtLoadCursor(pScr, pCurs, x, y);
      else if (OFLG_ISSET(OPTION_TI3020_CURS, &s3InfoRec.options))
         s3TiLoadCursor(pScr, pCurs, x, y);
      else if (OFLG_ISSET(OPTION_TI3026_CURS, &s3InfoRec.options))
         s3Ti3026LoadCursor(pScr, pCurs, x, y);
      else if (OFLG_ISSET(OPTION_IBMRGB_CURS, &s3InfoRec.options))
         s3IBMRGBLoadCursor(pScr, pCurs, x, y);
      else
         s3LoadCursor(pScr, pCurs, x, y);
   } else
      s3ReloadCursor = TRUE;
}

void
s3RestoreCursor(pScr)
     ScreenPtr pScr;
{
   int indx2 = pScr->myNum;
   int x, y;

   if (useSWCursor || tmp_useSWCursor) 
      return;

   s3ReloadCursor = FALSE;
   miPointerPosition(&x, &y);
   if (OFLG_ISSET(OPTION_BT485_CURS, &s3InfoRec.options))
      s3BtLoadCursor(pScr, s3SaveCursors[indx2], x, y);
   else if (OFLG_ISSET(OPTION_TI3020_CURS, &s3InfoRec.options))
      s3TiLoadCursor(pScr, s3SaveCursors[indx2], x, y);
   else if (OFLG_ISSET(OPTION_TI3026_CURS, &s3InfoRec.options))
      s3Ti3026LoadCursor(pScr, s3SaveCursors[indx2], x, y);
   else if (OFLG_ISSET(OPTION_IBMRGB_CURS, &s3InfoRec.options))
      s3IBMRGBLoadCursor(pScr, s3SaveCursors[indx2], x, y);
   else
      s3LoadCursor(pScr, s3SaveCursors[indx2], x, y);
}

void
s3RepositionCursor(pScr)
     ScreenPtr pScr;
{
   int x, y;

   if (useSWCursor || tmp_useSWCursor)
      return;

   miPointerPosition(&x, &y);
   if (OFLG_ISSET(OPTION_BT485_CURS, &s3InfoRec.options))
      s3BtMoveCursor(pScr, x, y);
   else if (OFLG_ISSET(OPTION_TI3020_CURS, &s3InfoRec.options))
      s3TiMoveCursor(pScr, x, y);
   else if (OFLG_ISSET(OPTION_TI3026_CURS, &s3InfoRec.options))
      s3Ti3026MoveCursor(pScr, x, y);
   else if (OFLG_ISSET(OPTION_IBMRGB_CURS, &s3InfoRec.options))
      s3IBMRGBMoveCursor(pScr, x, y);
   else {
      /* Wait for vertical retrace */
      VerticalRetraceWait();
      s3MoveCursor(pScr, x, y);
   }
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

   if (!S3_TRIOxx_SERIES(s3ChipId)) {
      if (S3_968_SERIES(s3ChipId))
	 x *= (2 * s3Bpp);
      else if (!S3_x64_SERIES(s3ChipId) && !S3_805_I_SERIES(s3ChipId)) 
	 x *= s3Bpp;
      else if (s3Bpp > 2)
	 x *= 2;
   }

   x -= s3hotX;
   y -= s3hotY;

   if (S3_968_SERIES(s3ChipId))
      x -= x % (2 * s3Bpp);
   else if (!S3_x64_SERIES(s3ChipId) && !S3_805_I_SERIES(s3ChipId))
      x -= x % s3Bpp;
   else if (s3Bpp > 2)
      x &= ~1;

   UNLOCK_SYS_REGS;

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

   LOCK_SYS_REGS;
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

   if (OFLG_ISSET(OPTION_BT485_CURS, &s3InfoRec.options))
      s3BtRecolorCursor(pScr, pCurs);
   else if (OFLG_ISSET(OPTION_TI3020_CURS, &s3InfoRec.options))
      s3TiRecolorCursor(pScr, pCurs);
   else if (OFLG_ISSET(OPTION_TI3026_CURS, &s3InfoRec.options))
      s3Ti3026RecolorCursor(pScr, pCurs);
   else if (OFLG_ISSET(OPTION_IBMRGB_CURS, &s3InfoRec.options))
      s3IBMRGBRecolorCursor(pScr, pCurs);
   else {
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

	 if (S3_TRIOxx_SERIES(s3ChipId)) {
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
	 }
	 else {
	    outb(vgaCRIndex, 0x0E);
	    outb(vgaCRReg, sourceColor.pixel);
	    outb(vgaCRIndex, 0x0F);
	    outb(vgaCRReg, maskColor.pixel);
	 }
	 break;
      case 16:
	 if (s3InfoRec.depth == 15) {
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

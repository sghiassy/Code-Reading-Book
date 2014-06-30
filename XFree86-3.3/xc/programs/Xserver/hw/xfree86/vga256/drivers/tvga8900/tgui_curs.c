/* $XConsortium: tgui_curs.c /main/10 1996/10/25 15:39:18 kaleb $ */
/*
 * Copyright 1994  The XFree86 Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * DAVID WEXELBLAT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Adapted for Trident by Alan Hourihane <alanh@fairlite.demon.co.uk>.
 *
 * Hardware cursor handling. Adapted from cirrus/cir_cursor.c and
 * accel/s3/s3Cursor.c, and ark/ark_cursor.c
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/tvga8900/tgui_curs.c,v 3.15.2.3 1997/05/31 13:34:46 dawes Exp $ */

#include "X.h"
#include "Xproto.h"
#include "misc.h"
#include "input.h"
#include "cursorstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "servermd.h"
#include "windowstr.h"
#include "xf86.h"
#include "mipointer.h"
#include "xf86Priv.h"
#include "xf86_Option.h"
#include "xf86_OSlib.h"
#include "vga.h"
#include "t89_driver.h"
#include "tgui_ger.h"

extern int tridentHWCursorType;

extern Bool vgaUseLinearAddressing;
extern Bool tridentUseLinear;

static int tridentFlipCursor = 0;
static Bool TridentRealizeCursor();
static Bool TridentUnrealizeCursor();
static void TridentSetCursor();
static void TridentMoveCursor();
static void TridentRecolorCursor();

static miPointerSpriteFuncRec TridentPointerSpriteFuncs =
{
   TridentRealizeCursor,
   TridentUnrealizeCursor,
   TridentSetCursor,
   TridentMoveCursor,
};

/* vga256 interface defines Init, Restore, Warp, QueryBestSize. */


extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern xf86InfoRec xf86Info;

static int TridentCursorGeneration = -1;
static int TridentCursorControlMode;
static int TridentCursorAddress;

/*
 * This is the set variables that defines the cursor state within the
 * driver.
 */

int TridentCursorHotX;
int TridentCursorHotY;
int TridentCursorWidth;	/* Must be set before calling TridentCursorInit. */
int TridentCursorHeight;
static CursorPtr TridentCursorpCurs;

/*
 * This is a high-level init function, called once; it passes a local
 * miPointerSpriteFuncRec with additional functions that we need to provide.
 * It is called by the SVGA server.
 */

Bool TridentCursorInit(pm, pScr)
	char *pm;
	ScreenPtr pScr;
{
	if (TridentCursorGeneration != serverGeneration) {
		if (!(miPointerInitialize(pScr, &TridentPointerSpriteFuncs,
		&xf86PointerScreenFuncs, FALSE)))
			return FALSE;

		TridentCursorHotX = 0;
		TridentCursorHotY = 0;
		pScr->RecolorCursor = TridentRecolorCursor;
		TridentCursorGeneration = serverGeneration;
	}

	/*
	 * Define the Trident cursor mode.
	 */
	TridentCursorControlMode = 0x40;	/* X11 Mode */
	if (TridentCursorWidth == 64)
		/* 64x64 cursor. */
		TridentCursorControlMode |= 0x01;

	/* Pop the cursor in the last 1KB aligned segment */
	TridentCursorAddress = vga256InfoRec.videoRam * 1024 - 
				(IsCyber ? 4096 : 1024);

	return TRUE;
}

/*
 * This enables displaying of the cursor by the Trident graphics chip.
 */

static void TridentShowCursor() {
   outb(0x3c8, 0x00);		/* DAC color 0 */
   outb(0x3c9, 0x00);
   outb(0x3c9, 0x00);
   outb(0x3c9, 0x00);

   outb (0x3c8, 0xFF);		/* DAC color 255 */
   outb (0x3c9, 0x3F);
   outb (0x3c9, 0x3F);
   outb (0x3c9, 0x3F);
	if (tridentHWCursorType == 2)
	{
		outb(GER_INDEX, 0x34);
		outb(GER_BYTE2, 0x01);	/* Enable Cursor in GER */
		outb(GER_INDEX, 0x78);
		outb(GER_BYTE0, TridentCursorWidth - 1); 
		outb(GER_BYTE2, TridentCursorHeight - 1);
	} else 
	wrinx(vgaIOBase + 4, 0x50, TridentCursorControlMode | 0x80);
}

/*
 * This disables displaying of the cursor by the Trident graphics chip.
 */

void TridentHideCursor() {
	if (tridentHWCursorType == 2)
	{
		outb(GER_INDEX, 0x34);
		outb(GER_BYTE2, 0x00);	/* Disable Cursor in GER */
	} else
	wrinx(vgaIOBase + 4, 0x50, TridentCursorControlMode & 0x7F);
}

/*
 * This function is called when a new cursor image is requested by
 * the server. The main thing to do is convert the bitwise image
 * provided by the server into a format that the graphics card
 * can conveniently handle, and store that in system memory.
 * Adapted from accel/s3/s3Cursor.c.
 */

static Bool TridentRealizeCursor(pScr, pCurs)
	ScreenPtr pScr;
	CursorPtr pCurs;
{
   register int i, j;
   unsigned long *pServMsk;
   unsigned long *pServSrc;
   int   index = pScr->myNum;
   pointer *pPriv = &pCurs->bits->devPriv[index];
   unsigned long *ram;
   int h;
   CursorBitsPtr bits = pCurs->bits;

   if (pCurs->bits->refcnt > 1)
      return TRUE;

   ram = (unsigned long *)xalloc((IsCyber ? 4096 : 1024));

   *pPriv = (pointer) ram;

   if (!ram)
      return FALSE;

   pServSrc = (unsigned long *)bits->source;
   pServMsk = (unsigned long *)bits->mask;

   h = bits->height;
   if (h > TridentCursorHeight)
	h = TridentCursorHeight;

   for (i = 0; i < h; i++) {
	unsigned long m, s;

	m = *pServMsk++;
	((char *)&m)[0] = byte_reversed[((unsigned char *)&m)[0]];
	((char *)&m)[1] = byte_reversed[((unsigned char *)&m)[1]];
	((char *)&m)[2] = byte_reversed[((unsigned char *)&m)[2]];
	((char *)&m)[3] = byte_reversed[((unsigned char *)&m)[3]];

	s = *pServSrc++;
	((char *)&s)[0] = byte_reversed[((unsigned char *)&s)[0]];
	((char *)&s)[1] = byte_reversed[((unsigned char *)&s)[1]];
	((char *)&s)[2] = byte_reversed[((unsigned char *)&s)[2]];
	((char *)&s)[3] = byte_reversed[((unsigned char *)&s)[3]];

	*ram++ = m;
	if (tridentFlipCursor == 0)
		*ram++ = ~s; 
	else
		*ram++ = s;
   }
   return TRUE;
}

/*
 * This is called when a cursor is no longer used. The intermediate
 * cursor image storage that we created needs to be deallocated.
 */

static Bool TridentUnrealizeCursor(pScr, pCurs)
	ScreenPtr pScr;
	CursorPtr pCurs;
{
	pointer priv;

	if (pCurs->bits->refcnt <= 1 &&
	(priv = pCurs->bits->devPriv[pScr->myNum])) {
		xfree(priv);
		pCurs->bits->devPriv[pScr->myNum] = 0x0;
	}
	return TRUE;
}

/*
 * This function uploads a cursor image to the video memory of the
 * graphics card. The source image has already been converted by the
 * Realize function to a format that can be quickly transferred to
 * the card.
 * This is a local function that is not called from outside of this
 * module.
 */

extern void TGUISetWrite();

static void TridentLoadCursorToCard(pScr, pCurs, x, y)
	ScreenPtr pScr;
	CursorPtr pCurs;
{
	unsigned char *cursor_image;
	int index = pScr->myNum;

	if (!xf86VTSema)
		return;

	cursor_image = pCurs->bits->devPriv[index];

	if (vgaUseLinearAddressing)
		memcpy((unsigned char *)vgaLinearBase + TridentCursorAddress,
			cursor_image, (IsCyber ? 4096 : 1024));
	else {
		vgaSaveBank();
		TGUISetWrite(TridentCursorAddress >> 16);
		memcpy((unsigned char *)vgaBase + (TridentCursorAddress & 0xFFFF),
			cursor_image, (IsCyber ? 4096 : 1024));
		vgaRestoreBank();
	}
}

/*
 * This function should make the graphics chip display new cursor that
 * has already been "realized". We need to upload it to video memory,
 * make the graphics chip display it.
 * This is a local function that is not called from outside of this
 * module (although it largely corresponds to what the SetCursor
 * function in the Pointer record needs to do).
 */

static void TridentLoadCursor(pScr, pCurs, x, y)
	ScreenPtr pScr;
	CursorPtr pCurs;
	int x, y;
{
	if (!xf86VTSema)
		return;

	if (!pCurs)
		return;

	/* Remember the cursor currently loaded into this cursor slot. */
	TridentCursorpCurs = pCurs;

	TridentHideCursor();

	/* Program the cursor image address in video memory. */
	if (tridentHWCursorType == 2)
	{
		outb(GER_INDEX, 0x74);
		outb(GER_BYTE0, TridentCursorAddress & 0x000000FF);
		outb(GER_BYTE1, (TridentCursorAddress & 0x0000FF00) >> 8);
		outb(GER_BYTE2, (TridentCursorAddress & 0x00FF0000) >> 16);
		outb(GER_BYTE3, (TridentCursorAddress & 0xFF000000) >> 24);
	} 
	else
	{
 		wrinx(vgaIOBase + 4, 0x44, 
			((TridentCursorAddress/1024) & 0x00FF));
		wrinx(vgaIOBase + 4, 0x45, 
			((TridentCursorAddress/1024) & 0xFF00) >> 8);
	}

	TridentRecolorCursor(pScr, pCurs, 1);

	TridentLoadCursorToCard(pScr, pCurs, x, y);

	/* Position cursor */
	TridentMoveCursor(pScr, x, y);

	/* Turn it on. */
	TridentShowCursor();
}

/*
 * This function should display a new cursor at a new position.
 */

static void TridentSetCursor(pScr, pCurs, x, y, generateEvent)
	ScreenPtr pScr;
	CursorPtr pCurs;
	int x, y;
	Bool generateEvent;
{
	if (!pCurs)
		return;

	TridentCursorHotX = pCurs->bits->xhot;
	TridentCursorHotY = pCurs->bits->yhot;

	TridentLoadCursor(pScr, pCurs, x, y);
}

/*
 * This function should redisplay a cursor that has been
 * displayed earlier. It is called by the SVGA server.
 */

void TridentRestoreCursor(pScr)
	ScreenPtr pScr;
{
	int x, y;

	miPointerPosition(&x, &y);

	TridentLoadCursor(pScr, TridentCursorpCurs, x, y);
}

/*
 * This function is called when the current cursor is moved. It makes
 * the graphic chip display the cursor at the new position.
 */

static void TridentMoveCursor(pScr, x, y)
	ScreenPtr pScr;
	int x, y;
{
	int xorigin, yorigin;

	if (!xf86VTSema)
		return;

	x -= vga256InfoRec.frameX0 + TridentCursorHotX;
	y -= vga256InfoRec.frameY0 + TridentCursorHotY;

	/*
	 * If the cursor is partly out of screen at the left or top,
	 * we need set the origin.
	 */
	xorigin = 0;
	yorigin = 0;
	if (x < 0) {
		xorigin = -x;
		x = 0;
	}
	if (y < 0) {
		yorigin = -y;
		y = 0;
	}

	if (XF86SCRNINFO(pScr)->modes->Flags & V_DBLSCAN)
		y *= 2;

	/* Program the cursor origin (offset into the cursor bitmap). */
	if (tridentHWCursorType == 1)
	{
		wrinx(vgaIOBase + 4, 0x46, xorigin);
		wrinx(vgaIOBase + 4, 0x47, yorigin);
	}

	/* Program the new cursor position. */
	if (tridentHWCursorType == 2)
	{
		outb(GER_INDEX, 0x30);
		outb(GER_BYTE0, x);
		outb(GER_BYTE1, x >> 8);
		outb(GER_BYTE3, y);
		outb(GER_INDEX, 0x34);
		outb(GER_BYTE0, y >> 8);
	}
	else
	{
		wrinx(vgaIOBase + 4, 0x40, x);		/* Low byte. */
		wrinx(vgaIOBase + 4, 0x41, x >> 8);	/* High byte. */
		wrinx(vgaIOBase + 4, 0x42, y);		/* Low byte. */
		wrinx(vgaIOBase + 4, 0x43, y >> 8);	/* High byte. */
	}
}

/*
 * This is a local function that programs the colors of the cursor
 * on the graphics chip.
 * Adapted from accel/s3/s3Cursor.c.
 */

static void
TridentRecolorCursor(pScr, pCurs, displayed)
     ScreenPtr pScr;
     CursorPtr pCurs;
     Bool displayed;
{
   unsigned short bred, bgreen, bblue;
   unsigned short fred, fgreen, fblue;
   int shift;
   int i;
   VisualPtr pVisual;
   unsigned char AddColReg;

   if (!xf86VTSema)
       return;

   if (!displayed)
       return;

   /* Find the PseudoColour or TrueColor visual for the colour mapping
    * function
    */

   for (i = 0, pVisual = pScr->visuals; i < pScr->numVisuals; i++, pVisual++)
     {
       if ((pVisual->class == PseudoColor) || (pVisual->class == TrueColor))
	 break;
     }

   if (i == pScr->numVisuals)
     {
       ErrorF ("TRIDENT: Failed to find a visual for mapping hardware cursor colours\n");
       return;
     }

   shift = 16 - pVisual->bitsPerRGBValue;

   bred   = pCurs->backRed;
   bgreen = pCurs->backGreen;
   bblue  = pCurs->backBlue;

   pScr->ResolveColor (&bred, &bgreen, &bblue, pVisual);

   fred   = pCurs->foreRed;
   fgreen = pCurs->foreGreen;
   fblue  = pCurs->foreBlue;

   pScr->ResolveColor (&fred, &fgreen, &fblue, pVisual);

#if 0
   outb(0x3c8, 0x00);		/* DAC color 0 */
   outb(0x3c9, fred>>shift);
   outb(0x3c9, fgreen>>shift);
   outb(0x3c9, fblue>>shift);

   outb (0x3c8, 0xFF);		/* DAC color 255 */
   outb (0x3c9, bred>>shift);
   outb (0x3c9, bgreen>>shift);
   outb (0x3c9, bblue>>shift);
#else
	tridentFlipCursor = fred>>shift;
	TridentRealizeCursor(pScr, pCurs);
#endif

   if (tridentHWCursorType == 2)
   {
	outb(GER_INDEX, 0x38);
	outb(GER_BYTE0, 0x00);
	outb(GER_BYTE1, 0xFF);
   }
   else
   if (TVGAchipset == TGUI96xx)
   {
	/* We've got specific colours now for the cursor */

	wrinx(vgaIOBase + 4, 0x48, 0xFF);
	wrinx(vgaIOBase + 4, 0x49, 0xFF);
	wrinx(vgaIOBase + 4, 0x4A, 0xFF);
	wrinx(vgaIOBase + 4, 0x4B, 0xFF);
	wrinx(vgaIOBase + 4, 0x4C, 0x00);
	wrinx(vgaIOBase + 4, 0x4D, 0x00);
	wrinx(vgaIOBase + 4, 0x4E, 0x00);
	wrinx(vgaIOBase + 4, 0x4F, 0x00);
   }
}

/*
 * This doesn't do very much. It just calls the mi routine. It is called
 * by the SVGA server.
 */

void TridentWarpCursor(pScr, x, y)
	ScreenPtr pScr;
	int x, y;
{
	miPointerWarpCursor(pScr, x, y);
	xf86Info.currentScreen = pScr;
}

/*
 * This function is called by the SVGA server. It returns the
 * size of the hardware cursor that we support when asked for.
 * It is called by the SVGA server.
 */

void TridentQueryBestSize(class, pwidth, pheight, pScreen)
	int class;
	unsigned short *pwidth;
	unsigned short *pheight;
	ScreenPtr pScreen;
{
 	if (*pwidth > 0) {
 		if (class == CursorShape) {
			*pwidth = TridentCursorWidth;
			*pheight = TridentCursorHeight;
		}
		else
			(void) mfbQueryBestSize(class, pwidth, pheight, pScreen);
	}
}



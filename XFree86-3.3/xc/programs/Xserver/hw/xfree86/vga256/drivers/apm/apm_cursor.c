/* $XConsortium: apm_cursor.c /main/3 1996/10/25 07:01:53 kaleb $ */




/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/apm/apm_cursor.c,v 3.2.2.1 1997/05/31 13:34:41 dawes Exp $ */

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

extern Bool vgaUseLinearAddressing;

static Bool ApmRealizeCursor();
static Bool ApmUnrealizeCursor();
static void ApmSetCursor();
static void ApmMoveCursor();
static void ApmRecolorCursor();

static miPointerSpriteFuncRec apmPointerSpriteFuncs =
{
   ApmRealizeCursor,
   ApmUnrealizeCursor,
   ApmSetCursor,
   ApmMoveCursor,
};

/* vga256 interface defines Init, Restore, Warp, QueryBestSize. */


extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern xf86InfoRec xf86Info;

static int              apmCursorGeneration = -1;
static unsigned char    apmCursorControlMode;
static int              apmCursorAddress;

/*
 * This is the set variables that defines the cursor state within the
 * driver.
 */

int apmCursorHotX;
int apmCursorHotY;
int apmCursorWidth;	/* Must be set before calling ApmCursorInit. */
int apmCursorHeight;
static CursorPtr apmCursorpCurs;

extern void wrxl();
extern void wrxw();
extern void wrxb();

/*
 * This is a high-level init function, called once; it passes a local
 * miPointerSpriteFuncRec with additional functions that we need to provide.
 * It is called by the SVGA server.
 */

Bool ApmCursorInit(pm, pScr)
	char *pm;
	ScreenPtr pScr;
{
	if (apmCursorGeneration != serverGeneration) {
		if (!(miPointerInitialize(pScr, &apmPointerSpriteFuncs,
		&xf86PointerScreenFuncs, FALSE)))
			return FALSE;

		apmCursorHotX = 0;
		apmCursorHotY = 0;
	}

	apmCursorGeneration = serverGeneration;

	apmCursorControlMode = 0;
	apmCursorAddress = vga256InfoRec.videoRam * 1024 - 35 * 1024;

	return TRUE;
}

/*
 * This enables displaying of the cursor by the APM graphics chip.
 * It's a local function, it's not called from outside of the module.
 */

static void ApmShowCursor() {
	/* Enable the hardware cursor. */
	wrxb(0x140, apmCursorControlMode | 1);
}

/*
 * This disables displaying of the cursor by the APM graphics chip.
 * This is also a local function, it's not called from outside.
 */

void ApmHideCursor() {
	/* Disable the hardware cursor. */
	wrxb(0x140, apmCursorControlMode);
}

/*
 * This function is called when a new cursor image is requested by
 * the server. The main thing to do is convert the bitwise image
 * provided by the server into a format that the graphics card
 * can conveniently handle, and store that in system memory.
 */

static Bool ApmRealizeCursor(pScr, pCurs)
	ScreenPtr pScr;
	CursorPtr pCurs;
{
   register int i, j;
   unsigned char *pServMsk;
   unsigned char *pServSrc;
   int   index = pScr->myNum;
   pointer *pPriv = &pCurs->bits->devPriv[index];
   int   w, h, stride;
   unsigned char *ram, *dst, v;
   CursorBitsPtr bits = pCurs->bits;

   if (pCurs->bits->refcnt > 1)
      return TRUE;

   ram = (unsigned char *)xalloc(1024);
   *pPriv = (pointer) ram;

   if (!ram)
      return FALSE;
   memset(ram, 0xaa, 64 * 64 / 4);

   pServSrc = (unsigned char *)bits->source;
   pServMsk = (unsigned char *)bits->mask;

#define MAX_CURS 64

	h = pCurs->bits->height;
	if (h > MAX_CURS) h = MAX_CURS;
	w = pCurs->bits->width;
	if (w > MAX_CURS) w = MAX_CURS;
	stride = ((pCurs->bits->width + 31) / 32) * 4;
	for (i = 0; i < h; ++i) {
		pServSrc = pCurs->bits->source + stride * i;
		pServMsk = pCurs->bits->mask + stride * i;
		dst = ram + i * 16;
		for (j = 0; j < w; ++j) {
			v = (pServSrc[j / 8] >> (j % 8)) & 1;
			v |= ((pServMsk[j / 8] >> (j % 8)) & 1) << 1;
			v <<= (j & 3) * 2;
			dst[j / 4] ^= v;
		}
	}
   return TRUE;
}

/*
 * This is called when a cursor is no longer used. The intermediate
 * cursor image storage that we created needs to be deallocated.
 */

static Bool ApmUnrealizeCursor(pScr, pCurs)
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

extern void ApmSetWrite();

static void ApmLoadCursorToCard(pScr, pCurs, x, y)
	ScreenPtr pScr;
	CursorPtr pCurs;
	int x, y;	/* Not used for APM. */
{
	unsigned char *cursor_image;
	int index = pScr->myNum;

	if (!xf86VTSema)
		return;

	cursor_image = pCurs->bits->devPriv[index];

	if (vgaUseLinearAddressing)
		memcpy((unsigned char *)vgaLinearBase + apmCursorAddress,
			cursor_image, 1024);
	else {
		/*
		 * The cursor can only be in the last 16K of video memory,
		 * which fits in the last banking window.
		 */
		vgaSaveBank();
		ApmSetWrite(apmCursorAddress >> 16);
		memcpy((unsigned char *)vgaBase + (apmCursorAddress & 0xFFFF),
			cursor_image, 1024);
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

static void ApmLoadCursor(pScr, pCurs, x, y)
	ScreenPtr pScr;
	CursorPtr pCurs;
	int x, y;
{
	if (!xf86VTSema)
		return;

	if (!pCurs)
		return;

	/* Remember the cursor currently loaded into this cursor slot. */
	apmCursorpCurs = pCurs;

	ApmHideCursor();

	/* Program the cursor image address in video memory. */
	/* We use the last slot (the last 256 bytes of video memory). */
	wrxw(0x144, vga256InfoRec.videoRam - 33);

	ApmLoadCursorToCard(pScr, pCurs, x, y);

	ApmRecolorCursor(pScr, pCurs, 1);

	/* Position cursor */
	ApmMoveCursor(pScr, x, y);

	/* Turn it on. */
	ApmShowCursor();
}

/*
 * This function should display a new cursor at a new position.
 */

static void ApmSetCursor(pScr, pCurs, x, y, generateEvent)
	ScreenPtr pScr;
	CursorPtr pCurs;
	int x, y;
	Bool generateEvent;
{
	if (!pCurs)
		return;

	apmCursorHotX = pCurs->bits->xhot;
	apmCursorHotY = pCurs->bits->yhot;

	ApmLoadCursor(pScr, pCurs, x, y);
}

/*
 * This function should redisplay a cursor that has been
 * displayed earlier. It is called by the SVGA server.
 */

void ApmRestoreCursor(pScr)
	ScreenPtr pScr;
{
	int x, y;

	miPointerPosition(&x, &y);

	ApmLoadCursor(pScr, apmCursorpCurs, x, y);
}

/*
 * This function is called when the current cursor is moved. It makes
 * the graphic chip display the cursor at the new position.
 */

static void ApmMoveCursor(pScr, x, y)
	ScreenPtr pScr;
	int x, y;
{
	int xorigin, yorigin;

	if (!xf86VTSema)
		return;

	x -= vga256InfoRec.frameX0 + apmCursorHotX;
	y -= vga256InfoRec.frameY0 + apmCursorHotY;

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
	wrxb(0x14c, xorigin);
	wrxb(0x14d, yorigin);

	/* Program the new cursor position. */
	wrxw(0x148, x);
	wrxw(0x14a, y);
}

/*
 * This is a local function that programs the colors of the cursor
 * on the graphics chip.
 * Adapted from accel/s3/s3Cursor.c.
 */

static void ApmRecolorCursor(pScr, pCurs, displayed)
	ScreenPtr pScr;
	CursorPtr pCurs;
	Bool displayed;
{
 	ColormapPtr pmap;
	unsigned short packedcolfg, packedcolbg;
	xColorItem sourceColor, maskColor;

	if (!xf86VTSema)
		return;

	switch (vgaBitsPerPixel) {
	case 8:
		wrxb(0x141, 0);  /* XXXX Fixed colors, debugging only. */
		wrxb(0x142, 1);
		break;
	case 16:
	case 32:
		packedcolfg = ((pCurs->foreRed & 0xe000) >> 8)
			| ((pCurs->foreGreen & 0xe000) >> 11)
			| ((pCurs->foreBlue & 0xc000) >> 14);
		packedcolbg = ((pCurs->backRed & 0xe000) >> 8)
			| ((pCurs->backGreen & 0xe000) >> 11)
			| ((pCurs->backBlue  & 0xc000) >> 14);

		wrxb(0x141, packedcolfg);
		wrxb(0x142, packedcolbg);
		break;
	}
}

/*
 * This doesn't do very much. It just calls the mi routine. It is called
 * by the SVGA server.
 */

void ApmWarpCursor(pScr, x, y)
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

void ApmQueryBestSize(class, pwidth, pheight, pScreen)
	int class;
	unsigned short *pwidth;
	unsigned short *pheight;
	ScreenPtr pScreen;
{
 	if (*pwidth > 0) {
 		if (class == CursorShape) {
			*pwidth = apmCursorWidth;
			*pheight = apmCursorHeight;
		}
		else
			(void) mfbQueryBestSize(class, pwidth, pheight, pScreen);
	}
}



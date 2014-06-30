/* $XConsortium: ct_cursor.c /main/4 1996/10/27 11:49:18 kaleb $ */

/*
 *
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Simon P. Cooper not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Simon P. Cooper makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * SIMON P. COOPER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL SIMON P. COOPER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_cursor.c,v 3.8.2.2 1997/05/31 13:34:42 dawes Exp $ */

/*
 * Hardware cursor handling. Adapted from cirrus/cir_cursor.c and
 * ark/ark_cursor.c.
 */

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
#include "ct_driver.h"

static Bool CHIPSRealizeCursor();
static Bool CHIPSUnrealizeCursor();
static void CHIPSSetCursor();
static void CHIPSMoveCursor();
static void CHIPSRecolorCursor();
static void CHIPSHideCursor();

#ifdef DEBUG
unsigned char *hex2bmp(unsigned char, unsigned char *);
#endif

static miPointerSpriteFuncRec CHIPSPointerSpriteFuncs =
{
    CHIPSRealizeCursor,
    CHIPSUnrealizeCursor,
    CHIPSSetCursor,
    CHIPSMoveCursor,
};

extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern xf86InfoRec xf86Info;

static int ctCursorGeneration = -1;
Bool ctHWcursorShown = FALSE;
int ctRealizedCursorCount = 0;

/*
 * This is the set variables that defines the cursor state within the
 * driver.
 */

int ctCursorHotX = 0;
int ctCursorHotY = 0;
int ctCursorClipX = 1;
int ctCursorClipY = 1;
int ctCursorWidth;
int ctCursorHeight;
static CursorPtr ctCursorpCurs;

Bool
CHIPSCursorInit(pm, pScr)
    char *pm;
    ScreenPtr pScr;
{

    CHIPSHideCursor();
    if (ctCursorGeneration != serverGeneration) {
	if (!(miPointerInitialize(pScr, &CHIPSPointerSpriteFuncs,
		    &xf86PointerScreenFuncs, FALSE)))
	    return FALSE;

	ctCursorHotX = 0;
	ctCursorHotY = 0;
    }
    ctCursorGeneration = serverGeneration;

    /* 1kB alignment. Note this allocation is now done in FbInit. */
    /* load address to card when cursor is loaded to card */
    if (ctisHiQV32) {
	outb(0x3D6, 0xA2);
	outb(0x3D7, (ctCursorAddress >> 8) & 0xFF);
	outb(0x3D6, 0xA3);
	outb(0x3D7, (ctCursorAddress >> 16) & 0x3F);
    } else if (!ctisWINGINE) {
      if(!ctUseMMIO) {
	HW_DEBUG(0xC);
	outl(DR(0xC), ctCursorAddress);
      } else {
	HW_DEBUG(MR(0xC));
	MMIOmeml(MR(0xC)) = ctCursorAddress;
      }
    }
    return TRUE;
}

void
CHIPSShowCursor()
{
    /* turn the cursor on */
    if (ctisHiQV32)
	outw(0x3D6, 0x11A0);
    else
      if(!ctUseMMIO) {
	HW_DEBUG(0x8);
	outw(DR(0x8), 0x21);
      } else {
	HW_DEBUG(DR(8));
	MMIOmemw(MR(8)) = 0x21;
      }
    ctHWcursorShown = TRUE;
}

void
CHIPSHideCursor()
{
    /* turn the cursor off */
    if (ctisHiQV32)
	outw(0x3D6, 0x10A0);
    else
      if(!ctUseMMIO) {
	HW_DEBUG(0x8);
	outw(DR(0x8), 0x20);
      } else {
	HW_DEBUG(DR(0x8));
	MMIOmemw(DR(0x8)) = 0x20;
      }
    ctHWcursorShown = FALSE;
}

/*
 * This function is called when a new cursor image is requested by
 * the server. The main thing to do is convert the bitwise image
 * provided by the server into a format that the graphics card
 * can conveniently handle, and store that in system memory.
 * Adapted from ark/ark_cursor.c.
 */

static Bool
CHIPSRealizeCursor(pScr, pCurs)
    ScreenPtr pScr;
    CursorPtr pCurs;
{
    register int i, j, k;
    unsigned char *pServMsk;
    unsigned char *pServSrc;
    int index = pScr->myNum;
    pointer *pPriv = &pCurs->bits->devPriv[index];
    int wsrc, h;
    unsigned char *ram;
    CursorBitsPtr bits = pCurs->bits;

#ifdef DEBUG
    unsigned char bmp[] = "12345678";

#endif

    if (pCurs->bits->refcnt > 1)
	return TRUE;

    ram = (unsigned char *)xalloc(1024);
    *pPriv = (pointer) ram;
    if (!ram)
	return FALSE;

    pServSrc = (unsigned char *)bits->source;
    pServMsk = (unsigned char *)bits->mask;

#define MAX_CURS 32

    ctCursorWidth = bits->width;
    ctCursorHeight = h = bits->height;

    if (h > MAX_CURS)
	h = MAX_CURS;

    wsrc = PixmapBytePad(bits->width, 1);	/* Bytes per line. */

    if (!ctisHiQV32) {
      if (ctisWINGINE) {
	unsigned char *tmpMsk = pServMsk;

	for (i = 0; i < MAX_CURS; i++) {
	    for (j = 0; j < MAX_CURS / 8; j++) {
		unsigned char mask;

		if (i < h && j < wsrc) {
		    mask = *pServMsk++;

		    if (j < MAX_CURS / 8) {
			*ram++ = ~byte_reversed[mask];	/*chip depend */
#ifdef DEBUG
			ErrorF("m:%s\n", hex2bmp(mask, bmp));
#endif
		    }
		} else {
		    *ram++ = 0xFF;     /*chip depend */
		}
	    }
	    /*
	     * if we still have more bytes on this line (j < wsrc),
	     * we have to ignore the rest of the line.
	     */
	    while (j++ < wsrc)
		pServMsk++;
	  }
	 for (i = 0; i < MAX_CURS; i++) {
	    for (j = 0; j < MAX_CURS / 8; j++) {
		unsigned char source;
		unsigned char tmp;
		
		if (i < h && j < wsrc) {
		    source = *pServSrc++;
		    tmp = (unsigned char)(*tmpMsk++);
		    if (j < MAX_CURS / 8) {
		      *ram++ = byte_reversed[(tmp & source)];	/*chip depend */
#ifdef DEBUG
			ErrorF("s:%s\n", hex2bmp(tmp & source, bmp));
#endif
		    }
		} else {
		    *ram++ = 0x00;     /*chip depend */
		}
	    }
	    /*
	     * if we still have more bytes on this line (j < wsrc),
	     * we have to ignore the rest of the line.
	     */
	    while (j++ < wsrc) {
		pServSrc++;
		tmpMsk++;
            }
	  }
    } else {
      for (i = 0; i < MAX_CURS; i++) {
	    for (j = 0; j < MAX_CURS / 8; j++) {
		unsigned char mask, source;
		unsigned char tmp;

		if (i < h && j < wsrc) {
		    mask = *pServMsk++;
		    source = *pServSrc++;

		    if (j < MAX_CURS / 8) {
			*ram++ = ~byte_reversed[mask];	/*chip depend */
			tmp = ~mask ^ source;
			*ram++ = byte_reversed[(tmp & source)];	/*chip depend */
#ifdef DEBUG
			ErrorF("m:%s\n", hex2bmp(mask, bmp));
			ErrorF("s:%s\n", hex2bmp(source, bmp));
#endif
		    }
		} else {
		    *ram++ = 0xFF;     /*chip depend */
		    *ram++ = 0x00;     /*chip depend */
		}
	    }
	    /*
	     * if we still have more bytes on this line (j < wsrc),
	     * we have to ignore the rest of the line.
	     */
	    while (j++ < wsrc)
		pServMsk++, pServSrc++;
	  }
    }
    } else {
        unsigned char *tmpMsk = pServMsk;
	for (i = 0; i < MAX_CURS; i += 2) {
	    for (j = 0; j < 2; j++) {
		for (k = 0; k < MAX_CURS / 8; k++) {
		    unsigned char mask;

		    if ((i + j) < h && k < wsrc) {
			mask = *pServMsk++;

			*ram++ = ~byte_reversed[mask];	/*chip depend */
#ifdef DEBUG
			ErrorF("m:%s\n", hex2bmp(mask, bmp));
#endif
		    } else {
			*ram++ = 0xFF; /*chip depend */
		    }
		}
		/*
		 * if we still have more bytes on this line (j < wsrc),
		 * we have to ignore the rest of the line.
		 */
		while (k++ < wsrc)
		    pServMsk++;
	    }
	    for (j = 0; j < 2; j++) {
		for (k = 0; k < MAX_CURS / 8; k++) {
		    unsigned char source;

		    if ((i + j) < h && k < wsrc) {
			source = *pServSrc++;
			*ram++ = byte_reversed[((*tmpMsk++) & source)];	/*chip depend */
#ifdef DEBUG
			ErrorF("s:%s\n", hex2bmp(source, bmp));
#endif
		    } else {
			*ram++ = 0x00; /*chip depend */
		    }
		}
		/*
		 * if we still have more bytes on this line (j < wsrc),
		 * we have to ignore the rest of the line.
		 */
		while (k++ < wsrc) {
		    pServSrc++;
		    tmpMsk++;
                }
	    }
	}
    }

    ctRealizedCursorCount++;	       /* to erase cursor when exit the server */

    return TRUE;
}

/*
 * This is called when a cursor is no longer used. The intermediate
 * cursor image storage that we created needs to be deallocated.
 */
static Bool
CHIPSUnrealizeCursor(pScr, pCurs)
    ScreenPtr pScr;
    CursorPtr pCurs;
{
    pointer priv;

    if (pCurs->bits->refcnt <= 1 &&
	(priv = pCurs->bits->devPriv[pScr->myNum])) {
	xfree(priv);
	pCurs->bits->devPriv[pScr->myNum] = 0x0;
    }
    if ((--ctRealizedCursorCount == 0) && xf86VTSema) {/* count down to erase cursor when exit the server */
	CHIPSHideCursor();
    }
    return TRUE;
}

CHIPSLoadCursorToCard(pScr, pCurs, x, y)
    ScreenPtr pScr;
    CursorPtr pCurs;
    int x, y;
{
    int i, j;
    unsigned char *cursor_image;
    int index = pScr->myNum;

    if (!xf86VTSema)
	return;

    cursor_image = pCurs->bits->devPriv[index];
    if (ctisWINGINE) {
      outl(DR(0x8),0x20);
      for (i=0;i<64;i++) {
	  outl(DR(0xC),*(unsigned long *)cursor_image);
#ifdef DEBUG1
	  ErrorF("CURSOR: %X\n",*(unsigned long *)cursor_image);
#endif
	  (cursor_image)+= sizeof (unsigned long);
      }
    } else {
    if (ctLinearSupport) {
#ifdef DEBUG
	ErrorF("CHIPSLoadCursorToCard: memcpy to 0x%X\n",
	    (unsigned char *)vgaLinearBase + ctCursorAddress);
#endif
	memcpy((unsigned char *)vgaLinearBase + ctCursorAddress,
	    cursor_image, 256);
#ifdef DEBUG1
	for(i=0;i<256;i++){
	  ErrorF("CURSOR: %X\n",*(unsigned long *)cursor_image);
	  cursor_image += sizeof(unsigned long);
	}
#endif
    } else {
	/*
	 * The cursor can only be in the last 16K of video memory,
	 * which fits in the last banking window.
	 */
	vgaSaveBank();
	if (ctisHiQV32) {
	    CHIPSHiQVSetWrite(ctCursorAddress >> 16);
	    memcpy((unsigned char *)vgaBase + (ctCursorAddress & 0xFFFF),
		cursor_image, 256);
	    outb(0x3D6, 0xA2);
	    outb(0x3D7, ((unsigned int)((ctCursorAddress & 0xFFFF) +
			(unsigned char *)vgaBase) >> 8) & 0xFF);
	    outb(0x3D6, 0xA3);
	    outb(0x3D7, ((unsigned int)((ctCursorAddress & 0xFFFF) +
			(unsigned char *)vgaBase) >> 16) & 0x3F);
	} else {
	    CHIPSSetWrite(ctCursorAddress >> 16);
	    memcpy((unsigned char *)vgaBase + (ctCursorAddress & 0xFFFF),
		cursor_image, 256);
	    if(!ctUseMMIO) {
	      HW_DEBUG(0xC);
	      outl(DR(0xC),
		 (int)((unsigned char *)vgaBase + (ctCursorAddress & 0xFFFF)));
	    } else {
	      HW_DEBUG(MR(0xC));
	      MMIOmeml(MR(0xC)) = (int)(unsigned char *)vgaBase + (ctCursorAddress & 0xFFFF);
	    }
	}
	vgaRestoreBank();

    }
    /* set cursor address here or we loose the cursor on video mode change */
    if (ctisHiQV32) {
	outb(0x3D6, 0xA2);
	outb(0x3D7, (ctCursorAddress >> 8) & 0xFF);
	outb(0x3D6, 0xA3);
	outb(0x3D7, (ctCursorAddress >> 16) & 0x3F);
    } else if (!ctisWINGINE) {
      if (!ctUseMMIO) {
	HW_DEBUG(0xC);
	outl(DR(0xC), ctCursorAddress);
      } else {
	HW_DEBUG(MR(0xC));
	MMIOmeml(MR(0xC)) = ctCursorAddress;
      }
    }
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
static void
CHIPSLoadCursor(pScr, pCurs, x, y)
    ScreenPtr pScr;
    CursorPtr pCurs;
    int x, y;
{
    if (!xf86VTSema)
	return;

    if (!pCurs)
	return;

    /* Remember the cursor currently loaded into this cursor slot. */
    ctCursorpCurs = pCurs;

    CHIPSHideCursor();

    CHIPSLoadCursorToCard(pScr, pCurs, x, y);

    CHIPSRecolorCursor(pScr, pCurs, 1);

    /* Position cursor */
    CHIPSMoveCursor(pScr, x, y);

    /* Turn it on. */
    CHIPSShowCursor();
}

static void
CHIPSSetCursor(pScr, pCurs, x, y, generateEvent)
    ScreenPtr pScr;
    CursorPtr pCurs;
    int x, y;
    Bool generateEvent;
{
    if (!pCurs)
	return;

    ctCursorHotX = pCurs->bits->xhot;
    ctCursorHotY = pCurs->bits->yhot;

    ctCursorClipX = -1 - ctCursorHotX;
    ctCursorClipY = -1 - ctCursorHotY;

    CHIPSLoadCursor(pScr, pCurs, x, y);
}

/*
 * This function should display a new cursor at a new position.
 */
void
CHIPSRestoreCursor(pScr)
    ScreenPtr pScr;
{
    int index = pScr->myNum;
    int x, y;

    miPointerPosition(&x, &y);

    CHIPSLoadCursor(pScr, ctCursorpCurs, x, y);
}

/*
 * This function should redisplay a cursor that has been
 * displayed earlier. It is called by the SVGA server.
 */
static void
CHIPSMoveCursor(pScr, x, y)
    ScreenPtr pScr;
    int x, y;
{
    if (!xf86VTSema)
	return;

    x -= vga256InfoRec.frameX0 + ctCursorHotX;
    y -= vga256InfoRec.frameY0 + ctCursorHotY;

    /*
     * If the cursor is partly out of screen at the left or top,
     * we need set the origin.
     */

    if (x < ctCursorClipX) {
	x = (ctCursorHotX + 1) | 0x8000;
    } else if (x < 0) {
	x = ~(x - 1) | 0x8000;
    }
    if (y < ctCursorClipY) {
	y = (ctCursorHotY + 1) | 0x8000;
    } else if (y < 0) {
	y = ~(y - 1) | 0x8000;
    }
    if (XF86SCRNINFO(pScr)->modes->Flags & V_DBLSCAN)
	y *= 2;

    /* Program the cursor origin (offset into the cursor bitmap). */
    if (ctisHiQV32) {
	outw(0x3D6, ((x & 0xFF) << 8) | 0xA4);
	outw(0x3D6, (x & 0x8700) | 0xA5);
	outw(0x3D6, ((y & 0xFF) << 8) | 0xA6);
	outw(0x3D6, (y & 0x8700) | 0xA7);
    } else {
	unsigned long xy;

	xy = y;
	xy = (xy << 16) | x;
	if(!ctUseMMIO) {
	  HW_DEBUG(0xB);
	  outl(DR(0xB), xy);
	} else {
	  HW_DEBUG(MR(0xB));
	  MMIOmeml(MR(0xB)) = xy;
	}
    }

}

/*
 * This is a local function that programs the colors of the cursor
 * on the graphics chip.
 * Adapted from accel/s3/s3Cursor.c.
 */
static void
CHIPSRecolorCursor(pScr, pCurs, displayed)
    ScreenPtr pScr;
    CursorPtr pCurs;
    Bool displayed;
{
    ColormapPtr pmap;
    unsigned long packedcolfg, packedcolbg;
    xColorItem sourceColor, maskColor;

#ifdef DEBUG
    ErrorF("RecolorCursor: fb: %X fg %X fr:%X\n bb %X bg %X br %X\n",
	pCurs->foreBlue, pCurs->foreGreen, pCurs->foreRed,
	pCurs->backBlue, pCurs->backGreen, pCurs->backRed);
#endif

    if (!xf86VTSema)
	return;

    if (ctisHiQV32) {
	unsigned char xr80;

	outb(0x3D6, 0x80);
	xr80 = inb(0x3D7);
	outb(0x3D7, xr80 | 0x1);       /* Enable extended palette addressing */

	/* Write the new colours to the extended VGA palette. Palette
	 * index is incremented after each write, so only write index
	 * once 
	 */
	outb(0x3C8, 0x4);
	outb(0x3C9, (pCurs->backRed >> 10) & 0xFF);
	outb(0x3C9, (pCurs->backGreen >> 10) & 0xFF);
	outb(0x3C9, (pCurs->backBlue >> 10) & 0xFF);
	outb(0x3C9, (pCurs->foreRed >> 10) & 0xFF);
	outb(0x3C9, (pCurs->foreGreen >> 10) & 0xFF);
	outb(0x3C9, (pCurs->foreBlue >> 10) & 0xFF);
	outb(0x3D6, 0x80);
	outb(0x3D7, xr80);	       /* Enable normal palette addressing */
    } else if (ctisWINGINE) {
	outl(DR(0xA), (((pCurs->backBlue) & 0xff00) >> 8) |
	     (((pCurs->backGreen) & 0xff00)) |
	     (((pCurs->backRed) & 0xff00) << 8));
	outl(DR(0x9), (((pCurs->foreBlue) & 0xff00) >> 8) |
	     (((pCurs->foreGreen) & 0xff00)) |
	     (((pCurs->foreRed) & 0xff00) << 8));
    } else {
	packedcolfg = ((pCurs->foreRed & 0xf800) >> 0)
	    | ((pCurs->foreGreen & 0xfc00) >> 5)
	    | ((pCurs->foreBlue & 0xf800) >> 11);
	packedcolbg = ((pCurs->backRed & 0xf800) >> 0)
	    | ((pCurs->backGreen & 0xfc00) >> 5)
	    | ((pCurs->backBlue & 0xf800) >> 11);
	packedcolfg = (packedcolfg << 16) | packedcolbg;
	if(!ctUseMMIO) {
	  HW_DEBUG(0x9);
	  outl(DR(0x9), packedcolfg);
	} else {
	  MMIOmeml(MR(0x9)) = packedcolfg;
	  HW_DEBUG(MR(0x9));
	}
    }
}

void
CHIPSWarpCursor(pScr, x, y)
    ScreenPtr pScr;
    int x, y;
{
    miPointerWarpCursor(pScr, x, y);
    xf86Info.currentScreen = pScr;
}

void
CHIPSQueryBestSize(class, pwidth, pheight, pScreen)
    int class;
    short *pwidth;
    short *pheight;
    ScreenPtr pScreen;
{
    if (*pwidth > 0) {
	switch (class) {
	case CursorShape:
	    if (*pwidth > ctCursorWidth)
		*pwidth = ctCursorWidth;
	    if (*pheight > ctCursorHeight)
		*pheight = ctCursorHeight;
	    break;

	default:
	    (void)mfbQueryBestSize(class, pwidth, pheight, pScreen);
	    break;
	}
    }
}

#ifdef DEBUG
unsigned char *
hex2bmp(unsigned char val, unsigned char *bmp)
{
    unsigned char *ret = bmp;
    int i;

    for (i = 0; i < 8; i++) {
	*bmp++ = (val & (0x80 >> i)) ? '1' : '.';
    }
    return ret;
}
#endif

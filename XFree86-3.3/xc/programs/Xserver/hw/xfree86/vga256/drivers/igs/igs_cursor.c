/*
 * Copyright 1997
 * Digital Equipment Corporation. All rights reserved.
 * This software is furnished under license and may be used and copied only in 
 * accordance with the following terms and conditions.  Subject to these 
 * conditions, you may download, copy, install, use, modify and distribute 
 * this software in source and/or binary form. No title or ownership is 
 * transferred hereby.
 * 1) Any source code used, modified or distributed must reproduce and retain 
 *    this copyright notice and list of conditions as they appear in the 
 *    source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of Digital 
 *    Equipment Corporation. Neither the "Digital Equipment Corporation" name 
 *    nor any trademark or logo of Digital Equipment Corporation may be used 
 *    to endorse or promote products derived from this software without the 
 *    prior written permission of Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied warranties,
 *    including but not limited to, any implied warranties of merchantability,
 *    fitness for a particular purpose, or non-infringement are disclaimed. In
 *    no event shall DIGITAL be liable for any damages whatsoever, and in 
 *    particular, DIGITAL shall not be liable for special, indirect, 
 *    consequential, or incidental damages or damages for lost profits, loss 
 *    of revenue or loss of use, whether such damages arise in contract, 
 *    negligence, tort, under statute, in equity, at law or otherwise, even if
 *    advised of the possibility of such damage. 
 */
/*
 * The CyberPro2010 driver is based on sample code provided by IGS 
 * Technologies, Inc. http://www.integraphics.com.
 * IGS Technologies, Inc makes no representations about the suitability of 
 * this software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

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
#include "igs_driver.h"

static Bool IGSRealizeCursor();
static Bool IGSUnrealizeCursor();
static void IGSSetCursor();
static void IGSMoveCursor();
static void IGSRecolorCursor();
static void IGSHideCursor();

#define IGS_MAX_CURS 64

/*
** The X server stores it's cursor shape as a source and an XOR mask,
** while the CyberPro chip uses 2 bits per pixel where
**    00 = color 0
**    01 = color 1
**    10 = transparent
**    11 = invert
** This array expands an 8 bit source or mask value into a
** 16 bit value that can be easily converted into the 2 bit format.
** eg. xxxxxxxx becomes x0x0x0x0x0x0x0x0.
*/
unsigned short igsByteExpand[] = 
{
0x0000, 0x0002, 0x0008, 0x000A, 0x0020, 0x0022, 0x0028, 0x002A, 0x0080, 0x0082,
0x0088, 0x008A, 0x00A0, 0x00A2, 0x00A8, 0x00AA, 0x0200, 0x0202, 0x0208, 0x020A,
0x0220, 0x0222, 0x0228, 0x022A, 0x0280, 0x0282, 0x0288, 0x028A, 0x02A0, 0x02A2,
0x02A8, 0x02AA, 0x0800, 0x0802, 0x0808, 0x080A, 0x0820, 0x0822, 0x0828, 0x082A,
0x0880, 0x0882, 0x0888, 0x088A, 0x08A0, 0x08A2, 0x08A8, 0x08AA, 0x0A00, 0x0A02,
0x0A08, 0x0A0A, 0x0A20, 0x0A22, 0x0A28, 0x0A2A, 0x0A80, 0x0A82, 0x0A88, 0x0A8A,
0x0AA0, 0x0AA2, 0x0AA8, 0x0AAA, 0x2000, 0x2002, 0x2008, 0x200A, 0x2020, 0x2022,
0x2028, 0x202A, 0x2080, 0x2082, 0x2088, 0x208A, 0x20A0, 0x20A2, 0x20A8, 0x20AA,
0x2200, 0x2202, 0x2208, 0x220A, 0x2220, 0x2222, 0x2228, 0x222A, 0x2280, 0x2282,
0x2288, 0x228A, 0x22A0, 0x22A2, 0x22A8, 0x22AA, 0x2800, 0x2802, 0x2808, 0x280A,
0x2820, 0x2822, 0x2828, 0x282A, 0x2880, 0x2882, 0x2888, 0x288A, 0x28A0, 0x28A2,
0x28A8, 0x28AA, 0x2A00, 0x2A02, 0x2A08, 0x2A0A, 0x2A20, 0x2A22, 0x2A28, 0x2A2A,
0x2A80, 0x2A82, 0x2A88, 0x2A8A, 0x2AA0, 0x2AA2, 0x2AA8, 0x2AAA, 0x8000, 0x8002,
0x8008, 0x800A, 0x8020, 0x8022, 0x8028, 0x802A, 0x8080, 0x8082, 0x8088, 0x808A,
0x80A0, 0x80A2, 0x80A8, 0x80AA, 0x8200, 0x8202, 0x8208, 0x820A, 0x8220, 0x8222,
0x8228, 0x822A, 0x8280, 0x8282, 0x8288, 0x828A, 0x82A0, 0x82A2, 0x82A8, 0x82AA,
0x8800, 0x8802, 0x8808, 0x880A, 0x8820, 0x8822, 0x8828, 0x882A, 0x8880, 0x8882,
0x8888, 0x888A, 0x88A0, 0x88A2, 0x88A8, 0x88AA, 0x8A00, 0x8A02, 0x8A08, 0x8A0A,
0x8A20, 0x8A22, 0x8A28, 0x8A2A, 0x8A80, 0x8A82, 0x8A88, 0x8A8A, 0x8AA0, 0x8AA2,
0x8AA8, 0x8AAA, 0xA000, 0xA002, 0xA008, 0xA00A, 0xA020, 0xA022, 0xA028, 0xA02A,
0xA080, 0xA082, 0xA088, 0xA08A, 0xA0A0, 0xA0A2, 0xA0A8, 0xA0AA, 0xA200, 0xA202,
0xA208, 0xA20A, 0xA220, 0xA222, 0xA228, 0xA22A, 0xA280, 0xA282, 0xA288, 0xA28A,
0xA2A0, 0xA2A2, 0xA2A8, 0xA2AA, 0xA800, 0xA802, 0xA808, 0xA80A, 0xA820, 0xA822,
0xA828, 0xA82A, 0xA880, 0xA882, 0xA888, 0xA88A, 0xA8A0, 0xA8A2, 0xA8A8, 0xA8AA,
0xAA00, 0xAA02, 0xAA08, 0xAA0A, 0xAA20, 0xAA22, 0xAA28, 0xAA2A, 0xAA80, 0xAA82,
0xAA88, 0xAA8A, 0xAAA0, 0xAAA2, 0xAAA8, 0xAAAA,
};


#ifdef DEBUG
unsigned char *hex2bmp(unsigned char, unsigned char *);
#endif

static miPointerSpriteFuncRec IGSPointerSpriteFuncs =
{
    IGSRealizeCursor,
    IGSUnrealizeCursor,
    IGSSetCursor,
    IGSMoveCursor,
};

extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern xf86InfoRec xf86Info;

static int igsCursorGeneration = -1;
Bool igsHWcursorShown = FALSE;
int igsRealizedCursorCount = 0;

/*
 * This is the set variables that defines the cursor state within the
 * driver.
 */

int igsCursorHotX = 0;
int igsCursorHotY = 0;
int igsCursorClipX = 1;
int igsCursorClipY = 1;
int igsCursorWidth;
int igsCursorHeight;
static CursorPtr igsCursorpCurs;

Bool
IGSCursorInit(pm, pScr)
    char *pm;
    ScreenPtr pScr;
{
    unsigned short loc;
    
    IGSHideCursor();
    if (igsCursorGeneration != serverGeneration) 
    {
	if (!(miPointerInitialize(pScr, &IGSPointerSpriteFuncs,
		    &xf86PointerScreenFuncs, FALSE)))
	    return FALSE;

	igsCursorHotX = 0;
	igsCursorHotY = 0;
    }
    igsCursorGeneration = serverGeneration;

#ifdef DEBUG
    /* It looks like the cursor might be hardcoded to the last 256 */
    /* or 1024 bytes of the frame buffer, depending on cursor type, */
    /* However you may be able to set a value in the sprite data location */
    outb(EXTINDEX, 0x7e);
    loc = inb(EXTDATA);
    outb(EXTINDEX, 0x7f);
    loc |= (inb(EXTDATA) << 8);

    ErrorF("Initial Sprite data location = 0x%x\n", loc);
#endif
    outb(EXTINDEX, 0x7e);
    outb(EXTDATA, ((igsCursorAddress >> 10) & 0xFF));
    outb(EXTINDEX, 0x7f);
    outb(EXTDATA, (((igsCursorAddress >> 10) & 0x0F00) >> 8) | 
	 (inb(EXTDATA) & 0xF0));
#ifdef DEBUG
    outb(EXTINDEX, 0x7e);
    loc = inb(EXTDATA);
    outb(EXTINDEX, 0x7f);
    loc |= (inb(EXTDATA) << 8);

    ErrorF("New Sprite data location = 0x%x\n", loc);
#endif 
    /*
     ** Set up 64x64 cursor type.
     */
    outb(EXTINDEX, 0x52); /* Horizontal Preset */
    outb(EXTDATA, 0x00);
    outb(EXTINDEX, 0x55); /* Vertical Preset */
    outb(EXTDATA, 0x00);
    outb(EXTINDEX, 0x56);  /* Sprite Control */
    outb(EXTDATA, inb(EXTDATA) | 0x02);
    
    return TRUE;
}

void
IGSShowCursor()
{
    int data;
    
    /* turn the cursor on */
    outb(EXTINDEX, 0x56);
    data = inb(EXTDATA) | 0x01;
    igsWaitVSync();
    outb(EXTDATA, data);
    igsHWcursorShown = TRUE;
}

void
IGSHideCursor()
{
    int data;
    
    /* turn the cursor off */
    outb(EXTINDEX, 0x56);
    data = inb(EXTDATA) & ~0x01;
    igsWaitVSync();
    outb(EXTDATA, data);
    
    igsHWcursorShown = FALSE;
}

/*
 * This function is called when a new cursor image is requested by
 * the server. The main thing to do is convert the bitwise image
 * provided by the server into a format that the graphics card
 * can conveniently handle, and store that in system memory.
 * Adapted from ark/ark_cursor.c.
 */

static Bool
IGSRealizeCursor(pScr, pCurs)
    ScreenPtr pScr;
    CursorPtr pCurs;
{
    register int i, j, k;
    unsigned char *pServMsk;
    unsigned char *pServSrc;
    int index = pScr->myNum;
    pointer *pPriv = &pCurs->bits->devPriv[index];
    int wsrc, h;
    unsigned short *ram;
    CursorBitsPtr bits = pCurs->bits;

#ifdef DEBUG
    unsigned char bmp[] = "12345678";
#endif

    if (pCurs->bits->refcnt > 1)
	return TRUE;

    ram = (unsigned short *)xalloc((IGS_MAX_CURS * IGS_MAX_CURS * 2) >> 3);
    *pPriv = (pointer) ram;
    if (!ram)
	return FALSE;

    pServSrc = (unsigned char *)bits->source;
    pServMsk = (unsigned char *)bits->mask;

    igsCursorWidth = bits->width;
    igsCursorHeight = h = bits->height;

    if (h > IGS_MAX_CURS)
	h = IGS_MAX_CURS;

    wsrc = PixmapBytePad(bits->width, 1);	/* Bytes per line. */

    for (i = 0; i < IGS_MAX_CURS; i++) 
    {
        for (j = 0; j < IGS_MAX_CURS / 8; j++) 
        {	    
	    if (i < h && j < wsrc) 
            {
		unsigned char mask, src;

		mask = ~*pServMsk;
		pServMsk++;
                src = *pServSrc;
		pServSrc++;
		
		*ram = igsByteExpand[mask] | (igsByteExpand[src] >> 1);
		ram++;
#ifdef DEBUG
		ErrorF("m:%s ", hex2bmp(mask, bmp));
		ErrorF("s:%s\n", hex2bmp(src, bmp));
#endif
	    } 
            else 
            {
		*ram++ = 0xAAAA; /* transparent - chip depend */
	    }
        }
	/*
	 * if we still have more bytes on this line (j < wsrc),
	 * we have to ignore the rest of the line.
	 */
	while (j++ < wsrc)
	    pServMsk++;
    }
    
    igsRealizedCursorCount++;	       /* to erase cursor when exit the server */

    return TRUE;
}

/*
 * This is called when a cursor is no longer used. The intermediate
 * cursor image storage that we created needs to be deallocated.
 */
static Bool
IGSUnrealizeCursor(pScr, pCurs)
    ScreenPtr pScr;
    CursorPtr pCurs;
{
    pointer priv;

    if (pCurs->bits->refcnt <= 1 &&
	(priv = pCurs->bits->devPriv[pScr->myNum])) {
	xfree(priv);
	pCurs->bits->devPriv[pScr->myNum] = 0x0;
    }
    if ((--igsRealizedCursorCount == 0) && xf86VTSema) 
    {/* count down to erase cursor when exit the server */
	IGSHideCursor();
    }
    return TRUE;
}

IGSLoadCursorToCard(pScr, pCurs, x, y)
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
    if (igsLinearSupport) 
    {
#ifdef DEBUG
        ErrorF("IGSLoadCursorToCard: memcpy to 0x%X\n",
	    (unsigned char *)vgaLinearBase + igsCursorAddress);
#endif

	memcpy((unsigned char *)vgaLinearBase + igsCursorAddress,
	       cursor_image, (IGS_MAX_CURS * IGS_MAX_CURS * 2) >> 3);

#ifdef DEBUG1
	for(i=0;i<256;i++)
	{
	    ErrorF("CURSOR: %X\n",*(unsigned long *)cursor_image);
	    cursor_image += sizeof(unsigned long);
	}
#endif
    } 
    else 
    {
        /*
	 * The cursor can only be in the last 16K of video memory,
	   * which fits in the last banking window.
	   */
	  vgaSaveBank();
	  IGSSetWrite(igsCursorAddress >> 16);
	  memcpy((unsigned char *)vgaBase + (igsCursorAddress & 0xFFFF),
		 cursor_image, (IGS_MAX_CURS * IGS_MAX_CURS * 2) >> 3);
	  vgaRestoreBank();
    }

    /* set cursor address here or we lose the cursor on video mode change */
    outb(EXTINDEX, 0x7e);
    outb(EXTDATA, (igsCursorAddress >> 10) & 0xFF);
    outb(EXTINDEX, 0x7f);
    outb(EXTDATA, (((igsCursorAddress >> 10) & 0x0F00) >> 8) | 
	 (inb(EXTDATA) & 0xF0));
    /*
     ** Set up 64x64 cursor type.
     */
    outb(EXTINDEX, 0x52); /* Horizontal Preset */
    outb(EXTDATA, 0x00);
    outb(EXTINDEX, 0x55); /* Vertical Preset */
    outb(EXTDATA, 0x00);
    outb(EXTINDEX, 0x56);  /* Sprite Control */
    outb(EXTDATA, inb(EXTDATA) | 0x02);
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
IGSLoadCursor(pScr, pCurs, x, y)
    ScreenPtr pScr;
    CursorPtr pCurs;
    int x, y;
{
    if (!xf86VTSema)
	return;

    if (!pCurs)
	return;

    /* Remember the cursor currently loaded into this cursor slot. */
    igsCursorpCurs = pCurs;

    IGSHideCursor();

    IGSLoadCursorToCard(pScr, pCurs, x, y);

    IGSRecolorCursor(pScr, pCurs, 1);

    /* Position cursor */
    IGSMoveCursor(pScr, x, y);

    /* Turn it on. */
    IGSShowCursor();
}

static void
IGSSetCursor(pScr, pCurs, x, y, generateEvent)
    ScreenPtr pScr;
    CursorPtr pCurs;
    int x, y;
    Bool generateEvent;
{
    if (!pCurs)
	return;

    igsCursorHotX = pCurs->bits->xhot;
    igsCursorHotY = pCurs->bits->yhot;

    igsCursorClipX = -1 - igsCursorHotX;
    igsCursorClipY = -1 - igsCursorHotY;

    IGSLoadCursor(pScr, pCurs, x, y);
}

/*
 * This function should display a new cursor at a new position.
 */
void
IGSRestoreCursor(pScr)
    ScreenPtr pScr;
{
    int index = pScr->myNum;
    int x, y;

    miPointerPosition(&x, &y);

    IGSLoadCursor(pScr, igsCursorpCurs, x, y);
}

/*
 * This function should redisplay a cursor that has been
 * displayed earlier. It is called by the SVGA server.
 */
static void
IGSMoveCursor(pScr, x, y)
    ScreenPtr pScr;
    int x, y;
{
    if (!xf86VTSema)
	return;

    x -= vga256InfoRec.frameX0 + igsCursorHotX;
    y -= vga256InfoRec.frameY0 + igsCursorHotY;

    /*
     * If the cursor is partly out of screen at the left or top,
     * we need set the origin.
     */

    if (x < igsCursorClipX) 
    {
	x = (igsCursorHotX + 1) | 0x8000;
    } 
    else if (x < 0) 
    {
	x = ~(x - 1) | 0x8000;
    }
    if (y < igsCursorClipY) 
    {
	y = (igsCursorHotY + 1) | 0x8000;
    } 
    else if (y < 0) 
    {
	y = ~(y - 1) | 0x8000;
    }
    if (XF86SCRNINFO(pScr)->modes->Flags & V_DBLSCAN)
    {
	y *= 2;
    }
    
    /* Program the cursor origin (offset into the cursor bitmap). */
    outb(EXTINDEX, 0x50); /* Sprite X low */
    outb(EXTDATA, x & 0xFF);
    outb(EXTINDEX, 0x51); /* Sprite X high */
    outb(EXTDATA, x >> 8);
    outb(EXTINDEX, 0x53); /* Sprite Y low */
    outb(EXTDATA, y & 0xFF);
    outb(EXTINDEX, 0x54);  /* Sprite Y high */
    outb(EXTDATA, y >> 8);
}

/*
 * This is a local function that programs the colors of the cursor
 * on the graphics chip.
 * Adapted from accel/s3/s3Cursor.c.
 */
static void
IGSRecolorCursor(pScr, pCurs, displayed)
    ScreenPtr pScr;
    CursorPtr pCurs;
    Bool displayed;
{
    unsigned char temp;
    
#ifdef DEBUG
    ErrorF("RecolorCursor: fb: %X fg %X fr:%X\n bb %X bg %X br %X\n",
	   pCurs->foreBlue, pCurs->foreGreen, pCurs->foreRed,
	   pCurs->backBlue, pCurs->backGreen, pCurs->backRed);
#endif
    
    if (!xf86VTSema)
	return;
    
    /* Enable access to cursor colour registers */
    outb(EXTINDEX, 0x56);  /* Sprite control */
    temp = inb(EXTDATA);
    outb(EXTDATA, temp | 0x04);

    /* 
     * Write the new colours to the extended VGA palette. Palette
     * index is incremented after each write, so only write index
     * once 
     */
    outb(RAMDACINDEXW, 0);
    outb(RAMDACDATA, (pCurs->backRed  >> 8) & 0xFF);
    outb(RAMDACDATA, (pCurs->backGreen  >> 8) & 0xFF);
    outb(RAMDACDATA, (pCurs->backBlue  >> 8) & 0xFF);
    outb(RAMDACINDEXW, 1);
    outb(RAMDACDATA, (pCurs->foreRed  >> 8) & 0xFF);
    outb(RAMDACDATA, (pCurs->foreGreen  >> 8) & 0xFF);
    outb(RAMDACDATA, (pCurs->foreBlue >> 8)  & 0xFF);
  
    /* Restore sprite control register */
    outb(EXTDATA, temp);
}

void
IGSWarpCursor(pScr, x, y)
    ScreenPtr pScr;
    int x, y;
{
    miPointerWarpCursor(pScr, x, y);
    xf86Info.currentScreen = pScr;
}

void
IGSQueryBestSize(class, pwidth, pheight, pScreen)
    int class;
    short *pwidth;
    short *pheight;
    ScreenPtr pScreen;
{
    if (*pwidth > 0) {
	switch (class) {
	case CursorShape:
	    if (*pwidth > igsCursorWidth)
		*pwidth = igsCursorWidth;
	    if (*pheight > igsCursorHeight)
		*pheight = igsCursorHeight;
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

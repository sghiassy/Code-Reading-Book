/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/et4000/tseng_cursor.c,v 1.6.2.4 1997/05/31 13:34:43 dawes Exp $ */

/*
 * Hardware cursor handling. Adapted mainly from apm/apm_cursor.c
 * and whatever piece of code in XFree86 that could provide me with
 * more usefull information.
 * 
 * '97 Dejan Ilic <svedja@lysator.liu.se>
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

#include "tseng.h"

extern Bool vgaUseLinearAddressing;

static Bool TsengRealizeCursor();
static Bool TsengUnrealizeCursor();
static void TsengSetCursor();
static void TsengMoveCursor();
static void TsengRecolorCursor();

static miPointerSpriteFuncRec tsengPointerSpriteFuncs =
{
   TsengRealizeCursor,
   TsengUnrealizeCursor,
   TsengSetCursor,
   TsengMoveCursor,
};

/* vga256 interface defines Init, Restore, Warp, QueryBestSize. */


extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern xf86InfoRec xf86Info;

static int              tsengCursorGeneration = -1;
static unsigned char    tsengCursorControlMode;
int                     tsengCursorAddress; /* initialized in the Probe */

/*
 * This is the set variables that defines the cursor state within the
 * driver.
 */

int tsengCursorHotX;
int tsengCursorHotY;
int tsengCursorOriginX; /* The offset into the sprite of the curren cursor. */
int tsengCursorOriginY;
int tsengCursorWidth;	/* Must be set before calling TsengCursorInit. */
int tsengCursorHeight;
static CursorPtr tsengCursorpCurs;

/*
 * This is a high-level init function, called once at
 * startup and once every time the server VC is reentered;
 * it passes a local miPointerSpriteFuncRec with
 * additional functions that we need to provide.
 * It is called by the SVGA server.
 */

Bool TsengCursorInit(pm, pScr)
	char *pm;
	ScreenPtr pScr;
{
	if (tsengCursorGeneration != serverGeneration) {
		if (!(miPointerInitialize(pScr, &tsengPointerSpriteFuncs,
		&xf86PointerScreenFuncs, FALSE)))
			return FALSE;

		tsengCursorHotX = 0;
		tsengCursorHotY = 0;
		tsengCursorOriginX = 0;
		tsengCursorOriginY = 0;
	}

	tsengCursorGeneration = serverGeneration;

	tsengCursorControlMode = 0;

	return TRUE;
}

/*
 * This enables displaying of the cursor by the TSENG graphics chip.
 * It's a local function, it's not called from outside of the module.
 */

static void TsengShowCursor() {
	unsigned char tmp;

	/* Enable the hardware cursor. */
	if (et4000_type >= TYPE_ET6000) {
	    tmp = inb(ET6Kbase+0x46);
	    outb(ET6Kbase+0x46, (tmp | 0x01));
	}
	else {
	    outb(0x217A, 0xF7);
	    tmp = inb(0x217B);
	    outb(0x217B, tmp | 0x80);
	}
}

/*
 * This disables displaying of the cursor by the TSENG graphics chip.
 * This is also a local function, it's not called from outside.
 */

void TsengHideCursor() {
	unsigned char tmp;

	/* Disable the hardware cursor. */
	if (et4000_type >= TYPE_ET6000) {
	    tmp = inb(ET6Kbase+0x46);
	    outb(ET6Kbase+0x46, (tmp & 0xfe));;
	}
	else {
	    outb(0x217A, 0xF7);
	    tmp = inb(0x217B);
	    outb(0x217B, tmp & ~0x80);
	}
}

/*
 * This function is called when a new cursor image is requested by
 * the server. The main thing to do is convert the bitwise image
 * provided by the server into a format that the graphics card
 * can conveniently handle, and store that in system memory.
 */

static Bool TsengRealizeCursor(pScr, pCurs)
	ScreenPtr pScr;
	CursorPtr pCurs;
{
   register int i, j, b;
   unsigned char *pServMsk;
   unsigned char *pServSrc;
   int   index = pScr->myNum;
   pointer *pPriv = &pCurs->bits->devPriv[index];
   int   w, h, wsrc;
   unsigned char *ram, *dst, v;
   CursorBitsPtr bits = pCurs->bits;

   if (pCurs->bits->refcnt > 1)
      return TRUE;

   /* ram needed = H*W*2bits/bytesize */
   ram = (unsigned char *)xalloc(tsengCursorHeight * tsengCursorWidth * 2 / 8);
   *pPriv = (pointer) ram;

   if (!ram)
      return FALSE;

   /* set to transparent colour*/
   memset(ram, 0xaa, tsengCursorHeight * tsengCursorWidth * 2 / 8);

        /*
          There are two bitmaps for the X cursor:  the Source and
          the Mask.  The following table decodes these bits:
          
          Src  Mask  |  Cursor color  |  Plane 0  Plane 1
          -----------+----------------+------------------
           0    0    |   Transparent  |     0        0
           0    1    |     Color 0    |     0        1
           1    0    |   Transparent  |     0        0
           1    1    |     Color 1    |     1        1
          
          Thus, the data for Plane 0 bits is Src AND Mask, and
          the data for Plane 1 bits is Mask.

          On the Tseng ET6000 the bits are in different order
                 00 = colour 0
                 01 = colour 1
                 10 = transparent (allow CRTC pass thru)
                 11 = invert (Display regular pixel data inverted)
                      Invert is actualy not used here.

          As a result, for Tseng the plane data changes:
              plane 0 = ~Src & Msk
              plane 1 = ~Msk
          
          The bit order on ET6000 cursor image:
          -------------------------------------
          bit number        7 6 5 4 3 2 1 0
          pixel number      3 3 2 2 1 1 0 0
          bit significance  1 0 1 0 1 0 1 0

          */

   pServSrc = (unsigned char *)bits->source;
   pServMsk = (unsigned char *)bits->mask;

   h = pCurs->bits->height;
   if (h > tsengCursorHeight) h = tsengCursorHeight;
   w = pCurs->bits->width;
   if (w > tsengCursorWidth) w = tsengCursorWidth;

   wsrc = PixmapBytePad(bits->width, 1); /* Determine padding, in Bytes per line. */
   

   /*
    * Since cursor data is at least byte-padded, and Tseng hardware cursor
    * is easier to handle with 4-pixel padding, we'll use that padding to
    * get 8-pixel padding, which avoids us from having to do some
    * byte-boundary magic in the cursor storage code. We also make w a width
    * in bytes, which is easier further on.
    */
   w = (w+7) / 8;

   /*
    * ram is first offset to move the cursor to the lower right
    * portion of the sprite, as shown in the Tseng databook. -HNH
    */
   ram += (tsengCursorHeight-h)*16 + (tsengCursorWidth/8 - w)*2;

#if 0
   ErrorF("w = %d ; h = %d\n", w, h);
   for (i = 0; i < h ; i++) {
     for (j = 0; j < w; j++) {
       int offset = i * wsrc + j;
       for (b = 0; b < 8; b++)
         ErrorF("%d", ( ((pServMsk[offset]) >> b) & 1)
                    | ( (((pServSrc[offset]) >> b) & 1) << 1));
     }
     ErrorF("\n");
   }
#endif


   for (i = 0; i < h; i++, ram+=16) {  /* each scanline of cursor data */
     for (j = 0; j < w; j++) {         /* each byte of cursor data */
        unsigned char m, s, b0, b1;

        int offset = i * wsrc + j;

        m = pServMsk[offset];
        s = pServSrc[offset];

        b0 = ~s & m;
        b1 = ~m;
        
        /*
         * Now b0 contains 8 bits "bit 0" and b1 8 bits "bit 1" of the
         * cursor data. All we need to do now is interleave them.
         */
         
        ram[j*2] =
            ((b0 & 0x01)     ) | ((b1 & 0x01) << 1) |
            ((b0 & 0x02) << 1) | ((b1 & 0x02) << 2) |
            ((b0 & 0x04) << 2) | ((b1 & 0x04) << 3) |
            ((b0 & 0x08) << 3) | ((b1 & 0x08) << 4) ;
        
        ram[(j*2)+1] =
            ((b0 & 0x10) >> 4) | ((b1 & 0x10) >> 3) |
            ((b0 & 0x20) >> 3) | ((b1 & 0x20) >> 2) |
            ((b0 & 0x40) >> 2) | ((b1 & 0x40) >> 1) |
            ((b0 & 0x80) >> 1) | ((b1 & 0x80)     ) ;
     }
   }

 return TRUE;
}

/*
 * This is called when a cursor is no longer used. The intermediate
 * cursor image storage that we created needs to be deallocated.
 */

static Bool TsengUnrealizeCursor(pScr, pCurs)
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

extern void ET4000SetWrite();

static void TsengLoadCursorToCard(pScr, pCurs, x, y)
	ScreenPtr pScr;
	CursorPtr pCurs;
	int x, y;	/* Not used for TSENG. */
{
	unsigned char *cursor_image;
	int index = pScr->myNum;

	if (!xf86VTSema)
		return;

	cursor_image = pCurs->bits->devPriv[index];

	if (vgaUseLinearAddressing)
		memcpy((unsigned char *)vgaLinearBase + tsengCursorAddress,
			cursor_image, 1024);
	else {
		/*
		 * The cursor can only be in the last 16K of video memory,
		 * which fits in the last banking window.
		 */
		vgaSaveBank();
		vgaSetVidPage(tsengCursorAddress >> 16);
		memcpy((unsigned char *)vgaBase + (tsengCursorAddress & 0xFFFF),
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

static void TsengLoadCursor(pScr, pCurs, x, y)
	ScreenPtr pScr;
	CursorPtr pCurs;
	int x, y;
{
        unsigned char tmp;

	if (!xf86VTSema)
		return;

	if (!pCurs)
		return;

	/* Remember the cursor currently loaded into this cursor slot. */
	tsengCursorpCurs = pCurs;

	TsengHideCursor();

	/* Program the cursor image address in video memory. */
        /* The adress is given in doublewords */
	if (et4000_type >= TYPE_ET6000) {
            /* bits 19:16 */
            outb(vgaIOBase + 0x04, 0x0E);
            tmp = inb(vgaIOBase + 0x05) & 0xF0;
            outb(vgaIOBase + 0x05, tmp | (((tsengCursorAddress/4) >> 16) & 0x0F));
            /* bits 15:8 */
            outb(vgaIOBase + 0x04, 0x0F);
            outb(vgaIOBase + 0x05, ((tsengCursorAddress/4) >> 8) & 0xFF);
	    /* on the ET6000, bits (7:0) are always 0 */
        }
        else {
            /* bits 19:16 */
            outb(0x217A, 0xEA);
            tmp = inb(0x217B) & 0xF0;
            outb(0x217B, tmp | (((tsengCursorAddress/4) >> 16) & 0x0F));
            /* bits 15:8 */
            outb(0x217A, 0xE9);
            outb(0x217B, ((tsengCursorAddress/4) >> 8) & 0xFF);
            /* bits 7:0 */
            outb(0x217A, 0xE8);
            outb(0x217B, (tsengCursorAddress/4) & 0xFF);

            /* this needs to be set for the sprite */
            outb(0x217A, 0xEB); outb(0x217B, 2);
            outb(0x217A, 0xEC); tmp = inb(0x217B); outb(0x217B, tmp & 0xFE);
            outb(0x217A, 0xEF); tmp = inb(0x217B); outb(0x217B, (tmp & 0xF8) | 0x02);
            outb(0x217A, 0xEE); outb(0x217B, 1);
        }

	TsengLoadCursorToCard(pScr, pCurs, x, y);

	TsengRecolorCursor(pScr, pCurs, 1);

	/* Position cursor */
	TsengMoveCursor(pScr, x, y);

	/* Turn it on. */
	TsengShowCursor();
}

/*
 * This function should display a new cursor at a new position.
 */

static void TsengSetCursor(pScr, pCurs, x, y, generateEvent)
	ScreenPtr pScr;
	CursorPtr pCurs;
	int x, y;
	Bool generateEvent;
{
	if (!pCurs)
		return;

	tsengCursorHotX = pCurs->bits->xhot;
	tsengCursorHotY = pCurs->bits->yhot;
	tsengCursorOriginX = (tsengCursorWidth - pCurs->bits->width) & ~7; /* byte aligned */
	tsengCursorOriginY = (tsengCursorHeight - pCurs->bits->height);

	TsengLoadCursor(pScr, pCurs, x, y);
}

/*
 * This function should redisplay a cursor that has been
 * displayed earlier. It is called by the SVGA server.
 */

void TsengRestoreCursor(pScr)
	ScreenPtr pScr;
{
	int x, y;

	miPointerPosition(&x, &y);

	TsengLoadCursor(pScr, tsengCursorpCurs, x, y);
}

/*
 * This function is called when the current cursor is moved. It makes
 * the graphic chip display the cursor at the new position.
 */

static void TsengMoveCursor(pScr, x, y)
	ScreenPtr pScr;
	int x, y;
{
	int xorigin, yorigin;

	if (!xf86VTSema)
		return;

	x -= vga256InfoRec.frameX0 + tsengCursorHotX;
	y -= vga256InfoRec.frameY0 + tsengCursorHotY;

	/* The default origin is calculated in TsengSetCursor */
	xorigin = tsengCursorOriginX;
	yorigin = tsengCursorOriginY;

	/*
	 * If the cursor is partly out of screen at the left or top,
	 * we need to modify the origin.
	 */
	if (x < 0) {
		xorigin += -x;
		x = 0;
	}
	if (y < 0) {
		yorigin += -y;
		y = 0;
	}

	/* Correct cursor position in DoubleScan modes */
	if (XF86SCRNINFO(pScr)->modes->Flags & V_DBLSCAN)
		y *= 2;

	/* Program the cursor origin (offset into the cursor bitmap). */
	/* Program the new cursor position. */
	if (et4000_type >= TYPE_ET6000) {
	    outb (ET6Kbase + 0x82, xorigin);
	    outb (ET6Kbase + 0x83, yorigin);

	    outb (ET6Kbase + 0x84, (x & 0xff));        /* X bits 7-0 */
            outb (ET6Kbase + 0x85, ((x >> 8) & 0x0f)); /* X bits 11-8 */

            outb (ET6Kbase + 0x86, (y & 0xff));        /* Y bits 7-0 */
            outb (ET6Kbase + 0x87, ((y >> 8) & 0x0f)); /* Y bits 11-8 */
        }
        else {
	    outb(0x217A, 0xE2); outb (0x217B, xorigin);
	    outb(0x217A, 0xE6); outb (0x217B, yorigin);

	    outb(0x217A, 0xE0); outb (0x217B, (x & 0xff));        /* X bits 7-0 */
	    outb(0x217A, 0xE1); outb (0x217B, ((x >> 8) & 0x0f)); /* X bits 10-8 */

	    outb(0x217A, 0xE4); outb (0x217B, (y & 0xff));        /* Y bits 7-0 */
	    outb(0x217A, 0xE5); outb (0x217B, ((y >> 8) & 0x0f)); /* Y bits 10-8 */
        }
}


/*
 * Check if ET6000 can generate requested color, and return 2 color bits for
 * in the ET6000's cursor color storage. Since the color is a short, this
 * code is probably not portable to architectures with a different idea
 * about shorts. On most systems, it is 16 bits.
 */

static unsigned char get_et6000_color_bits(unsigned short col_in, int* badColour)
{
#ifdef ONLY_PERFECT_HWCURSOR_COLORS
    switch (col_in >> 12) {   /* extract top four bits */
    case 0x0: /* bit combination "0000" */
    case 0x5: /* bit combination "0101" */
    case 0xa: /* bit combination "1010" */
    case 0xf: /* bit combination "1111" */
      return (col_in & 0xc000) >> 14; /* return bits for ET6000 */
      break;

    default: /* not a valid colour */
      *badColour = 1;
    }
    return 0;
#else
    return col_in >> 14; /* return 2 MSbits for closest matching color */
#endif
}


/*
 * This is a local function that programs the colors of the cursor
 * on the graphics chip.
 * Adapted from accel/s3/s3Cursor.c.
 */

static void TsengRecolorCursor(pScr, pCurs, displayed)
	ScreenPtr pScr;
	CursorPtr pCurs;
	Bool displayed;
{
  /*
    About the precision of color representation in a hardware cursor:
  
    The ET6000 RAMDAC is only 6 bits per color component in 8bpp mode, so in
    that case you can only be precise up to 6 bits per color. Thus,
    "01010101", "01010100", "01010110", and "01010111" will show EXACTLY the
    same color.
    
    In 15 or 16bpp mode, only 5 bits per color are significant, so there is no
    need to drop back to SW cursor mode when the 5 MSBits of the requested
    cursor color are correct.
    
    In addition, high-bit-depth color differences are only visible on _large_
    planes of equal color. i.e. small areas of a certain color (like a cursor)
    don't need many bits per pixel at all, because the difference will not be seen.
    
    I would guess that 4 significant bits per color component in all cases
    should be enough for a hardware cursor, in all modes.
    
    If we make the HW cursor behave like this, people can always use the
    "sw_cursor" option if they don't like this lack of precision.
    */

        ColormapPtr pmap;
        Bool badColour;
        int fgColour, bgColour;
        xColorItem sourceColor, maskColor;

	if (!xf86VTSema)
		return;

        badColour = 0; 
        fgColour = 0; bgColour = 0;

        if (et4000_type >= TYPE_ET6000) {
          /* Extract foreground Cursor Colour, put in position bits 5 and 4 */
          fgColour = get_et6000_color_bits(pCurs->foreRed,   &badColour) << 4
                   | get_et6000_color_bits(pCurs->foreGreen, &badColour) << 2
                   | get_et6000_color_bits(pCurs->foreBlue,  &badColour)     ;

          /* Extract background Cursor Colour */
          bgColour = get_et6000_color_bits(pCurs->backRed,   &badColour) << 4
                   | get_et6000_color_bits(pCurs->backGreen, &badColour) << 2
                   | get_et6000_color_bits(pCurs->backBlue,  &badColour)     ;

          if (!badColour){
            outb (ET6Kbase + 0x67, 0x09); /* prepare for colour data */
            outb (ET6Kbase + 0x69, fgColour);
            outb (ET6Kbase + 0x69, bgColour);
          }
        }
        else {  /* ET4000 */
          /*
           * The ET4000 uses color 0 as color "0", and color 0xFF as color
           * "1". Changing colors implies changing colors 0 and 255. It
           * doesn't seem to work at all in any non-8bpp modes (you get TWO
           * cursor images...). For the HW cursor to work all, we need to
           * allocate color #255 for color 1, which will then also allow us
           * to change it to any desired color. We could do this with color
           * 0 as well, so that we have full color control for both color 0
           * and 1, but that would mean claiming color 0 as well, and I
           * think color 0 and 1 are fixed at black and white or the
           * opposite (depending on the -flipPixels server flag).
           */

          /* get palette color 0, and see if it matches the HW cursor requirements */
          if (pCurs->foreRed == pCurs->foreGreen == pCurs->foreBlue == 0) {
          }
          
          /*
           * Allocate color #255 for the HW cursor, and set it to the correct color.
           */
           if (vgaBitsPerPixel == 8) {
                vgaGetInstalledColormaps(pScr, &pmap);
                
#if WHEN_THIS_GETS_FIXED
                /* ARK code. assumes we can use any color index */
                /* on ET4000W32, we need index 255. HOW? */
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
#endif
            }
            /* if not 8bpp, we can't change the color: it's always B&W */
        }

        if (badColour) {
            /*
             * At this point we should be switching to a SW cursor instead
             * -- if that is possible at this stage. If not, we should
             * remove this message, and just document the limitation.
             */
#ifdef HW_CURSOR_REPORT_BAD_COLOR
            ErrorF ("-- Bad Cursor Colour tried\n");
#endif
        }
}


/*
 * This doesn't do very much. It just calls the mi routine. It is called
 * by the SVGA server.
 */

void TsengWarpCursor(pScr, x, y)
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

void TsengQueryBestSize(class, pwidth, pheight, pScreen)
	int class;
	unsigned short *pwidth;
	unsigned short *pheight;
	ScreenPtr pScreen;
{
 	if (*pwidth > 0) {
 		if (class == CursorShape) {
                  *pwidth = tsengCursorWidth;
                  *pheight = tsengCursorHeight;
		}
		else
			(void) mfbQueryBestSize(class, pwidth, pheight, pScreen);
	}
}

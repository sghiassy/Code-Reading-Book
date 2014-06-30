/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/mga/mga_hwcurs.c,v 1.1.2.2 1997/05/31 13:34:44 dawes Exp $ */
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
 * Modified for MGA Millennium by Xavier Ducoin <xavier@rd.lectra.fr>
 *
 */


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
#include "mipointer.h"

#include "vga.h"
#include "vgaPCI.h"

#include "mga.h"

/*
 * exported functions
 */
Bool MGAHwCursorInit();

/*
 * implementation
 */
 
static int MGACursorHotX;
static int MGACursorHotY;
static int MGACursorGeneration = -1;
static CursorPtr MGACursorpCurs;

static Bool MGARealizeCursor();
static Bool MGAUnrealizeCursor();
static void MGASetCursor();
static void MGAMoveCursor();
static void MGALoadCursor();

extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern xf86InfoRec xf86Info;
extern vgaHWCursorRec vgaHWCursor;

static miPointerSpriteFuncRec MGAPointerSpriteFuncs =
{
    MGARealizeCursor,
    MGAUnrealizeCursor,
    MGASetCursor,
    MGAMoveCursor,
};

static Bool
MGARealizeCursor(pScr, pCurs)
    ScreenPtr pScr;
    CursorPtr pCurs;
{
    register int i, j;
    int   index = pScr->myNum;
    pointer *pPriv = &pCurs->bits->devPriv[index];
    int   wsrc, h;
    CursorBitsPtr bits = pCurs->bits;

    if (pCurs->bits->refcnt > 1)
        return TRUE;

    h = bits->height;
    wsrc = PixmapBytePad(bits->width, 1);	/* bytes per line */

    *pPriv = MGAdac.RealizeCursor(bits->source, bits->mask, wsrc, h);
    if (!*pPriv)
        return FALSE;
       
    return TRUE;
}

static Bool
MGAUnrealizeCursor(pScr, pCurs)
    ScreenPtr pScr;
    CursorPtr pCurs;
{
    pointer priv;

    if (pCurs->bits->refcnt <= 1 &&
            (priv = pCurs->bits->devPriv[pScr->myNum]))
        xfree(priv);
    return TRUE;
}

/*
 * This function should display a new cursor at a new position.
 */

static void 
MGASetCursor(pScr, pCurs, x, y, generateEvent)
    ScreenPtr pScr;
    CursorPtr pCurs;
    int x, y;
    Bool generateEvent;
{

    if (!pCurs)
        return;
    
    MGACursorHotX = pCurs->bits->xhot;
    MGACursorHotY = pCurs->bits->yhot;
    
    MGALoadCursor(pScr, pCurs, x, y);
}

static void
MGAMoveCursor(pScr, x, y)
    ScreenPtr pScr;
    int   x, y;
{
    if (!xf86VTSema)
        return;
   
    x -= vga256InfoRec.frameX0 + MGACursorHotX;
    y -= vga256InfoRec.frameY0 + MGACursorHotY;
   
    MGAdac.MoveCursor(x, y);
}

static void
MGARecolorCursor(pScr, pCurs, displayed)
    ScreenPtr pScr;
    CursorPtr pCurs;
    Bool displayed;
{
    if (!displayed)
        return;
       
    MGAdac.RecolorCursor(pCurs->backRed, pCurs->backGreen, pCurs->backBlue,
                         pCurs->foreRed, pCurs->foreGreen, pCurs->foreBlue);
}

static void 
MGALoadCursor(pScr, pCurs, x, y)
    ScreenPtr pScr;
    CursorPtr pCurs;
    int x, y;
{
    int   index = pScr->myNum;
    register int   i;
    unsigned char *ram, *p, tmp;

    if (!xf86VTSema)
        return;

    if (!pCurs)
        return;

    /* Remember the cursor currently loaded into this cursor slot. */
    MGACursorpCurs = pCurs;
   
    /* turn the cursor off */
    MGAdac.CursorOff();

    /* output the cursor data */
    MGAdac.LoadCursor(pCurs->bits->devPriv[index]);

    /* load colormap */
    MGARecolorCursor(pScr, pCurs, TRUE);

    /* position cursor */
    MGAMoveCursor(0, x, y);

    /* turn the cursor on */
    MGAdac.CursorOn();
}


/*
 * This is a high-level init function, called once; it passes a local
 * miPointerSpriteFuncRec with additional functions that we need to provide.
 * It is called by the SVGA server.
 */
static Bool
MGACursorInit(pm, pScr)
    char *pm;
    ScreenPtr pScr;
{
    if (MGACursorGeneration != serverGeneration) {
	if (!(miPointerInitialize(pScr, &MGAPointerSpriteFuncs,
				 &xf86PointerScreenFuncs, FALSE)))
	    return FALSE;
       
	MGACursorHotX = 0;
	MGACursorHotY = 0;
	pScr->RecolorCursor = MGARecolorCursor;
	MGACursorGeneration = serverGeneration;
    }
    return TRUE;
}

/*
 * This function should redisplay a cursor that has been
 * displayed earlier. It is called by the SVGA server.
 */
static void
MGARestoreCursor(pScr)
    ScreenPtr pScr;
{
    int x, y;

    miPointerPosition(&x, &y);

    MGALoadCursor(pScr, MGACursorpCurs, x, y);
}

/*
 * This doesn't do very much. It just calls the mi routine. It is called
 * by the SVGA server.
 */
static void
MGAWarpCursor(pScr, x, y)
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
static void 
MGAQueryBestSize(class, pwidth, pheight, pScreen)
    int class;
    unsigned short *pwidth;
    unsigned short *pheight;
    ScreenPtr pScreen;
{
    if (*pwidth > 0) {
        if (class == CursorShape)
            MGAdac.QueryCursorSize(pwidth, pheight);
        else
            (void) mfbQueryBestSize(class, pwidth, pheight, pScreen);
    }
}

/*
 * This is top-level initialization funtion called from mga_driver
 */
Bool MGAHwCursorInit()
{
    if (MGAdac.isHwCursor) {
        vgaHWCursor.Initialized = TRUE;
        vgaHWCursor.Init = MGACursorInit;
        vgaHWCursor.Restore = MGARestoreCursor;
        vgaHWCursor.Warp = MGAWarpCursor;
        vgaHWCursor.QueryBestSize = MGAQueryBestSize;
        return TRUE;
    }
    return FALSE;
}

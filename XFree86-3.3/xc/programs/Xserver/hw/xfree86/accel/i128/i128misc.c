/* $XConsortium: i128misc.c /main/2 1996/02/21 17:23:21 kaleb $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Thomas Roell makes no
 * representations about the suitability of this software for any purpose. It
 * is provided "as is" without express or implied warranty.
 * 
 * THOMAS ROELL AND KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL THOMAS ROELL OR KEVIN E. MARTIN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 * 
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the I128 by Robin Cutshaw (robin@XFree86.Org)
 * 
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/i128/i128misc.c,v 3.5.2.1 1997/05/22 14:00:37 dawes Exp $ */

#include "servermd.h"

#include "cfb.h"
#include "cfb16.h"
#include "cfb32.h"
#include "pixmapstr.h"
#include "fontstruct.h"
#include "i128.h"
#include "i128reg.h"
#include "xf86_HWlib.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "xf86scrin.h"

extern miPointerScreenFuncRec xf86PointerScreenFuncs;

extern ScreenPtr i128savepScreen;
static PixmapPtr ppix = NULL;
extern int i128DisplayWidth;
extern pointer vgaBase;
extern pointer i128VideoMem;
extern struct i128mem i128mem;

extern Bool xf86Exiting, xf86Resetting, xf86ProbeFailed, xf86Verbose;

static Bool AlreadyInited = FALSE;
static Bool i128ModeSwitched = FALSE;


/*
 * i128Initialize -- Attempt to find and initialize a VGA framebuffer Most of
 * the elements of the ScreenRec are filled in.  The video is enabled for the
 * frame buffer...
 */

Bool
i128Initialize(scr_index, pScreen, argc, argv)
     int   scr_index;		/* The index of pScreen in the ScreenInfo */
     ScreenPtr pScreen;		/* The Screen to initialize */
     int   argc;		/* The number of the Server's arguments. */
     char **argv;		/* The arguments themselves. Don't change! */
{
   int displayResolution = 75;	/* default to 75dpi */
   extern int monitorResolution;
   Bool (*ScreenInitFunc)(register ScreenPtr, pointer, int, int, int, int, int);

 /*
  * Initialize the screen, saving the original state for Save/Restore
  * function
  */

   i128Init(i128InfoRec.modes);

   i128InitEnvironment();
   AlreadyInited = TRUE;

   i128ImageInit();
   xf86InitCache(i128CacheMoveBlock);
   i128FontCache8Init();

   /*
    * Take display resolution from the -dpi flag if specified
    */

   if (monitorResolution)
      displayResolution = monitorResolution;

   if (OFLG_ISSET(OPTION_NOACCEL, &i128InfoRec.options))
	ScreenInitFunc = &i128ScreenInit;
   else {
	i128AccelInit();
        if (i128InfoRec.bitsPerPixel == 8)
		ScreenInitFunc = &xf86XAAScreenInit8bpp;
        else if (i128InfoRec.bitsPerPixel == 16)
		ScreenInitFunc = &xf86XAAScreenInit16bpp;
	else
		ScreenInitFunc = &xf86XAAScreenInit32bpp;
   }

   if (!ScreenInitFunc(pScreen,
	     (pointer) i128VideoMem,
	     i128InfoRec.virtualX, i128InfoRec.virtualY,
	     displayResolution, displayResolution,
	     i128DisplayWidth))
	return (FALSE);

   pScreen->CloseScreen = i128CloseScreen;
   pScreen->SaveScreen = i128SaveScreen;


   switch (i128InfoRec.bitsPerPixel) {
      case 8:
         pScreen->InstallColormap = i128InstallColormap;
         pScreen->UninstallColormap = i128UninstallColormap;
         pScreen->ListInstalledColormaps = i128ListInstalledColormaps;
         pScreen->StoreColors = i128StoreColors;
         break;       
      case 16:
      case 32:
         pScreen->InstallColormap = cfbInstallColormap;
         pScreen->UninstallColormap = cfbUninstallColormap;
         pScreen->ListInstalledColormaps = cfbListInstalledColormaps;
         pScreen->StoreColors = (void (*)())NoopDDA;
   }
   pScreen->QueryBestSize = i128QueryBestSize;
   xf86PointerScreenFuncs.WarpCursor = i128WarpCursor;
   (void)i128CursorInit(0, pScreen);

   i128savepScreen = pScreen;
   return (cfbCreateDefColormap(pScreen));

}


/*
 * i128EnterLeaveVT -- grab/ungrab the current VT completely.
 */

void
i128EnterLeaveVT(enter, screen_idx)
     Bool  enter;
     int screen_idx;
{
   PixmapPtr pspix;
   ScreenPtr pScreen = i128savepScreen;

   if (!xf86Exiting && !xf86Resetting) {
      /* cfbGetScreenPixmap(pScreen) */
        switch (i128InfoRec.bitsPerPixel) {
        case 8:
            pspix = (PixmapPtr)pScreen->devPrivate;
            break;
        case 16:
            pspix =
                  (PixmapPtr)pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr;
            break;
        case 32:
            pspix =
                  (PixmapPtr)pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr;
            break;
	}
   }

   if (enter) {
      if (!xf86Resetting) {
	 ScrnInfoPtr pScr = XF86SCRNINFO(pScreen);

         i128Init(i128InfoRec.modes);
         i128InitEnvironment();
         AlreadyInited = TRUE;
	 i128RestoreDACvalues();
	 i128ImageInit();
   	 i128FontCache8Init();
	 i128RestoreColor0(pScreen);
	 (void)i128CursorInit(0, pScreen); 
	 i128RestoreCursor(pScreen);
	 i128AdjustFrame(pScr->frameX0, pScr->frameY0);

	 if ((pointer)pspix->devPrivate.ptr != i128VideoMem && ppix) {
	    pspix->devPrivate.ptr = i128VideoMem;
#ifdef WORKWORKWORK
	    (*i128ImageWriteFunc)(0, 0, pScreen->width, pScreen->height,
			        ppix->devPrivate.ptr,
			        PixmapBytePad(i128DisplayWidth, 
					      pScreen->rootDepth),
			        0, 0, i128alu[GXcopy], ~0);
#endif
	 }
      }
      if (ppix) {
	 (pScreen->DestroyPixmap)(ppix);
	 ppix = NULL;
      }
   } else {
      if (!xf86Exiting) {
	 ppix = (pScreen->CreatePixmap)(pScreen, i128DisplayWidth,
					pScreen->height, pScreen->rootDepth);
	 if (ppix) {
#ifdef WORKWORKWORK
	    (*i128ImageReadFunc)(0, 0, pScreen->width, pScreen->height,
			       ppix->devPrivate.ptr,
			       PixmapBytePad(i128DisplayWidth, 
					     pScreen->rootDepth),
			       0, 0, ~0);
#endif
	    pspix->devPrivate.ptr = ppix->devPrivate.ptr;
	 }
      }

      xf86InvalidatePixmapCache();

      if (AlreadyInited) {
	  i128CleanUp();
	  AlreadyInited = FALSE;
      }
   }
}

/*
 * i128CloseScreen -- called to ensure video is enabled when server exits.
 */

Bool
i128CloseScreen(screen_idx, pScreen)
     int screen_idx;
     ScreenPtr pScreen;
{

 /*
  * Hmm... The server may shut down even if it is not running on the current
  * vt. Let's catch this case here.
  */
   xf86Exiting = TRUE;
   if (xf86VTSema)
      i128EnterLeaveVT(LEAVE, screen_idx);
   else if (ppix) {
      /*
       * 7-Jan-94 CEG: The server is not running on the current vt.
       * Free the screen snapshot taken when the server vt was left.
       */
      (i128savepScreen->DestroyPixmap)(ppix);
      ppix = NULL;
   }
   return (TRUE);
}

/*
 * i128SaveScreen -- blank the screen.
 */

Bool
i128SaveScreen(pScreen, on)
     ScreenPtr pScreen;
     Bool  on;
{
   unsigned char scrn, sync;

   if (on)
      SetTimeSinceLastInputEvent();

   if (xf86VTSema) {
      /* the server is running on the current vt */
      /* so just go for it */

      if (on)
         i128mem.rbase_g[0x58/4] |= 0x40;
      else
         i128mem.rbase_g[0x58/4] &= ~0x40;
   }
   return (TRUE);
}

static debugcache = 0;

Bool
i128SwitchMode(mode)
     DisplayModePtr mode;
{
#ifdef WORKWORKWORK
  WaitIdle();  /* wait till idle */
#endif

   if (OFLG_ISSET(OPTION_SHOWCACHE, &i128InfoRec.options)) {
      debugcache++;
      if (debugcache & 1) {
        mode = mode->prev;
        i128InfoRec.modes=mode;
      }
   }

   if (i128Init(mode)) {
      i128ModeSwitched = TRUE;
      i128ImageInit(); /* mode switching changes the memory bank */
      i128RestoreCursor(i128savepScreen);
      return (TRUE);
   } else {
      ErrorF("Mode switch failed because of hardware initialisation error\n");
      i128ImageInit();
      return (FALSE);
   }
}

void
i128AdjustFrame(int x, int y)
{
   int   Base;
   unsigned char tmp;
   extern int i128HDisplay, i128AdjustCursorXPos;
#define I128_PAN_MASK 0x01FFFFE0

   if (OFLG_ISSET(OPTION_SHOWCACHE, &i128InfoRec.options)) {
      if ( debugcache & 1)
         y += 512;
   }

   if (x > i128DisplayWidth - i128HDisplay)
      x  = i128DisplayWidth - i128HDisplay;

   Base = ((y*i128DisplayWidth + x) * (i128InfoRec.bitsPerPixel/8));
   i128mem.rbase_g[DB_ADR] = Base & I128_PAN_MASK;

   /* now warp the cursor after the screen move */
   i128AdjustCursorXPos = Base - (Base & I128_PAN_MASK);

   if (i128ModeSwitched) {
      i128ModeSwitched = FALSE;
      i128RestoreCursor(i128savepScreen);
   } else {
      i128RepositionCursor(i128savepScreen);
   }
}
